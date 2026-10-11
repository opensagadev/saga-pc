# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
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

1. Attribute the current startup wait, using unchanged source and owned bounded
   observations. Reconstruct the earliest verified missing action or incorrect
   PC binding. Resolve thread/timer storage ownership before activating that pair.
2. Close genuine window/input/device initialization and presentation providers,
   preserving native failure behavior. Verify repeated frames from installed assets.
3. Close permanent/area/world loading and real character/camera activation, then
   recurring updates, movement, collision, menus, audio and transitions. Prepare
   connected independent families in parallel when they help these milestones.
4. Integrate named layouts, mappings and storage once per coherent wave. Run
   required checks and full retail comparisons, preserve prior exact bytes, push
   validated checkpoints and verify CI on each exact revision.

Use [strategy.md](strategy.md), [identification-plan.md](identification-plan.md)
and [transfer-execution.md](transfer-execution.md) for selection and validation.
The [roadmap](../docs/roadmap.md) separates runnable milestones from byte coverage.
Earlier [plans and release history](../docs/history/progress-plan-through-2026-10-03.md)
remain available as evidence.
