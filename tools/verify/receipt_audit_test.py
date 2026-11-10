#!/usr/bin/env python3
"""Self-test of receipt_audit.py on a planted ledger (a temporary git repo with two versions of one source file).

Rows (function 02000000, file src/a.cpp; v2 is the landed text, it changed an earlier line than the spans):
  1  D    current difference receipt with a clean witness (-nulltrap)                  -> backed by a receipt
  2  EQ   only receipt compiled on v1 (its span sits elsewhere in v1)                   -> receipt `stale` naming v1's blob;
                                                                                           row UNBACKED
  3  D    receipt whose mutantN.cpp matches neither version                             -> receipt `error`
  4  EQ   same function and mutation text as row 40 (another span); its receipt lies in an id-mapped directory (local id
          5 -> global 4); its mutation text carries ` @a.cpp:3` before the ` (helper g)` tail                                                                -> `current` for 4 (not 40)
  40 EQ   no receipt, class_source "capture table x": --ruled-basis "capture table"    -> basis
  6  D    no receipt at all (a planted missing receipt), no basis                       -> UNBACKED
  7  D    the ledger span is stale (points at v1's offset); the receipt was compiled on the landed text at the right
          place                                                                         -> receipt `error` (it is not the
                                                                                           row's landed mutant); UNBACKED
  8  EQ   a current clean difference in the gate                                       -> CONTRADICTED
  10 EQ   a current survivor, and a current difference from the excluded parent spec lib2_parent (--exclude-spec) -> receipt,
          not CONTRADICTED; that receipt is `excluded` with the spec named
  9  EQ   only receipt on v1, where f() is identical to the landed f()                  -> UNBACKED; with --function-pin: pin
          (row 2 too, under --function-pin: its receipt is also on v1, f unchanged)
  11 NAMED_GAP a current clean difference, named by --ruled-difference                  -> backing ruled-difference,
          the difference listed in contradicted_ruled with the reason
  12 RI   a current clean difference, not named                                         -> CONTRADICTED
  --ruled-difference 4 (row 4 has no difference)                                        -> error (a stale exception)
  plus: a receipt in a mapped directory whose local id maps to a row of another mutation -> `error`;
        a receipt below an --exclude directory -> `excluded` with its reason; one with no row -> `unjoined`;
        one without mutantN.cpp -> `no-companion`.
  13 D    its only kill ran on a spec that is not in the tree's tools/verify/units      -> receipt `unlanded-spec`; UNBACKED
  14 D    its receipt ran a unit file under an old name the rename table maps to a landed spec -> current, backed
  15 EQ   a survivor whose `spec` field names a landed companion, and a clean difference whose `spec` names an unlanded
          one                                                                           -> backed; not CONTRADICTED
Expected: exit 1; UNBACKED = {2, 3, 6, 7, 9, 13}, CONTRADICTED = {8, 12};
Object pin (a second git repo, wwhd_src/include/toy.h and wwhd_src/toy/toy.cpp, compiled with this tree's tools): a header
change of struct S that leaves g's text unchanged must not pin g's row; k, which uses nothing that changed, pins. errors = the receipts of 3 and 7 and the bad map; every receipt has exactly one
status in --out-receipts.

`receipt_audit_test.py --old SCRIPT` runs the same fixture through the campaign script integrate_receipts.py (a copy, laid
out as it expects) and checks the same verdicts: it must FAIL (it exits 0, has no stale status, skips with an issue).
Temporary files go to a fresh directory under $TMPDIR. Exit 0 when all pass."""
import csv
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


V1 = "".join("int line%d = %d;\n" % (i, i) for i in range(10)) + "/* 02000000 */\nvoid f() {\n    x = 1;\n    y = 2;\n    z = 3;\n}\n/* WWHD glue */\n"
V2 = V1.replace("int line3 = 3;\n", "int line3 = 33;  // changed\n")
WITNESS = "02000000  f                                              0/1000        0/0      1/1  memory 00001000: original 01, candidate 02  {gen seed=1}"
SURV = "02000000  f                                           1000/1000        0/0      1/1  ok"


def git(repo, *a):
    subprocess.run(["git", "-C", repo, "-c", "user.name=t", "-c", "user.email=t@t"] + list(a), check=True, capture_output=True)


def receipt(d, rid, mutation, cls, text, stdout, nulltrap=True, cpp=True, spec="/b/lib2_scoped/spec.expanded.txt", src="src/a.cpp",
            func="02000000", jspec=None):
    os.makedirs(d, exist_ok=True)
    j = {"id": str(rid), "src": src, "func": func, "mutation": mutation, "class": cls, "seed": "1", "input_seed": 1, "n": 1000,
         "cmd": ["verify", "-spec", spec, "-n", "1000", "-seed", "1"] + (["-nulltrap"] if nulltrap else []), "stdout": stdout}
    if jspec:
        j["spec"] = jspec
    open(os.path.join(d, "mutant%s.json" % rid), "w").write(json.dumps(j))
    if cpp:
        open(os.path.join(d, "mutant%s.cpp" % rid), "w").write(text)


def edit(text, original, replacement, nth=0):
    k = -1
    for _ in range(nth + 1):
        k = text.index(original, k + 1)
    return text[:k] + replacement + text[k + len(original):], k


def build(T):
    repo = os.path.join(T, "repo")
    os.makedirs(os.path.join(repo, "src"))
    git(repo, "init", "-q")
    open(os.path.join(repo, "src", "a.cpp"), "w").write(V1)
    git(repo, "add", ".")
    git(repo, "commit", "-q", "-m", "v1")
    open(os.path.join(repo, "src", "a.cpp"), "w").write(V2)
    git(repo, "commit", "-q", "-am", "v2")
    # the unit specs on the audited tree (receipt_audit checks each receipt's spec landed), and a
    # rename table (old name lib2_oldname -> lib2_scoped)
    for u in ("lib2_scoped", "lib2_parent", "lib2_scoped/comp"):
        os.makedirs(os.path.dirname(os.path.join(repo, "tools", "verify", "units", u)), exist_ok=True)
        open(os.path.join(repo, "tools", "verify", "units", u + ".txt"), "w").write("src src/a.cpp\n")
    os.makedirs(os.path.join(repo, "verify-archive"))
    open(os.path.join(repo, "verify-archive", "spec-renames-x.tsv"), "w").write("# renames\nold\tnew\nlib2_oldname\tlib2_scoped\n")
    rows = []

    def row(gid, cls, original, replacement, mutation, text, nth=0, src="ledger x", span_text=None):
        _, k = edit(span_text or text, original, replacement, nth)
        rows.append({"global_id": str(gid), "function": "02000000", "helper_callers": "", "source": "src/a.cpp", "span_start": k,
                     "span_end": k + len(original), "original": original, "replacement": replacement, "mutation": mutation,
                     "final_class": cls, "class_source": src})
    row(1, "D", "x = 1;", "x = 5;", "literal 1->5", V2)
    row(2, "EQ", "y = 2;", "y = 2+0;", "plus zero", V2)
    row(3, "D", "z = 3;", "z = 4;", "literal 3->4", V2)
    row(4, "EQ", "= ", "=  ", "space (helper g)", V2, nth=10)
    row(40, "EQ", "= ", "=  ", "space (helper g)", V2, nth=11, src="capture table x")
    row(6, "D", "line5 = 5", "line5 = 6", "literal 5->6", V2)
    row(7, "D", "line9 = 9", "line9 = 8", "literal 9->8", V2, span_text=V1)  # the span of V1: off by 1 in V2
    row(8, "EQ", "z = 3;", "z = 3 ;", "space before semicolon", V2)
    row(9, "EQ", "x = 1;", "x = 1 ;", "space after one", V2)
    row(10, "EQ", "y = 2;", "y = 2 ;", "space after two", V2)
    row(11, "NAMED_GAP", "y = 2;", "y = 3;", "literal 2->3", V2)
    row(12, "RI", "z = 3;", "z = 33;", "literal 3->33", V2)
    row(13, "D", "line1 = 1", "line1 = 7", "literal 1->7", V2)
    row(14, "D", "line2 = 2", "line2 = 7", "literal 2->7", V2)
    row(15, "EQ", "line4 = 4", "line4 = 4 ", "space after four", V2)
    rc = os.path.join(T, "receipts")
    receipt(os.path.join(rc, "rc-plain"), 1, "literal 1->5 @a.cpp:12", "difference", edit(V2, "x = 1;", "x = 5;")[0], WITNESS)
    receipt(os.path.join(rc, "rc-plain"), 2, "plus zero", "survived", edit(V1, "y = 2;", "y = 2+0;")[0], SURV)
    receipt(os.path.join(rc, "rc-plain"), 3, "literal 3->4", "difference", V2.replace("z = 3;", "z = 9;"), WITNESS)
    receipt(os.path.join(rc, "rc-mapped-seed1"), 5, "space @a.cpp:3 (helper g)", "survived", edit(V2, "= ", "=  ", 10)[0], SURV)
    receipt(os.path.join(rc, "rc-mapped-seed1"), 6, "space (helper g)", "survived", edit(V2, "= ", "=  ", 10)[0], SURV)
    receipt(os.path.join(rc, "rc-plain"), 7, "literal 9->8", "difference", edit(V2, "line9 = 9", "line9 = 8")[0], WITNESS)
    receipt(os.path.join(rc, "rc-plain"), 8, "space before semicolon", "difference", edit(V2, "z = 3;", "z = 3 ;")[0], WITNESS)
    receipt(os.path.join(rc, "rc-plain"), 9, "space after one", "survived", edit(V1, "x = 1;", "x = 1 ;")[0], SURV)
    receipt(os.path.join(rc, "rc-plain"), 10, "space after two", "survived", edit(V2, "y = 2;", "y = 2 ;")[0], SURV)
    receipt(os.path.join(rc, "rc-parent"), 10, "space after two", "difference", edit(V2, "y = 2;", "y = 2 ;")[0], WITNESS,
            spec="/b/lib2_parent/spec.expanded.txt")
    receipt(os.path.join(rc, "rc-plain"), 11, "literal 2->3", "difference", edit(V2, "y = 2;", "y = 3;")[0], WITNESS)
    receipt(os.path.join(rc, "rc-plain"), 12, "literal 3->33", "difference", edit(V2, "z = 3;", "z = 33;")[0], WITNESS)
    # 13: its only kill ran on a spec that never landed: unlanded-spec, the row is unbacked
    receipt(os.path.join(rc, "rc-unlanded"), 13, "literal 1->7", "difference", edit(V2, "line1 = 1", "line1 = 7")[0], WITNESS,
            spec="/b/lib2_unlanded_missing/spec.expanded.txt")
    # 14: a unit file under an old name that the rename table maps to a landed spec: current
    receipt(os.path.join(rc, "rc-renamed"), 14, "literal 2->7", "difference", edit(V2, "line2 = 2", "line2 = 7")[0], WITNESS,
            spec="/clone/tools/verify/units/lib2_oldname.txt")
    # 15: the receipt's own `spec` field names a landed companion (survivor: backs the EQ row) and, in another
    # receipt, an unlanded one (a difference that does not contradict the row)
    receipt(os.path.join(rc, "rc-field"), 15, "space after four", "survived", edit(V2, "line4 = 4", "line4 = 4 ")[0], SURV,
            jspec="lib2_scoped/comp")
    receipt(os.path.join(rc, "rc-field-gone"), 15, "space after four", "difference", edit(V2, "line4 = 4", "line4 = 4 ")[0],
            WITNESS, jspec="lib2_scoped/gone")
    receipt(os.path.join(rc, "rc-void"), 1, "literal 1->5", "difference", edit(V2, "x = 1;", "x = 5;")[0], WITNESS)
    receipt(os.path.join(rc, "rc-plain"), 99, "nothing like this", "survived", V2, SURV)
    receipt(os.path.join(rc, "rc-plain"), 98, "literal 1->5", "difference", "", WITNESS, cpp=False)
    with open(os.path.join(T, "ledger.tsv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    with open(os.path.join(T, "map.tsv"), "w") as f:
        f.write("local_id\tglobal_id\n5\t4\n6\t1\n")
    return repo, rc


def run_new(T, repo, rc, script, extra=()):
    r = subprocess.run([sys.executable, script] + list(extra) + [ "--ledger", os.path.join(T, "ledger.tsv"), "--root", repo, "--receipts", rc,
                        "--versions", repo + ":HEAD", "--idmap", "rc-mapped-=" + os.path.join(T, "map.tsv"),
                        "--exclude", "rc-void=void receipts", "--exclude-spec", "lib2_parent=no-credit whole-unit spec",
                        "--ruled-basis", "capture table", "--ruled-difference", "11=gap ruling x", "--ruled-difference", "4=stale",
                        "--out-receipts", os.path.join(T, "rec.tsv"), "--out-rows", os.path.join(T, "rows.tsv")],
                       capture_output=True, text=True)
    recs = {os.path.relpath(x["receipt"], rc): x for x in csv.DictReader(open(os.path.join(T, "rec.tsv")), delimiter="\t")}
    rows = {x["global_id"]: x for x in csv.DictReader(open(os.path.join(T, "rows.tsv")), delimiter="\t")}
    return r.returncode, recs, rows, r.stdout + r.stderr


def run_old(T, repo, rc, script):
    """the campaign script on the same fixture, laid out as it expects; its verdicts in the new tool's terms"""
    root = os.path.join(T, "old")
    O = os.path.join(root, "build", "j3d-hdrew1", "resume-after-agl-20261030")
    os.makedirs(O)
    shutil.copytree(os.path.join(repo, "src"), os.path.join(root, "src"))
    os.makedirs(os.path.join(root, "tools"))
    os.symlink(HERE, os.path.join(root, "tools", "verify"))
    shutil.copy(script, os.path.join(O, "integrate_receipts.py"))
    import hashlib
    sha = hashlib.sha256(V2.encode()).hexdigest()
    rows = list(csv.DictReader(open(os.path.join(T, "ledger.tsv")), delimiter="\t"))
    for r in rows:
        r.update(source_sha256=sha, disposition=r["final_class"], evidence="", current_receipt_status="", current_receipt_evidence="", provenance="")
    with open(os.path.join(O, "current-global-fullpool-4923-ledger.tsv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    for d in os.listdir(rc):
        shutil.copytree(os.path.join(rc, d), os.path.join(O, d))
    r = subprocess.run([sys.executable, os.path.join(O, "integrate_receipts.py")], cwd=root, capture_output=True, text=True)
    try:
        summ = json.load(open(os.path.join(O, "current-receipt-integration-summary.json")))
    except (OSError, ValueError):
        summ = {}
    recs = {}
    for f in os.listdir(os.path.join(O, "rc-plain")):
        if f.endswith(".json"):
            recs["rc-plain/" + f] = {"status": "unknown to the old script"}
    out = {x["global_id"]: x for x in csv.DictReader(open(os.path.join(O, "current-global-fullpool-4923-ledger.tsv")), delimiter="\t")}
    rowsv = {g: {"backing": "receipt" if x.get("disposition") in ("KILL", "EQ") and x.get("current_receipt_evidence") else
                 "UNBACKED" if x.get("disposition") not in ("KILL", "EQ", "NAMED_GAP", "RI", "SYNC") else "?"} for g, x in out.items()}
    return r.returncode, recs, rowsv, r.stdout + r.stderr + json.dumps(summ)[:600]


H1 = "struct S { int a; int b; };\n"
H2 = "struct S { int a; int pad; int b; };\n"  # S::b moves: g's text stays, its code does not
C1 = ("#include \"toy.h\"\n#define VERIFY(a, f)\n/* 02000010 */\nint g(S* s) { return s->b; }\nVERIFY(0x02000010, &g);\n"
      "/* 02000020 */\nint h(int x) { return x + 1; }\nVERIFY(0x02000020, &h);\n/* 02000030 */\nint k(int x) { return x * 3; }\n"
      "VERIFY(0x02000030, &k);\n")
C2 = C1.replace("return x + 1;", "return x + 2;")  # the landed file differs (h), so receipts on C1 are stale


def objpin_case(T):
    """--object-pin: the row of g (its text unchanged, S's layout changed in the header) must not pin; the row of k (nothing
    it uses changed) pins"""
    repo = os.path.join(T, "orepo")
    for d in ("wwhd_src/include", "wwhd_src/toy"):
        os.makedirs(os.path.join(repo, d))
    git(repo, "init", "-q")
    for h, c, m in ((H1, C1, "v1"), (H2, C2, "v2")):
        open(os.path.join(repo, "wwhd_src/include/toy.h"), "w").write(h)
        open(os.path.join(repo, "wwhd_src/toy/toy.cpp"), "w").write(c)
        git(repo, "add", ".")
        git(repo, "commit", "-q", "-m", m)
    os.makedirs(os.path.join(repo, "tools", "verify", "units"))
    open(os.path.join(repo, "tools", "verify", "units", "lib2_scoped.txt"), "w").write("src wwhd_src/toy/toy.cpp\n")
    rows = []
    for gid, func, original, replacement, mut in (("g1", "02000010", "return s->b;", "return s->b + 0;", "plus zero g"),
                                                   ("k1", "02000030", "return x * 3;", "return x * 3 + 0;", "plus zero k")):
        k = C2.index(original)
        rows.append({"global_id": gid, "function": func, "helper_callers": "", "source": "wwhd_src/toy/toy.cpp", "span_start": k,
                     "span_end": k + len(original), "original": original, "replacement": replacement, "mutation": mut,
                     "final_class": "EQ", "class_source": "ledger x"})
        receipt(os.path.join(T, "oreceipts", "rc-o"), gid, mut, "survived", C1.replace(original, replacement), SURV,
                src="wwhd_src/toy/toy.cpp", func=func)
    led = os.path.join(T, "oledger.tsv")
    with open(led, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    res = {}
    for mode in ("--object-pin",):
        out = os.path.join(T, "orows%s.tsv" % mode)
        r = subprocess.run([sys.executable, os.path.join(HERE, "receipt_audit.py"), mode, "--ledger", led, "--root", repo,
                            "--receipts", os.path.join(T, "oreceipts"), "--versions", repo + ":HEAD", "--out-rows", out],
                           capture_output=True, text=True)
        res[mode] = ({x["global_id"]: x for x in csv.DictReader(open(out), delimiter="\t")} if os.path.exists(out) else {}, r)
    ob, r = res["--object-pin"]
    check(ob.get("g1", {}).get("backing") == "UNBACKED" and "compiles differently" in ob.get("g1", {}).get("pin_note", ""),
          "object pin: a struct-size change with g's text unchanged does not pin: %s" % ob.get("g1", {}).get("pin_note", r.stderr[-300:]))
    check(ob.get("k1", {}).get("backing") == "pin", "object pin: k (nothing it uses changed) pins: %s" % ob.get("k1", {}).get("pin_note", r.stderr[-300:]))
    check(r.returncode == 1, "object pin: the unpinned row fails the run")
    # (no --function-pin comparison here: that pin cuts functions with mutate.bodies, which needs the glue markers this toy
    # source lacks; the main fixture's rows 2 and 9 cover it)


def main():
    old = sys.argv[sys.argv.index("--old") + 1] if "--old" in sys.argv else None
    T = tempfile.mkdtemp(prefix="receipt_audit_test-")
    try:
        repo, rc = build(T)
        rc_, recs, rows, log = (run_old if old else run_new)(T, repo, rc, old or os.path.join(HERE, "receipt_audit.py"))
        st = {k: v["status"] for k, v in recs.items()}
        check(rc_ == 1, "exit status 1 (got %s)" % rc_)
        check(st.get("rc-plain/mutant1.json") == "current" and rows.get("1", {}).get("backing") == "receipt", "row 1: current difference backs it")
        check(st.get("rc-plain/mutant2.json") == "stale" and "commit" in recs.get("rc-plain/mutant2.json", {}).get("version", ""),
              "row 2: the v1 receipt is stale, naming its version: %s" % recs.get("rc-plain/mutant2.json"))
        check(rows.get("2", {}).get("backing") == "UNBACKED", "row 2: a stale receipt does not back a credited row")
        check(st.get("rc-plain/mutant3.json") == "error", "row 3: a mutantN.cpp matching no version is an error")
        check(st.get("rc-mapped-seed1/mutant5.json") == "current" and recs["rc-mapped-seed1/mutant5.json"]["global_id"] == "4"
              and rows.get("4", {}).get("backing") == "receipt", "row 4: the id map joins local 5 to global 4, not to row 40")
        check(st.get("rc-mapped-seed1/mutant6.json") == "error", "an id map to a row of another mutation is an error")
        check(rows.get("40", {}).get("backing") == "basis", "row 40: a ruled basis named in class_source")
        check(rows.get("6", {}).get("backing") == "UNBACKED", "row 6: a missing receipt leaves the credited row unbacked")
        check(st.get("rc-plain/mutant7.json") == "error" and rows.get("7", {}).get("backing") == "UNBACKED",
              "row 7: a stale span: the landed receipt does not fit it (error), the row is unbacked")
        check(st.get("rc-void/mutant1.json") == "excluded" and recs["rc-void/mutant1.json"].get("note") == "void receipts", "excluded with its reason")
        check(st.get("rc-plain/mutant99.json") == "unjoined" and st.get("rc-plain/mutant98.json") == "no-companion", "unjoined and no-companion listed")
        n_files = sum(1 for d, _, fs in os.walk(rc) for f in fs if f.endswith(".json"))
        check(len(recs) == n_files, "every receipt has a status (%d of %d)" % (len(recs), n_files))
        check(rows.get("8", {}).get("backing") == "CONTRADICTED", "row 8: an EQ row with a current clean difference is contradicted")
        check(rows.get("10", {}).get("backing") == "receipt" and st.get("rc-parent/mutant10.json") == "excluded" and
              "lib2_parent" in recs.get("rc-parent/mutant10.json", {}).get("note", ""),
              "row 10: a difference from the excluded parent spec does not contradict")
        check(rows.get("11", {}).get("backing") == "ruled-difference" and "gap ruling x" in rows.get("11", {}).get("contradicted_ruled", ""),
              "row 11: a named NAMED_GAP difference is listed as ruled, not failing: %s" % rows.get("11"))
        check(rows.get("12", {}).get("backing") == "CONTRADICTED", "row 12: an unnamed RI difference fails")
        check("--ruled-difference 4 names a row with no current clean difference" in log, "a named row without a difference is an error")
        check(rows.get("9", {}).get("backing") == "UNBACKED" and "function identical" in recs.get("rc-plain/mutant9.json", {}).get("version", ""),
              "row 9: stale with the function identical: unbacked by default")
        check(st.get("rc-unlanded/mutant13.json") == "unlanded-spec" and "lib2_unlanded_missing" in recs.get("rc-unlanded/mutant13.json", {}).get("note", "")
              and rows.get("13", {}).get("backing") == "UNBACKED", "row 13: a kill on a spec that never landed is unlanded-spec, the row unbacked: %s"
              % recs.get("rc-unlanded/mutant13.json"))
        check(st.get("rc-renamed/mutant14.json") == "current" and rows.get("14", {}).get("backing") == "receipt",
              "row 14: a renamed spec (rename table) counts as landed")
        check(st.get("rc-field/mutant15.json") == "current" and st.get("rc-field-gone/mutant15.json") == "unlanded-spec"
              and rows.get("15", {}).get("backing") == "receipt", "row 15: the receipt's spec field: landed backs, unlanded neither backs nor contradicts")
        check({g for g, x in rows.items() if x.get("backing") == "UNBACKED"} == {"2", "3", "6", "7", "9", "13"},
              "UNBACKED rows are exactly 2, 3, 6, 7, 9, 13: %s" % sorted(g for g, x in rows.items() if x.get("backing") == "UNBACKED"))
        if not old:
            objpin_case(T)
            _, _, rows2, _ = run_new(T, repo, rc, os.path.join(HERE, "receipt_audit.py"), ["--function-pin"])
            check(rows2.get("9", {}).get("backing") == "pin" and rows2.get("2", {}).get("backing") == "pin" and
                  rows2.get("6", {}).get("backing") == "UNBACKED" and rows2.get("7", {}).get("backing") == "UNBACKED",
                  "--function-pin: rows 2 and 9 (f unchanged since v1) pinned; 6 and 7 stay unbacked")
        if FAILS:
            print(log[-1500:])
    finally:
        shutil.rmtree(T, ignore_errors=True)  # this run's own temporary directory
    print("receipt_audit_test%s: %s" % (" (old script)" if old else "", "PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
