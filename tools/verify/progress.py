#!/usr/bin/env python3
"""Progress of running verify tools (2026-10-27): what a long verify / mutate run is doing, or that it hangs.

usage: progress.py [RUN_DIR | SCRATCH_DIR | UNIT | PID ...] [--json]

Without arguments: every progress file in the registry (/tmp/wwhd-verify-progress, env WWHD_PROGRESS_REGISTRY).
PID: that process's file (harness, verify.py or mutate.py) and, for verify.py/mutate.py, the files of its children.
RUN_DIR: a unit's build or scratch directory (its progress/ subdirectory), a progress/ directory itself, or any
directory above them (e.g. $RAMDISK or the tree key directory under it): every progress file below it.
UNIT: the unit name; its build/verify/<unit> and WWHD_VERIFY_SCRATCH directories are searched.
Each state shows its age (`updated ... ago`); a file whose process is gone is marked `(process gone)`.

Writers: harness.cpp (`harness-PID.json`, see its progress block), verify.py (`verify-PID.json`), mutate.py
(`mutate-PID.json`), verify_all.py / readable_check.py (`<tool>-PID.json`). Files live in the unit's directory +
/progress (on the WWHD_VERIFY_SCRATCH / WWHD_MUT_SCRATCH scratch when set, rewritten every PROGRESS_INTERVAL seconds;
on the SSD build directory at most every 30 s), with a symlink in the registry. A process removes its own file when
it ends normally. stderr lines all start with PREFIX ("[progress] "); stdout of every tool is unchanged.

Library part (used by verify.py, mutate.py, verify_all.py, readable_check.py): Settings (env + flags), Writer (atomic
JSON file + registry link), Ticker (periodic callback thread), read_state, find_child_state.
"""
import json
import os
import sys
import threading
import time

PREFIX = "[progress] "
REGISTRY = os.environ.get("WWHD_PROGRESS_REGISTRY") or "/tmp/wwhd-verify-progress"
SSD_FILE_INTERVAL = 30.0  # SSD wear: a progress file outside a scratch directory is rewritten at most this often

# flags verify.py takes for itself and turns into the harness's environment
FLAGS = {"-noprogress": 0, "-progress-interval": 1, "-stuck": 1, "-hanglimit": 1}


class Settings:
    """WWHD_VERIFY_PROGRESS=0 off; WWHD_VERIFY_PROGRESS_INTERVAL (s, default 10); WWHD_VERIFY_STUCK (s, default 60, 0 =
    never); WWHD_VERIFY_HANG_LIMIT (s, default 120, 0 = off); WWHD_VERIFY_PROGRESS_STDERR=0|alerts|1"""

    def __init__(self, env=None):
        env = os.environ if env is None else env
        self.on = env.get("WWHD_VERIFY_PROGRESS", "1") != "0"
        self.interval = _num(env.get("WWHD_VERIFY_PROGRESS_INTERVAL"), 10.0)
        self.stuck = _num(env.get("WWHD_VERIFY_STUCK"), 60.0)
        self.stderr = env.get("WWHD_VERIFY_PROGRESS_STDERR", "1")

    def take_flags(self, args):
        """remove -noprogress / -progress-interval S / -stuck S / -hanglimit S from args (in place) into os.environ, so
        every harness this process starts sees them; returns self"""
        i = 0
        while i < len(args):
            a = args[i]
            if a in FLAGS:
                if FLAGS[a] and i + 1 >= len(args):
                    sys.exit("%s takes a number of seconds" % a)
                if a == "-noprogress":
                    os.environ["WWHD_VERIFY_PROGRESS"] = "0"
                else:
                    try:
                        float(args[i + 1])
                    except ValueError:
                        sys.exit("%s takes a number of seconds" % a)
                    key = {"-progress-interval": "WWHD_VERIFY_PROGRESS_INTERVAL", "-stuck": "WWHD_VERIFY_STUCK",
                           "-hanglimit": "WWHD_VERIFY_HANG_LIMIT"}[a]
                    os.environ[key] = args[i + 1]
                del args[i:i + 1 + FLAGS[a]]
                continue
            i += 1
        self.__init__()
        return self

    def say(self, line, alert=False):
        if not self.on or self.stderr == "0" or (self.stderr == "alerts" and not alert):
            return
        try:
            sys.stderr.write(PREFIX + line + "\n")
            sys.stderr.flush()
        except (OSError, ValueError):
            pass


def _num(v, d):
    try:
        x = float(v)
        return x if x >= 0 else d
    except (TypeError, ValueError):
        return d


def fmt_dur(s):
    if s is None or s < 0 or s > 1e9:
        return "?"
    t = int(s + 0.5)
    if t >= 3600:
        return "%dh%02dm" % (t // 3600, t // 60 % 60)
    if t >= 60:
        return "%dm%02ds" % (t // 60, t % 60)
    return "%ds" % t


def file_interval(settings, on_scratch):
    """how often a progress file may be rewritten: the line interval on a scratch (RAM disk), >= 30 s on the SSD"""
    return settings.interval if on_scratch else max(SSD_FILE_INTERVAL, settings.interval)


class Writer:
    """DIR/<tool>-<pid>.json, written atomically (tmp + rename), at most every min_interval seconds unless forced, with a
    registry symlink; removed by close()"""

    def __init__(self, tool, directory, min_interval, pid=None):
        self.tool, self.pid = tool, pid or os.getpid()
        self.min_interval = min_interval
        self.path = self.link = None
        self.last = 0.0
        self.t0 = time.time()
        self.lock = threading.Lock()
        try:
            os.makedirs(directory, exist_ok=True)
            self.path = os.path.join(directory, "%s-%d.json" % (tool, self.pid))
        except OSError:
            return
        try:
            os.makedirs(REGISTRY, exist_ok=True)
            link = os.path.join(REGISTRY, "%s-%d.json" % (tool, self.pid))
            if os.path.abspath(directory) != os.path.abspath(REGISTRY):
                if os.path.lexists(link):
                    os.remove(link)
                os.symlink(self.path, link)
                self.link = link
        except OSError:
            pass

    def write(self, state, force=False):
        if not self.path:
            return
        with self.lock:
            now = time.time()
            if not force and now - self.last < self.min_interval:
                return
            self.last = now
            d = {"tool": self.tool, "pid": self.pid, "updated": int(now), "elapsed_s": round(now - self.t0, 1)}
            d.update(state)
            tmp = self.path + ".tmp"
            try:
                with open(tmp, "w") as f:
                    json.dump(d, f)
                    f.write("\n")
                os.replace(tmp, self.path)
            except OSError:
                pass

    def close(self):
        for p in (self.path, self.link):
            if p:
                try:
                    os.remove(p)
                except OSError:
                    pass
        self.path = None


class Ticker:
    """call fn() every `every` seconds on a daemon thread until stop()"""

    def __init__(self, every, fn):
        self.ev = threading.Event()
        self.every, self.fn = max(0.2, every), fn
        self.t = threading.Thread(target=self._loop, daemon=True)
        self.t.start()

    def _loop(self):
        while not self.ev.wait(self.every):
            try:
                self.fn()
            except Exception as e:  # progress must never take a run down
                sys.stderr.write(PREFIX + "progress error: %r\n" % (e,))

    def stop(self):
        self.ev.set()
        self.t.join()


def run_captured(cmd, label, settings=None, **kw):
    """subprocess.run(cmd, capture_output=True, text=True, **kw) for a verify.py child whose merged output a caller keeps
    (verify_all.py, readable_check.py, nulltrap_compare.py): the child prints no progress lines (its captured stdout and
    stderr stay exactly as before); this process prints `[progress] LABEL: ...` from the child's progress file every
    interval instead (and its STUCK state as an alert)."""
    import subprocess
    settings = settings or Settings()
    env = dict(kw.pop("env", None) or os.environ)
    if settings.on:
        env["WWHD_VERIFY_PROGRESS_STDERR"] = "0"
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env, **kw)
    tk = None
    if settings.on:
        def tick():
            s = state_of_pid(p.pid)
            if not s:
                return
            if s.get("phase") == "build":
                settings.say("%s: building" % label)
                return
            if s.get("total") is None:
                return
            settings.say("%s: %s, %s/%s inputs, %.1f inputs/s, ETA %s%s%s" % (
                label, s.get("line", ""), s.get("done"), s.get("total"), s.get("rate") or 0, fmt_dur(s.get("eta_s")),
                ", %d hang(s)" % s["hangs"] if s.get("hangs") else "", "  STUCK (progress.py %d)" % p.pid if s.get("stuck") else ""),
                alert=bool(s.get("stuck")))
        tk = Ticker(settings.interval, tick)
    try:
        out, err = p.communicate()
    except BaseException:
        p.kill()
        p.wait()
        raise
    finally:
        if tk:
            tk.stop()
    return subprocess.CompletedProcess(cmd, p.returncode, out, err)


def read_state(path):
    try:
        with open(path) as f:
            return json.load(f)
    except (OSError, ValueError):
        return None


def pid_alive(pid):
    try:
        os.kill(int(pid), 0)
        return True
    except ProcessLookupError:
        return False
    except (PermissionError, ValueError, TypeError):
        return True


def state_of_pid(pid):
    """the progress state of process pid from the registry (any tool), or None"""
    try:
        names = os.listdir(REGISTRY)
    except OSError:
        return None
    for n in names:
        if n.endswith("-%d.json" % pid):
            return read_state(os.path.join(REGISTRY, n))
    return None


def harness_summary(states):
    """one dict summing harness states (shards of one run): done/total, rate, ETA, slowest function index, stuck"""
    if not states:
        return None
    ad = sum(s.get("all_done", 0) for s in states)
    at = sum(s.get("all_total", 0) for s in states)
    rate = sum(s.get("rate", 0) for s in states)
    return {"all_done": ad, "all_total": at, "rate": round(rate, 2),
            "eta_s": round((at - ad) / rate) if rate > 0 and at > ad else None,
            "func_index": min(s.get("func_index", 0) for s in states), "func_total": max(s.get("func_total", 0) for s in states),
            "stuck": [s for s in states if s.get("stuck")], "hangs": sum(s.get("hangs", 0) for s in states),
            "updated": min(s.get("updated", 0) for s in states)}


def describe(s, now=None):
    """one human line for a state dict"""
    now = now or time.time()
    age = now - s.get("updated", now)
    gone = "" if pid_alive(s.get("pid", 0)) else " (process gone)"
    head = "%s pid %s%s, updated %s ago" % (s.get("tool", "?"), s.get("pid"), gone, fmt_dur(age))
    t = s.get("tool")
    if t == "harness":
        txt = "function %s/%s %s %s, %s %s/%s, all %s/%s inputs, %.1f inputs/s, ETA %s, seed %s (input %s, %s side, running %s)" % (
            s.get("func_index"), s.get("func_total"), s.get("func"), s.get("name"), s.get("phase"), s.get("done"), s.get("total"),
            s.get("all_done"), s.get("all_total"), s.get("rate", 0), fmt_dur(s.get("eta_s")), s.get("seed"), s.get("input"),
            s.get("side"), fmt_dur(s.get("input_elapsed_s")))
        if s.get("stuck"):
            txt += "  STUCK"
        if s.get("hangs"):
            txt += "  %d HANG(s)" % s["hangs"]
        if s.get("state") != "running":
            txt += "  [%s]" % s.get("state")
        return head + ": " + txt
    parts = ["%s=%s" % (k, s[k]) for k in ("unit", "phase", "seed", "shards", "spec_index", "spec_total", "done", "total",
                                             "kills", "killed", "survived", "timeouts") if k in s]
    if s.get("eta_s") is not None:
        parts.append("ETA %s" % fmt_dur(s["eta_s"]))
    if s.get("line"):
        parts.append(s["line"])
    return head + ": " + ", ".join(parts)


def collect(targets):
    """progress files for the command line targets (see the usage)"""
    files = []
    if not targets:
        targets = [REGISTRY]
    for t in targets:
        if t.isdigit():
            pid = int(t)
            seen = set()
            todo = [pid]
            while todo:
                p = todo.pop()
                if p in seen:
                    continue
                seen.add(p)
                for d in [REGISTRY]:
                    for n in _listdir(d):
                        if n.endswith("-%d.json" % p):
                            files.append(os.path.join(d, n))
                            s = read_state(os.path.join(d, n)) or {}
                            todo += [c for c in s.get("children", []) if isinstance(c, int)]
            continue
        if not os.path.isdir(t):
            sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
            import verify
            cand = [d for d in verify.unit_dirs_all(t)]
            t_dirs = [d for d in cand if os.path.isdir(d)]
            if not t_dirs:
                sys.exit("progress.py: %s is neither a PID, a directory nor a unit with a build directory" % t)
        else:
            t_dirs = [t]
        for td in t_dirs:
            for dp, dn, fs in os.walk(td):
                for f in fs:
                    if f.endswith(".json") and f.split("-")[0] in ("harness", "verify", "mutate", "verify_all", "readable_check",
                                                                    "nulltrap_compare"):
                        if os.path.basename(dp) == "progress" or os.path.abspath(dp) == os.path.abspath(REGISTRY):
                            files.append(os.path.join(dp, f))
                if "receipts" in dn:
                    dn.remove("receipts")
    out, seen = [], set()
    for f in files:
        r = os.path.realpath(f)
        if r not in seen and os.path.exists(r):
            seen.add(r)
            out.append(f)
    return out


def _listdir(d):
    try:
        return sorted(os.listdir(d))
    except OSError:
        return []


def main(argv):
    as_json = "--json" in argv
    argv = [a for a in argv if a != "--json"]
    if any(a in ("-h", "--help") for a in argv):
        print(__doc__)
        return 0
    files = collect(argv)
    if not files:
        print("progress.py: no progress files (nothing running, or the run has finished)")
        return 1
    now = time.time()
    states = [s for s in (read_state(f) for f in files) if s]
    if as_json:
        print(json.dumps(states, indent=1))
        return 0
    for s in sorted(states, key=lambda s: (s.get("tool") != "verify_all", s.get("tool") != "readable_check",
                                           s.get("tool") == "harness", s.get("pid", 0))):
        print(describe(s, now))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
