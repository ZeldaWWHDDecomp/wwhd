/**
 * d_a_obj_tousekiki.cpp (WWHD)
 * Object - Catapult (Tetra's Ship)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_tousekiki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_tousekiki.h"
const gc_tu_data gc_tu = { .safeString = 0x100315CC, .headerStatics = {0x1046C55C, 0x101CD4DC, 0x1046C568} };
const char daObj_Tousekiki_c::M_arcname[] = "Touseki";
static daObjPirateship_ActView* l_p_ship;

/* 023A5CCC */
BOOL daObj_Tousekiki_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BDL_ATOSK_A_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e);
    mMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                   nullptr, 0x80000, 0x11000002);
    if (!mMorf) {
        return false;
    }

    anm = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e);
    mMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    m2D0 = 0xFFFF;
    mMorf->setFrame(mMorf->getEndFrame() - 1.0f);
    mModel = mMorf->getModel();
    if (!mModel) {
        return false;
    }
    return true;
}

/* 023A5E98 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    return ((daObj_Tousekiki_c*)a_this)->CreateHeap();
}

inline cPhs_State daObj_Tousekiki_c::_create() {
    // HD: actor construction precedes resource loading.
    fopAcM_ct(this, daObj_Tousekiki_c);
    cPhs_State PVar1 = dComIfG_resLoad(&mPhase, M_arcname);
    if (PVar1 == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x900)) {
            return cPhs_ERROR_e;
        }

        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
        PSMTXCopy(mDoMtx_stack_c::get(), &m2A0);
        mModel->setBaseScale(scale);

        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::ZXYrotM(shape_angle);
        mModel->setBaseTRMtx(mDoMtx_stack_c::get());

        PSMTXCopy(mDoMtx_stack_c::get(), &m2A0);
        l_p_ship = (daObjPirateship_ActView*)fopAcM_SearchByID(parentActorID);
        fopAcM_SetMtx(this, mModel->getBaseTRMtx());
        PSMTXCopy(mModel->getBaseTRMtx(), &m2A0);
    }
    return PVar1;
}

/* 023A5E9C */
// HD: _create is inlined into this callback.
static cPhs_State daObj_TousekikiCreate(void* i_this) {
    return ((daObj_Tousekiki_c*)i_this)->_create();
}

inline bool daObj_Tousekiki_c::_delete() {
    dComIfG_resDeleteDemo(&mPhase, M_arcname);
    return true;
}

/* 023A6128 */
// HD: _delete is inlined into this callback.
static BOOL daObj_TousekikiDelete(void* i_this) {
    return ((daObj_Tousekiki_c*)i_this)->_delete();
}

inline void daObj_Tousekiki_c::demo_move() {
    static cXyz touseki_offset(0.0f, 700.0f, 850.0f);

    PSMTXMultVec(l_p_ship->mModel->getBaseTRMtx(), &touseki_offset, &current.pos);
    if (dDemo_setDemoData(this, DEMO_ENABLE_ANM_FRAME_e | DEMO_ENABLE_ANM_e | DEMO_ENABLE_ROTATE_e, mMorf, "Touseki",
                          0, nullptr, 0, 0) == 0) {
        daObjPirateship_ActView* ship = l_p_ship;
        s16 x = ship->shape_angle.x;
        shape_angle.x = x;
        s16 y = ship->shape_angle.y;
        shape_angle.y = y;
        s16 z = ship->shape_angle.z;
        current.angle.x = x;
        shape_angle.z = z;
        current.angle.y = y;
        current.angle.z = z;
    }
}

inline void daObj_Tousekiki_c::set_mtx() {
    mModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &m2A0);
}

inline bool daObj_Tousekiki_c::_execute() {
    demo_move();
    set_mtx();
    return false;
}

/* 023A6158 */
// HD: _execute is inlined into this callback.
static BOOL daObj_TousekikiExecute(void* i_this) {
    return ((daObj_Tousekiki_c*)i_this)->_execute();
}

inline bool daObj_Tousekiki_c::_draw() {
    if (l_p_ship->m2CC == 0) {
        return true;
    }
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    mMorf->updateDL();
    return true;
}

/* 023A6304 */
// HD: _draw is inlined into this callback.
static BOOL daObj_TousekikiDraw(void* i_this) {
    return ((daObj_Tousekiki_c*)i_this)->_draw();
}

/* 023A6418 */
static BOOL daObj_TousekikiIsDelete(void*) {
    return TRUE;
}

/* 023A6370 */
static void __sinit_d_a_obj_tousekiki_cpp() {
    __sinit_header_statics();
}

/* 023A6404 */
// HD: sead::SafeString deleting destructor (this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 023A648C */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* 023A6420 */
// HD: daObj_Tousekiki_c deleting destructor (compiler-generated, vtable +0xC)
static void daObj_Tousekiki_c_dt(daObj_Tousekiki_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10031630 # "Touseki"
str 10031620 in 023A6158 # "Touseki"
data l_p_ship 1046C558
data touseki_offset 101FDC58 guard 101FDC54
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8)
*/
