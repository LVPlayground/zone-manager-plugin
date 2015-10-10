// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "mutex.h"

#include <unordered_map>

class ZoneLayer;

class ScopedZoneLayerMapRef {
 public:
  ScopedZoneLayerMapRef(std::unordered_map<unsigned int, ZoneLayer*>& zone_layer_map, Mutex& mutex)
    : zone_layer_map_(zone_layer_map)
    , mutex_(mutex) {
    mutex.lock();
  }

  ~ScopedZoneLayerMapRef() {
    mutex_.unlock();
  }

  std::unordered_map<unsigned int, ZoneLayer*>& get() {
    return zone_layer_map_;
  }

 private:
  std::unordered_map<unsigned int, ZoneLayer*>& zone_layer_map_;
  Mutex& mutex_;
};