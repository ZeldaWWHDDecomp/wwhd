#!/usr/bin/env python3
"""Self-test of the harness option -fastreset (harness.cpp, 2026-10-28): with `setupsnapshot`, the captured post-setup
state lives in template pages that stay across inputs and only the pages an input touched are reset; validation inputs
give the candidate side the original side's post-setup state instead of running the setups a second time; the leading
absolute `fill`/`copy` lines are built once per function. Results must be byte-identical to the runs without it.

Every case runs verify.py with WWHD_VERIFY_FASTRESET=0 and =1 and compares stdout, the exit status and the stderr notes
(`setupsnapshot`, `nulltrap`, `semantic effects` lines). Cases on merged lib2_sead_15 (the setupsnapshot_test fixtures):
1. a constant setup (counter constructor), seeds 1 and 7, with and without -nulltrap, -snapcheck 500 and 7;
2. the tested function overwrites setup-written memory or advances the setup's allocation cursor (`fresh`), plus traces
   of late inputs (the trace of a snapshot input and of a validation input, -snapcheck 50);
3. a setup routine that spills per-input non-volatile registers on its own scratch stack (`spill`, footprint), seeds 1/7;
4. refused snapshots (static and run-twice checks) fall back to per-input setups;
5. a validation mismatch exits 2 with the same message;
6. two functions with setups (no -only): the template pages are rebuilt per function;
7. fill layering: an absolute fill, a register-relative fill over it, then an absolute fill over that (only the leading
   absolute line is built once; the later ones stay per input and win);
8. WWHD_VERIFY_FASTRESET_CHECK=1 with -snapcheck 1: the candidate side's real setups equal the kept original-side state.
Writes temporary unit files under tools/verify/units/_fastreset_test/ and removes them (and their build folders).
Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMPN = "_fastreset_test"
TMP = os.path.join(UNITS, TMPN)


def run(name, lines, args, fast, check=False):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    env = dict(os.environ, WWHD_VERIFY_FASTRESET="1" if fast else "0", WWHD_VERIFY_CACHE="0")
    if check:
        env["WWHD_VERIFY_FASTRESET_CHECK"] = "1"
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), TMPN + "/" + name, "-shards", "1", "-buildj", "6"] +
                       list(args), cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout, r.stderr


def notes(err):
    return [l for l in err.splitlines() if l.startswith(("semantic effects", "setupsnapshot", "nulltrap", "fastreset"))]


def same(fails, what, name, lines, args):
    a = run(name, lines, args, False)
    b = run(name, lines, args, True)
    if a[0] != b[0] or a[1] != b[1] or notes(a[2]) != notes(b[2]):
        fails.append("%s: differs with -fastreset (rc %d/%d)\n%s\n%s\n---\n%s\n%s" % (
            what, a[0], b[0], a[1][-500:], "\n".join(notes(a[2]))[-300:], b[1][-500:], "\n".join(notes(b[2]))[-300:]))
    return a


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        sead = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
        func = "02759564"
        for need in ("real 02759928", "real 02759EE8"):
            if need not in sead:
                fails.append("lib2_sead_15 no longer lists %s" % need)
        # 1. constant setup
        const = sead + ["setup %s 02759928 r3=0x5E000000" % func, "setupsnapshot"]
        for seed in ("1", "7"):
            for extra in ([], ["-nulltrap"], ["-snapcheck", "7"]):
                rc, out, err = same(fails, "const seed %s %s" % (seed, extra), "sead", const, ["-only", func, "-n", "1000", "-seed", seed] + extra)
                if "refused" in err or not re.search(r"^%s .* ok$" % func, out, re.M):
                    fails.append("const seed %s %s: snapshot refused or row not ok: %s%s" % (seed, extra, out[-200:], err[-200:]))
        # 2. fresh snapshot per input: the function clears the setup's word or allocates the next block
        fresh = sead + ["alloc 0273AD10 r3 0x5D000000 32", "allocnested 0273AD10", "setup 02759928 02759928 r3=0 ->5E000000",
                        "arg! 02759928 r3 0x5E000000,0", "setupsnapshot"]
        for seed in ("1", "7"):
            same(fails, "fresh seed %s" % seed, "sead", fresh, ["-only", "02759928", "-n", "300", "-seed", seed, "-snapcheck", "50"])
        for i in (150, 151, 152, 153, 298, 299):  # 150: a validation input under -snapcheck 50
            t = [run("sead", fresh, ["-only", "02759928", "-n", "300", "-seed", "1", "-snapcheck", "50", "-trace", str(1000003 + i)],
                     fast)[1] for fast in (False, True)]
            if t[0] != t[1]:
                fails.append("fresh: trace of input %d differs with -fastreset:\n%s\n---\n%s" % (i, t[0][-800:], t[1][-800:]))
        # 3. setup spilling per-input registers on its own stack (footprint)
        spill = sead + ["footprint 02759928 8", "arg! 02759928 r3 0x5E000000", "field * 5E300000 u16 0x31",
                        "field * 5E300002 u16 0x32", "field * 5E300004 u16 0", "setup 02759928 02759EE8 r3=0x5E200000 r4=0x5E300000",
                        "setupsnapshot"]
        for seed in ("1", "7"):
            same(fails, "spill seed %s" % seed, "sead", spill, ["-only", "02759928", "-n", "1000", "-seed", seed, "-snapcheck", "9"])
        # 4. refused snapshots
        choices = ",".join("0x5E0000%02X" % (16 * k) for k in range(8))
        dep = sead + ["field %s 5E100000 u32 %s" % (func, choices), "setup %s 02759928 r3=*5E100000" % func, "setupsnapshot"]
        picks = ",".join("0x5E0%02X000" % k for k in range(7))
        dep2 = sead + ["ret! 0273AD10 %s" % picks, "setup %s 02759928 r3=0" % func, "setupsnapshot"]
        for name, spec in (("static", dep), ("twice", dep2)):
            rc, out, err = same(fails, "refused %s" % name, "sead", spec, ["-only", func, "-n", "500", "-seed", "1"])
            if "setupsnapshot: refused" not in err:
                fails.append("refused %s: not refused: %s" % (name, err[-300:]))
        # 5. validation mismatch (setupsnapshot_test case 4: seed 3)
        val = sead + ["ret! 0273AD10 0x5E000000,0x5E000040", "setup %s 02759928 r3=0" % func, "setupsnapshot"]
        rc, out, err = same(fails, "validation mismatch", "sead", val, ["-only", func, "-n", "10", "-seed", "3"])
        if rc != 2 or "setupsnapshot: validation failed at input 0" not in err:
            fails.append("validation mismatch not fatal (rc %d): %s" % (rc, err[-300:]))
        # 6. two functions with snapshots: per-function template pages
        two = sead + ["setup %s 02759928 r3=0x5E000000" % func, "alloc 0273AD10 r3 0x5D000000 32", "allocnested 0273AD10",
                      "setup 02759928 02759928 r3=0 ->5E000000", "arg! 02759928 r3 0x5E000000,0", "setupsnapshot"]
        for seed in ("1", "7"):
            rc, out, err = same(fails, "two functions seed %s" % seed, "sead", two, ["-only", "%s,02759928" % func, "-n", "400",
                                                                                    "-seed", seed, "-snapcheck", "60"])
            if len(re.findall(r"^(%s|02759928) " % func, out, re.M)) != 2:
                fails.append("two functions seed %s: not two rows: %s" % (seed, out[-300:]))
        # 7. fill layering: an absolute fill (built once), a register-relative fill over part of it and a later absolute
        #    fill over part of that (both stay per input and win); setups read one word of each layer as their `this`
        #    (0x5D5D5D5D, 0x5E5E5E5E, 0x5C5C5C5C), so the layering decides where the counters are constructed
        layer = sead + ["fill * 0x5E400000 0x100 0x5D", "arg! %s r4 0x5E400040" % func, "fill * r4+0 0x40 0x5E",
                        "fill * 0x5E400050 0x10 0x5C", "setup %s 02759928 r3=*5E400010" % func,
                        "setup %s 02759928 r3=*5E400044" % func, "setup %s 02759928 r3=*5E400054" % func, "setupsnapshot"]
        for seed in ("1", "7"):
            same(fails, "fill layering seed %s" % seed, "sead", layer, ["-only", func, "-n", "500", "-seed", seed])
        t = [run("sead", layer, ["-only", func, "-n", "3", "-seed", "1", "-trace", "1000003"], fast)[1] for fast in (False, True)]
        if t[0] != t[1]:
            fails.append("fill layering: traced input differs with -fastreset:\n%s\n---\n%s" % (t[0][-600:], t[1][-600:]))
        for want in ("5D5D5D5D", "5E5E5E5E", "5C5C5C5C"):
            if "-> r3 %s" % want not in t[1]:
                fails.append("fill layering: no traced setup returns %s: %s" % (want, t[1][-600:]))
        # 8. self-check of the candidate side's validation state
        for name, spec, fn in (("fresh", fresh, "02759928"), ("spill", spill, "02759928"), ("const", const, func)):
            rc, out, err = run("sead", spec, ["-only", fn, "-n", "200", "-seed", "7", "-snapcheck", "1"], True, check=True)
            if rc != 0 or "fastreset check" in err:
                fails.append("check %s: rc %d %s" % (name, rc, err[-300:]))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        sys.path.insert(0, HERE)
        import verify  # noqa: E402  (the unit's build folders, also under WWHD_VERIFY_SCRATCH)
        for d in verify.unit_dirs_all(TMPN + "/sead"):
            shutil.rmtree(os.path.dirname(d), ignore_errors=True)  # the _fastreset_test folder
        shutil.rmtree(os.path.join(ROOT, "build", "verify", TMPN), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("fastreset_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
