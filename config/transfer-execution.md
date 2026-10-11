# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified source baseline DDD is [be647b8](https://github.com/opensagadev/saga-pc/commit/be647b859b259502f46569294b0cff7680ab7703), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38136446273). EEE exact-revision CI will be checked after publication.

Checkpoint EEE restores two genuine startup providers / 132 complete PC bytes:
the guarded critical-section initializer `0x4ff100` (31 bytes) and performance timing `0x4ffbb0` (101 bytes).
Both match all native bytes. Hardware calls use the original order and real SDK
imports. Timing preserves affinity-mask query/failure defaults, temporary pin,
counter/frequency queries and restoration; no timer value or return is fabricated.

The critical-section pool starts at native `0x91efa8`, rather than arithmetic bias `0x91ef90`.
Source indices subtract one; native creation permits 13 while enter/leave permit
only 1–12. Both prior exact 30-byte enter/leave bodies remain exact. The 13-slot working
cohort has unknown original owning extent. Correcting the old false 312-byte claim
retires 304 exact-data/mapping bytes while preserving the canonical eight-byte frequency.
New startup critical/guard views also have unknown original extents and zero credit.

The declaration cache now has a 256-record inferred working backing. This is the
strongest converging reconstruction: PC `0x808` gap before the independently identified
water table, Mac same inline eight-byte record and 2,048-byte spacing, and Android's authentic 256-entry pool.
The complete 2,048-byte loader initializer is native-equal, and the former 32-byte
prefix survives. Original PC capacity/owning extent remains unproved; 257 or a
smaller object plus unnamed storage is not mathematically excluded. Map extent
stays blank and receives no data/allocation credit. Counter, lookup, copy,
insertion, lifecycle and failure behavior are unchanged. This replaces arbitrary
incremental minimum growth with an explicitly qualified architectural inference.

Whole-PC fuzzy coverage is 29.591752%, a change of
+0.003031 percentage points.
Exact code is 235,010/3,471,246 bytes; exact data is
637,537 bytes. The data decrease is the explicit false
pool-extent retirement, separately from source gains. Complete prior outputs and
all legitimate exact contracts are independently reviewed; no masks or trimming.

The rebuilt source was tested with empty arguments and installed assets, first
under a correctly installed four-byte WineDbg write watch, then normally for
60 seconds. The debugger captured genuine declaration-count writes 1 and 2;
at a later fault the count was 13 and the cache contained real records. This run
passed the formerly observed four-record storage failure. It does not prove the
original cache capacity or establish a bound for every startup/gameplay path.

The new attributed fault is source `0x005347d5`, inside exact `GetSfxIdN`
(native `0x650e40`, 77 complete bytes), reading null `SfxInfo`. The recorded return
chain passes through particle loading. Current source omits the preceding native
sound configuration provider at `0x6521b0`, and never assigns `SfxInfo`.
Recovering that real producer and its consumed dependencies is the next priority;
a null guard or dummy table would hide the missing reconstruction.

The separate normal run reached its 60-second deadline without a logged source
fault; its PID-attributed window remained black in samples at 5.79, 21.44 and
55.81 seconds. Debugger timing changes scheduling, so these are distinct results.
No completed nontrivial rendering, world activation or playable game is established.
Both runs preserve all frozen inputs and objects, set and query crash-dialog
suppression, and leave no owned game/debugger process or sampled game/crash window.
The debugger was explicitly killed/quit after capture; the normal run was stopped
at its deadline. Neither is reported as a natural successful game exit.

Reconstruction follows the measured executable frontier: fix the earliest
attributed native failure or missing consumed provider, then rerun the unchanged
entry path against installed assets. Device/window invocation, completed presentation,
world activation and playable recurring updates are separate milestones. Prepare
connected independent source families in parallel where they help these milestones.
Do not introduce success returns, assets, vtables, guards or graphics workarounds.

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
