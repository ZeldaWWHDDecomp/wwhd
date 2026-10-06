/**
 * d_a_hot_floor.cpp (WWHD)
 * Heat-floor particles following an aim matrix.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hot_floor.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x10011794 /* HD: daHot_Floor_c vtable */

enum {
    ID_AK_SN_HEATFLOOR00 = 0x8120,
    ID_AK_SN_HEATFLOOR01 = 0x814C,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "mDoExt_brkAnm::init"; HD size 0x78) */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call(0x025E80D0, p); }
/* JPABaseEmitter::becomeInvalidEmitter (HD inline, as d_a_item) */
static inline void JPABaseEmitter_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}
/* JPABaseEmitter::setGlobalRTMatrix (HD inline): JPASetRMtxTVecfromMtx(m, &mGlobalRot (+0x1F0),
 * &mGlobalTrs (+0x22C)) (as d_a_kb.h) */
static inline void JPABaseEmitter_setGlobalRTMatrix(u32 e, Mtx34* m) { gabi::call(0x028249B0, m, e + 0x1F0, e + 0x22C); }

struct daHot_Floor_c : fopAc_ac_c {
    void set_mtx_init();
    void set_mtx();
    cPhs_State CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u32> field_0x298;
    /* 0x3B8 */ be<u32> field_0x29c;
    /* 0x3BC */ u8 mBrkAnm[0x78]; /* mDoExt_brkAnm (HD 0x78) */
    /* 0x434 */ be<f32> mSpawnTimer;
    /* 0x438 */ be<u32> field_0x2bc;
    /* 0x43C */ mDoExt_btkAnm mBtkAnm; /* HD 0x74 */
    /* 0x4B0 */ gptr<JPABaseEmitter> mEmitter1;
    /* 0x4B4 */ gptr<JPABaseEmitter> mEmitter2;
    /* 0x4B8 */ be<u8> mbSpawnParticle;
    /* 0x4B9 */ u8 _4B9[3];
    /* 0x4BC */ Mtx34 mtx[5];
    /* 0x5AC */ gptr<Mtx34> mtx_p;
    /* 0x5B0 */ be<u32> field_0x3d4;
};
WWHD_OFFSET(daHot_Floor_c, mSpawnTimer, 0x434);
WWHD_OFFSET(daHot_Floor_c, mBtkAnm, 0x43C);
WWHD_OFFSET(daHot_Floor_c, mbSpawnParticle, 0x4B8);
WWHD_OFFSET(daHot_Floor_c, mtx_p, 0x5AC);
WWHD_SIZE(daHot_Floor_c, 0x5B4);

/* 0217A0F4 */
void daHot_Floor_c::set_mtx_init() {
    WWHD_FUNC(0x0217A0F4, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    for (s32 i = 0; i < 5; i++)
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx[i]); /* mDoMtx_copy */
}
VERIFY(0x0217A0F4, &daHot_Floor_c::set_mtx_init);

/* 0217A280 */
void daHot_Floor_c::set_mtx() {
    WWHD_FUNC(0x0217A280, void, this);
    for (s32 i = 4; i > 0; i--)
        PSMTXCopy(&mtx[i - 1], &mtx[i]);
    if (mtx_p != nullptr) {
        gabi::Local<cXyz> pos;
        f32 x = cXyz_Zero->x, y = cXyz_Zero->y, z = cXyz_Zero->z; /* cXyz pos = cXyz::Zero */
        pos->x = x;
        pos->y = y;
        pos->z = z;
        PSMTXCopy(mtx_p, mDoMtx_stack_c::get()); /* mDoMtx_stack_c::copy */
        mDoMtx_stack_c::transM(0.0f, 5.0f, -5.0f);
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx[0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), pos.get(), &current.pos); /* mDoMtx_stack_c::multVec */
        if (mEmitter2 != nullptr)
            JPABaseEmitter_setGlobalRTMatrix(gabi::ea(mEmitter2.get()), &mtx[0]);
        if (mEmitter1 != nullptr)
            JPABaseEmitter_setGlobalRTMatrix(gabi::ea(mEmitter1.get()), &mtx[0]);
    }
    mtx_p = nullptr;
}
VERIFY(0x0217A280, &daHot_Floor_c::set_mtx);

/* 0217A178 */
cPhs_State daHot_Floor_c::CreateInit() {
    WWHD_FUNC(0x0217A178, cPhs_State, this);
    mbSpawnParticle = true;
    mSpawnTimer = 0.0f;
    set_mtx_init();
    cullMtx = gabi::ea(&mtx[0]); /* fopAcM_SetMtx */
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0217A178, &daHot_Floor_c::CreateInit);

/* 0217A1C4: daHot_FloorCreate (_create inlined) */
static cPhs_State daHot_FloorCreate(void* i_this) {
    WWHD_FUNC(0x0217A1C4, cPhs_State, i_this);
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    /* fopAcM_ct(this, daHot_Floor_c) */
    if (!fopAcM_CheckCondition(a, fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            fopAc_ac_c_ct(a);
            a->__vtbl = ACT_VTBL;
            mDoExt_brkAnm_ct(a->mBrkAnm);
            mDoExt_btkAnm::ct(&a->mBtkAnm);
        }
        fopAcM_OnCondition(a, fopAcCnd_INIT_e);
    }
    return a->CreateInit();
}
VERIFY(0x0217A1C4, daHot_FloorCreate);

/* 0217A238: daHot_FloorDelete (_delete inlined) */
static BOOL daHot_FloorDelete(void* i_this) {
    WWHD_FUNC(0x0217A238, BOOL, i_this);
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    if (a->mEmitter2 != nullptr)
        JPABaseEmitter_becomeInvalidEmitter(gabi::ea(a->mEmitter2.get()));
    if (a->mEmitter1 != nullptr)
        JPABaseEmitter_becomeInvalidEmitter(gabi::ea(a->mEmitter1.get()));
    return TRUE;
}
VERIFY(0x0217A238, daHot_FloorDelete);

/* 0217A380: daHot_FloorExecute (_execute inlined) */
static BOOL daHot_FloorExecute(void* i_this) {
    WWHD_FUNC(0x0217A380, BOOL, i_this);
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    if (a->mbSpawnParticle) {
        if (a->mEmitter2 == nullptr && !(fopAcM_GetParam(a) & 1))
            a->mEmitter2 = dComIfGp_particle_set(ID_AK_SN_HEATFLOOR01, &a->current.pos);
        if (a->mEmitter1 == nullptr && !(fopAcM_GetParam(a) & 2))
            a->mEmitter1 = dComIfGp_particle_set(ID_AK_SN_HEATFLOOR00, &a->current.pos);
        cLib_chaseF(&a->mSpawnTimer, 60.0f, 5.0f);
        a->mbSpawnParticle = false;
    } else {
        cLib_chaseF(&a->mSpawnTimer, 0.0f, 0.4f);
        if (a->mSpawnTimer < 20.0f) {
            if (a->mEmitter2 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(gabi::ea(a->mEmitter2.get()));
                a->mEmitter2 = nullptr;
            }
            if (a->mEmitter1 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(gabi::ea(a->mEmitter1.get()));
                a->mEmitter1 = nullptr;
            }
        }
        if (!(a->mSpawnTimer > 0.4f))
            fopAcM_delete(a);
    }

    a->set_mtx();
    return FALSE;
}
VERIFY(0x0217A380, daHot_FloorExecute);

/* 0217A55C */
static BOOL daHot_FloorDraw(void* i_this) {
    WWHD_FUNC(0x0217A55C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0217A55C, daHot_FloorDraw);

/* 0217A5F8 */
static BOOL daHot_FloorIsDelete(void* i_this) {
    WWHD_FUNC(0x0217A5F8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0217A5F8, daHot_FloorIsDelete);

/* 0217A564 */
static void __sinit_d_a_hot_floor_cpp() {
    WWHD_FUNC(0x0217A564, void, (u32)0);
    sinit_header_statics(0x10464760, 0x101B7770);
}
VERIFY(0x0217A564, __sinit_d_a_hot_floor_cpp);

/* 0217A600: daHot_Floor_c deleting destructor (the animation members have trivial destructors) */
static void daHot_Floor_c_dt(daHot_Floor_c* i_this, s32 flags) {
    WWHD_FUNC(0x0217A600, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0217A600, daHot_Floor_c_dt);
