#!/usr/bin/env python3
"""Identical-code check of mutate.py (trivial compiler equivalence, 2026-10-28).

A mutant is compiled exactly like mutate.py's per-mutant build (clang++ -std=c++20 -O1 -w, the verify FP flags, the
include paths, the autoglue/#line text) into an object under the unit source's own file name, and the WHOLE object
file is compared byte for byte with the unmutated source's object built the same way. clang's Mach-O objects carry no
time stamp and, without -g, no path: two compiles of the same text under the same file name give the same bytes from
any directory; the file name itself reaches the object only as the static-initialiser symbol __GLOBAL__sub_I_<name>,
which is why both sides use the source's own name. Equal bytes = equal code of every function, data, rodata,
relocations and symbol references: the mutant program is the original program, so it is closed as
`equivalent-identical-code` without a run.

Mutants whose objects are identical to EACH OTHER are the same program too: within one run configuration (the same
function scope and input selection) one of them runs and its outcome applies to all (Groups).
"""
import hashlib
import threading

EQ_CLASS = "equivalent-identical-code"
EQ_REASON = "compiles to identical machine code"


def object_digest(path):
    """sha256 (hex) of a whole object file"""
    h = hashlib.sha256()
    with open(path, "rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


class Groups:
    """Mutants with identical objects and the same run configuration: the first one to arrive runs (the representative),
    later ones wait for its outcome. claim(key, i) -> (True, None) for the representative, which must call done(key,
    outcome) (or failed(key) when it raised), else (False, (rep id, outcome)) once the representative finished; when
    the representative failed, the next waiter becomes the representative."""

    def __init__(self):
        self.lock = threading.Lock()
        self.groups = {}  # key -> {"rep": id, "event": Event, "outcome": object, "ok": bool}
        self.dups = 0

    def claim(self, key, i):
        while True:
            with self.lock:
                g = self.groups.get(key)
                if g is None:
                    self.groups[key] = {"rep": i, "event": threading.Event(), "outcome": None, "ok": False}
                    return True, None
            g["event"].wait()
            with self.lock:
                if g["ok"]:
                    self.dups += 1
                    return False, (g["rep"], g["outcome"])
                if self.groups.get(key) is g:  # the representative failed: the next one takes over
                    del self.groups[key]

    def done(self, key, outcome):
        with self.lock:
            g = self.groups[key]
            g["outcome"], g["ok"] = outcome, True
        g["event"].set()

    def failed(self, key):
        with self.lock:
            g = self.groups.get(key)
        if g is not None:
            g["event"].set()
