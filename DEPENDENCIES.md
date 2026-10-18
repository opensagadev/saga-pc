# Build-managed Ogg and Vorbis

The identifiable third-party source dependency is the Xiph Ogg/Vorbis family:
Ogg framing/bit packing and the Vorbis decoder. The compiler, CRT, Windows SDK,
objdiff and generated DLL import libraries remain separately managed build tools.

`make dependencies` downloads a pinned compatibility snapshot into ignored
`lib/ogg-vorbis/`. `make setup`, `make build`, and the formatting/analysis checks
prepare it automatically. No third-party implementation files are tracked in the
current source tree. The 22 C translation units keep their existing object names,
link order and compiler flags: Vorbis uses `/GS-`, except `vorbisfile.c`; the two
Ogg translation units retain the game flags.

`config/ogg-vorbis.json` pins the public project commit
`6370ba4aecfd5a18873c42a5dde1d3bef9be2a6e`, its archive SHA-256, and all 48
selected files. Only those regular archive members are materialized. Every file
must match its recorded hash before replacing the installed tree. COPYING and
source attributions are included. The archive is cached under `lib/.cache/`;
verified warm builds need no network. To preload a disconnected checkout:

```sh
python3 tools/dependencies.py --archive /path/to/snapshot.tar.gz
python3 tools/dependencies.py --offline
```

A missing or invalid cache fails explicitly in offline mode; a changed archive
never falls back to unpinned code. To repair a corrupt cache, remove that cache
file and retry online. `make clean` leaves verified dependency sources and cache
intact. Debug build IDs and Matching report inputs include the materialized
sources as well as their manifest and fetch tool.

This is an adapted PC compatibility snapshot from this project's history,
**not a pristine upstream release**. The retained Vorbis banner/revisions place
its origin in the 1.2.0 era ([official archive](https://downloads.xiph.org/releases/vorbis/libvorbis-1.2.0.tar.gz)).
The precise historical Ogg release is unverified; the
[official Ogg archive catalog](https://downloads.xiph.org/releases/ogg/) is a
reference, not the build input. Local matching changes, VC8 compatibility,
formatting, and notices are preserved byte for byte. Upgrading these libraries
requires a separate audited manifest change and native comparisons.
