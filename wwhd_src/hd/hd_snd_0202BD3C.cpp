/* hd_snd_0202BD3C: two HD sound TUs (no GameCube source).
 *
 * 0202BD3C..0202C0CF: the HD sound manager singleton (0x18: sound system container +0x10, the NW4F
 *                     sound-archive player 0x24 (02761BD8) +0x14; instance 1018EC64, disposer 1018EC68):
 *                     wiring of the five sub-systems into the player, constructor, createInstance,
 *                     destructor, forwarding accessors, disposer destructor; __sinit 0202C03C
 * 0202C0D0..0202C27B: a sound-handle holder (0x14, vtable 100040D0; NW4F handle +4, id +0xC = -1)
 *                     with null-safe forwarders to the handle; __sinit 0202C1E0 (+ an accessor companion)
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_0202BD3C {

/* 0202BD3C: hand the container's sub-systems to the sound-archive player */
static void SndMgr_connect(u32 self) {
    WWHD_FUNC(0x0202BD3C, void, self);
    {
        u32 x = gabi::call<u32>(0x0202BC78, ld(self + 0x10));
        gabi::call(0x02761C74, ld(self + 0x14), x);
    }
    {
        u32 x = gabi::call<u32>(0x0202BC80, ld(self + 0x10));
        gabi::call(0x02761C7C, ld(self + 0x14), x);
    }
    {
        u32 x = gabi::call<u32>(0x0202BC88, ld(self + 0x10));
        gabi::call(0x02761C84, ld(self + 0x14), x);
    }
    {
        u32 x = gabi::call<u32>(0x0202BC98, ld(self + 0x10));
        gabi::call(0x02761C8C, ld(self + 0x14), x);
    }
    {
        u32 x = gabi::call<u32>(0x0202BCA0, ld(self + 0x10));
        gabi::call(0x02761C8C, ld(self + 0x14), x);
    }
}
VERIFY(0x0202BD3C, SndMgr_connect);

/* 0202BDC8: constructor: container (default heap), player, wiring, sound tables */
static u32 SndMgr_ct(u32 self) {
    WWHD_FUNC(0x0202BDC8, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x18);
        if (t == 0) return 0;
    }
    st(t + 0x10, 0);
    st(t + 0x14, 0);
    u32 p = op_new(0x18);
    if (p != 0) p = gabi::call<u32>(0x0202BA00, p, 0);
    st(t + 0x10, p);
    u32 q = op_new(0x24);
    if (q != 0) q = gabi::call<u32>(0x02761BD8, q);
    st(t + 0x14, q);
    u32 r = gabi::call<u32>(0x0202BD3C, t);
    gabi::call(0x0202DD2C, r);
    return t;
}
VERIFY(0x0202BDC8, SndMgr_ct);

/* 0202BE64: createInstance(heap) (sead singleton): returns the instance (the existing one, or the
 * newly constructed one / 0); the caller 0203E9EC stores it */
static u32 SndMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x0202BE64, u32, heap);
    if (u32 existing = ld(0x1018EC64)) return existing;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x18, heap, 4);
    if (d != 0) {
        gabi::call(0x02752B0C, d, heap, 3);
        st(d + 0xC, 0x100040B8);
    }
    st(0x1018EC68, d);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x0202BDC8, d);
    st(0x1018EC64, r);
    return r;
}
VERIFY(0x0202BE64, SndMgr_createInstance);

static void SndMgr_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202BEF8, void, self, flags);
    if (self == 0) return;
    u32 x = ld(self + 0x10);
    if (x != 0) {
        gabi::call(0x0202BB58, x, 3);
        st(self + 0x10, 0);
    }
    u32 y = ld(self + 0x14);
    if (y != 0) {
        op_delete(y);
        st(self + 0x14, 0);
    }
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202BEF8, SndMgr_dt);

static u32 SndMgr_fwd_0202BF78(u32 self) {
    WWHD_FUNC(0x0202BF78, u32, self);
    return gabi::call<u32>(0x0202BC80, ld(self + 0x10));
}
VERIFY(0x0202BF78, SndMgr_fwd_0202BF78);

static u32 SndMgr_fwd_0202BF80(u32 self) {
    WWHD_FUNC(0x0202BF80, u32, self);
    return gabi::call<u32>(0x0202BC88, ld(self + 0x10));
}
VERIFY(0x0202BF80, SndMgr_fwd_0202BF80);

static u32 SndMgr_fwd_0202BF88(u32 self) {
    WWHD_FUNC(0x0202BF88, u32, self);
    return gabi::call<u32>(0x0202BC90, ld(self + 0x10));
}
VERIFY(0x0202BF88, SndMgr_fwd_0202BF88);

static u32 SndMgr_fwd_0202BF90(u32 self) {
    WWHD_FUNC(0x0202BF90, u32, self);
    return gabi::call<u32>(0x0202BCA0, ld(self + 0x10));
}
VERIFY(0x0202BF90, SndMgr_fwd_0202BF90);

static u32 SndMgr_fwd_0202BF98(u32 self) {
    WWHD_FUNC(0x0202BF98, u32, self);
    return gabi::call<u32>(0x0202BC98, ld(self + 0x10));
}
VERIFY(0x0202BF98, SndMgr_fwd_0202BF98);

/* 0202BFA0: singleton disposer destructor */
static void SndMgrDisposer_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202BFA0, void, self, flags);
    if (self == 0) return;
    st(self + 0xC, 0x100040B8);
    if (self == ld(0x1018EC68)) {
        u32 inst = ld(0x1018EC64);
        st(0x1018EC68, 0);
        gabi::call(0x0202BEF8, inst, 2);
        st(0x1018EC64, 0);
    }
    gabi::call(0x02752BEC, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202BFA0, SndMgrDisposer_dt);

static void sinit_0202C03C() {
    WWHD_FUNC(0x0202C03C, void);
    header_sinit(0x101FFD8C, 0x1018EC40, 0x100040B0);
}
VERIFY(0x0202C03C, sinit_0202C03C);

static u32 Handle_ct(u32 self) {
    WWHD_FUNC(0x0202C0D0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x14);
        if (t == 0) return 0;
    }
    st(t + 8, 0);
    st(t + 4, 0);
    st(t + 0x10, 0);
    st(t + 0, 0x100040D0);
    st(t + 0xC, 0xFFFFFFFF);
    return t;
}
VERIFY(0x0202C0D0, Handle_ct);

/* 0202C128: stop on the NW4F handle when there is one */
static u32 Handle_0202C128(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0202C128, u32, self, a, b);
    u32 h = ld(self + 4);
    if (h == 0) return 0;
    return gabi::call<u32>(0x02883FA8, h, a, b);
}
VERIFY(0x0202C128, Handle_0202C128);

/* 0202C1A0: pause on the NW4F handle when there is one */
static u32 Handle_0202C1A0(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0202C1A0, u32, self, a, b);
    u32 h = ld(self + 4);
    if (h == 0) return 0;
    return gabi::call<u32>(0x028850D4, h, a, b);
}
VERIFY(0x0202C1A0, Handle_0202C1A0);

/* 0202C1C0: fwd 02885914 on the NW4F handle when there is one */
static u32 Handle_0202C1C0(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0202C1C0, u32, self, a, b);
    u32 h = ld(self + 4);
    if (h == 0) return 0;
    return gabi::call<u32>(0x02885914, h, a, b);
}
VERIFY(0x0202C1C0, Handle_0202C1C0);

/* 0202C1D0: fwd 028859A0 on the NW4F handle when there is one */
static u32 Handle_0202C1D0(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0202C1D0, u32, self, a, b);
    u32 h = ld(self + 4);
    if (h == 0) return 0;
    return gabi::call<u32>(0x028859A0, h, a, b);
}
VERIFY(0x0202C1D0, Handle_0202C1D0);

static void Handle_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202C138, void, self, flags);
    if (self == 0) return;
    st(self + 0, 0x100040D0);
    gabi::call(0x0202C128, self, 0);
    gabi::call(0x02896CD8, self + 4);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202C138, Handle_dt);

static u32 Handle_isValid(u32 self) {
    WWHD_FUNC(0x0202C1B0, u32, self);
    return ld(self + 4) != 0 ? 1 : 0;
}
VERIFY(0x0202C1B0, Handle_isValid);

static void sinit_0202C1E0() {
    WWHD_FUNC(0x0202C1E0, void);
    header_sinit(0x101FFDA8, 0x1018EC6C, 0x100040C8);
}
VERIFY(0x0202C1E0, sinit_0202C1E0);

static u32 Comp_getC(u32 self) {
    WWHD_FUNC(0x0202C274, u32, self);
    return ld(self + 0xC);
}
VERIFY(0x0202C274, Comp_getC);

}  // namespace hd_snd_0202BD3C
