/**
 * d_a_lwood.cpp (WWHD)
 * Object - Normal tree (model whose leaf joint sways in the wind, moving background collision).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lwood.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_lwood.h"
#include "gc_api/f_op/f_op_actor_mng.h"

const gc_tu_data gc_tu = {.safeString = 0x1001418C};
enum { LWOOD_VTBL = 0x100141A4 };
const char daLwood_c::m_arcname[] = "Lwood";
using ModelPointer = gptr<lwood_layout::Model>;
static ModelPointer currentModel;
static Mtx34 currentMtx;
using MonotoneFlag = be<u8>;
static MonotoneFlag monotoneFlag;
// HD: daObj wind speed returns a pointer to static storage instead of a value.
cXyz* get_wind_spd(fopAc_ac_c*, f32);

lwood_layout::ResourceHeader* getJointResource(J3DModelData*);
s32 JUTNameTab_getIndex(void*, const char*);

static Mtx34* getAnmMtx(lwood_layout::Model* model, u32 jnt) {
    lwood_layout::MatrixBlock* block = model->matrixBlock;
    Mtx34* matrices = block->matrices;
    block->flags = block->flags | 0x10;
    return &matrices[jnt];
}
static void setAnmMtx(lwood_layout::Model* model, u32 jnt, Mtx34* src) {
    lwood_layout::MatrixBlock* block = model->matrixBlock;
    block->flags = block->flags | 0x10;
    mtx_copy(&block->matrices[jnt], src);
}
static void* getJointName(J3DModelData* data) {
    lwood_layout::ResourceHeader* header = getJointResource(data);
    u32 offset = header->jointNameOffset;
    // HD: the relative resource pointer must wrap in the 32-bit guest address space.
    return offset != 0 ? gabi::at<void>(gabi::ea(&header->jointNameOffset) + offset) : nullptr;
}
static void setJointCallBack(J3DModelData* data, u16 index, BOOL (*callback)(J3DNode*, s32)) {
    lwood_layout::ModelData* modelData = reinterpret_cast<lwood_layout::ModelData*>(data);
    u32 count = modelData->jointCount;
    lwood_layout::Joint* joint = modelData->joints;
    // HD: an out-of-range index leaves the pointer on the first joint.
    if (index < count)
        joint += index;
    joint->callback = gabi::guest_fn(reinterpret_cast<const void*>(callback));
}

/* inlined into CreateHeap */
void daLwood_c::setMoveBGMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
}

/* inlined into CreateInit */
void daLwood_c::set_mtx() {
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
}

/* 021B6B04 */
BOOL daLwood_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_LWOOD_BDL_ALWD_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0xBA, modelData != NULL) */
        JUT_ASSERT_fail("d_a_lwood.cpp", 0xBA, "modelData != NULL");
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mModel = model;
    if (model == nullptr)
        return FALSE;

    reinterpret_cast<lwood_layout::Model*>(model)->userArea = this;
    setMoveBGMtx();
    cBgD_t* bgp = (cBgD_t*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_LWOOD_DZB_ALWD_e, gc_tu.safeString);
    dBgW* bgw = dBgW_NewSet(bgp, 1 /* dBgW::MOVE_BG_e */, &mtx);
    mpBgW = bgw;
    if (bgw == nullptr)
        return FALSE;
    return TRUE;
}

/* 021B6C30 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daLwood_c*)i_this)->CreateHeap();
}

/* 021B6C34 */
static BOOL nodeCallBack(J3DNode* joint, s32 calcTiming) {
    if (monotoneFlag != 0)
        return TRUE;

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 jntNo = reinterpret_cast<lwood_layout::Joint*>(J3DNode_toJoint(joint))->jointNo;
        lwood_layout::Model* model = currentModel;
        daLwood_c* i_this = model->userArea;
        if (i_this != nullptr) {
            cXyz* windSpeed = get_wind_spd(i_this, 100.0f);
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

            PSMTXCopy(getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), p0, p1, p2);
            setAnmMtx(model, jntNo, mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), &currentMtx);
        }
    }
    return TRUE;
}

/* 021B6E98 */
void daLwood_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 2.37f;
    mTimer = (s16)gabi::ftoi(cM_rndF(32768.0f));
    mScale = cM_rndF(0.4f) + 0.8f;
    // HD: joint-name lookup replaces GameCube's loop over all joints.
    void* jointName = getJointName(J3DModel_getModelData(mModel));
    s32 idx = JUTNameTab_getIndex(jointName, "J_Alwd_ha");
    if (idx >= 0)
        setJointCallBack(J3DModel_getModelData(mModel), (u16)idx, nodeCallBack);

    J3DModel_calc(mModel);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    set_mtx();
    dBgW_Move(mpBgW);
}

/* inlined into daLwood_Create */
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
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x0E40) == 0) {
            ret = cPhs_ERROR_e;
        } else {
            CreateInit();
        }
    }
    return ret;
}

/* inlined into daLwood_Delete */
bool daLwood_c::_delete() {
    if (heap != nullptr)
        cBgS_Release(dComIfG_Bgsp(), mpBgW);
    dComIfG_resDelete(&mPhs, m_arcname);
    return TRUE;
}

/* inlined into daLwood_Execute */
bool daLwood_c::_execute() {
    mTimer = mTimer + 1;
    return TRUE;
}

/* inlined into daLwood_Draw */
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
    return ((daLwood_c*)i_this)->_create();
}

/* 021B7158 */
static BOOL daLwood_Delete(void* i_this) {
    return ((daLwood_c*)i_this)->_delete();
}

/* 021B71B0 */
static BOOL daLwood_Draw(void* i_this) {
    return ((daLwood_c*)i_this)->_draw();
}

/* 021B7248 */
static BOOL daLwood_Execute(void* i_this) {
    return ((daLwood_c*)i_this)->_execute();
}

/* 021B7304 */
static BOOL daLwood_IsDelete(void* i_this) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 021B725C */
static void __sinit_d_a_lwood_cpp() {
    sinit_header_statics(0x10464E6C, 0x101B9154);
}

/* 021B72F0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021B730C */
static void daLwood_c_dt(daLwood_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021B7360 */
static void empty_virtual(void* p) {
}

/* WWHD glue
str 10014228
str 100141B4
str 100141C4 "modelData != NULL"
str 10014210
ext 027F68FC lwood_layout::ResourceHeader* getJointResource(J3DModelData*)
ext 027DF9B0 s32 JUTNameTab_getIndex(void*, const char*)
ext 02311F80 cXyz* get_wind_spd(fopAc_ac_c*, f32)
data currentModel 104B462C
data currentMtx 104B4868
data monotoneFlag 101F4829
*/
