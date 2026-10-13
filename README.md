# saga-pc

<!-- matching-badge -->
![fuzzy code](https://img.shields.io/badge/fuzzy%20code-36.843%25-orange)<!-- /matching-badge --> <!-- mapping-badge -->![mapped code](https://img.shields.io/badge/mapped%20code-62.057%25-orange) ![mapped data](https://img.shields.io/badge/mapped%20data-2.562%25-red) ![mapped code+data](https://img.shields.io/badge/mapped%20code%2Bdata-7.792%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*: C/C++ source that compiles with the original
compiler (MSVC 2005) into the same x86 code as the retail executable. Most game
code comes from the [Android decompilation](https://github.com/opensagadev/saga)
and is adapted to the PC build.

**Status:** not playable yet. The source EXE starts the window, devices and
permanent-data loading; rendering a loaded world is not reached. Of the original
`.text` (3,471,246 bytes), about 44% is mapped, 30% is covered by objdiff-weighted
similarity and 7% matches byte for byte. See the [roadmap](docs/roadmap.md).

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the workflow and the automated
source-shape search (`tools/permute.py`).

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
