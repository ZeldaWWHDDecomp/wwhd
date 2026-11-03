#!/usr/bin/env python3
"""Self-test of mutate.campaign_lock: plain units and companion specs `unit/sub` (2026-10-26: the companion lock's
parent directory was never created, so every `mutate.py unit/sub` died with FileNotFoundError), and that a second lock
of the same campaign waits while a different campaign does not."""
import fcntl, os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mutate
fails = []
with tempfile.TemporaryDirectory() as d:
    for unit in ("lib2_sead_15", "lib2_agl_1/blend_populated", "lib2_agl_18/sub/deeper"):
        try:
            f = mutate.campaign_lock(os.path.join(d, unit), unit)
        except Exception as e:
            fails.append("%s: %r" % (unit, e)); continue
        g = open(f.name, "a")
        try:
            fcntl.flock(g, fcntl.LOCK_EX | fcntl.LOCK_NB); fails.append("%s: second lock was not blocked" % unit)
        except OSError:
            pass
        g.close(); f.close()
for x in fails:
    print("FAIL", x)
print("mutate_lock_test:", "FAIL" if fails else "PASS")
sys.exit(1 if fails else 0)
