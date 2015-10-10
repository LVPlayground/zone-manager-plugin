// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "zone_messages.h"

class ZoneDataStorage;
class ZoneProcessorClient;

class ZoneProcessorHost {
public:
  ZoneProcessorHost(ZoneDataStorage& data_storage);
  ~ZoneProcessorHost();

  // Process updates which we received from the ZoneProcessorClient.
  void ProcessUpdates();

  // Queues a player action which has to be sent to the client.
  void QueuePlayerAction(PlayerAction action, unsigned int player_id = 0);

private:
  ZoneProcessorClient* client_;
};