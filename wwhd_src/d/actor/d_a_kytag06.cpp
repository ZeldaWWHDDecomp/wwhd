/**
 * d_a_kytag06.cpp (WWHD)
 * Environment tag 06: rain and thunder during the "ARRIVAL_BRK" event (time runs on).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag06.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kytag06.h"

// HD: private views of the verified save and environment members.
using kytag06_save_ptr = gptr<kytag06_save_c>;
static kytag06_save_ptr l_save;
#undef g_env_light
#define g_env_light (*(kytag06_env_c*)dKy_getEnvlight())

static BOOL kytag06_dComIfGs_isSymbol(u8 i) {
    return kytag06_isSymbol(&l_save->mCollect, i);
}
static void dComIfGs_setDate(u16 date) { l_save->mDate = date; }
static BOOL dComIfGp_evmng_startCheckOld(const char* name) {
    return kytag06_startCheckOld(dComIfGp_getPEvtManager(), name);
}

/* 021B0D3C */
static BOOL daKytag06_Draw(kytag06_class*) {
    return TRUE;
}

/* 021B0D44 */
static BOOL daKytag06_Execute(kytag06_class* i_this) {
    f32 time;
    int date;

    if (dComIfGp_event_runCheck() == FALSE) {
        return TRUE;
    }
    if (!dComIfGp_evmng_startCheckOld("ARRIVAL_BRK")) {
        return TRUE;
    }

    // HD: time and date are read through the same save pointer snapshot.
    kytag06_save_c* save = l_save;
    time = save->mTime;
    date = save->mDate;
    if (time >= 90.0f && time < 345.0f) {
        time += 0.05f;
    }
    save->mTime = time;
    dComIfGs_setDate(date);

    dKy_change_colpat(1);
    date = gabi::ftoi(i_this->field_0x294 * 250.0f);
    if (date > g_env_light.mRainCount) {
        dKyw_rain_set(date);
    }
    g_env_light.mThunderEff.mMode = 1;
    cLib_addCalc(&i_this->field_0x294, 1.0f, 0.1f, 0.001f, 0.0001f);
    return TRUE;
}

/* 021B0E68 */
static BOOL daKytag06_IsDelete(kytag06_class*) {
    return TRUE;
}

/* 021B0E70 */
static BOOL daKytag06_Delete(kytag06_class*) {
    return TRUE;
}

/* 021B0E78 */
static cPhs_State daKytag06_Create(fopAc_ac_c* i_this) {
    // HD: a discarded environment accessor call precedes construction.
    dKy_getEnvlight();
    fopAcM_ct((kytag06_class*)i_this, kytag06_class);
    kytag06_class* a_this = (kytag06_class*)i_this;

    cPhs_State phase_state;
    if (kytag06_dComIfGs_isSymbol(dSymbol_NAYRU_e)) {
        phase_state = cPhs_ERROR_e;
    } else {
        a_this->field_0x294 = 0.0f;
        phase_state = cPhs_COMPLEATE_e;
    }

    return phase_state;
}

/* 021B0F1C */
// HD: translation-unit static initialisation.
static void __sinit_d_a_kytag06_cpp() {
    sinit_header_statics(0x10464D70, 0x101B8D80);
}

/* 021B0FB0 */
// HD: actor deleting destructor.
static void kytag06_class_dt(kytag06_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

kytag06_class::kytag06_class() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* WWHD glue (tools/verify/autoglue.py): private bindings
str 10013C34 "ARRIVAL_BRK"
data l_save 101F84DC
ext 0255FD48 void dKy_change_colpat(u8)
ext 0257E7C0 void dKyw_rain_set(s32)
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 025B7D90 BOOL kytag06_isSymbol(void*, u8)
ext 025445B8 BOOL kytag06_startCheckOld(void*, const char*)
*/
