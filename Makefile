PYTHON ?= python3
BUILD_KIND ?= Matching

ifeq ($(BUILD_KIND),Matching)
OBJ_DIR := build/obj
EXE := build/LEGOStarWarsSaga.exe
STRICT_EXE := build/strict/LEGOStarWarsSaga.strict.exe
else ifeq ($(BUILD_KIND),Debug)
OBJ_DIR := build/Debug/obj
EXE := build/LEGOStarWarsSaga_debug.exe
STRICT_EXE := build/Debug/LEGOStarWarsSaga.strict_debug.exe
DIAGNOSTIC_OBJECTS := build/Debug/stream_capture.obj build/Debug/log.obj build/Debug/crash_diagnostics.obj build/Debug/skin_trace.obj build/Debug/skin_trace_windows.obj
DEBUG_CAPTURE_HELPER := build/saga_debug_capture.exe
DEBUG_STRICT_CAPTURE_HELPER := build/Debug/saga_debug_capture.exe
ifneq ($(filter match diff check-objects,$(MAKECMDGOALS)),)
$(error Matching comparisons require BUILD_KIND=Matching; Debug artifacts are never comparison inputs)
endif
else
$(error BUILD_KIND must be Matching or Debug)
endif

# Parallelize object compilation by default; explicit -j still overrides.
MAKEFLAGS += -j$(shell nproc)

SOURCES := $(filter-out src/stream_capture.c src/log_copier.c src/log.c src/crash_diagnostics.cpp src/skin_trace.cpp src/skin_trace_windows.cpp,$(shell find src -type f \( -name '*.cpp' -o -name '*.c' \) | LC_ALL=C sort))
SOURCE_DIRS := $(shell find src -type d)
OBJECTS := $(patsubst src/%,$(OBJ_DIR)/%,$(SOURCES:.cpp=.obj))
OBJECTS := $(OBJECTS:.c=.obj)
$(filter $(OBJ_DIR)/nu2api/pc_fmv%.obj,$(OBJECTS)): src/nu2api/pc_fmv_native.h

# Original Vorbis private headers/textual tables belong to these owners only.
VORBIS_HEADERS := $(shell find src/audio/vorbis -type f \( -name '*.h' -o -name '*.inc' \) 2>/dev/null)
ifneq ($(filter $(OBJ_DIR)/audio/vorbis/%,$(OBJECTS)),)
$(filter $(OBJ_DIR)/audio/vorbis/%,$(OBJECTS)): $(VORBIS_HEADERS)
endif
# Retail libvorbis was built without /GS: none of its functions carry a security
# cookie. vorbisfile keeps them, so it was built with the game's flags.
$(filter-out $(OBJ_DIR)/audio/vorbis/vorbisfile.obj,$(filter $(OBJ_DIR)/audio/vorbis/% $(OBJ_DIR)/audio/vorbis_%,$(OBJECTS))): CXXFLAGS += /GS-
$(filter $(OBJ_DIR)/nu2api/oggreader.obj $(OBJ_DIR)/nu2api/nustream.obj,$(OBJECTS)): $(VORBIS_HEADERS)
D3DX_LIB := build/lib/imports/d3dx9_35.lib
DSOUND_LIB := build/lib/imports/dsound.lib
D3D9_LIB := build/lib/imports/d3d9.lib
DINPUT8_LIB := build/lib/imports/dinput8.lib
XINPUT_LIB := build/lib/imports/xinput1_3.lib
BINK_LIB := build/lib/imports/binkw32.lib
CPPFLAGS := /Isrc /Ibuild/tools/msvc8/include /Ibuild/tools/msvc8/PlatformSDK/Include
CXXFLAGS := /nologo /c /O2 /Gy /GS /MT
LDFLAGS := /nologo /machine:x86 /subsystem:windows /incremental:no /manifest:no /opt:noref /opt:noicf
ifeq ($(BUILD_KIND),Debug)
CPPFLAGS += /DSAGA_DEBUG
CXXFLAGS += /Z7
# Wine can give GUI applications no stdout when launched from a terminal. Keep the
# native WinMain CRT path, but let the loader provide diagnostic console handles.
LDFLAGS := $(subst /subsystem:windows,/subsystem:console /entry:WinMainCRTStartup,$(LDFLAGS))
# Explicit /OPT defaults above prevent /DEBUG from changing native link optimization.
LDFLAGS += /DEBUG /MAP:build/Debug/LEGOStarWarsSaga_debug.map
# Each Debug executable gets its own adjacent PDB, including link-strict.
pdb_flag = /PDB:$(patsubst %.exe,%.pdb,$(1))
endif
LDLIBS := user32.lib gdi32.lib winmm.lib ole32.lib oleaut32.lib shell32.lib advapi32.lib $(abspath $(D3D9_LIB)) $(abspath $(D3DX_LIB)) $(abspath $(DSOUND_LIB)) $(abspath $(DINPUT8_LIB)) $(abspath $(XINPUT_LIB)) $(abspath $(BINK_LIB))

.DEFAULT_GOAL := build
.DELETE_ON_ERROR:
.PHONY: all build objects link Matching Debug run setup editor check check-core lint check-objects test match diff inspect clean

all: build
build: link
objects: $(OBJECTS) $(DIAGNOSTIC_OBJECTS)
link: $(EXE)
Matching:
	$(MAKE) BUILD_KIND=Matching build
Debug:
	$(MAKE) BUILD_KIND=Debug build
.PHONY: link-strict
run: $(EXE)
	cd res && WINEPREFIX="$(CURDIR)/build/tools/wine" WINEDEBUG=-all WINEDLLOVERRIDES="winemenubuilder.exe=d" wine ../$(EXE)

# Directory dependencies also relink when a source is removed from the input list.
$(EXE): $(DEBUG_CAPTURE_HELPER) $(OBJECTS) $(DIAGNOSTIC_OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB) $(BINK_LIB) $(SOURCE_DIRS) config/toolchain.json Makefile tools/cc.py tools/lib.py
	$(PYTHON) tools/cc.py --link --allow-unresolved $@ $(OBJECTS) $(DIAGNOSTIC_OBJECTS) -- $(LDFLAGS) $(call pdb_flag,$@) $(LDLIBS)

# The complete link: every referenced function must be implemented.
link-strict: $(DEBUG_STRICT_CAPTURE_HELPER) $(OBJECTS) $(DIAGNOSTIC_OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB) $(BINK_LIB)
	$(PYTHON) tools/cc.py --link $(STRICT_EXE) $(OBJECTS) $(DIAGNOSTIC_OBJECTS) -- $(LDFLAGS) $(call pdb_flag,$(STRICT_EXE)) $(LDLIBS)

# Debug support contains diagnostics only and never enters Matching objects.
# Recompile this small Debug-only object so its identity tracks source edits and commits.
.PHONY: debug-identity
build/Debug/log.obj: src/log.c src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/debug_identity.py debug-identity | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(D3DX_LIB): tools/d3dx9_35.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(DSOUND_LIB): tools/dsound.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(D3D9_LIB): tools/d3d9.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(BINK_LIB): tools/binkw32.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib --decorated-exports $@ $<

$(DINPUT8_LIB): tools/dinput8.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(XINPUT_LIB): tools/xinput1_3.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(OBJ_DIR)/%.obj: src/%.cpp src/saga.h src/externals.h src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(OBJ_DIR)/%.obj: src/%.c src/saga.h src/externals.h src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

# Serialize first-time Wine setup, then compile distinct objects concurrently.
build/tools/wine/.ready:
	mkdir -p build/tools
	WINEPREFIX="$(CURDIR)/build/tools/wine" WINEDEBUG=-all WINEDLLOVERRIDES="winemenubuilder.exe=d" wineboot -i
	touch $@

setup:
	$(PYTHON) tools/setup.py
	$(PYTHON) tools/check.py --editor

editor:
	$(PYTHON) tools/check.py --editor

check: lint check-core

lint:
	$(PYTHON) tools/check.py --format
	$(PYTHON) tools/check.py --tidy

check-core:
	$(PYTHON) tools/check.py
	$(PYTHON) -m unittest discover -s tools -p 'test_*.py'


check-objects: objects
	$(PYTHON) tools/check.py --objects

test:
	$(PYTHON) -m unittest discover -s tools -p 'test_*.py'

match: check-objects
	$(PYTHON) tools/match.py --update

diff: check-objects
	@test -n "$(FUNC)" || { echo 'Usage: make diff FUNC=NuRand'; exit 2; }
	$(PYTHON) tools/match.py --function "$(FUNC)"

inspect:
	$(PYTHON) tools/check.py --inspect

clean:
	rm -rf build/obj build/objdiff build/Debug/obj
	rm -f build/LEGOStarWarsSaga.exe build/LEGOStarWarsSaga_debug.exe build/LEGOStarWarsSaga_debug.pdb
	rm -f build/strict/LEGOStarWarsSaga.strict.exe build/Debug/LEGOStarWarsSaga.strict_debug.exe build/Debug/LEGOStarWarsSaga.strict_debug.pdb
	rm -f build/saga_debug_capture.exe build/saga_debug_capture.pdb build/Debug/saga_debug_capture.exe build/Debug/saga_debug_capture.pdb build/Debug/stream_capture.obj build/Debug/log_copier.obj
	rm -f build/Debug/log.obj build/Debug/crash_diagnostics.obj build/Debug/skin_trace.obj build/Debug/skin_trace_windows.obj build/Debug/LEGOStarWarsSaga_debug.pdb build/Debug/LEGOStarWarsSaga_debug.map

build/Debug/skin_trace.obj: src/skin_trace.cpp src/skin_trace.h src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/Debug/skin_trace_windows.obj: src/skin_trace_windows.cpp src/skin_trace.h src/diagnostics.h src/saga.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/Debug/crash_diagnostics.obj: src/crash_diagnostics.cpp src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

ifeq ($(BUILD_KIND),Debug)
# The copier is a separate process: its main never enters the game executable.
build/Debug/stream_capture.obj: src/stream_capture.c src/diagnostics.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/Debug/log_copier.obj: src/log_copier.c config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/saga_debug_capture.exe: build/Debug/log_copier.obj config/toolchain.json Makefile tools/cc.py
	$(PYTHON) tools/cc.py --link $@ $< -- /nologo /machine:x86 /subsystem:console /incremental:no /manifest:no /opt:noref /opt:noicf /DEBUG /PDB:build/saga_debug_capture.pdb

# Strict Debug lives in a second directory and resolves the same adjacent leaf.
build/Debug/saga_debug_capture.exe: build/saga_debug_capture.exe
	cp $< $@
	cp build/saga_debug_capture.pdb build/Debug/saga_debug_capture.pdb
endif
