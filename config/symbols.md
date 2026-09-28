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
| `size` | Size in bytes, normally decimal (`0x` hexadecimal is also accepted). Leave blank when the extent is unknown. Functions end at their final instruction; following alignment padding is excluded. |
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

C and C++ linkage are supported: `_NuRand` and `?qrand@@YAHXZ` are different
MSVC decoration styles. `make -j"$(nproc)" diff FUNC=NuRand` accepts a simple undecorated C
name. For C++, use the exact symbol (`make -j"$(nproc)" diff 'FUNC=?qrand@@YAHXZ'`) or its
original address (`make -j"$(nproc)" diff FUNC=0x0054fa60`).

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
  reconstruction emits `?qrand@@YAHXZ`. Its multiplier `0x24cd` alone is ambiguous;
  `qseed` at `0x0080253c` starts at 12345, and the neighboring seed getter and reset
  routines corroborate the identity. The Android variable is named `qseed`; MSVC
  emits `?qseed@@3HA`. The similar routine at `0x005b9e50` instead starts with
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
private ABI, which ordinary VC8 C does not reproduce; our callable source
wrapper is intentionally nonmatching. Its three-byte zero-return callee at
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
