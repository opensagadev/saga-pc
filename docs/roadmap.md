# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The current source
build is **not playable**. Published checkpoint II
[`9c78272c`](https://github.com/opensagadev/saga-pc/commit/9c78272c5847aef790e82320b0a2401c20906c52)
has [passing CI](https://github.com/opensagadev/saga-pc/actions/runs/37710401246),
but its actual Wine run exits at a null shader-manager receiver before loading a
level. A transient window and bounded CPU tests establish neither persistent
window/device presentation nor recurring gameplay. Code similarity and mapping
are separate from runnable milestones; current measurements live in
[matching.json](../matching.json).

The verified QQ checkpoint [5f419786](https://github.com/opensagadev/saga-pc/commit/5f4197868473c2ce32ddaca5a932c1afb5ed3b43) passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37848679469).
The current RR source adds 66 complete function extents/13,504 compared PC bytes,
including the startup publisher and original-owner callbacks. Required checks and
90 selected CPU assertions pass using actual normal providers; full qualifications
are in transfer-checkpoint.json. Ten publisher actions and 19 distinct missing
family providers remain explicit. These checks do not establish successful
initialization, loaded assets, device presentation or gameplay. Genuine shader
publication and complete resource lifetimes remain missing. II is still the last
full-game observation; RR CI is pending.

Work proceeds through connected source families and verified PC contracts:

1. Complete genuine shader-manager publication and its constructor, parser,
   compiler and backend dependencies. Verify real startup state and a persistent
   native window/input path without fabricated providers or initialization bypasses.
2. Close device/reset/resource lifetimes and presentation. Verify repeated real
   frames and nontrivial pixels from installed assets, keeping CPU and GPU evidence
   separate.
3. Complete permanent, area and world loaders, scene/material/model fixups and
   character/callback activation. Verify a populated world/player/camera from real
   input files and producers.
4. Complete the recurring update/render loop, movement, collision, menus, audio,
   saves and transitions. Exercise a playable installed level and then the remaining
   game content; an isolated level or transfer checkpoint does not finish the goal.
5. Continue complete-byte matching alongside functional reconstruction. Preserve
   prior exact results and resolve PC-specific behavior, ABI and layout differences
   before treating source similarity as correctness.

Release reviewed subsets continuously; a missing family does not block independent
correct source. Partial actions and runtime limitations remain explicit. Android
is source evidence, while PC callers, fields, native boundaries and behavior
establish the contracts.

[CONTRIBUTING.md](../CONTRIBUTING.md) covers setup and validation.
The [current work plan](../config/progress-plan.md) and
[execution policy](../config/transfer-execution.md) guide development.
The original [implementation handoff](development/implementation-plan.md) and
[historical evidence index](history/README.md) retain the full earlier plans and
checkpoint records.
