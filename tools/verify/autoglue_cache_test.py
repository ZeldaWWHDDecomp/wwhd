#!/usr/bin/env python3
"""Self-test of autoglue's memoised lexing (2026-10-26, mutfast lever 0).

autoglue.transform masks every included header again for each namespace lookup; mutate.py calls transform once per
mutant, so on large sources (46 KB SEAD, 44 KB NW-SND) this was ~2.3 s CPU per mutant. mask() and the per-header
namespace scan are memoised (pure functions of the text), and include_closure per include lines + source directory
(a hit is used only while every header's mtime and size are unchanged). Checks: for three lib2 sources and mutants of them,
transform without the caches (the old behaviour) equals transform with warm caches, and the cached transform is at
least 3x faster; a cache that reaches its size limit is cleared and still gives the same result; an edited header
is read again (include_closure).
"""
import os
import random
import shutil
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402
import mutate  # noqa: E402

SRCS = ["wwhd_src/sead/ScreenInlineInstances.cpp", "wwhd_src/nw/snd/ArchiveLoading.cpp", "wwhd_src/agl/TimingHistory.cpp"]


def uncached(text, src):
    """transform as before the memo: every lookup computed again"""
    keep, inc = autoglue._cached, autoglue.include_closure
    autoglue._cached = lambda cache, key, fn: fn(key)
    autoglue.include_closure = autoglue._include_closure
    try:
        return autoglue.transform(text, src)
    finally:
        autoglue._cached, autoglue.include_closure = keep, inc


def main():
    os.chdir(ROOT)
    fails = []
    tunc = tcached = 0.0
    for src in SRCS:
        text = open(src).read()
        variants = [text]
        for _, a, b in mutate.bodies(text)[:4]:
            ms = mutate.mutants(text, a, b)
            for m in random.Random(7).sample(ms, min(3, len(ms))):
                variants.append(text[:m[1]] + m[3] + text[m[2]:])
        autoglue.transform(text, src)  # warm
        for k, v in enumerate(variants):
            t0 = time.process_time()
            try:
                c = uncached(v, src)
            except autoglue.GlueError as e:
                c = "GlueError " + str(e)
            t1 = time.process_time()
            try:
                w = autoglue.transform(v, src)
            except autoglue.GlueError as e:
                w = "GlueError " + str(e)
            t2 = time.process_time()
            tunc += t1 - t0
            tcached += t2 - t1
            if c != w:
                fails.append("%s variant %d: cached transform differs from the uncached one" % (src, k))
    # size limit: a full cache is cleared, results unchanged
    for i in range(4096 - len(autoglue._MASK_CACHE) + 1):
        autoglue.mask("// filler %d\n" % i)
    if len(autoglue._MASK_CACHE) > 4096:
        fails.append("mask cache exceeds its limit (%d)" % len(autoglue._MASK_CACHE))
    if autoglue.mask("a /* b */ \"c\"") != "a         \"x\"":
        fails.append("mask after a cache clear gives %r" % autoglue.mask("a /* b */ \"c\""))
    # an edited header is read again
    tmp = os.path.join(ROOT, "build", "verify", "_autoglue_cache_test")
    os.makedirs(tmp, exist_ok=True)
    try:
        hdr = os.path.join(tmp, "cache_probe.h")
        open(hdr, "w").write("// one\n")
        src = os.path.join(os.path.relpath(tmp, ROOT), "probe.cpp")
        first = autoglue.include_closure('#include "cache_probe.h"\n', src)
        open(hdr, "w").write("// two, longer\n")
        second = autoglue.include_closure('#include "cache_probe.h"\n', src)
        if [t for _, t in first] != ["// one\n"] or [t for _, t in second] != ["// two, longer\n"]:
            fails.append("include_closure after a header edit: %r then %r" % (first, second))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    if tcached * 3 > tunc:
        fails.append("cached transform not 3x faster (%.2f s uncached vs %.2f s cached)" % (tunc, tcached))
    print("transform CPU: uncached %.2f s, cached %.2f s" % (tunc, tcached))
    for f in fails:
        print("FAIL", f)
    print("autoglue_cache_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
