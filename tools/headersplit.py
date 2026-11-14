#!/usr/bin/env python3
"""Split src/saga.h into module headers behind an include-all shim (step 1 of docs/header-plan.md).

saga.h is cut into its top-level items: struct/union/enum/class definitions,
typedefs, forward declarations, function prototypes, extern variables, macros,
STATIC_ASSERTs, includes, and the `#pragma pack` regions and directive-only
conditionals around them. `config/header-map.csv` names the target header of
every item. The split writes each header's items there and turns saga.h into a
shim that includes the headers in an order reproducing the original declaration
order exactly. Items without a target stay in saga.h at their place.

- A header whose items are interleaved with another header's items in saga.h
  gets ordered fragments: `x.h` includes `x.part01.h`, `x.part02.h`, ... and the
  shim includes the fragments at their original places (`--interleaving` lists
  them, so the map can be adjusted instead).
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
    python3 tools/headersplit.py --interleaving         # headers that need fragments, and where
    python3 tools/headersplit.py --apply --root DIR     # write the split into DIR/src (default: this tree)
    python3 tools/headersplit.py --check-neutral src/legoapi/doors.c ...

`--check-neutral` preprocesses each unit with clang -E and the lint flags
(tools/declscan.py), once in this tree and once in a scratch copy with the split
applied (build/headersplit/), and compares the token streams with line markers
dropped. `extern "C" {` blocks are compared by meaning: their braces are dropped
and each token carries its linkage, so splitting one block into several is
neutral and anything else is not. One clang process runs at a time.

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
            tag = [toks[k].text for k in range(1, brace) if toks[k].kind == "id" and toks[k - 1].text == kw]
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
            name = toks[1].text if toks[1].kind == "id" else ""
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
            # function type: typedef i32 NAME(args);
            if i > 0 and toks[i - 1].kind == "id":
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
    # Include guard: #ifndef X / #define X first, the matching #endif last.
    if n >= 2 and tokens[0].kind == "dir" and tokens[0].directive == "ifndef" and \
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
            for name in set(identifier.findall(text)):
                self.users[name].add(relative)
            for match in definition.finditer(text):
                self.definers[match[1]].add(relative)

    # -- helpers ------------------------------------------------------------------------------------------
    def android_choice(self, name):
        """(header, weak, alternatives) of the Android header that declares `name`, or None."""
        headers = sorted(self.android_headers.get(name, ()))
        if not headers:
            return None
        strong = [h for h in headers if not ANDROID_WEAK.search(h)]
        pool = strong or headers
        pool.sort(key=lambda h: (h.count("/") == 0, len(h), h))
        return pool[0], not strong, [h for h in headers if h != pool[0]]

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
            return headers[0], "android-path"
        units = [a for a in android if not a.endswith(".h")]
        if units:
            return f"{Path(units[0]).parent.as_posix()}/{Path(units[0]).stem}.h", "android-dir"
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
        if LIBRARY_NAME.match(name):
            return "externals/ogg.h", "externals", "libogg/libvorbis API " + name
        if name in ev.vendored:
            return "externals/ogg.h", "externals", "vendored " + " ".join(sorted(ev.vendored[name]))
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
    for item in seg.items:
        target = decisions[item.index][0]
        if target and item.kind not in ("forward", "assert"):
            for name in item.names:
                owner_of_name.setdefault(name, target)
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
            elif match[1] in owner_of_name:
                decisions[item.index] = (owner_of_name[match[1]], "follow-type", match[1])
            elif match[1] in ev.externals_names:
                decisions[item.index] = (externals_header(match[1]), "externals",
                                         f"layout check of {match[1]} (src/externals.h)")
            elif match[1] not in all_names:
                decisions[item.index] = ("decomp.h", "decomp", f"data model check of SDK type {match[1]}")
            else:
                decisions[item.index] = (RESIDUE, "unknown", f"asserts on {match[1]}")
    # Items that follow the next item (an #undef before a declaration, an include inside a block).
    for position in range(len(seg.items) - 1, -1, -1):
        item = seg.items[position]
        if decisions[item.index][1] == "follow-next":
            nxt = decisions[seg.items[position + 1].index] if position + 1 < len(seg.items) else (RESIDUE, "", "")
            decisions[item.index] = (nxt[0], "follow-next", seg.items[position + 1].key if nxt[0] else "")
    rows = []
    for item in seg.items:
        target, how, detail = decisions[item.index]
        rows.append({"key": item.key, "kind": item.kind, "names": " ".join(item.names[:6]),
                     "line_start": item.first, "line_end": item.last, "lines": item.last - item.lead + 1,
                     "target": target or RESIDUE, "evidence": how, "detail": detail})
    return rows


STRONG = {"android", "android-stem", "symbol", "definition", "users", "decomp", "externals", "review"}


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


def make_plan(seg, rows):
    by_key = {row["key"]: row for row in rows}
    stale = [row for row in rows if row["key"] not in {i.key for i in seg.items}]
    missing = []
    runs = []
    for item in seg.items:
        row = by_key.get(item.key)
        if row is None:
            missing.append(item)
            target = RESIDUE
        else:
            target = row["target"].strip()
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
    plan = Plan(seg, runs, dict(targets), unplaced, stale, missing)
    plan.includes = dependencies(plan)
    return plan


TYPE_KINDS = {"struct", "union", "enum", "class", "typedef", "forward", "macro", "pack", "inline"}


def dependencies(plan):
    """header -> (plain includes, shim-only includes, notes): what each header needs from other headers.

    A header used by an item is the target of the closest earlier item that declares the name (types,
    typedefs, macros, enumerators). It is included plainly when all of it comes earlier in the shim (a no-op
    there); otherwise only outside the shim (`#ifndef SAGA_SHIM`), so the shim order stays as it is."""
    seg = plan.seg
    target_of = {}
    for run in plan.runs:
        for item in run.items:
            target_of[item.index] = (run.target, run.index)
    declared = collections.defaultdict(list)  # name -> [item index] in order
    for item in seg.items:
        if item.kind in TYPE_KINDS:
            for name in item.names:
                declared[name].append(item.index)
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
                    k = bisect.bisect_left(providers, item.index) - 1
                    if k < 0:
                        continue
                    if any(target_of[p][0] == header for p in providers[:k + 1]):
                        continue
                    provider = target_of[providers[k]][0]
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
            notes.append(f"{SHIM_NOTE}: " + ", ".join(shim_only) + " (not all earlier in saga.h order)")
        if residue:
            notes.append("uses declarations still in saga.h: " + ", ".join(sorted(residue)[:12]) +
                         (" ..." if len(residue) > 12 else ""))
        result[header] = (plain, shim_only, notes)
    return result


def render_items(seg, items):
    """The items' text with their #if contexts and extern "C" blocks re-emitted around them."""
    out = []
    open_frames = []  # [(frame id, branch)]
    linked = False

    def close_link():
        nonlocal linked
        if linked:
            out.append('#ifdef __cplusplus\n}\n#endif')
            linked = False

    for item in items:
        for first, last in item.carried:
            out.append("\n".join(seg.lines[first - 1:last]))
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
        lead = seg.lines[item.lead - 1:item.first - 1]
        out.append("\n".join(lead + seg.lines[item.first - 1:item.last]))
    close_link()
    while open_frames:
        out.append("#endif")
        open_frames.pop()
    return "\n".join(out)


HEADER_NOTE = ("/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at "
               "the place of its declarations. */")


def render_header(plan, header):
    runs = plan.targets[header]
    plain, shim_only, notes = plan.includes.get(header, ([], [], []))
    guard = guard_name(header)
    out = [f"#ifndef {guard}" if len(runs) == 1 else f"#ifndef {guard}_PART",
           f"#define {guard}" if len(runs) == 1 else f"#define {guard}_PART 1",
           f"#endif" if len(runs) > 1 else None, "", HEADER_NOTE]
    out = [line for line in out if line is not None]
    for note in notes:
        out.append(f"/* {note[0].upper()}{note[1:]}. */")
    if plain:
        out.append("")
        out += [f'#include "{h}"' for h in plain]
    if shim_only:
        out += ["", "#ifndef SAGA_SHIM"] + [f'#include "{h}"' for h in shim_only] + ["#endif"]
    if len(runs) == 1:
        out += ["", render_items(plan.seg, runs[0].items), "", f"#endif"]
        return "\n".join(out) + "\n"
    out.append(f"/* {len(runs)} parts: saga.h includes this header once per part, in its declaration order; "
               "included directly, it gives all parts at once. */")
    for run in runs:
        out += ["", f"#if {guard}_PART == {run.part} && !defined({guard}_STOP)",
                render_items(plan.seg, run.items),
                f"#undef {guard}_PART", f"#define {guard}_PART {run.part + 1}",
                "#ifdef SAGA_SHIM", f"#define {guard}_STOP", "#endif", "#endif"]
    out += ["", f"#undef {guard}_STOP"]
    return "\n".join(out) + "\n"


SHIM_COMMENT = """/* Include-all shim (docs/header-plan.md, step 1): the module headers in the order that reproduces the
 * declaration order of the former catch-all header exactly. A header with several parts is included once per
 * part. Declarations without a mapped header (config/header-map.csv) stay here, at their place. */"""


def render_shim(plan):
    seg = plan.seg
    guard = seg.guard
    out = []
    if guard:
        out += [guard[0].text, guard[1].text, ""]
    out += [SHIM_COMMENT, "#define SAGA_SHIM", ""]
    for run in plan.runs:
        if run.target:
            out.append(f'#include "{run.target}"')
        else:
            out.append(render_items(seg, run.items))
    if guard:
        out += ["", guard[2].text]
    return "\n".join(out) + "\n"


def apply_plan(plan, src):
    """Write the shim and the headers under `src` (a directory laid out like src/)."""
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
    report(f"{len(plan.runs)} runs in saga.h order; {len(plan.targets)} headers, {len(multi)} of them in "
           f"several parts ({parts} parts); {sum(1 for r in plan.runs if not r.target)} runs stay in saga.h")
    report("\nheaders by number of parts (parts, items, first-last line, headers in between most often):")
    order = sorted(multi.items(), key=lambda kv: -len(kv[1]))
    for header, runs in order[:top]:
        between = collections.Counter()
        for a, b in zip(runs, runs[1:]):
            for run in plan.runs[a.index + 1:b.index]:
                between[run.target or "(saga.h)"] += 1
        items = sum(len(r.items) for r in runs)
        span = f"{runs[0].items[0].first}-{runs[-1].items[-1].last}"
        report(f"  {header:<44} {len(runs):>4} {items:>5} {span:>12}  " +
               ", ".join(f"{h} {n}" for h, n in between.most_common(3)))
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
    mode.add_argument("--check-neutral", nargs="+", metavar="UNIT", help="compare preprocessed units")
    mode.add_argument("--roundtrip", action="store_true", help="check that the segmentation is lossless")
    mode.add_argument("--place", nargs="+", metavar="NAME",
                      help="target header of names declared in units (declscan class C/D) by the same rules")
    parser.add_argument("--android", help="Android decompilation checkout (default: ../saga-android or ../saga)")
    parser.add_argument("--map", default=MAP, help=f"mapping file (default {MAP})")
    parser.add_argument("--root", default=str(ROOT), help="tree to apply the split to (default: this tree)")
    parser.add_argument("--debug", action="store_true", help="--check-neutral: also with SAGA_DEBUG")
    parser.add_argument("--keep", action="store_true", help=f"--check-neutral: keep {SCRATCH}")
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
    plan = make_plan(seg, rows)
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
    if args.check_neutral:
        units = [Path(u).resolve().relative_to(ROOT).as_posix() if Path(u).is_absolute() else u
                 for u in args.check_neutral]
        scratch = scratch_tree(plan)
        try:
            differing = compare_units(units, ROOT, scratch)
            if args.debug:
                differing += compare_units(units, ROOT, scratch, debug=True)
        finally:
            if not args.keep:
                shutil.rmtree(scratch, ignore_errors=True)
        if differing:
            print(f"{len(differing)} of {len(units)} units differ")
            return 1
        print(f"all {len(units)} units neutral" + (" (Matching and Debug)" if args.debug else ""))
        return 0
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"headersplit: {error}", file=sys.stderr)
        sys.exit(1)
