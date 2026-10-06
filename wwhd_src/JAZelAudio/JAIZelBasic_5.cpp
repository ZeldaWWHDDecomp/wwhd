/* JAIZelBasic (WWHD), part 5: checkPlayingStreamBgmFlag .. processHeartGaugeSound (020233C8..02024477).
 * Ported from the GameCube decompilation where it has
 * bodies, else written from the WWHD code; verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_5_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline s32 checkEventBit_l(u32 b, u32 f) { return gabi::call<s32>(0x0201E480, b, f); }
static inline s32 isDemo_l(u32 b) { return gabi::call<s32>(0x0201E5CC, b); }
static inline void stopBattleBgm_l(u32 b) { gabi::call(0x0201D84C, b); }
static inline void bgmStart_l(u32 b, u32 id, u32 fade, u32 keep) { gabi::call(0x0202204C, b, id, fade, keep); }
static inline f32 mainVol_A4(u32 self, f32 a4) {
    f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0x9C));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, a4);
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, ldf(self + 0xAC));
    return mul(v, ldf(self + 0xBC));
}

/* 020233C8 (named checkPlayingStreamBgmFlag): HD stream check of an id through the sound manager */
s32 checkPlayingStreamBgmFlag(u32 self, u32 id) {
    WWHD_FUNC(0x020233C8, s32, self, id);
    return hdsnd_check_l(hdsnd_player_l(ld(0x1018EC64)), id, 0, 0, 0);
}
VERIFY(0x020233C8, checkPlayingStreamBgmFlag);

/* 02023410 JAIZelBasic::checkSeaBgmID() */
u32 checkSeaBgmID(u32 self) {
    WWHD_FUNC(0x02023410, u32, self);
    if (checkEventBit_l(self, 0xA02) == 1 && checkEventBit_l(self, 0x3920) == 0) return 0x8000003C;
    return 0x8000002E;
}
VERIFY(0x02023410, checkSeaBgmID);

/* 02023474 JAIZelBasic::checkDayTime(): 6:00 .. 19:59 */
s32 checkDayTime(u32 self) {
    WWHD_FUNC(0x02023474, s32, self);
    return (u32)ld8(self + 0x3C) - 6 < 14;
}
VERIFY(0x02023474, checkDayTime);

/* 0202348C JAIZelBasic::checkPlayingMainBgmFlag() (unnamed by the matcher) */
u32 checkPlayingMainBgmFlag(u32 self) {
    WWHD_FUNC(0x0202348C, u32, self);
    u32 m = ld(self + 0x78);
    if (m == 0) return (u32)-1;
    return JAISound_getID_l(m);
}
VERIFY(0x0202348C, checkPlayingMainBgmFlag);

/* 020234A4 JAIZelBasic::sceneBgmStart() */
void sceneBgmStart(u32 self) {
    WWHD_FUNC(0x020234A4, void, self);
    st8(self + 0x31, 1);
    bool night = false;
    if ((s32)ld(self + 0x294) == 0x12 && checkDayTime(self) == 0) {
        st8(self + 0x29E, 0);
        night = true;
    }
    if (ld8(self + 0x42) == 1) return;
    if (checkPlayingMainBgmFlag(self) == 0x8000000A) return;
    u32 skip = ld8(self + 0x29E);
    st8(self + 0x275, 0);
    st8(self + 0x29D, 1);
    st8(self + 0xDA, 0);
    if (skip == 0) {
        u32 sc = ld(self + 0x294);
        bool ok;
        if (sc >= 0x19) ok = sc == 0x19 || (sc >= 0x2E && sc <= 0x34) || sc == 0x38 || (sc >= 0x69 && sc <= 0x6A);
        else if (sc < 5) return;
        else ok = sc <= 6 || sc == 0xA || (sc >= 0x14 && sc <= 0x15);
        if (!ok) {
            if (sc >= 0x19 || sc < 0x14) return;
            return; /* 0x16..0x18: return without the flag reset */
        }
    }
    u32 bgm = ld(self + 0x298);
    if (bgm != 0 && bgm != 0x80000000 && !night) bgmStart_l(self, bgm, 0, 0);
    st8(self + 0x29E, 0);
    st8(self + 0x43, 0);
}
VERIFY(0x020234A4, sceneBgmStart);

/* 02023608 JAIZelBasic::expandSceneBgmNum(u32) */
u32 expandSceneBgmNum(u32 self, u32 n) {
    WWHD_FUNC(0x02023608, u32, self, n);
    if ((n & 0xF000) == 0x8000) return (n & 0x7FFF) | 0xC0000000;
    return (n & 0x7FFF) | 0x80000000;
}
VERIFY(0x02023608, expandSceneBgmNum);

/* 02023628 JAIZelBasic::mainBgmStopOnly(u32 fade) */
void mainBgmStopOnly(u32 self, u32 fade) {
    WWHD_FUNC(0x02023628, void, self, fade);
    u32 m = ld(self + 0x78);
    if (m != 0) JAISound_stop_l(m, fade);
    st(self + 0x78, 0);
    st(self + 0x88, (u32)-1);
}
VERIFY(0x02023628, mainBgmStopOnly);

/* 02023670 JAIZelBasic::checkOnOuterSea(f32* dist): 0 inside the island's radius .. 4 open sea,
 * -1 not on the sea map (isle areas {x, y, z, r} at 1018E8B4, 7x7 grid of 100000 units) */
s32 checkOnOuterSea(u32 self, u32 out) {
    WWHD_FUNC(0x02023670, s32, self, out);
    if (out == 0) return -1;
    stf(out, 0.0f);
    if ((s32)ld(self + 0x294) != 0x12) return -1;
    u32 room = ld8(self + 0x2A8);
    if (room == 0) return 4;
    u32 a = 0x1018E8B4 + room * 16;
    f32 ax = ldf(a);
    f32 az = ldf(a + 8);
    f32 ar = ldf(a + 0xC);
    if (ax > 50000.0f) return 4;
    s32 m = (s32)room - 1;
    s32 q = m / 7;
    s32 col = m - q * 7;
    f32 fz = (f32)(q - 3);
    f32 fx = (f32)(col - 3);
    u32 cam = ld(ld(self));
    f32 cz = ldf(cam + 8);
    f32 cx = ldf(cam);
    f32 rz = gabi::fnmsubs(fz, 100000.0f, cz);
    f32 rx = gabi::fnmsubs(fx, 100000.0f, cx);
    rz = az - rz;
    rx = ax - rx;
    f32 d2 = gabi::fmadds(rx, rx, rz * rz);
    stf(out, d2);
    f32 d = sqrtf_l(d2);
    stf(out, d);
    if (!(d > ar)) return 0;
    if (!(d > ar + 2000.0f)) return 1;
    if (!(d > ar + 3000.0f)) return 2;
    if (!(d > ar + 4000.0f)) return 3;
    return 4;
}
VERIFY(0x02023670, checkOnOuterSea);

/* 0202389C JAIZelBasic::startIsleBgm() (HD: keeps the sub bgm) */
void startIsleBgm(u32 self) {
    WWHD_FUNC(0x0202389C, void, self);
    if (checkDayTime(self) == 0) return;
    u32 room = ld8(self + 0x2A8);
    if (room == 0x2C && checkEventBit_l(self, 0xE20) == 1) {
        bgmStart_l(self, 0x80000055, 0, 1);
        return;
    }
    /* m_isle_info[room].bgmNum (room read before the event check, as GHS keeps it in a register) */
    bgmStart_l(self, expandSceneBgmNum(self, ld16(0x1018E60C + room * 4)), 0, 1);
}
VERIFY(0x0202389C, startIsleBgm);

/* 02023930 JAIZelBasic::processMorningToNormal() */
void processMorningToNormal(u32 self) {
    WWHD_FUNC(0x02023930, void, self);
    if (ld(self + 0x88) != 0x8000001D) {
        st8(self + 0x42, 0);
        return;
    }
    if (ld(self + 0x78) != 0) return;
    stf(self + 0xA4, 1.0f);
    gabi::Local<be<f32>> tmp;
    if (checkOnOuterSea(self, gabi::ea(tmp.get())) >= 2) {
        bgmStart_l(self, 0x8000002E, 0, 1);
        st8(self + 0x42, 0);
    } else {
        startIsleBgm(self);
        st8(self + 0x42, 0);
    }
}
VERIFY(0x02023930, processMorningToNormal);

/* 020239C8 JAIZelBasic::processTime() */
void processTime(u32 self) {
    WWHD_FUNC(0x020239C8, void, self);
    u16 t = (u16)(((ld8(self + 0x3C) << 8) & 0xFF00) + ld8(self + 0x3D));
    if ((s32)ld(self + 0x294) == 0x12 && ld8(self + 0x29D) == 1) {
        u32 old = ld16(self + 0x40);
        if (old < 0x1400) {
            if (t >= 0x1400) {
                mainBgmStopOnly(self, ld(0x1018DC90)); /* JAIZelParam::JAI_ZEL_NIGHT_FADEOUT_TIME */
                st16(self + 0x40, t);
                processMorningToNormal(self);
                return;
            }
            if (old < 0x53B && t >= 0x53B) {
                bgmStart_l(self, 0x8000001D, 0, 1);
                st8(self + 0x42, 1);
            }
        }
    }
    st16(self + 0x40, t);
    processMorningToNormal(self);
}
VERIFY(0x020239C8, processTime);

/* 02023A94 JAIZelBasic::changeSeaBgm() */
void changeSeaBgm(u32 self) {
    WWHD_FUNC(0x02023A94, void, self);
    if ((s32)ld(self + 0x294) != 0x12) return;
    if (ld8(self + 0x2A8) == 0) return;
    u32 n88 = ld(self + 0x88); /* kept in a register across the calls below */
    if (n88 == 0x8000000A) return;
    if (checkEventBit_l(self, 0xF80) == 0) return;
    if (ld(ld(self)) == 0) return;
    if (ld8(self + 0x29D) == 0) return;
    if (ld8(self + 0x268) != 0) return;
    if (ld8(self + 0x70) != 0) return;
    if (ld8(self + 0x72) != 0) return;
    if (checkDayTime(self) == 0 && checkSeaBgmID(self) != 0x8000003C) return;
    if (ld8(self + 0x42) == 1) return;
    if (isDemo_l(self) == 1) return;
    if (ld(self + 0x7C) != 0 && ld(self + 0x84) == 0x8000002B) return;
    if (ld(self + 0x78) != 0 && n88 == 0x80000053) return;
    if (ld8(self + 0xCD) != 0) return;
    gabi::Local<be<f32>> tmp;
    u32 tp = gabi::ea(tmp.get());
    stf(tp, 0.0f);
    u32 r = checkOnOuterSea(self, tp);
    u32 room = ld8(self + 0x2A8);
    u32 n = ld(self + 0x88);
    f32 ar = ldf(0x1018E8C0 + room * 16);
    if (n == 0x8000002E || n == 0x8000003C) {
        if (r == 1) {
            stf(self + 0xA4, 0.0f);
            startIsleBgm(self);
        } else if (r == 2) {
            f32 v = (ldf(tp) - (ar + 2000.0f)) / 1000.0f;
            u32 m = ld(self + 0x78);
            stf(self + 0xA4, v);
            if (m != 0) JAISound_setVolume_l(m, mainVol_A4(self, v), 1, 0);
        }
        return;
    }
    if (r < 1) return;
    if (r == 1) {
        f32 v = ((ar + 2000.0f) - ldf(tp)) / 2000.0f;
        u32 m = ld(self + 0x78);
        stf(self + 0xA4, v);
        if (m != 0) JAISound_setVolume_l(m, mainVol_A4(self, v), 1, 0);
    } else if (r < 4) {
        u32 m = ld(self + 0x78);
        stf(self + 0xA4, 0.0f);
        if (m != 0) JAISound_setVolume_l(m, mainVol_A4(self, 0.0f), 0, 0);
    } else if (r == 4) {
        stf(self + 0xA4, 1.0f);
        bgmStart_l(self, checkSeaBgmID(self), 0x5A, 1);
    }
}
VERIFY(0x02023A94, changeSeaBgm);

/* 02023E0C JAIZelBasic::bgmBattleGFrame() */
void bgmBattleGFrame(u32 self) {
    WWHD_FUNC(0x02023E0C, void, self);
    u32 n = ld(self + 0x84);
    if (n != 0x80000004 && n != 0x8000001C) return;
    u32 d0 = ld8(self + 0xD0);
    if (d0 != ld8(0x1018DCB4) && d0 == 0) {
        u32 m = ld(self + 0x78);
        stf(self + 0x9C, 1.0f);
        if (m != 0) {
            f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
            v = mul(v, ldf(self + 0x98));
            v = mul(v, ldf(self + 0xA0));
            v = mul(v, ldf(self + 0xA4));
            v = mul(v, ldf(self + 0xA8));
            v = mul(v, ldf(self + 0xAC));
            v = mul(v, ldf(self + 0xBC));
            JAISound_setVolume_l(m, v, 0x2D, 0);
        }
        stopBattleBgm_l(self);
        st(self + 0xD4, 0);
        st8(self + 0xD1, 0);
        return;
    }
    if (n == 0x80000004) {
        u32 c = ld(self + 0xD4);
        bool fade = true;
        u32 d8;
        if (c > 1) {
            d8 = ld8(self + 0xD8);
            st(self + 0xD4, c - 1);
        } else {
            if (c == 1) {
                u32 sub = ld(self + 0x7C);
                if (sub != 0) {
                    JAISound_setTrackVolume_l(sub, ld8(0x1018DCAF), 0.0f, ld(0x1018DC80));
                    JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB0), 0.0f, ld(0x1018DC80));
                }
                st(self + 0xD4, 0);
            }
            d8 = ld8(self + 0xD8);
        }
        if (d8 != 0) {
            d0 = ld8(self + 0xD0);
            st8(self + 0xD8, d8 - 1);
            fade = false;
        }
        if (fade) {
            u32 sub = ld(self + 0x7C);
            if (sub != 0 && ld(self + 0x84) == 0x80000004)
                JAISound_setTrackVolume_l(sub, ld8(0x1018DCB1), 0.0f, ld(0x1018DC88));
            d0 = ld8(self + 0xD0);
        }
    }
    if (d0 != 0) st8(self + 0xD0, d0 - 1);
}
VERIFY(0x02023E0C, bgmBattleGFrame);

/* 02024018 JAIZelBasic::mbossBgmMuteProcess() */
void mbossBgmMuteProcess(u32 self) {
    WWHD_FUNC(0x02024018, void, self);
    u32 n = ld(self + 0x84);
    if (n != 0x80000019 && n != 0x8000001A) return;
    u32 c = ld(self + 0xD4);
    u32 sub;
    if (c > 1) {
        sub = ld(self + 0x7C);
        st(self + 0xD4, c - 1);
    } else {
        if (c == 1) {
            u32 s = ld(self + 0x7C);
            if (s != 0 && (n == 0x80000019 || n == 0x8000001A))
                JAISound_setTrackVolume_l(s, ld8(0x1018DCB5), 0.0f, ld(0x1018DC80));
            st(self + 0xD4, 0);
        }
        sub = ld(self + 0x7C);
    }
    if (sub != 0) {
        if (ld8(self + 0xD9) == 0) JAISound_setTrackVolume_l(sub, ld8(0x1018DCB7), 0.0f, ld(0x1018DC8C));
        else JAISound_setTrackVolume_l(sub, ld8(0x1018DCB7), 1.0f, ld(0x1018DC8C));
    }
    u32 d8 = ld8(self + 0xD8);
    if (d8 != 0) {
        st8(self + 0xD8, d8 - 1);
        return;
    }
    u32 s = ld(self + 0x7C);
    if (s != 0) JAISound_setTrackVolume_l(s, ld8(0x1018DCB6), 0.0f, ld(0x1018DC88));
}
VERIFY(0x02024018, mbossBgmMuteProcess);

/* 02024190 JAIZelBasic::enemyNearByGFrame() */
void enemyNearByGFrame(u32 self) {
    WWHD_FUNC(0x02024190, void, self);
    if ((s32)ld(self + 0x294) != 9) return;
    s32 c = (s8)ld8(self + 0xDB);
    if (c < 0) return;
    if (c > 0) {
        st8(self + 0xDB, c - 1);
        return;
    }
    if (ld(self + 0x78) != 0) {
        u32 n = ld(self + 0x88);
        bool go = true;
        if (n == 0x80000014) {
            bgmStart_l(self, ld8(self + 0xDC) != 0 ? 0x80000110 : 0x80000014, 0, 0);
            if (ld(self + 0x78) == 0) go = false;
            else n = ld(self + 0x88);
        }
        if (go && n == 0x80000015) bgmStart_l(self, 0x80000015, 0, 0);
    }
    st8(self + 0xDB, 0xFF);
}
VERIFY(0x02024190, enemyNearByGFrame);

/* 02024278 JAIZelBasic::stSkyCloistersProcess() */
void stSkyCloistersProcess(u32 self) {
    WWHD_FUNC(0x02024278, void, self);
    u32 c = ld8(self + 0xC7);
    if (c == 0) return;
    bool expire = c == 1;
    if (c > 1) {
        c = (c - 1) & 0xFF;
        st8(self + 0xC7, c);
        expire = c == 1;
    }
    if (expire) {
        if (ld(self + 0x88) == 0x80000028) bgmStart_l(self, 0x80000028, 1, 0);
        st8(self + 0xC7, 0);
    }
    JAIZelBasic_seStart_l(self, 0x105A, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
}
VERIFY(0x02024278, stSkyCloistersProcess);

/* 02024334 JAIZelBasic::processHeartGaugeSound(). HD: the alarm can be switched off (an HD option
 * object of the save, *101F84DC + 0x12C0) */
void processHeartGaugeSound(u32 self) {
    WWHD_FUNC(0x02024334, void, self);
    u32 play = gabi::ea(dComIfGp_get());
    if (ld8(play + 0x5292) != 0) return; /* dComIfGp_event_runCheck() */
    u32 c = ld8(self + 0x50);
    if (c == 0) return;
    if (ld(self + 0x44) != 0 && ld8(self + 0x276) == 0) {
        u32 opt = gabi::call<u32>(0x027200D0, ld(0x101F84DC) + 0x12C0);
        if (gabi::call<s32>(0x0271FC5C, opt) == 0) {
            s32 h = ld(self + 0x44);
            if (h <= 2) JAIZelBasic_seStart_l(self, 0xD2, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            else if (h <= 4) JAIZelBasic_seStart_l(self, 0xD1, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
            else if (h <= 6) JAIZelBasic_seStart_l(self, 0xD0, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        }
        c = ld8(self + 0x50);
    }
    st8(self + 0x50, c - 1);
}
VERIFY(0x02024334, processHeartGaugeSound);

} // namespace JAIZelBasic_5_cpp
