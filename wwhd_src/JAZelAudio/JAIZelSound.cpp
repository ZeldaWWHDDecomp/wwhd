/* JAIZelSound (WWHD): Zelda's JAISound subclass (distance volume / pan / surround of sound effects).
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/JAZelAudio/JAIZelSound.cpp) and verified against cking.rpx.
 *
 * Translation unit 0202ACCC..0202B6AF (8 functions), from the image: the sinit-only data TUs
 * (JAIZelParam, JAIZelScene) end at 0202ACCB; JAIZelSound runs the constructor ..
 * setSeDistanceDolby, its header __sinit (0202B5C8, rodata 10003F20, before its vtable 10003F28),
 * then the deleting destructor (0202B65C). The next (unnamed) unit starts at 0202B6B0.
 *
 * HD layout (JAISound, size 0x48): +4 u8 camera index, +0x14 state, +0x1C position info array
 * (0x1C bytes per camera; +8 depth, +0x18 distance), +0x24, +0x44 vtable (10003F28). */
#include "bindings.h"

namespace JAIZelSound_cpp {
#include "jaizel_local.h"

static inline u32 getSwBit_l(u32 s) { return gabi::call<u32>(0x0280CFF0, s); }
static inline f32 getParamMaxVolumeDistance_l() { return gabi::call<f32>(0x02804724); }
static inline f32 getParamMinDistanceVolume_l() { return gabi::call<f32>(0x02804730); }
static inline u32 getParamAudioCameraMax_l() { return gabi::call<u32>(0x028047F0); }
static inline f32 getParamSeDolbyCenterValue_l() { return gabi::call<f32>(0x02804754); }
static inline f32 getParamSeDolbyFrontDistanceMax_l() { return gabi::call<f32>(0x02804760); }
static inline f32 getParamSeDolbyBehindDistanceMax_l() { return gabi::call<f32>(0x0280476C); }
static inline void setSeInterVolume_l(u32 s, u32 t, f32 v, u32 fade, u32 k) { gabi::call(0x0280B518, s, t, fade, k, v); }
static inline void setSeInterPan_l(u32 s, u32 t, f32 v, u32 fade, u32 k) { gabi::call(0x0280B8B0, s, t, fade, k, v); }
static inline void setSeInterDolby_l(u32 s, u32 t, f32 v, u32 fade, u32 k) { gabi::call(0x0280C18C, s, t, fade, k, v); }

/* (f32)(s32)(1 << sh) with the PowerPC slw semantics (shift amounts 32..63 give 0) */
static inline f32 pow2f(u32 sh) {
    sh &= 0x3F;
    u32 p = sh >= 32 ? 0 : (1u << sh);
    return (f32)(s32)p;
}

/* 0202ACCC JAIZelSound::JAIZelSound() (HD size 0x48) */
u32 JAIZelSound_ct(u32 self) {
    WWHD_FUNC(0x0202ACCC, u32, self);
    if (self == 0) {
        self = gabi::call<u32>(0x0273AD10, 0x48);
        if (self == 0) return 0;
    }
    gabi::call(0x0280AF40, self); /* JAISound::JAISound */
    st(self + 0x44, 0x10003F28);
    return self;
}
VERIFY(0x0202ACCC, JAIZelSound_ct);

/* 0202AD20 JAIZelSound::setDistanceVolumeCommon(f32 dist, u8 mode) */
f32 setDistanceVolumeCommon(u32 self, f32 dist, u32 mode) {
    WWHD_FUNC(0x0202AD20, f32, self, dist, mode);
    u32 c = ld8(self + 4);
    u32 pi = ld(self + 0x1C);
    f32 d;
    if (c != 4) {
        d = ldf(pi + c * 0x1C + 0x18);
    } else {
        d = ldf(pi + 0x18);
        for (u32 i = 1; i < getParamAudioCameraMax_l(); i = (i + 1) & 0xFF) {
            f32 v = ldf(ld(self + 0x1C) + i * 0x1C + 0x18);
            if (v < d) d = v;
        }
    }
    if (!(d > getParamMaxVolumeDistance_l())) return 1.0f;
    d = d - getParamMaxVolumeDistance_l();
    f32 r = dist - getParamMaxVolumeDistance_l();
    if (mode > 7) r = r * pow2f(mode - 4);
    else if (mode > 3) r = r / pow2f((mode & 3) + 1);
    else if (mode != 0) r = r * pow2f(mode & 3);
    bool lin = mode - 4 < 4;
    if (d < r) {
        f32 t = 1.0f - d / r;
        if (lin) return t;
        f32 mn = getParamMinDistanceVolume_l();
        f32 mn2 = getParamMinDistanceVolume_l();
        return gabi::fmadds(1.0f - mn2, t, mn);
    }
    if (lin) return 0.0f;
    return getParamMinDistanceVolume_l();
}
VERIFY(0x0202AD20, setDistanceVolumeCommon);

/* 0202AF70 (HD-only, unnamed): an inverse-distance volume: 1 within near*maxVolumeDistance, else
 * near*mvd / (near*mvd + k * (dist - near*mvd)), 0 below the floor */
f32 distanceVolumeHD(u32 self, f32 nearScale, f32 k, f32 floor) {
    WWHD_FUNC(0x0202AF70, f32, self, nearScale, k, floor);
    if (k == 0.0f) return 1.0f;
    f32 d = ldf(ld(self + 0x1C) + ld8(self + 4) * 0x1C + 0x18);
    f32 mv = getParamMaxVolumeDistance_l() * nearScale;
    if (d < mv) return 1.0f;
    f32 v = mv / gabi::fmadds(k, d - mv, mv);
    return v - floor >= 0.0f ? v : 0.0f;
}
VERIFY(0x0202AF70, distanceVolumeHD);

/* 0202B08C JAIZelSound::setSeDistanceVolume(u8 fade). HD: per-sound volume curves for a few ids */
void setSeDistanceVolume(u32 self, u32 fade) {
    WWHD_FUNC(0x0202B08C, void, self, fade);
    if ((getSwBit_l(self) & 0x00400000) && ld(self + 0x14) > 1) return;
    if (getSwBit_l(self) & 2) {
        setSeInterVolume_l(self, 4, 1.0f, fade, 0);
        return;
    }
    u32 mode;
    if (getSwBit_l(self) & 0x00300000) mode = (((getSwBit_l(self) >> 20) & 3) + 7) & 0xFF;
    else mode = (getSwBit_l(self) >> 16) & 7;
    const f32 small = ldf(0x10003EFC); /* 0.01 */
    u32 id = JAISound_getID_l(self);
    f32 v;
    int kind = 0; /* 0: setDistanceVolumeCommon, 1: HD curve, 2: HD curve (weaker) halved, 3: sea curve */
    if (id < 0x69DF) {
        if (id == 0x48DA || id == 0x590A) kind = 1;
        else if (id == 0x6238) kind = 2;
    } else if (id == 0x69DF || id == 0x6A36) {
        kind = 1;
    } else if (id == 0x701D) {
        kind = (s32)ld(zel_basic() + 0x294) == 0x12 ? 0 : 3;
    } else if (id == 0x7047) {
        kind = 2;
    }
    if (kind == 1) {
        v = distanceVolumeHD(self, small, 1.0f, small);
    } else if (kind == 2) {
        v = distanceVolumeHD(self, ldf(0x10003F08), 1.0f, small) * 0.5f; /* 0.005 */
    } else if (kind == 3) {
        v = distanceVolumeHD(self, 0.25f, 3.5f, small);
    } else {
        u32 vt = ld(self + 0x44);
        f32 max = JAIGlobalParameter_getParamDistanceMax_l();
        v = gabi::call_ptr<f32>(ld(vt + 0x24), self, mode, max); /* setDistanceVolumeCommon */
    }
    if ((getSwBit_l(self) & 0x01000000) && !(v > ldf(0x10003F10))) {
        setSeInterVolume_l(self, 4, ldf(0x10003F10), fade, 0); /* at least 0.3333 */
        return;
    }
    setSeInterVolume_l(self, 4, v >= 0.0f ? v : 0.0f, fade, 0);
}
VERIFY(0x0202B08C, setSeDistanceVolume);

/* 0202B354 JAIZelSound::setSeDistancePan(u8 fade) (unnamed by the matcher) */
void setSeDistancePan(u32 self, u32 fade) {
    WWHD_FUNC(0x0202B354, void, self, fade);
    if ((getSwBit_l(self) & 0x00800000) && ld(self + 0x14) > 1) return;
    f32 p = gabi::call_ptr<f32>(ld(ld(self + 0x44) + 0x2C), self); /* setDistancePanCommon */
    setSeInterPan_l(self, 4, p, fade, 0);
}
VERIFY(0x0202B354, setSeDistancePan);

static inline void setDolby(u32 self, f32 v, u32 fade) {
    u32 b = (u8)gabi::ftoi(v);
    setSeInterDolby_l(self, 4, (f32)b / 127.0f, fade, 0);
}

/* 0202B3C8 JAIZelSound::setSeDistanceDolby(u8 fade) */
void setSeDistanceDolby(u32 self, u32 fade) {
    WWHD_FUNC(0x0202B3C8, void, self, fade);
    if ((getSwBit_l(self) & 0x00800000) && ld(self + 0x14) > 1) return;
    u32 x24 = ld(self + 0x24);
    u32 pi = ld(self + 0x1C);
    if (x24 == 0) {
        setDolby(self, 0.0f, fade);
        return;
    }
    f32 fm0 = getParamSeDolbyFrontDistanceMax_l();
    if (ldf(pi + 8) < fm0) {
        setDolby(self, 0.0f, fade);
        return;
    }
    f32 z = ldf(pi + 8);
    if (z < 0.0f) {
        f32 c = getParamSeDolbyCenterValue_l();
        f32 fm = getParamSeDolbyFrontDistanceMax_l();
        f32 t = c * (fm - ldf(pi + 8));
        setDolby(self, t / getParamSeDolbyFrontDistanceMax_l(), fade);
        return;
    }
    if (!(z < getParamSeDolbyBehindDistanceMax_l())) {
        setDolby(self, 127.0f, fade);
        return;
    }
    f32 c = getParamSeDolbyCenterValue_l();
    f32 c2 = getParamSeDolbyCenterValue_l();
    f32 rest = 127.0f - c2;
    f32 bm = getParamSeDolbyBehindDistanceMax_l();
    f32 q = ldf(pi + 8) / bm;
    setDolby(self, gabi::fmadds(rest, q, c), fade);
}
VERIFY(0x0202B3C8, setSeDistanceDolby);

/* 0202B5C8 __sinit_JAIZelSound_cpp (header statics) */
void __sinit_JAIZelSound_cpp() {
    WWHD_FUNC(0x0202B5C8, void);
    header_sinit(0x101FFD24, 0x1018EBD4, 0x10003F20);
}
VERIFY(0x0202B5C8, __sinit_JAIZelSound_cpp);

/* 0202B65C JAIZelSound::~JAIZelSound() (deleting) */
void JAIZelSound_dt(u32 self, u32 flags) {
    WWHD_FUNC(0x0202B65C, void, self, flags);
    if (self == 0) return;
    gabi::call(0x0280AFE0, self, 0); /* JAISound::~JAISound */
    if (flags & 1) gabi::call(0x0273AF40, self); /* __dl */
}
VERIFY(0x0202B65C, JAIZelSound_dt);

} // namespace JAIZelSound_cpp
