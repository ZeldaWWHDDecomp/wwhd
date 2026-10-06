/**
 * d_a_kytag04.cpp (WWHD)
 * Environment tag 04: switches the colour pattern (palette) when a switch turns on/off.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag04.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG04_VTBL 0x10013B64 /* kytag04_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dStage_roomControl_c::mStayNo (s8 at 0x1047E6C8) */
static inline s8 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* 0255FD48 dKy_change_colpat(u8) */
static inline void dKy_change_colpat(u8 pat) { gabi::call(0x0255FD48, pat); }

struct kytag04_class : fopAc_ac_c {
    /* 0x3AC */ be<u8> mState;
    /* 0x3AD */ be<u8> mOffColPat;
    /* 0x3AE */ be<u8> mOnColPat;
    /* 0x3AF */ be<u8> mSwitchNo;
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<s32> mTimerThreshold;
    /* 0x3B8 */ be<f32> mScaleX;
    /* 0x3BC */ be<f32> mScaleY;
};
WWHD_SIZE(kytag04_class, 0x3C0);

/* 021B0464 */
static BOOL daKytag04_Draw(kytag04_class*) {
    WWHD_FUNC(0x021B0464, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0464, daKytag04_Draw);

/* 021B046C */
static BOOL daKytag04_Execute(kytag04_class* i_this) {
    WWHD_FUNC(0x021B046C, BOOL, i_this);
    if (i_this->mSwitchNo != 0xff && dComIfGs_isSwitch(i_this->mSwitchNo, dComIfGp_roomControl_getStayNo())) {
        if (i_this->mState == 0) {
            if (i_this->mTimer >= i_this->mTimerThreshold) {
                i_this->mTimer = 0;
                dKy_change_colpat(i_this->mOnColPat);
                i_this->mState = 1;
            } else {
                i_this->mTimer = i_this->mTimer + 1;
            }
        }
    } else if (i_this->mState == 1) {
        dKy_change_colpat(i_this->mOffColPat);
        i_this->mState = 0;
    }

    return TRUE;
}
VERIFY(0x021B046C, daKytag04_Execute);

/* 021B0530 */
static BOOL daKytag04_IsDelete(kytag04_class* i_this) {
    WWHD_FUNC(0x021B0530, BOOL, i_this);
    dKy_change_colpat(i_this->mOffColPat);
    return TRUE;
}
VERIFY(0x021B0530, daKytag04_IsDelete);

/* 021B0558 */
static BOOL daKytag04_Delete(kytag04_class*) {
    WWHD_FUNC(0x021B0558, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0558, daKytag04_Delete);

/* 021B0560 */
static cPhs_State daKytag04_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B0560, cPhs_State, i_this);
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */
    /* fopAcM_ct(i_this, kytag04_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = KYTAG04_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    kytag04_class* a_this = (kytag04_class*)i_this;
    a_this->mState = 0;
    a_this->mOffColPat = fopAcM_GetParam(a_this) & 0xFF;
    a_this->mOnColPat = (fopAcM_GetParam(a_this) >> 8) & 0xFF;
    a_this->mSwitchNo = i_this->current.angle.x;
    a_this->mScaleX = i_this->scale.x * 100.0f;
    a_this->mScaleY = i_this->scale.y * 100.0f;
    a_this->mTimer = 0;
    a_this->mTimerThreshold = 5;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021B0560, daKytag04_Create);

/* 021B0620: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag04_cpp() {
    WWHD_FUNC(0x021B0620, void, (u32)0);
    sinit_header_statics(0x10464D38, 0x101B8C98);
}
VERIFY(0x021B0620, __sinit_d_a_kytag04_cpp);

/* 021B06B4: kytag04_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag04_class_dt(kytag04_class* i_this, s32 flags) {
    WWHD_FUNC(0x021B06B4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B06B4, kytag04_class_dt);
