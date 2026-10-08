# saga-pc

**Runtime status: not playable.** Reconstructed startup includes native window,
input and renderer setup. The source executable still crashes during renderer
startup before a level loads. Device presentation and recurring gameplay remain
unverified; see the [current roadmap](docs/roadmap.md).

<!-- matching-badge -->
![fuzzy code](https://img.shields.io/badge/fuzzy%20code-28.424%25-yellow)<!-- /matching-badge --> <!-- mapping-badge -->![mapped code](https://img.shields.io/badge/mapped%20code-42.063%25-yellow) ![mapped data](https://img.shields.io/badge/mapped%20data-1.796%25-red) ![mapped code+data](https://img.shields.io/badge/mapped%20code%2Bdata-5.336%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*.

The goal is readable C/C++ that compiles into the exact same x86 machine code as
the original game, building on the [Android decompilation](https://github.com/opensagadev/saga).

This project is at an early stage. Our sources build a Windows executable and
reconstructed functions are compared with the original; the game is still incomplete.
The first badge tracks objdiff-weighted coverage of the original code: compared
functions contribute their fuzzy similarity, and unimplemented code contributes
zero. It is an estimate of code similarity, not a percentage of working game
features or an exact byte match. The mapping badges count known symbol extents,
including unimplemented symbols, against the original code/data section sizes.
Aliases count once; unknown sizes contribute zero. Native objdiff similarity among compared
functions and exact code coverage are recorded in [matching.json](matching.json).
The report also records identified code as the union of known inventory extents
and complete reviewed function ranges. It shows reviewed bytes outside the
inventory separately; those identities can still lack a usable signature or
source implementation. This code measure uses the retail `.text` denominator.

| Mapping denominator | Original bytes |
| --- | ---: |
| Code (`.text`) | 3,471,246 |
| Data (`.rdata` + `.data`, including zero-initialized storage) | 36,018,966 |
| Combined code and data | 39,490,212 |

These denominators measure section storage, rather than the unknown sum of all
original symbol sizes. They exclude resources, image headers and section gaps,
but retain padding within code/data sections. The large zero-initialized data
area contributes to data coverage even though it occupies little space on disk.

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the symbol map, and the build
and matching workflow, and the [documentation index](docs/README.md) for technical
guides and historical investigations.

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
