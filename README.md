# saga-pc

<!-- matching-badge -->
![fuzzy code match](https://img.shields.io/badge/fuzzy%20code%20match-46.721%25-orange)<!-- /matching-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

<!-- mapping-badge -->

| Metric / coverage | Meaning |
| --- | --- |
| ![exact code match](https://img.shields.io/badge/exact%20code%20match-13.083%25-red) | Byte-identical code. |
| ![mapped code](https://img.shields.io/badge/mapped%20code-69.513%25-orange) | Known code extents. |
| ![mapped file-backed data](https://img.shields.io/badge/mapped%20file--backed%20data-32.781%25-orange) | Known data stored in the executable. |
| ![mapped zero-fill/BSS](https://img.shields.io/badge/mapped%20zero--fill%2FBSS-4.181%25-red) | Known zero-initialized storage. |

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

Ogg/Vorbis sources are fetched into ignored `lib/ogg-vorbis/` during the build;
see [dependency setup and provenance](DEPENDENCIES.md).
