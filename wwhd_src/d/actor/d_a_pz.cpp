/**
 * d_a_pz.cpp (WWHD)
 * NPC - Princess Zelda (Ganon's Tower)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_pz.cpp) has only "Nonmatching" stubs for this actor, so every function is
 * written from the WWHD code (cking.rpx) and verified against it. GameCube names and structure
 * are kept where the GameCube symbols give them.
 */
#include "d/actor/d_a_pz.h"

/* 02450D80 */
static BOOL stealItem_CB(void* i_actor) {
    WWHD_FUNC(0x02450D80, BOOL, i_actor);
    if (i_actor != nullptr) {
        fopAc_ac_c* item = (fopAc_ac_c*)i_actor;
        u32 flags = gabi::ea(item) + 0x783; /* the item's collider: a flag byte (0x40) */
        gabi::store<u8>(flags, (u8)(gabi::load<u8>(flags) | 0x40));
        item->scale.x = 1.0f;
        item->scale.y = 1.0f;
        item->scale.z = 1.0f;
    }
    return TRUE;
}
VERIFY(0x02450D80, stealItem_CB);

/* 02450DB0 */
void daPz_c::_nodeHeadControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02450DB0, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());

    /* static cXyz eye_offset(0, 0, 0) (guard 0x1046D5E0) */
    if (gabi::load<u32>(0x1046D5E0) == 0) {
        gabi::store<f32>(0x1046D430, 0.0f);
        gabi::store<f32>(0x1046D438, 0.0f);
        gabi::store<u32>(0x1046D5E0, 1);
        gabi::store<f32>(0x1046D434, 0.0f);
    }
    /* static cXyz eye_offset2(24, -16, 0) (guard 0x1046D5E4) */
    if (gabi::load<u32>(0x1046D5E4) == 0) {
        gabi::store<u32>(0x1046D5E4, 1);
        gabi::store<f32>(0x1046D43C, 24.0f);
        gabi::store<f32>(0x1046D444, 0.0f);
        gabi::store<f32>(0x1046D440, -16.0f);
    }
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x1046D430), &mEyePos);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[0][0]);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x1046D43C), &mEyePos2);
    Mtx34* now = mDoMtx_stack_c::get();
    mHeadPos.x = now->m[0][3];
    mHeadPos.y = now->m[1][3];
    mHeadPos.z = now->m[2][3];
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}
VERIFY(0x02450DB0, &daPz_c::_nodeHeadControl);

/* 02450F68 */
static BOOL nodeHeadControl_CB(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02450F68, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        daPz_c* i_this = gabi::at<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeHeadControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}
VERIFY(0x02450F68, nodeHeadControl_CB);

/* 02450FB0 */
void daPz_c::_nodeWaistControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02450FB0, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    /* debug test angle (static s16 at 0x1046D5E8, turns by 0x1000 every call) */
    gabi::store<s16>(0x1046D5E8, (s16)(gabi::load<s16>(0x1046D5E8) + 0x1000));
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    if ((u32)(mAnm - 4) <= 2) {
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), 0xDAC);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), 0x5DC);
    }
    if (REG12_S(6) != 0) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), gabi::load<s16>(0x1046D5E8));
    } else {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[1][0]);
    }
    if ((u32)(mAnm - 4) <= 2) {
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -0x5DC);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -0xDAC);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), &mWaistMtx);
    Mtx34* now = mDoMtx_stack_c::get();
    mWaistPos.x = now->m[0][3];
    mWaistPos.y = now->m[1][3];
    mWaistPos.z = now->m[2][3];
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}
VERIFY(0x02450FB0, &daPz_c::_nodeWaistControl);

/* 02451178 */
static BOOL nodeWaistControl_CB(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02451178, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        daPz_c* i_this = gabi::at<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeWaistControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}
VERIFY(0x02451178, nodeWaistControl_CB);

/* 024511C0 */
void daPz_c::_nodeWaist2Control(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x024511C0, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    mDoMtx_YrotM(mDoMtx_stack_c::get(), mWaistAngleY);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), mWaistAngleZ);
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}
VERIFY(0x024511C0, &daPz_c::_nodeWaist2Control);

/* 024512DC */
static BOOL nodeWaist2Control_CB(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x024512DC, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        daPz_c* i_this = gabi::at<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeWaist2Control(i_node, (J3DModel*)model);
    }
    return TRUE;
}
VERIFY(0x024512DC, nodeWaist2Control_CB);

/* 02451324 */
void daPz_c::_nodeSkirtControl(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02451324, void, this, i_node, i_model);
    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    s16 angle = mWaistAngleZ;
    if (angle > mWaistAngleY)
        angle = mWaistAngleY;
    mDoMtx_XrotM(mDoMtx_stack_c::get(), REG12_S(2));
    mDoMtx_YrotM(mDoMtx_stack_c::get(), REG12_S(3));
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-(angle + REG12_S(4)));
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}
VERIFY(0x02451324, &daPz_c::_nodeSkirtControl);

/* 0245145C */
static BOOL nodeSkirtControl_CB(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0245145C, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        daPz_c* i_this = gabi::at<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeSkirtControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}
VERIFY(0x0245145C, nodeSkirtControl_CB);

/* 024514A4 daPz_matAnm_c::daPz_matAnm_c (HD: allocates when this == NULL) */
static daPz_matAnm_c* daPz_matAnm_c_ct(daPz_matAnm_c* i_this) {
    WWHD_FUNC(0x024514A4, daPz_matAnm_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daPz_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mMoveFlag = 0;
    i_this->mNowOffsetY = 0.0f;
    i_this->__vtbl = 0x10038D48;
    i_this->mNowOffsetX = 0.0f;
    return i_this;
}
VERIFY(0x024514A4, daPz_matAnm_c_ct);

/* 02452158 */
BOOL daPz_c::_createHeap() {
    WWHD_FUNC(0x02452158, BOOL, this);
    if (!bodyCreateHeap() || !bowCreateHeap())
        return FALSE;
    return TRUE;
}
VERIFY(0x02452158, &daPz_c::_createHeap);

/* 024521B8 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024521B8, BOOL, i_this);
    return ((daPz_c*)i_this)->_createHeap();
}
VERIFY(0x024521B8, createHeap_CB);

/* 024521BC: destroys the function-local static sead::SafeString array at 0x1046D560 (16 entries;
 * registered by its first use) */
static void __dt_safestring_array() {
    WWHD_FUNC(0x024521BC, void, (u32)0);
    gabi::call(0x028F0164, (u32)0x1046D560, (u32)0x10, (u32)8, (u32)0x02458F74, (u32)0, (u32)0); /* __destroy_arr */
}
VERIFY(0x024521BC, __dt_safestring_array);

/* 024521E0 enemyfire::enemyfire (this TU's copy; HD: allocates when this == NULL) */
static enemyfire_l* enemyfire_ct(enemyfire_l* i_this) {
    WWHD_FUNC(0x024521E0, enemyfire_l*, i_this);
    if (i_this == nullptr) {
        i_this = (enemyfire_l*)operator_new(0x22C);
        if (i_this == nullptr)
            return i_this;
    }
    if (gabi::ea(&i_this->mDirection) == 0) /* cXyz member "constructor" (never allocates) */
        operator_new(0xC);
    dCcD_Stts_ct(&i_this->mStts);
    gabi::call(0x025166F0, &i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->m228 = 1.0f;                   /* LIGHT_INFLUENCE (HD float) */
    return i_this;
}
VERIFY(0x024521E0, enemyfire_ct);

/* 024525F0 */
void daPz_c::getArg() {
    WWHD_FUNC(0x024525F0, void, this);
    s32 arg = (fopAcM_GetParam(this) >> 8) & 0xFF;
    if (arg == 0xFF)
        arg = 0;
    mArg = arg;
}
VERIFY(0x024525F0, &daPz_c::getArg);

/* 02452B44 */
cPhs_State daPz_c::_create() {
    WWHD_FUNC(0x02452B44, cPhs_State, this);
    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcName);
    /* fopAcM_SetupActor(this, daPz_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr)
            gabi::call(0x0245226C, this); /* daPz_c::daPz_c */
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (ret == cPhs_COMPLEATE_e) {
        getArg();
        if (mArg != 0 && dComIfGp_isStartStage(0x10038954 /* "kenroom" */) && dComIfGs_isEventBit(0x3520) == 1)
            return cPhs_ERROR_e;
        if (!fopAcM_entrySolidHeap(this, 0x024521B8 /* createHeap_CB */, 0xA740))
            return cPhs_ERROR_e;
        createInit();
    }
    return ret;
}
VERIFY(0x02452B44, &daPz_c::_create);

/* 02452D10 */
static cPhs_State daPzCreate(void* i_this) {
    WWHD_FUNC(0x02452D10, cPhs_State, i_this);
    return ((daPz_c*)i_this)->_create();
}
VERIFY(0x02452D10, daPzCreate);

/* 02452D14 */
BOOL daPz_c::_delete() {
    WWHD_FUNC(0x02452D14, BOOL, this);
    dComIfG_resDelete(&mPhs, l_arcName);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mRipple);
    mFollowCB0.remove();
    mFollowCB1.remove();
    if (heap.get() != nullptr)
        mpMorf->stopZelAnime();
    return TRUE;
}
VERIFY(0x02452D14, &daPz_c::_delete);

/* 02452D94 */
static BOOL daPzDelete(void* i_this) {
    WWHD_FUNC(0x02452D94, BOOL, i_this);
    return ((daPz_c*)i_this)->_delete();
}
VERIFY(0x02452D94, daPzDelete);

/* 024531E4 */
void daPz_c::setJntStatus() {
    WWHD_FUNC(0x024531E4, void, this);
    m_jnt.setParam(hio_s16(0xA), hio_s16(0xE), hio_s16(0x12), hio_s16(0x16), hio_s16(0x8), hio_s16(0xC), hio_s16(0x10),
                   hio_s16(0x14), hio_s16(0x18));
}
VERIFY(0x024531E4, &daPz_c::setJntStatus);

/* 024539BC */
void daPz_c::checkOrder() {
    WWHD_FUNC(0x024539BC, void, this);
    u16 cmd = eventInfo_getCommand(this);
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        mOrderState = 0;
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrderState == 1 || mOrderState == 2) {
            mOrderState = 0;
            modeProc(PROC_INIT_e, 9);
        }
    }
}
VERIFY(0x024539BC, &daPz_c::checkOrder);

/* 02453A00 (unnamed by the matcher: daPz_c::eventOrder) */
void daPz_c::eventOrder() {
    WWHD_FUNC(0x02453A00, void, this);
    if (mOrderState == 1 || mOrderState == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderState == 1)
            fopAcM_orderSpeakEvent(this);
    }
}
VERIFY(0x02453A00, &daPz_c::eventOrder);

/* 02454060 */
void daPz_c::setBowAnm(s8 i_anm, s32 i_force) {
    WWHD_FUNC(0x02454060, void, this, i_anm, i_force);
    if (i_anm != 5)
        mBowAnm = i_anm;
    dLib_bcks_setAnm(l_arcName, mpBowMcaMorf, &mBowBckIdx, &mBowAnm, &mBowOldAnm, 0x100389B8 /* bow bcks */,
                     0x100389C4 /* bow anm prms */, i_force, 0);
}
VERIFY(0x02454060, &daPz_c::setBowAnm);

/* 024540C0 (unnamed by the matcher: daPz_c::setBowString): shows one of the two bow-string shapes */
void daPz_c::setBowString(s32 i_pulled) {
    WWHD_FUNC(0x024540C0, void, this, i_pulled);
    J3DModel_l* model = (J3DModel_l*)mpBowMcaMorf->getModel();
    u32 data = gabi::ea(model->mModelData.get());
    u32 tbl = gabi::load<u32>(gabi::load<u32>(gabi::load<u32>(data + 8) + 0x10) + 4);
    u32 shapeA = gabi::load<u32>(tbl + 4);
    u32 shapeB = gabi::load<u32>(tbl + 8);
    if (i_pulled) {
        gabi::store<u8>(shapeB + 4, 1);
        gabi::store<u8>(gabi::load<u32>(shapeA + 8) + 4, 0);
    } else {
        gabi::store<u8>(shapeB + 4, 0);
        gabi::store<u8>(gabi::load<u32>(shapeA + 8) + 4, 1);
    }
}
VERIFY(0x024540C0, &daPz_c::setBowString);

/* 024545DC */
void daPz_c::setEyeAnm(s8 i_eye) {
    WWHD_FUNC(0x024545DC, void, this, i_eye);
    /* eye animation table (0x10038A70): {btp, btk} resource indices per eye state, -1 = keep */
    u32 entry = 0x10038A70 + i_eye * 8;
    s32 btp = gabi::load<s32>(entry);
    if (btp != -1)
        setEyeBtp(btp);
    s32 btk = gabi::load<s32>(entry + 4);
    if (btk != -1)
        setEyeBtk(btk);
    mCurEye = i_eye;
}
VERIFY(0x024545DC, &daPz_c::setEyeAnm);

/* 02458F88 */
static BOOL daPzIsDelete(void*) {
    WWHD_FUNC(0x02458F88, BOOL, (void*)nullptr);
    return TRUE;
}
VERIFY(0x02458F88, daPzIsDelete);

/* 02458EA4: static initialisation of the translation unit */
static void __sinit_d_a_pz_cpp() {
    WWHD_FUNC(0x02458EA4, void, (u32)0);
    sinit_header_statics(0x1046D404, 0x101CF240);
    /* HD: two static sead::SafeStrings (material names used by bodyCreateHeap) */
    gabi::store<u32>(0x1046D424, SAFESTRING_VTBL);
    gabi::store<u32>(0x1046D420, 0x10038CE8);
    gabi::store<u32>(0x1046D42C, SAFESTRING_VTBL);
    gabi::store<u32>(0x1046D428, 0x10038CF4);
    gabi::call(0x02458C04, (u32)L_HIO); /* static daPz_HIO_c l_HIO */
}
VERIFY(0x02458EA4, __sinit_d_a_pz_cpp);

/* 02458F74: sead::SafeString deleting destructor (this TU's copy; vtable 0x10038558 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02458F74, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x02458F74, SafeString_dt);

/* 02458F90: daPz_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daPz_c_dt(daPz_c* i_this, s32 flags) {
    WWHD_FUNC(0x02458F90, void, i_this, flags);
    if (i_this != nullptr) {
        for (int i = 3; i >= 0; i--)
            gabi::call(0x027F13DC, &i_this->mPacket[i], 0); /* packet destructor */
        gabi::call(0x02515AE8, &i_this->mEnemyFire.mSph, 2);  /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mEnemyFire.mStts, 2);
        u32 acch = gabi::ea(&i_this->mEnemyIce.mBgAcch);
        gabi::store<u32>(acch + 0x20, 0x100385B0);
        gabi::store<u32>(acch + 0x14, 0x100385C0);
        gabi::call(0x024EFD9C, acch, 0);                       /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::ea(&i_this->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
        u32 lin = gabi::ea(&i_this->mLinChk);
        gabi::store<u32>(lin + 0x58, 0x10038600);
        gabi::store<u32>(lin + 0x64, 0x10038590);
        gabi::store<u32>(lin + 0x20, 0x10038580);
        gabi::call(0x02008B4C, lin, 0);                        /* cBgS_LinChk::~cBgS_LinChk */
        gabi::call(0x025E89F8, i_this->mInvisibleModel, 2);   /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2);
        u32 oacch = gabi::ea(&i_this->mObjAcch);
        gabi::store<u32>(oacch + 0x20, 0x100385B0);
        gabi::store<u32>(oacch + 0x14, 0x100385C0);
        gabi::call(0x024EFD9C, oacch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02458F90, daPz_c_dt);

/* 024590F8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x024590F8, void, (SafeString*)nullptr);
}
VERIFY(0x024590F8, SafeString_assureTerminationImpl);
