"""Source-shape rewrites for one function, located with libclang.

The Android sources were shaped until clang reproduced its output. Clang
canonicalizes aggressively (CSE of loads, range-folded comparisons, block
reordering), so many source spellings that VC8 compiles differently look the
same to clang. These rewrites move between such spellings. Each candidate is
marked safe only when it preserves C semantics without aliasing assumptions
beyond "distinct named objects do not overlap"; permute.py keeps an unsafe
candidate only when it makes the function byte-exact.
"""
import re

try:
    import clang.cindex as ci
except ImportError as error:  # Only the rewrite search needs libclang.
    raise SystemExit("tools/rewrite.py needs libclang: python3 -m pip install libclang") from error

from tools.check import analysis_configuration, analysis_language
from tools.lib import ROOT

# Bump when rules change materially: batch runs retry functions searched with older rules.
VERSION = 2
K = ci.CursorKind
FLOATS = {ci.TypeKind.FLOAT, ci.TypeKind.DOUBLE, ci.TypeKind.LONGDOUBLE}
NEGATE = {"==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">"}
MIRROR = {"<": ">", ">": "<", "<=": ">=", ">=": "<=", "==": "==", "!=": "!="}
COMMUTATIVE = {"+", "*", "&", "|", "^", "==", "!="}
_index = None
_flags = None


class Candidate:
    """A rewrite; its full source text is built only when it is compiled."""

    def __init__(self, edit, kind, note, safe):
        self._edit, self.kind, self.note, self.safe = edit, kind, note, safe
        self._text = None

    @property
    def key(self):
        return self._edit[1]

    @property
    def text(self):
        if self._text is None:
            source, edits = self._edit
            self._text = source.apply(edits)
        return self._text


def function_name(symbol):
    """C or C++ linker name to the source identifier."""
    if symbol.startswith("?"):
        match = re.match(r"\?+(\w+)", symbol)
        return match.group(1) if match else symbol
    return symbol.lstrip("_").split("@")[0]


def _parse(text, unit):
    global _index, _flags
    if _index is None:
        _index = ci.Index.create()
        _, overlay, flags = analysis_configuration()
        _flags = [*flags, "-ivfsoverlay", str(overlay)]
    name = str(ROOT / unit)
    args = [*_flags, *analysis_language(name)]
    return _index.parse(name, args=args, unsaved_files=[(name, text)]), name


class Source:
    def __init__(self, text, unit, function):
        self.text = text
        self.data = text.encode("latin-1")
        self.tu, self.name = _parse(text, unit)
        self.c = str(unit).endswith(".c")
        self._operators = {}
        self._children = {}
        self._walks = {}
        self.function = None
        # Definitions may sit inside extern "C" blocks or namespaces.
        pending = list(self.tu.cursor.get_children())
        while pending:
            cursor = pending.pop()
            if not self.mine(cursor):
                continue
            if cursor.kind in (K.LINKAGE_SPEC, K.NAMESPACE, K.UNEXPOSED_DECL):
                pending.extend(cursor.get_children())
            elif (cursor.kind in (K.FUNCTION_DECL, K.CXX_METHOD) and cursor.spelling == function
                    and cursor.is_definition()):
                self.function = cursor
        if self.function is None:
            raise ValueError(f"{function}: definition not found in unit")
        self.body = next(c for c in self.function.get_children() if c.kind == K.COMPOUND_STMT)
        self.returns_void = self.function.result_type.kind == ci.TypeKind.VOID

    def mine(self, cursor):
        start, end = cursor.extent.start, cursor.extent.end
        return (start.file is not None and start.file.name == self.name
                and end.file is not None and end.file.name == self.name)

    def span(self, cursor):
        return cursor.extent.start.offset, cursor.extent.end.offset

    def src(self, cursor):
        a, b = self.span(cursor)
        return self.data[a:b].decode("latin-1")

    def children(self, cursor):
        """Children inside this file, cached: libclang traversal dominates rewrite time."""
        key = (cursor.hash, cursor.kind, cursor.extent.start.offset, cursor.extent.end.offset)
        if key not in self._children:
            self._children[key] = [c for c in cursor.get_children() if self.mine(c)]
        return self._children[key]

    def walk(self, cursor=None):
        cursor = cursor or self.body
        key = (cursor.hash, cursor.kind, cursor.extent.start.offset, cursor.extent.end.offset)
        if key not in self._walks:
            nodes = []
            for child in self.children(cursor):
                nodes.append(child)
                nodes.extend(self.walk(child))
            self._walks[key] = nodes
        return self._walks[key]

    def edit(self, edits):
        """Describe a rewrite without materializing the whole file."""
        return (self, tuple(sorted(edits, key=lambda e: -e[0])))

    def apply(self, edits):
        data = self.data
        for a, b, replacement in edits:
            data = data[:a] + replacement.encode("latin-1") + data[b:]
        return data.decode("latin-1")

    def operator(self, cursor):
        """Spelling of a binary operator: first token after the left operand."""
        span = (cursor.extent.start.offset, cursor.extent.end.offset, cursor.kind)
        if span not in self._operators:
            self._operators[span] = self._operator(cursor)
        return self._operators[span]

    def _operator(self, cursor):
        children = list(cursor.get_children())
        if len(children) != 2:
            return None
        end = children[0].extent.end.offset
        # Skip whitespace and comments after the left operand; read the operator text.
        match = re.match(rb"(?:\s|/\*.*?\*/|//[^\n]*\n)*(<<=|>>=|&&|\|\||==|!=|<=|>=|<<|>>|\+=|-=|\*=|/=|%=|&=|\|=|\^=|[-+*/%<>&|^=,])",
                         self.data[end:end + 64], re.S)
        return match.group(1).decode() if match else None


def strip(cursor):
    while cursor.kind in (K.UNEXPOSED_EXPR, K.PAREN_EXPR) and len(list(cursor.get_children())) == 1:
        cursor = next(cursor.get_children())
    return cursor


def is_float(cursor):
    return cursor.type.get_canonical().kind in FLOATS


def pure(source, cursor):
    """No calls, assignments or increments in an expression."""
    for node in [cursor, *source.walk(cursor)]:
        if node.kind in (K.CALL_EXPR, K.COMPOUND_ASSIGNMENT_OPERATOR):
            return False
        if node.kind == K.BINARY_OPERATOR and source.operator(node) == "=":
            return False
        if node.kind == K.UNARY_OPERATOR:
            tokens = [t.spelling for t in node.get_tokens()]
            if "++" in tokens or "--" in tokens:
                return False
    return True


def paren(text):
    return text if re.fullmatch(r"[\w.\->\[\]]+", text) else f"({text})"


def negate(source, cond):
    inner = strip(cond)
    if inner.kind == K.BINARY_OPERATOR:
        op = source.operator(inner)
        children = list(inner.get_children())
        if op in NEGATE and not (is_float(children[0]) or is_float(children[1])):
            left, right = (source.src(c) for c in children)
            return f"{left} {NEGATE[op]} {right}"
    if inner.kind == K.UNARY_OPERATOR and source.src(inner).startswith("!"):
        return source.src(next(inner.get_children()))
    return f"!({source.src(cond)})"


def braced(text):
    text = text.strip()
    return text if text.startswith("{") else "{ " + text + " }"


def candidates(text, unit, function):
    source = Source(text, unit, function)
    out = []
    for rule in RULES:
        try:
            out.extend(rule(source))
        except (ValueError, StopIteration, AttributeError):
            continue
    seen, unique = set(), []
    for candidate in out:
        if candidate.key and candidate.key not in seen:
            seen.add(candidate.key)
            unique.append(candidate)
    return unique


def rule_invert_if(source):
    """if (c) A else B  ->  if (!c) B else A."""
    for node in source.walk():
        if node.kind != K.IF_STMT:
            continue
        parts = list(node.get_children())
        if len(parts) != 3 or any(p.kind == K.IF_STMT for p in parts[2:]):
            continue
        cond, then, other = parts
        a, b = source.span(node)
        replacement = f"if ({negate(source, cond)}) {braced(source.src(other))} else {braced(source.src(then))}"
        yield Candidate(source.edit([(a, b, replacement)]), "invert_if",
                        f"line {node.location.line}", True)


def _ends_with_return(source, stmt):
    if stmt.kind == K.RETURN_STMT and not list(stmt.get_children()):
        return True
    if stmt.kind == K.COMPOUND_STMT:
        children = list(stmt.get_children())
        return bool(children) and _ends_with_return(source, children[-1])
    return False


def _drop_final_return(source, stmt):
    text = source.src(stmt)
    return re.sub(r"\s*return\s*;\s*}\s*$", "\n}", text) if stmt.kind == K.COMPOUND_STMT else "{ }"


def rule_guard_to_else(source):
    """In a void body: if (c) { A; return; } rest  ->  if (c) { A } else { rest }, and reverse."""
    if not source.returns_void:
        return
    for compound in [source.body, *source.walk()]:
        if compound.kind != K.COMPOUND_STMT:
            continue
        # Only the function's tail position falls through to return.
        if compound != source.body:
            continue
        stmts = list(compound.get_children())
        for i, stmt in enumerate(stmts[:-1]):
            if stmt.kind != K.IF_STMT:
                continue
            parts = list(stmt.get_children())
            if len(parts) != 2 or not _ends_with_return(source, parts[1]):
                continue
            rest_start = source.span(stmts[i + 1])[0]
            rest_end = source.span(stmts[-1])[1]
            rest = source.data[rest_start:rest_end].decode("latin-1")
            a, _ = source.span(stmt)
            replacement = (f"if ({source.src(parts[0])}) {braced(_drop_final_return(source, parts[1]))} "
                           f"else {{\n{rest}\n}}")
            yield Candidate(source.edit([(a, rest_end, replacement)]), "guard_to_else",
                            f"line {stmt.location.line}", True)
        last = stmts[-1] if stmts else None
        if last is not None and last.kind == K.IF_STMT:
            parts = list(last.get_children())
            if len(parts) == 3 and parts[1].kind == K.COMPOUND_STMT and parts[2].kind == K.COMPOUND_STMT:
                then = source.src(parts[1]).rstrip()[:-1].rstrip() + "\n    return;\n}"
                other = source.src(parts[2]).strip()[1:-1]
                a, b = source.span(last)
                replacement = f"if ({source.src(parts[0])}) {then}\n{other}"
                yield Candidate(source.edit([(a, b, replacement)]), "else_to_guard",
                                f"line {last.location.line}", True)


def rule_compare(source):
    """Mirror comparisons and move integer-constant bounds between < and <=."""
    for node in source.walk():
        if node.kind != K.BINARY_OPERATOR:
            continue
        op = source.operator(node)
        if op not in MIRROR:
            continue
        left, right = list(node.get_children())
        a, b = source.span(node)
        ltext, rtext = source.src(left), source.src(right)
        yield Candidate(source.edit([(a, b, f"{rtext} {MIRROR[op]} {ltext}")]), "mirror_compare",
                        f"line {node.location.line}", True)
        constant = re.fullmatch(r"(-?)(0x[0-9a-fA-F]+|\d+)", rtext.strip())
        if constant and not is_float(left):
            value = int(constant.group(2), 0) * (-1 if constant.group(1) else 1)
            swaps = {"<=": ("<", value + 1), "<": ("<=", value - 1),
                     ">=": (">", value - 1), ">": (">=", value + 1)}
            if op in swaps:
                new_op, new_value = swaps[op]
                yield Candidate(source.edit([(a, b, f"{ltext} {new_op} {new_value}")]), "bound_compare",
                                f"line {node.location.line}", True)
            if op in ("==", "!="):
                # Valid only when a dominating guard fixes the range; otherwise exact-only.
                low = _guarded_lower_bound(source, node, ltext)
                for new in ((f"<", value + 1), (">", value - 1), ("<=", value), (">=", value)):
                    shown = new[0] if op == "==" else NEGATE[new[0]]
                    safe = low is not None and low == value and new[0] in ("<", "<=")
                    yield Candidate(source.edit([(a, b, f"{ltext} {shown} {new[1]}")]), "range_compare",
                                    f"line {node.location.line}", safe)


def _guard_bound(source, stmt, ltext):
    """Lower bound on ltext implied after `if (ltext OP c) return;` falls through."""
    if stmt.kind != K.IF_STMT:
        return None
    parts = list(stmt.get_children())
    if len(parts) != 2 or not _ends_with_return(source, parts[1]):
        return None
    cond = strip(parts[0])
    if cond.kind != K.BINARY_OPERATOR:
        return None
    left, right = list(cond.get_children())
    match = re.fullmatch(r"(0x[0-9a-fA-F]+|\d+)", source.src(right).strip())
    if source.src(left).strip() != ltext or not match:
        return None
    value = int(match.group(1), 0)
    return {"<": value, "<=": value + 1, "==": ("eq", value)}.get(source.operator(cond))


def _guarded_lower_bound(source, node, ltext):
    """Walk earlier early-return guards in the enclosing compound to bound ltext from below."""
    for compound in [source.body, *source.walk()]:
        if compound.kind != K.COMPOUND_STMT:
            continue
        stmts = list(compound.get_children())
        for i, stmt in enumerate(stmts):
            a, b = source.span(stmt)
            if not a <= node.extent.start.offset < b:
                continue
            if stmt.kind != K.IF_STMT or source.span(next(stmt.get_children()))[1] < node.extent.end.offset:
                continue
            low = None
            for earlier in stmts[:i]:
                bound = _guard_bound(source, earlier, ltext)
                if bound is None:
                    if any(n.kind == K.BINARY_OPERATOR and source.operator(n) == "=" and
                           source.src(list(n.get_children())[0]).strip() == ltext for n in source.walk(earlier)):
                        low = None
                    continue
                if isinstance(bound, tuple):
                    if low is not None and bound[1] == low:
                        low += 1
                else:
                    low = bound if low is None else max(low, bound)
            return low
    return None


def rule_commute(source):
    for node in source.walk():
        if node.kind != K.BINARY_OPERATOR or source.operator(node) not in COMMUTATIVE:
            continue
        left, right = list(node.get_children())
        if not (pure(source, left) and pure(source, right)):
            continue
        a, b = source.span(node)
        op = source.operator(node)
        yield Candidate(source.edit([(a, b, f"{paren(source.src(right))} {op} {paren(source.src(left))}")]),
                        "commute", f"line {node.location.line}", True)


def _references(source, decl, scope):
    return [n for n in source.walk(scope) if n.kind == K.DECL_REF_EXPR and n.referenced == decl]


def _assigned(source, ref):
    """True when a reference is written or has its address taken."""
    parent = ref.semantic_parent
    text_after = source.data[ref.extent.end.offset:ref.extent.end.offset + 3].decode("latin-1")
    before = source.data[max(0, ref.extent.start.offset - 2):ref.extent.start.offset].decode("latin-1")
    if re.match(r"\s*(=[^=]|\+=|-=|\*=|/=|\|=|&=|\^=|<<=|>>=|\+\+|--)", text_after):
        return True
    return before.endswith("&") and not before.endswith("&&") or before.endswith("++") or before.endswith("--")


def rule_inline_local(source):
    """Replace a once-assigned pure local with its initializer (undo clang CSE)."""
    for decl_stmt in source.walk():
        if decl_stmt.kind != K.DECL_STMT:
            continue
        decls = list(decl_stmt.get_children())
        if len(decls) != 1 or decls[0].kind != K.VAR_DECL:
            continue
        decl = decls[0]
        inits = [c for c in decl.get_children() if c.kind not in (K.TYPE_REF,)]
        if not inits:
            continue
        init = inits[-1]
        if not pure(source, init) or "[" in decl.type.spelling:
            continue
        refs = _references(source, decl, source.body)
        if not refs or any(_assigned(source, r) for r in refs):
            continue
        # Operand bases of the initializer must not be written before the last use.
        bases = {n.referenced for n in [init, *source.walk(init)] if n.kind == K.DECL_REF_EXPR}
        last = max(r.extent.end.offset for r in refs)
        start = decl_stmt.extent.end.offset
        written = False
        calls = False
        for node in source.walk():
            a = node.extent.start.offset
            if not start <= a <= last:
                continue
            if node.kind == K.DECL_REF_EXPR and node.referenced in bases and _assigned(source, node):
                written = True
            if node.kind == K.CALL_EXPR:
                calls = True
        if written:
            continue
        replacement = paren(source.src(init))
        if decl.type.spelling != init.type.spelling and not init.type.spelling.startswith(decl.type.spelling):
            replacement = f"(({decl.type.spelling}){paren(source.src(init))})"
        edits = [(*source.span(r), replacement) for r in refs]
        a, b = source.span(decl_stmt)
        edits.append((a, b, ""))
        yield Candidate(source.edit(edits), "inline_local", decl.spelling, not calls)


def rule_merge_assign(source):
    """x = E; x = F(x);  ->  x = F(E);  for a local x read once by F."""
    for compound in [source.body, *source.walk()]:
        if compound.kind != K.COMPOUND_STMT:
            continue
        stmts = list(compound.get_children())
        for first, second in zip(stmts, stmts[1:]):
            pair = []
            for stmt in (first, second):
                expr = strip(stmt)
                if expr.kind != K.BINARY_OPERATOR or source.operator(expr) != "=":
                    break
                pair.append(list(expr.get_children()))
            if len(pair) != 2:
                continue
            (lhs1, rhs1), (lhs2, rhs2) = pair
            lhs1, lhs2 = strip(lhs1), strip(lhs2)
            if (lhs1.kind != K.DECL_REF_EXPR or lhs2.kind != K.DECL_REF_EXPR
                    or lhs1.referenced != lhs2.referenced or lhs1.referenced.semantic_parent.kind == K.TRANSLATION_UNIT):
                continue
            uses = [n for n in source.walk(rhs2) if n.kind == K.DECL_REF_EXPR and n.referenced == lhs1.referenced]
            if strip(rhs2).kind == K.DECL_REF_EXPR or len(uses) != 1:
                continue
            ua, ub = source.span(uses[0])
            ra, rb = source.span(rhs2)
            rhs = source.data[ra:ua] + paren(source.src(rhs1)).encode("latin-1") + source.data[ub:rb]
            a, _ = source.span(first)
            _, b = source.span(second)
            replacement = f"{source.src(lhs1)} = {rhs.decode('latin-1')}"
            yield Candidate(source.edit([(a, b, replacement)]), "merge_assign",
                            f"line {first.location.line}", True)


def rule_swap_statements(source):
    """Swap adjacent statements: safe only for independent pure local assignments."""
    for compound in [source.body, *source.walk()]:
        if compound.kind != K.COMPOUND_STMT:
            continue
        stmts = list(compound.get_children())
        for first, second in zip(stmts, stmts[1:]):
            if K.DECL_STMT in (first.kind, second.kind) and source.c:
                continue
            if any(s.kind in (K.RETURN_STMT, K.BREAK_STMT, K.CONTINUE_STMT, K.LABEL_STMT,
                              K.CASE_STMT, K.DEFAULT_STMT) for s in (first, second)):
                continue
            a, _ = source.span(first)
            _, b = source.span(second)
            gap = source.data[source.span(first)[1]:source.span(second)[0]].decode("latin-1")
            first_text, second_text = source.src(first), source.src(second)
            if not gap.lstrip().startswith(";") and first.kind not in (K.IF_STMT, K.FOR_STMT, K.WHILE_STMT,
                                                                       K.DO_STMT, K.SWITCH_STMT, K.COMPOUND_STMT):
                continue
            first_full = first_text + (";" if gap.lstrip().startswith(";") else "")
            second_end = source.data[b:b + 1].decode("latin-1")
            second_full = second_text + (";" if second_end == ";" else "")
            if second_end == ";":
                b += 1
            replacement = second_full + "\n" + first_full
            yield Candidate(source.edit([(a, b, replacement)]), "swap_statements",
                            f"line {first.location.line}", _independent(source, first, second))


def _independent(source, first, second):
    written, read = [], []
    for stmt in (first, second):
        expr = strip(stmt)
        if expr.kind != K.BINARY_OPERATOR or source.operator(expr) != "=" or not pure(source, list(expr.get_children())[1]):
            return False
        lhs, rhs = list(expr.get_children())
        lhs = strip(lhs)
        if lhs.kind != K.DECL_REF_EXPR or lhs.referenced.semantic_parent.kind == K.TRANSLATION_UNIT:
            return False
        written.append(lhs.referenced)
        read.append({n.referenced for n in [rhs, *source.walk(rhs)] if n.kind == K.DECL_REF_EXPR})
    return written[0] != written[1] and written[0] not in read[1] and written[1] not in read[0]


def rule_compound_assign(source):
    """x = x op e  <->  x op= e."""
    for node in source.walk():
        if node.kind == K.BINARY_OPERATOR and source.operator(node) == "=":
            lhs, rhs = list(node.get_children())
            inner = strip(rhs)
            if inner.kind != K.BINARY_OPERATOR:
                continue
            op = source.operator(inner)
            if op not in ("+", "-", "*", "/", "|", "&", "^", "<<", ">>"):
                continue
            first, second = list(inner.get_children())
            if source.src(first).strip() != source.src(lhs).strip() or not pure(source, lhs):
                continue
            a, b = source.span(node)
            yield Candidate(source.edit([(a, b, f"{source.src(lhs)} {op}= {source.src(second)}")]),
                            "compound_assign", f"line {node.location.line}", True)
        elif node.kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            lhs, rhs = list(node.get_children())
            op = source.operator(node)
            if not op or not op.endswith("=") or not pure(source, lhs):
                continue
            a, b = source.span(node)
            yield Candidate(source.edit([(a, b, f"{source.src(lhs)} = {source.src(lhs)} {op[:-1]} {paren(source.src(rhs))}")]),
                            "expand_assign", f"line {node.location.line}", True)


def rule_index(source):
    """p[0] <-> *p."""
    for node in source.walk():
        if node.kind == K.ARRAY_SUBSCRIPT_EXPR:
            base, index = list(node.get_children())
            if source.src(index).strip() == "0":
                a, b = source.span(node)
                yield Candidate(source.edit([(a, b, f"*{paren(source.src(base))}")]), "index_deref",
                                f"line {node.location.line}", True)


def rule_ternary(source):
    """x = c ? a : b;  ->  if (c) x = a; else x = b;"""
    for stmt in source.walk():
        expr = strip(stmt)
        if stmt.semantic_parent is None or expr.kind != K.BINARY_OPERATOR or source.operator(expr) != "=":
            continue
        if stmt.kind != K.BINARY_OPERATOR:
            continue
        lhs, rhs = list(expr.get_children())
        cond = strip(rhs)
        if cond.kind != K.CONDITIONAL_OPERATOR or not pure(source, lhs):
            continue
        c, x, y = list(cond.get_children())
        a, b = source.span(stmt)
        if source.data[b:b + 1] != b";":
            continue
        replacement = (f"if ({source.src(c)}) {{ {source.src(lhs)} = {source.src(x)}; }} "
                       f"else {{ {source.src(lhs)} = {source.src(y)}; }}")
        yield Candidate(source.edit([(a, b + 1, replacement)]), "ternary_to_if",
                        f"line {stmt.location.line}", True)


def _with_semicolon(source, stmt):
    """Statement text including its terminating semicolon, or None if absent."""
    text = source.src(stmt)
    if text.rstrip().endswith(";"):
        return text
    end = source.span(stmt)[1]
    return text + ";" if source.data[end:end + 1] == b";" else None


def _decl_names(source, stmt):
    return [d for d in stmt.get_children() if d.kind == K.VAR_DECL]


def _initializer_refs(source, decl):
    return {n.referenced for n in source.walk(decl) if n.kind == K.DECL_REF_EXPR}


def rule_swap_decls(source):
    """Swap adjacent declarations: VC8 assigns stack slots in declaration order."""
    for compound in [source.body, *source.walk()]:
        if compound.kind != K.COMPOUND_STMT:
            continue
        stmts = list(compound.get_children())
        for first, second in zip(stmts, stmts[1:]):
            if first.kind != K.DECL_STMT or second.kind != K.DECL_STMT:
                continue
            a_vars, b_vars = _decl_names(source, first), _decl_names(source, second)
            if not a_vars or not b_vars:
                continue
            if any(set(a_vars) & _initializer_refs(source, d) for d in b_vars):
                continue  # the second initializer reads the first variable
            a, _ = source.span(first)
            _, b = source.span(second)
            first_text, second_text = _with_semicolon(source, first), _with_semicolon(source, second)
            if first_text is None or second_text is None:
                continue
            b = source.span(second)[1] + (0 if source.src(second).rstrip().endswith(";") else 1)
            replacement = second_text + "\n" + first_text
            yield Candidate(source.edit([(a, b, replacement)]), "swap_decls",
                            f"{a_vars[0].spelling}/{b_vars[0].spelling}", True)


def _blocks(source):
    return [source.body, *(c for c in source.walk() if c.kind == K.COMPOUND_STMT)]


def _first_statement_offset(source, compound):
    """Offset just after the opening brace of a compound statement."""
    return source.span(compound)[0] + 1


def rule_move_decls(source):
    """Sink a top-level declaration into the innermost block using it, or hoist one up."""
    names = [d.spelling for d in source.walk() if d.kind == K.VAR_DECL]
    blocks = _blocks(source)
    for stmt in source.walk():
        if stmt.kind != K.DECL_STMT:
            continue
        decls = _decl_names(source, stmt)
        if len(decls) != 1 or [c for c in decls[0].get_children() if c.kind != K.TYPE_REF]:
            continue  # only plain declarations without initializers move freely
        decl = decls[0]
        text = _with_semicolon(source, stmt)
        if text is None:
            continue
        a, b = source.span(stmt)
        b = b if source.src(stmt).rstrip().endswith(";") else b + 1
        refs = _references(source, decl, source.body)
        if not refs:
            continue
        home = min((c for c in blocks if source.span(c)[0] < a < source.span(c)[1]),
                   key=lambda c: source.span(c)[1] - source.span(c)[0])
        if home == source.body:
            inner = [c for c in blocks if c != source.body
                     and all(source.span(c)[0] < r.extent.start.offset < source.span(c)[1] for r in refs)]
            if inner:
                target = min(inner, key=lambda c: source.span(c)[1] - source.span(c)[0])
                at = _first_statement_offset(source, target)
                yield Candidate(source.edit([(a, b, ""), (at, at, "\n" + text)]), "sink_decl",
                                decl.spelling, True)
        elif names.count(decl.spelling) == 1:
            at = _first_statement_offset(source, source.body)
            yield Candidate(source.edit([(a, b, ""), (at, at, "\n" + text)]), "hoist_decl",
                            decl.spelling, True)


def rule_drop_cast(source):
    """(f32)int_expr inside float arithmetic converts implicitly; VC8 may then use FIADD/FIMUL."""
    float_operands = {}
    for node in source.walk():
        if node.kind == K.BINARY_OPERATOR and source.operator(node) in ("+", "-", "*", "/"):
            operands = [strip(c) for c in node.get_children()]
            if len(operands) == 2:
                for mine, other in ((operands[0], operands[1]), (operands[1], operands[0])):
                    if is_float(other):
                        float_operands[source.span(mine)] = True
    for node in source.walk():
        if node.kind != K.CSTYLE_CAST_EXPR or not is_float(node) or source.span(node) not in float_operands:
            continue
        inner = [c for c in node.get_children() if c.kind != K.TYPE_REF]
        if len(inner) != 1 or is_float(inner[0]) or inner[0].type.get_canonical().kind == ci.TypeKind.POINTER:
            continue
        a, b = source.span(node)
        yield Candidate(source.edit([(a, b, paren(source.src(inner[0])))]), "drop_cast",
                        f"line {node.location.line}", True)


def _tail_returns(source, stmt):
    """Return statements ending every path of a switch case list or if/else chain, or None."""
    if stmt.kind == K.RETURN_STMT:
        return [stmt] if list(stmt.get_children()) else None
    if stmt.kind == K.COMPOUND_STMT:
        children = source.children(stmt)
        return _tail_returns(source, children[-1]) if children else None
    if stmt.kind == K.IF_STMT:
        parts = source.children(stmt)
        if len(parts) != 3:
            return None
        then, other = _tail_returns(source, parts[1]), _tail_returns(source, parts[2])
        return then + other if then is not None and other is not None else None
    return None


def rule_result_variable(source):
    """Turn returns in switch cases or an if/else chain into one result variable and return."""
    if source.returns_void:
        return
    stmts = source.children(source.body)
    if len(stmts) < 2 or stmts[-1].kind != K.RETURN_STMT or not source.children(stmts[-1]):
        return
    final = stmts[-1]
    default = source.src(source.children(final)[0])
    result_type = source.function.result_type.spelling
    names = {d.spelling for d in source.walk() if d.kind == K.VAR_DECL}
    name = next(n for n in ("result", "value", "ret", "res") if n not in names)
    body_start = source.span(source.body)[0] + 1
    for stmt in stmts[:-1]:
        edits = []
        if stmt.kind == K.SWITCH_STMT:
            compound = source.children(stmt)[-1]
            cases = source.children(compound)
            # Returns that end a case: the statement right before the next case label.
            for node in source.walk(compound):
                if node.kind != K.RETURN_STMT or not source.children(node):
                    continue
                inside = [c for c in source.walk(compound)
                          if c.kind in (K.FOR_STMT, K.WHILE_STMT, K.DO_STMT, K.SWITCH_STMT)
                          and source.span(c)[0] < source.span(node)[0] < source.span(c)[1]]
                if inside:
                    edits = None
                    break
                a, b = source.span(node)
                semicolon = 0 if source.src(node).rstrip().endswith(";") else 1
                expr = source.src(source.children(node)[0])
                edits.append((a, b + semicolon, f"{name} = {expr};\nbreak;"))
            if not edits:
                continue
        elif stmt.kind == K.IF_STMT:
            returns = _tail_returns(source, stmt)
            if not returns:
                continue
            for node in returns:
                a, b = source.span(node)
                semicolon = 0 if source.src(node).rstrip().endswith(";") else 1
                edits.append((a, b + semicolon, f"{name} = {source.src(source.children(node)[0])};"))
        else:
            continue
        fa, fb = source.span(final)
        semicolon = 0 if source.src(final).rstrip().endswith(";") else 1
        for initialized in (True, False):
            if initialized:
                extra = [(body_start, body_start, f"\n{result_type} {name} = {default};"),
                         (fa, fb + semicolon, f"return {name};")]
            else:
                extra = [(body_start, body_start, f"\n{result_type} {name};"),
                         (source.span(stmt)[0], source.span(stmt)[0], f"{name} = {default};\n"),
                         (fa, fb + semicolon, f"return {name};")]
            yield Candidate(source.edit(edits + extra), "result_variable",
                            f"line {stmt.location.line}{' init' if initialized else ''}", True)


def rule_forward_temp(source):
    """x = E; y = F(x);  ->  y = F(E);  when the local x is dead afterwards."""
    loops = [c for c in source.walk() if c.kind in (K.FOR_STMT, K.WHILE_STMT, K.DO_STMT)]
    for compound in _blocks(source):
        stmts = source.children(compound)
        for first, second in zip(stmts, stmts[1:]):
            one, two = strip(first), strip(second)
            if (one.kind != K.BINARY_OPERATOR or two.kind != K.BINARY_OPERATOR
                    or source.operator(one) != "=" or source.operator(two) != "="):
                continue
            lhs, rhs = source.children(one)
            lhs = strip(lhs)
            if lhs.kind != K.DECL_REF_EXPR or lhs.referenced.semantic_parent.kind == K.TRANSLATION_UNIT:
                continue
            variable = lhs.referenced
            target, value = source.children(two)
            uses = [n for n in [value, *source.walk(value)] if n.kind == K.DECL_REF_EXPR and n.referenced == variable]
            if len(uses) != 1 or any(n.kind == K.DECL_REF_EXPR and n.referenced == variable
                                     for n in [target, *source.walk(target)]):
                continue
            if any(source.span(l)[0] < source.span(first)[0] < source.span(l)[1] for l in loops):
                continue
            end = source.span(second)[1]
            later = sorted((n for n in source.walk() if n.kind == K.DECL_REF_EXPR and n.referenced == variable
                            and n.extent.start.offset > end), key=lambda n: n.extent.start.offset)
            if later and not _assigned(source, later[0]):
                continue
            if not (pure(source, rhs) or pure(source, value)):
                continue
            ua, ub = source.span(uses[0])
            va, vb = source.span(value)
            new_value = (source.data[va:ua] + paren(source.src(rhs)).encode("latin-1")
                         + source.data[ub:vb]).decode("latin-1")
            a, _ = source.span(first)
            b = end + (0 if source.src(second).rstrip().endswith(";") else 1)
            yield Candidate(source.edit([(a, b, f"{source.src(target)} = {new_value};")]), "forward_temp",
                            f"line {first.location.line}", True)


RULES = [rule_inline_local, rule_invert_if, rule_guard_to_else, rule_compare, rule_merge_assign,
         rule_commute, rule_swap_statements, rule_compound_assign, rule_index, rule_ternary,
         rule_swap_decls, rule_move_decls, rule_drop_cast, rule_result_variable, rule_forward_temp]
