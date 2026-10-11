/**
 * d_a_kytag04.cpp (WWHD)
 * Environment tag 04: switches the colour pattern (palette) when a switch turns on/off.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag04.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kytag04.h"
#include "gc_api/f_op/f_op_actor_mng.h"

// HD: room control's current room is shared data, rather than actor-local state.
static s8 mStayNo;
static s8 dComIfGp_roomControl_getStayNo() { return mStayNo; }

/* 021B0464 */
static BOOL daKytag04_Draw(kytag04_class*) {
    return TRUE;
}

/* 021B046C */
static BOOL daKytag04_Execute(kytag04_class* i_this) {
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

/* 021B0530 */
static BOOL daKytag04_IsDelete(kytag04_class* i_this) {
    dKy_change_colpat(i_this->mOffColPat);
    return TRUE;
}

/* 021B0558 */
static BOOL daKytag04_Delete(kytag04_class*) {
    return TRUE;
}

/* 021B0560 */
static cPhs_State daKytag04_Create(fopAc_ac_c* i_this) {
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */
    /* fopAcM_ct(i_this, kytag04_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = kytag04_class::__vt;
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

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 021B0620 */
static void __sinit_d_a_kytag04_cpp() {
    sinit_header_statics(0x10464D38, 0x101B8C98);
}

/* 021B06B4 */
static void kytag04_class_dt(kytag04_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data mStayNo 1047E6C8
ext 0255FD48 void dKy_change_colpat(u8 pat)
*/
