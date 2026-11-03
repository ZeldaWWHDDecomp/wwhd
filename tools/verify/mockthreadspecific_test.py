#!/usr/bin/env python3
"""Self-test of the unit-file `mockthreadspecific SET GET THREAD [CUR]` directive (harness.cpp; J3D-HDREW#1 shared SDK
TLS model,.

The shipping port's HLE (runtime/src/threads.cpp:67, :763-764): OSSetThreadSpecific(key, value) (C000A090) stores r4
big-endian at THREAD+0x57C+4*key, OSGetThreadSpecific(key) (C0009CC8, stub 028FDF40) returns that word, and
OSGetCurrentThread (C0009BC8) returns THREAD, at top level and nested (inside `real` callees and setups).

Carrier: a unit with the merged sources JKRHeap_context.cpp (becomeCurrentHeap 02756170: GET of the module manager's
key, then context->mpCurrentHeap at +0x70 is swapped) and sead Thread.cpp (ThreadManager::attachCurrent 02760680: CUR,
then SET(key, the new main thread)). The manager *101F8B94 = 0x55000000 with key 1 at +0x24, THREAD 0x5A000000 (slot 1
at 5A000580), the attachCurrent allocation pinned to 0x5B000000.
 1. set then get: the setups run the original attachCurrent (nested CUR -> THREAD, nested SET key 1 <- 5B000000 inside
    the `real` setup function) and the original becomeCurrentHeap (nested GET -> 5B000000); the tested 02756170 then
    gets 5B000000 at top level. attachCurrent tested: top-level CUR -> THREAD and SET. Rows ok.
 2. get without set: with a fixture word at the slot the get returns it; with generated memory both sides get the same
    word. Rows ok.
 3. both sides: every top-level get/set is traced once per side with the same value, rows ok (the candidate's calls
    reach the same model).
 4. setupsnapshot: the snapshot is captured, rows equal the live run; -snapcache-write / -snapcache / -snapcache-check
    give the same rows (rc 0).
 5. -refcache-write / -refcache / -refcache-check give the live rows (rc 0).
 6. key 31 aborts the input on both sides with `mockthreadspecific: thread-specific key outside 0..15`.
 7. CUR nested: the setup's nested OSGetCurrentThread returns THREAD (point 1).
 8. Malformed lines exit 2 (missing fields, bad hex, THREAD 0, a second THREAD, one target in two roles, extra token).
 9. Role lists: with GET written `028FDF40,C0009CC8` (the stub first) the import C0009CC8 is modelled exactly as with
    the plain directive (4 gets, same rows). The current image/gen has no caller of the stub itself.
Unit and spec files stay in tools/verify/units/_mockthreadspecific_test/ and a new directory under the verify scratch
(paths printed; never removed). Exit 0 when all pass.
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "_mockthreadspecific_test/tls"
DIRECTIVE = "mockthreadspecific C000A090 C0009CC8,028FDF40 5A000000 C0009BC8"
BASE = ["src wwhd_src/JSystem/JKernel/JKRHeap_context.cpp",
        "src wwhd_src/sead/Thread.cpp",
        "field * 101F8B94 u32 0x55000000",
        "stable 55000024 4",  # the key: no mock epoch clobbers it
        "field * 101F8B4C u32 0,0x55001000",
        "field * 5B000070 u32 0,0x57000000",
        "arg 02756170 r4 0,0x57000000,0x57000100",
        DIRECTIVE,
        "real 02760680",
        "real 02756170",
        "nestedret 0273B050 0x5B000000"]
KEY1 = ["field * 55000024 u32 1"]
SETUPS = ["setup 02756170 02760680 r3=0x55000000 r4=0x6A000000",
          "setup 02756170 02756170 r3=0 r4=0x57000100"]
TRACE = "1000003"


def rows(out):
    return [l for l in out.splitlines() if l[:8] in ("02756170", "02760680")]


def main():
    fails = []
    udir = os.path.join(HERE, "units", "_mockthreadspecific_test")
    os.makedirs(udir, exist_ok=True)
    with open(os.path.join(udir, "tls.txt"), "w") as f:
        f.write("\n".join(BASE + KEY1 + SETUPS) + "\n")
    exe = verify.build(UNIT, jobs=8)
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="mockthreadspecific_test-", dir=base)
    print("mockthreadspecific_test: files in %s and %s (not removed)" % (W, udir))

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        with open(p, "w") as f:
            f.write("\n".join(lines) + "\n")
        return p

    def run(sp, args, n="200", only="02756170"):
        r = subprocess.run([exe, "-spec", sp, "-noprogress", "-n", n, "-seed", "1", "-only", only] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr

    full = spec("full", BASE + KEY1 + SETUPS)
    # 1, 3, 7: set then get, nested and top level, both sides
    rc, out, err = run(full, ["-trace", TRACE], only="02756170,02760680")
    want = ["nested call C0009BC8", "mockthreadspecific C0009BC8 current thread -> 5A000000",
            "mockthreadspecific C000A090 set key 1 <- 5B000000 (at 5A000580)",
            "nested call C0009CC8 r3=00000001", "mockthreadspecific C0009CC8 get key 1 -> 5B000000 (at 5A000580)"]
    for w in want:
        if w not in out:
            fails.append("1: trace lacks %r" % w)
    lines = out.splitlines()
    try:
        i = next(k for k, l in enumerate(lines) if l.startswith("02756170"))
        # per side: 2 setups (nested set, nested get) + the function's top-level get; the last get of each side is top level
        top_gets = [l for l in lines[:i] if "C0009CC8 get key 1 -> 5B000000" in l]
        if len(top_gets) != 4:
            fails.append("1/3: want 4 gets of 5B000000 (nested + top level, both sides), got %d" % len(top_gets))
        j = next(k for k, l in enumerate(lines) if l.startswith("02760680"))
        cur = [l for l in lines[i + 1:j] if "current thread -> 5A000000" in l and "nested" not in l]
        sets = [l for l in lines[i + 1:j] if "C000A090 set key 1 <-" in l]
        if len(cur) < 2 or len(sets) < 2 or len({s.split("<-")[1] for s in sets[-2:]}) != 1:
            fails.append("1/3: attachCurrent tested: want top-level CUR and SET on both sides with the same value: %s" % (cur + sets))
    except StopIteration:
        fails.append("1: rows missing: %s" % out[-400:])
    for r in rows(out):
        if not r.rstrip().endswith("ok") and "ok (some inputs aborted on both sides)" not in r:
            fails.append("1: row not ok: %s" % r)
    live = rows(run(full, [])[1])
    if len(live) != 1 or not live[0].rstrip().endswith("ok"):
        fails.append("1: n200 live row not ok: %s" % live)

    # 2: get without set (fixture word, then generated memory)
    fx = spec("fixture", BASE + KEY1 + ["field * 5A000580 u32 0x5B000100"])
    rc, out, err = run(fx, ["-trace", TRACE])
    if out.count("mockthreadspecific C0009CC8 get key 1 -> 5B000100 (at 5A000580)") != 2:
        fails.append("2: fixture word not returned on both sides")
    if any(not r.rstrip().endswith("ok") for r in rows(out)):
        fails.append("2: fixture row not ok: %s" % rows(out))
    gen = spec("generated", BASE + KEY1)
    rc, out, err = run(gen, ["-trace", TRACE])
    gets = [l.split("->")[1].strip() for l in out.splitlines() if "C0009CC8 get key 1 ->" in l]
    if len(gets) != 2 or gets[0] != gets[1]:
        fails.append("2: generated word differs between the sides or missing: %s" % gets)
    if any(not r.rstrip().endswith("ok") for r in rows(run(gen, [])[1])):
        fails.append("2: generated row not ok")

    # 4: setupsnapshot + snapcache
    snap = spec("snap", BASE + KEY1 + SETUPS + ["setupsnapshot"])
    rc, out, err = run(snap, ["-n", "2000", "-v"])
    if "setupsnapshot: 02756170 captured" not in err or "refused" in err:
        fails.append("4: snapshot not captured: %s" % [l for l in err.splitlines() if "setupsnapshot" in l][:3])
    snap_live = rows(out)
    if rows(run(full, ["-n", "2000"])[1]) != snap_live:
        fails.append("4: setupsnapshot rows differ from the live setups")
    cache = os.path.join(W, "snapcache")
    os.makedirs(cache, exist_ok=True)
    K = ["-snapcache-key", "tls"]
    for tag, args in (("write", ["-snapcache-write", cache]), ("read", ["-snapcache", cache]),
                      ("check", ["-snapcache", cache, "-snapcache-check"])):
        rc, out, err = run(snap, ["-n", "2000"] + K + args)
        if rc != 0 or rows(out) != snap_live:
            fails.append("4: snapcache %s: rc %d rows %s" % (tag, rc, rows(out)))

    # 5: refcache
    rcache = os.path.join(W, "refcache")
    os.makedirs(rcache, exist_ok=True)
    RK = ["-refcache-key", "t" * 32]
    ref_live = rows(run(full, [])[1])
    for tag, args in (("write", ["-refcache-write", rcache]), ("read", ["-refcache", rcache]),
                      ("check", ["-refcache", rcache, "-refcache-check"])):
        rc, out, err = run(full, RK + args)
        if rc != 0 or rows(out) != ref_live:
            fails.append("5: refcache %s: rc %d rows %s" % (tag, rc, rows(out)))
    if not os.listdir(rcache):
        fails.append("5: no refcache record written")

    # 6: key 31 aborts on both sides
    k31 = spec("key31", BASE + ["field * 55000024 u32 31"])
    rc, out, err = run(k31, ["-trace", TRACE])
    if "mockthreadspecific: thread-specific key outside 0..15" not in out + err:
        fails.append("6: key 31 not rejected with the documented reason")
    r6 = rows(run(k31, [])[1])
    if len(r6) != 1 or " 0/0 " not in r6[0] or "aborted on both sides" not in r6[0]:
        fails.append("6: key 31: want every input aborted on both sides: %s" % r6)

    # 9: every target of a role list gets the role. The current image and gen have no caller of the stub 028FDF40 (the
    # generated original calls the import slot C0009CC8), so the stub cannot be called here; listing the import second,
    # after the stub, shows that a later list entry is modelled exactly like the first one.
    alias = spec("alias", [l for l in BASE if not l.startswith("mockthreadspecific")] + KEY1 + SETUPS +
                 ["mockthreadspecific C000A090 028FDF40,C0009CC8 5A000000 C0009BC8"])
    rc, out, err = run(alias, ["-trace", TRACE])
    if out.count("mockthreadspecific C0009CC8 get key 1 -> 5B000000 (at 5A000580)") != 4:
        fails.append("9: GET listed second in its role list not modelled like the first entry")
    if rows(run(alias, [])[1]) != live:
        fails.append("9: alias rows differ from the plain directive's rows")

    # 8: malformed lines
    for bad in ("mockthreadspecific C000A090 C0009CC8",
                "mockthreadspecific C000A090 C0009CC8 5A00000G",
                "mockthreadspecific C000A090 C0009CC8 0",
                "mockthreadspecific C000A090 C0009CC8 5A000000 C0009BC8 extra",
                "mockthreadspecific C000A090 C000A090 5A000000",
                "mockthreadspecific C000A090 C0009CC8,zz 5A000000",
                DIRECTIVE + "\nmockthreadspecific C000A090 C0009CC8 5A001000"):
        rc, out, err = run(spec("bad", [l for l in BASE if not l.startswith("mockthreadspecific")] + KEY1 + bad.split("\n")),
                           [], n="1")
        if rc != 2 or "invalid mockthreadspecific directive" not in err:
            fails.append("8: not rejected (rc %d): %s" % (rc, bad.replace("\n", " | ")))

    for f in fails:
        print("FAIL", f)
    print("mockthreadspecific_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
