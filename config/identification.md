# Recovering PC symbols from the Android reference

The useful comparison is between the **original Android ELF** and the **original
PC PE**, with the Android source supplying names and type hypotheses. Compiling
the reconstructed Android game for a host platform would add another compiler
and is unnecessary for this work. The four-column [symbol map](symbols.csv)
contains accepted identities; record uncertain alternatives here before adding
them. Identification and exact reconstruction matching are separate results.

## What is available

The structural progress batch follows the Android world, animation, file and
sound source families while retaining PC layouts and branch behavior. Reviewed
NuMain direct-call inventory additions cover area cleanup/loading, challenge and
player transitions, music and still-screen rendering; their complete single
intervals were decoded through their final instructions before mapping.
Inventory entries alone do not count as reconstructed code.

`LoadPermBuildLegalTextureName` at `0x004c9b20` receives its destination in EDI,
not a normal stack argument. Its C interface is a structural wrapper: the PC
region selects US suffixes, while the generic fallback always uses
`Text_Language`. The 169-byte helper does **not** append `.dds`. Its language-zero
branch changes the two text scales to `2.2f`. The wrapper remains nonmatching;
do not relabel the original private ABI as cdecl evidence.

`NuSpecialGetFirst` at `0x005cfef0` has a complete 96-byte extent and returns an
integer success flag. Its no-specials path optionally calls the one-byte return
at `0x005c1300`. `PCSpecialEmptyHandler_candidate` records that verified no-op
behavior without claiming a historical identity. Legacy and display-list
specials retain distinct PC record strides.

Both originals are little-endian x86 binaries with 32-bit pointers. The Android
`res/libTTapp.so` examined here has SHA-256
`30d3fe55503416139a1d0f9b25fc73924aab4b12dd731aafc08b4a1c9bd950d5`.
Its `.symtab` supplies 13,454 sized function entries (13,043 distinct function
ranges) and 8,913 sized data entries. Aliases explain the function-count difference.
Use its symbol VAs directly; the Android project's existing Ghidra database uses
a separate `+0x10000` rebase that must not leak into ELF-to-file calculations.

The PC image has imports and a PDB reference, but no COFF symbol table, exports,
or exception-directory function table. The embedded PDB path is a reference to
a missing file, not debug symbols. Import-address-table slots have known 4-byte
extents; the imported DLL routines themselves live outside this executable.
See [the external inventory](symbols.md) for their treatment.

MSVC RTTI also preserves 120 type-name strings, including game classes. The type
descriptors, their class-hierarchy metadata and vtable references provide
additional anchors even though ordinary code symbols are stripped. These are
internal data, not imported functions to exclude from reconstruction.

A fresh Ghidra 12.1.2_DEV analysis found **7,839 PC function candidates**, including
library routines and import thunks. **800 have multiple body ranges.** These are
analysis results, not an authoritative function census. It missed the three
unreferenced seed getters below, although their code remains in the image.
The local project and reference exports are under ignored `build/analysis/`.

## Recover addresses and sizes first

1. Seed disassembly from the entry point, direct-call targets, verified import
   thunks, initializer pointers and credible vtable entries. Follow control flow
   recursively; arbitrary byte scanning can decode embedded tables as instructions.
2. Inspect each candidate's predecessors, branches, returns, tail calls and jump
   tables. A prologue or 16-byte alignment is supporting evidence, not a boundary.
   Some uncalled routines require examining gaps between already understood code.
3. For a contiguous function, record its entry VA and the extent through its last
   instruction, including internal alignment but excluding padding after it. Do
   not stop at the first `ret`, or subtract adjacent entry addresses to get a size.
4. Review disjoint bodies and shared tails separately. Ghidra's body byte count
   and `max - min + 1` are different quantities; neither can blindly become this
   CSV's single extent. Leave size blank until the representation is justified.
5. Compile a small reconstruction with VC8 to check the proposed ABI, linker name,
   references and full extent. Exact bytes corroborate a boundary; they do not
   independently prove which of several identical functions supplied its name.

Useful checks need no changes to the build:

```sh
readelf -sW ../saga/res/libTTapp.so
nm -S --defined-only ../saga/res/libTTapp.so
objdump -d -Mintel --start-address=0x0054fa90 --stop-address=0x0054fad0 \
    res/LEGOStarWarsSaga.exe
```

Use the existing PE reader for VA-to-file translation and zero-filled data;
Android's writable ELF segment also has a different file offset from its VA.

## Name functions by propagating confirmed relationships

This is a **call graph**, with shared callees, recursion and indirect calls.
Start with the already confirmed random and angle routines, distinctive strings,
tables and constants. Compare callers and callees of accepted pairs, using the
set, multiplicity and order of known calls. Combine this with data references,
argument use and control flow. A shared common helper alone is weak evidence.

Normalize Android PLT/GOT references and ignore its `__x86.get_pc_thunk.*` setup;
resolve PC import thunks to their IAT identities. Keep indirect calls unresolved
until a vtable or pointer assignment supplies evidence. Windows API calls and
Android platform wrappers need not correspond.

Rank candidates using several independent features: rare constants/strings,
accepted function and data neighbors, then approximate control-flow shape. Keep
alternative candidates and require a clear reason to accept a name. Promote a
newly confirmed pair into the anchor set and repeat in both directions. Avoid
requiring identical call counts: optimization, source changes, folding and
inlining can create one-to-many or missing correspondences.

The PC `/O2` build inlines aggressively. A source-level helper can appear inside
several callers with no remaining call instruction. Treat an absent edge as
missing evidence, not a contradiction; look for the helper's operations and data
references inside the caller. Keep translation-unit boundaries in the analysis,
since moving a definition beside a caller can introduce additional inlining.

[Ghidra Version Tracking](https://github.com/NationalSecurityAgency/ghidra/blob/master/Ghidra/Features/VersionTracking/src/main/help/help/topics/VersionTrackingPlugin/VT_Correlators.html)
already provides function-reference, data-reference and combined correlators
that build on accepted matches. Define the PC functions, seed accepted pairs,
then use those correlators to propose more. It is an optional analysis tool,
not a compiler or CI dependency. Keep its suggestions out of the authoritative
CSV until reviewed. [BinDiff](https://www.zynamics.com/bindiff/manual/) is another
graph-comparison option; its similarity scores are not objdiff byte matches.

## Windows entry point and NuMain

The PC executable uses the Windows GUI subsystem. Its entry point at
`0x006e74a2` reaches CRT startup at `0x006e72c2`. The CRT call at `0x006e7434`
passes four arguments: image instance `0x00400000`, a null previous instance,
the command-line pointer and the show-window argument. Its target,
`0x006de0e0`, is therefore the Windows `WinMain` entry wrapper. The complete
200-byte extent ends with `ret 0x10` at `0x006de1a5`; following `int3` alignment
is excluded. No ordinary `main(argc, argv)` entry has been established.

At `0x006de196`, the wrapper calls `0x00492b40`, passing the argument count
loaded from `0x00827664` and an argument-vector pointer `0x02975ab0`. The caller
then removes eight argument bytes, establishing the two-argument `__cdecl`
ABI. The callee passes `&argc` and `&argv` to the command-line state helper,
then follows the startup sequence seen in Android `NuMain`: world setup,
`InitOnce`, menu initialization and menu reset, followed by game initialization
and the main loop. This identifies the PC `NuMain`; Android's named reference
is at ELF VA `0x000f7ea0`.

The PC `NuMain` has a reviewed contiguous extent of **4,332 bytes**, ending at
its sole return at `0x00493c2b`. Ghidra reports 4,330 reachable body bytes in two
ranges because it excludes an internal two-byte `mov edi, edi` at
`0x0049336e`. A direct-branch walk reaches 1,106 instructions, finds no indirect
jumps or branches outside the extent, and leaves only those two padding bytes
unreachable. The comparison extent includes internal padding and excludes the
`int3` bytes after the return.

The map uses reconstruction names `_NuMain` and `_WinMain@16`, with explicit C
linkage and the observed calling conventions. These are names emitted by our
declarations, not spellings recovered from the stripped original. The source
entry now calls its reconstructed startup, opens the six episode DAT archives,
then exits before dereferencing the missing level and area tables. `GAME.DAT`
contains those lists; its registration and parser chain are unfinished. The
complete `NuMain` and `WinMain` extents do not match.

The [data inventory](data.md) records reconstructed constants, shared state,
type/extent evidence and the remaining data coverage.

## Initial NuMain dependency reconstruction

The PC body contains 147 direct call sites to 115 unique targets; Android has
334 call sites to 251 targets. Its update block is a separate PC helper at
`0x00491d50`; rendering is another at `0x004927f0` and receives the world pointer
in ESI through a compiler-private calling convention. Do not treat the decompiler's
`void(void)` rendering prototype as an externally callable ABI. These helpers
still need reconstruction. The [NuMain investigation queue](numain.md) records the call/global candidates
separately from accepted mappings until identities and extents are reviewed.

PC startup opens six episode DAT archives after menu initialization. It also
pumps Windows messages and handles `WM_QUIT`. Android analytics, mobile sound
callbacks, extra-data loading, MechSystems initialization and five-frame time
smoothing are absent. PC menu count is `0x17`; PC level records have stride
`0x130` and an area-index byte at `+0xa3`, differing from Android. Such differences
must be taken from the PC instructions, never resolved by copying Android types.

| PC VA | Function | Bytes | Source reference or PC evidence |
| --- | --- | ---: | --- |
| `0x004f4060` | `NuCommandLine` | 24 | PC stores its two command-line cursors; Android's provider is empty. |
| `0x004f0d40` | `NuDatSet` | 16 | `nu2api/nufile/nufile.cpp`; returns the previous archive selection. |
| `0x004f0d50` | `NuDatGet` | 6 | Same source; NuMain saves/restores selection around episode loads. |
| `0x005476e0` | `StartPerm` | 23 | `legogame/startup.cpp`; initializes permanent buffer cursors. |
| `0x00547700` | `EndPerm` | 124 | Same source; reviewed PC offsets and integer alignment arithmetic. |
| `0x0055f430` | `WorldInfo_InitOnce` | 21 | `legoapi/world/world.cpp`; clears exactly `0xa358` bytes. |
| `0x0055f450` | `WorldInfo_CurrentlyActive` | 6 | Same source; reads WORLD. |
| `0x0055f460` | `WorldInfo_CurrentlyLoading` | 6 | Same source; reads LWORLD. |
| `0x0065f4d0` | `edGraEnableTerrainSwap` | 1 | `gameapi/edtools/edtoolsall_plain.cpp`; verified retail `ret`. |
| `0x0065f4e0` | `edGraDisableTerrainSwap` | 1 | Same source; paired calls around `NuFrameEnd` establish identity. |
| `0x0065f4f0` | `edGraInitTerrainSwapProtection` | 1 | Same source; PC call immediately follows `MenuReset`. |
| `0x006dd390` | `NuWindowsProcessMessages` | 97 | Descriptive reconstruction name; SDK `PeekMessageA`, `TranslateMessage`, `DispatchMessageA`. |

## Additional exact leaf matches

The following functions were compiled with VC8 and compared over their complete
PC extents after relocation. They add verified dependencies without completing
`NuMain` or identifying `InitOnce`.

| PC VA | Function | Bytes | Evidence |
| --- | --- | ---: | --- |
| `0x0055c5e0` | `ClearPause` | 13 | Both C++ game-state writes clear the four-byte `Paused` and `NetPaused` globals; PC and Android behavior agree. |
| `0x005c9130` | `NuPause` | 11 | Pause/resume callers pass 1/0; Android stores the integer as `f32` in `nu2api_paused` (`nucore_plain.cpp`). |
| `0x005c9ff0` | `NuRegisterEndFrameCallBackFn` | 10 | Android stores a `void (*)(void)` callback; PC frame-end consumers invoke the stored pointer. |
| `0x005d9030` | `NuPortalInit` | 34 | Android initializes the portal stack buffer and boundaries; PC computes the same aligned base and `0x3000` end. |
| `0x005f9530` | `NuWaterReset` | 21 | NuFrameBegin call order identifies the routine; PC writes its two four-byte water-state globals. |
| `0x006d58a0` | `NuMouseRead` | 1 | Android body is empty; PC extent is a single `ret`, called in the frame-begin sequence. |
| `0x005e8430` | `NuOcclusionManagerBeginFrame` | 59 | Android calls `g_OcclusionManager.BeginFrame()`; PC swaps the two set pointers, clears the set fields, and resets both manager counters. |
| `0x0064ae40` | `SetPartRTLSet` | 10 | NuMain's world setup passes `world+0x2ab4`; PC stores that pointer at `0x02920aac`, later read by the parts lighting-template allocator. Android `SetPartRTLSet`/`PartRTL` corroborate the use. The old ray-cast identity was wrong despite an exact ten-byte match. |
| `0x004f4fc0`–`0x004f5120` | Video accessors and setters, rolling-rate reset | 1–68 | `nuvideo.c`; six mapped functions at `0x004f4fc0`, `0x004f4fd0`, `0x004f4fe0`, `0x004f5100`, `0x004f5110`, and `0x004f5120` match exactly. |
| `0x00697a30`–`0x00698a00` | Sound reverb, rumble, loop and hold-time helpers | 1–22 | `nusound.c`; all six mapped functions match exactly. |
| `0x0055f430`–`0x0055f560` | WorldInfo initialization, accessors, other-level and room visibility | 6–103 | `world.c`; all five mapped functions match against PC world records and typed fields. |

The next structural tranche ports connected PC-confirmed families rather than
optimizing isolated instruction sequences. All ten `NuStr*` primitives in
`nustring.c` match their full original extents, and six of the eleven text
functions in `text.c` match. The other five text functions are present with real
logic and recorded as byte-different; PC language selection, seven-record
tables and string-table bounds override Android assumptions. `nutimebar.c`
adds subtraction, frequency lookup and four time conversions; PC
`0x004f3ca0` is microseconds and `0x004f3d10` is milliseconds, as their
original `1,000,000.0` and `1,000.0` multipliers show. The source-only EXE
also includes typed panel setup, frame-counter scheduling, world-buffer
rearrangement and sound-stream queue/status/volume handling. Their exactness
is reported by `matching.json`, not presumed from Android source similarity.

Two matrix setters at `0x005b4500` and `0x005b45a0` have full 68-byte VC8
scratch matches, but their C object also emits unreferenced 0.0f and 1.0f
literal definitions with no defensible original address binding. The candidate
stays in ignored `build/analysis/numtx_port.c` until that inventory question
is resolved. No retail address was invented to inflate data or code coverage.

`NuFrameBegin` at `0x004f3e20` has a 32-byte VC8 scratch candidate that matches
the complete PC extent after explicit bindings. It is not in tracked source because
the PC `NuKeyboardRead` callee at `0x006d9340` still depends on an unreconstructed
helper and state. The occlusion frame routine is tracked independently; its named
manager and set fields are asserted in `saga.h`.

The typed PC DirectInput input prototype is staged in ignored
`build/analysis/nuinput_port.cpp`, not the source build: the static owner at
`0x029765a0` has verified fields through `+0x38`, establishing only a minimum
`0x3c` extent. Its setup at `0x006dacb0`, field setter at `0x006d5950`,
message handler at `0x006de1c0`, and shutdown call at `0x006dfdf0` do not reveal
the complete object boundary or a `sizeof`/clear extent. Do not map the owner
size until that boundary is recovered. The poller at `0x006d5990` and entry at
`0x006d9340` remain unverified source candidates.

The PC stores `nu2api_paused` as a four-byte float, and the frame-end code
indirectly calls the four-byte callback pointer. Android's declarations confirm
the corresponding types; these mappings use standalone data addresses, with no
aggregate-offset casts.

WorldInfo occupies two records starting at `0x0093d878` and `0x00942a24`.
Their stride is `0x51ac`, four bytes less than Android's record. The shared
header exposes only the verified buffer fields at `+0x100`, `+0x104`, `+0x108`;
other fields remain opaque. `EndPerm` writes both records' buffer boundaries,
independently confirming the stride. WORLD and LWORLD both initially point to
the first record. The Windows message structure at `0x02975a94` is a 28-byte MSG.
Its pump translates and dispatches before testing `WM_QUIT`, following the retail
instruction order.

The command-line inputs do not alias the private cursors in the original:
NuMain passes addresses of its stack parameters. Centralizing those cursors
in `data.cpp` removes the compiler's private-storage knowledge. VC8's
`__restrict` input qualifiers express that existing no-alias contract and retain
the PC load/store order with the C frontend. No runtime pointers target retail
addresses; the map is used only for comparison relocations.

C routines now use `.c` units. Existing C random routines remain exact after the
frontend conversion. `qrand` and `ResetSeeds` keep their previously mapped C++
names. Source provenance belongs here and in the existing seed evidence below,
rather than repetitive source-file headers. Check `matching.json` for current
byte results; an accepted identity in this table alone is not a matching claim.

## Worked example: seed initialization

Among 13,043 Android function ranges, exactly one decoded caller references both
`NuRandSeed` and `NuRandSetSeed`: `_Z10ResetSeedsv` at `0x00490260`, 114 bytes.
Exactly one of the analyzed PC functions references the corresponding mapped
PC addresses: `0x0054fa90`, 61 bytes. The call order is:

```text
Android ResetSeeds                  PC candidate at 0x0054fa90
srand48(0)                          runtime seed helper at 0x006e8e11
NuRandSeed(0)                       NuRandSeed(0)       [known anchor]
DebrisSetSeed(0)                    0x00659230
NuRandSetSeed(NULL, 0)              NuRandSetSeed(0, 0) [known anchor]
NuPartSetSeed(0)                    0x0064a450
```

Both versions also write `12345` to the known `qseed` and `0x1f3ad27f` to a
second global, identifying PC `GAMERAND`. Following the two remaining game
callees identifies their seed globals; the adjacent getters read those same
locations. Data references and initializers distinguish the getters from the
many otherwise identical six-byte routines in the image.

| PC VA | Bytes | Reconstruction linker name | Type |
| --- | ---: | --- | --- |
| `0x0054fa80` | 6 | `?getqseed@@YAHXZ` | `i32 __cdecl (void)` |
| `0x0054fa90` | 61 | `?ResetSeeds@@YAXXZ` | `void __cdecl (void)` |
| `0x005b9e50` | 25 | `_NuWindRand` | `u32 __cdecl (void)` |
| `0x0064a450` | 10 | `_NuPartSetSeed` | `void __cdecl (i32)` |
| `0x0064a460` | 6 | `_NuPartGetSeed` | `u32 __cdecl (void)` |
| `0x00659230` | 10 | `_DebrisSetSeed` | `void __cdecl (i32)` |
| `0x00659240` | 6 | `_DebrisGetSeed` | `u32 __cdecl (void)` |
| `0x00802538` | 4 | `?GAMERAND@@3HA` | `i32` |
| `0x008065bc` | 4 | `_NuWindQS` | `u32` |
| `0x008102e8` | 4 | `_partseed` | `u32` |
| `0x00810900` | 4 | `_debrisseed` | `u32` |

Android source evidence: `legoapi/misc/utilities.cpp`,
`legoapi/render/fx/game_deb.cpp`, `legoapi/render/fx/parts.cpp`, `globals.cpp`,
and `gamelib/nuwind/nuwind_groups.cpp`. `NuWindRand` has the same arithmetic as
`qrand` but different state: `NuWindQS` starts at `0x1365`. `partseed` and
`debrisseed` both start at `0x5c0999`; the initializer alone cannot distinguish them.

Isolated VC8 probes reproduced all seven function extents and all four scalar
initializers exactly after applying the independently identified bindings.
`ResetSeeds` needed a separate compilation unit: putting the setters in its unit
let MSVC inline them and changed the result. The runtime call was tested with a
`srand(unsigned int)` declaration. Independently, `_srand` and `_rand` from the
pinned compiler's `libcmt.lib` member `build\intel\mt_obj\rand.obj` match
`0x006e8e11` (13 bytes) and `0x006e8e1e` (34 bytes) exactly after resolving their
common `__getptd` call to `0x006f040c`. This supplies separate library evidence for
the runtime call's identity. Compiler libraries are another source of named
COFF routines for identification; their bytes and extracted members stay ignored.
These probes remain outside `src/`;
the newly inventoried routines do not count as implemented project matches.

## Define data from types and references

A confirmed PC type makes the size straightforward: an x86 pointer is 4 bytes;
an array is `count * sizeof(element)`; a record is `sizeof(record)` with layout
assertions. Establish the object's base and array count independently using
initialization, indexed accesses, bounds or pointer arithmetic. A four-byte load
may access one member of a larger object, so it does not alone prove a global's
size. Android's symbol size is useful evidence, not a PC layout guarantee.

For the seed examples, a future implementation can use normal declarations and
definitions, with the CSV supplying placement rather than hardcoded pointers:

```c
/* src/saga.h, inside the existing C-linkage block */
extern u32 partseed;

/* The owning .c unit, or the C-linkage block of a .cpp unit */
u32 partseed = 0x5c0999;
```

Preserve internal linkage for private state such as `debrisseed`. Let VC8 emit
the linker spelling and use that in the CSV. Recover initializers from the
Android source or understood values, then compare the complete PC data extent;
do not commit extracted binary blobs. For pointers, reconstruct a typed reference
and let the COFF relocation resolve through the map. For BSS, check the original
zero-filled virtual extent even though it has no file bytes.

A larger example is Android's `_ZL6crctab`: its 256 `u32` entries (1,024 bytes)
occur exactly once in PC `.rdata`/`.data`, at **`0x0080ed78`**. Three indexed
four-byte reads implement the same three-character hash. It is mapped as
`_crctab`, `u32[256]`; the source's original `const` spelling is not proven by
those reads. The existing `ang` table similarly has 513 two-byte entries.

The CRC references also expose a limit of call-only matching: Android `refpack`
calls `HashString`, whereas PC candidate **`0x00610690`** contains the hash inline.
Do not create a standalone PC `HashString` at that interior instruction address.
Its CSV row instead uses `inlined` in the address column, leaves size blank, and
retains the prospective linker name and signature. This records observed inlining
without claiming that no out-of-line copy could ever be found elsewhere. Such
rows supply no relocation binding, mapped bytes or standalone matching target.
If a real out-of-line copy is later identified, replace the marker with its VA
and extent and keep the inline-callsite evidence in these notes.
The containing `refpack` identity and its disjoint analyzed body remain a
candidate for further review, rather than a newly accepted function row.

For structures and C++ objects, check PC member offsets, padding, alignment,
packing, vtable shape and calling convention before transferring types. Matching
32-bit pointer sizes does not make GCC and MSVC C++ layouts interchangeable.

## RTTI and virtual-call anchors

Validate type-name strings through the complete-object locator and hierarchy
records, rather than naming arbitrary nearby pointers. Examples found in this
PC image are:

| Class | Type descriptor VA | Vtable VA | Observed method slots |
| --- | --- | --- | ---: |
| `Fade` | `0x00802ed8` | `0x00761920` | 6 |
| `GameThingManager` | `0x0080638c` | `0x00763f34` | 12 |
| `NetListenerInterface` | `0x00801dd0` | `0x0076104c` | 11 |

A VC8 probe confirms type-descriptor names such as `??_R0?AVFade@@@8` and the
extent `8 + name_length + 1` (19 bytes for `.?AVFade@@`). Alignment after the
terminator is not part of that symbol. These compiler records have variable
tails; leave `ctype` blank rather than claiming `sizeof(type_info)` is their
size. Their internal pointers still need relocation-aware reconstruction later.

Likewise, the vtable's complete-object-locator pointer precedes the vtable symbol;
it is not an extra virtual method. Follow slots to candidate methods, then use
Android class declarations, virtual behavior and call/data references to identify
them. Do not copy slot numbers directly: the Itanium vtable header and destructor
variants differ from MSVC's. PC RTTI says `class` for these examples whereas some
Android headers use `struct`; that spelling also changes the MSVC RTTI symbol.
`AINet` is an example of a PC RTTI name without a corresponding Android name found
in the examined reference, so the process must allow platform-only classes.

For example, `Fade`'s table leads to a candidate `Init` at `0x005b1250` (a
10-byte store to `this + 4`), `UpdateFade` at `0x005b1290` (a one-byte return),
and `GetFadeType` at `0x00564370` (three bytes returning zero). Their behavior
agrees with Android's class. These are useful next candidates, but exact method
linker names and prototypes still need a PC declaration probe before CSV entry.

## Portable subsystem reconstruction

The PC matrix and vector families now have typed `NUMTX`/`NUVEC` operations.
`NuTrigInit` at `0x005bdd50` fills 32,768 floats in `NuTrigTable` at
`0x029899c0`; the three rotation bodies index that table with the PC angle mask.
`RotateGameMatrix` at `0x00550330` calls those real rotations in four PC-confirmed
orders. Its complete 348-byte extent includes the compiler's local switch table;
mapping only the reachable instructions would falsely report a match. The RDPI
hex classifiers at `0x004f52a0` and `0x004f5310` likewise own local tables, so
their mapped extents are 99 and 107 bytes. They are structurally ported but still
byte-different.

The camera getter family uses one 120-byte `global_camera` object at `0x00973708`,
with its matrix at offset zero. Ten separate 64-byte matrix globals at
`0x0080b708`–`0x0080b9c7` have independently checked identity initializers.
The PC `NuMtxCalcCheapFaceOn` and `NuMtxCalcDebrisFaceOn` use the real view matrix
through `NuCameraGetViewMtx`; neither requires an Android platform path. PC
`NuVideoSetSwapMode` at `0x004f5050` uses the original PAL/NTSC frame-rate groups
and calls the one-byte platform hook at `0x004feb70`.

MSVC emits some 0.0f and 1.0f COMDAT constants that a local function does not
relocate because it uses `fldz`/`fld1`. The symbol map binds these by compiler
linker name to the independently referenced canonical PC constants at
`0x00751658` and `0x00751524`; the bytes and four-byte extents were verified in
the retail image. No address was assigned merely from an orphan object's position.

`WorldInfo_LoadObjectAnimFile` at `0x0055f5d0` is a strong PC/Android control-flow
identity, but remains a candidate: its PC `NuFileExists` path depends on the
unmapped virtual-file system, and `edanimLoadPage` is a 1,482-byte editor loader
without a portable Android implementation. The sound sample-table initializer at
`0x00697a50` likewise uses 0x1c-byte PC records and a substantial PC loader and
thread closure; Android's 0x20-byte record and setup code cannot be copied.

The `UpdateFrame` prelude at `0x00491d50` has six directly called PC routines
whose call order and argument forms agree with the Android frame path. PC body
inspection supports `PortalDoors_Update` (229 bytes),
`GameAISysStartFrame` (467), `Shadow_SetMode` (60),
`GizmoPickups_SetOnOff` (686), and `DoInput` (786). The ten-byte setter at
`0x005f9e90` is only a provisional `NuRndrShadowOnOff` identity: the PC stores
one argument to renderer state, whereas Android's implementation is empty.
These mappings establish locations and complete extents, not matches or reusable
Android layouts. The PC frame packet is 16 bytes (`f32` time, pause state,
pointer to two pad pointers, flags) and is dispatched through the thing
manager's virtual slot `+0x20`; no static target is assigned to that call.

## Portable math, animation and flow structure

The current PC batch adds the quaternion interpolation/conversion family, plane
and triangle tests, vector and matrix operations, the three atan entry points,
spline positioning, special bounds/end-frame queries, animation-set operations,
gizmo dispatch, and the AI script macro/reference parser. Android logic is used
as a candidate; named PC layouts and complete retail extents determine the ports.
These are structural implementations unless the current matching report establishes
full equality. Moving a helper across a translation unit can change inlining.

`GameAnimSys_Update` uses PC set bits 1 (visibility state), 2 (stop request)
and 4 (active-list membership); instance-animation repeat remains bit 8. Its
matrix evaluation is still marked at the actual missing call. Static object
bounds are implemented, while animated bounds await the same evaluator closure.
The spline runtime uses the PC record's signed point stride; `PointAlongSpline`
uses the separately verified twelve-byte position array and its private angle
helper's portable logic. Its unsigned fixed-point conversion references the
retail 2^32 float at `0x00753288`.

Gizmo callbacks take two arguments even though the public dispatchers accept four.
Flow pointer reset still marks the missing reset-table dispatch; its three-row
PC callback table is researched separately. The reset's guard is word mask
0x1000, and the reset uses visibility rather than activation. AI macro/ref-script
parsing is partial: ordinary action/condition-definition lookup is still marked,
and macro allocation/copy uses the reconstructed allocator instead of the original
private EDI/ESI helper closure. The original five parser comparison spans retain
internal instruction alignment; reachable-byte counts are not substituted.

The next broad groups are the ANI3 node/curve/matrix closure, the display-list
special updater, the complete flow callback table, and AI definition tables.
Their ignored drafts and proposal maps require shared layout and branch review
before acceptance. NuMain remains the incomplete entry scaffold described above.

The verified batch adds 76 function inventory entries and 77 compiled function
definitions (including a second real `NuFdiv` copy). Thirty-two functions and
50 data definitions newly match, with no lost exact results. Whole-original
fuzzy coverage is 2.037%, exact code coverage 0.825%, and image mapping 1.436%.
The math leaves include exact atan interpolation, quaternion slerp/conversions,
vector normalization/rotations, axis triangle tests and the legacy animation end
frame. Quaternion multiply remains 99.60% advisory similarity with different
bytes; plane intersection and the larger parser/animation/flow routines remain
structural. The current `matching.json` is authoritative for later changes.

## Repeatable graph identification

`python3 tools/check.py --identify --holdout` ranks whole-program candidates from
the retained Android ELF graph and PC call/reference exports. Add `--source-graph
build/analysis/source-graph/android-source-graph.json` for source signatures,
clean direct calls, named global references and literal values. Extraction and
feature-refresh commands are described in [CONTRIBUTING.md](../CONTRIBUTING.md).
All generated graphs, candidate ledgers and original-derived evidence remain
under ignored `build/analysis/`; the accepted four-column CSV remains separate.

The current source-tree extraction covers 490 translation units using the actual
Android GNU C11/C++11 syntax modes: 447 have no diagnostics, while 43 remain
degraded. Clean evidence contains 11,267 definitions, 44,980 calls, 44,010 global
references and 5,983 literals. Virtual references and unresolved calls are not
promoted to direct edges. Adjacent string literals retain their complete value.
Source identities join ELF nodes only through unique, compatible linker names;
inline-only or otherwise unjoined definitions receive no invented addresses.

The reviewed map supplies 732 unique PC/Android name seeds. Four propagation
rounds produced 1,485 additional candidate pairs, with alternatives and individual
call/data/string supports in the ledger. In the deterministic holdout, 183 known
pairs were removed; 101 were proposed and all 101 agreed with those existing
labels. This is encouraging retrieval evidence on a biased known-function sample,
not a probability calibration or proof of unknown mappings. Generic parser/draw
calls, compiler helpers, inlining and source/platform differences still create
false positives. Do not infer PC code progress from these pair counts.

The ledger distinguishes entry/min/max/body/range information. Split bodies or
ranges below the entry leave the candidate extent blank; 202 inferred pairs need
that boundary review. Even a single Ghidra range remains an analysis hypothesis.
Input hashes and 36,155 corroborated retail direct-call operands prevent quietly
using unrelated exports. Reference caches reject stale PC map inputs; refresh
explicitly after extending the map. Android ELF GOT features currently resolve
R_386_RELATIVE slots and direct PIC addresses, not every relocation kind; the
source-reference channel supplies complementary evidence.

A family review accepted 71 additional function identities and three data entries:
17 gizmo/flow entries, 14 ANI3 entries, six rendering/file/particle identities,
11 sound wrappers/loaders, 12 table-anchored AI callbacks, and 11 resource/parser
entries. Complete code spans were checked against PC terminal instructions and
conditional branches; internal flow/loader padding remains included. The three
data entries are the 36-byte flow callback table at `0x00805514`, its eight-byte
`default` output name at `0x00757734`, and the zero byte checksum at `0x00956454`.
These are inventory additions, not implementations. Private flow helpers retain
blank ctypes where EDI/ESI conventions are not ordinary public C calls. ANI3
private helpers also retain blank ctypes pending their source organization.

Unresolved CharConfig fastcall decoration, network renderer member names and the
adjacent ANI3 shared-gap function remain research. The next connected source
batch is the six-function ANI3 matrix/extraction path, followed by the complete
flow callback closure and ordinary AI callbacks. Keep full byte comparison as
verification after each broad portable implementation batch.

## Transfer throughput and next families

The Android snapshot examined on 2026-09-30 reports 65.112% objdiff fuzzy
similarity, 11.013% exact code bytes and 46.430% exact functions. These are
different measures, and none predicts PC matching. Before the connected
transfer batch, the PC had 111,570 original bytes under comparison (3.214%
of its 3,471,246-byte text), averaging 63.434% objdiff similarity and supplying
2.037% whole-code fuzzy coverage. Expanding compared code is the principal
throughput target. The 202,038 mapped code bytes include inventory-only entries;
they are not all compiled implementations.

The graph's next large source families are movement (`move.cpp`, 20 candidates,
37,443 hypothesized bytes), game objects (`gameobjects.cpp`, 12/20,706), bolts
(8/14,845), and the AI system (`aisys.cpp`, 16/13,219). Those sums filter inferred
pairs for at least three seed supports and a single analysis range; they remain
research estimates, potentially overlapping or misidentified. Movement/object
work first needs a shared PC object-layout batch. Rendering candidates require
separate backend review and are not automatic portable transfers.

For the next lower-dependency source batch, review `NuStringTableLoadCSV`
(`0x004fda90`, current map extent 1,260) and TXT (`0x004fdf80`, 899) together
with their string-bank allocation/lookup closure in Android `nustring_c.cpp`.
Their file/parser APIs are already present. `NuStringFilterLoad` at `0x004fe390`
has a multi-range boundary and must be reviewed separately. The remaining ANI3
blend players are another connected batch with established types. A full LZ2K
decoder needs its bit/Huffman state and private ABI closure; the retained
`implode.cpp` reference supplies portable logic, but a header-size helper is
not evidence that decoding itself is reconstructed.

The first shared object-layout investigation narrows that larger batch to the
PC coordinate/angle region at +0x58..+0xa0, rather than the whole Android
GAMEOBJECT. Candidate MovePlayerSpline (0x00415c60) reads the +0x5c vector,
snapshots it at +0x8c and calls candidate GameObjectOrigin (0x0055d2b0),
which writes the vector at +0x80. Candidate UpdateGameObjects (0x00434a20)
calls both. These entries are still research: review identities, full extents,
calling conventions and the origin branch before naming fields or adding CSV
bindings. The +0x80 vector's precise role is unresolved. Current GAMEOBJECT
already establishes flags_1fc, target_velocity (+0xf18) and id (+0x1064);
replace only independently verified portions of its padding.

The connected transfer adds 62 compiled function identities and 19,088 original
code bytes to comparison. All previously exact functions and data are retained.
The ANI3 player bodies have weak instruction correspondence and are not exact;
large source/retail size differences still need compiler-structure review. The
PC uses SSE quaternion endpoint interpolation and unconditional rsqrtps
normalization. EulerQuat converts two Euler endpoints before quaternion
interpolation for quarters 0..2, but converts one interpolated Euler sample
for quarter 3, which bypasses normalization (0x005cbdac jumps past the SSE
store to 0x005cbe86). The interpolation arms retain those PC SDK intrinsics;
the Android scalar normalization/interpolation path is not interchangeable.
