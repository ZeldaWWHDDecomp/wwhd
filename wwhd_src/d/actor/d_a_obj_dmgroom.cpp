/**
 * d_a_obj_dmgroom.cpp (WWHD)
 * Object - Damaged room (demo object with a TEV register animation).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_dmgroom.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_dmgroom.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002723C,
    .headerStatics = {0x10469624, 0x101C8B54, 0x10469630},
};

static dDemo_object_c* m_object;

/* 02334608 */
BOOL daObjDmgroom_c::CreateHeap() {
    // HD: the archive name is lowercase.
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("dmgroom", dRes_INDEX_DMGROOM_BDL_DMGROOM_e);
    if (modelData == nullptr) /* JUT_ASSERT(82, modelData != NULL) */
        JUTAssertion::showAssert("d_a_obj_dmgroom.cpp", 82, "modelData != NULL");
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x00, 0x11020203);
    mpModel = model;
    if (model == nullptr) /* HD: tests the returned pointer */
        return FALSE;

    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes("dmgroom", dRes_INDEX_DMGROOM_BRK_DMGROOM_e);
    if (brk == nullptr) /* JUT_ASSERT(92, brk != NULL) */
        JUTAssertion::showAssert("d_a_obj_dmgroom.cpp", 92, "brk != NULL");
    if (!mBrkAnm.init(modelData, brk, true, J3DFrameCtrl::EMode_NONE))
        return FALSE;
    return TRUE;
}

/* 02334724 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObjDmgroom_c*)i_this)->CreateHeap();
}

/* 02334800 */
void daObjDmgroom_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    fopAcM_setCullSizeFar(this, 1.0f);
    set_mtx();
}

/* 02334728 */
void daObjDmgroom_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02334880 */
// HD: daObjDmgroom_Create (_create inlined)
static cPhs_State daObjDmgroom_Create(void* p) {
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    fopAcM_ct(i_this, daObjDmgroom_c);
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, "dmgroom");
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0x1460) == 0) {
            return cPhs_ERROR_e;
        }
        i_this->CreateInit();
    }
    return ret;
}

/* 0233494C */
static BOOL daObjDmgroom_Delete(void* p) {
    dComIfG_resDeleteDemo(&((daObjDmgroom_c*)p)->mPhs, "dmgroom");
    return TRUE;
}

/* 0233497C */
static BOOL daObjDmgroom_Draw(void* p) {
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    g_env_light.setLightTevColorType(i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    i_this->mBrkAnm.entry(i_this->mpModel->getModelData());
    mDoExt_modelUpdateDL(i_this->mpModel);
    i_this->mBrkAnm.remove(i_this->mpModel->getModelData());
    dComIfGd_setList();
    return TRUE;
}

/* 02334A38 */
static BOOL daObjDmgroom_Execute(void* p) {
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    u8 id = i_this->demoActorID;
    if (id != 0) {
        dDemo_actor_c* demoAc = nullptr;
        // HD: the demo actor lookup is inlined, including its range check and assertion.
        if (id <= 0x20) {
            if (m_object == nullptr) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUTAssertion::showAssert("d_demo.h", 0x23A, "m_object != NULL");
            demoAc = m_object->getActor(id);
        }
        if (demoAc != nullptr) {
            if (demoAc->checkEnable(dDemo_actor_c::ENABLE_ANM_FRAME_e))
                i_this->mBrkAnm.setFrame(demoAc->getAnmFrame());
        }
    }
    i_this->set_mtx();
    return TRUE;
}

/* 02334B88 */
static BOOL daObjDmgroom_IsDelete(void* p) {
    return TRUE;
}

/* 02334AE0 */
static void __sinit_d_a_obj_dmgroom_cpp() {
    __sinit_header_statics();
}

/* 02334B74 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02334B90 */
// HD: daObjDmgroom_c deleting destructor (mDoExt_brkAnm has a trivial destructor)
static void daObjDmgroom_c_dt(daObjDmgroom_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02334BE4 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and local bindings
str 10027284 in 02334608
str 1002728C
str 100272A0 "modelData != NULL"
str 100272B4 "brk != NULL" renamed
str 10027230 in 02334880
str 100272D0 in 0233494C
str 10027274
str 10027264 "m_object != NULL"
data m_object 101D5FFC
ext 025E80D0 void mDoExt_brkAnm_ct(void*)
ext 025E8154 BOOL mDoExt_brkAnm_init_dmgroom(void*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s16, s16, s32, s32)
ext 02526E70 dDemo_actor_c* dDemo_object_getActor(u32, u8)
*/
