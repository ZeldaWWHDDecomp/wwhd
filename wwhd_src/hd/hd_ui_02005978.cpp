/* hd_ui_02005978: three HD TUs: layout child object, the UI graphics singleton and the application entry (no GameCube source).
 *
 * 02005978..02005B3B: the 0x3C layout child created by hd_ui_020021FC's attach (three SafeStrings: empty,
 *                     the owner's name (+0x1C), the owner's text (+0xC)+0x80; flag +0x10; owner +0x1C,
 *                     value +0x20, a 16-byte rect +0x24); __sinit 02005A90 + companions
 * 02005B3C..02005EA7: the UI graphics singleton (0x24, sead singleton pattern: instance 1018C404,
 *                     disposer 1018C408): create, setup of the lyt graphics resource (0x310), the font
 *                     (0x168) and the draw info (0x68), disposer destructor; __sinit 02005DC4 also sets the
 *                     two built-in shader paths (font_BuildinShader.gsh / lyt_BuildinShader.gsh)
 * 02005EA8..02006293: the application entry (main): system init, root heap, sead::GameFrameworkCafe
 *                     ("FrameworkCafe"), graphics system ("GraphicsSystem"), and run with the root task
 *                     entry 02006294; __sinit 020061E8 + companions
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_02005978 {

/* 02005978: layout child constructor (rect, owner, value) */
static u32 Child_ct(u32 self, u32 rect, u32 owner, u32 value) {
    WWHD_FUNC(0x02005978, u32, self, rect, owner, value);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x3C);
        if (t == 0) return 0;
    }
    /* SafeString "" at +0 (its null check is dead) */
    st(t + 4, 0x10000980);
    st(t + 0, 0x10000160);
    u32 a = t + 8;
    u32 nm = ld(owner + 0x1C);
    if (a == 0) a = op_new(8);
    if (a != 0) {
        st(a + 0, nm);
        st(a + 4, 0x10000980);
    }
    stb(t + 0x10, 1);
    u32 b = t + 0x14;
    u32 tx = ld(owner + 0xC);
    if (b == 0) b = op_new(8);
    if (b != 0) {
        st(b + 4, 0x10000980);
        st(b + 0, tx + 0x80);
    }
    st(t + 0x20, value);
    st(t + 0x1C, owner);
    st(t + 0x24, ld(rect + 0));
    st(t + 0x28, ld(rect + 4));
    st(t + 0x2C, ld(rect + 8));
    st(t + 0x34, 0);
    st(t + 0x30, ld(rect + 0xC));
    st(t + 0x38, 0);
    return t;
}
VERIFY(0x02005978, Child_ct);

static void Child_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02005A7C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02005A7C, Child_dt);

static void sinit_02005A90() {
    WWHD_FUNC(0x02005A90, void);
    header_sinit(0x101FF2B8, 0x1018C3BC, 0x10000998);
}
VERIFY(0x02005A90, sinit_02005A90);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02005B24, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02005B24, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x02005B38, void);
}
VERIFY(0x02005B38, Comp_empty);

/* 02005B3C: graphics singleton constructor */
static u32 Gfx_ct(u32 self) {
    WWHD_FUNC(0x02005B3C, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x24);
        if (t == 0) return 0;
    }
    st(t + 0x10, 0);
    st(t + 0x20, 0x10000A20);
    st(t + 0x18, 0);
    st(t + 0x1C, 0);
    st(t + 0x14, 0);
    return t;
}
VERIFY(0x02005B3C, Gfx_ct);

/* 02005B90: createInstance(heap) (sead singleton: disposer first, then the instance) */
static void Gfx_createInstance(u32 heap) {
    WWHD_FUNC(0x02005B90, void, heap);
    if (ld(0x1018C404) != 0) return;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x24, heap, 4);
    if (d != 0) {
        gabi::call(0x02752B0C, d, heap, 3);
        st(d + 0xC, 0x10000A10);
    }
    st(0x1018C408, d);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x02005B3C, d);
    st(0x1018C404, r);
}
VERIFY(0x02005B90, Gfx_createInstance);

static void Gfx_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02005C24, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02005C24, Gfx_dt);

/* 02005C38: setup (heap, a, b): lyt allocator, graphics resource, font, draw info */
static void Gfx_setup(u32 self, u32 heap, u32 a, u32 b) {
    WWHD_FUNC(0x02005C38, void, self, heap, a, b);
    st(self + 0x10, heap);
    u32 alloc = gabi::call<u32>(0x020039C0, 0, heap);
    gabi::call(0x02874890, alloc);
    gabi::call(0x0274FBF8, ld(0x101F8B18));
    u32 p = gabi::call<u32>(0x0273B050, 0x310, ld(self + 0x10), 4);
    if (p != 0) p = gabi::call<u32>(0x0287EB18, p);
    st(self + 0x14, p);
    gabi::call(0x0287ECB4, p, 0x200, a);
    u32 q = gabi::call<u32>(0x0273B050, 0x168, ld(self + 0x10), 4);
    if (q != 0) q = gabi::call<u32>(0x0287D924, q);
    st(self + 0x1C, q);
    gabi::call(0x0287E4C4, q, ld(self + 0x14), b);
    gabi::call(0x0274FCCC, ld(0x101F8B18));
    u32 w = gabi::call<u32>(0x0273B050, 0x68, ld(self + 0x10), 4);
    if (w != 0) w = gabi::call<u32>(0x0286DED4, w);
    st(self + 0x18, w);
}
VERIFY(0x02005C38, Gfx_setup);

/* 02005D1C: singleton disposer destructor (destroys the instance it guards) */
static void GfxDisposer_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02005D1C, void, self, flags);
    if (self == 0) return;
    st(self + 0xC, 0x10000A10);
    if (self == ld(0x1018C408)) {
        u32 inst = ld(0x1018C404);
        st(0x1018C408, 0);
        gabi::call_ptr(vfn(inst, 0x20, 0xC), inst, 2);
        st(0x1018C404, 0);
    }
    gabi::call(0x02752BEC, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02005D1C, GfxDisposer_dt);

/* 02005DC4: __sinit (header statics + the two built-in shader path SafeStrings) */
static void sinit_02005DC4() {
    WWHD_FUNC(0x02005DC4, void);
    header_sinit(0x101FF2E4, 0x1018C3E0, 0x100009B8);
    st(0x101FF2D4, 0x100009A0);
    st(0x101FF2D0, 0x100009E8);
    st(0x101FF2CC, 0x100009A0);
    st(0x101FF2C8, 0x100009C0);
}
VERIFY(0x02005DC4, sinit_02005DC4);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x02005E90, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02005E90, Comp_dt2);

static void Comp_empty2() {
    WWHD_FUNC(0x02005EA4, void);
}
VERIFY(0x02005EA4, Comp_empty2);

struct w2_l { u8 b[8]; };
struct params_l { u8 b[0x2C]; };  /* framework parameters (+0x24: the "FrameworkCafe" SafeString) */
struct big_l { u8 b[0x80]; };

static inline f32 u2f(u32 v) { return (f32)(f64)v; } /* lfd magic 0x4330000000000000 - fsub - frsp */
static inline u32 root_heap() { return ld(0x104A030C) != 0 ? ld(ld(0x104A0314)) : 0; }

/* 02005EA8: application entry: init, heaps, framework, graphics system, run */
static s32 hd_main(s32 argc, u32 argv) {
    WWHD_FUNC(0x02005EA8, s32, argc, argv);
    gabi::call(0x028EA09C);
    gabi::call(0x02034380);
    gabi::call(0x02034884);
    /* objects at the original's frame offsets (stwu r1,-0x118): callees run at its sp (game test
     * 2026-10-06: the empty save slots' stale bytes) */
    gabi::FrameLocal<w2_l> arena(0x54);
    gabi::call(0x027478C4, arena.a);
    u32 bh = gabi::call<u32>(0xC0009790, 1);                 /* MEMGetBaseHeapHandle(MEM2) */
    if (ld(bh) == 0x45585048u) {                             /* 'EXPH' */
        gabi::call(0xC00097D8, bh);                          /* MEMGetTotalFreeSizeForExpHeap */
        gabi::call(0xC0009778, bh, 4);                       /* MEMGetAllocatableSizeForExpHeapEx */
    }
    st(arena.a, 0x3B7780C4);
    gabi::call(0x0274B1F0, arena.a);
    gabi::call(0x0203E584);
    gabi::call(0x0203E6A4);
    gabi::FrameLocal<params_l> p(0x20);
    st(p.a + 0x14, 0x500);
    st(p.a + 0x18, 0x2D0);
    u32 w0 = ld(0x104A01DC), w1 = ld(0x104A01E0);
    st(p.a + 4, w0);
    stb(p.a + 0x20, 1);
    st(p.a + 0, 1);
    st(p.a + 0x1C, 0x400000);
    u32 w3 = ld(0x104A01E8), w2 = ld(0x104A01E4);
    st(p.a + 8, w1);
    st(p.a + 0xC, w2);
    st(p.a + 0x10, w3);
    st(p.a + 0x14, gabi::call<u32>(0x02739398));
    st(p.a + 0x18, gabi::call<u32>(0x027393E8));
    stb(p.a + 0x20, 0);
    u32 rh = ld(0x104A030C);
    st(p.a + 0x24, 0x10000A58);                              /* "FrameworkCafe" */
    st(p.a + 0, 2);
    st(p.a + 0x28, 0x10000A34);
    u32 parent = rh != 0 ? ld(ld(0x104A0314)) : 0;
    u32 h = gabi::call<u32>(0x02753004, 0, p.a + 0x24, parent, 1, 0);
    u32 fw = gabi::call<u32>(0x0273B050, 0x390, h, 4);
    if (fw != 0) fw = gabi::call<u32>(0x02034C10, fw, p.a);
    gabi::call_ptr(vfn(h, 0xC, 0x2C), h);
    gabi::call(0x02756B94, 0x104A0380, root_heap(), 0x64, 0x2800000, 0);
    gabi::FrameLocal<w2_l> gname(0x5C);
    u32 rh2 = ld(0x104A030C);
    st(gname.a + 4, 0x10000A34);
    st(gname.a + 0, 0x10000A68);                             /* "GraphicsSystem" */
    u32 parent2 = rh2 != 0 ? ld(ld(0x104A0314)) : 0;
    u32 g = gabi::call<u32>(0x02753004, 0, gname.a, parent2, 1, 0);
    gabi::FrameLocal<w2_l> sz(0x10);
    gabi::FrameLocal<w2_l> sz2(0x18);
    u32 pw = ld(p.a + 0x14), ph = ld(p.a + 0x18);
    stf(sz.a + 0, u2f(pw));
    stf(sz.a + 4, u2f(ph));
    f32 dw = u2f(gabi::call<u32>(0x027394D8));
    u32 dhi = gabi::call<u32>(0x02739528);
    f32 dh = u2f(dhi);
    if (u32 nsp = gabi::native_sp()) { st(nsp + 8, 0x43300000); st(nsp + 0xC, dhi); } /* the original's lfd conversion scratch */
    stf(sz2.a + 0, dw);
    stf(sz2.a + 4, dh);
    gabi::call(0x0274B40C, fw, g, argc, argv, sz.a, sz2.a, ld(0x104A0380));
    gabi::call_ptr(vfn(g, 0xC, 0x2C), g);
    u32 r = root_heap();
    gabi::call_ptr(vfn(r, 0xC, 0x7C), r, 4);
    gabi::FrameLocal<w2_l> d(0x4C);
    gabi::FrameLocal<big_l> e(0x6C);
    st(d.a + 0, 2);
    st(d.a + 4, 0x02006294);                                 /* root task entry */
    gabi::call(0x02748B98, e.a, d.a);
    u32 run = ld(ld(fw + 0x24) + 0x24);
    gabi::FrameLocal<w2_l> f(0x64);
    u32 arg = gabi::call<u32>(0x02747908, f.a);
    /* on the main loop's stack when a save state is taken (frame-exact chain call): LR 020061C4 and
     * the callee-saved registers as the original holds them at its bctrl (r25 GraphicsSystem heap,
     * r26 104A030C, r27 104A0000, r28 the 0x43300000 conversion word, r29 run, r30 root heap,
     * r31 framework, f31 = frsp of the display width: ps0 = ps1) */
    gabi::call_ptr_site(0x020061C4, {{25, g}, {26, 0x104A030Cu}, {27, 0x104A0000u}, {28, 0x43300000u}, {29, run}, {30, r}, {31, fw}},
                        {{31, (f64)dw, (f64)dw}}, run, fw, r, e.a, arg);
    return 0;
}
VERIFY(0x02005EA8, hd_main);

static void sinit_020061E8() {
    WWHD_FUNC(0x020061E8, void);
    header_sinit(0x101FF300, 0x1018C40C, 0x10000A7C);
}
VERIFY(0x020061E8, sinit_020061E8);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x0200627C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0200627C, Comp_dt3);

static void Comp_empty3() {
    WWHD_FUNC(0x02006290, void);
}
VERIFY(0x02006290, Comp_empty3);

}  // namespace hd_ui_02005978
