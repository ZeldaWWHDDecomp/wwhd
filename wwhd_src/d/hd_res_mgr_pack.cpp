/* hd_res_mgr_pack: HD ResMgr loader threads, file loading and pack scanning (unit hd_res_mgr), WWHD.
 * HD-only code written from the WWHD code; see
 * hd_res_mgr.cpp for the ResMgr layout. sead objects built on the stack here (layouts from the
 * code): FileDevice path buffer FixedSafeString<0x104>, ResourceMgr::LoadArg, a temporary
 * resource list object (destroyed by 0275CFD0), ArchiveFileDevice, DirectoryHandle, DirectoryEntry.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_res_mgr {

static const u32 kSafeStringVt = 0x100E20A4;

static s32 packStrlen(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}

/* the RTTI type info of SharcArchiveRes-like resources, initialised on first use */
static u32 typeInfo(u32 guard, u32 info, u32 base) {
    if (!load<u32>(guard)) {
        store<u32>(guard, 1);
        store<u32>(info, base);
    }
    return info;
}

/* 0260FC98: creates the arena and the two loader threads */
void createThreads(u32 self, u32 heap) {
    WWHD_FUNC(0x0260FC98, void, self, heap);
    const u32 buf = call_ptr<u32>(load<u32>(load<u32>(heap + 0xC) + 0x34), heap, 0xA00000u, 0x40u);
    const u32 arena = call<u32>(0x0275E9D0, 0u, 0xA00000u, buf);
    const u32 r = self + 0x1123C;
    store<u32>(r, arena);
    Local<u32[4]> F;
    const u32 f = F.a; /* +0 core mask {4, 2}, +8 name SafeString */
    const u32 names[2] = {0x100E2354, 0x100E2364};
    const u32 fns[2] = {0x02613394, 0x026134B4};
    for (u32 i = 0; i < 2; i++) {
        u32 t = call<u32>(0x0273B050, 0x98u, heap, 4u);
        if (t) {
            store<u32>(f + 0xC, kSafeStringVt);
            store<u32>(f + 8, names[i]);
            const u32 d = call<u32>(0x0273B050, 0x10u, heap, 4u);
            if (d) {
                store<u32>(d + 4, self);
                store<u32>(d, 0x100E22BC);
                store<u16>(d + 8, 0);
                store<u16>(d + 0xA, 0xFFFF);
                store<u32>(d + 0xC, fns[i]);
            }
            const u32 prio = load<u32>(0x101F8B9C) + (i ? 3u : u32(-2));
            t = call<u32>(0x0275FBD8, t, f + 8, d, heap, prio, 0u, 0x7FFFFFFFu, 0x1800u, 0x20u);
        }
        store<u32>(r + 4 + 4 * i, t);
        store<u32>(f + 4, 2);
        store<u32>(f, 4);
        call<void>(0x02760C4C, t, f);
        const u32 th = load<u32>(r + 4 + 4 * i);
        call_ptr<void>(load<u32>(load<u32>(th + 0xC) + 0x2C), th);
    }
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
}
VERIFY(0x0260FC98, createThreads);

/* the temporary resource list object (o) and the FileDevice path buffer (pb) shared by the loaders */
static void tempListCtor(u32 o) {
    store<u32>(o + 0xC, 0);
    store<u32>(o + 8, o);
    store<u32>(o, 0);
    store<u32>(o + 4, 0);
    call<void>(0x02752A84, o + 0x10);
}
static void pathBufCtor(u32 pb) {
    store<u32>(pb, pb + 0xC);
    store<u8>(pb + 0x10F, 0);
    store<u8>(pb + 0xC, 0);
    store<u32>(pb + 8, 0x104);
    store<u32>(pb + 4, 0x100E2214);
}

/* 02610B40: loads a file as an archive resource (no decompression) */
u32 loadFile(u32 path, u32 heap, u32 align, u32 bufAlign) {
    WWHD_FUNC(0x02610B40, u32, path, heap, align, bufAlign);
    Local<u8[0x194]> F; /* game test 2026-10-05: pathBufCtor(f+0x84) writes f+0x193; the original frame keeps sp+0..0x194 below its stmw */
    const u32 f = F.a; /* frame offsets as the original's sp offsets */
    const u32 o = f + 0x34, pb = f + 0x84, a = f + 8;
    tempListCtor(o);
    store<u8>(f + 0x60, 0);
    store<u32>(f + 0x54, f + 0x60);
    store<u32>(f + 0x5C, 0x20);
    store<u8>(f + 0x7F, 0);
    store<u32>(f + 0x58, 0x100E217C);
    store<u32>(f + 0x80, 0x100E226C);
    store<u32>(f + 0x50, 0x100E22AC);
    pathBufCtor(pb);
    const u32 dev = call<u32>(0x02741C24, load<u32>(0x101F8B08), path, pb);
    u32 res = 0;
    if (dev) {
        store<u32>(a + 0x1C, 0);
        store<u32>(a, load<u32>(pb));
        store<u32>(a + 0x10, bufAlign);
        store<u32>(a + 0x14, bufAlign);
        store<u32>(a + 4, kSafeStringVt);
        store<u32>(a + 0xC, heap);
        store<u32>(a + 0x24, dev);
        store<u32>(a + 0x20, o);
        store<u32>(a + 8, heap);
        store<u32>(a + 0x28, align);
        store<u32>(a + 0x18, 0);
        res = call<u32>(0x0275DB94, load<u32>(0x101F8B68), a);
        if (res) {
            const u32 info = typeInfo(0x101FD6F4, 0x101FD9D0, 0x100E21B4);
            if (!call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0xC), res, info)) res = 0;
            store<u32>(f + 0x50, 0x1015E028);
            call<void>(0x0275CFD0, o, 0u);
            return res;
        }
    }
    store<u32>(f + 0x50, 0x1015E028);
    call<void>(0x0275CFD0, o, 0u);
    return 0;
}
VERIFY(0x02610B40, loadFile);

/* 02612C88: loads a compressed file (factory "sarc", decompressor) as an archive resource */
u32 loadCompressed(u32 path, u32 heap, u32 decomp, u32 align, u32 bufAlign) {
    WWHD_FUNC(0x02612C88, u32, path, heap, decomp, align, bufAlign);
    Local<u8[0x1A0]> F;
    const u32 f = F.a;
    const u32 o = f + 0x3C, pb = f + 0x8C, a = f + 0x10, fac = f + 8;
    tempListCtor(o);
    store<u32>(f + 0x5C, f + 0x68);
    store<u8>(f + 0x68, 0);
    store<u32>(f + 0x64, 0x20);
    store<u32>(f + 0x60, 0x100E217C);
    store<u8>(f + 0x87, 0);
    store<u32>(f + 0x88, 0x100E226C);
    store<u32>(f + 0x58, 0x100E22AC);
    pathBufCtor(pb);
    const u32 dev = call<u32>(0x02741C24, load<u32>(0x101F8B08), path, pb);
    if (dev) {
        store<u32>(a + 0x24, dev);
        store<u32>(a, load<u32>(pb));
        store<u32>(a + 0x1C, 0);
        store<u32>(a + 0x14, bufAlign);
        store<u32>(fac, 0x100E2420);
        store<u32>(fac + 4, kSafeStringVt);
        store<u32>(a + 0x28, align);
        store<u32>(a + 0xC, heap);
        store<u32>(a + 0x20, o);
        store<u32>(a + 0x18, 0);
        store<u32>(a + 4, kSafeStringVt);
        store<u32>(a + 0x10, bufAlign);
        store<u32>(a + 8, heap);
        u32 res = call<u32>(0x0275DD38, load<u32>(0x101F8B68), a, fac, decomp);
        if (res) {
            const u32 info = typeInfo(0x101FD6F4, 0x101FD9D0, 0x100E21B4);
            if (!call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0xC), res, info)) res = 0;
            call<void>(0xC00088A0, load<u32>(res + 0x14), load<u32>(res + 0x18));
            store<u32>(f + 0x58, 0x1015E028);
            call<void>(0x0275CFD0, o, 0u);
            return res;
        }
    }
    store<u32>(f + 0x58, 0x1015E028);
    call<void>(0x0275CFD0, o, 0u);
    return 0;
}
VERIFY(0x02612C88, loadCompressed);

/* 02610820: loads a stage pack file and registers it in the pack entry map */
u32 loadStagePack(u32 self, u32 name, u32 path, u32 heap, u32 align) {
    WWHD_FUNC(0x02610820, u32, self, name, path, heap, align);
    Local<u8[0x1A0]> F;
    const u32 f = F.a;
    const u32 o = f + 0x38, pb = f + 0x88, a = f + 0xC;
    pathBufCtor(pb);
    const u32 dev = call<u32>(0x02741C24, load<u32>(0x101F8B08), path, pb);
    if (!dev) return 0;
    store<u32>(o, 0);
    store<u32>(o + 4, 0);
    store<u32>(o + 8, o);
    store<u32>(o + 0xC, 0);
    call<void>(0x02752A84, o + 0x10);
    store<u8>(f + 0x64, 0);
    store<u32>(a + 0x18, 0);
    store<u32>(a + 0x1C, 0);
    store<u32>(f + 0x84, 0x100E22CC);
    store<u32>(a, load<u32>(pb));
    store<u32>(a + 8, heap);
    store<u32>(a + 0x28, align);
    store<u32>(a + 4, kSafeStringVt);
    store<u32>(a + 0x24, dev);
    store<u32>(f + 0x5C, 0x100E217C);
    store<u32>(f + 0x58, f + 0x64);
    store<u32>(a + 0x10, 0x2000);
    store<u32>(a + 0xC, heap);
    store<u32>(f + 0x54, 0x100E230C);
    store<u8>(f + 0x83, 0);
    store<u32>(a + 0x14, 0x2000);
    store<u32>(a + 0x20, o);
    store<u32>(f + 0x60, 0x20);
    u32 res = call<u32>(0x0275DB94, load<u32>(0x101F8B68), a);
    const u32 info = typeInfo(0x101FD778, 0x101FDD64, 0x100E21B4);
    if (!res || !call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0xC), res, info) || !res) {
        store<u32>(f + 0x54, 0x1015E028);
        call<void>(0x0275CFD0, o, 0u);
        return 0;
    }
    u32 holder = call<u32>(0x0273B050, 0x54u, heap, 4u);
    if (holder) holder = call<u32>(0x026106B4, holder, name, res, heap);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 top = load<u32>(name);
    call_ptr<void>(fn, name);
    const s32 len = packStrlen(load<u32>(name));
    const u32 h = call<u32>(0x0273B264, top, len);
    const u32 tree = self + 0xD228;
    if (s32(load<u32>(tree + 0xC)) < s32(load<u32>(tree + 0x10))) {
        const u32 node = load<u32>(tree + 4);
        if (node) store<u32>(tree + 4, load<u32>(node));
        if (node) {
            store<u32>(node + 0xC, h);
            store<u32>(node + 0x1C, tree);
            store<u32>(node, 0);
            store<u8>(node + 8, 1);
            store<u32>(node + 0x14, h);
            store<u32>(node + 0x18, holder);
            store<u32>(node + 0x10, 0x100E2334);
            store<u32>(node + 4, 0);
        }
        const u32 cnt = load<u32>(tree + 0xC) + 1;
        const u32 root = load<u32>(tree);
        store<u32>(tree + 0xC, cnt);
        const u32 r = call<u32>(0x0260AD08, tree, root, node);
        store<u32>(tree, r);
        store<u8>(r + 8, 0);
    } else {
        Local<u32> key;
        store<u32>(key.a, h);
        const u32 n = call<u32>(0x0260ACC8, tree, load<u32>(tree), key.a);
        if (n && n + 0x18) store<u32>(n + 0x18, holder);
    }
    store<u32>(f + 0x54, 0x1015E028);
    call<void>(0x0275CFD0, o, 0u);
    return 1;
}
VERIFY(0x02610820, loadStagePack);

/* ---- pack scanning (02610CF8 arc packs, 02611864 SZS packs) ---- */

/* copies the string object src into a buffer whose capacity is at capAddr (dst read before) */
static void packCopy(u32 dst, u32 capAddr, u32 src) {
    call_ptr<void>(load<u32>(load<u32>(src + 4) + 0x14), src);
    s32 n = packStrlen(load<u32>(src));
    const s32 size = s32(load<u32>(capAddr));
    const u32 fn = load<u32>(load<u32>(src + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, src);
    call<void>(0xC0009988, dst, load<u32>(src), u32(n), 0u);
    store<u8>(dst + n, 0);
}

/* sead::SafeString strlen with the overflow flag */
static s32 packStrlenOver(u32 p, bool& over) {
    over = false;
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) { over = true; return 0; }
        p++;
        if (!load<u8>(p)) return n;
    }
}

/* rfind of the "." string object d (at f+0x14) in the entry name (object at ent); T at f+0x1C */
static s32 findDot(u32 f, u32 ent, u32 dotStr) {
    const u32 d = f + 0x14, tt = f + 0x1C;
    store<u32>(d + 4, kSafeStringVt);
    store<u32>(d, dotStr);
    call_ptr<void>(load<u32>(load<u32>(ent + 4) + 0x14), ent);
    bool over;
    const s32 len = packStrlenOver(load<u32>(ent), over);
    call_ptr<void>(load<u32>(load<u32>(d + 4) + 0x14), d);
    bool dOver;
    s32 dlen = packStrlenOver(load<u32>(d), dOver);
    s32 pos;
    if (dOver) { dlen = 0; pos = len; }
    else pos = len - dlen;
    if (pos < 0) return -1;
    s32 count = pos + 1;
    for (;;) {
        store<u32>(tt + 4, kSafeStringVt);
        store<u32>(tt, load<u32>(ent) + u32(pos));
        call<void>(0x0261396C, tt);
        call_ptr<void>(load<u32>(load<u32>(tt + 4) + 0x14), tt);
        const u32 fn = load<u32>(load<u32>(d + 4) + 0x14);
        const u32 ttop = load<u32>(tt);
        call_ptr<void>(fn, d);
        if (ttop == load<u32>(d) || dlen <= 0) return pos;
        u32 a = load<u32>(d), b = load<u32>(tt);
        bool match = false;
        for (s32 k = dlen;;) {
            const u32 tb = load<u8>(b);
            if (!tb) { match = !load<u8>(a); break; }
            const u32 ta = load<u8>(a);
            if (!ta || ta != tb) break;
            a++;
            b++;
            if (--k == 0) { match = true; break; }
        }
        if (match) return pos;
        count--;
        pos--;
        if (!count) return pos;
    }
}

/* is the extension after pos the string extStr? (E at f+0x14, the extension string at f+0x24) */
static bool isExtension(u32 f, u32 ent, s32 pos, u32 extStr) {
    const u32 e = f + 0x14, x = f + 0x24;
    call_ptr<void>(load<u32>(load<u32>(ent + 4) + 0x14), ent);
    const u32 top = load<u32>(ent);
    const s32 nlen = packStrlen(top);
    const s32 p1 = pos + 1;
    const u32 et = (p1 < 0 || p1 > nlen) ? load<u32>(0x104A0CD8) : top + u32(p1);
    store<u32>(e + 4, kSafeStringVt);
    store<u32>(x + 4, kSafeStringVt);
    store<u32>(e, et);
    store<u32>(x, extStr);
    call<void>(0x0261396C, e);
    call_ptr<void>(load<u32>(load<u32>(e + 4) + 0x14), e);
    const u32 fn = load<u32>(load<u32>(x + 4) + 0x14);
    const u32 etop = load<u32>(e);
    call_ptr<void>(fn, x);
    const u32 xs = load<u32>(x);
    if (etop == xs) return true;
    const u32 a = load<u32>(e);
    for (u32 i = 0; i < 0x40001; i++) {
        const u32 c = load<u8>(a + i), y = load<u8>(xs + i);
        if (c != y) return false;
        if (!c) return true;
    }
    return false;
}

/* cuts the entry name at pos (when inside its buffer) */
static void cutAt(u32 ent, s32 pos) {
    if (pos < s32(load<u32>(ent + 8))) {
        const u32 top = load<u32>(ent);
        store<u8>(top + u32(pos < 0 ? 0 : pos), 0);
    } else {
        call_ptr<void>(load<u32>(load<u32>(ent + 4) + 0x14), ent);
        packStrlen(load<u32>(ent));
    }
}

static void sleepTicks(u32 t, u32 ticks) {
    store<u32>(t + 8, 0);
    store<u32>(t + 0xC, ticks);
    store<u32>(t, 0);
    store<u32>(t + 4, ticks);
    call<void>(0x02760C3C, t);
}

/* the archive map insertion of hd_res_mgr.cpp (key: the string object name) */
static void packInsertNamed(u32 self, u32 name, u32 holder) {
    const u32 tree = self + 0x14;
    if (s32(load<u32>(tree + 0xC)) >= s32(load<u32>(tree + 0x10))) {
        const u32 n = call<u32>(0x02613F20, tree, load<u32>(tree), name);
        if (n && n + 0x18) store<u32>(n + 0x18, holder);
        return;
    }
    const u32 node = load<u32>(tree + 4);
    if (node) store<u32>(tree + 4, load<u32>(node));
    if (node) {
        store<u32>(node + 0x10, kSafeStringVt);
        store<u8>(node + 8, 1);
        store<u32>(node + 0x14, 0x100E231C);
        store<u32>(node + 0xC, 0x10000160);
        store<u32>(node + 0x1C, tree);
        store<u32>(node + 0x18, holder);
        store<u8>(node + 0x50, 0);
        store<u32>(node, 0);
        store<u32>(node + 4, 0);
        const u32 buf = node + 0x20;
        call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
        bool over;
        s32 n = packStrlenOver(load<u32>(name), over);
        if (over) {
            call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
            call<void>(0xC0009988, buf, load<u32>(name), 0u, 0u);
            store<u8>(buf, 0);
        } else {
            if (n >= 0x31) n = 0x30;
            call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
            call<void>(0xC0009988, buf, load<u32>(name), u32(n), 0u);
            store<u8>(buf + n, 0);
        }
        store<u32>(node + 0xC, buf);
    }
    const u32 cnt = load<u32>(tree + 0xC) + 1;
    const u32 root = load<u32>(tree);
    store<u32>(tree + 0xC, cnt);
    const u32 r = call<u32>(0x0261401C, tree, root, node);
    store<u32>(tree, r);
    store<u8>(r + 8, 0);
}

/* the stack objects of both scanners: ArchiveFileDevice dev over arc (device name from devName),
 * DirectoryHandle h opened at the root path */
static void deviceCtor(u32 f, u32 dev, u32 nameObj, u32 devName, u32 arc, u32 saveVt) {
    store<u32>(nameObj + 4, kSafeStringVt);
    store<u32>(nameObj, devName);
    store<u32>(dev + 4, 0);
    store<u32>(dev, 0);
    store<u32>(dev + 0xC, 0);
    store<u32>(dev + 8, dev);
    call<void>(0x02752A84, dev + 0x10);
    store<u8>(dev + 0x4B, 0);
    store<u32>(dev + 0x50, 0x10143B78);
    store<u32>(dev + 0x20, dev + 0x2C);
    store<u32>(dev + 0x1C, 0x10143C58);
    store<u32>(dev + 0x28, 0x20);
    store<u8>(dev + 0x4C, 1);
    store<u8>(dev + 0x2C, 0);
    store<u32>(dev + 0x24, 0x100E217C);
    packCopy(dev + 0x2C, dev + 0x28, nameObj);
    store<u32>(dev + 0x54, arc);
    store<u32>(dev + 0x50, 0x101439A4);
    store<u32>(dev + 0x1C, 0x10143AA4);
    store<u32>(saveVt, 0x10143AA4);
}

static void closeHandle(u32 f, u32 h, u32 dev, u32 saveHVt, u32 saveDVt) {
    const u32 inner = load<u32>(h + 0x14);
    store<u32>(h + 0xC, load<u32>(saveHVt));
    if (inner) call<void>(0x02741088, inner, h);
    call<void>(0x02752BEC, h, 0u);
    store<u32>(dev + 0x1C, load<u32>(saveDVt));
    call<void>(0x02740A38, dev, 0u);
}

/* 02610CF8: loads a pack archive and registers its ".szs" members as sarc archives by name */
u32 loadPack(u32 self, u32 name, u32 heap) {
    WWHD_FUNC(0x02610CF8, u32, self, name, heap);
    Local<u8[0x2C0]> F;
    const u32 f = F.a;
    const u32 s = f + 0x48, dev = f + 0x218, h = f + 0x1E0, ent = f + 0xD0, la = f + 0xA4, lst = f + 0x270;
    store<u32>(s, 0x100E23A0);
    store<u32>(s + 4, kSafeStringVt);
    const u32 tmpHeap = call<u32>(0x02754B38, 0u, s, load<u32>(load<u32>(0x101F4F28) + 0x2040), 0xFFFFFFFFu, 0u);
    const u32 arc = call<u32>(0x02610B40, name, tmpHeap, load<u32>(self + 0x11318), 0x2000u);
    call_ptr<void>(load<u32>(load<u32>(tmpHeap + 0xC) + 0x2C), tmpHeap);
    store<u32>(s + 4, kSafeStringVt);
    store<u32>(s, 0x100E23B0);
    const u32 resHeap = call<u32>(0x02754B38, 0u, s, heap, 1u, 0u);
    deviceCtor(f, dev, f + 0x9C, 0x100E238C, arc, f + 0x10);
    call<void>(0x02752A84, h);
    store<u32>(h + 0x14, 0);
    store<u32>(s + 4, kSafeStringVt);
    store<u32>(f + 8, 0x100E2194);
    store<u32>(h + 0x10, 0);
    store<u32>(h + 0xC, 0x100E2194);
    store<u32>(s, 0x100E2396);
    call<void>(0x02740FDC, dev, h, s);
    if (!load<u32>(h + 0x10)) {
        call_ptr<void>(load<u32>(load<u32>(tmpHeap + 0xC) + 0x24), tmpHeap);
        closeHandle(f, h, dev, f + 8, f + 0x10);
        return 0;
    }
    store<u32>(lst, 0);
    store<u32>(lst + 4, 0);
    store<u8>(ent + 0x10B, 0);
    store<u8>(ent + 0xC, 0);
    store<u32>(ent + 4, 0x100E214C);
    store<u8>(ent + 0x10C, 0);
    store<u32>(ent, ent + 0xC);
    store<u32>(lst + 8, lst);
    store<u32>(lst + 0xC, 0);
    store<u32>(ent + 8, 0x100);
    call<void>(0x02752A84, lst + 0x10);
    store<u8>(lst + 0x4B, 0);
    store<u32>(lst + 0x20, lst + 0x2C);
    store<u32>(la + 4, kSafeStringVt);
    store<u32>(la + 0x1C, 0);
    store<u32>(la + 0xC, resHeap);
    store<u8>(lst + 0x2C, 0);
    store<u32>(f + 0xC, 0x1015E028);
    store<u32>(la + 0x20, lst);
    store<u32>(la + 0x14, 0x2000);
    store<u32>(lst + 0x1C, 0x100E22AC);
    store<u32>(lst + 0x24, 0x100E217C);
    store<u32>(la + 0x18, 0);
    store<u32>(lst + 0x4C, 0x100E226C);
    store<u32>(la + 8, resHeap);
    store<u32>(la + 0x10, 0x2000);
    store<u32>(la + 0x24, dev);
    store<u32>(la, 0x10000160);
    store<u32>(la + 0x28, 0x40000);
    store<u32>(lst + 0x28, 0x20);
    while (call<u32>(0x02741604, h, ent, 1u)) {
        if (load<u8>(ent + 0x10C)) continue;
        const s32 pos = findDot(f, ent, 0x100E2394);
        const u32 k = f + 0x50;
        store<u8>(k + 0x4B, 0);
        store<u32>(k, k + 0xC);
        store<u32>(k + 8, 0x40);
        store<u8>(k + 0xC, 0);
        store<u32>(k + 4, 0x100E211C);
        bool szs = false;
        if (pos >= 0) szs = isExtension(f, ent, pos, 0x100E2390);
        if (szs) {
            const u32 arena = load<u32>(self + 0x1123C);
            store<u32>(la, load<u32>(ent));
            store<u32>(f + 0x2C, 0x100E2398);
            store<u32>(f + 0x30, kSafeStringVt);
            u32 res = call<u32>(0x0275DD38, load<u32>(0x101F8B68), la, f + 0x2C, arena);
            const u32 info = typeInfo(0x101FD6F4, 0x101FD9D0, 0x100E21B4);
            if (!res || !call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0xC), res, info) || !res) continue;
            cutAt(ent, pos);
            const u32 dst = load<u32>(k);
            packCopy(dst, k + 8, ent);
            call<void>(0xC00088A0, load<u32>(res + 0x14), load<u32>(res + 0x18));
            u32 holder = call<u32>(0x0273B050, 0x54u, resHeap, 4u);
            if (holder) holder = call<u32>(0x026100D0, holder, k, res, tmpHeap);
            packInsertNamed(self, k, holder);
        }
        sleepTicks(f + 0x38, 0x1E);
    }
    call<void>(0x02741088, dev, h);
    call_ptr<void>(load<u32>(load<u32>(resHeap + 0xC) + 0x2C), resHeap);
    call_ptr<void>(load<u32>(load<u32>(tmpHeap + 0xC) + 0x24), tmpHeap);
    store<u32>(lst + 0x1C, load<u32>(f + 0xC));
    call<void>(0x0275CFD0, lst, 0u);
    closeHandle(f, h, dev, f + 8, f + 0x10);
    return 1;
}
VERIFY(0x02610CF8, loadPack);

/* 02611864: loads an SZS permanent pack and registers its ".szs" members (still compressed) in the
 * pack entry map by name hash; the pack heap creation is retried every 30000000 ticks */
u32 loadSzsPack(u32 self, u32 name, u32 heap) {
    WWHD_FUNC(0x02611864, u32, self, name, heap);
    Local<u8[0x2D0]> F;
    const u32 f = F.a;
    const u32 dev = f + 0x228, h = f + 0x1F0, ent = f + 0xE0, la = f + 0xB4, lst = f + 0x280, k = f + 0x60;
    const u32 parent = load<u32>(load<u32>(0x101F4F28) + 0x2040);
    u32 packHeap;
    for (;;) {
        store<u32>(f + 0x30, kSafeStringVt);
        store<u32>(f + 0x2C, 0x100E23C8);
        packHeap = call<u32>(0x02754B38, 0x3200000u, f + 0x2C, parent, 0xFFFFFFFFu, 0u);
        if (packHeap) break;
        store<u32>(f + 0x3C, 0x1C9C380);
        store<u32>(f + 0x4C, 0x1C9C380);
        store<u32>(f + 0x38, 0);
        store<u32>(f + 0x48, 0);
        call<void>(0x02760C3C, f + 0x48);
    }
    const u32 arc = call<u32>(0x02610B40, name, packHeap, load<u32>(self + 0x11318), 0x2000u);
    call_ptr<void>(load<u32>(load<u32>(packHeap + 0xC) + 0x2C), packHeap);
    store<u32>(f + 0xB0, kSafeStringVt);
    store<u32>(f + 0xAC, 0x100E23DC);
    const u32 resHeap = call<u32>(0x02754B38, 0u, f + 0xAC, heap, 1u, 0u);
    deviceCtor(f, dev, f + 0x58, 0x100E23BC, arc, f + 0xC);
    call<void>(0x02752A84, h);
    store<u32>(h + 0x14, 0);
    store<u32>(f + 0xB0, kSafeStringVt);
    store<u32>(f + 0x10, 0x100E2194);
    store<u32>(h + 0x10, 0);
    store<u32>(h + 0xC, 0x100E2194);
    store<u32>(f + 0xAC, 0x100E23C6);
    call<void>(0x02740FDC, dev, h, f + 0xAC);
    if (!load<u32>(h + 0x10)) {
        call_ptr<void>(load<u32>(load<u32>(packHeap + 0xC) + 0x24), packHeap);
        closeHandle(f, h, dev, f + 0x10, f + 0xC);
        return 0;
    }
    store<u32>(lst, 0);
    store<u8>(ent + 0x10B, 0);
    store<u32>(lst + 4, 0);
    store<u8>(ent + 0xC, 0);
    store<u32>(ent + 4, 0x100E214C);
    store<u8>(ent + 0x10C, 0);
    store<u32>(ent, ent + 0xC);
    store<u32>(lst + 8, lst);
    store<u32>(lst + 0xC, 0);
    store<u32>(ent + 8, 0x100);
    call<void>(0x02752A84, lst + 0x10);
    store<u8>(lst + 0x4B, 0);
    store<u32>(lst + 0x20, lst + 0x2C);
    store<u32>(la + 0xC, resHeap);
    store<u32>(la + 0x18, 0);
    store<u32>(la + 0x28, 0);
    store<u8>(lst + 0x2C, 0);
    store<u32>(f + 8, 0x1015E028);
    store<u32>(lst + 0x28, 0x20);
    store<u32>(la + 8, resHeap);
    store<u32>(la, 0x10000160);
    store<u32>(la + 4, kSafeStringVt);
    store<u32>(la + 0x20, lst);
    store<u32>(la + 0x10, 0x2000);
    store<u32>(lst + 0x24, 0x100E217C);
    store<u32>(la + 0x24, dev);
    store<u32>(la + 0x1C, 0);
    store<u32>(lst + 0x4C, 0x100E22CC);
    store<u32>(la + 0x14, 0x2000);
    store<u32>(lst + 0x1C, 0x100E230C);
    while (call<u32>(0x02741604, h, ent, 1u)) {
        if (load<u8>(ent + 0x10C)) continue;
        const s32 pos = findDot(f, ent, 0x100E23C4);
        store<u8>(k + 0x4B, 0);
        store<u8>(k + 0xC, 0);
        store<u32>(k + 8, 0x40);
        store<u32>(k, k + 0xC);
        store<u32>(k + 4, 0x100E211C);
        if (pos < 0 || !isExtension(f, ent, pos, 0x100E23C0)) continue;
        store<u32>(la, load<u32>(ent));
        const u32 res = call<u32>(0x0275DB94, load<u32>(0x101F8B68), la);
        const u32 info = typeInfo(0x101FD778, 0x101FDD64, 0x100E21B4);
        if (!res || !call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0xC), res, info) || !res) continue;
        cutAt(ent, pos);
        const u32 dst = load<u32>(k);
        packCopy(dst, k + 8, ent);
        u32 holder = call<u32>(0x0273B050, 0x54u, resHeap, 4u);
        if (holder) holder = call<u32>(0x026106B4, holder, k, res, 0u);
        call_ptr<void>(load<u32>(load<u32>(k + 4) + 0x14), k);
        const u32 fn = load<u32>(load<u32>(k + 4) + 0x14);
        const u32 top = load<u32>(k);
        call_ptr<void>(fn, k);
        const s32 klen = packStrlen(load<u32>(k));
        const u32 hh = call<u32>(0x0273B264, top, klen);
        const u32 tree = self + 0xD228;
        if (s32(load<u32>(tree + 0xC)) < s32(load<u32>(tree + 0x10))) {
            const u32 node = load<u32>(tree + 4);
            if (node) store<u32>(tree + 4, load<u32>(node));
            if (node) {
                store<u32>(node, 0);
                store<u32>(node + 0xC, hh);
                store<u32>(node + 0x10, 0x100E2334);
                store<u32>(node + 0x18, holder);
                store<u32>(node + 0x1C, tree);
                store<u32>(node + 4, 0);
                store<u8>(node + 8, 1);
                store<u32>(node + 0x14, hh);
            }
            const u32 cnt = load<u32>(tree + 0xC) + 1;
            const u32 root = load<u32>(tree);
            store<u32>(tree + 0xC, cnt);
            const u32 r = call<u32>(0x0260AD08, tree, root, node);
            store<u32>(tree, r);
            store<u8>(r + 8, 0);
        } else {
            store<u32>(f + 0x34, hh);
            const u32 n = call<u32>(0x0260ACC8, tree, load<u32>(tree), f + 0x34);
            if (n && n + 0x18) store<u32>(n + 0x18, holder);
        }
        sleepTicks(f + 0x50, 1);
    }
    call<void>(0x02741088, dev, h);
    call_ptr<void>(load<u32>(load<u32>(resHeap + 0xC) + 0x2C), resHeap);
    call_ptr<void>(load<u32>(load<u32>(packHeap + 0xC) + 0x24), packHeap);
    store<u32>(lst + 0x1C, load<u32>(f + 8));
    call<void>(0x0275CFD0, lst, 0u);
    closeHandle(f, h, dev, f + 0x10, f + 0xC);
    return 1;
}
VERIFY(0x02611864, loadSzsPack);

} // namespace hd_res_mgr
