/**
 * d_a_pz_init.cpp (WWHD): d_a_pz constructors, heaps, creation, helpers and the mode init
 * functions. Written from the WWHD code (the GameCube
 * decompilation has only stubs for d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static cXyz l_fallSplashScale = {0.75f, 0.75f, 0.75f};
static dPa_levelEcallBack l_fallSplashCallback;
static s8 fire_j[10] = {7, 2, 12, 13, 17, 18, 22, 23, 25, 26};
static f32 fire_sc[10] = {2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static s8 msg_anm[2] = {12, 13};

static PzLightParametersHD l_defaultLight;
static const dBgS_LinChk_vt LINCHK_VT = {PZ_VT_OBJ_LINE, PZ_VT_OBJ_LINE_INFO,
                                         PZ_VT_OBJ_LINE_POLYGON, PZ_VT_OBJ_LINE_GROUP};
static const dBgS_ObjAcch_vt OBJACCH_VT = {PZ_VT_OBJ_ACCH, PZ_VT_ACCH_POLYGON, PZ_VT_ACCH_GROUP};

/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb); the
 * process id is read after the reverb lookup */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    pzMonsSeStart(id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}
/* 025D5578 pzFopAcM_SearchByName(s16 name, fopAc_ac_c** out) */

// HD: dBgS::GetAttributeCode(cBgS_PolyInfo&)
// Native binding 024EF0F4

static inline void* gndPoly(daPz_c* a) {
    return static_cast<cBgS_PolyInfo*>(&a->mObjAcch.m_gnd);
}
// HD: pzCLib_calcTimer<int> (out-of-line copy in another TU)
// Native binding 0211D2F8

// HD: std::sqrtf: GHS keeps the f1 result unrounded
// Native binding 028F4384

/* 0259DED0 dNpc_JntCtrl_c::pzLookAtTarget(s16* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8
 * headOnly) */

/* JPABaseEmitter (HD) fields used here */
static inline void emitter_setGlobalRGB(u32 address, daPz_c* i_this) {
    PzEmitterHD* emitter = pzGuest<PzEmitterHD>(address);
    emitter->mGlobalColor[0] = (u8)(s16)i_this->mTevstr.mColorC0.r;
    emitter->mGlobalColor[2] = (u8)(s16)i_this->mTevstr.mColorC0.b;
    emitter->mGlobalColor[1] = (u8)(s16)i_this->mTevstr.mColorC0.g;
}

// HD: daPz_c::daPz_c (HD: allocates when this == NULL; inline member constructors)
/* 0245226C */
daPz_c* daPz_c_ct(daPz_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daPz_c*)operator_new(0x16B8);
        if (i_this == nullptr)
            return i_this;
    }
    pzNpcConstruct(i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = PZ_VTBL;
    pzRippleConstruct(&i_this->mRipple); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dPa_followEcallBack_ct(&i_this->mFollowCB0, 0, 0);
    dPa_followEcallBack_ct(&i_this->mFollowCB1, 0, 0);
    pzInvisibleConstruct(
        i_this->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */

    PzTevParametersHD* parameters = reinterpret_cast<PzTevParametersHD*>(&i_this->mTevstr);
    for (int b = 0; b < 3; b++) {
        PzLightParametersHD* dst = b == 0   ? &parameters->mLight
                                   : b == 1 ? &parameters->mCopy0
                                            : &parameters->mCopy1;
        for (int o = 0; o < 6; o++)
            dst->mVectors[o] = l_defaultLight.mVectors[o];
        for (int o = 0; o < 4; o++)
            dst->mColor[o] = l_defaultLight.mColor[o];
        for (int o = 0; o < 4; o++)
            dst->mAttenuation[o] = l_defaultLight.mAttenuation[o];
        for (int o = 0; o < 8; o++)
            dst->mParameters[o] = l_defaultLight.mParameters[o];
    }

    dBgS_LinChk_ct(&i_this->mLinChk, LINCHK_VT, true); /* dBgS_ObjLinChk */
    /* enemyice (inline) */
    dCcD_Stts_ct(&i_this->mEventIce.mStts);
    dCcD_Cyl_ct(&i_this->mEventIce.mCyl, PZ_VT_ICE_CYLINDER);
    dBgS_AcchCir_ct(&i_this->mEventIce.mBgAcchCir);

    dBgS_ObjAcch_ct(&i_this->mEventIce.mBgAcch, OBJACCH_VT);
    enemyfire_ct(&i_this->mEnemyFire); /* enemyfire::enemyfire */
    pzBrkConstruct(&i_this->mBrkAnm);  /* mDoExt_brkAnm::mDoExt_brkAnm */
    mDoExt_btkAnm::ct((mDoExt_btkAnm*)&i_this->mBtkAnm);
    pzBtpConstruct(&i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    pzBtpConstruct(&i_this->mBtpAnm2);
    for (int i = 0; i < 4; i++) {
        pzPacketConstruct(&i_this->mPacket[i]); /* packet base constructor */
        i_this->mPacket[i].__vtbl = i < 2 ? PZ_VT_FACE_PACKET : PZ_VT_SPECIAL_PACKET;
    }
    if (gabi::ea(i_this->mMatIdx) == 0) /* sead::SafeArray member "constructor" (never allocates) */
        operator_new(0x40);
    return i_this;
}

/* 02452050 */
BOOL daPz_c::bowCreateHeap() {

    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(l_arcName, PZ_BDL_BOW, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x2F5, modelData != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x2F5, "modelData != (0)");
    mpBowMcaMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f,
                                          0, -1, 1, nullptr, 0x80000, 0x11000022);
    if (mpBowMcaMorf.get() == nullptr || mpBowMcaMorf->getModel() == nullptr)
        return FALSE;
    ((J3DModel*)mpBowMcaMorf->getModel())->mUserArea = gabi::ea(this);
    return TRUE;
}

/* 0245260C */
void daPz_c::bodyCreateInit() {

    if (pzIsStartStage("GTower"))
        eventInfo_setEventId(this, dComIfGp_evmng_getEventIdx("PZ_TALK" /* "PZ_TALK" */, 0xFF));
    /* HD: the materials named by bodyCreateHeap (mMatIdx), and a pointer from each */
    u32 data = gabi::ea(J3DModel_getModelData(mpMorf->getModel()));
    u32 num = pzGuest<J3DModelData>(data)->mMaterialNum.get();
    u32 mats = pzGuest<J3DModelData>(data)->mMaterials.get();
    mpJointNodes = pzGuest<J3DModelData>(data)->mJointNodes.get();
    for (int i = 0; i < 6; i++) {
        u16 idx = (u16)mMatIdx[i].get();
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat0[i] = m;
        mMatSh0[i] = gabi::ea(pzGuest<PzMaterialHD>(m)->mpShape.get());
    }
    for (int i = 0; i < 6; i++) {
        u16 idx = (u16)mMatIdx[6 + i].get();
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat1[i] = m;
        mMatSh1[i] = gabi::ea(pzGuest<PzMaterialHD>(m)->mpShape.get());
    }
    for (u32 i = 0; i < 4; i++) {
        u32 j = i + 12;
        u16 idx = (u16)mMatIdx[j < 16 ? j : 0].get(); /* sead::SafeArray */
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat2[i] = m;
        mMatSh2[i] = gabi::ea(pzGuest<PzMaterialHD>(m)->mpShape.get());
    }
}

/* 024528D8 */
void daPz_c::createInit() {

    max_health = 30;
    health = 30;
    stealItemLeft = 10;
    /* enemyfire setup */
    mEnemyFire.mpActor = this;
    mEnemyFire.mpMcaMorf = mpMorf.get();
    for (int i = 0; i < 10; i++) {
        mEnemyFire.mFlameJntIdxs[i] = fire_j[i];   /* fire_j */
        mEnemyFire.mParticleScale[i] = fire_sc[i]; /* fire_sc */
    }
    m08B0 = 0;
    mEventIce.mWallRadius = 50.0f;
    mEventIce.m00C = 1;
    mEventIce.mpActor = this;
    mEventIce.mCylHeight = 250.0f;
    m0F80 = 0;
    m0F7C = l_HIO.mTalkWaitTime[0];
    bodyCreateInit();
    be<u32>& attnFlags = attention_info.flags; /* attention_info.flags */
    if (mArg == 0) {
        m0F88 = 1;
        modeProc(PROC_INIT_e, MODE_MOVE);
        attention_info.distances[1] = 3; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
        m0920 = 1;
        attnFlags |= 0xA;
        attention_info.distances[3] = 3; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    } else {
        attnFlags = 0xA;
        modeProc(PROC_INIT_e, MODE_WAIT);
        m0920 = 1;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -50.0f, -60.0f, 60.0f, 1800.0f, 60.0f);
    mAcchCir.SetWall(80.0f, 60.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.SetRoofNone();
    gravity = l_HIO.mGravity;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&m_cyl_src);
    mCyl.SetStts(&mStts);
    setCollision(30.0f, 130.0f);
    pzDKy_tevstr_init(&mTevstr, home.roomNo, 0xFF);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
}

/* 02452D98 */
void daPz_c::setFallSplash() {

    settingTevStruct(dKy_getEnvlight(), 2, &current.pos, &mTevstr);
    mFallSplashPos.z = mEyePos2.z;
    mFallSplashPos.x = mEyePos2.x;
    mFallSplashPos.y = mEyePos2.y;
    mFallSplashPos.y = mFallSplashPos.y + l_HIO.mSplashYOffset;
    if (!mObjAcch.ChkGroundHit()) {
        dPa_followEcallBack_end(&mFollowCB0);
        return;
    }
    if (pzDBgS_GetAttributeCode(dComIfG_Bgsp(), gndPoly(this)) == 0x17) {
        if (mFollowCB0.getEmitter() == nullptr) {
            dPa_control_set(dComIfGp_getParticle(), 0, PZ_PARTICLE_SPLASH, &mFallSplashPos,
                            &shape_angle, nullptr, 0xFF, (dPa_levelEcallBack*)&mFollowCB0, -1,
                            nullptr, nullptr, nullptr);
            if (mFollowCB0.getEmitter() != nullptr) {
                dKy_getEnvlight();
                mFollowCB0.getEmitter()->mRate = 4.0f;
                mFollowCB0.getEmitter()->mSpread = 1.0f;
                emitter_setGlobalRGB(gabi::ea(mFollowCB0.getEmitter()), this);
            }
        }
    } else {
        dPa_followEcallBack_end(&mFollowCB0);
    }
    if (pzCLib_calcTimer(&mFallSplashTimer) != 0)
        return;
    Mtx34* mtx = getAnmMtx(mpMorf->getModel(), 0);
    cXyz pos;
    pos.x = mtx->m[0][3];
    pos.y = mtx->m[1][3];
    pos.z = mtx->m[2][3];
    dPa_control_set(dComIfGp_getParticle(), 5, PZ_PARTICLE_FALL_SPLASH, &pos, nullptr,
                    &l_fallSplashScale, 0xFF, &l_fallSplashCallback, -1, nullptr, nullptr, nullptr);
    mFallSplashTimer = 15;
}

// HD: (GameCube name getGndPos: Ganondorf's position)
/* 02452F8C */
void daPz_c::getGndPos() {

    mbHasGanondorf = 0;
    gabi::Local<be<u32>> gnd;
    if (pzFopAcM_SearchByName(fpcNm_GND_e, gnd)) {
        fopAc_ac_c* g = pzGuest<fopAc_ac_c>(*gnd);
        mGanondorfPos4.copy(g->eyePos);
        mGanondorfPosCurrent.copy(g->current.pos);
        mbHasGanondorf = 1;
    }
}

/* 0245300C */
BOOL daPz_c::checkEyeArea(cXyz* i_pos) {

    s16 target = cLib_targetAngleY(&current.pos, i_pos);
    s32 angle = cLib_distanceAngleS(shape_angle.y, target);
    cXyz d;
    cXyz_mi(&current.pos, &d, i_pos);
    cXyz xz;
    xz.x = d.x;
    xz.y = 0.0f;
    xz.z = d.z;
    f64 dist = pzSqrtf_d(PSVECSquareMag(&xz)); /* absXZ */
    if (angle < l_HIO.mEyeAreaAngle && dist < l_HIO.mEyeAreaRadius)
        return TRUE;
    return FALSE;
}

/* 024530DC */
void daPz_c::setRipple() {

    if (!mObjAcch.ChkGroundHit())
        return;
    if (pzDBgS_GetAttributeCode(dComIfG_Bgsp(), gndPoly(this)) == 0x13) {
        if (gabi::ea(mRipple.mpEmitter.get()) == 0) { /* getEmitter() */
            // HD: Preserve the compiler's component and guard write order.

            if (l_rippleScaleInitialized == 0) {
                l_rippleScaleInitialized = 1;
                l_rippleScale.x = 0.8f;
                l_rippleScale.z = 0.8f;
                l_rippleScale.y = 0.8f;
            }
            dPa_control_set(dComIfGp_getParticle(), 5, PZ_PARTICLE_RIPPLE, &current.pos, nullptr,
                            &l_rippleScale, 0xFF, (dPa_levelEcallBack*)&mRipple, -1, nullptr,
                            nullptr, nullptr);
            if (gabi::ea(mRipple.mpEmitter.get()) != 0)
                mRipple.mRate = 0.0f; /* ripple rate */
        }
    } else {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRipple);
    }
}

/* 024537F8 */
void daPz_c::setMtx() {

    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 024538E0 */
void daPz_c::setHeadSplash() {

    settingTevStruct(dKy_getEnvlight(), 2, &current.pos, &mTevstr);
    JPABaseEmitter* e = mFollowCB1.getEmitter();
    if (e == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, PZ_PARTICLE_HEAD_SPLASH, &mHeadPos, nullptr,
                        nullptr, 0xFF, (dPa_levelEcallBack*)&mFollowCB1, -1, nullptr, nullptr,
                        nullptr);
        e = mFollowCB1.getEmitter();
        if (e == nullptr)
            return;
    }
    emitter_setGlobalRGB(gabi::ea(e), this);
    J3DMtxBuffer_hd* blk = ((J3DModel*)mpMorf->getModel())->mpMtxBuffer;
    u32 em = gabi::ea(mFollowCB1.getEmitter());
    blk->mFlags |= 0x10;
    pzJPASetRMtxTVecfromMtx(pzGuest<Mtx34>(blk->mpAnmMtx) + 4,
                            gabi::ea(&pzGuest<PzEmitterHD>(em)->mGlobalRotation),
                            gabi::ea(&pzGuest<PzEmitterHD>(em)->mGlobalTranslation));
}

/* 02453A30 */
void daPz_c::setAttention() {

    cXyz* attnPos = &attention_info.position; /* attention_info.position */
    attnPos->z = mEyePos.z;
    attnPos->x = mEyePos.x;
    attnPos->y = mEyePos.y;
    eyePos.copy(mEyePos2);
    attnPos->y = attnPos->y + l_HIO.mAttentionYOffset;
    cXyz d;
    cXyz_mi(&mEyePos2, &d, &m08C4);
    cXyz xz;
    xz.x = d.x;
    xz.y = 0.0f;
    xz.z = d.z;
    pzSqrtf_d(PSVECSquareMag(&xz)); /* absXZ: result unused */
    s16 target = cLib_targetAngleY(&mEyePos2, &m08C4);
    cLib_distanceAngleS(shape_angle.y, target); /* result unused */
    s16 speed;
    if (!m_jnt.mbTrn)
        speed = 0;
    else if (mEventCut.mTurnSpeed != 0)
        speed = mEventCut.mTurnSpeed;
    else
        speed = l_HIO.mTurnSpeed;
    cLib_addCalcAngleS2(&mLookTurnSpeed, speed, 4, 0x800);
    cXyz eye;
    eye.x = mEyePos2.x;
    eye.y = mEyePos2.y;
    eye.z = mEyePos2.z;
    pzLookAtTarget(&m_jnt, &shape_angle.y, &m08C4, &eye, shape_angle.y, mLookTurnSpeed, mbHeadOnly);
}

// HD: The texture transform has its own initialization entry point.
/* 02454520 */
void daPz_c::setEyeBtk(int i_idx) {

    J3DModel* model = mpMorf->getModel();
    J3DAnmTextureSRTKey* btk =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcName, i_idx, SAFESTRING_VTBL);
    if (btk == nullptr) /* JUT_ASSERT(0x742, btk != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x742, "btk != (0)");
    ((mDoExt_btkAnm*)&mBtkAnm)
        ->init(J3DModel_getModelData(model), btk, true, 0, 1.0f, 0, -1, true, 0);
}

/* 02458A74 */
u16 daPz_c::next_msgStatus(be<u32>* i_msgNo) {

    if (*i_msgNo == PZ_MSG_ADVICE) {
        *i_msgNo = PZ_MSG_ADVICE_CONTINUE;
        return fopMsgStts_MSG_CONTINUES_e;
    }
    if (*i_msgNo == PZ_MSG_ADVICE_CONTINUE) {
        gabi::Local<be<u32>> gnd;
        if (pzFopAcM_SearchByName(fpcNm_GND_e, gnd)) {
            fopAc_ac_c* g = pzGuest<fopAc_ac_c>(*gnd);
            if (g != nullptr && g->stealItemBitNo == 0)
                g->stealItemBitNo = 0x23; /* Ganondorf: next demo step */
        }
    }
    return fopMsgStts_MSG_ENDS_e;
}

/* 02458AEC */
u32 daPz_c::getMsg() {

    u32 msgNo = PZ_MSG_FIRST;
    if (m08B0 == 0) {
        msgNo = PZ_MSG_FIRST;
    } else if (m08B0 == 2) {
        if (m0F65 == 0) {
            m0F65 = 1;
            msgNo = PZ_MSG_ADVICE;
        } else {
            msgNo = PZ_MSG_REPEAT;
        }
    }
    return msgNo;
}

/* 02458B34 */
void daPz_c::anmAtr(u32 i_msgStatus) {

    switch (i_msgStatus) {
    case 6: /* fopMsgStts_MSG_TYPING_e */
        if (mMessageAnmStarted == 0) {
            u8 atr = pzGuest<PzMessageAttributeHD>(dComIfGp_ea())
                         ->mAttribute.get(); /* dComIfGp_getMesgAnimeAttrInfo() */
            mMessageAnmStarted = 1;
            if (atr >= 2) /* JUT_ASSERT(0x3A1, atr >= 0 && atr < 2) */
                JUT_ASSERT_fail("d_a_pz.cpp", 0x3A1, "atr >= 0 && atr < 2");
            setAnm(msg_anm[atr], 0, 0xF);
        }
        break;
    case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
        mMessageAnmStarted = 0;
        break;
    }
}

// HD: daPz_HIO_c::daPz_HIO_c (HD: allocates when this == NULL)
/* 02458C04 */
u8* daPz_HIO_c_ct(u8* i_this) {

    if (i_this == nullptr) {
        i_this = (u8*)operator_new(0x10C);
        if (i_this == nullptr)
            return i_this;
    }
    daPz_HIO_c* hio = reinterpret_cast<daPz_HIO_c*>(i_this);
    hio->__vtbl = PZ_VT_HIO;                      /* vtable */
    pzJointHIOConstruct(&hio->mPlayerEyeYOffset); /* member HIO object constructor */
    hio->mbDebugSideStep = 0;
    hio->mbDebugWarpToPlayer = 0;
    hio->mDebugFlag = 0;
    hio->mbEnableAttack = 1;
    for (u32 i = 0; i < 10; i++)
        hio->mDebugButtons[i] = 0;
    hio->mJointDebugFlag = 0;
    hio->mDebugAnm = 0;
    hio->mDebugFlag2 = 0;
    hio->mPlayerEyeYOffset = -20.0f;
    hio->mHeadMaxX = 0x1FFE;
    hio->mHeadMinX = 7000;
    hio->mBackboneMaxX = 10000;
    hio->mBackboneMinX = 13000;
    hio->mHeadMaxY = -2500;
    hio->mHeadMinY = -7000;
    hio->mBackboneMaxY = -10000;
    hio->mBackboneMinY = -13000;
    hio->mMaxJointStep = 0x1000;
    hio->mTurnSpeed = 0x800;
    hio->mJointRadius = 50.0f;
    hio->mJointAngle = 13000;
    hio->mJointDistance = 400.0f;
    hio->mAttentionYOffset = 60.0f;
    hio->mRunRate = 1.2f;
    hio->mRunMaxRate = 2.0f;
    hio->mRunMinRate = 0.9f;
    hio->mMoveSpeed = 10.0f;
    hio->mEyeAreaAngle = 0x2000;
    hio->mEyeAreaRadius = 200.0f;
    hio->mAimSpread[0] = 100.0f;
    hio->mAimSpread[2] = 100.0f;
    hio->mPlayerAimChance[0] = 0.0f;
    hio->mPlayerAimChance[2] = 100.0f;
    hio->mStraightAimChance[0] = 100.0f;
    hio->mStraightAimChance[2] = 80.0f;
    hio->mAttackWaitTime[0] = 30;
    hio->mAttackWaitTime[2] = 30;
    hio->mMoveWaitTime[0] = 300;
    hio->mMoveWaitTime[2] = 100;
    hio->mMoveWaitRandom[0] = 0;
    hio->mMoveWaitRandom[2] = 100;
    hio->mAttackChargeTime[0] = 30;
    hio->mAttackChargeTime[2] = 30;
    hio->mAttackChargeRandom[0] = 0;
    hio->mAttackChargeRandom[2] = 60;
    hio->mCircleRadius[0] = 1000.0f;
    hio->mCircleRadius[2] = 1000.0f;
    hio->mCircleSpeed = 100.0f;
    hio->mGravity = -2.5f;
    hio->mBackStepJumpSpeed = 20.0f;
    hio->mBackStepSpeed = 20.0f;
    hio->mSideStepJumpSpeed = 20.0f;
    hio->mSideStepSpeed = 20.0f;
    hio->mDownJumpSpeed = 4.0f;
    hio->mDownSlideSpeed = 15.0f;
    hio->mDownSlideTime = 14;
    hio->mDodgeDistance = 600.0f;
    hio->mSplashYOffset = 15.0f;
    hio->mReverseDirectionChance = 30.0f;
    hio->mDebugTimer = 40;
    hio->mDebugTimer2 = 40;
    hio->mHitCooldown = 5;
    hio->mGanondorfHitLimit = 4;
    hio->mGanondorfTargetAttacks = 6;
    hio->mPlayerHitLimit = 4;
    hio->mPlayerTargetAttacks = 6;
    hio->mFollowDistance = 800.0f;
    hio->mDownMorfTime = 2.0f;
    hio->mDownWaitMorfTime = 1.0f;
    hio->mTalkWaitTime[0] = 60;
    hio->mTalkWaitTime[2] = 600;
    hio->mTalkDistance[0] = 3000.0f;
    hio->mTalkDistance[2] = 800.0f;
    return i_this;
}

// clang-format off
/* WWHD glue
data l_fallSplashScale 101CF234
data l_fallSplashCallback 1047B2E4
str 100387E8 in 0245260C
data l_defaultLight 1016E414
data fire_j 101CF228
data fire_sc 101CF200
data msg_anm 10038C90
str 100387C0 in 02452050
str 100387CC in 02452050
str 100387E0 in 0245260C
str 10038A58 in 02454520
str 10038A64 in 02454520
str 10038CA8 in 02458B34
str 10038C94 in 02458B34
str 10038D00
*/
// clang-format on
