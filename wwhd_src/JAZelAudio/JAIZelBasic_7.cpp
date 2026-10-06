/* JAIZelBasic (WWHD), part 7: monsSeInit .. heartGaugeOn (0202573C..02027D4B).
 * Ported from the GameCube decompilation where it has
 * bodies, else written from the WWHD code; verified against cking.rpx. See JAIZelBasic.cpp. */
#include "bindings.h"

namespace JAIZelBasic_7_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

static inline void initLevObjSE_l(u32 b) { gabi::call(0x02021290, b); }
static inline void bgmStart_l(u32 b, u32 id, u32 fade, u32 keep) { gabi::call(0x0202204C, b, id, fade, keep); }
static inline void bgmStop_l(u32 b, u32 fade, u32 keep) { gabi::call(0x02021F28, b, fade, keep); }
static inline s32 checkBgmPlaying_l(u32 b) { return gabi::call<s32>(0x02021E30, b); }
static inline s32 checkSeqIDDemoPlaying_l(u32 b, u32 id) { return gabi::call<s32>(0x020217A4, b, id); }
static inline s32 checkDayTime_l(u32 b) { return gabi::call<s32>(0x02023474, b); }
static inline s32 checkEventBit_l(u32 b, u32 f) { return gabi::call<s32>(0x0201E480, b, f); }
static inline u32 expandSceneBgmNum_l(u32 b, u32 n) { return gabi::call<u32>(0x02023608, b, n); }
static inline void sceneBgmStart_l(u32 b) { gabi::call(0x020234A4, b); }
static inline void menuOut_l(u32 b) { gabi::call(0x0201DCD4, b); }
static inline s32 isDemo_l(u32 b) { return gabi::call<s32>(0x0201E5CC, b); }
static inline void zeldaGFrameWork_l(u32 b) { gabi::call(0x02024478, b); }
static inline void hdStreamVolume_l(u32 b, u32 fade, f32 v) { gabi::call(0x02021B10, b, fade, v); }
static inline void bgmMute_l(u32 b, u32 h, u32 bgm, u32 set, u32 fade) { gabi::call(0x0201DD18, b, h, bgm, set, fade); }
static inline u32 getParamSeqPlayTrackMax_l() { return gabi::call<u32>(0x0280470C); }
static inline u32 SequenceMgr_getPlayTrackInfo_l(u32 i) { return gabi::call<u32>(0x0280AD2C, i); }
static inline s32 isDungeonItem_l(u32 off, u32 n) { return gabi::call<s32>(0x025B9100, ld(0x101F84DC) + off, n); }
/* JAudio wave banks (JAInter::BankWave / JAIBasic): status, load, unload helpers */
static inline s32 getWaveLoadStatus_l(u32 w) { return gabi::call<s32>(0x028024CC, w); }
static inline void loadWave_l(u32 w) { gabi::call(0x02802440, w, 0); }
static inline void unloadWave_l(u32 w) {
    gabi::call(0x02812E70, w, 0);
    gabi::call(0x0280203C, w, 0);
    gabi::call(0x02802028, w, (u32)-1);
}
static inline void JAISound_setTempoProportion_l(u32 s, f32 v, u32 fade) { gabi::call(0x0280C320, s, fade, v); }

/* 0202573C JAIZelBasic::monsSeInit() */
void monsSeInit(u32 self) {
    WWHD_FUNC(0x0202573C, void, self);
    for (u32 k = 0; k < 30; k++) {
        st(self + 0x1FBC + k * 8, (u32)-1);
        st(self + 0x1FC0 + k * 8, 0);
    }
}
VERIFY(0x0202573C, monsSeInit);

/* 02025760 JAIZelBasic::initSe() */
void initSe(u32 self) {
    WWHD_FUNC(0x02025760, void, self);
    st(ld(self), 0);
    st(ld(self) + 4, 0);
    st(ld(self) + 8, 0);
    st(self + 0x264, 0);
    for (u32 i = 0; i < 32; i++) {
        st(self + 0xE4 + i * 4, 0);
        st(self + 0x164 + i * 4, 0);
        st(self + 0x1E4 + i * 4, 0);
    }
    for (u32 i = 0; i < 4; i++) {
        st(self + 0x20B0 + i * 4, 0);
        st(self + 0x20C0 + i * 4, 0);
    }
    st(self + 0x1E40, 0);
    st8(self + 0x271, 0);
    st16(self + 0x20D4, 0xFFFF);
    st(self + 0x1FB8, 0);
    st(self + 0x1F34, 0);
    st8(self + 0x26B, 0);
    st(self + 0x20E4, 0);
    st(self + 0x1FB4, 0);
    st(self + 0x288, 0);
    st8(self + 0x269, 0);
    st(self + 0x27C, 0);
    st(self + 0x1F40, 0);
    st(self + 0x1F3C, 0);
    st8(self + 0x20AD, 0);
    st(self + 0x284, 0);
    st(self + 0x1F08, 0);
    st8(self + 0x26C, 0);
    st8(self + 0x20AC, 0);
    st(self + 0x1FA8, 0);
    st(self + 0x1FA4, 0);
    st8(self + 0x30, 0);
    st(self + 0x280, 0);
    st(self + 0x1F38, 0);
    st(self + 0x20D0, 0);
    st(self + 0x1BF0, 0);
    monsSeInit(self);
    initLevObjSE_l(self);
    gabi::call(0x02804508, 2); /* JAudio global setting (2) */
    st8(self + 0xCA, 0);
}
VERIFY(0x02025760, initSe);

/* 0202587C JAIZelBasic::kuroboVoicePlay(u32 id, Vec* pos, s8 reverb) */
void kuroboVoicePlay(u32 self, u32 id, u32 pos, u32 reverb) {
    WWHD_FUNC(0x0202587C, void, self, id, pos, reverb);
    u32 c = ld8(self + 0x20AD);
    if (c >= 4) return;
    st8(self + 0x20AD, c + 1);
    gabi::call(0x02802DFC, self, 4, pos); /* JAIBasic: stop the object's sounds of category 4 */
    u32 i;
    for (i = 0; i < 4; i++)
        if (ld(self + 0x20C0 + i * 4) == 0) break;
    if (i == 4) return;
    u32 h = self + 0x20C0 + i * 4;
    JAIBasic_startSoundVec_l(self, id, h, pos, 0, 0, 4);
    if (ld(h) != 0) JAISound_setPortData_l(ld(h), 9, reverb & 0xFFFF);
}
VERIFY(0x0202587C, kuroboVoicePlay);

/* 02025940 JAIZelBasic::mbossBgmNearByProcess(f32 dist) */
void mbossBgmNearByProcess(u32 self, f32 dist) {
    WWHD_FUNC(0x02025940, void, self, dist);
    if (ld(self + 0x7C) == 0) return;
    if (!(dist > ldf(0x1018DC78))) { /* JAIZelParam::ENEMY_NEARBY_DIST */
        s32 d1 = (s8)ld8(self + 0xD1);
        st(self + 0xD4, 0);
        if (d1 != 0) return;
        JAISound_setTrackVolume_l(ld(self + 0x7C), ld8(0x1018DCB5), 1.0f, ld(0x1018DC7C));
        st8(self + 0xD1, 1);
    } else if ((s8)ld8(self + 0xD1) == 1) {
        u32 t = ld(0x1018DC80);
        st8(self + 0xD1, 0);
        st(self + 0xD4, t);
    }
}
VERIFY(0x02025940, mbossBgmNearByProcess);

/* 020259E8 JAIZelBasic::enemyNearBy() */
void enemyNearBy(u32 self) {
    WWHD_FUNC(0x020259E8, void, self);
    if (ld(self + 0x78) != 0) {
        u32 n = ld(self + 0x88);
        bool go = true;
        if (n == 0x80000014) {
            if ((s8)ld8(self + 0xDB) != -1) {
                go = false;
            } else {
                bgmStart_l(self, ld8(self + 0xDC) != 0 ? 0x80000112 : 0x80000111, 0, 0);
                if (ld(self + 0x78) == 0) go = false;
                else n = ld(self + 0x88);
            }
        }
        if (go && n == 0x80000015 && (s8)ld8(self + 0xDB) == -1) bgmStart_l(self, 0x80000115, 0, 0);
    }
    st8(self + 0xDB, ld8(0x1018DCB4)); /* JAIZelParam::COMMON_BATTLE_FO_DELAY */
}
VERIFY(0x020259E8, enemyNearBy);

/* 02025AB8 JAIZelBasic::getMapInfoFxline(u32) (virtual) */
u32 getMapInfoFxline(u32 self, u32 n) {
    WWHD_FUNC(0x02025AB8, u32, self, n);
    return n & 0xFFFF;
}
VERIFY(0x02025AB8, getMapInfoFxline);

/* 02025AC0 JAIZelBasic::checkLinkOnSea() */
s32 checkLinkOnSea(u32 self) {
    WWHD_FUNC(0x02025AC0, s32, self);
    if ((s32)ld(self + 0x294) == 0x12 && (ld8(self + 0x2A8) == 0 || ld8(self + 0x28D) != 0)) return 1;
    return 0;
}
VERIFY(0x02025AC0, checkLinkOnSea);

/* 02025AF4 JAIZelBasic::checkLinkOnBoardSea() */
s32 checkLinkOnBoardSea(u32 self) {
    WWHD_FUNC(0x02025AF4, s32, self);
    if (checkLinkOnSea(self) == 1 && ld8(self + 0x57) != 0) return 1;
    return 0;
}
VERIFY(0x02025AF4, checkLinkOnBoardSea);

/* 02025B44 JAIZelBasic::JAIZelBasic() (HD size 0x21F4) */
u32 JAIZelBasic_ct(u32 self) {
    WWHD_FUNC(0x02025B44, u32, self);
    if (self == 0) {
        self = gabi::call<u32>(0x0273AD10, 0x21F4); /* operator new */
        if (self == 0) return 0;
    }
    gabi::call(0x02802564, self); /* JAIBasic::JAIBasic */
    st8(self + 0x3C, 0);
    st8(self + 0x56, 0);
    stf(self + 0x4C, 0.0f);
    st8(self + 0x57, 0);
    st8(self + 0x50, 0);
    st8(self + 0x30, 0);
    st8(self + 0x53, 0);
    st8(self + 0x52, 0);
    st8(self + 0x31, 0);
    st8(self + 0x55, 0);
    st8(self + 0x42, 0);
    st(self + 0x38, 0);
    st8(self + 0x3E, 0);
    st(self + 0x44, 0);
    st8(self + 0x43, 0);
    st8(self + 0x54, 0);
    st(self + 0x2C, 0x10003A08);
    st(self + 0x34, 0);
    st8(self + 0x3F, 0);
    st8(self + 0x3D, 0);
    st8(self + 0x51, 0);
    st(self + 0x48, 0);
    st16(self + 0x40, 0);
    gabi::call(0x028F521C, self + 0x58, 0xC); /* Vec ctor (zero) */
    gabi::call(0x028F521C, self + 0x64, 0xC);
    stf(self + 0x90, 0.0f);
    st(self + 0x84, 0);
    st8(self + 0xC4, 0);
    st8(self + 0x76, 0);
    stf(self + 0x98, 0.0f);
    stf(self + 0xA4, 0.0f);
    st8(self + 0xCB, 0);
    stf(self + 0x94, 0.0f);
    st8(self + 0x72, 0);
    st8(self + 0xCC, 0);
    stf(self + 0xA0, 0.0f);
    st8(self + 0x71, 0);
    st8(self + 0xC6, 0);
    stf(self + 0xB4, 0.0f);
    st8(self + 0xCF, 0);
    st8(self + 0x75, 0);
    stf(self + 0xA8, 0.0f);
    st(self + 0x7C, 0);
    stf(self + 0xB0, 0.0f);
    st8(self + 0xC5, 0);
    st8(self + 0xCA, 0);
    st8(self + 0xD0, 0);
    st8(self + 0x74, 0);
    st(self + 0x80, 0);
    st8(self + 0xC9, 0);
    st8(self + 0xD1, 0);
    stf(self + 0xB8, 0.0f);
    st8(self + 0x73, 0);
    st(self + 0x8C, 0);
    st(self + 0xD4, 0);
    st8(self + 0xCE, 0);
    st8(self + 0xC7, 0);
    st8(self + 0xCD, 0);
    st8(self + 0xD8, 0);
    stf(self + 0xAC, 0.0f);
    st(self + 0x88, 0);
    stf(self + 0x9C, 0.0f);
    st8(self + 0xD9, 0);
    st8(self + 0x70, 0);
    st(self + 0xC0, 0);
    st(self + 0x78, 0);
    st8(self + 0xDA, 0);
    stf(self + 0xBC, 0.0f);
    st8(self + 0xC8, 0);
    st8(self + 0xDB, 0);
    st8(self + 0xDC, 0);
    st8(self + 0xDD, 0);
    st8(self + 0xDE, 0);
    st(self + 0xE0, 0);
    st(self + 0x264, 0);
    st8(self + 0x268, 0);
    st8(self + 0x269, 0);
    st8(self + 0x26A, 0);
    st8(self + 0x273, 0);
    st8(self + 0x29C, 0);
    st(self + 0x298, 0);
    st8(self + 0x272, 0);
    st8(self + 0x274, 0);
    st8(self + 0x29D, 0);
    st8(self + 0x28E, 0);
    st(self + 0x284, 0);
    st8(self + 0x2A3, 0);
    st8(self + 0x29F, 0);
    st8(self + 0x26F, 0);
    st8(self + 0x26B, 0);
    st(self + 0x280, 0);
    st8(self + 0x276, 0);
    st8(self + 0x277, 0);
    st8(self + 0x2A4, 0);
    st8(self + 0x26C, 0);
    st8(self + 0x270, 0);
    st8(self + 0x2A0, 0);
    st8(self + 0x2A5, 0);
    st(self + 0x288, 0);
    st(self + 0x290, 0);
    st8(self + 0x28D, 0);
    st8(self + 0x2A6, 0);
    st8(self + 0x2A2, 0);
    st8(self + 0x29E, 0);
    st8(self + 0x26E, 0);
    st8(self + 0x2A7, 0);
    st(self + 0x27C, 0);
    st8(self + 0x275, 0);
    st8(self + 0x278, 0);
    st8(self + 0x2A8, 0);
    st8(self + 0x26D, 0);
    st8(self + 0x271, 0);
    st8(self + 0x2A1, 0);
    st8(self + 0x2A9, 0);
    st8(self + 0x28C, 0);
    st(self + 0x294, 0);
    st8(self + 0x2AA, 0);
    st(self + 0x2AC, 0);
    st(self + 0x2B0, 0);
    st(self + 0x15EC, 0);
    st(self + 0x1BF0, 0);
    gabi::call(0x028F521C, self + 0x1BF4, 0xC);
    st8(self + 0x1FAC, 0);
    st(self + 0x1E40, 0);
    st(self + 0x1FA8, 0);
    st(self + 0x1FA4, 0);
    st(self + 0x1FB4, 0);
    st(self + 0x1F3C, 0);
    st8(self + 0x20AD, 0);
    st(self + 0x1F08, 0);
    st16(self + 0x20D4, 0);
    st(self + 0x20E4, 0);
    st8(self + 0x1E44, 0);
    st(self + 0x1F30, 0);
    st(self + 0x1FB8, 0);
    st(self + 0x1F34, 0);
    st(self + 0x1F38, 0);
    st8(self + 0x20AC, 0);
    st(self + 0x1F40, 0);
    stf(self + 0x1FB0, 0.0f);
    st8(self + 0x1FAD, 0);
    st(self + 0x20D0, 0);
    gabi::call(0x028EFFD0, self + 0x20E8, 0x10, 8, 0x02029C44); /* __construct_array: 16 allocation records */
    st(self + 0x2168, 0);
    {
        u32 rnd = self + 0x216C; /* JMath::TRandom_enough_ (constructed in place) */
        if (rnd == 0) rnd = gabi::call<u32>(0x0273AD10, 0x88);
        if (rnd != 0) gabi::call(0x027ED730, rnd, 0); /* setSeed(0) */
    }
    st(0x101FFC78, self); /* JAIZelBasic::zel_basic = this */
    stf(self + 0x4C, 1.0f);
    stf(self + 0x60, 10000000.0f);
    stf(self + 0x68, 10000000.0f);
    stf(self + 0x5C, 10000000.0f);
    stf(self + 0x58, 10000000.0f);
    stf(self + 0x6C, 10000000.0f);
    stf(self + 0x64, 10000000.0f);
    stf(self + 0xA8, 1.0f);
    stf(self + 0x90, 1.0f);
    stf(self + 0xA4, 1.0f);
    stf(self + 0xB8, 1.0f);
    st8(self + 0x73, 0);
    st(self + 0x88, (u32)-1);
    st(self + 0x80, 0);
    st(self + 0x84, (u32)-1);
    st8(self + 0x57, 0);
    st8(self + 0x31, 0);
    st8(self + 0x3E, 0);
    st(self + 0x48, 0);
    st(self + 0x44, 0);
    st8(self + 0x3D, 0);
    stf(self + 0xAC, 1.0f);
    st8(self + 0x30, 0);
    stf(self + 0x94, 1.0f);
    st8(self + 0x3F, 0);
    stf(self + 0xA0, 1.0f);
    st8(self + 0x54, 0);
    st16(self + 0x40, 0xC00);
    st8(self + 0x2A3, 0);
    st(self + 0xE0, 0);
    st8(self + 0x276, 0);
    stf(self + 0xB4, 1.0f);
    st8(self + 0x75, 0);
    stf(self + 0x9C, 1.0f);
    st8(self + 0x29F, 0);
    st8(self + 0x53, 0);
    st8(self + 0x2A2, 0);
    st8(self + 0x56, 0);
    st8(self + 0xCB, 0);
    st(self + 0x34, 0);
    st8(self + 0xC5, 0);
    st8(self + 0x271, 0);
    st(self + 0x294, (u32)-1);
    st8(self + 0x3C, 0);
    stf(self + 0xBC, 1.0f);
    st8(self + 0x272, 0);
    st8(self + 0x2A5, 0);
    st8(self + 0xCA, 0);
    st(self + 0xC0, (u32)-1);
    st8(self + 0x2A6, 0);
    st8(self + 0x26A, 0);
    stf(self + 0xB0, 1.0f);
    st8(self + 0x51, 0);
    st(self + 0x290, (u32)-1);
    st8(self + 0x274, 0);
    st8(self + 0x2A1, 0);
    st8(self + 0x55, 0);
    st8(self + 0x268, 0);
    st(self + 0x298, 0);
    st8(self + 0x52, 0);
    st(self + 0x8C, (u32)-1);
    st8(self + 0x273, 0);
    st8(self + 0x2A4, 0);
    st8(self + 0x2A0, 0);
    st8(self + 0x42, 0);
    st8(self + 0x2A7, 0);
    st8(self + 0x269, 0);
    stf(self + 0x98, 1.0f);
    st8(self + 0x29E, 0);
    st8(self + 0x2AA, 0);
    st8(self + 0x74, 0);
    st8(self + 0xC8, 0);
    st8(self + 0x1FAC, 0);
    st8(self + 0x26B, 0);
    st8(self + 0x29C, 0);
    st8(self + 0x26C, 0);
    st8(self + 0xC6, 0);
    st8(self + 0x29D, 0);
    st8(self + 0xC4, 0);
    st8(self + 0xCC, 0);
    st8(self + 0xD0, 0);
    st8(self + 0xD1, 0);
    st(self + 0xD4, 0);
    st8(self + 0xD8, 0);
    st8(self + 0xD9, 0);
    st8(self + 0xDA, 0);
    st8(self + 0x28C, 0);
    st(self + 0x38, 0);
    st8(self + 0x50, 0);
    st8(self + 0x270, 0);
    st8(self + 0xCE, 0);
    st8(self + 0x278, 0);
    stf(self + 0x1FB0, 0.0f);
    st8(self + 0x26F, 0);
    st8(self + 0xDD, 0);
    st8(self + 0xC9, 0);
    st8(self + 0x26E, 0);
    st8(self + 0xC7, 0);
    st8(self + 0x2A9, 0);
    st8(self + 0xCD, 0);
    st8(self + 0x275, 0);
    st8(self + 0xDC, 0);
    st8(self + 0xCF, 0);
    st8(self + 0x72, 0);
    st8(self + 0x277, 0);
    st8(self + 0xDB, 0xFF);
    st8(self + 0x71, 0);
    st(self + 0x1F30, 0);
    st8(self + 0x1FAD, 0);
    st8(self + 0x28E, 0);
    st8(self + 0x43, 0);
    st8(self + 0x1E44, 0);
    st8(self + 0x26D, 0);
    st8(self + 0x28D, 0);
    st8(self + 0x70, 0);
    st8(self + 0xDE, 0);
    st8(self + 0x76, 0);
    st8(self + 0x2A8, 0);
    return self;
}
VERIFY(0x02025B44, JAIZelBasic_ct);

/* 02026044 JAIZelBasic::init(JKRSolidHeap*, u32): JAIGlobalParameter setup from JAIZelParam
 * (1018DC14..), the HD path strings (SafeString objects from 0202EA8C/98/A4), driver and interface */
void init(u32 self, u32 heap, u32 size) {
    WWHD_FUNC(0x02026044, void, self, heap, size);
    gabi::call(0x02804454, ld(0x1018DC14));
    gabi::call(0x0280446C, ld(0x1018DC18));
    gabi::call(0x02804460, ld(0x1018DC1C));
    gabi::call(0x02804484, ld(0x1018DC24));
    gabi::call(0x02804490, ld(0x1018DC28));
    gabi::call(0x02804594, ld(0x1018DC20));
    gabi::call(0x0280449C, ld(0x1018DC2C));
    gabi::call(0x028044A8, ld(0x1018DC30));
    gabi::call(0x028045B0, ld(0x1018DC34));
    gabi::call(0x028045BC, ld(0x1018DC38));
    gabi::call(0x028044B4, ld(0x1018DC40));
    gabi::call(0x02804680, ld(0x1018DC3C));
    gabi::call(0x028044E4, ldf(0x1018DC44));
    gabi::call(0x028044F0, ldf(0x1018DC48));
    gabi::call(0x028044FC, ldf(0x1018DC4C));
    gabi::call(0x028045C8, (u32)ld8(0x1018DC96));
    gabi::call(0x028045FC, ldf(0x1018DC50));
    gabi::call(0x02804608, ldf(0x1018DC54));
    gabi::call(0x02804588, (u32)ld16(0x1018DC94));
    gabi::call(0x028044CC, 1.0f);
    gabi::call(0x028044CC, ldf(0x100039CC)); /* 0.707 */
    gabi::call(0x028044D8, 1.0f);
    gabi::call(0x02804644, 0);
    {
        u32 s = gabi::call<u32>(0x0202EA8C);
        gabi::call_ptr(ld(ld(s + 4) + 0x14), s);
        gabi::call(0x02804614, ld(s));
    }
    {
        u32 s = gabi::call<u32>(0x0202EA98);
        gabi::call_ptr(ld(ld(s + 4) + 0x14), s);
        gabi::call(0x02804620, ld(s));
    }
    {
        u32 s = gabi::call<u32>(0x0202EAA4);
        gabi::call_ptr(ld(ld(s + 4) + 0x14), s);
        gabi::call(0x0280462C, ld(s));
    }
    gabi::call(0x02804638, ld(0x1018DC60));
    gabi::call(0x02804650, ld(0x1018DC64));
    gabi::call(0x02804674, 1);
    gabi::call(0x02804668, ld(0x1018DC58));
    gabi::call(0x0280465C, ld(0x1018DC5C));
    gabi::call(0x02804598, 1);
    gabi::call(0x02803614, self, heap, size, 1); /* JAIBasic::initDriver */
    gabi::call(0x02803BC4, self, 1);             /* JAIBasic::initInterface */
    st(self + 0x78, 0);
    st(self + 0x7C, 0);
    st(self + 0x80, 0);
    initSe(self);
    for (u32 c = 0; c < 8; c++) JAIBasic_setSeCategoryVolume_l(self, c, ld8(0x1018DC97 + c));
    stf(self + 0xBC, 1.0f);
    stf(self + 0x90, 1.0f);
    stf(self + 0xAC, 1.0f);
    st8(self + 0x2AA, 1);
    stf(self + 0xA8, 1.0f);
    stf(self + 0x9C, 1.0f);
    st(self + 0x2AC, 0x57);
    st(self + 0x2B0, 0x1018E314);
    stf(self + 0x94, 1.0f);
    stf(self + 0xA4, 1.0f);
    stf(self + 0x98, 1.0f);
    stf(self + 0xA0, 1.0f);
}
VERIFY(0x02026044, init);

/* 0202636C JAIZelBasic::gframeProcess() */
void gframeProcess(u32 self) {
    WWHD_FUNC(0x0202636C, void, self);
    u32 h = ld(0x104B5018); /* JAInter::SeMgr::seHandle */
    if (h == 0) return;
    if (ld8(h + 1) >= 4) zeldaGFrameWork_l(self);
    gabi::call(0x02802F20, self); /* JAIBasic::processFrameWork */
}
VERIFY(0x0202636C, gframeProcess);

/* 020263C0 JAIZelBasic::resetProcess() */
void resetProcess(u32 self) {
    WWHD_FUNC(0x020263C0, void, self);
    for (u32 i = 0; i < 32; i++) {
        u32 s = ld(self + 0xE4 + i * 4);
        if (s == 0) continue;
        JAISound_stop_l(s, 1);
        st(self + 0xE4 + i * 4, 0);
        st(self + 0x1E4 + i * 4, 0);
        st(self + 0x164 + i * 4, 0);
    }
    menuOut_l(self);
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s != 0) JAISound_setSeqInterVolume_l(s, 6, 0.0f, 1);
    }
    u32 h = ld(0x104B5018);
    if (h != 0) JAISound_setSeqInterVolume_l(h, 6, 0.0f, 1);
    gabi::call(0x027632E4, hdsnd_player_l(ld(0x1018EC64)), 0); /* HD stream player: stop */
    st(self + 0x298, 0);
    st8(self + 0x29E, 0);
    st8(self + 0x29D, 0);
}
VERIFY(0x020263C0, resetProcess);

/* 020264F8 JAIZelBasic::sceneChange(u32 bgm, u32 wave1, u32 wave2, s32 keepBgm) */
void sceneChange(u32 self, u32 bgm, u32 wave1, u32 wave2, s32 keep) {
    WWHD_FUNC(0x020264F8, void, self, bgm, wave1, wave2, keep);
    if ((s32)ld(self + 0x294) == 0x12 && ld8(self + 0x43) != 0 && checkDayTime_l(self) == 1) bgm = 0;
    u32 cur = ld(self + 0x298);
    u32 n88 = ld(self + 0x88);
    int same = -1; /* 1: keep the playing bgm (29E = 0), 0: start a new one (29E = 1) */
    if (bgm != cur) {
        if (n88 == 0x80000042 && checkBgmPlaying_l(self) == 1) {
            if (bgm > 0x80000133) same = 0;
            else if (bgm >= 0x80000130 || bgm == 0x80000042) same = 1;
            else same = 0;
        } else if (bgm == n88 && checkBgmPlaying_l(self) == 1) {
            same = 1;
        }
    } else if (bgm == n88 && checkBgmPlaying_l(self) != 0) {
        same = 1;
    }
    if (same < 0) same = checkSeqIDDemoPlaying_l(self, bgm) == 1 ? 1 : 0;
    u32 strm = ld(self + 0x80);
    st(self + 0x298, bgm);
    st8(self + 0x29E, same ? 0 : 1);
    if (strm != 0) JAISound_stop_l(strm, ld(0x1018DC74));
    st(self + 0x80, 0);
    st(self + 0x8C, (u32)-1);
    gabi::call(0x02030C10);
    if (ld8(self + 0x29E) != 0) {
        if (keep == 0) {
            u32 n84 = ld(self + 0x84);
            if ((n84 == 0x80000032 || n84 == 0x80000030) && ld(self + 0x7C) != 0) bgmStop_l(self, ld(0x1018DC74), 1);
            else bgmStop_l(self, ld(0x1018DC74), 0);
        }
        st8(self + 0x2A1, 0);
    }
    bool w2same;
    if (wave1 != ld8(self + 0x2A2) && wave1 != 0) {
        st8(self + 0x2A3, ld8(self + 0x2A2));
        u32 w2 = ld8(self + 0x2A5);
        st8(self + 0x2A2, wave1);
        st8(self + 0x2A4, 1);
        w2same = wave2 == w2;
    } else {
        u32 w2 = ld8(self + 0x2A5);
        st8(self + 0x2A4, 0);
        w2same = wave2 == w2;
    }
    if (!w2same && wave2 != 0) {
    } else if (ld8(self + 0x2A4) == 0) {
        st8(self + 0x2A7, 0);
        return;
    }
    u32 old = ld8(self + 0x2A5);
    st8(self + 0x2A5, wave2);
    st8(self + 0x2A6, old);
    st8(self + 0x2A7, 1);
}
VERIFY(0x020264F8, sceneChange);

/* 0202677C JAIZelBasic::setScene(s32 scene, s32 room, s32 keepBgm, s32 layer) */
void setScene(u32 self, s32 sc, s32 room, s32 keep, s32 layer) {
    WWHD_FUNC(0x0202677C, void, self, sc, room, keep, layer);
    if (sc >= 0x79) return;
    if (ld8(self + 0x29C) != 0) return;
    st8(self + 0xCD, 0);
    st8(self + 0xDD, 0);
    if (sc == 0x12) {
        /* the sea: the island room's bgm (m_isle_info at 1018E60C) */
        u32 r = room > 0 ? (room & 0xFF) : 0;
        st8(self + 0x2A8, r);
        u32 bgm;
        if (keep == 1) {
            bgm = 0;
        } else {
            int sel = 2; /* 0: none, 1: special id, 2: the table */
            u32 special = 0;
            if (r == 0x2C) {
                if (layer == 0xA) {
                    sel = 0;
                } else if (checkEventBit_l(self, 0x3510) == 0) {
                    sel = 1;
                    special = 0x80000038;
                } else {
                    bool a;
                    if (checkEventBit_l(self, 1) == 1) {
                        s32 e = checkEventBit_l(self, 0x101);
                        if (e == 0) {
                            sel = 1;
                            special = 0x8000000E;
                        }
                        a = e == 1;
                    } else {
                        a = checkEventBit_l(self, 0x101) == 1;
                    }
                    if (sel == 2) {
                        s32 e = checkEventBit_l(self, 0xE20);
                        if (a && e == 0) sel = 0;
                        else if (e == 1) {
                            sel = 1;
                            special = 0x80000055;
                        }
                    }
                }
            } else if (r == 0xB) {
                if (checkEventBit_l(self, 0x2E01) == 0) sel = 0;
            } else if (r == 0xD) {
                if (layer == 8 || layer == 0xA || layer == 0xB) sel = 0;
            } else if (r == 0xE) {
                if (layer == 2 || layer == 3) {
                    st8(self + 0xCD, 1);
                    sceneChange(self, 0, 0, 0, keep);
                    st(self + 0x290, sc);
                    st8(self + 0x29D, 0);
                    st8(self + 0x29C, 1);
                    st8(self + 0x2A9, layer);
                    return;
                }
            }
            if (sel == 0) bgm = 0;
            else if (sel == 1) bgm = special;
            else bgm = expandSceneBgmNum_l(self, ld16(0x1018E60C + r * 4));
        }
        sceneChange(self, bgm, 0, 0, keep);
    } else {
        /* m_scene_info at 1018E428: {u16 bgm, u8 wave1, u8 wave2} */
        st8(self + 0x2A8, 0);
        const u32 tab = 0x1018E428;
        const u32 e = tab + sc * 4;
        int sel = 2;     /* 0: bgm 0, 1: special, 2: the table, 3: special with an expanded id */
        u32 special = 0;
        bool dungeon = false;
        u32 memOff = 0, altBgm = 0;
        bool set274 = false;
        switch (sc) {
        case 0x10:
            if (checkEventBit_l(self, 0x280) == 0) {
                sel = 1;
                special = 0x80000016;
            }
            break;
        case 0x13:
            if (checkEventBit_l(self, 0x801) == 0) sel = 0;
            break;
        case 2: dungeon = true; memOff = 0x40C; altBgm = tab + 4; set274 = true; break;
        case 7: dungeon = true; memOff = 0x430; altBgm = tab + 0xC; break;
        case 0x25: dungeon = true; memOff = 0x454; altBgm = tab + 0x28; break;
        case 0x18:
        case 0x2D:
            if (ld8(self + 0x31) == 0) break;
            memOff = sc == 0x18 ? 0x49C : 0x478;
            if (isDungeonItem_l(memOff, 5) != 0) st8(self + 0xDD, 1);
            if ((u32)(layer - 8) <= 3) sel = 0;
            else if (isDungeonItem_l(memOff, 3) != 0) sel = 0;
            break;
        case 0x59:
            if ((u32)(layer - 8) <= 3) {
                sel = 0;
                break;
            }
            /* fall through */
        case 0x36:
            if (checkEventBit_l(self, 0x2D04) == 0) {
                sel = 1;
                special = 0x80000034;
            }
            break;
        case 0x35:
        case 0x27:
        case 0x29:
        case 0x16:
        case 0x2C:
            if ((u32)(layer - 8) <= 3) sel = 0;
            break;
        case 0xB:
            if (ld8(self + 0x53) == 0) {
                if (layer == 9) sel = 0;
            } else if (checkEventBit_l(self, 0x2A20) == 0) {
                sel = 1;
                special = 0x80000044;
            }
            break;
        case 0x20:
            if (checkEventBit_l(self, 0x2110) == 0 && checkEventBit_l(self, 0xA02) == 1) {
                sel = 1;
                special = 0x8000003D;
            }
            break;
        }
        if (dungeon && ld8(self + 0x31) != 0) {
            if (isDungeonItem_l(memOff, 5) != 0) st8(self + 0xDD, 1);
            if (isDungeonItem_l(memOff, 3) != 0) {
                u32 bgm = expandSceneBgmNum_l(self, ld16(altBgm));
                if (set274) st8(self + 0x274, 1);
                sceneChange(self, bgm, ld8(e + 2), ld8(e + 3), keep);
                sel = 4;
            }
        }
        if (sel == 0) sceneChange(self, 0, ld8(e + 2), ld8(e + 3), keep);
        else if (sel == 1) sceneChange(self, special, ld8(e + 2), ld8(e + 3), keep);
        else if (sel == 2) {
            u32 bgm = expandSceneBgmNum_l(self, ld16(e));
            sceneChange(self, bgm, ld8(e + 2), ld8(e + 3), keep);
        }
    }
    st8(self + 0x29D, 0);
    st8(self + 0x2A9, layer);
    st8(self + 0x29C, 1);
    st(self + 0x290, sc);
}
VERIFY(0x0202677C, setScene);

/* 02026E04 JAIZelBasic::load1stDynamicWave(): unloads the previous scene waves and loads the new
 * first set (wave pair tables at 1018E3C4 (1st) and 1018E2EC (2nd)), then resets the bgm state */
void load1stDynamicWave(u32 self) {
    WWHD_FUNC(0x02026E04, void, self);
    st(self + 0x294, ld(self + 0x290));
    if (ld8(self + 0x2A1) != 0 || ld8(self + 0x2A4) != 0 || ld8(self + 0x2A7) != 0) {
        u32 w = ld8(self + 0x2A6);
        u32 b = ld8(0x1018E2EC + w * 2 + 1), a = ld8(0x1018E2EC + w * 2);
        if (b != 0) unloadWave_l(b);
        if (a != 0) unloadWave_l(a);
        if (ld8(self + 0x2A1) != 0 || ld8(self + 0x2A4) != 0) {
            u32 w1 = ld8(self + 0x2A3);
            u32 b1 = ld8(0x1018E3C4 + w1 * 2 + 1), a1 = ld8(0x1018E3C4 + w1 * 2);
            if (b1 != 0) unloadWave_l(b1);
            if (a1 != 0) unloadWave_l(a1);
            if (ld8(self + 0x2A1) != 0) {
                bool load29F = true;
                u32 x = ld8(self + 0x2A0);
                if (x != 0) {
                    gabi::call(0x02812E70, x, 0);
                    gabi::call(0x0280203C, (u32)ld8(self + 0x2A0), 0);
                    gabi::call(0x02802028, (u32)ld8(self + 0x2A0), (u32)-1);
                    load29F = ld8(self + 0x2A1) != 0;
                }
                if (load29F) {
                    u32 y = ld8(self + 0x29F);
                    if (y != 0) loadWave_l(y);
                }
            }
            if (ld8(self + 0x2A4) != 0) {
                u32 w2 = ld8(self + 0x2A2);
                u32 c = ld8(0x1018E3C4 + w2 * 2), d = ld8(0x1018E3C4 + w2 * 2 + 1);
                if (c != 0) loadWave_l(c);
                if (d != 0) loadWave_l(d);
            }
        }
    }
    st8(self + 0x29C, 0);
    st8(self + 0x1FAC, 0);
    u32 n84 = ld(self + 0x84);
    stf(self + 0x98, 1.0f);
    stf(self + 0x90, 1.0f);
    stf(self + 0x94, 1.0f);
    if (n84 != 0x80000032 && n84 != 0x80000030 && n84 != 0x8000001C) stf(self + 0x9C, 1.0f);
    u32 n88 = ld(self + 0x88);
    u32 main = ld(self + 0x78);
    if (n88 != 0x80000039) stf(self + 0xA0, 1.0f);
    f32 f94 = ldf(self + 0x94), f9c = ldf(self + 0x9C);
    stf(self + 0xBC, 1.0f);
    stf(self + 0xB4, 1.0f);
    stf(self + 0xAC, 1.0f);
    stf(self + 0xA4, 1.0f);
    stf(self + 0xB0, 1.0f);
    stf(self + 0xA8, 1.0f);
    stf(self + 0xB8, 1.0f);
    if (main == 0 || ld(self + 0x88) != 0x8000001D) st8(self + 0x42, 0);
    f32 v = mul(mul(f94, f9c), 1.0f);
    st8(self + 0xC9, 0);
    hdStreamVolume_l(self, 1, v);
    u32 sc = ld(self + 0x294);
    st8(self + 0x278, 0);
    st(self + 0x38, 0);
    st8(self + 0x28E, 1);
    st8(self + 0x72, 0);
    st(self + 0xE0, 0);
    st8(self + 0x70, 0);
    st8(self + 0x1E44, 0);
    st8(self + 0x274, 0);
    st8(self + 0x29D, 0);
    st8(self + 0xCE, 0);
    if (sc == 0x76) {
        gabi::call(0x02030E90, 1);
        gabi::call(0x02030E64);
    } else if (sc == 0x77) {
        gabi::call(0x02030E90, 2);
        gabi::call(0x02030E64);
    } else {
        gabi::call(0x02030E38);
    }
    for (u32 k = 0; k < 30; k++) {
        u32 s = ld(self + 0x1FC0 + k * 8);
        if (s != 0) JAISound_stop_l(s, 0);
    }
    monsSeInit(self);
}
VERIFY(0x02026E04, load1stDynamicWave);

/* 02027168 JAIZelBasic::bgmStreamPrepare(u32 id) (named check1stDynamicWave by the matcher) */
void bgmStreamPrepare(u32 self, u32 id) {
    WWHD_FUNC(0x02027168, void, self, id);
    if (ld8(self + 0x73) != 0) return;
    gabi::call(0x02802880, self, id, self + 0x80, 0, 0, 0, 4); /* JAIBasic: prepare a stream */
    if (id == 0xC0000009) st8(self + 0xCE, 1);
    st(self + 0x8C, id);
}
VERIFY(0x02027168, bgmStreamPrepare);

/* 020271E4 JAIZelBasic::load2ndDynamicWave(): loads the second wave set, prepares the dungeon stream */
void load2ndDynamicWave(u32 self) {
    WWHD_FUNC(0x020271E4, void, self);
    u32 sc = ld(self + 0x294);
    u32 id = 0;
    u32 strm = 0;
    switch (sc) {
    case 2: strm = 0xC0000002; break;
    case 7: strm = 0xC0000007; break;
    case 0x25: strm = 0xC0000009; break;
    case 0x18: strm = 0xC000000F; break;
    case 0x2D: strm = 0xC0000034; break;
    }
    if (strm != 0 && ld8(self + 0xDD) == 0) id = strm;
    if (ld8(self + 0x2A7) != 0) {
        u32 w = ld8(self + 0x2A5);
        u32 a = ld8(0x1018E2EC + w * 2), b = ld8(0x1018E2EC + w * 2 + 1);
        if (a != 0) loadWave_l(a);
        if (b != 0) loadWave_l(b);
    }
    if (id != 0) bgmStreamPrepare(self, id);
}
VERIFY(0x020271E4, load2ndDynamicWave);

/* 0202736C JAIZelBasic::resetRecover() */
void resetRecover(u32 self) {
    WWHD_FUNC(0x0202736C, void, self);
    u32 h = ld(0x104B5018);
    if (h != 0) JAISound_setSeqInterVolume_l(h, 6, 1.0f, 0);
    for (u32 i = 0; i < getParamSeqPlayTrackMax_l(); i++) {
        u32 info = SequenceMgr_getPlayTrackInfo_l(i);
        if (info == 0) continue;
        u32 s = ld(info + 0x48);
        if (s == 0) continue;
        if (s == ld(0x104B5018)) continue;
        JAISound_stop_l(s, 0);
    }
    gabi::call(0x027632E4, hdsnd_player_l(ld(0x1018EC64)), 0);
    setScene(self, 0, 0, 0, -1);
    load1stDynamicWave(self);
    sceneBgmStart_l(self);
    load2ndDynamicWave(self);
    stf(self + 0x9C, 1.0f);
    stf(self + 0xAC, 1.0f);
    stf(self + 0x98, 1.0f);
    stf(self + 0xA0, 1.0f);
    stf(self + 0x94, 1.0f);
    stf(self + 0xB0, 1.0f);
    stf(self + 0xBC, 1.0f);
    stf(self + 0xA4, 1.0f);
    stf(self + 0x90, 1.0f);
    stf(self + 0xA8, 1.0f);
    stf(self + 0xB4, 1.0f);
    for (u32 c = 0; c < 8; c++) JAIBasic_setSeCategoryVolume_l(self, c, ld8(0x1018DC97 + c));
    st8(self + 0x276, 0);
    st8(self + 0x271, 0);
    st8(self + 0xCF, 0);
    st8(self + 0x277, 0);
}
VERIFY(0x0202736C, resetRecover);

/* 02027558 JAIZelBasic::getCameraInfo(Vec* pos, MtxP mtx, u32 idx). HD: a position of
 * (1e7, 1e7, 1e7) (or none) marks "no camera"; the previous camera position is kept at +0x58 */
void getCameraInfo(u32 self, u32 pos, u32 mtx, u32 idx) {
    WWHD_FUNC(0x02027558, void, self, pos, mtx, idx);
    bool none = false;
    u32 e = idx * 12;
    u32 cam = ld(self);
    if (pos == 0) none = true;
    else if (ldf(pos) == 1e7f && ldf(pos + 4) == 1e7f && ldf(pos + 8) == 1e7f) none = true;
    u32 old = ld(cam + e);
    u32 v = self + 0x58;
    if (old != 0) {
        st(v, ld(old));
        st(v + 4, ld(old + 4));
        st(v + 8, ld(old + 8));
    } else {
        stf(v + 8, 1e7f);
        stf(v + 4, 1e7f);
        stf(v, 1e7f);
    }
    st(ld(self) + e, none ? v : pos);
    st(ld(self) + e + 4, v);
    st(ld(self) + e + 8, mtx);
    if (none) {
        st8(self + 0x268, 10);
        stf(v, 1e7f);
        stf(v + 4, 1e7f);
        stf(v + 8, 1e7f);
        return;
    }
    u32 c = ld8(self + 0x268);
    if (c != 0) st8(self + 0x268, c - 1);
}
VERIFY(0x02027558, getCameraInfo);

/* 02027668 JAIZelBasic::getCameraMapInfo(u32) (unnamed by the matcher) */
void getCameraMapInfo(u32 self, u32 v) {
    WWHD_FUNC(0x02027668, void, self, v);
    st(self + 0x288, v);
}
VERIFY(0x02027668, getCameraMapInfo);

/* 02027670 JAIZelBasic::setCameraGroupInfo(u8) */
void setCameraGroupInfo(u32 self, u32 g) {
    WWHD_FUNC(0x02027670, void, self, g);
    if (isDemo_l(self) == 1) return;
    if (ld8(self + 0xCD) != 0) return;
    if (ld8(self + 0xCF) != 0) return;
    u32 cur = ld8(self + 0x28C);
    if (g == cur) return;
    if (g & 0x80) {
        u32 cur2 = ld8(self + 0x28C);
        st8(self + 0x2A8, g & 0x3F);
        if (cur2 == 0) {
            /* outer sea -> island edge */
            setScene(self, 0x12, g & 0x3F, 1, -1);
            load1stDynamicWave(self);
            load2ndDynamicWave(self);
            st8(self + 0x28C, g);
            st8(self + 0x28E, 0);
            st8(self + 0x29D, 1);
            return;
        }
    } else if (g == 0 || (g & 0x40) != 0) {
        if (cur & 0x80) st8(self + 0x2A8, 0);
    }
    st8(self + 0x28C, g);
}
VERIFY(0x02027670, setCameraGroupInfo);

/* 02027754 JAIZelBasic::setCameraPolygonPos(Vec*) (unnamed by the matcher) */
void setCameraPolygonPos(u32 self, u32 pos) {
    WWHD_FUNC(0x02027754, void, self, pos);
    if (pos == 0) return;
    st(self + 0x64, ld(pos));
    st(self + 0x68, ld(pos + 4));
    st(self + 0x6C, ld(pos + 8));
}
VERIFY(0x02027754, setCameraPolygonPos);

/* 02027778 JAIZelBasic::setLinkGroupInfo(u8) */
void setLinkGroupInfo(u32 self, u32 g) {
    WWHD_FUNC(0x02027778, void, self, g);
    if ((s32)ld(self + 0x294) != 0x12) return;
    if (ld8(self + 0x2A8) == 0) return;
    if (g == ld8(self + 0x28D)) return;
    st8(self + 0x28D, g);
}
VERIFY(0x02027778, setLinkGroupInfo);

/* 020277A4 JAIZelBasic::setEventBit(void*) (unnamed by the matcher) */
void setEventBit(u32 self, u32 p) {
    WWHD_FUNC(0x020277A4, void, self, p);
    st(self + 0x34, p);
}
VERIFY(0x020277A4, setEventBit);

/* 020277AC JAIZelBasic::spotNameToId(char*): spot_dir_name[120] at 1018E6D4 */
s32 spotNameToId(u32 self, u32 name) {
    WWHD_FUNC(0x020277AC, s32, self, name);
    if (name == 0) return 0;
    u32 i;
    for (i = 0; i < 120; i++) {
        u32 s = ld(0x1018E6D4 + i * 4);
        u32 k = 0;
        u8 a, b;
        do {
            a = ld8(name + k);
            b = ld8(s + k);
            k++;
        } while (a == b && a != 0);
        if (a == b) break;
    }
    if (i == 120) return 0;
    return i + 1;
}
VERIFY(0x020277AC, spotNameToId);

/* 02027814 JAIZelBasic::setSceneName(char*, s32 room, s32 layer) */
void setSceneName(u32 self, u32 name, s32 room, s32 layer) {
    WWHD_FUNC(0x02027814, void, self, name, room, layer);
    s32 id = spotNameToId(self, name);
    if (id == 0x75) {
        for (u32 i = 0; i < 32; i++) {
            u32 s = ld(self + 0xE4 + i * 4);
            if (s != 0) {
                JAISound_stop_l(s, 1);
                st(self + 0xE4 + i * 4, 0);
            }
            st(self + 0x164 + i * 4, 0);
            st(self + 0x1E4 + i * 4, 0);
            st(self + 0x264, 0);
        }
        menuOut_l(self);
        st8(self + 0x76, 1);
        st8(self + 0xCF, 0);
        setScene(self, id, room, 0, layer);
    } else {
        st8(self + 0x76, 0);
        setScene(self, id, room, 0, layer);
    }
}
VERIFY(0x02027814, setSceneName);

/* 020278E8 (HD-only, unnamed): all 20 static waves (list at 10003844) loaded */
s32 checkStaticWaves(u32 self) {
    WWHD_FUNC(0x020278E8, s32, self);
    for (u32 i = 0; i < 20; i++)
        if (getWaveLoadStatus_l(ld(0x10003844 + i * 4)) != 2) return 0;
    return 1;
}
VERIFY(0x020278E8, checkStaticWaves);

/* 0202796C JAIZelBasic::check1stDynamicWave() (probably; unnamed by the matcher): the load state of
 * the scene's first wave pair (high byte / low byte, 0 = loaded); 1 while static waves or the
 * scene's HD stream data are still loading */
u32 check1stDynamicWave(u32 self) {
    WWHD_FUNC(0x0202796C, u32, self);
    u32 w = ld8(self + 0x2A2);
    u32 a = ld8(0x1018E3C4 + w * 2), b = ld8(0x1018E3C4 + w * 2 + 1);
    u32 sa = 0, sb = 0;
    if (a != 0) sa = 2 - getWaveLoadStatus_l(a);
    if (b != 0) sb = 2 - getWaveLoadStatus_l(b);
    u32 r = (sa << 8) + sb;
    u32 sc = ld(self + 0x294);
    if ((s32)sc != 0x75) {
        if (checkStaticWaves(self) == 0) return 1;
        sc = ld(self + 0x294);
    }
    if (sc == 0x76) {
        if (gabi::call<s32>(0x02030F1C, 1) == 0) return 1;
    } else if (sc == 0x77) {
        if (gabi::call<s32>(0x02030F1C, 2) == 0) return 1;
    }
    return r;
}
VERIFY(0x0202796C, check1stDynamicWave);

/* 02027A7C JAIZelBasic::loadStaticWaves() (unnamed by the matcher) */
void loadStaticWaves(u32 self) {
    WWHD_FUNC(0x02027A7C, void, self);
    if (checkStaticWaves(self) != 0) return;
    for (u32 i = 0; i < 20; i++) loadWave_l(ld(0x10003844 + i * 4));
}
VERIFY(0x02027A7C, loadStaticWaves);

/* 02027AE4 JAIZelBasic::checkFirstWaves() */
s32 checkFirstWaves(u32 self) {
    WWHD_FUNC(0x02027AE4, s32, self);
    return 2 - getWaveLoadStatus_l(2);
}
VERIFY(0x02027AE4, checkFirstWaves);

/* 02027B0C JAIZelBasic::setLinkHp(s32 hp, s32 max): HD speeds the battle bgms up at low health */
void setLinkHp(u32 self, s32 hp, s32 max) {
    WWHD_FUNC(0x02027B0C, void, self, hp, max);
    st(self + 0x44, hp);
    st(self + 0x48, max);
    f32 r;
    if (max != 0) {
        r = (f32)hp / (f32)max;
        if (r > 1.0f) r = 1.0f;
    } else {
        r = 1.0f;
    }
    u32 sub = ld(self + 0x7C);
    stf(self + 0x4C, r);
    if (sub == 0) return;
    u32 n = ld(self + 0x84);
    if (n == 0x8000001C) JAISound_setTempoProportion_l(sub, gabi::fmadds(ldf(0x100039D8), 1.0f - r, 1.0f), 0);
    else if (n > 0x8000001A) return;
    else if (n >= 0x80000019) JAISound_setTempoProportion_l(sub, gabi::fmadds(ldf(0x100039D4), 1.0f - r, 1.0f), 0);
    else if (n == 0x80000004) JAISound_setTempoProportion_l(sub, gabi::fmadds(ldf(0x100039D0), 1.0f - r, 1.0f), 0);
}
VERIFY(0x02027B0C, setLinkHp);

/* 02027C50 JAIZelBasic::setLinkSwordType(s32, s32) (unnamed by the matcher) */
void setLinkSwordType(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02027C50, void, self, a, b);
    st8(self + 0x51, a);
    st8(self + 0x52, b);
    st8(self + 0x55, ((a & 0xFF) != 0 && (b & 0xFF) == 2 && ld8(self + 0x53) != 0 && ld8(self + 0x54) == 2) ? 1 : 0);
}
VERIFY(0x02027C50, setLinkSwordType);

/* 02027C9C JAIZelBasic::setLinkShieldType(s32, s32) (unnamed by the matcher) */
void setLinkShieldType(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02027C9C, void, self, a, b);
    u32 sw = ld8(self + 0x51);
    st8(self + 0x53, a);
    st8(self + 0x54, b);
    st8(self + 0x55, (sw != 0 && ld8(self + 0x52) == 2 && ld8(self + 0x53) != 0 && (b & 0xFF) == 2) ? 1 : 0);
}
VERIFY(0x02027C9C, setLinkShieldType);

/* 02027CEC JAIZelBasic::setLinkBootsType(s32) (unnamed by the matcher) */
void setLinkBootsType(u32 self, u32 a) {
    WWHD_FUNC(0x02027CEC, void, self, a);
    st8(self + 0x56, a);
}
VERIFY(0x02027CEC, setLinkBootsType);

/* 02027CF4 JAIZelBasic::setLinkOnBoard(s32) */
void setLinkOnBoard(u32 self, u32 a) {
    WWHD_FUNC(0x02027CF4, void, self, a);
    u32 sub = ld(self + 0x7C);
    st8(self + 0x57, a);
    if (sub == 0) return;
    if (ld(self + 0x84) != 0x8000001C) return;
    bgmMute_l(self, self + 0x7C, 0x8000001C, (a & 0xFF) == 1 ? 0 : 1, 0xA);
}
VERIFY(0x02027CF4, setLinkOnBoard);

/* 02027D40 JAIZelBasic::heartGaugeOn() (unnamed by the matcher) */
void heartGaugeOn(u32 self) {
    WWHD_FUNC(0x02027D40, void, self);
    st8(self + 0x50, 2);
}
VERIFY(0x02027D40, heartGaugeOn);

} // namespace JAIZelBasic_7_cpp
