// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "mutex.h"

#include <unordered_map>

class Player;

class ScopedPlayerMapRef {
 public:
  ScopedPlayerMapRef(std::unordered_map<unsigned int, Player*>& player_map, Mutex& mutex)
    : player_map_(player_map)
    , mutex_(mutex) {
    mutex.lock();
  }

  ~ScopedPlayerMapRef() {
    mutex_.unlock();
  }

  std::unordered_map<unsigned int, Player*>& get() {
    return player_map_;
  }

 private:
  std::unordered_map<unsigned int, Player*>& player_map_;
  Mutex& mutex_;
};