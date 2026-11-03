#!/usr/bin/env python3
"""Self-test of the unit-file `setupsnapshot` directive (harness.cpp, 2026-10-26, AGL#3): the `setup` lines of a function
run once per process, every input starts from the captured post-setup state (pokes, postsetups and the function as before).

1. Merged lib2_agl_16_render_context (14 setups over two functions, header -only 027C424C,027C4538). As committed its
   setups depend on the input: registerLayerGlobal 028F026C links the registration into the list head 104DA130 and
   further setups read six unsteered globals at 101FD304..101FDEC4, all generated per input, so the snapshot is refused
   (checked). With those seven words pinned (`field * ADDR u32 0`) it is accepted: stdout and stderr are byte-identical
   with and without `setupsnapshot` at seeds 1 and 7 (n=1000), and the snapshot runs take less CPU.
   A traced input prints `setupsnapshot: installed (N bytes)`; input 0 (validated) prints `setupsnapshot: validated`.
2. Merged lib2_sead_15 (tested function 02759564) with `setup 02759564 02759928 r3=0x5E000000` (a counter constructor):
   identical output with and without the directive (n=2000, seed 1).
3. Input-dependent setups are refused and fall back to per-input setups (output identical to the run without the
   directive, seeds 1 and 7): (a) an operand `r3=*5E100000` over a per-input `field` (static check: `setupsnapshot: refused
   (setup result depends on the input: setup 02759928 operand r3=*5E100000 reads a per-input `field`)`); (b) a setup whose
   nested allocation is a per-input `ret!` pick (run-twice check: `... first difference at ADDR)`).
4. A per-input pick that is the same for both check seeds but different for input 0: the twice-run check passes, the
   validation of input 0 against the real setups fails and the run exits 2.
   4b. Fresh snapshot per input: the tested function overwrites setup-written memory or advances the setup's allocation
   cursor; traces of late inputs (150..153, 298, 299 of 300) are identical with and without the snapshot.
   4c. A setup routine that saves per-input non-volatile registers (`footprint`) on its own scratch stack: accepted (the
   window below the entry SP + 8 is not snapshot state), output identical with and without the snapshot, seeds 1 and 7.
5. `setupsnapshot 1` (an operand) exits 2.
Writes temporary unit files under tools/verify/units/_setupsnapshot_test/ and removes them (and their build folders).
Exit 0 when all cases pass. `--quick` skips case 1 (the render_context runs take a few minutes).
"""
import os
import re
import shutil
import subprocess
import sys
import resource

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMPN = "_setupsnapshot_test"
TMP = os.path.join(UNITS, TMPN)
M64 = (1 << 64) - 1


def mix(x):  # harness.cpp mix()/hash3(): the field pick of an input
    x = (x + 0x9E3779B97F4A7C15) & M64
    x = ((x ^ (x >> 30)) * 0xBF58476D1CE4E5B9) & M64
    x = ((x ^ (x >> 27)) * 0x94D049BB133111EB) & M64
    return x ^ (x >> 31)


def hash3(a, b, c):
    return mix(a ^ mix(b ^ mix(c)))


def pick(seed, addr, n):
    return (hash3(seed, addr, 0xF1E1D) >> 5) % n


def run(name, lines, args):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    t = resource.getrusage(resource.RUSAGE_CHILDREN).ru_utime
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), TMPN + "/" + name, "-shards", "1", "-buildj", "6"] +
                       list(args), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout, r.stderr, resource.getrusage(resource.RUSAGE_CHILDREN).ru_utime - t  # CPU s (user)


def main():
    quick = "--quick" in sys.argv[1:]
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        # 1. render_context: identical, faster
        if not quick:
            rc_spec = open(os.path.join(UNITS, "lib2_agl_16_render_context.txt")).read().splitlines()
            if sum(1 for l in rc_spec if l.startswith("setup ")) < 2:
                fails.append("lib2_agl_16_render_context has no setup lines any more")
            rc, out, err, _ = run("rc", rc_spec + ["setupsnapshot"], ["-n", "1", "-seed", "1"])
            if "setupsnapshot: refused (setup result depends on the input: first difference at 101F95D8)" not in err:
                fails.append("render_context as committed: input-dependent setups not refused at 101F95D8: %s" % err[-300:])
            rc_spec = rc_spec + ["field * %s u32 0" % a for a in
                                 ("104DA130", "101FD304", "101FD308", "101FD69C", "101FD98C", "101FDEC0", "101FDEC4")]
            run("rc", rc_spec, ["-n", "1", "-seed", "1"])  # build first: the timed runs only run
            for seed in ("1", "7"):
                rc0, out0, err0, t0 = run("rc", rc_spec, ["-n", "1000", "-seed", seed])
                rc1, out1, err1, t1 = run("rc", rc_spec + ["setupsnapshot"], ["-n", "1000", "-seed", seed])
                print("render_context seed %s: without %.2f s, with setupsnapshot %.2f s CPU (x%.1f)" % (seed, t0, t1, t0 / max(t1, 1e-3)))
                strip = lambda e: [l for l in e.splitlines() if l.startswith(("semantic effects", "setupsnapshot"))]
                if strip(err0) != strip(err1):
                    fails.append("render_context seed %s: stderr differs:\n%s\n---\n%s" % (seed, err0[-400:], err1[-400:]))
                if rc0 != rc1 or out0 != out1:
                    fails.append("render_context seed %s: output differs with setupsnapshot (rc %d/%d)\n%s\n---\n%s" %
                                 (seed, rc0, rc1, out0[-600:], out1[-600:]))
                if "refused" in err1:
                    fails.append("render_context seed %s: snapshot refused: %s" % (seed, err1[-300:]))
                if t1 >= t0:
                    fails.append("render_context seed %s: not faster (%.1f s vs %.1f s)" % (seed, t1, t0))
            rc, out, err, _ = run("rc", rc_spec + ["setupsnapshot"], ["-n", "3", "-seed", "1", "-trace", "1000004"])
            if not re.search(r"setupsnapshot: installed \(\d+ bytes\)", out):
                fails.append("traced input 1 does not print `setupsnapshot: installed (N bytes)`: %s" % out[-300:])
            rc, out, err, _ = run("rc", rc_spec + ["setupsnapshot"], ["-n", "3", "-seed", "1", "-trace", "1000003"])
            if "setupsnapshot: validated" not in out:
                fails.append("traced input 0 does not print `setupsnapshot: validated`: %s" % out[-300:])

        sead = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
        func = "02759564"
        if "real 02759928" not in sead:
            fails.append("lib2_sead_15 no longer lists real 02759928")
        # 2. a constant setup: identical
        on = sead + ["setup %s 02759928 r3=0x5E000000" % func]
        rc0, out0, _, _ = run("sead", on, ["-only", func, "-n", "2000", "-seed", "1"])
        rc1, out1, err1, _ = run("sead", on + ["setupsnapshot"], ["-only", func, "-n", "2000", "-seed", "1"])
        if rc0 or out0 != out1 or "refused" in err1:
            fails.append("lib2_sead_15 setup: output differs or refused (rc %d/%d)\n%s\n---\n%s%s" % (rc0, rc1, out0[-300:], out1[-300:], err1[-300:]))
        # 3a. a setup operand that reads a per-input field: refused by the static check, falls back
        choices = ",".join("0x5E0000%02X" % (16 * k) for k in range(8))
        dep = sead + ["field %s 5E100000 u32 %s" % (func, choices), "setup %s 02759928 r3=*5E100000" % func]
        # 3b. a setup routine whose nested allocation result is a per-input pick (`ret!` on the allocator, not visible in
        #     the operands): refused by the run-twice check with the first differing address
        picks = ",".join("0x5E0%02X000" % k for k in range(7))  # 7 picks: the two check seeds differ at seeds 1 and 7
        dep2 = sead + ["ret! 0273AD10 %s" % picks, "setup %s 02759928 r3=0" % func]
        for name, spec, want in (("static", dep, r"setupsnapshot: refused \(setup result depends on the input: setup 02759928 "
                                                 r"operand r3=\*5E100000 reads a per-input `field`\)"),
                                 ("twice", dep2, r"setupsnapshot: refused \(setup result depends on the input: first difference at "
                                                 r"[0-9A-F]{8}\)")):
            for seed in ("1", "7"):
                rc0, out0, _, _ = run("sead", spec, ["-only", func, "-n", "1000", "-seed", seed])
                rc1, out1, err1, _ = run("sead", spec + ["setupsnapshot"], ["-only", func, "-n", "1000", "-seed", seed])
                if not re.search(want, err1):
                    fails.append("%s seed %s: input-dependent setup not refused: %s" % (name, seed, err1[-300:]))
                if rc0 != rc1 or out0 != out1:
                    fails.append("%s seed %s: fallback output differs (rc %d/%d)\n%s\n---\n%s" % (name, seed, rc0, rc1, out0[-300:], out1[-300:]))
        # 4. the check seeds agree by chance, input 0 does not (2 allocator picks, seed 3, computed with the harness hash):
        #    the twice-run check passes, the validation of input 0 against the real setups fails and the run exits 2
        def ret_pick(seed, n):
            return (hash3(seed, 0x0273AD10, 0xABCDEF) >> 13) % n
        s0 = 3 * 1000003
        ca, cb = s0 + 0x80000000, s0 + 0x80000000 + 0x9E37
        if not (ret_pick(ca, 2) == ret_pick(cb, 2) != ret_pick(s0, 2)):
            fails.append("case 4 seed choice no longer gives agreeing check seeds")
        val = sead + ["ret! 0273AD10 0x5E000000,0x5E000040", "setup %s 02759928 r3=0" % func, "setupsnapshot"]
        rc, out, err, _ = run("sead", val, ["-only", func, "-n", "10", "-seed", "3"])
        if rc != 2 or "setupsnapshot: validation failed at input 0" not in err:
            fails.append("validation mismatch not fatal (rc %d): %s" % (rc, err[-300:]))
        # 4b. fresh snapshot per input: the setup allocates a counter (steered allocator) and stores the pointer at 5E000000;
        #     the tested constructWorkerCounter 02759928 either clears the word at 5E000000 (r3=0x5E000000: a change only
        #     against the pristine snapshot) or allocates the next block (r3=0: 5D000020 only from the pristine cursor).
        #     Traces of late inputs (every access, call, allocation and the net effect) must equal the per-input setups'.
        fresh = sead + ["alloc 0273AD10 r3 0x5D000000 32", "allocnested 0273AD10", "setup 02759928 02759928 r3=0 ->5E000000",
                        "arg! 02759928 r3 0x5E000000,0"]
        rc0, out0, _, _ = run("sead", fresh, ["-only", "02759928", "-n", "300", "-seed", "1"])
        rc1, out1, err1, _ = run("sead", fresh + ["setupsnapshot"], ["-only", "02759928", "-n", "300", "-seed", "1"])
        if rc0 or out0 != out1 or "refused" in err1:
            fails.append("fresh: output differs or refused (rc %d/%d)\n%s\n---\n%s%s" % (rc0, rc1, out0[-300:], out1[-300:], err1[-300:]))
        noise = re.compile(r"setup|nested (alloc|call|return)")
        seen = set()
        for i in (150, 151, 152, 153, 298, 299):
            t = []
            for spec in (fresh, fresh + ["setupsnapshot"]):
                _, out, _, _ = run("sead", spec, ["-only", "02759928", "-n", "300", "-seed", "1", "-trace", str(1000003 + i)])
                t.append([l for l in out.splitlines() if not noise.search(l)])
            if t[0] != t[1]:
                fails.append("fresh: trace of input %d differs with setupsnapshot:\n%s\n---\n%s" % (i, "\n".join(t[0][-15:]), "\n".join(t[1][-15:])))
            seen.update(l.strip() for l in t[1] if "alloc 0273AD10 ->" in l or l.strip().startswith("5E000000:"))
        if not any("alloc 0273AD10 -> 5D000020" in l for l in seen) or not any(l.startswith("5E000000:") for l in seen):
            fails.append("fresh: the traced inputs did not exercise both paths (pristine cursor / pristine word): %s" % sorted(seen))
        # 4c. per-input bytes on the setups' own scratch stack are not setup state (AGL#18, 7EFF6E60 `1F vs F3`): with
        #     `footprint` the tested function's inputs get per-seed non-volatile registers r14..r31, and the setup routine
        #     parseWidth 02759EE8 saves r30/r31 into its frame 0x8000 below the entry SP. Its heap effect (this->mWidth = 12
        #     from the constant text "12") is the same in every input, so the snapshot is accepted (it was refused at a
        #     setup-stack address before the scratch window was excluded) and the output is identical, seeds 1 and 7.
        spill = sead + ["footprint 02759928 8", "arg! 02759928 r3 0x5E000000", "field * 5E300000 u16 0x31",
                        "field * 5E300002 u16 0x32", "field * 5E300004 u16 0", "setup 02759928 02759EE8 r3=0x5E200000 r4=0x5E300000"]
        if "real 02759EE8" not in sead:
            fails.append("lib2_sead_15 no longer lists real 02759EE8")
        for seed in ("1", "7"):
            rc0, out0, err0, _ = run("sead", spill, ["-only", "02759928", "-n", "1000", "-seed", seed])
            rc1, out1, err1, _ = run("sead", spill + ["setupsnapshot"], ["-only", "02759928", "-n", "1000", "-seed", seed, "-v"])
            if "refused" in err1 or "setupsnapshot: 02759928 captured" not in err1:
                fails.append("spill seed %s: snapshot not accepted: %s" % (seed, err1[-300:]))
            if rc0 != rc1 or out0 != out1 or not re.search(r"^02759928 .* ok$", out0, re.M):
                fails.append("spill seed %s: output differs or fails (rc %d/%d)\n%s\n---\n%s" % (seed, rc0, rc1, out0[-300:], out1[-300:]))
        _, out, _, _ = run("sead", spill + ["setupsnapshot"], ["-only", "02759928", "-n", "3", "-seed", "1", "-trace", "1000004"])
        if not re.search(r"setupsnapshot: installed \(\d+ bytes\)", out):
            fails.append("spill: traced input does not install the snapshot: %s" % out[-300:])
        # 5. malformed
        rc, _, _, _ = run("sead", sead + ["setupsnapshot 1"], ["-only", func, "-n", "1"])
        if rc != 2:
            fails.append("`setupsnapshot 1` not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", TMPN), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("setupsnapshot_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
