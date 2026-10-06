/**
 * d_a_hitobj.cpp (WWHD)
 * Hit object: a short-lived water-type attack sphere (3 frames).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hitobj.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define HITOBJ_VTBL 0x10011324 /* hitobj_class vtable (HD virtual destructor) */
#define cc_sph_src gabi::at<dCcD_SrcSph>(0x101B7020)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025DAD48 fopKyM_Delete(void*) */
static inline BOOL fopKyM_Delete(void* p) { return gabi::call<BOOL>(0x025DAD48, p); }

struct hitobj_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u8> mUnusedParam;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<s16> mTimer;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
};
WWHD_OFFSET(hitobj_class, mStts, 0x3B8);
WWHD_OFFSET(hitobj_class, mSph, 0x3F4);
WWHD_SIZE(hitobj_class, 0x520);

/* 02175148 */
static BOOL daHitobj_Draw(hitobj_class* i_this) {
    WWHD_FUNC(0x02175148, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02175148, daHitobj_Draw);

/* 02175150 */
static BOOL daHitobj_Execute(hitobj_class* i_this) {
    WWHD_FUNC(0x02175150, BOOL, i_this);
    if (i_this->mTimer != 0) {
        i_this->mTimer -= 1;
        i_this->mSph.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
    } else {
        fopKyM_Delete(i_this);
    }
    return TRUE;
}
VERIFY(0x02175150, daHitobj_Execute);

/* 021751C0 */
static BOOL daHitobj_IsDelete(hitobj_class* i_this) {
    WWHD_FUNC(0x021751C0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021751C0, daHitobj_IsDelete);

/* 021751C8: HD: no dComIfG_resDeleteDemo (the "Hitobj" archive is not loaded) */
static BOOL daHitobj_Delete(hitobj_class* i_this) {
    WWHD_FUNC(0x021751C8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021751C8, daHitobj_Delete);

/* 021751D0 */
static cPhs_State daHitobj_Create(fopAc_ac_c* pActor) {
    WWHD_FUNC(0x021751D0, cPhs_State, pActor);
    hitobj_class* i_this = (hitobj_class*)pActor;
    dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0): unused, only the accessor call remains */

    /* fopAcM_ct(i_this, hitobj_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = HITOBJ_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            gabi::call(0x025166F0, &i_this->mSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    /* HD: no dComIfG_resLoad(&mPhs, "Hitobj"): always complete */
    i_this->mUnusedParam = fopAcM_GetParam(i_this) & 0xFF;
    i_this->mStts.Init(0xFF, 0xFF, i_this);
    i_this->mSph.Set(cc_sph_src);
    i_this->mSph.SetStts(&i_this->mStts);
    i_this->mTimer = 3;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021751D0, daHitobj_Create);

/* 021752A8: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_hitobj_cpp() {
    WWHD_FUNC(0x021752A8, void, (u32)0);
    sinit_header_statics(0x1046470C, 0x101B7060);
}
VERIFY(0x021752A8, __sinit_d_a_hitobj_cpp);

/* 0217533C: hitobj_class deleting destructor (compiler-generated, HD virtual destructor) */
static void hitobj_class_dt(hitobj_class* i_this, s32 flags) {
    WWHD_FUNC(0x0217533C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0217533C, hitobj_class_dt);
