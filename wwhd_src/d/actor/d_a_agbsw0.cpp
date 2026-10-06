/**
 * d_a_agbsw0.cpp (WWHD)
 * GBA (Tingle Tuner) switch actor. HD has no Tingle Tuner: of the fifteen GameCube trigger types
 * only the bomb-hit switch (the core of GameCube's ExeSubT without the GBA link checks) is left,
 * with a class reduced to the timer, the collision status and the cylinder.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_agbsw0.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define L_CYL_SRC 0x1018F788u    /* l_cyl_src (dCcD_SrcCyl): radius +0x3C, height +0x40 */
#define AGBSW0_VTBL 0x10006D9Cu  /* daAgbsw0_c vtable (HD virtual destructor) */
#define AAB_VTBL 0x10006D8Cu     /* this TU's copy of the cM3dGAab vtable */

/* HD layout: GameCube 0x290..0x29B (mOrigScaleX/Z, field_0x298/299, mNonCircular, field_0x29B)
 * are gone; mTimer (GameCube 0x29C) moves to the start of the actor's own fields. */
struct daAgbsw0_c : fopAc_ac_c {
    BOOL execute();

    u8 getSw0() { return fopAcM_GetParam(this) >> 0x10 & 0xFF; }

    /* 0x3AC */ be<u32> mTimer;
    /* 0x3B0 */ dCcD_Stts mStts;
    /* 0x3EC */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(daAgbsw0_c, mTimer, 0x3AC);
WWHD_OFFSET(daAgbsw0_c, mStts, 0x3B0);
WWHD_OFFSET(daAgbsw0_c, mCyl, 0x3EC);
WWHD_SIZE(daAgbsw0_c, 0x51C);

/* 02048054: execute (inline in GameCube, out of line in HD).
 * HD: the type table is gone; this is ExeSubT without the GBA link: the switch is set on any
 * bomb-type Tg hit (GameCube additionally required a link, a bomb actor in state 8, and sent
 * the GBA a sound), and the delete after the 30-frame delay waits for a running event
 * (GameCube checked the event only while a GBA was linked). */
BOOL daAgbsw0_c::execute() {
    WWHD_FUNC(0x02048054, BOOL, this);
    u8 sw0 = getSw0();

    if (sw0 != 0xFF && fopAcM_isSwitch(this, sw0)) {
        if (mTimer == 0) {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) { /* dComIfGp_event_runCheck() */
                return true;
            }
            fopAcM_delete(this);
        } else {
            mTimer = mTimer - 1;
        }
        return true;
    }

    if (mCyl.ChkTgHit()) {
        dComIfGs_onSwitch(sw0, home.roomNo); /* fopAcM_onSwitch */
        mTimer = 30;
    } else {
        dComIfG_Ccsp_Set(&mCyl);
    }
    return true;
}
VERIFY(0x02048054, &daAgbsw0_c::execute);

/* 02048148 */
static BOOL daAgbsw0_Execute(daAgbsw0_c* i_this) {
    WWHD_FUNC(0x02048148, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x02048148, daAgbsw0_Execute);

/* 0204814C: HD: deleteSub (GBA cursor/flag resets) is gone */
static BOOL daAgbsw0_Delete(daAgbsw0_c* i_this) {
    WWHD_FUNC(0x0204814C, BOOL, i_this);
    return true;
}
VERIFY(0x0204814C, daAgbsw0_Delete);

/* 02048154: fopAcM_ct and create() inlined.
 * HD: create keeps only the type-T path: error when sw0 is unset or already on; the scale is
 * always multiplied by 200 (no type-S 8000) and the reduced fields are not initialised (mTimer
 * relies on the zeroed actor memory). */
static cPhs_State daAgbsw0_Create(fopAc_ac_c* ac) {
    WWHD_FUNC(0x02048154, cPhs_State, ac);
    daAgbsw0_c* i_this = static_cast<daAgbsw0_c*>(ac);
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = AGBSW0_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    u8 sw0 = i_this->getSw0();
    if (sw0 == 0xFF || fopAcM_isSwitch(i_this, sw0)) {
        return cPhs_ERROR_e;
    }

    i_this->scale.x = gabi::fmuls_ppc(i_this->scale.x, 200.0f);
    i_this->scale.y = gabi::fmuls_ppc(i_this->scale.y, 200.0f);
    i_this->scale.z = gabi::fmuls_ppc(i_this->scale.z, 200.0f);
    i_this->shape_angle.x = 0;
    i_this->shape_angle.y = i_this->current.angle.y;
    i_this->shape_angle.z = 0;

    gabi::store<f32>(L_CYL_SRC + 0x3C, i_this->scale.x); /* l_cyl_src.mCylAttr.mCyl.mRadius */
    gabi::store<f32>(L_CYL_SRC + 0x40, i_this->scale.y); /* l_cyl_src.mCylAttr.mCyl.mHeight */
    i_this->mStts.Init(0, 0xFF, i_this);
    i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(L_CYL_SRC));
    i_this->mCyl.SetC(&i_this->current.pos);
    i_this->mCyl.SetStts(&i_this->mStts);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02048154, daAgbsw0_Create);

/* 02048300 */
static void __sinit_d_a_agbsw0_cpp() {
    WWHD_FUNC(0x02048300, void, (u32)0);
    sinit_header_statics(0x10461388, 0x1018F7CC);
}
VERIFY(0x02048300, __sinit_d_a_agbsw0_cpp);

/* 02048394: HD: the map icons (dMap_drawPoint) are gone */
static BOOL daAgbsw0_Draw(daAgbsw0_c* i_this) {
    WWHD_FUNC(0x02048394, BOOL, i_this);
    return true;
}
VERIFY(0x02048394, daAgbsw0_Draw);

/* 0204839C */
static BOOL daAgbsw0_IsDelete(daAgbsw0_c* i_this) {
    WWHD_FUNC(0x0204839C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0204839C, daAgbsw0_IsDelete);

/* 020483A4: daAgbsw0_c deleting destructor (compiler-generated) */
static void daAgbsw0_c_dt(daAgbsw0_c* i_this, s32 flags) {
    WWHD_FUNC(0x020483A4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020483A4, daAgbsw0_c_dt);
