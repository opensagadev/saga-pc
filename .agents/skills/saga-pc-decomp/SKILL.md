---
name: saga-pc-decomp
description: Work on saga-pc's matching decompilation of the GOG PC executable, including symbol identification, MSVC 2005 source reconstruction, objdiff comparisons, tool setup, and build checks. Use for changes in this repository.
---

# saga-pc matching decompilation

Read [CONTRIBUTING.md](../../../CONTRIBUTING.md) for setup, commands, compiler
constraints and required validation. Use [config/symbols.md](../../../config/symbols.md)
for mapping rules and identification evidence. Run commands from the repository root.

## Architecture

- This project builds PC x86 MSVC objects for matching. Full executable relinking,
  host game builds and WASM game builds are outside the current implementation.
- GNU Make owns dependencies. Use `make -j"$(nproc)"` for bounded parallel builds.
  The order-only Wine `.ready` target initializes its prefix before compiler jobs.
- Four commands live in `tools/`: setup downloads pinned tools, cc invokes MSVC,
  check validates the project/objects, and match compares retail and updates reports.
  Reuse `tools/lib.py` for configuration, symbol maps, hashes and coverage, and
  `tools/binary.py` for PE/COFF extraction, relocation and comparison objects.
  Do not introduce another relocation implementation or build orchestrator.
- `src/saga.h` contains all shared declarations and layout checks. Guard C linkage
  with `__cplusplus` and hide C++-only declarations from C. `.c` and `.cpp` units
  exercise both compiler front ends. Preserve C++ linkage where the map uses it.
- Use VC8's native C++98/03-era dialect with MS extensions. It has no `/std` switch;
  do not infer full C++03 conformance from the era or impose an artificial strict
  C++98 mode. Follow constructs accepted by the pinned compiler and check output.
- `config/project.json`, `toolchain.json` and `tools.json` are actual machine inputs.
  They pin the retail image bounds/hash, compiler and objdiff respectively.

## Mapping and source work

`config/symbols.csv` is independent of source annotations. It has exactly four
comma-aligned columns: `address`, `symbol`, `size`, `ctype`. Addresses are preferred
image VAs, not RVAs or file offsets. The symbol is our exact reconstructed linker
name. Ctype records the recovered C/C++ signature or data type; actual COFF metadata
supplies code/data classification. Leave unknown sizes blank until established.

Use constants, calls, strings, neighbors, shared state and vtables to identify
anchors. Android source is a reference; its layout, ABI and names do not prove PC
identity. Record evidence and uncertainty in `config/symbols.md`, preserve Android
attribution, and do not modify the separate Android repository as part of PC work.

Keep the CSV sorted by original address. Add a mapping for every compiled function
and data definition, including private globals, literals and vtables. The matcher
finds objects by their current source files and exact linker names. Multiple copies
must all match. Names mapped to different VAs cannot resolve a reference unambiguously;
use distinct reconstruction names for unrelated file-local entities.

## Required verification

Run `make check` and `make -j"$(nproc)" check-objects`. These require no original:
static map/layout/input checks, synthetic tests, objdiff fixture integration after
setup, compilation, complete symbol coverage and resolvable supported relocations.
Unknown inventory entries are allowed; compiled definitions need known original
sizes. Different rebuilt sizes are allowed for work in progress, not proof of matching.

After source, map, compiler or tooling changes, **run `make -j"$(nproc)" match`
locally with the supported retail executable**. Inspect affected differences with
`make -j"$(nproc)" diff FUNC=Name` (C shorthand, exact linker name or VA). Refresh
`matching.json` and README badges before claiming matches. If retail is unavailable,
report matching as unverified; do not invent or refresh comparison results.

## Meaning of results

Retail PE functions are extracted into local COFF comparison views for objdiff.
Candidate bytes come independently from compiled functions with real DIR32/REL32
relocations applied through the map. Extents stay complete; no masking, truncation
or bytes copied from retail into the candidate. Unsupported cases stop comparison.
Data comparison checks initializers, including BSS, not runtime state.

Only full byte equality and equal extents establish `exact_match`. Objdiff's advisory
score can reach 100% for different external branch destinations in these views.
Matching progress counts the union of exact function ranges over the full original
`.text` virtual size. Mapping progress counts all known-size symbol ranges, including
data and unimplemented entries, over PE `SizeOfImage`. Unknown sizes contribute
nothing; overlapping aliases count once. Mapping includes image gaps/headers/padding
in its denominator and is not a count of identified functions. Never alter bounds
or bindings simply to improve a metric.

CI installs pinned tools and runs the original-free checks. PR comments update one
bot comment from committed report snapshots; those reported numbers do not prove
retail matching. The comment workflow reads PR data without checking out or running
PR code with its write token.

Original input belongs at ignored `res/LEGOStarWarsSaga.exe` (symlinks work).
All generated state is under ignored `build/`: tools and Wine in `build/tools/`,
compiled objects in `build/obj/`, comparisons in `build/objdiff/`. `make clean` keeps
downloaded tools. Never commit originals, game assets, tool binaries or extracted
retail bytes. The tracked report contains hashes and metrics only.

## AI commit attribution

Every AI-assisted commit must credit the contributing AI tool as a co-author
using its verified GitHub account identity. Preserve the human author and any
existing co-author trailers; add one trailer per contributing AI account.

For [Codex](https://github.com/codex), use:

```text
Co-authored-by: Codex <267193182+codex@users.noreply.github.com>
```

For other AI tools, verify their GitHub account and associated commit email
before adding the trailer; do not invent an account or attribution.
