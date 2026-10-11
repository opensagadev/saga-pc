# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified published baseline BBB is [c1e87df](https://github.com/opensagadev/saga-pc/commit/c1e87dfe7901cb4b383e9b3a4465a51a28e9f2be), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37892772484) passing all 13 steps. CCC exact-revision CI will be checked after publication.

Checkpoint CCC transfers nine complete functions / 1,685 newly compared PC bytes:
occlusion initialization (three / 720), startup material creation (one / 469),
and input mapping/configuration (five / 496). Five functions / 203 bytes are
byte exact. The other four retain complete nonexact comparisons and independently
reviewed PC contracts. Three original hardware sites now call real providers;
the partial hardware parent receives no duplicate transferred-byte credit.

Whole-PC fuzzy coverage is 29.547906%, up 0.034794
percentage points. Exact code is 234,248/3,471,246 bytes
(6.748240%), up 203; exact data remains 637,833 bytes.
Every prior exact contract survives: 2,451 code copies, 5,712 data copies and
13 named members. Of 16,636 prior complete outputs, 16,633 stay identical.
The three changes are the intended hardware parent and two specific previously
reviewed camera/particle compiler-output pairs, with complete bytes/references
and their stable-memory/floating-point qualifications retained.

The existing occlusion manager's complete 368-byte initializer remains unchanged
and native-equal. Six new scalar/array views total 36 typed bytes in ordinary
sections; all complete emitted initializers equal native, but original owning
extents stay blank and receive zero data credit. Three material class statics
remain in original occlusion.cpp. No new constructor or dynamic initializer is
introduced. Existing central initialization remains unchanged.

All required local checks pass: LLVM18 format/tidy, 796 tests (three existing
skips), source build/link, 343-owner object coverage and full retail matching.
The initial sandboxed match attempt could not start Wine IPC; the unchanged
normal-environment retry passed. No compiler flags, source guards, comparison
masks or extents were changed to obtain these results.

CCC has not been run as a game. The latest full-game observation remains BBB's
60-second deadline termination, with no fault recorded and no verified native
game window, recurring frames, loaded world or playability. The earlier AAA
declaration-cache failure and unproved original allocation remain unresolved.
No cache size, counter, search or guard is changed. Occlusion keeps native
default capacity zero and its zero-capacity early return.

Next is the producer-backed viewport family (two / 213 candidate bytes), with
genuine 64-byte viewport views and storage review before activation. Lighting
and resource registry providers remain separate bounded holds. Candidate bytes
receive no comparison credit. Cleanup follows publication before further source
integration; originals, current tools, active proofs and human edits are retained.

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
