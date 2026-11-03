#!/usr/bin/env python3
"""Self-test of mutate.py's identical-code check (--no-tce switches it off; 2026-10-28).

1. mutcode.Groups: the first claim of a key is the representative, later claims get its outcome; when the
   representative fails, a waiter takes over.
2. On the merged spec lib2_sead_15 (function 02759928), a --rerun of hand-made receipts:
   - mutant 0 = the unmutated source, mutant 1 = the source with trailing blanks on its first line (same code):
     both `equivalent-identical-code`, never run (no run command in the receipt), reason "compiles to identical
     machine code";
   - mutant 2 = a mutant a short campaign killed, mutant 3 = the same mutant plus trailing blanks on its line (same object, same scope):
     one of them runs, the other takes its outcome and names it (tce_same_code_as); both are killed;
   - with --no-tce mutants 0 and 1 run and survive, and 2 and 3 both run.
Writes temporary directories under build/verify/_mutate_tce_test/ and removes them. Exit 0 when all cases pass.
"""
import glob
import json
import os
import shutil
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import mutcode  # noqa: E402

TMP = os.path.join(ROOT, "build", "verify", "_mutate_tce_test")
UNIT, FUNC = "lib2_sead_15", "02759928"
KILLS = ("difference", "abort", "signal", "invalid")


def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)


def mutate(*args):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--func", FUNC, "--n", "100", "-j", "4",
                        "--no-progress"] + list(args), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def groups_cases(fails):
    g = mutcode.Groups()
    first, got = g.claim("k", 1)
    if not first or got is not None:
        fails.append("Groups: first claim is not the representative")
    res = {}

    def waiter():
        res["w"] = g.claim("k", 2)
    t = threading.Thread(target=waiter)
    t.start()
    time.sleep(0.2)
    if "w" in res:
        fails.append("Groups: a waiter returned before the representative finished")
    g.done("k", ("difference", {"rc": 1}))
    t.join(5)
    if res.get("w") != (False, (1, ("difference", {"rc": 1}))):
        fails.append("Groups: waiter got %r" % (res.get("w"),))
    # a failed representative hands over to the next claim
    g2 = mutcode.Groups()
    g2.claim("x", 1)
    res2 = {}
    t = threading.Thread(target=lambda: res2.setdefault("w", g2.claim("x", 2)))
    t.start()
    time.sleep(0.2)
    g2.failed("x")
    t.join(5)
    if res2.get("w") != (True, None):
        fails.append("Groups: after a failed representative the waiter got %r" % (res2.get("w"),))


def main():
    fails = []
    groups_cases(fails)
    private_build_dir()
    os.makedirs(TMP, exist_ok=True)
    A, R, B, C = (os.path.join(TMP, x) for x in "ARBC")
    try:
        rc, out = mutate("--max", "12", "--receipts", A, "--no-tce")
        ra = receipts(A)
        killed = [i for i, j in sorted(ra.items()) if j["class"] in KILLS]
        if rc or not killed:
            fails.append("campaign A: rc %d, no killed mutant: %s" % (rc, out[-400:]))
            raise SystemExit
        k = killed[0]
        src = ra[k]["src"]
        orig = open(os.path.join(ROOT, src)).read()
        kt = open(os.path.join(A, "mutant%d.cpp" % k)).read()
        os.makedirs(R)
        blank = lambda t: t.replace("\n", "   \n", 1)
        # mutant 3: the trailing blanks on the mutated line itself, so its changed span (and so its per-mutant scope,
        # part of the shared-run key) is the same as mutant 2's (2026-10-30: blanks on the first line widened mutant 3
        # to the whole scope once the real/setup scoping scoped mutant 2 to its real-listed function)
        import mutscope
        at = mutscope.changed_span(orig, kt)[0]
        eol = kt.index("\n", at)
        for i, (text, cls) in enumerate([(orig, "survived"), (blank(orig), "survived"), (kt, "difference"),
                                         (kt[:eol] + "   " + kt[eol:], "difference")]):
            open(os.path.join(R, "mutant%d.cpp" % i), "w").write(text)
            json.dump({"id": i, "src": src, "func": FUNC, "mutation": "tce test %d" % i, "class": cls},
                      open(os.path.join(R, "mutant%d.json" % i), "w"))
        rc, out = mutate("--rerun", R, "--rerun-class", "survived,difference", "--receipts", B)
        rb = receipts(B)
        if rc or sorted(rb) != [0, 1, 2, 3]:
            fails.append("TCE rerun: rc %d, ids %s: %s" % (rc, sorted(rb), out[-600:]))
        for i in (0, 1):
            j = rb.get(i, {})
            if j.get("class") != mutcode.EQ_CLASS or "cmd" in j or j.get("reason") != mutcode.EQ_REASON:
                fails.append("mutant %d: %s" % (i, {x: j.get(x) for x in ("class", "cmd", "reason")}))
        j2, j3 = rb.get(2, {}), rb.get(3, {})
        if j2.get("class") not in KILLS or j3.get("class") != j2.get("class"):
            fails.append("mutants 2/3 classes %s/%s" % (j2.get("class"), j3.get("class")))
        names = (j2.get("tce_same_code_as"), j3.get("tce_same_code_as"))
        if sorted(map(str, names)) not in (["3", "None"], ["2", "None"]):
            fails.append("mutants 2/3: exactly one should name the other as representative: %s" % (names,))
        if "2 mutant(s) closed as equivalent-identical-code" not in out or "1 mutant(s) took the outcome" not in out:
            fails.append("summary line missing: %s" % out[-600:])
        rc, out = mutate("--rerun", R, "--rerun-class", "survived,difference", "--receipts", C, "--no-tce")
        rcs = receipts(C)
        got = {i: rcs.get(i, {}).get("class") for i in range(4)}
        if rc or got[0] != "survived" or got[1] != "survived" or got[2] not in KILLS or got[3] not in KILLS or \
                any("tce_same_code_as" in j for j in rcs.values()) or any("cmd" not in j for j in rcs.values()):
            fails.append("--no-tce: rc %d classes %s" % (rc, got))
    except SystemExit:
        pass
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_tce_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
