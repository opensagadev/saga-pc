# saga-pc

<!-- matching-badge -->
![fuzzy code](https://img.shields.io/badge/fuzzy%20code-43.214%25-orange)<!-- /matching-badge --> <!-- mapping-badge -->![mapped code](https://img.shields.io/badge/mapped%20code-66.893%25-orange) ![mapped data](https://img.shields.io/badge/mapped%20data-3.078%25-red) ![mapped code+data](https://img.shields.io/badge/mapped%20code%2Bdata-8.687%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*: C/C++ source that compiles with the original
compiler (MSVC 2005) into the same x86 code as the retail executable. Most game
code comes from the [Android decompilation](https://github.com/opensagadev/saga)
and is adapted to the PC build.

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
