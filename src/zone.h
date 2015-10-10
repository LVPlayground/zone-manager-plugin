// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "rectangle.h"

class Zone {
 public:
  Zone(unsigned int zone_id, Rectangle& area, float max_height)
    : zone_id_(zone_id)
    , area_(area)
    , max_height_(max_height) {
  }

  // Returns the Id which this zone can be identified with.
  inline unsigned int Id() const { return zone_id_; }

  // Returns the rectangular area this zone is encapsulated in.
  inline const Rectangle& Area() const { return area_; }

  // Returns the maximum height of any activity to be considered part of this zone.
  inline float MaxHeight() const { return max_height_; }

 private:
  unsigned int zone_id_;
  Rectangle area_;
  float max_height_;
};