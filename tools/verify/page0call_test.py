#!/usr/bin/env python3
"""Self-test of -nulltrap page-0 calls (2026-11-02, ): under -nulltrap / unit `nulltrap` an indirect call or
branch (bctrl/bctr, gabi::call_ptr/tail_ptr, any depth) whose target word address lies in page 0 (0x0000..0x0FFF: a NULL
function pointer or vtable slot) is a console ISI on EITHER side and ends that side's run exactly like a page-0 DSI
(Result::dsi, kind 'X', ea = target), classified like one: candidate-only = difference (a kill), original-only = fixture
defect / no evidence, both sides at the same target = compared up to there. Non-code targets at 0x1000 and above keep the
candidate-only rule of codetarget_test.py. Without -nulltrap nothing changes (generic mock); -p0exclude notes the
original's page-0 call like its page-0 access.

Uses lib_js_base's JKRFileLoader::getGlbResource 027EBF08 (fixture as codetarget_test.py: findVolume 027EBE3C mocked to
the volume V = 0x41000000, vtable 0x42000000 at V+0xC, the call through vtable slot +0x24):
a. candidate-only: the original calls a .text slot, the candidate calls 0: `trap: page-0/invalid instruction fetch by the
   candidate only (target 00000000, depth 0)`, a kill under cascade.row_kills;
b. original-only: the slot holds 0, the candidate calls .text 027EBE3C directly: `... by the original only (target
   00000000, depth 0)`, stderr `the original took a page-0 DSI on 200 input(s); first: fetch 00000000`, no kill under
   cascade.row_kills; the target 2 (low bits ignored) likewise;
c. both sides at the same site: the slot holds 0, unmodified source: 200/200 (compared up to the fault, no return value),
   the original's DSI note on stderr; a candidate that stores a word into the volume before the same call: a difference
   that is not a trap (the effect up to the fault is compared);
d. without -nulltrap: no trap and no nulltrap note in any case above; the unmodified slot-0 row passes (generic mock on
   both sides) and the case-b mutant gives the old non-trap difference; with -p0exclude the case-b differences are
   excluded (`p0exclude: 027EBF08: 200 difference(s) after the original's page-0 access excluded`).
Writes a temporary unit directory (tools/verify/units/_page0call_test, build/verify/_page0call_test) and a mutant source
next to the original (overwritten by the next run, never deleted: remove them by hand when no longer wanted). Exit 0 when
all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import cascade  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_page0call_test")
F = "027EBF08"
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
MUT = "wwhd_src/JSystem/JKernel/_page0call_test_JKRFileLoader.cpp"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
CALL = "    return gabi::call_ptr<void*>(volume->mVtable->mGetResource,volume,(const be<u8>*)*remaining);\n"
FIX = ["ret! 027EBE3C 0x41000000", "field %s 0x4100000C u32 0x42000000" % F] + ["stable 0x%X 0x40" % (0x41000000 + i * 0x1000000) for i in range(5)]


def slot(v):
    return "field %s 0x42000024 u32 %s" % (F, v)


def run(name, lines, extra=(), src=SRC, n="200"):
    body = [("src " + src) if l.strip() == "src " + SRC else l for l in BASE]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_page0call_test/" + name, "-only", F, "-n", n,
                        "-buildj", "4"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(F)]
    return r.returncode, (row[0] if row else r.stdout[-400:]), r.stderr


def mutant(old, new):
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(old) != 1:
        raise SystemExit("mutation site not found once in %s: %r" % (SRC, old))
    open(os.path.join(ROOT, MUT), "w").write(text.replace(old, new))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    nt = ["-nulltrap"]
    fetch = "trap: page-0/invalid instruction fetch by the %s only (target %s, depth 0)"
    text_slot = FIX + [slot("0x027EBE3C")]
    zero_slot = FIX + [slot("0")]

    def check(what, cond, got):
        if not cond:
            fails.append("%s: got %s" % (what, got))

    def plain(what, rc, row, err, want_pass):
        check(what + " without -nulltrap: no trap, no nulltrap note" + (", passes" if want_pass else ", old difference"),
              "trap:" not in row and "nulltrap:" not in err and (("200/200" in row and rc == 0) if want_pass else rc != 0),
              "%s %s" % (row, err[-300:]))

    # a. candidate-only call to page 0
    mutant(CALL, "    return gabi::call_ptr<void*>(0u,volume,(const be<u8>*)*remaining);\n")
    rc, row, err = run("cand_on", text_slot, extra=nt, src=MUT)
    check("candidate-only call to 0", rc != 0 and fetch % ("candidate", "00000000") in row and cascade.row_kills(row), row)
    rc, row, err = run("cand_dir", text_slot + ["nulltrap"], src=MUT)
    check("candidate-only call to 0 (unit directive)", rc != 0 and fetch % ("candidate", "00000000") in row, row)
    rc, row, err = run("cand_off", text_slot, src=MUT)
    plain("candidate-only call to 0", rc, row, err, False)
    # b. original-only call to page 0 (the fixture leaves the slot 0)
    for v, t in (("0", "00000000"), ("2", "00000002")):
        mutant(CALL, "    return gabi::call_ptr<void*>(0x027EBE3Cu,volume,(const be<u8>*)*remaining);\n")
        rc, row, err = run("orig_on_" + v, FIX + [slot(v)], extra=nt, src=MUT)
        check("original-only call to %s" % t, rc != 0 and fetch % ("original", t) in row and not cascade.row_kills(row) and
              "the original took a page-0 DSI on 200 input(s); first: fetch %s" % t in err, "%s %s" % (row, err[-300:]))
    rc, row, err = run("orig_off", zero_slot, src=MUT)
    plain("original-only call to 0", rc, row, err, False)
    rc, row, err = run("orig_p0x", zero_slot, extra=["-p0exclude"], src=MUT)
    check("original-only call to 0 with -p0exclude: excluded",
          "p0exclude: %s: 200 difference(s) after the original's page-0 access excluded" % F in err and "trap:" not in row,
          "%s %s" % (row, err[-300:]))
    # c. both sides call page 0 at the same site
    rc, row, err = run("both_on", zero_slot, extra=nt)
    check("both sides call 0: compared up to there, equal", rc == 0 and "200/200" in row and
          "the original took a page-0 DSI on 200 input(s); first: fetch 00000000" in err, "%s %s" % (row, err[-300:]))
    rc, row, err = run("both_dir", zero_slot + ["nulltrap"])
    check("both sides call 0 (unit directive)", rc == 0 and "200/200" in row, row)
    mutant(CALL, "    gabi::store<u32>(0x41000020u, 0x12345678u);\n" + CALL)
    rc, row, err = run("both_pre_on", zero_slot, extra=nt, src=MUT)
    check("both sides call 0 after a candidate-only store: a compared (non-trap) difference",
          rc != 0 and "trap:" not in row and "200/200" not in row and "41000020" in row, row)
    # d. without -nulltrap (rows as before the change: generic mock)
    rc, row, err = run("both_off", zero_slot)
    plain("both sides call 0", rc, row, err, True)
    rc, row, err = run("both_pre_off", zero_slot, src=MUT)
    plain("candidate-only store before the shared call to 0", rc, row, err, False)
    for f in fails:
        print("FAIL", f)
    print("page0call_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
