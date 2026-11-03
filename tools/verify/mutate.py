#!/usr/bin/env python3
"""Mutation test of the harness: how many deliberately wrong candidates does it reject?

usage: mutate.py UNIT [--n 300] [--max 400] [--seed 1] [--input-seed S] [--func ADDR,...] [-j JOBS] [--scratch DIR]
       mutate.py UNIT --rerun RECEIPTS_DIR [--rerun-class survived] [--n N] [--input-seed S] [-j JOBS]

--func limits the mutants to those functions; each mutant runs the functions whose rows can execute its change
(tools/verify/mutscope.py; --whole-unit: every function of the campaign scope);
-j runs mutants in parallel.

For every function body in the unit's candidate sources (from WWHD_FUNC to the next VERIFY),
make single-point mutants: change a literal, flip a comparison or logical operator, drop a
statement, swap two adjacent statements, change a + to -. Each mutant is compiled into the
unit and run with -n N generated inputs on its own function. A mutant that still passes is a
survivor: either equivalent to the original (e.g. swapping two independent stores) or a gap
in the inputs. Survivors are listed for inspection.
"""
import argparse
import atexit
import contextlib
import json
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile
import threading

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import mutschema  # noqa: E402
import mutscope  # noqa: E402  (per-mutant test scope, 2026-10-27)
import mutcode  # noqa: E402  (identical-code check, 2026-10-28)
import autoglue  # noqa: E402
import progress  # noqa: E402  (progress lines / file, 2026-10-27)
import refcache  # noqa: E402  (the reference-side cache's key, gate records; 2026-10-30)
import basecache  # noqa: E402  (baseline cache across cascade passes, 2026-10-30)
import specfile  # noqa: E402


def replace_object(objs, src, mobj):
    """The link list with src's object replaced by the mutant object mobj.

    Objects are named <basename of the source>.o (verify.build), so the match is the whole basename:
    a suffix match would also replace mtxvec.cpp.o when mutating vec.cpp (duplicate symbols at link).
    Exactly one object must match."""
    name = os.path.basename(src) + ".o"
    hits = [o for o in objs if os.path.basename(o) == name]
    if len(hits) != 1:
        raise SystemExit("mutate.py: %d objects named %s in the link list (want 1): %s" % (len(hits), name, objs))
    return [mobj if o == hits[0] else o for o in objs]


def parse_scope(text):
    """--only ADDR,ADDR,...: the set of addresses (hex, 8 digits); malformed entries stop the run"""
    out = set()
    for x in text.split(","):
        x = x.strip()
        if not re.match(r"^(0x)?[0-9A-Fa-f]{1,8}$", x):
            sys.exit("mutate.py: bad --only address %r" % x)
        out.add(int(x, 16))
    return out


def bodies(text):
    """(addr, start, end): the code of each verified function, including the helpers defined
    between the previous VERIFY line and its own (inlined GameCube functions)"""
    glued = autoglue.bodies(text)  # GameCube-style source: functions marked /* ADDR */
    if glued is not None:
        return glued
    out = []
    prev = 0
    for m in re.finditer(r"WWHD_FUNC\(\s*0x([0-9A-Fa-f]{8})[^;]*;", text):
        end = text.find("\nVERIFY(", m.end())
        end = end if end > 0 else len(text)
        # helpers: from the previous VERIFY line (or the first function definition) on
        start = prev if prev else text.rfind("\n", 0, text.rfind("\n", 0, m.start()))
        out.append((int(m.group(1), 16), max(start, 0), end))
        prev = text.find("\n", end + 1)
    return out


NOT_FUNCS = {"if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_assert"}


def preamble_helpers(text, regions):
    """File-local function definitions above the first marked function of a GameCube-style (glue) source, which
    bodies() leaves out (autoglue starts the first region at its own marker; 2026-10-26, AGL#1: the blend* helpers of
    EnvironmentManagerBlend.cpp were never mutated). Returns [(start, end, name, callers)]: callers = the marked
    functions (addresses, file order) that use the helper, directly or through other helpers; a helper no marked
    function uses cannot change a result and is left out. Only top-level definitions count (namespace and extern "C"
    blocks are transparent); types, globals, declarations and the glue comment stay out."""
    if not regions:
        return []
    code = autoglue.mask(text)
    limit = regions[0][1]
    found = []
    stack = []
    last = 0  # start of the current top-level header (after the last ; { or })
    i = 0
    while i < limit:
        c = code[i]
        if c == ";":
            last = i + 1
        elif c == "}":
            if stack:
                stack.pop()
            last = i + 1
        elif c == "{":
            header = code[last:i]
            if re.search(r"\bnamespace\b[\w\s:]*$", header) or re.search(r'\bextern\s+"x*"\s*$', header):
                stack.append("ns")
            elif all(k == "ns" for k in stack) and "=" not in header.split("(")[0]:
                m = re.search(r"\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?$", header)
                lines = [ln for ln in header.split("\n") if ln.strip()]
                if m and m.group(1) not in NOT_FUNCS and lines and not lines[0].lstrip().startswith("#"):
                    close = autoglue.match_close(code, i, "{", "}")
                    start = last + len(header) - len(header.lstrip())
                    found.append((start, close + 1, m.group(1)))
                    i = close + 1
                    last = i
                    continue
                stack.append("other")
            else:
                stack.append("other")
            last = i + 1
        i += 1
    uses = {}
    for hs, he, name in found:
        pat = re.compile(r"\b%s\b" % re.escape(name))
        uses[name] = ([addr for addr, s, e in regions if pat.search(code[s:e])],
                      [h for h2s, h2e, h in found if h != name and pat.search(code[h2s:h2e])])
    callers = {name: set(uses[name][0]) for _, _, name in found}
    changed = True
    while changed:  # through other helpers: a helper used by helper H is used by H's callers
        changed = False
        for _, _, name in found:
            for h in uses[name][1]:
                if not callers[name] >= callers[h]:
                    callers[name] |= callers[h]
                    changed = True
    order = [addr for addr, _, _ in regions]
    return [(hs, he, name, sorted(callers[name], key=order.index)) for hs, he, name in found if callers[name]]


def mutants(text, start, end):
    # blank out comments (keeping offsets) so mutations only hit code
    # (and the WWHD_FUNC/VERIFY bookkeeping, which only names the function)
    body = re.sub(r"/\*.*?\*/|//[^\n]*|WWHD_FUNC\([^;]*;|VERIFY\([^;]*;", lambda m: re.sub(r"[^\n]", " ", m.group(0)),
                  text[start:end], flags=re.S)
    res = []
    for m in re.finditer(r"(?<![\w.])(\d+\.\d+f|\d+)(?![\w.x])", body):
        lit = m.group(1)
        if lit.endswith("f"):
            new = repr(float(lit[:-1]) + 1.0) + "f"
        else:
            new = str(int(lit) + 1)
        res.append(("literal %s->%s" % (lit, new), start + m.start(1), start + m.end(1), new))
    for m in re.finditer(r"0x([0-9A-Fa-f]+)", body):
        v = int(m.group(1), 16)
        res.append(("literal 0x%X->0x%X" % (v, v ^ 1), start + m.start(), start + m.end(), "0x%X" % (v ^ 1)))
    flips = {"==": "!=", "!=": "==", "<=": "<", ">=": ">", "<": "<=", ">": ">=", "&&": "||", "||": "&&"}
    for m in re.finditer(r"==|!=|<=|>=|&&|\|\||(?<![<>-])[<>](?![<>=])", body):
        op = m.group(0)
        res.append(("operator %s->%s" % (op, flips[op]), start + m.start(), start + m.end(), flips[op]))
    for m in re.finditer(r"(?<=[\w)\]]) \+ (?=[\w(])", body):
        res.append(("operator + -> -", start + m.start(), start + m.end(), " - "))
    for m in re.finditer(r"!(?=[\w(])", body):
        res.append(("drop !", start + m.start(), start + m.end(), ""))
    lines = []
    pos = start
    for ln in body.split("\n"):
        s = ln.strip()
        if not s or s.startswith("#") or "/* 0x" in s or s.startswith(("VERIFY(", "WWHD_")):
            pos += len(ln) + 1
            continue
        if s.endswith(";") and not s.startswith(("return", "WWHD_FUNC", "gabi::Local", "//", "/*")) and "=" in s or \
                (s.endswith(";") and re.match(r"^[\w:>.\-]+\(.*\);$", s) and not s.startswith("return")):
            lines.append((pos, pos + len(ln)))
        pos += len(ln) + 1
    for a, b in lines:
        res.append(("drop statement `%s`" % text[a:b].strip()[:50], a, b, ""))
    for (a1, b1), (a2, b2) in zip(lines, lines[1:]):
        between = text[b1:a2]
        if between.strip() == "":
            res.append(("swap `%s` / `%s`" % (text[a1:b1].strip()[:30], text[a2:b2].strip()[:30]), a1, b2,
                        text[a2:b2] + between + text[a1:b1]))
    return res


SCRATCH_FLOOR = 200 << 20
REFCACHE_MIN_FREE = 2 << 30  # free bytes the reference-side cache needs where it is made (the harness caps each function)  # minimum free bytes in --scratch before a mutant is built there


def free_bytes(path):
    """free bytes of the file system holding path (a function, so the self-test can replace it)"""
    return shutil.disk_usage(path).free


def mutant_scratch_request(opt):
    """The requested per-mutant scratch directory and where the request came from: (--scratch DIR, "--scratch"), else
    (WWHD_MUT_SCRATCH, "WWHD_MUT_SCRATCH"), else (None, None). The one place the mutate-side scratch is resolved;
    Scratch checks it (missing, not writable, free space) and makes the per-run subdirectory."""
    if opt:
        return opt, "--scratch"
    env = os.environ.get("WWHD_MUT_SCRATCH")
    return (env, "WWHD_MUT_SCRATCH") if env else (None, None)


class Scratch:
    """Where the per-mutant scratch files (mutantN.cpp, mutantN.o, the mutantN executable) go.

    Default: the unit's build directory (as before). With --scratch DIR or WWHD_MUT_SCRATCH=DIR (the option wins), a
    per-run unique subdirectory of DIR, so parallel runs and parallel units never collide. DIR missing or not writable:
    exit 2 with an error (never created, never the SSD). Falls back to the build directory per mutant when DIR has less free
    space than the margin (3x the baseline executable, at least SCRATCH_FLOOR; WWHD_MUT_SCRATCH_MIN_FREE=BYTES
    overrides it) or a write/compile/link there runs out of space."""

    def __init__(self, unit, out, opt, exe):
        self.out = out
        self.dir = None
        self.noted = set()
        self.lock = threading.Lock()
        want, src = mutant_scratch_request(opt)
        if not want:
            return
        verify.require_scratch_dir(src, want, "mutate.py")  # missing / not writable: stop, never the SSD (2026-11-02)
        try:
            # a companion spec's name has a slash (lib2_agl_5/shader_holder): flatten it like campaign_lock does, else
            # mkdtemp fails on the missing parent and the campaign silently falls back to the SSD (2026-10-27)
            self.dir = tempfile.mkdtemp(prefix="%s-%d-" % (unit.replace("/", "__"), os.getpid()), dir=want)
        except OSError as e:
            print("mutate.py: error: %s=%s: cannot create a run directory (%s); nothing written" % (src, want, e), file=sys.stderr)
            raise SystemExit(2)
        env = os.environ.get("WWHD_MUT_SCRATCH_MIN_FREE")
        try:
            size = os.path.getsize(exe) if exe and os.path.exists(exe) else 0
        except OSError:
            size = 0
        self.margin = int(env) if env else max(SCRATCH_FLOOR, 3 * size)
        print("mutate.py: mutant scratch files in %s (from %s)" % (self.dir, src), file=sys.stderr)

    def note(self, key, msg):
        with self.lock:
            if key in self.noted:
                return
            self.noted.add(key)
        print(msg, file=sys.stderr)

    def pick(self):
        """the directory for the next mutant: the run directory while it has room, else the build directory"""
        if self.dir is None:
            return self.out
        try:
            free = free_bytes(self.dir)
        except OSError:
            free = 0
        if free < self.margin:
            self.note("full", "mutate.py: scratch %s has %d MB free (< %d MB margin); mutants built in %s while it stays low" % (
                self.dir, free >> 20, self.margin >> 20, self.out))
            return self.out
        return self.dir

    def fell_back(self, d):
        if d != self.out:
            self.note("full", "mutate.py: scratch %s ran out of space mid-mutant; that mutant is rebuilt in %s" % (self.dir, self.out))

    def close(self):
        if self.dir:
            shutil.rmtree(self.dir, ignore_errors=True)  # only this run's directory (each mutant already removes its files)


class TimeoutPolicy:
    """The per-mutant run limit (2026-10-26). Fixed with --timeout N (the old behaviour, no retry). Otherwise
    max(TIMEOUT_FLOOR, TIMEOUT_FACTOR x the wall time of the unmutated unit's own run) - at load 85-139 a clean
    lib2_sead_8 run took 27 s wall for 12 s CPU, so a fixed 60 s classed good mutants as timeouts - measured again
    when the 1-minute load has moved by more than half since the last measurement. Since 2026-10-30 that
    re-measurement runs in a background thread instead of under the lock (it blocked every worker, ~10% of cascade
    wall): until it finishes, the limit is the last measured one scaled up by the load ratio (never down), so a load
    rise cannot shorten it; the 2x retry is unchanged. measure() returns the baseline wall time in seconds; loadavg()
    the 1-minute load (both replaceable for the self-test).

    Since 2026-10-31 (fasttools2, change 1) the default never re-measures (remeasure=False): the first measurement (the
    campaign's own baseline gate run, or the --baseline-cache entry's recorded wall time and load) is the base for the
    whole campaign, and the limit is that base scaled up by the load ratio (never down), as between re-measurements
    before. A background re-measurement was a full unmutated run at the campaign's -n (AGL#19 seed-1 n10000 replay: 2-4
    extra full baselines per big spec). --timeout-remeasure keeps the old behaviour. A timeout is never a kill either
    way."""
    FLOOR = 60.0
    FACTOR = 10.0

    def __init__(self, fixed, measure, loadavg=lambda: os.getloadavg()[0], first_load=None, remeasure=False):
        """first_load: the 1-minute load at which measure()'s first value was taken, when that is not now (a cached
        baseline, --baseline-cache): with remeasure, a later call re-measures as usual when the load has moved by more
        than half; without it, the limit scales with the load relative to first_load"""
        self.fixed, self.measure_fn, self.loadavg = fixed, measure, loadavg
        self.adaptive = remeasure
        self.lock = threading.Lock()
        self.base = self.load = self.value = None
        self.thread = None
        if fixed is None:
            self.remeasure(first_load)

    def remeasure(self, load=None):
        load = self.loadavg() if load is None else load
        base = self.measure_fn()
        with self.lock:
            self.load, self.base = load, base
            self.value = max(self.FLOOR, self.FACTOR * base)
        print("mutate.py: per-mutant timeout %.0f s (%.0fx the unmutated run, %.1f s at 1-min load %.0f; %s; --timeout N "
              "fixes it)" % (self.value, self.FACTOR, base, load, "measured again when the load moves by more than half"
                             if self.adaptive else "scaled up with the load, never measured again"),
              file=sys.stderr, flush=True)

    def _background(self, load):
        try:
            self.remeasure(load)
        except Exception as e:  # keep the last measurement
            print("mutate.py: per-mutant timeout re-measurement failed (%s); keeping %.0f s" % (e, self.value), file=sys.stderr)

    def wait(self):
        """the running background re-measurement, if any, finished (self-test; the thread is not a daemon, so the
        process also waits for it at exit)"""
        t = self.thread
        if t is not None:
            t.join()

    def current(self):
        """the limit for the next mutant run; with remeasure, a load move of more than 50% starts a background
        re-measurement"""
        if self.fixed is not None:
            return float(self.fixed)
        with self.lock:
            now = self.loadavg()
            if self.adaptive and abs(now - self.load) > 0.5 * max(self.load, 1.0) and \
                    (self.thread is None or not self.thread.is_alive()):
                self.thread = threading.Thread(target=self._background, args=(now,), name="timeout-remeasure")
                self.thread.start()
            return max(self.FLOOR, self.FACTOR * self.base * max(1.0, now / max(self.load, 1.0)))

    @property
    def retry(self):
        return self.fixed is None


def run_with_retry(cmd, timeout, retry, **kw):
    """subprocess.run with a limit; a run over the limit is retried once at twice the limit when retry is set.
    Returns (CompletedProcess or None when it still timed out, the limits tried)"""
    tried = [timeout]
    while True:
        try:
            return subprocess.run(cmd, timeout=tried[-1], **kw), tried
        except subprocess.TimeoutExpired:
            if not retry or len(tried) > 1:
                return None, tried
            tried.append(2 * tried[-1])


class JobPool:
    """Machine-wide job tokens shared by every mutate.py --pool (2026-10-26): WWHD_MUT_POOL_SLOTS slot files (default
    the core count, sysctl hw.ncpu) in WWHD_MUT_POOL_DIR (default /tmp/wwhd-mutpool), each held by an flock while one
    mutant is built and run (or one schema object compiled). Independent campaigns (other worktrees, on
    this branch) then share the cores instead of each running -j at once. On top, while the 1-minute load exceeds
    1.25x the cores (work outside the pool), a process that already holds a slot waits for another one: every
    campaign keeps at least one job going. loadavg is replaceable for the self-test."""

    def __init__(self, slots=None, path=None, loadavg=lambda: os.getloadavg()[0]):
        self.n = int(slots or os.environ.get("WWHD_MUT_POOL_SLOTS") or os.cpu_count() or 8)
        self.dir = path or os.environ.get("WWHD_MUT_POOL_DIR") or "/tmp/wwhd-mutpool"
        os.makedirs(self.dir, exist_ok=True)
        self.loadavg = loadavg
        self.cores = os.cpu_count() or 8
        self.lock = threading.Lock()
        self.held = 0

    def acquire(self):
        import fcntl
        import time
        start = int.from_bytes(os.urandom(2), "little") % self.n
        while True:
            with self.lock:
                busy = self.held > 0 and self.loadavg() > 1.25 * self.cores
            if not busy:
                for k in range(self.n):
                    f = open(os.path.join(self.dir, "slot.%d" % ((start + k) % self.n)), "a")
                    try:
                        fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
                    except OSError:
                        f.close()
                        continue
                    with self.lock:
                        self.held += 1
                    return f
            time.sleep(0.2)

    def release(self, f):
        with self.lock:
            self.held -= 1
        f.close()  # releases the flock

    def __call__(self, fn):
        """fn wrapped to run while holding a slot"""
        def run(*args, **kw):
            f = self.acquire()
            try:
                return fn(*args, **kw)
            finally:
                self.release(f)
        return run


class ForkServer:
    """One harness started with -forkserver for a schema executable (mutate.py --fork, 2026-10-26): image, specs and
    static initialisers load once; each run is a fork that makes one mutant live. static = the schema sites the
    static initialisers ran (those mutants need a fresh process). Used by one thread at a time."""

    def __init__(self, cmd, workdir):
        self.workdir = workdir
        self.p = subprocess.Popen(cmd + ["-forkserver"], cwd=ROOT, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                  stderr=subprocess.DEVNULL, env=dict(os.environ, WWHD_MUT_REC="1"))
        self.buf = b""
        self.closed = False
        import select
        self.poller = select.poll()  # poll, not select(): fds above FD_SETSIZE (1024) are fine (2026-10-27, NW-SND#6)
        self.poller.register(self.p.stdout.fileno(), select.POLLIN)
        line = self.readline(600)
        if line is None or not line.startswith("READY"):
            self.close()
            raise OSError("fork server did not start (%r)" % line)
        f = line.split()
        self.static = {int(x) for x in f[1].split(",")} if len(f) > 1 else set()

    def readline(self, timeout):
        import time
        end = time.time() + timeout
        while b"\n" not in self.buf:
            left = end - time.time()
            if left <= 0:
                return None
            r = self.poller.poll(max(1, int(left * 1000)))
            if r:
                chunk = os.read(self.p.stdout.fileno(), 65536)
                if not chunk:
                    raise OSError("fork server ended")
                self.buf += chunk
        line, self.buf = self.buf.split(b"\n", 1)
        return line.decode()

    def run(self, k, runlist, timeout):
        """(returncode, stdout, stderr), or None when the run passed the timeout (the child is killed)"""
        base = os.path.join(self.workdir, "fork-%d-%d" % (os.getpid(), k))
        self.p.stdin.write(("RUN %d %s %s\n" % (k, base, runlist or "-")).encode())
        self.p.stdin.flush()
        line = self.readline(60)
        if line is None or not line.startswith("PID "):
            raise OSError("fork server: %r" % line)
        pid = int(line.split()[1])
        line = self.readline(timeout)
        timed_out = line is None
        if timed_out:
            try:
                os.kill(pid, 9)
            except OSError:
                pass
            line = self.readline(60)
        if line is None or not line.startswith("DONE "):
            raise OSError("fork server: %r" % line)
        res = []
        for x in (".out", ".err"):
            try:
                with open(base + x, errors="replace") as fh:
                    res.append(fh.read())
                os.remove(base + x)
            except OSError:
                res.append("")
        return None if timed_out else (int(line.split()[1]), res[0], res[1])

    def close(self):
        """end the server and release both pipes (2026-10-27: stdout was never closed, one fd per server)"""
        if self.closed:
            return
        self.closed = True
        try:
            self.p.stdin.close()
            self.p.wait(timeout=60)
        except Exception:
            self.p.kill()
            try:
                self.p.wait(timeout=60)
            except Exception:
                pass
        try:
            self.p.stdout.close()
        except Exception:
            pass


def grouped_order(keys):
    """positions 0..n-1 ordered so equal keys are adjacent, groups in order of first appearance, stable within a group"""
    first = {}
    for k, key in enumerate(keys):
        first.setdefault(key, k)
    return sorted(range(len(keys)), key=lambda k: (first[keys[k]], k))


class ServerPool:
    """The fork servers of a campaign (mutate.py --fork). Before 2026-10-27 every worker thread kept one server per schema
    executable for the whole campaign: threads x executables harness processes and twice that many pipe fds, so a long
    campaign (NW-SND#6: 1767 of 1979 mutants) passed fd 1024 and select() raised "filedescriptor out of range". Now
      - a thread keeps at most `per_thread` servers open (least recently used one closed first),
      - all servers of an executable close as soon as its last mutant has run (`done`), and
      - a server that failed is closed and that thread runs the executable's mutants plainly (as before).
    factory(exe) -> server or raises OSError; servers have .close()."""

    def __init__(self, factory, per_thread=4):
        self.factory, self.per_thread = factory, max(1, per_thread)
        self.local = threading.local()
        self.lock = threading.Lock()
        self.by_exe = {}      # exe -> list of open servers (all threads)
        self.remaining = {}   # exe -> mutants still to run
        self.opened = 0

    def set_remaining(self, counts):
        with self.lock:
            self.remaining = dict(counts)

    def _mine(self):
        d = getattr(self.local, "d", None)
        if d is None:
            d = self.local.d = {}  # exe -> server | None (failed); insertion order = least recently used first
        return d

    def get(self, exe):
        """this thread's server for exe, started when needed; None when it failed for this thread"""
        d = self._mine()
        if exe in d:
            srv = d.pop(exe)
            if srv is None or not getattr(srv, "closed", False):
                d[exe] = srv  # most recently used
                return srv
        live = [e for e, v in d.items() if v is not None]
        while len(live) >= self.per_thread:
            e = live.pop(0)
            self._drop(e, d.pop(e))
        try:
            srv = self.factory(exe)
        except OSError:
            d[exe] = None
            raise
        with self.lock:
            self.by_exe.setdefault(exe, []).append(srv)
            self.opened += 1
        d[exe] = srv
        return srv

    def failed(self, exe):
        """this thread's server for exe failed: close it, plain runs from now on"""
        d = self._mine()
        srv = d.get(exe)
        d[exe] = None
        if srv is not None:
            self._drop(exe, srv)

    def _drop(self, exe, srv):
        with self.lock:
            lst = self.by_exe.get(exe, [])
            if srv in lst:
                lst.remove(srv)
        srv.close()

    def done(self, exe):
        """one mutant of exe has finished; after the last one every server of exe closes"""
        with self.lock:
            left = self.remaining.get(exe, 0) - 1
            self.remaining[exe] = left
            lst = self.by_exe.pop(exe, []) if left <= 0 else []
        for srv in lst:
            srv.close()

    def open_count(self):
        with self.lock:
            return sum(len(v) for v in self.by_exe.values())

    def close_all(self):
        with self.lock:
            lst = [s for v in self.by_exe.values() for s in v]
            self.by_exe = {}
        for srv in lst:
            srv.close()


def campaign_lock(unit_dir, unit):
    """an flock held for the whole campaign on <build dir>/../.<unit>.mutate.lock: campaigns of one unit in one build
    directory share its objects and mutant files, so a second one waits (2026-10-26: parallel self-tests broke each
    other's lib2_sead_15 build). The open file is returned; the lock goes with the process."""
    import fcntl
    lock = os.path.join(os.path.dirname(unit_dir), ".%s.mutate.lock" % unit.replace("/", "__"))  # companion specs: unit/sub
    os.makedirs(os.path.dirname(lock), exist_ok=True)
    f = open(lock, "a")
    try:
        fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except OSError:
        print("mutate.py: another campaign of %s uses %s; waiting for it" % (unit, unit_dir), file=sys.stderr, flush=True)
        fcntl.flock(f, fcntl.LOCK_EX)
    return f


def undefined_symbols(obj):
    """the undefined symbols of an object file (nm -u), Mach-O leading underscore removed"""
    r = subprocess.run(["nm", "-u", obj], capture_output=True, text=True)
    if r.returncode:
        raise OSError("nm -u %s: %s" % (obj, r.stderr.strip()))
    out = set()
    for line in r.stdout.split():
        out.add(line[1:] if line.startswith("_") else line)
    return out


def new_host_symbols(base_obj, mutant_obj, source_text=None):
    """C (unmangled) symbols the mutant object references and the base object does not - with source_text, only those
    spelled as an identifier in it. A mutant that drops or moves a declaration such as `u32 index = ...;` makes the
    later use bind to the host libc index() (strings.h): it compiles on the host only, and the run is a host crash
    (AGL#13: 17 mutants classed killed/signal). C++ (mangled, Z...) symbols are never host libc names; a new libc call
    the compiler makes on its own (memset for a loop) is not spelled in the source. Importable (merged-archive survey)."""
    new = {x for x in undefined_symbols(mutant_obj) - undefined_symbols(base_obj) if not x.startswith(("_Z", "Z"))}
    if source_text is not None:
        new = {x for x in new if re.search(r"(?<![\w.>])%s\b" % re.escape(x), source_text)}
    return new


FUNC_VALUE_WARNING = re.compile(r"warning: [^\n]*\bfunction '([A-Za-z_]\w*)'")


def function_value_warnings(cpp, cflags):
    """names in clang warnings about a function used as a value ("comparison of function 'index' equal to a null
    pointer is always false", "address of function 'index' will always evaluate to 'true'") for one -fsyntax-only
    compile of cpp with cflags (any -w removed: these warnings are on by default)"""
    r = subprocess.run(["clang++"] + [f for f in cflags if f != "-w"] + ["-fsyntax-only", cpp], cwd=ROOT,
                       capture_output=True, text=True)
    return set(FUNC_VALUE_WARNING.findall(r.stderr))


def host_function_values(base_cpp, mutant_cpp, cflags, source_text=None):
    """the companion of new_host_symbols: functions the mutant uses as a value that the unmutated source does not -
    with source_text, only names spelled in it. A swap of `s16 index = ...;` with `if (index == 0) return ...;`
    binds index to libc index(), and clang folds the comparison to false, so the object references nothing new
    (17 of 20 surveyed AGL cases); the warning still names it. Importable (merged-archive survey)."""
    new = function_value_warnings(mutant_cpp, cflags) - function_value_warnings(base_cpp, cflags)
    if source_text is not None:
        new = {x for x in new if re.search(r"(?<![\w.>])%s\b" % re.escape(x), source_text)}
    return new


MIN_SHARD_INPUTS = int(os.environ.get("WWHD_MUT_MIN_SHARD_INPUTS", "250"))  # generated inputs per -range part at least
# (mutate.py's sharded serial phases, 2026-10-31; 250 by 
# per-part dictionary pre-runs and snapshot capture; the env override exists for self-tests)


def shard_parts(n, k):
    """-range parts for n generated inputs and k requested parts: at least MIN_SHARD_INPUTS inputs per part"""
    return max(1, min(k, n // MIN_SHARD_INPUTS))
CACHE_LINE = re.compile(r"^refcache: (\d+) input\(s\) took the recorded original, (\d+) ran it live\n?", re.M)


DIGEST_WRITE_LINE = re.compile(r"^refcache: wrote digest records of (\d+) input\(s\); the unmutated candidate matches (\d+) of them\n?",
                               re.M)


def sum_cache_lines(err):
    """the -range parts' stderr with their `refcache: H input(s) took the recorded original, L ran it live` lines (and
    the digest writers' `refcache: wrote digest records of N input(s); the unmutated candidate matches M of them`) summed
    into one at the end (as one process prints it)"""
    for pat, fmt in ((CACHE_LINE, "refcache: %d input(s) took the recorded original, %d ran it live\n"),
                     (DIGEST_WRITE_LINE, "refcache: wrote digest records of %d input(s); the unmutated candidate matches %d of them\n")):
        found = pat.findall(err)
        if len(found) >= 2:
            err = pat.sub("", err) + fmt % (sum(int(h) for h, _ in found), sum(int(l) for _, l in found))
    return err


def rm(*fs):
    for f in fs:
        try:
            os.remove(f)
        except OSError:
            pass


def nospace(text):
    return "No space left on device" in text or "ENOSPC" in text


def failing_rows(out):
    """the function rows of a harness output that are not ok. A passing row may end in "ok (some inputs aborted on both
    sides)" (2026-10-27: such rows used to count as differing rows)."""
    res = []
    for l in out.splitlines():
        if not re.match(r"^[0-9A-F]{8}  ", l):
            continue
        m = verify.RESULT_LINE.match(l)
        if m and m.group(9).startswith("ok") and m.group(3) == m.group(4) and m.group(5) == m.group(6):
            continue
        res.append(l.rstrip())
    return res


def check_baseline(rc, out, what):
    """None when the unmutated run passed (rc 0, at least one row, every row ok), else the message naming every
    non-ok row. A campaign whose unmutated baseline fails is void: every mutant run fails on the same rows and would
    count as a kill (2026-10-27, AGL#8: 3,165/3,165 "killed" with 0279345C failing unmutated)."""
    rows = [l for l in out.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)]
    bad = failing_rows(out)
    if rc == 0 and rows and not bad:
        return None
    msg = ["mutate.py: the unmutated baseline (%s) does not pass: exit %d, %d of %d function row(s) not ok" % (
        what, rc, len(bad), len(rows))]
    msg += ["  " + l for l in bad]
    if not rows:
        msg.append("  (no function rows; output tail:)")
        msg += ["  " + l for l in out.splitlines()[-15:]]
    msg.append("mutate.py: no mutant was run. A campaign on a failing baseline is void (every mutant would fail on the same "
               "rows and count as a kill): fix the spec or the candidate until the baseline passes "
               "(verify.py with the same -n/-seed/scope), or use --allow-failing-baseline to leave these functions out.")
    return "\n".join(msg)


def classify(rc, out):
    """a nonzero mutant run: 'difference' (a function row differs), 'abort' (only the mutant side aborted),
    'signal' (host signal or crash), or 'invalid' (no differing row: harness or spec error)"""
    if rc < 0 or "host signal" in out:
        return "signal"
    rows = failing_rows(out)
    if not rows:
        return "invalid"
    if all("only one side aborted" in l for l in rows):
        return "abort"
    return "difference"


def main():
    import verify as _scratch_verify  # noqa: E402  (same directory)
    _scratch_verify.check_scratch_env('mutate.py')  # a set scratch variable naming a missing/unwritable folder stops here, nothing written
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--n", type=int, default=300)
    ap.add_argument("--max", type=int, default=400)
    ap.add_argument("--seed", type=int, default=1, help="mutant selection order; also the harness input seed unless --input-seed is given")
    ap.add_argument("--input-seed", type=int, help="harness -seed (generated inputs); default: --seed")
    ap.add_argument("--rerun", help="receipts directory of an earlier campaign: rerun exactly its recorded mutants "
                    "(mutantN.cpp as recorded, same IDs) of the classes in --rerun-class; --max/--func/--seed do not select")
    ap.add_argument("--ids", help="comma-separated mutant IDs: run only these from the campaign this command line (same --seed/--max/"
                    "--func/--only, unchanged sources) enumerates; receipt-free survivor replay")
    ap.add_argument("--rerun-class", default="survived", help="comma-separated classes to rerun with --rerun (default survived)")
    ap.add_argument("--rec")
    ap.add_argument("--func", help="comma-separated addresses: mutate only these functions")
    ap.add_argument("--only", help="comma-separated addresses: the test scope. Every mutant run checks only these functions "
                    "(passed to the harness as -only); mutants are made only in these functions (and --func must be a subset). "
                    "For a spec that links support sources it does not own (2026-10-21, JS-JParticle#3)")
    ap.add_argument("--nulltrap", action="store_true", help="harness -nulltrap on every mutant run: a page-0 access (console DSI) "
                    "ends that side's run as a trap event, so a mutant-only NULL dereference is a `difference`, not an abort or "
                    "timeout (also from a `# verify: -nulltrap` spec header or the unit directive `nulltrap`)")
    ap.add_argument("-j", type=int, default=1)
    ap.add_argument("--scratch", help="directory for the per-mutant scratch files (mutantN.cpp/.o/executable), e.g. a RAM disk; "
                    "a per-run subdirectory is made there. Default: env WWHD_MUT_SCRATCH, else the unit's build directory. "
                    "Falls back to the build directory when DIR is missing, not writable or low on space")
    ap.add_argument("--no-schema-confirm", action="store_true",
                    help="with --schemata: do not rerun schema survivors as plain per-mutant builds (default: every "
                    "schema survivor is confirmed by its own build, since a schema build can fold an uninitialised read "
                    "differently)")
    ap.add_argument("--schemata", action="store_true",
                    help="build all schema-able mutants of a source into one executable (tools/verify/mutschema.py) and pick the "
                    "live one at run time (WWHD_MUT_ACTIVE) instead of compiling and linking each mutant; the rest are built "
                    "one by one as before. Off by default (2026-10-26)")
    ap.add_argument("--allow-host-symbols", action="store_true",
                    help="do not class a mutant broken when its object newly references a host C symbol spelled in its source "
                    "(e.g. a dropped `u32 index = ...;` binding later uses to libc index()); the old behaviour (2026-10-26)")
    ap.add_argument("--no-preamble-helpers", action="store_true",
                    help="do not mutate file-local helpers defined above a source's first marked function (the old "
                    "enumeration); since 2026-10-26 they are mutated, as IDs after the old campaign's")
    ap.add_argument("--pool", action="store_true",
                    help="hold a machine-wide job slot (shared by every mutate.py --pool, WWHD_MUT_POOL_SLOTS, default the core "
                    "count) for each mutant's build and run, and wait for more slots while the load exceeds 1.25x the cores; "
                    "-j is then only an upper bound. Off by default (2026-10-26)")
    ap.add_argument("--fork", action="store_true",
                    help="with --schemata: start each schema executable once (harness -forkserver) and fork one run per mutant "
                    "instead of starting the harness for each (start-up, image and specs load once). Off by default (2026-10-26)")
    ap.add_argument("--reach-skip", action="store_true",
                    help="run each mutant only on the functions and inputs that reach its site (recorded once on a marker build "
                    "of the unit, tools/verify/mutschema.py), stopping at the first output difference; a mutant no input "
                    "reaches is a survivor marked 'not reached' and is not built. Off by default (2026-10-26)")
    ap.add_argument("--timeout", type=int, help="seconds per mutant run, fixed (the old behaviour; a run that exceeds it is reported "
                    "as TIMEOUT, not killed; no retry, nothing measured). Default: max(60, 10x the unmutated baseline's wall "
                    "time), scaled up (never down) by the 1-minute load relative to the load at that baseline, never measured "
                    "again (2026-10-31), and a run over it is retried once at twice the limit before it counts as TIMEOUT")
    ap.add_argument("--shards", type=int, help="run the serial phases (the unmutated baseline gate, the --schemata check per schema "
                    "executable, a --timeout-remeasure measurement) as this many -range parts in parallel and merge them "
                    "(verify.run_range_shards; record and validation-state parts are joined, .reach parts concatenated). "
                    "Default (2026-10-31): -j, at most one part per %d inputs; 1 = unsharded (also env WWHD_MUT_SHARDS)" % MIN_SHARD_INPUTS)
    ap.add_argument("--timeout-remeasure", action="store_true",
                    help="the derived timeout's behaviour before 2026-10-31: re-run the whole unmutated baseline in the background "
                    "whenever the 1-minute load has moved by more than half since the last measurement")
    ap.add_argument("--whole-unit", action="store_true",
                    help="run every mutant on the whole campaign scope (every function under test, or --only), the behaviour "
                    "before 2026-10-27. Default: each mutant runs only the functions whose rows can execute its changed code "
                    "(tools/verify/mutscope.py: the mutated function, plus every function using a mutated helper or a host static "
                    "it touches; the whole scope when that cannot be bounded); the receipts name the scope")
    ap.add_argument("--no-stopfirst", action="store_true",
                    help="run every row of a mutant to the end. Default (2026-10-27): harness -stopfirst on every mutant run (the "
                    "first row whose first difference is an output difference ends the run; a row whose first difference is an "
                    "abort ends that row only, so the difference/abort split is kept), as --reach-skip did before; the baseline "
                    "gate always runs every row")
    ap.add_argument("--observe-pointees", action="store_true",
                    help="harness -observe-pointees on the baseline and every mutant run (2026-10-28, opt-in): at each mocked "
                    "call, an undeclared pointer argument into a heap block (`observeheap` heaps) or a static object also has "
                    "that object's bytes compared (also from a `# verify: -observe-pointees` spec header). The baseline gate "
                    "runs with it, so the campaign is creditable only when the unmutated baseline passes with it")
    ap.add_argument("--no-refcache", action="store_true",
                    help="run the original side in every mutant run. Default (2026-10-28): the unmutated baseline run (same "
                    "spec, -n, seed, flags, nulltrap) records the original's result for every generated input of every function "
                    "(harness -refcache-write, in a per-campaign directory on the scratch, removed at exit) and every mutant run "
                    "loads it instead of running the original again (harness -refcache); rows, traps and totals are the same")
    ap.add_argument("--refcache-full", action="store_true",
                    help="the baseline writes full reference-side records (the whole original Result per input; the format "
                    "before 2026-10-31). Default: digest records (harness -refcache-digest): per input only what the "
                    "candidate's run and the outcome need exactly, plus a 128-bit hash of the original's calls, net memory "
                    "effect and `final` bytes; a mutant run whose candidate hashes the same takes the original from the "
                    "record, any other runs the original live and compares in full (about 1 KB per input instead of up "
                    "to 700 KB, so big functions stay under WWHD_REFCACHE_MAX_MB)")
    ap.add_argument("--no-snapcache", action="store_true",
                    help="run the `setupsnapshot` setups live in every mutant run. Default (2026-10-30): the unmutated "
                    "baseline also stores each function's setup snapshot and the post-setup state of its validation inputs "
                    "(harness -snapcache-write, ADDR-TAG.snap / world-HASH.blob / ADDR-TAG-sSEED.val in the reference-side cache directory, so they share "
                    "its lifetime and travel with --baseline-cache entries) and every mutant run loads them instead of running "
                    "the setups (harness -snapcache); rows are the same. Off with --no-refcache too. WWHD_SNAPCACHE_CHECK=1 "
                    "adds -snapcache-check to every mutant run (validation)")
    ap.add_argument("--refcache-from", metavar="DIR",
                    help="reuse a gate's original-side records (2026-10-30): DIR/SPEC/s<input seed> (or DIR/s<seed>, DIR) as written by "
                    "`nulltrap_compare.py --refcache-out` for this spec. Used only when its manifest's key (objects, spec, "
                    "harness options, recordings, tools), n and seed equal this campaign's exactly; the unmutated baseline then "
                    "runs only the candidate side against the records (it still has to pass) and every mutant run reads them. "
                    "Otherwise the baseline runs and records as without it")
    ap.add_argument("--no-tce", action="store_true",
                    help="no identical-code check. Default (2026-10-28): every mutant is compiled exactly like a per-mutant "
                    "build (same compiler, flags and defines, the source's own file name) and its whole object compared with the "
                    "unmutated source's; an identical object is closed as `equivalent-identical-code` (EQ: compiles to identical "
                    "machine code) and never run, and mutants whose objects are identical to each other (same run scope) run "
                    "once, the outcome applying to all (the receipts name the representative). Schema mutants (--schemata) are "
                    "compiled for the check only when their run did not kill them (or before the run with --ids)")
    ap.add_argument("--no-receipts", action="store_true", help="keep no receipts (default since 2026-10-27: a receipts "
                    "directory per campaign, see --receipts)")
    ap.add_argument("--allow-failing-baseline", action="store_true",
                    help="when the unmutated baseline has failing function rows, leave those functions out (not tested, not "
                    "mutated; named in every receipt as baseline_excluded) instead of stopping. Default: stop before any mutant")
    ap.add_argument("--baseline-cache", metavar="DIR",
                    help="reuse a PASSING unmutated baseline (verdict, function rows, wall time, reference-side records) "
                    "recorded in DIR by an earlier campaign with exactly the same key (executable bytes, spec, harness "
                    "command, run-time files, tools, environment; tools/verify/basecache.py), and record this campaign's "
                    "passing baseline there. A failing baseline is never recorded. Also env WWHD_BASELINE_CACHE. Off by "
                    "default (2026-10-30)")
    ap.add_argument("--baseline-only", action="store_true",
                    help="build, run the baseline gate (recording it with --baseline-cache) and stop before any mutant: "
                    "exit 0 when it passes, 3 when it does not (basecache.py prefetch runs companion baselines this way)")
    ap.add_argument("--receipts", help="directory for per-mutant receipts: mutantN.cpp (the mutated source) and mutantN.json "
                    "(function, hunk, compile/link/run return codes, stdout/stderr tails, classification)")
    ap.add_argument("--no-progress", action="store_true", help="no `[progress]` lines and no progress file (also "
                    "WWHD_VERIFY_PROGRESS=0). Default (2026-10-27): every WWHD_VERIFY_PROGRESS_INTERVAL s (10) a line on stderr "
                    "with mutants done/total, kills so far and ETA, and the file mutate-PID.json (tools/verify/progress.py). The "
                    "harness runs of the campaign always run without their own progress and wall-clock hang limit, so receipts "
                    "and verdicts are unchanged")
    a = ap.parse_args()
    if a.scratch:
        verify.require_scratch_dir("--scratch", a.scratch, "mutate.py")  # before any build: missing -> stop, nothing written
    prog = progress.Settings()
    if a.no_progress:
        prog.on = False
    # mutant runs: no harness progress lines (receipts keep stderr tails) and no wall-clock hang limit (verdicts and the
    # adaptive timeout stay exactly as before)
    os.environ["WWHD_VERIFY_PROGRESS"] = "0"
    if a.fork and not a.schemata:
        sys.exit("mutate.py: --fork needs --schemata (it forks schema executables)")
    if a.input_seed is None:
        a.input_seed = a.seed
    if a.unit.startswith("_diag/"):
        sys.exit("mutate.py: %s is diagnosis only: it can support no credit, so it gets no receipt" % a.unit)
    hdr = verify.header_args(a.unit)  # `# verify: -only ... [-calleesp]` in the spec header
    hdr_extra = []
    i = 0
    while i < len(hdr):
        if hdr[i] == "-only":
            if not a.only:
                a.only = hdr[i + 1]
                print("mutate.py: test scope from the spec header: --only " + a.only)
            i += 2
        else:
            hdr_extra.append(hdr[i])
            i += 1
    only = {int(x, 16) for x in a.func.split(",")} if a.func else None
    scope = parse_scope(a.only) if a.only else None
    if scope is not None:
        if only is not None and not only <= scope:
            sys.exit("mutate.py: --func %s is outside the --only test scope" % ",".join("%08X" % x for x in sorted(only - scope)))
        only = only if only is not None else set(scope)
    unit_lock = campaign_lock(verify.unit_dir(a.unit), a.unit)  # noqa: F841 (held until exit)
    exe = verify.build(a.unit, jobs=a.j)  # baseline objects with the campaign's -j parallel compilers
    srcs = []
    for line in specfile.expanded_lines(os.path.join(HERE, "units", a.unit + ".txt")):
        f = line.split("#")[0].split()
        if f and f[0] == "src":
            srcs += f[1:]
    out = verify.unit_dir(a.unit)  # build/verify/<unit>, or its WWHD_VERIFY_SCRATCH copy (the same one verify.build used)
    spec = specfile.spec_path(a.unit, ROOT, out)

    # per-mutant scope (2026-10-27): the functions of the campaign scope whose rows can run a mutant's changed code
    base_rows = None  # the campaign scope's function rows (the full baseline's rows); set before the first mutant_scope call
    scoper = None
    # `osmem ... threads` (2026-11-02): a guest thread, AX callback or alarm callback runs a unit function's candidate code in
    # any row (harness gthr::unit_fn), so mutscope's premise (a marked function's body runs only in its own row) does not hold
    # and a narrowed scope can leave a false survivor: every mutant runs the whole campaign scope
    thread_spec = any(f[:1] == ["osmem"] and "threads" in f for f in (l.split("#")[0].split() for l in
                      specfile.expanded_lines(os.path.join(HERE, "units", a.unit + ".txt"))))
    whole_why = "--whole-unit" if a.whole_unit else "spec declares `osmem ... threads`" if thread_spec else None
    if thread_spec and not a.whole_unit:
        print("mutate.py: the spec declares `osmem ... threads`: every mutant runs the whole campaign scope", file=sys.stderr)
    if whole_why is None:
        try:
            scoper = mutscope.Scope(srcs, list(specfile.expanded_lines(os.path.join(HERE, "units", a.unit + ".txt"))), ROOT)
        except Exception as ex:  # a source the scope parser cannot read: every mutant runs the whole scope, as before
            print("mutate.py: per-mutant scope unavailable (%s); every mutant runs the whole campaign scope" % ex, file=sys.stderr)
    src_texts = {}
    scope_stats = {"scoped": 0, "whole": 0, "funcs": 0, "widened": 0}
    scope_lock = threading.Lock()

    def mutant_scope(src, addr, mt):
        """(set of functions, None) or (None, reason): the functions this mutant runs (None: the whole campaign scope)"""
        if scoper is None:
            return None, whole_why or "scope parser unavailable"
        if src not in src_texts:
            src_texts[src] = open(os.path.join(ROOT, src)).read()
        sp = mutscope.changed_span(src_texts[src], mt)
        if sp is None:
            aff, why = {addr}, None
        else:
            try:
                aff, why = scoper.affected(src, sp[0], sp[1])
            except Exception as ex:
                aff, why = None, "scope error: %s" % ex
        if aff is None:
            return None, why
        aff = (set(aff) | {addr}) & base_rows
        if not aff:
            return None, "no affected function in the campaign scope"
        if aff >= base_rows:
            return None, "every function of the campaign scope"
        return aff, None
    refc = {"dir": None, "key": None}  # the reference-side cache (refcache_setup): written by the baseline, read by mutant runs
    snapc = {"dir": None, "key": None}  # the setup-snapshot cache (files in the refcache directory): the same lifetime

    def harness_cmd(exe, fscope=None, cache=False):
        """the harness command; fscope: this mutant's functions (-only), else the campaign scope (every function or --only);
        cache: a mutant run, which reads the baseline's reference-side records (-refcache)"""
        # -nulltrap: console DSI on page-0 accesses (also via the spec header / unit directive); -observe-pointees: pointee
        # objects at mocked calls (part of the configuration); -only: this mutant's functions, else the same test scope
        # for every mutant; -rec: the unit file's `rec DIR` unless --rec is given. refcache.harness_args keeps this order
        # (nulltrap_compare.py --refcache-out computes the same key from it)
        cmd = [exe] + refcache.harness_args(spec, a.n, a.input_seed, hdr_extra, a.nulltrap, a.observe_pointees,
                                            fscope if fscope is not None else scope, a.rec or verify.unit_rec(a.unit))
        if cache and refc["dir"]:
            cmd += ["-refcache", refc["dir"], "-refcache-key", refc["key"]]
            if os.environ.get("WWHD_REFCACHE_CHECK") == "1":  # validation: run the original anyway, exit 97 on any difference
                cmd.append("-refcache-check")
        if cache and snapc["dir"]:
            cmd += ["-snapcache", snapc["dir"], "-snapcache-key", snapc["key"]]
            if os.environ.get("WWHD_SNAPCACHE_CHECK") == "1":  # validation: run the setups anyway, exit 98 on any difference
                cmd.append("-snapcache-check")
        return cmd

    def refcache_setup(staged=None):
        """the per-campaign record directory and its key (sha256 of everything the original side depends on: the objects
        of unit.c, the FP runtime, the harness and the support sources, the expanded spec, the harness options of the
        campaign, the recordings), or nothing with --no-refcache / no room. staged: the record directory to use (a
        --baseline-cache staging directory, which has room), else a new one on the scratch"""
        if a.no_refcache:
            return []
        if staged is not None:
            d = staged
        else:
            want, _ = mutant_scratch_request(a.scratch)
            base = want if want and os.path.isdir(want) and os.access(want, os.W_OK) else verify.verify_scratch_base() or out
            try:
                if shutil.disk_usage(base).free < REFCACHE_MIN_FREE:
                    print("mutate.py: reference-side cache off: less than %d MB free in %s" % (REFCACHE_MIN_FREE >> 20, base),
                          file=sys.stderr)
                    return []
                d = tempfile.mkdtemp(prefix="refcache-%s-%d-" % (a.unit.replace("/", "__"), os.getpid()), dir=base)
            except OSError as e:
                print("mutate.py: reference-side cache off (%s)" % e, file=sys.stderr)
                return []
        atexit.register(shutil.rmtree, d, True)  # a staged directory moved into the cache entry is no longer there
        refc["key"] = refcache_key()
        refc["pending"] = d
        print("mutate.py: reference-side cache in %s (--no-refcache to run the original in every mutant run)" % d, file=sys.stderr)
        extra = []
        if not a.no_snapcache:  # the setup-snapshot cache: its files go into the same directory
            extra = ["-snapcache-write", d, "-snapcache-key", snapcache_key()]
        return ["-refcache-write", d, "-refcache-key", refc["key"]] + ([] if a.refcache_full else ["-refcache-digest"]) + extra

    def snapcache_key():
        """-snapcache-key: the reference-side key without -n/-seed (a snapshot depends on neither; the harness keys the
        validation states by input seed and constant dictionary itself)"""
        args, k, kept = harness_cmd("")[1:], 0, []
        while k < len(args):
            if args[k] in ("-n", "-seed"):
                k += 2
                continue
            kept.append(args[k])
            k += 1
        return refcache.key(out, spec, kept, a.rec or verify.unit_rec(a.unit), ROOT, verify.support_objs())

    def refcache_key():
        """-refcache-key: sha256 of the original side's objects, the support objects, the spec, the campaign's harness
        options, the recordings and the tools' Python sources (refcache.key, shared with nulltrap_compare --refcache-out
        and the --baseline-cache key)"""
        return refcache.key(out, spec, harness_cmd("")[1:], a.rec or verify.unit_rec(a.unit), ROOT, verify.support_objs())

    def refcache_gate():
        """--refcache-from: a gate's records of exactly this configuration -> True (refc points at them; the
        baseline runs the candidate side only and records nothing), else False with the reason printed"""
        if a.no_refcache or not a.refcache_from:
            return False
        k = refcache_key()
        d, m = refcache.gate_dir(a.refcache_from, a.input_seed, a.unit)
        why = m if d is None else refcache.check_manifest(m, k, a.n, a.input_seed)
        if why is None:
            refc.update(dir=d, key=k, reuse=True)
            print("mutate.py: reference-side cache: reusing the gate's records in %s (key %s; the baseline runs the "
                  "candidate side only)" % (d, k), file=sys.stderr)
            return True
        print("mutate.py: --refcache-from %s not used: %s; the baseline records its own" % (a.refcache_from, why),
              file=sys.stderr)
        return False

    def rerun_entries():
        """--rerun: the recorded mutants of the classes in --rerun-class, [(id, src, func, (mutation, 0, 0, ""), text)] by id"""
        want = set(a.rerun_class.split(","))
        res = []
        for fn in os.listdir(a.rerun):
            mm = re.fullmatch(r"mutant(\d+)\.json", fn)
            if not mm:
                continue
            rj = json.load(open(os.path.join(a.rerun, fn)))
            cpp = os.path.join(a.rerun, "mutant%s.cpp" % mm.group(1))
            if rj.get("class") not in want:
                continue
            if not os.path.exists(cpp):
                sys.exit("mutate.py: --rerun %s: %s has no mutant%s.cpp" % (a.rerun, fn, mm.group(1)))
            if rj["src"] not in srcs:
                sys.exit("mutate.py: --rerun: mutant %s mutates %s, not a src of %s" % (mm.group(1), rj["src"], a.unit))
            res.append((int(mm.group(1)), rj["src"], int(rj["func"], 16), (rj["mutation"], 0, 0, ""), open(cpp).read()))
        return sorted(res)

    def listed_rows():
        """the functions the full baseline prints a row for, without running it: the harness's candidates (-list) in the
        campaign scope (--only / the spec header's -only)"""
        r = subprocess.run(harness_cmd(exe) + ["-list"], cwd=ROOT, capture_output=True, text=True, errors="replace")
        rows = {int(x, 16) for x in r.stdout.split() if re.fullmatch(r"[0-9A-F]{8}", x)}
        if r.returncode or not rows:
            raise OSError("harness -list: exit %d, %d function(s)" % (r.returncode, len(rows)))
        return rows if scope is None else rows & scope

    def shard_count(jobs):
        """-range parts for a serial phase given `jobs` processes for it (fasttools2 change 3): --shards / WWHD_MUT_SHARDS,
        else jobs; at least MIN_SHARD_INPUTS generated inputs per part (each part runs every function's dictionary pre-runs
        and setup snapshot capture again)"""
        env = os.environ.get("WWHD_MUT_SHARDS")
        k = int(env) if env else a.shards if a.shards else jobs
        return shard_parts(a.n, k)

    def run_sharded(cmd, k, rec_dir=None, **kw):
        """cmd as k -range parts (verify.run_range_shards); rec_dir: the -refcache-write/-snapcache-write directory, whose
        parts are joined. Returns (CompletedProcess, summed shard wall time) or (None, problems) when a part failed"""
        res = verify.run_range_shards(cmd, a.n, k, **kw)
        if res["problems"]:
            return None, res["problems"]
        if rec_dir:
            for name, why in sorted(refcache.join_shard_dir(rec_dir, a.n).items()):
                print("mutate.py: sharded run: %s: no joined file (%s); readers run it live" % (name, why), file=sys.stderr)
        return subprocess.CompletedProcess(cmd, res["rc"], res["stdout"], sum_cache_lines(res["stderr"])), sum(res["walls"])

    # --rerun (2026-10-31, fasttools2 change 2): the baseline gate (and with it the reference-side records, the setup
    # snapshots and the timeout base) runs only the union of the rerun mutants' per-mutant scopes (mutant_scope, the same
    # function sets every mutant run then uses with -only), not every function of the spec: a cascade pass that reruns one
    # mutant of a 39-function companion ran a 2,172 s baseline for a 3-function scope (AGL#19 banks_prepass_accessors,
    # n10000). Each function's inputs depend on nothing that ran before it in the process (the dictionary pre-runs' clobber
    # switch is fixed since 2026-10-29), so its rows and records are the same in the scoped run (self-test
    # mutate_rerun_scope_test.py compares rows and record files). The whole campaign scope runs as before when one mutant's
    # scope cannot be bounded (or is every function), with --whole-unit / --allow-failing-baseline, or with
    # WWHD_MUT_RERUN_BASELINE_WHOLE=1. A function outside the union is not run at all, so its failure no longer voids the
    # pass: no rerun mutant can change its rows (mutscope), and no mutant run includes it.
    bscope, bscope_why = None, None  # the baseline's -only (None: the campaign scope)
    if a.rerun:
        if whole_why or scoper is None:
            bscope_why = whole_why or "scope parser unavailable"
        elif a.allow_failing_baseline:
            bscope_why = "--allow-failing-baseline"
        elif os.environ.get("WWHD_MUT_RERUN_BASELINE_WHOLE") == "1":
            bscope_why = "WWHD_MUT_RERUN_BASELINE_WHOLE=1"
        else:
            try:
                base_rows = listed_rows()
            except OSError as e:
                bscope_why = str(e)
            if base_rows is not None:
                u = set()
                for i, src, addr, m, text in rerun_entries():
                    fsc, why = mutant_scope(src, addr, text)
                    if fsc is None:
                        u, bscope_why = None, "mutant %d: %s" % (i, why)
                        break
                    u |= fsc
                if u is not None and u and u < base_rows:
                    bscope = u
                elif u is not None:
                    bscope_why = "the rerun mutants' functions are every function of the campaign scope" if u else "no rerun mutant"
                if bscope is None:
                    base_rows = None  # the full baseline's rows, as before
        print("mutate.py: --rerun baseline scope: %s" % (
            "%d of %d function(s), the union of the rerun mutants' scopes (%s)" % (
                len(bscope), len(base_rows), ",".join("%08X" % x for x in sorted(bscope))) if bscope is not None else
            "the whole campaign scope (%s)" % bscope_why), file=sys.stderr, flush=True)

    # the unmutated baseline in the campaign's exact configuration (spec, -n, seed, header flags, nulltrap, scope,
    # recordings) must pass before any mutant runs (2026-10-27)
    import time as _time
    first_wall = []
    first_load = None  # the load at which first_wall was measured, when it was not just now (a cached baseline)
    baseline_excluded = []

    # --baseline-cache: a passing baseline with exactly this key, recorded by an earlier campaign
    bcache, bc_info, bc_lock, cached = None, None, contextlib.ExitStack(), None
    bc_dir = a.baseline_cache or os.environ.get("WWHD_BASELINE_CACHE")
    if bc_dir:
        try:
            bcache = basecache.Cache(bc_dir)
            if not a.no_refcache and shutil.disk_usage(bcache.root).free < REFCACHE_MIN_FREE:
                print("mutate.py: baseline cache off: less than %d MB free in %s" % (REFCACHE_MIN_FREE >> 20, bcache.root),
                      file=sys.stderr)
                bcache = None
        except OSError as e:
            print("mutate.py: baseline cache off (%s)" % e, file=sys.stderr)
            bcache = None
    if bcache:
        rk = None if a.no_refcache else refcache_key()
        cmd0 = harness_cmd(exe, bscope) + (["-refcache-write", basecache.RECORD_PLACEHOLDER, "-refcache-key", rk] +
                                            ([] if a.refcache_full else ["-refcache-digest"]) if rk else [])
        bkey, bparts = basecache.baseline_key(cmd0, rk, list(specfile.expanded_lines(os.path.join(HERE, "units", a.unit + ".txt"))))
        bc_info = {"key": bkey, "use": "ran"}
        bc_lock.enter_context(bcache.lock(bkey))  # held until this baseline is looked up, run and stored
        cached = bcache.lookup(bkey, rk, lambda rc, so: check_baseline(rc, so, "cached"))
    rc_write = []
    if cached:
        bc_info["use"] = "hit"
        if a.refcache_from and not a.no_refcache:  # a hit needs no baseline run at all: it wins over the gate's records
            print("mutate.py: --refcache-from %s not used: the baseline cache has this configuration's passing baseline and "
                  "records" % a.refcache_from, file=sys.stderr)
        refc["key"], refc["dir"] = rk, cached["refcache_dir"]
        first_wall[:] = [cached["wall"]]
        first_load = cached["load"]
    elif refcache_gate():  # no cache hit: the gate's records instead of an own recording baseline; nothing
        bcache = None      # goes into the baseline cache (an entry carries its own records and a full-baseline wall time)
        bc_lock.close()
    else:
        rc_write = refcache_setup(bcache.staging(bkey) if bcache and rk else None)

    jp = JobPool() if a.pool else None  # (the mutant phase's own pool comes later)

    def run_baseline():
        """the baseline gate run; first_wall = its wall time (sharded: the shards' summed wall time, as one process would
        take: the base of the per-mutant timeout)"""
        cmd = harness_cmd(exe, bscope, cache=refc.get("reuse", False)) + rc_write
        k = shard_count(a.j)
        t = _time.time()
        if k > 1:  # fasttools2 change 3: -range parts in parallel, merged (rows) and joined (records, validation states)
            r, w = run_sharded(cmd, k, rec_dir=refc.get("pending") if rc_write else None, wrap=jp)
            if r is not None:
                first_wall[:] = [w]
                print("mutate.py: baseline gate as %d -range parts in parallel: %.1f s wall, %.1f s summed over the parts" % (
                    k, _time.time() - t, w), file=sys.stderr, flush=True)
                return r
            print("mutate.py: sharded baseline incomplete (%s); running it unsharded" % "; ".join(w), file=sys.stderr, flush=True)
            if rc_write:  # this campaign's own record directory: the parts and whatever the shards finished
                for f in os.listdir(refc["pending"]):
                    rm(os.path.join(refc["pending"], f))
            t = _time.time()
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, errors="replace")
        first_wall[:] = [_time.time() - t]
        return r
    what = " ".join(os.path.basename(x) if k == 0 else x for k, x in enumerate(harness_cmd(exe, bscope)))
    if cached:
        r = subprocess.CompletedProcess(cmd0, 0, cached["stdout"], "")
    else:
        r = run_baseline()
    msg = check_baseline(r.returncode, r.stdout, what)
    if msg and a.allow_failing_baseline and r.returncode in (0, 1):
        rows = [l for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)]
        bad = {int(l[:8], 16) for l in failing_rows(r.stdout)}
        keep = {int(l[:8], 16) for l in rows} - bad
        if keep:
            baseline_excluded = sorted(bad)
            print(msg.split("\nmutate.py: no mutant")[0], file=sys.stderr)
            print("mutate.py: --allow-failing-baseline: leaving out %s (not tested, not mutated; every receipt names them as "
                  "baseline_excluded)" % " ".join("%08X" % x for x in baseline_excluded), file=sys.stderr)
            scope = keep if scope is None else scope - bad
            only = set(keep) if only is None else only - bad
            r = run_baseline()
            msg = check_baseline(r.returncode, r.stdout, what + " without the excluded functions")
    if msg:
        print(msg, file=sys.stderr)
        sys.exit(3)
    if rc_write:
        refc["dir"] = refc["pending"]  # the baseline that passed wrote the records
    if refc.get("reuse"):
        mm = re.search(r"^refcache: (\d+) input\(s\) took the recorded original, (\d+) ran it live$", r.stderr, re.M)
        print("mutate.py: baseline against the gate's records: %s" % (
            "%s input(s) from the records, %s run live" % mm.groups() if mm else "no refcache line"), file=sys.stderr)
    if bcache and not cached and not baseline_excluded:  # only a pass of the unchanged configuration is recorded
        if bcache.store(bkey, bparts, cmd0, r.returncode, r.stdout, first_wall[0], os.getloadavg()[0], rk,
                        refc["dir"] if rc_write else None):
            bc_info["use"] = "stored"
            if rc_write:
                refc["dir"] = os.path.join(bcache.entry(bkey), "refcache")  # the records now live in the entry
    bc_lock.close()
    print("mutate.py: unmutated baseline passes (%d function rows%s, %.1f s%s)" % (
        len([l for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)]),
        "" if bscope is None else " of %d, scoped to the rerun mutants" % len(base_rows), first_wall[0],
        "; cached baseline %s from %s, not run again" % (bc_info["key"][:16], cached["time"]) if cached else
        "; recorded in the baseline cache as %s" % bc_info["key"][:16] if bc_info and bc_info["use"] == "stored" else ""),
        file=sys.stderr)
    mm = DIGEST_WRITE_LINE.search(r.stderr or "")
    if mm:  # how many of the baseline's inputs an unchanged candidate takes from the digest records
        print("mutate.py: digest records: the unmutated candidate matches %s of %s recorded input(s)" % (mm.group(2), mm.group(1)),
              file=sys.stderr, flush=True)
    if a.baseline_only:
        sys.exit(0)
    # the setup-snapshot cache: wherever the records ended up (this baseline's directory, a baseline-cache
    # entry); only when it holds snapshots (a gate's records from nulltrap_compare, or a --no-snapcache entry, have none)
    if not a.no_refcache and not a.no_snapcache and refc.get("dir") and os.path.isdir(refc["dir"]) and \
            any(f.endswith(".snap") for f in os.listdir(refc["dir"])):
        snapc.update(dir=refc["dir"], key=snapcache_key())
    if not a.receipts and not a.no_receipts:  # default receipts (2026-10-27): on the scratch when there is one
        base = verify.verify_scratch_base()
        if base is None:
            want, _ = mutant_scratch_request(a.scratch)
            base = want if want and os.path.isdir(want) and os.access(want, os.W_OK) else os.path.join(ROOT, "build", "verify")
        a.receipts = os.path.join(base, "_receipts", "%s-%s-%d" % (a.unit.replace("/", "__"), _time.strftime("%Y%m%d-%H%M%S"),
                                                                   os.getpid()))
        print("mutate.py: receipts in %s (--receipts DIR to choose, --no-receipts for none)" % a.receipts, file=sys.stderr)
    if a.no_receipts:
        a.receipts = None
    if base_rows is None:  # (a scoped --rerun baseline took them from the harness's -list before it ran)
        base_rows = {int(l[:8], 16) for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)}
    objs = [os.path.join(out, os.path.basename(s) + ".o") for s in verify.unit_parts(out) + ["runtime/src/espresso_fp.c",
                                                                                              "tools/verify/src/harness.cpp"] + srcs]
    objs += verify.support_objs()  # the harness's shared support objects (mocksurfacesize), never mutated
    names = [os.path.basename(o) for o in objs]
    dup = sorted({n for n in names if names.count(n) > 1})
    if dup:
        sys.exit("mutate.py: sources with the same file name share one object (%s); rename one" % ", ".join(dup))
    inc = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include"]
    allm = []
    extra = []  # helpers above a source's first marked function (preamble_helpers), appended after the old campaign
    for src in srcs:
        text = open(os.path.join(ROOT, src)).read()
        regions = bodies(text)
        for addr, s, e in regions:
            if only is not None and addr not in only:
                continue
            for m in mutants(text, s, e):
                # source location in the description, so a survivor can be traced to its line
                loc = " @%s:%d" % (os.path.basename(src), text.count("\n", 0, m[1]) + 1)
                allm.append((src, addr, (m[0] + loc,) + tuple(m[1:])))
        if a.no_preamble_helpers or autoglue.bodies(text) is None:
            continue
        for hs, he, name, callers in preamble_helpers(text, regions):
            inscope = [c for c in callers if only is None or c in only]
            if not inscope:
                continue
            for m in mutants(text, hs, he):  # attributed to the first in-scope function that uses the helper
                loc = " @%s:%d (helper %s)" % (os.path.basename(src), text.count("\n", 0, m[1]) + 1, name)
                extra.append((src, inscope[0], (m[0] + loc,) + tuple(m[1:])))
    pool = len(allm)
    random.Random(a.seed).shuffle(allm)
    allm = allm[:a.max]
    if extra:  # the old campaign keeps its IDs; helper mutants follow, sampled at the old pool's rate
        random.Random(a.seed).shuffle(extra)
        take = len(extra) if pool <= a.max else -(-len(extra) * a.max // pool)
        allm += extra[:take]
    allm = [(i, src, addr, m, None) for i, (src, addr, m) in enumerate(allm)]
    if a.ids:  # receipt-free replay: the same enumeration gives the same IDs
        want = {int(x) for x in a.ids.split(",") if x.strip()}
        bad = sorted(want - {e[0] for e in allm})
        if bad:
            sys.exit("mutate.py: --ids %s not in this campaign's %d mutants (same --seed/--max/--func/--only?)" % (bad, len(allm)))
        allm = [e for e in allm if e[0] in want]
        print("mutate.py: --ids: running %d of the campaign's mutants" % len(allm))
    if a.rerun:  # the recorded variants, by ID, independent of today's enumeration
        allm = rerun_entries()
        print("mutate.py: rerun of %d recorded mutant(s) of class %s from %s" % (len(allm), a.rerun_class, a.rerun))
        if a.schemata or a.reach_skip:
            # --rerun with --schemata/--fork/--reach-skip: each recorded text becomes one edit of
            # today's source (the span where it differs, mutscope.changed_span), so text[:s] + new + text[e:] is the recorded
            # text exactly; the schema builds, fork servers and reach markers then work as in an enumerated campaign. A
            # recorded text identical to today's source keeps its text (the identical-code check closes it)
            conv, cur = [], {}
            for i, src, addr, m, recorded in allm:
                text = cur.setdefault(src, open(os.path.join(ROOT, src)).read())
                sp = mutscope.changed_span(text, recorded)
                if sp is None:
                    conv.append((i, src, addr, m, recorded))
                    continue
                s0, ea, eb = sp
                conv.append((i, src, addr, (m[0], s0, ea, recorded[s0:eb]), None))
            allm = conv
    import concurrent.futures

    if a.receipts:
        os.makedirs(a.receipts, exist_ok=True)

    def receipt(i, src, addr, desc, mt, cls, **kw):
        """per-mutant record (--receipts): enough to re-run the case and to tell a difference from an abort"""
        if not a.receipts:
            return
        open(os.path.join(a.receipts, "mutant%d.cpp" % i), "w").write(mt)
        rec = {"id": i, "src": src, "func": "%08X" % addr, "mutation": desc, "class": cls, "seed": a.seed,
               "input_seed": a.input_seed, "n": a.n}
        if a.rerun:
            rec["rerun_of"] = os.path.abspath(a.rerun)
        if refc.get("reuse"):
            rec["refcache_from"] = refc["dir"]
        if baseline_excluded:
            rec["baseline_excluded"] = ["%08X" % x for x in baseline_excluded]
        if bc_info:
            rec["baseline_cache"] = bc_info
        if bscope is not None:
            rec["baseline_scope"] = ["%08X" % x for x in sorted(bscope)]
        rec.update(kw)
        json.dump(rec, open(os.path.join(a.receipts, "mutant%d.json" % i), "w"), indent=1)

    scratch = Scratch(a.unit, out, a.scratch, exe)
    atexit.register(scratch.close)  # also on an error exit or Ctrl-C

    def compiled_text(src):
        """the text the per-mutant build compiles for a source text of src (autoglue or #line 1 + text)"""
        def tr(t):
            return autoglue.transform(t, src) if "/* WWHD glue" in t else "#line 1 \"%s\"\n" % src + t
        return tr

    def compile_schema(src):
        def cc0(cpp, obj):
            r = subprocess.run(["clang++", "-std=c++20", "-O1", "-Werror=vla", "-ferror-limit=0"] + verify.FP + inc +
                               ["-I" + os.path.dirname(os.path.join(ROOT, src)), "-include", "tools/verify/src/mutschema.h",
                                "-c", cpp, "-o", obj], cwd=ROOT, capture_output=True, text=True)
            return r.returncode, r.stderr
        return pool(cc0) if pool else cc0

    workdir = None
    pool = JobPool() if a.pool else None

    def run_dir():
        """per-run directory for schema/marker builds and run lists: the scratch run directory, else out/mutfast-PID"""
        nonlocal workdir
        if workdir is None:
            if scratch.dir:
                workdir = scratch.dir
            else:
                workdir = os.path.join(out, "mutfast-%d" % os.getpid())
                os.makedirs(workdir, exist_ok=True)
                atexit.register(shutil.rmtree, workdir, True)
        return workdir

    def baseline_wall():
        """wall seconds of the unmutated unit's own run (the baseline build, this campaign's -n and scope); the first call
        returns the time of the baseline check's run"""
        import time
        if first_wall:
            return first_wall.pop()
        t = time.time()  # no HEAVY slot here: the campaign holds one (a second flock of this process would wait forever)
        # reusing a gate's records, the first measurement read them: measure again the same way
        cmd = harness_cmd(exe, bscope, cache=refc.get("reuse", False))
        k = shard_count(a.j)
        if k > 1:  # as the baseline gate: the summed wall time of its -range parts
            r, w = run_sharded(cmd, k)
            if r is not None:
                return w
            t = time.time()
        subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        return time.time() - t

    limits = TimeoutPolicy(a.timeout, baseline_wall, first_load=first_load, remeasure=a.timeout_remeasure)

    reach = {}  # mutant id -> {func: None (all inputs) | set of input indices}; {} = not reached; absent = run everything
    schema_exe = {}  # mutant id -> the schema executable that holds it (mutschema.build + link)
    marker_exe = {}  # mutant id -> the schema executable that holds only a marker of it (reach recording)
    schema_why = {}  # fallback reason -> count

    def prepare_schemata():
        """one schema executable per source and layer: every schema-able mutant compiled in, none live"""
        by_src = {}
        for i, src, addr, m, recorded in allm:
            if recorded is None:
                by_src.setdefault(src, []).append((i, m[0], m[1], m[2], m[3]))
        d = run_dir()
        for src, entries in by_src.items():
            text = open(os.path.join(ROOT, src)).read()
            tag = "schema-" + os.path.basename(src)
            objects, fallback = mutschema.build(src, text, entries, compiled_text(src), compile_schema(src), d, tag, jobs=a.j)
            for r in fallback.values():
                schema_why[r] = schema_why.get(r, 0) + 1
            for obj, ks in objects:
                exe = obj[:-2]
                r = subprocess.run(["clang++", "-o", exe] + replace_object(objs, src, obj), cwd=ROOT, capture_output=True, text=True)
                rm(obj, obj[:-2] + ".cpp")
                if r.returncode:
                    schema_why["schema link error"] = schema_why.get("schema link error", 0) + len(ks)
                    continue
                for k in ks:
                    if fallback.get(k, "").startswith(mutschema.MARK_PREFIX):
                        marker_exe[k] = exe  # reach recording only; the mutant is built on its own
                    else:
                        schema_exe[k] = exe
        print("mutate.py: schema builds: %d of %d mutant(s) in %d executable(s), %d more as reach markers%s" % (
            len(schema_exe), sum(len(v) for v in by_src.values()), len(set(schema_exe.values()) | set(marker_exe.values())),
            len(marker_exe),
            ("; per-mutant build: " + "; ".join("%s %d" % kv for kv in sorted(schema_why.items(), key=lambda kv: -kv[1])))
            if schema_why else ""), file=sys.stderr)
        if os.environ.get("WWHD_MUT_SCHEMA_DUMP"):  # self-test: the schema executables as groups of mutant IDs
            groups = {}
            for k, e in list(schema_exe.items()) + list(marker_exe.items()):
                groups.setdefault(os.path.basename(e), []).append(k)
            json.dump(sorted(sorted(v) for v in groups.values()), open(os.environ["WWHD_MUT_SCHEMA_DUMP"], "w"))

    def check_schemata():
        """run each schema executable once unmutated (recording the sites each input runs): one that does not pass
        is dropped - its mutants are built one by one and run every input. The executables are independent (own
        binary, own .reach file, no -refcache): they run -j at a time (2026-10-28; WWHD_MUT_SCHEMA_CHECK_J overrides,
        under --pool each holds a slot); results are applied, and messages printed, in sorted executable order, as the
        serial loop did. Each check runs the union of its members' per-mutant scopes (2026-10-30; see check_scope)"""
        recs = {}
        exes = sorted(set(schema_exe.values()) | set(marker_exe.values()))
        members = {}
        for k, e in list(schema_exe.items()) + list(marker_exe.items()):
            members.setdefault(e, []).append(k)
        env = dict(os.environ, WWHD_MUT_REC="1")

        fail = os.environ.get("WWHD_MUT_SCHEMA_CHECK_FAIL") or ""  # self-test: mutant IDs, or "all"
        failing = {int(x) for x in fail.split(",") if x.strip().isdigit()}

        # the check's scope (2026-10-30, AGL#8 request): the union of the members' per-mutant scopes (mutant_scope:
        # mutscope.affected - the mutated function, every function naming a mutated helper directly or through other
        # helpers, every function naming a host static the changed code touches; the whole campaign scope wherever one
        # member's scope cannot be bounded). A site of member K runs only in rows of K's scope, so a row left out records
        # no reach for any member: the same reach rows come out as with every row, and every mutant run (-only its scope,
        # or a run list from these rows) sees the same rows either way. The check writes no cache (no -refcache-write,
        # no -snapcache) and its -refcache key stays the campaign's (records are per function and input, read the same
        # way by the scoped mutant runs). The reach widening in one() (a site seen in a row outside its mutant's scope)
        # can now add rows of the union only; it is a guard against a mutscope gap and fired in none of AGL#8's 4669
        # receipts. WWHD_MUT_SCHEMA_CHECK_WHOLE=1: every row of the campaign scope, as before (self-test, measurements)
        mdesc = {i: (src, addr, m) for i, src, addr, m, recorded in allm if recorded is None}
        whole_check = os.environ.get("WWHD_MUT_SCHEMA_CHECK_WHOLE") == "1"

        def check_scope(exe):
            """(set of functions, None) or (None, reason): the rows exe's unmutated check runs (None: the campaign scope)"""
            if whole_check:
                return None, "WWHD_MUT_SCHEMA_CHECK_WHOLE=1"
            u = set()
            for k in members.get(exe, []):
                if k not in mdesc:
                    return None, "mutant %d not in the campaign" % k
                src, addr, m = mdesc[k]
                if src not in src_texts:
                    src_texts[src] = open(os.path.join(ROOT, src)).read()
                text = src_texts[src]
                fsc, why = mutant_scope(src, addr, text[:m[1]] + m[3] + text[m[2]:])
                if fsc is None:
                    return None, "mutant %d: %s" % (k, why)
                u |= fsc
            if not u or u >= base_rows:
                return None, "every function of the campaign scope"
            return u, None
        csc, cwhy = {}, {}
        for exe in exes:
            csc[exe], why = check_scope(exe)
            if why and not whole_check:
                why = re.sub(r"^mutant \d+: ", "", why)
                cwhy[why] = cwhy.get(why, 0) + 1
        nrows = len(base_rows)
        print("mutate.py: schema check scope: %d of %d executable(s) on their members' functions, %d row run(s) instead of "
              "%d%s" % (sum(1 for v in csc.values() if v is not None), len(exes),
                        sum(nrows if v is None else len(v) for v in csc.values()), nrows * len(exes),
                        " (WWHD_MUT_SCHEMA_CHECK_WHOLE=1)" if whole_check else
                        ("; every row: " + "; ".join("%s %d" % kv for kv in sorted(cwhy.items(), key=lambda kv: -kv[1])))
                        if cwhy else ""), file=sys.stderr, flush=True)

        ran = {}  # exe -> rows the check printed (its harness table): the row runs it made

        def check_one(exe):
            if fail == "all" or failing & set(members.get(exe, [])):  # self-test: counts as not passing
                return False
            try:
                # the original side from the baseline's records (2026-10-30: the check is a full unmutated run per
                # schema executable, so live originals doubled a pass's baseline cost); the reach recording is the candidate
                # side's and the setups still run live. WWHD_MUT_SCHEMA_CHECK_NOCACHE=1 runs the original live (self-test)
                rc_args = (["-refcache", refc["dir"], "-refcache-key", refc["key"]]
                           if refc["dir"] and os.environ.get("WWHD_MUT_SCHEMA_CHECK_NOCACHE") != "1" else [])
                cmd = harness_cmd(exe, csc[exe]) + rc_args
                if ks > 1:  # fasttools2 change 3: -range parts, each with its own .reach file, concatenated (the reach
                    # records are sets of (function, input) per site: the union is the unsharded file's content)
                    nparts = len(verify.shard_ranges(a.n, ks))
                    parts = ["%s.reach.%d" % (exe, i) for i in range(nparts)]
                    r, w = run_sharded(cmd, ks, env=env, timeout=max(limits.current(), 600), wrap=pool,
                                       part_args=lambda i, lo, hi: ["-mutrec", parts[i]])
                    try:
                        if r is None:
                            print("mutate.py: schema check of %s: %s" % (os.path.basename(exe), "; ".join(w)), file=sys.stderr)
                            return False
                        with open(exe + ".reach", "w") as fo:
                            for pf in parts:
                                with open(pf) as fi:
                                    fo.write(fi.read())
                    finally:
                        rm(*parts)
                else:
                    r = subprocess.run(cmd + ["-mutrec", exe + ".reach"], cwd=ROOT, capture_output=True, text=True,
                                       timeout=max(limits.current(), 600), env=env)
                ran[exe] = sum(1 for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l))
                return r.returncode == 0
            except (subprocess.TimeoutExpired, OSError):
                return False
        jall = int(os.environ.get("WWHD_MUT_SCHEMA_CHECK_J") or a.j)  # env: the self-test's -j1
        ks = shard_count(max(1, jall // max(1, len(exes))))  # fasttools2 change 3: idle job slots become -range parts
        job = pool(check_one) if pool and ks == 1 else check_one  # (sharded: each part holds its own slot)
        jobs = max(1, min(jall // ks, len(exes)))
        if jobs > 1 or ks > 1:
            print("mutate.py: checking %d schema build(s) unmutated, %d at a time%s" % (
                len(exes), jobs, ", each as %d -range parts" % ks if ks > 1 else ""), file=sys.stderr, flush=True)
        t_check = _time.time()
        with verify.heavy_lock(a.unit), concurrent.futures.ThreadPoolExecutor(jobs) as ex:
            oks = list(ex.map(job, exes))  # in exes order
        print("mutate.py: schema check took %.1f s, %d row run(s)" % (_time.time() - t_check, sum(ran.values())),
              file=sys.stderr, flush=True)
        for exe, ok in zip(exes, oks):
            ks = members.get(exe, [])
            recf = exe + ".reach"
            if not ok:
                print("mutate.py: schema build %s does not pass unmutated; its %d mutants are built one by one"
                      % (os.path.basename(exe), len(ks)), file=sys.stderr)
                for k in ks:
                    schema_exe.pop(k, None)
                    marker_exe.pop(k, None)
                rm(recf)
                continue
            recs[exe] = recf
        if exes and not recs:  # the baseline passed, yet no schema build does: the schema machinery is broken here
            sys.exit("mutate.py: none of the %d schema build(s) passes unmutated although the baseline does; stopping before "
                     "any mutant (rerun without --schemata/--fork/--reach-skip and report the unit)" % len(exes))
        return recs

    def prepare_reach(recs):
        """which inputs run each site (the recordings of check_schemata)"""
        nmark = 0
        for exe, recf in sorted(recs.items()):
            ks = [k for k, e in list(schema_exe.items()) + list(marker_exe.items()) if e == exe]
            every = set()
            rows = {k: {} for k in ks}
            for line in open(recf):
                f = line.split()
                hits = [int(x) for x in f[-1].split(",")]
                if f[0] in ("G", "E"):  # start-up (static initialisers) or after the last input: every input
                    every.update(hits)
                elif f[0] == "S":  # a function's setup: every input of that function
                    fn = int(f[1], 16)
                    for k in hits:
                        if k in rows:
                            rows[k][fn] = None
                elif f[0] == "I":
                    fn, idx = int(f[1], 16), int(f[2])
                    for k in hits:
                        if k in rows and rows[k].get(fn, 0) is not None:
                            rows[k].setdefault(fn, set()).add(idx)
            for k in ks:
                if k not in every:
                    reach[k] = rows[k]
                    nmark += 1
        if os.environ.get("WWHD_MUT_REACH_DUMP"):  # self-test: the reach records, to compare runs
            json.dump({str(k): {"%08X" % f: (None if v is None else sorted(v)) for f, v in sorted(reach[k].items())}
                       for k in sorted(reach)}, open(os.environ["WWHD_MUT_REACH_DUMP"], "w"), indent=0, sort_keys=True)
        nr = sum(1 for v in reach.values() if not v)
        print("mutate.py: --reach-skip: %d mutant(s) with reach data (%d reached by no input), %d run every input"
              % (nmark, nr, len(allm) - nmark), file=sys.stderr)

    fork_cmds = {}  # schema executable -> fork server command (the same for all its mutants)
    servers = ServerPool(lambda exe: ForkServer(fork_cmds[exe], run_dir()),
                         per_thread=int(os.environ.get("WWHD_MUT_FORK_SERVERS_PER_THREAD") or 4))

    def fork_run(i, exe, cmd, runlist):
        """run mutant i of schema executable exe on this thread's fork server, with the timeout policy and retry;
        returns (CompletedProcess-like or None on timeout, tried) or None when it cannot use a server"""
        fork_cmds.setdefault(exe, cmd)
        try:
            srv = servers.get(exe)
        except OSError as e:
            print("mutate.py: --fork: %s; plain runs for %s" % (e, os.path.basename(exe)), file=sys.stderr)
            return None
        if srv is None or i in srv.static:
            return None
        tried = [limits.current()]
        while True:
            try:
                r = srv.run(i, runlist, tried[-1])
            except OSError as e:
                print("mutate.py: --fork: %s; plain runs for %s" % (e, os.path.basename(exe)), file=sys.stderr)
                servers.failed(exe)
                return None
            if r is not None:
                return subprocess.CompletedProcess(cmd, r[0], r[1], r[2]), tried
            if not limits.retry or len(tried) > 1:
                return None, tried
            tried.append(2 * tried[-1])

    base_o1 = {}
    base_lock = threading.Lock()

    def base_object(src):
        """the unmutated source compiled exactly like a mutant (-O1, once per source, in the run directory, under the
        source's own file name like every mutant: the file name reaches the object as the static-initialiser symbol
        __GLOBAL__sub_I_<name>, so the identical-code check needs the same name on both sides). (object or None, source)"""
        return base_build(src)[:2]

    def base_build(src):
        """(object or None, source, sha256 of the object or None)"""
        with base_lock:
            if src not in base_o1:
                text = open(os.path.join(ROOT, src)).read()
                bdir = os.path.join(run_dir(), "base.tu")
                os.makedirs(bdir, exist_ok=True)
                bpath = os.path.join(bdir, os.path.basename(src))
                with open(bpath, "w") as fh:
                    fh.write(compiled_text(src)(text))
                r = subprocess.run(mutant_flags(src) + ["-c", bpath, "-o", bpath + ".o"], cwd=ROOT, capture_output=True, text=True)
                base_o1[src] = (bpath + ".o", bpath, mutcode.object_digest(bpath + ".o")) if r.returncode == 0 else (None, bpath, None)
            return base_o1[src]

    def mutant_flags(src):
        return ["clang++", "-std=c++20", "-O1", "-w"] + verify.FP + inc + ["-I" + os.path.dirname(os.path.join(ROOT, src))]

    descs = {e[0]: e[3][0] for e in allm}

    def desc_kind(i):
        return descs.get(i, "")

    def mutant_files(i, src, d):
        """(source, object, executable) of mutant i in directory d: the source keeps the unit source's file name (in its
        own subdirectory mutantN.tu), as the identical-code check needs (base_build)"""
        tu = os.path.join(d, "mutant%d.tu" % i)
        mpath = os.path.join(tu, os.path.basename(src))
        return mpath, mpath + ".o", os.path.join(d, "mutant%d" % i)

    def rm_mutant(i, src, d):
        mpath, mobj, mexe = mutant_files(i, src, d)
        rm(mpath, mobj, mexe, os.path.join(d, "mutant%d.cpp" % i), os.path.join(d, "mutant%d.o" % i))
        try:
            os.rmdir(os.path.dirname(mpath))
        except OSError:
            pass

    def keep_broken(i, d, mpath, mobj):
        """a broken mutant's files stay under their old names (mutantN.cpp, mutantN.o) in d, as before the mutantN.tu layout"""
        for f, to in ((mpath, "mutant%d.cpp" % i), (mobj, "mutant%d.o" % i)):
            try:
                os.replace(f, os.path.join(d, to))
            except OSError:
                pass
        try:
            os.rmdir(os.path.dirname(mpath))
        except OSError:
            pass

    def build_mutant(i, src, mt, d):
        """write and compile mutant i in directory d: ("obj", mpath, mobj, sha256 of the object) or (stage, rc, stderr,
        nospace); link_mutant links it"""
        mpath, mobj, mexe = mutant_files(i, src, d)
        try:
            os.makedirs(os.path.dirname(mpath), exist_ok=True)
        except OSError as oe:
            return ("write", None, str(oe), True)
        if "/* WWHD glue" in mt:
            try:
                body = autoglue.transform(mt, src)
            except autoglue.GlueError as ge:
                return ("glue", None, str(ge), False)
        else:
            body = "#line 1 \"%s\"\n" % src + mt
        try:
            with open(mpath, "w") as fh:
                fh.write(body)
        except OSError as oe:
            rm(mpath)
            return ("write", None, str(oe), True)
        r = subprocess.run(mutant_flags(src) + ["-c", mpath, "-o", mobj], cwd=ROOT, capture_output=True, text=True)
        if r.returncode:  # the files of a broken mutant stay, as before (a scratch run directory goes at the end)
            keep_broken(i, d, mpath, mobj)
            return ("compile", r.returncode, r.stderr, nospace(r.stderr))
        if not a.allow_host_symbols:
            base, base_cpp = base_object(src)
            host = new_host_symbols(base, mobj, mt) if base else set()
            if not host and base and desc_kind(i).startswith(("drop statement", "swap")):  # a moved/dropped declaration
                host = host_function_values(base_cpp, mpath, mutant_flags(src)[1:], mt)
            if host:  # compiles only against the host's libc: as broken as a compile error on the target
                keep_broken(i, d, mpath, mobj)
                return ("hostsym", 1, "mutant references host symbol(s) the original does not: %s" % ", ".join(sorted(host)), False)
        return ("obj", mpath, mobj, mutcode.object_digest(mobj))

    def link_mutant(i, src, mobj, d):
        """("ok", mpath, mobj, mexe) or ("link", rc, stderr, nospace)"""
        mpath, _, mexe = mutant_files(i, src, d)
        lo = replace_object(objs, src, mobj)
        r = subprocess.run(["clang++", "-o", mexe] + lo, cwd=ROOT, capture_output=True, text=True)
        if r.returncode:
            return ("link", r.returncode, r.stderr, nospace(r.stderr))
        return ("ok", mpath, mobj, mexe)

    tce = not a.no_tce
    tce_groups = mutcode.Groups()
    tce_stats = {"identical": 0, "after_run": 0, "dup": 0, "checks": 0}
    tce_lock = threading.Lock()
    rc_stats = {"hit": 0, "live": 0, "dmatch": 0, "dlite": 0, "dlive": 0}
    sc_stats = {"fhit": 0, "flive": 0, "vhit": 0, "vlive": 0}

    def build_and_link(i, src, mt, d):
        """build_mutant, the identical-code check, then link_mutant: ("ok", mpath, mobj, mexe, sha256 of the object),
        ("same", mpath, mobj, digest) or a failure tuple as build_mutant/link_mutant return it"""
        b = build_mutant(i, src, mt, d)
        if b[0] != "obj":
            return b
        if tce:
            base = base_build(src)
            with tce_lock:
                tce_stats["checks"] += 1
            if base[2] is not None and b[3] == base[2]:
                return ("same", b[1], b[2], b[3])
        l = link_mutant(i, src, b[2], d)
        return l + (b[3],) if l[0] == "ok" else l

    def tce_same(i, src, mt):
        """compile mutant i's own variant like a per-mutant build (schema and unreached mutants have none) and compare
        its whole object with the unmutated source's; True when identical. Off with --no-tce."""
        if not tce:
            return False
        base = base_build(src)
        if base[2] is None:
            return False
        d = scratch.pick()
        mpath, mobj, _ = mutant_files(i, src, d)
        try:
            body = autoglue.transform(mt, src) if "/* WWHD glue" in mt else "#line 1 \"%s\"\n" % src + mt
            os.makedirs(os.path.dirname(mpath), exist_ok=True)
            with open(mpath, "w") as fh:
                fh.write(body)
            r = subprocess.run(mutant_flags(src) + ["-c", mpath, "-o", mobj], cwd=ROOT, capture_output=True, text=True)
            same = r.returncode == 0 and mutcode.object_digest(mobj) == base[2]
        except (OSError, autoglue.GlueError):
            same = False
        rm_mutant(i, src, d)
        with tce_lock:
            tce_stats["checks"] += 1
        return same

    def tce_closed(i, src, addr, desc, mt, **kw):
        """receipt and result of a mutant closed by the identical-code check"""
        with tce_lock:
            tce_stats["identical"] += 1
        receipt(i, src, addr, desc, mt, mutcode.EQ_CLASS, reason=mutcode.EQ_REASON, object_sha256=base_build(src)[2], **kw)
        return mutcode.EQ_CLASS, addr, desc

    def one(i, src, addr, m, recorded, force_plain=False):
        desc, s, e, new = m
        if recorded is not None:
            mt = recorded
        else:
            text = open(os.path.join(ROOT, src)).read()
            mt = text[:s] + new + text[e:]
        unreached = a.reach_skip and i in reach and not reach[i]  # no input runs the site: nothing can differ
        if unreached and i not in marker_exe:  # compiled in a schema, so it compiles: no build, no run
            if tce_same(i, src, mt):
                return tce_closed(i, src, addr, desc, mt, reach="not reached")
            receipt(i, src, addr, desc, mt, "survived", reach="not reached")
            return "survived-unreached", addr, desc
        env = None
        digest = None
        if a.schemata and i in schema_exe and not force_plain:  # compiled in a schema executable: no build, the harness makes it live
            if a.ids and tce_same(i, src, mt):  # a survivor replay: the check costs less than the run it saves
                return tce_closed(i, src, addr, desc, mt)
            b = ("ok", None, None, schema_exe[i])
            env = dict(os.environ, WWHD_MUT_ACTIVE=str(i))
        else:
            d = scratch.pick()
            b = build_and_link(i, src, mt, d)
            if env is None and b[0] not in ("ok", "same") and b[3] and d != out:  # the scratch filled up mid-mutant: this mutant again in the build directory
                scratch.fell_back(d)
                rm_mutant(i, src, d)
                d = out
                b = build_and_link(i, src, mt, out)
            if b[0] == "same":  # the whole object equals the unmutated source's: never linked or run
                rm_mutant(i, src, d)
                return tce_closed(i, src, addr, desc, mt, **({"reach": "not reached"} if unreached else {}))
            if b[0] == "ok":
                digest = b[4]
                b = b[:4]
        if b[0] == "glue":
            receipt(i, src, addr, desc, mt, "broken", stage="glue", error=b[2])
            return "broken", addr, desc
        if b[0] == "write":
            raise OSError("mutate.py: cannot write mutant %d: %s" % (i, b[2]))
        if b[0] != "ok":
            receipt(i, src, addr, desc, mt, "broken", stage=b[0], rc=b[1], stderr=b[2][-4000:])
            return "broken", addr, desc
        mpath, mobj, mexe = b[1:]
        if unreached:  # a marker-only mutant (declaration): it had to compile on its own to tell broken from survived
            receipt(i, src, addr, desc, mt, "survived", reach="not reached")
            rm_mutant(i, src, d)
            return "survived-unreached", addr, desc
        fsc, why = mutant_scope(src, addr, mt)
        sc_extra = {}
        if fsc is not None and a.reach_skip and reach.get(i):
            wid = set(reach[i]) - fsc  # rows the marker build saw run the site: never outside the scope
            if wid:
                print("mutate.py: mutant %d: reach data adds %s to its scope" % (i, " ".join("%08X" % x for x in sorted(wid))),
                      file=sys.stderr)
                fsc |= wid
                sc_extra["scope_widened_by_reach"] = ["%08X" % x for x in sorted(wid)]
        with scope_lock:
            if fsc is None:
                scope_stats["whole"] += 1
            else:
                scope_stats["scoped"] += 1
                scope_stats["funcs"] += len(fsc)
            scope_stats["widened"] += bool(sc_extra)
        sc_extra["scope"] = "whole" if fsc is None else ["%08X" % x for x in sorted(fsc)]
        if fsc is None:
            sc_extra["scope_reason"] = why
        forked = a.fork and env is not None  # a fork server's command is shared by its mutants: the scope goes in the run list
        cmd = harness_cmd(mexe, None if forked else fsc, cache=True)
        runlist = None
        if not a.no_stopfirst:  # the first output difference ends the run; an abort ends its row only (verdicts unchanged)
            cmd += ["-stopfirst"]
        rl = None
        if a.reach_skip and i in reach:
            rl = sorted(reach[i].items())
        elif forked and fsc is not None:
            rl = [(fn, None) for fn in sorted(fsc)]
        if rl is not None:
            runlist = os.path.join(run_dir(), "runlist%d.txt" % i)
            with open(runlist, "w") as fh:
                for fn, idx in rl:
                    fh.write("%08X %s\n" % (fn, "all" if idx is None else ",".join(map(str, sorted(idx)))))
            cmd += ["-runlist", runlist]
        extra = {"schema": os.path.basename(mexe)} if env is not None else {}
        if force_plain:
            extra["schema_survivor_rerun"] = "plain per-mutant build"
        extra.update(sc_extra)
        gkey = None
        if tce and digest is not None:  # same object and same run configuration as another mutant: one run serves both
            gkey = (src, digest, None if fsc is None else tuple(sorted(fsc)), None if rl is None else repr(rl))
            first, got = tce_groups.claim(gkey, i)
            if not first:
                rep_id, (cls, kw) = got
                kw = dict(kw, tce_same_code_as=rep_id, object_sha256=digest)
                receipt(i, src, addr, desc, mt, cls, **kw)
                rm_mutant(i, src, d)
                if runlist:
                    rm(runlist)
                return cls, addr, desc
        try:
            res = run_one(i, src, addr, desc, mt, env, mexe, cmd, runlist, extra, gkey, digest)
        except BaseException:
            if gkey is not None:
                tce_groups.failed(gkey)
            raise
        finally:
            if env is None:
                rm_mutant(i, src, d)
            if runlist:
                rm(runlist)
        if env is not None and res[0] == "survived" and not a.no_schema_confirm:
            # Since 2026-10-31 (AGL#8 2086/2825): a schema build can fold a read of an uninitialised local differently
            # from the mutant's own build, so a schema survivor is confirmed by a plain per-mutant build and run; that
            # run's receipt replaces the schema run's (kills count, a survivor stays survived)
            return one(i, src, addr, m, recorded, force_plain=True)
        return res

    def run_one(i, src, addr, desc, mt, env, mexe, cmd, runlist, extra, gkey, digest):
        """run one built mutant and write its receipt (the second half of one())"""
        fr = None
        t_run = _time.time()
        if a.fork and env is not None:
            fr = fork_run(i, mexe, [c for c in cmd if c not in ("-runlist", runlist)], runlist)
            if fr is not None:
                extra["fork"] = True
        if fr is not None:
            r, tried = fr
        else:
            r, tried = run_with_retry(cmd, limits.current(), limits.retry, cwd=ROOT, capture_output=True, text=True, env=env)
        extra["wall"] = round(_time.time() - t_run, 2)  # seconds of this mutant's harness run(s), retries included
        if r is not None:
            mm = re.search(r"^refcache: (\d+) input\(s\) took the recorded original, (\d+) ran it live$", r.stderr, re.M)
            if mm:
                with tce_lock:
                    rc_stats["hit"] += int(mm.group(1))
                    rc_stats["live"] += int(mm.group(2))
            mm = re.search(r"^refcache: digest records: (\d+) input\(s\) matched the candidate's digest, (\d+) needed no "
                           r"comparison \(aborts\), (\d+) ran the original live after a digest difference$", r.stderr, re.M)
            if mm:
                with tce_lock:
                    for k, g in (("dmatch", 1), ("dlite", 2), ("dlive", 3)):
                        rc_stats[k] += int(mm.group(g))
            mm = re.search(r"^snapcache: (\d+) function\(s\) took the cached snapshot, (\d+) captured it live; (\d+) validation "
                           r"input\(s\) took the recorded setups, (\d+) ran them live$", r.stderr, re.M)
            if mm:
                with tce_lock:
                    for k, g in (("fhit", 1), ("flive", 2), ("vhit", 3), ("vlive", 4)):
                        sc_stats[k] += int(mm.group(g))
            cls = "survived" if r.returncode == 0 else classify(r.returncode, r.stdout + r.stderr)
            kw = dict(cmd=cmd, rc=r.returncode, stdout=r.stdout[-8000:], stderr=r.stderr[-4000:], timeouts_tried=tried, **extra)
        else:
            cls = "timeout"  # undetermined: a slow or looping function anywhere in the unit, not a kill
            kw = dict(cmd=cmd, timeout=tried[-1], timeouts_tried=tried, **extra)
        if env is not None and cls in ("survived", "timeout") and not a.ids and tce_same(i, src, mt):
            # a schema mutant the run did not kill: its own variant compiles to the unmutated object (checked after the run,
            # where a kill needs no compile); the run's fields stay in the receipt
            with tce_lock:
                tce_stats["identical"] += 1
                tce_stats["after_run"] += 1
            kw.update(run_class=cls, reason=mutcode.EQ_REASON, object_sha256=base_build(src)[2])
            cls = mutcode.EQ_CLASS
        if gkey is not None:
            kw["object_sha256"] = digest
        receipt(i, src, addr, desc, mt, cls, **kw)
        if gkey is not None:
            tce_groups.done(gkey, (cls, dict(kw)))
        return cls, addr, desc

    if a.reach_skip or a.schemata:  # also with --rerun since 2026-10-30: the recorded texts became edits above
        prepare_schemata()
        recs = check_schemata()
        if a.reach_skip:
            prepare_reach(recs)
        for recf in recs.values():
            rm(recf)
    atexit.register(servers.close_all)
    if a.fork:  # each schema executable's servers close after its last mutant (ServerPool.done)
        cnt = {}
        for i, src, addr, m, rc in allm:
            if i in schema_exe:
                cnt[schema_exe[i]] = cnt.get(schema_exe[i], 0) + 1
        servers.set_remaining(cnt)

    def one_done(i, *rest):
        try:
            return one(i, *rest)
        finally:
            if a.fork and i in schema_exe:
                servers.done(schema_exe[i])
    killed = survived = broken = timeouts = unreached = equivalent = 0
    eqs = []
    kinds = {"difference": 0, "abort": 0, "signal": 0, "invalid": 0}
    surv = []
    tmo = []
    other = []
    pw, ptk = None, None
    pc = {"done": 0, "killed": 0, "survived": 0, "broken": 0, "timeouts": 0, "equivalent": 0}
    plock = threading.Lock()
    if prog.on:
        import time as _time
        t_mut = _time.time()
        pdir = os.path.join(out, "progress") if verify.verify_scratch_base() is not None or not scratch.dir else scratch.dir
        on_scratch = verify.verify_scratch_base() is not None or bool(scratch.dir)
        pw = progress.Writer("mutate", pdir, progress.file_interval(prog, on_scratch))
        atexit.register(pw.close)

        def mstate():
            n, d = len(allm), pc["done"]
            el = _time.time() - t_mut
            eta = el / d * (n - d) if d else None
            return {"unit": a.unit, "phase": "mutants", "done": d, "total": n, "killed": pc["killed"], "survived": pc["survived"],
                    "broken": pc["broken"], "timeouts": pc["timeouts"], "equivalent": pc["equivalent"],
                    "rate": round(d / el, 3) if el > 0 else 0,
                    "eta_s": round(eta) if eta is not None else None}

        def mtick():
            st = mstate()
            pw.write(st)
            prog.say("mutate %s: mutants %d/%d done, kills %d, survived %d, identical-code EQ %d, broken %d, timeouts %d, "
                     "%.2f mutants/s, ETA %s" % (a.unit, st["done"], st["total"], st["killed"], st["survived"], st["equivalent"],
                                                 st["broken"], st["timeouts"], st["rate"], progress.fmt_dur(st["eta_s"])))
        pw.write(mstate(), force=True)
        ptk = progress.Ticker(prog.interval, mtick)

    def counted(fut):
        try:
            st = fut.result()[0]
        except BaseException:
            return
        key = {"broken": "broken", "timeout": "timeouts", "survived": "survived", "survived-unreached": "survived",
               mutcode.EQ_CLASS: "equivalent"}.get(st, "killed")
        with plock:
            pc["done"] += 1
            pc[key] += 1
    with verify.heavy_lock(a.unit), concurrent.futures.ThreadPoolExecutor(max(1, a.j)) as ex:
        job = pool(one_done) if pool else one_done
        order = allm
        if a.fork:  # run each schema executable's mutants together, so its servers are reused and close early (ServerPool)
            order = [allm[k] for k in grouped_order([schema_exe.get(e[0]) for e in allm])]
        byid = {e[0]: ex.submit(job, *e) for e in order}
        futs = [(e[0], byid[e[0]]) for e in allm]  # results, receipts and the printed lists stay in mutant order
        for _, f in futs:
            f.add_done_callback(counted)
        for mid, f in futs:
            st, addr, desc = f.result()
            desc = "#%d %s" % (mid, desc)
            if st == "broken":
                broken += 1
            elif st == "timeout":
                timeouts += 1
                tmo.append("%08X %s" % (addr, desc))
            elif st == "survived":
                survived += 1
                surv.append("%08X %s" % (addr, desc))
            elif st == mutcode.EQ_CLASS:
                equivalent += 1
                eqs.append("%08X %s" % (addr, desc))
            elif st == "survived-unreached":
                survived += 1
                unreached += 1
                surv.append("%08X %s (not reached)" % (addr, desc))
            else:
                killed += 1
                kinds[st] += 1
                if st != "difference":
                    other.append("%s %08X %s" % (st, addr, desc))
    if ptk:
        ptk.stop()
        pw.close()
    total = killed + survived
    print("unit %s: %d mutants compiled (%d did not compile), killed %d, survived %d (kill rate %.1f%%)%s" % (
        a.unit, total + equivalent, broken, killed, survived, 100.0 * killed / max(1, total),
        ", equivalent-identical-code %d (compiles to identical machine code; not run, not in the kill rate)" % equivalent
        if equivalent else ""))
    print("  killed by: difference %d, abort %d, signal %d, invalid %d (only 'difference' is an observed output "
          "difference; review the others)" % (kinds["difference"], kinds["abort"], kinds["signal"], kinds["invalid"]))
    for s in other:
        print("  killed-" + s)
    if timeouts:
        print("  %d mutant run(s) hit the timeout (%s) and are NOT counted (steer the slow function or raise --timeout):" % (
            timeouts, "%d s" % a.timeout if a.timeout else "last limit %.0f s, retried at 2x" % (2 * limits.value)))
        for s in tmo:
            print("  timeout: " + s)
    if unreached:
        print("  %d survivor(s) not reached by any input (--reach-skip: not built or run)" % unreached)
    if scope_stats["scoped"] or scope_stats["whole"]:
        print("  per-mutant scope: %d mutant run(s) on their affected functions (mean %.1f of %d), %d on the whole campaign "
              "scope%s" % (scope_stats["scoped"], scope_stats["funcs"] / max(1, scope_stats["scoped"]), len(base_rows),
                           scope_stats["whole"], ", %d widened by reach data" % scope_stats["widened"] if scope_stats["widened"] else ""))
    if tce:
        print("  identical-code check: %d object compare(s), %d mutant(s) closed as equivalent-identical-code (%d after a run "
              "that did not kill them), %d mutant(s) took the outcome of an identical object's run" % (
                  tce_stats["checks"], tce_stats["identical"], tce_stats["after_run"], tce_groups.dups))
    if refc["dir"]:
        print("  reference-side cache: %d generated input(s) took the baseline's recorded original, %d ran it live (%.1f%% hits)%s" % (
            rc_stats["hit"], rc_stats["live"], 100.0 * rc_stats["hit"] / max(1, rc_stats["hit"] + rc_stats["live"]),
            "; digest records: %d matched the candidate's digest, %d aborts needed no comparison, %d ran the original live "
            "after a digest difference" % (rc_stats["dmatch"], rc_stats["dlite"], rc_stats["dlive"])
            if rc_stats["dmatch"] + rc_stats["dlite"] + rc_stats["dlive"] else ""))
    if snapc["dir"] and sum(sc_stats.values()):
        print("  setup-snapshot cache: %d function snapshot(s) loaded, %d captured live; %d validation input(s) took the recorded "
              "setups, %d ran them live" % (sc_stats["fhit"], sc_stats["flive"], sc_stats["vhit"], sc_stats["vlive"]))
    for s in eqs:
        print("  equivalent-identical-code: " + s)
    for s in surv:
        print("  survivor: " + s)


if __name__ == "__main__":
    main()
