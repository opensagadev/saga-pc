# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified baseline EEE is [9143206](https://github.com/opensagadev/saga-pc/commit/91432069da26694dfd12a63d4d8fbb2d55e554b0), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38138778480). FFF exact-revision CI is pending publication.

Checkpoint FFF restores the genuine sound configuration, SFX table producer,
backend initialization and loading-time pad reads: 15 functions / 6,052 complete
PC bytes, including 440 newly exact code bytes. Hardware and permanent-data
call sites now invoke these real providers in native order. This addresses the
missing producer behind EEE's attributed null-SfxInfo fault in GetSfxIdN; it does
not establish that the current runtime passed that fault.

The current unchanged source executable was observed under WineDbg for 60.20
seconds with empty arguments and installed assets. A four-byte write watch on the
current source SfxInfo pointer was verified installed, but captured no write and
no fault before the deadline. The test did not capture the final wait stack, so
it does not identify where startup waited or prove sound initialization completed.
The debugger was explicitly stopped, not naturally exited. All 344 objects and
frozen source/input pins remained unchanged; owned game/debugger processes and
sampled crash/game windows were absent after cleanup. Crash-dialog suppression
was set and queried before launch.

No completed nontrivial rendering, loaded world or playable recurring loop is
established. EEE's earlier cache-write/fault and separate black-window observations
remain historical evidence, not results of this new executable. Next, attribute
the actual startup wait and recover the earliest verified missing native provider.
In parallel, close real per-frame input polling and its cache reset before
claiming responsive menus or gameplay. Preserve native behavior and matching;
never substitute a null guard, fake table, success return or forced configuration.

Whole-PC fuzzy coverage is 29.732275% (+0.140523 percentage points from EEE).
Exact code is 235,450/3,471,246 bytes (+440), exact data is 637,561 bytes (+24),
and compared PC code is 1,485,614 bytes (+6,052). Mapping covers code
1,517,547/3,471,246, data 646,738/36,018,966, and combined code/data
2,164,285/39,490,212 bytes (5.480561%). Unknown original data extents earn zero
allocation or coverage credit. Similarity and mapping do not measure playability.

All 16,682 prior complete outputs were reviewed, preserving 6,274 legitimate exact
rows and 8,182 exact owner copies. The final local run passed 796 tests (three
existing skips), LLVM 18 formatting/tidy, source build/link and object validation,
and authoritative full retail matching. The immutable report describes the source
snapshot before explanatory publication edits; no source, map, tool, member or
object changes followed that comparison.

## Ownership and continuous work

Use GPT-6.1 Sol workers with exclusive bounded ownership; reuse agents and avoid
nested delegation. The main agent owns shared header/map/report integration,
aggregate validation and publication. Assign one writer to shared data at a time.
Other lanes deliver reviewed source and declaration/map/storage proposals.
Preserve original private helpers and states with their genuine caller cohort;
shared globals normally belong in `src/data.cpp`. Do not infer historical private
visibility or allocation capacity from convenient placement.

The user authorized ongoing whole-project work and direct pushes to main.
Aim for coherent checkpoints every half hour; repair validation failures before
publishing and explain delays. Verify CI for every exact pushed revision. Keep
heartbeats quiet while state is unchanged or non-actionable; report meaningful
progress, verified checkpoints or actual blockers. A checkpoint or isolated test
never marks the overall goal complete.

Preserve human edits, especially `identification-decisions.jsonl`, and exclude
unrelated journal changes from commits. Reuse retained evidence and current
baselines. Keep generated research under ignored `build/`; do not delete originals,
active evidence or tools as repository cleanup.

## Source and comparison contracts

The pinned compiler is VC8 RTM 14.00.50727.42 with `/O2 /Gy /GS /MT`; objdiff is
3.8.1. Use genuine C/C++ and named fields with asserted PC widths/offsets. The PC
retail executable establishes identity, ABI, constants and complete extents.
Preserve whole bodies including owned tables/cold tails. Unknown data extents
receive no extent, allocation or exact-data credit. Retain full initializer
mismatches and explicit qualifications rather than masking or trimming bytes.

Use real providers, callbacks, imports and returned values. Mark unresolved native
void actions at their actual sites; never fabricate a value, backend, asset,
vtable, publication or successful initialization. The linked EXE comes from our
sources, compiler and libraries, without retail code bytes. Partial source and
qualified runtime domains remain explicit. A field cohort can be released without
first reconstructing unrelated types or the entire program.

The explicit natural compiler-output inventory is separate from native identity.
It admits only pinned source/SDK occurrences with no operative incoming reference,
using fresh ordinary source-link evidence and a complete library payload census.
Positive retained/discarded COMDAT evidence handles duplicate library bodies;
new live references are fatal. All admitted outputs remain source-linked and
receive zero original binding, extent, allocation or match credit. No generic
unused-name waiver or fabricated provider replaces normal native verification.

Before publication run `make check`, `make -j"$(nproc)" build check-objects` and
`make -j"$(nproc)" match` against the supported retail executable. Use LLVM 18 to
reproduce CI. Freeze source/input/object pins for aggregate proof, compare every
previous exact contract and investigate changed complete outputs. Metadata-only
cleanup is recorded separately from source transfer. Original-free CI proves
checks/build/linking, not retail matching or a playable game.

## Progress reporting

Report fuzzy and exact code coverage over the full original `.text`, with native
objdiff's compared-subset denominator separately. Newly transferred bytes are the
unique accepted PC range union; aliases, existing caller repairs, inventory-only
rows and candidate code receive no new transfer credit.

The user-approved mapping view reports separate code, data and combined
code+data ratios over the original code/data **section virtual sizes**. Genuine
zero-initialized data is included; resource/relocation metadata, PE headers and
inter-section gaps are outside that denominator. The numerator is the union of
known symbol/member extents clipped to those sections. Unknown-size symbols
contribute zero covered bytes and are counted separately: this is not a ratio
against an unknowable sum of all original symbol sizes. Keep the former full
`SizeOfImage` metric as a separate legacy field for historical comparability.
Neither mapping nor similarity measures runnable completion.

## Automated Wine tests

Before every automated launch, set and query
`HKCU\Software\Wine\WineDbg\ShowCrashDialog` as `REG_DWORD 0` in the test's own
`WINEPREFIX`, including after prefix recreation. Retain genuine crash text,
stderr, natural exits and timeout results. This suppresses debugger UI only;
it changes no game source, flags, graphics behavior or initialization guards.

Launch in a private process group and clean only its owned surviving processes
in a finally block, using bounded SIGTERM then SIGKILL if necessary. Never kill
Wine or wineserver globally or interfere with another worker's prefix/processes.
Record owned window/process inventories and remaining runtime gaps. A successful
CPU fixture or absence of a crash dialog does not prove device, asset or frame
success.
