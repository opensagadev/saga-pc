# Execution plan: broad PC source transfer

Based on [the strategy review](strategy.md), starting at `ba24830`.
Execution authorized on 2026-09-30. All subagents use GPT-6.1 Sol; no Luna agents.

## Objective and baseline

Grow whole-PC fuzzy coverage by bringing substantially more **correctly identified, structurally reconstructed PC code** into comparison. Instruction matching is secondary, except where a difference exposes incorrect behavior, layout, ABI or constants.

Baseline: 178,198 unique original bytes compared; 2.8183615% whole-PC fuzzy; 31,483 exact bytes; 1.7350607% image mapping. Keep `.text` at 3,471,246 bytes and image bounds unchanged. Candidate spans and map-only additions never count as transferred code.

## Work order

| Stage | Concrete work | Completion condition |
| --- | --- | --- |
| 0. Prepare the work queue | Preserve current report, frozen Android graph fingerprints and retained research. Build a per-family ledger of candidate, accepted, contract-ready, partial, compared and exact bytes. Correct misleading private-ABI documentation and align the repository skill with original-TU ownership and this plan. | Every initial task has exclusive files, PC evidence, dependencies and a measurable byte scope. |
| 1. Correct the established baseline | Move StartupParseArguments into its verified caller closure as a static C helper; remove obsolete public declaration/separate definition. Integrate the verified security-cookie/compiler-helper mappings and enable `/GS`, using the pinned CRT's real implementations. | Helper remains exact over 59 bytes; two `/GS` candidates reproduce; source EXE links; full checks pass; all previous exact matches preserved or any change explained. |
| 2. Review the first broad wave | Movement, GameObject and AI: 81 stronger hypotheses, 66 candidate spans totaling 89,407 bytes, 15 unresolved boundaries. Verify identities, caller ABI, complete extents, private helper groups and the fields each body actually uses. | Reviewed subsets continuously become accepted, contract-ready source tasks; uncertain candidates remain outside CSV. |
| 3. Transfer confirmed subsets in parallel | Reconstruct coherent function groups in their subsystem/original-TU owners from Android logic and PC evidence. Solve shared fields/data once. Compile each owned unit early and compare complete extents. | A coherent batch adds new unique compared PC bytes, with real relocations and honest partial status. |
| 4. Run a contrasting resource/config wave | Combine accepted portable backlog with adjacent parser, DAT, world/area configuration and gizmo candidates. Aim for a 50–100 KB review queue where credible leads exist; do not pad the wave with weak guesses. | Measure acceptance, implementation and weighted-byte yield against the first wave; choose subsequent families from observed results. |
| 5. Expand discovery continuously | Add qualified C++ signature seeds; strengthen existing retrieval with call order/multiplicity and typed field-use evidence. Review RTTI/vtable/callback anchors. Pilot independent BSim/Version Tracking retrieval where the current graph misses families. | Discovery replenishes eligible source work, with family-disjoint tests and reviewed unknowns; scores never automatically write CSV. |

Stages 2–5 overlap. Implementation starts as soon as one subset has its PC contract, without waiting for the whole wave or every type to be complete. Stage 1 is bounded: it must not become another prolonged tiny-match campaign.

## First wave: contracts and implementation ownership

Use the current eight available subagent slots with GPT-6.1 Sol. Reuse agents; no nested delegation. The main agent performs concrete reconstruction alongside them.

| Owner | Initial task | File/coordination boundary |
| --- | --- | --- |
| Main agent | Integrate baseline corrections; establish PC GameObject stride/needed field cohorts; personally reconstruct a contract-ready AI-script group in `src/gameapi/aiscript.c` alongside them; integrate accepted header/map proposals and verify whole batches. | Own startup-helper integration and `src/gameapi/aiscript.c`; sole writer for `saga.h`, `externals.h`, CSV, reports and badges. |
| Sol 1 | Review movement candidates: identity, full extent, caller ABI and private helpers. | Research packets only; no shared source/header/CSV writes. |
| Sol 2 | Review GameObject candidates, prioritizing producers/initializers and fields reused by movement. | Research packets only. |
| Sol 3 | Review AI candidates and action/condition/callback relationships. | Research packets only. |
| Sol 4 | Recover blocking constants/tables/global extents and initializers for released subsets. | One writer for `src/data.cpp` after main-agent approval of evidence; other data work remains proposals. |
| Sol 5 | Implement the first contract-ready movement group. | Own `src/legoapi/move.c`; private helpers stay with verified callers. |
| Sol 6 | Implement the first contract-ready object group. | Own `src/legoapi/gameobjects.c`. |
| Sol 7 | Implement the first contract-ready AI-system group. | Own `src/gameapi/aisys.c`. |
| Sol 8 | Implement independent accepted portable backlog while the first wave's interfaces are prepared. | Pick one unowned existing TU in parser/config/GameAnim/gizmo families; disclose exact file before editing. |

Do not put implementers on uncertain interfaces and leave them waiting. Initially they can prepare source/reference packets or take accepted backlog. As contracts become ready, shift reviewers into additional independent source units or the next discovery wave. If an original-TU closure crosses today's file boundaries, main-agent integration resolves ownership before edits.

All assignments must name the exact function group, accepted or candidate PC intervals, reference source, required fields/data, owned files and expected deliverable. Task scope should be a meaningful connected source group, not one accessor. Avoid multiple files containing variants of the same subsystem.

## Layout and source rules

- Integrate the independently established **PC GameObject stride 0x10d8** with opaque unreviewed tail and a full stride assertion; preserve all verified member offsets. Do not import Android's 0x10e4 size.
- Start with transform/position/contact/spline field cohorts used by released movement/object groups. Unknown joint/matrix regions block only their consumers.
- Recover AI record fields and callbacks from PC producers, readers and caller ABI. Android prototypes are hypotheses until verified.
- Preserve original TU/static/inlining context. Keep truly local declarations local; do not force private helpers into globally callable stack-ABI wrappers.
- Use named typed fields and `STATIC_ASSERT`; no byte-pointer offset access. Use C units for C routines and C++ where real linkage/features require it.
- Keep Android platform code out. Use actual PC API/services; never fabricate collision, allocation, callback or resource success to close a dependency.
- Mark real missing operations with STUBBED at their callsites. A partial body may be compared, but its skipped behavior must be listed. If an unknown return value controls the reconstructed logic, research it or defer that consumer.

## Verification and commit cadence

Per-owner development uses isolated object/comparison outputs and frozen shared contracts. Shared header proposals are grouped into small coherent integrations. There is one aggregate build/matcher lane, owned by the main agent.

For every coherent production checkpoint:

```sh
make check
make -j"$(nproc)" build check-objects
make -j"$(nproc)" match
```

Inspect changed functions at complete extents and all real relocation destinations. Check exact-match regressions against the previous verified report. Refresh report/badges, commit with the required Codex co-author identity, push under existing authorization, and inspect CI. Target roughly 30-minute implementation checkpoints when verified work is ready; never force an unverified commit to satisfy the clock.

Keep the simple Makefile and the source-only Windows executable linkable. No host/WASM game target, Wine headers, substitute implementations or retail bytes in candidate source.

## Measures and progress gates

Each wave records:

| Stage measure | What it diagnoses |
| --- | --- |
| Candidate original bytes reviewed | Research throughput; candidates may still be false |
| Accepted unique original bytes | Mapping/boundary throughput |
| Accepted bytes with ABI/layout/TU contracts | Whether shared research feeds source owners |
| New unique compared original bytes | Actual source expansion |
| Net fuzzy-weighted bytes / whole-code percentage | Main Windows matching progress |
| Exact bytes gained/lost | Reproduction and regression quality |
| Partial functions and skipped operations | Remaining behavioral gaps |
| Active research/source/integration time, waiting, tool time and rework | Where parallel capacity is useful |

Use the first three broad waves to establish actual acceptance and transfer rates. A 50–100 KB candidate wave is a planning scale, not a promised matching increase. Do not select only tiny easy functions or suppress low/zero scores to improve averages.

The first coverage milestone is **250 KB of unique original code compared**, up from about 178 KB. Next is **500 KB**, followed by **1 MB**. These are scope milestones, not matching percentages: report fuzzy and exact results at each. At 1 MB compared and 70% mean similarity, whole-code fuzzy would be about 20.2%, but that similarity is a scenario, not a forecast.

Reprioritize from observed bottlenecks:

- Few accepted/ready bytes: add identity, boundary, layout and callback research.
- Ready work accumulating: add independent source owners and batch shared integration.
- Large bodies repeatedly score zero: assign one bounded identity/layout/TU/source-shape audit; continue other families. Do not endlessly copy siblings or tune individual instructions.
- Verification dominates time: batch frozen checkpoints and reuse unchanged analysis/comparisons with exact input fingerprints.
- A low-dependency wave outperforms a complex object wave: expand that family while the layout research continues separately.

Retain whole-PC fuzzy as the main metric. Mapping expansion, linked runtime behavior and exactness remain separately reported. The longer-term 40–50% objective requires substantially more discovery and source scope; revise its forecast only after measured broad-wave results.

## Next concrete checkpoint

Integrate and verify the static startup-helper and `/GS` corrections, establish the initial field/ownership packets, and accept the first movement/object/AI subsets into CSV. Then produce the first **substantial multi-TU source expansion**, reporting new unique compared bytes and whole-PC fuzzy change separately from map-only additions. NuMain remains explicitly partial.

## First verified execution checkpoint

The first multi-unit release compares **13,982 additional unique original code
bytes**, increasing 178,198 to **192,180**. Whole-PC fuzzy increases
**2.818362% → 3.121956%** (+0.303594 percentage points); native objdiff similarity
is 56.40272% across 192,235 compared bytes including the existing 55-byte alias.
Exact code increases by 1,602 bytes to 33,085; 17 functions and 148 data report
rows newly match exactly, with no lost exact functions or data. Mapping increases
695,188 to 722,600 image bytes (1.735061% → 1.803476%). These are measured release
results, not acceptance of the full 89 KB candidate wave.

The main bottleneck was shared layout/data integration: reviewed source owners
accumulated ready bodies while waiting. Future releases should integrate minimal
field packets earlier and keep later cohorts preparing scratch source while the
previous snapshot builds. Private-helper TU experiments succeeded; forcing
ordinary public interfaces would have blocked two useful families. Some large
candidates still need genuine backend/callback or geometry closures, and the
ForcePush selector needs a bounded representation audit. Do not interpret its
zero score as either implementation success or a reason to tune every sibling.

Next release: complete ApplyGravity using the recovered tube/context providers;
expand character path/processing to remove Snap's genuine missing query; recover
larger object helper closures; and load gizmos with the real edit-file subsystem.
Keep the 250 KB unique-compared milestone and broad original-byte throughput as
the scope targets. NuMain/startup remain partial.

## Second verified execution checkpoint

The second release adds **18,213 unique compared original code bytes**, reaching
**210,393** (objdiff's 210,448 includes the unchanged 55-byte alias). Whole-PC
fuzzy increases **3.121956% → 3.461923%**; native similarity is 57.129005%.
Exact code increases by 548 bytes to **33,633**: twelve new exact functions and
87 exact data report rows, with no lost exact results. Mapping reaches
**742,934 / 40,067,072 bytes (1.854226%)**. Combined with the first checkpoint,
source comparison expands by 32,195 unique PC bytes over the strategy baseline.
These releases do not yet meet the 250 KB compared-code milestone.

Complete structural bodies now include gravity and its private helper, formation
and character-path geometry, player layers and camera/goal-speed selection,
gizmo/edit-file loading, infinite-line geometry, and spline-area intersections.
Snap calls the actual character-path query; its two surface actions remain
explicit omissions. NuMain and a playable game remain incomplete. Large private
formation bodies can score poorly despite coherent reconstruction; full extents
and genuine relocations remain in comparison, including a zero-scored driver.

The parallel queue now continues into the 17 KB directional/vehicle family,
12 KB object lifecycle family, AI destination/action drivers, and a contrasting
character-configuration parser wave. Reviewers prepare contracts and scratch
sources during frozen aggregate checks; source owners resume after the commit.
The main agent reconstructs the particle backend closure alongside them. This
avoids making every agent wait for the aggregate validation lane. All agents use
GPT-6.1 Sol without nested delegation. The continuous objective prioritizes new
whole-PC compared bytes and fuzzy gain, with verified checkpoints roughly every
30 minutes rather than a fixed number of tiny exact matches.

## Third verified structural comparison

The next frozen source wave adds **66,526 unique compared original code bytes**,
reaching **276,919** and passing the 250 KB milestone. Native objdiff compares
276,974 bytes including the unchanged 55-byte alias, with 59.16157% similarity.
Whole-PC fuzzy coverage increases **3.461923% → 4.718974%** (+1.257051 percentage
points, 43,635.32 additional similarity-weighted original bytes). Exact code
increases 7,208 bytes to **40,841 / 3,471,246 (1.176552%)**: 132 new exact functions,
721 total, with no previous exact function lost. Image mapping reaches
**835,186 / 40,067,072 (2.084470%)**, an increase of 92,252 known original bytes.

The character configuration closure contributes over 20 KB, with its real
callback table, parser state and loader providers. Other broad releases include
AI movement/retreat drivers, script runtime and character processing, vehicle
movement, object lifecycle, and a typed particle constructor. The main agent
implements the constructor/math/API/query and file-pack loading closures while
integrating reviewed source, data, fields and maps from eight parallel Sol agents.

Partial source remains visible: AI networking notification, selected lifecycle
and particle effects, damage/debris actions, nested gizmo destruction and parser
diagnostics have explicit call-site markers. Large partial lifecycle bodies score
poorly; these full comparisons remain in the denominator. Successful source-only
EXE linking does not establish playable gameplay or completed NuMain. Two former
12-byte LevHSpecial aliases are now views into one exact 1,056-byte array; their
old inventory rows remain, but there are no duplicated source owners.

Next wave prepares takeover/tag/rider logic, ordinary AI actions and path/circle
instructions, socket configuration/geometry and last-safe/Buck dependencies.
Reviewers verify identities and full extents while source owners prepare scratch
C; the root integrates field cohorts before production release. Continue the
active structural objective and regular verified commits rather than stopping at
this milestone or tuning the already-compared functions.

## Fourth verified structural checkpoint

The next coherent source batch adds 48,885 unique compared original bytes,
bringing comparison coverage to 325,804 bytes. Whole-PC fuzzy coverage rises
from 4.718974% to 5.705892%, a net gain of 34,258.36 similarity-weighted bytes.
The native objdiff average is 60.79941% over 325,859 bytes, including an unchanged
55-byte overlap; this average is not the whole-game percentage. Exact coverage
is 45,048 bytes (1.297747%), with 795 exact functions, 74 newly exact functions,
and no lost existing exact functions or data. Image mapping is 887,823 of
40,067,072 bytes (2.215842%). The retail bounds remain unchanged.

This batch transfers ordinary AI actions, locator/follow/path/interrupt logic,
takeover/tag/rider processing, object management/removal, socket parsing and
geometry, and last-safe/jump/Buck dependencies. Real animation, collection,
explosion, matrix/quaternion and input queries close dependencies. The animation
loop now builds basketball and rocket descriptors with recovered callbacks and
real AddPart calls. Existing special lookups move into their original nuspecial
owner; the migrated bytes are not counted as new coverage.

Large partial functions remain honest: ManageGameObjects scores about 10.54%,
AfterAnimation about 23.82%, and nested gizmo destruction about 28.77%. Missing
cleanup, rendering, debris, collision and context actions are still marked at
their real call sites. These low scores remain in the whole-code computation.
No returned query is replaced with invented success. NuMain and gameplay remain
partial despite the source-only EXE linking successfully.

Next contract-ready source work is the connected movement dispatcher and GameAI
combat/follow actions. The socket camera family requires genuine terrain
scan/cast results before release; its research remains separate from transferred
code. Continue the active goal with all eight Sol agents, prioritizing new
verified PC spans rather than small instruction-order refinements.

## Fifth verified structural checkpoint and pause

This batch adds 27,002 unique compared original bytes, bringing the total to
352,806. Whole-PC fuzzy coverage rises from 5.705892% to 6.211619%, adding
17,555.00 similarity-weighted bytes. Native objdiff averages 61.121914% over
352,861 bytes, including the unchanged 55-byte overlap. Exact coverage is
45,746 bytes (1.317855%): 801 exact functions, six newly exact functions and
698 newly exact bytes. No previous exact function or data result is lost.
Image mapping is 913,001 of 40,067,072 bytes (2.278682%).

New compared families include the movement dispatcher and POD/rolling/gunship/
circle/force-along modes; GameAI simpleton, combat and control actions; the
complete UpdateGameObjects comparison span; and genuine supporting queries.
GetAICreatureOrigin, ViewCamGetGamePad, oneAtOnce_CanAttack,
Action_SetDefaultMovementRange, MovementMtxRotateY and InDoubleScoreZone
match exactly. The batch also adds 131 exact data comparison rows.

UpdateGameObjects remains partial at 36.11261% with 36 missing void actions.
Its RememberPlayerIDs call is an explicit action gap until the real provider
is reconstructed. The genuine name lookup remains nonexact at 53.810345%;
MakeThrowVector scores 97.438095% without exact equality. Unknown data owner
extents, including Hub input views and adaptive difficulty, remain uncredited.
Source-only linking does not establish completed NuMain or playable gameplay.

At the user's request, expansion stops after this verified commit and push.
Retained next-wave contracts cover 8,385 bytes of enemy/control callbacks,
player context/reset dependencies and PartCollide/Deflect/Torpedo queries.
Terrain camera candidates remain withheld for state-producer and semantic
verification. Resume with these retained packets instead of repeating setup
or whole-binary identification.


## Resumed CompleteLevel source release

The user resumed continuous work after the earlier stopping point. The active
objective continues bounded identification and genuine source transfer, with
serialized heavy jobs, verified commits and no fixed stopping milestone.

The CompleteLevel closure adds 566 newly compared original code bytes, reaching
353,372 unique bytes (353,427 including the unchanged 55-byte overlap). Whole-PC
fuzzy coverage rises from 6.211661% to 6.224201%, a gain of 0.012540 percentage
points. All 801 exact functions and 45,746 exact code bytes remain unchanged.
The four helper identities and queue counter add 290 mapped bytes, reaching
1,116,538 of 40,067,072 (2.786672%). Only the counter adds four exact data bytes;
unknown queue/flag/string access views earn no extent credit.

The source callback is close at 98.175255%, but none of the five new routines is
exact. The level fallback's genuine area lookup is inlined by VC8 and scores
poorly; retain its complete comparison and prioritize the next source closure
before instruction ordering refinements. Next prepare the accepted 606-byte
RaceOpponent callback using the existing movement providers. Speed-control
callbacks require separate field and dependency review; indirect lookup and
unmapped providers remain concrete holds. NuMain and gameplay remain partial.


## Race and animation-speed transfer

The next connected source release adds 976 original code bytes, reaching
354,348 unique compared bytes (354,403 with the unchanged 55-byte overlap).
Whole-PC fuzzy coverage rises from 6.224201% to 6.249405%, a gain of 0.025205
percentage points. All 801 exact functions and 45,746 exact code bytes remain
unchanged. Previously accepted code inventory is now implemented, so mapped
bytes remain 1,116,538 / 40,067,072. Five new literal bindings have unknown
complete original extents and earn no mapping or exact-data credit.

The release closes RaceOpponent's existing movement/socket/parser providers
and the animation-speed callback's small parser chain. Both actual compiled
rounding and argument contracts pass independent review; full original byte
comparison retains their nonexact regions. Next prepare CurrentStart and
UseCurrentSpeed as a connected 624-byte source closure. CurrentStart's 288-byte
PC identity/ABI proposal requires independent approval; its PC dogfight-level
check differs from Android TwistLevel. The separate SetCurrentSpeed indirect
lookup remains a concrete unresolved provider boundary.


## Current-speed source release

CurrentStart and UseCurrentSpeed add 624 compared original code bytes,
reaching 354,972 unique bytes (355,027 including the unchanged 55-byte
overlap). Whole-PC fuzzy coverage increases from 6.249405% to 6.265642%,
a gain of 0.016237 percentage points. All 801 exact functions and 45,746
exact code bytes remain unchanged. CurrentStart's independently verified
complete 288-byte identity adds that extent to the map: 1,116,826 of
40,067,072 virtual bytes, or 2.787391%. Two source literal access views
have unknown complete allocations and receive no extent or exact credit.

Both actual compiled contracts pass independent review. The original PC
zero-only/NaN-admitting comparison and dogfight-level fallback are retained;
no historical aggregate signature or complete data owner is inferred. The
next connected source target is SetScaleOverride's accepted 687-byte body
with existing fields and parser/random providers. SetCurrentSpeed's distinct
indirect lookup remains held. NuMain and playable gameplay remain incomplete.


## Scale-override source release

SetScaleOverride adds 687 compared original code bytes, reaching 355,659
unique bytes (355,714 with the unchanged 55-byte overlap). Whole-PC fuzzy
coverage rises from 6.265642% to 6.276357%, a gain of 0.010715 percentage
points. All 801 exact functions and 45,746 exact code bytes are preserved.
The accepted inventory extent already existed, so mapped bytes remain
1,116,826 / 40,067,072. The reset source literal has an unknown complete
allocation and receives zero mapping or exact credit.

Source and actual compiler reviews preserve floating-point predicates,
random weighting, timed-target rounding and rounded rate reuse. The
645-byte compiled body scores 54.141594% and remains nonexact; prioritize
the next connected closure over instruction-order tuning. Next review
SnapToPosition/SnapToSockPosition's shared surface helpers and socket helper.
Candidate helper sizes/identities stay unaccepted until fresh original
extent, physical ABI and provider closure pass independent review.


## Surface and socket helper release

Two independently identified complete helpers add 474 newly compared and
known mapped PC bytes: SetObjOnSurface233 and SetSockPostion241. Unique compared
code reaches 356,133 bytes (356,188 with the existing 55-byte overlap). Whole-PC
fuzzy coverage increases from 6.276357% to 6.285899%, gaining 0.009542 percentage
points. All 801 exact functions and 45,746 exact code bytes remain unchanged.
Known mapped image extent reaches 1,117,300 / 40,067,072. The callback pointer
access view has unknown allocation extent/type and earns no coverage credit.

Source and actual compilation reviews cover PC float rounding/predicates,
socket boundary controls and the generated private helper cohort. The two
new bodies remain nonexact, scoring 72.968254% and 66.89286%; continue provider
closure rather than instruction tuning. Next close InitSurfaceInfo's actual
query and shadow/reflection providers to release the accepted snap callback
family. The separate SetCurrentSpeed indirect lookup remains unresolved.


## Surface query and terrain provider release

GetSurfaceInfo536, four getters109 and the previously mapped vector transform196
add 841 compared original bytes (357,029 total, 356,974 unique). Whole-PC fuzzy
coverage rises from 6.285899% to 6.304993%, gaining 0.019094 percentage points.
The four getters are exact, adding 109 original bytes; all 801 earlier exact
functions survive, yielding 805 functions and 45,855 exact bytes. New function
identities add 645 mapped bytes, reaching 1,117,945 / 40,067,072. Unknown global
access views add no original allocation coverage or exact-data credit.

The query and vector bodies remain nonexact at 67.30769% and 98.477615%.
Their source preserves qualified PC predicates, field access, in-place staging
and required float storage; instruction scheduling can wait. Next close the
8,155-byte NewScanRot/NewCast source group with PC query/scan/cache/polygon
records and its TerrainSkinAllocate/SkinPlatform/TerrainSkin provider cohort.
Physical effects are reviewed separately from unresolved source fields and
initialization/bounds. Then close shadow/reflection and release InitSurfaceInfo
and the accepted snap callbacks. SetCurrentSpeed's unrelated indirect lookup
remains held.

## Shadow and surface initialization release

Six connected implementations add 1,260 compared original bytes, reaching
369,300 total and 369,245 unique after the unchanged 55-byte alias overlap.
Whole-PC fuzzy coverage increases from 6.3935661% to 6.4234240%, gaining
0.02985795 percentage points. Four new exact functions add 649 bytes; all
806 earlier exact functions remain, yielding 810 functions and 46,519 exact
bytes out of 3,471,246 text bytes. Three new independently reviewed identities
add 621 mapped bytes, reaching 1,118,618 / 40,067,072 virtual image bytes.
Two neutral data access views earn zero extent credit.

SnapCreaturePos's existing implementation now calls both real surface
providers. InitSurfaceInfo and PlatOnOff remain nonexact; retain their complete
comparisons and normal-engine qualifications while expanding source closure.
Next review the accepted 2,076-byte SnapToPosition/SnapToSockPosition action
family, including parser rounding, socket providers, actor writes and the
separate eight-slot party pointer view. The unrelated current-speed indirect
lookup and the playable entry path remain incomplete.

## Snap actions and structural propagation checkpoint

Two previously mapped actions add 2,076 newly compared original bytes: 371,376
total and 371,321 unique after the unchanged 55-byte overlap. Whole-PC fuzzy
coverage rises from 6.4234240% to 6.4712667%, a gain of 0.04784270 percentage
points. All 810 exact functions and 46,519 exact code bytes remain unchanged;
known mapped image extent remains 1,118,618 / 40,067,072. Eleven unknown-size
strings and two overlapping scalar aliases add zero extent credit.

The next work is structural discovery: verify shared-caller signature leads,
repair independently established missing code nodes, and test a candidate-only
function-to-data propagation channel with explicit accepted-root lineage. A
longer propagation experiment reaches a fixed point after seven rounds, adding
22 tentative pairs beyond the four-round pool. This modest yield is evidence
against assuming exponential growth from more rounds alone. Preserve original
identity/ABI/extent review and distinguish tentative propagation from accepted
mapping and source comparison progress.

## Structural identity and candidate-data channel

Five independent structural identity reviews add 530 mapped code bytes, reaching
1,119,148 / 40,067,072 virtual image bytes. They add no source definitions,
compared original bytes or fuzzy/exact credit. StartLaunch remains an ABI hold,
despite its verified graph boundary. The longer run plus missing-node repairs
has 1,328 tentative function pairs; inferred counts remain research inventory.

The new opt-in data channel carries tentative function ancestry into a separate
candidate ledger. It counts deduplicated accepted roots and does not change the
confirmed-user data scorer or feed its output back into function/state/table
scoring. Next compare disabled and enabled runs with identical inputs, compare
four versus sixteen rounds, and review newly exposed data roles/owners against
the originals. Holdout support remains explicitly unavailable for this channel;
no accuracy or calibrated confidence claim follows from candidate yield.

The controlled candidate-data experiment completed under a hard 1 GiB process
limit: four rounds produce 9,892 tentative data rows; the seven-round fixed
point produces 9,949, a net +57. These are uncertain hypotheses, potentially
including existing candidates, and have no mapping/extent/source credit.
Normal function/data/state outputs agree between disabled/enabled runs at the
same round limit. The final experimental run peaks at 1,006,836 KiB RSS with
zero swaps; the first 10,000-row guard failure is retained explicitly.
## Structural anchor cascade

Seven reviewed function identities add 389 mapped code bytes, bringing known
image extent to 1,119,537 / 40,067,072. Three data identities retain unknown
allocation/type and zero extent credit. This release adds no source comparison
bytes: 371,376 compared bytes, 810 exact functions / 46,519 exact code bytes
and 6.4712667% whole-PC fuzzy coverage remain unchanged.

The improved anchors produce a larger cascade: 1,528 tentative function pairs,
a net increase of 199, reaching a fixed point after eight rounds. The separate
function-to-data channel produces 10,835 hypotheses. Its compact CSV now allows
bounded selection followed by exact JSONL evidence seeks. Candidate counts do
not measure accuracy; inferred roots must remain explicit and cannot become
reviewed anchors without original verification.

The disposable repair of the already accepted 192-byte AIPathCnxSetTemporaryBlock
now preserves all 8,045 prior function extents and adds exactly one function.
Next replay propagation on that graph, review remaining global caller-signature leads and examine
selected data users/owners against both originals. Preserve callback/ABI holds
and unknown allocation boundaries. Heavy jobs remain serialized; the expanded
data experiment used a 1.5 GiB address-space cap and about 1 GiB peak RSS after
the original 1 GiB cap safely stopped an earlier attempt.

## Structural leaf expansion

Twelve independently verified function identities add 3,920 known code bytes,
reaching 1,123,457 / 40,067,072 mapped image bytes. Their historical types remain
blank. Ten independently reviewed data identities retain blank allocation sizes
and types and add zero extent credit. Source comparison stays at 371,376 original bytes, with 810 exact
functions / 46,519 exact code bytes and 6.4712667% whole-PC fuzzy coverage.
The missing setter repairs preserve all 8,046 earlier function extents and
bring the graph to 8,049 functions.

Before the creature pair and data identities, the refreshed run has 1,467 accepted function seeds, 1,522 inferred function
pairs and 2,060 normal data candidates, with 99 reviewed data anchors and no
stale decisions. Six earlier hypotheses became accepted seeds; all other
1,522 hypotheses are retained, with zero new pairs or other disappearances.
Eight rounds reach a fixed point. Candidate counts are not
verified identity or accuracy counts. Next compare exact hypothesis sets as
reviewed seeds replace candidates, extend global shared-caller discovery and
test a separate single-parent call-context diagnostic. Review the fixed data
shortlist through actual original user operands and owners; ancestry and
address-only evidence cannot authorize data names, allocations or types.
The single-parent context diagnostic supplies 102 review leads but recovers
only six of twenty known withheld controls; this is a research queue, not a
measured accuracy gain or an automatic acceptance rule. TerrainInitEx's qualified
physical follow-up and the new Batarangs_Reset identity await the next release;
PanelRender retains finite physical-contract holds. Continue original review
and turn the diagnostic prototype into a bounded, reproducible tool.

After the creature pair and ten data identities, the final run has 1,469 accepted
function seeds, 1,522 inferred pairs, 2,063 normal data candidates and 109
reviewed data anchors, with no stale decisions. Eight rounds reach a fixed point.
This run takes 47.36 seconds at 746,344 KiB RSS under the 1 GiB address-space
limit, with zero swaps. Equal inferred totals do not establish equal pair sets
or scoring accuracy; preserve the exact frozen outputs for comparison.

## Larger structural mapping and call-order tool

Seven more reviewed identities add 6,773 known PC code bytes, including
TerrainInitEx, NuDisplayListRndrSpecial, AddDebrisEffect and KeepVehicleOnScreen.
Source comparison, exact matches and fuzzy coverage receive no inventory credit.
The new bounded call-order tool uses accepted caller/callee anchors and reviewed
data-role evidence, retains competitors and publishes candidate-only results.
Its full-input run takes about 1.3 seconds at 44 MiB RSS under a 256 MiB cap.

The normal joint rescore has 1,476 accepted seeds, 1,515 inferred pairs and 2,087
normal data candidates, with 109 reviewed data anchors and no stale decisions.
Eight rounds reach a fixed point in 48.19 seconds at 739,168 KiB RSS under the
1 GiB address-space cap, with zero swaps. Six prior hypotheses became seeds;
Batarangs_Reset is a newly accepted pair. The advisory pair 0x005d6860 /
0x002d5670 no longer propagates but remains a rank-one raw candidate. No new
inferred pairs appear. Counts alone do not establish identity or accuracy.

Continue original review of the new InitPartDebris/InitParts call-context leads.
GizmoBlowUp_Hit's shared explosion-flag ancestry supplies a distinctive identity
and complete PC boundary; actual installed callback tables and four helper
interfaces need qualified physical closure before map release. ReadPad's callback
and PanelRender's child/virtual interfaces remain separate holds. Keep heavy
jobs serialized and frozen counterfactual outputs intact.

### Wave 46: turn retained identities into usable inventory and source

Review the 18-function character/player cohort as one finite dependency family.
Five retained identities now supply 9913 usable inventory bytes with blank formal
types; the actual CutScene callback dispatch remains the next interface gate.
This inventory release adds no compiled behavior by itself.

The source batch adds the two animation buffer evaluators (3045 original bytes),
POI matrix composition, the camera matrix builder with explicit backend gaps,
and their finite matrix/projection providers. Existing partial typed views are
extended only with original-verified fields. Whole original extents enter
comparison; scratch-slot and normal-provider qualifications remain recorded.
The verified batch adds 4665 compared original code bytes. Whole-code fuzzy
coverage rises from 6.471500% to 6.550184%; exact code rises by 315 bytes to
46834 of 3471246 bytes. The camera builder remains explicitly partial.
The 125-object build, complete bindings/relocations and 640-test suite pass.
The normal scorer peaks at 689584 KiB RSS with no swap under its 1 GiB cap;
the tests peak at 131152 KiB and retail comparison at 58672 KiB.

The matching report now separates inventory/journal identification coverage from
reconstruction. Reviewed complete ranges outside CSV count in the identification
union, with overlaps and disjoint gaps handled explicitly. ABI-held identities
and unknown data extents never acquire compiled or exact-byte credit through
this reporting change. Heavy jobs remain serialized; fixtures use disk-backed
TMPDIR and helper processes retain bounded memory.

The normal graph run retains all 1541 previous accepted seeds and reaches
1547 accepted plus 1317 inferred pairs. Exact set comparison finds fourteen
union gains and one lost inference, for thirteen net gains. The lost pair
remains a rank-one raw candidate; pair counts do not establish accuracy.
The new identification union is 585401 bytes (16.864290% of original text),
including 26605 explicitly reviewed bytes outside the CSV inventory.
This percentage measures identified ranges, not reconstructed behavior.

Continue the connected EvalModelAnim/APIDumpCharacterModels/APICharacterSysInit
cohort (1465 prospective PC bytes) using the retained character-system views
and animation providers. Keep CutScene's actual callback dispatch and the
camera backend gaps as separate contract gates.

### Wave 47: close the viewport family and evaluate hierarchy joints

Implement the six-body viewport/backend family and the connected hierarchy
evaluator (1596 new original code bytes), with twelve minimum data views carrying
no original allocation credit. Replace the three verified camera call markers
and correct the existing projection builders' PC rounding/association. Keep the
remaining camera actions explicit and retain historical type holds.

The CutScene callback inventory release follows its actual installed dispatch;
it supplies no source coverage. Seven model/animation identity reviews add 2524
identified bytes with provider holds kept separate. Continue the blend animation
child and wrapper using existing buffer evaluators, then the default layer
selector and model animation caller. Scene cleanup still needs five real child
contracts; do not substitute an allocator with a different argument count.

Keep the frozen wave-46 report and exact pair sets as the wave-47 baseline.
Verify the complete VC8 build, new emitted bodies, old exact-match preservation,
full checks and normal graph transition before publishing metrics. Heavy jobs
stay serialized; disk-backed scratch and bounded graph/helper processes protect
the host's memory.

Wave 47 adds 1596 compared bytes and 327 exact bytes while retaining every prior
exact function/data row. Whole-code fuzzy coverage reaches 6.583432%, exact code
47161 / 3471246 bytes, and reviewed identification 589065 bytes (16.969843%).
The final normal control has 1555 accepted plus 1313 inferred pairs: four
hypotheses are promoted, four new seeds enter the union, and no unconverted
losses or new inferred pairs occur. Seven rounds reach a fixed point; increasing
the round cap cannot expand this control.

Next integrate the independently reviewed 708-byte animation player/wrapper
subset and 242-byte model layer/locator helpers using existing providers. Two
new animation scratch views keep unknown original extents; preserve the existing
callback owner and qualify its actual four-word invocation explicitly. Investigate
the callsite/CFG context lost by direct-neighbor sets with bounded review
annotations and production controls before changing propagation safeguards.

### Wave 48: release connected animation and model helpers

Integrate the reviewed animation children/wrapper (708 original bytes), the
one-byte cross-unit diagnostic return provider, and the model layer/locator
helpers (242 bytes). Preserve signed capacity, callback ownership, actual
overflow/pop behavior and raw locator copies. Compile, link and review emitted
interfaces, then compare all complete bodies and retain every prior exact row.
The two scratch data views keep unknown allocation extents and no credit.

Restore analyzed boundary diagnostics on final joint candidate rows without
changing their scores or eligibility. Test tied production candidates with
callsite/CFG annotations as review evidence; do not treat annotations or analyzed
boundaries as verified mappings. Continue the real DWA/provider closure needed
by the larger model animation caller.

The 128-object build and complete binding/link gate pass. The batch adds
951 compared original bytes and 262 exact bytes, preserving all 820 previous
exact functions and 2257 exact data rows. Whole-code fuzzy coverage reaches
6.605381%; exact code is 47423 / 3471246 bytes. The full LLVM 18 check passes
643 tests with three skips; final candidate diagnostic fixtures also pass after
their disjoint-range cleanup. Build RSS peaks at 38156 KiB and full checks at
120088 KiB, both with no swaps. Larger animation bodies remain genuine
nonexact source; no instruction-order tuning is needed before advancing the
real DWA/time/curve provider closure.

### Wave 49: close the real deformer providers

Implement the 1373-byte curve,333-byte time/node helpers,102-byte allocators,
1008-byte DWA parents and 96-byte verified blend-root wrapper as one connected
batch. Keep all format actions, source field assertions, float checkpoints,
partial-time definedness limits and unusual original allocator/clear behavior.
Four source backing views keep unknown original extents and zero credit.
Compile against the settled 20-byte DWA prefix; compare whole functions and
retain every previous exact row, then run full checks and the fixed graph
control before publishing.

The next connected skeletal family has a 1400-byte eleven-word backend and
777-byte parent plus 46-byte wrapper. Its children are already mapped or part
of the current curve family. Review shared pivot/override fields, the separate
skeletal 24-byte header, lower x87 prefixes at FTOL and initialized-local
requirements before integrating genuine source. This will advance the larger
model animation caller without skipping its legacy branch.

Wave 49 adds 2912 compared bytes and 228 exact bytes; the full LLVM 18 check
passes 643 tests with three skips. The settled build links 128 objects with
complete bindings. Normal propagation reaches 1563 accepted plus 1309 inferred
pairs: three old hypotheses are promoted and three new seeds enter the union,
with no new inferred pairs or unconverted losses. Seven rounds reach a fixed
point. Two additional data candidates remain advisory. The graph process peaks
at 692440 KiB RSS with no swaps; its address-space cap remains 1024 MiB.

The skeletal parent draft needs the original identity-template owner and signed
override-loop entry guard; preserve the rejected draft and independent review.
Missing-node scale and partially written time fields remain explicit source
domains. Integrate the corrected family only after this checkpoint's final
whole-body comparison, previous exact-match preservation and pushed CI pass.
