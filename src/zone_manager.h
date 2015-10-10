// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "zone_data_storage.h"
#include "zone_processor_host.h"

struct Rectangle;
class Script;

const int InvalidZoneId = 2147483647 /** INT_MAX **/ - 1;

class ZoneManager {
 public:
  ZoneManager();

  // Create a new layer which can contain zones.
  unsigned int CreateLayer(Script* script, unsigned int layer_id);

  // Remove a layer including all the zones it owns.
  void DestroyLayer(unsigned int layer_id);

  // Removes all layers associated with a certain script.
  void DestroyLayersForScript(Script* script);

  // Creates a new zone with the given area on the given layer.
  unsigned int CreateZone(unsigned int layer_id, Rectangle& area, float max_height);

  // Allows the caller to retrieve the boundary information for a selected zone.
  bool ZoneInfo(unsigned int layer_id, unsigned int zone_id, Rectangle* area, float* max_height);

  // Destroys a zone and removes it from the given layer.
  void DestroyZone(unsigned int layer_id, unsigned int zone_id);

  // Start tracking a player to see which zone they're in.
  void StartTrackingPlayer(unsigned int player_id);

  // Stop tracking the given player.
  void StopTrackingPlayer(unsigned int player_id);

  // Resets the status of all players and removes them.
  void ResetPlayers();

  // Processes any updates in the ZoneProcessorHost and feeds them to the script.
  void ProcessUpdates();

 private:
  ZoneDataStorage zone_data_storage_;
  ZoneProcessorHost zone_processor_host_;

  unsigned int player_update_counter_;
};