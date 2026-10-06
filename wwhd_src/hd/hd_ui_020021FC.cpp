/* hd_ui_020021FC: four HD UI TUs around the layout screen loader (no GameCube source).
 *
 * 020021FC..020023CB: a 0x104 NW4F-lyt-derived object (lyt ctors 0287795C / 02877DB8, dtor 02877FE8,
 *                     vtable 100002C8 at +8); __sinit 02002338
 * 020023CC..02002697: layout child holder (0xC, vtable 100003A8) with its sead RTTI static and a
 *                     DynamicCast-based attach (02005978 / 020042A4); __sinit 02002604
 * 02002698..02002803: a 0xC0 NW4F-lyt-derived object (02874AAC / 02874C64, vtable 100003C8); __sinit 02002770
 * 02002804..020032F3: the layout screen (0xE4, vtable 100004F8 at +0xE0): resource accessor at +4,
 *                     layout at +8, lyt object +0xC, animation set +0xD4, pane-pointer table +0xD8/+0xDC;
 *                     loads "%s.bflyt"; __sinit 020031F4; sead SafeString / deleting-dtor companions after it */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_020021FC {

static u32 LytA_ct3(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x020021FC, u32, self, a, b, c);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x104);
        if (t == 0) return 0;
    }
    gabi::call(0x0287795C, t, a, b, c);
    st(t + 8, 0x100002C8);
    return t;
}
VERIFY(0x020021FC, LytA_ct3);

static u32 LytA_ct1(u32 self, u32 a) {
    WWHD_FUNC(0x02002280, u32, self, a);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x104);
        if (t == 0) return 0;
    }
    gabi::call(0x02877DB8, t, a);
    st(t + 8, 0x100002C8);
    return t;
}
VERIFY(0x02002280, LytA_ct1);

static void LytA_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x020022E4, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02877FE8, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x020022E4, LytA_dt);

static void sinit_02002338() {
    WWHD_FUNC(0x02002338, void);
    header_sinit(0x101FF168, 0x1018C20C, 0x100002C0);
}
VERIFY(0x02002338, sinit_02002338);

/* 020023CC: Holder::getRuntimeTypeInfoStatic() */
static u32 Holder_rtti() {
    WWHD_FUNC(0x020023CC, u32);
    return rtti_static3(0x101FD8F0, 0x101FD8F4, 0x101FD89C, 0x101FD8F8, 0x101FD888, 0x101FD8FC);
}
VERIFY(0x020023CC, Holder_rtti);

/* 02002450: holder constructor (owner, layout) */
static u32 Holder_ct(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02002450, u32, self, a, b);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xC);
        if (t == 0) return 0;
    }
    st(t + 8, b);
    st(t + 4, a);
    st(t + 0, 0x100003A8);
    return t;
}
VERIFY(0x02002450, Holder_ct);

static void Holder_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x020024B0, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020024B0, Holder_dt);

/* 020024C4: attach: cast OBJ to the pane-owner type (RTTI 101FD900) and give its target (+0x34) a new
 * 0x3C child built from (ARG, OBJ, this->+4) */
static void Holder_attach(u32 self, u32 arg, u32 obj) {
    WWHD_FUNC(0x020024C4, void, self, arg, obj);
    rtti_static2(0x101FD8C4, 0x101FD900, 0x101FD894, 0x101FD904);
    u32 o = dyncast(obj, 0x30, 0xC, 0x101FD900);
    u32 target = ld(o + 0x34); /* o may be null: the original reads 0x34 then */
    if (target == 0) return;
    u32 heap = ld(ld(0x1018C404) + 0x10);
    u32 p = gabi::call<u32>(0x0273B050, 0x3C, heap, 4);
    if (p != 0) p = gabi::call<u32>(0x02005978, p, arg, o, ld(self + 4));
    gabi::call(0x020042A4, target, p);
}
VERIFY(0x020024C4, Holder_attach);

static void sinit_02002604() {
    WWHD_FUNC(0x02002604, void);
    header_sinit(0x101FF184, 0x1018C230, 0x100003A0);
}
VERIFY(0x02002604, sinit_02002604);

static u32 LytB_ct(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x02002698, u32, self, a, b, c);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xC0);
        if (t == 0) return 0;
    }
    gabi::call(0x02874AAC, t, a, b, c);
    st(t + 8, 0x100003C8);
    return t;
}
VERIFY(0x02002698, LytB_ct);

static void LytB_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0200271C, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02874C64, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0200271C, LytB_dt);

static void sinit_02002770() {
    WWHD_FUNC(0x02002770, void);
    header_sinit(0x101FF1A0, 0x1018C254, 0x100003C0);
}
VERIFY(0x02002770, sinit_02002770);

/* 02002804: Screen::getRuntimeTypeInfoStatic() */
static u32 Screen_rtti() {
    WWHD_FUNC(0x02002804, u32);
    return rtti_static3(0x101FD908, 0x101FD90C, 0x101FD898, 0x101FD910, 0x101FD888, 0x101FD8FC);
}
VERIFY(0x02002804, Screen_rtti);

/* 02002888: screen constructor */
static u32 Screen_ct(u32 self) {
    WWHD_FUNC(0x02002888, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xE4);
        if (t == 0) return 0;
    }
    st(t + 8, 0);
    st(t + 4, 0);
    st(t + 0xE0, 0x100004F8);
    st(t + 0, 0);
    gabi::call(0x02873D70, t + 0xC);
    u32 tab = t + 0xD8;
    st(t + 0xD4, 0);
    if (tab == 0) tab = op_new(8);
    if (tab != 0) {
        st(tab + 4, 0);
        st(tab + 0, 0);
    }
    return t;
}
VERIFY(0x02002888, Screen_ct);

static void Screen_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02002918, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02873FD0, self + 0xC, 2);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02002918, Screen_dt);

/* 0200296C: set the lyt object's resource from X (02874D83C converts it) */
static void Screen_setRes(u32 self, u32 x) {
    WWHD_FUNC(0x0200296C, void, self, x);
    if (x == 0) return;
    u32 r = gabi::call<u32>(0x0274D83C, x);
    gabi::call(0x02873FE4, self + 0xC, r);
}
VERIFY(0x0200296C, Screen_setRes);

/* 020029AC: copy a 0x30-byte block (12 words) to +0x4C */
static void Screen_setBlock(u32 self, u32 src) {
    WWHD_FUNC(0x020029AC, void, self, src);
    if (src == 0) return;
    for (u32 i = 0; i < 12; ++i) st(self + 0x4C + i * 4, ld(src + i * 4));
}
VERIFY(0x020029AC, Screen_setBlock);

/* 020029EC: build: record the system word, set the resource and the block */
static void Screen_setup(u32 self, u32 res, u32 block) {
    WWHD_FUNC(0x020029EC, void, self, res, block);
    st(self + 0xB4, ld(ld(0x1018C404) + 0x14));
    gabi::call(0x0200296C, self, res);
    gabi::call(0x020029AC, self, block);
}
VERIFY(0x020029EC, Screen_setup);

static inline u32 svf(u32 self, u32 off) { return vfn(self, 0xE0, off); }

/* 02002A40: build with optional archive (r10) through the screen's virtuals */
static s32 Screen_build(u32 self, u32 a4, u32 a5, u32 res, u32 block, u32 v8, u32 v9, u32 arc) {
    WWHD_FUNC(0x02002A40, s32, self, a4, a5, res, block, v8, v9, arc);
    (void)a4; (void)a5;
    if (arc == 0) {
        if (gabi::call_ptr<u32>(svf(self, 0x3C), self) == 0) return 0;
        if (gabi::call_ptr<u32>(svf(self, 0x4C), self, v8, v9) == 0) return 0;
        gabi::call(0x020029EC, self, res, block);
        return 1;
    }
    if (lbz(arc + 0x10) == 0) {
        gabi::call(0x020029EC, self, res, block);
        return 1;
    }
    if (gabi::call_ptr<u32>(svf(self, 0x44), self, arc) == 0) return 0;
    if (gabi::call_ptr<u32>(svf(self, 0x4C), self, v8, v9) == 0) return 0;
    gabi::call_ptr(svf(self, 0x5C), self, arc);
    gabi::call_ptr(svf(self, 0x54), self, arc);
    gabi::call(0x020029EC, self, res, block);
    return 1;
}
VERIFY(0x02002A40, Screen_build);

/* 02002B84: release the pane table, the animation set, the layout and the resource accessor */
static void Screen_release(u32 self) {
    WWHD_FUNC(0x02002B84, void, self);
    s32 n = (s32)ld(self + 0xD8);
    if (n > 0 && ld(self + 0xDC) != 0) { /* the count fix-up for n <= 0 is dead */
        u32 tab = ld(self + 0xDC);
        u32 h = gabi::call<u32>(0x02755FEC, ld(0x101F8B4C), tab); /* r4 still holds the table */
        u32 fn = vfn(h, 0xC, 0x3C);
        gabi::call_ptr(fn, h, ld(self + 0xDC));
        st(self + 0xD8, 0);
        st(self + 0xDC, 0);
    }
    if (ld(self + 0xD4) != 0) {
        gabi::call(0x02004CAC, ld(self + 0xD4));
        u32 a = ld(self + 0xD4);
        if (a != 0) gabi::call_ptr(vfn(a, 0x14, 0xC), a, 3);
        st(self + 0xD4, 0);
    }
    u32 l = ld(self + 8);
    if (l != 0) {
        gabi::call_ptr(vfn(l, 0, 0xC), l, 3);
        st(self + 8, 0);
    }
    u32 r = ld(self + 4);
    if (ld(ld(r + 0xC) + 0xC) == 0 && r != 0) {
        gabi::call_ptr(vfn(r, 0x30, 0x14), r, 3);
        st(self + 4, 0);
    }
}
VERIFY(0x02002B84, Screen_release);

/* 02002C90: draw through the resource accessor (when it is not busy) */
static void Screen_draw(u32 self) {
    WWHD_FUNC(0x02002C90, void, self);
    u32 r = ld(self + 4);
    if (ld(ld(r + 0xC) + 0xC) != 0) return;
    gabi::call_ptr(vfn(r, 0x30, 0x4C), r);
    r = ld(self + 4);
    gabi::call_ptr(vfn(r, 0x30, 0x64), r, self + 0xC, 0);
    r = ld(self + 4);
    gabi::call_ptr(vfn(r, 0x30, 0x54), r);
}
VERIFY(0x02002C90, Screen_draw);

static u32 Screen_calc(u32 self) {
    WWHD_FUNC(0x02002D0C, u32, self);
    return gabi::call_ptr<u32>(svf(self, 0x34), self, self + 0xC);
}
VERIFY(0x02002D0C, Screen_calc);

/* 02002D20: draw with a fresh draw-info (two flags cleared, mode 2) */
static void Screen_drawWith(u32 self, u32 info) {
    WWHD_FUNC(0x02002D20, void, self, info);
    u32 r = ld(self + 4);
    if (ld(ld(r + 0xC) + 0xC) != 0) return;
    u32 d = ld(ld(0x1018C404) + 0x18);
    stb(d + 0, 0);
    stb(d + 1, 0);
    d = ld(ld(0x1018C404) + 0x18);
    st(d + 8, 2);
    gabi::call(0x0286DFB4, ld(ld(0x1018C404) + 0x18));
    r = ld(self + 4);
    gabi::call_ptr(vfn(r, 0x30, 0x6C), r, info);
}
VERIFY(0x02002D20, Screen_drawWith);

struct sbuf64_l { u8 b[0x4C]; }; /* sead::FixedSafeString<64>: {char*, vtable, capacity, char[64]} */

/* 02002DB4: load: create the resource accessor and the "<name>.bflyt" layout */
static u32 Screen_load(u32 self, u32 name, u32 arg) {
    WWHD_FUNC(0x02002DB4, u32, self, name, arg);
    u32 heap = ld(ld(0x1018C404) + 0x10);
    u32 p = gabi::call<u32>(0x0273B050, 0x248, heap, 4);
    if (p != 0) p = gabi::call<u32>(0x02003D84, p);
    st(self + 4, p);
    if (p == 0) return 0;
    gabi::Local<sbuf64_l> buf;
    u32 nvt = ld(name + 4);
    stb(buf.a + 0x4B, 0);
    stb(buf.a + 0xC, 0);
    st(buf.a + 8, 0x40);
    st(buf.a + 0, buf.a + 0xC);
    st(buf.a + 4, 0x100004C8);
    gabi::call_ptr(ld(nvt + 0x14), name);
    gabi::call(0x02759C28, buf.a, 0x100004E0, ld(name + 0));
    u32 q = gabi::call<u32>(0x0273B050, 0xC, heap, 4);
    if (q != 0) q = gabi::call<u32>(0x02002450, q, self, ld(self + 4));
    st(self + 8, q);
    if (q == 0) return 0;
    gabi::call_ptr(ld(ld(buf.a + 4) + 0x14), buf.a);
    return gabi::call<u32>(0x0287F640, ld(self + 4), ld(buf.a + 0), arg, ld(self + 8), 0);
}
VERIFY(0x02002DB4, Screen_load);

/* 02002F04: read a {+0x20, +0x1C} pair; false when +0x1C is 0 */
static u32 Screen_getPair(u32 out, u32 src) {
    WWHD_FUNC(0x02002F04, u32, out, src);
    u32 v = ld(src + 0x1C);
    st(out + 4, v);
    if (v == 0) return 0;
    st(out + 0, ld(src + 0x20));
    return 1;
}
VERIFY(0x02002F04, Screen_getPair);

/* 02002F30: create the animation set (0x18) for (a, b) */
static u32 Screen_createAnim(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02002F30, u32, self, a, b);
    if (a == 0 || b == 0) return 1;
    u32 p = gabi::call<u32>(0x0273B050, 0x18, ld(ld(0x1018C404) + 0x10), 4);
    if (p != 0) p = gabi::call<u32>(0x02004AC0, p);
    st(self + 0xD4, p);
    if (p == 0) return 0;
    return gabi::call<u32>(0x02004B70, p, ld(self + 4), a, b);
}
VERIFY(0x02002F30, Screen_createAnim);

/* 02002FE4: pane table: one entry per pane name of the resource (lhz +0x28), looked up by name */
static void Screen_bindPanes(u32 self, u32 res) {
    WWHD_FUNC(0x02002FE4, void, self, res);
    u32 n = lhz(res + 0x28);
    if (n == 0) return;
    u32 heap = ld(ld(0x1018C404) + 0x10);
    if (heap == 0) heap = gabi::call<u32>(0x02756140, ld(0x101F8B4C));
    u32 p = gabi::call_ptr<u32>(vfn(heap, 0xC, 0x34), heap, n << 2, 4);
    if (p != 0) {
        st(self + 0xDC, p);
        st(self + 0xD8, n);
    }
    s32 m = (s32)ld(self + 0xD8);
    for (s32 i = 0; i < m; ++i) st(ld(self + 0xDC) + i * 4, 0);
    u32 cnt = n > 1 ? n : 1;
    for (u32 i = 0; cnt != 0; --cnt, ++i) {
        u32 s = gabi::call<u32>(0x02873D24, res + 0x24, i);
        if (s == 0 || lbz(s) == 0) continue;
        u32 obj = ld(ld(self + 4) + 0xC);
        u32 size = ld(self + 0xD8);
        u32 fn = vfn(obj, 8, 0x5C);
        u32 slot = ld(self + 0xDC);
        if (i < size) slot += i * 4;
        u32 r = gabi::call_ptr<u32>(fn, obj, s, 1);
        st(slot, r);
    }
}
VERIFY(0x02002FE4, Screen_bindPanes);

/* 02003130: register the animation names of the resource (lhz +0x2A) with the animation set */
static void Screen_bindAnims(u32 self, u32 res) {
    WWHD_FUNC(0x02003130, void, self, res);
    u32 n = lhz(res + 0x2A);
    if (n == 0) return;
    u32 cnt = n > 1 ? n : 1;
    gabi::Local<be<u32>[2]> s;
    for (u32 i = 0; cnt != 0; --cnt, ++i) {
        u32 nm = gabi::call<u32>(0x02873D48, res + 0x24, i);
        if (nm == 0 || lbz(nm) == 0) continue;
        u32 set = ld(self + 0xD4);
        st(s.a + 4, 0x10000480);
        st(s.a + 0, nm);
        gabi::call(0x02004F50, set, i, s.a);
    }
}
VERIFY(0x02003130, Screen_bindAnims);

static void sinit_020031F4() {
    WWHD_FUNC(0x020031F4, void);
    header_sinit(0x101FF1BC, 0x1018C278, 0x100004EC);
}
VERIFY(0x020031F4, sinit_020031F4);

/* companions after the __sinit: deleting destructors, an empty virtual and the sead
 * BufferedSafeString::assureTerminationImpl_ instance (writes the last byte of the buffer) */
static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02003288, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02003288, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x0200329C, void);
}
VERIFY(0x0200329C, Comp_empty);

static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x020032A0, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x020032A0, SafeString_assureTermination);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x020032B8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020032B8, Comp_dt2);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x020032CC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020032CC, Comp_dt3);

static void Comp_dt4(u32 self, s32 flags) {
    WWHD_FUNC(0x020032E0, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020032E0, Comp_dt4);

}  // namespace hd_ui_020021FC
