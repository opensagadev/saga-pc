#!/usr/bin/env python3
"""Name or bound every unmapped function that tools/unmapped.py decodes, with its evidence.

tools/unmapped.py decodes the extents of the unmapped .text (--functions). This
tool gives each of those functions a map row and records why, in
config/code-evidence.csv, one evidence class per row (strongest first):

  copy      a byte-identical copy (near-call displacements masked) of a known
            header-static helper (HELPERS); named after the helper with the
            owning compiland's prefix, the way the earlier copies are named
            (`_AttractoFabsPc`, `?RtlVuVecCopy@@YAXAAUVuVec@@ABU1@@Z`)
  compiler  a dynamic initializer in the .CRT$XC table (`??__E<object>@@YAXXZ`)
            or the atexit destructor it registers (`??__F<object>@@YAXXZ`),
            named after the global it constructs or destroys
  sibling   a game function the sibling build (LEGO Batman Mac, symbols kept)
            places between the same two named neighbours of the same source
            file, with at least one agreeing content signal (named callees,
            float constants, named globals); `--reference DIR`
  extent    bounded, no name proven: the standard address name `_fun_XXXXXXXX`
            (lowercase hex of the retail address; config/symbols.md)

Library code is named by tools/libmatch.py (config/library-objects.json) and is
not repeated here. A name is written only where the class proves it; where a
class proposes a name it cannot spell as a linker name (a sibling function in a
C++ compiland: the MSVC decoration needs the prototype), the row keeps the
address name and the proposal goes in the `proposed` column.

    python3 tools/codemap.py                       # rows per evidence class (dry run)
    python3 tools/codemap.py --write               # add them to symbols.csv and the ledger
    python3 tools/codemap.py --write --only copy   # one evidence class at a time
    python3 tools/codemap.py --refresh             # recompute compiland and port columns
    python3 tools/codemap.py --check               # every ledger row still holds (retail)
    python3 tools/codemap.py --worklist            # newly bounded functions per compiland (markdown)

The ledger only grows: rows written once keep their evidence, so a run without
the private sibling build never drops a sibling name. --check re-derives what
the retail bytes alone prove (extent, copy bytes, initializer shape, address
names) and fails on any contradiction.
"""
import argparse
import bisect
import collections
import csv
import io
import json
from pathlib import Path
import re
import struct
import sys

if __package__:
    from . import unmapped
    from .lib import ROOT, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import unmapped
    from tools.lib import ROOT, load_symbols, project, retail

LEDGER = ROOT / "config/code-evidence.csv"
LAYOUT = ROOT / "config/layout-target.csv"
FIELDS = ("address", "size", "symbol", "evidence", "proposed", "compiland", "signals", "port")
EVIDENCE = ("copy", "compiler", "sibling", "extent")
ADDRESS_NAME = re.compile(r"_?fun_[0-9a-f]{8}")
DEFAULT_REFERENCE = ROOT / "build/reference/batman-mac"
ANDROID = ROOT / "build/reference/saga-android"
CALL = re.compile(rb"\xe8[\x00-\xff]{4}", re.S)

# Header-static helpers that VC8 emits once per compiland that includes their
# header. Each is identified by an existing named copy (the reference row); its
# stem names the new copies, its MSVC signature decorates them in C++ compilands
# (None: the header declares it extern "C").
HELPERS = (
    # (stem, reference row, C++ signature after the name)
    ("VuVecMake", "?VuVecMake@@YA?AUVuVec@@MMMM@Z", "@@YA?AUVuVec@@MMMM@Z"),
    ("Fabs", "_AttractoFabsPc", "@@YAMM@Z"),
    ("VuVecCopy", "?VuVecCopy@@YAXAAUVuVec@@ABU1@@Z", "@@YAXAAUVuVec@@ABU1@@Z"),
    ("Fdiv", "?NuFdiv@@YAMMM@Z", "@@YAMMM@Z"),
    ("ACos", "_NuACos", "@@YAMM@Z"),
    ("VuVecAdd", "?RtlVuVecAdd@@YA?AUVuVec@@ABU1@0@Z", "@@YA?AUVuVec@@ABU1@0@Z"),
    ("VuVecSub", "?RtlVuVecSub@@YA?AUVuVec@@ABU1@0@Z", "@@YA?AUVuVec@@ABU1@0@Z"),
    ("VuVecScale", "?RtlVuVecScale@@YA?AUVuVec@@ABU1@M@Z", "@@YA?AUVuVec@@ABU1@M@Z"),
    ("VuVecNormalise", "?VuVecNormalise@@YAXAAUVuVec@@0@Z", "@@YAXAAUVuVec@@0@Z"),
    ("VuVecLengthSq", "?VuVecLengthSq@@YAMABUVuVec@@@Z", "@@YAMABUVuVec@@@Z"),
    ("VuVecLength", "?VuVecLength@@YAMABUVuVec@@@Z", "@@YAMABUVuVec@@@Z"),
    ("VuVecDot", "?VuVecDot@@YAMABUVuVec@@0@Z", "@@YAMABUVuVec@@0@Z"),
    ("VuVecSetOrigin", "?VuVecSetOrigin@@YAXAAUVuVec@@@Z", "@@YAXAAUVuVec@@@Z"),
    ("VuAngleDifference", "?VuAngleDifference@@YAHHH@Z", "@@YAHHH@Z"),
    # vorbisfile.h (extern "C", so C names in C++ compilands too): the seek
    # callback of its static OV_CALLBACKS tables.
    ("OvHeaderFseekWrap", "__ov_header_fseek_wrap", None),
)


def masked(data):
    return CALL.sub(b"\xe8\0\0\0\0", data)


def address_name(va):
    return f"_fun_{va:08x}"


# ---------------------------------------------------------------- compilands

def load_layout(path=LAYOUT):
    """[(start, end, compiland, language)] sorted, from config/layout-target.csv."""
    if not Path(path).is_file():
        return []
    with open(path, newline="") as stream:
        rows = [(int(r["start"], 16), int(r["end"], 16), r["compiland"], r["language"]) for r in csv.DictReader(stream)]
    return sorted(rows)


def compiland_of(layout, va, leading=False):
    """(compiland, language) holding va. Between two rows a function belongs to
    the previous compiland (unreferenced code after its last mapped function),
    unless `leading` (a header-static copy, emitted at a compiland's start)."""
    starts = [a for a, _, _, _ in layout]
    i = bisect.bisect_right(starts, va) - 1
    if i >= 0 and va < layout[i][1]:
        return layout[i][2], layout[i][3]
    if leading and i + 1 < len(layout):
        return layout[i + 1][2], layout[i + 1][3]
    if i >= 0:
        return layout[i][2], layout[i][3]
    return "", ""


def prefix(compiland, qualified=False):
    """`legogame/attracto.c` -> `Attracto` (the earlier copies' naming); qualified:
    `LegogameAttracto`, for two compilands of one file name in different directories."""
    stem = Path(compiland).stem if not qualified else "_".join(Path(compiland).with_suffix("").parts[-2:])
    parts = [p for p in re.split(r"[^A-Za-z0-9]+", stem) if p]
    return "".join(p[:1].upper() + p[1:] for p in parts)


def is_cxx(language):
    return language.startswith("cpp")


# ---------------------------------------------------------------- evidence classes

def helper_bytes(records, read):
    """{masked bytes: (stem, signature)} of HELPERS, read at their reference rows."""
    at = {r["symbol"]: r for r in records if r["address"] and r["size"]}
    result = {}
    for stem, reference, signature in HELPERS:
        row = at.get(reference)
        if row is not None:
            result[masked(read(row["address"], row["size"]))] = (stem, signature, reference)
    return result


def copy_name(stem, signature, compiland, language, qualified=False):
    if not compiland:
        return None
    if is_cxx(language) and signature:
        return f"?{prefix(compiland, qualified)}{stem}{signature}"
    return f"_{prefix(compiland, qualified)}{stem}Pc"


def crt_initializers(ctx):
    """{stub va: table slot va} for the .CRT$XC table entries."""
    at = {r["symbol"]: r["address"] for r in ctx.records if r["address"] is not None}
    if "___xc_a" not in at or "___xc_z" not in at:
        return {}
    result = {}
    for slot in range(at["___xc_a"], at["___xc_z"], 4):
        value = struct.unpack("<I", ctx.read(slot, 4))[0]
        if value:
            result[value] = slot
    return result


def source_name(symbol):
    """Source name of a global for ??__E/??__F: `_Foo` -> Foo, `?foo@@3VBar@@A` -> foo.

    Static class members and names in a namespace are not spelled here (None)."""
    m = re.fullmatch(r"\?(\w+)@@3.*", symbol)
    if m:
        return m.group(1)
    if re.fullmatch(r"_[A-Za-z]\w*", symbol):
        return symbol[1:]
    return None


def initializer_shape(flow, start, end):
    """(object address or None, atexit destructor or None) of an initializer stub.

    The object is the `this` of a constructor or destructor call: `mov ecx,
    imm32` directly followed by a call or jmp. The destructor is the immediate
    pushed directly before a call (`push dtor; call _atexit`). Stubs that only
    copy other globals or touch several do not name an object."""
    obj, destructor, previous = None, None, None
    va = start
    x86 = flow.x86
    while va < end:
        offset = va - flow.ctx.text_start
        ins = next(flow.md.disasm(flow.ctx.text[offset:offset + 16], va, 1), None)
        if ins is None:
            break
        ops = ins.operands
        if ins.mnemonic in ("call", "jmp") and previous is not None:
            kind, value = previous
            if kind == "this" and obj is None:
                obj = value
            elif kind == "push" and ins.mnemonic == "call":
                destructor = value
        previous = None
        if ins.mnemonic == "push" and ops and ops[0].type == x86.X86_OP_IMM:
            previous = ("push", ops[0].imm & 0xffffffff)
        elif ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == x86.X86_OP_REG \
                and ins.reg_name(ops[0].reg) == "ecx" and ops[1].type == x86.X86_OP_IMM:
            previous = ("this", ops[1].imm & 0xffffffff)
        va += ins.size
        if ins.mnemonic in ("ret", "jmp"):
            break
    return obj, destructor


def this_call(flow, start, end):
    """(object, target) of the first `mov ecx, imm32` directly followed by a direct call or jmp."""
    va, previous = start, None
    x86 = flow.x86
    while va < end:
        offset = va - flow.ctx.text_start
        ins = next(flow.md.disasm(flow.ctx.text[offset:offset + 16], va, 1), None)
        if ins is None:
            break
        ops = ins.operands
        if ins.mnemonic in ("call", "jmp") and previous is not None and ops and ops[0].type == x86.X86_OP_IMM:
            return previous, ops[0].imm & 0xffffffff
        previous = None
        if ins.mnemonic == "mov" and len(ops) == 2 and ops[0].type == x86.X86_OP_REG \
                and ins.reg_name(ops[0].reg) == "ecx" and ops[1].type == x86.X86_OP_IMM:
            previous = ops[1].imm & 0xffffffff
        va += ins.size
        if ins.mnemonic in ("ret", "jmp"):
            break
    return None, None


def named_at(records):
    """{address: name} of rows that start a sized data or code extent (first name wins)."""
    result = {}
    for r in sorted((r for r in records if r["address"] is not None), key=lambda r: (r["address"], r["symbol"])):
        result.setdefault(r["address"], r["symbol"])
    return result


def heads_helper_run(ctx, va, size, helpers):
    """Size of the helper copy after the empty static at va (after fill), or None."""
    if ctx.read(va, size) != b"\xc3":
        return None
    following = unmapped._fill_run(ctx, va + size, va + size + unmapped.PADDING_ALIGNMENT)
    return next((n for n in range(1, 256) if following + n <= ctx.text_start + len(ctx.text)
                 and masked(ctx.read(following, n)) in helpers), None)


# ---------------------------------------------------------------- sibling build

def load_reference(directory):
    """The sibling build (tools/sibling.py Reference) with per-object function order, or None."""
    directory = Path(directory)
    if not (directory / "debugmap.tsv.gz").is_file():
        return None
    from tools.sibling import Reference
    reference = Reference(directory)
    reference.objects = collections.defaultdict(list)
    for va, row in ((va, row) for hits in reference.by_name.values() for va, row in hits):
        reference.objects[row["object"]].append(va)
    for key in reference.objects:
        reference.objects[key] = sorted(set(reference.objects[key]))
    return reference


def summary(instructions, name_at, read):
    from tools.sibling import annotate
    _, result = annotate(instructions, name_at, read)
    return result


def content_signals(ours, theirs):
    """Agreeing content signals between two call/constant summaries."""
    signals = []

    def agree(a, b, minimum=1):
        a, b = set(a), set(b)
        shared = a & b
        return len(shared) >= minimum and len(shared) * 2 >= len(a | b)
    calls_a = [c for c in ours["calls"] if not c.startswith("(")]
    calls_b = [c for c in theirs["calls"] if not c.startswith("(")]
    if agree(calls_a, calls_b):
        signals.append("calls " + " ".join(sorted(set(calls_a) & set(calls_b))))
    if agree(ours["floats"], theirs["floats"], 2):
        signals.append("floats " + " ".join(sorted(set(ours["floats"]) & set(theirs["floats"]))))
    globals_a = [g for g in ours["globals"] if not g.startswith(("?", "_real", "real@", "LC"))]
    globals_b = [g for g in theirs["globals"] if not g.startswith(("L", "_L"))]
    if agree(globals_a, globals_b):
        signals.append("globals " + " ".join(sorted(set(globals_a) & set(globals_b))))
    return signals


def sibling_names(ctx, flow, extents, reference, records, layout):
    """{va: (name, signals)} for unmapped functions the sibling build names.

    Window: the nearest mapped neighbours before and after (up to 8 rows away)
    whose names the sibling build defines exactly once, in one object, the
    earlier one first. Candidates: the sibling functions between them in that
    object whose names the map does not hold. A name is taken when it is the
    only candidate with a content signal for this function and this function
    the only unmapped one in the window with a signal for it.
    """
    from tools.sibling import base_name, disassemble
    rows = sorted((r for r in records if r["address"] and r["size"]
                   and ctx.text_start <= r["address"] < ctx.text_start + len(ctx.text)), key=lambda r: r["address"])
    starts = [r["address"] for r in rows]
    mapped_names = {base_name(r["symbol"]) for r in records if r["address"]}
    ours_name = {r["address"]: base_name(r["symbol"]) for r in rows}

    def ours_read(va, size):
        try:
            return ctx.pe.read_va(va, size)
        except Exception:
            return None

    def unique_hit(row):
        hits = reference.lookup(base_name(row["symbol"]))
        objects = {r["object"] for _, r in hits}
        return (hits[0][0], hits[0][1]["object"]) if len(hits) == 1 and len(objects) == 1 else None

    windows = collections.defaultdict(list)
    for e in extents:
        i = bisect.bisect_right(starts, e.start) - 1
        before = next((h for h in (unique_hit(rows[j]) for j in range(i, max(-1, i - 8), -1)) if h), None)
        after = next((h for h in (unique_hit(rows[j]) for j in range(i + 1, min(len(rows), i + 9))) if h), None)
        if not before or not after or before[1] != after[1] or before[0] >= after[0]:
            continue
        windows[(before, after)].append(e)
    result = {}
    for (before, after), members in windows.items():
        order = reference.objects[before[1]]
        between = [va for va in order if before[0] < va < after[0]]
        candidates = []
        for va in between:
            name = reference.name_at(va)
            if not name or name in mapped_names:
                continue
            start, size = reference.macho.extent(va)
            code = reference.macho.read(start, size)
            if not code:
                continue
            candidates.append((va, name, summary(disassemble(code, start), reference.name_at, reference.macho.read)))
        if not candidates:
            continue
        scores = {}
        for e in members:
            ours = summary(disassemble(ctx.read(e.start, e.end - e.start), e.start), ours_name.get, ours_read)
            for va, name, theirs in candidates:
                signals = content_signals(ours, theirs)
                if signals:
                    scores[(e.start, va)] = (name, signals)
        for (start, va), (name, signals) in scores.items():
            rivals_here = [k for k in scores if k[0] == start]
            rivals_there = [k for k in scores if k[1] == va]
            if len(rivals_here) == 1 and len(rivals_there) == 1:
                row = reference.demangled.get(va, name)
                result[start] = (name, [f"position between {reference.name_at(before[0])} and "
                                        f"{reference.name_at(after[0])} in {before[1]}", *signals], row)
    return result


# ---------------------------------------------------------------- ports

def android_definitions(directory=ANDROID):
    """{function name: source path} of the Android port's definitions, or {}."""
    from tools.unitmap import definitions_in
    directory = Path(directory)
    result = {}
    if not (directory / "src").is_dir():
        return result
    for path in sorted((directory / "src").rglob("*")):
        if path.suffix not in (".c", ".cpp", ".cc"):
            continue
        try:
            text = path.read_text(errors="replace")
        except OSError:
            continue
        for name in definitions_in(text):
            result.setdefault(name.split("::")[-1], path.relative_to(directory).as_posix())
            result.setdefault(name, path.relative_to(directory).as_posix())
    return result


def port_of(name, android):
    if not name:
        return ""
    from tools.sibling import base_name
    key = base_name(name)
    if key in android:
        return "android:" + android[key]
    return ""


# ---------------------------------------------------------------- derivation

def derive(ctx, reference=None, android=None, layout=None):
    """Ledger rows (dicts with FIELDS) for every decoded unmapped function."""
    layout = load_layout() if layout is None else layout
    android = android or {}
    flow = unmapped.Flow(ctx)
    extents = [e for e in unmapped.function_extents(ctx, flow) if e.kind == unmapped.EXTENT_KINDS[1]]
    helpers = helper_bytes(ctx.records, ctx.read)
    taken = {r["symbol"] for r in ctx.records if r["address"] is not None}
    names = named_at(ctx.records)
    initializers = crt_initializers(ctx)
    used = set()
    rows = {}

    def add(e, symbol, evidence, proposed="", signals=(), leading=False, port=""):
        compiland, _ = compiland_of(layout, e.start, leading)
        if symbol is None or symbol in taken or symbol in used:
            if symbol and not proposed:
                proposed = symbol.lstrip("_") if not symbol.startswith("?") else symbol
                signals = [*signals, "name already mapped elsewhere"]
            symbol = address_name(e.start)
        used.add(symbol)
        rows[e.start] = {"address": f"0x{e.start:08x}", "size": str(e.end - e.start), "symbol": symbol,
                         "evidence": evidence, "proposed": proposed, "compiland": compiland,
                         "signals": "; ".join(signals), "port": port}

    by_start = {e.start: e for e in extents}
    # compiler: initializer stubs and the destructors they register.
    for stub in sorted(initializers):
        e = by_start.get(stub)
        if e is None:
            continue
        obj, destructor = initializer_shape(flow, e.start, e.end)
        dtor = by_start.get(destructor) if destructor else None
        if obj is None and dtor is not None:
            obj, _ = initializer_shape(flow, dtor.start, dtor.end)
        global_name = names.get(obj) if obj else None
        source = source_name(global_name) if global_name else None
        signals = [f".CRT$XC slot {initializers[stub]:#x}"]
        if global_name:
            signals.append(f"object {global_name}")
        add(e, f"??__E{source}@@YAXXZ" if source else None, "compiler", signals=signals)
        if dtor is not None and dtor.start not in rows:
            add(dtor, f"??__F{source}@@YAXXZ" if source else None, "compiler",
                signals=[f"atexit destructor registered by the initializer at {stub:#x}"]
                + ([f"object {global_name}"] if global_name else []))
    # compiler: the constructor or destructor an initializer or destructor stub calls with
    # its object as `this` (`mov ecx, offset object; call/jmp target`): a special member of
    # the object's class (its map type); without a vftable for the class the destructor is
    # not virtual, and a constructor or destructor run from a global initializer is public.
    ctypes = {r["address"]: r["ctype"] for r in ctx.records if r["address"] is not None and r["ctype"]}
    vftables = {r["symbol"] for r in ctx.records if r["symbol"].startswith("??_7")}
    stubs = [(r["address"], r["size"], r["symbol"]) for r in ctx.records if r["address"] and r["size"]]
    stubs += [(int(r["address"], 16), int(r["size"]), r["symbol"]) for r in rows.values()]
    for stub, size, symbol in sorted(stubs):
        special = "??0" if symbol.startswith("??__E") else "??1" if symbol.startswith("??__F") else None
        if special is None:
            continue
        obj, target = this_call(flow, stub, stub + size)
        cls = ctypes.get(obj, "")
        callee = by_start.get(target)
        if callee is None or callee.start in rows or not re.fullmatch(r"[A-Za-z_]\w*", cls):
            continue
        what = "constructor" if special == "??0" else "destructor"
        signals = [f"{what}: {symbol} calls it with {names.get(obj)} ({cls}) as this"]
        if f"??_7{cls}@@6B@" in vftables:
            add(callee, None, "compiler", proposed=f"{cls}::{'~' if special == '??1' else ''}{cls}()",
                signals=signals + ["the class has a vftable: the destructor may be virtual"])
        else:
            add(callee, f"{special}{cls}@@QAE@XZ", "compiler", signals=signals)
    # copy: header-static helpers.
    for e in extents:
        if e.start in rows:
            continue
        hit = helpers.get(masked(ctx.read(e.start, e.end - e.start)))
        if hit:
            stem, signature, reference_row = hit
            compiland, language = compiland_of(layout, e.start, leading=True)
            symbol = copy_name(stem, signature, compiland, language)
            if symbol in taken or symbol in used:
                owner = next((r for r in ctx.records if r["symbol"] == symbol), None)
                other = compiland_of(layout, owner["address"], True)[0] if owner else \
                    next((r["compiland"] for r in rows.values() if r["symbol"] == symbol), "")
                if other != compiland:
                    symbol = copy_name(stem, signature, compiland, language, qualified=True)
            add(e, symbol, "copy",
                proposed="" if compiland else stem,
                signals=[f"bytes equal {reference_row}"], leading=True)
    # copy: the empty static heading a compiland's header-static run (NuError,
    # extern "C" in the sibling build, where it heads every object that uses it).
    for e in extents:
        size = None if e.start in rows else heads_helper_run(ctx, e.start, e.end - e.start, helpers)
        if size is None:
            continue
        following = unmapped._fill_run(ctx, e.end, e.end + unmapped.PADDING_ALIGNMENT)
        compiland, _ = compiland_of(layout, e.start, leading=True)
        stem, _, reference_row = helpers[masked(ctx.read(following, size))]
        add(e, f"_{prefix(compiland)}NuErrorPc" if compiland else None, "copy", proposed="" if compiland else "NuError",
            signals=[f"empty static directly before the {stem} copy at {following:#x}",
                     "sibling build: the empty extern \"C\" NuError heads each object's header statics"],
            leading=True)
    # sibling: names from the sibling build.
    if reference is not None:
        found = sibling_names(ctx, flow, [e for e in extents if e.start not in rows], reference, ctx.records, layout)
        for start, (name, signals, demangled) in sorted(found.items()):
            e = by_start[start]
            compiland, language = compiland_of(layout, start)
            symbol = None if is_cxx(language) or "::" in name else "_" + name
            add(e, symbol, "sibling", proposed=demangled if symbol is None else "", signals=signals,
                port=port_of(name, android))
    # extent: the rest, with what refers to it.
    refs = ctx.references()
    for e in extents:
        if e.start not in rows:
            signals = [f"{e.reached} of {e.end - e.start} bytes decoded"] if e.reached != e.end - e.start else []
            signals.append(reference_note(ctx, refs.get(e.start, ())))
            add(e, None, "extent", signals=signals)
    return [rows[k] for k in sorted(rows)]


def reference_note(ctx, sources):
    """What refers to a function start: calls and absolute operands in mapped code, dwords in data."""
    if not sources:
        return "unreferenced"
    starts = ctx.mapped_starts
    rows = sorted(((r["address"], r["size"], r["symbol"]) for r in ctx.records if r["address"] and r["size"]))
    row_starts = [a for a, _, _ in rows]
    parts = []
    for source, kind in sorted(sources)[:4]:
        i = bisect.bisect_right(row_starts, source) - 1
        while i >= 0 and rows[i][0] + rows[i][1] <= source and source - rows[i][0] < 0x10000:
            i -= 1
        holder = rows[i][2] if i >= 0 and rows[i][0] <= source < rows[i][0] + rows[i][1] else "unmapped data"
        what = {"call": "called from", "abs": "address operand in", "data": "pointer in"}.get(kind, kind)
        parts.append(f"{what} {holder} at {source:#x}")
    del starts
    return "; ".join(parts) + (f" (+{len(sources) - 4})" if len(sources) > 4 else "")


# ---------------------------------------------------------------- ledger

def read_ledger(path=LEDGER):
    if not Path(path).is_file():
        return []
    with open(path, newline="") as stream:
        return list(csv.DictReader(stream))


def write_ledger(rows, path=LEDGER):
    out = io.StringIO()
    writer = csv.DictWriter(out, fieldnames=FIELDS, lineterminator="\n")
    writer.writeheader()
    for row in sorted(rows, key=lambda r: int(r["address"], 16)):
        writer.writerow({k: row.get(k, "") for k in FIELDS})
    Path(path).write_text(out.getvalue())


def add_symbols(rows):
    unmapped.write_rows([(int(r["address"], 16), r["symbol"], int(r["size"]), "", False) for r in rows])


def check(ctx, ledger):
    """Contradictions between the ledger, the map and the retail bytes."""
    errors = []
    sized = {(r["address"], r["symbol"]): r["size"] for r in ctx.records if r["address"] is not None}
    helpers = helper_bytes(ctx.records, ctx.read)
    flow = unmapped.Flow(ctx)
    seen = set()
    for row in ledger:
        va, size, symbol = int(row["address"], 16), int(row["size"]), row["symbol"]
        where = f"{symbol} at {va:#x}"
        if row["evidence"] not in EVIDENCE:
            errors.append(f"{where}: unknown evidence {row['evidence']!r}")
        if (va, symbol) in seen:
            errors.append(f"{where}: listed twice")
        seen.add((va, symbol))
        if sized.get((va, symbol)) != size:
            errors.append(f"{where}: symbols.csv does not map it with size {size}")
        if ADDRESS_NAME.fullmatch(symbol) and symbol.lstrip("_") != f"fun_{va:08x}":
            errors.append(f"{where}: address name of another address")
        if row["evidence"] == "copy" and masked(ctx.read(va, size)) not in helpers \
                and not heads_helper_run(ctx, va, size, helpers):
            errors.append(f"{where}: bytes are no known helper's")
        if row["evidence"] == "compiler" and not symbol.startswith(("??__E", "??__F", "??0", "??1")) \
                and not ADDRESS_NAME.fullmatch(symbol):
            errors.append(f"{where}: compiler-generated row without an initializer or special-member name")
        seen_flow, tables = flow.descend([va], va, va + size)
        if unmapped._extent_end(seen_flow, tables, va) != va + size:
            errors.append(f"{where}: its control flow ends at "
                          f"{unmapped._extent_end(seen_flow, tables, va):#x}, not at {va + size:#x}")
    return errors


def worklist(ledger):
    """Markdown: newly bounded functions per compiland."""
    table = collections.defaultdict(lambda: collections.Counter())
    for row in ledger:
        key = row["compiland"] or "(compiland undetermined)"
        table[key]["functions"] += 1
        table[key]["bytes"] += int(row["size"])
        table[key][row["evidence"]] += 1
        table[key]["named"] += 0 if ADDRESS_NAME.fullmatch(row["symbol"]) else 1
        table[key]["port"] += bool(row["port"])
    lines = ["| Compiland | Functions | Bytes | Named | copy | compiler | sibling | extent | Port source |",
             "| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |"]
    for key, c in sorted(table.items(), key=lambda kv: -kv[1]["bytes"]):
        lines.append(f"| {key} | {c['functions']} | {c['bytes']:,} | {c['named']} | {c['copy']} | {c['compiler']} "
                     f"| {c['sibling']} | {c['extent']} | {c['port']} |")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--write", action="store_true", help="add the rows to symbols.csv and the ledger")
    mode.add_argument("--refresh", action="store_true", help="recompute the compiland and port columns")
    mode.add_argument("--check", action="store_true", help="verify the ledger against the map and retail")
    mode.add_argument("--worklist", action="store_true", help="newly bounded functions per compiland")
    parser.add_argument("--only", choices=EVIDENCE, help="write the rows of one evidence class only")
    parser.add_argument("--reference", default=str(DEFAULT_REFERENCE), help="sibling build directory")
    parser.add_argument("--android", default=str(ANDROID), help="Android port checkout (port column)")
    args = parser.parse_args()
    ledger = read_ledger()
    if args.worklist:
        print(worklist(ledger))
        return 0
    ctx = unmapped.Context(retail(project()["target"]), load_symbols(), unmapped.member_ranges())
    if args.check:
        errors = check(ctx, ledger)
        if errors:
            print("codemap: ledger rows contradict the map or retail:\n  - " + "\n  - ".join(errors[:50]),
                  file=sys.stderr)
            return 1
        print(f"All {len(ledger)} ledger rows agree with the map and retail.")
        return 0
    android = android_definitions(args.android)
    if args.refresh:
        layout = load_layout()
        for row in ledger:
            leading = row["evidence"] == "copy"
            row["compiland"] = compiland_of(layout, int(row["address"], 16), leading)[0]
            if not row["port"] or row["port"].startswith("android:"):
                row["port"] = port_of(row["proposed"] or ("" if ADDRESS_NAME.fullmatch(row["symbol"])
                                                          else row["symbol"]), android) \
                    if row["evidence"] == "sibling" else row["port"]
        write_ledger(ledger)
        print(f"Refreshed {len(ledger)} ledger rows.")
        return 0
    reference = load_reference(args.reference)
    if reference is None:
        print(f"note: no sibling build in {args.reference}; the sibling class is skipped", file=sys.stderr)
    rows = derive(ctx, reference, android)
    if args.only:
        rows = [r for r in rows if r["evidence"] == args.only]
    counts = collections.Counter(r["evidence"] for r in rows)
    sizes = collections.Counter()
    for r in rows:
        sizes[r["evidence"]] += int(r["size"])
    for evidence in EVIDENCE:
        named = sum(1 for r in rows if r["evidence"] == evidence and not ADDRESS_NAME.fullmatch(r["symbol"]))
        print(f"{evidence:>9}: {counts[evidence]:>5} functions {sizes[evidence]:>7} bytes, {named} named")
    if args.write and rows:
        add_symbols(rows)
        write_ledger(ledger + rows)
        print(f"Added {len(rows)} rows to symbols.csv and {LEDGER.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
