#!/usr/bin/env python3
"""Self-test of the harness option `-nulltrap` / unit directive `nulltrap` (2026-10-26, AGL#13): a guest access to page 0
(EA 0x0000..0x0FFF) is a console DSI; with the option it ends that side's run as a trap event, so a mutant-only NULL
dereference is an ordinary difference (first difference `trap: ...`), never a budget abort.

Uses lib_js_base (JKRFileLoaderList__dt 027EC058: `if (list == NULL) return;` first; JKRDvdFile::close 027EACBC: reads
this->mIsAvailable without a NULL check) and lib2_sead_15 (real 02759928 clears the word at r3, as in watch_test.py):
1. 027EC058 with r3 in {0, ptr}: the unmodified source passes with -nulltrap;
2. a candidate that reads list+4 before the NULL check passes without the option (old result) and differs with
   `-nulltrap` and with the unit directive `nulltrap` (`trap: page-0 DSI by the candidate only (load 00000004`);
3. a candidate that walks `at(0) .. at(16)` before the check: without the option the host walk never meets its end and
   an access budget aborts it; with the option it traps at the first access (EA 00000000) and the run is quick;
4. 027EACBC with this = NULL: both sides fault alike -> ok, and the original's DSI is reported on stderr (a fixture defect
   is never hidden);
4b. both sides fault at the same access (027EACBC, `footprint 027EACBC 0x40`, this = NULL): a candidate that first stores
   into its own frame (entry SP - 0x10) still compares ok (the dying frames are not observable, AGL#4; the original's
   prologue stores the LR save word at entry SP + 4 before it faults, the candidate only at its first call); one that first
   reads mIsAvailable+1 faults at a different access: ok (no calls before either fault) and counted on stderr; one that
   first stores to the caller's frame (entry SP + 8) or to the heap differs (`memory ...`);
4c. faults at different accesses after a call (JKRFileLoader::getGlbResource 027EBF08, `ret! 027EBE3C 0x10`: the original
   calls findVolume, then faults reading the volume's vtable): a candidate that reads volume+1 first passes (same call
   prefix, counted); one that also passes a different argument to findVolume differs (`call #0 ... (before the page-0
   fault; ...)`);
5. a setup that touches page 0 exits 2 with `setup 02759928 touched page 0`;
6. `nulltrap extra` is rejected (exit 2).
Writes a temporary unit directory and mutant sources next to the originals and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_nulltrap_test")
F, CLOSE = "027EC058", "027EACBC"
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
DVD = "wwhd_src/JSystem/JKernel/JKRDvdFile.cpp"
DVDMUT = "wwhd_src/JSystem/JKernel/_nulltrap_test_JKRDvdFile.cpp"
CLOSE_HEAD = "    if ((u8)mIsAvailable == 0) return;\n    if (JKRDvdCloseInfo"
MUT = "wwhd_src/JSystem/JKernel/_nulltrap_test_JKRFileLoader.cpp"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
SEAD = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
NULLARG = "arg! %s r3 0,0x41000000" % F
CHECK = "    if (list == NULL) return;\n"


def run(name, lines, func=F, extra=(), src=SRC, n="200", spec=BASE, orig=SRC):
    body = [("src " + src) if l.strip() == "src " + orig else l for l in spec]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    t = time.time()
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_nulltrap_test/" + name, "-only", func, "-n", n,
                        "-buildj", "4"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(func)]
    return r.returncode, (row[0] if row else r.stdout[-400:]), r.stderr, time.time() - t


def mutant(new):
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(CHECK) != 1:
        raise SystemExit("mutation site not found once in %s" % SRC)
    open(os.path.join(ROOT, MUT), "w").write(text.replace(CHECK, new + CHECK))


def dvd_mutant(new):
    text = open(os.path.join(ROOT, DVD)).read()
    if text.count(CLOSE_HEAD) != 1:
        raise SystemExit("mutation site not found once in %s" % DVD)
    open(os.path.join(ROOT, DVDMUT), "w").write(text.replace(CLOSE_HEAD, new + CLOSE_HEAD))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        # 1. the original checks NULL first: never traps
        rc, row, err, _ = run("plain", [NULLARG], extra=["-nulltrap"])
        if rc or "200/200" not in row or "nulltrap:" in err:
            fails.append("unmodified source with -nulltrap: %s %s" % (row, err[-300:]))
        # 2. read before the NULL check
        mutant("    u32 head = ((const gabi::be<u32>*)list)[1];\n    (void)head;\n")
        rc, row, _, _ = run("read_off", [NULLARG], src=MUT)
        if rc or "200/200" not in row:
            fails.append("read-first candidate without -nulltrap should keep the old result (pass): %s" % row)
        for how, lines, extra in (("flag", [NULLARG], ["-nulltrap"]), ("directive", [NULLARG, "nulltrap"], [])):
            rc, row, _, _ = run("read_" + how, lines, extra=extra, src=MUT)
            if rc == 0 or "trap: page-0 DSI by the candidate only (load 00000004, depth 0)" not in row:
                fails.append("read-first candidate with %s: want a trap difference, got %s" % (how, row))
        # 3. a walk from at(0) to at(16): host nullptr .. PPC_MEM_BASE+16 never meet
        mutant("    for (const gabi::be<u32>* p = gabi::at<const gabi::be<u32>>(0); p != gabi::at<const gabi::be<u32>>(16); ++p)\n"
               "        (void)(u32)*p;\n")
        rc, row, _, _ = run("walk_off", [NULLARG], src=MUT, n="2")
        if rc == 0 or "only one side aborted" not in row:
            fails.append("walk candidate without -nulltrap: want the old budget abort, got %s" % row)
        rc, row, _, dt = run("walk_on", [NULLARG], extra=["-nulltrap"], src=MUT)
        if rc == 0 or "trap: page-0 DSI by the candidate only (load 00000000, depth 0)" not in row:
            fails.append("walk candidate with -nulltrap: want a trap at the first access, got %s" % row)
        # 4. both sides fault alike: ok, and the original's DSI is reported
        rc, row, err, _ = run("both", ["arg! %s r3 0" % CLOSE], func=CLOSE, extra=["-nulltrap"])
        if rc or "200/200" not in row:
            fails.append("identical DSI on both sides should compare equal: %s" % row)
        if "nulltrap: %s: the original took a page-0 DSI on 200 input(s); first: load 00000" % CLOSE not in err:
            fails.append("the original's DSI is not reported: %s" % err[-400:])
        # 4b. both sides fault: same access with different own-frame stores -> ok; different access or caller/heap store -> difference
        both = ["arg! %s r3 0" % CLOSE, "footprint %s 0x40" % CLOSE]
        cases = (
            ("frame", "    gabi::store<u32>(gabi::cpu->r[1] - 0x10, 0x12345678u);\n", None),
            ("other_ea", "    (void)(u8)((const gabi::be<u8>*)this)[0x11];\n", None),
            ("caller", "    gabi::store<u32>(gabi::cpu->r[1] + 8, 0x12345678u);\n", "memory "),
            ("heap", "    gabi::store<u8>(0x41000000u, 0x5A);\n", "memory 41000000"),
        )
        for name, code, want in cases:
            dvd_mutant(code)
            rc, row, err, _ = run("both_" + name, both, func=CLOSE, extra=["-nulltrap"], src=DVDMUT, orig=DVD)
            if want is None and (rc or "200/200" not in row):
                fails.append("both fault, %s: want ok, got %s %s" % (name, row, err[-300:]))
            if name == "other_ea" and "nulltrap: %s: 200 input(s) ended in page-0 faults on both sides at different accesses" % CLOSE not in err:
                fails.append("different-access faults are not counted: %s" % err[-400:])
            if want is not None and (rc == 0 or want not in row):
                fails.append("both fault, %s: want %r, got %s %s" % (name, want, row, err[-300:]))
        # 4c. different accesses after a call: the common call prefix compares
        glb = ["ret! 027EBE3C 0x10"]
        for name, old, new, want in (
                ("prefix_ok", "    if (volume == NULL) return NULL;\n    return gabi::call_ptr<void*>(volume->mVtable->mGetResource",
                 "    if (volume == NULL) return NULL;\n    (void)(u8)((const gabi::be<u8>*)volume)[1];\n"
                 "    return gabi::call_ptr<void*>(volume->mVtable->mGetResource", None),
                ("prefix_diff", "    JKRFileLoader* volume = findVolume(remaining.get());\n    if (volume == NULL) return NULL;\n",
                 "    JKRFileLoader* volume = findVolume(remaining.get() + 1);\n    if (volume == NULL) return NULL;\n"
                 "    (void)(u8)((const gabi::be<u8>*)volume)[1];\n", "(before the page-0 fault; the sides faulted at different accesses")):
            text = open(os.path.join(ROOT, SRC)).read()
            if text.count(old) != 1:
                raise SystemExit("mutation site not found once in %s: %r" % (SRC, old))
            open(os.path.join(ROOT, MUT), "w").write(text.replace(old, new))
            rc, row, err, _ = run("glb_" + name, glb, func="027EBF08", extra=["-nulltrap"], src=MUT)
            if want is None and (rc or "200/200" not in row or "nulltrap: 027EBF08: 200 input(s) ended in page-0 faults" not in err):
                fails.append("different accesses after the same call: want ok and counted, got %s %s" % (row, err[-400:]))
            if want is not None and (rc == 0 or want not in row or "call #0" not in row):
                fails.append("different call before different-access faults: want a call #0 difference, got %s" % row)
        # control: the same own-frame store is observable when nothing faults (footprint compares the frame)
        dvd_mutant(cases[0][1])
        rc, row, _, _ = run("frame_nonull", ["arg! %s r3 0x41000000" % CLOSE, "footprint %s 0x40" % CLOSE], func=CLOSE,
                            extra=["-nulltrap"], src=DVDMUT, orig=DVD)
        if rc == 0 or "memory 7" not in row:
            fails.append("control: the own-frame store should differ without a fault (test lost its point): %s" % row)
        # 5. a setup that touches page 0
        rc, row, err, _ = run("setup", ["setup 02759564 02759928 r3=0"], func="02759564", extra=["-nulltrap"], n="1", spec=SEAD)
        if rc != 2 or "nulltrap: setup 02759928 touched page 0 (store 0000000" not in err:
            fails.append("a setup touching page 0 must exit 2 with a clear error (rc %d): %s" % (rc, err[-400:]))
        # 6. malformed directive
        rc, _, _, _ = run("bad", ["nulltrap extra"], n="1")
        if rc != 2:
            fails.append("`nulltrap extra` not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_nulltrap_test"), ignore_errors=True)
        for m in (MUT, DVDMUT):
            if os.path.exists(os.path.join(ROOT, m)):
                os.remove(os.path.join(ROOT, m))
    for f in fails:
        print("FAIL", f)
    print("nulltrap_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
