# Contributing

This project reconstructs the GOG PC executable with MSVC 2005 and compares the
resulting x86 COFF objects against the original. It currently builds individual
compilation units; a full executable relink is future work. There are no host
or WebAssembly game targets.

## Requirements and setup

The environment supports **Linux x86-64** and requires Python 3.11+, GNU Make,
Git, CA certificates, and Wine with 32-bit Windows support. On Debian 12 /
Ubuntu 24.04 or newer:

```sh
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install --no-install-recommends \
    ca-certificates git make python3 wine wine64 wine32:i386
```

On another Linux distribution, install the same prerequisites with its package
manager. Then clone and prepare the project:

```sh
git clone git@github.com:opensagadev/saga-pc.git
cd saga-pc
make setup
make check
make -j"$(nproc)" check-objects
```

`make setup` downloads checksum-pinned MSVC 2005 and objdiff-cli into ignored
`build/tools/`. For compiler-only work, use
`python3 tools/setup.py --compiler-only`; an existing pinned compiler archive
can be supplied with `--archive /path/to/archive.zip`.

[Makefile](Makefile) owns source discovery and compilation dependencies.
`make -j"$(nproc)"` builds all sources, capped at the CPU count; `make -j2` uses
a smaller limit. Wine initializes its isolated prefix at `build/tools/wine/`
once before compilation jobs run in parallel. The first build can take longer.
Compilation and tooling checks need no original game files.

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

Identify the original function or global, record its boundary and references in
[config/symbols.csv](config/symbols.csv), then implement it under `src/` and add
shared declarations to [src/saga.h](src/saga.h). Keep all shared types, calling
conventions, and layout assertions in that single header. Preserve Android source
attribution when transferring code. Record identification evidence and uncertainty
in [config/symbols.md](config/symbols.md).

Run the checks while iterating:

```sh
make check
make -j"$(nproc)" check-objects
make -j"$(nproc)" diff FUNC=NuRand
make -j"$(nproc)" diff 'FUNC=?qrand@@YAHXZ'
make -j"$(nproc)" match
```

`make check` runs static project checks and synthetic regression tests, including
an objdiff-cli roundtrip using generated fixture objects. That integration test
is skipped locally only when objdiff has not been installed; CI installs it.
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
no original game files: `make check` followed by
`make -j"$(nproc)" check-objects`, including compilation. It installs and caches
both the compiler and objdiff, verifying their hashes on every run. CI has no
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

The badge counts the union of exactly matched function bytes divided by the
full original `.text` virtual size (**3,471,246 bytes**). It includes unmapped
code in the denominator, counts aliases once, and excludes data. The initial
inventory contains 13 implemented functions and six data/literal entries; it is
far from complete. The separate **mapping** badge counts the union of all known
symbol extents, including data and symbols without implementations, against the
full `SizeOfImage` VA range (**40,067,072 bytes**). Unknown sizes contribute no
bytes and overlaps count once. This can be computed without the retail file;
`make check` prints current coverage. The full image is the 100% reference and
includes headers, section gaps, alignment and padding, so mapping is raw address
coverage rather than a claim that every function is identified. At present,
1,496 bytes are mapped (**0.004%**).

[matching.json](matching.json) records local results and input hashes, with no
original bytes. `make clean` removes generated object and
comparison files while preserving `build/tools/`.

## Compiler and project layout

The PE linker version (8.0) and Rich-header compiler build (50727) identify the
Visual C++ 2005 family. The pinned x86 compiler is **14.00.50727.42 (RTM)**;
metadata alone does not distinguish all VC8 service-pack variants. It reproduces
the initial functions with `/O2 /Gy /GS-` and the x86 default calling convention
and floating-point settings. Other code may require additional evidence and
per-file settings as reconstruction grows.

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
from C. The angle unit is compiled as C, while the random units compile as C++,
so every build exercises the shared header in both languages.

The initial random-number and angle code comes from the Android project. Most
bodies transfer unchanged; `NuAngAdd` and `NuAngSub` use the equivalent
`>= 0x8000` spelling to reproduce VC8's comparison instruction. Functions retain
C or C++ linkage as appropriate, including mangled `qrand`.

```text
config/            Target/tool pins, symbol inventory and identification notes
src/saga.h         All shared declarations and layout checks
src/nurand.cpp     Random-number routines
src/nuang.c        Angle routines
src/qrand.cpp       C++ random-number routine and state
res/               Your original executable (ignored)
tools/             Helpers, shared libraries, and test_*.py checks
build/obj/         Compiled MSVC objects (ignored)
build/objdiff/      Local comparison objects and reports (ignored)
build/tools/       Downloaded msvc8/, objdiff in bin/, Wine in wine/ (ignored)
```

The four command-line helpers are `tools/setup.py` for tool downloads,
`tools/cc.py` for invoking MSVC through Wine, `tools/check.py` for project and
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
