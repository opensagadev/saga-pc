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
