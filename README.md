# saga-pc

<!-- matching-badge -->
![matching](https://img.shields.io/badge/match%20progress-0.266%25-red)<!-- /matching-badge --> <!-- mapping-badge -->![mapping](https://img.shields.io/badge/mapping-0.918%25-red)<!-- /mapping-badge --> [![Build](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml/badge.svg)](https://github.com/opensagadev/saga-pc/actions/workflows/build.yml)

**saga-pc** is a matching decompilation of the GOG PC release of
*LEGO Star Wars: The Complete Saga*.

The goal is readable C/C++ that compiles into the exact same x86 machine code as
the original game, building on the [Android decompilation](https://github.com/opensagadev/saga).

This project is at an early stage. Our sources build a Windows executable and
reconstructed functions are compared with the original; the game is still incomplete.
The badges track exact code matches and symbol coverage of the original virtual
image, respectively.

See [CONTRIBUTING.md](CONTRIBUTING.md) for setup, the symbol map, and the build
and matching workflow.

## License

Source is distributed under [GNU GPL version 3](LICENSE). This independent
project does not distribute the original game or its assets, which remain the
property of their respective owners.
