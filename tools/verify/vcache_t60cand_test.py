#!/usr/bin/env python3
"""Self-test of the result cache's `t60cand` keys (vcache.py, 2026-11-17), on the true60 T-02 spec of
d_a_obj_timer (`real` + `t60cand 023A232C` = Act_c::mode_count, reached from _execute 023A2258 through the mode table):
1. a temporary copy of the T-02 spec over a copy of the source: the first T-02 run (-t60steps 2) fills, the second takes
   every row from the cache;
2. one line of the t60cand callee mode_count changed in the copy (`timer % 30` -> `timer % 31`): every row misses
   (_execute and Mthd_Execute run the callee's candidate code through a guest call, which is no relocation edge; before
   this key part they came back cached with the old rows);
3. the line restored: every row is cached again, with the rows of step 1.
Builds and the cache go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set); temporary files
are removed.
"""
import atexit
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_vcache_t60cand_test")
SRC = "wwhd_src/d/actor/d_a_obj_timer.cpp"
COPY = "wwhd_src/d/actor/_vcache_t60cand_test_d_a_obj_timer.cpp"
EDIT = ("if (timer % 30 == 0) {", "if (timer % 31 == 0) {")
CALLERS = ("023A2258", "023A2480")  # _execute, Mthd_Execute: reach mode_count 023A232C only through the t60cand swap
ARGS = ["-n", "200", "-seed", "1", "-t60steps", "2", "-noprogress", "-buildj", "4", "-shards", "2"]


def run(unit, *extra):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit] + ARGS + list(extra), cwd=ROOT,
                       capture_output=True, text=True)
    if r.returncode not in (0, 1):
        sys.stderr.write(r.stdout[-3000:] + r.stderr[-3000:])
        raise SystemExit("verify.py %s: exit %d" % (unit, r.returncode))
    m = re.search(r"vcache: (\d+) of (\d+) function\(s\) cached", r.stderr)
    cached = set(re.findall(r"vcache: ([0-9A-F]{8}) cached", r.stderr))
    return r.stdout, (int(m.group(1)), int(m.group(2))) if m else None, cached


def rows(out):
    return [l for l in out.splitlines() if re.match(r"^[0-9A-F]{8} ", l)]


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-vcache_t60cand_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    for k in ("WWHD_VERIFY_ARCHIVE", "WWHD_VERIFY_CACHE", "WWHD_VERIFY_CACHE_MAX_MB"):
        os.environ.pop(k, None)
    atexit.register(shutil.rmtree, scratch, True)
    fails = []

    def check(cond, what):
        print("%s %s" % ("ok  " if cond else "FAIL", what), flush=True)
        if not cond:
            fails.append(what)

    try:
        text = open(os.path.join(ROOT, SRC)).read()
        if text.count(EDIT[0]) != 1:
            raise SystemExit("edit site not found once in %s" % SRC)
        spec = open(os.path.join(UNITS, "d_a_obj_timer.txt")).read()
        if not re.search(r"^src\s+" + re.escape(SRC) + r"\s*$", spec, flags=re.M):
            raise SystemExit("src line not found in d_a_obj_timer.txt")
        spec = re.sub(r"^src\s+" + re.escape(SRC) + r"\s*$", "src " + COPY, spec, flags=re.M)
        t60 = [l for l in open(os.path.join(UNITS, "_t60", "d_a_obj_timer.txt")).read().split("\n") if not l.startswith("include ")]
        if not any(l.split()[:2] == ["t60cand", "023A232C"] for l in t60 if l.strip()):
            raise SystemExit("_t60/d_a_obj_timer.txt has no `t60cand 023A232C` line")
        os.makedirs(TMPU, exist_ok=True)
        open(os.path.join(TMPU, "a.txt"), "w").write(spec + "\n" + "\n".join(t60) + "\n")
        open(os.path.join(ROOT, COPY), "w").write(text)
        u = "_vcache_t60cand_test/a"
        # 1. fill, then cached
        oa, sa, _ = run(u)
        ob, sb, cb = run(u)
        n = sa[1] if sa else 0
        check(sa is not None and sa[0] == 0 and n > 0 and sb == (n, n) and oa == ob,
              "T-02 copy spec: fills (%s), then all cached (%s), identical rows" % (sa, sb))
        check(all(c in cb for c in CALLERS), "callers %s are rows of the run" % (CALLERS,))
        # 2. the t60cand callee changed: every row misses
        open(os.path.join(ROOT, COPY), "w").write(text.replace(*EDIT))
        oc, sc, cc = run(u)
        check(sc == (0, n) and not cc, "edited t60cand callee mode_count: 0/%d cached (%s; cached: %s)" % (n, sc, sorted(cc)))
        check(not any(c in cc for c in CALLERS), "callers %s miss" % (CALLERS,))
        # 3. restored: all cached again with the old rows
        open(os.path.join(ROOT, COPY), "w").write(text)
        od, sd, _ = run(u)
        check(sd == (n, n) and od == oa, "restored line: %d/%d cached, identical rows (%s)" % (n, n, sd))
    finally:
        if os.path.exists(os.path.join(ROOT, COPY)):
            os.remove(os.path.join(ROOT, COPY))
        shutil.rmtree(TMPU, ignore_errors=True)
    print("vcache_t60cand_test: %s" % ("OK" if not fails else "FAIL (%d)" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
