#!/usr/bin/env python3
"""osmem_equiv.py: the `osmem` heap model (harness.cpp namespace osm) against the shipping port's own code.

Extracts `struct ExpHeap`, `struct FrmHeap` and `frm_alloc` from the port's runtime/src/hle/coreinit_mem.cpp (read only;
path: --port-src, default $WWHD_PORT_SRC or runtime/src of the port checkout) and the same pieces
from tools/verify/src/harness.cpp, compiles both into one host program (in a temporary directory) and runs seeded random
operation sequences on both: ExpHeap alloc (head and tail, alignments 0..0x2000 incl. non-powers of two, sizes 0..4 GB
incl. sizes past the heap), release (valid, double, foreign), largest, total_free; FrmHeap head/tail allocation. Every
result and the full free/block state after every operation must be equal. Prints PASS/FAIL; exit 0 on PASS, 1 on a
difference, 2 when a source piece cannot be found."""
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def block(src, start_pat):
    m = re.search(start_pat, src, re.M)
    if not m:
        return None
    i = src.index("{", m.start())
    depth = 0
    for j in range(i, len(src)):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                end = j + 1
                if src[end:end + 1] == ";":
                    end += 1
                return src[m.start():end]
    return None


def main():
    port = None
    args = sys.argv[1:]
    if "--port-src" in args:
        port = args[args.index("--port-src") + 1]
    port = port or os.environ.get("WWHD_PORT_SRC") or "runtime/src"
    psrc = open(os.path.join(port, "hle", "coreinit_mem.cpp")).read()
    hsrc = open(os.path.join(HERE, "src", "harness.cpp")).read()
    osm = hsrc[hsrc.index("namespace osm {"):hsrc.index("}  // namespace osm")]
    pieces = {
        "port ExpHeap": block(psrc, r"^struct ExpHeap \{"),
        "port FrmHeap": block(psrc, r"^struct FrmHeap \{"),
        "port frm_alloc": block(psrc, r"^uint32_t frm_alloc\("),
        "osm ExpHeap": block(osm, r"^struct ExpHeap \{"),
        "osm FrmHeap": block(osm, r"^struct FrmHeap \{"),
        "osm frm_alloc": block(osm, r"^static uint32_t frm_alloc\("),
    }
    for k, v in pieces.items():
        if not v:
            print("osmem_equiv: cannot find %s" % k)
            return 2
    prog = r"""
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
namespace port {
%(pe)s
%(pf)s
std::mutex g_mem_mutex;
std::unordered_map<uint32_t, FrmHeap*> g_frm;
%(pa)s
}
namespace osm {
%(oe)s
%(of)s
struct State { std::map<uint32_t, FrmHeap> frm; };
static State st;
%(oa)s
}
static uint64_t rs = 1;
static uint32_t rnd() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)rs; }
static int32_t pick_align() {
    static const int32_t a[] = {0, 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x1000, 0x2000, 3, 0x44, 12, -4, -0x40, -0x20, -0x1000, -8};
    return a[rnd() %% (sizeof a / sizeof a[0])];
}
static uint32_t pick_size() {
    switch (rnd() %% 6) {
    case 0: return rnd() %% 0x40;
    case 1: return rnd() %% 0x10000;
    case 2: return rnd() %% 0x1000000;
    case 3: return 0x3B7780C4u + (rnd() %% 3) * 0x100000;
    case 4: return rnd();
    default: return 0xFFFFFFFFu - (rnd() %% 8);
    }
}
template <class E> static std::string dump(const E& h) {
    std::string s;
    char b[64];
    for (auto& f : h.free) { snprintf(b, sizeof b, "F%%X:%%X ", f.first, f.second); s += b; }
    std::vector<std::pair<uint32_t, std::pair<uint32_t, uint32_t>>> v(h.blocks.begin(), h.blocks.end());
    std::sort(v.begin(), v.end());
    for (auto& x : v) { snprintf(b, sizeof b, "B%%X:%%X:%%X ", x.first, x.second.first, x.second.second); s += b; }
    return s;
}
int main() {
    long ops = 0, diffs = 0;
    for (int seed = 1; seed <= 400 && !diffs; seed++) {
        rs = 0x9E3779B97F4A7C15ull * seed;
        uint32_t start = seed %% 2 ? 0x104DB040u : 0x10000040u + (rnd() & 0xFFFF0u), end = seed %% 3 ? 0x50000000u : start + (rnd() %% 0x200000) + 0x40;
        port::ExpHeap p; osm::ExpHeap o;
        p.init(start, end); o.init(start, end);
        std::vector<uint32_t> live;
        for (int k = 0; k < 400; k++, ops++) {
            uint32_t r = rnd() %% 10, a1 = 0, a2 = 0;
            if (r < 5) {
                uint32_t sz = pick_size(); int32_t al = pick_align();
                a1 = p.alloc(sz, al); a2 = o.alloc(sz, al);
                if (a1) live.push_back(a1);
            } else if (r < 8) {
                uint32_t u = live.empty() || rnd() %% 5 == 0 ? rnd() : live[rnd() %% live.size()];
                a1 = p.release(u); a2 = o.release(u);
            } else if (r == 8) {
                uint32_t al = (uint32_t)pick_align();
                uint32_t q = (al & 0x80000000u ? 0u - al : al) | 4;
                a1 = p.largest(q); a2 = o.largest(q);
            } else { a1 = p.total_free(); a2 = o.total_free(); }
            if (a1 != a2 || dump(p) != dump(o)) { printf("ExpHeap seed %%d op %%d (kind %%u): port %%08X osm %%08X\n", seed, k, r, a1, a2); diffs++; break; }
        }
        port::g_frm.clear(); osm::st.frm.clear();
        uint32_t fs = seed %% 2 ? 0xF4000040u : 0xE0000040u, fe = fs - 0x40 + (seed %% 2 ? 0x02000000u : 0x02800000u);
        static port::FrmHeap ph; ph = port::FrmHeap{fs, fe, fs, fe};
        port::g_frm[fs - 0x40] = &ph;
        osm::st.frm[fs - 0x40] = osm::FrmHeap{fs, fe, fs, fe};
        for (int k = 0; k < 300; k++, ops++) {
            uint32_t sz = rnd() %% 5 == 0 ? pick_size() : rnd() %% 0x20000; int32_t al = pick_align();
            uint32_t h = rnd() %% 16 == 0 ? 0x12345678u : fs - 0x40;
            uint32_t a1 = port::frm_alloc(h, sz, al), a2 = osm::frm_alloc(h, sz, al);
            osm::FrmHeap& oh = osm::st.frm[fs - 0x40];
            if (a1 != a2 || ph.head != oh.head || ph.tail != oh.tail) { printf("FrmHeap seed %%d op %%d: port %%08X osm %%08X\n", seed, k, a1, a2); diffs++; break; }
        }
    }
    printf("osmem_equiv: %%ld operations, %%ld differences\n", ops, diffs);
    return diffs ? 1 : 0;
}
""" % {"pe": pieces["port ExpHeap"], "pf": pieces["port FrmHeap"], "pa": pieces["port frm_alloc"],
       "oe": pieces["osm ExpHeap"], "of": pieces["osm FrmHeap"], "oa": pieces["osm frm_alloc"]}
    d = tempfile.mkdtemp(prefix="osmem_equiv-", dir=os.environ.get("TMPDIR"))
    cpp, exe = os.path.join(d, "eq.cpp"), os.path.join(d, "eq")
    with open(cpp, "w") as f:
        f.write(prog)
    r = subprocess.run(["clang++", "-std=c++17", "-O1", "-fwrapv", "-o", exe, cpp], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr[-3000:])
        print("osmem_equiv: FAIL (compile)")
        return 2
    r = subprocess.run([exe], capture_output=True, text=True)
    print(r.stdout.strip())
    print("osmem_equiv: %s" % ("PASS" if r.returncode == 0 else "FAIL"))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())
