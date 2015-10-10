// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "zone_processor_host.h"

#include "script.h"
#include "zone_processor_client.h"

ZoneProcessorHost::ZoneProcessorHost(ZoneDataStorage& data_storage) {
  client_ = new ZoneProcessorClient(data_storage);
  client_->StartThread();
}

ZoneProcessorHost::~ZoneProcessorHost() {
  client_->StopThread();
  delete client_;
}

void ZoneProcessorHost::ProcessUpdates() {
  PlayerZoneMutationMessage message;

  while (client_->player_leave_zone_message_queue_.size() > 0) {
    client_->player_leave_zone_message_queue_.pop(message);
    Script* script = Script::GetIfExists(static_cast<AMX*>(message.script));
    if (script == nullptr)
      continue; // bail: the script went away.

    script->OnPlayerLeaveZone(message.player_id, message.layer_id, message.zone_id);
  }

  while (client_->player_enter_zone_message_queue_.size() > 0) {
    client_->player_enter_zone_message_queue_.pop(message);
    Script* script = Script::GetIfExists(static_cast<AMX*>(message.script));
    if (script == nullptr)
      continue; // bail: the script went away.

    script->OnPlayerEnterZone(message.player_id, message.layer_id, message.zone_id);
  }
}

void ZoneProcessorHost::QueuePlayerAction(PlayerAction action, unsigned int player_id) {
  PlayerActionMessage message;
  message.action = action;
  message.player_id = player_id;

  client_->player_action_message_queue_.push(message);
}