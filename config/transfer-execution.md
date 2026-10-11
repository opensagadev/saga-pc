# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified published source baseline CCC is [eebedac](https://github.com/opensagadev/saga-pc/commit/eebedac3b0adddc2da1323459076065d37f17b45), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38133432755). The subsequent [repository cleanup](https://github.com/opensagadev/saga-pc/commit/6877a7756400388be1c75facdebf1a8a96120b84) also [passes all 13 steps](https://github.com/opensagadev/saga-pc/actions/runs/38133974217); its verified recoverable archive reclaimed 1.28 GB. DDD exact-revision CI will be checked after publication.

Checkpoint DDD adds 13 complete functions / 2,044 newly compared PC bytes:
viewport/lighting/resource initialization (six / 667) and connected character,
camera, audio, cheat, batarang and scene reset providers (seven / 1,377).
Six functions / 630 bytes are exact; exact data increases by eight bytes.
Three original hardware call sites now consume the real viewport, lighting and
resource-registry providers. The partial caller receives no duplicate byte credit.

Whole-PC fuzzy coverage is 29.588721%, up 0.040816 percentage points.
Exact code is 234,878/3,471,246 bytes (6.766389%); exact data is 637,841 bytes.
Mapping is 1,511,503/3,471,246 code bytes and 647,018/36,018,966 data bytes,
combined 2,158,521/39,490,212 section bytes. None measures runnable completion.
All prior exact bytes survive, including 32 viewport bytes migrated to genuine
named members. All 13 prior named members survive; 14 viewport contracts are added.
Of 16,653 prior complete outputs, 16,639 are byte-identical, 11 have explicit
canonical viewport storage qualifications, and three have independently reviewed
complete output changes (hardware calls, constant-order code and data padding).
No comparison is masked or truncated.

The viewport has one genuine 64-byte working view, with asserted named fields;
its original owning extent remains unknown. Ten duplicate scalar allocations
are removed. The getter preserves raw float bits and matches all 62 native bytes.
Lighting retains native loader/runtime defaults and the eight-record working
cohort without claiming an original allocation boundary. Resource cleanup calls
real NuFree and COM providers; NuFree remains partial in arena/manager domains,
so successful lifetime closure is not claimed. All new unknown data views receive
zero owning-extent credit. The genuine two-float audio timeout array adds eight
exact data bytes. No new constructor or dynamic initializer is introduced.

Required local checks pass: 796 tests (three existing skips), LLVM18 format/tidy,
source build/link, complete coverage of 344 objects and full authoritative retail
matching. Full prior outputs, references and exact results were independently
reviewed against the immutable normal object bank.

The DDD source executable was observed for 60 seconds with empty arguments and
installed assets. A PID-attributed window titled `TitleNotSet` persisted from
2.5 seconds to the deadline. No rendered frame, loaded world, native window-class
milestone or playability is established. Mesa EGL/DRI2 warnings were recorded;
those do not locate the source wait. A bounded debugger attach failed with
`ptrace: Operation not permitted`, so there is no stack attribution. The launcher
terminated only its owned process group; no owned processes or sampled game/crash
windows remained. Wine crash-dialog suppression was set and queried before launch.

Decompilation order now prioritizes the actual startup frontier, then device and
presentation, installed-world activation and recurring update/render. First obtain
an attributable wait/fault with the unchanged source executable and distinguish
host graphics behavior from source omissions. Repair the earliest verified missing
provider or incorrect native binding, then rerun a bounded test. Do not infer a
blocker from a percentage or add success returns, fallback behavior or guards.

Remaining holds include the original declaration-cache allocation/preceding write,
real renderer/bootstrap constructor and shader providers, and the consumed
providers of the full CheckResetBits caller. The prepared critical-section/timing
pair (132 candidate bytes) is held: the existing thread-array mapping overlaps
canonical performance-frequency storage. Resolve native ownership/layout before
activation; candidate bytes receive no transfer credit.

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
