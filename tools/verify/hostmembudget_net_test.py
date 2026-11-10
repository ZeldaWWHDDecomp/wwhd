#!/usr/bin/env python3
"""Self-test (2026-11-09): a host-memory-budget abort that happens while the harness collects a
run's net effect names that cause, also when the run had already aborted for another reason.

Carrier: lib_js_jaudio_4's JASystem::Calc::bzero 02843E34 as the tested function, clearing 32 MB - 1 at 0x40000001 (unaligned, so it stores words itself instead of calling clearAudioCacheRange) that a
`fill` set to 0xAA: every byte changes, so each side's net effect holds 32M entries (8 bytes each, ~256 MB of host
memory), far more than the run's own page state (8192 pages, ~90 MB). With budgets B on a ladder, small B aborts
inside the run, large B passes, and in between the run stays under B and the net-effect collection passes it.
1. Plain run: some B on the ladder gives `hostmembudget: 02843E34 ... host memory budget exceeded (RSS N MB > B MB in one
   run, collecting its net effect)` on stderr; no B gives that cause together with a FAIL row (aborts never count).
2. With `pagebudget 6000` the run aborts after 6000 pages (`page budget exceeded ...`), and its net effect (~24M entries)
   is still collected: some B gives the appended reason `page budget exceeded (6000 guest pages touched in one run);
   host memory budget exceeded (RSS N MB > B MB in one run, collecting its net effect)`.
   (A run the budget already ended inside the run keeps that reason alone: small B on the ladder.)
3. mutate.hostmem_fields names the causes for a receipt: `net-effect collection` for both texts above, `run` for the
   in-run text, nothing for output without a budget abort.
Writes temporary unit files under tools/verify/units/_hostmembudget_net_test/ and removes them. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
TMP = os.path.join(HERE, "units", "_hostmembudget_net_test")
FUNC = "02843E34"
SPEC = [l for l in open(os.path.join(HERE, "units", "lib_js_jaudio_4.txt")).read().splitlines()
        if not re.match(r"^\s*(arg!?|field|fill|stable)\s+%s\b" % FUNC, l)]
BIG = ["arg! %s r3 0x40000001" % FUNC, "arg! %s r4 0x1FFFFFF" % FUNC,
       "fill %s 40000000 0x1000000 0xAA" % FUNC, "fill %s 41000000 0x1000000 0xAA" % FUNC, "accessbudget 200000000"]
LADDER = list(range(64, 2049, 32))
NET = re.compile(r"^hostmembudget: %s (original|candidate) side, gen seed=\d+: (.*)$" % FUNC, re.M)


def run(name, lines, budget):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    e = dict(os.environ)
    e["WWHD_VERIFY_HOSTMEM_MB"] = str(budget)
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_hostmembudget_net_test/" + name, "-only", FUNC,
                        "-n", "1", "-shards", "1", "-seed", "1", "--no-cache"], cwd=ROOT, capture_output=True, text=True, env=e)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    return r.returncode, (row[0] if row else ""), out


def ladder(name, lines, want, fails):
    """the first budget on the ladder whose stderr names the net-effect cause matching `want`"""
    for b in LADDER:
        rc, row, out = run(name, lines, b)
        if rc not in (0, 1) or not row:
            fails.append("%s B=%d: no function row (rc %d): %s" % (name, b, rc, out[-400:]))
            return None
        if rc == 1 and "only one side aborted" not in row and "aborted" not in row:
            fails.append("%s B=%d: a FAIL row that is not an abort: %s" % (name, b, row))
            return None
        for side, why in NET.findall(out):
            if want.match(why):
                rss, bud = map(int, re.search(r"host memory budget exceeded \(RSS (\d+) MB > (\d+) MB", why).groups())
                if rss <= bud or bud != b:
                    fails.append("%s B=%d: the reason must name the sampled RSS above the budget: %s" % (name, b, why))
                    return None
                print("hostmembudget_net_test: %s B=%d %s side: %s" % (name, b, side, why))
                return b
            if "collecting its net effect" in why:
                fails.append("%s B=%d: net-effect cause with the wrong text: %s" % (name, b, why))
                return None
        if rc == 0 and "aborted" not in row:
            break  # passes from here on: no budget on the ladder overflows in the collection
    fails.append("%s: no budget on the ladder %d..%d MB overflowed while collecting the net effect" % (name, LADDER[0], LADDER[-1]))
    return None


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        plain = re.compile(r"^host memory budget exceeded \(RSS \d+ MB > \d+ MB in one run, collecting its net effect\)$")
        ladder("plain", SPEC + BIG, plain, fails)
        appended = re.compile(r"^page budget exceeded \(6000 guest pages touched in one run\); host memory budget exceeded "
                              r"\(RSS \d+ MB > \d+ MB in one run, collecting its net effect\)$")
        ladder("pagebudget", SPEC + BIG + ["pagebudget 6000"], appended, fails)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_hostmembudget_net_test"), ignore_errors=True)
    import mutate
    t_net = "only one side aborted: host memory budget exceeded (RSS 900 MB > 800 MB in one run, collecting its net effect)"
    t_app = "access budget exceeded; host memory budget exceeded (RSS 900 MB > 800 MB in one run, collecting its net effect)"
    t_run = "only one side aborted: host memory budget exceeded (RSS 900 MB > 800 MB in one run)"
    if mutate.hostmem_fields(t_net).get("hostmem_cause") != ["net-effect collection"]:
        fails.append("hostmem_fields: net-effect text: %r" % mutate.hostmem_fields(t_net))
    if mutate.hostmem_fields(t_app).get("hostmem_cause") != ["net-effect collection"]:
        fails.append("hostmem_fields: appended text: %r" % mutate.hostmem_fields(t_app))
    if mutate.hostmem_fields(t_run) != {"hostmem_aborts": [t_run[len("only one side aborted: "):]], "hostmem_cause": ["run"]}:
        fails.append("hostmem_fields: in-run text: %r" % mutate.hostmem_fields(t_run))
    if mutate.hostmem_fields("only one side aborted: access budget exceeded") != {}:
        fails.append("hostmem_fields: a run without a budget abort gets fields")
    for f in fails:
        print("FAIL", f)
    print("hostmembudget_net_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
