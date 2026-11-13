# File-scope declarations in units

Parallel work could not edit `src/saga.h`, so units collected their own
prototypes and `extern` data declarations. `tools/declscan.py` parses every
unit with libclang and the lint flags and puts each file-scope declaration
without a definition into one class:

| Class | Meaning | Action |
|---|---|---|
| A | repeats an earlier declaration (a header the unit includes, or the same unit) with the same canonical type | delete (`--fix`) |
| B | differs from the earlier declaration: unprototyped `()`, another array bound, an attribute | keep, review by hand |
| C | forward declaration of a function or variable defined later in the unit | keep for now |
| D | defined in another unit (or a vendored library) and declared by no header | header candidate |
| E | defined in no source (retail-only or dead); `E+` used, `E-` unused | delete `E-` |

```sh
python3 tools/declscan.py                       # per-unit counts, records in build/declscan.json
python3 tools/declscan.py src/legoapi --list --classes BD
python3 tools/declscan.py src/legoapi --fix --dry-run
python3 tools/declscan.py --check               # class counts against config/declscan-budget.json (CI)
python3 tools/declscan.py --write-budget        # lower the budget after removing declarations
```

`make check-core` runs `--check`: a unit that adds a file-scope prototype or
`extern` raises a class count above its budget and fails. Declare it in the
owner's header instead.

`--fix` deletes only whole-line declarations without a comment of their own.
A comment line directly above, a trailing comment or a nearby
`MATCH-EXCEPTION` keeps a declaration, and a line holding several
declarators goes only when all of them can go. Cross-unit identity is the
linker name, so a C++ declaration with another parameter type is not matched
to the definition.

## Results

The first pass removed 2,985 declarations (2,978 class A, 7 class E-) from
178 units. The code sections of every object stayed byte-identical to the
base build, and the exact function set stayed the same.

Deleting a redundant declaration is not always codegen-neutral. In two of
the 178 units it changed the code of a function that does not use the
declaration ("declaration noise", the same effect as the saga.h noise):
`ShaderLexicalClassesInitPc` in `shader_dictionary.cpp` (it changed
`ShaderCompilerDictionaryPc::EmitAttribute`) and five declarations in
`nu3d_includes.cpp` (they changed `NuDeferredFilterGen::render`). They were
found by bisecting the deletions against the object code. They stay, each
with a comment that says why, so the tool keeps them.

What remains:

| Directory | A (kept) | B | C | D | E+ |
|---|---:|---:|---:|---:|---:|
| (top: data.cpp, batman.cpp) | 0 | 0 | 2 | 192 | 0 |
| editor | 0 | 0 | 17 | 63 | 0 |
| gameapi | 9 | 2 | 43 | 822 | 0 |
| gamelib | 0 | 0 | 0 | 2 | 0 |
| legoapi | 85 | 2 | 292 | 1,235 | 1 |
| legogame | 12 | 0 | 2 | 69 | 0 |
| nu2api | 27 | 2 | 92 | 326 | 3 |
| total | 133 | 6 | 448 | 2,709 | 4 |

The 133 class A declarations that are left have a comment of their own (49), a
trailing comment (64), or share a line with a declaration that has to stay (20).
Review them by hand. Most class D declarations are functions (1,666);
of the 1,043 variables, 724 are defined in `data.cpp`. The largest units are
`gameapi/ai_registry_pc.c` (481), `legoapi/levels.cpp` (264), `data.cpp`
(116), `legoapi/shard.c` (102) and `nu2api/nu3d_includes.cpp` (79).

Class C against retail order (`config/symbols.csv`): 263 of the 350
function forward declarations would be unnecessary if the unit's functions
were in retail address order (every user comes after the definition there).
For 14 others a user comes before the definition in retail too, so the
original source also had a declaration. 18 have no user. The rest are
variables (98), or the function or a user has no mapping (55).

## Rule for future work

- Do not add a declaration to a unit when saga.h (or a header the unit
  includes) already declares it. Run `python3 tools/declscan.py <unit> --list`
  before committing; new class A or E- declarations are noise.
- A declaration that has to stay for matching (unprototyped `()`, a
  different local type, declaration noise) gets a comment that says why.
  The comment makes `--fix` keep it.
- After any deletion, check that the object code stays the same. Compare
  the code sections of the old and new objects, or compare exact sets by
  address after `tools/match.py --update`. Bisect any change down to the
  declaration that causes it and keep that declaration.
- The gate for a declaration removal includes the Debug compile
  (`make -k -j2 BUILD_KIND=Debug build`, zero compile errors): a declaration
  used only inside a diagnostic macro or a `#ifdef SAGA_DEBUG` block is
  invisible to the release build. `declscan.py` counts those uses too.

## Work list for after the layout move

The source files are about to be reorganised to the original compilands.
Classes C and D depend on that layout, so leave them alone until the move is
done:

1. Re-run the scan after the move. Moving functions into their original
   units turns many class D declarations into C or into nothing.
2. Class D: put the remaining cross-unit declarations into the header of the
   owning subsystem, next to the related declarations. The Batman Mac symbols
   (same engine, 2009) give the original source file of many functions,
   including statics, and name original headers such as `legoapi/legoapi.h`,
   `legoapi/customiser.h`, `nu2api/nu3d/nuprim.h` and
   `gameapi/Support/ThingManager.h`. Measure every move across all units:
   a change in what a unit sees can change unrelated functions (BoulderSpawnPc,
   SubItemMenu, InitGameAfterConfig, BlockadeRunnerDEjectCreaturePc,
   NuRndrParticleGroup and DebrisSetup2 are known to be fragile).
3. Class C: with the units in retail order, most forward declarations can go.
   Keep the 14 whose retail order still needs them.
4. Classes B and E+: review the 6 B and 4 E+ declarations by hand
   (`saveload_slotused`, `GizmoSnapOptionNamePc` and `NuPrimCoordSystemStack`
   have array bounds that saga.h leaves open, and `CustomiserMenu_End` is
   unprototyped. `DXGetErrorString9A`/`DXGetErrorDescription9A` come from
   DxErr9.lib and `__movsd` is an intrinsic).
