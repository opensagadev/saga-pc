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
`NuLstFree` (`NULSTHDR.used_count` u16 -> i16). The same holds for byte
stores of `-1`: retail `or al, -1` (or `mov byte [m], 0xff` through a signed
register) means a signed `char` field; `MAINFRAMECOUNTERS` u8 -> i8 closed
`ResetFrameCounters` and `UpdateFrameCounters`. A signed `i32` local instead
of `u32` closed `CRC16::CRC16`.

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

### I22: one conditional jump with the opposite sense

```
retail                                   ours
  test eax, eax                            test eax, eax
  je   .else                               jne  .then
```

VC8 lays out the `if` body as the fall-through. An inverted jump with the
same blocks swapped means the original wrote the branches the other way round
(`if (!c) B else A`). Signature only (`idiomscan.py`); no sample closed yet.

### I23: comparison spelling (`> 0` vs `!= 0`, `<= N` vs `< N + 1`)

```
retail                                   ours
  cmp  byte ptr [edx+0xf9], cl  ; cl = 0   cmp  byte ptr [edx+0xf9], cl
  jbe  .skip                               je   .skip
```

On an unsigned operand `x > 0` and `x != 0` are the same test, but VC8 keeps
the spelling: `jbe` for `> 0`, `je` for `!= 0`. Likewise `cmp x, 0x10; ja`
is `x > 16`, `cmp x, 0x11; jae` is `x >= 17`. Example:
`Ledges_ReserveBufferSpace` (`level->max_ledges > 0`). Mechanical rule in
`tools/idiomfix.py I23`.

### I24: x87 comparison flags (`test ah, 0x41` vs `test ah, 5`)

The flag mask after `fnstsw ax` encodes which of `<`, `<=`, `>`, `>=` the
source wrote and which operand was loaded first. Signature only.

### I25: float operand order follows the source

```
retail                                   ours
  fld   dword ptr [esi+0x1dc]              fld   dword ptr [esi+0x88]
  fadd  dword ptr [esi+0x88]               fadd  dword ptr [esi+0x1dc]
```

Unlike integer code, VC8 loads the left operand of a float `+`/`*` first and
uses the right one as the memory operand. Swap the operands (`b + a`); IEEE
addition and multiplication are commutative, so behaviour does not change.
Mechanical rule in `tools/idiomfix.py I25`.

### I26: field width (byte vs dword access, bitfield store)

Retail `xor r8, [m]; and r8, 1; xor [m], r8` is a 1-bit bitfield store; a
`mov byte`/`or byte [m+1], 4` where we use `or dword [m], 0x400` means the
original field was a byte or a bitfield of a smaller unit. Signature only.

### I27: store the result inside the search loop before `break`

```
retail                                   ours
  je   .found                              je   .found
  ...                                      ...
.found: mov [esi+0x18], cl                 cmp  ecx, 6
  cmp  ecx, 6                              jne  .else
                                           ...
                                         .else: mov [esi+0x18], cl
```

`for (...) { if (match) break; } if (i == N) {...} else x = i;` vs the
original `for (...) { if (match) { x = i; break; } } if (i == N) {...}`.
Example: `LedgeInitRecordPc` (96.85 -> exact).

### I28: one loop clearing two arrays instead of two `memset`s

Two small `memset`s expand to two runs of `mov [g+4k], eax`; retail interleaves
the stores of both arrays, which is one unrolled loop
`for (i = 0; i < 8; ++i) { a[i] = 0; b[i] = 0; }`. Example: `ParticleReset`
(99.48 -> exact).

### I29: two guards instead of one `||` guard

```
retail                                   ours
  test ecx, ecx                            test ecx, ecx
  jne  .next                               je   .ret0
  xor  eax, eax                            cmp  [ecx], 0
  ret  4                                   je   .ret0
.next: cmp [ecx], 0                        ...
  je   .ret0_above                       .ret0: xor eax, eax; ret 4
```

`if (a == NULL || a->x == 0) return 0;` jumps both tests to one shared
return at the end. Two separate guards (`if (a == NULL) return 0;
if (a->x == 0) return 0;`) keep the first return inline and send the second
test back to it. The same holds for a chain of `return 1` tests
(`CoinsGoToMainTotal`), and the other way round: retail sometimes has one
long `||` where we wrote two `if`s (`Teleport_UpdateHints`,
`ShinyMetal_UpdateHint`). Examples: `SoundQueue::SetStreamFlags`,
`SoundQueue::SetStreamFile`, `Teleport_ActivateRev`, `createGizRandom`.

### I30: `switch` instead of an if chain

Retail `sub eax, 0; je; sub eax, 1; je` is a small `switch` lowered as a
decrement chain; an if chain compiles to `test`/`cmp` pairs. A jump table over
`0..3` whose entry 0 is the `default` block means the source listed
`case 0:` with its own (identical) statements as well as `default:`; a
`case 0: default:` label pair or no `case 0` gives a decrement chain.
Examples: `GizSpecial_GetOutputName`, `GizMiniCut_GetOutputName`,
`PcInputAspectRatioPc`.

### I31: parentheses steer x87 operand order

VC8 reorders the operands of an unparenthesised float `+`/`-`/`*` term
(Sethi-Ullman style), but evaluates a parenthesised sub-expression as written.
`det = (a * A) - (b * B) + (c * C);` and `det = a * A - b * B + c * C;` give
different load orders. `NuMtxInvRSSH` became exact only with the
parenthesised determinant. Redundant parentheses around a cast
(`((f64)x) * ((f64)y)`) also move code, which suggests macros in the original;
those were not kept because nothing in the source would explain them.

### I32: a file-static global lets VC8 move loads above its store

```
retail                                   ours
  mov  eax, [eax+0xc]                      mov  eax, [eax+0xc]
  cmp  dword ptr [eax+0x34], 0             mov  [ed_curr], eax
  mov  [ed_curr], eax                      cmp  dword ptr [eax+0x34], 0
  jne  ...                                 jne  ...
```

`ed_curr = module; if (module->reserved == NULL)`: VC8 schedules the load
through `module` above the store to `ed_curr` only when it can prove the two
do not alias, which it does for a `static` global whose address is never
taken. This port defined most globals in `data.cpp` and declared them in
`saga.h`, so every such store stayed in source order. The same holds for
globals kept in registers across stores through pointers. A search over every
`saga.h` global referenced by one unit only (make the unit's candidates
static, keep a set when functions improve and none gets worse) moved 175
globals into 44 units as `static`: 19 functions became exact
(`cbMMRegSel`, `IconScenes_Dump`, the U3 example above, `MiniKit_LSW_Update`,
`Move_CRITTER`, ...) and 54 improved. Scanner: the `I4` moves of a single
store to an absolute address. Globals a `data.cpp` table takes the address of
(character fixup ids) cannot be static.

### I33: a function compiled in another unit is called, not inlined

```
retail                                   ours
  mov  ecx, offset PCDeviceBackend         ret        ; empty body inlined
  jmp  PostEffectBarrierPc
```

A call to a small function (even an empty one) that retail keeps, where ours
inlines it, means the caller's unit only had a declaration: the caller lives
in another unit than the callee. The neighbours of the caller in retail
address order name that unit. Examples: `PCDevicePostEffectBarrierPc` (to
`d3d_api_lifetime.cpp`), `ShaderCompilerBufferAllocatorPc::operator<<` (to
`shader_dictionary.cpp`), and the atexit entry `??__FPCDeviceBackend`, which
calls the destructor because the global itself was defined next to
`nu_keyboard_owner` in `data.cpp`. `tools/callset.py` lists these as call-set
mismatches.

### I34: indexed loop instead of a pointer walk

`for (record = Records; record->id != -1; ++record)` loads the start address
before the first test; `for (i = 0; Records[i].id != -1; ++i)` (or
`techno = &WORLD->technos[i]` inside the loop) lets VC8 test first and
materialise the pointer in the loop preheader. Examples:
`PCGraphicsPrepareVendor`, `Technos_FindControllingTechno` (both exact).
`while (count-- != 0)` instead of a trailing `--count` likewise moved the
decrement before the call in `NuMtxMulHArrayPc`.

A switch case that returns (`case 0: return;`) where the others break gives
the `sub eax, 0; je` step of I30 even without other code in the case
(`EdFileFillBuffer`).

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
  order. `EduiExpanderOverButtonPc`, `MiniTrooperMovePc` (`NuMtxOrth`, once
  listed here, closed with I31 parentheses).
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
- U16 x87 stack position of a dead value: retail pops with `fstp st(0)`, ours
  with `fstp st(1)` (or the reverse). `LevelComplete_LSW_Update`.
- U17 a loop over a known-range `u32` compiled to a signed compare (`jl`) in
  ours and `jb` in retail; signedness of the counter and the bound did not
  move it. `SfxBuildCRCTable`.
- U18 the last `if (c) return 1; return 0;` pair: ours is if-converted to
  `neg/sbb/neg` (or `sbb/and`), retail branches to an earlier shared
  `return 1`. Separate guards, `||`, `!=`/`==` spellings and a result variable
  did not change it. `Text_IsOtherPlatformWord`, `CanWearHatsInFreePlay`,
  `PcInputSetDeadzonePc`.
- U19 the `else` block of a float `||` test placed before the `then` block
  (or the reverse); every if/else/return arrangement compiled the same.
  `MenuUpdateLoading`, `MenuUpdateSaving`, `MenuUpdateExtras`.

### U3 investigation (register choice)

Micro-tests compiled with the project flags (scratch C, `/O2 /Gy /GS`) and the
real `IconScenes_Dump` (97.50, an `ecx`/`eax` swap) were used to find which
source changes move VC8's register assignment.

No effect: declaration order, block scope of a local, a dead initialiser,
operand order of a commutative integer operator, `for`/`while`/`do` loop form,
reusing one variable across disjoint live ranges, signedness, no-op casts.

Effect: using a parameter directly vs copying it into a local (even a cast
copy such as `S *s = (S *)v;`); the order of uses inside a loop (but that also
reorders instructions); equivalent statement pairs such as `G[i] = 0; s = G;`
vs `s = G; s[i] = 0;`, which changed the interference graph in a micro-test.

On `IconScenes_Dump` the best of these rewrites scored 84.50, so the function
stays at 97.50. Conclusion: U3 is reachable only when the register difference
comes from a copied parameter or from statement order; a pure colouring swap
with identical code has no natural source lever.

## Repository-wide estimates

`tools/idiomscan.py` tags every differing function with the signatures above
(retail vs relocated rebuilt bytes, split into spots by `whyline`) and writes
the tags into `matching.json` as `idioms`. A tag says the idiom *may* be
present; it does not prove the cause. `--list` prints the work queue by tier:
`I` (only idiom tags), `mixed`, `unexplained` (`?` spots left), `U` (only
unexplained-pattern tags). `--summary` gives these counts.

Functions per tag after the mechanical passes and the manual sweep (one
function can carry several tags):

| tag | functions | | tag | functions |
|---|---:|---|---|---:|
| I4 | 2251 | | U3 | 2369 |
| I8 | 515 | | U7 | 1454 |
| I17 | 453 | | U8 | 352 |
| C | 300 | | U9 | 251 |
| I20 | 168 | | U16 | 158 |
| I26 | 130 | | U1 | 59 |
| I25 | 102 | | ? | 2654 |
| I24 | 100 | | | |
| I23 | 86 | | | |
| I5 | 70 | | | |
| I22 | 42 | | | |
| I10 | 26 | | | |
| I18 | 25 | | | |
| I3 | 18 | | | |
| I16 | 12 | | | |
| I11 | 2 | | | |

Near-exact functions (>= 90%) by tier: I 135, mixed 440, unexplained 368,
U 197.

### Mechanical passes

`tools/idiomfix.py IDIOM` rewrites each candidate with libclang, rebuilds the
unit and keeps a rewrite only when the function improves and no function in
the unit gets worse. Its rules produce ordinary source; results that read
unnaturally were dropped by hand before committing.

| idiom | rule | candidates | kept | exact |
|---|---|---:|---:|---:|
| I3 | swap `==`/`!=` operands | 53 | 41 functions | 2 |
| I17 | guard <-> wrapped body, swap guard and final return | 593 | 99 | 28 |
| I17 (second pass) | same, after I23/I25 | | 11 | 2 |
| I20 | ternary/boolean return -> if/return | 177 | 1 | 0 |
| I23 | `> 0` vs `!= 0`, `<= N` vs `< N + 1` | 159 | 79 | 28 |
| I25 | commute float operands | 122 | 13 | 1 |
| I31 | parenthesise float product terms | 68 | 21 | 7 |

I10, I16 and I18 were not run: each has fewer than 30 functions and the
rewrites need type information the rule set does not model (a second
variable for I16, a struct type for I18), so they were left to the manual
sweep. I3 is the earlier pass described here before: for each hit the
operands of `==`/`!=` on the source line behind the `cmp` spot are swapped one
comparison at a time (two functions became exact: `CalculateIntersection`,
`NuMusic::Voice::Load`).

### Manual sweep

Short functions from the `I` and `unexplained` queues were fixed by hand,
one unit at a time, about three tries each. What closed them, most often:
statement and store order (I4), guard versus wrapped body (I17), separate
guards versus one `||` (I29), small `switch`es (I30), field or local
signedness (I11), and comparison operand order (I25). Several "unexplained"
functions were port bugs (wrong field, missing term, a stub returning 0); see
the commit bodies marked `Runtime-relevant`.

Symbol extents: some functions scored low only because `config/symbols.csv`
gave them the wrong size, checked against the retail bytes and the next
symbol. Fourteen were corrected (for example `InitPartTable` 49 -> 50 bytes,
which then matched exactly, and `Levels_FixUp`, which overlapped its
neighbour).
