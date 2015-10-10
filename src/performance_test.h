// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

class Player;
struct Point;
class ZoneManager;

class PerformanceTest {
 public:
  static void Initialize(ZoneManager* zone_manager, unsigned int layers, unsigned int zones, unsigned int players);
  static void EnableRealisticMovement() { realistic_movement_ = true; }

  static void Tick(ZoneManager* zone_manager);

  static bool GetPlayerPos(const Player* player, Point& position);

 private:
  static bool realistic_movement_;
};