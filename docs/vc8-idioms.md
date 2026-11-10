# VC8 source idioms behind near-exact functions

Notes from a pilot over 58 near-exact functions (100 B to 430 B; diff classes
registers, stack, operand-shape, reordered and structural with at most 3 hunks).
Each spot was located with `tools/whyline.py`, which names the source lines
behind the differing instructions. Each entry lists the retail shape, the shape
we emitted, the source construct that changes one into the other, an example,
and how many sample functions it closed.

Flags are the project flags (`/O2 /Gy /GS /MT`, VC8). "Ours" is the build
before the fix.

## Explained idioms

### I1: an early return inside a loop duplicates the cleanup tail

```
retail                                   ours
  push  [critical_section]                 jmp   .after_loop
  call  PCThreadCriticalSectionLeave     .after_loop:
  ret                                      push  [critical_section]
                                           call  PCThreadCriticalSectionLeave
```

Source: when the matching element is found, the retail code leaves the
critical section and returns in place (`Leave(); return;`) instead of `break`
to the common leave after the loop. VC8 does not tail-merge the two copies.
Example: `NuTexAnimRemoveList`. Sample: 1.

### I2: the order of local declarations decides register assignment

Our side has the same instructions with two callee-saved registers swapped.
Declaring the locals in a different order (or declaring without an
initialiser and assigning on the next statement) gives VC8's allocator a
different tie-break. Example: `NuFileReldirFix`, `cbmcfgCameraSpeed`
(`edcam_s *cam; cam = gp_cam;`). Sample: 1.

### I3: compare operand order follows the source

```
retail                                   ours
  mov  eax, [esi+0x2c]                     mov  eax, [LEGOCONTEXT_...]
  cmp  eax, [LEGOCONTEXT_...]              cmp  eax, [esi+0x2c]
```

VC8 keeps `a == b` as `cmp a, b` (left operand in the register). Writing
`object->field == GLOBAL` instead of `GLOBAL == object->field` swaps the
loaded and the memory operand. Examples: `SpecialMove_Cancel`,
`edrtlDrawBurnouts`. Sample: 2.

### I4: statement order of independent statements, local init after a guard

VC8 schedules almost in source order. Two stores to unrelated fields, or a
local initialised in its declaration (`GAMEOBJECT *object = Obj;`) where retail
loads it after the guard, show up as `reordered`. Move the statement to where
retail does it (`GAMEOBJECT *object; ... if (...) return; object = Obj;`).
Examples: `StartHold`, `StoreProgressAICharacter`, `AIScriptNameFromIx`,
`cbSetAutoSpeed`. Sample: 4.

### I5: use the global or field, not a cached local

```
retail                                   ours
  mov  eax, [key+4]                        mov  eax, [key+4]
  mov  [debris_keystack], eax              mov  [debris_keystack], eax
  test eax, eax                            mov  esi, eax          ; local kept
  je   ...                                 test esi, esi
```

A local copy of a value that is also stored to a global (or a field read before
a NULL test) gets its own register. Retail tests the global/field itself:
`debris_keystack = key->previous; if (debris_keystack != NULL)` and
`if (scene->camera_system != NULL) { system = ... scene->camera_system + delta; }`.
In the second case retail loads the field into eax before the prologue pushes;
ours loaded it into a callee-saved register after them. Examples:
`RemoveDebrisEffectFromStack`, `CutCameraFixPtrsPc` (also closed
`CutRigidFixPtrsPc` and improved `CutLocatorFixPtrsPc` 82.23 -> 98.43,
`CutCharacterFixPtrsPc` 92.42 -> 98.98). Sample: 2.

### I6: local vector type (NUVEC4 vs NUVEC) changes the frame layout

`stack` diffs where every `[esp+N]` offset of a vector local is off by 4 or a
multiple of 16: the original declared `NUVEC4` (16 bytes, often
`__declspec(align(16))`) where we used `NUVEC` (12 bytes). Examples:
`NuFadeObjAngleTerrain` (`NuFadeObjCreateMtx` locals), `FlightSplineLegacyLengthPc`.
Sample: 2.

### I7: float temporaries in locals instead of chained assignment

`near.x = far.x = expr;` makes VC8 store to one field and reload; retail
computes `x = expr; y = expr;` into locals and multiplies per field. The frame
size and the stack slots differ. Example: `NuCameraCalcRay`. Sample: 1.

### I8: reuse one variable instead of a new temporary

Retail reuses the parameter or one handle variable (`count = 1.0f / count;`,
one `handle` for both tangents) where we declared a fresh local; the extra
local shifts stack slots. Examples: `instNuGCutSceneCalculateAverageCentre`,
`KnotHelper::DistanceToObject`. Sample: 2.

### I10: `x * 2.0f` is not `x + x`

```
retail                                   ours
  fld   st(0)                              fld   st(0)
  fadd  st(0), st(1)  ; from x * 2.0f      fadd  st(1), st(0) ; from x + x
```

VC8 strength-reduces a multiply by 2.0 to an `fadd` whose operand order
differs from a written `x + x`. Example: `CharConfig_CalculateJumpStats`.
Sample: 1.

### I11: field signedness changes the immediate encoding

```
retail                                   ours
  add  word ptr [edi+0x16], -1   (83 /0)   add  word ptr [edi+0x16], 0xffff (66 81 /0)
```

`--` on a `u16` field is emitted with a 16-bit immediate; on an `i16` field
with a sign-extended imm8. The original field was signed. Example:
`NuLstFree` (`NULSTHDR.used_count` u16 -> i16). Sample: 1.

### I12: natural `for` loop with `&array[index]` instead of a hand-rotated do-while

A decompiler-shaped `if (n > 0) { p = Obj; i = 0; do { ...; ++i; ++p; } while (i < n); }`
compiles differently from the original
`for (i = 0; i < HIGHGAMEOBJECT; ++i) { object = &Obj[index]; ... }`; VC8 does
the rotation and strength reduction itself. Example:
`Condition_EmptyTakeOverPc`. Sample: 1.

### I13: explicit `(f64)` casts change negation folding

`(f32)(-(f64)value * 0.25)` emits `fld; fchs; fmul`; the original
`-value * 0.25` lets VC8 fold the sign into the constant
(`fmul [-0.25]`). Example: `ConfigureComplexShadow`. Sample: 1.

### I14: a NULL guard that retail does not have

An extra `test/je` pair and a shifted block layout. Retail trusted the inner
pointer. Remove the guard only when the callers show it cannot be NULL.
Example: `Plugs_AddGizmos`. Sample: 1.

### I15: direct indexed access instead of a pointer temporary

`PULSE_PC *pulse = &system->pulses[i]; pulse->a = ...` vs
`system->pulses[i].a = ...`: different addressing (`[eax+ecx*8+N]` vs a
separate `lea`). Example: `Pulses_Reset`. Sample: 1.

### I16: NuFabs as a masked copy into a second variable

```
retail                                   ours
  fstp  dword ptr [esp+0x10]               fstp  dword ptr [esp+0x10]
  mov   eax, [esp+0x10]                    and   dword ptr [esp+0x10], 0x7fffffff
  and   eax, 0x7fffffff
  mov   [esp+0x14], eax
```

The SDK's `NuFabs` macro copies the masked bits into another float
(`*(i32 *)&out = *(i32 *)&in & 0x7fffffff`). A union with `bits &= mask`
masks in place. A `static inline` helper is not an option in C++ units: it
leaves a standalone COMDAT copy and `check-objects` fails. Example:
`BuildItAxisWobbleFactorPc`. Sample: 1.

### I17: an early-return guard shares one epilogue

```
retail                                   ours
  test  eax, eax                           test  eax, eax
  je    .epilogue                          je    .ret0
  ...                                      ...
.epilogue: pop esi; ret                    pop esi; ret
                                         .ret0: pop esi; ret
```

`if (!x) return; body;` vs `if (x) { body; }` with a second return path.
Writing the guard as an early return lets VC8 jump to the common epilogue.
Example: `Action_CanSeeBehind` and eight siblings in `aisysall.c` (one macro).
Sample: 1 (+8).

### I18: struct assignment instead of word copy loops

```
retail
  mov   ecx, 0x23          ; sizeof(RTL_LIGHT_PC) / 4
  mov   esi, ebx
  lea   edi, [esp+0x10]
  rep movsd
```

Retail copies the 140-byte struct with `rep movsd` three times. Our explicit
copy loops over the struct's words compiled to counted loops instead. Use
`saved = *curr; *curr = undo; ...; *curr = saved;`. Example:
`cbCancelLightProperties`. Sample: 1.

### I19: switch `default:` instead of an initialiser

`u16 priority = 20000; switch (...) { case ...: priority = ...; }` places the
default store before the jump table; retail has a `default:` case block.
Example: `AddVariableShotDebrisEffectMtx3`. Sample: 1.

### I20: if/return instead of a ternary or a boolean expression

```
retail                                   ours
  cmp  ...                                 cmp  ...
  je   .zero                               sbb  eax, eax
  mov  eax, 2                              neg  eax ...
  ret
.zero: xor eax, eax; ret
```

A ternary or `return a <= b;` lets VC8 go branchless (`sbb/neg/and`, `setcc`)
and widen to `eax`; retail had `if (...) return 2; return 0;`. In an `i16`
function the constants then become `mov ax, 1` / `xor ax, ax` like retail.
Examples: `NuStrToL`, `Collection_Got`, `PortalPointInRoomPc` (partly).
Sample: 3.

### I21: if/else assignment instead of init plus conditional update

```
retail                                   ours
  je    .else                              mov  edi, 0x45
  call  NuStrLen                           je   .skip
  lea   edi, [eax+0x45]                    call NuStrLen
  jmp   .join                              add  edi, eax
.else: mov edi, 0x45
```

`u32 n = K; if (s) n += strlen(s);` vs the original
`if (s) n = strlen(s) + K; else n = K;`. Example: `eduiMenuCreate`. Sample: 1.

## Unexplained (no natural source change found)

Patterns where only an unnatural change (dummy locals, reordered unrelated
declarations without effect, volatile) might help, or nothing did. These stay
as recorded differences.

- U1 SIB base/index order: retail `[ebx+eax]`, ours `[eax+ebx]` with the same
  registers. `Customiser_FindPieceByName`.
- U2 a copy-propagated equal value stored from a different register.
  `ResetStreaks`.
- U3 register choice with identical instruction shape (sometimes a different
  encoding size, so short/near branches and alignment change). Declaration
  order, statement order and permute.py did not move it. `ForceGroupPopPc`,
  `TextureAnimationCrossReferencePrivatePc`, `NuTexAnimEnvCreate`,
  `Arcade_CoinCollected`, `antinodeEditor_UpdateAntiNodesOnPlatforms`,
  `NuPadRecordSave`.
- U4 store sunk past a load that may alias. `MaulF_Init`.
- U5 register-parameter assignment of a static callee. `GetBladeJoints`.
- U6 temporary register for an outgoing argument. `GoingForwardsAlongNarrowSock`.
- U7 which temporary is packed into a dead parameter slot / stack-slot
  assignment. `SplinePointAngles`, `NuRndrLine2di`, `FindIncomingBolt`,
  `Lever_FindNearest`, `AiEditorRespawnMinCountPc`.
- U8 an immediate replaced by a register known to hold the value:
  `or eax, eax` after `memset(..., 0xff, ...)` instead of `or eax, -1`,
  `lea ebp, [edi+1]`, `lea edx, [ecx+0xe]`. Micro-tests with many memset and
  flag variants never reproduced it. `GizForces_ClearProgress`,
  `DrawShopPrompts`, `LoadAnimFromPAK`, `Ledges_StoreProgress`,
  `RecomputeHighGameObject`.
- U9 operand order of a commutative `fadd`/`fmul` not controlled by source
  order. `EduiExpanderOverButtonPc`, `NuMtxOrth`, `MiniTrooperMovePc`.
- U10 argument evaluation order. `GizObstacle_PlayForwards`.
- U11 scheduling of a global store. `InitStillRender`.
- U12 a call to an empty function in another unit kept although the caller
  relies on eax surviving it; moving the callee into the unit inlines it away
  (and breaks ten exact functions). `NuFParResume`.
- U13 hoisting of a return constant / epilogue sharing. `Action_ReleaseLocator`.
- U14 jump threading of a `break` into the post-loop test: retail re-tests
  `distance <= 0` after the loop, ours jumps straight to `return 0` and needs
  two `fstp st(0)`. `PortalPointInRoomPc` (95.73).
- U15 placement of a tail-merged `return 5` block shared by two switch cases;
  no case order or switch/if form reproduced retail's block order.
  `TextureAnimationReadComparisonPrivatePc`.

## Repository-wide estimates

Signature counts over every `different` function with one definition
(`build/tools-local/idiomscan.py`: retail vs relocated rebuilt bytes,
disassembled with the diffclass machinery). A signature says the idiom *may* be
present; it does not prove the cause.

| signature | idiom | functions | bytes | structural, <= 3 hunks |
|---|---|---:|---:|---:|
| more `ret` on our side than retail | I17 | 171 | 83993 | 32 |
| `sbb` on our side not in retail (branchless) | I20 | 69 | 85509 | 10 |
| one-instruction `cmp` spot with memory operand moved | I3 | 53 | 58354 | 1 |
| `or r, -1` vs retail `or r, r` | U8 | 16 | 7898 | 1 |
| `fadd st(0), ...` present on one side only | I10 | 14 | 13802 | 2 |
| `and dword [m], 0x7fffffff` in place | I16 | 12 | 10896 | 2 |
| retail `rep movsd` missing on our side | I18 | 8 | 1072 | 3 |
| `add word [m], 0xffff` vs retail `-1` | I11 | 2 | 3112 | 0 |

I17 and I20 need a control-flow rewrite of each function. They are recognised
mechanically but not fixed mechanically. I3 was applied mechanically
(`build/tools-local/fix_i3.py`): for each hit, the operands of `==`/`!=` on the
source line behind the `cmp` spot are swapped one comparison at a time. A swap
is kept only when the function improves and no function in the unit gets worse.

Result of the mechanical I3 pass: 53 candidates, 52 swaps kept in 41
functions (the commit message for this pass says 37 and "26 more"; the right
numbers are 41 and 30). Two became exact (`CalculateIntersection`
99.84 -> exact, `NuMusic::Voice::Load` 99.09 -> exact). The rest improved by
0.01 to 0.7 points, and no function got worse. Because an I3 spot is rarely the only
difference, the pass mostly helps later work on these functions rather than
closing them.
