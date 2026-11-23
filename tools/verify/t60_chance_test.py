#!/usr/bin/env python3
"""Self-test of the true60 per-step chance helpers (the per-step chance rule (8); wwhd_src/include/true60.h t60_chance / t60_chance_in).

1. dt = 1 returns the given chance / threshold bit for bit (100,000 values, incl. 0, 1, negatives, > 1, NaN);
2. N = 2, 4, 8 draws at dt = 1/N fire at least once with the per-frame probability: 1 - (1 - p')^N = p (|err| < 2e-6),
   and the threshold form gives range * that chance;
3. a Monte Carlo run (fixed seed, 2,000,000 frames at N = 2 and 4) lands within 0.1 percentage points of p.
Compiles a small host program with clang++ into a temporary directory (removed at the end). Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
PROG = r'''
#include "true60.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <random>
extern "C" { float wwhd_t60_dt = 1.0f; uint32_t wwhd_t60_tick30 = 1, wwhd_t60_steps = 1, wwhd_t60_first = 1, wwhd_t60_stepk = 1; }
static bool same(float a, float b) { return !memcmp(&a, &b, 4); }
int main() {
    int bad = 0;
    std::mt19937 g(1);
    std::uniform_real_distribution<float> u(-0.5f, 1.5f);
    float specials[] = {0.0f, -0.0f, 1.0f, 0.5f, 1e-30f, 0.999999f, -1.0f, 2.0f, NAN};
    for (int i = 0; i < 100000 + 9; i++) {
        float p = i < 9 ? specials[i] : u(g), r = i < 9 ? 100.0f : u(g) * 200.0f;
        if (!same(t60_chance_dt(p, 1.0f), p) || !same(t60_chance_in_dt(p * r, r, 1.0f), p * r)) {
            if (bad++ < 5) printf("dt=1 changed %g\n", p);
        }
        wwhd_t60_dt = 1.0f;
        if (!same(t60_chance(p), p)) bad++;
    }
    printf("dt=1 identity: %s\n", bad ? "FAIL" : "ok");
    int bad2 = 0;
    for (int n : {2, 4, 8}) {
        for (int i = 1; i < 1000; i++) {
            float p = i / 1000.0f, q = t60_chance_dt(p, 1.0f / n);
            double back = 1.0 - pow(1.0 - (double)q, n);
            float qi = t60_chance_in_dt(p * 100.0f, 100.0f, 1.0f / n);
            if (fabs(back - p) > 2e-6 || fabs(qi - 100.0f * q) > 2e-4) { if (bad2++ < 5) printf("N=%d p=%g back %.9g\n", n, p, back); }
        }
    }
    printf("N-step probability: %s\n", bad2 ? "FAIL" : "ok");
    int bad3 = 0;
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    for (int n : {2, 4}) {
        for (float p : {0.02f, 0.3f, 0.75f}) {
            long hit = 0, frames = 2000000;
            float q = t60_chance_dt(p, 1.0f / n);
            for (long f = 0; f < frames; f++) {
                bool any = false;
                for (int k = 0; k < n; k++) any |= d(g) < q;
                hit += any;
            }
            double rate = (double)hit / frames;
            printf("N=%d p=%.2f: %.5f\n", n, p, rate);
            if (fabs(rate - p) > 0.001) bad3++;
        }
    }
    printf("Monte Carlo: %s\n", bad3 ? "FAIL" : "ok");
    return bad || bad2 || bad3;
}
'''


def main():
    tmp = tempfile.mkdtemp(prefix="t60_chance_")
    try:
        src = os.path.join(tmp, "t.cpp")
        with open(src, "w") as f:
            f.write(PROG)
        exe = os.path.join(tmp, "t")
        cc = subprocess.run(["clang++", "-std=gnu++20", "-O1", "-ffp-contract=off", "-I", os.path.join(ROOT, "wwhd_src/include"),
                             "-I", os.path.join(HERE, "include"), src, "-o", exe], capture_output=True, text=True)
        if cc.returncode:
            print(cc.stderr[-2000:])
            print("t60_chance_test: FAIL (compile)")
            return 1
        r = subprocess.run([exe], capture_output=True, text=True)
        print(r.stdout, end="")
        print("t60_chance_test: %s" % ("PASS" if r.returncode == 0 else "FAIL"))
        return 1 if r.returncode else 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
