/* JAIZelBasic (WWHD), part 6: zeldaGFrameWork (02024478..0202573B).
 * A "Nonmatching" stub in the GameCube decompilation,
 * so the body is written from the WWHD code and verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_6_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline void processLevObjSE_l(u32 b) { gabi::call(0x020212EC, b); }
static inline void cbPracticeProcess_l(u32 b) { gabi::call(0x02021534, b); }
static inline void processDemoFanfareMute_l(u32 b) { gabi::call(0x02021DC4, b); }
static inline s32 checkBgmPlaying_l(u32 b) { return gabi::call<s32>(0x02021E30, b); }
static inline void bgmStop_l(u32 b, u32 fade, u32 keep) { gabi::call(0x02021F28, b, fade, keep); }
static inline void bgmStart_l(u32 b, u32 id, u32 fade, u32 keep) { gabi::call(0x0202204C, b, id, fade, keep); }
static inline s32 checkSubBgmPlaying_l(u32 b) { return gabi::call<s32>(0x0202148C, b); }
static inline void subBgmStart_l(u32 b, u32 id) { gabi::call(0x0201DE00, b, id); }
static inline void subBgmStopInner_l(u32 b) { gabi::call(0x0201D9C8, b); }
static inline s32 isDemo_l(u32 b) { return gabi::call<s32>(0x0201E5CC, b); }
static inline s32 checkSeqIDDemoPlaying_l(u32 b, u32 id) { return gabi::call<s32>(0x020217A4, b, id); }
static inline s32 checkStream020233C8_l(u32 b, u32 id) { return gabi::call<s32>(0x020233C8, b, id); }
static inline s32 checkEventBit_l(u32 b, u32 f) { return gabi::call<s32>(0x0201E480, b, f); }
static inline u32 checkSeaBgmID_l(u32 b) { return gabi::call<u32>(0x02023410, b); }
static inline void sceneBgmStart_l(u32 b) { gabi::call(0x020234A4, b); }
static inline u32 expandSceneBgmNum_l(u32 b, u32 n) { return gabi::call<u32>(0x02023608, b, n); }
static inline void processHeartGaugeSound_l(u32 b) { gabi::call(0x02024334, b); }

static inline void se(u32 self, u32 id, f32 vol) { JAIZelBasic_seStart_l(self, id, 0, 0, 0, 1.0f, vol, -1.0f, -1.0f, 0); }
static inline u8 dec(u32 a) {
    u8 x = ld8(a);
    return x != 0 ? x - 1 : 0;
}
/* the main-bgm volume with all factors read */
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

/* 02024478 JAIZelBasic::zeldaGFrameWork(): the per-frame sound work */
void zeldaGFrameWork(u32 self) {
    WWHD_FUNC(0x02024478, void, self);
    /* frame counter, timers */
    if (ld8(self + 0x29D) != 0) {
        u32 c = ld(self + 0x38);
        st(self + 0x38, c < 0xFFFFFFFF ? c + 1 : 0xFFFFFFFF);
    } else {
        st(self + 0x38, 0);
    }
    processLevObjSE_l(self);
    cbPracticeProcess_l(self);
    u8 a26a = dec(self + 0x26A);
    st(self + 0x1FA4, 0);
    st8(self + 0x269, 0);
    st8(self + 0x26C, 0);
    st8(self + 0x26B, 0);
    st8(self + 0x26A, a26a);
    st8(self + 0x272, dec(self + 0x272));
    st8(self + 0x273, dec(self + 0x273));
    st8(self + 0x277, dec(self + 0x277));
    st8(self + 0x26D, dec(self + 0x26D));
    st8(self + 0x26E, dec(self + 0x26E));
    st8(self + 0x26F, dec(self + 0x26F));
    st8(self + 0x270, dec(self + 0x270));
    st(self + 0x1F08, 0);
    st8(self + 0x20AD, 0);
    st8(self + 0x20AC, 0);
    u32 play = gabi::ea(dComIfGp_get());
    if (ld8(play + 0x5292) != 0) st8(self + 0x71, 5);
    else st8(self + 0x71, dec(self + 0x71));
    st8(self + 0xDE, dec(self + 0xDE));
    processDemoFanfareMute_l(self);
    if (ld(self + 0x88) == 0x8000000F && checkBgmPlaying_l(self) == 0) {
        bgmStop_l(self, 0, 0);
        bgmStart_l(self, 0x80000007, 0, 0);
    }
    /* restore the sub bgm after it was interrupted */
    u32 n84 = ld(self + 0x84);
    bool restore;
    if (n84 >= 0x80000030)
        restore = n84 == 0x80000061 || n84 == 0x8000005D || n84 == 0x8000004F || n84 <= 0x80000032;
    else
        restore = n84 == 0x80000027 || (n84 <= 0x80000025 && (n84 >= 0x80000024 || n84 == 0x80000002));
    if (restore && checkSubBgmPlaying_l(self) == 0) {
        f32 b4 = ldf(self + 0xB4);
        st8(self + 0x276, 0);
        if (b4 != 0.0f && ld8(self + 0xCB) != 0) {
            u32 c6 = ld8(self + 0xC6);
            if (c6 == 1) subBgmStart_l(self, 0x8000001A);
            else if (c6 == 2) subBgmStart_l(self, 0x80000046);
            else if (c6 == 3) subBgmStart_l(self, 0x80000047);
            else {
                st(self + 0xC0, (u32)-2);
                subBgmStopInner_l(self);
            }
            st8(self + 0xCB, 0);
        }
    }
    u32 n88 = ld(self + 0x88);
    if (n88 == 0x80000045) {
        if (checkBgmPlaying_l(self) == 0) bgmStart_l(self, 0x8000003F, 0, 1);
    } else if (n88 == 0x80000013) {
        if (checkBgmPlaying_l(self) == 0) bgmStart_l(self, 0x80000017, 0, 1);
    }
    /* sub bgm fade-out countdown (+0xC0) */
    s32 c0 = ld(self + 0xC0);
    if (c0 != -1) {
        if (c0 > 0) {
            st(self + 0xC0, c0 - 1);
        } else {
            subBgmStopInner_l(self);
            st(self + 0xC0, (u32)-1);
        }
    }
    bool demoCheck = true;
    if (ld(self + 0x88) == 0x80000006) {
        if (ld8(self + 0x75) == 1) {
            u32 m = ld(self + 0x78);
            stf(self + 0xA0, 0.0f);
            if (m != 0) {
                f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
                v = mul(v, ldf(self + 0x98));
                v = mul(v, ldf(self + 0x9C));
                v = mul(v, 0.0f);
                v = mul(v, ldf(self + 0xA4));
                v = mul(v, ldf(self + 0xA8));
                v = mul(v, ldf(self + 0xAC));
                v = mul(v, ldf(self + 0xBC));
                JAISound_setVolume_l(m, v, 0x1E, 0);
            }
            st8(self + 0x75, 0);
        } else if (ldf(self + 0xA0) == 0.0f) {
            u32 m = ld(self + 0x78);
            stf(self + 0xA0, 1.0f);
            if (m != 0) {
                f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
                v = mul(v, ldf(self + 0x98));
                v = mul(v, ldf(self + 0x9C));
                v = mul(v, ldf(self + 0xA4));
                v = mul(v, ldf(self + 0xA8));
                v = mul(v, ldf(self + 0xAC));
                v = mul(v, ldf(self + 0xBC));
                JAISound_setVolume_l(m, v, 0x1E, 0);
            }
        }
    }
    (void)demoCheck;
    if (isDemo_l(self) == 1 && checkSeqIDDemoPlaying_l(self, 0x80000044) != 0) bgmStart_l(self, 0x80000044, 0, 0);

    /* scene ambience and the volume of the main bgm by the camera position */
    s32 sc = ld(self + 0x294);
    bool skipScenes = false;
    if (sc == 0x21) {
        skipScenes = true;
        if (ld8(self + 0x29D) == 1) {
            u32 cp = ld(ld(self));
            if (cp != 0) {
                f32 y = ldf(cp + 4);
                if (!(y < 2400.0f)) {
                    f32 v;
                    bool go = true;
                    if (y < 3200.0f) {
                        v = (y - 2400.0f) / 800.0f;
                        if (v == 0.0f) go = false;
                    } else {
                        v = 1.0f;
                    }
                    if (go) {
                        JAIBasic_startSoundVec_l(self, 0x1060, self + 0x1FA8, 0, 0, 0, 4);
                        if (ld(self + 0x1FA8) != 0) JAISound_setVolume_l(ld(self + 0x1FA8), v, 0, 0);
                        sc = ld(self + 0x294);
                        skipScenes = false;
                    }
                }
            }
        }
    }
    if (!skipScenes) {
        if (sc == 0x12 || sc == 0x55 || sc == 0x13) {
            f32 y = 0.0f;
            f32 v29 = 0.0f;
            u32 room;
            bool toF14 = false;
            bool haveRoom = false;
            if (ld8(self + 0x29D) == 1) {
                u32 cp = ld(ld(self));
                if (cp != 0) y = ldf(cp + 4);
                if (cp != 0 && !(y < 2400.0f)) {
                    if (!(y < 3200.0f)) {
                        room = ld8(self + 0x2A8);
                        v29 = 1.0f;
                    } else {
                        room = ld8(self + 0x2A8);
                        v29 = (y - 2400.0f) / 800.0f;
                    }
                } else {
                    room = ld8(self + 0x2A8);
                    v29 = 0.0f;
                }
                if (room == 0xB && isDemo_l(self) == 1) toF14 = true;
                if (!toF14) {
                    if (v29 == 0.0f) {
                        haveRoom = true;
                    } else if (checkStream020233C8_l(self, 0xC0000036) == 0) {
                        JAIBasic_startSoundVec_l(self, 0x1060, self + 0x1FA8, 0, 0, 0, 4);
                        if (ld(self + 0x1FA8) != 0) JAISound_setVolume_l(ld(self + 0x1FA8), v29, 0, 0);
                    }
                }
            }
            if (!toF14) {
                if (!haveRoom) room = ld8(self + 0x2A8);
                bool setVol = false;
                if (room == 0x2C) {
                    if (checkEventBit_l(self, 1) == 1) {
                        f32 v;
                        if (ld(self + 0x88) == 0x8000000A || y < 1800.0f) v = 1.0f;
                        else if (!(y < 3000.0f)) v = 0.0f;
                        else v = 1.0f - (y - 1800.0f) / 1200.0f;
                        u32 m = ld(self + 0x78);
                        stf(self + 0xBC, v);
                        setVol = m != 0;
                    }
                } else if (room == 0xD) {
                    f32 v;
                    if (ld(self + 0x88) == 0x8000000A || ld8(self + 0x70) == 2 || y < 1800.0f) v = 1.0f;
                    else if (!(y < 3000.0f)) v = ldf(0x10003910); /* 0.3 */
                    else v = gabi::fmadds(1.0f - (y - 1800.0f) / 1200.0f, 0.7f, 0.3f);
                    u32 m = ld(self + 0x78);
                    stf(self + 0xBC, v);
                    setVol = m != 0;
                } else if (room == 0x29) {
                    if (ld8(self + 0x70) != 3) {
                        u32 pl = gabi::ea(dComIfGp_get());
                        if (ld8(pl + 0x5292) == 0) {
                            u32 cp = ld(ld(self));
                            f32 dz = ldf(cp + 8) - 198000.0f;
                            f32 dx = ldf(cp) - 195000.0f;
                            f32 d = sqrtf_l(gabi::fmadds(dx, dx, dz * dz));
                            f32 v = 1.0f;
                            if (d > 8000.0f) {
                            } else if (d > 5000.0f) {
                                v = (d - 5000.0f) / 3000.0f;
                            } else {
                                v = 0.0f;
                            }
                            u32 n = ld(self + 0x88);
                            u32 b275 = ld8(self + 0x275);
                            if (n == 0x8000000A) v = 1.0f;
                            if (b275 == 0) stf(self + 0xA0, v);
                            setVol = ld(self + 0x78) != 0;
                        }
                    }
                }
                if (setVol) JAISound_setVolume_l(ld(self + 0x78), mainVol(self), 0, 0);
                sc = ld(self + 0x294);
            }
        }
        if (sc == 0x19) {
            u32 cp = ld(ld(self));
            u32 n = ld(self + 0x88);
            f32 y = cp != 0 ? ldf(cp + 4) : 0.0f;
            f32 v;
            if (n == 0x8000000A || y < 2500.0f) v = 1.0f;
            else if (y < 5000.0f) v = 1.0f - (y - 2500.0f) / 2500.0f;
            else v = 0.0f;
            u32 m = ld(self + 0x78);
            stf(self + 0xBC, v);
            if (m != 0) {
                f32 t = mul(ldf(self + 0x90), ldf(self + 0x94));
                t = mul(t, ldf(self + 0x98));
                t = mul(t, ldf(self + 0x9C));
                t = mul(t, ldf(self + 0xA0));
                t = mul(t, ldf(self + 0xA4));
                t = mul(t, ldf(self + 0xA8));
                t = mul(t, ldf(self + 0xAC));
                t = mul(t, v);
                JAISound_setVolume_l(m, t, 0, 0);
            }
            sc = ld(self + 0x294);
        }
        if (sc == 0x35) {
            u32 cp = ld(ld(self));
            f32 z = ldf(cp + 8), x = ldf(cp);
            f32 d = sqrtf_l(gabi::fmadds(x, x, z * z));
            f32 v = 1.0f;
            if (d > 8500.0f) {
            } else if (d > 6500.0f) {
                v = (d - 6500.0f) / 2000.0f;
            } else {
                v = 0.0f;
            }
            u32 n = ld(self + 0x88);
            u32 m = ld(self + 0x78);
            if (n == 0x8000000A) v = 1.0f;
            stf(self + 0xA0, v);
            if (m != 0) {
                f32 t = mul(ldf(self + 0x90), ldf(self + 0x94));
                t = mul(t, ldf(self + 0x98));
                t = mul(t, ldf(self + 0x9C));
                t = mul(t, v);
                t = mul(t, ldf(self + 0xA4));
                t = mul(t, ldf(self + 0xA8));
                t = mul(t, ldf(self + 0xAC));
                t = mul(t, ldf(self + 0xBC));
                JAISound_setVolume_l(m, t, 0, 0);
            }
        }
    }
    if (ld(self + 0x88) == 0x8000001E) {
        u32 s = ld(self + 0x294);
        if (s < 0x4E || (s > 0x52 && s != 0x5D)) se(self, 0x703B, 1.0f);
    }
    /* queued bgm (+0xCC) once nothing plays */
    u32 cc = ld8(self + 0xCC);
    if (cc != 0 && checkBgmPlaying_l(self) == 0) {
        if (cc == 1) {
            if (ld(self + 0x78) == 0 || (ld(self + 0x88) != 0x8000002E && ld(self + 0x88) != 0x8000003C))
                bgmStart_l(self, checkSeaBgmID_l(self), 0x5A, 0);
            st8(self + 0x28E, 0);
            st8(self + 0xCC, 0);
        } else if (cc == 2) {
            sceneBgmStart_l(self);
            st8(self + 0x28E, 0);
            st8(self + 0xCC, 0);
        } else {
            if (cc == 3) {
                u32 room = ld8(self + 0x2A8);
                if (room == 0x2C && checkEventBit_l(self, 0xE20) == 1)
                    bgmStart_l(self, 0x80000055, 0, 0);
                else
                    bgmStart_l(self, expandSceneBgmNum_l(self, ld16(0x1018E60C + room * 4)), 0, 0);
            }
            st8(self + 0xCC, 0);
        }
    }
    gabi::call(0x020239C8, self); /* processTime */
    gabi::call(0x02023A94, self); /* changeSeaBgm */
    gabi::call(0x02023E0C, self); /* bgmBattleGFrame */
    gabi::call(0x02024018, self); /* mbossBgmMuteProcess */
    gabi::call(0x02024190, self); /* enemyNearByGFrame */
    gabi::call(0x02024278, self); /* stSkyCloistersProcess */
    /* scene ambience loops */
    if (ld8(self + 0x29D) == 1) {
        u32 s = ld(self + 0x294);
        switch (s) {
        case 7: se(self, 0x1065, 1.0f); break;
        case 0xC:
        case 0x54:
        case 0x58: se(self, 0x7019, 1.0f); break;
        case 0x11: se(self, 0x701A, 1.0f); break;
        case 0x18: se(self, 0x1075, 1.0f); break;
        case 0x21: se(self, 0x10A5, 1.0f); break;
        case 0x23:
            if (checkSubBgmPlaying_l(self) == 1) se(self, 0x105A, 0.3f);
            else se(self, 0x105A, ldf(0x100039BC)); /* 0.9 */
            break;
        case 0x29: se(self, 0x1087, 1.0f); break;
        case 0x5A: se(self, 0x10A6, 1.0f); break;
        case 0x5C: {
            u32 r = ld8(self + 0x2A9);
            if (r != 8 && r != 9 && r != 0xA && r != 0xB) se(self, 0x706C, 1.0f);
            break;
        }
        case 9:
        case 0x40: {
            if (JAIZelBasic_checkStreamPlaying_l(self, 0xC0000004) != 0) break;
            u32 cp = ld(ld(self));
            f32 y = cp != 0 ? ldf(cp + 4) : 0.0f;
            if (y < 9000.0f) break;
            f32 v;
            if (y < 11000.0f) {
                v = (y - 9000.0f) / 2000.0f;
                if (v == 0.0f) break;
            } else {
                v = 1.0f;
            }
            JAIBasic_startSoundVec_l(self, 0x1064, self + 0x1FA8, 0, 0, 0, 4);
            if (ld(self + 0x1FA8) != 0) JAISound_setVolume_l(ld(self + 0x1FA8), v, 0, 0);
            break;
        }
        }
    }
    processHeartGaugeSound_l(self);
    if (ld(self + 0x20D0) == 0) st16(self + 0x20D4, 0xFFFF);
}
VERIFY(0x02024478, zeldaGFrameWork);

} // namespace JAIZelBasic_6_cpp
