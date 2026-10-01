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
