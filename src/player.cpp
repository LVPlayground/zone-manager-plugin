// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "player.h"

#include <cmath>

const float MinimumDistanceForInvalidatingPosition = 3.0f;

void Player::UpdatePosition(Point& position) {
  if (fabs(latest_position_.x - position.x) >= MinimumDistanceForInvalidatingPosition ||
      fabs(latest_position_.y - position.y) >= MinimumDistanceForInvalidatingPosition ||
      fabs(latest_position_.z - position.z) >= MinimumDistanceForInvalidatingPosition) {
    latest_position_ = position;
    invalidated_ = true;
  }
}