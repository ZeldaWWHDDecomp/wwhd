/* JAIZelBasic (WWHD), part 2: setLevObjSE and seStart (0201E74C..0202128F).
 * Both are "Nonmatching" stubs in the GameCube
 * decompilation (zeldaret/tww src/JAZelAudio/JAIZelBasic.cpp), so they are written from the WWHD
 * code and verified against cking.rpx. See JAIZelBasic.cpp for the unit and jaizel_basic_local.h
 * for the layout. */
#include "bindings.h"

namespace JAIZelBasic_2_cpp {
#include "jaizel_local.h"
#include "jaizel_basic_local.h"

/* ---- calls into the unit's other functions (other parts / TUs) ---- */
static inline f32 calcPosVolume_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C360, b, pos, s); }
static inline f32 calcPosPanLR_l(u32 b, u32 pos) { return gabi::call<f32>(0x0201C0E8, b, pos); }
static inline f32 calcPosPanSR_l(u32 b, u32 pos, f32 s) { return gabi::call<f32>(0x0201C228, b, pos, s); }
static inline void kuroboMotionPlay_l(u32 b, u32 id, u32 pos, u32 info, u32 reverb) { gabi::call(0x0201D57C, b, id, pos, info, reverb); }
static inline s32 checkSePlaying_l(u32 b, u32 id) { return gabi::call<s32>(0x0201D62C, b, id); }
static inline s32 seLimitInstances_l(u32 b, u32 id, u32 pos, s32 prio, s32 cnt, f32 dist) { return gabi::call<s32>(0x0201D670, b, id, pos, prio, cnt, dist); }
static inline void stopBattleBgm_l(u32 b) { gabi::call(0x0201D84C, b); }
static inline void subBgmStop_l(u32 b) { gabi::call(0x0201D8BC, b); }
static inline void subBgmStopInner_l(u32 b) { gabi::call(0x0201D9C8, b); }
static inline void subBgmStart_l(u32 b, u32 id) { gabi::call(0x0201DE00, b, id); }
static inline s32 checkEventBit_l(u32 b, u32 f) { return gabi::call<s32>(0x0201E480, b, f); }
static inline void onEnemyDamage_l(u32 b) { gabi::call(0x0201E4AC, b); }
static inline void seStop_l(u32 b, u32 id, u32 fade) { gabi::call(0x0201E538, b, id, fade); }
static inline s32 isDemo_l(u32 b) { return gabi::call<s32>(0x0201E5CC, b); }
static inline void menuIn_l(u32 b) { gabi::call(0x0201E5E0, b); }
static inline void menuOut_l(u32 b) { gabi::call(0x0201DCD4, b); }
static inline void JAIBasic_deleteObject_l(u32 b, u32 pos) { gabi::call(0x02802E00, b, pos); }
static inline void JAISound_setPan_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280BA28, s, t, k, v); }
static inline void JAISound_setDolby_l(u32 s, f32 v, u32 t, u32 k) { gabi::call(0x0280C248, s, t, k, v); }

/* 0201E74C JAIZelBasic::setLevObjSE(u32 id, Vec* pos, s8 reverb): records volume/pan/surround of one
 * instance of a level-object sound (15 sounds x 20 instances at +0x2B4, 0x148 bytes each; count +0x15EC) */
void setLevObjSE(u32 self, u32 id, u32 pos, u32 reverb) {
    WWHD_FUNC(0x0201E74C, void, self, id, pos, reverb);
    u32 cam = ld(self);
    u32 mtx = ld(cam + 8);
    gabi::Local<Vec_l> vv;
    u32 v = gabi::ea(vv.get());
    if (pos == 0) {
        /* default (0, 0, -50) unless the camera has a position */
        st(v, ld(0x100038B0));
        st(v + 4, ld(0x100038B4));
        st(v + 8, ld(0x100038B8));
        u32 cp = ld(cam);
        if (cp != 0) {
            st(v, ld(cp));
            st(v + 4, ld(cp + 4));
            st(v + 8, ld(cp + 8));
        }
    } else {
        st(v, ld(pos));
        st(v + 8, ld(pos + 8));
        st(v + 4, ld(pos + 4));
    }
    if ((s32)id == 0x7009) {
        /* not above the camera */
        f32 cy = ldf(ld(cam) + 4);
        if (ldf(v + 4) > cy) stf(v + 4, cy);
    }
    if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
    f32 scale = 1.0f;
    if (id < 0x5053) {
        if (id == 0x105F) scale = 4.0f;
        else if (id < 0x3033) scale = 1.0f;
        else if (id <= 0x3034) scale = 3.0f;
        else if (id == 0x501E) scale = 3.0f;
        else if (id == 0x501F) scale = 4.0f;
    } else if (id >= 0x7009) {
        if (id == 0x7009) scale = 3.0f;
        else if (id == 0x701F) scale = 6.0f;
        else if (id == 0x7035) scale = 0.5f;
    } else if (id <= 0x5054) {
        scale = 4.0f;
    } else if (id == 0x6103) {
        scale = 0.5f;
    } else if (id == 0x612E) {
        scale = 0.4f;
    }
    f32 vol = calcPosVolume_l(self, v, scale);
    f32 lr = calcPosPanLR_l(self, v);
    f32 sr = calcPosPanSR_l(self, v, 1.0f);
    if (vol == 0.0f) return;
    u32 n = ld(self + 0x15EC);
    u32 i = 0;
    for (; i < n; i++)
        if (ld(self + 0x2B4 + i * 0x148) == id) break;
    u32 e = self + 0x2B4 + i * 0x148;
    if (i == n) {
        if ((s32)n == 0xF) return;
        st(e, id);
        st(self + 0x15EC, ld(self + 0x15EC) + 1);
    }
    s32 c = ld(e + 4);
    if (c == 0x14) return;
    u32 slot = e + c * 16 + 8;
    stf(slot, vol);
    stf(slot + 4, lr);
    stf(slot + 8, sr);
    st8(slot + 0xC, reverb);
    st(e + 4, ld(e + 4) + 1);
}
VERIFY(0x0201E74C, setLevObjSE);

/* main-bgm volume without the +0x9C factor (just set to 1.0) */
static inline f32 mainVol_no9C(u32 self) {
    f32 v = mul(ldf(self + 0x90), ldf(self + 0x94));
    v = mul(v, ldf(self + 0x98));
    v = mul(v, ldf(self + 0xA0));
    v = mul(v, ldf(self + 0xA4));
    v = mul(v, ldf(self + 0xA8));
    v = mul(v, ldf(self + 0xAC));
    return mul(v, ldf(self + 0xBC));
}

static inline f32 dist_of(u32 v) {
    f32 y = ldf(v + 4), x = ldf(v), z = ldf(v + 8);
    return sqrtf_l(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
}

/* 0201EBA0 JAIZelBasic::seStart(u32 id, Vec* pos, u32 info, s8 reverb, f32 pitch, f32 volume,
 * f32 pan, f32 dolby, u8 flag): returns the sound handle slot, or 0 */
u32 seStart(u32 self, u32 id, u32 pos, u32 info, u32 reverb, f32 pitch, f32 volume, f32 pan, f32 dolby, u32 flag) {
    WWHD_FUNC(0x0201EBA0, u32, self, id, pos, info, reverb, flag, pitch, volume, pan, dolby);
    if (pos != 0) ptr_check_l(pos);
    if (ld8(self + 0x271) == 1) return 0;
    bool kurobo = id - 0x587A < 3 || id - 0x592D < 3;
    if (!kurobo && (s32)ld(self + 0x294) == 0x2D) {
        if ((s32)id == 0x7051) return 0;
        kurobo = id == 0x383C || id == 0x5135 || (id >= 0x5930 && id <= 0x5934) || id == 0x5936;
    }
    if (kurobo) {
        kuroboMotionPlay_l(self, id, pos, info, reverb);
        return 0;
    }
    if (id - 0x486D < 3) return 0;
    if (ld(self + 0x88) == 0x80000012 && ((s32)id == 0x580E || (s32)id == 0x580D)) return 0;
    if (ld8(self + 0x276) != 0) {
        if (!(id - 0x690B < 9 || (id & 0xFFFFF000) == 0 || (s32)id == 0x593B || (s32)id == 0x4926 || (s32)id == 0x4925))
            return 0;
    }
    if (ld8(self + 0x277) != 0 && (id & 0xFFFFF000) != 0) {
        bool ok;
        if (id >= 0x2880) ok = id <= 0x2881 || id == 0x48A1 || (id >= 0x4901 && id <= 0x4902);
        else ok = id == 0x2066 || id == 0x2868;
        if (!ok) return 0;
    }
    /* one-shot sounds per scene and sounds that must not overlap */
    switch ((s32)id) {
    case 0x3814: {
        if (ld8(self + 0x269) == 1) return 0;
        s32 scene = ld(self + 0x294);
        st8(self + 0x269, 1);
        if (scene == 0x12 && ld8(self + 0x2A8) == 0x2C && ld8(self + 0x2A9) == 9 && ld(self + 0x38) < 30) return 0;
        break;
    }
    case 0x6847:
        if (ld8(self + 0x26C) == 1) return 0;
        st8(self + 0x26C, 1);
        break;
    case 0x693F:
        if (ld8(self + 0x26B) == 1) return 0;
        st8(self + 0x26B, 1);
        break;
    case 0x282C:
        if (ld8(self + 0x26A) != 0) return 0;
        st8(self + 0x26A, 4);
        break;
    case 0x6932:
    case 0x69E6:
        if (ld8(self + 0x272) != 0) return 0;
        st8(self + 0x272, 2);
        break;
    case 0x6806:
        if (ld8(self + 0x273) != 0) return 0;
        st8(self + 0x273, 2);
        break;
    case 0x6A42:
        if (ld8(self + 0x26D) != 0) return 0;
        st8(self + 0x26D, 0x14);
        break;
    case 0x6A43:
        if (ld8(self + 0x26E) != 0) return 0;
        st8(self + 0x26E, 0x14);
        break;
    case 0x6A44:
        if (ld8(self + 0x26F) != 0) return 0;
        st8(self + 0x26F, 0x14);
        break;
    case 0x6A45:
        if (ld8(self + 0x270) != 0) return 0;
        st8(self + 0x270, 0x14);
        break;
    case 0x6934:
        if (checkSePlaying_l(self, 0x6934) != 0) return 0;
        break;
    case 0x2889:
        if (checkSePlaying_l(self, 0x2889) != 0) return 0;
        break;
    case 0x7034:
        if ((s32)ld(self + 0x294) == 0x11) id = 0x706F;
        break;
    }
    /* instance limits: {distance, priority, count} */
    f32 hundred = 100.0f;
    int lim = 0; /* A..H = 1..8 */
    if (id < 0x5885) {
        if (id < 0x48DA) {
            if (id < 0x4879) {
                if (id == 0x2828) lim = 3;
                else if (id == 0x486E) lim = 1;
                else if (id >= 0x4875 && id <= 0x4876) lim = 5;
            } else if (id == 0x4879) lim = 5;
            else if (id >= 0x48B4 && id <= 0x48B6) lim = 7;
            else if (id == 0x48D6) lim = 8;
        } else if (id < 0x5801) {
            if (id == 0x48DA) lim = 8;
            else if (id >= 0x493C && id <= 0x493D) lim = 6;
            else if (id == 0x4944) lim = 6;
        } else if (id == 0x5801) lim = 2;
        else if (id == 0x587C) lim = 1;
        else if (id >= 0x587C && id <= 0x587E) lim = 5;
        else if (id == 0x5882) lim = 5;
    } else if (id < 0x6901) {
        if (id < 0x5909) {
            if (id == 0x5885) lim = 5;
            else if (id == 0x58B3 || id == 0x58B6) lim = 7;
            else if (id == 0x58E0) lim = 4;
        } else if (id <= 0x590D) lim = 8;
        else if (id >= 0x594E && id <= 0x594F) lim = 6;
        else if (id == 0x5951) lim = 6;
        else if (id >= 0x6806 && id <= 0x6807) lim = 3;
    } else if (id < 0x6986) {
        if (id == 0x6901 || id == 0x696D || id == 0x6982) lim = 1;
        else if (id == 0x6921) lim = 3;
    } else if (id == 0x6986) lim = 4;
    else if (id >= 0x69DF && id <= 0x69E0) lim = 8;
    else if (id == 0x6A06) lim = 1;
    else if (id == 0x6A36) lim = 5;
    s32 blocked = 0;
    switch (lim) {
    case 1: blocked = seLimitInstances_l(self, id, pos, 0xA, 4, 160.0f); break;
    case 2: blocked = seLimitInstances_l(self, 0x5801, pos, 0xA, 3, 200.0f); break;
    case 3: blocked = seLimitInstances_l(self, id, pos, 0xA, 3, hundred); break;
    case 4: blocked = seLimitInstances_l(self, id, pos, 0xA, 2, hundred); break;
    case 5: blocked = seLimitInstances_l(self, id, pos, 0xF, 4, 320.0f); break;
    case 6: blocked = seLimitInstances_l(self, id, pos, 0x14, 2, 400.0f); break;
    case 7: blocked = seLimitInstances_l(self, id, pos, 0xF, 3, 320.0f); break;
    case 8: blocked = seLimitInstances_l(self, id, pos, 0x14, 3, 400.0f); break;
    }
    if (blocked != 0) return 0;
    if (id == 0xFFFFFFFF) return 0;
    if ((id & 0xFFFFFF00) == 0x1800 && (id & 0xFF) < 0x5A) {
        /* system sounds 0x1800..0x1859 use their own handle */
        JAIBasic_startSoundVec_l(self, id, self + 0x20E4, pos, 0, 0, 4);
        if (ld(self + 0x20E4) == 0) return 0;
        JAISound_setPortData_l(ld(self + 0x20E4), 9, reverb & 0xFFFF);
        return 0;
    }
    /* the sound position (or the camera position) for the distance-dependent sounds */
    u32 mtx = ld(ld(self) + 8);
    gabi::Local<Vec_l> vv;
    u32 v = gabi::ea(vv.get());
    st(v, ld(0x10003934));
    st(v + 4, ld(0x10003938));
    st(v + 8, ld(0x1000393C));
    if (pos == 0) {
        u32 cp = ld(ld(self));
        if (cp != 0) {
            st(v, ld(cp));
            st(v + 4, ld(cp + 4));
            st(v + 8, ld(cp + 8));
        }
    } else {
        st(v, ld(pos));
        st(v + 4, ld(pos + 4));
        st(v + 8, ld(pos + 8));
    }
    const u32 STREAM5 = 0xC0000005;
    bool ret0 = false; /* return 0 */
    f32 f;             /* (f32)info */
    switch (id) {
    case 0x8: {
        if (info > 0x7FFF) info = 0x7FFF;
        f32 d = (f32)info / 32767.0f * 20.0f;
        if (d > 1.0f) d = 1.0f;
        JAIBasic_startSoundVec_l(self, id, self + 0x284, 0, 0, 0, 4);
        if (ld(self + 0x284) == 0) return 0;
        JAISound_setPitch_l(ld(self + 0x284), gabi::fmadds(0.2f, d, 0.8f), 0, 0);
        JAISound_setVolume_l(ld(self + 0x284), d, 2, 0);
        return 0;
    }
    case 0x1B:
    case 0x381F:
    case 0x584B:
    case 0x6A2A:
    case 0x6A2B:
        return 0;
    case 0x804:
    case 0x81C:
        seStop_l(self, 0x805, 0);
        seStop_l(self, 0x81D, 0);
        break;
    case 0x805:
    case 0x81D:
        seStop_l(self, 0x804, 0);
        seStop_l(self, 0x81C, 0);
        break;
    case 0x80A:
    case 0x80C:
    case 0x825:
        if (JAIZelBasic_checkStreamPlaying_l(self, STREAM5) == 1) return 0;
        if (isDemo_l(self) == 1) return 0;
        if (ld8(self + 0x277) != 0) return 0;
        if (ld8(self + 0xCE) != 0) return 0;
        break;
    case 0x80B:
        if (JAIZelBasic_checkStreamPlaying_l(self, STREAM5) == 1) return 0;
        if (isDemo_l(self) != 1 && ld8(self + 0x277) == 0) {
            if (ld8(self + 0xCE) != 0) return 0;
            break;
        }
        id = 0x8FD;
        if (ld8(self + 0xCE) != 0) return 0;
        break;
    case 0x80D:
        if (JAIZelBasic_checkStreamPlaying_l(self, STREAM5) == 1) return 0;
        if (isDemo_l(self) == 1) id = 0x8FE;
        if (checkSePlaying_l(self, 0x871) == 1) return 0;
        if (ld8(self + 0xCE) != 0) return 0;
        break;
    case 0x80F:
        menuIn_l(self);
        break;
    case 0x810:
    case 0x8C1:
        menuOut_l(self);
        break;
    case 0x82F:
        seStop_l(self, 0x90B, 0x3C);
        break;
    case 0x854: {
        u32 scene = ld(self + 0x294);
        if (scene == 8 || scene == 0xC || scene == 0x13) break;
        id = 0x85D;
        break;
    }
    case 0x876:
        subBgmStart_l(self, 0x80000031);
        break;
    case 0x87D:
        if (info != 0) info -= 1;
        pitch = gabi::fmadds((f32)info, ldf(0x1000392C), 1.0f);
        break;
    case 0x8A7:
    case 0x285D:
    case 0x591B:
    case 0x6981:
        pos = 0;
        break;
    case 0x8AB:
        seStop_l(self, 0x8AA, 0);
        break;
    case 0x8B3:
        st8(self + 0x275, 1);
        stopBattleBgm_l(self);
        {
            u32 main = ld(self + 0x78);
            stf(self + 0x9C, 1.0f);
            if (main != 0) JAISound_setVolume_l(main, mainVol_no9C(self), 0x2D, 0);
        }
        stopBattleBgm_l(self);
        st8(self + 0xD1, 0);
        st8(self + 0xD0, 0);
        st(self + 0xD4, 0);
        subBgmStop_l(self);
        break;
    case 0x8DA:
        subBgmStart_l(self, 0x80000030);
        st8(self + 0x43, 1);
        return 0;
    case 0x8E4:
        if (info >= 100) info = 0x63;
        else if (info != 0) info -= 1;
        pitch = gabi::fmadds((f32)info, 0.01f, 1.0f);
        break;
    case 0x8F5: {
        u32 c6 = ld8(self + 0xC6);
        st8(self + 0x276, 0);
        st8(self + 0xCB, 0);
        if (c6 != 0) break;
        u32 n = ld(self + 0x84);
        if (n == 0x80000036 || n == 0x80000037) break;
        st(self + 0xC0, (u32)-2);
        subBgmStopInner_l(self);
        break;
    }
    case 0x105B:
        if ((s32)ld(self + 0x294) == 0x5C && ld8(self + 0x2A9) < 8) id = 0x706D;
        break;
    case 0x1068:
    case 0x3822:
    case 0x5807:
    case 0x5811:
    case 0x5814:
    case 0x5815:
    case 0x588C:
    case 0x5973:
    case 0x6955:
    case 0x6988:
    case 0x6989:
    case 0x6991:
    case 0x6998:
    case 0x6999:
    case 0x69E7:
    case 0x69E8:
    case 0x69F2:
    case 0x6A06:
    case 0x6A07:
    case 0x6A08:
    case 0x6A09:
    case 0x6A0A:
    case 0x6A0B:
    case 0x6A0C:
    case 0x6A0D:
        if (info >= 100) {
            info = 10000;
            volume = (f32)info / 10000.0f;
        } else {
            if (info == 0) return 0;
            info = info * info;
            volume = (f32)info / 10000.0f;
        }
        break;
    case 0x106A:
        if (info >= 100) info = 100;
        f = (f32)info;
        {
            f32 sq = f * f;
            f32 lin = f / hundred;
            f32 p = sq / 40000.0f;
            volume = gabi::fmadds(lin, 0.85f, 0.15f);
            pitch = p + 0.75f;
        }
        break;
    case 0x1072: {
        if (info >= 100) info = 100;
        f = (f32)info;
        f32 sq = f * f;
        f32 lin = f / hundred;
        f32 p = sq / 20000.0f;
        volume = gabi::fmadds(lin, 0.6f, 0.4f);
        pitch = p + 0.75f;
        if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
        f32 d = dist_of(v);
        if (!(d < 4500.0f)) break;
        pan = 0.5f;
        dolby = 0.5f;
        volume = volume * (d / 4500.0f * 0.7f);
        pos = 0;
        break;
    }
    case 0x107D:
        if (info >= 100) info = 100;
        f = (f32)info;
        volume = f / hundred;
        if (volume != 0.0f) volume = gabi::fmadds(volume, 0.6f, 0.4f);
        pitch = f / 200.0f + 0.5f;
        break;
    case 0x1088:
        if ((s32)ld(self + 0x294) == 0x55) {
            u32 r = ld8(self + 0x2A9);
            if (r == 9 || r == 0xA) return 0;
        }
        break;
    case 0x10A9:
    case 0x303D:
    case 0x50CE:
    case 0x5128:
        if (info >= 100) {
            info = 100;
        } else if (info == 0) {
            return 0;
        }
        volume = (f32)info / hundred;
        break;
    case 0x1863:
        st8(self + 0x274, 1);
        break;
    case 0x186E:
        subBgmStart_l(self, 0x80000032);
        break;
    case 0x205A:
        if (info == 7) id = 0x205C;
        else if (info < 9) break;
        else if (info <= 0xE) id = ld16(0x1000392E + info * 2);
        else if (info >= 0x11 && info <= 0x12) id = 0x205C;
        break;
    case 0x2066:
        st8(self + 0x277, 2);
        break;
    case 0x282D:
    case 0x282E:
        onEnemyDamage_l(self);
        break;
    case 0x2831: {
        if (ld(self + 0x78) == 0) break;
        if (ld(self + 0x88) != 0x8000000B) break;
        u32 sub = ld(self + 0x7C);
        f32 d = ldf(0x1018DC68);
        stf(self + 0xA0, d);
        if (sub != 0) break;
        f32 vol = mul(ldf(self + 0x90), ldf(self + 0x94));
        vol = mul(vol, ldf(self + 0x98));
        vol = mul(vol, ldf(self + 0x9C));
        vol = mul(vol, d);
        vol = mul(vol, ldf(self + 0xA4));
        vol = mul(vol, ldf(self + 0xA8));
        vol = mul(vol, ldf(self + 0xAC));
        vol = mul(vol, ldf(self + 0xBC));
        JAISound_setVolume_l(ld(self + 0x78), vol, 2, 0);
        break;
    }
    case 0x303E:
    case 0x7051: {
        if (info >= 100) info = 100;
        f = (f32)info;
        f32 sq = f * f;
        f32 lin = f / hundred;
        f32 p = sq / 20000.0f;
        volume = gabi::fmadds(lin, 0.7f, 0.3f);
        pitch = p + 0.75f;
        break;
    }
    case 0x3808:
        if (JAIZelBasic_checkStreamPlaying_l(self, STREAM5) == 1) return 0;
        if (ld(self + 0x20E4) != 0) JAISound_stop_l(ld(self + 0x20E4), 1);
        break;
    case 0x380D: {
        if (pos == 0) break;
        u32 tab = self + 0x1F44;
        u32 e = tab + ld(self + 0x1FA4) * 12;
        st(e, ld(pos));
        st(e + 4, ld(pos + 4));
        st(e + 8, ld(pos + 8));
        u32 n = ld(self + 0x1FA4);
        f32 cy = ldf(ld(ld(self)) + 4);
        e = tab + n * 12;
        if (ldf(e + 4) > cy) {
            stf(e + 4, cy);
            n = ld(self + 0x1FA4);
            e = tab + n * 12;
        }
        st(self + 0x1FA4, n + 1);
        pos = e; /* the sound follows the stored (height-clamped) copy */
        break;
    }
    case 0x3815:
    case 0x3818:
        if (ld8(self + 0x56) == 1) {
            id = 0x382F;
            if ((s32)info != 0xD) info = 9;
        }
        if (ld8(self + 0x55) != 1) break;
        JAIZelBasic_seStart_l(self, 0x3831, pos, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        break;
    case 0x50BE:
        if ((s32)ld(self + 0x294) == 0x12 && ld8(self + 0x2A8) == 0x2C && ld8(self + 0x2A9) == 0xA) {
            if (JAIZelBasic_checkStreamPlaying_l(self, STREAM5 + 0x17) == 0 && ld8(self + 0x278) == 0) return 0;
            st8(self + 0x278, 1);
        }
        break;
    case 0x50BF:
    case 0x5895:
        if (info >= 100) info = 100;
        f = (f32)info;
        pitch = f / 300.0f + 1.0f;
        volume = f / hundred;
        break;
    case 0x5127:
        if (info >= 100) info = 100;
        else if (info == 0) return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 20000.0f + 0.5f;
            volume = sq / 10000.0f;
        }
        break;
    case 0x580E:
    case 0x580F: {
        if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
        f32 d = dist_of(v);
        if (d > JAIGlobalParameter_getParamDistanceMax_l()) return 0;
        break;
    }
    case 0x5825:
        st8(self + 0xC5, 1);
        break;
    case 0x584C:
        JAIZelBasic_seStart_l(self, 0x7818, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        break;
    case 0x5875:
        st8(self + 0xDC, 1);
        break;
    case 0x61B1:
        if (info >= 100) {
            info = 100;
            pitch = 100.0f / hundred + 1.0f;
            info = 30;
            f = 30.0f;
        } else {
            if (info == 0) return 0;
            f = (f32)info;
            pitch = f / hundred + 1.0f;
            if (info >= 30) {
                info = 30;
                f = 30.0f;
            }
        }
        volume = f / 30.0f;
        break;
    case 0x61FE:
        if ((s32)ld(self + 0x294) == 0x35 && checkEventBit_l(self, 0x2D04) == 0) return 0;
        break;
    case 0x6906:
        seStop_l(self, 0x6906, 0);
        break;
    case 0x6907:
    case 0x6908:
    case 0x6909:
    case 0x690A:
        seStop_l(self, 0x6907, 0);
        seStop_l(self, 0x6908, 0);
        break;
    case 0x6934: {
        st8(self + 0xDA, 1);
        stopBattleBgm_l(self);
        u32 main = ld(self + 0x78);
        stf(self + 0x9C, 1.0f);
        if (main != 0) JAISound_setVolume_l(main, mainVol_no9C(self), 0x2D, 0);
        stopBattleBgm_l(self);
        st8(self + 0xD1, 0);
        pos = 0;
        st8(self + 0xD0, 0);
        st(self + 0xD4, 0);
        break;
    }
    case 0x6942: {
        if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
        f32 d = dist_of(v);
        if (d > JAIGlobalParameter_getParamDistanceMax_l() * 0.5f) return 0;
        break;
    }
    case 0x6956:
        seStop_l(self, 0x6957, 0);
        break;
    case 0x6957:
        seStop_l(self, 0x6956, 0);
        break;
    case 0x701D:
    case 0x701E:
    case 0x705A:
    case 0x705B:
        if (info >= 100) info = 100;
        else if (info == 0) return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 40000.0f + 0.75f;
            volume = sq / 10000.0f;
        }
        break;
    case 0x702C: {
        if (mtx != 0) PSMTXMultVec_l(mtx, v, v);
        f32 a = std::fabs(ldf(v + 4));
        f32 max = JAIGlobalParameter_getParamDistanceMax_l();
        volume = 1.0f - a / (max + max);
        if (volume < 0.0f) {
            pos = 0;
            volume = 0.0f;
            break;
        }
        if (volume > 1.0f) volume = 1.0f;
        pos = 0;
        break;
    }
    case 0x7036:
    case 0x7037:
    case 0x7038:
    case 0x7039:
        if (info >= 100) info = 100;
        else if (info == 0) return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 80000.0f + 0.95f;
            volume = sq / 10000.0f;
        }
        break;
    case 0x703A:
        st8(self + 0xCE, 1);
        break;
    case 0x705C:
        if (info >= 100) info = 100;
        else if (info == 0) return 0;
        f = (f32)info;
        pitch = f / 80.0f + 0.2f;
        volume = f * f / 10000.0f;
        if (!(pitch > 0.5f)) pitch = 0.5f;
        break;
    }
    (void)ret0;
    /* level-object sounds are mixed by processLevObjSE */
    if (flag != 1) {
        switch (id) {
        case 0x105F:
        case 0x3019:
        case 0x3033:
        case 0x3034:
        case 0x303A:
        case 0x501E:
        case 0x501F:
        case 0x5053:
        case 0x5054:
        case 0x6103:
        case 0x612E:
        case 0x6131:
        case 0x614F:
        case 0x7035:
            setLevObjSE(self, id, pos, reverb);
            return 0;
        }
    }
    const u32 snd = self + 0xE4;
    if ((id & 0x800) == 0) {
        /* restart the instance already playing for this position */
        for (u32 i = 0; i < 32; i++) {
            if (ld(self + 0x164 + i * 4) != id) continue;
            if (ld(snd + i * 4) == 0) continue;
            if (ld(self + 0x1E4 + i * 4) != pos) continue;
            u32 h = snd + i * 4;
            JAIBasic_startSoundVec_l(self, id, h, pos, 0, info, 4);
            if (ld(h) != 0) {
                JAISound_setPortData_l(ld(h), 9, reverb & 0xFFFF);
                if (pitch != 1.0f) JAISound_setPitch_l(ld(h), pitch, 0, 0);
                if (volume != 1.0f) JAISound_setVolume_l(ld(h), volume, 0, 0);
                if (pan != -1.0f) JAISound_setPan_l(ld(h), pan, 0, 0);
                if (dolby != -1.0f) JAISound_setDolby_l(ld(h), dolby, 0, 0);
            }
            return h;
        }
    }
    /* a free slot from the rotating index (+0x264) */
    s32 idx = ld(self + 0x264);
    u32 h = snd + idx * 4;
    if (ld(h) != 0) {
        s32 start = idx;
        if (ld(snd + idx * 4) != 0) {
            for (;;) {
                idx = (idx + 1) % 32;
                if ((u32)idx == (u32)start) return 0;
                if (ld(snd + idx * 4) == 0) break;
            }
        }
        h = snd + idx * 4;
        st(self + 0x264, idx);
    }
    st(h, 0);
    JAIBasic_startSoundVec_l(self, id, snd + ld(self + 0x264) * 4, pos, 0, info, 4);
    if ((s32)id == 0x8AB) {
        if (pos != 0) JAIBasic_deleteObject_l(self, pos);
        pos = 0;
    }
    u32 i2 = ld(self + 0x264);
    h = snd + i2 * 4;
    if (ld(h) != 0) {
        JAISound_setPortData_l(ld(h), 9, reverb & 0xFFFF);
        if (pitch != 1.0f) JAISound_setPitch_l(ld(snd + ld(self + 0x264) * 4), pitch, 0, 0);
        if (volume != 1.0f) JAISound_setVolume_l(ld(snd + ld(self + 0x264) * 4), volume, 0, 0);
        if (pan != -1.0f) JAISound_setPan_l(ld(snd + ld(self + 0x264) * 4), pan, 0, 0);
        if (dolby != -1.0f) JAISound_setDolby_l(ld(snd + ld(self + 0x264) * 4), dolby, 0, 0);
        if (id == 0x887 || (id >= 0x893 && id <= 0x895))
            JAISound_setPortData_l(ld(snd + ld(self + 0x264) * 4), 8, info & 0xFFFF);
        if ((s32)id == 0x2867 || (s32)id == 0x285F) {
            JAISound_setPortData_l(ld(snd + ld(self + 0x264) * 4), 7, info >> 16);
            JAISound_setPortData_l(ld(snd + ld(self + 0x264) * 4), 8, info & 0xFFFF);
        }
        i2 = ld(self + 0x264);
        h = snd + i2 * 4;
    }
    st(self + 0x164 + i2 * 4, id);
    st(self + 0x1E4 + ld(self + 0x264) * 4, pos);
    s32 nx = ld(self + 0x264) + 1;
    st(self + 0x264, nx % 32);
    return h;
}
VERIFY(0x0201EBA0, seStart);

} // namespace JAIZelBasic_2_cpp
