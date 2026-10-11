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
#include "d/actor/d_a_agbsw0.h"

// HD: this translation unit's copies of compiler-generated data.
const gc_tu_data gc_tu = {
    .headerStatics = {0x10461388, 0x1018F7CC, 0},
};

static daAgbsw0_c::CylinderSource l_cyl_src;

/* 02048054 */
// HD: execute is out of line, and only ExeSubT remains. Any bomb-type Tg hit sets the switch;
// the GBA link and bomb actor state checks are gone. Deletion always waits for a running event.
BOOL daAgbsw0_c::execute() {
    u8 sw0 = getSw0();

    if (sw0 != 0xFF && fopAcM_isSwitch(this, sw0)) {
        if (mTimer == 0) {
            if (dComIfGp_event_runCheck()) {
                return true;
            }
            fopAcM_delete(this);
        } else {
            mTimer -= 1;
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

/* 02048148 */
static BOOL daAgbsw0_Execute(daAgbsw0_c* i_this) {
    return i_this->execute();
}

/* 0204814C */
// HD: deleteSub (GBA cursor/flag resets) is gone.
static BOOL daAgbsw0_Delete(daAgbsw0_c* i_this) {
    return true;
}

/* inlined into daAgbsw0_Create (fopAcM_ct) */
daAgbsw0_c::daAgbsw0_c() {
    // HD: GHS explicitly constructs the base and collision members.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, __aab_vt);
}

/* 02048154 */
static cPhs_State daAgbsw0_Create(fopAc_ac_c* i_this) {
    fopAcM_ct(i_this, daAgbsw0_c);
    return static_cast<daAgbsw0_c*>(i_this)->create();
}

/* inlined into daAgbsw0_Create */
cPhs_State daAgbsw0_c::create() {
    // HD: only type T remains; the zeroed actor memory supplies the initial timer.
    u8 sw0 = getSw0();
    if (sw0 == 0xFF || fopAcM_isSwitch(this, sw0)) {
        return cPhs_ERROR_e;
    }

    scale.x = gabi::fmuls_ppc(scale.x, 200.0f);
    scale.y = gabi::fmuls_ppc(scale.y, 200.0f);
    scale.z = gabi::fmuls_ppc(scale.z, 200.0f);
    shape_angle.x = 0;
    shape_angle.y = current.angle.y;
    shape_angle.z = 0;

    l_cyl_src.mCylAttr.mCyl.mRadius = scale.x;
    l_cyl_src.mCylAttr.mCyl.mHeight = scale.y;
    mStts.Init(0, 0xFF, this);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl*>(&l_cyl_src));
    mCyl.SetC(&current.pos);
    mCyl.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}

/* 02048394 */
// HD: the map icons (dMap_drawPoint) are gone.
static BOOL daAgbsw0_Draw(daAgbsw0_c* i_this) {
    return true;
}

/* 0204839C */
static BOOL daAgbsw0_IsDelete(daAgbsw0_c* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 020483A4 */
/* daAgbsw0_c deleting destructor (compiler-generated) */
static void daAgbsw0_c_dt(daAgbsw0_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02048300 */
static void __sinit_d_a_agbsw0_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_cyl_src 1018F788
*/
