"""Keep native objdiff similarity separate from exact original code coverage."""
from pathlib import Path
from contextlib import ExitStack, redirect_stdout
import colorsys
import io
import json
import math
import os
import re
import shutil
import struct
import subprocess
import tempfile
from types import SimpleNamespace
import unittest
from xml.etree import ElementTree
from unittest.mock import patch

from tools import match
from tools.binary import write_function


class ProgressTests(unittest.TestCase):
    target = {"text_va": "0x401000", "text_size": "0x1000"}

    def row(self, address=0x401000, size=16, exact=True):
        return {"name": "f", "address": hex(address), "target_size": size,
                "exact_match": exact, "objdiff_percent": 100.0}

    def test_unmapped_code_stays_in_denominator(self):
        stats = match.progress([self.row()], self.target)
        self.assertEqual(stats["matched_code_bytes"], 16)
        self.assertEqual(stats["text_bytes"], 4096)
        self.assertEqual(stats["matching_percent"], 100 * 16 / 4096)

    def test_similarity_does_not_count_as_matching(self):
        stats = match.progress([self.row(exact=False)], self.target)
        self.assertEqual(stats["matching_percent"], 0)
        self.assertEqual(stats["mapped_code_bytes"], 16)

    def test_fuzzy_coverage_without_padding_uses_original_text_denominator(self):
        row = self.row(exact=False)
        row["objects"] = [{"objdiff_percent": 50.0}]
        stats = match.progress([row], self.target)
        self.assertEqual(stats["fuzzy_coverage_percent"], 100 * 8 / 4096)
        self.assertEqual(stats["matching_percent"], 0)

    def test_fuzzy_denominator_excludes_padding_and_text_data_only(self):
        row = self.row(exact=False)
        row["objects"] = [{"objdiff_percent": 50.0}]
        padding = [{"address": 0x401010, "size": 6, "exact": False}]
        data = [{"address": "0x401100", "target_size": 90}, {"address": "0x403000", "target_size": 50}]
        stats = match.progress([row], self.target, padding, data)
        self.assertEqual(stats["fuzzy_denominator_bytes"], 4096 - 6 - 90)
        self.assertEqual(stats["fuzzy_text_data_bytes"], 90)
        self.assertEqual(stats["fuzzy_coverage_percent"], 100 * 8 / (4096 - 6 - 90))
        self.assertEqual(stats["matching_with_padding_percent"], 100 * 0 / 4096)

    def test_fuzzy_aliases_count_once_at_least_similar_score(self):
        first = self.row(exact=False)
        first["objects"] = [{"objdiff_percent": 80.0}]
        alias = self.row(exact=False)
        alias["objects"] = [{"objdiff_percent": 60.0}]
        overlap = self.row(address=0x401008, exact=False)
        overlap["objects"] = [{"objdiff_percent": 40.0}]
        stats = match.progress([first, alias, overlap], self.target)
        self.assertAlmostEqual(stats["fuzzy_coverage_percent"],
                               100 * (8 * .6 + 8 * .4 + 8 * .4) / 4096)

    def test_aliases_and_overlaps_are_counted_once(self):
        stats = match.progress([self.row(), self.row(), self.row(0x401008)], self.target)
        self.assertEqual(stats["mapped_code_bytes"], 24)
        self.assertEqual(stats["matched_code_bytes"], 24)

    def test_unknown_boundaries_do_not_contribute(self):
        stats = match.progress([self.row(size=None, exact=False)], self.target)
        self.assertEqual(stats["mapped_code_bytes"], 0)
        self.assertEqual(stats["matched_functions"], 0)

    def test_outside_code_section_is_rejected(self):
        for address in (0x400fff, 0x401fff, 0x402000):
            with self.subTest(address=address), self.assertRaisesRegex(ValueError, "outside original"):
                match.progress([self.row(address)], self.target)

    def test_fuzzy_badge_and_coverage_table(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            marker = "<!-- matching-badge -->old<!-- /matching-badge -->"
            mapping_marker = "<!-- mapping-badge -->old<!-- /mapping-badge -->"
            build_badge = "[![build](https://example.com/build.svg)](https://example.com/build)"
            path.write_text("before\n" + marker + " " + build_badge + "\n\n" + mapping_marker + "\nafter\n")
            stats = {"fuzzy_coverage_percent": 1.29352, "matching_percent": 0.012733,
                     "mapping_code_percent": 24.9999, "mapping_file_backed_data_percent": 25,
                     "mapping_zero_fill_data_percent": 75}
            match.update_badge(stats)
            updated = path.read_text()
            self.assertTrue(updated.startswith("before\n"))
            self.assertTrue(updated.endswith("\nafter\n"))
            self.assertIn(badge_img("fuzzy-code-match", "1.294%", "fuzzy code match") + "<!-- /matching-badge -->",
                          updated)
            self.assertEqual(updated.count('<img src="docs/badges/'), 6)
            self.assertNotIn("img.shields.io/badge/", updated)
            badges = sorted(path.name for path in (Path(temp) / "docs/badges").iterdir())
            self.assertEqual(badges, ["exact.svg", "fuzzy-code-match.svg", "fuzzy.svg", "mapped-bss.svg",
                                      "mapped-code.svg", "mapped-data.svg"])
            self.assertIn(">0.013%</text>", (Path(temp) / "docs/badges/exact.svg").read_text())
            self.assertIn("| &nbsp;&nbsp;↳ **Fuzzy match**: objdiff's fuzzy match over all original code, by "
                          "size ([details](docs/mapping-status.md#what-the-readme-numbers-measure)). | ", updated)
            self.assertIn(build_badge, updated)
            self.assertIn("| Metric | Coverage" + "&nbsp;" * 10 + " |" + "\n| --- | --- |\n", updated)
            # Short meanings keep the table narrow enough that GitHub does not shrink the badges.
            for line in updated.splitlines():
                if line.startswith("| &nbsp;"):
                    meaning = line.split(" | ")[0][2:].replace("&nbsp;", " ").replace("**", "")
                    self.assertLessEqual(len(re.sub(r"\[([^]]*)\]\([^)]*\)", r"\1", meaning)), 85, line)
            self.assertIn("| **Code** | |\n| &nbsp;&nbsp;↳ **Fuzzy match**", updated)
            self.assertIn("| &nbsp;&nbsp;↳ **Exact match**: byte-identical code. | "
                          f"{badge_img('exact', '0.013%')} |", updated)
            self.assertIn("| &nbsp;&nbsp;↳ **Mapped**: known code extents. | "
                          f"{badge_img('mapped-code', '25.000%')} |", updated)
            self.assertIn("| **Data** | |\n| &nbsp;&nbsp;↳ **Mapped file-backed**: known data in the "
                          f"executable. | {badge_img('mapped-data', '25.000%')} |",
                          updated)
            self.assertIn("| &nbsp;&nbsp;↳ **Mapped zero-fill/BSS**: known zero-initialized storage. | "
                          f"{badge_img('mapped-bss', '75.000%')} |", updated)
            self.assertEqual(sum(line.startswith("| &nbsp;&nbsp;↳ ") for line in updated.splitlines()), 5)
            self.assertEqual(sum("<img " in line for line in updated.splitlines()), 6)
            self.assertTrue(updated.splitlines()[1].startswith("<!-- matching-badge -->"))
            match.update_badge(stats)
            self.assertEqual(path.read_text(), updated)
            for content in ("missing", marker + marker):
                path.write_text(content)
                with self.assertRaisesRegex(ValueError, "one matching-badge"):
                    match.update_badge(stats)
                self.assertEqual(path.read_text(), content)

    def test_badge_colors_follow_the_displayed_value(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("<!-- matching-badge -->old<!-- /matching-badge -->\n\n"
                            "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
            for value in (0, 60.5, 99.9996, 100):
                with self.subTest(value=value):
                    match.update_badge({"fuzzy_coverage_percent": value, "matching_percent": value,
                                        "mapping_code_percent": value, "mapping_file_backed_data_percent": value,
                                        "mapping_zero_fill_data_percent": value})
                    color = match.badge_color(value)
                    for svg in (Path(temp) / "docs/badges").glob("*.svg"):
                        self.assertIn(f'fill="#{color}"', svg.read_text())
                        self.assertIn(f">{value:.3f}%</text>", svg.read_text())

    @staticmethod
    def hue(value):
        rgb = bytes.fromhex(match.badge_color(value))
        return colorsys.rgb_to_hls(*(channel / 255 for channel in rgb))[0] * 360

    def test_badge_hue_rises_monotonically_to_full_green_only_at_100(self):
        values = [step / 10 for step in range(1001)] + [99.95, 99.99, 99.999, 99.9994]
        hues = [self.hue(value) for value in sorted(values)]
        self.assertEqual(hues, sorted(hues))
        key = [self.hue(value) for value in (0, 60, 90, 94, 97, 99, 99.9, 100)]
        self.assertEqual(key, sorted(set(key)))
        full = match.badge_color(100)
        self.assertEqual(match.badge_color(99.9996), full)  # displayed as 100.000
        self.assertEqual([value for value in values if match.badge_color(value) == full], [100.0])
        self.assertGreater(self.hue(100) - self.hue(99.999), 30)

    def test_badge_colors_near_the_top_are_distinct(self):
        def distance(a, b):
            a, b = bytes.fromhex(match.badge_color(a)), bytes.fromhex(match.badge_color(b))
            return math.dist(a, b)
        values = (60, 90, 94, 97, 99, 99.9, 100)
        for low, high in zip(values, values[1:]):
            with self.subTest(low=low, high=high):
                self.assertGreaterEqual(distance(low, high), 25)
        self.assertGreater(distance(99.9, 100), 80)
        self.assertGreater(distance(60, 95), 80)

    def test_badge_text_stays_readable(self):
        for step in range(1001):
            with self.subTest(value=step / 10):
                rgb = bytes.fromhex(match.badge_color(step / 10))
                self.assertGreaterEqual(match.contrast(rgb, match.BADGE_TEXT), 4.5)
        self.assertEqual(match.contrast((255, 255, 255), (0, 0, 0)), 21)

    def test_badge_low_end_is_saturated_orange_red(self):
        for value in (0, 25, 50, 75):
            with self.subTest(value=value):
                r, g, b = bytes.fromhex(match.badge_color(value))
                self.assertEqual(r, 255)
                self.assertLess(b, 0x20)
        self.assertLess(self.hue(0), 20)
        self.assertTrue(25 < self.hue(75) < 40)


def badge_img(name, message, label=""):
    width = re.search(r'width="(\d+)"', match.badge_svg(message, "000000", label)).group(1)
    alt = f"{label}: {message}" if label else message
    return f'<img src="docs/badges/{name}.svg" alt="{alt}" width="{width}" height="28">'


class BadgeSvgTests(unittest.TestCase):
    def test_layout_follows_shields_for_the_badge(self):
        svg = match.badge_svg("94.207%", "ffc200", "fuzzy code match")
        root = ElementTree.fromstring(svg)
        ns = "{http://www.w3.org/2000/svg}"
        self.assertEqual((root.get("width"), root.get("height")), ("232", "28"))
        self.assertEqual(root.get("aria-label"), "FUZZY CODE MATCH: 94.207%")
        self.assertEqual(root.find(ns + "title").text, "FUZZY CODE MATCH: 94.207%")
        label_rect, message_rect = root.iter(ns + "rect")
        self.assertEqual((label_rect.get("width"), label_rect.get("fill")), ("148", "#555"))
        self.assertEqual((message_rect.get("x"), message_rect.get("width"), message_rect.get("fill")),
                         ("148", "84", "#ffc200"))
        label, message = root.iter(ns + "text")
        # badge-maker: (anafanafo width | 0) + 1.25 per character, centred, at ten times the scale.
        self.assertEqual((label.text, label.get("textLength"), label.get("x"), label.get("fill")),
                         ("FUZZY CODE MATCH", "1240", "740", None))
        self.assertEqual((message.text, message.get("textLength"), message.get("x"), message.get("fill"),
                          message.get("font-weight")), ("94.207%", "597.5", "1900", "#222222", "bold"))

    def test_message_only_badge(self):
        root = ElementTree.fromstring(match.badge_svg("100.000%", match.badge_color(100)))
        ns = "{http://www.w3.org/2000/svg}"
        self.assertEqual(root.get("width"), "92")
        (rect,) = root.iter(ns + "rect")
        self.assertEqual((rect.get("x"), rect.get("fill")), (None, "#4fff88"))
        (text,) = root.iter(ns + "text")
        self.assertEqual((text.text, text.get("x"), text.get("textLength")), ("100.000%", "460", "680"))

    def test_badges_are_deterministic(self):
        stats = {"fuzzy_coverage_percent": 94.20712, "matching_percent": 60.5449,
                 "mapping_code_percent": 99.99988, "mapping_file_backed_data_percent": 95.6388,
                 "mapping_zero_fill_data_percent": 97.7124}
        readme = ("<!-- matching-badge -->old<!-- /matching-badge -->\n\n"
                  "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
        first = match.render_badges(stats, readme)
        match.badge_color.cache_clear()
        self.assertEqual(match.render_badges(dict(stats), readme), first)
        for name, svg in first[1].items():
            with self.subTest(name=name):
                ElementTree.fromstring(svg)
                self.assertTrue(svg.endswith("</svg>\n"))


class TextCoverageTests(unittest.TestCase):
    target = {"text_va": "0x401000", "text_size": "0x1000"}

    def row(self, address=0x401000, size=16, exact=True):
        return {"name": "f", "address": hex(address), "target_size": size,
                "exact_match": exact, "objects": []}

    def test_categories_partition_original_text_and_preserve_strict_denominator(self):
        rows = [self.row(), self.row(0x401008, exact=False)]
        glue = [{"kind": "import thunk", "address": "0x401040", "size": 6}]
        stats = match.text_coverage(rows, self.target, glue)
        self.assertEqual(stats["exact_reconstruction_bytes"], 16)
        self.assertEqual(stats["mapped_not_exact_bytes"], 8)
        self.assertEqual(stats["excluded_import_glue_bytes"], 6)
        self.assertEqual(stats["unclassified_text_bytes"], 4096 - 24 - 6)
        self.assertEqual(stats["strict_original_text_exact_percent"],
                         match.progress(rows, self.target)["matching_percent"])
        self.assertEqual(stats["strict_original_text_denominator_bytes"], 4096)
        self.assertEqual(stats["reconstruction_scope_denominator_bytes"], 4090)
        self.assertEqual(stats["mapped_scope_denominator_bytes"], 24)

    def test_duplicate_aliases_and_import_slots_do_not_inflate_categories(self):
        row = self.row()
        thunk = {"kind": "import thunk", "address": "0x401040", "size": 6}
        slot = {"kind": "import slot", "address": "0x405000", "size": 4}
        stats = match.text_coverage([row, row], self.target, [thunk, thunk, slot])
        self.assertEqual(stats["mapped_reconstruction_bytes"], 16)
        self.assertEqual(stats["excluded_import_glue_bytes"], 6)

    def test_unknown_boundaries_and_padding_stay_unclassified(self):
        stats = match.text_coverage([self.row(size=None, exact=False)], self.target)
        self.assertEqual(stats["unclassified_text_bytes"], 4096)
        self.assertEqual(stats["mapped_scope_exact_percent"], 0)
        self.assertIn("unproven padding", stats["qualification"])

    def test_data_inside_text_is_classified_without_function_credit(self):
        rows = [self.row()]
        tables = [self.row(0x401100, 64), self.row(0x401200, 32, exact=False), self.row(0x405000, 8)]
        stats = match.text_coverage(rows, self.target, (), (), tables)
        self.assertEqual(stats["text_data_bytes"], 96)
        self.assertEqual(stats["exact_text_data_bytes"], 64)
        self.assertEqual(stats["unclassified_text_bytes"], 4096 - 16 - 96)
        self.assertEqual(stats["exact_reconstruction_bytes"], 16)
        with self.assertRaisesRegex(ValueError, "overlaps"):
            match.text_coverage(rows, self.target, (), (), [self.row(0x401008, 16)])

    def test_invalid_or_overlapping_categories_are_rejected(self):
        for address in (0x400fff, 0x401fff, 0x402000):
            with self.subTest(address=address), self.assertRaisesRegex(ValueError, "outside original"):
                match.text_coverage([self.row(address)], self.target)
        with self.assertRaisesRegex(ValueError, "overlap"):
            match.text_coverage([self.row()], self.target,
                                [{"kind": "import thunk", "address": "0x401008", "size": 6}])

    def test_report_emits_additive_categories_without_changing_legacy_progress(self):
        result, _ = InlinedInventoryTests().run_match([])
        stats = result["text_coverage"]
        self.assertEqual(stats["original_text_bytes"], 4096)
        self.assertEqual(stats["unclassified_text_bytes"], 4096)
        self.assertEqual(stats["strict_original_text_exact_percent"],
                         result["progress"]["matching_percent"])


class PaddingTests(unittest.TestCase):
    target = {"text_va": "0x401000", "text_size": "0x1000"}

    def row(self, address, size, exact=True):
        return {"name": "f", "address": hex(address), "target_size": size, "exact_match": exact}

    def reader(self, image):
        return lambda va, size: bytes(image[va - 0x401000:va - 0x401000 + size])

    def test_int3_and_nop_fill_between_functions_is_padding_exact_after_exact_function(self):
        image = bytearray(b"\xc3" * 0x1000)
        image[0x0c:0x10] = b"\xcc" * 4
        image[0x1d:0x20] = b"\x8d\x49\x00"
        rows = [self.row(0x401000, 12), self.row(0x401010, 13, exact=False), self.row(0x401020, 16)]
        runs = match.padding_runs(rows, self.target, self.reader(image))
        self.assertEqual(runs, [{"address": 0x40100c, "size": 4, "exact": True},
                                {"address": 0x40101d, "size": 3, "exact": False}])
        stats = match.progress(rows, self.target, runs)
        self.assertEqual(stats["matched_code_bytes"], 28)
        self.assertEqual(stats["matched_padding_bytes"], 4)
        self.assertEqual(stats["matched_code_with_padding_bytes"], 32)
        self.assertEqual(stats["padding_bytes"], 7)
        self.assertEqual(stats["matching_percent"], 100 * 28 / 4096)
        self.assertEqual(stats["matching_with_padding_percent"], 100 * 32 / 4096)
        coverage = match.text_coverage(rows, self.target, (), runs)
        self.assertEqual(coverage["alignment_padding_bytes"], 7)
        self.assertEqual(coverage["exact_alignment_padding_bytes"], 4)
        self.assertEqual(coverage["unclassified_text_bytes"], 4096 - 41)
        self.assertEqual(coverage["unclassified_non_padding_text_bytes"], 4096 - 48)
        self.assertEqual(coverage["strict_original_text_exact_percent"], stats["matching_percent"])

    def test_code_long_unaligned_or_unbounded_gaps_are_not_padding(self):
        image = bytearray(b"\xcc" * 0x1000)
        image[0x0d] = 0xc3  # a byte of code in the gap
        cases = [
            [self.row(0x401000, 12), self.row(0x401010, 16)],         # code inside the fill
            [self.row(0x401020, 4), self.row(0x401040, 16)],          # 28 bytes: longer than alignment
            [self.row(0x401050, 4), self.row(0x401057, 16)],          # next start not aligned
            [self.row(0x401050, 4), self.row(0x40105c, 16)],          # 8 bytes: not shorter than 4-alignment
            [self.row(0x401070, 4)],                                  # no following extent
        ]
        for rows in cases:
            with self.subTest(rows=rows):
                self.assertEqual(match.padding_runs(rows, self.target, self.reader(image)), [])

    def test_fill_before_a_less_aligned_start_is_padding_shorter_than_its_alignment(self):
        image = bytearray(b"\xcc" * 0x100)
        rows = [self.row(0x401000, 0x51), self.row(0x401054, 16)]  # 3 bytes before a 4-aligned start
        self.assertEqual(match.padding_runs(rows, self.target, self.reader(image)),
                         [{"address": 0x401051, "size": 3, "exact": True}])

    def test_padding_after_import_glue_or_mixed_aliases_is_not_exact(self):
        image = bytearray(b"\xcc" * 0x1000)
        thunk = {"kind": "import thunk", "address": "0x401000", "size": 6}
        runs = match.padding_runs([self.row(0x401010, 16)], self.target, self.reader(image), [thunk])
        self.assertEqual(runs, [{"address": 0x401006, "size": 10, "exact": False}])
        rows = [self.row(0x401000, 12), self.row(0x401004, 8, exact=False), self.row(0x401010, 4)]
        self.assertEqual(match.padding_runs(rows, self.target, self.reader(image)),
                         [{"address": 0x40100c, "size": 4, "exact": False}])

    def test_fillers_must_be_whole(self):
        self.assertTrue(match.is_padding(b"\xcc\x8b\xff\x8d\xa4\x24\x00\x00\x00\x00"))
        self.assertFalse(match.is_padding(b"\x8d\x49"))
        self.assertFalse(match.is_padding(b""))

    def test_badge_uses_padding_inclusive_exact_when_reported(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("<!-- matching-badge -->old<!-- /matching-badge -->\n\n"
                            "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
            match.update_badge({"fuzzy_coverage_percent": 80, "matching_percent": 30,
                                "matching_with_padding_percent": 31.5, "mapping_code_percent": 90,
                                "mapping_file_backed_data_percent": 40, "mapping_zero_fill_data_percent": 5})
            text = path.read_text()
            self.assertIn("**Exact match**: byte-identical code, with the padding after it. | "
                          f"{badge_img('exact', '31.500%')} |", text)
            self.assertNotIn("30.000", text)


class DataPaddingTests(unittest.TestCase):
    sections = [{"name": ".text", "virtual_address": 0x1000, "virtual_size": 0x1000, "raw_size": 0x1000,
                 "characteristics": 0x60000020},
                {"name": ".rdata", "virtual_address": 0x2000, "virtual_size": 0x100, "raw_size": 0x100,
                 "characteristics": 0x40000040},
                {"name": ".data", "virtual_address": 0x3000, "virtual_size": 0x1000, "raw_size": 0x100,
                 "characteristics": 0xc0000040}]

    def reader(self, image):
        return lambda va, size: bytes(image.get(va + i, 0) for i in range(size))

    def test_zero_fill_before_an_aligned_extent_is_padding(self):
        rows = [(0x402000, 5), (0x402008, 4), (0x40200c, 2), (0x402010, 4)]
        runs = match.data_padding_runs(rows, self.sections, 0x400000, self.reader({}))
        self.assertEqual(runs, [{"address": 0x402005, "size": 3}, {"address": 0x40200e, "size": 2}])

    def test_nonzero_long_unbounded_or_zero_fill_storage_is_not_padding(self):
        cases = [
            ([(0x402000, 5), (0x402008, 4)], {0x402006: 1}),   # a nonzero byte in the gap
            ([(0x402000, 4), (0x402008, 4)], {}),              # four bytes: could be an object
            ([(0x402000, 5), (0x402007, 4)], {}),              # next start not aligned
            ([(0x402000, 5)], {}),                             # nothing follows
            ([(0x403200, 5), (0x403208, 4)], {}),              # zero-fill storage, not file bytes
            ([(0x401000, 5), (0x401008, 4)], {}),              # code
        ]
        for rows, image in cases:
            with self.subTest(rows=rows):
                self.assertEqual(match.data_padding_runs(rows, self.sections, 0x400000, self.reader(image)), [])

    def test_mapped_badges_count_padding_when_reported(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("<!-- matching-badge -->old<!-- /matching-badge -->\n\n"
                            "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
            match.update_badge({"fuzzy_coverage_percent": 80, "matching_percent": 30,
                                "mapping_code_percent": 90, "mapping_code_with_padding_percent": 92.5,
                                "mapping_file_backed_data_percent": 40,
                                "mapping_file_backed_data_with_padding_percent": 41,
                                "mapping_zero_fill_data_percent": 5, "mapping_zero_fill_data_with_padding_percent": 5})
            text = path.read_text()
            self.assertIn("**Mapped**: known code, with the padding between functions (2.5%). | "
                          f"{badge_img('mapped-code', '92.500%')} |", text)
            self.assertIn("**Mapped file-backed**: known data in the executable, with its padding. "
                          f"| {badge_img('mapped-data', '41.000%')} |", text)
            self.assertNotIn("90.000", text)

    def test_named_and_reproduced_rows_nest_under_their_parents(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("<!-- matching-badge -->old<!-- /matching-badge -->\n\n"
                            "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
            match.update_badge({"fuzzy_coverage_percent": 80, "matching_percent": 30,
                                "mapping_code_percent": 90, "mapping_code_named_percent": 85.5,
                                "mapping_code_address_named_percent": 4.5,
                                "mapping_file_backed_data_percent": 40, "reproduced_file_backed_data_percent": 20,
                                "mapping_zero_fill_data_percent": 5})
            lines = path.read_text().splitlines()
            start = lines.index("| Metric | Coverage" + "&nbsp;" * 10 + " |")
            self.assertEqual([line.split("**")[1] for line in lines[start + 2:start + 11]],
                             ["Code", "Fuzzy match", "Exact match", "Mapped", "Named", "Data",
                              "Mapped file-backed", "Reproduced", "Mapped zero-fill/BSS"])
            # Named is a share of the mapped function bytes: 85.5 / (85.5 + 4.5), padding left out.
            self.assertIn("| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;↳ **Named**: mapped functions with real names; 5.0% "
                          "are `fun_XXXXXXXX`. | "
                          f"{badge_img('named-code', '95.000%')} |", lines)
            self.assertNotIn("85.500", "\n".join(lines))
            self.assertTrue(lines[start + 9].startswith("| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;↳ **Reproduced**: "))
            self.assertTrue(lines[start + 9].endswith(
                f"| {badge_img('reproduced-data', '20.000%')} |"))


class ReproducedDataTests(unittest.TestCase):
    def test_each_byte_counts_in_its_strongest_class(self):
        storage = {"file_backed_data": [(0x100, 0x140)], "zero_fill_data": [(0x140, 0x180)]}
        classes = {"exact": [(0x100, 8)], "library": [(0x104, 12)], "different_padding": [(0x110, 4)],
                   "different": [(0x114, 4)], "undefined": [(0x100, 0x20), (0x140, 0x10)],
                   "padding": [(0x120, 2)]}
        result = match.reproduced_data(storage, classes)
        self.assertEqual([result[f"reproduced_file_backed_data_{name}_bytes"] for name, _ in match.REPRODUCED_CLASSES],
                         [8, 8, 4, 4, 8, 2, 30])
        self.assertEqual(result["reproduced_file_backed_data_bytes"], 16)
        self.assertEqual(result["reproduced_file_backed_data_percent"], 25.0)
        self.assertEqual(result["reproduced_zero_fill_data_undefined_bytes"], 16)
        self.assertEqual(result["reproduced_zero_fill_data_unidentified_bytes"], 48)

    def test_address_named_code_is_reported_apart(self):
        target = {"text_va": "0x401000", "text_size": "0x100"}
        records = [{"address": 0x401000, "size": 0x40, "symbol": "_NuRand"},
                   {"address": 0x401040, "size": 0x20, "symbol": "_fun_00401040"},
                   {"address": 0x500000, "size": 4, "symbol": "_dat_00500000"}]
        result = match.code_name_progress(records, target)
        self.assertEqual((result["mapping_code_named_bytes"], result["mapping_code_address_named_bytes"],
                          result["mapping_code_address_named_functions"]), (0x40, 0x20, 1))


class ObjdiffProgressTests(unittest.TestCase):
    def functions(self, copies=1):
        return [{"target_size": 16, "objects": [{} for _ in range(copies)]},
                {"target_size": 1000, "objects": []},
                {"target_size": None, "objects": []}]

    def test_native_metric_is_preserved_with_its_compared_code_denominator(self):
        for total in (16, "16"):
            with self.subTest(total=total):
                report = {"measures": {"fuzzy_match_percent": 64.125, "total_code": total}}
                self.assertEqual(match.objdiff_progress(report, self.functions()),
                                 {"objdiff_fuzzy_percent": 64.125, "objdiff_total_code": 16})

    def test_each_comparison_copy_counts_toward_native_denominator(self):
        report = {"measures": {"fuzzy_match_percent": 75.0, "total_code": "32"}}
        self.assertEqual(match.objdiff_progress(report, self.functions(copies=2))["objdiff_total_code"], 32)
        with self.assertRaisesRegex(ValueError, "differs from compared original extents 16"):
            match.objdiff_progress(report, self.functions())

    def test_omitted_proto_scalar_means_zero_for_aggregate_and_function(self):
        self.assertEqual(match.objdiff_percent({"name": "_different"}), 0.0)
        report = {"measures": {"total_code": "16"}}
        self.assertEqual(match.objdiff_progress(report, self.functions())["objdiff_fuzzy_percent"], 0.0)

    def test_no_comparison_objects_has_zero_fuzzy_progress(self):
        expected = {"objdiff_fuzzy_percent": 0.0, "objdiff_total_code": 0}
        self.assertEqual(match.objdiff_progress(None, self.functions(copies=0)), expected)
        self.assertEqual(match.objdiff_progress({"measures": {}}, []), expected)

    def test_missing_or_invalid_measures_are_rejected_when_comparisons_exist(self):
        for report in (None, {}, [], {"measures": None}, {"measures": []}, {"measures": 1}):
            with self.subTest(report=report), self.assertRaisesRegex(ValueError, "contain measures"):
                match.objdiff_progress(report, self.functions())

    def test_invalid_native_percentages_are_rejected(self):
        for value in (None, True, "75", -1, 101, 10 ** 1000, float("nan"), float("inf")):
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, "finite percentage"):
                match.objdiff_progress({"measures": {"total_code": "16", "fuzzy_match_percent": value}},
                                       self.functions())

    def test_invalid_or_inconsistent_native_totals_are_rejected(self):
        for total in (None, True, -1, 16.0, "", "-16", "0x10", "16.0", [], " 16"):
            with self.subTest(total=total), self.assertRaisesRegex(ValueError, "nonnegative integer"):
                match.objdiff_progress({"measures": {"total_code": total}}, self.functions())
        for measures in ({}, {"total_code": "15"}, {"total_code": "17"}):
            with self.subTest(measures=measures), self.assertRaisesRegex(ValueError, "compared original extents"):
                match.objdiff_progress({"measures": measures}, self.functions())


class ObjdiffIntegrationTests(unittest.TestCase):
    def test_instruction_gaps_render_on_either_side_without_game_files(self):
        if not (match.ROOT / "build/tools/bin/objdiff-cli").is_file():
            self.skipTest("Run make setup to include the objdiff integration check (required in CI)")
        cli, _ = match.objdiff()
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            short, longer = root / "short.obj", root / "longer.obj"
            write_function(short, "_synthetic", 0x401000, b"\xc3")
            write_function(longer, "_synthetic", 0x401000, b"\x90\xc3")
            for left, right, gap_side in ((short, longer, "left"), (longer, short, "right")):
                with self.subTest(gap_side=gap_side):
                    diff_path = root / "diff.json"
                    match.run([cli, "diff", "-1", left, "-2", right,
                               "_synthetic", "-o", diff_path])
                    diff = json.loads(diff_path.read_text())
                    lines = diff[gap_side]["symbols"][0]["instructions"]
                    self.assertTrue(any(line.get("instruction") is None for line in lines))
                    with redirect_stdout(io.StringIO()) as output:
                        match.print_diff(diff, "_synthetic")
                    rendered = output.getvalue()
                    self.assertIn("RETAIL\n", rendered)
                    self.assertIn("REBUILT\n", rendered)
                    self.assertEqual(rendered.count("ret"), 2)
                    self.assertEqual(rendered.count("nop"), 1)

    def test_pinned_cli_reads_named_objects_without_game_files(self):
        if not (match.ROOT / "build/tools/bin/objdiff-cli").is_file():
            self.skipTest("Run make setup to include the objdiff integration check (required in CI)")
        cli, _ = match.objdiff()  # Verify the installed binary's pinned checksum.
        address = 0x401000
        original = b"\xe8" + struct.pack("<i", 0x402000 - address - 5) + b"\xc3"
        candidates = {
            "same": original,
            "wrong_call": b"\xe8" + struct.pack("<i", 0x403000 - address - 5) + b"\xc3",
            "wrong_size": original + b"\x90",
            "zero": b"\xcc" * len(original),
        }
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            units = []
            for name, code in candidates.items():
                target, base = f"{name}.target.obj", f"{name}.base.obj"
                write_function(root / target, "_synthetic", address, original)
                write_function(root / base, "_synthetic", address, code)
                units.append({"name": name, "target_path": target, "base_path": base})
            (root / "objdiff.json").write_text(json.dumps({
                "build_target": False, "build_base": False, "units": units}))
            result = subprocess.run([str(cli), "report", "generate", "-p", str(root),
                                     "-o", str(root / "report.json")],
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            report = json.loads((root / "report.json").read_text())
        scores = {unit["name"]: match.objdiff_percent(unit["functions"][0])
                  for unit in report["units"]}
        self.assertEqual(set(scores), set(candidates))
        self.assertEqual(scores["same"], 100.0)
        zero = next(unit for unit in report["units"] if unit["name"] == "zero")
        self.assertNotIn("fuzzy_match_percent", zero["functions"][0])
        self.assertEqual(scores["zero"], 0.0)
        # Every target object contributes, including repeated original addresses.
        native = match.objdiff_progress(report, [{"target_size": len(original),
                                                 "objects": [{} for _ in candidates]}])
        self.assertEqual(native["objdiff_total_code"], len(original) * len(candidates))
        self.assertEqual(native["objdiff_fuzzy_percent"], report["measures"]["fuzzy_match_percent"])
        # A 100% advisory score must never grant either negative a byte match.
        for name in ("wrong_call", "wrong_size", "zero"):
            row = ProgressTests().row(size=len(original), exact=candidates[name] == original)
            row["objdiff_percent"] = scores[name]
            self.assertEqual(match.progress([row], ProgressTests.target)["matched_code_bytes"], 0)


class InlinedInventoryTests(unittest.TestCase):
    inline = {"symbol": "?HashString@@YAIPAE@Z", "address": None, "size": None,
              "ctype": "u32 __cdecl (u8 *)"}
    target = {"image_base": "0x400000", "image_size": "0x10000", "sha256": "a" * 64,
              "text_va": "0x401000", "text_size": "0x1000"}

    def run_match(self, records, args=(), objects=None, import_definitions=None):
        with tempfile.TemporaryDirectory() as temp, ExitStack() as stack:
            root = Path(temp)
            (root / "config").mkdir()
            (root / "src").mkdir()
            (root / "src/saga.h").write_text("// fixture\n")
            (root / "src/externals.h").write_text("// API declarations\n")
            (root / "Makefile").write_text("# fixture\n")
            (root / "config/toolchain.json").write_text('{"version": "fixture"}')
            (root / "tools").mkdir()
            for name, text in (import_definitions or {}).items():
                (root / "tools" / name).write_text(text)
            pe = SimpleNamespace(imports=lambda: [], import_thunk=lambda _: None,
                                 read_va=lambda va, size: b"\xcc" * size, sections=[
                {"name": ".text", "virtual_address": 0x1000, "virtual_size": 0x1000,
                 "raw_size": 0x1000, "characteristics": 0x20},
                {"name": ".data", "virtual_address": 0x3000, "virtual_size": 0x1000,
                 "raw_size": 0x200, "characteristics": 0x40}])
            for name, value in (("ROOT", root), ("OUT", root / "build/objdiff")):
                stack.enter_context(patch.object(match, name, value))
            for name, value in (("project", {"target": self.target}), ("load_symbols", records),
                                ("retail", pe), ("objdiff", ("unused", {"version": "fixture"})),
                                ("compiled_objects", objects or {})):
                stack.enter_context(patch.object(match, name, return_value=value))
            # The reproduced-data inputs compare the real objects and archives.
            stack.enter_context(patch.object(match, "reproduced_inputs", return_value=({}, {})))
            stack.enter_context(patch("sys.argv", ["match.py", *args]))
            output = stack.enter_context(redirect_stdout(io.StringIO()))
            self.assertEqual(match.main(), 0)
            report = json.loads((root / "build/objdiff/summary.json").read_text())
            return report, output.getvalue()

    def test_import_definitions_are_hashed_as_report_inputs(self):
        definitions = {name + ".def": "LIBRARY " + name + ".dll\nEXPORTS\n"
                       for name in ("d3dx9_35", "d3d9", "dsound", "dinput8", "xinput1_3")}
        result, _ = self.run_match([], import_definitions=definitions)
        for name, text in definitions.items():
            self.assertEqual(result["inputs"]["tools/" + name], match.digest(text.encode()))
        self.assertIn("src/externals.h", result["inputs"])

    def test_import_export_change_updates_fingerprint_without_changing_match_results(self):
        original = "LIBRARY d3dx9_35.dll\nEXPORTS\n"
        revised = original + "    D3DXCompileShader@40\n"
        before, _ = self.run_match([], import_definitions={"d3dx9_35.def": original})
        after, _ = self.run_match([], import_definitions={"d3dx9_35.def": revised})
        self.assertEqual(set(before["inputs"]), set(after["inputs"]))
        changed = {name for name in before["inputs"]
                   if before["inputs"][name] != after["inputs"][name]}
        self.assertEqual(changed, {"tools/d3dx9_35.def"})
        self.assertEqual({k: v for k, v in before.items() if k != "inputs"},
                         {k: v for k, v in after.items() if k != "inputs"})

    def test_inlined_symbols_report_separately_without_increasing_coverage_or_function_counts(self):
        records = [{"symbol": "_code", "address": 0x401000, "size": 16, "ctype": ""},
                   {"symbol": "_data", "address": 0x403000, "size": 4, "ctype": "u32"}]
        baseline, _ = self.run_match(records)
        result, output = self.run_match([*records, self.inline])
        self.assertEqual(result["functions"], baseline["functions"])
        self.assertEqual(result["data"], baseline["data"])
        self.assertEqual(result["progress"], baseline["progress"])
        self.assertEqual(result["externals"], [])
        self.assertEqual(result["inputs"]["src/externals.h"], match.digest(b"// API declarations\n"))
        self.assertEqual(result["inlined"], [{"name": self.inline["symbol"],
                          "symbol": self.inline["symbol"], "address": None,
                          "ctype": self.inline["ctype"], "status": "inlined"}])
        self.assertIn("part of their callers", output)

    def test_inlined_function_selection_explains_caller_without_requiring_retail_or_tools(self):
        with patch.object(match, "project", return_value={"target": self.target}), \
                patch.object(match, "load_symbols", return_value=[self.inline]), \
                patch.object(match, "retail") as retail, patch.object(match, "objdiff") as objdiff, \
                patch("sys.argv", ["match.py", "--function", self.inline["symbol"]]):
            with self.assertRaisesRegex(ValueError, "part of its caller.*no standalone original address"):
                match.main()
            retail.assert_not_called()
            objdiff.assert_not_called()

    def test_match_cannot_treat_an_inlined_only_definition_as_addressable(self):
        obj = SimpleNamespace(defined_symbols=lambda: [{"name": self.inline["symbol"]}])
        with self.assertRaisesRegex(ValueError, "standalone definition has only an inlined mapping"):
            self.run_match([self.inline], objects={"src/accidental.cpp": obj})

    def test_recovered_standalone_copy_remains_selectable_by_name_or_address(self):
        standalone = dict(self.inline, address=0x401000, size=16)
        for selector in (self.inline["symbol"], "0x00401000"):
            with self.subTest(selector=selector):
                report, _ = self.run_match([standalone, self.inline], ["--function", selector])
                self.assertEqual(len(report["functions"]), 1)
                self.assertEqual(report["inlined"], [])
