// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <stdint.h>
#include <unordered_map>

typedef struct tagAMX AMX;
struct Point;

#ifdef WIN32
typedef int32_t (*GetPlayerPos_t)(AMX*, int32_t*);
#else
typedef int (*GetPlayerPos_t)(AMX*, int*);
#endif

class Script {
 public:
  // Returns the Script instance for the given AMX runtime if it currently exists in the
  // script map. This is used to validate that scripts didn't go away while the client thread
  // was determining player statuses for a certain layer owned by said script.
  static Script* GetIfExists(AMX* runtime) {
    auto iter = script_map_.find(runtime);
    if (iter == script_map_.end())
      return nullptr;

    return iter->second;
  }

  // Returns the Script instance for the given AMX runtime, in an O(1) operation for the average
  // case, or create a new entry and push that to the script map instead.
  static Script* GetOrCreate(AMX* runtime) {
    Script* script = GetIfExists(runtime);
    if (script != nullptr)
      return script;

    script_map_[runtime] = new Script(runtime);
    return script_map_[runtime];
  }

  // Removes the given runtime from the stored Script map.
  static void Remove(AMX* runtime) {
    script_map_.erase(runtime);
  }

  // Invokes the GetPlayerPos() native in the SA-MP server and returns the position. This method
  // is static because we don't care which runtime is being used, and this operation is
  // agnostic of which script it's being executed on.
  static bool GetPlayerPos(unsigned int player_id, Point& position);

  // This method should be called once per run-time. We'll attempt to find the address of the
  // GetPlayerPos() native in the SA-MP server.
  static void InitializeForRuntime(AMX* runtime);

 public:
  // Invokes the OnPlayerEnterZone() callback in the gamemode.
  int OnPlayerEnterZone(unsigned int player_id, unsigned int layer_id, unsigned int zone_id);
   
  // Invokes the OnPlayerLeaveZone() callback in the gamemode.
  int OnPlayerLeaveZone(unsigned int player_id, unsigned int layer_id, unsigned int zone_id);
  
  // Returns the runtime for this script.
  AMX* Runtime() const { return runtime_; }

 private:
  explicit Script(AMX* runtime)
    : runtime_(runtime)
    , on_player_enter_zone_index_(0)
    , on_player_leave_zone_index_(0) {
  }

  AMX* runtime_;
  
  int on_player_enter_zone_index_;
  int on_player_leave_zone_index_;

  // Statics.
  static std::unordered_map<AMX*, Script*> script_map_;
  static GetPlayerPos_t get_player_pos_;
};