#!/usr/bin/env python3
"""Self-test: paired-single float stores carry the float mark (vm_gen.h vm_psq_store, 2026-11-18).

The recompiled original stores paired singles with psq_st / psq_stx. A 4-byte word (GQR store types 0-3: single float
bits) is a float store like stfs and must go through vm_stf32w, which sets the float mark the net-effect comparison's
NaN payload tolerance reads (harness.cpp same_net); before, it went through vm_st32 (no mark), so a matrix builder
(MtxTrans / PSMTX*) run `real` on a NaN showed a NaN-sign-only T-01 difference (Keese). Quantised words
(types 4-7: u8 / u16 / s8 / s16) stay integer stores. Compiles a small program with vm_gen.h against stub memory
functions that record the store kind, in a temporary folder. Exit 0 when all cases pass."""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

PROG = r'''
#include <stdio.h>
#include <string.h>
#include "vm_gen.h"
static char g_log[4096];
static void logf_(const char* k, uint32_t ea) { char b[64]; snprintf(b, sizeof b, "%s@%X ", k, ea); strcat(g_log, b); }
extern "C" {
uint8_t vm_ld8(uint32_t) { return 0; }
uint16_t vm_ld16(uint32_t) { return 0; }
uint32_t vm_ld32(uint32_t) { return 0; }
uint64_t vm_ld64(uint32_t) { return 0; }
void vm_st8(uint32_t ea, uint8_t) { logf_("st8", ea); }
void vm_st16(uint32_t ea, uint16_t) { logf_("st16", ea); }
void vm_st32(uint32_t ea, uint32_t) { logf_("st32", ea); }
void vm_st64(uint32_t ea, uint64_t) { logf_("st64", ea); }
void vm_call(Cpu*, uint32_t, int) {}
void vm_cov(uint32_t, uint32_t) {}
void vm_trap(Cpu*, uint32_t, uint32_t) {}
}
void vm_stf32w(uint32_t ea, uint32_t) { logf_("stf32", ea); }
void vm_stf64w(uint32_t ea, uint64_t) { logf_("stf64", ea); }
int main() {
    static Cpu c;
    c.f[1].ps0 = __builtin_nan("0x5"); c.f[1].ps1 = -1.5;
    const uint32_t types[] = {0, 4, 5, 6, 7};
    for (uint32_t t : types) {
        c.gqr[0] = t; /* store type in bits 0-2, scale 0 */
        g_log[0] = 0;
        vm_psq_store(&c, 1, 0x100, 0, 0);
        printf("pair type %u: %s\n", t, g_log);
        g_log[0] = 0;
        vm_psq_store(&c, 1, 0x200, 1, 0);
        printf("single type %u: %s\n", t, g_log);
    }
    return 0;
}
'''

WANT = {
    "pair type 0": "stf32@100 stf32@104",
    "single type 0": "stf32@200",
    "pair type 4": "st8@100 st8@101",
    "single type 4": "st8@200",
    "pair type 5": "st16@100 st16@102",
    "single type 5": "st16@200",
    "pair type 6": "st8@100 st8@101",
    "pair type 7": "st16@100 st16@102",
}


def main():
    tmp = tempfile.mkdtemp(prefix="psqmark_test_", dir=os.environ.get("T60_TEST_TMP") or None)
    fails = []
    try:
        src = os.path.join(tmp, "p.cpp")
        exe = os.path.join(tmp, "p")
        open(src, "w").write(PROG)
        cc = os.environ.get("CXX", "clang++")
        r = subprocess.run([cc, "-std=c++17", "-O1", "-I" + os.path.join(HERE, "include"), "-I" + os.path.join(ROOT, "runtime", "include"),
                            "-I" + os.path.join(ROOT, "build", "gen"), src, "-o", exe], capture_output=True, text=True)
        if r.returncode:
            fails.append("compile: " + r.stderr[-1500:])
        else:
            out = subprocess.run([exe], capture_output=True, text=True).stdout
            got = {l.split(":", 1)[0]: l.split(":", 1)[1].strip() for l in out.splitlines() if ":" in l}
            for k, v in WANT.items():
                ok = got.get(k) == v
                print("%s %s -> %s" % ("ok  " if ok else "FAIL", k, got.get(k)))
                if not ok:
                    fails.append("%s: want %s, got %s" % (k, v, got.get(k)))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)  # this test's own folder
    print("psqmark_test: %s" % ("FAIL" if fails else "PASS"))
    for f in fails:
        print("  " + f)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
