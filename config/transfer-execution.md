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


### Vehicle/POD/terrain and connected cutscene loader (2026-10-04)

Fifty-one actual functions transfer 24,861 complete original PC bytes over b9c64dd:
Vehicle/POD/terrain/PostReset22/17,819 and cutscene kernel/loader29/7,042.
Whole-PC fuzzy coverage rises from 12.952078%
to 13.494166% of 3,471,246B. Unique compared code is
740,420B; native objdiff compares 740,532B including overlapping aliases and
reports 63.269043% separately. Eleven new exact functions
add 1,147B, reaching71,565B (2.061652%). Every previous exact code/data match survives.
All 2,631 prior complete function copies remain identical.

The genuine private movement group now contains Vehicle/POD/Force/glow/shield/
smart-bomb/cable helpers and callers. Actual ForceGlow 1,204 includes its native
post-return switch table; CreateCable 147 and CableTarget 517 close Cable 362,
which matches completely. Cable records use WORD flags at 1ea and stride 1ec,
not Android BYTE 1e9. Minimal working array views retain unknown owning extents.
Terrain includes the complete 7,994-byte parent and real 91/202-byte helpers.
Both naturally emit after removing unsupported noinline annotations. The source
is C89 with all 7,461 executable/controlflow tokens and initializer order preserved.
Its threshold comparison keeps the retail's extended x87 value, removing the prior
CPP DWORD spill absent from PC. Actual nonexact 53.880543% remains in the report.

Cutscene relocation/fixup/instance creation retains genuine private caller
contexts and actual register-interface evidence. The real 510/720-byte Configure/
Listed helpers are naturally retained by public Load 1,333 in the existing CPP
runtime owner; no retention pins or public private-ABI adapters are added.
Listed has no invented capacity guard; its working view is qualified by at most
32 eligible entries. Five loader states are distinct from the older draw-system
pointer. Original callbacks, table and NuMusic implementation remain unchanged.

Required LLVM18 checks 226 files/224 units, pinned build/link and all 2,682 function/
5,175 data definitions pass; 665 tests pass with three skips. Final native replay
passes 170 checks using all 224 actual normal objects without substitutions:
Vehicle 18, cable 55, loader 74 and kernel 23. A wrong Android cable field fails 19
controls. Hover fixture initialization now calls the real NuTrigInit startup
provider; the earlier omitted-init failure remains retained. Selected loader
cases use the genuine memory/file parser and real Configure/Listed/story gates.
Successful asset construction, complete terrain gameplay and active hardware
visual/audio paths remain untested; inherited real partials stay explicit.

Forty-nine required physical views add 254 source bytes: four complete immutable
NUL strings 44B have known extents,45 mutable/pointer/array views 210B remain
unknown and receive zero allocation credit. Scope 1,411 retains all 1,362 prior
proofs; all 3,100 complete named normal data regions equal the frozen isolated
object. The checkpoint waited for final review and validation before committing;
the user's journal remains untouched. Next are the accepted renderer 4,745B
(including spline 1,309 once), UI 1,903+67B, resource 966B and prompt 1,277B waves.


### Renderer, spline, UI and input source checkpoint (2026-10-04)

Twenty-six actual functions add 6,715 complete PC bytes over dcaa8f6: renderer
16/4,745 (including two spline functions 1,309B exactly once), UI/font 7/1,903,
and input routing 3/67. Whole-PC fuzzy coverage rises from 13.494166% to
13.617748% of 3,471,246B. Unique compared code reaches 747,135B; native objdiff
compares 747,247B and reports 63.274574% separately. Fourteen new exact functions
add 1,121B to 72,686B (2.093946%). All 2,682 previous complete function copies
remain identical; no previous exact function or data match is lost.

The source includes the real spline interpolation/distance readers and their
asserted PC resource fields, level spline setup, particle shutdown and genuine
effect cleanup, surface reset, shadow mode, camera focal conversion, status/icon/
backdrop rendering, widescreen and NoRender. The original font owner contains
both the matrix wrapper and complete 1,345-byte justified-text body, with the
real 110-byte wrapper and existing diagnostic provider. UI adds customiser names,
intro text and icon scene lookup; input setters preserve signed BYTE truncation,
the real six-frame keyboard countdown and device10..13 test. No fake providers,
artificial retention calls, public private-ABI wrappers or flag tuning are used.

Pinned build/link and all 2,708 function/5,223 data definitions across 228 actual
objects pass. LLVM18 checks 230 files/228 units; 665 tests pass with three skips.
Final replay passes 216 checks using every current normal object and genuine
normal data, without substitutions: renderer40, analytical spline151, UI/input25.
Selected typed records cover native state, signed sentinels, normal interpolation,
font justification and input routing. Full asset loading, hardware visual/audio
submission, arbitrary NaN/out-of-range spline inputs and complete gameplay remain
outside the tested domain. Existing real partial font/graphics services stay explicit.

Nineteen shared physical views add 138 source bytes: two complete NUL strings24B
have known extents,17 mutable/pointer/array views114B remain unknown with zero
allocation credit. Scope1,430 retains all1,411 prior proofs. All3,119 complete
normal data regions equal the frozen isolated object. Three unsupported inventory
extents retire10B with no old compiled or exact loss. The checkpoint waited for
final validation beyond its half-hour deadline; the user's journal is unchanged.

Resource966 is deliberately held: its canonical48B reset storage proposal preserves
physical initializers but would retire two previously exact WORD records totaling
4B. Named field views need an honest verified matcher representation before that
storage can merge. Next connected work combines Draw/RTL26/6,122 with the real
CutScenes_Draw/locator34/10,654 graph, larger UI/prompt and renderer providers,
plus closed animation/gameplay subsets. Those candidate bytes are not progress.


### Draw, RTL, UI and cutscene locator checkpoint (2026-10-04)

Ninety complete functions add 27,288 newly compared unique PC bytes over62bf2b3:
Draw/RTL26/6,122, prompts6/1,277, renderer7/885, UI8/4,860,
cutscene drawing/locators34/10,654 and connected locator backends9/3,490.
Whole-PC fuzzy coverage rises13.617748% to14.222798% of3,471,246 bytes.
Unique compared code reaches774,423B; native objdiff compares774,535B with
63.756966% advisory similarity. Forty-four new exact functions add4,374B,
reaching77,060B (2.219952%). No earlier exact code or data match is lost.

Of2,708 previous complete function copies,2,707 remain byte-identical. The
nonexact DebReAlloc2 copy changes only two independent integer MOV loads around
an unchanged FSTP; independent decoding verifies identical control flow and all
30 relocations, with unchanged36.482307 advisory similarity. This is recorded
as a reviewed codegen change, not an identical prior copy or exactness gain.

Connected source now implements RTL light selection/application, global lighting,
shadow queues, real UI wrapping and prompts, full SmartTextEx and DrawMenu,
cutscene callback installation and drawing, locator transforms/reflections/VFX,
scaled effect cache creation/reuse and real generator/axis publication. Private
helper groups stay in their actual source owners with natural VC8 private ABIs.
No fabricated providers, retaining calls, ABI wrappers or compiler controls were
added. Named shared fields include light records, particles and the native1,064-byte (0x428)
debris effect stride; minimum views do not prove original allocation capacities.

Pinned build/link and all2,798 function/5,368 data definitions across231 actual
objects pass. LLVM18 checks233 files/231 units;665 tests pass with three skips.
Nine final native fixtures pass391 selected checks: Draw/RTL110, UI103,
renderer69, locator backends67 and cutscene drawing42. Every fixture uses all
current normal source objects and genuine normal data with zero replacements or
supplements, and root verifies each source/object hash. This establishes selected
state, callback, cache, UTF8 and geometry behavior. Complete hardware visuals,
audio, assets, animated locator modes4/5 and gameplay remain unverified; existing
real partial font/graphics providers remain explicit.

Seventy-three shared physical views add3,488 complete source initializer bytes,
all with unknown original extents and zero allocation credit. Scope1,503 preserves
all1,430 earlier proofs; all3,192 normal named data regions equal the frozen
isolated object. The final link caught missing CutStopInfo storage. Its actual PC
pointer use and zero initializer were independently verified and supplied; its
unsupported uncompiled4B inventory extent became blank. All required checks and
normal fixtures were rerun. The half-hour checkpoint was delayed for these gates.
The user's three journal edits remain untouched and excluded from the commit.

Next is independently accepted gameplay/resource39/14,110 with genuine37-view
Init/POD613B data closure. These are candidate bytes until final production
integration and matching. Resource966 remains held for preservation of two
historical exact WORD records4B; NewPlayerCharacter1433 requires caller-arity
proof. Neither hold stops the other connected source transfers.


### Player animation, creature initialization and character transfer checkpoint (2026-10-04)

Forty-three complete functions add15,543 newly compared unique PC bytes over
source-identical report refreshaf4c378: gameplay/animation23/8,142,
dynamic/init/POD15/5,942, character-shadow reset26 and character transfer4/1,433.
Whole-PC fuzzy coverage rises14.222798% to14.540193% of3,471,246B. Unique
compared code reaches789,966B; native objdiff compares790,078B with63.897194%
advisory similarity. Seventeen newly exact functions add1,298B, reaching78,358B
(2.257345%). Every2,798 earlier complete function copy remains identical; no
previous exact code or data match is lost.

Original source owners now contain real animation timer/packet updates, player
capability/move/AI initialization, object allocation, idle/reset and footsteps,
creature initialization and layer selection, dynamic creation, gamepad allocation,
signed Suit lookup, player-start lookup and POD parsing/creation. Character
transfer retains its four-word PC ABI despite the unused fourth parameter. The
independent review corrected native BYTEe22 bit1 to DWORDmask10000, rather than
Android's low bit. Idle reset uses the actual PC blendout time. Plain private
AnimBlendModePc stays naturally emitted in its real API TU; no data_seg, new
retaining controls, ABI wrappers or fake providers were added.

Pinned build/link and all2,841 function/5,444 data definitions across235 actual
objects pass. LLVM18 checks237 files/235 units;665 tests pass with three skips.
Four final native fixtures pass328 checks: gameplay/shadow75, actual allocator/
lookup/guard states167, successful controlled InitCreature22 and character
transitions64. Every fixture uses all current normal objects and genuine normal
data, without replacements or supplements. Character cases exercise both the
native10000 flag and unrelated low/high bits, routes, frame state, signed health
and velocity. This establishes selected successful initialization and transitions,
not full game/asset loading or general AddDynamic/POD success; existing hardware,
font/graphics/audio partials remain explicit.

Fixture failures were diagnosed without changing production: long immutable
snapshot paths exceeded Windows command length; short byte-identical paths fixed
linking. Native Scale/GetTopBot/Dimensions order proves initial height0 with later
y-scale1, so the successful InitCreature fixture retained22 assertions with the
correct expectation. A character fixture omitted real CDataList, then supplied
the legitimate typed input and passed all64 checks. No assertion was dropped to
hide a provider or source failure.

Thirty-seven shared physical views add613 complete source initializer bytes,
all blank original extents/zero allocation credit. Rich24-byte Suit records use
real relocated names/text IDs and actual PC magnet mask200000. Scope1,540
preserves all1,503 earlier proofs; all3,229 normal data regions equal the frozen
isolated object. Thirteen genuine private selection arrays43 declared/59 emittedB
and natural private state4B stay in their source owners; their original extents
remain unknown. Three compiler f64 literals24B are separately verified. User
journal edits remain untouched and excluded from commits.

Next coherent packets span effects25/8,022, visual creator4/1,707 plus real
repairs to1,860 already-compared font/primitive bytes, and the music/configuration
startup spine. They remain candidates pending independent source/data gates.
Resource966 and graphics fallback40 aggregation remain held for preservation of
prior exact physical WORD/scalar bytes; a typed data-member contract is only a
scratch proposal and has not weakened current checks or entered production.


### Effects, graphics submission and music configuration checkpoint (2026-10-04)

Eighty-one new complete functions add14,300 unique original PC bytes over9b1c3ad:
effects25/8,022, graphics creators/dispatch7/2,052 and music/configuration/math
49/4,226. Whole-PC fuzzy coverage rises14.540193% to14.871450% of3,471,246B.
Unique compared code reaches804,266B; native objdiff compares804,486B and reports
64.193184% separately. Fifty newly exact functions add2,513B and the previously
compared PrintU becomes exact108B, yielding+2,621B/51 functions:80,979B
(2.332851%). No previous exact function or data match is lost.

All2,841 previous complete function copies remain;2,836 are identical. Exactly
five accepted nonexact copies change: PrintCharW and both primitive begins repair
missing native operations; PrintU and JustifiedRSW change naturally with the real
font helper context. The complete1,860 previously compared repair bytes receive
no new transfer credit. One genuine additional108-byte font helper copy increases
the native denominator by108 while unique coverage counts it once. An independent
normal-object audit confirms every81 new body and all4,302 earlier complete
initializer copies, including effects142 and audio31 unchanged prior bodies.

Effects include native sphere/torus queries, sorting/priorities, clocks/cleanup,
shifts, geometry, editor deletion, actual material/texture children and Setup.
AllCollision executes torus side effects but returns-1 after a sphere miss. Setup
copies0x10a DWORDs (1,064 bytes/0x428), clears4 cache bytes per slot and uses texture
ID1. Cone uses promoted2.8f, preserving a discriminating one-ULP result. The new
frame callback setter is its actual4f4040/91ef24 owner, distinct from the existing
5c9ff0/9738a4 callback. Null-template fields have native initializer/layout evidence;
the field-stream witness is Android source, with no new PC reader identity claim.

Graphics adds real allocation/600-slot dynamic creator, full executor and complete
2D primitive/shader/COM draw wrappers. Dynamic creator extent1012 includes its
previously clipped finalRET and retains two physical arguments. Point dispatch
reloads count after SetRenderState. Font now emits real colors, positions, UVs and
quad/triangle command streams; nonfallback primitive linking/transforms are real.
Fallback40 and global hardware dispatch population remain held. Music now uses
real file/parser/arena callbacks, string rebasing, sound tables and i32 fixed-point
pitch4096. Positive DUCK/ATTENUATION above1 remain unchanged, as in native PC.
NuExp10 uses its actual lazy10000-float table, NuAlloc, CRTpow and OOM retry; no
replacement provider, new retention control, public private-ABI adapter or tuning
flag is introduced.

Pinned build/link and all2,923 function/5,518 data definitions across236 actual
objects pass. LLVM18 checks238 files/236 units;665 tests pass with three skips.
Final normal fixtures pass204 checks: effects94, visuals48 and music/math62.
Every fixture uses all current normal objects and genuine normal data, without
replacements or supplements. Wrong collision return fails1, wrong music clamp
and threshold fail2 each, and the earlier partial font fails10 discriminating
checks. An effects fixture initially omitted the actual active editor page and
free-key pool; supplying legitimate typed caller inputs fixes the pagefault while
retaining all94 assertions. Production remains frozen throughout. Setup/material/
device success, GPU drawing, music playback/full startup/assets and full gameplay
remain untested; inherited partials and allocation preconditions stay explicit.

Twenty-four shared physical views add1,199 initializer bytes, all unknown original
extents and zero allocation credit. Scope1,564 preserves1,540 old proofs;
all3,253 complete normal named data regions equal the isolated object. All3,301
prior exact data owners/4,286 actual copies preserve complete widths, bytes and
relocations, including fallback12B and resource4B. Private music tables/keywords
and NuExp10 state stay in genuine original TUs; compiler literals retain separate
actual-owner proofs. User journal edits remain unchanged and excluded from commits.

Next connected work covers retained resource/model/scene/area-load drafts, native
audio SDK initialization/shutdown and remaining visual providers. The embedded
fallback DDS8,320B is a game asset, retained only in ignored research; its getter
closure remains held with canonical existing cache/path, rather than fabricated
storage. Physical resource966/fallback40 aggregation also remains held without
weakening exact-data preservation.


### Resource, world startup, terrain and native audio source checkpoint (2026-10-04)

Thirty-seven new complete functions add9,432 unique PC bytes overb83d90d:
resource resolver224; obstacle controls/progress11/1,035; players/collection/area
startup5/1,865; customiser/archive3/336; terrain/scene/state7/3,065; and native
audio manager/stream10/2,907. Whole-PC fuzzy coverage rises14.871450% to15.065450%
of3,471,246B. Unique compared code reaches813,698B; native objdiff compares814,013B
and reports64.280820% separately. Fifteen new exact functions add614 unique bytes,
reaching81,593B/2.350539%. A genuine private scene matrix-copy95 alias reproduces
exactly at the existing Occlusion helper VA and receives zero new unique credit;
38 newly compared rows include that alias, adding9,527 to the native denominator.

All2,923 previous complete function copies survive;2,922 remain identical. The
one reviewed existing SetVolume body repairs a real ABI bug: retail pushes both
buffer and volume, calls SDK slot3c and performs no caller cleanup. The former
thiscall declaration was incorrect; canonical shared sink now uses stdcall. Its
complete82-byte PC extent remains unchanged; rebuilt72 becomes73 and advisory
score honestly decreases83.906250 to81.562500. It receives no new transfer credit.
No previous exact code or data match is lost. Every38 accepted complete normal
body agrees with independently reviewed candidates, including full native tables.

Connected source now includes real bounded torpedo reservation, remembered
player IDs, collection/minikit lookup and player reset/init; customiser animation
flags/18 texture IDs and archive switching; obstacle visibility/activation,
progress and SFX; texture references; terrain reset/platform aging/bounds/bounce,
scene time/visibility and state lookup; ordinary audio storage/unload methods,
stream destruction/format/fill and worker command dispatch. Native audio command4
pauses and5 resumes; negative-ready command2 still clears its timer, and buffers
are reloaded after external decoder/COM/timer calls. Shared declarations preserve
actual stack/thiscall/stdcall contracts, natural private register helpers and
prior attributes. No new constructor, fake decoder/vtable, compiler control,
retaining call or ABI adapter is introduced.

Pinned build/link and2,961 function/5,544 data definitions across236 actual objects
pass. LLVM18 checks238 files/236 units;665 tests pass with3 skips. Five strict
normal-object fixture groups pass245 selected checks: resolver16, scene32, terrain29,
startup59, obstacle68, customiser/archive16 and native audio25. Every fixture uses
all236 actual normal objects and genuine data with zero replacements/supplements;
root verifies each source/object/snapshot pin. The preserved wrong audio version
fails2 timer controls. Fixture corrections supplied the mandatory scene-special
table and actual16-byte gizmo allocator alignment; all assertions remained and
production source stayed frozen. Successful player creation/restoration, asset
loading, scene curve/backend matrix evaluation, constructed decoder/hardware
playback/rendering and whole-game startup remain outside tested domains.

Twenty-two shared views add13,916 initializer bytes, all unknown owning extents
and zero allocation credit. The actual existing manager view grows3,480 to3,496B,
preserving the complete old prefix plus a zero16-byte tail. All other3,252 prior
complete shared regions remain identical; all3,275 current named normal regions
equal the isolated object. Scope1,587 includes1,564 unchanged prior proofs,
22 new physical owners and one previously unscoped existing manager owner. Known
559/137,619 unique bytes stay unchanged; unknown1,028/120,737 source bytes receive
zero allocation credit. Private TorpedoPackets[16]/2,176B stays in torpedo.c.
All4,315 actual previous exact initializer copies retain complete sizes/raw bytes/
symbolic relocations;3 additional literal copies are retail-exact. Unsupported
uncompiled TerrPlatScanDist extent4 becomes blank, retiring4 mapped inventory
bytes without previous source/exact loss. Its genuine scalar initializer is10f.

The retail manager+d14 overlaps exact canonical reverb BYTE; existing separate
source storage remains an explicit global-domain hold, with local manager cases
qualified. Physical resource966/fallback40/member tooling stays research only.
Asset-dependent4581 readers remain held on embedded DDS8,320B, retained only in
ignored research. Next are accepted connected environment loader2,101B/bridge487B,
world startup providers, native Wav/Ogg families and reviewed optional physical
member tools. This checkpoint waited past the half-hour deadline for actual
source/data integration, independent semantic fixes and final validation. The
user's three journal edits remain untouched and excluded from commits.


### Environment loaders, world setup and music lookup checkpoint (2026-10-04)

Twenty-four complete source functions add 4,574 unique PC bytes over 86d0cd4:
environment loaders 12/2,101; obstacle reset and SFX 3/670; bridge editor 2/487;
world initialization providers 6/1,234; and music action lookup 1/82. FindPlatInst
is counted once. Whole-PC fuzzy coverage rises from 15.065450% to 15.168865% of
3,471,246 bytes; unique compared code reaches 818,272 bytes. Native objdiff
compares 818,587 bytes including existing aliases and reports 64.360176%.
Four new exact functions add 397 unique bytes, reaching 81,990 / 2.361976%.

Real connected source now reads gizmo names and specials, animation sets and
obstacle records, reserves and postloads obstacle pools, resets obstacle state,
resolves blowup types, creates bridge editor pages, initializes doors and dynamic
lights, fixes level platform IDs and scans the music action table. Reviewed
private helper ABIs remain natural original-owner compiler output. PC animation
serialization requires a DWORD at offset20; the named alias union preserves
existing BYTE readers and object size28. Reverting only the two new DWORD
accesses causes 10 targeted failures, including real obstacle callback cases.

All 2,961 previous complete function copies survive; 2,959 are identical. Two
old nonexact functions change only compiler scheduling under the expanded header.
Exact baseline headers reproduce both old endpoints: NuRndrParticleGroup rebuilt
713 becomes717; DebReAlloc2 remains1458. Independent full CFG, instruction, x87
and 34/30 relocation review qualifies initialized finite values. Fault ordering
and unmasked signaling NaNs are outside that proof. Actual current advisory
scores remain in the report; these functions gain no new transfer credit.
No previous exact code or data is lost.

Pinned source EXE build/link and 2,985 function / 5,562 data definitions across
236 actual objects pass. LLVM18 checks 238 files and 236 units; 699 tests pass
with 3 skips. The first general run failed temporary-storage quota, then the
unchanged full suite passed using workspace TMPDIR. Four strict all-normal
fixture groups pass 236 selected checks: loaders/reset151, bridge35, world
support39 and action lookup11. Root verifies every current source/object pin;
zero replacements or supplementary providers are linked. The support fixture
initially omitted genuine NuTrigInit; adding required input initialization
preserves all 39 assertions and frozen production source. Successful whole-game
startup, assets, registry modes, animated scene backends and hardware output
remain outside these tested domains.

Eight shared physical views add 1,057 initializer bytes: two known literals15B
and six unknown views1,042B with zero owning-allocation credit. All 3,275 old
complete shared regions and 1,587 scoped proofs remain unchanged. Current 3,283
normal regions equal isolated compilation; scope1,595 has known561/137,634 unique
bytes and unknown1,034/121,779 source bytes. All 4,318 previous actual exact
initializer copies preserve full widths, raw bytes and named relocations; eight
additional copies are retail-exact. ActionPairTab's unsupported uncompiled4B
inventory extent becomes blank; its genuine NULL pointer view implies no table
capacity and loses no prior source or exact match.

Optional typed physical-member tools are implemented and independently reviewed,
with 34 additional tests; the project registry remains disabled. CSV retains
four actual-COFF columns. Members never become physical definitions or address
bindings, complete parent relocation uses existing tools, and previous exact
member history cannot be silently removed or disabled. Fresh resource48 proof
preserves two exact WORDs4B and all 3,280 other data regions, with165/168 consumer
bodies unchanged and three qualified old nonexact getters. That migration stays
inactive until the next source wave. Unknown parent48 receives no allocation
credit. Next are connected resource966, natural WavReader1,408, accepted startup
providers1,728 and larger environment/Vorbis families. No retail asset bytes are
committed. This checkpoint waits for completed quota recovery, full validation
and scheduling review; the user's three journal edits remain untouched.


### Resource reset, WAV reader and world startup checkpoint (2026-10-04)

Thirty complete source functions add 5,483 unique PC bytes over bfa 2863:
WavReader 15/1,408; resource reset, menu and lap parsing 3/966;
player positions, doors, episode lookup and shadow 6/1,728; item lookup 4/351;
and WorldInfo_Init plus the real renderer RET 2/1,030. No new alias rows are
counted. Whole-PC fuzzy coverage rises from 15.168865% to 15.295465% of
3,471,246 bytes. Unique compared code reaches 823,755; native objdiff compares
824,070 including old aliases at 64.465256%. Fourteen new exact functions add
630 bytes, reaching 82,620/2.380125%. Full-size nonexact implementations remain
visible with their actual scores.

The resource reset uses one genuine 48-byte physical parent. MiscTime and both
TargetDeg WORDs become named typed fields at 0x20/0x24/0x28, with original member VAs
and the two exact WORDs 4 B preserved by active compiler-generated member proofs.
Member records never become COFF definitions or relocation bindings. Parent
allocation extent and MiscTime extent remain unknown, with zero allocation
credit. ClearLevData can now reset the complete reviewed working view without
separate conflicting global storage. Four retained PC-name aliases use narrow
legacy naming exceptions; one redundant extern declaration was removed.

All 2,985 prior function copies survive: 2,982 full relocated bodies identical,
including 30 copies whose original C owners become C++ with unchanged C linkage.
Three old nonexact targeting routines change compiler scheduling under the
reviewed member access; fresh final endpoint review preserves native offsets,
semantics and complete relocations. These changes gain no new transfer credit.
All 4,330 previous exact initializer copies are preserved: 4,328 remain physical
and two become complete typed members. Current physical exact copies 4,334
plus two typed members include six new physical copies. Newly compared exact
Wav literals/TypeDescriptor add 34 unique data bytes. All previous exact code
and data remain exact, including the two retired physical WORD rows now
represented by their complete typed members.

Pinned source EXE link, 3,015 function and 5,609 data definitions across 239 actual
objects pass. LLVM 18 checks 241 files/239 units; 699 tests pass with 3 skips.
Strict runtime fixtures link all 239 normal objects with no replacements or
supplemental providers. Wav 40, resource 36, item 15, startup helper 37 and actual
parent 39 checks validate generated PCM/real Win32 file APIs, selected reset/menu/
lap cases, item bounds, doors, camera-pair wrapping, real Players_InitPositions
and WorldInfo_Init callbacks/progress/room/pool/timer/shadow state. Full startup,
positive audio/hardware paths, asset loading and character creation remain
outside these finite domains. The parent fixture initially assumed an exact 0.5
corridor ratio; native trigonometric sampling gives 0x3f000001. Independent source/PC review approves
a two-ULP bound while preserving all 39 parent assertions and discriminating
bonus/camera/callback checks. Production source remains unchanged.

Thirty-seven new physical shared views replace three old field definitions,
net 34 physical additions. Four existing views grow 103 bytes; physical source
storage grows 557 bytes. All 3,276 other complete shared regions remain identical,
four prior prefixes and three field initializers remain preserved. Current
normal 3,317 regions equal isolated compilation. Scope 1,631 contains 561 known
regions/137,634 unique bytes and 1,070 unknown views/122,340 source bytes; the two
known typed members 4 B are reported separately. Ten unsupported, previously
uncompiled inventory extents totaling 40 bytes become unknown; no earlier source
or exact match is removed. No retail assets or fabricated providers are added.

Next is the accepted connected obstacle/animation runtime 22/5,183 plus real
geometry/Shove 6/2,200: 28 functions/7,383 PC bytes, pending genuine shared data and
all-normal runtime checks. Broader Ogg/Vorbis and audio sample-manager banks
continue separately. The checkpoint waits for coherent final verification;
the user's three identification-journal edits remain untouched.


### Obstacle, animation and CPU collision checkpoint (2026-10-04)

Twenty-eight newly implemented source functions compare 7,383 PC row bytes over
25a8333. One exact 138-byte NuPointRelToBoundingBox implementation shares a
previously compared native span: the unique newly compared union is 7,245 bytes.
Unique compared code reaches 831,000; native objdiff compares 831,453 including
aliases at 64.604500%. Whole-PC fuzzy coverage rises from 15.295465% to
15.461953% of 3,471,246 bytes. Exact union grows 412 bytes to 83,032/2.391994%.
The obstacle update/mode callbacks, triggers, controls, bolt targeting, blowup
lookup/type action, animation reflection, draw, Shove queue and CPU box/line
geometry retain complete native extents and real source provider closure.

Runtime testing found an actual transfer error: Proximity used a BYTE field
for a native DWORD test of bit 0x1000. That candidate could never select eligible
objects. The initial static approval was withdrawn, both eligibility tests now
use the asserted DWORD view, and independent final native review accepts the
repair. Signed low-BYTE behavior stays intact. A named two-WORD local payload
replaces four raw object-data accesses without changing emitted code. All-normal
fixtures pass 139 checks: environment 88, geometry 28 and Shove 23. Deliberately
restoring the wrong BYTE predicate fails two checks; the wrong NaN predicate
fails one. No assertions were removed or relaxed. Native unordered rejection
branches, rather than an Android conjunction, determine the NaN behavior.

All 3,015 previous full function copies survive; 3,014 are identical. The sole
old nonexact NuRndrParticleGroup difference repeats a previously independently
verified x87 scheduling pair, 717 to 713 candidate bytes, with unchanged source,
identical symbolic stack at dispatch and all 34 relocation destinations intact.
Its advisory score remains honestly 88.419685%. All previous exact code/data and
the two exact typed resource members, 4 bytes, remain preserved.

Pinned source-only EXE link and map/relocation checks pass for 3,043 function and
5,626 data definitions across 240 actual objects. LLVM 18 checks 242 files/240
units; all 699 tests pass with 3 skips. Each native fixture links all 240 normal
objects, with zero replacements, supplemental providers, fake globals or retail
runtime code. Domains cover initialized mode 0..7, real animation/pool producers,
empty animation sets, valid triggers/object contexts, finite CPU bounds and a
real Shove queue with counts 0..16. Positive device/audio/destructible effects,
reflected scenes and full-game execution remain outside these finite fixtures.

Seven physical shared views add 143 source bytes: one complete 59-byte NUL
literal and six mutable unknown views totaling 84 bytes. All 3,317 old shared
regions remain unchanged; current 3,324 normal regions equal isolated compilation.
Scope 1,638 has 562 known regions/137,693 unique bytes and 1,076 unknown views/
122,424 source bytes. All 4,334 previous actual exact physical initializer copies
remain intact; nine new copies yield 4,343, plus the two unchanged exact members.
The natural private 32-byte mode callback table and 388-byte Shove queue stay in
their genuine original owners; full initializers match, owning extents remain
unknown, and no allocation credit is assigned.

The 05:19 UTC checkpoint waited for full matching, normal data/fixture proof and
qualification of the recurring compiler difference. The user's three journal
edits remain untouched. Next releases are the accepted player toggle/reset/
Move_DEFAULT family, 3/3,113, and genuine LZ2K decoder, 13/2,332, with 15 shared
views/4,128 source bytes. A genuine libogg 1.1.3 container bank, 68/6,239, undergoes
independent release review. Proven libvorbis 1.2.0 vorbisfile source reproduces
63/14,513 exact PC bytes; its genuine core-codec provider closure remains pending.
These research bytes are not current reconstruction progress. DAT caller buffer
capacity remains unresolved; no large fabricated backing array is introduced.


### Player reset, LZ2K, Ogg and FFT source-bank checkpoint (2026-10-04)

One hundred newly implemented complete source functions add 33,582 unique PC
bytes over 6b78f90, with no new aliases: player toggle, individual/group AI reset
and Move_DEFAULT 4/3,529; genuine LZ2K decoder 13/2,332; original libogg container
68/6,239; original libvorbis FFT 15/21,482. Whole-PC fuzzy coverage rises from
15.461953% to 16.403316% of 3,471,246 bytes. Unique compared code reaches 864,582;
native objdiff compares 865,035 including older aliases at 65.873970%. Eighty-eight
new exact functions add 27,433 bytes, reaching 110,465/3.182287%. Full nonexact
routines retain complete original extents and actual advisory scores. All 3,043
previous full relocated function copies remain identical; no old exact code,
data or typed member is lost.

Using the correct original source versions enables a substantial direct source
transfer. The native vendor identifier is Xiph.Org libVorbis I 20070622, matching
libvorbis 1.2.0. Its complete original smallft.c owner reproduces all 15 functions,
21,482 bytes, exactly under the unchanged pinned compiler. Original libogg 1.1.3
reproduces 67/5,713 exact bytes; its full flush routine is nonexact but has verified
equivalent register initialization/flags and complete CFG. Original BSD licenses
remain in the sources. Algorithms, native types, natural private helper groups
and original initializer ownership remain intact. No transcribed retail assembly,
asset bytes, fabricated providers, retention changes or compiler tuning is used.
Only the verified original Ogg long/short declaration block has a narrow naming
policy exception, preserving its actual Win32 C types and 45 layout assertions (5 sizes and 40 field offsets).

Player reset restores the PC angle WORD at 0x27a, unsigned model/difficulty WORD
comparisons and low-CL disabled-mask semantics. Move_DEFAULT is the genuine exact
50-byte provider. Natural individual/group reset and movement owners preserve
all 156 prior copies; five generated local-label names change compiler ordinals
but retain identical actual target addresses, types and addends. The decoder keeps
its genuine private EDI-buffer/stack-size caller/helper group and full working
initializers. Its diagnostic calls real NuFileError rather than Android exit(1).
The existing two DAT callers, 928 already-compared bytes, remain unchanged and
held: no native proof establishes a 0x40000 decode-buffer allocation.

Pinned source-only EXE link and map/relocation checks pass for 3,143 function and
5,671 data definitions across 244 actual objects. LLVM 18 checks 246 files/244
units; all 699 tests pass with 3 skips. Strict fixtures link only all 244 normal
objects, with zero replacements, supplemental providers or fake globals. Player
46 checks cover toggle callbacks/timing, individual AI reset and once-only/group
masks, unsigned difficulty and NaN stagger behavior. Decoder 28 checks cover
legal generated LZ2K literal/overlapping-copy/ring/pending-copy/Huffman inputs.
Ogg 57 checks cover bit packing, truncation, generated page/packet encode-decode,
CRC, peek/consume/reset. FFT 58 checks cover radices 2/3/4/5/7, sizes 1..128,
unnormalized roundtrips, independent impulse/DC/cosine spectra and canaries.
All 189 checks pass. These finite CPU/container domains do not establish decoded
audio, device/GPU execution, asset/model creation or full-game startup.

Fifteen real shared working views add 4,128 source bytes, including two Huffman
working trees, actual decoder scalars and genuine player input/callback state.
All 3,324 prior complete shared regions and 1,638 prior scoped proofs remain
identical. Current 3,339 normal regions equal isolated compilation; scope 1,653
has 562 known regions/137,693 unique bytes and 1,091 unknown views/126,552 source
bytes. All 4,343 previous actual exact physical initializer copies survive;
nine new copies yield 4,352, plus the two unchanged exact typed members, 4 bytes.
New exact data union grows 15 bytes. Two unsupported previously uncompiled
4-byte inventory extents become blank while their genuine 4-byte scalar views
remain. No allocation capacity or original owning extent is inferred from them.
Private implode/Ogg/FFT tables remain in genuine original owners.

Next is the genuine full Vorbis reader/core source bank. Its actual supplier graph
closes with 19 original core owners including the released FFT, plus Ogg and 42
confirmed static-CRT suppliers. Eighteen remaining original core owners and the
63-function/14,513-byte reader can form one coherent source release. The full core probe
223/93,791 includes the released FFT 15/21,482; the remaining core 208/72,309 plus
reader 63/14,513 gives 271/86,822 potentially new bytes, still candidates until review. Seventeen private
name collisions have naming-only whole-code preservation proof; 29 nonexact
identities and 33 emitted data regions remain PC gates. Library-local private
headers preserve original context without polluting unrelated game owners.
Connected blowup animation/loader/zipup research proceeds in parallel, with a
20–30 KB candidate wave and finite genuine parser/registration frontiers.
The user's three journal edits remain untouched; whole-project work continues.

The 05:59 UTC scheduled checkpoint completed at 06:02 after the final
documentation/input-fingerprint refresh; exact revision 909b644 passed CI.


### Vorbis core, gameplay loaders and transport checkpoint (2026-10-04)

286 complete new functions add 92,837 unique PC bytes over 909b644: original
remaining Vorbis core 208/72,309 and reader 63/14,513; blowup callbacks, allocation,
versioned loader and anti-node helpers 12/5,639; ZipUp selection 1/311 and unsigned
EdFile readers 2/65. The existing FFT is excluded from new credit. Whole-PC fuzzy
coverage rises from 16.403316% to 19.021145%. Unique compared code reaches 957,419;
native objdiff compares 957,872 including older aliases at 68.976240%. 252 new
exact functions add 65,407 bytes, reaching 175,872/5.066538% of 3,471,246 bytes.
No old exact function, initializer or member is lost. Of 3,143 old full copies,
3,142 remain identical; unchanged particle source recompiles to the reviewed
713/717-byte scheduling pair in reverse direction. All 34 relocations, complete
CFG and qualified x87 stack behavior remain equivalent; the current 717-byte
body remains nonexact at its honest 89.984460% advisory score.

The genuine libvorbis1.2.0 source bank preserves nineteen original owners and
all 271 complete functions/179 initializer regions. 242 functions/64,453 bytes
are exact; the other 29 retain independently reviewed full CFG, precision and
cold tails, including compiler-owned switch tables. Native preextrapolate source
is accepted despite a low alignment-sensitive score; its encoder-only runtime
is untested. All 42 required CRT suppliers are actual pinned static-library
providers. Nineteen absent reference bindings enter the map with blank owning
extents and no source credit. A genuinely unused, unbound four-byte _seq is
omitted without invented identity or allocation credit.

Original local headers, algorithms, types and BSD license remain with the library.
A fixed 24-header catalog is allowed only there; unrelated project and vendor
headers remain rejected. Genuine lookup.c is named lookup.inc and included by
its original owner, rather than compiled twice. Relative include names preserve
the general existing /Isrc compiler context. Private header/table dependencies
are tracked for the library owners. All formatting and static analysis remain
required; 74 individual diagnosed upstream naming exceptions preserve original
identifiers. Complete normal code/data output remains equal to the accepted
upstream recipe. The earlier Ogg declaration count is corrected to 45 layout
assertions: five sizes and 40 field offsets. Compiler/retention settings stay pinned.

Gameplay preserves typed 0x100-byte types and 0x114-byte instances, status DWORD
at 0x94, drawing DWORD at 0x98 and type animation BYTE at 0xfc. Early update uses
the native unordered/equal radius choice; a single-choice wrong variant fails
one fixture assertion. Arena wrappers use the actual unsigned WORD capacities,
16/128-byte alignment and real GameBufferAlloc. The blowup loader follows the PC
version protocol, including legacy flag migration and distinct vector offsets;
all five complete NUL literals match. ZipUp selection uses the actual six-argument
cdecl ABI and three endpoints, with no Android touch argument. Unsigned EdFile
returns occupy AX/AL; upper EAX is unspecified and native callers handle those
widths. No new backing capacity or device/provider definition is fabricated.

The source-only EXE links with 3,429 function/5,863 data definitions across 264
actual objects. LLVM18 validates 291 source/header files and 264 units; all 700
tests pass with three skips. Strict fixtures use all 264 normal objects and no
replacements, supplemental providers or fake globals: gameplay callbacks and
allocation 55, versioned loader 224, decoder 80, ZipUp 23 and unsigned readers 14;
all 396 pass. Generated legal mono/stereo streams decode to 4,096/8,192/6,144 frames
against an independent installed libvorbis1.3.7 decoder, maximum float error
1.79e-7. Loader fixtures cover versions 2/11/19/26/27/28/30/31 with actual stream,
arena and scene services. These selected domains do not establish audio devices,
GPU/destructive effects, instance animation evaluation success, malformed-input
capacity, general encoding or full-game execution.

One genuine shared unit-scale vector adds twelve unknown-extent source bytes.
All 3,339 old shared regions and 1,653 scoped proofs survive; 3,340 normal regions
match isolated compilation. Scope 1,654 has 562 known regions/137,693 unique bytes
and 1,092 unknown views/126,564 source bytes. All 4,352 previous exact physical
initializer copies remain intact. 83 new known exact copies yield 4,435, plus two
unchanged exact members/four bytes. All 192 new complete initializer copies are
retail-equal; 109 have unknown original extents and receive no exact or allocation
credit. The 4,544 verified-copy union must not be called exact coverage. Five known
NUL literals add 57 exact data union bytes. Genuine private codec owners retain
their own initializers.

The 06:32 UTC checkpoint waited for complete native/CRT packaging acceptance,
normal runtime, full retail matching and independent endpoint validation. The
user's three journal edits remain untouched. Next are the now-connected native
Ogg facade and ZIPUP loader/reset/transport bank, alongside blowup/terrain
lifecycle work. These source/ABI hypotheses remain scratch research until
accepted; whole-project transfer and direct-main checkpoints continue.


### Native audio facade and terrain lifecycle checkpoint (2026-10-04)

32 complete new functions add 7,283 unique PC bytes over fb3d5d9: actual native
Ogg facade, clock, stream construction and sample/manager services 20/4,478;
blowup selection, sorting, terrain lifecycle and platform rotation 12/2,805.
Whole-PC fuzzy coverage rises from 19.021145% to 19.193244%. Unique compared
code reaches 964,702; native objdiff compares 965,219 including older aliases and
two new physical copies of the existing 32-byte header callback, at 69.076760%.
Those 64 repeated bytes receive no new unique transfer credit. Thirteen new exact
functions add 1,100 bytes, reaching 176,972/5.098227% of 3,471,246 bytes.

All prior exact code/data/members survive. Of 3,429 old full function copies,
3,428 remain identical; unchanged particle source toggles the independently
reviewed historical 717/713-byte pair again, preserving all 34 relocations and
qualified x87 behavior. Four other identical functions only regenerate numeric
local compiler-label suffixes; their section offsets, types, addends, resolved
addresses and actual written relocation values remain unchanged. No source or
compiler tuning produces these differences.

OggReader is the actual 0x338-byte derived WavReader class, with canonical 720-byte
Vorbis state at 0x48, nine virtual slots and native i64 seek/long tell callbacks.
Its real constructor allocates five 0x114-byte cache records through NuAlloc;
only the global pointer is defined, with no invented static backing. The exact
existing WavReader declaration moves into the shared C++ guard; identical Ogg
class declarations remain local to two actual owners. Original canonical Vorbis
headers remain local, with per-site type/naming lint comments merged when both
apply. All 271 codec functions and 179 initializer regions remain unchanged.
The queue constructor keeps the full 696-byte original extent, including its
compiler-owned eight-DWORD switch table. Actual DSOUND.dll ordinal 11 supplies
DirectSoundCreate8 through a declaration-only import library; no SDK function
implementation is fabricated. Real caps/listener slots and manager/sample fields
carry verified offsets. The source preserves original NULL-cache missing unlock,
required read-count pointer, event policy and other qualified native behavior.

Terrain setup/teardown, final setup, visibility, sorting and selection use the
verified TYPE100/INSTANCE114 and terrain views. Nearest selection preserves
single-precision distance rounding, ordered strict thresholds, first-tie choice
and mask 0x804001. All 52 prior subsystem bodies and 65 prior audio/Wav bodies
remain complete and unchanged. Hardware DirectSound, active constructed queues,
global manager reset/reverb overlap, unsupported-extension dispatcher 65,
destructive effects and general game execution remain unclaimed.

The genuine source-only executable links 3,463 function/5,895 data definitions
across 265 actual objects. LLVM18 validates 292 source/header files and 265 units;
all 700 tests pass with three skips. The existing isolated DAT fixture now extracts
both complete actual clock bodies and the actual tick-base definition to resolve
the expanded thread owner; no clock mock or reduced assertion replaces them.
Strict runtime uses all 265 normal objects with zero replacements/supplements:
audio 184 and terrain/lifecycle 62, all 246 pass. Three legal mono/stereo streams
match an independent installed decoder within one PCM16 LSB; real cache reuse,
file/memory callbacks, seek/reset/EOF, Wav delegation and selected local failure
paths are exercised. These domains do not establish hardware/GPU or malformed
input capacity.

Eight new genuine shared definitions add 53 source-view bytes with unknown original
extents and zero allocation/exact credit. All 3,340 old shared regions and 1,654
scopes survive; 3,348 normal regions equal isolated output across 1,662 scopes.
Known 562/137,693 remains unchanged; unknown 1,100/126,617 describes source views.
All 4,435 prior exact physical initializer copies and two exact members/four bytes
survive. Eight new known exact copies yield 4,443. All 32 new complete initializers
are retail-equal, including 24 unknown-extent copies that receive no coverage.
The 4,467 verified-copy union is not exact credit. Newly exact data union adds 43
bytes. Original private callback/RTTI/literal ownership is retained.

The 07:35 checkpoint waited for normal linkage, fixture dependency and scoped lint
repairs, full retail comparison and independent preservation review. The user's
three journal edits remain untouched. Next are accepted anti-node/type-arena,
portal clipping and renderer subsets alongside the broad ZIP transport/reset bank
and actual matrix inverse/transform family. ZIP rope modulation and ArcSine x87
intermediates were repaired before scratch source acceptance; storage and final
normal runtime remain pending. Candidates receive no source coverage. Whole-project transfer and direct-main checkpoints continue.


### ZIP transport, portal math and arena/message checkpoint (2026-10-04)

57 complete new functions add 15,727 unique PC bytes over 0a0bb75: ZIP transport,
reset, callbacks and gizmo services 35/10,627; rendering 3/411; anti-node pool/type
arena 6/1,366; portal clipping 2/702; matrix/vector math 3/1,597; anti-node update
and camera getters 3/796; Message/endian 5/228. Whole-PC fuzzy coverage rises from
19.193244% to 19.502728%. Unique compared code reaches 980,429; the native objdiff
denominator is 980,946 with existing aliases retained. Twenty-three new exact
functions add 1,305 bytes, reaching 178,277/5.135822% of 3,471,246 .text bytes.

All old exact code/data/member matches survive. Of 3,463 prior complete function
copies, 3,462 remain identical. Unchanged particle source reproduces the already
reviewed 713/717 scheduling pair; all 34 named relocations and qualified x87
behavior survive. No flags, original boundaries or source tuning change the metric.

ZIP preserves the original private ESI Reset/caller group, actual ArcSine x87
intermediates, rope modulation state and native two-million-unit reset height.
The independent native loader/arena/callback/movement/rope fixture passes 142
checks. Its initial six movement failures are retained: test context 0 denotes
Jump; context 1 is the valid LandJump. Correcting that input and asserting the
actual ObjLandReady domain changed only the harness. Low-dot ArcSine geometry is
qualified within two angle units; whole-domain accuracy and decorative cheat
rope rendering remain untested. Real render-stream providers pass 45 checks with
valid initialized materials/display lists; hardware submission and inherited
NuPrimBegin NULL-list fallback remain unclaimed.

Anti-node construction keeps native 18 + count*40 allocation and exact-end
acceptance; unregistration retains native count behavior even for an absent node.
Type/instance code uses actual unsigned WORD capacities and initializes seven
particle WORDs. Update retains cached-next traversal, real timer/expiry comparisons
and verified AI field offsets. The genuine Message C++ owner defines its private
512-record POD pool, stride 0x4b4, with a complete 616,448-byte zero initializer;
its original owning extent remains unknown. Shared NETMESSAGE_PC fields and
thiscall/RET4 constructor ABI are asserted. Endian/write services preserve native
NULL-data behavior, without added guards or fabricated networking providers.
Arena, update and stream fixtures pass 150 checks across all current normal owners.

Portal clipping stages eight f32 dot products before ordered comparisons and uses
native signed WORD plane counts. Matrix inverse retains full scaled-pivot logic;
three-vector homogeneous transform stages the reciprocal, while four-vector
transform performs the PC operation without W division. Twenty arithmetic and
nine clipping checks pass. Temporary shorter path copies overcome Windows command
length limits; every linked normal object is hash-equal before/after, with no
provider or content substitution. Private frustum construction/caller recursion
is still scratch research and receives no source credit.

The source-only EXE links 3,520 function/5,961 data definitions across 267 actual
objects. LLVM18 checks 294 source/header files and 267 units; all 700 tests pass
with three skips. All 366 selected native checks use the full 267 normal object
set, with zero candidate replacements or supplemental/fake providers. Complete
retail matching and independent endpoint/preservation review pass. Selected valid
CPU domains do not establish GPU, hardware audio, networking, arbitrary malformed
capacity or complete game execution. Prior exact-SHA CI for 0a0bb75 passed.

26 shared definitions add 632 source-view bytes: three verified NUL owners/32
known bytes and 23 unknown views/600 bytes, including the real 512-byte grid.
All 3,348 old regions/1,662 scopes survive; 3,374 normal regions equal isolated
output across 1,688 scopes. Known 565/137,725 and unknown 1,123/127,217 describe the
verified shared scope. All 4,443 prior exact physical copies and two exact member
fields/four bytes remain unchanged; 38 new known exact copies yield 4,481. All 66
new complete native initializer copies are retail-equal; 28 unknown views receive
zero original-extent/allocation credit. The 4,509 verified-copy union is not exact
coverage. Exact data union rises by 59 bytes to 630,991. Private ZIP/default tables
and Message pool retain their real source ownership.

The 08:30 checkpoint waits for final complete initializer/endpoint acceptance,
documentation and fresh report fingerprints before commit. The user's three
journal edits remain untouched. Next are four accepted utilities/terrain getters
(255 exact bytes), pending two real scalar views, alongside larger Bolt provider,
private portal/frustum caller and camera-action banks. Those scratch candidates
receive no source credit. Broad whole-project transfers and direct-main pushes
continue until the user stops or pauses them.


### Bolt, camera, portal and renderer checkpoint (2026-10-04)

59 complete functions add 27,668 unique compared PC bytes over 83b4d3a:
utilities/terrain/Parts 7/839, renderer 4/1,617, camera 8/7,850, portal
21/7,206 and Bolt runtime/collision 19/10,156. Whole-PC fuzzy coverage rises
from 19.502728% to 20.086058%; unique compared code reaches 1,008,097 bytes.
The native objdiff denominator is 1,008,614 with prior aliases retained.
Thirteen new exact functions add 897 bytes, reaching 179,174/3,471,246
.text bytes (5.161662%). All prior exact code/data/member matches survive.
Of 3,520 prior function copies, 3,519 remain identical; unchanged particle
source reproduces its independently reviewed full 717/713 scheduling pair,
with the same 34 named relocations and qualified x87 behavior. Compiler flags,
original extents and matching rules remain pinned.

FindAnglesXY reuses the full existing 65,538-byte ConfigBuffer physical owner:
its native Y WORD at 0x93b0e8 becomes an asserted typed member at offset 0x10000,
without a second global, guessed owning extent or data credit. Existing parser
consumers use the same byte view. Terrain getters retain native unsigned material
BYTE and asserted state offsets. Parts dispatch has a typed callback and real
collision/SFX/rumble providers; inherited KillPart behavior remains partial.

The renderer's 28-byte vertex has colour at 0x10, and sphere radius squared uses
native f64 precision. Camera commands retain the native 32-command queue and
index 32 metadata alias. MiniCut3's complete original extent is 1,496 bytes; the
old scratch 1,532 span included alignment and another disconnected function and
never entered production. Ordinary shared declaration context changes its rebuilt
body from 1,789 to 1,709 bytes; independent full-body/ABI review accepts the actual
emission without tuning or compiler controls. Portal helpers remain together in
the original C translation unit with native private register conventions.
Intersections retain ordered comparisons, first-operand ties/NaNs, signed plane
counts, allocator alignment and genuine overflow diagnostics. Debug rectangle
vertices share the corrected renderer layout.

The genuine C++ Bolt helper/caller owner connects Init/Add/Deflect, message sender
and receiver, points/orientation/raycast and collision services. Verified PC
behavior uses model_flags at CHARACTERDATA+4, reloads bolt.owner after callbacks,
retains deactivation feedback rumble and the native jitter/float spill constants.
C++ message ABI and original private helper ownership remain intact; no provider
adapters or fabricated definitions are added. Selected ray-disabled, CPU and
valid-input fixtures do not establish full terrain/shadow, hardware audio/GPU,
networked gameplay or arbitrary original backing capacities.

The source-only EXE links 3,579 functions/6,083 data definitions across 269 real
objects. LLVM18 checks 296 source/header files and 269 units; 700 tests pass with
three skips. All 207 selected native checks pass with all 269
normal objects, zero replacements/supplements and 309 unchanged production pins.
Authoritative full retail comparison and independent endpoint acceptance pass.
Previous exact-SHA CI 83b4d3a passed (run 37189555505).

81 real shared definitions add 2,740 source-view bytes: 38 known NUL owners/436B
and 43 unknown working views/2,304B. All 3,374 old shared regions and 1,688 scopes
survive; normal 3,455 regions equal isolated output across 1,769 scopes. Shared
known scope 603/138,161B and unknown 1,166/129,521B describe initializer proofs,
not inferred allocation sizes. All 4,481 old exact physical initializer copies
and 2 exact typed members/4B survive; all 3 old active members remain unchanged.
The new Y member is a verified 2B unknown-extent view with zero exact credit.
79 new known exact copies yield
4560; all 122 new complete initializer
copies are retail-equal, including 43 unknown views with zero owning-extent credit.
Exact data union rises 452B to 631,443B. The verified-copy union is not coverage.

The preexisting sceneParameters[32] working view overlaps canonical zx, texture
count, shadow state and fstack_limit. Its full initializer is verified, but native
owning capacity is unresolved. The proposed larger union migration remains held;
new portal code reuses the existing canonical state without fresh aliases. Selected
fixtures exclude scene-stack alias writes and do not claim whole-game correctness.

The 09:08 checkpoint was delayed until real PC repairs, connected data, complete
native/retail validation and endpoint acceptance were safe. The user's three
journal edits remain untouched. Next are the full 2,738-byte Bolts_Update parent
alongside MiniCam consumers, scene visibility/instance-tree callers,
ManageGameObjects with three missing callees and closed world configuration
producers. Larger resource/world lifecycle parents have actual callback/config/
loader gaps; their seven-parent 18,699-byte hypothesis bank remains held.
Candidate inventory receives no transfer credit. Broad whole-project work and
validated direct-main pushes continue until the user stops or pauses them.


### Gameplay lifecycle, MiniCam and visibility consumers checkpoint (2026-10-04)

20 complete functions add 7,817 unique compared PC bytes over 7b31086:
MiniCam consumers 6/3,787, Pulses and Gizmo visibility 4/2,532, genuine
object-management suppliers 6/735, configuration siblings 2/679 and terrain
queries 2/84. Unique compared code reaches 1,015,914 bytes; native objdiff
includes the retained aliases and compares 1,016,431. Whole-PC fuzzy coverage
rises from 20.086058% to 20.283675%. Six newly exact functions, including the
already-compared reflection, add 414 bytes: 179,588/3,471,246 (5.173589%).
Exact initializer union rises 277 bytes to 631,720. Original denominators,
extent rules and compiler settings remain unchanged.

Existing ManageGameObjects (2,793 original bytes) now calls genuine crate spawn,
AI group destruction and torpedo cleanup providers, retains native low WORD
tests, and reloads world AI state after callbacks. KillGameObject's existing
3,750-byte parent replaces its missing torpedo cleanup action with the real
provider and retains the following native NULL store. The 2,738-byte Bolt update
and 79-byte reflection move into the genuine original C++ helper/caller group;
the old partial C copies are removed. Full update retains native callback state
reloads, numeric comparison behavior and effect/count access widths. These
previously compared parent spans receive no new transfer credit. Inherited
object destruction, default debris and positive terrain/shadow collision
services remain explicit partials outside the selected tested domains.

MiniCam's six consumers preserve the complete dispatch-table extents, 32-command
queue and metadata alias, native angular wrapping, easing and float spills.
Pulses reconstruct parser-driven producers, reset and timer/halfspace updates;
the shared world pulse pointer is at 0x5050, not Android's 0x5054. PC NaN/ordered
comparisons and empty-word outer-parser termination were corrected before
activation. Gizmo visibility uses native BYTE IDs and signed registry counts.
EquivalentObjects and CharPlatforms configuration each have the actual two
cdecl arguments demonstrated by the WorldLoad caller. Terrain query getters
reuse existing typed state and the existing complete TerrPolyInfo owner.

The source-only EXE links 3,599 functions and 6,122 data definitions across 269
genuine objects. LLVM18 checks 296 source/header files and 269 units; 700 tests
pass with three skips. All 171 selected CPU checks pass with all actual normal
objects, zero replacements/supplements and 309 unchanged production pins:
MiniCam 41, Pulses/configuration 33, object lifecycle 68 and Bolt update 29.
Full retail comparison and independent final endpoint review pass. Prior exact
revision 7b31086 CI passed (run 37193486958). Asset loading, GPU/hardware audio,
expired stack pointers and arbitrary original storage capacities remain untested.

Of 3,579 previous function copies, 3,573 remain identical. Four intentional
existing-parent supersessions are independently accepted. Unchanged camera
source returns to its previously accepted complete 1,789-byte MiniCut3 emission
from 1,709 bytes. The unchanged particle source changes from 713 to 717 bytes;
the current 717-byte body differs from its historical counterpart only in five
bytes selecting three FLD stack operands and two FXCH indices. Independent
symbolic x87 review verifies the same four-value stack at the join, with no
arithmetic or branch change and all 34 named relocations unchanged. Full original
extents and ABI remain pinned. Neither body is claimed byte-preserved against
the immediate baseline. No exact function match is lost; compiler differences
are recorded rather than hidden or tuned away.

Shared data source is unchanged: all 3,455 complete regions and 1,769 scoped
initializer proofs agree with isolated output; all four active typed members
remain unchanged, including two exact members/four bytes. The actual previous
per-object exact initializer count is 4,561, correcting the earlier 4,560
row-level count: the exact TerrainUnusedSpatialBlockPc copy in data.cpp was
omitted because its aggregate row has another nonexact owner. 4,557 old copies
remain unchanged. Four duplicate compiler constants disappear from bolts.c
during the reviewed C-to-C++ parent migration; all four retain their complete
retail-equal bits in the genuine C++ owner and other real owners. Forty-three
new known complete initializer copies/398 emitted copy bytes yield 4,600 actual
exact copies. Duplicate copy counts are not unique data coverage. No shared
allocation or unknown-extent credit is added.

The 10:24 checkpoint was delayed for complete final validation; scene/tree
scratch work did not block or enter this release. The user's three journal
edits remain untouched. Next are twelve scene visibility/tree functions
(3,050 new PC bytes), five gameplay utility suppliers (324 bytes), two ready
configuration siblings (1,124 bytes), and broader configuration/collision
provider families. Scene width/output will reuse named fields in the unchanged
existing owner. Native tree clipping reads an unwritten fourth local stack lane;
that original defect is preserved, with positive-path residue equivalence
unclaimed and selected execution limited to defined paths. GameAI and larger
world loader parents remain held until their real services are connected.
Whole-project work and validated direct-main pushes continue.


### Scene visibility, configuration and gameplay providers checkpoint (2026-10-04)

58 complete functions add 10,935 unique compared PC bytes over 07dc534:
scene visibility/tree 12/3,050, configuration siblings 2/1,124, gameplay utilities
5/324, configuration callbacks 30/3,004, display callers 3/1,296 and Parts runtime
6/2,137. Unique compared code reaches 1,026,849; native objdiff includes retained
aliases and compares 1,027,366. Whole-PC fuzzy coverage rises from 20.283675% to
20.521280%. Thirty-three new exact functions add 2,760 bytes, reaching
182,348/3,471,246 .text bytes (5.253099%). Exact data union adds 535 bytes to
632,255. Candidate inventories, duplicate copies and unknown extents receive
no additional coverage.

Visibility preserves genuine original C private helper/caller groups and named
fields with assertions. Width at 0x966290 and occlusion output at 0x9663b0 reuse
record16 of the unchanged existing sceneParameters[32] working owner: 17,152
bytes, stride0x218. Anonymous unions retain the previous typed fields and opaque
views; no extra backing or original capacity is inferred. Macros use the actual
LLVM18 uppercase naming policy without waivers. The native tree function reads
an unwritten fourth local stack lane after the real transform writes three
floats. Source preserves that defect; the 39 selected runtime assertions use
near/far rejection before that read. Positive clipping and stack-residue equality
remain unclaimed. Display retains signed BYTE visibility, full LOD/special
selection and genuine original private register conventions; active GPU/asset
rendering is not established by this CPU/source validation.

Configuration siblings retain actual two-argument cdecl WorldLoad interfaces.
Grabber, ForceSFX and Doors preserve complete original callback/keyword groups
and field widths. PC comparison caught an invented door cutscene reset: native
DoorConfigure preserves the existing pointer unless the optional cutscene
callback writes it. Removing that reset passes all 26 selected assertions;
the retained incorrect version fails one. Private ForceSFX lookup remains in its
original TU with the native EAX/stack interface. Active SFX model parsing is
outside the selected runtime domain. Utilities share the existing ten-record
Suit owner and canonical category type; all 374 full utility-owner bodies and
prior initialized data survive. Larger GameAI is still held for genuine providers.

Parts adds time slip, type cleanup, scaled effects, the real editor scaler,
unsigned file reader and bounded SFX lookup. Type stride0x178, canonical128-record
working view and scale/float/NaN comparisons are verified. Private cleanup state
stays in its original Parts owner. The unsigned reader has its actual distinct
0x621820 identity. Existing genuine terrain diagnostic is reused. Runtime uses
valid initialized types0..127; inclusive native128 guard behavior is preserved
without claiming an out-of-array runtime domain. A fixture restores real media
state after EdFileClose before a second open; no production guard/provider or
assertion is removed. All27 checks pass; the retained changed unordered-cleanup
version fails one. Full terrain/raycast, hardware sound and general gameplay
remain untested.

The genuine source-only EXE links 3,657 functions and 6,231 data definitions
across 272 actual objects, including new nuvisibility.c, gizportal.c and edpart.c
owners. LLVM18 validates 299 source/header files and 272 units; 700 tests pass
with three skips. All143 selected native assertions pass against every normal
object, with zero replacements/supplements or fabricated providers: visibility39,
configuration26, utilities51 and Parts27. All312 frozen production pins remain
unchanged. Full authoritative retail matching and independent endpoint review
pass. Prior exact revision07dc534 CI passed (run37196551050).

All prior exact code/data/member matches survive. Of3,599 previous function
copies,3,597 are identical. Unchanged camera and particle source return to their
independently accepted historical1709/713-byte compiler bodies from1789/717.
Full raw bytes and named relocations match retained accepted copies; the prior
717-byte particle's five-byte x87 permutation retains its separate semantic
proof. Immediate byte equality is not claimed. Compiler flags, native extents,
original denominators and matching rules remain pinned.

28 shared definitions add547 source-view bytes with unknown owning extents and
zero allocation/exact credit. All3,455 old complete regions and1,769 scopes
survive; actual normal3,483 regions/1,797 scopes equal isolated output. Shared
known scope603/138,161B is unchanged; unknown1,194/130,068B describes verified
working views. The inherited CRC16 dynamic initializer remains unchanged; no
new constructor is introduced. All4,600 prior exact physical initializer copies
and all four typed members survive, including two exact members/four bytes.
All109 new complete initializer copies are retail-equal:69 known copies/676
emitted copy bytes yield4,669 actual exact physical copies;40 unknown copies/859
working bytes receive zero owning-extent/allocation/exact credit. Unknown copies
include28 shared/547B and12 original-TU or unknown literal views/312B. Counts of
physical copies are separate from unique exact data coverage.

The11:18 UTC checkpoint waited for the final retail/initializer/independent
endpoint gates before safe commit. The user's three journal edits remain
untouched. Next are broader Parts5/6,419 and genuine matrix/reflection/RTL
providers, interaction7/6,338, Grid/math6,816 and Teleports/helpers16/1,853.
Those are candidates until the next full gates. Image's native cache at0x966298
shares record16+8 and remains held for canonical alias/capacity evidence.
WorldInfo_Load has no production definition; its uncompiled candidate retains
31 unique missing direct targets,21 unmapped. No parent/source credit is claimed.
Whole-project transfer and validated direct-main checkpoints continue.


### Particles, interactions, Grid math and Teleports checkpoint (2026-10-04)

38 complete functions add 23,107 unique compared PC bytes over 5e49a64:
Teleports/name/editor helpers 16/1,853, creature interactions 7/6,338,
Parts/matrix/reflection/RTL 11/8,100 and Grid/math 4/6,816. Unique compared
code reaches 1,049,956; native objdiff includes retained aliases and compares
1,050,473. Whole-PC fuzzy coverage rises from 20.521280% to 20.920709%.
Nineteen newly exact functions add 1,918 bytes, reaching 184,266/3,471,246
.text bytes (5.308353%). Exact initializer union adds 129 bytes to 632,384.
All 3,657 prior full function copies remain identical. All previous exact
code/data/member matches survive; no compiler-variant exception is needed.

Interactions retain the original C helper/caller groups, ordinary four-argument
callback table and private orbit convention. PC source uses unsigned WORD
priorities, nullable fixed flags, collision links and actual typed timer/geometry
fields. Native Circle spills summed squared distance to f32 before comparison;
restoring that stage passes 40 checks, while removing it fails two boundary
assertions. The apparent default diagnostic call is unreachable: surviving
priority selection always fixes exactly one object, equal priorities skip the
pair, and the diagnostic requires neither fixed flag. No substitute diagnostic
body is added. The three-pointer dispatch table remains in the actual aisys TU;
its full 12 working bytes are retail-equal, but its owning extent remains blank
and receives zero allocation/exact credit.

Parts reconstructs scene lookup, single-type/page loading, nearest selection
and emitter update, plus genuine matrix orientation, reflection and RTL setters.
The full emitter extent is 2,728 bytes, including two alignment bytes and the
20-byte embedded switch table; an earlier 2,706-byte body-only hypothesis is
retained as rejected. Source preserves native extended period addition, explicit
f32 pause-duration spill and exact integer rotation-base conversion. Positive
26 assertions pass; the original three-stage error version fails exactly three.
All 205 prior supplier-owner function copies remain unchanged. Selected inputs
cover initialized type/emitter state, equality crossing, >2^24 rotation bases,
valid switch127, real model-less inherited partial AddPart and finite matrix/
reflection/RTL paths. Asset/parser and terrain/raycast completion remain unclaimed.

Grid uses the actual negative half-FOV denominator and stores brightness after
intensity multiplication. Its minimal bloom reader retains every accessed offset;
only that identical local type moves into the shared header. Log475 and Pow57
are complete-byte exact; ASin320 retains native x87 products and its genuine
conversion tail. All 45 prior render/math code/data regions survive. Finite
scalar runtime passes 31 checks with normalized log indices0..8999. The exact
native Log's equality10000 case reads beyond its9000-float table; source preserves
that original defect and failed analytical fixtures remain recorded. Grid output
and GPU execution are untested. Viewport, Image and ClearScreen stay outside this
release; no overlapping viewport/cache backing is introduced.

Teleports keeps all ten real callbacks, the native prefix/keyword cohort and
actual editor-first spline lookup. Full unique-name extent354 includes its cold
return. Name generation preserves alias-sensitive truncation and native byte/
WORD behavior. Canonical NuStrNCpy copies n-1 characters: correcting the fixture's
expectation changed no source or assertions. Parser/name/spline tests pass37;
valid initialcount0, adequate caller arena, quoted-empty input, duplicate names,
flags, matrices and cursor alignment are exercised. The three private state/
cohort extents stay unknown. No full WorldInfo_Load execution is claimed.

The source-only EXE links 3,695 functions and 6,295 data definitions across273
actual objects, including the new genuine gizteleports.c owner. LLVM18 checks
300 source/header files and273 units;700 tests pass with three skips. All134
native assertions pass against every current normal object, with zero candidate
replacements/supplements or fabricated providers: Teleports37, interactions40,
scalar math31 and Parts26. All313 frozen inputs remain unchanged. Authoritative
retail comparison and independent complete source/data endpoint review pass.
Previous exact revision5e49a64 CI passed (run37199512352).

Ten shared definitions add58 working bytes with unknown original extents and
zero allocation/exact credit. All3,483 old complete regions and1,797 scopes
survive; actual normal3,493 regions/1,807 scopes equal isolated output. Known
shared scope603/138,161B is unchanged; unknown1,204/130,126B describes verified
working views. The inherited CRC16 initializer and all four typed members stay
unchanged, including two exact members/four bytes. All4,669 previous exact
physical initializer copies survive. All64 new complete copies are retail-equal:
43 known copies/300 emitted bytes yield4,712 exact physical copies;21 unknown
copies/206 working bytes receive no owning-extent/allocation/exact credit.
Unknown copies include the genuine dispatch12 and private Teleports/Grid/Log
state. Copy counts are distinct from the unique data union.

The12:10 UTC checkpoint was delayed when aggregate coverage found the missing
map row for the already verified real dispatch initializer. Only that binding
was added, with blank owning extent. The failed log and prior freeze were kept;
all unchanged required checks and final endpoint were completed before commit.
The user's three journal edits remain untouched. Next are accepted turret2/653
and terrain unit-ray3/411, LOS3/1,742, then real CastPart1,315/UpdateParts2,755
and Draw700 providers. NewRayCast has one exclusive genuine terrain owner;
LOS reuses it rather than producing a duplicate. Image's eight-byte canonical
cache alias and a48-byte viewport prefix plus existing16-byte projection owner
are factual scratch proposals only, with no invented original capacity. Broader
world/resource suppliers and full GameAI remain under reconstruction. Whole-
project transfer and validated direct-main checkpoints continue.


### Ray, particle, rendering and opponent checkpoint (2026-10-04)

24 complete functions add 14,695 unique compared PC bytes over ab761af:
turrets 2/653, terrain unit ray 3/411, LOS 3/1,742, particle cast 2/1,363,
particle update/draw/effects 7/3,572, opponent selection 2/3,988 and Image/
Clear/viewport 5/2,966. Unique compared code reaches 1,064,651 bytes; native
objdiff includes retained aliases and compares 1,065,168. Whole-PC fuzzy
coverage rises from 20.920709% to 21.238860%. Eight new exact functions add
336 bytes, reaching 184,602/3,471,246 .text bytes (5.318033%). Matched data
union remains 632,384 bytes. Full source dependencies and original helper/
caller groups remain linked; the new genuine turret owner is gizturret.c.

The unit-radius NewRayCast is the actual 389-byte supplier with inverse
collision radius zero; it is distinct from the existing ScaleYMask routine.
LOS and PartRayCast reuse this one original terrain owner. The true 48-byte
PartRayCast wrapper is exact. LOS preserves the native left-shift producer,
64 hostility/visibility mask slots, directional/cached checks and the actual
seven-word public caller. Its private ray helper naturally uses registers
plus three float stack arguments, while retail uses six stack arguments;
this remains an honest nonexact original-TU implementation.

Turret selection preserves WORD flags/count, signed BYTE cursor, native
record stride and real rider offset. Priority modes select the first valid
candidate even beyond the initial nearest-distance threshold; the rejected
old branch fails seven of 24 checks. Opponent selection retains the actual
nine logical/ten physical arguments, awareness masks, budget/history and
reciprocal assignment rules. Horizontal alert deltas stay extended until
the native square/add operations; hysteresis and dot values spill at their
native stages. Private helper ABI differences remain explicitly qualified.

UpdateParts, DrawParts and their genuine private matrix/directional helpers
retain the full native extents and signed/WORD/f32 accesses. Named matrix
copies preserve x87 signaling-NaN quieting; the old raw-copy candidate fails
one of 31 checks. CastPart uses actual pinned static CRT finite/isnan
providers, with zero new engine source credit for their inventory aliases.
The impact normal and enabled flag remain private in the original parts TU.
Current AddPart/KillPart still have documented omitted actions; their later
real RTL/effect completion is a separate scratch wave, not hidden credit.

Image keeps native unrounded directional blend and Pow-return lifetimes,
then stores brightness at the actual f32 boundary. Both Image and Clear
restore the previous coordinate stack slot before decrement. The selected
eight-byte cache union retains both existing scene DWORD names, every old
offset and the full 17,152-byte owner. Prefix48 plus existing projection16
avoids duplicate viewport backing. A producer-backed fixture passes 194
checks; precision-only old source fails one, full v1 fails ten. The first
fixture's incorrect stream record/count layout and its 118 failures remain
recorded; only those two fixture layout facts were corrected, with all 194
expectations retained. Stream input is finite, rows positive and cols2..8;
GPU output and general cache capacity remain unclaimed.

The source EXE links 3,719 functions and 6,332 data definitions across 274
actual objects. LLVM18 checks 301 source/header files and 274 units; all
700 tests pass with three skips. All 357 strict native assertions pass with
every current normal object and zero replacements/supplements: Turret24,
LOS42, Opponent38, Image194, Cast28 and Update31. All 314 frozen inputs and
274 object hashes remain unchanged. Authoritative retail matching and the
independent complete source/data endpoint pass. Previous ab761af CI passed
on its exact revision (run37202599482).

Of 3,695 prior full function copies, 3,693 remain byte-identical. The two
nonexact compiler variants are independently proved: DebReAlloc2 swaps
commuting stack-to-register loads without altering x87 or named relocations;
NuRndrParticleGroup changes from 713 to 715 bytes by swapping two initial
FLDs and adding FXCH, yielding the identical dispatch x87 stack and all 186
remaining instructions/CFG. This new 715-byte variant is separately reviewed,
not treated as an automatic recurrence of older 713/717-byte versions.
Finite valid-memory behavior is verified; unmasked trapping order is not
claimed. Every prior exact function/data/member match survives.

All 3,493 old shared initializer regions and 1,807 scopes survive. Actual
normal 3,500 regions/1,814 scopes equal isolated output; all 4,712 prior
exact physical copies and four members remain unchanged. All 37 new full
initializer copies equal retail: 21 known copies yield 4,733 exact physical
copies; 16 unknown copies/118 working bytes receive zero owning-extent,
allocation or exact credit. Seven shared views add 74 working bytes. Private
Norm12, enabled4 and seven Image locals stay with their original owners.
Known scope603/138,161 bytes is unchanged; unknown scope1,211/130,200 bytes
describes verified working views. The CRC initializer remains unchanged.

The 13:04 UTC checkpoint waits for complete mandatory whole-header lint,
repository tests, retail comparison and both finite prior-variant proofs.
The user's three journal edits remain untouched. The next connected wave
includes source-accepted resource page7/4,698, compiled Trigger6/2,794,
RTL3/195 and true GameAI6,245/private790, plus texture/material animation.
Candidate bytes remain separate from production credit. World/resource,
rendering, gameplay and PC services continue under the whole-project mandate.


### Resources, gameplay and texture animation checkpoint (2026-10-04)

72 complete functions add 21,628 unique compared PC bytes over5020983:
resource pages7/4,698, RTL3/195, gameplay AI10/10,621 and texture/material
animation52/6,114. Unique compared code reaches1,086,279 bytes; native
objdiff compares1,086,796 including existing aliases. Whole-PC fuzzy
coverage rises21.238860% to21.632154%;38 exact functions add2,445 bytes,
reaching187,047/3,471,246 .text bytes (5.388469%). Data union grows148 bytes
to632,532. KillPart175/AddPart1,884 regain actual omitted effects/RTL actions
but were already compared, so add zero new transfer bytes.

Resource loaders preserve serialized offsets, private ESI reader context,
real world buffer-end pointer contract, native extended emission timing and
its f32 ratio boundary. PC behavior excludes the Android STARDESTROYER
exception. Actual named fields have width/offset assertions; two genuine
cross-unit loader contexts are parts_load.c and edpp_load.c. Selected versions
and counts/adequate arenas are qualified; full WorldLoad remains incomplete.
RTL uses full signed template-ID comparison, native bounds/free ordering,
real effect providers, callback pointer reload and post-callback -1 store.

GameAI retains native startup/context branches, context17 skip policy,
extended mover/radius calculations and genuine private wall helper. Trigger
includes its cold cleanup and route>=64 native zero-shift outcome. Original
previous-attacker history requires four initialized slots; both-ray-miss
wall direction has native uninitialized behavior. Neither domain is concealed
with fabricated initializers. Genuine Trigger and Lever source owners remain
separate; private helpers stay with their real original caller groups.

Texture animation includes the genuine21-command parser/private table,
script callbacks, comparison/ENV helpers and real RNG/material/shader fields.
External cube loading uses real file/SDK/free providers and source filename;
no DDS asset bytes enter source. The false5e4840 NuMtlInitEx mapping remains
rejected; true5f1dd0 and embedded-asset shader providers are held separately.
All full native extents, cold tails, private register ABIs and canonical
source data references were reviewed without clipping or synthetic providers.

The source-only EXE links3,791 function and6,422 data definitions across278
actual normal objects. LLVM18 format305/tidy278 and static9,714 mappings pass.
All710 tests pass with three existing skips. The first full check reproduced
an existing128MiB lock-test failure after thread tests retained virtual
address arenas. Its exercise now runs in a fresh process with unchanged128MiB
cap, real contending process, sentinel, reacquisition and restoration assertions.
Production limits remain unchanged. Passing lint/static stages plus independently
reviewed repair and full suite establish all required check components; the
initial failed make check is not represented as exit0.

All340 strict native checks use all278 actual normal providers with zero
replacements/supplements: Resource120+13, RTL37, Trigger33, GameAI16 and
Graphics121. All320 frozen production inputs and278 object hashes survive.
Of3,719 prior full copies,3,714 are identical. Four qualified Parts changes
complete Kill/Add and natural Update/Draw inlining. Particle715->717 is separately
proved for finite valid memory/x87 state: reordered loads plus FXCH yield the
same dispatch stack and186 remaining instructions/CFG/named relocations.
Unmasked trapping order is unclaimed. Every old exact code/data/member survives.

Actual shared-data3,535 regions/1,849 scopes equal isolated proof. All4,733
old exact physical copies and four members survive;42 new known copies give
4,775. All90 new complete initializer copies equal retail;48 unknown copies/
7,380 working bytes add zero owning-extent/allocation/exact credit.35 shared
views add192 working bytes: one existing-known Grapple4 view plus34 unknown/
188. Private command/table/label storage stays in original contexts. Two exact
WORD members remain4 bytes. Known scope604/138,165 and unknown1,245/130,388
working bytes are distinct from original allocation claims. CRC initializer
and complete member parents remain unchanged.

The14:04 UTC checkpoint was delayed for full validation and the reproduced
existing test isolation repair rather than committing broken checks.5020983
CI37206299111 passed on its exact revision. The user's three journal edits
remain untouched. Next are RTL/list12/3,268 and GizFlow39/4,515 source candidates,
then real UpdateGameObjects collision/lighting/world suppliers and scene/resource
services. Scratch candidate bytes do not count as transfer. Whole-project
engine, gameplay, resources and platform work continues.


### Lighting, lists and GizFlow ingestion checkpoint (2026-10-04)

51 complete functions add 7,783 unique compared PC bytes over 04cc852:
RTL/list12/3,268 and GizFlow ingestion39/4,515. Unique compared code reaches
1,094,062 bytes; native objdiff compares 1,094,579 including existing aliases.
Whole-PC fuzzy coverage rises from 21.632154% to 21.809572%; 27 exact
functions add 872 bytes, reaching 187,919/3,471,246 .text bytes (5.413589%).
Data union grows 12 bytes to 632,544. Already compared parents and inventory
additions receive no new transfer credit.

RTL retains the original private EAX chain/single and ESI process helpers,
complete nine-DWORD dispatch table, timers, blends, modifiers and full load
switch tail. NuLstCreateBuff uses real caller-owned arena/list fields,
positive count, native alignment, strict remaining-capacity comparison and
WORD truncation. Valid serialized versions0..5 preserve descending fog
migration and wrapping UID producer. IndexLights originally used an unsigned
incremented BYTE index; independent review found native MOVSX. V3 now sign-
extends the count before the store. The retained count128 discriminator passes
4/4; old unsigned source fails1/4. Native missing capacity/count127 guards
remain explicit, with finite initialized sentinel/index/modifier storage.

GizFlow preserves two-pass parsing, collapse/remap expansion, genuine private
callback/condition tables and current registry count reloads. True constructors
5b0550/134, 5b01b0/148 and5ae1c0/344 retain canonical GIZMO_PC* ABI; native
Special refcount increment still occurs when its constructor returns NULL.
Native diagnostics59d760/1 and59d770/1 retain genuine cross-unit calls; the
original physical TU name is unknown. NuFreeFPar is the existing real close
provider. Private constructor reader types stay separately scoped; redundant
research declarations were removed without altering all160 accepted regions.
No WorldInfo_Load parent bytes or general constructor/callback runtime credit
is claimed. Parser capacities, valid noncyclic remaps and adequate arenas are
explicit native domains rather than invented guards.

The source-only EXE links 3,842 functions and 6,497 data definitions across
282 actual normal objects. LLVM18 format309/tidy282, static9,831 mappings and
all710 tests pass with three existing skips (108.210s suite). Required build,
object checks, make check and authoritative retail match all return exit0.
All119 native assertions pass using all282 actual normal objects with zero
replacements/supplements: RTL99 and GizFlow20. All326 frozen production inputs
and282 source/object pins survive. All3,791 previous full function copies are
byte-identical; every old exact code/data/member survives. No prior compiler
variant needs a new qualification in this wave.

Three genuine shared working views add12 bytes: RtlFileVersionPc=5 at80fde8,
giztimer_gizmotype_id=-1 at8061b8 and gizrandom_gizmotype_id=-1 at806144.
All owning extents remain blank with zero allocation/exact credit. Normal
3,538 shared regions/1,852 scopes equal isolated output, preserving all3,535
prior regions, four complete typed members/parents and CRC10B plus relocations.
All75 new complete initializer copies equal retail: ten known copies increase
exact physical copies4,775->4,785;65 unknown views/1,045 working bytes remain
uncredited. Private67 GizFlow and five RTL compiler copies stay in original
owners. Known scope604/138,165 bytes is unchanged; unknown1,248/130,400 describes
working views, not original capacities. Exact WORD members remain4 bytes.

Previous04cc852 CI37210226070 passed on its exact pushed revision. The user's
three journal edits remain untouched. Next are source-accepted gameplay3/1,130
with selected CPU40/0, real API/combat collision6,992, camera/frustum and legacy
animation families. Camera6/4,470 and animation3/1,068 are compiled scratch
candidates under review/expansion, not production transfer. Whole-project
engine, gameplay, resources, platform and runtime work continues.

The15:11 UTC checkpoint waited for the complete independent raw-body/data
audit. Eight relocation sites use six renamed compiler-local ordinal labels;
each retains the identical same-function section/value, raw bytes and full
official relocated bytes. External semantic names remain strict. This is
metadata only, with zero changed prior function bodies. Final endpoint:
build/analysis/transfer-execution-20261003/platform/lighting-gizflow-final-review/
endpoint-acceptance.json.
