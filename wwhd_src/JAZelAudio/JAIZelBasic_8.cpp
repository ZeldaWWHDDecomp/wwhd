/* JAIZelBasic (WWHD), part 8: talkIn .. the unit's trailing copies (02027D4C..02029C9F).
 * Ported from the GameCube decompilation where it has
 * bodies, else written from the WWHD code; verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_8_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline f32 calcPosVolume_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C360, b, pos, s); }
static inline f32 calcPosPanLR_l(u32 b, u32 pos) { return gabi::call<f32>(0x0201C0E8, b, pos); }
static inline f32 calcPosPanSR_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C228, b, pos, s); }
static inline void JAISound_setPan_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280BA28, s, t, k, v); }
static inline void JAISound_setDolby_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280C248, s, t, k, v); }
static inline s32 isDemo_l(u32 b) { return gabi::call<s32>(0x0201E5CC, b); }
static inline void bgmStart_l(u32 b, u32 id, u32 fade, u32 keep) { gabi::call(0x0202204C, b, id, fade, keep); }
static inline void bgmStop_l(u32 b, u32 fade, u32 keep) { gabi::call(0x02021F28, b, fade, keep); }
static inline void bgmMute_l(u32 b, u32 h, u32 bgm, u32 set, u32 fade) { gabi::call(0x0201DD18, b, h, bgm, set, fade); }
static inline void hdStreamVolume_l(u32 b, u32 fade, f32 v) { gabi::call(0x02021B10, b, fade, v); }
static inline void enemyNearBy_l(u32 b) { gabi::call(0x020259E8, b); }
static inline void mbossBgmNearByProcess_l(u32 b, f32 d) { gabi::call(0x02025940, b, d); }
static inline s32 checkLinkOnSea_l(u32 b) { return gabi::call<s32>(0x02025AC0, b); }
static inline s32 checkLinkOnBoardSea_l(u32 b) { return gabi::call<s32>(0x02025AF4, b); }
static inline void kuroboVoicePlay_l(u32 b, u32 id, u32 pos, u32 reverb) { gabi::call(0x0202587C, b, id, pos, reverb); }
static inline f32 TRandom_get_l(u32 r) { return gabi::call<f32>(0x027ED7E0, r); }

/* 02027D4C JAIZelBasic::talkIn() */
void talkIn(u32 self) {
    WWHD_FUNC(0x02027D4C, void, self);
    if (ld8(self + 0x74) == 1) return;
    if (isDemo_l(self) == 1) return;
    if (ldf(self + 0xA8) == 0.0f) return;
    u32 m = ld(self + 0x78);
    f32 t = ldf(0x1018DC6C); /* JAIZelParam::VOL_BGM_TALKING */
    stf(self + 0x94, t);
    if (m != 0 && ld(self + 0x88) != 0x80000051 && ld(self + 0x88) != 0x80000052) {
        f32 v = mul(ldf(self + 0x90), t);
        v = mul(v, ldf(self + 0x98));
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xA8));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(m, v, 0xF, 0);
    }
    u32 sub = ld(self + 0x7C);
    f32 t2 = ldf(0x1018DC6C);
    stf(self + 0xB0, t2);
    if (sub != 0) JAISound_setSeqInterVolume_l(sub, 0, mul(mul(t2, ldf(self + 0xB4)), ldf(self + 0xB8)), 0xF);
    for (u32 c = 0; c < 8; c++) JAIBasic_setSeCategoryVolume_l(self, c, ld8(0x1018DC9F + c));
}
VERIFY(0x02027D4C, talkIn);

/* 02027F04 JAIZelBasic::bgmStreamPlay() */
void bgmStreamPlay(u32 self) {
    WWHD_FUNC(0x02027F04, void, self);
    if (ld8(self + 0x73) != 0) return;
    u32 s = ld(self + 0x80);
    if (s != 0) gabi::call(0x0280B160, s, 0); /* JAISound::start */
    u32 n = ld(self + 0x8C);
    if (n == 0xC0000001) {
        u32 m = ld(self + 0x78);
        st8(self + 0xCA, 1);
        if (m != 0) JAISound_stop_l(m, 0x1E);
        st8(self + 0x1FAC, 0);
        st8(self + 0xC8, 1);
        st8(self + 0x271, 1);
    } else if (n == 0xC0000000) {
        u32 m = ld(self + 0x78);
        if (m != 0) JAISound_stop_l(m, 0x1E);
        st8(self + 0x1FAC, 0);
        st8(self + 0xC8, 1);
    } else {
        st8(self + 0xC8, 1);
    }
}
VERIFY(0x02027F04, bgmStreamPlay);

/* 02027FD4 JAIZelBasic::changeBgmStatus(s32) */
void changeBgmStatus(u32 self, u32 v) {
    WWHD_FUNC(0x02027FD4, void, self, v);
    u32 m = ld(self + 0x78);
    if (m != 0) JAISound_setPortData_l(m, 9, v & 0xFFFF);
}
VERIFY(0x02027FD4, changeBgmStatus);

/* 02027FF0 JAIZelBasic::changeSubBgmStatus(s32) */
void changeSubBgmStatus(u32 self, u32 v) {
    WWHD_FUNC(0x02027FF0, void, self, v);
    u32 sub = ld(self + 0x7C);
    if (sub == 0) return;
    TTrack_writePortApp_l(JAISound_getSeqParameter_l(sub) + 0x1360, 0x00090000, v & 0xFFFF);
}
VERIFY(0x02027FF0, changeSubBgmStatus);

/* 02028038 JAIZelBasic::cbPracticePlay(Vec*): HD: a different tune in scene 0x39 */
void cbPracticePlay(u32 self, u32 pos) {
    WWHD_FUNC(0x02028038, void, self, pos);
    u32 cam = ld(self);
    st(self + 0xE0, pos);
    u32 mtx = ld(cam + 8);
    gabi::Local<Vec_l> vv;
    u32 v = gabi::ea(vv.get());
    st(v, ld(0x100039DC));
    st(v + 4, ld(0x100039E0));
    st(v + 8, ld(0x100039E4));
    u32 src = pos != 0 ? pos : ld(cam);
    if (src != 0) {
        st(v, ld(src));
        st(v + 4, ld(src + 4));
        st(v + 8, ld(src + 8));
    }
    if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
    u32 id = (s32)ld(self + 0x294) == 0x39 ? 0x80000037 : 0x80000036;
    JAIBasic_startSoundVec_l(self, id, self + 0x7C, 0, 0, 0, 4);
    u32 sub = ld(self + 0x7C);
    st(self + 0x84, id);
    if (sub != 0) JAISound_setSeqInterVolume_l(sub, 0, ldf(self + 0xB0), 0);
    f32 vol = calcPosVolume_l(self, v, 2.0f);
    f32 lr = calcPosPanLR_l(self, v);
    f32 sr = calcPosPanSR_l(self, v, 1.0f);
    sub = ld(self + 0x7C);
    stf(self + 0xB8, vol);
    if (sub != 0) {
        JAISound_setVolume_l(sub, mul(mul(ldf(self + 0xB0), ldf(self + 0xB4)), vol), 0, 0);
        JAISound_setPan_l(ld(self + 0x7C), lr, 0, 0);
        JAISound_setDolby_l(ld(self + 0x7C), sr, 0, 0);
    }
    s32 sc = ld(self + 0x294);
    u32 m = ld(self + 0x78);
    f32 duck = sc == 0x27 ? gabi::fnmsubs(vol, 0.4f, 1.0f) : gabi::fsubs_ppc(1.0f, vol);
    stf(self + 0xAC, duck);
    if (m != 0) {
        f32 t = mul(ldf(self + 0x90), ldf(self + 0x94));
        t = mul(t, ldf(self + 0x98));
        t = mul(t, ldf(self + 0x9C));
        t = mul(t, ldf(self + 0xA0));
        t = mul(t, ldf(self + 0xA4));
        t = mul(t, ldf(self + 0xA8));
        t = mul(t, duck);
        t = mul(t, ldf(self + 0xBC));
        JAISound_setVolume_l(m, t, 0x1E, 0);
    }
}
VERIFY(0x02028038, cbPracticePlay);

/* 020282F8 JAIZelBasic::cbPracticeStop() */
void cbPracticeStop(u32 self) {
    WWHD_FUNC(0x020282F8, void, self);
    u32 sub = ld(self + 0x7C);
    if (sub != 0) {
        u32 n = ld(self + 0x84);
        if (n == 0x80000036 || n == 0x80000037) {
            JAISound_stop_l(sub, 0xF);
            st(self + 0x7C, 0);
            st(self + 0x84, (u32)-1);
        }
    }
    u32 m = ld(self + 0x78);
    stf(self + 0xB8, 1.0f);
    stf(self + 0xAC, 1.0f);
    if (m != 0) {
        f32 t = mul(ldf(self + 0x90), ldf(self + 0x94));
        t = mul(t, ldf(self + 0x98));
        t = mul(t, ldf(self + 0x9C));
        t = mul(t, ldf(self + 0xA0));
        t = mul(t, ldf(self + 0xA4));
        t = mul(t, ldf(self + 0xA8));
        t = mul(t, ldf(self + 0xBC));
        JAISound_setVolume_l(m, t, 0x5A, 0);
    }
    st(self + 0xE0, 0);
}
VERIFY(0x020282F8, cbPracticeStop);

/* 020283D8 JAIZelBasic::bgmMuteMtDragon() (unnamed by the matcher) */
void bgmMuteMtDragon(u32 self) {
    WWHD_FUNC(0x020283D8, void, self);
    st8(self + 0x75, 1);
}
VERIFY(0x020283D8, bgmMuteMtDragon);

/* 020283E4 JAIZelBasic::stSkyCloisters() (unnamed by the matcher) */
void stSkyCloisters(u32 self) {
    WWHD_FUNC(0x020283E4, void, self);
    if (ld(self + 0x88) != 0x80000028) return;
    if (ld8(self + 0xC7) == 0) bgmStart_l(self, 0x80000126, 1, 0);
    st8(self + 0xC7, 4);
}
VERIFY(0x020283E4, stSkyCloisters);

/* 0202844C JAIZelBasic::bgmAllMute(u32 fade) */
void bgmAllMute(u32 self, u32 fade) {
    WWHD_FUNC(0x0202844C, void, self, fade);
    u32 m = ld(self + 0x78);
    stf(self + 0x90, 0.0f);
    if (m == 0) return;
    f32 v = mul(0.0f, ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0x9C));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, ldf(self + 0xA4));
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, ldf(self + 0xAC));
    v = mul(v, ldf(self + 0xBC));
    JAISound_setVolume_l(m, v, fade, 0);
}
VERIFY(0x0202844C, bgmAllMute);

/* 020284B0 JAIZelBasic::stWaterLevelUp() (unnamed by the matcher) */
void stWaterLevelUp(u32 self) {
    WWHD_FUNC(0x020284B0, void, self);
    if (ld8(self + 0xC7) != 0) return;
    if (ld(self + 0x88) != 0x80000028) return;
    bgmStart_l(self, 0x80000125, 1, 0);
}
VERIFY(0x020284B0, stWaterLevelUp);

/* 020284E4 JAIZelBasic::stWaterLevelDown() (unnamed by the matcher) */
void stWaterLevelDown(u32 self) {
    WWHD_FUNC(0x020284E4, void, self);
    if (ld8(self + 0xC7) != 0) return;
    if (ld(self + 0x88) != 0x80000028) return;
    bgmStart_l(self, 0x80000028, 1, 0);
}
VERIFY(0x020284E4, stWaterLevelDown);

/* 02028518 JAIZelBasic::taktModeMute() */
void taktModeMute(u32 self) {
    WWHD_FUNC(0x02028518, void, self);
    u32 m = ld(self + 0x78);
    stf(self + 0xA8, 0.0f);
    if (m != 0) {
        u32 n = ld(self + 0x88);
        if (n == 0x80000021 || n == 0x80000022) {
            stf(self + 0xA8, 1.0f);
        } else {
            f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
            v = mul(v, ldf(self + 0x98));
            v = mul(v, ldf(self + 0x9C));
            v = mul(v, ldf(self + 0xA0));
            v = mul(v, ldf(self + 0xA4));
            v = mul(v, 0.0f);
            v = mul(v, ldf(self + 0xAC));
            v = mul(v, ldf(self + 0xBC));
            JAISound_setVolume_l(m, v, 0xA, 0);
        }
    }
    u32 sub = ld(self + 0x7C);
    stf(self + 0xB4, 0.0f);
    if (sub != 0) JAISound_setVolume_l(sub, mul(mul(ldf(self + 0xB0), 0.0f), ldf(self + 0xB8)), 0xA, 0);
}
VERIFY(0x02028518, taktModeMute);

/* 0202862C JAIZelBasic::taktModeMuteOff() */
void taktModeMuteOff(u32 self) {
    WWHD_FUNC(0x0202862C, void, self);
    u32 m = ld(self + 0x78);
    stf(self + 0xA8, 1.0f);
    if (m != 0) {
        f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
        v = mul(v, ldf(self + 0x98));
        v = mul(v, ldf(self + 0x9C));
        v = mul(v, ldf(self + 0xA0));
        v = mul(v, ldf(self + 0xA4));
        v = mul(v, ldf(self + 0xAC));
        v = mul(v, ldf(self + 0xBC));
        JAISound_setVolume_l(m, v, 0xA, 0);
    }
    f32 c = ldf(self + 0x9C);
    if (!(c == 0.0f)) hdStreamVolume_l(self, 0xA, mul(mul(ldf(self + 0x94), c), ldf(self + 0xA8)));
    if (ld(self + 0x84) == 0x80000031 && ld8(self + 0xC6) == 0) {
        u32 m2 = ld(self + 0x78);
        stf(self + 0x9C, 1.0f);
        if (m2 != 0) {
            f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
            v = mul(v, ldf(self + 0x98));
            v = mul(v, ldf(self + 0xA0));
            v = mul(v, ldf(self + 0xA4));
            v = mul(v, ldf(self + 0xA8));
            v = mul(v, ldf(self + 0xAC));
            v = mul(v, ldf(self + 0xBC));
            JAISound_setVolume_l(m2, v, 0x2D, 0);
        }
    }
    u32 sub = ld(self + 0x7C);
    stf(self + 0xB4, 1.0f);
    if (sub != 0) JAISound_setVolume_l(sub, mul(ldf(self + 0xB0), ldf(self + 0xB8)), 0xA, 0);
}
VERIFY(0x0202862C, taktModeMuteOff);

/* 0202879C JAIZelBasic::bgmNowBattle(f32 dist): starts the battle bgm (HD: the sea battle bgm on the
 * sea) and fades its "enemy near" tracks by distance */
void bgmNowBattle(u32 self, f32 dist) {
    WWHD_FUNC(0x0202879C, void, self, dist);
    if (ld8(self + 0x73) != 0) return;
    if (ld8(self + 0xDA) == 1) return;
    if (ldf(self + 0xA8) == 0.0f) return;
    if (ld8(self + 0x29D) == 0) return;
    if (ld(gabi::ea(dComIfGp_get()) + 0x5B34) != 0 && ld8(gabi::ea(dComIfGp_get()) + 0x5292) != 0) return;
    u32 sc = ld(self + 0x294);
    if (sc < 0x24) {
        if (sc == 2 || sc == 7 || sc == 0x18) return;
        if (sc == 9) {
            enemyNearBy_l(self);
            return;
        }
    } else if (sc < 0x3A) {
        if (sc <= 0x25 || sc == 0x28 || sc == 0x2D || sc == 0x37) return;
    } else {
        if (sc <= 0x3B) return;
        if (sc == 0x40) {
            enemyNearBy_l(self);
            return;
        }
        if (sc >= 0x5B && (sc <= 0x5C || sc == 0x69)) return;
    }
    if (ld8(self + 0xC5) == 1) return;
    if (ld8(self + 0x276) == 1) return;
    u32 n88 = ld(self + 0x88);
    if (n88 < 0x8000000A) {
        if (n88 == 0x80000005 || n88 == 0x80000003) return;
    } else if (n88 == 0x80000105 || n88 == 0x80000010 || n88 == 0x8000000A) {
        return;
    }
    if (ld8(self + 0x277) != 0) return;
    u32 sub = ld(self + 0x7C);
    if (sub != 0 && (ld(self + 0x84) == 0x8000002B || ld(self + 0x84) == 0x80000046)) return;
    if (n88 == 0x80000021 || n88 == 0x80000022) return;
    if (sub != 0 && (ld(self + 0x84) == 0x80000041 || ld(self + 0x84) == 0x80000047)) return;
    if (ld8(self + 0xDE) != 0) return;
    u32 n84 = ld(self + 0x84);
    if (n84 == 0x80000019 || n84 == 0x8000001A) {
        mbossBgmNearByProcess_l(self, dist);
        return;
    }
    if (ld8(self + 0xCB) != 0) return;
    if (ld8(self + 0xC6) != 0) return;
    sub = ld(self + 0x7C);
    st8(self + 0xD0, ld8(0x1018DCB4));
    bool start = true;
    if (sub != 0) {
        n84 = ld(self + 0x84);
        if (n84 == 0x80000004) start = false;
        else if (n84 == 0x8000001C) return;
    }
    if (start) {
        u32 id = checkLinkOnSea_l(self) == 1 ? 0x8000001C : 0x80000004;
        JAIBasic_startSoundVec_l(self, id, self + 0x7C, 0, 0x3C, 0, 4);
        u32 s = ld(self + 0x7C);
        st(self + 0x84, id);
        if (s != 0) JAISound_setSeqInterVolume_l(s, 0, ldf(self + 0xB0), 0);
        u32 m = ld(self + 0x78);
        stf(self + 0x9C, 0.0f);
        if (m != 0) {
            f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
            v = mul(v, ldf(self + 0x98));
            v = mul(v, 0.0f);
            v = mul(v, ldf(self + 0xA0));
            v = mul(v, ldf(self + 0xA4));
            v = mul(v, ldf(self + 0xA8));
            v = mul(v, ldf(self + 0xAC));
            v = mul(v, ldf(self + 0xBC));
            JAISound_setVolume_l(m, v, 0x3C, 0);
        }
        if (checkLinkOnSea_l(self) == 1) {
            s32 on = checkLinkOnBoardSea_l(self);
            bgmMute_l(self, self + 0x7C, 0x8000001C, (on ^ 1) != 0 ? 1 : 0, 0);
        } else {
            bgmMute_l(self, self + 0x7C, 0x80000004, 0, 0);
        }
        sub = ld(self + 0x7C);
        if (sub == 0) return;
        if (ld(self + 0x84) == 0x8000001C) return;
    }
    /* enemy near / far tracks */
    if (!(dist > ldf(0x1018DC78))) {
        s32 d1 = (s8)ld8(self + 0xD1);
        st(self + 0xD4, 0);
        if (d1 == 0) {
            JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCAF), 1.0f, ld(0x1018DC7C));
            JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB0), 1.0f, ld(0x1018DC7C));
            st8(self + 0xD1, 1);
        }
        sub = ld(self + 0x7C);
    } else if ((s8)ld8(self + 0xD1) == 1) {
        u32 t = ld(0x1018DC80);
        st8(self + 0xD1, 0);
        st(self + 0xD4, t);
        sub = ld(self + 0x7C);
    }
    u32 d9 = ld8(self + 0xD9);
    u32 fade = ld(0x1018DC8C);
    if (d9 == 0) {
        JAISound_setTrackVolume_l(sub, ld8(0x1018DCB3), 1.0f, fade);
        JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB2), 0.0f, ld(0x1018DC8C));
    } else {
        JAISound_setTrackVolume_l(sub, ld8(0x1018DCB2), 1.0f, fade);
        JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB3), 0.0f, ld(0x1018DC8C));
    }
}
VERIFY(0x0202879C, bgmNowBattle);

/* 02028D6C JAIZelBasic::bgmNowKaitengiri() */
void bgmNowKaitengiri(u32 self) {
    WWHD_FUNC(0x02028D6C, void, self);
    if (ld8(self + 0xD8) == 0) {
        u32 sub = ld(self + 0x7C);
        if (sub != 0) {
            u32 n = ld(self + 0x84);
            if (n > 0x8000001A) {
            } else if (n >= 0x80000019) {
                JAISound_setTrackVolume_l(sub, ld8(0x1018DCB6), 1.0f, ld(0x1018DC84));
            } else if (n == 0x80000004) {
                JAISound_setTrackVolume_l(sub, ld8(0x1018DCB1), 1.0f, ld(0x1018DC84));
            }
        }
    }
    st8(self + 0xD8, 2);
}
VERIFY(0x02028D6C, bgmNowKaitengiri);

/* 02028E24 JAIZelBasic::getRandomU32(u32 n): the shift-register generator at 1018D4F0 */
u32 getRandomU32(u32 self, u32 n) {
    WWHD_FUNC(0x02028E24, u32, self, n);
    u32 t = ld(0x1018D4F0);
    u32 s = (t << 1) + ((t >> 31) ^ ((t >> 6) & 0x10));
    st(0x1018D4F0, s);
    u32 q = n != 0 ? s / n : 0;
    return s - q * n;
}
VERIFY(0x02028E24, getRandomU32);

/* 02028E54 JAIZelBasic::bgmHitSound(s32 kind) */
void bgmHitSound(u32 self, u32 kind) {
    WWHD_FUNC(0x02028E54, void, self, kind);
    u32 n = ld(self + 0x84);
    if (n > 0x8000001A) return;
    if (n < 0x80000019 && n != 0x80000004) return;
    if (ld8(self + 0xD9) == 0) return;
    u16 v;
    switch (kind) {
    case 1: v = (u16)(getRandomU32(self, 2) + 5); break;
    case 2: v = 9; break;
    case 3: v = (u16)(getRandomU32(self, 2) + 10); break;
    case 4: v = (u16)(getRandomU32(self, 2) + 7); break;
    default: v = (u16)((u16)getRandomU32(self, 4) + 1); break;
    }
    u32 sub = ld(self + 0x7C);
    if (sub == 0) return;
    u32 track = JAISound_getSeqParameter_l(sub) + 0x1360;
    gabi::Local<be<u16>> tmp;
    u32 tp = gabi::ea(tmp.get());
    st16(tp, 0);
    gabi::call(0x02818C68, track, 0x100A0001, tp); /* TTrack::readPortApp */
    TTrack_writePortApp_l(track, 0x00090000, v);
    TTrack_writePortApp_l(track, 0x000A0000, ld16(tp));
}
VERIFY(0x02028E54, bgmHitSound);

/* 02028FE0 JAIZelBasic::bgmSetSwordUsing(s32) (unnamed by the matcher; 025E1D08 is the m_Do_audio shim) */
void bgmSetSwordUsing(u32 self, u32 v) {
    WWHD_FUNC(0x02028FE0, void, self, v);
    st8(self + 0xD9, v);
}
VERIFY(0x02028FE0, bgmSetSwordUsing);

/* 02028FE8 JAIZelBasic::prepareLandingDemo(s32) (unnamed by the matcher) */
void prepareLandingDemo(u32 self, u32 v) {
    WWHD_FUNC(0x02028FE8, void, self, v);
    st8(self + 0x70, v);
    bgmStop_l(self, 0x5A, 0);
}
VERIFY(0x02028FE8, prepareLandingDemo);

/* 02028FF8 JAIZelBasic::startLandingDemo() */
void startLandingDemo(u32 self) {
    WWHD_FUNC(0x02028FF8, void, self);
    u32 c = ld8(self + 0x70);
    if (c == 2) bgmStart_l(self, 0x8000003A, 0, 0);
    else if (c == 3) bgmStart_l(self, 0x80000039, 0, 0);
}
VERIFY(0x02028FF8, startLandingDemo);

/* 02029038 JAIZelBasic::endLandingDemo() (unnamed by the matcher) */
void endLandingDemo(u32 self) {
    WWHD_FUNC(0x02029038, void, self);
    st8(self + 0x70, 0);
}
VERIFY(0x02029038, endLandingDemo);

/* 02029044 JAIZelBasic::seStopActor(Vec*, u32) (unnamed by the matcher) */
void seStopActor(u32 self, u32 pos, u32 id) {
    WWHD_FUNC(0x02029044, void, self, pos, id);
    if ((s32)id == -1) {
        gabi::call(0x02802994, self, pos); /* JAIBasic::stopAllSound(pos) */
    } else if (pos == 0) {
        gabi::call(0x02802AEC, self, id);
    } else {
        gabi::call(0x02802C60, self, id, pos);
    }
}
VERIFY(0x02029044, seStopActor);

/* 02029070 JAIZelBasic::seDeleteObject(Vec*) (unnamed by the matcher) */
void seDeleteObject(u32 self, u32 pos) {
    WWHD_FUNC(0x02029070, void, self, pos);
    gabi::call(0x02802E00, self, pos); /* JAIBasic::deleteObject */
}
VERIFY(0x02029070, seDeleteObject);

/* 02029074 JAIZelBasic::charVoicePlay(s32, s32, Vec*, s8): HD skips the voice already playing */
void charVoicePlay(u32 self, u32 a, u32 b, u32 pos, u32 reverb) {
    WWHD_FUNC(0x02029074, void, self, a, b, pos, reverb);
    if (ld8(self + 0x271) == 1) return;
    u32 v = ((a << 8) + b) & 0xFFFF;
    if (v == ld16(self + 0x20D4)) return;
    u32 s = ld(self + 0x20D0);
    if (s != 0) {
        JAISound_stop_l(s, 0);
        st16(self + 0x20D4, 0xFFFF);
    }
    JAIBasic_startSoundVec_l(self, 0x481F, self + 0x20D0, pos, 0, 0, 4);
    if (ld(self + 0x20D0) == 0) return;
    JAISound_setPortData_l(ld(self + 0x20D0), 8, v);
    JAISound_setPortData_l(ld(self + 0x20D0), 9, reverb & 0xFFFF);
    st16(self + 0x20D4, v);
}
VERIFY(0x02029074, charVoicePlay);

/* 0202914C JAIZelBasic::messageSePlay(u16 msg, Vec* pos, s8 reverb): table at 1018D53C {u16 a, u16 b} */
void messageSePlay(u32 self, u32 msg, u32 pos, u32 reverb) {
    WWHD_FUNC(0x0202914C, void, self, msg, pos, reverb);
    u32 p = pos;
    if (pos != 0) {
        st(self + 0x20D8, ld(pos));
        st(self + 0x20DC, ld(pos + 4));
        st(self + 0x20E0, ld(pos + 8));
        p = self + 0x20D8;
    }
    if (msg >= 0x118) return;
    if (msg == 0x9F || (msg >= 0xB4 && msg <= 0xBA) || msg == 0x102 || msg == 0x104) p = 0;
    if (msg == 0x10D || msg == 0x115) return;
    u32 e = 0x1018D53C + msg * 4;
    u32 a = ld16(e), b = ld16(e + 2);
    if (a == 0xFFFF || b == 0xFFFF) return;
    u32 t = a & 0xF000;
    if (t == 0) {
        charVoicePlay(self, a, b, p, reverb);
    } else if (t == 0x1000 || t == 0x2000 || t == 0x3000 || t == 0x8000) {
        JAIZelBasic_seStart_l(self, b, p, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
    }
}
VERIFY(0x0202914C, messageSePlay);

/* a random entry of a voice table (8 bytes per voice, {sel, vowel} pairs): TRandom * 4 */
static inline u32 randomIndex(u32 self) {
    f32 f = TRandom_get_l(self + 0x216C) * 4.0f;
    if (!(f < 2147483648.0f)) return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000;
    return (u32)gabi::ftoi(f);
}

/* 020292B4 JAIZelBasic::getLinkVoiceVowel(u32): linkVoiceTable at 1018D99C */
u32 getLinkVoiceVowel(u32 self, u32 n) {
    WWHD_FUNC(0x020292B4, u32, self, n);
    u32 tab = 0x1018D99C + n * 8;
    u32 i, k = 0;
    for (i = 0; i < 400; i++) {
        k = randomIndex(self);
        if (ld8(tab + k * 2) != 0xFF) break;
    }
    if (i == 400) return 0xFF;
    return ld8(tab + k * 2 + 1);
}
VERIFY(0x020292B4, getLinkVoiceVowel);

/* 020293CC JAIZelBasic::linkVoiceStart(u32 n, Vec* pos, u8 vowel, s8 reverb) */
void linkVoiceStart(u32 self, u32 n, u32 pos, u32 vowel, u32 reverb) {
    WWHD_FUNC(0x020293CC, void, self, n, pos, vowel, reverb);
    if (ld8(self + 0x29D) == 0) return;
    if (ld(self + 0x8C) == 0xC0000005 && (s32)n == 0x10) {
        if (ld8(self + 0xC8) != 0) return;
        if (getRandomU32(self, 3) != 0) return;
    }
    if (ld(self + 0x88) == 0x8000000B && (s32)n == 3) {
        u32 m = ld(self + 0x78);
        f32 t = ldf(0x1018DC6C);
        stf(self + 0xA0, t);
        if (m != 0 && ld(self + 0x7C) == 0) {
            f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
            v = mul(v, ldf(self + 0x98));
            v = mul(v, ldf(self + 0x9C));
            v = mul(v, t);
            v = mul(v, ldf(self + 0xA4));
            v = mul(v, ldf(self + 0xA8));
            v = mul(v, ldf(self + 0xAC));
            v = mul(v, ldf(self + 0xBC));
            JAISound_setVolume_l(m, v, 2, 0);
        }
    } else if ((s32)n == 0xC) {
        pos = 0;
    }
    u32 tab = 0x1018D99C + n * 8;
    u32 sel = 0;
    bool found = false;
    if ((s32)vowel != 0xFF) {
        for (u32 i = 0; i < 400; i++) {
            u32 k = randomIndex(self);
            if (ld8(tab + k * 2 + 1) == vowel) {
                u32 c = ld8(tab + k * 2);
                if (c != 0xFF) {
                    sel = c;
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) {
        sel = ld8(tab);
        if (sel == 0xFF) return;
    }
    JAIZelBasic_seStart_l(self, sel + 0x1800, pos, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}
VERIFY(0x020293CC, linkVoiceStart);

/* 02029638 JAIZelBasic::shipCruiseSePlay(Vec* pos, f32 speed) */
void shipCruiseSePlay(u32 self, u32 pos, f32 speed) {
    WWHD_FUNC(0x02029638, void, self, pos, speed);
    u32 m = ld(self + 0x78);
    stf(self + 0x1FB0, speed);
    if (m != 0) {
        u32 n = ld(self + 0x88);
        if (n == 0x8000002E || n == 0x8000003C) {
            u32 set = ld8(self + 0x1FAD) == 0 && speed < 0.2f ? 1 : 0;
            bgmMute_l(self, self + 0x78, n == 0x8000002E ? 0x8000002E : 0x8000003C, set, 0x1E);
        }
    }
    if (ld8(self + 0x271) == 1) return;
    u32 cam = ld(self);
    u32 mtx = ld(cam + 8);
    gabi::Local<Vec_l> vv;
    u32 v = gabi::ea(vv.get());
    st(v, ld(0x100039F0));
    st(v + 4, ld(0x100039F4));
    st(v + 8, ld(0x100039F8));
    u32 src = pos != 0 ? pos : ld(cam);
    if (src != 0) {
        st(v, ld(src));
        st(v + 4, ld(src + 4));
        st(v + 8, ld(src + 8));
    }
    if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
    f32 y = ldf(v + 4), x = ldf(v), z = ldf(v + 8);
    f32 d = sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
    if (!(d < JAIGlobalParameter_getParamDistanceMax_l())) return;
    f32 vol2 = 1.0f;
    if (!(speed > 0.1f)) {
        JAIBasic_startSoundVec_l(self, 0x3006, self + 0x1FB8, pos, 0, 0, 4);
        if (ld(self + 0x1FB8) != 0) JAISound_setVolume_l(ld(self + 0x1FB8), 1.0f, 0, 0);
        return;
    }
    u32 id2;
    f32 vol1;
    bool play1 = true;
    if (!(speed > 0.5f)) {
        vol1 = (speed - 0.1f) / 0.4f;
        vol2 = 1.0f - vol1;
        id2 = 0x3006;
        if (vol1 == 0.0f) play1 = false;
    } else if (!(speed > 0.8f) || !(speed > 0.9f)) {
        id2 = 0x3005;
        vol1 = 1.0f;
        vol2 = (speed - 0.5f) / 0.4f;
    } else {
        vol1 = 1.0f;
        id2 = 0x3005;
    }
    if (play1) {
        JAIBasic_startSoundVec_l(self, 0x3004, self + 0x1FB4, pos, 0, 0, 4);
        if (ld(self + 0x1FB4) != 0) JAISound_setVolume_l(ld(self + 0x1FB4), vol1, 0, 0);
    }
    if (vol2 == 0.0f) return;
    JAIBasic_startSoundVec_l(self, id2, self + 0x1FB8, pos, 0, 0, 4);
    if (ld(self + 0x1FB8) != 0) JAISound_setVolume_l(ld(self + 0x1FB8), vol2, 0, 0);
}
VERIFY(0x02029638, shipCruiseSePlay);

/* 020299D0 JAIZelBasic::setShipSailState(s32) (unnamed by the matcher) */
void setShipSailState(u32 self, u32 v) {
    WWHD_FUNC(0x020299D0, void, self, v);
    st8(self + 0x1FAD, v);
}
VERIFY(0x020299D0, setShipSailState);

/* 020299D8 JAIZelBasic::monsSeStart(u32 id, Vec* pos, u32 obj, u32 info, s8 reverb): 30 monster
 * sound slots {obj, sound} at +0x1FBC */
void monsSeStart(u32 self, u32 id, u32 pos, u32 obj, u32 info, u32 reverb) {
    WWHD_FUNC(0x020299D8, void, self, id, pos, obj, info, reverb);
    if (ld8(self + 0x271) == 1) return;
    if (pos != 0) ptr_check_l(pos);
    bool kurobo = id - 0x486D < 3 || id - 0x48F7 < 6;
    if (!kurobo && (s32)ld(self + 0x294) == 0x2D) kurobo = (id >= 0x48F1 && id <= 0x48F6) || id == 0x48FD;
    if (kurobo) {
        kuroboVoicePlay_l(self, id, pos, reverb);
        return;
    }
    const u32 tab = self + 0x1FBC;
    u32 slot = 0;
    bool assigned = false;
    u32 i;
    for (i = 0; i < 30; i++) {
        if (ld(tab + i * 8) != obj) continue;
        u32 s = ld(tab + i * 8 + 4);
        if (s != 0) JAISound_stop_l(s, 0);
        slot = i;
        assigned = true;
        break;
    }
    if (!assigned) {
        slot = 0x1D;
        for (u32 k = 0; k < 30; k++) {
            if ((s32)ld(tab + k * 8) == -1) {
                st(tab + k * 8, obj);
                slot = k;
                break;
            }
        }
    }
    if (info > 9) info = obj % 10;
    u32 h = tab + slot * 8 + 4;
    JAIBasic_startSoundVec_l(self, id, h, pos, 0, info, 4);
    if (ld(h) != 0) JAISound_setPortData_l(ld(h), 9, reverb & 0xFFFF);
}
VERIFY(0x020299D8, monsSeStart);

/* 02029B88 __sinit_JAIZelBasic_cpp (header statics) */
void __sinit_JAIZelBasic_cpp() {
    WWHD_FUNC(0x02029B88, void);
    header_sinit(0x101FFC5C, 0x1018D4F4, 0x10003A00);
}
VERIFY(0x02029B88, __sinit_JAIZelBasic_cpp);

/* 02029C1C: this TU's sead::SafeString deleting destructor (vtable 1000382C) */
void basic_safestring_deleting_dtor(u32 self, u32 flags) {
    WWHD_FUNC(0x02029C1C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    gabi::call(0x0273AF40, self); /* __dl */
}
VERIFY(0x02029C1C, basic_safestring_deleting_dtor);

/* 02029C30 JAIZelBasic::getMapInfoFxParameter(u32) (virtual) */
f32 getMapInfoFxParameter(u32 self, u32 n) {
    WWHD_FUNC(0x02029C30, f32, self, n);
    return ldf(0x10003898); /* 0.0 */
}
VERIFY(0x02029C30, getMapInfoFxParameter);

/* 02029C3C JAIZelBasic::getMapInfoGround(u32) (virtual) */
u32 getMapInfoGround(u32 self, u32 n) {
    WWHD_FUNC(0x02029C3C, u32, self, n);
    return 0;
}
VERIFY(0x02029C3C, getMapInfoGround);

/* 02029C44: constructor of the 8-byte allocation record (+0x20E8 table) */
u32 allocRecord_ct(u32 self) {
    WWHD_FUNC(0x02029C44, u32, self);
    if (self == 0) {
        self = gabi::call<u32>(0x0273AD10, 8);
        if (self == 0) return 0;
    }
    st(self + 4, 0);
    st(self, 0);
    return self;
}
VERIFY(0x02029C44, allocRecord_ct);

/* 02029C84 JAIZelBasic deleting destructor (vtable 10003A08 slot 0xC) */
void JAIZelBasic_deleting_dtor(u32 self, u32 flags) {
    WWHD_FUNC(0x02029C84, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02029C84, JAIZelBasic_deleting_dtor);

/* 02029C98: an empty per-TU function */
void basic_empty_02029C98(u32 a) {
    WWHD_FUNC(0x02029C98, void, a);
}
VERIFY(0x02029C98, basic_empty_02029C98);

/* 02029C9C: this TU's SafeString slot-0x14 function (empty; called before string comparisons) */
void basic_safestring_02029C9C(u32 a) {
    WWHD_FUNC(0x02029C9C, void, a);
}
VERIFY(0x02029C9C, basic_safestring_02029C9C);

} // namespace JAIZelBasic_8_cpp
