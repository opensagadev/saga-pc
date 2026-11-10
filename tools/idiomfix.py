#!/usr/bin/env python3
"""Try mechanical source rewrites for one VC8 idiom and keep only improvements.

    python3 tools/idiomfix.py I17                       # every function tagged I17
    python3 tools/idiomfix.py I20 --only NuStrToL       # one function
    python3 tools/idiomfix.py I3 --report scan.json     # tags from `idiomscan.py --scan --save`

Candidates are the functions whose `idioms` (matching.json, written by
`match.py --update`, or a saved `idiomscan.py --scan`) contain the idiom. For
each one the idiom's rewrite rules (located with libclang, like
tools/rewrite.py) are applied one at a time to the current source. The unit is
reformatted with clang-format and compiled; a rewrite is kept only when the
function scores higher and no other function of the unit scores lower.
After a kept rewrite the rules run again on the new text, so nested guards
unwind one level per round.

Rules per idiom (docs/vc8-idioms.md):

  I3   mirror an `==`/`!=` whose left operand would not become a constant
       (only on source lines behind the function's I3 spots)
  I10  `x + x` <-> `x * 2.0f` on floats (lines behind I10 spots)
  I17  `if (c) { body } return v;`  <->  `if (!c) return v; body return v;`,
       `return c ? a : b;` -> if/return, `if (c) return a; return b;` ->
       `if (!c) return b; return a;`, and the result-variable /
       guard-to-else rewrites of rewrite.py
  I20  `return c ? a : b;` / `return a < b;` -> `if (c) return a; return b;`,
       and `x = c ? a : b;` -> if/else
  I23  `x <= N` <-> `x < N + 1` where exact, unsigned `x != 0` <-> `x > 0`
       (lines behind I23 spots)
  I25  swap the operands of a commutative operator (lines behind I25 spots)
  I29  `if (a || b) S`  <->  `if (a) S if (b) S` for a return (or one-statement) S
       without `else`
  I31  parenthesise the product terms of a float sum

Every kept rewrite is printed and appended to build/tools-local/idiomfix.log.
The driver does not commit; review the diff for natural source before
committing.
"""
import argparse
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from tools import unitscore  # noqa: E402

LOG = ROOT / "build/tools-local/idiomfix.log"
SCORES = ROOT / f"build/tools-local/idiomfix-score/{os.getpid()}"
EPSILON = 1e-6
_pending = {}  # path -> text to restore when interrupted during a trial


def _interrupted(number, frame):
    for path, text in _pending.items():
        path.write_text(text, encoding="latin-1")
    raise SystemExit(f"interrupted; restored {', '.join(str(p) for p in _pending) or 'nothing'}")


# --- libclang helpers -----------------------------------------------------------------------------

def _rewrite():
    from tools import rewrite
    return rewrite


def _negate(source, cond):
    """(text, top operator) of the logical negation of `cond`, De Morgan on && and ||."""
    rw = _rewrite()
    K = rw.K
    inner = rw.strip(cond)
    if inner.kind == K.BINARY_OPERATOR:
        op = source.operator(inner)
        left, right = list(inner.get_children())
        if op in ("&&", "||"):
            other = "||" if op == "&&" else "&&"
            parts = []
            for child in (left, right):
                text, top = _negate(source, child)
                parts.append(f"({text})" if top == op else text)
            return f"{parts[0]} {other} {parts[1]}", other
        if op in ("==", "!=") or op in rw.NEGATE and not (rw.is_float(left) or rw.is_float(right)):
            return f"{source.src(left)} {rw.NEGATE[op]} {source.src(right)}", "cmp"
        return f"!({source.src(inner)})", "not"
    if inner.kind == K.UNARY_OPERATOR and source.src(inner).startswith("!"):
        operand = rw.strip(next(inner.get_children()))
        text = source.src(operand)
        top = "atom" if re.fullmatch(r"[\w.\->\[\]()]+", text) else "?"
        return (text, top) if top == "atom" else (f"({text})", "atom")
    text = source.src(inner)
    if re.fullmatch(r"[\w.\->\[\]]+", text) or inner.kind == K.CALL_EXPR:
        return f"!{text}", "not"
    return f"!({text})", "not"


def _body_text(source, stmt):
    """Statements of a then-branch without its braces."""
    text = source.src(stmt)
    if stmt.kind == _rewrite().K.COMPOUND_STMT:
        return text.strip()[1:-1].strip()
    return text if text.rstrip().endswith(";") else text + ";"


def _final_return(source, stmts):
    """(text of the final `return ...;`, its statement) or ("", None) for a void body."""
    rw = _rewrite()
    if stmts and stmts[-1].kind == rw.K.RETURN_STMT:
        text = source.src(stmts[-1]).strip()
        return (text if text.endswith(";") else text + ";"), stmts[-1]
    return ("", None) if source.returns_void else (None, None)


def _simple_return(source, stmt):
    rw = _rewrite()
    if stmt is None:
        return True
    children = source.children(stmt)
    return not children or rw.pure(source, children[0]) and not any(
        n.kind == rw.K.CALL_EXPR for n in [children[0], *source.walk(children[0])])


def rule_wrap_to_guard(source):
    """if (c) { body } return v;  ->  if (!c) return v; body return v;  (function body level)."""
    rw = _rewrite()
    stmts = source.children(source.body)
    final, final_stmt = _final_return(source, stmts)
    if final is None or not _simple_return(source, final_stmt):
        return
    tail = len(stmts) - (1 if final_stmt is not None else 0)
    for i in range(tail):
        stmt = stmts[i]
        if stmt.kind != rw.K.IF_STMT or i != tail - 1:
            continue
        parts = source.children(stmt)
        if len(parts) != 2:
            continue
        cond, then = parts
        text, _ = _negate(source, cond)
        early = final if final else "return;"
        a, b = source.span(stmt)
        if source.data[b:b + 1] == b";":
            b += 1
        if _ends_in_return(source, then) and final_stmt is not None:
            # The body returns itself: the old final return becomes dead, drop it.
            b = source.span(final_stmt)[1]
            if source.data[b:b + 1] == b";":
                b += 1
        replacement = f"if ({text})\n{early}\n{_body_text(source, then)}"
        yield rw.Candidate(source.edit([(a, b, replacement)]), "wrap_to_guard",
                           f"line {stmt.location.line}", True)


def _ends_in_return(source, stmt):
    rw = _rewrite()
    while stmt.kind == rw.K.COMPOUND_STMT:
        children = source.children(stmt)
        if not children:
            return False
        stmt = children[-1]
    return stmt.kind == rw.K.RETURN_STMT


def _is_return_of(source, stmt, final):
    rw = _rewrite()
    if stmt.kind == rw.K.COMPOUND_STMT:
        children = source.children(stmt)
        if len(children) != 1:
            return False
        stmt = children[0]
    if stmt.kind != rw.K.RETURN_STMT:
        return False
    text = source.src(stmt).strip()
    text = text if text.endswith(";") else text + ";"
    return re.sub(r"\s+", " ", text) == re.sub(r"\s+", " ", final or "return;")


def rule_guard_to_wrap(source):
    """if (c) return v; rest return v;  ->  if (!c) { rest } return v;  (function body level)."""
    rw = _rewrite()
    stmts = source.children(source.body)
    final, final_stmt = _final_return(source, stmts)
    if final is None or not _simple_return(source, final_stmt):
        return
    tail = len(stmts) - (1 if final_stmt is not None else 0)
    for i in range(tail - 1):
        stmt = stmts[i]
        if stmt.kind != rw.K.IF_STMT:
            continue
        parts = source.children(stmt)
        if len(parts) != 2 or not _is_return_of(source, parts[1], final):
            continue
        if any(s.kind == rw.K.DECL_STMT for s in stmts[i + 1:tail]):
            continue
        text, _ = _negate(source, parts[0])
        a, _ = source.span(stmt)
        b = source.span(stmts[tail - 1])[1]
        if source.data[b:b + 1] == b";":
            b += 1
        rest = source.data[source.span(stmts[i + 1])[0]:b].decode("latin-1")
        replacement = f"if ({text}) {{\n{rest}\n}}"
        yield rw.Candidate(source.edit([(a, b, replacement)]), "guard_to_wrap",
                           f"line {stmt.location.line}", True)


def rule_ternary_return(source):
    """return c ? a : b;  ->  if (c) return a; return b;   and  return a < b;  ->  if/return 1/0."""
    rw = _rewrite()
    K = rw.K
    if source.returns_void:
        return
    for stmt in source.walk():
        if stmt.kind != K.RETURN_STMT or not source.children(stmt):
            continue
        value = rw.strip(source.children(stmt)[0])
        a, b = source.span(stmt)
        if source.data[b:b + 1] == b";":
            b += 1
        if value.kind == K.CONDITIONAL_OPERATOR:
            c, x, y = list(value.get_children())
            negated, _ = _negate(source, c)
            for cond, first, second, note in ((source.src(c), x, y, ""), (negated, y, x, " inverted")):
                replacement = f"if ({cond})\nreturn {source.src(first)};\nreturn {source.src(second)};"
                if not _ends_block(source, stmt):
                    replacement = f"if ({cond})\nreturn {source.src(first)};\nelse\nreturn {source.src(second)};"
                yield rw.Candidate(source.edit([(a, b, replacement)]), "ternary_return",
                                   f"line {stmt.location.line}{note}", True)
            continue
        op = source.operator(value) if value.kind == K.BINARY_OPERATOR else None
        logical = op in ("==", "!=", "<", ">", "<=", ">=", "&&", "||") or (
            value.kind == K.UNARY_OPERATOR and source.src(value).startswith("!"))
        if not logical:
            continue
        negated, _ = _negate(source, value)
        for cond, first, second, note in ((source.src(value), "1", "0", ""), (negated, "0", "1", " inverted")):
            replacement = f"if ({cond})\nreturn {first};\nreturn {second};"
            if not _ends_block(source, stmt):
                replacement = f"if ({cond})\nreturn {first};\nelse\nreturn {second};"
            yield rw.Candidate(source.edit([(a, b, replacement)]), "bool_return",
                               f"line {stmt.location.line}{note}", True)


def rule_invert_return(source):
    """if (c) return a; return b;  ->  if (!c) return b; return a;  (VC8 falls through into the if body)."""
    rw = _rewrite()
    for compound in [source.body, *source.walk()]:
        if compound.kind != rw.K.COMPOUND_STMT:
            continue
        stmts = source.children(compound)
        for first, second in zip(stmts, stmts[1:]):
            if first.kind != rw.K.IF_STMT or second.kind != rw.K.RETURN_STMT:
                continue
            parts = source.children(first)
            if len(parts) != 2:
                continue
            then = parts[1]
            if then.kind == rw.K.COMPOUND_STMT:
                inner = source.children(then)
                if len(inner) != 1:
                    continue
                then = inner[0]
            if then.kind != rw.K.RETURN_STMT:
                continue
            text, _ = _negate(source, parts[0])
            a = source.span(first)[0]
            b = source.span(second)[1]
            if source.data[b:b + 1] == b";":
                b += 1
            ret_a = source.src(then).strip().rstrip(";") + ";"
            ret_b = source.src(second).strip().rstrip(";") + ";"
            yield rw.Candidate(source.edit([(a, b, f"if ({text})\n{ret_b}\n{ret_a}")]), "invert_return",
                               f"line {first.location.line}", True)


def _ends_block(source, stmt):
    """The statement is the last one of a compound statement (a following return is reachable)."""
    rw = _rewrite()
    for compound in [source.body, *source.walk()]:
        if compound.kind == rw.K.COMPOUND_STMT:
            children = source.children(compound)
            if children and children[-1] == stmt:
                return True
    return False


def _constant(source, cursor):
    """No variable, field or call in the expression: it is a constant."""
    rw = _rewrite()
    K = rw.K
    for node in [cursor, *source.walk(cursor)]:
        if node.kind in (K.CALL_EXPR, K.MEMBER_REF_EXPR, K.ARRAY_SUBSCRIPT_EXPR):
            return False
        if node.kind == K.DECL_REF_EXPR and node.referenced is not None and \
                node.referenced.kind in (K.VAR_DECL, K.PARM_DECL, K.FIELD_DECL, K.FUNCTION_DECL):
            return False
    tokens = [t.spelling for t in cursor.get_tokens()]
    return bool(tokens)


def _operand(source, cursor):
    """Operand text for an equality, parenthesised only when precedence needs it."""
    rw = _rewrite()
    inner = rw.strip(cursor)
    text = source.src(cursor)
    if inner.kind in (rw.K.CONDITIONAL_OPERATOR, rw.K.COMPOUND_ASSIGNMENT_OPERATOR) or (
            inner.kind == rw.K.BINARY_OPERATOR and source.operator(inner) not in ("+", "-", "*", "/", "%", "<<", ">>")):
        return text if cursor.kind == rw.K.PAREN_EXPR else f"({text})"
    return text


def rule_mirror_equality(source):
    """a == b  ->  b == a  when b is not a constant (no Yoda comparisons)."""
    rw = _rewrite()
    for node in source.walk():
        if node.kind != rw.K.BINARY_OPERATOR or source.operator(node) not in ("==", "!="):
            continue
        left, right = list(node.get_children())
        if _constant(source, right) or _constant(source, left):
            continue
        a, b = source.span(node)
        op = source.operator(node)
        yield rw.Candidate(source.edit([(a, b, f"{_operand(source, right)} {op} {_operand(source, left)}")]),
                           "mirror_equality", f"line {node.location.line}", True)


def rule_double(source):
    """x + x  <->  x * 2.0f on floats."""
    rw = _rewrite()
    for node in source.walk():
        if node.kind != rw.K.BINARY_OPERATOR or not rw.is_float(node):
            continue
        op = source.operator(node)
        left, right = list(node.get_children())
        a, b = source.span(node)
        lt, rt = source.src(left), source.src(right)
        if op == "+" and lt == rt and rw.pure(source, left):
            yield rw.Candidate(source.edit([(a, b, f"{rw.paren(lt)} * 2.0f")]), "add_to_mul",
                               f"line {node.location.line}", True)
        elif op == "*" and re.fullmatch(r"2(\.0*)?f?", rt.strip()) and rw.pure(source, left):
            yield rw.Candidate(source.edit([(a, b, f"{rw.paren(lt)} + {rw.paren(lt)}")]), "mul_to_add",
                               f"line {node.location.line}", True)


def rule_bound(source):
    """x <= N  <->  x < N + 1 (integers, only where the bound stays exact), and on unsigned
    operands x != 0  <->  x > 0."""
    rw = _rewrite()
    for candidate in rw.rule_compare(source):
        if candidate.kind == "bound_compare" and candidate.safe:
            yield candidate
    for node in source.walk():
        if node.kind != rw.K.BINARY_OPERATOR:
            continue
        op = source.operator(node)
        if op not in ("!=", ">"):
            continue
        left, right = list(node.get_children())
        unsigned = rw.strip(left).type.get_canonical().kind in (rw.ci.TypeKind.UINT, rw.ci.TypeKind.UCHAR,
                                                     rw.ci.TypeKind.USHORT, rw.ci.TypeKind.ULONG,
                                                     rw.ci.TypeKind.CHAR_U)
        if not unsigned or source.src(right).strip() not in ("0", "0u", "0U"):
            continue
        a, b = source.span(node)
        new = ">" if op == "!=" else "!="
        yield rw.Candidate(source.edit([(a, b, f"{source.src(left)} {new} {source.src(right)}")]),
                           "unsigned_zero", f"line {node.location.line}", True)


def rule_float_commute(source):
    """a + b  ->  b + a (and *) on floats: the operand loaded first follows the source."""
    rw = _rewrite()
    for node in source.walk():
        if node.kind != rw.K.BINARY_OPERATOR or source.operator(node) not in ("+", "*") or not rw.is_float(node):
            continue
        left, right = list(node.get_children())
        if not (rw.pure(source, left) and rw.pure(source, right)):
            continue
        a, b = source.span(node)
        op = source.operator(node)
        text = f"{_commute_operand(source, right, op, False)} {op} {_commute_operand(source, left, op, True)}"
        yield rw.Candidate(source.edit([(a, b, text)]), "commute", f"line {node.location.line}", True)


def _wrapped(text):
    """True when the whole text is one parenthesised group."""
    if not (text.startswith("(") and text.endswith(")")):
        return False
    depth = 0
    for i, ch in enumerate(text):
        depth += {"(": 1, ")": -1}.get(ch, 0)
        if depth == 0 and i < len(text) - 1:
            return False
    return True


def _commute_operand(source, cursor, op, on_right):
    """Operand text after `a op b -> b op a`, parenthesised only when the grouping needs it."""
    rw = _rewrite()
    text = source.src(cursor)
    if _wrapped(text):
        return text
    cursor = rw.strip(cursor)
    if cursor.kind == rw.K.PAREN_EXPR or cursor.kind in (rw.K.CALL_EXPR, rw.K.DECL_REF_EXPR, rw.K.MEMBER_REF_EXPR,
                                                          rw.K.ARRAY_SUBSCRIPT_EXPR, rw.K.FLOATING_LITERAL,
                                                          rw.K.INTEGER_LITERAL, rw.K.CSTYLE_CAST_EXPR,
                                                          rw.K.UNARY_OPERATOR):
        return text
    if cursor.kind == rw.K.BINARY_OPERATOR:
        inner = source.operator(cursor)
        tighter = ("*", "/", "%") if op == "+" else ()
        if inner in tighter or (inner == op and not on_right):
            return text
    return f"({text})"


def rule_paren_terms(source):
    """a * b - c * d  ->  (a * b) - (c * d) on floats: VC8 evaluates a parenthesised term as written."""
    rw = _rewrite()
    sums = [node for node in source.walk() if _is_sum(source, node) and rw.is_float(node)]
    inner = {source.span(term) for node in sums for term in _operands(node) if _is_sum(source, term)}
    for node in sums:
        if source.span(node) in inner:
            continue
        edits = [(*source.span(term), f"({source.src(term)})") for term in _sum_terms(source, node)
                 if term.kind == rw.K.BINARY_OPERATOR and source.operator(term) in ("*", "/")]
        if edits:
            yield rw.Candidate(source.edit(edits), "paren_terms", f"line {node.location.line}", True)


def _is_sum(source, cursor):
    rw = _rewrite()
    return cursor.kind == rw.K.BINARY_OPERATOR and source.operator(cursor) in ("+", "-")


def _operands(node):
    rw = _rewrite()
    out = []
    for child in node.get_children():
        while child.kind == rw.K.UNEXPOSED_EXPR and len(list(child.get_children())) == 1:
            child = next(child.get_children())
        out.append(child)
    return out


def _sum_terms(source, node):
    """Terms of an unparenthesised +/- chain."""
    terms = []
    for child in _operands(node):
        terms.extend(_sum_terms(source, child) if _is_sum(source, child) else [child])
    return terms


def _single(source, stmt):
    """The one statement of a then-branch (unwrapping braces), or None."""
    rw = _rewrite()
    if stmt.kind == rw.K.COMPOUND_STMT:
        inner = source.children(stmt)
        return inner[0] if len(inner) == 1 else None
    return stmt


def _flat(source, stmt):
    text = source.src(stmt).strip()
    return re.sub(r"\s+", " ", text if text.endswith(";") else text + ";")


def _or_operand(source, cursor):
    rw = _rewrite()
    inner = rw.strip(cursor)
    text = source.src(cursor)
    if inner.kind in (rw.K.CONDITIONAL_OPERATOR, rw.K.COMPOUND_ASSIGNMENT_OPERATOR) or (
            inner.kind == rw.K.BINARY_OPERATOR and source.operator(inner) in ("=", ",")):
        return text if cursor.kind == rw.K.PAREN_EXPR else f"({text})"
    return text


def rule_guard_split(source):
    """if (a || b) S  ->  if (a) S if (b) S  (S one return or simple statement, no else)."""
    rw = _rewrite()
    for stmt in source.walk():
        if stmt.kind != rw.K.IF_STMT:
            continue
        parts = source.children(stmt)
        if len(parts) != 2:
            continue
        cond = rw.strip(parts[0])
        then = _single(source, parts[1])
        if cond.kind != rw.K.BINARY_OPERATOR or source.operator(cond) != "||" or then is None:
            continue
        if then.kind not in (rw.K.RETURN_STMT, rw.K.BREAK_STMT, rw.K.CONTINUE_STMT):
            continue
        left, right = list(cond.get_children())
        body = _flat(source, then)
        a, b = source.span(stmt)
        if source.data[b:b + 1] == b";":
            b += 1
        replacement = f"if ({source.src(left)})\n{body}\nif ({source.src(right)})\n{body}"
        yield rw.Candidate(source.edit([(a, b, replacement)]), "guard_split", f"line {stmt.location.line}", True)


def rule_guard_merge(source):
    """if (a) S if (b) S  ->  if (a || b) S  (the same return or simple statement, no else)."""
    rw = _rewrite()
    for compound in [source.body, *source.walk()]:
        if compound.kind != rw.K.COMPOUND_STMT:
            continue
        stmts = source.children(compound)
        for first, second in zip(stmts, stmts[1:]):
            if first.kind != rw.K.IF_STMT or second.kind != rw.K.IF_STMT:
                continue
            p1, p2 = source.children(first), source.children(second)
            if len(p1) != 2 or len(p2) != 2:
                continue
            t1, t2 = _single(source, p1[1]), _single(source, p2[1])
            if t1 is None or t2 is None or t1.kind not in (rw.K.RETURN_STMT, rw.K.BREAK_STMT, rw.K.CONTINUE_STMT):
                continue
            if _flat(source, t1) != _flat(source, t2):
                continue
            a = source.span(first)[0]
            b = source.span(second)[1]
            if source.data[b:b + 1] == b";":
                b += 1
            replacement = f"if ({_or_operand(source, p1[0])} || {_or_operand(source, p2[0])})\n{_flat(source, t1)}"
            yield rw.Candidate(source.edit([(a, b, replacement)]), "guard_merge", f"line {first.location.line}",
                               True)


# Idioms without a scanner tag of their own (or a narrow one) run on the functions tagged with these.
SELECT = {"I31": ("I25", "U9", "I10", "I31"), "I29": ("I29", "I17", "I22")}


def _rewrite_rules(names):
    rw = _rewrite()
    return [getattr(rw, name) for name in names]


RULES = {
    "I3": (lambda: [rule_mirror_equality], True),
    "I23": (lambda: [rule_bound], True),
    "I25": (lambda: [rule_float_commute], True),
    "I31": (lambda: [rule_paren_terms], False),
    "I29": (lambda: [rule_guard_split, rule_guard_merge], False),
    "I10": (lambda: [rule_double], True),
    "I17": (lambda: [rule_wrap_to_guard, rule_guard_to_wrap, rule_ternary_return, rule_invert_return,
                     *_rewrite_rules(["rule_result_variable", "rule_guard_to_else"])], False),
    "I20": (lambda: [rule_ternary_return, *_rewrite_rules(["rule_ternary"])], False),
}


# --- driver ---------------------------------------------------------------------------------------

def object_for(unit):
    return "build/obj/" + unit[4:].rsplit(".", 1)[0] + ".obj"


def build(unit):
    result = subprocess.run(["make", "-s", "-j4", object_for(unit)], cwd=ROOT, capture_output=True, text=True)
    return result.returncode == 0


def fmt(path):
    from tools.check import clang_tool
    subprocess.run([clang_tool("clang-format"), "-i", str(path)], cwd=ROOT, check=True)


def score(unit, symbols=None):
    return unitscore.snapshot([unit], None, symbols=symbols, out_dir=SCORES, quiet=True)


def spot_lines(unit, row, idiom):
    """Source lines (of `unit`) behind the spots tagged `idiom`."""
    from tools import diffclass, idiomscan, whyline
    from tools.binary import COFF, relocate_function
    from tools.lib import bindings_for, load_symbols, project, retail
    obj, listing = whyline.compile_listing(unit)
    va, size, symbol = int(row["address"], 16), row["target_size"], row["symbol"]
    base, _ = relocate_function(COFF(obj), symbol, va, bindings_for(load_symbols()))
    original = retail(project()["target"]).read_va(va, size)
    target, ours = diffclass.disassemble(original, va), diffclass.disassemble(base, va)
    offsets = whyline.disassemble(base, va)
    entries = whyline.parse_listing(listing.read_text(errors="replace"), symbol)
    lines = set()
    for (tag, i1, i2, j1, j2), tags in idiomscan.spot_tags(target, ours):
        if idiom not in tags:
            continue
        for j in range(j1, max(j2, j1 + 1)):
            if j >= len(offsets):
                continue
            name, found = whyline.lines_at(entries, offsets[j][0])
            if name is None or Path(re.sub(r"^[a-zA-Z]:", "", name.replace("\\", "/"))).name == Path(unit).name:
                lines.update(found)
    return lines


def log(text):
    print(text, flush=True)
    LOG.parent.mkdir(parents=True, exist_ok=True)
    with LOG.open("a") as handle:
        handle.write(text + "\n")


def fix_function(row, idiom, rules, by_lines, baseline, max_rounds=4):
    """Try the rules on one function; return the new baseline of its unit."""
    rw = _rewrite()
    unit, symbol = row["units"][0], row["symbol"]
    path = ROOT / unit
    name = rw.function_name(symbol)
    start = baseline.get(symbol, {}).get("percent")
    if start is None:
        return baseline
    lines = spot_lines(unit, row, idiom) if by_lines else None
    if by_lines and not lines:
        log(f"{row['name']}: no {idiom} spot lines")
        return baseline
    tried = set()
    for _ in range(max_rounds):
        text = path.read_text(encoding="latin-1")
        try:
            candidates = []
            source = rw.Source(text, unit, name)
            for rule in rules:
                try:
                    candidates.extend(rule(source))
                except (ValueError, StopIteration, AttributeError):
                    continue
        except ValueError as error:
            log(f"{row['name']}: {error}")
            return baseline
        kept = False
        for candidate in candidates:
            found = re.match(r"line (\d+)", candidate.note)
            if lines is not None and not (found and int(found.group(1)) in lines):
                continue
            try:
                new_text = candidate.text
            except ValueError:
                continue
            if re.search(r"break;\s*break;", new_text) and not re.search(r"break;\s*break;", text):
                continue  # a result-variable rewrite left a dead `break;`
            if new_text in tried:
                continue
            tried.add(new_text)
            try:
                baseline, kept = try_candidate(row, candidate, path, text, new_text, baseline)
            except Exception as error:  # keep going: one odd candidate must not stop the batch
                log(f"{row['name']}: {candidate.kind} {candidate.note}: error {type(error).__name__}: {error}")
                path.write_text(text, encoding="latin-1")
                kept = False
            if kept:
                break
        if not kept or baseline[symbol]["exact"]:
            break
    end = baseline[symbol]
    log(f"RESULT {row['name']} ({row['target_size']} B): {start:.2f} -> "
        f"{'exact' if end['exact'] else format(end['percent'], '.2f')}")
    return baseline


def try_candidate(row, candidate, path, text, new_text, baseline):
    """(baseline, kept): compile and score one rewrite; restore `text` unless it is kept."""
    _pending[path] = text
    try:
        return _try_candidate(row, candidate, path, text, new_text, baseline)
    finally:
        _pending.pop(path, None)


def _try_candidate(row, candidate, path, text, new_text, baseline):
    unit, symbol = row["units"][0], row["symbol"]
    path.write_text(new_text, encoding="latin-1")
    try:
        fmt(path)
    except subprocess.CalledProcessError:
        path.write_text(text, encoding="latin-1")
        return baseline, False
    if not build(unit):
        path.write_text(text, encoding="latin-1")
        return baseline, False
    quick = score(unit, {symbol})
    better = symbol in quick and (quick[symbol]["percent"] > baseline[symbol]["percent"] + EPSILON
                                  or quick[symbol]["exact"] and not baseline[symbol]["exact"])
    if not better:
        path.write_text(text, encoding="latin-1")
        return baseline, False
    after = score(unit)
    worse = [s for s, r in after.items() if s in baseline and s != symbol and
             (r["percent"] < baseline[s]["percent"] - EPSILON or baseline[s]["exact"] and not r["exact"])]
    if worse:
        log(f"{row['name']}: {candidate.kind} {candidate.note} rejected, worse: "
            + ", ".join(unitscore.display(s) for s in worse))
        path.write_text(text, encoding="latin-1")
        return baseline, False
    log(f"{row['name']}: {candidate.kind} {candidate.note}: {baseline[symbol]['percent']:.2f} -> "
        f"{'exact' if after[symbol]['exact'] else format(after[symbol]['percent'], '.2f')}")
    return after, True


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("idiom", choices=sorted(RULES))
    parser.add_argument("--report", type=Path, default=ROOT / "matching.json")
    parser.add_argument("--only", nargs="+", help="function names")
    parser.add_argument("--skip", nargs="+", default=[], help="function names to leave alone")
    parser.add_argument("--units", nargs="+", help="only functions of these units")
    parser.add_argument("--rules", nargs="+", help="only these rule functions (e.g. rule_ternary_return)")
    parser.add_argument("--shard", help="I/N: only every N-th unit starting at I (parallel runs)")
    parser.add_argument("--skip-done", action="store_true",
                        help="skip functions that already have a RESULT line in the log")
    args = parser.parse_args(argv)
    signal.signal(signal.SIGTERM, _interrupted)
    signal.signal(signal.SIGINT, _interrupted)
    make_rules, by_lines = RULES[args.idiom]
    rules = make_rules()
    if args.rules:
        rules = [rule for rule in rules if rule.__name__ in args.rules]
    rows = [r for r in json.loads(args.report.read_text())["functions"]
            if r.get("status") == "different" and set(SELECT.get(args.idiom, (args.idiom,))) & set(r.get("idioms", []))
            and len(r.get("units", [])) == 1 and r["units"][0].endswith((".c", ".cpp"))]
    if args.only:
        rows = [r for r in rows if r["name"] in args.only]
    if args.units:
        rows = [r for r in rows if r["units"][0] in args.units]
    rows = [r for r in rows if r["name"] not in args.skip]
    if args.skip_done and LOG.is_file():
        done = set(re.findall(r"^RESULT (\S+) ", LOG.read_text(), re.M))
        rows = [r for r in rows if r["name"] not in done]
    by_unit = {}
    for row in rows:
        by_unit.setdefault(row["units"][0], []).append(row)
    if args.shard:
        index, count = (int(x) for x in args.shard.split("/"))
        by_unit = {u: r for k, (u, r) in enumerate(sorted(by_unit.items())) if k % count == index}
    log(f"== {args.idiom}: {len(rows)} candidates in {len(by_unit)} units")
    for unit, unit_rows in sorted(by_unit.items()):
        if not build(unit):
            log(f"{unit}: does not build, skipped")
            continue
        baseline = score(unit)
        for row in unit_rows:
            try:
                baseline = fix_function(row, args.idiom, rules, by_lines, baseline)
            except Exception as error:  # keep going: one odd function must not stop the batch
                log(f"{row['name']}: error {type(error).__name__}: {error}")
                build(unit)
                baseline = score(unit)
    return 0


if __name__ == "__main__":
    sys.exit(main())
