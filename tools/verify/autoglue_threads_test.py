#!/usr/bin/env python3
"""Self-test: autoglue.transform from several threads at once (mutate.py -j N) equals the serial transform.

2026-11-01: _spec_siblings published its list of verify-unit sources empty before walking tools/verify/units, so a
mutant transformed by another thread during the walk saw no sibling sources, kept its exported TU-wide views `static`
and failed to link (hd_screen_pictograph_box_resources.cpp: sBoxStateGuard, used by the camera and list resources).
The walk is slowed down here so the threads overlap it every time.
"""
import os
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

SRC = "wwhd_src/d/hd_screen_pictograph_box_resources.cpp"
NAME = "\nbe<u32>& sBoxStateGuard = "  # at a line start: external linkage, not `static be<u32>& ...`


def fresh():
    autoglue._SPEC_SRCS = None
    autoglue._EXPORTED_CACHE.clear()


def main():
    os.chdir(ROOT)
    text = open(SRC).read()
    texts = [text + "\n" * (i + 1) for i in range(6)]  # each thread a slightly different text (as each mutant), so
    # no _EXPORTED_CACHE hit hides the race
    serial = []
    for t in texts:
        fresh()
        serial.append(autoglue.transform(t, SRC))
    if not all(NAME in x for x in serial):
        print("FAIL: serial transform does not export sBoxStateGuard")
        return 1
    real_walk = os.walk

    def slow_walk(*a, **k):
        for x in real_walk(*a, **k):
            time.sleep(0.001)
            yield x
    fails = 0
    for rnd in range(3):
        fresh()
        out = [None] * len(texts)
        autoglue.os.walk = slow_walk
        try:
            ts = [threading.Thread(target=lambda i=i: out.__setitem__(i, autoglue.transform(texts[i], SRC))) for i in range(len(out))]
            for t in ts:
                t.start()
            for t in ts:
                t.join()
        finally:
            autoglue.os.walk = real_walk
        bad = [i for i, o in enumerate(out) if o != serial[i]]
        if bad:
            print("FAIL round %d: threads %s differ from the serial transform (sBoxStateGuard exported: %s)"
                  % (rnd, bad, [NAME in (out[i] or "") for i in bad]))
            fails += 1
    print("autoglue_threads_test: %s" % ("FAIL" if fails else "ok (3 rounds x 6 threads equal the serial transform)"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
