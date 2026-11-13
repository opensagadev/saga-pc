#!/usr/bin/env python3
"""Compare the symbol inventory to retail, using exact bytes and objdiff-cli."""
import argparse
import functools
import json
import math
from pathlib import Path
import re
import subprocess
import sys

if __package__:
    from . import diffclass, idiomscan
    from .binary import compare_data, relocate_function, write_function
    from .lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, dependency_files, covered_bytes,
                      digest, external_symbols, load_code_groups, prepare_code_group_objects, library_objects, load_json, load_symbols, mapping_progress, number, object_path, project, retail, section_mapping_progress, sha256)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass, idiomscan
    from tools.binary import compare_data, relocate_function, write_function
    from tools.lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, dependency_files, covered_bytes,
                     digest, external_symbols, load_code_groups, prepare_code_group_objects, library_objects, load_json, load_symbols, mapping_progress, number, object_path, project, retail, section_mapping_progress, sha256)

OUT = ROOT / "build/objdiff"


def run(command):
    result = subprocess.run([str(arg) for arg in command], capture_output=True,
                            text=True, timeout=120)
    if result.returncode:
        raise RuntimeError(result.stdout + result.stderr)
    return result.stdout


def objdiff():
    pin = load_json(ROOT / "config/tools.json")["objdiff"]
    path = ROOT / "build/tools/bin/objdiff-cli"
    if not path.is_file() or sha256(path) != pin["sha256"]:
        raise ValueError("Missing or changed objdiff-cli; run make setup")
    return path, pin


# Alignment fillers the linker or assembler puts between functions: int3, nop and
# the multi-byte no-op forms of the VC8 runtime's alignment directives.
PADDING_FILLERS = (b"\x8d\xa4\x24\x00\x00\x00\x00", b"\x8d\x9b\x00\x00\x00\x00",
                   b"\x8d\x64\x24\x00", b"\x8d\x49\x00", b"\x8d\x40\x00",
                   b"\x8b\xff", b"\x8b\xc0", b"\x90", b"\xcc")
PADDING_ALIGNMENT = 16


def pads_to(address, gap):
    """True when `gap` filler bytes can align the extent starting at `address`.

    Functions start on 16-byte boundaries; runtime assembler sections and import
    thunks on smaller ones (the start's own alignment, at most 16)."""
    return 0 < gap < min(PADDING_ALIGNMENT, address & -address if address else PADDING_ALIGNMENT)


def is_padding(data):
    """True when `data` is a sequence of whole alignment fillers."""
    offset = 0
    while offset < len(data):
        for filler in PADDING_FILLERS:
            if data.startswith(filler, offset):
                offset += len(filler)
                break
        else:
            return False
    return bool(data)


def padding_runs(functions, target, read, externals=()):
    """Alignment padding between two mapped extents of the original .text.

    A run qualifies when it is shorter than the alignment, ends on an aligned
    address where the next mapped extent starts, and holds only fillers. It is
    exact when every function ending where it starts is exact: the compiler and
    linker emit this fill deterministically after that function. Padding after
    import glue is classified but never exact.
    """
    start, total = number(target["text_va"]), number(target["text_size"])
    extents = []
    for row in functions:
        if row.get("target_size"):
            extents.append((number(row["address"]), row["target_size"], bool(row["exact_match"])))
    for row in externals:
        if row["kind"] == "import thunk":
            extents.append((number(row["address"]), row["size"], False))
    runs, end, before = [], None, []
    for address, size, exact in sorted(extents):
        gap = address - end if end is not None else 0
        if pads_to(address, gap) \
                and start <= end and address <= start + total and is_padding(read(end, gap)):
            runs.append({"address": end, "size": gap, "exact": bool(before) and all(before)})
        if end is None or address + size > end:
            end, before = address + size, [exact]
        elif address + size == end:
            before.append(exact)
    return runs


DATA_PADDING_ALIGNMENT = 4


def data_padding_runs(ranges, sections, image_base, read):
    """Zero fill the linker leaves between two known data extents.

    A run qualifies when it lies in file-backed .rdata/.data storage, is shorter
    than four bytes, ends on a four-byte boundary where the next known extent
    starts, directly follows a known extent and holds only zero bytes (string
    literals and other odd-sized items before a four-byte aligned item).
    """
    merged = []
    for start, size in sorted(ranges):
        if merged and start <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], start + size)
        else:
            merged.append([start, start + size])
    storage = []
    for section in sections:
        flags = section["characteristics"]
        if section["name"] in (".rsrc", ".reloc") or flags & 0x20 or not flags & 0x40:
            continue
        start = image_base + section["virtual_address"]
        storage.append((start, start + min(section["raw_size"], section["virtual_size"])))
    runs = []
    for (_, end), (start, _) in zip(merged, merged[1:]):
        gap = start - end
        if 0 < gap < DATA_PADDING_ALIGNMENT and start % DATA_PADDING_ALIGNMENT == 0 \
                and any(lower <= end and start <= upper for lower, upper in storage) \
                and not any(read(end, gap)):
            runs.append({"address": end, "size": gap})
    return runs


ADDRESS_NAME = re.compile(r"_?fun_[0-9a-f]{8}")


def code_name_progress(records, target):
    """Mapped .text split into identified names and standard address names (fun_XXXXXXXX)."""
    lower = number(target["text_va"])
    upper = lower + number(target["text_size"])

    def clipped(rows):
        return [(max(lower, r["address"]), min(upper, r["address"] + r["size"]) - max(lower, r["address"]))
                for r in rows if r["address"] is not None and r["size"]
                and r["address"] < upper and r["address"] + r["size"] > lower]
    rows = [r for r in records if r["address"] is not None and r["size"]]
    every = covered_bytes(clipped(rows))
    address = covered_bytes(clipped([r for r in rows if ADDRESS_NAME.fullmatch(r["symbol"])]))
    total = upper - lower
    return {"mapping_code_named_bytes": every - address,
            "mapping_code_named_percent": 100 * (every - address) / total,
            "mapping_code_address_named_bytes": address,
            "mapping_code_address_named_percent": 100 * address / total,
            "mapping_code_address_named_functions": sum(bool(ADDRESS_NAME.fullmatch(r["symbol"])) for r in rows
                                                        if lower <= r["address"] < upper)}


# Classes of initialised and zero-fill data storage, strongest first; a byte
# counts in the first class whose ranges hold it.
REPRODUCED_CLASSES = (
    ("exact", "defined by our objects and byte-exact (tools/datadiff.py class =)"),
    ("library", "provided by a pinned archive member whose section is proven byte-exact in place"),
    ("different_padding", "defined by our objects; differs only by zero padding (datadiff class p)"),
    ("different", "defined by our objects with real differences (datadiff classes a-f, u)"),
    ("undefined", "identified (mapped) but defined by no object or member"),
    ("padding", "proven alignment padding between known extents"),
    ("unidentified", "not mapped"),
)


def reproduced_data(storage, classes):
    """Bytes of each storage kind in each class of REPRODUCED_CLASSES.

    `storage` is {kind: [(start, end)]}; `classes` is {class: [(start, size)]}.
    `reproduced_<kind>_bytes` is exact plus library: bytes an object or archive
    member we link would place exactly.
    """
    def merge(ranges):
        out = []
        for start, end in sorted(ranges):
            if end <= start:
                continue
            if out and start <= out[-1][1]:
                out[-1][1] = max(out[-1][1], end)
            else:
                out.append([start, end])
        return out

    def inside(ranges, lower, upper):
        return sum(max(0, min(upper, b) - max(lower, a)) for a, b in ranges)
    result = {}
    for kind, regions in storage.items():
        total = sum(b - a for a, b in regions)
        seen, counts = [], {}
        for name, _ in REPRODUCED_CLASSES[:-1]:
            ranges = merge([(a, a + n) for a, n in classes.get(name, ())] + [tuple(r) for r in seen])
            counts[name] = sum(inside(ranges, a, b) - inside(seen, a, b) for a, b in regions)
            seen = ranges
        counts["unidentified"] = total - sum(counts.values())
        for name, count in counts.items():
            result[f"reproduced_{kind}_{name}_bytes"] = count
        reproduced = counts["exact"] + counts["library"]
        result[f"reproduced_{kind}_bytes"] = reproduced
        result[f"reproduced_{kind}_total_bytes"] = total
        result[f"reproduced_{kind}_percent"] = 100 * reproduced / total if total else 0.0
    result["reproduced_classes"] = dict(REPRODUCED_CLASSES)
    return result


def data_storage(sections, image_base):
    """{"file_backed_data": [(start, end)], "zero_fill_data": [...]} of the data sections."""
    storage = {"file_backed_data": [], "zero_fill_data": []}
    for section in sections:
        flags = section["characteristics"]
        if section["name"] in (".rsrc", ".reloc") or flags & 0x20 or not flags & 0xc0 or not section["virtual_size"]:
            continue
        start = image_base + section["virtual_address"]
        boundary = start + min(section["raw_size"], section["virtual_size"])
        storage["file_backed_data"].append((start, boundary))
        storage["zero_fill_data"].append((boundary, start + section["virtual_size"]))
    return storage


def reproduced_inputs(records, member_ranges, padding, data_rows):
    """Class ranges for reproduced_data from tools/datadiff.py and tools/unmapped.py.

    datadiff compares every current object definition slot by slot (pointers by
    name); where it disagrees with the blob comparison in `data_rows`
    (matching.json "exact"/"different"), datadiff is authoritative. Returns
    (classes, disagreements).
    """
    from tools import datadiff
    from tools.unmapped import Context, library_layout
    scanner = datadiff.Scanner()
    worst = {}
    for row in datadiff.scan(scanner, datadiff.data_rows(scanner)):
        key = (number(row["address"]), row["symbol"])
        cls = row["class"]
        rank = "=" if cls == "=" else "p" if cls == "p" else "x"
        previous = worst.get(key, ("=", 0))[0]
        order = "=px"
        worst[key] = (max(previous, rank, key=order.index), max(row.get("size") or 0, worst.get(key, ("", 0))[1]))
    classes = {"exact": [], "different_padding": [], "different": []}
    for (address, _), (rank, size) in worst.items():
        classes[{"=": "exact", "p": "different_padding", "x": "different"}[rank]].append((address, size))
    ctx = Context(retail(project()["target"]), records, member_ranges)
    classes["library"] = [(address, extent) for (address, _), extent in library_layout(ctx).items()]
    classes["undefined"] = [(r["address"], r["size"]) for r in records if r["address"] is not None and r["size"]] \
        + list(member_ranges)
    classes["padding"] = list(padding)
    by_display = {(number(r["address"]), r["name"]): r["status"] for r in data_rows if r.get("address")}
    disagreements = {"exact_in_report_different_in_datadiff": 0, "different_in_report_exact_in_datadiff": 0}
    for (address, name), (rank, _) in worst.items():
        status = by_display.get((address, name))
        if status == "exact" and rank != "=":
            disagreements["exact_in_report_different_in_datadiff"] += 1
        elif status == "different" and rank == "=":
            disagreements["different_in_report_exact_in_datadiff"] += 1
    return classes, disagreements


def text_coverage(functions, target, externals=(), padding=(), data=()):
    """Describe original text categories without changing exact-match credit.

    `data` rows inside .text (the DirectInput object-format tables the SDK's
    dilib objects place in their code section) are classified as text data:
    neither unclassified nor function credit.
    """
    start, total = number(target["text_va"]), number(target["text_size"])
    end = start + total

    def ranges(rows):
        result = []
        for row in rows:
            size = row.get("target_size", row.get("size"))
            if not size:
                continue
            address = number(row["address"])
            if size < 0 or not start <= address < address + size <= end:
                raise ValueError("Text coverage extent outside original .text")
            result.append((address, size))
        return result

    mapped_ranges = ranges(functions)
    exact_ranges = ranges(row for row in functions if row["exact_match"])
    glue_ranges = ranges(row for row in externals if row["kind"] == "import thunk")
    mapped, exact = covered_bytes(mapped_ranges), covered_bytes(exact_ranges)
    glue = covered_bytes(glue_ranges)
    if covered_bytes(mapped_ranges + glue_ranges) != mapped + glue:
        raise ValueError("Reconstruction and excluded import glue overlap")
    in_text = [row for row in data if row.get("target_size")
               and start <= number(row["address"]) < end]
    data_ranges = ranges(in_text)
    text_data = covered_bytes(data_ranges)
    exact_text_data = covered_bytes(ranges(row for row in in_text if row["exact_match"]))
    if covered_bytes(mapped_ranges + glue_ranges + data_ranges) != mapped + glue + text_data:
        raise ValueError("Text data overlaps functions or import glue")
    reconstruction_total = total - glue
    padding_bytes = sum(run["size"] for run in padding)
    exact_padding = sum(run["size"] for run in padding if run["exact"])
    return {
        "original_text_bytes": total,
        "mapped_reconstruction_bytes": mapped,
        "exact_reconstruction_bytes": exact,
        "mapped_not_exact_bytes": mapped - exact,
        "excluded_import_glue_bytes": glue,
        "text_data_bytes": text_data,
        "exact_text_data_bytes": exact_text_data,
        "unclassified_text_bytes": total - mapped - glue - text_data,
        "strict_original_text_denominator_bytes": total,
        "strict_original_text_exact_percent": 100 * exact / total if total else 0.0,
        "reconstruction_scope_denominator_bytes": reconstruction_total,
        "reconstruction_scope_exact_percent": 100 * exact / reconstruction_total if reconstruction_total else 0.0,
        "mapped_scope_denominator_bytes": mapped,
        "mapped_scope_exact_percent": 100 * exact / mapped if mapped else 0.0,
        "qualification": "Unclassified text includes undiscovered code and unproven padding; "
                         "these percentages compare relocated objects, not the linked image.",
        "alignment_padding_bytes": padding_bytes,
        "exact_alignment_padding_bytes": exact_padding,
        "unclassified_non_padding_text_bytes": total - mapped - glue - text_data - padding_bytes,
        "exact_with_padding_bytes": exact + exact_padding,
        "exact_with_padding_percent": 100 * (exact + exact_padding) / total if total else 0.0,
        "padding_qualification": f"Padding is int3/nop alignment fill shorter than {PADDING_ALIGNMENT} bytes "
                                 "between mapped extents, shorter than the alignment of the extent start it "
                                 "ends on; it is exact "
                                 "only after an exact function.",
    }


def progress(functions, target, padding=(), data=()):
    """Exact and fuzzy code progress over the original .text.

    fuzzy_coverage_percent = sum(objdiff fuzzy% x size of each original function) / fuzzy_denominator_bytes,
    fuzzy_denominator_bytes = .text - identified alignment padding - identified data stored in .text.
    Unported and unidentified code stay in the denominator at 0%: the definition of objdiff's whole-binary
    fuzzy_match_percent, where padding and data belong to no function symbol. The exact percentages count
    byte-identical functions (and the alignment padding after them) over all of .text.
    """
    total = number(target["text_size"])
    start = number(target["text_va"])
    for row in functions:
        address = number(row["address"])
        if not start <= address < start + total or address + (row["target_size"] or 0) > start + total:
            raise ValueError(f"Function outside original .text: {row['name']}")
    mapped = covered_bytes((number(r["address"]), r["target_size"])
                           for r in functions if r["target_size"])
    matched = covered_bytes((number(r["address"]), r["target_size"])
                            for r in functions if r["exact_match"])
    # Credit each original byte at most once. With overlapping aliases, use the
    # least similar comparison; an extra definition cannot inflate coverage.
    endpoints = []
    for index, row in enumerate(functions):
        if not row.get("objects") or not row["target_size"]:
            continue
        scores = [obj["objdiff_percent"] for obj in row["objects"]]
        if any(type(score) not in (int, float) or not math.isfinite(score)
               or not 0 <= score <= 100 for score in scores):
            raise ValueError(f"Invalid objdiff score for {row['name']}")
        address = number(row["address"])
        score = min(scores)
        endpoints.extend(((address, 1, index, score),
                          (address + row["target_size"], -1, index, score)))
    active = {}
    fuzzy_bytes = 0.0
    previous = None
    for address, action, index, score in sorted(endpoints):
        if previous is not None and active:
            fuzzy_bytes += (address - previous) * min(active.values()) / 100
        if action < 0:
            del active[index]
        else:
            active[index] = score
        previous = address
    matched_padding = sum(run["size"] for run in padding if run["exact"])
    padding_bytes = sum(run["size"] for run in padding)
    text_data = covered_bytes((number(row["address"]), row["target_size"]) for row in data
                              if row.get("target_size") and start <= number(row["address"]) < start + total)
    denominator = total - padding_bytes - text_data
    return {"text_bytes": total, "mapped_code_bytes": mapped,
            "matched_code_bytes": matched, "matching_percent": 100 * matched / total,
            "padding_bytes": padding_bytes,
            "matched_padding_bytes": matched_padding,
            "matched_code_with_padding_bytes": matched + matched_padding,
            "matching_with_padding_percent": 100 * (matched + matched_padding) / total,
            "fuzzy_text_data_bytes": text_data,
            "fuzzy_denominator_bytes": denominator,
            "fuzzy_coverage_percent": 100 * fuzzy_bytes / denominator if denominator else 0.0,
            "mapped_functions": len(functions),
            "matched_functions": sum(r["exact_match"] for r in functions)}


def objdiff_percent(measures):
    # Protobuf JSON omits scalar fields at their default value, including 0%.
    value = measures.get("fuzzy_match_percent", 0.0)
    if (type(value) not in (int, float) or not 0 <= value <= 100
            or not math.isfinite(value)):
        raise ValueError("objdiff fuzzy_match_percent must be a finite percentage")
    return float(value)


def objdiff_progress(report, functions):
    # Native objdiff weights every comparison object by its original code size.
    # Copies and aliases are repeated here; exact coverage uses the union above.
    compared = sum(row["target_size"] * len(row["objects"])
                   for row in functions if row["objects"])
    if report is None and compared == 0:
        return {"objdiff_fuzzy_percent": 0.0, "objdiff_total_code": 0}
    measures = report.get("measures") if isinstance(report, dict) else None
    if not isinstance(measures, dict):
        raise ValueError("objdiff report must contain measures")
    total = measures.get("total_code", 0)
    # Protobuf uint64 values are serialized as decimal strings by the pinned CLI.
    if isinstance(total, str) and re.fullmatch(r"0|[1-9][0-9]*", total):
        total = int(total)
    if type(total) is not int or total < 0:
        raise ValueError("objdiff total_code must be a nonnegative integer")
    if total != compared:
        raise ValueError(f"objdiff total_code {total} differs from compared original extents {compared}")
    return {"objdiff_fuzzy_percent": objdiff_percent(measures), "objdiff_total_code": total}


def repository_paths(value):
    """Report paths relative to the repository so reports carry no local directories."""
    prefix = str(ROOT) + "/"
    if isinstance(value, dict):
        return {repository_paths(k): repository_paths(v) for k, v in value.items()}
    if isinstance(value, list):
        return [repository_paths(v) for v in value]
    if isinstance(value, str) and value.startswith(prefix):
        return value[len(prefix):]
    return value


def srgb_linear(channel):
    channel /= 255
    return channel / 12.92 if channel <= 0.04045 else ((channel + 0.055) / 1.055) ** 2.4


def contrast(background, text):
    """WCAG contrast ratio between two (r, g, b) colours of 0..255 channels."""
    def luminance(rgb):
        r, g, b = (srgb_linear(channel) for channel in rgb)
        return 0.2126 * r + 0.7152 * g + 0.0722 * b
    lighter, darker = sorted((luminance(background), luminance(text)), reverse=True)
    return (lighter + 0.05) / (darker + 0.05)


def shields_brightness(rgb):
    """Brightness as shields' badge-maker computes it (lib/color.js brightness(), YIQ luma to 2 places)."""
    return round((rgb[0] * 299 + rgb[1] * 587 + rgb[2] * 114) / 255000, 2)


def oklch_to_srgb(lightness, chroma, hue):
    """(r, g, b) of 0..255 channels for an OKLCH colour, or None outside the sRGB gamut."""
    a, b = chroma * math.cos(math.radians(hue)), chroma * math.sin(math.radians(hue))
    l = (lightness + 0.3963377774 * a + 0.2158037573 * b) ** 3
    m = (lightness - 0.1055613458 * a - 0.0638541728 * b) ** 3
    s = (lightness - 0.0894841775 * a - 1.2914855480 * b) ** 3
    linear = (4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s,
              -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s,
              -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s)
    if min(linear) < -1e-9 or max(linear) > 1 + 1e-9:
        return None
    return tuple(round(255 * (12.92 * c if c <= 0.0031308 else 1.055 * max(c, 0) ** (1 / 2.4) - 0.055))
                 for c in linear)


# shields' badge-maker (colorsForBackground in lib/badge-renderers.js) draws the message
# text #333 instead of white when the background's brightness is above 0.69, in every
# style. The badge colours keep clear of that switch: saturated orange with white text
# below BADGE_DARK_TEXT_FROM, bright colours with #333 text from there on.
SHIELDS_DARK_TEXT_ABOVE = 0.69
SHIELDS_DARK_TEXT = (0x33, 0x33, 0x33)
WHITE = (0xff, 0xff, 0xff)
BADGE_DARK_TEXT_FROM = 88  # percent; the one value where the text turns from white to #333
BADGE_MIN_BRIGHTNESS = 0.74  # bright colours, #333 text
BADGE_MIN_WHITE_CONTRAST = 3.0  # orange colours, white text (shields' own orange fe7d37 has 2.56)
BADGE_ORANGE_HUES = (34, 56)  # OKLCH hues: orange-red at 0 %, orange just below BADGE_DARK_TEXT_FROM
BADGE_HUES = (25, 136, 150)  # OKLCH hue scale of the bright colours (orange-amber at 88 %, lime below 100 %), green at 100 %


def badge_hue(shown):
    """OKLCH hue of a badge for a displayed percentage."""
    if shown < BADGE_DARK_TEXT_FROM:
        low, high = BADGE_ORANGE_HUES
        return low + (high - low) * shown / BADGE_DARK_TEXT_FROM
    red, top, green = BADGE_HUES
    if shown >= 100:
        return green
    return red + (top - red) * (1 - math.log10(101 - shown) / math.log10(101)) ** 1.2


@functools.lru_cache(maxsize=None)
def badge_color(value):
    """Hex colour (RRGGBB, no '#') for a percentage badge, from the value as the badge displays it.

    Below BADGE_DARK_TEXT_FROM the hue runs from orange-red to orange, as saturated as
    white text at BADGE_MIN_WHITE_CONTRAST allows. From there the OKLCH hue runs on
    through amber and yellow to lime along (1 - log(101 - v) / log(101)) ** 1.2, a
    logarithmic scale of the shortfall that gives 90..99.9 more than half of the hue
    range, so 90, 94, 97, 99 and 99.9 are different hues. Only a displayed 100.000 gets
    the full green. These bright colours take the most chroma inside the sRGB gamut whose
    shields brightness (unrounded) is at least BADGE_MIN_BRIGHTNESS, so shields gives
    them #333 text.
    """
    shown = min(max(float(f"{value:.3f}"), 0), 100)
    hue = badge_hue(shown)

    def widest(lightness):
        low, high = 0.0, 0.4  # chroma; 0.4 lies outside the sRGB gamut at every hue
        for _ in range(25):
            middle = (low + high) / 2
            low, high = (middle, high) if oklch_to_srgb(lightness, middle, hue) else (low, middle)
        return low, oklch_to_srgb(lightness, low, hue)
    # The widest chroma rises with lightness up to the hue's cusp and falls after it,
    # while the brightness keeps rising and the contrast with white keeps falling.
    low, high = 0.4, 1.0
    for _ in range(40):
        left, right = low + (high - low) / 3, high - (high - low) / 3
        low, high = (left, high) if widest(left)[0] < widest(right)[0] else (low, right)
    cusp = low
    dark_text = shown >= BADGE_DARK_TEXT_FROM
    low, high = 0.4, 1.0
    for _ in range(30):
        middle = (low + high) / 2
        rgb = widest(middle)[1]
        if dark_text:
            enough = (rgb[0] * 299 + rgb[1] * 587 + rgb[2] * 114) / 255000 >= BADGE_MIN_BRIGHTNESS
        else:
            enough = contrast(rgb, WHITE) < BADGE_MIN_WHITE_CONTRAST  # too light for white text
        low, high = (low, middle) if enough else (middle, high)
    # Dark text: the cusp or the lowest lightness bright enough. White text: the cusp or the
    # highest lightness still dark enough.
    lightness = max(cusp, high) if dark_text else min(cusp, low)
    return bytes(widest(lightness)[1]).hex()


def update_badge(stats):
    path = ROOT / "README.md"
    content = path.read_text()
    def image(field, label="", alt=None):
        # for-the-badge style; table cells carry no label text, only the percentage.
        value = stats[field]
        return (f"![{alt or f'{value:.3f}%'}](https://img.shields.io/badge/{label}-{value:.3f}%25-"
                f"{badge_color(value)}?style=for-the-badge)")

    badge = ("<!-- matching-badge -->\n"
             + image("fuzzy_coverage_percent", "fuzzy%20code%20match", "fuzzy code match")
             + "<!-- /matching-badge -->")
    # The exact row counts alignment padding after exact functions when the report has it;
    # matching_percent stays the strict function-only number.
    exact = ("matching_with_padding_percent", "byte-identical code, with the padding after it.") \
        if "matching_with_padding_percent" in stats else ("matching_percent", "byte-identical code.")
    # The mapped rows count proven alignment padding between known extents when the
    # report has it; mapping_<kind>_percent stays the symbol-only number.
    padded = "mapping_code_with_padding_percent" in stats
    # Mapped code counts the padding; the named figures do not. Show named as a share of
    # the mapped function bytes alone so that it and the address-named rest add up to 100 %.
    padding = stats["mapping_code_with_padding_percent"] - stats["mapping_code_percent"] if padded else 0
    if "mapping_code_named_percent" in stats:
        named, address_named = stats["mapping_code_named_percent"], stats["mapping_code_address_named_percent"]
        stats = {**stats, "mapping_code_named_share_percent": 100 * named / (named + address_named)
                 if named + address_named else 0}
    # (depth, name, field, meaning); a row without a field is a group heading. The meanings
    # stay short: GitHub squeezes the badge column when the first column's text is long,
    # which scales the badges down. docs/mapping-status.md has the full definitions.
    rows = [(0, "Code", None, None),
            (1, "Fuzzy match", "fuzzy_coverage_percent",
             "objdiff's fuzzy match over all original code, by size "
             "([details](docs/mapping-status.md#what-the-readme-numbers-measure))."),
            (1, "Exact match", *exact),
            (1, "Mapped", "mapping_code_with_padding_percent" if padded else "mapping_code_percent",
             f"known code, with the padding between functions ({padding:.1f}%)." if padded
             else "known code extents.")]
    if "mapping_code_named_percent" in stats:
        share = stats["mapping_code_named_share_percent"]
        rows.append((2, "Named", "mapping_code_named_share_percent",
                     f"mapped functions with real names; {100 - share:.1f}% are `fun_XXXXXXXX`."))
    rows += [(0, "Data", None, None),
             (1, "Mapped file-backed", "mapping_file_backed_data_with_padding_percent" if padded
              else "mapping_file_backed_data_percent",
              "known data in the executable, with its padding." if padded
              else "known data in the executable.")]
    if "reproduced_file_backed_data_percent" in stats:
        rows.append((2, "Reproduced", "reproduced_file_backed_data_percent",
                     "file-backed data we define byte-exactly "
                     "([details](docs/mapping-status.md#reproduced-data))."))
    rows.append((1, "Mapped zero-fill/BSS", "mapping_zero_fill_data_percent", "known zero-initialized storage."))

    def row(depth, name, field, meaning):
        if field is None:
            return f"| **{name}** | |"
        indent = "&nbsp;" * (4 * depth - 2) + "↳ "
        return f"| {indent}**{name}**: {meaning} | {image(field)} |"

    # The non-breaking spaces give the badge column a minimum width that GitHub's table
    # layout keeps; the images alone can shrink to nothing.
    table = ("<!-- mapping-badge -->\n\n| Metric | Coverage" + "&nbsp;" * 10 + " |\n"
             "| --- | --- |\n"
             + "\n".join(row(*entry) for entry in rows)
             + "\n\n<!-- /mapping-badge -->")
    for metric, replacement in (("matching", badge), ("mapping", table)):
        content, count = re.subn(f"<!-- {metric}-badge -->.*?<!-- /{metric}-badge -->",
                                lambda _: replacement, content, flags=re.S)
        if count != 1:
            raise ValueError(f"README must have one {metric}-badge marker pair")
    path.write_text(content)


def display_name(symbol):
    return symbol[1:] if symbol.startswith("_") and not symbol.startswith("__") else symbol


def print_diff(diff, symbol_name):
    for side, label in (("left", "RETAIL"), ("right", "REBUILT")):
        print(label)
        for symbol in diff[side]["symbols"]:
            if symbol["name"] == symbol_name:
                for line in symbol.get("instructions", []):
                    # Insertions/deletions leave an alignment gap on the other side.
                    ins = line.get("instruction")
                    if ins is not None:
                        print(f"  {int(ins['address']):08x}  {ins.get('formatted', '')}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Update matching.json and the README badge")
    parser.add_argument("--function", help="Show one function by linker name, C shorthand or original VA")
    parser.add_argument("--require-match", action="store_true", help="Fail if any selected implemented symbol differs")
    args = parser.parse_args()
    if args.update and args.function:
        parser.error("A filtered comparison cannot update overall progress")
    config = project()
    target = config["target"]
    records = load_symbols()
    selected = records
    if args.function:
        selected = [r for r in records if args.function in
                    (display_name(r["symbol"]), r["symbol"],
                     f"0x{r['address']:08x}" if r["address"] is not None else None)]
        addressed = [r for r in selected if r["address"] is not None]
        selected = addressed or selected
        if len(selected) != 1:
            raise ValueError(f"Expected one function for {args.function!r}; found {len(selected)}")
        if selected[0]["address"] is None:
            raise ValueError(f"{selected[0]['symbol']!r} is recorded as inlined, part of its caller; "
                             "no standalone original address is established. Compare the caller instead")
    pe = retail(target)
    cli, cli_pin = objdiff()
    OUT.mkdir(parents=True, exist_ok=True)
    objects = compiled_objects()
    from tools.candidate_only import load as load_candidate_only
    candidate_only = load_candidate_only(config, objects, records)
    objects = prepare_code_group_objects(objects, load_code_groups(config), records)
    definition_index = DefinitionIndex(objects)
    for unit, (obj, names) in library_objects().items():
        definition_index.add_library(unit, obj, names)
    members, member_proof, migrations = [], None, []
    if "data_members" in config and not args.function:
        if __package__:
            from .data_members import evaluate_members, reviewed_migrations
        else:
            from tools.data_members import evaluate_members, reviewed_migrations
        members, member_proof = evaluate_members(config, objects, records, target, pe)
    if not args.function:
        previous = ROOT / "matching.json"
        if previous.is_file():
            previous_report = json.loads(previous.read_text())
            if "data_members" in config or previous_report.get("data_members"):
                if __package__:
                    from .data_members import reviewed_migrations
                else:
                    from tools.data_members import reviewed_migrations
                migrations = reviewed_migrations(previous_report, members)
    externals = external_symbols(pe, records, objects, definition_index=definition_index)
    bindings = bindings_for(records)
    units, functions, data, inlined = [], [], [], []
    code_components = []
    for record in selected:
        symbol, va = record["symbol"], record["address"]
        if va is None:
            if symbol not in bindings and definition_index.definitions_for(symbol):
                raise ValueError(f"{symbol!r}: compiled standalone definition has only an inlined mapping; "
                                 "no standalone original address is established. Keep the code in its "
                                 "caller, or recover a standalone address and size in config/symbols.csv")
            inlined.append({"name": display_name(symbol), "symbol": symbol, "address": None,
                            "ctype": record["ctype"], "status": "inlined"})
            continue
        if (va, symbol) in externals:
            if args.function:
                raise ValueError(f"{symbol!r} is an external {externals[(va, symbol)]['kind']}; "
                                 "external declarations are excluded from reconstruction matching")
            continue
        definitions = definition_index.definitions_for(symbol)
        kinds = definition_index.kinds_for(symbol)
        if len(kinds) > 1:
            raise ValueError(f"Linker name {symbol!r} identifies both code and data")
        # For unknown symbols, the original code range identifies inventory
        # coverage only. An absent definition can never contribute a match.
        is_function = kinds.pop() if kinds else (
            number(target["text_va"]) <= va < number(target["text_va"]) + number(target["text_size"]))
        if args.function and not is_function:
            raise ValueError(f"{symbol!r} is data, not a function")
        components = [(unit, obj.code_component(symbol)) for unit, obj in definitions]
        if components and any(component is not None for _, component in components):
            if not all(component is not None for _, component in components):
                raise ValueError(f"{symbol!r}: internal EH membership must be validated for every owner")
            if args.function:
                raise ValueError(f"{symbol!r}: internal EH point has no independent extent; compare its parent group")
            code_components.append({"symbol": symbol, "address": f"0x{va:08x}",
                                    "status": "compared in parent group", "target_size": None,
                                    "owners": [{"unit": unit, **component} for unit, component in components]})
            continue
        row = {"name": display_name(symbol), "symbol": symbol,
               "address": f"0x{record['address']:08x}",
               "units": [unit for unit, _ in definitions], "target_size": record["size"],
               "exact_match": False, "objects": []}
        (functions if is_function else data).append(row)
        if not record["size"]:
            row["status"] = "unknown size"
            continue
        if not definitions:
            row["status"] = "not implemented"
            continue
        if candidate_only is not None:
            uncompared = []
            for unit, obj in definitions:
                extract = obj.extract_function if is_function else obj.extract_data
                references = candidate_only.uncompared_references(unit, obj, extract(symbol), va, bindings)
                if references:
                    uncompared.append({"unit": unit, "references": references})
            if uncompared:
                row["status"] = "uncompared candidate-only relocations"
                row["candidate_only_references"] = uncompared
                continue
        for unit, obj in definitions:
            result = {"unit": unit}
            if not is_function:
                result.update(compare_data(pe, obj, symbol, va, record["size"], bindings))
            else:
                original = pe.read_va(va, record["size"])
                base, relocations = relocate_function(obj, symbol, va, bindings)
                if obj.code_group(symbol) is not None:
                    row["region_kind"] = "complete_code_group"
                stem = f"{va:08x}-" + digest((unit + symbol).encode())[:12]
                original_path, base_path = OUT / (stem + ".target.obj"), OUT / (stem + ".base.obj")
                write_function(original_path, symbol, va, original)
                write_function(base_path, symbol, va, base)
                units.append({"name": stem, "target_path": original_path.name, "base_path": base_path.name})
                result.update({"base_size": len(base), "exact_match": original == base,
                               "target_sha256": digest(original), "base_sha256": digest(base),
                               "relocation_count": len(relocations), "comparison_unit": stem})
                if original != base and "diff_class" not in row and diffclass.available():
                    # Optional (capstone): why the first differing copy differs.
                    row.update(diffclass.describe(original, base, va))
                    row["idioms"] = idiomscan.signatures(original, base, va)
            row["objects"].append(result)
        row["exact_match"] = all(result["exact_match"] for result in row["objects"])
        row["status"] = "exact" if row["exact_match"] else "different"
    (OUT / "objdiff.json").write_text(json.dumps({"build_target": False, "build_base": False,
                                                 "units": units}, indent=2) + "\n")
    report = None
    if units:
        report_path = OUT / "report.json"
        run([cli, "report", "generate", "-p", OUT, "-o", report_path])
        report = json.loads(report_path.read_text())
        percentages = {(u["name"], f["name"]): objdiff_percent(f)
                       for u in report["units"] for f in u.get("functions", [])}
        for row in functions:
            for result in row["objects"]:
                key = (result["comparison_unit"], row["symbol"])
                if key not in percentages:
                    raise ValueError(f"objdiff omitted function {row['symbol']}")
                result["objdiff_percent"] = percentages[key]
            scores = [r["objdiff_percent"] for r in row["objects"]]
            if scores:
                row["objdiff_percent"] = min(scores)
    for row in functions + data:
        similarity = row.get("objdiff_percent")
        score = f"; objdiff {similarity:.2f}%" if similarity is not None else ""
        print(f"{row['status']:16} {row['name']:32} {row['target_size']} bytes{score}")
    if externals:
        print(f"Excluded {len(externals)} imported address slots/thunks from reconstruction matching.")
    if inlined:
        print(f"Recorded {len(inlined)} inlined symbols as part of their callers; "
              "no standalone address or byte coverage is assigned.")
    inputs = [ROOT / "Makefile", *sorted((ROOT / "src").rglob("*.h")),
              *sorted((ROOT / "config").glob("*")),
              *sorted((ROOT / "tools").glob("*.py")), *sorted((ROOT / "tools").glob("*.def")),
              *sorted((ROOT / "src").rglob("*.cpp")),
              *sorted((ROOT / "src").rglob("*.c"))]
    inputs.extend(dependency_files(ROOT))
    if config.get("code_groups") and ROOT / config["code_groups"] not in inputs:
        inputs.append(ROOT / config["code_groups"])
    result = {"format_version": 2, "target_sha256": target["sha256"],
              "compiler": load_json(ROOT / "config/toolchain.json")["version"],
              "objdiff_version": cli_pin["version"],
              "inputs": {p.relative_to(ROOT).as_posix(): sha256(p) for p in inputs},
              "objects": {object_path(unit).relative_to(ROOT).as_posix(): digest(obj.data)
                          for unit, obj in sorted(objects.items())},
              "functions": functions, "data": data, "externals": list(externals.values()),
              "inlined": inlined}
    if candidate_only is not None:
        result["candidate_only"] = candidate_only.rows
    if code_components:
        result["code_components"] = code_components
    if not args.function:
        padding = padding_runs(functions, target, pe.read_va, externals.values())
        result["progress"] = progress(functions, target, padding, data)
        result["text_coverage"] = text_coverage(functions, target, externals.values(), padding, data)
        result["progress"].update(objdiff_progress(report, functions))
        member_ranges = [(number(r["address"]), r["target_size"]) for r in members
                         if r["target_size"] is not None]
        result["progress"].update(mapping_progress(records, target, member_ranges))
        known = [(r["address"], r["size"]) for r in records if r["address"] is not None and r["size"]]
        data_padding = data_padding_runs(known + member_ranges, pe.sections, number(target["image_base"]), pe.read_va)
        result["progress"].update(section_mapping_progress(
            records, target, pe.sections, member_ranges,
            [(number(run["address"]), run["size"]) for run in padding] +
            [(run["address"], run["size"]) for run in data_padding]))
        result["progress"].update(code_name_progress(records, target))
        classes, disagreements = reproduced_inputs(records, member_ranges,
                                                   [(run["address"], run["size"]) for run in data_padding], data)
        result["progress"].update(reproduced_data(data_storage(pe.sections, number(target["image_base"])), classes))
        result["progress"]["reproduced_data_comparison"] = {
            "authority": "tools/datadiff.py (slot by slot, pointers by name) over the blob comparison of data rows",
            **disagreements}
        if "data_members" in config:
            result["data_members"] = members
            result["data_member_proof"] = member_proof
            result["data_member_migrations"] = migrations
            result["progress"]["matched_data_region_bytes"] = covered_bytes(
                [(number(r["address"]), r["target_size"]) for r in data + members
                 if r["exact_match"] and r["target_size"] is not None])
    result = repository_paths(result)
    (OUT / "summary.json").write_text(json.dumps(result, indent=2) + "\n")
    if args.update:
        update_badge(result["progress"])
        (ROOT / "matching.json").write_text(json.dumps(result, indent=2) + "\n")
        stats = result["progress"]
        print(f"Objdiff fuzzy: {stats['objdiff_fuzzy_percent']:.3f}% across "
              f"{stats['objdiff_total_code']} compared original code bytes")
        print(f"Fuzzy code coverage: {stats['fuzzy_coverage_percent']:.3f}% of {stats['fuzzy_denominator_bytes']} "
              "original code bytes (.text without alignment padding and text data)")
        print(f"Exact code: {stats['matched_code_bytes']}/{stats['text_bytes']} bytes "
              f"({stats['matching_percent']:.3f}% of original .text)")
        print(f"Exact code with padding: {stats['matched_code_with_padding_bytes']}/{stats['text_bytes']} bytes "
              f"({stats['matching_with_padding_percent']:.3f}%; {stats['matched_padding_bytes']} of "
              f"{stats['padding_bytes']} alignment padding bytes follow exact functions)")
        for kind, label in (("code", "code"), ("file_backed_data", "file-backed data"),
                            ("zero_fill_data", "zero-fill/BSS")):
            print(f"Mapped {label}: {stats[f'mapping_{kind}_bytes']}/"
                  f"{stats[f'mapping_{kind}_total_bytes']} bytes "
                  f"({stats[f'mapping_{kind}_percent']:.3f}% of original section storage; "
                  f"{stats.get(f'mapping_{kind}_with_padding_percent', 0.0):.3f}% with proven alignment padding)")
        print(f"Mapped code under real names: {stats['mapping_code_named_bytes']} bytes "
              f"({stats['mapping_code_named_percent']:.3f}%); under address names: "
              f"{stats['mapping_code_address_named_bytes']} bytes ({stats['mapping_code_address_named_percent']:.3f}%, "
              f"{stats['mapping_code_address_named_functions']} functions)")
        for kind, label in (("file_backed_data", "file-backed data"), ("zero_fill_data", "zero-fill/BSS")):
            parts = ", ".join(f"{name} {stats[f'reproduced_{kind}_{name}_bytes']}" for name, _ in REPRODUCED_CLASSES)
            print(f"Reproduced {label}: {stats[f'reproduced_{kind}_bytes']}/{stats[f'reproduced_{kind}_total_bytes']} "
                  f"bytes ({stats[f'reproduced_{kind}_percent']:.3f}%; {parts})")
    if args.function and units:
        for unit in units:
            diff_path = OUT / (unit["name"] + ".diff.json")
            run([cli, "diff", "-1", OUT / unit["target_path"],
                 "-2", OUT / unit["base_path"], functions[0]["symbol"], "-o", diff_path])
            diff = json.loads(diff_path.read_text())
            print_diff(diff, functions[0]["symbol"])
    print("Local comparison objects and reports: build/objdiff/")
    return int(args.require_match and any(r["units"] and not r["exact_match"] for r in functions + data))


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"match: {error}", file=sys.stderr)
        sys.exit(2)
