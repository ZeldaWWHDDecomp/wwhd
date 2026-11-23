#!/usr/bin/env python3
"""true60 host-be<> probe (t60_test check 21).

The bug class: true60 host-side code (frame latches, per-frame state, glue) keeps a guest-layout object (a be<> field, a
cXyz, an Mtx34, a struct with such fields) in HOST memory: a host local, a host static, a std:: container element.
be<> is a view of guest memory (operator T = load<T>(ea(this))), so every access reads / writes the guest word at
ea(<host address>): garbage reads and stray guest writes. T-01 cannot see it when the code only runs at N > 1
(the frame latch read garbage at N = 2 only).

The check: every translation unit that includes a true60 header (`t60` / `true60` in the include name) is glued as
verify builds it and compiled with its verify flags plus gabi's static probe (-DGABI_HOST_BE_PROBE=1 -fsyntax-only:
be<>'s default constructor deleted, so every be<> object the host constructs is an error). The original code's
constructions in guest memory (an actor's constructor run by placement new: its fields' implicit construction is
reported at the constructor's own line) are accepted: an error located on a constructor definition line that is not a template instantiation. Every other
error is a finding, unless listed in PENDING (a known finding with its owner, reported but not failed).
Negative self-test: a host be<u32> local in a TU that includes true60.h is found.

Standalone: python3 tools/verify/t60_beprobe.py  (exit 0 when there is no new finding).
"""
import concurrent.futures
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

TMP = os.path.join(HERE, "units", "_t60_beprobe")
T60_INCLUDE = re.compile(r'^\s*#\s*include\s*[<"][^">]*(?:t60|true60)[^">]*[">]', re.M)
INC = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include", "-Ibuild/gen"]
# a constructor definition: `X::X(` (out of class) or `X(...) {` / `X() :` (in class): guest placement construction
CTOR = re.compile(r"^\s*(?:(?:[\w]+::)*(\w+)::\1\s*\(|(?:explicit\s+)?\w+\s*\([^;]*\)\s*(?::|\{))")
KEYWORD = re.compile(r"^\s*(?:if|for|while|switch|return|catch)\b")
ERR = re.compile(r"^(?P<file>[^:\n]+):(?P<line>\d+):\d+: error: (?P<msg>.*)$", re.M)
JOBS = int(os.environ.get("T60_BEPROBE_JOBS", "4"))

# Known findings (file, line text stripped) -> owner; reported, not failed. Owners remove their line with the fix.
PENDING = {
}


def sources():
    out = []
    for d in ("wwhd_src", "runtime/src"):
        for dp, _, fs in os.walk(os.path.join(ROOT, d)):
            for f in fs:
                if f.endswith(".cpp"):
                    p = os.path.join(dp, f)
                    if T60_INCLUDE.search(open(p, errors="replace").read()):
                        out.append(os.path.relpath(p, ROOT))
    return sorted(out)


def compile_probe(src, out):
    os.makedirs(out, exist_ok=True)
    g = verify.glue(src, out)
    r = subprocess.run(["clang++", "-std=c++20", "-fsyntax-only", "-w", "-ferror-limit=0"] + verify.FP + INC +
                       ["-I" + os.path.dirname(src), "-DGABI_HOST_BE_PROBE=1", g], cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stderr


def line_text(path, n, glued_dir):
    for p in (path if os.path.isabs(path) else os.path.join(ROOT, path),
              os.path.join(glued_dir, "glue", os.path.basename(path))):
        try:
            return open(p, errors="replace").read().splitlines()[n - 1].strip()
        except (OSError, IndexError):
            pass
    return ""


def findings(src, err, glued_dir):
    """the error locations that are not guest placement constructors: [(key, message)]"""
    res = []
    ms = list(ERR.finditer(err))
    for i, m in enumerate(ms):
        f, n, msg = m.group("file"), int(m.group("line")), m.group("msg")
        block = err[m.end():ms[i + 1].start() if i + 1 < len(ms) else len(err)]
        rel = os.path.relpath(f, ROOT) if os.path.isabs(f) else f
        if rel.startswith("..") or os.path.isabs(rel):  # a system header (std::pair / std::map of a guest type)
            t = re.search(r"of '(?:\(anonymous namespace\)::)?([\w:<>, \[\]]+)'", msg)
            res.append((("<system>", t.group(1) if t else msg), msg))
            continue
        if "/glue/" in rel:
            rel = src
        text = line_text(f, n, glued_dir)
        if CTOR.match(text) and not KEYWORD.match(text) and "in instantiation of" not in block:
            continue  # an original constructor (guest placement); a template's (host) instantiation is a finding
        res.append(((rel, text), msg))
    return res


def run_probe(srcs, tag):
    out = os.path.join(TMP, tag)
    res = {}
    with concurrent.futures.ThreadPoolExecutor(JOBS) as ex:
        jobs = {s: ex.submit(compile_probe, s, os.path.join(out, s.replace("/", "_"))) for s in srcs}
        for s, j in jobs.items():
            rc, err = j.result()
            if rc:
                for key, msg in findings(s, err, os.path.join(out, s.replace("/", "_"))):
                    res.setdefault(key, (s, msg))
    return res


NEG = """#include "wwhd.h"
#include "true60.h"
/* negative: the old frame latch's bug (a host be<> local decoding host bytes) */
u32 t60_beprobe_neg(const u8* b) {
    be<u32> v;
    __builtin_memcpy(&v, b, 4);
    return (u32)v;
}
"""


def beprobe_checks(fails):
    os.makedirs(TMP, exist_ok=True)
    try:
        neg = os.path.relpath(os.path.join(TMP, "neg.cpp"), ROOT)
        open(os.path.join(ROOT, neg), "w").write(NEG)
        r = run_probe([neg], "neg")
        if not any(k[0] == neg and "be<u32>" in k[1] for k in r):
            fails.append("beprobe negative: a host be<u32> local was not found: %r" % sorted(r))
        else:
            print("ok   beprobe negative: a host be<u32> local is found")
        srcs = sources()
        r = run_probe(srcs, "tree")
        new = {k: v for k, v in r.items() if k not in PENDING}
        for (f, text), owner in sorted(PENDING.items()):
            print("%s beprobe pending (%s): %s: %s" % ("    " if (f, text) in r else "note", owner, f, text) +
                  ("" if (f, text) in r else "  [no longer found: remove it from PENDING]"))
        for (f, text), (s, msg) in sorted(new.items()):
            fails.append("beprobe: host be<> object in %s: %s  (%s; TU %s)" % (f, text, msg, s))
        if not new:
            print("ok   beprobe: %d true60 TUs, no host be<> object beyond %d pending" %
                  (len(srcs), sum(1 for k in PENDING if k in r)))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)


def main():
    fails = []
    beprobe_checks(fails)
    for f in fails:
        print("FAIL", f)
    print("t60_beprobe: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
