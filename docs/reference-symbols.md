# Reference symbols from a sibling game

A private symbol dump of another game made by the same studio on an older
version of the same engine (a PS2 build, debug symbols exported with stdump)
gives, for about 10,900 functions, the original source file, the prototype with
parameter names and types, and every local variable in declaration order. A
separate file has the struct, union, enum and typedef definitions with offsets.
About 2,560 function names also exist here. The dump is never committed; keep
it untracked under `build/reference/` (git-ignored).

The engine changed between the two games, so a hint is a lead: retail bytes
decide.

## Tool

`tools/refsyms.py` parses the dump and compares it with our sources (our side
is parsed with libclang). Without the dump every command prints a notice and
exits 0.

```sh
python3 tools/refsyms.py NuAllocFPar          # reference prototype and locals next to ours, with hints
python3 tools/refsyms.py --overlap            # functions in both, non-exact first
python3 tools/refsyms.py --hints out.json     # hints for every non-exact function in both
python3 tools/refsyms.py --structs [NAME]     # same-named structs: field type differences that change code
python3 tools/refsyms.py --files              # reference source file vs our unit
python3 tools/refsyms.py --reference DIR ...  # dump in another directory
```

Hints: parameter and return type classes (signedness, width, float, pointer
vs integer; typedefs and enums are resolved on both sides), locals only on one
side, the local count, the declaration order of shared names, and locals
declared several times (one per inlined expansion of a helper; the helpers of
the same file that take such a parameter are named).

## What paid off (pilot, 30 functions)

| hint | tried | exact | improved |
|---|---:|---:|---:|
| fewer locals in the reference: index the array or read the field at each use instead of a cached pointer/copy | 12 | 8 (+3 callers that inline them) | 1 |
| a helper expanded inline (repeated locals): call the same-unit function instead of a macro | 4 | 4 | 1 |
| declaration order / initialised locals | 2 | 0 | 2 |
| struct bit names (bitfield store where we masked a word) | 1 | 0 | 1 |
| parameter / return type class | 3 | 0 | 0 |

- The most frequent and most useful hint is *the reference has no local for
  this*. Decompiled ports cache `&array[index]` or `object->field` in a local;
  VC8 then keeps it in a callee-saved register, while the original indexed the
  array at every use (`NuAllocFPar`, `NuFileBeginBlkRead`,
  `NuStringTableUnload`, `NuMemFileRead`/`Write`, whose inlined copies made
  `NuFilePatchAddress`, `NuFileAlign` and `NuStringTableSaveCharacterList`
  exact). It is not universal: `edcamGetPosAng` and `NuMemFileClose` got worse
  (retail reads the global once, I4), so keep the change only when it helps.
- Repeated locals show inlining: `NuStrNICmpW` declares `c` twice, the
  parameter of `NuToUpperW`, so the string functions called `NuToUpperW` (same
  unit, inlined by VC8) rather than a macro. With the helper written as
  `if (range1) c -= 0x20; else if (range2) c -= 0x20;` four functions became
  exact. Inlining in the PS2 build is weaker evidence on its own: `NuStrLenW`
  is inlined there but called in retail.
- Struct definitions differ little where both games have the struct (149
  same-named structs, 16 field type differences, mostly PS2-specific). The
  useful part is bitfield names for words we treat as masks:
  `APIOBJECT_PC.flags_low` is the object flag bitfield (`ignore_antinodes` is
  bit 17), and retail stores it with the bitfield xor/and/xor sequence.
- Parameter type hints were mostly PS2-specific (`i64` file positions on PC,
  `u8` colour channels widened) and did not help.
- File assignment: 123 reference files contain mapped functions; 38 of them
  are spread over several of our units, mostly where the PC build compiles a
  group of engine files as one unit (`nucore_includes.c`, `nu3d_includes.cpp`,
  `numath_includes.c`) or where we split an editor file (`edptl.c` into
  `edpp.c`/`edpp_load.c`, `debris.c` over five units). Not acted on.

Remaining differences seen in the sample were known unexplained classes: U8
(`lea eax, [reg+1]` for `return 1` after a NULL test of that register), U1, and
callers that keep `eax` live across a call to the empty `NuFileError`
(declaring it `noreturn` improved some callers and broke others, so retail
probably calls two different, folded error functions).
