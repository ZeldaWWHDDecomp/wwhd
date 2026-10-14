#!/usr/bin/env python3
"""Run test of gabi::guest_str, the host string literal -> guest copy lookup (gabi.h).

Several glued sources can share one registry (the port's decomp build links a game unit's sources
together), so the lookup order is: (a) the registration pinned to the running function (`in FUNC`),
wherever it is in the list; (b) else the unpinned registrations of the running function's own file
(gabi::GuestFile); (c) else every unpinned registration. A choice between different addresses within
(b), or within (c) when (b) is empty, still stops with "ambiguous string".

Cases: pinned entry later in the list than two clashing unpinned ones; same-file preference (two files
register one text at different addresses); real same-file ambiguity (fatal); real cross-file ambiguity
from a function of neither file (fatal); a string no file registers (fatal); the per-call-site cache
(keyed by the running function, a host buffer reused with other text); EntryScope restores the
caller's running function after a nested entry.
`--bench`: also time the lookup (old scan, new scan, cached) on a registry of the largest port unit's size.
Exit 0 when every case passes.
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

SRC = r"""
#include "wwhd.h"
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
namespace gabi { thread_local Cpu* cpu; } /* defined by the harness (harness.cpp) */
using gabi::GuestFile;
using gabi::GuestStr;

/* registration order = definition order; the list head is the last one, so the pinned entry
 * defined first is the LAST one the scan reaches */
static const u32 fa_funcs[] = {0x02000100, 0x02000110};
static const u32 fb_funcs[] = {0x02000200};
static const GuestFile file_a{fa_funcs, 2}, file_b{fb_funcs, 1};
static GuestStr pin_c(0x10000003, "InAll", 0x02000300, nullptr);  /* `str ... in 02000300` (other file) */
static GuestStr un_a(0x10000001, "InAll", 0, &file_a);
static GuestStr un_b(0x10000002, "InAll", 0, &file_b);
static GuestStr dup_a1(0x10000005, "Dup", 0, &file_a);
static GuestStr dup_a2(0x10000006, "Dup", 0, &file_a);
static GuestStr only_b(0x10000007, "OnlyB", 0, &file_b);
static GuestStr pin_a(0x10000008, "Pinned", 0x02000110, &file_a);
static GuestStr un_a3(0x10000009, "Pinned", 0, &file_a);

static int fails = 0;
static void expect(const char* what, u32 got, u32 want) {
    if (got != want) { std::printf("FAIL %s: 0x%08X, expected 0x%08X\n", what, got, want); fails++; }
}
static u32 as(u32 fn, const char* s) { gabi::Activation::current = fn; return gabi::guest_str(s); }

/* the lookup before this change, for --bench */
static u32 old_guest_str(const char* s) {
    const GuestStr* any = nullptr;
    for (const GuestStr* g = GuestStr::head; g; g = g->next)
        if (!std::strcmp(g->text, s)) {
            if (g->func == gabi::current_function()) return g->addr;
            if (!g->func) any = g;
        }
    return any ? any->addr : 0;
}

int main(int argc, char** argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0); /* glue_fatal prints, then traps */
    std::string mode = argc > 1 ? argv[1] : "ok";
    if (mode == "same_file_ambiguity") { as(0x02000100, "Dup"); return 0; }
    if (mode == "cross_file_ambiguity") { as(0x02000900, "InAll"); return 0; }
    if (mode == "missing") { as(0x02000100, "Nowhere"); return 0; }
    /* (a) pinned, behind the two clashing unpinned registrations */
    expect("pinned later in the list", as(0x02000300, "InAll"), 0x10000003);
    expect("pinned beats same-file unpinned", as(0x02000110, "Pinned"), 0x10000008);
    expect("same file when the pin is for another function", as(0x02000100, "Pinned"), 0x10000009);
    /* (b) same-file preference */
    expect("same file A", as(0x02000100, "InAll"), 0x10000001);
    expect("same file A, second function", as(0x02000110, "InAll"), 0x10000001);
    expect("same file B", as(0x02000200, "InAll"), 0x10000002);
    /* (c) one unpinned copy in the unit: any function finds it */
    expect("unique unpinned from another file", as(0x02000100, "OnlyB"), 0x10000007);
    expect("unique unpinned outside every file", as(0, "OnlyB"), 0x10000007);
    /* cache: one call site, the running function changes */
    for (int i = 0; i < 3; i++) {
        expect("cached A", as(0x02000100, "InAll"), 0x10000001);
        expect("cached B", as(0x02000200, "InAll"), 0x10000002);
        expect("cached pinned", as(0x02000300, "InAll"), 0x10000003);
    }
    /* cache: a host buffer reused with other text */
    char buf[16];
    std::strcpy(buf, "InAll");
    expect("buffer InAll", as(0x02000200, buf), 0x10000002);
    std::strcpy(buf, "OnlyB");
    expect("buffer reused", as(0x02000200, buf), 0x10000007);
    /* EntryScope: after a nested entry the caller's pins apply again */
    {
        Cpu c{};
        gabi::Activation::current = 0x02000300;
        {
            gabi::EntryScope scope(&c);
            gabi::Activation::current = 0x02000200;
        }
        expect("EntryScope restores the running function", gabi::current_function(), 0x02000300);
        expect("pin after nested entry", gabi::guest_str("InAll"), 0x10000003);
    }
    if (mode == "bench") {
        /* a registry of N strings, the looked-up ones spread over it */
        int n = argc > 2 ? std::atoi(argv[2]) : 4000;
        std::vector<std::string> texts;
        for (int i = 0; i < n; i++) texts.push_back("bench_string_" + std::to_string(i));
        for (int i = 0; i < n; i++) new GuestStr(0x11000000 + 4 * i, texts[i].c_str(), 0, nullptr);
        std::vector<const char*> keys;
        for (int i = 0; i < 64; i++) keys.push_back(texts[(i * 7919) % n].c_str());
        gabi::Activation::current = 0x02000100;
        auto t = [&](auto f) {
            const int reps = 20000;
            u32 sink = 0;
            auto t0 = std::chrono::steady_clock::now();
            for (int r = 0; r < reps; r++) sink += f(keys[r & 63]);
            double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - t0).count() / reps;
            return ns + (sink == 1 ? 1 : 0);
        };
        std::printf("bench n=%d: old scan %.0f ns, new scan %.0f ns, new cached %.1f ns per lookup\n", n,
                    t(old_guest_str), t([](const char* s) { return gabi::guest_str_scan(s, 0x02000100)->addr; }),
                    t([](const char* s) { return gabi::guest_str(s); }));
    }
    if (fails) return 1;
    std::printf("cases ok\n");
    return 0;
}
"""


def main():
    cxx = os.environ.get("CXX", "clang++")
    bench = "--bench" in sys.argv
    with tempfile.TemporaryDirectory(prefix="gabistr-") as tmp:
        cpp, exe = os.path.join(tmp, "t.cpp"), os.path.join(tmp, "t")
        open(cpp, "w").write(SRC)
        inc = ["-I" + os.path.join(ROOT, d) for d in ("runtime/include", "tools/verify/include", "tools/verify/src", "wwhd_src/include")]
        r = subprocess.run([cxx, "-std=c++20", "-O2", "-w"] + inc + [cpp, "-o", exe], capture_output=True, text=True)
        if r.returncode:
            print(r.stderr.strip()[:3000])
            print("gabi_strlookup_test: FAIL (build)")
            return 1
        bad = 0
        r = subprocess.run([exe] + (["bench", sys.argv[sys.argv.index("--bench") + 1]] if bench and len(sys.argv) > sys.argv.index("--bench") + 1 else ["bench"] if bench else []),
                           capture_output=True, text=True)
        print(r.stdout.strip())
        if r.returncode:
            print("gabi_strlookup_test: FAIL (cases)")
            bad += 1
        for mode, msg in (("same_file_ambiguity", "ambiguous string in one source file"),
                          ("cross_file_ambiguity", "gabi: ambiguous string (add"),
                          ("missing", "string literal without a guest copy")):
            r = subprocess.run([exe, mode], capture_output=True, text=True)
            ok = r.returncode != 0 and msg in r.stdout
            print("%-22s %s" % (mode, "stops: " + r.stdout.strip() if ok else "FAIL (rc %d, %r)" % (r.returncode, r.stdout)))
            bad += not ok
    print("gabi_strlookup_test: %s" % ("FAIL" if bad else "ok"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
