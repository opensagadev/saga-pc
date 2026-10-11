# Contributing

saga-pc reconstructs the GOG PC executable of *LEGO Star Wars: The Complete
Saga* with the original compiler, MSVC 2005 (VC8), and compares each function
against the retail binary. Most game code is ported from the
[Android decompilation](https://github.com/opensagadev/saga).

## Setup

Linux x86-64 with Python 3.11+, GNU Make, Git, Wine (32-bit), LLVM's
`llvm-dlltool`, and clang-format/clang-tidy 18. On Ubuntu 24.04:

```sh
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install --no-install-recommends \
    ca-certificates git make python3 wine wine64 wine32:i386 clang-format-18 clang-tidy-18 llvm-18
python3 -m pip install libclang        # only for tools/permute.py
make setup                             # pinned MSVC 2005 + objdiff-cli into build/tools/
make check
make -j"$(nproc)" build check-objects
```

`make setup` verifies every compiler, header, library and objdiff hash. For
compiler-only work use `python3 tools/setup.py --compiler-only`; an existing
pinned archive can be passed with `--archive`.

## Original executable

Put your own `LEGOStarWarsSaga.exe` in `res/` (a symlink works). The supported
file is 5,111,808 bytes with SHA-256
`239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113`.
Never commit original game files, assets, compiler binaries or extracted objects.

## Workflow

1. Map the function in [config/symbols.csv](config/symbols.csv) (address,
   linker name, size, type; see [config/symbols.md](config/symbols.md)).
2. Implement it under `src/`; shared types, calling conventions and
   `STATIC_ASSERT` layout checks go in [src/saga.h](src/saga.h). APIs missing
   from the bundled SDK go in `src/externals.h`.
3. Compare: `make -j"$(nproc)" diff FUNC=NuRand` (C name, exact linker symbol
   or original address).
4. Run `make -j"$(nproc)" match` to refresh `matching.json` and the README
   badges, then commit.

The PC executable is authoritative for identity, ABI, constants, layouts and
control flow. Android bodies are the starting point, not the answer. Use named
fields with asserted PC offsets, never byte-offset casts. `STUBBED()` /
`STUBBED_NAMED(...)` mark a verified PC call whose dependency is still missing;
they emit no code.

`make check` runs formatting, clang-tidy (Windows x86 target against the pinned
SDK headers), project checks and unit tests. `check-objects` requires every
compiled definition to have a mapping or to be listed in
[config/candidate-only.json](config/candidate-only.json) (compiler-generated
helpers with no known original address, which earn no credit). CI runs
everything that needs no original files; matching runs locally only.

## Matching VC8 from Android source

Android sources were shaped until clang reproduced the Android binary. Clang
optimizes much more than VC8, so many source spellings collapse to the same
clang output while VC8 compiles them differently. Copied bodies therefore often
score 50-70% for reasons of spelling alone. Typical fixes:

- Read globals and fields directly instead of through cached locals; VC8 reloads
  them and clang had removed the reloads (`transposeClipPlanes`: 62% -> exact).
- Turn `== n` chains back into the range checks clang folded them from
  (`if (count < 5) return;`).
- Prefer one result variable and a shared `return`, and single expressions over
  split statements (`CalcValue1648`: 64% -> 96%).
- Keep the source block order: VC8 lays out the `if` body as the fall-through.
- Keep functions in the translation unit of their callers. The Rich header has
  no LTCG objects, so a callee that receives arguments in registers was `static`
  in the caller's file (`ZapCode`: 0% -> 74% after moving it next to `MoveCode`).

### Automated search

`tools/permute.py` applies such rewrites (located with libclang, in
`tools/rewrite.py`) to one function, compiles each candidate with VC8 and scores
it with the matcher's relocation code and objdiff. No model is involved.
Rewrites that are not semantics-preserving in general (statement reordering,
`==` to `<` without a proven guard) are kept only when they make the function
byte-exact.

```sh
python3 tools/permute.py transposeClipPlanes                 # one function, report only
python3 tools/permute.py --batch 50 --workers 4 --jobs 4 --apply
tools/permute_loop.sh                                         # forever; pushes to main
```

Improvements go to `build/permute/<symbol>.patch`; `--apply` writes them into
`src/`. Batch runs record results in `build/permute/batch.jsonl` and skip
functions already recorded. Workers are separate processes sharing the build's
Wine prefix. `permute_loop.sh` pulls `main` each round, searches the
next functions, commits and pushes improved sources (the journal stays in `build/permute/`),
and after a full pass re-measures with `make match` and starts a new pass.

## Runtime library

The original links the VC8 runtime statically (`/MT`), and the pinned toolchain
ships the same `libcmt.lib`/`libcpmt.lib`. `python3 tools/libmatch.py --write`
finds archive functions that occur once in unmapped retail code, derives their
relocation targets from the retail bytes, and keeps only functions that then
compare byte-exact. It adds them and their references to `config/symbols.csv`
and records the members in `config/library-objects.json`; `match.py` compares
those members like compiled sources. Rerun it after mapping changes.

## Mapping with call-site alignment

`python3 tools/callmap.py --android ../saga --write` names unmapped retail
functions from the public Android decompilation. For each mapped function with
an Android definition, it aligns the Android callee order with the retail call
targets (mapped callees anchor the alignment) and accepts names supported by
two callers, or by one caller with anchors on both sides. Extents follow the
retail control flow, and sizes far from the Android size are rejected. Rerun
until it finds nothing new; each round's names anchor the next.

## Measures

`matching.json` records, per function, exact equality and objdiff similarity.
The fuzzy badge is objdiff-weighted coverage of the whole original `.text`
(3,471,246 bytes); unimplemented code counts as zero. Exact coverage counts
byte-identical functions. Mapped code/data badges count known symbol extents
against the code (3,471,246) and data (36,018,966) section sizes. None of these
measure playability.

## Compiler and layout

The pinned compiler is VC8 14.00.50727.42 with `/O2 /Gy /GS /MT`. Use VC8's own
C++98/03-era dialect; `.c` files use the C front end, `.cpp` files C++. The
Rich header lists 199 C and 282 C++ objects and no LTCG.

```text
config/        Target and tool pins, symbol map, data members, code groups
src/saga.h     Shared declarations and layout checks
src/nu2api/    Engine, resources, rendering and Windows platform services
src/legoapi/   Characters, movement, world loading, menus and gameplay
src/gameapi/   Objects, AI, scripts and collision
src/audio/     Ogg and original libvorbis 1.2.0
tools/         setup.py, cc.py (VC8 via Wine), check.py, match.py, permute.py
build/         Objects, linked EXE, objdiff reports, tools (ignored)
```
