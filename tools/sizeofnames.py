#!/usr/bin/env python3
"""Write `sizeof` where a literal is the size or element count of the object it acts on.

A decompiler prints every size as a number: `memset(&x, 0, 0x40)`, `NuAlloc(0x1c, ...)`,
`for (i = 0; i < 16; i++) { a[i] = ...; }`. The size contexts read here (libclang, lint flags, 32-bit
MSVC target; units that use the diagnostic macros also with `-DSAGA_DEBUG`):

- the length argument of a size-taking call (SIZE_CALLS: memset, memcpy, memmove, memcmp, strncpy,
  NuStrNCpy) whose pointer argument is a known object: `&x` or an array (`sizeof(x)`), or a pointer to a
  complete type other than a byte or void (`sizeof(*p)`);
- the size argument of an allocator (ALLOCATORS) whose result is cast to, assigned to or initialises a
  pointer to a complete type other than a byte or void (`sizeof(T)`);
- the bound of a `for` loop `i < N` whose body indexes arrays with `i` directly and every such array has N
  elements (`sizeof(a) / sizeof(a[0])`, only for an unsigned 32-bit counter: `sizeof` is unsigned, so a
  signed comparison would turn unsigned; those sites are listed as signed loops and keep their literal);
- the offset added to a byte-pointer cast of a pointer to a complete type, `(u8 *)p + N` (`sizeof(*p)`).

A literal qualifies only when libclang's size (or element count) of that object equals its value; a size
is never guessed from the value alone. A literal written in a macro or a macro argument is left alone.
Sites where the literal is a multiple of the object's size (a count of elements) are listed, not
rewritten. tools/castscan.py counts the qualifying sites as `size-literal` (config/castscan-budget.json).

    python3 tools/sizeofnames.py --dry-run           # list the edits (from the castscan cache)
    python3 tools/sizeofnames.py --list-multiples    # also the sites that are a multiple of the size
    python3 tools/sizeofnames.py                     # rewrite them, then format and prove

The facts come from tools/castscan.py's scan (its cache, or a parse on SAGA_SCAN_LOCK with `--jobs`).
Only the literal's text changes; prove each run (`make proof`).
"""
import argparse
import collections
import os
import re
import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT, clang_jobs  # noqa: E402

LITERAL = re.compile(r'(0[xX][0-9a-fA-F]+|[0-9]+)$')
# Callee name -> (index of the length argument, indexes of the pointer arguments it measures).
SIZE_CALLS = {"memset": (2, (0,)), "memcpy": (2, (0, 1)), "memmove": (2, (0, 1)), "memcmp": (2, (0, 1)),
              "strncpy": (2, (0,)), "NuStrNCpy": (2, (0,))}
# Allocator name -> index of the size argument; the object is the pointer the result becomes.
ALLOCATORS = {"NuAlloc": 0, "AllocPool": 0, "malloc": 0, "GameBufferAlloc": 2}
ADD, LT, NE, ASSIGN = 6, 11, 16, 22
UNSIGNED_INTS = set()  # clang TypeKinds, filled by Finder
ADDRESS_OF = 5
# Names tools/oddscan.py counts as oddities (placeholder fields, PC suffixes, view types): a sizeof that would
# spell one more of them keeps its literal until the name is fixed.
ODD_NAMES = re.compile(r'\b(?:unk_?[0-9a-fA-F]*\w*|pad_\w*|_?pad[0-9]+\w*|field_(?:0x)?[0-9a-fA-F]+\w*'
                       r'|[A-Za-z_]\w*(?:Pc|_PC|_pc)|\w*(?:_VIEW_PC|_VIEW|_view|View_PC|ViewPc))\b')

_ci = None


def cindex():
    global _ci
    if _ci is None:
        import ctypes  # noqa: PLC0415
        import clang.cindex as ci  # noqa: PLC0415
        lib = ci.conf.lib
        for name in ("clang_getCursorBinaryOperatorKind", "clang_getCursorUnaryOperatorKind"):
            getattr(lib, name).argtypes = [ci.Cursor]
            getattr(lib, name).restype = ctypes.c_int
        _ci = ci
    return _ci


def literal_value(text):
    match = LITERAL.match(text)
    if not match:
        return None
    digits = match.group(1)
    if digits.lower().startswith("0x"):
        return int(digits, 16)
    if len(digits) > 1 and digits.startswith("0"):
        return int(digits, 8)
    return int(digits)


class Finder:
    """The size literals of one parsed unit: [{file, offset, text, value, kind, new, function}]."""

    def __init__(self, tu, root=ROOT):
        self.ci = cindex()
        self.tu = tu
        self.root = str(root) + "/"
        self.texts = {}
        self.sites = {}
        K = self.ci.CursorKind
        self.casts = (K.CSTYLE_CAST_EXPR, K.CXX_STATIC_CAST_EXPR, K.CXX_FUNCTIONAL_CAST_EXPR,
                      K.CXX_REINTERPRET_CAST_EXPR, K.CXX_CONST_CAST_EXPR)
        self.functions = (K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR)
        T = self.ci.TypeKind
        self.bytes = (T.CHAR_S, T.CHAR_U, T.SCHAR, T.UCHAR, T.VOID, T.BOOL)
        UNSIGNED_INTS.update((T.UINT, T.ULONG))

    def binop(self, cursor):
        return self.ci.conf.lib.clang_getCursorBinaryOperatorKind(cursor)

    def unop(self, cursor):
        return self.ci.conf.lib.clang_getCursorUnaryOperatorKind(cursor)

    def text_at(self, cursor):
        """(file, offset, text) of a cursor's own source text, or None when it is not plain text in src/."""
        start, end = cursor.extent.start, cursor.extent.end
        if start.file is None or end.file is None or start.file.name != end.file.name:
            return None
        name = os.path.realpath(start.file.name)
        if not name.startswith(self.root + "src/"):
            return None
        if name not in self.texts:
            self.texts[name] = Path(name).read_bytes()
        return name[len(self.root):], start.offset, self.texts[name][start.offset:end.offset].decode("latin-1")

    def expressions(self, cursor):
        return [c for c in cursor.get_children() if c.kind.is_expression()]

    def strip(self, cursor, casts=False):
        """Without parentheses and implicit conversions (and explicit casts with `casts`)."""
        K = self.ci.CursorKind
        while cursor.kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR) or casts and cursor.kind in self.casts:
            inner = self.expressions(cursor)
            if len(inner) != 1:
                break
            cursor = inner[0]
        return cursor

    def literal(self, cursor):
        """(file, offset, text, value) of a plain integer literal written in src/, or None."""
        cursor = self.strip(cursor)
        if cursor.kind != self.ci.CursorKind.INTEGER_LITERAL:
            return None
        found = self.text_at(cursor)
        if found is None:
            return None
        value = literal_value(found[2])
        return None if value is None else (*found, value)

    def simple(self, cursor):
        """Source text of an expression without side effects or calls (fine inside `sizeof`), or None."""
        K = self.ci.CursorKind
        stack = [cursor]
        while stack:
            node = stack.pop()
            if node.kind in (K.CALL_EXPR, K.COMPOUND_ASSIGNMENT_OPERATOR):
                return None
            if node.kind == K.BINARY_OPERATOR and self.binop(node) >= ASSIGN:
                return None
            if node.kind == K.UNARY_OPERATOR and self.unop(node) <= 4:  # ++ and --
                return None
            stack.extend(node.get_children())
        found = self.text_at(cursor)
        if found is None or "\n" in found[2] or "#" in found[2]:
            return None
        return found[2]

    def complete(self, ctype):
        """Size of a complete type other than a byte or void, or None."""
        canonical = ctype.get_canonical()
        if canonical.kind in self.bytes:
            return None
        size = canonical.get_size()
        return size if size > 0 else None

    def record(self, literal, kind, size, new, function):
        file, offset, text, value = literal
        site = {"file": file, "offset": offset, "text": text, "value": value, "kind": kind,
                "function": function}
        if value == size and kind == "signed-loop":
            site["signed"] = new
        elif value == size and ODD_NAMES.search(new):
            site["odd"] = new
        elif value == size:
            site["new"] = new
        elif size and value > size and value % size == 0:
            site["multiple"] = f"{value // size} * {new}"
        else:
            return
        self.sites.setdefault((file, offset), site)

    # ----------------------------------------------------------------------- objects
    def measured(self, argument):
        """[(size, sizeof text)] of the object a pointer argument points at, most specific first."""
        K = self.ci.CursorKind
        T = self.ci.TypeKind
        node = self.strip(argument, casts=True)
        found = []
        if node.kind == K.UNARY_OPERATOR and self.unop(node) == ADDRESS_OF:
            inner = self.expressions(node)
            if len(inner) == 1:
                target = self.strip(inner[0])
                text = self.simple(target)
                size = target.type.get_canonical().get_size()
                if text and size > 0:
                    found.append((size, f"sizeof({text})"))
                if target.kind == K.ARRAY_SUBSCRIPT_EXPR:
                    base = self.strip(self.expressions(target)[0]) if self.expressions(target) else None
                    index = self.literal(self.expressions(target)[-1]) if len(self.expressions(target)) == 2 \
                        else None
                    if base is not None and index is not None and index[3] == 0 and \
                            base.type.get_canonical().kind == T.CONSTANTARRAY:
                        text = self.simple(base)
                        if text:
                            found.append((base.type.get_canonical().get_size(), f"sizeof({text})"))
            return found
        canonical = node.type.get_canonical()
        text = self.simple(node)
        if not text:
            return found
        if canonical.kind == T.CONSTANTARRAY:
            found.append((canonical.get_size(), f"sizeof({text})"))
        elif canonical.kind == T.POINTER:
            size = self.complete(canonical.get_pointee())
            if size:
                if node.kind not in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.ARRAY_SUBSCRIPT_EXPR):
                    text = f"({text})"
                found.append((size, f"sizeof(*{text})"))
        return found

    def size_call(self, call, function):
        name = call.spelling
        size_index, objects = SIZE_CALLS[name]
        arguments = list(call.get_arguments())
        if len(arguments) <= size_index:
            return
        literal = self.literal(arguments[size_index])
        if literal is None:
            return
        candidates = [c for index in objects for c in self.measured(arguments[index])]
        exact = [c for c in candidates if c[0] == literal[3]]
        size, new = exact[0] if exact else candidates[0] if candidates else (0, None)
        if new:
            self.record(literal, "call", size, new, function)

    def allocation(self, call, ancestors, function):
        K = self.ci.CursorKind
        T = self.ci.TypeKind
        arguments = list(call.get_arguments())
        index = ALLOCATORS[call.spelling]
        if len(arguments) <= index:
            return
        literal = self.literal(arguments[index])
        if literal is None:
            return
        target = None
        child = call
        for parent in reversed(ancestors):
            if parent.kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
                child = parent
                continue
            if parent.kind in self.casts or parent.kind == K.VAR_DECL:
                target = parent.type
            elif parent.kind == K.BINARY_OPERATOR and self.binop(parent) == ASSIGN:
                operands = self.expressions(parent)
                if len(operands) == 2 and operands[1] == child:
                    target = operands[0].type
            break
        if target is None or target.get_canonical().kind != T.POINTER:
            return
        pointee = target.get_pointee()
        size = self.complete(pointee)
        spelling = pointee.spelling.replace("const ", "").strip()
        if not size or "(" in spelling or "anonymous" in spelling or "unnamed" in spelling:
            return
        self.record(literal, "allocation", size, f"sizeof({spelling})", function)

    def loop(self, node, function):
        K = self.ci.CursorKind
        T = self.ci.TypeKind
        children = list(node.get_children())
        if len(children) < 2:
            return
        condition = None
        for child in children[:-1]:
            if child.kind == K.BINARY_OPERATOR and self.binop(child) in (LT, NE):
                condition = child
        if condition is None:
            return
        operands = self.expressions(condition)
        if len(operands) != 2:
            return
        counter = self.strip(operands[0])
        literal = self.literal(operands[1])
        if counter.kind != K.DECL_REF_EXPR or literal is None or counter.referenced is None:
            return
        variable = counter.referenced
        counts, bases = set(), []
        stack = [children[-1]]
        while stack:
            current = stack.pop()
            if current.kind == K.ARRAY_SUBSCRIPT_EXPR:
                parts = self.expressions(current)
                if len(parts) == 2:
                    index = self.strip(parts[1])
                    if index.kind == K.DECL_REF_EXPR and index.referenced == variable:
                        base = self.strip(parts[0])
                        canonical = base.type.get_canonical()
                        if canonical.kind == T.CONSTANTARRAY:
                            counts.add(canonical.get_array_size())
                            bases.append(base)
            stack.extend(current.get_children())
        if counts != {literal[3]}:
            return
        # `sizeof` is unsigned: only a comparison that is unsigned already keeps its code (and draws no
        # signed/unsigned warning); a signed counter keeps its literal.
        unsigned = self.strip(operands[0], casts=False).type.get_canonical().kind in UNSIGNED_INTS and \
            operands[0].type.get_canonical().get_size() == 4
        for base in sorted(bases, key=lambda b: b.extent.start.offset):
            text = self.simple(base)
            if text and self.visible(base, condition, node):
                element = f"{text}[0]" if self.postfix(base) else f"({text})[0]"
                new = f"sizeof({text}) / sizeof({element})"
                if unsigned:
                    self.record(literal, "loop", literal[3], new, function)
                else:
                    self.record(literal, "signed-loop", literal[3], new, function)
                return

    def postfix(self, expression):
        """Whether `expression[0]` indexes the whole expression (a name, member access or subscript)."""
        K = self.ci.CursorKind
        return expression.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.ARRAY_SUBSCRIPT_EXPR)

    def visible(self, expression, condition, loop):
        """Whether every name in `expression` is declared before the loop's condition or outside the loop."""
        K = self.ci.CursorKind
        start, end = condition.extent.start, loop.extent.end
        stack = [expression]
        while stack:
            node = stack.pop()
            if node.kind == K.DECL_REF_EXPR and node.referenced is not None:
                where = node.referenced.location
                if where.file is not None and start.file is not None and where.file.name == start.file.name and \
                        start.offset <= where.offset <= end.offset:
                    return False
            stack.extend(node.get_children())
        return True

    def offset(self, node, function):
        """`(u8 *)p + N` where N is the size of *p."""
        T = self.ci.TypeKind
        operands = self.expressions(node)
        if len(operands) != 2 or node.type.get_canonical().kind != T.POINTER:
            return
        pointer, literal = operands[0], self.literal(operands[1])
        if literal is None:
            return
        cast = self.strip(pointer)
        if cast.kind not in self.casts:
            return
        if self.complete(node.type.get_canonical().get_pointee()):
            return  # not a byte pointer: the offset is scaled
        inner = self.expressions(cast)
        if not inner:
            return
        source = self.strip(inner[-1])
        canonical = source.type.get_canonical()
        if canonical.kind != T.POINTER:
            return
        size = self.complete(canonical.get_pointee())
        text = self.simple(source)
        if size and text:
            if source.kind not in (self.ci.CursorKind.DECL_REF_EXPR, self.ci.CursorKind.MEMBER_REF_EXPR):
                text = f"({text})"
            self.record(literal, "offset", size, f"sizeof(*{text})", function)

    # ----------------------------------------------------------------------- walk
    def run(self):
        K = self.ci.CursorKind
        roots = []
        stack = [self.tu.cursor]
        while stack:
            for cursor in stack.pop().get_children():
                if cursor.kind in (K.LINKAGE_SPEC, K.UNEXPOSED_DECL, K.NAMESPACE, K.STRUCT_DECL, K.CLASS_DECL):
                    stack.append(cursor)
                elif cursor.kind in self.functions and cursor.is_definition():
                    file = cursor.location.file
                    if file is not None and os.path.realpath(file.name).startswith(self.root + "src/"):
                        roots.append(cursor)
        for root in roots:
            self.visit(root)
        return sorted(self.sites.values(), key=lambda s: (s["file"], s["offset"]))

    def visit(self, root):
        K = self.ci.CursorKind
        function = root.spelling
        stack = [(root, ())]
        while stack:
            node, ancestors = stack.pop()
            if node.kind == K.CALL_EXPR:
                if node.spelling in SIZE_CALLS:
                    self.size_call(node, function)
                elif node.spelling in ALLOCATORS:
                    self.allocation(node, ancestors, function)
            elif node.kind == K.FOR_STMT:
                self.loop(node, function)
            elif node.kind == K.BINARY_OPERATOR and self.binop(node) == ADD:
                self.offset(node, function)
            below = (*ancestors, node)
            stack.extend((child, below) for child in node.get_children())


def find(tu, root=ROOT):
    """The size literal sites of one parsed unit (plain data)."""
    return Finder(tu, root).run()


def apply(sites, root=ROOT):
    """Write `new` at every site that has one; returns the number written."""
    by_file = collections.defaultdict(list)
    for site in sites:
        if site.get("new"):
            by_file[site["file"]].append(site)
    count = 0
    for file, changes in by_file.items():
        path = root / file
        data = path.read_bytes()
        for site in sorted(changes, key=lambda s: -s["offset"]):
            old, offset = site["text"], site["offset"]
            if data[offset:offset + len(old)].decode("latin-1") != old:
                raise SystemExit(f"{file}:{offset}: expected {old!r}")
            data = data[:offset] + site["new"].encode("latin-1") + data[offset + len(old):]
            count += 1
        path.write_bytes(data)
    return count


def line_of(file, offset, root=ROOT):
    return (root / file).read_bytes()[:offset].count(b"\n") + 1


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--dry-run", action="store_true", help="list the edits, change nothing")
    parser.add_argument("--list-multiples", action="store_true", help="also list literals that are a multiple of "
                        "the object's size (element counts to name by hand)")
    parser.add_argument("--kinds", default="call,allocation,loop,offset", help="contexts to rewrite")
    parser.add_argument("--jobs", type=int, default=None)
    parser.add_argument("paths", nargs="*", help="limit the rewrite to these files or directories")
    args = parser.parse_args(argv)
    from tools import castscan  # noqa: PLC0415
    from tools.magicnames import ScanLock  # noqa: PLC0415
    with ScanLock():
        facts = castscan.scan(clang_jobs(args.jobs))
    sites, failures = castscan.gather_sizes(facts)
    for failure in failures:
        print(f"sizeofnames: {failure}", file=sys.stderr)
    kinds = set(args.kinds.split(","))
    wanted = [Path(p).resolve().relative_to(ROOT).as_posix() for p in args.paths]
    chosen = [s for s in sites if s["kind"] in kinds and (not wanted or any(
        s["file"] == p or s["file"].startswith(p.rstrip("/") + "/") for p in wanted))]
    if args.dry_run or args.list_multiples:
        for site in chosen:
            if site.get("new") and args.dry_run or site.get("multiple") and args.list_multiples:
                what = site.get("new") or f"multiple: {site['multiple']}"
                print(f"{site['file']}:{line_of(site['file'], site['offset'])}: {site['kind']} {site['text']} -> "
                      f"{what} in {site['function']}")
    found = collections.Counter(s["kind"] for s in sites if s.get("new") or s.get("odd"))
    print(f"sizeofnames: {sum(found.values())} size literals ({', '.join(f'{k} {n}' for k, n in sorted(found.items()))}"
          f"), {sum(1 for s in sites if s.get('odd'))} of them left for a name oddscan counts, "
          f"{sum(1 for s in sites if s.get('multiple'))} multiples, "
          f"{sum(1 for s in sites if s.get('signed'))} counts of a signed loop left as literals")
    if not args.dry_run and not args.list_multiples:
        print(f"sizeofnames: {apply(chosen)} rewritten; run make format")
    return 0


if __name__ == "__main__":
    sys.exit(main())
