/**
 * d_a_kytag07.cpp (WWHD)
 * Environment tag 07: Tower of the Gods / Hyrule weather (time flow, rain and fog during demos).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag07.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kytag07.h"
static u32 l_demoFrame;
static u32 l_saveInfo;

/* 021B1004 */
static BOOL daKytag07_Draw(kytag07_class*) {
    return TRUE;
}

/* 021B100C */
static BOOL daKytag07_Execute(kytag07_class* i_this) {
    kytag07_env_c* envLight = kytag07_env();
    if (!dComIfGp_checkStageName(STR(0x10013CA0) /* "GTower" */)) {
        kytag07_save_c* save = kytag07_save(l_saveInfo);
        f32 time = save->mTime;
        u16 date = save->mDate;
        if (time >= 60.0f && time < 315.0f)
            time += 0.4f;
        save->mTime = time;          /* dComIfGs_setTime */
        kytag07_save(l_saveInfo)->mDate = date; /* dComIfGs_setDate */
    } else {
        u8 mode = fopAcM_GetParam(i_this) & 0xFF;
        if (dComIfGp_event_runCheck() == FALSE)
            return TRUE;
        if (mode != 1) {
            /* HD: no dComIfGp_demo_get() null check */
            if (kytag07_startCheckOld(dComIfGp_getPEvtManager(), "g2before") && l_demoFrame >= 4719) {
                if (l_demoFrame == 4719)
                    dKy_change_colpat(1);
                if (envLight->mRainCount < 250)
                    envLight->mRainCount = envLight->mRainCount + 1;
                if (kytag07_env()->mMoyaCount < 30) { /* HD: 30 (GameCube 100) */
                    kytag07_env()->mMoyaMode = 0;
                    kytag07_env_c* e = kytag07_env();
                    e->mMoyaCount = e->mMoyaCount + 1;
                }
                if (kytag07_env()->mThunderEff.mMode == 0)
                    kytag07_env()->mThunderEff.mMode = 2;
            }
        } else {
            /* HD: no dComIfGp_demo_get() null check */
            kytag07_env()->mMoyaMode = 0;
            if (l_demoFrame >= 2602) {
                envLight->mRainCount = 0;
                envLight->mMoyaCount = 0;
                kytag07_env()->mThunderEff.mMode = 0;
            } else {
                envLight->mRainCount = 250;
                envLight->mMoyaCount = 30; /* HD: 30 (GameCube 100) */
                if (kytag07_env()->mThunderEff.mMode == 0)
                    kytag07_env()->mThunderEff.mMode = 2;
            }

            f32 blend = 0.0f;
            u32 frame = l_demoFrame;
            if (frame >= 2601) {
                blend = (f32)(frame - 2601) / 200.0f;
                if (blend > 1.0f)
                    blend = 1.0f;
            }
            dKy_custom_colset(1, 2, blend);
        }
    }
    return TRUE;
}

/* 021B131C */
static BOOL daKytag07_IsDelete(kytag07_class*) {
    return TRUE;
}

/* 021B1324 */
static BOOL daKytag07_Delete(kytag07_class*) {
    kytag07_env()->mbDayNightTactStop = 0;
    return TRUE;
}

/* 021B1350 */
static cPhs_State daKytag07_Create(fopAc_ac_c* i_this) {
    kytag07_env_c* env_light = kytag07_env();
    fopAcM_ct(i_this, kytag07_class);

    if (!dComIfGp_checkStageName(STR(0x10013CB4) /* "GTower" */))
        env_light->mbDayNightTactStop = 1;

    return cPhs_COMPLEATE_e;
}

/* 021B1490 */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_kytag07_cpp() {
    sinit_header_statics(0x10464D8C, 0x101B8DF4);
}

/* 021B1524 */
// HD: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013C54 +0xC)
static void SafeString_dt(SafeString* s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 021B1538 */
// HD: kytag07_class deleting destructor (compiler-generated, HD virtual destructor)
static void kytag07_class_dt(kytag07_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        kytag07_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021B158C */
// HD: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_; empty for this TU's
// copy)
static void SafeString_assureTermination(SafeString*) {
}

kytag07_class::kytag07_class() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
str 10013CA8 "g2before"
data l_demoFrame 101D600C
data l_saveInfo 101F84DC
ext 025D50BC void kytag07_actor_dt(void*, s32)
ext 0255FD48 void dKy_change_colpat(u8)
ext 0255FDA4 void dKy_custom_colset(u8, u8, f32)
ext 025445B8 BOOL kytag07_startCheckOld(void*, const char*)
*/
