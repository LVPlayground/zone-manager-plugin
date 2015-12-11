# Copyright 2012 Las Venturas Playground. All rights reserved.
# Use of this source code is governed by the GPLv2 license, a copy of which can
# be found in the LICENSE file.

# Compile the Zone Manager project on Linux systems. Just run "make"

$(shell [ -d "out" ] || mkdir -p out)

OUTFILE=zone-manager-plugin.so
COMPILE_FLAGS=-c -m32 -fPIC -O3 -std=gnu++0x -w -DLINUX -I.

all:
	g++ $(COMPILE_FLAGS) sdk/amxplugin.cpp -o out/amxplugin.o
	g++ $(COMPILE_FLAGS) mutex.cpp -o out/mutex.o
	g++ $(COMPILE_FLAGS) performance_test.cpp -o out/performance_test.o
	g++ $(COMPILE_FLAGS) player.cpp -o out/player.o
	g++ $(COMPILE_FLAGS) plugin.cpp -o out/plugin.o
	g++ $(COMPILE_FLAGS) script.cpp -o out/script.o
	g++ $(COMPILE_FLAGS) script_tracker.cpp -o out/script_tracker.o
	g++ $(COMPILE_FLAGS) thread.cpp -o out/thread.o
	g++ $(COMPILE_FLAGS) zone.cpp -o out/zone.o
	g++ $(COMPILE_FLAGS) zone_layer.cpp -o out/zone_layer.o
	g++ $(COMPILE_FLAGS) zone_manager.cpp -o out/zone_manager.o
	g++ $(COMPILE_FLAGS) zone_processor_client.cpp -o out/zone_processor_client.o
	g++ $(COMPILE_FLAGS) zone_processor_host.cpp -o out/zone_processor_host.o
	g++ -O2 -m32 -fshort-wchar -Wl,--export-dynamic -shared -o out/$(OUTFILE) out/*.o
