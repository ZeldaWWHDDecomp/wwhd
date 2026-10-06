/**
 * d_a_npc_kf1.cpp (WWHD)
 * NPC - Mila's father (rich): the Windfall pot shop owner ("angry" when pots are broken)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_kf1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_kf1.h"

enum : u32 { PMF_wait_action1 = 0x1001B878 };

/* 02048038 daObj::PrmAbstract(actor, width, shift): (mParameters >> shift) & ((1 << width) - 1) */
static inline u32 daObj_PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) { return gabi::call<u32>(0x02048038, a, width, shift); }
/* 0x101FF558: a global frame counter (g_Counter) */
static inline s32 g_Counter_mCounter0() { return gabi::load<s32>(0x101FF558); }
/* searchActor_Tsubo's results (file statics) */
static inline be<s32>& l_tsubo_num() { return *gabi::at<be<s32>>(0x104674C0); }
static inline be<u32>* l_tsubo() { return gabi::at<be<u32>>(0x10467528); } /* [20] */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* play + 0x52A6: dComIfGp flag word (bit 0x80: show the rupee counter) */
static inline void dComIfGp_onRupeeCntFlag() { u32 p = dComIfGp_ea() + 0x52A6; gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | 0x80)); }
static inline void dComIfGp_offRupeeCntFlag() { u32 p = dComIfGp_ea() + 0x52A6; gabi::store<u16>(p, (u16)(gabi::load<u16>(p) & 0xFF7F)); }

/* 02260D64 */
void daNpc_Kf1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02260D64, void, this, i_node, i_model);
    /* static cXyz a_eye_pos_off(30.0f, 30.0f, 0.0f): guard 0x10467578, object 0x1046751C */
    cXyz* a_eye_pos_off = gabi::at<cXyz>(0x1046751C);
    if (gabi::load<u32>(0x10467578) == 0) {
        a_eye_pos_off->x = 30.0f;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->y = 30.0f;
        gabi::store<u32>(0x10467578, 1);
    }
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, a_eye_pos_off, &mEyePos);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x02260D64, &daNpc_Kf1_c::_nodeCB_Head);

/* node callbacks: if (calcTiming == In && j3dSys.getModel()->getUserArea()) actor->_nodeCB_X(node, model) */
static inline void nodeCB_dispatch(J3DNode* node, int timing, u32 fn) {
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 user = gabi::load<u32>(model + 0xB8);
        if (user != 0)
            gabi::call(fn, user, node, model);
    }
}

/* 02260EB4 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02260EB4, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x02260D64);
    return TRUE;
}
VERIFY(0x02260EB4, nodeCB_Head);

/* 02260EFC */
void daNpc_Kf1_c::_nodeCB_Neck(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02260EFC, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x02260EFC, &daNpc_Kf1_c::_nodeCB_Neck);

/* 02261020 */
static BOOL nodeCB_Neck(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x02261020, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x02260EFC);
    return TRUE;
}
VERIFY(0x02261020, nodeCB_Neck);

/* 02261068 */
void daNpc_Kf1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {
    WWHD_FUNC(0x02261068, void, this, i_node, i_model);
    u32 jnt_no = jntNo_of(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(getAnmMtx(i_model, jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, (s16)-m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, j3dSys_mCurrentMtx());
    mtx_copy(getAnmMtx(i_model, jnt_no), stk);
}
VERIFY(0x02261068, &daNpc_Kf1_c::_nodeCB_BackBone);

/* 0226118C */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x0226118C, BOOL, i_node, i_calcTiming);
    nodeCB_dispatch(i_node, i_calcTiming, 0x02261068);
    return TRUE;
}
VERIFY(0x0226118C, nodeCB_BackBone);

/* 022611D4 */
int daNpc_Kf1_c::btpResID(int i_btpNum) {
    WWHD_FUNC(0x022611D4, int, this, i_btpNum);
    return gabi::load<s32>(0x1001B918 + i_btpNum * 4);
}
VERIFY(0x022611D4, &daNpc_Kf1_c::btpResID);

/* 022611E8 */
u32 daNpc_Kf1_c::setBtp(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x022611E8, u32, this, i_btpNum, i_bModify);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(525, a_btp != NULL) */
        JUT_ASSERT_fail(STR(0x1001B924), 0x20D, STR(0x1001B934));
    mBtpNum = i_btpNum;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(morf_model_p), a_btp, TRUE, 0, 1.0f, 0, -1, i_bModify, 0) != 0;
}
VERIFY(0x022611E8, &daNpc_Kf1_c::setBtp);

/* 022612D4 */
u32 daNpc_Kf1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {
    WWHD_FUNC(0x022612D4, u32, this, i_btpNum, i_bModify);
    return setBtp(i_btpNum, i_bModify);
}
VERIFY(0x022612D4, &daNpc_Kf1_c::init_texPttrnAnm);

/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline: joints are 0x1C-byte records) */
static inline void setJointCallBack(J3DModel* model, s8 jnt, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(model);
    u32 idx = (u16)(s16)jnt;
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (idx < n)
        joint += idx * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 022612D8 */
BOOL daNpc_Kf1_c::bodyCreateHeap() {
    WWHD_FUNC(0x022612D8, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 1);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2448, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001B954), 0x990, STR(0x1001B964));
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            u32 vt = gabi::load<u32>(gabi::ea(morf));
            gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3); /* delete (virtual deleting destructor) */
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001B944) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001B954), 0x9A6, STR(0x1001B978));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001B98C) /* "backbone" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001B954), 0x9A8, STR(0x1001B998));
    m_nck_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x1001B94C) /* "neck" */);
    if (m_nck_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x1001B954), 0x9AA, STR(0x1001B9B0));
    setJointCallBack(mpMorf->getModel(), m_hed_jnt_num, 0x02260EB4 /* nodeCB_Head */);
    setJointCallBack(mpMorf->getModel(), m_bbone_jnt_num, 0x0226118C /* nodeCB_BackBone */);
    setJointCallBack(mpMorf->getModel(), m_nck_jnt_num, 0x02261020 /* nodeCB_Neck */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x022612D8, &daNpc_Kf1_c::bodyCreateHeap);

/* 02261624 */
BOOL daNpc_Kf1_c::itemCreateHeap() {
    WWHD_FUNC(0x02261624, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 0);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2503, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x1001B9C4), 0x9C7, STR(0x1001B9D4));
    mpItemModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpItemModel.get() != nullptr;
}
VERIFY(0x02261624, &daNpc_Kf1_c::itemCreateHeap);

/* 022616BC */
BOOL daNpc_Kf1_c::CreateHeap() {
    WWHD_FUNC(0x022616BC, BOOL, this);
    if (!bodyCreateHeap()) {
        return FALSE;
    }
    if (!itemCreateHeap()) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 90.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x022616BC, &daNpc_Kf1_c::CreateHeap);

/* 02261778 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02261778, BOOL, i_this);
    return static_cast<daNpc_Kf1_c*>(i_this)->CreateHeap();
}
VERIFY(0x02261778, CheckCreateHeap);

/* 0226177C: collects the shop's pots (PROC 0x1C5 with parameter bits 24..27 == 14) */
static void* searchActor_Tsubo(void* i_actor, void*) {
    WWHD_FUNC(0x0226177C, void*, i_actor, (void*)nullptr);
    fopAc_ac_c* actor = (fopAc_ac_c*)i_actor;
    if (l_tsubo_num() < 20 && fopAc_IsActor(actor) && actor != nullptr && fpcM_GetName(actor) == 0x1C5 &&
        daObj_PrmAbstract(actor, 4, 0x18) == 0xE) {
        s32 n = l_tsubo_num();
        l_tsubo_num() = n + 1;
        l_tsubo()[n] = gabi::ea(actor);
    }
    return nullptr;
}
VERIFY(0x0226177C, searchActor_Tsubo);

/* 02261814 */
u8 daNpc_Kf1_c::decideType(int i_type) {
    WWHD_FUNC(0x02261814, u8, this, i_type); /* the type parameter is not used */
    if (mHeapType > 0) {
        return true;
    }
    mHeapType = 1;
    mType = 0;
    /* strcpy(mArcName, "Kf") (3 bytes) */
    u8 c0 = gabi::load<u8>(0x1001B9F8), c1 = gabi::load<u8>(0x1001B9F9), c2 = gabi::load<u8>(0x1001B9FA);
    gabi::store<u8>(gabi::ea(mArcName) + 0, c0);
    gabi::store<u8>(gabi::ea(mArcName) + 1, c1);
    gabi::store<u8>(gabi::ea(mArcName) + 2, c2);
    return mHeapType != -1 && mType != -1;
}
VERIFY(0x02261814, &daNpc_Kf1_c::decideType);

/* 02261878 */
void daNpc_Kf1_c::set_pthPoint(u32 i_idx) { /* u8, passed on unnormalised */
    WWHD_FUNC(0x02261878, void, this, i_idx);
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    gabi::Local<cXyz> pnt;
    gabi::call(0x0259E778, &mPathRun, pnt.get(), i_idx); /* getPoint */
    current.pos.copy(*pnt);
    if (dNpc_PathRun_nextIdx(&mPathRun)) {
        gabi::Local<cXyz> next;
        dNpc_PathRun_getPoint(&mPathRun, next, mPathRun.mIdx);
        gabi::Local<cXyz> target;
        target->copy(*next);
        current.angle.y = cLib_targetAngleY(&current.pos, target);
    }
}
VERIFY(0x02261878, &daNpc_Kf1_c::set_pthPoint);

/* 02261920 */
BOOL daNpc_Kf1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x02261920, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mCurrProcFunc;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
        if (cur->i == 0)
            goto set;
    }
    mActState = 9;
    pmf_call(this, cur, i_argsP);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x02261920, &daNpc_Kf1_c::set_action);

/* 02261A4C */
bool daNpc_Kf1_c::init_KF1_0() {
    WWHD_FUNC(0x02261A4C, bool, this);
    if (dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    return true;
}
VERIFY(0x02261A4C, &daNpc_Kf1_c::init_KF1_0);

/* 02261AD0 */
void daNpc_Kf1_c::play_btp_anm() {
    WWHD_FUNC(0x02261AD0, void, this);
    u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)));
    if (mBtpNum == 0 && cLib_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frame_max) {
        return;
    }
    if (mBtpNum == 0) {
        mBlinkTimer = (s16)cLib_getRndValue(60, 90);
        frame_max = 0;
    }
    mBtpFrame = frame_max;
}
VERIFY(0x02261AD0, &daNpc_Kf1_c::play_btp_anm);

/* 02261B70 */
void daNpc_Kf1_c::play_animation() {
    WWHD_FUNC(0x02261B70, void, this);
    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}
VERIFY(0x02261B70, &daNpc_Kf1_c::play_animation);

/* 02261C14 */
void daNpc_Kf1_c::setAttention(u32 i_setEyePos) {
    WWHD_FUNC(0x02261C14, void, this, i_setEyePos);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 y = current.pos.y + l_HIO().mPrm.mAttPosOffsetY;
    attPos->x = current.pos.x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}
VERIFY(0x02261C14, &daNpc_Kf1_c::setAttention);

/* 02261C68 */
void daNpc_Kf1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x02261C68, void, this, i_setEyePos);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    if (mpItemModel.get()) {
        /* the item (a pot) sits on the head */
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), m_hed_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_transM(33.87f, 3.26f, 0.0f);
        mDoMtx_XYZrotM(mDoMtx_stack_c::get(), -0x4000, -0x4000, 0);
        J3DModel_setBaseTRMtx(mpItemModel, mDoMtx_stack_c::get());
        J3DModel_calc(mpItemModel);
    }
    setAttention(i_setEyePos);
}
VERIFY(0x02261C68, &daNpc_Kf1_c::setMtx);

/* 02261E4C */
bool daNpc_Kf1_c::createInit() {
    WWHD_FUNC(0x02261E4C, bool, this);
    /* l_evn_tbl (.data 0x101BEE14): "angry", "rupee_age", "bensyou" */
    for (int i = 0; i < 3; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101BEE14 + i * 4));
        mEventIDTbl[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2(STR(0x1001BA08) /* "Kf1" */, (fopNpc_npc_c*)this);
    u32 prm = mParameters;
    mSwitchNo = (u8)(prm >> 8);
    u8 path_no = (u8)(prm >> 16);
    u8 weight = 0xFF;
    if (path_no != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, path_no, fopAcM_GetRoomNo(this), 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[SPEAK] */
    mBckNum = 0xA;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gravity = -4.5f;
    if (mType != 0) {
        return false;
    }
    if (!init_KF1_0()) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x02261E4C, &daNpc_Kf1_c::createInit);

/* 02262030 */
cPhs_State daNpc_Kf1_c::_create() {
    WWHD_FUNC(0x02262030, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Kf1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this);    /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x1001BC18;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoaded = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    /* a_heap_size_tbl (.data 0x101BEE20) */
    if (!fopAcM_entrySolidHeap(this, 0x02261778 /* CheckCreateHeap */, gabi::load<u32>(0x101BEE20 + mHeapType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -90.0f, -20.0f, -80.0f, 90.0f, 200.0f, 80.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x02262030, &daNpc_Kf1_c::_create);

/* 02262180 */
static cPhs_State daNpc_Kf1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02262180, cPhs_State, i_this);
    return ((daNpc_Kf1_c*)i_this)->_create();
}
VERIFY(0x02262180, daNpc_Kf1_Create);

/* 02262184 */
BOOL daNpc_Kf1_c::_delete() {
    WWHD_FUNC(0x02262184, BOOL, this);
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x02262184, &daNpc_Kf1_c::_delete);

/* 022621D8 */
static BOOL daNpc_Kf1_Delete(daNpc_Kf1_c* i_this) {
    WWHD_FUNC(0x022621D8, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022621D8, daNpc_Kf1_Delete);

/* 022621DC: on the second action frame, records the ids of the shop's 8 pots */
u8 daNpc_Kf1_c::srch_Tsubo() {
    WWHD_FUNC(0x022621DC, u8, this);
    if (mActState != 1) {
        return false;
    }
    m870 = 0xFFFFFFFF;
    l_tsubo_num() = 0;
    for (int i = 0; i < 20; i++) {
        l_tsubo()[i] = 0;
    }
    fpcEx_Search(0x0226177C /* searchActor_Tsubo */, this);
    if (l_tsubo_num() < 8) {
        return false;
    }
    mTsuboNum = 0;
    for (int i = 0; i < 8; i++) {
        u32 a = l_tsubo()[i];
        mTsuboIds[i] = a != 0 ? gabi::load<u32>(a + 4) /* fopAcM_GetID */ : 0xFFFFFFFF;
        mTsuboNum = mTsuboNum + 1;
    }
    mActState = mActState + 1;
    return true;
}
VERIFY(0x022621DC, &daNpc_Kf1_c::srch_Tsubo);

/* 022622D0 */
void daNpc_Kf1_c::checkOrder() {
    WWHD_FUNC(0x022622D0, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
            if (mEventIndex == 0) {
                /* the player faces the way it faces now (player + 0x422) */
                fopAc_ac_c* player = kf1_getPlayer();
                s16 ang = kf1_getPlayer()->current.angle.y;
                gabi::store<s16>(gabi::ea(player) + 0x422, ang);
            }
            mEvtState = 0;
            mMesgAnimeTag = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == 1 /* checkCommandTalk() */) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}
VERIFY(0x022622D0, &daNpc_Kf1_c::checkOrder);

/* 022623CC */
u8 daNpc_Kf1_c::demo() {
    WWHD_FUNC(0x022623CC, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo) {
            mbDemo = false;
        }
        return mbDemo;
    }
    if (!mbDemo) {
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
        mbDemo = true;
        mbNoShapeAngle = false;
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor_p = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001B904), 0x23A, STR(0x1001B8E8));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<void*>(0x02526E70, obj, id);
    }
    u32 btp = gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10);
    if (btp != 0) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(gabi::at<u8>(btp));
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = gabi::call<void*>(0x02527828, demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            mDoExt_btpAnm_init(mBtpAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btp_p, TRUE, 0, 1.0f, 0, -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 2;
        }
    }
    gabi::call(0x02527028, this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}
VERIFY(0x022623CC, &daNpc_Kf1_c::demo);

/* 02262588 */
s32 daNpc_Kf1_c::isEventEntry() {
    WWHD_FUNC(0x02262588, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x02262588, &daNpc_Kf1_c::isEventEntry);

/* 022625C8 (the matcher names it cLib_getRndValue<int>): o_isDeleted = 1 when the process is gone */
fopAc_ac_c* daNpc_Kf1_c::searchByID(u32 i_id, be<s32>* o_isDeleted) {
    WWHD_FUNC(0x022625C8, fopAc_ac_c*, this, i_id, o_isDeleted);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_isDeleted = 0;
    if (!fopAcM_SearchByID_out(i_id, actor)) {
        *o_isDeleted = 1;
    }
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x022625C8, &daNpc_Kf1_c::searchByID);

/* 0226261C: the number of the shop's pots that still exist */
s32 daNpc_Kf1_c::chk_tsubo() {
    WWHD_FUNC(0x0226261C, s32, this); /* an s16, sign-extended (callers use the whole register) */
    s16 num = 0;
    gabi::Local<be<s32>> is_deleted;
    for (int i = 0; i < 8; i++) {
        searchByID(mTsuboIds[i], is_deleted);
        if (*is_deleted == 0) {
            num++;
        }
    }
    return num;
}
VERIFY(0x0226261C, &daNpc_Kf1_c::chk_tsubo);

/* 02262698 */
int daNpc_Kf1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x02262698, int, this, i_bckNum);
    return gabi::load<s32>(0x1001BA20 + i_bckNum * 4);
}
VERIFY(0x02262698, &daNpc_Kf1_c::bckResID);

/* 022626AC */
void daNpc_Kf1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x022626AC, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    dNpc_setAnmIDRes(mpMorf, i_anmPrmP->loopMode, i_anmPrmP->morf, i_anmPrmP->speed, resID, -1, mArcName);
    s8 new_bck = i_anmPrmP->bckNum;
    mbMorfAnimStopped = false;
    mBckNum = new_bck;
    mPrevMorfFrame = 0.0f;
    m90D = 0;
}
VERIFY(0x022626AC, &daNpc_Kf1_c::setAnm_anm);

/* 02262744 */
void daNpc_Kf1_c::setAnm() {
    WWHD_FUNC(0x02262744, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BEE28);
    init_texPttrnAnm(a_anm_prm_tbl[mStt].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mStt]);
}
VERIFY(0x02262744, &daNpc_Kf1_c::setAnm);

/* 022627B4 */
void daNpc_Kf1_c::setStt(s8 i_stt) {
    WWHD_FUNC(0x022627B4, void, this, i_stt);
    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
        mEvtState = 0;
        mWalkTimer = (s16)cLib_getRndValue(60, 90);
        speedF = 0.0f;
        break;
    case 2:
        mEvtState = 0;
        mAnmAtr = 0xFF;
        mPrevStt = prev;
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        break;
    case 3:
        mEvtState = 0;
        mWalkTimer = (s16)cLib_getRndValue(90, 90);
        break;
    }
    setAnm();
}
VERIFY(0x022627B4, &daNpc_Kf1_c::setStt);

/* 02262880 */
void daNpc_Kf1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    WWHD_FUNC(0x02262880, void, this, i_anmNum, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BEE68);
    if (i_setBtp) {
        init_texPttrnAnm(a_anm_prm_tbl[i_anmNum].btpNum, true);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x02262880, &daNpc_Kf1_c::setAnm_NUM);

/* 022628F0 */
void daNpc_Kf1_c::endEvent() {
    WWHD_FUNC(0x022628F0, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}
VERIFY(0x022628F0, &daNpc_Kf1_c::endEvent);

/* 02262934 */
void daNpc_Kf1_c::cut_init_ANGRY_START(int i_staffIdx) {
    WWHD_FUNC(0x02262934, void, this, i_staffIdx);
    cXyz* pos = (cXyz*)dComIfGp_evmng_getMyXyzP(i_staffIdx, STR(0x1001BA48) /* "Pos" */);
    if (pos != nullptr) {
        f32 z = pos->z, y = pos->y, x = pos->x;
        current.pos.y = y;
        current.pos.z = z;
        current.pos.x = x;
        /* *mObjAcch.GetOldPos() = *mObjAcch.GetPos() (word copy) */
        u32 src = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x2C);
        u32 w0 = gabi::load<u32>(src);
        u32 dst = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x30);
        gabi::store<u32>(dst, w0);
        gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
        gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
    }
    f32 eye_y = eyePos.y;
    mLookMode = 2;
    mLookPos.z = 0.0f;
    mLookPos.y = eye_y;
    mLookPos.x = 0.0f;
    current.angle.y = cLib_targetAngleY(&current.pos, &mLookPos);
    speedF = 0.0f;
    setAnm_NUM(0, 1);
}
VERIFY(0x02262934, &daNpc_Kf1_c::cut_init_ANGRY_START);

/* 02262A20 */
void daNpc_Kf1_c::cut_init_BENSYOU_START(int i_staffIdx) {
    WWHD_FUNC(0x02262A20, void, this, i_staffIdx);
    cut_init_ANGRY_START(i_staffIdx);
    gabi::Local<cXyz> goal;
    goal->x = 0.0f;
    goal->z = 700.0f;
    goal->y = 0.0f;
    dComIfGp_evmng_setGoal(goal);
}
VERIFY(0x02262A20, &daNpc_Kf1_c::cut_init_BENSYOU_START);

/* 02262A6C */
void daNpc_Kf1_c::cut_init_TSUBO_CNT(int i_staffIdx) {
    WWHD_FUNC(0x02262A6C, void, this, i_staffIdx); /* the staff index is not used */
    s32 num = chk_tsubo();
    m_jnt.mAngles[0][0] = 0;
    m_jnt.mAngles[1][0] = 0;
    mBrokenNum = (s16)(mTsuboNum - num);
    m_jnt.mAngles[0][1] = 0;
    mLookMode = 0;
    m_jnt.mAngles[1][1] = 0;
}
VERIFY(0x02262A6C, &daNpc_Kf1_c::cut_init_TSUBO_CNT);

/* 02262ABC: the compensation (10 rupees a pot) is taken from the player */
void daNpc_Kf1_c::cut_init_BENSYOU(int i_staffIdx) {
    WWHD_FUNC(0x02262ABC, void, this, i_staffIdx); /* the staff index is not used */
    s32 broken = mBrokenNum;
    mRupee = dComIfGs_getRupee();
    u32 p = dComIfGp_ea() + 0x5B48; /* dComIfGp_setItemRupeeCount(-broken * 10) */
    gabi::store<s32>(p, gabi::load<s32>(p) - broken * 10);
}
VERIFY(0x02262ABC, &daNpc_Kf1_c::cut_init_BENSYOU);

/* 02262B08 */
void daNpc_Kf1_c::cut_init_GET_OUT(int i_staffIdx) {
    WWHD_FUNC(0x02262B08, void, this, i_staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BA54) /* "Timer" */);
    mCutTimer = 0;
    if (timer != nullptr) {
        mCutTimer = (s16)(s32)*timer;
    }
    /* player + 0x420/0x422/0x428/0x430: the player's event move */
    gabi::store<s16>(gabi::ea(kf1_getPlayer()) + 0x422, 0);
    u32 player = gabi::ea(kf1_getPlayer());
    gabi::store<s16>(player + 0x420, 3);
    gabi::store<u32>(player + 0x428, 0);
    if ((s32)mRupee >= mBrokenNum * 10) {
        gabi::Local<cXyz> sea;
        sea->x = 0.0f;
        sea->z = 999.0f;
        sea->y = 0.0f;
        cLib_targetAngleY(&kf1_getPlayer()->current.pos, sea); /* result unused */
        gabi::store<u32>(gabi::ea(kf1_getPlayer()) + 0x430, 3);
    } else {
        gabi::store<u32>(gabi::ea(kf1_getPlayer()) + 0x428, 1);
        gabi::store<u32>(gabi::ea(kf1_getPlayer()) + 0x430, 9);
    }
}
VERIFY(0x02262B08, &daNpc_Kf1_c::cut_init_GET_OUT);

/* 02262C1C (unnamed by the matcher) */
void daNpc_Kf1_c::cut_init_DSP_RUPEE_CNT(int i_staffIdx) {
    WWHD_FUNC(0x02262C1C, void, this, i_staffIdx); /* the staff index is not used */
    dComIfGp_offRupeeCntFlag();
}
VERIFY(0x02262C1C, &daNpc_Kf1_c::cut_init_DSP_RUPEE_CNT);

/* 02262C48 */
void daNpc_Kf1_c::cut_init_PLYER_TRN(int i_staffIdx) {
    WWHD_FUNC(0x02262C48, void, this, i_staffIdx); /* the staff index is not used */
    s16 ang = cLib_targetAngleY(&kf1_getPlayer()->current.pos, &current.pos);
    gabi::store<s16>(gabi::ea(kf1_getPlayer()) + 0x422, ang);
}
VERIFY(0x02262C48, &daNpc_Kf1_c::cut_init_PLYER_TRN);

/* 02262C94 */
void daNpc_Kf1_c::cut_init_START_AGE(int i_staffIdx) {
    WWHD_FUNC(0x02262C94, void, this, i_staffIdx); /* the staff index is not used */
    mLookPos.y = eyePos.y;
    mLookPos.x = 0.0f;
    mLookPos.z = 0.0f;
    s16 ang = cLib_targetAngleY(&current.pos, &mLookPos);
    mLookAngleY = ang;
    shape_angle.y = ang;
    m_jnt.mbTrn = 1; /* setTrn() */
    mLookMode = 3;
    mbNoShapeAngle = true;
    setAnm_NUM(0, 1);
}
VERIFY(0x02262C94, &daNpc_Kf1_c::cut_init_START_AGE);

/* 02262D08 */
void daNpc_Kf1_c::cut_init_PLYER_MOV(int i_staffIdx) {
    WWHD_FUNC(0x02262D08, void, this, i_staffIdx); /* the staff index is not used */
    s16 diff = (s16)(cLib_targetAngleY(&current.pos, &kf1_getPlayer()->current.pos) - current.angle.y);
    if (abs((int)diff) > 0x2000) {
        cXyz* pos = &kf1_getPlayer()->current.pos;
        dComIfGp_evmng_setGoal(pos);
        return;
    }
    gabi::Local<cXyz> offset;
    offset->y = 0.0f;
    offset->x = 0.0f;
    offset->z = 0.0f;
    s16 rot = diff > 0 ? 0x2800 : -0x2800;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(current.angle.y + rot));
    offset->z = 150.0f;
    gabi::Local<cXyz> goal;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, goal);
    dComIfGp_evmng_setGoal(goal);
}
VERIFY(0x02262D08, &daNpc_Kf1_c::cut_init_PLYER_MOV);

/* 02262E10: three of the 8 pots, chosen at random, will hold the rupees.
 * The random index is not range-checked (cM_rndF(32) / 4 is 0..7 in the game); to stay exact for any
 * index, the candidate keeps the original's 0x70-byte stack frame at the same address: back chain,
 * the search key (0x8), the float-to-int temporary (0x10), used[8] (0x18), saved r20..r31 (0x20),
 * f30/f31 (0x50/0x60, with their second halves as singles at 0x58/0x68), and the saved LR at 0x74. */
void daNpc_Kf1_c::cut_init_RUPEE_SET(int i_staffIdx) {
    WWHD_FUNC(0x02262E10, void, this, i_staffIdx); /* the staff index is not used */
    gabi::Local<u8[0x70]> frame; /* first local: at the entry SP - 0x70, as the original's frame */
    u32 F = frame.a;
    gabi::store<u32>(F, F + 0x70);
    gabi::store<u32>(F + 0x74, gabi::cpu->lr); /* the saved LR, in the caller's frame */
    for (int r = 20; r <= 31; r++)
        gabi::store<u32>(F + 0x20 + (r - 20) * 4, gabi::cpu->r[r]);
    for (int i = 0; i < 2; i++) {
        f64 ps0 = gabi::cpu->f[30 + i].ps0, ps1 = gabi::cpu->f[30 + i].ps1;
        u64 bits;
        memcpy(&bits, &ps0, 8);
        gabi::store<u32>(F + 0x50 + i * 0x10, (u32)(bits >> 32));
        gabi::store<u32>(F + 0x54 + i * 0x10, (u32)bits);
        gabi::store<f32>(F + 0x58 + i * 0x10, (f32)ps1);
    }
    if (mTsuboNum == 8) {
        u32 used = F + 0x18;
        gabi::store<u32>(used, 0);
        gabi::store<u32>(used + 4, 0);
        for (int i = 0; i < 3; i++) {
            s32 idx;
            do {
                idx = gabi::ftoi(cM_rndF(32.0f) * 0.25f);
                gabi::store<s32>(F + 0x10, idx);
            } while (gabi::load<u8>(used + idx) != 0);
            /* fopAcM_SearchByID(mTsuboIds[idx]) (HD inline; the key at 0x8) */
            u32 id = mTsuboIds[idx];
            gabi::store<u32>(F + 0x8, id);
            fopAc_ac_c* a_tsubo_actor = id == 0xFFFFFFFFu ? nullptr : fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, gabi::at<u8>(F + 0x8));
            if (a_tsubo_actor == nullptr) /* JUT_ASSERT(1485, NULL != a_tsubo_actor) */
                JUT_ASSERT_fail(STR(0x1001BA68), 0x5CD, STR(0x1001BA78));
            a_tsubo_actor->mParameters = (a_tsubo_actor->mParameters & ~0x3Fu) | 4; /* the pot's item: a rupee */
            dComIfGp_event_setItemPartner(a_tsubo_actor);
            id = mTsuboIds[idx];
            gabi::store<u8>(used + idx, 1);
            mKutaniIds[i] = id;
        }
    }
    setAnm_NUM(8, 1);
}
VERIFY(0x02262E10, &daNpc_Kf1_c::cut_init_RUPEE_SET);

/* 02262F64 */
void daNpc_Kf1_c::cut_init_TSUBO_ATN(int i_staffIdx) {
    WWHD_FUNC(0x02262F64, void, this, i_staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BA90) /* "Timer" */);
    be<s32>* count = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BA98) /* "Count" */);
    mCutTimer = 0;
    if (timer != nullptr) {
        mCutTimer = (s16)(s32)*timer;
    }
    mCutCount = 0;
    if (count != nullptr) {
        mCutCount = (s16)(s32)*count;
    }
}
VERIFY(0x02262F64, &daNpc_Kf1_c::cut_init_TSUBO_ATN);

/* 02263008 */
void daNpc_Kf1_c::cut_init_TLK_MSG(int i_staffIdx) {
    WWHD_FUNC(0x02263008, void, this, i_staffIdx);
    be<u32>* msg_num = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BAA0) /* "MsgNum" */);
    be<u32>* end_msg = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BAA8) /* "EndMsg" */);
    mAnmAtr = 0xFF;
    mCurrMsgNo = 0;
    mEndMsgNo = 0xFFFFFFFF;
    mMesgAnimeTag = 0xFF;
    mAtrSet = 0;
    if (end_msg != nullptr) {
        mEndMsgNo = *end_msg;
    }
    if (msg_num != nullptr) {
        u32 no = *msg_num;
        mCurrMsgNo = no;
        switch (no) {
        case 0x1C2D: {
            s16 v = (s16)(mBrokenNum * 10);
            gabi::store<s16>(dComIfGp_ea() + 0x5BA0, v); /* the message's number value */
            break;
        }
        case 0x1C2F:
        case 0x1C30:
            mCurrMsgNo = (s32)mRupee >= mBrokenNum * 10 ? 0x1C2F : 0x1C30;
            break;
        case 0x1C39: {
            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x21, up);
            break;
        }
        }
    }
    mCurrMsgBsPcId = 0xFFFFFFFF;
}
VERIFY(0x02263008, &daNpc_Kf1_c::cut_init_TLK_MSG);

/* 02263170 */
void daNpc_Kf1_c::cut_init_CONTNUE_TLK(int i_staffIdx) {
    WWHD_FUNC(0x02263170, void, this, i_staffIdx);
    be<u32>* end_msg = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001BAB0) /* "EndMsg" */);
    mEndMsgNo = 0xFFFFFFFF;
    if (end_msg != nullptr) {
        mEndMsgNo = *end_msg;
    }
}
VERIFY(0x02263170, &daNpc_Kf1_c::cut_init_CONTNUE_TLK);

/* 022631D8 */
u32 daNpc_Kf1_c::cut_move_GET_OUT() {
    WWHD_FUNC(0x022631D8, u32, this);
    if (cLib_calcTimer(&mCutTimer) == 0) {
        dComIfGp_setNextStage(STR(0x1001BAB8) /* "sea" */, 3, 0xB, -1, 0.0f, 0, 1, 0);
    }
    return FALSE;
}
VERIFY(0x022631D8, &daNpc_Kf1_c::cut_move_GET_OUT);

/* 02263234 */
u32 daNpc_Kf1_c::cut_move_RUPEE_CNT_END() {
    WWHD_FUNC(0x02263234, u32, this);
    u32 p = dComIfGp_ea();
    if (dComIfGs_getRupee() == gabi::load<u16>(p + 0x5BAE)) {
        dComIfGp_onRupeeCntFlag();
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02263234, &daNpc_Kf1_c::cut_move_RUPEE_CNT_END);

/* 02263294 (unnamed by the matcher) */
u32 daNpc_Kf1_c::cut_move_START_AGE() {
    WWHD_FUNC(0x02263294, u32, this);
    return (u8)(m_jnt.mbTrn ^ 1);
}
VERIFY(0x02263294, &daNpc_Kf1_c::cut_move_START_AGE);

/* 022632A4 (cXyz by value: a pointer to the caller's copy) */
void daNpc_Kf1_c::create_rupee(cXyz* i_pos, int i_num) {
    WWHD_FUNC(0x022632A4, void, this, i_pos, i_num);
    /* the stack frame: the angle, then the offset table */
    struct frame_l {
        /* 0x0 */ csXyz angle;
        /* 0x6 */ u8 _6[2];
        /* 0x8 */ be<f32> offs[3];
    };
    gabi::Local<frame_l> frm;
    csXyz_ct(&frm->angle, 0, 0, 0);
    s32 cnt = g_Counter_mCounter0();
    for (int i = 0; i < i_num; i++, cnt++) {
        /* static const f32 a_offs[3] = {-30, 0, 30} (copied) */
        u32 w0 = gabi::load<u32>(0x1001BAEC), w1 = gabi::load<u32>(0x1001BAF0), w2 = gabi::load<u32>(0x1001BAF4);
        gabi::store<u32>(gabi::ea(&frm->offs[0]), w0);
        gabi::store<u32>(gabi::ea(&frm->offs[1]), w1);
        gabi::store<u32>(gabi::ea(&frm->offs[2]), w2);
        f32 r = cM_rndF(30.0f) - 15.0f;
        f32 off = gabi::load<f32>(gabi::ea(&frm->offs[0]) + (cnt % 3) * 4);
        s16 deg = (s16)gabi::ftoi(r + off);
        frm->angle.y = (s16)(current.angle.y + (s16)gabi::ftoi((f32)deg * 182.0389f)); /* cM_deg2s */
        f32 speed_f = cM_rndFX(2.0f) + 13.0f;
        f32 speed_y = cM_rndFX(4.0f) + 31.0f;
        fopAc_ac_c* a_actor_p = fopAcM_createItemForKP2(i_pos, 4 /* RED_RUPEE? */, fopAcM_GetRoomNo(this), nullptr, nullptr,
                                                        speed_f, speed_y, -2.0f, 1);
        if (a_actor_p == nullptr) { /* JUT_ASSERT(1052, a_actor_p != NULL) */
            JUT_ASSERT_fail(STR(0x1001BADC), 0x41C, STR(0x1001BAF8));
            continue;
        }
        u32 st = a_actor_p->actor_status;
        a_actor_p->scale.x = 0.2f;
        a_actor_p->scale.y = 0.2f;
        a_actor_p->scale.z = 0.2f;
        a_actor_p->actor_status = (st | 0x4000) & ~0x80u;
        u16 ax = gabi::load<u16>(gabi::ea(&frm->angle) + 0);
        gabi::store<u16>(gabi::ea(&a_actor_p->shape_angle) + 0, ax);
        u16 ay = gabi::load<u16>(gabi::ea(&frm->angle) + 2);
        gabi::store<u16>(gabi::ea(&a_actor_p->shape_angle) + 2, ay);
        u16 az = gabi::load<u16>(gabi::ea(&frm->angle) + 4);
        gabi::store<u16>(gabi::ea(&a_actor_p->shape_angle) + 4, az);
        gabi::store<u16>(gabi::ea(&a_actor_p->current.angle) + 0, ax);
        gabi::store<u16>(gabi::ea(&a_actor_p->current.angle) + 2, ay);
        gabi::store<u16>(gabi::ea(&a_actor_p->current.angle) + 4, az);
        mRupeeIds[i] = gabi::load<u32>(gabi::ea(a_actor_p) + 4); /* fopAcM_GetID */
    }
}
VERIFY(0x022632A4, &daNpc_Kf1_c::create_rupee);

/* 022635C4: the rupee shown over the chosen pot (the "camera" item) */
void daNpc_Kf1_c::ready_kutaniCamera(int i_idx, int i_flg) {
    WWHD_FUNC(0x022635C4, void, this, i_idx, i_flg);
    gabi::Local<be<s32>> is_deleted;
    fopAc_ac_c* a_actor = searchByID(mCameraItemId, is_deleted);
    if (a_actor != nullptr && *is_deleted == 0) {
        fopAcM_delete(a_actor);
    }
    if (!i_flg) {
        return;
    }
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    a_actor = searchByID(mKutaniIds[i_idx], is_deleted);
    if (a_actor == nullptr || *is_deleted != 0) /* JUT_ASSERT(1084, NULL != a_actor && 0 == i_flg) */
        JUT_ASSERT_fail(STR(0x1001BB18), 0x43C, STR(0x1001BB28));
    dComIfGp_event_setItemPartner(a_actor);
    angle->y = a_actor->current.angle.y;
    gabi::Local<cXyz> pos;
    pos->x = a_actor->current.pos.x;
    pos->y = a_actor->current.pos.y + 180.0f;
    pos->z = a_actor->current.pos.z;
    fopAc_ac_c* item = fopAcM_createItemForKP2(pos, 4, fopAcM_GetRoomNo(this), nullptr, nullptr, 0.0f, 0.0f, -4.0f, 1);
    if (item == nullptr) /* JUT_ASSERT(1096, NULL != a_actor) */
        JUT_ASSERT_fail(STR(0x1001BB18), 0x448, STR(0x1001BB48));
    u32 st = item->actor_status;
    item->scale.z = 0.1f;
    item->scale.y = 0.1f;
    item->scale.x = 0.1f;
    item->actor_status = (st | 0x4000) & ~0x80u;
    gabi::store<u16>(gabi::ea(&item->current.angle) + 0, gabi::load<u16>(gabi::ea(angle.get()) + 0));
    gabi::store<u16>(gabi::ea(&item->current.angle) + 2, gabi::load<u16>(gabi::ea(angle.get()) + 2));
    gabi::store<u16>(gabi::ea(&item->current.angle) + 4, gabi::load<u16>(gabi::ea(angle.get()) + 4));
    mCameraItemId = item != nullptr ? gabi::load<u32>(gabi::ea(item) + 4) : 0xFFFFFFFF;
}
VERIFY(0x022635C4, &daNpc_Kf1_c::ready_kutaniCamera);

/* 022637C4 */
u32 daNpc_Kf1_c::cut_move_RUPEE_SET() {
    WWHD_FUNC(0x022637C4, u32, this);
    gabi::Local<be<s32>> is_deleted;
    if (mbMorfAnimStopped == 0) {
        if (mpMorf->checkFrame(68.0f)) {
            gabi::Local<cXyz> offset;
            offset->x = 0.0f;
            offset->z = 40.0f;
            offset->y = 40.0f;
            mDoAud_seStart(0x69E9, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_stack_c::YrotM(current.angle.y);
            gabi::Local<cXyz> pos;
            PSMTXMultVec(mDoMtx_stack_c::get(), offset, pos);
            gabi::Local<cXyz> pos_copy;
            f32 x = pos->x, y = pos->y;
            pos_copy->x = x;
            f32 z = pos->z;
            pos_copy->y = y;
            pos_copy->z = z;
            create_rupee(pos_copy, 3);
            return FALSE;
        }
        if (68.0f < mpMorf->getFrame()) {
            for (int i = 0; i < 3; i++) {
                fopAc_ac_c* a = searchByID(mRupeeIds[i], is_deleted);
                if (a != nullptr && *is_deleted == 0) {
                    eyePos.copy(a->current.pos);
                }
            }
            mbSetEyePos = 0;
        }
        return FALSE;
    }
    u32 num = 0;
    for (int i = 0; i < 3; i++) {
        fopAc_ac_c* a = searchByID(mRupeeIds[i], is_deleted);
        if (a != nullptr) {
            fopAcM_delete(a);
            num++;
        } else if (*is_deleted != 0) {
            num++;
        }
    }
    if (num != 3) {
        return FALSE;
    }
    ready_kutaniCamera(0, 1);
    mbSetEyePos = 1;
    setAnm_NUM(0, 1);
    return TRUE;
}
VERIFY(0x022637C4, &daNpc_Kf1_c::cut_move_RUPEE_SET);

/* 022639E0 */
u32 daNpc_Kf1_c::cut_move_TSUBO_ATN() {
    WWHD_FUNC(0x022639E0, u32, this);
    if (cLib_calcTimer(&mCutTimer) == 0) {
        u32 count = (u32)(s32)mCutCount;
        if (count < 4) {
            if (count <= 2) {
                ready_kutaniCamera((s32)count, 1);
            } else {
                ready_kutaniCamera(0, 0);
            }
        }
        return TRUE;
    }
    if (mCutTimer == 0x14) {
        mDoAud_seStart(0x6981, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
    return FALSE;
}
VERIFY(0x022639E0, &daNpc_Kf1_c::cut_move_TSUBO_ATN);

/* 02263A94 */
u32 daNpc_Kf1_c::cut_move_TLK_MSG() {
    WWHD_FUNC(0x02263A94, u32, this);
    u16 status = talk(0);
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mMesgAnimeTag = 0xFF;
        mAtrSet = 0;
        mAnmAtr = 0xFF;
        return TRUE;
    }
    if (status == 2 || status == 6) {
        return mCurrMsgNo == mEndMsgNo;
    }
    return FALSE;
}
VERIFY(0x02263A94, &daNpc_Kf1_c::cut_move_TLK_MSG);

/* 02263B34 */
void daNpc_Kf1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x02263B34, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101BEF08): ANGRY_START, BENSYOU_START, TSUBO_CNT, BENSYOU, GET_OUT, DSP_RUPEE_CNT,
     * PLYER_TRN, RUPEE_CNT_END, START_AGE, PLYER_MOV, RUPEE_SET, TSUBO_ATN, TLK_MSG, CONTNUE_TLK */
    if (i_staffIdx == -1) {
        return;
    }
    mActionIndex = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BEF08, 14, TRUE, 0);
    if (mActionIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staffIdx)) {
        switch ((u32)(s32)mActionIndex) {
        case 0: cut_init_ANGRY_START(i_staffIdx); break;
        case 1: cut_init_BENSYOU_START(i_staffIdx); break;
        case 2: cut_init_TSUBO_CNT(i_staffIdx); break;
        case 3: cut_init_BENSYOU(i_staffIdx); break;
        case 4: cut_init_GET_OUT(i_staffIdx); break;
        case 5: cut_init_DSP_RUPEE_CNT(i_staffIdx); break;
        case 6: cut_init_PLYER_TRN(i_staffIdx); break;
        case 8: cut_init_START_AGE(i_staffIdx); break;
        case 9: cut_init_PLYER_MOV(i_staffIdx); break;
        case 10: cut_init_RUPEE_SET(i_staffIdx); break;
        case 11: cut_init_TSUBO_ATN(i_staffIdx); break;
        case 12: cut_init_TLK_MSG(i_staffIdx); break;
        case 13: cut_init_CONTNUE_TLK(i_staffIdx); break;
        }
    }
    u32 cut_end;
    switch ((u32)(s32)mActionIndex) {
    case 4: cut_end = cut_move_GET_OUT(); break;
    case 7: cut_end = cut_move_RUPEE_CNT_END(); break;
    case 8: cut_end = cut_move_START_AGE(); break;
    case 10: cut_end = cut_move_RUPEE_SET(); break;
    case 11: cut_end = cut_move_TSUBO_ATN(); break;
    case 12:
    case 13: cut_end = cut_move_TLK_MSG(); break;
    default: cut_end = TRUE; break;
    }
    if (cut_end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x02263B34, &daNpc_Kf1_c::privateCut);

/* 02263E2C */
void daNpc_Kf1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x02263E2C, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        switch ((u32)(s32)mEventIndex) {
        case 0: /* "angry" */
            dComIfGs_onEventBit(0x2780);
            mTsuboNum = chk_tsubo();
            break;
        case 1: { /* "rupee_age" */
            u8 reg = dComIfGs_getEventReg(0xBCFF);
            dComIfGs_setEventReg(0xBCFF, reg | 1);
            mbNoShapeAngle = false;
            setStt(1);
            setAnm_NUM(0, 1);
            mWaitTimer = (s16)cLib_getRndValue(30, 60);
            m900 = (s16)cLib_getRndValue(15, 30);
            m911 = 1;
            break;
        }
        }
        mWaitTimer = (s16)cLib_getRndValue(30, 60);
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}
VERIFY(0x02263E2C, &daNpc_Kf1_c::event_proc);

/* 02263FF0 (unnamed by the matcher) */
void daNpc_Kf1_c::lookBack() {
    WWHD_FUNC(0x02263FF0, void, this);
    gabi::Local<cXyz> look_pos;
    look_pos->x = 0.0f;
    f32 srcX = current.pos.x;
    s16 target_y = current.angle.y;
    look_pos->y = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    u8 head_only = mbHeadOnly;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    look_pos->z = 0.0f;
    mActorAngleY = target_y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        mLookPos.copy(*eye);
        look_pos->copy(*eye);
        look_pos_p = look_pos;
        break;
    }
    case 2:
        look_pos->copy(mLookPos);
        look_pos_p = look_pos;
        break;
    case 3:
        target_y = mLookAngleY;
        break;
    case 4: {
        gabi::Local<be<s32>> is_deleted;
        fopAc_ac_c* a = searchByID(mLookActorId, is_deleted);
        if (a != nullptr && *is_deleted == 0) {
            mLookPos.copy(a->current.pos);
            look_pos->x = mLookPos.x;
            f32 eye_y = a->eyePos.y;
            look_pos->y = eye_y;
            look_pos->z = mLookPos.z;
            look_pos_p = look_pos;
            mLookPos.y = eye_y;
        }
        break;
    }
    }
    gabi::Local<cXyz> cur_pos; /* passed by value: a copy */
    cur_pos->x = srcX;
    cur_pos->z = srcZ;
    cur_pos->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, look_pos_p, cur_pos, target_y, l_HIO().mPrm.mLookVelMax, head_only);
}
VERIFY(0x02263FF0, &daNpc_Kf1_c::lookBack);

/* 02264284 */
void daNpc_Kf1_c::eventOrder() {
    WWHD_FUNC(0x02264284, void, this);
    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        mEventIndex = (s16)(state - 3);
        fopAcM_orderOtherEventId(this, mEventIDTbl[mEventIndex], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02264284, &daNpc_Kf1_c::eventOrder);

/* 022642F4 */
BOOL daNpc_Kf1_c::_execute() {
    WWHD_FUNC(0x022642F4, BOOL, this);
    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Kf1_HIO_c::hio_prm_c& prm = l_HIO().mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (m917 && demoActorID == 0) {
        return TRUE;
    }
    srch_Tsubo();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if ((dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
             (staff_id = isEventEntry()) >= 0) ||
            m915) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        u32 flags = mObjAcch.m_flags;
        mbAcchFlag20 = (flags >> 5) & 1;
        mbAcchFlag1000 = (flags >> 12) & 1;
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        m917 = 0;
    }
    eventOrder();
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    if (!mbNoShapeAngle) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    setMtx(false);
    if (!mbDemo) {
        setCollision(90.0f, 200.0f);
    }
    return TRUE;
}
VERIFY(0x022642F4, &daNpc_Kf1_c::_execute);

/* 022645F4 */
static BOOL daNpc_Kf1_Execute(daNpc_Kf1_c* i_this) {
    WWHD_FUNC(0x022645F4, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022645F4, daNpc_Kf1_Execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 022645F8 */
BOOL daNpc_Kf1_c::_draw() {
    WWHD_FUNC(0x022645F8, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = J3DModel_getModelData_l(morf_model_p);
    if (m917 || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(morf_model_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    if (mpItemModel.get()) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpItemModel, &tevStr);
        mDoExt_modelEntryDL(mpItemModel);
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x59 /* DSNAP_TYPE_NPC_KF1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO().mPrm.mDebugDraw) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001B888);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001B88C);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001B890);
    }
    return TRUE;
}
VERIFY(0x022645F8, &daNpc_Kf1_c::_draw);

/* 02264780 */
static BOOL daNpc_Kf1_Draw(daNpc_Kf1_c* i_this) {
    WWHD_FUNC(0x02264780, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02264780, daNpc_Kf1_Draw);

/* 02264784 */
static BOOL daNpc_Kf1_IsDelete(daNpc_Kf1_c*) {
    WWHD_FUNC(0x02264784, BOOL, (daNpc_Kf1_c*)nullptr);
    return TRUE;
}
VERIFY(0x02264784, daNpc_Kf1_IsDelete);

/* 0226478C */
void daNpc_Kf1_c::setAnm_ATR() {
    WWHD_FUNC(0x0226478C, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BEF40); /* [11] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].btpNum, true);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}
VERIFY(0x0226478C, &daNpc_Kf1_c::setAnm_ATR);

/* 022647F4 */
void daNpc_Kf1_c::chngAnmAtr(u8 i_atr) {
    WWHD_FUNC(0x022647F4, void, this, i_atr);
    if (mCurrMsgNo == 0x1C2E) {
        dComIfGp_offRupeeCntFlag();
    }
    if (i_atr == mAnmAtr || i_atr >= 11) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}
VERIFY(0x022647F4, &daNpc_Kf1_c::chngAnmAtr);

/* 02264864 */
void daNpc_Kf1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x02264864, void, this);
    u8 atr = mAnmAtr;
    if ((atr == 7 || atr == 9) && mbMorfAnimStopped) {
        setAnm_NUM(0, 1);
        mAnmAtr = 0;
    }
}
VERIFY(0x02264864, &daNpc_Kf1_c::ctrlAnmAtr);

/* 022648C4 */
void daNpc_Kf1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x022648C4, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            chngAnmAtr(gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925) /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            mMesgAnimeTag = tag;
            /* chngAnmTag(): empty */
        }
        break;
    }
    case 14:
        mAtrSet = 0;
        break;
    }
    ctrlAnmAtr();
}
VERIFY(0x022648C4, &daNpc_Kf1_c::anmAtr);

/* 02264984 */
u16 daNpc_Kf1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x02264984, u16, this, o_msgNoP);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 mesg = gabi::load<u32>(0x101F4B5C); /* HD message manager: the selected answer at +0x948 */
    switch (*o_msgNoP) {
    case 0x1C21:
        *o_msgNoP = 0x1C22;
        break;
    case 0x1C22:
        *o_msgNoP = 0x1C34;
        break;
    case 0x1C23: {
        u32 sel = gabi::load<u32>(mesg + 0x948);
        if (sel == 0) {
            *o_msgNoP = 0x1C24;
        } else if (sel == 1) {
            *o_msgNoP = 0x1C25;
        }
        break;
    }
    case 0x1C24:
        dComIfGs_onEventBit(0x0B02);
        msg_status = 0x10;
        break;
    case 0x1C27:
        *o_msgNoP = 0x1C28;
        break;
    case 0x1C28: {
        u32 sel = gabi::load<u32>(mesg + 0x948);
        if (sel == 0) {
            *o_msgNoP = 0x1C2A;
        } else if (sel == 1) {
            *o_msgNoP = 0x1C29;
        }
        break;
    }
    case 0x1C2A:
        *o_msgNoP = 0x1C2B;
        break;
    case 0x1C2B:
        m910 = 1;
        msg_status = 0x10;
        break;
    case 0x1C2D:
        *o_msgNoP = 0x1C2E;
        break;
    case 0x1C30:
        *o_msgNoP = 0x1C31;
        break;
    case 0x1C33:
        *o_msgNoP = (s32)mRupee >= mBrokenNum * 10 ? 0x1C2F : 0x1C30;
        break;
    case 0x1C34:
        *o_msgNoP = 0x1C23;
        break;
    case 0x1C36:
        *o_msgNoP = 0x1C37;
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x02264984, &daNpc_Kf1_c::next_msgStatus);

/* 02264B38 */
u32 daNpc_Kf1_c::getMsg_KF1_0() {
    WWHD_FUNC(0x02264B38, u32, this);
    if (m911) {
        return 0x1C38;
    }
    if (dComIfGs_isEventBit(0x0A02) && !dComIfGs_isSymbol(0)) {
        return 0x1C3B;
    }
    if (dComIfGs_isEventBit(0x0B02)) {
        u8 reg = dComIfGs_getEventReg(0xBCFF);
        if (dKy_daynight_check() == 1 || !dComIfGs_isEventBit(0x2780) || (reg & 1)) {
            return 0x1C26;
        }
        return 0x1C27;
    }
    return 0x1C21;
}
VERIFY(0x02264B38, &daNpc_Kf1_c::getMsg_KF1_0);

/* 02264C74 */
u32 daNpc_Kf1_c::getMsg() {
    WWHD_FUNC(0x02264C74, u32, this);
    if (mType == 0) {
        return getMsg_KF1_0();
    }
    return 0;
}
VERIFY(0x02264C74, &daNpc_Kf1_c::getMsg);

/* 02264CAC */
u8 daNpc_Kf1_c::chk_talk() {
    WWHD_FUNC(0x02264CAC, u8, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mItemNo = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mItemNo = 0xFF;
    return true;
}
VERIFY(0x02264CAC, &daNpc_Kf1_c::chk_talk);

/* 02264D44 */
u8 daNpc_Kf1_c::chkAttention() {
    WWHD_FUNC(0x02264D44, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02264D44, &daNpc_Kf1_c::chkAttention);

/* 02264DCC: a pot was broken: "angry" (state 3), or after the first time, "bensyou" (state 5) */
u8 daNpc_Kf1_c::orderTsuboEvent() {
    WWHD_FUNC(0x02264DCC, u8, this);
    if (mbTalk) {
        return false;
    }
    if (!dComIfGs_isEventBit(0x2780)) {
        s32 num = chk_tsubo();
        if (mTsuboNum > num) {
            mEvtState = 3;
            return true;
        }
        return false;
    }
    if (mSwitchNo != 0xFF && dComIfGs_isSwitch(mSwitchNo, fopAcM_GetRoomNo(this))) {
        s32 num = chk_tsubo();
        if (mTsuboNum > num) {
            mEvtState = 5;
            return true;
        }
    }
    return false;
}
VERIFY(0x02264DCC, &daNpc_Kf1_c::orderTsuboEvent);

/* 02264EC0 */
BOOL daNpc_Kf1_c::wait_1() {
    WWHD_FUNC(0x02264EC0, BOOL, this);
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
            mbHeadOnly = false;
            m_jnt.mbTrn = 1; /* setTrn() */
            mLookMode = 1;
        }
        return TRUE;
    }
    s8 bck = mBckNum;
    if (mEvtState < 3) {
        mEvtState = 2;
    }
    mbHeadOnly = true;
    if (bck == 9) {
        mWaitTimer = 0;
    }
    if (cLib_calcTimer(&mWaitTimer) == 0) {
        if (mBckNum != 9) {
            setAnm_NUM(9, 1);
            mWalkTimer = 0;
        }
        if (cLib_calcTimer(&mWalkTimer) == 0) {
            setStt(3);
            return TRUE;
        }
    }
    mLookMode = 0;
    return TRUE;
}
VERIFY(0x02264EC0, &daNpc_Kf1_c::wait_1);

/* 02264FC4 */
BOOL daNpc_Kf1_c::walk_1() {
    WWHD_FUNC(0x02264FC4, BOOL, this);
    dPath* path = mPathRun.mPath;
    if (path == nullptr || (gabi::load<u8>(gabi::ea(path) + 5) & 1) == 0 /* dPath_ChkClose() */) {
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
        dNpc_PathRun_nextIdxAuto(&mPathRun);
    }
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    gabi::Local<cXyz> target;
    target->copy(*pnt);
    s16 ang = cLib_targetAngleY(&current.pos, target);
    cLib_addCalcAngleS(&current.angle.y, ang, l_HIO().mPrm.mWalkTurnScale, l_HIO().mPrm.mWalkTurnStep, 0x80);
    f32 spd = l_HIO().mPrm.mWalkSpeed;
    if (cLib_calcTimer(&mWalkTimer) == 0 || mbTalk) {
        spd = 0.0f;
    }
    cLib_chaseF(&speedF, spd, l_HIO().mPrm.mWalkAccel);
    f32 rate = speedF * l_HIO().mPrm.mWalkAnmRate;
    s32 ispd = gabi::ftoi(spd);
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
    if (ispd == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk) {
            if (chk_talk()) {
                setStt(1);
                setAnm_NUM(9, 1);
                mbHeadOnly = false;
                m_jnt.mbTrn = 1; /* setTrn() */
                mLookMode = 1;
            }
        } else {
            setStt(1);
            setAnm_NUM(9, 1);
        }
        return TRUE;
    }
    if (mEvtState < 3) {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    return TRUE;
}
VERIFY(0x02264FC4, &daNpc_Kf1_c::walk_1);

/* 022651E0 */
BOOL daNpc_Kf1_c::talk_1() {
    WWHD_FUNC(0x022651E0, BOOL, this);
    bool sold = false;
    talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    /* HD: the message status comes from the message manager */
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        if (mCurrMsgNo == 0x1C2B) {
            sold = true;
        }
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        setAnm_NUM(0, 1);
        mWaitTimer = (s16)cLib_getRndValue(30, 60);
        m900 = (s16)cLib_getRndValue(15, 30);
        if (sold) {
            mEvtState = 4;
        }
        endEvent();
    }
    return mBckNum != 4;
}
VERIFY(0x022651E0, &daNpc_Kf1_c::talk_1);

/* 022652F0 */
BOOL daNpc_Kf1_c::wait_action1(void*) {
    WWHD_FUNC(0x022652F0, BOOL, this, (void*)nullptr);
    s8 act = mActState;
    if (act == 0) {
        setStt(3);
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)act > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case 1:
        mbSetEyePos = wait_1();
        break;
    case 2:
        mbSetEyePos = talk_1();
        break;
    case 3:
        mbSetEyePos = walk_1();
        break;
    }
    if (mActState > 1) {
        orderTsuboEvent();
    }
    return TRUE;
}
VERIFY(0x022652F0, &daNpc_Kf1_c::wait_action1);

/* 022653DC daNpc_Kf1_HIO_c::daNpc_Kf1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Kf1_HIO_c* daNpc_Kf1_HIO_c_ct(daNpc_Kf1_HIO_c* i_this) {
    WWHD_FUNC(0x022653DC, daNpc_Kf1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Kf1_HIO_c*)operator_new(0x3C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001B8D8;
    memcpy_g(&i_this->mPrm, gabi::at<u8>(0x101BEFF0), 0x30); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->m04 = -1;
    i_this->m08 = -1;
    return i_this;
}
VERIFY(0x022653DC, daNpc_Kf1_HIO_c_ct);

/* 02265448: static initialisation of the translation unit */
static void __sinit_d_a_npc_kf1_cpp() {
    WWHD_FUNC(0x02265448, void);
    sinit_header_statics(0x104674C4, 0x101BF020);
    daNpc_Kf1_HIO_c_ct(&l_HIO()); /* static daNpc_Kf1_HIO_c l_HIO */
}
VERIFY(0x02265448, __sinit_d_a_npc_kf1_cpp);

/* 022654E8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022654E8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022654E8, SafeString_dt);

/* 022654FC: daNpc_Kf1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Kf1_c_dt(daNpc_Kf1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022654FC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001B8B8);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001B8C8);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022654FC, daNpc_Kf1_c_dt);

/* 02265598: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x02265598, void, (SafeString*)nullptr);
}
VERIFY(0x02265598, SafeString_assureTerminationImpl);
