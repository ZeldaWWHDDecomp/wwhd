#!/usr/bin/env python3
"""Self-test of the readable_renames.tsv support in readable_check.py (static checks only, no builds).

usage: readable_renames_test.py [UNIT]   (default UNIT: hd_sys_02032A34, a small HD unit)

Creates a temporary detached worktree of HEAD (removed afterwards), copies this tree's readable_check.py
and autoglue.py into it, renames UNIT's source in several ways and runs `readable_check.py --static-only`:

  mapped      git mv + spec src line + table entry          -> must pass (spec ok via the table)
  old-target  same, target given by the old source path     -> must pass
  unmapped    git mv + spec src line, no table entry        -> must FAIL (spec file differs)
  stale-spec  git mv + table entry, spec src line unchanged -> must FAIL (source missing)
  lost-func   mapped rename, one VERIFY line / ADDR marker dropped -> must FAIL (function set changed)
  bogus-entry table entry whose old path is not in the ref  -> must FAIL
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
HEADER = "# test table\n"


def sh(cwd, *cmd):
    return subprocess.run(list(cmd), cwd=cwd, capture_output=True, text=True)


def main():
    unit = sys.argv[1] if len(sys.argv) > 1 else "hd_sys_02032A34"
    spec_rel = "tools/verify/units/%s.txt" % unit
    spec_text = open(os.path.join(ROOT, spec_rel)).read()
    old = [w for l in spec_text.splitlines() for w in l.split("#")[0].split()[1:] if l.split() and l.split()[0] == "src"][0]
    new = os.path.join(os.path.dirname(old), "hd_rename_selftest" + os.path.splitext(old)[1])
    tmp = tempfile.mkdtemp(prefix="rdren-")
    wt = os.path.join(tmp, "wt")
    r = sh(ROOT, "git", "worktree", "add", "--detach", wt, "HEAD")
    if r.returncode:
        sys.exit("cannot create a worktree: " + r.stderr)
    failures = []
    try:
        def case(name, expect_ok, expect_text, mv=True, table=None, spec=True, drop_verify=False, target=None):
            sh(wt, "git", "checkout", "-q", "-f", "HEAD")
            sh(wt, "git", "clean", "-q", "-fd", os.path.dirname(new))
            for f in ("readable_check.py", "autoglue.py"):  # the checker under test: this tree's
                shutil.copy2(os.path.join(HERE, f), os.path.join(wt, "tools", "verify", f))
            if mv:
                sh(wt, "git", "mv", old, new)
            if spec:
                open(os.path.join(wt, spec_rel), "w").write(spec_text.replace(old, new))
            open(os.path.join(wt, "tools/verify/readable_renames.tsv"), "w").write(HEADER + "".join(
                "%s\t%s\n" % e for e in (table or [])))
            if drop_verify:
                p = os.path.join(wt, new)
                t = open(p).read()
                # a raw source's last VERIFY line, or a GameCube-style source's last /* ADDR */ marker
                m = list(re.finditer(r"^VERIFY\(.*\n", t, re.M)) or list(re.finditer(r"^/\* [0-9A-Fa-f]{8} \*/[ \t]*\n", t, re.M))
                open(p, "w").write(t[:m[-1].start()] + t[m[-1].end():])
            r = sh(wt, sys.executable, "tools/verify/readable_check.py", target or unit, "--static-only")
            out = r.stdout + r.stderr
            ok = (r.returncode == 0) == expect_ok and expect_text in out
            print("%-12s %s (exit %d, expected %s, %r %s)" % (name, "as expected" if ok else "WRONG", r.returncode,
                                                            "pass" if expect_ok else "FAIL", expect_text,
                                                            "found" if expect_text in out else "missing"))
            if not ok:
                failures.append(name)
                print("\n".join("    " + l for l in out.splitlines()[-8:]))

        case("mapped", True, "spec ok (src renamed per readable_renames.tsv)", table=[(old, new)])
        case("old-target", True, "static checks ok", table=[(old, new)], target=old)
        case("unmapped", False, "spec file differs")
        case("stale-spec", False, "source missing", table=[(old, new)], spec=False)
        case("lost-func", False, "function set changed", table=[(old, new)], drop_verify=True)
        case("bogus-entry", False, "is not in", table=[(old, new), ("wwhd_src/hd/no_such_file.cpp", "wwhd_src/hd/x.cpp")])
    finally:
        sh(ROOT, "git", "worktree", "remove", "--force", wt)
        shutil.rmtree(tmp, ignore_errors=True)
    print("readable_renames_test: %s" % ("FAIL: " + " ".join(failures) if failures else "ok (6/6 cases)"))
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
