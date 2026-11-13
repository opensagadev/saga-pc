#!/usr/bin/env python3
"""Derive the original source-file (compiland) layout of the retail .text.

The linker places every object file's functions in one contiguous block, in
source order, so the retail .text is a sequence of compilands. This tool cuts
the mapped functions (config/symbols.csv) into those compilands and names them
from the evidence below, strongest first:

  file     a `__FILE__` path string in the retail EXE referenced from code in
           the block (only engine files carry them; read from res/orig/)
  marker   a cluster of header-static helper copies (fabs, NuFdiv, `ret`, ...)
           that VC8 emits at the start of a compiland (tools/unitmap.py)
  batman   the file of the same-named function in the LEGO Batman Mac debug map
           (private, never committed; `--batman DIR` with debugmap.tsv.gz and,
           for the files #included into unity builds, the binary itself)
  review   a decision recorded in REVIEW below, with its reason (alphabetical
           order of the game library, the PS2 sibling-game dump of
           docs/reference-symbols.md, C++-only constructs, ...)
  android  the unit name in config/units.csv (taken from the Android port)

Each compiland gets a proposed source path, a language (C or C++) with the
evidence for it, a confidence, and the source files it currently lives in.
Unity compilands (a .cpp that #includes .c/.cpp files) get one row per included
file. `--write` replaces config/layout-target.csv; without it the summary is
printed only.

    python3 tools/layoutscan.py --batman build/reference/batman-mac --write
    python3 tools/layoutscan.py --show 0x497c10      # one compiland with its functions

Without the retail EXE the unit-start markers fall back to the `marker` rows of
config/units.csv; without the Batman map only `review` and `android` names are
used and most rows come out undetermined. Both inputs only add evidence.
"""
import argparse
import bisect
import collections
import csv
import gzip
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if not __package__:
    sys.path.insert(0, str(ROOT))

TABLE = ROOT / "config/layout-target.csv"
COLUMNS = ("start", "end", "compiland", "inner_file", "language", "evidence", "confidence",
           "functions", "bytes", "current_unit_name", "current_source_files", "note")

# ------------------------------------------------------------------ reviewed knowledge

# Coarse regions of the game's own code (library code is recognised per function).
# The area is the proposed directory under src/; `orig` is the original TT tree
# where a retail or Mac `__FILE__` path proves it.
AREAS = [
    (0x401000, 0x401870, "legogame", "saga\\ (project dir; Mac 1.0: saga/pcbatman.cpp)"),
    (0x401870, 0x4eeae0, "legogame", "game library, files in alphabetical order"),
    (0x4eeae0, 0x4fff00, "nu2api", "nu2api.saga\\nucore"),
    (0x4fff00, 0x508f30, "nu2api", "nu2api.saga\\gamelib (PS2 sibling dump: gamelib\\gcutscn.c)"),
    (0x508f30, 0x53d130, "gameapi", "gameapi.saga\\gui\\gamemenu, gameapi.saga\\ai\\aisys"),
    (0x53d130, 0x53db50, "gamelib/net", "network library (C++)"),
    (0x53db50, 0x5476e0, "nu2api", "gameapi socksys, nu2api.saga\\gamelib"),
    (0x5476e0, 0x5b2da0, "legoapi", "legoapi library, project order (not alphabetical)"),
    (0x5b2da0, 0x60dee0, "nu2api", "nu2api.saga\\numath, nu3d, nu3d\\pc"),
    (0x60dee0, 0x617c10, "gamelib/net", "network library (C++)"),
    (0x617c10, 0x693b60, "gameapi", "gameapi.saga\\edtools, rtl, objects, gamelib"),
    (0x693b60, 0x6b7c40, "nu2api", "nu2api.saga\\nusound3"),
    (0x6b7c40, 0x6d3070, "editor", "editor"),
    (0x6d3070, 0x751000, "nu2api", "nu2api.saga\\nuwin32, shaderbuilder"),
]

# LEGO Batman (2008) renamed some files; TCS keeps the older name. Keys are
# Batman stems, values the TCS stem (only applied where Batman is the evidence).
RENAMES = {
    "technos": "techno", "ledges": "ledge", "zipups": "zipup", "tightropes": "tightrope",
    "securitydoors": "securitydoor", "suits": "suit", "shards": "shard",
    "detonators": "detonator", "hub_batman": "hub", "gamestatus_batman": "gamestatus",
    "pcbatman": "pcbatman", "listman_gen": "listman", "numem_gen": "nucore_includes",
}

# Unity compilands: the compiland name and how the inner (included) files are named.
UNITY = {"nucore_includes", "nucore_pc_includes", "numath_includes", "nu3d_includes",
         "nu3d_pc_includes", "aisysall", "edtoolsall", "objectsall", "gameliball",
         "cutscene_include", "gamemenuall", "socksysall", "edlevelall", "nusound3_include"}

# Reviewed decisions per compiland start (hex). name None = undetermined: the
# row keeps the Android name only as a hint. Keys: name, lang, conf, why, join
# (join=True folds the atom into the neighbouring compiland of the same name).
REVIEW = {
    0x401000: dict(name="pcbatman", lang="cpp", conf="high",
                   why="retail .\\pcbatman.cpp referenced from InitOnce; Mac 1.0 saga/pcbatman.cpp"),
    0x401810: dict(name=None, why="separate unit-start marker; Batman puts GameFog_Set in pcbatman.cpp; "
                   "second project file before the game library, name unknown"),
    0x405d70: dict(name="batman_gameai", conf="low",
                   why="Batman batman_gameai block after batarang, no marker between; fits the alphabetical "
                   "order (batarang < batman_gameai < bolt)"),
    0x40e700: dict(name="chris", conf="medium",
                   why="TCS only (no Batman counterpart); marker; Android chris.c fits between camera and config"),
    0x4200c0: dict(name=None, lang="cpp", why="PC control screen (C++ PC_CONTROL_SCREEN_PC thiscall methods) after "
                   "creature's last Batman function; no marker; alphabetically between creature and customise"),
    0x421050: dict(name=None, join=True, why="PC_CONTROL_SCREEN_PC methods, continues 0x4200c0"),
    0x4234b0: dict(name=None, join=True, why="PC_CONTROL_SCREEN_PC methods, continues 0x4200c0"),
    0x4238d0: dict(name="customise", conf="medium", why="TCS only; Android customise.c; alphabetical position fits"),
    0x458470: dict(name=None, why="marker; status screen code (Android status_alternate_pc invented); "
                   "alphabetically gamehint < ? < gamestatus"),
    0x45e350: dict(name="gamestatus", conf="medium",
                   why="marker; Batman gamestatus_batman (1 vote); TCS name assumed without the _batman suffix"),
    0x46a8d0: dict(name="gizmopickup", conf="high", why="Batman legogame/gizmopickup"),
    0x472770: dict(name="hatmachine", conf="medium",
                   why="marker; no Batman counterpart; Android hatmachine; grabber < hatmachine < hub"),
    0x491380: dict(name="lsw_hub_ai", conf="low",
                   why="TCS hub AI, no marker; Android lsw_hub_ai.c fits lever < lsw_hub_ai < menu"),
    0x491b90: dict(name=None, why="marker; 4 PC draw helpers, no counterpart; alphabetically l..m"),
    0x4ab8a0: dict(name="move", join=True, why="single functions between the two move halves, no marker"),
    0x4ab910: dict(name="move", join=True, why="single functions between the two move halves, no marker"),
    0x4abaa0: dict(name="move", join=True, why="single functions between the two move halves, no marker"),
    0x4abab0: dict(name="move", join=True, why="single functions between the two move halves, no marker"),
    0x4bc2f0: dict(name="pursuit", conf="medium", why="TCS only; Android pursuit.c fits panel < pursuit < radio"),
    0x4c00c0: dict(name=None, join=True, why="one function between sfx and shard"),
    0x4c8bd0: dict(name="speederchase", conf="medium", why="TCS only; Android speederchase; signal < sp < startup"),
    0x4cadb0: dict(name="starwars_gameai", conf="medium",
                   why="TCS only; Android starwars_gameai.c (Batman's analogue is batman_gameai); startup < st < suit"),
    0x4d8190: dict(name="episodeI", conf="high", why="marker; TCS episode files follow the alphabetical block"),
    0x4ddc80: dict(name="episodeII", conf="high", why="marker"),
    0x4e2cf0: dict(name="episodeIII", conf="medium", why="no marker; Android episodeIII.c"),
    0x4e4a40: dict(name="episodeIV", conf="medium", why="no marker; Android episodeIV.c"),
    0x4e5f10: dict(name="episodeV", conf="medium", why="no marker; Android episodeV.c"),
    0x4e9f30: dict(name="episodeVI", conf="medium", why="no marker; Android episodeVI.c"),
    0x4eeae0: dict(name=None, why="marker; DrawRopeCurved (Batman legoapi utilities) and Win32 imports thunks"),
    0x4ef6e0: dict(name="nucore_includes", conf="high",
                   why="Batman nucore_includes.cpp; retail numem.c, nufile.c, numemblk.c, nupad.cpp inside"),
    0x4fe640: dict(name="nucore_pc_includes", conf="medium",
                   why="marker; Batman pc/nucore_pc_includes.cpp; retail nucore\\pc\\nufile_pc.c inside"),
    0x4fff00: dict(name="cutscene_include", conf="medium",
                   why="marker; Batman cutscene_include.cpp (gcutscn+trigger+script); the PS2 sibling dump "
                   "has them as separate files"),
    0x508f30: dict(name="gamemenuall", conf="medium",
                   why="marker; apimenu/apisave/apitext interleaved, so one compiland; retail gui\\gamemenu\\apisave.c"),
    0x512850: dict(name="aisysall", conf="high", why="marker; Batman ai/aisys/aisysall.cpp"),
    0x53d130: dict(name=None, why="marker; 4 AINetTrace helpers"),
    0x53d1b0: dict(name=None, why="C++ NetListenerInterface methods, network library"),
    0x53d640: dict(name=None, join=True, why="continues 0x53d1b0"),
    0x53d670: dict(name=None, join=True, why="continues 0x53d1b0"),
    0x53db50: dict(name="socksysall", conf="high", why="marker; Batman socksysall"),
    0x543ed0: dict(name="nubridge", conf="medium", why="marker; PS2 sibling dump: gamelib\\ps2\\nubridge.c"),
    0x546fd0: dict(name="glutils", conf="medium", why="PS2 sibling dump: gamelib\\glutils.c; Android glutils.c"),
    0x547060: dict(name="listman", lang="c", conf="high", why="retail gamelib\\listman.c"),
    0x56cdb0: dict(name="guidelines", conf="medium", why="marker at 0x56cd80; Android guidelines; Batman has none"),
    0x5925c0: dict(name="customiser", conf="medium", why="marker; TCS only; Android customiser.c"),
    0x5a86d0: dict(name="simpletons", conf="medium", why="marker; TCS only; Android simpletons.c"),
    0x5b2da0: dict(name="numath_includes", conf="high", why="marker; Batman numath_includes; retail nugraph.c, nutrig.c"),
    0x5c1360: dict(name="nu3d_includes", conf="high", why="marker; Batman nu3d_includes; retail nuanim.c, nutexanm.c ..."),
    0x5e4600: dict(name="nu3d_includes", join=True, why="inside nu3d_includes, no marker"),
    0x5e4b00: dict(name="nu3d_includes", join=True, why="inside nu3d_includes, no marker"),
    0x5f7490: dict(name="nu3d_pc_includes", conf="medium",
                   why="marker; Batman pc/nu3d_pc_includes; retail nu3d\\pc\\*.c/.cpp inside, no marker between"),
    0x617c10: dict(name="edtoolsall", conf="high", why="marker; Batman edtools/edtoolsall; retail edtools\\edui.cpp"),
    0x62a5e0: dict(name="apiobject", conf="high", why="marker; Batman apiobject.c"),
    0x633df0: dict(name="rtl", lang="c", conf="high", why="marker; retail gameapi.saga\\rtl\\rtl.c"),
    0x642340: dict(name="objectsall", conf="high", why="marker; Batman objects/objectsall"),
    0x64fd90: dict(name="gameliball", conf="high", why="marker; Batman gamelib/gameliball; retail gamelib\\edgra.c, terrain.c"),
    0x693b60: dict(name="nusound3_include", conf="medium", why="marker; retail nusound3 pc\\nusound.cpp, pc\\oggreader.cpp"),
    0x6b7c40: dict(name="edlevelall", conf="medium", why="marker; editor unity (Android edlevelall)"),
    0x5c0240: dict(name="nufloat_pc", conf="low",
                   why="Batman pc/nufloat_pc.cpp is its own object; no marker here, may be part of numath_includes"),
    0x5c0310: dict(name="numaths", conf="low",
                   why="Batman pc/numaths.cpp is its own object; no marker here, may be part of numath_includes"),
    0x60ed30: dict(name=None, why="C++ members that Batman has in supportall; probably COMDAT inlines placed "
                   "with the first network object that uses them"),
    0x630e00: dict(name="nufmv_gen", area="nu2api/nufmv", conf="low",
                   why="one function, Batman nufmv/nufmv_gen.cpp; TCS spelling unknown"),
    0x6310c0: dict(name="dsplay_bink", area="nu2api/nufmv/pc/Bink", lang="cpp", conf="high",
                   why="marker; retail .\\pc\\Bink\\dsplay_bink.cpp from FmvInitializePc"),
    0x633390: dict(name="supportall", lang="cpp", conf="medium", why="marker; Batman supportall (C++ classes)"),
    0x633520: dict(name="supportall", join=True, why="continues supportall"),
    0x6335e0: dict(name="supportall", join=True, why="continues supportall"),
    0x696a90: dict(name="nusound", area="nu2api/nusound3/pc", lang="cpp", conf="medium",
                   why="marker-bounded 0x696a90-0x698fe0; retail .\\pc\\nusound.cpp referenced at 0x697f64, 0x698710"),
    0x697580: dict(name="nusound", join=True, why="continues nusound"),
    0x697a50: dict(name="nusound", join=True, why="continues nusound"),
    0x6dbab0: dict(name="pcapi", area="nu2api/nuwin32", lang="cpp", conf="high",
                   why="marker; retail .\\pcapi.cpp from NuInitHardwarePc (Mac nuwin32/pcapi.cpp)"),
    0x6dddf0: dict(name="windows", area="nu2api/nuwin32", lang="cpp", conf="medium",
                   why="retail .\\windows.cpp from PcCreateCursorMaterialPc, Batman WinMain in windows.cpp; "
                   "start between 0x6ddd40 and 0x6ddec0 (no marker)"),
    0x6de6d0: dict(name="Stubs", area="nu2api/nuwin32", lang="cpp", conf="low",
                   why="Batman nuwin32 Stubs.cpp (NuScratch*, NuPs2*); start between 0x6de6d0 and 0x6de8c0"),
    0x6df010: dict(name="d3dApiCalls", area="nu2api/nuwin32", lang="cpp", conf="high",
                   why="marker; retail .\\d3dApiCalls.cpp (Mac nuwin32/d3dApiCalls.cpp)"),
    0x6e1820: dict(name="d3dCore", area="nu2api/nuwin32", lang="cpp", conf="high",
                   why="marker; retail .\\d3dCore.cpp from PC_DEVICE_BACKEND_PC::CaptureScreenshotPc"),
    0x6e4ff0: dict(name="d3dCalls", area="nu2api/nuwin32", lang="cpp", conf="medium",
                   why="retail .\\d3dCalls.cpp at 0x6e58d0/0x6e5ab0; start from the Android split (no marker)"),
    0x6dee30: dict(name=None, why="one Batman nucore vote (numc); separate from nucore_includes, which ends at 0x4fe640"),
    0x740f00: dict(name="VirtualStackAllocator", area="gamelib/util", lang="cpp", conf="medium",
                   why="C++ class after the CRT; Batman includes VirtualStackAllocator_gen.cpp in nucore_includes"),
    0x740f60: dict(name="VirtualStackAllocator", join=True, why="continues 0x740f00"),
}

# ------------------------------------------------------------------ inputs


def read_csv(path):
    with open(path, newline="") as stream:
        return list(csv.DictReader(stream))


def load_functions(symbols, matching, text_start, text_end):
    """Functions in .text, one per address, with our source files and status."""
    scores = {}
    if matching and Path(matching).is_file():
        for f in json.loads(Path(matching).read_text())["functions"]:
            scores[int(f["address"], 16)] = f
    out, seen = [], set()
    for row in read_csv(symbols):
        if not row["size"] or not row["address"]:
            continue
        va = int(row["address"], 16)
        if not text_start <= va < text_end or va in seen:
            continue
        seen.add(va)
        score = scores.get(va, {})
        units = [u.removeprefix("src/") for u in score.get("units") or []]
        out.append({"va": va, "size": int(row["size"]), "symbol": row["symbol"], "ours": units,
                    "status": score.get("status") or "", "library": bool(units) and all(
                        u.startswith("lib/") for u in units), "funclet": is_funclet(row["symbol"])})
    out.sort(key=lambda f: f["va"])
    # Import thunks (`jmp [iat]`) and unmapped pieces after library code are library code too.
    previous = None
    for f in out:
        if f["size"] == 6 and re.fullmatch(r"_\w+@\d+", f["symbol"]) and not f["ours"]:
            f["library"] = True
        elif not f["ours"] and previous is not None and previous["library"]:
            f["library"] = True
        previous = f
    return out


def is_funclet(symbol):
    """EH handlers and dynamic initializers/atexit thunks: the linker groups them
    by section name (.text$x, .text$di/$yd) after all ordinary code."""
    return symbol.startswith(("__ehhandler$", "??__E", "??__F", "__unwindfunclet$"))


def name_key(symbol):
    """Plain function name of a linker symbol (class::method for members)."""
    m = re.match(r"\?(\w+)@@[YS]", symbol)
    if m:
        return m.group(1)
    m = re.match(r"\?(\w+)@(\w+)@@", symbol)
    if m:
        return f"{m.group(2)}::{m.group(1)}"
    name = symbol[1:] if symbol[:1] in "_@" else symbol
    return name.split("@")[0]


def batman_key(mangled, demangled):
    if not mangled.startswith("__Z"):
        return mangled[1:] if mangled.startswith("_") else mangled
    base = demangled.split("(")[0]
    parts = base.split("::")
    return "::".join(parts[-2:]) if len(parts) > 1 else base


def short_source(path):
    """'legogame/legogame/../levels.cpp' -> ('legogame', 'levels', '.cpp')."""
    path = os.path.normpath(path.replace("/Volumes/Development/Projects/Songbird/Development/", ""))
    parts = path.split("/")
    stem, ext = os.path.splitext(parts[-1])
    return parts[0], stem, ext


def load_batman(directory):
    """name -> set of (top dir, stem, ext, inner file) from the private Batman map."""
    directory = Path(directory)
    table = directory / "debugmap.tsv.gz"
    if not table.is_file():
        return None
    inner = load_batman_inner(directory / "LEGO Batman")
    out = collections.defaultdict(set)
    with gzip.open(table, "rt") as stream:
        next(stream)
        for line in stream:
            addr, kind, mangled, demangled, source, _ = line.rstrip("\n").split("\t")
            if kind != "func":
                continue
            top, stem, ext = short_source(source)
            out[batman_key(mangled, demangled)].add((top, stem, ext, inner.get(mangled, "")))
    return out


def load_batman_inner(binary):
    """Mangled name -> file #included into a unity build (from STABS SOL entries).

    A SOL inside a function's BNSYM..ENSYM switches the current file; it applies
    to that function and the ones after it. GCC also emits deferred inline
    functions far from their file, so callers smooth the result per block."""
    nm = shutil.which("llvm-nm")
    if not binary.is_file() or not nm:
        return {}
    lines = subprocess.run([nm, "-ap", str(binary)], capture_output=True, text=True).stdout.splitlines()
    out, current, function, first = {}, "", None, None
    for line in lines:
        p = line.split()
        if len(p) < 5 or p[1] != "-":
            continue
        kind, name = p[4], " ".join(p[5:])
        if kind == "SO" and name and not name.endswith("/"):
            current = os.path.basename(name)
        elif kind == "FUN" and name:
            function, first = name, None
        elif kind == "SOL" and name.endswith((".c", ".cpp")):
            if function and first is None:
                first = os.path.basename(name)
            current = os.path.basename(name)
        elif kind == "ENSYM" and function:
            out[function] = first or current
            function = None
    return out


FILE_PATH = re.compile(rb"(?:[a-z]:\\|\.\\)[\x20-\x7e]*?\.(?:c|cpp)\x00", re.I)


def retail_files(exe, functions):
    """Function VA -> __FILE__ paths referenced from it (absolute or .\\ relative)."""
    from tools.binary import PE
    pe = PE(exe)
    text = next(s for s in pe.sections if s["name"] == ".text")
    data = pe.data[text["raw_offset"]:text["raw_offset"] + text["raw_size"]]
    base = pe.image_base + text["virtual_address"]
    starts = [f["va"] for f in functions]
    out = collections.defaultdict(set)
    for section in pe.sections:
        raw = pe.data[section["raw_offset"]:section["raw_offset"] + section["raw_size"]]
        for m in FILE_PATH.finditer(raw):
            va = pe.image_base + section["virtual_address"] + m.start()
            path = m.group()[:-1].decode()
            for ref in re.finditer(re.escape(va.to_bytes(4, "little")), data):
                i = bisect.bisect_right(starts, base + ref.start()) - 1
                if i >= 0 and base + ref.start() < starts[i] + functions[i]["size"]:
                    out[functions[i]["va"]].add(path)
    return out


def unit_markers(functions, units, use_exe=True):
    """Unit-start marker VAs: recomputed from the EXE when present, else units.csv."""
    try:
        if not use_exe:
            raise OSError("retail EXE not used")
        from tools import unitmap
        from tools.lib import number, project, retail
        target = project()["target"]
        pe = retail(target)
        base, size = number(target["text_va"]), number(target["text_size"])
        text = pe.read_va(base, size)
        pieces = unitmap.text_pieces(text, base, [(f["va"], f["size"], f["symbol"]) for f in functions])
        clusters = unitmap.marker_clusters(text, base, pieces, unitmap.helper_patterns(text, base, pieces))
        return dict(clusters), True
    except (OSError, ValueError, ImportError):
        return {int(u["start"], 16): u["marker"] for u in units if u["evidence"] == "marker"}, False


# ------------------------------------------------------------------ segmentation


def area_of(va):
    for start, end, area, _ in AREAS:
        if start <= va < end:
            return area
    return "nu2api"


def label_of(f, batman):
    """Batman evidence of one function: (stem, ext, inner) or None when absent/ambiguous."""
    if batman is None:
        return None
    hits = batman.get(name_key(f["symbol"]))
    if not hits or len({(t, s) for t, s, _, _ in hits}) != 1:
        return None
    top, stem, ext, inner = next(iter(hits))
    if stem.startswith("game-"):
        stem = "game"
    stem = RENAMES.get(stem, stem)
    return stem, ext, inner


def smooth(labels, small=3):
    """Relabel short runs that sit between two runs of the same label (functions
    Batman moved to another file). `labels` holds None for unknown functions."""
    labels = list(labels)
    changed = True
    while changed:
        changed = False
        runs = []
        for i, lab in enumerate(labels):
            if lab is None:
                continue
            if runs and runs[-1][0] == lab:
                runs[-1][2].append(i)
            else:
                runs.append([lab, i, [i]])
        for k in range(1, len(runs) - 1):
            lab, _, idx = runs[k]
            left, right = runs[k - 1], runs[k + 1]
            if left[0] == right[0] != lab and len(idx) <= small and len(idx) < len(left[2]) + len(right[2]):
                for i in idx:
                    labels[i] = left[0]
                changed = True
                break
    return labels


class Atom:
    def __init__(self, start, end, functions, unit, marker):
        self.start, self.end, self.functions, self.unit, self.marker = start, end, functions, unit, marker
        self.label = None
        self.evidence = ""
        self.join = False
        self.review = None


def atoms_of(functions, units, markers):
    """Cut .text at units.csv rows and unit-start markers."""
    starts = sorted({int(u["start"], 16) for u in units} | set(markers) | set(REVIEW))
    names = {int(u["start"], 16): u["unit"].removeprefix("src/") for u in units}
    out = []
    vas = [f["va"] for f in functions]
    for i, start in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else (functions[-1]["va"] + functions[-1]["size"])
        lo, hi = bisect.bisect_left(vas, start), bisect.bisect_left(vas, end)
        unit = names.get(start) or (out[-1].unit if out else "")
        # Library objects (CRT, DirectX SDK) and the funclet tail cut a row into pieces.
        pieces, current = [], []
        for f in functions[lo:hi]:
            if f["library"] or f["funclet"]:
                if current:
                    pieces.append(current)
                current = []
            else:
                current.append(f)
        if current:
            pieces.append(current)
        for k, fs in enumerate(pieces):
            leading = functions[lo:hi] and (functions[lo]["library"] or functions[lo]["funclet"])
            first = start if k == 0 and not leading else fs[0]["va"]
            stop = pieces[k + 1][0]["va"] if k + 1 < len(pieces) else end
            out.append(Atom(first, stop, fs, unit, markers.get(start, "") if first == start else ""))
    return out


def split_blocks(atom, labels):
    """Split an atom whose Batman labels form clean consecutive blocks of >= 3."""
    runs = []
    for f, lab in zip(atom.functions, labels):
        if lab is None:
            continue
        if runs and runs[-1][0] == lab:
            runs[-1][1].append(f)
        else:
            runs.append([lab, [f]])
    stems = [r[0] for r in runs]
    if len(runs) < 2 or len(set(stems)) != len(stems) or any(len(r[1]) < 3 for r in runs):
        return [atom]
    out = []
    for k, (lab, fs) in enumerate(runs):
        start = atom.start if k == 0 else fs[0]["va"]
        end = runs[k + 1][1][0]["va"] if k + 1 < len(runs) else atom.end
        part = Atom(start, end, [f for f in atom.functions if start <= f["va"] < end], atom.unit,
                    atom.marker if k == 0 else "")
        out.append(part)
    return out


def segment(functions, units, markers, batman):
    labels = {f["va"]: label_of(f, batman) for f in functions}
    order = [f["va"] for f in functions if not f["library"]]
    smoothed = dict(zip(order, smooth([labels[va][0] if labels[va] else None for va in order])))
    atoms = []
    for atom in atoms_of(functions, units, markers):
        atoms.extend(split_blocks(atom, [smoothed[f["va"]] for f in atom.functions]))
    for atom in atoms:
        votes = collections.Counter(smoothed[f["va"]] for f in atom.functions if smoothed[f["va"]])
        if votes:
            stem, n = votes.most_common(1)[0]
            if n * 2 >= sum(votes.values()):
                atom.label, atom.evidence = stem, "batman"
        review = REVIEW.get(atom.start)
        if review:
            atom.review = review
            atom.join = review.get("join", False)
            if review.get("name") or "name" in review:
                atom.label = review.get("name")
                atom.evidence = "review"
    # Unlabelled atoms without a marker between two atoms of the same label belong to them.
    # Only directly adjacent atoms count: library code or a marker in between ends the run.
    library = sorted(f["va"] for f in functions if f["library"] or f["funclet"])

    def contiguous(a, b):
        k = bisect.bisect_left(library, a.end)
        return k >= len(library) or library[k] >= b.start

    for i, atom in enumerate(atoms):
        if atom.label or atom.marker or (atom.review and not atom.join) or i == 0:
            continue
        prev = atoms[i - 1]
        j = i + 1
        while j < len(atoms) and not atoms[j].label and not atoms[j].marker:
            j += 1
        nxt = atoms[j] if j < len(atoms) else None
        if (prev.label and nxt and prev.label == nxt.label and not nxt.marker
                and contiguous(prev, atom) and contiguous(atom, nxt)):
            atom.label, atom.join = prev.label, True
    for i, atom in enumerate(atoms):
        if atom.join and atom.label is None and i:
            atom.label = atoms[i - 1].label
            atom.unknown_join = True
    return atoms, labels


# ------------------------------------------------------------------ compilands

CXX_MEMBER = re.compile(r"^\?\?[0-9_]|^\?\w+@\w+@@[QAIEMUS]")


def language(fs, ext_votes, retail, unity):
    """(language, reason) for a compiland."""
    members = sum(1 for f in fs if CXX_MEMBER.match(f["symbol"]))
    if retail:
        exts = {os.path.splitext(p)[1].lower() for p in retail}
        if not unity and exts == {".c"}:
            return "c", "retail __FILE__ .c"
        if exts == {".cpp"}:
            return "cpp", "retail __FILE__ .cpp"
    if members >= 2:
        return "cpp", f"{members} C++ member/special functions"
    if unity:
        return "cpp?", "unity file, Batman .cpp"
    if ext_votes.get(".c", 0) > ext_votes.get(".cpp", 0):
        return "c", "Batman .c"
    if members:
        return "cpp?", "1 C++ member function (may be a COMDAT inline from elsewhere)"
    return "c?", "no C++ construct; TCS game/legoapi code assumed C (Batman converted to .cpp)"


def build(functions, units, markers, batman, retail):
    atoms, labels = segment(functions, units, markers, batman)
    groups = []
    for atom in atoms:
        if groups and atom.join and groups[-1][-1].label == atom.label:
            groups[-1].append(atom)
        elif groups and atom.label and groups[-1][-1].label == atom.label and not atom.marker \
                and not (atom.review and not atom.join):
            groups[-1].append(atom)
        elif groups and atom.label and groups[-1][-1].label == atom.label and atom.evidence == "batman" \
                and groups[-1][-1].evidence == "batman":
            # Same Batman file across a marker: TCS split what Batman merged, or a stray marker.
            atom.marker_inside = True
            groups[-1].append(atom)
        else:
            groups.append([atom])
    seen = collections.Counter((area_of(g[0].start), g[0].label) for g in groups if g[0].label)
    comps = []
    for g in groups:
        fs = [f for a in g for f in a.functions]
        head = g[0]
        name = head.label
        area = (head.review or {}).get("area") or area_of(head.start)
        files = set()
        for f in fs:
            files |= retail.get(f["va"], set())
        ext_votes = collections.Counter(labels[f["va"]][1] for f in fs if labels[f["va"]])
        unity = name in UNITY
        lang, lang_why = language(fs, ext_votes, files, unity)
        review = head.review or {}
        if review.get("lang"):
            lang, lang_why = review["lang"], "review: " + review.get("why", "")
        evidence = sorted({a.evidence for a in g if a.evidence} | ({"marker"} if head.marker else set())
                          | ({"file"} if files else set()))
        if not name:
            conf, evidence = "none", evidence + ["android"]
        elif review.get("conf"):
            conf = review["conf"]
        elif head.marker and head.evidence == "batman":
            conf = "high"
        elif head.evidence == "batman":
            conf = "medium"
        else:
            conf = "low"
        notes = []
        if name and seen[(area, name)] > 1:
            notes.append(f"{area}/{name} used by {seen[(area, name)]} ranges")
            conf = "low" if conf != "none" else conf
        if any(getattr(a, "marker_inside", False) for a in g):
            notes.append("unit-start marker inside: TCS may have two files here")
        if review.get("why"):
            notes.append(review["why"])
        ours = collections.Counter()
        for f in fs:
            if f["ours"]:
                ours[f["ours"][0]] += f["size"]
        comps.append({
            "start": head.start, "end": max(f["va"] + f["size"] for f in fs), "name": name, "area": area,
            "lang": lang, "lang_why": lang_why, "unity": unity, "files": sorted(files), "evidence": evidence,
            "confidence": conf, "functions": fs, "units": sorted({a.unit for a in g}, key=lambda u: u),
            "ours": ours, "notes": notes, "labels": labels})
    return comps


def target_path(comp):
    if not comp["name"]:
        return ""
    ext = ".c" if comp["lang"].startswith("c") and not comp["lang"].startswith("cpp") else ".cpp"
    return f"{comp['area']}/{comp['name']}{ext}"


def file_base(name):
    """'nufile_gen.cpp' and 'nufile.c' -> 'nufile' (Batman added _gen to many engine files)."""
    stem = Path(name).stem.lower()
    return stem[:-4] if stem.endswith("_gen") else stem


def inner_rows(comp, retail):
    """Blocks of one unity compiland by included file.

    A retail __FILE__ names the file of the function that references it; else the
    Batman SOL file is used, renamed to the retail spelling when one with the same
    base name occurs in this compiland. An included file is one contiguous range,
    so a name that ends up in several blocks keeps only its largest block (or its
    retail-proven ones); the rest become undetermined ('?')."""
    fs_all = comp["functions"]
    proven = {}
    for f in fs_all:
        for path in retail.get(f["va"], ()):
            name = Path(path.replace("\\", "/")).name
            proven[file_base(name)] = name
    own = {file_base(comp["name"] + ext) for ext in (".c", ".cpp")}
    labels = []
    for f in fs_all:
        files = retail.get(f["va"])
        if files and len(files) == 1:
            labels.append(("file", Path(next(iter(files)).replace("\\", "/")).name))
            continue
        lab = comp["labels"][f["va"]]
        if lab and lab[2] and file_base(lab[2]) not in own:
            labels.append(("batman", proven.get(file_base(lab[2]), lab[2])))
        else:
            labels.append(None)
    smoothed = smooth([lab[1] if lab else None for lab in labels], small=2)
    # Retail-proven labels are facts, never smoothed away.
    smoothed = [lab[1] if lab and lab[0] == "file" else s for lab, s in zip(labels, smoothed)]
    blocks = []
    for f, lab, kind in zip(fs_all, smoothed, labels):
        if lab is None and blocks:
            blocks[-1][1].append(f)
            continue
        if blocks and blocks[-1][0] == lab:
            blocks[-1][1].append(f)
        else:
            blocks.append([lab, [f], set()])
        if kind and kind[0] == "file":
            blocks[-1][2].add("file")
    # One range per included file.
    by_name = collections.defaultdict(list)
    for k, (lab, fs, kinds) in enumerate(blocks):
        if lab:
            by_name[lab].append(k)
    for lab, ks in by_name.items():
        if len(ks) < 2:
            continue
        keep = [k for k in ks if "file" in blocks[k][2]]
        if len(keep) > 1:
            for k in keep:
                blocks[k][2].add("conflict")
        else:
            keep = keep or [max(ks, key=lambda k: len(blocks[k][1]))]
        for k in ks:
            if k not in keep:
                blocks[k][0] = "?"
    merged = []
    for lab, fs, kinds in blocks:
        if merged and (merged[-1][0] == lab or (len(fs) < 2 and not kinds and lab != "?")):
            merged[-1][1].extend(fs)
            merged[-1][2] |= kinds
        else:
            merged.append([lab, fs, kinds])
    return merged


def rows(comps, retail):
    out = []
    for c in comps:
        path = target_path(c)
        base = {"compiland": path, "language": c["lang"], "current_unit_name": " ".join(c["units"]),
                "note": "; ".join(c["notes"] + [f"language: {c['lang_why']}"])}
        blocks = inner_rows(c, retail) if c["unity"] and c["name"] else [[None, c["functions"], set()]]
        for lab, fs, kinds in blocks:
            ours = collections.Counter()
            for f in fs:
                if f["ours"]:
                    ours[f["ours"][0]] += f["size"]
            evidence = list(c["evidence"])
            conf = c["confidence"]
            if lab is not None and c["unity"]:
                if lab == "?":
                    evidence, conf = ["none"], "none"
                elif "conflict" in kinds:
                    evidence, conf = ["file-conflict"], "low"
                else:
                    evidence = ["file" if "file" in kinds else "batman-sol"]
                    conf = "high" if "file" in kinds else "low"
            out.append({**base, "start": f"0x{fs[0]['va']:06x}" if lab is not None or len(blocks) > 1
                        else f"0x{c['start']:06x}",
                        "end": f"0x{max(f['va'] + f['size'] for f in fs):06x}",
                        "inner_file": lab or "", "evidence": "+".join(evidence), "confidence": conf,
                        "functions": len(fs), "bytes": sum(f["size"] for f in fs),
                        "current_source_files": " ".join(f"{k}:{v}" for k, v in ours.most_common())})
    return out


# ------------------------------------------------------------------ numbers


def carrier(comp):
    """The current source file that should become the compiland (rename at most)."""
    if not comp["ours"]:
        return None
    stem = comp["name"] or ""
    same = [o for o in comp["ours"] if Path(o).stem.lower().rstrip("s") == stem.lower().rstrip("s")]
    return max(same or comp["ours"], key=lambda o: comp["ours"][o])


def same_stem(a, b):
    a, b = a.lower(), b.lower()
    a, b = RENAMES.get(a, a), RENAMES.get(b, b)
    return a.rstrip("s") == b.rstrip("s")


def units_status(comps, units):
    """Classify each config/units.csv row against the target compilands:
    ok, name (wrong file name), boundary (right name, wrong start or end),
    inside (the row is part of a unity or larger compiland of another name),
    undetermined (the target compiland has no name yet)."""
    starts = sorted(c["start"] for c in comps)
    by_start = {c["start"]: c for c in comps}
    out = {}
    for u in units:
        s, e = int(u["start"], 16), int(u["end"], 16)
        i = bisect.bisect_right(starts, s) - 1
        comp = by_start[starts[i]] if i >= 0 else None
        if comp is None or s >= comp["end"]:
            out[u["start"]] = "library"
        elif not comp["name"]:
            out[u["start"]] = "undetermined"
        elif not same_stem(Path(u["unit"]).stem, comp["name"]):
            out[u["start"]] = "inside" if comp["start"] != s else "name"
        elif comp["start"] != s or comp["end"] > e + 16:
            out[u["start"]] = "boundary"
        else:
            out[u["start"]] = "ok"
    return out


def plan(comps):
    """Migration actions per current source file, keyed by batch:
    rename (whole file becomes one target), merge (whole file folds into another
    file's target), split (functions go to several targets)."""
    targets = collections.defaultdict(collections.Counter)
    carriers = {}
    for c in comps:
        path = target_path(c) or f"?0x{c['start']:06x}"
        keep = carrier(c)
        if keep:
            carriers.setdefault(keep, set()).add(path)
        for f in c["functions"]:
            if f["ours"] and not f["ours"][0].startswith("lib/"):
                targets[f["ours"][0]][path] += 1
    out = {"exact": [], "rename": [], "merge": [], "split": []}
    for path, ts in sorted(targets.items()):
        if len(ts) > 1:
            out["split"].append((path, dict(ts.most_common())))
        elif path in carriers and set(ts) <= carriers[path]:
            out["exact" if path in ts else "rename"].append((path, next(iter(ts))))
        else:
            out["merge"].append((path, next(iter(ts))))
    return out


def summary(comps, units):
    total = placed = renamed = moved = und = 0
    fb = collections.Counter()
    for c in comps:
        keep = carrier(c)
        path = target_path(c)
        for f in c["functions"]:
            if not f["ours"]:
                continue
            total += 1
            fb["total"] += f["size"]
            if not c["name"]:
                und += 1
                fb["undetermined"] += f["size"]
            elif f["ours"][0] == path:
                placed += 1
                fb["placed"] += f["size"]
            elif f["ours"][0] == keep:
                renamed += 1
                fb["renamed"] += f["size"]
            else:
                moved += 1
                fb["moved"] += f["size"]
    named = [c for c in comps if c["name"]]
    # Per current source file: how many target compilands its functions go to.
    targets = collections.defaultdict(set)
    carriers = {}
    for c in comps:
        keep = carrier(c)
        if keep:
            carriers.setdefault(keep, set()).add(target_path(c) or f"?{c['start']:x}")
        for f in c["functions"]:
            if f["ours"] and not f["ours"][0].startswith("lib/"):
                targets[f["ours"][0]].add(target_path(c) or f"?{c['start']:x}")
    files = collections.Counter()
    for path, ts in targets.items():
        if len(ts) > 1:
            files["split"] += 1
        elif path in carriers and ts <= carriers[path]:
            files["exact" if path in ts else "rename"] += 1
        else:
            files["merge"] += 1
    rows_status = units_status(comps, units)
    return {
        "compilands": len(comps), "named": len(named), "undetermined": len(comps) - len(named),
        "unity": sum(1 for c in named if c["unity"]),
        "by_confidence": dict(collections.Counter(c["confidence"] for c in comps)),
        "by_language": dict(collections.Counter(c["lang"] for c in named)),
        "functions": total, "in_target_file": placed, "in_carrier_needs_rename": renamed,
        "need_move": moved, "in_undetermined": und,
        "bytes": dict(fb),
        "units_rows": len(units), "units_rows_by_status": dict(collections.Counter(rows_status.values())),
        "current_files": len(targets), "current_files_by_action": dict(files),
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--batman", help="directory with the private LEGO Batman Mac debug map")
    parser.add_argument("--symbols", default=str(ROOT / "config/symbols.csv"))
    parser.add_argument("--units", default=str(ROOT / "config/units.csv"))
    parser.add_argument("--matching", default=str(ROOT / "matching.json"))
    parser.add_argument("--exe", default=str(ROOT / "res/orig/LEGOStarWarsSaga.exe"))
    parser.add_argument("--write", action="store_true", help=f"write {TABLE.relative_to(ROOT)}")
    parser.add_argument("--show", help="print one compiland (address inside it) with its functions")
    parser.add_argument("--json", action="store_true", help="print the summary as JSON")
    parser.add_argument("--plan", action="store_true", help="print the per-file migration actions")
    args = parser.parse_args(argv)

    from tools.lib import number, project
    target = project()["target"]
    text_start = number(target["text_va"])
    text_end = text_start + number(target["text_size"])
    functions = load_functions(args.symbols, args.matching, text_start, text_end)
    units = read_csv(args.units)
    markers, from_exe = unit_markers(functions, units, Path(args.exe).is_file())
    batman = load_batman(args.batman) if args.batman else None
    retail = retail_files(args.exe, functions) if Path(args.exe).is_file() else {}
    comps = build(functions, units, markers, batman, retail)
    if args.show:
        va = int(args.show, 16)
        for c in comps:
            if c["start"] <= va < c["end"]:
                print(f"{target_path(c) or '?'} 0x{c['start']:06x}-0x{c['end']:06x} {c['confidence']} "
                      f"{c['lang']} ({c['lang_why']}) {'; '.join(c['notes'])}")
                for f in c["functions"]:
                    lab = c["labels"][f["va"]]
                    print(f"  0x{f['va']:06x} {f['size']:6} {f['symbol'][:56]:56} "
                          f"{(lab[0] + lab[1] + ' ' + lab[2]) if lab else '':40} {' '.join(f['ours'][:1])}")
        return 0
    if args.plan:
        for action, items in plan(comps).items():
            print(f"## {action} ({len(items)})")
            for path, where in items:
                if isinstance(where, dict):
                    where = " ".join(f"{k}:{v}" for k, v in where.items())
                print(f"  src/{path} -> {where}")
        return 0
    table = rows(comps, retail)
    tail = [f for f in functions if f["funclet"]]
    if tail:
        ours = collections.Counter(f["ours"][0] for f in tail if f["ours"])
        table.append({
            "start": f"0x{tail[0]['va']:06x}", "end": f"0x{tail[-1]['va'] + tail[-1]['size']:06x}",
            "compiland": "", "inner_file": "", "language": "cpp", "evidence": "section",
            "confidence": "high", "functions": len(tail), "bytes": sum(f["size"] for f in tail),
            "current_unit_name": "", "current_source_files": " ".join(f"{k}:{v}" for k, v in ours.most_common()),
            "note": "EH handlers (.text$x) and dynamic initializers/atexit thunks (.text$di/$yd), grouped by "
                    "section after all code; each belongs to the C++ compiland that defines the object, in "
                    "link order, so their order is a second link-order witness"})
    stats = summary(comps, units)
    stats["inputs"] = {"markers_from_exe": from_exe, "batman": batman is not None, "retail_files": bool(retail)}
    if args.write:
        with TABLE.open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=COLUMNS, lineterminator="\n")
            writer.writeheader()
            writer.writerows(table)
        print(f"wrote {len(table)} rows to {TABLE.relative_to(ROOT)}")
    print(json.dumps(stats, indent=None if not args.json else 2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
