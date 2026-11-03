#!/usr/bin/env python3
"""Self-test of the unit-file flag `strictmocks` (harness.cpp g_strictmocks; 2026-11-02;
opt-in): inside a setup or postsetup, a call that gets the generated answer of the generic mock for a
value-returning (or undeclared) import ends the run with exit 2 naming the import, the setup and the call site.
Carrier: units/_axframes_test/carrier_alarm.txt (setups 028A2138 / 028A2240 call OSCreateAlarm, OSSetPeriodicAlarm, ...).
  1. carrier_alarm + strictmocks (`osmem hle threads` answers every import its setups call): rows ok, identical to the
     run without the flag.
  2. `osmem hle` without `threads` + strictmocks: exit 2, `strictmocks: setup 028A2138 calls import C0009A38 OSCreateAlarm
     ... from 028A1A90 (lr 028A1AEC)`.
  3. the same spec without the flag: runs as before (rows ok).
  4. `strictmocks extra` is a spec error (exit 2).
  5. `strictmocks report` (diagnostic only): on the spec of 2 the run does not stop at the first hit: stderr lists every
     (setup, import, caller, lr) once as it is first hit (OSCreateAlarm from 028A1A90 among them, more than one key), the run goes on, and a
     normal exit prints one count per key; on the answered spec of 1 it lists none, with counts at
     exit (`no generated import answer`) and rows as without the flag.
  6. Setups see import SLOTS only (why the check tests `target >= 0xC0000000`): original code reaches every import through an
     imp_ thunk, and mkunit routes each thunk to its slot. In the built unit.c of carrier_alarm every imp_ thunk calls its
     imports.json slot, and no stub address appears: the per-library stub tables (slot = stub + delta) are measured from
     every `bl` to an import in build/gen (the recompiler keeps the bl word in a comment). Fails if mkunit ever routes an
     import call to its stub.
  7. `importvoid SLOT` (mkunit: defr = deff = 0 for that import; without it every import counts as value-returning):
     on the spec of 2, `importvoid C0009A38` moves the stop from OSCreateAlarm to the next import, OSSetAlarmUserData; the report
     no longer lists OSCreateAlarm; the trace shows r3 and f1 as they were at the call; the unit.c table has defr = deff = 0 for it
     and all ones for its neighbour. A non-slot address or a `ret!` on the same import ends mkunit with an error.
Temporary specs units/_axframes_test/_sm_*.txt are removed. Exit 0 when all pass."""
import glob
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UDIR = os.path.join(HERE, "units", "_axframes_test")
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def run(name):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_axframes_test/" + name, "-n", "5", "-noprogress", "-buildj", "8"],
                       cwd=ROOT, capture_output=True, text=True, errors="replace", env=dict(os.environ, WWHD_VERIFY_CACHE="0"))
    return r.returncode, [l for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)], r.stderr


def slots_only():
    gen = os.path.join(ROOT, "build", "gen")
    imps = [x for x in json.load(open(os.path.join(gen, "imports.json"))) if x["kind"] == "f"]
    ident = {"imp_%s_%s" % (re.sub(r"[^A-Za-z0-9_]", "_", x["lib"].replace(".rpl", "")), re.sub(r"[^A-Za-z0-9_]", "_", x["name"])): x
             for x in imps}
    deltas = {}
    pat = re.compile(r"(imp_\w+)\(c\); /\* ([0-9A-F]{8}): ([0-9A-F]{8}) \*/")
    for f in glob.glob(os.path.join(gen, "code_*.c")):
        for m in pat.finditer(open(f).read()):
            x = ident.get(m.group(1))
            ins = int(m.group(3), 16)
            if not x or ins >> 26 != 18:
                continue
            off = ins & 0x03FFFFFC
            off -= 0x04000000 if off & 0x02000000 else 0
            deltas.setdefault(x["lib"], set()).add((x["slot"] - (int(m.group(2), 16) + off)) & 0xFFFFFFFF)
    check(deltas and all(len(d) == 1 for d in deltas.values()), "6 one stub delta per library: %s" %
          {k: sorted("%08X" % d for d in v) for k, v in sorted(deltas.items())})
    stubs = {(x["slot"] - next(iter(deltas[x["lib"]]))) & 0xFFFFFFFF for x in imps if x["lib"] in deltas}
    unit_c = glob.glob(os.path.join(os.environ.get("WWHD_VERIFY_SCRATCH") or os.path.join(ROOT, "build", "verify"), "*",
                                    "_axframes_test", "_sm_plain", "unit.c")) + \
        glob.glob(os.path.join(ROOT, "build", "verify", "_axframes_test", "_sm_plain", "unit.c"))
    if not unit_c:
        check(False, "6 the built unit.c of _sm_plain is missing")
        return
    text = open(max(unit_c, key=os.path.getmtime)).read()
    thunks = re.findall(r"void (imp_\w+)\(Cpu\* c\) \{ vm_call\(c, 0x([0-9A-F]{8})u, VM_CALL_IMPORT\); \}", text)
    bad = [(n, a) for n, a in thunks if n not in ident or ident[n]["slot"] != int(a, 16)]
    check(thunks and not bad, "6 all %d imp_ thunks of unit.c call their imports.json slot%s" % (len(thunks), (": %s" % bad[:3]) if bad else ""))
    consts = {int(a, 16) for a in re.findall(r"0x([0-9A-F]{8})u", text)}
    hit = sorted(consts & stubs)
    check(not hit, "6 no stub address (%d known) appears in unit.c%s" % (len(stubs), (": %s" % ["%08X" % a for a in hit[:5]]) if hit else ""))


def main():
    base = open(os.path.join(UDIR, "carrier_alarm.txt")).read()
    noth = base.replace("\nosmem hle threads threadrate=0\n", "\nosmem hle\n")
    specs = {"_sm_plain": base, "_sm_a": base + "strictmocks\n", "_sm_b": noth + "strictmocks\n", "_sm_c": noth,
             "_sm_d": base + "strictmocks extra\n", "_sm_e": noth + "strictmocks report\n",
             "_sm_g": base + "strictmocks report\n",
             "_sm_h": noth + "importvoid C0009A38\nstrictmocks\n", "_sm_i": noth + "importvoid C0009A38\nstrictmocks report\n",
             "_sm_j": noth + "importvoid 028A2138\n", "_sm_k": noth + "importvoid C0009A38\nret! C0009A38 1\n",
             "_sm_l": noth + "importvoid C0009A38\n"}
    if noth == base:
        raise SystemExit("carrier_alarm.txt no longer has the line `osmem hle threads threadrate=0`")
    try:
        for n, t in specs.items():
            open(os.path.join(UDIR, n + ".txt"), "w").write(t)
        rc0, rows0, _ = run("_sm_plain")
        rc, rows, err = run("_sm_a")
        check(rc == 0 and rows and rows == rows0 and all(r.endswith(" ok") for r in rows), "1 answered setups pass with strictmocks, rows as without")
        rc, rows, err = run("_sm_b")
        msg = next((l for l in err.splitlines() if l.startswith("strictmocks:")), "")
        check(rc == 2 and not rows and "setup 028A2138 calls import C0009A38 OSCreateAlarm" in msg and "from 028A1A90 (lr 028A1AEC)" in msg,
              "2 a generated import answer in a setup ends the run: %s" % (msg or err.strip().splitlines()[-1:]))
        rc, rows, err = run("_sm_c")
        check(rc == 0 and rows and all(r.endswith(" ok") for r in rows) and "strictmocks" not in err, "3 off by default: the same spec runs")
        rc, rows, err = run("_sm_d")
        check(rc == 2 and "invalid strictmocks directive" in err, "4 operands are a spec error")
        rc, rows, err = run("_sm_e")
        first = [l for l in err.splitlines() if l.startswith("strictmocks report: first: ")]
        check(any("calls import C0009A38 OSCreateAlarm from 028A1A90 (lr 028A1AEC)" in l for l in first) and len(set(first)) > 1
              and not any(l.startswith("strictmocks: ") for l in err.splitlines()),
              "5 report mode lists %d key(s) without stopping: %s" % (len(set(first)), first[:2]))
        counts = [l for l in err.splitlines() if re.match(r"strictmocks report: .* \d+ time\(s\)$", l)]
        check(len(counts) == len(set(first)) and any("OSCreateAlarm from 028A1A90 (lr 028A1AEC): " in l for l in counts) and "host signal" not in err,
              "5 a normal exit prints one count per key (%d) and no host signal: %s" % (len(counts), [l for l in err.splitlines() if "host signal" in l][:1]))
        rc, rows, err = run("_sm_g")
        check(rc == 0 and rows == rows0 and "strictmocks report: no generated import answer inside setups" in err
              and "strictmocks report: first: " not in err, "5 report mode on answered setups: nothing listed, rows as without")
        rc, rows, err = run("_sm_h")
        msg = next((l for l in err.splitlines() if l.startswith("strictmocks:")), "")
        check(rc == 2 and "calls import C0009FF0 OSSetAlarmUserData" in msg, "7 importvoid moves the stop to the next import: %s" % (msg or err.strip().splitlines()[-1:]))
        rc, rows, err = run("_sm_i")
        first = [l for l in err.splitlines() if l.startswith("strictmocks report: first: ")]
        check(first and not any("C0009A38" in l for l in first), "7 the report no longer lists it (%d keys)" % len(set(first)))
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_axframes_test/_sm_l", "-n", "1", "-trace", "1000003", "-noprogress", "-buildj", "8"],
                           cwd=ROOT, capture_output=True, text=True, errors="replace", env=dict(os.environ, WWHD_VERIFY_CACHE="0"))
        pairs = re.findall(r"nested call C0009A38 r3=(\w+) .*\n\s+importvoid C0009A38: r3 (\w+) f1", r.stdout)
        check(pairs and all(a == b for a, b in pairs), "7 r3 stays as at the call: %s" % pairs[:2])
        tab = max(glob.glob(os.path.join(os.environ.get("WWHD_VERIFY_SCRATCH") or os.path.join(ROOT, "build", "verify"), "*", "_axframes_test", "_sm_l", "unit.c")) +
                  glob.glob(os.path.join(ROOT, "build", "verify", "_axframes_test", "_sm_l", "unit.c")), key=os.path.getmtime)
        text = open(tab).read()
        row_v = re.search(r'\{0xC0009A38u, "OSCreateAlarm", 0x\w+u, 0x\w+u, (0x\w+u), (0x\w+u),', text)
        row_o = re.search(r'\{0xC0009FF0u, "OSSetAlarmUserData", 0x\w+u, 0x\w+u, (0x\w+u), (0x\w+u),', text)
        check(row_v and row_v.groups() == ("0x0u", "0x0u") and row_o and row_o.groups() == ("0xFFFFFFFFu", "0xFFFFFFFFu"),
              "7 unit.c: defr/deff %s for it, %s for its neighbour" % (row_v and row_v.groups(), row_o and row_o.groups()))
        rc, rows, err = run("_sm_j")
        check(rc != 0 and "importvoid: not a function import slot: 028A2138" in err, "7 a non-slot address is an error")
        rc, rows, err = run("_sm_k")
        check(rc != 0 and "importvoid C0009A38 contradicts the unit line" in err, "7 a stated value on the same import is an error")
    finally:
        for n in specs:
            p = os.path.join(UDIR, n + ".txt")
            if os.path.exists(p):
                os.remove(p)
    slots_only()
    print("strictmocks_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
