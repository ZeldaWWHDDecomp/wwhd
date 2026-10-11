/**
 * d_a_npc_mn_exec.cpp (WWHD)
 * NPC - Manny: execute, move procs, events, messages.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_mn.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_mn.h"

static MnExecuteInit l_execute_init[5] = {
    {
        &daNpcMn_c::executeWaitInit,
    },
    {
        &daNpcMn_c::executeTalkInit,
    },
    {
        &daNpcMn_c::executeWalkInit,
    },
    {
        &daNpcMn_c::executeTurnInit,
    },
    {
        &daNpcMn_c::executeTalk3Init,
    },
};
static MnMoveProc moveProc[5] = {
    {
        &daNpcMn_c::executeWait,
    },
    {
        &daNpcMn_c::executeTalk,
    },
    {
        &daNpcMn_c::executeWalk,
    },
    {
        &daNpcMn_c::executeTurn,
    },
    {
        &daNpcMn_c::executeTalk3,
    },
};

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
    i_this->m_jnt.mbTrn = 1;
    return i_this->current.angle.y == ang;
}

/* 0229E41C */
bool daNpcMn_c::_execute() {
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck()  ||
        eventInfo.mCommand == 1  || mTalk3State != TALK3_INIT) {
        (this->*moveProc[mMoveState].action)();
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
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), mn_npc_dat(mNpcNo)->mCylRadius, 150.0f);
    NpcDatStruct* dat = mn_npc_dat(mNpcNo);
    cXyz* attPos = &attention_info.position;
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

/* 0229E8A0 */
u8 daNpcMn_c::executeCommon() {
    mEvtOrderType = mbPlayerAttention != 0;
    if (mTalkOrder == 1 && mMoveState != MOVE_PROC_TALK) {
        executeSetMode(MOVE_PROC_TALK);
    }
    return mTalkOrder;
}

/* 0229CF58 */
void daNpcMn_c::executeSetMode(u32 i_initIdx) {
    mTargetSpeedF = 0.0f;
    speedF = 0.0f;
    mMoveState = (this->*l_execute_init[i_initIdx].action)();
}

/* 0229E900 */
int daNpcMn_c::executeWaitInit() {
    speedF = 0.0f;
    if (mbLookFigure) {
        setAnmTbl(l_npc_anm_wait2);
        mWaitTimer = (s16)gabi::ftoi(cM_rndF(30.0f) + 150.0f);
    } else {
        setAnmTbl(l_npc_anm_wait);
        NpcDatStruct* dat = mn_npc_dat(mNpcNo);
        f32 r = cM_rndF((f32)(dat->mWaitTimerMax - dat->mWaitTimerMin));
        mWaitTimer = (s16)gabi::ftoi(r + (f32)mn_npc_dat(mNpcNo)->mWaitTimerMin);
    }
    return MOVE_PROC_WAIT;
}

/* 0229EA30 */
void daNpcMn_c::executeWait() {
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
        if (mbPlayerAttention && dComIfGs_isEventBit(MN_EVENT_FIRST_TALK) && mn_checkTelescopeLook()) {
            reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mScopeType = 1;
            if (reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mMesgStatus == 0xB  &&
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
            fopAc_ac_c* figure = fopAcM_searchFromName("Figure" , 0xFF, mFigureArg);
            if (figure != nullptr && reinterpret_cast<MnFigureView*>(figure)->mbDisplay ) {
                mLookAtPos.copy(figure->eyePos);
                m_jnt.mbTrn = 1;
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

/* 0229ECEC */
int daNpcMn_c::executeTalkInit() {
    return MOVE_PROC_TALK;
}

/* 0229ECF4 */
void daNpcMn_c::executeTalk() {
    executeCommon();
    if (talk2(1) == 0x12) {
        mTalkOrder = 0;
        executeSetMode(MOVE_PROC_WAIT);
        dComIfGp_event_onEventFlag(8);
    } else {
        setAnmFromMsgTag();
    }
}

/* 0229ED6C */
int daNpcMn_c::executeTalk3Init() {
    mTalk3State = TALK3_INIT;
    return MOVE_PROC_TALK3;
}

/* 0229EF28 */
void daNpcMn_c::executeTalk3() {
    switch (mTalk3State) {
    case TALK3_INIT:
    case TALK3_ORDER:
        if (eventInfo.mCommand == 2 ) {
            mTalk3State = TALK3_TALK;
        } else {
            fopAcM_orderPotentialEvent(this, 0xA , 0, 0);
            mTalk3State = TALK3_ORDER;
            eventInfo.onCondition(dEvtCnd_UNK2_e);
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

/* 0229F00C */
int daNpcMn_c::executeWalkInit() {
    setAnmTbl(l_npc_anm_walk);
    return MOVE_PROC_WALK;
}

/* 0229F038 */
void daNpcMn_c::executeWalk() {
    if (executeCommon()) {
        return;
    }
    bool reachedEnd = false;
    gabi::Local<cXyz> cur;
    cur->x = current.pos.x;
    cur->y = current.pos.y;
    cur->z = current.pos.z;
    if (mPathRun.chkPointPass(cur.get(), mPathRun.mbDir != 0)) {
        mFigureArg = mPathRun.pointArg(mPathRun.mIdx);
        if (mFigureArg != 0xFF) {
            fopAc_ac_c* figure = fopAcM_searchFromName("Figure" , 0xFF, mFigureArg);
            if (figure != nullptr && reinterpret_cast<MnFigureView*>(figure)->mbDisplay ) {
                mFigureMsgIdx = getRand(5);
                mbLookFigure = 1;
                executeSetMode(MOVE_PROC_WAIT);
            }
        }
        if (!mPathRun.nextIdxAuto()) {
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
        mPathRun.getPoint(point.get(), mPathRun.mIdx);
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
        NpcDatStruct* dat = mn_npc_dat(mNpcNo);
        s16 ang = *angle;
        mHomeYRot = ang;
        mTargetYRot = ang;
        mHeadOnlyFollow = false;
        mLookMode = LOOK_MODE_TURN;
        mLookAtMaxVel = dat->mWalkLookAtMaxVel;
        m_jnt.mbTrn = 1;
        mTargetSpeedF = dat->mWalkSpeed;
    } else {
        mPathRun.mbDir = mPathRun.mbDir ^ 1;
        gabi::Local<cXyz> point;
        mPathRun.getPoint(point.get(), mPathRun.mIdx);
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
        // HD: no mPathRun.setInf(0xFF, roomNo, true)

        executeSetMode(MOVE_PROC_WAIT);
    }
}

/* 0229F2B0 */
int daNpcMn_c::executeTurnInit() {
    gabi::Local<cXyz> point;
    mPathRun.getPoint(point.get(), mPathRun.mIdx);
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
        setAnmTbl(l_npc_anm_walk);
        NpcDatStruct* dat = mn_npc_dat(mNpcNo);
        f32 r = cM_rndF((f32)(dat->mTurnWaitMax - dat->mTurnWaitMin));
        mWaitTimer = (s16)gabi::ftoi(r + (f32)mn_npc_dat(mNpcNo)->mTurnWaitMin);
        return MOVE_PROC_WALK;
    }
    return MOVE_PROC_TURN;
}

/* 0229F410 */
void daNpcMn_c::executeTurn() {
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> point;
    mPathRun.getPoint(point.get(), mPathRun.mIdx);
    if (mn_turnTo(this, point.get())) {
        executeSetMode(MOVE_PROC_WALK);
    }
}

/* 0229CFE4 */
void daNpcMn_c::checkOrder() {
    u16 command = eventInfo.mCommand;
    if (command == 2 ) {
        if (dComIfGp_evmng_startCheck(mHatchEventIdx) && mEvtOrderType == 3) {
            mEvtOrderType = 0;
        }
    } else if (command == 1  && (mEvtOrderType == 2 || mEvtOrderType == 1)) {
        mTalkOrder = 1;
        executeSetMode(MOVE_PROC_TALK);
    }
}

/* 0229E014 */
void daNpcMn_c::eventOrder() {
    if (mEvtOrderType == 2 || mEvtOrderType == 1) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (mEvtOrderType == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (mEvtOrderType == 3) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, mHatchEventIdx, 0, 0xFFFF);
    }
}

/* 0229DF8C */
void daNpcMn_c::eventMove() {
    if (!chkEndEvent()) {
        u8 attn = mEventCut.mbAttention;
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

/* 0229DC44 */
void daNpcMn_c::privateCut() {

    const char* staff = l_npc_staff_id[0];
    int staffIdx = dComIfGp_evmng_getMyStaffId(staff, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    mActIdx = mn_getMyActIdx(staffIdx, cut_name_tbl, 10, TRUE, 0);
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

/* 0229D120 */
void daNpcMn_c::eventMesSetInit(int i_staffIdx) {
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "MsgNo" );
    if (pData != nullptr) {
        mpMsgNo = nullptr;
        u32 msg = *pData;
        if (msg == 1) {
            return;
        }
        if (msg == 0) {
            // HD: virtual
            msg = mn_v_getMsg(this);
        }
        setMessage(msg);
        if (mpMsgNo.get() != nullptr) {
            setMessage(*mpMsgNo.get());
        }
    } else {
        mpMsgNo = mpMsgNo.get() + 1;
        setMessage(*mpMsgNo.get());
    }
}

/* 0229D6F8 */
bool daNpcMn_c::eventMesSet() {
    return talk2(0) == 0x12 ;
}

/* 0229D1DC */
void daNpcMn_c::eventGetItemInit() {
    u32 procItem = fopAcM_createItemForPresentDemo(&current.pos, mItemNo, 0, -1, -1, nullptr, nullptr);
    if (procItem != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(procItem);
    }
}

/* 0229D234 */
void daNpcMn_c::eventWaitInit(int i_staffIdx) {
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Timer" );
    mEventTimer = 0;
    if (timer != nullptr) {
        mEventTimer = (s16)(s32)*timer;
    }
}

/* 0229D754 */
bool daNpcMn_c::eventWait(int i_staffIdx) {
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        return false;
    }
    if (dComIfGp_evmng_getMyIntegerP(i_staffIdx, "SwOn" ) != nullptr) {
        u8 sw = getPrmSwitchBit2();
        dComIfGs_onSwitch(sw, home.roomNo);
    }
    return true;
}

/* 0229D4DC */
void daNpcMn_c::eventSwOnInit(int i_staffIdx) {
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Timer" );
    mEventTimer = 0;
    if (timer != nullptr) {
        mEventTimer = (s16)(s32)*timer;
    }
}

/* 0229DBC8 */
bool daNpcMn_c::eventSwOn() {
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        return false;
    }
    u8 sw = getPrmSwitchBit2();
    dComIfGs_onSwitch(sw, home.roomNo);
    return true;
}

/* 0229D29C */
void daNpcMn_c::eventHatchInit() {
    fopAc_ac_c* hatch = fopAcM_searchFromName("Ohatch" , 0, 0);
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

/* 0229D804 */
bool daNpcMn_c::eventHatch() {
    mTargetYRot = mHomeYRot;
    mHeadOnlyFollow = false;
    mLookMode = LOOK_MODE_TURN;
    m_jnt.mbTrn = 1;
    return mHomeYRot == current.angle.y;
}

/* 0229D32C */
void daNpcMn_c::eventBikkuriInit(int i_staffIdx) {
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "Timer" );
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

/* 0229D838 */
bool daNpcMn_c::eventBikkuri() {
    if (mEventTimer != 0) {
        mEventTimer = mEventTimer - 1;
        if (mEventTimer == 0) {
            setAnmTbl(l_npc_anm_bikkuri);
        }
        return false;
    }
    return (mAnmFlag & 1) != 0;
}

/* 0229D3BC */
u32 daNpcMn_c::eventTurnInit() {
    return mn_setAnmTblResult(this, l_npc_anm_wait);
}

/* 0229D8A8 */
bool daNpcMn_c::eventTurn(int i_staffIdx) {
    be<s32>* pData = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "TurnMode" );
    if (pData != nullptr && *pData == 1) {
        fopAc_ac_c* hatch = fopAcM_searchFromName("Ohatch" , 0, 0);
        if (hatch != nullptr) {
            return mn_turnTo(this, &hatch->current.pos);
        }
    }
    gabi::Local<cXyz> pos;
    mPathRun.getPoint(pos.get(), mPathRun.mIdx);
    return mn_turnTo(this, pos.get());
}

/* 0229D3C8 */
u32 daNpcMn_c::eventWalkInit() {
    return mn_setAnmTblResult(this, l_npc_anm_walk);
}

/* 0229DA24 */
bool daNpcMn_c::eventWalk() {
    gabi::Local<cXyz> cur;
    cur->x = current.pos.x;
    cur->y = current.pos.y;
    cur->z = current.pos.z;
    if (mPathRun.chkPointPass(cur.get(), mPathRun.mbDir != 0)) {
        if (!mPathRun.nextIdxAuto()) {
            mTargetSpeedF = 0.0f;
            speedF = 0.0f;
            return true;
        }
    }
    gabi::Local<cXyz> point;
    mPathRun.getPoint(point.get(), mPathRun.mIdx);
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
    NpcDatStruct* dat = mn_npc_dat(mNpcNo);
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

/* 0229D3D4 */
u32 daNpcMn_c::eventLookInit() {
    return mn_setAnmTblResult(this, l_npc_anm_jump1);
}

/* 0229DB54 */
bool daNpcMn_c::eventLook() {
    return !(mpMorf->getFrame() < 72.0f);
}

/* 0229D3E0 */
void daNpcMn_c::eventJumpInit(int i_staffIdx) {
    be<f32>* speedX = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, "SpeedX" );
    dComIfGp_evmng_getMyFloatP(i_staffIdx, "SpeedY" );
    dComIfGp_evmng_getMyFloatP(i_staffIdx, "Gravity" );
    // HD: the GameCube values from SpeedY / Gravity are overwritten anyway, so only speedF depends on the event data

    if (speedX != nullptr) {
        speedF = *speedX;
    } else {
        speedF = 3.0f;
    }
    mTargetSpeedF = 40.0f;
    mJumpSpeedY = 25.0f;
    gravity = -1.0f;
}

/* 0229DB78 */
bool daNpcMn_c::eventJump() {
    f32 jump = mJumpSpeedY;
    f32 newJump = jump + gravity;
    f32 y = current.pos.y + jump;
    mJumpSpeedY = newJump;
    current.pos.y = y;
    if (newJump < 0.0f && mObjAcch.ChkGroundLanding() ) {
        speedF = 0.0f;
        mTargetSpeedF = 0.0f;
        return true;
    }
    return false;
}

/* 0229D544 */
u16 daNpcMn_c::talk2(int i_param) {
    // HD: the message (GameCube mpCurrMsg) is the message manager, read once

    u32 mgr = mn_msgManager();
    u16 status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (i_param == 1) {
            mCurrMsgNo = mn_v_getMsg(this);
        }
        mCurrMsgBsPcId = msgMng_messageSet(mgr, mCurrMsgNo, &eyePos);
        // HD: only when the message was set
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
            mLastMsgStatus = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)msgMng_getStatus(mgr);
        switch (status) {
        case 0xE:
            msgMng_setStatus(mgr, mn_v_next_msgStatus(this, &mCurrMsgNo));
            if (msgMng_getStatus(mgr) == 0xF ) {
                msgMng_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;

        case 0x12:
            msgMng_setStatus(mgr, 0x13 );
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        mLastMsgStatus = status;
        mn_v_anmAtr(this, status);
    } else {
        // HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag

        mbHasMsg = 1;
    }
    return status;
}

/* 0229ED8C */
u16 daNpcMn_c::talk3(int i_param) {
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
            dComIfGp_setScopeMesgStatus(0x11 );
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

/* 0229F4D0 */
u16 daNpcMn_c::next_msgStatus(be<u32>* i_msgNo) {
    u16 ret = 0xF;
    if (mpMsgNo.get() != nullptr) {
        mpMsgNo = mpMsgNo.get() + 1;
        u32 msg = *mpMsgNo.get();
        if (msg == 0) {
            mpMsgNo = nullptr;
            ret = 0x10;
        } else {
            *i_msgNo = msg;
        }
    } else {
        ret = 0x10;
    }
    return ret;
}

/* 0229F518 */
u32 daNpcMn_c::getMsg() {
    u32 msg = 0;
    mpMsgNo = nullptr;
    if (!dComIfGp_event_chkTalkXY() ) {
        if (!mPosFlag) {
            if (!dComIfGs_isEventBit(MN_EVENT_FIRST_TALK)) {
                dComIfGs_onEventBit(MN_EVENT_FIRST_TALK);
                mpMsgNo = l_msg_mn_1st_talk;
            } else {
                mpMsgNo = l_msg_mn_2nd_talk;
            }
        } else if (mPosFlag == 1 && dComIfGs_isEventBit(MN_EVENT_COMPLETE)) {
            dComIfGs_onEventBit(MN_EVENT_FIRST_ROOM_TALK);
            if (!dComIfGs_isEventBit(MN_EVENT_COMPLETE_TALK)) {
                dComIfGs_onEventBit(MN_EVENT_COMPLETE_TALK);
                mpMsgNo = l_msg_mn_comp_1st;
            } else {
                mpMsgNo = l_msg_mn_comp_2nd;
            }
        } else {
            if (!dComIfGs_isEventBit(MN_EVENT_FIRST_ROOM_TALK)) {
                dComIfGs_onEventBit(MN_EVENT_FIRST_ROOM_TALK);
                mpMsgNo = l_msg_mn_1st_talk_in;
            } else if (mbLookFigure != 0) {
                msg = l_msg_mn_figure[mFigureMsgIdx];
            } else if (!dComIfGs_isEventBit(MN_EVENT_ALLOW_ROOM_CHANGE)) {
                mpMsgNo = l_msg_mn_2nd_talk_in;
            } else {
                mpMsgNo = l_msg_mn_3rd_talk_in;
            }
        }
    }
    if (mpMsgNo.get() != nullptr) {
        msg = *mpMsgNo.get();
    }
    return msg;
}

/* 0229ED7C */
u32 daNpcMn_c::getMsg3() {
    mpMsgNo = nullptr;
    return 0x35EF;
}

/* 0229D118 */
void daNpcMn_c::setMessage(u32 i_message) {
    mCurrMsgNo = i_message;
}

/* 0229DF30 */
void daNpcMn_c::setAnmFromMsgTag() {
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (attr <= 3) {
        // HD: a table at )

        setAnmTbl(l_msgAnm[attr]);
    }
    dComIfGp_setMesgAnimeAttrInfo(0xFF);
}

/* 0229E210 */
void daNpcMn_c::lookBack() {
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
        m_jnt.mbTrn = 1;
    }
    if (m_jnt.mbTrn != 0) {
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

/* 0229E0AC */
void daNpcMn_c::playTexPatternAnm() {
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

/* 0229E14C */
void daNpcMn_c::playAnm() {
    if (mpMorf->play(nullptr, 0, 0) && mpAnmDat.get() != nullptr && mAnmLoopCnt > 0) {
        mAnmLoopCnt = mAnmLoopCnt - 1;
        if (mAnmLoopCnt == 0) {
            mpAnmDat = mpAnmDat.get() + 1;
            if (setAnmTbl(mpAnmDat)) {
                mAnmFlag = mAnmFlag | 1;
            }
        } else {
            setAnm(mpAnmDat->mBckIdx, 0, 0.0f);
        }
    }
}

/* 0229D094 */
BOOL daNpcMn_c::chkEndEvent() {
    if (dComIfGp_evmng_endCheck(mHatchEventIdx)) {
        dComIfGp_event_onEventFlag(8);
        fopAcM_delete(this);
        return TRUE;
    }
    return FALSE;
}

/* WWHD glue (tools/verify/autoglue.py)

str 1001E81C in 0229D120
str 1001E824 in 0229D234
str 1001E82C in 0229D29C
str 1001E834 in 0229D32C
str 1001E84C in 0229D3E0
str 1001E854 in 0229D3E0
str 1001E844 in 0229D3E0
str 1001E85C in 0229D4DC
str 1001E864 in 0229D754
str 1001E874 in 0229D8A8
str 1001E86C in 0229D8A8
str 1001E8F4 in 0229EA30
str 1001E8FC in 0229F038
data l_execute_init 101C20E4
data moveProc 101C210C
ext 0229C4E4 u32 mn_setAnmTblResult(daNpcMn_c* self, sMnAnmDat* anm)
*/
