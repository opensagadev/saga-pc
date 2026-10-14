PYTHON ?= python3
BUILD_KIND ?= Matching

ifeq ($(BUILD_KIND),Matching)
OBJ_DIR := build/obj
EXE := build/LEGOStarWarsSaga.exe
else ifeq ($(BUILD_KIND),Debug)
OBJ_DIR := build/Debug/obj
EXE := build/Debug/LEGOStarWarsSaga.exe
DIAGNOSTIC_OBJECTS := build/Debug/log.obj
ifneq ($(filter match diff check-objects,$(MAKECMDGOALS)),)
$(error Matching comparisons require BUILD_KIND=Matching; Debug artifacts are never comparison inputs)
endif
else
$(error BUILD_KIND must be Matching or Debug)
endif

# Parallelize object compilation by default; explicit -j still overrides.
MAKEFLAGS += -j$(shell nproc)

SOURCES := $(shell find src -type f \( -name '*.cpp' -o -name '*.c' \) | LC_ALL=C sort)
SOURCE_DIRS := $(shell find src -type d)
OBJECTS := $(patsubst src/%,$(OBJ_DIR)/%,$(SOURCES:.cpp=.obj))
OBJECTS := $(OBJECTS:.c=.obj)
# Original Vorbis private headers/textual tables belong to these owners only.
VORBIS_HEADERS := $(shell find src/audio/vorbis -type f \( -name '*.h' -o -name '*.inc' \) 2>/dev/null)
ifneq ($(filter $(OBJ_DIR)/audio/vorbis/%,$(OBJECTS)),)
$(filter $(OBJ_DIR)/audio/vorbis/%,$(OBJECTS)): $(VORBIS_HEADERS)
endif
# Retail libvorbis was built without /GS: none of its functions carry a security
# cookie. vorbisfile keeps them, so it was built with the game's flags.
$(filter-out $(OBJ_DIR)/audio/vorbis/vorbisfile.obj,$(filter $(OBJ_DIR)/audio/vorbis/% $(OBJ_DIR)/audio/vorbis_%,$(OBJECTS))): CXXFLAGS += /GS-
$(filter $(OBJ_DIR)/nu2api/oggreader.obj $(OBJ_DIR)/nu2api/nustream.obj,$(OBJECTS)): $(VORBIS_HEADERS)
D3DX_LIB := build/d3dx9_35.lib
DSOUND_LIB := build/dsound.lib
D3D9_LIB := build/d3d9.lib
DINPUT8_LIB := build/dinput8.lib
XINPUT_LIB := build/xinput1_3.lib
CPPFLAGS := /Isrc /Ibuild/tools/msvc8/include /Ibuild/tools/msvc8/PlatformSDK/Include
CXXFLAGS := /nologo /c /O2 /Gy /GS /MT
LDFLAGS := /nologo /machine:x86 /subsystem:windows /incremental:no /manifest:no /opt:noref /opt:noicf
ifeq ($(BUILD_KIND),Debug)
CPPFLAGS += /DSAGA_DEBUG
CXXFLAGS += /Z7
# Keep native linker defaults: the pinned PDB writer cannot emit a PDB.
LDFLAGS += /MAP:$(EXE:.exe=.map)
endif
LDLIBS := user32.lib gdi32.lib winmm.lib ole32.lib oleaut32.lib shell32.lib advapi32.lib $(abspath $(D3D9_LIB)) $(abspath $(D3DX_LIB)) $(abspath $(DSOUND_LIB)) $(abspath $(DINPUT8_LIB)) $(abspath $(XINPUT_LIB))

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
$(EXE): $(OBJECTS) $(DIAGNOSTIC_OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB) $(SOURCE_DIRS) config/toolchain.json Makefile tools/cc.py tools/lib.py
	$(PYTHON) tools/cc.py --link --allow-unresolved $@ $(OBJECTS) $(DIAGNOSTIC_OBJECTS) -- $(LDFLAGS) $(LDLIBS)

# The complete link: every referenced function must be implemented.
link-strict: $(OBJECTS) $(DIAGNOSTIC_OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB)
	$(PYTHON) tools/cc.py --link $(EXE:.exe=.strict.exe) $(OBJECTS) $(DIAGNOSTIC_OBJECTS) -- $(LDFLAGS) $(LDLIBS)

# Debug support contains diagnostics only and never enters Matching objects.
build/Debug/log.obj: diagnostics/log.c src/diagnostics.h config/toolchain.json Makefile tools/cc.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(D3DX_LIB): tools/d3dx9_35.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(DSOUND_LIB): tools/dsound.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

$(D3D9_LIB): tools/d3d9.def tools/cc.py Makefile
	$(PYTHON) tools/cc.py --implib $@ $<

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
	rm -f build/LEGOStarWarsSaga.exe build/LEGOStarWarsSaga.strict.exe
	rm -f build/Debug/log.obj build/Debug/LEGOStarWarsSaga.exe build/Debug/LEGOStarWarsSaga.strict.exe
	rm -f build/Debug/LEGOStarWarsSaga.pdb build/Debug/LEGOStarWarsSaga.map
