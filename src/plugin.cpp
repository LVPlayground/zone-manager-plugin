// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "sdk/plugin.h"

#include "performance_test.h"
#include "rectangle.h"
#include "script.h"
#include "script_tracker.h"
#include "zone_manager.h"

// Undocumented SUPPORTS rule from the SA-MP server.
#define SUPPORTS_PROCESS_TICK (0x20000)

logprintf_t logprintf;

ZoneManager* zone_manager = 0;

// -----------------------------------------------------------------------------

// native zone_layer_create(layerId);
//     returns: zone_layer_id
cell AMX_NATIVE_CALL n_zone_layer_create(AMX* amx, cell* params) {
  CHECK_PARAMS(1);

  return zone_manager->CreateLayer(Script::GetOrCreate(amx), static_cast<unsigned int>(params[1]));
}

// native zone_layer_destroy(layerId);
//     returns: [nothing]
cell AMX_NATIVE_CALL n_zone_layer_destroy(AMX* amx, cell* params) {
  CHECK_PARAMS(1);

  zone_manager->DestroyLayer(static_cast<unsigned int>(params[1]));
  return 0;
}

// native zone_create(layerId, Float: x1, Float: x2, Float: y1, Float: y2, Float: max_height);
//     returns: zone_id
cell AMX_NATIVE_CALL n_zone_create(AMX* amx, cell* params) {
  CHECK_PARAMS(6);

  unsigned int layer_id = static_cast<unsigned int>(params[1]);
  Rectangle rectangle(
    amx_ctof(params[2]),  // x1
    amx_ctof(params[4]),  // y1
    amx_ctof(params[3]),  // x2
    amx_ctof(params[5])); // y2
  float max_height = amx_ctof(params[6]);

  if (rectangle.x1 > rectangle.x2) {
    logprintf("[Zone Manager] WARNING: x1(%.2f) > x2(%.2f) for a new zone on layer %d.",
        rectangle.x1, rectangle.x2, layer_id);
    std::swap(rectangle.x1, rectangle.x2);
  }

  if (rectangle.y1 > rectangle.y2) {
    logprintf("[Zone Manager] WARNING: y1(%.2f) > y2(%.2f) for a new zone on layer %d.",
        rectangle.y1, rectangle.y2, layer_id);
    std::swap(rectangle.y1, rectangle.y2);
  }

  return zone_manager->CreateZone(layer_id, rectangle, max_height);
}

// native zone_destroy(layerId, zoneId);
//     returns: [nothing]
cell AMX_NATIVE_CALL n_zone_destroy(AMX* amx, cell* params) {
  CHECK_PARAMS(2);

  zone_manager->DestroyZone(params[1], params[2]);
  return 0;
}

// native zone_info(layerId, zoneId, &Float: x1, &Float: x2, &Float: y1, &Float: y2, &Float: max_height);
//     returns: boolean (zone found)
cell AMX_NATIVE_CALL n_zone_info(AMX* amx, cell* params) {
  CHECK_PARAMS(7);

  unsigned int layer_id = static_cast<unsigned int>(params[1]);
  unsigned int zone_id = static_cast<unsigned int>(params[2]);

  Rectangle area;
  float max_height;

  if (!zone_manager->ZoneInfo(layer_id, zone_id, &area, &max_height))
    return 0; // unable to load information for this zone.

  cell* cell_address[5];
  for (int index = 3; index <= 7; ++index)
    amx_GetAddr(amx, params[index], &cell_address[index - 3]);

  *cell_address[0] = amx_ftoc(area.x1);
  *cell_address[1] = amx_ftoc(area.x2);
  *cell_address[2] = amx_ftoc(area.y1);
  *cell_address[3] = amx_ftoc(area.y2);
  *cell_address[4] = amx_ftoc(max_height);

  return 1;
}

// native zone_player_start_tracking(playerId);
//     returns: [nothing]
cell AMX_NATIVE_CALL n_zone_player_start_tracking(AMX* amx, cell* params) {
  CHECK_PARAMS(1);

  zone_manager->StartTrackingPlayer(params[1]);
  return 0;
}

// native zone_player_stop_tracking(playerId);
//     returns: [nothing]
cell AMX_NATIVE_CALL n_zone_player_stop_tracking(AMX* amx, cell* params) {
  CHECK_PARAMS(1);

  zone_manager->StopTrackingPlayer(params[1]);
  return 0;
}

// native zone_reset_players();
//     returns: [nothing]
cell AMX_NATIVE_CALL n_zone_reset_players(AMX* amx, cell* params) {
  CHECK_PARAMS(0);

#if !defined(PERFORMANCE_TEST)
  zone_manager->ResetPlayers();
#endif

  return 0;
}

// -----------------------------------------------------------------------------

AMX_NATIVE_INFO pluginNativeFunctions[] = {
  { "zone_layer_create", n_zone_layer_create },
  { "zone_layer_destroy", n_zone_layer_destroy },

  { "zone_create", n_zone_create },
  { "zone_destroy", n_zone_destroy },

  { "zone_info", n_zone_info },

  { "zone_player_start_tracking", n_zone_player_start_tracking },
  { "zone_player_stop_tracking", n_zone_player_stop_tracking },
  { "zone_reset_players", n_zone_reset_players },

	{ 0, 0 }
};

// -----------------------------------------------------------------------------

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

  zone_manager = new ZoneManager();

#if defined(PERFORMANCE_TEST)
  PerformanceTest::Initialize(zone_manager, 200, 5000, 500);
  PerformanceTest::EnableRealisticMovement();
#endif

	logprintf("  [LVP] ZoneManager plugin loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
  if (zone_manager != 0)
    delete zone_manager;

	logprintf("  [LVP] ZoneManager plugin unloaded.");
}

PLUGIN_EXPORT void ProcessTick() {
  if (zone_manager != 0)
    zone_manager->ProcessUpdates();

#if defined(PERFORMANCE_TEST)
  PerformanceTest::Tick(zone_manager);
#endif
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
  ScriptTracker::OnRuntimeLoaded(amx);
  Script::InitializeForRuntime(amx);

	return amx_Register(amx, pluginNativeFunctions, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
  // Remove all zone layers which were registered by this script.
  zone_manager->DestroyLayersForScript(Script::GetIfExists(amx));

  ScriptTracker::OnRuntimeUnloaded(amx);
	return AMX_ERR_NONE;
}
