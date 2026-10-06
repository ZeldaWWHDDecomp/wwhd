/* hd_olv_02036A64: the HD JPEG encoder translation unit (02036A64..02036E57). No GameCube source:
 * HD-only code.
 *
 * A 0x20 object {heap +0, work buffer +4, work size +8, quality +0xC, width +0x10, height +0x14,
 * format +0x18, last size +0x1C} around the system JPEG encoder (0284DEF8 work size, 0284BBB4 encode):
 * own heap and work buffer, size/quality setters, and the encode that halves the quality until the
 * picture fits 0x50000 bytes (the Miiverse screenshot limit); plus the delegate-invoke companion,
 * the TU's __sinit and its companions. */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_olv_02036A64 {

/* 02036A64: sead delegate invoke (GHS member pointer; r6..r8 are scratch, r4/r5 pass through) */
static u32 Delegate_invoke(u32 self, u32 a4, u32 a5) {
    WWHD_FUNC(0x02036A64, u32, self, a4, a5);
    u32 obj = ld(self + 4);
    if (obj == 0) return self;
    s32 idx = (s16)lhz(self + 0xA);
    if (idx == 0) return self;
    u32 t = obj + (u32)(s32)(s16)lhz(self + 8);
    u32 fn;
    if (idx < 0) {
        fn = ld(self + 0xC);
    } else {
        u32 vt = ld(t + (u32)(s32)(s16)lhz(self + 0xE));
        fn = ld(vt + (u32)idx * 8 + 4);
    }
    return gabi::call_ptr<u32>(fn, t, a4, a5);
}
VERIFY(0x02036A64, Delegate_invoke);

/* 02036AB8: constructor (quality 100, 800 x 450) */
static u32 JpegEncoder_ct(u32 self) {
    WWHD_FUNC(0x02036AB8, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x20);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 0x14, 0x1C2);
    st(t + 0xC, 0x64);
    st(t + 0x10, 0x320);
    st(t + 8, 0);
    st(t + 0x1C, 0);
    st(t + 0x18, 0);
    st(t + 4, 0);
    return t;
}
VERIFY(0x02036AB8, JpegEncoder_ct);

static void Dt_02036B1C(u32 p, u32 flags) {
    WWHD_FUNC(0x02036B1C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02036B1C, Dt_02036B1C);

/* 02036B30: own expanded heap of SIZE under PARENT, the whole free space as work buffer */
static void JpegEncoder_init(u32 self, u32 parent, u32 size) {
    WWHD_FUNC(0x02036B30, void, self, parent, size);
    gabi::Local<be<u32>[2]> nm;
    st(nm.a + 4, 0x10005260);
    st(nm.a + 0, 0x10005278);
    u32 h = gabi::call<u32>(0x02753004, size, nm.a, parent, 1u, 0u);
    st(self + 0, h);
    if (h == 0) return;
    u32 sz = gabi::call_ptr<u32>(vfn(h, 0xC, 0x7C), h, 4u);
    u32 hh = ld(self + 0);
    st(self + 8, sz);
    st(self + 4, gabi::call<u32>(0x0273B0D4, sz, hh, 4u));
}
VERIFY(0x02036B30, JpegEncoder_init);

/* 02036BBC: free the work buffer and destroy the heap */
static void JpegEncoder_fini(u32 self) {
    WWHD_FUNC(0x02036BBC, void, self);
    u32 b = ld(self + 4);
    if (b != 0) {
        gabi::call(0x0273AFC8, b);
        st(self + 4, 0);
    }
    u32 h = ld(self + 0);
    st(self + 8, 0);
    if (h == 0) return;
    gabi::call_ptr<u32>(vfn(h, 0xC, 0x24), h);
    st(self + 0, 0);
}
VERIFY(0x02036BBC, JpegEncoder_fini);

static void JpegEncoder_setSize(u32 self, u32 w, u32 h, u32 fmt) {
    WWHD_FUNC(0x02036C28, void, self, w, h, fmt);
    st(self + 0x14, h);
    st(self + 0x18, fmt);
    st(self + 0x10, w);
}
VERIFY(0x02036C28, JpegEncoder_setSize);

static void JpegEncoder_setQuality(u32 self, u32 q) {
    WWHD_FUNC(0x02036C38, void, self, q);
    st(self + 0xC, q);
}
VERIFY(0x02036C38, JpegEncoder_setQuality);

/* 02036C40: encode SRC into DST; halve the quality while the result exceeds 0x50000 bytes */
static u32 JpegEncoder_encode(u32 self, u32 src, u32 pitch, u32 dst) {
    WWHD_FUNC(0x02036C40, u32, self, src, pitch, dst);
    struct frame_l { be<u32> outSize; be<u32> info[3]; be<u32> workSize; }; /* sp+0x10..0x23 */
    gabi::Local<frame_l> f;
    const u32 outSize = f.a, info = f.a + 4, workSize = f.a + 0x10;
    st(workSize, 0);
    st(info + 4, ld(self + 0x14));
    st(info + 0, ld(self + 0x10));
    st(info + 8, ld(self + 0x18));
    if (gabi::call<u32>(0x0284DEF8, info, workSize) != 0) return 0;
    if (ld(self + 8) < ld(workSize)) return 0;
    u32 q = ld(self + 0xC);
    st(outSize, 0);
    if (gabi::call<u32>(0x0284BBB4, ld(self + 4), ld(self + 8), dst, info, q, src, pitch, outSize) != 0) return 0;
    f32 half = ldf(0x10005284);
    for (;;) {
        u32 sz = ld(outSize);
        st(self + 0x1C, sz);
        if (!(sz > 0x50000)) return 1;
        q = (u32)gabi::ftoi((f32)(f64)(s32)q * half);
        if (gabi::call<u32>(0x0284BBB4, ld(self + 4), ld(self + 8), dst, info, q, src, pitch, outSize) != 0) return 0;
    }
}
VERIFY(0x02036C40, JpegEncoder_encode);

static void sinit_02036DAC() {
    WWHD_FUNC(0x02036DAC, void);
    header_sinit(0x102009D0, 0x1018F3FC, 0x10005290);
}
VERIFY(0x02036DAC, sinit_02036DAC);

static void Dt_02036E40(u32 p, u32 flags) {
    WWHD_FUNC(0x02036E40, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02036E40, Dt_02036E40);

static void Empty_02036E54() {
    WWHD_FUNC(0x02036E54, void);
}
VERIFY(0x02036E54, Empty_02036E54);

}  // namespace hd_olv_02036A64
