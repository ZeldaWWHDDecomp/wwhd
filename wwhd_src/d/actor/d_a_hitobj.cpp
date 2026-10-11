/**
 * d_a_hitobj.cpp (WWHD)
 * Hit object: a short-lived water-type attack sphere (3 frames).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hitobj.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_hitobj.h"

static dCcD_SrcSph cc_sph_src;

/* 02175148 */
static BOOL daHitobj_Draw(hitobj_class* i_this) {
    return TRUE;
}

/* 02175150 */
static BOOL daHitobj_Execute(hitobj_class* i_this) {
    if (i_this->mTimer != 0) {
        i_this->mTimer -= 1;
        i_this->mSph.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
    } else {
        fopKyM_Delete(i_this);
    }
    return TRUE;
}

/* 021751C0 */
static BOOL daHitobj_IsDelete(hitobj_class* i_this) {
    return TRUE;
}

/* 021751C8 */
// HD: the Hitobj archive is not loaded, so no resource deletion is needed.
static BOOL daHitobj_Delete(hitobj_class* i_this) {
    return TRUE;
}

/* 021751D0 */
static cPhs_State daHitobj_Create(fopAc_ac_c* pActor) {
    hitobj_class* i_this = (hitobj_class*)pActor;
    dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0): unused, only the accessor call remains */

    fopAcM_ct(i_this, hitobj_class);

    /* HD: no dComIfG_resLoad(&mPhs, "Hitobj"): always complete */
    i_this->mUnusedParam = fopAcM_GetParam(i_this) & 0xFF;
    i_this->mStts.Init(0xFF, 0xFF, i_this);
    i_this->mSph.Set(&cc_sph_src);
    i_this->mSph.SetStts(&i_this->mStts);
    i_this->mTimer = 3;
    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 021752A8 */
static void __sinit_d_a_hitobj_cpp() {
    sinit_header_statics(0x1046470C, 0x101B7060);
}

/* 0217533C */
// HD: deleting destructor for the collision members and actor base.
static void hitobj_class_dt(hitobj_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Sph_destroy(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* inlined into daHitobj_Create (fopAcM_ct) */
hitobj_class::hitobj_class() {
    // HD: GHS constructs the base and collision members explicitly.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    dCcD_Stts_ct(&mStts);
    dCcD_Sph_construct(&mSph);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data cc_sph_src 101B7020
ext 025DAD48 BOOL fopKyM_Delete(void*)
ext 025166F0 void dCcD_Sph_construct(dCcD_Sph*)
ext 02515AE8 void dCcD_Sph_destroy(dCcD_Sph*, int)
*/
