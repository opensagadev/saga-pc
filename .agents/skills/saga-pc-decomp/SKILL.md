---
name: saga-pc-decomp
description: Work on saga-pc's matching decompilation of the GOG PC executable, including symbol identification, MSVC 2005 source reconstruction, objdiff comparisons, tool setup, and build checks. Use for changes in this repository.
---

# saga-pc matching decompilation

Read [CONTRIBUTING.md](../../../CONTRIBUTING.md) for setup, commands, compiler
constraints and required validation. Use [config/symbols.md](../../../config/symbols.md)
for mapping rules and identification evidence. Run commands from the repository root.
For NuMain and its dependencies, also read the [investigation queue](../../../config/numain.md)
and [data inventory](../../../config/data.md). Keep current targets and remaining gaps
in those documents; use the current source and `matching.json` for progress, rather
than treating an older handoff's counts or commit ID as current state.

## Agent coordination and continuation

- Use Luna subagents in parallel for bounded research and implementation when
  useful; keep the main agent responsible for code integration and final PC
  verification. Reuse agents, avoid nested delegation, and have the main agent
  analyze and implement concrete reconstruction work too. Use Sol for tasks
  that require deeper reasoning when it materially helps.
- Give each subagent a bounded function/dependency or data task, exclusive file
  ownership, and an explicit instruction to read this skill. Require PC identity,
  type/ABI and complete-extent evidence, proposed map/header changes, and actual
  compilation/comparison results. An unverified candidate remains a candidate.
- Assign ownership by subsystem and consolidate related C routines into one
  translation unit, such as `nuframe.c`, `nusound.c`, or `nuvideo.c`. Keep a
  separate unit only when the original codegen needs a real cross-unit call or
  the routine belongs to a different subsystem. Recompile and compare after
  consolidation because MSVC may change code generation.
- The main agent integrates `src/saga.h`, `src/externals.h`, `config/symbols.csv`,
  `matching.json` and README badges. Assign `src/data.cpp` to one writer at a time.
  Subagents hand off findings and owned source edits; the main agent runs aggregate
  checks and handles commits/pushes within the user's existing authorization.
- Inspect the working tree and reuse installed tools, existing maps, and relevant
  research under ignored `build/analysis/`. Check that scratch results concern the
  configured original before relying on them. Avoid repeating setup, broad binary
  scans or completed investigations without a concrete reason.

## Architecture

- The default build links our PC x86 sources into `build/LEGOStarWarsSaga.exe`
  using the original MSVC linker and static CRT. Its entry path is still
  incomplete; a successful link does not mean the game is reconstructed. No retail
  bytes are used to produce this EXE. Matching uses the individual COFF objects.
  There are no host game or WASM targets.
- GNU Make owns dependencies. Use `make -j"$(nproc)"` for bounded parallel builds.
  The order-only Wine `.ready` target initializes its prefix before compiler jobs.
- Four commands live in `tools/`: setup downloads pinned tools, cc invokes MSVC compiler/linker,
  check validates the project/objects, and match compares retail and updates reports.
  Reuse `tools/lib.py` for configuration, symbol maps, hashes and coverage, and
  `tools/binary.py` for PE/COFF extraction, relocation and comparison objects.
  Do not introduce another relocation implementation or build orchestrator.
- `src/saga.h` contains shared game declarations and layout checks. It includes
  `src/externals.h` for APIs missing from the bundled Microsoft SDK; use the SDK
  headers for APIs they provide. Do not use Wine headers or duplicate SDK types.
  Wine runs the compiler/linker. Setup installs the original CRT/STL and Platform
  SDK headers/libraries and checksums their complete trees.
  Use `.c` for C routines and `.cpp` when C++ linkage or features are needed.
  Guard shared C declarations with `__cplusplus` and hide C++-only declarations
  from C. Use `STATIC_ASSERT(...)` for verified sizes and offsets. Source units
  may use subsystem directories, but keep both headers directly in `src/`.
  Avoid repetitive file-header comments; keep source attribution in the docs.
  Preserve C++ linkage where the map uses it.
- Access aggregate members through named typed fields and assert their PC
  offsets with `STATIC_ASSERT`. Never cast an aggregate to bytes and access a
  numeric offset to bypass layout verification. Keep genuine C/C++ as the
  implementation; assembly copied or transcribed from retail is not a match.
- Use VC8's native C++98/03-era dialect with MS extensions. It has no `/std` switch;
  do not infer full C++03 conformance from the era or impose an artificial strict
  C++98 mode. Follow constructs accepted by the pinned compiler and check output.
- `config/project.json`, `toolchain.json` and `tools.json` are actual machine inputs.
  They pin the retail image bounds/hash, compiler and objdiff respectively.

## Mapping and source work

Prioritize broad PC program structure and coherent portable function families
before instruction-level byte tuning. Use the Android decomp as a source candidate,
then check each function against PC control flow, constants, data layout and
dependencies. Port real PC behavior even when the first build is not exact;
record the mismatch and move on to the next connected function. The final metric
is exact PC code coverage, but isolated tiny matches must not displace work on
major families. Never infer a PC percentage directly from Android's percentage.

`config/symbols.csv` is independent of source annotations. It has exactly four
comma-aligned columns: `address`, `symbol`, `size`, `ctype`. Addresses are preferred
image VAs, not RVAs or file offsets. The symbol is our exact reconstructed linker
name. Ctype records the recovered C/C++ signature or data type; actual COFF metadata
supplies code/data classification. Leave unknown sizes blank until established.

Read [config/identification.md](../../../config/identification.md) for recovering
PC boundaries and propagating Android call/data/RTTI anchors. `/O2` performs
substantial inlining: an absent call is not proof of a different identity. Use
`inlined` instead of an address, with a blank size, for an observed inline helper
without an established standalone PC entry. Such rows supply no address binding,
coverage or standalone matching result. Record callsite evidence in Markdown.
Do not copy Android sizes, C++ layouts or vtable slot numbers onto the PC.

For broad identification, use `python3 tools/check.py --identify --holdout`
against retained analysis exports, optionally adding `--source-graph` with the
clean Android AST graph. See the reproducible workflow and input schemas in
[identification.md](../../../config/identification.md). Prefer bounded family
reviews of high-ranked pairs over starting isolated searches from scratch.
Tentative pairs may propagate inside the research graph but never automatically
enter `symbols.csv`. Scores and held-out recovery are advisory; verify PC
identity, caller ABI and complete extent before accepting a row. Keep degraded
AST evidence and source-only inline definitions separate; never invent original
addresses or claim reconstruction coverage from inferred pair counts. Source
signatures describe Android candidates until checked against PC callers.

Use constants, calls, strings, neighbors, shared state and vtables to identify
anchors. The PC retail executable is authoritative. Never port Android-specific code,
constants or layouts. Android source is a reference; its layout, ABI and names
do not prove PC identity. Verify every port with the pinned Windows compiler
and original PC bytes. Record evidence and uncertainty in `config/symbols.md`, preserve Android
attribution, and do not modify the separate Android repository as part of PC work.

Keep the CSV sorted by original address. Add a mapping for every compiled function
and data definition, including private globals, literals, vtables and tentative C
common allocations. The matcher finds objects by their current source files and
exact linker names. Multiple copies
must all match. Names mapped to different VAs cannot resolve a reference unambiguously;
use distinct reconstruction names for unrelated file-local entities.

Keep speculative identities and reachable-body counts in the research notes until
PC identity, ABI and full boundaries are reviewed. For the entry-path scaffolding,
mark unreconstructed PC actions with `STUBBED()` or `STUBBED_NAMED(...)` at their
actual call sites. These macros deliberately emit no code in matching builds;
they make the source-only dependency gaps visible without adding unmapppable
logging data or fake linker definitions. Keep such functions nonmatching and
document the skipped behavior. Replace markers with verified real dependencies
down the call tree. Preserve a source-only executable that links, and describe
partial entry-point reconstruction explicitly. Shared data/constants belong in
`src/data.cpp` with PC initializer and extent evidence; retain existing private
owners and C++ linkage where required.

## Required verification

Run `make check` and `make -j"$(nproc)" build check-objects`. These require no original:
clang-format/clang-tidy policies, static map/layout/input checks, synthetic tests,
objdiff fixture integration after
setup, compilation/linking, complete symbol coverage and resolvable supported relocations.
Unknown inventory entries are allowed. Compiled functions need known original
sizes; compiled data with an unknown original extent may be mapped without a size,
but receives no coverage or exact-match credit until that extent is established.
Different rebuilt sizes are allowed for work in progress, not proof of matching.
`make objects` and `make check-objects` work independently of the EXE link. Keep
unresolved references fatal when linking. The source EXE has its own layout;
never use its addresses as original mappings. `/OPT:NOREF /OPT:NOICF` retain
unused routines and distinct identical functions in this early build.

After source, map, compiler or tooling changes, **run `make -j"$(nproc)" match`
locally with the supported retail executable**. Inspect affected differences with
`make -j"$(nproc)" diff FUNC=Name` (C shorthand, exact linker name or VA). Refresh
`matching.json` and README badges before claiming matches. If retail is unavailable,
report matching as unverified; do not invent or refresh comparison results.
Compare against the previous verified report and investigate any lost exact matches.
At a checkpoint, report newly verified functions/data, remaining differences and
one concrete next target. When pushing is authorized, verify the resulting CI run
and state its outcome; do not describe local success as a passing CI run.

## Meaning of results

The README's first badge is `fuzzy_coverage_percent`: each compared function's
objdiff score weighted by its original extent and divided by the entire retail
`.text` extent. Unmapped code is zero; overlapping aliases count once at their
least similar score. This is an estimate of whole-code progress, not proof of
equal bytes. `matching.json` also retains native `objdiff_fuzzy_percent` among
compared functions (`objdiff_total_code` denominator) and `matching_percent`:
the union of exact function bytes divided by the entire retail `.text`. Report
the denominators when discussing progress. The mapping badge covers known
symbol extents across the original virtual image, not byte matches.

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

CI installs pinned tools and runs the original-free checks. `make lint` checks
formatting and static analysis; `make check-core` runs the remaining checks.
CI uses LLVM 18; local Clang 18+ is supported with `CLANG_FORMAT`/`CLANG_TIDY`
overrides. Use LLVM 18 when reproducing CI; formatting can differ across major
versions, so a newer local pass alone does not establish CI compatibility.
Clang analyzes Windows x86 using a generated case-insensitive VFS
overlay over verified Microsoft headers; never substitute host or Wine headers.
The real compiler and matching flags remain MSVC 2005. PR comments update one
bot comment from committed report snapshots; those reported numbers do not prove
retail matching. The comment workflow reads PR data without checking out or running
PR code with its write token.

`src/externals.c` must remain declaration-only. Import slots and verified import
thunks belong in the inventory but are excluded from reconstruction matching;
do not implement replacement DLL routines. Their original PE import identities
remain separate from reconstruction linker aliases. RTTI records are internal
data and are not excluded as imports.

Original input belongs at ignored `res/LEGOStarWarsSaga.exe` (symlinks work).
All generated state is under ignored `build/`: tools and Wine in `build/tools/`,
compiled objects in `build/obj/`, the EXE in `build/`, comparisons in
`build/objdiff/`. `make clean` keeps
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
