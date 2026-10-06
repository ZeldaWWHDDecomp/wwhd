/**
 * d_a_kytag01.cpp (WWHD)
 * Environment tag 01: wave influence area (registers a WAVE_INFO and sets up the sea waves).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag01.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG01_VTBL 0x10013A3C    /* kytag01_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x10013A24 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== (copied from
 * d_a_bk_action): the virtual at vtable +0x14 is called twice on the literal's SafeString and once
 * on the stage name's (play+0x5134), then pointer compare and a bounded strcmp (0x40001 chars) */
static inline void SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool dComIfGp_checkStageName(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(name);
    a->__vtbl = SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = SAFESTRING_VTBL;
    b->mStringTop = stage;
    SafeString_vcall(a);
    SafeString_vcall(a);
    u32 pa = a->mStringTop;
    SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* g_env_light (dScnKy_env_light_c, HD offsets): mWaveChan (wave channel) and mpWaveInfl[10] */
static inline u32 env_ea() { return gabi::ea(dKy_getEnvlight()); }
enum : u32 {
    ENV_WAVE_SPEED = 0x9D8,          /* mWaveChan.mWaveSpeed (f32) */
    ENV_WAVE_SPAWNDIST = 0x9DC,      /* mWaveSpawnDist */
    ENV_WAVE_SPAWNRADIUS = 0x9E0,    /* mWaveSpawnRadius */
    ENV_WAVE_SCALE = 0x9E4,          /* mWaveScale */
    ENV_WAVE_SCALERAND = 0x9E8,      /* mWaveScaleRand */
    ENV_WAVE_COUNTERSPEED = 0x9EC,   /* mWaveCounterSpeedScale */
    ENV_WAVE_SCALEBOTTOM = 0x9F0,    /* mWaveScaleBottom */
    ENV_WAVE_FLATINTER = 0x9F4,      /* mWaveFlatInter */
    ENV_WAVE_COUNT = 0x9F8,          /* mWaveCount (s16) */
    ENV_WAVE_RESET = 0x9FA,          /* mWaveReset (u8) */
    ENV_WAVE_2F = 0x9FB,             /* field_0x2f (u8) */
    ENV_WAVEINFL = 0xAEC,            /* WAVE_INFO* mpWaveInfl[10] */
};

struct WAVE_INFO {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ be<f32> mOuterRadius;
    /* 0x10 */ be<f32> mInnerRadius;
};

struct kytag01_class : fopAc_ac_c {
    /* 0x3AC */ WAVE_INFO mWaveInfo;
};
WWHD_SIZE(kytag01_class, 0x3C0);

/* 021AF434 */
static BOOL daKytag01_Draw(kytag01_class* i_this) {
    WWHD_FUNC(0x021AF434, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AF434, daKytag01_Draw);

/* 021AF43C: wether_tag_move (empty) inlined */
static BOOL daKytag01_Execute(kytag01_class* i_this) {
    WWHD_FUNC(0x021AF43C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AF43C, daKytag01_Execute);

/* 021AF444 */
static BOOL daKytag01_IsDelete(kytag01_class* i_this) {
    WWHD_FUNC(0x021AF444, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AF444, daKytag01_IsDelete);

/* 021AF44C */
static BOOL daKytag01_Delete(kytag01_class* i_this) {
    WWHD_FUNC(0x021AF44C, BOOL, i_this);
    u32 env_light = env_ea();
    for (u32 i = 0; i < 10; i++) {
        if (gabi::load<u32>(env_light + ENV_WAVEINFL + 4 * i) == gabi::ea(&i_this->mWaveInfo))
            gabi::store<u32>(env_light + ENV_WAVEINFL + 4 * i, 0);
    }
    gabi::store<s16>(env_ea() + ENV_WAVE_COUNT, 0);
    return TRUE;
}
VERIFY(0x021AF44C, daKytag01_Delete);

/* wave_make (inlined into Create). HD: each g_env_light access goes through the accessor */
static inline void wave_make() {
    if (gabi::load<s16>(env_ea() + ENV_WAVE_COUNT) == 0) {
        gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNDIST, 20000.0f);
        gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNRADIUS, 22000.0f);
        gabi::store<u8>(env_ea() + ENV_WAVE_RESET, 0);
        gabi::store<f32>(env_ea() + ENV_WAVE_SCALE, 300.0f);
        gabi::store<f32>(env_ea() + ENV_WAVE_SCALERAND, 0.001f);
        gabi::store<f32>(env_ea() + ENV_WAVE_COUNTERSPEED, 1.2f);
        gabi::store<u8>(env_ea() + ENV_WAVE_2F, 0);
        gabi::store<f32>(env_ea() + ENV_WAVE_SCALEBOTTOM, 6.0f);
        gabi::store<s16>(env_ea() + ENV_WAVE_COUNT, 300);
        gabi::store<f32>(env_ea() + ENV_WAVE_SPEED, 30.0f);
        gabi::store<f32>(env_ea() + ENV_WAVE_FLATINTER, 0.0f);
        if (dComIfGp_checkStageName(STR(0x10013A1C) /* "MajyuE" */)) {
            gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNDIST, 25000.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNRADIUS, 27000.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SCALEBOTTOM, 8.0f);
        }
        if (dComIfGp_checkStageName(STR(0x10013A10) /* "M_NewD2" */)) {
            gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNDIST, 35000.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SPAWNRADIUS, 37000.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SCALEBOTTOM, 8.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_COUNTERSPEED, 1.5f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SCALE, 500.0f);
            gabi::store<f32>(env_ea() + ENV_WAVE_SPEED, 55.0f);
        }
    }
}

/* 021AF4B8 */
static cPhs_State daKytag01_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021AF4B8, cPhs_State, i_ac);
    u32 env_light = env_ea();
    kytag01_class* i_this = (kytag01_class*)i_ac;
    /* fopAcM_ct(i_ac, kytag01_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = KYTAG01_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }

    i_this->mWaveInfo.mPos.copy(i_ac->current.pos);
    i_this->mWaveInfo.mInnerRadius = i_ac->scale.x * 5000.0f;
    i_this->mWaveInfo.mOuterRadius = i_ac->scale.z * 5000.0f;

    f32 defaultOuter = i_this->mWaveInfo.mInnerRadius + 500.0f;
    if (!(defaultOuter < i_this->mWaveInfo.mOuterRadius)) /* GHS: `>=` tested as !(<) */
        i_this->mWaveInfo.mOuterRadius = defaultOuter;

    for (u32 i = 0; i < 10; i++) {
        if (gabi::load<u32>(env_light + ENV_WAVEINFL + 4 * i) == 0) {
            gabi::store<u32>(env_light + ENV_WAVEINFL + 4 * i, gabi::ea(&i_this->mWaveInfo));
            break;
        }
    }

    wave_make();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021AF4B8, daKytag01_Create);

/* 021AF87C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag01_cpp() {
    WWHD_FUNC(0x021AF87C, void, (u32)0);
    sinit_header_statics(0x10464CE4, 0x101B8B3C);
}
VERIFY(0x021AF87C, __sinit_d_a_kytag01_cpp);

/* 021AF910: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013A24 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x021AF910, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x021AF910, SafeString_dt);

/* 021AF924: kytag01_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag01_class_dt(kytag01_class* i_this, s32 flags) {
    WWHD_FUNC(0x021AF924, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021AF924, kytag01_class_dt);

/* 021AF978: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x021AF978, void, (u32)0);
}
VERIFY(0x021AF978, SafeString_assureTermination);
