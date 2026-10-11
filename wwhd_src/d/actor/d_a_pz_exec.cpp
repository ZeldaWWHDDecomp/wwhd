/**
 * d_a_pz_exec.cpp (WWHD): d_a_pz heap creation, demo, execute and body drawing. Written from the WWHD code (the GameCube decompilation has
 * only stubs for d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static PzRegistrationHD l_materialRegistration;
static PzRendererHD l_pzRenderer;
static PzDemoSingletonHD l_demoSingleton;

static const char* material_name_literals[16] = {
    "m_pz_eyeLdamB", "m_pz_eyeLdamA",  "m_pz_eyeL",      "m_pz_eyeRdamB",
    "m_pz_eyeRdamA", "m_pz_eyeR",      "m_pz_mayuLdamB", "m_pz_mayuLdamA",
    "m_pz_mayuL",    "m_pz_mayuRdamB", "m_pz_mayuRdamA", "m_pz_mayuR",
    "m_pz_shadowL",  "m_pz_shadowR",   "m_pz_head",      "m_pz_hair"};
static PzMaterialNamesStaticHD material_names;

/* resources by index, the SafeString key built on the stack */
static inline void* getPzRes(s32 index) {
    return dComIfG_getObjectRes(l_arcName, index, SAFESTRING_VTBL);
}
/* 025E789C mDoExt_btpAnm::init / 025E7CE0 mDoExt_btkAnm::init / 025E8154 mDoExt_brkAnm::init
 * (anm, modelData, res, anmPlay, mode, rate, start, end, modify, entry) */
static inline BOOL btpAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return pzBtpInit(anm, d, res, 1, 0, 1.0f, 0, -1, modify, 0);
}
static inline BOOL btkAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return pzBtkInit(anm, d, res, 1, 0, 1.0f, 0, -1, modify, 0);
}
static inline BOOL brkAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return pzBrkInit(anm, d, res, 1, 2, 1.0f, 0, -1, modify, 0);
}
/* J3DModelData (HD): joint nodes (0x1C each) at +8, count at +4; materials (0x39C each) at +0x10,
 * count at +0xC; the joint / material name tables through 027F3F8C / 027F68F4 (+0x18, offset based)
 */
static inline void setJointCallBack(J3DModelData* d, u32 i, J3DJointCallBack cb) {
    u32 data = gabi::ea(d);
    u32 n = d->mJointNum;
    u32 p = d->mJointNodes;
    if (i < n)
        p += i * 0x1C;
    pzGuest<J3DJoint>(p)->setCallBack(cb);
}
static inline u32 getMaterialNodePointer(u32 data, u32 i) {
    u32 n = pzGuest<J3DModelData>(data)->mMaterialNum;
    u32 p = pzGuest<J3DModelData>(data)->mMaterials;
    return i < n ? p + i * 0x39C : p;
}

static inline void assureTermination(SafeString* s) {
    pzAssureTermination(s);
}
/* inlined into daPz_c methods */
static inline bool SafeString_eq_lit(SafeString* a, const char* lit) {
    SafeString b;
    b.mStringTop = gabi::guest_str(lit);
    b.__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, &b);
}

/* HD (inline, three copies): the texture-pattern animation of the second btp drives every material
 * except the mouth and the two face shadows; links the btp's material entries to the model's */
/* inlined into daPz_c methods */
static void setBtpMaterialTable(daPz_c* i_this, J3DModelData* data, PzBtpResourceHD* btp,
                                const char* mouth, const char* shadowL, const char* shadowR) {
    u16 num = btp->mMaterialCount.get();
    for (u32 i = 0; i < num; i++) {
        PzBtpNameEntryHD* entry = btp->mNames.get() + i;
        SafeString name;
        name.mStringTop = gabi::ea(entry->mName.get());
        name.__vtbl = SAFESTRING_VTBL;
        u32 mtab = pzGetMaterialTable(data);
        assureTermination(&name);
        s32 idx = pzJUTNameTab_getIndex(gabi::ea(pzGuest<PzMaterialNamesHD>(mtab)->mNames.get()),
                                        name.mStringTop);
        bool other = !SafeString_eq_lit(&name, mouth) && !SafeString_eq_lit(&name, shadowL) &&
                     !SafeString_eq_lit(&name, shadowR);
        be<u32>* tbl = reinterpret_cast<PzBtpAnmHD*>(&i_this->mBtpAnm2)->mpMaterialLinks;
        u32 w = tbl[i].get();
        if (other) {
            tbl[i] = w & 0x3FFF8000;
            tbl = reinterpret_cast<PzBtpAnmHD*>(&i_this->mBtpAnm2)->mpMaterialLinks;
            s32 next = idx + 1;
            tbl[i] = tbl[i].get() | (next & 0x7FFF);
            tbl = reinterpret_cast<PzBtpAnmHD*>(&i_this->mBtpAnm2)->mpMaterialLinks;
            be<u32>* a = tbl + next;
            *a = a->get() & 0xC0007FFF;
            tbl = reinterpret_cast<PzBtpAnmHD*>(&i_this->mBtpAnm2)->mpMaterialLinks;
            a = tbl + next;
            *a = a->get() | ((i << 15) & 0x3FFF8000);
        } else {
            tbl[i] = w | 0xC0007FFF;
            tbl = reinterpret_cast<PzBtpAnmHD*>(&i_this->mBtpAnm2)->mpMaterialLinks;
            be<u32>* a = tbl + idx;
            *a = a->get() | 0x3FFF8000;
        }
    }
}

/* 024514FC */
BOOL daPz_c::bodyCreateHeap() {

    J3DModelData* modelData = (J3DModelData*)getPzRes(PZ_BDL_BODY);
    if (modelData == nullptr) /* JUT_ASSERT(0x24A, modelData != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x24A, "modelData != (0)");
    u32 data = gabi::ea(modelData);

    for (u32 i = 0; i < 2; i++) {
        SafeString* s = &l_eyeMaterialNames.mNames[i];
        u32 tbl = pzGuest<J3DModelData>(data)->mpResHeader.get();
        assureTermination(s);
        s32 idx = pzJUTNameTab_getIndex(gabi::ea(pzGuest<PzMaterialNamesHD>(tbl)->mNames.get()),
                                        s->mStringTop);
        u32 mat = idx < 0 ? 0 : getMaterialNodePointer(data, (u32)idx);
        daPz_matAnm_c* anm = (daPz_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = daPz_matAnm_c_ct(anm);                 /* daPz_matAnm_c::daPz_matAnm_c */
        pzGuest<PzMaterialHD>(mat)->mpMaterialAnm = anm; /* J3DMaterial::setMaterialAnm */
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr)
        return FALSE;
    ((J3DModel*)mpMorf->getModel())->mUserArea = gabi::ea(this);
    if (!pzInvisibleCreate(mInvisibleModel, mpMorf->getModel())) /* mDoExt_invisibleModel::create */
        return FALSE;
    m_jnt.mHeadJntNum = PZ_JNT_HEAD_e;
    setJointCallBack(modelData, PZ_JNT_HEAD_e, nodeHeadControl_CB);
    m_jnt.mBackboneJntNum = PZ_JNT_WAIST_e;
    setJointCallBack(modelData, PZ_JNT_WAIST_e, nodeWaistControl_CB);
    setJointCallBack(modelData, PZ_JNT_WAIST2_e, nodeWaist2Control_CB);
    setJointCallBack(modelData, PZ_JNT_SKIRT_e, nodeSkirtControl_CB);

    void* btp = getPzRes(PZ_BTP_EYE_NORMAL);
    if (btp == nullptr) /* JUT_ASSERT(0x28E, btp != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x28E, "btp != 0");
    if (!btpAnm_init(&mBtpAnm, modelData, btp, 0))
        return FALSE;
    btpAnm_init(&mBtpAnm2, modelData, btp, 0);
    setBtpMaterialTable(this, J3DModel_getModelData(mpMorf->getModel()),
                        reinterpret_cast<PzBtpAnmHD*>(&mBtpAnm2)->mpMaterialResource.get(),
                        "m_pz_mouth", "m_pz_shadowL", "m_pz_shadowR");

    void* btk = getPzRes(PZ_BTK_EYE_NORMAL);
    if (btk == nullptr) /* JUT_ASSERT(0x2B2, btk != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x2B2, "btk != 0");
    if (!btkAnm_init(&mBtkAnm, modelData, btk, 0))
        return FALSE;
    /* HD: the eye material animations, one per btk entry */
    PzBtkStateHD* btkAnm = reinterpret_cast<PzBtkAnmHD*>(&mBtkAnm)->mpState;
    u16 num = btkAnm->mpResource->mMaterialCount.get();
    for (u32 j = 0; j < num; j++) {
        SafeString* s = &l_eyeMaterialNames.mNames[j];
        u32 tbl = pzGuest<J3DModelData>(data)->mpResHeader.get();
        assureTermination(s);
        s32 idx = pzJUTNameTab_getIndex(gabi::ea(pzGuest<PzMaterialNamesHD>(tbl)->mNames.get()),
                                        s->mStringTop);
        u32 mat = idx < 0 ? 0 : getMaterialNodePointer(data, (u32)idx);
        mpMatAnm[j] = pzGuest<PzMaterialHD>(mat)->mpMaterialAnm.get();
    }

    void* brk = getPzRes(PZ_BRK_BODY);
    if (brk == nullptr) /* JUT_ASSERT(0x2CC, brk != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x2CC, "brk != 0");
    if (!brkAnm_init(&mBrkAnm, modelData, brk, 0))
        return FALSE;

    if (l_materialNamesInitialized == 0) {

        l_materialNamesInitialized = 1;
        for (u32 k = 0; k < 16; k++) {
            if (gabi::ea(&material_names.mNames[k]) ==
                0) /* SafeString "constructor" (never allocates) */
                operator_new(8);
            material_names.mNames[k].__vtbl = SAFESTRING_VTBL;
            material_names.mNames[k].mStringTop = gabi::guest_str(material_name_literals[k]);
        }
        __register_global_object(gabi::ea(&l_materialRegistration));
    }
    for (u32 k = 0; k < 16; k++) {
        SafeString* s = &material_names.mNames[k];

        u32 mtab = pzGetMaterialNameTable(modelData);
        assureTermination(s);
        mMatIdx[k < 16 ? k : 0] = pzJUTNameTab_getIndex(
            gabi::ea(pzGuest<PzMaterialNamesHD>(mtab)->mNames.get()), s->mStringTop);
    }
    return TRUE;
}

/* 0245323C */
BOOL daPz_c::demo() {

    if (demoActorID == 0) {
        if (mbDemoActive != 0)
            mbDemoActive = 0;
        return FALSE;
    }
    u8 id = demoActorID;
    mbDemoActive = 1;
    u32 demoActor;
    if (id == 0 || id > 0x20) {
        demoActor = 0;
    } else {
        /* dComIfGp_demo_getActor(id) */
        if (l_demoSingleton.mObject.get() == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
        demoActor =
            pzDemoGetActor(l_demoSingleton.mObject.get(), id); /* dDemo_object_c::getActor */
    }
    for (int i = 0; i < 2; i++)
        if (mpMatAnm[i].get() != nullptr)
            mpMatAnm[i]->mMoveFlag = 0;
    u32 btpRes = gabi::ea(mBtpAnm.mpAnm.get());
    if (btpRes != 0) {
        u32 frameMax =
            gabi::call_ptr<u32>(pzGuest<PzAnimationResourceHD>(btpRes)->mpVTable->mGetFrameMax,
                                btpRes); /* getFrameMax */
        u8 frame = (u8)(mEyeFrame + 1);
        mEyeFrame = frame;
        if (frame >= (u8)frameMax)
            mEyeFrame = (u8)frameMax;
    }
    if (demoActor == 0) {
        pzSetDemoData(this, 0x6A, mpMorf.get(), l_arcName, 0, 0, 0, 0); /* dDemo_setDemoData */
        return mbDemoActive;
    }
    void* btp = pzDemoGetBtp(demoActor, l_arcName); /* dDemo_actor_c::getP_BtpData */
    if (btp != nullptr) {
        J3DModelData* data = J3DModel_getModelData(mpMorf->getModel());
        btpAnm_init(&mBtpAnm, data, btp, 1);
        btpAnm_init(&mBtpAnm2, data, btp, 1);
        setBtpMaterialTable(this, data,
                            reinterpret_cast<PzBtpAnmHD*>(&mBtpAnm2)->mpMaterialResource.get(),
                            "m_pz_mouth", "m_pz_shadowL", "m_pz_shadowR");
        mEyeFrame = 0;
    }
    void* btk = pzDemoGetBtk(demoActor, l_arcName); /* dDemo_actor_c::getP_BtkData */
    if (btk != nullptr)
        btkAnm_init(&mBtkAnm, J3DModel_getModelData(mpMorf->getModel()), btk, 1);
    void* brk = pzDemoGetBrk(demoActor, l_arcName); /* dDemo_actor_c::getP_BrkData */
    if (brk != nullptr)
        brkAnm_init(&mBrkAnm, J3DModel_getModelData(mpMorf->getModel()), brk, 1);
    pzSetDemoData(this, 0x6A, mpMorf.get(), l_arcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemoActive;
}

/* 02454114 */
void daPz_c::setEyeBtp(int i_idx) {

    J3DModel* model = mpMorf->getModel();
    void* btp = getPzRes(i_idx);
    if (btp == nullptr) /* JUT_ASSERT(0x713, btp != NULL) */
        JUT_ASSERT_fail("d_a_pz.cpp", 0x713, "btp != (0)");
    mEyeFrame = 0;
    btpAnm_init(&mBtpAnm, J3DModel_getModelData(model), btp, 1);
    btpAnm_init(&mBtpAnm2, J3DModel_getModelData(model), btp, 1);
    setBtpMaterialTable(this, J3DModel_getModelData(mpMorf->getModel()),
                        reinterpret_cast<PzBtpAnmHD*>(&mBtpAnm2)->mpMaterialResource.get(),
                        "m_pz_mouth", "m_pz_shadowL", "m_pz_shadowR");
}

/* 02454B7C */
BOOL daPz_c::_execute() {

    if (l_HIO.mbDebugWarpToPlayer != 0) {
        /* HIO: warp in front of Link */
        modeProc(PROC_INIT_e, MODE_WAIT);
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        f32 x = link->current.pos.x;
        current.pos.x = x;
        f32 y = link->current.pos.y;
        current.pos.y = y;
        f32 z = link->current.pos.z;
        current.pos.z = z;
        current.pos.y = y + 50.0f;
        current.pos.x = gabi::fnmsubs(200.0f, cM_ssin((u16)link->shape_angle.y), x);
        current.pos.z = gabi::fnmsubs(200.0f, cM_scos((u16)link->shape_angle.y), z);
    }
    setFallSplash();
    getGndPos();
    mbEyesFollowGanondorf = 0;
    if (mbHasGanondorf)
        mbEyesFollowGanondorf = (u8)checkEyeArea(&mGanondorfPosCurrent);
    s32 mode = mMode;
    gravity = l_HIO.mGravity;
    if (mode != 6 && mode != 5 && m073F != 0)
        modeProc(PROC_INIT_e, MODE_AFRAID);
    setRipple();
    if (mMode != MODE_DOWN && m08B0 == 1)
        modeProc(PROC_INIT_e, MODE_DOWN);
    setJntStatus();
    if (demo()) {
        mDoExt_baseAnm_play(&mBtkAnm);
        mDoExt_baseAnm_play(&mBrkAnm);
        setMtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        return TRUE;
    }
    current.angle.z = shape_angle.z;
    current.angle.y = shape_angle.y;
    current.angle.x = shape_angle.x;
    mpMorf->calc();
    pzEnemyFire(&mEnemyFire);     /* enemy_fire */
    if (pzEnemyIce(&mEventIce)) { /* enemy_ice */
        J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
        return TRUE;
    }
    if (m06D3 == ANM_WAKE)
        setHeadSplash();
    if (mArg == 0 && (m08B0 == 0 || m08B0 == 2) && m0F80 == 0 && mMode != MODE_ATTACK_WAIT &&
        mMode != MODE_ATTACK && mMode != MODE_DOWN && mMode != MODE_TALK &&
        pzCLib_calcTimer(&m0F7C) == 0) {
        gabi::Local<be<u32>> gndId;
        if (pzFopAcM_SearchByName(fpcNm_GND_e, gndId) && *gndId != 0) {
            fopAc_ac_c* gnd = pzGuest<fopAc_ac_c>(*gndId);
            f64 dist = pzSearchActorDistanceXZ_d(this, dComIfGp_getPlayer(0));
            if (dist < l_HIO.mTalkDistance[m08B0] &&
                reinterpret_cast<PzGanondorfHD*>(gnd)->mDemoState == 0) {
                m0F82 = 1;
                modeProc(PROC_INIT_e, MODE_TALK);
            }
        }
    }
    checkOrder();
    modeProc(PROC_EXEC_e, MODE_KEEP);
    eventOrder();
    setAttention();
    cLib_addCalc2(&speedF, mTargetSpeed, 0.3f, 4.0f);
    if (mObjAcch.ChkGroundHit()) {
        u32 snd = dBgS_GetMtrlSndId(
            dComIfG_Bgsp(),
            pzGuest<cBgS_PolyInfo>(gabi::ea(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd))));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        pzMorfPlay(mpMorf.get(), &eyePos, snd, reverb); /* mDoExt_McaMorf::play */
    } else {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        pzMorfPlay(mpMorf.get(), &eyePos, 0, reverb);
    }
    pzMorfPlay(mpBowMcaMorf.get(), 0, 0, 0);
    mDoExt_baseAnm_play(&mBrkAnm);
    playEyeAnm();
    if (l_HIO.mDebugAnm != 0)
        setAnm((s8)l_HIO.mDebugAnm, 0, 0xF);
    fopAcM_posMoveF(this, nullptr);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    if (!dComIfGp_event_runCheck()) /* !dComIfGp_event_runCheck() */
        setCollision(30.0f, 130.0f);
    setAnmRunSpeed();
    current.angle.x = shape_angle.x;
    current.angle.y = shape_angle.y;
    current.angle.z = shape_angle.z;
    setMtx();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    return TRUE;
}

static inline void entryImm(u32 packet) {
    pzEntryImmediate(gabi::ea(l_pzRenderer.mpOpa.get()), packet, 0);
}
static inline void setDrawBuffers(PzDrawBuffer_e opa, PzDrawBuffer_e xlu) {
    l_pzRenderer.mpOpa = pzGuest<PzPlayDrawHD>(dComIfGp_ea())->mBuffers[opa].get();
    l_pzRenderer.mpXlu = pzGuest<PzPlayDrawHD>(dComIfGp_ea())->mBuffers[xlu].get();
}
static inline void setShapeVisible(u32 shape, u8 v) {
    pzGuest<J3DShape>(shape)->mVisible = v;
}
static inline u16 getMaterialNum(J3DModelData* d) {
    return pzGuest<PzMaterialNamesHD>(pzGetMaterialTable(d))->mMaterialCount.get();
}

// HD: (HD: the face is drawn in several passes through the four packets)
/* 02455074 */
void daPz_c::bodyDraw() {

    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData(model);
    u32 data = gabi::ea(modelData);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mpMorf->calc();
    if (mArg != 0)
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)&mBrkAnm, modelData, mBrkAnm.mFrameCtrl.mFrame.get());
    setDrawBuffers(PZ_DRAW_FACE, PZ_DRAW_FACE);
    entryImm(gabi::ea(&mPacket[3]));
    /* pass 1: only the four "damB" eye / brow materials */
    for (u16 i = 0; i < getMaterialNum(modelData); i++) {
        u32 mat = getMaterialNodePointer(data, i);
        bool damB = i == mMatIdx[0] || i == mMatIdx[3] || i == mMatIdx[6] || i == mMatIdx[9];
        setShapeVisible(gabi::ea(pzGuest<PzMaterialHD>(mat)->mpShape.get()), damB ? 1 : 0);
    }
    pzUpdateMaterialDisplayList(model,
                                (u32)mpJointNodes); /* HD: J3DModel material display list update */
    entryImm(gabi::ea(&mPacket[1]));
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)&mBtkAnm, modelData, mBtkAnm.mFrameCtrl.mFrame.get());
    setShapeVisible(mMatSh0[0], 0);
    setShapeVisible(mMatSh0[3], 0);
    setShapeVisible(mMatSh0[2], 1);
    setShapeVisible(mMatSh0[5], 1);
    setShapeVisible(mMatSh1[0], 0);
    setShapeVisible(mMatSh1[3], 0);
    setShapeVisible(mMatSh1[2], 1);
    setShapeVisible(mMatSh1[5], 1);
    pzUpdateMaterialDisplayList(model, (u32)mpJointNodes);
    setShapeVisible(mMatSh0[2], 0);
    setShapeVisible(mMatSh0[5], 0);
    setShapeVisible(mMatSh1[2], 0);
    setShapeVisible(mMatSh1[5], 0);
    for (int k = 0; k < 4; k++)
        setShapeVisible(mMatSh2[k], 1);
    pzUpdateMaterialDisplayList(model, (u32)mpJointNodes);
    setDrawBuffers(PZ_DRAW_SPECIAL, PZ_DRAW_SPECIAL);
    entryImm(gabi::ea(&mPacket[2]));
    for (int k = 0; k < 4; k++)
        setShapeVisible(mMatSh2[k], 0);
    setShapeVisible(mMatSh0[1], 1);
    setShapeVisible(mMatSh0[4], 1);
    setShapeVisible(mMatSh1[1], 1);
    setShapeVisible(mMatSh1[4], 1);
    pzUpdateMaterialDisplayList(model, (u32)mpJointNodes);
    entryImm(gabi::ea(&mPacket[0]));
    setShapeVisible(mMatSh0[1], 0);
    setShapeVisible(mMatSh0[4], 0);
    setShapeVisible(mMatSh1[1], 0);
    setShapeVisible(mMatSh1[4], 0);
    setDrawBuffers(PZ_DRAW_OPA, PZ_DRAW_XLU);
    /* pass 2: every material that is not one of the named face materials */
    for (u16 i = 0; i < getMaterialNum(modelData); i++) {
        bool named = false;
        for (int k = 0; k < 16; k++)
            if (i == mMatIdx[k]) {
                named = true;
                break;
            }
        if (!named)
            setShapeVisible(
                gabi::ea(pzGuest<PzMaterialHD>(getMaterialNodePointer(data, i))->mpShape.get()), 1);
    }
    mpMorf->entryDL();
    pzBtpEntry(&mBtpAnm, model, (f32)mEyeFrame); /* HD: mDoExt_btpAnm::entry(J3DModel*, f32) */
    pzBtpEntry(&mBtpAnm2, model, (f32)mEyeFrame);
    dComIfGd_setList();
    for (u16 i = 0; i < getMaterialNum(modelData); i++)
        setShapeVisible(
            gabi::ea(pzGuest<PzMaterialHD>(getMaterialNodePointer(data, i))->mpShape.get()), 1);
    reinterpret_cast<PzModelDataTailHD*>(modelData)->mBtpAnm = 0;
    reinterpret_cast<PzModelDataTailHD*>(modelData)->mBtkAnm = 0;
    if (mArg != 0)
        reinterpret_cast<PzModelDataTailHD*>(modelData)->mBrkAnm = 0;
}

// clang-format off
/* WWHD glue
data l_materialRegistration 101CF1F4
data l_pzRenderer 104B4634
data l_demoSingleton 101D5FFC
data material_names 1046D560
str 10038688 in 024514FC
str 10038698 in 024514FC
str 100386A8 in 024514FC
str 100386B4 in 024514FC
str 100386C4 in 024514FC
str 100386D4 in 024514FC
str 10038768 in 024514FC
str 10038778 in 024514FC
str 10038788 in 024514FC
str 10038794 in 024514FC
str 100387A4 in 024514FC
str 100387B4 in 024514FC
str 10038730 in 024514FC
str 10038740 in 024514FC
str 100386E0 in 024514FC
str 100386EC in 024514FC
str 10038724 in 024514FC
str 10038964 in 0245323C
str 10038970 in 0245323C
str 10038980 in 0245323C
str 10038A2C in 02454114
str 10038A38 in 02454114
str 10038A48 in 02454114
str 100386F8 in 024514FC
str 10038704 in 024514FC
str 10038718 in 024514FC
str 10038750 in 024514FC
str 1003875C in 024514FC
str 10038670 in 0245323C
str 10038660 in 0245323C
str 10038A14 in 02454114
str 10038A20 in 02454114
str 10038D00
*/
// clang-format on
