#!/usr/bin/env python3
"""refcache.py: the reference-side cache's key, shard join and gate manifest (2026-10-30).

The harness writes the original's result for every generated input of a function (-refcache-write DIR -refcache-key K,
file DIR/ADDR.ref; harness.cpp namespace refc, RFC4 header (RFD1 for digest records, -refcache-digest, 2026-10-31) with
the key, n, seed, function, block count, comparison switches and constant dictionary). The container (header, records,
offset table) is the same for both formats, so join() handles either. This module holds what the Python tools share about it:

- key(): the configuration key mutate.py has always computed for a campaign (sha256 of the objects of unit.c, the FP
  runtime, the harness and the support sources, the expanded spec, the harness options of the campaign and the
  recordings), plus (2026-10-30) a hash of the verification tools' own Python sources. harness_args() builds those
  options exactly as mutate.py's harness command does, so another tool (nulltrap_compare.py) computes the same key for
  the same configuration.
- join_parts(): a sharded verify.py run (-range shards) writes one part per shard and function (ADDR.ref.LO-HI, the
  inputs LO..HI-1); join_parts() checks the parts (identical headers, offsets only inside each part's range, the shard
  ranges cover 0..n-1) and joins them into ADDR.ref, byte-identical to the file an unsharded run writes for the same
  records; joined records over WWHD_REFCACHE_MAX_MB (default 1024, the harness writer's cap on header + records) are
  dropped as an unsharded writer drops them (2026-10-30: four shards each under the cap joined to 1.4 GB for AGL#19
  027D55F8, which every mutant run would have loaded). Anything else leaves no ADDR.ref for that function (readers then
  run its original live).
- join_val_parts(): the same for the setup-snapshot cache's validation states of a sharded writing run
  (ADDR-TAG-sSEED.val.LO-HI parts, 2026-10-31); join_shard_dir() does both.
- the gate manifest (refcache.json), written by `nulltrap_compare.py --refcache-out DIR` next to the -nulltrap side's
  records, and gate_dir(), which mutate.py --refcache-from uses to accept a directory only when the manifest's key, n
  and seed equal the replay's own. A mismatch (other sources, spec, options, tools, n or seed) means the replay runs its
  own unmutated baseline as before.
Self-test: refcache_reuse_test.py.
"""
import hashlib
import json
import os
import re
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
MANIFEST = "refcache.json"
PART = re.compile(r"^([0-9A-F]{8})\.ref\.(\d+)-(\d+)$")


def harness_args(spec, n, seed, hdr_extra, nulltrap, observe_pointees, scope, rec):
    """the harness options of a mutate.py campaign run without the executable, in mutate.py's order (its harness_cmd):
    -spec, -n, -seed, the spec header's options, -nulltrap, -observe-pointees, -only SCOPE (a set of addresses), -rec"""
    cmd = ["-spec", spec, "-n", str(n), "-seed", str(seed)] + list(hdr_extra)
    if nulltrap and "-nulltrap" not in cmd:
        cmd.append("-nulltrap")
    if observe_pointees and "-observe-pointees" not in cmd:
        cmd.append("-observe-pointees")
    if scope is not None:
        cmd += ["-only", ",".join("%08X" % x for x in sorted(scope))]
    if rec:
        cmd += ["-rec", rec]
    return cmd


def tools_hash():
    """sha256 of the verification tools' Python sources (tools/verify/*.py without the self-tests): a record made by
    other tool code is never reused"""
    h = hashlib.sha256()
    for fn in sorted(os.listdir(HERE)):
        if fn.endswith(".py") and not fn.endswith("_test.py"):
            with open(os.path.join(HERE, fn), "rb") as fh:
                h.update(fn.encode() + b"\0" + hashlib.sha256(fh.read()).digest())
    return h.hexdigest()


def key(out, spec, args, rec, root, support_objs):
    """the campaign key (32 hex digits): out = the unit's build directory (unit.c.o, espresso_fp.c.o, harness.cpp.o),
    spec = the expanded spec, args = harness_args(...), rec = the recordings directory or None"""
    h = hashlib.sha256()
    m = os.path.join(out, "unit.parts")  # a split unit (verify.py unitsplit): its part objects instead of unit.c.o
    uobjs = [x + ".o" for x in open(m).read().split()] if os.path.exists(m) else ["unit.c.o"]
    side = [os.path.join(out, x) for x in uobjs + ["espresso_fp.c.o", "harness.cpp.o"]] + list(support_objs) + [spec]
    for f in side:
        h.update(f.encode() + b"\0")
        with open(f, "rb") as fh:
            h.update(hashlib.sha256(fh.read()).digest())
    h.update(repr(list(args)).encode())
    if rec and os.path.isdir(os.path.join(root, rec)):
        for dp, _, fs in sorted(os.walk(os.path.join(root, rec))):
            for fn in sorted(fs):
                with open(os.path.join(dp, fn), "rb") as fh:
                    h.update(fn.encode() + hashlib.sha256(fh.read()).digest())
    h.update(b"tools\0" + tools_hash().encode())
    return h.hexdigest()[:32]


def _split(data):
    """(header bytes incl. its u64 length, records region start, table offset, n) of one record file, or None"""
    if len(data) < 24:
        return None
    hl = struct.unpack_from("<Q", data, 0)[0]
    tab = struct.unpack_from("<Q", data, len(data) - 8)[0]
    if 8 + hl > tab or (len(data) - 8 - tab) % 8:
        return None
    return data[:8 + hl], 8 + hl, tab, (len(data) - 8 - tab) // 8


def join(parts, n):
    """parts: [(lo, hi, bytes)] of one function; the joined file's bytes, or (None, reason)"""
    parts = sorted(parts)
    if not parts or parts[0][0] != 0 or parts[-1][1] != n or any(a[1] != b[0] for a, b in zip(parts, parts[1:])):
        return None, "the parts do not cover inputs 0..%d once (%s)" % (n - 1, " ".join("%d-%d" % p[:2] for p in parts))
    head = None
    body, table = [], [0] * n
    pos = None
    for lo, hi, data in parts:
        s = _split(data)
        if s is None:
            return None, "part %d-%d is truncated" % (lo, hi)
        h, start, tab, pn = s
        if pn != n:
            return None, "part %d-%d has %d inputs, not %d" % (lo, hi, pn, n)
        if head is None:
            head, pos = h, len(h)
        elif h != head:
            return None, "part %d-%d has another header (key, n, seed, block count, switches or dictionary)" % (lo, hi)
        offs = struct.unpack_from("<%dQ" % n, data, tab)
        for i, o in enumerate(offs):
            if o and not lo <= i < hi:
                return None, "part %d-%d has a record for input %d" % (lo, hi, i)
            if o and not start <= o < tab:
                return None, "part %d-%d: record offset of input %d outside its records" % (lo, hi, i)
            if o:
                table[i] = o - start + pos
        body.append(data[start:tab])
        pos += tab - start
    cap = int(float(os.environ.get("WWHD_REFCACHE_MAX_MB") or 1024) * (1 << 20))
    if pos > cap:  # the harness's own cap on header + records (Writer::add): an unsharded writer drops this function too
        return None, "the joined records pass WWHD_REFCACHE_MAX_MB (%d MB); this function runs its original live" % (cap >> 20)
    out = head + b"".join(body)
    return out + struct.pack("<%dQ" % n, *table) + struct.pack("<Q", len(out)), None


def join_parts(d, n):
    """join every function's shard parts in d into ADDR.ref and remove the parts; returns {addr: reason} for functions
    whose parts could not be joined (no ADDR.ref is written for them)"""
    groups, bad = {}, {}
    try:
        names = os.listdir(d)
    except OSError:
        return bad
    for fn in names:
        m = PART.match(fn)
        if m:
            groups.setdefault(m.group(1), []).append((int(m.group(2)), int(m.group(3)), fn))
    for addr, ps in sorted(groups.items()):
        data = []
        for lo, hi, fn in ps:
            with open(os.path.join(d, fn), "rb") as fh:
                data.append((lo, hi, fh.read()))
        out, why = join(data, n)
        if out is None:
            bad[addr] = why
        else:
            tmp = os.path.join(d, "%s.ref.join%d" % (addr, os.getpid()))
            with open(tmp, "wb") as fh:
                fh.write(out)
            os.replace(tmp, os.path.join(d, addr + ".ref"))
        for _, _, fn in ps:  # the parts are this run's own temporaries
            os.remove(os.path.join(d, fn))
    return bad


VAL_PART = re.compile(r"^(.+-s\d+\.val)\.(\d+)-(\d+)$")


def _val_split(data):
    """(header bytes incl. its u64 length, entry count, entries bytes) of a .val file (harness.cpp snapc::ValWriter: u64
    header length, header, u64 n, n x (u32 input, u64 hash, u64 length, payload)), or None when it does not parse"""
    if len(data) < 16:
        return None
    hl = struct.unpack_from("<Q", data, 0)[0]
    if 8 + hl + 8 > len(data):
        return None
    n = struct.unpack_from("<Q", data, 8 + hl)[0]
    p = 16 + hl
    for _ in range(n):
        if p + 20 > len(data):
            return None
        ln = struct.unpack_from("<Q", data, p + 12)[0]
        p += 20 + ln
    if p != len(data):
        return None
    return data[:8 + hl], n, data[16 + hl:]


def join_val_parts(d):
    """join the setup-snapshot cache's per-shard validation-state parts (ADDR-TAG-sSEED.val.LO-HI, written by -range
    shards with -snapcache-write; 2026-10-31, fasttools2 change 3) into ADDR-TAG-sSEED.val: same header (key, seed,
    dictionary, snapshot hash) required, entries concatenated in shard order. Removes the parts; returns {file: reason}
    for those that could not be joined (no file is written for them: readers run those setups live)."""
    groups, bad = {}, {}
    try:
        names = os.listdir(d)
    except OSError:
        return bad
    for fn in names:
        m = VAL_PART.match(fn)
        if m:
            groups.setdefault(m.group(1), []).append((int(m.group(2)), int(m.group(3)), fn))
    for name, ps in sorted(groups.items()):
        head, n, body, why = None, 0, [], None
        for lo, hi, fn in sorted(ps):
            with open(os.path.join(d, fn), "rb") as fh:
                sp = _val_split(fh.read())
            if sp is None:
                why = "part %d-%d is truncated" % (lo, hi)
                break
            if head is not None and sp[0] != head:
                why = "part %d-%d has another header (key, seed, dictionary or snapshot)" % (lo, hi)
                break
            head = sp[0]
            n += sp[1]
            body.append(sp[2])
        if why:
            bad[name] = why
        else:
            tmp = os.path.join(d, "%s.join%d" % (name, os.getpid()))
            with open(tmp, "wb") as fh:
                fh.write(head + struct.pack("<Q", n) + b"".join(body))
            os.replace(tmp, os.path.join(d, name))
        for _, _, fn in ps:
            os.remove(os.path.join(d, fn))
    return bad


def join_shard_dir(d, n):
    """after a sharded run with -refcache-write/-snapcache-write into d: the record parts (join_parts) and the validation
    state parts (join_val_parts) joined; returns {name: reason} of what could not be joined"""
    bad = join_parts(d, n)
    bad.update(join_val_parts(d))
    return bad


def write_manifest(d, **kw):
    with open(os.path.join(d, MANIFEST + ".tmp"), "w") as fh:
        json.dump(kw, fh, indent=1, sort_keys=True)
    os.replace(os.path.join(d, MANIFEST + ".tmp"), os.path.join(d, MANIFEST))


def gate_dir(d, seed, spec):
    """the record directory for input seed `seed` under d (d/SPEC/s<seed>, d/s<seed> or d itself) and its manifest, or
    (None, reason); the manifest must name this spec"""
    for c in (os.path.join(d, spec, "s%d" % seed), os.path.join(d, "s%d" % seed), d):
        p = os.path.join(c, MANIFEST)
        if os.path.exists(p):
            try:
                m = json.load(open(p))
            except (OSError, ValueError) as e:
                return None, "unreadable %s (%s)" % (p, e)
            if not isinstance(m, dict) or m.get("spec") != spec:
                return None, "%s is not a manifest of %s" % (p, spec)
            return c, m
    return None, "no %s for %s seed %d under %s" % (MANIFEST, spec, seed, d)


def check_manifest(m, want_key, n, seed):
    """None when the gate's records are this run's configuration exactly, else the reason"""
    if not isinstance(m, dict):
        return "malformed manifest"
    for what, have, want in (("key", m.get("key"), want_key), ("n", m.get("n"), n), ("seed", m.get("seed"), seed),
                             ("-nulltrap", m.get("nulltrap"), True)):
        if have != want:
            return "%s differs (gate %s, this run %s)" % (what, have, want)
    return None
