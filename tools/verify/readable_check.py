#!/usr/bin/env python3
"""Acceptance check for a readability rewrite.

usage: readable_check.py TARGET... --ref REF [-n 10000] [--seeds 1,7] [--no-baseline]
                         [--allow-spec-change] [--keep] [--static-only] [--shards K] [--unscoped]

TARGET: a unit name (tools/verify/units/<unit>.txt), or a source file (wwhd_src/...cpp). Every unit
spec that compiles one of the targets' sources is checked, companion specs (units/<unit>/*.txt)
included, so a shared source is never checked through only one of its specs.

Checks, in order (any failure -> exit status 1):
  1. function set: the addresses each spec tests (VERIFY lines + /* ADDR */ markers of glue sources,
     exactly what mkunit.py tests) are the same as in REF (the source before the rewrite);
  2. spec files: the unit spec files are byte-identical to REF (steering, facts, `real`, rec lines:
     the inputs the verification uses). A change needs --allow-spec-change and a review;
  3. verification: verify.py on every spec, N generated inputs (default 10,000), each seed (default
     1 and 7): every function must pass all generated and recorded inputs;
  4. coverage: per function, the basic blocks covered must not drop below REF's (REF's numbers are
     measured once per REF commit/spec/seed in a cached export under build/readable/, unless
     --no-baseline). A coverage drop means the rewrite changed how inputs are generated (e.g. a
     parameter type) and hides paths: fix the signature or explain it in the review note.

Descriptive renames: tools/verify/readable_renames.tsv maps an old source
path (as in REF) to its new path (`old<TAB>new`, `#` comments). A spec whose only difference from REF
is that its `src` lines name the new paths of mapped renames counts as unchanged (check 2); the function
set (check 1) still compares the addresses of the renamed source with REF's old file. Every entry must
be a real rename: old exists in REF and not in HEAD, new exists in HEAD and not in REF. A renamed file
without a table entry fails check 2 (the spec's src line differs from REF).

Sharding and load: every verify.py run (candidate and REF baseline) is sharded: --shards K, else
WWHD_SHARDS, else 4 at a 1-minute load <= 30 and 2 above; each run starts only at a load <=
READABLE_LOAD_MAX (default 55). A sharded run equals an unsharded one (inputs are seeded by index).

Scoped companions: a companion spec that steers only some functions was gated (and merged) with
-only; readable_check runs it with the same -only set, from tools/verify/readable_scopes.tsv
(`spec<TAB>ADDR[,ADDR...]`) or a `# ... run with -only ADDR[,ADDR]` / `# readable-scope: ...` comment in
the spec. Checks 3/4 then cover exactly that set (a run returning other functions fails). --unscoped
runs every function of every spec (diagnostics only).

The summary line at the end is the result of the check. Build output of each spec is removed
afterwards (logs kept in build/readable/logs/) unless --keep.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
sys.path.insert(0, HERE)
from verify import RESULT_LINE as LINE  # noqa: E402  (same directory; names may contain spaces)


def git(*args, check=True):
    r = subprocess.run(["git"] + list(args), cwd=ROOT, capture_output=True, text=True)
    if check and r.returncode:
        sys.exit("readable_check: git %s failed: %s" % (" ".join(args), r.stderr.strip()))
    return r.stdout


def all_specs():
    out = []
    for dp, _, fs in os.walk(UNITS):
        for f in fs:
            if f.endswith(".txt"):
                out.append(os.path.relpath(os.path.join(dp, f), UNITS)[:-4])
    return sorted(out)


def spec_srcs(text):
    srcs = []
    for line in text.splitlines():
        w = line.split("#")[0].split()
        if w and w[0] == "src":
            srcs += w[1:]
    return srcs


def tested(src_texts):
    """the addresses mkunit.py tests for these source texts"""
    import autoglue
    t = set()
    for text in src_texts:
        t |= {int(m.group(1), 16) for m in re.finditer(r"^\s*VERIFY\(\s*0x([0-9A-Fa-f]{8})\s*,", text, re.M)}
        t |= {a for a, _, _ in autoglue.bodies(text) or []}
    return t


RENAMES = os.path.join(HERE, "readable_renames.tsv")


def load_renames(path=RENAMES):
    """{old path: new path} from readable_renames.tsv, and a list of format problems"""
    table, problems, news = {}, [], set()
    if not os.path.exists(path):
        return table, problems
    for n, line in enumerate(open(path), 1):
        line = line.split("#")[0].rstrip("\n")
        if not line.strip():
            continue
        w = line.split("\t") if "\t" in line else line.split()
        w = [x.strip() for x in w if x.strip()]
        where = "%s:%d" % (os.path.basename(path), n)
        if len(w) != 2:
            problems.append("%s: expected `old<TAB>new`, got %r" % (where, line))
            continue
        old, new = w
        if old == new or not old.startswith("wwhd_src/") or not new.startswith("wwhd_src/"):
            problems.append("%s: %s -> %s: both paths must be different wwhd_src/ paths" % (where, old, new))
        elif old in table or new in news or old in news or new in table:
            problems.append("%s: %s -> %s: path already used by another entry (no chains, no duplicates)" % (where, old, new))
        else:
            table[old] = new
            news.add(new)
    return table, problems


def map_spec(text, table):
    """the spec text with every mapped `src` path replaced by its new path (comments and spacing kept)"""
    out = []
    for line in text.splitlines(True):
        body, sep, comment = line.partition("#")
        w = body.split()
        if w and w[0] == "src" and any(x in table for x in w[1:]):
            for x in w[1:]:
                if x in table:
                    body = re.sub(r"(?<!\S)%s(?!\S)" % re.escape(x), table[x].replace("\\", "\\\\"), body)
            line = body + sep + comment
        out.append(line)
    return "".join(out)


def ref_file(ref, path):
    r = subprocess.run(["git", "show", "%s:%s" % (ref, path)], cwd=ROOT, capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None


def parse(out):
    res = {}
    for line in out.splitlines():
        m = LINE.match(line)
        if m:
            res[m.group(1)] = dict(name=m.group(2).strip(), gp=int(m.group(3)), gn=int(m.group(4)), rp=int(m.group(5)),
                                   rn=int(m.group(6)), cov=int(m.group(7)), nb=int(m.group(8)), first=m.group(9).strip())
    return res


LOAD_MAX = float(os.environ.get("READABLE_LOAD_MAX", "55"))


def wait_for_load():
    """the machine rule: start a heavy run only at a 1-minute load <= READABLE_LOAD_MAX (default 55)"""
    import time
    while os.getloadavg()[0] > LOAD_MAX:
        time.sleep(60)


def shards_for(requested):
    """-shards for one verify run: --shards / WWHD_SHARDS when given, else 4 at load <= 30 and 2 above
    (a sharded run equals an unsharded one: inputs are seeded by their index alone)"""
    if requested:
        return requested
    if os.environ.get("WWHD_SHARDS"):
        return int(os.environ["WWHD_SHARDS"])
    return 4 if os.getloadavg()[0] <= 30 else 2


SCOPES = os.path.join(HERE, "readable_scopes.tsv")
SCOPE_COMMENT = re.compile(r"^#.*?(?:run with -only|readable-scope:)\s*([0-9A-Fa-f]{8}(?:\s*,\s*[0-9A-Fa-f]{8})*)", re.M)


def load_scopes(path=SCOPES):
    """{spec: [ADDR, ...]} from readable_scopes.tsv (`spec<TAB>ADDR,ADDR  # source`): the published -only set of a
    scoped companion spec, i.e. the functions its gate covered when the unit was verified and merged"""
    table = {}
    if os.path.exists(path):
        for n, line in enumerate(open(path), 1):
            line = line.split("#")[0].strip()
            if not line:
                continue
            w = line.split()
            if len(w) != 2 or not all(re.fullmatch(r"[0-9A-Fa-f]{8}", x) for x in w[1].split(",")):
                sys.exit("readable_check: %s:%d: expected `spec<TAB>ADDR[,ADDR...]`, got %r" % (os.path.basename(path), n, line))
            table[w[0]] = [x.upper() for x in w[1].split(",")]
    return table


def spec_scope(spec, text, table):
    """the -only list for a spec (readable_scopes.tsv first, else a `# ... run with -only ADDR[,ADDR]` /
    `# readable-scope: ADDR[,ADDR]` comment in the spec), or None for an unscoped spec"""
    if spec in table:
        return table[spec], "readable_scopes.tsv"
    m = SCOPE_COMMENT.search(text)
    if m:
        return [x.strip().upper() for x in m.group(1).split(",")], "spec comment"
    return None, None


def run_verify(root, spec, n, seed, logdir, keep, shards=None, only=None):
    wait_for_load()
    cmd = [sys.executable, os.path.join(root, "tools", "verify", "verify.py"), spec, "-n", str(n), "-seed", str(seed),
           "-shards", str(shards_for(shards))]
    if only:
        cmd += ["-only", ",".join(only)]
    r = subprocess.run(cmd, cwd=root, capture_output=True, text=True)
    out = r.stdout + r.stderr
    os.makedirs(logdir, exist_ok=True)
    open(os.path.join(logdir, spec.replace("/", "__") + ".s%d.log" % seed), "w").write(out)
    if not keep:
        d = os.path.join(root, "build", "verify", spec)
        if os.path.isdir(d):
            for e in os.listdir(d):
                p = os.path.join(d, e)
                # companion specs build in build/verify/<unit>/<spec>: leave those to their own runs
                if os.path.exists(os.path.join(UNITS, spec, e + ".txt")):
                    continue
                shutil.rmtree(p) if os.path.isdir(p) else os.remove(p)
    return r.returncode, out


def ref_export(sha):
    """a cached export of REF for baseline runs: sources from git, generated inputs shared with this tree"""
    d = os.path.join(ROOT, "build", "readable", "ref-" + sha[:12])
    if not os.path.exists(os.path.join(d, ".complete")):
        shutil.rmtree(d, ignore_errors=True)
        os.makedirs(d)
        a = subprocess.Popen(["git", "archive", sha, "wwhd_src", "tools", "runtime/include", "runtime/src"],
                             cwd=ROOT, stdout=subprocess.PIPE)
        subprocess.run(["tar", "-x", "-C", d], stdin=a.stdout, check=True)
        a.wait()
        os.makedirs(os.path.join(d, "build", "verify"))
        os.symlink(os.path.realpath(os.path.join(ROOT, "game")), os.path.join(d, "game"))
        os.symlink(os.path.realpath(os.path.join(ROOT, "build", "gen")), os.path.join(d, "build", "gen"))
        for f in os.listdir(os.path.join(ROOT, "build")):
            if f.endswith(".tsv"):
                os.symlink(os.path.join(ROOT, "build", f), os.path.join(d, "build", f))
        for f in ("image.bin", "livein.tsv", "frames.tsv"):
            src = os.path.join(ROOT, "build", "verify", f)
            if os.path.exists(src):
                shutil.copy2(src, os.path.join(d, "build", "verify", f))
        open(os.path.join(d, ".complete"), "w").close()
    return d


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("targets", nargs="+")
    ap.add_argument("--ref", default=os.environ.get("READABLE_REF"), required="READABLE_REF" not in os.environ,
                    help="git ref of the original (pre-readability) sources to compare against (or READABLE_REF)")
    ap.add_argument("-n", type=int, default=10000)
    ap.add_argument("--seeds", default="1,7")
    ap.add_argument("--no-baseline", action="store_true", help="skip the coverage comparison with REF")
    ap.add_argument("--allow-spec-change", action="store_true")
    ap.add_argument("--keep", action="store_true", help="keep the build directories")
    ap.add_argument("--static-only", action="store_true", help="checks 1 and 2 only (no verification; never a merge PASS)")
    ap.add_argument("--shards", type=int, default=0,
                    help="verify.py -shards per run (default: WWHD_SHARDS, else 4 at load <= 30, 2 above)")
    ap.add_argument("--unscoped", action="store_true", help="ignore readable_scopes.tsv / scope comments (run every "
                    "function of every companion spec)")
    a = ap.parse_args()
    seeds = [int(s) for s in a.seeds.split(",")]
    ref_sha = git("rev-parse", a.ref).strip()
    head = git("rev-parse", "--short=8", "HEAD").strip()
    dirty = bool(git("status", "--porcelain", "--untracked-files=no").strip())

    renames, rproblems = load_renames()
    for old, new in sorted(renames.items()):
        if ref_file(ref_sha, old) is None:
            rproblems.append("readable_renames.tsv: %s -> %s: %s is not in %s" % (old, new, old, a.ref))
        if ref_file(ref_sha, new) is not None:
            rproblems.append("readable_renames.tsv: %s -> %s: %s already exists in %s (not a rename)" % (old, new, new, a.ref))
        if os.path.exists(os.path.join(ROOT, old)):
            rproblems.append("readable_renames.tsv: %s -> %s: %s still exists in HEAD (not a rename)" % (old, new, old))
        if not os.path.exists(os.path.join(ROOT, new)):
            rproblems.append("readable_renames.tsv: %s -> %s: %s does not exist in HEAD" % (old, new, new))

    # resolve targets -> sources -> every spec that compiles one of them
    specs = all_specs()
    texts = {s: open(os.path.join(UNITS, s + ".txt")).read() for s in specs}
    srcs = set()
    for t in a.targets:
        if t.endswith(".txt"):
            t = os.path.relpath(os.path.abspath(t), UNITS)[:-4]
        if t in texts:
            srcs |= set(spec_srcs(texts[t]))
        elif os.path.exists(os.path.join(ROOT, t)):
            srcs.add(os.path.relpath(os.path.join(ROOT, t), ROOT))
        elif os.path.relpath(os.path.join(ROOT, t), ROOT) in renames:  # old name of a renamed source
            srcs.add(renames[os.path.relpath(os.path.join(ROOT, t), ROOT)])
        else:
            sys.exit("readable_check: %s is neither a unit spec nor a source file" % t)
    chosen = [s for s in specs if set(spec_srcs(texts[s])) & srcs]
    if not chosen:
        sys.exit("readable_check: no unit spec compiles %s" % " ".join(sorted(srcs)))
    print("readable_check: HEAD %s%s vs %s %s; sources: %s" % (head, " (+uncommitted changes)" if dirty else "", a.ref,
                                                              ref_sha[:8], " ".join(sorted(srcs))))
    print("specs (%d): %s" % (len(chosen), " ".join(chosen)))
    scopes = {} if a.unscoped else load_scopes()
    scope_of = {}
    for s in chosen:
        only, origin = (None, None) if a.unscoped else spec_scope(s, texts[s], scopes)
        if only:
            scope_of[s] = only
            print("scoped: %-40s -only %s (%s)" % (s, ",".join(only), origin))
    problems = list(rproblems)
    if renames:
        print("renames (%d, readable_renames.tsv): %s" % (len(renames), " ".join(
            "%s->%s" % (o, n) for o, n in sorted(renames.items()) if n in srcs or o in srcs) or "none for these sources"))

    # 1 + 2: function set and spec files
    for s in chosen:
        path = "tools/verify/units/%s.txt" % s
        rtext = ref_file(ref_sha, path)
        if rtext is None:
            problems.append("%s: spec not in %s (new spec: review it before accepting)" % (s, a.ref))
            continue
        if rtext != texts[s] and renames and map_spec(rtext, renames) == texts[s]:
            print("spec ok (src renamed per readable_renames.tsv): %s" % s)
        elif rtext != texts[s]:
            msg = "%s: spec file differs from %s" % (s, a.ref)
            if a.allow_spec_change:
                print("NOTE " + msg + " (allowed; list the change in the review note)")
            else:
                problems.append(msg + " (rerun with --allow-spec-change after reviewing the spec change)")
        missing = [f for f in spec_srcs(texts[s]) if not os.path.exists(os.path.join(ROOT, f))]
        if missing:
            problems.append("%s: source missing in HEAD: %s%s" % (s, " ".join(missing), " (renamed? map it in readable_renames.tsv and update the src line)"
                                                                  if any(f not in renames.values() for f in missing) else ""))
            continue
        mine = tested(open(os.path.join(ROOT, f)).read() for f in spec_srcs(texts[s]))
        theirs_txt = [ref_file(ref_sha, f) for f in spec_srcs(rtext)]
        theirs = tested(t for t in theirs_txt if t is not None)
        if mine != theirs:
            gone = sorted(theirs - mine)
            new = sorted(mine - theirs)
            problems.append("%s: function set changed: missing %s; added %s" % (
                s, " ".join("%08X" % x for x in gone) or "-", " ".join("%08X" % x for x in new) or "-"))
        else:
            print("function set ok: %-40s %d functions" % (s, len(mine)))
    if problems:
        print("\n".join("FAIL " + p for p in problems))
        print("readable_check: FAIL (static checks; nothing was run)")
        sys.exit(1)
    if a.static_only:
        print("readable_check: static checks ok (--static-only: nothing was run; not a merge PASS)")
        sys.exit(0)

    # 3 + 4: verification and coverage
    logdir = os.path.join(ROOT, "build", "readable", "logs", head)
    refroot = None if a.no_baseline else ref_export(ref_sha)
    totals = dict(funcs=0, ok=0, cov=0, nb=0, refcov=0)
    for seed in seeds:
        for s in chosen:
            only = scope_of.get(s)
            rc, out = run_verify(ROOT, s, a.n, seed, logdir, a.keep, a.shards, only)
            res = parse(out)
            base = None
            if refroot:
                cache = os.path.join(ROOT, "build", "readable", "baseline", ref_sha[:12],
                                     "%s.n%d.s%d%s.log" % (s.replace("/", "__"), a.n, seed,
                                                           ".only-" + "-".join(only) if only else ""))
                if not os.path.exists(cache):
                    _, bout = run_verify(refroot, s, a.n, seed, os.path.join(refroot, "build", "logs"), False, a.shards, only)
                    os.makedirs(os.path.dirname(cache), exist_ok=True)
                    open(cache, "w").write(bout)
                base = parse(open(cache).read())
                if not base:
                    os.remove(cache)
                    problems.append("%s seed %d: the %s baseline run produced no results (see %s); rerun, or --no-baseline"
                                    % (s, seed, a.ref, os.path.join(refroot, "build", "logs")))
            if not res:
                problems.append("%s seed %d: no results (build failed?) - see %s" % (s, seed, logdir))
                print("\n".join("    " + l for l in out.splitlines()[-12:]))
                continue
            bad = [(ad, r) for ad, r in sorted(res.items())
                   if not (r["first"].startswith("ok") and r["gp"] == r["gn"] and r["rp"] == r["rn"] and r["gn"] + r["rn"] > 0)]
            drops = []
            for ad, r in sorted(res.items()):
                if base and ad in base and r["cov"] < base[ad]["cov"]:
                    drops.append("%s %s %d<%d" % (ad, r["name"], r["cov"], base[ad]["cov"]))
            totals["funcs"] += len(res)
            totals["ok"] += len(res) - len(bad)
            totals["cov"] += sum(r["cov"] for r in res.values())
            totals["nb"] += sum(r["nb"] for r in res.values())
            if base:
                totals["refcov"] += sum(base[ad]["cov"] for ad in res if ad in base)
            state = "ok" if not bad and rc == 0 else "FAIL"
            if only and set(res) != set(only):
                problems.append("%s seed %d: scoped run returned %s, expected -only %s" % (
                    s, seed, ",".join(sorted(res)) or "nothing", ",".join(only)))
            print("seed %d %-40s %3d/%3d ok  cov %d/%d%s  %s%s" % (
                seed, s, len(res) - len(bad), len(res), sum(r["cov"] for r in res.values()),
                sum(r["nb"] for r in res.values()),
                (" (ref %d)" % sum(base[ad]["cov"] for ad in res if ad in base)) if base else "", state,
                "  [scoped %d]" % len(only) if only else ""))
            for ad, r in bad:
                problems.append("%s seed %d: %s %s %d/%d gen %d/%d rec: %s" % (s, seed, ad, r["name"], r["gp"], r["gn"],
                                                                              r["rp"], r["rn"], r["first"]))
            for d in drops:
                problems.append("%s seed %d: coverage dropped: %s" % (s, seed, d))
    for p in problems:
        print("FAIL " + p)
    verdict = "PASS" if not problems else "FAIL"
    print("readable_check: %s  %s  %d specs, %d function runs ok of %d (n=%d, seeds %s), coverage %d/%d%s%s" % (
        verdict, head + ("+dirty" if dirty else ""), len(chosen), totals["ok"], totals["funcs"], a.n, a.seeds,
        totals["cov"], totals["nb"], (" (ref %d)" % totals["refcov"]) if refroot else " (no baseline)",
        "" if not dirty else "  [uncommitted changes: commit, then re-run for a clean result]"))
    sys.exit(0 if not problems else 1)


if __name__ == "__main__":
    main()
