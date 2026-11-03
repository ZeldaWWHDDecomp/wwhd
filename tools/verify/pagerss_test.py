#!/usr/bin/env python3
"""Self-test of the harness's host memory per touched guest page (2026-11-02).

A synthetic spec on lib_js_base: before JKRDvdFile::close 027EACBC, a `setup` runs the original memzero 028F521C
(it tail-calls the coreinit memset import, modelled by `mockset`, which writes every byte through the harness's mock
path as the `osmem fs` read model does) over N * 4 KB at 0x60000000, so each run touches N distinct guest pages. The
harness runs generated inputs 0 and 1 (-n 2: input 0 with clobbering, so every touched chunk also gets its clobber
epochs; input 1 without), for N = 1024, 8192 and 32768, each in its own process; its peak RSS is taken from wait4.
Prints the peak RSS per N and the slope (host bytes per touched guest page) between the smallest and the largest N.
Exit 0 when the slope is at most --max-kb KB per page (default 12; measured 2026-11-02: 14.7 KB before, 10.6 after)
and every run printed its result row. The unit is built once (verify.py); the other N reuse its executable with their own spec.
Also runs the harness's `-selftest-mem` checks (shared fill pages copied on write, fills with different bytes never
shared, guest writes and resets give back the fill byte, the compact clobber epochs, NetMap) and fails on any FAIL line.
Writes a temporary unit directory and removes it (its build directory stays with the other unit builds).
"""
import argparse
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNITS = os.path.join(HERE, "units")
UNIT = "_pagerss_test/t"
TMP = os.path.join(UNITS, "_pagerss_test")
CLOSE = "027EACBC"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()


def spec(npages):
    return BASE + [
        "real 028F521C",                      # memzero(p, n) -> memset(p, 0, n)
        "mockset C000A858 r3 r4 r5",          # coreinit memset import: really stores (mock write path)
        "opbudget 0x40000000",
        "pagebudget 200000",
        "accessbudget 1000000000",
        "setup * 028F521C r3=0x60000000 r4=0x%X" % (npages * 4096),
    ]


def run_rss(exe, sp):
    """(stdout, stderr, exit code, peak RSS in bytes) of one harness process"""
    p = subprocess.Popen([exe, "-spec", sp, "-only", CLOSE, "-n", "2", "-seed", "1", "-noprogress"], cwd=ROOT,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=dict(os.environ, WWHD_VERIFY_PROGRESS="0"))
    out = p.stdout.read()
    err = p.stderr.read()
    _, status, ru = os.wait4(p.pid, 0)
    rss = ru.ru_maxrss if sys.platform == "darwin" else ru.ru_maxrss * 1024
    return out.decode(errors="replace"), err.decode(errors="replace"), os.waitstatus_to_exitcode(status), rss


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-kb", type=float, default=12.0)
    ap.add_argument("--sizes", default="1024,8192,32768")
    a = ap.parse_args()
    sizes = [int(x) for x in a.sizes.split(",")]
    os.makedirs(TMP, exist_ok=True)
    fails, res = [], {}
    try:
        for n in sizes:
            with open(os.path.join(TMP, "t%d.txt" % n), "w") as f:
                f.write("\n".join(spec(n)) + "\n")
        shutil.copy(os.path.join(TMP, "t%d.txt" % sizes[0]), os.path.join(TMP, "t.txt"))
        b = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-list", "--no-cache", "-buildj", "4"], cwd=ROOT,
                           capture_output=True, text=True)
        exe = os.path.join(verify.unit_dir(UNIT), "verify")
        if b.returncode or not os.path.exists(exe):
            print(b.stdout[-2000:], b.stderr[-2000:])
            print("FAIL build")
            return 1
        st = subprocess.run([exe, "-selftest-mem"], cwd=ROOT, capture_output=True, text=True)
        print(st.stdout.rstrip())
        if st.returncode or "FAIL" in st.stdout or "PASS" not in st.stdout:
            fails.append("-selftest-mem (exit %d)" % st.returncode)
        for n in sizes:
            out, err, rc, rss = run_rss(exe, os.path.join(TMP, "t%d.txt" % n))
            row = [l for l in out.splitlines() if l.startswith(CLOSE)]
            if rc not in (0, 1) or not row:
                fails.append("N=%d: exit %d, no row: %s" % (n, rc, (out + err)[-600:]))
                continue
            res[n] = rss
            print("N=%6d pages: peak RSS %8.1f MB  %s" % (n, rss / 1048576.0, row[0].strip()[:110]), flush=True)
        if len(res) == len(sizes):
            lo, hi = sizes[0], sizes[-1]
            slope = (res[hi] - res[lo]) / float(hi - lo)
            print("slope: %.1f KB of host RSS per touched guest page (N %d -> %d)" % (slope / 1024.0, lo, hi))
            if slope > a.max_kb * 1024:
                fails.append("slope %.1f KB per page > %.1f KB" % (slope / 1024.0, a.max_kb))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
