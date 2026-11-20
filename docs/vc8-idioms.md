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
`RemoveDebrisEffectFromStack`, `CutCameraFixPtrs` (also closed
`CutRigidFixPtrs` and improved `CutLocatorFixPtrs` 82.23 -> 98.43,
`CutCharacterFixPtrs` 92.42 -> 98.98). Sample: 2.

### I6: local vector type (NUVEC4 vs NUVEC) changes the frame layout

`stack` diffs where every `[esp+N]` offset of a vector local is off by 4 or a
multiple of 16: the original declared `NUVEC4` (16 bytes, often
`__declspec(align(16))`) where we used `NUVEC` (12 bytes). Examples:
`NuFadeObjAngleTerrain` (`NuFadeObjCreateMtx` locals), `FlightSplineLegacyLength`.
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
`Condition_EmptyTakeOver`. Sample: 1.

### I13: explicit `(f64)` casts change negation folding

`(f32)(-(f64)value * 0.25)` emits `fld; fchs; fmul`; the original
`-value * 0.25` lets VC8 fold the sign into the constant
(`fmul [-0.25]`). Example: `ConfigureComplexShadow`. Sample: 1.

### I14: a NULL guard that retail does not have

An extra `test/je` pair and a shifted block layout. Retail trusted the inner
pointer. Remove the guard only when the callers show it cannot be NULL.
Example: `Plugs_AddGizmos`. Sample: 1.

### I15: direct indexed access instead of a pointer temporary

`PULSE *pulse = &system->pulses[i]; pulse->a = ...` vs
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
`BuildItAxisWobbleFactor`. Sample: 1.

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
  mov   ecx, 0x23          ; sizeof(RTL_LIGHT) / 4
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
Examples: `NuStrToL`, `Collection_Got`, `PortalPointInRoom` (partly).
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
Example: `Ledge_Reset` (96.85 -> exact).

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
`PcInputAspectRatio`.

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
  jmp  PostEffectBarrier
```

A call to a small function (even an empty one) that retail keeps, where ours
inlines it, means the caller's unit only had a declaration: the caller lives
in another unit than the callee. The neighbours of the caller in retail
address order name that unit. Examples: `PCDevicePostEffectBarrier` (to
`nuwin32/d3dApiCalls.cpp`), `ShaderCompilerBufferAllocator::operator<<` (to
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
decrement before the call in `NuMtxMulArrayVU0`.

A switch case that returns (`case 0: return;`) where the others break gives
the `sub eax, 0; je` step of I30 even without other code in the case
(`EdFileFillBuffer`).

### I35: empty hooks defined in the caller's unit lose their calls

```
retail                                   ours (hook defined in the same file)
  test eax, eax                            test eax, eax
  jge  .ok                                 ; test and call removed
  push eax
  call PCSoundFailure
```

VC8 sees an empty function defined anywhere in the same translation unit
(before or after the caller, C and C++ alike) and drops calls to it, often
together with the test that guarded the call. Retail keeps the calls, so the
hook lived in another unit: move the definition to the file of the unit
retail placed it in (the unit's first or last function in address order).
Examples: `PCSoundFailure` (to `nusound.c`), `MoviesListNoOp` (to
`movies.cpp`), `FileDirectoryError` (next to `bgProcDataOversize` in
`nuvideo.c`), `SaveCreateDirectory` (to `nufile_pc.c`). A C++ varargs hook
(`PCSpecialEmptyHandler(...)`) only loses argument-less calls that
come before its definition; defining it after the last such caller kept
`NuSpecialGetFirst`'s call and `NuSpecialList`'s register use (retail knew
the hook preserves `eax`/`ecx`/`edx`, so it lives in the same unit).
The same holds for x87: `StateAnimEvaluate` keeps the frame value on the x87
stack and `edx` live across its argument-less call, so it is defined in
`nu3d_includes.cpp` before the hook's definition (the sibling build also
files it there); in its own unit it reloaded both (46.6 -> 50.2).

### I36: a local object pointer instead of repeated casts

`return ((TUBE *)gizmo->object)->bits.visible && ((TUBE *)gizmo->object)->bits.active;`
loads the object into a fresh register (`mov ecx, [eax]`); with
`TUBE *tube = (TUBE *)gizmo->object;` VC8 reuses it (`mov eax, [eax]`). The
same holds when the source copies a field into a local first
(`u32 flags = panel->flags;`). Examples: `Tube_GetOutput`,
`GizPanel_GetOutput`, `Plug_GetOutput`.

### I37: return width

`xor eax, eax`/`mov eax, 1` in the callee with `test al, al` in a caller of
the same unit is a `bool` return in C++ (`MenuCanChangeShadows`); `xor al, al`
where retail has `xor eax, eax` and no caller narrows the value is an `int`
return (`HasAnisoFlip`, `HasPhongLighting`, which also changes the mangled
name). A `char` return of a `u8` field needs `if`/`return` rather than a
ternary, or the ternary promotes and gives `movzx` (`NuKeyToAscii`). Float
pushes of `fldz; fst [esp+4]; fstp [esp]` mean both arguments are `f32`:
`NuSound3AddRumble`'s fourth parameter was `f32`, which made the seven rumble
helpers exact.

### I38: initialised declaration versus a later assignment

Retail `mov ecx, [esp+4]; mov ecx, [ecx+X]; xor eax, eax` where ours has
`mov eax, [esp+4]; mov ecx, [eax+X]; xor eax, eax`: the source declares the
pointer first and assigns it after the other initialisers
(`u32 total = 0; ... system = world->giz_force_sys;`), not as an initialised
declaration ahead of `total = 0`. Examples: the four `*_TotalScore` gizmo
helpers (`GizForce`, `GizObstacles`, `GizTurrets`, `GizBuildIts`).

### I39: D3D, DirectInput, DirectSound and XInput calls go through thunks

Retail calls these APIs with `call rel32` into the linker's `jmp [__imp_]`
thunks, not `call [__imp_]`: the SDK headers of that era declare them without
`dllimport`. Our declarations in `externals.h` must match, or every caller is
one instruction off (`PCDeviceCreateTextureFromMemory` and three siblings).

### I40: AI action prologues

The AI script actions share a prologue shape that the register allocator
exposes. Retail clears the object before the `first_time` guard
(`object = NULL; if (!first_time) return 1;`: `xor edi, edi` ahead of the
guard), tests the argument count before a parameter loop
(`if (count != 0) for (...)`: a `test eax, eax; je` ahead of the usual
`jle`), and reads `param_5` only after the object guard when the object is
loaded first. Exact by these: `Action_ResetContext`, `Action_CanBeCarried`,
`Action_AlertCreatures`, `Action_TagCharacter`,
`Action_RegisterTakeOverObject`, `Action_SetControlSystem`.

### I41: a named local for a value read before a test or a call

Register choice (U3) often follows the expression tree: a field or global read
directly inside a compare or an argument list gets a different register than
the same value first held in a local. Retail sometimes has the local
(`GIZFORCE *top = ...->forces[count - 1]; GizForce_GetHeight(top);` in
`ForceGroupPop`, `u16 heading` and `i32 context` in
`GoingForwardsAlongNarrowSock`, `f32 length_squared` in
`edbriDetermineNearest`, `f32 timer = detonator->timer` re-read after a call in
`Detonators_Update`), and sometimes not (`CalcSplinePointFromDist` reads
`cumulative_distances[index]` twice instead of keeping `f32 end`). Try both
forms before recording U3. A copy of a *parameter* into a local also moves
registers but is not accepted as natural source (see U20).

### I42: operand order of an integer compare between two variables

`a != b` and `b != a` emit `cmp` with the operands swapped, which changes the
register that holds the loaded global. `ObjLandReady` needs
`context != ZipUpLandCombatRollContext` and `context == ZipUpWallShuffleContext`
(local first; a constant still goes on the right).

### I43: order of initialised declarations

Unlike plain declarations (no effect, see the U3 investigation), initialised
declarations are statements and their order sets the load order:
`Lever_FindNearest` initialises `lever = world->levers` before
`nearest_distance = 1.0e9f`, which moves the stack-slot packing (U7).
More: `RenderPostViewportReset` (`height` before `width`: the `fild` order),
`CutSceneRebindPlayerCharactersPrivate` and `CutSceneEndCharactersPC`
(two element pointers `&a[j]`, `&b[j]`: their order picks the registers of
the strength-reduced pointers).

### I44: post-increment index store

`token[n++] = ']'; token[n] = '\0';` is not the same code as
`token[n] = ']'; token[n + 1] = '\0';`: the second form keeps `n` and
addresses `[n+1]` (an extra `lea`). `Hint_ExpandButtons`.

### I45: a cast local of a parameter versus the cast at each use

The reverse of I36 for parameters: `T *view = (T *)param;` and
`((T *)param)->field` at each use give different register choice (U3) or SIB
base/index order (U1), even when the local is never reassigned. A cast used
once reads naturally inline (`DisplayListCreateSkinTransformPS`:
`((PC_SPECIAL_DEFORMER_WEIGHTS *)weights)->weights[morph]`); a cast repeated
at several uses is committed as a `MATCH-EXCEPTION` (`DestroyRippleMtls`,
`Customiser_Set100PercentPieces`, `Animate_BEAST`). A scripted pass over the
63 near-exact functions with such a local made 3 exact; `tools/mutate.py`
has it as the `inline_cast` rule.

### I46: a second `if` after the block instead of `else if`

```
retail                                   ours (else if)
  fcom  [message_delay]                    fcom  [message_delay]
  jp    .busy_or_result                    jnp   .busy
  ...busy block, falls into ret            fcomp [result_delay] ; result test first
.busy_or_result:                           ...
  fcomp [result_delay]
```

`if (busy || delay > 0.0f) { result_delay = 1.5f; ... } else if (result_delay
<= 0.0f) BackupMenu();` places the `else` block first. The original tests the
result delay in a separate `if` after the busy block; VC8 knows the stored
1.5 and skips the second test on the busy path, so the logic is unchanged.
The sibling build (see docs/reference-symbols.md) shows it directly: its
busy path falls into the shared `result_delay <= 0` compare. Made
`MenuUpdateLoading`, `MenuUpdateSaving`, `MenuUpdateDeleting` and
`MenuUpdateFormatting` exact (U19).

### I47: `va_arg` instead of a walked parameter pointer

A variadic function written as `i32 *argument = &first; ... *++argument`
takes the address of the named parameter and keeps the pointer in a
register; `va_list args; va_start(args, first); ... va_arg(args, T)` keeps
the cursor in a stack slot and changes the loop's register use.
`bgPostRequestV` (84.5 -> 88.9), `NuFmvPlayV` (86.0 -> 87.1). Read a float
argument with `va_arg(args, f32)` where retail reads 4 bytes.

### I48: an unreferenced standalone copy is a helper the unit called

VC8 emits an inline function (an in-class member, a template member, a
`static inline` helper) as its own COMDAT in every unit whose source calls
it, even when every call is inlined, and the retail link keeps the copy
(`/opt:noref`). A function nothing references is not emitted, whether it is
`static` or inline. So a short unreferenced `fun_` in retail is usually an
accessor or constructor the original source called somewhere in that unit:
`mov eax, [ecx]; ret` and `mov eax, [ecx+4]; ret` after a
`CSListLink<T>` constructor are `T::Next()` and `T::Previous()` walking that
list, a 13-byte `first = last = count = 0` is the list's constructor. Writing
the walk with the accessor (`menu = menu->Next()`) emits the copy without
changing the inlined callers. An empty `ret` heading a unit (`NuError`) or a
named debug routine with external linkage is emitted unreferenced; a
`static` copy (`XxxVuVecMake`, `XxxFabs`) needs a real call in the unit.
Before writing one, check whether our build already emits it under a name
symbols.csv lacks (the map's unmapped symbols with byte-identical retail
twins): `??0ReplicatorList@NetworkObjectManager@@QAE@XZ` and
`?Self@ClassObject@@QAEAAU1@XZ` only needed their row.

### I49: `fabs` on a float in a C++ unit is the float overload

`<math.h>` under C++ adds `float fabs(float)` and `fabsf`, both inline
functions: a C++ unit that calls either on an `f32` emits a COMDAT
(`?fabs@@YAMM@Z`, `_fabsf`) that the map does not know, even when the call
itself is inlined. Retail's uncalled NuFabs copies (`fld; fabs; fstp; fld`)
come from a C unit, where `fabs` is the double intrinsic and the implicit
narrowing back to `f32` needs no cast. `PCNativeDebugFabs`,
`PCNativeDebugNuFabs` (0 -> exact).

### I50: an inline default constructor of a local is emitted as a COMDAT

Giving a C++ struct an empty `D3DXMATRIX() {}` so that a local can be
declared compiles the caller exact but leaves `??0D3DXMATRIX@@QAE@XZ` in the
object. Write such a function in C, where the SDK names are plain structs
(`struct _D3DMATRIX`, `struct _D3DVECTOR` behind `D3DXMATRIX`/`D3DXVECTOR3`
in `externals.h`). `NuCameraLookAt` (0 -> exact, `nucamera.c`).

### I51: `add esp, 4` after a final call returns its result

After the last call of a function, VC8 drops a single pushed argument with
`pop ecx` when the call's result is discarded and with `add esp, 4` when the
function returns it. `LightingSetAmbientLight` and `PartsSetAmbientLight`
(`return NuRndrSetAmbientLightPS(ambient);`, 94 -> 100).

### I52: nested guard blocks are inlined guarded wrappers

`if (g == 0) { A(); if (g == 0) { B(); if (g == 0) { C(); } } }` is what
three calls of small `static void X(...) { if (g == 0) Y(...); }` wrappers
look like once VC8 inlines them: after each call the global is tested again,
and when a test fails the later ones are skipped. Retail keeps every such
wrapper as a standalone copy (I48), so write the calls: the edui renderers
call `EduiRndrRect2di`, `EduiQFntSet`, `EduiQFntSetColour`, ... (nine
wrappers exact, every caller unchanged). Blocks that compute their arguments
into braced locals first (`{ f32 x0 = ...; if (g == 0) NuRndrLine2di((i32)(x0
* EduiScaleX), ...); }`) are the same for a wrapper taking floats
(`EduiDrawLine`, `EduiDrawLineRect`).

### I53: placement new from a pool is an inlined class `operator new`

`new (theMemoryManager.AllocPool(sizeof(X), 1)) X(...)` compiles like
`new X(...)` with `static void *operator new(size_t size) { return
theMemoryManager.AllocPool(size, 1); }` in X, but only the latter emits the
`??2X@@SAPAXI@Z` copy retail keeps (18 bytes, seven of them in the RTL helper
unit; classes derived from `EdRef` use its copy).

### I54: a helper's `switch` keeps the case order of its source

An emitted copy of a static helper with a `switch` (the display list item
`SetOp` in both 3D units) lays out the case blocks in source order and the
jump table points into them: retail's targets for ids 0, 1, 3, 4 sit in the
order call (3), skip (0), jump (1), end (4), so the source lists `case CALL`
first. Ours with the cases in numeric order compiled the same instructions
in another order (90 % instead of exact). Callers that pass a constant id
fold the switch away, so only the copy shows it. A call through such a
helper can also settle a caller's scheduling: `NuDisplayListCreateMtlDlist`
became exact once its first and last items were written with the `Init`
helper (type, next, op) instead of three stores.

### I55: a static helper's parameter order picks its argument registers

VC8 passes the arguments of a static function it calls directly in
registers of its own choosing (U5), and the order the parameters are declared
in decides which argument gets which register: `SockAngleDifference(u16 to,
u16 from)` zero-extended `ax` before `cx`, retail does `cx` first; declaring
`(u16 from, u16 to)` and swapping the six call sites made the helper exact
with both callers unchanged.

### I56: `memset` of a one-word array keeps its store in place

```
retail                                   ours (progress->pulled_down[0] = 0;)
  or   eax, -1                             or   eax, -1
  mov  dword ptr [ebx], 0                  mov  [ebx+4], eax
  mov  [ebx+4], eax                        ...
  ...                                      test eax, eax
  test eax, eax                            mov  dword ptr [ebx], 0   ; sunk
```

VC8 expands `memset(a, 0, sizeof(a))` of a 4-byte array into the same
`mov dword ptr [m], 0`, but the scheduler leaves an expanded `memset` store
where it is, while a plain element store may sink below later loads. The
gizmo progress records clear their one-word bit arrays with `memset`, like the
exact `Shards_ClearProgress`. Examples: `Levers_StoreProgress`,
`GizPanels_StoreProgress` (now calling `GizPanels_ClearProgress`),
`Attractos_StoreProgress` (all exact).

### I57: the SDK `NuFabs` form of a unit's float helper

A unit's `static f32 XxxFabs(f32 f)` is the SDK shape
`f32 rv; *(i32 *)&rv = *(i32 *)&f & 0x7fffffff; return rv;`. A
`i32 bits = ...; bits &= mask; return *(f32 *)&bits;` body compiles to the
same standalone copy, but where a caller inlines it the two forms give the
temporaries different stack slots. `CutScene_PreUpdateFn_LSW` became exact
with the SDK form and the call written inside the expression that uses it.

### I58: which identical return tail a guard jumps to

```
retail                                   ours
  test eax, eax                            test eax, eax
  je   .tail_a       ; first ret block     je   .tail_b       ; a later ret block
```

When a function has several byte-identical `return` tails (same value,
same epilogue), a guard's jump target only shows which one VC8 merged it into,
and that follows the shape of the source around the returns:
- a fixed-count search loop written as `i = 0; do { ... } while (i < 8);`
  instead of `for` (`edbriLoadPage`: the open/version guards then share the
  loop's not-found tail);
- one `return 0` at the end of an `if (first_time) { ... } else if (...)`
  chain instead of a `return 0` in each branch (`Action_Idle`);
- a search written as a static helper returning the element or NULL makes
  `FogHelper::CreateObject` exact (its `if (fog != NULL)` then jumps to the
  final `ret`), but the helper's emitted copy is not in retail, so that unit
  keeps its `goto`.

The other way round, an early `return` placed in a branch that also falls to
the end keeps its own tail. Only the branch target differs (diff class
`operands`, 0 hunks).
### I59: a vector local keeps its own stack slots

Scalar `f32` locals of disjoint scopes share stack slots; the members of a
struct local do not. A frame that is 12 bytes larger than ours, with three
temporaries stored to slots no other value uses, is a `NUVEC` the original
filled field by field: `NuRainProcess` stored the camera delta in
`NUVEC delta` (`delta.x = ...; particle->position.x += delta.x;`), not in
`f32 dx, dy, dz` (99.95 -> exact).

### I60: an inlined call of a counting helper instead of the loop written out

`while (path[path_len] != '\0') ++path_len;` counts in the parameter's
register; `path_len = NuStrLen(path);` (the unit's own `NuStrLen`, inlined)
counts in a fresh register and copies the result back (`mov edi, eax`).
A copy of the count after the loop means a helper was called.
`NuFileExtGetInfoPC` (78 -> exact with a `path_end` local for `path +
path_len` computed before the loop, I41), and with it the three callers
`NuFileExtGetType`, `NuFileExtConvert`, `NuFileExtRemove`.

### I61: the terminator at the loop index

After `for (i = 0; i < info->len; ++i) dest[i] = ...;`, retail writes
`dest[i] = '\0'` (the index register), not `dest[info->len] = '\0'` (a
reload of the field). `NuFileExtGetExt` (also I3: `dest_size < info->len`).

### I62: a clamp written as two `if`s, not `if`/`else if`

```
retail                                   ours (else if)
  jmp  [table]                             jmp  [table]
case START:                              ; case START jumps into the clamp's
  fst  [title_alpha]                     ; `title_alpha = 0` store
  jmp  .clamp
```

With `if (a < 0) a = 0; else if (a > 1) a = 1;` after a `switch`, VC8 sends
a case that stores the same constant (`title_alpha = 0.0f; break;`) straight
into the clamp's store. Two separate tests (`if (a < 0) a = 0; if (a > 1)
a = 1;`) keep the case block of its own like retail. `MiniKit_LSW_Draw`
(99.07 -> exact); the sibling status draws did not change.

### I63: the first field a loop reads sets the base of its strength-reduced pointer

```
retail                                   ours (u32 pressed = pad->buttons_pressed;)
  mov  edi, &GamePadStorage[0].buttons_pressed   mov edi, &GamePadStorage[0].left_pressed
  ...  [edi], [edi+8]                            ...  [edi-8], [edi]
  cmp  edi, &GamePadStorage[2].buttons_pressed   cmp edi, &GamePadStorage[2].left_pressed
```

When a loop walks an array of records, VC8 keeps one pointer to a field of
the current record and addresses the others relative to it. Which field it
picks follows how the source reads them: caching one field in a local
(`pressed = pad->buttons_pressed`) leaves the other field as the most-read
one, so the base moves and every displacement and the loop bound change.
Retail read the fields through the record at each test
(`(pad->buttons_pressed & X) != 0`), or read the field it is based on first.
Examples: `MenuUpdateMissions` (96.5 -> 97.8), `UpdateStatusScreen`
(90.5 -> 96.4), `AITriggerSetSysProcess` (route mask read before the locator
pointer is taken, 99.09 -> 99.30).

### I64: conversion targets are visible in the code

- Float to `u16`/`i16` converts inline (`fnstcw`, `fistp dword`, `movzx r, word`);
  float to `i32` calls `_ftol2_sse`. A retail inline conversion where we call
  `_ftol2` means the original cast straight to the 16-bit type, not
  `(u16)(i32)x` or an `i32` local truncated later (`LoadAreaCharacters` prompt
  phase, `Minicam_Update` change-mode pitch).
- `u32` to float adds the 2^32 correction from `__real@4f800000` (f32) when the
  target is `f32` and from `__real@41f0000000000000` (f64) when it is `f64`.
  Retail's f32 constant means `u * f` with an `f32` operand, not `(f64)u * f`
  (`Coins_LSW_Update`, `DisplayCutSceneMemory`).
- `(u32)(i32)x << 24` converts through a 64-bit `fistp qword`; retail's
  `_ftol2` and a signed shift is `(i32)x << 24` (`LoadPerm`).
- `tools/refscan.py` lists functions whose two sides use different conversion
  constants.

### I65: an unreferenced static copy names the helper its unit wrote out

A not-implemented function nothing calls, with register arguments (`eax`,
`ecx`, `esi` read before any stack load), is the standalone copy of a
`static` helper whose every call was inlined (I48). Its body tells which
helper; the unit's source usually still has that body written out at each
use. Compare it with the mapped functions of other units (relocations
masked): a twin such as `CreatureMtxSetRotationXY` or `DebrisMtxCopyPrivate`
names the helper, then replace the written-out copies with calls:
`SnakeMtxSetRotationXY` (also made `DrawSnakeBody` and `SnakeBeenHit`
exact), `TorpedoMtxCopy`/`TorpedoMtxSetIdentity`, `VuMtxSetIdentity`,
`AiEditorSetCameraPos` (`pathEditor_Process` 99.1 -> 99.9),
`AiEditorLocatorInSet`, `GuideLine_Init`, `EduiTextPickCursorLeft`. The
same holds for inline accessors (`ReplicatorList::First`,
`V2Peer::GetStats`). When the written-out form compiles differently from
the copy (the copy stores `z, y, x`, the sites `x, y, z`), the sites did
not use that helper: leave it unwritten.

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
  order, statement order and permute.py did not move it (but see I41).
  `TextureAnimationCrossReferencePrivate`, `NuTexAnimEnvCreate`,
  `Arcade_CoinCollected`, `antinodeEditor_UpdateAntiNodesOnPlatforms`,
  `NuPadRecordSave`.
- U4 store sunk past a load that may alias. `MaulF_Init`.
- U5 register-parameter assignment of a static callee. `GetBladeJoints`
  (the parameter order can decide it: I55).
- U6 temporary register for an outgoing argument (`GoingForwardsAlongNarrowSock`
  closed with I41).
- U7 which temporary is packed into a dead parameter slot / stack-slot
  assignment. `SplinePointAngles`, `NuRndrLine2di`, `FindIncomingBolt`,
  `AiEditorRespawnMinCount` (`Lever_FindNearest` closed with I43).
- U8 an immediate replaced by a register known to hold the value:
  `or eax, eax` after `memset(..., 0xff, ...)` instead of `or eax, -1`,
  `lea ebp, [edi+1]`, `lea edx, [ecx+0xe]`. Micro-tests with many memset and
  flag variants never reproduced it. `GizForces_ClearProgress`,
  `DrawShopPrompts`, `LoadAnimFromPAK`, `Ledges_StoreProgress`,
  `RecomputeHighGameObject`.
- U9 operand order of a commutative `fadd`/`fmul` not controlled by source
  order. `EduiExpanderOverButton`, `MiniTrooperMove` (`NuMtxOrth`, once
  listed here, closed with I31 parentheses).
- U10 argument evaluation order. `GizObstacle_PlayForwards`.
- U11 scheduling of a global store. `InitStillRender`.
- U12 a call to an empty function in another unit kept although the caller
  relies on eax surviving it; moving the callee into the unit inlines it away
  (and breaks ten exact functions). `NuFParResume`.
- U13 hoisting of a return constant / epilogue sharing. `Action_ReleaseLocator`.
- U14 jump threading of a `break` into the post-loop test: retail re-tests
  `distance <= 0` after the loop, ours jumps straight to `return 0` and needs
  two `fstp st(0)`. `PortalPointInRoom` (95.73).
- U15 placement of a tail-merged `return 5` block shared by two switch cases;
  no case order or switch/if form reproduced retail's block order.
  `TextureAnimationReadComparisonPrivate`.
- U16 x87 stack position of a dead value: retail pops with `fstp st(0)`, ours
  with `fstp st(1)` (or the reverse). `LevelComplete_LSW_Update`.
- U17 a loop over a known-range `u32` compiled to a signed compare (`jl`) in
  ours and `jb` in retail; signedness of the counter and the bound did not
  move it. `SfxBuildCRCTable`.
- U18 the last `if (c) return 1; return 0;` pair: ours is if-converted to
  `neg/sbb/neg` (or `sbb/and`), retail branches to an earlier shared
  `return 1`. Separate guards, `||`, `!=`/`==` spellings and a result variable
  did not change it. `Text_IsOtherPlatformWord`, `CanWearHatsInFreePlay`,
  `PcInputSetDeadzone`.
- U19 the `else` block of a float `||` test placed before the `then` block
  (or the reverse); every if/else/return arrangement compiled the same.
  Closed for the memory card menus by I46 (`MenuUpdateLoading`,
  `MenuUpdateSaving`, `MenuUpdateDeleting`, `MenuUpdateFormatting`);
  `MenuUpdateExtras` not yet tried.

- U20 exact only with an unnatural change found by `tools/permute2.py`
  (recorded, not committed): a copy of a parameter into a local
  (`Customiser_FindPieceByName` `index`, `NuRndrLine2di` `colour`,
  `EduiExpanderOverButton` `expander` plus mirrored compares,
  `StatusPacketReset` `packet` plus a different store order,
  `DisplayListCreateSkinTransformPS` `skin` plus a `while` loop,
  `GetBladeJoints` several copies and narrowed temporaries); the cast repeated
  at each use instead of one cast pointer plus two temporaries
  (`DestroyRippleMtls`); a duplicated idempotent store plus parameter copies
  (`PushSeekComplete`). Patches are in `build/permute2/` of the trial.

- U21 a range test `x >= 0 && x < N` on a small signed value: ours is folded
  into one unsigned compare (`cmp al, N-1; ja`), retail keeps the two signed
  tests (`test al, al; jl` then `cmp al, N; jge`). `> -1`, `<= N-1`, the
  operand type and the test order did not change it. `TrafficAnimSys_Configure`
  (room id), `PartImpact_ThermalDetonator` (surface type).

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
with identical code has no natural source lever. The permuter trial below
adds named locals for loaded values (I41) and compare operand order (I42).

### Permuter trial (tools/permute2.py)

60 U-tagged or untagged near-exact functions (80-756 B, 90-99.98%) were
searched for 10 minutes each with random stacks of the AST mutations in
`tools/mutate.py` (natural mutations only), compiled as verified reduced
units. 16 became exact (27%); 8 of them have a natural form, found by
re-minimising the patch and trying the obvious spellings by hand (I41-I44).
The other 8 need a parameter copy, a repeated cast or a duplicated store
(U20). By difference type: register choice 5 of 24 exact (3 natural), stack
slot 3 of 10 (1), x87 order 2 of 4 (1), SIB order 3 of 3 (0), untagged 3 of
9 (3), immediate replaced by a known register (U8) 0 of 10. Every exact find
came within 153 CPU seconds (median 19), so a 2-3 minute budget per function
loses nothing. U8, U14-U19 (block placement, x87 pops, if-conversion) and
`ResetStreaks` (U2) did not move at all.

Second run (2.5 minutes per function, 2 workers, all tiers, U8/U16/C-only
functions skipped, highest objdiff first): 40 functions searched, 11 exact
finds. Most minimised patches still carried unrelated temporaries; the
committed forms came from trying the obvious spellings of the one remaining
spot by hand: initialised declaration order (I43, 3), statement order (I4),
compare and float operand order (I42, I25), a cast at each use (I45) and
locals for re-read fields (`NuMtxAlignY`, MATCH-EXCEPTION). `GizMiniCut_Load`
and `LineToPlaneIntersecion` stay: only a dozen temporaries reproduce them.
Scripted single-rewrite passes paid more per compile than the random search:
I45 over the 63 near-exact functions with a parameter cast local made 3 exact,
I43 (now `idiomfix.py I43`) over the 157 near-exact functions with adjacent
initialised declarations 4 (one from 61%), and 0 in the first 350 variants of
the 70-90% functions. With `MATCH-EXCEPTION` allowed, five of the U20 patches
of the first trial were committed in a reduced form; `GetBladeJoints`
(narrows a type), `PushSeekComplete` (duplicated store) and
`StatusPacketReset` were not.

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
