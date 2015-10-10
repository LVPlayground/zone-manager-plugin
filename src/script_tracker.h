// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <algorithm>
#include <vector>

typedef struct tagAMX AMX;

class ScriptTracker {
 public:
  // Must be invoked when a new Pawn runtime has been loaded.
  static void OnRuntimeLoaded(AMX* runtime) {
    loaded_runtime_map_.push_back(runtime);
  }

  // Must be invoked when a Pawn runtime has been closed.
  static void OnRuntimeUnloaded(AMX* runtime) {
    auto iter = std::find(loaded_runtime_map_.begin(), loaded_runtime_map_.end(), runtime);
    if (iter != loaded_runtime_map_.end())
      loaded_runtime_map_.erase(iter);
  }

  // Returns an active Pawn runtime.
  static AMX* GetActiveRuntime() {
    if (loaded_runtime_map_.size() == 0)
      return 0;

    return loaded_runtime_map_.front();
  }

 private:
  static std::vector<AMX*> loaded_runtime_map_;
};