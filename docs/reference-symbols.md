# Reference symbols from a sibling game

A private symbol dump of another game made by the same studio on an older
version of the same engine (a PS2 build, debug symbols exported with stdump)
gives, for about 10,900 functions, the original source file, the prototype with
parameter names and types, and every local variable in declaration order. A
separate file has the struct, union, enum and typedef definitions with offsets.
About 2,560 function names also exist here. The dump is never committed; keep
it untracked under `build/reference/` (git-ignored).

The engine changed between the two games, so a hint is a lead: retail bytes
decide.

## Tool

`tools/refsyms.py` parses the dump and compares it with our sources (our side
is parsed with libclang). Without the dump every command prints a notice and
exits 0.

```sh
python3 tools/refsyms.py NuAllocFPar          # reference prototype and locals next to ours, with hints
python3 tools/refsyms.py --overlap            # functions in both, non-exact first
python3 tools/refsyms.py --hints out.json     # hints for every non-exact function in both
python3 tools/refsyms.py --structs [NAME]     # same-named structs: field type differences that change code
python3 tools/refsyms.py --files              # reference source file vs our unit
python3 tools/refsyms.py --reference DIR ...  # dump in another directory
```

Hints: parameter and return type classes (signedness, width, float, pointer
vs integer; typedefs and enums are resolved on both sides), locals only on one
side, the local count, the declaration order of shared names, and locals
declared several times (one per inlined expansion of a helper; the helpers of
the same file that take such a parameter are named).

## What paid off (pilot, 25 functions)

| hint | tried | exact | improved |
|---|---:|---:|---:|
| fewer locals in the reference: index the array or read the field at each use instead of a cached pointer/copy | 10 | 6 (+3 callers that inline them) | 1 (2 worse, reverted) |
| a helper expanded inline (repeated locals): call the same-unit function instead of a macro | 4 | 3 (+1 sibling) | 1 |
| declaration order / initialised locals | 2 | 0 | 2 |
| no hint applied (known U classes) or other cause | 8 | 2 (shift mask, not from the dump) | 0 |
| struct bit names (bitfield store where we masked a word) | 1 | 0 | 0 (reverted) |

- The most frequent and most useful hint is *the reference has no local for
  this*. Decompiled ports cache `&array[index]` or `object->field` in a local;
  VC8 then keeps it in a callee-saved register, while the original indexed the
  array at every use (`NuAllocFPar`, `NuFileBeginBlkRead`,
  `NuStringTableUnload`, `NuMemFileRead`/`Write`, whose inlined copies made
  `NuFilePatchAddress`, `NuFileAlign` and `NuStringTableSaveCharacterList`
  exact). It is not universal: `edcamGetPosAng` and `NuMemFileClose` got worse
  (retail reads the global once, I4), so keep the change only when it helps.
- Repeated locals show inlining: `NuStrNICmpW` declares `c` twice, the
  parameter of `NuToUpperW`, so the string functions called `NuToUpperW` (same
  unit, inlined by VC8) rather than a macro. With the helper written as
  `if (range1) c -= 0x20; else if (range2) c -= 0x20;` three functions and `NuStrUprW` became
  exact. Inlining in the PS2 build is weaker evidence on its own: `NuStrLenW`
  is inlined there but called in retail.
- Struct definitions differ little where both games have the struct (149
  same-named structs, 16 field type differences, mostly PS2-specific). The
  useful part is bitfield names for words we treat as masks:
  `APIOBJECT_PC.flags_low` is the object flag bitfield (`ignore_antinodes` is
  bit 17), and retail stores it with the bitfield xor/and/xor sequence. With
  the bits declared as an anonymous union member, `Action_SetIgnoreAntinodes`
  went from 87.56 to 99.06, but the same header change moved code in two
  functions of unrelated units (`BoulderSpawnPc` left exact,
  `GameCameraMakeMiniCut3` 85.75 -> 74.79), so it was reverted. Header struct
  changes need a full build and match before they are kept.
- Parameter type hints (44 over the 648 non-exact functions in both) look
  mostly PS2-specific on inspection (`i64` file positions on PC, `u8` colour
  channels widened); none was tried.
- Yield: 15 of 25 functions tried became exact (13 directly, 2 for another
  reason) in about two hours, mostly from one hint type in one unit. Of the 648
  non-exact functions in both, the batch hints flag fewer reference locals than
  ours in about 150 (>= 75%, < 1200 B); at the observed rate of roughly one in
  two, the "cached local" lever might give 50-80 exact functions there, and the
  same rewrite (index instead of a cached element pointer) applies to
  functions without a name in the dump that use the same tables.
- File assignment: 123 reference files contain mapped functions; 38 of them
  are spread over several of our units, mostly where the PC build compiles a
  group of engine files as one unit (`nucore_includes.c`, `nu3d_includes.cpp`,
  `numath_includes.c`) or where we split an editor file (`edptl.c` into
  `edpp.c`/`edpp_load.c`, `debris.c` over five units). Not acted on.

Remaining differences seen in the sample were known unexplained classes: U8
(`lea eax, [reg+1]` for `return 1` after a NULL test of that register), U1, and
callers that keep `eax` live across a call to the empty `NuFileError`
(declaring it `noreturn` improved some callers and broke others, so retail
probably calls two different, folded error functions).

## Sibling build disassembly

A private sibling build of the same engine (a later game, compiled by GCC for
i386 with its symbol table and the source file of every function) shows the
original source structure independently of VC8: which calls are made and in
what order, which helpers stay calls, loop and branch shapes, constants,
float versus double arithmetic, field offsets and the order of stores. It is
kept untracked under `build/reference/` (git-ignored); the engine moved on
between the two games, so it is a lead and retail bytes decide.

`tools/sibling.py` (needs capstone) prints the sibling's disassembly of a
function next to retail's and ours, with symbolised calls, globals and float
constants, and a summary per side: calls in order, float constants, integer
immediates and field offsets. It compares the call lists sibling -> retail and
retail -> ours and lists constants and fields that only retail or only ours
use. Without the reference directory it shows retail and ours only.

```sh
python3 tools/sibling.py NuPortalWhichRoom            # summaries and three listings
python3 tools/sibling.py NuPortalWhichRoom --summary  # summaries only
python3 tools/sibling.py --scan names.txt             # one line per function: call-list agreement
python3 tools/sibling.py --reference DIR ...          # reference in another directory (default build/reference/batman-mac)
```

### Pilot (40 non-exact engine functions with a sibling counterpart)

10 per match band (95-99.9, 85-95, 60-85; all 9 below 60), larger functions
first, about 10-15 minutes each.

| band | tried | sibling showed a difference | exact | improved |
|---|---:|---:|---:|---:|
| < 60 | 9 | 5 | 4 | 1 |
| 60-85 | 11 | 4 | 0 | 3 (1 not from the sibling) |
| 85-95 | 10 | 1 | 0 | 1 |
| 95-99.9 | 10 | 3 | 0 | 0 |

What the sibling showed, by yield:

- A second `if` instead of `else if`: the sibling's then-path falls into the
  same compare as the else-path (I46). Four memory card menu updates exact.
- A store the source made conditional: the sibling writes
  `candidates[count] = room` on every pass (`NuPortalWhichRoom`, 71.3 -> 83.3).
- `va_start`/`va_arg` instead of a walked parameter pointer (I47).
- File assignment: a function the sibling compiles in the same unit as an
  empty hook it calls (`StateAnimEvaluate`, I35).
- An error block at the end of the function (`NuDatOpenEx`, +0.04).
- Seen but not productive: one shared tail store instead of an early return,
  no cached index local (`TerrainSkinAllocate`), a ternary (`NewRayCastMask`),
  separate locals per option (`Action_GoToLocator`), a bitfield where we OR a
  word (`xConditions`, not tried: header-wide change).

Most near-exact functions (95%+) differ in register choice, x87 order and
store scheduling, which the sibling cannot show: GCC allocates and schedules
differently. Many sibling functions were rewritten after the PC release
(AI condition evaluation, vertex declarations, `edanimLoadPage` became a
wrapper), and GCC inlines helpers that retail calls, so a call-list difference
alone is mostly noise (213 of 284 engine leads differ sibling -> retail).

### Detector pass (`tools/sibscan.py`)

The pilot's advice was automatic passes with manual work only on hits.
`tools/sibscan.py` runs seven detectors over every non-exact function with a
same-named sibling function (704) and prints ranked hits; `--exact-sample N`
runs them on exact functions, where every hit is noise.

```sh
python3 tools/sibscan.py                        # all detectors, ranked hits
python3 tools/sibscan.py --detector calls       # one detector
python3 tools/sibscan.py --exact-sample 600     # false-positive rate per detector
python3 tools/sibscan.py --all --names out.csv  # original-name candidates
```

| detector | what it compares | hits | noise on 599 exact | worked | exact | improved |
|---|---|---:|---:|---:|---:|---:|
| joins | compare blocks reached by two forward edges, sibling and retail more than ours (I46) | 21 | 0 (15.4% without the retail filter) | 2 | 0 | 0 |
| vararg | parameter-pointer walks in our variadic sources, sibling va_start (I47) | 0 | 0 | - | - | - |
| stores | stores per field offset or global, sibling agrees with retail | 29 | 0 (9.7%) | 3 | 1 | 0 |
| calls | call multiset or order, sibling agrees with retail | 17 | 0 (35.2%) | 6 | 3 | 3 |
| consts | constants, immediates, field offsets shared by sibling and retail but not ours (and the reverse) | 54 | 0 (40.7%) | 6 | 2 | 2 |
| files | empty or tiny callee: sibling file vs our unit (I35) | 6 | 1 | 3 | 0 | 0 |
| shape | loop count and counter direction, sibling agrees with retail | 15 | 0 (29.2%) | 4 | 2 | 0 |

Every detector but `joins` and `files` needs retail to differ from ours, so it
cannot fire on exact functions; the bracketed rate is the sibling-versus-ours
noise that the retail filter removes. GCC joins compares far more often than
VC8, so `joins` alone (sibling more than ours) flagged 34% of the leads and
15% of exact functions; requiring retail to agree left 21 hits. GCC keeps a
single return, so early returns are not compared. `files` mostly reports
folded no-op hooks and helpers the sibling moved between files.

Exact: `Condition_OriginRange`, `Condition_OpponentToOrigin`,
`Condition_PlayerToOrigin` (an element pointer for the creature; the sibling
shares one NuVecDist call), `NuStrFindPosU` (loop test at the head, range
compare), `Tag_NewTransfer` (stores through a vector pointer),
`Signals_StoreProgress` (loop bounded by both limits, indexed suit read),
`GizSpinner_GetTargetPoints` (direction derived from the position angle, int
step). Improved: `Hub_DrawPanel` 94.36 -> 97.02, `CastPart` 82.09 -> 84.46
(impact position stored before the normal), `ForcePushed_SetTargetMom`
92.91 -> 94.42, `NuPortalVisibility` 97.96 -> 99.85, `KillPartsByScene` 78.27 -> 79.19, `GizSpinner_Spin` 68.78 -> 69.15.

Recurring lessons: where the sibling has one call reached from two branches,
retail usually tail-merged two identical calls; the fix is to make the two
paths identical (same element-pointer arithmetic, same load order), not to
write one call. Where the sibling repeats a block per branch, so did the
source. A `u16` step or angle where retail keeps an `int` shows as a
zero-extension in ours.

Tried without result: `ResetGizAIMessageSys` (constant CSE of zero),
`CharConfig_ConfigureAll` and `DrawPanel` (block placement in large functions;
the sibling versions were rewritten), `instNuGCutSceneServiceLoad` (retail
and sibling reload the instance pointer at every use), `StartDoorPositions`
(data layout), `NuASin` (the sibling uses a sign function), `NuMtxMulnVU0`
(retail does not tail-call). Copying the shared impact block into each
`CastPart` branch and casting the stuck state in `PartImpact_ThermalDetonator`
scored higher but added placeholder-field and magic-flag findings, so both were
left out.

`--all --names` aligns the call and global-access lists of every function
with a counterpart (exact functions included) and keeps placeholder names
whose aligned sibling name is unique and not already one of ours.
`config/sibling-names.csv` lists them for a later rename batch: `high` when the
pair appears in two functions or the names share a word, `low` otherwise
(alignment can shift by one in long global lists, so check `low` rows by
hand).
