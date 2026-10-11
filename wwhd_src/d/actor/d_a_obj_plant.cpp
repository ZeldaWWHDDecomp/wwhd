/**
 * d_a_obj_plant.cpp (WWHD)
 * Object - Plant (a bush that swings when something bumps into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_plant.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_plant.h"
#include "gc_api/f_op/f_op_actor_mng.h"

const gc_tu_data gc_tu = {.safeString = 0x1002EB1C};
enum { PLANT_VTBL = 0x1002EB44, PLANT_AAB_VTBL = 0x1002EB34 };
static dCcD_SrcCyl l_cyl_src;
using ModelPointer = gptr<plant_layout::Model>;
static ModelPointer currentModel;
static Mtx34 currentMtx;

plant_layout::ResourceHeader* getJointResource(J3DModelData*);
s32 JUTNameTab_getIndex(void*, const char*);

static Mtx34* getAnmMtx(plant_layout::Model* model, u32 jnt) {
    plant_layout::MatrixBlock* block = model->matrixBlock;
    Mtx34* matrices = block->matrices;
    block->flags = block->flags | 0x10;
    return &matrices[jnt];
}
static void setAnmMtx(plant_layout::Model* model, u32 jnt, Mtx34* src) {
    plant_layout::MatrixBlock* block = model->matrixBlock;
    block->flags = block->flags | 0x10;
    mtx_copy(&block->matrices[jnt], src);
}
static void* getJointName(J3DModelData* data) {
    plant_layout::ResourceHeader* header = getJointResource(data);
    u32 offset = header->jointNameOffset;
    // HD: the relative resource pointer must wrap in the 32-bit guest address space.
    return offset != 0 ? gabi::at<void>(gabi::ea(&header->jointNameOffset) + offset) : nullptr;
}
static void setJointCallBack(J3DModelData* data, u16 index, BOOL (*callback)(J3DNode*, s32)) {
    plant_layout::ModelData* modelData = reinterpret_cast<plant_layout::ModelData*>(data);
    u32 count = modelData->jointCount;
    plant_layout::Joint* joint = modelData->joints;
    // HD: an out-of-range index leaves the pointer on the first joint.
    if (index < count)
        joint += index;
    joint->callback = gabi::guest_fn(reinterpret_cast<const void*>(callback));
}

/* 023861E0 */
static BOOL nodeCallBack(J3DNode* node, s32 calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 jntNo = reinterpret_cast<plant_layout::Joint*>(J3DNode_toJoint(node))->jointNo;
        plant_layout::Model* model = currentModel;
        daObjPlant_c* plant = model->userArea;
        if (plant != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            cMtx_XrotM(calc_mtx(), plant->field_0x40E);
            cMtx_YrotM(calc_mtx(), plant->field_0x408);
            cMtx_XrotM(calc_mtx(), (s16)-plant->field_0x40E);
            setAnmMtx(model, jntNo, calc_mtx());
            PSMTXCopy(calc_mtx(), &currentMtx);
        }
    }
    return TRUE;
}

/* 02386324 */
BOOL daObjPlant_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Plant", dRes_INDEX_PLANT_BDL_YRMWD_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0xAA, modelData != NULL) */
        JUT_ASSERT_fail("d_a_obj_plant.cpp", 0xAA, "modelData != NULL");

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpModel != nullptr) {
        // HD: the joint is looked up by name (GameCube: strcmp over all joints)
        void* nameTab = getJointName(J3DModel_getModelData(mpModel));
        s32 idx = JUTNameTab_getIndex(nameTab, "joint2");
        if (idx >= 0)
            setJointCallBack(J3DModel_getModelData(mpModel), (u16)idx, nodeCallBack);
        reinterpret_cast<plant_layout::Model*>(mpModel.get())->userArea = this;
    } else {
        return FALSE;
    }
    return TRUE;
}

/* 02386438 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObjPlant_c*)i_this)->CreateHeap();
}

/* 0238643C */
void daObjPlant_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 02386514 */
void daObjPlant_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 1.0f;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    field_0x408 = 0;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    eyePos.x = x;
    eyePos.y = y + 150.0f;
    eyePos.z = z;
    set_mtx();
}

/* inlined into daObjPlant_Create */
cPhs_State daObjPlant_c::_create() {
    /* fopAcM_ct(this, daObjPlant_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = PLANT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, PLANT_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhase, "Plant");
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x0D20))
            return cPhs_ERROR_e;
        CreateInit();
    }
    field_0x410 = 0;
    return phase_state;
}

/* inlined into daObjPlant_Draw */
BOOL daObjPlant_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    // HD: no simple shadow
    return TRUE;
}

/* inlined into daObjPlant_Delete */
BOOL daObjPlant_c::_delete() {
    dComIfG_resDelete(&mPhase, "Plant");
    return TRUE;
}

/* inlined into daObjPlant_Execute */
BOOL daObjPlant_c::_execute() {
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);

    if ((mHitTimer > 0x38 || field_0x410 == 0) && mCyl.ChkCoHit()) {
        fopAc_ac_c* hitAc = mCyl.GetCoHitAc();
        cXyz diff = current.pos - hitAc->current.pos;
        s16 angle = cM_atan2s(diff.x, diff.z);
        field_0x40E = (s16)(angle - 0x4000);
        field_0x410 = 1;
        mHitTimer = 0;
        field_0x40A = 0;
        fopAcM_seStart(this, JA_SE_OBJ_TREE_SWING_S, 0);
    }

    if (field_0x410 != 0) {
        mHitTimer = mHitTimer + 1;
        if (mHitTimer < 8) {
            field_0x408 = (s16)gabi::ftoi(cM_ssin(field_0x40A) * (f32)(0x100 - mHitTimer) * 32.0f);
            field_0x40A = field_0x40A + 0x1000;
        } else {
            field_0x408 = (s16)gabi::ftoi(cM_ssin(field_0x40A) * (f32)(0x100 - mHitTimer) * 16.0f);
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

/* 023865EC */
static cPhs_State daObjPlant_Create(void* i_this) {
    return ((daObjPlant_c*)i_this)->_create();
}

/* 0238672C */
static BOOL daObjPlant_Delete(void* i_this) {
    return ((daObjPlant_c*)i_this)->_delete();
}

/* 0238675C */
static BOOL daObjPlant_Draw(void* i_this) {
    return ((daObjPlant_c*)i_this)->_draw();
}

/* 023867F4 */
static BOOL daObjPlant_Execute(void* i_this) {
    return ((daObjPlant_c*)i_this)->_execute();
}

/* 02386AA8 */
static BOOL daObjPlant_IsDelete(void* i_this) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit

/* 02386A00 */
static void __sinit_d_a_obj_plant_cpp() {
    sinit_header_statics(0x1046BCD0, 0x101CC6A4);
}

/* 02386A94 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02386AB0 */
static void daObjPlant_c_dt(daObjPlant_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02386B1C */
static void empty_virtual(void* p) {
}

/* WWHD glue
str 1002EB14 in 023865EC
str 1002EB54 in 02386324
str 1002EBA4 in 0238672C
str 1002EB64
str 1002EB78 "modelData != NULL"
str 1002EB5C
ext 027F68FC plant_layout::ResourceHeader* getJointResource(J3DModelData*)
ext 027DF9B0 s32 JUTNameTab_getIndex(void*, const char*)
data l_cyl_src 101CC660
data currentModel 104B462C
data currentMtx 104B4868
*/
