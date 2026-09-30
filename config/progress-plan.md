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
