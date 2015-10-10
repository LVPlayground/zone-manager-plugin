// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "zone_processor_client.h"

#include "player.h"
#include "zone_data_storage.h"
#include "zone_layer.h"

#include <algorithm>
#include <vector>

namespace {

inline bool PositionInArea(const Point& position, const Rectangle& area, float max_height) {
  if (position.x < area.x1 || position.x > area.x2)
    return false;

  if (position.y < area.y1 || position.y > area.y2)
    return false;

  return position.z < max_height;
}

bool ZonePositionComparator(const Zone* zone, float x) {
  return zone->Area().x2 < x;
}

} // namespace

int process_players_last_iteration = 0;
int process_players_last_iteration_time() {
  return process_players_last_iteration;
}

// client thread
void ZoneProcessorClient::ProcessPlayerActionMessageQueue() {
  auto scoped_player_map_ref = data_storage_->PlayerMap();
  auto& player_map = scoped_player_map_ref.get();

  std::vector<unsigned int> player_statuses_to_reset;

  PlayerActionMessage message;
  while (player_action_message_queue_.size() > 0) {
    player_action_message_queue_.pop(message);
    switch (message.action) {
      case TrackPlayerAction:
        player_map.insert(std::pair<unsigned int, Player*>(message.player_id, new Player(message.player_id)));
        break;
      case RemovePlayerAction: {
        auto iter = player_map.find(message.player_id);
        if (iter == player_map.end())
          break;

        player_statuses_to_reset.push_back(message.player_id);

        delete iter->second;
        player_map.erase(iter);
        break;
      }
      case ResetPlayersAction:
        for (std::pair<unsigned int, Player*> entry : player_map) {
          player_statuses_to_reset.push_back(entry.first);
          delete entry.second;
        }

        player_map.clear();
        break;
    }
  }

  if (player_statuses_to_reset.size() == 0)
    return;

  auto scoped_zone_layer_map_ref = data_storage_->ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();

  // Now proceed to resetting all mutated player's statuses. If we don't do this, the
  // OnPlayerLeaveZone() callback would be invoked after they connect to the server.
  for (std::pair<unsigned int, ZoneLayer*> layer_entry : zone_layer_map) {
    for (unsigned int player_id : player_statuses_to_reset)
      layer_entry.second->PlayerState().ResetZoneForPlayer(player_id);
  }
}

// client thread
void ZoneProcessorClient::ProcessPlayerLocalization() {
  // Note that we intentionally access the Player map without a scoped mutex here. The
  // reasoning is explained in the zone_data_storage.h header file.
  auto& player_map = data_storage_->player_map_;

  Point position;
  for (std::pair<unsigned int, Player*> player_entry : player_map) {
    if (player_entry.second->GetPositionIfInvalidated(position) == false)
      continue; // quick bail: the player's position hasn't been invalidated.
    
    auto scoped_zone_layer_map_ref = data_storage_->ZoneLayerMap();
    auto& zone_layer_map = scoped_zone_layer_map_ref.get();
    
    // Each layer needs to be processed independently and contains its own set of state
    // for each player, allowing us to keep track of that.
    for (std::pair<unsigned int, ZoneLayer*> layer_entry : zone_layer_map) {
      ZoneLayer* layer = layer_entry.second;
      
      const Zone* player_zone = layer->PlayerState().CurrentZoneForPlayer(player_entry.first);
      if (player_zone != nullptr) {
        if (PositionInArea(position, player_zone->Area(), player_zone->MaxHeight()))
          break; // quick bail: the player is still in their existing zone.

        layer->PlayerState().ResetZoneForPlayer(player_entry.first);

        // The player left this zone, so we need to send a message to the host thread
        // making sure the callback can be invoked on the server thread.
        PlayerZoneMutationMessage message;
        message.player_id = player_entry.first;
        message.layer_id = layer_entry.first;
        message.zone_id = player_zone->Id();
        message.script = static_cast<void*>(layer->OwningScript()->Runtime());

        player_leave_zone_message_queue_.push(message);
      }

      // The player is currently not in any zone. We'll try to locate the zone they're in using a
      // binary search on the layer, then iteratively proceeding until the zone is out of area.
      auto location = std::lower_bound(layer->Begin(), layer->End(), position.x, ZonePositionComparator);
      while (location != layer->End()) {
        const Zone* current_zone = *location;
        if (current_zone->Area().x1 > position.x)
          break;
        
        // If we found a zone where the player is located in, send a message to the server thread
        // telling them to send a notification to the gamemode about the player entering this zone.
        if (PositionInArea(position, current_zone->Area(), current_zone->MaxHeight())) {
          layer->PlayerState().SetZoneForPlayer(player_entry.first, current_zone);

          PlayerZoneMutationMessage message;
          message.player_id = player_entry.first;
          message.layer_id = layer_entry.first;
          message.zone_id = current_zone->Id();
          message.script = static_cast<void*>(layer->OwningScript()->Runtime());

          player_enter_zone_message_queue_.push(message);
          break;
        }

        ++location;
      }
    }
  }
}

// client thread
void ZoneProcessorClient::run() {
  static unsigned int process_players_counter = 0;
  while (!shutdown_requested()) {
    if (player_action_message_queue_.size() > 0)
      ProcessPlayerActionMessageQueue();

    if (++process_players_counter > 5) {
      int process_start_time = time();
      ProcessPlayerLocalization();

      process_players_last_iteration = timeSpan(process_start_time);
      process_players_counter = 0;
    }

    thread_sleep(25);
  }
}