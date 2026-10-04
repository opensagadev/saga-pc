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
EXE := build/LEGOStarWarsSaga.exe
D3DX_LIB := build/d3dx9_35.lib
CPPFLAGS := /Isrc /Ibuild/tools/msvc8/include /Ibuild/tools/msvc8/PlatformSDK/Include
CXXFLAGS := /nologo /c /O2 /Gy /GS /MT
LDFLAGS := /nologo /machine:x86 /subsystem:windows /incremental:no /manifest:no /opt:noref /opt:noicf
LDLIBS := user32.lib winmm.lib $(abspath $(D3DX_LIB))

.DEFAULT_GOAL := build
.DELETE_ON_ERROR:
.PHONY: all build objects link run setup editor check check-core lint check-objects test match diff inspect clean

all: build
build: link
objects: $(OBJECTS)
link: $(EXE)
run: $(EXE)
	cd res && WINEPREFIX="$(CURDIR)/build/tools/wine" WINEDEBUG=-all WINEDLLOVERRIDES="winemenubuilder.exe=d" wine ../$(EXE)

# Directory dependencies also relink when a source is removed from the input list.
$(EXE): $(OBJECTS) $(D3DX_LIB) $(SOURCE_DIRS) config/toolchain.json Makefile tools/cc.py tools/lib.py
	$(PYTHON) tools/cc.py --link $@ $(OBJECTS) -- $(LDFLAGS) $(LDLIBS)

$(D3DX_LIB): tools/d3dx9_35.def tools/cc.py Makefile
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
	rm -rf build/obj build/objdiff
	rm -f $(EXE)
