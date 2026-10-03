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


## Terrain query, occlusion and pickup checkpoint (2026-10-03)

Compared with 5bb33cc, this release adds 18 actual compiled functions spanning
15,186 unique PC bytes: four terrain query kernels (11,923), six occlusion backend
bodies (2,225), water/platform/deflection and terrain setters (370), ReleaseHearts
(288) and MakePartVector (380). Compared original code grows from 573,457 to
588,643 bytes. Whole-code fuzzy coverage rises from 10.058667661% to
10.206869876%; exact code increases by 141 to 61,746 bytes (1.778784909%).
All previous exact code and data results remain exact. Similarity among compared
code is 60.209220%, with the 588,643-byte denominator.

The terrain kernels preserve the 16-entry cache, signed ages, group/platform
layouts, null handle guard, scaled polygons, filtered scan output and complete
post-return tails. Eight native cases execute the actual production object with
real scratch allocation, transformation and data providers: accepted/rejected
materials, group bounds, output limit, scaled/rotating polygons and scratch
restoration. ScanTerrainPlatform's advisory score remains zero across its full
3,582-byte span; this is a reviewed, tested but nonexact reconstruction.

Occlusion uses the PC 0xa0 record stride, projection/query matrices, actual depth
sorting, lazy preparation and query counters. A selected fixture links the final
production providers and canonical data; the wrong Android 0x98 stride fails a
discriminating two-valid-record case with exit 11. The private 95-byte matrix copy
remains with its real callers and is byte exact. Two verified scratch scene
fade/clipping bodies (1,036 bytes) are withheld because their only current caller
is a fixture adapter; the actual 454-byte scene caller still needs genuine
visibility/render services. No fixture entry is transferred into the game.

Water tests distinguish contact bytes 1/9 from high-bit variants, retain unordered
intersection comparisons and check output aliasing in deflection. Both terrain
setters add 46 exact bytes. ReleaseHearts has 19 real-source cases covering signed
health, active heart parts, difficulty, adaptive cheat scaling and random bounds.
The vector fixture uses real qrand, trig initialization, angle/rotation and scaling
providers, checking the null-direction branch's mutable scale/gravity, random
draw count, cone/norm and signed scale. Its complete 380-byte comparison remains
zero advisory similarity rather than being shortened or tuned for credit.

The source-only executable links across 199 objects with 2,267 function and
3,854 data definitions; all mappings and relocations resolve. LLVM18 formatting
checks 201 files. The 665 tests pass with three expected skips. Final lint and
comparison receipts are retained under the ignored continuation-frontier and
owning subsystem directories.

Eleven real globals add 44 source-view bytes. Two existing surface mask owners
are widened in place from one to four bytes: the existing GetSurfaceInfo consumer
still reads .low, while the reviewed TerrainPlayer contract reads .word.
No duplicate mutable owner is introduced. All thirteen four-byte initializers
match PC; historical allocation extents remain blank, with no extra allocation
or prior exact-data credit. The scoped data receipt now has 846 definitions,
522 known extents covering 136,652 unique bytes and 324 unknown extents exposing
21,460 source-view bytes. All 833 previous scoped proofs are preserved.

Next connected releases are the message producer/reset, genuine pickup callbacks
and constructors, carry graph, TerrainPlayer and further terrain scanners.
Full gameplay, hardware rendering and level resources remain unverified. The
unbounded whole-project goal remains active.


## Message producer and legacy literal correction checkpoint (2026-10-03)

Compared with d40c413, two actual production message bodies add 1,110 original
PC bytes, bringing compared code to 589,753.
Whole-code fuzzy coverage becomes 10.230999386%,
up from 10.206869876%; exact code adds 32 bytes
to 61,778. Every previous exact code and data
result remains exact. AddGameMsg1078 retains all late blocks beyond its sole early
RET and preserves the PC protected-slot overwrite behavior. ResetGameMessages32
is fully byte exact and clears only active flags, preserving the index and record
contents. Sixteen native cases link actual production providers and verify
wrapping, ratios/NaNs, flags, text bounds, real special rejection, protected-slot
overwrite, temporary records and callback-before-validation ordering.

A bounded audit of all compiled known-size nonexact readonly compiler literals
found an older incorrect reciprocal binding and three grass operation constants.
PointAlongSpline/GamePad_Rotate correctly use true 1/65536; their actual PC
operands read 0x00761610, so the alias moves there from the erroneous 0x00751700
binding. The pickup-vector promoted-f32 1/65535 at 0x00751700 remains independently
verified and unchanged. Grass case 1 must multiply by true 1.25, subtract from
promoted 1.2f and divide by promoted 1.7625f, retaining its float spills.
The two revoked wrong-value aliases are removed and the actual generated aliases
are bound from decoded PC operands and exact eight-byte initializers.

Actual production grass fixture scales are 1.29306257 and 1.18206549 for its two
unclamped seeds, with a separate lower clamp of 3 and matrix/pointer/guard checks.
The old three-constant source fails ten assertions against the same true providers.
This is a semantic correction, not new source-span credit or byte tuning.

The source-only link passes across 200 objects, 2269 functions and
3859 data definitions with complete mappings/relocations. LLVM18 format/tidy
and 665 tests pass with three expected skips. Two actual BSS owners add 35,332
source-view bytes: 128 message records and the separate index. Both initializers
match PC in full. Scoped data totals are 848 proofs, 522 known extents/136,652
unique bytes and 326 unknown extents/56,792 source-view bytes; no historical
allocation credit is inferred. All 846 prior scoped proofs remain unchanged.

The previous d40c413 checkpoint passed exact-head CI run 37127951525.
Next connected work covers the genuine pickup table callbacks/children,
terrain gameplay parents, KillParts, combat and UI score/menu services.
Their compiled scratch candidates remain distinct from released source.
The unbounded whole-project goal remains active; full gameplay is unverified.


## UI and gameplay parent checkpoint (2026-10-03)

Compared with c7a7e92, 27 actual production functions add 6,879 original PC bytes:
UI/menu/arcade services 2,078, jump/reset/water and their children 3,439,
score-message/coin-total services 782, LoseCoins 225 and player predicates/alert/
adaptive difficulty 355. Compared code reaches 596,632 bytes. Whole-code fuzzy
coverage rises from 10.230999386% to 10.398802651%; exact code adds 500 to 62,278
bytes (1.794110818%). Every prior exact function and data result remains exact.
Similarity among compared code is 60.519690%, with a 596,632-byte denominator.

The menu, score and arcade services retain PC pointer returns, menu remembered
selection, saturating/multiplied score arithmetic, actual player activity flags,
point goals and message icons. The selected native fixture links final production
objects and canonical globals without copied functions or replacement callees.
It passes; an Android both-players-active implementation fails the discriminating
negative fixture with exit 22. All 50 old functions in those original groups
retain their complete hashes.

PreReset retains its genuine private ESI helper in players.c; all observed reset
fields have named asserted local views. BigJump keeps the real 66-byte default
landing callback and initialized pointer in jumping.c. The complete callback is
byte exact and selects preferred/secondary/fallback model B IDs without the
Android variant switch. Water splash is byte exact across 122 bytes; NuLst lookup,
arcade activity, bonus winner, coin position callback and adaptive updater are
also exact. Full comparisons retain all other mismatches rather than tuning or
shortening them. Retained private reset fixtures cover 15 branches; full gameplay
remains unverified.

The source-only executable links across 203 objects, 2,296 functions and 3,947
data definitions with complete symbol and relocation resolution. LLVM18 checks
205 format files and 203 tidy units; 665 tests pass with three expected skips.
The first aggregate link caught an accidental replacement of panel.c's old
providers. Its complete HEAD prefix was restored before the successful link,
final lint and exact-regression comparison; no provider replacement remains.

Forty-three canonical shared definitions and one genuine private callback pointer
are added. Four complete NUL strings contribute 40 known bytes; 40 other source
views retain blank original extents. Shared initializer receipts cover 891
proofs: 526 known extents/136,692 unique bytes and 365 unknown extents/57,062
source-view bytes, preserving all 848 prior proofs. STATSPOSY, AddGameMsg_Default
and BuildUpTotal remain single mutable owners; alternate consumer names alias
them. Nine TerrainPlayer-only data definitions remain held.

AddCoinsToPanel's 854-byte source remains held because it needs the genuine full
pickup-type table: twelve nonzero callback slots, nine targets, plus the remaining
hint/power-up/message/camera services. No partial table or NULL substitutes are
released. Next connected work closes this table and the combat/context cycle,
KillParts and vehicle callbacks. The unbounded whole-project goal remains active.

A full audit checks all 2,269 previously compiled function copies: 2,267 retain
identical relocated bytes. Paired old/new-header recompiles reproduce two
nonexact scheduling changes. NuRndrParticleGroup loads the same four x87 values
in another order, reaches the identical loop-entry stack and removes two FXCH
instructions (717 to 713 bytes); its lower advisory score is retained.
DebReAlloc2 exchanges two independent ESI/EDI stack loads around FSTP with the
same 1,458-byte extent. No accessed fields, behavior or prior exact results
change. The paired evidence is retained under mapping/header-regression/.

The selected score/loss native fixture passes 42 checks using actual production
providers and canonical data. It covers bonus/adaptive-cheat scaling, coin
mutation before suppressed return values, NaN double-score time, unsigned
64-bit score accumulation/saturation, icon conversion, actual menus 19/20,
message reset, camera blending and repeat-sound queue behavior.


### Pickup and vehicle breakup checkpoint (2026-10-03)

Seventy-two newly compiled functions add 18,176 complete original PC bytes over
e34447e. Compared code rises to 614,808 bytes; whole-code fuzzy coverage reaches
10.799117% of 3,471,246 bytes, up 0.400315 percentage points. Exact code grows by
1,674 bytes to 63,952 (1.842336%); all earlier exact code and data remain exact.
This is transferred source, distinct from the larger accepted inventory.

The production subset includes 45 pickup, collection, hint and message providers
(8,001 PC bytes), the 19-function vehicle breakup group (8,423), coin-panel and
connected gameplay services. Both timed debris wrappers remain with their genuine
private matrix state in parts.c. TIE private register helpers remain with callers
in parts_breakup.cpp. Existing partial KillGameObject is reused honestly; its
remaining STUBBED paths and full gameplay are not declared reconstructed.

The actual ten-record pickup table retains twelve nonzero callback slots targeting
nine real functions. AddCoinsToPanel854 now uses that genuine table. Six new pickup
bodies totaling 1,408 bytes still need the missing renderer; three private helpers
totaling 583 bytes naturally stop compiling when those callers are held. AddPickups
and GizmoBlowUp_AddEffects (2,606 bytes) also remain complete reviewed scratch.
None receives source-transfer credit, and no renderer substitute closes the link.

The source-only executable links 206 objects with 2,368 function and 4,107 data
definitions; all mappings and supported relocations resolve. LLVM18 checks 208
format files and 206 tidy units; 665 tests pass with three skips. A native fixture
links the actual 206-object set and passes 23 checks covering messages, panel coin
conservation/suppression, interpolation, nearest-object filtering, ties and NaNs.
The independent real hint/panel fixture passes. All 2,296 prior compiled function
copies remain: 2,295 have identical relocated bytes. DebReAlloc2 restores the
previously verified equivalent scheduling hash, with equal extent and score.

Seventy new canonical shared definitions have complete initializer proofs covering
2,113 bytes. Forty-seven source views retain unknown original allocation extents.
The old 576-byte accessory allocation is preserved under one typed owner, never
counted twice. Next work closes genuine renderer material, shadow, wind and display
list dependencies, then releases the held pickup and combat callers. The unbounded
whole-project goal and direct-main checkpoint cadence remain active.


### Renderer closure and restored pickup checkpoint (2026-10-03)

Fifty-two newly compiled functions add 12,631 complete original PC bytes over
b146862. The source-only executable links 208 objects, 2,420 functions and 4,167
data definitions with all mappings and relocations resolved. Compared code reaches
627,439 bytes; whole-code fuzzy coverage is 11.071185% of 3,471,246 bytes, up
0.272068 percentage points. Exact code increases by 1,081 bytes to 65,033
(1.873477%); all earlier exact code and data remain exact.

Genuine material allocation/variants, wind, shadow clipping, display-list transforms
and the 1,491-byte special renderer close the six held pickup bodies. Their three
private helpers naturally compile again: 1,991 bytes total. AddPickups1284 and
GizmoBlowUp_AddEffects1322 now release with their actual renderer dependencies.
No renderer substitute, public private-register bridge or copied binary is used.
Material private helpers and callers remain in the original generic material unit;
platform creation remains separate. Its private ABI mismatch stays nonexact and
explicit. The positive-count pool domain is tested; the original uninitialized
nonpositive-count path is not replaced by invented NULL behavior.

Other connected providers include build-it reset393, rumble294, Snake549 plus its
matrix278 leaf, and arcade kill67/AI-kill258. Matrix rotation retains actual mutable
NuTrigTable[0]/[0x2000] reads and its qword product spill. PC shadow producer evidence
covers exactly four working pointer/result slots; unknown allocation extents remain
blank. PC material copies preserve fields28/2c and actual shader/vertex metadata.

LLVM18 checks 210 format files and 208 tidy units; all 665 tests pass with three
skips. The restored caller fixture strictly links all 208 actual objects and passes
28 guarded checks. Selected actual-object fixtures pass 25 material, 25 shadow,
15 reset, 15 rumble and 20 Snake checks. The transform fixture passes; replacing
the PC widened-float reciprocal with Android division fails with exit22. These
selected tests do not establish full gameplay, asset loading or hardware rendering.

All 2,368 prior function copies remain. Of these, 2,366 have identical relocated
bytes; the two changed copies exactly reproduce earlier independently proved x87
and integer-load scheduling hashes. Actual advisory changes remain in the report.
Twenty new canonical data definitions have full initializer proofs over 248 bytes:
one complete NUL string establishes 16 original bytes, while nineteen views totaling
232 bytes retain blank original extents. All 961 prior data proofs remain unchanged.
Next work releases the genuine combat/context, carrying/whip and TerrainPlayer
families by closing their finite remaining providers. The unbounded goal continues.


### Combat, contexts and genuine collision checkpoint (2026-10-03)

Twenty-four newly compiled functions add 13,774 complete original PC bytes over
043fbdc. Compared code reaches 641,213 bytes. Whole-code fuzzy coverage rises
from 11.071185% to 11.300798% of 3,471,246 bytes; exact code adds 659 bytes to
65,692 (1.892462%). All previous exact code and data remain exact.

The connected release includes ObjHitObj5764, shield520, player hit/deactivation
775, reaction739, Clear58, genuine private special-move helper/caller241, carrying
1296, whip163, item817 and InstantKillParts69. The real private collision1973
with both public17-byte wrappers replaces the earlier skipped collision action.
Deflection569, torpedo limit72, build/push release189 and sabre512 close its direct
providers. Original-TU private EDI/ESI calling contexts are preserved. LowWORD
sabre ID comparisons, promoted-float pickup delay, PC adaptive difficulty and
NaN branch behavior are retained. The collision body remains nonexact, with its
actual 0.05009634 advisory score; that score is not behavioral equivalence.

The source-only executable links 213 objects with 2,444 function and 4,253 data
definitions; all mappings and relocations resolve. LLVM18 checks all213 tidy
units; 665 tests pass with three skips. A fixture strictly links the actual213
production objects and passes39 selected checks for guard/reset ordering, NaNs,
context masks, real private helper calls, torpedo limits and collision rejection.
Successful collision/damage/effects, full gameplay and hardware remain unverified.
Existing partial AddPart, KillPart and KillGameObject services stay explicit.

All2,420 prior function copies remain:2,418 are byte-identical. PartCollide_3D
now implements its actual17-byte callback. The other change exactly restores the
previously proved713-byte particle scheduling hash with equivalent x87 loop state;
its advisory decrease is retained. Twenty-nine canonical data initializers129B
are fully proved: three NUL strings38B have known extents, while26 mutable views
91B retain blank extents. All981 prior data proofs remain unchanged.

The half-hour checkpoint was delayed to integrate and validate the actual collision
family with its combat callers. Next comes genuine vehicle engine/audio/weapon
428 and private thrust/launch/deactivation, then Move_VEHICLE1789/PostReset111
and reviewed TerrainPlayer8287. The whole-project continuation remains authorized.


### Vehicle leaves, reactivation and grapples checkpoint (2026-10-03)

Twelve newly compiled functions add 2,108 complete original PC bytes over a140055.
Compared code reaches 643,321 bytes; whole-code fuzzy coverage is 11.349925% of
3,471,246 bytes, up 0.049127 percentage points. Exact code adds 234 bytes to
65,926 (1.899203%). All previous exact code and data remain exact.

The connected providers include engine/audio/weapon routines 428, StartFlatten454,
DropInOut196, mutable gunship hover143, CatchUp350, ActivatePlayer94, camera
hit judder186, dynamic grapple allocation/removal191 and SetWeaponIn66.
PC engine pitch spills, drop NaN hold, signed player selector, looped socket wrap
and camera ordered guards are preserved. Grapple allocation searches existing
attachments before empty slots, keeps duplicate retention, and preserves the
actual pointer return from removal despite Android's void signature. Its full
121-byte allocation extent includes the final LEA/RET omitted by advisory118.

The source-only executable links 216 objects, 2,456 functions and 4,277 data
definitions with all mappings and relocations resolved. LLVM18 checks 218 format
files and 216 tidy units; all 665 tests pass with three skips. Actual-object
fixtures pass 14 engine/flatten, 18 drop/hover/catch, 80 reactivation/camera and
20 grapple contract checks. These cover meaningful selected behavior; full game,
asset loading and physical audio/input/rendering remain unverified.

All 2,444 prior function copies remain, with 2,443 byte-identical. The particle
copy changes 713 to 715 bytes: independently verified x87 load reordering and one
extra exchange leave identical stack state. All remaining 186 instruction pairs
and 34 relocations agree; actual advisory changes remain in the report. Six new
canonical data views have complete initializer proofs over 1,012 bytes and all
1,010 earlier proofs remain unchanged. All six original extents stay unknown.

The next connected wave is the reviewed 3,380-byte vehicle/float/tractor family
and 1,051-byte Loop/Twist/Hint family. Private weapon/target and vehicle groups
continue in genuine caller contexts. Move_VEHICLE1789/PostReset111 still require
real provider closure before TerrainPlayer8287 can link. No replacement vehicle
body or fabricated private ABI wrapper is used.


### Vehicle movement, targeting and weapon checkpoint (2026-10-03)

Twenty-eight newly compiled functions add 9,085 complete original PC bytes over
9c0d056. Unique compared code reaches 652,294 bytes; native objdiff's denominator
is 652,406 because overlapping aliases remain visible. Whole-PC fuzzy coverage
rises from 11.349925% to 11.550616% of 3,471,246 bytes. Exact code adds 640 bytes
to 66,566 (1.917640%); all previous exact code and data remain exact.

Connected providers include Float's actual private53-byte helper and883-byte
caller, rumble/carrying/rotation/disorientation/tractor routines, genuine Loop955
with Twist24 and Hint72, ResetForceGlow51, TakeHit866, three force queries219,
six target selectors/setters2575 and five item/weapon providers943. Original
private helper/caller context, actual PC phase constants, mutable table reads,
registry reloads after calls and exact callback stack conventions are retained.
The source-only executable links216 objects,2484 functions and4325 data
definitions with all references resolved. No replacement private-register bridge
or artificial storage provider is introduced.

LLVM18 checks218 format files and216 tidy units; all665 tests pass with three
skips. Actual compiled-object fixtures pass18 float/tractor/carry checks,19
Loop/Twist/Hint cases,4 glow reset canary patterns,12 TakeHit behavior checks
plus12 guards,21 force-query checks and13 target-selection checks. TakeHit's
death/pickup paths remain untested and call genuine existing partial services.
SlowWeaponIn retains the original negative-action array access: neither a new
guard nor a fake callback replaces that behavior. Full game/assets and physical
audio/input/rendering remain unverified.

All2456 prior function copies remain;2454 are byte-identical. The particle and
DebReAlloc2 copies retain actual nonexact scores, with independent review of their
changed relocated bytes. Seven new canonical initializer views total22 bytes;
all1016 previous views remain unchanged. All seven original extents stay unknown
and receive zero allocation credit. The sole writer's scope reaches1023 proofs.

This half-hour checkpoint waited for the coherent targeting/weapon integration
and final comparison review. Next releases the independently repaired Awkward
plus true six-argument ray query and full switch-table impact helper1471. A
genuine-object fixture distinguishes the PC's second carry-position output: the
corrected source passes22 checks, while the prior wrong output fails its regression
case. Cannon/Shoot/FindID, Force/POD and Move_VEHICLE then connect the held terrain
parent. Parallel retained inventory replenishes resource/render/UI/audio families;
the whole-project mandate continues.


### Terrain, force selection, particles, world configuration and PC music checkpoint (2026-10-03)

This batch transfers 23 functions over 9,135 complete PC bytes from 4bf989d:
movement/terrain support 1,829, traffic/faders 1,658, PC music/stream 2,219,
particle emission 1,441 and force selection 1,988. Unique compared code reaches
661,429 bytes; native objdiff compares 661,541 including overlapping aliases.
Whole-PC fuzzy coverage rises from 11.550616% to 11.743011% of 3,471,246 bytes.
Exact code adds 520 bytes to 67,086 (1.932620%). All 2,484 earlier function copies
remain identical and all previous exact code/data remain exact.

The source executable links 218 genuine objects with 2,507 function and 4,367
data definitions. LLVM18 checks 220 format files and 218 analysis units; 665
tests pass with three skips. Actual source-object fixtures pass 22 movement/ray,
30 traffic/faders, 39 music, 21 particle and 11 force checks. Wrong carry-output,
music-state and signed-cache controls fail the corresponding tests as expected.
These selected contracts do not establish full gameplay or hardware playback.

AwkwardShapeCode uses the PC's second carry-position output, and zero ray scale
uses inverse 1.0. RayImpact includes its complete native switch tables. Music
retains actual PC state transitions, promoted-double varargs, original C++ ABI,
real cross-unit queue providers and the executable's genuine one-byte diagnostic.
Traffic callbacks and private storage remain with their original caller units;
the particle emitter uses the actual PC identity-matrix owner and scene indices.

The force selector preserves the actual unguarded writer. Original PC capacity
and allocation extent remain UNKNOWN. Its 384-record working view is qualified
by cumulative eligible writes <=384, receives no allocation credit and is not
claimed as recovered historical capacity. Group expansion and nonempty terrain
are outside the fixture. Existing partial AddPart/death services remain explicit.

Eleven shared initializer views add 6,356 bytes with unknown owning extents;
all 1,023 prior views remain unchanged. Six private traffic owners emit 396 bytes,
with 23 independently checked initializer regions totaling 548 bytes. Unknown
views receive no full-allocation credit. NuMusic::Process's incomplete 1,398-byte
inventory span becomes its complete 1,432-byte extent; unproved LevelTimer16 and
the old music constant4 inventory extents are withdrawn.

The 19:32 checkpoint waited for final data, required checks and production
contract verification. The next connected candidate group is Cannon350, eight weapon
functions8,170 and four private helpers1,841: at most10,361 PC bytes. Release
counts will include only actual emitted functions with genuine caller context;
Shield/SmartBomb/Dodgem await the true Move_VEHICLE caller. Parallel
resource/render/UI work continues from the retained bank.


### Connected Cannon, weapon firing and creature bolt selection checkpoint (2026-10-03)

Ten newly compiled functions transfer9,130 complete PC bytes over d214711:
FindID1,317 in bolts.c and nine genuine move.c bodies7,813 (Cannon350,
Shoot3,449, Fire1,460, StartQuickShoot553, ShootThisFrame160, Deactivated1,327,
filters232/109 and ZapTarget173). Whole-PC fuzzy coverage rises from11.743011%
to11.955131% of3,471,246 bytes. Unique compared code reaches670,559; native
objdiff's denominator is670,671. Cannon350 and ZapFilter109 add459 exact bytes,
reaching67,545 (1.945843%). No previous exact code or data is lost.

The10,361-byte candidate ceiling was reduced to actual emitted9,130:
Shield/SmartBomb/Dodgem1,231 await the genuine Move_VEHICLE caller. No artificial
retaining calls, public register bridges or unrelated Thrust/Launch bodies were
added. The original move caller group retains its real private register ABIs;
the MindTrick helper's rebuilt register swap is coherent with its actual callers.
The CHARACTERDATA+18 callback is proven by actual cdecl invocation and genuine
Cannon function identity. Weapon ammunition is an inline8-byte record at7f4,
with item pointer at7f4 and ammo BYTE at7f8; unsigned phase/latch bytes preserve
native comparisons and wrapping increments. Minimal other views remain local.

Pinned build/link and all2517 function/4421 data definitions pass across218
objects. LLVM18 checks220 format files/218 analysis units;665 tests pass with
three skips. Actual current-production fixtures pass15 Cannon/weapon cases and
39 FindID cases, including unsigned128 rejection,255 wrapping, real consumable
ammo/phase, native callback identity and signed mutable IDs. Fixture inputs
explicitly select gravity/Awkward/animation guards; full gameplay/assets remain
unverified. Existing real partial services remain explicit and unchanged.

All2507 prior function copies remain,2506 identical. The unchanged particle source
rebuilds713->719 bytes; independent full-code/x87/relocation review establishes
ordinary finite-float equivalence. Its actual88.32124 score stays nonexact;
unmasked signaling-NaN exception order is outside that proof. A scratch source composition
step accidentally omitted seven source definitions; the complete-object gate
caught this before release, and genuine retained fragments restored the group.

Forty-five required mutable initializer views add112 bytes, all with unknown
allocation extents/no allocation credit; all1034 previous regions remain
unchanged. Four unused Shield/SmartBomb/Launch owners remain excluded. Five genuine
compiler-owned NUL/numeric literal regions add53 independently verified bytes.
Next release targets rendering/API/root-motion7,663 with minimal shared storage,
alongside search/text/input providers. Cutscene10,374 and level audio9,548 remain
connected research while genuine missing callbacks and Beast services close.


### Rendering, root motion, audio services and target searches (2026-10-03)

Thirty-three actual newly compiled functions transfer10,384 complete PC bytes
over73fb2ab: rendering/API/root motion19/7,663, public movement4/1,100,
Torpedo searches3/918 and level audio services7/703. Related C routines are
consolidated into their existing owners; the genuine pulse subsystem adds
pulses.c. Whole-PC fuzzy coverage rises from11.955131%
to12.197465% of3,471,246 bytes. Unique compared code is
680,943; native objdiff's denominator is681,055. Twelve new exact functions
add852 bytes, reaching68,397 (1.970388%). All previous exact code/data survives.

Pinned build/link and all2550 function/4456 data definitions pass across219
actual objects. LLVM18 checks and665 tests pass with three skips. Selected
actual aggregate-object fixtures pass27 rendering/root/queue cases,31 audio
service cases,21 movement cases and20 search cases; reversed root interpolation
fails exactly three expected controls. These fixtures retain real game
providers and mutable storage. Assets, general GPU submissions and active
turret/special/obstacle search paths remain outside the selected fixture domain.

Review repaired StartSlide's VehicleArea early return before any object read;
StartSlide(NULL,0) is now tested with VehicleArea enabled. Root callbacks retain
their physical floating-point/stack ABI and the real vector interpolation
operand order. Shadow packets preserve signed count and state-byte behavior,
WORD/BYTE truncation and the native280 guard. The280x104-byte working view does
not establish the original allocation capacity. No guard or fake provider was
invented. ForcePushed_MoveCode712 remains held without a genuine Beast caller;
it receives no transfer credit.

All2517 prior compiled function copies remain,2515 identical. The unchanged
particle and debris sources reschedule; independent full-code/relocation review
is retained with actual new nonexact scores. Every prior data initializer is
preserved. The sole writer adds23 required complete initializer views/29,215B,
all retail-equal but unknown original owning extents/no allocation credit.
Next release is the connected cutscene loader/rendering/callback family10,374B
plus its genuine1B diagnostic, alongside retained text/input and Beast closure.
Authoritative review confirms GetMusicIndex is genuinely constant-1 in this PC
build; the earlier proposed missing music lookup was an incorrect qualification.


### Cutscene callbacks, text/input and real Beast providers (2026-10-03)

Fifty-three actual functions add10,724 complete PC bytes over451acbe:
37 cutscene callbacks4,044,13 text/input/target-draw bodies6,406, MiniAnim reset
and part search225, and InStory49. Whole-PC fuzzy coverage rises from
12.197465% to12.389966% of
3,471,246 bytes. Unique compared code reaches691,667; native objdiff's
denominator is691,779. Thirty-three new exact functions add1,251 bytes,
reaching69,648 (2.006427%). No previous exact code or data is lost.

The existing cutscene_runtime.c owner becomes.cpp for the real NuMusic method
call. Its complete previous source prefix and all three function bodies and
relocations are preserved. The genuine43 installed slots plus terminator retain
37 static callbacks and the full352-byte table in that owner. The table's
original owning extent remains unknown/no allocation credit. Configure510,
Listed720, Load1333, Draw3767 and their genuine diagnostic remain held; none
is forced into the build with fake references or public ABI adapters.

Text review repaired actual behavior before release: inclusive screen boundaries
and unordered coordinates, unordered width/scale branches, reversed input
types5/6, sufficient34-slot label access and producer-proven custom4x32 rows.
The actual device-link producer establishes3x12 records, not32x24; its raw
initializer is zero, separately from runtime reset values. The singleton
reuses nu_input_state. Genuine keyboard names use a238-pointer working view
with146 distinct pointees,144 new and two canonical reuses. Capacity remains
unknown. Target drawing consolidates into existing torpedo.c; the39-byte font
generic wrapper retains the real cross-unit backend call. Existing font visual
submission gaps remain explicit; selected fixtures do not establish visuals.

Pinned source build/link, all2603 function/4758 data definitions across221
objects, LLVM18 checks223 format files/221 units and665 tests (three skips)
pass. Final fixtures pass58 callback/story,32 MiniAnim/part and30 text/input
cases using only all221 actual normal objects and real data, without providers
or globals substituted. Every prior data initializer remains unchanged.
All2550 prior function copies remain,2549 identical, including the reviewed
three-owner rename. The unchanged particle source's717-byte rescheduling has
independent full x87/CFG/34-relocation proof for finite initialized inputs;
unmasked NaN exception order remains outside that proof. Its real89.98446
score is retained. The checkpoint waited for final validation rather than
committing the frozen batch while checks were still running.

The sole writer adds180 required initializer views/1964B with unknown owning
extents/no allocation credit. Next is the genuinely connected Beast/Punch/
opponent group, with true Beast identity closing the retained8845-byte level
audio parent. Cutscene load/fixup/create providers remain separate bounded work.


### Beast/Punch, targeting, level audio and torpedo parents (2026-10-04)

Twenty-eight actual functions add23,892 complete PC bytes over25b0680:
Beast/Hold/fall/barrier8195, public Punch services2350, opponent selection/range1408,
level sound setup8845 and torpedo parents3094. Whole-PC fuzzy coverage rises from
12.389966% to12.952078% of3471246B.
Unique compared code reaches715559B; native objdiff denominator715671B and
advisory62.837555% remain separate. Five newly exact functions add770B,
reaching70418B (2.028609%). No previous exact code or data is lost.

The real Move_BEAST224 and its original static caller/helper group retain private
register interfaces naturally. Punch includes the full3000-byte native body and
switch table, restored combo-style chooser BYTEe41, StartupGameFlag1 range split,
quiet unordered timer/hit-frame paths and real public children. Its rebuilt
register choices remain nonexact; no assembly, public adapters or artificial
retention calls are added. StartHold104 joins that actual owner. Range selection
uses the true WORD27a heading rather than roll27c; the radius square must round
to f32 before comparison. An intentional old unrounded-square control fails
exactly the discriminating boundary case. GizmoBlowUpOpponent's complete724-byte
extent corrects the earlier uncompiled718-byte inventory truncation.

The full8845-byte audio parent uses actual Move_BEAST identity, existing audio
services and199 verified compiler-owned literal regions. A C partition retains
the retail's real public cross-unit calls; historical original TU identity is
unproved. Combining with the existing C++ helper owner inlined113 lookups and
changed codegen substantially; no compiler pragmas, flags or fake shims replace
the real interfaces. EWOK/WICKET readers use canonical signed WORD ID owners.
Torpedo parents reuse the three target searches, real target callback and input/
text providers. The XWing_Torpedo view includes all14 bytes with NUL; markerX
reuses the existing Text key owner rather than duplicating storage.

Pinned build/link, all2631 function/5068 data definitions across223 objects,
LLVM18 checks225 files/223 units, and665 tests (three skips) pass. Native replay
passes206 cases: Beast30, Punch49, opponent31, Torpedo49 and level audio47, each
using all223 actual normal objects with no substitutes. Intentional reverse
Beast identity fails two audio checks; wrong radius-square fails one opponent
check. Poo and audio fixture faults were invalid test inputs: actual readers
require level_objects[24] and camera_splines[19..23]. Real typed inactive/null
tables repair the fixtures; no production guards or missing providers are added.
Active visual/audio/emission and complete gameplay remain outside these fixtures.

All2603 previous function copies remain;2602 are identical. The unchanged
particle717->713 body equals the independently proved historical variant:
only FLD/FXCH scheduling differs, two FXCH are removed, all186 other instructions/
CFG edges and34 relocations remain. Initialized finite f32 stack behavior is
preserved; unmasked signaling-NaN/invalid-memory trap order remains unproved.
Its actual88.419685 nonexact score is retained. The checkpoint waited for final
native replay and this bounded retained proof, preserving the user journal.

The sole data writer adds80 required views327B, all with blank original owning
extents/no allocation credit. All3051 complete data regions agree between actual
normal and frozen isolated objects. Eight older uncompiled pointer inventory
extents lack allocation proof and become blank, retiring32 inventory bytes with
no previous exact loss. Next are the compiled4478-byte cutscene kernel and its
2564-byte real loader/private callers, with resource and Vehicle/POD families
prepared concurrently. Draw3767 remains separate bounded work.
