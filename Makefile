PYTHON ?= python3

# Parallelize object compilation by default; explicit -j still overrides.
MAKEFLAGS += -j$(shell nproc)

SOURCES := $(shell find src -type f \( -name '*.cpp' -o -name '*.c' \) | LC_ALL=C sort)
SOURCE_DIRS := $(shell find src -type d)
OBJECTS := $(patsubst src/%,build/obj/%,$(SOURCES:.cpp=.obj))
OBJECTS := $(OBJECTS:.c=.obj)
# Original Vorbis private headers/textual tables belong to these owners only.
VORBIS_HEADERS := $(shell find src/audio/vorbis -type f \( -name '*.h' -o -name '*.inc' \) 2>/dev/null)
ifneq ($(filter build/obj/audio/vorbis/%,$(OBJECTS)),)
$(filter build/obj/audio/vorbis/%,$(OBJECTS)): $(VORBIS_HEADERS)
endif
# Retail libvorbis was built without /GS: none of its functions carry a security
# cookie. vorbisfile keeps them, so it was built with the game's flags.
$(filter-out build/obj/audio/vorbis/vorbisfile.obj,$(filter build/obj/audio/vorbis/% build/obj/audio/vorbis_%,$(OBJECTS))): CXXFLAGS += /GS-
$(filter build/obj/nu2api/oggreader.obj build/obj/nu2api/nustream.obj,$(OBJECTS)): $(VORBIS_HEADERS)
EXE := build/LEGOStarWarsSaga.exe
D3DX_LIB := build/d3dx9_35.lib
DSOUND_LIB := build/dsound.lib
D3D9_LIB := build/d3d9.lib
DINPUT8_LIB := build/dinput8.lib
XINPUT_LIB := build/xinput1_3.lib
CPPFLAGS := /Isrc /Ibuild/tools/msvc8/include /Ibuild/tools/msvc8/PlatformSDK/Include
CXXFLAGS := /nologo /c /O2 /Gy /GS /MT
LDFLAGS := /nologo /machine:x86 /subsystem:windows /incremental:no /manifest:no /opt:noref /opt:noicf
LDLIBS := user32.lib gdi32.lib winmm.lib ole32.lib oleaut32.lib shell32.lib advapi32.lib $(abspath $(D3D9_LIB)) $(abspath $(D3DX_LIB)) $(abspath $(DSOUND_LIB)) $(abspath $(DINPUT8_LIB)) $(abspath $(XINPUT_LIB))

.DEFAULT_GOAL := build
.DELETE_ON_ERROR:
.PHONY: all build objects link runtime run setup editor check check-core lint check-objects test match diff inspect clean

all: build
build: link
objects: $(OBJECTS)
link: $(EXE)
.PHONY: link-strict
run: $(EXE)
	cd res && WINEPREFIX="$(CURDIR)/build/tools/wine" WINEDEBUG=-all WINEDLLOVERRIDES="winemenubuilder.exe=d" wine ../$(EXE)

# Directory dependencies also relink when a source is removed from the input list.
$(EXE): $(OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB) $(SOURCE_DIRS) config/toolchain.json Makefile tools/cc.py tools/lib.py
	$(PYTHON) tools/cc.py --link --allow-unresolved $@ $(OBJECTS) -- $(LDFLAGS) $(LDLIBS)

# The complete link: every referenced function must be implemented.
link-strict: $(OBJECTS) $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB)
	$(PYTHON) tools/cc.py --link build/LEGOStarWarsSaga.strict.exe $(OBJECTS) -- $(LDFLAGS) $(LDLIBS)

# Runtime build: same sources with SAGA_RUNTIME, so reached stubs are logged and
# unresolved symbols trap by name. It never touches the matching objects or EXE.
RUNTIME_DIR := build/runtime
RUNTIME_EXE := $(RUNTIME_DIR)/LEGOStarWarsSaga.exe
RUNTIME_OBJECTS := $(patsubst build/obj/%,$(RUNTIME_DIR)/obj/%,$(OBJECTS)) $(RUNTIME_DIR)/saga_runtime.obj
RUNTIME_LIBS := $(D3DX_LIB) $(DSOUND_LIB) $(D3D9_LIB) $(DINPUT8_LIB) $(XINPUT_LIB)
$(RUNTIME_DIR)/%.obj: CPPFLAGS += /DSAGA_RUNTIME
$(patsubst build/obj/%,$(RUNTIME_DIR)/obj/%,$(filter build/obj/audio/vorbis/%,$(OBJECTS))): $(VORBIS_HEADERS)
$(patsubst build/obj/%,$(RUNTIME_DIR)/obj/%,$(filter-out build/obj/audio/vorbis/vorbisfile.obj,$(filter build/obj/audio/vorbis/% build/obj/audio/vorbis_%,$(OBJECTS)))): CXXFLAGS += /GS-

runtime: $(RUNTIME_EXE)

$(RUNTIME_EXE): $(RUNTIME_OBJECTS) $(RUNTIME_DIR)/traps.obj $(RUNTIME_LIBS)
	$(PYTHON) tools/cc.py --link $@ $(RUNTIME_OBJECTS) $(RUNTIME_DIR)/traps.obj -- $(LDFLAGS) /MAP $(LDLIBS)

# Probe link: its unresolved symbols become named traps for the real link above.
$(RUNTIME_DIR)/traps.c: $(RUNTIME_OBJECTS) $(RUNTIME_LIBS) tools/traps.py
	$(PYTHON) tools/cc.py --link --allow-unresolved $(RUNTIME_DIR)/probe.exe $(RUNTIME_OBJECTS) -- $(LDFLAGS) $(LDLIBS) > $(RUNTIME_DIR)/probe.log
	$(PYTHON) tools/traps.py $(RUNTIME_DIR)/probe.log $@

$(RUNTIME_DIR)/traps.obj: $(RUNTIME_DIR)/traps.c | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(RUNTIME_DIR)/saga_runtime.obj: runtime/saga_runtime.c config/toolchain.json Makefile tools/cc.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(RUNTIME_DIR)/obj/%.obj: src/%.cpp src/saga.h src/externals.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

$(RUNTIME_DIR)/obj/%.obj: src/%.c src/saga.h src/externals.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
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

build/obj/%.obj: src/%.cpp src/saga.h src/externals.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/obj/%.obj: src/%.c src/saga.h src/externals.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
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
	rm -rf build/obj build/objdiff build/runtime
	rm -f $(EXE)
