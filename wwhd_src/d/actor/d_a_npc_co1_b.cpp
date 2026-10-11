/**
 * d_a_npc_co1_b.cpp (WWHD)
 * NPC - Prince Komali (before Dragon Roost Cavern): part B (events, execute, draw, messages,
 * actions, compiler-generated tail)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_co1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_co1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, the pointer re-read at each use */
/* play object fields (dComIfGp_get() at each use) */
/* event manager (play + 0x52C4) */
static inline s8 co1_dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return co1_getActionIndex(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 02544950 dEvent_manager_c::ChkPresentEnd, 02544980 CancelPresent */
/* 025B7270 dSv_player_bag_item_c::setReserveItemEmpty (save info + 0x96) */
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status */
static inline u32 co1_fopMsgM_getStatus() { return co1_getMessageStatus(gabi::ea(fopNpc_npc_c::mpCurrMsg.get())); }
/* dAttention_c (play + 0x5804): 024EC8D0 LockonTarget, 024EE464 ActionTarget (matcher swaps them) */
static inline bool co1_dAttention_LockonTruth(dAttention_c* a) { return co1_lockonTruth(a); }
static inline fopAc_ac_c* co1_dAttention_LockonTarget(dAttention_c* a, s32 i) { return co1_lockonTarget(a, i); }
static inline fopAc_ac_c* co1_dAttention_ActionTarget(dAttention_c* a, s32 i) { return co1_actionTarget(a, i); }
/* d_npc */
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void co1_dNpc_playerEyePos_l(cXyz* out, f32 offs) { co1_playerEyePosition(out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 desiredYrot, s16 maxVel, bool headOnly) */
static inline void co1_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    co1_lookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D734 dNpc_chkLetterPassed() */
/* 02055B64 co1_cLib_calcTimer<s16> (out of line) */
static inline s16 co1_cLib_calcTimer(be<s16>* t) { return co1_decrementTimer(t); }

/* ---- file statics ---- */
/* daNpc_Co1_HIO_c, HD: vtable at 0; mPrmTbl (0x2C bytes) at +0xC */


/* (this->*pmf)(arg) */
static inline void pmf_call(daNpc_Co1_c* self, Co1ProcFunc* pmf, void* arg) {
    (self->*(*pmf))(arg);
}

static inline s32 morfFrameInt(mDoExt_McaMorf* morf) { return gabi::ftoi(morf->getFrame()); }

/* 02228CE4 */
void daNpc_Co1_c::eInit_MDR_() {
    setAnm_NUM(0xA, 1);
    mLookMode = 0;
}

/* 02228D20 */
void daNpc_Co1_c::eInit_RED_LTR_() {
    setAnm_NUM(8, 1);
}

/* 02228D2C */
void daNpc_Co1_c::event_actionInit(int i_staff_idx) {
    be<s32>* act_no_p = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staff_idx, "ActNo", 3); /* "ActNo" */
    if (act_no_p == nullptr) {
        return;
    }
    s8 act = (s8)*act_no_p;
    mEventAction = act;
    switch ((u32)(s32)act) {
    case 0:
        eInit_MDR_();
        break;
    case 1:
        eInit_RED_LTR_();
        break;
    }
}

/* 02228DDC */
u32 daNpc_Co1_c::eMove_MDR_() {
    bool ret = morfFrameInt(mpMorf) == 0;
    if (ret) {
        setAnm_NUM(0xB, 1);
    }
    return ret;
}

/* 02228E34 */
u32 daNpc_Co1_c::eMove_RED_LTR_() {
    bool ret = false;
    if (mAnimationStopped != 0) {
        switch ((u32)(s32)mBckNumber) {
        case 4:
            mNextState = 7;
            setStt(2);
            ret = true;
            break;
        case 8:
            setAnm_NUM(4, 1);
            break;
        default:
            setAnm_NUM(8, 1);
            break;
        }
    }
    return ret;
}

/* 02228EB8 */
u32 daNpc_Co1_c::event_action() {
    switch ((u32)(s32)mEventAction) {
    case 0:
        return eMove_MDR_();
    case 1:
        return eMove_RED_LTR_();
    default:
        return 1;
    }
}

/* 02228EFC */
void daNpc_Co1_c::privateCut(int i_staff_idx) {
    /* static char* a_cut_tbl[] = {"ACTION"} (.data 0x101BDDA0) */
    if (i_staff_idx == -1) {
        return;
    }
    mCutIndex = co1_dComIfGp_evmng_getMyActIdx(i_staff_idx, gabi::ea(l_cutNames), 1, TRUE, 0);
    if (mCutIndex == -1) {
        dComIfGp_evmng_cutEnd(i_staff_idx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(i_staff_idx)) {
        if (mCutIndex == 0) {
            event_actionInit(i_staff_idx);
        }
    }
    bool endCut;
    if (mCutIndex == 0) {
        endCut = event_action();
    } else {
        endCut = true;
    }
    if (endCut) {
        dComIfGp_evmng_cutEnd(i_staff_idx);
    }
}

/* 02228FD0 */
void daNpc_Co1_c::lookBack() {
    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    mPreviousBackboneYaw = m_jnt.mAngles[1][1];
    s16 desiredYrot = current.angle.y;
    mPreviousActorYaw = desiredYrot;
    mPreviousHeadYaw = m_jnt.mAngles[0][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    f32 srcZ = current.pos.z;
    dstPos_p = nullptr;
    u8 headOnlyFollow = mHeadOnlyFollow;

    switch ((u32)(s32)mLookMode) {
    case 1: {
        gabi::Local<cXyz> eye;
        co1_dNpc_playerEyePos_l(eye, -20.0f);
        dstPos->copy(*eye);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        srcY = eyePos.y;
        break;
    }
    case 2:
        dstPos->copy(mLookTarget);
        dstPos_p = dstPos;
        srcZ = current.pos.z;
        srcX = current.pos.x;
        break;
    case 3:
        desiredYrot = mLookTargetYaw;
        break;
    }
    cLib_addCalcAngleS2(&mLookTurnSpeed, l_HIO.mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mLookTurnSpeed = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    co1_dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, mLookTurnSpeed, headOnlyFollow);
}

/* 02229210 */
void daNpc_Co1_c::event_proc(int i_staff_idx) {
    if (dComIfGp_evmng_endCheck(mEventIdx[mActiveEventIdx])) {
        switch ((u32)(s32)mActiveEventIdx) {
        case 0:
            dComIfGs_onEventBit(0x0F04);
            setStt(4);
            break;
        case 1:
            dComIfGs_onEventBit(0x1810);
            break;
        case 2:
            mEventOrderKind = 1;
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staff_idx);
        }
        lookBack();
    }
}

/* 02229378 */
void daNpc_Co1_c::eventOrder() {
    s8 condition = mEventOrderKind;
    if (condition == 1 || condition == 2) {
        s8 c = mEventOrderKind;
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e); /* onCondition(CANTALK | CANTALKITEM) */
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        s16 idx = condition - 3;
        mActiveEventIdx = idx;
        fopAcM_orderOtherEventId(this, mEventIdx[idx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 022293E8 */
void daNpc_Co1_c::setCollision_SP_() {
    if (mTalkRequested != 0) {
        return;
    }
    if (mBckNumber == 1) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        gabi::Local<cXyz> off;
        off->x = 0.0f;
        off->y = 0.0f;
        off->z = -20.0f;
        gabi::Local<cXyz> center;
        PSMTXMultVec(mDoMtx_stack_c::get(), off, center);
        mCyl.SetC(center);
        mCyl.SetR(70.0f);
        mCyl.SetH(60.0f);
        dComIfG_Ccsp_Set(&mCyl);
    } else {
        gabi::Local<cXyz> center;
        center->x = current.pos.x;
        center->y = current.pos.y;
        center->z = current.pos.z;
        mCyl.SetC(center);
        mCyl.SetR(40.0f);
        mCyl.SetH(90.0f);
        dComIfG_Ccsp_Set(&mCyl);
    }
}

/* 02229558 */
BOOL daNpc_Co1_c::_execute() {
    if (!mSavedInitialTransform) {
        mSavedAngleX = current.angle.x;
        mSavedAngleY = current.angle.y;
        mSavedAngleZ = current.angle.z;
        mSavedPosition.copy(current.pos);
        mSavedInitialTransform = 1;
    }
    daNpc_Co1_HIO_c& prm = l_HIO;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (mSkipExecute && demoActorID == 0) {
        return TRUE;
    }
    mSkipPositionMove = 0;
    mSkipExecute = 0;
    checkOrder();
    if (!demo()) {
        s32 cond = -1;
        if (dComIfGp_event_runCheck() && eventInfo.mCommand != 1 /* eventInfo.mCommand != dEvtCmd_INTALK_e */) {
            cond = isEventEntry();
        }
        if (cond >= 0) {
            event_proc(cond);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!mSkipPositionMove) {
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }
        if (!mKeepShapeAngle) {
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(0);
    if (!mInDemo) {
        setCollision_SP_();
    }
    return TRUE;
}

/* 02229734 */
static BOOL daNpc_Co1_Execute(daNpc_Co1_c* i_this) {
    return i_this->_execute();
}

/* 02229738 */
BOOL daNpc_Co1_c::_draw() {
    J3DModelData* model_data = mpMorf->getModel()->getModelData();
    if (mPearlMorf.get() == nullptr) {
        return TRUE;
    }
    mDoExt_McaMorf* prlMorf = mPearlMorf.get();
    J3DModel* prlModel = prlMorf->getModel();
    J3DModelData* prl_data = prlModel->getModelData();
    if (mSkipExecute || mHideModel) {
        return TRUE;
    }
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry(&mBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    reinterpret_cast<Co1ModelData*>(model_data)->mTexturePattern = 0; /* mBtpAnm.remove(model_data) */
    setLightTevColorType(dKy_getEnvlight(), prlModel, &tevStr);
    mDoExt_btkAnm_entry(&mBtkAnm, prl_data, (f32)(u8)mPearlBtkFrame);
    mPearlMorf.get()->entryDL();
    reinterpret_cast<Co1ModelData*>(prl_data)->mTextureSRT = 0; /* mBtkAnm.remove(prl_data) */
    if (mShowLetterModel) {
        setLightTevColorType(dKy_getEnvlight(), mLetterModel.get(), &tevStr);
        mDoExt_modelEntryDL(mLetterModel.get());
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (l_HIO.mDebugDraw != 0) {
        if (l_debugColorGuard == 0) {
            l_debugColorGuard = 1;
            memcpy_g(l_debugColor, l_debugColorInitial, 4); /* 028FEAC0 memcpy */
        }
        if (l_debugPearlColorGuard == 0) {
            l_debugPearlColorGuard = 1;
            memcpy_g(l_debugPearlColor, l_debugPearlColorInitial, 4);
        }
        if (mPearlMorf.get() != nullptr) {
            if (l_debugMorfColorGuard == 0) {
                l_debugMorfColorGuard = 1;
                memcpy_g(l_debugMorfColor, l_debugMorfColorInitial, 4);
            }
        }
    }
    /* HD: no shadowDraw() */
    dSnap_RegistFig(0x8B /* DSNAP_TYPE_NPC_CO1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022298F8 */
static BOOL daNpc_Co1_Draw(daNpc_Co1_c* i_this) {
    return i_this->_draw();
}

/* 022298FC */
static BOOL daNpc_Co1_IsDelete(daNpc_Co1_c*) {
    return TRUE;
}

/* 02229904 */
void daNpc_Co1_c::set_target(int i_type) {
    switch ((u32)i_type) {
    case 0:
        mLookMode = 0;
        break;
    case 1: {
        gabi::Local<cXyz> eye;
        co1_dNpc_playerEyePos_l(eye, -20.0f);
        gabi::Local<cXyz> vec;
        vec->copy(*eye);
        f32 dist = fopAcM_searchPlayerDistance(this);
        s16 angle = fopAcM_searchPlayerAngleY(this) - 0x2000;
        mDoMtx_stack_c::transS(current.pos.x, vec->y, current.pos.z);
        mDoMtx_stack_c::YrotM(angle);
        vec->z = dist;
        vec->x = 0.0f;
        vec->y = 0.0f;
        PSMTXMultVec(mDoMtx_stack_c::get(), vec, &mLookTarget);
        mLookMode = 2;
        break;
    }
    }
}

/* 02229A38 */
void daNpc_Co1_c::setAnm_ATR(int i_param_1) {
    anm_prm_c* a_anm_prm_tbl = l_attributeAnmPrm;
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[mAnimationAttribute].mBtpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[mAnimationAttribute]);
}

/* 02229AA8 */
void daNpc_Co1_c::chg_anmAtr(u8 i_anmAtr) {
    if (i_anmAtr >= 0xE || i_anmAtr == mAnimationAttribute) {
        return;
    }
    switch (i_anmAtr) {
    case 3:
    case 4:
    case 5:
    case 7:
    case 0xB:
        mLookMode = 0;
        break;
    case 0xD:
        mLookMode = 1;
        set_target(1);
        break;
    default:
        mLookMode = 1;
        break;
    }
    mAnimationAttribute = i_anmAtr;
    setAnm_ATR(1);
}

/* 02229B74 */
void daNpc_Co1_c::control_anmAtr() {
    switch (mAnimationAttribute) {
    case 2:
    case 8:
        if (mAnimationStopped != 0) {
            setAnm_NUM(7, 1);
            mAnimationAttribute = 6;
        }
        break;
    case 3:
        if (mAnimationStopped != 0) {
            setAnm_NUM(4, 1);
            mAnimationAttribute = 0xA;
        }
        break;
    case 7:
        if (mAnimationStopped != 0) {
            setAnm_NUM(6, 1);
            mAnimationAttribute = 5;
        }
        break;
    case 9:
        if (mAnimationStopped != 0) {
            setAnm_NUM(1, 1);
            mAnimationAttribute = 1;
        }
        break;
    }
}

/* 02229C90 */
void daNpc_Co1_c::anmAtr(u16 i_param_1) {
    switch (i_param_1) {
    case 6: {
        if (mMessageAnimationPhase == 0) {
            mAnimationAttribute = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mMessageAnimationPhase = (s8)((u8)mMessageAnimationPhase + 1);
        }
        u8 mesgAnimeTagInfo = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        dComIfGp_clearMesgAnimeTagInfo();                /* dComIfGp_clearMesgAnimeTagInfo() */
        if (mesgAnimeTagInfo != 0xFF && mAnimationTag != mesgAnimeTagInfo) {
            mAnimationTag = mesgAnimeTagInfo;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        mMessageAnimationPhase = 0;
        break;
    }
    /* control_anmTag(): empty */
    control_anmAtr();
}

/* 02229D58 */
bool daNpc_Co1_c::chk_talk() {
    bool ret = true;
    mPresentItemNo = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mPresentItemNo = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}

/* 02229DD8 */
u8 daNpc_Co1_c::chk_partsNotMove() {
    return mPreviousHeadYaw == m_jnt.mAngles[0][1] && mPreviousBackboneYaw == m_jnt.mAngles[1][1] && mPreviousActorYaw == current.angle.y;
}

/* 02229E18 */
u16 daNpc_Co1_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x183E: *pMsgNo = 0x183F; break;
    case 0x183F: *pMsgNo = 0x1840; break;
    case 0x1841: *pMsgNo = 0x1842; break;
    case 0x1842: *pMsgNo = 0x1843; break;
    case 0x1843: *pMsgNo = 0x1844; break;
    case 0x1844: *pMsgNo = 0x1845; break;
    case 0x1845: *pMsgNo = 0x1846; break;
    case 0x1846: *pMsgNo = 0x1847; break;
    case 0x1847: *pMsgNo = 0x1848; break;
    case 0x1849: *pMsgNo = 0x184A; break;
    case 0x184A: *pMsgNo = 0x184B; break;
    default:
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}

/* 02229F00 */
u32 daNpc_Co1_c::getMsg_CO1_0() {
    if (mPresentItemNo == 0x98) {
        return 0x183C;
    }
    if (mPresentItemNo != 0xFF) {
        return 0x183B;
    }
    if (mReceivedLetter != 0) {
        return 0x183E;
    }
    if (dNpc_chkLetterPassed()) {
        if (dComIfGs_isEventBit(0x0F04)) {
            return 0x1849;
        }
        return 0x1841;
    }
    u32 msgNo = 0x1839;
    if (dComIfGs_isEventBit(0x0F08)) {
        msgNo = 0x183A;
    }
    return msgNo;
}

/* 02229FEC */
u32 daNpc_Co1_c::getMsg() {
    u32 msgNo = 0;
    if (mCharacterType == 0) {
        msgNo = getMsg_CO1_0();
    }
    return msgNo;
}

/* 0222A024 */
u8 daNpc_Co1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (co1_dAttention_LockonTruth(attention)) {
        return this == (void*)co1_dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)co1_dAttention_ActionTarget(attention, 0);
}

/* 0222A0AC */
BOOL daNpc_Co1_c::wait_1() {
    if (mEventOrderKind == 1 || mEventOrderKind >= 3) {
        return TRUE;
    }
    if (mTalkRequested) {
        if (chk_talk()) {
            mNextState = 4;
            setStt(3);
        }
        return TRUE;
    }
    mEventOrderKind = 2;
    return TRUE;
}

/* 0222A130 */
BOOL daNpc_Co1_c::wait_2() {
    if (mTalkRequested) {
        if (chk_talk()) {
            s8 next = 4;
            if (dNpc_chkLetterPassed()) {
                next = 7;
            }
            mNextState = next;
            setStt(2);
        }
        return TRUE;
    }
    mEventOrderKind = 2;
    if (mHasPlayerAttention) {
        mLookTimer = 60;
    }
    mLookMode = co1_cLib_calcTimer(&mLookTimer) != 0;
    return TRUE;
}

/* 0222A1DC */
BOOL daNpc_Co1_c::wakeup() {
    if (mAnimationStopped != 0) {
        mNextState = 4;
        setStt(2);
    }
    return TRUE;
}

/* 0222A218 */
BOOL daNpc_Co1_c::talk_1() {
    u8 ret = chk_partsNotMove();
    if (mEventOrderKind == 1 || mEventOrderKind >= 3) {
        return TRUE;
    }
    if (dNpc_chkLetterPassed() && mReceivedLetter == 0 && !dComIfGs_isEventBit(0x0F04)) {
        if (mEventOrderKind != 3) {
            mTalkRequested = 0;
            mPresentItemNo = 0xFF;
            endEvent();
            mEventOrderKind = 3;
        }
        return TRUE;
    }
    talk(1);
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus) */
    if (mbHasMsg) {
        u32 status = co1_fopMsgM_getStatus();
        if (status == 2 || status == 6) {
            if (mCurrMsgNo == 0x184B) {
                mLookMode = 0;
            }
        } else if (status == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0x1839:
                dComIfGs_onEventBit(0x0F08);
                break;
            case 0x1840:
                mReceivedLetter = 0;
                break;
            case 0x1848:
                dComIfGs_onEventBit(0x0F04);
                break;
            }
            if (mPresentItemNo == 0x98) {
                mEventOrderKind = 5;
                dComIfGp_evmng_CancelPresent();
                dComIfGs_setReserveItemEmpty();
                endEvent();
                mReceivedLetter = 1;
            } else {
                setStt(mNextState);
                if (mNextState != 7) {
                    mTalkRequested = 0;
                    endEvent();
                }
            }
            mPresentItemNo = 0xFF;
            mLookTimer = 60;
        }
    }
    return ret;
}

/* 0222A450 */
BOOL daNpc_Co1_c::toru_1() {
    if (mTalkRequested) {
        if (mPresentItemNo == 0x98) {
            mPresentItemNo = 0xFF;
            setAnm_NUM(8, 1);
        }
        if (mAnimationStopped != 0) {
            setStt(6);
        }
    }
    return FALSE;
}

/* 0222A4C4 */
BOOL daNpc_Co1_c::read_1() {
    if (mAnimationStopped != 0) {
        mNextState = 7;
        setStt(2);
    }
    return FALSE;
}

/* 0222A500 */
BOOL daNpc_Co1_c::modoru() {
    if (morfFrameInt(mpMorf) == 0) {
        setStt(4);
        mTalkRequested = 0;
        endEvent();
    }
    return TRUE;
}

/* 0222A568 */
BOOL daNpc_Co1_c::wait_action1(void*) {
    LIGHT_INFLUENCE* light = &mPearlLight;
    if (mActionPhase == 0) {
        if (dNpc_chkLetterPassed()) {
            setStt(4);
        } else {
            if (!dComIfGs_isEventBit(0x1810)) {
                mEventOrderKind = 4;
            }
            setStt(1);
        }
        light->mPower = 0.0f;
        light->mPos.copy(current.pos);
        dKy_plight_set(light);
        mActionPhase = (s8)((u8)mActionPhase + 1);
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) {
        return TRUE;
    }
    mHasPlayerAttention = chkAttention();
    switch ((u32)(s32)mState) {
    case 1:
        mEyePositionResult = wait_1();
        break;
    case 2:
        mEyePositionResult = talk_1();
        break;
    case 3:
        mEyePositionResult = wakeup();
        break;
    case 4:
        mEyePositionResult = wait_2();
        break;
    case 5:
        mEyePositionResult = toru_1();
        break;
    case 6:
        mEyePositionResult = read_1();
        break;
    case 7:
        mEyePositionResult = modoru();
        break;
    }
    lookBack();
    return TRUE;
}

/* 0222A73C daNpc_Co1_HIO_c::daNpc_Co1_HIO_c (HD: allocates when this == NULL) */
/* 0222A73C */
static daNpc_Co1_HIO_c* daNpc_Co1_HIO_c_ct(daNpc_Co1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Co1_HIO_c*)operator_new(0x38);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x100193B8;
    /* memcpy(&mPrmTbl, &a_prm_tbl (.data 0x101BDE84), 0x2C) */
    memcpy_g(&i_this->mMaxHeadX, &l_hioParameters, 0x2C); /* 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

/* 0222A7A8: static initialisation of the translation unit */
/* 0222A7A8 */
static void __sinit_d_a_npc_co1_cpp() {
    __sinit_header_statics();
    daNpc_Co1_HIO_c_ct(&l_HIO); /* static daNpc_Co1_HIO_c l_HIO */
}

/* 0222A848: sead::SafeString deleting destructor (this TU's copy) */
/* 0222A848 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0222A85C: daNpc_Co1_c deleting destructor (compiler-generated, HD virtual destructor) */
/* 0222A85C */
static void daNpc_Co1_c_dt(daNpc_Co1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        co1_destroyCircle(gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        __dt(&i_this->mObjAcch);
        co1_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0222A8F8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
/* 0222A8F8 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 10019600 "ActNo" in 02228D2C
*/
