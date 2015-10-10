// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "performance_test.h"

#include "player.h"
#include "rectangle.h"
#include "sdk/plugin.h"
#include "zone_manager.h"

#include <sys/timeb.h>
#include <ctime>

extern int process_players_last_iteration_time();
extern logprintf_t logprintf;

namespace {

int CurrentTime() {
  timeb time;
	ftime(&time);

	return time.millitm + (time.time & 0xFFFFF) * 1000;
}

int TimeDifference(int start_time) {
  int duration = CurrentTime() - start_time;
	if (duration < 0)
		duration += 0x100000 * 1000;

	return duration;
}

inline float RandomFloat(float min, float max) {
  return min + (float)rand() / ((float)RAND_MAX / (max - min));
}

Rectangle RandomArea() {
  float x = RandomFloat(-3000, 3000);
  float y = RandomFloat(-3000, 3000);

  return Rectangle(
    x, y, 
    x + RandomFloat(0, 200),
    y + RandomFloat(0, 200));
}

float RandomMaxHeight() {
  return RandomFloat(0, 320);
}

} // namespace

// static
bool PerformanceTest::realistic_movement_ = false;

// static
void PerformanceTest::Initialize(ZoneManager* zone_manager, unsigned int layers, unsigned int zones, unsigned int players) {
  int start_time = CurrentTime();
  srand((unsigned)time(0));

  Rectangle area;

  for (unsigned int layer_index = 0; layer_index < layers; ++layer_index) {
    zone_manager->CreateLayer(nullptr, layer_index + 1);
    for (unsigned int zone_index = 0; zone_index < zones; ++zone_index) {
      area = RandomArea();
      zone_manager->CreateZone(layer_index, area, RandomMaxHeight());
    }
  }

  for (unsigned int player_index = 0; player_index < players; ++player_index)
    zone_manager->StartTrackingPlayer(player_index);

  logprintf("[Zone Manager] Created %d layers, %d zones and %d players in %d ms.", layers,
    layers * zones, players, TimeDifference(start_time));
}

// static
void PerformanceTest::Tick(ZoneManager* zone_manager) {
  static int last_iteration_time = 0;
  int current_iteration_time = process_players_last_iteration_time();

  if (current_iteration_time == last_iteration_time)
    return;

  last_iteration_time = current_iteration_time;
  logprintf("[Zone Manager] Iteration time: %d ms.", current_iteration_time);
}

// static
bool PerformanceTest::GetPlayerPos(const Player* player, Point& position) {
  if (realistic_movement_ == true) {
    const Point& current_position = player->latest_position_;
    
    position.x = current_position.x + RandomFloat(-5, 5);
    position.y = current_position.y + RandomFloat(-5, 5);
    position.z = current_position.z + RandomFloat(-1, 1);

    if (position.x < -3000 || position.x > 3000)
      position.x = 0;

    if (position.y < -3000 || position.y > 3000)
      position.y = 0;

    if (position.z < 0 || position.z > 200)
      position.z = 50;

    return true;
  }

  position.x = RandomFloat(-3000, 3000);
  position.y = RandomFloat(-3000, 3000);
  position.z = RandomFloat(-20, 300);

  return true;
}