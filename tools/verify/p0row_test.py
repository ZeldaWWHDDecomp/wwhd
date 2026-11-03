#!/usr/bin/env python3
"""Self-test: a difference on an input where the original itself faults on page 0 never kills (zero-page rule; 
2026-11-02). When both sides fault at the same access the harness compares up to the fault, and the shown first difference had no
page-0 wording, so cascade.row_kills counted it as a kill. Now the row text carries cascade.P0_MARK whenever the shown difference is
on such an input.

Fixture as nulltrap_test.py 4c: JKRFileLoader::getGlbResource 027EBF08 (lib_js_base), which calls findVolume 027EBE3C and then reads
the volume's vtable. Mutant: findVolume gets another argument (call #0 differs on every input). -nulltrap throughout.
  1. findVolume always returns 0x10: both sides fault at the same access on every input: the row fails, carries the marker, row_kills
     is False, the receipt's kind is difference-page0.
  2. findVolume returns 0x10 or 0x41000000, same run: some inputs fault on both sides (stderr counts them), others are clean: the row
     shows the clean input's difference, has no marker, row_kills is True, the kind is difference. Also with -shards 2.
  3. the unmutated source on both specs: ok.
  4. the shard merge prefers a clean input's difference at a higher input over a marked one at a lower input (it is the unsharded row).
Temporary units and the mutant source are removed. Exit 0 when all pass."""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import cascade  # noqa: E402
import verify  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_p0row_test")
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
MUT = "wwhd_src/JSystem/JKernel/_p0row_test_JKRFileLoader.cpp"
F = "027EBF08"
OLD = "    JKRFileLoader* volume = findVolume(remaining.get());\n"
NEW = "    JKRFileLoader* volume = findVolume(remaining.get() + 1);\n"
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def run(name, lines, src, extra=()):
    base = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
    body = [("src " + src) if l.strip() == "src " + SRC else l for l in base]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_p0row_test/" + name, "-only", F, "-n", "200", "-nulltrap", "-noprogress", "-buildj", "8"] +
                       list(extra), cwd=ROOT, capture_output=True, text=True, errors="replace", env=dict(os.environ, WWHD_VERIFY_CACHE="0"))
    return r.returncode, next((l for l in r.stdout.splitlines() if l.startswith(F)), ""), r.stderr


def kind(row):
    return cascade.kind({"class": "difference", "stdout": row, "cmd": ["verify", "-nulltrap"]})


def main():
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(OLD) != 1:
        raise SystemExit("mutation site not found once in %s" % SRC)
    os.makedirs(TMP, exist_ok=True)
    made = [os.path.join(ROOT, MUT)]
    allfault, mixed = ["ret! 027EBE3C 0x10"], ["ret! 027EBE3C 0x10,0x41000000"]
    try:
        open(os.path.join(ROOT, MUT), "w").write(text.replace(OLD, NEW))
        rc, row, err = run("fault", allfault, MUT)
        check(rc == 1 and "call #0" in row and "different accesses" not in row and cascade.P0_MARK in row, "1 same-access faults on both sides: marked: %s" % row[50:230])
        check(not cascade.row_kills(row) and kind(row) == "difference-page0", "1 no kill: row_kills %s, kind %s" % (cascade.row_kills(row), kind(row)))
        rc, row, err = run("mixed", mixed, MUT)
        check("the original took a page-0 DSI on" in err or "input(s) ended in page-0 faults" in err, "2 the same run has inputs that fault: %s" %
              [l[:110] for l in err.splitlines() if "page-0" in l][:1])
        check(rc == 1 and "call #0" in row and cascade.P0_MARK not in row, "2 the row shows a clean input's difference: %s" % row[50:230])
        check(cascade.row_kills(row) and kind(row) == "difference", "2 it kills: row_kills %s, kind %s" % (cascade.row_kills(row), kind(row)))
        rc, row2, err = run("mixed2", mixed, MUT, ["-shards", "2"])
        check(rc == 1 and cascade.P0_MARK not in row2 and cascade.row_kills(row2), "2 the same with -shards 2: %s" % row2[50:230])
        for name, lines in (("fault_ok", allfault), ("mixed_ok", mixed)):
            rc, row, err = run(name, lines, SRC)
            check(rc == 0 and row.endswith(" ok"), "3 unmutated, %s: %s" % (name, row[50:]))
        head = "@SHARD 027EBF08 0 %d 0 0 0 0 %d 5 - |f|%s"
        funcs, _, _ = verify.merge_shard_outputs(["\n".join([head % (10, 3, "call #0 x " + cascade.P0_MARK + "  {a}")]), "\n".join([head % (10, 12, "call #0 y  {b}")])])
        m = funcs[0x027EBF08]
        check(m["first"] == "call #0 y  {b}" and m["fidx"] == 12, "4 the merge prefers the clean input's difference: %s" % m["first"])
        funcs, _, _ = verify.merge_shard_outputs(["\n".join([head % (10, 12, "call #0 y  {b}")]), "\n".join([head % (10, 3, "call #0 x " + cascade.P0_MARK + "  {a}")])])
        check(funcs[0x027EBF08]["first"] == "call #0 y  {b}", "4 in either shard order")
    finally:
        for p in made:
            if os.path.exists(p):
                os.remove(p)
        for n in os.listdir(TMP):
            os.remove(os.path.join(TMP, n))
        os.rmdir(TMP)
    print("p0row_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
