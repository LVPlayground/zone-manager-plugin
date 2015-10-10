// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "zone_layer.h"

#include "rectangle.h"

#include <algorithm>

namespace {

bool ZoneAreaComperator(const Zone* left, const Zone* right) {
  return left->Area().x1 < right->Area().x1;
}

} // namespace

unsigned int ZoneLayer::CreateZone(Rectangle& area, float max_height) {
  Zone* zone = new Zone(current_zone_id_, area, max_height);
  ZoneVector::iterator location = std::lower_bound(zone_vector_.begin(),
    zone_vector_.end(), zone, ZoneAreaComperator);
  zone_vector_.insert(location, zone);

  return current_zone_id_++;
}

bool ZoneLayer::ZoneInfo(unsigned int zone_id, Rectangle* area, float* max_height) const {
  for (auto zone_iterator = zone_vector_.begin(); zone_iterator != zone_vector_.end(); ++zone_iterator) {
    if ((*zone_iterator)->Id() != zone_id)
      continue;

    *area = (*zone_iterator)->Area();
    *max_height = (*zone_iterator)->MaxHeight();
    return true;
  }

  return false;
}

void ZoneLayer::DestroyZone(unsigned int zone_id) {
  ZoneVector::iterator zone_iterator;
  for (zone_iterator = zone_vector_.begin(); zone_iterator != zone_vector_.end(); ++zone_iterator) {
    if ((*zone_iterator)->Id() == zone_id)
      break;
  }

  if (zone_iterator == zone_vector_.end())
    return; // quick bail: The vector does not contain this zone_id.

  player_zone_state_.ResetPlayersForZone(*zone_iterator);
  delete *zone_iterator;

  zone_vector_.erase(zone_iterator);
}