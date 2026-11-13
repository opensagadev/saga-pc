# saga-pc

<!-- matching-badge -->
![fuzzy code match](https://img.shields.io/badge/fuzzy%20code%20match-90.913%25-brightgreen)<!-- /matching-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

<!-- mapping-badge -->

| Metric / coverage | Meaning |
| --- | --- |
| ![exact code match](https://img.shields.io/badge/exact%20code%20match-60.545%25-orange) | Byte-identical code and the alignment padding after it. |
| ![mapped code](https://img.shields.io/badge/mapped%20code-100.000%25-brightgreen) | Known code extents and the alignment padding between them. |
| ![named code](https://img.shields.io/badge/named%20code-94.285%25-brightgreen) | Mapped code under identified names; another 2.417% is bounded under address names (`fun_XXXXXXXX`). |
| ![mapped file-backed data](https://img.shields.io/badge/mapped%20file--backed%20data-95.639%25-brightgreen) | Known data stored in the executable and the zero padding between it. |
| ![reproduced file-backed data](https://img.shields.io/badge/reproduced%20file--backed%20data-90.511%25-brightgreen) | Data stored in the executable that our objects or pinned library members define byte-exactly ([details](docs/mapping-status.md#reproduced-data)). |
| ![mapped zero-fill/BSS](https://img.shields.io/badge/mapped%20zero--fill%2FBSS-97.712%25-brightgreen) | Known zero-initialized storage. |

<!-- /mapping-badge -->

What the mapping numbers count, and every byte they do not yet cover, is in
[docs/mapping-status.md](docs/mapping-status.md).

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
