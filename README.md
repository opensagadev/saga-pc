# saga-pc

<!-- matching-badge -->
![fuzzy code](https://img.shields.io/badge/fuzzy%20code-6.583%25-red)<!-- /matching-badge --> <!-- mapping-badge -->![mapping](https://img.shields.io/badge/mapping-2.891%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*.

The goal is readable C/C++ that compiles into the exact same x86 machine code as
the original game, building on the [Android decompilation](https://github.com/opensagadev/saga).

This project is at an early stage. Our sources build a Windows executable and
reconstructed functions are compared with the original; the game is still incomplete.
The first badge tracks objdiff-weighted coverage of the original code: compared
functions contribute their fuzzy similarity, and unimplemented code contributes
zero. It is an estimate, not an exact byte match. The second tracks mapped symbol
extents across the original virtual image. Native objdiff similarity among compared
functions and exact code coverage are recorded in [matching.json](matching.json).
The report also records identified code as the union of known inventory extents
and complete reviewed function ranges. It shows reviewed bytes outside the
inventory separately; those identities can still lack a usable signature or
source implementation. This code measure uses the retail `.text` denominator.

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the symbol map, and the build
and matching workflow.

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
