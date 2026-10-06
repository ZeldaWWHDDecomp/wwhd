/* hd_olv_02036084: the HD JPEG encode manager translation unit (02036084..02036A63). No GameCube
 * source: HD-only code.
 *
 * A sead singleton (0x7C: IDisposer +0, thread +0x10, state +0x14, done/ok flags +0x18/+0x19, the save
 * job +0x1C..+0x2C, the decode job +0x30..+0x3C, critical section +0x40) running on a
 * "JpegEncodeMgrThread" sead::DelegateThread: job 1 writes a picture into the save's album (picture
 * slots of the Tingle bottle / Picto Box), job 2 decodes a downloaded Miiverse JPEG screenshot into a
 * GX2 texture (decode, GX2CopySurface into the destination, texture registers, lyt texture map).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_olv_02036084 {

static constexpr u32 GX2CalcSurfaceSizeAndAlignment = 0xC00060D0;
static constexpr u32 GX2CopySurface = 0xC0006120;
static constexpr u32 GX2InitTextureRegs = 0xC0006498;
static constexpr u32 OSBlockMove_ = 0xC0009988;
static constexpr u32 INST = 0x1018F3F4, DISP = 0x1018F3F8;

/* 02036084: constructor (0x7C; the IDisposer at +0 is built by createInstance) */
static u32 JpegMgr_ct(u32 self) {
    WWHD_FUNC(0x02036084, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x7C);
        if (t == 0) return 0;
    }
    st(t + 0x10, 0);
    st(t + 0x14, 3);
    stb(t + 0x19, 0);
    stb(t + 0x18, 0);
    gabi::call(0x028F521C, t + 0x1C, 0x14u);
    gabi::call(0x028F521C, t + 0x30, 0x10u);
    gabi::call(0x02760084, t + 0x40);
    return t;
}
VERIFY(0x02036084, JpegMgr_ct);

/* 020360FC: sead singleton createInstance(heap) */
static u32 JpegMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x020360FC, u32, heap);
    u32 cur = ld(INST);
    if (cur != 0) return cur;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x7Cu, heap, 4u);
    if (d != 0) {
        gabi::call(0x02752B0C, d, heap, 3u);
        st(d + 0xC, 0x10005250);
    }
    st(DISP, d);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x02036084, d);
    st(INST, r);
    return r;
}
VERIFY(0x020360FC, JpegMgr_createInstance);

static void JpegMgr_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x02036190, void, p, flags);
    if (p == 0) return;
    gabi::call(0x02760168, p + 0x40, 2u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x02036190, JpegMgr_dt);

/* 020361E4: the worker thread (method 02036860) on its own heap */
static void JpegMgr_createThread(u32 self) {
    WWHD_FUNC(0x020361E4, void, self);
    gabi::Local<be<u32>[2]> nm;
    st(nm.a + 4, 0x10005120);
    st(nm.a + 0, 0x10005234);
    u32 parent = gabi::call<u32>(0x0203E900);
    u32 heap = gabi::call<u32>(0x02753004, 0u, nm.a, parent, 1u, 0u);
    u32 th = gabi::call<u32>(0x0273B050, 0x98u, heap, 4u);
    if (th != 0) {
        st(nm.a + 4, 0x10005120);
        st(nm.a + 0, 0x10005220);
        u32 dg = gabi::call<u32>(0x0273B050, 0x10u, heap, 4u);
        if (dg != 0) {
            st(dg + 4, self);
            sth(dg + 8, 0);
            st(dg + 0xC, 0x02036860);
            sth(dg + 0xA, 0xFFFF);
            st(dg + 0, 0x10005138);
        }
        th = gabi::call<u32>(0x0275FBD8, th, nm.a, dg, heap, ld(0x101F8B9C) + 2, 0u, 0x7FFFFFFFu, 0x5000u, 0x20u);
    }
    st(self + 0x10, th);
    gabi::call_ptr<u32>(vfn(heap, 0xC, 0x2C), heap);
    u32 t = ld(self + 0x10);
    gabi::call_ptr<u32>(vfn(t, 0xC, 0x2C), t);
    st(self + 0x14, 0);
}
VERIFY(0x020361E4, JpegMgr_createThread);

/* 020363E4: queue job 1 (save the picture BUF: three parameters and the album slot) */
static u32 JpegMgr_requestSave(u32 self, u32 buf, u32 a, u32 b, u32 c, u32 slot) {
    WWHD_FUNC(0x020363E4, u32, self, buf, a, b, c, slot);
    if (ld(self + 0x14) != 0) return 0;
    st(self + 0x1C, buf);
    st(self + 0x2C, slot);
    st(self + 0x20, a);
    st(self + 0x14, 1);
    st(self + 0x24, b);
    u32 t = ld(self + 0x10);
    stb(self + 0x18, 0);
    st(self + 0x28, c);
    gabi::call_ptr<u32>(vfn(t, 0xC, 0x1C), t, 1u, 0u);
    return 1;
}
VERIFY(0x020363E4, JpegMgr_requestSave);

/* 02036474: queue job 2 (decode JPEG of SIZE into texture TEX at image DST) */
static u32 JpegMgr_requestDecode(u32 self, u32 jpeg, u32 tex, u32 dst, u32 size) {
    WWHD_FUNC(0x02036474, u32, self, jpeg, tex, dst, size);
    if (ld(self + 0x14) != 0) return 0;
    st(self + 0x3C, size);
    u32 t = ld(self + 0x10);
    st(self + 0x38, dst);
    stb(self + 0x18, 0);
    st(self + 0x14, 2);
    st(self + 0x30, jpeg);
    st(self + 0x34, tex);
    gabi::call_ptr<u32>(vfn(t, 0xC, 0x1C), t, 2u, 0u);
    return 1;
}
VERIFY(0x02036474, JpegMgr_requestDecode);

/* 020364F4: job 1: write the picture into the save's album (and its two descriptor values) */
static void JpegMgr_doSave(u32 self) {
    WWHD_FUNC(0x020364F4, void, self);
    if (ld(self + 0x1C) == 0) return;
    struct frame_l { be<u32> obj[2]; be<u32> prm[3]; }; /* sp+8..0x1B */
    gabi::Local<frame_l> f;
    const u32 obj = f.a, prm = f.a + 8;
    u32 h = gabi::call<u32>(0x0203E90C);
    gabi::call(0x0272720C, obj, h, 0u);
    u32 buf = ld(self + 0x1C);
    st(prm + 4, ld(self + 0x24));
    st(prm + 0, ld(self + 0x20));
    u32 c = ld(self + 0x28), slot = ld(self + 0x2C);
    st(prm + 8, 3);
    gabi::call(0x027272FC, obj, buf, prm, c, slot);
    u32 ok = lbz(obj + 4);
    stb(self + 0x19, ok);
    if (ok != 0) {
        u32 v = gabi::call<u32>(0x025BE8E8);
        gabi::call(0x02727420, obj, ld(self + 0x2C), v);
        v = gabi::call<u32>(0x025BE8F8);
        gabi::call(0x02727470, obj, ld(self + 0x2C), v);
        gabi::call(0x0273AFC8, ld(self + 0x1C));
        st(self + 0x1C, 0);
        stb(self + 0x19, 1);
    }
    gabi::call(0x027272A0, obj, 2u);
}
VERIFY(0x020364F4, JpegMgr_doSave);

/* 020365C8: job 2: decode the JPEG into a linear RGBA surface and copy it into the texture */
static void JpegMgr_doDecode(u32 self) {
    WWHD_FUNC(0x020365C8, void, self);
    if (ld(self + 0x30) == 0) {
        stb(self + 0x19, 0);
        return;
    }
    gabi::Local<be<u32>[0x82]> fr; /* the original frame sp+0..0x207 (offsets below as in it) */
    const u32 F = fr.a;
    const u32 S1 = F + 0x20, S2 = F + 0x94;
    u32 h = gabi::call<u32>(0x0203E90C);
    st(F + 8, 0);
    if (gabi::call<u32>(0x0284DDB0, ld(self + 0x30), 0x50000u, ld(self + 0x3C), F + 0x14, F + 8) != 0) {
        stb(self + 0x19, 0);
        return;
    }
    u32 freeSz = gabi::call_ptr<u32>(vfn(h, 0xC, 0x74), h);
    u32 work_sz = ld(F + 8);
    if (freeSz <= work_sz) {
        stb(self + 0x19, 0);
        return;
    }
    u32 work = gabi::call<u32>(0x0273B0D4, work_sz, h, 4u);
    /* source surface: 2D, RGBA8 (0x1A), linear (tile mode 0x10) */
    st(S1 + 0xC, 0);
    st(S1 + 4, ld(F + 0x14));
    st(S1 + 0x14, 0x1A);
    st(S1 + 0, 1);
    st(S1 + 0x18, 0);
    st(S1 + 0x10, 0);
    st(S1 + 0x1C, 1);
    st(S1 + 0x2C, 0);
    st(S1 + 0x24, 0);
    st(S1 + 8, ld(F + 0x18));
    st(S1 + 0x34, 0);
    st(S1 + 0x30, 0x10);
    gabi::call(GX2CalcSurfaceSizeAndAlignment, S1);
    u32 buf = gabi::call<u32>(0x0273B0D4, (ld(S1 + 0x20) + 3) & ~3u, h, ld(S1 + 0x38));
    u32 rc = gabi::call<u32>(0x0284BE8C, work, ld(F + 8), ld(self + 0x30), 0x50000u, ld(self + 0x3C), buf, ld(S1 + 0x20));
    gabi::call(0x0273AFC8, work);
    st(S1 + 0x24, buf);
    for (u32 i = 0; i < 0x36; i++) st(F + 0x130 + i * 4, ld(0x10005148 + i * 4));
    /* destination texture surface: 2D, RGBA8, default tiling */
    st(S2 + 8, ld(F + 0x18));
    st(S2 + 0x10, 0);
    st(F + 0x10C, 0);
    st(F + 0x118, ld(F + 0x198));
    st(F + 0x114, 0);
    st(S2 + 0xC, 0);
    st(F + 0x108, 0);
    st(S2 + 0, 1);
    st(S2 + 0x18, 0);
    st(S2 + 0x30, 0);
    st(F + 0x110, 0);
    st(S2 + 4, ld(F + 0x14));
    st(S2 + 0x14, 0x1A);
    st(S2 + 0x34, 0);
    st(S2 + 0x1C, 1);
    gabi::call(GX2CalcSurfaceSizeAndAlignment, S2);
    gabi::call(GX2InitTextureRegs, S2);
    st(S2 + 0x24, ld(self + 0x38));
    gabi::call(GX2CopySurface, S1, 0u, 0u, S2, 0u, 0u);
    u32 tex = ld(self + 0x34);
    gabi::call(OSBlockMove_, tex + 0xC, S2, 0x74u, 0u);
    st(tex + 0x80, 0);
    u32 a1c = ld(tex + 0x1C);
    st(tex + 0x90, 0x10203);
    st(tex + 0x84, a1c);
    st(tex + 0x20, 0x41A);
    u32 a18 = ld(tex + 0x18);
    st(tex + 0x88, 0);
    st(tex + 0x8C, a18);
    gabi::call(GX2InitTextureRegs, tex + 0xC);
    gabi::call(0x0287F1EC, F + 0xC, 0x41Au, 1u);
    u32 wh = ((u32)lhz(S2 + 6) << 16) | lhz(S2 + 0xA);
    u32 fmt = ld(F + 0xC);
    st(tex + 0, 1);
    st(F + 0x10, wh);
    sth(tex + 4, lhz(F + 0x10));
    stb(tex + 8, fmt);
    sth(tex + 6, lhz(F + 0x12));
    gabi::call(0x0273AFC8, buf);
    stb(self + 0x19, rc == 0 ? 1 : 0);
}
VERIFY(0x020365C8, JpegMgr_doDecode);

/* 02036860: thread message handler */
static void JpegMgr_threadProc(u32 self, u32 thread, u32 msg) {
    WWHD_FUNC(0x02036860, void, self, thread, msg);
    if (msg != 1 && msg != 2) return;
    u32 cs = self + 0x40;
    gabi::call(0x027601BC, cs);
    gabi::call(msg == 1 ? 0x020364F4 : 0x020365C8, self);
    st(self + 0x14, 0);
    stb(self + 0x18, 1);
    gabi::call(0x027601F0, cs);
}
VERIFY(0x02036860, JpegMgr_threadProc);

/* 0203691C: the singleton disposer's destructor */
static void JpegMgr_disposer_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203691C, void, p, flags);
    if (p == 0) return;
    st(p + 0xC, 0x10005250);
    if (p == ld(DISP)) {
        u32 inst = ld(INST);
        st(DISP, 0);
        gabi::call(0x02036190, inst, 2u);
        st(INST, 0);
    }
    gabi::call(0x02752BEC, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203691C, JpegMgr_disposer_dt);

static void sinit_020369B8() {
    WWHD_FUNC(0x020369B8, void);
    header_sinit(0x102009B4, 0x1018F3D0, 0x10005244);
}
VERIFY(0x020369B8, sinit_020369B8);

static void Dt_02036A4C(u32 p, u32 flags) {
    WWHD_FUNC(0x02036A4C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02036A4C, Dt_02036A4C);

static void Empty_02036A60() {
    WWHD_FUNC(0x02036A60, void);
}
VERIFY(0x02036A60, Empty_02036A60);

}  // namespace hd_olv_02036084
