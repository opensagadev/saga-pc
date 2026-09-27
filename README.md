# saga-pc

<!-- matching-badge -->
![objdiff fuzzy](https://img.shields.io/badge/objdiff%20fuzzy-60.898%25-yellow)<!-- /matching-badge --> <!-- mapping-badge -->![mapping](https://img.shields.io/badge/mapping-1.311%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*.

The goal is readable C/C++ that compiles into the exact same x86 machine code as
the original game, building on the [Android decompilation](https://github.com/opensagadev/saga).

This project is at an early stage. Our sources build a Windows executable and
reconstructed functions are compared with the original; the game is still incomplete.
The first badge tracks objdiff's fuzzy similarity among functions we have
compiled and compared. It is not whole-game coverage. The second tracks mapped
symbol extents across the original virtual image; exact byte coverage of the
original code is recorded separately in [matching.json](matching.json).

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the symbol map, and the build
and matching workflow.

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
