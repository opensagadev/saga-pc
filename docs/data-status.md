# Initialised data against retail

Code matching cannot see what a table holds: a function that reads
`GameMenuInfo` is exact as long as its relocation names the right address,
whatever the table contains. `tools/datadiff.py` compares the data itself.

## Method

```
python3 tools/datadiff.py                     # ranked report (about 80 s)
python3 tools/datadiff.py --symbol MenuInfo --slots
python3 tools/datadiff.py --json report.json
```

For every data symbol that `config/symbols.csv` maps outside `.text`, each
current object that defines it is compared with retail bytes at its address.
Our relocations are resolved by name (mapped symbols get their address,
`$LN` labels their place in the mapped function, and unmapped locals such as
`$SG` strings are compared by the bytes they point to). Retail dwords that land
in `.text`, `.rdata` or `.data` are named by the mapped symbol that contains
them. A C tentative definition (COFF common) is skipped when another object
initialises the symbol, because the linker keeps that one. Each difference gets
a class:

| class | meaning |
|---|---|
| a | function slot NULL in ours, a function in retail |
| b | function pointer to another function, or one where retail has none |
| c | data pointer NULL or to other data |
| e | our extent is truncated, or one side has non-zero bytes past the other's end |
| f | zero-fill (BSS) in ours, initialised in retail, or the reverse |
| d | other scalar bytes |
| g | retail-initialised data that our code references and no object defines |
| u | pointer to an unmapped local that holds pointers itself (not verifiable) |
| p | the extents differ only by zero padding (no behaviour change) |

The tool also lists externals that our objects reference and that neither an
object nor a linked library defines (the build links with `/FORCE:UNRESOLVED`,
so such a reference reads or calls address 0), and initialised data our
objects define that `config/symbols.csv` does not map (never compared).

Rule: a data symbol is compared by its own bytes at its mapped size; a zero gap between two symbols is
part of neither and is not scored (not reproduced and not in the total), only reported as gap bytes.

## Where a global is defined

Each global is defined in the unit whose code owns it, the way the original
sources were split. `src/data.cpp` held most of them while placement had to be
pinned; with the honest-size rule (above) placement no longer scores, so they
move out by this rule, applied by script from the build objects' relocations:

1. Used by the code of exactly one unit: defined in that unit.
2. Used by several units of one module (`src/<module>/`): defined in the unit
   that references it most (ties: the lower layer, then the name); the others
   see the module header's declaration.
3. Used by several modules: a pooled literal (a `const` float, double or
   string that VC8 would emit as one `__real@`/`??_C@` constant shared by
   every module) goes to `src/shared_constants.c`; any other object goes to
   the lowest layer that uses it (nu2api, gamelib, gameframework, gameapi,
   legoapi, legogame, editor, then the top level), in its most-referencing unit.
4. Used only by other data (a string or a sub-table that only a table names):
   defined with that table. When the tables that name it go to different
   units, rule 2 or 3 picks among those units. A `static` string stays
   `static`; no static is shared by tables of two units.
5. Used by nothing: defined in the unit its retail address lies among (the
   original object order), or by the subsystem its name gives.

A table goes after the definitions in its unit that its initialiser names, and
the unit includes the module headers those names come from. A definition that
other units' tables still name leaves an `extern` declaration with them (until
the declaration pass moves it into the owner's module header). Initialisers keep
their values; masks, colours and values with the top bit set are written in
hex, and a matrix, vec4, plane or SSE operand that retail keeps 16-aligned
declares `__declspec(align(16))`.

### What `matching.json` covers

`tools/match.py` compares each mapped data symbol with a known size as one
blob after relocation and records only `exact` or `different`. It does not
say why: of its 2,610 `different` data rows, 2,568 differ only by alignment
padding in the extent. It does not compare the 9,333 rows without a size (33
of them have a definition), and it compares a C tentative definition as zero
bytes. Two names mapped to one address hide a value difference completely:
`__real@3fb999999999999a` (0.1) and `__real@3fb99999a0000000` (0.1f widened)
shared one address, so five of the seven functions that used the wrong
constant were reported exact.

## Current numbers

27,759 mapped data symbols, 17,263 with a definition (21,778 object
comparisons): 19,209 identical, 2,568 padding only, 1 real difference.
No unresolved references, no unmapped initialised definitions.

Fixed in this pass:

| symbol | file | class | effect |
|---|---|---|---|
| `GameMenuInfo` | src/data.cpp | a | 8 PC menus had no callbacks (Bounty Hunter Missions, end screens, credits, minikits) |
| `MenuInfo` 1007, 1019, 1021 | src/data.cpp | a | save/card menus: Formatting update, CardWarning enter/exit, HeaderSave enter |
| `PS2_VCNTR_X`, `PS2_VCNTR_Y` | src/data.cpp | f | screen centre starts at 320, 240 instead of 0 |
| `NuPrimCurrentPrimitiveType` | src/data.cpp | f | starts at 10000 (no primitive) instead of 0 |
| `nudlist_frame_active` | src/data.cpp | f | starts at 2 instead of 0 |
| `__real@3fb999999999999a`, `__real@bfb999999999999a` | editor, gizbuildit, rtlhelper | d | 0.1 constants are 0.1f in retail (build-it piece bob, editor nudges, duplicate and smoothing thresholds) |
| `??_R2AINet@@8` | src/gameapi/message.cpp | c | AINet's base is private in retail (RTTI only) |
| `??_7EdRefRTL@@6B@`, `??_7EdRefFog@@6B@` | config/symbols.csv | e | size included the next vtable's locator |

## Open items

`MenuInfo` (src/data.cpp, class a): nine slots point to functions that are not
ported, so these menus still have no draw or update step:

| menu | slot | retail function |
|---|---|---|
| 1001 | draw, update | 0x50a7f0 (empty), 0x50a910 |
| 1007 | draw | 0x5110d0 |
| 1019 | draw, update | 0x511e80 (empty), 0x50b270 |
| 1024 | enter, draw, update, exit | 0x509470, 0x509480, 0x509490, 0x5094a0 (all empty) |
