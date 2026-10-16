# saga-pc

<!-- matching-badge -->
![exact code match](https://img.shields.io/badge/exact%20code%20match-12.567%25-red)<!-- /matching-badge --> <!-- mapping-badge -->![mapped code](https://img.shields.io/badge/mapped%20code-67.578%25-orange) ![mapped file-backed data](https://img.shields.io/badge/mapped%20file--backed%20data-27.866%25-orange) ![mapped zero-fill/BSS](https://img.shields.io/badge/mapped%20zero--fill%2FBSS-2.339%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*: C/C++ source that compiles with the original
compiler (MSVC 2005) into the same x86 code as the retail executable. Most game
code comes from the [Android decompilation](https://github.com/opensagadev/saga)
and is adapted to the PC build.

The coverage badges separate exact code matching, known code extents, file-backed
data, and zero-fill/BSS storage. Mapping counts known byte ranges even before
implementation; it is not a gameplay completion percentage. File-backed data
covers data sections stored in the executable, while zero-fill/BSS covers their
in-memory tails. See [metric definitions](config/symbols.md#coverage-metrics).

**Status:** incomplete. Human testing of the rebuilt executable confirmed visible,
movable characters and audio in Negotiations after the ship cutscene. This covers
the tested sequence, not the full game. Remaining reports include droids not
shooting, unusable panels, white blades, UI/minikit visibility and unspecified
audio issues. Complete visual fidelity remains unverified. See the
[roadmap](docs/roadmap.md) and [matching report](matching.json).

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the workflow and the automated
source-shape search (`tools/permute.py`).

`make Matching` (the default) builds the matching executable. `make Debug`
builds a separate executable with diagnostics and symbols. Both share the same
game behavior; build-kind differences are strictly diagnostics only. See
[build kinds and logging](CONTRIBUTING.md#build-kinds-and-logging).

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
