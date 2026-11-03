#!/usr/bin/env python3
"""header_args_test.py: a `# verify: -only ADDR,... [-calleesp]` header line is read by verify.header_args (verify.py
applies it when the command line gives no -only; mutate.py takes it as its --only scope). A `# readable-scope:
ADDR,...` comment counts as the -only when no `# verify:` line gives one."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import verify
d = os.path.join(verify.HERE, "units")
name = "_header_args_selftest"
p = os.path.join(d, name + ".txt")
cases = [
    ("# verify: -only 02800000,02800010 -calleesp\n# Run with -only 0BADBAD0 (prose, ignored)\nsrc x.cpp\n# verify: -foo\n",
     ["-only", "02800000,02800010", "-calleesp", "-foo"]),
    ("# readable-scope: 02800000, 02800010\nsrc x.cpp\n", ["-only", "02800000,02800010"]),
    ("# readable-scope: 02800000\n# verify: -strictnan\nsrc x.cpp\n", ["-strictnan", "-only", "02800000"]),
    ("# readable-scope: 0BADBAD0\n# verify: -only 02800000 -calleesp\nsrc x.cpp\n", ["-only", "02800000", "-calleesp"]),
    ("# readable-scope: see notes\nsrc x.cpp\n", []),
]
fails = []
for text, want in cases:
    open(p, "w").write(text)
    try:
        got = verify.header_args(name)
    finally:
        os.remove(p)
    if got != want:
        fails.append("%r -> %r (want %r)" % (text.splitlines()[0], got, want))
for f in fails:
    print("FAIL", f)
print("header_args_test: %s" % ("FAIL" if fails else "PASS"))
sys.exit(1 if fails else 0)
