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

static cXyz l_eyeOffset;
static cXyz l_eyeOffset2;
static cXyz l_rippleScale;
static cXyz l_landScale;
static cXyz l_downFootScale;
static cXyz l_downWaistScale;
static cXyz l_downHeadScale;
static u32 l_eyeOffsetInitialized;
static u32 l_eyeOffset2Initialized;
static s16 l_waistDebugAngle;
static u32 l_materialNamesInitialized;
static u32 l_rippleScaleInitialized;
static u32 l_landScaleInitialized;
static u32 l_downFootScaleInitialized;
static u32 l_downWaistScaleInitialized;
static u32 l_downHeadScaleInitialized;
static PzEyeAnmEntryHD eye_anm_tbl[10] = {{39, 31}, {35, 30}, {36, -1}, {37, -1}, {41, 32},
                                          {38, -1}, {44, -1}, {40, 31}, {42, -1}, {43, -1}};
static PzEyeMaterialNamesHD l_eyeMaterialNames;
static s32 bow_bcks[3] = {PZ_BCK_BOW_READY, PZ_BCK_BOW_HOLD, PZ_BCK_BOW_SHOOT};
static PzAnmParameterHD bow_anm_prm[5] = {{2, -1, 0, 0.0f, 0.0f, 0},
                                          {2, -1, 0, 0.0f, 1.0f, 0},
                                          {0, -1, 0, 0.0f, 1.0f, 0},
                                          {1, -1, 0, 0.0f, 1.0f, 0},
                                          {2, -1, 0, 0.0f, 1.0f, 0}};
static dCcD_SrcCyl m_cyl_src;
static PzHeaderStaticsHD l_headerStatics;
static PzRegistrationHD l_headerRegistration;
static daPz_HIO_c l_HIO;
static PzModelPointerHD l_pzCurrentModel;
static Mtx34 l_pzCurrentMtx;

/* 02450D80 */
BOOL stealItem_CB(void* i_actor) {

    if (i_actor != nullptr) {
        fopAc_ac_c* item = (fopAc_ac_c*)i_actor;
        be<u8>& flags = reinterpret_cast<PzItemHD*>(item)
                            ->mColliderFlags; /* the item's collider: a flag byte (0x40) */
        flags |= 0x40;
        item->scale.x = 1.0f;
        item->scale.y = 1.0f;
        item->scale.z = 1.0f;
    }
    return TRUE;
}

/* 02450DB0 */
void daPz_c::_nodeHeadControl(J3DNode* i_node, J3DModel* i_model) {

    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());

    // HD: Preserve the compiler's component and guard write order.

    if (l_eyeOffsetInitialized == 0) {
        l_eyeOffset.x = 0.0f;
        l_eyeOffset.z = 0.0f;
        l_eyeOffsetInitialized = 1;
        l_eyeOffset.y = 0.0f;
    }
    // HD: Preserve the compiler's component and guard write order.

    if (l_eyeOffset2Initialized == 0) {
        l_eyeOffset2Initialized = 1;
        l_eyeOffset2.x = 24.0f;
        l_eyeOffset2.z = 0.0f;
        l_eyeOffset2.y = -16.0f;
    }
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_eyeOffset, &mEyePos);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[0][0]);
    PSMTXMultVec(mDoMtx_stack_c::get(), &l_eyeOffset2, &mEyePos2);
    Mtx34* now = mDoMtx_stack_c::get();
    mHeadPos.x = now->m[0][3];
    mHeadPos.y = now->m[1][3];
    mHeadPos.z = now->m[2][3];
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}

/* 02450F68 */
BOOL nodeHeadControl_CB(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daPz_c* i_this = pzGuest<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeHeadControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}

/* 02450FB0 */
void daPz_c::_nodeWaistControl(J3DNode* i_node, J3DModel* i_model) {

    J3DJoint* joint = J3DNode_toJoint(i_node);

    l_waistDebugAngle = (s16)(l_waistDebugAngle + 0x1000);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    if ((u32)(m06D3 - 4) <= 2) {
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), 0xDAC);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), 0x5DC);
    }
    if (REG12_S(6) != 0) {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), l_waistDebugAngle);
    } else {
        mDoMtx_XrotM(mDoMtx_stack_c::get(), m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)-m_jnt.mAngles[1][0]);
    }
    if ((u32)(m06D3 - 4) <= 2) {
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

/* 02451178 */
BOOL nodeWaistControl_CB(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daPz_c* i_this = pzGuest<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeWaistControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}

/* 024511C0 */
void daPz_c::_nodeWaist2Control(J3DNode* i_node, J3DModel* i_model) {

    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    mDoMtx_YrotM(mDoMtx_stack_c::get(), mWaistAngleY);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), mWaistAngleZ);
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}

/* 024512DC */
BOOL nodeWaist2Control_CB(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daPz_c* i_this = pzGuest<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeWaist2Control(i_node, (J3DModel*)model);
    }
    return TRUE;
}

/* 02451324 */
void daPz_c::_nodeSkirtControl(J3DNode* i_node, J3DModel* i_model) {

    J3DJoint* joint = J3DNode_toJoint(i_node);
    s32 jntNo = J3DJoint_getJntNo(joint);
    PSMTXCopy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
    s16 angle = mWaistAngleZ;
    if (angle > mWaistAngleY)
        angle = mWaistAngleY;
    mDoMtx_XrotM(mDoMtx_stack_c::get(), REG12_S(2));
    mDoMtx_YrotM(mDoMtx_stack_c::get(), REG12_S(3));
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16) - (angle + REG12_S(4)));
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jntNo), mDoMtx_stack_c::get());
}

/* 0245145C */
BOOL nodeSkirtControl_CB(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daPz_c* i_this = pzGuest<daPz_c>(model->mUserArea);
        if (i_this != nullptr)
            i_this->_nodeSkirtControl(i_node, (J3DModel*)model);
    }
    return TRUE;
}

// HD: daPz_matAnm_c::daPz_matAnm_c (HD: allocates when this == NULL)
/* 024514A4 */
daPz_matAnm_c* daPz_matAnm_c_ct(daPz_matAnm_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daPz_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mMoveFlag = 0;
    i_this->mNowOffsetY = 0.0f;
    i_this->__vtbl = PZ_VT_MATERIAL_ANM;
    i_this->mNowOffsetX = 0.0f;
    return i_this;
}

/* 02452158 */
BOOL daPz_c::_createHeap() {

    if (!bodyCreateHeap() || !bowCreateHeap())
        return FALSE;
    return TRUE;
}

/* 024521B8 */
BOOL createHeap_CB(fopAc_ac_c* i_this) {

    return ((daPz_c*)i_this)->_createHeap();
}

// HD: Destroy the 16 material-name SafeStrings registered by the first heap creation.
/* 024521BC */
static void __dt_safestring_array() {

    pzDestroyArray(gabi::ea(material_names.mNames), (u32)0x10, (u32)8,
                   gabi::guest_fn((const void*)SafeString_dt), (u32)0, (u32)0); /* __destroy_arr */
}

// HD: enemyfire::enemyfire (this TU's copy; HD: allocates when this == NULL)
/* 024521E0 */
enemyfire* enemyfire_ct(enemyfire* i_this) {

    if (i_this == nullptr) {
        i_this = (enemyfire*)operator_new(0x22C);
        if (i_this == nullptr)
            return i_this;
    }
    if (gabi::ea(&i_this->mDirection) == 0) /* cXyz member "constructor" (never allocates) */
        operator_new(0xC);
    dCcD_Stts_ct(&i_this->mStts);
    pzSphereConstruct(&i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->mLight.mHD20 = 1.0f;      /* LIGHT_INFLUENCE (HD float) */
    return i_this;
}

/* 024525F0 */
void daPz_c::getArg() {

    s32 arg = (fopAcM_GetParam(this) >> 8) & 0xFF;
    if (arg == 0xFF)
        arg = 0;
    mArg = arg;
}

/* 02452B44 */
cPhs_State daPz_c::_create() {

    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcName);
    /* fopAcM_SetupActor(this, daPz_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr)
            daPz_c_ct(this); /* daPz_c::daPz_c */
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (ret == cPhs_COMPLEATE_e) {
        getArg();
        if (mArg != 0 && pzIsStartStage("kenroom") && dComIfGs_isEventBit(0x3520) == 1)
            return cPhs_ERROR_e;
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)createHeap_CB), 0xA740))
            return cPhs_ERROR_e;
        createInit();
    }
    return ret;
}

/* 02452D10 */
static cPhs_State daPzCreate(void* i_this) {

    return ((daPz_c*)i_this)->_create();
}

/* 02452D14 */
BOOL daPz_c::_delete() {

    dComIfG_resDelete(&mPhs, l_arcName);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRipple);
    mFollowCB0.remove();
    mFollowCB1.remove();
    if (heap.get() != nullptr)
        mpMorf->stopZelAnime();
    return TRUE;
}

/* 02452D94 */
static BOOL daPzDelete(void* i_this) {

    return ((daPz_c*)i_this)->_delete();
}

/* 024531E4 */
void daPz_c::setJntStatus() {

    m_jnt.setParam(l_HIO.mHeadMinX, l_HIO.mBackboneMinX, l_HIO.mHeadMinY, l_HIO.mBackboneMinY,
                   l_HIO.mHeadMaxX, l_HIO.mBackboneMaxX, l_HIO.mHeadMaxY, l_HIO.mBackboneMaxY,
                   l_HIO.mMaxJointStep);
}

/* 024539BC */
void daPz_c::checkOrder() {

    u16 cmd = eventInfo.getCommand();
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        m0F82 = 0;
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (m0F82 == 1 || m0F82 == 2) {
            m0F82 = 0;
            modeProc(PROC_INIT_e, MODE_TALK);
        }
    }
}

/* 02453A00 */
void daPz_c::eventOrder() {

    if (m0F82 == 1 || m0F82 == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (m0F82 == 1)
            fopAcM_orderSpeakEvent(this);
    }
}

/* 02454060 */
void daPz_c::setBowAnm(s8 i_anm, s32 i_force) {

    if (i_anm != 5)
        mBowAnm = i_anm;
    pzDLib_bcks_setAnm(l_arcName, mpBowMcaMorf, &mBowBckIdx, &mBowAnm, &mBowOldAnm,
                       gabi::ea(bow_bcks), gabi::ea(bow_anm_prm), i_force, 0);
}

/* 024540C0 */
void daPz_c::setBowString(s32 i_pulled) {

    J3DModel* model = mpBowMcaMorf->getModel();
    PzJointTreeHD* tree = pzGuest<PzJointTreeHD>(model->mModelData->mJointNodes);
    gptr<PzShapeHD>* shapes = tree->mpShapeHolder->mpShapes;
    PzShapeHD* shapeA = shapes[1];
    PzShapeHD* shapeB = shapes[2];
    if (i_pulled) {
        shapeB->mVisible = 1;
        shapeA->mpLinkedShape->mVisible = 0;
    } else {
        shapeB->mVisible = 0;
        shapeA->mpLinkedShape->mVisible = 1;
    }
}

/* 024545DC */
void daPz_c::setEyeAnm(s8 i_eye) {

    PzEyeAnmEntryHD* entry = &eye_anm_tbl[i_eye];
    s32 btp = entry->mBtp.get();
    if (btp != -1)
        setEyeBtp(btp);
    s32 btk = entry->mBtk.get();
    if (btk != -1)
        setEyeBtk(btk);
    mCurEye = i_eye;
}

/* 02458F88 */
static BOOL daPzIsDelete(void*) {

    return TRUE;
}

// HD: static initialisation of the translation unit
/* 02458EA4 */
static void __sinit_d_a_pz_cpp() {

    sinit_header_statics(gabi::ea(&l_headerStatics), gabi::ea(&l_headerRegistration));
    /* HD: two static sead::SafeStrings (material names used by bodyCreateHeap) */
    l_eyeMaterialNames.mNames[0].__vtbl = SAFESTRING_VTBL;
    l_eyeMaterialNames.mNames[0].mStringTop = gabi::guest_str("m_pz_eyeL");
    l_eyeMaterialNames.mNames[1].__vtbl = SAFESTRING_VTBL;
    l_eyeMaterialNames.mNames[1].mStringTop = gabi::guest_str("m_pz_eyeR");
    daPz_HIO_c_ct(reinterpret_cast<u8*>(&l_HIO)); /* static daPz_HIO_c l_HIO */
}

// HD: sead::SafeString deleting destructor (this TU's copy; vtable 0x10038558 slot +0xC)
/* 02458F74 */
void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

// HD: daPz_c deleting destructor (compiler-generated, HD virtual destructor)
/* 02458F90 */
static void daPz_c_dt(daPz_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        for (int i = 3; i >= 0; i--)
            pzPacketDestruct(&i_this->mPacket[i], 0);  /* packet destructor */
        pzSphereDestruct(&i_this->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mEnemyFire.mStts, 2);
        u32 acch = gabi::ea(&i_this->mEventIce.mBgAcch);
        i_this->mEventIce.mBgAcch.dBgS_GrpPassChk::__vtbl = PZ_VT_ACCH_GROUP;
        i_this->mEventIce.mBgAcch.dBgS_PolyPassChk::__vtbl = PZ_VT_ACCH_POLYGON;
        pzAcchDestruct(acch, 0); /* dBgS_Acch::~dBgS_Acch */
        pzCircleDestruct(gabi::ea(&i_this->mEventIce.mBgAcchCir.m_cir),
                         2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&i_this->mEventIce.mCyl, 2);
        dCcD_Stts_dt(&i_this->mEventIce.mStts, 2);
        u32 lin = gabi::ea(&i_this->mLinChk);
        i_this->mLinChk.dBgS_PolyPassChk::__vtbl = PZ_VT_LINE_POLYGON;
        i_this->mLinChk.dBgS_GrpPassChk::__vtbl = PZ_VT_LINE_GROUP;
        i_this->mLinChk.cBgS_PolyInfo::__vtbl = PZ_VT_LINE_INFO;
        pzLineDestruct(lin, 0); /* cBgS_LinChk::~cBgS_LinChk */
        pzInvisibleDestruct(i_this->mInvisibleModel,
                            2); /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        pzCircleDestruct(gabi::ea(&i_this->mAcchCir.m_cir), 2);
        u32 oacch = gabi::ea(&i_this->mObjAcch);
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = PZ_VT_ACCH_GROUP;
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = PZ_VT_ACCH_POLYGON;
        pzAcchDestruct(oacch, 0);
        pzActorDestruct(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty)
/* 024590F8 */
static void SafeString_assureTerminationImpl(SafeString*) {}

// clang-format off
/* WWHD glue
data l_headerStatics 1046D404
data l_headerRegistration 101CF240
data m_cyl_src 10038D04
str 10038954 in 02452B44
data bow_bcks 100389B8
data bow_anm_prm 100389C4
data l_eyeMaterialNames 1046D420
str 10038CE8 in 02458EA4
str 10038CF4 in 02458EA4
data eye_anm_tbl 10038A70
ext 025E1AA4 void pzMonsSeStart(u32 id, cXyz* pos, fpc_ProcID actorID, u32 param, s32 reverb)
ext 025A8BFC u32 pzSetSimpleLand(dPa_control_c* particle, u32 poly, cXyz* pos, csXyz* angle, dKy_tevstr_c* tev, f32 scaleX, f32 scaleY, f32 scaleZ, be<s32>* id, s32 type)
ext 02544830 const char* pzGetNowCutName(dEvent_manager_c* manager, s32 staffID)
ext 02543F10 u32 pzGetEventIndex(dEvent_manager_c* manager, const char* name, s32 archive)
ext 02544044 u32 pzGetEventData(u32 manager, u32 index)
ext 02588230 u8 pzCheckActorInFan(cXyz* pos, fopAc_ac_c* actor, s16 angle, s32 fanAngle, f32 radius, f32 height)
ext 028F0164 void pzDestroyArray(u32 array, u32 count, u32 size, u32 destructor, u32 flags, u32 unused)
ext 025166F0 void pzSphereConstruct(dCcD_Sph* sphere)
ext 027F13DC void pzPacketDestruct(daPz_packet_c* packet, s32 flags)
ext 02515AE8 void pzSphereDestruct(dCcD_Sph* sphere, s32 flags)
ext 024EFD9C void pzAcchDestruct(u32 acch, s32 flags)
ext 02018034 void pzCircleDestruct(u32 circle, s32 flags)
ext 02008B4C void pzLineDestruct(u32 line, s32 flags)
ext 025E89F8 void pzInvisibleDestruct(void* model, s32 flags)
ext 025D50BC void pzActorDestruct(fopAc_ac_c* actor, s32 flags)
ext 0252F4E0 void pzSetDrawPosition(u32 drawList)
ext 0259138C void pzIceEntryDL(mDoExt_McaMorf* morf, s32 material, void* invisibleModel)
ext 027DF9B0 s32 pzFindNameIndex(u32 table, const char* name)
ext 025E789C BOOL pzBtpInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, s32 modify, s32 entry)
ext 025E7CE0 BOOL pzBtkInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, s32 modify, s32 entry)
ext 025E8154 BOOL pzBrkInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, s32 modify, s32 entry)
ext 027F3F8C u32 pzGetMaterialTable(J3DModelData* model)
ext 025E8A48 BOOL pzInvisibleCreate(void* invisibleModel, J3DModel* model)
ext 027F68F4 u32 pzGetMaterialNameTable(J3DModelData* model)
ext 02526E70 u32 pzDemoGetActor(u32 object, u8 id)
ext 02527028 void pzSetDemoData(fopAc_ac_c* actor, s32 flags, mDoExt_McaMorf* morf, const char* archive, s32 a, s32 b, s32 c, s32 d)
ext 02527828 void* pzDemoGetBtp(u32 actor, const char* archive)
ext 025279C8 void* pzDemoGetBtk(u32 actor, const char* archive)
ext 02527A98 void* pzDemoGetBrk(u32 actor, const char* archive)
ext 02041570 void pzEnemyFire(enemyfire* fire)
ext 020402C8 BOOL pzEnemyIce(enemyice* ice)
ext 025E535C void pzMorfPlay(mDoExt_McaMorf* morf, cXyz* pos, u32 sound, s32 reverb)
ext 027F0E04 void pzEntryImmediate(u32 drawBuffer, u32 packet, s32 index)
ext 027F583C void pzUpdateMaterialDisplayList(J3DModel* model, u32 joints)
ext 025E7BE4 void pzBtpEntry(void* animation, J3DModel* model, f32 frame)
ext 025A1458 void pzNpcConstruct(fopNpc_npc_c* actor)
ext 025A9084 void pzRippleConstruct(void* callback)
ext 025E895C void pzInvisibleConstruct(void* model)
ext 025E80D0 void pzBrkConstruct(void* animation)
ext 025E7820 void pzBtpConstruct(void* animation)
ext 027F1278 void pzPacketConstruct(daPz_packet_c* packet)
ext 0259DA18 void pzJointHIOConstruct(void* parameters)
data l_eyeOffset 1046D430
data l_eyeOffset2 1046D43C
data l_rippleScale 1046D448
data l_landScale 1046D5F8
data l_downFootScale 1046D610
data l_downWaistScale 1046D61C
data l_downHeadScale 1046D628
data l_eyeOffsetInitialized 1046D5E0
data l_eyeOffset2Initialized 1046D5E4
data l_waistDebugAngle 1046D5E8
data l_materialNamesInitialized 1046D5EC
data l_rippleScaleInitialized 1046D5F0
data l_landScaleInitialized 1046D5F4
data l_downFootScaleInitialized 1046D604
data l_downWaistScaleInitialized 1046D608
data l_downHeadScaleInitialized 1046D60C
data l_pzCurrentModel 104B462C
data l_pzCurrentMtx 104B4868
data l_HIO 1046D454

str 10038D00
ext 025D5578 BOOL pzFopAcM_SearchByName(s16 name, be<u32>* out)
ext 024EF0F4 s32 pzDBgS_GetAttributeCode(dBgS* bgs, void* poly)
ext 0211D2F8 s32 pzCLib_calcTimer(be<s32>* t)
ext 028F4384 f64 pzSqrtf_d(f32 x)
ext 0259DED0 void pzLookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly)
ext 028249B0 void pzJPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t)
ext 02587128 void pzDLib_setCirclePath(void* path)
ext 0207A9A0 s32 pzCLib_calcTimer_u8(be<u8>* t)
ext 02518CC8 void pzDef_se_set(fopAc_ac_c* a, void* obj, u32 mtrl)
ext 0255F554 void pzDKy_SordFlush_set(cXyz* pos, s32 p)
ext 025D8AB0 fopAc_ac_c* pzFopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* rot, cXyz* scale, f32 speedF, f32 speedY, f32 gravity, s32 bitNo, u32 createFunc)
ext 024F1AFC void pzLineSet(dBgS_LinChk* chk, cXyz* start, cXyz* end, fopAc_ac_c* actor)
ext 025D5A20 fpc_ProcID pzFopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc)
ext 025D54C4 BOOL pzFopAcM_SearchByID(fpc_ProcID id, be<u32>* out)
ext 0200F974 s16 pzCLib_targetAngleX(const cXyz* a, const cXyz* b)
ext 0200ECD4 void pzCLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 025D6958 f64 pzSearchActorDistanceXZ_d(fopAc_ac_c* a, fopAc_ac_c* b)
ext 027DF9B0 s32 pzJUTNameTab_getIndex(u32 tab, u32 name)
ext 02587A0C void pzDLib_bcks_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks, u32 prms, s32 force, s32 hd)
ext 0255FFF4 void pzDKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p)
*/
// clang-format on
