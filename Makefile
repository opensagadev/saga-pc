PYTHON ?= python3

SOURCES := $(shell find src -type f \( -name '*.cpp' -o -name '*.c' \) | LC_ALL=C sort)
SOURCE_DIRS := $(shell find src -type d)
OBJECTS := $(patsubst src/%,build/obj/%,$(SOURCES:.cpp=.obj))
OBJECTS := $(OBJECTS:.c=.obj)
EXE := build/LEGOStarWarsSaga.exe
CPPFLAGS := /Isrc /Ibuild/tools/msvc8/include /Ibuild/tools/msvc8/PlatformSDK/Include
CXXFLAGS := /nologo /c /O2 /Gy /GS- /MT
LDFLAGS := /nologo /machine:x86 /subsystem:windows /incremental:no /manifest:no /opt:noref /opt:noicf
LDLIBS := user32.lib

.DEFAULT_GOAL := build
.DELETE_ON_ERROR:
.PHONY: all build objects link setup check check-core lint check-objects test match diff inspect clean

all: build
build: link
objects: $(OBJECTS)
link: $(EXE)

# Directory dependencies also relink when a source is removed from the input list.
$(EXE): $(OBJECTS) $(SOURCE_DIRS) config/toolchain.json Makefile tools/cc.py tools/lib.py
	$(PYTHON) tools/cc.py --link $@ $(OBJECTS) -- $(LDFLAGS) $(LDLIBS)

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
