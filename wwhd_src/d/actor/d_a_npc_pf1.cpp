/**
 * d_a_npc_pf1.cpp (WWHD)
 * NPC - Maggie's father (poor)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_pf1.cpp) has only "Nonmatching" stubs for this unit: every function here is
 * written from the WWHD code (cking.rpx) with the GameCube names, and verified against it.
 */
#include "d/actor/d_a_npc_pf1.h"

static BOOL nodeCB_Head(J3DNode*, int);
static BOOL nodeCB_BackBone(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
const gc_tu_data gc_tu = {.safeString = 0x10020A14};
static auto& l_HIO = gabi::object_at<daNpc_Pf1_HIO_c>(0x10468358);
static auto& a_eye_pos_off = gabi::object_at<cXyz>(0x1046834C);
static auto& l_eyeGuard = gabi::object_at<be<u32>>(0x104683A4);
static auto& a_btp_res_tbl = gabi::object_at<be<s32>[3]>(0x10020AE4);
static auto& a_bck_res_tbl = gabi::object_at<be<s32>[8]>(0x10020BB8);
static auto& l_eventName = gabi::object_at<gptr<char>>(0x101C4D00);
static auto& a_heap_size_tbl = gabi::object_at<be<u32>[2]>(0x101C4D24);
static auto& a_cut_tbl = gabi::object_at<gptr<char>[1]>(0x101C4D2C);
static auto& a_anm_num_tbl = gabi::object_at<daNpc_Pf1_c::anm_prm_c[8]>(0x101C4D30);
static auto& a_anm_prm_tbl = gabi::object_at<daNpc_Pf1_c::anm_prm_c[8]>(0x101C4DB0);
static auto& a_anm_atr_tbl = gabi::object_at<daNpc_Pf1_c::anm_prm_c[6]>(0x101C4E30);
static auto& a_prm_tbl = gabi::object_at<daNpc_Pf1_HIO_c::hio_prm_c>(0x101C4E90);
static auto& l_waitAction = gabi::object_at<Pf1ActionFunc>(0x100209F0);
static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_message = gabi::object_at<gptr<void>>(0x101F4B5C);
static auto& l_frameCounter = gabi::object_at<be<u32>>(0x101FF558);
static auto& l_arcName = gabi::object_at<be<u8>[3]>(0x10020B88);
static auto& l_debugGuard0 = gabi::object_at<be<u32>>(0x101FDA48);
static auto& l_debugColour0 = gabi::object_at<be<u32>>(0x101FEBEC);
static auto& l_debugSource0 = gabi::object_at<be<u32>>(0x100209F8);
static auto& l_debugGuard1 = gabi::object_at<be<u32>>(0x101FDA44);
static auto& l_debugColour1 = gabi::object_at<be<u32>>(0x101FEBE8);
static auto& l_debugSource1 = gabi::object_at<be<u32>>(0x100209FC);
static auto& l_debugGuard2 = gabi::object_at<be<u32>>(0x101FDA50);
static auto& l_debugColour2 = gabi::object_at<be<u32>>(0x101FEBF4);
static auto& l_debugSource2 = gabi::object_at<be<u32>>(0x10020A00);
static auto& l_debugGuard3 = gabi::object_at<be<u32>>(0x101FDAC0);
static auto& l_debugColour3 = gabi::object_at<be<u32>>(0x101FEBF8);
static auto& l_debugSource3 = gabi::object_at<be<u32>>(0x10020A04);
/* inlined into attk_1 */
static inline void pf1_chase(daNpc_Pf1_c* i_this) {
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    s16 ang = cLib_targetAngleY(&i_this->current.pos, &dComIfGp_getLinkPlayer()->current.pos);
    cLib_addCalcAngleS(&i_this->current.angle.y, ang, prm.mTurnScale, prm.mTurnMaxStep, 0x80);
    cLib_chaseF(&i_this->speedF, prm.mAttkSpeed, prm.mAttkAccel);
    f32 rate = i_this->speedF * prm.mAttkAnmRate;
    i_this->mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
}

/* 022C89FC */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Pf1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_Head(i_node, model);
    }
    return TRUE;
}

/* 022C887C */
void daNpc_Pf1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {
    cXyz* eyeOffset = &a_eye_pos_off;
    if ((u32)l_eyeGuard == 0) {
        l_eyeGuard = 1;
        eyeOffset->z = 0.0f;
        eyeOffset->x = 20.0f;
        eyeOffset->y = -30.0f;
    }
    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, eyeOffset, &mEyePos);
    mDoMtx_YrotM(stk, -m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 022C8B60 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daNpc_Pf1_c*>(model->getUserArea());
        if (actor)
            actor->_nodeCB_BackBone(i_node, model);
    }
    return TRUE;
}

/* 022C8A44 */
void daNpc_Pf1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {

    u32 jnt_no = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jnt_no), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jnt_no), stk);
}

/* 022C900C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {

    return static_cast<daNpc_Pf1_c*>(i_this)->CreateHeap();
}

/* 022C9248 */
bool daNpc_Pf1_c::init_PF1_0() {

    if (dComIfGs_isEventBit(0x2D01)) {
        return false;
    }
    Pf1ActionFunc pmf;
    Pf1_loadAction(&pmf, &l_waitAction);
    set_action(&pmf, nullptr);
    return true;
}

/* 022C9578 */
bool daNpc_Pf1_c::createInit() {
    const char* name = l_eventName.get();
    mEventIDTbl[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2("Pf1" /* "Pf1" */, (fopNpc_npc_c*)(void*)this);
    u8 weight = 0xFF;
    u32 path_id = (mParameters >> 16) & 0xFF;
    if (path_id != 0xFF) {
        mPathRun.setInf(path_id, fopAcM_GetRoomNo(this), TRUE);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &=
            ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        weight = 0xD9;
        set_pthPoint(0);
    }
    if (mPathRun.mPath.get() == nullptr) {
        return false;
    }
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.flags = 0xA;                               /* attention_info.flags */
    mBckNum = 8;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9; /* attention_info.distances[TALK] */
    gravity = -4.5f;
    bool init_result;
    switch ((u32)(s32)mSubType) {
    case 0:
        init_result = init_PF1_0();
        break;
    default:
        return false;
    }
    if (!init_result) {
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
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = Pf1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Pf1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 022C9378 */
void daNpc_Pf1_c::play_animation() {

    u32 snd_id = 0;
    play_btp_anm();
    if (mObjAcch.m_flags & 0x20) { /* mObjAcch.ChkGroundHit() */
        snd_id = Pf1_materialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mbMorfAnimStopped = (s8)mpMorf->play(&eyePos, snd_id, (s8)reverb);
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = true;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 022C9470 */
void daNpc_Pf1_c::setMtx(u32 i_setEyePos) {

    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_setEyePos);
}

/* 022CA538 */
int daNpc_Pf1_c::bckResID(int i_bckNum) { return (s32)a_bck_res_tbl[i_bckNum]; }

/* 022C8BA8 */
int daNpc_Pf1_c::btpResID(int i_btpNum) { return (s32)a_btp_res_tbl[i_btpNum]; }

/* 022C8BBC */
u32 daNpc_Pf1_c::setBtp(s8 i_btpNum, u32 i_bModify) {

    J3DModel* morf_model_p = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (a_btp == nullptr) /* JUT_ASSERT(465, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_pf1.cpp", 0x1D1, "a_btp != (0)");
    mBtpNum = i_btpNum;
    mBtpFrame = i_btpNum == 1 ? 2 : 0;
    mBlinkTimer = 0;
    return Pf1_btpInit(mBtpAnm, morf_model_p->getModelData(), a_btp, TRUE, 0, 1.0f, 0, -1,
                       i_bModify, 0) != 0;
}

/* 022C8CB8 */
u32 daNpc_Pf1_c::init_texPttrnAnm(s8 i_btpNum, u32 i_bModify) {

    return setBtp(i_btpNum, i_bModify);
}

/* 022C92CC */
void daNpc_Pf1_c::play_btp_anm() {

    u8 frame_max = (u8)Pf1_frameMax(Pf1_view<Pf1Btp>(mBtpAnm)->mpAnm);
    if (mBtpNum == 1) {
        return;
    }
    if (mBtpNum == 0 && Pf1_calcTimer(&mBlinkTimer) != 0) {
        return;
    }
    u8 frame = (u8)(mBtpFrame + 1);
    mBtpFrame = frame;
    if (frame < frame_max) {
        return;
    }
    if (mBtpNum == 0) {
        mBlinkTimer = (s16)Pf1_random(60, 90);
        frame_max = 0;
    }
    mBtpFrame = frame_max;
}

/* 022CA54C */
void daNpc_Pf1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {

    s8 bck = i_anmPrmP->bckNum;
    if (bck < 0 || mBckNum == bck) {
        return;
    }
    int resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->loopMode;
    f32 morf = i_anmPrmP->morf;
    f32 speed = i_anmPrmP->speed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = false;
    mBckNum = i_anmPrmP->bckNum;
    mPrevMorfFrame = 0.0f;
    mAtrLoopCnt = 0;
}

/* 022CA5E4 */
void daNpc_Pf1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {

    anm_prm_c* anmTable = a_anm_num_tbl; /* [8] */
    if (i_setBtp) {
        init_texPttrnAnm(anmTable[i_anmNum].btpNum, true);
    }
    setAnm_anm(&anmTable[i_anmNum]);
}

/* 022CA654 */
void daNpc_Pf1_c::setAnm() {

    anm_prm_c* anmTable = a_anm_prm_tbl; /* [8] */
    init_texPttrnAnm(anmTable[mStt].btpNum, true);
    setAnm_anm(&anmTable[mStt]);
}

/* 022CA72C */
void daNpc_Pf1_c::chngAnmAtr(u8 i_atr) {

    if (i_atr == mAnmAtr || i_atr >= 6) {
        return;
    }
    mAnmAtr = i_atr;
    setAnm_ATR();
}

/* 022CA748 */
void daNpc_Pf1_c::ctrlAnmAtr() {

    if (mAnmAtr != 4 || !mbMorfAnimStopped) {
        return;
    }
    s8 cnt = (s8)(mAtrLoopCnt + 1);
    mAtrLoopCnt = cnt;
    if (cnt > 2) {
        mAnmAtr = 6;
        setAnm_NUM(0, 1);
    }
}

/* 022CA6C4 */
void daNpc_Pf1_c::setAnm_ATR() {

    anm_prm_c* anmTable = a_anm_atr_tbl; /* [6] */
    init_texPttrnAnm(anmTable[mAnmAtr].btpNum, true);
    setAnm_anm(&anmTable[mAnmAtr]);
}

/* 022CA78C */
void daNpc_Pf1_c::anmAtr(u16 i_msgStatus) {

    switch (i_msgStatus) {
    case 6: {
        if (mAtrSet == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeAttrInfo() */);
            mAtrSet = mAtrSet + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTag) {
            dComIfGp_clearMesgAnimeTagInfo();
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

/* 022CA84C */
u16 daNpc_Pf1_c::next_msgStatus(be<u32>* o_msgNoP) {

    u16 msg_status = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    Pf1Message* mng = Pf1_view<Pf1Message>(l_message.get());
    switch (*o_msgNoP) {
    case 0x1B59: {
        u32 sel = (u32)mng->mSelectedAnswer; /* the selected answer */
        if (sel < 1) {
            *o_msgNoP = 0x1B5A;
        } else if (sel == 1) {
            *o_msgNoP = 0x1B5D;
        }
        mbTalked = true;
        break;
    }
    case 0x1B5A:
        *o_msgNoP = 0x1B61;
        break;
    case 0x1B5B:
        *o_msgNoP = 0x1B62;
        break;
    case 0x1B5C:
        *o_msgNoP = 0x1B5F;
        break;
    case 0x1B5E:
        *o_msgNoP = 0x1B60;
        break;
    case 0x1B60:
        mbMessageEnded = true;
        msg_status = fopMsgStts_MSG_ENDS_e;
        break;
    case 0x1B61:
        *o_msgNoP = 0x1B5B;
        break;
    case 0x1B62:
        dComIfGs_onEventBit(0x0B04);
        msg_status = fopMsgStts_MSG_ENDS_e;
        break;
    default:
        msg_status = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 022CA970 */
u32 daNpc_Pf1_c::getMsg_PF1_0() {

    if (dComIfGs_isEventBit(0x0B04)) {
        return 0x1B5C;
    }
    return mbTalked ? 0x1B5E : 0x1B59;
}

/* 022CA9E0 */
u32 daNpc_Pf1_c::getMsg() {

    u32 msg = 0;
    if (mSubType == 0) {
        msg = getMsg_PF1_0();
    }
    return msg;
}

/* 022CA034 */
void daNpc_Pf1_c::eventOrder() {

    s8 state = mEvtState;
    if (state == 1 || state == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e); /* dEvtCnd_CANTALK_e */
        if (mEvtState == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (state >= 3) {
        s16 idx = (s16)(state - 3);
        mEventIndex = idx;
        fopAcM_orderOtherEventId(this, mEventIDTbl[idx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 022C98F8 */
void daNpc_Pf1_c::checkOrder() {

    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        if (dComIfGp_evmng_startCheck(mEventIDTbl[mEventIndex]) && mEvtState >= 3) {
            mEvtState = 0;
            mMesgAnimeTag = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == dEvtCmd_INTALK_e) {
        if (mEvtState == 1 || mEvtState == 2) {
            mEvtState = 0;
            mbTalk = true;
        }
    }
}

/* 022CAA18 */
BOOL daNpc_Pf1_c::chk_talk() {

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

/* 022CAAB0 */
u8 daNpc_Pf1_c::chk_parts_notMov() {

    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
             mActorAngleY == current.angle.y);
}

/* 022C9D4C */
fopAc_ac_c* daNpc_Pf1_c::searchByID(fpc_ProcID i_id, be<s32>* o_notFound) {

    Pf1GuestWord actor;
    actor = 0;
    *o_notFound = 0;
    if (!Pf1_searchByID(i_id, &actor)) {
        *o_notFound = 1;
    }
    return Pf1_actorAt(actor);
}

/* 022C9DA0 */
void daNpc_Pf1_c::lookBack() {

    cXyz look_pos;
    look_pos.set(0.0f, 0.0f, 0.0f);
    s16 target_y = current.angle.y;
    mJointHeadY = m_jnt.mAngles[0][1];
    u8 head_only = mbHeadOnly;
    f32 srcX = current.pos.x;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    mActorAngleY = target_y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* look_pos_p = nullptr;
    switch ((u32)(s32)mLookMode) {
    case 1: {
        cXyz eye;
        Pf1_playerEyePos(&eye, -20.0f);
        mLookPos.copy(eye);
        look_pos.copy(eye);
        look_pos_p = &look_pos;
        break;
    }
    case 2:
        look_pos.copy(mLookPos);
        look_pos_p = &look_pos;
        break;
    case 3:
        target_y = mLookAngleY;
        break;
    case 4: {
        Pf1GuestInt not_found;
        fopAc_ac_c* actor = searchByID(mLookActorId, &not_found);
        if (actor != nullptr && not_found == 0) {
            mLookPos.copy(actor->current.pos);
            f32 x = mLookPos.x;
            f32 y = actor->eyePos.y;
            look_pos.x = x;
            look_pos.y = y;
            look_pos.z = mLookPos.z;
            mLookPos.y = y;
            look_pos_p = &look_pos;
        }
        break;
    }
    }
    cXyz cur_pos; /* passed by value: a copy */
    cur_pos.x = srcX;
    cur_pos.y = srcY;
    cur_pos.z = srcZ;
    Pf1_lookAtTarget(&m_jnt, &current.angle.y, look_pos_p, &cur_pos, target_y,
                     l_HIO.mPrm.mLookVelMax, head_only);
}

/* 022CAAF0 */
u8 daNpc_Pf1_c::chkAttention() {

    dAttention_c* attention = dComIfGp_getAttention();
    if (Pf1_lockonTruth(attention)) {
        return this == (void*)Pf1_lockonTarget(attention, 0);
    }
    return this == (void*)Pf1_actionTarget(attention, 0);
}

/* 022C941C */
void daNpc_Pf1_c::setAttention(u32 i_setEyePos) {

    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 y = mHeadPos.y + l_HIO.mPrm.mAttPosOffsetY;
    attPos->x = mHeadPos.x;
    attPos->y = y;
    attPos->z = mHeadPos.z;
    if (!mbSetEyePos && !i_setEyePos) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}

/* 022C9010 */
u8 daNpc_Pf1_c::decideType(int i_type) {

    if (mType > 0) {
        return true;
    }
    mType = 1;
    mSubType = 0;
    /* strcpy(mArcName, "Pf") (3-byte block copy) */
    for (int i = 0; i < 3; i++)
        Pf1_view<be<u8>>(mArcName)[i] = l_arcName[i];
    return mType != -1 && mSubType != -1;
}

/* 022C9C10 */
void daNpc_Pf1_c::privateCut(int i_staffIdx) {
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = (s8)dComIfGp_evmng_getMyActIdx(i_staffIdx, a_cut_tbl, 1, TRUE, 0);
    mActionIndex = idx;
    if (idx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 022C9BCC */
void daNpc_Pf1_c::endEvent() {

    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTag = 0xFF;
}

/* 022C9B8C */
s32 daNpc_Pf1_c::isEventEntry() {

    const char* name = Pf1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 022C9CA8 */
void daNpc_Pf1_c::event_proc(int i_staffIdx) {

    if (dComIfGp_evmng_endCheck(mEventIDTbl[mEventIndex])) {
        endEvent();
    } else if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 022C911C */
BOOL daNpc_Pf1_c::set_action(Pf1ActionFunc* i_newProcFunc, void* i_argsP) {

    Pf1ActionFunc* cur = &mCurrProcFunc;
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
    }
    if (cur->i != 0) {
        mActState = 9;
        Pf1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Pf1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 022CAB78 */
void daNpc_Pf1_c::setStt(s8 i_stt) {

    s8 prev = mStt;
    mStt = i_stt;
    switch ((u32)(s32)i_stt) {
    case 1:
    case 4:
    case 6:
    case 7:
        mEvtState = 0;
        speedF = 0.0f;
        break;
    case 2:
        mEvtState = 0;
        speedF = 0.0f;
        mAtrSet = 0;
        mMesgAnimeTag = 0xFF;
        mPrevStt = prev;
        mAnmAtr = 0xFF;
        break;
    case 3:
        mEvtState = 0;
        mTimer = (s16)Pf1_random(180, 90);
        break;
    case 5:
        mTimer = 0;
        mEvtState = 0;
        mAttkTimer = 30;
        break;
    }
    setAnm();
}

/* 022CAC64 */
void daNpc_Pf1_c::createTama(f32 i_param) {

    csXyz angle;
    csXyz_ct(&angle, 0, 0, 0);
    cXyz start;
    start.x = eyePos.x;
    start.y = eyePos.y + 15.0f;
    start.z = eyePos.z;
    cXyz eye;
    Pf1_playerEyePos(&eye, -20.0f);
    cXyz target;
    target.copy(eye);
    cXyz_mi(&target, &eye, &eyePos);
    cXyz xz;
    xz.x = eye.x;
    xz.y = 0.0f;
    xz.z = eye.z;
    std_sqrtf(PSVECSquareMag(&xz)); /* unused */
    angle.y = cLib_targetAngleY(&start, &target);
    angle.x = Pf1_targetAngleX(&start, &target);
    fopAc_ac_c* tama =
        Pf1_fastCreate(0x1D0, 0, &eyePos, fopAcM_GetRoomNo(this), &angle, nullptr, -1, 0, nullptr);
    if (tama != nullptr) {
        Pf1_view<Pf1Tama>(tama)->mThrowParam = i_param;
        Pf1_view<Pf1Tama>(tama)->mParentID = mBsPcId; /* fopAcM_GetID(this) */
        tama->speedF = 50.0f;
    }
}

/* 022CADB8 */
BOOL daNpc_Pf1_c::chk_areaIN(f32 i_dist, cXyz* i_pos) {

    cXyz diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, &diff, i_pos);
    cXyz xz;
    f32 dx = diff.x, dz = diff.z;
    xz.y = 0.0f;
    xz.x = dx;
    xz.z = dz;
    u8 in = std_sqrtf(PSVECSquareMag(&xz)) < i_dist;
    if (in && (u32)l_frameCounter % 3 == 0) {
        createTama(i_dist);
    }
    return in;
}

/* 022CAE8C */
BOOL daNpc_Pf1_c::endEvent_check() {

    if (mbTalked) {
        return !dComIfGs_isEventBit(0x0B04);
    }
    return false;
}

/* 022CAEE4 */
BOOL daNpc_Pf1_c::startEvent_check() {

    cXyz pos;
    pos.x = mHomePos.x;
    pos.y = mHomePos.y + 100.0f;
    pos.z = mHomePos.z;
    if (chk_areaIN(l_HIO.mPrm.mStartDist, &pos)) {
        if (std_sqrtf(Pf1_squareDistance(&current.pos, &dComIfGp_getLinkPlayer()->current.pos)) <
                210.0f ||
            Pf1_view<Pf1NpcContact>(this)->mContactState != 0) {
            return true;
        }
    }
    return false;
}

/* 022C9074 */
void daNpc_Pf1_c::set_pthPoint(u32 i_idx) {

    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    mPathRun.mIdx = i_idx;
    cXyz pnt;
    Pf1_pathPoint(&mPathRun, &pnt, i_idx);
    current.pos.copy(pnt);
    if (mPathRun.nextIdx()) {
        cXyz next;
        Pf1_pathPoint(&mPathRun, &next, mPathRun.mIdx);
        cXyz next_copy;
        next_copy.copy(next);
        current.angle.y = cLib_targetAngleY(&current.pos, &next_copy);
    }
}

/* 022CAF98 */
BOOL daNpc_Pf1_c::chk_attn() {

    cXyz diff;
    cXyz_mi(&current.pos, &diff, &dComIfGp_getLinkPlayer()->current.pos);
    cXyz xz;
    f32 dx = diff.x, dz = diff.z;
    xz.y = 0.0f;
    xz.x = dx;
    xz.z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    f32 dy = current.pos.y - player->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos) -
                      current.angle.y);
    f32 max_angle = mLookMode == 1 ? 90.0f : 60.0f;
    u8 ret = false;
    if (dist < 200.0f) {
        if ((f32)abs((int)angle) / 182.04445f < max_angle && std::fabs(dy) < 300.0f) {
            ret = true;
        }
    }
    return ret;
}

/* 022CB1F4 */
void daNpc_Pf1_c::setBikon(cXyz* i_offset) {

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    cXyz pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), i_offset, &pos);
    dPa_control_set(dComIfGp_getParticle(), 0, 0x8152, &pos, nullptr, nullptr, 0xFF, nullptr, -1,
                    nullptr, nullptr, nullptr);
}

/* 022CB2A4 */
BOOL daNpc_Pf1_c::regret() {

    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    if (!endEvent_check()) {
        mEvtState = 0;
        if (startEvent_check()) {
            setStt(5);
        }
    } else {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    if (mbMorfAnimStopped) {
        setStt(3);
    }
    return TRUE;
}

/* 022CB378 */
BOOL daNpc_Pf1_c::attk_1() {

    cXyz diff;
    cXyz_mi(&current.pos, &diff, &dComIfGp_getLinkPlayer()->current.pos);
    cXyz xz;
    f32 dx = diff.x, dz = diff.z;
    xz.y = 0.0f;
    xz.x = dx;
    xz.z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    if (mbTalk) {
        if (chk_talk()) {
            setStt(6);
            setStt(2);
        }
        return TRUE;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    if (mBckNum == 0) {
        if (mTimer == 0) {
            s16 ang = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
            cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x80);
            if (abs((s16)(current.angle.y - ang)) < 0x800) {
                Pf1_sound(0x58BD, &current.pos);
                cXyz offset;
                offset.x = 0.0f;
                offset.y = -40.0f;
                offset.z = 60.0f;
                setBikon(&offset);
                mTimer = 20;
            }
        } else if (Pf1_calcTimer(&mTimer) == 0) {
            setAnm_NUM(2, 1);
        }
        return TRUE;
    }
    if (mEvtState != 1 && dist < 210.0f) {
        mEvtState = 1;
    }
    if (Pf1_calcTimer(&mAttkTimer) == 0) {
        cXyz home;
        home.x = mHomePos.x;
        home.y = mHomePos.y;
        home.z = mHomePos.z;
        if (!chk_areaIN(l_HIO.mPrm.mAreaDist, &home)) {
            setStt(6);
            return TRUE;
        }
    }
    pf1_chase(this);
    return TRUE;
}

/* 022CB66C */
BOOL daNpc_Pf1_c::walk_1() {

    dPath* path = mPathRun.mPath;
    if (path == nullptr || !((u8)Pf1_view<Pf1Path>(path)->mClosed & 1)) {
        return TRUE;
    }
    cXyz pos;
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (mPathRun.chkPointPass(&pos, mPathRun.mbDir != 0)) {
        mPathRun.nextIdxAuto();
    }
    cXyz pnt;
    Pf1_pathPoint(&mPathRun, &pnt, mPathRun.mIdx);
    cXyz pnt_copy;
    pnt_copy.copy(pnt);
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(&current.pos, &pnt_copy), prm.mTurnScale,
                       prm.mTurnMaxStep, 0x80);
    f32 target = prm.mWalkSpeed;
    if (Pf1_calcTimer(&mTimer) == 0 || mbTalk) {
        target = 0.0f;
    }
    if (!endEvent_check() && startEvent_check()) {
        speedF = 0.0f;
        mTimer = 0;
        setStt(5);
        mLookMode = 0;
        mbHeadOnly = true;
        return TRUE;
    }
    cLib_chaseF(&speedF, target, prm.mWalkAccel);
    f32 rate = speedF * prm.mWalkAnmRate;
    s32 itarget = gabi::ftoi(target + 0.5f);
    mpMorf->setPlaySpeed(rate - 0.5f >= 0.0f ? rate : 0.5f);
    if (itarget == 0 && gabi::ftoi(speedF) == 0) {
        if (mbTalk) {
            if (chk_talk()) {
                setStt(2);
            }
            return TRUE;
        }
        setStt(4);
        return TRUE;
    }
    mEvtState = 0;
    if (endEvent_check()) {
        mEvtState = 2;
    }
    mLookMode = 0;
    mbHeadOnly = true;
    return TRUE;
}

/* 022CB8C8 */
BOOL daNpc_Pf1_c::wait_2() {

    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
        }
        return TRUE;
    }
    bool stay = false;
    if (dComIfGs_isEventBit(0x0B04)) {
        cXyz home;
        home.x = mHomePos.x;
        home.y = mHomePos.y;
        home.z = mHomePos.z;
        stay = chk_areaIN(l_HIO.mPrm.mAreaDist, &home) != 0;
    }
    if (!stay) {
        mPathRun.setNearPathIndx(&current.pos, 0.0f);
        setStt(3);
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? 1 : 0;
    return TRUE;
}

/* 022CB9D0 */
BOOL daNpc_Pf1_c::wait_3() {

    cXyz diff;
    cXyz_mi(&current.pos, &diff, &dComIfGp_getLinkPlayer()->current.pos);
    cXyz xz;
    xz.x = diff.x;
    xz.y = 0.0f;
    xz.z = diff.z;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    if (mbTalk) {
        if (chk_talk()) {
            setStt(2);
            mbFar = false;
        }
        return TRUE;
    }
    mbFar = dist > 300.0f;
    if (mbFar) {
        setStt(3);
        mLookMode = 0;
        mbHeadOnly = true;
        return TRUE;
    }
    mEvtState = 2;
    mbHeadOnly = true;
    mLookMode = chk_attn() ? 1 : 0;
    return TRUE;
}

/* 022CBAE0 */
BOOL daNpc_Pf1_c::talk_1() {

    BOOL res = chk_parts_notMov();
    s16 ang = cLib_targetAngleY(&current.pos, &dComIfGp_getLinkPlayer()->current.pos);
    cLib_addCalcAngleS(&current.angle.y, ang, 4, l_HIO.mPrm.mLookVelMax, 0x80);
    u16 status = talk(1);
    if (!mbHasMsg) {
        return TRUE;
    }
    if (status == 10 && mbMorfAnimStopped && mCurrMsgNo == 0x1B60) {
        Pf1_view<Pf1Message>(l_message.get())->mbWait = 1;
    }
    /* HD: the message status comes from the message manager */
    if (Pf1_messageStatus(l_message.get()) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        mItemNo = 0xFF;
        mbTalk = false;
        setStt(mPrevStt);
        endEvent();
    }
    return res;
}

/* 022CBBE8 */
BOOL daNpc_Pf1_c::wait_action1(void*) {

    switch ((u32)(s32)mActState) {
    case 0:
        setStt(3);
        mActState = mActState + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 1:
            mbSetEyePos = TRUE; /* wait_1(): returns TRUE */
            break;
        case 2:
            mbSetEyePos = talk_1();
            break;
        case 3:
            mbSetEyePos = walk_1();
            break;
        case 4:
            mbSetEyePos = regret();
            break;
        case 5:
            mbSetEyePos = attk_1();
            break;
        case 6:
            mbSetEyePos = wait_2();
            break;
        case 7:
            mbSetEyePos = wait_3();
            break;
        }
        break;
    }
    return TRUE;
}

// HD: compiler-generated initialization and deleting destructors.

/* 022C99D0 */
u8 daNpc_Pf1_c::demo() {

    u8 id = demoActorID;
    if (id == 0) {
        if (mbDemo) {
            mbDemo = false;
            return 0;
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
        u32 obj = (u32)l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demoObject;
        }
        demo_actor_p = Pf1_demoActor(obj, id);
    }
    J3DAnmTexPattern* btp = Pf1_view<Pf1Btp>(mBtpAnm)->mpAnm;
    if (btp != nullptr) {
        u8 frame_max = (u8)Pf1_frameMax(btp);
        u8 frame = (u8)(mBtpFrame + 1);
        mBtpFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor_p) {
        void* demo_btp_p = Pf1_demoBtp(demo_actor_p, mArcName); /* getP_BtpData */
        if (demo_btp_p) {
            Pf1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), demo_btp_p, TRUE, 0, 1.0f, 0,
                        -1, 1, 0);
            mBtpFrame = 0;
            mBtpNum = 2;
        }
    }
    Pf1_demoData(this, 0x6A, mpMorf.get(), mArcName, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mbDemo;
}

/* 022CA398 */
BOOL daNpc_Pf1_c::_draw() {

    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_info_p = morf_model_p->getModelData();
    if (mbWaitForDemo || mbNoDraw) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morf_model_info_p, mBtpFrame);
    mpMorf->entryDL();
    Pf1_view<Pf1ModelAnimation>(morf_model_info_p)->mpBtpAnimation = 0; /* mBtpAnm.remove() */
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x5B /* DSNAP_TYPE_NPC_PF1 */, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mPrm.mDebugDraw) {
        Pf1_initDebugColour(&l_debugGuard0, &l_debugColour0, &l_debugSource0);
        Pf1_initDebugColour(&l_debugGuard1, &l_debugColour1, &l_debugSource1);
        Pf1_initDebugColour(&l_debugGuard2, &l_debugColour2, &l_debugSource2);
        Pf1_initDebugColour(&l_debugGuard3, &l_debugColour3, &l_debugSource3);
    }
    return TRUE;
}

/* 022CA0A4 */
BOOL daNpc_Pf1_c::_execute() {

    if (!mbHomeSet) {
        mHomePos.copy(current.pos);
        mHomeAngle.x = current.angle.x;
        mHomeAngle.y = current.angle.y;
        mHomeAngle.z = current.angle.z;
        mbHomeSet = true;
    }
    daNpc_Pf1_HIO_c::hio_prm_c& prm = l_HIO.mPrm;
    m_jnt.setParam(prm.mMaxBackBoneX, prm.mMaxBackBoneY, prm.mMinBackBoneX, prm.mMinBackBoneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mbWaitForDemo && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (!demo()) {
        s32 staff_id;
        if (dComIfGp_event_runCheck() &&
            (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staff_id = isEventEntry()) >= 0) {
            event_proc(staff_id);
        } else {
            Pf1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        Pf1_view<Pf1NpcContact>(this)->mContactState = 0;
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        if (mObjAcch.GetGroundH() != -1000000000.0f) {
            void* pla =
                dBgS_GetTriPla(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
            if (pla != nullptr) {
                mGndNormal.copy(*Pf1_view<cXyz>(pla)); /* ->GetNP() */
            }
        }
        play_animation();
    } else {
        mbWaitForDemo = false;
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
    tevStr.mRoomNo = Pf1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Pf1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(false);
    if (!mbDemo) {
        setCollision(100.0f, 200.0f);
    }
    return TRUE;
}

/* 022C98A0 */
BOOL daNpc_Pf1_c::_delete() {

    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022C9748 */
cPhs_State daNpc_Pf1_c::_create() {

    /* fopAcM_ct(this, daNpc_Pf1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Pf1_npcCtor(this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = 0x10020C08;
            Pf1_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
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
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size_tbl[mType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, -20.0f, -80.0f, 100.0f, 180.0f, 140.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 022C8CBC */
BOOL daNpc_Pf1_c::bodyCreateHeap() {

    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 8);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(2069, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_pf1.cpp", 0x815, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Pf1_deleteMorf(morf);
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, false)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head");
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_pf1.cpp", 0x82B, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone1");
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_pf1.cpp", 0x82D, "m_bbone_jnt_num >= 0");
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer(m_hed_jnt_num)
        ->setCallBack(nodeCB_Head);
    mpMorf->getModel()
        ->getModelData()
        ->getJointNodePointer(m_bbone_jnt_num)
        ->setCallBack(nodeCB_BackBone);
    mpMorf->getModel()->setUserArea((uintptr_t)this);
    return TRUE;
}

/* 022C8F7C */
BOOL daNpc_Pf1_c::CreateHeap() {

    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 100.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 022C989C */
static cPhs_State daNpc_Pf1_Create(fopAc_ac_c* i_this) { return ((daNpc_Pf1_c*)i_this)->_create(); }

/* 022C98F4 */
static BOOL daNpc_Pf1_Delete(daNpc_Pf1_c* i_this) { return i_this->_delete(); }

/* 022CA394 */
static BOOL daNpc_Pf1_Execute(daNpc_Pf1_c* i_this) { return i_this->_execute(); }

/* 022CA52C */
static BOOL daNpc_Pf1_Draw(daNpc_Pf1_c* i_this) { return i_this->_draw(); }

/* 022CA530 */
static BOOL daNpc_Pf1_IsDelete(daNpc_Pf1_c*) { return TRUE; }

// HD: constructor allocation, initialization and deleting destructors.
/* 022CBD1C */
static daNpc_Pf1_HIO_c* daNpc_Pf1_HIO_c_ct(daNpc_Pf1_HIO_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_Pf1_HIO_c*)operator_new(0x4C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10020AAC;
    memcpy_g(&i_this->mPrm, &a_prm_tbl, 0x40); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->mEntryID = -1;
    return i_this;
}

/* 022CBD88 */
static void __sinit_d_a_npc_pf1_cpp() {

    sinit_header_statics(0x10468330, 0x101C4ED0);
    daNpc_Pf1_HIO_c_ct(&l_HIO); /* static daNpc_Pf1_HIO_c l_HIO */
}

/* 022CBE28 */
static void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022CBE3C */
static void daNpc_Pf1_c_dt(daNpc_Pf1_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Pf1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Pf1_view<Pf1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x10020A4C;
        Pf1_view<Pf1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x10020A5C;
        Pf1_acchDestroy(&i_this->mObjAcch, 0);
        Pf1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022CBED8 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 10020AF0 "d_a_npc_pf1.cpp" in 022C8BBC
str 10020B00 "a_btp != (0)" in 022C8BBC
str 10020B18 "d_a_npc_pf1.cpp" in 022C8CBC
str 10020B34 "a_mdl_dat != (0)" in 022C8CBC
str 10020B10 "head" in 022C8CBC
str 10020B48 "m_hed_jnt_num >= 0" in 022C8CBC
str 10020B28 "backbone1" in 022C8CBC
str 10020B5C "m_bbone_jnt_num >= 0" in 022C8CBC
str 10020B90 "Pf1" in 022C9578
str 10020ACC "d_demo.h" in 022C99D0
str 10020ABC "m_object != (0)" in 022C99D0
ext 02055B64 s16 Pf1_calcTimer(be<s16>*)
ext 021E1E78 s32 Pf1_random(s32,s32)
ext 025E789C s32 Pf1_btpInit(void*,J3DModelData*,void*,s32,s32,f32,s32,s32,u32,s32)
ext 024EDFCC bool Pf1_lockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* Pf1_lockonTarget(dAttention_c*,s32)
ext 024EE464 fopAc_ac_c* Pf1_actionTarget(dAttention_c*,s32)
ext 024EF130 s8 Pf1_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 Pf1_polyColour(dBgS*,cBgS_PolyInfo*)
ext 024EECAC u32 Pf1_materialSound(dBgS*,cBgS_PolyInfo*)
ext 0259D54C void Pf1_playerEyePos(cXyz*,f32)
ext 0259E2D4 void Pf1_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 025D54C4 BOOL Pf1_searchByID(fpc_ProcID,be<u32>*)
ext 0259E778 void Pf1_pathPoint(dNpc_PathRun_c*,cXyz*,u32)
ext 025E19CC void Pf1_sound(u32,cXyz*)
ext 025D5928 fopAc_ac_c* Pf1_fastCreate(s16,u32,cXyz*,s32,csXyz*,cXyz*,s8,u32,void*)
ext 025A1458 void Pf1_npcCtor(daNpc_Pf1_c*)
ext 025E7820 void Pf1_btpCtor(void*)
ext 02526E70 void* Pf1_demoActor(u32,u8)
ext 02527828 void* Pf1_demoBtp(void*,const char*)
ext 02527028 void Pf1_demoData(daNpc_Pf1_c*,u32,mDoExt_McaMorf*,const char*,s32,u32,u32,s32)
ext 02018034 void Pf1_circleDestroy(void*,s32)
ext 024EFD9C void Pf1_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Pf1_actorDestroy(daNpc_Pf1_c*,s32)
ext 025F795C u32 Pf1_messageStatus(void*)
ext 0200F974 s16 Pf1_targetAngleX(cXyz*,cXyz*)
ext 028E8DE8 f32 Pf1_squareDistance(const cXyz*,const cXyz*)
*/
