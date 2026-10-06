/**
 * d_a_pz_init.cpp (WWHD): d_a_pz constructors, heaps, creation, helpers and the mode init
 * functions. Written from the WWHD code (the GameCube
 * decompilation has only stubs for d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb); the
 * process id is read after the reverb lookup */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    gabi::call(0x025E1AA4, id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}
/* 025D5578 fopAcM_SearchByName(s16 name, fopAc_ac_c** out) */
static inline BOOL fopAcM_SearchByName(s16 name, be<u32>* out) { return gabi::call<BOOL>(0x025D5578, name, out); }
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0F4, bgs, poly); }
static inline void* gndPoly(daPz_c* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
/* 0211D2F8 cLib_calcTimer<int> (out-of-line copy in another TU) */
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
/* 028F4384 std::sqrtf: GHS keeps the f1 result unrounded */
static inline f64 sqrtf_d(f32 x) { return gabi::call<f64>(0x028F4384, x); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) */
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, dst, eye, defY, maxVel, headOnly);
}
/* JPABaseEmitter (HD) fields used here */
static inline void emitter_setGlobalRGB(u32 e, daPz_c* i_this) {
    u32 tev = gabi::ea(&i_this->mTevstr);
    gabi::store<u8>(e + 0x244, gabi::load<u8>(tev + 0x91));
    gabi::store<u8>(e + 0x246, gabi::load<u8>(tev + 0x95));
    gabi::store<u8>(e + 0x245, gabi::load<u8>(tev + 0x93));
}
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }

/* 0245226C daPz_c::daPz_c (HD: allocates when this == NULL; inline member constructors) */
static daPz_c* daPz_c_ct(daPz_c* i_this) {
    WWHD_FUNC(0x0245226C, daPz_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daPz_c*)operator_new(0x16B8);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x025A1458, i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = PZ_VTBL;
    gabi::call(0x025A9084, i_this->mRipple); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dPa_followEcallBack_ct(&i_this->mFollowCB0, 0, 0);
    dPa_followEcallBack_ct(&i_this->mFollowCB1, 0, 0);
    gabi::call(0x025E895C, i_this->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    /* dKy_tevstr_c (inline): three 0x44-byte blocks from the default at 0x1016E414 */
    static const u32 blocks[3] = {0x0, 0xC0, 0x144};
    for (int b = 0; b < 3; b++) {
        u32 dst = gabi::ea(&i_this->mTevstr) + blocks[b];
        const u32 src = 0x1016E414;
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(dst + o, gabi::load<f32>(src + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(dst + o, gabi::load<u8>(src + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(dst + o, gabi::load<s16>(src + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(dst + o, gabi::load<f32>(src + o));
    }
    static const dBgS_LinChk_vt LINCHK_VT = {0x10038610, 0x10038620, 0x10038640, 0x10038630};
    dBgS_LinChk_ct(&i_this->mLinChk, LINCHK_VT, true); /* dBgS_ObjLinChk */
    /* enemyice (inline) */
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, 0x10038570);
    dBgS_AcchCir_ct(&i_this->mEnemyIce.mBgAcchCir);
    static const dBgS_ObjAcch_vt OBJACCH_VT = {0x100385A0, 0x100385C0, 0x100385B0};
    dBgS_ObjAcch_ct(&i_this->mEnemyIce.mBgAcch, OBJACCH_VT);
    gabi::call(0x024521E0, &i_this->mEnemyFire);   /* enemyfire::enemyfire */
    gabi::call(0x025E80D0, i_this->mBrkAnm);       /* mDoExt_brkAnm::mDoExt_brkAnm */
    mDoExt_btkAnm::ct((mDoExt_btkAnm*)i_this->mBtkAnm);
    gabi::call(0x025E7820, i_this->mBtpAnm);       /* mDoExt_btpAnm::mDoExt_btpAnm */
    gabi::call(0x025E7820, i_this->mBtpAnm2);
    for (int i = 0; i < 4; i++) {
        gabi::call(0x027F1278, &i_this->mPacket[i]); /* packet base constructor */
        i_this->mPacket[i].__vtbl = i < 2 ? 0x10058D10 : 0x10058D40;
    }
    if (gabi::ea(i_this->mMatIdx) == 0) /* sead::SafeArray member "constructor" (never allocates) */
        operator_new(0x40);
    return i_this;
}
VERIFY(0x0245226C, daPz_c_ct);

/* 02452050 */
BOOL daPz_c::bowCreateHeap() {
    WWHD_FUNC(0x02452050, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, 0x17, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x2F5, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100387C0), 0x2F5, STR(0x100387CC));
    mpBowMcaMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                          0x11000022);
    if (mpBowMcaMorf.get() == nullptr || mpBowMcaMorf->getModel() == nullptr)
        return FALSE;
    ((J3DModel_l*)mpBowMcaMorf->getModel())->mUserArea = gabi::ea(this);
    return TRUE;
}
VERIFY(0x02452050, &daPz_c::bowCreateHeap);

/* 0245260C */
void daPz_c::bodyCreateInit() {
    WWHD_FUNC(0x0245260C, void, this);
    if (dComIfGp_isStartStage(0x100387E8 /* "GTower" */))
        eventInfo_setEventId(this, dComIfGp_evmng_getEventIdx(STR(0x100387E0) /* "PZ_TALK" */, 0xFF));
    /* HD: the materials named by bodyCreateHeap (mMatIdx), and a pointer from each */
    u32 data = gabi::ea(J3DModel_getModelData(mpMorf->getModel()));
    u32 num = gabi::load<u32>(data + 0xC);
    u32 mats = gabi::load<u32>(data + 0x10);
    m1394 = gabi::load<u32>(data + 8);
    for (int i = 0; i < 6; i++) {
        u16 idx = gabi::load<u16>(gabi::ea(&mMatIdx[i]) + 2);
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat0[i] = m;
        mMatSh0[i] = gabi::load<u32>(m + 8);
    }
    for (int i = 0; i < 6; i++) {
        u16 idx = gabi::load<u16>(gabi::ea(&mMatIdx[6 + i]) + 2);
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat1[i] = m;
        mMatSh1[i] = gabi::load<u32>(m + 8);
    }
    for (u32 i = 0; i < 4; i++) {
        u32 j = i + 12;
        u16 idx = gabi::load<u16>(gabi::ea(&mMatIdx[j < 16 ? j : 0]) + 2); /* sead::SafeArray */
        u32 m = idx < num ? mats + idx * 0x39C : mats;
        mMat2[i] = m;
        mMatSh2[i] = gabi::load<u32>(m + 8);
    }
}
VERIFY(0x0245260C, &daPz_c::bodyCreateInit);

/* 0245282C */
void daPz_c::modeProc(s32 i_proc, s32 i_mode) {
    WWHD_FUNC(0x0245282C, void, this, i_proc, i_mode);
    /* mode table (0x100387F0): {init, execute} pointers to member functions and a name, 0x14 bytes each */
    const u32 tbl = 0x100387F0;
    if (i_proc == PROC_INIT_e) {
        mMode = i_mode;
        ptmf_call(tbl + i_mode * 0x14, this);
    } else if (i_proc == PROC_EXEC_e) {
        ptmf_call(tbl + mMode * 0x14 + 8, this);
    }
}
VERIFY(0x0245282C, &daPz_c::modeProc);

/* 024528D8 */
void daPz_c::createInit() {
    WWHD_FUNC(0x024528D8, void, this);
    max_health = 30;
    health = 30;
    stealItemLeft = 10;
    /* enemyfire setup */
    mEnemyFire.mpActor = this;
    mEnemyFire.mpMcaMorf = mpMorf.get();
    for (int i = 0; i < 10; i++) {
        mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101CF228 + i);    /* fire_j */
        mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101CF200 + 4 * i); /* fire_sc */
    }
    m0ADC = 0;
    mEnemyIce.mWallRadius = 50.0f;
    mEnemyIce.m00C = 1;
    mEnemyIce.mpActor = this;
    mEnemyIce.mCylHeight = 250.0f;
    m11B0 = 0;
    m11AC = hio_s16(0xF8);
    bodyCreateInit();
    u32 attnFlags = gabi::ea(this) + 0x39C; /* attention_info.flags */
    if (mArg == 0) {
        m11B8 = 1;
        modeProc(PROC_INIT_e, 1);
        gabi::store<u8>(gabi::ea(this) + 0x389, 3); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
        m0B4C = 1;
        gabi::store<u32>(attnFlags, gabi::load<u32>(attnFlags) | 0xA);
        gabi::store<u8>(gabi::ea(this) + 0x38B, 3); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    } else {
        gabi::store<u32>(attnFlags, 0xA);
        modeProc(PROC_INIT_e, 0);
        m0B4C = 1;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -60.0f, -50.0f, -60.0f, 60.0f, 1800.0f, 60.0f);
    mAcchCir.SetWall(80.0f, 60.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.SetRoofNone();
    gravity = hio_f32(0xB0);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(m_cyl_src);
    mCyl.SetStts(&mStts);
    setCollision(30.0f, 130.0f);
    dKy_tevstr_init(&mTevstr, home.roomNo, 0xFF);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
}
VERIFY(0x024528D8, &daPz_c::createInit);

/* 02452D98 */
void daPz_c::setFallSplash() {
    WWHD_FUNC(0x02452D98, void, this);
    settingTevStruct(dKy_getEnvlight(), 2, &current.pos, &mTevstr);
    m0834.z = mEyePos2.z;
    m0834.x = mEyePos2.x;
    m0834.y = mEyePos2.y;
    m0834.y = m0834.y + hio_f32(0xD4);
    if (!mObjAcch.ChkGroundHit()) {
        dPa_followEcallBack_end(&mFollowCB0);
        return;
    }
    if (dBgS_GetAttributeCode(dComIfG_Bgsp(), gndPoly(this)) == 0x17) {
        if (mFollowCB0.getEmitter() == nullptr) {
            dPa_control_set(dComIfGp_getParticle(), 0, 0x23, &m0834, &shape_angle, nullptr, 0xFF,
                            (dPa_levelEcallBack*)&mFollowCB0, -1, nullptr, nullptr, nullptr);
            if (mFollowCB0.getEmitter() != nullptr) {
                dKy_getEnvlight();
                gabi::store<f32>(gabi::ea(mFollowCB0.getEmitter()) + 0x34, 4.0f);
                gabi::store<f32>(gabi::ea(mFollowCB0.getEmitter()) + 0x58, 1.0f);
                emitter_setGlobalRGB(gabi::ea(mFollowCB0.getEmitter()), this);
            }
        }
    } else {
        dPa_followEcallBack_end(&mFollowCB0);
    }
    if (cLib_calcTimer(&m0840) != 0)
        return;
    Mtx34* mtx = getAnmMtx(mpMorf->getModel(), 0);
    gabi::Local<cXyz> pos;
    pos->x = mtx->m[0][3];
    pos->y = mtx->m[1][3];
    pos->z = mtx->m[2][3];
    dPa_control_set(dComIfGp_getParticle(), 5, 0x3F, pos, nullptr, gabi::at<cXyz>(0x101CF234), 0xFF,
                    gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr);
    m0840 = 15;
}
VERIFY(0x02452D98, &daPz_c::setFallSplash);

/* 02452F8C (GameCube name getGndPos: Ganondorf's position) */
void daPz_c::getGndPos() {
    WWHD_FUNC(0x02452F8C, void, this);
    mbHasGanondorf = 0;
    gabi::Local<be<u32>> gnd;
    if (fopAcM_SearchByName(0xF6 /* PROC_GND */, gnd)) {
        fopAc_ac_c* g = gabi::at<fopAc_ac_c>(*gnd);
        mGanondorfPos4.copy(g->eyePos);
        mGanondorfPosCurrent.copy(g->current.pos);
        mbHasGanondorf = 1;
    }
}
VERIFY(0x02452F8C, &daPz_c::getGndPos);

/* 0245300C */
BOOL daPz_c::checkEyeArea(cXyz* i_pos) {
    WWHD_FUNC(0x0245300C, BOOL, this, i_pos);
    s16 target = cLib_targetAngleY(&current.pos, i_pos);
    s32 angle = cLib_distanceAngleS(shape_angle.y, target);
    gabi::Local<cXyz> d;
    cXyz_mi(&current.pos, d, i_pos);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    f64 dist = sqrtf_d(PSVECSquareMag(xz)); /* absXZ */
    if (angle < hio_s16(0x54) && dist < hio_f32(0x58))
        return TRUE;
    return FALSE;
}
VERIFY(0x0245300C, &daPz_c::checkEyeArea);

/* 024530DC */
void daPz_c::setRipple() {
    WWHD_FUNC(0x024530DC, void, this);
    if (!mObjAcch.ChkGroundHit())
        return;
    if (dBgS_GetAttributeCode(dComIfG_Bgsp(), gndPoly(this)) == 0x13) {
        if (gabi::load<u32>(gabi::ea(mRipple) + 4) == 0) { /* getEmitter() */
            /* static cXyz ripple_scale(0.8, 0.8, 0.8) (guard 0x1046D5F0) */
            if (gabi::load<u32>(0x1046D5F0) == 0) {
                gabi::store<u32>(0x1046D5F0, 1);
                gabi::store<f32>(0x1046D448, 0.8f);
                gabi::store<f32>(0x1046D450, 0.8f);
                gabi::store<f32>(0x1046D44C, 0.8f);
            }
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, gabi::at<cXyz>(0x1046D448), 0xFF,
                            (dPa_levelEcallBack*)mRipple, -1, nullptr, nullptr, nullptr);
            if (gabi::load<u32>(gabi::ea(mRipple) + 4) != 0)
                gabi::store<f32>(gabi::ea(mRipple) + 0x10, 0.0f); /* ripple rate */
        }
    } else {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)mRipple);
    }
}
VERIFY(0x024530DC, &daPz_c::setRipple);

/* 024537F8 */
void daPz_c::setMtx() {
    WWHD_FUNC(0x024537F8, void, this);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x024537F8, &daPz_c::setMtx);

/* 024538E0 */
void daPz_c::setHeadSplash() {
    WWHD_FUNC(0x024538E0, void, this);
    settingTevStruct(dKy_getEnvlight(), 2, &current.pos, &mTevstr);
    JPABaseEmitter* e = mFollowCB1.getEmitter();
    if (e == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x837D, &mHeadPos, nullptr, nullptr, 0xFF, (dPa_levelEcallBack*)&mFollowCB1, -1,
                        nullptr, nullptr, nullptr);
        e = mFollowCB1.getEmitter();
        if (e == nullptr)
            return;
    }
    emitter_setGlobalRGB(gabi::ea(e), this);
    J3DMtxBlock_l* blk = ((J3DModel_l*)mpMorf->getModel())->mpMtxBlock;
    u32 em = gabi::ea(mFollowCB1.getEmitter());
    blk->mFlags |= 0x10;
    JPASetRMtxTVecfromMtx(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + 4 * 0x30), em + 0x1F0, em + 0x22C);
}
VERIFY(0x024538E0, &daPz_c::setHeadSplash);

/* 02453A30 */
void daPz_c::setAttention() {
    WWHD_FUNC(0x02453A30, void, this);
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attnPos->z = mEyePos.z;
    attnPos->x = mEyePos.x;
    attnPos->y = mEyePos.y;
    eyePos.copy(mEyePos2);
    attnPos->y = attnPos->y + hio_f32(0x40);
    gabi::Local<cXyz> d;
    cXyz_mi(&mEyePos2, d, &mLookPos);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    sqrtf_d(PSVECSquareMag(xz)); /* absXZ: result unused */
    s16 target = cLib_targetAngleY(&mEyePos2, &mLookPos);
    cLib_distanceAngleS(shape_angle.y, target); /* result unused */
    s16 speed;
    if (!m_jnt.mbTrn)
        speed = 0;
    else if (mEventCut.mTurnSpeed != 0)
        speed = mEventCut.mTurnSpeed;
    else
        speed = hio_s16(0x1A);
    cLib_addCalcAngleS2(&m0B14, speed, 4, 0x800);
    gabi::Local<cXyz> eye;
    eye->x = mEyePos2.x;
    eye->y = mEyePos2.y;
    eye->z = mEyePos2.z;
    lookAtTarget(&m_jnt, &shape_angle.y, &mLookPos, eye, shape_angle.y, m0B14, m0B16);
}
VERIFY(0x02453A30, &daPz_c::setAttention);

/* 02454520 (the matcher names it setEyeBtp; it is setEyeBtk) */
void daPz_c::setEyeBtk(int i_idx) {
    WWHD_FUNC(0x02454520, void, this, i_idx);
    J3DModel* model = mpMorf->getModel();
    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcName, i_idx, SAFESTRING_VTBL);
    if (btk == nullptr) /* JUT_ASSERT(0x742, btk != NULL) */
        JUT_ASSERT_fail(STR(0x10038A58), 0x742, STR(0x10038A64));
    ((mDoExt_btkAnm*)mBtkAnm)->init(J3DModel_getModelData(model), btk, true, 0, 1.0f, 0, -1, true, 0);
}
VERIFY(0x02454520, &daPz_c::setEyeBtk);

/* 02455828 */
void daPz_c::modeWaitInit() {
    WWHD_FUNC(0x02455828, void, this);
    m0B50 = 0.0f;
    setAnm(1, 0, 0xF);
    m_jnt.mbTrn = 0;
    m_jnt.mbBackBoneLock = 1;
    m0B18 = 30;
    m_jnt.mbHeadLock = 1;
    m0B16 = 1;
}
VERIFY(0x02455828, &daPz_c::modeWaitInit);

/* 02456990 */
void daPz_c::modeAttackWaitInit() {
    WWHD_FUNC(0x02456990, void, this);
    m0B18 = 20;
    m0B1C = 10;
    m0B50 = 0.0f;
    setAnm(2, 0, 0xF);
    m0B16 = 0;
    m_jnt.mbHeadLock = 0;
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x02456990, &daPz_c::modeAttackWaitInit);

/* 02457464 */
void daPz_c::modeDefendInit() {
    WWHD_FUNC(0x02457464, void, this);
    setAnm(10, 1, 0xF);
    m0B18 = 30;
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 1;
    m_jnt.mbBackBoneLock = 1;
    m0B16 = 1;
}
VERIFY(0x02457464, &daPz_c::modeDefendInit);

/* 02457664 */
void daPz_c::modeDownInit() {
    WWHD_FUNC(0x02457664, void, this);
    u32 attnFlags = gabi::ea(this) + 0x39C;
    gabi::store<u32>(attnFlags, gabi::load<u32>(attnFlags) & ~0xAu);
    setAnm(7, 1, 0xF);
    fopAcM_monsSeStart(this, 0x4960, 0);
    m0B16 = 0;
    m0B50 = 0.0f;
    m_jnt.mbHeadLock = 1;
    m_jnt.mbTrn = 0;
    speedF = 0.0f;
    m_jnt.mbBackBoneLock = 1;
    speed.y = hio_f32(0xC4);
    m11A0 = hio_f32(0xC8);
    m0B18 = hio_s16(0xCC);
}
VERIFY(0x02457664, &daPz_c::modeDownInit);

/* 02457D10 */
void daPz_c::modeAfraidInit() {
    WWHD_FUNC(0x02457D10, void, this);
    setAnm(11, 1, 0xF);
    m0B16 = 0;
    speedF = 0.0f;
    m_jnt.mbHeadLock = 0;
    m0B50 = 0.0f;
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x02457D10, &daPz_c::modeAfraidInit);

/* 02457F80 */
void daPz_c::modeSideStepInit() {
    WWHD_FUNC(0x02457F80, void, this);
    if (m07E0 != 2 && m07E0 != 3)
        setAnm(11, 1, 0xF);
    m0B16 = 0;
    speedF = 0.0f;
    m0B50 = 0.0f;
    speed.y = hio_f32(0xBC);
    m11A0 = hio_f32(0xC0);
}
VERIFY(0x02457F80, &daPz_c::modeSideStepInit);

/* 024581F4 */
void daPz_c::modeBackStepInit() {
    WWHD_FUNC(0x024581F4, void, this);
    if (m07E0 != 2 && m07E0 != 3)
        setAnm(11, 1, 0xF);
    m0B16 = 0;
    speedF = 0.0f;
    m0B50 = 0.0f;
    speed.y = hio_f32(0xB4);
    m11A0 = hio_f32(0xB8);
}
VERIFY(0x024581F4, &daPz_c::modeBackStepInit);

/* 02458438 */
void daPz_c::modeTalk() {
    WWHD_FUNC(0x02458438, void, this);
    gabi::Local<cXyz> eye;
    dNpc_playerEyePos(eye, hio_f32(0x4));
    m_jnt.mbTrn = 1;
    mLookPos.copy(*eye);
    if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        dComIfGp_event_reset();
        modeProc(PROC_INIT_e, 2);
    }
}
VERIFY(0x02458438, &daPz_c::modeTalk);

/* 024584C0 */
void daPz_c::modeTalkInit() {
    WWHD_FUNC(0x024584C0, void, this);
    setAnm(2, 1, 0xF);
    m_jnt.mbHeadLock = 0;
    m0B50 = 0.0f;
    m0B16 = 0;
    speedF = 0.0f;
    m_jnt.mbBackBoneLock = 0;
    if (m0ADC == 0)
        m11B0 = 1;
    else
        m11AC = hio_s16(0xF8 + 2 * m0ADC);
}
VERIFY(0x024584C0, &daPz_c::modeTalkInit);

/* 0245897C */
void daPz_c::modeFollowInit() {
    WWHD_FUNC(0x0245897C, void, this);
    u32 attnFlags = gabi::ea(this) + 0x39C;
    gabi::store<u32>(attnFlags, gabi::load<u32>(attnFlags) | 0xA);
    u32 o = 2 * m0ADC;
    f32 rnd = cM_rndF((f32)hio_s16(0x8C + o));
    m0B18 = gabi::ftoi((f32)hio_s16(0x86 + o) + rnd);
    m0B1C = 10;
    m0B20 = 120;
    setAnm(3, 0, 0xF);
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 0;
    m0B16 = 0;
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x0245897C, &daPz_c::modeFollowInit);

/* 02458A74 */
u16 daPz_c::next_msgStatus(be<u32>* i_msgNo) {
    WWHD_FUNC(0x02458A74, u16, this, i_msgNo);
    if (*i_msgNo == 0x3563) {
        *i_msgNo = 0x3564;
        return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    }
    if (*i_msgNo == 0x3564) {
        gabi::Local<be<u32>> gnd;
        if (fopAcM_SearchByName(0xF6 /* PROC_GND */, gnd)) {
            fopAc_ac_c* g = gabi::at<fopAc_ac_c>(*gnd);
            if (g != nullptr && gabi::load<u8>(gabi::ea(g) + 0x3A8) == 0)
                gabi::store<u8>(gabi::ea(g) + 0x3A8, 0x23); /* Ganondorf: next demo step */
        }
    }
    return 0x10; /* fopMsgStts_MSG_ENDS_e */
}
VERIFY(0x02458A74, &daPz_c::next_msgStatus);

/* 02458AEC */
u32 daPz_c::getMsg() {
    WWHD_FUNC(0x02458AEC, u32, this);
    u32 msgNo = 0x3562;
    if (m0ADC == 0) {
        msgNo = 0x3562;
    } else if (m0ADC == 2) {
        if (m1195 == 0) {
            m1195 = 1;
            msgNo = 0x3563;
        } else {
            msgNo = 0x3565;
        }
    }
    return msgNo;
}
VERIFY(0x02458AEC, &daPz_c::getMsg);

/* 02458B34 */
void daPz_c::anmAtr(u32 i_msgStatus) {
    WWHD_FUNC(0x02458B34, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: /* fopMsgStts_MSG_TYPING_e */
        if (m07E4 == 0) {
            u8 atr = gabi::load<u8>(dComIfGp_ea() + 0x5BC5); /* dComIfGp_getMesgAnimeAttrInfo() */
            m07E4 = 1;
            if (atr >= 2) /* JUT_ASSERT(0x3A1, atr >= 0 && atr < 2) */
                JUT_ASSERT_fail(STR(0x10038CA8), 0x3A1, STR(0x10038C94));
            setAnm(gabi::load<s8>(0x10038C90 + atr), 0, 0xF);
        }
        break;
    case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
        m07E4 = 0;
        break;
    }
}
VERIFY(0x02458B34, &daPz_c::anmAtr);

/* 02458C04 daPz_HIO_c::daPz_HIO_c (HD: allocates when this == NULL) */
static u8* daPz_HIO_c_ct(u8* i_this) {
    WWHD_FUNC(0x02458C04, u8*, i_this);
    if (i_this == nullptr) {
        i_this = (u8*)operator_new(0x10C);
        if (i_this == nullptr)
            return i_this;
    }
    u32 p = gabi::ea(i_this);
    gabi::store<u32>(p + 0x0, 0x10038650); /* vtable */
    gabi::call(0x0259DA18, p + 4);         /* member HIO object constructor */
    auto b = [p](u32 o, u8 v) { gabi::store<u8>(p + o, v); };
    auto h = [p](u32 o, s16 v) { gabi::store<s16>(p + o, v); };
    auto f = [p](u32 o, f32 v) { gabi::store<f32>(p + o, v); };
    b(0x32, 0); b(0x33, 0); b(0x2C, 0); b(0x2D, 1);
    for (u32 i = 0; i < 10; i++) b(0x34 + i, 0);
    b(0x22, 0); b(0x2F, 0); b(0x31, 0);
    f(0x04, -20.0f);
    h(0x08, 0x1FFE); h(0x0A, 7000); h(0x0C, 10000); h(0x0E, 13000); h(0x10, -2500); h(0x12, -7000); h(0x14, -10000);
    h(0x16, -13000); h(0x18, 0x1000); h(0x1A, 0x800);
    f(0x1C, 50.0f); h(0x20, 13000); f(0x24, 400.0f);
    f(0x40, 60.0f); f(0x44, 1.2f); f(0x48, 2.0f); f(0x4C, 0.9f); f(0x50, 10.0f); h(0x54, 0x2000); f(0x58, 200.0f);
    f(0x5C, 100.0f); f(0x64, 100.0f); f(0x68, 0.0f); f(0x70, 100.0f); f(0x74, 100.0f); f(0x7C, 80.0f);
    h(0x80, 30); h(0x84, 30); h(0x86, 300); h(0x8A, 100); h(0x8C, 0); h(0x90, 100); h(0x92, 30); h(0x96, 30); h(0x98, 0);
    h(0x9C, 60);
    f(0xA0, 1000.0f); f(0xA8, 1000.0f); f(0xAC, 100.0f); f(0xB0, -2.5f); f(0xB4, 20.0f); f(0xB8, 20.0f); f(0xBC, 20.0f);
    f(0xC0, 20.0f); f(0xC4, 4.0f); f(0xC8, 15.0f); h(0xCC, 14); f(0xD0, 600.0f); f(0xD4, 15.0f); f(0xD8, 30.0f);
    h(0xDC, 40); h(0xDE, 40); h(0xE0, 5); h(0xE2, 4); h(0xE4, 6); h(0xE6, 4); h(0xE8, 6);
    f(0xEC, 800.0f); f(0xF0, 2.0f); f(0xF4, 1.0f); h(0xF8, 60); h(0xFC, 600); f(0x100, 3000.0f); f(0x108, 800.0f);
    return i_this;
}
VERIFY(0x02458C04, daPz_HIO_c_ct);
