#!/usr/bin/env python3
"""Self-test of the function-scoped include `include PATH for FUNC [FUNC ...]` (specfile.py, 2026-10-29).

a. `include shared/agl_world.txt for 027A90AC` expands to the world's lines with the function field `*` of every SCOPED
   directive (setup, field, fill, poke, blob, ...) replaced by 027A90AC; every other line (stable, real, mocks,
   nestedret, budgets, comments) unchanged; no SCOPED `*` line left;
b. `for F1 F2`: each scoped line once per function, in the given order, right where the `*` line stood; global lines once;
c. a plain include is unchanged: the world's own lines between the markers;
d. errors: `for` without a function, a non-hex function, a function named twice, and a scoped include inside a scoped
   include stop with `invalid include line`;
e. harness: lib2_agl_12_world_default_quad (tests 027A90AC) with its world include scoped to 027A90AC gives the same row
   for 027A90AC as the unchanged spec (n 50, seed 1); the expanded spec of the scoped copy has no `*` setup/field/fill
   line from the world.
Writes a temporary unit directory (tools/verify/units/_worldscope_test; overwritten by the next run, never deleted).
Builds go below WWHD_VERIFY_SCRATCH when set. Exit 0 when all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import specfile  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_worldscope_test")
WORLD = os.path.join(UNITS, "shared", "agl_world.txt")


def spec(name, text):
    os.makedirs(TMP, exist_ok=True)
    p = os.path.join(TMP, name + ".txt")
    with open(p, "w") as f:
        f.write(text)
    return p


def body(lines):
    """the lines between the outermost include markers"""
    return [l for l in lines if not l.startswith("# >>> include") and not l.startswith("# <<< include")]


def scoped(line):
    f = line.split("#")[0].split()
    return len(f) >= 2 and f[0] in specfile.SCOPED and f[1] == "*"


def main():
    fails = []
    world = [l if l.endswith("\n") else l + "\n" for l in open(WORLD)]
    nscoped = sum(scoped(l) for l in world)
    # a. one function
    got = body(specfile.expanded_lines(spec("one", "include shared/agl_world.txt for 027a90ac\n")))
    want = [l.replace("*", "027A90AC", 1) if scoped(l) else l for l in world]
    print("a. world: %d lines, %d scoped `*` lines (%s)" % (len(world), nscoped,
          ", ".join(sorted({l.split()[0] for l in world if scoped(l)}))))
    if got != want or any(scoped(l) for l in got) or nscoped < 30:
        fails.append("a: the scoped expansion is not the world with `*` -> 027A90AC on scoped lines")
    # b. two functions
    got = body(specfile.expanded_lines(spec("two", "include shared/agl_world.txt for 027A90AC 027ADE6C\n")))
    want = []
    for l in world:
        want += [l.replace("*", f, 1) for f in ("027A90AC", "027ADE6C")] if scoped(l) else [l]
    print("b. two functions: %d lines (world %d + %d)" % (len(got), len(world), nscoped))
    if got != want:
        fails.append("b: two-function expansion differs from per-line duplication in order")
    # c. plain include unchanged
    lines = specfile.expanded_lines(spec("plain", "include shared/agl_world.txt\n"))
    if body(lines) != world or lines[0] != "# >>> include shared/agl_world.txt\n":
        fails.append("c: a plain include is not the world's own lines")
    # d. errors
    for name, text in (("nofunc", "include shared/agl_world.txt for\n"), ("badhex", "include shared/agl_world.txt for 27A90AC\n"),
                       ("twice", "include shared/agl_world.txt for 027A90AC 027a90ac\n"),
                       ("nested", "include _worldscope_test/one.txt for 027ADE6C\n")):
        r = subprocess.run([sys.executable, "-c", "import sys; sys.path.insert(0, %r); import specfile; "
                            "specfile.expanded_lines(%r)" % (HERE, spec("err_" + name, text))], capture_output=True, text=True)
        if r.returncode == 0 or "invalid include line" not in r.stderr:
            fails.append("d %s: want `invalid include line`, got rc %d %s" % (name, r.returncode, r.stderr[-200:]))
    print("d. error cases checked")
    # e. harness rows
    orig = open(os.path.join(UNITS, "lib2_agl_12_world_default_quad.txt")).read()
    if orig.count("include shared/agl_world.txt\n") != 1:
        raise SystemExit("world include not found once in lib2_agl_12_world_default_quad")
    spec("quad_plain", orig)
    spec("quad_scoped", orig.replace("include shared/agl_world.txt\n", "include shared/agl_world.txt for 027A90AC\n"))
    rows = {}
    for name in ("quad_plain", "quad_scoped"):
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_worldscope_test/" + name, "-only", "027A90AC",
                            "-n", "50", "-seed", "1", "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
        rows[name] = (r.returncode, [l for l in r.stdout.splitlines() if l.startswith("027A90AC")])
        print("e. %s: rc %d %s" % (name, r.returncode, rows[name][1][0] if rows[name][1] else r.stderr[-300:]))
    if rows["quad_plain"] != rows["quad_scoped"] or rows["quad_plain"][0] or not rows["quad_plain"][1]:
        fails.append("e: the scoped world gives a different row: %s / %s" % (rows["quad_plain"], rows["quad_scoped"]))
    exp = specfile.expanded_lines(os.path.join(TMP, "quad_scoped.txt"))
    inner = exp[exp.index("# >>> include shared/agl_world.txt for 027A90AC\n"):exp.index("# <<< include shared/agl_world.txt for 027A90AC\n")]
    if any(scoped(l) for l in inner):
        fails.append("e: the scoped spec still has a `*` world line")
    for x in fails:
        print("FAIL", x)
    print("worldscope_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
