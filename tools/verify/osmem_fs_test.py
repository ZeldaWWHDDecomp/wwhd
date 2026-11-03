#!/usr/bin/env python3
"""Self-test of `osmem ... fs` and `osmemfile GUESTPATH SHA256|absent` (harness.cpp namespace osm; opt-in part, not in
`hle`): the coreinit FS imports answered read-only from declared game files as the port's runtime/src/hle/fs.cpp does.
 1. A declared file with its SHA-256 (computed here from game/content/Common/env.baglenv, read only) is accepted; the
    same spec's rows are ok.
 2. Refused (exit 2): a wrong hash, a missing file with a hash, `absent` for a file that exists, the same file twice
    (another case spelling), a /vol/save path, malformed lines.
 3. A declared-absent missing file is accepted.
 4. `osmem hle` alone leaves the FS imports to other directives (a `ret!` on FSOpenFile is accepted); with `fs` the same
    line is a conflict (exit 2).
 5. The model's call semantics against the port's own fs.cpp: osmem_fs_equiv.py.
Spec files stay under the verify scratch (printed). Exit 0 when all pass."""
import hashlib
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "_osmem_fs_test/fs"
F = "02756D18"
BASE = ["src wwhd_src/sead/PoolHeap.cpp", "src wwhd_src/sead/PoolHeapStatics.cpp", "src wwhd_src/sead/HeapRegistryStatics.cpp",
        "real 028F588C", "arg! 02756D18 r3 0x5A000000", "arg! 02756D18 r4 0x100", "setup 02756D18 028F588C r3=0x210"]
GFILE = "/vol/content/Common/env.baglenv"


def main():
    fails = []
    udir = os.path.join(HERE, "units", "_osmem_fs_test")
    os.makedirs(udir, exist_ok=True)
    with open(os.path.join(udir, "fs.txt"), "w") as f:
        f.write("\n".join(["# verify: -strictnan"] + BASE + ["osmem hle"]) + "\n")
    exe = verify.build(UNIT, jobs=8)
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="osmem_fs_test-", dir=base)
    print("osmem_fs_test: files in %s (not removed)" % W)
    sha = hashlib.sha256(open(os.path.join(ROOT, "game", "content", "Common", "env.baglenv"), "rb").read()).hexdigest()

    def run(name, lines, n="200"):
        p = os.path.join(W, name + ".txt")
        with open(p, "w") as f:
            f.write("\n".join(BASE + lines) + "\n")
        r = subprocess.run([exe, "-spec", p, "-noprogress", "-strictnan", "-n", n, "-seed", "1", "-only", F], cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr

    def ok_row(out):
        rows = [l for l in out.splitlines() if l.startswith(F)]
        return len(rows) == 1 and rows[0].rstrip().endswith(" ok")

    rc, out, err = run("good", ["osmem hle fs", "osmemfile %s %s" % (GFILE, sha)])
    if rc != 0 or not ok_row(out):
        fails.append("1: declared file refused or row not ok: rc %d %s" % (rc, err[-300:]))
    rc, out, err = run("absent", ["osmem hle fs", "osmemfile /vol/content/Common/NoSuchFile.bin absent"])
    if rc != 0 or not ok_row(out):
        fails.append("3: declared-absent file refused: rc %d %s" % (rc, err[-300:]))
    bad = {"wrong hash": ["osmem hle fs", "osmemfile %s %s" % (GFILE, "0" * 64)],
           "missing with hash": ["osmem hle fs", "osmemfile /vol/content/Common/NoSuchFile.bin %s" % sha],
           "absent but exists": ["osmem hle fs", "osmemfile %s absent" % GFILE],
           "twice": ["osmem hle fs", "osmemfile %s %s" % (GFILE, sha), "osmemfile %s %s" % (GFILE.upper().replace("/VOL/CONTENT", "/vol/content"), sha)],
           "save": ["osmem hle fs", "osmemfile /vol/save/common/x.sav %s" % sha],
           "malformed 1": ["osmem hle fs", "osmemfile %s" % GFILE],
           "malformed 2": ["osmem hle fs", "osmemfile %s abc" % GFILE],
           "conflict": ["osmem hle fs", "ret! C0008E98 0"]}
    for k, lines in bad.items():
        rc, out, err = run("bad", lines, n="1")
        if rc != 2:
            fails.append("2/4: %s not refused (rc %d)" % (k, rc))
    rc, out, err = run("nofs", ["osmem hle", "ret! C0008E98 0"], n="1")
    if rc != 0:
        fails.append("4: without `fs` a ret! on FSOpenFile should be accepted (rc %d) %s" % (rc, err[-200:]))
    r = subprocess.run([sys.executable, os.path.join(HERE, "osmem_fs_equiv.py")], capture_output=True, text=True)
    if r.returncode != 0:
        fails.append("5: osmem_fs_equiv: %s" % r.stdout.strip()[-300:])
    for f in fails:
        print("FAIL", f)
    print("osmem_fs_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
