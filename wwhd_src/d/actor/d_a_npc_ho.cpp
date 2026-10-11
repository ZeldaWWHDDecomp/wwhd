/**
 * d_a_npc_ho.cpp (WWHD)
 * NPC - Mrs. Marie (Windfall Island teacher)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ho.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ho.h"

// HD: GHS emits this translation unit's inline constructor/destructor vtables.
const gc_tu_data gc_tu = {
    .safeString = 0x1001A578,
    .safeStringAssure = 0x02240E28,
    .objAcch = {0x1001A5A0, 0x1001A5C0, 0x1001A5B0},
    .objAcchDt = {0x1001A5B0, 0x1001A5C0},
    .headerStatics = {0x104671B0, 0x101BE830},
};

static u32 l_msgId;
static const s32 l_bck_ix_tbl[] = {9, 7, 8, 6, 8};
static const s32 l_btp_ix_tbl[] = {18};
static const s8 msg_anm_table[] = {0, 1, 2, 3, 4};

static msg_class* l_msg; // HD: shared message-manager pointer replaces the GC message actor.
static ho_cpp::CylinderSource dNpc_cyl_src;

using namespace ho_cpp;
enum { fpcNm_NPC_HO_e = 0x16E };

/* 0223E47C */
static BOOL nodeCallBack_Ho(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Ho_c* i_this = (daNpc_Ho_c*)model->getUserArea();
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
                MtxPosition(temp, temp2);
                i_this->mAttnBasePos.x = temp2->x; /* setAttentionBasePos(temp2) */
                i_this->mAttnBasePos.y = temp2->y;
                i_this->mAttnBasePos.z = temp2->z;
                temp->x = 20.0f;
                temp->y = -20.0f;
                temp->z = 0.0f;
                MtxPosition(temp, temp2);
                i_this->mEyePos.x = temp2->x; /* setEyePos(temp2) */
                i_this->mEyePos.y = temp2->y;
                i_this->mEyePos.z = temp2->z;
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            }
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            mtx_copy(model->getAnmMtx(jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* 0223E70C */
static s16 daNpc_ho_XyCheckCB(void* i_this, int i_itemBtn) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Ho_c*)i_this)->XyCheckCB(i_itemBtn);
}

/* 0223E704 */
s16 daNpc_Ho_c::XyCheckCB(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

/* 0223FA10 */
void daNpc_Ho_c::receivePendant(int numPendantsGiven) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    int totalGiven = dComIfGs_getEventReg(0xC0FF /* UNK_C0FF */);
    dComIfGp_setItemBeastNumCount(dBeastIdx_JOY_PENDANT_e, -numPendantsGiven);
    totalGiven += numPendantsGiven;
    if (totalGiven > 99) {
        dComIfGp_setItemBeastNumCount(dBeastIdx_JOY_PENDANT_e, totalGiven - 99);
        totalGiven = 99;
    }
    dComIfGs_setEventReg(0xC0FF, (u8)totalGiven);
}

/* 0223E710 */
BOOL daNpc_Ho_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* modelData = J3DModel_getModelData(mpMorf->getModel());
    // HD: the texture table always uses index zero rather than mTexPatternIdx.
    m_head_tex_pattern =
        (J3DAnmTexPattern*)dComIfG_getObjectRes("Ho" /* "Ho" */, entry(l_btp_ix_tbl, 0), gc_tu.safeString);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(329, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x149, "m_head_tex_pattern != (0)");
    if (!hoBtpInit(&mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify,
                   FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 0223EFF0 */
void daNpc_Ho_c::playTexPatternAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = getFrameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= frameMax0) {
            s32 frameMax = getFrameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = mBlinkFrame + 1;
        }
    }
}

/* 0223F0B4 */
void daNpc_Ho_c::setAnm(s8 newAnmIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 morf = 8.0f;
    if (newAnmIdx != mCurrAnmIdx && newAnmIdx != -1) {
        mCurrAnmIdx = newAnmIdx;
        mAnmTimer = 0.0f;
        dNpc_setAnm(mpMorf, -1 /* EMode_NULL */, morf, 1.0f, entry(l_bck_ix_tbl, newAnmIdx), -1,
                    "Ho" /* "Ho" */);
    }
}

/* 0223F774 */
void daNpc_Ho_c::setAnmStatus() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    setAnm(0);
}

/* 0223F77C */
bool daNpc_Ho_c::chkAttentionLocal() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dAttention_c* attention = dComIfGp_getAttention();
    if (chkFlag(HO_FLAG_00000001 | HO_FLAG_00000010)) {
        return true;
    }
    if (mAttentionTimer != 0) {
        mAttentionTimer = mAttentionTimer - 1;
        return true;
    }
    if (hoLockonTruth(attention)) {
        return this == (void*)hoLockonTarget(attention, 0);
    }
    return this == (void*)hoActionTarget(attention, 0);
}

/* 0223F860 */
void daNpc_Ho_c::chkAttention() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    bool temp = chkFlag(HO_FLAG_00000004);
    if (chkAttentionLocal()) {
        setFlag(HO_FLAG_00000004);
    } else {
        clrFlag(HO_FLAG_00000004);
    }
    if (temp != chkFlag(HO_FLAG_00000004) && temp == true) {
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
}

/* 0223F1B8 */
void daNpc_Ho_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (ChkOrder(1)) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }
    if (ChkOrder(2)) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(4)) {
        eventInfo.onCondition(dEvtCnd_CANTALKITEM_e);
    }
}

/* 0223F130 */
void daNpc_Ho_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (eventInfo.checkCommandTalk() && ChkOrder(7)) {
        if (dComIfGp_event_chkTalkXY()) {
            setFlag(HO_FLAG_00000010);
        } else {
            setFlag(HO_FLAG_00000001);
        }
        talkInit();
    }
    ClrOrder();
}

/* 0223F8E4 */
u32 daNpc_Ho_c::next_msg_sub0(u32 msgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (mMsgSelectNum) {
    case 0:
        if (msgNo == 0x2712) {
            if (dComIfGs_isEventBit(0x2201)) {
                if (dComIfGs_isEventBit(0x1340)) {
                    if (dComIfGs_isEventBit(0x1F80)) {
                        return 0x2720;
                    }
                    dComIfGs_onEventBit(0x1F80);
                    if (!dKy_daynight_check()) {
                        dComIfGs_onTmpBit(0x0580);
                    }
                    return 0x271E;
                }
                return 0x271D;
            }
            return 0x2716;
        }
        return 0x272B;
    default:
        return 0x2714;
    }
}

/* 0223FAB8 */
u32 daNpc_Ho_c::next_msgStatus(be<u32>* pMsgNo) { /* u16 (talk passes r3 on as is) */
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x2712:
    case 0x2713:
        *pMsgNo = next_msg_sub0(*pMsgNo);
        break;
    case 0x2718:
        switch (mMsgSelectNum) {
        case 0:
            *pMsgNo = 0x271A;
            dComIfGs_onEventBit(0x1380);
            break;
        default:
            *pMsgNo = 0x2719;
            break;
        }
        break;
    case 0x271E:
        mNextMessageId = 0x2748;
        mItemNum = 5; /* dItemNo_PURPLE_RUPEE_e */
        *pMsgNo = *pMsgNo + 1;
        mState = HO_STATE_TALK_03;
        break;
    case 0x272B:
        switch (mMsgSelectNum) {
        case 0:
            *pMsgNo = 0x272C;
            break;
        case 1:
            *pMsgNo = 0x272E;
            break;
        default:
            *pMsgNo = 0x2732;
            break;
        }
        break;
    case 0x2732:
        *pMsgNo = 0x2736;
        break;
    case 0x272C:
        if (dComIfGs_isEventBit(0x1C04)) {
            *pMsgNo = 0x2758;
        } else if (dComIfGs_isEventBit(0x1C08)) {
            *pMsgNo = 0x2759;
        } else {
            *pMsgNo = 0x272D;
        }
        break;
    case 0x2725:
        setFlag(HO_FLAG_00000020);
        mNextMessageId = 0x2726;
        msgStatus = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    case 0x2716:
    case 0x2717:
    case 0x271A:
    case 0x271B:
    case 0x2720:
    case 0x2724:
    case 0x2726:
    case 0x2727:
    case 0x2728:
    case 0x272E:
    case 0x272F:
    case 0x2730:
    case 0x2736:
    case 0x2737:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x273D:
        *pMsgNo = 0x275B;
        receivePendant(1);
        break;
    case 0x273E:
        if (dComIfGs_getBeastNum(dBeastIdx_JOY_PENDANT_e) >= 20) {
            *pMsgNo = 0x2741;
        } else {
            *pMsgNo = 0x273F;
        }
        break;
    case 0x2749:
        *pMsgNo = 0x273E;
        break;
    case 0x275C:
        mNextMessageId = 0x275D;
        mItemNum = 4; /* dItemNo_RED_RUPEE_e */
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    case 0x2742:
        *pMsgNo = 0x2743;
        receivePendant(0x14);
        break;
    case 0x2743:
        mNextMessageId = 0x2744;
        mItemNum = 0x9C; /* dItemNo_CABANA_DEED_e */
        msgStatus = fopMsgStts_MSG_ENDS_e;
        dComIfGs_onEventBit(0x1C08);
        dComIfGs_onTmpBit(0x0104);
        break;
    case 0x274B:
        *pMsgNo = 0x274D;
        break;
    case 0x274D:
        *pMsgNo = 0x274E;
        receivePendant(dComIfGs_getBeastNum(dBeastIdx_JOY_PENDANT_e));
        break;
    case 0x274E:
        if (dComIfGs_getEventReg(0xC0FF) >= 0x28 && !dComIfGs_isEventBit(0x1C04)) {
            *pMsgNo = 0x274F;
        } else {
            msgStatus = fopMsgStts_MSG_ENDS_e;
        }
        break;
    case 0x2754: {
        *pMsgNo = 0x2755;
        int numPendantsGiven = dComIfGs_getBeastNum(dBeastIdx_JOY_PENDANT_e);
        receivePendant(numPendantsGiven);
        if (numPendantsGiven < 3) {
            mItemNum = 4; /* dItemNo_RED_RUPEE_e */
        } else if (numPendantsGiven < 5) {
            mItemNum = 5; /* dItemNo_PURPLE_RUPEE_e */
        } else {
            mItemNum = 6; /* dItemNo_ORANGE_RUPEE_e */
        }
        mNextMessageId = 0x2757;
        break;
    }
    case 0x2751:
        mNextMessageId = 0x2752;
        mItemNum = 0xF8; /* dItemNo_HEROS_CHARM_e */
        msgStatus = fopMsgStts_MSG_ENDS_e;
        dComIfGs_onEventBit(0x1C04);
        break;
    case 0x2756:
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    case 0x273A:
    case 0x274C:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x273B:
    case 0x273C:
    case 0x273F:
    case 0x2741:
    case 0x2744:
    case 0x2745:
    case 0x2746:
    case 0x2747:
    case 0x274A:
    case 0x274F:
    case 0x2750:
    case 0x2752:
    case 0x2755:
    case 0x275B:
        *pMsgNo = *pMsgNo + 1;
        break;
    default:
        msgStatus = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return msgStatus;
}

/* 0223FFB4 */
u32 daNpc_Ho_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (mState) {
    case HO_STATE_TALK_01:
        if (hoNightStop()) {
            return 0x2722;
        }
        if (!dComIfGs_isEventBit(0x1E01)) {
            dComIfGs_onEventBit(0x1E01);
            return 0x2711;
        }
        return (dComIfGs_getEventReg(0xC0FF) == 0 ? 0 : 1) + 0x2712;
    case HO_STATE_TALK_03:
        if (dComIfGp_event_getPreItemNo() != dItemNo_JOY_PENDANT_e || !dComIfGs_isEventBit(0x1E04)) {
            return 0x2739;
        }
        if (dComIfGs_getEventReg(0xC0FF) == 0) {
            return 0x273A;
        }
        if (!dComIfGs_isEventBit(0x1C08)) {
            return 0x2749;
        }
        if (!dComIfGs_isEventBit(0x1C04)) {
            if (dComIfGs_isTmpBit(0x0104)) {
                return 0x274A;
            }
            return 0x274C;
        }
        if (dComIfGs_getEventReg(0xC0FF) >= 0x63) {
            return 0x275E;
        }
        return 0x2754;
    case HO_STATE_TALK_03_CONTINUE:
        return mNextMessageId;
    }
    return 0;
}

/* 0223F230 */
void daNpc_Ho_c::setCollision() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    f32 cylCollision = mCylCollisionRadius;
    f32 height = 140.0f;
    mCyl.SetC(centerPos);
    mCyl.SetR(cylCollision);
    mCyl.SetH(height);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 0223FF20 */
void daNpc_Ho_c::msgPushButton() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_msg;
    switch (mCurrMsgNo) {
    case 0x2712:
    case 0x2713:
    case 0x2718:
    case 0x272B:
        mMsgSelectNum = (u8)mgr->mSelectNum; /* HD: l_msg->mSelectNum */
        break;
    }
}

/* 0223FF58 */
void daNpc_Ho_c::msgAnm(u8 param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // HD: the animation table is translation-unit data.
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = param_1;
        if (param_1 < 5) {
            setAnm(entry(msg_anm_table, param_1));
        }
        mAnmLoopCount = 0;
    }
}

/* 0223F11C */
void daNpc_Ho_c::talkInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mTalkState = daNpc_Ho_c::TALK_INIT;
    mMsgAnmIdx = 0xFF;
}

/* 022401E8 */
u16 daNpc_Ho_c::talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // HD: use the message manager directly; GameCube searched a message actor by l_msgId.
    msg_class* mgr = l_msg;
    u16 msgStatus = 0xFF;
    if (mTalkState == daNpc_Ho_c::TALK_INIT) {
        l_msgId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = daNpc_Ho_c::TALK_MSG_CREATE;
    } else if (mTalkState != daNpc_Ho_c::TALK_FINISHED) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = hoMessageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            if (!chkFlag(HO_FLAG_00000008)) {
                msgAnm(dComIfGp_getMesgAnimeAttrInfo());
            }
            switch ((u32)(s32)mTalkState) {
            case 1:
                mTalkState = TALK_ACTIVE; // HD: the manager replaces fopMsgM_SearchByID.
                break;
            case 2:
                msgStatus = hoMessageStatus(mgr);
                if (msgStatus == fopMsgStts_MSG_DISPLAYED_e) {
                    msgPushButton();
                    hoSetMessageStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (hoMessageStatus(mgr) == fopMsgStts_MSG_CONTINUES_e /* fopMsgStts_MSG_CONTINUES_e */) {
                        hoMessageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
                    hoSetMessageStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                    mTalkState = TALK_FINISHED;
                } else if (msgStatus == fopMsgStts_MSG_CONTINUES_e && (hoTriggerA(0) || hoTriggerB(0))) {
                    switch (mCurrMsgNo) {
                    case 0x274D:
                    case 0x273E:
                    case 0x273B:
                    case 0x2755:
                        dComIfGp_evmng_CancelPresent();
                        break;
                    }
                }
                break;
            }
        }
    }
    return msgStatus;
}

/* 0223EB98 */
BOOL daNpc_Ho_c::init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e |
                           fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x6F;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x6F; /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    switch (mType) {
    case 0: {
        /* setAction(&daNpc_Ho_c::wait_action, NULL) (inline) */
        ProcFunc_l* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == WAIT_ACTION_ADDRESS;
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = daNpc_Ho_c::ACTION_ENDING;
                callAction(this, cur, nullptr);
            }
            cur->i = -1;
            cur->f = WAIT_ACTION_ADDRESS; /* wait_action */
            cur->d = 0;
            mActionStatus = daNpc_Ho_c::ACTION_STARTING;
            wait_action(nullptr);
        }
        eventInfo.setXyCheckCB(daNpc_ho_XyCheckCB);
        break;
    }
    }
    mAttnBasePos.x = current.pos.x;
    mAttnBasePos.y = current.pos.y;
    mAttnBasePos.z = current.pos.z;
    mAttnBasePos.y = mAttnBasePos.y + 100.0f;
    mEyePos.x = mAttnBasePos.x;
    mEyePos.y = mAttnBasePos.y;
    mEyePos.z = mAttnBasePos.z;
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    cXyz* attPos = &attention_info.position;
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 50.0f;
    attPos->z = mAttnBasePos.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl*>(&dNpc_cyl_src) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    mCylCollisionRadius = 40.0f;
    mtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mCurrAnmIdx = -1; // HD: reset the animation index during initialization.
    return TRUE;
}

/* 022403A8 */
void daNpc_Ho_c::setAttention(bool param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    cXyz* attPos = &attention_info.position;
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 90.0f;
    attPos->z = mAttnBasePos.z;
}

/* 022403FC */
void daNpc_Ho_c::lookBack() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> eye; /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp; /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (mState) {
    case HO_STATE_WAIT_01:
        if (chkFlag(HO_FLAG_00000004)) {
            hoPlayerEyePos(eye, -20.0f);
            temp2->copy(*eye);
            dstPos = temp2;
            tx = current.pos.x;
            tz = current.pos.z;
            ty = eyePos.y;
            headOnlyFollow = true;
        } else {
            desiredYRot = home.angle.y;
        }
        break;
    default:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        hoPlayerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x5DC, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    hoLookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, mMaxHeadTurnVelocity, headOnlyFollow);
    shape_angle.y = current.angle.y;
}

/* 022405D4 */
bool daNpc_Ho_c::wait01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (chkFlag(HO_FLAG_00000010)) {
        mPrevState = mState;
        mState = HO_STATE_TALK_02;
        setAnmStatus();
        mItemNum = 0xFF; /* dItemNo_NONE_e */
    } else if (chkFlag(HO_FLAG_00000001)) {
        mPrevState = mState;
        mState = HO_STATE_TALK_01;
        setAnmStatus();
    } else {
        SetOrder(HO_FLAG_00000001);
        if (dComIfGs_isEventBit(0x1E04) && !hoNightStop()) {
            SetOrder(HO_FLAG_00000004);
        }
    }
    return isMorf();
}

/* 022406E4 */
bool daNpc_Ho_c::talk01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (talk() == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        if (chkFlag(HO_FLAG_00000020)) {
            mState = HO_STATE_PREACH;
            hoOrderChangeEvent(dComIfGp_getLinkPlayer(), this, "HO_PREACH" /* "HO_PREACH" */, 0, 0xFFFF);
            clrFlag(HO_FLAG_00000020);
        } else {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
            clrFlag(HO_FLAG_00000001 | HO_FLAG_00000010);
            mAttentionTimer = 5;
            mCylCollisionRadius = 40.0f;
        }
    }
    return isMorf();
}

/* 022407D8 */
bool daNpc_Ho_c::talk02() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_evmng_ChkPresentEnd()) {
        mState = HO_STATE_TALK_03;
    }
    if (mCylCollisionRadius < 90.0f) {
        mCylCollisionRadius = mCylCollisionRadius + 5.0f;
    }
    return false;
}

/* 02240844 */
bool daNpc_Ho_c::talk03() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (talk() == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
        if (mItemNum != dItemNo_NONE_e) {
            mState = HO_STATE_GIVE_01;
            hoOrderChangeEvent(dComIfGp_getLinkPlayer(), this, "DEFAULT_GIVEITEM" /* "DEFAULT_GIVEITEM" */, 0,
                               0xFFFF);
        } else {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
            clrFlag(HO_FLAG_00000001 | HO_FLAG_00000010);
            mAttentionTimer = 5;
            mCylCollisionRadius = 40.0f;
        }
    }
    return isMorf();
}

/* 0224092C */
bool daNpc_Ho_c::give01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_event_giveItemCut(mItemNum) != 0) {
        mState = HO_STATE_GIVE_02;
    } else {
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x4CA, "0"); /* JUT_ASSERT(1226, NULL) */
    }
    return isMorf();
}

/* 022409D4 */
bool daNpc_Ho_c::give02() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (hoEndCheck("DEFAULT_GIVEITEM" /* "DEFAULT_GIVEITEM" */)) {
        hoOrderChangeEvent(dComIfGp_getLinkPlayer(), this, "DEFAULT_TALK" /* "DEFAULT_TALK" */, 0, 0xFFFF);
        mState = HO_STATE_TALK_03_CONTINUE;
        mItemNum = 0xFF;
        talkInit();
    }
    return isMorf();
}

/* 02240A70 */
bool daNpc_Ho_c::preach() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    setFlag(HO_FLAG_00000004);
    if (hoEndCheck("HO_PREACH" /* "HO_PREACH" */)) {
        hoOrderChangeEvent(dComIfGp_getLinkPlayer(), this, "DEFAULT_TALK" /* "DEFAULT_TALK" */, 0, 0xFFFF);
        mItemNum = 0xFF;
        mState = HO_STATE_TALK_03_CONTINUE;
        talkInit();
    }
    return isMorf();
}

/* 02240B18 */
BOOL daNpc_Ho_c::wait_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == daNpc_Ho_c::ACTION_STARTING) {
        mState = HO_STATE_WAIT_01;
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != daNpc_Ho_c::ACTION_ENDING) {
        chkAttention();
        bool temp;
        switch ((u32)(s32)mState) {
        case HO_STATE_WAIT_01:
            temp = wait01();
            break;
        case HO_STATE_TALK_01:
            temp = talk01();
            break;
        case HO_STATE_TALK_02:
            temp = talk02();
            break;
        case HO_STATE_TALK_03:
            temp = talk03();
            break;
        case HO_STATE_TALK_03_CONTINUE:
            temp = talk03();
            break;
        case HO_STATE_GIVE_01:
            temp = give01();
            break;
        case HO_STATE_GIVE_02:
            temp = give02();
            break;
        case HO_STATE_PREACH:
            temp = preach();
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

/* 0223F5FC */
BOOL daNpc_Ho_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpJoyPendentModel, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, pModelData, mBlinkFrame);
    hoMorfEntry(mpMorf);
    removeTexPattern(pModelData);
    s8 bb = m_jnt.mBackboneJntNum;
    J3DModel_setBaseTRMtx(mpJoyPendentModel, pModel->getAnmMtx(bb));
    if (dComIfGs_getEventReg(0xC0FF) != 0) {
        mDoExt_modelUpdateDL(mpJoyPendentModel);
    }
    // HD: the GameCube shadow draw is absent.
    dSnap_RegistFig(0x60 /* DSNAP_TYPE_NPC_HO */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 0223F2D0 */
BOOL daNpc_Ho_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m_jnt.setParam(200, 0x9C4, -500, -0x9C4, 8000, 8000, -0x9C4, -8000, 0x3E8);
    playTexPatternAnm();
    mAnmEnded = (s8)mpMorf->play(&eyePos, mtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((u32)(s32)mCurrAnmIdx) {
        case 3:
            if (mAnmLoopCount < 2) {
                mAnmLoopCount = mAnmLoopCount + 1;
            } else {
                mAnmLoopCount = 0;
                setAnm(2);
            }
            break;
        case 4:
            if (mAnmLoopCount < 2) {
                mAnmLoopCount = mAnmLoopCount + 1;
            } else {
                mAnmLoopCount = 0;
                setAnm(0);
            }
            break;
        }
    }
    checkOrder();
    callAction(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    shape_angle.y = current.angle.y;
    fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    mtrlSndId = 0;
    void* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* mObjAcch.m_gnd */
    if (!mObjAcch.ChkGroundHit()) {
        mtrlSndId = hoMaterialSound(dComIfG_Bgsp(), gnd);
    }
    tevStr.mRoomNo = hoRoomId(dComIfG_Bgsp(), gnd);
    tevStr.mEnvrIdxOverride = hoPolyColor(dComIfG_Bgsp(), gnd);
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    setCollision();
    return TRUE;
}

/* 0223EFA0 */
BOOL daNpc_Ho_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_resDelete(&mPhs, "Ho" /* "Ho" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 0223EB94 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Ho_c*)i_this)->CreateHeap();
}

/* 0223ED9C */
cPhs_State daNpc_Ho_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* fopAcM_ct(this, daNpc_Ho_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            hoActorCtor(this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = HO_VTABLE;
            hoBtpCtor(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, CYL_VTABLE);
            hoJointCtor(&m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Ho" /* "Ho" */);
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_HO_e:
            mType = 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        actor_status = (actor_status & ~0x3Fu) | 0x28; /* fopAcM_SetStatusMap(this, 0x28) */
        actor_status = actor_status | 0x20;            /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xB7B0)) {
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

/* 0223E814 */
BOOL daNpc_Ho_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(
        "Ho" /* "Ho" */, 0xF /* dRes_INDEX_HO_BDL_HO_e */, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(1576, modelData) */
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x628, "modelData");
    J3DAnmTransform* bck =
        (J3DAnmTransform*)dComIfG_getObjectRes("Ho", 9 /* dRes_INDEX_HO_BCK_HO_WAIT01_e */, gc_tu.safeString);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0,
                                    -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    s8 head = hoNameIndex(modelData->getJointName(), "head" /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(1612, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x64C, "m_jnt.getHeadJntNum() >= 0");
    s8 backbone = hoNameIndex(modelData->getJointName(), "backbone" /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(1617, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x651, "m_jnt.getBackboneJntNum() >= 0");
    J3DModelData* penModelData =
        (J3DModelData*)dComIfG_getObjectRes("Ho", 0xC /* dRes_INDEX_HO_BDL_HO_PEND_e */, gc_tu.safeString);
    if (penModelData == nullptr) /* JUT_ASSERT(1634, penModelData) */
        JUT_ASSERT_fail("d_a_npc_ho.cpp", 0x662, "penModelData");
    mpJoyPendentModel = mDoExt_J3DModel__create(penModelData, 0, 0x11020203);
    if (mpJoyPendentModel.get() == nullptr) {
        return FALSE;
    }
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Ho) */
            J3DModelData* md = J3DModel_getModelData(mpMorf->getModel());
            md->getJointNodePointer(i)->setCallBack(nodeCallBack_Ho);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 0223EF9C */
static cPhs_State daNpc_Ho_Create(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Ho_c*)i_this)->_create();
}

/* 0223EFEC */
static BOOL daNpc_Ho_Delete(daNpc_Ho_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_delete();
}

/* 0223F5F8 */
static BOOL daNpc_Ho_Execute(daNpc_Ho_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_execute();
}

/* 0223F768 */
static BOOL daNpc_Ho_Draw(daNpc_Ho_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_draw();
}

/* 0223F76C */
static BOOL daNpc_Ho_IsDelete(daNpc_Ho_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 02240CE4 */
static void __sinit_d_a_npc_ho_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 02240D78 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 02240D8C */
static void daNpc_Ho_c_dt(daNpc_Ho_c* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        hoCircleDtor(circle(&i_this->mAcchCir), 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        acch(&i_this->mObjAcch)->mActorVtable = gc_tu.objAcchDt[0];
        acch(&i_this->mObjAcch)->mCheckVtable = gc_tu.objAcchDt[1];
        hoAcchDtor(&i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        hoActorDtor(i_this, 0);           /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02240E28 */
static void SafeString_assureTerminationImpl(SafeString*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): TU-local data and named ABI bindings
str 1001A604 "Ho" in 0223E710
str 1001A624 "d_a_npc_ho.cpp" in 0223E710
str 1001A608 "m_head_tex_pattern != (0)" in 0223E710
str 1001A6D4 "Ho" in 0223F0B4
str 1001A6E0 "HO_PREACH" in 022406E4
str 1001A6F0 "DEFAULT_GIVEITEM" in 02240844
str 1001A708 "d_a_npc_ho.cpp" in 0224092C
str 1001A704 "0" in 0224092C
str 1001A718 "DEFAULT_GIVEITEM" in 022409D4
str 1001A72C "DEFAULT_TALK" in 022409D4
str 1001A73C "HO_PREACH" in 02240A70
str 1001A748 "DEFAULT_TALK" in 02240A70
str 1001A6CB "Ho" in 0223EFA0
str 1001A6C8 "Ho" in 0223ED9C
str 1001A638 "Ho" in 0223E814
str 1001A650 "d_a_npc_ho.cpp" in 0223E814
str 1001A644 "modelData" in 0223E814
str 1001A63C "head" in 0223E814
str 1001A660 "m_jnt.getHeadJntNum() >= 0" in 0223E814
str 1001A67C "backbone" in 0223E814
str 1001A688 "m_jnt.getBackboneJntNum() >= 0" in 0223E814
str 1001A6A8 "penModelData" in 0223E814
data l_msgId 104671AC
data l_msg 101F4B5C
data l_bck_ix_tbl 1001A5E0
data l_btp_ix_tbl 1001A574
data msg_anm_table 101BE828
data dNpc_cyl_src 101EA190
ext 02556BC0 BOOL hoNightStop()
ext 0254457C BOOL hoEndCheck(dEvent_manager_c*, const char*)
ext 02007898 BOOL hoTriggerA(s32)
ext 020078BC BOOL hoTriggerB(s32)
ext 024EDFCC bool hoLockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* hoLockonTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* hoActionTarget(dAttention_c*, s32)
ext 0259D54C void hoPlayerEyePos(cXyz*, f32)
ext 0259DED0 void hoLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 025E789C s32 hoBtpInit(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32)
ext 027DF9B0 s8 hoNameIndex(JUTNameTab*, const char*)
ext 024EF130 s8 hoRoomId(dBgS*, void*)
ext 024EEEB8 u8 hoPolyColor(dBgS*, void*)
ext 024EECAC u32 hoMaterialSound(dBgS*, void*)
ext 025E5580 void hoMorfEntry(mDoExt_McaMorf*)
ext 025F795C u16 hoMessageStatus(msg_class*)
ext 025F74D0 void hoSetMessageStatus(msg_class*, u32)
ext 025F7DB0 u32 hoMessageSet(msg_class*, u32, cXyz*)
ext 025D4ED0 void hoActorCtor(fopAc_ac_c*)
ext 025E7820 void hoBtpCtor(void*)
ext 0259DAA0 void hoJointCtor(dNpc_JntCtrl_c*)
ext 02018034 void hoCircleDtor(void*, s32)
ext 024EFD9C void hoAcchDtor(dBgS_ObjAcch*, s32)
ext 025D50BC void hoActorDtor(fopAc_ac_c*, s32)
ext 025D79F4 BOOL hoOrderChangeEvent(fopAc_ac_c*, fopAc_ac_c*, const char*, u16, u16)
*/
// clang-format on
