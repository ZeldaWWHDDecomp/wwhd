/**
 * d_a_kytag01.cpp (WWHD)
 * Environment tag 01: wave influence area (registers a WAVE_INFO and sets up the sea waves).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag01.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kytag01.h"

/* 021AF434 */
static BOOL daKytag01_Draw(kytag01_class* i_this) {
    return TRUE;
}

/* 021AF43C */
// HD: wether_tag_move (empty) inlined
static BOOL daKytag01_Execute(kytag01_class* i_this) {
    return TRUE;
}

/* 021AF444 */
static BOOL daKytag01_IsDelete(kytag01_class* i_this) {
    return TRUE;
}

/* 021AF44C */
static BOOL daKytag01_Delete(kytag01_class* i_this) {
    kytag01_env_c* env_light = kytag01_env();
    for (u32 i = 0; i < 10; i++) {
        if (env_light->mpWaveInfl[i] == &i_this->mWaveInfo)
            env_light->mpWaveInfl[i] = nullptr;
    }
    kytag01_env()->mWaveChan.mWaveCount = 0;
    return TRUE;
}

/* wave_make (inlined into Create). HD: each g_env_light access goes through the accessor */
static inline void wave_make() {
    if (kytag01_env()->mWaveChan.mWaveCount == 0) {
        kytag01_env()->mWaveChan.mWaveSpawnDist = 20000.0f;
        kytag01_env()->mWaveChan.mWaveSpawnRadius = 22000.0f;
        kytag01_env()->mWaveChan.mWaveReset = 0;
        kytag01_env()->mWaveChan.mWaveScale = 300.0f;
        kytag01_env()->mWaveChan.mWaveScaleRand = 0.001f;
        kytag01_env()->mWaveChan.mWaveCounterSpeedScale = 1.2f;
        kytag01_env()->mWaveChan.field_0x2f = 0;
        kytag01_env()->mWaveChan.mWaveScaleBottom = 6.0f;
        kytag01_env()->mWaveChan.mWaveCount = 300;
        kytag01_env()->mWaveChan.mWaveSpeed = 30.0f;
        kytag01_env()->mWaveChan.mWaveFlatInter = 0.0f;
        if (dComIfGp_checkStageName("MajyuE")) {
            kytag01_env()->mWaveChan.mWaveSpawnDist = 25000.0f;
            kytag01_env()->mWaveChan.mWaveSpawnRadius = 27000.0f;
            kytag01_env()->mWaveChan.mWaveScaleBottom = 8.0f;
        }
        if (dComIfGp_checkStageName("M_NewD2")) {
            kytag01_env()->mWaveChan.mWaveSpawnDist = 35000.0f;
            kytag01_env()->mWaveChan.mWaveSpawnRadius = 37000.0f;
            kytag01_env()->mWaveChan.mWaveScaleBottom = 8.0f;
            kytag01_env()->mWaveChan.mWaveCounterSpeedScale = 1.5f;
            kytag01_env()->mWaveChan.mWaveScale = 500.0f;
            kytag01_env()->mWaveChan.mWaveSpeed = 55.0f;
        }
    }
}

/* 021AF4B8 */
static cPhs_State daKytag01_Create(fopAc_ac_c* i_ac) {
    kytag01_env_c* env_light = kytag01_env();
    kytag01_class* i_this = (kytag01_class*)i_ac;
    fopAcM_ct(i_ac, kytag01_class);

    i_this->mWaveInfo.mPos.copy(i_ac->current.pos);
    i_this->mWaveInfo.mInnerRadius = i_ac->scale.x * 5000.0f;
    i_this->mWaveInfo.mOuterRadius = i_ac->scale.z * 5000.0f;

    f32 defaultOuter = i_this->mWaveInfo.mInnerRadius + 500.0f;
    if (!(defaultOuter < i_this->mWaveInfo.mOuterRadius)) // HD: GHS uses !(<), including the unordered NaN case.
        i_this->mWaveInfo.mOuterRadius = defaultOuter;

    for (u32 i = 0; i < 10; i++) {
        if (env_light->mpWaveInfl[i] == nullptr) {
            env_light->mpWaveInfl[i] = &i_this->mWaveInfo;
            break;
        }
    }

    wave_make();
    return cPhs_COMPLEATE_e;
}

/* 021AF87C */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_kytag01_cpp() {
    sinit_header_statics(0x10464CE4, 0x101B8B3C);
}

/* 021AF910 */
// HD: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013A24 +0xC)
static void SafeString_dt(SafeString* s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 021AF924 */
// HD: kytag01_class deleting destructor (compiler-generated, HD virtual destructor)
static void kytag01_class_dt(kytag01_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        kytag01_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021AF978 */
// HD: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy)
static void SafeString_assureTermination(SafeString*) {
}

kytag01_class::kytag01_class() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
str 10013A1C "MajyuE"
str 10013A10 "M_NewD2"
ext 025D50BC void kytag01_actor_dt(void*, s32)
*/
