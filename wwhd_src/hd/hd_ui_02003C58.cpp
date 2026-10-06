/* hd_ui_02003C58: two HD UI TUs: the layout resource accessor / pane builder and a lyt wrapper (no GameCube source).
 *
 * 02003C58..0200487F: the layout object of the HD UI (0x248 = NW4F lyt::Layout 0287F418 + owner +0x34,
 *                     resource accessor +0x38, vtable 10000810 at +0x30, child array {count +0x3C,
 *                     capacity +0x40, data +0x44} allocated by 0273B560 with 0x80 entries): sead RTTI static,
 *                     constructors, destructor, the BuildPaneObj override that creates the HD pane classes by
 *                     signature ('pic1' 0xC0, 'txt1' 0x104, 'wnd1' 0xD8, 'bnd1' 0xA4, 'prt1' 0xB0), a parts-layout
 *                     builder, a pane-tree builder and the "%s%02d" part-name binder; __sinit 02004794 + companions
 * 02004880..02004A3B: RTTI static (inlined elsewhere as 020024C4's check) and a 0xA4 lyt-derived pane
 *                     (028757F0 / 02875864, vtable 100008B8); __sinit 020049A8
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_02003C58 {

static u32 Layout_rtti() {
    WWHD_FUNC(0x02003C58, u32);
    return rtti_static3(0x101FD92C, 0x101FD930, 0x101FD8C0, 0x101FD934, 0x101FD888, 0x101FD8FC);
}
VERIFY(0x02003C58, Layout_rtti);

/* inline construction of the child array (its null check is dead unless this == -0x3C) */
static inline void child_array_ct(u32 t) {
    u32 a = t + 0x3C;
    if (a == 0) {
        a = op_new(0x20C);
        if (a == 0) return;
    }
    st(a + 8, 0);
    st(a + 0, 0);
    st(a + 4, 0);
    gabi::call(0x0273B560, a, 0x80, a + 0xC);
}

/* 02003CDC: constructor (owner, resource accessor) */
static u32 Layout_ct(u32 self, u32 owner, u32 acc) {
    WWHD_FUNC(0x02003CDC, u32, self, owner, acc);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x248);
        if (t == 0) return 0;
    }
    gabi::call(0x0287F418, t);
    st(t + 0x38, acc);
    st(t + 0x34, owner);
    st(t + 0x30, 0x10000810);
    child_array_ct(t);
    return t;
}
VERIFY(0x02003CDC, Layout_ct);

/* 02003D84: default constructor (its own accessor) */
static u32 Layout_ct0(u32 self) {
    WWHD_FUNC(0x02003D84, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x248);
        if (t == 0) return 0;
    }
    gabi::call(0x0287F418, t);
    st(t + 0x38, t);
    st(t + 0x34, 0);
    st(t + 0x30, 0x10000810);
    child_array_ct(t);
    return t;
}
VERIFY(0x02003D84, Layout_ct0);

/* 02003E1C: destructor: deletes the children, then the lyt base */
static void Layout_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02003E1C, void, self, flags);
    if (self == 0) return;
    s32 n = (s32)ld(self + 0x3C);
    st(self + 0x30, 0x10000810);
    for (s32 i = 0; i < n; ++i) {
        if ((u32)i >= (u32)n) continue;
        u32 e = ld(ld(self + 0x44) + i * 4);
        if (e == 0) continue;
        gabi::call(0x02005A7C, e, 3);
        n = (s32)ld(self + 0x3C);
    }
    st(self + 0x3C, 0);
    gabi::call(0x0287F4E0, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02003E1C, Layout_dt);

struct build_l { u8 b[0x34]; };

/* one pane kind: copy the 0x34-byte build argument block, allocate from the lyt allocator, construct */
static inline u32 build_kind(u32 size, u32 ctor, u32 res, u32 a6, u32 args) {
    gabi::Local<build_l> copy;
    for (u32 i = 0; i < 13; ++i) st(copy.a + i * 4, ld(args + i * 4));
    u32 p = gabi::call<u32>(0x0287F3DC, size, 4);
    if (p == 0) return 0;
    return gabi::call<u32>(ctor, p, res, a6, copy.a);
}

/* 02003EE8: BuildPaneObj override: HD pane classes by signature, the base for anything else */
static u32 Layout_buildPaneObj(u32 self, u32 kind, u32 res, u32 a6, u32 args) {
    WWHD_FUNC(0x02003EE8, u32, self, kind, res, a6, args);
    if (kind < 0x70727431u) {
        if (kind == 0x626E6431u) return build_kind(0xA4, 0x020048D0, res, a6, args);  /* bnd1 */
        if (kind == 0x70696331u) return build_kind(0xC0, 0x02002698, res, a6, args);  /* pic1 */
        return gabi::call<u32>(0x028807D0, self, kind, res, a6, args);
    }
    if (kind == 0x70727431u) return build_kind(0xB0, 0x020037D0, res, a6, args);      /* prt1 */
    if (kind == 0x74787431u) return build_kind(0x104, 0x020021FC, res, a6, args);     /* txt1 */
    if (kind == 0x776E6431u) return build_kind(0xD8, 0x02003AEC, res, a6, args);      /* wnd1 */
    return gabi::call<u32>(0x028807D0, self, kind, res, a6, args);
}
VERIFY(0x02003EE8, Layout_buildPaneObj);

/* 020041EC: build a parts layout: a child layout object sharing this accessor */
static u32 Layout_buildParts(u32 self, u32 a4, u32 a5, u32 a6) {
    WWHD_FUNC(0x020041EC, u32, self, a4, a5, a6);
    u32 r = gabi::call<u32>(0x02880AE0, self, a4, a5, a6);
    u32 acc = ld(self + 0x38);
    u32 p = gabi::call<u32>(0x0287F3DC, 0x248, 4);
    if (p != 0) p = gabi::call<u32>(0x02003CDC, p, self, acc);
    u32 fn = vfn(p, 0x30, 0x7C); /* p may be null: the original reads 0x30 then */
    gabi::call_ptr(fn, p, r, ld(self + 0x20), a6, a5);
    return p;
}
VERIFY(0x020041EC, Layout_buildParts);

/* 020042A4: append a child (ignored when full) */
static void Layout_addChild(u32 self, u32 c) {
    WWHD_FUNC(0x020042A4, void, self, c);
    s32 n = (s32)ld(self + 0x3C);
    if (n >= (s32)ld(self + 0x40)) return;
    st(ld(self + 0x44) + n * 4, c);
    st(self + 0x3C, ld(self + 0x3C) + 1);
}
VERIFY(0x020042A4, Layout_addChild);

struct res_l { u8 b[0x14]; };

/* 020042D0: build a pane from a resource (or none) and its group members */
static u32 Layout_buildPane(u32 self, u32 name, u32 group, u32 a6, u32 a7) {
    WWHD_FUNC(0x020042D0, u32, self, name, group, a6, a7);
    gabi::call_ptr(vfn(name, 4, 0x14), name);
    u32 r = gabi::call<u32>(0x0287F6D0, self, ld(name + 0));
    gabi::Local<res_l> res;
    gabi::call(0x028727E8, res.a, r);
    u32 data = ld(res.a + 4);
    u32 pane = 0;
    u32 g;
    if (data == 0) {
        gabi::call_ptr(vfn(group, 4, 0x14), group);
        g = gabi::call<u32>(0x02874820, ld(self + 0x10), ld(group + 0));
    } else {
        u32 p = gabi::call<u32>(0x0287F3DC, 0x34, 4);
        if (p == 0) {
            gabi::call_ptr(vfn(group, 4, 0x14), group);
            g = gabi::call<u32>(0x02874820, ld(self + 0x10), ld(group + 0));
        } else {
            pane = p;
            gabi::call(0x02872998, p);
            st(p + 0x30, 0);
            st(p + 0x14, 0x101800B8);
            gabi::Local<be<u32>> w;
            st(w.a, self + 4);
            gabi::call(0x0286DA70, self, w.a, p);
            gabi::call_ptr(vfn(p, 0x14, 0x44), p, data, ld(self + 0x20));
            gabi::call_ptr(vfn(group, 4, 0x14), group);
            g = gabi::call<u32>(0x02874820, ld(self + 0x10), ld(group + 0));
        }
    }
    u32 head = g + 0xC;
    for (u32 n = ld(g + 0xC); n != head; n = ld(n)) {
        gabi::call_ptr(vfn(self, 0x30, 0x9C), self, pane, ld(n + 8), a6);
    }
    gabi::call_ptr(vfn(pane, 0x14, 0x24), pane, a7);
    return pane;
}
VERIFY(0x020042D0, Layout_buildPane);

struct str2_l { u8 b[8]; };
struct sbuf29_l { u8 b[0x29]; };  /* FixedSafeString<29>: {char*, vtable, capacity, char[29]} */

/* 02004490: bind the numbered parts: for every part entry of PANE's resource whose name equals NAMES+0x80,
 * build "<name minus 2 chars><NN>" (NN = INDEX) and look it up in the layout's pane / material table */
static void Layout_bindParts(u32 self, u32 pane, u32 names, u32 index) {
    WWHD_FUNC(0x02004490, void, self, pane, names, index);
    u32 res = ld(pane + 8);
    u32 tab = res + ld(res + 0x10);
    gabi::Local<str2_l> a;
    gabi::Local<str2_l> b;
    gabi::Local<sbuf29_l> buf;
    for (u32 i = 0; i < lhz(res + 0xE); i = (i + 1) & 0xFFFF) {
        u32 entry = res + ld(tab + i * 4);
        st(b.a + 4, 0x100007AC);
        st(a.a + 4, 0x100007AC);
        st(a.a + 0, names + 0x80);
        st(b.a + 0, entry);
        gabi::call(0x0200483C, a.a);
        gabi::call_ptr(ld(ld(a.a + 4) + 0x14), a.a);
        u32 fb = ld(ld(b.a + 4) + 0x14);
        u32 pa = ld(a.a + 0);
        gabi::call_ptr(fb, b.a);
        if (pa != ld(b.a + 0)) {
            u32 p = ld(a.a + 0), q = ld(b.a + 0) - 1;
            bool eq = false;
            for (u32 k = 0; k < 0x40001; ++k) {
                u8 c1 = lbz(p);
                ++q;
                u8 c2 = lbz(q);
                if (c1 != c2) break;
                if (c1 == 0) { eq = true; break; }
                ++p;
            }
            if (!eq) continue;
        }
        u32 d = buf.a + 0xC;
        st(buf.a + 0, d);
        st(buf.a + 8, 0x1D);
        stb(d, 0);
        stb(d + 0x1C, 0);
        st(buf.a + 4, 0x100007DC);
        gabi::call(0x02759C28, buf.a, 0x100007FC, entry, 0x100007DC);
        gabi::call_ptr(ld(ld(buf.a + 4) + 0x14), buf.a);
        u32 data = ld(buf.a + 0);
        u32 len = 0, cut;
        u32 c = data;
        if (lbz(c) != 0) {
            for (;;) {
                ++len;
                ++c;
                if ((s32)len > 0x40000) { len = 0xFFFFFFFF; break; }
                if (lbz(c) == 0) break;
            }
        }
        if (len == 0xFFFFFFFF) cut = 0;
        else cut = len - (len < 2 ? len : 2);
        stb(data + cut, 0);
        gabi::call_ptr(ld(ld(buf.a + 4) + 0x14), buf.a);
        gabi::call(0x02759C28, buf.a, 0x10000800, ld(buf.a + 0), index);
        u32 flag = lbz(entry + 0x1D);
        u32 tbl = ld(ld(self + 0xC) + 8);
        u32 base = flag == 0 ? tbl + 0x58 : tbl + 0x60;
        gabi::call_ptr(ld(ld(buf.a + 4) + 0x14), buf.a);
        u32 fn = ld(base + 4);
        u32 obj = gabi::call_ptr<u32>(fn, ld(self + 0xC), ld(buf.a + 0), 1);
        if (obj == 0) continue;
        u32 ok = gabi::call<u32>(flag == 0 ? 0x02871710 : 0x0287174C, pane, obj, entry);
        if (ok == 0) return;
    }
}
VERIFY(0x02004490, Layout_bindParts);

static void sinit_02004794() {
    WWHD_FUNC(0x02004794, void);
    header_sinit(0x101FF264, 0x1018C350, 0x10000808);
}
VERIFY(0x02004794, sinit_02004794);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02004828, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02004828, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x0200483C, void);
}
VERIFY(0x0200483C, Comp_empty);

/* sead BufferedSafeString::assureTerminationImpl_ instance */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x02004840, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x02004840, SafeString_assureTermination);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x02004858, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02004858, Comp_dt2);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x0200486C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0200486C, Comp_dt3);

/* 02004880: pane-owner RTTI static (the check inlined into 020024C4) */
static u32 PaneOwner_rtti() {
    WWHD_FUNC(0x02004880, u32);
    rtti_static2(0x101FD8C4, 0x101FD900, 0x101FD894, 0x101FD904);
    return 0x101FD900;
}
VERIFY(0x02004880, PaneOwner_rtti);

static u32 LytE_ct(u32 self, u32 a0, u32 a1, u32 a2) {
    WWHD_FUNC(0x020048D0, u32, self, a0, a1, a2);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xA4);
        if (t == 0) return 0;
    }
    gabi::call(0x028757F0, t, a0, a1, a2);
    st(t + 8, 0x100008B8);
    return t;
}
VERIFY(0x020048D0, LytE_ct);

static void LytE_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02004954, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02875864, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02004954, LytE_dt);

static void sinit_020049A8() {
    WWHD_FUNC(0x020049A8, void);
    header_sinit(0x101FF280, 0x1018C374, 0x100008B0);
}
VERIFY(0x020049A8, sinit_020049A8);

}  // namespace hd_ui_02003C58
