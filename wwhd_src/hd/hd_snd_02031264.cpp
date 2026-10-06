/* hd_snd_02031264: two HD sound TUs (no GameCube source).
 *
 * 02031264..020317BF: the HD BGM / JAudio controller (0x6C, vtable 100048D0): two output faders +0x14 /
 *                     +0x2C, BGM fader +0x44, JAIZelBasic instance +0xC (0x21F4), JA->NW4F bridge +0x5C,
 *                     initialised flag +0x10, output-on flag +0x60, pause +0x64, delayed-SE timer +0x68:
 *                     ctor/dtor, init, per-frame update (TV/DRC volumes to the NW4F output object
 *                     101F9FF4, delayed SE 0x806, JAIZelBasic frame), fades; __sinit 020316EC + companions
 * 020317C0..02031A97: the SE player wrapper (4 bytes: NW4F player): ctor/dtor, start SE by id / by name
 *                     (two start modes); __sinit 02031A04
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_02031264 {

static void Ctl_fadeOut(u32 self, u32 frames, f32 tv, f32 drc) {
    WWHD_FUNC(0x02031264, void, self, frames, tv, drc);
    gabi::call(0x02762258, self + 0x14, frames, tv);
    gabi::call(0x02762258, self + 0x2C, frames, drc);
}
VERIFY(0x02031264, Ctl_fadeOut);

static u32 Ctl_ct(u32 self) {
    WWHD_FUNC(0x020312CC, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x6C);
        if (t == 0) return 0;
    }
    st(t + 8, 0);
    stb(t + 0x10, 0);
    st(t + 4, 0);
    st(t + 0, 0x100048D0);
    st(t + 0xC, 0);
    gabi::call(0x028EFFD0, t + 0x14, 2, 0x18, 0x02031258);
    f32 one = ldf(0x10004850);
    gabi::call(0x02762170, t + 0x44, one);
    st(t + 0x5C, 0);
    u32 p = t + 0x64;
    stb(t + 0x60, 0);
    if (p == 0) p = op_new(4);
    if (p != 0) st(p, 0);
    st(t + 0x68, 0xFFFFFFFF);
    u32 j = op_new(0x21F4);
    if (j != 0) j = gabi::call<u32>(0x02025B44, j);
    st(t + 0xC, j);
    u32 b = op_new(0x44);
    if (b != 0) b = gabi::call<u32>(0x0202C27C, b);
    st(t + 0x5C, b);
    gabi::call(0x02031264, t, 0, one, ldf(0x100048AC));
    gabi::call(0x02762258, t + 0x44, 0, one);
    st(t + 0x64, 0);
    return t;
}
VERIFY(0x020312CC, Ctl_ct);

static void Ctl_clearInit(u32 self) {
    WWHD_FUNC(0x02031408, void, self);
    stb(self + 0x10, 0);
}
VERIFY(0x02031408, Ctl_clearInit);

static void Ctl_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02031414, void, self, flags);
    if (self == 0) return;
    st(self + 0, 0x100048D0);
    gabi::call(0x02031408, self);
    u32 j = ld(self + 0xC);
    if (j != 0) {
        gabi::call_ptr(vfn(j, 0x2C, 0xC), j, 3);
        st(self + 0xC, 0);
    }
    u32 b = ld(self + 0x5C);
    if (b != 0) {
        gabi::call_ptr(vfn(b, 0, 0xC), b, 3);
        st(self + 0x5C, 0);
    }
    if (flags & 1) op_delete(self);
}
VERIFY(0x02031414, Ctl_dt);

/* 020314C4: init the JAIZelBasic instance (once) and hand it the bridge */
static void Ctl_init(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x020314C4, void, self, a, b);
    (void)a;
    if (lbz(self + 0x10) != 0) return;
    gabi::call(0x02026044, ld(self + 0xC), b, 0x3000000);
    gabi::call(0x0280348C, ld(self + 0xC), ld(self + 0x5C));
    stb(self + 0x10, 1);
}
VERIFY(0x020314C4, Ctl_init);

/* 0203151C: faders, then TV / DRC output volumes to the NW4F output object */
static void Ctl_updateOutput(u32 self) {
    WWHD_FUNC(0x0203151C, void, self);
    gabi::call(0x027621EC, self + 0x14);
    gabi::call(0x027621EC, self + 0x2C);
    gabi::call(0x027621EC, self + 0x44);
    u32 out = ld(0x101F9FF4);
    if (out == 0) return;
    f32 a = ldf(self + 0x14);
    f32 m = ldf(self + 0x44);
    f32 a2 = gabi::fmuls_ppc(a, a);
    f32 b = ldf(self + 0x2C);
    f32 k = ldf(0x100048B0);
    f32 b2 = gabi::fmuls_ppc(b, b);
    f32 tv = gabi::fmuls_ppc(a2, m);
    f32 drc = gabi::fmuls_ppc(gabi::fmuls_ppc(b2, k), m);
    gabi::call(0x0281A2D0, out, tv, drc);
    stb(self + 0x60, m > ldf(0x100048AC) ? 0 : 1); /* mfcr + CR0[GT]: "not positive" */
}
VERIFY(0x0203151C, Ctl_updateOutput);

/* 020315CC: per frame: output, delayed SE 0x806, JAIZelBasic frame */
static void Ctl_update(u32 self) {
    WWHD_FUNC(0x020315CC, void, self);
    if (lbz(self + 0x10) == 0) return;
    gabi::call(0x0203151C, self);
    u32 j = ld(self + 0xC);
    if (j == 0) return;
    s32 c = (s32)ld(self + 0x68);
    if (c > 0) {
        if (ld(self + 0x64) != 0 || lbz(j + 0x30) != 0) {
            gabi::call(0x0202636C, j);
            return;
        }
        --c;
        st(self + 0x68, (u32)c);
        if (c > 0) {
            gabi::call(0x0202636C, ld(self + 0xC));
            return;
        }
        f32 one = ldf(0x10004850), m1 = ldf(0x100048B4);
        gabi::call(0x0201EBA0, ld(self + 0xC), 0x806, 0, 0, 0, 0, one, one, m1, m1);
    }
    st(self + 0x68, 0xFFFFFFFF);
    gabi::call(0x0202636C, ld(self + 0xC));
}
VERIFY(0x020315CC, Ctl_update);

static u32 Ctl_bgmFadeOut(u32 self, u32 frames) {
    WWHD_FUNC(0x02031688, u32, self, frames);
    return gabi::call<u32>(0x02762258, self + 0x44, frames, ldf(0x100048AC));
}
VERIFY(0x02031688, Ctl_bgmFadeOut);

static u32 Ctl_bgmPause(u32 self, u32 frames) {
    WWHD_FUNC(0x02031698, u32, self, frames);
    if (ld(self + 0x64) != 0) return self;
    return gabi::call<u32>(0x02031688, self, frames);
}
VERIFY(0x02031698, Ctl_bgmPause);

static u32 Ctl_bgmFadeIn(u32 self, u32 frames) {
    WWHD_FUNC(0x020316A8, u32, self, frames);
    return gabi::call<u32>(0x02762258, self + 0x44, frames, ldf(0x10004850));
}
VERIFY(0x020316A8, Ctl_bgmFadeIn);

static u32 Ctl_bgmResume(u32 self, u32 frames) {
    WWHD_FUNC(0x020316B8, u32, self, frames);
    if (ld(self + 0x64) != 0) return self;
    return gabi::call<u32>(0x020316A8, self, frames);
}
VERIFY(0x020316B8, Ctl_bgmResume);

static u32 Ctl_bgmUnpause(u32 self, u32 frames) {
    WWHD_FUNC(0x020316C8, u32, self, frames);
    st(self + 0x64, 0);
    return gabi::call<u32>(0x020316A8, self, frames);
}
VERIFY(0x020316C8, Ctl_bgmUnpause);

static void Ctl_armSe(u32 self) {
    WWHD_FUNC(0x020316D4, void, self);
    st(self + 0x68, 0x28);
}
VERIFY(0x020316D4, Ctl_armSe);

static void Ctl_disarmSe(u32 self) {
    WWHD_FUNC(0x020316E0, void, self);
    st(self + 0x68, 0xFFFFFFFF);
}
VERIFY(0x020316E0, Ctl_disarmSe);

static void sinit_020316EC() {
    WWHD_FUNC(0x020316EC, void);
    header_sinit(0x10200478, 0x1018EE2C, 0x100048B8);
    st(0x10200468, 0x10004864);
    st(0x10200464, 0x100048C0);
}
VERIFY(0x020316EC, sinit_020316EC);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x020317A0, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020317A0, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x020317B4, void);
}
VERIFY(0x020317B4, Comp_empty);

static u32 Comp_zero() {
    WWHD_FUNC(0x020317B8, u32);
    return 0;
}
VERIFY(0x020317B8, Comp_zero);

static u32 SePlayer_ct(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x020317C0, u32, self, a, b);
    u32 t = self;
    if (t == 0) {
        t = op_new(4);
        if (t == 0) return 0;
    }
    st(t, 0);
    st(t, gabi::call<u32>(0x02761D48, 0, a, b));
    return t;
}
VERIFY(0x020317C0, SePlayer_ct);

static u32 SePlayer_stopAll(u32 self, u32 x) {
    WWHD_FUNC(0x02031830, u32, self, x);
    return gabi::call<u32>(0x02761E2C, ld(self), x);
}
VERIFY(0x02031830, SePlayer_stopAll);

static void SePlayer_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02031838, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02031830, self, 0);
    u32 p = ld(self);
    if (p != 0) {
        gabi::call_ptr(vfn(p, 0xC, 0xC), p, 3);
        st(self, 0);
    }
    if (flags & 1) op_delete(self);
}
VERIFY(0x02031838, SePlayer_dt);

/* 020318B4: start sound ID (handle mode 1); the handle, or null when the start failed */
static u32 SePlayer_start_020318B4(u32 self, u32 id) {
    WWHD_FUNC(0x020318B4, u32, self, id);
    u32 h = gabi::call<u32>(0x027620A8, ld(self), id, 1);
    if (h == 0) return 0;
    u32 p = gabi::call<u32>(0x0202DAD0);
    if (gabi::call<u32>(0x02897C94, p + 4, h, id, 0) != 0) return 0;
    return h;
}
VERIFY(0x020318B4, SePlayer_start_020318B4);

/* 0203195C: start sound ID (handle mode 0); the handle, or null when the start failed */
static u32 SePlayer_start_0203195C(u32 self, u32 id) {
    WWHD_FUNC(0x0203195C, u32, self, id);
    u32 h = gabi::call<u32>(0x027620A8, ld(self), id, 0);
    if (h == 0) return 0;
    u32 p = gabi::call<u32>(0x0202DAD0);
    if (gabi::call<u32>(0x02897DE4, p + 4, h, id, 0) != 0) return 0;
    return h;
}
VERIFY(0x0203195C, SePlayer_start_0203195C);

static u32 SePlayer_startByName_02031920(u32 self, u32 name) {
    WWHD_FUNC(0x02031920, u32, self, name);
    u32 id = gabi::call<u32>(0x0202DC30, name);
    return gabi::call<u32>(0x020318B4, self, id);
}
VERIFY(0x02031920, SePlayer_startByName_02031920);

static u32 SePlayer_startByName_020319C8(u32 self, u32 name) {
    WWHD_FUNC(0x020319C8, u32, self, name);
    u32 id = gabi::call<u32>(0x0202DC30, name);
    return gabi::call<u32>(0x0203195C, self, id);
}
VERIFY(0x020319C8, SePlayer_startByName_020319C8);

static void sinit_02031A04() {
    WWHD_FUNC(0x02031A04, void);
    header_sinit(0x10200494, 0x1018EE50, 0x10004970);
}
VERIFY(0x02031A04, sinit_02031A04);

}  // namespace hd_snd_02031264
