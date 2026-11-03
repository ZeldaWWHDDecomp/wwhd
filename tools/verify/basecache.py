#!/usr/bin/env python3
"""Baseline cache for mutate.py campaigns (2026-10-30): a survivor cascade runs one `mutate.py SPEC --rerun DIR`
per companion spec, and each of them first runs the unmutated baseline gate in the campaign's exact configuration
(the 2026-10-27 gate) and writes the reference-side records (-refcache-write) its mutant runs read. A cascade run again
(a resumed or repeated cascade, a second class set, the per-stage passes of a shared cascade) re-ran the same baselines.

Opt-in: `mutate.py --baseline-cache DIR` (or WWHD_BASELINE_CACHE=DIR). A baseline that PASSES is recorded once in
DIR/<key>/: receipt.json (verdict, exit code, the harness stdout the campaign reads its function rows from, the wall time
and the 1-minute load right after it, the reference-side cache key, the sha256 of every record file) and refcache/ (the
records the mutant runs read). A later campaign with the same key takes the verdict, the rows and the records from there
instead of running the baseline. A failing baseline is never recorded (only passes are stored, and a lookup re-checks the
stored run with mutate.check_baseline), so a campaign on a failing baseline stays void.

The key (sha256) is exact; any change re-runs:
  - the full baseline harness command (executable path, spec path, -n, -seed, header flags, -nulltrap,
    -observe-pointees, -only scope, -rec; the record directory itself as a placeholder) and the working directory;
  - the bytes of the verify executable (every object linked into it: unit.c, the candidate sources, espresso_fp, the
    harness, the support objects; the link is deterministic) and of the spec file the harness reads, plus the expanded
    spec text;
  - the files the harness reads at run time: build/verify/{image.bin,livein.tsv,frames.tsv,statics.tsv}, build/*.tsv,
    ctor_sizes.tsv/stage_names.txt beside the spec, every `blob` game file, the recordings directory (names + bytes);
  - the tool version (every file under tools/verify except units/ and recordings/, so mutate.py and this module too),
    the gen fingerprint, the compiler version, the OS release and machine;
  - every WWHD_* environment variable except the ones that only steer progress, scratch, slots and caches;
  - the reference-side cache key mutate.py passes to -refcache-key (also checked against the entry).
Entries are written once (staging directory + rename) and never changed; a per-key flock makes a second campaign with
the same key wait for the first one's baseline and then reuse it. Nothing here deletes an entry: the cache lives as long
as its directory (point it into the cascade's OUT directory).

Overlap (prefetch): `basecache.py prefetch --cache DIR [--parallel 2] [--load-max 85] SPEC... -- MUTATE_ARGS` runs
`mutate.py SPEC --baseline-only --baseline-cache DIR MUTATE_ARGS` for all companions up front, at most --parallel at a
time and starting one only while the 1-minute load is below --load-max; the cascade's passes then hit the cache. The
python API is prefetch(). Self-test: basecache_test.py.
"""
import argparse
import contextlib
import fcntl
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import vcache  # noqa: E402  (content-hash helpers shared with the verify.py result cache)

VERSION = "basecache-1"
# WWHD_* variables that do not change a baseline's result (progress, scratch, slots, caches)
ENV_NEUTRAL = re.compile(r"^WWHD_(VERIFY_SCRATCH.*|VERIFY_PROGRESS.*|VERIFY_STUCK|PROGRESS_REGISTRY|SHARED_READY|"
                         r"VERIFY_CACHE.*|VERIFY_ARCHIVE|SHARDS|HEAVY_.*|MUT_SCRATCH.*|MUT_POOL_SLOTS|MEMSTAT|BASELINE_CACHE)$")
RECORD_PLACEHOLDER = "<refcache-dir>"


def env_key(environ=None):
    environ = os.environ if environ is None else environ
    return sorted("%s=%s" % (k, v) for k, v in environ.items() if k.startswith("WWHD_") and not ENV_NEUTRAL.match(k))


def _verify_dir(spec):
    """harness.cpp stage_names_dir(): the directory of ctor_sizes.tsv / stage_names.txt for a spec path - beside a unit
    spec under units/, else (an expanded `include` spec in the build or verify-scratch directory) this tools/verify
    (2026-11-02, as the harness since f3a3d3e05)"""
    u = spec.rfind("/units/")
    if u >= 0:
        return spec[:u]
    if spec.startswith("units/"):
        return "."
    return os.path.dirname(os.path.abspath(__file__))


def _image(cmd):
    for i, x in enumerate(cmd):
        if x == "-image" and i + 1 < len(cmd):
            return cmd[i + 1]
    return "build/verify/image.bin"


def _rec(cmd):
    for i, x in enumerate(cmd):
        if x == "-rec" and i + 1 < len(cmd):
            return cmd[i + 1]
    return None


def _abs(p, root):
    return p if os.path.isabs(p) else os.path.join(root, p)


def baseline_key(cmd, refcache_key, spec_lines, root=ROOT, environ=None, cdir=None):
    """the exact key of a baseline run: cmd is the harness command as mutate.py runs it (cmd[0] the executable, the
    record directory of -refcache-write replaced by RECORD_PLACEHOLDER), refcache_key the -refcache-key value or None,
    spec_lines the expanded spec lines. Returns (key, parts) - parts for the receipt (what the key covers)."""
    exe = _abs(cmd[0], root)
    spec = _abs(cmd[cmd.index("-spec") + 1], root) if "-spec" in cmd else None
    u = os.uname()
    parts = [VERSION, "root:" + root, "cmd:" + "\0".join(cmd), "refcache-key:%s" % refcache_key,
             "exe:" + vcache.file_sha(exe), "spec-file:" + (vcache.file_sha(spec) if spec else "-"),
             "spec:" + "".join(spec_lines), "tools:" + vcache.tree_sha(HERE, skip=("units", "recordings")),
             "gen:" + vcache.gen_fingerprint(root), "compiler:" + vcache.compiler_id(),
             "os:%s %s %s" % (u.sysname, u.release, u.machine), "env:" + " ".join(env_key(environ))]
    shared = [_abs(_image(cmd), root)] + [os.path.join(root, "build", "verify", f) for f in
                                          ("livein.tsv", "frames.tsv", "statics.tsv")]
    bdir = os.path.join(root, "build")
    shared += sorted(os.path.join(bdir, f) for f in os.listdir(bdir) if f.endswith(".tsv")) if os.path.isdir(bdir) else []
    if spec:
        vd = _abs(_verify_dir(spec), root)
        shared += [os.path.join(vd, "ctor_sizes.tsv"), os.path.join(vd, "stage_names.txt")]
    for p in shared:
        parts.append("file:%s:%s" % (p, vcache.big_file_sha(p, cdir) if os.path.exists(p) else "-"))
    for line in spec_lines:  # blob FUNC|* DST FILE OFFSET SIZE: the game file's bytes
        f = line.split("#")[0].split()
        if len(f) >= 4 and f[0] == "blob":
            p = os.path.join(root, "game", f[3])
            parts.append("blob:%s:%s" % (f[3], vcache.big_file_sha(p, cdir) if os.path.exists(p) else "-"))
    rec = _rec(cmd)
    if rec:
        rd = _abs(rec, root)
        files = []
        if os.path.isdir(rd):
            for dp, dns, fns in os.walk(rd):
                dns.sort()
                for fn in sorted(fns):
                    files.append("%s:%s" % (os.path.relpath(os.path.join(dp, fn), rd), vcache.file_sha(os.path.join(dp, fn))))
        parts.append("rec:%s:%s" % (rec, vcache.sha(*files) if files else "-"))
    return vcache.sha(*parts), parts


def records_digest(d):
    """{file name: sha256} of a record directory"""
    return {fn: vcache.file_sha(os.path.join(d, fn)) for fn in sorted(os.listdir(d))} if d and os.path.isdir(d) else {}


class Cache:
    """one cache directory: DIR/<key>/{receipt.json,refcache/}, DIR/<key>.lock, DIR/.staging-*"""

    def __init__(self, root):
        self.root = os.path.abspath(root)
        os.makedirs(self.root, exist_ok=True)

    def entry(self, key):
        return os.path.join(self.root, key)

    @contextlib.contextmanager
    def lock(self, key):
        """held while one campaign looks up, runs and stores the baseline of this key: a second one waits, then hits"""
        f = open(os.path.join(self.root, key + ".lock"), "a")
        try:
            try:
                fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
            except OSError:
                print("basecache: another campaign runs the baseline %s; waiting for it" % key[:16], file=sys.stderr, flush=True)
                fcntl.flock(f, fcntl.LOCK_EX)
            yield
        finally:
            f.close()

    def staging(self, key):
        """a fresh directory in the cache for a baseline's records (moved into the entry when the baseline passes)"""
        return tempfile.mkdtemp(prefix=".staging-%s-%d-" % (key[:16], os.getpid()), dir=self.root)

    def lookup(self, key, refcache_key, check):
        """the stored PASSING entry of key, or None (no entry, or anything that does not hold: then the baseline runs).
        check(rc, stdout) is mutate.check_baseline's verdict (None = passes): a stored run must pass it again."""
        d = self.entry(key)
        try:
            j = json.load(open(os.path.join(d, "receipt.json")))
        except (OSError, ValueError):
            return None
        why = None
        if j.get("version") != VERSION or j.get("key") != key:
            why = "version/key mismatch"
        elif j.get("verdict") != "pass" or j.get("rc") != 0 or check(j.get("rc"), j.get("stdout", "")) is not None:
            why = "the stored baseline does not pass"  # never happens for entries this module wrote; never reused
        elif j.get("refcache_key") != refcache_key:
            why = "reference-side cache key differs"
        elif j.get("records") is not None and records_digest(os.path.join(d, "refcache")) != j["records"]:
            why = "record files differ from the receipt"
        if why:
            print("basecache: entry %s not used (%s); running the baseline" % (key[:16], why), file=sys.stderr)
            return None
        j["dir"] = d
        j["refcache_dir"] = os.path.join(d, "refcache") if j.get("records") is not None else None
        return j

    def store(self, key, parts, cmd, rc, stdout, wall, load, refcache_key, staged):
        """record a PASSING baseline (the caller checked it); staged: the directory of its reference-side
        records (moved to DIR/<key>/refcache) or None. True when stored; False (staged left in place) when the entry
        exists already or cannot be written."""
        if rc != 0:
            raise ValueError("basecache: only a passing baseline is stored")
        d = self.entry(key)
        if os.path.exists(d):
            return False
        tmp = tempfile.mkdtemp(prefix=".staging-entry-%s-%d-" % (key[:16], os.getpid()), dir=self.root)
        recs = None
        if staged:
            os.rename(staged, os.path.join(tmp, "refcache"))
            recs = records_digest(os.path.join(tmp, "refcache"))
        rec = {"version": VERSION, "key": key, "verdict": "pass", "rc": rc, "stdout": stdout, "wall": wall, "load": load,
               "refcache_key": refcache_key, "records": recs, "cmd": cmd, "covers": [p[:200] for p in parts],
               "time": time.strftime("%Y-%m-%d %H:%M:%S"), "pid": os.getpid()}
        json.dump(rec, open(os.path.join(tmp, "receipt.json"), "w"), indent=1)
        try:
            os.rename(tmp, d)
        except OSError:
            if staged:  # put the records back for this campaign
                os.rename(os.path.join(tmp, "refcache"), staged)
            shutil.rmtree(tmp, True)  # this call's own staging directory (receipt.json only by now)
            return False
        return True


def loadavg():
    return os.getloadavg()[0]


def prefetch(specs, mutate_args, cache_dir, parallel=2, load_max=85.0, poll=5.0, log_dir=None, env=None, root=ROOT,
             load=loadavg):
    """run the baselines of specs up front: `mutate.py SPEC --baseline-only --baseline-cache cache_dir MUTATE_ARGS`, at most
    `parallel` at a time, a new one only while the 1-minute load is below load_max. Returns {spec: exit code}
    (0 passed / cached, 3 failing baseline, else a tool error). Logs to log_dir/SPEC.baseline.log when given."""
    todo, running, res = list(specs), {}, {}
    while todo or running:
        for s, (p, fh) in list(running.items()):
            if p.poll() is not None:
                res[s] = p.returncode
                if fh:
                    fh.close()
                del running[s]
                print("basecache prefetch: %s exit %d" % (s, p.returncode), file=sys.stderr, flush=True)
        if todo and len(running) < parallel and (not running or load() < load_max):
            s = todo.pop(0)
            fh = open(os.path.join(log_dir, s.replace("/", "__") + ".baseline.log"), "w") if log_dir else None
            cmd = [sys.executable, os.path.join(HERE, "mutate.py"), s, "--baseline-only", "--baseline-cache", cache_dir] + \
                list(mutate_args)
            running[s] = (subprocess.Popen(cmd, cwd=root, env=env, stdout=fh or subprocess.DEVNULL,
                                           stderr=subprocess.STDOUT if fh else subprocess.DEVNULL), fh)
            continue
        if running:
            time.sleep(poll)
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("prefetch", help="run companion baselines up front into the cache (then the passes hit it)")
    p.add_argument("--cache", required=True)
    p.add_argument("--parallel", type=int, default=2)
    p.add_argument("--load-max", type=float, default=85.0)
    p.add_argument("--log-dir")
    p.add_argument("specs", nargs="+", help="companion specs; after `--`: the mutate.py arguments of the passes (--n, --input-seed, --nulltrap, -j, ...)")
    q = sub.add_parser("ls", help="list the entries of a cache directory (key, size, wall, time, first command words)")
    q.add_argument("cache")
    argv, margs = sys.argv[1:], []
    if "--" in argv:  # prefetch SPEC... -- mutate.py arguments
        k = argv.index("--")
        argv, margs = argv[:k], argv[k + 1:]
    a = ap.parse_args(argv)
    if a.cmd == "prefetch":
        specs = a.specs
        if a.log_dir:
            os.makedirs(a.log_dir, exist_ok=True)
        res = prefetch(specs, margs, a.cache, a.parallel, a.load_max, log_dir=a.log_dir)
        bad = {s: rc for s, rc in res.items() if rc}
        print("basecache prefetch: %d of %d baseline(s) pass%s" % (len(res) - len(bad), len(res),
              "; NOT passing (their passes will stop as void): " + " ".join("%s(exit %d)" % x for x in sorted(bad.items()))
              if bad else ""))
        return 1 if bad else 0
    for e in sorted(os.listdir(a.cache)):
        rp = os.path.join(a.cache, e, "receipt.json")
        if not os.path.isfile(rp):
            continue
        j = json.load(open(rp))
        size = sum(os.path.getsize(os.path.join(dp, f)) for dp, _, fs in os.walk(os.path.join(a.cache, e)) for f in fs)
        print("%s  %8.1f MB  %6.1f s  %s  %s" % (e[:16], size / 1e6, j.get("wall", 0), j.get("time"),
                                               " ".join(os.path.basename(x) for x in j.get("cmd", [])[:7])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
