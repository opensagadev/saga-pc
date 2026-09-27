# Recovering PC symbols from the Android reference

The useful comparison is between the **original Android ELF** and the **original
PC PE**, with the Android source supplying names and type hypotheses. Compiling
the reconstructed Android game for a host platform would add another compiler
and is unnecessary for this work. The four-column [symbol map](symbols.csv)
contains accepted identities; record uncertain alternatives here before adding
them. Identification and exact reconstruction matching are separate results.

## What is available

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
declarations, not spellings recovered from the stripped original. The current
source entry path implements the first two PC calls, `NuCommandLine` and
`WorldInfo_InitOnce`, then returns. The additional exact leaf matches below do not
complete that path: `InitOnce` and the rest of the game loop remain unfinished, and
the complete `NuMain` and `WinMain` extents do not match.

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
| `0x005c9130` | `NuPause` | 11 | Pause/resume callers pass 1/0; Android stores the integer as `f32` in `nu2api_paused` (`nucore_plain.cpp`). |
| `0x005c9ff0` | `NuRegisterEndFrameCallBackFn` | 10 | Android stores a `void (*)(void)` callback; PC frame-end consumers invoke the stored pointer. |
| `0x004f4fc0`–`0x004f5120` | Video accessors and setters, rolling-rate reset | 1–68 | `nuvideo.c`; six mapped functions at `0x004f4fc0`, `0x004f4fd0`, `0x004f4fe0`, `0x004f5100`, `0x004f5110`, and `0x004f5120` match exactly. |
| `0x00697a30`–`0x00698a00` | Sound reverb, rumble, loop and hold-time helpers | 1–22 | `nusound.c`; all five mapped functions match exactly. |
| `0x005d9030` | `NuPortalInit` | 34 | Initializes PC portal stack boundaries from the mapped `fstack` range. |
| `0x0055f430`–`0x0055f500` | WorldInfo initialization, accessors and `WorldInfo_OtherLevel` | 6–36 | `world.c`; all four mapped functions match against PC world records and fields. |

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
