#!/usr/bin/env python3
"""Inventory source constructs that a hand-written codebase would not have.

Every finding belongs to one class (see CLASSES). The scan is textual: comments
and string literals are separated from code first, braces are tracked to know
whether a line is inside a function body, a struct/enum body, an `extern "C"`
block or at file scope. It needs no compiler and no originals.

    python3 tools/oddscan.py                    # summary table (src/), top files
    python3 tools/oddscan.py --class pragma-seg --list   # file:line list of one class
    python3 tools/oddscan.py --json > out.json  # everything, including magic-number analysis
    python3 tools/oddscan.py --magic            # enum and flag candidates, ranked by use
    python3 tools/oddscan.py --magic --hints DIR   # also match values against the enums of another tree
    python3 tools/oddscan.py --check            # fail if a class exceeds config/oddscan-budget.json
    python3 tools/oddscan.py --write-budget     # lower the budget to the current counts

`--check` only fails on growth; a class that shrank is reported so the budget
can be lowered. Budgets only go down (tools/budget.py --guard). Vendored sources (lib/ogg-vorbis, fetched by tools/dependencies.py)
are scanned and reported separately when present, never budgeted.

The magic-number analysis keys a field by its member name and resolves it to a
struct when exactly one struct in src/ declares a member of that name. Literal
uses are classified: `0`, `1` and `-1` outside a `case` are trivial; relational
comparisons are bounds (limits, sizes); `==`/`!=`/`case`/assignments are
state values; operands of `&`, `|`, `^`, `|=`, `&=`, `^=` are flags (one bit) or
masks (several bits). Float literals are never counted.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import sys

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import budget  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BUDGET = ROOT / "config/oddscan-budget.json"
VENDORED = ROOT / "lib/ogg-vorbis"
SUFFIXES = {".c", ".cpp", ".h", ".inc"}

# (class, group, description). The group is the audit section (a-g).
CLASSES = [
    ("pragma-seg", "a", "#pragma const_seg/data_seg/bss_seg/code_seg/section/alloc_text/init_seg (placement)"),
    ("pragma-optimizer", "a", "#pragma optimize/inline_depth/inline_recursion/auto_inline/intrinsic/function"),
    ("pragma-pack", "a", "#pragma pack"),
    ("pragma-warning", "a", "#pragma warning"),
    ("pragma-comment", "a", "#pragma comment(lib/linker)"),
    ("pragma-macro", "a", "#pragma push_macro/pop_macro"),
    ("pragma-other", "a", "any other #pragma or __pragma()"),
    ("noinline", "b", "__declspec(noinline)"),
    ("forceinline", "b", "__forceinline"),
    ("inline-keyword", "b", "__inline (non-standard spelling)"),
    ("naked", "b", "__declspec(naked)"),
    ("asm", "b", "__asm blocks or statements"),
    ("volatile-local", "b", "volatile inside a function body (locals, casts)"),
    ("volatile-other", "b", "volatile at file scope or in declarations"),
    ("register", "b", "register storage class"),
    ("fastcall", "b", "__fastcall"),
    ("thiscall", "b", "__thiscall"),
    ("stdcall-definition", "b", "__stdcall on a function defined in a unit"),
    ("unused-edx", "b", "parameter that only absorbs a register argument (unused_edx and similar)"),
    ("goto", "b", "goto statement"),
    ("if0", "b", "#if 0 blocks"),
    ("match-exception", "c", "// MATCH-EXCEPTION: marker"),
    ("nolint", "c", "NOLINT / NOLINTNEXTLINE / NOLINTBEGIN"),
    ("format-off", "c", "clang-format off regions"),
    ("do-while0", "c", "do { } while (0) outside a macro"),
    ("dummy", "c", "dummy/sink/discard identifiers and self-assignments"),
    ("void-cast", "c", "(void)name; statements (unused parameter or value)"),
    ("ptr-int-cast", "c", "pointer cast through an integer ((u32)&x, (u32)p - (u32)q)"),
    ("raw-offset", "c", "byte-pointer plus constant ((u8 *)p + 0x50), raw field access"),
    ("view-type", "c", "definition of a *_VIEW_PC / *_view overlay type"),
    ("view-use", "c", "use of a *_VIEW_PC / *_view overlay type"),
    ("assert-offsetof-header", "c", "STATIC_ASSERT(offsetof ...) in a header"),
    ("assert-offsetof-unit", "c", "STATIC_ASSERT(offsetof ...) in a .c/.cpp (overlay scaffolding)"),
    ("assert-sizeof", "c", "STATIC_ASSERT(sizeof ...)"),
    ("placeholder-field", "c", "unk_/pad_/field_0xNN/padNN identifiers"),
    ("hex-name", "c", "address-named symbols (FUN_/DAT_/sub_/data_0x.., Foo67e8), older forms"),
    ("address-name", "c", "standard address names fun_XXXXXXXX/dat_XXXXXXXX in src/ and config/symbols.csv "
                          "(config/symbols.md): shrinks by naming"),
    ("candidate-name", "c", "identifiers with a _candidate suffix"),
    ("pc-suffix", "c", "identifiers ending in Pc/_PC/_pc (accepted, count only)"),
    ("comment-draft", "c", "SOURCE DRAFT ONLY comments"),
    ("comment-todo", "c", "TODO/FIXME/XXX/HACK comments"),
    ("comment-retail", "c", "comments that mention retail"),
    ("comment-address", "c", "comments that cite an executable address"),
    ("define-in-unit", "d", "#define in a .c/.cpp"),
    ("define-keyword", "d", "#define of a keyword or builtin type"),
    ("define-prototype", "d", "macro-generated unprototyped calls (*_BARE, UNPROTOTYPED)"),
    ("define-steering", "d", "macro whose body steers code generation (noinline, volatile, __asm, pragma)"),
    ("undef-in-unit", "d", "#undef in a .c/.cpp"),
    ("dup-macro", "d", "macro defined in more than one file"),
    ("dup-macro-conflict", "d", "macro defined in more than one file with different bodies"),
    ("dup-type", "d", "struct/union/enum/typedef defined in more than one file"),
    ("dup-type-conflict", "d", "struct/union/enum defined in more than one file with different bodies"),
    ("local-extern", "d", "extern declaration inside a function body"),
    ("magic-state", "g", "literal state/mode/type value (==, !=, case, assignment), not 0/1/-1"),
    ("magic-flag", "g", "single-bit literal in a bit operation"),
    ("magic-mask", "g", "multi-bit literal in a bit operation"),
    ("magic-bound", "g", "literal in a relational comparison (limits, sizes)"),
    ("enum-anonymous", "g", "anonymous enum"),
    ("enum-value-name", "g", "enum member named by its value (FOO_0x10, STATE_3)"),
]
CLASS_IDS = [name for name, _, _ in CLASSES]
# Classes recorded as counts per file only (their line lists would be very long and carry no news).
COUNT_ONLY = {"pc-suffix"}

KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "inline", "int", "long", "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "bool", "class",
    "true", "false", "new", "delete", "this", "virtual", "public", "private", "protected", "__inline",
    "__forceinline", "__cdecl", "__stdcall", "__fastcall", "__thiscall", "__declspec", "NULL", "BOOL", "TRUE",
    "FALSE",
}

LEX = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.DOTALL)
LITERAL = r'(-\s*)?(0[xX][0-9a-fA-F]+|\d+)[uUlL]*(?![\w.])'
ACCESS = r'(?<![\w.>])([A-Za-z_]\w*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*|\s*\[[^\[\]]*\])*)'
NAMED = r'([A-Z][A-Z0-9_]{2,})\b'
CMP = re.compile(ACCESS + r'\s*(==|!=|<=|>=|<(?![<=])|>(?![>=]))\s*' + LITERAL)
CMP_REV = re.compile(r'(?<![\w.])' + LITERAL + r'\s*(==|!=)\s*' + ACCESS)
CMP_NAMED = re.compile(ACCESS + r'\s*(==|!=)\s*' + NAMED + r'(?!\s*\()')
ASSIGN = re.compile(ACCESS + r'\s*=(?!=)\s*' + LITERAL + r'\s*;')
ASSIGN_NAMED = re.compile(ACCESS + r'\s*=(?!=)\s*' + NAMED + r'\s*;')
BIT = re.compile(ACCESS + r'\s*(&(?![&=])|\|(?![|=])|\^(?!=)|&=|\|=|\^=)\s*(~\s*)?' + LITERAL)
BIT_NAMED = re.compile(ACCESS + r'\s*(&(?![&=])|\|(?![|=])|&=|\|=)\s*(~\s*)?' + NAMED + r'(?!\s*\()')
CASE = re.compile(r'\bcase\s+' + LITERAL + r'\s*:')
CASE_NAMED = re.compile(r'\bcase\s+' + NAMED + r'\s*:')
VAR_DECL = re.compile(r'\b(?:struct\s+|union\s+)?([A-Za-z_]\w*)\s*(?:\*\s*(?:const\s+)?)*\s+\**\s*'
                      r'([A-Za-z_]\w*)\s*(?=[,;=)\[])')
SWITCH = re.compile(r'\bswitch\s*\((.*)\)\s*$')

PRAGMA = re.compile(r'^\s*#\s*pragma\s+(\w+)|__pragma\s*\(\s*(\w+)')
PRAGMA_KIND = {
    "const_seg": "pragma-seg", "data_seg": "pragma-seg", "bss_seg": "pragma-seg", "code_seg": "pragma-seg",
    "section": "pragma-seg", "alloc_text": "pragma-seg", "init_seg": "pragma-seg",
    "optimize": "pragma-optimizer", "inline_depth": "pragma-optimizer", "inline_recursion": "pragma-optimizer",
    "auto_inline": "pragma-optimizer", "intrinsic": "pragma-optimizer", "function": "pragma-optimizer",
    "pack": "pragma-pack", "warning": "pragma-warning", "comment": "pragma-comment",
    "push_macro": "pragma-macro", "pop_macro": "pragma-macro",
}
CODE_PATTERNS = [
    ("noinline", re.compile(r'__declspec\s*\(\s*noinline\s*\)')),
    ("forceinline", re.compile(r'\b__forceinline\b')),
    ("inline-keyword", re.compile(r'\b__inline\b')),
    ("naked", re.compile(r'__declspec\s*\(\s*naked\s*\)')),
    ("asm", re.compile(r'\b_?_asm\b')),
    ("register", re.compile(r'\bregister\b(?!\s*\()')),
    ("fastcall", re.compile(r'\b__fastcall\b')),
    ("thiscall", re.compile(r'\b__thiscall\b')),
    ("unused-edx", re.compile(r'\b\w*(?:unused_e?[abcd]x|_edx|edx_unused|unused_register)\w*\b')),
    ("goto", re.compile(r'\bgoto\s+\w+\s*;')),
    ("ptr-int-cast", re.compile(
        r'\(\s*(?:u32|i32|int|unsigned(?:\s+int)?|uintptr_t|UINT_PTR|DWORD|size_t|long|unsigned\s+long|LONG)\s*\)'
        r'\s*(?:&\s*[A-Za-z_(]|[A-Za-z_]\w*(?:->\w+)*\s*[-+]\s*\(\s*(?:u32|i32|int|uintptr_t|DWORD)\s*\))')),
    ("raw-offset", re.compile(
        r'\(\s*(?:const\s+)?(?:char|u8|i8|unsigned\s+char|signed\s+char|BYTE|uchar)\s*\*\s*\)\s*'
        r'[A-Za-z_][\w>.\-]*\s*\)?\s*\+\s*(?:0[xX][0-9a-fA-F]+|\d+)')),
    ("placeholder-field", re.compile(r'\b(?:unk_?[0-9a-fA-F]*\w*|pad_\w*|_?pad[0-9]+\w*|field_(?:0x)?[0-9a-fA-F]+\w*)\b')),
    ("address-name", re.compile(r'\b_?(?:fun|dat)_[0-9a-f]{8}\b')),
    ("hex-name", re.compile(
        r'\b(?!_?(?:fun|dat)_[0-9a-f]{8}\b)'
        r'(?:(?:FUN|DAT|LAB|PTR|sub|SUB|data|loc|off|byte|dword|word)_(?:0x)?[0-9a-fA-F]{4,8}'
        r'|[A-Za-z]\w*?[a-z_](?=[0-9a-f]*[0-9][0-9a-f]*\b)(?=[0-9a-f]*[a-f])[0-9a-f]{4,8})\b')),
    ("candidate-name", re.compile(r'\b\w+_candidate\w*\b')),
]
VIEW_NAME = r'\b[A-Za-z_]\w*(?:_VIEW_PC|_VIEW|_view|View_PC|ViewPc)\b'
VIEW_DEF = re.compile(r'\b(?:struct|union|typedef\s+(?:struct|union))\s+(' + VIEW_NAME[2:-2] + r')\b')
VIEW_USE = re.compile(VIEW_NAME)
PC_SUFFIX = re.compile(r'\b[A-Za-z_]\w*(?:Pc|_PC|_pc)\b')
ASSERT = re.compile(r'\bSTATIC_ASSERT\s*\(\s*(offsetof|sizeof)?')
DUMMY = re.compile(r'\b(?:dummy\w*|\w+_dummy|sink\w*|discard\w*)\b|(?<![\w.>])(\w+)\s*=\s*\1\s*;')
VOID_CAST = re.compile(r'^\s*\(\s*void\s*\)\s*[A-Za-z_]\w*\s*;')
EXTERN_LOCAL = re.compile(r'^\s*extern\b')
VOLATILE = re.compile(r'\bvolatile\b')
STDCALL_DEF = re.compile(r'\b__stdcall\b')
COMMENT_PATTERNS = [
    ("comment-draft", re.compile(r'SOURCE DRAFT ONLY')),
    ("comment-todo", re.compile(r'\b(?:TODO|FIXME|XXX|HACK)\b')),
    ("comment-retail", re.compile(r'\b[Rr]etail\b')),
    ("comment-address", re.compile(r'\b0x(?:00)?[4-7][0-9a-fA-F]{5}\b')),
    ("nolint", re.compile(r'\bNOLINT(?:NEXTLINE|BEGIN|END)?\b')),
    ("format-off", re.compile(r'clang-format\s+off')),
]
DEFINE = re.compile(r'^\s*#\s*define\s+(\w+)(\([^)]*\))?\s*(.*)$', re.DOTALL)
UNDEF = re.compile(r'^\s*#\s*undef\s+(\w+)')
INCLUDE = re.compile(r'^\s*#\s*include\s*([<"][^>"]*[>"])')
IF0 = re.compile(r'^\s*#\s*if\s+0\b')
BUILTIN_TYPES = {"int", "char", "short", "long", "float", "double", "void", "bool", "unsigned", "signed",
                 "i8", "i16", "i32", "u8", "u16", "u32", "f32", "f64", "s8", "s16", "s32", "BOOL", "DWORD",
                 "BYTE", "WORD"}
DEFINE_KEYWORDS = {"static", "inline", "__inline", "__forceinline", "const", "volatile", "register", "extern",
                   "auto", "struct", "union", "enum", "typedef", "return", "if", "for", "while"} | BUILTIN_TYPES
STEERING_BODY = re.compile(r'__declspec\s*\(\s*noinline|__forceinline|\bvolatile\b|\b__asm\b|__pragma|\bregister\b')
PROTOTYPE_MACRO = re.compile(r'_BARE\b|UNPROTOTYPED|NOPROTO')
VALUE_NAMED_MEMBER = re.compile(r'(?:_0[xX][0-9a-fA-F]+|(?:UNK|UNKNOWN|VALUE|VAL|STATE|MODE|TYPE|FLAG|BIT|CONFIG)'
                                r'_?[0-9a-fA-F]*[0-9][0-9a-fA-F]*)$')


def lex(text):
    """Return (code, comments): code with comments blanked and string contents removed; comment text per line."""
    comments = collections.defaultdict(list)
    out, last, line, line_pos = [], 0, 0, 0
    for match in LEX.finditer(text):
        start = match.start()
        line += text.count("\n", line_pos, start)
        line_pos = start
        out.append(text[last:start])
        token = match.group()
        if token[0] == "/":
            for offset, part in enumerate(token.split("\n")):
                comments[line + offset].append(part)
            out.append(re.sub(r'[^\n]', ' ', token))
        else:
            out.append(token[0] + " " * (len(token) - 2) + token[-1])
        last = match.end()
    out.append(text[last:])
    return "".join(out), comments


def int_value(token):
    token = token.strip().rstrip("uUlL")
    negative = token.startswith("-")
    token = token.lstrip("-").strip()
    try:
        value = int(token, 16) if token.lower().startswith("0x") else int(token, 10)
    except ValueError:
        return None
    return -value if negative else value


def eval_constant(expression, known):
    """Evaluate a simple enum/#define constant expression; None when it is not a plain integer expression."""
    expression = re.sub(r'(?<=[0-9a-fA-F])[uUlL]+\b', '', expression.strip())
    if not expression or len(expression) > 80:
        return None

    def name(match):
        word = match.group()
        if re.fullmatch(r'0[xX][0-9a-fA-F]+|\d+', word):
            return str(int(word, 16) if word.lower().startswith("0x") else int(word))
        if word in known and known[word] is not None:
            return str(known[word])
        raise ValueError(word)

    try:
        expression = re.sub(r'0[xX][0-9a-fA-F]+|\b\w+\b', name, expression)
    except ValueError:
        return None
    if not re.fullmatch(r'[\d\s()+\-*|&<>~^]+', expression):
        return None
    try:
        value = eval(expression, {"__builtins__": {}}, {})  # noqa: S307 - digits and operators only
    except Exception:  # noqa: BLE001 - malformed expressions are simply not constants
        return None
    return value if isinstance(value, int) else None


def field_key(access):
    """Member name of an access chain, or the plain identifier."""
    access = re.sub(r'\s+', '', access)
    while access.endswith("]"):
        depth, index = 0, len(access) - 1
        while index >= 0:
            depth += {"]": 1, "[": -1}.get(access[index], 0)
            if depth == 0:
                break
            index -= 1
        access = access[:index]
    parts = re.split(r'->|\.', access)
    member = len(parts) > 1
    return parts[-1], member


def is_single_bit(value):
    value &= 0xFFFFFFFF
    return value and not value & (value - 1)


class FileScan:
    """Scan of one file: findings, declarations and literal uses."""

    def __init__(self, relative, text):
        self.path = relative
        self.unit = relative.endswith((".c", ".cpp"))
        self.header = not self.unit
        self.findings = collections.defaultdict(list)    # class -> [(line, text)]
        self.counts = collections.Counter()
        self.pragmas = collections.Counter()             # pragma kind -> count
        self.defines = []                                # (name, line, body)
        self.aggregates = []                             # (kind, name, line, body)
        self.typedefs = []                               # (name, line)
        self.enums = []                                  # (name, line, [(member, value)])
        self.members = []                                # (struct name, member, type text, bitfield width)
        self.includes = []
        self.uses = []                                   # (access, context, value or name, line)
        self.aliases = {}                                # typedef name -> struct tag
        self.var_types = collections.defaultdict(collections.Counter)  # identifier -> declared type names
        self.top_level_statements = []                   # (line, text) at file or extern "C" scope
        self.scan(text)

    def add(self, cls, line, text=""):
        self.counts[cls] += 1
        if cls not in COUNT_ONLY:
            self.findings[cls].append((line, text.strip()[:160]))

    def scan(self, text):
        code, comments = lex(text)
        raw_lines = text.split("\n")
        lines = code.split("\n")
        stack = []          # entries: dict(kind=..., line=..., body=[...], name=..., typedef=bool)
        head = []           # code since the last ; { or }
        head_line = 0
        pending_switch = None
        switch_stack = []
        in_macro = False
        for number, line in enumerate(lines, 1):
            raw = raw_lines[number - 1] if number - 1 < len(raw_lines) else ""
            for part in comments.get(number - 1, ()):
                for cls, pattern in COMMENT_PATTERNS:
                    if pattern.search(part):
                        self.add(cls, number, part)
                if "MATCH-EXCEPTION" in part:
                    self.add("match-exception", number, part.split("MATCH-EXCEPTION", 1)[1].lstrip(": "))
            stripped = line.strip()
            directive = in_macro or stripped.startswith("#")
            if directive:
                if not in_macro:
                    self.directive(number, line, lines, raw_lines)
                    if IF0.match(line):
                        self.add("if0", number, raw)
                in_macro = raw.rstrip().endswith("\\")
                # Macro bodies: steering patterns are recorded by directive(); code patterns too.
                self.code_patterns(number, line, raw, in_function=False, in_macro=True)
                continue
            in_function = any(entry["kind"] == "function" for entry in stack)
            in_enum = bool(stack) and stack[-1]["kind"] == "enum"
            self.code_patterns(number, line, raw, in_function, in_macro=False)
            for match in VAR_DECL.finditer(line):
                if match.group(1) not in KEYWORDS:
                    self.var_types[match.group(2)][match.group(1)] += 1
            if in_function:
                if EXTERN_LOCAL.search(line):
                    self.add("local-extern", number, raw)
                if VOLATILE.search(line):
                    self.add("volatile-local", number, raw)
                if re.search(r'\bwhile\s*\(\s*0\s*\)', line):
                    self.add("do-while0", number, raw)
                if not in_enum:
                    self.literal_uses(number, line, switch_stack)
            elif VOLATILE.search(line):
                self.add("volatile-other", number, raw)
            if STDCALL_DEF.search(line) and self.unit and not in_function:
                if re.search(r'\)\s*\{\s*$', line) or (number < len(lines) and lines[number].strip() == "{"):
                    self.add("stdcall-definition", number, raw)
            match = SWITCH.search(line.rstrip().rstrip("{").rstrip())
            if match and in_function:
                pending_switch = match.group(1)
            # Brace and statement tracking.
            position = 0
            for event in re.finditer(r'[{};]', line):
                segment = line[position:event.start()]
                position = event.end()
                if not head:
                    head_line = number
                head.append(segment)
                for entry in stack:
                    if entry["kind"] in ("struct", "union", "class", "enum"):
                        entry["body"].append(segment)
                char = event.group()
                text_head = " ".join(head).strip()
                if char == "{":
                    kind = self.brace_kind(text_head, stack)
                    for entry in stack:
                        if entry["kind"] in ("struct", "union", "class", "enum"):
                            entry["body"].append("{")
                    name = None
                    match = re.search(r'\b(?:struct|union|class|enum)\s+(\w+)\s*(?::[^{]*)?$', text_head)
                    if match and kind in ("struct", "union", "class", "enum"):
                        name = match.group(1)
                    stack.append({"kind": kind, "line": head_line, "body": [], "name": name,
                                  "typedef": bool(re.match(r'typedef\b', text_head)), "switch": pending_switch})
                    if kind == "block" and pending_switch is not None:
                        switch_stack.append(pending_switch)
                        stack[-1]["is_switch"] = True
                    pending_switch = None
                    head = []
                elif char == "}":
                    if stack:
                        entry = stack.pop()
                        if entry.get("is_switch") and switch_stack:
                            switch_stack.pop()
                        if entry["kind"] in ("struct", "union", "class", "enum"):
                            for outer in stack:
                                if outer["kind"] in ("struct", "union", "class", "enum"):
                                    outer["body"].append(" ".join(entry["body"]) + "}")
                            rest = line[event.end():]
                            tail = re.match(r'\s*(\w+)\s*;', rest)
                            if entry["typedef"] and tail:
                                if entry["name"] and entry["name"] != tail.group(1):
                                    self.aliases[tail.group(1)] = entry["name"]
                                entry["name"] = entry["name"] or tail.group(1)
                                self.typedefs.append((tail.group(1), entry["line"]))
                            self.close_aggregate(entry, stack)
                    head = []
                else:
                    if text_head and not stack or (stack and stack[-1]["kind"] in ("extern", "namespace")):
                        self.top_level_statements.append((head_line, text_head))
                        match = re.match(r'typedef\b(?!.*\{).*?(?:\(\s*\*\s*(\w+)\s*\)|(\w+)\s*(?:\[[^\]]*\])*)\s*'
                                         r'(?:\(.*\))?\s*$', text_head)
                        if match and text_head.startswith("typedef") and "saga_static_assert_" not in text_head:
                            name = match.group(1) or match.group(2)
                            if name:
                                self.typedefs.append((name, head_line))
                            alias = re.fullmatch(r'typedef\s+(?:struct|union|class)\s+(\w+)\s+(\w+)', text_head)
                            if alias and alias.group(1) != alias.group(2):
                                self.aliases[alias.group(2)] = alias.group(1)
                    for entry in stack:
                        if entry["kind"] in ("struct", "union", "class", "enum"):
                            entry["body"].append(";")
                    head = []
            remainder = line[position:]
            if remainder.strip():
                if not head:
                    head_line = number
                head.append(remainder)
                for entry in stack:
                    if entry["kind"] in ("struct", "union", "class", "enum"):
                        entry["body"].append(remainder)

    @staticmethod
    def brace_kind(head, stack):
        inside = stack[-1]["kind"] if stack else None
        in_function = any(entry["kind"] == "function" for entry in stack)
        if inside == "init":
            return "init"
        if re.search(r'\bextern\s*"\s*"\s*$', head):
            return "extern"
        if re.search(r'\bnamespace\b[^()]*$', head):
            return "namespace"
        if re.search(r'=\s*$', head):
            return "init"
        match = re.search(r'\b(struct|union|class|enum)\b[^()=]*$', head)
        if match:
            return match.group(1)
        if in_function:
            return "block"
        if inside in ("struct", "union", "class") and not re.search(r'\)\s*(?:const\s*)?$', head):
            return "block"
        if re.search(r'\)\s*(?:const\s*)?(?:throw\s*\(\s*\)\s*)?$', head) or re.search(r'\)\s*:\s*\w', head):
            return "function"
        return "block"

    def close_aggregate(self, entry, stack):
        body = " ".join(entry["body"])
        name = entry["name"]
        self.aggregates.append((entry["kind"], name, entry["line"], re.sub(r'\s+', ' ', body).strip()))
        if entry["kind"] == "enum":
            members, known, value = [], {}, -1
            for item in self.split_top(body):
                item = item.strip()
                match = re.match(r'([A-Za-z_]\w*)\s*(?:=\s*(.+))?$', item, re.DOTALL)
                if not match:
                    continue
                if match.group(2) is not None:
                    value = eval_constant(match.group(2), {**self.enum_constants_known(), **known})
                elif value is not None:
                    value += 1
                known[match.group(1)] = value
                members.append((match.group(1), value))
                if VALUE_NAMED_MEMBER.search(match.group(1)):
                    self.add("enum-value-name", entry["line"], match.group(1))
            self.enums.append((name, entry["line"], members))
            if name is None:
                self.add("enum-anonymous", entry["line"], ", ".join(m for m, _ in members[:4]))
        elif entry["kind"] in ("struct", "union", "class") and name:
            # Members of nested anonymous structs/unions belong to this struct too.
            flat = re.sub(r'\b(?:struct|union)\s*\{', ';', body).replace("{", ";").replace("}", ";")
            for declaration in flat.split(";"):
                declaration = declaration.strip()
                if not declaration or "(" in declaration or declaration.startswith(("typedef", "friend")):
                    continue
                bitfield = re.search(r':\s*(\d+)\s*$', declaration)
                base = re.sub(r':\s*\d+\s*$', '', declaration)
                parts = [p.strip() for p in base.split(",")]
                type_match = re.match(r'(.*?)(\**\s*[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*)$', parts[0])
                type_text = type_match.group(1).strip() if type_match else ""
                for part in parts:
                    member = re.search(r'([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*$', part)
                    if member and member.group(1) not in KEYWORDS:
                        self.members.append((name, member.group(1), type_text,
                                             int(bitfield.group(1)) if bitfield else None))

    _shared_constants = {}

    def enum_constants_known(self):
        return FileScan._shared_constants

    @staticmethod
    def split_top(body):
        depth, item, items = 0, [], []
        for char in body:
            if char in "({":
                depth += 1
            elif char in ")}":
                depth -= 1
            if char == "," and depth == 0:
                items.append("".join(item))
                item = []
            else:
                item.append(char)
        items.append("".join(item))
        return items

    def directive(self, number, line, lines, raw_lines):
        # Join continuation lines of this directive (code text, comments removed).
        text, index = line, number - 1
        while raw_lines[index].rstrip().endswith("\\") and index + 1 < len(lines):
            index += 1
            text = text.rstrip().rstrip("\\") + " " + lines[index]
        match = PRAGMA.search(text)
        if match:
            kind = match.group(1) or match.group(2)
            self.pragmas[kind] += 1
            self.add(PRAGMA_KIND.get(kind, "pragma-other"), number, raw_lines[number - 1])
        match = INCLUDE.match(raw_lines[number - 1])
        if match:
            self.includes.append(match.group(1))
        match = DEFINE.match(text)
        if match:
            name, body = match.group(1), re.sub(r'\s+', ' ', match.group(3)).strip()
            self.defines.append((name, number, (match.group(2) or "") + " " + body))
            if self.unit:
                self.add("define-in-unit", number, raw_lines[number - 1])
            if name in DEFINE_KEYWORDS:
                self.add("define-keyword", number, raw_lines[number - 1])
            if PROTOTYPE_MACRO.search(name) or re.search(r'\(\s*\w+\s*\(\s*__cdecl\s*\*\s*\)\s*\(\s*\)\s*\)', body):
                self.add("define-prototype", number, raw_lines[number - 1])
            if STEERING_BODY.search(body) or re.search(r'#\s*pragma', body):
                self.add("define-steering", number, raw_lines[number - 1])
            if not match.group(2):
                value = eval_constant(body, {})
                if value is not None:
                    FileScan._shared_constants.setdefault(name, value)
        match = UNDEF.match(text)
        if match and self.unit:
            self.add("undef-in-unit", number, raw_lines[number - 1])

    def code_patterns(self, number, line, raw, in_function, in_macro):
        for cls, pattern in CODE_PATTERNS:
            for _ in pattern.finditer(line):
                if cls == "register" and not re.search(r'\bregister\s+\w', line):
                    continue
                self.add(cls, number, raw)
        if not in_macro:
            for match in DUMMY.finditer(line):
                self.add("dummy", number, raw)
            if VOID_CAST.search(line):
                self.add("void-cast", number, raw)
        if VIEW_DEF.search(line):
            self.add("view-type", number, raw)
        elif VIEW_USE.search(line):
            self.add("view-use", number, raw)
        for _ in PC_SUFFIX.finditer(line):
            self.counts["pc-suffix"] += 1
        for match in ASSERT.finditer(line):
            if match.group(1) == "offsetof":
                self.add("assert-offsetof-header" if self.header else "assert-offsetof-unit", number, raw)
            elif match.group(1) == "sizeof":
                self.add("assert-sizeof", number, raw)

    def literal_uses(self, number, line, switch_stack):
        if re.match(r'\s*(?:for|while)\b', line) is None and "for (" not in line:
            loop = False
        else:
            loop = True
        seen = set()

        def record(access, context, value, start):
            if start in seen:
                return
            seen.add(start)
            key, member = field_key(access)
            if key in KEYWORDS or re.fullmatch(r'[A-Z][A-Z0-9_]+', key):
                return
            self.uses.append((re.sub(r'\s+', '', access), context, value, number))
            if isinstance(value, str):
                return
            if context == "bit" or context == "bitassign":
                cls = "magic-flag" if is_single_bit(value) else "magic-mask"
                if value in (0, -1, 0xFFFFFFFF):
                    return
            elif context == "bound":
                if value in (0, 1, -1) or loop:
                    return
                cls = "magic-bound"
            else:
                if value in (0, 1, -1) and context != "case":
                    return
                cls = "magic-state"
            self.add(cls, number, line)

        for match in CMP.finditer(line):
            value = int_value((match.group(3) or "") + match.group(4))
            context = "eq" if match.group(2) in ("==", "!=") else "bound"
            record(match.group(1), context, value, match.start())
        for match in CMP_REV.finditer(line):
            record(match.group(4), "eq", int_value((match.group(1) or "") + match.group(2)), match.start(4))
        for match in ASSIGN.finditer(line):
            record(match.group(1), "assign", int_value((match.group(2) or "") + match.group(3)), match.start())
        for match in BIT.finditer(line):
            value = int_value((match.group(4) or "") + match.group(5))
            context = "bitassign" if match.group(2).endswith("=") else "bit"
            record(match.group(1), context, value, match.start())
        for match in CMP_NAMED.finditer(line):
            record(match.group(1), "eq", match.group(3), match.start())
        for match in ASSIGN_NAMED.finditer(line):
            record(match.group(1), "assign", match.group(2), match.start())
        for match in BIT_NAMED.finditer(line):
            record(match.group(1), "bit", match.group(4), match.start())
        if switch_stack:
            subject = switch_stack[-1]
            subject_match = re.fullmatch(r'\s*(?:\(\s*\w+\s*\)\s*)?' + ACCESS + r'\s*', subject)
            if subject_match:
                for match in CASE.finditer(line):
                    record(subject_match.group(1), "case", int_value((match.group(1) or "") + match.group(2)),
                           -1 - match.start())
                for match in CASE_NAMED.finditer(line):
                    record(subject_match.group(1), "case", match.group(1), -100000 - match.start())


def source_paths(root=ROOT):
    src = sorted(p for p in (root / "src").rglob("*") if p.is_file() and p.suffix in SUFFIXES)
    vendored = sorted(p for p in (root / "lib/ogg-vorbis").rglob("*") if p.is_file() and p.suffix in SUFFIXES) \
        if (root / "lib/ogg-vorbis").is_dir() else []
    return src, vendored


def scan_files(paths, root=ROOT):
    FileScan._shared_constants = {}
    scans = []
    for path in paths:
        relative = path.relative_to(root).as_posix()
        scans.append(FileScan(relative, path.read_text(encoding="latin-1")))
    return scans


def cross_file(scans):
    """Duplicate macros and types across files (src scope)."""
    macros, types = collections.defaultdict(dict), collections.defaultdict(dict)
    for scan in scans:
        for name, line, body in scan.defines:
            if name.startswith("saga_") or name in ("SAGA_JOIN",):
                continue
            macros[name].setdefault(scan.path, (line, body))
        for kind, name, line, body in scan.aggregates:
            if name:
                types[name].setdefault(scan.path, (line, hashlib.sha1(body.encode()).hexdigest()[:12]))
        for name, line in scan.typedefs:
            types[name].setdefault(scan.path, (line, None))
    findings = collections.defaultdict(list)
    for kind, table in (("macro", macros), ("type", types)):
        for name in sorted(table):
            sites = table[name]
            if len(sites) < 2:
                continue
            bodies = {body for _, body in sites.values() if body is not None}
            conflict = len(bodies) > 1
            for path in sorted(sites):
                findings[path].append(("dup-" + kind, sites[path][0], name))
                if conflict:
                    findings[path].append(("dup-" + kind + "-conflict", sites[path][0], name))
    for scan in scans:
        for cls, line, name in findings.get(scan.path, ()):
            scan.add(cls, line, name)


def struct_index(scans):
    owners, types, bitfields = collections.defaultdict(set), {}, collections.Counter()
    for scan in scans:
        for struct, member, type_text, width in scan.members:
            owners[member].add(struct)
            types[(struct, member)] = type_text
            if width is not None:
                bitfields[scan.path] += 1
    return owners, types, bitfields


def constant_index(scans, constants):
    """Named integer values: enum members and object-like #defines, grouped by enum or name prefix."""
    groups = collections.defaultdict(dict)
    def prefix(name):
        tokens = name.split("_")
        return "_".join(tokens[:2]) + "_*" if len(tokens) > 2 else tokens[0] + "_*"

    for scan in scans:
        for name, _, members in scan.enums:
            for member, value in members:
                if value is not None:
                    groups["enum " + name if name else prefix(member) + " (anonymous enum/#define)"][member] = value
    for name, value in constants.items():
        groups[prefix(name) + " (anonymous enum/#define)"][name] = value
    return groups


def canonical_struct(type_text, aliases, structs):
    name = re.sub(r'\b(?:struct|union|class|const|volatile)\b|\*', ' ', type_text or "").split()
    if len(name) != 1:
        return None
    name = name[0]
    for _ in range(5):
        if name in structs:
            return name
        if name not in aliases:
            return None
        name = aliases[name]
    return name if name in structs else None


def resolve_owner(access, var_types, owners, types, aliases, structs):
    """STRUCT.member for an access chain: follow declared types, else a unique owner, else *.member."""
    chain = re.sub(r'\[[^\[\]]*\]', '', access)
    while '[' in chain:
        chain = re.sub(r'\[[^\[\]]*\]', '', chain)
    parts = re.split(r'->|\.', chain)
    member = parts[-1]
    declared = var_types.get(parts[0])
    current = None
    if declared:
        current = canonical_struct(declared.most_common(1)[0][0], aliases, structs)
    for index, name in enumerate(parts[1:], 1):
        if current is None or (current, name) not in types:
            current = None
            break
        if index == len(parts) - 1:
            break
        current = canonical_struct(types[(current, name)], aliases, structs)
    if current is not None:
        return f"{current}.{member}"
    candidates = sorted(owners.get(member, ()))
    return f"{candidates[0]}.{member}" if len(candidates) == 1 else f"*.{member}"


def name_tokens(text):
    text = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', text)
    return {token for token in re.split(r'[^A-Za-z]+', text.lower())
            if len(token) >= 4 and token not in {"flag", "flags", "struct", "enum", "define", "anonymous", "type"}}


def hint_groups(directory):
    """Named constants (enums, object-like #defines) of another source tree, offered as naming hints."""
    saved = FileScan._shared_constants
    paths = sorted(p for p in Path(directory).rglob("*.h") if p.is_file())
    scans = scan_files(paths, Path(directory))
    groups = {"hint " + label: members for label, members in constant_index(scans, FileScan._shared_constants).items()}
    FileScan._shared_constants = saved
    return groups


def magic_analysis(scans, limit=40, constants=None, hints=None):
    owners, types, _ = struct_index(scans)
    groups = constant_index(scans, FileScan._shared_constants if constants is None else constants)
    groups.update(hints or {})
    aliases = {}
    for scan in scans:
        aliases.update(scan.aliases)
    structs = {struct for struct, _ in types}
    fields = {}
    for scan in scans:
        for access, context, value, line in scan.uses:
            key, member = field_key(access)
            if not member and len(key) <= 2:
                continue
            label = key
            if member:
                label = resolve_owner(access, scan.var_types, owners, types, aliases, structs)
            entry = fields.setdefault(label, {"field": label, "member": member, "uses": 0, "named": 0,
                                              "state": collections.Counter(), "bits": collections.Counter(),
                                              "masks": collections.Counter(), "names": collections.Counter(),
                                              "files": collections.Counter(), "first": f"{scan.path}:{line}"})
            entry["files"][scan.path] += 1
            if isinstance(value, str):
                entry["named"] += 1
                entry["names"][value] += 1
                continue
            if value is None:
                continue
            if context in ("eq", "case", "assign"):
                if value in (0, 1, -1) and context != "case":
                    continue
                entry["state"][value] += 1
                entry["uses"] += 1
            elif context in ("bit", "bitassign"):
                if value in (0, -1, 0xFFFFFFFF):
                    continue
                (entry["bits"] if is_single_bit(value) else entry["masks"])[value & 0xFFFFFFFF] += 1
                entry["uses"] += 1

    def best_group(values, single_bits, field):
        best = None
        values = set(values)
        wanted = name_tokens(field)
        for label, members in groups.items():
            tokens = name_tokens(label.replace("anonymous enum/#define", ""))
            if not wanted & tokens:
                continue
            group_values = set(members.values())
            if single_bits:
                group_values = {v & 0xFFFFFFFF for v in group_values if is_single_bit(v)}
            hit = values & group_values
            if len(hit) < 2 or len(group_values) < 2:
                continue
            score = (len(hit) / len(values), len(hit), -len(group_values))
            if best is None or score > best[0]:
                best = (score, label)
        if best and best[0][0] >= 0.6:
            return {"group": best[1], "coverage": round(best[0][0], 2)}
        return None

    member_group = {}
    for label, members in groups.items():
        for member in members:
            member_group.setdefault(member, label)

    def used_group(entry, values, single_bits):
        """The group of the named constants already used with this field, and how many literals it names."""
        votes = collections.Counter()
        for name, count in entry["names"].items():
            if name in member_group:
                votes[member_group[name]] += count
        if not votes:
            return None
        label = sorted(votes.items(), key=lambda item: (-item[1], item[0]))[0][0]
        named = set(groups[label].values())
        if single_bits:
            named = {v & 0xFFFFFFFF for v in named}
        values = set(values)
        return {"group": label, "used_names": votes[label], "literals_it_names": len(values & named),
                "distinct_literals": len(values)}

    enum_candidates, flag_candidates = [], []
    for entry in fields.values():
        struct, _, member = entry["field"].partition(".")
        type_text = types.get((struct, member), "") if member else ""
        files = [f for f, _ in sorted(entry["files"].items(), key=lambda item: (-item[1], item[0]))[:3]]
        values = [v for v in entry["state"] if v not in (0, 1, -1)]
        large = sum(1 for v in entry["state"] if abs(v) > 0xFFFF)
        if len(entry["state"]) >= 3 and len(values) >= 2 and large * 2 < len(entry["state"]):
            enum_candidates.append({
                "field": entry["field"], "type": type_text, "uses": sum(entry["state"].values()),
                "named_uses": entry["named"], "values": sorted(entry["state"]),
                "existing_names": used_group(entry, entry["state"], False) or best_group(entry["state"], False, entry["field"]), "files": files})
        if len(entry["bits"]) >= 2:
            flag_candidates.append({
                "field": entry["field"], "type": type_text,
                "uses": sum(entry["bits"].values()) + sum(entry["masks"].values()),
                "named_uses": entry["named"], "bits": [hex(v) for v in sorted(entry["bits"])],
                "masks": [hex(v) for v in sorted(entry["masks"])],
                "existing_names": used_group(entry, entry["bits"], True) or best_group(entry["bits"], True, entry["field"]), "files": files})
    enum_candidates.sort(key=lambda e: (-e["uses"], e["field"]))
    flag_candidates.sort(key=lambda e: (-e["uses"], e["field"]))
    return {"enum_candidates": enum_candidates[:limit] if limit else enum_candidates,
            "flag_candidates": flag_candidates[:limit] if limit else flag_candidates,
            "enum_candidate_fields": len(enum_candidates), "flag_candidate_fields": len(flag_candidates)}


def structure(scans, root=ROOT):
    by_path = {scan.path: scan for scan in scans}
    result = {}
    saga = by_path.get("src/saga.h")
    if saga:
        text = (root / "src/saga.h").read_text(encoding="latin-1")
        result["saga_h_lines"] = text.count("\n")
        result["saga_h_top_level_declarations"] = sum(
            1 for _, statement in saga.top_level_statements if "saga_static_assert_" not in statement
            and not statement.startswith("STATIC_ASSERT"))
        result["saga_h_aggregates"] = sum(1 for kind, name, _, _ in saga.aggregates if name)
        result["saga_h_static_asserts"] = len(re.findall(r'\bSTATIC_ASSERT\s*\(', text))
    units = [scan for scan in scans if scan.unit]
    result["units"] = len(units)
    result["headers"] = sorted(scan.path for scan in scans if not scan.unit)
    result["units_including_other_project_headers"] = sorted(
        scan.path for scan in units if any(inc.startswith('"') and inc != '"saga.h"' for inc in scan.includes))
    result["units_without_saga_h"] = sorted(scan.path for scan in units if '"saga.h"' not in scan.includes)
    data = by_path.get("src/data.cpp")
    if data:
        definitions = [s for _, s in data.top_level_statements
                       if not re.match(r'(?:extern|typedef|STATIC_ASSERT|using|template)\b', s)
                       and not re.search(r'\)\s*$', s)]
        result["data_cpp_definitions"] = len(definitions)
        result["data_cpp_lines"] = (root / "src/data.cpp").read_text(encoding="latin-1").count("\n")
    result["diagnostic_stub_files"] = sorted(s.path for s in scans if re.search(r'_diagnostic(_pc)?\.c', s.path))
    result["pc_suffixed_files"] = sorted(s.path for s in scans if re.search(r'_pc\.(?:c|cpp|h)$', s.path))
    _, _, bitfields = struct_index(scans)
    result["bitfield_members"] = sum(bitfields.values())
    return result


def build_special_cases(root=ROOT):
    """Target-specific Makefile variables and the comment lines directly above them."""
    makefile = root / "Makefile"
    if not makefile.is_file():
        return []
    lines = makefile.read_text().split("\n")
    cases = []
    for index, line in enumerate(lines):
        if re.match(r'^\$\(.*\)\s*:\s*\w+\s*[+:]?=', line) or re.match(r'^\S+\.obj\s*:\s*\w+\s*[+:]?=', line):
            reason, back = [], index - 1
            while back >= 0 and lines[back].startswith("#"):
                reason.insert(0, lines[back].lstrip("# "))
                back -= 1
            cases.append({"line": index + 1, "rule": line.strip(), "reason": " ".join(reason)})
    return cases


def summarize(scans):
    totals = collections.Counter()
    per_file = collections.defaultdict(collections.Counter)
    for scan in scans:
        for cls, count in scan.counts.items():
            totals[cls] += count
            per_file[cls][scan.path] += count
    return totals, per_file


def report(root=ROOT, magic_limit=40, hints=None):
    src_paths, vendored_paths = source_paths(root)
    scans = scan_files(src_paths, root)
    cross_file(scans)
    constants = dict(FileScan._shared_constants)
    vendored = scan_files(vendored_paths, root) if vendored_paths else []
    totals, per_file = summarize(scans)
    vendored_totals, _ = summarize(vendored)
    # Map rows under a standard address name are bounded but unnamed: the class
    # counts them too, so naming them shows as shrinkage and new ones as growth.
    mapped = map_address_names(root)
    if mapped:
        totals["address-name"] += mapped
        per_file["address-name"]["config/symbols.csv"] = mapped
    pragmas = collections.Counter()
    for scan in scans:
        pragmas.update(scan.pragmas)
    vendored_pragmas = collections.Counter()
    for scan in vendored:
        vendored_pragmas.update(scan.pragmas)
    return {
        "classes": [{"class": name, "group": group, "description": description, "count": totals[name],
                     "files": len(per_file[name]), "vendored": vendored_totals[name]}
                    for name, group, description in CLASSES],
        "pragma_kinds": dict(sorted(pragmas.items())),
        "vendored_pragma_kinds": dict(sorted(vendored_pragmas.items())),
        "per_file": {name: dict(sorted(per_file[name].items(), key=lambda item: (-item[1], item[0])))
                     for name in CLASS_IDS},
        "findings": {name: [f"{scan.path}:{line}: {text}" for scan in scans for line, text in scan.findings[name]]
                     for name in CLASS_IDS if name not in COUNT_ONLY},
        "magic": magic_analysis(scans, magic_limit, constants, hint_groups(hints) if hints else None),
        "structure": structure(scans, root),
        "build_special_cases": build_special_cases(root),
    }


ADDRESS_ROW = re.compile(r'^[^,]*,\s*_?(?:fun|dat)_[0-9a-f]{8}\s*,', re.M)


def map_address_names(root=ROOT):
    """Rows of config/symbols.csv whose symbol is a standard address name."""
    path = root / "config/symbols.csv"
    return len(ADDRESS_ROW.findall(path.read_text())) if path.is_file() else 0


def load_budget(path=BUDGET):
    return json.loads(path.read_text()) if path.is_file() else {}


def check(result, budget):
    """Classes over budget (growth) and under budget (the budget can be lowered)."""
    over, under = [], []
    for row in result["classes"]:
        allowed = budget.get(row["class"])
        if allowed is None:
            over.append((row["class"], row["count"], None))
        elif row["count"] > allowed:
            over.append((row["class"], row["count"], allowed))
        elif row["count"] < allowed:
            under.append((row["class"], row["count"], allowed))
    return over, under


def print_table(result, top=3):
    print(f"{'class':24} {'grp':3} {'count':>7} {'files':>5} {'vend':>5}  top files")
    for row in result["classes"]:
        files = result["per_file"][row["class"]]
        leaders = ", ".join(f"{Path(path).name}:{count}" for path, count in list(files.items())[:top])
        print(f"{row['class']:24} {row['group']:3} {row['count']:7} {row['files']:5} {row['vendored']:5}  {leaders}")


def describe_names(names):
    if not names:
        return "-"
    if "literals_it_names" in names:
        return (f"{names['group']} (used {names['used_names']}x; names {names['literals_it_names']}"
                f"/{names['distinct_literals']} literal values)")
    return f"{names['group']} (value overlap {names['coverage']})"


def print_magic(magic, count=20):
    print(f"Enum candidates ({magic['enum_candidate_fields']} fields with >= 3 distinct compared/assigned values):")
    for entry in magic["enum_candidates"][:count]:
        hint = describe_names(entry["existing_names"])
        values = ",".join(str(v) for v in entry["values"][:10]) + ("..." if len(entry["values"]) > 10 else "")
        print(f"  {entry['uses']:5} {entry['field']:40} [{entry['type'] or '?'}] named={entry['named_uses']} "
              f"values={values} names: {hint}")
    print(f"Flag candidates ({magic['flag_candidate_fields']} fields with >= 2 distinct single-bit masks):")
    for entry in magic["flag_candidates"][:count]:
        hint = describe_names(entry["existing_names"])
        bits = ",".join(entry["bits"][:10]) + ("..." if len(entry["bits"]) > 10 else "")
        print(f"  {entry['uses']:5} {entry['field']:40} [{entry['type'] or '?'}] named={entry['named_uses']} "
              f"bits={bits} names: {hint}")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--json", action="store_true", help="print the whole result as JSON")
    parser.add_argument("--class", dest="cls", choices=CLASS_IDS, help="restrict --list to one class")
    parser.add_argument("--list", action="store_true", help="print file:line findings")
    parser.add_argument("--magic", action="store_true", help="print enum and flag candidates")
    parser.add_argument("--top", type=int, default=3, help="top files per class in the table")
    parser.add_argument("--check", action="store_true", help="fail when a class exceeds its budget")
    parser.add_argument("--write-budget", action="store_true", help="lower the budget to the current counts")
    parser.add_argument("--budget", type=Path, default=BUDGET)
    parser.add_argument("--hints", type=Path,
                        help="another source tree whose enums/#defines are offered as names for literal values")
    args = parser.parse_args(argv)
    result = report(magic_limit=0 if args.json else 40, hints=args.hints)
    counts = {row["class"]: row["count"] for row in result["classes"]}
    if args.write_budget:
        old = load_budget(args.budget)
        budget.write(args.budget, budget.lowered(old, counts) if old else counts)
        print(f"Wrote {args.budget.relative_to(ROOT) if args.budget.is_relative_to(ROOT) else args.budget}")
        return 0
    if args.check:
        over, under = budget.compare(counts, load_budget(args.budget))
        status = budget.report("oddscan", over, under, "python3 tools/oddscan.py --write-budget")
        if over:
            print("oddscan: list a class with python3 tools/oddscan.py --list --class NAME "
                  "(docs/cleanup-audit.md)", file=sys.stderr)
        else:
            print(f"oddscan: all {len(result['classes'])} classes within budget")
        return status
    if args.json:
        json.dump(result, sys.stdout, indent=1, sort_keys=False)
        print()
        return 0
    if args.list:
        for name in [args.cls] if args.cls else CLASS_IDS:
            for item in result["findings"].get(name, ()):
                print(f"{name}\t{item}")
        return 0
    if args.magic:
        print_magic(result["magic"])
        return 0
    print_table(result, args.top)
    return 0


if __name__ == "__main__":
    sys.exit(main())
