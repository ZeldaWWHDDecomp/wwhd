#!/usr/bin/env python3
"""Self-test of the readable_moves.tsv support in readable_check.py (static checks only, no builds).

usage: readable_moves_test.py

Creates a temporary detached worktree of HEAD (removed afterwards), copies this tree's readable_check.py and autoglue.py
into it and simulates moves between two unit specs of HEAD, with HEAD itself as REF (READABLE_REF):
a spec P with two `src` lines hands its second source (and so that source's functions) to another spec Q.

  listed        the moved functions listed P -> Q            -> must pass (function set ok with moves)
  unlisted      same move, empty table                        -> must FAIL (function set changed)
  wrong-old     table names a spec that never tested them     -> must FAIL (old spec does not test it)
  duplicate     one address listed twice                      -> must FAIL (listed twice)
  removed-spec  P deleted, all its sources go to Q, all listed -> must pass for target P (removed spec ok)
  moved-lines   a one-source spec R with a `field *` line is removed; its function bodies move into Q's source and its
                directive lines into Q's spec, `*` narrowed to the moved address -> must pass, plain (spec ok, lines moved)
  edited-line   same, one moved line edited beyond the narrowing     -> must FAIL (spec file differs)
Exit status 0 when every case behaves as expected.
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(ROOT, "tools", "verify", "units")
sys.path.insert(0, HERE)
HEADER = "# test table\n"


def sh(cwd, *cmd, env=None):
    return subprocess.run(list(cmd), cwd=cwd, capture_output=True, text=True, env=env)


def srcs_of(text):
    return [w for l in text.splitlines() for w in l.split("#")[0].split()[1:] if l.split() and l.split()[0] == "src"]


def pick():
    """P: a top-level spec with exactly two src lines whose second source tests functions; Q: a one-src top-level spec"""
    import readable_check
    names = sorted(f[:-4] for f in os.listdir(UNITS) if f.endswith(".txt"))
    p = q = None
    for n in names:
        t = open(os.path.join(UNITS, n + ".txt")).read()
        s = srcs_of(t)
        if "include" in t or not all(os.path.exists(os.path.join(ROOT, x)) for x in s):
            continue
        if p is None and len(s) == 2 and readable_check.tested([open(os.path.join(ROOT, s[1])).read()]) and \
                readable_check.tested([open(os.path.join(ROOT, s[0])).read()]):
            p = n
        elif q is None and len(s) == 1:
            q = n
        if p and q:
            return p, q
    sys.exit("readable_moves_test: no suitable pair of specs in this tree")


def pick_r(q):
    """R: a one-source, include-free top-level spec (not Q) with a `field *` line, whose source tests 1-3 functions"""
    import readable_check
    for n in sorted(f[:-4] for f in os.listdir(UNITS) if f.endswith(".txt")):
        t = open(os.path.join(UNITS, n + ".txt")).read()
        s = srcs_of(t)
        if n == q or "include" in t or len(s) != 1 or not os.path.exists(os.path.join(ROOT, s[0])):
            continue
        if any(l.split()[:2] == ["field", "*"] for l in t.splitlines()) and \
                1 <= len(readable_check.tested([open(os.path.join(ROOT, s[0])).read()])) <= 3:
            return n
    sys.exit("readable_moves_test: no suitable spec R")


def main():
    import readable_check
    p, q = pick()
    ptext = open(os.path.join(UNITS, p + ".txt")).read()
    qtext = open(os.path.join(UNITS, q + ".txt")).read()
    ps = srcs_of(ptext)
    moved = sorted(readable_check.tested([open(os.path.join(ROOT, ps[1])).read()]))
    allp = sorted(readable_check.tested([open(os.path.join(ROOT, f)).read() for f in ps]))
    print("P = %s (srcs %s), Q = %s; second source tests %d function(s)" % (p, " ".join(ps), q, len(moved)))
    rspec = pick_r(q)
    rtext = open(os.path.join(UNITS, rspec + ".txt")).read()
    rsrc = srcs_of(rtext)[0]
    rfuncs = sorted(readable_check.tested([open(os.path.join(ROOT, rsrc)).read()]))
    qsrc = srcs_of(qtext)[0]
    print("R = %s (%s, %d function(s)) moves into Q's source %s" % (rspec, rsrc, len(rfuncs), qsrc))
    tmp = tempfile.mkdtemp(prefix="rdmove-")
    wt = os.path.join(tmp, "wt")
    r = sh(ROOT, "git", "worktree", "add", "--detach", wt, "HEAD")
    if r.returncode:
        sys.exit("cannot create a worktree: " + r.stderr)
    head = sh(ROOT, "git", "rev-parse", "HEAD").stdout.strip()
    env = dict(os.environ, READABLE_REF=head)
    failures = []
    try:
        def case(name, expect_ok, expect_text, table, remove_p=False, target=None):
            sh(wt, "git", "checkout", "-q", "-f", "HEAD")
            for f in ("readable_check.py", "autoglue.py"):  # the checker under test: this tree's
                shutil.copy2(os.path.join(HERE, f), os.path.join(wt, "tools", "verify", f))
            pp, qp = os.path.join(wt, "tools/verify/units", p + ".txt"), os.path.join(wt, "tools/verify/units", q + ".txt")
            if remove_p:
                os.remove(pp)  # inside the temporary worktree this test created
                open(qp, "w").write(qtext.rstrip("\n") + "\n" + "".join("src %s\n" % f for f in ps))
            else:
                open(pp, "w").write("".join(l for l in ptext.splitlines(True) if l.split()[:2] != ["src", ps[1]]))
                open(qp, "w").write(qtext.rstrip("\n") + "\nsrc %s\n" % ps[1])
            open(os.path.join(wt, "tools/verify/readable_moves.tsv"), "w").write(HEADER + "".join(
                "%08X\t%s\t%s\n" % e for e in table))
            r = sh(wt, sys.executable, "tools/verify/readable_check.py", target or q, *([] if remove_p else [p]),
                   "--static-only", "--allow-spec-change", env=env)
            out = r.stdout + r.stderr
            ok = (r.returncode == 0) == expect_ok and expect_text in out
            print("%-13s %s (exit %d, expected %s, %r %s)" % (name, "as expected" if ok else "WRONG", r.returncode,
                                                             "pass" if expect_ok else "FAIL", expect_text,
                                                             "found" if expect_text in out else "missing"))
            if not ok:
                failures.append(name)
                print("\n".join("    " + l for l in out.splitlines()[-8:]))

        case("listed", True, "static checks ok", [(a, p, q) for a in moved])
        case("unlisted", False, "function set changed", [])
        case("wrong-old", False, "does not test it", [(a, q + "_nosuch", q) for a in moved])
        case("duplicate", False, "listed twice", [(a, p, q) for a in moved] + [(moved[0], p, q)])
        case("removed-spec", True, "removed spec ok", [(a, p, q) for a in allp], remove_p=True, target=p)
        def line_case(name, expect_ok, expect_text, edit=False):
            sh(wt, "git", "checkout", "-q", "-f", "HEAD")
            for f in ("readable_check.py", "autoglue.py"):
                shutil.copy2(os.path.join(HERE, f), os.path.join(wt, "tools", "verify", f))
            b = open(os.path.join(ROOT, rsrc)).read()
            m = re.search(r"^/\* [0-9A-Fa-f]{8} \*/", b, re.M)
            g = b.find("/* WWHD glue")
            body = b[m.start():g if g > 0 else len(b)]
            qp = os.path.join(wt, qsrc)
            a = open(qp).read()
            ga = a.find("/* WWHD glue")
            ga = len(a) if ga < 0 else ga
            open(qp, "w").write(a[:ga] + body + "\n" + a[ga:])
            os.remove(os.path.join(wt, "tools/verify/units", rspec + ".txt"))  # inside this test's temporary worktree
            moved = []
            for l in rtext.splitlines():
                w = l.split()
                if not w or w[0].startswith("#") or w[0] == "src":
                    continue
                if len(w) > 1 and w[1] == "*":
                    w[1] = "%08X" % rfuncs[0]
                moved.append(" ".join(w))
            if edit:
                moved[0] = moved[0] + " 1"
            open(os.path.join(wt, "tools/verify/units", q + ".txt"), "w").write(qtext.rstrip("\n") + "\n" + "\n".join(moved) + "\n")
            open(os.path.join(wt, "tools/verify/readable_moves.tsv"), "w").write(HEADER + "".join(
                "%08X\t%s\t%s\n" % (x, rspec, q) for x in rfuncs))
            r = sh(wt, sys.executable, "tools/verify/readable_check.py", q, rspec, "--static-only", env=env)
            out = r.stdout + r.stderr
            ok = (r.returncode == 0) == expect_ok and expect_text in out
            print("%-13s %s (exit %d, expected %s, %r %s)" % (name, "as expected" if ok else "WRONG", r.returncode,
                                                             "pass" if expect_ok else "FAIL", expect_text,
                                                             "found" if expect_text in out else "missing"))
            if not ok:
                failures.append(name)
                print("\n".join("    " + l for l in out.splitlines()[-8:]))

        # a move REF already contains (REF newer than the move, here REF = HEAD): skipped with a note, the check passes
        sh(wt, "git", "checkout", "-q", "-f", "HEAD")
        for f in ("readable_check.py", "autoglue.py"):
            shutil.copy2(os.path.join(HERE, f), os.path.join(wt, "tools", "verify", f))
        open(os.path.join(wt, "tools/verify/readable_moves.tsv"), "w").write(HEADER + "".join(
            "%08X\t%s\t%s\n" % (a, p, q) for a in moved))
        open(os.path.join(wt, "tools/verify/units", p + ".txt"), "w").write(
            "".join(l for l in ptext.splitlines(True) if l.split()[:2] != ["src", ps[1]]))
        open(os.path.join(wt, "tools/verify/units", q + ".txt"), "w").write(qtext.rstrip("\n") + "\nsrc %s\n" % ps[1])
        sh(wt, "git", "-c", "user.name=t", "-c", "user.email=t@t", "commit", "-qam", "applied move")
        newhead = sh(wt, "git", "rev-parse", "HEAD").stdout.strip()
        r = sh(wt, sys.executable, "tools/verify/readable_check.py", q, p, "--static-only", env=dict(env, READABLE_REF=newhead))
        out = r.stdout + r.stderr
        ok = r.returncode == 0 and "already applied" in out
        print("%-13s %s (exit %d, expected pass, 'already applied' %s)" % ("applied-ref", "as expected" if ok else "WRONG",
              r.returncode, "found" if "already applied" in out else "missing"))
        if not ok:
            failures.append("applied-ref")
            print("\n".join("    " + l for l in out.splitlines()[-8:]))
        line_case("moved-lines", True, "spec ok (lines moved per readable_moves.tsv)")
        line_case("edited-line", False, "spec file differs", edit=True)
    finally:
        sh(ROOT, "git", "worktree", "remove", "--force", wt)
        shutil.rmtree(tmp, ignore_errors=True)
    print("readable_moves_test: %s" % ("FAIL: " + " ".join(failures) if failures else "ok (8/8 cases)"))
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
