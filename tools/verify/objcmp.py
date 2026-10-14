#!/usr/bin/env python3
"""Object-code identity check for header-only changes (readability pass).

usage: objcmp.py --ref REFROOT [-j JOBS] [--units] [SRC.cpp ...]

Compiles every candidate source (default: all `src` files of all unit files, plus the harness) in this
tree and in REFROOT (a checkout or `git archive` export of the reference commit) with
exactly the flags verify.py uses, and compares the object files' code and data. Sources with a `/* WWHD
glue` block are run through autoglue.py first (in each tree with that tree's autoglue, when it has one).

A source whose object is identical in both trees cannot behave differently: it needs no re-verification
for a change that only touched headers. Every source that differs (or fails to build) is listed, and with
--units the units that compile it are printed: those are the units to re-verify.

Compared: the __TEXT/__DATA section contents and the relocations, via `otool -tvV`-independent bytes:
each object's sections are dumped with `objdump --full-contents --reloc` and symbol names of local
(non-exported) labels are ignored, so line-number-derived static names (VERIFY's wwhd_verify_<line>)
do not count as a difference.

Renamed sources (tools/verify/readable_renames.tsv, old -> new): the new file in this tree is compared
with the old file in REFROOT, compiled under the new file's name (static-initialiser symbols are named
after the file).
"""
import argparse
import concurrent.futures
import hashlib
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
FP = ["-ffp-contract=off", "-fno-fast-math", "-fno-strict-aliasing", "-fwrapv"]
INC = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include"]


def unit_sources():
    srcs, users = [], {}
    ud = os.path.join(HERE, "units")
    for dp, _, fs in os.walk(ud):
        for f in fs:
            if not f.endswith(".txt"):
                continue
            unit = os.path.relpath(os.path.join(dp, f), ud)[:-4]
            for line in open(os.path.join(dp, f)):
                w = line.split("#")[0].split()
                if w and w[0] == "src":
                    for s in w[1:]:
                        users.setdefault(s, set()).add(unit)
                        if s not in srcs:
                            srcs.append(s)
    return srcs, users


def glued(root, src, tmp, name=None):
    """the file to compile: src itself, or its autoglued text (in tmp); name: compile under this file name"""
    text = open(os.path.join(root, src)).read()
    if "/* WWHD glue" not in text:
        if not name or os.path.basename(name) == os.path.basename(src):
            return os.path.join(root, src)
        d = os.path.join(tmp, "ren_" + hashlib.sha1((root + src).encode()).hexdigest()[:12])
        os.makedirs(d, exist_ok=True)
        out = os.path.join(d, os.path.basename(name))
        open(out, "w").write(text)
        return out
    ag = os.path.join(root, "tools", "verify", "autoglue.py")
    if not os.path.exists(ag):
        return os.path.join(root, src)
    # same file name in both trees: clang names static-initialiser symbols after the file
    d = os.path.join(tmp, "glue_" + hashlib.sha1(root.encode()).hexdigest()[:12])
    os.makedirs(d, exist_ok=True)
    out = os.path.join(d, os.path.basename(name or src))
    code = ("import sys; sys.path.insert(0, %r); import autoglue; "
            "open(%r, 'w').write(autoglue.transform(open(%r).read(), %r))") % (os.path.dirname(ag), out, os.path.join(root, src), src)
    r = subprocess.run([sys.executable, "-c", code], cwd=root, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError("autoglue failed in %s: %s" % (root, r.stderr.strip()[-300:]))
    return out


LOCAL = re.compile(r"<(l_|ltmp|L)[^>]*>|_?wwhd_verify_\d+\w*")


def digest(root, src, tmp, harness=None, name=None):
    """sha1 of the object's section contents + relocations (local label names masked), or an error string.
    harness: compile against this tree's tools/verify/include (gabi.h) instead of root's own;
    name: compile under this file name (renamed sources)"""
    path = glued(root, src, tmp, name)
    lang = ["clang", "-std=c11", "-w"] if src.endswith(".c") else ["clang++", "-std=c++20", "-w"]
    obj = os.path.join(tmp, hashlib.sha1((root + src).encode()).hexdigest()[:16] + ".o")
    inc = (["-I" + os.path.join(harness, "tools", "verify", "include")] if harness else []) + INC
    # a renamed raw source compiled as a copy still finds the "..." includes of its own directory
    quote = ["-iquote", os.path.dirname(os.path.join(root, src))] if os.path.basename(os.path.dirname(path)).startswith("ren_") else []
    r = subprocess.run(lang + ["-O2"] + FP + quote + inc + ["-c", path, "-o", obj], cwd=root, capture_output=True, text=True)
    if r.returncode:
        return None, r.stderr.strip().splitlines()[:3]
    d = subprocess.run(["objdump", "--full-contents", "--reloc", "--syms", obj], capture_output=True, text=True).stdout
    os.remove(obj)
    if path.startswith(tmp):
        os.remove(path)
    lines = [LOCAL.sub("<local>", l) for l in d.splitlines()[2:]]  # skip the file-name header
    # symbol table: keep only external symbols (local statics are named after source lines)
    keep = []
    in_syms = False
    for l in lines:
        if l.startswith("SYMBOL TABLE"):
            in_syms = True
            continue
        if in_syms and l.strip() and " l " in l[:40]:
            continue
        keep.append(l)
    return hashlib.sha1("\n".join(keep).encode()).hexdigest(), None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ref", required=True, help="reference tree (e.g. a `git archive REF` export)")
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--units", action="store_true", help="print the units that compile a differing source")
    ap.add_argument("--same-harness", action="store_true",
                    help="compile the reference against THIS tree's gabi.h too: isolates wwhd_src header changes from harness API changes")
    ap.add_argument("srcs", nargs="*")
    a = ap.parse_args()
    all_srcs, users = unit_sources()
    srcs = a.srcs or all_srcs + ["tools/verify/src/harness.cpp"]
    tmp = tempfile.mkdtemp(prefix="objcmp-")
    same, diff, bad = [], [], []
    sys.path.insert(0, HERE)
    from readable_check import load_renames
    renamed_from = {new: old for old, new in load_renames()[0].items()}

    def one(s):
        rs = s
        if not os.path.exists(os.path.join(a.ref, s)) and s in renamed_from and os.path.exists(os.path.join(a.ref, renamed_from[s])):
            rs = renamed_from[s]
        if not os.path.exists(os.path.join(a.ref, rs)):
            return s, "new", None
        try:
            mine, e1 = digest(ROOT, s, tmp)
        except RuntimeError as e:  # autoglue failure: a build failure of this source, not a crash of the run
            mine, e1 = None, str(e).splitlines()[-3:]
        try:
            ref, e2 = digest(os.path.abspath(a.ref), rs, tmp, ROOT if a.same_harness else None, s if rs != s else None)
        except RuntimeError as e:
            ref, e2 = None, str(e).splitlines()[-3:]
        if e1:
            return s, "BUILD FAILED (this tree)", e1
        if e2:
            return s, "BUILD FAILED (reference)", e2
        return s, "identical" if mine == ref else "DIFFERENT", None

    with concurrent.futures.ThreadPoolExecutor(a.j) as ex:
        for s, state, err in ex.map(one, srcs):
            (same if state == "identical" else diff if state in ("DIFFERENT", "new") else bad).append(s)
            if s in renamed_from:
                print("%-60s renamed from %s: %s" % (s, renamed_from[s], state), flush=True)
            elif state != "identical":
                print("%-60s %s" % (s, state), flush=True)
                for e in err or []:
                    print("    " + e)
    import shutil
    shutil.rmtree(tmp, ignore_errors=True)
    print("objcmp: %d identical, %d different/new, %d failed to build (of %d)" % (len(same), len(diff), len(bad), len(srcs)))
    if a.units:
        us = sorted(set(u for s in diff + bad for u in users.get(s, ())) |
                    (set(u for us_ in users.values() for u in us_) if "tools/verify/src/harness.cpp" in diff else set()))
        print("units to re-verify (%d): %s" % (len(us), " ".join(us)))
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
