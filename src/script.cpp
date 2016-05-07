// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "script.h"

#include "point.h"
#include "script_tracker.h"
#include "sdk/plugin.h"

const int CallbackNotFoundIndex = -1;

// static
AmxNativeCall_t Script::get_player_pos_ = 0;
AmxNativeCall_t Script::get_player_virtual_world_ = 0;

// static
std::unordered_map<AMX*, Script*> Script::script_map_;

// static
void Script::InitializeForRuntime(AMX* runtime) {
  if (get_player_pos_ != 0 && get_player_virtual_world_ != 0)
    return;

  AMX_HEADER* runtime_header = reinterpret_cast<AMX_HEADER*>(runtime->base);

  int get_player_pos_index;
  if (amx_FindNative(runtime, "GetPlayerPos", &get_player_pos_index) == AMX_ERR_NONE) {
    get_player_pos_ = (AmxNativeCall_t)((AMX_FUNCSTUB *)((char *)runtime_header + runtime_header->natives + runtime_header->defsize * get_player_pos_index))->address;
  }

  int get_player_virtual_world_index;
  if (amx_FindNative(runtime, "GetPlayerVirtualWorld", &get_player_virtual_world_index) == AMX_ERR_NONE) {
    get_player_virtual_world_ = (AmxNativeCall_t)((AMX_FUNCSTUB *)((char *)runtime_header + runtime_header->natives + runtime_header->defsize * get_player_virtual_world_index))->address;
  }
}

// public OnPlayerEnterZone(player_id, layer_id, zone_id);
int Script::OnPlayerEnterZone(unsigned int player_id, unsigned int layer_id, unsigned int zone_id) {
  if (on_player_enter_zone_index_ == CallbackNotFoundIndex)
    return 0;

  if (on_player_enter_zone_index_ == 0) {
		if (amx_FindPublic(runtime_, "OnPlayerEnterZone", &on_player_enter_zone_index_) != AMX_ERR_NONE) {
			on_player_enter_zone_index_ = CallbackNotFoundIndex;
			return 0;
		}
	}

  cell return_value;

  amx_Push(runtime_, zone_id);
  amx_Push(runtime_, layer_id);
  amx_Push(runtime_, player_id);

  amx_Exec(runtime_, &return_value, on_player_enter_zone_index_);
  return return_value;
}

// public OnPlayerLeaveZone(player_id, layer_id, zone_id);
int Script::OnPlayerLeaveZone(unsigned int player_id, unsigned int layer_id, unsigned int zone_id) {
  if (on_player_leave_zone_index_ == CallbackNotFoundIndex)
    return 0;

  if (on_player_leave_zone_index_ == 0) {
		if (amx_FindPublic(runtime_, "OnPlayerLeaveZone", &on_player_leave_zone_index_) != AMX_ERR_NONE) {
			on_player_leave_zone_index_ = CallbackNotFoundIndex;
			return 0;
		}
	}

  cell return_value;

  amx_Push(runtime_, zone_id);
  amx_Push(runtime_, layer_id);
  amx_Push(runtime_, player_id);

  amx_Exec(runtime_, &return_value, on_player_leave_zone_index_);
  return return_value;
}

extern logprintf_t logprintf;

// static
bool Script::GetPlayerPos(unsigned int player_id, Point& position) {
  AMX* runtime = ScriptTracker::GetActiveRuntime();
  if (get_player_pos_ == 0 || runtime == 0)
    return false;

  cell* physical_storage[3];
  cell params[5];

  params[0] = 4 * sizeof(cell);
  params[1] = player_id;
  amx_Allot(runtime, 1, &params[2], &physical_storage[0]);
  amx_Allot(runtime, 1, &params[3], &physical_storage[1]);
  amx_Allot(runtime, 1, &params[4], &physical_storage[2]);

  cell retval = get_player_pos_(runtime, params);
  if (retval == 1) {
    position.x = amx_ctof(*physical_storage[0]);
    position.y = amx_ctof(*physical_storage[1]);
    position.z = amx_ctof(*physical_storage[2]);
  }

  amx_Release(runtime, params[2]);
  amx_Release(runtime, params[3]);
  amx_Release(runtime, params[4]);

  return retval == 1;
}

// static
bool Script::GetPlayerVirtualWorld(unsigned int player_id, int32_t* virtual_world) {
  AMX* runtime = ScriptTracker::GetActiveRuntime();
  if (get_player_virtual_world_ == 0 || runtime == 0)
    return false;

  cell params[2];

  params[0] = 1 * sizeof(cell);
  params[1] = player_id;

  cell retval = get_player_virtual_world_(runtime, params);
  *virtual_world = static_cast<int32_t>(retval);

  return true;
}
