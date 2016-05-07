// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "zone_manager.h"

#include "performance_test.h"
#include "player.h"
#include "point.h"
#include "rectangle.h"
#include "zone_layer.h"
#include "zone_messages.h"

ZoneManager::ZoneManager()
    : player_update_counter_(0)
    , status_update_counter_(0)
    , zone_processor_host_(zone_data_storage_) {
}

unsigned int ZoneManager::CreateLayer(Script* script, unsigned int layer_id) {
  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  scoped_zone_layer_map_ref.get().insert(
    std::pair<unsigned int, ZoneLayer*>(layer_id, new ZoneLayer(script)));

  return layer_id;
}

void ZoneManager::DestroyLayer(unsigned int layer_id) {
  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();

  auto iter = zone_layer_map.find(layer_id);
  if (iter == zone_layer_map.end())
    return;

  delete iter->second;
  zone_layer_map.erase(iter);
}

void ZoneManager::DestroyLayersForScript(Script* script) {
  if (script == nullptr)
    return; // quick bail: this script never did anything with zones.

  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();
  
  auto iter = zone_layer_map.begin();
  while (iter != zone_layer_map.end()) {
    if (iter->second->OwningScript() == script) {
      delete iter->second;

      iter = zone_layer_map.erase(iter);
      continue;
    }

    ++iter;
  }
}

unsigned int ZoneManager::CreateZone(unsigned int layer_id, Rectangle& area, float max_height) {
  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();

  auto zone_layer_iter = zone_layer_map.find(layer_id);
  if (zone_layer_iter == zone_layer_map.end())
    return InvalidZoneId;

  return zone_layer_iter->second->CreateZone(area, max_height);
}

bool ZoneManager::ZoneInfo(unsigned int layer_id, unsigned int zone_id, Rectangle* area, float* max_height) {
  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();

  auto zone_layer_iter = zone_layer_map.find(layer_id);
  if (zone_layer_iter == zone_layer_map.end())
    return false;

  return zone_layer_iter->second->ZoneInfo(zone_id, area, max_height);
}

void ZoneManager::DestroyZone(unsigned int layer_id, unsigned int zone_id) {
  auto scoped_zone_layer_map_ref = zone_data_storage_.ZoneLayerMap();
  auto& zone_layer_map = scoped_zone_layer_map_ref.get();

  auto zone_layer_iter = zone_layer_map.find(layer_id);
  if (zone_layer_iter == zone_layer_map.end())
    return;

  return zone_layer_iter->second->DestroyZone(zone_id);
}

void ZoneManager::StartTrackingPlayer(unsigned int player_id) {
  zone_processor_host_.QueuePlayerAction(TrackPlayerAction, player_id);
}

void ZoneManager::StopTrackingPlayer(unsigned int player_id) {
  zone_processor_host_.QueuePlayerAction(RemovePlayerAction, player_id);
}

void ZoneManager::ResetPlayers() {
  // Note that player actions will be processed in reverse order by the
  // client thread, so there is a possible race here. Since SA-MP's gamemode
  // restarts take 12 seconds, however, this shouldn't be a problem unless
  // that period significantly decreases (to, for example, 1 second).
  zone_processor_host_.QueuePlayerAction(ResetPlayersAction);
}

void ZoneManager::ProcessUpdates() {
  if (status_update_counter_++ >= 10) {
    zone_processor_host_.ProcessUpdates();
    status_update_counter_ = 0;
  }

  // We update player positions once per fourty frames, which will roughly equal to
  // six times per second. More frequent doesn't serve a purpose as players will
  // barely move in that timeframe, less frequent could miss updates for smaller
  // zones and faster moving players.
  if (player_update_counter_++ < 40)
    return;

  player_update_counter_ = 0;

  auto scoped_player_map_ref = zone_data_storage_.PlayerMap();
  auto& player_map = scoped_player_map_ref.get();

  Point position;
  int32_t virtual_world;

  for (std::pair<unsigned int, Player*> entry : player_map) {
    Player* player = entry.second;

    if (Script::GetPlayerPos(entry.first, position) &&
        Script::GetPlayerVirtualWorld(entry.first, &virtual_world)) {
      player->UpdatePosition(position, virtual_world);
    }
  }
}