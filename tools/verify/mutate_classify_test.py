#!/usr/bin/env python3
"""mutate_classify_test.py: mutate.classify separates an output difference from an abort, a host signal and an
invalid run, so a nonzero mutant run is never counted as an observed difference by default."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mutate

ok = "02800000  f                                         10000/10000        0/0      1/1  ok\n"
diff = "02800004  g                                          9999/10000        0/0      1/1  input 7: r3 0 vs 1\n"
abort = "02800008  h                                          9999/10000        0/0      1/1  only one side aborted: access budget exceeded\n"
cases = [
    (1, ok + diff, "difference"),
    (1, ok + abort, "abort"),
    (1, ok + abort + diff, "difference"),
    (-11, ok, "signal"),
    (1, ok + "verify: host signal 11 while running candidate side of 02800000 (g_cur_func); input aborted\n", "signal"),
    (2, "invalid mockfield directive: x\n", "invalid"),
    (1, ok, "invalid"),
]
bad = [(rc, want, mutate.classify(rc, out)) for rc, out, want in cases if mutate.classify(rc, out) != want]
for rc, want, got in bad:
    print("FAIL rc=%d: want %s, got %s" % (rc, want, got))
print("mutate_classify_test: %s" % ("FAIL" if bad else "PASS"))
sys.exit(1 if bad else 0)
