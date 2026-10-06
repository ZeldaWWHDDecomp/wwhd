/* hd_res_load_task: HD ResLoadTask (asynchronous resource loading task), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 0260B770..0260F9E3; static initialiser 0260F7A8.
 *
 * ResLoadTask (0x1120, sead::Task, ctor 027454E0 "ResLoadTask"): singleton 101F4F54,
 * +0x20/+0x70 vtables, +0xC8 u8 instance flag,
 * +0xCC request ring {entries (0xA0 each), capacity, head, count},
 * +0xDC state (0 idle, 1 loading, 2 waiting), +0xE0 reference-count tree (sead TreeMap with a
 * 0x80-node pool; find 0260ACC8, insert 0260AD08), +0x10F4 id ring {u32[], capacity, head, count},
 * +0x1104 second request ring, +0x1114 current heap, +0x1118 u8 flush flag, +0x111C wait timer.
 * Request entry (0xA0): two FixedSafeString<64> (0x4C each: {top, vtable, 0x40, buf[0x40]}) at +0
 * (archive) and +0x4C (resource), +0x98 id, +0x9C u8 kind, +0x9D u8 stage-resource flag.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_res_load_task {

static const u32 kInstance = 0x101F4F54;
static const u32 kResMgr = 0x101F4F7C;
static const u32 kResCtrl = 0x101F4F28;
static const u32 kHeapMgr = 0x101F8B4C;
static const u32 kSafeStringVt = 0x100E1DD4;
static const u32 kFixed64Vt = 0x100E1E3C;

/* 0260B770: SingletonDisposer deleting destructor (clears the instance) */
void disposerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0260B770, void, p, flags);
    if (!p) return;
    if (load<u8>(p)) store<u32>(kInstance, 0);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0260B770, disposerDtor);

/* 0260B79C: destructor thunk of the second base (this - 0x14) */
void dtorThunk(u32 p, u32 flags) {
    WWHD_FUNC(0x0260B79C, void, p, flags);
    call<void>(0x0260F850, p - 0x14, flags);
}
VERIFY(0x0260B79C, dtorThunk);

/* 0260B7A4: registers the singleton instance */
void setInstance(u32 p) {
    WWHD_FUNC(0x0260B7A4, void, p);
    if (load<u32>(kInstance)) return;
    store<u32>(kInstance, p);
    store<u8>(p + 0xC8, 1);
}
VERIFY(0x0260B7A4, setInstance);

/* zeroes a 16-byte ring header (placement-new null check as GHS emits it) */
static void ringCtor(u32 q) {
    if (!q) q = call<u32>(0x0273AD10, 0x10u);
    if (q) {
        store<u32>(q + 4, 0);
        store<u32>(q + 8, 0);
        store<u32>(q, 0);
        store<u32>(q + 0xC, 0);
    }
}

/* 0260B7C4: constructor */
u32 ctor(u32 p, u32 arg) {
    WWHD_FUNC(0x0260B7C4, u32, p, arg);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x1120u);
        if (!p) return 0;
    }
    call<void>(0x027454E0, p, arg, 0x100E1EA4u);
    store<u8>(p + 0xC8, 0);
    store<u32>(p + 0x70, 0x100E1FD0);
    store<u32>(p + 0x20, 0x100E2090);
    ringCtor(p + 0xCC);
    store<u32>(p + 0xDC, 0);
    u32 t = p + 0xE0;
    if (!t) t = call<u32>(0x0273AD10, 0x1014u);
    if (t) {
        const u32 nodes = t + 0x14;
        store<u32>(t, 0);
        store<u32>(t + 0xC, 0);
        store<u32>(t + 4, nodes);
        store<u32>(t + 0x10, 0x80);
        for (u32 i = 0; i < 0x7F; i++) store<u32>(nodes + 0x20 * i, nodes + 0x20 * (i + 1));
        store<u32>(nodes + 0xFE0, 0);
        store<u32>(t + 8, nodes);
    }
    ringCtor(p + 0x10F4);
    ringCtor(p + 0x1104);
    store<u8>(p + 0x1118, 0);
    store<u32>(p + 0x1114, 0);
    store<u32>(p + 0x111C, 0);
    return p;
}
VERIFY(0x0260B7C4, ctor);

/* sead::FixedSafeString<64> constructor, inlined */
static void fixed64Ctor(u32 s) {
    u32 q = s;
    if (!q) q = call<u32>(0x0273AD10, 0xCu);
    if (q) {
        store<u32>(q + 4, 0x100E1DEC);
        store<u32>(q + 8, 0x40);
        store<u32>(q, s + 0xC);
        store<u8>(s + 0x4B, 0);
    }
    const u32 top = load<u32>(s);
    store<u32>(s + 4, 0x100E1E24);
    store<u8>(top, 0);
    store<u32>(s + 4, kFixed64Vt);
}

/* builds 0x80 request entries in a 0x5000-byte block */
static void entriesCtor(u32 base) {
    for (u32 i = 0; i < 0x80; i++) {
        const u32 e = base + 0xA0 * i;
        if (!e) continue;
        u32 s = e;
        if (!s) s = call<u32>(0x0273AD10, 0x4Cu);
        if (s) fixed64Ctor(s);
        s = e + 0x4C;
        if (!s) s = call<u32>(0x0273AD10, 0x4Cu);
        if (s) fixed64Ctor(s);
        store<u8>(e + 0x9C, 0);
        store<u32>(e + 0x98, 0);
        store<u8>(e + 0x9D, 0);
    }
}

static u32 heapAlloc(u32 heap, u32 size) {
    const u32 h = heap ? heap : call<u32>(0x02756140, load<u32>(kHeapMgr));
    return call_ptr<u32>(load<u32>(load<u32>(h + 0xC) + 0x34), h, size, 4u);
}

static void setStr(u32 s, u32 top) {
    store<u32>(s + 4, kSafeStringVt);
    store<u32>(s, top);
}

/* 0260B91C: prepare: permanent packs, particle/program-texture/jpeg archives, font and message
 * managers, the task heap and its request rings */
void prepare(u32 self) {
    WWHD_FUNC(0x0260B91C, void, self);
    Local<u8[0x40]> F;
    const u32 f = F.a;
    const u32 a = f + 8, b = f + 0x10, e = f + 0x18, c = f + 0x20, d = f + 0x28, g = f + 0x30, h = f + 0x38;
    call<void>(0x027488E8, self);
    setStr(a, 0x100E1F40);
    call<void>(0x02610CF8, load<u32>(kResMgr), a, load<u32>(load<u32>(kResCtrl) + 0x2128));
    store<u32>(a, 0x100E1EC4);
    store<u32>(a + 4, kSafeStringVt);
    call<void>(0x02611864, load<u32>(kResMgr), a, load<u32>(load<u32>(kResCtrl) + 0x2128));
    u32 mgr = load<u32>(kResMgr);
    const u32 pack = call<u32>(0x02612BE0, mgr);
    call<void>(0x02610CF8, mgr, pack, load<u32>(load<u32>(0x101F7274) + 0x14));
    const u32 parent = load<u32>(load<u32>(kResCtrl) + 0x2040);
    setStr(a, 0x100E1F58);
    u32 heap = call<u32>(0x02754B38, 0u, a, parent, 1u, 0u);
    mgr = load<u32>(kResMgr);
    store<u32>(a + 4, kSafeStringVt);
    store<u32>(b + 4, kSafeStringVt);
    store<u32>(a, 0x100E1F58);
    store<u32>(b, 0x100E1EE4);
    call<void>(0x02612E64, mgr, a, b, heap, 1u, 0x40000u, 0x2000u);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    setStr(b, 0x100E1F64);
    heap = call<u32>(0x02754B38, 0u, b, parent, 1u, 0u);
    mgr = load<u32>(kResMgr);
    store<u32>(b + 4, kSafeStringVt);
    store<u32>(a + 4, kSafeStringVt);
    store<u32>(b, 0x100E1F64);
    store<u32>(a, 0x100E1EFC);
    call<void>(0x02612E64, mgr, b, a, heap, 1u, 0x40000u, 0x2000u);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    setStr(c, 0x100E1F74);
    u32 fh = call<u32>(0x02753004, 0x1000u, c, load<u32>(load<u32>(0x101F7274) + 0x14), 1u, 0u);
    call<void>(0x025F2FE0, fh);
    call<void>(0x025F33A4, load<u32>(0x101F4A50), fh);
    setStr(d, 0x100E1F20);
    fh = call<u32>(0x02753004, 0x20000u, d, load<u32>(load<u32>(0x101F7274) + 0x14), 1u, 0u);
    call<void>(0x025F3A14, fh);
    call<void>(0x025F4A60, load<u32>(0x101F4AE8), fh);
    call<void>(0x027199CC, load<u32>(0x101F8378));
    setStr(e, 0x100E1F30);
    u32 p2 = call<u32>(0x0203E8D0);
    heap = call<u32>(0x02754B38, 0u, e, p2, 1u, 0u);
    mgr = load<u32>(kResMgr);
    store<u32>(e + 4, kSafeStringVt);
    store<u32>(e, 0x100E1EB0);
    store<u32>(g, 0x100E1F80);
    store<u32>(g + 4, kSafeStringVt);
    call<void>(0x02612E64, mgr, e, g, heap, 0u, 0x40000u, 0x2000u);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    setStr(h, 0x100E1EB8);
    p2 = call<u32>(0x0203E8F4);
    heap = call<u32>(0x02754B38, 0u, h, p2, 1u, 1u);
    u32 ring = heapAlloc(heap, 0x5000);
    entriesCtor(ring);
    if (ring) {
        store<u32>(self + 0xCC, ring);
        store<u32>(self + 0xD4, 0);
        store<u32>(self + 0xD0, 0x80);
        store<u32>(self + 0xD8, 0);
    }
    ring = heapAlloc(heap, 0x200);
    if (ring) {
        store<u32>(self + 0x10F4, ring);
        store<u32>(self + 0x10FC, 0);
        store<u32>(self + 0x10F8, 0x80);
        store<u32>(self + 0x1100, 0);
    }
    ring = heapAlloc(heap, 0x5000);
    entriesCtor(ring);
    if (ring) {
        store<u32>(self + 0x1104, ring);
        store<u32>(self + 0x110C, 0);
        store<u32>(self + 0x1108, 0x80);
        store<u32>(self + 0x1110, 0);
    }
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    store<u32>(self + 0xD4, 0);
    store<u32>(self + 0x1110, 0);
    store<u32>(self + 0x110C, 0);
    store<u32>(self + 0x1100, 0);
    store<u32>(self + 0xD8, 0);
    store<u32>(self + 0x10FC, 0);
    call<void>(0x02031048);
    call<void>(0x0203106C);
    setInstance(self);
}
VERIFY(0x0260B91C, prepare);

/* 0260BEF0: back to idle */
void setIdle(u32 self) {
    WWHD_FUNC(0x0260BEF0, void, self);
    store<u32>(self + 0xDC, 0);
}
VERIFY(0x0260BEF0, setIdle);

/* 0260BEFC: releases one reference of a request's resource; unloads it at zero */
u32 release(u32 self, u32 entry) {
    WWHD_FUNC(0x0260BEFC, u32, self, entry);
    const u32 st = load<u32>(self + 0xDC);
    if (st == 1 || st == 2) return 0;
    Local<u32> key;
    store<u32>(key.a, load<u32>(entry + 0x98));
    const u32 node = call<u32>(0x0260ACC8, self + 0xE0, load<u32>(self + 0xE0), key.a);
    if (!node) return 0;
    const u32 rc = node + 0x18;
    if (!rc) return 0;
    u32 n = load<u32>(rc);
    if (!n) return 1;
    n--;
    store<u32>(rc, n);
    if (n) return 1;
    const u32 ctrl = load<u32>(kResCtrl);
    if (load<u8>(entry + 0x9C)) call<void>(0x02607138, ctrl, entry);
    else call<void>(0x0260757C, ctrl, entry + 0x4C, entry);
    return 1;
}
VERIFY(0x0260BEFC, release);

/* 0260BFC8: unload request: a stage resource ("<archive>_<name>") or a reference release */
u32 unload(u32 self, u32 entry) {
    WWHD_FUNC(0x0260BFC8, u32, self, entry);
    if (!load<u8>(entry + 0x9D)) return release(self, entry);
    Local<u8[0x90]> F;
    const u32 s = F.a;
    store<u32>(s + 8, 0x80);
    store<u32>(s, s + 0xC);
    store<u8>(s + 0xC, 0);
    store<u32>(s + 4, 0x100E1E6C);
    store<u8>(s + 0x8B, 0);
    call_ptr<void>(load<u32>(load<u32>(entry + 4) + 0x14), entry);
    const u32 fn = load<u32>(load<u32>(entry + 0x50) + 0x14);
    const u32 arc = load<u32>(entry);
    call_ptr<void>(fn, entry + 0x4C);
    call<void>(0x02759C28, s, 0x100E1F90u, arc, load<u32>(entry + 0x4C));
    return call<u32>(0x026126C0, load<u32>(kResMgr), s);
}
VERIFY(0x0260BFC8, unload);

/* strlen capped at 0x40000 as sead does */
static s32 cstrLength(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}

/* copies the string object src into the buffered string dst (dst buffer loaded first) */
static void copyString(u32 dstObj, u32 src) {
    const u32 dst = load<u32>(dstObj);
    call_ptr<void>(load<u32>(load<u32>(src + 4) + 0x14), src);
    s32 n = cstrLength(load<u32>(src));
    const s32 size = s32(load<u32>(dstObj + 8));
    const u32 fn = load<u32>(load<u32>(src + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, src);
    call<void>(0xC0009988, dst, load<u32>(src), u32(n), 0u);
    store<u8>(dst + n, 0);
}

/* 0260C0A4: copies the front request of the ring into out (not removed) */
u32 peekRequest(u32 self, u32 out) {
    WWHD_FUNC(0x0260C0A4, u32, self, out);
    const s32 count = s32(load<u32>(self + 0xD8));
    if (!count) return 0;
    u32 e;
    if (count > 0) {
        s32 i = s32(load<u32>(self + 0xD4));
        const s32 cap = s32(load<u32>(self + 0xD0));
        const u32 base = load<u32>(self + 0xCC);
        if (i >= cap) i -= cap;
        e = base + u32(i) * 0xA0;
    } else {
        e = load<u32>(self + 0xCC);
    }
    copyString(out, e);
    copyString(out + 0x4C, e + 0x4C);
    store<u32>(out + 0x98, load<u32>(e + 0x98));
    store<u8>(out + 0x9C, load<u8>(e + 0x9C));
    store<u8>(out + 0x9D, load<u8>(e + 0x9D));
    return 1;
}
VERIFY(0x0260C0A4, peekRequest);

/* 0260C290: finishes a request (archive load or resource registration) */
u32 finish(u32 self, u32 entry) {
    WWHD_FUNC(0x0260C290, u32, self, entry);
    const u32 ctrl = load<u32>(kResCtrl);
    const u32 heap = load<u32>(self + 0x1114);
    if (load<u8>(entry + 0x9C)) call<void>(0x02604EB4, ctrl, entry, heap);
    else call<void>(0x0260623C, ctrl, entry + 0x4C, entry, heap);
    return 1;
}
VERIFY(0x0260C290, finish);

/* an empty request entry on the stack */
static void localEntry(u32 e) {
    store<u32>(e + 0x98, 0);
    store<u32>(e, e + 0xC);
    store<u32>(e + 0x54, 0x40);
    store<u8>(e + 0x9C, 0);
    store<u32>(e + 0x4C, e + 0x58);
    store<u8>(e + 0xC, 0);
    store<u8>(e + 0x4B, 0);
    store<u8>(e + 0x97, 0);
    store<u32>(e + 8, 0x40);
    store<u8>(e + 0x58, 0);
    store<u32>(e + 4, kFixed64Vt);
    store<u32>(e + 0x50, kFixed64Vt);
    store<u8>(e + 0x9D, 0);
}

/* 0260C2EC: request done: remembers its id, pops it and starts the next one */
u32 next(u32 self, u32 entry, u32 retry) {
    WWHD_FUNC(0x0260C2EC, u32, self, entry, retry);
    if (!load<u8>(entry + 0x9D)) {
        const s32 n = s32(load<u32>(self + 0x1100));
        if (n < s32(load<u32>(self + 0x10F8))) {
            const s32 head = s32(load<u32>(self + 0x10FC));
            const s32 cap = s32(load<u32>(self + 0x10F8));
            s32 i = head + n;
            store<u32>(self + 0x1100, u32(n + 1));
            const u32 id = load<u32>(entry + 0x98);
            const u32 ids = load<u32>(self + 0x10F4);
            if (i >= cap) i -= cap;
            store<u32>(ids + 4 * u32(i), id);
        }
    }
    if (s32(load<u32>(self + 0xD8)) > 0) {
        const s32 h = s32(load<u32>(self + 0xD4)) + 1;
        store<u32>(self + 0xD4, u32(h));
        const s32 cap = s32(load<u32>(self + 0xD0));
        const u32 c = load<u32>(self + 0xD8);
        if (h >= cap) store<u32>(self + 0xD4, 0);
        store<u32>(self + 0xD8, c - 1);
    }
    Local<u8[0xA0]> E;
    localEntry(E.a);
    if (call<u32>(0x0260C0A4, self, E.a)) return call<u32>(0x0260C4CC, self, E.a, retry);
    store<u32>(self + 0xDC, 0);
    return 0;
}
VERIFY(0x0260C2EC, next);

/* 0260C43C: retry after a failed archive load */
void retryLoad(u32 self, u32 entry, u32 retry) {
    WWHD_FUNC(0x0260C43C, void, self, entry, retry);
    if (!call<u32>(0x026105C4, load<u32>(kResMgr), entry, load<u32>(self + 0x1114))) return;
    const u32 heap = load<u32>(self + 0x1114);
    if (heap) call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    finish(self, entry);
    next(self, entry, retry - 1);
}
VERIFY(0x0260C43C, retryLoad);

/* 0260C4CC: starts loading a request */
u32 startLoad(u32 self, u32 entry, u32 retry) {
    WWHD_FUNC(0x0260C4CC, u32, self, entry, retry);
    store<u32>(self + 0xDC, 1);
    if (call<u32>(0x02612504, load<u32>(kResMgr), entry)) {
        store<u32>(self + 0x1114, 0);
        return 1;
    }
    const u32 parent = call<u32>(0x02607F80, load<u32>(kResCtrl), entry + 0x4C, entry);
    call_ptr<void>(load<u32>(load<u32>(entry + 4) + 0x14), entry);
    Local<u32[2]> name;
    store<u32>(name.a + 4, kSafeStringVt);
    store<u32>(name.a, load<u32>(entry));
    store<u32>(self + 0x1114, call<u32>(0x02754B38, 0u, name.a, parent, 1u, 0u));
    const s32 range = s32(load<s16>(0x1047C768)) + 1;
    const u32 r = call<u32>(0x0275CEE4, load<u32>(0x101F8B60));
    const u32 wait = u32((u64(r) * u64(u32(range))) >> 32);
    store<u32>(self + 0x111C, wait + u32(s32(load<s16>(0x1047C76A))));
    if (load<u8>(entry + 0x9D))
        return call<u32>(0x02612AE8, load<u32>(kResMgr), entry, entry + 0x4C, load<u32>(self + 0x1114));
    if (!load<u8>(entry + 0x9C))
        return call<u32>(0x02612908, load<u32>(kResMgr), entry, entry + 0x4C, load<u32>(self + 0x1114));
    if (retry && call<u32>(0x02612540, load<u32>(kResMgr), entry)) {
        retryLoad(self, entry, retry);
        return 1;
    }
    return call<u32>(0x026127D4, load<u32>(kResMgr), entry, load<u32>(self + 0x1114));
}
VERIFY(0x0260C4CC, startLoad);

/* 0260C67C: waits out the timer and the resource manager, then finishes the request */
u32 waitLoad(u32 self, u32 entry) {
    WWHD_FUNC(0x0260C67C, u32, self, entry);
    const s32 t = s32(load<u32>(self + 0x111C));
    if (t > 0) {
        store<u32>(self + 0x111C, u32(t - 1));
        return 0;
    }
    if (load<u32>(load<u32>(kResMgr) + 0x11248) == 1) return 0;
    const u32 heap = load<u32>(self + 0x1114);
    if (heap) call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
    if (!load<u8>(entry + 0x9D)) finish(self, entry);
    return 1;
}
VERIFY(0x0260C67C, waitLoad);

/* 0260C74C: per-frame update: flushes the unload ring when idle, else advances the request ring */
void calc(u32 self) {
    WWHD_FUNC(0x0260C74C, void, self);
    if (!load<u32>(kResCtrl)) return;
    if ((load<u8>(self + 0x1118) || !load<u32>(self + 0xD8)) && !load<u32>(self + 0xDC)) {
        for (;;) {
            s32 n = s32(load<u32>(self + 0x1110));
            if (!n) break;
            if (n > 0) {
                s32 i = s32(load<u32>(self + 0x110C));
                const s32 cap = s32(load<u32>(self + 0x1108));
                const u32 base = load<u32>(self + 0x1104);
                if (i >= cap) i -= cap;
                unload(self, base + u32(i) * 0xA0);
                n = s32(load<u32>(self + 0x1110));
            } else {
                unload(self, load<u32>(self + 0x1104));
                n = s32(load<u32>(self + 0x1110));
            }
            while (n <= 0) {
                if (!n) goto done;
                unload(self, load<u32>(self + 0x1104));
                n = s32(load<u32>(self + 0x1110));
            }
            const s32 h = s32(load<u32>(self + 0x110C)) + 1;
            store<u32>(self + 0x110C, u32(h));
            const s32 cap = s32(load<u32>(self + 0x1108));
            const u32 c = load<u32>(self + 0x1110);
            if (h >= cap) store<u32>(self + 0x110C, 0);
            store<u32>(self + 0x1110, c - 1);
        }
    done:
        store<u8>(self + 0x1118, 0);
        return;
    }
    Local<u8[0xA0]> E;
    localEntry(E.a);
    if (!peekRequest(self, E.a)) return;
    const u32 st = load<u32>(self + 0xDC);
    if (!st) {
        startLoad(self, E.a, 1);
        return;
    }
    if ((st == 1 || st == 2) && waitLoad(self, E.a)) next(self, E.a, 1);
}
VERIFY(0x0260C74C, calc);

/* frees one ring's storage */
static void freeRing(u32 self, u32 off) {
    if (!load<u32>(self + off)) return;
    const u32 h = call<u32>(0x02755FEC, load<u32>(kHeapMgr), load<u32>(self + off));
    call_ptr<void>(load<u32>(load<u32>(h + 0xC) + 0x3C), h, load<u32>(self + off));
    store<u32>(self + off + 4, 0);
    store<u32>(self + off + 0xC, 0);
    store<u32>(self + off, 0);
    store<u32>(self + off + 8, 0);
}

/* 0260C940: frees the three rings */
void freeRings(u32 self) {
    WWHD_FUNC(0x0260C940, void, self);
    freeRing(self, 0xCC);
    freeRing(self, 0x10F4);
    freeRing(self, 0x1104);
}
VERIFY(0x0260C940, freeRings);

/* 0260CA40: adds a reference to a request's resource (1 when it was already referenced) */
u32 addRef(u32 self, u32 entry) {
    WWHD_FUNC(0x0260CA40, u32, self, entry);
    const u32 id = load<u32>(entry + 0x98);
    const u32 tree = self + 0xE0;
    const u32 root = load<u32>(tree);
    Local<u32> key;
    store<u32>(key.a, id);
    const u32 node = call<u32>(0x0260ACC8, tree, root, key.a);
    if (node && node + 0x18) {
        store<u32>(node + 0x18, load<u32>(node + 0x18) + 1);
        return 1;
    }
    if (s32(load<u32>(tree + 0xC)) < s32(load<u32>(tree + 0x10))) {
        const u32 n = load<u32>(tree + 4);
        if (n) store<u32>(tree + 4, load<u32>(n));
        if (n) {
            store<u32>(n, 0);
            store<u32>(n + 0x14, id);
            store<u8>(n + 8, 1);
            store<u32>(n + 0x10, 0x100E1E84);
            store<u32>(n + 0xC, id);
            store<u32>(n + 0x18, 1);
            store<u32>(n + 4, 0);
            store<u32>(n + 0x1C, tree);
        }
        const u32 cnt = load<u32>(tree + 0xC) + 1;
        const u32 r = load<u32>(tree);
        store<u32>(tree + 0xC, cnt);
        const u32 newRoot = call<u32>(0x0260AD08, tree, r, n);
        store<u32>(tree, newRoot);
        store<u8>(newRoot + 8, 0);
        return 0;
    }
    Local<u32> key2;
    store<u32>(key2.a, id);
    const u32 n2 = call<u32>(0x0260ACC8, tree, root, key2.a);
    if (n2 && n2 + 0x18) store<u32>(n2 + 0x18, 1);
    return 0;
}
VERIFY(0x0260CA40, addRef);

/* ---- request building and ring insertion (inlined in every request function) ---- */

/* copies a request entry (both strings, id, kind, flag) into a ring slot */
static void copyEntry(u32 slot, u32 src) {
    copyString(slot, src);
    copyString(slot + 0x4C, src + 0x4C);
    store<u32>(slot + 0x98, load<u32>(src + 0x98));
    store<u8>(slot + 0x9C, load<u8>(src + 0x9C));
    store<u8>(slot + 0x9D, load<u8>(src + 0x9D));
}

/* appends to the ring at off (request ring 0xCC or unload ring 0x1104) */
static void pushBack(u32 self, u32 off, u32 src) {
    const s32 cnt = s32(load<u32>(self + off + 0xC));
    if (cnt >= s32(load<u32>(self + off + 4))) return;
    const s32 h = s32(load<u32>(self + off + 8));
    const s32 cap = s32(load<u32>(self + off + 4));
    s32 i = h + cnt;
    const u32 base = load<u32>(self + off);
    store<u32>(self + off + 0xC, u32(cnt + 1));
    if (i >= cap) i -= cap;
    copyEntry(base + u32(i) * 0xA0, src);
}

/* inserts at the front of the request ring */
static void pushFront(u32 self, u32 src) {
    const s32 h = s32(load<u32>(self + 0xD4)) - 1;
    const u32 c = load<u32>(self + 0xD8);
    store<u32>(self + 0xD4, u32(h));
    if (h < 0) store<u32>(self + 0xD4, load<u32>(self + 0xD0) - 1);
    s32 i = s32(load<u32>(self + 0xD4));
    const s32 cap = s32(load<u32>(self + 0xD0));
    store<u32>(self + 0xD8, c + 1);
    if (i >= cap) i -= cap;
    copyEntry(load<u32>(self + 0xCC) + u32(i) * 0xA0, src);
}

/* hash of a SafeString's text: sead::HashCRC32::calcHash(data, size) at 0273B264, which reads only
 * r3 and r4 (names.tsv calls it dRes_control_c::getResInfoLoaded; corrected in matcher_errors.tsv) */
static u32 hashString(u32 s) {
    call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s);
    const u32 fn = load<u32>(load<u32>(s + 4) + 0x14);
    const u32 top = load<u32>(s);
    call_ptr<void>(fn, s);
    const s32 len = cstrLength(load<u32>(s));
    return call<u32>(0x0273B264, top, len);
}

/* hash of "<arc>_<name>" formatted into the FixedSafeString<128> at s */
static u32 hashPair(u32 s, u32 fmt, u32 arc, u32 name) {
    store<u8>(s + 0x8B, 0);
    store<u8>(s + 0xC, 0);
    store<u32>(s + 8, 0x80);
    store<u32>(s, s + 0xC);
    store<u32>(s + 4, 0x100E1E6C);
    call_ptr<void>(load<u32>(load<u32>(arc + 4) + 0x14), arc);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 arcTop = load<u32>(arc);
    call_ptr<void>(fn, name);
    call<void>(0x02759C28, s, fmt, arcTop, load<u32>(name));
    return hashString(s);
}

/* first string of a stack request entry, copied from src */
static void entryFirst(u32 e, u32 src) {
    store<u8>(e + 0x4B, 0);
    store<u32>(e + 8, 0x40);
    store<u32>(e + 4, 0x100E1E24);
    store<u32>(e, e + 0xC);
    copyString(e, src);
}

/* second string of a stack request entry, copied from src */
static void entrySecond(u32 e, u32 src) {
    store<u32>(e + 0x50, 0x100E1E24);
    store<u8>(e + 0x97, 0);
    store<u32>(e + 4, kFixed64Vt);
    store<u32>(e + 0x4C, e + 0x58);
    store<u32>(e + 0x54, 0x40);
    copyString(e + 0x4C, src);
}

/* second string from the global empty-name SafeString 104A0CD8 (vtable read once) */
static void entrySecondGlobal(u32 e) {
    const u32 g = 0x104A0CD8;
    store<u8>(e + 0x97, 0);
    store<u32>(e + 4, kFixed64Vt);
    store<u32>(e + 0x4C, e + 0x58);
    store<u32>(e + 0x54, 0x40);
    store<u32>(e + 0x50, 0x100E1E24);
    const u32 vt = load<u32>(g + 4);
    call_ptr<void>(load<u32>(vt + 0x14), g);
    const u32 top = load<u32>(g);
    s32 n = cstrLength(top);
    const s32 size = s32(load<u32>(e + 0x54));
    const u32 fn = load<u32>(vt + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, g);
    call<void>(0xC0009988, e + 0x58, top, u32(n), 0u);
    store<u8>(e + 0x58 + n, 0);
}

static void entryTail(u32 e, u32 id, u32 kind, u32 flag) {
    store<u32>(e + 0x98, id);
    store<u8>(e + 0x9C, u8(kind));
    store<u32>(e + 0x50, kFixed64Vt);
    store<u8>(e + 0x9D, u8(flag));
}

/* reference-counted insertion: front requests go behind the request being loaded */
static void enqueue(u32 self, u32 L, u32 front) {
    if (!front) {
        pushBack(self, 0xCC, L);
        return;
    }
    s32 cnt = s32(load<u32>(self + 0xD8));
    Local<u8[0xA0]> TT;
    const u32 T = TT.a;
    localEntry(T);
    if (cnt > 0) {
        s32 i = s32(load<u32>(self + 0xD4));
        const s32 cap = s32(load<u32>(self + 0xD0));
        const u32 base = load<u32>(self + 0xCC);
        if (i >= cap) i -= cap;
        const u32 e = base + u32(i) * 0xA0;
        copyString(T, e);
        copyString(T + 0x4C, e + 0x4C);
        store<u32>(T + 0x98, load<u32>(e + 0x98));
        const s32 h = s32(load<u32>(self + 0xD4)) + 1;
        const s32 c2 = s32(load<u32>(self + 0xD0));
        store<u8>(T + 0x9C, load<u8>(e + 0x9C));
        store<u32>(self + 0xD4, u32(h));
        const s32 c = s32(load<u32>(self + 0xD8));
        store<u8>(T + 0x9D, load<u8>(e + 0x9D));
        if (h >= c2) store<u32>(self + 0xD4, 0);
        cnt = c - 1;
        store<u32>(self + 0xD8, u32(cnt));
    } else if (cnt == 0) {
        if (s32(load<u32>(self + 0xD0)) > 0) pushFront(self, L);
        return;
    }
    if (cnt >= s32(load<u32>(self + 0xD0))) return;
    pushFront(self, L);
    if (s32(load<u32>(self + 0xD8)) >= s32(load<u32>(self + 0xD0))) return;
    pushFront(self, T);
}

/* 0260CBB4: requests a resource archive by name (front: right behind the current request) */
u32 requestArchive(u32 self, u32 name, u32 front) {
    WWHD_FUNC(0x0260CBB4, u32, self, name, front);
    if (s32(load<u32>(self + 0xD8)) >= s32(load<u32>(self + 0xD0))) return 0;
    const u32 h = hashString(name);
    Local<u8[0xA0]> LL;
    const u32 L = LL.a;
    entryFirst(L, name);
    entrySecondGlobal(L);
    entryTail(L, h, 1, 0);
    if (addRef(self, L)) return 1;
    enqueue(self, L, front);
    return 1;
}
VERIFY(0x0260CBB4, requestArchive);

/* 0260D618: requests a resource <arc>/<name> */
u32 requestResource(u32 self, u32 arc, u32 name, u32 front) {
    WWHD_FUNC(0x0260D618, u32, self, arc, name, front);
    if (s32(load<u32>(self + 0xD8)) >= s32(load<u32>(self + 0xD0))) return 0;
    Local<u8[0x90]> S;
    const u32 h = hashPair(S.a, 0x100E1F98, arc, name);
    Local<u8[0xA0]> LL;
    const u32 L = LL.a;
    entryFirst(L, arc);
    entrySecond(L, name);
    entryTail(L, h, 0, 0);
    if (addRef(self, L)) return 1;
    enqueue(self, L, front);
    return 1;
}
VERIFY(0x0260D618, requestResource);

/* 0260E0DC: requests a stage resource (no reference counting) */
u32 requestStage(u32 self, u32 arc, u32 name) {
    WWHD_FUNC(0x0260E0DC, u32, self, arc, name);
    if (s32(load<u32>(self + 0xD8)) >= s32(load<u32>(self + 0xD0))) return 0;
    Local<u8[0x90]> S;
    const u32 h = hashPair(S.a, 0x100E1FA0, arc, name);
    Local<u8[0xA0]> LL;
    const u32 L = LL.a;
    entryFirst(L, arc);
    entrySecond(L, name);
    entryTail(L, h, 0, 1);
    pushBack(self, 0xCC, L);
    return 1;
}
VERIFY(0x0260E0DC, requestStage);

/* 0260E500: releases an archive; queues the unload when no reference is left to release */
void releaseArchive(u32 self, u32 name) {
    WWHD_FUNC(0x0260E500, void, self, name);
    const u32 h = hashString(name);
    Local<u8[0xA0]> LL;
    const u32 L = LL.a;
    entryFirst(L, name);
    entrySecondGlobal(L);
    entryTail(L, h, 1, 0);
    if (release(self, L)) return;
    pushBack(self, 0x1104, L);
}
VERIFY(0x0260E500, releaseArchive);

/* 0260E8A8: releases a resource; "Stage" also flushes the unload ring */
void releaseResource(u32 self, u32 arc, u32 name) {
    WWHD_FUNC(0x0260E8A8, void, self, arc, name);
    Local<u8[0x90]> S;
    const u32 h = hashPair(S.a, 0x100E1FA8, arc, name);
    Local<u8[0xA8]> F;
    const u32 T = F.a, L = F.a + 8;
    entryFirst(L, arc);
    entrySecond(L, name);
    store<u32>(L + 0x50, kFixed64Vt);
    store<u32>(T, 0x100E1FB0);
    store<u32>(L + 0x98, h);
    store<u8>(L + 0x9C, 0);
    store<u8>(L + 0x9D, 0);
    store<u32>(T + 4, kSafeStringVt);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    const u32 fn = load<u32>(load<u32>(T + 4) + 0x14);
    const u32 top = load<u32>(name);
    call_ptr<void>(fn, T);
    bool stage = top == load<u32>(T);
    if (!stage) {
        u32 a = load<u32>(name), b = load<u32>(T);
        bool decided = false;
        for (u32 i = 0; i < 0x40001; i++) {
            const u32 x = load<u8>(a + i), y = load<u8>(b + i);
            if (x != y) { decided = true; break; }
            if (!x) { stage = true; decided = true; break; }
        }
        (void)decided;
    }
    if (stage) {
        const u32 cnt = load<u32>(self + 0x1110);
        store<u8>(self + 0x1118, 1);
        if (cnt) pushBack(self, 0x1104, L);
    }
    if (release(self, L)) return;
    pushBack(self, 0x1104, L);
}
VERIFY(0x0260E8A8, releaseResource);

/* 0260EEE8: releases a stage resource: unloads at once when idle, else queues the unload */
void releaseStage(u32 self, u32 arc, u32 name) {
    WWHD_FUNC(0x0260EEE8, void, self, arc, name);
    Local<u8[0x90]> S;
    const u32 h = hashPair(S.a, 0x100E1FB8, arc, name);
    Local<u8[0xA0]> LL;
    const u32 L = LL.a;
    entryFirst(L, arc);
    entrySecond(L, name);
    const u32 st = load<u32>(self + 0xDC);
    entryTail(L, h, 0, 1);
    if (!st) {
        unload(self, L);
        return;
    }
    pushBack(self, 0x1104, L);
}
VERIFY(0x0260EEE8, releaseStage);

/* 0260F30C: is the id in the loaded-id ring? */
u32 isLoadedId(u32 self, u32 id) {
    WWHD_FUNC(0x0260F30C, u32, self, id);
    const u32 cnt = load<u32>(self + 0x1100);
    for (u32 i = 0; i != cnt; i++) {
        const s32 h = s32(load<u32>(self + 0x10FC));
        const s32 cap = s32(load<u32>(self + 0x10F8));
        s32 j = h + s32(i);
        const u32 base = load<u32>(self + 0x10F4);
        if (j >= cap) j -= cap;
        if (load<u32>(base + 4 * u32(j)) == id) return 1;
    }
    return 0;
}
VERIFY(0x0260F30C, isLoadedId);

/* 0260F374: is the id still pending (requested and not loaded)? */
u32 isPendingId(u32 self, u32 id) {
    WWHD_FUNC(0x0260F374, u32, self, id);
    if (isLoadedId(self, id)) return 0;
    const u32 cnt = load<u32>(self + 0xD8);
    for (u32 i = 0; i != cnt; i++) {
        s32 j = s32(load<u32>(self + 0xD4)) + s32(i);
        const s32 cap = s32(load<u32>(self + 0xD0));
        if (j >= cap) j -= cap;
        if (load<u32>(load<u32>(self + 0xCC) + u32(j) * 0xA0 + 0x98) == id) return 1;
    }
    return 0;
}
VERIFY(0x0260F374, isPendingId);

/* 0260F414: is the archive pending? */
u32 isPendingArchive(u32 self, u32 name) {
    WWHD_FUNC(0x0260F414, u32, self, name);
    return isPendingId(self, hashString(name));
}
VERIFY(0x0260F414, isPendingArchive);

/* 0260F4CC: is the resource <arc>_<name> pending? */
u32 isPendingResource(u32 self, u32 arc, u32 name) {
    WWHD_FUNC(0x0260F4CC, u32, self, arc, name);
    Local<u8[0x90]> S;
    const u32 s = S.a;
    store<u32>(s, s + 0xC);
    store<u32>(s + 8, 0x80);
    store<u32>(s + 4, 0x100E1E6C);
    store<u8>(s + 0x8B, 0);
    store<u8>(s + 0xC, 0);
    call_ptr<void>(load<u32>(load<u32>(arc + 4) + 0x14), arc);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 arcTop = load<u32>(arc);
    call_ptr<void>(fn, name);
    call<void>(0x02759C28, s, 0x100E1FC0u, arcTop, load<u32>(name));
    return isPendingId(self, hashString(s));
}
VERIFY(0x0260F4CC, isPendingResource);

/* 0260F61C: forgets a loaded id and drops its reference entry */
void forgetId(u32 self, u32 id) {
    WWHD_FUNC(0x0260F61C, void, self, id);
    const u32 ring = self + 0x10F4;
    const s32 cnt = s32(load<u32>(self + 0x1100));
    for (u32 i = 0; i != u32(cnt); i++) {
        const s32 h = s32(load<u32>(ring + 8));
        const s32 cap = s32(load<u32>(ring + 4));
        s32 j = h + s32(i);
        const u32 base = load<u32>(ring);
        if (j >= cap) j -= cap;
        if (load<u32>(base + 4 * u32(j)) != id) continue;
        if (cnt <= 0 || s32(i) < 0 || s32(i) >= cnt) break;
        s32 last = cnt - 1;
        if (s32(i) < last) {
            s32 k = s32(i), m = s32(i) + 1;
            do {
                const s32 hh = s32(load<u32>(ring + 8));
                const s32 cc = s32(load<u32>(ring + 4));
                s32 a = hh + k;
                if (a >= cc) a -= cc;
                s32 b = hh + m;
                const u32 bb = load<u32>(ring);
                if (b >= cc) b -= cc;
                store<u32>(bb + 4 * u32(a), load<u32>(bb + 4 * u32(b)));
                const s32 c = s32(load<u32>(ring + 0xC));
                k++;
                last = c - 1;
                m++;
            } while (k < last);
        }
        store<u32>(ring + 0xC, u32(last));
        break;
    }
    const u32 root = load<u32>(self + 0xE0);
    Local<u32[2]> keys;
    store<u32>(keys.a, id);
    store<u32>(keys.a + 4, id);
    const u32 n = call<u32>(0x0260ACC8, self + 0xE0, root, keys.a + 4);
    if (!n || !(n + 0x18)) return;
    const u32 r = call<u32>(0x0260AEFC, self + 0xE0, root, keys.a);
    store<u32>(self + 0xE0, r);
    if (r) store<u8>(r + 8, 0);
}
VERIFY(0x0260F61C, forgetId);

/* 0260F7A8: static initialiser */
void staticInit() {
    WWHD_FUNC(0x0260F7A8, void);
    const u32 b = 0x1048DD3C;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4F30u);
    const f32 lo = load<f32>(0x100E1FC8), hi = load<f32>(0x100E1FCC);
    store<f32>(0x1048DD30, lo);
    store<f32>(0x1048DD34, hi);
    call<void>(0x028ED6F8, 0x1048DD38u);
    call<void>(0x028F026C, 0x101F4F3Cu);
    call<void>(0x028EAB2C, 0x1048DD39u);
    call<void>(0x028F026C, 0x101F4F48u);
}
VERIFY(0x0260F7A8, staticInit);

/* 0260F850: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0260F850, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x20, 0x100E2090);
    call<void>(0x0260B770, p + 0xC8, 2u);
    call<void>(0x027452C8, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0260F850, dtor);

/* 0260F8EC: sead RTTI checkDerivedRuntimeTypeInfo (own and base type info, initialised on first use) */
u32 isDerived(u32 self, u32 info) {
    WWHD_FUNC(0x0260F8EC, u32, self, info);
    if (!load<u32>(0x101FD598)) {
        store<u32>(0x101FD598, 1);
        store<u32>(0x101FD97C, 0x100E1E14);
    }
    if (info == 0x101FD97C) return 1;
    if (!load<u32>(0x101FD594)) {
        store<u32>(0x101FD594, 1);
        store<u32>(0x101FD980, 0x100E1E04);
    }
    return info == 0x101FD980;
}
VERIFY(0x0260F8EC, isDerived);

/* 0260F9D4 / 0260F9DC: destructor thunks of the second base (this - 0x10) */
void dtorThunk2(u32 p, u32 flags) {
    WWHD_FUNC(0x0260F9D4, void, p, flags);
    call<void>(0x026138AC, p - 0x10, flags);
}
VERIFY(0x0260F9D4, dtorThunk2);
void dtorThunk3(u32 p, u32 flags) {
    WWHD_FUNC(0x0260F9DC, void, p, flags);
    call<void>(0x0261390C, p - 0x10, flags);
}
VERIFY(0x0260F9DC, dtorThunk3);

} // namespace hd_res_load_task
