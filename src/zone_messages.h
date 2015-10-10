// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

enum PlayerAction {
  TrackPlayerAction,
  RemovePlayerAction,
  ResetPlayersAction
};

struct PlayerActionMessage {
  PlayerAction action;
  unsigned int player_id;
};

struct PlayerZoneMutationMessage {
  unsigned int player_id;
  unsigned int layer_id;
  unsigned int zone_id;

  void* script;
};