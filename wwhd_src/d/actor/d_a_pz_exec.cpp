/**
 * d_a_pz_exec.cpp (WWHD): d_a_pz heap creation, demo, execute and body drawing. Written from the WWHD code (the GameCube decompilation has
 * only stubs for d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static inline BOOL fopAcM_SearchByName(s16 name, be<u32>* out) { return gabi::call<BOOL>(0x025D5578, name, out); }
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline f64 searchActorDistanceXZ_d(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<f64>(0x025D6958, a, b); }
/* resources by index, the SafeString key built on the stack */
static inline void* getPzRes(s32 index) { return dComIfG_getObjectRes(l_arcName, index, SAFESTRING_VTBL); }
/* 025E789C mDoExt_btpAnm::init / 025E7CE0 mDoExt_btkAnm::init / 025E8154 mDoExt_brkAnm::init
 * (anm, modelData, res, anmPlay, mode, rate, start, end, modify, entry) */
static inline BOOL btpAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return gabi::call<BOOL>(0x025E789C, anm, d, res, 1, 0, 1.0f, 0, -1, modify, 0);
}
static inline BOOL btkAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return gabi::call<BOOL>(0x025E7CE0, anm, d, res, 1, 0, 1.0f, 0, -1, modify, 0);
}
static inline BOOL brkAnm_init(void* anm, J3DModelData* d, void* res, s32 modify) {
    return gabi::call<BOOL>(0x025E8154, anm, d, res, 1, 2, 1.0f, 0, -1, modify, 0);
}
/* J3DModelData (HD): joint nodes (0x1C each) at +8, count at +4; materials (0x39C each) at +0x10,
 * count at +0xC; the joint / material name tables through 027F3F8C / 027F68F4 (+0x18, offset based) */
static inline void setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline u32 getMaterialNodePointer(u32 data, u32 i) {
    u32 n = gabi::load<u32>(data + 0xC);
    u32 p = gabi::load<u32>(data + 0x10);
    return i < n ? p + i * 0x39C : p;
}
static inline u32 offsetPtr(u32 field) {
    s32 off = gabi::load<s32>(field);
    return off != 0 ? field + off : 0;
}
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
static inline void assureTermination(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool SafeString_eq_lit(SafeString* a, u32 lit) {
    gabi::Local<SafeString> b;
    b->mStringTop = lit;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}

/* HD (inline, three copies): the texture-pattern animation of the second btp drives every material
 * except the mouth and the two face shadows; links the btp's material entries to the model's */
static void setBtpMaterialTable(daPz_c* i_this, J3DModelData* data, u32 btp, u32 mouth, u32 shadowL, u32 shadowR) {
    u16 num = gabi::load<u16>(btp + 0x16);
    u32 nameField = btp + 0x2C;
    for (u32 i = 0; i < num; i++) {
        u32 entry = offsetPtr(nameField) + i * 0x1C;
        gabi::Local<SafeString> name;
        name->mStringTop = offsetPtr(entry + 0xC);
        name->__vtbl = SAFESTRING_VTBL;
        u32 mtab = gabi::call<u32>(0x027F3F8C, data);
        assureTermination(name);
        s32 idx = JUTNameTab_getIndex(offsetPtr(mtab + 0x18), name->mStringTop);
        bool other = !SafeString_eq_lit(name, mouth) && !SafeString_eq_lit(name, shadowL) && !SafeString_eq_lit(name, shadowR);
        u32 tbl = gabi::load<u32>(gabi::ea(i_this) + 0x1374);
        u32 w = gabi::load<u32>(tbl + 4 * i);
        if (other) {
            gabi::store<u32>(tbl + 4 * i, w & 0x3FFF8000);
            tbl = gabi::load<u32>(gabi::ea(i_this) + 0x1374);
            s32 next = idx + 1;
            gabi::store<u32>(tbl + 4 * i, gabi::load<u32>(tbl + 4 * i) | (next & 0x7FFF));
            tbl = gabi::load<u32>(gabi::ea(i_this) + 0x1374);
            u32 a = tbl + next * 4;
            gabi::store<u32>(a, gabi::load<u32>(a) & 0xC0007FFF);
            tbl = gabi::load<u32>(gabi::ea(i_this) + 0x1374);
            a = tbl + next * 4;
            gabi::store<u32>(a, gabi::load<u32>(a) | ((i << 15) & 0x3FFF8000));
        } else {
            gabi::store<u32>(tbl + 4 * i, w | 0xC0007FFF);
            tbl = gabi::load<u32>(gabi::ea(i_this) + 0x1374);
            u32 a = idx * 4 + tbl;
            gabi::store<u32>(a, gabi::load<u32>(a) | 0x3FFF8000);
        }
    }
}

/* 024514FC */
BOOL daPz_c::bodyCreateHeap() {
    WWHD_FUNC(0x024514FC, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)getPzRes(0x18);
    if (modelData == nullptr) /* JUT_ASSERT(0x24A, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x100386F8), 0x24A, STR(0x10038704));
    u32 data = gabi::ea(modelData);
    /* HD: daPz_matAnm_c on the two eye materials (static SafeStrings at 0x1046D420) */
    for (u32 i = 0; i < 2; i++) {
        SafeString* s = gabi::at<SafeString>(0x1046D420 + 8 * i);
        u32 tbl = gabi::load<u32>(data + 0);
        assureTermination(s);
        s32 idx = JUTNameTab_getIndex(offsetPtr(tbl + 0x18), s->mStringTop);
        u32 mat = idx < 0 ? 0 : getMaterialNodePointer(data, (u32)idx);
        daPz_matAnm_c* anm = (daPz_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = gabi::call<daPz_matAnm_c*>(0x024514A4, anm); /* daPz_matAnm_c::daPz_matAnm_c */
        gabi::store<u32>(mat + 0x24, gabi::ea(anm)); /* J3DMaterial::setMaterialAnm */
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr)
        return FALSE;
    ((J3DModel_l*)mpMorf->getModel())->mUserArea = gabi::ea(this);
    if (!gabi::call<BOOL>(0x025E8A48, mInvisibleModel, mpMorf->getModel())) /* mDoExt_invisibleModel::create */
        return FALSE;
    m_jnt.mHeadJntNum = 4;
    setJointCallBack(modelData, 4, 0x02450F68 /* nodeHeadControl_CB */);
    m_jnt.mBackboneJntNum = 1;
    setJointCallBack(modelData, 1, 0x02451178 /* nodeWaistControl_CB */);
    setJointCallBack(modelData, 0x15, 0x024512DC /* nodeWaist2Control_CB */);
    setJointCallBack(modelData, 0x19, 0x0245145C /* nodeSkirtControl_CB */);

    void* btp = getPzRes(0x27);
    if (btp == nullptr) /* JUT_ASSERT(0x28E, btp != NULL) */
        JUT_ASSERT_fail(STR(0x100386F8), 0x28E, STR(0x10038718));
    if (!btpAnm_init(mBtpAnm, modelData, btp, 0))
        return FALSE;
    btpAnm_init(mBtpAnm2, modelData, btp, 0);
    setBtpMaterialTable(this, J3DModel_getModelData(mpMorf->getModel()), gabi::load<u32>(gabi::ea(mBtpAnm2) + 0x60), 0x10038724,
                        0x10038730, 0x10038740);

    void* btk = getPzRes(0x1F);
    if (btk == nullptr) /* JUT_ASSERT(0x2B2, btk != NULL) */
        JUT_ASSERT_fail(STR(0x100386F8), 0x2B2, STR(0x10038750));
    if (!btkAnm_init(mBtkAnm, modelData, btk, 0))
        return FALSE;
    /* HD: the eye material animations, one per btk entry */
    u32 btkAnm = gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68);
    u16 num = gabi::load<u16>(gabi::load<u32>(btkAnm + 0xC) + 0x14);
    for (u32 j = 0; j < num; j++) {
        SafeString* s = gabi::at<SafeString>(0x1046D420 + 8 * j);
        u32 tbl = gabi::load<u32>(data + 0);
        assureTermination(s);
        s32 idx = JUTNameTab_getIndex(offsetPtr(tbl + 0x18), s->mStringTop);
        u32 mat = idx < 0 ? 0 : getMaterialNodePointer(data, (u32)idx);
        gabi::store<u32>(gabi::ea(&mpMatAnm[0]) + 4 * j, gabi::load<u32>(mat + 0x24));
    }

    void* brk = getPzRes(0x1B);
    if (brk == nullptr) /* JUT_ASSERT(0x2CC, brk != NULL) */
        JUT_ASSERT_fail(STR(0x100386F8), 0x2CC, STR(0x1003875C));
    if (!brkAnm_init(mBrkAnm, modelData, brk, 0))
        return FALSE;

    /* HD: static sead::SafeString mat_name[16] (guard 0x1046D5EC; destroyed by 024521BC) */
    const u32 names = 0x1046D560;
    if (gabi::load<u32>(0x1046D5EC) == 0) {
        static const u32 lit[16] = {0x10038688, 0x10038698, 0x100386A8, 0x100386B4, 0x100386C4, 0x100386D4, 0x10038768, 0x10038778,
                                    0x10038788, 0x10038794, 0x100387A4, 0x100387B4, 0x10038730, 0x10038740, 0x100386E0, 0x100386EC};
        gabi::store<u32>(0x1046D5EC, 1);
        for (u32 k = 0; k < 16; k++) {
            if (names + 8 * k == 0) /* SafeString "constructor" (never allocates) */
                operator_new(8);
            gabi::store<u32>(names + 8 * k + 4, SAFESTRING_VTBL);
            gabi::store<u32>(names + 8 * k, lit[k]);
        }
        __register_global_object(0x101CF1F4);
    }
    for (u32 k = 0; k < 16; k++) {
        SafeString* s = gabi::at<SafeString>(names + 8 * k);
        u32 dst = gabi::ea(&mMatIdx[k < 16 ? k : 0]);
        u32 mtab = gabi::call<u32>(0x027F68F4, modelData);
        assureTermination(s);
        gabi::store<s32>(dst, JUTNameTab_getIndex(offsetPtr(mtab + 0x18), s->mStringTop));
    }
    return TRUE;
}
VERIFY(0x024514FC, &daPz_c::bodyCreateHeap);

/* 0245323C */
BOOL daPz_c::demo() {
    WWHD_FUNC(0x0245323C, BOOL, this);
    if (demoActorID == 0) {
        if (m1196 != 0)
            m1196 = 0;
        return FALSE;
    }
    u8 id = demoActorID;
    m1196 = 1;
    u32 demoActor;
    if (id == 0 || id > 0x20) {
        demoActor = 0;
    } else {
        /* dComIfGp_demo_getActor(id) */
        if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
            JUT_ASSERT_fail(STR(0x10038670), 0x23A, STR(0x10038660));
        demoActor = gabi::call<u32>(0x02526E70, gabi::load<u32>(0x101D5FFC), id); /* dDemo_object_c::getActor */
    }
    for (int i = 0; i < 2; i++)
        if (mpMatAnm[i].get() != nullptr)
            mpMatAnm[i]->mMoveFlag = 0;
    u32 btpRes = gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10);
    if (btpRes != 0) {
        u32 frameMax = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(btpRes + 4) + 0x14), btpRes); /* getFrameMax */
        u8 frame = (u8)(m1197 + 1);
        m1197 = frame;
        if (frame >= (u8)frameMax)
            m1197 = (u8)frameMax;
    }
    if (demoActor == 0) {
        gabi::call(0x02527028, this, 0x6A, mpMorf.get(), l_arcName, 0, 0, 0, 0); /* dDemo_setDemoData */
        return m1196;
    }
    void* btp = gabi::call<void*>(0x02527828, demoActor, l_arcName); /* dDemo_actor_c::getP_BtpData */
    if (btp != nullptr) {
        J3DModelData* data = J3DModel_getModelData(mpMorf->getModel());
        btpAnm_init(mBtpAnm, data, btp, 1);
        btpAnm_init(mBtpAnm2, data, btp, 1);
        setBtpMaterialTable(this, data, gabi::load<u32>(gabi::ea(mBtpAnm2) + 0x60), 0x10038964, 0x10038970, 0x10038980);
        m1197 = 0;
    }
    void* btk = gabi::call<void*>(0x025279C8, demoActor, l_arcName); /* dDemo_actor_c::getP_BtkData */
    if (btk != nullptr)
        btkAnm_init(mBtkAnm, J3DModel_getModelData(mpMorf->getModel()), btk, 1);
    void* brk = gabi::call<void*>(0x02527A98, demoActor, l_arcName); /* dDemo_actor_c::getP_BrkData */
    if (brk != nullptr)
        brkAnm_init(mBrkAnm, J3DModel_getModelData(mpMorf->getModel()), brk, 1);
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), l_arcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return m1196;
}
VERIFY(0x0245323C, &daPz_c::demo);

/* 02454114 (unnamed by the matcher: daPz_c::setEyeBtp; HD: two btp animations) */
void daPz_c::setEyeBtp(int i_idx) {
    WWHD_FUNC(0x02454114, void, this, i_idx);
    J3DModel* model = mpMorf->getModel();
    void* btp = getPzRes(i_idx);
    if (btp == nullptr) /* JUT_ASSERT(0x713, btp != NULL) */
        JUT_ASSERT_fail(STR(0x10038A14), 0x713, STR(0x10038A20));
    m1197 = 0;
    btpAnm_init(mBtpAnm, J3DModel_getModelData(model), btp, 1);
    btpAnm_init(mBtpAnm2, J3DModel_getModelData(model), btp, 1);
    setBtpMaterialTable(this, J3DModel_getModelData(mpMorf->getModel()), gabi::load<u32>(gabi::ea(mBtpAnm2) + 0x60), 0x10038A2C,
                        0x10038A38, 0x10038A48);
}
VERIFY(0x02454114, &daPz_c::setEyeBtp);

/* 02454B7C */
BOOL daPz_c::_execute() {
    WWHD_FUNC(0x02454B7C, BOOL, this);
    if (hio_u8(0x33) != 0) {
        /* HIO: warp in front of Link */
        modeProc(PROC_INIT_e, 0);
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
    m0856 = 0;
    if (mbHasGanondorf)
        m0856 = (u8)checkEyeArea(&mGanondorfPosCurrent);
    s32 mode = mMode;
    gravity = hio_f32(0xB0);
    if (mode != 6 && mode != 5 && m0857 != 0)
        modeProc(PROC_INIT_e, 6);
    setRipple();
    if (mMode != 5 && m0ADC == 1)
        modeProc(PROC_INIT_e, 5);
    setJntStatus();
    if (demo()) {
        mDoExt_baseAnm_play(mBtkAnm);
        mDoExt_baseAnm_play(mBrkAnm);
        setMtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        return TRUE;
    }
    current.angle.z = shape_angle.z;
    current.angle.y = shape_angle.y;
    current.angle.x = shape_angle.x;
    mpMorf->calc();
    gabi::call(0x02041570, &mEnemyFire); /* enemy_fire */
    if (gabi::call<BOOL>(0x020402C8, &mEnemyIce)) { /* enemy_ice */
        J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
        return TRUE;
    }
    if (mAnm == 9)
        setHeadSplash();
    if (mArg == 0 && (m0ADC == 0 || m0ADC == 2) && m11B0 == 0 && mMode != 2 && mMode != 3 && mMode != 5 && mMode != 9 &&
        cLib_calcTimer(&m11AC) == 0) {
        gabi::Local<be<u32>> gndId;
        if (fopAcM_SearchByName(0xF6 /* PROC_GND */, gndId) && *gndId != 0) {
            fopAc_ac_c* gnd = gabi::at<fopAc_ac_c>(*gndId);
            f64 dist = searchActorDistanceXZ_d(this, dComIfGp_getPlayer(0));
            if (dist < hio_f32(0x100 + 4 * m0ADC) && gabi::load<s16>(gabi::ea(gnd) + 0x3EA) == 0) {
                mOrderState = 1;
                modeProc(PROC_INIT_e, 9);
            }
        }
    }
    checkOrder();
    modeProc(PROC_EXEC_e, 0xB);
    eventOrder();
    setAttention();
    cLib_addCalc2(&speedF, m0B50, 0.3f, 4.0f);
    if (mObjAcch.ChkGroundHit()) {
        u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(gabi::ea(&mObjAcch) + 0xD4 + 0x14));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        gabi::call(0x025E535C, mpMorf.get(), &eyePos, snd, reverb); /* mDoExt_McaMorf::play */
    } else {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        gabi::call(0x025E535C, mpMorf.get(), &eyePos, 0, reverb);
    }
    gabi::call(0x025E535C, mpBowMcaMorf.get(), 0, 0, 0);
    mDoExt_baseAnm_play(mBrkAnm);
    playEyeAnm();
    if (hio_u8(0x2F) != 0)
        setAnm((s8)hio_u8(0x2F), 0, 0xF);
    fopAcM_posMoveF(this, nullptr);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0) /* !dComIfGp_event_runCheck() */
        setCollision(30.0f, 130.0f);
    setAnmRunSpeed();
    current.angle.x = shape_angle.x;
    current.angle.y = shape_angle.y;
    current.angle.z = shape_angle.z;
    setMtx();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    return TRUE;
}
VERIFY(0x02454B7C, &daPz_c::_execute);

static inline void entryImm(u32 packet) { gabi::call(0x027F0E04, gabi::load<u32>(0x104B4634), packet, 0); }
static inline void setDrawBuffers(u32 opaOff, u32 xluOff) {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + opaOff));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + xluOff));
}
static inline void setShapeVisible(u32 shape, u8 v) { gabi::store<u8>(shape + 4, v); }
static inline u16 getMaterialNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F8C, d) + 0x24); }

/* 02455074 (HD: the face is drawn in several passes through the four packets) */
void daPz_c::bodyDraw() {
    WWHD_FUNC(0x02455074, void, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData(model);
    u32 data = gabi::ea(modelData);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mpMorf->calc();
    if (mArg != 0)
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrkAnm, modelData, gabi::load<f32>(gabi::ea(mBrkAnm) + 4));
    setDrawBuffers(0x5D64, 0x5D64);
    entryImm(gabi::ea(&mPacket[3]));
    /* pass 1: only the four "damB" eye / brow materials */
    for (u16 i = 0; i < getMaterialNum(modelData); i++) {
        u32 mat = getMaterialNodePointer(data, i);
        bool damB = i == mMatIdx[0] || i == mMatIdx[3] || i == mMatIdx[6] || i == mMatIdx[9];
        setShapeVisible(gabi::load<u32>(mat + 8), damB ? 1 : 0);
    }
    gabi::call(0x027F583C, model, (u32)m1394); /* HD: J3DModel material display list update */
    entryImm(gabi::ea(&mPacket[1]));
    mDoExt_btkAnm_entry((mDoExt_btkAnm*)mBtkAnm, modelData, gabi::load<f32>(gabi::ea(mBtkAnm) + 4));
    setShapeVisible(mMatSh0[0], 0);
    setShapeVisible(mMatSh0[3], 0);
    setShapeVisible(mMatSh0[2], 1);
    setShapeVisible(mMatSh0[5], 1);
    setShapeVisible(mMatSh1[0], 0);
    setShapeVisible(mMatSh1[3], 0);
    setShapeVisible(mMatSh1[2], 1);
    setShapeVisible(mMatSh1[5], 1);
    gabi::call(0x027F583C, model, (u32)m1394);
    setShapeVisible(mMatSh0[2], 0);
    setShapeVisible(mMatSh0[5], 0);
    setShapeVisible(mMatSh1[2], 0);
    setShapeVisible(mMatSh1[5], 0);
    for (int k = 0; k < 4; k++) setShapeVisible(mMatSh2[k], 1);
    gabi::call(0x027F583C, model, (u32)m1394);
    setDrawBuffers(0x5D6C, 0x5D6C);
    entryImm(gabi::ea(&mPacket[2]));
    for (int k = 0; k < 4; k++) setShapeVisible(mMatSh2[k], 0);
    setShapeVisible(mMatSh0[1], 1);
    setShapeVisible(mMatSh0[4], 1);
    setShapeVisible(mMatSh1[1], 1);
    setShapeVisible(mMatSh1[4], 1);
    gabi::call(0x027F583C, model, (u32)m1394);
    entryImm(gabi::ea(&mPacket[0]));
    setShapeVisible(mMatSh0[1], 0);
    setShapeVisible(mMatSh0[4], 0);
    setShapeVisible(mMatSh1[1], 0);
    setShapeVisible(mMatSh1[4], 0);
    setDrawBuffers(0x5D68, 0x5D60);
    /* pass 2: every material that is not one of the named face materials */
    for (u16 i = 0; i < getMaterialNum(modelData); i++) {
        bool named = false;
        for (int k = 0; k < 16; k++)
            if (i == mMatIdx[k]) {
                named = true;
                break;
            }
        if (!named)
            setShapeVisible(gabi::load<u32>(getMaterialNodePointer(data, i) + 8), 1);
    }
    mpMorf->entryDL();
    gabi::call(0x025E7BE4, mBtpAnm, model, (f32)m1197);  /* HD: mDoExt_btpAnm::entry(J3DModel*, f32) */
    gabi::call(0x025E7BE4, mBtpAnm2, model, (f32)m1197);
    dComIfGd_setList();
    for (u16 i = 0; i < getMaterialNum(modelData); i++)
        setShapeVisible(gabi::load<u32>(getMaterialNodePointer(data, i) + 8), 1);
    gabi::store<u32>(data + 0x44, 0);
    gabi::store<u32>(data + 0x38, 0);
    if (mArg != 0)
        gabi::store<u32>(data + 0x48, 0);
}
VERIFY(0x02455074, &daPz_c::bodyDraw);
