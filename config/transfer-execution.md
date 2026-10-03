# Whole-project transfer execution — 2026-10-03

The authorized [whole-project plan](../SOL_IMPLEMENTATION_PLAN.md) is in execution.
Eight GPT-6.1 Sol workers transferred source by subsystem while the lead owned
shared contracts, mappings and integration. The project objective remains the
complete PC reconstruction; the batches below are verified checkpoints.

## Source transferred

| Family | Actual production work |
| --- | --- |
| AI | Locator/path movement, owner state, flags, actions and physical seven-argument callbacks in `gameapi/aisys.c`; preserved PC masks, guards and selector differences. |
| File/resource I/O | Extension registration/search/conversion, loading, DAT access, genuine RNC2 decoding and packed loading, parser corrections and file initialization. Private extension helpers remain with their callers. |
| Character/animation | Cache readers, model reset, frame queries, animation flags/speed, remap/debris initialization, redirect registration and character-system arena initialization. |
| World/progress | Bounds and saved animation factors, config loading, area changes with real reset providers, script/AI/grabber progress serialization. |
| Rendering/movement | Straight/arc laser producers and draws, actual four-lane vector provider and wrappers, coordinate stack writes, movement leaves and obstacle playback. |
| Platform/audio/UI | DirectSound stop/update state, actual D3D9 state/cache wrappers, texture reference/release chain, music diagnostics and font state/point-size services. |
| Debris | Effect lookup and three real emitter vector setters. |

## Integration decisions

Retail PC remains authoritative. Compiler comparisons use pinned VC8 RTM
14.00.50727.42, `/O2 /Gy /GS /MT`, and objdiff 3.8.1. Android supplies source
structure, not automatic identity or type proof. Complete original function
spans include internal dispatch tables, padding and decoder backedges. Low
similarity bodies remain visible rather than being shortened or dropped.

The proposed Android XYZ vector crosswalk was rejected: the actual PC provider
scales all four lanes. Its 27-byte wrapper uses the neutral reconstruction name
`NuVec4ScaleForwardPc`. The 0x38-byte character-system header is established by
its arena advance, memset and highest store; an older unproven tail was removed.
Font/core and platform wrappers retain real cross-unit calls. Real one-byte RET
providers are reconstructed only where the original executable has that body.

The packed loader signature is three arguments: file, destination and capacity.
The decoder has five native synthetic execution cases, and the DAT integration
fixture exercises packed/raw loading and allocation alongside its existing
archive, seek, memory-file and PAK assertions. Fixture-only allocation/state
providers are independent test support, not implementations in the game.

Complete typed globals and initializers are checked against retail bytes after
relocation. Bounded access views with unknown original allocation extents are
excluded from full-allocation credit. The existing identification journal's
pre-session edits are preserved.

## Remaining work and next source families

The full game lifecycle is incomplete. Keep the current runtime entry guard until
real device/input initialization, fixups, scene/resource creation and world
startup/shutdown are connected and tested with assets. The initial native entry smoke faulted in the unchanged `NuKeyboardRead`
when `LoadPerm` called `NuFrameBegin` with an absent input owner. An explicit
entry-path guard now stops before that call while input initialization is held.
This is an intentional incomplete-startup marker, not retail behavior or a
replacement input provider. Guarded entry testing does not establish gameplay.

* Complete world reset/dump/activation and grass/bridge loading need actual scene,
  terrain, debris-page and customiser providers. A complete 1,866-byte source
  packet remains held; no calls were replaced with fabricated results.
* The 723-byte finite debris emission backend needs `AddDebrisEffect` and real
  sound helpers before ForceThrow and the larger animation-effects caller can
  be released. Judder and momentum setters are now real providers.
* The 2,619-byte model loader and 2,808-byte animation-effects body compile in
  scratch; finish their finite resource/emission providers, preserving private
  helper/TU context.
* Punch/directional movement includes material PC-only behavior and missing
  providers; retain the tested drafts and field evidence, then transfer the
  actual provider families rather than repeating discovery.
* Continue accepted engine resources, scene/spline lookup, UI, rendering, input
  and portable gameplay families; map new families in parallel. Whole-program
  mapping is not a prerequisite for compiling a local source group.

Detailed reproducible compiler/comparison packets and logs remain under
`build/analysis/transfer-execution-20261003/`, grouped by owner. The durable source,
asserted contracts, accepted map, runtime tests and `matching.json` are the
production artifacts. Coverage below is refreshed only after aggregate checks.

## Measured production checkpoint

The baseline is commit `680ca578092fafa147d5887da14104b1cc85f8da` with the
pre-session journal additions preserved. Both comparisons use the same retail
`.text` denominator, 3,471,246 bytes.

| Measurement | Before | After |
| --- | ---: | ---: |
| Compared functions | 1,711 | 1,914 |
| Unique compared PC code bytes | 388,082 | 433,435 |
| Whole-PC fuzzy coverage | 6.787306% | 7.710590% |
| Exact PC code bytes | 47,697 | 53,115 |
| Exact functions | 828 | 898 |
| Exact data definitions | 2,257 | 2,467 |

No previously exact function was lost. Native objdiff's 433,490-byte denominator
includes 55 overlapping bytes; unique source coverage credits 433,435 bytes.
The 45,353-byte increase is actual compiled code, excluding held source and
map-only additions. Shared data's 228 reviewed definitions include 197 complete
exact initializers covering 88,031 original bytes; 31 bounded unknown-extent
views receive no complete-allocation credit.

### Newly compared code by source unit

| Unit | Functions | Original PC bytes |
| --- | ---: | ---: |
| [src/gameapi/aisys.c](../src/gameapi/aisys.c) | 99 | 25,392 |
| [src/gameapi/apiobject.c](../src/gameapi/apiobject.c) | 11 | 1,368 |
| [src/legoapi/areas.c](../src/legoapi/areas.c) | 3 | 1,009 |
| [src/legoapi/camera.c](../src/legoapi/camera.c) | 1 | 301 |
| [src/legoapi/creature.c](../src/legoapi/creature.c) | 1 | 119 |
| [src/legoapi/doors.c](../src/legoapi/doors.c) | 1 | 23 |
| [src/legoapi/game_deb.c](../src/legoapi/game_deb.c) | 2 | 328 |
| [src/legoapi/gameaudio.c](../src/legoapi/gameaudio.c) | 1 | 11 |
| [src/legoapi/gizobstacle.c](../src/legoapi/gizobstacle.c) | 5 | 669 |
| [src/legoapi/grabber.c](../src/legoapi/grabber.c) | 1 | 76 |
| [src/legoapi/level_area_load.c](../src/legoapi/level_area_load.c) | 1 | 164 |
| [src/legoapi/minikits.c](../src/legoapi/minikits.c) | 1 | 13 |
| [src/legoapi/move.c](../src/legoapi/move.c) | 8 | 826 |
| [src/legoapi/parts.c](../src/legoapi/parts.c) | 4 | 290 |
| [src/legoapi/takeoverobjects.c](../src/legoapi/takeoverobjects.c) | 1 | 31 |
| [src/legoapi/world.c](../src/legoapi/world.c) | 6 | 1,121 |
| [src/nu2api/nuanim.c](../src/nu2api/nuanim.c) | 3 | 252 |
| [src/nu2api/nudatlookup.c](../src/nu2api/nudatlookup.c) | 1 | 205 |
| [src/nu2api/nudebug.c](../src/nu2api/nudebug.c) | 2 | 2 |
| [src/nu2api/nufile.c](../src/nu2api/nufile.c) | 17 | 3,986 |
| [src/nu2api/nufile_pc.c](../src/nu2api/nufile_pc.c) | 2 | 9 |
| [src/nu2api/nufont.c](../src/nu2api/nufont.c) | 1 | 220 |
| [src/nu2api/nulgtlaser.c](../src/nu2api/nulgtlaser.c) | 10 | 6,203 |
| [src/nu2api/nuqfnt.c](../src/nu2api/nuqfnt.c) | 6 | 356 |
| [src/nu2api/nusound_music.cpp](../src/nu2api/nusound_music.cpp) | 1 | 757 |
| [src/nu2api/nusound_pc.cpp](../src/nu2api/nusound_pc.cpp) | 3 | 523 |
| [src/nu2api/nuspline.c](../src/nu2api/nuspline.c) | 1 | 93 |
| [src/nu2api/nustream.cpp](../src/nu2api/nustream.cpp) | 2 | 33 |
| [src/nu2api/nutex.c](../src/nu2api/nutex.c) | 1 | 66 |
| [src/nu2api/nutex_pc.c](../src/nu2api/nutex_pc.c) | 1 | 34 |
| [src/nu2api/nuvideo.c](../src/nu2api/nuvideo.c) | 1 | 220 |
| [src/nu2api/pc_device.cpp](../src/nu2api/pc_device.cpp) | 5 | 653 |

## Verification

* Required MSVC source build/link and whole object checks pass: 140 objects,
  1,915 function and 2,996 data definitions; every mapping/reference resolves.
* Whole retail matching and README badges are refreshed. Previous exact code
  and data comparisons are preserved.
* Full repository validation uses LLVM 18 formatting/tidy and 665 tests with
  three expected skips. Temporary fixtures use an ignored build directory to
  avoid the system temporary-directory quota.
* Native DAT initialization/archive/PAK/memory-file/packed/allocated-loading
  fixture passes, as do five RNC2 decoder execution cases.
* The source executable exits successfully under Wine/Xvfb through the explicit
  incomplete-input guard. Full gameplay remains untested and incomplete.

Retained commands/logs: `final-check.log`, `final-match.log`, `final-build.log`,
`entry-smoke-after-guard.json`, and each subsystem handoff in the execution
analysis directory. The initial failing entry smoke is also retained.


## Resource and debris continuation checkpoint (2026-10-03)

The user requested indefinite whole-project continuation, commits every half hour,
and pushes directly to main on opensagadev/saga-pc. The active goal and thread
heartbeat preserve this cadence. A checkpoint requires coherent passing source;
validation failures are repaired before committing.

From the pushed b22c8d0 checkpoint this wave adds **76 compared functions and
24,407 unique original PC code bytes**. Whole-code fuzzy coverage increases from
**7.710590% to 8.200580%** across the unchanged 3,471,246-byte PC text extent.
Exact code increases **2,417 bytes** to **55,532 bytes** (933 exact functions).
Every previously exact function and data comparison survives.

Transferred families include EdFile float/short/vector reads; bridge and grass
loaders, nearest/sort/page initialization, complete PC grass calculation including
dispatch tables, real wind/fade matrix groups, terrain/scene-special providers;
list/RTL/object resets; debris emitter/orientation helpers, generators, momentum
and chunk-control callbacks, ForceThrow and the AddDebris action; native particle
table generation; animation-cache/POI helpers; and real native texture creation.
Native texture creation uses the actual D3DX import through the existing
d3dx9_35 import library. No platform implementation surrogate was added.

The particle producer specifically omits Android's FLY patch, uses PC ARGB packing
and alpha doubling/clamping, and retains the float-rounded double UV multiplier.
Its full 1,855-byte PC body compares at 85.09%; a native fixture exercises all 64
interpolated frames, UV/color handling, exhausted slots and that platform divergence.
The exhaustion provider is a verified one-byte PC RET, with a neutral PC name.
Low-scoring full bodies remain recorded; FadeCreate has no reported objdiff score
and receives zero fuzzy credit rather than invented progress.

The single data writer adds **79 definitions**: 307 verified shared definitions
total, including real named debris dispatch pointers. Of those, 206 established
extents compare exactly across 88,072 original bytes; 101 bounded access views
cover 5,820 source bytes and receive no original allocation credit. Runtime group
capacities and allocations are not inferred from adjacent addresses.

Final aggregate validation passes: 149 objects, 1,991 function and 3,156 data
definitions, complete maps/relocations, source-only link, LLVM 18 formatting for
151 files and tidy for 149 units, and all 665 tests with three expected skips.
The linked executable exits successfully under Wine/Xvfb through the documented
incomplete-input guard. Full gameplay and real level resource loading remain
unverified. A transient header read during integration failed one lint run;
the final frozen-source run passes without weakening checks.

Remaining high-value families are the real scene remove/display/material/texture
animation services, GHG model loaders and upload/fixup providers, part-template
creation, and connected world lifecycle consumers. The existing
PartDebrisCreatePart_candidate remains explicitly partial: the emitter transfer
does not establish that part-template backend or the held variable/AddAnimEffects
backend. GHG/model code remains scratch until real providers and its private PC
register ABI are recovered. Keep these as source implementation waves rather
than repeating broad inventory audits.

Evidence and reproduction logs are under
build/analysis/transfer-execution-20261003/continuation-resources/, with current
subsystem handoffs under parser, mapping/resource, debris, apiobject, data and
world. Initial checkpoint evidence remains historical.


### Lifecycle and particle checkpoint (2026-10-03)

Against a00c901, the frozen release adds 51 newly compared functions and 12,218
unique PC code bytes. Whole-code fuzzy coverage rises from 8.20058% to 8.44939%
against the unchanged 3,471,246-byte PC text extent. Exact code increases by
1,497 bytes to 57,029; no previously exact code function is lost.

The connected release contains complete variable particle emission, genuine
finite PART creation replacing its old return-minus-one stub, camera axis bounds,
particle allocator/chunk/control/render providers, full AddAnimEffects with real
providers, scene native resource creation/destruction, texture animation cleanup,
material/ripple destruction, and grass/bridge storage and resets. Coherent original
helper/caller groups and genuine cross-unit calls remain intact. Two actual D3DX
DLL imports complete source-only linking; no DLL substitutes were introduced.

Producer inspection corrects native particle chunks to0x424 and controls to0x2c;
previous0x430/0x58 interpretations were reservation estimates. The display-scene
registry is an inline100-pointer access view, not the old four-byte pointer slot.
PART emitters start at291f7c8 with position before effect_id at+c and stride6c;
the old map incorrectly started at the interior effect field291f7d4. Its old
4,320-byte exact-data credit is withdrawn: the corrected full source view is zero
and independently compares, but original allocation size is not yet established.
These are evidence corrections, not byte tuning or hidden coverage gains.

All required validation passes: source-only link, 157 objects with2,042 function
and3,214 data definitions and complete relocations; LLVM18 format159 files/tidy157
units; 665 tests with three expected skips; full retail comparison. A pinned
native runtime fixture verifies arena partitions,32 particle initializers,
normal/glass chunk links, control recycling, render ordering/unlinks, and sphere
wrap/finite lifetimes. Full gameplay and real level loading remain unverified.

The embedded fallback DDS contains a varying original image asset with no source
initializer. Its five dependent removal/restoration/lazy-texture functions
(1,050 identified bytes) stay in ignored scratch under the repository prohibition
on committing game assets; no fake texture or dependency was substituted.
Dependent scene consumers remain held. Model/GHG private ABI, scene upload/fixup,
remaining particle allocation services and world lifecycle are the next targets.
Logs/deltas are retained under continuation-lifecycle, with subsystem handoffs.
The ongoing goal and30-minute heartbeat remain active; commits go directly tomain.


### Runtime and loader checkpoint (2026-10-03)

Against ca845ad, the release adds 41 newly compared linker identities and 15,992
unique PC code bytes. Whole-PC fuzzy coverage rises from8.44939% to8.70113% against
the unchanged3,471,246-byte text extent. Compared code is486,107 bytes; exact code
increases by1,348 bytes to58,377. All previous exact code functions are preserved.
Identity promotions and full-boundary corrections are included in the union delta,
so renamed functions and old truncated bytes are not counted twice.

The connected release includes particle growth/shrink, immediate/deferred/orphan
freeing and full control-chunk processing; genuine part initialization and page
lifecycle; editor spline and animation loading; native scene upload; the complete
cutscene end/destruction family and object-animation load caller; state animation
and special transforms; and display scene registration/frame resets/sort insertion.
Private original helper/caller groups remain coherent. The display reset receives
ESI plus a stack argument in retail and ESI/EBX in our private rebuilt group; that
register difference stays nonexact, with both callers retained and ordinary public
interfaces intact. Some large engine bodies receive zero advisory score; their
complete verified source and runtime checks remain present without invented credit.

The old60-byte two-dimensional-render stub was a truncated misidentification of
ResetSceneBeforeFramePC238. Its original caller had a wrongNUSORTPRI interpretation
and an omitted finalRET: the actual frame entry is188 bytes. The real group replaces
those implementations. The display registry is a256-pointer access view; its
original allocation extent remains blank. Four overlapping dynamic-scene scalar
owners are merged into one144-byte typed view with field aliases. Two old four-byte
exact-data credits are withdrawn because the whole aggregate extent is unproven.
The duplicate PCGrassTerrainScene storage is removed in favor of canonical
CurrentTerrain, so both consumers share actual rebuilt runtime state.

Independent InitParts producer evidence proves a4,320-byte clear at the corrected
PART emitter base291f7c8, followed by40 effect_id stores at stride6c. Its extent and
exact-data credit are restored from this evidence; the prior withdrawal remains a
historical correction. A separate unchanged callback's BSS padding initially broke
its comparison; isolating its real four-byte definition in a dedicated section
restores the complete comparison without truncation or changing its initializer.

All required checks pass after repair: source-only link,164 objects/2,081 function
and3,241 data definitions with resolved mappings; LLVM18 formatting166 files/tidy164
units;665 tests with three expected skips. The isolated DAT fixture now declares
the real loading-yield state required after its provider moved into nuthread.c.
The full suite and that parser fixture were rerun. The final data repair also passes
scoped project formatting/tidy, actual compilation and complete retail comparison.
Native fixtures exercise allocator limits/growth/expiry/control recycling and
collision, part/page cleanup, state-animation boundaries/NaNs, and display bitsets,
first/next frame buffers, sorted insertion and real critical sections. Full gameplay
and real resource loading remain unverified; entry scaffolding is still incomplete.

Remaining source gaps include embedded-image-dependent scene removal, material/
shader/vertex declaration cache closure, GHG/model loads, broad terrain/player/world
families and level-script callbacks. The next wave is preparing substantial player,
terrain and world/area cohorts plus genuine diagnostics. Evidence is retained in
continuation-runtime and the engine-next/pages, parser/lifecycle-next, scene-next,
world/cutscene and platform fixture handoffs. The goal remains active indefinitely.


## Terrain and movement checkpoint, 3 October 2026

The next full source checkpoint adds 18 newly compared functions spanning 9,927
unique original PC bytes. Compared code rises from 486,107 to 496,034 bytes,
whole-PC fuzzy coverage from 8.701130682656059% to 8.845562515159719%, and exact
code from 58,377 to 58,644 bytes. None of the previous exact code or data
comparisons is lost. Mapping-only movement and callback identities are excluded
from these source counts.

The connected release includes the full terrain initializer/reader and its
genuine one-byte diagnostic return; five movement momentum providers; particle
generation, trigger dispatch and allocation; the warning/native-debug backend;
level lookup/fixup and level-hack setup; Emperor fight initialization; and texture
animation list insertion. All bodies use reviewed PC boundaries, ABI and accessed
field cohorts. Some advisory scores remain low or zero, including the complete
Emperor initializer; no bytes are masked or omitted to improve them.

Independent review corrected the warning loop to check the cumulative limit on
every copied byte. Boundary-crossing tests at starting indices 1020 and 1022
pass against actual compiled providers; the former incorrect loop fails the
same test. The native format prefix has 12 bytes including its terminator.
Existing blank-string and format-string owners are reused through aliases.
Moving real error accessors into their original error subsystem preserves their
17 exact bytes and four previous frame exact functions, while the incomplete
NuFrameEnd comparison falls from 20.70412% to 2.329588% because VC8 loses
same-unit interprocedural knowledge. That nonexact change is included in the
net whole-PC result.

The terrain initializer preserves x87 sum precision through its minimum-height
comparison and integer conversion. Its new workspace name aliases the existing
TerrainScaleWorkBasePc owner; duplicate reconstructed storage was removed.
All 18 prior terrain bodies remain byte-identical after relocation. Particle
emission and pause formulas follow PC coefficients rather than Android's
different formulas. Emperor initialization omits the Android obstacle/force
operations that are absent from the complete PC body.

The sole data owner adds 61 verified definitions. Its scoped inventory is now 435:
248 established extents cover 133,853 unique original bytes, while 187 unknown
extents provide 15,343 source-view bytes without original allocation or exact-data
credit. The 128-byte object-switch view has a selected-input contract of the -1
sentinel or indices 0 through 127; its original allocation remains unknown.

All required checks pass: source-only link, 170 objects with 2,099 function and
3,310 data definitions and resolved mappings, LLVM18 formatting of 172 files and
tidy of 170 units, and 665 tests with three expected skips. Native fixtures use
real production providers for movement momentum, warning boundaries, and particle
generation/triggers. The particle fixture exercises genuine generation, random
timing, finite-count transitions, shrink/free and switch states; audio IDs are
disabled, so hardware sound playback is not claimed. Full gameplay and real level
resources remain unverified.

The next wave retains a compiled 5,322-byte movement backend group, larger
directional movement and terrain scanner/collision providers, Episode VI reset
and update families, and display scene publication. Fifteen reviewed immediate
movement callees and three reviewed children now provide symbol anchors; their
inventory entries alone carry no transfer credit. Rendering inventory errors are
corrected: 5f0470 is NuDisplaySceneClone with a complete 1,612-byte extent, and
the former combined sort range separates private clipping (688 bytes) from frame
publication (392 bytes). Broad callback hypotheses remain separate from accepted
PC identities. The ongoing goal remains active.


## Publication, scanning and movement checkpoint — 2026-10-03

Transferred 57 newly compared functions covering 27,055 unique original PC code
bytes. Compared code grows from 496,034 to 523,089 bytes. Whole-PC fuzzy coverage
grows from 8.8455625% to 9.2502337% over 3,471,246 retail text bytes. Exact code
grows from 58,644 to 59,426 bytes (+782); all prior exact code and data remain
exact. Inventory-only additions are excluded.

Connected families: movement backends (6,130 bytes), terrain scanners/native
division (9,351), display publication/native frustum (4,690), gameplay
bounds/force/boss/antinode helpers (2,316), debris clock/cache/camera setters
(2,468), Episode VI callbacks/terrain loader (2,100). Directional movement,
general combat, and shader/material/GHG remain next-wave work.

PC corrections include all 24 display registry slots in both publication copies,
preserving the front registry; 32 normal versus 12 glass timestamps; cache eviction
tie order; cached camera behavior; actual movement constants and field offsets.
Antinode radius selects x for equal or unordered comparisons, preserving PC
NaN/signed-zero semantics. Its final advisory similarity is 0%; it is nonexact.

The private display helper/caller group remains coherent ordinary C. One rebuilt
helper uses a stack scene parameter instead of retail ESI: a documented nonexact
internal ABI difference, with full comparisons and no register adapter.
ScanTerrain first score is 22.338854%, ScanWallSplineTerrain 27.37013%; complete
original extents, out-of-line tails and genuine actions are retained.

Real-provider native fixtures pass: display publication 96 pointer assertions
over all 24 slots/repeated updates, frustum 505 cases, debris clock isolation,
timestamp shifts, equal-age eviction, pool recycle, camera cache hold/update,
inverse ranges and rotation pointers. Source-only linkage remains an incomplete
game entry path; full gameplay and original level resources remain unverified.

Required aggregate checks pass: 665 tests with three expected skips, LLVM18
formatting and tidy over 180 translation units, and a source-only link across
180 objects with 2,156 function and 3,428 data definitions; all mappings and
relocations resolve. The native movement backend fixture also passes. All source
families remain frozen during final report refresh and direct-main push.


## Directional movement, shader, particle and geometry checkpoint — 2026-10-03

This release adds 59 newly compared functions spanning 30,650 unique original PC
bytes. Compared code grows from 523,089 to 553,739 bytes. Whole-PC fuzzy coverage
rises from 9.250233652% to 9.736648876% over 3,471,246 retail text bytes; exact
code rises from 59,426 to 60,691 bytes (+1,265), or 1.748392364%. All previous
exact function and data comparisons remain exact. The 61.05673% advisory score
among compared code uses a different denominator and is not whole-game coverage.

The connected groups comprise directional movement/private Walking/clipbox/feedback
(11,839 bytes), shader/native/material providers and genuine CRC startup/diagnostic
(5,568), particle renderer (2,737), display clipping/material update (712),
terrain geometry (9,200), and gameplay context/block/audio/menu/droid actions (594).
The actual directional call replaces its former marker. Walking's private ECX,
stack float and ST0 contract survives in its original caller group. Display clipping
moves the existing display TU to genuine C++ for its native frustum provider;
all 17 prior candidates remain byte-identical. Its reconstructed stack scene
parameter remains a documented nonexact difference from retail's private ESI
contract. No register adapter or invented helper mapping was added.

Independent review found and repaired three PC behavior errors. ScanTerrain now
sets platinrange only on accepted nonrotating/rotating branches, after capacity,
material, flag and clip checks. Six actual-production native cases pass; the
similarity falls from 22.338854% to 22.225605%, an honest correctness correction.
Shader material mode 2 clears first/second path flags while preserving deferred
state before layer counting. A fixture linked to the actual production manager
passes; the previous missing-branch object fails the same test with exit 12.
Copied fixture method definitions were removed from the validation claim.
InsideLineF no longer spills the side expression to f32 before testing its sign.
Retail compares the unrounded x87 sum: a tiny negative result must remain outside
even if a float store would round it to signed zero. The previous actual object
fails that discriminating case; the repaired real object passes 37 geometry
checks and preserves retail's direct FADDP-to-FCOMP contract. A proposed native
retail differential loader did not relocate the EXE under Wine, so these are
analytical PC-contract tests, not an executed retail equivalence claim.

Real-provider native fixtures also exercise Walking thresholds/ties/scales/NaNs,
nine display/frustum cases, and particle interpolation, gravity, color/UV,
two-particle output, near clipping, bank publication, cache behavior, software
buffer exhaustion, overflow, flush and repeat. The PC renderer uses four native
and eight software buffers, rather than Android's different buffer counts.
Hardware D3D paths and host rumble playback were not executed. CheckSphere and
HitWallSpline compare their complete verified bodies with zero advisory score;
broad runtime equivalence for those geometry routines remains unverified.

Canonical storage is shared: ATST IDs use the existing u16 owner with a signed
cast; clip camera position uses the canonical camera matrix translation; idle
uses the existing compiler string pool. The real CRUISERD_LDATA pointer storage
closes a link failure that object/map checks alone did not reveal. No fake
callee, duplicate mutable owner, copied retail implementation or DDS asset is added.

Required aggregate checks pass: source-only executable linkage across 191 objects,
2,215 function and 3,741 data definitions; all mappings/relocations resolve.
Pinned LLVM18 format/tidy and 665 tests with three expected skips pass. Complete
retail comparisons and regression receipts are retained under the ignored
continuation-native directory. The scoped data receipt is 798 definitions:
521 known extents cover 136,648 unique original bytes; 277 unknown extents expose
20,606 source-view bytes with zero unknown-allocation credit. There are 276 new
physical definitions plus one expanded legacy backend owner.

The selected vertex declaration cache is a single verified entry: initial empty
insertion and same-key reuse are within its source-view domain, while a second
distinct insertion is outside it. General cache capacity and full backend readiness
are not established. GHG/model bodies totaling 3,712 bytes remain held behind real
scene/material closure, including a noncommittable original DDS fallback asset.
Full gameplay, game entry completion and original level resources remain unverified.

The next retained wave is 14,915 original bytes of terrain solver/player/sphere
providers plus Mask, with finite header/data proposals; combat and DebrisDraw/portal
closure are prepared separately. The ongoing goal remains active. This checkpoint
exceeded the requested half-hour interval because automatic approval execution
timed out and integration exposed real ownership/provider and formatting defects;
the delay is not counted as progress.


## Terrain solver, clone, draw and carry checkpoint — 2026-10-03

Adds 34 newly compared functions covering 19,718 unique original PC bytes.
Compared code rises from 553,739 to 573,457 bytes. Whole-PC fuzzy coverage rises
from 9.736648876% to 10.058667661% over 3,471,246 retail text bytes; exact code
rises from 60,691 to 61,605 (+914), or 1.774722967%. Every previous exact code
and data result remains exact. Advisory similarity among compared code is
60.906567% with its different 573,457-byte denominator.

Connected source families are terrain solver/Mask (19 functions, 14,766 bytes),
terrain matrix getter/camera/death audio (3, 556), scene clone/native child (2,
1,670), particle draw/portal clipping (3, 1,594), and genuine carry/math/plug/
respawn providers (7, 1,132). Inventory entries and held candidates receive no
source credit. Five terrain field cohorts are shared without changing existing
function bodies. The original private respawn helper/caller context preserves
its actual ESI contract; its full 119-byte body is exact.

Independent real-object checks found and fixed two TerrainImpact rounding errors:
travelled products must survive position addition/movement subtraction, and contact
products must survive their cancellation. The old objects fail discriminating
cases (zero versus 273.581421, and zero versus -220.833664); corrected production
bodies pass all 35 checks, including deflection aliases and 16-sphere capacity.
Three Mask thresholds are actual f64 promotions of f32 .95/.003/.001, rather
than true double literals; all 53 emitted solver literals now match actual PC bits.
This is compiled-source testing against PC instruction expectations, not executed
retail equivalence. The proposed Wine retail loader did not relocate the EXE.

The matrix getter rereads CurrentTerrain after writing its first output because
that output can alias the global. The real alias case passes; the complete 49-byte
function is now exact. Scene cloning uses force-reset 1 and relies on its existing
sort provider's increment, removing a redundant caller increment. Actual production
selected-state tests pass and the wrong-force object fails with exit 12; all 21
old clone/native bodies remain byte-identical. Particle draw's full extent is
1,232 bytes through its final epilogue/return, not the truncated 1,228-byte candidate.

Native fixtures use real production providers for portal sphere/room filtering,
published particle vertices, mutable render scale, cache/header colors, time banks,
visibility and rotations; getter aliases; camera null alias, NaN/nonpositive duration,
disabled mode and signed mode selection; and 12 nearest-plug cases including ties,
filters, empty/negative counts and nonfinite distances. The particle hardware path
and the existing incomplete display-state camera action remain unverified.
Scene portal count 0..16 and particle material type 0..8 are selected source-view
domains, not new historical allocation claims.

The full source-only link succeeds across 194 objects with 2,249 function and
3,821 data definitions and resolved mappings/relocations. Pinned LLVM18 formatting
checks 196 source/header files, tidy checks 194 units, and 665 tests pass with
three expected skips. Final comparison and native fixture receipts are retained
under the ignored continuation-solver and owning subsystem directories.
A mapped terrain scale with no source storage initially caused a real LNK2019;
its PC zero initializer and setter now support the genuine data definition.
Its prior unproven four-byte inventory extent is cleared, with no prior exact-data loss.

The data owner adds 35 physical definitions. Its final receipt is 833 scoped
definitions: 522 known extents cover 136,652 unique original bytes, while 311
unknown extents expose 21,408 source-view bytes with zero unknown-allocation credit.
All 798 previous owned definitions are preserved. The optional death callback
and impact threshold use real NULL/4.0 initializers; no fake provider is introduced.

Remaining broad targets include TerrainPlayer's 7,994-byte caller, its finite
unmapped services, genuine carry Throw/PartKill/Clear/SpecialMove dependency closure,
and additional renderer/scene/resource families. GHG/model closure remains held
behind the original embedded DDS fallback asset. Full gameplay and original level
resources remain unverified. The unbounded whole-project goal remains active.
