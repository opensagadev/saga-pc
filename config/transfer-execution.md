# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
[source checkpoints JJ–SS](../docs/history/source-checkpoints-JJ-SS-2026-10-09.md),
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified baseline FFF is [a803b6f](https://github.com/opensagadev/saga-pc/commit/a803b6f0973efced1b6d8e00885ff1f675f9aa53), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38141919062). GGG exact-revision CI is pending publication.

Checkpoint GGG connects genuine per-frame input polling and cache reset in
native caller order. The 27 new functions cover 8,881 complete PC bytes: keyboard
construction/destruction, real DirectInput/XInput sampling, mouse translation,
binding capture, record output and its math/file dependencies. Eight functions
match all 693 native bytes. State polling calls the real XINPUT1_3 DLL;
no device results, enabled flags or provider returns are fabricated.

The earlier FFF debugger run independently established InitSfx returning with
1,130 definitions, 1,404 samples and a non-null SfxInfo table, then entering
GetSfxIdN. This passes the previously attributed null-table startup frontier;
it does not establish audible output, lookup completion or playability.

The unchanged GGG source executable ran under Wine for 60.70 seconds before the
configured deadline stopped it. The PID-attributed window captures at 6.31,
20.83 and 58.83 seconds are identical black images. No fault was logged; no final
wait stack or completed frame was captured. This establishes a persistent game
window during the observation, not responsive input, completed rendering, loaded
world or playability. The actual wait remains unattributed by this ordinary run.
All 344 source objects and frozen input pins were unchanged. Crash-dialog
suppression was set and queried before launch; all owned game processes and
sampled game/crash windows were absent after identity-bound cleanup. Termination
was by the test deadline, not natural game exit.

The next runnable dependency wave is the genuine save-folder registration
(49 bytes) and save initialization/slot enumeration (385 bytes), restoring the
conditional hardware call and original InitOnce arguments. Current startup skips
both producers; the permanent loader waits for saveload_status. Restore actual
folder publication and six-slot file enumeration before readiness is produced.
Then connect the reviewed legal texture/material/color/vertex family (514 bytes),
font/text-table backing and the remaining permanent/world loaders. Keep native
failure behavior, caller order and matching; no forced readiness or substitute
providers. Research proposals remain local and uncompiled.

Work is paused at the user's request after this validated checkpoint is pushed.
Resume only on the user's instruction, using the retained proposals and current
source; the recurring automation is paused.

Whole-PC fuzzy coverage is 29.932080%
(+0.199805 percentage points).
Exact code is 236,143/3,471,246 bytes (+693), exact data is
637,627 bytes (+66), and compared PC code is
1,494,495 bytes (+8,881). Mapping covers code
1,526,428/3,471,246, data
646,798/36,018,966, and combined code/data
2,173,226/39,490,212 bytes
(5.503202%). Unknown original data extents earn zero
allocation or extent credit. These measurements do not measure playability.

Final checks pass: 796 tests (three existing skips), LLVM18 format371/tidy344,
source build/link and object validation (5,247 functions, 11,490 data definitions,
344 objects), and authoritative full retail matching. The isolated DAT fixture
now reuses the actual file-writer endian definition; the initial missing-fixture
link dependency was fixed without changing production behavior or weakening tests.

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
