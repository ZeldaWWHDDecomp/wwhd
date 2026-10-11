/**
 * d_a_obj_demo_barrel.cpp (WWHD)
 * Object - Demo barrel (the barrel Link escapes Forsaken Fortress in; breaks and splashes in the demo).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_demo_barrel.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_demo_barrel.h"

const gc_tu_data gc_tu = {
    .safeString = 0x100271BC,
    .headerStatics = {0x104695F8, 0x101C8AE0, 0x10469604},
};

const char daObj_Demo_Barrel_c::M_arcname[] = "DBarrel";
static dDemo_object_c* m_object;

/* 02333FC4 */
void daObj_Demo_Barrel_c::setParticleHahen() {
    const GXColor* k0 = &tevStr.mColorK0;
    DemoBarrelEmitter* emitter = (DemoBarrelEmitter*)dComIfGp_getParticle()->set(
        0, dPa_name::ID_IT_JN_TR_HAHEN_A, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, k0, k0, nullptr);
    if (emitter != nullptr) {
        static cXyz em_scl(1.0f, 0.8f, 1.0f);
        emitter->setEmitterScale(em_scl);
    }
}

/* 02334098 */
void daObj_Demo_Barrel_c::setParticleSibuki() {
    cXyz sp18;
    f32 py = current.pos.y, px = current.pos.x;
    m2D0.y = py;
    m2D0.x = px;
    f32 pz = current.pos.z;
    sp18.x = px;
    m2D0.z = pz;
    sp18.z = pz;
    sp18.y = 100.0f;
    m2D0.y = dBgS_ObjGndChk_Wtr_Func(&sp18);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_JN_WP_HAMON01, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_JN_WP_HAMON02, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_SN_DEMO_HAMON_S, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_SN_DEMO_SUIMEN_A, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_SN_MIZUBASHIRA2, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_SN_DEMO_SHIBUKI_A, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dComIfGp_getParticle()->set(0, dPa_name::ID_IT_SN_DEMO_SHIBUKI_B, &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 02333CF4 */
BOOL daObj_Demo_Barrel_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BDL_KTARU_02_e);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BCK_02_TR_CD_e);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                  nullptr, 0, 0x11020203);
    mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BCK_02_TR_CD_e);
    mpMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    morf = mpMorf;
    morf->setFrame(morf->getEndFrame() - 1.0f);
    mpModel = mpMorf->getModel();
    return TRUE;
}

/* 02333EB0 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    return ((daObj_Demo_Barrel_c*)a_this)->CreateHeap();
}

/* 02333EB4 */
// HD: daObj_Demo_BarrelCreate (_create inlined)
static cPhs_State daObj_Demo_BarrelCreate(void* v_this) {
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    fopAcM_ct(i_this, daObj_Demo_Barrel_c);
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, daObj_Demo_Barrel_c::M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0x22E0)) {
            return cPhs_ERROR_e;
        }
        fopAcM_SetMtx(i_this, i_this->mpModel->getBaseTRMtx());
        i_this->m2DC = 1;
        i_this->m2DE = 0;
    }
    return ret;
}

/* 02333F94 */
static BOOL daObj_Demo_BarrelDelete(void* v_this) {
    dComIfG_resDeleteDemo(&((daObj_Demo_Barrel_c*)v_this)->mPhase, daObj_Demo_Barrel_c::M_arcname);
    return TRUE;
}

/* 02334294 */
// HD: daObj_Demo_BarrelExecute (_execute inlined)
static BOOL daObj_Demo_BarrelExecute(void* v_this) {
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    u8 id = i_this->demoActorID;
    if (id != 0) {
        dDemo_actor_c* ac = nullptr;
        // HD: the demo lookup retains its inline range check and assertion.
        if (id <= 0x20) {
            if (m_object == nullptr) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUTAssertion::showAssert("d_demo.h", 0x23A, "m_object != (0)");
            ac = m_object->getActor(id);
        }
        /* HD: null check on the demo actor */
        if (ac != nullptr && ac->checkEnable(dDemo_actor_c::ENABLE_UNK_e)) {
            dDemo_prm_c* prm = ac->getPrm();
            if (i_this->m2DC == 1 && prm->getId() == 1) {
                i_this->m2DC = 0;
                dComIfGs_onCollect(3, 0);
                dComIfGs_offCollect(0, 0);
                dComIfGs_setSelectEquip(0, dItemNo_NONE_e);
                i_this->setParticleHahen();
            }
            if (i_this->m2DE == 0 && prm->getId() == 2) {
                i_this->setParticleSibuki();
                i_this->m2DE = 1;
            }
        }
    }
    dDemo_setDemoData(i_this, dDemo_actor_c::ENABLE_ANM_FRAME_e | dDemo_actor_c::ENABLE_ANM_e |
                                   dDemo_actor_c::ENABLE_ROTATE_e | dDemo_actor_c::ENABLE_TRANS_e, i_this->mpMorf, "DBarrel", 0, nullptr, 0, 0);
    i_this->mpModel->setBaseScale(i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
    i_this->mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    return FALSE;
}

/* 02334494 */
// HD: daObj_Demo_BarrelDraw (_draw inlined)
static BOOL daObj_Demo_BarrelDraw(void* v_this) {
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    if (i_this->m2DC == 0) {
        return FALSE;
    }
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    g_env_light.setLightTevColorType(i_this->mpModel, &i_this->tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}

/* 023345A8 */
static BOOL daObj_Demo_BarrelIsDelete(void* v_this) {
    return TRUE;
}

/* 02334500 */
static void __sinit_d_a_obj_demo_barrel_cpp() {
    __sinit_header_statics();
}

/* 02334594 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023345B0 */
// HD: daObj_Demo_Barrel_c deleting destructor
static void daObj_Demo_Barrel_c_dt(daObj_Demo_Barrel_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02334604 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and existing local bindings
str 10027228
str 100271B0 in 02334294
str 100271F4
str 100271E4
data m_object 101D5FFC
data em_scl 10469618 guard 10469614
ext 02526E70 dDemo_actor_c* dDemo_object_getActor(u32, u8)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8)
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 024F1478 f32 dBgS_ObjGndChk_Wtr_Func(cXyz*)
ext 02522398 void dComIfGs_setSelectEquip(s32, u8)
*/
