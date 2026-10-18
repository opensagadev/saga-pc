# Resolve Debug crash addresses offline

The Debug observer records a fault instruction (EIP), runtime module base and 16 raw
stack words. These are first-chance observations and can include handled exceptions.
The tool below resolves addresses with the exact build's linker map. It does not
unwind a call stack, provide source lines or change the game.

After a completed Debug build, retain its executable and
`build/Debug/LEGOStarWarsSaga_debug.map` together before rebuilding. Capture a binding
for that pair (choose a new manifest path for each build):

```sh
python3 tools/crash_symbols.py capture \
  --exe build/LEGOStarWarsSaga_debug.exe \
  --map build/Debug/LEGOStarWarsSaga_debug.map \
  --manifest /tmp/debug-crash-binding.json
```

Capture extracts the build identity from the executable, verifies map/PE timestamp,
preferred image base and function section addresses, then records both complete
SHA-256 hashes. It refuses to overwrite an existing manifest. Capture establishes
provenance for the supplied pair; it cannot prove how a manually edited map was
produced. Preserve the pair and manifest together. Source files or the current git
revision are not substitutes for the executable's embedded identity.

For a record in `build/log/latest-fault-*.log` or its timestamped normal log, take
`build=`, the main executable's `module-at-init` `base=`, and `EIP=` from the **same
record**. Hexadecimal command arguments require `0x`. For example (replace the
illustrative build ID and addresses with your record):

```sh
python3 tools/crash_symbols.py resolve \
  --exe build/LEGOStarWarsSaga_debug.exe \
  --map build/Debug/LEGOStarWarsSaga_debug.map \
  --manifest /tmp/debug-crash-binding.json \
  --build-id abcdef012345-123456abcdef \
  --module-base 0x00400000 --ip 0x0041ed70 \
  --stack 0x0041ed60 --stack 0x00000000
```

Resolution rejects changed artifact hashes or a different fault build identity.
Use the recorded base of this executable, not a DLL's base; unknown module identity
cannot be safely resolved. DLL faults require that DLL's own symbols and are outside
this tool's scope. The module list was captured during logger initialization, so a
later-loaded module may be unavailable. Runtime relocation is accounted for using
`runtime address - runtime base + preferred base`.

JSON reports a matching function start or the nearest preceding function and its
object owner. Aliases are retained. A nonzero offset is explicitly **not extent
verified**: it can fall in padding or code with no map symbol, even before the next
symbol. Addresses outside executable PE sections are not attributed to functions.
The fault IP is the observed instruction; raw stack words are only unverified
address candidates, never unwound frames or confirmed callers. At most 16 stack
words are accepted. Return-address adjustment is not guessed. No source-line or
full-stack claim is made, including when a stack word matches a symbol exactly.

The current VC8 Debug build retains `/Z7` CodeView information in object files but
has no linked PDB/debug directory. Wine's `winedbg` provides `bt`, but the existing
object files do not automatically supply a symbolic backtrace. The map-based
lookup works offline without attaching a debugger or rerunning the game.
