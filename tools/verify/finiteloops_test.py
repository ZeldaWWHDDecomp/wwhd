#!/usr/bin/env python3
"""Self-test of -fno-finite-loops in the verification builds (2026-10-29; RI91 / AGL#18 #1709).

C++ lets the compiler assume that a loop without side effects terminates (forward-progress rule), so clang deleted a
side-effect-free loop that never ends: AGL#18 mutant 91 / global 1709 drops `last = link->mNext` from a read-only list
walk (RangePoolRelease.cpp:109), the guest would hang on every input that enters the walk, but the host build returned
normally and the mutant survived. verify.FP now has -fno-finite-loops (verify.py, mutate.py, mutcode.py, objcmp.py all
compile with it), so such a loop stays a loop.

Uses lib2_sead_15's constructWorkerCounter 02759928 (WorkerRuntimeInstances.cpp) with a mutant of the same shape as
RI91: a list walk whose progress statement (its only guest read) is dropped (`address` never changes):
a. with the flag (verify.FP as shipped), built at -O1 like mutate.py's mutants: the candidate's run is ended by the CPU
   time budget (`time budget exceeded`), the row is a candidate-only abort (mutate.classify: `abort`), and a run under a
   mutate.py per-run limit below the budget hits it (mutate.run_with_retry: no result, class `timeout`): the
   non-termination record. The object of the mutant differs from the unmutated object (mutcode equivalence: no EQ);
b. control (verify.FP without the flag, the old build): the same mutant finishes with the row `ok` (the hang is invisible);
c. normal code: the unmodified source and a variant with a terminating side-effect-free loop give byte-identical rows
   with and without the flag (n 300, seeds 1 and 7, with -nulltrap too).
Writes a temporary unit directory (tools/verify/units/_finiteloops_test) and mutant sources next to the original
(overwritten by the next run, never deleted: remove them by hand when no longer wanted). Builds go below
WWHD_VERIFY_SCRATCH when set. Exit 0 when all pass. Takes about 2 min (two runs spend the 30 s CPU budget).
"""
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_finiteloops_test")
F = "02759928"
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT = {"hang": "wwhd_src/sead/_finiteloops_test_hang_WorkerRuntimeInstances.cpp",
       "bounded": "wwhd_src/sead/_finiteloops_test_bounded_WorkerRuntimeInstances.cpp"}
SITE = "    if(object!=NULL) object->mValue=0;\n"
# RI91's shape: a list walk whose progress statement (the only guest read, `address = word->mValue;`) is dropped, so
# `address` never changes and the body has no side effect (a guest read would count against the access budget). The walk
# is entered only on some inputs (a misaligned `object`, as the generated input 0 of seed 1): a loop the compiler can
# prove infinite on every path makes the whole branch unreachable instead, which is not RI91's case
HANG = ("    if(object!=NULL) {\n"
        "        u32 address = gabi::ea(object) & 3;\n"
        "        while (address != 0) {\n"
        "            CounterWord* word = gabi::at<CounterWord>(address);\n"
        "            (void)word;\n"
        "        }\n"
        "        object->mValue=0;\n"
        "    }\n")
BOUNDED = ("    if(object!=NULL) {\n"
           "        u32 sum = 0;\n"
           "        for (u32 i = 0; i < 1000; i++) sum += gabi::at<CounterWord>(gabi::ea(object))->mValue;\n"
           "        (void)sum;\n"
           "        object->mValue=0;\n"
           "    }\n")
FLAG = "-fno-finite-loops"
NOFLAG = ("import sys; sys.path.insert(0, %r); import verify; "
          "verify.FP = [f for f in verify.FP if f != %r]; "
          "sys.argv = ['verify.py'] + sys.argv[1:]; verify.main()") % (HERE, FLAG)
HDR = ["-strictnan", "-calleesp", "-footprint"]  # lib2_sead_15's `# verify:` header


def spec(name, src):
    os.makedirs(TMP, exist_ok=True)
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write(open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + src))
    return "_finiteloops_test/" + name


def run(unit, flag, args):
    head = [sys.executable, os.path.join(HERE, "verify.py")] if flag else [sys.executable, "-c", NOFLAG]
    t = time.time()
    r = subprocess.run(head + [unit, "-only", F, "-noprogress", "-buildj", "4"] + args, cwd=ROOT, capture_output=True, text=True)
    rows = [l for l in r.stdout.splitlines() if l.startswith(F)]
    return r.returncode, rows, r.stdout, r.stderr, time.time() - t


def main():
    import verify
    import mutate
    import mutcode
    if FLAG not in verify.FP:
        print("FAIL verify.FP lacks %s" % FLAG)
        return 1
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(SITE) != 1:
        raise SystemExit("mutation site not found once in %s" % SRC)
    for k, body in (("hang", HANG), ("bounded", BOUNDED)):
        open(os.path.join(ROOT, MUT[k]), "w").write(text.replace(SITE, body))
    fails = []

    # a. the hanging mutant with the flag: the CPU time budget ends the candidate's run
    u = spec("hang", MUT["hang"])
    rc, rows, out, err, dt = run(u, True, ["-n", "1", "-srcopt", "O1"] + HDR)
    cls = mutate.classify(rc, out + err) if rc else "survived"
    print("a. hanging mutant, -fno-finite-loops (-O1): rc %d, %.0f s, class %s\n   %s" % (rc, dt, cls, rows[0] if rows else out[-300:]))
    if not rows or "time budget exceeded" not in rows[0] or cls != "abort":
        fails.append("hang with the flag: want a candidate-only abort by the CPU time budget, got rc %d %s" % (rc, rows or out[-400:]))
    # a2. the same run under a mutate.py per-run limit below the CPU budget: a timeout (no result)
    exe = os.path.join(verify.unit_dir(u), "verify.O1")
    import specfile
    cmd = [exe, "-spec", specfile.spec_path(u, ROOT, verify.unit_dir(u)), "-n", "1", "-seed", "1", "-only", F, "-stopfirst"] + HDR
    t = time.time()
    r, tried = mutate.run_with_retry(cmd, 8, False, cwd=ROOT, capture_output=True, text=True,
                                     env=dict(os.environ, WWHD_VERIFY_PROGRESS="0", WWHD_VERIFY_HANG_LIMIT="0"))
    print("   under an 8 s per-run limit: %s after %.0f s (limits tried %s)" % ("timeout" if r is None else "rc %d" % r.returncode,
                                                                            time.time() - t, tried))
    if r is not None:
        fails.append("hang under an 8 s limit: want a timeout, the run finished rc %d %s" % (r.returncode, r.stdout[-300:]))
    # a3. the mutant's object is not the unmutated object (no EQ by object identity)
    try:
        flags = ["-std=c++20", "-O1", "-w"] + verify.FP + ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src",
                                                           "-Iwwhd_src/include", "-Ibuild/gen"]
        objs = {}
        for k, s in (("base", SRC), ("hang", MUT["hang"])):
            o = os.path.join(verify.unit_dir(u), "_finiteloops_%s.o" % k)
            subprocess.run(["clang++"] + flags + ["-I" + os.path.dirname(os.path.join(ROOT, SRC)), "-c", verify.glue(s, verify.unit_dir(u)),
                            "-o", o], cwd=ROOT, check=True, capture_output=True)
            objs[k] = mutcode.object_digest(o)
        same = objs["base"] == objs["hang"]
        print("   object identical to the unmutated one: %s" % same)
        if same:
            fails.append("hang with the flag: the mutant compiles to the unmutated object (loop deleted)")
    except (subprocess.CalledProcessError, OSError) as e:
        fails.append("object compare failed: %s" % e)

    # b. control: the old build (no flag) deletes the loop, the mutant passes
    u0 = spec("hang_noflag", MUT["hang"])
    rc, rows, out, err, dt = run(u0, False, ["-n", "1", "-srcopt", "O1"] + HDR)
    print("b. hanging mutant, old flags (-O1): rc %d, %.0f s\n   %s" % (rc, dt, rows[0] if rows else out[-300:]))
    if rc or not rows or " ok" not in rows[0]:
        fails.append("hang without the flag: want the old `ok` (loop deleted), got rc %d %s" % (rc, rows or out[-400:]))

    # c. normal code: identical rows with and without the flag
    for k, src in (("base", SRC), ("bounded", MUT["bounded"])):
        for seed in ("1", "7"):
            for nt in ([], ["-nulltrap"]):
                args = ["-n", "300", "-seed", seed] + nt + HDR
                a1 = run(spec(k + "_flag", src), True, args)
                a0 = run(spec(k + "_noflag", src), False, args)
                body = lambda o: [l for l in o.splitlines() if not l.startswith("unit _finiteloops_test/")]
                same = a1[0] == a0[0] and body(a1[2]) == body(a0[2])
                print("c. %-7s seed %s%s: %s  %s" % (k, seed, " -nulltrap" if nt else "", "identical" if same else "DIFFERENT",
                                                     a1[1][0] if a1[1] else a1[2][-200:]))
                if not same or a1[0]:
                    fails.append("normal code %s seed %s %s: want the same passing rows, got %s / %s" % (k, seed, nt, a1[1], a0[1]))
    for f in fails:
        print("FAIL", f)
    print("finiteloops_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
