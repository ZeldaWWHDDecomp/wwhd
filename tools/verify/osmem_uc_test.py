#!/usr/bin/env python3
"""Self-test of `osmem ... uc` (harness.cpp namespace osm; opt-in part, not in `hle`): UCOpen / UCClose / UCReadSysConfig
as the port's runtime/src/hle/coreinit_misc.cpp answers them (no language setting: cafe.language = 1).
Carrier: the original 0286D670 (UCOpen, one 0x54-byte UCSysConfig item on its stack from (name, type, size, data),
UCReadSysConfig, UCClose; returns 1 on success) run as setups before sead::ResourceState::allocate 02756D18.
 1. "cafe.cntry_reg" with size 4 stores 00 00 00 31 (49) at the data pointer, "cafe.language" with size 1 stores 01,
    "parent.enable" with size 2 stores 00 00; UCOpen -> 1, UCReadSysConfig -> 0 (trace), each setup returns 1; rows ok.
 2. Without `uc` (`osmem hle` alone) the UC imports keep the generic mock (no osmem UC line).
 3. `uc` plus another mock directive on a UC import exits 2; `osmem uc` alone is accepted.
 4. The model against the port's own code: osmem_uc_equiv.py.
Spec files stay under the verify scratch (printed). Exit 0 when all pass."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "_osmem_uc_test/uc"
F = "02756D18"
TRACE = "1000003"


def words(addr, s):
    b = s.encode() + b"\0"
    b += b"\0" * (-len(b) % 4)
    return ["field * %08X u32 0x%s" % (addr + i, b[i:i + 4].hex().upper()) for i in range(0, len(b), 4)]


BASE = (["src wwhd_src/sead/PoolHeap.cpp", "src wwhd_src/sead/PoolHeapStatics.cpp", "src wwhd_src/sead/HeapRegistryStatics.cpp",
         "real 0286D670", "arg! 02756D18 r3 0x5A000000", "arg! 02756D18 r4 0x100"]
        + words(0x5C006000, "cafe.cntry_reg") + words(0x5C006100, "cafe.language") + words(0x5C006200, "parent.enable")
        + ["setup 02756D18 0286D670 r3=0x5C006000 r4=4 r5=4 r6=0x5C005000",
           "setup 02756D18 0286D670 r3=0x5C006100 r4=1 r5=1 r6=0x5C005100",
           "setup 02756D18 0286D670 r3=0x5C006200 r4=2 r5=2 r6=0x5C005200"])


def main():
    fails = []
    udir = os.path.join(HERE, "units", "_osmem_uc_test")
    os.makedirs(udir, exist_ok=True)
    with open(os.path.join(udir, "uc.txt"), "w") as f:
        f.write("\n".join(["# verify: -strictnan"] + BASE + ["osmem hle uc"]) + "\n")
    exe = verify.build(UNIT, jobs=8)
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="osmem_uc_test-", dir=base)
    print("osmem_uc_test: files in %s (not removed)" % W)

    def run(name, lines, args, n="200"):
        p = os.path.join(W, name + ".txt")
        with open(p, "w") as f:
            f.write("\n".join(BASE + lines) + "\n")
        r = subprocess.run([exe, "-spec", p, "-noprogress", "-strictnan", "-n", n, "-seed", "1", "-only", F] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr

    rc, out, err = run("uc", ["osmem hle uc"], ["-trace", TRACE], n="1")
    for w in ("nested osmem C000A430", "-> 00000001", "nested osmem C000A448", "-> 00000000", "nested osmem C000A408"):
        if w not in out:
            fails.append("1: trace lacks %r" % w)
    if out.count("setup 0286D670 -> r3 00000001") < 3:
        fails.append("1: a 0286D670 setup did not return 1")
    for addr, want in (("5C005000:4", ["00", "00", "00", "31"]), ("5C005100:1", ["01"]), ("5C005200:2", ["00", "00"])):
        rc, out, err = run("uc", ["osmem hle uc"], ["-watch", addr], n="1")
        got = [l.split()[3] for l in err.splitlines() if l.startswith("watch P")]
        if got[-len(want):] != want:
            fails.append("1: data at %s: %s, want %s" % (addr, got, want))
    rc, out, err = run("uc", ["osmem hle uc"], [])
    rows = [l for l in out.splitlines() if l.startswith(F)]
    if len(rows) != 1 or not rows[0].rstrip().endswith(" ok"):
        fails.append("1: row not ok: %s" % rows)
    rc, out, err = run("nouc", ["osmem hle"], ["-trace", TRACE], n="1")
    if "osmem C000A4" in out or "nested call C000A448" not in out:
        fails.append("2: without `uc` the UC imports should keep the generic mock")
    rc, out, err = run("conflict", ["osmem hle uc", "ret! C000A430 1"], [], n="1")
    if rc != 2 or "osmem models" not in err:
        fails.append("3: conflict not refused (rc %d)" % rc)
    rc, out, err = run("alone", ["osmem uc"], [], n="1")
    if rc != 0:
        fails.append("3: `osmem uc` alone refused (rc %d) %s" % (rc, err[-200:]))
    r = subprocess.run([sys.executable, os.path.join(HERE, "osmem_uc_equiv.py")], capture_output=True, text=True)
    if r.returncode != 0:
        fails.append("4: osmem_uc_equiv: %s" % r.stdout.strip()[-300:])
    for f in fails:
        print("FAIL", f)
    print("osmem_uc_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
