// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "thread.h"
#include "threadsafe_queue.h"
#include "zone_messages.h"

class ZoneDataStorage;
class ZoneProcessorHost;

class ZoneProcessorClient : public Thread {
 public:
  ZoneProcessorClient(ZoneDataStorage& data_storage)
    : data_storage_(&data_storage) {
  }

 protected:
  virtual void run();

 private:
  // Processes the player action (i.e. players joining, leaving or a full reset) messages which
  // we received from the server thread.
  void ProcessPlayerActionMessageQueue();

  // Iterates through all in-game players, then all created layers to find out where exactly
  // the player is for each layer. This is a very expensive operation, depending on the number
  // of layers, which we execute only once per 125 ms.
  void ProcessPlayerLocalization();

 private:
  friend class ZoneProcessorHost;

  ZoneDataStorage* data_storage_;

  // Push-only for the server thread, pop-only for the client thread.
  ThreadSafeQueue<PlayerActionMessage> player_action_message_queue_;

  // Push-only for the client thread, pop-only for the server thread.
  ThreadSafeQueue<PlayerZoneMutationMessage> player_enter_zone_message_queue_;
  ThreadSafeQueue<PlayerZoneMutationMessage> player_leave_zone_message_queue_;
};