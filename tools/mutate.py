"""Random source mutations of one function for tools/permute2.py, located with libclang.

Every mutation is a `Site`: a set of text edits on the current unit text plus
three labels:

- `kind`: the rule that made it;
- `safe`: the rewrite preserves C semantics (False: "probably equivalent", a
  human must check the result, e.g. float reassociation or a narrowed type);
- `natural`: the result is ordinary source a programmer would write. Unnatural
  mutations (dummy locals, `register`, dead initialisers, extra blocks, Yoda
  comparisons) exist only to measure what the compiler is sensitive to; their
  results are never committed.

The rules aim at the unexplained patterns of docs/vc8-idioms.md: declaration
order and local/parameter copies (register choice, U3), temporaries and
loads (stack slots, U7; known-register reuse, U8), operand order and
association (x87 order, U9), statement order, loop and guard forms (tail
merging, U13-U15), and integer widths/signedness (U17). The rule set of
tools/rewrite.py is included as well.
"""
import random
import re

try:
    import clang.cindex as ci
except ImportError as error:  # Only the permuter needs libclang.
    raise SystemExit("tools/mutate.py needs libclang: python3 -m pip install libclang") from error

from tools import rewrite

K = ci.CursorKind
LOOPS = (K.FOR_STMT, K.WHILE_STMT, K.DO_STMT)
INT_TYPES = ["i32", "u32", "i16", "u16", "i8", "u8", "int", "unsigned int", "short", "unsigned short",
             "char", "unsigned char", "signed char", "long", "unsigned long"]
SIGN_FLIP = {"i32": "u32", "u32": "i32", "i16": "u16", "u16": "i16", "i8": "u8", "u8": "i8",
             "int": "unsigned int", "unsigned int": "int", "short": "unsigned short",
             "unsigned short": "short", "char": "unsigned char", "unsigned char": "char",
             "long": "unsigned long", "unsigned long": "long", "signed char": "unsigned char"}
WIDTHS = {"i32": ["i16", "i8"], "u32": ["u16", "u8"], "i16": ["i32", "i8"], "u16": ["u32", "u8"],
          "i8": ["i32", "i16"], "u8": ["u32", "u16"], "int": ["short", "char"],
          "unsigned int": ["unsigned short", "unsigned char"], "short": ["int"],
          "unsigned short": ["unsigned int"], "char": ["int"], "unsigned char": ["unsigned int"]}
FLOATS = rewrite.FLOATS
# rewrite.py rules and their labels (natural, and whether their own `safe` flag is kept).
REWRITE_RULES = {
    "inline_local": True, "invert_if": True, "guard_to_else": True, "else_to_guard": True,
    "mirror_compare": True, "bound_compare": True, "range_compare": True, "merge_assign": True,
    "commute": True, "swap_statements": True, "compound_assign": True, "expand_assign": True,
    "index_deref": True, "ternary_to_if": True, "swap_decls": True, "sink_decl": True,
    "hoist_decl": True, "drop_cast": True, "result_variable": True, "forward_temp": True,
    "signedness": True, "chain_swap": True,
}
UNNATURAL = {"dummy_local", "register_local", "dead_init", "extra_block", "yoda_compare"}
# Not natural, but allowed with a `// MATCH-EXCEPTION:` comment (searched by default).
EXCEPTION_KINDS = {"inline_cast"}


class Site:
    """One mutation: non-overlapping (start, end, replacement) edits on the unit text."""

    __slots__ = ("edits", "kind", "note", "safe", "natural")

    def __init__(self, edits, kind, note="", safe=True, natural=True):
        self.edits = tuple(sorted(edits, key=lambda e: (e[0], e[1])))
        self.kind, self.note, self.safe, self.natural = kind, note, safe, natural

    def __repr__(self):
        return f"Site({self.kind} {self.note} safe={self.safe} natural={self.natural})"

    def overlaps(self, other):
        for a, b, _ in self.edits:
            for c, d, _ in other.edits:
                if a < d and c < b or a == c or (a == b and c < a < d) or (c == d and a < c < b):
                    return True
        return False


def apply(text, sites):
    """Apply non-overlapping sites to text (byte offsets in latin-1)."""
    data = text.encode("latin-1")
    edits = sorted((e for s in sites for e in s.edits), key=lambda e: (-e[0], -e[1]))
    for left, right in zip(edits, edits[1:]):
        if right[1] > left[0] or (right[0] == left[0] and right[1] == left[1]):
            raise ValueError("overlapping edits")
    for a, b, replacement in edits:
        data = data[:a] + replacement.encode("latin-1") + data[b:]
    return data.decode("latin-1")


class Parser:
    """A libclang translation unit that is re-parsed in place (precompiled preamble)."""

    def __init__(self, path, args):
        self.path, self.args = str(path), list(args)
        self.index = ci.Index.create()
        self.tu = None

    def parse(self, text):
        unsaved = [(self.path, text)]
        if self.tu is None:
            try:
                self.tu = self.index.parse(self.path, args=self.args, unsaved_files=unsaved,
                                           options=ci.TranslationUnit.PARSE_PRECOMPILED_PREAMBLE)
            except ci.TranslationUnitLoadError:
                # A preamble file can collide with a concurrent parse of the same unit.
                self.tu = self.index.parse(self.path, args=self.args, unsaved_files=unsaved)
        else:
            self.tu.reparse(unsaved_files=unsaved)
        return self.tu


class Function(rewrite.Source):
    """rewrite.Source over an already parsed translation unit, plus parent links."""

    def __init__(self, tu, text, path, function, location=None):  # pylint: disable=super-init-not-called
        self.text = text
        self.data = text.encode("latin-1")
        self.tu = tu
        self.name = str(path)
        self.c = self.name.endswith(".c")
        self._operators, self._children, self._walks = {}, {}, {}
        self.function = None
        if location is not None:
            # Mutations never move the function's name: find it without walking the headers.
            cursor = ci.Cursor.from_location(tu, tu.get_location(self.name, location))
            if cursor is not None and cursor.kind in (K.FUNCTION_DECL, K.CXX_METHOD) and \
                    cursor.spelling == function and cursor.is_definition():
                self.function = cursor
        pending = [] if self.function is not None else list(tu.cursor.get_children())
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
            raise ValueError(f"{function}: definition not found")
        self.body = next(c for c in self.function.get_children() if c.kind == K.COMPOUND_STMT)
        self.location = (self.function.location.line, self.function.location.column)
        self.returns_void = self.function.result_type.kind == ci.TypeKind.VOID
        self.params = [c for c in self.function.get_children() if c.kind == K.PARM_DECL]
        self.parent = {}
        self._link(self.body)
        self.nodes = self.walk()
        self.refs = {}
        for node in self.nodes:
            if node.kind == K.DECL_REF_EXPR and node.referenced is not None:
                self.refs.setdefault(self.key(node.referenced), []).append(node)
        self.locals = [n for n in self.nodes if n.kind == K.VAR_DECL]
        self.names = {n.spelling for n in self.locals} | {p.spelling for p in self.params}
        self.has_goto = any(n.kind in (K.GOTO_STMT, K.LABEL_STMT) for n in self.nodes)

    @staticmethod
    def key(cursor):
        return (cursor.hash, cursor.kind.value, cursor.extent.start.offset, cursor.extent.end.offset)

    def _link(self, cursor):
        for child in self.children(cursor):
            if self.key(child) != self.key(cursor):
                self.parent[self.key(child)] = cursor
            self._link(child)

    def up(self, cursor):
        return self.parent.get(self.key(cursor))

    def uses(self, decl):
        return self.refs.get(self.key(decl), [])

    def fresh(self, base):
        name, n = base, 2
        while name in self.names or re.search(rf"\b{name}\b", self.text) and name != base:
            name = f"{base}{n}"
            n += 1
        if name in self.names:
            name = f"{base}{n}"
        self.names.add(name)
        return name

    def full(self, stmt):
        """Span of a statement including a trailing semicolon."""
        a, b = self.span(stmt)
        tail = self.data[b:b + 40]
        match = re.match(rb"[ \t]*;", tail)
        if match and not self.data[a:b].rstrip().endswith(b";") and not self.data[a:b].rstrip().endswith(b"}"):
            b += match.end()
        return a, b

    def full_src(self, stmt):
        a, b = self.full(stmt)
        return self.data[a:b].decode("latin-1")

    def decl_run(self, compound):
        stmts = self.children(compound)
        run = []
        for s in stmts:
            if s.kind != K.DECL_STMT:
                break
            run.append(s)
        return run, stmts[len(run):]

    def decl_insert_point(self, compound):
        """Offset where a new declaration goes: after the last leading declaration."""
        run, _ = self.decl_run(compound)
        if run:
            return self.full(run[-1])[1]
        return self.span(compound)[0] + 1

    def assigned(self, ref):
        return rewrite._assigned(self, ref)

    def address_taken(self, decl):
        for ref in self.uses(decl):
            a = ref.extent.start.offset
            before = self.data[max(0, a - 3):a].decode("latin-1").rstrip()
            if before.endswith("&") and not before.endswith("&&"):
                return True
        return False

    def top_index(self, node, stmts):
        """Index of the statement in `stmts` that contains `node`, or None."""
        a = node.extent.start.offset
        for i, s in enumerate(stmts):
            sa, sb = self.full(s)
            if sa <= a < sb:
                return i
        return None

    def enclosing(self, node, kinds):
        cursor = self.up(node)
        while cursor is not None:
            if cursor.kind in kinds:
                return cursor
            cursor = self.up(cursor)
        return None

    def type_text(self, cursor):
        spelling = cursor.type.spelling
        if "(" in spelling or "[" in spelling or "<" in spelling:
            return None
        return spelling


def _decl_text(func, decl_stmt):
    return func.full_src(decl_stmt).strip()


def _strip_ws(text):
    return text.strip()


# --------------------------------------------------------------------------- rules

def rule_decl_order(func):
    """Move a declaration to another position among its block's leading declarations."""
    for compound in [func.body, *(n for n in func.nodes if n.kind == K.COMPOUND_STMT)]:
        run, _ = func.decl_run(compound)
        if len(run) < 2:
            continue
        names = [[d.spelling for d in s.get_children() if d.kind == K.VAR_DECL] for s in run]
        reads = []
        for s in run:
            reads.append({n.spelling for n in func.walk(s) if n.kind == K.DECL_REF_EXPR})
        texts = [_decl_text(func, s) for s in run]
        a, b = func.full(run[0])[0], func.full(run[-1])[1]
        for i in range(len(run)):
            for j in range(len(run)):
                if i == j:
                    continue
                order = list(range(len(run)))
                order.insert(j, order.pop(i))
                declared, ok = set(), True
                for k in order:
                    if reads[k] & (set(sum(names, [])) - declared - set(names[k])):
                        ok = False
                        break
                    declared |= set(names[k])
                if not ok:
                    continue
                indent = _indent(func, run[0])
                replacement = ("\n" + indent).join(texts[k] for k in order)
                yield Site([(a, b, replacement)], "decl_order", f"{names[i][0]}->{j}")


def _indent(func, node):
    a = node.extent.start.offset
    line_start = func.data.rfind(b"\n", 0, a) + 1
    prefix = func.data[line_start:a].decode("latin-1")
    return prefix if not prefix.strip() else "    "


def rule_split_multi_decl(func):
    """int a, b;  ->  int a; int b;"""
    for stmt in func.nodes:
        if stmt.kind != K.DECL_STMT:
            continue
        decls = [d for d in func.children(stmt) if d.kind == K.VAR_DECL]
        if len(decls) < 2:
            continue
        parts = []
        for d in decls:
            t = func.type_text(d)
            if t is None:
                parts = None
                break
            inits = [c for c in func.children(d) if c.kind not in (K.TYPE_REF,)]
            init = f" = {func.src(inits[-1])}" if inits else ""
            parts.append(f"{_declare(t, d.spelling)}{init};")
        if parts:
            a, b = func.full(stmt)
            yield Site([(a, b, ("\n" + _indent(func, stmt)).join(parts))], "split_multi_decl",
                       decls[0].spelling)


def _declare(type_text, name):
    if type_text.endswith("*"):
        return f"{type_text}{name}"
    return f"{type_text} {name}"


def rule_copy_param(func):
    """Copy a parameter into a local at the top and use the local (U3 lever)."""
    if func.has_goto:
        return
    run, _ = func.decl_run(func.body)
    for p in func.params:
        uses = func.uses(p)
        t = func.type_text(p)
        if not uses or t is None or func.address_taken(p):
            continue
        name = func.fresh(p.spelling + "_local" if len(p.spelling) > 2 else p.spelling + "2")
        edits = [(*func.span(r), name) for r in uses]
        indent = _indent(func, run[0]) if run else "    "
        decl = f"{_declare(t, name)} = {p.spelling};"
        points = [func.span(func.body)[0] + 1] + [func.full(s)[1] for s in run]
        for k, at in enumerate(points):
            yield Site(edits + [(at, at, "\n" + indent + decl)], "copy_param", f"{p.spelling}@{k}")


def _top_partition(func, first, second, stmts):
    """Index i such that all uses of `first` lie in stmts[:i] (or the RHS of stmts[i]) and all
    uses of `second` in stmts[i:], with stmts[i] = `second = E;` fully assigning `second`."""
    f_idx = [func.top_index(r, stmts) for r in func.uses(first)]
    s_idx = [func.top_index(r, stmts) for r in func.uses(second)]
    if not s_idx or None in f_idx or None in s_idx:
        return None
    i = min(s_idx)
    stmt = rewrite.strip(stmts[i])
    if stmt.kind != K.BINARY_OPERATOR or func.operator(stmt) != "=":
        return None
    lhs, rhs = func.children(stmt)
    lhs = rewrite.strip(lhs)
    if lhs.kind != K.DECL_REF_EXPR or func.key(lhs.referenced) != func.key(second):
        return None
    if any(func.key(n.referenced) == func.key(second) for n in func.walk(rhs) if n.kind == K.DECL_REF_EXPR):
        return None
    rhs_span = func.span(rhs)
    for r, k in zip(func.uses(first), f_idx):
        if k > i or (k == i and not rhs_span[0] <= r.extent.start.offset < rhs_span[1]):
            return None
    return i


def _plain_local(func, decl, top_run):
    """A top-level local without initializer, address not taken."""
    stmt = func.up(decl)
    if stmt is None or stmt not in top_run or func.address_taken(decl):
        return False
    if [c for c in func.children(decl) if c.kind != K.TYPE_REF]:
        return False
    return len([d for d in func.children(stmt) if d.kind == K.VAR_DECL]) == 1


def rule_reuse_variable(func):
    """Reuse a dead parameter or local for a later local of the same type (and remove it)."""
    if func.has_goto:
        return
    run, stmts = func.decl_run(func.body)
    candidates = [d for d in func.locals if _plain_local(func, d, run)]
    sources = [p for p in func.params if not func.address_taken(p)] + candidates
    for second in candidates:
        for first in sources:
            if first == second or first.type.get_canonical().spelling != second.type.get_canonical().spelling:
                continue
            if not func.uses(first):
                continue
            i = _top_partition(func, first, second, stmts)
            if i is None:
                continue
            edits = [(*func.span(r), first.spelling) for r in func.uses(second)]
            a, b = func.full(func.up(second))
            edits.append((a, b, ""))
            kind = "reuse_param" if first.kind == K.PARM_DECL else "merge_locals"
            yield Site(edits, kind, f"{second.spelling}->{first.spelling}")


def rule_split_local(func):
    """v = E1; ...v...; v = E2; ...v...  ->  a second local for the later live range."""
    if func.has_goto:
        return
    run, stmts = func.decl_run(func.body)
    for v in func.locals:
        if not _plain_local(func, v, run):
            continue
        t = func.type_text(v)
        if t is None:
            continue
        uses = func.uses(v)
        idx = [func.top_index(r, stmts) for r in uses]
        if None in idx:
            continue
        for i in sorted(set(idx)):
            if i == min(idx):
                continue
            stmt = rewrite.strip(stmts[i])
            if stmt.kind != K.BINARY_OPERATOR or func.operator(stmt) != "=":
                continue
            lhs, rhs = func.children(stmt)
            lhs = rewrite.strip(lhs)
            if lhs.kind != K.DECL_REF_EXPR or func.key(lhs.referenced) != func.key(v):
                continue
            # Uses at statement i other than the LHS would read the old value.
            if any(k == i and r.extent.start.offset != lhs.extent.start.offset for r, k in zip(uses, idx)):
                continue
            name = func.fresh(v.spelling + "2")
            edits = [(*func.span(r), name) for r, k in zip(uses, idx) if k >= i]
            at = func.full(func.up(v))[1]
            edits.append((at, at, "\n" + _indent(func, func.up(v)) + _declare(t, name) + ";"))
            yield Site(edits, "split_local", f"{v.spelling}@{i}")


def _evaluated_once(func, node, stmt):
    """True when `node` is evaluated exactly once, unconditionally, when `stmt` runs."""
    child, cursor = node, func.up(node)
    while cursor is not None and func.key(child) != func.key(stmt):
        if cursor.kind in LOOPS or cursor.kind == K.COMPOUND_STMT:
            return False
        if cursor.kind == K.IF_STMT or cursor.kind == K.SWITCH_STMT:
            if func.key(func.children(cursor)[0]) != func.key(child):
                return False
        if cursor.kind == K.CONDITIONAL_OPERATOR and func.key(func.children(cursor)[0]) != func.key(child):
            return False
        if cursor.kind == K.BINARY_OPERATOR and func.operator(cursor) in ("&&", "||"):
            if func.key(func.children(cursor)[0]) != func.key(child):
                return False
        child, cursor = cursor, func.up(cursor)
    return cursor is not None or func.key(child) == func.key(stmt)


def _statement_of(func, node):
    """(statement, compound) where the statement is a direct child of a compound."""
    child, cursor = node, func.up(node)
    while cursor is not None:
        if cursor.kind == K.COMPOUND_STMT:
            return child, cursor
        child, cursor = cursor, func.up(cursor)
    return None, None


def _rvalue(func, node):
    parent = func.up(node)
    if parent is None:
        return False
    if parent.kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR):
        op = func.operator(parent)
        if op and op.endswith("=") and op not in ("==", "!=", "<=", ">=") and \
                func.key(func.children(parent)[0]) == func.key(node):
            return False
    if parent.kind == K.UNARY_OPERATOR:
        text = func.src(parent)
        if text.startswith(("&", "++", "--")) or text.endswith(("++", "--")):
            return False
    if parent.kind == K.MEMBER_REF_EXPR and not func.src(parent)[len(func.src(node)):].lstrip().startswith("->"):
        return False  # x.f: x itself is an object, not a loaded value
    if parent.kind == K.ARRAY_SUBSCRIPT_EXPR and func.key(func.children(parent)[0]) == func.key(node) \
            and node.type.get_canonical().kind == ci.TypeKind.CONSTANTARRAY:
        return False
    return True


TEMP_KINDS = (K.MEMBER_REF_EXPR, K.ARRAY_SUBSCRIPT_EXPR, K.BINARY_OPERATOR, K.CSTYLE_CAST_EXPR,
              K.DECL_REF_EXPR, K.CALL_EXPR)


def _temp_type(func, node):
    t = node.type
    canon = t.get_canonical()
    if canon.kind in (ci.TypeKind.VOID, ci.TypeKind.RECORD, ci.TypeKind.CONSTANTARRAY,
                      ci.TypeKind.INCOMPLETEARRAY, ci.TypeKind.FUNCTIONPROTO, ci.TypeKind.ELABORATED):
        return None
    spelling = t.spelling
    if "(" in spelling or "[" in spelling or "<" in spelling or not spelling:
        return None
    if spelling.startswith("const ") and canon.kind != ci.TypeKind.POINTER:
        spelling = spelling[6:]
    return spelling


def rule_temp_expr(func):
    """Introduce a temporary for a subexpression: `t = E;` before its statement."""
    for node in func.nodes:
        if node.kind not in TEMP_KINDS:
            continue
        if node.kind == K.DECL_REF_EXPR and (node.referenced is None or node.referenced.kind != K.VAR_DECL
                                             or node.referenced.semantic_parent is None
                                             or node.referenced.semantic_parent.kind != K.TRANSLATION_UNIT):
            continue  # only globals: locals are already temporaries
        if node.kind == K.BINARY_OPERATOR and func.operator(node) not in ("+", "-", "*", "/", "&", "|", "^",
                                                                         "<<", ">>"):
            continue
        t = _temp_type(func, node)
        if t is None or not _rvalue(func, node):
            continue
        stmt, compound = _statement_of(func, node)
        if stmt is None or stmt.kind in (K.DECL_STMT, K.LABEL_STMT, K.CASE_STMT, K.DEFAULT_STMT) or \
                stmt.kind in LOOPS or not _evaluated_once(func, node, stmt):
            continue
        if node.kind == K.CALL_EXPR and stmt.kind == K.CALL_EXPR:
            continue
        calls = [n for n in func.walk(stmt) if n.kind == K.CALL_EXPR]
        inner = func.span(node)
        others = [c for c in calls if not inner[0] <= c.extent.start.offset < inner[1]
                  and not func.span(c)[0] <= inner[0] < func.span(c)[1]]
        safe = rewrite.pure(func, node) and not others or node.kind == K.CALL_EXPR and not others
        name = func.fresh("temp")
        at = func.decl_insert_point(compound)
        indent = _indent(func, stmt)
        decl = f"\n{indent}{_declare(t, name)};"
        a = func.full(stmt)[0]
        edits = [(at, at, decl), (a, a, f"{name} = {func.src(node)};\n{indent}"), (*inner, name)]
        if at == a:
            edits = [(a, a, decl.lstrip("\n") + f"\n{indent}{name} = {func.src(node)};\n{indent}"),
                     (*inner, name)]
        try:
            yield Site(edits, "temp_expr", f"line {node.location.line} {func.src(node)[:30]}", safe)
        except ValueError:
            continue


def rule_hoist_load(func):
    """Load a repeated field/global/subscript once into a local for a run of statements."""
    for compound in [func.body, *(n for n in func.nodes if n.kind == K.COMPOUND_STMT)]:
        _, stmts = func.decl_run(compound)
        groups = {}
        for i, stmt in enumerate(stmts):
            for node in [stmt, *func.walk(stmt)]:
                if node.kind not in (K.MEMBER_REF_EXPR, K.ARRAY_SUBSCRIPT_EXPR, K.DECL_REF_EXPR):
                    continue
                if node.kind == K.DECL_REF_EXPR and (node.referenced is None
                                                     or node.referenced.kind != K.VAR_DECL
                                                     or node.referenced.semantic_parent.kind != K.TRANSLATION_UNIT):
                    continue
                if _temp_type(func, node) is None or not _rvalue(func, node) or not rewrite.pure(func, node):
                    continue
                text = re.sub(r"\s+", "", func.src(node))
                groups.setdefault(text, []).append((i, node))
        for text, occurrences in groups.items():
            if len(occurrences) < 2:
                continue
            first = min(i for i, _ in occurrences)
            last = max(i for i, _ in occurrences)
            # The value must not be stored to inside the range.
            stored = calls = False
            for stmt in stmts[first:last + 1]:
                for n in [stmt, *func.walk(stmt)]:
                    if n.kind == K.CALL_EXPR:
                        calls = True
                    if n.kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR) and \
                            (func.operator(n) or "").endswith("=") and func.operator(n) not in ("==", "!=", "<=", ">="):
                        lhs = re.sub(r"\s+", "", func.src(func.children(n)[0]))
                        if lhs == text or text.startswith(lhs):
                            stored = True
                    if n.kind == K.UNARY_OPERATOR and re.sub(r"\s+", "", func.src(n)).strip("+-") == text:
                        stored = True
            if stored or stmts[first].kind in LOOPS and False:
                continue
            node = occurrences[0][1]
            # Nested occurrences (a->b inside a->b->c) would overlap: keep outermost.
            spans = sorted({func.span(n) for _, n in occurrences})
            spans = [s for s in spans if not any(o != s and o[0] <= s[0] and s[1] <= o[1] for o in spans)]
            name = func.fresh("value")
            t = _temp_type(func, node)
            indent = _indent(func, stmts[first])
            at = func.decl_insert_point(compound)
            a = func.full(stmts[first])[0]
            edits = [(*s, name) for s in spans]
            if at == a:
                edits.append((a, a, f"{_declare(t, name)};\n{indent}{name} = {func.src(node)};\n{indent}"))
            else:
                edits += [(at, at, f"\n{indent}{_declare(t, name)};"), (a, a, f"{name} = {func.src(node)};\n{indent}")]
            try:
                yield Site(edits, "hoist_load", f"{text[:30]} x{len(spans)}", not calls)
            except ValueError:
                continue


def rule_reassociate(func):
    """(a op b) op c <-> a op (b op c), and redundant parentheses around products."""
    for node in func.nodes:
        if node.kind != K.BINARY_OPERATOR:
            continue
        op = func.operator(node)
        if op not in ("+", "*", "-"):
            continue
        floating = rewrite.is_float(node)
        left, right = func.children(node)
        sl, sr = rewrite.strip(left), rewrite.strip(right)
        if op in ("+", "*") and sl.kind == K.BINARY_OPERATOR and func.operator(sl) == op:
            a, b = func.children(sl)
            text = f"{func.src(a)} {op} ({func.src(b)} {op} {func.src(right)})"
            yield Site([(*func.span(node), text)], "reassociate", f"line {node.location.line}",
                       safe=not floating)
        if op in ("+", "*") and sr.kind == K.BINARY_OPERATOR and func.operator(sr) == op and \
                right.kind == K.PAREN_EXPR:
            b, c = func.children(sr)
            text = f"{func.src(left)} {op} {func.src(b)} {op} {func.src(c)}"
            yield Site([(*func.span(node), text)], "reassociate", f"line {node.location.line}",
                       safe=not floating)
        if floating and op in ("+", "-"):
            for child in (left, right):
                if child.kind == K.BINARY_OPERATOR and func.operator(child) in ("*", "/"):
                    yield Site([(*func.span(child), f"({func.src(child)})")], "parenthesize",
                               f"line {node.location.line}")
                if child.kind == K.PAREN_EXPR:
                    inner = rewrite.strip(child)
                    if inner.kind == K.BINARY_OPERATOR and func.operator(inner) in ("*", "/"):
                        yield Site([(*func.span(child), func.src(inner))], "unparenthesize",
                                   f"line {node.location.line}")


def rule_incdec(func):
    """i++ / ++i / i += 1 / i = i + 1 as statements or loop increments."""
    for node in func.nodes:
        parent = func.up(node)
        if parent is None or parent.kind not in (K.COMPOUND_STMT, K.FOR_STMT):
            continue
        if parent.kind == K.FOR_STMT and func.children(parent)[-1] == node:
            continue
        text = func.src(node).strip()
        match = (re.fullmatch(r"(\+\+|--)\s*([\w.\->\[\]]+)", text) or
                 re.fullmatch(r"([\w.\->\[\]]+)\s*(\+\+|--)", text))
        compound = re.fullmatch(r"([\w.\->\[\]]+)\s*([+-])=\s*1", text)
        explicit = re.fullmatch(r"([\w.\->\[\]]+)\s*=\s*([\w.\->\[\]]+)\s*([+-])\s*1", text)
        if match:
            groups = match.groups()
            target, op = (groups[1], groups[0]) if groups[0] in ("++", "--") else groups
            sign = op[0]
        elif compound:
            target, sign = compound.groups()
        elif explicit and explicit.group(1) == explicit.group(2):
            target, sign = explicit.group(1), explicit.group(3)
        else:
            continue
        forms = [f"{target}{sign}{sign}", f"{sign}{sign}{target}", f"{target} {sign}= 1",
                 f"{target} = {target} {sign} 1"]
        for form in forms:
            if form != text:
                yield Site([(*func.span(node), form)], "incdec", f"line {node.location.line}")


def _for_parts(func, node):
    """(init, cond, inc, body) texts of a for statement, or None."""
    text = func.src(node)
    match = re.match(r"for\s*\(", text)
    if not match:
        return None
    depth, parts, start = 0, [], match.end()
    i = start
    while i < len(text):
        ch = text[i]
        if ch == "(":
            depth += 1
        elif ch == ")":
            if depth == 0:
                parts.append(text[start:i])
                break
            depth -= 1
        elif ch == ";" and depth == 0:
            parts.append(text[start:i])
            start = i + 1
        i += 1
    if len(parts) != 3:
        return None
    body = func.children(node)[-1]
    return [p.strip() for p in parts] + [body]


def _has_continue(func, body):
    """A `continue` that belongs to this loop (not to a nested loop)."""
    for n in func.walk(body):
        if n.kind == K.CONTINUE_STMT:
            loop = func.enclosing(n, LOOPS)
            if loop is not None and func.key(func.up(body) or body) == func.key(loop):
                return True
    return False


def _body_inner(func, body):
    text = func.full_src(body).strip()
    if body.kind == K.COMPOUND_STMT:
        return text[1:-1].strip()
    return text


def rule_loop_form(func):
    """for <-> while, while -> if + do-while (loop rotation by hand)."""
    for node in func.nodes:
        if node.kind == K.FOR_STMT:
            parts = _for_parts(func, node)
            if parts is None:
                continue
            init, cond, inc, body = parts
            if not cond or _has_continue(func, body) or re.match(r"(int|i32|u32|unsigned)\b", init):
                continue
            inner = _body_inner(func, body)
            pre = f"{init};\n" if init else ""
            step = f"\n{inc};" if inc else ""
            a, b = func.full(node)
            yield Site([(a, b, f"{pre}while ({cond}) {{\n{inner}{step}\n}}")], "for_to_while",
                       f"line {node.location.line}")
            yield Site([(a, b, f"{pre}if ({cond}) {{\ndo {{\n{inner}{step}\n}} while ({cond});\n}}")],
                       "for_to_do", f"line {node.location.line}")
        elif node.kind == K.WHILE_STMT:
            cond, body = func.children(node)[0], func.children(node)[-1]
            inner = _body_inner(func, body)
            a, b = func.full(node)
            c = func.src(cond)
            yield Site([(a, b, f"if ({c}) {{\ndo {{\n{inner}\n}} while ({c});\n}}")], "while_to_do",
                       f"line {node.location.line}")
            yield Site([(a, b, f"for (; {c};) {{\n{inner}\n}}")], "while_to_for",
                       f"line {node.location.line}", natural=False)


def rule_guard_tail(func):
    """Void function tails: `if (c) { B }` <-> `if (!c) return; B`."""
    if not func.returns_void:
        return
    stmts = func.children(func.body)
    if not stmts:
        return
    last = stmts[-1]
    if last.kind == K.IF_STMT and len(func.children(last)) == 2:
        cond, then = func.children(last)
        inner = _body_inner(func, then)
        a, b = func.full(last)
        yield Site([(a, b, f"if ({rewrite.negate(func, cond)})\nreturn;\n{inner}")], "wrap_to_guard",
                   f"line {last.location.line}")
    for i, stmt in enumerate(stmts[:-1]):
        if stmt.kind != K.IF_STMT or len(func.children(stmt)) != 2:
            continue
        cond, then = func.children(stmt)
        if not (then.kind == K.RETURN_STMT or then.kind == K.COMPOUND_STMT and
                [c.kind for c in func.children(then)] == [K.RETURN_STMT]):
            continue
        if func.children(rewrite.strip(then) if then.kind == K.RETURN_STMT else func.children(then)[0]):
            continue
        rest_a = func.full(stmts[i + 1])[0]
        rest_b = func.full(stmts[-1])[1]
        rest = func.data[rest_a:rest_b].decode("latin-1")
        if any(s.kind == K.DECL_STMT for s in stmts[i + 1:]) and func.c is False:
            pass
        a = func.full(stmt)[0]
        yield Site([(a, rest_b, f"if ({rewrite.negate(func, cond)}) {{\n{rest}\n}}")], "guard_to_wrap",
                   f"line {stmt.location.line}")


def rule_ternary(func):
    """return c ? a : b <-> if (c) return a; return b;  and  if/else assignment -> ternary."""
    for node in func.nodes:
        if node.kind == K.RETURN_STMT and func.children(node):
            value = rewrite.strip(func.children(node)[0])
            if value.kind == K.CONDITIONAL_OPERATOR:
                c, x, y = func.children(value)
                a, b = func.full(node)
                yield Site([(a, b, f"if ({func.src(c)})\nreturn {func.src(x)};\nreturn {func.src(y)};")],
                           "ternary_return_to_if", f"line {node.location.line}")
        if node.kind == K.IF_STMT and len(func.children(node)) == 3:
            c, then, other = func.children(node)
            pair = []
            for branch in (then, other):
                stmt = branch
                if branch.kind == K.COMPOUND_STMT:
                    inner = func.children(branch)
                    if len(inner) != 1:
                        break
                    stmt = inner[0]
                expr = rewrite.strip(stmt)
                if stmt.kind == K.RETURN_STMT and func.children(stmt):
                    pair.append(("return", None, func.src(func.children(stmt)[0])))
                elif expr.kind == K.BINARY_OPERATOR and func.operator(expr) == "=":
                    lhs, rhs = func.children(expr)
                    pair.append(("assign", re.sub(r"\s+", "", func.src(lhs)), func.src(rhs), func.src(lhs)))
                else:
                    break
            if len(pair) != 2 or pair[0][:2] != pair[1][:2]:
                continue
            a, b = func.full(node)
            if pair[0][0] == "return":
                text = f"return {func.src(c)} ? {pair[0][2]} : {pair[1][2]};"
            else:
                lhs = pair[0][3]
                text = f"{lhs} = {func.src(c)} ? {pair[0][2]} : {pair[1][2]};"
            yield Site([(a, b, text)], "if_to_ternary", f"line {node.location.line}")


def rule_element_pointer(func):
    """Inside a loop body: `T *e = &a[i];` and e->f for repeated a[i].f (I36 / pointer walk)."""
    for loop in func.nodes:
        if loop.kind not in LOOPS:
            continue
        body = func.children(loop)[-1] if loop.kind != K.DO_STMT else func.children(loop)[0]
        if body.kind != K.COMPOUND_STMT:
            continue
        groups = {}
        for node in func.walk(body):
            if node.kind != K.ARRAY_SUBSCRIPT_EXPR:
                continue
            base, index = func.children(node)
            if not rewrite.pure(func, node):
                continue
            canon = node.type.get_canonical()
            if canon.kind != ci.TypeKind.RECORD:
                continue
            groups.setdefault(re.sub(r"\s+", "", func.src(node)), []).append(node)
        for text, nodes in groups.items():
            node = nodes[0]
            names = {n.spelling for n in [node, *func.walk(node)] if n.kind == K.DECL_REF_EXPR}
            changed = any(func.assigned(n) for n in func.walk(body) if n.kind == K.DECL_REF_EXPR
                          and n.spelling in names)
            t = node.type.spelling
            if "(" in t or "[" in t:
                continue
            name = func.fresh("entry")
            edits = []
            for n in nodes:
                a, b = func.span(n)
                after = func.data[b:b + 2]
                if after[:1] == b".":
                    edits.append((a, b + 1, name + "->"))
                else:
                    edits.append((a, b, f"(*{name})"))
            at = func.decl_insert_point(body)
            edits.append((at, at, f"\n{_indent(func, func.children(body)[0])}{t} *{name} = &{func.src(node)};"))
            try:
                yield Site(edits, "element_pointer", text[:30], not changed)
            except ValueError:
                continue


def rule_inline_pointer(func):
    """T *e = &X; e->f  ->  X.f (undo a local object pointer)."""
    for decl in func.locals:
        inits = [c for c in func.children(decl) if c.kind != K.TYPE_REF]
        if not inits:
            continue
        init = rewrite.strip(inits[-1])
        text = func.src(init)
        if init.kind != K.UNARY_OPERATOR or not text.startswith("&") or not rewrite.pure(func, init):
            continue
        target = text[1:].strip()
        uses = func.uses(decl)
        if not uses or any(func.assigned(r) for r in uses):
            continue
        edits = []
        for r in uses:
            a, b = func.span(r)
            after = func.data[b:b + 2]
            before = func.data[a - 1:a]
            if after == b"->":
                edits.append((a, b + 2, f"{rewrite.paren(target)}."))
            elif before == b"*":
                edits.append((a - 1, b, rewrite.paren(target)))
            else:
                edits.append((a, b, f"(&{rewrite.paren(target)})"))
        stmt = func.up(decl)
        if len([d for d in func.children(stmt) if d.kind == K.VAR_DECL]) != 1:
            continue
        edits.append((*func.full(stmt), ""))
        try:
            yield Site(edits, "inline_pointer", decl.spelling, False)
        except ValueError:
            continue


def rule_inline_cast(func):
    """T *v = (T *)param; v->f  ->  ((T *)param)->f (cast at each use, I45). VC8 allocates
    registers differently for the copy; a repeated cast is a MATCH-EXCEPTION form."""
    params = {p.spelling for p in func.params}
    for decl in func.locals:
        inits = [c for c in func.children(decl) if c.kind != K.TYPE_REF]
        if not inits:
            continue
        init = inits[-1]
        while init.kind == K.UNEXPOSED_EXPR and func.children(init):
            init = func.children(init)[0]
        if init.kind != K.CSTYLE_CAST_EXPR:
            continue
        refs = [n for n in func.walk(init) if n.kind == K.DECL_REF_EXPR]
        if len(refs) != 1 or refs[0].spelling not in params:
            continue
        param = refs[0].spelling
        uses = func.uses(decl)
        if not uses or any(func.assigned(r) for r in uses) or func.address_taken(decl):
            continue
        stmt = func.up(decl)
        if stmt is None or len([d for d in func.children(stmt) if d.kind == K.VAR_DECL]) != 1:
            continue
        cast = f"({func.src(init)})"
        edits = [(*func.span(r), cast) for r in uses] + [(*func.full(stmt), "")]
        param_assigned = any(n.spelling == param and func.assigned(n) for n in func.nodes
                             if n.kind == K.DECL_REF_EXPR)
        try:
            yield Site(edits, "inline_cast", decl.spelling, not param_assigned, False)
        except ValueError:
            continue


def _int_type_token(func, stmt):
    """(start, end, spelling) of the integer type at the start of a declaration statement."""
    text = func.src(stmt)
    for t in sorted(INT_TYPES, key=len, reverse=True):
        if re.match(rf"{re.escape(t)}\b", text):
            a = func.span(stmt)[0]
            return a, a + len(t), t
    return None


def rule_int_type(func):
    """Change a local's integer type: signedness (same size) or width."""
    for stmt in func.nodes:
        if stmt.kind != K.DECL_STMT:
            continue
        token = _int_type_token(func, stmt)
        if token is None:
            continue
        a, b, t = token
        decls = [d for d in func.children(stmt) if d.kind == K.VAR_DECL]
        if any("*" in d.type.spelling or "[" in d.type.spelling for d in decls):
            continue
        for new in [SIGN_FLIP.get(t)] + WIDTHS.get(t, []):
            if new:
                yield Site([(a, b, new)], "int_type", f"{decls[0].spelling if decls else ''} {t}->{new}",
                           safe=False)


def rule_struct_copy(func):
    """a = b; for a struct  <->  member-by-member copies."""
    for node in func.nodes:
        if node.kind != K.BINARY_OPERATOR or func.operator(node) != "=":
            continue
        parent = func.up(node)
        if parent is None or parent.kind != K.COMPOUND_STMT:
            continue
        lhs, rhs = func.children(node)
        canon = lhs.type.get_canonical()
        if canon.kind != ci.TypeKind.RECORD:
            continue
        fields = list(canon.get_fields())
        if not fields or any(f.type.get_canonical().kind in (ci.TypeKind.CONSTANTARRAY, ci.TypeKind.RECORD)
                             or f.is_bitfield() for f in fields):
            continue
        if not rewrite.pure(func, lhs) or not rewrite.pure(func, rhs):
            continue
        l, r = rewrite.paren(func.src(lhs)), rewrite.paren(func.src(rhs))
        indent = _indent(func, node)
        text = ("\n" + indent).join(f"{l}.{f.spelling} = {r}.{f.spelling};" for f in fields)
        a, b = func.full(node)
        yield Site([(a, b, text)], "struct_to_members", f"line {node.location.line}")
    for compound in [func.body, *(n for n in func.nodes if n.kind == K.COMPOUND_STMT)]:
        stmts = func.children(compound)
        i = 0
        while i < len(stmts):
            expr = rewrite.strip(stmts[i])
            parsed = _member_copy(func, expr)
            if parsed is None:
                i += 1
                continue
            lbase, rbase, record = parsed[0], parsed[1], parsed[3]
            fields = [f.spelling for f in record.get_fields()]
            j, seen = i, []
            while j < len(stmts):
                p = _member_copy(func, rewrite.strip(stmts[j]))
                if p is None or p[0] != lbase or p[1] != rbase:
                    break
                seen.append(p[2])
                j += 1
            if sorted(seen) == sorted(fields) and len(seen) == len(fields):
                a, b = func.full(stmts[i])[0], func.full(stmts[j - 1])[1]
                yield Site([(a, b, f"{lbase} = {rbase};")], "members_to_struct", lbase)
            i = max(j, i + 1)


def _member_copy(func, expr):
    if expr.kind != K.BINARY_OPERATOR or func.operator(expr) != "=":
        return None
    lhs, rhs = [rewrite.strip(c) for c in func.children(expr)]
    if lhs.kind != K.MEMBER_REF_EXPR or rhs.kind != K.MEMBER_REF_EXPR or lhs.spelling != rhs.spelling:
        return None
    lt, rt = func.src(lhs), func.src(rhs)
    lb, rb = lt[:lt.rfind(".")], rt[:rt.rfind(".")]
    if not lb or not rb or lt[len(lb):] != "." + lhs.spelling or rt[len(rb):] != "." + rhs.spelling:
        return None
    base = func.children(lhs)
    if not base:
        return None
    record = base[0].type.get_canonical()
    if record.kind != ci.TypeKind.RECORD:
        return None
    return lb.strip(), rb.strip(), lhs.spelling, record


def rule_decl_init(func):
    """T x = E;  <->  T x; ... x = E; as the first statement after the declarations."""
    for compound in [func.body, *(n for n in func.nodes if n.kind == K.COMPOUND_STMT)]:
        run, stmts = func.decl_run(compound)
        if not run:
            continue
        first_stmt = stmts[0] if stmts else None
        insert = func.full(run[-1])[1]
        for stmt in run:
            decls = [d for d in func.children(stmt) if d.kind == K.VAR_DECL]
            if len(decls) != 1:
                continue
            d = decls[0]
            inits = [c for c in func.children(d) if c.kind != K.TYPE_REF]
            t = func.type_text(d)
            if not inits or t is None or inits[-1].kind == K.INIT_LIST_EXPR:
                continue
            a, b = func.full(stmt)
            indent = _indent(func, stmt)
            safe = rewrite.pure(func, inits[-1])
            yield Site([(a, b, f"{_declare(t, d.spelling)};"),
                        (insert, insert, f"\n{indent}{d.spelling} = {func.src(inits[-1])};")],
                       "split_init", d.spelling, safe)
        if first_stmt is None:
            continue
        expr = rewrite.strip(first_stmt)
        if expr.kind != K.BINARY_OPERATOR or func.operator(expr) != "=":
            continue
        lhs, rhs = func.children(expr)
        lhs = rewrite.strip(lhs)
        if lhs.kind != K.DECL_REF_EXPR:
            continue
        target = lhs.referenced
        for k, stmt in enumerate(run):
            decls = [d for d in func.children(stmt) if d.kind == K.VAR_DECL]
            if len(decls) != 1 or func.key(decls[0]) != func.key(target):
                continue
            if [c for c in func.children(target) if c.kind != K.TYPE_REF]:
                break
            later = {d.spelling for s in run[k:] for d in func.children(s) if d.kind == K.VAR_DECL}
            if any(n.kind == K.DECL_REF_EXPR and n.spelling in later for n in [rhs, *func.walk(rhs)]):
                break
            t = func.type_text(target)
            if t is None:
                break
            safe = rewrite.pure(func, rhs) and all(
                not [c for c in func.children(d) if c.kind != K.TYPE_REF]
                for s in run[k + 1:] for d in func.children(s) if d.kind == K.VAR_DECL)
            yield Site([(*func.full(stmt), f"{_declare(t, target.spelling)} = {func.src(rhs)};"),
                        (*func.full(first_stmt), "")], "join_init", target.spelling, safe)


# Unnatural probes: never committed; they show what VC8's allocator reacts to.

def rule_unnatural(func):
    run, stmts = func.decl_run(func.body)
    points = [func.span(func.body)[0] + 1] + [func.full(s)[1] for s in run]
    for k, at in enumerate(points):
        yield Site([(at, at, f"\n    int unused{k};")], "dummy_local", f"@{k}", True, False)
    for d in func.locals:
        stmt = func.up(d)
        if stmt is None or stmt.kind != K.DECL_STMT:
            continue
        a = func.span(stmt)[0]
        yield Site([(a, a, "register ")], "register_local", d.spelling, True, False)
        if not [c for c in func.children(d) if c.kind != K.TYPE_REF] and \
                d.type.get_canonical().kind not in (ci.TypeKind.RECORD, ci.TypeKind.CONSTANTARRAY):
            e = d.extent.end.offset
            yield Site([(e, e, " = 0")], "dead_init", d.spelling, True, False)
    for s in stmts:
        if s.kind in (K.DECL_STMT, K.CASE_STMT, K.DEFAULT_STMT, K.LABEL_STMT):
            continue
        a, b = func.full(s)
        yield Site([(a, a, "{ "), (b, b, " }")], "extra_block", f"line {s.location.line}", True, False)


def rule_rewrite(func):
    """The rules of tools/rewrite.py, relabelled."""
    for rule in rewrite.RULES:
        try:
            for candidate in rule(func):
                source, edits = candidate._edit
                kind = candidate.kind
                natural = REWRITE_RULES.get(kind, True)
                if kind == "mirror_compare":
                    # A constant moved to the left is a Yoda comparison.
                    left = func.data[edits[0][0]:edits[0][1]].decode("latin-1")
                    if re.match(r"\s*(-?\d|0x|NULL|'|\")", edits[0][2]):
                        kind, natural = "yoda_compare", False
                yield Site(list(edits), kind, candidate.note, candidate.safe, natural)
        except (ValueError, StopIteration, AttributeError, IndexError, KeyError):
            continue


RULES = [rule_decl_order, rule_split_multi_decl, rule_copy_param, rule_reuse_variable, rule_split_local,
         rule_temp_expr, rule_hoist_load, rule_reassociate, rule_incdec, rule_loop_form, rule_guard_tail,
         rule_ternary, rule_element_pointer, rule_inline_pointer, rule_inline_cast, rule_int_type, rule_struct_copy,
         rule_decl_init, rule_rewrite, rule_unnatural]


YODA = re.compile(r"^[\s(]*(-?\d[\w.]*|0x\w+|NULL|'[^']*')[\s)]*(==|!=|<=|>=|<|>)[^=]")


def sites(func, unnatural=False):
    """All mutation sites of the parsed function (natural ones only unless asked)."""
    out, seen = [], set()
    for rule in RULES:
        try:
            for site in rule(func):
                if site.natural and any(YODA.match(e[2]) for e in site.edits):
                    site.kind, site.natural = "yoda_compare", False
                if not site.natural and not unnatural and site.kind not in EXCEPTION_KINDS:
                    continue
                if site.edits in seen or not site.edits:
                    continue
                seen.add(site.edits)
                out.append(site)
        except (ValueError, StopIteration, AttributeError, IndexError, KeyError):
            continue
    return out


def combine(rng, available, count):
    """Choose up to `count` non-overlapping sites."""
    chosen = []
    for site in rng.sample(available, min(len(available), count * 4)):
        if any(site.overlaps(c) for c in chosen):
            continue
        chosen.append(site)
        if len(chosen) == count:
            break
    return chosen
