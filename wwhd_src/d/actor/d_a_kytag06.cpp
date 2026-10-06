/**
 * d_a_kytag06.cpp (WWHD)
 * Environment tag 06: rain and thunder during the "ARRIVAL_BRK" event (time runs on).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag06.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG06_VTBL 0x10013C04 /* kytag06_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0255FD48 dKy_change_colpat(u8) */
static inline void dKy_change_colpat(u8 pat) { gabi::call(0x0255FD48, pat); }
/* 0257E7C0 dKyw_rain_set(int) */
static inline void dKyw_rain_set(s32 count) { gabi::call(0x0257E7C0, count); }
/* 0200ECD4 cLib_addCalc(f32*, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 025B7D90 dSv_player_collect_c::isSymbol (save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }
/* dComIfGp_event_runCheck(): play+0x5292 (u8) != 0 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 025445B8 dEvent_manager_c::startCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* save info (info + 0x24 = mSavedata.mPlayer.mPlayerStatusB): time f32 at +0x44, date u16 at +0x48 */
static inline u32 dComIfGs_raw() { return gabi::load<u32>(0x101F84DC); }

/* g_env_light fields (dScnKy_env_light_c, HD offsets) */
static inline u32 env_ea() { return gabi::ea(dKy_getEnvlight()); }
enum : u32 {
    ENV_RAINCOUNT = 0xA40,      /* mRainCount (s32) */
    ENV_THUNDER_MODE = 0xAB4,   /* mThunderEff.mMode (s32) */
};

struct kytag06_class : fopAc_ac_c {
    /* 0x3AC */ be<s32> field_0x290;
    /* 0x3B0 */ be<f32> field_0x294;
};
WWHD_SIZE(kytag06_class, 0x3B4);

/* 021B0D3C */
static BOOL daKytag06_Draw(kytag06_class*) {
    WWHD_FUNC(0x021B0D3C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0D3C, daKytag06_Draw);

/* 021B0D44 */
static BOOL daKytag06_Execute(kytag06_class* i_this) {
    WWHD_FUNC(0x021B0D44, BOOL, i_this);
    f32 time;
    int date;

    if (dComIfGp_event_runCheck() == FALSE) {
        return TRUE;
    }
    if (!dComIfGp_evmng_startCheckOld(STR(0x10013C34) /* "ARRIVAL_BRK" */)) {
        return TRUE;
    }

    u32 save = dComIfGs_raw();
    time = gabi::load<f32>(save + 0x44);
    date = gabi::load<u16>(save + 0x48);
    if (time >= 90.0f && time < 345.0f) {
        time += 0.05f;
    }
    gabi::store<f32>(save + 0x44, time);               /* dComIfGs_setTime(time) */
    gabi::store<u16>(dComIfGs_raw() + 0x48, (u16)date); /* dComIfGs_setDate(date) */

    dKy_change_colpat(1);
    date = gabi::ftoi(i_this->field_0x294 * 250.0f);
    if (date > gabi::load<s32>(env_ea() + ENV_RAINCOUNT)) {
        dKyw_rain_set(date);
    }
    gabi::store<s32>(env_ea() + ENV_THUNDER_MODE, 1);
    cLib_addCalc(&i_this->field_0x294, 1.0f, 0.1f, 0.001f, 0.0001f);
    return TRUE;
}
VERIFY(0x021B0D44, daKytag06_Execute);

/* 021B0E68 */
static BOOL daKytag06_IsDelete(kytag06_class*) {
    WWHD_FUNC(0x021B0E68, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0E68, daKytag06_IsDelete);

/* 021B0E70 */
static BOOL daKytag06_Delete(kytag06_class*) {
    WWHD_FUNC(0x021B0E70, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0E70, daKytag06_Delete);

/* 021B0E78 */
static cPhs_State daKytag06_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B0E78, cPhs_State, i_this);
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */
    /* fopAcM_ct_Retail(i_this, kytag06_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = KYTAG06_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    kytag06_class* a_this = (kytag06_class*)i_this;

    cPhs_State phase_state;
    if (dComIfGs_isSymbol(0 /* dSymbol_NAYRU_e */)) {
        phase_state = cPhs_ERROR_e;
    } else {
        a_this->field_0x294 = 0.0f;
        phase_state = cPhs_COMPLEATE_e;
    }

    return phase_state;
}
VERIFY(0x021B0E78, daKytag06_Create);

/* 021B0F1C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag06_cpp() {
    WWHD_FUNC(0x021B0F1C, void, (u32)0);
    sinit_header_statics(0x10464D70, 0x101B8D80);
}
VERIFY(0x021B0F1C, __sinit_d_a_kytag06_cpp);

/* 021B0FB0: kytag06_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag06_class_dt(kytag06_class* i_this, s32 flags) {
    WWHD_FUNC(0x021B0FB0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B0FB0, kytag06_class_dt);
