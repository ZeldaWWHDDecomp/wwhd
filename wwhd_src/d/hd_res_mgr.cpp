/* hd_res_mgr: HD resource manager (ResMgr singleton 101F4F7C: archives by name, pack loading,
 * loader threads), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 0260F9E4..02614BDF (sinit 02613710; two initialiser-only TUs 02614AB8 and
 * 02614B4C). The pack scanners are in hd_res_mgr_pack.cpp. Not verified here (sead library
 * template instances emitted in this TU): 02613884 02613898 026138AC 0261390C 0261396C 02613970
 * 02613988 0261399C 02613BBC..02613C0C 02613C20 02613C94 02613C98 02613CA0 02613CA4 02613D60
 * 02613DB4 02613DE4 02613E38 02613E90..02613ECC 02613EE0 02613F00 02613F20 (TreeMap find)
 * 0261401C (TreeMap insert) 026142C4 (TreeMap erase).
 *
 * ResMgr (0x1131C): +0 vtable 100E222C, +0x14 archive map (sead TreeMap keyed by name, 0x280 nodes
 * of 0x54 at +0x28; node +0xC key {top, vtable} with a 0x30-char buffer at +0x20, +0x18 value),
 * +0xD228 pack entry map (hash keys, 0x200 nodes of 0x20; find 0260ACC8, insert 0260AD08,
 * erase 0260AEFC), +0x1123C arena, +0x11240/+0x11244 loader threads, +0x11248 request state,
 * +0x1124C request name (FixedSafeString<48>) and +0x11288 path (FixedSafeString<64>),
 * +0x112D4 heap, +0x112D8 u8 "from pack", +0x112DC critical section, +0x11318 load alignment.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_res_mgr {

static const u32 kSafeStringVt = 0x100E20A4;
static const u32 kResMgr = 0x101F4F7C;

/* sead strlen bound */
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

/* copies the string object src into the buffer dst (capacity read from cap) */
static void copyInto(u32 dst, u32 capAddr, u32 src) {
    call_ptr<void>(load<u32>(load<u32>(src + 4) + 0x14), src);
    s32 n = cstrLength(load<u32>(src));
    const s32 size = s32(load<u32>(capAddr));
    const u32 fn = load<u32>(load<u32>(src + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, src);
    call<void>(0xC0009988, dst, load<u32>(src), u32(n), 0u);
    store<u8>(dst + n, 0);
}

/* hash of a SafeString's text: sead::HashCRC32::calcHash (0273B264, r3/r4 only; matcher name
 * dRes_control_c::getResInfoLoaded corrected in matcher_errors.tsv) */
static u32 hashString(u32 s) {
    call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s);
    const u32 fn = load<u32>(load<u32>(s + 4) + 0x14);
    const u32 top = load<u32>(s);
    call_ptr<void>(fn, s);
    const s32 len = cstrLength(load<u32>(s));
    return call<u32>(0x0273B264, top, len);
}

/* FixedSafeString<N> constructor, inlined (placement-new null checks as GHS emits them) */
static void fixedCtor(u32 s, u32 n, u32 vt1, u32 vt2, u32 vt3, u32 allocSize) {
    u32 q = s;
    if (!q) q = call<u32>(0x0273AD10, allocSize);
    if (!q) return;
    u32 r = q;
    if (!r) r = call<u32>(0x0273AD10, 0xCu);
    if (r) {
        store<u32>(r, q + 0xC);
        store<u32>(r + 4, vt1);
        store<u32>(r + 8, n);
        store<u8>(q + 0xC + n - 1, 0);
    }
    const u32 top = load<u32>(q);
    store<u32>(q + 4, vt2);
    store<u8>(top, 0);
    store<u32>(q + 4, vt3);
}

/* 0260F9E4: ResMgr constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x0260F9E4, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x1131Cu);
        if (!p) return 0;
    }
    store<u32>(p, 0x100E222C);
    u32 t = p + 0x14;
    if (!t) t = call<u32>(0x0273AD10, 0xD214u);
    if (t) {
        const u32 nodes = t + 0x14;
        store<u32>(t, 0);
        store<u32>(t + 0xC, 0);
        store<u32>(t + 0x10, 0x280);
        store<u32>(t + 4, nodes);
        for (u32 i = 0; i < 0x27F; i++) store<u32>(nodes + 0x54 * i, nodes + 0x54 * (i + 1));
        store<u32>(nodes + 0xD1AC, 0);
        store<u32>(t + 8, nodes);
    }
    const u32 m = p + 0xD228;
    u32 q = m;
    if (!q) q = call<u32>(0x0273AD10, 0x4014u);
    if (q) {
        const u32 nodes = q + 0x14;
        store<u32>(q, 0);
        store<u32>(q + 0xC, 0);
        store<u32>(q + 4, nodes);
        store<u32>(q + 0x10, 0x200);
        for (u32 i = 0; i < 0x1FF; i++) store<u32>(nodes + 0x20 * i, nodes + 0x20 * (i + 1));
        store<u32>(nodes + 0x3FE0, 0);
        store<u32>(q + 8, nodes);
    }
    store<u32>(m + 0x4014, 0);
    store<u32>(m + 0x4020, 0);
    store<u32>(m + 0x401C, 0);
    store<u32>(m + 0x4018, 0);
    call<void>(0x028F521C, m + 0x4024, 0x90u);
    fixedCtor(m + 0x4024, 0x30, 0x100E20BC, 0x100E223C, 0x100E2254, 0x3C);
    fixedCtor(m + 0x4060, 0x40, 0x100E20BC, 0x100E2104, 0x100E211C, 0x4C);
    call<void>(0x02760084, m + 0x40B4);
    store<u32>(m + 0x40F0, 0x40000);
    return p;
}
VERIFY(0x0260F9E4, ctor);

/* 0260FBF4: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x0260FBF4, void, heap);
    if (load<u32>(kResMgr)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x1131Cu, heap, 4u);
    const u32 d = p + 4;
    if (d) {
        call<void>(0x02752B0C, d, heap, 3u);
        store<u32>(d + 0xC, 0x100E25C0);
    }
    store<u32>(0x101F4F80, d);
    store<u32>(kResMgr, p ? ctor(p) : 0u);
}
VERIFY(0x0260FBF4, createInstance);

/* 02610070: "Object/<name>.szs" */
void objectPath(u32 self, u32 out, u32 name) {
    WWHD_FUNC(0x02610070, void, self, out, name);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    call<void>(0x02759C28, out, 0x100E237Cu, load<u32>(name));
}
VERIFY(0x02610070, objectPath);

/* 026100D0: archive holder constructor (name, archive resource, heap; default heap: current) */
u32 holderCtor(u32 p, u32 name, u32 res, u32 heap) {
    WWHD_FUNC(0x026100D0, u32, p, name, res, heap);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x54u);
        if (!p) return 0;
    }
    call<void>(0x02752A84, p);
    store<u32>(p + 0xC, 0x100E25D0);
    fixedCtor(p + 0x10, 0x30, 0x100E20BC, 0x100E223C, 0x100E2254, 0x3C);
    store<u32>(p + 0x4C, res);
    const u32 dst = load<u32>(p + 0x10);
    copyInto(dst, p + 0x18, name);
    store<u32>(p + 0x50, heap ? heap : call<u32>(0x02756140, load<u32>(0x101F8B4C)));
    return p;
}
VERIFY(0x026100D0, holderCtor);

/* inserts holder under name into the archive map (map full: replaces an existing entry) */
static void insertNamed(u32 self, u32 name, u32 holder) {
    const u32 tree = self + 0x14;
    if (s32(load<u32>(tree + 0xC)) >= s32(load<u32>(tree + 0x10))) {
        const u32 n = call<u32>(0x02613F20, tree, load<u32>(tree), name);
        if (n && n + 0x18) store<u32>(n + 0x18, holder);
        return;
    }
    const u32 node = load<u32>(tree + 4);
    if (node) store<u32>(tree + 4, load<u32>(node));
    if (node) {
        store<u32>(node + 0x18, holder);
        store<u32>(node, 0);
        store<u32>(node + 0x1C, tree);
        store<u32>(node + 0x14, 0x100E231C);
        store<u8>(node + 0x50, 0);
        store<u32>(node + 0xC, 0x10000160);
        store<u8>(node + 8, 1);
        store<u32>(node + 0x10, kSafeStringVt);
        store<u32>(node + 4, 0);
        const u32 buf = node + 0x20;
        call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
        const u32 text = load<u32>(name);
        s32 n = 0;
        bool over = false;
        if (load<u8>(text)) {
            for (;;) {
                n++;
                if (n > 0x40000) { over = true; break; }
                if (!load<u8>(text + n)) break;
            }
        }
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

/* reads a pack entry into a new SharcArchiveRes */
static u32 readPackEntry(u32 entry, u32 heap) {
    const u32 size = call<u32>(0x0275F800, load<u32>(load<u32>(entry) + 0x4C));
    const u32 aligned = (size + 0x1F) & ~0x1Fu;
    const u32 buf = call<u32>(0x0273B0D4, aligned, heap, 0x2000u);
    call<void>(0x0275F808, load<u32>(load<u32>(entry) + 0x4C), buf, aligned);
    u32 arc = call<u32>(0x0273B050, 0x44u, heap, 0x2000u);
    if (arc) arc = call<u32>(0x0275E11C, arc);
    call<void>(0x0275D21C, arc, buf, size, aligned, 0u, heap);
    call<void>(0xC00088A0, load<u32>(arc + 0x14), load<u32>(arc + 0x18));
    return arc;
}

/* 02610258: makes an archive from the loaded packs available under its name */
u32 loadFromPack(u32 self, u32 name, u32 path, u32 heap) {
    WWHD_FUNC(0x02610258, u32, self, name, path, heap);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    if (n && n + 0x18) return 1;
    Local<u32> key;
    store<u32>(key.a, hashString(name));
    const u32 e = call<u32>(0x0260ACC8, self + 0xD228, load<u32>(self + 0xD228), key.a);
    if (!e || !(e + 0x18)) return 0;
    const u32 arc = readPackEntry(e + 0x18, heap);
    if (!arc) return 0;
    u32 holder = call<u32>(0x0273B050, 0x54u, heap, 4u);
    if (holder) holder = holderCtor(holder, name, arc, heap);
    insertNamed(self, name, holder);
    return 1;
}
VERIFY(0x02610258, loadFromPack);

/* 026105C4: request by name: stores the name and its "Object/<name>.szs" path, then loads it */
u32 requestObject(u32 self, u32 name, u32 heap) {
    WWHD_FUNC(0x026105C4, u32, self, name, heap);
    const u32 r = self + 0x1124C;
    const u32 dst = load<u32>(r);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    s32 n = cstrLength(load<u32>(name));
    const s32 size = s32(load<u32>(r + 8));
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, name);
    call<void>(0xC0009988, dst, load<u32>(name), u32(n), 0u);
    store<u8>(dst + n, 0);
    objectPath(self, r + 0x3C, name);
    store<u32>(r + 0x88, heap);
    return loadFromPack(self, r, r + 0x3C, heap);
}
VERIFY(0x026105C4, requestObject);

/* 026106B4: second holder type constructor (pack-scanned archives) */
u32 holder2Ctor(u32 p, u32 name, u32 res, u32 heap) {
    WWHD_FUNC(0x026106B4, u32, p, name, res, heap);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x54u);
        if (!p) return 0;
    }
    call<void>(0x02752A84, p);
    store<u32>(p + 0xC, 0x100E25E0);
    fixedCtor(p + 0x10, 0x30, 0x100E20BC, 0x100E223C, 0x100E2254, 0x3C);
    store<u32>(p + 0x50, heap);
    store<u32>(p + 0x4C, res);
    const u32 dst = load<u32>(p + 0x10);
    copyInto(dst, p + 0x18, name);
    return p;
}
VERIFY(0x026106B4, holder2Ctor);

/* 02612374: "Stage/<arc>_<name>.szs" */
void stagePath(u32 self, u32 out, u32 arc, u32 name) {
    WWHD_FUNC(0x02612374, void, self, out, arc, name);
    call_ptr<void>(load<u32>(load<u32>(arc + 4) + 0x14), arc);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 arcTop = load<u32>(arc);
    call_ptr<void>(fn, name);
    call<void>(0x02759C28, out, 0x100E23ECu, arcTop, load<u32>(name));
}
VERIFY(0x02612374, stagePath);

/* 026123FC: the archive resource of a loaded name (0 when not loaded) */
u32 getArchive(u32 self, u32 name) {
    WWHD_FUNC(0x026123FC, u32, self, name);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    if (!n || !(n + 0x18)) return 0;
    return load<u32>(load<u32>(n + 0x18) + 0x4C);
}
VERIFY(0x026123FC, getArchive);

/* 02612450: file of an archive resource (size through outSize) */
u32 getFile(u32 res, u32 file, u32 outSize) {
    WWHD_FUNC(0x02612450, u32, res, file, outSize);
    Local<u32[2]> info;
    store<u32>(info.a, 0);
    store<u32>(info.a + 4, 0);
    const u32 r = call_ptr<u32>(load<u32>(load<u32>(res + 0x10) + 0x3C), res, file, info.a);
    if (r && outSize) store<u32>(outSize, load<u32>(info.a + 4));
    return r;
}
VERIFY(0x02612450, getFile);

/* 026124B0: file of a named archive */
u32 getArchiveFile(u32 self, u32 name, u32 file, u32 outSize) {
    WWHD_FUNC(0x026124B0, u32, self, name, file, outSize);
    return getFile(getArchive(self, name), file, outSize);
}
VERIFY(0x026124B0, getArchiveFile);

/* 026124F4: getFile with a leading unused argument */
u32 getFile2(u32 unused, u32 res, u32 file, u32 outSize) {
    WWHD_FUNC(0x026124F4, u32, unused, res, file, outSize);
    return getFile(res, file, outSize);
}
VERIFY(0x026124F4, getFile2);

/* 02612504: is the archive loaded? */
u32 isLoaded(u32 self, u32 name) {
    WWHD_FUNC(0x02612504, u32, self, name);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    return (n ? n + 0x18 : 0) != 0;
}
VERIFY(0x02612504, isLoaded);

/* 02612540: is the archive in a loaded pack? */
u32 isInPack(u32 self, u32 name) {
    WWHD_FUNC(0x02612540, u32, self, name);
    Local<u32> key;
    store<u32>(key.a, hashString(name));
    const u32 n = call<u32>(0x0260ACC8, self + 0xD228, load<u32>(self + 0xD228), key.a);
    return (n ? n + 0x18 : 0) != 0;
}
VERIFY(0x02612540, isInPack);

/* 0261261C: deletes a loaded archive's resource and holder */
u32 unloadArchive(u32 self, u32 name) {
    WWHD_FUNC(0x0261261C, u32, self, name);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    if (!n || !(n + 0x18)) return 0;
    const u32 v = n + 0x18;
    u32 holder = load<u32>(v);
    const u32 res = load<u32>(holder + 0x4C);
    if (res) {
        call_ptr<void>(load<u32>(load<u32>(res + 0x10) + 0x1C), res, 3u);
        holder = load<u32>(v);
    }
    if (holder) call_ptr<void>(load<u32>(load<u32>(holder + 0xC) + 0xC), holder, 3u);
    return 1;
}
VERIFY(0x0261261C, unloadArchive);

/* 026126C0: destroys the heap of a pack entry */
u32 freePackEntry(u32 self, u32 name) {
    WWHD_FUNC(0x026126C0, u32, self, name);
    Local<u32> key;
    store<u32>(key.a, hashString(name));
    const u32 n = call<u32>(0x0260ACC8, self + 0xD228, load<u32>(self + 0xD228), key.a);
    if (!n || !(n + 0x18)) return 0;
    const u32 heap = load<u32>(load<u32>(n + 0x18) + 0x50);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x24), heap);
    return 1;
}
VERIFY(0x026126C0, freePackEntry);

/* 026127D4: asynchronous object request (loader thread message 1) */
u32 requestObjectAsync(u32 self, u32 name, u32 heap) {
    WWHD_FUNC(0x026127D4, u32, self, name, heap);
    const u32 busy = load<u32>(self + 0x11248);
    u32 fromPack = 0;
    if (busy) fromPack = 1;
    else store<u32>(self + 0x11248, 1);
    const u32 r = self + 0x1124C;
    const u32 dst = load<u32>(r);
    call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
    s32 n = cstrLength(load<u32>(name));
    const s32 size = s32(load<u32>(r + 8));
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, name);
    call<void>(0xC0009988, dst, load<u32>(name), u32(n), 0u);
    store<u8>(dst + n, 0);
    objectPath(self, r + 0x3C, name);
    store<u8>(r + 0x8C, u8(fromPack));
    store<u32>(r + 0x88, heap);
    const u32 t = load<u32>(self + 0x11240);
    call_ptr<void>(load<u32>(load<u32>(t + 0xC) + 0x1C), t, 1u, 0u);
    return 1;
}
VERIFY(0x026127D4, requestObjectAsync);

/* 02612908: asynchronous stage request "<arc>_<name>" (loader thread message 1) */
u32 requestStageAsync(u32 self, u32 arc, u32 name, u32 heap) {
    WWHD_FUNC(0x02612908, u32, self, arc, name, heap);
    const u32 busy = load<u32>(self + 0x11248);
    u32 fromPack = 0;
    if (busy) fromPack = 1;
    else store<u32>(self + 0x11248, 1);
    call_ptr<void>(load<u32>(load<u32>(arc + 4) + 0x14), arc);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 arcTop = load<u32>(arc);
    call_ptr<void>(fn, name);
    const u32 r = self + 0x1124C;
    call<void>(0x02759C28, r, 0x100E23FCu, arcTop, load<u32>(name));
    stagePath(self, r + 0x3C, arc, name);
    store<u8>(r + 0x8C, u8(fromPack));
    store<u32>(r + 0x88, heap);
    const u32 t = load<u32>(self + 0x11240);
    call_ptr<void>(load<u32>(load<u32>(t + 0xC) + 0x1C), t, 1u, 0u);
    return 1;
}
VERIFY(0x02612908, requestStageAsync);

/* 02612A80: starts the permanent pack thread (message 2) when idle */
u32 startPackLoad(u32 self) {
    WWHD_FUNC(0x02612A80, u32, self);
    if (load<u32>(self + 0x11248)) return 0;
    const u32 t = load<u32>(self + 0x11244);
    store<u32>(self + 0x11248, 2);
    call_ptr<void>(load<u32>(load<u32>(t + 0xC) + 0x1C), t, 2u, 0u);
    return 1;
}
VERIFY(0x02612A80, startPackLoad);

/* 02612AE8: asynchronous stage pack scan (loader thread message 3) when idle */
u32 requestStagePackAsync(u32 self, u32 arc, u32 name, u32 heap) {
    WWHD_FUNC(0x02612AE8, u32, self, arc, name, heap);
    if (load<u32>(self + 0x11248)) return 0;
    store<u32>(self + 0x11248, 1);
    call_ptr<void>(load<u32>(load<u32>(arc + 4) + 0x14), arc);
    const u32 fn = load<u32>(load<u32>(name + 4) + 0x14);
    const u32 arcTop = load<u32>(arc);
    call_ptr<void>(fn, name);
    const u32 r = self + 0x1124C;
    call<void>(0x02759C28, r, 0x100E2404u, arcTop, load<u32>(name));
    stagePath(self, r + 0x3C, arc, name);
    store<u32>(r + 0x88, heap);
    store<u8>(r + 0x8C, 0);
    const u32 t = load<u32>(self + 0x11240);
    call_ptr<void>(load<u32>(load<u32>(t + 0xC) + 0x1C), t, 3u, 0u);
    return 1;
}
VERIFY(0x02612AE8, requestStagePackAsync);

/* 02612BE0: the 2D permanent pack name of the console language */
u32 languagePack() {
    WWHD_FUNC(0x02612BE0, u32);
    const u32 cfg = load<u32>(0x101F4BAC);
    const u32 region = load<u32>(cfg + 0x10);
    if (region == 1) return 0x1048DD4C;
    if (region == 2) {
        const u32 lang = load<u32>(cfg + 0x14);
        if (lang == 2) return 0x1048DD5C;
        if (lang == 5) return 0x1048DD64;
        return 0x1048DD54;
    }
    if (region == 4) {
        const u32 lang = load<u32>(cfg + 0x14);
        if (lang >= 1 && lang <= 5) return load<u32>(0x100E2408 + 4 * lang);
    }
    return 0x1048DD54;
}
VERIFY(0x02612BE0, languagePack);

/* 02612E64: loads an archive (from a loaded pack, else from the file system) under its name */
u32 loadArchive(u32 self, u32 name, u32 path, u32 heap, u32 compressed, u32 align, u32 bufAlign) {
    WWHD_FUNC(0x02612E64, u32, self, name, path, heap, compressed, align, bufAlign);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    if (n && n + 0x18) return 1;
    Local<u32> key;
    store<u32>(key.a, hashString(name));
    const u32 m = self + 0xD228;
    const u32 e = call<u32>(0x0260ACC8, m, load<u32>(m), key.a);
    u32 arc;
    if (e && e + 0x18) {
        arc = readPackEntry(e + 0x18, heap);
    } else if (compressed) {
        arc = call<u32>(0x02612C88, path, heap, load<u32>(m + 0x4014), align, bufAlign);
    } else {
        arc = call<u32>(0x02610B40, path, heap, align, bufAlign);
    }
    if (!arc) return 0;
    u32 holder = call<u32>(0x0273B050, 0x54u, heap, 4u);
    if (holder) holder = holderCtor(holder, name, arc, heap);
    insertNamed(self, name, holder);
    return 1;
}
VERIFY(0x02612E64, loadArchive);

/* 0261321C: removes a name from the archive map */
void eraseArchive(u32 self, u32 name) {
    WWHD_FUNC(0x0261321C, void, self, name);
    const u32 n = call<u32>(0x02613F20, self + 0x14, load<u32>(self + 0x14), name);
    if (!n || !(n + 0x18)) return;
    const u32 r = call<u32>(0x026142C4, self + 0x14, load<u32>(self + 0x14), name);
    store<u32>(self + 0x14, r);
    if (r) store<u8>(r + 8, 0);
}
VERIFY(0x0261321C, eraseArchive);

/* 02613294: removes a name from the pack entry map */
void erasePackEntry(u32 self, u32 name) {
    WWHD_FUNC(0x02613294, void, self, name);
    const u32 h = hashString(name);
    Local<u32[2]> keys;
    store<u32>(keys.a, h);
    const u32 m = self + 0xD228;
    const u32 root = load<u32>(m);
    store<u32>(keys.a + 4, h);
    const u32 n = call<u32>(0x0260ACC8, m, root, keys.a + 4);
    if (!n || !(n + 0x18)) return;
    const u32 r = call<u32>(0x0260AEFC, m, root, keys.a);
    store<u32>(m, r);
    if (r) store<u8>(r + 8, 0);
}
VERIFY(0x02613294, erasePackEntry);

/* 02613394: ResMgrThread message handler (1: archive request, 3: stage pack scan) */
void threadMain(u32 self, u32 thread, u32 msg) {
    WWHD_FUNC(0x02613394, void, self, thread, msg);
    const u32 st = self + 0x11248, r = self + 0x1124C, cs = self + 0x112DC;
    if (msg == 1) {
        if (load<u8>(self + 0x112D8)) {
            loadFromPack(self, r, r + 0x3C, load<u32>(r + 0x88));
            return;
        }
        call<void>(0x027601BC, cs);
        loadArchive(self, r, r + 0x3C, load<u32>(r + 0x88), 1u, load<u32>(self + 0x11318), 0x2000u);
        store<u32>(st, 0);
        call<void>(0x027601F0, cs);
    } else if (msg == 3) {
        call<void>(0x027601BC, cs);
        call<void>(0x02610820, self, r, self + 0x11288, load<u32>(self + 0x112D4), load<u32>(self + 0x11318));
        store<u32>(st, 0);
        call<void>(0x027601F0, cs);
    }
}
VERIFY(0x02613394, threadMain);

/* 026134B4: ResMgrPackLoadThread message handler (2: the three SZS permanent packs) */
void packThreadMain(u32 self, u32 thread, u32 msg) {
    WWHD_FUNC(0x026134B4, void, self, thread, msg);
    if (msg != 2) return;
    const u32 st = self + 0x11248, cs = st + 0x94;
    call<void>(0x027601BC, cs);
    Local<u32[2]> name;
    const u32 packs[3] = {0x100E2428, 0x100E2444, 0x100E2460};
    for (u32 i = 0; i < 3; i++) {
        store<u32>(name.a, packs[i]);
        store<u32>(name.a + 4, kSafeStringVt);
        call<void>(0x02611864, self, name.a, load<u32>(load<u32>(0x101F4F28) + 0x2128));
    }
    store<u32>(st, 0);
    call<void>(0x027601F0, cs);
}
VERIFY(0x026134B4, packThreadMain);

/* 02613580: SingletonDisposer deleting destructor (destroys the instance) */
void disposerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02613580, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E25C0);
    if (p == load<u32>(0x101F4F80)) {
        const u32 inst = load<u32>(kResMgr);
        store<u32>(0x101F4F80, 0);
        store<u32>(inst, 0x100E222C);
        call<void>(0x02760168, inst + 0x112DC, 2u);
        store<u32>(kResMgr, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02613580, disposerDtor);

/* 02613630: archive holder destructor (removes its map entry) */
void holderDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02613630, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E25D0);
    eraseArchive(load<u32>(kResMgr), p + 0x10);
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02613630, holderDtor);

/* 026136A0: pack holder destructor (removes its pack map entry) */
void holder2Dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x026136A0, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E25E0);
    erasePackEntry(load<u32>(kResMgr), p + 0x10);
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x026136A0, holder2Dtor);

/* the standard initialiser of these TUs: the shared static objects */
static void stdInit(u32 b, u32 r1, u32 lo, u32 hi, u32 f, u32 r2, u32 r3) {
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, r1);
    const f32 a = load<f32>(lo), c = load<f32>(hi);
    store<f32>(f, a);
    store<f32>(f + 4, c);
    call<void>(0x028ED6F8, f + 8);
    call<void>(0x028F026C, r2);
    call<void>(0x028EAB2C, f + 9);
    call<void>(0x028F026C, r3);
}

/* 02613710: static initialiser (shared objects, the language pack names) */
void staticInit() {
    WWHD_FUNC(0x02613710, void);
    stdInit(0x1048DDA0, 0x101F4F58, 0x100E247C, 0x100E2480, 0x1048DD94, 0x101F4F64, 0x101F4F70);
    const u32 vt = kSafeStringVt;
    store<u32>(0x1048DD50, vt);
    store<u32>(0x1048DD4C, 0x100E24E4);
    store<u32>(0x1048DD58, vt);
    store<u32>(0x1048DD54, 0x100E2508);
    store<u32>(0x1048DD60, vt);
    store<u32>(0x1048DD68, vt);
    store<u32>(0x1048DD70, vt);
    store<u32>(0x1048DD78, vt);
    store<u32>(0x1048DD80, vt);
    store<u32>(0x1048DD7C, 0x100E24C4);
    store<u32>(0x1048DD64, 0x100E252C);
    store<u32>(0x1048DD74, 0x100E24A4);
    store<u32>(0x1048DD88, vt);
    store<u32>(0x1048DD84, 0x100E2574);
    store<u32>(0x1048DD6C, 0x100E2550);
    store<u32>(0x1048DD90, vt);
    store<u32>(0x1048DD5C, 0x100E2484);
    store<u32>(0x1048DD8C, 0x100E2598);
}
VERIFY(0x02613710, staticInit);

/* 02614AB8 / 02614B4C: initialisers of two TUs without code of their own */
void staticInit2() {
    WWHD_FUNC(0x02614AB8, void);
    stdInit(0x1048DDBC, 0x101F4F84, 0x100E25F0, 0x100E25F4, 0x1048DDB0, 0x101F4F90, 0x101F4F9C);
}
VERIFY(0x02614AB8, staticInit2);
void staticInit3() {
    WWHD_FUNC(0x02614B4C, void);
    stdInit(0x1048DDD8, 0x101F4FA8, 0x100E25F8, 0x100E25FC, 0x1048DDCC, 0x101F4FB4, 0x101F4FC0);
}
VERIFY(0x02614B4C, staticInit3);

} // namespace hd_res_mgr
