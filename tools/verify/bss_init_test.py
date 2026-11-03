#!/usr/bin/env python3
"""bss_init_test.py: initialised `data` views in .bss are checked against the loader's zero fill (img_read gives
zeros inside NOBITS sections and None outside every section), so `static T x = {0...};` at a .bss address passes
and a non-zero initialiser there fails."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import autoglue
bad = []
if autoglue.img_read(0x101FD888, 4) != b"\0\0\0\0": bad.append(".bss word is not zero")
if autoglue.img_read(0x101FCDFC, 8) is not None: bad.append("a read across the .bss start is not None")
if autoglue.img_read(0x10000000, 4) is None: bad.append(".rodata read failed")
if autoglue.img_read(0x0F000000, 4) is not None: bad.append("unmapped read is not None")
try:
    autoglue.check_table("t", 0x101FD888, "u32", [2], "{0, 0}", "", {}, "test", "test.cpp", "")
except Exception as ex:
    bad.append("zero initialiser in .bss rejected: %s" % ex)
try:
    autoglue.check_table("t", 0x101FD888, "u32", [1], "{1}", "", {}, "test", "test.cpp", "")
    bad.append("non-zero initialiser in .bss accepted")
except autoglue.GlueError:
    pass
for b in bad: print("FAIL", b)
print("bss_init_test: %s" % ("FAIL" if bad else "PASS"))
sys.exit(1 if bad else 0)
