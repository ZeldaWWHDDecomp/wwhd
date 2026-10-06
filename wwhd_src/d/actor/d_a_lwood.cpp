/**
 * d_a_lwood.cpp (WWHD)
 * Object - Normal tree (model whose leaf joint sways in the wind, moving background collision).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lwood.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x10014228) /* "Lwood" (static const char[]: one address) */
#define SAFESTRING_VTBL 0x1001418C
#define LWOOD_VTBL 0x100141A4

enum {
    dRes_INDEX_LWOOD_BDL_ALWD_e = 4,
    dRes_INDEX_LWOOD_DZB_ALWD_e = 7,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02311F80 daObj::get_wind_spd(actor, f32): HD returns a pointer to a function-local static cXyz */
static inline cXyz* daObj_get_wind_spd(fopAc_ac_c* a, f32 rate) { return gabi::call<cXyz*>(0x02311F80, a, rate); }
/* 024F2478 dBgW_NewSet(cBgD_t*, u32 flag, Mtx34*) -> dBgW* */
static inline dBgW* dBgW_NewSet(void* bgd, u32 flag, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, bgd, flag, mtx); }
/* mDoGph_gInf_c::isMonotone(): HD u8 at 0x101F4829 */
static inline bool mDoGph_isMonotone() { return gabi::load<u8>(0x101F4829) != 0; }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint matrix block at
 * model+0x2C {+4 u16 flags (0x10: dirty), +0x10 Mtx34* matrices}; model data at +0xAC; user area +0xB8 */
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

struct daLwood_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    f32 getYureScale() const { return mScale; }
    s16 getYureTimer() const { return mTimer; }
    void setMoveBGMtx();
    void set_mtx();

    BOOL CreateHeap();
    void CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ Mtx34 mtx;
    /* 0x3EC */ be<s16> mTimer;
    /* 0x3EE */ u8 _3EE[2];
    /* 0x3F0 */ be<f32> mScale;
};
WWHD_OFFSET(daLwood_c, mtx, 0x3BC);
WWHD_OFFSET(daLwood_c, mScale, 0x3F0);

void daLwood_c::setMoveBGMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
}

void daLwood_c::set_mtx() {
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
}

/* 021B6B04 */
BOOL daLwood_c::CreateHeap() {
    WWHD_FUNC(0x021B6B04, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_LWOOD_BDL_ALWD_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xBA, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100141B4), 0xBA, STR(0x100141C4));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mModel = model;
    if (model == nullptr)
        return FALSE;

    gabi::store<u32>(gabi::ea(model) + 0xB8, gabi::ea(this)); /* setUserArea */
    setMoveBGMtx();
    void* bgp = dComIfG_getObjectRes(m_arcname, dRes_INDEX_LWOOD_DZB_ALWD_e, SAFESTRING_VTBL);
    dBgW* bgw = dBgW_NewSet(bgp, 1 /* dBgW::MOVE_BG_e */, &mtx);
    mpBgW = bgw;
    if (bgw == nullptr)
        return FALSE;
    return TRUE;
}
VERIFY(0x021B6B04, &daLwood_c::CreateHeap);

/* 021B6C30 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B6C30, BOOL, i_this);
    return ((daLwood_c*)i_this)->CreateHeap();
}
VERIFY(0x021B6C30, CheckCreateHeap);

/* 021B6C34 */
static BOOL nodeCallBack(J3DNode* joint, s32 calcTiming) {
    WWHD_FUNC(0x021B6C34, BOOL, joint, calcTiming);
    if (mDoGph_isMonotone())
        return TRUE;

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(joint)) + 4);
        u32 model = j3dSys_getModel();
        daLwood_c* i_this = gabi::at<daLwood_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            cXyz* windSpeed = daObj_get_wind_spd(i_this, 100.0f);
            f32 wx = windSpeed->x;
            f32 wz = windSpeed->z;
            s32 t = i_this->getYureTimer() * 300;
            f32 sy = cM_ssin(t);
            s16 r2 = (s16)gabi::ftoi(wx * sy * 10.0f);
            f32 cy = cM_scos(t);
            s16 r0 = (s16)gabi::ftoi(wz * cy * 10.0f);
            s16 r1 = (s16)gabi::ftoi(fabsf(sy + 1.0f) * 250.0f);

            f32 sc = i_this->getYureScale();
            s16 p1 = (s16)gabi::ftoi(sc * r2);
            s16 p2 = (s16)gabi::ftoi(sc * r0);
            s16 p0 = (s16)gabi::ftoi(sc * r1);

            PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), p0, p1, p2);
            J3DModel_setAnmMtx(model, jntNo, mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), j3dSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x021B6C34, nodeCallBack);

/* 021B6E98 */
void daLwood_c::CreateInit() {
    WWHD_FUNC(0x021B6E98, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 2.37f;
    mTimer = (s16)gabi::ftoi(cM_rndF(32768.0f));
    mScale = cM_rndF(0.4f) + 0.8f;
    /* HD: the joint is looked up by name (GameCube: strcmp over all joints) */
    u32 jointName = J3DModelData_getJointName(J3DModel_getModelData(mModel));
    s32 idx = JUTNameTab_getIndex(jointName, STR(0x10014210) /* "J_Alwd_ha" */);
    if (idx >= 0)
        J3DModelData_setJointCallBack(J3DModel_getModelData(mModel), (u16)idx, 0x021B6C34 /* nodeCallBack */);

    J3DModel_calc(mModel);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    set_mtx();
    dBgW_Move(mpBgW);
}
VERIFY(0x021B6E98, &daLwood_c::CreateInit);

cPhs_State daLwood_c::_create() {
    /* fopAcM_ct(this, daLwood_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = LWOOD_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhs, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x021B6C30 /* CheckCreateHeap */, 0x0E40) == 0) {
            ret = cPhs_ERROR_e;
        } else {
            CreateInit();
        }
    }
    return ret;
}

bool daLwood_c::_delete() {
    if (heap != nullptr)
        cBgS_Release(dComIfG_Bgsp(), mpBgW);
    dComIfG_resDelete(&mPhs, m_arcname);
    return TRUE;
}

bool daLwood_c::_execute() {
    mTimer = mTimer + 1;
    return TRUE;
}

bool daLwood_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return TRUE;
}

/* 021B7094 */
static cPhs_State daLwood_Create(void* i_this) {
    WWHD_FUNC(0x021B7094, cPhs_State, i_this);
    return ((daLwood_c*)i_this)->_create();
}
VERIFY(0x021B7094, daLwood_Create);

/* 021B7158 */
static BOOL daLwood_Delete(void* i_this) {
    WWHD_FUNC(0x021B7158, BOOL, i_this);
    return ((daLwood_c*)i_this)->_delete();
}
VERIFY(0x021B7158, daLwood_Delete);

/* 021B71B0 */
static BOOL daLwood_Draw(void* i_this) {
    WWHD_FUNC(0x021B71B0, BOOL, i_this);
    return ((daLwood_c*)i_this)->_draw();
}
VERIFY(0x021B71B0, daLwood_Draw);

/* 021B7248 */
static BOOL daLwood_Execute(void* i_this) {
    WWHD_FUNC(0x021B7248, BOOL, i_this);
    return ((daLwood_c*)i_this)->_execute();
}
VERIFY(0x021B7248, daLwood_Execute);

/* 021B7304 */
static BOOL daLwood_IsDelete(void* i_this) {
    WWHD_FUNC(0x021B7304, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021B7304, daLwood_IsDelete);

/* 021B725C */
static void __sinit_d_a_lwood_cpp() {
    WWHD_FUNC(0x021B725C, void, (u32)0);
    sinit_header_statics(0x10464E6C, 0x101B9154);
}
VERIFY(0x021B725C, __sinit_d_a_lwood_cpp);

/* 021B72F0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021B72F0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021B72F0, trivial_dt);

/* 021B730C: daLwood_c deleting destructor */
static void daLwood_c_dt(daLwood_c* i_this, s32 flags) {
    WWHD_FUNC(0x021B730C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B730C, daLwood_c_dt);

/* 021B7360: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x021B7360, void, p);
}
VERIFY(0x021B7360, empty_virtual);
