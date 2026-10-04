# Contributing

This project reconstructs the GOG PC executable with MSVC 2005 and compares the
resulting x86 COFF objects against the original. The default build also links
our current sources into a Windows executable using the original MSVC linker
and libraries. The entry path opens the PC episode archives, then exits before permanent
resources or a level are loaded because PC input initialization is missing.
Configuration parsers exist, but their presence does not complete resource/world
loading. The executable is not playable.
There are no host or WebAssembly game targets.

## Requirements and setup

The environment supports **Linux x86-64** and requires Python 3.11+, GNU Make,
Git, CA certificates, Wine with 32-bit Windows support, LLVM's `llvm-dlltool`,
and clang-format / clang-tidy 18 or newer. CI uses LLVM 18. On Ubuntu 24.04:

```sh
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install --no-install-recommends \
    ca-certificates git make python3 wine wine64 wine32:i386 clang-format-18 clang-tidy-18 llvm-18
```

On another Linux distribution, install the same prerequisites with its package
manager. Then clone and prepare the project:

```sh
git clone git@github.com:opensagadev/saga-pc.git
cd saga-pc
make setup
make check
make -j"$(nproc)" build check-objects
```

`make setup` downloads checksum-pinned MSVC 2005, its linker, and objdiff-cli
into ignored `build/tools/`. The compiler installation includes its original CRT/STL headers
and libraries and the bundled Windows Platform SDK. Their complete extracted
trees are verified as well as the compiler/linker executables. Wine runs the tools;
the build does not use Wine headers. For compiler-only work, use
`python3 tools/setup.py --compiler-only`; an existing pinned compiler archive
can be supplied with `--archive /path/to/archive.zip`.

[Makefile](Makefile) owns source discovery, compilation and linking dependencies.
`make -j"$(nproc)"` builds `build/LEGOStarWarsSaga.exe`, capped at the CPU count;
`make -j2` uses a smaller limit. `make -j"$(nproc)" objects` compiles only the
individual objects, and `make -j"$(nproc)" link` explicitly builds the executable.
Wine initializes its isolated prefix at `build/tools/wine/` once before compiler
jobs run in parallel. The first build can take longer.

Linking uses our objects, the static Microsoft CRT (`/MT`), and bundled SDK import
libraries. It needs no original EXE or game DLLs. The Windows subsystem uses the
CRT's normal `WinMain` startup; unresolved references fail the link. `/OPT:NOREF`
and `/OPT:NOICF` keep compiled routines even when the stub does not call them, and
avoid folding distinct identical routines. `llvm-dlltool` makes an import library
for `D3DXMatrixMultiply` from its export declaration; no DLL contents are bundled.
The same tool generates `build/dsound.lib` from `tools/dsound.def`, importing
actual `DSOUND.dll` ordinal 11 for `DirectSoundCreate8`; no audio implementation
or DLL contents are bundled.
The factory bootstrap also generates `build/d3d9.lib` from `tools/d3d9.def`
for the real `d3d9.dll` export `Direct3DCreate9`. This import supplies no graphics
implementation; the bundled Platform SDK does not include that import library.
Input producers generate `build/dinput8.lib` and `build/xinput1_3.lib` from their
export declarations for real DirectInput creation and XINPUT1_3 ordinal 4
(capabilities). The link also uses bundled Shell32, Ole32 and OleAut32 import
libraries for actual folder and COM services. These libraries supply no replacement
implementation or bundled DLL contents.
The linked image has its own layout;
matching still compares individual objects at the original mapped addresses.
As source begins calling additional APIs, supply their import libraries at link
time and their DLLs at runtime. Declarations alone introduce no DLL dependency.

## Original executable

Place your own `LEGOStarWarsSaga.exe` in `res/` (a symlink works). The supported
executable is **5,111,808 bytes**, PE32 i386, with preferred image base
`0x00400000` and this SHA-256:

```text
239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113
```

[config/project.json](config/project.json) records the target fingerprint and
code-section extent. Matching refuses a different executable. Run `make inspect`
to inspect your local file. Never commit original game files, assets, downloaded
compiler binaries, or extracted comparison objects. Local inputs and outputs
are ignored; see [res/README.md](res/README.md).

## Development and validation

Prioritize the overall PC program structure before tuning individual instruction
sequences. Map connected function families and data layouts, then port portable
Android logic only where the PC call graph, constants, and behavior support it.
Complete real dependencies so the source executable continues to link. A
ported function may remain byte-different while its PC-specific control flow or
types are being recovered; record that honestly in the matching report. Once a
family is structurally present, use complete byte comparisons to refine it.
Whole-code Windows fuzzy coverage measures code similarity; exact code coverage
separately records complete byte equality. Neither measures playable completion.
Track observed process/window, installed scene/world loading and recurring gameplay
separately; a passing build or function test does not establish those milestones. Android's percentage
does not transfer directly across compilers and platform backends.

Select transfer batches by **original PC code bytes unlocked per shared layout
and dependency**, rather than function count or ease of exact matching. Start
with complete portable families that use established types, then recover one
shared object layout to enable several larger callers. Review a family's PC
entries, calling conventions, accessed fields, literals and call closure together
before splitting source ownership among agents. Send one combined header/map/data
proposal early, and integrate it once. Use the retained graph ledger to select
that batch; avoid rebuilding analysis exports without changed inputs.

Bring real control flow and dependency behavior into the Windows build first.
Stop instruction tuning once comparison confirms the intended PC structure and
identifies remaining differences; only fix those immediately when they reveal
a wrong ABI, layout, constant or behavior. Batch compiler literals and private
helper decisions with the implementation: VC8 may emit even `__forceinline` C
helpers as separate functions, so source abstractions must not introduce
fictitious standalone original mappings. Expand verified inline operations in
place when necessary. Measure each checkpoint by newly compared original code
bytes and whole-code fuzzy/exact deltas; inventory-only rows and stub markers
are not transferred implementations.

Startup code may use `STUBBED()` or `STUBBED_NAMED(...)` to mark a verified PC
call or state transition whose dependency is still missing. These markers emit
no code in matching builds and do not imply a byte match. Keep the original call
order, document the skipped work, and replace markers down the dependency tree.
The source-only executable currently exits at the missing input initialization
guard before permanent and world loading. Real window/device/input producers,
resource fixups and the recurring update/render loop remain incomplete. Its DAT metadata parser is
structural and has additional short-read and null-arena guards; those are
source-runtime safeguards, not retail byte matches.

Identify the original function or global, record its boundary and references in
[config/symbols.csv](config/symbols.csv), then implement it under `src/` and add
shared declarations to [src/saga.h](src/saga.h). Keep shared game types, calling
conventions, and `STATIC_ASSERT(...)` layout checks there. Source units may be
grouped into subsystem directories; keep `saga.h` and `externals.h` directly in
`src/`. The independently reviewed original libvorbis 1.2.0 header catalog
stays local in `src/audio/vorbis/`; its private macros do not enter shared game
declarations. The original textual lookup table is `lookup.inc`, included by
its genuine owner and checked by formatting and that owner's static analysis.
The build tracks those private headers and table as dependencies of the library
owners. Other project or vendor headers remain rejected. Use `.c` for C routines
and `.cpp` where C++ linkage or features are needed. APIs absent from the bundled Microsoft
headers have declarations in `src/externals.h`, included by `saga.h`; do not copy
SDK declarations into it. The PC retail executable is authoritative: never
transfer Android-specific behavior, constants or layouts. Check every transferred
routine against the Windows build and original PC bytes. Keep source attribution
and identification evidence in the documentation instead of repeated file-header
comments. Record identification evidence and uncertainty
in [config/symbols.md](config/symbols.md).

Keep related C routines in one subsystem translation unit, such as `nuframe.c`,
`nusound.c`, or `nuvideo.c`. Use named aggregate fields with asserted PC offsets;
do not access members through byte-offset casts. Recompile and compare complete
functions after moving code between units, since MSVC may inline or fold calls.

See [the identification guide](config/identification.md) for recovering PC
function boundaries, matching Android call/data relationships, and defining
typed data. It includes a worked seed-initialization example and explains
which analysis results still need review before entering the symbol map. The
[data inventory](config/data.md) documents shared definitions and remaining gaps;
[the NuMain queue](config/numain.md) keeps unconfirmed call/data candidates separate.

Run the checks while iterating:

```sh
make check
make -j"$(nproc)" build check-objects
make -j"$(nproc)" diff FUNC=NuRand
make -j"$(nproc)" diff 'FUNC=?qrand@@YAHXZ'
make -j"$(nproc)" match
```

`make check` runs formatting, static analysis, project checks and synthetic
regression tests, including
an objdiff-cli roundtrip using generated fixture objects. That integration test
is skipped locally only when objdiff has not been installed; CI installs it.
`make lint` runs both clang-format and clang-tidy with the committed configurations.
Named LLVM 18 executables are preferred; set `CLANG_FORMAT` / `CLANG_TIDY` to
select installed binaries. Missing tools and reported violations fail the check.
Use LLVM 18 for the same formatting version as CI. `make check-core` runs the
remaining project/tests portion after lint has already passed.

Clang analyzes the Windows x86 target with MSVC compatibility and the verified
original CRT/SDK headers. A generated case-insensitive VFS overlay in `build/lint/`
handles Windows header spelling on Linux. It uses no host or Wine headers and
has no effect on MSVC compilation or matching. The legacy math header's `_Complex` parameter identifier is renamed only for
Clang analysis because modern Clang treats it as a keyword. Two obsolete SDK intrinsic names
are aliased only during analysis to avoid conflicts with modern Clang builtins.
The SDK's required `_WIN32_WINNT` macro spelling and fundamental integer alias
definitions have narrowly scoped lint suppressions; other source policy remains
enforced. Clang is a source-analysis tool here, not the matching compiler.

For VS Code, `make setup` also prepares the editor inputs; run `make editor`
again after adding sources or changing SDK includes. This generates
`build/editor/compile_commands.json` for the clang-tidy extension with the same
Windows x86 target and language flags as `make lint`. Headers have
explicit C++ analysis entries so opening `saga.h` directly also works.
Both editor extensions use generated header symlinks in
`build/editor/include/` with the filename spellings used by the source and SDK.
This handles includes such as `windows.h` versus `Windows.h` on Linux without
changing the checksum-pinned SDK. The committed workspace settings keep indexing
inside the source/header paths and exclude Wine's links to the host filesystem.

The compiler integration tests exercise C/C++ headers, SDK declarations and a
source-only EXE link and execution under Wine, including unresolved-reference
failure. These checks require the pinned tools; CI always installs them.
The PR-comment workflow is exercised against a mocked GitHub API using Node.js;
install Node.js to run that check locally. It is required and available in CI.
`check-objects` first builds all current sources, then validates their COFF
objects without the retail executable. Every compiled function or data symbol
must be represented in the map, including private globals and compiler literals.
The checks cover ambiguous definitions, symbol bounds, supported relocation
kinds, and resolvable relocation targets. They do not require candidate code or
data to have the same size or bytes as retail; unfinished implementations can
pass these checks and still produce useful local diffs.

**After source, symbol-map, compiler, or tool changes, run
`make -j"$(nproc)" match` locally against the supported executable.** Inspect the
affected diffs and each `exact_match` result, and refresh the tracked report and
README badge before claiming a match. Do not change addresses, boundaries, or
bindings merely to make a mismatch disappear. If the executable is unavailable,
report compilation and check results explicitly as **matching unverified**;
do not fabricate or refresh a matching report without the original.

[GitHub Actions](.github/workflows/build.yml) runs all validation that requires
no original game files: a separate `make lint` step, `make check-core`, then
`make -j"$(nproc)" build check-objects`, including executable linking. It installs
and caches the compiler, linker, SDK and objdiff, verifying their hashes on
every run. CI has no
retail executable, so passing CI does not establish matching or refresh reports.

The [progress workflow](.github/workflows/progress.yml) updates one bot comment
on each PR with the matching and mapping changes between the committed base and
PR reports. These are reported local results; CI cannot verify the retail
comparisons. The workflow only reads report data through GitHub's API and does
not execute PR code with its write token. It becomes active once the workflow
is on the default branch.

## Symbol map and comparison

The stripped executable has no useful function names. The map is independent
of reconstructed source and has exactly four columns: `address`, `symbol`,
`size`, and `ctype`. `symbol` is the exact expected MSVC linker name, including
its decoration. The first three fields follow
[reccmp's CSV meanings](https://github.com/isledecomp/reccmp/blob/master/docs/csv.md);
`ctype` records the recovered C/C++ type. Our reader accepts aligned whitespace
around fields. See [the map reference](config/symbols.md) for format rules and
identification notes.

The matcher discovers definitions in objects built from the current `src/`
files. A map entry without a definition remains unimplemented. If several
objects define the same mapped symbol, every definition must match. COFF symbol
and section metadata distinguish functions from data. A linker name mapped to
different original addresses is ambiguous and cannot resolve a relocation.

The PC `/O2` build performs substantial inlining. When a helper is identified
inside a caller but has no established standalone PC entry, use `inlined` in the
CSV address column and leave its size blank. Keep its name and type for reference;
it contributes no address binding, mapping bytes or standalone matching result.
Do not give the helper the address of an instruction inside its caller. Record
the callsite evidence in [the identification guide](config/identification.md).

`FUNC` accepts an exact symbol, a simple undecorated C name such as `NuRand`, or
an original hexadecimal address. For C++, use the decorated symbol or address:

```sh
make -j"$(nproc)" diff 'FUNC=?qrand@@YAHXZ'
make -j"$(nproc)" diff FUNC=0x0054fa60
```

The matcher reads a complete compiled function, applies its relocations through
the map, and places it and the untouched retail bytes in separate named COFF
comparison objects at the original address.
[objdiff-cli](https://github.com/encounter/objdiff) compares these x86 COFF
objects; the pinned CLI does not load the retail PE directly. Objects,
disassembly, and local reports live under `build/objdiff/`.

An **exact match** requires equal sizes and every byte equal after relocation.
No bytes are masked, copied from retail into the candidate, or truncated to fit.
Unknown relocation targets and unsupported formats stop comparison. Objdiff's
instruction similarity is useful feedback, but can report 100% for different
external branch destinations in these comparison objects. Inspect `exact_match`
rather than treating an objdiff percentage as proof. Mapped data definitions,
including compiler literals and zero-initialized state, are compared too.

The first badge shows **fuzzy code coverage**: each compiled function contributes
its objdiff similarity weighted by its original PC extent, divided by the full
original `.text` virtual size (**3,471,246 bytes**). Unmapped and unimplemented
code contributes zero. Overlapping aliases contribute only once, at their least
similar score. This measures fuzzy progress against the whole game, but it is an
estimate rather than a count of equal bytes: objdiff can score a wrong branch
destination as similar. `fuzzy_coverage_percent` records the same figure in
`matching.json`. The report also retains objdiff's native
`objdiff_fuzzy_percent` among compared functions, with `objdiff_total_code` as
its narrower denominator. An omitted zero score in objdiff's JSON counts as zero.

The separate `matching_percent` in `matching.json` counts the union of exactly
matched function bytes divided by the full original `.text` virtual size
(**3,471,246 bytes**). It includes unmapped code in the denominator, counts
aliases once, and excludes data. Imported slots and jump thunks are inventoried
separately and are never credited as reconstructed matches. The original `.text`
denominator remains unchanged. The **mapping** badge counts the union of all known
symbol extents, including data and symbols without implementations, against the
full `SizeOfImage` VA range (**40,067,072 bytes**). Unknown sizes contribute no
bytes and overlaps count once. This can be computed without the retail file;
`make check` prints current coverage. The full image is the 100% reference and
includes headers, section gaps, alignment and padding, so mapping is raw address
coverage rather than a claim that every function is identified. See the README
badges and `matching.json` for the current totals. Entries marked `inlined` have
no independent extent and add no bytes to either metric.

[matching.json](matching.json) records local results and input hashes, with no
original bytes. `make clean` removes the generated executable, object and
comparison files while preserving `build/tools/`.

## Compiler and project layout

The PE linker version (8.0) and Rich-header compiler build (50727) identify the
Visual C++ 2005 family. The pinned x86 compiler is **14.00.50727.42 (RTM)**;
metadata alone does not distinguish all VC8 service-pack variants. It reproduces
the initial functions with `/O2 /Gy /MT` and the x86 default floating-point
settings. The build enables `/GS`: retail stack-cookie references and an isolated
93-unit probe support it, with two additional exact candidates and no lost exact
matches. Preserve same-TU static helpers where PC callers use private register
arguments; ordinary VC8 can generate these without `/GL`. Other code may require
additional evidence and per-file settings. See [the strategy review](config/strategy.md)
and [execution plan](config/progress-plan.md) for broad source transfer.

Use **VC8's native C++98/03-era dialect with Microsoft extensions**, rather than
forcing a separate C++98 mode. This compiler has no `/std:c++03` switch; the
pinned binary reports it as an ignored option, which our wrapper rejects.
Microsoft introduced [standard-version switches in Visual Studio 2015 Update 3](https://devblogs.microsoft.com/cppblog/standards-version-switches-in-the-compiler/).
There was no published C++07 edition: the published revisions went from C++03
to C++11 ([WG21 history](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4862.pdf)).
This does not claim full C++03 conformance or prove which standard the original
authors intended. Use constructs accepted by the pinned compiler and verify the
resulting code.

`.c` files use VC8's legacy C front end; `.cpp` files use C++. `src/saga.h` guards
its shared `extern "C"` block with `__cplusplus`, and hides C++-linkage declarations
from C. The angle and C random-number units compile as C; `qrand` and `ResetSeeds` retain
C++ linkage. Every build exercises the shared header in both languages.

The initial random-number and angle code comes from the Android project. Most
bodies transfer unchanged; `NuAngAdd` and `NuAngSub` use the equivalent
`>= 0x8000` spelling to reproduce VC8's comparison instruction. Functions retain
C or C++ linkage as appropriate, including mangled `qrand`.

```text
config/            Target/tool pins, symbol inventory and identification notes
src/saga.h         All shared declarations and layout checks
src/externals.h    Declarations for APIs absent from the bundled SDK
src/externals.c    Declaration-only external dependency unit
src/batman.c       Windows entry point and partial application initialization
src/data.cpp       Verified shared PC data and constants
src/nurand.c       Random-number routines
src/nuang.c        Angle routines
src/qrand.cpp      C++ random-number routine and state
src/nu2api/        Command-line, archive and Windows platform routines
src/legoapi/       Game helpers and world initialization
src/audio/vorbis/  Original libvorbis 1.2.0 owners, local headers and BSD license
src/legogame/      Startup buffer setup
src/gameapi/       Terrain editor hooks
src/gamelib/       Wind random-number routine
res/               Your original executable (ignored)
tools/             Helpers, shared libraries, and test_*.py checks
build/obj/         Compiled MSVC objects (ignored)
build/LEGOStarWarsSaga.exe  Executable linked from our sources (ignored)
build/objdiff/      Local comparison objects and reports (ignored)
build/tools/       Downloaded msvc8/, objdiff in bin/, Wine in wine/ (ignored)
```

The four command-line helpers are `tools/setup.py` for tool downloads,
`tools/cc.py` for invoking the MSVC compiler/linker through Wine, `tools/check.py` for project and
object checks, and `tools/match.py` for retail comparison and progress. Their
shared libraries are `tools/binary.py` for PE/COFF parsing, relocations,
comparison objects, and data comparison; and `tools/lib.py` for configuration,
CSV parsing, hashes, and common paths. These shared libraries have no CLI.
Regression tests live beside the corresponding components in `tools/test_*.py`.

All three JSON configurations are consumed by the helpers:

| File | Purpose |
| --- | --- |
| [config/project.json](config/project.json) | Target fingerprint, addresses, and map location for inspection and matching. |
| [config/toolchain.json](config/toolchain.json) | Compiler archive and file checksums for setup and compilation. |
| [config/tools.json](config/tools.json) | Objdiff version and checksum for setup and matching. |

The repository [Codex skill](.agents/skills/saga-pc-decomp/SKILL.md) describes this
workflow for coding agents.

## Fast structural identification

The optional analysis commands use the same `tools/check.py` entry point and
ignored `build/analysis/` workspace. They do not change the Makefile build or
accept symbol bindings. The call/reference scorer reuses these retained inputs:

- `android-callgraph.json`: original Android ELF nodes with integer `address`,
  `size`, linker `name`, and `calls` containing `site`/`target`/`name`.
- `pc/functions.tsv`: `entry`, `min`, `max`, `body_bytes`, `ranges`, `name`.
- `pc/references.tsv`: `function`, `instruction`, `target`, `type`.
- Optional `bulk-identify/{android,pc}-data-features.json`: named data and literal
  reference features. `--refresh-features` regenerates these from the originals,
  retained `data/accepted-symbols.json`, and the current symbol map. This needs
  Python Capstone and the Android repository's retained ELF/PIC readers.

The PC TSVs are Ghidra analysis exports, not authoritative boundaries. Scoring
checks the retail hash and corroborates exported direct-call operands against
its actual instructions. Output hashes identify every supplied graph/map input.
Re-export after changing the analyzed binary or its function definitions.

```sh
python3 tools/check.py --identify --holdout
python3 tools/check.py --identify --refresh-features --android-root ../saga --holdout
```

For source-level types, inline definitions, calls and references, optionally
extract the Android AST using Python `clang.cindex`/libclang, a local `clang`
executable, and an existing NDK r8e extraction. These are analysis dependencies;
they are not compiler or CI requirements. Set the actual NDK extraction path:

```sh
python3 tools/check.py --extract-source-graph --android-root ../saga \
    --ndk-root /path/to/existing/android-ndk-r8e --jobs 8
python3 tools/check.py --identify --holdout \
    --source-graph build/analysis/source-graph/android-source-graph.json
```

The extractor scans C/C++ source-tree translation units using the Android
GNU C11/C++11 syntax modes, not the host/WASM build. Limit scope with repeated
`--input /path/to/file.cpp`; `--source-graph` selects the output path. Cached TU
results fingerprint source and SDK headers, extractor code, compiler arguments,
and Clang/libclang versions. Clean AST records
are separated from degraded parses, and indirect/unresolved calls stay explicit.
A unique compatible linker name joins clean source records to existing ELF nodes; unmatched inline
source definitions retain their source identities without invented addresses.

Review `bulk-identify/hypotheses.csv`, `anchor-crosswalk.csv`, `summary.json`,
and optional `holdout.json`. The ledger records original candidate spans,
source signatures/paths, ranked alternatives, graph/reference supports, margins
and propagation rounds. Scores are comparative heuristics, not calibrated
probabilities. The deterministic holdout hides every fourth unique accepted pair
before scoring; its recovery test is useful but biased toward already identified
functions. It cannot certify unknown identities, PC layouts or function extents.
Accept coherent groups only after the PC review in [identification.md](config/identification.md),
then port their portable behavior and run the required build/matching checks.
