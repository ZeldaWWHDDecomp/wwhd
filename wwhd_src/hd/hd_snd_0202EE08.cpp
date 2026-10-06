/* hd_snd_0202EE08: three HD sound TUs (no GameCube source).
 *
 * 0202EE08..0202EF23: initialiser-only TU (two float constants 240 / 450 at 102003A8) + companions
 * 0202EF24..0202EFC3: initialiser-only TU + the fader constructor thunk 0202EFB8 (initial value 1.0)
 * 0202EFC4..020302E3: the HD sound player (0x20C, derived from the NW4F sound archive player 0276359C;
 *                     vtables 10004610 / 10004688 / 100046A8): players by name (PLAYER_SYSTEM_SE,
 *                     PLAYER_SEQ_BGM, PLAYER_STRM_BGM, PLAYER_STRM_SUB_BGM, PLAYER_MENU_UI, PLAYER_DRC_ONLY,
 *                     PLAYER_DRC_GAME, PLAYER_DRC_TV, PLAYER_ITEM_SET, PLAYER_CONTROLLER), TV/DRC output
 *                     volumes per frame (two master faders +0x194 / +0x1AC, two BGM faders +0x1C4 / +0x1DC,
 *                     mode flags +0x1F4..+0x1F8, mode +0x1FC, pause +0x200), a distance-based volume curve
 *                     for the controller speaker, pause/resume of all players, stop by sound id, and the
 *                     four HD stream handles (+0x204 demo45 / ending / epilogue, +0x208 staff roll);
 *                     __sinit 02030228 (curve range 170.0 at 102003F0)
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_0202EE08 {

/* 0202EE08: __sinit (header statics + two constants) */
static void sinit_0202EE08() {
    WWHD_FUNC(0x0202EE08, void);
    header_sinit(0x102003BC, 0x1018ED38, 0x1000442C);
    f32 a = ldf(0x10004434), b = ldf(0x10004438);
    stf(0x102003A8, a);
    stf(0x102003AC, b);
}
VERIFY(0x0202EE08, sinit_0202EE08);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EEB8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EEB8, Comp_dt1);

static void SafeString_assureNone() {
    WWHD_FUNC(0x0202EECC, void);
}
VERIFY(0x0202EECC, SafeString_assureNone);

/* sead BufferedSafeString::assureTerminationImpl_ instance */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x0202EED0, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x0202EED0, SafeString_assureTermination);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EEE8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EEE8, Comp_dt2);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EEFC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EEFC, Comp_dt3);

static void Comp_dt4(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EF10, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EF10, Comp_dt4);

static void sinit_0202EF24() {
    WWHD_FUNC(0x0202EF24, void);
    header_sinit(0x102003D8, 0x1018ED5C, 0x10004450);
}
VERIFY(0x0202EF24, sinit_0202EF24);

/* 0202EFB8: fader constructor with the initial value 1.0 */
static u32 Fader_ct1(u32 self) {
    WWHD_FUNC(0x0202EFB8, u32, self);
    return gabi::call<u32>(0x02762170, self, ldf(0x10004458));
}
VERIFY(0x0202EFB8, Fader_ct1);

static void Player_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EFC4, void, self, flags);
    if (self == 0) return;
    st(self + 4, 0x10004688);
    st(self + 0x120, 0x100046A8);
    gabi::call(0x02896CD8, self + 0x208);
    gabi::call(0x02896CD8, self + 0x204);
    gabi::call(0x02763360, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202EFC4, Player_dt);

static u32 Player_thunk(u32 self, u32 a) {
    WWHD_FUNC(0x0202F040, u32, self, a);
    return gabi::call<u32>(0x02763498, self - 4, a);
}
VERIFY(0x0202F040, Player_thunk);

/* 0202F048: constructor */
static u32 Player_ct(u32 self) {
    WWHD_FUNC(0x0202F048, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x20C);
        if (t == 0) return 0;
    }
    gabi::call(0x0276359C, t);
    st(t + 0, 0x10004610);
    st(t + 4, 0x10004688);
    st(t + 0x120, 0x100046A8);
    gabi::call(0x028EFFD0, t + 0x194, 2, 0x18, 0x0202EFB8);
    f32 one = ldf(0x10004458);
    gabi::call(0x02762170, t + 0x1C4, one);
    gabi::call(0x02762170, t + 0x1DC, one);
    stb(t + 0x1F4, 0);
    st(t + 0x1FC, 0);
    stb(t + 0x1F7, 0);
    stb(t + 0x1F5, 0);
    stb(t + 0x1F8, 0);
    stb(t + 0x1F6, 0);
    u32 p = t + 0x200;
    if (p == 0) p = op_new(4);
    if (p != 0) st(p, 0);
    st(t + 0x208, 0);
    st(t + 0x204, 0);
    return t;
}
VERIFY(0x0202F048, Player_ct);

/* 0202F14C: fade both master faders to (TV, DRC) over FRAMES */
static void Player_fadeMaster(u32 self, u32 frames, f32 tv, f32 drc) {
    WWHD_FUNC(0x0202F14C, void, self, frames, tv, drc);
    gabi::call(0x02762258, self + 0x194, frames, tv);
    gabi::call(0x02762258, self + 0x1AC, frames, drc);
}
VERIFY(0x0202F14C, Player_fadeMaster);

static u32 Player_fadeBgm1(u32 self, u32 frames, f32 v) {
    WWHD_FUNC(0x0202F1B4, u32, self, frames, v);
    return gabi::call<u32>(0x02762258, self + 0x1C4, frames, v);
}
VERIFY(0x0202F1B4, Player_fadeBgm1);

static u32 Player_fadeBgm2(u32 self, u32 frames, f32 v) {
    WWHD_FUNC(0x0202F1BC, u32, self, frames, v);
    return gabi::call<u32>(0x02762258, self + 0x1DC, frames, v);
}
VERIFY(0x0202F1BC, Player_fadeBgm2);

/* 0202F1C4: reset: base reset, the shared output-volume object, all faders to 1/0, unpause */
static void Player_reset(u32 self) {
    WWHD_FUNC(0x0202F1C4, void, self);
    gabi::call(0x02763C80, self);
    if (ld(0x101FD8C8) == 0) {
        st(0x101FD8C8, 1);
        gabi::call(0x02888A50, 0x101FCE00);
    }
    f32 one = ldf(0x10004458);
    gabi::call(0x02889F18, 0x101FCE00, 0, one);
    gabi::call(0x0202F14C, self, 0, one, ldf(0x100044C0));
    gabi::call(0x0202F1B4, self, 0, one);
    gabi::call(0x0202F1BC, self, 0, one);
    st(self + 0x200, 0);
    u32 h = gabi::call<u32>(0x0202DB6C);
    if (h != 0 && gabi::call<s32>(0x02762C88, h) == 2) gabi::call(0x02762BC8, h, 0);
}
VERIFY(0x0202F1C4, Player_reset);

static u32 Player_fwd(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0202F2B4, u32, self, a, b);
    return gabi::call<u32>(0x02763758, self, a, b);
}
VERIFY(0x0202F2B4, Player_fwd);

/* 0202F2B8: distance byte of the controller (pad 8, RTTI-checked), 0 without one */
static u32 Player_padDistance() {
    WWHD_FUNC(0x0202F2B8, u32);
    u32 c = ld(0x101F8AE8);
    if (c == 0) return 0;
    u32 p = gabi::call<u32>(0x0273CB8C, c, 8);
    if (p == 0) return 0;
    if (ld(0x101FD694) == 0) {
        st(0x101FD694, 1);
        st(0x101FD968, 0x100044B0);
    }
    if (gabi::call_ptr<u32>(vfn(p, 0x10, 0xC), p, 0x101FD968) == 0) return 0;
    if ((s32)ld(p + 0xAD4) <= 0) return 0;
    return lbz(p + 0xB4);
}
VERIFY(0x0202F2B8, Player_padDistance);

/* 0202F378: controller-speaker volume from the distance: 0.25 up to 30, 0 from 200, quadratic in between */
static f32 Player_padVolume(u32 self) {
    WWHD_FUNC(0x0202F378, f32, self);
    u32 d = gabi::call<u32>(0x0202F2B8);
    f32 c = (f32)(f64)d;
    f32 lo = ldf(0x100044D4);
    f32 v = ldf(0x100044D0);
    if (c <= lo) return v;
    if (!(c < ldf(0x100044D8))) return ldf(0x100044C0);
    f32 t = fdivs_ppc(gabi::fsubs_ppc(c, lo), ldf(0x102003F0));
    f32 u = gabi::fsubs_ppc(ldf(0x10004458), t);
    return gabi::fmuls_ppc(gabi::fmuls_ppc(v, u), u);
}
VERIFY(0x0202F378, Player_padVolume);

static inline void vol(u32 h, u32 ch, f32 v) { gabi::call(0x0289765C, h, ch, v); }

/* 0202F41C: per-frame TV / DRC output volumes of every player */
static void Player_updateVolumes(u32 self) {
    WWHD_FUNC(0x0202F41C, void, self);
    gabi::call(0x027621EC, self + 0x194);
    gabi::call(0x027621EC, self + 0x1AC);
    if (ld(self + 0x1C) == 0) return;
    f32 d = ldf(self + 0x1AC);
    f32 t = ldf(self + 0x194);
    f32 drc = gabi::fmuls_ppc(d, d);
    f32 k = ldf(0x100044DC);
    f32 tv = gabi::fmuls_ppc(gabi::fmuls_ppc(t, t), k);
    static const u32 names[10] = {0x10004558, 0x1000456C, 0x100044E0, 0x10004520, 0x1000457C,
                                  0x100044F0, 0x10004500, 0x10004534, 0x10004510, 0x10004544};
    u32 h[10];
    for (u32 i = 0; i < 10; ++i) h[i] = gabi::call<u32>(0x0289419C, self + 4, names[i]);
    for (u32 i = 0; i < 10; ++i) gabi::call(0x02897654, h[i], 0x21);
    const u32 se = h[0], seq = h[1], strm = h[2], sub = h[3], menu = h[4], drcOnly = h[5],
              drcGame = h[6], drcTv = h[7], item = h[8], ctrl = h[9];
    vol(se, 0, tv); vol(se, 1, drc);
    vol(seq, 0, tv); vol(seq, 1, drc);
    vol(strm, 0, tv); vol(strm, 1, drc);
    vol(sub, 0, tv); vol(sub, 1, drc);
    bool loud = lbz(self + 0x1F7) != 0 || lbz(self + 0x1F8) != 0; /* read before the DRC_GAME calls */
    f32 one = ldf(0x10004458);
    vol(drcGame, 0, tv);
    vol(drcGame, 1, loud ? drc : one);
    f32 zero = ldf(0x100044C0);
    bool drcMode = lbz(self + 0x1F4) != 0 && ld(self + 0x1FC) == 1;
    if (drcMode) {
        vol(menu, 0, k); vol(menu, 1, zero);
    } else if (lbz(self + 0x1F6) != 0 || lbz(self + 0x1F7) != 0 || lbz(self + 0x1F8) != 0) {
        vol(menu, 0, tv); vol(menu, 1, drc);
    } else {
        f32 pv = gabi::call<f32>(0x0202F378, self);
        vol(menu, 0, gabi::fmuls_ppc(pv, tv)); vol(menu, 1, one);
    }
    vol(drcOnly, 0, zero); vol(drcOnly, 1, one);
    vol(drcTv, 0, k); vol(drcTv, 1, one);
    if (lbz(self + 0x1F4) == 0) {
        vol(item, 0, tv); vol(item, 1, one);
    } else if (ld(self + 0x1FC) == 1) {
        vol(item, 0, k); vol(item, 1, zero);
    } else {
        vol(item, 0, zero); vol(item, 1, one);
    }
    if (ld(self + 0x1FC) == 1) {
        vol(ctrl, 0, k); vol(ctrl, 1, zero);
    } else {
        f32 pv = gabi::call<f32>(0x0202F378, self);
        vol(ctrl, 0, gabi::fmuls_ppc(pv, tv)); vol(ctrl, 1, one);
    }
}
VERIFY(0x0202F41C, Player_updateVolumes);

/* 0202F994: BGM fader (+0x1C4) applied to the player 0x1000458C */
static void Player_bgmFade_0202F994(u32 self) {
    WWHD_FUNC(0x0202F994, void, self);
    gabi::call(0x027621EC, self + 0x1C4);
    if (ld(self + 0x1C) == 0) return;
    u32 h = gabi::call<u32>(0x0289419C, self + 4, 0x1000458C);
    gabi::call(0x02897640, h, ldf(self + 0x1C4));
}
VERIFY(0x0202F994, Player_bgmFade_0202F994);

/* 0202F9E8: BGM fader (+0x1DC) applied to the player 0x1000459C */
static void Player_bgmFade_0202F9E8(u32 self) {
    WWHD_FUNC(0x0202F9E8, void, self);
    gabi::call(0x027621EC, self + 0x1DC);
    if (ld(self + 0x1C) == 0) return;
    u32 h = gabi::call<u32>(0x0289419C, self + 4, 0x1000459C);
    gabi::call(0x02897640, h, ldf(self + 0x1DC));
}
VERIFY(0x0202F9E8, Player_bgmFade_0202F9E8);

static void Player_update(u32 self) {
    WWHD_FUNC(0x0202FA3C, void, self);
    gabi::call(0x0202F41C, self);
    gabi::call(0x0202F994, self);
    gabi::call(0x0202F9E8, self);
    gabi::call(0x027637B4, self);
}
VERIFY(0x0202FA3C, Player_update);

struct str2_l { u8 b[8]; };

/* 0202FA80: pause (1, X) every player except PLAYER_DRC_TV */
static void Player_pauseAll(u32 self, u32 x) {
    WWHD_FUNC(0x0202FA80, void, self, x);
    u32 n = ld(self + 0x1C);
    if (n == 0) return;
    gabi::Local<str2_l> s1;
    gabi::Local<str2_l> s2;
    for (u32 i = 0; n != 0; --n, ++i) {
        u32 id = i + 0x04000000;
        st(s1.a + 4, 0x10004478);
        st(s1.a + 0, 0x100045AC); /* "PLAYER_DRC_TV" */
        u32 nm = gabi::call<u32>(0x0202DC64, id);
        st(s2.a + 4, 0x10004478);
        st(s2.a + 0, nm);
        sstr_assure(s1.a);
        sstr_assure(s1.a);
        u32 fn2 = ld(ld(s2.a + 4) + 0x14);
        u32 p = ld(s1.a + 0);
        gabi::call_ptr(fn2, s2.a);
        u32 q = ld(s2.a + 0);
        if (p == q || sstr_eq(p, q)) continue;
        u32 pl = gabi::call<u32>(0x02893CE8, self + 4, id);
        gabi::call(0x028975CC, pl, 1, x);
    }
}
VERIFY(0x0202FA80, Player_pauseAll);

static u32 Player_pause(u32 self, u32 x) {
    WWHD_FUNC(0x0202FB98, u32, self, x);
    if (ld(self + 0x200) != 0) return self;
    return gabi::call<u32>(0x0202FA80, self, x);
}
VERIFY(0x0202FB98, Player_pause);

/* 0202FBA8: resume (0, X) every player */
static void Player_resumeAll(u32 self, u32 x) {
    WWHD_FUNC(0x0202FBA8, void, self, x);
    u32 n = ld(self + 0x1C);
    for (u32 i = 0; n != 0; --n, ++i) {
        u32 pl = gabi::call<u32>(0x02893CE8, self + 4, i + 0x04000000);
        gabi::call(0x028975CC, pl, 0, x);
    }
}
VERIFY(0x0202FBA8, Player_resumeAll);

static u32 Player_resume(u32 self, u32 x) {
    WWHD_FUNC(0x0202FC28, u32, self, x);
    if (ld(self + 0x200) != 0) return self;
    return gabi::call<u32>(0x0202FBA8, self, x);
}
VERIFY(0x0202FC28, Player_resume);

static u32 Player_unpause(u32 self, u32 x) {
    WWHD_FUNC(0x0202FC38, u32, self, x);
    st(self + 0x200, 0);
    return gabi::call<u32>(0x0202FBA8, self, x);
}
VERIFY(0x0202FC38, Player_unpause);

/* 0202FC44: per sound of the stop-by-id scan: mark 1018EDBE when the sound matches the id 1018EDA4
 * (optionally only started sounds, 1018EDBF) and is not one of the two excluded handles */
static void Player_matchSound(u32 h) {
    WWHD_FUNC(0x0202FC44, void, h);
    u32 s = ld(h + 0);
    u32 info = s == 0 ? 0 : ld(s + 0x268);
    u32 key = ld(0x1018EDA4);
    if (ld(info) != key) return;
    if (lbz(0x1018EDBF) != 0) {
        if (ld(h + 0) == 0) return;
        if (gabi::call_ptr<u32>(ld(ld(s + 0x294) + 0x2C), s) == 0) return;
        u32 s3 = ld(h + 0);
        if (s3 == 0) return;
        if (ld(s3 + 0xFC) == 0) return;
    }
    u32 a = ld(0x1018EDA8);
    u32 v = ld(info + 4);
    if (a != 0 && v == a) return;
    u32 b = ld(0x1018EDAC);
    if (b != 0 && v == b) return;
    stb(0x1018EDBE, 1);
}
VERIFY(0x0202FC44, Player_matchSound);

/* scan all sounds of all players with CB on a sound-handle temporary; true when CB set 1018EDBE */
static inline bool scan_players(u32 self, u32 n, u32 cb, bool stopOnHit) {
    gabi::Local<be<u32>> l;
    for (u32 i = 0; n != 0; --n, ++i) {
        u32 p = gabi::call<u32>(0x02893CE8, self + 4, i + 0x04000000);
        u32 head = p + 4;
        for (u32 it = ld(p + 4); it != head; ) {
            u32 cur = it;
            it = ld(it);
            st(l.a, 0);
            gabi::call(0x02896D70, l.a, cur - 0x27C);
            gabi::call(cb, l.a);
            gabi::call(0x02896CD8, l.a);
        }
        if (stopOnHit && lbz(0x1018EDBE) != 0) return true;
    }
    return false;
}

/* 0202FD40: is sound KEY playing (FLAG: only started ones; A/B: handles to ignore) */
static u32 Player_isPlaying(u32 self, u32 key, u32 flag, u32 a, u32 b) {
    WWHD_FUNC(0x0202FD40, u32, self, key, flag, a, b);
    u32 n = ld(self + 0x1C);
    if (n == 0) return 0;
    st(0x1018EDA8, a);
    stb(0x1018EDBE, 0);
    st(0x1018EDAC, b);
    stb(0x1018EDBF, (u8)flag);
    st(0x1018EDA4, key);
    if (scan_players(self, n, 0x0202FC44, true)) {
        st(0x1018EDA4, 0xFFFFFFFF);
        st(0x1018EDA8, 0);
        st(0x1018EDAC, 0);
        stb(0x1018EDBE, 0);
        stb(0x1018EDBF, 0);
        return 1;
    }
    st(0x1018EDA4, 0xFFFFFFFF);
    st(0x1018EDAC, 0);
    stb(0x1018EDBF, 0);
    stb(0x1018EDBE, 0);
    st(0x1018EDA8, 0);
    return 0;
}
VERIFY(0x0202FD40, Player_isPlaying);

/* 0202FE78: per sound of the stop scan: stop it (fade 1018EDB8) unless it is one of the two kept ids */
static u32 Player_stopSound(u32 h) {
    WWHD_FUNC(0x0202FE78, u32, h);
    u32 s = ld(h + 0);
    u32 a = ld(0x1018EDB0);
    u32 info = s == 0 ? 0 : ld(s + 0x268);
    u32 v = ld(info + 4);
    if (a != 0 && v == a) return h;
    u32 b = ld(0x1018EDB4);
    if (b != 0 && v == b) return h;
    u32 s2 = ld(h + 0);
    u32 fade = ld(0x1018EDB8);
    if (s2 == 0) return h;
    return gabi::call<u32>(0x02883FA8, s, fade);
}
VERIFY(0x0202FE78, Player_stopSound);

/* 0202FEF4: stop every sound (FADE frames) except the ids A and B */
static void Player_stopAll(u32 self, u32 fade, u32 a, u32 b) {
    WWHD_FUNC(0x0202FEF4, void, self, fade, a, b);
    u32 n = ld(self + 0x1C);
    if (n == 0) return;
    st(0x1018EDB0, a);
    st(0x1018EDB8, fade);
    st(0x1018EDB4, b);
    scan_players(self, n, 0x0202FE78, false);
    st(0x1018EDB0, 0);
    st(0x1018EDB8, 0);
    st(0x1018EDB4, 0);
}
VERIFY(0x0202FEF4, Player_stopAll);

/* 0202FFB8/0202FFF4/02030044: HD stream 0x100045BC on the handle +0x204 (start, is playing, pause) */
static u32 Strm_start_0202FFB8(u32 self) {
    WWHD_FUNC(0x0202FFB8, u32, self);
    return gabi::call<u32>(0x02897F48, self + 4, self + 0x204, 0x100045BC, 0) == 0 ? 1 : 0;
}
VERIFY(0x0202FFB8, Strm_start_0202FFB8);

static u32 Strm_isPlaying_0202FFF4(u32 self) {
    WWHD_FUNC(0x0202FFF4, u32, self);
    if (ld(self + 0x204) == 0) return 0;
    u32 s = ld(self + 0x204);
    return gabi::call_ptr<u32>(ld(ld(s + 0x294) + 0x2C), s) != 0 ? 1 : 0;
}
VERIFY(0x0202FFF4, Strm_isPlaying_0202FFF4);

static u32 Strm_pause_02030044(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02030044, u32, self, a, b);
    u32 s = ld(self + 0x204);
    if (s == 0) return 0;
    return gabi::call<u32>(0x028850D4, s, a, b);
}
VERIFY(0x02030044, Strm_pause_02030044);

/* 02030054/02030090/020300E0: HD stream 0x100045CC on the handle +0x204 (start, is playing, pause) */
static u32 Strm_start_02030054(u32 self) {
    WWHD_FUNC(0x02030054, u32, self);
    return gabi::call<u32>(0x02897F48, self + 4, self + 0x204, 0x100045CC, 0) == 0 ? 1 : 0;
}
VERIFY(0x02030054, Strm_start_02030054);

static u32 Strm_isPlaying_02030090(u32 self) {
    WWHD_FUNC(0x02030090, u32, self);
    if (ld(self + 0x204) == 0) return 0;
    u32 s = ld(self + 0x204);
    return gabi::call_ptr<u32>(ld(ld(s + 0x294) + 0x2C), s) != 0 ? 1 : 0;
}
VERIFY(0x02030090, Strm_isPlaying_02030090);

static u32 Strm_pause_020300E0(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x020300E0, u32, self, a, b);
    u32 s = ld(self + 0x204);
    if (s == 0) return 0;
    return gabi::call<u32>(0x028850D4, s, a, b);
}
VERIFY(0x020300E0, Strm_pause_020300E0);

/* 020300F0/0203012C/0203017C: HD stream 0x100045DC on the handle +0x204 (start, is playing, pause) */
static u32 Strm_start_020300F0(u32 self) {
    WWHD_FUNC(0x020300F0, u32, self);
    return gabi::call<u32>(0x02897F48, self + 4, self + 0x204, 0x100045DC, 0) == 0 ? 1 : 0;
}
VERIFY(0x020300F0, Strm_start_020300F0);

static u32 Strm_isPlaying_0203012C(u32 self) {
    WWHD_FUNC(0x0203012C, u32, self);
    if (ld(self + 0x204) == 0) return 0;
    u32 s = ld(self + 0x204);
    return gabi::call_ptr<u32>(ld(ld(s + 0x294) + 0x2C), s) != 0 ? 1 : 0;
}
VERIFY(0x0203012C, Strm_isPlaying_0203012C);

static u32 Strm_pause_0203017C(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x0203017C, u32, self, a, b);
    u32 s = ld(self + 0x204);
    if (s == 0) return 0;
    return gabi::call<u32>(0x028850D4, s, a, b);
}
VERIFY(0x0203017C, Strm_pause_0203017C);

/* 0203018C/020301C8/02030218: HD stream 0x100045F0 on the handle +0x208 (start, is playing, pause) */
static u32 Strm_start_0203018C(u32 self) {
    WWHD_FUNC(0x0203018C, u32, self);
    return gabi::call<u32>(0x02897F48, self + 4, self + 0x208, 0x100045F0, 0) == 0 ? 1 : 0;
}
VERIFY(0x0203018C, Strm_start_0203018C);

static u32 Strm_isPlaying_020301C8(u32 self) {
    WWHD_FUNC(0x020301C8, u32, self);
    if (ld(self + 0x208) == 0) return 0;
    u32 s = ld(self + 0x208);
    return gabi::call_ptr<u32>(ld(ld(s + 0x294) + 0x2C), s) != 0 ? 1 : 0;
}
VERIFY(0x020301C8, Strm_isPlaying_020301C8);

static u32 Strm_pause_02030218(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02030218, u32, self, a, b);
    u32 s = ld(self + 0x208);
    if (s == 0) return 0;
    return gabi::call<u32>(0x028850D4, s, a, b);
}
VERIFY(0x02030218, Strm_pause_02030218);

/* 02030228: __sinit (header statics + the speaker curve range 170) */
static void sinit_02030228() {
    WWHD_FUNC(0x02030228, void);
    /* the header statics of this TU keep their -pi/pi pair at obj-0x10 (the curve range sits at obj-8) */
    u32 obj = 0x102003F8;
    st(obj + 8, 0); st(obj + 0, 0); st(obj + 0xC, 0); st(obj + 4, 0);
    gabi::call(0x028F026C, 0x1018ED80);
    f32 a = ldf(0x10004604), b = ldf(0x10004608);
    stf(obj - 0x10, a);
    stf(obj - 0xC, b);
    gabi::call(0x028ED6F8, obj - 4);
    gabi::call(0x028F026C, 0x1018ED8C);
    gabi::call(0x028EAB2C, obj - 3);
    gabi::call(0x028F026C, 0x1018ED98);
    stf(0x102003F0, ldf(0x1000460C));
}
VERIFY(0x02030228, sinit_02030228);

static void Comp_dt5(u32 self, s32 flags) {
    WWHD_FUNC(0x020302CC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020302CC, Comp_dt5);

static void Comp_empty() {
    WWHD_FUNC(0x020302E0, void);
}
VERIFY(0x020302E0, Comp_empty);

}  // namespace hd_snd_0202EE08
