# Mapping status: what the map covers and what is left

The README's three mapping badges count bytes of the original sections that a
sized row of [`config/symbols.csv`](../config/symbols.csv) (or a typed data
member in `matching.json`) covers. This page says what the numbers measure,
classifies every byte they do not cover, and lists the work that remains.
The classes come from `tools/unmapped.py` (stdlib only, test in
`tools/test_unmapped.py`), which reads only the retail executable, the map and
the pinned runtime/SDK archives:

```sh
python3 tools/unmapped.py                 # bytes per class and section
python3 tools/unmapped.py --markdown      # the table below
python3 tools/unmapped.py --list CLASS    # the ranges of one class
python3 tools/unmapped.py --worklist      # unmapped code/data bytes per original compiland
python3 tools/unmapped.py --check         # re-derive every derivable map row from retail
python3 tools/unmapped.py --write KIND    # add the provable rows of one kind (see --help)
```

## What the README numbers measure

| Badge | Numerator | Denominator |
| --- | --- | --- |
| fuzzy code match | objdiff similarity of compiled functions, weighted by original size | original `.text` |
| exact code match | byte-identical functions after relocation, plus the alignment padding that follows an exact function | original `.text` |
| mapped code | union of sized map rows in `.text`, plus proven alignment padding between known extents | original `.text` (VirtualSize) |
| mapped file-backed data | union of sized map rows and typed data members in the raw-data part of `.rdata` and `.data`, plus proven zero padding between known extents | file-backed `.rdata` + `.data` |
| mapped zero-fill/BSS | the same union in the zero-filled tail of `.data` | VirtualSize minus raw size of `.data` |

"Mapped" means identified: an address, a linker name and a complete extent.
It does not mean compiled or matched; the exact badge measures that. Until
this change, two things made the numbers look odd:

- **String literal rows had no size.** 8,764 `??_C@` rows (388,454 bytes,
  mostly the DirectX error tables of `dxerr9.obj`) were inventoried without an
  extent, so they counted nothing. That alone was most of the 50.36 % on
  file-backed data. The MSVC literal name encodes the length and a CRC of the
  bytes, so the size follows from the name and is checked against retail.
- **Padding could never be mapped.** Alignment fill is not a symbol, so code
  coverage topped out at 97.6 % (83,191 bytes of int3/nop fill) and data below
  100 % (about 24,000 zero bytes after odd-sized items). The mapped badges now
  count fill that a rule proves, the way the exact badge already does: in
  `.text`, int3/nop filler between two known extents that is shorter than the
  alignment of the start it ends on, at most 16 (`match.pads_to`: functions
  start on 16-byte boundaries, runtime assembler and import thunks on smaller
  ones; `match.padding_runs`); in file-backed data, fewer than 4
  zero bytes between two known extents ending where the next one starts on a
  4-byte boundary (`match.data_padding_runs`). The symbol-only numbers stay
  in the report as `mapping_<kind>_percent`; the badges use
  `mapping_<kind>_with_padding_percent`.

**What verifies a mapped byte.** `check.py` only checks that a row lies in the
image and does not cross `.text`; a sized row counts as mapped whether or not
anything proves its extent. That is the map's convention (identification
before source), and most rows come from reference symbol data with no
mechanical proof. Two checks now cover the rows that can be proven:

- `check.py` (original-free) rejects a `??_C@`, `__real@` or `__xmm@` row whose
  size contradicts the size its name encodes.
- `unmapped.py --check` (needs retail) re-derives every `??_C@` row (length,
  CRC, terminator), every `__real@` row (value; a wrong name may only alias the
  correct one, as `__real@3fb999999999999a` does for a `0.1f` widened to
  double), the PE-structure rows, the archive-member data layout and the C++
  EH table sizes. It fails on any contradiction.

Proposal: rename the badges to **known code (incl. padding)**, **known
file-backed data (incl. padding)** and **known zero-fill**, and run
`unmapped.py --check` next to `match.py` locally. The data badges are not a
statement about compiled data: `matched_data_region_bytes` in `matching.json`
is that number.

## Classification after this change

Retail executable, map at this commit. "Referenced" counts byte-pattern
candidates: absolute dwords and rel32 call targets inside mapped code, and
aligned pointers in initialized data. They are not decoded instructions.

| Section | Class | Ranges | Bytes |
| --- | --- | ---: | ---: |
| .text | **mapped** (sized map rows) | | 3,262,669 |
| .text | alignment padding (match.py rule) | 10,236 | 83,195 |
| .text | unmapped code: unreferenced | 1,443 | 76,957 |
| .text | alignment fill after unmapped code | 2,547 | 22,822 |
| .text | unmapped code: unreferenced identical copy | 1,134 | 15,366 |
| .text | alignment fill after a mapped extent, before unmapped code | 1,000 | 8,469 |
| .text | unmapped code: address taken | 31 | 1,050 |
| .text | unmapped code: static initializer/terminator (.CRT$X* table entry) | 23 | 480 |
| .text | unmapped code: direct call/jump target from mapped code | 2 | 186 |
| .text | switch jump table | 2 | 52 |
| .text | **section total** | | 3,471,246 |
| .rdata | **mapped** (sized map rows) | | 608,294 |
| .rdata | alignment padding (match.py rule) | 11,750 | 23,297 |
| .rdata | zero fill (unreferenced) | 863 | 4,275 |
| .rdata | unknown data, unreferenced | 223 | 3,654 |
| .rdata | unknown data, referenced | 121 | 2,992 |
| .rdata | string (unnamed, referenced) | 260 | 2,975 |
| .rdata | string bytes (unnamed, unreferenced) | 87 | 750 |
| .rdata | zero fill after an unnamed string, to 4-byte alignment | 261 | 497 |
| .rdata | named, extent unknown | 4 | 32 |
| .rdata | **section total** | | 646,766 |
| .data (file-backed) | **mapped** (sized map rows) | | 385,699 |
| .data (file-backed) | unknown data, unreferenced | 324 | 24,476 |
| .data (file-backed) | unknown data, referenced | 88 | 3,220 |
| .data (file-backed) | named, extent unknown | 11 | 1,676 |
| .data (file-backed) | zero fill (unreferenced) | 54 | 1,617 |
| .data (file-backed) | alignment padding (match.py rule) | 413 | 816 |
| .data (file-backed) | string bytes (unnamed, unreferenced) | 68 | 246 |
| .data (file-backed) | zero fill after an unnamed string, to 4-byte alignment | 20 | 38 |
| .data (file-backed) | x87 constant (unnamed, FPU operand in mapped code) | 1 | 4 |
| .data (file-backed) | **section total** | | 417,792 |
| .data (zero-fill) | **mapped** (sized map rows) | | 34,154,793 |
| .data (zero-fill) | zero-fill/BSS, referenced | 42 | 777,363 |
| .data (zero-fill) | zero-fill/BSS, unreferenced | 1,118 | 17,736 |
| .data (zero-fill) | named library data (archive member) | 21 | 4,480 |
| .data (zero-fill) | named, extent unknown | 9 | 36 |
| .data (zero-fill) | **section total** | | 34,954,408 |

Before this change the same tool reported, for `.rdata`: 156,308 mapped,
388,444 named string literals with the size in their name, 39,284 named
library data, 5,285 PE structures, 16 named `__real@` constants, 12,976 bytes
of rule padding and 6,464 bytes of named rows without extent; for `.data`
file-backed: 379,767 mapped and 5,028 named library data; zero-fill: 34,152,018
mapped and 7,232 named library data. `.text` is unchanged.

## What this change mapped

| Class | Rows | Bytes | Proof |
| --- | ---: | ---: | --- |
| string literal sizes (`??_C@`) | 8,764 | 388,454 | length and CRC in the name equal the retail bytes |
| `__real@` sizes | 2 | 16 | value in the name equals the retail bytes |
| PE structures | 252 | 5,285 | import descriptors, lookup tables, hint/name entries, DLL names, IAT terminators, debug directory and CodeView record, load configuration, SafeSEH table, all from the PE headers |
| archive data placed by a mapped symbol | 520 | 46,176 | the member section is contributed whole; initialized bytes equal the member (bound relocations compared, others masked) |
| archive data placed by unique bytes | 662 | 20,572 | the member section (relocations as wildcards) occurs once in retail data: dxguid GUIDs, runtime tables and messages, dinput8 device-order tables |
| C++ EH table sizes | 8 | 200 | FuncInfo magic, maxState, nTryBlocks and nCatches |
| **total** | | **460,703** | |

Expected badges after `match.py --update` (data members as in the current
report): mapped code 93.991 % symbol-only, **96.388 %** with padding;
mapped file-backed data **93.371 %** symbol-only, **95.636 %** with padding
(was 50.357 %); mapped zero-fill **97.712 %** (was 97.704 %).

Nothing here names a function. Unmapped functions need names from source,
reference symbol data or sibling builds; the cleanup plan removes address
names rather than adding them (docs/cleanup-audit.md, batch 8).

## What remains, and what it needs

| Section | Class | Bytes | Work |
| --- | --- | ---: | --- |
| .text | unreferenced functions (1,414 of 1,443 start 16-aligned; no call, pointer or table reference from mapped code or data) | 76,957 | functions kept although nothing mapped reaches them: name from reference data or Android/sibling builds, then source. 34.7 k of all unmapped code lies in `nu3d_includes.cpp` and `nu3d_pc_includes.cpp` |
| .text | identical unreferenced copies | 15,366 | header `static` helpers emitted per compiland (the 28-byte vector store appears 172 times): one source definition per owning compiland, not map rows |
| .text | fill after or before unmapped code | 31,291 | becomes rule padding once the neighbouring function is mapped |
| .text | address-taken, called, initializer stubs, jump tables | 1,768 | 23 `.CRT$XC` initializer/terminator stubs (`??__E`/`??__F` of their globals), 31 callbacks, 2 called functions, 2 switch tables |
| .text | of the above, code outside the compiland table (runtime/SDK) | 5,849 | run `tools/libmatch.py` against a full build; whatever it does not accept is code the pinned archives do not contain |
| .rdata | unnamed strings and their fill | 4,222 | referenced only from unmapped code or tables; literal or `const char[]` cannot be told apart from bytes (`--dry-run literals` lists the code-referenced ones with the literal name) |
| .rdata | unknown data and zero fill | 10,921 | game tables and runtime data without anchor: typed globals in their owning compilands |
| .data file-backed | unknown initialized data, zero fill and stray strings | 29,601 | game globals not yet declared; see the worklist |
| .data file-backed | named rows without extent | 1,676 | vorbis tables (`_ATH`, `_COS_LOOKUP`, ...) and five `LEGO_AIPATHCNX_*`: `datasizes.py --write` against a full build |
| .data zero-fill | BSS arrays | 795,099 | 42 referenced ranges hold 777,363 bytes; mostly a handful of large pools (below) |
| .data zero-fill | named library data / named without extent | 4,516 | runtime BSS whose section has no unique anchor; size them from the member once one name of the section is bound |

### Worklist for "data into owning compilands"

The largest unmapped zero-fill and data ranges, with the compilands whose
mapped code references them (candidate references, by count) and the
preceding map row:

| Address | Bytes | Class | Referencing compilands (mapped code) | Preceding map row |
| --- | ---: | --- | --- | --- |
| `0x027a84a0` | 267,800 | zero-fill/BSS, referenced | src/legogame/move.c (69), (runtime/SDK) (42) | `_DwaWeightPoolPc` |
| `0x0280d8c0` | 163,840 | zero-fill/BSS, referenced | src/legogame/move.c (64), src/legogame/creature.c (30) | `_PcGeometryInverseViewProjectionMatrixPc` |
| `0x027e9eb8` | 132,096 | zero-fill/BSS, referenced | src/gameapi/rtl.c (38), (runtime/SDK) (36) | `_DwaBlockTablePc` |
| `0x028368d0` | 75,776 | zero-fill/BSS, referenced | (runtime/SDK) (17), src/legogame/move.c (13) | `_NuLgtLaserData` |
| `0x0284931c` | 47,176 | zero-fill/BSS, referenced | src/gameapi/gameliball.cpp (9), (runtime/SDK) (8) | `_NuCameraProjectionGlobalScaleX` |
| `0x00987530` | 43,152 | zero-fill/BSS, referenced | src/gameapi/gameliball.cpp (71), (runtime/SDK) (66) | `_NuRndr_DebrisPlane` |
| `0x009821c0` | 17,088 | zero-fill/BSS, referenced | (runtime/SDK) (29), src/editor/edlevelall.cpp (27) | `_RendererClearRecordsPc` |
| `0x0097fa58` | 8,720 | zero-fill/BSS, referenced | (runtime/SDK) (14), src/legoapi/lighting.c (6) | `_PCDeviceSamplerInvalidation` |
| `0x0280a581` | 7,967 | zero-fill/BSS, referenced | src/legogame/move.c (43), src/legogame/game_obj.c (19) | `_NuPortalVisibilityBitsPc` |
| `0x029231d8` | 6,400 | zero-fill/BSS, unreferenced | - | `_TerrWallNorm` |
| `0x0097e290` | 5,892 | zero-fill/BSS, referenced | src/gamelib/net/?0x6133e0 (1), src/legogame/game.c (1) | `_PcGeometryViewProjectionMatrixPc` |
| `0x00856064` | 2,052 | zero-fill/BSS, referenced | (runtime/SDK) (2), src/nu2api/nu3d_includes.cpp (2) | `_PcControlTextPc` |
| `0x007e815c` | 1,932 | unknown data, unreferenced | src/gameapi/aisysall.cpp (4), src/legoapi/gizspinner.c (2) | `?PcMouseDataFormat@@3UPC_DI_DATA_FORMAT_PC@@B` |
| `0x0097bb58` | 1,652 | zero-fill/BSS, referenced | src/gameapi/gameliball.cpp (1), (runtime/SDK) (1) | `_nurndr_tritot_this_frame` |
| `0x00973160` | 1,440 | zero-fill/BSS, referenced | - | `_zy` |
| `0x02974d51` | 1,031 | zero-fill/BSS, unreferenced | - | `_PcCapsConfigured` |
| `0x0280d400` | 1,024 | zero-fill/BSS, unreferenced | - | `_NuRndrFxColourPc` |
| `0x029896a4` | 764 | zero-fill/BSS, referenced | src/gameapi/gameliball.cpp (1) | `_PcExecutableFilename` |
| `0x02920b10` | 640 | zero-fill/BSS, unreferenced | - | `_edptl_torus_menu` |
| `0x0280c4e0` | 640 | zero-fill/BSS, referenced | src/legogame/levels.c (1) | `_clip_test_mtx` |
| `0x02989358` | 520 | zero-fill/BSS, unreferenced | - | `_saveload_slotcode` |
| `0x028358c8` | 520 | zero-fill/BSS, unreferenced | - | `_NuLgtArcLaserCnt` |
| `0x0081f4a8` | 520 | named, extent unknown | (runtime/SDK) (7), src/legogame/game.c (1) | `_floor0_exportbundle` |
| `0x029753c0` | 512 | zero-fill/BSS, unreferenced | - | `_nu_keyboard_pages` |

Unmapped code and data per original compiland (top rows; the full table is
`python3 tools/unmapped.py --worklist`):

| Compiland | Unmapped code bytes (chunks) | Unmapped data bytes (ranges) | Unmapped zero-fill bytes (ranges) |
| --- | ---: | ---: | ---: |
| src/legogame/move.c | 406 (15) | 56 (4) | 439,735 (4) |
| src/gameapi/rtl.c | 337 (20) | 0 (0) | 132,096 (1) |
| (code outside config/units.csv: runtime and SDK) | 5,849 (283) | 6,603 (83) | 103,852 (7) |
| src/gameapi/gameliball.cpp | 5,161 (83) | 324 (5) | 92,752 (6) |
| (no mapped-code reference) | 0 (0) | 26,699 (1029) | 19,180 (1120) |
| src/nu2api/nu3d_includes.cpp | 21,224 (413) | 408 (7) | 36 (6) |
| src/nu2api/nu3d_pc_includes.cpp | 13,480 (300) | 4 (1) | 4 (1) |
| src/gamelib/net/?0x6133e0 | 490 (4) | 0 (0) | 5,892 (1) |
| src/nu2api/PCInput.cpp | 4,896 (107) | 0 (0) | 0 (0) |
| src/editor/edlevelall.cpp | 3,480 (152) | 472 (1) | 0 (0) |
| src/gameapi/aisysall.cpp | 1,376 (10) | 1,940 (2) | 0 (0) |
| src/gameapi/edtoolsall.cpp | 2,636 (54) | 4 (1) | 37 (2) |
| src/gameapi/gamemenuall.cpp | 2,513 (65) | 0 (0) | 0 (0) |
| src/nu2api/nuwin32/d3dApiCalls.cpp | 2,215 (29) | 0 (0) | 0 (0) |
| src/nu2api/nucore_includes.cpp | 1,746 (59) | 135 (3) | 0 (0) |
| src/nu2api/nuwin32/pcapi.cpp | 1,498 (28) | 0 (0) | 0 (0) |
| src/nu2api/nusound3/pc/nusound.cpp | 1,482 (71) | 0 (0) | 0 (0) |
| src/legogame/chris.c | 1,326 (42) | 108 (1) | 0 (0) |
| src/legogame/?0x4200c0 | 1,109 (16) | 0 (0) | 0 (0) |
| src/nu2api/?0x7493e0 | 755 (23) | 14 (1) | 0 (0) |
| src/nu2api/CRC16.cpp | 743 (25) | 0 (0) | 0 (0) |
| src/legogame/episodeI.c | 591 (15) | 104 (3) | 0 (0) |
| src/nu2api/socksysall.cpp | 353 (9) | 268 (2) | 56 (1) |
| src/legogame/levels.c | 33 (3) | 0 (0) | 640 (1) |
| src/nu2api/nurain.c | 639 (5) | 0 (0) | 0 (0) |
| src/nu2api/nuwin32/d3dCore.cpp | 530 (30) | 108 (1) | 0 (0) |
| src/nu2api/ShaderObjectKey.cpp | 608 (38) | 0 (0) | 0 (0) |
| src/nu2api/numath_includes.cpp | 545 (16) | 16 (1) | 4 (1) |
| src/nu2api/?0x6dff30 | 42 (4) | 0 (0) | 512 (1) |
| src/nu2api/nuwin32/Stubs.cpp | 537 (57) | 0 (0) | 0 (0) |
| src/legoapi/gameantinode.c | 527 (4) | 0 (0) | 0 (0) |
| src/gameapi/apiobject.c | 493 (11) | 8 (1) | 0 (0) |
