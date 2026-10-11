# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Checkpoint III transfers 68 additional functions from connected Android source
families, covering **17,037 original PC bytes**. These include AI actions and
providers, player combat, matrix/vector/half conversion, rectangle rendering and
pause resumption. Thirty-eight new functions match **4,138 bytes** exactly.
Seven real movement calls and the after-animation laser path are restored.

Startup now uses three real PC text tables and the complete reconstructed
Text_LoadStrings tail. All **9,760 emitted table bytes** and their pointer
relocations match native memory. The tables reuse 46 canonical ID definitions
and provide 669 new IDs. Unknown owning extents receive no allocation credit.
The loader's old boundary ended inside an instruction; its complete PC extent
is corrected from 397 to 403 bytes, accounting for six additional compared bytes.

Whole-PC fuzzy coverage is **30.428346%**
(+0.374951 percentage points over HHH).
Exact code is **241,489/3,471,246 bytes**;
compared PC code is **1,516,024 bytes**.
No previous exact function or data match was lost. Dedicated text-table ownership
preserves canonical data definitions. Two older nonexact central regions change
only in emitted zero padding; all their typed values and full initializers remain
native-equal. Unchanged-source compiler scheduling differences were reviewed.

Local validation passes: 797 tests (three existing skips), LLVM 18 formatting
and static analysis, source EXE build/link, 350-object checks and full retail
matching. HHH revision `a774cd3` passed
[CI](https://github.com/opensagadev/saga-pc/actions/runs/38147589059).
III's exact-revision CI is verified after its direct push to main.

A bounded ordinary run with installed assets in an isolated Wine prefix held a
visible game window for about 30 seconds; the captured image remains entirely
black. No game fault was logged, but Mesa reported EGL/driver warnings. The run
ended at its deadline and left no prefix-owned processes. This does not prove
completed rendering, a loaded world or playability. The source is **not playable**.

Continue bulk source transfer through named Android callers and existing PC
anchors. The retained direct-call frontier contains 56,502 bytes of candidate
spans; those are research only. The next connected target is the AI connection
controller family with its genuine fake-animation producer, followed by the
remaining player combat dependencies. For runtime, attribute the earliest
startup wait and close actual permanent/world-loading providers. Avoid register
and instruction-order tuning unless comparison reveals a behavior/type error.

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
