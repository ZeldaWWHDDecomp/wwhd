/* JAIZelBasic (WWHD), part 3: initLevObjSE .. bgmStop (02021290..0202204B).
 * Ported from the GameCube decompilation where it has
 * bodies, else written from the WWHD code; verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_3_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline f32 calcPosVolume_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C360, b, pos, s); }
static inline f32 calcPosPanLR_l(u32 b, u32 pos) { return gabi::call<f32>(0x0201C0E8, b, pos); }
static inline f32 calcPosPanSR_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C228, b, pos, s); }
static inline void JAISound_setPan_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280BA28, s, t, k, v); }
static inline void JAISound_setDolby_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280C248, s, t, k, v); }
static inline u32 getParamSeqPlayTrackMax_l() { return gabi::call<u32>(0x0280470C); }
static inline u32 SequenceMgr_getPlayTrackInfo_l(u32 i) { return gabi::call<u32>(0x0280AD2C, i); }
/* HD stream player (see checkStreamPlaying): 0202F1BC / 0202F1B4 set a volume pair, 0202FEF4 stops */
static inline void hdsnd_F1BC_l(u32 p, u32 fade, f32 v) { gabi::call(0x0202F1BC, p, fade, v); }
static inline void hdsnd_F1B4_l(u32 p, u32 fade, f32 v) { gabi::call(0x0202F1B4, p, fade, v); }
static inline void hdsnd_stop_l(u32 p, u32 fade, u32 main, u32 sub) { gabi::call(0x0202FEF4, p, fade, main, sub); }

static inline f32 fsel(f32 a, f32 c, f32 b) { return a >= 0.0f ? c : b; }

static inline f32 mainVol(u32 self) {
    f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0x9C));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, ldf(self + 0xA4));
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, ldf(self + 0xAC));
    return mul(v, ldf(self + 0xBC));
}
static inline f32 mainVol_no9C(u32 self) {
    f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, ldf(self + 0xA4));
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, ldf(self + 0xAC));
    return mul(v, ldf(self + 0xBC));
}

/* 02021290 JAIZelBasic::initLevObjSE(): 15 sounds x 20 instances {vol 0, pan 0.5, surround 0} */
void initLevObjSE(u32 self) {
    WWHD_FUNC(0x02021290, void, self);
    st(self + 0x15EC, 0);
    for (u32 k = 0; k < 15; k++) {
        u32 e = self + 0x2B4 + k * 0x148;
        st(e, (u32)-1);
        st(e + 4, 0);
        for (u32 j = 0; j < 20; j++) {
            stf(e + 8 + j * 16, 0.0f);
            stf(e + 0xC + j * 16, 0.5f);
            stf(e + 0x10 + j * 16, 0.0f);
        }
    }
}
VERIFY(0x02021290, initLevObjSE);

/* 02021310 JAIZelBasic::processLevObjSE(): one seStart per level-object sound, mixed from its instances */
void processLevObjSE(u32 self) {
    WWHD_FUNC(0x020212EC, void, self);
    u32 n = ld(self + 0x15EC);
    for (u32 i = 0; i < n; i++) {
        u32 e = self + 0x2B4 + i * 0x148;
        f32 l = 0.0f, r = 0.0f, s = 0.0f;
        s32 reverb = 0;
        u32 c = ld(e + 4);
        for (u32 j = 0; j < c; j++) {
            u32 p = e + 8 + j * 16;
            f32 vol = ldf(p);
            f32 lr = ldf(p + 4);
            if (vol > 0.0f) reverb = (s8)ld8(p + 0xC);
            f32 sr = ldf(p + 8);
            f32 ss = mul(sr, vol);
            f32 ll = mul(gabi::fsubs_ppc(1.0f, lr), vol);
            f32 rr = mul(lr, vol);
            if (ll > l) l = ll;
            if (rr > r) r = rr;
            if (ss > s) s = ss;
        }
        f32 vol = fsel(gabi::fsubs_ppc(r, l), r, l);
        f32 pan = 0.5f;
        vol = fsel(gabi::fsubs_ppc(s, vol), s, vol);
        u32 b277 = ld8(self + 0x277);
        if (l != 0.0f || r != 0.0f) pan = r / gabi::fadds_ppc(l, r);
        if (b277 == 0) {
            JAIZelBasic_seStart_l(self, ld(e), 0, 0, reverb, 1.0f, vol, pan, s, 1);
            n = ld(self + 0x15EC);
        }
    }
    gabi::call(0x02021290, self); /* initLevObjSE */
}
VERIFY(0x020212EC, processLevObjSE);

/* 0202148C JAIZelBasic::checkSubBgmPlaying() */
s32 checkSubBgmPlaying(u32 self) {
    WWHD_FUNC(0x0202148C, s32, self);
    return ld(self + 0x7C) != 0;
}
VERIFY(0x0202148C, checkSubBgmPlaying);

/* 0202149C JAIZelBasic::checkPlayingSubBgmFlag() (unnamed by the matcher) */
u32 checkPlayingSubBgmFlag(u32 self) {
    WWHD_FUNC(0x0202149C, u32, self);
    u32 s = ld(self + 0x7C);
    if (s == 0) return (u32)-1;
    return JAISound_getID_l(s);
}
VERIFY(0x0202149C, checkPlayingSubBgmFlag);

/* 020214B4 JAIZelBasic::checkCbPracticePlay() */
s32 checkCbPracticePlay(u32 self) {
    WWHD_FUNC(0x020214B4, s32, self);
    if (checkSubBgmPlaying(self) == 1 &&
        (checkPlayingSubBgmFlag(self) == 0x80000036 || checkPlayingSubBgmFlag(self) == 0x80000037))
        return 1;
    return 0;
}
VERIFY(0x020214B4, checkCbPracticePlay);

/* 02021534 JAIZelBasic::cbPracticeProcess(): the practice tune follows its position (+0xE0) and
 * ducks the main bgm through +0xAC (HD: less in scene 0x27) */
void cbPracticeProcess(u32 self) {
    WWHD_FUNC(0x02021534, void, self);
    if (ld(self + 0xE0) == 0) return;
    f32 duck = 1.0f;
    u32 main;
    if (checkCbPracticePlay(self) == 0) {
        if (ldf(self + 0xAC) == 1.0f) return;
        main = ld(self + 0x78);
        stf(self + 0xAC, 1.0f);
        if (main == 0) return;
    } else {
        u32 cam = ld(self);
        u32 mtx = ld(cam + 8);
        u32 src = ld(self + 0xE0);
        gabi::Local<Vec_l> vv;
        u32 v = gabi::ea(vv.get());
        st(v, ld(0x10003950));
        st(v + 4, ld(0x10003954));
        st(v + 8, ld(0x10003958));
        if (src == 0) src = ld(cam);
        if (src != 0) {
            st(v, ld(src));
            st(v + 4, ld(src + 4));
            st(v + 8, ld(src + 8));
        }
        if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
        f32 vol = calcPosVolume_l(self, v, 2.0f);
        f32 lr = calcPosPanLR_l(self, v);
        f32 sr = calcPosPanSR_l(self, v, 1.0f);
        u32 sub = ld(self + 0x7C);
        stf(self + 0xB8, vol);
        if (sub != 0) {
            JAISound_setVolume_l(sub, mul(mul(ldf(self + 0xB0), ldf(self + 0xB4)), vol), 0, 0);
            JAISound_setPan_l(ld(self + 0x7C), lr, 0, 0);
            JAISound_setDolby_l(ld(self + 0x7C), sr, 0, 0);
        }
        if ((s32)ld(self + 0x294) == 0x27) duck = gabi::fnmsubs(vol, 0.4f, 1.0f);
        else duck = gabi::fsubs_ppc(1.0f, vol);
        main = ld(self + 0x78);
        stf(self + 0xAC, duck);
        if (main == 0) return;
    }
    f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0x9C));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, ldf(self + 0xA4));
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, duck);
    v = mul(v, ldf(self + 0xBC));
    JAISound_setVolume_l(main, v, 0, 0);
}
VERIFY(0x02021534, cbPracticeProcess);

/* 020217A4 JAIZelBasic::checkSeqIDDemoPlaying(u32 id): a sequence other than the main/sub bgm, or an
 * HD stream, playing id */
s32 checkSeqIDDemoPlaying(u32 self, u32 id) {
    WWHD_FUNC(0x020217A4, s32, self, id);
    if (id == 0) return 0;
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s == 0) continue;
        if (s == ld(self + 0x78)) continue;
        if (s == ld(self + 0x7C)) continue;
        if (JAISound_getID_l(s) == 0x80000800) continue;
        if (id == JAISound_getID_l(s)) return 1;
    }
    u32 main = ld(self + 0x78);
    u32 mgr = ld(0x1018EC64);
    u32 sub = ld(self + 0x7C);
    u32 p = hdsnd_player_l(mgr);
    if (hdsnd_check_l(p, id, 0, main, sub) != 0) return 1;
    return 0;
}
VERIFY(0x020217A4, checkSeqIDDemoPlaying);

/* 0202189C JAIZelBasic::checkDemoFanfarePlaying(): the fanfare playing, else 1 in stage "Demo34"
 * while the global demo state is 1, else 0 */
u32 checkDemoFanfarePlaying(u32 self) {
    WWHD_FUNC(0x0202189C, u32, self);
    static const u32 ids[] = {0x80000009, 0x80000002, 0x80000025, 0x80000027, 0x80000024, 0x8000004F, 0x8000005D, 0x80000061};
    for (u32 k = 0; k < 8; k++)
        if (checkSeqIDDemoPlaying(self, ids[k]) == 1) return ids[k];
    /* sead::SafeString == : the stage name (1047E6B8) against "Demo34" (1000395C) */
    struct SafeString_l { be<u32> mStringTop; be<u32> __vtbl; };
    gabi::Local<SafeString_l> sa;
    gabi::Local<SafeString_l> sb;
    u32 a = gabi::ea(sa.get()), b = gabi::ea(sb.get());
    st(a + 4, 0x1000382C);
    st(b + 4, 0x1000382C);
    st(a, 0x1000395C);
    st(b, 0x1047E6B8);
    gabi::call(0x02029C9C, a);
    gabi::call_ptr(ld(ld(a + 4) + 0x14), a);
    u32 vb = ld(ld(b + 4) + 0x14);
    u32 pa = ld(a);
    gabi::call_ptr(vb, b);
    bool eq;
    if (pa == ld(b)) {
        eq = true;
    } else {
        eq = false;
        u32 p = ld(a), q = ld(b);
        u32 i = 0;
        for (; i < 0x40001; i++) {
            u8 c = ld8(p + i);
            if (c != ld8(q + i)) break;
            if (c == 0) {
                eq = true;
                break;
            }
        }
    }
    if (eq && ld(0x101D6010) == 1) return 1;
    return 0;
}
VERIFY(0x0202189C, checkDemoFanfarePlaying);

/* 02021B10 (HD-only, unnamed): passes a bgm volume to the HD stream player (two settings) */
void hdStreamVolume(u32 self, u32 fade, f32 v) {
    WWHD_FUNC(0x02021B10, void, self, fade, v);
    hdsnd_F1BC_l(hdsnd_player_l(ld(0x1018EC64)), fade, v);
    hdsnd_F1B4_l(hdsnd_player_l(ld(0x1018EC64)), fade, v);
}
VERIFY(0x02021B10, hdStreamVolume);

/* 02021B88 JAIZelBasic::muteMainBgmAll() */
void muteMainBgmAll(u32 self) {
    WWHD_FUNC(0x02021B88, void, self);
    stf(self + 0x9C, 0.0f);
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s == 0) continue;
        if (ld8(s) != 0) continue;
        JAISound_setVolume_l(s, mainVol(self), 1, 0);
    }
    hdStreamVolume(self, 1, mul(mul(ldf(self + 0x94), ldf(self + 0x9C)), ldf(self + 0xA8)));
}
VERIFY(0x02021B88, muteMainBgmAll);

/* 02021C7C JAIZelBasic::unmuteMainBgmAll() */
void unmuteMainBgmAll(u32 self) {
    WWHD_FUNC(0x02021C7C, void, self);
    u32 main = ld(self + 0x78);
    stf(self + 0x9C, 1.0f);
    if (main != 0) JAISound_setVolume_l(main, mainVol_no9C(self), 2, 0);
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s == 0) continue;
        if (ld8(s) != 0) continue;
        JAISound_setVolume_l(s, mainVol(self), 0x2D, 0);
    }
    hdStreamVolume(self, 0x2D, mul(mul(ldf(self + 0x94), ldf(self + 0x9C)), ldf(self + 0xA8)));
}
VERIFY(0x02021C7C, unmuteMainBgmAll);

/* 02021DC4 JAIZelBasic::processDemoFanfareMute() */
void processDemoFanfareMute(u32 self) {
    WWHD_FUNC(0x02021DC4, void, self);
    u32 r = checkDemoFanfarePlaying(self);
    u32 c9 = ld8(self + 0xC9);
    if (r != 0) {
        if (c9 == 0) muteMainBgmAll(self);
        st8(self + 0xC9, 1);
    } else {
        if (c9 != 0) unmuteMainBgmAll(self);
        st8(self + 0xC9, 0);
    }
}
VERIFY(0x02021DC4, processDemoFanfareMute);

/* 02021E30 JAIZelBasic::checkBgmPlaying() */
s32 checkBgmPlaying(u32 self) {
    WWHD_FUNC(0x02021E30, s32, self);
    if (ld(self + 0x78) != 0 || ld(self + 0x80) != 0) return 1;
    return 0;
}
VERIFY(0x02021E30, checkBgmPlaying);

/* 02021E58 JAIZelBasic::demoBgmStop(u32 fade) */
void demoBgmStop(u32 self, u32 fade) {
    WWHD_FUNC(0x02021E58, void, self, fade);
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s == 0) continue;
        if (s == ld(self + 0x78)) continue;
        if (s == ld(self + 0x7C)) continue;
        if (ld8(s) != 0) continue;
        JAISound_stop_l(s, fade);
    }
    u32 main = ld(self + 0x78);
    u32 mgr = ld(0x1018EC64);
    u32 sub = ld(self + 0x7C);
    hdsnd_stop_l(hdsnd_player_l(mgr), fade, main, sub);
}
VERIFY(0x02021E58, demoBgmStop);

/* 02021F28 JAIZelBasic::bgmStop(u32 fade, s32 keepSub) */
void bgmStop(u32 self, u32 fade, s32 keepSub) {
    WWHD_FUNC(0x02021F28, void, self, fade, keepSub);
    u32 n = ld(self + 0x88);
    u32 main = ld(self + 0x78);
    if (n != 0x80000003 && n != 0x80000010) st8(self + 0xC5, 0);
    if (main != 0) JAISound_stop_l(main, fade);
    u32 strm = ld(self + 0x80);
    st(self + 0x78, 0);
    st(self + 0x88, (u32)-1);
    if (strm != 0) JAISound_stop_l(strm, fade);
    st(self + 0x80, 0);
    st(self + 0x8C, (u32)-1);
    if (keepSub == 0) {
        u32 sub = ld(self + 0x7C);
        if (sub != 0) {
            JAISound_stop_l(sub, fade);
            st(self + 0x7C, 0);
        }
        st8(self + 0xCB, 0);
        st(self + 0x84, (u32)-1);
        stf(self + 0x9C, 1.0f);
        st8(self + 0x276, 0);
    }
    demoBgmStop(self, fade);
    st8(self + 0x1FAC, 0);
    st8(self + 0x42, 0);
    st8(self + 0xDB, 0xFF);
    st8(self + 0x74, 0);
    st8(self + 0xDC, 0);
    st8(self + 0x271, 0);
    st8(self + 0xC8, 0);
    st8(self + 0xC6, 0);
    st8(self + 0x75, 0);
}
VERIFY(0x02021F28, bgmStop);

} // namespace JAIZelBasic_3_cpp
