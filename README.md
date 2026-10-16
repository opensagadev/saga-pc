# saga-pc

<!-- matching-badge -->
![fuzzy code match](https://img.shields.io/badge/fuzzy%20code%20match-43.811%25-orange)<!-- /matching-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

<!-- mapping-badge -->

| Metric | Coverage | Meaning |
| --- | ---: | --- |
| Exact code match | 12.567% | Byte-identical code. |
| Mapped code | 67.578% | Known code extents. |
| Mapped file-backed data | 27.866% | Known data stored in the executable. |
| Mapped zero-fill/BSS | 2.339% | Known zero-initialized storage. |

<!-- /mapping-badge -->

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*: C/C++ source that compiles with the original
compiler (MSVC 2005) into the same x86 code as the retail executable. Most game
code comes from the [Android decompilation](https://github.com/opensagadev/saga)
and is adapted to the PC build.

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
