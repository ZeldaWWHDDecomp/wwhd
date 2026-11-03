#!/usr/bin/env python3
"""Self-test of the guest-memory guard (gabi.h GABI_GUEST_GUARD, harness.cpp gmem_host_object; 2026-10-27, AGL#2).

A be<>/gptr<> is a view of guest memory: its accesses go to ea(this). Built as a HOST object (`gptr<T> x; x.v = a;`)
its store lands at host address - PPC_MEM_BASE, an arbitrary guest location. Verification builds (-DGABI_GUEST_GUARD=1
in verify.FP) end such a run at the first access with a diagnostic and exit status 86.

Uses lib_js_base (JKRFileLoaderList__dt 027EC058, `if (list == NULL) return;` first):
1. the unmodified source with the guard: passes, and its verify output is byte-identical to the same spec built without
   the guard (verify.FP without -DGABI_GUEST_GUARD=1; all but the `unit NAME` header line);
2. a candidate that stores through a host-local gptr<> before the NULL check: every input FAILs as a candidate-only
   abort (`only one side aborted: host-local be<>/gptr<> access (guard: ...)`, exit 1, the run completes: no longer exit 86, which lost
   every later row, 2026-10-27), stderr names `gabi: host-local be<>/gptr<>`, the function 027EC058 and the
   candidate side once; without the guard the same candidate builds and runs (the old silent behaviour);
3. the static probe (-DGABI_HOST_BE_PROBE=1 -fsyntax-only): the mutant fails to compile with be<>'s deleted default
   constructor, the unmodified source compiles;
4. gabi::guest_ref directly: guest addresses, addresses a candidate derives by 32-bit-wrapping arithmetic beyond the
   4 GB window or below PPC_MEM_BASE (c_bg_s GetTriGrp, c_bg_w MakeBlckMinMax, JAIZel*, JASDSPInterface,
   JASOuterParam index tables), NULL-object members (positive and negative offsets) are accepted; a host local, a host
   static and host heap blocks are not.
Writes a temporary unit directory and a mutant source next to the original and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_beguard_test")
F = "027EC058"
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
MUT = "wwhd_src/JSystem/JKernel/_beguard_test_JKRFileLoader.cpp"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
CHECK = "    if (list == NULL) return;\n"
HOSTLOCAL = "    { gptr<u8> wwhd_host_local; wwhd_host_local.v = 0x41000000u; }\n"
NOGUARD = ("import sys; sys.path.insert(0, %r); import verify; "
           "verify.FP = [f for f in verify.FP if not f.startswith('-DGABI_GUEST_GUARD')]; "
           "sys.argv = ['verify.py'] + sys.argv[1:]; verify.main()") % HERE


def spec(name, src):
    body = [("src " + src) if l.strip() == "src " + SRC else l for l in BASE]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + ["arg! %s r3 0,0x41000000" % F]) + "\n")
    return "_beguard_test/" + name


def body(out):
    """verify's output without its header line (`unit NAME: ... -> BUILD DIR` names the spec)"""
    return [l for l in out.splitlines() if not l.startswith("unit _beguard_test/")]


def run(unit, guard=True):
    args = [unit, "-only", F, "-n", "200", "-seed", "1", "-nulltrap", "-buildj", "4"]
    cmd = [sys.executable, os.path.join(HERE, "verify.py")] + args if guard else [sys.executable, "-c", NOGUARD] + args
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout, r.stderr


def probe(src):
    sys.path.insert(0, HERE)
    import verify
    out = os.path.join(TMP, "probe")
    os.makedirs(out, exist_ok=True)
    g = verify.glue(src, out)
    inc = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include", "-Ibuild/gen",
           "-I" + os.path.dirname(src)]
    r = subprocess.run(["clang++", "-std=c++20", "-fsyntax-only", "-w"] + verify.FP + inc + ["-DGABI_HOST_BE_PROBE=1", g],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stderr


REF = r"""
#include "gabi.h"
#include <cstdio>
static int host_static;
int main() {
    int local = 0;
    uint8_t* b = PPC_MEM_BASE;
    const void* yes[] = {b, b + 0x10000000, b + 0xFFFFFFFFull, b + 0x2771472B3ull, b + 0xE765E1F8ull * 3, b - 0x1000,
                         (void*)0x40, (void*)(uintptr_t)-8, nullptr};
    const void* no[] = {&local, &host_static, malloc(16), malloc(1 << 20), malloc(1 << 28)};
    int bad = 0;
    for (const void* p : yes) if (!gabi::guest_ref(p)) { printf("rejected %p\n", p); bad = 1; }
    for (const void* p : no) if (gabi::guest_ref(p)) { printf("accepted host %p\n", p); bad = 1; }
    return bad;
}
"""


def ref_check():
    d = os.path.join(TMP, "ref")
    os.makedirs(d, exist_ok=True)
    src, exe = os.path.join(d, "ref.cpp"), os.path.join(d, "ref")
    open(src, "w").write(REF)
    r = subprocess.run(["clang++", "-std=c++20", "-O2", "-w", "-Iruntime/include", "-Itools/verify/include", src, "-o", exe,
                        "-Wl,-undefined,dynamic_lookup"], cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        return r.returncode, r.stderr[-800:]
    r = subprocess.run([exe], capture_output=True, text=True)
    return r.returncode, r.stdout


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        text = open(os.path.join(ROOT, SRC)).read()
        if text.count(CHECK) != 1:
            raise SystemExit("mutation site not found once in %s" % SRC)
        open(os.path.join(ROOT, MUT), "w").write(text.replace(CHECK, HOSTLOCAL + CHECK))

        # 1. unmodified: passes with the guard, output identical to the build without it
        rc, out, err = run(spec("plain", SRC))
        rc0, out0, err0 = run(spec("plain_noguard", SRC), guard=False)
        if rc != 0 or "gabi: host-local" in err:
            fails.append("1: unmodified source with the guard: rc %d\n%s%s" % (rc, out[-600:], err[-600:]))
        elif rc0 != 0 or body(out) != body(out0):
            fails.append("1: output differs from the build without the guard (rc %d/%d)\n--- guard\n%s--- no guard\n%s"
                         % (rc, rc0, out[-800:], out0[-800:]))

        # 2. host-local gptr<> store: FAIL rows (candidate-only aborts) with the diagnostic once; without the guard the old
        # silent run
        rc, out, err = run(spec("hostlocal", MUT))
        want = ["gabi: host-local be<>/gptr<>", "in function %s" % F, "candidate side"]
        row = [l for l in out.splitlines() if l.startswith(F)]
        if rc != 1 or any(w not in err for w in want) or err.count("gabi: host-local") != 1 or len(row) != 1 or \
                "only one side aborted: host-local be<>/gptr<> access (guard" not in row[0] or " 0/" not in row[0]:
            fails.append("2: host-local store: rc %d (want 1, a FAIL row), rows %s, stderr\n%s" % (rc, row, err[-1200:]))
        rc0, out0, err0 = run(spec("hostlocal_noguard", MUT), guard=False)
        if rc0 == 86 or "gabi: host-local" in err0:
            fails.append("2: without the guard the run must not stop at the host-local store (rc %d)" % rc0)

        # 3. static probe
        prc, perr = probe(MUT)
        if prc == 0 or "deleted" not in perr:
            fails.append("3: probe accepted the host-local gptr<> (rc %d)\n%s" % (prc, perr[-800:]))
        prc, perr = probe(SRC)
        if prc != 0:
            fails.append("3: probe rejected the unmodified source\n%s" % perr[-800:])

        # 4. the accepted address ranges
        rrc, rout = ref_check()
        if rrc:
            fails.append("4: guest_ref ranges (rc %d)\n%s" % (rrc, rout))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
        sys.path.insert(0, HERE)
        import verify
        for u in ("plain", "plain_noguard", "hostlocal", "hostlocal_noguard"):
            for d in verify.unit_dirs_all("_beguard_test/" + u):
                shutil.rmtree(d, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("beguard_test: %s" % ("ok" if not fails else "%d failure(s)" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
