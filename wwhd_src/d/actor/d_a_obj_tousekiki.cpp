/**
 * d_a_obj_tousekiki.cpp (WWHD)
 * Object - Catapult (Tetra's Ship)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_tousekiki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10031630)     /* "Touseki" */
#define SAFESTRING_VTBL 0x100315CC    /* this TU's sead::SafeString vtable */
#define TOUSEKIKI_VTBL 0x100315F4     /* HD: daObj_Tousekiki_c vtable */
#define AAB_VTBL 0x100315E4           /* this TU's cM3dGAab vtable */

enum {
    dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e = 5,
    dRes_INDEX_TOUSEKI_BDL_ATOSK_A_e = 8,
};

struct daObj_Tousekiki_c : fopAc_ac_c {
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mMorf;
    /* 0x3BC */ Mtx34 m2A0;
    /* 0x3EC */ be<u16> m2D0;
    /* 0x3EE */ u8 _3EE[2];
    /* 0x3F0 */ dCcD_Stts mStts;
    /* 0x42C */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(daObj_Tousekiki_c, m2A0, 0x3BC);
WWHD_OFFSET(daObj_Tousekiki_c, mStts, 0x3F0);
WWHD_OFFSET(daObj_Tousekiki_c, mCyl, 0x42C);
WWHD_SIZE(daObj_Tousekiki_c, 0x55C);

/* daObjPirateship::Act_c, the fields read here (HD offsets) */
struct daObjPirateship_Act_l : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3E4 - 0x3AC];
    /* 0x3E4 */ be<u8> m2CC;              /* GameCube 0x2C8 */
    /* 0x3E5 */ u8 _3E5[3];
    /* 0x3E8 */ gptr<J3DModel> mModel;    /* GameCube 0x2CC */
};
WWHD_OFFSET(daObjPirateship_Act_l, mModel, 0x3E8);

/* static daObjPirateship::Act_c* l_p_ship */
static gptr<daObjPirateship_Act_l>& l_p_ship() { return *gabi::at<gptr<daObjPirateship_Act_l>>(0x1046C558); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02527028 dDemo_setDemoData(actor, flags, morf, arcName, n, ids, p6, p7) [v tousekiki] */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, m, arc, n, ids, p6, p7);
}

/* 023A5CCC */
BOOL daObj_Tousekiki_c::CreateHeap() {
    WWHD_FUNC(0x023A5CCC, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BDL_ATOSK_A_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e, SAFESTRING_VTBL);
    mMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                   nullptr, 0x80000, 0x11000002);
    if (!mMorf) {
        return false;
    }

    anm = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e, SAFESTRING_VTBL);
    mMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    m2D0 = 0xFFFF;
    mMorf->setFrame(mMorf->getEndFrame() - 1.0f);
    mModel = mMorf->getModel();
    if (!mModel) {
        return false;
    }
    return true;
}
VERIFY(0x023A5CCC, &daObj_Tousekiki_c::CreateHeap);

/* 023A5E98 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x023A5E98, BOOL, a_this);
    return ((daObj_Tousekiki_c*)a_this)->CreateHeap();
}
VERIFY(0x023A5E98, CheckCreateHeap);

/* 023A5E9C: _create() inlined */
static cPhs_State daObj_TousekikiCreate(void* v_this) {
    WWHD_FUNC(0x023A5E9C, cPhs_State, v_this);
    daObj_Tousekiki_c* i_this = (daObj_Tousekiki_c*)v_this;
    /* fopAcM_ct(this, daObj_Tousekiki_c) (HD/USA: before the resource load) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = TOUSEKIKI_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, M_arcname);
    if (PVar1 == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, 0x023A5E98 /* CheckCreateHeap */, 0x900)) {
            return cPhs_ERROR_e;
        }

        mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
        mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
        mDoMtx_stack_c::scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
        PSMTXCopy(mDoMtx_stack_c::get(), &i_this->m2A0);
        J3DModel_setBaseScale(i_this->mModel, &i_this->scale);

        mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
        J3DModel_setBaseTRMtx(i_this->mModel, mDoMtx_stack_c::get());

        PSMTXCopy(mDoMtx_stack_c::get(), &i_this->m2A0);
        l_p_ship() = (daObjPirateship_Act_l*)fopAcM_SearchByID(i_this->parentActorID);
        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mModel)); /* fopAcM_SetMtx */
        PSMTXCopy(J3DModel_getBaseTRMtx(i_this->mModel), &i_this->m2A0);
    }
    return PVar1;
}
VERIFY(0x023A5E9C, daObj_TousekikiCreate);

/* 023A6128: _delete() inlined */
static BOOL daObj_TousekikiDelete(void* v_this) {
    WWHD_FUNC(0x023A6128, BOOL, v_this);
    daObj_Tousekiki_c* i_this = (daObj_Tousekiki_c*)v_this;
    dComIfG_resDelete(&i_this->mPhase, M_arcname); /* dComIfG_resDeleteDemo */
    return true;
}
VERIFY(0x023A6128, daObj_TousekikiDelete);

/* 023A6158: _execute(), demo_move() and set_mtx() inlined.
 * static cXyz touseki_offset(0.0f, 700.0f, 850.0f): HD guard 0x101FDC54, object 0x101FDC58 */
static BOOL daObj_TousekikiExecute(void* v_this) {
    WWHD_FUNC(0x023A6158, BOOL, v_this);
    daObj_Tousekiki_c* i_this = (daObj_Tousekiki_c*)v_this;
    be<u32>& guard = *gabi::at<be<u32>>(0x101FDC54);
    cXyz* touseki_offset = gabi::at<cXyz>(0x101FDC58);
    if (guard == 0) {
        guard = 1;
        touseki_offset->x = 0.0f;
        touseki_offset->y = 700.0f;
        touseki_offset->z = 850.0f;
    }

    /* demo_move() */
    PSMTXMultVec(J3DModel_getBaseTRMtx(l_p_ship()->mModel), touseki_offset, &i_this->current.pos);
    if (dDemo_setDemoData(i_this, 0x68 /* ENABLE_ANM_FRAME | ENABLE_ANM | ENABLE_ROTATE */, i_this->mMorf, STR(0x10031620) /* "Touseki" */,
                          0, nullptr, 0, 0) == 0) {
        daObjPirateship_Act_l* ship = l_p_ship();
        s16 x = ship->shape_angle.x;
        i_this->shape_angle.x = x;
        s16 y = ship->shape_angle.y;
        i_this->shape_angle.y = y;
        s16 z = ship->shape_angle.z;
        i_this->current.angle.x = x;
        i_this->shape_angle.z = z;
        i_this->current.angle.y = y;
        i_this->current.angle.z = z;
    }

    /* set_mtx() */
    J3DModel_setBaseScale(i_this->mModel, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(i_this->mModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->m2A0);
    return false;
}
VERIFY(0x023A6158, daObj_TousekikiExecute);

/* 023A6304: _draw() inlined */
static BOOL daObj_TousekikiDraw(void* v_this) {
    WWHD_FUNC(0x023A6304, BOOL, v_this);
    daObj_Tousekiki_c* i_this = (daObj_Tousekiki_c*)v_this;
    if (l_p_ship()->m2CC == 0) {
        return true;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mModel, &i_this->tevStr);
    i_this->mMorf->updateDL();
    return true;
}
VERIFY(0x023A6304, daObj_TousekikiDraw);

/* 023A6418 */
static BOOL daObj_TousekikiIsDelete(void*) {
    WWHD_FUNC(0x023A6418, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023A6418, daObj_TousekikiIsDelete);

/* 023A6370 */
static void __sinit_d_a_obj_tousekiki_cpp() {
    WWHD_FUNC(0x023A6370, void, (u32)0);
    sinit_header_statics(0x1046C55C, 0x101CD4DC);
}
VERIFY(0x023A6370, __sinit_d_a_obj_tousekiki_cpp);

/* 023A6404: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023A6404, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x023A6404, SafeString_dt);

/* 023A648C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x023A648C, void, (u32)0);
}
VERIFY(0x023A648C, SafeString_assureTerminationImpl);

/* 023A6420: daObj_Tousekiki_c deleting destructor (compiler-generated, vtable +0xC) */
static void daObj_Tousekiki_c_dt(daObj_Tousekiki_c* i_this, s32 flags) {
    WWHD_FUNC(0x023A6420, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023A6420, daObj_Tousekiki_c_dt);
