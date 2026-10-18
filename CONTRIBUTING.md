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

## Build kinds and logging

There are exactly two build kinds, **Matching** and **Debug**:

```sh
make                                      # BUILD_KIND=Matching, the default
make Matching                             # same build
make Debug                                # BUILD_KIND=Debug
make BUILD_KIND=Debug run                  # explicitly run the Debug executable
```

The executables are `build/LEGOStarWarsSaga.exe` (Matching) and
`build/LEGOStarWarsSaga_debug.exe` (Debug). Objects stay in `build/obj/` and
`build/Debug/obj/`; Debug uses embedded CodeView
symbols in its objects and an isolated linker map. The two kinds use the same source, `/O2 /Gy /GS /MT`, native linker
optimization settings and unresolved-symbol policy. Debug adds `/Z7`, linker
`/MAP`, and `SAGA_DEBUG` for diagnostic support. Its PE uses the console subsystem
with an explicit `WinMainCRTStartup` entry so Wine terminal launches inherit working
stdout; the native `WinMain` startup path stays the same. Matching retains the GUI
subsystem. Debug preserves inherited terminal, pipe and file redirection, and does not define
`DEBUG` or `_DEBUG`, select a debug CRT, or change assertion policy.
The pinned PDB writer rejects `/DEBUG` with LNK1101, so Debug does not emit a
PDB; its object symbols and map are the supported symbol outputs.
`match`, `diff` and `check-objects` require Matching and never consume Debug
objects. `make clean` removes the managed outputs of both kinds, leaving legacy
investigation archives alone.

**Only diagnostics may differ, except for the user-requested Debug save location.**
Debug initializes the save-path buffers to `SavedGames` under the required `res`
working directory; Matching retains the native profile-folder behavior.
Apart from that save-path exception, both kinds must share native game behavior,
including initialization, required calls, correctness, error handling, bugs,
crashes and incomplete paths. Never use a build-kind `#ifdef` to add guards,
crash avoidance, fallbacks, return values, or Matching-only real logic. Fixes
belong in both kinds. Never replace a required native call with a diagnostic
macro. Original retail logging and error functions remain native code in both
kinds. The project check rejects build-kind conditionals in game source; the
private selector belongs only in `src/diagnostics.h`.

The logging conventions follow the sibling saga repo's `src/decomp.h`:
`LOG(level, ...)`, `LOG_ERR`, `LOG_WARN`, `LOG_INFO`, `LOG_DEBUG`,
`LOG_INFO_IF`, `LOG_WARN_ONCE`, `STUBBED()` and `UNIMPLEMENTED(...)`.
`STUBBED_NAMED(description)` retains existing PC call-site descriptions.
Console output uses saga's red errors, yellow warnings and green INFO messages,
restoring the terminal's original attributes after each record. Redirected stdout,
pipes and log files stay plain text. No environment flag is needed for default logs.
Debug writes timestamped severity/file/line/function messages simultaneously to
stdout and a best-effort `log/` directory beside its executable (`build/log/` in
the normal layout), filtering above INFO as saga does. Each run creates a fresh
file named with local timestamp, git revision, deterministic build-input hash,
process ID and collision suffix. The startup banner identifies the build. File
creation/write failure leaves stdout active. Records are serialized across
threads and flushed immediately; stdout can also be redirected to capture Wine
crash diagnostics, for example `make BUILD_KIND=Debug run >build/human-test.log 2>&1`.
World, scene and streaming lifecycle events use INFO; missing native work
continues to use WARN/ERROR.
The first logged event also initializes a Debug-only observer for selected exception
codes. It records raw x86 fault context as **FIRST-CHANCE**, which can include handled
exceptions; a record alone does not establish a crash. It continues normal exception
routing without recovery. Up to eight records append through a duplicated log handle,
and a separate `build/log/latest-fault-*.log` file retains the latest observed fault
after that limit. The observer uses preallocated buffers and raw file writes, not the
normal logger lock. Unreadable context is marked unavailable; stack overflow is
excluded because the observer cannot guarantee handler stack space. Logging does not intercept exceptions.
WARN_ONCE and STUBBED use an atomic flag per site. The logger preserves
`errno` and Win32 `GetLastError`. STUBBED and UNIMPLEMENTED only report missing
work: they do not implement it, return a safe alternative, or suppress a crash.

All arguments, levels and conditions supplied to logging macros must be free
of side effects and valid to read. Do required work before logging and pass
values, never assignments, increments or calls that change game state. Matching
removes diagnostics completely, including argument and condition evaluation;
Debug may filter messages or emit a once-only message. Required work must not
depend on whether any message is emitted.

The former `make runtime`, `SAGA_RUNTIME` and `SAGA_RUNTIME_*` watch/capture
options are retired. Their Present hooks, thread sampler, crash interception
and unresolved-symbol exit traps are not part of Debug. Existing ignored
private executables, logs and validation archives are preserved; they are
historical artifacts, not another supported build kind.

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

Explicitly initialize globals and function statics that would otherwise receive
zero initialization (`= 0` or `= {0}`). Preserve existing nonzero initializers and
C++ constructor semantics. Never zero automatic locals merely to hide an original
bug or uninitialized read.

The PC executable is authoritative for identity, ABI, constants, layouts and
control flow. Android bodies are the starting point, not the answer. Use named
fields with asserted PC offsets, never byte-offset casts. `STUBBED()` /
`STUBBED_NAMED(...)` mark a verified PC call whose dependency is still missing;
they emit no code.

`make link` (and `make build`) link with `/FORCE:UNRESOLVED` while the
decompilation is incomplete: a ported function may call functions nobody has
written yet, as long as every reference is mapped to its retail address
(`check-objects`). The link reports how many symbols are still unresolved;
`make link-strict` is the complete link and requires none.

`make check` runs formatting, clang-tidy (Windows x86 target against the pinned
SDK headers), project checks and unit tests. `check-objects` requires every
compiled definition to have a mapping or to be listed in
[config/candidate-only.json](config/candidate-only.json) (compiler-generated
helpers with no known original address, which earn no credit). CI runs
everything that needs no original files; matching runs locally only.

### Temporary artifacts

Remove temporary files when their task or checkpoint no longer needs them. After
validation and publication, retain concise provenance summaries and any candidate,
log or capture still needed for an active investigation or human test; discard
obsolete object copies, compiler experiments and exact duplicate captures. Check
active agent references before deleting shared scratch. Preserve current builds,
pinned toolchains, active or shared Wine prefixes, original assets and user
provided evidence. Obsolete task-owned prefixes and scratch may be removed once
all owned processes have exited and no active work depends on them. Report what
was removed and what remains needed.

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

`python3 tools/calleemap.py --android ../saga --write` works the other way
round: it names an unmapped retail function by the mapped functions it calls.
Their order is compared with each unmapped Android definition's callees
(longest common subsequence); a pairing must be the best for both sides, have
at least three anchors, clearly beat the runner-up and fit the Android size.
In hold-out tests (hiding 15% of mapped functions) it re-derived 129 names with
no wrong ones. Alternate both tools until neither finds anything.

## Transferring struct fields

`python3 tools/typexfer.py --android ../saga --write` fills opaque padding in
`src/saga.h` (`unknown_*`, `opaque_*`, `reserved*`, `padding*` byte arrays)
with named fields from the matching Android structs. Structs are paired by
shared field names; Android layouts are computed with MSVC rules, and a field's
PC offset comes from the shift between layouts, which must agree on both sides.
Existing PC fields are never changed, padding that sources name or that sits in
brace-initialized or locally instantiated structs stays (VC8 code generation for
local struct instances depends on member types), and edits are dropped for any
struct whose size or existing offsets would change.

`python3 tools/typeimport.py TYPE... --android ../saga --write` imports Android
structs that `src/saga.h` has no counterpart for (nor any unit declares) into a
section of the header, with dependencies held by value, PC types for paired
members and a size assertion each; structs whose layout for the MSVC x86 target
differs from the Android one are dropped. No existing code uses them, so object
code stays the same; the scores of the functions ported with them check them.

`python3 tools/port.py NAME --android ../saga` drafts a PC port of an Android
function: struct types become their PC pairs, each field access (including
`a.b.c` chains through embedded and anonymous members) becomes the PC member at
the corresponding offset, declarations are hoisted for C units, and anything it
cannot translate is listed. `--insert` places the draft after its address
neighbour.

`python3 tools/globals.py UNIT...` then makes the unit compile and link: it
declares the globals and enum constants the draft uses (from the Android
sources, in a section of `src/saga.h`), infers each unmapped global's, string
literal's and callee's retail address by aligning the compiled function with
retail, maps them in `config/symbols.csv` and defines globals in
`src/data.cpp` with their retail initial values. Callees it maps still need an
implementation (port them the same way); ambiguous addresses are reported and
left to you.

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
