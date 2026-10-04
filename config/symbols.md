# Original executable symbol map

[`symbols.csv`](symbols.csv) is the authoritative address map for the stripped
PC executable identified by [`project.json`](project.json). It grows independently
of the source tree: identifying a function or global does not require implementing
it first. The initial map is incomplete.

The file has exactly four columns. `address`, `symbol`, and `size` use
the meanings in [reccmp's CSV format](https://github.com/isledecomp/reccmp/blob/master/docs/csv.md);
`ctype` is the recovered C/C++ type. Our reader supports whitespace padding
around fields so the map can be aligned for inspection. Reccmp is not a build
dependency.

| Column | Meaning |
| --- | --- |
| `address` | Preferred-image virtual address, in hexadecimal. The image base is `0x00400000`; these are not file offsets or RVAs. |
| `symbol` | Exact MSVC COFF linker name expected in our reconstruction. The stripped executable does not supply this name. Use a provisional name for an identified entity without source yet. |
| `size` | Size in bytes, normally decimal (`0x` hexadecimal is also accepted). Leave blank when the extent is unknown. Functions include owned compiler tables and internal alignment; alignment after the complete owner is excluded. |
| `ctype` | Recovered C/C++ function signature or data type. Leave unknown types blank rather than guessing. |

Keep rows sorted by address and quote signatures containing commas. Known sizes
must be positive and fit within the 32-bit address space. Unknown extents can
remain in the inventory with a blank `size` until their boundaries are recovered;
they cannot be compared yet. Aliases and overlapping extents are allowed.
Progress counts their covered-byte union so they do not inflate the totals.
Repeating the same address and symbol is an error, including with a different size.

The matcher searches the compiled objects corresponding to the current source
list for each exact `symbol`; there is no source-unit column. A missing definition
means the entry is unimplemented. Multiple definitions of one mapped symbol are
compared independently, and all must match for that entry to count. A linker name
mapped to multiple original addresses produces an ambiguous relocation binding;
comparison rejects a reference that needs it instead of choosing an address.
Compiled data may retain a blank original size while its complete PC extent is
being recovered. The object check still requires its address binding and checks
all relocations, but matching gives it no byte coverage or exact-match credit.
Compiled functions require complete PC extents.

Compiled COFF symbol and section metadata distinguish functions from data. For
inventory reporting, an entry without a compiled definition inside the original
`.text` section is unimplemented code; it never counts as a match. The `ctype`
column documents the recovered C/C++ type and does not replace the COFF metadata.

C and C++ linkage are supported: `_NuRand` and `?SetSampleName@SoundVoiceManagerPC@@QAEXPBDH@Z` are different
MSVC decoration styles. `make -j"$(nproc)" diff FUNC=NuRand` accepts a simple undecorated C
name. For C++, use the exact mapped linker symbol or its original address.

[`src/saga.h`](../src/saga.h) owns compilable type definitions, calling conventions,
and layout checks. The map's `ctype` is useful during inspection, including before
source exists; update it together with the header when revising a declaration.
Matching bytes do not prove the exact original typedef spelling, signedness,
source name, or prototype.

## Identification notes

See [the identification guide](identification.md) for the address-transfer workflow,
call-graph evidence, and additional internal mappings.

Keep identity hypotheses, supporting evidence, Android correspondences, and
uncertainty here, keyed by the original address. These are separate from the
current matching results in [`matching.json`](../matching.json).

Use distinctive strings, constants, calls, data references, and neighboring
functions as anchors. Follow callers, callees, shared state, and vtables to find
more candidates. Record uncertainty before treating a candidate as established.
Address similarities between Android and PC are not evidence: their layouts and
ABIs differ.

The initial mappings have the following evidence:

- **Random-number family, `0x005b9d70`–`0x005b9ef0`:** the corresponding Android
  source is `src/nu2api/numath/nurand.cpp`. The seeded integer and float generators
  share multiplier `0x19660d` and increment `0x3c6ef35f`; the float generator
  constructs an IEEE mantissa. `NuRandIntSeeded` has a unique complete body.
  The unseeded variants specialize these operations to `fseed` at `0x0095e5f4`,
  in zero-filled `.data`. Adjacent `NuRandSeed` and `NuRandGetSeed` write and read
  that same state. Their Android function symbols use the undecorated C names.
- **`NuRand`, `0x005b9e90`:** its unique complete body has opening and closing XOR
  constant `0x075bd924`, division by `0x1f31d`, negative-result correction,
  zero-state repair, and a null-pointer fallback. The fallback `global_rand` at
  `0x008066c0` starts at 1. `NuRandSetSeed` uses that same fallback.
  `NuFloatRand` at `0x005b9ef0` calls `NuRand` and scales the result by `2^-31`.
- **`qrand`, `0x0054fa60`:** the Android source is
  `src/legoapi/core/input/qrand.cpp`, with Android symbol `_Z5qrandv`. The PC
  reconstruction now emits `_qrand` from a C translation unit. Its multiplier `0x24cd` alone is ambiguous;
  `qseed` at `0x0080253c` starts at 12345, and the neighboring seed getter and reset
  routines corroborate the identity. The Android variable is named `qseed`; MSVC
  emits `_qseed` with C linkage. The similar routine at `0x005b9e50` instead starts with
  `0x1365` and corresponds to `NuWindRand`; it is not mapped as `qrand`.
- **Angle helpers, `0x005be590`, `0x005be5b0`, `0x005be600`:** Android's
  `src/nu2api/numath/nutrig.c` has the same argument order and operations for
  `NuAngAdd`, `NuAngSub`, and `NuAng2AltSol`. The first two wrap 16-bit angles.
  Retail VC8 uses `cmp ...,0x8000; jl`; the reconstruction therefore uses
  `>= 0x8000` instead of Android's equivalent `> 0x7fff`. The last helper's three
  stores and source order agree with Android. Their Android symbols use the
  undecorated C names.
- **Compiler literals, `0x00751698` and `0x00764000`:** these are double `1.0`
  and `2^-31`, used by the float random routines. The mapped `__real@...` names
  are generated by MSVC. Discovered literal definitions are compared as data
  and also supply relocation bindings.
- **Angle lookup table, `0x008066c8`:** all 513 unsigned-short entries correspond
  to `ang` in Android's `src/nu2api/numath/nutrig.cpp`. `_ang` is the prospective
  reconstruction linker name, not a symbol recovered from the stripped PC file.
  It has no implementation yet and does not count as reconstructed matching code.

Comparison resolves each compiled reference through this map and checks complete
extents against the local retail executable. Synthetic original objects are local
comparison artifacts, not recovered original object files. Objdiff's similarity
scores are separate from strict byte equality.

## Current structural mapping pass

The direct-call inventory in [`numain.md`](numain.md) supplies call sites inside
the complete PC `NuMain` body. The accepted unimplemented entries in
`symbols.csv` have separately reviewed final instructions and full contiguous
extents; the Ghidra body counts in that queue alone are not used as sizes.
Caller pushes and return-value use establish their callable types. In particular,
`Game_Exit` at `0x0055c6d0` returns an integer that NuMain tests, while
`WorldInfo_StreamLevel` receives a `BGPROCINFO *`. Their linker names are
reconstruction names, since the retail executable is stripped. These rows add
inventory coverage but no matched code bytes until compiled definitions exist.

The parser family at `0x004f1240`, `0x004fa300`, and `0x004fc2d0`–`0x004fc900`
is identified by shared access to four 0x584-byte parser records, their 0x1000-byte
read buffers, the UTF flags at offsets `0x578`/`0x579`, and PC call edges from
the level and area loaders. The create, destroy, and narrow line-reader source is
present but still structurally different from retail; its wide path and table
record layouts remain to be recovered.
The level and area loaders at `0x005788f0` and `0x00572b00` have complete PC
extents and their 0x130/0x9c record strides, respectively.

The newly accepted four-byte startup/frame globals include independent level
pointers (`LastLData`, `NewLData`, `HUB_LDATA`, `STATUS_LDATA`,
`CREDITS_LDATA`), timers (`MainRenderTime`, `MainRenderTargetTime`,
`WaitingForLevelTime`, `WaitingForCharacterTime`, `MAXFRAMETIME`), and scalar
flags/counters. Their addresses and access widths come from PC references in
NuMain and its direct callees; Android names and types only corroborate the
identity. Fields inside `GAMESAVE`, `FadeSys`, and `StatusPacket` remain aggregate
members rather than stand-alone symbols.

The primitive call chain is now inventoried from the PC `NuPrim2DBegin` and
`NuPrim3DBegin` callers through the display-list material linker, render-state
updater, item linker, and geometry-transform builder. PC argument pushes,
material/state references, and corresponding Android call order support these
identities; their full instruction extents are in the CSV. The primitive end
functions match exactly, while begin functions and the display-list closure are
unimplemented. The packed-colour helper at `0x005c1f70` and the second matrix
transpose form at `0x005f7610` retain provisional identities because the PC
uses private register ABIs there.

The expanded DAT/file inventory follows PC handle ranges: native files below
`0x400`, memory files from `0x400`, and archive members from `0x800`. The accepted
`NuMemFileOpen`/read/write/seek/close, `NuDatFileClose`/open-size/load-buffer,
`NuFileLoadBuffer`/VP, parser-position, and file-pak rows were checked against
complete retail bodies and their call graph. The PC-specific extension table has
64 records of 16 bytes; its initialization takes a selector argument, unlike the
Android no-argument helper. The `LZ2K` decoder and `RNC` packed-file paths are
mapped but remain unimplemented. `read_buffer` and `decode_buffer` have reviewed
addresses but blank extents until their capacities are established from PC uses.

The NuMusic family map now includes album/track lookups, class selection,
pause/resume, the large `Process` mixer, and both `PlayTrack` overloads. VC8
scratch declarations establish the decorated names; retail fields and call edges
establish the PC identities and full extents. Most remain unimplemented, so the
map does not imply an operating mixer. `music_man` retains an unknown full data
extent despite PC-confirmed field offsets through `+0x1e0`.

The world/level family inventory adds 79 reviewed PC entries across
`WorldInfo_Load`, `WorldInfo_Init`/`Reset`/`Dump`, progress helpers, 13 named
configuration parsers, cutscene callbacks, and editor page teardown. Each entry
has a complete PC instruction extent, including internal alignment where present;
the calling convention and record type are corroborated by PC callers and state
accesses. These are structural addresses and signatures for future source work,
not a claim that the level loaders or frame loop are playable.

The file-pak byte-copy family is verified against retail call sites:
`PCByteCopy_candidate` (`0x004ef660`, 84 bytes) copies forward when
`dst < src` and backward otherwise, with the forward loop spelling
`count--; *d = *s; d++; s++;` and the backward loop splitting `count--; d--; s--; *d = *s;`
(that exact spelling reproduces the retail 84 bytes). Retail call sites fix the
argument orders: `SfxBitsStore` copies from `SfxBits` (`0x002921780`) into the
argument table, `SfxBitsRestore` copies back, and `ResetSounds` copies
`GlobalSfxBits` (`0x002947cc8`) into `SfxBits`, all 0xc8 bytes.
`NuFilePakGetSize` and `NuFilePakRelocate` load their header fields into
registers before the `fpk_err = 0` store; Relocate pushes count, pak,
destination and the byte-copy ABI is `(destination, pak, size)`.
`NuFilePakGetItemInfo` and `NuFilePakSetItemRequired` match exactly.
`NuFilePakCondense` and `NuFilePakGetItem` are structurally identical to retail
(same magic switch, item stride `0x1c`, flag masks) and differ only in
instruction scheduling; assigning `removed_size = 0` after the inlined
`GetPakItems` switch improves the Condense similarity. `PCByteCopy_candidate`
keeps its tentative name: no retail symbol spelling exists in the stripped
image, and the identical-copy identity is not yet pinned to a specific Android
routine.

## LoadPerm / LoadPermData callee identification

The PC `LoadPerm` (`0x004ca5b0`, 2,004 bytes) and `LoadPermData`
(`0x004c9fb0`, 1,532 bytes) call a set of now-mapped helpers. Android
correlations come from `legogame/startup.cpp`; PC call sites, constants and
data references confirm each identity. All extents below end at a `ret`
followed by `int3` (or directly by the next function where no padding exists).

Accepted identities (Android body/callsite parity, verified PC extents):

| PC VA | Function | Bytes | Evidence |
| --- | --- | ---: | --- |
| `0x0054b090` | `ReadPads` | 32 | Loop calls `ReadPad(0)`/`ReadPad(1)` then clears `0x00926bf0`; Android `gamepads.cpp` `ReadPads`. |
| `0x00603cc0` | `NuCameraSet` | 91 | `LoadPerm` copies the identity matrix to `0x00925184` then calls it with that pointer, and calls it with `global_camera` per frame; Android `NuCameraSet(pNuCam)`/`NuCameraSet(&global_camera)`. Forwards to `0x005ed540` and the `0x0080b788` viewport block. |
| `0x00509830` | `NewMenu` | 629 | `LoadPerm` calls `NewMenu(7, 0, -1)` where Android does the same; stride-`0x1c` menu table at `0x00800aa8`, `GameMenuLevel` at `0x00800944`, screen stride `0xe0` from `0x00921460`. |
| `0x0050fed0` | `DrawMenu` | 543 | Called inside the `0.2f` tail-timer guard like Android `DrawMenu(0)`; `[screen+0x14] == 0xffff` early-out and per-menu callbacks match. |
| `0x005d4290` | `NuRndrBeginScene` | 109 | Resets the `0x218`-byte scene record at `0x009718c0` (scene id `-1`, colours, counters) exactly as Android `nurndr_plain.cpp`; argument unused on both platforms. |
| `0x005feab0` | `NuRndrGradClear` | 27 | Forwards `(flags, colour, alpha)` to `0x005ebd80`, dropping the third colour like Android's stub; `LoadPerm` calls `(0xf00, 0x80000000, 0x80000000, 1.0f)`. |
| `0x005ebd80` | `NuRndrClear` | 111 | ORs flags into `0x009718c8`, stores colour/alpha at `+0xc/+0x10`, reads the viewport pair, then PC-specific aspect math through `0x00963e38/0x00963e3c` and `0.0625`. `LoadPerm` calls `(0xb00, 0, 1.0f)`. |
| `0x005f3950` | `NuRndrEndScene` | 139 | Appends the `0x218`-byte scene to `0x00964110` at `0x0095fbd8`, calls `0x005efff0` for the scene id, and handles the dynamic-light pointer at `+0x38/+0x3c` exactly as Android. Called before `edGraEnableTerrainSwap`/`NuFrameEnd`. |
| `0x005e50e0` | `NuMtlDestroy` | 95 | Clears the used bit (`and word, 0xfffe`), destroys via `0x005de220`, walks the override list testing `[esi+4] == mtl`; both `LoadPerm` tails call it on `legal_mtl` and null it. |
| `0x005ff7a0` | `NuSpecialDrawAtAlpha` | 122 | Null-scene/alpha<=0 rejection, `0x005cfb50` constant-alpha pair around `0x005f0f00` (`NuDisplayListRndrSpecial`, five args); `LoadPerm` draws the `pc_bits.gsc` scene with the `NuMtxSetScale` matrix. |
| `0x0054bf20` | `IntroText_Draw` | 262 | Early-out on `IntroText_TextID` (`0x00802368`) == -1; `(alpha*128.0)<<24 | 0x7f5f00` colour, `SetQFont2D` callee at `0x0050e6a0`, `NuQFntPrintJustifiedW` at `0x005f69c0`; matches Android `text.cpp` `IntroText_Draw`. |

Candidate identities (role confirmed, name/identity not fully pinned):

| PC VA | Candidate | Bytes | Note |
| --- | --- | ---: | --- |
| `0x00698e70` | `NuSound3Update` | 86 | Mix-buffer copy, `SetEvent` on `0x02972c44`, stream kick, tail-jump to the voice sweep at `0x00698cc0`; guarded by the `0x002962054` sound flag everywhere. No byte-provable Android counterpart (different audio backend). |
| `0x004c9a40` | `NuPrim2DSetColor` | 108 | Packs ARGB for the 8888 or 565-format immediate-2D vertex colour into `[stream]+0xc`; the argument arrives in EAX (private register ABI, like the `InitOnce` argument helper) from both `LoadPerm` call sites. |
| `0x004c9d10` | `LoadPermCreateLegalMtl` | 97 | Creates the legal-screen material (`attrib |= 0xe022`, diffuse/opacity 1.0f, `+0xb4 = 0x1000`, texture id at `+0x74`); the out-pointer arrives in EDI preloaded with `&legal_mtl` (private ABI). |
| `0x004c9f60` | `MemoryManagerInit` | 71 | 16-aligns the buffer cursors and clears the `0x22c`-byte free lists; kept COMDAT copy of the code inlined into `LoadPermData` at `0x004ca183`. |

LoadPermData-family helpers (all in the startup translation unit,
`0x004c9b20`-`0x004c9fa6`): `LoadPermBuildLegalTextureName` (`0x004c9b20`)
builds `"legal" + region suffix + language path`; `LoadPermLoadLegalTexture`
(`0x004c9bd0`, stack-cookie function) loads `"stuff\legal\<name>.dds"` into
`legal_tid` (`0x00880590`) once per run; `LoadPermDestroyLegalMtl`
(`0x004c9d80`) destroys `legal_mtl`; `LoadPermLoadFonts` (`0x004c9db0`) picks
the PC font path by startup flags (`pc_font`, `starwars_font_j`,
`starwars_font`, `batman_font`); `LoadPermLoadSaveIconScene`
(`0x004c9e10`) loads `stuff\save_icon.gsc` or `stuff\pc_bits.gsc` into
`0x009248a8`; `LoadPermProgressDraw` (`0x004c9e70`) draws the loading
percentage (times 100.0 from `0x007519f0`) as the alpha byte of a filled
rectangle. `LoadPermFadeAlpha` (`0x004c9eb0`) is implemented in source: its
piecewise window (`t/0.3f` rise, hold to `duration + 0.3f`, reciprocal fade-out
`1 - end/(end + 0.3f - (t - end))`, zero outside `[0, end + 0.3f + 0.2f)`)
matches the first 109 retail bytes exactly; the remaining tail differs only in
branch layout and exit-block merging (retail shares the two return-zero exits
and spills `end + 0.3f` across the branch), with identical semantics.

The `0.3f`-promoted double at `0x007531c0` (`__real@3fd3333340000000`), the
`0.2f`-promoted double at `0x00751680`, and the `0.3f` at `0x00751728` are
referenced directly by the retail fade helper.

`LoadPerm` (`0x004ca5b0`) and `LoadPermData` (`0x004c9fb0`) are now
structurally reconstructed with their real PC stage sequence. PC-specific
behavior verified against the retail bytes: `LoadPerm` initializes its
language-stage timer to the saturated `3.3f` (the language menu runs before
`LoadPerm` on PC, through `MenuInitialise`), so the in-loop menu state machine
and the special-scene timer block are gated off; the ready, legal and intro
stages run under the `>= 3.3f` language gate. `menu_flash` is
`NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f`, the legal timer
runs 0 to 5.8f with the legal material created once when `legal_tid != 0` and
`loadlegal_done != 0`, the intro timer runs to 3.8f, and the tail countdown
requires `PermDataLoaded`, `LoadPerm_LanguageSelect == 3`, ready `> 1.0f`,
legal `>= 5.8f` and intro `>= 3.8f`. The loop additionally exits when
`FRAMETIME > MAXFRAMETIME` and `CUTSTOPGAME == 0`; otherwise `FRAMETIME` is
clamped to `MAXFRAMETIME`. The legal quad half-size is
`(1 - 4/3 / aspect) * 0.5` with vertices at that value and its complement,
where the aspect comes from the `0x00826494` selector (`0` = 16:9, `1` = 1.6,
`2` = the runtime `0x02976c18` value, otherwise 4:3). `InitPanel` receives
`Game.options_save.widescreen` (`Game` + 0xf). `LoadPermData` splits the text
tables at ids `0x31f`/`0x320` only when both startup flags are clear, runs the
shared registration tail when `!(Flag0 == 0 && Flag1 != 0)`, wires the ten LSW
gameplay hook pointers only in the `Flag1 != 0` branch, initializes the
`0x248`-byte memory manager at `0x02913270` (arena `0x200` bytes carved from
the aligned permbuffer, self-anchored start/end pointers, `0x22c` free-list
bytes), copies the default-character ids from the Flag1 pair
(`0x007f185c`/`0x007f1860`) or the base pair (`0x007f17b8`/`0x007f180c`) into
`id_DEFAULTCHARACTER`, and finishes with two 16-byte alignments around
`PermDataLoaded = 1`. The default-character source id names are hypotheses
from the Android `id_QUIGONJINN`/`id_OBIWANKENOBI` defaults; the PC stores are
verified. Every callee without a compiled reconstruction remains a
`STUBBED_NAMED` marker at its real call position; the loading-screen
registration struct at `0x00421320`, the two tail function-pointer
registrations, and the ten hook targets (`0x0042d740` through `0x0042df30`)
are the next mapping candidates in this family.

The render-scene family lives in `src/nu2api/nurndr.c`. `NuRndrGradClear`
(`0x005feab0`), `NuRndrClear` (`0x005ebd80`), `NuFdiv` (`0x00403cb0`) and
`NuVpGetPosition2` (`0x005dad10`) are exact. `NuRndrClear` stores the
alpha/colour/flags, reads the viewport position through `NuVpGetPosition2`
into scene +`0x14`/`+0x18`, and computes the two overscan factors as
`0.0625 * vpCurrent.width / (pixel_width / PS2_VREZ_W)` and the height
equivalent, with the CSE'd `0.0625` constant staying on the FPU stack across
both factors. `NuVpGetPosition2` is Android's `NuVpGetPosition2` verbatim:
`NuFdiv((f32)vpCurrent.x * 0.0625f - ((f32)PS2_VCNTR_X - (f32)(pixel_width >> 1)),
NuFdiv((f32)pixel_width, (f32)PS2_VREZ_W))` and the height equivalent, with
`NuFdiv` inlined four times (its guarded `a == 0 || b == 0` divide). The
retail image carries roughly thirty identical standalone 54-byte `NuFdiv`
copies (per-unit out-of-line emissions); the reconstruction binds one of them
at `0x00403cb0` and inlines the rest in its callers, as the retail does.
`vpCurrent` (`0x00963e30`) holds `i32 x, y, width, height` (Android's
`NUVIEWPORT` head); its first four bytes are the position and
`width`/`height` feed `NuRndrClear`'s overscan factors. The render-target
globals `0x00973920`/`0x00973924` keep their accepted `DebrisGlassTexture`
names and also serve as the viewport pixel dimensions in the `NuFdiv`
divisors. `NuRndrBeginScene` (`0x005d4290`, 109 bytes) resets the named
fields of the `0x218`-byte scene record and always returns 1; the retail
epilogue computes the return as `lea eax,[ecx+2]` from the register-held -1
(the compiler-level `or ecx,-1` encoding of the constant prevents the
constant fold), which our spelling does not yet reproduce.
`NuRndrEndScene` (`0x005f3950`) remains unimplemented pending the
`NuDisplayListAddRenderScene` callee at `0x005efff0`; its retail body appends
the `0x218`-byte record to the `0x00964110` array at `0x0095fbd8` and routes
the scene id into the dynamic-light pending ring at light+`0x354`/`0x374`.

## LSW gameplay hook family

`LoadPermData`'s `Flag1` branch wires ten hook pointers
(`0x00951cac`-`0x00951cbc`, `0x00954410`, `0x00951ce4`, `0x00951cec`,
`0x00950bd8`, `0x0093b208`) to the LSW hook targets in
`src/legoapi/cutscene_hooks.c`, reconstructed from the Android
`*_LSW` functions. `CutScene_PostUpdateFn_LSW` (`0x0042da00`, a single
`ret`), `GizObstacle_SetDefaultSFXFn_LSW` (`0x0042df30`: mode `2` and
trigger radius `< 0.25f` set the `SwPPad` sfx id through `GetSfxId`) and
`CutScene_StoppedFn_LSW` are exact. `CutScene_StartFn_LSW`
(`0x0042d740`, 412 bytes, 46%) is reconstructed with its real
podrace/bonus-gunship selection: the `PODRACE_ADATA`/`BONUS_GUNSHIP_ADATA`
area gates, the `PODRACEA/B/GUNSHIPA_LDATA` level branches, the five
verified die positions (`0x007f5d08`-`0x007f5d40`), the degree-to-NUANG
conversion written as `(u32)degrees << 16 / 360` (the retail's
`0xb60b60b7` multiply-shift idiom), and `instNuGCutSceneSetMtx`
(`0x00506430`, 41 bytes, 53%) copying the matrix into the instance at
+0x18 with the +0x88 bit 0x80 flag. `instNuGCutSceneSetMtx`'s tail calls
`NuGCutSceneUpdateMtx` (`0x00500b90`, 86 bytes), whose retail body reads
its operand from EDI (the private register ABI); the reconstruction
declares it `__cdecl`, so the call shape differs.

`CutScene_ReplaceCharacterModelFn_LSW` (`0x0042daa0`, 507 bytes, 71.6%)
and `InitBolt_AddMomentumType_LSW` (`0x0042dca0`, 174 bytes, 84.2%) are
reconstructed with their full branch logic. The replacement hook walks
the podrace/podsprint/bonus-gunship/gunship/dogfight selections with the
`APICHARACTERDATA` (0x4c stride, name at +0xc) name match through
`apicharsys` (`0x02913104`) and the vehicle chain
(`CutDeadVehiclePlayer` at `0x00951bfc` falling back to `player` at
`0x0093d830`), replacing the ten character id words (all initialized
-1). The bolt hook scales `GAMEOBJECT.target_velocity` (+0xf18) into
the momentum vector with `bolt->speed` (+0xcc) accumulation behind the
`LSW1` gate, and returns 1 for the speederbike/death-star level gates.
`Bolt_HitPlatFn_LSW` (`0x0042dd50`, 35 bytes, 87.9%) now calls the
reconstructed `KaminoE_CheckPlatHit` (`0x004e1460`, 228 bytes, 88.5%):
the bolt's +0xfa word must match the plate id (`0x008890a6`), then the
first of the four Kamino plates (`0x00889058`-`0x00889064`, each gated
by its active flag and the +0x13a flag bits 0x30/0x2) becomes the plate
hit (`0x00889068`); the tail differs by four bytes of scheduling.
`Bolt_HitCustomFn_LSW` still carries its marker text pending
`ChrisExtraBoltCollision` (`0x00412aa0`), and `GameBlowUpBlownUpFn_LSW`
pending the eleven-argument `AddDynamicCreature` (`0x0043d890`).
`KAMINOE_LDATA` (`0x0087b134`) and `DOGFIGHTA_LDATA` (`0x0087b118`) are
written by `Level_FindByName("Kamino_E"/"dogfight_a")` in the
level-handler registration cluster around `0x0048e3xx`-`0x0048e7xx`.

The background-post path is reconstructed. `NuLstAllocTail`
(`0x00547240`, 117 bytes) pops the free list, links the node at the tail
(`NULNKHDR` 0x10 bytes, `NULSTHDR` 0x20 bytes per the Android `nulst`
layout), sets `is_used` and bumps `used_count`, and gates both
`NuThreadDisableThreadSwap`/`NuThreadEnableThreadSwap` calls
(`0x00800820`/`0x00800824`, indirect pointers) behind the
`safe_thread != nu_current_thread_id` test against the volatile current
thread id at `0x00297ad74`. `bgPostRequest` (`0x004fecf0`, 151 bytes)
posts the 0x118-byte `BGPROCINFO` work item through the
`bgproc_procinfo_pool` list (`0x0091f1d4`, the pointer created by
`NuLstCreate(0x10, 0x118)` inside `bgProcInit`), under
`bgproc_critical_section`, copying up to the caller's data size into
`+0x10` with the one-byte `bgProcDataOversize` hook (`0x004fe640`)
notified when the size exceeds `0x100`, then signals
`bgproc_work_event`. The retail keeps the post's `data_size` in `eax`
across the hook call, which requires same-unit visibility of the empty
hook body; our hook lives in `nuvideo.c`, so the reload registers differ
(78% similarity, identical semantics). `bgProcDataOversize` itself is a
one-byte `ret` and is exact. `LoadPerm` now posts `LoadPermData` with a
real `bgPostRequest` call.

`NuLstCreate` (`0x00547060`, 136 bytes, 62%) is reconstructed: it
allocates `count * (element_size + 0x10) + 0x20` through `NuAlloc`
(`0x004eef40`, mapped) with the retail's embedded
`d:\projects\game_saga\nu2api.saga\gamelib\listman.c` call-site tag
(`0x007615b0`, 52 bytes) and category 0x67, then chains the free list
with owner/next/id fields and stores `nu_current_thread_id` as the
header's `safe_thread`. `bgProcInit` now creates the 0x10-slot
`BGPROCINFO` pool for real.

The display-list manager family is mapped with verified extents as
candidates pending their reconstruction: the frame begin
(`0x005dfab0`), the stream allocator (`0x005deb20`), the dynamic
material-list creator (`0x005efbf0`, 1,011 bytes), the per-scene draw
(`0x005f02e0`), the draw driver looping `render_scene_index`
(`0x005f03c0`), the sort-priority processor (`0x005f0470`, 1,611
bytes), the ESI-argument sort-pri draw (`0x005f0ac0`, 1,079 bytes —
private register ABI), the frame draw (`0x005f0d70`), the init at
`0x00603350` and the purge (`0x005dece0`). Their state globals
(`0x00973e50`/`0x00973ea0`/`0x00973ea4`/`0x00973e98`/`0x00973ee8`/
`0x00973eec`/`0x00973ef0`/`0x00973ef4` and the `0x00973700` stream
cursor pointing at `0x0097c1cc`) are already mapped from the
`NuDisplayListAddRenderScene` work.

Two of them are now reconstructed. `NuDisplayListStreamAlloc`
(`0x005deb20`, 139 bytes, 51.8%) reserves four regions of the stream
cursor for `count` items: the cursor bytes, a 4-aligned block, the
count-times-36 sort-pri scratch, and the 16-aligned item base
(`nudlist_item_base`, `0x00973fc0`) followed by count-times-32 item
slots, then clears `count` bytes at the allocation base and writes the
0x84/id-4 terminator into each slot's +0x10 item. `NuDisplayListInit`
(`0x00603350`, 147 bytes, 34.7%) zeroes the manager state, sets
`displaylist_2d_first` to the static item at `0x00973ed4`, requests the
dynamic material list (`0x005efbf0`, still a marker), sets the
needs-build flag bit 0x10 at `0x00973e50`, allocates the 0x40-slot
stream regions, points `NuDisplayListStreamCursor` at `0x0097c1cc`, and
stores the `0x002854ba8` value into `0x00971598` — the remaining
call-site markers (`0x5fd1c0`, `0x4ffa60`, `0x5f0d70`,
`NuDisplayListBeginFrame`) are the mapped-unimplemented callees.

The special-visibility path is reconstructed: `NuSpecialSetVisibility`
(`0x005cfc00`, 184 bytes, 74.7%) handles the legacy and display-special
layouts of the handle (`NULEGACYSPECIAL` instance/flags at +0x40/+0x44/+0x48,
`NUDISPLAY_SPECIAL_PC` flags at +0xb8 and `instance_ix` at +0xc0, and the
scene display-list's visibility-flag byte gate at +0x76), and
`GameCam_NewShake` (`0x00549bb0`, 43 bytes, 90%) stores the three shake
parameters into the camera at +0x1d4/+0x1d8/+0x1dc with the null-camera
`GameCam` (`0x008021b4`) fallback. `AddPartDebris` (`0x00571b60`, 54 bytes,
85.5%) validates the index against `part_debris_count` (`0x00951018`), reads
the 0x14-stride id table at `part_debris_sys`+8, and calls the still-stubbed
part spawner at `0x0064b370` (candidate name
`PartDebrisCreatePart_candidate`, kept in `game_deb.c` so the cross-unit
call survives constant folding). With these in place
`CutScene_StoppedFn_LSW` is exact: it hides the two special handles
`LevHSpecial_50`/`LevHSpecial_51` (`0x0087ad50`/`0x0087ad5c`, standalone
12-byte handles rather than Android's array indexing) and spawns part
debris when the stopped cutscene is `game_cutscenes_cutscene`
(`0x0087660c`).

The math helpers behind the hook family are exact: `NuFsqrt`
(`0x005c0240`, `if (f < 0) return 0; return sqrt(f);` with the CRT
`__CIsqrt` intrinsic bound at `0x006e9530`), `NuVecMag`
(`0x005bec80`, 49 bytes) and `NuVecDist` (`0x005becc0`, 182 bytes; the
two-path form with the delta written to the third argument when
non-NULL). The `NuVecDist` else-path differs from retail only in one
FPU load scheduling choice (x versus y first from the stack delta,
99.97%). The CRT `__CIsqrt` member at `0x006e9530` is inventoried like
`__ftol2_sse` before it.

## External symbols

The original PE import directory supplies 197 four-byte IAT slots: 191 imports by
name and six by ordinal, from 14 DLLs. An IAT address is the executable's pointer
slot, not the address or size of the DLL implementation. `symbols.csv` maps these
slots and 199 verified six-byte local `jmp [IAT]` entry stubs. DLL function bodies
are outside this executable and have no mapped code extent here.

| DLL | IAT slots |
| --- | ---: |
| KERNEL32.dll | 121 |
| USER32.dll | 25 |
| d3dx9_35.dll | 14 |
| binkw32.dll | 12 |
| ole32.dll | 5 |
| ADVAPI32.dll, SHELL32.dll, WINMM.dll, d3d9.dll, XINPUT1_3.dll | 3 each |
| OLEAUT32.dll | 2 |
| DINPUT8.dll, DSOUND.dll, GDI32.dll | 1 each |

The PE has no export, delay-import or bound-import directory and no COFF symbol
table. Its debug directory contains one CodeView reference to
`LEGOStarWarsSaga.pdb`, not an embedded function-symbol table. The parser explicitly
rejects nonempty delay imports until support is added, rather than omitting them.

The six ordinal imports are identified from x86 DLL export tables; their raw
ordinal identities remain in the match report:

| IAT address | Import identity | Export name |
| --- | --- | --- |
| `0x00751018` | `DSOUND.dll!#11` | `DirectSoundCreate8` |
| `0x00751210` | `OLEAUT32.dll!#2` | `SysAllocString` |
| `0x00751214` | `OLEAUT32.dll!#6` | `SysFreeString` |
| `0x007512a4` | `XINPUT1_3.dll!#4` | `XInputGetCapabilities` |
| `0x007512a8` | `XINPUT1_3.dll!#2` | `XInputGetState` |
| `0x007512ac` | `XINPUT1_3.dll!#3` | `XInputSetState` |

`XINPUT1_3.dll` was read from the local game prefix (SHA-256
`9775c7b650a44a92c6b66b1c6932ce483a4f14f519d3136a86dab128935f824a`).
The OLEAUT32 and DSOUND names were checked against the installed Wine x86 DLLs,
with SHA-256 `2bb1a72b4d8b21dc2a8c5b824e16eb613a19558a5557bf0130ee3fc080547e9a`
and `7effb7925a6aea8a58be34ac4b769fb655b98d9f279aab1941e307b1ee093489` respectively.
These names are ABI identifications; the stripped EXE itself records only ordinals.

The local thunk ranges are `0x004eed92..0x004eed98`,
`0x006f05ae..0x006f05ba`, `0x007157c8..0x0071581c`,
`0x0073efb4..0x0073efcc`, `0x00740ac0..0x00740eda`, and
`0x0074fbdc..0x0074fbee` (exclusive ends). `GetCurrentThreadId` and
`GetCurrentThread` have duplicate entry stubs: their normal linker names bind to
`0x00740b62` and `0x00740c46`; the earlier copies use address-qualified
`saga_thunk_...` aliases. An identical jump pattern at `0x00696cf7` occurs inside
a larger function after argument/register work and is deliberately not inventoried
as a standalone thunk. Pattern occurrence alone is not function-boundary evidence.

API declarations and types come from the pinned original VC8 CRT/Platform SDK
headers. APIs absent from that SDK are declared in
[`src/externals.h`](../src/externals.h), included by the shared header. See
[`toolchain.json`](toolchain.json) and the declaration-only
[`src/externals.c`](../src/externals.c). MSVC C and C++ probes verify the linker
spellings generated by those headers. The known API declarations produce
`__imp__...` IAT references; both IAT slots and complete local thunks are mapped. Only the
12 Bink imports still use opaque pointer-slot declarations: their decorated export
names reveal stack-byte counts, not enough information to assert full prototypes.
Their function `ctype` cells remain blank.

The bundled SDK's `winternl.h` omits `__stdcall` on `RtlUnwind`. The supplemental
header corrects this one ABI declaration: the pinned `Kernel32.Lib` contains
`__imp__RtlUnwind@16` and `_RtlUnwind@16`, corroborating the x86 calling convention.

The matcher recognizes mapped IAT slots and import thunks from the verified
original PE and lists them separately under `externals`. They supply relocation
bindings and count toward mapped image coverage; they do not count as reconstructed
functions or matched code/data. A compiled definition of an external slot or thunk
is an error, and `src/externals.c` must emit no definitions. Original-free CI checks
the latter rule; classification against actual import bytes happens locally.

## PC startup and connected gameplay mappings

The current startup map binds `InitSnakes` at `0x00597250` from NuMain's direct
call at `0x00493111`. Its three `NuSpecialFind` calls use the PC scene pointer
at `WORLDINFO+0x13c`, three 12-byte handles, and the distinct `Snake_bit_1/2/3`
strings. `InitCables` is the adjacent direct call at `0x00493105`; its defaults
and `WORLDINFO+0x12c` area comparison establish the typed cable state. Both
entry functions match their full PC extents after relocation.

The AI message system's PC constructor allocates a `0x18`-byte header and
`count * 0x38`-byte records, then resets its two eight-byte linked lists. PC
calls to `NuLinkedListAppend/Remove/GetHead/GetNext` use their confirmed
`0x004f13xx` entries. The message flags are 16-bit at `+0x36`, rather than the
Android declaration's byte. The real list helpers and four AI-message routines
match completely; the other source routines remain marked different in
`matching.json`.

For file-position dispatch, PC file initialization clears 20 `0x30`-byte DAT
records at `0x0297dc80` and 20 `0x14`-byte memory-file records at
`0x02983760`. `NuMemFilePos` at `0x004f6c20` and the distinct three-byte
`NuMcSeek` at `0x006dee90` are exact. `NuFilePos` and `NuFileSize` preserve the
PC handle branches and archive-hit buffering behavior, but are not yet byte
matches. The level/area loader entry names follow `LoadPermData` calls to
`0x005788f0` and `0x00572b00`, with PC record strides `0x130` and `0x9c`;
their current NULL-return sources are scaffolds, not loaded tables.

PC font instructions establish `VUFNT+0x48` as platform data and a matrix at
that data's `+0x10`. `NuQFntMoveRS` (`0x005d10e0`) and `NuQFntSetMtxRS`
(`0x005d0f40`) match complete PC extents. The mode-save array starts at
`0x0096d890`: the preceding word at `0x0096d88c` is separate, as shown by the
push and restore displacements in `NuQFntPrintEx`. Its deeper glyph renderer
and the wide/narrow NuFPar parser remain open dependencies.

`InitOnce` calls the argument iterator at `0x00401100` using ECX/EAX as a
private ABI. An ordinary VC8 `/O2` same-TU static helper reproduces its full
59-byte body with three real relocations when the final stores follow PC order.
The helper now belongs to the InitOnce unit rather than a public stack-ABI wrapper. Its three-byte zero-return callee at
`0x006d3430` is exact. The PC `NuMusic::SetFader` member at `0x00694d50` uses
`this+0x19c/0x1a0/0x1a4` and matches with its real C++ linkage. These are
reconstruction linker names for a stripped executable, not recovered retail
symbol spellings.

The `InitOnce` dispatcher passes token `0x18`, the seven-pointer table at
`0x007ef090`, and count seven. Its PC branch calls `StartupOpenDats` at
`0x006d3440`; the C reconstruction matches all 116 bytes and ten resolved
relocations. That helper installs `GAME.DAT` through `NuDatSet` and the
10-byte `NuSound3SetDat` setter. It does not perform archive-member reads.
`NuDatFileSeek`, `NuDatCalcPos`, and the archive-slot allocators have separate
PC call and boundary evidence; `NuDatFileOpen` is structurally reconstructed
but remains byte-different and is not yet called by the source file-open
dispatcher. The new `GameAnimSys` records use PC strides `0x28`, `0x14`,
`0x1c`, and `0x10` for object, pool, set, and system respectively; its
progress state at `0x00956634` is a 12-byte zero initializer.

The structural movement/animation/string/script batch uses direct PC caller,
field, string and boundary evidence. In particular, `Bolt_Reflect` is
`0x0057fc30` (79 bytes); `0x0057fb30` is a private direction/matrix helper.
`0x00580c00` is the default debris callback, not `Bolt_End`; its full span is
149 bytes through `ret` at `0x00580c94`, retaining nine internal alignment bytes.
The inline bolt-type name is passed as the record base to `NuStrICmp` while
records step by 0xa4. The world-local table begins at `WORLDINFO+0x4b14`.
The owner liveness gate reads the DWORD at `GAMEOBJECT+0x1fc`, sharing a verified
byte view; it does not read `+0x1f8`.

Parser records and string banks use PC strides, and compiled helper bindings
include the actual `PCByteCopy_candidate` dependency for overlap-safe copying.
`AIScriptLoadPakFile` and its context driver use private register arguments;
source markers remain until their callsite integration is reconstructed. Retained
analysis proposals are advisory and have not been promoted solely from graph
scores. Full comparison extents remain in the CSV even for partial routines.

## Camera, path geometry, sample names and bolt pool follow-up

Camera extents and block transforms are bound through their direct PC calls and
complete return boundaries (0x00603f10/497, 0x00604110/456, 0x006042e0/1608).
The historical SSE helper identity remains neutral; its implemented ABI uses
48-byte SoA blocks and full packed outcode words. The one-byte hook at 0x005f7490
is a genuine retail RET, reached by the screen-clipping trap. It aliases the
already-mapped special-transform no-op at the same original address, so it
adds no distinct code coverage. No generated inline
helper is given a fictitious original binding.

Projection helpers at 0x005ed300/282 and 0x005ed420/274 use four cdecl arguments,
64-byte matrix owners and the shared trig table. Full x87 stack tracing resolves
the argument-slot reuse and division direction; neither an invented mode-bit
float reinterpretation nor inverted m32 formula is retained.

WithinConnection at 0x0051b1f0/1532 takes eleven cdecl slots. The verified route
is argument seven and previous connection is argument six. Its cache accesses
AIPATH+0x78, while the connection extent, nodes, widths and height ranges use
existing asserted PC fields. Node updates are real dependencies.

Unpositioned playback entries at 0x00698040/118 and 0x006980c0/62 preserve all
nine/eight caller slots; unused slots are not erased from the signature. The
sample-name method at 0x00696f10/141 is thiscall `(const char *, i32)` with RET 8.
It establishes the 0x400-byte path at sample+4 before the valid byte at +0x404.
Bolt_Alloc at 0x0057fba0 uses its entire 123-byte span, including the three-byte
interior LEA omitted by an earlier noncontiguous research body count. Jitter at
0x005809a0/54 reads the asserted GAMEOBJECT float at +0xb0 and calls C qrand.
The qrand/getqseed/qseed linker names changed to C without changing original
addresses or extents; compare prior exact results by those addresses.

## First plan-execution wave (September 2026)

The accepted movement, GameObject and AI families reuse the frozen clean Android
source graph and ranked hypotheses, then require independent retail entry/ABI,
complete-boundary, field and dependency review. Research packets remain under
`build/analysis/wave1/`; the checked-in C bodies, assertions and CSV are the
reviewable results. Candidate-only large bodies receive no implementation credit.

The PC allocator establishes 64 GameObjects with stride `0x10d8`:
`WorldInfo_Load` pushes that stride at `0x0048d514` into `APIObjectSysInit`, whose
allocation uses 64 slots. The object-system record is `0x218`, with two-word
hostility masks. The embedded packet begins at object `+0x2c8`, while locator
callbacks also access a distinct AI pointer at object `+4`; the partial
`APIOBJECT_PC` view keeps those interfaces explicit. The callback accepts a byte
ID; a dead object's `+0x1010` value must be positive, including rejection of NaN.

AI locator records have stride `0x3c`, locator sets `0x1c`, and PC areas `0x40`
(Android areas are `0x3c`). Path-position source preserves complete connection
searches, route checks and original return paths. The same-TU static reset helper
at `0x0051a760` naturally emits the verified private EAX input and exactly matches
86 bytes. Verified real calls to selected large sibling functions are preserved
with `noinline`; this is distinct from forcing public calling conventions on
private helpers. The sole new AI omission is the route-map diagnostic at
`0x00512860`, marked at the actual action.

The static ForcePush eligibility helper at `0x004a3b60` also naturally uses EAX
for the first object and two stack pointers. Its complete 123-byte retail extent
is mapped without a public prototype. The full ForcePush target selector has no
omitted selection loop, but its first structural build scores zero; a bounded
branch-order/TU audit is required before copying more siblings. Its size remains
1,014 bytes regardless of score. Source shape, rather than byte tweaks, is the
next investigation. `SnapCreaturePos` explicitly leaves surface initialization,
surface placement and character-path query actions unresolved. Source linking
and high similarity do not establish runnable startup or complete game behavior.

The root reconstructed camera sphere clipping, homogeneous vector transformation
and nearest-segment squared distance in the existing engine units. The transform
matches its full 143-byte PC extent; clipping and segment distance remain
structural. All comparisons keep complete extents and actual relocations.

## Second structural execution release

The next accepted closure covers complete character-path geometry, formation,
gravity, player-layer selection, gizmo loading/edit-file parsing, layer-name
lookups and spline-area intersections. PC field cohorts are asserted in saga.h;
unknown regions remain opaque. AddToAIGroup's signed conversion and the
character-goal-speed x87 evaluation order were corrected from actual retail
instructions. The latter matches all 112 original bytes.

CharacterTestPathCnx and FormationMove remain original-TU static helpers. Their
current compiled private register allocation differs from the original while
the caller/callee pairs remain coherent; no fake public ABI wrappers conceal
that difference. RowMoveTowards currently supplies no usable objdiff similarity
measurement and conservatively contributes zero. Full original spans remain
unchanged. A bounded Force selector branch-layout correction raises its previous
zero score without adding or omitting selection behavior.

The plain C random utility unit preserves its original addresses and extents:
ResetSeeds and GAMERAND now have C linker names in the CSV. Compare prior exact
results by their original addresses when following this intentional linkage
conversion; ResetSeeds remains exact. LineIntersectXY retains a genuine
cross-TU call from OutSideSplineArea. Retail runtime probes establish that both
segments include both endpoints: 15 endpoint cases and 16 polygon cases passed,
including null/output handling. An initial endpoint interpretation was rejected
before release. This runtime evidence complements complete byte comparisons;
it does not establish exact instruction matching.

Techno_FindOperator uses the proven eight-player context scan and typed fields.
The engine infinite-line projection preserves the PC zero-length behavior.
Cheats_CheckFlags occupies 13 bytes through its RET, not 14. SnapCreaturePos now
uses AISysGetCharacterPathPos with the typed World AI owner and retains two
surface-action markers. Loader diagnostics that are genuine retail RET callees
are recorded separately from missing resource behavior.

The fourth structural release maps complete PC spans for ordinary AI actions,
takeover and lifecycle processing, socket parsing/geometry, and jump/last-safe
closures. Existing nuspecial lookup definitions migrate to their original TU;
comparison follows the same original addresses. Two-byte contexts and counters
retain their actual PC widths. Full comparisons and existing exact matches are
preserved; large partial bodies contribute their measured low scores.

NuMtxVecToEulerXYZ (0x005bf4f0, 233 bytes) has five ordinary pointer arguments and
three genuine angle/rotation operations. NuMtxGetEulerXYZ (0x005bf5e0, 81 bytes)
copies the first and third matrix basis rows and is exact. ForceTowardsMid
(0x00498d40, 1,057 bytes) belongs to the pushing subsystem, uses unsigned candidate
count and named socket/matrix fields, and currently scores 68.62%. NuQuatSlerp
ends at the RET at 0x005b8987: its 488-byte extent does not include the adjacent
weighted-quaternion routine. The retained graph's EnforceSockYLimits candidate
at 0x005405f0 was rejected: the actual routine is a four-vector perspective
interpolation provider and has not been implemented or mapped under that name.

## Reviewed graph and BSim mapping release (October 2026)

Five functions entered the accepted inventory after original PC identity, caller
ABI and complete-boundary review. This adds 1,064 known function bytes without
adding compiled definitions or claiming reconstruction progress.

| PC entry | Identity | PC bytes | Named Android entry |
| --- | --- | ---: | --- |
| `0x0055b6c0` | `IconScene_FindById` | 29 | `0x004604c0` |
| `0x0055b830` | `DrawCharIcon` | 641 | `0x004606a0` |
| `0x005930c0` | `Customiser_DumpAccessories` | 211 | `0x004a0760` |
| `0x005f3270` | `NuAnimData2LoadBuffEx` | 154 | `0x002bc3d0` |
| `0x005f3310` | `NuAnimData2LoadBuff` | 29 | `0x002bc490` |

The character-icon family uses a ten-slot cdecl draw contract, separate frame and
character alpha, a special-handle override, and the bounded character scene-table
lookup. The PC draw body omits Android's extra spin, scale and depth globals.
The animation loader aligns its destination to sixteen bytes, distinguishes the
ANI5 format through the signed header comparison, and follows the independently
mapped file-loading, pointer-fixing and animation-fixing calls. Its three-slot
wrapper forwards a local output pointer as the Ex loader's fourth argument. The
PC empty-file path also calls a one-byte diagnostic return absent on Android.

The Customiser routine uses the reviewed accessories-state guard, two sides and
nine categories, and restores textures or removes scenes. Its opaque pointer
contract is established; complete Customiser layout remains unresolved.
`Customiser_AccessoriesLoaded` at `0x00955894` now has an identity-only inventory
row. Its size and type stay blank: observed DWORD reads/writes do not prove the
complete object's extent. The unimplemented `_Name` bindings are reconstruction
linker names, consistent with existing C bindings; the stripped PE does not prove
historical source linkage.

The [review journal](identification-decisions.jsonl) retains these decisions and
the following data corrections. Android `numtx_identity` at `0x00625570` serves
two distinct PC allocations: the set-identity template at `0x00806680` and the
default primitive matrix at `0x0080e270`. They are semantic correspondences with
separate storage, and their original PC names and extents remain separate.
Android `render_state` at `0x0119b900` corresponds to PC base `0x009868c0`;
`0x009868e0` is its field at offset `0x20`, so pairing that field with the whole
Android aggregate is rejected. The original copy operations use 420 PC bytes
and 432 Android bytes, confirming why their layouts cannot be transferred.

Five already-mapped callbacks were also confirmed as genuine missing function
nodes in the retained PC graph: `Condition_Timer`, `Condition_Random`,
`Condition_AlwaysTrue`, `CheckOutputConditionFlowBox`, and `ProcessActionFlowBox`.
Their complete 244 bytes add no map coverage. The Random tail transfer and the
action callback's direct call are verified; table-mediated dispatch remains
conditional. Their entries and four further Flow callbacks were repaired in an
independent project copy with exact reviewed bodies. The re-export preserves all
existing function bodies, and retained project content and metadata are unchanged.
The nine callbacks total 537 already-mapped bytes; they add no coverage.

Detailed operands, callers and terminal instructions remain under ignored
`build/analysis/identification/continuation/`, in `bsim-review`, `adjacent-review`,
`data-review` and `callback-review` JSON/Markdown packets. Python review processes
used 256 MiB limits; full scoring uses the serialized 1 GiB guard.

`MovePlayer_DIRECTIONAL` is now mapped at `0x004ac620`, with the complete
10,904-byte extent ending at `0x004af0b8`. Its `CurrentAnim` and
`Techno_FindOperator` entry, inactive-vehicle handling, `GrapWindLp` sound,
input-angle processing, target-velocity seeking, lean limits and terminal
`GizmoBlowupCheckProximity` call match the named Android routine at `0x001670d0`.
The accepted `MovePlayer` caller at `0x00416ab3` passes one object pointer and
cleans four bytes; both bare returns leave incidental ignored results, supporting
the source's `void` contract. Every original instruction is reachable, local
branches stay on instruction boundaries, and returns at `0x004ac71d` and
`0x004af0b7` close all paths. Following alignment bytes are excluded. The C linker
name is a reconstruction choice. Existing `GAMEOBJECT` field contracts support
the pointer type; its entire layout remains incomplete. This is an unimplemented
mapping, adding no fuzzy or exact-match credit. The ignored
`continuation/movement-directional-review` packet preserves the original CFG and
caller evidence.

`SetLevelSfxBits` is mapped at `0x004bde10`, with 8,845 original bytes through
the return at `0x004c009c`. `WorldInfo_Init` passes its world pointer at
`0x00560209`. The ordered sound literals, level-dependent crane/hover/magnet
choices, arcade/character conditions, terminated ID list and
`SetSfxBit_On`/`SfxBitsStore` epilogue corroborate the named Android routine at
`0x00145ad0`. Original branches remain inside the reviewed complete extent;
alignment after its sole return is excluded. Its one-pointer `void __cdecl`
signature is established, while several aggregate field cohorts still require
review. For example, the first sound ID and count occur at PC world offsets
`0x471c` and `0x4b10`, versus Android `0x4720` and `0x4b14`. The function remains
unimplemented.

The same review identifies three four-byte `LEVELDATA_PC *` slots:
`CLOUDCITYESCAPEC_LDATA` at `0x0087b240`, `CLOUDCITYTRAPA_LDATA` at
`0x0087b244`, and `JABBASPALACEB_LDATA` at `0x0087b2a4`. Original writers call
accepted `Level_FindByName` with the exact corresponding names and store its
pointer result at `0x0048f0cd`, `0x0048efc3`, and `0x0048f131`. Independent sound
branches compare the world level against those same slots. The reviewed extent
is each complete pointer cell, with original zero initialization; it establishes
no pointee layout. Android counterparts at `0x006a1460`, `0x006a145c`, and
`0x006a13f4` remain in their original order. The ignored
`continuation/set-level-sfx-review` packet preserves the writer, reader and
boundary evidence. These rows add mapping coverage, with no matching credit.

Five further four-byte level-pointer cells were independently reviewed in the
same writer family: `CLOUDCITYESCAPEA_LDATA` at `0x0087b238`,
`JABBASPALACEA_LDATA` at `0x0087b2a0`, `JABBASPALACED_LDATA` at `0x0087b2a8`,
`JABBASPALACEE_LDATA` at `0x0087b2ac`, and `ENDORBATTLEA_LDATA` at `0x0087b2b4`.
Exact named `Level_FindByName` calls store their returned pointers, and separate
`SetLevelSfxBits` branches compare those cells while selecting the expected
steam/control-desk, disco, minecart or lantern sounds. Parent reinspection
confirmed each original literal, absolute return store, comparison and zero
initializer. Android counterparts are `0x006a1468`, `0x006a13f8`, `0x006a13f0`,
`0x006a13ec` and `0x006a13e4`. The ignored `continuation/level-pointer-frontier`
packet records these contracts. The separate particle-reader review below resolves
`CLOUDCITYTRAPB_LDATA` at `0x0087b248`. No
pointed-to aggregate layout or additional Android sound branch is transferred.


`PartKill_ForceThrow` is mapped at `0x0042d000`, with 371 original bytes
through the return at `0x0042d172`; the following thirteen INT3 bytes are excluded.
The level-specific debris IDs, special-name selection, rumble, qrand threshold,
camera shake and `Explode1` call establish identity independently of the previously
held level-pointer name. All local branches land on decoded instructions within
the complete body. The named Android function is at `0x00121600`, with a different
630-byte extent that is not transferred to PC.

Its two-argument `void __cdecl (PART_PC *, i32)` contract follows original
runtime callback initialization and dispatch. The initializer at `0x00467a35`
writes the callback into AddPart parameter offset `0x44`; accepted AddPart copies
that field to PART offset `0x1c0` at `0x0064d790`. Accepted KillPart loads that
callback at `0x0064acec`, pushes the reason and PART pointer, calls it at
`0x0064acfd`, and cleans eight bytes. The callback does not use the reason;
its bare return and ignored result support the void contract. Complete PART
layout remains unresolved. The ignored `continuation/force-throw-review` and
`force-throw-abi-closure` packets preserve original range hashes and instructions.
Parent review independently verified the complete body and dispatch windows.

This independently identified reader compares current_level against
`CLOUDCITYTRAPB_LDATA` at `0x0042d012`. The original lookup writer pushes exact
`CloudCityTrap_B` at `0x0048efed`, calls Level_FindByName, and stores returned EAX
into `0x0087b248` at `0x0048effc`. Together these establish the complete four-byte,
zero-initialized `LEVELDATA_PC *` cell, corresponding to Android `0x006a1458`.
No pointee layout is inferred. Both new rows are inventory mappings without
source definitions or reconstruction credit.


The sound literal `GrapWindLp` at `0x0075bf90` is mapped as the complete
`char[11]` array, through its first NUL at `0x0075bf9a`. Original PlaySfx argument
sites at `0x004ac76e` and `0x004d6875` both pass that exact start. Its preceding
qword at `0x0075bf88` is independently read by `0x004ae825`; the five following
alignment bytes and next qword at `0x0075bfa0` are excluded. Ghidra's string
starting at `0x0075bf8f` incorrectly absorbs the preceding double's last byte
and is not the accepted storage boundary.

The named Android directional function resolves the same literal at `0x00567b17`:
its original thunk call returns `0x001670db`, adds `0x004af795` to establish EBX
`0x00616870`, then its LEA at `0x0016792a` selects the literal before PlaySfx.
Bounded whole-function register-flow review verifies that PIC base reaches the
argument, subject to the standard SysV callee-preserved EBX convention. No ELF
relocation overlaps those selected operands. Multiple zero-size LC aliases do
not establish a unique source translation unit. `_GrapWindLp_text` is a
prospective reconstruction name, without a compiled definition. The ignored
`continuation/grap-wind-literal-review` and `grap-wind-pic-register-proof` packets
preserve the byte contracts and PIC proof.


`ZipUp_MoveCode` at `0x004d6060` is mapped with a complete 3,774-byte PC body,
ending after the return at `0x004d6f1d`. All local jumps remain on decoded
instruction boundaries; thirteen returns close the paths, and two following
INT3 bytes are excluded. Context `0x47`, the three-stage animation route,
hook timeout/detachment and hint `0x262`, the swing arc and sound threshold,
and nearest-zipup/StartBigJump eligibility establish identity independently of
the shared GrapWindLp literal. The named Android function at `0x001d2750`
has a different 4,982-byte ELF extent.

Four original callers pass the object pointer and special input. In particular,
`0x004b3ca6` pushes the input, `0x004b3ca7` pushes the object, and the direct call
at `0x004b3ca8` is followed by eight-byte cleanup. The callee reads both stack
arguments and uses plain returns; callers ignore the result. This establishes
`void __cdecl (GAMEOBJECT *, i32)` without asserting full GAMEOBJECT or ZIPUP
layouts. Parent byte/branch/caller reinspection agrees with the ignored
`continuation/zipup-review` packet. This remains an unimplemented inventory row.


`Action_RemoveThrownForceObjects` is recovered at `0x00449e10` from the original
named action-definition row at `0x007f76a8`: the twelve-byte row contains the
`RemoveThrownForceObjects` string pointer, callback entry, and zero flags. That
static initializer supplies a real disassembly seed despite the missing retained
function node. The original body tests each active PART's kill callback against
accepted PartKill_ForceThrow, calls KillPart(part, 0), advances by PC stride
`0x220`, and returns one. Every instruction is reachable; the complete 75-byte
body ends after the return at `0x00449e5a`, excluding subsequent INT3 padding.

Accepted AIScriptProcess loads the action callback at definition offset four,
pushes its seven established interface arguments, calls through EAX at
`0x0052cf4f`, cleans 28 bytes, and tests the result. This establishes the existing
seven-argument cdecl action contract without inferring ignored parameters solely
from Android. Android's named function at `0x00188120` uses PART stride `0x224`,
which is not transferred to PC. Parent reinspection verified the original table
row, literal, body and dispatcher window. The ignored
`continuation/force-remove-frontier` packet retains this proof. No whole action
table, PART pool extent, aggregate layout or source definition is added.


Three more ZipUp sound arrays are reviewed: GrapDetach at `0x0075c1d4`
(`char[11]`, Android `0x0056797a`), GrapAttach at `0x0075c1e0` (`char[11]`,
Android `0x0056796f`), and GrapSwing at `0x0075d464` (`char[10]`, Android
`0x0056c161`). Original ZipUp argument pushes use those exact PC starts;
full Android ZipUp PIC flow resolves the corresponding LEA addresses into its
PlaySfx arguments, subject to the usual SysV callee-preserved EBX convention.
Each extent ends after its first NUL; subsequent alignment and enclosing
allocations are excluded. The PC Attach/Detach alignment differs from Android.

GrapSwing's old Ghidra string begins at `0x0075d463`, absorbing the final byte
of the preceding float. Original `FLD DWORD [0x0075d460]` at `0x004d5343`
establishes that separate four-byte scalar; following independent floats begin
at `0x0075d470` and `0x0075d474`. The disposable typed-data repair replaces the
incorrect thirteen-byte string with the float and exact ten-byte character
array, preserving original bytes and leaving alignment unclaimed. Parent review
rechecked each literal's hash and exact pointer push. The ignored
`continuation/zipup-literal-frontier` packet contains storage/PIC evidence.
The three prospective `_Name_text` bindings remain unimplemented mappings.


Six adjacent original AI registry rows recover four further unique callbacks:

| Identity | PC entry | Complete PC bytes | Named Android entry |
| --- | --- | ---: | --- |
| Action_SetFormationCommander | `0x00449cf0` | 282 | `0x001881a0` |
| Action_AlwaysTriggerObstacle | `0x0044e8f0` | 186 | `0x001902b0` |
| Action_CanTriggerObstacle | `0x0044e9b0` | 186 | `0x00190190` |
| Action_PlayGizObstacle | `0x0044ea70` | 362 | `0x00191bc0` |

The original named twelve-byte rows seed these entries. Formation commander
selects the last member, checks count/remainder, chooses a character ID and
copies the new hitpoints. The two obstacle toggles parse name=/FALSE and insert
distinct bits four/eight. PlayGizObstacle parses direction, persistent state and
SNAP, dispatches playback or instant jump, and updates those same flags. Their
PC DWORD flag field is at `0x9c`; Android uses a byte at `0xa0`. Formation packet
and actor fields also differ; no complete layout or dependency identity is
transferred. Original AIScriptProcess establishes the shared seven-argument
cdecl action interface, and each body's used arguments and balanced returns
corroborate it. Parent inspection verified all table bytes, literal names,
function hashes and complete reachable instruction sets; following alignment is
excluded.

PlayGizObstacle and PlayObstacle are two registry names for one physical callback
in both shipped binaries. The reconstructed Android source has NULL placeholders,
but the original ELF rows share the named Action_PlayGizObstacle function.
Only one function row is accepted. The ignored `continuation/action-registry-frontier`
packet records this source gap and the three missing PC graph entries. The four
functions add 1,016 inventory bytes without implementations or match credit.

The obstacle action's original directional/SNAP branches independently seed
four animation wrappers: `GizObstacle_PlayForwards` at `0x00557960` (111 bytes),
`PlayBackwards` at `0x005579d0` (119), `JumpToStart` at `0x00558970` (25), and
`JumpToEnd` at `0x00558990` (25). Their named Android originals are respectively
`0x004ab6c0`, `0x004abd80`, `0x004acc90`, and `0x004accc0`. Original callers pass
one obstacle pointer; playback's incoming ECX push only reserves a float slot
that is overwritten before use. Every local return restores ESP. The jump
wrappers replace the stack argument with `anim_set` and terminate in explicit
tails to the already mapped animation-set helpers; their extents exclude those
helpers. All instructions are reachable and hashes cover each complete body.
These four inventory rows add 280 bytes without compiled implementations.

Six character-ID storage identities are accepted at `0x007f1888` through
`0x007f189c`, with four-byte spacing: `id_BATTLEDROID`,
`id_BATTLEDROIDCOMMANDER`, `id_BATTLEDROIDGEONOSIAN`,
`id_BATTLEDROIDSECURITY`, `id_CLONEEP3`, and `id_CLONEEP3SAND`.
Exact original name/fixup rows, the startup resolver's WORD store, and independent
formation comparisons establish their semantic identities and signed 16-bit
accessed values. They do not establish complete PC allocation extents or original
declarations. Every CSV size and ctype stays blank; journal type/extent flags are
false and there are no complete ranges, source definitions or coverage credit.
Android's two-byte globals and Ghidra's analysis-only undefined2 owners do not
prove PC object boundaries. The ignored `continuation/formation-id-independent-audit`
records this limitation and exact-address owner checks.

`GizActions_PlayObstacle` at `0x00465590` adds a complete 389-byte inventory
body. The original eight-byte gizmo-action definition at `0x007f8b88` binds
`PlayObstacle` to this entry; the Android original binds the same action to
`0x001c37e0`. The actual flow dispatcher pushes flow, box, params and count
and cleans sixteen bytes. All 124 PC instructions are reachable, every branch
closes within `[0x465590,0x465715)`, and every return restores ESP. Direction,
SNAP, stay-open/shut tokens select the four reviewed playback helpers; PC flags
use DWORD `+0x9c`, unlike Android's BYTE `+0xa0`. The obstacle type-ID reader at
`0x00802644` is a pending data dependency; no allocation extent is inferred.
The ignored `continuation/obstacle-dependency-next` retains the original registry,
dispatcher, body hash and counterpart evidence. No implementation is claimed.

The original registration initializer seeds `GizObstacles_RegisterGizmo` at
`0x00559430`, with a complete 318-byte body and Android counterpart `0x004ad4c0`.
The actual registration dispatcher passes one index, cleans four bytes and uses
the returned configuration pointer. The PC callback copies 120 bytes, returns
`0x0093b210`, installs the original `GizObstacle` name and callback fixups, and
stores the index to `0x00802644`. Android copies 124 bytes; its record size is not
transferred. All 44 PC instructions are reachable and the final return restores
ESP. The existing private configuration contract supplies the opaque return
type; dependent callback bodies and static allocations remain separate work.

`obstacle_gizmotype_id` at `0x00802644` is an identity-only storage mapping to
Android `0x006682e0`. Independent AI/flow DWORD readers, the registration writer,
and the actual Android relative-GOT slot prove its role. Ghidra undefined4 and
four-byte accesses do not establish the full PC allocation. CSV size and ctype
remain blank; journal type/extent flags are false, without complete ranges or
coverage credit. The ignored `continuation/obstacle-id-frontier` preserves this
limitation and original provenance.

The complete original AI registries independently seed sixteen more callback
identities, adding 2,677 PC bytes. Namespace and name come from the original
records and registration/dispatch code; neither existing CSV labels nor Android
record ordinals supply identity. All original PC extents and seven-argument
caller contracts receive independent CFG/stack review. Complete aggregate
layouts remain unproved, so journal type flags remain false.

The later mixed-body review supersedes the instruction-only obstacle output
extent proposals. Complete extents are 168 and 72 bytes, including their five-cell
local switch tables and internal alignment. Original full-range hashes and all
guarded case targets are independently checked. Journal replacement decisions
explicitly supersede the earlier extent decisions; ABI identities are unchanged.

`Action_SetSide` at `0x444940` maps to original Android `0x19c510`, with the
existing seven-argument callback contract and false aggregate type-review flag.
Its full 1,496-byte PC extent contains 1,463 instruction bytes, one NOP and two
16-byte local switch tables. Both unsigned guards, all eight table targets,
default paths and four balanced plain returns are independently resolved. The
following eight INT3 bytes are excluded. Its existing analysis body omits the
tables; an explicit old-body replacement pins that difference rather than
truncating the map to the reachable code. The ignored `ai-set-side-boundary-review`,
`obstacle-registration-local-table-extent-review` and independent mixed-extent
audit retain original ownership and complete boundary evidence. No source
implementation or reconstruction gain is claimed.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| SetAnimation | `0x44e860` | 91 | `0x18b660` |
| AnimTimeRandom | `0x44e8c0` | 47 | `0x18b610` |
| CanOpenDoors | `0x44ede0` | 139 | `0x1853e0` |
| CanShootOffScreen | `0x44ee70` | 139 | `0x185310` |
| KeepWeaponOut | `0x44ef00` | 190 | `0x19da00` |
| SnapWeaponOut | `0x44efc0` | 319 | `0x19d820` |
| ResetContext | `0x44f100` | 161 | `0x19d750` |
| PrefersPlayers | `0x44f1b0` | 113 | `0x185280` |
| SetBoltsDontGetDeflectedBack | `0x44f230` | 100 | `0x1851f0` |
| CanShootObstructions | `0x44f2a0` | 113 | `0x185160` |
| UseBigJumpToJump | `0x44f320` | 113 | `0x1850d0` |
| SetTaggable | `0x44f930` | 342 | `0x19d570` |
| CatchUpForbidden | `0x44f860` | 193 | `0x19d450` |
| CannotDropIn | `0x44fa90` | 249 | `0x19d300` |
| CanAttack | `0x44fc50` | 184 | `0x19d1e0` |
| NotWithParty | `0x44fb90` | 184 | `0x19d0c0` |

`SetTaggable` has a complete 342-byte contiguous PC extent including three
reviewed interior alignment bytes. Its retained analysis body has 339 bytes
in two ranges; that sparse analysis does not shrink the reviewed original
function. The other fifteen entries require disposable graph repair. This
wave adds no implementations or reconstruction credit. Both frozen packets
`continuation/ai-registry-new-callback-batch` and its independent audit retain
original registry rows, hashes, behavior, platform flag-width differences and
full boundary/ABI evidence.

The next original-registry wave accepts sixteen additional callbacks (3,574 PC
bytes) after independent identity, seven-argument caller ABI, CFG/stack and
complete-boundary review. Full aggregate types remain unproved.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| TakeDamage | `0x44fd10` | 549 | `0x19cdc0` |
| CanHitForceObjects | `0x44f580` | 100 | `0x185040` |
| AlwaysBackFlip | `0x44f5f0` | 113 | `0x184fb0` |
| PlayerSpeederHack | `0x44f670` | 100 | `0x184f20` |
| SetAnimSpeedMul | `0x44f6e0` | 370 | `0x185930` |
| CanDefend | `0x4445c0` | 110 | `0x184e80` |
| DontAimAt | `0x444630` | 110 | `0x184d40` |
| CanUseWeapon | `0x444850` | 110 | `0x184ca0` |
| SetBoss | `0x4448c0` | 113 | `0x184c00` |
| SetDontMove | `0x445b70` | 180 | `0x199700` |
| DontSetStoppedFlag | `0x445c30` | 180 | `0x1995e0` |
| SetInvulnerable | `0x449440` | 565 | `0x1992b0` |
| DontPush | `0x4497b0` | 435 | `0x199020` |
| DontAvoidCharacter | `0x449680` | 291 | `0x198eb0` |
| SetZeroAcceleration | `0x449bb0` | 215 | `0x18f720` |
| BreakFormation | `0x449c90` | 33 | `0x183d20` |

The game `DontPush` callback at `0x4497b0` uses reconstruction name
`Action_GameDontPush`; the already accepted API `Action_DontPush` at `0x516420`
remains a separate callback. Original registration and dispatch establish both
namespaces. Mask widths, aggregate offsets and Android extents are independently
reviewed rather than transferred. Five PC entries already have complete retained
bodies; eleven missing entries receive disposable graph repair. The ignored
`continuation/ai-registry-new-callback-batch2` and independent audit retain original
records, hashes and complete ABI/extent evidence. No implementation or matching
gain is claimed.

Original obstacle registration fields and their runtime consumers independently
identify `GizmoObstacle_GetGizmoName` (`0x558770`, 14 bytes),
`GizmoObstacle_GetOutput` (`0x558780`, 168 bytes), and
`GizmoObstacle_GetOutputName` (`0x558830`, 72 bytes). Actual consumers pass one,
three and two stack arguments respectively, clean their calls and use the
returned character pointer or integer. Both five-case indirect switches have
guarded indices and independently resolved original targets. Their complete
extents include local tables and internal alignment after terminal instructions;
the following eight INT3 bytes are excluded. Partial aggregate layouts keep
function type-review flags false.

The name callback returns the obstacle record base as character storage; it
does not load a pointer-valued name member. Output checks use PC DWORD progress
flags at `+0x94` and proximity at `+0x98`; Android uses a byte at `+0x98` and a
DWORD at `+0x9c`. These platform differences remain explicit. The six-byte
output-count callback at `0x558880` remains held for missing original caller ABI
evidence despite its strong configuration-field identity.

`GizObstacleName_text` at `0x761810` has a separately reviewed complete
`char[12]` literal contract, adding twelve mapped bytes. Original pointer cell
`0x802710`, registration stores and the actual `NuStrCpy` source argument prove
its use. The exact first-NUL span starts at its own owner, with a preceding NUL
and a separately accessed following `MAXGIZ...` string. This proves the literal
subobject, without assigning the enclosing string pool's allocation. The ignored
`continuation/obstacle-registration-dependencies` and independent audit preserve
both original binaries, initializer/consumer operands, switch targets and bounds.
These three function identities and one literal add 266 mapped bytes, without
implementations or reconstruction credit.

Sixteen further original-registry callbacks covering 2,042 PC bytes pass the
same independent identity, seven-slot caller ABI and complete-extent checks.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| ImmuneToKillTerrain | `0x4446a0` | 113 | `0x184470` |
| ImmuneToBolts | `0x444720` | 113 | `0x1843d0` |
| Respawnable | `0x4447a0` | 165 | `0x1842f0` |
| IgnoreShoveSystem | `0x44ca80` | 110 | `0x1846f0` |
| CannotBeForcedBack | `0x44caf0` | 113 | `0x1845b0` |
| CanTurn | `0x44cb70` | 113 | `0x184510` |
| NoIdleSpeed | `0x44cbf0` | 211 | `0x1979b0` |
| CannotBeSeen | `0x44ccd0` | 110 | `0x184650` |
| IgnoreTurnAroundSpline | `0x44f3a0` | 113 | `0x183f30` |
| CanMoveWhenDeactivated | `0x44f420` | 100 | `0x183ea0` |
| DontAttack | `0x44f490` | 100 | `0x183e10` |
| CanPullLevers | `0x44f500` | 113 | `0x183d80` |
| UseTimeBasedUpdate | `0x455290` | 141 | `0x1841a0` |
| WalkBackwards | `0x455320` | 201 | `0x1867b0` |
| CanHelpWithTriggers | `0x456490` | 113 | `0x184100` |
| CanCollideWithObjects | `0x456510` | 113 | `0x184060` |

All sixteen entries were missing from the retained graph. Disposable repair
checks the current listing for conflicts before creating their complete bodies.
`Respawnable` retains asymmetric original flag semantics: PC sets `0x200000`
and clears `0x100000`, while Android sets `0x20` and clears `0x10`. This is not
a generic toggle of one bit. Other masks and offsets are reviewed separately
for each binary. Full aggregate types remain unproved; type flags stay false.
The ignored `continuation/ai-registry-new-callback-batch3` and independent audit
preserve original hashes, registry records and full CFG/ABI evidence. No source
implementation or reconstruction gain is claimed.

The fourth new AI callback batch accepts fifteen complete original-registry
identities covering 2,213 PC bytes, with independently reviewed seven-slot ABI
and full extents. Aggregate type flags remain false.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| PressSpecialButton | `0x4492e0` | 163 | `0x1849e0` |
| PressTagButton | `0x449390` | 50 | `0x183c20` |
| PressActionButton | `0x4493d0` | 60 | `0x183c60` |
| UseWeapon | `0x449410` | 45 | `0x183cb0` |
| PressJumpButton | `0x449970` | 50 | `0x183ce0` |
| FormationMove | `0x449cc0` | 33 | `0x183d40` |
| Launch | `0x449f40` | 47 | `0x1934d0` |
| SetCurrentSpeed | `0x44aac0` | 382 | `0x1980b0` |
| SetWalkSpeed | `0x44b0e0` | 107 | `0x185890` |
| UseCurrentSpeed | `0x44c2a0` | 336 | `0x188cd0` |
| SetHoverPhase | `0x44c1d0` | 201 | `0x184830` |
| ApplyGravity | `0x44ca00` | 113 | `0x184790` |
| SetControlSystem | `0x449a50` | 109 | `0x184940` |
| SetHitPoints | `0x44b150` | 316 | `0x197ec0` |
| SetShieldHitPoints | `0x44b290` | 201 | `0x197d90` |

All fifteen accepted entries need disposable graph repair. Original runtime
namespace, name pointers, callback pointers, hashes and both CFGs are retained in
`continuation/ai-registry-new-callback-batch4` and its independent audit. Its
original SetSide hold is resolved separately by the full mixed-table review;
that frozen packet is preserved unchanged. No source implementation or matching
gain is claimed.

The fifth new AI callback batch accepts sixteen complete original-registry
identities covering 5,570 PC bytes, with the same independently reviewed
seven-slot ABI and conservative aggregate type flags.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| Activate | `0x4442f0` | 412 | `0x19f340` |
| DeActivate | `0x444490` | 295 | `0x19f180` |
| SetPath | `0x44e160` | 388 | `0x19ef50` |
| GoToOriginalPath | `0x44e580` | 724 | `0x18c100` |
| SetLocator | `0x445cf0` | 745 | `0x194960` |
| SetLocatorSet | `0x445fe0` | 115 | `0x190cf0` |
| SetDoomedEscapeLocator | `0x446b30` | 608 | `0x19db20` |
| SetStateArea | `0x444f20` | 651 | `0x193af0` |
| GrabVictim | `0x448ed0` | 168 | `0x19ac40` |
| EatVictim | `0x448f80` | 40 | `0x18ab70` |
| ReleaseVictim | `0x448fb0` | 40 | `0x18ab30` |
| UpdateSockPos | `0x4451b0` | 113 | `0x184b60` |
| SetAIOverrideControl | `0x445910` | 403 | `0x1855e0` |
| SetLastSafePathPos | `0x445ab0` | 179 | `0x18f870` |
| AddToSet | `0x4499b0` | 151 | `0x18ff10` |
| SelectRandomSpline | `0x449f70` | 538 | `0x193520` |

`Action_GameSetLocator` at `0x445cf0` is the game namespace callback; it stays
separate from the existing API `Action_SetLocator`. Original namespaces and
callback records provide identity, rather than reconstruction names. SetPath
has a full 388-byte extent including six interior LEA alignment bytes; its
retained sparse analysis body has 382 bytes and does not shrink that map.
Eleven entries require disposable graph repair; five already exist. The frozen
`continuation/ai-registry-new-callback-batch5` and independent audit retain both
original hashes, row/relocation proofs and full CFG/boundary evidence. No source
implementation or reconstruction gain is claimed.

Sixteen AI registry name-storage identities now have independently reviewed
original record pointers, exact first-NUL content hashes, runtime name consumers
and graph endpoints on both sides. Full allocation extents and storage types
remain unproved: CSV sizes/types are blank, journal extent/type/ABI flags are
false, and no complete ranges or byte credit are assigned.

| Registry name | PC storage | Android storage |
| --- | --- | --- |
| GoToLevelPath | `0x756aac` | `0x569ed0` |
| SetAnimation | `0x7569ec` | `0x569f75` |
| AnimTimeRandom | `0x7569dc` | `0x569f82` |
| CanOpenDoors | `0x7569cc` | `0x569f91` |
| CanShootOffScreen | `0x7569b8` | `0x569f9e` |
| KeepWeaponOut | `0x7569a8` | `0x569fb0` |
| SnapWeaponOut | `0x756998` | `0x569fbe` |
| ResetContext | `0x756988` | `0x569fcc` |
| PrefersPlayers | `0x756978` | `0x569fd9` |
| SetBoltsDontGetDeflectedBack | `0x756958` | `0x569fe8` |
| CanShootObstructions | `0x756940` | `0x56a005` |
| UseBigJumpToJump | `0x75692c` | `0x56a01a` |
| SetTaggable | `0x756920` | `0x56a02b` |
| CatchUpForbidden | `0x75690c` | `0x56a037` |
| CannotDropIn | `0x7568fc` | `0x56a048` |
| CanAttack | `0x7568f0` | `0x56a055` |

The first Android export stopped at its 512 MiB heap limit without publishing.
A serialized 1 GiB retry on an independent project copy added only sixteen exact
original-VA data sites; all 266,598 previous node records and all 248,675 edges
remain identical. No data definitions, enclosing allocations, users or pointer
edges are invented. The original project is unchanged. These inventory names
do not establish reconstruction gain or ranking-quality improvement. Ignored
`ai-registry-name-data-frontier`, endpoint evidence, whole-graph preservation
audit and independent identity review retain provenance and limitations.

The sixth and seventh new original AI registry batches accept sixteen callbacks
covering 10,325 PC bytes. Independently reviewed runtime namespaces, named record
pointers and Android relocations supply identity; actual PC dispatcher calls prove
the seven stack slots and return use. Full original PC CFGs and range hashes
establish extents. Complete aggregate types remain unproved.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| SnapToLocator | `0x446060` | 1001 | `0x19eaa0` |
| SnapToOrigin | `0x447030` | 504 | `0x19e880` |
| BigJumpToLocator | `0x446450` | 698 | `0x19e380` |
| BigJump | `0x446710` | 1047 | `0x19de40` |
| SnapToPosition | `0x446d90` | 663 | `0x1945f0` |
| SnapToSockPosition | `0x447230` | 1413 | `0x18ba40` |
| EngageObject | `0x448bf0` | 735 | `0x18abb0` |
| TagCharacter | `0x44ff40` | 239 | `0x19cca0` |
| UseForce | `0x450030` | 627 | `0x18a750` |
| ForcePush | `0x450340` | 521 | `0x19a970` |
| DeflectPlayersPart | `0x450550` | 113 | `0x184ab0` |
| Kill | `0x445230` | 1236 | `0x199820` |
| Explode | `0x445710` | 38 | `0x18a710` |
| SetSpline | `0x449ac0` | 236 | `0x18a510` |
| LaunchGuidedMissile | `0x44bed0` | 759 | `0x188eb0` |
| AddPartDebris | `0x44b950` | 495 | `0x197ad0` |

Ten missing entries are repaired on independent project copies, preserving every
prior function extent. Kill retains its complete 1,236-byte mapped span although
its analyzed body excludes three interior LEA padding bytes; SetSpline includes
its corresponding three-byte alignment. Neither distinction licenses truncation.
Frozen `continuation/ai-registry-new-callback-batch6` and `batch7`, with their
independent audits, retain original provenance. These remain inventory entries.

Seven further GizObstacle callbacks cover 732 complete PC bytes. Actual original
initializer writes, config-to-runtime slot copies, named Android callback symbols
and both bodies establish identity. Original PC consumers independently prove
one-, two- or three-slot callback contracts, including unused incoming arguments.
The PC configuration name begins with NUL; no literal PC type-name claim is made.

| Callback | PC entry | Complete bytes | Named Android entry |
| --- | --- | ---: | --- |
| GizObstacles_GetMaxGizmos | `0x5586f0` | 25 | `0x4a9e00` |
| GizmoObstacle_Activate | `0x557580` | 119 | `0x4aaf10` |
| GizmoObstacle_ActivateRev | `0x559300` | 229 | `0x4ac5c0` |
| GizmoObstacle_SetVisibility | `0x557630` | 60 | `0x4aaea0` |
| GizObstacles_AllocateProgressData | `0x558890` | 21 | `0x4aa7d0` |
| GizObstacles_ClearProgress | `0x5588b0` | 99 | `0x4aa5a0` |
| GizObstacles_StoreProgress | `0x5589b0` | 179 | `0x4ab1c0` |

Full obstacle layouts remain unproved. AddGizmos and GetNumOutputs remain held
for actual PC dispatcher contracts. Six missing callback entries are repaired
without altering prior bodies. The ignored `gizmo-registration-next-wave` and
independent audit retain pointer, body, consumer and complete-extent evidence.

The existing Gizmo wrapper identities were reversed. Original callback registration
and consumers establish activation at runtime `+0x54` and visibility at `+0x5c`:
`GizmoActivate` is PC `0x55a250` / Android `0x4b08f0` (92 PC bytes), while
`GizmoSetVisibility` is PC `0x55a2b0` / Android `0x4b0980` (107 PC bytes).
Activation makes the gizmo visible when `active != 0`; the earlier reconstructed
zero test was incorrect. Header fields, private config fields, copy order, source
condition and the two address bindings are corrected together. All twelve rebuilt
source calls are checked against fourteen original E8 sites. Ten call names change;
ProcessGizmoFlowBox retains activation, and its reverse call retains its identity.
This supersedes earlier conclusions based on the swapped wrapper labels; frozen
research is kept unchanged. `gizmo-wrapper-identity-correction`, its independent
audit and applied-diff review preserve the original proof and caller exceptions.

PunchCode is independently identified at PC `0x49ea70` / Android `0x157490`.
Original paired Swipe/WhipSwish sound consumers, named ELF symbol and shared
animation/math/sound calls establish identity. The PC caller supplies six words,
including a separately prepared float zero, and cleans 24 bytes. Full argument
types and signedness remain unproved, so its CSV ctype is blank. The complete
3,000-byte extent includes its seven-cell compiler switch table at `0x49f60c`;
the analyzed 2,972-byte body alone is incomplete. Exact guarded targets, prior
AddressSet, original hashes and all seven four-byte pointer owners authorize
extension through `0x49f628`, excluding following INT3 alignment. The independent
`punch-code-independent-audit` and broader `nonregistry-function-frontier-next`
retain evidence. ForcePushCode/ZapCode private ABIs and five larger tentative
movement identities remain explicit holds.

A second sixteen-name storage cohort brings reviewed AI registry names to 32.
Original pointer records, content hashes and actual lookup consumers establish
identity only. All sizes/types remain blank and extent/type/ABI flags false;
these rows add no byte coverage.

| Registry name | PC storage | Android storage |
| --- | --- | --- |
| DeActivate | `0x756abc` | `0x569ec5` |
| SetPath | `0x756aa4` | `0x569ede` |
| GoToOriginalPath | `0x756a90` | `0x569ee6` |
| SetLocator | `0x756a74` | `0x569f05` |
| SetLocatorSet | `0x756a64` | `0x569f10` |
| SetDoomedEscapeLocator | `0x756a20` | `0x569f3c` |
| NotWithParty | `0x7568e0` | `0x56a05f` |
| TakeDamage | `0x7568d4` | `0x56a06c` |
| CanHitForceObjects | `0x7568b0` | `0x56a084` |
| AlwaysBackFlip | `0x7568a0` | `0x56a097` |
| PlayerSpeederHack | `0x75688c` | `0x56a0a6` |
| SetAnimSpeedMul | `0x75687c` | `0x56a0b8` |
| SetSide | `0x756874` | `0x56a0c8` |
| SetStateArea | `0x756864` | `0x56a0d0` |
| SetOpponent | `0x756858` | `0x56a0dd` |
| AttackOpponent | `0x756848` | `0x56a0e9` |

The second read-only Android export carries the merged 32 annotations, preserving
all 266,614 preceding nodes and all 248,675 edges exactly and adding only sixteen
unknown-extent sites. It neither defines arrays nor invents users or edges.
The independent identity/endpoint audit and root streaming preservation audit
retain separate attribution. Non-registry data review also holds six existing
owner interiors, the unknown FadeSys allocation and the camera extent; pointee
or indexed-field references do not prove an independent whole-object allocation.

## Registry and original data continuation

Eleven further AI callbacks cover 7,804 complete PC bytes: HelpWithTriggers,
UseTriggerSet, PullLever, UsePanel, UseTechno, ReleaseLocator, AssignLocator,
GetLocatorFromSet, CnxController, CnxHelper and CircleLocator. Original registry
pointers and independently verified namespaces establish identities before
disassembly. The original seven-slot dispatcher, each callback's stack and
nonvolatile-register behavior, terminal extent and original range hash have
independent reviews. Nine missing entries are repaired on a project copy;
GetLocatorFromSet and CnxHelper already had analyzed functions. Frozen
`ai-registry-new-callback-batch8` and its independent audit retain the proof.

The original WorldInfo_Load caller closes the AddGizmos dispatcher gap.
GizmoSysAddGizmos at `0x55af20` (169 bytes) supplies four arguments to runtime
callback slot `+0x30`. GizObstacles_AddGizmos at `0x558710` (92 bytes) passes four
arguments to AddGizmo at `0x559ee0` (112 bytes). Original callers, registration,
paired bodies and complete extents are independently reviewed; all three
functions already exist in the graph. Aggregate types remain opaque.
This supersedes the earlier AddGizmos ABI hold; GetNumOutputs remains held.
The ignored `gizmo-add-gizmos-dispatcher-closure` and independent audit record
the actual consumers and original Android PIC materialization.

Three data identities have independently proved original PC storage extents:

| Identity | PC storage | Complete bytes | Original Android storage |
| --- | --- | ---: | --- |
| FalconDebKey | `0x7f5dc4` | 8 | `0x61e748` |
| takeoverobjects | `0x956458` | 352 | `0x12a5d40` |
| edpp_ptls | `0x2924b70` | 45,056 | `0x11e9ea0` |

FalconDebKey is two signed sentinel keys initialized to -1, with independently
bounded indices and a separate adjacent float. Takeover storage is explicitly
cleared for 352 bytes and indexed as eight 44-byte records; its count at
`0x956628` is separate. The particle base, 512 positions and 88-byte stride alone
initially left six terminal bytes unproved. Original serializer reads and loader
writes then established every terminal byte through record offset `+0x57`;
the final record ends at `0x292fb70`, before an independently allocated pointer.
The journal preserves the initial identity-only review and explicitly supersedes
it with the complete extent. Neither Android sizes nor BSS zero fill supply the
PC allocation proof. Partial aggregate field cohorts remain research; their CSV
ctypes and type-review flags stay blank/false until declarations are integrated.

Original operands independently reject three previous graph hypotheses:
PC key capacity `0x29620b4` versus Android debkeydata pointer `0x1203ec0`,
takeover array `0x956458` versus Android count `0x12a5d30`, and particle instance
field `0x2924b80` versus Android array base `0x11e9ea0`. These are explicit
negative journal decisions, not automatic corrections from names. The ignored
`nonregistry-data-frontier-next2`, its independent audit, and both
`edpp-particle-storage-boundary` reviews preserve the distinct proofs and holds.

TerrainPlayer is independently identified at PC `0x416d60` / Android `0x1026a0`.
Its complete PC span is 7,994 bytes through RET `0x418c99`; all 2,143 instructions
are reachable, with no indirect jump, local table or unexplained interior gap.
Original UpdateGameObjects callers pass one object pointer and clean four bytes.
Entry liveness finds no private incoming GPR, XMM, condition-flag or x87 argument;
the sole return restores the stack and nonvolatile registers. Three initially
opaque float-return helpers required a separate bounded dependency review.
Their verified returns close five underdefined floating reads, and all five
calls enter with owned FP depth zero. The combined core peak is eight, under the
existing reviewed NuFsqrt contract. Normal DF zero, well-formed local counts and
conventional other callee effects remain explicit assumptions; full CRT error
semantics are not newly proved. Return type and aggregate layout remain unknown,
so the CSV ctype stays blank. Its existing analyzed function needs no repair.
Frozen `terrain-player-original-closure`, independent/entry-input audits and
both helper-return audits retain these separate proofs and decode budgets.

Sixteen more reviewed registry callbacks cover 2,881 PC bytes: SetLevelPath,
ResetGameCamera, PlayCutScene, SetHint, SetHintComplete, CancelHint,
DisableNarrowSocks, CameraShake, SetGravityHeight, SetVisibility, EnableSock,
JudderGameCamera, AddDebris, TriggerBlowUp, EndCameraCut and DontRaycastLOS.
Original registry pointers, namespaces, actual seven-slot dispatch, full paired
bodies and independent entry/stack/FP/register reviews establish the contracts.
SetGravityHeight retains all 342 bytes, including six interior LEA padding bytes;
its 336-byte sparse analysis body is not the complete symbol extent. The two
Android hint callbacks are twelve-byte no-ops while PC retains richer behavior.
This platform difference is explicit and does not alter the registry identity.
SetLevelPath's original Android entry is `0x192910`, verified by original row
`0x61f534` and the ELF symbol; erroneous `0x19a910` review prose is superseded
by the independent original-byte audit. Thirteen missing entries require repair;
CameraShake, SetVisibility and SetGravityHeight already exist. Aggregate types
remain unreviewed. Frozen `ai-registry-new-callback-batch9` and its independent
audit preserve the original proof, full ranges and separate decode ledgers.

## Callback, terrain helper and state-slot continuation

The next reviewed release adds sixteen registry callbacks covering 3,815 PC
bytes and six terrain/shadow helper identities covering 8,829 PC bytes. Every
function has independent original identity, physical ABI and complete-range
reviews. Aggregate/formal types remain unproved, so the terrain helper CSV
ctypes stay blank. Fourteen missing callback entries are repaired in a copied
Ghidra project; all 8,005 prior function extents and the previously reviewed
mixed-body, literal and scalar owners survive. The resulting PC graph has
8,019 functions. Android originals and its retained export remain unchanged.

The helper identities are GameShadow (`0x5506f0`, 305 bytes),
FindReflectionNoPlatforms (`0x553500`, 100), AnimSpeed (`0x62f1c0`, 35),
NewShadowEx (`0x673730`, 234), NewScanRot (`0x671750`, 4,748), and
NewCast (`0x6729e0`, 3,407). Paired original behavior independently corroborates
platform disable/restore, reflection filtering, animation scaling, and the
shadow scan/cast chain. NewShadowEx has five physical stack slots; the four-slot
Android NewShadow wrapper is a different function. Its unused second slot and
NewScanRot's formal return remain unknown. Normal floating returns reuse the
pinned helper-return and NuFsqrt contracts; full private CRT exceptional behavior
is not established. The frozen `terrain-helper-paired-identity-frontier` and
independent audit preserve both proofs and separate bounded decode ledgers.

Ten standalone data cells have proved four-byte PC extents, totaling 40 bytes:

| Identity | PC | Original Android | Reviewed cell representation |
| --- | --- | --- | --- |
| num_takeoverobjects | `0x956628` | `0x12a5d30` | signed count |
| maxdebkeys | `0x29620b4` | `0x1203f00` | signed capacity |
| debkeydata | `0x29620c4` | `0x1203ec0` | opaque arena pointer |
| apiloadcharactermodels_nopakfile | `0x291310c` | `0x123f920` | 32-bit gate |
| CutStopInfo | `0x951c08` | `0x1290a20` | opaque nullable pointer |
| DEBPAGE_AREA | `0x805b3c` | `0x6681a0` | page handle with -1 sentinel |
| GAMEPAD_TOGGLERIGHT | `0x80228c` | `0x667c24` | unsigned mask |
| grapple_gizmotype_id | `0x802460` | `0x6686f0` | signed id with -1 sentinel |
| vehicle_scene | `0x9248b8` | `0x126c3cc` | NUGSCN pointer slot |
| Door_Last | `0x9513d0` | `0x1279e10` | opaque nullable pointer |

Actual PC producers and consumers identify each independent slot; separately
used neighboring storage closes its allocation boundary. DWORD touches or
Android symbol widths alone never supply extent proof. Pointer-cell types do
not establish their pointee layouts or allocations. Existing NuGScnRead and
NuGScnRemove contracts support only the vehicle-scene pointer representation.

Four further identities retain blank size/type and receive no extent credit:
active_cutscene_instances (`0x92135c`), debris_name (`0x7f5dd0`),
MiniKitCollection (`0x855f44`) and CutBorderScale (`0x950780`). The supposed
cutscene neighbor `0x921360` is a misleading analyzed view: the original adds
`0x921460` to a scaled index, then addresses an instance field. For debris_name,
147 active names and a separate runtime capacity of 400 do not prove a 400-cell
static array; the capacity sizes a different allocated record arena. The
collection's twenty touched bytes leave its terminal allocation unproved.
Float semantics do not close CutBorderScale's upper boundary or exclude a
surrounding owner. Original paired identity evidence remains useful despite
these allocation holds. Frozen `nonregistry-data-frontier-next3`, `next4` and
their independent audits retain the original operand and boundary proofs.

The proposed PC `0x800000` / Android weights `0x788b20` binding is explicitly
rejected. Original OR and TEST operands use `0x800000` as a flag constant;
Android OR/AND/CMP operands corroborate that role. Folding these speculative
analysis references into a huge data owner does not identify a PC allocation.
This rejects the cross-binary pairing without denying unrelated storage at the
same numeric PC address. The append-only journal retains this negative evidence.

These additions identify 12,644 PC code bytes and 40 data bytes. They add no
compiled definitions, newly compared bytes or reconstruction matching credit.

## Further camera callbacks and terrain leaves

Twelve independently reviewed callbacks add 8,953 complete PC bytes:
CameraCut, DynamicCameraCut, SetForceBack, FaceCamera, FaceCharacter,
SpinOnSpot, PlayGizSpecial, SetObstacleToEnd, MoveAwayFromLastAttacker,
SetPathCnxFlag, CycleCharacter and SetScaleOverride. Original registry namespace,
relative Android pointers, ELF symbols and complete paired bodies establish
identity. Original seven-slot dispatch, entry liveness, stack/nonvolatile and
floating-state reviews establish physical contracts; aggregate types remain
unreviewed. The independent audit normalizes known x87 store semantics before
liveness checks and rechecks the preceding two frozen batches without new
original decoding. Four missing entries are repaired in a project copy,
preserving all 8,019 preceding function extents and the explicit mixed/table/data
owners; the graph now contains 8,023 PC functions. Frozen
`ai-registry-new-callback-batch11` and its independent audit retain the proofs.

Five further terrain helper identities add 3,049 PC bytes: TerrainSkinAllocate
(`0x66f790`, 345), SkinPlatform (`0x66f170`, 1,475), TerrainSkin (`0x66ed90`,
984), TerrainPlatGetMtx (`0x67fb50`, 49) and NuVec4MtxTransformVU0x3
(`0x5c0620`, 196). Original callers establish their physical one-, three- and
five-word frames, including TerrainSkin's explicit twelve-byte result buffer.
Android's hidden-result RET 4 is not transferred. Allocate's seven-byte internal
LEA and SkinPlatform's one-byte internal NOP belong to the complete extents;
following CC alignment does not. PC's negative sample index and subtraction
cancel to Android's positive indexing for the explicitly clamped finite path;
the apparent direction difference is not a behavioral discrepancy. Floating
environment and exceptional inputs remain qualified. All five CSV ctypes stay
blank. Frozen `terrain-helper-paired-identity-frontier-next2`, `next3` and their
independent audits retain original constants, callers, ranges and decode budgets.

NewTerrainScaleYMask at `0x68f940` has positive identity and complete 1,984-byte
extent evidence, plus an actual ten-word caller frame. Fifteen opaque call sites
over ten targets still prevent full ABI approval; it receives no CSV row.
The held callee queue records those concrete dependencies rather than treating
unknown floating effects as zero.

Two further data allocations have independently established PC extents:
Accessory at `0x955650` contains eighteen 32-byte records (576 bytes), and
edpp_page_on at `0x2948ed0` is eight four-byte integer cells (32 bytes).
Accessory's original eight-DWORD record clear and cleanup reach the terminal
record; the exclusive end `0x955890` is independently used pointer storage.
Its field cohort distinguishes scene/special-handle state, saved material id,
integer texture id, integer material index and opaque model pointer. The proposed
aggregate declaration remains research in this map-only release, with blank
CSV ctype and no aggregate type promotion. The page array has an explicit
eight-cell clear and a separate pointer at its exclusive end `0x2948ef0`.
Four Accessory interior-to-base pairings and the PC loop-terminal constant
`0x292fb80` / Android page-array pairing are explicit exclusions. Frozen
`nonregistry-data-frontier-next5` and its independent audit retain the allocation,
neighbor and literal-operand proofs. These additions identify 12,002 code bytes
and 608 data bytes; they add no compiled definitions or comparison credit.

## Pod callback and further data identities

`Action_CreatePod` at `0x4dd810` adds a complete 1,135-byte body. The original
game-AI registry and Android named ELF function establish the paired identity.
Independent review closes every original PC CFG path, enumerates the four
entry-stack residues modulo sixteen, and checks stack restoration, saved
registers, seven-slot dispatch and the qualified normal floating-state contract.
Its aggregate types remain opaque. Frozen `ai-registry-new-callback-batch12`
and its independent audit retain the original proofs. Other callbacks in that
packet remain held on specific callee effects; this acceptance does not extend
to them.

Two data identities have complete four-byte PC allocations: `LevDeaths` at
`0x87b2f0` and `CRUISERD_LDATA` at `0x87b16c`. Original reset/increment consumers
and independently identified neighboring allocations bound the death counter.
Original level lookup results, lifecycle field installation and world-level
comparison establish the level pointer; distinct lookup stores on both sides
bound its allocation. Only the static pointer cell receives extent credit;
level allocation sizes and callback interfaces remain separate questions.

Six further identities retain blank size and ctype: `force_gizmotype_id`
(`0x7f8cc4`), `lever_gizmotype_id` (`0x7faaa4`), `pFadeInfo` (`0x87b4f0`),
`script_spline_selected` (`0x876db4`), `TerI` (`0x292188c`) and `ShadPoly`
(`0x292fc10`). The selected-spline value is a pointer from a selected pointer
list, rather than its integer index. TerI's `0x948` allocator request concerns
the runtime pointee, rather than the static pointer allocation. Original paired
consumers establish these identities, but neither access widths nor Android
sizes close their PC allocation boundaries. Frozen
`nonregistry-data-frontier-next6` and its independent audit retain the original
PIC/GOT, consumer and neighboring-allocation proofs. These additions identify
1,135 code bytes and eight data bytes, with no new source or comparison credit.

The next allocation review establishes `FadeSys`'s previously unknown extent
as 44 bytes at `0x95079c`, retaining the existing `FADEINFO_PC` field cohort.
Original initialization reaches the last field at `+0x28`; original methods
and independently identified adjacent storage close the allocation boundaries.
This enables comparison of an existing compiled data definition, rather than
adding a source definition. Its pointee and virtual interfaces are separate
contracts.

The private `gizmoblowupnametable` at `0x9507d0` has a complete 1,024-byte
`char[32][32]` allocation: original reset clears 1,024 bytes, lookup enforces
capacity 32 with a 32-byte stride, and insertion copies at most 32 bytes into
each slot. Independently initialized state at `0x950bd0` closes its exclusive
end. The name counter at `0x9507c8` and `ForceSFX_ConfigKeywords` at `0x7f8ccc`
receive identity-only mappings with blank extent and type; unresolved gaps and
terminal-record boundaries receive no credit. Frozen
`nonregistry-data-frontier-next7` and its independent audit retain original
PC boundaries, Android named symbols, PIC/GOT references and initializers.

## Lap-time callback and remaining registry analysis gaps

`Action_SetLapTime` at `0x4db130` has a complete 324-byte PC body. Independent
original registry and named Android ELF checks establish its game-AI identity.
The actual seven-slot dispatcher, full CFG and original parser/string leaves
close its stack, saved-register and private-input contracts. Floating results
are consumed from ST0, including the parser's zero-result path; integer EAX is
not an input there. The observed `ftol` consumer includes its alternate compiler
entry and closes the combined floating-stack peak at three under the retained
normal CRT boundary. Exceptional paths and runtime hooks remain outside that
contract. Aggregate types remain opaque.

Frozen `ai-registry-new-callback-batch13` and its independent audit retain the
proofs. The other five callbacks have reviewed identities and complete extents
for graph analysis, but remain outside the accepted map because specific callee
or indirect-call effects are unresolved. This callback adds 324 mapped code
bytes and no compiled definition or comparison credit.

The private `GameCamera` at `0x925188` has a complete 560-byte PC allocation.
The original initialized `GameCam` pointer at `0x8021b4` selects that address;
the original initialization call at `0x428d2d` clears `0x230` bytes through it.
An independent reset cell at `0x9253b8` bounds the exclusive end. Paired original
reset consumers and the named Android ELF allocation establish identity; the
PC clear and neighbors establish extent. The initializer is original virtual
zero-filled storage. Its aggregate declaration remains unknown, so ctype stays
blank and the journal keeps type review false.

Three particle candidates are rejected: the PC record arena cannot bind to the
Android pointer table, its `+0x90` WORD field cannot bind to the whole record
arena, and the separate PC pointer table cannot bind to the Android record arena.
Original literal bases, 376-byte strides and actual PIC/GOT targets distinguish
these roles. Corrected pairings remain research candidates until a separate
fixed review closes their allocations. Frozen `nonregistry-data-frontier-next8`
and its independent audit retain the camera and negative-pair proofs.

The separate fixed review accepts the corrected `part_types` arena at
`0x2913a78` with 48,128 bytes and `part_scene` at `0x29208f0` with 128 bytes.
Original PC initialization clears `0xbc00` bytes in the arena; independent
load/clear loops traverse 128 records with a 376-byte stride to `0x291f678`.
Separate original vector stores begin at that exact exclusive end. The scene
table has all 32 DWORD initializer stores and independent insertion, load and
clear capacities of 32. These PC observations establish the extents; named
Android ELF objects and actual PIC/GOT consumers establish paired identities.
Both original initializers are virtual zero-filled storage, verified from trusted
section headers without reading a 48-KiB array from the executable. Aggregate
and scene-pointee types remain unknown, so both ctypes stay blank.

`KillPartsByScene` deactivates matching runtime particles; it does not free scene
allocations. A superseding rejection decision corrects that lifecycle wording
while retaining the wrong-pair exclusion. `InitParts` owns 631 bytes across three
ranges within its 643-byte span; its last two stores and RET are already present.
The twelve-byte difference consists of internal alignment gaps, so no tail
repair or function mapping follows from the byte count. Frozen
`nonregistry-data-frontier-next9` and its independent audit retain these proofs.
The static tables remain distinct from the dynamic particle buffer and its
PC-specific 544-byte records; Android's runtime record layout is not imported.

`NewTerrainScaleYMask` at `0x68f940` now has a qualified normal ABI review for
its complete 1,984-byte extent. Independent physical and floating-state checks
compose 37 acyclic owner contracts with seven original runtime interfaces and
82 exact call bindings. The actual caller supplies ten stack words and removes
40 bytes; stack, saved-register and private-input requirements close. The
private `ftol` consumes one owned ST0 value and preserves deeper values. All
39 `NuFsqrt` call sites have no caller-owned floating values at entry and use
the retained atomic normal-return boundary.

Valid memory, runtime state, domain, clear direction flag and normal return are
explicit conditions. Exceptional paths, hooks and exact internal CRT floating
peak remain outside the review; formal return, scalar and aggregate types remain
unresolved. Its ctype stays blank. Frozen
`terrain-scale-ymask-runtime-boundaries-next1` and its independent audit retain
the final composition, actual interfaces and earlier proof hashes. This adds
1,984 mapped code bytes with no source definition or comparison credit.

## Undercover callback and bounded CRT member review

`Action_PartyCanBeUnderCover` at `0x456850` has a complete 94-byte PC body.
Original game registry records and the named Android ELF callback bind identity.
Independent complete CFG, saved-register and stack checks close all three
normal returns. The actual dispatcher supplies seven stack slots, removes
28 bytes and tests EAX. The callback returns one; its original `NuStrICmp`
dependency is a call-free integer leaf. An identity LEA used for alignment
does not introduce a semantic incoming ECX requirement. Aggregate pointer
types remain opaque, with type review false. Frozen
`ai-registry-new-callback-batch14` and its independent audit retain the proof.

The same fixed review establishes complete graph-analysis spans for `AddPart`
at `0x44b740` (516 bytes), `DrawBossHitPoints` at `0x455e60` (35), and
`LetGoOfBalloon` at `0x455e30` (35). Their outgoing callee effects remain held;
the graph repairs do not authorize accepted map rows. Party alone adds 94
mapped code bytes, with no compiled definition or reconstruction gain.

The installed MSVC CRT archive supplies a complementary source of named
member hypotheses. Its linker index selects bounded COFF members; the existing
COFF parser and relocation code remain authoritative for candidate extraction.
An index name alone does not establish original PC identity, ABI or extent.
Assembly entries with missing function metadata or shared multi-entry ownership
must remain held when the extractor cannot represent their complete bodies.
Exact full relocated byte equality is required, including external destinations.

## Terrain callback research anchors and particle-count exclusion

Four original game callbacks have independently verified identities and complete
PC extents: `AwkwardShapeOverride` at `0x454bb0` (222 bytes),
`IgnoreLastSafePathPos` at `0x454ad0` (222), `IgnoreSlideTerrain` at `0x454c90`
(193), and `SplineFollowTerrain` at `0x454d60` (193). Their original registry
records, names and Android ELF/PIC records establish the correspondences.
They enter the research journal as identity anchors with ABI and type flags
false. Optional indirect lookup effects remain unreviewed; the first two also
have an additional unresolved helper. They do not enter the accepted CSV and
add no mapped or reconstructed bytes. Frozen `ai-registry-new-callback-batch15`
and its independent audit retain the proofs and holds.

The pairing of PC `0x291f68c` to Android `PartNorm` at `0x124f220` is rejected.
The original PC DWORD is incremented by integer one while counting `0x270e`
WORD IDs; the original named Android object supplies three floating normal
components. The rejection does not depend on proving the value of ECX at the
PC store across opaque calls. The correct PC count name, type and complete
allocation remain unknown. The adjacent unnamed vector likewise retains its
identity hold; the existing four-byte `Part` pointer has no new extent credit.
Its dynamically carved buffer uses PC 544-byte records, independently of the
Android runtime stride. Frozen `nonregistry-data-frontier-next10` and its
independent audit preserve those distinctions.


## RaceOpponent and particle scene page IDs

`Action_RaceOpponent` at `0x4569b0` has a complete 606-byte PC body.
Original game registry records and Android ELF/PIC records independently bind
its identity to the named Android callback at `0x185ea0`. Independent complete
PC call enumeration, stack, saved-register, initialized-local and floating-state
checks close its physical ABI for normal returns under the recorded valid
runtime, memory and numeric-domain conditions. Its actual AIMove call supplies
mode one. The goal-speed callback must remain the verified installed
`0x43c130` target or a separately reviewed equivalent. Arbitrary callback
replacement, other AIMove modes and exceptional CRT paths remain unreviewed.
Formal return and aggregate types remain unknown, so ctype stays blank.

Fresh complete RaceOpponent and AIMove call lists disprove the earlier stale
`0x923924` lookup hold for this reachable mode-one closure. That lookup remains
a real unresolved interface for the separately held terrain callbacks. Frozen
`callback13-mid-distance-boundary-next1` and
`callback13-race-final-independent-next1` retain the correction, complete
606-byte body hash and prior independent proof pins. This function adds
inventory coverage without a reconstructed definition or code comparison gain.

`part_scene_pageid` at `0x291f6a8` is a 128-byte `i32[32]` allocation.
Original PC InitParts initializes all 32 DWORDs; insertion, loading and clearing
consumers independently establish its complete extent ending at `0x291f728`.
Signed page comparisons and runtime `-1` tags distinguish it from the separate
scene-pointer array. Original Android ELF/PIC/GOT records bind the named
128-byte object at `0x1242040`; Android size is not PC extent authority.

The original PE virtual storage is zero. The reconstructed data definition
therefore uses a zero initializer; InitParts' runtime `-1` writes do not become
a static initializer. Frozen `nonregistry-data-frontier-next11` and its
independent audit retain identity, signed type, complete PC ownership and
initializer evidence. This batch adds 734 mapped bytes: 606 code and 128 data.


## Particle page occupancy gates

`part_page_used` at `0x29208bc` is an independently bounded 32-byte allocation
of eight integer page occupancy gates. Original PC InitParts explicitly writes
all eight DWORDs through `0x29208d8`. An independent free-slot scan tests cells
1 through 7, stops at index eight and sets the selected gate to one; ClearPage
resets the indexed gate to zero. Those consumers establish the complete extent
ending at `0x29208dc`. The following 20-byte gap is not part of this owner.

Original Android ELF and PIC/GOT relocation records bind the named
`part_page_used` object at `0x1242160`. Android allocation size is not the PC
extent authority. The PC declaration is `i32[8]` for zero/one integer gates;
this does not establish signed arithmetic or arbitrary index safety. The named
Android FileLoadEffects body has different bulk-reset behavior and remains
unaccepted as a PC function correspondence.

Original PE storage is zero and data.cpp uses that initializer. Frozen
`nonregistry-data-frontier-next12` and its independent audit retain the original
operands, capacity, allocation, relocation and initializer proofs.


## CompleteLevel callback

`Action_CompleteLevel` at `0x455e90` has a complete 280-byte PC body,
independently bound to the named Android callback at `0x190930` by original
game registry and ELF/PIC records. Fresh review of the complete callback and
all nine direct calls closes ten original bodies, including their transitive
leaf calls. Normal stack and saved-register preservation, consumed arguments,
local initialization and private register/flag requirements pass. The integer
closure contains no x87 instructions and both callback returns produce EAX one.

The review assumes valid initialized tables, signed IDs, groups, strings and
runtime objects. Record stride is exactly `0x130` (304 bytes). The queue count
must be in 0..8: the original signed upper-bound test supplies no negative-count
guard. These conditions do not establish arbitrary-input safety, exceptional
behavior, concurrent mutation rules or formal aggregate/return types. Ctype
stays blank. The following source release supplies a genuine implementation
without claiming recovery of its historical formal types.
Frozen `callback13-next-function-frontier-next1` and
`callback13-completelevel-independent-next1` retain the complete proof and
qualifications. Together with the page occupancy gates this batch adds
312 mapped bytes: 280 code and 32 data.


## CompleteLevel source closure

The callback now has genuine C together with NewCutScene (`0x57cd30`, 69 bytes),
CutScene_Find (`0x57c970`, 77), GoToNewLevel (`0x57a580`, 43) and CompleteLevel
(`0x57a5b0`, 97). Independent original paired bodies establish each helper's
identity, complete PC extent and qualified normal physical ABI. All four retain
blank ctypes. NewCutScene receives four stack words, reads three and returns the
queued pointer or NULL; the callback consumes that result. The callback ignores
the two level helpers' EAX results, which does not establish historical return
types for other callers.

Named access fields establish WORLDINFO.cutscene_sys at `0x2ab0`, the CUTSYS_PC
list/count prefix at offsets zero/four and CUTINFO's inline name at eight.
CUTINFO.instance at four retains its separate existing proof. These are access
prefixes; neither full cutscene allocation sizes nor name capacity are known.
WORLDINFO's verified size and existing offsets remain unchanged. PC level
records use the verified 304-byte stride, never Android's 324-byte layout.

The five routines add 566 original bytes to comparison. The callback scores
98.175255%; GoToNewLevel 96.666664%, CutScene_Find 82.30556% and NewCutScene
80.208336%. CompleteLevel scores 0.125% with a 162-byte rebuilt body: VC8
faithfully inlines Area_FindStatusLevel in the shared areas unit, whereas the
97-byte retail body calls it. Full extents remain in the comparison; no code
is masked or truncated. No new function is exact. Every previous exact function
and data comparison remains unchanged.

Frozen `completelevel-source-draft-next1-handoff`,
`completelevel-source-draft-independent-next1` and
`completelevel-compiled-contract-independent-next1` retain C, branch, argument,
result, volatile-access and initializer evidence. Normal queue range 0..8 and
valid initialized runtime/table/string conditions remain explicit; source
interfaces do not prove unrestricted safety or historical aggregate types.


## RaceOpponent and animation-speed source

Action_RaceOpponent (`0x4569b0`, 606 original bytes) and Action_SetAnimSpeedMul
(`0x44f6e0`, 370) now have genuine C in the existing game action unit. Their
original registry identities, complete extents and qualified normal physical
contracts remain separate from formal aggregate types. SetAnimSpeedMul's prior
CSV signature described a source convention despite type_reviewed being false;
it is now blank, with an explicit journal supersession naming the earlier
same-pair decision. The seven-word source declarations do not recover historical
formal types.

Race uses the existing named APIOBJECT owner/AI prefix, the packet's own script
processor for parsing and the separate processor's action_data_4 for distance
state. Actor byte `0x408` is movement_stopped, distinct from goal_speed_mode at
`0x407`. Its new descriptive float field movement_speed_scale_pc at `0xec8`
replaces only four opaque bytes. Animation speed similarly exposes only the
verified float at `0x1034`. Existing GAMEOBJECT offsets and size are preserved.

Independent current-object audits verify Race's two float32 subtraction
rounding checkpoints before strict double half-distance comparisons, double
lateral products/addition before the float32 destination store, all eleven
call targets, and parser/movement arguments. Compiler lateral product order
is different within the reviewed finite normal domain; no arbitrary NaN-payload
or exact-byte claim follows. The stable goal-speed callback and normal socket,
table and math conditions retain their original qualifications.

The animation callback performs immediate value writes, has no first_time
guard, and applies multiplication/clamping only when the multiplier is not
ordered-equal to one. Its source-local volatile float enforces the original
explicit float32 storage/reload before comparisons; this does not claim a
historical volatile local or volatile GAMEOBJECT field. Actual VC8 output
preserves maximum-first then minimum clamping, inverted-bound ordering and
NaN behavior, balanced returns and the normal six-body parser closure.

Full comparison adds 976 original code bytes. Race scores 91.99448% with a
617-byte rebuilt body; SetAnimSpeedMul scores 85.79032% with a 396-byte body.
Neither is exact. Frozen source, field, physical-closure and compiled-contract
reviews remain under `raceopponent-source-*`, `raceopponent-compiled-contract-*`
and `animspeedmul-source-*` / `animspeedmul-compiled-contract-*`. No original
bytes are copied into the rebuilt functions or masked out of comparison.


### CurrentStart and UseCurrentSpeed source closure

`CurrentStart` is independently identified at PC `0x499ca0`, complete size 288,
against the original named Android `0x153840`/412-byte body. Fresh PC decoding
covers all 78 reachable instructions and both terminal returns through
`0x499dc0`; the 288-byte SHA256 is
`6d695075da492177589087b626461186b5daa4db3840c00ac7434c68b3e9d7f9`.
The accepted `Action_UseCurrentSpeed` (`0x44c2a0`/336, Android `0x188cd0`)
actually passes object/1/1 and cleans 12 bytes. Independent original provider
closures cover `WorldInfo_CurrentlyActive` and both call-free vector rotations.
Normal stack, nonvolatile-register and owned x87 states close without private
incoming register/flag dependencies. The earlier procedural budget-held
CurrentStart review remains held; the fresh next2 audit supplies approval.

The helper always obtains the active world and clears the object's C4C float
before its guards. Its speed comparison rejects ordered zero only: negative
and NaN values continue. Second argument zero requires the actual PC
`DOGFIGHTA_LDATA` pointer and current-level equality; Android's TwistLevel test
is not transferred. The stored raw socket speed is multiplied by EC8 when
EF4 bit 0x200000 is set, otherwise C44. Third argument nonzero rotates the
local current vector X then Y; zero rotates the existing velocity around Y.
The C4C field split and socket pitch assertion describe accessed PC prefixes,
not a recovered complete historical aggregate. The legacy field named
`field_0x276` is physically at 0x27a and remains explicitly zero-extended.

The callback retains its first-time guard, signed parameter count, FALSE
priority, substring `multiplier` with advancement 11, optional `snaptospeed`,
portal flag and signed socket arguments. Independent source replay and actual
MSVC COFF reviews verify both routines' qualified normal contracts. Existing
canonical parser/socket interfaces remain explicit boundaries; this does not
prove all malformed inputs or floating exceptions. Historical formal types
remain held: the helper's observed ignored EAX supports a reconstructed void
view, while the callback's old source-convention CSV ctype is cleared through
an explicit append-only journal supersession.

Genuine C resides in the existing gameobjects and gameaiactions units. The
helper compiles to 279 bytes and scores 83.92308%; the callback compiles to 344
bytes and scores 95.80808%. Neither is exact. This adds 624 compared original
bytes, preserves all 801 exact functions and 45,746 exact bytes, and reaches
355,027 compared bytes including the unchanged 55-byte alias overlap
(354,972 unique). Whole-PC fuzzy coverage is 6.265641761481748%. The new
helper adds 288 known mapped bytes, reaching 1,116,826 / 40,067,072.

Frozen evidence under `build/analysis/identification/continuation/`:
`usecurrentspeed-callee-independent-next2.json`,
`usecurrentspeed-source-cohort-next1-handoff.json`,
`usecurrentspeed-source-fields-independent-next1.json`,
`usecurrentspeed-source-draft-independent-next1.json`,
`currentstart-compiled-contract-independent-next1.json` and
`usecurrentspeed-compiled-contract-independent-next1.json`.


### SetScaleOverride source transfer

Accepted `Action_SetScaleOverride` at PC `0x44c750`/687 (original named
Android `0x186bf0`/993) now has genuine C in the existing gameaiactions unit.
Its complete original range is `[0x44c750, 0x44c9ff)`, SHA256
`471a0143bcc441d31311e63bbe8d8a9315655c43b1245bdd95947af995887d74`.
Fresh independent source review covers 226 instructions/six returns and
319 bit-aware register/flag/stack/x87 states, with five providers and two
parser children checked separately. Existing collision_scale_override102c,
processor action_data_4/5 at70/74, FRAMETIME and distinct F32/F64 billion
sentinels are reused; only the missing processor74 assertion is added.

The callback preserves token priority, reset, min/max randomization,
substring `time` plus NuStrLen+1, immediate and timed scale paths. Both
bounds must compare unequal to the double sentinel to randomize; unordered
comparisons count as unequal. Only ordered zero time selects the immediate
path. Nonpositive or unordered remaining time completes, and interpolation
continues only for ordered FRAMETIME < remaining time. These conditions are
reviewed within the qualified initialized, masked-floating, DF0 domain;
historical aggregate/formal types and universal exceptional effects remain
unproved. The previous source-convention signature is explicitly superseded
to blank, preserving the accepted identity and complete original extent.

Actual VC8 output independently preserves the important rounding boundaries:
timed random target FSTP32/reload before division and processor74 FSTP32
followed by a fresh F32 load before interpolation. NuRandFloat returns and
call-site random locals are F32; weighting products/addition remain extended
without intermediate product rounding. The source-local volatile target
expresses required storage, not a recovered historical volatile declaration.
All eleven compiled direct calls, private input/register effects, stack and
owned x87 return states pass. The compiled body is 645 bytes and scores
54.141594%; it is not exact and its full original extent remains compared.

This adds 687 compared original bytes, reaching 355,714 including the
unchanged 55-byte alias overlap (355,659 unique). Whole-PC fuzzy coverage
increases to 6.276357013318698%. All 801 exact functions and 45,746 exact
code bytes are preserved. Since the callback was already accepted inventory,
no mapped code extent is added; the new reset literal access view earns zero
extent/exact credit. Frozen evidence under the continuation directory:
`scaleoverride-source-cohort-next1-handoff.json`,
`scaleoverride-source-draft-independent-next1.json`,
`scaleoverride-source-fields-independent-next1.json` and
`scaleoverride-compiled-contract-independent-next1.json`.


### Surface and socket helper source transfer

`SetObjOnSurface` at PC `0x572660`/233 is independently paired with original
Android `0x4e2470`/303; `SetSockPostion` at PC `0x53f640`/241 with
Android `0x3c00b4`/418. The socket name retains the native misspelling.
Their complete original ranges exclude following padding, already agree with
the retained graph, and add 474 known mapped and compared code bytes. Physical
two-word and five-word call interfaces are reviewed separately from historical
formal types; CSV ctypes remain blank. Genuine C joins the existing gameobjects
and socksys units, reusing verified fields and providers.

Surface placement retains the PC F64 epsilon `0.10000000149011612`, extended
product/subtraction arithmetic and F32 Y storage before the three position
writes. The Android-specific extra subtraction is absent. Sentinel/level gates
return before velocity changes; equal or unordered floor comparisons permit
placement, and remaining paths set vertical velocity to minus one. Actual VC8
output preserves both required F32 storage/reload checkpoints. Its 246-byte
body scores 72.968254% against all 233 original bytes.

Socket placement invalidates the output before null/unsigned-index checks,
preserves open/closed segment limits, signed narrowed next-segment equality,
NaN and negative-zero fraction behavior, and the hardware-masked shift for
accepted indices 32 through 63. FillSockPosition, camera-position copying and
SockSysPointAlongMID retain their order. Original private register interfaces
and the compiler's same-unit interfaces differ; the complete generated caller
and static providers are reviewed together, with no public stack wrappers.
The new 244-byte body scores 66.89286% against all 241 original bytes.
Both new functions remain nonexact. All 2,022 previous function report rows,
including both private providers, are unchanged; all 801 exact functions and
45,746 exact code bytes remain preserved.

`SurfaceInfo_ExtraReflectFn` at PC `0x951030` is independently paired with the
native Android OBJECT `0x12a48f0`. The source emits a four-byte pointer access
view, but its complete original allocation and formal type are unknown: CSV
size/type stay blank, with zero extent or exact credit. Retained direct accesses
comprise writer `0x42f12c` and readers `0x572645`/`0x572aa8`. The writer installs
unnamed call-free target `0x42a690`/167; the reviewed null/literal-target states
do not prove arbitrary alias closure or name that target from the slot name.
The original zero-filled initializer does not imply a permanently null slot.

The checkpoint compares 356,188 original bytes including the unchanged 55-byte
alias overlap (356,133 unique). Whole-PC fuzzy coverage is
6.285899057426779%; known mapped image extent is 1,117,300 / 40,067,072.
The additional exact eight-byte epsilon copy reuses an existing allocation and
adds no original coverage. Frozen continuation proofs include
`snap-setobjonsurface-independent-next1.json`,
`snap-setsockpostion-independent-next1.json`,
`setobjonsurface-source-draft-independent-next1.json`,
`setsockpostion-source-draft-independent-next1.json`,
`setobjonsurface-compiled-contract-independent-next1.json`,
`setsockpostion-compiled-contract-independent-next1.json` and
`surface-callback-data-independent-next1.json`. InitSurfaceInfo and the two
snap callbacks remain held for their separate query/shadow/reflection closure.


### Surface query, getter and vector source release

GetSurfaceInfo PC `0x572440`/536 is independently identified with native
Android `0x4e21b0`/699. Complete original extent, three caller-cleaned words
and ignored return are reviewed independently of historical formal types;
CSV ctype remains blank. Four zero-argument leaf getters are independently
identified and implemented in the existing nuterrain unit: ShadowInfo
`0x67a600`/18, EShadowInfo `0x67a670`/18, ShadowRoofInfo `0x67a690`/18 and
NewShadowOnPlatform `0x67a6e0`/55. Each matches the original bytes exactly.
The five new identities add 645 known mapped original code bytes.

The query preserves raw versus clamped surface IDs, signed byte checks,
reflection gates, material masks, angle calls and ordered-equal versus
unordered-unequal sentinel branches. The direct character-data chain and
existing TerSurface flags, area gates, vector and angle globals are reused.
The three new object fields are narrow access views: roof float224 (also
preserving the old opaque four-byte alias), roof byte284 and raw byteE42.
Historical full object types are unproved. The optional callback retains the
qualified original null/literal42a690 states; arbitrary runtime target or
writer closure and whole InitSurfaceInfo remain separate holds.

NuVec4MtxTransformVU0x3 PC `0x5c0620`/196, already accepted against native
Android `0x291221`/551, joins the existing nuvec unit as genuine C. It transforms
exactly three four-component vectors and preserves observed in-place use.
Actual output rounds Y/Z/W through FSTP32 locals before X output, then reloads
them for the remaining stores. The compiler exchanges the first two operands
of one Z addition and swaps pointer increments; qualified finite normal
behavior is preserved, but the full equal-size bodies remain byte-different.
It scores 98.477615%. GetSurfaceInfo compiles to 622 bytes and scores
67.30769%; its complete 536 original bytes remain compared.

The group adds 841 compared original bytes, reaching 357,029 including the
unchanged 55-byte overlap (356,974 unique). Whole-PC fuzzy coverage increases
to 6.30499264352814%. All 801 prior exact functions remain preserved; four new
exact getters bring the total to 805 and exact code to 45,855 / 3,471,246 bytes
(1.3209954004988411%). Mapped image extent is 1,117,945 / 40,067,072.
Neutral global access views have unknown owner sizes/types and earn zero
original extent or exact-data credit; existing ShadPoly/CurrentTerrain owners
are reused, and the additional exact 2e6 constant copy adds no allocation.

Frozen continuation evidence: surface-query-independent-next1.json,
surface-query-getter-independent-next1.json,
surface-query-data-fields-independent-next1.json,
getsurfaceinfo-source-draft-independent-next1.json,
surface-query-getter-source-independent-next1.json and
terrain-vector-source-independent-next1.json, with actual compiled-contract
reviews for each of the query/getter/vector cohorts. The next larger closure
is NewScanRot/NewCast (8,155 PC bytes), its terrain skin providers and the
shadow/reflection wrappers before InitSurfaceInfo and the snap callbacks.


## Terrain skin and scratch source continuation

TerrainSkin (`0x66ed90`, 984 bytes), SkinPlatform (`0x66f170`, 1,475),
NuScratchAlloc32 (`0x6de8d0`, 37) and NuScratchRelease (`0x6de960`, 15)
now have genuine PC C implementations. The two skin identities were already
in the accepted inventory; only the scratch pair adds 52 mapped code bytes.
All historical formal ctypes remain blank. The source adds 2,511 compared
original bytes, reaching 359,540 including the unchanged 55-byte overlap
(359,485 unique). Whole-PC fuzzy coverage is 6.349062864687342%. All 805
previous exact functions survive; the 15-byte scratch release is exact,
bringing exact code to 45,870 / 3,471,246 bytes across 806 functions.
Mapped image extent is 1,117,997 / 40,067,072 bytes.

The skin code shares nuterrain.c and its verified 100-byte polygon view.
Original REP MOVSD copies all 25 words, including material bytes60/61.
Skin writers establish Y bounds at8/c; group origin/data/bounds/radius
refinements preserve the prior38-byte stride and every existing getter.
TerrainSkin returns its original output pointer in EAX; the actual same-unit
caller uses the five-word interface and caller cleanup. Signed negative-eight
sampling, 16 matrix stores, quantized float temporaries, and normal/squared
length/length/inverse rounding are retained. Existing SkinFlipTab[8],
NuTrigTable, CurrentTerrain, NuTimeBarTop and BoltTargetRandomDenominator
owners are reused. Table access is qualified to vertex_order0/1 and valid
initialized variable records with enough output storage. The compiled x87
peak is eight from empty entry, including FTOL fallback effects; future
caller live-prefix closure is separate from this finite source approval.

TerrainSkin compiles to889 bytes and scores49.66265%; SkinPlatform compiles
to1331 and scores67.70601%. The allocator compiles to36 versus37 original
bytes and scores74.166664%; its normal unsigned alignment/trailer effects
are independently reviewed. These three functions remain nonexact. Release
has full byte equality at15 bytes. The scratch backing is a 0xa20 (2,592)
byte minimum source view for the reviewed empty-start nested0x948 (2,376)
and0xd0 (208) requests plus two four-byte trailers. Its pointer initializer
resolves symbolically to original0x2976d00; unknown larger requests, deeper
nesting, original full allocations and historical types remain unproved.

Frozen evidence includes terrain-scratch-independent-next1.json and its
declaration-scope supplement, terrain-scratch-compiled-independent-next1.json,
terrain-skin-source-independent-next1.json,
terrain-skin-selector-data-independent-next1.json,
terrain-skin-compiled-independent-next1.json and its map correction supplement,
and terrain-wave20-data-independent-next1.json. The compiled scratch ledger
explicitly records its initial metadata bookkeeping overrun and authorized
bounded supplement; its AS256MiB memory cap remained in force.

Next are TerrainSkinAllocate345 and the connected NewScanRot/NewCast8155
bytes. Their accepted identities do not establish source closure. Finite
scan writer limits, W-lane values, active platform-list writers, cell/SET
float associations and promoted-float constants have separate reviewed
proofs. NewCast's initial+2million equality path reads uninitialized original
stack normal data; regular-input source is qualified separately and has no
fabricated normal initializer or prior-global carry. Whole shadow/reflection,
InitSurfaceInfo and the snap callbacks remain incomplete.

## Terrain scan, cast and pool source transfer (2026-10-01)

The existing accepted entries now have genuine C in `src/nu2api/nuterrain.c`:
`TerrainSkinAllocate` at `0x0066f790` / 345 bytes,
`NewScanRot` at `0x00671750` / 4,748 bytes, and
`NewCast` at `0x006729e0` / 3,407 bytes. Their Android reference entries are
native `0x00376940`, `0x00378fb0` and `0x0037aa40`, respectively. This adds
8,500 compared original bytes; it does not add new function identities.
The rebuilt sizes are 333, 5,060 and 2,693 bytes. All three remain nonexact:
58.26923%, 9.219463% and 26.593693% native objdiff similarity. Low similarity
is retained honestly; broad source transfer does not establish byte equality.

The pool source preserves epoch updates, first-free/strict signed oldest
selection, eviction and sixteen cache-age resets. The scan uses the reviewed
96-entry active group view, signed record headers, ordered cell overlap,
strict SET containment and five logical unsigned append limits. VC8 unrolls
those limits into eleven machine guards. Header slot 509 terminates the list;
512 storage words do not mean 512 available polygon slots.

Actual compiler reviews preserve extended midpoint/radius arithmetic and
one explicit squared-distance `f32` spill/reload. Both cast plane paths
round the quotient to `f32` before adding the base height, then round the
final height before comparisons. Scan and allocator return with owned x87
depth zero; all seven scan provider calls enter at zero. Cast takes three
stack words and returns positive zero in ST0, with entry depth zero and
peak eight. The existing skin bodies and relocation semantics are unchanged.
The macro naming repair also leaves every emitted terrain function unchanged.

These are qualified initialized-engine/normal-provider source contracts.
Original sentinel-first above-plane ties can read undefined stack residue;
no artificial normal initializer or unconditional all-input equivalence is
claimed. Valid skin indices, pool slots, spans and backing capacity remain
caller qualifications. Full original aggregate/data owner types remain held.

Frozen local evidence: `terrain-scan-source-independent-next2.json`,
`terrain-scan-compiled-independent-next1.json`,
`terrain-cast-compiled-independent-next1.json`,
`terrain-skin-allocate-compiled-independent-next1.json`,
`terrain-wave21-data-independent-next1.json`, and
`registry-wave21-macro-rename-compiled-supplement.json`, under ignored
`build/analysis/identification/continuation/`. The next shadow wrappers still
need separate provider/caller closure; their inventory names are not source.

## Shadow wrappers and surface initialization (2026-10-01)

Genuine C now closes NewShadowEx234, GameShadow305,
FindReflectionNoPlatforms100, PlatOnOff73, NewTerrPlatformsOff10 and
InitSurfaceInfo538: 1,260 newly compared original PC bytes. The first three
already had accepted inventory rows. The last three add independently reviewed
identities and 621 known mapped bytes; their historical ctypes remain blank.
InitSurfaceInfo's native Android symbol is `_Z15InitSurfaceInfoP12GameObject_s`
at `0x004e2680` / 553 bytes; its PC body at `0x005728a0` is independently
complete at 538 bytes. The switch pairs are PC `0x0067c8e0` / 73 to native
Android `0x00389800`, and PC `0x0067a6d0` / 10 to native `0x00384ba0`.
The canonical graph IDs use those native addresses without a guessed bias.

The corrected identity supplement `init-surface-paired-identity-next1.json`
verifies the original ELF symbol, full body, child bindings and matching control
sequence. The earlier six-helper paired audit does not contain InitSurfaceInfo
and supplies no authority for that identity. Original PC size and physical ABI
remain separate from Android's symbol extent and historical formal types.

NewShadowEx, GameShadow, reflection and the all-platform-off switch are exact:
649 original bytes across four functions. PlatOnOff rebuilds to 68 bytes and
scores 53%; InitSurfaceInfo rebuilds to 634 and scores 48.68276%. They remain
nonexact. SnapCreaturePos's existing 408-byte implementation now makes its
actual InitSurfaceInfo call followed by optional SetObjOnSurface(object, 0).
It scores 74.88421%; restoring these calls adds no new compared-byte credit.
SetObjOnSurface233 and GetSurfaceInfo536 already had genuine implementations.

The source retains duplicate platform handles, the qualified sixteen-handle
stack domain, signed handles and original nullable world/list guards. Float
results are stored to f32 at the original observable boundaries. Reflection
rejects ordered equality with the double sentinel; unordered values remain
admitted. Init preserves the water predicates, two-query order, accessed D6C
float and once-loaded final callback. These remain qualified valid-engine,
normal-provider contracts; unknown full backing capacities and historical
aggregate types are not promoted. The terrain wrapper calls enter with empty
owned x87 stacks; its result is the local Y after discarding NewCast's return.
All fifteen previous terrain bodies and semantic relocations remain unchanged.

Ignored continuation evidence includes the corrected paired identity,
`registry-wave22-map-independent-next1.json`, the shadow source and actual
compiled gates, and the Init source/compiled gates. No extracted retail bytes
or original inputs are committed.

## Snap action source and structural discovery (2026-10-01)

Action_SnapToPosition663 and Action_SnapToSockPosition1413 now have genuine C
implementations using the reviewed surface and socket providers. Their existing
accepted identities add no new mapped function extent. The complete rebuilt
bodies are 677 and 1,370 bytes and score 72.567566% and 83.48301%; both remain
nonexact. Position preserves parser shadowing, float-to-integer yaw conversion,
actor/camera writes and conditional terrain queries. Sock preserves discarded
parameter parses, socket guards, four actual random calls weighted by the
external F64 zero, signed remainder/spread and the eight-slot Player traversal.
These remain qualified normal-engine/provider contracts.

Actual compiled review caught a misleading member name: the heading writes
belong at actor +0x27a, represented by the existing asserted field_0x276 member;
supporting_platform_id is at +0x27e. Both new writes were corrected. Each complete
body changed only that displacement byte; all other bytes and semantic
relocations remained unchanged. The earlier failed gates are retained locally.
No aggregate layout changed. Frozen corrected gates are
`action-snap-position-compiled-independent-next2.json` and
`action-snap-sock-compiled-independent-next2.json` under ignored continuation.

Structural discovery now takes priority over further source backlog. Configurable
round limits expose convergence, and a streamed accepted-seed crosswalk supports
bounded independent caller-signature retrieval. The four-to-seven-round experiment
adds 22 tentative pairs; a separate pilot supplies 13 unmapped review leads.
Neither publishes names automatically. Missing function nodes and candidate data
feedback are being reviewed separately so uncertain edges retain their lineage.

## Structural caller-signature mapping release (2026-10-01)

Five original identities add 530 known code bytes without source reconstruction:
GameCam_Reset at 0x0054a2f0 / 232, SetWeaponIn at 0x0058d8e0 / 66,
APIResetCharacterRemap at 0x0062d500 / 53, AddVariableShotDebrisEffect at
0x00684c90 / 71 and EatVictim at 0x0049ca10 / 108. All ctypes remain blank.
Shared accepted caller signatures selected the candidates; independent original
PC bodies, native Android records, callers and complete boundaries established
the released identities. Android APIResetCharacterRemap is exactly 0x003cc1dc;
an earlier prose typo 0x003cc7dc was rejected, not normalized into a match.

SetWeaponIn and the remap reset are callfree. GameCam_Reset retains an explicit
normal canonical-memset child qualification; the debris wrapper retains a normal
callee-effects qualification with deeper shot-emitter semantics unreviewed.
EatVictim's animation-duration provider physical effects are reviewed. None of
these qualified caller boundaries establishes a full historical formal type,
all-input source implementation or exception guarantee. StartLaunch's identity
and 98-byte boundary support graph repair, while its callee ABI remains held;
no StartLaunch CSV or accepted journal row is added.

The two repaired graph nodes expose the original calls from 0x00449f61 and
0x00448f9a. A longer repaired-graph run reaches 1,328 tentative pairs, two more
than the same longer run before repair. Original inputs, prior extents and retained
projects remain pinned. Ignored evidence comprises the two structural callee
reviews, missing-callee review, repair audit and structural-wave24-root-release.
## Structural anchor expansion (2026-10-01)

Seven independently reviewed identities add 389 known PC code bytes:
SetFlicker 0x00414ab0/25, oneAtOnce_SetNumAttackers 0x0043d500/32,
ResetForceBack 0x004a48f0/13, ResetGameMessages 0x00560b40/32,
Player_ClearContext 0x0055e4a0/58, PlayDieSfx 0x00548460/209 and
NuMtlCreate3D 0x005e4d40/20. All historical ctypes remain blank. Original
identity, complete PC boundaries and physical caller effects are independently
reviewed; graph scores alone do not authorize names.

Player_ClearContext retains normal callback/provider qualifications.
PlayDieSfx requires the original null hook or an independently verified ordinary
one-actor hook. NuMtlCreate3D retains the positive-count, initialized material
pool and capacity domain, with reviewed normal finalizer/memset effects;
nonpositive-count/error paths remain held. These qualifications do not establish
arbitrary callback effects or source implementation completeness.

The release reaches 2,046 mapped functions and 1,119,537 / 40,067,072 known
mapped image bytes. Three identity-only data rows add no extent credit. The
mapping release and independent original packets are retained under ignored
`build/analysis/identification/continuation/structural-wave25-*` evidence.

## Structural leaf and caller-context release (2026-10-01)

Twelve further original identities add 3,920 known PC code bytes, with blank ctypes:
AlertSurroundingCreatures 0x004399e0/101,
oneAtOnce_SetAttackersPerRow 0x0043d520/16,
oneAtOnce_SetDistPerRow 0x0043d530/41,
oneAtOnce_SetInitDistPerRow 0x0043d560/41,
FreeTorpedoPacket 0x004d22f0/90,
CharCategory_IsCategory 0x0055bc80/115,
GameObjectSetCanUse 0x0055dd70/47,
SetPlayer 0x0055e410/85,
DestroySnakeBody 0x00597350/47, NuGScnGetSpecial 0x005da860/98,
ResetAICreature 0x0041c120/1276 and AddDynamicCreature 0x0043d890/1963.
Paired original bodies, native ELF identities, actual PC callers and complete
PC extents establish these bindings independently of propagation scores.

The floating setters require two free x87 slots and their stated normal finite
input domain; negative inputs also overwrite their own argument stack slot with
positive zero. Seven trailing INT3 bytes are outside each 41-byte body.
GameObjectSetCanUse consumes five caller words, with the fourth unused.
SetPlayer clears only player when neither actor qualifies, leaving player2
unchanged. NuGScnGetSpecial retains the initialized backing, writable output
prefix and nonnegative-index domain. FreeTorpedoPacket and DestroySnakeBody
retain qualified normal canonical-memset effects, without transferring native
aggregate sizes or approving unknown backing allocations.

The creature pair preserves reviewed local ESP/nonvolatile-register and x87
effects, with all outgoing calls entering at local x87 depth zero. The actual
private integer-shift helper is reviewed; engine child calls retain explicit
normal return, stack/register and float-effect qualifications. Transitive
callbacks, malformed aggregate domains and exception behavior remain held.

The three setters required graph repair. A disposable export preserves all
8,046 previous function IDs/extents and adds exactly those three functions,
for 8,049 PC functions. The retained project and Android export remain pinned.
Graph repair alone adds no mapping or source credit. StartLaunch and
instNuGCutSceneEnd remain excluded because their physical callee contracts
are unresolved. Independent release evidence
`structural-wave26-mapping-release-independent-next1.json` covers the first ten
functions; the combined function/data release is recorded separately under
ignored continuation. There are now 2,058 mapped functions and
1,123,457 / 40,067,072 known mapped image bytes. Ten newly reviewed data
identities have blank extents/types and add zero image coverage. This release
adds no source comparison bytes. TerrainInitEx's separately reviewed 3,491-byte
identity and physical-contract supplement await the next mapping release;
PanelRender's physical child/virtual-call contracts remain held.

## Larger structural caller families (2026-10-01)

Seven further original identities have complete PC extents and qualified
physical caller contracts; their ctypes remain blank:

| Function | PC entry | PC bytes | Native Android entry |
| --- | --- | --- | --- |
| Batarangs_Reset | 0x00404990 | 184 | 0x001d7a20 |
| AddCreature | 0x0041f8f0 | 67 | 0x000fdf50 |
| KeepVehicleOnScreen | 0x00499300 | 509 | 0x00150000 |
| GetLevelExBlowupFlags | 0x00569c60 | 6 | 0x004bd870 |
| NuDisplayListRndrSpecial | 0x005f0f00 | 1491 | 0x002f3340 |
| TerrainInitEx | 0x0066b8c0 | 3491 | 0x0036e580 |
| AddDebrisEffect | 0x00684ee0 | 1025 | 0x00352ee0 |

The 6,773 bytes add inventory coverage only. Original caller cleanup, private
register inputs, nonvolatile preservation and local x87 effects are reviewed;
normal engine/provider calls retain explicit qualifications. TerrainInitEx
consumes ten caller-writable words and requires the reviewed filename length
at most 95 for its child buffer. Its combined reviewed x87 peak is seven.
KeepVehicleOnScreen's clamp mutates its outgoing argument storage; its vector
helpers close at a combined peak of three. Display and debris private renderer
and matrix interfaces are inspected rather than assumed to use stack arguments.
AddCreature's integer shift preserves its allocator's live floating value;
its EAX-object and ESI-object child interfaces are also inspected.

The call-free batarang reset has 32 distinctive ordered stores. The flag getter
reads the already identified flag owner, whose allocation size/type remain
unknown. Neither adds a data definition. ReadPad, PanelRender and GizmoBlowUp_Hit
remain research identities with explicit installed-callback or child-contract
holds. Native sizes, argument layouts and runtime callback effects are not
transferred to PC source. Frozen original and independent release packets remain
under ignored continuation.

## Structural propagation counterfactual (2026-10-02)

Five further bindings have independently reviewed original identities, complete
PC extents and qualified physical caller contracts. Their ctypes remain blank:

| Function | PC entry | PC bytes | Native Android entry |
| --- | --- | --- | --- |
| GameCam_HitJudder | 0x00549f10 | 186 | 0x004d84b0 |
| NewRumble | 0x0054a850 | 66 | 0x004890c0 |
| SphereSphereOverlap | 0x0054fc70 | 101 | 0x004904d0 |
| InitPartDebris | 0x00571890 | 352 | 0x004a2ed0 |
| InitParts | 0x0064a4b0 | 643 | 0x0041d640 |

The 1,348 bytes add inventory coverage only. The two initializers retain their
internal alignment, including seven bytes in InitPartDebris and twelve in
InitParts. InitParts receives three physical caller words, although its body
uses only two. The sphere helper mutates its outgoing argument storage. The
rumble conversion and camera random helpers have reviewed local stack, register
and floating-point effects under their stated normal input/provider domains.

ReadPad at 0x0054ae40 (591 bytes) and GizmoBlowUp_Hit at 0x00569c70 (1,046)
are journal-only identity anchors with reviewed complete extents. Their ABI and
type flags remain false; unresolved installed callbacks and child contracts
remain explicit. They add no CSV bindings, allocation sizes, source definitions
or coverage. These anchors were tested separately after the five mapped entries
to distinguish their propagation effects. Original and independent review
packets and the three frozen experiment arms remain under ignored analysis.

## Shared-caller and level initialization anchors (2026-10-02)

Seven more original function identities have complete PC extents and qualified
physical caller contracts, with historical ctypes left blank:

| Function | PC entry | PC bytes | Native Android entry |
| --- | --- | --- | --- |
| StoreProgressAICharacter | 0x00414f80 | 119 | 0x000fd340 |
| Customiser_InitNames | 0x00423bc0 | 99 | 0x001b8f90 |
| LSW_SetIndy | 0x0042aa00 | 489 | 0x0011dc90 |
| FixUpLevels | 0x0048da50 | 6860 | 0x0012b370 |
| MenuLoadTechnicalStrings | 0x0050b4e0 | 867 | 0x00428340 |
| DieRumble | 0x0054a9b0 | 90 | 0x004894b0 |
| Collection_CreateCustom | 0x00594640 | 464 | 0x004dbbd0 |

These 8,988 bytes add inventory coverage only. FixUpLevels retains the internal
three-byte alignment omitted by the analysis body count; Collection_CreateCustom
also retains its internal three-byte alignment. Original callers establish one
pointer word for FixUpLevels, four words for MenuLoadTechnicalStrings and twelve
for Collection_CreateCustom. Canonical normal music, string and engine provider
interfaces remain explicit qualifications, without universal exception/runtime
claims. The progress helper's private integer shift and the customiser's string
copy, Indy setter and rumble helper effects are independently inspected.

AddFade at 0x005b11e0 (56 bytes, native 0x004ff560) is journal-only pending a PC
reconstruction linkage decision. Its thiscall input and one RET4 argument are
qualified for the four observed static virtual-target objects. Identity, extent
and qualified ABI are reviewed; historical class, return type and arbitrary
replacement vtables remain held. It contributes no CSV or coverage bytes.

The neutral data binding TerrainCountPc at 0x02962000 corresponds to native
terraincnt at 0x0122c020. Two mapped function pairs provide original accumulator
and reset instructions; the neighboring curSphereter alternative has only reset
evidence and remains held. PC increment-by-one is not established. The CSV size
and ctype stay blank, with ABI/type/extent flags false and no allocation or source
credit. The Android four-byte symbol is not a recovered PC allocation size.

## Shared-helper reviews and debris rival correction (2026-10-02)

Four further identities have complete PC extents and qualified physical caller
contracts. Their historical ctypes remain blank:

| Function | PC entry | PC bytes | Native Android entry |
| --- | --- | --- | --- |
| AISysGetPathPos2 | 0x0043e040 | 321 | 0x00181410 |
| GameCam_Judder | 0x00549de0 | 301 | 0x004d8370 |
| Player_ResetContexts | 0x0055e4e0 | 280 | 0x0046b240 |
| InitGameDebris | 0x0062c400 | 339 | 0x003ca2d0 |

The 1,241 bytes add inventory coverage only. The first three helpers have three
distinct accepted caller roots each. Original instructions and caller contracts,
rather than equal empty data profiles, establish their identities. AISysGetPathPos2
requires four pointer words; several callers supply two additional unused words.
Its already mapped character-path provider retains an explicit normal engine
qualification. Camera and context helpers preserve their lower floating-point
stack values under their reviewed vector, random and square-root contracts.

InitGameDebris builds an aligned arena with a twelve-byte header, twenty-byte
entries, name copying and debris lookup. Its complete extent retains internal
alignment and both returns. One observed caller supplies six words, another
supplies an unused seventh. The structurally competing PC 0x0068df20 is instead
a mode-dependent file/parser loader managing eight slots. Its proposed pair
against Android 0x003ca2d0 is explicitly rejected in the append-only journal.
The correct arena initializer was a rival with only one accepted parent root;
multi-parent support alone did not settle this identity.

ObjHitObj at 0x00438230 corresponds to native Android 0x0013a9e0. Original
control flow, twenty-one accepted callee families and distinctive collision
state/actions establish this journal-only identity. Its complete 5,764-byte PC
extent retains internal alignment and four returns. Six physical caller words
are observed, but complete child-call contract closure remains pending; ABI and
type flags stay false. It adds no CSV binding, source definition or coverage.
Frozen original and independent release packets remain under ignored analysis.

## Collision-tree leaf contracts (2026-10-02)

Seven original leaf identities have complete PC extents and qualified physical
caller contracts, with historical ctypes left blank:

| Function | PC entry | PC bytes | Native Android entry |
| --- | --- | --- | --- |
| NewBuzz | 0x0054a8a0 | 40 | 0x00489130 |
| KillRumble | 0x0054aa10 | 94 | 0x00489530 |
| HitRumble | 0x0054aa70 | 108 | 0x004895b0 |
| TakeHitRumble | 0x0054aae0 | 92 | 0x00489630 |
| Player_HasInvincibility | 0x0055f1d0 | 49 | 0x0046d0d0 |
| Player_HasFastBuild | 0x0055f210 | 49 | 0x0046d140 |
| AnimMiscFlags | 0x0062a6a0 | 36 | 0x003c678b |

The 468 bytes add inventory coverage only. Rumble helpers are distinguished by
their original amplitudes and durations: KillRumble uses 0.7 and 0.1, HitRumble
uses 0.5 and two frames, while TakeHitRumble receives its duration. The native
frame conversion is inspected rather than transferring a native frame-rate
layout. The reviewed FTOL primary entry consumes one owned floating value and
preserves lower values under finite representable inputs and masked normal
exceptions. Combined rumble headroom is three slots. HitRumble deliberately
reuses its incoming object argument word as float scratch; the actual caller
discards that word after the call. The two nearby caller sites are alternative
branches, not sequential calls.

The player predicates have distinct original cheat masks and object timer tests.
AnimMiscFlags is a call-free integer table lookup with a sentinel and optional
table pointer; its two caller words and EAX bit test are verified. Its native
unaligned entry is confirmed by the original ELF symbol record. Historical
argument types and allocation capacities remain held.

ObjHitObj remains journal-only with ABI/type flags false. Several immediate
integer contracts now close, but paths reach Player_ClearContext's optional
callback and other unresolved helper effects. Original code installs a callback
at 0x00429010 into slot 0x0093d848; four child effects remain held. The native
slot calculation is proved through the complete get-PC thunk and real GOT
mapping, but the initializer pair is not an accepted anchor. These research
contracts supply no extra CSV, data allocation or source coverage.

## Shared state and level-data identities (2026-10-02)

Three global identities have independent original instruction and native
symbol/GOT review. Their PC sizes and ctypes remain blank:

| Reconstruction name | PC entry | Native Android entry |
| --- | --- | --- |
| ForceBackPosPc | 0x0087b8ec | 0x006a2520 |
| MAULA_LDATA | 0x0087b0d8 | 0x006a15e0 |
| DOOKUC_LDATA | 0x0087b13c | 0x006a1570 |

The force-back reset, setter and directional movement pairs provide distinct
pointer roles: the position cell receives either actor+0x80 or the second setter
argument. The companion actor cell remains separate and unproposed. This does
not establish a PC pointer allocation extent or a recovered C type.

Similar field stores initially suggested the wrong Maul-to-Dooku correspondence.
Original PC constructor strings identify Maul_A and Dooku_C; the returned values
are stored in their respective globals. Original PartKill selectors distinguish
Maul's 0x2d/9 case from Dooku's 0x34/10 case on both platforms. The wrong
0x0087b0d8-to-0x006a1570 pair is explicitly rejected. Native MAULF and Explode1
alternatives remain held rather than being rejected through absent edges.
Platform field offsets differ and are not transferred. Native GOT addends,
relative relocation records and exact ELF symbol records are checked together
through their actual load segments. These rows add zero allocation, mapped-image,
source or exact-match credit; ABI/type/extent flags remain false.

## Force-back reference expansion (2026-10-02)

The reviewed position global exposes SetForceBack at PC 0x004a4900 against native
Android 0x0015f830. Its complete 58-byte PC leaf is mapped with a blank ctype.
Both originals select actor+0x80 or a fallback vector, then store magnitude,
its squared value and a mode. Actual PC callers supply four stack words and
clean sixteen bytes. The leaf preserves nonvolatile registers and stack state;
its owned floating stack returns to depth zero with a peak of one, qualified
for valid storage and normal masked floating-point operation. These 58 bytes
add inventory coverage only; the companion globals' sizes remain unknown.

ForcedBackCode at PC 0x004a4a20 against native 0x0015fa00 has a reviewed identity
and complete 635-byte extent, including two returns. State 22, distance/radius
modes, timers, context changes and effect/rumble branches establish the match.
It remains journal-only with ABI and type flags false. The effect chain reaches
unreviewed 0x006561d0, while Player_ClearContext's optional callback also remains
unresolved. Three immediate private matrix leaf contracts are conditional local
evidence; they do not close the whole routine. Frozen original, independent and
physical-hold supplements remain under ignored analysis.

The same data-reference expansion identifies DrawParaphernalia at PC 0x0041e4c0
against native 0x0010b770 and ForceThrowCode at PC 0x004a4420 against native
0x0014c8a0. Independent originals verify distinctive branch, level, geometry
and throw-state behavior alongside fifteen and six accepted callee relationships.
Their complete PC extents are 4,024 and 1,223 bytes. DrawParaphernalia retains
three bytes of internal alignment and four returns; ForceThrowCode has three
returns and a tail jump through 0x00576960 to an unresolved provider. Its PC
entry receives an object in EAX plus one stack word, unlike the native specialized
function's source signature. Both identities are journal-only with ABI/type
flags false; unresolved private providers prevent CSV or source release. The
5,247 reviewed bytes add no mapping or reconstructed-code coverage.

The actor global is independently identified as `_ForceBackObjPc` at PC
0x0087b8e8 against native 0x006a2570 (`_ZL12ForceBackObj`). Original SetForceBack
stores, reset clears and ForcedBackCode exclusions establish its role; the
native address comes from the actual PIC calculation, not adjacency to the
position global. Its four-function consumer cohort also exposes DrawForceBackEffect
at PC 0x004a4940 against native 0x0015f880 for separate review. The data row has
blank size and ctype: the native four-byte allocation does not establish the PC
allocation, and this identity adds no mapped-image or source coverage.

The DrawForceBackEffect review finds an existing extent error in
NuSpecialSetVisibility at 0x005cfc00: its mapped 184 bytes omit the shared return
at 0x005cfcb8. Independent original decoding verifies all 185 bytes, four returns
and five conditional branches to that last return. Seven following alignment
bytes remain excluded before NuSpecialSetCollision. The row is corrected to
185 bytes with its existing name and ctype preserved; this adds no new identity
or ABI claim. Matching is refreshed against the complete range.

DrawForceBackEffect itself is independently reviewed at PC 0x004a4940 against
native 0x0015f880. Its nullable actor and mode-3 visibility rule, 64-byte matrix
copy, actor position, radius scale and draw-matrix update agree. The complete
220-byte PC body has two returns; four following alignment bytes stay excluded.
Its entry reads one stack word, but an actual PC caller and downstream provider
effects remain unverified. It is therefore a journal identity/extent anchor with
ABI/type false and no CSV, source or coverage credit.

## Hint-state reference expansion (2026-10-02)

Hint_SaveGameState at PC 0x005766e0 against native 0x004e7690 is independently
mapped with its complete 99-byte extent and a blank ctype. Both originals scan
16-bit IDs up to a 0xffff sentinel and set the matching index in a saved bitmap.
The PC inlines the operation; native Hint_SaveBits calls SET_HINT_COMPLETE.
The original table strides are 0x1c and 0x24 respectively, so neither layouts
nor native bitmap capacities are transferred to the PC.

Actual PC callers pass one pointer through a push and four-byte cleanup or an
argument rewrite before a tail jump. The PC leaf is call-free, uses no floating
stack or private register/flag contract, and preserves stack and nonvolatile
registers. The physical ABI is qualified for a finite readable sentinel table
and valid bitmap/index storage. Historical formal types and allocation extents
remain unknown. This adds inventory bytes, with no source or exact-match credit.

The reviewed SetForceBack producer and its DrawForceBackEffect/ForcedBack users
also identify radius at PC 0x0087b8b0 against native 0x006a2590, squared radius
at PC 0x0087b8b4 against native 0x006a25a0, and mode at PC 0x0087b8f0 against
native 0x006a2580. The producer writes the unchanged third float, its square,
and the raw fourth word respectively. Draw scales by radius and tests mode 3;
Forced uses radius in distance arithmetic and tests modes 2/3. Squared radius
has one indexed writer and no indexed reader on either platform; this does not
prove absence of aliases or other consumers. Exact native local OBJECT names
and original PIC arithmetic confirm the distinct identities. Crossed radius/mode
and radius/square bindings are explicitly rejected.

All three PC rows leave size and ctype blank. Radius and squared radius retain
their actual data-site graph IDs; mode has an object ID, which still does not
establish its allocation boundary. Native four-byte extents and zero-filled
loader views do not prove PC allocation sizes. These data anchors add no mapped
bytes, source or exact-match credit and do not discharge consumer ABI debts.

The ForceThrowCode camera call identifies GameCam_Blend at PC 0x0054a3e0
against native 0x004d8b20. Independent original review confirms the camera
snapshot/blend algorithm, complete 298-byte body, two returns and six excluded
alignment bytes. The actual caller supplies camera, duration, secondary float
and mode in four stack words. Its deferred 24-byte cleanup also includes the
following call's eight bytes. The call-free leaf preserves stack/nonvolatile
state and a valid lower x87 prefix with three free registers; its own x87 use
has net zero and peak three. Native unsigned CMP/SBB and PC SETNE encode zero
as 1 and every nonzero 32-bit mode as 2. No boolean-only restriction is needed.

The row has a blank ctype. Its ABI is qualified for valid initialized nonalias
camera storage through the accessed prefix, a valid default for NULL, and the
normal engine floating environment. Historical types, return type and full
allocation remain unknown. The earlier inferred PC 0x004a1fb0 binding to this
native camera function is rejected: its original is a private actor-target
helper with different state writes and register inputs. This adds 298 inventory
bytes without source or exact-match credit; ForceThrow's remaining ABI debts
are unaffected.

### Character-model private helper identities (2026-10-02)

Original review of APILoadCharacterModels and its six PC/native helper bodies
confirms three journal-only pairs:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x0062d2e0` | `0x003cbde7` | APICharacterModelReset | 125 |
| `0x0062d360` | `0x003cbf1f` | LoadAnimFromPAK | 147 |
| `0x0062d400` | `0x003cc039` | LoadAnim | 148 |

Reset clears record fields, three count-scaled pointer arrays and a 64-byte
tail. The loaders share a name cache, counters and slot selection, but the
packed-buffer path passes two values to PC `0x005f6420`; the filename path calls
NuAnimData2LoadBuff with a filename, third word and address of a copied fourth
word. The parent supplies one/four/four stack words respectively and stores
the loaders' EAX results. The native extra name-normalization call remains a
platform difference. Exact original ELF GLOBAL/LOCAL records supply the native
names; source annotations alone do not establish them.

Independent original instruction and branch review confirms the complete PC
spans, including internal flow and excluding following CC padding. All six
crossed role pairs are explicitly rejected. Generic Pcode shape leaves the two
loaders tied, demonstrating why common parent membership is insufficient.
Unknown reader/string-provider physical and floating-point effects keep ABI
and types held. These rows enter the decision journal only: no CSV declarations,
source, inventory coverage or matching gain is claimed. The retained independent
packet is `structural-api-character-private-triad-independent-next1.json` under
ignored continuation analysis. Next review the four actual loader providers,
starting with the unmapped raw-buffer reader at `0x005f6420` and its full extent.

### Character arena writer and packed-buffer dependency (2026-10-02)

The character-model family opens two more reviewed identities in the journal:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x005f6420` | `0x002bc4d0` | NuAnimData2LoadBuffFromPAK | 69 |
| `0x00630b40` | `0x003cba8f` | APICharacterSysInit | 541 |

The packed-buffer reader returns NULL for zero size, then routes the signed
header comparison to pointer-block fixing or animation fixup. PC `>= ANI5`
and native `> ANI4` are equivalent. Both returns and the following eleven CC
bytes establish its complete PC boundary. The filename wrapper's incoming-ECX
scratch word and backend write-before-read behavior remain separate debts.
Original NuStrICmp and NuStrCpy are call-free under finite valid storage
conditions. Native NuStrCmp is case-sensitive; its standalone pairing with PC
NuStrICmp is rejected by the original `a`/`A` behavior. This platform difference
does not reject the corresponding animation-loader roles.

The initializer aligns an arena to sixteen bytes and reserves/clears its
56-byte header, 84-byte model records, three pointer arrays per model, two-byte
remap entries and 52-byte animation entries. These are observed PC operations,
not copied native layouts or a proposed aggregate type. Actual callers supply
eight words and discard EAX. The unused second word, PC optional existing-slot
action and differing incidental returns keep formal types and return behavior
unresolved. Complete branch review reaches both PC returns through byte 541;
three following CC bytes and the separate RET at `0x00630d60` are excluded.

The corresponding original writer and three reviewed helper reads identify
PC `apicharsys` at `0x02913104` with the exact native GLOBALOBJECT at
`0x0123f918`. Native GOT cell `0x006122f0` supplies that address and is not an
object alias. Both initial zero views and four-byte accesses are verified.
The new data journal row reviews identity only: it does not establish a PC
allocation extent or transfer the native size/type. Existing CSV/source
inventory is preserved. Both functions retain ABI/type holds on their
providers; these 610 reviewed code bytes add no inventory or reconstruction
coverage. Original producer/peer packets are frozen under ignored continuation
analysis with `structural-api-animation-provider-*wave35*` and
`structural-apicharsys-writer-*wave35*` names.

### Character state animation and mini-packet update (2026-10-02)

Two new inferences from the reviewed `apicharsys` anchor pass independent
original identity and complete PC boundary review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x004a8f20` | `0x0016ae60` | Animate_CHARACTER | 1,024 |
| `0x0062f0a0` | `0x003cf1e7` | UpdateMiniAnimPacket | 85 |

Animate_CHARACTER shares distinctive class/state selection, timer updates,
signed remap access, model selection and two mini-packet update paths with the
original native GLOBAL function. Native movement selection adds operations,
and native SSE/PC x87 unordered comparisons differ. Identity does not establish
source or floating-point equivalence. The complete PC CFG includes five returns
through `0x004a931f`; the next analyzed entry at `0x004a9320` is distinct even
without intervening CC padding.

Both reviewed callers supply five words to UpdateMiniAnimPacket twice. The
helper converts a mini packet into a local full view, calls the full updater
with six words including an additional zero, then converts back. Its PC local
reserve and outgoing argument staging remain observed operations, not a
transferred native type. The complete 85-byte body ends at `0x0062f0f4`, before
eleven CC bytes. Conversion/updater effects, private animation providers,
caller contracts and aggregate types remain held.

The journal records both identities and complete extents; CSV/source and
coverage are unchanged. Producer and independent packets are frozen as
`structural-wave36-animate-mini-original-next1.json` and
`structural-wave36-animate-mini-independent-next1.json` under ignored
continuation analysis. GOT address carriage remains separate from the reviewed
`apicharsys` data object; no additional data allocation or type is claimed.

### Icon messages and shared message pool (2026-10-02)

Original PC/native instruction review and an independent review identify
Tag_DrawIcon_Batman at PC `0x004bbd10` / native `0x00144c00`, and AddGameMsg at
PC `0x00560b60` / native `0x004a4ce0`. The first has a complete 927-byte PC body;
the second requires all 1,078 bytes, including seven internal alignment bytes
and the final jump back into its body. Ghidra's 1,071-byte owned-range count
does not describe the complete message function.

The icon routine shares modal/fade/actor gates, color selection, averaging of
sixteen model attachment positions, three message emissions and the final
165/166 icon selection. AddGameMsg shares a 128-slot circular pool with a
`0x114` record stride, free/replacement selection, callback gates, a bounded
119-byte text copy and a distinctive group of input-to-record assignments.
PC x87 and native SSE arithmetic, actor offsets, clearing code and stack-cookie
handling differ. These witnesses support identity without transferring layouts
or exact floating-point behavior.

Both entries are journal identities with complete PC extents. Unresolved message
callbacks and provider effects keep caller ABI, aggregate types, source and CSV
release held. Message-pool and cursor addresses remain binding hypotheses.
Evidence is frozen under ignored continuation analysis as
`structural-wave37-tag-message-original-next1.json` and
`structural-wave37-tag-message-independent-next1.json`.

### Message-pool and cursor data identities (2026-10-02)

Original AddGameMsg and ResetGameMessages review, independently repeated,
identifies the PC pool base `0x00947be0` with native `0x01298800`, and the PC
circular scan/replacement cursor `0x009505e0` with native `0x012a1200`.
The native pool is the original GameMessage OBJECT; GOT carrier `0x00616318`
and its relative relocation carry the pool address. The cursor endpoint follows
the original PIC reads and writes; its annotated native local name remains
advisory until its exact symbol record is established.

Both algorithms address 128 records at stride `0x114`. Reset clears each
record's active byte only and does not reset the cursor. The native 35,328-byte
symbol and equal base-to-cursor gaps do not establish the PC allocation extent.
Both journal rows therefore release identity only, with sizes, aggregate types,
ABI and source/CSV release held and no coverage credit.

PC `0x00947cd8` is the active-byte member at pool offset `0xf8`; its candidate
pairing with the whole native owner remains an ambiguity. Only explicitly
reviewed member/site incidences may support a partial-owner diagnostic; no
arbitrary interval is normalized. Evidence is frozen as
`structural-wave38-message-data-original-next1.json` and
`structural-wave38-message-data-independent-next1.json` under ignored analysis.

### Message update and ID search (2026-10-02)

Two function hypotheses opened by the message-pool/cursor anchors pass
independent original identity and complete PC boundary review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x00561020` | `0x004a5210` | UpdateGameMessages | 718 |
| `0x005618e0` | `0x004a5e60` | FindGameMsgsWithID | 88 |

The updater shares 128-record active/ended handling, lifetime progression, four
easing selectors, position/scale/alpha interpolation and three callback fields.
Its complete PC CFG ends after RET at `0x005612ed`; padding and the distinct next
entry are excluded. PC x87/native SSE and callback/provider effects differ.

The finder shares signed ID/sub-ID matching with a minus-one wildcard, record
exclusion, match counting and optional active-byte clearing. Its complete PC
body ends after RET at `0x00561937`. The native function uses four specialized
loops, explaining its larger extent. Its internal stack observations do not
qualify the still-unreviewed external caller contract.

Both journal rows release identity and complete extent, with ABI/type held.
They add 806 reviewed PC code bytes and no CSV, source or reconstruction coverage.
Producer and independent evidence are frozen as
`structural-wave39-message-children-original-next1.json` and
`structural-wave39-message-children-independent-next1.json` under ignored
continuation analysis.

The normal propagation pass retains all previous pairs: accepted functions rise
from 1,519 to 1,521 while inferred functions fall from 1,321 to 1,319. The union
remains 2,840 and all 2,416 DATA candidates remain identical. These two reviews
promote existing hypotheses; they do not demonstrate another discovery burst.

The auxiliary exact-site owner trial preserves all 99 AddGameMsg references.
Only the two reviewed indexed active-byte sites can be associated with the pool;
97 references retain unknown ownership. Four declared parent cross-pairs remain
held, with no bilateral distinguishing evidence, new roots or scoring changes.
The field-role trial likewise leaves all four animation-loader cross-pairs tied.
Further structural discrimination needs value ancestry and control-flow context,
while retaining the platform/provider qualifications. These experiments remain
under ignored analysis and do not alter the maintained scorer.

### Rocket firing and security-door movement (2026-10-02)

Independent original review confirms the other two function hypotheses opened
after the message-pool/cursor review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x004a69e0` | `0x001666d0` | FireBountyHunterRocket | 442 |
| `0x004bdaf0` | `0x001e1630` | SecurityDoor_MoveCode | 436 |

The rocket routine shares the idle-state weapon/action lookup, capability-27
gate, message lookup, 2/77 mode selection and animation/timer/state-14 setup.
PC additionally handles an already-active state-14 update and has a live ESI
private input. These differences remain explicit; the native firing function
does not supply the PC's complete behavior or caller contract.

The door routine shares state-`0x5c` frame/completion handling, `0x20`/`0x40`
flags, special-object mode 2 and rumble 0.6 with argument zero. PC delegates
activation to a private helper at `0x004bda50` with EAX and one stack word;
native inlines that setup. Helper and floating-point return effects remain held.
Both complete PC CFGs and following RET/CC/next-entry guards are verified.

The two journal rows add 878 reviewed PC code bytes, with ABI/type held and no
CSV/source/coverage credit. Evidence is frozen as
`structural-wave40-pool-consumers-original-next1.json` and
`structural-wave40-pool-consumers-independent-next1.json` under ignored analysis.

A separate original symbol/name review and independent peer verify native
cursor spelling `_ZL9i_gamemsg` at `0x012a1200`, from the exact LOCAL OBJECT
record and terminated string. This closes the earlier spelling hold only;
its native four-byte size does not establish PC extent or type. The immutable
supplement packets are `structural-message-cursor-name-original-next1.json` and
`structural-message-cursor-name-independent-next1.json`.

The normal control retains every previous pair, promotes these two functions
and adds one inference: PC `0x004bd3d0` / native `0x001e12f0`. Its observed roots
include the reviewed door mover and NuVecDistSqr. Accepted/inferred counts are
1,523/1,318, the union grows to 2,841, and all 2,416 DATA pairs remain unchanged.
The new inference still needs original identity and boundary review.

The native pool's indexed active-byte read at `0x004a4d8c` is independently
verified through its original PIC/GOT load and index arithmetic. It supplies
the third proved member/site in the 99-reference diagnostic; 96 remain unknown.
The extra same-site native `+0x20c` target stays unqualified. The diagnostic now
groups both already-reviewed AddGameMsg users under the pool, without adding an
independent root, scoring change or new identity. This is a local indexed static
reference proof, not a complete runtime address or a general field-offset map.

### Message rendering and nearest security-door search (2026-10-02)

Two connected hypotheses pass independent original identity and full PC extent
review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x00561400` | `0x004a5720` | DrawGameMessages | 1,235 |
| `0x004bd3d0` | `0x001e12f0` | SecurityDoor_FindNearest | 207 |

The message renderer shares 128-slot lifecycle gates/easing, screen clamp and
interpolation, custom callbacks, object/text branches, alpha/color handling and
the optional extra-object callback. The full PC extent includes thirteen
interior JMP/LEA alignment bytes absent from Ghidra's 1,222-byte owned count;
its sole RET at `0x005618d2` and following CC/distinct-entry guards close the body.
Renderer, callback and floating-point conversion effects remain held.

The door search shares the special-array/count loop, `0xc0` stride, optional
filter, flags/state/zero-field predicate, strict nearest-distance selection and
optional output distance. The native combined word mask enforces the same flag
and zero-state conditions for all 65,536 input words. Original constants confirm
the float minimum of 1e9; equal distances retain the first candidate and unordered
distances are rejected. The complete 207-byte PC body ends after RET at
`0x004bd49e`. Its reviewed parent passes four words, but unqualified NuVecDistSqr
effects keep the complete caller contract unresolved.

Both journal rows release identity and extent only: 1,442 reviewed PC code bytes,
with ABI/type held and no CSV/source/reconstruction credit. Original and peer
evidence are frozen as `structural-wave41-message-door-original-next1.json` and
`structural-wave41-message-door-independent-next1.json` under ignored analysis.

The normal control promotes the two existing round-one inferences and retains
all previous pairs. Accepted/inferred counts become 1,525/1,316; the union remains
2,841 and all 2,416 DATA pairs are identical. These reviews add no new inference
or reconstruction credit.

The generic value/control-flow annotation trial preserves the prior field and
pair results exactly. It records a conditional writer access-order motif,
write A/read A/write B/read B, but every writer STORE value ancestry remains
unknown and all four loader cross-pairs still tie. Both native loaders call the
same unqualified helper at `0x003cbed2`; its imported graph label is not an exact
original symbol or an effect certificate. That original callee and the held
reader/Reset providers are the next field-evidence frontier. Prototype fixtures
and independent actual audits pass, while the interpretation helper's source
budget and the call-frontier setup-SQL accounting remain explicitly qualified in
their immutable scratch corrections. No scorer or mapping release relies on
those budget-compliance claims.

### Character effects and drawing: compound structural review

Two larger connected functions pass independent original identity and complete
PC extent review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x0062c7e0` | `0x003caa06` | AddAnimEffects | 2,808 |
| `0x0062fbf0` | `0x003d0563` | APIDrawCharacterModel | 2,690 |

The effect consumer matches animation selection, sentinel-terminated 52-byte
effect records, state and event-time filters, a sixteen-locator mask and averaging,
position/height alternatives, callbacks, bits-on/off updates, velocity/rate/random
gates, debris modes and sound dispatch. The drawing orchestrator matches frame
`0xffff` and evaluation-only flags, hierarchy bounds/clipping/layer selection,
DWA and ordinary animation dispatch, locator updates followed by effect
consumption, lighting/transparency, sixteen-point surface effects, reflection
start/render/end and cleanup gates. These compound behaviors distinguish the
functions beyond their shared data anchor and call counts.

Closed original control-flow graphs confirm both complete PC spans, including
nine and eight interior alignment bytes respectively. The effect consumer's
sole RET is at `0x0062c96f`; drawing returns at `0x0062fca5` and `0x00630671`.
Native original symbol records independently establish the two names. The
Draw-to-Add call corroborates their relationship; it adds no independent root.

The journal records 5,498 reviewed PC code bytes with blank signatures and held
ABI/types. PC field offsets, x87 predicates, callback guards and the unexplained
effect-consumer call to `0x006dea60` remain separate reconstruction obligations.
No CSV binding, source implementation or reconstruction coverage is added.
The producer and independent evidence are frozen as
`structural-wave42-character-consumers-role-original-next1.json` and
`structural-wave42-character-consumers-independent-next1.json` under ignored
analysis; earlier accounting failures remain disclosed in their original scopes.

The compound call contexts retain six unapproved child candidates: plain,
rotated, matrix and momentum debris emission, locator-coordinate storage and
transparent character drawing. They are candidates for original review rather
than automatic identities inferred from a parent.

The normal control promotes both prior round-one function hypotheses and retains
the exact 2,841-function union. Accepted/inferred counts become 1,527/1,314.
All 2,416 prior DATA pairs remain, with one additional candidate
`0x0097390c` / `0x011a7cb0`. That candidate remains unreviewed. The graph and
scoring rules are unchanged; this checkpoint adds no reconstruction credit.

The native loader normalizer's original effect proof and corrected two-entry
certificate metadata pass independent review. Its writable finite-string and
control-storage nonalias conditions are unproved for arbitrary loader inputs.
A separate scratch prototype therefore keeps such calls as barriers by default;
explicit domain assumptions preserve only reviewed effects and keep downstream
field roles conditional. All 51 fixtures pass independent review. Actual
discrimination remains an experiment, and no production scorer uses these
conditional certificates.

### Following the character call contexts

All six selected character callees pass independent original identity and full
PC extent review. Their branch and data roles distinguish helpers whose call
graphs alone look similar:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x0062c560` | `0x003ca5b6` | AddGameDebris | 72 |
| `0x0062c680` | `0x003ca789` | AddGameDebrisRot | 73 |
| `0x0062c6d0` | `0x003ca839` | AddGameDebrisMtx | 137 |
| `0x0062c760` | `0x003ca91d` | AddGameDebrisMom | 104 |
| `0x0062e1e0` | `0x003cd5ec` | StoreLocatorCoordinates | 171 |
| `0x0062fac0` | `0x003d03aa` | APITransparentCharDraw | 291 |

The debris family shares signed index bounds, twenty-byte record stepping and
a `-1` identifier gate. Plain emission supplies a sentinel pointer to a distinct
backend; rotation forwards angles; matrix emission constructs a quarter-turn
matrix and multiplies it by its input; momentum emission substitutes a local
zero vector for a NULL momentum pointer. The latter two share a backend, so
that call alone cannot distinguish them. All twelve crossed sibling alternatives
are rejected by their internal roles. Native rotation narrows two angle words
to signed sixteen-bit values; PC forwards full words. Backend effects and
complete interfaces remain held.

Locator storage evaluates sixteen presence slots and optionally copies twelve-
byte positions and sixty-four-byte matrices, leaving missing slots unchanged.
Transparent drawing removes selected type-list entries by swapping in the last
entry, brackets rendering with state changes, saves/restores an object byte and
conditionally restores removed entries. Its PC render-context calls and snapshot
at `0x00973910` are absent in native and remain explicit PC obligations. Neither
output allocation nor complete list equality is inferred.

A separate compound review confirms LOCAL `CutScene_DrawCharacter` at
PC `0x0057d3a0` / native `0x00498db0`, complete PC extent 3,767 bytes. Animation
transitions, matrix construction, sixteen-locator shadows, distance/material
reflection gates, bracketed rendering and attachment traversal establish the
identity. All PC instructions are reachable; RET at `0x0057e256` and the following
nine CC bytes close the span. Native extras and PC level/platform shadow gates
remain distinct.

These seven journal identities cover 4,615 reviewed PC code bytes. Original
names, bodies and bounds pass independent review; ABI/types, CSV bindings and
source reconstruction remain held. Frozen evidence uses the
`structural-wave43-debris-callees-*`, `structural-wave43-locator-transparent-*`
and `structural-wave43-cutscene-*` packets under ignored analysis. Parent call
contexts corroborate the matches without adding independent roots; the earlier
callee-context SQL accounting failure remains held in its original scope.

The field trial's default barriers and explicit conditional-domain policy pass
independent actual review. They add no field, value, ordering or pair-role gain
over the preceding trial. All four loader cross-pairs still tie; reader/provider
and Reset effects remain unresolved. Conditional register preservation alone
does not remove those later barriers, and it supplies no new mapping credit.

The newly proposed DATA pair also passes independent original identity review:
PC `0x0097390c` corresponds to native `nurndr_force_lod` at `0x011a7cb0`.
The reviewed display-list renderer reads it as a LOD selection bound; character
drawing writes one around reflection rendering and zero afterward. Original
native PIC/GOT transport and both original symbol tables confirm the same object.
The four-byte GOT carrier is separate from that object. Native symbol size and
observed four-byte accesses establish neither the complete PC allocation nor
its historical type or initializer. Its journal identity has unknown PC size
and held extent/type, adding one data anchor with no CSV or coverage credit.

The normal control retains every prior accepted/inferred function pair. Five
reviews promote earlier inferences; AddGameDebrisMom and StoreLocatorCoordinates
were previously candidates without inferred status. Seven further pairs become
inferred, expanding the function union from 2,841 to 2,850, with no loss or
reroute. Accepted/inferred counts are 1,534/1,316. This is candidate propagation,
not original review or source reconstruction of those seven additions.

DATA candidates change by fourteen gains and six removals, reaching 2,425.
One removal is the force-LOD pair promoted into the 121 reviewed data anchors;
the remaining candidate changes stay advisory. The observed root lineage does
not isolate a single cause for the gains. Graph, scoring and source inputs are
unchanged apart from the journal and documentation.

### Player replacement and startup structure

Three connected functions pass independent original identity and complete PC
extent review:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x0055eaa0` | `0x0046c730` | Player_ToggleCharacter | 1,787 |
| `0x0041f940` | `0x000fe280` | Players_Init | 1,355 |
| `0x0041bc20` | `0x000ff530` | NewPlayerCharacter | 1,273 |

Toggle handles directional input and repeat timing, searches the model list with
wrapping and availability gates, brackets replacement with callbacks and camera
effects, then remembers both player IDs. NewPlayerCharacter rejects invalid IDs,
preserves selected old-object state across replacement, resets context and
animation packets, and initializes surface state in the same ordered sequence.
Players_Init clears eight player slots, selects remembered/default IDs, creates
bounded player contexts and establishes backup snapshots. The PC snapshot copy
is `0x10d8` bytes; native copies `0x10e4`. Native allocation/init combines separate
PC calls, and several finishing paths differ in inlining and platform behavior.

All six crossed alternatives fail their compound algorithm roles. Independent
original CFGs establish every PC instruction as reachable and close each full
span with RET and following CC/entry guards. Original native symbol sizes include
alignment and remain separate from analyzed reachable bytes. Incoming private
ABIs, unknown providers, callbacks and complete aggregate types remain held.

These three journal identities cover 4,415 reviewed PC code bytes. They supply
no CSV binding, source implementation or matching coverage. Frozen producer and
peer packets are `structural-wave44-connected-parents-original-next1.json` and
`structural-wave44-connected-parents-independent-next1.json` under ignored
analysis. The previous frontier's accounting failures remain preserved.

The normal control promotes all three existing round-one inferences, retaining
the exact 2,850-function union with no new pair, loss or reroute. Accepted/inferred
counts become 1,537/1,313. DATA candidates gain twenty-eight pairs and lose three,
reaching 2,450; reviewed data anchors remain 121. These candidate changes receive
no identity or extent approval. An initial run also enabled the separate
tentative function-to-data diagnostic channel. Its core output hashes agree with
the normal control; the additional 9,813 diagnostic rows do not feed function
retrieval or accepted state. Both runs and their memory measurements are retained.

### Lighting and character-scene propagation

Following the new cutscene hypotheses confirms four further original identities:

| PC entry | Native entry | Identity | Complete PC bytes |
| --- | --- | --- | --- |
| `0x005958b0` | `0x004a6a40` | SetZeroLights | 96 |
| `0x00595910` | `0x004a6b10` | FindAndSetLights | 92 |
| `0x0055b520` | `0x00460260` | CharScene_FindHSpecial | 80 |
| `0x0055b570` | `0x004602d0` | CharScene_Draw | 63 |

The lighting routines distinguish a fixed zero-vector/ambient setup from an
ordered scratch-context transaction: disable, search with incoming position and
context, enable, then commit. Character-scene lookup tries global override and
world fallback arrays, selects a sixteen-byte entry and checks its handle.
Drawing holds the lookup result and conditionally renders each of two matrices.
All twelve crossed roles fail their compound algorithms. Original native records
and complete PC CFGs pass independent review; adjacent functions without CC
padding are closed by their independently verified next entries.

These four journal identities cover 331 PC bytes. Unknown lighting/render
providers, PC private register setup, platform differences and full signatures
remain held. No CSV, source or coverage credit is added. Frozen original and
peer evidence uses `structural-wave45-lighting-charscene-*` under ignored analysis.

The generic call-context diagnostic now has a maintained command and 32 controls.
Independent actual review of all 36 pilot pairs confirms two additional unique
correct ranks from external call anchors/order. The remaining debris ties stay
explicit, and local push-origin observations add no shared support in this pilot.
The tool supplies advisory structure only; automatic identities and production
graph scores remain unchanged. Its earlier stale-origin bugs and held prototype
receipts remain preserved alongside the corrected source and actual reviews.

The normal control promotes all four earlier inferences and retains every prior
function pair. One new hypothesis, PC `0x00595660` / native `0x004a6670`, expands
the function union to 2,851, with 1,541 accepted and 1,310 inferred pairs. Its
lineage includes the newly reviewed lighting transaction, existing anchors and
literal evidence; no isolated cause or original identity is inferred. All 2,450
prior DATA candidates remain, with eight gains reaching 2,458. Reviewed DATA
anchors remain 121. Graph/scoring rules and reconstruction coverage are unchanged.

## Connected identity and source release (wave 46)

The player/character cohort now exposes five independently identified, complete
PC functions as inventory with blank `ctype`: `Players_Init` (`0x41f940`, 1355),
`NewPlayerCharacter` (`0x41bc20`, 1273), `Player_ToggleCharacter` (`0x55eaa0`, 1787),
`AddAnimEffects` (`0x62c7e0`, 2808), and `APIDrawCharacterModel` (`0x62fbf0`, 2690).
Actual callers supply respectively 0, 4, 3, 10 and 17 physical stack words;
complete own normal-return paths balance ESP and owned x87 state under the
recorded normal-child preservation conditions. These 9913 bytes remain
unimplemented. Formal types, callback/private-child behavior and source closure
remain held. `CutScene_DrawCharacter` retains its identity/extent review, but
registration alone has not established its actual dispatch interface.

`NuHGobjPOIMtx` (`0x5daae0`, 72 bytes) has a reviewed five-word cdecl interface.
The partial hierarchy view now names the point-record pointer at `+0x184` and
byte index map at `+0x18c`. An 80-byte record contains a matrix at zero and joint
index byte at `+0x44`. Genuine C performs the record/joint product followed by
in-place world composition. The existing three-word D3DX matrix-multiply
boundary is retained as a qualified normal imported-math contract; no complete
hierarchy allocation or DLL implementation is inferred.

Both buffer evaluators now have genuine C in `nuanim.c`:
`NuAnimBuffEvaluate_3_QuatB` (`0x5e96d0`, 1720 bytes) and
`NuAnimBuffEvaluate_3` (`0x5e9d90`, 1325 bytes). They use the existing verified
joint, buffer, hierarchy and callback views. Parent/flag branches, six sign
changes, callback aliasing and one-time root callback, identity tails and
callback cleanup follow the PC original. The private quaternion product uses
explicit SSE products and their original add/subtract grouping, including
zero-minus-product W lanes. VC8 lacks `_mm_cvtss_f32`; supported `_mm_store_ss`
expresses those four scalar stores without changing the reviewed operation.
The original quaternion path writes quaternion slot 255 and reads scale slot
255 despite smaller allocator requests. Source preserves this behavior under
explicit physical scratch backing and initialized scale-slot conditions;
successful allocation requests alone establish neither. Callback-table
initialization, valid engine counts/indices and normal math-provider conditions
remain explicit qualifications. No root-scale initializer was invented.

`NuCameraSetEx` (`0x5ed540`, 1105 bytes) now brings the camera copy, ordinary and
reflection matrix paths, perspective/orthographic selection, clip-mode change
ordering, matrix products and clip-plane math into comparison. `NuCameraSet`
calls it with `(camera, 0)` and uses the original `NuMtxMulH` entry. Viewport,
scissor-ratio, frustum/backend submissions and renderer-cache work remain
explicit `STUBBED_NAMED` actions; this builder remains partial.
`NuCameraSetProjectionMtx` (`0x604a80`, 200 bytes) preserves its five arguments,
F32 checkpoints and four additional updates to global `pmtx` even when the
supplied output is another matrix. Source uses explicit volatile locals for
required F32 storage rounds. `NuMtxInvVU0` (`0x5c0940`, 212 bytes) has the same
complete original bytes as the existing inverse and reuses its genuine C body;
scale and rotational-multiply variants retain their actual five-byte tail
entries. `NuMtxMulH` retains its distinct 21-byte D3DX forwarding entry.

Twelve camera data bindings are minimum source views with unknown original
allocation extents and blank CSV sizes/types. Original image evidence supplies
an identity `psmtx`, the `0.1f` orthographic scalar and ten loader-zero views.
They receive no data coverage or initializer-match credit. The projection Y
offset may lie inside the unresolved coordinate-stack owner; no independent
historical global allocation is claimed.

Frozen independent authorities are retained under ignored
`build/analysis/identification/continuation/`: the player and render interface
packets, POI matrix source audit, animation source audit, camera source audit and
camera data initializer packet. Android source remains attributed to
`src/nu2api/nucore/nucore_plain.cpp` and the animation buffer evaluator family;
PC original bytes, caller roles, constants and field offsets establish this
release. Compiled comparison and reconstruction metrics are reported separately
in `matching.json`.

The wave-46 VC8 build and independent emitted-code reviews pass for the
qualified animation and partial camera contracts. All 125 objects have complete
bindings and resolved relocations; full checks pass 640 tests with three skips.
Retail comparison adds 4665 original code bytes and five exact functions
totalling 315 bytes: POI matrix composition, inverse, scale, rotational multiply
and the distinct D3DX forwarding entry. Whole-code fuzzy coverage increases
from 6.471500% to 6.550184%. Camera backend markers and the original animation
scratch-backing qualifications remain; these results do not establish a
complete renderer or playable game.

### Wave 47: viewport/backend and hierarchy evaluation

`NuHGobjEval` (`0x5d36d0`, 456 bytes) now evaluates the connected hierarchy
using the verified joint parent, bind-matrix pointer and 52-byte override
record. The PC loops retain their byte counters, remap sentinel, matrix aliases
and double-precision angle conversion. These are minimum working views with
asserted offsets; the original complete object types and historical return
signature remain unresolved. Valid engine counts, readable joint/bind arrays
and the existing matrix-provider contracts qualify this implementation.

The viewport family adds six complete original bodies (1140 bytes): the
scaling-template builder, signed fixed-point clipping/update, null-safe viewport
submission, the private PC backend method, matrix submission and null-safe
identity-copy provider. The private method keeps its actual thiscall receiver
and one stack argument; the indirect
COM calls use the original physical slots and cleanup. It submits a clamped
temporary viewport before caching the six original requested fields. Matrix
submission stores and returns the backend result. The final two camera calls
now submit `(2, &vmtx)` then `(3, &pmtx)`; the camera builder calls `NuVpUpdate`
only when its initial fast flag is zero. Other camera gaps remain explicit.

Original operands distinguish the mutable identity matrix at `0x80ba68` from
the existing identity template at `0x806680`. Review caught and corrected this
distinction before source integration. The viewport's constants are explicit
double-precision views of `0x761610` (1/65536) and `0x76a7c8` (16777215), preserving
their actual reference addresses. Twelve new data bindings have reviewed minimum
initializers but blank original allocation sizes/types, supplying zero data
coverage or exact-match credit. Backend objects and their normal API providers
must be initialized before these routines are used.

Complete camera comparison also distinguishes `NuVpSetIdentityPc` (`0x5db100`,
25 bytes): it accepts a nullable matrix pointer and copies the mutable matrix
at `0x80ba68`. `NuCameraSet` uses this distinct provider rather than the general
identity-template copier. A 100% advisory instruction score had hidden that
different external call destination; complete byte comparison exposed it.

The existing viewport/scissor projection builders now preserve the PC F32
checkpoints and distinct multiplication association. Ordered comparisons,
zero-only optional clip defaults and the fast path are retained. Volatile locals
express observed storage rounding; they do not assert original source spelling.

The actual installed CutScene callback's six-word physical dispatch permits its
3767-byte inventory row with a blank historical signature. Its body remains
unimplemented. Seven independently reviewed model/animation identities add
2524 identified code bytes; only the 456-byte hierarchy evaluator enters source
in this release. The remaining identities retain their source/interface holds.
The blend wrapper's PC format guards and its external return-stub tail remain
separate from the Android child; no guard equivalence was assumed.

Retained authorities under ignored `build/analysis/identification/continuation/`
include the corrected `camera-viewport-scissor-source-next2` capsule, independent
source/data/emitted-code reviews, hierarchy evaluator reviews and the model
identity cohort. The failed matrix proposal remains preserved. Source combines
the Android animation-family reference with authoritative PC caller, operand,
layout and full-boundary evidence. Inventory counts remain separate from
compiled comparison and exact-byte reconstruction.

The final wave-47 source compiles and links as 127 objects with complete symbol
bindings and relocation resolution. Full checks pass 640 tests with three skips;
LLVM 18 formatting/static analysis passes after the final source corrections.
Independent emitted-code review confirms the viewport's signed clipping,
floating-point checkpoints, actual constant references and call order, and the
backend's receiver/COM transport, clamps, raw cache copies and result store.
The first emitted backend used x87 to copy depth fields; genuine `memcpy` now
emits the original integer-copy behavior. The failed first audit remains retained.
These checks qualify normal initialized engine objects and providers rather than
proving arbitrary external callbacks or API exception behavior. The source EXE
continues to have explicit unreconstructed camera and game actions.

Retail comparison adds 1596 original code bytes, reaching 377638 compared bytes.
Five additional functions are exact (327 bytes), including the repaired camera
setter and all three backend routines. All 815 earlier exact functions and 2257
earlier exact DATA rows are retained. Whole-code fuzzy coverage increases from
6.550184% to 6.583432%; exact code is 47161 of 3471246 bytes. The identification
union is 589065 bytes (16.969843%), measured separately from reconstruction.
The remaining differences include compiler register/frame choices and the
explicit unreconstructed portions of the larger camera builder.

The unchanged graph control reaches 1555 accepted seeds and 1313 inferred pairs.
Four prior hypotheses become seeds; four additional seeds enter the union,
with no unconverted losses or new inferred pairs. DATA candidates increase from
2457 to 2459, with 122 reviewed anchors and zero stale decisions. Seven rounds
reach a fixed point under the 1 GiB address-space cap; candidate counts remain
advisory. The added PC identity-copy provider has a distinct working name and
does not invent an Android pair. The final provider rescore preserves all pair
table hashes from the preceding run.

### Wave 48: animation children and model helpers

The connected animation subset adds `NuHGobjEvalAnim2Root_3` (0x5eb790,
230 bytes), `NuHGobjEvalAnimBlend2Root_3` (0x5ebb90, 388 bytes) and
`NuHGobjEvalAnimBlend2` (0x5f38f0, 90 bytes) in the existing hierarchy unit.
Each owns its complete PC span; the wrapper's external return leaf is separate.
Scratch sizing sign-extends the 16-bit capacity at globalbuffer pointee +4,
keeps 16-byte joint alignment, and releases the two allocations in LIFO order.
Time subtraction retains the PC float checkpoint. The two root translations
and blend output remain distinct; the blend output is not given an invented
initializer. The wrapper dereferences the first animation before its signed
ANI4/ANI5 guard and forwards nine arguments to the eleven-argument child.

The existing callback cell at 0x9738a4 keeps its owner, name and declaration.
An explicit local working view invokes it with four words only when the override
count and pointer are nonzero. This source requires a compatible installed
callback and the previously reviewed valid scratch/provider domain; it does not
establish all callback targets or full historical aggregate types.

Quaternion state preserves the original overflow behavior: a failed push at
depth four still permits the final pop to restore slot three. No balanced-push
repair is introduced. `AnimationDiagnosticNoOpPc` (0x5c12e0, one byte) represents
the complete original RET leaf as genuine empty C in the animation provider unit,
separate from its hierarchy callers. Its descriptive name is not historical.

`PCBuildModelLayerList` (0x62e190, 71 bytes) and
`PCStoreModelLocatorCoordinates` (0x62e1e0, 171 bytes) share the model animation
unit and existing qualified character/hierarchy fields. The selector retains
its nullable model, signed render-count limit, 32-bit mask and 16-bit outputs.
The locator helper uses the implemented POI matrix provider and preserves
missing outputs and locators. Its translation and matrix copies retain raw
bits, including all float payloads. Nonnull output arrays require sixteen
elements and valid input/provider storage. Alternate layer-callback targets
and the larger model animation/DWA family remain separate contract gates.

Ignored continuation retains the source and independent packets
`animation-children-source-independent-next1`,
`animation-children-data-independent-next1`,
`model-eval-connected-source-independent-next1` and
`animation-diagnostic-locator-copy-independent-next1`. Compiler and whole-extent
comparison gates remain required before claiming exactness.

Actual VC8 review passes for all six released bodies, preserving the qualified
stack/register/x87 interfaces and complete relocation targets. The wrapper
(90 bytes), diagnostic leaf (one byte) and locator helper (171 bytes) match
completely. The selector and two larger animation children remain nonexact;
matching extents alone are not byte equality. The animation children use the
verified double-one operand at 0x751698 and store/reload their float checkpoint.
The raw locator copies issue no floating-point operations. Frozen emitted-code
audits are `animation-children-compiled-independent-next1` and
`model-helpers-compiled-independent-next1`, with the animation literal authority
kept in its additive supplement.

### Wave 49: DWA curve and animation closure

Eight complete original functions now have genuine source: curve evaluation
0x5c9520/1373, time staging 0x5c9c40/303, node count 0x5ca450/30, DWA evaluation
0x5d3a20/345 and blend 0x5d3dc0/663, modern blend-root wrapper 0x5ebd20/96, and
the two deformer allocators 0x5fe5a0/63 and 0x5fe5e0/39. They share the existing
animation, hierarchy and renderer units and add 2912 compared original bytes.
Original curve code 1373, alignment 3 and switch tables 44 stay separate. The
compiled complete curve extent contains its own tail tables; comparison keeps
both whole extents without masking, truncation or enlarged original credit.

Independent arithmetic review rejected the first curve draft: format 1 scales
tangents by frame duration, and formats 5/6 round the reciprocal before
normalization. The corrected source preserves all format actions and the
coefficient/result checkpoints. Its shared polynomial block is real C within
the function, avoiding an invented standalone helper binding. Two and three
retain their original double operands at 0x7517a0/0x7519b8. The time endpoint
uses 0x7531a8, the exact promoted 0.01f value, rather than a double 0.01 literal.
Time staging preserves partial writes and the float offset before Floor/FTOL.

The allocators retain unusual original behavior: weight cursor 61184 subtracts
count*4+0x204 and selects a block at remaining-cursor*0x208; pointer cursor 2048
subtracts count but tests the weight cursor. Both negative-weight paths call
the existing RET-only hook at 0x5f7490 and return NULL. A root task description
incorrectly said execution continued after that hook; original review corrected
it before source acceptance. The DWA parents clear the tiny original byte
count ((count*4+15)>>4) before testing the returned pointer; no inferred factor 16
or safer call order replaces that behavior. Canonical CRT behavior is an
explicit inherited provider qualification, not a fresh CRT reconstruction.

The supported source domain requires valid storage and initialized fields for
each requested curve format. The modern ANI4/ANI5 time path writes only time;
encoded requests that consume its untouched fields remain held. The legacy
duration-one shortcut also leaves offset/chunk frame untouched. No stack
initialization or content invariant is invented to hide those paths. Numerical
review retains finite, valid conversion/provider qualifications; exceptional
NaN equivalence is not claimed. All parent curve calls enter with empty x87,
allowing the reviewed curve peak of eight.

The 96-byte NuHGobjEvalAnimBlend2Root wrapper independently forwards eleven
words after the signed ANI4/ANI5 guard, with a separate tail to the RET leaf.
Exact original ELF records identify its native peer at 0x2ce8e0. Earlier
propagation paired modern blend 0x5ebb90 with native 0x2c6560; original review
identifies that native entry as NuAnimBuffAccumulate_3, a different algorithm.
The independently reviewed modern seed at 0x2ce760 replaces that hypothesis.
Tentative scores never supplied acceptance for the old pair.

Frozen ignored packets include `dwa-curve-independent-next1`, its additive
`dwa-curve-single-function-source-next1` proposal,
`dwa-time-node-allocators-compiled-independent-next1`,
`dwa-parents-compiled-independent-next1` and its mandatory time-definedness
supplement, plus `animation-blend-root-compiled-independent-next1`. Historical
types, pool ownership, exceptional paths and remaining skeletal closure stay
separate from code comparison.

The actual compiled curve gate (`dwa-curve-compiled-independent-next1`) passes
all eight actions under the stated domain. Its 980 executable bytes and 44 table
bytes form a complete 1024-byte COFF region; extraction uses the sole function
through section end, not an auxiliary size. All four returns balance the stack
and preserve nonvolatiles/ST0 ownership. Required float checkpoints remain;
polynomial intermediates stay in x87 without extra double spills. The inline
float-to-int path restores its control word under the canonical valid conversion
qualification. Whole retail comparison still reports this curve nonexact, with
zero advisory similarity; that score does not reverse its semantic gate or
supply byte equality.

The batch adds 228 exact bytes while preserving all 823 previous exact function
rows and 2257 exact data rows. The wrapper 96, node helper 30 and allocator 63/39
match completely. Time/curve/DWA parents remain nonexact. Whole-code fuzzy
coverage reaches 6.637517%, exact code 47651 / 3471246 bytes, and compared original
bytes 381501. Inventory/identified union 591659 (17.044571% of text) is separate
from reconstruction and includes reviewed bodies awaiting source closure.

### Wave 50: connected skeletal and model evaluation

Four complete PC bodies receive genuine source: the neutral eleven-word
PCBuildLegacySkeletalJoint at 0x5ceee0/1400, NuHGobjEvalAnim2Root at
0x5eb880/777, its six-word wrapper at 0x5f38c0/46, and EvalModelAnim at
0x62e290/852. The batch covers 3075 original bytes. Compiler and complete
comparison gates remain required before crediting exact results.

The skeletal header has a separate 24-byte working prefix, preserving DWA's
20-byte prefix. Joint pivot and flag accesses use asserted offsets 0x40/0x51;
overrides use scale at 0x18, signed rotation maxima at 0x24 and minima at 0x2a,
with index/flags at 0x30/0x31. Existing object strides remain unchanged.
Backend source retains all 25 original actions, raw parent-scale copies, the
override-only zero scale, any-zero reciprocal branch, root translation before
overrides, pivot locator handling and six matrix sign changes. Nine curve
operations and three clamps are expanded in place, avoiding invented helper
bindings. Finite arithmetic, normal providers and initialized payloads remain
explicit qualifications; exceptional locator payload equivalence stays held.

Independent review rejected the first parent draft's identity owner and loop
entry. The corrected source uses NuMtxSetIdentityTemplate at 0x806680 and the
original signed override entry guard. Missing nodes retain undefined scale
locals; later scale readers require a valid initialized prior parent or the
initialized parent-255 sentinel. Legacy duration-one encoded requests retain
the earlier time-offset qualification. No safe initialization replaces
original behavior, and callback effects remain conditional on the reviewed
normal engine contract.

The model caller preserves the independent DWA and skeletal fallback chains:
valid blend, target, source, current animation, then NULL DWA/static skeleton.
Both blend quotients materialize separately as floats before child calls.
Model and animation-packet fields retain asserted offsets and existing strides.
The layer-list dispatch stays indirect through 0x80fb94. Its original four-byte
initializer points to PCBuildModelLayerList at 0x62e190; arbitrary installed
callback behavior and writer ownership are separate holds.

Ignored continuation keeps the original rejected parent gate, its corrected
source review, the two backend source gates, shared-view review and the model
caller review. These qualify behavior and working interfaces; historical
types, full original allocations and nonfinite behavior remain separate.

Actual compiler gates pass for the complete backend (1367 rebuilt bytes),
parent (717), wrapper (46) and model caller (794). The wrapper matches all
46 original bytes. The parent compiler merges two original backend sites into
one call with equivalent root-only optional outputs. The child consumes only
the low byte of its flag word, so unspecified upper bits in the caller do not
change behavior. All nine backend curve calls enter with empty x87 and consume
the float result; the combined curve peak remains eight. Both model blend
quotients keep their separate float stores, and the slot emits a genuine DIR32
to the existing selector. Larger functions remain nonexact.

This batch adds 3075 compared bytes and 46 exact bytes, retaining all 827
previous exact functions and 2257 exact data rows. Compared original code
reaches 384576 bytes; exact code is 47697 / 3471246 bytes and whole-code fuzzy
coverage reaches 6.704052%. Reviewed/inventory identification is separate,
with union 593836 bytes (17.107287% of text).

Paired original review confirms the backend and root evaluator identities.
Android's nonroot nine-word wrapper at 0x2c81e0 forwards to 0x2c7990 with two
NULL outputs, while PC calls 0x5ceee0 directly for both roles. Raw callsite
context therefore differs; both parents already have the direct backend
anchor, so this observation does not create a second independent root or
prove additional automatic mapping. The wrapper gets no invented PC binding.

## Cutscene matrix and character animation family (wave 52)

Seven complete PC functions are reconstructed together in `cutscene_hooks.c`:
NuGCutLocatorCalcMtx at 0x500e50 (474 bytes), NuGCutRigidCalcMtx at 0x502390
(644), NuGCutCharAnimProcess at 0x503050 (1263), and their selected ANI3
providers at 0x500d50 (253), 0x502260 (296), 0x502e10 (564) and 0x502dc0 (67).
Independent paired original-body reviews establish all seven identities and
complete extents. The providers use the neutral PC names PCCutLocatorAni3,
PCCutRigidAni3, PCCutCharacterAni3 and PCCutAni3CurveInteger; retained native
names identify the corresponding `_3` routines and GetIntCurveVal. Android
source remains a reference, with the PC bodies authoritative.

The working interfaces have four, three and nine physical argument words for
the generic parents, and three, three, nine and three for the providers.
Asserted field cohorts distinguish legacy skeletal animation from ANI3;
they establish accessed offsets rather than historical complete types.
Source preserves signed format guards, lazy visibility/output writes,
translation and handedness changes, raw scale copies, and the different
generic and provider index conversions. Curve type 10 returns the raw integer
bits. Character paths that leave start/layer outputs untouched still do so.
Angular conversion reuses the verified existing double at 0x755d18.

Independent compiler reviews cover every normal return, physical call
arguments, nonvolatile registers, stack and x87 balance under the documented
valid pointer, curve, finite conversion and engine-provider domains. The
final translation unit emits the same complete bytes and relocations as the
reviewed pilot for all seven functions. Complete retail comparisons retain
their original extents; all seven remain nonexact. The new functions introduce
one compiler copy of the existing zero literal at 0x751658, with no new original
data range. The journal accepts identities/extents and explicitly leaves
historical ABI/type promotion false. Compared original code reaches 388137
bytes, with 47697 exact bytes and 6.787306% whole-code fuzzy coverage. This
family adds 3561 compared bytes and no new exact bytes.

Next, review NuGCutLookAtLocatorUpdate at PC 0x506e40 against native 0x438d90
(1266 PC bytes; the retained native span is 1546 bytes with 1519 reachable).
Independent compound identity and PC extent review passes, but private
incoming ECX, providers, field contracts and the exact original native name
remain held. It receives no source coverage or CSV binding yet.
The first camera pairing was crossed: PC 0x506880 is a camera interpolation
body, whereas native 0x4393a0 is a locator/debris update. Native 0x4361a0 is a
camera candidate requiring its own complete original-body review.


Lifecycle transfer evidence (2026-10-03): complete PC extents and actual VC8
comparisons are retained in transfer-execution-20261003/{debris/next,debris/engine,
apiobject,mapping/scene,parser/resource-cleanup,world} analysis packets. Producer
proof corrects chunk/control strides to424/2c, display registry9739dc to an inline
100-pointer bounded view, and PART emitter base291f7c8 (effect_id+c). Unknown
allocation extents stay blank. Scene removal/restoration bindings are identified
but remain unimplemented because their embedded fallback image initializer is
held; they receive no reconstructed-byte credit. All accepted bodies retain full
returns, ABI and branches; no previous exact code match is lost.


Runtime transfer corrections (2026-10-03): original5ddd10 is the complete238-byte
private display reset, not a60-byte two-dimensional renderer. Its genuine frame
caller5dfab0 includes the finalRET and spans188 bytes. Complete disassembly,
logical signatures and private ESI/EBX differences are documented in scene-next.
Cutscene private EAX/EDI helper interfaces reproduce actual PC registers and retain
their original caller group. InitParts64a4b0 independently proves the corrected
PART emitter extent. Display/terrain global ownership is canonical in rebuilt
source; aliases retain verified relocation base/addends. Retained full comparisons
and real-provider runtime fixtures support the execution checkpoint.


### Reviewed transfer corrections, 3 October 2026

The next source checkpoint corrects three rendering inventory ranges.
NuDisplaySceneClone at 0x005f0470 has the complete 1,612-byte body through its
final return; the previous process-sort candidate name was wrong.
DisplayClipMaterialsPC at 0x005f0ac0 is a 688-byte private ESI-scene routine
including its out-of-line return path. NuDisplayListSwapBuffersEndFrame starts
a separate 392-byte body at 0x005f0d70; the former 1,079-byte sort candidate
incorrectly crossed that boundary. Logical C signatures remain in the map;
private physical register contracts stay in retained evidence.

Fifteen immediate movement callees totaling 5,025 original bytes have independently
reviewed identities, ABI and full PC hashes. Their new inventory rows do not count
as transferred source. Broad level-registration crosswalk hypotheses likewise
remain unpromoted until individually reviewed. The neutral terrain diagnostic at
0x0064fdb0 is a genuine one-byte return; importing Android's active NaN-check
behavior there would be incorrect.


### Publication/scanner contracts (2026-10-03)

Complete reviewed PC spans and calling conventions underpin the movement,
display, scanner, debris and gameplay source families. Compiler literal aliases
at the same VA retain existing bindings; overlapping bytes count only once.
Display front/safe registries expose 24 pointers; their historical allocation
extents remain unknown. Native frustum accessed groups assert stride 0x380,
planes at 0x1a0 and count at 0x260, without claiming backing capacity. Private
display ABI differences are documented in transfer-execution.md.


### Directional/native/geometry release contracts (2026-10-03)

The release adds actual source for 59 complete PC spans, 30,650 unique original
bytes, independently reviewed for identity, caller ABI, fields and out-of-line exits.
Terrain geometry includes Derotate 171, Rotate 230, DeRotateTerrain 554,
DeRotatePoint 254, InsideLineF 68, InsidePolLines 1,320, CheckCylinder 2,145,
CheckSphere 558, CheckSphereTer 408, HitPoly 1,689 and HitWallSpline 1,803.
Their 948-byte query/100-byte polygon field cohorts are asserted. Zero advisory
scores on two complete bodies remain visible rather than being masked or shortened.

Shader CRC hash at 0x00746360 spans 61 original bytes: its following two CC bytes
are alignment, not part of the body. The constructor spans 255. The genuine
compiler startup spans 10 and its initializer pointer four. Native lock/unlock
wrappers span 38/13; the diagnostic return is an actual one-byte C3 routine.
Private Walking remains ECX object, stack float, ST0 result/plain RET within its
original movement TU. DisplayClipMaterialsPC is 688 bytes with retail ESI scene;
its rebuilt logical stack signature is explicitly nonexact in the coherent C++ TU.

GHG/model identities retain only inventory credit while their 3,712-byte source
closure is held. SceneFixupTIDs 397/FixupTID 186 depend on the genuine 81-byte DDS
fallback and its 8,320-byte embedded original image; no fake provider or blank asset
is substituted. Accepted identities alone do not imply newly transferred source.
Finite next-wave solver maps likewise require actual integration/compilation before
source credit. Existing immutable aliases retain original names and addresses;
new mutable storage reuses canonical owners where they already exist.


### Solver, clone, Draw and carry contracts (2026-10-03)

All 34 released spans (19,718 unique bytes) have accepted PC identity, ABI and
complete extents, including terrain compiler switch tails. Native ClonePS at
0x006039d0 spans 58 bytes through RET; its six trailing CC alignment bytes are
excluded. The parent clone's 1,612-byte span remains accepted. Draw at 0x00685770
ends 0x00685c40 after RET at 0x00685c3f: 1,232 bytes, correcting the truncated
1,228-byte candidate. Portal clipping 0x005da3b0/150 and sphere helper
0x005d94a0/212 use ordinary caller-clean stack words, not invented private GPR ABI.
Their portal plane pointer/count/room fields at 0x80/0x9c/0x9e belong to a distinct
minimal 0xa0 view, not the separate native 0x380 frustum.

The seven carry-provider spans total 1,132 bytes. Private restore at
0x00566ba0/119 takes its object in ESI, no stack arguments; the real respawn
caller preserves that register contract in their original C group. Its compiled
full body is exact. Shared clear-context callback takes (GAMEOBJECT *, i32),
as proven by the actual PC caller's two stack words; its larger source cycle
remains held. Camera/death audio preserve PC mode selection and the actual
two-argument PlaySfxById call, rather than Android's different audio service.

Three erroneous scratch Mask literal bindings were removed before production:
the native qwords represent promoted-f32 .95/.003/.001, not true f64 literals.
The corrected compiled literals match actual PC bytes; no relocation masking
or lookup rewrite conceals a source constant mismatch. The terrain matrix
getter's second global read is verified by actual alias behavior and full byte
equality. Refer to transfer-execution.md for native evidence and remaining gaps.


### Query and occlusion release contracts (2026-10-03)

Eighteen actual source spans total 15,186 original bytes. NewScan 0x00670640/4,361
ends after the second RET at 0x00671748; its internal return is not the boundary.
ScanTerrainHandel 0x0067eac0/3,427, ScanTerrainPlatform 0x0067fd40/3,582 and
HitTerrPoly 0x00675610/553 retain their complete accepted tails. The platform
kernel's zero advisory score remains visible alongside its selected native cases.

Six occlusion backend bodies total 2,225 bytes. PrepareForQueries
0x005e8190/668 includes the x87 cleanup tails after its early RET 8.
The 95-byte private copy takes source in EAX and destination in ECX, emitted by
ordinary VC8 within the original helper/caller group. IsOccludedOBB uses
thiscall/RET 12 and an AL boolean. Sort uses the actual pinned CRT qsort body
0x006e96b0/679; that SDK provider receives inventory credit only, no reconstructed
source credit. Fade/private clip remain scratch without a production caller.

Water contact/intersection spans are 48/69, deflection 135, character platform
lookup 72, terrain setters 35/11, ReleaseHearts 288 and MakePartVector 380.
The deflection argument order is normal, movement, output, mode; its .015/.006
thresholds and terrain 1.02 margin are promoted f32 values in f64 compiler literals.
Full initializer equality verifies these bindings. No comparison extent is
masked or shortened. See transfer-execution.md for actual-object fixtures and
the remaining scene/pickup/gameplay providers.


### Message and legacy literal correction contracts (2026-10-03)

AddGameMsg 0x00560b60/1,078 ends after JMP 0x00560f91 to the common return;
RET 0x00560d15 is not its boundary. ResetGameMessages 0x00560b40/32 is exact.
Input/runtime named views are 0x50/0x114, with the observed pool loop bounded
to 128 records. The end callback takes one message pointer; copied delay/tick/
update pointer types retain source hints without claiming additional caller proof.

Actual spline/gamepad qword operands bind true 1/65536 at 0x00761610.
Grass operands prove true 1.25 at 0x00751720, promoted 1.2f at 0x00758be0
and promoted 1.7625f at 0x0076d7b8. Wrong .1/true-1.7625 literal aliases are
revoked. These correct existing source/bindings; no additional function extents
or source bytes are credited for the three old affected functions.


### UI and gameplay parent release contracts (2026-10-03)

Twenty-seven compiled/compared bodies add 6,879 original PC bytes. NewMenu at
0x00509830/629 returns MENU*, correcting the old MENUSCREEN inventory signature.
Arcade_AwardPoint 0x00475c60/833 ends after RET at 0x00475fa0; the earlier 841-byte
scratch estimate is rejected. NuSPrintf 0x004f2740/43 is cdecl variadic and uses
the genuine CRT vsprintf/strlen; its reconstruction name does not establish an
Android identity. Arcade_BothPlayersActive 0x0055f1a0/45 preserves null tests and
flags_1fc bit 0x80, rather than Android's unconditional activity.

PreResetCode 0x00498a40/753 keeps PreResetCanAccelerateFallPc 0x00498120/388
with its genuine ESI private ABI. BigJumpCode 0x00563ec0/1068 keeps the default
landing callback 0x00561dd0/66 in jumping.c and its actual initialized pointer.
The callback selects model_data_b preferred/secondary/fallback IDs and is exact.
Full byte equality also confirms AddWaterSplash122, NuLstGetByIdx31,
SetBonusWinner55, both-player activity45 and coin-position callback25.
AdaptiveDifficultyAdjustPc 0x004289a0/156 is a neutral, fully exact PC provider
which retains float spills, cheat/bonus/demo gates, capped level changes and
unordered accumulator behavior; it has no asserted Android source identity.

EndScoreMessage679, CoinsGoToMainTotal78 and LoseCoins225 retain full extents and
real production dependencies. AddCoinsToPanel854 remains held on the complete
pickup-type table's actual nonzero callbacks. Compiler literal aliases are
checked against full raw initializer witnesses. Unknown runtime allocation
extents remain blank; source initializer proofs and map inventory are distinct
from transferred code. All earlier exact code/data comparisons remain exact.


### Pickup, vehicle breakup and service contracts (2026-10-03)

Seventy-two newly compiled/compared functions contribute 18,176 original PC bytes.
The 19-function breakup unit preserves its genuine private register groups;
AddVariableShotDebrisEffectTimed1/3 retain original private matrix owners in parts.c.
CharScene_FindHSpecial has complete extent 80 bytes. FindNearestGameObject has the
verified eleven-argument ABI; GizmoPickup_CollectCoin has six arguments and a full
994-byte span. The short blue/purple extra-coin entries preserve their real tail
jumps to the 136-byte private provider rather than shortening either comparison.

Pickup records are 0x2c; pickup-type records are 0x38, with callback fields at
0x24/0x28. The true ten-record table's twelve nonzero slots target nine real bodies.
The public camera source prefix covers all observed accesses through 0x22e, with
named asserted fields; this does not establish the original allocation size.
World pickup/torpedo state, save-area records, mini-piece and hint views retain
asserted offsets without guessing Android layouts. Unknown data extents stay blank.

Accepted inventory includes held constructor and parent identities, while source
credit comes only from actual compiled object comparisons. Six renderer-dependent
pickup bodies plus AddPickups/AddEffects remain scratch, along with their three
naturally elided private helpers. No unimplemented row counts as transferred code.
All prior exact code/data remains exact in the final aggregate report.


### Genuine renderer and restored pickup contracts (2026-10-03)

The compiled checkpoint adds 52 complete functions/12,631 PC bytes. Actual
NuSpecialDrawAt43 calls NuDisplayListRndrSpecial1491; the coherent private item
dispatcher/helpers, material variants, shadow methods and native transforms are
real source. Private material ABI differences stay within the original caller
group and remain nonexact. NuLstAlloc5 is the real tail wrapper to the newly
recovered head allocator118, rather than an alias to the tail allocator.

PCFrustumClipSet::TestBoxes69 retains its ECX receiver/RET8, count at0x710 and
actual TestBox642 provider. Shadow wrapper293 preserves signed-positive cached
mask behavior and forced invalidation; the Android no-op is rejected. The producer
caps four lights. Native reciprocal at0x0075be20 is the promoted float-third value,
not Android's double division3. Rotation278 reads mutable sine/cosine table values
and preserves the qword product spill. Arcade_Kill67 includes its late tail jump
after the early RET; AIKilled258 uses unsigned counter/target comparison and signed
mode indexing. The full16-byte sound string includes its NUL terminator.

Previously held constructors/private helpers1,991 and pickup/effect parents2,606
now have actual linked providers and receive source credit. All prior exact code
and data remains exact; unimplemented combat/terrain candidates remain scratch.


### Complete combat and private collision contracts (2026-10-03)

The source checkpoint adds24 functions/13,774 original PC bytes, plus replaces
the existing17-byte PartCollide_3D placeholder with its real dispatch. ObjHitObj
438230 spans5,764 bytes through its cold finalRET4398b3; all late paths remain
in the comparison. Shield433be0/520 and reaction432330/739 retain actual PC
fields and flags. The private SpecialMoveApplyDamagePc58b080/108 receives ESI
inside the genuine ReleaseVictim58b0f0/133 caller TU. No public ABI adapter
substitutes for the private call. Clear58 remains exact; new player callers
naturally inline it within their original unit and retain full nonexact extents.

PartCollidePC42c740/1973 receives PART in EDI and one caller-clean stack mode;
the real2D42cf00/17 and3D42cf20/17 wrappers are both exact. Its complete
nonexact comparison is preserved. DeflectPart42c500/569 uses the actual deflect
callback field; getMaxTorpedos4d23e0/72 reads lowWORD IDs and establishes3/5
limits. DropTorpedoPickups4d2350/133 calls plural AddPickups46d270 with thirteen
stack arguments, not a singular allocator. LightSabreDebris49f850/512 ends
RET49fa4f immediately before49fa50, preserving lowWORD character ID comparisons.
PC normal-pickup delay uses promoted.05f times slot modulo11 plus.75; owner age
and pickup age retain distinct unordered behavior. Selected real-object runtime
checks pass without replacement providers; successful damage remains unverified.


### Real vehicle leaves and grapple contracts (2026-10-03)

The source checkpoint adds 12 complete functions/2,108 PC bytes. EngineNoise
547dc0/354 preserves PC float spills before hostility jitter. DropInOut427b40/196
retains unordered hold; CatchUp4ab1f0/350 preserves signed socket index, loop wrap
and clamp. Its equivalent ordered negative-distance comparison reuses a genuine
existing positive-half constant; no unrelated negative-half qword binding is added.
FindGunshipHoverHeight4aa5a0/143 reads mutable controls and signed BYTE280.
ActivatePlayer4327a0 ends4327fe (94 bytes), before a distinct function432800;
GameCam_HitJudder549f10 spans186 bytes and preserves actual sign/axis behavior.

Grapple_AddDynamic54e750 spans121 bytes through RET54e7c8, rather than advisory
118; Grapple_RemoveDynamic54e7d0 spans70 through RET54e815. Both return the
actual record pointer in EAX with ordinary cdecl2/1 stack arguments. Their table
view starts926c78, with f8 stride and attached pointer54/retention1d/flags1e.
SetWeaponIn58d8e0/66 is complete byte-exact and reads signed object context
against actual mutable DWORD selectors802864/802868. All prior exact matches
remain exact; pending vehicle/terrain contracts receive no compiled source credit.


### Vehicle public provider transfer (2026-10-03)

The accepted28-function9085-PC-byte cohort implements real float/tractor,
loop/hint, force-query, targeting and item/weapon services. Full native extents
include FloatAdvance53 in its original caller TU, all seven TakeHit returns
and GizmoBest's630-byte tail omitted by the older624 advisory boundary.
Gizmo callback70 has12 cdecl stack arguments; its registry count is reloaded
after external flag/callback actions. WORLD count50cc/array50d0 preserve the
PC order and existing51ac stride. Weapon callbacks9555ac/9555b0 and target
filter950be8 use their actual NULL initializers, blank storage extents and real
callback contracts. ResetForceGlow remains a standalone void packet service;
its original TU is unknown and it is not forced into a private move group.

Current-source retail results and prior-copy reviews are retained under
continuation-vehicle-public/, mapping/{targets2575,takehit866}-accepted-packet.json
and apiobject/. Ordinary nonexact source is retained, including TakeHit's
15.475837 score; no extent masks or copied native bytes improve comparisons.


### Terrain, force, world configuration and PC music transfer (2026-10-03)

Twenty-three genuine production functions add 9,135 full native bytes. Verified
PC contracts include the second SuperCarry output in AwkwardShapeCode818,
six-argument NewRayCastScaleYMask473, complete RayImpact180 tables, real
TerrainPlatId6, SuperWeirdo60, PlayGruntSfx98 and SetHeadTarget194. Traffic/Faders
retain original private callback groups, signed limits64/16 and world stride51ac.
Music1432 includes actual alignment and eight switch-table DWORDs omitted by the
old1398 inventory; Voice::Unload209 is thiscall, StereoV396 is variadic cdecl
and the cross-unit181-byte backend calls genuine existing queue providers.

GizForce_FindBestForceTarget1988 retains native cached-u32 conversion, raw timer
bits, facing/tie/LOS rules and unguarded candidate writes. Storage capacity is
unproved: <=384 cumulative eligible writes qualifies the source working view;
no new native guard or original allocation bound is inferred. All2484 earlier
function copies remain identical. Complete retail comparisons and independent
ABI/field/initializer evidence are retained in continuation-vehicle-parents/,
platform/force-best-review/, mapping/music-next/ and apiobject/parser-config-next/.


### Genuine Cannon and weapon caller group (2026-10-03)

Ten complete functions9,130 PC bytes compile with actual original caller groups.
FindID1,317 has ordinary cdecl2 ABI in bolts.c. The nine move functions retain
private Fire EAX+four stack arguments, ShootThisFrame ESI+two stack arguments,
Deactivated EAX, ZapFilter EDI/ESI and coherent MindTrick register-pair codegen.
No public bridge changes those native interfaces. CHARACTERDATA.move_fn18 is
proven by actual TerrainPlayer cdecl invocation as well as genuine Cannon
identity; allocation stride4c and the neighboring game-character field24 stay
unchanged. Inline weapon/ammo7f4/7f8 and phase/latch bytes remain asserted local
consumer records. Shield/SmartBomb/Dodgem lack a genuine released caller and are
excluded, reducing the candidate ceiling10,361 to emitted9,130.

Full source/ABI/extent/caller/literal proofs are retained in
coherent10361-next/, findid-next/production/ and platform/cannon-weapon-review/.
Actual retail comparisons remain nonexact except Cannon350/ZapFilter109;
neither private ABI differences nor field proofs substitute for byte equality.


### Renderer, movement, audio and search source families (2026-10-03)

Thirty-three complete PC functions10,384B are released with real source
dependencies and original owner context. Rendering/root motion19/7663 uses
existing apiobject.c, nuhgobj.c and nurndr.c rather than parallel C++ wrappers;
all58 previous owner bodies and symbolic relocations survive consolidation.
Root callback signatures carry six physical words (seven for RootFnEx), and
NuVecLerp's actual reversed operand convention is preserved. Evaluator joint
restriction saves a BYTE, writes DWORD1 and restores the zero-extended BYTE.
Skin working buffers retain the verified16/128 alignment.

Public movement4/1100 includes the verified early-return ordering repair;
private ForcePushed712 and Beast helpers are not credited. Search3/918 retains
PC's1,000,000-float nearest-bolt threshold, full target masks and real canonical
BOUNTYHUNTERPURSUIT_ADATA. Audio7/703 retains pulse WORD fields6/8/a/c/e,
ten-byte event stride and five-argument gizmo callbacks with count reload.
Full extents and actual current-source comparisons remain authoritative,
including nonexact bodies. Three compiler literal bindings add26 bytes.


### Cutscene callbacks and corrected text/input transfer (2026-10-03)

Fifty-three functions10,724B compile with full PC extent/ABI evidence. The
cutscene owner retains37 static cdecl callbacks in its genuine43-slot table,
including six alias slots and the terminator. Renaming the existing C owner to
C++ permits the genuine NuMusic thiscall without a shim; all three old bodies
and source prefix remain unchanged. Private held loader helpers receive no
source credit. InStory49 is exact and calls real Mission_Active33.

Text3DEx uses native12-argument cdecl ABI and inclusive/unordered screen guards.
The input member's full1328-byte span includes its thirteen-entry switch table,
with ECX receiver, eight stack arguments and RET32. Types5/6 directional choices,
signed label indexes,34-slot overlapping label view and custom4x32 rows are
verified. Device links are three12-byte records based at297e060. The input
wrapper uses canonical nu_input_state rather than a duplicate manager owner.
MiniAnim43 is exact, preserves WORD truncation and initializes verified fields;
FindPart182 retains real typed Part/MAXPARTS and strict nearest selection.


### Connected Beast/Punch, audio and torpedo transfer (2026-10-04)

Twenty-eight complete PC functions23892B are emitted in original subsystem
owners, with private movement helpers retained by real callers. Move_BEAST224,
Beast fall149, opponent chooser189, matrix rotation160 and barrier48 are fully
exact. Public function extents include GizmoBlowUpOpponent724's cold tail;
the former718 inventory length was incomplete. Native Punch3000 includes its
seven-entry dispatch table. Private register ABI qualifications remain in the
retained evidence and blank ctype rows; no public stack ABI is invented.

Opponent1179 plus StillThere229 use canonical bounds_radius1dc, physical
headingWORD27a and a rounded f32 squared radius. Final native cases distinguish
27a/27c and a real rounding boundary. Complete torpedo spans1820/1274 retain
mandatory WORLD reads, fill-before-cheat and unordered timer behavior. Canonical
text marker storage is reused. Level sound8845 uses the genuine exact Beast
pointer and signed WORD EWOK/WICKET IDs; the pure C partition preserves actual
public cross-unit calls without claiming historical TU identity.


### Vehicle/terrain and cutscene loader source release (2026-10-04)

Fifty-one complete source functions 24,861B are compared in actual subsystem
owners. ForceGlow 1,204 includes its four-entry switch table; the former rough
1,460-byte candidate was not its full PC identity. Genuine CreateCable 147 and
CableTarget 517 close byte-exact Cable 362. The public Create interface returns
an opaque CABLE_RECORD_PC pointer; physical flags WORD 1ea/stride 1ec are locally
asserted. Private helper ctypes stay blank and actual register witnesses remain
in retained evidence. No artificial retaining calls or compiler controls are used.

Terrain's genuine C89 caller/helper group contains91/202/7,994-byte PC spans.
Removing reconstructed noinline annotations preserves both helper bodies;
mechanical C conversion preserves initializer execution order. The main remains
nonexact. Native417b96/417b9c/417b9e computes and compares an extended threshold
without a DWORD spill; final C follows that precision, unlike the prior CPP build.

Cutscene kernel25/4,478 plus natural loader4/2,564 uses complete entries and
actual same-TU private context, including Configure 510, Listed 720 and Load 1,333.
Original inherited camera/locator registers remain documented separately from
candidate compiler allocation. Both one-byte kernel diagnostics and the separate
one-byte config diagnostic are genuine retail RET functions, not fake providers.


### Renderer, spline, font and input release (2026-10-04)

Actual production owners compare26 complete functions/6,715B. CalcSplinePoint
1,119 and CalcSplinePointFromDist190 share the existing spline owner; asserted
530-byte FLIGHT_SPLINE_RESOURCE_PC has point_count400, length410,
cumulative414 and id524. Finite reader domains do not prove original allocation
capacity. DrawStatusText583, DrawPanel3DObject255, DrawCharIcon641 and byte-exact
BackDrop_Draw285 use genuine shared camera/font/special providers. Camera55,
Shadow_SetMode60, NoRender159, Particles_Stop113, DebFreeAllCreatedEffects84 and
the font matrix39 are fully exact. Private storage/literals stay in original
subsystem owners; DrawStatusText's SDK macro collision is explicitly undefined.

NuQFntPrintJustifiedRSW1,345 includes the complete native body and real diagnostic
call; its110-byte wrapper is exact. The word-width/scale computation preserves
the PC's f32 rounding, language0 FFFE hyphen and unordered predicates. Customiser
IDs are signed WORDs at6c; names use actual128-byte rows. Input3/67 preserves
signed BYTE state/truncation and physical caller contracts. All prior bodies and
relocations are identical after aggregate reconstruction. No bounds or bindings
were changed to hide comparison differences.


### Draw, RTL, UI and locator source release (2026-10-04)

Actual production owners compare90 complete functions/27,288B. All entry, ABI,
access-width and full-extent gates use retail PC evidence. RTLCalcLights744 and
ApplyLoop2,068 retain switch tails; SmartTextEx2,661 omits Android-only message
box drawing according to the PC body. DrawMenu receives paused state, obtains
its real menu from GameMenu[GameMenuLevel] and retains the native NULL callback
guard. CutScenes_InitSystem85 naturally registers the real3,767-byte draw body
and its original40B private type table. Private LookAt and debris/VFX helper ABIs
remain naturally optimized in the coherent original caller/helper contexts.

Locator effects use the verified428-byte DEBRIS_INFO_PC stride and12-byte cache
records with actual source providers. PC generator interfaces retain native
argument widths, the999 cap, unordered thinning behavior and real clock/state
updates. Private colour setup remains naturally retained by real callers; no
noinline attributes or artificial wrappers are introduced. A prior nonexact
DebReAlloc2 integer-load reorder is independently reviewed and explicitly counted
as one changed prior copy. Every earlier exact code/data record remains exact.
