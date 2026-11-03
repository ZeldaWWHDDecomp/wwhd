#!/usr/bin/env python3
"""nulltrap_compare.py: what -nulltrap changes for a unit (refined zero-page rule, 2026-10-26).

Inputs where the unmodified ORIGINAL touches page 0 count only up to the fault under -nulltrap. Fixtures need steering
only where that loses something compared with the run without -nulltrap:
  (a) a block covered without -nulltrap but not with it,
  (b) a mutant killed without --nulltrap but not with it,
  (c) a function with fewer clean inputs (inputs without an original page-0 DSI) than 10% of n (1,000 of 10,000) per seed.

Verify mode (runs verify.py per seed, -v -v, with -nulltrap and then without it where that can differ):
    nulltrap_compare.py SPEC [-n 10000] [-seed 1 -seed 7] [-buildj 4] [--full] [other verify.py options...]
  (other verify.py options include --observe-pointees / -observe-pointees: both sides then compare pointee objects at mocked calls)
  prints per function and seed: rows (without / with), coverage, clean inputs, LOST blocks, LOW clean, and a final
  verdict line `nulltrap_compare: SPEC: OK`, `... FAIL: <functions>` or `... STEER: <functions>`. OK means the gate
  passed: every row on both sides passed (candidate = original on every input) and nothing is lost. FAIL (exit 1,
  2026-10-28): a function row on either side has passes < total (generated or recorded; a row whose inputs aborted
  on both sides is not short of passes, and a total below n stays INCOMPLETE as below); one `FAIL seed S ADDR SIDE
  rows ...: <first difference>` line per failing row, and FAIL wins over STEER and INCOMPLETE (the verdict line then
  also lists the STEER / INCOMPLETE functions). STEER (exit 1): the rows pass but -nulltrap loses something. Exit 2 when
  a run yields no function rows (verify failed, e.g. a spec path instead of a unit name), verify.py exits other than 0/1
  (e.g. a dead shard), or a row's generated total is not n without inputs aborted on both sides (`INCOMPLETE (rerun)`).
  Redundant runs are skipped (2026-10-27): -nulltrap changes a run only at a page-0 access (harness.cpp nulltrap_hit; the
  memory, setups, mocks, input generation and budgets are the same), so an input on which neither side took a page-0 DSI
  gives the same result without it. The -nulltrap run writes those inputs (-dsiout, a -runlist); the run without -nulltrap
  runs only them, and the row without -nulltrap is the -nulltrap row with those inputs' results replaced (counts: minus
  their -nulltrap passes, plus their plain passes; uncovered blocks: the -nulltrap ones that the plain inputs did not
  cover either - a DSI input's -nulltrap blocks are a prefix of its plain run). A function with no DSI input is not run
  without -nulltrap at all (`plain = nulltrap (all clean)`). The inputs draw on a dictionary of .rodata constants the
  original reads in 8 pre-runs; when a pre-run takes a DSI the harness repeats the pre-runs without -nulltrap, and only
  if that dictionary differs (so the inputs may differ) the function runs whole without -nulltrap. --full runs both sides whole, as before. -shards K shards both sides (verify.py
  shards -v -v runs since 840e74944, 2026-10-27; the -dsiout file is written per shard and joined in shard order).
  --logdir DIR (2026-10-30: the gate's raw logs are the coverage evidence, no separate `verify.py -v -v` coverage
  pass): per seed S writes DIR/SPEC.sS.nulltrap.out/.err (the -nulltrap side's verify.py -v -v output as printed: every
  row, `uncovered blocks:` list, per-input FAIL/unusable line and stderr note; the same text a separate
  `verify.py SPEC -n N -seed S -nulltrap -v -v` prints), DIR/SPEC.sS.plain.out/.err (the side without -nulltrap: the
  page-0 inputs' run, or the whole run with --full / a dictionary change), DIR/SPEC.sS.dsiout.txt and
  DIR/SPEC.sS.coverage.txt (per function and side: rows, coverage, clean inputs and the uncovered blocks; the plain side's
  are the merged row's, equal to a separate plain -v -v run).
  --refcache-out DIR (2026-10-30): the -nulltrap side also writes the original's reference-side records
  (harness -refcache-write; sharded runs join their parts) into DIR/SPEC/sS with a manifest refcache.json carrying
  mutate.py's campaign key for `mutate.py SPEC --nulltrap --n N --input-seed S` (objects, spec, harness options,
  recordings, tools hash); an earlier DIR/SPEC/sS is renamed to sS.oldK, never mixed in. Survivor replays with
  `mutate.py SPEC ... --refcache-from DIR` reuse them when the key, n and
  seed match exactly instead of running the unmutated original again. Only with options that do not change the
  original's run (-shards, -buildj, --archive, --no-cache, -observe-pointees, -noprogress, -progress-interval); any
  other verify option turns it off with a note.
Receipts mode (two mutate.py --receipts directories of the same campaign, one run with --nulltrap):
    nulltrap_compare.py --receipts DIR_WITHOUT DIR_WITH
  lists mutants whose class was `difference` without --nulltrap but is not with it (kills lost), and all other class
  changes for information (e.g. abort/timeout -> difference, which is the intended gain). Exit 0 when no kill is lost.
Self-test: nulltrap_compare_test.py.
"""
import json, os, re, shutil, subprocess, sys, tempfile
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import progress  # noqa: E402  (progress lines, 2026-10-27; the verify output it parses stays exactly as before)
ROW = re.compile(r"^([0-9A-F]{8})\s+.*?\s(\d+)/(\d+)\s+(\d+)/(\d+)(?:\(-?\d+\))?\s+(\d+)/(\d+)\s+(.*)$")
AB_NOTE = " (some inputs aborted on both sides)"
UNC = re.compile(r"^\s+uncovered blocks:\s*([0-9 ]*)$")
DSI = re.compile(r"nulltrap: ([0-9A-F]{8}): the original took a page-0 DSI on (\d+) input")
P0X = re.compile(r"p0exclude: ([0-9A-F]{8}): (\d+) difference\(s\) after the original's page-0 access excluded \(zero-page rule\); first: (.*)$", re.M)


def parse(out, err):
    """per function: dict(gen=(ok,total), cov=(hit,total), status, uncovered=set, dsi=int)"""
    res, cur = {}, None
    for line in out.splitlines():
        m = ROW.match(line)
        if m:
            cur = m.group(1)
            res[cur] = dict(gen=(int(m.group(2)), int(m.group(3))), rec=(int(m.group(4)), int(m.group(5))), cov=(int(m.group(6)), int(m.group(7))),
                            status=m.group(8).strip(), uncovered=set(), dsi=0, p0x=0, p0x_first="",
                            ab="(some inputs aborted on both sides)" in m.group(8))
            continue
        m = UNC.match(line)
        if m and cur:
            res[cur]["uncovered"] |= {int(x) for x in m.group(1).split()}
    for m in DSI.finditer(err):  # one line per shard: sum
        if m.group(1) in res:
            res[m.group(1)]["dsi"] += int(m.group(2))
    for m in P0X.finditer(err):  # -p0exclude: one line per shard: sum
        if m.group(1) in res:
            res[m.group(1)]["p0x"] += int(m.group(2))
            res[m.group(1)]["p0x_first"] = res[m.group(1)]["p0x_first"] or m.group(3)
    return res


def parse_dsiout(text):
    """-dsiout file: {addr: (indices or "all", passed indices)}; functions without a line had no DSI input. A sharded run
    has one record per shard and function: united"""
    sel, passed, done = {}, {}, set()
    for line in text.splitlines():
        f = line.split()
        if not f:
            continue
        if f[0] == "#":
            if len(f) >= 3 and f[2] == "pass":
                passed.setdefault(f[1], set()).update(int(x) for x in f[3].split(","))
            elif len(f) >= 3 and f[2] == "done":
                done.add(f[1])
            continue
        if f[1] == "all" or sel.get(f[0]) == "all":
            sel[f[0]] = "all"
        else:
            sel.setdefault(f[0], set()).update(int(x) for x in f[1].split(","))
    return {a: (sel[a], passed.get(a, set())) for a in sel}, done


def merge_plain(nt, part, dsi, n):
    """the full run without -nulltrap from the -nulltrap run and the plain run of the DSI inputs only (see the module
    docstring); returns (plain results, note per function)"""
    out, notes = {}, {}
    for f, q in nt.items():
        if f not in dsi:
            out[f] = dict(q, dsi=0)
            notes[f] = "plain = nulltrap (all clean)"
            continue
        idx, passed = dsi[f]
        p = part.get(f)
        if p is None:
            continue  # missing: compare() reports it
        if idx == "all":
            out[f] = p
            notes[f] = "plain run whole (a pre-run DSI changes the dictionary)"
            continue
        gen = [i for i in idx if i < n]
        gpass = sum(1 for i in passed if i < n)
        out[f] = dict(q, dsi=0, gen=(q["gen"][0] - gpass + p["gen"][0], q["gen"][1] - len(gen) + p["gen"][1]),
                      uncovered=q["uncovered"] & p["uncovered"], ab=q.get("ab") or p.get("ab"))
        out[f]["cov"] = (q["cov"][1] - len(out[f]["uncovered"]), q["cov"][1])
        if not passed_row(out[f]):  # the first difference of the merged row: the plain inputs' if they failed
            out[f]["status"] = p["status"] if not passed_row(p) else q["status"]
        elif not passed_row(q) or not passed_row(p):
            out[f]["status"] = "ok"
        notes[f] = "plain on %d page-0 input(s)" % len(idx)
    return out, notes


def passed_row(r):
    """verify.py's own row verdict: every generated and recorded input passed (aborts on both sides are not counted)"""
    return r["gen"][0] == r["gen"][1] and r.get("rec", (0, 0))[0] == r.get("rec", (0, 0))[1]


def failures(plain, nt):
    """(function, side, row text, first difference) for every row with passes < total, either side (2026-10-28: an OK
    verdict over a failing row read as a gate pass)"""
    out = []
    for f in sorted(set(plain) | set(nt)):
        for side, r in (("plain", plain.get(f)), ("-nulltrap", nt.get(f))):
            if r and not passed_row(r):
                rows = "%d/%d" % r["gen"] + (" recorded %d/%d" % r["rec"] if r.get("rec", (0, 0))[1] else "")
                out.append((f, side, rows, r["status"].replace(AB_NOTE, "").strip() or "?"))
    return out


def compare(plain, nt, n):
    """rows of findings; lost = functions needing steering"""
    rows, lost = [], []
    for f in sorted(set(plain) | set(nt)):
        p, q = plain.get(f), nt.get(f)
        if not p or not q:
            rows.append((f, "missing in one run", True)); lost.append(f); continue
        # every generated input counts unless both sides aborted on it (the row then says so): a total below n without
        # that note is an incomplete run
        short = [w for w, r in (("plain", p), ("-nulltrap", q)) if r["gen"][1] != n and not r.get("ab")]
        if short:
            rows.append((f, "INCOMPLETE rows %s / %s (total != %d, no inputs aborted on both sides: %s run cut short)" % (
                "%d/%d" % p["gen"], "%d/%d" % q["gen"], n, " and ".join(short)), True))
            lost.append(f)
            continue
        gone = sorted(q["uncovered"] - p["uncovered"])
        clean = n - q["dsi"]
        low = clean < n // 10
        bad = bool(gone) or low
        rows.append((f, "rows %s / %s, coverage %d/%d / %d/%d, clean %d/%d%s%s" % (
            "%d/%d" % p["gen"], "%d/%d" % q["gen"], p["cov"][0], p["cov"][1], q["cov"][0], q["cov"][1], clean, n,
            ", LOST blocks %s" % " ".join(map(str, gone)) if gone else "", ", LOW clean" if low else ""), bad))
        if bad:
            lost.append(f)
    return rows, lost


RESUMED = []  # --resume: (seed, side, DIR, functions taken from DIR) for the verdict's audit lines


def resume_note(seed, side, rdir, err):
    """the functions verify.py took from the campaign's parts (its `resume: ADDR from DIR` notes)"""
    fs = sorted(set(re.findall(r"^resume: ([0-9A-F]{8}) from ", err or "", re.M)))
    RESUMED.append((seed, side, rdir, fs))
    print("nulltrap_compare: resume: seed %d %s side: %d function(s) from %s%s" % (
        seed, side, len(fs), rdir, (": " + " ".join(fs)) if fs else ""))


def run_verify(spec, args, seed, nulltrap, extra=()):
    cmd = [sys.executable, os.path.join(HERE, "verify.py"), spec] + args + ["-seed", str(seed), "-v", "-v"] + list(extra)
    if nulltrap:
        cmd.append("-nulltrap")
    r = progress.run_captured(cmd, "nulltrap_compare %s seed %d%s%s" % (spec, seed, " -nulltrap" if nulltrap else "",
                              " (page-0 inputs)" if "-runlist" in extra else ""), cwd=ROOT, errors="replace")
    if r.returncode not in (0, 1):  # verify failed or a shard died: its rows (if any) are not a result
        print("nulltrap_compare: %s: seed %d: verify.py%s exited %d; no verdict. verify output tail:\n%s" % (
            spec, seed, " -nulltrap" if nulltrap else "", r.returncode, (r.stderr or r.stdout)[-1500:]))
        sys.exit(2)
    return r.stdout, r.stderr


def coverage_text(spec, seed, n, plain, nt, notes):
    """the coverage evidence of one seed from the gate's own runs: per function and side the row, coverage, clean
    inputs and uncovered blocks (the plain side's merged row, as a separate plain -v -v run prints it)"""
    out = ["# nulltrap_compare coverage: %s seed %d n %d (from the gate's -v -v runs; no separate coverage pass)" % (spec, seed, n)]
    for f in sorted(set(plain) | set(nt)):
        for side, r in (("-nulltrap", nt.get(f)), ("plain", plain.get(f))):
            if not r:
                out.append("%s %-9s missing" % (f, side))
                continue
            out.append("%s %-9s rows %d/%d coverage %d/%d%s uncovered: %s%s" % (
                f, side, r["gen"][0], r["gen"][1], r["cov"][0], r["cov"][1],
                " clean %d/%d" % (n - r["dsi"], n) if side == "-nulltrap" else "",
                " ".join(map(str, sorted(r["uncovered"]))) or "-",
                " [%s]" % notes[f] if side == "plain" and f in notes else ""))
    return "\n".join(out) + "\n"


# verify options that do not change the original's run (or only its printing/sharding): the -nulltrap side's records are
# then those of `mutate.py SPEC --nulltrap --n N --input-seed S`
RC_SAFE = {"-shards": 1, "-buildj": 1, "--archive": 0, "--no-cache": 0, "-observe-pointees": 0, "--observe-pointees": 0,
           "-noprogress": 0, "-progress-interval": 1}


def refcache_plan(spec, args):
    """(function(seed, n) -> (dir args, key), None) for --refcache-out, or (None, why not)"""
    i, observe, buildj = 0, False, 1
    while i < len(args):
        if args[i] == "-n":
            i += 2; continue
        if args[i] not in RC_SAFE:
            return None, "verify option %s may change the original's run" % args[i]
        observe |= args[i] in ("-observe-pointees", "--observe-pointees")
        if args[i] == "-buildj":
            buildj = int(args[i + 1])
        i += 1 + RC_SAFE[args[i]]
    import verify, specfile, refcache, mutate  # noqa: E401 (only for --refcache-out)
    hdr, hdr_extra, scope = verify.header_args(spec), [], None
    i = 0
    while i < len(hdr):  # as mutate.py reads the spec header
        if hdr[i] == "-only":
            scope = mutate.parse_scope(hdr[i + 1]); i += 2
        else:
            hdr_extra.append(hdr[i]); i += 1
    sys.stdout.flush()  # the objects the key hashes (verify.py's own build then finds them current); its lines go to stderr
    fd = os.dup(1)
    os.dup2(2, 1)
    try:
        verify.build(spec, jobs=buildj)
    finally:
        sys.stdout.flush()
        os.dup2(fd, 1)
        os.close(fd)
    out = verify.unit_dir(spec)
    sp = specfile.spec_path(spec, ROOT, out)
    rec = verify.unit_rec(spec)

    def plan(seed, n):
        return refcache.key(out, sp, refcache.harness_args(sp, n, seed, hdr_extra, True, observe, scope, rec), rec, ROOT,
                            verify.support_objs())
    return plan, None


def receipts_classes(d):
    out = {}
    for fn in os.listdir(d):
        m = re.match(r"mutant(\d+)\.json$", fn)
        if m:
            out[int(m.group(1))] = json.load(open(os.path.join(d, fn))).get("class")
    return out


def compare_receipts(a, b):
    ca, cb = receipts_classes(a), receipts_classes(b)
    lost, changed = [], []
    for i in sorted(set(ca) & set(cb)):
        if ca[i] == "difference" and cb[i] != "difference":
            lost.append((i, ca[i], cb[i]))
        elif ca[i] != cb[i]:
            changed.append((i, ca[i], cb[i]))
    return lost, changed, sorted(set(ca) ^ set(cb))


def main(argv):
    import verify as _scratch_verify  # noqa: E402  (same directory)
    _scratch_verify.check_scratch_env('nulltrap_compare.py')  # a set scratch variable naming a missing/unwritable folder stops here, nothing written
    if argv[:1] == ["--receipts"]:
        lost, changed, only = compare_receipts(argv[1], argv[2])
        for i, x, y in lost:
            print("KILL LOST  #%d: %s without --nulltrap, %s with" % (i, x, y))
        for i, x, y in changed:
            print("changed    #%d: %s -> %s" % (i, x, y))
        if only:
            print("in one directory only: %s" % " ".join(map(str, only)))
        print("nulltrap_compare: receipts: %s" % ("OK" if not lost else "STEER: %d kill(s) lost" % len(lost)))
        return 1 if lost else 0
    spec, args, seeds, n, full, logdir, rcout, resume = argv[0], [], [], 10000, False, None, None, None
    i = 1
    while i < len(argv):
        if argv[i] == "--full":
            full = True; i += 1; continue
        if argv[i] == "--resume":
            resume = os.path.abspath(argv[i + 1]); i += 2; continue
        if argv[i] == "--logdir":
            logdir = argv[i + 1]; i += 2; continue
        if argv[i] == "--refcache-out":
            rcout = argv[i + 1]; i += 2; continue
        if argv[i] == "-seed":
            seeds.append(int(argv[i + 1])); i += 2; continue
        if argv[i] == "-n":
            n = int(argv[i + 1])
        args.append(argv[i]); i += 1
    if "-n" not in args:
        args += ["-n", str(n)]
    rckey = None
    if rcout:
        rckey, why = refcache_plan(spec, args)
        if rckey is None:
            print("nulltrap_compare: %s: --refcache-out off: %s" % (spec, why))
    if logdir:
        os.makedirs(logdir, exist_ok=True)

    def logpath(seed, what):
        """DIR/SPEC.sS.WHAT; a unit-directory spec (lib2_agl_7/nt_base) keeps its subdirectory, created here"""
        p = os.path.join(logdir, "%s.s%d.%s" % (spec, seed, what))
        os.makedirs(os.path.dirname(p), exist_ok=True)
        return p

    def log(seed, what, text):
        if logdir:
            with open(logpath(seed, what), "w") as fh:
                fh.write(text)

    def rc_args(seed):
        """-refcache-write for the -nulltrap side: a fresh DIR/sS (records of an earlier gate never mix in)"""
        if not rckey:
            return []
        d = os.path.join(rcout, spec, "s%d" % seed)
        if os.path.exists(d):  # an earlier gate's directory is moved aside, never mixed with or deleted
            k = 1
            while os.path.exists("%s.old%d" % (d, k)):
                k += 1
            os.rename(d, "%s.old%d" % (d, k))
            print("nulltrap_compare: %s: earlier records moved to %s.old%d" % (spec, d, k))
        os.makedirs(d)
        return ["-refcache-write", d, "-refcache-key", rckey(seed, n)]

    def rc_manifest(seed, qq):
        if not rckey or not qq:
            return
        d = os.path.join(rcout, spec, "s%d" % seed)
        have = sorted(f[:8] for f in os.listdir(d) if re.match(r"^[0-9A-F]{8}\.ref$", f))
        import refcache
        refcache.write_manifest(d, key=rckey(seed, n), spec=spec, n=n, seed=seed, nulltrap=True,
                                functions=sorted(qq), records=have, rows_pass=all(passed_row(r) for r in qq.values()),
                                tools=refcache.tools_hash())
        print("nulltrap_compare: %s: seed %d: reference-side records of %d/%d function(s) in %s" % (
            spec, seed, len(have), len(qq), d))

    def rs_args(seed, side, dsitext=None):
        """--resume: the per-seed, per-side part directory. The page-0 side (the plain run of the -nulltrap side's DSI
        inputs) is keyed on the -dsiout content: a rerun -nulltrap side with other records never reuses its parts"""
        if not resume:
            return [], None
        sub = "s%d.%s" % (seed, side)
        if dsitext is not None:
            import hashlib
            sub += "." + hashlib.sha256(dsitext.encode()).hexdigest()[:16]
        d = os.path.join(resume, spec.replace("/", "__"), sub)
        os.makedirs(d, exist_ok=True)
        return ["--resume", d], d

    if resume:
        print("nulltrap_compare: %s: resume directory %s (functions this campaign finished earlier with the same key are "
              "taken from it; listed per seed and side)" % (spec, resume))
    allost, incomplete, failed = set(), set(), []
    for s in seeds or [1, 7]:
        notes = {}
        if full:
            ra, rd = rs_args(s, "plain-full")
            po, pe = run_verify(spec, args, s, False, ["-p0exclude"] + ra)
            if rd:
                resume_note(s, "plain (--full)", rd, pe)
            log(s, "plain.out", po); log(s, "plain.err", pe)
            ra, rd = rs_args(s, "nulltrap-full")
            qo, qe = run_verify(spec, args, s, True, rc_args(s) + ra)
            if rd:
                resume_note(s, "-nulltrap (--full)", rd, qe)
            log(s, "nulltrap.out", qo); log(s, "nulltrap.err", qe)
            pp, qq = parse(po, pe), parse(qo, qe)
            rc_manifest(s, qq)
        else:
            with tempfile.TemporaryDirectory(prefix="nulltrap_compare.", dir=os.environ.get("WWHD_VERIFY_SCRATCH") or None) as d:
                dpath, rpath = os.path.join(d, "dsi.txt"), os.path.join(d, "runlist.txt")
                ra, rd = rs_args(s, "nulltrap")
                qo, qe = run_verify(spec, args, s, True, ["-dsiout", dpath] + rc_args(s) + ra)
                if rd:
                    resume_note(s, "-nulltrap", rd, qe)
                log(s, "nulltrap.out", qo); log(s, "nulltrap.err", qe)
                if logdir and os.path.exists(dpath):
                    shutil.copyfile(dpath, logpath(s, "dsiout.txt"))
                qq = parse(qo, qe)
                rc_manifest(s, qq)
                dsi, done = parse_dsiout(open(dpath).read()) if os.path.exists(dpath) else ({}, set())
                if qq and set(qq) - done:  # an older harness or a cut run: no -dsiout record, never assume clean
                    print("nulltrap_compare: %s: seed %d: no -dsiout record for %s; running both sides whole" % (
                        spec, s, " ".join(sorted(set(qq) - done))))
                    dsi = {f: ("all", set()) for f in qq}
                po, pe, part = "", "", {}
                if qq and dsi:
                    with open(rpath, "w") as w:
                        for f, (idx, _) in sorted(dsi.items()):
                            w.write("%s %s\n" % (f, "all" if idx == "all" else ",".join(map(str, sorted(idx)))))
                    ra, rd = rs_args(s, "plain", open(dpath).read() if os.path.exists(dpath) else "")
                    po, pe = run_verify(spec, args, s, False, ["-runlist", rpath, "-p0exclude"] + ra)
                    if rd:
                        resume_note(s, "plain (page-0 inputs)", rd, pe)
                    log(s, "plain.out", po); log(s, "plain.err", pe)
                    part = parse(po, pe)
                pp, notes = merge_plain(qq, part, dsi, n) if qq and (part or not dsi) else ({}, {})
        if not pp or not qq:  # verify failed (e.g. a spec path instead of a unit name): never a vacuous OK
            bad_plain = bool(qq)
            print("nulltrap_compare: %s: seed %d: no function rows (%s run); verify output tail:\n%s" % (
                spec, s, "plain" if bad_plain else "-nulltrap", ((pe if bad_plain else qe) or (po if bad_plain else qo))[-800:]))
            return 2
        for f, r in sorted((part if not full else pp).items()) if (full or (qq and dsi)) else ():
            if r.get("p0x"):  #: plain differences after the original's page-0 access, excluded
                notes[f] = (notes.get(f, "") + "; " if notes.get(f) else "") + \
                    "%d plain difference(s) after the original's page-0 access excluded" % r["p0x"]
                print("nulltrap_compare: seed %d %s: %d plain difference(s) after the original's page-0 access excluded "
                      "(zero-page rule); first: %s" % (s, f, r["p0x"], r["p0x_first"]))
        log(s, "coverage.txt", coverage_text(spec, s, n, pp, qq, notes))
        rows, lost = compare(pp, qq, n)
        for f, text, bad in rows:
            print("%s seed %d %s %s%s" % ("!!" if bad else "  ", s, f, text, " [%s]" % notes[f] if f in notes else ""))
        allost |= set(lost)
        for f, side, rowtxt, first in failures(pp, qq):
            failed.append(f)
            print("FAIL seed %d %s %s rows %s: %s" % (s, f, side, rowtxt, first))
        incomplete |= {f for f, text, _ in rows if text.startswith("INCOMPLETE")}
    if logdir and RESUMED:  # the audit record of the resume: which functions came from which part directory
        p = os.path.join(logdir, "%s.resume.json" % spec)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        with open(p, "w") as fh:
            json.dump({"spec": spec, "resume_dir": resume, "n": n,
                       "runs": [{"seed": sd, "side": sdname, "dir": d, "functions": fs} for sd, sdname, d, fs in RESUMED]},
                      fh, indent=1)
    if failed:  # a failing row is a failed gate whatever else the comparison finds
        print("nulltrap_compare: %s: FAIL: %s%s%s" % (spec, " ".join(sorted(set(failed))),
              "; STEER: " + " ".join(sorted(allost - incomplete)) if allost - incomplete else "",
              "; INCOMPLETE: " + " ".join(sorted(incomplete)) if incomplete else ""))
        return 1
    if incomplete:  # no verdict from a cut-short run: rerun
        print("nulltrap_compare: %s: INCOMPLETE (rerun): %s" % (spec, " ".join(sorted(incomplete))))
        return 2
    print("nulltrap_compare: %s: %s" % (spec, "OK" if not allost else "STEER: " + " ".join(sorted(allost))))
    return 1 if allost else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
