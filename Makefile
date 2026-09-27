PYTHON ?= python3

SOURCES := $(shell find src -type f \( -name '*.cpp' -o -name '*.c' \) | LC_ALL=C sort)
OBJECTS := $(patsubst src/%,build/obj/%,$(SOURCES:.cpp=.obj))
OBJECTS := $(OBJECTS:.c=.obj)
CPPFLAGS := /Isrc
CXXFLAGS := /nologo /c /O2 /Gy /GS-

.DEFAULT_GOAL := build
.DELETE_ON_ERROR:
.PHONY: all build setup check check-objects test match diff inspect clean

all: build
build: $(OBJECTS)

build/obj/%.obj: src/%.cpp src/saga.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

build/obj/%.obj: src/%.c src/saga.h config/toolchain.json Makefile tools/cc.py tools/lib.py | build/tools/wine/.ready
	$(PYTHON) tools/cc.py $< $@ $(CPPFLAGS) $(CXXFLAGS)

# Serialize first-time Wine setup, then compile distinct objects concurrently.
build/tools/wine/.ready:
	mkdir -p build/tools
	WINEPREFIX="$(CURDIR)/build/tools/wine" WINEDEBUG=-all WINEDLLOVERRIDES="winemenubuilder.exe=d" wineboot -i
	touch $@

setup:
	$(PYTHON) tools/setup.py

check:
	$(PYTHON) tools/check.py
	$(PYTHON) -m unittest discover -s tools -p 'test_*.py'

check-objects: build
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
