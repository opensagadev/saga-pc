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
being recovered. `python3 tools/datasizes.py --write` fills such sizes from the
declared C/C++ size when it agrees with the compiled COFF extent (which may only
add alignment padding) and fits before the next known original address; larger
declarations are reported as conflicts and stay blank. The object check still requires its address binding and checks
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

## Evidence for code rows, and address names

A row in this file claims an address, a linker name and an extent. Rows added
since the code-mapping pass also record why, in
[`code-evidence.csv`](code-evidence.csv) (written and checked by
`tools/codemap.py`; extents from `tools/unmapped.py --functions`). Its columns:

| Column | Meaning |
| --- | --- |
| `address`, `size`, `symbol` | the symbols.csv row |
| `evidence` | `copy`, `compiler`, `sibling` or `extent` (below) |
| `proposed` | a name the evidence suggests but cannot spell as a linker name (a sibling function in a C++ compiland, whose MSVC decoration needs the prototype; a helper copy whose compiland is undetermined) |
| `compiland` | the compiland of config/layout-target.csv holding the function |
| `signals` | the independent observations behind the row |
| `port` | a source to port from, when one exists (`android:<path>` in the public Android decompilation) |

| Evidence | What proves it | Name |
| --- | --- | --- |
| `copy` | bytes equal (near-call displacements masked) to a named header-static helper; or the empty static directly before such a copy (`NuError`, which the sibling build declares `extern "C"` and emits first in every object) | the helper's name with the compiland's prefix, as the earlier copies are named: `_<Prefix><Helper>Pc` in C, `?<Prefix><Helper>` with the helper's signature in C++ (`<Prefix>` is the compiland's file stem in CamelCase, qualified by its directory when two compilands share a stem) |
| `compiler` | a `.CRT$XC` table slot points at it, or a `.CRT$XC` stub registers it with `atexit`; or such a stub calls it with its object as `this` | `??__E<object>@@YAXXZ` / `??__F<object>@@YAXXZ` when the stub passes the object as `this` (`mov ecx, offset object` before the constructor or destructor call), otherwise an address name; the function it calls so is the constructor `??0<Class>@@QAE@XZ` or destructor `??1<Class>@@QAE@XZ` of the object's mapped type (proposed only when the class has a vftable: the destructor may be virtual) |
| `sibling` | the sibling build has a function of that name between the same two named neighbours of the same source file, and its named callees, float constants or named globals agree; it is the only candidate there with an agreeing signal, and this the only unmapped function there that agrees with it | the C name in a C compiland; in a C++ compiland an address name with the name in `proposed` |
| `extent` | only the decoded extent | address name |

Runtime and SDK library functions are named by `tools/libmatch.py` and recorded
in [`library-objects.json`](library-objects.json); they are not repeated in
the ledger. Rows older than the ledger carry no recorded evidence (most come
from reference symbol data).

**Address names.** A function whose extent is known but whose name is not gets
the standard address name `fun_` followed by the eight lowercase hex digits of
its retail address: `fun_0060a1b0`, linker name `_fun_0060a1b0` (C decoration;
the row is renamed, and decorated as C++ if needed, when the function is
identified or ported). One format everywhere: no `FUN_`, `sub_`, upper-case
digits or shortened addresses. The analogous form for data is `dat_` plus
eight hex digits (`dat_007facf0`, linker name `_dat_007facf0`); no row uses it
yet. Address names are counted separately from identified names
(`matching.json` `mapping_code_address_named_bytes`; README) and by
`tools/oddscan.py` (class `address-name`, over `src/` and the map rows), as a
class that shrinks through real names, not as a violation: naming a row lowers
the count, and a new address name (in the map or ported under that name) raises
it above the budget on purpose.

Older address-style names remain: about 1,900 data rows whose name ends in
their address (`_PcInputText77066c`, `_TextIdPc880e54`,
`_AITransferLiteral757ebc`) and the address-suffixed data names in sources
(oddscan `hex-name`); no code row and no function name in `src/` uses an
older address form any more. They are not renamed here. A later rename batch:

1. renames each unnamed data row to `dat_XXXXXXXX` (its retail address; C
   decoration `_dat_...`, or the C++ decoration its `ctype` needs) and each
   remaining unnamed code row to `fun_XXXXXXXX`, keeping address, size and type;
2. renames every reference in `src/` (declarations, `extern`s, `MAP_` macros,
   `ALIAS` entries) in the same commit, so `check.py`, `relocmap.py` and the
   object comparison keep resolving the same addresses;
3. leaves rows with any source evidence (a string they hold, a Batman or Android
   name) to be named from that evidence instead;
4. reruns `tools/oddscan.py --write-budget` (hex-name falls, address-name rises
   by the same amount) and `tools/match.py --update`, whose exact and fuzzy
   numbers must not change.

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
[`src/externals.h`](../src/externals.h). MSVC C and C++ probes verify the linker
spellings generated by those headers. The known API declarations produce
`__imp__...` IAT references; both IAT slots and complete local thunks are mapped. Only the
12 Bink imports still use opaque pointer-slot declarations: their decorated export
names reveal stack-byte counts, not enough information to assert full prototypes.
Their function `ctype` cells remain blank.

The bundled SDK's `winternl.h` omits `__stdcall` on `RtlUnwind`. The supplemental
header corrects this one ABI declaration: the pinned `Kernel32.Lib` contains
`__imp__RtlUnwind@16` and `_RtlUnwind@16`, corroborating the x86 calling convention.

The rest of the import metadata stored in `.rdata` is mapped as what it is. The
import descriptors use the linker names `__IMPORT_DESCRIPTOR_<module>` and
`__NULL_IMPORT_DESCRIPTOR` (20 bytes each). The linker-generated pieces without
a spellable linker name get provisional names: `saga_import_lookup_<dll>` (the
import lookup table with its terminator), `saga_import_address_end_<dll>` (the
IAT terminator, `\x7f<module>_NULL_THUNK_DATA` in the import library),
`saga_import_name_<dll>_<api>` (hint/name entries, padded to two bytes) and
`saga_import_dll_name_<dll>`. The debug directory (`saga_debug_directory`) and
its CodeView record (`saga_debug_data_0`), the load configuration
(`__load_config_used`, 72 bytes) and the SafeSEH handler table
(`___safe_se_handler_table`) complete the PE structures inside sections.
`python3 tools/unmapped.py --check` re-derives every one of these rows from the
PE headers, and every `??_C@`/`__real@` row from the bytes its name encodes.

The matcher recognizes mapped IAT slots and import thunks from the verified
original PE and lists them separately under `externals`. They supply relocation
bindings and count toward mapped image coverage; they do not count as reconstructed
functions or matched code/data. A compiled definition of an external slot or thunk
is an error. External APIs remain declarations; a legacy `src/externals.c`, if
present, must emit no definitions. Original-free CI checks
the latter rule; classification against actual import bytes happens locally.
