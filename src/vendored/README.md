# Vendored sources

These files were identified by their retained Xiph copyright/license banners,
source revision IDs and codec version string. They were moved unchanged from
`src/audio`; they include local adaptations for the original PC build and are
not pristine upstream releases.

- **Vorbis** (`vorbis/` and `vorbis_smallft.c`): libvorbis 1.2.0-era code.
  The retained `Xiph.Org libVorbis I 20070622` marker and 2007 revision IDs agree
  with upstream 1.2.0. Get the [official 1.2.0 source archive](https://downloads.xiph.org/releases/vorbis/libvorbis-1.2.0.tar.gz)
  from the [Vorbis project](https://xiph.org/vorbis/).
- **Ogg** (`ogg_bitwise.c`, `ogg_framing.c` and headers under `vorbis/ogg/`):
  modified Xiph code with retained 2004/2005 revision IDs. Its exact release is
  unverified; the [official libogg 1.1.3 archive](https://downloads.xiph.org/releases/ogg/libogg-1.1.3.tar.gz)
  is a historical reference, not a claim of an identical snapshot. See the
  [Ogg project](https://xiph.org/ogg/).

Keep `vorbis/COPYING` and all per-file notices, including the TU-Berlin GSM
attribution in `vorbis/lpc.c`. Reconstructed game/engine adapters such as
`nu2api/oggreader.cpp` and `nu2api/nustream.cpp` remain with their native owners.
