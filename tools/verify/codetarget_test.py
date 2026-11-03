#!/usr/bin/env python3
"""Self-test of the -nulltrap candidate-only instruction-fetch rule (2026-10-29, decision D): under
-nulltrap / unit `nulltrap` an indirect call or branch (bctrl/bctr, gabi::call_ptr/tail_ptr) at any depth whose target is
page 0 or not a code address (outside the image's code below 0x10000000, not a function import slot, not a target the
unit declares) keeps the generic mock on the original side; the candidate may make the same calls (same target value, at
most as often per target, on the same input) with the same mock, and any other candidate call to such a target is a
console ISI that ends the candidate's run like a page-0 DSI (Result::dsi, kind 'X', ea = target). Without the option such
calls keep the generic mock (old result).

Uses lib_js_base's JKRFileLoader::getGlbResource 027EBF08 (findVolume 027EBE3C mocked by `ret!` to a fixture volume
V = 0x41000000, vtable 0x42000000 at V+0xC, the call through vtable slot +0x24 with r3 = V, r4 = name):
a. both sides call the non-code target 30000000: ok, no fault, same row as without -nulltrap (a page-0 target faults on
   both sides since 2026-11-02, : page0call_test.py);
b. a candidate that calls through 0 while the original calls a .text target: `trap: page-0/invalid instruction fetch by
   the candidate only (target 00000000, depth 0)` (also via the unit directive), a .data target 10161F10 likewise; without
   -nulltrap the old difference;
c. a candidate that stores the non-code word 30000010 into the slot and calls through it (the original calls .text): fault
   `... (target 30000010, depth 0)`; without -nulltrap no trap;
d. import slot (C0008898) and .text (027EBE3C) targets: ok with -nulltrap; a declared `callee 0BAD0000 fixture` target too;
e. without -nulltrap: the same rows as before for every case above (no trap anywhere);
f. the candidate calls the original's non-code target 30000000 twice (the original once): the extra call faults
   `... (target 30000000, depth 0)`;
g. original-only non-code dispatch (the original calls 30000000, the candidate calls .text 027EBE3C): no fetch fault on
   either side, the row is the same with and without -nulltrap;
h. nested: the slot holds `real` 027EBF60 (getGlbResourceFromVolume), which calls through name->vtable slot +0x2C; a
   candidate that passes name + 0x10 (whose vtable 0x45000000 has slot 0) faults inside the real callee:
   `... (target 00000000, depth 1 in 027EBF60)`;
i. -refcache: the case-f mutant run with the original side taken from records written by the unmodified source gives the
   same stdout as the live run (verdict and first difference), every input from the records.
Writes a temporary unit directory (tools/verify/units/_codetarget_test, build/verify/_codetarget_test) and a mutant source
next to the original (overwritten by the next run, never deleted: remove them by hand when no longer wanted). Exit 0 when
all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_codetarget_test")
F = "027EBF08"
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
MUT = "wwhd_src/JSystem/JKernel/_codetarget_test_JKRFileLoader.cpp"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
CALL = "    return gabi::call_ptr<void*>(volume->mVtable->mGetResource,volume,(const be<u8>*)*remaining);\n"
# `stable`: mocked calls (findVolume) must not clobber the fixture words (generated-mode clobbering)
FIX = ["ret! 027EBE3C 0x41000000", "field %s 0x4100000C u32 0x42000000" % F] + ["stable 0x%X 0x40" % (0x41000000 + i * 0x1000000) for i in range(5)]


def slot(v):
    return "field %s 0x42000024 u32 %s" % (F, v)


def run(name, lines, extra=(), src=SRC, n="200"):
    body = [("src " + src) if l.strip() == "src " + SRC else l for l in BASE]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_codetarget_test/" + name, "-only", F, "-n", n,
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

    def want_ok(what, rc, row, err, nofault=True):
        if rc or "200/200" not in row or (nofault and "nulltrap:" in err):
            fails.append("%s: want ok without faults, got %s %s" % (what, row, err[-300:]))

    def want_trap(what, rc, row, text):
        if rc == 0 or text not in row:
            fails.append("%s: want %r, got %s" % (what, text, row))

    try:
        text_slot = FIX + [slot("0x027EBE3C")]
        nt = ["-nulltrap"]
        cand = "trap: page-0/invalid instruction fetch by the candidate only (target %s, depth %s)"

        def same_rows(what, lines, src=SRC):
            rc1, r1, e1 = run(what + "_on", lines, extra=nt, src=src)
            rc2, r2, e2 = run(what + "_off", lines, src=src)
            if rc1 != rc2 or r1.split()[2:] != r2.split()[2:] or "fetch" in e1 or "trap:" in r1:
                fails.append("%s: want the same row with and without -nulltrap and no fetch fault, got %s / %s %s" % (what, r1, r2, e1[-300:]))
            return rc1, r1

        # a. both sides call the same non-code target (a page-0 target faults on both sides since 2026-11-02:
        # page0call_test.py)
        for v in ("0x30000000",):
            rc, row = same_rows("both_" + v, FIX + [slot(v)])
            if rc or "200/200" not in row:
                fails.append("both sides call %s: want ok, got %s" % (v, row))
        # b. candidate-only page-0 / .data target
        mutant(CALL, "    return gabi::call_ptr<void*>(0u,volume,(const be<u8>*)*remaining);\n")
        rc, row, _ = run("null_off", text_slot, src=MUT)
        if rc == 0 or "trap:" in row:
            fails.append("NULL call without -nulltrap: want the old (generic mock) difference, got %s" % row)
        rc, row, _ = run("null_on", text_slot, extra=nt, src=MUT)
        want_trap("NULL call with -nulltrap", rc, row, cand % ("00000000", 0))
        rc, row, _ = run("null_dir", text_slot + ["nulltrap"], src=MUT)
        want_trap("NULL call with the unit directive", rc, row, cand % ("00000000", 0))
        mutant(CALL, "    return gabi::call_ptr<void*>(0x10161F10u,volume,(const be<u8>*)*remaining);\n")
        rc, row, _ = run("data_on", text_slot, extra=nt, src=MUT)
        want_trap("call to .data 10161F10 with -nulltrap", rc, row, cand % ("10161F10", 0))
        # c. candidate-only program-stored non-code word
        mutant(CALL, "    gabi::store<u32>(0x42000024u, 0x30000010u);\n" + CALL)
        rc, row, _ = run("stored_on", text_slot, extra=nt, src=MUT)
        want_trap("program-stored non-code target with -nulltrap", rc, row, cand % ("30000010", 0))
        rc, row, _ = run("stored_off", text_slot, src=MUT)
        if rc == 0 or "trap:" in row:
            fails.append("program-stored target without -nulltrap: want the old (non-trap) difference, got %s" % row)
        # d. import slot / .text / declared fixture targets
        for name, lines in (("import", FIX + [slot("0xC0008898")]), ("text", text_slot),
                            ("fixture", FIX + [slot("0x0BAD0000"), "callee 0BAD0000 fixture r3=0"])):
            rc, row = same_rows(name, lines)
            if rc or "200/200" not in row:
                fails.append("%s target: want ok, got %s" % (name, row))
        # f. the candidate repeats the original's non-code call
        twice = ("    (void)gabi::call_ptr<void*>(volume->mVtable->mGetResource,volume,(const be<u8>*)*remaining);\n" + CALL)
        mutant(CALL, twice)
        rc, row, _ = run("twice_on", FIX + [slot("0x30000000")], extra=nt, src=MUT)
        want_trap("extra call to the original's non-code target", rc, row, cand % ("30000000", 0))
        rc, row, _ = run("twice_off", FIX + [slot("0x30000000")], src=MUT)
        if rc == 0 or "trap:" in row:
            fails.append("extra call without -nulltrap: want the old (non-trap) difference, got %s" % row)
        # g. original-only non-code dispatch: legacy mock on the original side, no fetch fault
        mutant(CALL, "    return gabi::call_ptr<void*>(0x027EBE3Cu,volume,(const be<u8>*)*remaining);\n")
        rc, row = same_rows("origonly", FIX + [slot("0x30000000")], src=MUT)
        if rc == 0:
            fails.append("original-only dispatch: the candidate calls a different target, want the old difference, got %s" % row)
        # h. nested: inside a real callee
        nested = FIX + [slot("0x027EBF60"), "real 027EBF60", "arg! %s r3 0x43000000" % F,
                        "field %s 0x4300000C u32 0x44000000" % F, "field %s 0x4400002C u32 0x027EBE3C" % F,
                        "field %s 0x4300001C u32 0x45000000" % F, "field %s 0x4500002C u32 0" % F]
        rc, row = same_rows("nested_ok", nested)
        if rc or "200/200" not in row:
            fails.append("nested .text target (unmodified): want ok, got %s" % row)
        mutant(CALL, "    return gabi::call_ptr<void*>(volume->mVtable->mGetResource,volume,(const be<u8>*)*remaining + 0x10);\n")
        rc, row, _ = run("nested_on", nested, extra=nt, src=MUT)
        want_trap("nested NULL call with -nulltrap", rc, row, cand % ("00000000", "1 in 027EBF60"))
        rc, row, _ = run("nested_off", nested, src=MUT)
        if rc == 0 or "trap:" in row:
            fails.append("nested mutant without -nulltrap: want the old (non-trap) difference, got %s" % row)
        # i. -refcache: records of the unmodified original, read by the case-f mutant
        sys.path.insert(0, HERE)
        import verify
        import specfile
        rcl = FIX + [slot("0x30000000"), "nulltrap"]
        mutant(CALL, twice)
        for name, src in (("rc_base", SRC), ("rc_mut", MUT)):
            body = [("src " + src) if l.strip() == "src " + SRC else l for l in BASE]
            with open(os.path.join(TMP, name + ".txt"), "w") as f:
                f.write("\n".join(body + rcl) + "\n")
        exes, specs = {}, {}
        for name in ("rc_base", "rc_mut"):
            u = "_codetarget_test/" + name
            exes[name] = verify.build(u, jobs=4)
            specs[name] = specfile.spec_path(u, ROOT, verify.unit_dir(u))
        rdir = os.path.join(ROOT, "build", "verify", "_codetarget_test", "refcache")
        os.makedirs(rdir, exist_ok=True)

        def hx(name, *args):
            r = subprocess.run([exes[name], "-spec", specs[name], "-n", "200", "-seed", "1", "-only", F] + list(args), cwd=ROOT,
                               capture_output=True, text=True, env=dict(os.environ, WWHD_VERIFY_PROGRESS="0"))
            return r.returncode, r.stdout, r.stderr
        hx("rc_base", "-refcache-write", rdir, "-refcache-key", "CT")
        live = hx("rc_mut")
        cached = hx("rc_mut", "-refcache", rdir, "-refcache-key", "CT")
        if live[:2] != cached[:2] or cand % ("30000000", 0) not in live[1]:
            fails.append("-refcache: cached and live original give different results (or no fault): %s / %s" % (live[1][-300:], cached[1][-300:]))
        if "200 input(s) took the recorded original, 0 ran it live" not in cached[2]:
            fails.append("-refcache: the records were not used: %s" % cached[2][-300:])
    finally:
        pass  # nothing is deleted (project rule): the temporary unit directory, its build output and the mutant stay
    for f in fails:
        print("FAIL", f)
    print("codetarget_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
