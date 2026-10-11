# Roadmap

Current numbers are in [matching.json](../matching.json) and the README badges.
At the start of October 2026, 1.52 MB of the 3.47 MB `.text` had source and
averaged about 70% objdiff similarity; about 392 KB of the remaining gap sat in
functions with no stubbed dependencies, so it comes from source shape, not
missing work.

1. **Raise similarity of existing source without manual effort.** Run
   `tools/permute_loop.sh` on spare machines. Extend `tools/rewrite.py` with the
   VC8 patterns that remain (single result variable, cast and temporary
   placement, loop forms) as they show up in the logs.
2. **Recover the original translation units.** Functions are laid out in the
   executable per object file and in source order. Move functions so each
   source file covers one contiguous original range; this is required for
   `static` callees with register arguments and for same-file inlining.
3. **Map the rest of `.text` in bulk.** Compile Android files with VC8 and match
   the compiled functions against unmapped PC functions by structure, instead
   of identifying them one by one.
4. **Make it run.** Either keep closing startup dependencies in the source EXE,
   or inject reconstructed functions into the retail executable so the game
   runs from the start and each function is tested in place.
