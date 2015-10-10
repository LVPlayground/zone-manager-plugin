// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

struct Point {
  Point()
    : x(0)
    , y(0)
    , z(0) {
  }

  Point(float _x, float _y, float _z)
    : x(_x)
    , y(_y)
    , z(_z) {
  }

  float x, y, z;
};