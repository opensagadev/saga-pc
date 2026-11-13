#!/usr/bin/env python3
"""Rank non-exact functions by source differences a private sibling build suggests.

    python3 tools/sibscan.py                      # all non-exact functions with a sibling counterpart
    python3 tools/sibscan.py --exact-sample 400   # noise: the same detectors on exact functions
    python3 tools/sibscan.py --detector stores    # one detector only
    python3 tools/sibscan.py --names names.csv    # write original-name candidates (calls and globals)
    python3 tools/sibscan.py --all --names n.csv  # name candidates from every function with a counterpart
    python3 tools/sibscan.py --reference DIR ...  # reference in another directory

For every function it disassembles three versions (the sibling build's, retail's
and ours; see tools/sibling.py and docs/reference-symbols.md) and runs these
detectors, each printing one line per hit, ranked by expected payoff (larger
differences in lower-matching functions first):

  joins    the sibling and retail reach a compare block from two forward edges
           (a second `if` after a block) more often than ours does (I46); GCC
           alone joins far more often, so retail must agree
  vararg   our source walks a pointer from the last named parameter of a
           variadic function, or the sibling takes the address of the argument
           area while ours does not (I47)
  stores   the number of stores to one field offset or global differs between
           retail and ours, and the sibling agrees with retail
  calls    the call list differs between retail and ours, and the sibling agrees
           with retail on the differing calls (missing, extra or reordered)
  consts   float constants, immediates or field offsets that retail and the
           sibling both use and ours does not, or that only ours uses
  files    the function calls an empty or tiny helper that the sibling compiles
           in the same source file while ours defines it in another unit, or the
           reverse (I35)
  shape    retail and the sibling agree on the number of loops or the loop
           counter direction and ours differs (GCC keeps a single return, so
           early returns are not compared)

`--exact-sample N` runs the detectors on N exact functions instead: any hit
there is noise, so it measures each detector's false-positive rate. Detectors
that require retail to differ from ours cannot fire on exact functions; for
them the sample also reports the looser sibling-versus-ours rate that the
retail filter removes.

Without the reference directory the tool prints a notice and exits 0.
"""
import argparse
import collections
import csv
import difflib
import json
from pathlib import Path
import random
import re
import sys

if __package__:
    from . import sibling as sib
    from .lib import ROOT
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import sibling as sib
    from tools.lib import ROOT

COMPARE = re.compile(r"^(cmp|test|fcom|fcomp|fcompp|fucom|fucomp|fucompp|fnstsw|ucomis|comis|sahf|fcomip|fucomip)")
STORE_OPS = {"mov", "fstp", "fst", "fistp", "fist", "movss", "movsd", "movq", "movd", "inc", "dec", "add",
             "sub", "or", "and", "xor", "movaps", "movups"}
MEMORY = re.compile(r"(byte|word|dword|qword|xword|tbyte) ptr \[([^\]]+)\]")
PLACEHOLDER = re.compile(r"(Pc\d*$|_candidate$|[0-9a-fA-F]{6,8}$|^(sub|FUN|DAT|dword|byte|word|unk|g)_)")
DETECTORS = ("joins", "vararg", "stores", "calls", "consts", "files", "shape")


# ---------------------------------------------------------------------------
# Instruction normalisation and control flow


def resolve(instructions):
    """Replace GCC PIC base-relative operands by absolute addresses (as tools/sibling.py does)."""
    following = {va: (mnem, ops) for va, mnem, ops in instructions}
    pic, out = {}, []
    for va, mnem, ops in instructions:
        if mnem == "call" and ops.startswith("0x") and following.get(int(ops, 16), ("",))[0] == "pop":
            pic[following[int(ops, 16)][1]] = int(ops, 16)
            out.append((va, "picbase", ""))
            continue
        if mnem == "pop" and ops in pic and pic[ops] != va:
            del pic[ops]
        for reg, base in pic.items():
            ops = re.sub(r"\[" + reg + r" \+ (0x[0-9a-f]+)\]", lambda m: f"[0x{base + int(m.group(1), 16):x}]", ops)
        out.append((va, mnem, ops))
    return out


def has_frame(instructions):
    return len(instructions) > 1 and instructions[0][1:] == ("push", "ebp") \
        and instructions[1][1:] == ("mov", "ebp, esp")


def target_of(ops):
    try:
        return int(ops, 16)
    except ValueError:
        return None


class Flow:
    """Basic blocks and edges of one function's instruction list."""

    def __init__(self, instructions):
        self.instructions = instructions
        addresses = [va for va, _, _ in instructions]
        self.start = addresses[0] if addresses else 0
        self.end = (addresses[-1] + 1) if addresses else 0
        inside = set(addresses)
        leaders = {self.start}
        for i, (va, mnem, ops) in enumerate(instructions):
            if mnem.startswith("j") or mnem in ("ret", "retn"):
                if i + 1 < len(instructions):
                    leaders.add(instructions[i + 1][0])
                target = target_of(ops)
                if mnem.startswith("j") and target in inside:
                    leaders.add(target)
        self.blocks = {}
        current = None
        for va, mnem, ops in instructions:
            if va in leaders:
                current = va
                self.blocks[current] = []
            self.blocks[current].append((va, mnem, ops))
        order = sorted(self.blocks)
        self.successors = {}
        for i, block in enumerate(order):
            last = self.blocks[block][-1]
            succ = []
            mnem, target = last[1], target_of(last[2])
            if mnem.startswith("j") and target in self.blocks:
                succ.append(target)
            if not (mnem == "jmp" or mnem.startswith("ret")) and i + 1 < len(order):
                succ.append(order[i + 1])
            self.successors[block] = succ
        self.predecessors = collections.defaultdict(list)
        for block, succ in self.successors.items():
            for s in succ:
                self.predecessors[s].append(block)

    def back_edges(self):
        return [(b, s) for b, succ in self.successors.items() for s in succ if s <= b]

    def joined_compares(self):
        """Compare blocks (a test feeding a conditional jump) reached by two or more forward edges."""
        count = 0
        for block, body in self.blocks.items():
            last = body[-1][1]
            if not (last.startswith("j") and last != "jmp"):
                continue
            if not any(COMPARE.match(m) for _, m, _ in body):
                continue
            preds = self.predecessors.get(block, [])
            if len(preds) >= 2 and all(p < block for p in preds):
                count += 1
        return count

    def loop_steps(self):
        """Direction of the counter update in each loop's latch block: '+', '-' or '?'."""
        steps = []
        for latch, header in self.back_edges():
            direction = "?"
            for _, mnem, ops in reversed(self.blocks[latch]):
                if mnem == "inc" or (mnem == "add" and ops.endswith((", 1", ", 2", ", 4", ", 8"))):
                    direction = "+"
                    break
                if mnem == "dec" or (mnem == "sub" and ops.endswith((", 1", ", 2", ", 4", ", 8"))):
                    direction = "-"
                    break
            steps.append(direction)
        return sorted(steps)


def stores(instructions, name_at, frame):
    """Count stores per destination: field offset (any base register but the stack) or global name."""
    counts = collections.Counter()
    for _, mnem, ops in instructions:
        if mnem not in STORE_OPS and not mnem.startswith(("fst", "fist", "set")):
            continue
        destination = ops.split(",")[0] if "," in ops else ops
        if mnem in ("fld", "cmp", "test"):
            continue
        if mnem.startswith(("fst", "fist")) and "," in ops:
            continue  # fst st(i)
        match = MEMORY.search(destination)
        if not match:
            continue
        width, address = match.group(1), match.group(2)
        if re.fullmatch(r"0x[0-9a-f]+", address):
            name = name_at(int(address, 16))
            counts[("global", name or address, width)] += 1
            continue
        base = address.split()[0]
        if base == "esp" or (frame and base == "ebp"):
            continue
        offset = re.search(r"[+-] (0x[0-9a-f]+|\d+)$", address)
        value = int(offset.group(1), 0) if offset else 0
        if offset and "-" in address.split()[-2]:
            value = -value
        counts[("field", value, width)] += 1
    return counts


class View:
    """One function as one compiler built it: instructions, flow and summary."""

    def __init__(self, code, va, name_at, read):
        raw = sib.disassemble(code, va)
        self.instructions = resolve(raw)
        self.frame = has_frame(self.instructions)
        self.flow = Flow(self.instructions)
        _, self.summary = sib.annotate(raw, name_at, read)
        self.stores = stores(self.instructions, name_at, self.frame)
        self.fields = self._fields()
        self.argument_area = self._argument_area()
        self.global_sequence = list(dict.fromkeys(self.summary["globals"]))

    def _fields(self):
        fields = set()
        for _, mnem, ops in self.instructions:
            for match in MEMORY.finditer(ops):
                address = match.group(2)
                base = address.split()[0]
                if base == "esp" or (self.frame and base == "ebp") or re.fullmatch(r"0x[0-9a-f]+", address):
                    continue
                offset = re.search(r"\+ (0x[0-9a-f]+|\d+)$", address)
                if offset and int(offset.group(1), 0) < 0x10000:
                    fields.add(int(offset.group(1), 0))
        return fields

    def _argument_area(self):
        """GCC va_start: the address of an incoming argument slot is taken (`lea reg, [ebp + N]`, N >= 8).

        Only for frame-pointer code: without a frame, locals and arguments share esp offsets."""
        for _, mnem, ops in self.instructions:
            if mnem == "lea" and self.frame:
                match = re.search(r"\[ebp \+ (0x[0-9a-f]+|\d+)\]", ops)
                if match and int(match.group(1), 0) >= 8:
                    return True
        return False


# ---------------------------------------------------------------------------
# Detectors. Each returns (score, text) or None. `tri` has .sibling, .retail,
# .ours (View or None), .record, .percent, .unit, .sibling_row.


def lcs(a, b):
    return sum(block.size for block in difflib.SequenceMatcher(a=a, b=b, autojunk=False).get_matching_blocks())


def detect_joins(tri, loose=False):
    s, o = tri.sibling.flow.joined_compares(), tri.ours.flow.joined_compares()
    r = tri.retail.flow.joined_compares()
    if s > o and (loose or r > o):
        return s - o, f"sibling joins {s} retail {r} ours {o}"
    return None


def detect_vararg(tri, loose=False):
    text = []
    if tri.source_walks:
        text.append("source walks a parameter pointer: " + tri.source_walks)
    if tri.sibling.argument_area and tri.variadic and not tri.uses_va_start:
        text.append("sibling takes the argument area address; source has no va_start")
    return (2 if text else 0, "; ".join(text)) if text else None


def detect_stores(tri, loose=False):
    sibling_counts = collections.Counter()
    for (kind, key, width), n in tri.sibling.stores.items():
        sibling_counts[(kind, key)] += n
    retail_counts, ours_counts = collections.Counter(), collections.Counter()
    for (kind, key, width), n in tri.retail.stores.items():
        retail_counts[(kind, key)] += n
    for (kind, key, width), n in tri.ours.stores.items():
        ours_counts[(kind, key)] += n
    reference = sibling_counts if loose else retail_counts
    hits = []
    for key in sorted(set(reference) | set(ours_counts), key=str):
        r, o, s = retail_counts[key], ours_counts[key], sibling_counts[key]
        if loose:
            if key[0] == "field" and s != o and s and o:
                hits.append(f"{key[0]} {key[1]:#x}: sibling {s} ours {o}")
            continue
        if r != o and s and (s == r or (s - o) * (r - o) > 0):
            label = f"{key[1]:#x}" if key[0] == "field" else key[1]
            hits.append(f"{key[0]} {label}: sibling {s} retail {r} ours {o}")
    return (len(hits), "; ".join(hits)) if hits else None


def detect_calls(tri, loose=False):
    sc, rc = tri.sibling.summary["calls"], tri.retail.summary["calls"]
    oc = tri.ours.summary["calls"]
    if loose:
        diff = sib.compare_calls(sc, oc)
        return (len(diff), "sibling->ours " + "; ".join(f"{t} {' '.join(a)}=>{' '.join(b)}" for t, a, b in diff)) \
            if diff else None
    if rc == oc:
        return None
    s, r, o = collections.Counter(sc), collections.Counter(rc), collections.Counter(oc)
    notes = []
    for name in sorted(set(r) | set(o)):
        if r[name] != o[name] and s[name] and (s[name] - o[name]) * (r[name] - o[name]) > 0:
            notes.append(f"{'missing' if r[name] > o[name] else 'extra'} {name} (sibling {s[name]} "
                         f"retail {r[name]} ours {o[name]})")
    if r == o and lcs(sc, rc) > lcs(sc, oc):
        notes.append("order: sibling agrees with retail (" + " ".join(rc) + ")")
    return (len(notes), "; ".join(notes)) if notes else None


def float_set(view):
    return {v for v in view.summary["floats"]}


def detect_consts(tri, loose=False):
    S, R, O = tri.sibling, tri.retail, tri.ours
    notes = []
    pairs = [("float", float_set(S), float_set(R), float_set(O)),
             ("imm", set(S.summary["immediates"]), set(R.summary["immediates"]), set(O.summary["immediates"])),
             ("field", S.fields, R.fields, O.fields)]
    for kind, s, r, o in pairs:
        if loose:
            only = (s - o) if kind != "field" else set()
            if only:
                notes.append(f"{kind} sibling-not-ours {sorted(map(str, only))}")
            continue
        missing, extra = (s & r) - o, (o - r) - s
        fmt = (lambda v: hex(v)) if kind != "float" else str
        if missing:
            notes.append(f"{kind} missing {' '.join(fmt(v) for v in sorted(missing))}")
        if extra and (r - o):
            notes.append(f"{kind} extra {' '.join(fmt(v) for v in sorted(extra))}")
    return (len(notes), "; ".join(notes)) if notes else None


def detect_files(tri, loose=False):
    notes = []
    for retail_name, sibling_name, address in tri.helpers:
        sibling_file = tri.sibling_files.get(sibling_name)
        our_unit = tri.units_by_address.get(address)
        if not sibling_file or not our_unit or not tri.unit:
            continue
        same_sibling = sibling_file == tri.sibling_row["source"]
        same_ours = our_unit == tri.unit
        if same_sibling != same_ours:
            notes.append(f"{retail_name}: sibling {'same file' if same_sibling else sibling_file.split('/')[-1]}, "
                         f"ours {'same unit' if same_ours else our_unit}")
    return (len(notes), "; ".join(notes)) if notes else None


def detect_shape(tri, loose=False):
    S, R, O = (v.flow for v in (tri.sibling, tri.retail, tri.ours))
    notes = []
    checks = [("loops", len(S.back_edges()), len(R.back_edges()), len(O.back_edges())),
              ("steps", S.loop_steps(), R.loop_steps(), O.loop_steps())]
    for label, s, r, o in checks:
        if loose:
            if s != o:
                notes.append(f"{label} sibling {s} ours {o}")
        elif r != o and s == r:
            notes.append(f"{label} sibling {s} retail {r} ours {o}")
    return (len(notes), "; ".join(notes)) if notes else None


FUNCTIONS = {"joins": detect_joins, "vararg": detect_vararg, "stores": detect_stores, "calls": detect_calls,
             "consts": detect_consts, "files": detect_files, "shape": detect_shape}


# ---------------------------------------------------------------------------
# Source side: variadic functions


VARIADIC = re.compile(r"^[ \t]*(?:extern \"C\" )?[A-Za-z_][\w \*]*?\b(\w+)\s*\(([^;{}()]*?),\s*\.\.\.\s*\)\s*\{",
                      re.M)


def variadic_sources():
    """Map function name -> (uses va_start, walk description) for variadic definitions in src/."""
    result = {}
    for path in sorted([*ROOT.glob("src/**/*.c"), *ROOT.glob("src/**/*.cpp")]):
        text = path.read_text(errors="replace")
        for match in VARIADIC.finditer(text):
            name, params = match.group(1), match.group(2)
            last = re.findall(r"(\w+)\s*$", params.strip())
            body_start = match.end()
            depth, i = 1, body_start
            while depth and i < len(text):
                depth += {"{": 1, "}": -1}.get(text[i], 0)
                i += 1
            body = text[body_start:i]
            walk = ""
            if last and re.search(r"&\s*" + re.escape(last[0]) + r"\b", body):
                walk = f"&{last[0]} in {path.relative_to(ROOT)}"
            result[name] = ("va_start" in body, walk)
    return result


# ---------------------------------------------------------------------------
# Driver


class Ours(sib.Ours):
    """tools/sibling.Ours with one parsed object per unit."""

    def __init__(self):
        super().__init__()
        self._objects = {}
        coff = self._coff

        def cached(path):
            key = str(path)
            if key not in self._objects:
                self._objects[key] = coff(path)
            return self._objects[key]
        self._coff = cached


class Triple:
    pass


def load_report():
    return json.loads((ROOT / "matching.json").read_text())["functions"]


def percent_of(row):
    value = row.get("fuzzy_match_percent", row.get("objdiff_percent"))
    return 100.0 if row.get("exact_match") else (value if value is not None else 0.0)


def build_triple(reference, ours, row, record, context):
    short = sib.base_name(record["symbol"])
    hits = reference.lookup(short)
    if not hits:
        return None
    va, sibling_row = hits[0]
    start, size = reference.macho.extent(va)
    code = reference.macho.read(start, size)
    original = ours.read(record["address"], record["size"])
    if not code or not original:
        return None
    _, base = ours.rebuilt(record)
    if not base:
        return None
    tri = Triple()
    tri.name, tri.record, tri.row, tri.sibling_row = short, record, row, sibling_row
    tri.percent = percent_of(row)
    tri.size = record["size"]
    tri.unit = (row.get("units") or [None])[0] if len(row.get("units") or []) == 1 else None
    tri.sibling = View(code, start, reference.name_at, reference.macho.read)
    tri.retail = View(original, record["address"], ours.name_at, ours.read)
    tri.ours = View(base, record["address"], ours.name_at, ours.read)
    variadic = context["variadic"].get(short)
    tri.variadic = variadic is not None
    tri.uses_va_start = bool(variadic and variadic[0])
    tri.source_walks = variadic[1] if variadic else ""
    tri.units_by_address = context["units_by_address"]
    tri.sibling_files = context["sibling_files"]
    tri.helpers = helpers(tri, ours, context)
    return tri


def aligned_pairs(left, right):
    """Pairs (left item, right item) aligned by difflib: equal runs and equal-length replacements."""
    pairs = []
    matcher = difflib.SequenceMatcher(a=left, b=right, autojunk=False)
    for tag, a0, a1, b0, b1 in matcher.get_opcodes():
        if tag in ("equal", "replace") and a1 - a0 == b1 - b0:
            pairs.extend(zip(left[a0:a1], right[b0:b1]))
    return pairs


def call_targets(view):
    targets = []
    for _, mnem, ops in view.instructions:
        if mnem == "call":
            target = target_of(ops)
            if target is not None:
                targets.append(target)
    return targets


def helpers(tri, ours, context):
    """Retail callees that are empty or tiny, with the sibling name aligned to each."""
    result = []
    retail_calls = tri.retail.summary["calls"]
    sibling_calls = tri.sibling.summary["calls"]
    pairs = dict(aligned_pairs(retail_calls, sibling_calls))
    for address in dict.fromkeys(call_targets(tri.retail)):
        size = context["sizes"].get(address)
        name = ours.name_at(address)
        if not name or size is None:
            continue
        code = ours.read(address, min(size, 16)) or b""
        empty = size <= 3 or code[:1] == b"\xc3" or code[:3] in (b"\x33\xc0\xc3", b"\x32\xc0\xc3")
        if not (empty or size <= 16 or name in context["sibling_static"]):
            continue
        sibling_name = name if name in context["sibling_files"] else pairs.get(name)
        if sibling_name:
            result.append((name, sibling_name, address))
    return result


def make_context(reference, ours):
    rows = load_report()
    units_by_address = {}
    sizes = {}
    for r in ours.records:
        if r["size"]:
            sizes.setdefault(r["address"], r["size"])
    for row in rows:
        units = row.get("units") or []
        if len(units) == 1:
            units_by_address.setdefault(int(row["address"], 16), units[0])
    sibling_files, sibling_static = {}, set()
    for name, entries in reference.by_name.items():
        sibling_files.setdefault(name, entries[0][1]["source"])
    external = getattr(reference.macho, "external", None)
    if external is not None:
        for name, entries in reference.by_name.items():
            if entries[0][0] not in external:
                sibling_static.add(name)
    return {"rows": rows, "units_by_address": units_by_address, "sizes": sizes, "sibling_files": sibling_files,
            "sibling_static": sibling_static, "variadic": variadic_sources()}


def select(rows, ours, reference, exact_sample, names, seed, everything=False):
    by_symbol = {}
    for r in ours.records:
        by_symbol.setdefault(r["symbol"], r)
    chosen = []
    for row in rows:
        record = by_symbol.get(row["symbol"])
        if not record or not record["size"]:
            continue
        if names and sib.base_name(row["symbol"]) not in names:
            continue
        if not reference.lookup(sib.base_name(row["symbol"])):
            continue
        if exact_sample:
            if row.get("exact_match"):
                chosen.append((row, record))
        elif row.get("status") == "different" or names or everything:
            chosen.append((row, record))
    if exact_sample:
        random.Random(seed).shuffle(chosen)
        chosen = chosen[:exact_sample]
    return chosen


def rank(score, tri):
    """Expected payoff: more evidence, lower match, moderate size first."""
    weight = (100.0 - tri.percent) / 100.0 + 0.1
    return score * weight * (1.0 if tri.size < 1500 else 0.5)


def collect_names(tri, ours, names):
    """Placeholder names in ours aligned with a differently named sibling call or global."""
    for kind, left, right in (("function", tri.retail.summary["calls"], tri.sibling.summary["calls"]),
                              ("global", tri.retail.global_sequence, tri.sibling.global_sequence)):
        for ours_name, sibling_name in aligned_pairs(left, right):
            if ours_name == sibling_name or not PLACEHOLDER.search(ours_name):
                continue
            if sibling_name.startswith(("L", "(", "_L", "__")) or PLACEHOLDER.search(sibling_name):
                continue
            names[(ours_name, sibling_name, kind)].add(tri.name)


def similar(left, right):
    """Names that share a word: a common run of five letters, ignoring case and underscores."""
    a, b = (re.sub(r"[^a-z0-9]", "", n.lower()) for n in (left, right))
    match = difflib.SequenceMatcher(a=a, b=b, autojunk=False).find_longest_match(0, len(a), 0, len(b))
    return match.size >= 5


def name_candidates(names, our_names):
    """Keep pairs whose placeholder has one sibling name that is still free in ours. Confidence is high
    when the pair is seen in two functions or the names share a word. Returns sorted rows."""
    by_ours = collections.defaultdict(list)
    for (ours_name, sibling_name, kind), where in names.items():
        by_ours[(ours_name, kind)].append((sibling_name, where))
    rows = []
    for (ours_name, kind), candidates in by_ours.items():
        if len(candidates) != 1:
            continue  # folded helpers or misaligned lists
        sibling_name, where = candidates[0]
        if sibling_name in our_names:
            continue  # the original name already belongs to another of our symbols
        confidence = "high" if len(where) >= 2 or similar(ours_name, sibling_name) else "low"
        evidence = (f"{confidence}: {'called' if kind == 'function' else 'accessed'} at the same position in "
                    + " ".join(sorted(where)))
        rows.append((ours_name, sibling_name, kind, evidence))
    return sorted(rows)


def write_names(path, names, our_names):
    with open(path, "w", newline="") as stream:
        writer = csv.writer(stream, lineterminator="\n")
        writer.writerow(["our_name", "original_name", "kind", "evidence"])
        writer.writerows(name_candidates(names, our_names))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("functions", nargs="*", help="limit to these functions")
    parser.add_argument("--reference", default=str(sib.DEFAULT_REFERENCE), help="sibling reference directory")
    parser.add_argument("--detector", choices=DETECTORS, action="append", help="run only these detectors")
    parser.add_argument("--exact-sample", type=int, metavar="N", help="measure noise on N exact functions")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--all", action="store_true",
                        help="scan exact functions too (for --names; detectors cannot fire on them)")
    parser.add_argument("--names", metavar="CSV", help="write original-name candidates to CSV")
    parser.add_argument("--tsv", metavar="FILE", help="also write all hits as TSV")
    args = parser.parse_args()
    if sib.capstone is None:
        print("capstone is not installed; nothing to scan (pip install capstone)")
        return 0
    directory = Path(args.reference)
    if not (directory.is_dir() and (directory / "debugmap.tsv.gz").is_file()):
        print(f"note: no sibling reference in {directory}; nothing to scan")
        return 0
    reference = sib.Reference(directory)
    ours = Ours()
    context = make_context(reference, ours)
    detectors = args.detector or list(DETECTORS)
    chosen = select(context["rows"], ours, reference, args.exact_sample, set(args.functions), args.seed,
                    args.all)
    hits = collections.defaultdict(list)
    loose_hits = collections.Counter()
    names = collections.defaultdict(set)
    scanned = 0
    for row, record in chosen:
        try:
            tri = build_triple(reference, ours, row, record, context)
        except Exception as error:  # unusual extents, missing objects
            print(f"note: {row['symbol']}: {error}", file=sys.stderr)
            continue
        if tri is None:
            continue
        scanned += 1
        collect_names(tri, ours, names)
        for detector in detectors:
            found = FUNCTIONS[detector](tri)
            if found:
                hits[detector].append((rank(found[0], tri), tri, found[1]))
            if args.exact_sample and FUNCTIONS[detector](tri, loose=True):
                loose_hits[detector] += 1
    out = open(args.tsv, "w") if args.tsv else None
    for detector in detectors:
        found = sorted(hits[detector], key=lambda h: -h[0])
        rate = f"{len(found)}/{scanned} = {100.0 * len(found) / max(scanned, 1):.1f}%"
        line = f"== {detector}: {rate}"
        if args.exact_sample:
            line += (f" false positives (loose sibling-vs-ours: {loose_hits[detector]}/{scanned} = "
                     f"{100.0 * loose_hits[detector] / max(scanned, 1):.1f}%)")
        print(line)
        for score, tri, text in found:
            print(f"{score:7.2f}  {tri.name:<40} {tri.percent:6.2f} {tri.size:5d}  {text}")
            if out:
                out.write(f"{detector}\t{score:.3f}\t{tri.name}\t{tri.percent:.2f}\t{tri.size}\t{tri.unit}\t{text}\n")
    if args.names:
        write_names(args.names, names, {sib.base_name(r["symbol"]) for r in ours.records})
    return 0


if __name__ == "__main__":
    sys.exit(main())
