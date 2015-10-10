// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "mutex.h"
#include "scoped_zone_layer_map_ref.h"
#include "scoped_player_map_ref.h"

#include <unordered_map>

class Player;
class ZoneLayer;
class ZoneProcessorClient;

class ZoneDataStorage {
 public:
  // Returns a scoped reference to the zone layer map, as soon as the data is exclusively available.
  ScopedZoneLayerMapRef ZoneLayerMap() {
    return ScopedZoneLayerMapRef(zone_layer_map_, zone_layer_map_mutex_);
  }

  // Returns a scoped reference to the player map, as soon as the data is exclusively available.
  ScopedPlayerMapRef PlayerMap() {
    return ScopedPlayerMapRef(player_map_, player_map_mutex_);
  }

 private:
   friend class ZoneProcessorClient;

  // Any access to the zone map must be scoped. On the server thread this means when manipulating
  // the data (i.e. adding or removing layers or zones). On the client thread this means when
  // iterating over the data to match them against a certain player.
  std::unordered_map<unsigned int, ZoneLayer*> zone_layer_map_;

  // Access to the player map is free on the client thread, except when manipulating the list (i.e.
  // when adding or removing players). On the server thread access must always be scoped, although
  // the only legal access is when updating player positions.
  std::unordered_map<unsigned int, Player*> player_map_;

  Mutex zone_layer_map_mutex_;
  Mutex player_map_mutex_;
};