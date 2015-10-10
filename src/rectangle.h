// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

struct Rectangle {
  Rectangle()
    : x1(0)
    , y1(0)
    , x2(0)
    , y2(0) {
  }

  Rectangle(float _x1, float _y1, float _x2, float _y2)
    : x1(_x1)
    , y1(_y1)
    , x2(_x2)
    , y2(_y2) {
  }

  float x1, y1, x2, y2;
};