"""verify.py result cache (2026-10-27): re-checks (re-gates after folds, repeated gate sets) reuse the result rows of
functions whose inputs to the verdict did not change.

One entry per (unit spec, function, every result-affecting option), keyed by a SHA-256 over
  - the function's compiled candidate code: the closure of its entry thunk over the unit's `src` objects, atom by atom
    (Mach-O symbols delimit atoms, relocations are the edges; anonymous literals are hashed by content, named atoms by
    name + content), plus the attached unwind records and every object's static-initialiser closure; an undefined
    symbol resolves to the object the linker took it from (the link map). Anything this reader cannot follow exactly
    (a section-relative relocation, an unknown relocation type, a missing map entry) makes that function's key cover
    every `src` object of the unit whole, so uncertainty only costs hits;
    on a spec declaring `osmem ... threads` every key covers every `src` object (candidate code runs in other rows there);
  - every compiled object a function can run for real: unit.c.o whole (all originals, real callees and setup /
    postsetup chains of the unit), harness, espresso_fp and support objects;
  - the expanded spec (includes and shared world files inlined; comment-only lines dropped except `# verify:` /
    `# readable-scope:` headers), the unit name, the content of every `blob` file and of the function's recordings;
  - the tool version (every file under tools/verify except units/ and recordings/), the gen (build/gen: real path,
    names, sizes, modification times; the NaN-exact gen is a different directory), the shared tables (image.bin,
    livein.tsv, frames.tsv, build/*.tsv), the compiler (clang/clang++ --version), the compile flags and -srcopt;
  - the harness arguments that reach the run (-n, -seed, -nulltrap, -v, -strictnan, -calleesp, ...; not -only, the
    progress options or -shards' process count for unsharded runs), the shard count of a sharded run, and every WWHD_*
    environment variable except the ones that only steer progress, scratch, slots or this cache.
The generated input stream is a pure function of (function, seed, n, spec, gen, harness), all of which are in the key.

Entries live in $WWHD_VERIFY_SCRATCH/vcache (the RAM disk; no cache without it), shared by all worktrees (keys are
content hashes), capped at WWHD_VERIFY_CACHE_MAX_MB (default 256 MB) with least-recently-used eviction.
A cached function's stdout block is printed byte for byte as the fresh run printed it; stderr gets its recorded notes
(nulltrap DSI counts, setupsnapshot refusals) plus one `vcache: ADDR cached` line. Off with --no-cache or
WWHD_VERIFY_CACHE=0; --archive or WWHD_VERIFY_ARCHIVE=1 (final/archive gates; readable_check.py always sets it) never
uses a cached row. Self-test: vcache_test.py.
"""
import bisect
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
VERSION = "vcache-1"
MARK = re.compile(r"^@VC([<>]) ([0-9A-F]{8})$")

# harness options that print per-input detail or select inputs/runs themselves: such runs are never cached
UNCACHEABLE = ("-trace", "-watch", "-range", "-list", "-listfn", "-shardout", "-stopfirst", "-mutrec", "-forkserver",
               "-image")
# options (with their value count) that do not change any result row
NEUTRAL = {"-noprogress": 0, "-progress-interval": 1, "-stuck": 1}
# WWHD_* variables that do not change results (progress, scratch, slots, this cache)
ENV_NEUTRAL = re.compile(r"^WWHD_(VERIFY_SCRATCH.*|VERIFY_PROGRESS.*|VERIFY_STUCK|PROGRESS_REGISTRY|SHARED_READY|"
                         r"VERIFY_CACHE.*|VERIFY_ARCHIVE|SHARDS|HEAVY_.*|MUT_SCRATCH|MEMSTAT)$")


def sha(*parts):
    h = hashlib.sha256()
    for p in parts:
        if isinstance(p, str):
            p = p.encode()
        h.update(struct.pack("<Q", len(p)))
        h.update(p)
    return h.hexdigest()


def file_sha(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def archive_mode(args=None):
    return os.environ.get("WWHD_VERIFY_ARCHIVE", "") not in ("", "0") or bool(args and "--archive" in args)


def cache_dir():
    want = os.environ.get("WWHD_VERIFY_SCRATCH")
    if not want or not os.path.isdir(want) or not os.access(want, os.W_OK | os.X_OK):
        return None
    d = os.path.join(want, "vcache")
    try:
        os.makedirs(d, exist_ok=True)
    except OSError:
        return None
    return d


# ---------------------------------------------------------------------------------------------- Mach-O object reader
class Uncertain(Exception):
    """the closure cannot be followed exactly: the caller falls back to the whole unit"""


S_ZEROFILL, S_THREAD_LOCAL_ZEROFILL, S_GB_ZEROFILL = 0x1, 0x12, 0xC
S_ATTR_PURE_INSTRUCTIONS, S_ATTR_SOME_INSTRUCTIONS = 0x80000000, 0x400
ARM64_RELOC_UNSIGNED, ARM64_RELOC_SUBTRACTOR, ARM64_RELOC_BRANCH26, ARM64_RELOC_ADDEND = 0, 1, 2, 10
KNOWN_RELOCS = set(range(0, 11))  # UNSIGNED .. ADDEND (arm64); anything else is Uncertain
ANON = re.compile(r"^(l|L|ltmp|GCC_except_table|___unnamed)")


class Sect:
    __slots__ = ("idx", "seg", "name", "addr", "size", "data", "relocs", "raddrs", "flags", "starts", "atomsyms", "code")


class MachO:
    def __init__(self, path):
        self.path = path
        b = open(path, "rb").read()
        magic, cpu, _sub, ftype, ncmds, _sz, _fl, _r = struct.unpack_from("<IiiIIIII", b, 0)
        if magic != 0xFEEDFACF or ftype != 1:
            raise Uncertain("%s: not a 64-bit Mach-O object" % path)
        self.sects, syms = [None], []
        off = 32
        symtab = None
        for _ in range(ncmds):
            cmd, size = struct.unpack_from("<II", b, off)
            if cmd == 0x19:  # LC_SEGMENT_64
                nsects = struct.unpack_from("<I", b, off + 64)[0]
                so = off + 72
                for _k in range(nsects):
                    sn, sg, addr, ssize, foff, _al, reloff, nreloc, flags = struct.unpack_from("<16s16sQQIIIII", b, so)
                    s = Sect()
                    s.idx, s.name, s.seg = len(self.sects), sn.rstrip(b"\0").decode(), sg.rstrip(b"\0").decode()
                    s.addr, s.size, s.flags = addr, ssize, flags
                    zf = (flags & 0xFF) in (S_ZEROFILL, S_THREAD_LOCAL_ZEROFILL, S_GB_ZEROFILL)
                    s.data = None if zf else b[foff:foff + ssize]
                    s.code = bool(flags & (S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS))
                    # relocations in table order; an ARM64_RELOC_ADDEND or ARM64_RELOC_SUBTRACTOR entry belongs to
                    # the entry after it: (address, symnum, pcrel, length, extern, type, explicit addend, subtractor)
                    s.relocs, pend_add, pend_sub = [], 0, None
                    for r in range(nreloc):
                        ra, info = struct.unpack_from("<iI", b, reloff + 8 * r)
                        e = (ra, info & 0xFFFFFF, (info >> 24) & 1, (info >> 25) & 3, (info >> 27) & 1, info >> 28)
                        if e[5] == ARM64_RELOC_ADDEND:
                            pend_add = e[1] if e[1] < 0x800000 else e[1] - 0x1000000
                            continue
                        if e[5] == ARM64_RELOC_SUBTRACTOR:
                            pend_sub = e
                            continue
                        s.relocs.append(e + (pend_add, pend_sub))
                        pend_add, pend_sub = 0, None
                    if pend_add or pend_sub:
                        raise Uncertain("%s: dangling relocation pair in %s" % (path, s.name))
                    s.relocs.sort(key=lambda x: x[0])
                    s.raddrs = [r[0] for r in s.relocs]
                    self.sects.append(s)
                    so += 80
            elif cmd == 0x2:  # LC_SYMTAB
                symtab = struct.unpack_from("<IIII", b, off + 8)
            off += size
        if not symtab:
            raise Uncertain("%s: no symbol table" % path)
        symoff, nsyms, stroff, _ss = symtab
        self.syms = []  # (name, type, sect, value)
        for i in range(nsyms):
            strx, ntype, nsect, _desc, value = struct.unpack_from("<IBBHQ", b, symoff + 16 * i)
            e = b.index(b"\0", stroff + strx)
            self.syms.append((b[stroff + strx:e].decode(errors="replace"), ntype, nsect, value))
        self.defined = {}  # name -> (sect idx, value) for N_SECT symbols
        for s in self.sects[1:]:
            s.starts, s.atomsyms = [s.addr], {s.addr: []}
        for name, ntype, nsect, value in self.syms:
            if ntype & 0xE0:  # stab
                continue
            if (ntype & 0x0E) == 0x0E and nsect:
                self.defined[name] = (nsect, value)
                s = self.sects[nsect]
                if value not in s.atomsyms:
                    s.atomsyms[value] = []
                s.atomsyms[value].append(name)
        for s in self.sects[1:]:
            s.starts = sorted(a for a in s.atomsyms if s.addr <= a <= s.addr + s.size)
            if not s.starts or s.starts[0] != s.addr:
                s.starts.insert(0, s.addr)
                s.atomsyms.setdefault(s.addr, [])
            if s.seg == "__LD" and s.name == "__compact_unwind":  # 32-byte records, one per function range
                s.starts = list(range(s.addr, s.addr + s.size, 32))

    def atom_of(self, sidx, addr):
        """(sect idx, atom start) of the atom holding addr (an address one past the section end: its last atom)"""
        s = self.sects[sidx]
        if not (s.addr <= addr <= s.addr + s.size):
            raise Uncertain("%s: target %X outside section %s" % (self.path, addr, s.name))
        i = bisect.bisect_right(s.starts, addr) - 1
        if addr == s.addr + s.size and i > 0 and s.starts[i] == addr:
            i -= 1
        return sidx, s.starts[i]

    def atom_end(self, sidx, start):
        s = self.sects[sidx]
        i = bisect.bisect_right(s.starts, start)
        return s.starts[i] if i < len(s.starts) else s.addr + s.size

    def atom_name(self, sidx, start):
        """the atom's identity name: its first non-anonymous symbol, or None (anonymous: identified by content)"""
        names = sorted(n for n in self.sects[sidx].atomsyms.get(start, []) if not ANON.match(n))
        return names[0] if names else None

    def target(self, symnum, ext, addend, absolute=False):
        """('atom', sidx, start, off) | ('ext', name, addend) | ('sect', sidx) (section-relative without a readable
        address: the whole section). absolute: a section-relative pointer whose in-place value is the address"""
        if not ext:
            if absolute and 1 <= symnum < len(self.sects):
                ts, tstart = self.atom_of(symnum, addend)
                return ("atom", ts, tstart, addend - tstart)
            return ("sect", symnum)
        name, ntype, nsect, value = self.syms[symnum]
        if (ntype & 0x0E) == 0x0E and nsect:
            ts, tstart = self.atom_of(nsect, value + addend)
            return ("atom", ts, tstart, value + addend - tstart)
        return ("ext", name, addend)

    def edges(self, sidx, start):
        """the atom's relocations as (offset in atom, type, length, pcrel, target); a SUBTRACTOR pair gives two
        entries (type 1 for the subtracted symbol)"""
        s = self.sects[sidx]
        end = self.atom_end(sidx, start)
        out = []
        lo = bisect.bisect_left(s.raddrs, start - s.addr)
        for ra, symnum, pcrel, length, ext, rtype, addend, sub in s.relocs[lo:]:
            a = s.addr + ra
            if a >= end:
                break
            if rtype not in KNOWN_RELOCS:
                raise Uncertain("%s: relocation type %d" % (self.path, rtype))
            if rtype == ARM64_RELOC_UNSIGNED and s.data is not None:
                n = 8 if length == 3 else 4
                o = a - s.addr
                addend += int.from_bytes(s.data[o:o + n], "little", signed=True)
            if sub is not None:
                out.append((a - start, ARM64_RELOC_SUBTRACTOR, sub[3], sub[2], self.target(sub[1], sub[4], 0)))
            absolute = rtype == ARM64_RELOC_UNSIGNED and s.data is not None and sub is None
            out.append((a - start, rtype, length, pcrel, self.target(symnum, ext, addend, absolute)))
        return out

    def atom_bytes(self, sidx, start):
        s = self.sects[sidx]
        end = self.atom_end(sidx, start)
        if s.data is None:
            return b"zerofill:%d" % (end - start)
        return s.data[start - s.addr:end - s.addr]


class Closure:
    """per-function candidate-code hashes over a unit's `src` objects (see the module docstring)"""

    def __init__(self, objs, linkmap):
        self.objs = {os.path.realpath(o): MachO(o) for o in objs}
        self.where = parse_linkmap(linkmap)  # name -> object path the linker took it from; address -> (path, name)
        self.memo = {}

    def resolve_ext(self, name):
        """an undefined symbol: the atom of the object the linker took it from, or None (outside the unit's sources)"""
        obj = self.where["names"].get(name)
        if obj in self.objs:
            m = self.objs[obj]
            if name not in m.defined:
                raise Uncertain("%s: %s not defined there" % (obj, name))
            sidx, val = m.defined[name]
            return obj, m.atom_of(sidx, val)
        return None

    def atom_key(self, obj, sidx, start):
        m = self.objs[obj]
        name = m.atom_name(sidx, start)
        s = m.sects[sidx]
        return (obj, sidx, start), name, s

    def walk(self, roots, follow_code_refs=True):
        """the set of named atoms, anonymous-atom hashes and outside names reachable from roots ((obj, sidx, start));
        follow_code_refs=False (static initialisers): a non-call reference into code (a function pointer taken for a
        registration) records the target's name only"""
        seen, todo, leaves = set(), list(roots), set()
        while todo:
            obj, sidx, start = todo.pop()
            if (obj, sidx, start) in seen:
                continue
            seen.add((obj, sidx, start))
            m = self.objs[obj]
            for _off, rtype, _l, _p, tgt in m.edges(sidx, start):
                if tgt[0] == "sect":
                    s = m.sects[tgt[1]]
                    for st in s.starts:
                        todo.append((obj, tgt[1], st))
                    continue
                if tgt[0] == "ext":
                    r = self.resolve_ext(tgt[1])
                    if r is None:
                        leaves.add(tgt[1])
                        continue
                    tobj, (ts, tstart) = r
                else:
                    tobj, ts, tstart = obj, tgt[1], tgt[2]
                tm = self.objs[tobj]
                if not follow_code_refs and tm.sects[ts].code and rtype != ARM64_RELOC_BRANCH26:
                    leaves.add("&" + (tm.atom_name(ts, tstart) or "anon@%s" % self.anon_hash(tobj, ts, tstart, set())))
                    continue
                todo.append((tobj, ts, tstart))
        # unwind records attached to a reached function
        for obj, m in self.objs.items():
            for s in m.sects[1:]:
                if s.seg == "__LD" and s.name == "__compact_unwind":
                    for st in s.starts:
                        e = m.edges(s.idx, st)
                        f = [t for t in e if t[0] == 0]
                        if not f:
                            continue
                        t = f[0][4]
                        if t[0] == "atom" and (obj, t[1], t[2]) in seen:
                            seen.add((obj, s.idx, st))
                            for _o, _r, _l, _p, tg in e:
                                if tg[0] == "atom":
                                    seen.add((obj, tg[1], tg[2]))
                if s.seg == "__TEXT" and s.name == "__eh_frame":
                    for st in s.starts:
                        seen.add((obj, s.idx, st))
        return seen, leaves

    def anon_hash(self, obj, sidx, start, stack, cyc=None):
        """content hash of an atom: bytes + relocations, a named target by name, an anonymous one by its own hash
        (a cycle through anonymous atoms hashes as `cycle` there and is not memoised)"""
        key = (obj, sidx, start)
        if key in self.memo:
            return self.memo[key]
        if key in stack:
            if cyc is not None:
                cyc.append(1)
            return "cycle"
        stack = stack | {key}
        mine = []
        m = self.objs[obj]
        s = m.sects[sidx]
        edges = m.edges(sidx, start)
        data = bytearray(m.atom_bytes(sidx, start))
        if s.data is not None:  # pointer words hold the target's object address: it is described below instead
            for off, rtype, length, _p, _t in edges:
                if rtype in (ARM64_RELOC_UNSIGNED, ARM64_RELOC_SUBTRACTOR):
                    n = 8 if length == 3 else 4
                    data[off:off + n] = b"\0" * len(data[off:off + n])
        parts = [s.seg, s.name, bytes(data)]
        for off, rtype, length, pcrel, tgt in edges:
            if tgt[0] == "sect":
                d = "sect:%s,%s" % (m.sects[tgt[1]].seg, m.sects[tgt[1]].name)
            elif tgt[0] == "ext":
                d = "ext:%s+%d" % (tgt[1], tgt[2])
            else:
                nm = m.atom_name(tgt[1], tgt[2])
                d = ("name:%s" % nm) if nm else ("anon:%s" % self.anon_hash(obj, tgt[1], tgt[2], stack, mine))
                d += "+%d" % tgt[3]
            parts.append("%d/%d/%d/%d/%s" % (off, rtype, length, pcrel, d))
        h = sha(*parts)
        if mine:
            if cyc is not None:
                cyc.append(1)
        else:
            self.memo[key] = h
        return h

    def summarize(self, seen, leaves):
        items = []
        for obj, sidx, start in seen:
            nm = self.objs[obj].atom_name(sidx, start)
            items.append("%s=%s" % (nm or "anon", self.anon_hash(obj, sidx, start, set())))
        return sha(*(sorted(items) + sorted("leaf:" + x for x in leaves)))

    def static_init(self):
        """every object's static initialisers (dynamic initialisation of the globals a function reads)"""
        if "init" not in self.memo:
            roots = []
            for obj, m in self.objs.items():
                for s in m.sects[1:]:
                    if s.name in ("__StaticInit", "__mod_init_func", "__init_offsets"):
                        roots += [(obj, s.idx, st) for st in s.starts]
            self.memo["init"] = self.summarize(*self.walk(roots, follow_code_refs=False))
        return self.memo["init"]

    def function(self, vmaddr):
        """the closure hash of the candidate whose entry thunk the linker placed at vmaddr"""
        hits = [(o, n) for o, n in self.where["addrs"].get(vmaddr, []) if o in self.objs and n in self.objs[o].defined]
        if not hits:
            raise Uncertain("entry %X is not a symbol of a unit source object" % vmaddr)
        obj, name = hits[0]
        m = self.objs[obj]
        sidx, val = m.defined[name]
        seen, leaves = self.walk([(obj,) + m.atom_of(sidx, val)])
        return sha(self.summarize(seen, leaves), self.static_init())


def parse_linkmap(path):
    """ld's -map output: the defining object of every symbol, and the symbol at each address"""
    files, names, addrs = {}, {}, {}
    sect = None
    for line in open(path, errors="replace"):
        if line.startswith("# Object files:"):
            sect = "obj"
            continue
        if line.startswith("# Symbols:"):
            sect = "sym"
            continue
        if line.startswith("# Dead Stripped Symbols:") or line.startswith("# Sections:"):
            sect = None if not line.startswith("# Sections:") else "sec"
            continue
        if line.startswith("#"):
            continue
        if sect == "obj":
            m = re.match(r"^\[\s*(\d+)\]\s+(.*?)\s*$", line)
            if m:
                files[int(m.group(1))] = os.path.realpath(m.group(2)) if os.path.exists(m.group(2)) else m.group(2)
        elif sect == "sym":
            m = re.match(r"^0x([0-9A-Fa-f]+)\s+0x[0-9A-Fa-f]+\s+\[\s*(\d+)\]\s+(.*?)\s*$", line)
            if m:
                obj = files.get(int(m.group(2)))
                names.setdefault(m.group(3), obj)
                addrs.setdefault(int(m.group(1), 16), []).append((obj, m.group(3)))
    return {"names": names, "addrs": addrs}


# ------------------------------------------------------------------------------------------------- unit-level parts
_memo_cache = None


def big_file_sha(path, cdir):
    """content hash of a large shared file (image.bin, blobs), remembered by (path, size, mtime) in the cache dir"""
    global _memo_cache
    st = os.stat(path)
    k = "%s|%d|%d" % (os.path.realpath(path), st.st_size, st.st_mtime_ns)
    mp = os.path.join(cdir, "filememo.json") if cdir else None
    if _memo_cache is None:
        try:
            _memo_cache = json.load(open(mp)) if mp else {}
        except (OSError, ValueError):
            _memo_cache = {}
    if k not in _memo_cache:
        _memo_cache[k] = file_sha(path)
        if mp:
            tmp = "%s.%d.tmp" % (mp, os.getpid())
            try:
                json.dump(_memo_cache, open(tmp, "w"))
                os.replace(tmp, mp)
            except OSError:
                pass
    return _memo_cache[k]


def tree_sha(root, skip=()):
    parts = []
    for dp, dns, fns in os.walk(root):
        dns[:] = sorted(d for d in dns if d != "__pycache__" and os.path.relpath(os.path.join(dp, d), root) not in skip)
        for f in sorted(fns):
            if f.endswith(".pyc") or f.startswith("."):
                continue
            p = os.path.join(dp, f)
            parts += [os.path.relpath(p, root), file_sha(p)]
    return sha(*parts)


def gen_fingerprint(root):
    gen = os.path.join(root, "build", "gen")
    real = os.path.realpath(gen)
    parts = [real]
    for dp, dns, fns in os.walk(real):
        dns.sort()
        for f in sorted(fns):
            st = os.stat(os.path.join(dp, f))
            parts.append("%s|%d|%d" % (os.path.relpath(os.path.join(dp, f), real), st.st_size, st.st_mtime_ns))
    return sha(*parts)


_clang = None


def compiler_id():
    global _clang
    if _clang is None:
        _clang = sha(*[subprocess.run([c, "--version"], capture_output=True, text=True).stdout for c in ("clang", "clang++")])
    return _clang


def spec_text(lines):
    out = []
    for line in lines:
        s = line.strip()
        if not s:
            continue
        if s.startswith("#") and not re.match(r"#\s*(verify|readable-scope):", s):
            continue
        out.append(line.rstrip("\n"))
    return "\n".join(out)


def key_args(args, shards):
    """the harness arguments that reach the rows, normalised; None when the run must not be cached"""
    out, i = [], 0
    while i < len(args):
        a = args[i]
        if a in UNCACHEABLE:
            return None
        if a in NEUTRAL:
            i += 1 + NEUTRAL[a]
            continue
        if a in ("-only", "-rec"):  # the scope is the function itself; recordings are hashed by content
            i += 2
            continue
        if a in ("-dsiout", "-runlist"):  # kept as flags; -dsiout records are cached, a -runlist entry is per function
            out.append(a)
            i += 2
            continue
        out.append(a)
        i += 1
    return out + ["shards=%d" % shards]


def only_set(args):
    res = set()
    for i, a in enumerate(args):
        if a == "-only" and i + 1 < len(args):
            for x in args[i + 1].split(","):
                if x.strip():
                    res.add(int(x, 16))
    return res


def rec_dir(args):
    for i, a in enumerate(args):
        if a == "-rec" and i + 1 < len(args):
            return args[i + 1]
    return None


def runlist_sel(args):
    """-runlist FILE as {addr: its lines, verbatim and in order}, None without -runlist. Functions follow the harness
    (load_runlist: a hex address after optional blanks starts a function's line; other lines are skipped); the key uses
    the raw lines, so any difference in a function's selection misses"""
    if "-runlist" not in args:
        return None
    sel = {}
    for line in open(args[args.index("-runlist") + 1]):
        m = re.match(r"^\s*(?:0[xX])?([0-9A-Fa-f]+)", line)
        if m:
            sel.setdefault(int(m.group(1), 16) & 0xFFFFFFFF, []).append(line.rstrip("\n"))
    return {a: "\n".join(v) for a, v in sel.items()}


def env_key():
    return sorted("%s=%s" % (k, v) for k, v in os.environ.items() if k.startswith("WWHD_") and not ENV_NEUTRAL.match(k))


class Session:
    """one verify.py run's cache: keys for the run's functions, lookups, stores"""

    def __init__(self, unit, spec_lines, args, shards, build_info, cdir):
        self.cdir, self.unit = cdir, unit
        self.stats = {"hit": 0, "miss": 0, "fallback": 0}
        bi = build_info
        parts = [VERSION, "unit:" + unit, "args:" + " ".join(key_args(args, shards)), "env:" + " ".join(env_key()),
                 "compiler:" + compiler_id(), "flags:" + " ".join(bi["flags"]), "srcopt:%s/%s" % (bi["srcopt"], bi["only"]),
                 "tools:" + tree_sha(HERE, skip=("units", "recordings")),
                 "gen:" + gen_fingerprint(ROOT), "spec:" + spec_text(spec_lines)]
        for o in bi["fixed_objs"]:  # unit.c.o (originals, real callees, setups), espresso_fp, harness, support
            parts.append("obj:%s:%s" % (os.path.basename(o), file_sha(o)))
        shared = [os.path.join(ROOT, "build", "verify", f) for f in ("image.bin", "livein.tsv", "frames.tsv")]
        shared += sorted(os.path.join(ROOT, "build", f) for f in os.listdir(os.path.join(ROOT, "build")) if f.endswith(".tsv"))
        for p in shared:
            parts.append("shared:%s:%s" % (os.path.basename(p), big_file_sha(p, cdir) if os.path.exists(p) else "-"))
        for line in spec_lines:  # blob FUNC|* DST FILE OFFSET SIZE: the game file's bytes
            f = line.split("#")[0].split()
            if len(f) >= 4 and f[0] == "blob":
                p = os.path.join(ROOT, "game", f[3])
                parts.append("blob:%s:%s" % (f[3], big_file_sha(p, cdir) if os.path.exists(p) else "-"))
        self.unit_key = sha(*parts)
        self.rec = rec_dir(args)
        self.whole = sha(*["src:%s:%s" % (os.path.basename(o), file_sha(o)) for o in bi["src_objs"]])
        self.closure = None
        # `osmem ... threads` (2026-11-02): a guest thread, AX callback or alarm callback runs candidate code of any unit function
        # in any row (harness gthr::unit_fn), outside the closure of the row's own entry thunk: keys cover all unit sources
        thread_spec = any(f[:1] == ["osmem"] and "threads" in f for f in (l.split("#")[0].split() for l in spec_lines))
        if thread_spec:
            self.note("vcache: the spec declares `osmem ... threads`; keys cover all unit sources")
            return
        try:
            self.closure = Closure(bi["src_objs"], bi["linkmap"])
        except (Uncertain, OSError, ValueError, struct.error, IndexError) as e:
            self.note("vcache: object reader: %s; keys cover all unit sources" % e)

    def note(self, msg):
        print(msg, file=sys.stderr, flush=True)

    def func_key(self, addr, vmaddr, runsel=None):
        code = None
        if self.closure is not None:
            try:
                code = self.closure.function(vmaddr)
            except (Uncertain, ValueError, struct.error, IndexError, KeyError) as e:
                self.note("vcache: %08X: %s; its key covers all unit sources" % (addr, e))
        if code is None:
            self.stats["fallback"] += 1
            code = "whole:" + self.whole
        rec = "-"
        if self.rec:
            d = os.path.join(self.rec if os.path.isabs(self.rec) else os.path.join(ROOT, self.rec), "%08X" % addr)
            if os.path.isdir(d):
                rec = sha(*[x for f in sorted(os.listdir(d)) if ".tap" in f for x in (f, file_sha(os.path.join(d, f)))])
        return sha(self.unit_key, "%08X" % addr, code, rec, "runlist:" + (runsel or "-"))

    def path(self, key):
        return os.path.join(self.cdir, key[:2], key + ".json")

    def get(self, key):
        p = self.path(key)
        try:
            e = json.load(open(p))
        except (OSError, ValueError):
            return None
        if e.get("version") != VERSION or e.get("key") != key:
            return None
        try:
            os.utime(p)  # LRU: a hit is a use
        except OSError:
            pass
        return e

    def put(self, key, addr, block, notes, ok, dsiout=None):
        p = self.path(key)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        tmp = "%s.%d.tmp" % (p, os.getpid())
        try:
            json.dump({"version": VERSION, "key": key, "unit": self.unit, "addr": "%08X" % addr, "stdout": block,
                       "stderr": notes, "ok": ok, "dsiout": dsiout, "time": time.time()}, open(tmp, "w"))
            os.replace(tmp, p)
        except OSError as e:
            self.note("vcache: cannot store %08X: %s" % (addr, e))

    def evict(self):
        cap = int(float(os.environ.get("WWHD_VERIFY_CACHE_MAX_MB") or 256) * (1 << 20))
        ents, total = [], 0
        for dp, _dns, fns in os.walk(self.cdir):
            for f in fns:
                if f.endswith(".json") and f != "filememo.json":
                    p = os.path.join(dp, f)
                    try:
                        st = os.stat(p)
                    except OSError:
                        continue
                    ents.append((st.st_mtime, st.st_size, p))
                    total += st.st_size
        if total <= cap:
            return
        ents.sort()
        for _t, sz, p in ents:
            if total <= cap * 9 // 10:
                break
            try:
                os.remove(p)
                total -= sz
            except OSError:
                pass


class ResumeStore(Session):
    """verify.py --resume DIR: this gate campaign's own
    store, also in archive mode. Each harness process stores a function's part (its stdout lines between the marks, its
    stderr notes, its -dsiout records) as soon as that function is done in that process: one part per shard (k = 1 for an
    unsharded run), written atomically (tmp + rename) with the function's full key, the shard index, the shard count and
    the input range, and a SHA-256 over the part's content. A rerun with the same DIR and key takes a function from DIR
    only when every one of its k parts is present and intact (key, fields and hash checked); anything else is a miss and
    runs. The key is vcache's key (compiled code closure, unit objects, harness/support objects, expanded spec, tools,
    gen, shared tables, compiler, flags, result-affecting arguments incl. the shard count, every result-affecting WWHD_*
    variable such as WWHD_VERIFY_HANG_LIMIT) plus the linked harness executable's own hash. Nothing in DIR is evicted
    or removed."""
    RESUME_VERSION = "resume-1"

    def __init__(self, unit, spec_lines, args, shards, build_info, cdir, exe):
        os.makedirs(cdir, exist_ok=True)
        Session.__init__(self, unit, spec_lines, args, shards, build_info, cdir)
        self.unit_key = sha(self.unit_key, self.RESUME_VERSION, "exe:" + file_sha(exe))

    def part_path(self, key, i, k, lo, hi):
        return os.path.join(self.cdir, key[:2], "%s.p%dof%d.%d-%d.json" % (key, i, k, lo, hi))

    @staticmethod
    def part_sha(key, addr, i, k, lo, hi, chunk, notes, dsi):
        return sha(ResumeStore.RESUME_VERSION, key, "%08X" % addr, "%d/%d" % (i, k), "%d:%d" % (lo, hi), chunk,
                   json.dumps(notes), json.dumps(dsi))

    def put_part(self, key, addr, i, k, lo, hi, chunk, notes, dsi):
        p = self.part_path(key, i, k, lo, hi)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        tmp = "%s.%d.tmp" % (p, os.getpid())
        try:
            with open(tmp, "w") as f:
                json.dump({"version": self.RESUME_VERSION, "key": key, "unit": self.unit, "addr": "%08X" % addr,
                           "shard": i, "shards": k, "range": [lo, hi], "stdout": chunk, "stderr": notes, "dsiout": dsi,
                           "sha": self.part_sha(key, addr, i, k, lo, hi, chunk, notes, dsi), "time": time.time()}, f)
                f.flush()
                os.fsync(f.fileno())
            os.replace(tmp, p)
        except OSError as e:
            self.note("resume: cannot store %08X part %d/%d: %s" % (addr, i, k, e))

    def get_part(self, key, addr, i, k, lo, hi):
        """the part's (stdout, notes, dsi) when present and intact, else None (a partial/corrupt file is a miss)"""
        try:
            e = json.load(open(self.part_path(key, i, k, lo, hi)))
        except (OSError, ValueError):
            return None
        try:
            if (e["version"] != self.RESUME_VERSION or e["key"] != key or e["addr"] != "%08X" % addr or e["shard"] != i
                    or e["shards"] != k or e["range"] != [lo, hi]
                    or e["sha"] != self.part_sha(key, addr, i, k, lo, hi, e["stdout"], e["stderr"], e["dsiout"])):
                return None
        except (KeyError, TypeError):
            return None
        return e["stdout"], e["stderr"], e["dsiout"]

    def get(self, key):  # full vcache entries are never read from a resume store (parts only)
        return None

    def put(self, key, addr, block, notes, ok, dsiout=None):  # parts are stored as they finish (put_part)
        return None

    def evict(self):  # the campaign's parts stay
        return None


def row_ok(block, addr, result_line):
    """the function's status from its stdout block: True/False, or None when the block has no single result row"""
    rows = [result_line.match(l) for l in block.splitlines()]
    rows = [m for m in rows if m and int(m.group(1), 16) == addr]
    if len(rows) != 1:
        return None
    st = rows[0].group(9).strip()
    return st == "ok" or st == "ok (some inputs aborted on both sides)"
