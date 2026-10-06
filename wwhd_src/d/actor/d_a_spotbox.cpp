/**
 * d_a_spotbox.cpp (WWHD)
 * Spot box (a box-shaped volume for the GameCube's spot-light model).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_spotbox.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SPOTBOX_VTBL 0x1003D0EC /* daSpotbox_c vtable (HD virtual destructor) */

struct daSpotbox_c : fopAc_ac_c {
    u32 getType() { return fopAcM_GetParam(this) & 1; }
    /* 0x3AC */ Mtx34 mMtx;
};
WWHD_OFFSET(daSpotbox_c, mMtx, 0x3AC);
WWHD_SIZE(daSpotbox_c, 0x3DC);

/* 0248AA64: HD: draws nothing (GameCube: dComIfGd_setSpotModel(TYPE_CUBE, mMtx, 0x20)) */
static BOOL daSpotbox_Draw(daSpotbox_c* i_this) {
    WWHD_FUNC(0x0248AA64, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0248AA64, daSpotbox_Draw);

/* 0248A7F0: execute() inlined */
static BOOL daSpotbox_Execute(daSpotbox_c* i_this) {
    WWHD_FUNC(0x0248A7F0, BOOL, i_this);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_stack_c::scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mMtx);
    return TRUE;
}
VERIFY(0x0248A7F0, daSpotbox_Execute);

/* 0248A864 */
static BOOL daSpotbox_IsDelete(daSpotbox_c* i_this) {
    WWHD_FUNC(0x0248A864, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0248A864, daSpotbox_IsDelete);

/* 0248A86C: the destructor call (GameCube) is gone */
static BOOL daSpotbox_Delete(daSpotbox_c* i_this) {
    WWHD_FUNC(0x0248A86C, BOOL, i_this);
    return true;
}
VERIFY(0x0248A86C, daSpotbox_Delete);

/* 0248A874: create() inlined */
static cPhs_State daSpotbox_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0248A874, cPhs_State, i_this);
    daSpotbox_c* a_this = (daSpotbox_c*)i_this;
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SPOTBOX_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    f32 baseScale = a_this->getType() != 0 ? 1000.0f : 100.0f;
    i_this->scale.x *= baseScale;
    i_this->scale.y *= baseScale;
    i_this->current.pos.y = gabi::fmadds(0.5f, i_this->scale.y, i_this->current.pos.y);  /* += scale.y * 0.5f */
    i_this->scale.z *= (baseScale * 1.2f);
    i_this->cullMtx = gabi::ea(&a_this->mMtx);  /* fopAcM_SetMtx */
    /* HD: explicit unit cull box (scaled by the matrix) */
    fopAcM_setCullSizeBox(i_this, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0248A874, daSpotbox_Create);

/* 0248A9D0 */
static void __sinit_d_a_spotbox_cpp() {
    WWHD_FUNC(0x0248A9D0, void, (u32)0);
    sinit_header_statics(0x1046DF38, 0x101D0B38);
}
VERIFY(0x0248A9D0, __sinit_d_a_spotbox_cpp);

/* 0248AA6C: daSpotbox_c deleting destructor (compiler-generated) */
static void daSpotbox_c_dt(daSpotbox_c* i_this, s32 flags) {
    WWHD_FUNC(0x0248AA6C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0248AA6C, daSpotbox_c_dt);
