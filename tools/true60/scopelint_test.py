#!/usr/bin/env python3
"""scopelint_test.py: scopelint.py on small sources: a RAII gate local in a glued (marked) body is flagged; the same in an
unmarked function, a template local, a type with a gabi_construct hook, a `scopelint: ok` line and a member are not."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = r'''
namespace x_t60 {
struct DtOne {
    float prev;
    DtOne() : prev(t60_set_dt(1.0f)) {}
    ~DtOne() { t60_set_dt(prev); }
};
template <class A> struct Form { Form(A* a) { (void)a; } };
struct Hooked { Hooked() {} };
}
struct Holder {
    x_t60::DtOne member;
};
inline void gabi_construct(x_t60::Hooked&) {}

/* 02000000 */
void glued(int* a) {
    x_t60::DtOne gate;
    Form<int> f(a);
    x_t60::Hooked h;
    x_t60::DtOne accepted; // scopelint: ok test
    (void)a;
}

void unmarked() {
    x_t60::DtOne gate;
}
'''


def main():
    with tempfile.TemporaryDirectory() as d:
        p = os.path.join(d, "t.cpp")
        open(p, "w").write(SRC)
        r = subprocess.run([sys.executable, os.path.join(HERE, "scopelint.py"), p], capture_output=True, text=True)
        hits = [l for l in r.stdout.splitlines() if "declares a constructor" in l]
        ok = r.returncode == 1 and len(hits) == 1 and ":18: `x_t60::DtOne gate`" in hits[0]
        print(r.stdout)
        print("scopelint_test: %s" % ("PASS" if ok else "FAIL"))
        return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
