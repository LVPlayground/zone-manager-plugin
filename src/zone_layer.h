// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "player_zone_state.h"
#include "script.h"
#include "zone.h"

#include <vector>

class Player;
struct Rectangle;

class ZoneLayer {
  typedef std::vector<Zone*> ZoneVector;

 public:
  ZoneLayer(Script* script)
    : script_(script)
    , current_zone_id_(0) {
  }

  // Creates a new zone which will be owned by this layer.
  unsigned int CreateZone(Rectangle& area, float max_height);

  // Returns boundary information for the given zone.
  bool ZoneInfo(unsigned int zone_id, Rectangle* area, float* max_height) const;

  // Removes a zone from this layer.
  void DestroyZone(unsigned int zone_id);

  // Provides a constant iterator to the beginning of the zone vector.
  inline ZoneVector::const_iterator Begin() const { return zone_vector_.begin(); }

  // Provices a constant iterator to the end of the zone vector.
  inline ZoneVector::const_iterator End() const { return zone_vector_.end(); }

  // Returns this layer's instance of the Player zone state map, which contains the
  // states of all players in this zone, i.e. where they are.
  PlayerZoneState& PlayerState() { return player_zone_state_; }

  // Retrieves the Script object which created this layer.
  Script* OwningScript() const { return script_; }

 private:
  Script* script_;
  PlayerZoneState player_zone_state_;
  unsigned int current_zone_id_;

  // All zones are stored in this vector, which the CreateZone() method keeps sorted.
  ZoneVector zone_vector_;
};