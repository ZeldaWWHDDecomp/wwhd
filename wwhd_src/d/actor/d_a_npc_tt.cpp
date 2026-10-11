/**
 * d_a_npc_tt.cpp (WWHD)
 * NPC - Tott (the dancer on Windfall Island)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_tt.cpp and d_a_npc_tt_anm.inc) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_tt.h"

static BOOL nodeCallBack_Tt(J3DNode*, int);
static BOOL CheckCreateHeap(fopAc_ac_c*);
static s16 daNpc_tt_XyCheckCB(void*, int);
static s16 daNpc_tt_XyEventCB(void*, int);
enum { fpcNm_NPC_TT_e = 0x16C };
const gc_tu_data gc_tu = {.safeString = 0x10022D5C};
static auto& l_msgId = gabi::object_at<be<u32>>(0x10468900);
static auto& l_message = gabi::object_at<gptr<TtMessage>>(0x101F4B5C);
static auto& l_bck_ix_tbl = gabi::object_at<be<s16>[32]>(0x10022DC4);
static auto& l_bas_ix_tbl = gabi::object_at<be<s16>[32]>(0x10022E04);
static auto& l_btp_ix_tbl = gabi::object_at<be<s32>[1]>(0x10022D58);
static auto& action_table = gabi::object_at<gptr<char>[6]>(0x101C68C8);
static auto& l_lineColour = gabi::object_at<be<u32>>(0x101C68C4);
static auto& l_debugRegisters = gabi::object_at<TtDebugRegisters>(0x1047B608);
static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
f32 daNpc_Tt_c::curMorf() { return mpMorf->mCurMorf; }
/* 022ED8B0 */
static s16 daNpc_tt_XyCheckCB(void* i_this, int i_itemBtn) {

    return ((daNpc_Tt_c*)i_this)->XyCheckCB(i_itemBtn);
}

/* 022ED870 */
s16 daNpc_Tt_c::XyCheckCB(int i_itemBtn) {

    return dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */ ? TRUE : FALSE;
}

/* 022ED8B4 */
static s16 daNpc_tt_XyEventCB(void* i_this, int) {

    daNpc_Tt_c* a_this = (daNpc_Tt_c*)i_this;
    s16 idx = dComIfGp_evmng_getEventIdx("TACT_TT10" /* "TACT_TT10" */, 0xFF);
    a_this->mEventIdx = idx;
    return idx;
}

/* 022ED8F8 */
static BOOL nodeCallBack_Tt(J3DNode* node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Tt_c* i_this = static_cast<daNpc_Tt_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_head_jnt_num) {
                cXyz temp;
                cXyz temp2;
                temp.x = 0.0f;
                temp.y = 0.0f;
                temp.z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(),
                           (s16) - (i_this->m_jnt.mAngles[0][0] + i_this->mHeadAnm.field_0x00.x));
                MtxPosition(&temp, &temp2);
                i_this->mAttnBasePos.copy(temp2); /* setAttentionBasePos( &temp2) */
                temp.x = 20.0f;
                temp.y = -20.0f;
                temp.z = 0.0f;
                MtxPosition(&temp, &temp2);
                i_this->mEyePos.copy(temp2);         /* setEyePos( &temp2) */
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_backbone_jnt_num) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            Tt_copyMatrix(model->getAnmMtx(jntNo),
                          calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* 022EF0D8 */
void daNpc_Tt_c::setAnmStatus() {

    switch ((s32)mState) {
    case 1:
    case 2:
        if (dComIfGs_isEventBit(0xC40 /* UNK_0C40 */)) {
            danceInit(0);
        } else {
            setAnm(4);
        }
        break;
    case 3:
        break;
    default:
        setAnm(0);
        break;
    }
}

/* 022EDB94 */
BOOL daNpc_Tt_c::initTexPatternAnm(u32 i_modify) {

    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    /* HD: l_btp_ix_tbl[0] (not indexed with mTexPatternIdx) */
    m_head_tex_pattern =
        (J3DAnmTexPattern*)dComIfG_getObjectRes("Tt" /* "Tt" */, (s32)l_btp_ix_tbl[0]);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(130, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_tt_anm.inc", 0x82, "m_head_tex_pattern != (0)");
    if (!Tt_btpInit(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1,
                    i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022EE3FC */
BOOL daNpc_Tt_c::init() {

    gravity = -30.0f;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB; /* attention_info.distances[SPEAK] */
    actor_status =
        (actor_status & ~0x3Fu) | 0x27; /* fopAcM_SetStatusMap(0x27) + OnStatus(SHOWMAP) */
    attention_info.flags =
        fopAc_Attn_ACTION_SPEAK_e |
        fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    switch ((s8)mType) {
    case 0: {
        /* setAction(&daNpc_Tt_c::wait_action, NULL) (inline) */
        TtActionFunc* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == TtWaitAction;
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = -1; /* ACTION_ENDING */
                Tt_callAction(this, cur, nullptr);
            }
            mActionStatus = 0; /* ACTION_STARTING */
            cur->d = 0;
            cur->i = -1;
            cur->f = TtWaitAction; /* wait_action */
            Tt_callDirect(this, cur->f);
        }
        break;
    }
    }
    mAttnBasePos.copy(current.pos);
    mEyePos.copy(current.pos);
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    Tt_copyMatrix(&Tt_view<TtModelMatrix>(model)->mBaseMatrix,
                  mDoMtx_stack_c::get()); /* model->setBaseTRMtx */
    ke_execute();
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    eventInfo.setXyCheckCB(daNpc_tt_XyCheckCB); /* eventInfo.setXyCheckCB(daNpc_tt_XyCheckCB) */
    eventInfo.setXyEventCB(daNpc_tt_XyEventCB); /* eventInfo.setXyEventCB(daNpc_tt_XyEventCB) */
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}

/* 022EE8E8 */
void daNpc_Tt_c::playTexPatternAnm() {

    if (Tt_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = Tt_frameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= frameMax0) {
            s32 frameMax = Tt_frameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = mBlinkFrame + 1;
        }
    }
}

/* 022EE9AC */
void daNpc_Tt_c::setAnm(int newAnmIdx) {

    if (mTexPatternIdx != 0) {
        mTexPatternIdx = 0;
        initTexPatternAnm(true);
    }
    s8 anmIdx = (s8)newAnmIdx;
    f32 morf = 8.0f;
    f32 speed = 1.0f;
    s32 loopMode = 2; /* J3DFrameCtrl::EMode_LOOP */
    if (newAnmIdx >= 7 && newAnmIdx <= 31) {
        morf = 7.0f;
        mSoundTimer = 7; /* (s16)morf */
    }
    if (newAnmIdx == 6) {
        speed = 0.9f;
    }
    if (newAnmIdx < 5) {
        clrFlag(0x40);
    } else {
        loopMode = 0; /* J3DFrameCtrl::EMode_NONE */
    }
    if (newAnmIdx == 2) {
        speed = 0.0f;
    }
    if (anmIdx != mCurrAnmIdx && anmIdx != -1) {
        mCurrAnmIdx = anmIdx;
        mAnmLoopCount = 0;
        mAnmTimer = 0.0f;
        Tt_setAnm(mpMorf, loopMode, morf, speed, (s16)l_bck_ix_tbl[anmIdx] /* l_bck_ix_tbl */,
                  (s16)l_bas_ix_tbl[anmIdx] /* l_bas_ix_tbl */, "Tt" /* "Tt" */);
    }
}

/* 022EF074 */
void daNpc_Tt_c::danceInit(int param_1) {

    if (param_1 != 0 || !chkFlag(0x40)) {
        mDanceStep = 0;
        mDanceStepTimer = 8;
        setAnm(5);
        setFlag(0x40);
    }
}

/* 022EFE70 */
BOOL daNpc_Tt_c::danceProc() {

    if (!chkFlag(0x40)) {
        return FALSE;
    }
    if (curMorf() < 1.0f) { /* mpMorf->isMorf() */
        return FALSE;
    }
    if (mDanceStep == 5 && mDanceStepTimer == 0x19) {
        mHeadAnm.swing_vertical_init(2, 0x1194, -1000, 1);
    }
    if (mDanceStepTimer != 0) {
        mDanceStepTimer = mDanceStepTimer - 1;
        return FALSE;
    }
    return danceNext();
}

/* 022EFCD0 */
BOOL daNpc_Tt_c::danceNext() {

    if (mDanceStep < 6) {
        mDanceStep = mDanceStep + 1;
    } else {
        mDanceStep = 0;
    }
    switch ((u32)mDanceStep) {
    case 0:
    case 2:
        setAnm(5);
        mDanceStepTimer = 8;
        break;
    case 1:
        setAnm(10);
        mDanceStepTimer = 8;
        break;
    case 3:
        setAnm(9);
        mDanceStepTimer = 8;
        break;
    case 4:
        setAnm(6);
        mDanceStepTimer = 58;
        break;
    case 5:
        if (chkFlag(0x100)) {
            clrFlag(0x100);
            fopAcM_seStart(this, 0x48A1 /* JA_SE_CV_TT_DANCE_FIN */, 0);
        }
        setAnm(8);
        mDanceStepTimer = 53;
        break;
    }
    return mDanceStep == 0;
}

/* 022EEBA0 */
void daNpc_Tt_c::eventOrder() {

    if (ChkOrder(1)) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(2)) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
    }
    if (ChkOrder(4)) {
        eventInfo_onCondition(this, dEvtCnd_CANTALKITEM_e);
    }
}

/* 022EEB10 */
void daNpc_Tt_c::checkOrder() {

    u16 cmd = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd != dEvtCmd_INDEMO_e) {
        if (cmd == dEvtCmd_INTALK_e && ChkOrder(7)) {
            if (dComIfGp_event_chkTalkXY()) {
                setFlag(0x4);
            } else {
                setFlag(0x2);
            }
            talkInit();
        }
    }
    ClrOrder();
}

/* 022EF190 */
u32 daNpc_Tt_c::next_msgStatus(be<u32>* pMsgNo) {

    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    switch ((u32)*pMsgNo) {
    case 0x22C5:
    case 0x22C7:
    case 0x22C8:
    case 0x22D5:
    case 0x22D6:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x22C6:
        *pMsgNo = 0x22D5;
        break;
    case 0x22D7:
        *pMsgNo = 0x22C7;
        break;
    default:
        msgStatus = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}

/* 022EF208 */
u32 daNpc_Tt_c::getMsg() {

    u32 msgNo;
    s8 state = mState;
    clrFlag(0x20);
    switch ((s32)state) {
    case 2:
        if (dComIfGs_isEventBit(0xC40 /* UNK_0C40 */)) {
            msgNo = 0x22CB;
        } else if (dComIfGs_isEventBit(0xB08 /* UNK_0B08 */)) {
            msgNo = 0x22CA;
            setFlag(0x20);
        } else {
            dComIfGs_onEventBit(0xB08);
            msgNo = 0x22C5;
            setFlag(0x20);
        }
        break;
    default:
        msgNo = mMsgNo;
        switch (msgNo) {
        case 0x5B3:
            dComIfGp_setMelodyNum(5);
            break;
        }
    }
    return msgNo;
}

/* 022EEC18 */
void daNpc_Tt_c::setCollision() {

    cXyz centerPos;
    centerPos.copy(current.pos);
    mCyl.SetC(&centerPos);
    mCyl.SetR(48.0f);
    mCyl.SetH(140.0f);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 022EEAFC */
void daNpc_Tt_c::talkInit() {

    mTalkState = TALK_INIT;
    mMsgAnmIdx = 0xFF;
}

/* 022EF334 */
void daNpc_Tt_c::msgAnm() {

    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (mMsgAnmIdx != attr) {
        mMsgAnmIdx = dComIfGp_getMesgAnimeAttrInfo();
        switch ((u32)mMsgAnmIdx) {
        case 0:
            setAnm(0);
            break;
        case 1:
            setAnm(1);
            break;
        case 2:
            setAnm(3);
            break;
        case 3:
            setAnm(4);
            break;
        case 4:
            danceInit(0);
            break;
        case 5:
            setAnm(2);
            break;
        }
    }
}

/* 022EF2E4 */
void daNpc_Tt_c::msgPushButton() {

    if ((s32)mCurrMsgNo != 0x22C6) {
        return;
    }
    mNoticeLinkTimer = 0xF;
}

/* 022EF2FC */
void daNpc_Tt_c::msgContinue() {

    switch ((u32)mCurrMsgNo) {
    case 0x22D5:
        if (mNoticeLinkTimer > 0) {
            mNoticeLinkTimer = mNoticeLinkTimer - 1;
        } else {
            l_message->mbSendOn = 1; /* fopMsgM_messageSendOn() */
        }
        break;
    }
}

/* 022EF464 */
u16 daNpc_Tt_c::talk() {

    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    TtMessage* mgr = l_message.get();
    u16 msgStatus = 0xFF;
    if (mTalkState == TALK_INIT) {
        l_msgId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = TALK_MSG_CREATE;
    } else if (mTalkState != TALK_FINISHED) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = Tt_messageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            msgAnm();
            switch ((u32)(s32)mTalkState) {
            case TALK_MSG_CREATE:
                mTalkState = TALK_ACTIVE; /* HD: no fopMsgM_SearchByID */
                break;
            case TALK_ACTIVE:
                msgStatus = Tt_messageStatus(mgr);
                if (msgStatus == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                    msgContinue();
                } else if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                    msgPushButton();
                    Tt_setMessageStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (Tt_messageStatus(mgr) == 0xF) {
                        Tt_messageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                    Tt_setMessageStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                    l_msgId = 0xFFFFFFFF;
                }
                break;
            }
        }
    }
    return msgStatus;
}

/* 022EFB54 */
void daNpc_Tt_c::demoInitChange() {

    void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "prm0" /* "prm0" */);
    if (a_intP == nullptr) { /* JUT_ASSERT(763, a_intP) */
        JUT_ASSERT_fail("d_a_npc_tt.cpp", 0x2FB, "a_intP");
        return;
    }
    mEventIdx = dComIfGp_evmng_getEventIdx("TACT_TT11" /* "TACT_TT11" */, 0xFF);
    setEvFlag(0x1);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    s16 ev = mEventIdx;
    Tt_orderChangeEvent(link, this, ev, 0, 0xFFFF);
}

/* 022F0070 */
bool daNpc_Tt_c::demoProcTact1() {

    /* daPy_lk_c::getTactTimerCancel(): virtual, HD vtable (+0xB4) slot 0x34 */
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    u32 cancel = Tt_tactTimerCancel(link);
    switch (cancel) {
    case 1: {
        mEventIdx = dComIfGp_evmng_getEventIdx("TACT_TT12" /* "TACT_TT12" */, 0xFF);
        setEvFlag(0x1);
        fopAc_ac_c* l = dComIfGp_getLinkPlayer();
        s16 ev = mEventIdx;
        Tt_orderChangeEvent(l, this, ev, 0, 0xFFFF);
        break;
    }
    case 2: {
        mEventIdx = dComIfGp_evmng_getEventIdx("TACT_TT13" /* "TACT_TT13" */, 0xFF);
        setEvFlag(0x1);
        fopAc_ac_c* l = dComIfGp_getLinkPlayer();
        s16 ev = mEventIdx;
        Tt_orderChangeEvent(l, this, ev, 0, 0xFFFF);
        break;
    }
    default:
        dComIfGs_onEventBit(0xC40 /* UNK_0C40 */);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    return TRUE;
}

/* 022EFAD8 */
void daNpc_Tt_c::demoInitPatten() {

    setEvFlag(0x4);
    mSoundTimer = 0;
    danceInit(0);
    be<s32>* cnt = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Cnt" /* "Cnt" */);
    if (cnt != nullptr) {
        field_0xE7A = (u8)*cnt;
    } else {
        field_0xE7A = 2;
    }
}

/* 022EFF38 */
bool daNpc_Tt_c::demoProcPatten() {

    if (field_0xE7A == 0) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        clrEvFlag(0x4);
    } else {
        if (danceProc()) {
            field_0xE7A = field_0xE7A - 1;
        }
        if (mSoundTimer > 0) {
            mSoundTimer = mSoundTimer - 1;
            if (mSoundTimer == 0) {
                switch ((s32)mCurrAnmIdx) {
                case 8:
                    fopAcM_seStart(this, 0x48A1 /* JA_SE_CV_TT_DANCE_FIN */, 0);
                    break;
                case 9:
                    fopAcM_seStart(this, 0x4902 /* JA_SE_CV_TT_DANCE_2 */, 0);
                    break;
                case 10:
                    fopAcM_seStart(this, 0x4901 /* JA_SE_CV_TT_DANCE_1 */, 0);
                    break;
                }
            }
        }
    }
    return FALSE;
}

/* 022EF9D8 */
void daNpc_Tt_c::demoInitSpeak() {

    talkInit();
    be<s32>* a_intP = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "MsgNo" /* "MsgNo" */);
    if (a_intP == nullptr) /* JUT_ASSERT(888, a_intP) */
        JUT_ASSERT_fail("d_a_npc_tt.cpp", 0x378, "a_intP");
    u32 msgNo = (u32)(s32)*a_intP;
    mMsgNo = msgNo;
    switch (msgNo) {
    case 0x22CE:
        if (dKy_daynight_check() == 1 /* dKy_TIME_NIGHT_e */) {
            mMsgNo = 0x22CF;
        }
        break;
    }
}

/* 022EFC24 */
bool daNpc_Tt_c::demoProcSpeak() {

    u16 temp = talk();
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp == 0xFE) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return false;
}

/* 022EFA74 */
void daNpc_Tt_c::demoInitWait() {

    be<s32>* pTimer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Timer" /* "Timer" */);
    if (pTimer != nullptr) {
        mTimer = (s16)*pTimer;
    } else {
        mTimer = 0;
    }
}

/* 022EFC78 */
bool daNpc_Tt_c::demoProcWait() {

    if (mTimer > 0) {
        mTimer = mTimer - 1;
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return FALSE;
}

/* 022EF87C */
void daNpc_Tt_c::demoInitCom() {

    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    be<s32>* hidePl = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "HIDE_PL" /* "HIDE_PL" */);
    if (hidePl != nullptr) {
        /* daPy_lk_c::onPlayerNoDraw() / offPlayerNoDraw(): bit 0x08000000 of the word at +0x3B8 */
        auto* player = Tt_view<TtPlayer>(pLink);
        if (*hidePl != 0) {
            player->mNoDrawFlags |= 0x08000000;
        } else {
            player->mNoDrawFlags &= ~0x08000000u;
        }
    }
    be<s32>* disp = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "DISP" /* "DISP" */);
    if (disp != nullptr) {
        if (*disp != 0) {
            clrEvFlag(0x2);
        } else {
            setEvFlag(0x2);
        }
    }
    be<s32>* anm = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Anm" /* "Anm" */);
    if (anm != nullptr) {
        setAnm(*anm & 0xFF);
    }
}

/* 022F0190 */
void daNpc_Tt_c::demoProcCom() {

    if (chkEvFlag(0x2)) {
        setFlag(0x8);
    }
}

/* 022EF830 */
s32 daNpc_Tt_c::getNowEventAction() {
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, action_table, 6, FALSE, 1);
}

/* 022F01AC */
bool daNpc_Tt_c::demoProc() {

    bool temp = false;
    if (chkEvFlag(0x1)) {
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Tt" /* "Tt" */, nullptr, 0);
        clrEvFlag(0x1);
    }
    s32 eventAction = getNowEventAction();
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        setFlag(0x80);
        switch (eventAction) {
        case 1:
            demoInitSpeak();
            break;
        case 0:
            demoInitWait();
            break;
        case 2:
            demoInitPatten();
            temp = true;
            break;
        case 3:
            demoInitChange();
            break;
        }
    }
    switch (eventAction) {
    case 1:
        temp = demoProcSpeak();
        break;
    case 0:
        demoProcWait();
        break;
    case 2:
        demoProcPatten();
        break;
    case 3:
        break;
    case 4:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 5:
        demoProcTact1();
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    demoProcCom();
    if (eventAction != 2) {
        danceProc();
    }
    return temp;
}

/* 022EF5C4 */
void daNpc_Tt_c::setAttention(bool param_1) {

    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    cXyz* att = &attention_info.position; /* attention_info.position */
    Tt_word(att->x, Tt_word(mAttnBasePos.x));
    att->y = mAttnBasePos.y + 65.0f;
    Tt_word(att->z, Tt_word(mAttnBasePos.z));
    eyePos.copy(mEyePos); /* eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z) */
}

/* 022EF618 */
void daNpc_Tt_c::lookBack() {

    cXyz eye; /* dNpc_playerEyePos result */
    cXyz temp2;
    cXyz temp;                  /* passed by value: a copy */
    u32 tx = 0, ty = 0, tz = 0; /* 0.0f, bit copies */
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    switch ((s32)mState) {
    case 1:
        desiredYRot = home.angle.y;
        m_jnt.mbTrn = desiredYRot != current.angle.y ? 1 : 0; /* setTrn() / clrTrn() */
        break;
    default:
        if (chkFlag(0x20)) {
            desiredYRot = home.angle.y;
            m_jnt.mbTrn = 0; /* clrTrn() */
        } else {
            m_jnt.mbTrn = 1; /* setTrn() */
            Tt_playerEyePos(&eye, -20.0f);
            temp2.copy(eye);
            dstPos = &temp2;
            tx = Tt_word(current.pos.x);
            tz = Tt_word(current.pos.z);
            ty = Tt_word(eyePos.y);
        }
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x800, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    Tt_word(temp.x, tx);
    Tt_word(temp.y, ty);
    Tt_word(temp.z, tz);
    Tt_lookAtTarget(&m_jnt, &current.angle.y, dstPos, &temp, desiredYRot, mMaxHeadTurnVelocity,
                    false);
    shape_angle.y = current.angle.y;
}

/* 022F03B4 */
bool daNpc_Tt_c::wait01() {

    if (chkFlag(0x4)) {
        mReturnToState = mState;
        mState = 3;
        setAnmStatus();
        mTimer = 0x14;
        setEvFlag(0x1);
        fopAcM_seStart(this, 0x8A7 /* JA_SE_PRE_TAKT */, 0);
        return demoProc();
    } else if (chkFlag(0x2)) {
        mReturnToState = mState;
        mState = 2;
        setAnmStatus();
    } else {
        SetOrder(0x2);
        if (!dComIfGs_isEventBit(0xC40 /* UNK_0C40 */) &&
            dComIfGs_isEventBit(0xB08 /* UNK_0B08 */)) {
            SetOrder(0x4);
        }
    }
    danceProc();
    return curMorf() < 1.0f; /* mpMorf->isMorf() */
}

/* 022F0514 */
bool daNpc_Tt_c::talk01() {

    u16 temp = talk();
    if (mCurrMsgNo == 0x22C6 && dComIfGp_checkMesgSendButton()) {
        clrFlag(0x20);
    }
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x2 | 0x4);
    }
    danceProc();
    return curMorf() < 1.0f; /* mpMorf->isMorf() */
}

/* 022F05CC */
bool daNpc_Tt_c::tact00() {

    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x2 | 0x4);
        return true;
    }
    return demoProc();
}

/* 022F0664 */
BOOL daNpc_Tt_c::wait_action(void*) {

    if (mActionStatus == ACTION_STARTING) {
        mState = 1;
        setAnmStatus();
        mActionStatus = mActionStatus + 1; /* ACTION_ONGOING */
    } else if (mActionStatus != ACTION_ENDING) {
        bool temp;
        clrFlag(0x8);
        switch ((s32)mState) {
        case 1:
            temp = wait01();
            break;
        case 2:
            temp = talk01();
            break;
        case 3:
            temp = tact00();
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}

/* 022EE0D8 */
void tt_ke_s::ke_control(f32 param_1) {

    cXyz temp;
    cXyz temp6;
    cXyz sum;
    f32 _065 = (f32)l_debugRegisters.mHairDamping + 0.65f; /* HD: + a debug register */
    temp.x = 0.0f;
    temp.y = 0.0f;
    temp.z = 7.875f;
    cXyz* temp2 = &field_0x00[1];
    cXyz* temp3 = &field_0x78[1];
    for (int i = 1; i < 10; i++, temp2++, temp3++) {
        cXyz* prev = temp2 - 1;
        f32 x4 = gabi::fadds_ppc(gabi::fsubs_ppc(temp2->x, prev->x), temp3->x);
        f32 temp7 =
            gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(temp2->y, temp3->y), -3.0f),
                            (f32)l_debugRegisters.mHairGravity); /* HD: + a debug register */
        f32 z4 = gabi::fadds_ppc(gabi::fsubs_ppc(temp2->z, prev->z), temp3->z);
        if (temp7 < param_1) {
            temp7 = param_1;
        }
        f32 y4 = gabi::fsubs_ppc(temp7, prev->y);
        Mtx34* m = calc_mtx();
        Tt_XrotS(m, (s16)-cM_atan2s(y4, z4));
        m = calc_mtx();
        f32 len = std_sqrtf(gabi::fmadds(y4, y4, z4 * z4));
        mDoMtx_YrotM(m, cM_atan2s(x4, len));
        MtxPosition(&temp, &temp6);
        temp3->copy(*temp2);
        cXyz_pl(prev, &sum, &temp6);
        temp2->copy(sum);
        temp3->x = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->x, temp3->x), _065);
        temp3->y = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->y, temp3->y), _065);
        temp3->z = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->z, temp3->z), _065);
    }
}

/* 022EE2FC */
void tt_ke_s::ke_pos_set(cXyz* i_param) {

    for (int i = 0; i < 10; i_param++) {
        i_param->copy(field_0x00[i]);
        i++;
    }
}

/* 022EDF64 */
void daNpc_Tt_c::set_ke_root(int param_1, int param_2, int param_3) {

    cXyz temp;
    cXyz a; /* GHS reuses three result temporaries */
    cXyz b;
    cXyz c;
    temp.x = 0.0f;
    temp.y = 0.0f;
    temp.z = 0.0f;
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(param_1), calc_mtx());
    cXyz* k0 = &mLineKe[param_3].field_0x00[0];
    MtxPosition(&temp, k0);
    PSMTXCopy(mpMorf->getModel()->getAnmMtx(param_2), calc_mtx());
    cXyz* k3 = &mLineKe[param_3 + 3].field_0x00[0];
    MtxPosition(&temp, k3);
    cXyz_mi(k3, &a, k0);
    cXyz_ml(&a, &b, 0.3333f);
    cXyz_pl(&b, &c, k0);
    mLineKe[param_3 + 1].field_0x00[0].copy(c);
    cXyz_mi(k3, &c, k0);
    cXyz_ml(&c, &b, 0.6667f);
    cXyz_pl(&b, &a, k0);
    mLineKe[param_3 + 2].field_0x00[0].copy(a);
}

/* 022EE32C */
void daNpc_Tt_c::ke_execute() {

    mpMorf->calc();
    set_ke_root(6, 7, 0);
    set_ke_root(10, 11, 4);
    tt_ke_s* ke = mLineKe;
    f32 y = current.pos.y + 3.0f;
    for (int i = 0; i < 8; i++, ke++) {
        ke->ke_control(y);
        ke->ke_pos_set(Tt_linePosition(&mLineMat, i));
    }
}

/* 022EEF68 */
BOOL daNpc_Tt_c::_draw() {

    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = pModel->getModelData();
    if (chkFlag(0x8)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pModelData, mBlinkFrame);
    mpMorf->entryDL();
    Tt_view<TtModelAnimation>(pModelData)->mpBtpAnimation = 0; /* mBtpAnm.remove(pModelData) */
    /* HD: no dComIfGd_setShadow */
    Tt_lineUpdate(&mLineMat, 10, 0.8f, &l_lineColour, 0, &tevStr);
    Tt_sortLine(Tt_linePacket(&mLineMat), &mLineMat);
    dSnap_RegistFig(0x57 /* DSNAP_TYPE_NPC_TT */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022EECA0 */
BOOL daNpc_Tt_c::_execute() {

    m_jnt.setParam(0, 0x1C70, 0, -0x1C70, 0x1FFE, 9000, -0x1FFE, -9000, 0x1000);
    playTexPatternAnm();
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((s32)mCurrAnmIdx) {
        case 3:
            if (mAnmLoopCount > 0) {
                setAnm(0);
            } else {
                mAnmLoopCount = mAnmLoopCount + 1;
            }
            break;
        }
    }
    mHeadAnm.move();
    checkOrder();
    Tt_callAction(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    mMtrlSndId = 0;
    cBgS_PolyInfo* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* mObjAcch.m_gnd */
    if (!mObjAcch.ChkGroundHit()) {
        mMtrlSndId = Tt_materialSound(dComIfG_Bgsp(), gnd);
    }
    tevStr.mRoomNo = Tt_roomId(dComIfG_Bgsp(), gnd);
    tevStr.mEnvrIdxOverride = Tt_polyColour(dComIfG_Bgsp(), gnd); /* tevStr.mEnvrIdxOverride */
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    Tt_copyMatrix(&Tt_view<TtModelMatrix>(pModel)->mBaseMatrix,
                  mDoMtx_stack_c::get()); /* pModel->setBaseTRMtx */
    setCollision();
    ke_execute();
    return TRUE;
}

/* 022EE898 */
BOOL daNpc_Tt_c::_delete() {

    dComIfG_resDelete(&mPhs, "Tt" /* "Tt" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022EDF60 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Tt_c*)i_this)->CreateHeap(); }

/* 022EE674 */
cPhs_State daNpc_Tt_c::_create() {

    /* fopAcM_ct(this, daNpc_Tt_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Tt_actorCtor(this); /* fopAc_ac_c::fopAc_ac_c */
            /* dNpc_HeadAnm_c::dNpc_HeadAnm_c() (inline) */
            mHeadAnm.field_0x14 = 0.0f;
            mHeadAnm.field_0x18 = 0.0f;
            mHeadAnm.field_0x1C = 0;
            mHeadAnm.field_0x1E = 0;
            mHeadAnm.field_0x20 = 0;
            mHeadAnm.field_0x00.x = 0;
            mHeadAnm.field_0x00.y = 0;
            mHeadAnm.field_0x00.z = 0;
            __vtbl = 0x10022DB4;
            Tt_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x10022D84, 0x10022DA4, 0x10022D94});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10022D74);
            Tt_jointCtor(&m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            Tt_lineCtor(&mLineMat);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Tt" /* "Tt" */);
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_TT_e:
            mType = 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xE880)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}

/* 022EDC98 */
BOOL daNpc_Tt_c::CreateHeap() {

    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes("Tt" /* "Tt" */, 0x1C /* dRes_INDEX_TT_BDL_TT_e */);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Tt", 0x19 /* BCK_WAIT01 */);
    void* bas = dComIfG_getObjectRes("Tt", 0xE /* BAS_WAIT01 */);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, bas, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = (s8)modelData->getJointName()->getIndex("head");
    m_head_jnt_num = head;
    if (head < 0) /* JUT_ASSERT(1718, m_head_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_tt.cpp", 0x6B6, "m_head_jnt_num >= 0");
    s8 backbone = (s8)modelData->getJointName()->getIndex("backbone1");
    m_backbone_jnt_num = backbone;
    if (backbone < 0) /* JUT_ASSERT(1720, m_backbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_tt.cpp", 0x6B8, "m_backbone_jnt_num >= 0");
    switch ((s8)mType) {
    case 0:
        mTexPatternIdx = 0;
        break;
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Tt) */
            mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
                nodeCallBack_Tt);
        }
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this);
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return Tt_lineInit(&mLineMat, 8, 10, 0) == FALSE ? FALSE : TRUE;
}

/* 022EE894 */
static cPhs_State daNpc_Tt_Create(fopAc_ac_c* i_this) { return ((daNpc_Tt_c*)i_this)->_create(); }

/* 022EE8E4 */
static BOOL daNpc_Tt_Delete(daNpc_Tt_c* i_this) { return i_this->_delete(); }

/* 022EEF64 */
static BOOL daNpc_Tt_Execute(daNpc_Tt_c* i_this) { return i_this->_execute(); }

/* 022EF068 */
static BOOL daNpc_Tt_Draw(daNpc_Tt_c* i_this) { return i_this->_draw(); }

/* 022EF06C */
static BOOL daNpc_Tt_IsDelete(daNpc_Tt_c*) { return TRUE; }

// HD: compiler-generated static initialization and deleting destructors.
/* 022F0788 */
static void __sinit_d_a_npc_tt_cpp() { sinit_header_statics(0x10468904, 0x101C68E0); }

/* 022F081C */
static void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022F0830 */
static void daNpc_Tt_c_dt(daNpc_Tt_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        Tt_lineDestroy(&i_this->mLineMat, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Tt_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        Tt_view<TtAcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x10022D94;
        Tt_view<TtAcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x10022DA4;
        Tt_acchDestroy(&i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        Tt_actorDestroy(i_this, 0);           /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022F08D8 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 10022D48 "TACT_TT10" in 022ED8B4
str 10022E54 "Tt" in 022EDB94
str 10022E74 "d_a_npc_tt_anm.inc" in 022EDB94
str 10022E58 "m_head_tex_pattern != (0)" in 022EDB94
str 10022E8C "Tt" in 022EDC98
str 10022E90 "head" in 022EDC98
str 10022ED0 "d_a_npc_tt.cpp" in 022EDC98
str 10022EB0 "m_head_jnt_num >= 0" in 022EDC98
str 10022EC4 "backbone1" in 022EDC98
str 10022E98 "m_backbone_jnt_num >= 0" in 022EDC98
str 10022EFC "Tt" in 022EE674
str 10022EFF "Tt" in 022EE898
str 10022F14 "Tt" in 022EE9AC
str 10022F58 "HIDE_PL" in 022EF87C
str 10022F64 "DISP" in 022EF87C
str 10022F60 "Anm" in 022EF87C
str 10022F6C "MsgNo" in 022EF9D8
str 10022F7C "d_a_npc_tt.cpp" in 022EF9D8
str 10022F74 "a_intP" in 022EF9D8
str 10022F8C "Timer" in 022EFA74
str 10022F94 "Cnt" in 022EFAD8
str 10022F98 "prm0" in 022EFB54
str 10022FB4 "d_a_npc_tt.cpp" in 022EFB54
str 10022FA0 "a_intP" in 022EFB54
str 10022FA8 "TACT_TT11" in 022EFB54
str 10022FC4 "TACT_TT12" in 022F0070
str 10022FD0 "TACT_TT13" in 022F0070
str 10022FDC "Tt" in 022F01AC
ext 025D7970 BOOL Tt_orderChangeEvent(fopAc_ac_c*,fopAc_ac_c*,s16,u16,u16)
ext 025F795C u16 Tt_messageStatus(void*)
ext 025F74D0 void Tt_setMessageStatus(void*,u32)
ext 025F7DB0 u32 Tt_messageSet(void*,u32,cXyz*)
ext 0259D454 BOOL Tt_setAnm(mDoExt_McaMorf*,s32,f32,f32,s32,s32,const char*)
ext 02055B64 s16 Tt_calcTimer(be<s16>*)
ext 025E789C s32 Tt_btpInit(void*,J3DModelData*,J3DAnmTexPattern*,s32,s32,f32,s32,s32,u32,s32)
ext 0259D54C void Tt_playerEyePos(cXyz*,f32)
ext 0259DED0 void Tt_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 024EF130 s8 Tt_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 Tt_polyColour(dBgS*,cBgS_PolyInfo*)
ext 024EECAC u32 Tt_materialSound(dBgS*,cBgS_PolyInfo*)
ext 025F18EC void Tt_XrotS(Mtx34*,s16)
ext 025E9960 void Tt_lineCtor(mDoExt_3DlineMat0_tt*)
ext 025E99E0 void Tt_lineDestroy(mDoExt_3DlineMat0_tt*,s32)
ext 025E9B80 BOOL Tt_lineInit(mDoExt_3DlineMat0_tt*,u16,u16,BOOL)
ext 025EA548 void Tt_lineUpdate(mDoExt_3DlineMat0_tt*,u16,f32,void*,u16,dKy_tevstr_c*)
ext 025EDD04 void Tt_sortLine(void*,mDoExt_3DlineMat0_tt*)
ext 025D4ED0 void Tt_actorCtor(daNpc_Tt_c*)
ext 025E7820 void Tt_btpCtor(void*)
ext 0259DAA0 void Tt_jointCtor(dNpc_JntCtrl_c*)
ext 02018034 void Tt_circleDestroy(void*,s32)
ext 024EFD9C void Tt_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Tt_actorDestroy(daNpc_Tt_c*,s32)
*/
