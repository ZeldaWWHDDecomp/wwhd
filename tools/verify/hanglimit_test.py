#!/usr/bin/env python3
"""Self-test of the harness's wall-clock hang limit under CPU starvation (2026-10-27, AGL#12 027ADA7C: at load ~104 an
input used < 5% CPU for 120 s and was ended as a false candidate-only timeout).

The limit may end only a run whose thread is blocked or sleeping, never one that is runnable but starved of CPU.
Two temporary specs (lib2_sead_15 with a copy of WorkerRuntimeInstances.cpp whose constructWorkerCounter 02759928
never returns):
1. blocked: the candidate sleeps forever; with -hanglimit 3 the row is "wall-clock limit exceeded" and the HANG line
   names the 1-min load;
2. starved: the candidate spins (a computing loop); this harness is held at about 1% of a CPU (SIGSTOP/SIGCONT duty
   cycle, so the machine is not loaded) for 45 s with -hanglimit 3: the wall-clock limit must not fire (the run uses
   < 5% CPU, as under heavy load, but stays runnable). Before the fix it fired after ~3 s.
Builds go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set); temporary files are removed.
"""
import atexit
import os
import shutil
import signal
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_hanglimit_test")
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUTS = {"sleep": "wwhd_src/sead/_hanglimit_test_sleep_WorkerRuntimeInstances.cpp",
        "spin": "wwhd_src/sead/_hanglimit_test_spin_WorkerRuntimeInstances.cpp"}
F = "02759928"


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-hanglimit_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    atexit.register(shutil.rmtree, scratch, True)
    import verify
    import specfile
    fails = []
    try:
        text = open(os.path.join(ROOT, SRC)).read()
        old = "    if(object!=NULL) object->mValue=0;\n    return object;"
        decl = "extern void releaseWorkerArray(void*);\n"
        if text.count(old) != 1 or text.count(decl) != 1:
            raise SystemExit("mutation site not found once in %s" % SRC)
        open(os.path.join(ROOT, MUTS["sleep"]), "w").write(
            text.replace(old, "    if(object!=NULL) { for (;;) usleep(50000); }\n    return object;").replace(
                decl, decl + 'extern "C" int usleep(unsigned int);\n'))
        open(os.path.join(ROOT, MUTS["spin"]), "w").write(
            text.replace(old, "    if(object!=NULL) { volatile unsigned spin = 0; for (;;) spin++; }\n    return object;"))
        os.makedirs(TMPU, exist_ok=True)
        exes = {}
        for name, src in MUTS.items():
            open(os.path.join(TMPU, name + ".txt"), "w").write(
                open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + src))
            unit = "_hanglimit_test/" + name
            r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-n", "1", "-only", "0275A024",
                                "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
            out = verify.unit_dir(unit)
            exes[name] = [os.path.join(out, "verify"), "-spec", specfile.spec_path(unit, ROOT, out)]
            if not os.path.exists(exes[name][0]):
                raise SystemExit("build of %s failed: %s" % (unit, (r.stdout + r.stderr)[-500:]))
        hdr = ["-strictnan", "-calleesp", "-footprint"]  # lib2_sead_15's `# verify:` header

        # 1. blocked
        t = time.time()
        r = subprocess.run(exes["sleep"] + hdr + ["-only", F, "-n", "1", "-hanglimit", "3", "-stuck", "0"], cwd=ROOT,
                           capture_output=True, text=True)
        row = [l for l in r.stdout.splitlines() if l.startswith(F)]
        hang = [l for l in r.stderr.splitlines() if "HANG" in l]
        print("1. blocked candidate: %.1f s, rc %d\n   %s\n   %s" % (time.time() - t, r.returncode, row[0] if row else "-",
                                                                   hang[0] if hang else "-"))
        if not row or "wall-clock limit exceeded" not in row[0] or not hang or "1-min load" not in hang[0]:
            fails.append("blocked: not ended by the wall-clock limit: %s %s" % (row, hang))

        # 2. starved (runnable) candidate: ~1% of a CPU for 45 s
        pdir = os.path.join(scratch, "progress")
        env = dict(os.environ, WWHD_VERIFY_PROGRESS_DIR=pdir, WWHD_VERIFY_PROGRESS_FILE_INTERVAL="1")
        p = subprocess.Popen(exes["spin"] + hdr + ["-only", F, "-n", "1", "-hanglimit", "3", "-stuck", "0"], cwd=ROOT,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env)
        import json
        state = {}
        for _ in range(120):  # start-up and the original's pre-runs at full speed, until the candidate's loop runs
            time.sleep(0.25)
            try:
                state = json.load(open(os.path.join(pdir, "harness-%d.json" % p.pid)))
            except (OSError, ValueError):
                continue
            if state.get("side") == "candidate" and state.get("phase") == "generated":
                break
        if state.get("side") != "candidate":
            fails.append("starved: the candidate's loop never started (%r)" % state)
        t = time.time()
        while time.time() - t < 45 and p.poll() is None:
            os.kill(p.pid, signal.SIGSTOP)
            time.sleep(0.99)
            os.kill(p.pid, signal.SIGCONT)
            time.sleep(0.01)
        ended = p.poll() is not None
        cpu = None
        try:
            cpu = subprocess.run(["ps", "-o", "time=", "-p", str(p.pid)], capture_output=True, text=True).stdout.strip()
        except OSError:
            pass
        if not ended:
            p.kill()
        out, err = p.communicate()
        fired = "wall-clock limit" in out + err
        print("2. starved spinning candidate: %.0f s at ~1%% of a CPU (process CPU time %s), %s, wall-clock limit %s" % (
            time.time() - t, cpu, "ended by itself" if ended else "still running (killed)", "FIRED" if fired else "not fired"))
        if fired:
            fails.append("starved: the wall-clock limit ended a runnable run: %s" % (out + err)[-400:])
    finally:
        shutil.rmtree(TMPU, ignore_errors=True)
        for m in MUTS.values():
            if os.path.exists(os.path.join(ROOT, m)):
                os.remove(os.path.join(ROOT, m))
    for x in fails:
        print("FAIL", x)
    print("hanglimit_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
