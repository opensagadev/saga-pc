#!/usr/bin/env python3
"""Split src/saga.h into module headers behind an include-all shim (step 1 of docs/header-plan.md).

saga.h is cut into its top-level items: struct/union/enum/class definitions,
typedefs, forward declarations, function prototypes, extern variables, macros,
STATIC_ASSERTs, includes, and the `#pragma pack` regions and directive-only
conditionals around them. `config/header-map.csv` names the target header of
every item. The split writes each header's items there and turns saga.h into a
shim that includes the headers.

- Order: by default the items are reordered as far as that is inert
  (dependency_graph: a declaration stays after what it names, redeclarations
  and macros keep their order), so most headers are one block and the shim is
  a list of whole-header includes. `--exact-order` keeps saga.h order instead
  (token-neutral, most headers in many parts).
- A header that cannot be one block gets numbered parts in the same file: the
  shim includes it once per part, a direct include gives all parts at once
  (`--interleaving` lists them and why).
- Each item keeps its preprocessor context: the enclosing top-level `#if`
  branches and the `extern "C"` block are re-emitted around every run of items
  (original directive text where the run starts in the first branch, a derived
  `#if` otherwise).
- A header includes the headers its items use when all of them come earlier in
  the shim (a no-op through the shim); otherwise the header says which ones it
  still gets through saga.h only ("included via saga.h only for now").

    python3 tools/headersplit.py --build-map            # (re)write config/header-map.csv from the evidence
    python3 tools/headersplit.py --stats                # items per kind and target, coverage, unknowns
    python3 tools/headersplit.py --dry-run              # what the split would write, nothing written
    python3 tools/headersplit.py --interleaving         # headers that need parts, and why
    python3 tools/headersplit.py --apply --root DIR     # write the split into DIR/src (default: this tree)
    python3 tools/headersplit.py --check-counts src/legoapi/doors.c ...
    python3 tools/headersplit.py --check-neutral src/legoapi/doors.c ...   # the --exact-order split

Both checks preprocess each unit with clang -E and the lint flags
(tools/declscan.py), once in this tree and once in a scratch copy with the split
applied (build/headersplit/), one clang process at a time. `--check-neutral`
compares the token streams with line markers dropped; `extern "C" {` blocks are
compared by meaning (their braces are dropped and each token carries its
linkage). `--check-counts` compares the multiset of top-level declarations
(tokens, linkage, `#pragma pack` in effect; STATIC_ASSERT counter numbers
dropped) and the clang -fsyntax-only errors: the proof that a reordered split
gives every unit the same declarations; object identity is the build's proof.

The segmentation is lossless: the items and the context directives between them
give back saga.h byte for byte (`--roundtrip`).
"""
import argparse
import collections
import csv
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT  # noqa: E402

SAGA = "src/saga.h"
MAP = "config/header-map.csv"
SCRATCH = "build/headersplit"
MAP_FIELDS = ["key", "kind", "names", "line_start", "line_end", "lines", "target", "evidence", "detail"]
RESIDUE = ""  # target of an item that stays in saga.h
SHIM_NOTE = "included via saga.h only for now"

# Headers the Android tree keeps under platform names get neutral names here (no Android/iOS/OpenGL in src).
NEUTRAL_HEADERS = {
    "legoapi/misc/androidbatman.h": "legoapi/misc/gamemain.h",
    "nu2api/nu3d/ShaderManagerOpenGL.h": "nu2api/nu3d/ShaderManager.h",
    "nu2api/nuandroid/ios_graphics.h": "nu2api/nucore/graphics.h",
}
# Platform halves whose plain name a PC header already has.
PLATFORM_HEADERS = {"nudlist", "nugscn", "numtl", "nuportal", "nurndr", "bgproc"}


def neutral_path(path):
    """`path` (an Android tree path) as it is named in src."""
    if path in NEUTRAL_HEADERS:
        return NEUTRAL_HEADERS[path]
    path = path.replace("/android/", "/")
    stem, dot, suffix = path.rpartition(".")
    if stem.endswith("_android"):
        stem = stem[: -len("_android")]
        if stem.rpartition("/")[2] in PLATFORM_HEADERS:
            stem += "_platform"
    return stem + dot + suffix


# --------------------------------------------------------------------------------------------------------------
# Lexer


@dataclass
class Token:
    kind: str  # "id", "num", "str", "punct", "dir"
    text: str
    line: int  # 1-based first line
    end: int  # 1-based last line
    directive: str = ""  # for "dir": the directive word (if, ifdef, define, ...)
    rest: str = ""  # for "dir": the text after the directive word, comments removed


PUNCT2 = {"::", "->", "##"}


def strip_comments(text):
    """`text` with C and C++ comments replaced by one space (strings kept)."""
    out = []
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(" ")
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" ")
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def lex(text):
    """Tokens of `text`; a preprocessor directive (with its continuation lines) is one token."""
    tokens = []
    i = 0
    n = len(text)
    line = 1
    at_line_start = True
    while i < n:
        c = text[i]
        if c == "\n":
            line += 1
            i += 1
            at_line_start = True
            continue
        if c in " \t\r\f\v":
            i += 1
            continue
        if c == "\\" and i + 1 < n and text[i + 1] == "\n":
            i += 2
            line += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            line += text.count("\n", i, j)
            i = j
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            i = n if j < 0 else j
            continue
        if c == "#" and at_line_start:
            start_line = line
            j = i
            raw = []
            while j < n:
                k = text.find("\n", j)
                k = n if k < 0 else k
                segment = text[j:k]
                raw.append(segment)
                # A block comment that starts on the directive line can run on.
                stripped = strip_comments("\n".join(raw))
                if "/*" in segment and "*/" not in text[j:k] and segment.rfind("/*") > segment.rfind("*/"):
                    end = text.find("*/", k)
                    end = n if end < 0 else end + 2
                    newline = text.find("\n", end)
                    newline = n if newline < 0 else newline
                    raw[-1] = text[j:newline]
                    line += text.count("\n", k, newline)
                    k = newline
                    stripped = strip_comments("\n".join(raw))
                if stripped.rstrip().endswith("\\") and k < n:
                    j = k + 1
                    line += 1
                    continue
                j = k
                break
            body = strip_comments("\n".join(raw)).replace("\\\n", " ")
            match = re.match(r"\s*#\s*(\w*)\s*(.*)", body, re.S)
            tokens.append(Token("dir", "\n".join(raw), start_line, line, match[1], " ".join(match[2].split())))
            i = j
            at_line_start = False
            continue
        at_line_start = False
        if c.isalpha() or c == "_":
            j = i + 1
            while j < n and (text[j].isalnum() or text[j] == "_"):
                j += 1
            tokens.append(Token("id", text[i:j], line, line))
            i = j
            continue
        if c.isdigit() or (c == "." and i + 1 < n and text[i + 1].isdigit()):
            j = i + 1
            while j < n and (text[j].isalnum() or text[j] in "._" or (text[j] in "+-" and text[j - 1] in "eEpP")):
                j += 1
            tokens.append(Token("num", text[i:j], line, line))
            i = j
            continue
        if c in "\"'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            tokens.append(Token("str", text[i:j + 1], line, line))
            i = j + 1
            continue
        if text.startswith("...", i):
            tokens.append(Token("punct", "...", line, line))
            i += 3
            continue
        if text[i:i + 2] in PUNCT2:
            tokens.append(Token("punct", text[i:i + 2], line, line))
            i += 2
            continue
        tokens.append(Token("punct", c, line, line))
        i += 1
    return tokens


# --------------------------------------------------------------------------------------------------------------
# Segmentation


@dataclass
class Frame:
    """A top-level conditional: its branches' directive texts and conditions."""
    id: int
    directives: list  # Token per branch (#if/#ifdef/#ifndef, #elif..., #else)
    conditions: list  # C expression per branch ("" for #else)
    end: Token = None


@dataclass
class Item:
    index: int
    first: int  # first line of its own tokens
    last: int  # last line
    lead: int  # first line of the comment/blank lines before it that travel with it
    kind: str
    names: list
    refs: set
    context: tuple  # ((frame id, branch), ...)
    linkage: int  # extern "C" depth
    key: str = ""
    detail: str = ""
    carried: list = field(default_factory=list)  # comment lines before context directives just before it
    locals: set = field(default_factory=set)  # parameter names of its declarators: refs that bind no global


@dataclass
class Event:
    """One step of saga.h in order: an item, or a context directive between items."""
    kind: str  # "item", "open", "branch", "close", "link_open", "link_close", "guard"
    first: int
    last: int
    lead: int
    item: Item = None
    frame: Frame = None
    branch: int = 0


def condition_of(token):
    if token.directive == "ifdef":
        return f"defined({token.rest})"
    if token.directive == "ifndef":
        return f"!defined({token.rest})"
    if token.directive in ("if", "elif"):
        return token.rest
    return ""


def _match_endif(tokens, i):
    """Index of the #endif that closes the conditional opened at tokens[i]."""
    depth = 0
    for j in range(i, len(tokens)):
        t = tokens[j]
        if t.kind != "dir":
            continue
        if t.directive in ("if", "ifdef", "ifndef"):
            depth += 1
        elif t.directive == "endif":
            depth -= 1
            if depth == 0:
                return j
    raise ValueError(f"unterminated conditional at line {tokens[i].line}")


def _defines_own_condition(tokens, start, close):
    """Whether a #define/#undef inside the conditional names a macro its #if/#elif conditions test."""
    depth = 0
    tested = set()
    changed = set()
    for t in tokens[start:close + 1]:
        if t.kind != "dir":
            continue
        if t.directive in ("if", "ifdef", "ifndef"):
            depth += 1
            if depth == 1:
                tested.update(re.findall(r"[A-Za-z_]\w*", t.rest))
        elif t.directive == "elif" and depth == 1:
            tested.update(re.findall(r"[A-Za-z_]\w*", t.rest))
        elif t.directive == "endif":
            depth -= 1
        elif t.directive in ("define", "undef") and t.rest:
            changed.add(re.match(r"[A-Za-z_]\w*", t.rest)[0] if re.match(r"[A-Za-z_]\w*", t.rest) else "")
    return bool(tested & changed - {"defined"})


def _is_pack(t, word):
    return t.kind == "dir" and t.directive == "pragma" and re.match(rf"pack\s*\(\s*{word}\b", t.rest) is not None


def _match_pack(tokens, i):
    depth = 0
    for j in range(i, len(tokens)):
        if _is_pack(tokens[j], "push"):
            depth += 1
        elif _is_pack(tokens[j], "pop"):
            depth -= 1
            if depth == 0:
                return j
    raise ValueError(f"unterminated #pragma pack at line {tokens[i].line}")


def _ids(tokens):
    return [t.text for t in tokens if t.kind == "id"]


KEYWORDS = set("""
auto break case char const continue default do double else enum extern float for goto if inline int long register
restrict return short signed sizeof static struct switch typedef union unsigned void volatile while class public
private protected virtual operator template typename this new delete friend namespace using bool true false explicit
mutable __cdecl __stdcall __fastcall __thiscall __declspec __inline __forceinline __int64 __int32 __int16 __int8
__w64 __ptr32 __ptr64 __unaligned align noinline selectany dllimport dllexport naked noreturn novtable
defined STATIC_ASSERT offsetof
""".split())
CALLCONV = {"__cdecl", "__stdcall", "__fastcall", "__thiscall", "WINAPI", "CALLBACK", "APIENTRY", "STDMETHODCALLTYPE"}


def _skip_group(code, i, open_, close):
    depth = 0
    for j in range(i, len(code)):
        if code[j].text == open_:
            depth += 1
        elif code[j].text == close:
            depth -= 1
            if depth == 0:
                return j
    return len(code) - 1


def _top_level(code):
    """Tokens of `code` at paren/bracket/brace depth 0, with their index."""
    depth = 0
    out = []
    for i, t in enumerate(code):
        if t.text in "([{" and t.kind == "punct":
            if depth == 0:
                out.append((i, t))
            depth += 1
        elif t.text in ")]}" and t.kind == "punct":
            depth -= 1
            if depth == 0:
                out.append((i, t))
        elif depth == 0:
            out.append((i, t))
    return out


def classify(code, directives):
    """(kind, names) of one item from its code tokens and the directives inside it."""
    if not code:
        defines = [d.rest.split("(")[0].split()[0] for d in directives if d.directive == "define" and d.rest]
        if defines:
            return "macro", defines
        includes = [d.rest for d in directives if d.directive == "include"]
        if includes:
            return "include", [i.strip('<>"') for i in includes]
        if any(d.directive == "undef" for d in directives):
            return "undef", [d.rest for d in directives if d.directive == "undef"]
        if any(d.directive == "pragma" for d in directives):
            return "pragma", [d.rest for d in directives if d.directive == "pragma"]
        return "directive", []
    toks = list(code)
    # Leading linkage specification and declspecs.
    linkage = False
    while toks and toks[0].text == "extern" and len(toks) > 1 and toks[1].kind == "str":
        linkage = True
        toks = toks[2:]
    words = [t.text for t in toks]
    if words and words[0] == "STATIC_ASSERT":
        return "assert", []
    if words and words[0] == "template":
        end = _skip_group_angle(toks, 1)
        toks = toks[end + 1:]
        words = [t.text for t in toks]
    top = _top_level(toks)
    top_words = [t.text for _, t in top]
    has_body = "{" in top_words
    if words and words[0] == "typedef":
        if has_body:
            kw = next((w for w in words[:4] if w in ("struct", "union", "enum", "class")), "typedef")
            brace = next(i for i, t in top if t.text == "{")
            tag = []
            if kw in words[:4]:
                name = _tag_name(toks, words.index(kw) + 1)
                tag = [name] if name else []
            close = _skip_group(toks, brace, "{", "}")
            names = tag + _declarator_names(toks[close + 1:])
            names += _enumerators(toks[brace:close + 1]) if kw == "enum" else []
            return kw, names
        name = _typedef_name(toks)
        if len(words) >= 3 and words[1] in ("struct", "union", "enum", "class") and "(" not in words:
            return "forward", [words[2]] + ([name] if name and name != words[2] else [])
        return "typedef", [name] if name else []
    if words and words[0] in ("struct", "union", "enum", "class"):
        if has_body:
            brace = next(i for i, t in top if t.text == "{")
            name = _tag_name(toks, 1)
            close = _skip_group(toks, brace, "{", "}")
            names = ([name] if name else []) + _declarator_names(toks[close + 1:])
            names += _enumerators(toks[brace:close + 1]) if words[0] == "enum" else []
            return words[0], names
        if len(words) <= 3:
            return "forward", [words[1]]
    if has_body:
        brace = next(i for i, t in top if t.text == "{")
        before = [t for i, t in top if i < brace]
        if any(t.text == "(" for t in before):
            return "inline", [_function_name(toks[:brace])]
        if any(t.text == "=" for t in before):  # initialised variable
            return "definition", _variable_name(toks[:brace])
        return "block", []
    if "(" in top_words:
        paren = next(i for i, t in top if t.text == "(")
        inner = toks[paren + 1:_skip_group(toks, paren, "(", ")")]
        pointer = _pointer_declarator(inner)
        if pointer:
            kind = "variable" if "extern" in words or linkage else "definition"
            return kind, [pointer]
        name = _function_name(toks)
        if "=" in top_words[:top_words.index("(")]:
            return "definition", _variable_name(toks)
        return "function", [name] if name else []
    kind = "variable" if "extern" in words or linkage else "definition"
    return kind, _variable_name(toks)


def _tag_name(toks, k):
    """The tag after a class key at toks[k], past `__declspec(...)`; "" for an anonymous one."""
    while k < len(toks) and toks[k].text == "__declspec" and k + 1 < len(toks) and toks[k + 1].text == "(":
        k = _skip_group(toks, k + 1, "(", ")") + 1
    return toks[k].text if k < len(toks) and toks[k].kind == "id" and toks[k].text not in KEYWORDS else ""


def _pointer_declarator(inner):
    """NAME of a parenthesised pointer declarator `(__cdecl *NAME)` or `(*NAME[4])`, else None."""
    k = 0
    while k < len(inner) and inner[k].text in CALLCONV:
        k += 1
    if k >= len(inner) or inner[k].text != "*":
        return None
    while k < len(inner) and inner[k].text in ("*", "const"):
        k += 1
    if k < len(inner) and inner[k].kind == "id" and (k + 1 == len(inner) or inner[k + 1].text == "["):
        return inner[k].text
    return None


def _skip_group_angle(toks, i):
    depth = 0
    for j in range(i, len(toks)):
        if toks[j].text == "<":
            depth += 1
        elif toks[j].text == ">":
            depth -= 1
            if depth == 0:
                return j
    return i


def _declarator_names(toks):
    names = []
    for k, t in enumerate(toks):
        if t.kind == "id" and t.text not in KEYWORDS and k + 1 < len(toks) and toks[k + 1].text in (",", ";", "["):
            names.append(t.text)
    return names


def _enumerators(body):
    names = []
    depth = 0
    for k, t in enumerate(body):
        if t.text in "({[" and t.kind == "punct":
            depth += 1
        elif t.text in ")}]" and t.kind == "punct":
            depth -= 1
        elif depth == 1 and t.kind == "id" and body[k - 1].text in ("{", ",") and \
                k + 1 < len(body) and body[k + 1].text in ("=", ",", "}"):
            names.append(t.text)
    return names


def _typedef_name(toks):
    top = _top_level(toks)
    for i, t in top:
        if t.text == "(":
            inner = toks[i + 1:_skip_group(toks, i, "(", ")")]
            name = _pointer_declarator(inner)
            if name:
                return name
            # parenthesised function type: typedef void (__cdecl NAME)(args);
            named = [t for t in inner if t.text not in CALLCONV]
            if len(named) == 1 and named[0].kind == "id" and named[0].text not in KEYWORDS:
                return named[0].text
            # function type: typedef i32 NAME(args);
            if i > 0 and toks[i - 1].kind == "id" and toks[i - 1].text not in KEYWORDS:
                return toks[i - 1].text
            break
    ids = [t for _, t in top if t.kind == "id" and t.text not in KEYWORDS and not ATTRIBUTE_MACRO.match(t.text)]
    return ids[-1].text if ids else ""


# Attribute macros after a typedef name (Android: `typedef int64_t i64 SAGA_I386_ALIGN;`).
ATTRIBUTE_MACRO = re.compile(r"SAGA_\w*ALIGN\w*$|__attribute__$")


def _function_name(toks):
    top = _top_level(toks)
    for n, (i, t) in enumerate(top):
        if t.text != "(":
            continue
        prev = toks[i - 1] if i > 0 else None
        if prev is None:
            continue
        if prev.text in ("__declspec", "__pragma"):
            continue
        if prev.kind == "id" and prev.text not in KEYWORDS - {"operator"}:
            if i >= 2 and toks[i - 2].text == "operator":
                return "operator" + prev.text
            return prev.text
        if prev.text == "operator" or (i >= 2 and toks[i - 2].text == "operator"):
            return "operator" + prev.text
    return ""


def _variable_name(toks):
    top = _top_level(toks)
    for n, (i, t) in enumerate(top):
        if t.text in ("[", "=", ";", ":") and i > 0 and toks[i - 1].kind == "id":
            return [toks[i - 1].text]
    ids = [t.text for _, t in top if t.kind == "id" and t.text not in KEYWORDS]
    return ids[-1:] if ids else []


def segment(text):
    """The events of a header in order (see Event); raises ValueError on a structure it cannot follow."""
    tokens = lex(text)
    lines = text.split("\n")
    events = []
    frames = []  # open top-level frames: [Frame, branch]
    linkage = 0
    all_frames = []
    i = 0
    n = len(tokens)
    guard = None
    # `#pragma once` first (the project's form), or an include guard: #ifndef X / #define X first, the matching
    # #endif last (older inputs).
    if n >= 1 and tokens[0].kind == "dir" and tokens[0].directive == "pragma" and tokens[0].rest == "once":
        guard = (tokens[0],)
        events.append(Event("guard", tokens[0].line, tokens[0].end, 1))
        tokens = tokens[1:]
        n = len(tokens)
    elif n >= 2 and tokens[0].kind == "dir" and tokens[0].directive == "ifndef" and \
            tokens[1].kind == "dir" and tokens[1].directive == "define" and tokens[1].rest == tokens[0].rest:
        close = _match_endif(tokens, 0)
        if close == n - 1:
            guard = (tokens[0], tokens[1], tokens[-1])
            events.append(Event("guard", tokens[0].line, tokens[1].end, 1))
            tokens = tokens[2:-1]
            n = len(tokens)

    def add(event):
        events.append(event)

    while i < n:
        t = tokens[i]
        # Linkage markers: `#ifdef __cplusplus / extern "C" { / #endif` and `#ifdef __cplusplus / } / #endif`.
        if t.kind == "dir" and t.directive == "ifdef" and t.rest == "__cplusplus" and i + 4 < n + 1:
            nxt = tokens[i + 1:i + 5]
            if len(nxt) >= 4 and [x.text for x in nxt[:3]] == ["extern", '"C"', "{"] and \
                    nxt[3].kind == "dir" and nxt[3].directive == "endif":
                linkage += 1
                add(Event("link_open", t.line, nxt[3].end, 0))
                i += 5
                continue
            if len(nxt) >= 2 and nxt[0].text == "}" and nxt[1].kind == "dir" and nxt[1].directive == "endif" \
                    and linkage > 0:
                linkage -= 1
                add(Event("link_close", t.line, nxt[1].end, 0))
                i += 3
                continue
        if t.kind == "id" and t.text == "extern" and i + 2 < n and tokens[i + 1].text == '"C"' and \
                tokens[i + 2].text == "{":
            linkage += 1
            add(Event("link_open", t.line, tokens[i + 2].end, 0))
            i += 3
            continue
        if t.kind == "punct" and t.text == "}" and linkage > 0:
            linkage -= 1
            add(Event("link_close", t.line, t.end, 0))
            i += 1
            continue
        if t.kind == "dir":
            if t.directive in ("if", "ifdef", "ifndef"):
                close = _match_endif(tokens, i)
                body = tokens[i + 1:close]
                if all(x.kind == "dir" for x in body) or _defines_own_condition(tokens, i, close):
                    # Directive-only conditional (#ifndef MIN / #define MIN / #endif), or one that changes its
                    # own condition (#ifndef X / #define X ...): one item, since re-evaluating its condition
                    # around a part of it would give another answer.
                    add(_make_item(tokens[i:close + 1], frames, linkage, events))
                    i = close + 1
                    continue
                frame = Frame(len(all_frames), [t], [condition_of(t)])
                all_frames.append(frame)
                frames.append([frame, 0])
                add(Event("open", t.line, t.end, 0, frame=frame, branch=0))
                i += 1
                continue
            if t.directive in ("elif", "else"):
                if not frames:
                    raise ValueError(f"#{t.directive} without #if at line {t.line}")
                frame = frames[-1][0]
                frame.directives.append(t)
                frame.conditions.append(condition_of(t))
                frames[-1][1] += 1
                add(Event("branch", t.line, t.end, 0, frame=frame, branch=frames[-1][1]))
                i += 1
                continue
            if t.directive == "endif":
                if not frames:
                    raise ValueError(f"#endif without #if at line {t.line}")
                frame = frames.pop()[0]
                frame.end = t
                add(Event("close", t.line, t.end, 0, frame=frame))
                i += 1
                continue
            if _is_pack(t, "push"):
                close = _match_pack(tokens, i)
                add(_make_item(tokens[i:close + 1], frames, linkage, events))
                i = close + 1
                continue
            add(_make_item([t], frames, linkage, events))
            i += 1
            continue
        # A declaration: up to `;` at depth 0, or a function body's closing brace.
        j = i
        depth = 0
        nested_if = 0
        end = None
        while j < n:
            x = tokens[j]
            if x.kind == "dir":
                if x.directive in ("if", "ifdef", "ifndef"):
                    nested_if += 1
                elif x.directive == "endif":
                    nested_if -= 1
            elif x.text == "{" and x.kind == "punct":
                depth += 1
            elif x.text == "}" and x.kind == "punct":
                depth -= 1
                if depth == 0 and nested_if == 0:
                    following = tokens[j + 1] if j + 1 < n else None
                    opening = _opening_brace(tokens, i, j)
                    before = tokens[opening - 1] if opening > i else None
                    if before is not None and before.text in (")", "const") and \
                            (following is None or following.text != ";"):
                        end = j
                        break
            elif x.text == ";" and depth == 0 and nested_if == 0:
                end = j
                break
            j += 1
        if end is None:
            raise ValueError(f"unterminated declaration at line {t.line}")
        # Keep the rest of the last line with this item (`} X; struct Y;` on one line).
        while end + 1 < n and tokens[end + 1].line == tokens[end].end and tokens[end + 1].kind != "dir":
            k = end + 1
            depth = 0
            while k < n:
                if tokens[k].text == "{":
                    depth += 1
                elif tokens[k].text == "}":
                    depth -= 1
                elif tokens[k].text == ";" and depth == 0:
                    break
                k += 1
            end = min(k, n - 1)
        add(_make_item(tokens[i:end + 1], frames, linkage, events))
        i = end + 1
    if frames:
        raise ValueError(f"unterminated conditional opened at line {frames[-1][0].directives[0].line}")
    # Leading comment/blank lines travel with the next event.
    previous_last = events[0].last if events and events[0].kind == "guard" else 0
    for event in events:
        if event.kind == "guard":
            continue
        event.lead = previous_last + 1
        if event.item:
            event.item.lead = event.lead
        previous_last = event.last
    carried = []
    for event in events:
        if event.kind == "guard":
            continue
        if event.item:
            event.item.carried = carried
            carried = []
        elif event.first > event.lead:
            carried.append((event.lead, event.first - 1))
    items = [e.item for e in events if e.item]
    keys = collections.Counter()
    for item in items:
        base = f"{item.kind}:{item.names[0] if item.names else '-'}"
        keys[base] += 1
        item.key = base if keys[base] == 1 else f"{base}#{keys[base]}"
    return Segmentation(text, lines, events, items, all_frames, guard)


def _opening_brace(tokens, start, close):
    depth = 0
    for k in range(close, start - 1, -1):
        if tokens[k].text == "}":
            depth += 1
        elif tokens[k].text == "{":
            depth -= 1
            if depth == 0:
                return k
    return start


NOT_A_NAME_AFTER = {"struct", "union", "enum", "class", "const", "volatile", "typename"}


def _parameter_names(code):
    """The names a declaration gives its parameters (also of function pointer parameters): the last identifier
    of a parameter that follows a type (an identifier, `*` or `&`), before any array bounds or default."""
    out = set()
    stack = []
    for i, t in enumerate(code):
        if t.text == "(":
            stack.append(i)
        elif t.text == ")" and stack:
            start = stack.pop()
            depth = 0
            parameter = []
            for u in code[start + 1:i] + [Token("punct", ",", 0, 0)]:
                if u.text in "([" and u.kind == "punct":
                    depth += 1
                elif u.text in ")]" and u.kind == "punct":
                    depth -= 1
                if depth == 0 and u.text == ",":
                    head = []
                    for v in parameter:
                        if v.text in ("=", "["):
                            break
                        head.append(v)
                    if len(head) >= 2 and head[-1].kind == "id" and head[-1].text not in KEYWORDS:
                        before = head[-2]
                        if (before.kind == "id" and before.text not in NOT_A_NAME_AFTER) or before.text in "*&":
                            out.add(head[-1].text)
                    parameter = []
                else:
                    parameter.append(u)
    return out


def _make_item(toks, frames, linkage, events):
    code = [t for t in toks if t.kind != "dir"]
    directives = [t for t in toks if t.kind == "dir"]
    if directives and _is_pack(directives[0], "push"):
        inner = segment_names(toks[1:-1])
        kind, names = "pack", inner
    elif directives and not code and directives[0].directive in ("if", "ifdef", "ifndef"):
        kind, names = classify([], directives[1:-1])
    elif toks[0].kind == "dir" and toks[0].directive in ("if", "ifdef", "ifndef"):
        kind, names = "block", segment_names(toks[1:-1])
    else:
        kind, names = classify(code, directives)
    refs = set()
    for t in toks:
        if t.kind == "id":
            refs.add(t.text)
        elif t.kind == "dir" and t.directive in ("define", "if", "elif", "ifdef", "ifndef"):
            refs.update(re.findall(r"[A-Za-z_]\w*", t.rest))
    names = [x for x in dict.fromkeys(names) if x]
    refs -= set(names)
    refs -= KEYWORDS
    context = tuple((f.id, b) for f, b in frames)
    item = Item(len([e for e in events if e.item]), toks[0].line, toks[-1].end, 0, kind, names, refs, context,
                linkage)
    if kind in ("function", "variable", "typedef"):
        item.locals = _parameter_names(code) & refs
    return Event("item", toks[0].line, toks[-1].end, 0, item=item)


def segment_names(toks):
    """Names declared by a token run (the inside of a #pragma pack region)."""
    names = []
    i = 0
    while i < len(toks):
        if toks[i].kind == "dir":
            i += 1
            continue
        j = i
        depth = 0
        while j < len(toks):
            if toks[j].text == "{":
                depth += 1
            elif toks[j].text == "}":
                depth -= 1
            elif toks[j].text == ";" and depth == 0:
                break
            j += 1
        code = [t for t in toks[i:j + 1] if t.kind != "dir"]
        names += classify(code, [])[1]
        i = j + 1
    return names


@dataclass
class Segmentation:
    text: str
    lines: list
    events: list
    items: list
    frames: list
    guard: tuple

    def event_text(self, event):
        return "\n".join(self.lines[event.lead - 1:event.last])

    def own_text(self, event):
        return "\n".join(self.lines[event.first - 1:event.last])

    def lead_text(self, event):
        return "\n".join(self.lines[event.lead - 1:event.first - 1]) if event.first > event.lead else None

    def roundtrip(self):
        """saga.h rebuilt from the events (must equal the original)."""
        parts = []
        last = 0
        for event in self.events:
            if event.kind == "guard":
                parts.append("\n".join(self.lines[0:event.last]))
                last = event.last
                continue
            parts.append("\n".join(self.lines[event.lead - 1:event.last]))
            last = event.last
        tail = self.lines[last:]
        parts.append("\n".join(tail))
        return "\n".join(parts)


# --------------------------------------------------------------------------------------------------------------
# Evidence and the map

DECLARED_KINDS = {"struct", "union", "enum", "class", "typedef", "function", "variable", "macro", "inline",
                  "definition", "pack"}
# Android headers that are catch-alls or scaffolding themselves: a name found only there is weak evidence.
ANDROID_WEAK = re.compile(r"(^|/)(decomp|globals|batman)\.h$|stubs|_stubs_types\.h$")
# Decomp-only scaffolding of saga.h.
DECOMP_MACROS = {"SAGA_JOIN_IMPL", "SAGA_JOIN", "STATIC_ASSERT", "STUBBED", "STUBBED_NAMED", "U64_PAIR",
                 "_WIN32_WINNT", "WIN32_LEAN_AND_MEAN", "NOMINMAX"}
VIEW_NAME = re.compile(r"(_VIEW|View|_VIEW_PC)$")
# The libogg/libvorbis API that saga.h carries (lib/ogg-vorbis/vorbis/ogg/ogg.h includes saga.h for it).
LIBRARY_NAME = re.compile(r"(ogg|oggpack|oggpackB|vorbis|ov|drft|_vorbis|_ogg)_\w+$|^OggVorbis_File$|^ogg_int\d+_t$")
# The original upstream headers that saga.h carries verbatim (declarations token-identical; checked against libogg
# 1.1.3 and libvorbis 1.2.0). lib/ogg-vorbis materialises stubs at these paths that include saga.h.
# The upstream header each copy in saga.h comes from. lib/ogg-vorbis (config/ogg-vorbis.json) materialises the
# real libvorbis vorbis/codec.h and vorbis/vorbisfile.h, but its vorbis/ogg/ogg.h, ogg/os_types.h and smallft.h
# are stubs that include saga.h, where the copies live. Until the pin carries the upstream files, the split moves
# the copies into these holding headers (not named like the library headers: MSVC's quoted-include search would
# find src/ogg/ogg.h before the stub from a vendored unit); they go when the stubs become the real headers.
LIBRARY_HEADERS = {
    "ogg/ogg.h": ("transitional/libogg/ogg.h", "libogg 1.1.3 include/ogg/ogg.h"),
    "ogg/os_types.h": ("transitional/libogg/os_types.h", "libogg 1.1.3 include/ogg/os_types.h (_WIN32 MSVC branch)"),
    "smallft.h": ("transitional/libvorbis/smallft.h", "libvorbis 1.2.0 lib/smallft.h (same declarations)"),
}
LIBRARY_ORIGIN = {holding: f"{origin}; replaces lib stub {real}"
                  for real, (holding, origin) in LIBRARY_HEADERS.items()}


def library_header(name):
    """The holding header of a saga.h copy of a libogg/libvorbis declaration, or None."""
    if re.match(r"ogg_u?int\d+_t$", name):
        return LIBRARY_HEADERS["ogg/os_types.h"][0]
    if name.startswith("drft_"):
        return LIBRARY_HEADERS["smallft.h"][0]
    if LIBRARY_NAME.match(name):
        return LIBRARY_HEADERS["ogg/ogg.h"][0]
    return None


# SDK/CRT header names a module header must not take (MSVC searches the directories of every open includer for a
# quoted include); the full list comes from the pinned SDK when it is installed.
SHADOWED_DEFAULT = {"windows.h", "signal.h", "traffic.h", "assert.h", "string.h", "math.h", "time.h", "memory.h",
                    "process.h", "errno.h", "limits.h", "float.h", "stdio.h", "stdlib.h", "ctype.h", "locale.h"}


def unshadowed(target, shadowed):
    """`target`, or `<dir>/<area>_<name>` when its file name is an SDK/CRT header name."""
    path = Path(target)
    if path.name.lower() not in shadowed or "/" not in target:
        return target
    return f"{path.parent.as_posix()}/{target.split('/')[0]}_{path.name}"
SDK_HEADER = {"binkw32": "bink", "d3d9": "d3d9", "d3dx9_35": "d3dx9", "dinput8": "dinput", "dsound": "dsound",
              "xinput1_3": "xinput"}
SDK_DEFS = tuple(SDK_HEADER)


def read_text(path):
    return Path(path).read_text(encoding="latin-1")


def demangle(symbol):
    """Plain name of a free function or global from its decorated linker name (methods give None)."""
    if symbol.startswith("?"):
        match = re.match(r"\?([A-Za-z_]\w*)@@", symbol)
        return match[1] if match else None
    match = re.match(r"[_@]?([A-Za-z_]\w*?)(?:@\d+)?$", symbol)
    return match[1] if match else None


class Evidence:
    """The facts the map is built from (all read once)."""

    def __init__(self, root=ROOT, android=None):
        self.root = Path(root)
        self.android = Path(android) if android else None
        self.android_headers = collections.defaultdict(set)  # name -> Android headers that declare it
        self.android_files = collections.defaultdict(set)  # file stem -> Android paths (headers and units)
        self.android_failed = []
        self.vendored = collections.defaultdict(set)  # name -> vendored library header
        self.sdk_exports = {}  # name -> DLL
        self.compiland_of_address = []
        self.symbol_compiland = {}  # name -> (compiland, inner)
        self.file_compiland = {}  # current src file -> (compiland, inner)
        self.android_unit = {}  # compiland -> Android unit names
        self.users = collections.defaultdict(set)  # identifier -> current src files using it
        self.definers = collections.defaultdict(set)  # identifier -> current src files defining it (globals)
        self.data_address = {}  # global -> retail address (outside .text)
        self.text_range = (0, 0)
        self._folded = None
        self.externals_names = set()
        self.uses = collections.defaultdict(dict)  # identifier -> {current src file: occurrences}
        self.lib_users = set()  # identifiers of the vendored library units
        self.compiland_start = {}  # (compiland, inner) -> lowest retail address
        self.shadowed = set(SHADOWED_DEFAULT)
        for directory in ("build/tools/msvc8/include", "build/tools/msvc8/PlatformSDK/Include"):
            if (self.root / directory).is_dir():
                self.shadowed |= {p.name.lower() for p in (self.root / directory).rglob("*.h")}
        self._load()

    def _load(self):
        if self.android and (self.android / "src").is_dir():
            base = self.android / "src"
            for path in sorted(base.rglob("*")):
                if path.suffix not in (".h", ".c", ".cpp") or not path.is_file():
                    continue
                relative = path.relative_to(base).as_posix()
                self.android_files[path.stem.lower()].add(relative)
                if path.suffix != ".h":
                    continue
                try:
                    seg = segment(read_text(path))
                except ValueError as error:
                    self.android_failed.append(f"{relative}: {error}")
                    continue
                for item in seg.items:
                    if item.kind in DECLARED_KINDS:
                        for name in item.names:
                            self.android_headers[name].add(relative)
        vendored = self.root / "lib/ogg-vorbis/vorbis"
        if vendored.is_dir():
            for path in sorted(vendored.rglob("*.h")):
                try:
                    seg = segment(read_text(path))
                except ValueError:
                    continue
                for item in seg.items:
                    if item.kind in DECLARED_KINDS:
                        for name in item.names:
                            self.vendored[name].add(path.relative_to(vendored).as_posix())
        self.same_entity = []
        sibling = self.root / "config/sibling-names.csv"
        if sibling.is_file():
            for row in csv.DictReader(open(sibling, encoding="utf-8")):
                if row.get("kind") == "global" and row.get("original_name"):
                    self.same_entity.append((row["our_name"], row["original_name"]))
        externals = self.root / "src/externals.h"
        if externals.is_file():
            for item in segment(read_text(externals)).items:
                self.externals_names.update(item.names)
        for dll in SDK_DEFS:
            path = self.root / "tools" / f"{dll}.def"
            if path.is_file():
                for line in read_text(path).splitlines():
                    match = re.match(r"\s+_?([A-Za-z]\w*?)(?:@\d+)?(?:\s|$)", line)
                    if match and not line.strip().startswith(";"):
                        self.sdk_exports.setdefault(match[1], dll)
        rows = list(csv.DictReader(open(self.root / "config/layout-target.csv", encoding="utf-8")))
        ranges = []
        for row in rows:
            compiland = row["compiland"]
            inner = row["inner_file"] if row["inner_file"] not in ("", "?") else ""
            ranges.append((int(row["start"], 16), int(row["end"], 16), compiland, inner))
            if compiland:
                key = (compiland, inner)
                self.compiland_start[key] = min(self.compiland_start.get(key, 1 << 32), int(row["start"], 16))
            if compiland:
                self.android_unit.setdefault(compiland, set()).update(row["android_unit"].split())
            weights = collections.Counter()
            for part in row["current_source_files"].split():
                name, _, size = part.rpartition(":")
                weights[name] += int(size or 0)
            for name, size in weights.items():
                best = self.file_compiland.get(name)
                if compiland and (best is None or best[2] < size):
                    self.file_compiland[name] = (compiland, inner, size)
        ranges.sort()
        starts = [r[0] for r in ranges]
        self.text_range = (ranges[0][0], max(r[1] for r in ranges))
        import bisect  # noqa: PLC0415
        for row in csv.DictReader(open(self.root / "config/symbols.csv", encoding="utf-8")):
            if not row["address"].startswith("0x"):
                continue
            name = demangle(row["symbol"])
            if not name:
                continue
            address = int(row["address"], 16)
            if not self.text_range[0] <= address < self.text_range[1]:
                self.data_address.setdefault(name, address)
                continue
            k = bisect.bisect_right(starts, address) - 1
            if k >= 0 and ranges[k][0] <= address < max(ranges[k][1], ranges[k][0] + 1) and ranges[k][2]:
                self.symbol_compiland.setdefault(name, (ranges[k][2], ranges[k][3]))
        identifier = re.compile(r"[A-Za-z_]\w*")
        definition = re.compile(r"^[ \t]*(?!extern\b|typedef\b|static\b|return\b)[A-Za-z_][\w \t\*]*?\b([A-Za-z_]\w*)"
                                r"[ \t]*(?:\[[^\]\n]*\][ \t]*)*(?:=[^=]|;)", re.M)
        for path in sorted((self.root / "src").rglob("*")):
            if path.suffix not in (".c", ".cpp") or not path.is_file():
                continue
            relative = path.relative_to(self.root / "src").as_posix()
            text = strip_comments(read_text(path))
            for name, count in collections.Counter(identifier.findall(text)).items():
                self.users[name].add(relative)
                self.uses[name][relative] = count
            for match in definition.finditer(text):
                self.definers[match[1]].add(relative)
        vendored_units = self.root / "lib/ogg-vorbis"
        if vendored_units.is_dir():
            for path in sorted(vendored_units.rglob("*.c")):
                self.lib_users.update(identifier.findall(strip_comments(read_text(path))))

    # -- helpers ------------------------------------------------------------------------------------------
    def android_choice(self, name):
        """(header, weak, alternatives) of the Android header that declares `name`, or None."""
        headers = sorted(self.android_headers.get(name, ()))
        if not headers:
            return None
        strong = [h for h in headers if not ANDROID_WEAK.search(h)]
        pool = strong or headers
        pool.sort(key=lambda h: (h.count("/") == 0, len(h), h))
        return neutral_path(pool[0]), not strong, [neutral_path(h) for h in headers if h != pool[0]]

    def android_choice_folded(self, name):
        """android_choice of the Android name that differs from `name` only in case (gameobject_s -> GameObject_s)."""
        if self._folded is None:
            self._folded = collections.defaultdict(set)
            for known in self.android_headers:
                self._folded[known.lower()].add(known)
        spellings = sorted(self._folded.get(name.lower(), set()) - {name})
        if len(spellings) != 1 or not re.search(r"_s$|_t$", name):
            return None
        choice = self.android_choice(spellings[0])
        return (*choice, spellings[0]) if choice else None

    def header_for_compiland(self, compiland, inner):
        """Target header of a PC compiland (or of one file of a unity compiland), Android path when it exists."""
        stem = Path(inner or compiland).stem
        android = sorted(self.android_files.get(stem.lower(), ()))
        headers = [a for a in android if a.endswith(".h")]
        if headers:
            headers.sort(key=lambda h: (h.endswith("_types.h"), len(h)))
            return neutral_path(headers[0]), "android-path"
        units = [a for a in android if not a.endswith(".h")]
        if units:
            return neutral_path(f"{Path(units[0]).parent.as_posix()}/{Path(units[0]).stem}.h"), "android-dir"
        directory = Path(compiland).parent.as_posix()
        return f"{directory}/{stem}.h", "pc-path"

    def files_per_compiland(self, name):
        """Counter of (compiland, inner) -> number of current src files that use `name`."""
        found = collections.Counter()
        for path in self.users.get(name, ()):
            owner = self.file_compiland.get(path)
            if owner and path != "data.cpp":
                found[owner[:2]] += 1
        return found

    def heaviest_user(self, names):
        """(compiland, inner), occurrences: the compiland whose units use `names` most (ties: lowest address)."""
        weight = collections.Counter()
        for name in names:
            for path, count in self.uses.get(name, {}).items():
                # A file outside the unit table (a newer PC-only file) stands for itself.
                owner = self.file_compiland.get(path, (path, ""))
                if path != "data.cpp":
                    weight[owner[:2]] += count
        if not weight:
            return None, 0
        best = max(weight.values())
        tied = sorted((self.compiland_start.get(c, 1 << 32), c) for c, n in weight.items() if n == best)
        return tied[0][1], best

    def compilands_using(self, name):
        found = set()
        for path in self.users.get(name, ()):
            if path == "data.cpp":
                continue
            owner = self.file_compiland.get(path)
            if owner:
                found.add(owner[:2])
        return found


EXTERNALS_PREFIX = (("IDirectSound", "dsound"), ("PC_DS_", "dsound"), ("DSOUND", "dsound"), ("ID3DX", "d3dx9"),
                    ("D3DX", "d3dx9"), ("IDirect3D", "d3d9"), ("D3D", "d3d9"), ("IDirectInput", "dinput"),
                    ("DI", "dinput"), ("Bink", "bink"), ("BINK", "bink"), ("XINPUT", "xinput"))


def externals_header(name):
    """externals/<sdk>.h for a declaration that stands in for an SDK header the build does not have."""
    return "externals/" + next((h for prefix, h in EXTERNALS_PREFIX if name.startswith(prefix)), "sdk") + ".h"


def pc_stems(name):
    """Android spellings a PC name may stand for (GIZMOSET_PC -> GIZMOSET, NuThingPc -> NuThing)."""
    stems = []
    for suffix, replacement in (("_PC", ""), ("_pc_s", "_s"), ("_PC_s", "_s"), ("_pc", ""), ("Pc", ""), ("PC", ""),
                                ("_PC_VIEW", ""), ("_VIEW", "")):
        if name.endswith(suffix) and len(name) > len(suffix) + 1:
            stems.append(name[:-len(suffix)] + replacement)
    return stems


def decide(item, ev, seg):
    """(target, evidence, detail) of one item before the dependent rules (asserts, forwards, undefs)."""
    names = item.names
    first = names[0] if names else ""
    if item.kind == "include":
        if first in ("externals.h",):
            return "decomp.h", "decomp", "platform prelude: externals.h"
        if first.startswith("debug/"):
            return "decomp.h", "decomp", "diagnostics"
        if seg and item.context and any(seg.frames[f].conditions[0] == "defined(SAGA_SHADER_PARSER_TYPES)"
                                for f, _ in item.context):
            return None, "follow-next", ""
        return "decomp.h", "decomp", "platform prelude: SDK include"
    if item.kind == "macro" and set(names) & DECOMP_MACROS:
        return "decomp.h", "decomp", "scaffolding macro"
    if item.kind in ("struct", "union", "typedef", "forward", "class") and any(VIEW_NAME.search(n) for n in names):
        return "decomp.h", "decomp", "view type"
    for name in names:
        header = library_header(name)
        if header:
            return header, "library", LIBRARY_ORIGIN[header]
        if name in ev.sdk_exports:
            dll = ev.sdk_exports[name]
            return f"externals/{SDK_HEADER[dll]}.h", "externals", f"SDK {dll}.dll, no SDK header"
    for name in names:
        choice = ev.android_choice(name)
        if choice and not choice[1]:
            header, _, others = choice
            return header, "android", name + (" (also " + " ".join(others[:3]) + ")" if others else "")
    if item.kind in ("function", "inline", "variable", "definition"):
        for name in names:
            owner = ev.symbol_compiland.get(name)
            if owner:
                header, how = ev.header_for_compiland(*owner)
                return header, "symbol", f"{owner[0]}{'[' + owner[1] + ']' if owner[1] else ''} {how}"
        for name in names:
            defined = {p for p in ev.definers.get(name, ()) if p != "data.cpp"}
            owners = {ev.file_compiland[p][:2] for p in defined if p in ev.file_compiland}
            if len(owners) == 1:
                owner = owners.pop()
                header, how = ev.header_for_compiland(*owner)
                return header, "definition", f"{owner[0]}{'[' + owner[1] + ']' if owner[1] else ''} {how}"
    for name in names:
        for stem in pc_stems(name) + [name]:
            choice = ev.android_choice(stem) or ev.android_choice_folded(stem)
            if choice and not choice[1]:
                return choice[0], "android-stem", f"{name} -> {choice[3] if len(choice) > 3 else stem}"
    for name in names:
        owners = ev.compilands_using(name)
        if len(owners) == 1:
            owner = owners.pop()
            header, how = ev.header_for_compiland(*owner)
            return header, "users", f"{name}: only {owner[0]}{'[' + owner[1] + ']' if owner[1] else ''} {how}"
    if item.kind == "assert":
        return None, "follow-type", ""
    if item.kind in ("undef",):
        return None, "follow-next", ""
    users = collections.Counter()
    for name in names:
        users.update(ev.files_per_compiland(name))
    if not users and item.kind in ("function", "variable"):
        return RESIDUE, "unknown", "no user in src/"
    if not users:
        return RESIDUE, "unknown", "used in saga.h only"
    top = ", ".join(f"{c}{'[' + i + ']' if i else ''} {n}" for (c, i), n in users.most_common(3))
    return RESIDUE, "unknown", f"{len(users)} compilands use it, most: {top}"


ASSERT_TYPE = re.compile(r"STATIC_ASSERT\s*\(\s*(?:sizeof|offsetof)\s*\(\s*(?:struct\s+|union\s+|enum\s+)?(\w+)")


def build_map(seg, ev, previous=None):
    """Map rows for every item of `seg` (reviewed rows of `previous` are kept)."""
    reviewed = {row["key"]: row for row in (previous or []) if row.get("evidence") == "review"}
    decisions = {}
    owner_of_name = {}
    for item in seg.items:
        decisions[item.index] = decide(item, ev, seg)
        if item.key in reviewed:
            decisions[item.index] = (reviewed[item.key]["target"], "review", reviewed[item.key]["detail"])
    _propagate(seg, ev, decisions)
    _same_entity(seg, ev, decisions)
    all_names = {name for item in seg.items for name in item.names}
    owner_of_name.update(_owners(seg, decisions))
    for item in seg.items:
        target, how, detail = decisions[item.index]
        if how == "review":
            continue
        if item.kind == "forward" or (how == "unknown" and item.kind in ("typedef",)):
            for name in item.names:
                if owner_of_name.get(name) and owner_of_name[name] != target:
                    decisions[item.index] = (owner_of_name[name], "follow-definition", name)
                    break
        if how in ("follow-type", "unknown") and item.kind == "assert":
            text = seg.own_text(Event("item", item.first, item.last, item.lead))
            match = ASSERT_TYPE.search(text)
            if match is None:
                decisions[item.index] = ("decomp.h", "decomp", "data model check")
            elif match[1] in owner_of_name and owner_of_name[match[1]] in LIBRARY_ORIGIN:
                decisions[item.index] = ("decomp.h", "decomp", f"layout check of library type {match[1]}")
            elif match[1] in owner_of_name:
                decisions[item.index] = (owner_of_name[match[1]], "follow-type", match[1])
            elif match[1] in ev.externals_names:
                decisions[item.index] = (externals_header(match[1]), "externals",
                                         f"layout check of {match[1]} (src/externals.h)")
            elif match[1] not in all_names:
                decisions[item.index] = ("decomp.h", "decomp", f"data model check of SDK type {match[1]}")
            else:
                decisions[item.index] = (RESIDUE, "unknown", f"asserts on {match[1]}")
    _rule(seg, ev, decisions)
    _data_nearest(seg, ev, decisions)
    _follow_types(seg, ev, decisions, all_names)
    _unused(seg, ev, decisions)
    # Items that follow the next item (an #undef before a declaration, an include inside a block).
    for position in range(len(seg.items) - 1, -1, -1):
        item = seg.items[position]
        if decisions[item.index][1] == "follow-next":
            nxt = decisions[seg.items[position + 1].index] if position + 1 < len(seg.items) else (RESIDUE, "", "")
            decisions[item.index] = (nxt[0], "follow-next", seg.items[position + 1].key if nxt[0] else "")
    rows = []
    for item in seg.items:
        target, how, detail = decisions[item.index]
        if target in CATCH_ALL and not detail.startswith(CATCH_ALL_NOTE):
            detail = f"{CATCH_ALL_NOTE}; {detail}"
        if target and how != "review":
            renamed = unshadowed(target, ev.shadowed)
            if renamed != target:
                detail = f"{detail}; renamed from {target} (SDK/CRT header name)"
                target = renamed
        rows.append({"key": item.key, "kind": item.kind, "names": " ".join(item.names[:6]),
                     "line_start": item.first, "line_end": item.last, "lines": item.last - item.lead + 1,
                     "target": target or RESIDUE, "evidence": how, "detail": detail})
    return rows


STRONG = {"android", "android-stem", "symbol", "definition", "users", "decomp", "externals", "library", "review"}
# Android's own catch-all headers: acceptable step-1 targets, to be given module owners later.
CATCH_ALL = {"globals.h", "legoapi/legoapi_types.h"}
CATCH_ALL_NOTE = "catch-all (needs a module owner)"
TRANSITIONAL = "transitional/unused.h"
TRANSFERRED_FROM = 25487  # first line of the sections tools/typeimport.py and tools/globals.py wrote


def _follow_types(seg, ev, decisions, all_names):
    """STATIC_ASSERTs and forward declarations still open follow the type they name, once it is placed."""
    owner_of_name = _owners(seg, decisions)
    for item in seg.items:
        target, how, _ = decisions[item.index]
        if how != "unknown":
            continue
        if item.kind == "assert":
            match = ASSERT_TYPE.search(seg.own_text(Event("item", item.first, item.last, item.lead)))
            if match and match[1] in owner_of_name:
                decisions[item.index] = (owner_of_name[match[1]], "follow-type", match[1])
        elif item.kind in ("forward", "typedef"):
            for name in item.names:
                if name in owner_of_name:
                    decisions[item.index] = (owner_of_name[name], "follow-definition", name)
                    break


def _owners(seg, decisions):
    """name -> target of its defining item (a forward declaration's target when nothing else declares it)."""
    owners = {}
    for forward in (False, True):
        for item in seg.items:
            target = decisions[item.index][0]
            if target and item.kind != "assert" and (item.kind == "forward") == forward:
                for name in item.names:
                    owners.setdefault(name, target)
    return owners


def _rule(seg, ev, decisions):
    """Shared declarations: the compiland that defines it, else the one whose units use it most (ties: lowest
    address); a type only saga.h uses goes where most of its saga.h users go."""
    by_name = collections.defaultdict(list)
    closure = _macro_closure(seg)  # a use of a typedef or macro uses what it stands for
    for item in seg.items:
        refs = set(item.refs)
        for ref in item.refs:
            refs |= closure.get(ref, set())
        for ref in refs:
            by_name[ref].append(item)
    for item in seg.items:
        target, how, _ = decisions[item.index]
        if how != "unknown" or item.kind in ("assert", "undef"):
            continue
        defined = set()
        for name in item.names:
            defined |= {ev.file_compiland[p][:2] for p in ev.definers.get(name, ())
                        if p != "data.cpp" and p in ev.file_compiland}
        if defined and item.kind in ("variable", "function", "definition"):
            owner = min(defined, key=lambda c: ev.compiland_start.get(c, 1 << 32))
            header, path_how = ev.header_for_compiland(*owner)
            decisions[item.index] = (header, "rule", f"defined in {_label(owner)} {path_how}")
            continue
        owner, count = ev.heaviest_user(item.names)
        if owner:
            header, path_how = ev.header_for_compiland(*owner)
            decisions[item.index] = (header, "rule", f"used most in {_label(owner)} ({count}) {path_how}")
    for _ in range(4):
        for item in seg.items:
            target, how, _ = decisions[item.index]
            if how != "unknown" or item.kind in ("assert", "undef"):
                continue
            alias = next((f for f in seg.items if f.kind == "forward" and len(f.names) > 1 and
                          item.kind in ("struct", "union", "class") and f.names[0] in item.names[:1] and
                          decisions[f.index][1] not in ("unknown", "unused")), None)
            if alias is not None:
                # Units use the struct through its typedef: the struct goes where the typedef went.
                decisions[item.index] = (decisions[alias.index][0], "rule",
                                         f"with its typedef {alias.names[1]} ({decisions[alias.index][1]})")
                continue
            targets = collections.Counter()
            first = {}
            for name in item.names:
                for user in by_name.get(name, ()):
                    user_target = decisions[user.index][0]
                    if user is not item and user.kind != "assert" and user_target:
                        targets[user_target] += 1
                        first.setdefault(user_target, user.index)
            if targets:
                best = max(targets.values())
                header = min((h for h, n in targets.items() if n == best), key=lambda h: first[h])
                decisions[item.index] = (header, "rule", f"used in saga.h only, most by {header} ({best})")


def _data_nearest(seg, ev, decisions):
    """A global still open that only data.cpp mentions: the header of its nearest placed neighbour in retail
    data (weaker than data-order, which needs both neighbours to agree)."""
    import bisect  # noqa: PLC0415
    placed = sorted((ev.data_address[n], decisions[i.index][0]) for i in seg.items
                    if i.kind == "variable" and decisions[i.index][0] and decisions[i.index][1] in STRONG
                    for n in i.names if n in ev.data_address)
    addresses = [a for a, _ in placed]
    for item in seg.items:
        if decisions[item.index][1] != "unknown" or item.kind != "variable":
            continue
        address = next((ev.data_address[n] for n in item.names if n in ev.data_address), None)
        if address is None or not placed:
            continue
        k = bisect.bisect_left(addresses, address)
        near = min((p for p in (placed[k - 1] if k > 0 else None, placed[k] if k < len(placed) else None) if p),
                   key=lambda p: abs(p[0] - address))
        decisions[item.index] = (near[1], "data-nearest",
                                 f"only data.cpp mentions it; retail 0x{address:x}, nearest 0x{near[0]:x}")


def _label(owner):
    return owner[0] + (f"[{owner[1]}]" if owner[1] else "")


def _unused(seg, ev, decisions):
    """Declarations no unit uses: open ones, and those of the Android-transferred sections, go to the
    transitional header (deleted in step 3) unless a kept declaration needs them."""
    candidates = set()
    # The pairs of config/sibling-names.csv: deleting one of them changed code generation (docs/header-plan.md).
    renamed = {name for pair in ev.same_entity for name in pair}
    for item in seg.items:
        target, how, _ = decisions[item.index]
        if how in ("decomp", "library", "externals", "review", "same-entity") or \
                item.kind in ("include", "undef", "assert"):
            continue
        if not item.names or any(n in ev.users or n in ev.lib_users or n in renamed for n in item.names):
            continue
        if how == "unknown" or item.first >= TRANSFERRED_FROM:
            candidates.add(item.index)
    items = {item.index: item for item in seg.items}
    declarers = collections.defaultdict(set)
    for item in seg.items:
        for name in item.names:
            declarers[name].add(item.index)
    bodies = {"inline", "definition", "macro", "class", "block"}
    closure = _macro_closure(seg)  # a use of a typedef or macro uses what it stands for
    changed = True
    while changed:
        changed = False
        for item in seg.items:
            if item.index in candidates or item.kind == "assert":
                continue
            refs = set(item.refs)
            for ref in item.refs:
                refs |= closure.get(ref, set())
            for ref in refs:
                # A variable or function is needed only by code (bodies, initialisers, macros); a type by any
                # declaration that names it.
                needed = {i for i in declarers.get(ref, set()) & candidates
                          if item.kind in bodies or items[i].kind not in ("variable", "function")}
                if needed:
                    candidates -= needed
                    changed = True
    placed = sorted((item.first, decisions[item.index][0]) for item in seg.items
                    if item.index not in candidates and decisions[item.index][0] and item.kind != "assert"
                    and decisions[item.index][0] not in CATCH_ALL)
    lines = [line for line, _ in placed]
    import bisect  # noqa: PLC0415
    for index in sorted(candidates):
        item = items[index]
        target, how, _ = decisions[index]
        if not target:
            # No evidence at all: propose the module of the nearest placed declaration around it in saga.h.
            k = bisect.bisect_left(lines, item.first)
            near = [placed[j] for j in (k - 1, k) if 0 <= j < len(placed)]
            target = min(near, key=lambda p: abs(p[0] - item.first))[1]
            how = "saga.h neighbour"
        if item.first >= TRANSFERRED_FROM:
            decisions[index] = (TRANSITIONAL, "unused",
                                f"no unit uses it; Android-transferred section; would go to {target} ({how})")
        else:
            decisions[index] = (target, "unused", f"no unit uses it; PC declaration; owner by {how}")
    unused_names = {n for i in candidates for n in items[i].names}
    for item in seg.items:
        if item.kind == "assert" and decisions[item.index][1] in ("follow-type", "unknown"):
            match = ASSERT_TYPE.search(seg.own_text(Event("item", item.first, item.last, item.lead)))
            if match and match[1] in unused_names:
                owner = next(decisions[i][0] for i in candidates if match[1] in items[i].names)
                decisions[item.index] = (owner, "follow-type", match[1])


def _propagate(seg, ev, decisions):
    """Second-pass rules for items the direct evidence left open.

    - type-users: a type that units of at most one target use, and whose users inside saga.h all go to one
      header, goes there (repeated while it places more)
    - data-order: a global whose nearest placed neighbours in retail data (by address, both sides) go to the
      same header goes there; the linker lays out each object's data together, in link order
    """
    by_name = collections.defaultdict(list)
    for item in seg.items:
        for ref in item.refs:
            by_name[ref].append(item)
    types = ("struct", "union", "enum", "typedef", "forward", "class", "macro")
    for _ in range(6):
        changed = 0
        for item in seg.items:
            target, how, _ = decisions[item.index]
            if how != "unknown" or item.kind not in types:
                continue
            targets = set()
            for name in item.names:
                for user in by_name.get(name, ()):
                    user_target, user_how, _ = decisions[user.index]
                    if user is not item and user.kind != "assert" and user_target:
                        targets.add(user_target)
            unit_targets = set()
            for name in item.names:
                unit_targets |= {ev.header_for_compiland(*owner)[0] for owner in ev.compilands_using(name)}
            if len(targets) == 1 and unit_targets <= targets:
                decisions[item.index] = (targets.pop(), "type-users", "used by its saga.h users only there")
                changed += 1
        if not changed:
            break
    placed = []
    for item in seg.items:
        target, how, _ = decisions[item.index]
        if item.kind == "variable" and target and how in STRONG:
            for name in item.names:
                if name in ev.data_address:
                    placed.append((ev.data_address[name], target))
    placed.sort()
    import bisect  # noqa: PLC0415
    addresses = [a for a, _ in placed]
    for item in seg.items:
        target, how, _ = decisions[item.index]
        if how != "unknown" or item.kind != "variable":
            continue
        address = next((ev.data_address[n] for n in item.names if n in ev.data_address), None)
        if address is None:
            continue
        k = bisect.bisect_left(addresses, address)
        before = placed[k - 1] if k > 0 else None
        after = placed[k] if k < len(placed) else None
        if before and after and before[1] == after[1]:
            decisions[item.index] = (before[1], "data-order",
                                     f"retail 0x{address:x} between 0x{before[0]:x} and 0x{after[0]:x}")
    # Last: a name that Android declares only in a catch-all header (globals.h, decomp.h, batman.h, stubs).
    for item in seg.items:
        if decisions[item.index][1] != "unknown":
            continue
        for name in item.names:
            choice = ev.android_choice(name)
            if choice:
                decisions[item.index] = (choice[0], "android-weak", name)
                break


RANK = {"review": 0, "android": 1, "symbol": 2, "definition": 2, "externals": 2, "decomp": 2, "android-stem": 3,
        "users": 4, "type-users": 5, "data-order": 6, "android-weak": 7}


def _same_entity(seg, ev, decisions):
    """Declarations of one global under its PC name and its original name (config/sibling-names.csv rows of
    kind global, verified as the same variable) go to the header of the better-evidenced one."""
    items_of = collections.defaultdict(list)
    for item in seg.items:
        for name in item.names:
            items_of[name].append(item)
    for ours, original in ev.same_entity:
        group = items_of.get(ours, []) + items_of.get(original, [])
        placed = [(RANK.get(decisions[i.index][1], 9), decisions[i.index][0], i) for i in group
                  if decisions[i.index][0] and decisions[i.index][1] in RANK]
        source = None
        if placed:
            _, target, source = min(placed, key=lambda p: p[0])
            source = source.key
        elif not items_of.get(ours):
            # The PC name is defined in a unit and declared by no header: place it like a declscan name.
            _, target, how, _ = place_names([ours], ev)[0]
            source = f"{ours} ({how})"
            if not target or how not in RANK:
                continue
        else:
            continue
        for item in group:
            if decisions[item.index][0] != target and decisions[item.index][1] != "review":
                decisions[item.index] = (target, "same-entity", f"{ours} = {original}; follows {source}")


def place_names(names, ev):
    """(name, target, evidence, detail) for names declared outside saga.h (declscan class C/D)."""
    out = []
    for name in names:
        item = Item(0, 0, 0, 0, "function" if name in ev.symbol_compiland else "variable", [name], set(), (), 0)
        target, how, detail = decide(item, ev, None)
        if how == "unknown":
            choice = ev.android_choice(name)
            if choice:
                target, how, detail = choice[0], "android-weak", name
        out.append((name, target or RESIDUE, how, detail))
    return out


def write_map(rows, path):
    with open(path, "w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, MAP_FIELDS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def read_map(path):
    with open(path, encoding="utf-8") as stream:
        return list(csv.DictReader(stream))


# --------------------------------------------------------------------------------------------------------------
# Plan and rendering


def guard_name(path):
    return "SAGA_" + re.sub(r"[^A-Za-z0-9]", "_", path).upper()


@dataclass
class Run:
    index: int
    target: str
    items: list
    part: int = 0  # 1-based part of its header when the header has several


@dataclass
class Plan:
    seg: Segmentation
    runs: list
    targets: dict  # header -> [Run]
    unplaced: list  # items that stay in saga.h
    stale: list  # map rows that no longer match an item
    missing: list  # items without a map row
    includes: dict = field(default_factory=dict)  # header -> (plain includes, shim-only includes, notes)
    exact: bool = True  # saga.h order (token-neutral) rather than grouped
    split: dict = field(default_factory=dict)  # grouped: header -> "forced" or "cycle" for the headers in parts
    forced: dict = field(default_factory=dict)  # grouped: header -> an item that depends on it and that it needs


def make_plan(seg, rows, exact=False, extra=None):
    """The runs of the split. `exact`: saga.h order (token-neutral); otherwise grouped: the items reordered
    as far as their dependencies allow (see dependency_graph), so most headers are one run."""
    by_key = {row["key"]: row for row in rows}
    stale = [row for row in rows if row["key"] not in {i.key for i in seg.items}]
    missing = [item for item in seg.items if item.key not in by_key]
    target_of = {item.index: (by_key[item.key]["target"].strip() if item.key in by_key else RESIDUE)
                 for item in seg.items}
    why = {}
    order = seg.items if exact else schedule(seg, target_of, extra, why)
    runs = []
    for item in order:
        target = target_of[item.index]
        if runs and runs[-1].target == target:
            runs[-1].items.append(item)
        else:
            runs.append(Run(len(runs), target, [item]))
    targets = collections.defaultdict(list)
    for run in runs:
        if run.target:
            targets[run.target].append(run)
    for header, header_runs in targets.items():
        if len(header_runs) > 1:
            for n, run in enumerate(header_runs, 1):
                run.part = n
    unplaced = [i for run in runs if not run.target for i in run.items]
    plan = Plan(seg, runs, dict(targets), unplaced, stale, missing, exact=exact, split=why.get("split", {}),
                forced=why.get("forced", {}))
    plan.includes = dependencies(plan)
    return plan


# --------------------------------------------------------------------------------------------------------------
# Which reorderings are inert
#
# Two top-level items commute (can swap places without changing the meaning of either) unless
# - one declares a name the other declares too (redeclarations keep their order: linkage, composite types,
#   overloads, the C/C++ variants of one declaration),
# - one declares a name the other mentions (a use stays after the declaration it binds to; in C a
#   `struct tag *` in a prototype must not move before or after the tag's declaration), counting the
#   identifiers a mentioned macro expands to,
# - one is a macro definition or #undef and the other contains its name (expansion happens at the item),
# - one tests a macro in its #if context that the other defines,
# - one is an #include of a system header (a barrier), or of a project header, which counts as declaring and
#   mentioning what that header does.
# A macro's own body is not bound where it is defined, so it does not order the macro against the names it
# uses. Any order that keeps every non-commuting pair in saga.h order is equivalent: each unit then sees the
# same declarations, each with the same meaning; only their order and the __COUNTER__ numbers that name the
# STATIC_ASSERT typedefs change.


def _macro_closure(seg):
    """name -> the names a mention of it also mentions: a macro's body (expanded where it is used) and the
    types a typedef stands for (using `N` of `typedef struct T N;` may need T complete), transitively."""
    bodies = {}
    for item in seg.items:
        if item.kind == "macro":
            for name in item.names:
                bodies.setdefault(name, set()).update(item.refs)
        elif item.kind == "typedef":
            for name in item.names:
                bodies.setdefault(name, set()).update(item.refs - item.locals)
        elif item.kind == "forward" and len(item.names) > 1:
            for name in item.names[1:]:
                bodies.setdefault(name, set()).add(item.names[0])
    closure = {}
    for name in bodies:
        seen = set()
        stack = [name]
        while stack:
            current = stack.pop()
            for ref in bodies.get(current, ()):
                if ref not in seen:
                    seen.add(ref)
                    if ref in bodies:
                        stack.append(ref)
        closure[name] = seen
    return closure


def _project_header_names(path, seen=None):
    """(names, refs) of a project header and the project headers it includes."""
    seen = seen if seen is not None else set()
    names, refs = set(), set()
    if path in seen or not path.is_file():
        return names, refs
    seen.add(path)
    seg = segment(read_text(path))
    for item in seg.items:
        if item.kind == "include":
            for included in item.names:
                more = _project_header_names(path.parent / included, seen)
                names |= more[0]
                refs |= more[1]
        names.update(item.names)
        refs.update(item.refs)
    return names, refs - names


SYSTEM_INCLUDE_DIRS = ("build/tools/msvc8/include", "build/tools/msvc8/PlatformSDK/Include")
INCLUDE_LINE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)


def _depth_zero_ids(text):
    """Identifiers of preprocessed C++ outside any braces, where `extern "C" {` and enum braces count as
    transparent: a superset of the names declared at global scope."""
    out = set()
    stack = []  # True for a transparent brace
    previous = []
    for match in re.finditer(r'"(?:\\.|[^"\\\n])*"|[A-Za-z_]\w*|[{};]', text):
        token = match.group()
        if token == "{":
            transparent = (len(previous) >= 2 and previous[-1].startswith('"') and previous[-2] == "extern") or \
                any(t == "enum" for t in previous[-4:])
            stack.append(transparent)
        elif token == "}":
            if stack:
                stack.pop()
        elif all(stack) and (token[0].isalpha() or token[0] == "_"):
            out.add(token)
        if token in "{};":
            previous = []
        else:
            previous.append(token)
    return out


def system_header_names(header, root=ROOT):
    """(global, every, macros) identifiers of a toolchain header, or None when they cannot be read.

    `every`: the identifiers of the header and the headers it includes, read lexically over all branches, a
    superset of what it defines and tests as macros. `global`: its macros and the identifiers outside braces
    of its preprocessed text (one clang -E), plus `std`, a superset of what it declares outside namespace std,
    where a saga.h declaration could meet it."""
    dirs = [root / d for d in SYSTEM_INCLUDE_DIRS if (root / d).is_dir()]
    clang = os.environ.get("CLANG") or shutil.which("clang-18") or shutil.which("clang")
    if not dirs or not clang:
        return None
    every = set()
    seen = set()
    stack = [(header, None)]
    while stack:
        name, here = stack.pop()
        candidates = ([here / name] if here else []) + [d / name for d in dirs]
        path = next((p for p in candidates if p.is_file()), None)
        if path is None or path in seen:
            continue
        seen.add(path)
        text = strip_comments(read_text(path))
        every |= set(re.findall(r"[A-Za-z_]\w*", text))
        stack += [(included, path.parent) for included in INCLUDE_LINE.findall(text)]
    if not seen:
        return None
    from tools import declscan  # noqa: PLC0415

    def expand(source):
        return subprocess.run([clang, "-E", "-dD", *declscan.base_flags(), "-x", "c++", "-"], cwd=root,
                              input=source, capture_output=True, text=True, encoding="latin-1", timeout=600)

    result = expand(f"#include <{header}>\n")
    if result.returncode or not result.stdout:
        return None
    defined = re.compile(r"^#\s*define\s+([A-Za-z_]\w*)", re.M)
    macros = set(defined.findall(result.stdout)) - set(defined.findall(expand("").stdout))  # not the predefined
    code = "\n".join(line for line in result.stdout.splitlines() if not line.lstrip().startswith("#"))
    return (macros | _depth_zero_ids(code) | {"std"}, every | macros, macros)


TAG_KINDS = {"struct", "union", "class"}
NAMING_KINDS = {"function"}  # prototypes: a later definition of a tag they name changes nothing for them


def _context_prefix(outer, inner):
    return tuple(inner[:len(outer)]) == tuple(outer)


def dependency_graph(seg, extra=None, root=ROOT):
    """predecessors[i]: the items that must stay before item i (indices into seg.items).

    `extra`: header -> system_header_names(header); a system #include not in it is looked up in the toolchain.
    The includes of the prelude (before the first declaration) and those that cannot be read are barriers.
    Another system include declares and mentions its `global` identifiers, and keeps its place against every
    macro definition or #undef of one of its identifiers."""
    extra = dict(extra or {})
    closure = _macro_closure(seg)
    conditions = {}
    for frame in seg.frames:
        ids = set()
        for condition in frame.conditions:
            ids.update(re.findall(r"[A-Za-z_]\w*", condition))
        conditions[frame.id] = ids - {"defined"}
    declares = []
    mentions = []
    barrier = []
    prelude = True  # system includes before the first declaration stay barriers
    system = []  # (index, every identifier) of the system includes that are not barriers
    for item in seg.items:
        names = set(item.names)
        refs = set() if item.kind == "macro" else set(item.refs)
        is_barrier = False
        if item.kind == "include":
            header = item.names[0] if item.names else ""
            local = root / "src" / header
            if local.is_file():
                more = _project_header_names(local)
                names, refs = more[0], more[1]
            else:
                if header not in extra and not prelude:
                    extra[header] = system_header_names(header, root)
                if extra.get(header) is None or prelude:
                    is_barrier = True
                else:
                    names, refs = set(extra[header][0]), set()  # declares all; mentions too, below
                    system.append((item.index, extra[header][1]))
        elif item.kind not in ("macro", "undef", "pragma", "directive"):
            prelude = False
        if item.kind in ("undef", "pragma", "directive"):
            refs |= names
        for ref in list(refs):
            refs |= closure.get(ref, set())
        for frame_id, _ in item.context:
            refs |= conditions[frame_id]
        names -= KEYWORDS  # a keyword a name was taken from by mistake binds nothing
        declares.append(names)
        mentions.append(refs - names - KEYWORDS)
        barrier.append(is_barrier)
        if item.kind == "include" and not is_barrier and not (root / "src" / item.names[0]).is_file():
            mentions[-1] = set(names)
    predecessors = [set() for _ in seg.items]
    by_name = collections.defaultdict(list)
    for i, item in enumerate(seg.items):
        for name in declares[i]:
            by_name[name].append((i, "D"))
        for name in mentions[i]:
            by_name[name].append((i, "M" if name in item.locals else "R"))
    macro_like = {i for i, item in enumerate(seg.items) if item.kind in ("macro", "undef")}
    system_macros = {i: extra[seg.items[i].names[0]][2] for i, _ in system}
    for name, touches in by_name.items():
        if not any(role == "D" for _, role in touches):
            continue
        # A parameter name binds nothing; it only meets a macro of that name.
        local_counts = any(role == "D" and (i in macro_like or name in system_macros.get(i, ())) for i, role in touches)
        touches.sort()
        last_declarer = None
        tags = []  # earlier struct/union/class/forward/typedef declarers of the name
        since = []  # mentions after the last declarer
        for i, role in touches:
            if role == "M" and not local_counts:
                continue
            kind = seg.items[i].kind
            if role == "D":
                if last_declarer is not None and last_declarer != i:
                    predecessors[i].add(last_declarer)
                visible = any(_context_prefix(seg.items[t].context, seg.items[i].context) for t in tags)
                if kind == "forward" and visible:
                    # Redeclaring a declared tag changes nothing for the mentions around it.
                    last_declarer = i
                    continue
                if kind in TAG_KINDS and visible:
                    # Completing a declared tag changes nothing for declarations that only name it.
                    predecessors[i].update(j for j in since if j != i and seg.items[j].kind not in NAMING_KINDS)
                else:
                    predecessors[i].update(j for j in since if j != i)
                if kind in TAG_KINDS or kind in ("forward", "typedef"):
                    tags.append(i)
                last_declarer = i
                since = []
            else:
                if last_declarer is not None and last_declarer != i:
                    predecessors[i].add(last_declarer)
                since.append(i)
    for i, every in system:
        for j, item in enumerate(seg.items):
            if item.kind in ("macro", "undef") and j != i and every.intersection(item.names):
                predecessors[max(i, j)].add(min(i, j))
    last_barrier = None
    since_barrier = []
    for i in range(len(seg.items)):
        if barrier[i]:
            predecessors[i].update(since_barrier)
            if last_barrier is not None:
                predecessors[i].add(last_barrier)
            last_barrier = i
            since_barrier = []
        else:
            if last_barrier is not None:
                predecessors[i].add(last_barrier)
            since_barrier.append(i)
    return predecessors


def forced_splits(predecessors, header_of):
    """The headers that cannot be one run in any order: some item outside the header both depends on one of
    its items and is needed by another (header -> such an outside item, the first in saga.h order)."""
    successors = [[] for _ in predecessors]
    for i, preds in enumerate(predecessors):
        for j in preds:
            successors[j].append(i)
    members = collections.defaultdict(list)
    for i, header in enumerate(header_of):
        members[header].append(i)

    def outside_reach(header, starts, step):
        seen = set()
        stack = [k for s in starts for k in step[s] if header_of[k] != header]
        seen.update(stack)
        while stack:
            for k in step[stack.pop()]:
                if k not in seen:
                    seen.add(k)
                    stack.append(k)
        return seen

    forced = {}
    for header, items in members.items():
        later = outside_reach(header, items, successors)
        if later:
            between = later & outside_reach(header, items, predecessors)
            if between:
                forced[header] = min(between)
    return forced


def _cyclic_components(nodes, edges):
    """The strongly connected components with more than one node (iterative Tarjan)."""
    index, low, on_stack, stack, out = {}, {}, set(), [], []
    counter = 0
    for root in nodes:
        if root in index:
            continue
        work = [(root, iter(edges.get(root, ())))]
        index[root] = low[root] = counter
        counter += 1
        stack.append(root)
        on_stack.add(root)
        while work:
            node, children = work[-1]
            advanced = False
            for child in children:
                if child not in index:
                    index[child] = low[child] = counter
                    counter += 1
                    stack.append(child)
                    on_stack.add(child)
                    work.append((child, iter(edges.get(child, ()))))
                    advanced = True
                    break
                if child in on_stack:
                    low[node] = min(low[node], index[child])
            if advanced:
                continue
            work.pop()
            if work:
                low[work[-1][0]] = min(low[work[-1][0]], low[node])
            if low[node] == index[node]:
                component = []
                while True:
                    member = stack.pop()
                    on_stack.discard(member)
                    component.append(member)
                    if member == node:
                        break
                if len(component) > 1:
                    out.append(component)
    return out


def choose_splits(predecessors, header_of):
    """(split headers, why): the forced ones, then, while the headers that stay whole still form a cycle,
    the header of each cycle with the most dependency edges inside it."""
    forced = forced_splits(predecessors, header_of)
    split = dict.fromkeys(forced, "forced")
    while True:
        node = [("item", i) if header_of[i] in split else header_of[i] for i in range(len(header_of))]
        edges = collections.defaultdict(set)
        for i, preds in enumerate(predecessors):
            for j in preds:
                if node[j] != node[i]:
                    edges[node[j]].add(node[i])
        components = _cyclic_components(list(dict.fromkeys(node)), edges)
        if not components:
            return split, forced
        for component in components:
            members = set(component)
            weight = collections.Counter()
            for a in component:
                if isinstance(a, str):
                    inside = sum(1 for b in edges.get(a, ()) if b in members)
                    weight[a] = inside * sum(1 for b in component if a in edges.get(b, ()))
            split[max(weight, key=lambda h: (weight[h], h))] = "cycle"


def schedule(seg, target_of, extra=None, report=None):
    """Items in an order that keeps every dependency (dependency_graph) with as few headers as can be split
    into parts: the headers that stay whole are emitted at once, in saga.h order; the items of a split header
    are emitted one by one, continuing its current run while it has ready items."""
    import heapq  # noqa: PLC0415
    items = seg.items
    header_of = [target_of[item.index] for item in items]
    predecessors = dependency_graph(seg, extra)
    split, forced = choose_splits(predecessors, header_of)
    if report is not None:
        report.update(split=split, forced=forced)
    node = [("item", i) if header_of[i] in split else header_of[i] for i in range(len(items))]
    members = collections.defaultdict(list)
    for i, n in enumerate(node):
        members[n].append(i)
    edges = collections.defaultdict(set)
    pending = collections.Counter()
    for i, preds in enumerate(predecessors):
        for j in preds:
            if node[j] != node[i] and node[i] not in edges[node[j]]:
                edges[node[j]].add(node[i])
                pending[node[i]] += 1
    whole_ready = []  # heap of (first item, header) of the whole headers that are ready
    item_ready = collections.defaultdict(list)  # split header -> heap of its ready items
    left = collections.Counter(header_of[i] for i in range(len(items)) if header_of[i] in split)

    def release(n):
        if isinstance(n, tuple):
            heapq.heappush(item_ready[header_of[n[1]]], n[1])
        else:
            heapq.heappush(whole_ready, (members[n][0], n))

    for n in members:
        if not pending[n]:
            release(n)
    current = None
    order = []
    while len(order) < len(items):
        if current is None or not item_ready[current]:
            # A split header whose remaining items are all ready closes now; then the whole headers; only when
            # none is ready the split header with the earliest ready item starts or continues a part.
            closing = [(item_ready[h][0], h) for h in left if left[h] and len(item_ready[h]) == left[h]]
            candidates = closing + whole_ready[:1]
            if candidates:
                choice = min(candidates)[1]
            else:
                choice = min((heap[0], h) for h, heap in item_ready.items() if heap)[1]
            if choice not in split:
                heapq.heappop(whole_ready)
                done = [choice]
                current = None
            else:
                current = choice
        if current is not None:
            i = heapq.heappop(item_ready[current])
            left[current] -= 1
            done = [("item", i)]
        for n in done:
            order += [items[i] for i in members[n]]
            for m in edges.get(n, ()):
                pending[m] -= 1
                if not pending[m]:
                    release(m)
    if len(order) != len(items):
        raise ValueError("dependency cycle among the headers that stay whole")
    return order


TYPE_KINDS = {"struct", "union", "enum", "class", "typedef", "forward", "macro", "pack", "inline"}


def dependencies(plan):
    """header -> (plain includes, shim-only includes, notes): what each header needs from other headers.

    A header used by an item is the target of the closest earlier item that declares the name (types,
    typedefs, macros, enumerators). It is included plainly when all of it comes earlier in the shim (a no-op
    there); otherwise only outside the shim (`#ifndef SAGA_SHIM`), so the shim order stays as it is."""
    seg = plan.seg
    target_of = {}
    position = {}
    for run in plan.runs:
        for item in run.items:
            target_of[item.index] = (run.target, run.index)
            position[item.index] = len(position)
    declared = collections.defaultdict(list)  # name -> [position] in shim order
    for item in seg.items:
        if item.kind in TYPE_KINDS:
            for name in item.names:
                declared[name].append(position[item.index])
    at = {p: i for i, p in position.items()}
    first_run = {}
    last_run = {}
    for run in plan.runs:
        first_run.setdefault(run.target, run.index)
        last_run[run.target] = run.index
    result = {}
    import bisect  # noqa: PLC0415
    for header, header_runs in plan.targets.items():
        needs = collections.defaultdict(set)  # other header -> names
        residue = set()
        for run in header_runs:
            for item in run.items:
                for ref in item.refs:
                    providers = declared.get(ref)
                    if not providers:
                        continue
                    k = bisect.bisect_left(providers, position[item.index]) - 1
                    if k < 0:
                        continue
                    if any(target_of[at[p]][0] == header for p in providers[:k + 1]):
                        continue
                    provider = target_of[at[providers[k]]][0]
                    if provider == header:
                        continue
                    if not provider:
                        residue.add(ref)
                    else:
                        needs[provider].add(ref)
        plain = []
        shim_only = []
        start = header_runs[0].index
        for other in sorted(needs):
            if len(header_runs) == 1 and last_run[other] < start:
                plain.append(other)
            else:
                shim_only.append(other)
        notes = []
        if shim_only:
            notes.append(f"{SHIM_NOTE}: " + ", ".join(shim_only) + " (not all of it earlier in the shim)")
        if residue:
            notes.append("uses declarations still in saga.h: " + ", ".join(sorted(residue)[:12]) +
                         (" ..." if len(residue) > 12 else ""))
        result[header] = (plain, shim_only, notes)
    return result


NOLINT_REGION = re.compile(r"^\s*//\s*NOLINT(BEGIN|END)\(([^)]*)\)")


def nolint_regions(seg):
    """[(first line, last line, BEGIN comment)] of the `// NOLINTBEGIN(...)` / `// NOLINTEND(...)` regions."""
    regions = []
    open_ = {}
    for number, line in enumerate(seg.lines, 1):
        match = NOLINT_REGION.match(line)
        if not match:
            continue
        if match.group(1) == "BEGIN":
            open_[match.group(2)] = (number, line.strip())
        elif match.group(2) in open_:
            first, text = open_.pop(match.group(2))
            regions.append((first, number, text))
    return regions


def _region_of(seg, line):
    if not hasattr(seg, "_nolint_regions"):
        seg._nolint_regions = nolint_regions(seg)
    for first, last, text in seg._nolint_regions:
        if first < line < last:
            return text
    return None


def _without_nolint_regions(lines):
    return [line for line in lines if not NOLINT_REGION.match(line)]


def render_items(seg, items):
    """The items' text with their #if contexts and extern "C" blocks re-emitted around them. An item inside a
    `// NOLINTBEGIN(...)` region of saga.h gets the region's comments around its run of items (asserts only
    inside a run of other items)."""
    out = []
    open_frames = []  # [(frame id, branch)]
    linked = False
    region = None

    def close_region():
        nonlocal region
        if region:
            out.append("// NOLINTEND(" + NOLINT_REGION.match(region).group(2) + ")")
            region = None

    def close_link():
        nonlocal linked
        if linked:
            out.append('#ifdef __cplusplus\n}\n#endif')
            linked = False

    for item in items:
        want_region = _region_of(seg, item.first)
        if item.kind == "assert" and want_region != region:
            want_region = None  # a run of asserts alone needs no NOLINT region
        if want_region != region:
            close_region()
        for first, last in item.carried:
            out.append("\n".join(_without_nolint_regions(seg.lines[first - 1:last])))
        want = list(item.context)
        want_link = item.linkage > 0
        common = 0
        while common < len(open_frames) and common < len(want) and open_frames[common] == want[common]:
            common += 1
        if common < len(open_frames) or (want_link != linked):
            close_link()
        # Next branch of the innermost open frame: reuse the original #elif/#else.
        if common == len(open_frames) - 1 and common < len(want) and open_frames[common][0] == want[common][0] \
                and want[common][1] == open_frames[common][1] + 1:
            frame = seg.frames[want[common][0]]
            out.append(frame.directives[want[common][1]].text)
            open_frames[common] = want[common]
            common += 1
        while len(open_frames) > common:
            out.append("#endif")
            open_frames.pop()
        for frame_id, branch in want[common:]:
            frame = seg.frames[frame_id]
            if branch == 0:
                out.append(frame.directives[0].text)
            else:
                parts = [f"!({c})" for c in frame.conditions[:branch]]
                if frame.conditions[branch]:
                    parts.append(f"({frame.conditions[branch]})")
                out.append("#if " + " && ".join(parts))
            open_frames.append((frame_id, branch))
        if want_link and not linked:
            out.append('#ifdef __cplusplus\nextern "C" {\n#endif')
            linked = True
        if want_region and not region:
            out.append(want_region)
            region = want_region
        lead = seg.lines[item.lead - 1:item.first - 1]
        out.append("\n".join(_without_nolint_regions(lead + seg.lines[item.first - 1:item.last])))
    close_region()
    close_link()
    while open_frames:
        out.append("#endif")
        open_frames.pop()
    return "\n".join(out)


HEADER_NOTE = ("/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at "
               "the place of its declarations. */")


PART_NOTE = ("/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes "
             "only; the\n * condition and the _PART selector go away with the shim. */")


def render_header(plan, header):
    """A header with one part is `#pragma once`; one with several parts is that only outside the shim, which
    includes it once per part (the `_PART` selector)."""
    runs = plan.targets[header]
    plain, shim_only, notes = plan.includes.get(header, ([], [], []))
    guard = guard_name(header)
    out = [HEADER_NOTE]
    for note in notes:
        out.append(f"/* {note[0].upper()}{note[1:]}. */")
    out.append("")
    if len(runs) == 1:
        out.append("#pragma once")
    else:
        out += [PART_NOTE, "#ifndef SAGA_SHIM", "#pragma once", "#endif",
                f"#ifndef {guard}_PART", f"#define {guard}_PART 1", "#endif"]
    if plain:
        out.append("")
        out += [f'#include "{h}"' for h in plain]
    if shim_only:
        out += ["", "#ifndef SAGA_SHIM"] + [f'#include "{h}"' for h in shim_only] + ["#endif"]
    if len(runs) == 1:
        out += ["", render_items(plan.seg, runs[0].items)]
        return "\n".join(out) + "\n"
    out.append(f"/* {len(runs)} parts: saga.h includes this header once per part, each where its declarations belong; "
               "included directly, it gives all parts at once. */")
    for run in runs:
        out += ["", f"#if {guard}_PART == {run.part} && !defined({guard}_STOP)",
                render_items(plan.seg, run.items),
                f"#undef {guard}_PART", f"#define {guard}_PART {run.part + 1}",
                "#ifdef SAGA_SHIM", f"#define {guard}_STOP", "#endif", "#endif"]
    out += ["", f"#undef {guard}_STOP"]
    return "\n".join(out) + "\n"


SHIM_COMMENT = """/* Include-all shim (docs/header-plan.md, step 1): every module header, in an order where each declaration
 * comes after what it depends on. A header with several parts is included once per part. Declarations without a
 * mapped header (config/header-map.csv) stay here, at their place. */"""
SHIM_COMMENT_EXACT = """/* Include-all shim (docs/header-plan.md, step 1): the module headers in the order that reproduces the
 * declaration order of the former catch-all header exactly. A header with several parts is included once per
 * part. Declarations without a mapped header (config/header-map.csv) stay here, at their place. */"""


def render_shim(plan):
    seg = plan.seg
    guard = seg.guard
    out = []
    if guard:
        out += ["#pragma once", ""]
    out += [SHIM_COMMENT_EXACT if plan.exact else SHIM_COMMENT, "#define SAGA_SHIM", ""]
    for run in plan.runs:
        if run.target:
            out.append(f'#include "{run.target}"')
        else:
            out.append(render_items(seg, run.items))
    return "\n".join(out) + "\n"


def apply_plan(plan, src, format_=True):
    """Write the shim and the headers under `src` (a directory laid out like src/), formatted with the house
    style (.clang-format: includes are never reordered) so that `check.py --format` passes as written."""
    src = Path(src)
    written = []
    for header in sorted(plan.targets):
        path = src / header
        if path.exists() and header != "saga.h":
            raise ValueError(f"{header} exists already; map its items to a new header")
    for header in sorted(plan.targets):
        path = src / header
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(render_header(plan, header), encoding="latin-1", newline="\n")
        written.append(header)
    (src / "saga.h").write_text(render_shim(plan), encoding="latin-1", newline="\n")
    if format_:
        from tools.check import clang_tool  # noqa: PLC0415
        files = [str(src / header) for header in written] + [str(src / "saga.h")]
        subprocess.run([clang_tool("clang-format"), "--style=file", "--fallback-style=none", "-i", *files],
                       cwd=ROOT, check=True, timeout=600)
    return written


# --------------------------------------------------------------------------------------------------------------
# Neutrality check

PP_TOKEN = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_]\w*|\.?\d(?:[\w.]|[eEpP][+-])*'
                      r'|\.\.\.|::|->|##|<<=|>>=|[-+*/%&|^!=<>]=|&&|\|\||<<|>>|\+\+|--|\S')
LINE_MARKER = re.compile(r"^#\s*(?:line\s+)?\d+(?:\s|$)")


def pp_tokens(text):
    """Tokens of preprocessed output: line markers dropped, each #pragma line one token."""
    tokens = []
    for line in text.split("\n"):
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith("#"):
            if LINE_MARKER.match(stripped):
                continue
            tokens.append(" ".join(stripped.split()))
            continue
        tokens.extend(PP_TOKEN.findall(line))
    return tokens


def with_linkage(tokens):
    """(token, linkage) pairs with the braces of `extern "C" { ... }` blocks dropped."""
    out = []
    stack = []
    linkage = 0
    i = 0
    n = len(tokens)
    while i < n:
        t = tokens[i]
        if t == "extern" and i + 2 < n and tokens[i + 1] == '"C"' and tokens[i + 2] == "{":
            stack.append(True)
            linkage += 1
            i += 3
            continue
        if t == "{":
            stack.append(False)
        elif t == "}" and stack:
            if stack.pop():
                linkage -= 1
                i += 1
                continue
        out.append((t, linkage > 0))
        i += 1
    return out


def preprocess(unit, cwd, flags):
    """clang -E output of `unit` (relative path) with `cwd` as the root; (text, error count)."""
    clang = os.environ.get("CLANG") or shutil.which("clang-18") or shutil.which("clang")
    if not clang:
        raise ValueError("clang (18) is required for --check-neutral; set CLANG")
    result = subprocess.run([clang, "-E", *flags, unit], cwd=cwd, capture_output=True, text=True,
                            encoding="latin-1", timeout=600)
    if not result.stdout:
        raise ValueError(f"clang -E {unit} produced nothing:\n{result.stderr[-2000:]}")
    errors = sum(1 for line in result.stderr.splitlines() if ": error:" in line or ": fatal error:" in line)
    return result.stdout, errors


def compare_units(units, before_root, after_root, debug=False, report=print):
    """Compare each unit's preprocessed token stream in two trees; returns the units that differ."""
    from tools import declscan  # noqa: PLC0415
    base = declscan.base_flags()
    differing = []
    for unit in units:
        flags = declscan.unit_flags(unit, base)
        if debug:
            flags = declscan.debug_flags(flags)
        before_text, before_errors = preprocess(unit, before_root, flags)
        after_text, after_errors = preprocess(unit, after_root, flags)
        before = with_linkage(pp_tokens(before_text))
        after = with_linkage(pp_tokens(after_text))
        if before == after and before_errors == after_errors:
            report(f"{unit}: neutral ({len(before)} tokens, {before_errors} clang errors on both sides)")
            continue
        differing.append(unit)
        k = next((k for k, (a, b) in enumerate(zip(before, after)) if a != b), min(len(before), len(after)))
        context = lambda seq: " ".join(t + ("" if c else "") for t, c in seq[max(0, k - 12):k + 12])  # noqa: E731
        report(f"{unit}: DIFFERS at token {k} of {len(before)}/{len(after)}, errors {before_errors}/{after_errors}\n"
               f"  before: {context(before)}\n  after:  {context(after)}")
    return differing


STATIC_ASSERT_NAME = re.compile(r"^saga_static_assert_\d+$")
PACK = re.compile(r"^#\s*pragma\s+pack\s*\((.*)\)$")


def top_level_declarations(pairs):
    """The top-level declarations of a preprocessed unit, from with_linkage() pairs: (tokens, extern "C",
    #pragma pack in effect). A declaration ends at a `;` outside brackets, or at the `}` that closes a function
    body or namespace. `saga_static_assert_N` names lose their __COUNTER__ number; #pragma lines outside a
    declaration are entries of their own, and `#pragma pack` lines also set the packing of what follows."""
    out = []
    current = []
    depth = 0
    body = False  # the open top-level brace is a function body or a namespace
    pack = []  # stack of explicit packings; [] is the command-line default
    linkage = False
    for token, linked in pairs:
        if token.startswith("#") and not current:
            match = PACK.match(token)
            if match:
                args = [a.strip() for a in match.group(1).split(",") if a.strip()]
                if args[:1] == ["push"]:
                    pack.append(args[-1] if len(args) > 1 else (pack[-1] if pack else ""))
                elif args[:1] == ["pop"]:
                    if pack:
                        pack.pop()
                else:
                    pack[-1:] = [args[0] if args else ""]
            out.append(((token,), linked, pack[-1] if pack else ""))
            continue
        if not current:
            linkage = linked
        current.append("saga_static_assert_#" if STATIC_ASSERT_NAME.match(token) else token)
        if token in ("(", "["):
            depth += 1
        elif token in (")", "]"):
            depth -= 1
        elif token == "{":
            if depth == 0:
                before = current[-2] if len(current) > 1 else ""
                body = before in (")", "const") or "namespace" in current
            depth += 1
        elif token == "}":
            depth -= 1
        if depth == 0 and (token == ";" or (token == "}" and body)):
            out.append((tuple(current), linkage, pack[-1] if pack else ""))
            current = []
            body = False
    if current:
        out.append((tuple(current), linkage, pack[-1] if pack else ""))
    return out


def syntax_errors(unit, cwd, flags):
    """The error messages of clang -fsyntax-only on `unit`, without their file and line."""
    clang = os.environ.get("CLANG") or shutil.which("clang-18") or shutil.which("clang")
    if not clang:
        raise ValueError("clang (18) is required for --check-counts; set CLANG")
    result = subprocess.run([clang, "-fsyntax-only", "-ferror-limit=0", *flags, unit], cwd=cwd,
                            capture_output=True, text=True, encoding="latin-1", timeout=600)
    return collections.Counter(line.split(" error: ", 1)[1] for line in result.stderr.splitlines()
                               if " error: " in line)


def compare_counts(units, before_root, after_root, debug=False, report=print):
    """Compare the declarations each unit sees in two trees as multisets (tokens, linkage, packing), and the
    clang -fsyntax-only errors; returns the units that differ. Order is not compared: a grouped split moves
    declarations, and only the object code (built per unit) shows that the moves are inert."""
    from tools import declscan  # noqa: PLC0415
    base = declscan.base_flags()
    differing = []
    for unit in units:
        # VC8 reads // comments in C too; under the lint flags' -std=c89 they would become tokens that travel
        # with the declarations they follow.
        flags = ["-std=gnu89" if f == "-std=c89" else f for f in declscan.unit_flags(unit, base)]
        if debug:
            flags = declscan.debug_flags(flags)
        sides = []
        for root in (before_root, after_root):
            text, _ = preprocess(unit, root, flags)
            tokens = with_linkage(pp_tokens(text))
            sides.append((top_level_declarations(tokens), syntax_errors(unit, root, flags)))
        (before, before_errors), (after, after_errors) = sides
        before_set, after_set = collections.Counter(before), collections.Counter(after)
        moved = sum(1 for a, b in zip(before, after) if a != b)
        if before_set == after_set and before_errors == after_errors:
            report(f"{unit}: same {len(before)} declarations ({moved} at another place), "
                   f"{sum(before_errors.values())} clang errors on both sides")
            continue
        differing.append(unit)
        report(f"{unit}: DIFFERS: {len(before)}/{len(after)} declarations, "
               f"{sum(before_errors.values())}/{sum(after_errors.values())} clang errors")
        for label, extra in (("only before", before_set - after_set), ("only after", after_set - before_set)):
            for (tokens, linked, pack), count in list(extra.items())[:5]:
                text = " ".join(tokens)
                report(f"  {label} x{count} (extern C {int(linked)}, pack {pack or '-'}): {text[:160]}")
        for label, extra in (("errors only before", before_errors - after_errors),
                             ("errors only after", after_errors - before_errors)):
            for message, count in list(extra.items())[:5]:
                report(f"  {label} x{count}: {message[:160]}")
    return differing


def scratch_tree(plan, root=ROOT, scratch=None):
    """A copy of src/ with the split applied (lib/ linked), for --check-neutral; returns its root."""
    scratch = Path(scratch or Path(root) / SCRATCH)
    if scratch.exists():
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True)
    shutil.copytree(Path(root) / "src", scratch / "src")
    if (Path(root) / "lib").exists():
        os.symlink(Path(root) / "lib", scratch / "lib")
    apply_plan(plan, scratch / "src")
    return scratch


# --------------------------------------------------------------------------------------------------------------
# Reports


def coverage(rows):
    placed = [r for r in rows if r["target"]]
    lines = sum(int(r["lines"]) for r in rows)
    placed_lines = sum(int(r["lines"]) for r in placed)
    return len(placed), len(rows), placed_lines, lines


def stats(seg, rows, top=40, report=print):
    placed, total, placed_lines, lines = coverage(rows)
    report(f"{total} items in {len(seg.lines)} lines; placed {placed} ({100 * placed / total:.1f}%), "
           f"{placed_lines} of {lines} lines ({100 * placed_lines / lines:.1f}%)")
    kinds = collections.Counter(r["kind"] for r in rows)
    placed_kinds = collections.Counter(r["kind"] for r in rows if r["target"])
    report("\nitems per kind (placed/total):")
    for kind, count in kinds.most_common():
        report(f"  {kind:<12} {placed_kinds[kind]:>6} / {count:<6}")
    report("\nevidence (items, lines):")
    evidence = collections.Counter(r["evidence"] for r in rows)
    evidence_lines = collections.Counter()
    for r in rows:
        evidence_lines[r["evidence"]] += int(r["lines"])
    for how, count in evidence.most_common():
        report(f"  {how:<18} {count:>6} {evidence_lines[how]:>7}")
    targets = collections.Counter()
    target_lines = collections.Counter()
    for r in rows:
        if r["target"]:
            targets[r["target"]] += 1
            target_lines[r["target"]] += int(r["lines"])
    report(f"\n{len(targets)} target headers; the largest (items, lines):")
    for header, count in targets.most_common(top):
        report(f"  {header:<48} {count:>5} {target_lines[header]:>6}")
    unknown = sorted((r for r in rows if not r["target"]), key=lambda r: -int(r["lines"]))
    report(f"\n{len(unknown)} items without a target, largest first:")
    for r in unknown[:top]:
        report(f"  {r['line_start']:>6} {r['lines']:>4} {r['kind']:<9} {r['names'][:40]:<40} {r['detail']}")


def interleaving(plan, top=30, report=print):
    multi = {h: runs for h, runs in plan.targets.items() if len(runs) > 1}
    parts = sum(len(r) for r in multi.values())
    mode = "saga.h order (--exact-order)" if plan.exact else "grouped order"
    report(f"{len(plan.runs)} runs in {mode}; {len(plan.targets)} headers, {len(multi)} of them in "
           f"several parts ({parts} parts); {sum(1 for r in plan.runs if not r.target)} runs stay in saga.h")
    if not plan.exact:
        forced = sum(1 for why in plan.split.values() if why == "forced")
        report(f"{forced} headers must have parts in any order (an item of another header depends on one of "
               f"their items and is needed by another); {len(plan.split) - forced} more break the cycles left "
               "among the headers that stay whole")
    report("\nheaders by number of parts (parts, items, first-last line, headers in between most often"
           + ("" if plan.exact else "; why") + "):")
    order = sorted(multi.items(), key=lambda kv: -len(kv[1]))
    for header, runs in order[:top]:
        between = collections.Counter()
        for a, b in zip(runs, runs[1:]):
            for run in plan.runs[a.index + 1:b.index]:
                between[run.target or "(saga.h)"] += 1
        items = sum(len(r.items) for r in runs)
        span = f"{runs[0].items[0].first}-{runs[-1].items[-1].last}"
        why = ""
        if not plan.exact:
            witness = plan.forced.get(header)
            if witness is not None:
                item = plan.seg.items[witness]
                why = f"; forced: {item.kind} {(item.names or ['?'])[0]} (line {item.first})"
            else:
                why = f"; {plan.split.get(header, '?')}"
        report(f"  {header:<44} {len(runs):>4} {items:>5} {span:>12}  " +
               ", ".join(f"{h} {n}" for h, n in between.most_common(3)) + why)
    # What breaks runs: single items of another header between two items of one header.
    breakers = collections.Counter()
    for k in range(1, len(plan.runs) - 1):
        run = plan.runs[k]
        if len(run.items) == 1 and plan.runs[k - 1].target == plan.runs[k + 1].target:
            breakers[run.items[0].kind] += 1
    report("\nsingle items that split a run of another header, by kind: " +
           ", ".join(f"{k} {n}" for k, n in breakers.most_common()))


def dry_run(plan, report=print):
    shim = render_shim(plan)
    report(f"would write {len(plan.targets)} headers and a shim of {shim.count(chr(10))} lines "
           f"({sum(1 for r in plan.runs if r.target)} includes, {len(plan.unplaced)} items left in place)")
    for header in sorted(plan.targets):
        runs = plan.targets[header]
        plain, shim_only, notes = plan.includes.get(header, ([], [], []))
        items = sum(len(r.items) for r in runs)
        report(f"  {header:<48} items {items:>5} parts {len(runs):>3} includes {len(plain)}+{len(shim_only)}"
               + ("  [" + SHIM_NOTE + "]" if shim_only else ""))
    if plan.stale:
        report(f"{len(plan.stale)} map rows match no item (stale): " + ", ".join(r["key"] for r in plan.stale[:10]))
    if plan.missing:
        report(f"{len(plan.missing)} items have no map row: " + ", ".join(i.key for i in plan.missing[:10]))


def default_android():
    for candidate in (os.environ.get("SAGA_ANDROID"), ROOT.parent / "saga-android", ROOT.parent / "saga"):
        if candidate and (Path(candidate) / "src/decomp.h").is_file():
            return Path(candidate)
    return None


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--build-map", action="store_true", help=f"write {MAP} from the evidence")
    mode.add_argument("--stats", action="store_true", help="items per kind/evidence/target and the unknowns")
    mode.add_argument("--dry-run", action="store_true", help="what --apply would write")
    mode.add_argument("--interleaving", action="store_true", help="headers whose items are interleaved")
    mode.add_argument("--apply", action="store_true", help="write the split into ROOT/src")
    mode.add_argument("--check-neutral", nargs="+", metavar="UNIT",
                      help="compare preprocessed units token by token (the --exact-order split)")
    mode.add_argument("--check-counts", nargs="+", metavar="UNIT",
                      help="compare the declarations units see as multisets, and their clang errors")
    mode.add_argument("--roundtrip", action="store_true", help="check that the segmentation is lossless")
    mode.add_argument("--place", nargs="+", metavar="NAME",
                      help="target header of names declared in units (declscan class C/D) by the same rules")
    parser.add_argument("--android", help="Android decompilation checkout (default: ../saga-android or ../saga)")
    parser.add_argument("--map", default=MAP, help=f"mapping file (default {MAP})")
    parser.add_argument("--root", default=str(ROOT), help="tree to apply the split to (default: this tree)")
    parser.add_argument("--exact-order", action="store_true",
                        help="keep saga.h order (token-neutral; most headers in several parts)")
    parser.add_argument("--debug", action="store_true", help="--check-neutral/--check-counts: also with SAGA_DEBUG")
    parser.add_argument("--keep", action="store_true", help=f"--check-neutral/--check-counts: keep {SCRATCH}")
    parser.add_argument("--top", type=int, default=40, help="rows per report table")
    args = parser.parse_args(argv)
    text = read_text(ROOT / SAGA)
    seg = segment(text)
    if args.roundtrip:
        if seg.roundtrip() != text:
            raise ValueError("segmentation is not lossless")
        print(f"{len(seg.items)} items and {len(seg.events) - len(seg.items)} context directives give back "
              f"{SAGA} byte for byte")
        return 0
    map_path = ROOT / args.map
    if args.place:
        android = Path(args.android) if args.android else default_android()
        for name, target, how, detail in place_names(args.place, Evidence(ROOT, android)):
            print(f"{name}\t{target or '(unknown)'}\t{how}\t{detail}")
        return 0
    if args.build_map:
        android = Path(args.android) if args.android else default_android()
        if android is None:
            raise ValueError("--build-map needs the Android checkout (--android PATH)")
        previous = read_map(map_path) if map_path.exists() else None
        rows = build_map(seg, Evidence(ROOT, android), previous)
        write_map(rows, map_path)
        placed, total, placed_lines, lines = coverage(rows)
        print(f"wrote {args.map}: {total} items, {placed} placed ({100 * placed / total:.1f}%), "
              f"{placed_lines}/{lines} lines")
        return 0
    rows = read_map(map_path)
    if args.stats:
        stats(seg, rows, args.top)
        return 0
    plan = make_plan(seg, rows, exact=args.exact_order or bool(args.check_neutral))
    if args.interleaving:
        interleaving(plan, args.top)
        return 0
    if args.dry_run:
        dry_run(plan)
        return 0
    if args.apply:
        if plan.stale or plan.missing:
            raise ValueError(f"the map does not match {SAGA}: {len(plan.stale)} stale rows, "
                             f"{len(plan.missing)} items without a row (run --build-map)")
        written = apply_plan(plan, Path(args.root) / "src")
        print(f"wrote {len(written)} headers and the shim under {Path(args.root) / 'src'}")
        return 0
    if args.check_neutral or args.check_counts:
        compare = compare_units if args.check_neutral else compare_counts
        units = [Path(u).resolve().relative_to(ROOT).as_posix() if Path(u).is_absolute() else u
                 for u in (args.check_neutral or args.check_counts)]
        scratch = scratch_tree(plan)
        try:
            differing = compare(units, ROOT, scratch)
            if args.debug:
                differing += compare(units, ROOT, scratch, debug=True)
        finally:
            if not args.keep:
                shutil.rmtree(scratch, ignore_errors=True)
        if differing:
            print(f"{len(differing)} of {len(units)} units differ")
            return 1
        verdict = "neutral" if args.check_neutral else "see the same declarations"
        print(f"all {len(units)} units {verdict}" + (" (Matching and Debug)" if args.debug else ""))
        return 0
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"headersplit: {error}", file=sys.stderr)
        sys.exit(1)
