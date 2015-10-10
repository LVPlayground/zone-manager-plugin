// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "player.h"

#include <unordered_map>
#include <vector>

class Zone;

class PlayerZoneState {
 public:
  // Returns the known zone for the given player. This is a quick O(1) (average case)
  // lookup in the unordered map which we store with player statuses.
  const Zone* CurrentZoneForPlayer(unsigned int player_id) const {
    auto iter = player_zone_map_.find(player_id);
    if (iter == player_zone_map_.end())
      return nullptr;

    return iter->second;
  }

  // Updates the current zone for a player to the given zone.
  void SetZoneForPlayer(unsigned int player_id, const Zone* zone) {
    player_zone_map_[player_id] = zone;
  }

  // Removes any stored zone for this player from the status map.
  void ResetZoneForPlayer(unsigned int player_id) {
    player_zone_map_.erase(player_id);
  }

  // Resets player states for any player who is in the given zone. This will be used
  // when a zone is being removed from this layer, which invalidates all those states.
  void ResetPlayersForZone(Zone* zone) {
    auto player_zone_map_iterator = player_zone_map_.begin();
    while (player_zone_map_iterator != player_zone_map_.end()) {
      if (player_zone_map_iterator->second == zone)
        player_zone_map_iterator = player_zone_map_.erase(player_zone_map_iterator);
      else
        ++player_zone_map_iterator;
    }
  }

 private:
  std::unordered_map<unsigned int, const Zone*> player_zone_map_;
};