/** JAIZelSound: distance volume, pan and surround for Zelda sound effects.
 * Verified against cking.rpx; tww names and WWHD layout. */
#include "JAZelAudio/JAIZelSound.h"
#include "gc_api/tu.h"

// HD: header statics follow the GHS per-translation-unit layout.
const gc_tu_data gc_tu = {.headerStatics = {0x101FFD24, 0x1018EBD4, 0x101FFD30}};
static f32 l_small = 0.01f;
static f32 l_weakerNear = 0.005f;
static f32 l_minimumVolume = 0.3333f;
using JAIZelBasicPtr = gptr<JAIZelBasicSoundView>;
static JAIZelBasicPtr l_basic;
static JAISound::Vtable l_vtable;
void JAISound__ct(JAISound*);
void JAISound__dt(JAISound*, u32);
f32 JAIGlobalParameter_getParamMaxVolumeDistance();
f32 JAIGlobalParameter_getParamMinDistanceVolume();
u32 JAIGlobalParameter_getParamAudioCameraMax();
f32 JAIGlobalParameter_getParamSeDolbyCenterValue();
f32 JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
f32 JAIGlobalParameter_getParamSeDolbyBehindDistanceMax();
f32 JAIGlobalParameter_getParamDistanceMax();
// HD: retain signed PowerPC slw behavior, including zero for shifts 32..63.
static inline f32 pow2f(u32 sh) {
    sh &= 0x3F;
    u32 p = sh >= 32 ? 0 : (1u << sh);
    return (f32)(s32)p;
}

/* 0202ACCC */
JAIZelSound* JAIZelSound__ct(JAIZelSound* self) {
    if (self == 0) {
        self = static_cast<JAIZelSound*>(operator_new(sizeof(JAIZelSound)));
        if (self == 0) return 0;
    }
    JAISound__ct(self); /* JAISound::JAISound */
    self->__vtbl = &l_vtable;
    return self;
}

/* 0202AD20 */
f32 JAIZelSound::setDistanceVolumeCommon(f32 dist, u32 mode) {
    u32 c = field_0x8;
    PositionInfo_t* pi = mPositionInfo;
    f32 d;
    if (c != 4) {
        d = pi[c].field_0x18;
    } else {
        d = pi[0].field_0x18;
        for (u32 i = 1; i < JAIGlobalParameter_getParamAudioCameraMax(); i = (i + 1) & 0xFF) {
            f32 v = mPositionInfo.get()[i].field_0x18;
            if (v < d) d = v;
        }
    }
    // HD: unordered or equal distance stays at full volume.
    if (!(d > JAIGlobalParameter_getParamMaxVolumeDistance())) return 1.0f;
    d = d - JAIGlobalParameter_getParamMaxVolumeDistance();
    f32 r = dist - JAIGlobalParameter_getParamMaxVolumeDistance();
    if (mode > 7) r = r * pow2f(mode - 4);
    else if (mode > 3) r = r / pow2f((mode & 3) + 1);
    else if (mode != 0) r = r * pow2f(mode & 3);
    bool lin = mode - 4 < 4;
    if (d < r) {
        f32 t = 1.0f - d / r;
        if (lin) return t;
        f32 mn = JAIGlobalParameter_getParamMinDistanceVolume();
        f32 mn2 = JAIGlobalParameter_getParamMinDistanceVolume();
        return gabi::fmadds(1.0f - mn2, t, mn);
    }
    if (lin) return 0.0f;
    return JAIGlobalParameter_getParamMinDistanceVolume();
}

/* 0202AF70 */
// HD: inverse-distance attenuation for selected sound IDs.
f32 JAIZelSound::distanceVolumeHD(f32 nearScale, f32 k, f32 floor) {
    if (k == 0.0f) return 1.0f;
    f32 d = mPositionInfo.get()[(u32)field_0x8].field_0x18;
    f32 mv = JAIGlobalParameter_getParamMaxVolumeDistance() * nearScale;
    if (d < mv) return 1.0f;
    f32 v = mv / gabi::fmadds(k, d - mv, mv);
    return v - floor >= 0.0f ? v : 0.0f;
}

/* 0202B08C */
void JAIZelSound::setSeDistanceVolume(u32 fade) {
    if ((getSwBit() & 0x00400000) && field_0x18 > 1) return;
    if (getSwBit() & 2) {
        setSeInterVolume(SOUNDPARAM_Distance, 1.0f, fade, 0);
        return;
    }
    u32 mode;
    if (getSwBit() & 0x00300000) mode = (((getSwBit() >> 20) & 3) + 7) & 0xFF;
    else mode = (getSwBit() >> 16) & 7;
    const f32 small = l_small; /* 0.01 */
    u32 id = getID();
    f32 v;
    // HD: additional per-sound attenuation curves.
    enum VolumeCurve { Common, InverseDistance, WeakerInverseDistance, Sea };
    VolumeCurve kind = Common;
    if (id < 0x69DF) {
        if (id == 0x48DA || id == 0x590A) kind = InverseDistance;
        else if (id == 0x6238) kind = WeakerInverseDistance;
    } else if (id == 0x69DF || id == 0x6A36) {
        kind = InverseDistance;
    } else if (id == 0x701D) {
        kind = (s32)l_basic->mScene == 0x12 ? Common : Sea;
    } else if (id == 0x7047) {
        kind = WeakerInverseDistance;
    }
    if (kind == InverseDistance) {
        v = distanceVolumeHD(small, 1.0f, small);
    } else if (kind == WeakerInverseDistance) {
        v = distanceVolumeHD(l_weakerNear, 1.0f, small) * 0.5f; /* 0.005 */
    } else if (kind == Sea) {
        v = distanceVolumeHD(0.25f, 3.5f, small);
    } else {
        // HD: preserve virtual dispatch and the load before the parameter getter.
        Vtable* vt = __vtbl;
        f32 max = JAIGlobalParameter_getParamDistanceMax();
        v = gabi::call_ptr<f32>(vt->setDistanceVolumeCommon, this, mode, max); /* setDistanceVolumeCommon */
    }
    if ((getSwBit() & 0x01000000) && !(v > l_minimumVolume)) {
        setSeInterVolume(SOUNDPARAM_Distance, l_minimumVolume, fade, 0); /* at least 0.3333 */
        return;
    }
    setSeInterVolume(SOUNDPARAM_Distance, v >= 0.0f ? v : 0.0f, fade, 0);
}

/* 0202B354 */
void JAIZelSound::setSeDistancePan(u32 fade) {
    if ((getSwBit() & 0x00800000) && field_0x18 > 1) return;
    f32 p = gabi::call_ptr<f32>(__vtbl->setDistancePanCommon, this); /* setDistancePanCommon */
    setSeInterPan(SOUNDPARAM_Distance, p, fade, 0);
}

// inlined into JAIZelSound::setSeDistanceDolby
static inline void setDolby(JAIZelSound* self, f32 v, u32 fade) {
    u32 b = (u8)gabi::ftoi(v);
    self->setSeInterDolby(SOUNDPARAM_Distance, (f32)b / 127.0f, fade, 0);
}

/* 0202B3C8 */
void JAIZelSound::setSeDistanceDolby(u32 fade) {
    if ((getSwBit() & 0x00800000) && field_0x18 > 1) return;
    cXyz* x24 = field_0x28;
    PositionInfo_t* pi = mPositionInfo;
    if (x24 == 0) {
        setDolby(this, 0.0f, fade);
        return;
    }
    f32 fm0 = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
    if ((f32)pi->field_0x8 < fm0) {
        setDolby(this, 0.0f, fade);
        return;
    }
    f32 z = (f32)pi->field_0x8;
    if (z < 0.0f) {
        f32 c = JAIGlobalParameter_getParamSeDolbyCenterValue();
        f32 fm = JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
        f32 t = c * (fm - (f32)pi->field_0x8);
        setDolby(this, t / JAIGlobalParameter_getParamSeDolbyFrontDistanceMax(), fade);
        return;
    }
    if (!(z < JAIGlobalParameter_getParamSeDolbyBehindDistanceMax())) {
        setDolby(this, 127.0f, fade);
        return;
    }
    f32 c = JAIGlobalParameter_getParamSeDolbyCenterValue();
    f32 c2 = JAIGlobalParameter_getParamSeDolbyCenterValue();
    f32 rest = 127.0f - c2;
    f32 bm = JAIGlobalParameter_getParamSeDolbyBehindDistanceMax();
    f32 q = (f32)pi->field_0x8 / bm;
    setDolby(this, gabi::fmadds(rest, q, c), fade);
}

// HD: compiler-generated header statics and deleting destructor.
/* 0202B5C8 */
void __sinit_JAIZelSound_cpp() {
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2], 0x10003F20);
}

/* 0202B65C */
void JAIZelSound__dt(JAIZelSound* self, u32 flags) {
    if (self == 0) return;
    JAISound__dt(self, 0); /* JAISound::~JAISound */
    if (flags & 1) operator_delete(self); /* __dl */
}


/* WWHD glue
data l_small 10003EFC
data l_weakerNear 10003F08
data l_minimumVolume 10003F10
data l_vtable 10003F28
data l_basic 101FFC78
ext 0280AF40 void JAISound__ct(JAISound*)
ext 0280AFE0 void JAISound__dt(JAISound*, u32)
ext 0280CFF0 u32 JAISound::getSwBit()
ext 0280D2D8 u32 JAISound::getID()
ext 0280B518 void JAISound::setSeInterVolume(u32, f32, u32, u32)
ext 0280B8B0 void JAISound::setSeInterPan(u32, f32, u32, u32)
ext 0280C18C void JAISound::setSeInterDolby(u32, f32, u32, u32)
ext 02804724 f32 JAIGlobalParameter_getParamMaxVolumeDistance()
ext 02804730 f32 JAIGlobalParameter_getParamMinDistanceVolume()
ext 028047F0 u32 JAIGlobalParameter_getParamAudioCameraMax()
ext 02804754 f32 JAIGlobalParameter_getParamSeDolbyCenterValue()
ext 02804760 f32 JAIGlobalParameter_getParamSeDolbyFrontDistanceMax()
ext 0280476C f32 JAIGlobalParameter_getParamSeDolbyBehindDistanceMax()
ext 02804718 f32 JAIGlobalParameter_getParamDistanceMax()
*/
