"""Unit-spec includes (2026-10-26): `include PATH` in a unit file inserts the lines of tools/verify/units/PATH
(e.g. `include shared/agl_world.txt`) at that point, recursively (a cycle or a missing file stops). Every tool reads
specs through expanded_lines()/spec_path(): the harness gets one flat file, so directive order is the textual order
after expansion. Header comments (`# verify: ...`) are taken from the including file only.

Function-scoped include (2026-10-29, AGL#8): `include PATH for FUNC [FUNC ...]` inserts the same lines, but every
per-function directive whose function field is `*` (SCOPED below: setup/postsetup, field/fieldstr, fill, poke, cr,
copy/blob/blobfile, arg/arg!/argf64/argf64!) is written once per FUNC with that FUNC instead of `*`, in the order given,
so those lines apply only to the named tested functions. Lines with an explicit function, and the global directives
(stable, real, mock*, nestedret, budgets, stackfill, ...), are inserted unchanged, once. The scoping reaches nested
includes. A spec then shares e.g. shared/agl_world.txt for one function instead of carrying a hand-made scoped copy that
can drift from the world."""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
UNITS = os.path.join(HERE, "units")


SCOPED = {"setup", "postsetup", "field", "fieldstr", "fill", "poke", "cr", "copy", "blob", "blobfile", "arg", "arg!",
          "argf64", "argf64!"}


def scope_line(line, funcs):
    """`line` once per function in funcs when it is a SCOPED directive with function `*`, else unchanged"""
    f = line.split("#")[0].split()
    if not funcs or len(f) < 2 or f[0] not in SCOPED or f[1] != "*":
        return [line]
    head, rest = line.split("*", 1)  # the first `*` is the function field (the directive keyword has none)
    return [head + fn + rest for fn in funcs]


def include_args(f, path, line):
    """`include PATH` or `include PATH for FUNC [FUNC ...]` -> (PATH, [FUNC, ...])"""
    if len(f) == 2:
        return f[1], []
    if len(f) >= 4 and f[2] == "for":
        funcs = [x.upper() for x in f[3:]]
        for x in funcs:
            if len(x) != 8 or any(c not in "0123456789ABCDEF" for c in x):
                raise SystemExit("invalid include line in %s (FUNC must be 8 hex digits): %s" % (path, line.strip()))
        if len(set(funcs)) != len(funcs):
            raise SystemExit("invalid include line in %s (a function is named twice): %s" % (path, line.strip()))
        return f[1], funcs
    raise SystemExit("invalid include line in %s (include PATH [for FUNC ...]): %s" % (path, line.strip()))


def gated_specs(units_dir):
    """the specs the whole-tree gates run (verify_all.py, readable_check.py), as units without .txt. Left out: units/shared/ (include
    fragments, no tested function) and every top-level units/_NAME/ (self-test fixtures, and _diag: diagnosis only, no credit). Naming
    such a spec on the command line still runs it."""
    out = []
    for r, ds, fs in os.walk(units_dir):
        if r == units_dir:
            ds[:] = [d for d in ds if d != "shared" and not d.startswith("_")]
        out += [os.path.relpath(os.path.join(r, f), units_dir)[:-4] for f in fs if f.endswith(".txt")]
    return sorted(out)


def expanded_lines(path, _stack=None, _funcs=None):
    stack = _stack or []
    path = os.path.abspath(path)
    if path in stack:
        raise SystemExit("spec include cycle: %s" % " -> ".join(stack + [path]))
    if not os.path.exists(path):
        raise SystemExit("spec include not found: %s" % path)
    out = []
    for line in open(path):
        f = line.split("#")[0].split()
        if f and f[0] == "include":
            inc, funcs = include_args(f, path, line)
            if funcs and _funcs:
                raise SystemExit("invalid include line in %s (`for` inside an include that is already scoped): %s"
                                 % (path, line.strip()))
            scope = funcs or _funcs
            tag = inc + (" for " + " ".join(funcs) if funcs else "")
            out.append("# >>> include %s\n" % tag)
            out += [l for l in expanded_lines(os.path.join(UNITS, inc), stack + [path], scope)
                    if not l.lstrip().startswith("# verify:")]
            out.append("# <<< include %s\n" % tag)
        else:
            out += scope_line(line if line.endswith("\n") else line + "\n", _funcs)
    return out


def has_include(path):
    for line in open(path):
        f = line.split("#")[0].split()
        if f and f[0] == "include":
            return True
    return False


def spec_path(unit, root, out_dir=None):
    """The spec file the harness should read: the unit file itself, or (when it has includes) an expanded copy in
    the unit's build directory (out_dir, default build/verify/<unit>)/spec.expanded.txt, rewritten when it changed."""
    src = os.path.join(UNITS, unit + ".txt")
    if not has_include(src):
        return src
    out = os.path.join(out_dir or os.path.join(root, "build", "verify", unit), "spec.expanded.txt")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    data = "".join(expanded_lines(src))
    if not os.path.exists(out) or open(out).read() != data:
        open(out, "w").write(data)
    return out
