// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <stdint.h>

#include "point.h"

class PerformanceTest;

class Player {
 public:
  Player(unsigned int player_id)
      : player_id_(player_id),
        latest_virtual_world_(0),
        flush_(false) {
  }

  // If this player has been invalidated, store the latest known position in the
  // given Point and mark the player as no longer being invalidated.
  bool GetPositionIfInvalidated(Point& position, bool* flush) {
    if (!invalidated_)
      return false;

    *flush = flush_;

    position = latest_position_;
    invalidated_ = false;
    flush_ = false;

    return true;
  }

  // Update the position for this player. This method must only be called on
  // the server thread, as we'll access SA-MP's internal methods (through Pawn).
  void UpdatePosition(Point& position, int32_t virtual_world);

 private:
  friend class PerformanceTest;

  unsigned int player_id_;

  Point latest_position_;
  int32_t latest_virtual_world_;

  bool invalidated_;
  bool flush_;
};