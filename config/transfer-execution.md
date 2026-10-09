# Current transfer execution policy

The whole-project PC reconstruction remains active. Use the
[mandate](../SOL_IMPLEMENTATION_PLAN.md), [roadmap](../docs/roadmap.md) and
[work plan](progress-plan.md). This page is current policy; dated measurements and
all earlier release evidence remain in
[the complete history through checkpoint II](../docs/history/transfer-execution-through-II-2026-10-08.md)
and [transfer-checkpoint.json](transfer-checkpoint.json).

## Published baseline and runtime

Verified published baseline VV is [d5faf138](https://github.com/opensagadev/saga-pc/commit/d5faf1388f5e5570df2bbfceefa14761109da4aa), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37879148887) passing all 13 steps. WW exact-revision CI is pending publication.

The WW batch adds 27 complete functions / 8,078 unique compared PC bytes across
material/shader parameters, display-list drawing, back-buffer/resource suppliers
and native presentation. Fourteen are byte exact, adding 463 exact code bytes.
Whole-PC fuzzy coverage is 29.474249%, up
0.078212 percentage points from VV; exact code is
233,469/3,471,246 bytes (6.725798%). Every previous exact code/data result
and all 13 named member rows survive.

All 20 observed display callbacks now have compiled source. All 31 new complete
initializers (840 typed bytes) equal native, including the 49-entry handler view
and its pointer: 27 NULL entries and 22 pointers to 20 distinct real callbacks.
Unknown owning extents remain blank and receive zero allocation or exact-data
credit. The old unimplemented DrawScene extent is corrected from 209 to 210 bytes:
209 cut off the final byte of its terminal jump. This is a boundary correction,
not additional transferred source.

Four genuine calls are restored: renderer DrawScene and native presentation,
then frame-end TimeBar(-1) and SwapScreenEx with the current callback. These
existing parents add zero new compared ranges; renderer 1,211/native 1,465 and
frame end 365/native 987 remain partial and nonexact. No separate scene counter
reset, enlarged array, forced focus/profiling or fabricated success is added.

Required checks pass: 796 tests (three existing skips), LLVM18 formatting and
342-unit analysis, normal source link/object checks and full retail matching.
Among 16,514 old complete outputs, 16,509 retain identical relocated bytes; two
parents change through the four reviewed calls, and three unchanged-source
Camera/Deb/Init outputs retain specifically reviewed historical finite pairs.
The 89 natural unused SDK outputs preserve their contracts and pass fresh
normal-link/archive admission with zero PC credit.

The latest ordinary source executable exits 5 in LoadPerm1 while reading
DebMat[5]->texture_id through a NULL material pointer. The preceding genuine
DebrisSetup call at native 48a6f8 is still omitted. Its native wrapper at 683530
and the existing DebrisSetup2 material/resource producers are the next connected
matching targets. No replacement material or direct DebMat assignment is added.

One actual source-game run exited naturally after 2.546 seconds. Its complete
781-byte LoadPerm1 body is verified against the linked source image; fault
source453f85/native48a7e7 reads WORD [EDX+74] with NULL DebMat[5]. All 342 normal
objects and 455 frozen inputs remained unchanged. Wine crash suppression was set
and queried, and owned process/window cleanup was empty. This is a failed startup
observation, not a passing frame, presentation, assets or gameplay test. The source
game remains not playable. Earlier CPU fixture results retain their original
bounded domains; they do not test the new material or live renderer paths.

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


## Current source checkpoint JJ (2026-10-08)

The connected push/ledge/teleport/title/geometry wave adds 23 complete functions
and 16,291 unique PC bytes, with nine real existing-caller reconnections.
Normal owners rise to 324. Required local checks pass: 732 tests (three existing
skips), LLVM18 format/tidy, source build/link, object coverage and full retail
matching. Actual-normal bounded CPU replay passes 156/156; initial oracle failures
and their native-byte correction are retained. All prior exact contracts remain.
The detailed current measurements and qualifications are in
[transfer-checkpoint.json](transfer-checkpoint.json). The new source EXE was not
rerun; checkpoint II remains the last full-game runtime observation above.

The separate mapping/documentation cleanup `1c2a569eed9ef6a74ecb7e3bfe316491fd39c457`
has [all-job exact-revision CI success](https://github.com/opensagadev/saga-pc/actions/runs/37713793616).
JJ passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37716326923) on the exact pushed revision.
The next-wave geometry producers are recorded in KK below; remaining motion/world
providers continue alongside the unresolved native startup frontier.

## Current source checkpoint KK (2026-10-08)

This snapshot adds 41 full PC functions/10,024 unique bytes: geometry producers
6/3,401, motion 3/847, CPU gizmos 14/2,432, mission readers 6/1,314, uniform upload
8/808 and push/terrain supports 4/1,222. Three existing movement actions and five
camera uniform publications now call real providers. Caller reconnections add no
extra transferred range. The shader-object owner retains its previous bodies
through its reviewed C-to-C++ conversion.

Required checks pass: 732 tests (three existing skips), LLVM18 format for 356
files and tidy for 329 units, source build/link, object coverage and full retail
matching. All prior exact contracts remain preserved. Whole-PC fuzzy code is
27.558930%; exact code is 219,137 bytes (6.312920%), a 953-byte gain. Known exact
data stays 637,612 bytes. Mapping is 40.896785% code, 1.796293% data and 5.233282%
combined code/data section storage.

The actual-normal CPU fixture passes 227/227: reset 120, perspective 16, basis 76
and conservative guards 15. Its 24 positive camera checks remain held because
the genuine shader manager/compiler is unpublished. The retained VC8 test-macro
parse failure was corrected solely by renaming its formal parameter; assertions,
goldens and engine code stayed fixed. ShowCrashDialog DWORD 0 was set and queried;
the owned test process group was empty afterward. No source EXE launch, GPU,
asset or playable-game claim is made. Complete qualification and evidence pins
remain in [transfer-checkpoint.json](transfer-checkpoint.json).

## Current source checkpoint LL (2026-10-08)

This local snapshot adds 74 complete functions/25,502 unique PC bytes: lever
21/4,472, streaming 5/1,453, mission menu 5/2,425, push 23/9,546 and hat 20/7,606.
Two existing Move_CHARACTER actions now call real hat and lever providers, adding
no extra transferred range. Canonical mission-save fields and the menu packet use
asserted PC offsets; private registration descriptors retain their original owners.

Required checks pass for 331 owners: 732 tests (three existing skips), LLVM18
format for 358 files and tidy for 331 units, source build/link, object coverage
and retail matching. Whole-PC fuzzy code is 28.075228%, +0.516298 percentage
points; exact code is 221,357 bytes (6.376874%), +2,220. All prior exact copies
and 13 named members/28 bytes survive. Known exact data stays 637,612 bytes.
The 166 new complete initializer copies/1,645 bytes are retail-equal; unknown
owning extents receive no allocation or exact-data credit. Mapping is 41.631449%
code, 1.796293% data and 5.297860% combined code/data section storage.

Actual-normal CPU checks pass 264/264 through genuine providers and registration
callbacks. The unchanged fixture required ordinary VC8 response-file packaging
because the original command exceeded Win32's process-creation length limit.
A parser-only correction on retained stdout fixed an unanchored section-name
match; the fixture and engine were not rerun or changed. Crash dialogs were
suppressed and the owned process group was empty afterward. No source game EXE,
Hat drawing, positive asset streaming, device or gameplay milestone is claimed.

Final local review accepts three specific prior nonexact compiler-output pairs
within their stated finite domains. Camera and DebReAlloc reproduce previously
reviewed pairs; the particle reload permutation has its own complete instruction
and binding proof. LL passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37811078781). Full pair and data-tail
qualifications remain in the checkpoint evidence. Next connected source includes
obstacle registration and character loading. Corrected transport source passed
independent review; genuine shared storage and current-header integration remain
conditional. The rejected signed-index version and earlier history are retained.

## Current source checkpoint MM (2026-10-08)

This snapshot adds 87 functions/15,476 unique PC bytes across obstacles,
tubes/plugs, MiniCut, character loading/callbacks and special gizmos. Ten original
source owners retain all previous bodies; gizminicut.c is the one new owner.
Private callbacks/descriptors and minimum named field cohorts retain asserted PC
ABI/offsets. Eighteen central state views use real native initial values; their
unknown owning extents receive zero allocation or exact-data credit.

All required checks pass: 732 tests (three existing skips), LLVM18 format for
359 files and tidy for 332 units, source build/link, complete object coverage
and retail matching. Whole-PC fuzzy code reaches 28.423950%, +0.348722 percentage
points; exact code reaches 223,832 bytes (6.448175%), +2,475. All prior complete
outputs and exact contracts survive. Five finite contained compiler-local serial
changes retain identical bytes; no old body/data pair changed. All 61 new complete
initializer copies/950 bytes equal retail. Known exact data remains 637,612 bytes.
Mapping is 42.062677% code, 1.796293% data and 5.335765% combined section storage.

Actual-normal CPU checks pass 241/241 through real callbacks, character parser
and POD suppliers. The initial fixture compile referred to a nonexistent public
max_specials field; one initialization was corrected to the already-asserted
local reader cohort. Assertions, values, providers, engine and flags stayed fixed.
Crash dialogs were set and queried off, natural exit was zero and no owned
process remained. No camera playback, installed asset, active movement, source
EXE or device/gameplay milestone is claimed. Partial native actions and bounded
valid-input domains remain explicit.

MM passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37819227040). Next source closes the genuine GCDATA_DEFAULT
minimum record and FixUpCharacters/Extra parents, alongside Techno/Portal/Shadow.
Their hypotheses receive no transfer credit. Full ordered registration tables,
real startup CharFixUp storage and shader publication remain separate contracts.

### Techno, Portal, Shadow and character installer source NN (2026-10-08)

The current source adds 47 complete functions/5,816 unique PC bytes: Techno
20/2,467, Portal 15/978, Shadow 10/1,019 and character installer parents 2/1,352.
Five existing owners retain their exact prior prefixes; edgizshadowmachine.c is
one new owner. Every prior complete function output and exact contract survives.
The one changed prior data region is the explicitly reviewed unknown-sized
BATTLEDROID WORD tail: full COFF 2→8 remains nonexact and zero-credit; no tail
is used as the distinct native Commander state.

Required checks pass: 732 tests (three existing skips), LLVM18 format for 360 files and tidy for 333 units,
source build/link/object coverage and full retail matching. CPU 183/183 passes
against the actual normal objects, with genuine sentinel preconditions and no
ID assignments, provider replacements or fixture corrections. Crash dialogs
were disabled and queried; no owned process remained. Whole-PC fuzzy code is
28.574010%, +0.150060 percentage points; exact code is 225,541 bytes (6.497408%),
+1,709. Known exact data remains 637,612 bytes. All 56 new complete initializer
copies/1,089 bytes equal native; unknown owning extents receive zero credit.

GCDATA_DEFAULT supplies its genuine minimum 288-byte record/91 initialized fields
and real model-layer callback. Techno/Portal/Shadow canonical state and private
registration owners retain verified offsets and defaults. FixUpCharacters (178 bytes)
is exact; ExtraCharacterFixUpAfterConfig (1,174 bytes) remains nonexact with its finite
stable initialized record flag-width qualification. Startup CharFixUp 218 and
Commander storage remain held. No source game EXE, assets, GPU or gameplay
milestone is claimed. Exact pushed-revision CI is pending. Full qualifications,
remaining contracts and evidence pins are in transfer-checkpoint.json.

### Game reset source checkpoint OO (2026-10-08)

The current batch adds nine complete functions/1,779 unique compared PC bytes.
Required checks and 125 actual-normal CPU checks pass; all prior exact contracts
survive. Specific particle reload and unknown BARMAN tail qualifications are
recorded in transfer-checkpoint.json, together with the retained fixture failures.
Fuzzy code is 28.619259%; exact code is 6.506280%. This establishes no complete
startup, assets, device or gameplay milestone. OO passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37833980118). Next work connects genuine cutscene and Batarang/camera providers to the
larger startup callback cohort.


### Connected cutscene and targetting source checkpoint PP (2026-10-08)

Five complete functions add 1,149 unique PC bytes. Required checks and 208 CPU
checks pass; all prior exact contracts survive. Fuzzy code is 28.651749%; exact
code is 6.511379% (226,026/3,471,246 bytes). Specific complete camera and fresh
particle qualifications are retained. Configure's parser/callback runtime,
assets, device and gameplay remain held. PP passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37838400254).
Next work closes Collection and the 65 genuine startup callback publications.
All 17 former span holds are recovered; source/provider/storage contracts remain.
Candidate bodies and pointer inventories receive no transfer credit.


### Connected startup suppliers QQ (2026-10-08)

Twenty complete functions add 2,684 unique PC bytes across Collection, Attracto,
customization, targeting, vehicle/suit, music, Hub and audio. Nine are exact,
adding 452 bytes; the corrected position getter preserves coordinate bits exactly.
Required checks pass for 338 owners: 732 tests (three existing skips), LLVM18
format/tidy, source link/object coverage and complete retail comparison.
Whole-PC fuzzy is 28.716408%, exact 226,478/3,471,246=6.524401%.
All prior exact and 13 member/28-byte contracts survive.

Selected runtime assertions pass 152: 90 from the first launch and 62 from a
separate corrected/targeted delta. The first fixture's one failed assumption
(Suit IDs -1 instead of native zero) remains retained; no engine ID was assigned.
Crash dialogs were disabled and queried; owned process groups were empty afterward.
Tag's complete eight-byte initializer remains nonexact and zero-credit. Specific
prior physical data/code pairs retain complete bytes and finite qualifications.
No source-game, loaded assets, device or gameplay milestone is claimed. QQ passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37848679469). Next work integrates the genuine 65-callback publisher with original-owner
configuration and shared states; candidate/source-ready bytes receive no credit.

### Connected startup publisher RR (2026-10-09)

The current source integrates InitGameAfterConfig and its genuine original-owner
callbacks: 66 newly compiled complete function extents add 13,504 unique PC bytes.
This comprises 64 previously uncompared callback targets (8,766 bytes), the
4,659-byte parent and its naturally emitted 79-byte private helper. An already
compared Batarang callback receives no duplicate credit. All 65 actual native
callback publications are represented; ten parent actions and 19 distinct missing
providers across the connected callback family remain explicit void omissions.

Required checks pass for 338 owners: LLVM18 format/tidy, 732 tests (three existing
skips), source EXE link, complete symbol/relocation coverage and retail comparison.
Fuzzy coverage is 29.034284%, +0.317876 percentage points; exact code is
228,700/3,471,246 bytes (6.588412%), +2,222. All prior exact code/data and 13 named
member contracts survive. All 210 new complete initializer copies (2,360 bytes)
match native, with unknown owning extents receiving zero allocation or exact-data
credit. Known exact data remains 637,616 bytes. Mapping is 42.777464% code,
1.796304% data and 5.398606% combined code/data section storage.

Actual-normal CPU tests pass 90/90 on 13 genuine private callbacks, with complete
COFF-to-linked-test-body equality and original provider/object order. Crash-dialog
suppression was set and queried; all owned process groups were empty afterward.
No source-game launch, successful Init/configuration, loaded assets, device, GPU
or gameplay milestone is claimed. The earlier parent report remains rejected for
its signed-byte cheat sentinel error; the corrected source and report are used.
Specific complete Tag pointer-tail and particle reload pairs retain their bounded
proofs. No code-generation flags, native extents or comparison checks were weakened.

RR passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37855398941). The complete checkpoint is recorded in
transfer-checkpoint.json. Next work reconnects real terrain registration,
GameAudio/Menu sound IDs, FancyMessage, narrow-socket, surface-mask, colour and
PodRace providers. The retained connected frontier has 9,608 candidate native
bytes, with 12 functions/1,510 bytes ready for source authoring; these estimates
receive no transfer credit. Full Bolt 44-record backing, CharFixUp, shader
publication and resource/device lifetime remain separate contracts.

### Shader startup checkpoint SS and observed frame-lifecycle gap (2026-10-09)

The validated source wave adds 52 newly compared functions / 6,422 unique PC
bytes, including 22 shader publisher/parser/compiler functions and genuine PC
SDK helpers. Complete EH parents retain their cold entries and all bytes. The
retail replay preserves every RR exact code and data contract: whole-PC fuzzy
coverage rises from 29.034284% to 29.202954%, exact code from 228,700 to 231,692
bytes. The source remains unplayable; these figures measure reconstruction.

A bounded data watch proves that the first display-list pointer is initialized
correctly. The full 139-byte `NuRndrEndScene` matches native exactly, but its copy
at scene index 32 overwrites the source first-list pointer with zero. The actual
caller is `LoadPerm`, which repeatedly calls `NuFrameEnd`. Original array capacity
remains unknown; enlarging the working view would not reconstruct its lifecycle.

Native `NuFrameEnd` calls `0x60b5b0` at `0x4fd18e`, after the time-bar renderer and
before `0x5de1b0`. This 24-byte callback wrapper calls `0x60aff0` (1,465 bytes).
The renderer processes the queued scenes and clears `sceneParametersCount` at
`0x60b50d`. Our frame end omits this entire call. The next connected wave is this
real renderer, wrapper and provider closure, followed by their authentic startup
and per-frame connections. A direct counter reset would skip their actual work.

Required checks pass with 796 tests (three existing skips), LLVM18 format/tidy
and the normal source link/object checks. The retained shader CPU suite passes
208 assertions in its bounded domain. A strict source/library reference census
records 89 unused natural compiler outputs separately with zero PC credit.
Wine crash dialogs are disabled and all owned diagnostic processes are cleaned.
The full checkpoint and exact-revision CI status are recorded in
`transfer-checkpoint.json` and `transfer-execution.md`.
