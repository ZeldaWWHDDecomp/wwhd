/**
 * d_a_obj_rflw.cpp (WWHD)
 * Object - Rflw (Rito flower: swings when something bumps into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_rflw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

/* String literals are not pooled: each use of "Rflw" has its own address */
#define ARC_CREATE STR(0x1002ECC4) /* "Rflw" (dComIfG_resLoad) */
#define ARC_HEAP STR(0x1002ED04)   /* "Rflw" (getObjectRes) */
#define ARC_DELETE STR(0x1002ED50) /* "Rflw" (resDelete) */
#define SAFESTRING_VTBL 0x1002ECCC
#define RFLW_VTBL 0x1002ECF4
#define RFLW_AAB_VTBL 0x1002ECE4
#define l_cyl_src 0x101CC820

enum { dRes_INDEX_RFLW_BDL_PHANA_e = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint matrix block at
 * model+0x2C {+4 u16 flags (0x10: dirty), +0x10 Mtx34* matrices}; model data at +0xAC; user area +0xB8
 * (copied from d_a_lwood) */
static inline u32 j3dSys_getModel() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* J3DModel_getAnmMtx(u32 model, u32 jnt) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
static inline void J3DModel_setAnmMtx(u32 model, u32 jnt, Mtx34* src) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30), src);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
static inline void J3DModelData_setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n)
        p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}

struct daObjRflw_c : fopAc_ac_c {
    cPhs_State _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();

    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<s16> field_0x408;
    /* 0x526 */ be<s16> field_0x40A;
    /* 0x528 */ be<s16> mHitTimer;
    /* 0x52A */ be<s16> field_0x40E;
    /* 0x52C */ be<u8> field_0x410;
    /* 0x52D */ u8 _52D[3];
};
WWHD_OFFSET(daObjRflw_c, mCyl, 0x3F4);
WWHD_OFFSET(daObjRflw_c, field_0x410, 0x52C);
WWHD_SIZE(daObjRflw_c, 0x530);

/* 02387A74 */
static BOOL nodeCallBack(J3DNode* node, s32 calcTiming) {
    WWHD_FUNC(0x02387A74, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
        u32 model = j3dSys_getModel();
        daObjRflw_c* plant = gabi::at<daObjRflw_c>(gabi::load<u32>(model + 0xB8));
        if (plant != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), calc_mtx());
            cMtx_XrotM(calc_mtx(), plant->field_0x40E);
            cMtx_YrotM(calc_mtx(), plant->field_0x408);
            cMtx_XrotM(calc_mtx(), (s16)-plant->field_0x40E);
            J3DModel_setAnmMtx(model, jntNo, calc_mtx());
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x02387A74, nodeCallBack);

/* 02387BB8 */
BOOL daObjRflw_c::CreateHeap() {
    WWHD_FUNC(0x02387BB8, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(ARC_HEAP, dRes_INDEX_RFLW_BDL_PHANA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xAA, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1002ED14), 0xAA, STR(0x1002ED28));

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpModel != nullptr) {
        /* HD: the joint is looked up by name (GameCube: strcmp over all joints) */
        u32 nameTab = J3DModelData_getJointName(J3DModel_getModelData(mpModel));
        s32 idx = JUTNameTab_getIndex(nameTab, STR(0x1002ED0C) /* "joint2" */);
        if (idx >= 0)
            J3DModelData_setJointCallBack(J3DModel_getModelData(mpModel), (u16)idx, 0x02387A74 /* nodeCallBack */);
        gabi::store<u32>(gabi::ea(mpModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */
    } else {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02387BB8, &daObjRflw_c::CreateHeap);

/* 02387CCC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02387CCC, BOOL, i_this);
    return ((daObjRflw_c*)i_this)->CreateHeap();
}
VERIFY(0x02387CCC, CheckCreateHeap);

/* 02387CD0 */
void daObjRflw_c::set_mtx() {
    WWHD_FUNC(0x02387CD0, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02387CD0, &daObjRflw_c::set_mtx);

/* 02387DA8 */
void daObjRflw_c::CreateInit() {
    WWHD_FUNC(0x02387DA8, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 1.0f;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(l_cyl_src));
    mCyl.SetStts(&mStts);
    field_0x408 = 0;
    set_mtx();
}
VERIFY(0x02387DA8, &daObjRflw_c::CreateInit);

cPhs_State daObjRflw_c::_create() {
    /* fopAcM_ct(this, daObjRflw_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = RFLW_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, RFLW_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhase, ARC_CREATE);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x02387CCC /* CheckCreateHeap */, 0x0C60))
            return cPhs_ERROR_e;
        CreateInit();
    }
    field_0x410 = 0;
    return phase_state;
}

BOOL daObjRflw_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    /* HD: no simple shadow */
    return TRUE;
}

BOOL daObjRflw_c::_delete() {
    dComIfG_resDelete(&mPhase, ARC_DELETE);
    return TRUE;
}

BOOL daObjRflw_c::_execute() {
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);

    if ((mHitTimer > 0x38 || field_0x410 == 0) && mCyl.ChkCoHit()) {
        fopAc_ac_c* hitAc = mCyl.GetCoHitAc();
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff, &hitAc->current.pos);
        s16 angle = cM_atan2s(diff->x, diff->z);
        field_0x40E = (s16)(angle - 0x4000);
        field_0x410 = 1;
        mHitTimer = 0;
        field_0x40A = 0;
        fopAcM_seStart(this, 0x69E6 /* JA_SE_OBJ_TREE_SWING_S */, 0);
    }

    if (field_0x410 != 0) {
        mHitTimer = mHitTimer + 1;
        if (mHitTimer < 8) {
            field_0x408 = (s16)gabi::ftoi(cM_ssin(field_0x40A) * (f32)(0x100 - mHitTimer) * 4.0f);
            field_0x40A = field_0x40A + 0x1000;
        } else {
            field_0x408 = (s16)gabi::ftoi(cM_ssin(field_0x40A) * (f32)(0x100 - mHitTimer) * 2.0f);
            field_0x40A = field_0x40A + 0x0800;
        }
        if (mHitTimer > 0x100) {
            field_0x410 = 0;
            mHitTimer = 0;
            field_0x408 = 0;
        }
    }

    set_mtx();
    return TRUE;
}

/* 02387E5C */
static cPhs_State daObjRflw_Create(void* i_this) {
    WWHD_FUNC(0x02387E5C, cPhs_State, i_this);
    return ((daObjRflw_c*)i_this)->_create();
}
VERIFY(0x02387E5C, daObjRflw_Create);

/* 02387F9C */
static BOOL daObjRflw_Delete(void* i_this) {
    WWHD_FUNC(0x02387F9C, BOOL, i_this);
    return ((daObjRflw_c*)i_this)->_delete();
}
VERIFY(0x02387F9C, daObjRflw_Delete);

/* 02387FCC */
static BOOL daObjRflw_Draw(void* i_this) {
    WWHD_FUNC(0x02387FCC, BOOL, i_this);
    return ((daObjRflw_c*)i_this)->_draw();
}
VERIFY(0x02387FCC, daObjRflw_Draw);

/* 02388064 */
static BOOL daObjRflw_Execute(void* i_this) {
    WWHD_FUNC(0x02388064, BOOL, i_this);
    return ((daObjRflw_c*)i_this)->_execute();
}
VERIFY(0x02388064, daObjRflw_Execute);

/* 02388310 */
static BOOL daObjRflw_IsDelete(void* i_this) {
    WWHD_FUNC(0x02388310, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02388310, daObjRflw_IsDelete);

/* 02388268 */
static void __sinit_d_a_obj_rflw_cpp() {
    WWHD_FUNC(0x02388268, void, (u32)0);
    sinit_header_statics(0x1046BD40, 0x101CC864);
}
VERIFY(0x02388268, __sinit_d_a_obj_rflw_cpp);

/* 023882FC: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023882FC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023882FC, trivial_dt);

/* 02388318: daObjRflw_c deleting destructor */
static void daObjRflw_c_dt(daObjRflw_c* i_this, s32 flags) {
    WWHD_FUNC(0x02388318, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02388318, daObjRflw_c_dt);

/* 02388384: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02388384, void, p);
}
VERIFY(0x02388384, empty_virtual);
