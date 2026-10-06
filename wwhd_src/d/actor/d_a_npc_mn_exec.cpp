/**
 * d_a_npc_mn_exec.cpp (WWHD)
 * NPC - Manny: execute, move procs, events, messages.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_mn.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_mn.h"

enum : u32 {
    MN_l_execute_init = 0x101C20E4, /* ExecuteInit_t[5] (GHS pointers to member, 8 bytes) */
    MN_moveProc = 0x101C210C,       /* MoveProc_t[5] */
    MN_l_npc_anm_wait2 = 0x101C2057,
    MN_l_npc_anm_walk = 0x101C2060,
    MN_l_npc_anm_bikkuri = 0x101C2044,
    MN_l_npc_anm_jump1 = 0x101C204C,
    MN_l_msg_mn_figure = 0x101C2094,
};

/* 0229CF58 */
void daNpcMn_c::executeSetMode(u32 i_initIdx) {
    WWHD_FUNC(0x0229CF58, void, this, i_initIdx);
    mTargetSpeedF = 0.0f;
    speedF = 0.0f;
    mMoveState = mn_pmf_call0(this, MN_l_execute_init + i_initIdx * 8); /* (this->*l_execute_init[i])() */
}
VERIFY(0x0229CF58, &daNpcMn_c::executeSetMode);

/* 0229CFE4 */
void daNpcMn_c::checkOrder() {
    WWHD_FUNC(0x0229CFE4, void, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mHatchEventIdx) && mEvtOrderType == 3) {
            mEvtOrderType = 0;
        }
    } else if (command == 1 /* checkCommandTalk() */ && (mEvtOrderType == 2 || mEvtOrderType == 1)) {
        mTalkOrder = 1;
        executeSetMode(MOVE_PROC_TALK);
    }
}
VERIFY(0x0229CFE4, &daNpcMn_c::checkOrder);

/* 0229D094 */
BOOL daNpcMn_c::chkEndEvent() {
    WWHD_FUNC(0x0229D094, BOOL, this);
    if (dComIfGp_evmng_endCheck(mHatchEventIdx)) {
        dComIfGp_event_onEventFlag(8);
        fopAcM_delete(this);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0229D094, &daNpcMn_c::chkEndEvent);

/* 0229D118 (unnamed by the matcher) */
void daNpcMn_c::setMessage(u32 i_message) {
    WWHD_FUNC(0x0229D118, void, this, i_message);
    mCurrMsgNo = i_message;
}
VERIFY(0x0229D118, &daNpcMn_c::setMessage);

/* 0229D120 */
void daNpcMn_c::eventMesSetInit(int i_staffIdx) {
    WWHD_FUNC(0x0229D120, void, this, i_staffIdx);
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E81C) /* "MsgNo" */);
    if (pData != nullptr) {
        mpMsgNo = nullptr;
        u32 msg = *pData;
        if (msg == 1) {
            return;
        }
        if (msg == 0) {
            msg = mn_v_getMsg(this); /* HD: virtual */
        }
        setMessage(msg);
        if (mpMsgNo.get() != nullptr) {
            setMessage(*mpMsgNo.get());
        }
    } else {
        mpMsgNo = gabi::at<be<u32>>(gabi::ea(mpMsgNo.get()) + 4);
        setMessage(*mpMsgNo.get());
    }
}
VERIFY(0x0229D120, &daNpcMn_c::eventMesSetInit);

/* 0229D1DC */
void daNpcMn_c::eventGetItemInit() {
    WWHD_FUNC(0x0229D1DC, void, this);
    u32 procItem = fopAcM_createItemForPresentDemo(&current.pos, mItemNo, 0, -1, -1, nullptr, nullptr);
    if (procItem != fpcM_ERROR_PROCESS_ID_e) {
        gabi::store<u32>(dComIfGp_ea() + 0x52A0, procItem); /* dComIfGp_event_setItemPartnerId */
    }
}
VERIFY(0x0229D1DC, &daNpcMn_c::eventGetItemInit);

/* 0229D234 (unnamed by the matcher) */
void daNpcMn_c::eventWaitInit(int i_staffIdx) {
    WWHD_FUNC(0x0229D234, void, this, i_staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E824) /* "Timer" */);
    mEventTimer = 0;
    if (timer != nullptr) {
        mEventTimer = (s16)(s32)*timer;
    }
}
VERIFY(0x0229D234, &daNpcMn_c::eventWaitInit);

/* 0229D29C */
void daNpcMn_c::eventHatchInit() {
    WWHD_FUNC(0x0229D29C, void, this);
    fopAc_ac_c* hatch = fopAcM_searchFromName(STR(0x1001E82C) /* "Ohatch" */, 0, 0);
    if (hatch != nullptr) {
        gabi::Local<cXyz> a;
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        gabi::Local<cXyz> b;
        b->x = hatch->current.pos.x;
        b->y = hatch->current.pos.y;
        b->z = hatch->current.pos.z;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
        mHomeYRot = *angle;
    }
}
VERIFY(0x0229D29C, &daNpcMn_c::eventHatchInit);

/* 0229D32C */
void daNpcMn_c::eventBikkuriInit(int i_staffIdx) {
    WWHD_FUNC(0x0229D32C, void, this, i_staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E834) /* "Timer" */);
    mEventTimer = 1;
    if (timer != nullptr) {
        s16 t = (s16)(s32)*timer;
        if (t > 0) {
            mEventTimer = t;
        } else {
            mEventTimer = 1;
        }
    }
}
VERIFY(0x0229D32C, &daNpcMn_c::eventBikkuriInit);

/* 0229D3BC (unnamed by the matcher) */
u32 daNpcMn_c::eventTurnInit() {
    WWHD_FUNC(0x0229D3BC, u32, this);
    return gabi::call<u32>(0x0229C4E4, this, gabi::at<sMnAnmDat>(MN_l_npc_anm_wait)); /* setAnmTbl */
}
VERIFY(0x0229D3BC, &daNpcMn_c::eventTurnInit);

/* 0229D3C8 (unnamed by the matcher) */
u32 daNpcMn_c::eventWalkInit() {
    WWHD_FUNC(0x0229D3C8, u32, this);
    return gabi::call<u32>(0x0229C4E4, this, gabi::at<sMnAnmDat>(MN_l_npc_anm_walk)); /* setAnmTbl */
}
VERIFY(0x0229D3C8, &daNpcMn_c::eventWalkInit);

/* 0229D3D4 (unnamed by the matcher) */
u32 daNpcMn_c::eventLookInit() {
    WWHD_FUNC(0x0229D3D4, u32, this);
    return gabi::call<u32>(0x0229C4E4, this, gabi::at<sMnAnmDat>(MN_l_npc_anm_jump1)); /* setAnmTbl */
}
VERIFY(0x0229D3D4, &daNpcMn_c::eventLookInit);

/* 0229D3E0 */
void daNpcMn_c::eventJumpInit(int i_staffIdx) {
    WWHD_FUNC(0x0229D3E0, void, this, i_staffIdx);
    be<f32>* speedX = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x1001E84C) /* "SpeedX" */);
    dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x1001E854) /* "SpeedY" */);
    dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x1001E844) /* "Gravity" */);
    /* HD: the GameCube values from SpeedY / Gravity are overwritten anyway, so only speedF depends
     * on the event data */
    if (speedX != nullptr) {
        speedF = *speedX;
    } else {
        speedF = 3.0f;
    }
    mTargetSpeedF = 40.0f;
    mJumpSpeedY = 25.0f;
    gravity = -1.0f;
}
VERIFY(0x0229D3E0, &daNpcMn_c::eventJumpInit);

/* 0229D4DC (unnamed by the matcher) */
void daNpcMn_c::eventSwOnInit(int i_staffIdx) {
    WWHD_FUNC(0x0229D4DC, void, this, i_staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E85C) /* "Timer" */);
    mEventTimer = 0;
    if (timer != nullptr) {
        mEventTimer = (s16)(s32)*timer;
    }
}
VERIFY(0x0229D4DC, &daNpcMn_c::eventSwOnInit);

/* 0229D544 */
u16 daNpcMn_c::talk2(int i_param) {
    WWHD_FUNC(0x0229D544, u16, this, i_param);
    /* HD: the message (GameCube mpCurrMsg) is the message manager, read once */
    u32 mgr = mn_msgManager();
    u16 status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (i_param == 1) {
            mCurrMsgNo = mn_v_getMsg(this);
        }
        mCurrMsgBsPcId = msgMng_messageSet(mgr, mCurrMsgNo, &eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) { /* HD: only when the message was set */
            mbHasMsg = 0;
            mLastMsgStatus = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)msgMng_getStatus(mgr);
        switch (status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            msgMng_setStatus(mgr, mn_v_next_msgStatus(this, &mCurrMsgNo));
            if (msgMng_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                msgMng_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        /* fopMsgStts_MSG_TYPING_e: chkMsg() is empty */
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            msgMng_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        mLastMsgStatus = status;
        mn_v_anmAtr(this, status);
    } else {
        /* HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag */
        mbHasMsg = 1;
    }
    return status;
}
VERIFY(0x0229D544, &daNpcMn_c::talk2);

/* 0229D6F8 */
bool daNpcMn_c::eventMesSet() {
    WWHD_FUNC(0x0229D6F8, bool, this);
    return talk2(0) == 0x12 /* fopMsgStts_BOX_CLOSED_e */;
}
VERIFY(0x0229D6F8, &daNpcMn_c::eventMesSet);

/* 0229D754 */
bool daNpcMn_c::eventWait(int i_staffIdx) {
    WWHD_FUNC(0x0229D754, bool, this, i_staffIdx);
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        return false;
    }
    if (dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E864) /* "SwOn" */) != nullptr) {
        u8 sw = getPrmSwitchBit2();
        dComIfGs_onSwitch(sw, home.roomNo);
    }
    return true;
}
VERIFY(0x0229D754, &daNpcMn_c::eventWait);

/* 0229D804 */
bool daNpcMn_c::eventHatch() {
    WWHD_FUNC(0x0229D804, bool, this);
    mTargetYRot = mHomeYRot;
    mHeadOnlyFollow = false;
    mLookMode = LOOK_MODE_TURN;
    m_jnt.mbTrn = 1; /* setTrn() */
    return mHomeYRot == current.angle.y;
}
VERIFY(0x0229D804, &daNpcMn_c::eventHatch);

/* 0229D838 */
bool daNpcMn_c::eventBikkuri() {
    WWHD_FUNC(0x0229D838, bool, this);
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        if (mEventTimer == 0) {
            setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_bikkuri));
        }
        return false;
    }
    return (mAnmFlag & 1) != 0;
}
VERIFY(0x0229D838, &daNpcMn_c::eventBikkuri);

/* turn towards pos: mTargetYRot, look mode, the result of eventTurn */
static inline bool mn_turnTo(daNpcMn_c* i_this, cXyz* pos) {
    gabi::Local<cXyz> a;
    a->x = i_this->current.pos.x;
    a->y = i_this->current.pos.y;
    a->z = i_this->current.pos.z;
    gabi::Local<cXyz> b;
    b->x = pos->x;
    b->y = pos->y;
    b->z = pos->z;
    gabi::Local<be<s16>> angle;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    s16 ang = *angle;
    i_this->mTargetYRot = ang;
    i_this->mHeadOnlyFollow = false;
    i_this->mLookMode = daNpcMn_c::LOOK_MODE_TURN;
    i_this->m_jnt.mbTrn = 1; /* setTrn() */
    return i_this->current.angle.y == ang;
}

/* 0229D8A8 */
bool daNpcMn_c::eventTurn(int i_staffIdx) {
    WWHD_FUNC(0x0229D8A8, bool, this, i_staffIdx);
    be<s32>* pData = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001E874) /* "TurnMode" */);
    if (pData != nullptr && *pData == 1) {
        fopAc_ac_c* hatch = fopAcM_searchFromName(STR(0x1001E86C) /* "Ohatch" */, 0, 0);
        if (hatch != nullptr) {
            return mn_turnTo(this, &hatch->current.pos);
        }
    }
    gabi::Local<cXyz> pos;
    dNpc_PathRun_getPoint(&mPathRun, pos.get(), mPathRun.mIdx);
    return mn_turnTo(this, pos.get());
}
VERIFY(0x0229D8A8, &daNpcMn_c::eventTurn);

/* 0229DA24 */
bool daNpcMn_c::eventWalk() {
    WWHD_FUNC(0x0229DA24, bool, this);
    gabi::Local<cXyz> cur;
    cur->x = current.pos.x;
    cur->y = current.pos.y;
    cur->z = current.pos.z;
    if (dNpc_PathRun_chkPointPass(&mPathRun, cur.get(), mPathRun.mbDir != 0)) {
        if (!dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            mTargetSpeedF = 0.0f;
            speedF = 0.0f;
            return true;
        }
    }
    gabi::Local<cXyz> point;
    dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
    gabi::Local<cXyz> b;
    b->x = point->x;
    b->y = point->y;
    b->z = point->z;
    gabi::Local<cXyz> a; /* the getPoint result's storage is reused */
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    gabi::Local<be<s16>> angle;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    MnNpcDat* dat = mn_npc_dat(mNpcNo);
    s16 ang = *angle;
    mHeadOnlyFollow = false;
    mTargetYRot = ang;
    mHomeYRot = ang;
    mLookMode = LOOK_MODE_TURN;
    mLookAtMaxVel = dat->mWalkLookAtMaxVel;
    m_jnt.mbTrn = 1;
    mTargetSpeedF = dat->mWalkSpeed;
    return false;
}
VERIFY(0x0229DA24, &daNpcMn_c::eventWalk);

/* 0229DB54 */
bool daNpcMn_c::eventLook() {
    WWHD_FUNC(0x0229DB54, bool, this);
    return !(mpMorf->getFrame() < 72.0f);
}
VERIFY(0x0229DB54, &daNpcMn_c::eventLook);

/* 0229DB78 */
bool daNpcMn_c::eventJump() {
    WWHD_FUNC(0x0229DB78, bool, this);
    f32 jump = mJumpSpeedY;
    f32 newJump = jump + gravity;
    f32 y = current.pos.y + jump;
    mJumpSpeedY = newJump;
    current.pos.y = y;
    if (newJump < 0.0f && (gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28) & 0x80) /* ChkGroundLanding() */) {
        speedF = 0.0f;
        mTargetSpeedF = 0.0f;
        return true;
    }
    return false;
}
VERIFY(0x0229DB78, &daNpcMn_c::eventJump);

/* 0229DBC8 */
bool daNpcMn_c::eventSwOn() {
    WWHD_FUNC(0x0229DBC8, bool, this);
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        return false;
    }
    u8 sw = getPrmSwitchBit2();
    dComIfGs_onSwitch(sw, home.roomNo);
    return true;
}
VERIFY(0x0229DBC8, &daNpcMn_c::eventSwOn);

/* 0229DC44 */
void daNpcMn_c::privateCut() {
    WWHD_FUNC(0x0229DC44, void, this);
    /* cut_name_tbl (.data 0x101C221C): "MES_SET", "GET_ITEM", "WAIT", "HATCH", "BIKKURI", "TURN",
     * "WALK", "LOOK", "JUMP", "SWON" */
    const char* staff = gabi::at<const char>(gabi::load<u32>(MN_l_npc_staff_id));
    int staffIdx = dComIfGp_evmng_getMyStaffId(staff, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    mActIdx = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101C221C, 10, TRUE, 0);
    if (mActIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch (mActIdx) {
        case 0: eventMesSetInit(staffIdx); break;
        case 1: eventGetItemInit(); break;
        case 2: eventWaitInit(staffIdx); break;
        case 3: eventHatchInit(); break;
        case 4: eventBikkuriInit(staffIdx); break;
        case 5: eventTurnInit(); break;
        case 6: eventWalkInit(); break;
        case 7: eventLookInit(); break;
        case 8: eventJumpInit(staffIdx); break;
        case 9: eventSwOnInit(staffIdx); break;
        }
    }
    bool shouldEnd;
    switch (mActIdx) {
    case 0: shouldEnd = eventMesSet(); break;
    case 2: shouldEnd = eventWait(staffIdx); break;
    case 3: shouldEnd = eventHatch(); break;
    case 4: shouldEnd = eventBikkuri(); break;
    case 5: shouldEnd = eventTurn(staffIdx); break;
    case 6: shouldEnd = eventWalk(); break;
    case 7: shouldEnd = eventLook(); break;
    case 8: shouldEnd = eventJump(); break;
    case 9: shouldEnd = eventSwOn(); break;
    default: shouldEnd = true; break;
    }
    if (shouldEnd) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}
VERIFY(0x0229DC44, &daNpcMn_c::privateCut);

/* 0229DF30 */
void daNpcMn_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x0229DF30, void, this);
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (attr <= 3) {
        /* l_npc_anm_wait, l_npc_anm_talk, l_npc_anm_talk2, l_npc_anm_wait2 (HD: a table at 0x1001E8D8) */
        setAnmTbl(gabi::at<sMnAnmDat>(gabi::load<u32>(0x1001E8D8 + attr * 4)));
    }
    dComIfGp_setMesgAnimeAttrInfo(0xFF);
}
VERIFY(0x0229DF30, &daNpcMn_c::setAnmFromMsgTag);

/* 0229DF8C */
void daNpcMn_c::eventMove() {
    WWHD_FUNC(0x0229DF8C, void, this);
    if (!chkEndEvent()) {
        u8 attn = mEventCut.mbAttention; /* getAttnFlag() */
        if (mEventCut.cutProc()) {
            if (!mEventCut.mbAttention) {
                mEventCut.mbAttention = attn;
            }
        } else {
            privateCut();
            setAnmFromMsgTag();
        }
    }
}
VERIFY(0x0229DF8C, &daNpcMn_c::eventMove);

/* 0229E014 */
void daNpcMn_c::eventOrder() {
    WWHD_FUNC(0x0229E014, void, this);
    if (mEvtOrderType == 2 || mEvtOrderType == 1) {
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (mEvtOrderType == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (mEvtOrderType == 3) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, mHatchEventIdx, 0, 0xFFFF);
    }
}
VERIFY(0x0229E014, &daNpcMn_c::eventOrder);

/* 0229E0AC */
void daNpcMn_c::playTexPatternAnm() {
    WWHD_FUNC(0x0229E0AC, void, this);
    if (cLib_calcTimer(&mBtpTimer) == 0) {
        s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mBtpFrame >= frameMax) {
            s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
            mBtpTimer = 0x78;
            mBtpFrame = mBtpFrame - max;
        } else {
            mBtpFrame = mBtpFrame + 1;
        }
    }
}
VERIFY(0x0229E0AC, &daNpcMn_c::playTexPatternAnm);

/* 0229E14C */
void daNpcMn_c::playAnm() {
    WWHD_FUNC(0x0229E14C, void, this);
    if (mpMorf->play(nullptr, 0, 0) && mpAnmDat.get() != nullptr && mAnmLoopCnt > 0) {
        mAnmLoopCnt = mAnmLoopCnt - 1;
        if (mAnmLoopCnt == 0) {
            mpAnmDat = gabi::at<sMnAnmDat>(gabi::ea(mpAnmDat.get()) + 3);
            if (setAnmTbl(mpAnmDat)) {
                mAnmFlag = mAnmFlag | 1;
            }
        } else {
            setAnm(mpAnmDat->mBckIdx, 0, 0.0f);
        }
    }
}
VERIFY(0x0229E14C, &daNpcMn_c::playAnm);

/* 0229E210 */
void daNpcMn_c::lookBack() {
    WWHD_FUNC(0x0229E210, void, this);
    s16 maxVel = mLookAtMaxVel;
    s16 desiredYRot = current.angle.y;
    cXyz* dstTemp = nullptr;
    gabi::Local<cXyz> temp2;
    /* cXyz dstPos = eyePos (through FPRs) */
    f32 eye_x = eyePos.x;
    f32 eye_y = eyePos.y;
    f32 eye_z = eyePos.z;
    u8 headOnlyFollow = mHeadOnlyFollow;
    switch ((u32)(s32)mLookMode) {
    case LOOK_MODE_ATTN:
        temp2->copy(mLookAtPos);
        dstTemp = temp2.get();
        break;
    case LOOK_MODE_TURN:
        desiredYRot = mTargetYRot;
        break;
    }
    if (mTalkOrder != 0 && mbAllowBodyTurn != 0) {
        headOnlyFollow = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        if (mEventCut.mTurnSpeed != 0) {
            maxVel = mEventCut.mTurnSpeed;
        }
        cLib_addCalcAngleS2(&mTurnVel, maxVel, 4, 0x800);
    } else {
        mTurnVel = 0;
    }
    gabi::Local<cXyz> dstPos;
    dstPos->x = eye_x;
    dstPos->y = eye_y;
    dstPos->z = eye_z;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstTemp, dstPos.get(), desiredYRot, mTurnVel, headOnlyFollow);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
VERIFY(0x0229E210, &daNpcMn_c::lookBack);

/* 0229E41C */
bool daNpcMn_c::_execute() {
    WWHD_FUNC(0x0229E41C, bool, this);
    chkAttention();
    checkOrder();
    if (!gabi::load<u8>(dComIfGp_ea() + 0x5292) /* !dComIfGp_event_runCheck() */ ||
        gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */ || mTalk3State != TALK3_INIT) {
        mn_pmf_call0(this, MN_moveProc + mMoveState * 8); /* (this->*moveProc[mMoveState])() */
    } else {
        eventMove();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    if (mBckIdx == BCK_WALK) {
        cLib_chaseF(&speedF, mTargetSpeedF, 0.3f);
        f32 speed = speedF * mn_npc_dat(mNpcNo)->mWalkAnmRate;
        if (speed < 0.5f) {
            speed = 0.5f;
        }
        mpMorf->setPlaySpeed(speed);
    } else {
        cLib_chaseF(&speedF, mTargetSpeedF, 0.1f);
    }
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), mn_npc_dat(mNpcNo)->mCylRadius, 150.0f);
    MnNpcDat* dat = mn_npc_dat(mNpcNo);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    attPos->z = z;
    attPos->x = x;
    attPos->y = y + dat->mAttnPosOfsY;
    eyePos.z = z;
    eyePos.x = x;
    eyePos.y = y + dat->mEyePosOfsY;
    lookBack();
    setMtx();
    return false;
}
VERIFY(0x0229E41C, &daNpcMn_c::_execute);

/* 0229E8A0 */
u8 daNpcMn_c::executeCommon() {
    WWHD_FUNC(0x0229E8A0, u8, this);
    mEvtOrderType = mbPlayerAttention != 0;
    if (mTalkOrder == 1 && mMoveState != MOVE_PROC_TALK) {
        executeSetMode(MOVE_PROC_TALK);
    }
    return mTalkOrder;
}
VERIFY(0x0229E8A0, &daNpcMn_c::executeCommon);

/* 0229E900 */
int daNpcMn_c::executeWaitInit() {
    WWHD_FUNC(0x0229E900, int, this);
    speedF = 0.0f;
    if (mbLookFigure) {
        setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_wait2));
        mWaitTimer = (s16)gabi::ftoi(cM_rndF(30.0f) + 150.0f);
    } else {
        setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_wait));
        MnNpcDat* dat = mn_npc_dat(mNpcNo);
        f32 r = cM_rndF((f32)(dat->mWaitTimerMax - dat->mWaitTimerMin));
        mWaitTimer = (s16)gabi::ftoi(r + (f32)mn_npc_dat(mNpcNo)->mWaitTimerMin);
    }
    return MOVE_PROC_WAIT;
}
VERIFY(0x0229E900, &daNpcMn_c::executeWaitInit);

/* 0229EA30 */
void daNpcMn_c::executeWait() {
    WWHD_FUNC(0x0229EA30, void, this);
    if (executeCommon()) {
        return;
    }
    if (mPosFlag == 0) {
        BOOL scope = mn_checkTelescopeLook();
        f32 attnDist = mn_npc_dat(mNpcNo)->mAttnDist;
        if (scope) {
            mAttnDist = (f32)(s16)gabi::ftoi(attnDist + attnDist);
        } else {
            mAttnDist = attnDist;
            mEtcFlag = mEtcFlag & 0xFFFE;
        }
        if (mbPlayerAttention && dComIfGs_isEventBit(0x2F08) && mn_checkTelescopeLook()) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BCF, 1); /* dComIfGp_setScopeType(dScpTyp_PICTO_BOX_e) */
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BB2) == 0xB /* dComIfGp_getMesgStatus() == fopMsgStts_SCOPE_ACTIVE_e */ &&
                !(mEtcFlag & 1)) {
                mEtcFlag = mEtcFlag | 1;
                executeSetMode(MOVE_PROC_TALK3);
            }
        }
        if (!(mSwitchFlag & 1) && fopAcM_isSwitch(this, getPrmSwitchBit())) {
            u8 sw = getPrmSwitchBit();
            dComIfGs_onSwitch(sw, home.roomNo);
            mSwitchFlag = mSwitchFlag | 1;
            mbAllowBodyTurn = 0;
            mbLookOnly = 0;
            mEvtOrderType = 3;
        }
    } else if (mbLookFigure) {
        if (mWaitTimer == 0) {
            mbLookFigure = 0;
            executeSetMode(MOVE_PROC_TURN);
        } else {
            mWaitTimer = mWaitTimer - 1;
            fopAc_ac_c* figure = fopAcM_searchFromName(STR(0x1001E8F4) /* "Figure" */, 0xFF, mFigureArg);
            if (figure != nullptr && gabi::load<u8>(gabi::ea(figure) + 0xA32) /* daObjFigure_c::mbDisplay */) {
                mLookAtPos.copy(figure->eyePos);
                m_jnt.mbTrn = 1; /* setTrn() */
                mLookMode = LOOK_MODE_ATTN;
                mHeadOnlyFollow = false;
            }
        }
    } else {
        if (mPathRun.mPath.get() != nullptr && mWaitTimer != 0 && !mbPlayerAttention && !mbNearPlayer) {
            mWaitTimer = mWaitTimer - 1;
            if (mWaitTimer == 0) {
                mbLookFigure = 0;
                executeSetMode(MOVE_PROC_TURN);
            }
        }
    }
}
VERIFY(0x0229EA30, &daNpcMn_c::executeWait);

/* 0229ECEC (unnamed by the matcher) */
int daNpcMn_c::executeTalkInit() {
    WWHD_FUNC(0x0229ECEC, int, this);
    return MOVE_PROC_TALK;
}
VERIFY(0x0229ECEC, &daNpcMn_c::executeTalkInit);

/* 0229ECF4 */
void daNpcMn_c::executeTalk() {
    WWHD_FUNC(0x0229ECF4, void, this);
    executeCommon();
    if (talk2(1) == 0x12) {
        mTalkOrder = 0;
        executeSetMode(MOVE_PROC_WAIT);
        dComIfGp_event_onEventFlag(8);
    } else {
        setAnmFromMsgTag();
    }
}
VERIFY(0x0229ECF4, &daNpcMn_c::executeTalk);

/* 0229ED6C (unnamed by the matcher) */
int daNpcMn_c::executeTalk3Init() {
    WWHD_FUNC(0x0229ED6C, int, this);
    mTalk3State = TALK3_INIT;
    return MOVE_PROC_TALK3;
}
VERIFY(0x0229ED6C, &daNpcMn_c::executeTalk3Init);

/* 0229ED7C */
u32 daNpcMn_c::getMsg3() {
    WWHD_FUNC(0x0229ED7C, u32, this);
    mpMsgNo = nullptr;
    return 0x35EF;
}
VERIFY(0x0229ED7C, &daNpcMn_c::getMsg3);

/* 0229ED8C */
u16 daNpcMn_c::talk3(int i_param) {
    WWHD_FUNC(0x0229ED8C, u16, this, i_param);
    u16 status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (i_param == 1) {
            mCurrMsgNo = getMsg3();
        }
        mCurrMsgBsPcId = msgMng_scopeMessageSet(mn_msgManager(), mCurrMsgNo);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
            mLastMsgStatus = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = dComIfGp_getScopeMesgStatus();
        switch (status) {
        case 0xE:
            dComIfGp_setScopeMesgStatus(mn_v_next_msgStatus(this, &mCurrMsgNo));
            if (dComIfGp_getScopeMesgStatus() == 0xF) {
                msgMng_scopeMessageSet(mn_msgManager(), mCurrMsgNo);
            }
            break;
        case 0x12:
            dComIfGp_setScopeMesgStatus(0x11 /* fopMsgStts_BOX_CLOSING_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        mLastMsgStatus = status;
        mn_v_anmAtr(this, status);
    } else {
        mbHasMsg = 1;
    }
    return status;
}
VERIFY(0x0229ED8C, &daNpcMn_c::talk3);

/* 0229EF28 */
void daNpcMn_c::executeTalk3() {
    WWHD_FUNC(0x0229EF28, void, this);
    switch (mTalk3State) {
    case TALK3_INIT:
    case TALK3_ORDER:
        if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */) {
            mTalk3State = TALK3_TALK;
        } else {
            fopAcM_orderPotentialEvent(this, 0xA /* dEvtFlag_STAFF_ALL_e | dEvtFlag_UNK8_e */, 0, 0);
            mTalk3State = TALK3_ORDER;
            u32 a = gabi::ea(this) + 0xFA;
            gabi::store<u16>(a, gabi::load<u16>(a) | 2); /* eventInfo.onCondition(dEvtCnd_UNK2_e) */
        }
        break;
    case TALK3_TALK:
        if (talk3(1) == 0x12) {
            mTalk3State = TALK3_INIT;
            executeSetMode(MOVE_PROC_WAIT);
            dComIfGp_event_onEventFlag(8);
        }
        break;
    }
}
VERIFY(0x0229EF28, &daNpcMn_c::executeTalk3);

/* 0229F00C */
int daNpcMn_c::executeWalkInit() {
    WWHD_FUNC(0x0229F00C, int, this);
    setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_walk));
    return MOVE_PROC_WALK;
}
VERIFY(0x0229F00C, &daNpcMn_c::executeWalkInit);

/* 0229F038 */
void daNpcMn_c::executeWalk() {
    WWHD_FUNC(0x0229F038, void, this);
    if (executeCommon()) {
        return;
    }
    bool reachedEnd = false;
    gabi::Local<cXyz> cur;
    cur->x = current.pos.x;
    cur->y = current.pos.y;
    cur->z = current.pos.z;
    if (dNpc_PathRun_chkPointPass(&mPathRun, cur.get(), mPathRun.mbDir != 0)) {
        mFigureArg = dNpc_PathRun_pointArg(&mPathRun, mPathRun.mIdx);
        if (mFigureArg != 0xFF) {
            fopAc_ac_c* figure = fopAcM_searchFromName(STR(0x1001E8FC) /* "Figure" */, 0xFF, mFigureArg);
            if (figure != nullptr && gabi::load<u8>(gabi::ea(figure) + 0xA32) /* isDispFigure() */) {
                mFigureMsgIdx = getRand(5);
                mbLookFigure = 1;
                executeSetMode(MOVE_PROC_WAIT);
            }
        }
        if (!dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            reachedEnd = true;
        }
    }
    if (mbPlayerAttention || mbNearPlayer) {
        mbLookFigure = 0;
        executeSetMode(MOVE_PROC_WAIT);
        return;
    }
    if (!reachedEnd) {
        if (mbLookFigure != 0) {
            return;
        }
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
        gabi::Local<cXyz> b;
        b->x = point->x;
        b->y = point->y;
        b->z = point->z;
        cXyz* a = point.get(); /* the getPoint result's storage is reused */
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(a, b.get(), nullptr, angle.get());
        MnNpcDat* dat = mn_npc_dat(mNpcNo);
        s16 ang = *angle;
        mHomeYRot = ang;
        mTargetYRot = ang;
        mHeadOnlyFollow = false;
        mLookMode = LOOK_MODE_TURN;
        mLookAtMaxVel = dat->mWalkLookAtMaxVel;
        m_jnt.mbTrn = 1;
        mTargetSpeedF = dat->mWalkSpeed;
    } else {
        mPathRun.mbDir = mPathRun.mbDir ^ 1; /* turnDir() */
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
        gabi::Local<cXyz> a;
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        gabi::Local<cXyz> b;
        b->x = point->x;
        b->y = point->y;
        b->z = point->z;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
        mHomeYRot = *angle;
        /* HD: no mPathRun.setInf(0xFF, roomNo, true) */
        executeSetMode(MOVE_PROC_WAIT);
    }
}
VERIFY(0x0229F038, &daNpcMn_c::executeWalk);

/* 0229F2B0 */
int daNpcMn_c::executeTurnInit() {
    WWHD_FUNC(0x0229F2B0, int, this);
    gabi::Local<cXyz> point;
    dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
    gabi::Local<cXyz> a;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    gabi::Local<cXyz> b;
    b->x = point->x;
    b->y = point->y;
    b->z = point->z;
    gabi::Local<be<s16>> angle;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    if (*angle == current.angle.y) {
        setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_walk));
        MnNpcDat* dat = mn_npc_dat(mNpcNo);
        f32 r = cM_rndF((f32)(dat->mTurnWaitMax - dat->mTurnWaitMin));
        mWaitTimer = (s16)gabi::ftoi(r + (f32)mn_npc_dat(mNpcNo)->mTurnWaitMin);
        return MOVE_PROC_WALK;
    }
    return MOVE_PROC_TURN;
}
VERIFY(0x0229F2B0, &daNpcMn_c::executeTurnInit);

/* 0229F410 */
void daNpcMn_c::executeTurn() {
    WWHD_FUNC(0x0229F410, void, this);
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> point;
    dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
    if (mn_turnTo(this, point.get())) {
        executeSetMode(MOVE_PROC_WALK);
    }
}
VERIFY(0x0229F410, &daNpcMn_c::executeTurn);

/* 0229F4D0 */
u16 daNpcMn_c::next_msgStatus(be<u32>* i_msgNo) {
    WWHD_FUNC(0x0229F4D0, u16, this, i_msgNo);
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    if (mpMsgNo.get() != nullptr) {
        mpMsgNo = gabi::at<be<u32>>(gabi::ea(mpMsgNo.get()) + 4);
        u32 msg = *mpMsgNo.get();
        if (msg == 0) {
            mpMsgNo = nullptr;
            ret = 0x10; /* fopNpc_npc_c::next_msgStatus: fopMsgStts_MSG_ENDS_e */
        } else {
            *i_msgNo = msg;
        }
    } else {
        ret = 0x10;
    }
    return ret;
}
VERIFY(0x0229F4D0, &daNpcMn_c::next_msgStatus);

/* 0229F518 */
u32 daNpcMn_c::getMsg() {
    WWHD_FUNC(0x0229F518, u32, this);
    u32 msg = 0;
    mpMsgNo = nullptr;
    if (!((u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3) /* !dComIfGp_event_chkTalkXY() */) {
        if (!mPosFlag) {
            if (!dComIfGs_isEventBit(0x2F08)) {
                dComIfGs_onEventBit(0x2F08);
                mpMsgNo = gabi::at<be<u32>>(0x101C2064); /* l_msg_mn_1st_talk */
            } else {
                mpMsgNo = gabi::at<be<u32>>(0x101C2074); /* l_msg_mn_2nd_talk */
            }
        } else if (mPosFlag == 1 && dComIfGs_isEventBit(0x3D08)) {
            dComIfGs_onEventBit(0x2F04);
            if (!dComIfGs_isEventBit(0x3120)) {
                dComIfGs_onEventBit(0x3120);
                mpMsgNo = gabi::at<be<u32>>(0x101C2084); /* l_msg_mn_comp_1st */
            } else {
                mpMsgNo = gabi::at<be<u32>>(0x101C2034); /* l_msg_mn_comp_2nd */
            }
        } else {
            if (!dComIfGs_isEventBit(0x2F04)) {
                dComIfGs_onEventBit(0x2F04);
                mpMsgNo = gabi::at<be<u32>>(0x101C201C); /* l_msg_mn_1st_talk_in */
            } else if (mbLookFigure != 0) {
                msg = gabi::load<u32>(MN_l_msg_mn_figure + mFigureMsgIdx * 4);
            } else if (!dComIfGs_isEventBit(0x3A01)) {
                mpMsgNo = gabi::at<be<u32>>(0x101C2024); /* l_msg_mn_2nd_talk_in */
            } else {
                mpMsgNo = gabi::at<be<u32>>(0x101C202C); /* l_msg_mn_3rd_talk_in */
            }
        }
    }
    if (mpMsgNo.get() != nullptr) {
        msg = *mpMsgNo.get();
    }
    return msg;
}
VERIFY(0x0229F518, &daNpcMn_c::getMsg);
