/**
 * d_a_npc_mk.cpp (WWHD)
 * NPC - Ivan (Outset Island, hide-and-seek)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_mk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_mk.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1001E1E8,
    .objAcch = {0x1001E210, 0x1001E230, 0x1001E220},
    .objAcchDt = {0x1001E220, 0x1001E230},
};

#define MK_VTBL 0x1001E240         /* daNpc_Mk_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_MK_e = 0xAA };

// HD: GHS direct action targets; the descriptor is stored in guest memory.
enum : u32 {
    MK_WAIT_ACTION = 0x02299EAC,
    MK_HIND_ACTION = 0x02299F8C,
    MK_VISIT_ACTION = 0x0229A17C,
    MK_SEEK_ACTION = 0x0229A3C8,
};

/* ---- file statics ---- */
static const char* action_table[17] = {
    "WAIT", "TALK", "HOME", "RUN", "RUN3", "SPEAK", "PARTNER", "JUMP", "JUMP2",
    "JITABATA", "LOOK_P", "HIND", "DISP", "MAKEITEM", "GAMESET", "WARP", "TURN",
};
// Resource indices in GameCube animation order; glue checks the original table bytes.
static s32 l_bck_ix_tbl[12] = {15, 12, 13, 14, 16, 11, 9, 6, 7, 5, 8, 10};
static s32 l_btp_ix_tbl[1] = {22};
static u8 msgAnm_table[9] = {0, 1, 2, 0, 7, 4, 5, 6, 9};
// HD: l_msg is gone; the message is reached through the message manager.
static u32 l_msgId = 0;

static u32 l_messageManager = 0;
static f32 l_clipperFar = 0.0f;

// HD: message manager and ABI adapters for unextended event results.

static inline s32 mk_dComIfGp_evmng_getMyActIdx(s32 staff, u32 names, s32 n, s32 force, s32 p) {
    return mk_getMyActIdx(dComIfGp_getPEvtManager(), staff, names, n, force, p);
}
static inline void* mk_dComIfGp_evmng_getMyXyzP(s32 staff, const char* name) { return dComIfGp_evmng_getMySubstanceP(staff, name, 1); }

static inline u32 mk_dComIfGp_evmng_getEventIdx_r(const char* name, u8 evNo) {
    return mk_getEventIdx(dComIfGp_getPEvtManager(), name, evNo);
}

// HD: message status and selection are owned by the message manager.
static inline u32 msgMgr() { return l_messageManager; }

static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    return reinterpret_cast<MkTexPatternView*>(p)->getFrameMax();
}

static inline f32 mDoLib_clipper_getFar() { return l_clipperFar; }

/* GHS pointer-to-member dispatch, including virtual action descriptors. */
static inline void pmf_call(daNpc_Mk_c* self, daNpc_Mk_c::ActionFunc* pmf, void* arg) {
    (self->*(*pmf))(arg);
}

bool daNpc_Mk_c::isMorf() { return mpMorf->isMorf(); }

// HD: keep descriptor writes in GHS order: index, target, then this adjustment.
void daNpc_Mk_c::setAction(u32 actionFunc) {
    daNpc_Mk_c::ActionFunc* cur = &mCurrActionFunc;
    bool callOld = true;
    s16 i = cur->i;
    if (i == -1) {
        if (cur->d == 0 && cur->f == actionFunc)
            return;
    } else if (i == 0) {
        callOld = false; /* NULL */
    }
    if (callOld) {
        mActionStatus = ACTION_ENDING;
        pmf_call(this, cur, nullptr);
    }
    cur->i = -1;
    cur->f = actionFunc;
    cur->d = 0;
    mActionStatus = ACTION_STARTING;
    (this->*(*cur))(nullptr);
}

/* dist = pLink->current.pos - current.pos; dist.abs2XZ() */
static f32 mk_abs2XZ(fopAc_ac_c* a, fopAc_ac_c* b) {
    gabi::Local<cXyz> dist;
    gabi::Local<cXyz> flat;
    cXyz_mi(&a->current.pos, dist, &b->current.pos);
    flat->x = dist->x;
    flat->y = 0.0f;
    flat->z = dist->z;
    return PSVECSquareMag(flat);
}

// HD: the inlined sound call has no actor null check.
static inline void mk_seStart(fopAc_ac_c* a, u32 id) {
    mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

/* 022955EC */
static BOOL nodeCallBack_Mk(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Mk_c* i_this = gabi::at<daNpc_Mk_c>(model->mUserArea);
        u32 jntNo = ((J3DJoint*)node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]); /* getHead_y() */
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[0][0]); /* getHead_x() */
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
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]); /* getBackbone_y() */
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]); /* getBackbone_x() */
            }
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            mtx_copy(model->getAnmMtx(jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* 02295870 */
BOOL daNpc_Mk_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    // HD: l_btp_ix_tbl[0] (not indexed with mTexPatternIdx)
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes("Mk" , l_btp_ix_tbl[0]);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(373, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x175, "m_head_tex_pattern != (0)");
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 02296810 */
void daNpc_Mk_c::playTexPatternAnm() {
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= frameMax0) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = mBlinkFrame + 1;
        }
    }
}

/* 022968D4 */
void daNpc_Mk_c::setAnm(s8 newAnmIdx) {
    f32 morf = 8.0f;
    if (newAnmIdx == 5 && mAnmIdx == 11) {
        morf = 0.0f;
    }
    if (newAnmIdx != mAnmIdx && newAnmIdx != -1) {
        mAnmIdx = newAnmIdx;
        mAnmTimer = 0.0f;
        dNpc_setAnm(mpMorf, -1 /* EMode_NULL */, morf, 1.0f, l_bck_ix_tbl[newAnmIdx] /* l_bck_ix_tbl */, -1,
                    "Mk" );
    }
}

/* 0229706C */
void daNpc_Mk_c::setAnmStatus() {
    switch ((u32)(s32)mState) {
    case STATE_DEMO01:
        setAnm(2);
        break;
    case STATE_CLIMB01:
    case STATE_RUNAWAY:
        setAnm(6);
        break;
    case STATE_JITANDA02:
        break;
    default:
        setAnm(0);
        break;
    }
}

/* 022970AC */
bool daNpc_Mk_c::chkAttentionLocal() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (chkFlag(0x1)) {
        return true;
    }
    if (mAttentionTimer != 0) {
        mAttentionTimer = mAttentionTimer - 1;
        return true;
    }
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}

/* 0229718C */
void daNpc_Mk_c::chkAttention() {
    bool temp = chkFlag(0x4);
    if (chkAttentionLocal()) {
        setFlag(0x4);
    } else {
        clrFlag(0x4);
    }
    if (temp != chkFlag(0x4) && temp == true) {
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
}

/* 022969EC */
void daNpc_Mk_c::eventOrder() {
    if (ChkOrder(3)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (ChkOrder(2)) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022969A0 */
void daNpc_Mk_c::checkOrder() {
    if (eventInfo.mCommand == 1 /* eventInfo.checkCommandTalk() */ && ChkOrder(3)) {
        setFlag(1);
        talkInit();
    }
    ClrOrder();
}

/* 02297210 */
u32 daNpc_Mk_c::next_msgStatus(be<u32>* pMsgNo) { /* u16 */
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x26BD:
    case 0x26C5:
    case 0x26C6:
    case 0x26C7:
    case 0x26C8:
    case 0x26C9:
    case 0x26CB:
    case 0x26CD:
    case 0x26D0:
    case 0x26D1:
    case 0x26D3:
    case 0x26D4:
    case 0x26D8:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x26CA:
        if (mMsgSelectNum == 0) {
            setFlag(0x8);
            msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        } else {
            field_0x6A4 = 1;
            *pMsgNo = 0x26CB;
        }
        break;
    default:
        msgStatus = 0x10;
        break;
    }
    return msgStatus;
}

/* 022972C8 */
u32 daNpc_Mk_c::getMsg() {
    u32 msgNo = 0;
    clrFlag(0x8);
    switch ((u32)(s32)mType) {
    case TYPE_NORMAL:
        if (mState == STATE_DEMO01 || mState == STATE_DEMO02 || mState == STATE_DEMO03) {
            msgNo = mMsgNo;
        } else if (dComIfGs_isTmpBit(0x0040)) {
            msgNo = 0x26B5;
        } else if (dComIfGs_isEventBit(0x1340)) {
            msgNo = 0x26AD;
        } else if (dComIfGs_isEventBit(0x1380)) {
            msgNo = 0x26C5;
        } else if (dComIfGs_isEventBit(0x1210)) {
            msgNo = 0x26B9;
        } else {
            msgNo = 0x26BD;
            dComIfGs_onEventBit(0x1210);
        }
        break;
    case TYPE_MINIGAME:
        if (dComIfGs_isTmpBit(0x0020)) {
            msgNo = 0x26B5;
        } else {
            msgNo = 0x26B1;
            dComIfGs_onTmpBit(0x0020);
        }
        break;
    }
    return msgNo;
}

/* 02296A14 */
void daNpc_Mk_c::setCollision() {
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    f32 cylCollisionRadius = 40.0f;
    f32 height = 80.0f;
    mCyl.SetC(centerPos);
    mCyl.SetR(cylCollisionRadius);
    mCyl.SetH(height);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 02297404 */
void daNpc_Mk_c::msgAnm(u8 param_1) {
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = param_1;
        // HD: message animation indices come from a table except the whistle.

        if (param_1 == 3) {
            mDoAud_seStart_noPos(0x8F0 /* JA_SE_START_WHISTLE */);
            setAnm(3);
        } else if (param_1 <= 8) {
            setAnm(msgAnm_table[param_1]);
        }
    }
}

/* 0229698C */
void daNpc_Mk_c::talkInit() {
    mTalkState = TALK_INIT;
    mMsgAnmIdx = 0xFF;
}

/* 022974AC */
void daNpc_Mk_c::msgPushButton() {
    u32 mgr = msgMgr();
    if ((s32)mCurrMsgNo != 0x26CA) {
        return;
    }
    mMsgSelectNum = (u8)gabi::at<MkMessageManager>(mgr)->mSelectNum; // HD: l_msg->mSelectNum
}

/* 022974CC */
u16 daNpc_Mk_c::talk() {
    // HD: the message is the message manager (GameCube: l_msg found by l_msgId)
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    if (mTalkState == TALK_INIT) {
        l_msgId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = TALK_MSG_CREATE;
    } else if (mTalkState == TALK_FINISHED) {
        msgStatus = 0xFE;
    } else if (l_msgId == 0xFFFFFFFF) {
        l_msgId = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
    } else {
        if (!chkFlag(0x100)) {
            msgAnm(dComIfGp_getMesgAnimeAttrInfo());
        }
        switch ((u32)(s32)mTalkState) {
        case 1:
            mTalkState = TALK_ACTIVE; // HD: no fopMsgM_SearchByID
            break;
        case 2:
            msgStatus = msgMgr_getStatus(mgr);
            if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                msgPushButton();
                msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                if (msgMgr_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                    msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                }
            } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                msgMgr_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                mTalkState = TALK_FINISHED;
            }
            break;
        }
    }
    return msgStatus;
}

/* 022985D0 */
BOOL daNpc_Mk_c::checkDemoStart() {
    mStaffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mpName), nullptr, 0);
    if (mStaffIdx != -1) {
        mEventAction = getNowEventAction();
        return TRUE;
    }
    return FALSE;
}

/* 02298584 */
s32 daNpc_Mk_c::getNowEventAction() {

    return mk_dComIfGp_evmng_getMyActIdx(mStaffIdx, gabi::ea(action_table), 17, FALSE, 0);
}

/* 02295C30 */
BOOL daNpc_Mk_c::init() {
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e;  /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9; /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    gravity = -3.3f; // HD: GameCube -30.0
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    mStts.Init(0xA0, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    actor_status = (actor_status & ~0x3Fu) | 0x27;
    switch ((u32)(s32)mType) {
    case TYPE_NORMAL:
        if (dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_HIND_ACTION);
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        } else {
            setAction(MK_VISIT_ACTION);
            if (dComIfGs_isTmpBit(0x0580)) {
                actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            }
        }
        dNpc_PathRun_setInf(&field_0x688, getPath(), fopAcM_GetRoomNo(this), true);
        if (field_0x688.mPath.get() == nullptr) {
            return FALSE;
        }
        dNpc_PathRun_setInfDrct(&field_0x690, dNpc_PathRun_nextPath(&field_0x688, fopAcM_GetRoomNo(this)));
        if (field_0x690.mPath.get() == nullptr) {
            return FALSE;
        }
        dNpc_PathRun_setInfDrct(&field_0x698, dNpc_PathRun_nextPath(&field_0x690, fopAcM_GetRoomNo(this)));
        if (field_0x698.mPath.get() == nullptr) {
            return FALSE;
        }
        break;
    case TYPE_MINIGAME:
        mStts.m_weight = 0xFE; /* SetWeight */
        dNpc_PathRun_setInf(&field_0x688, getPath(), fopAcM_GetRoomNo(this), true);
        if (field_0x688.mPath.get() == nullptr) {
            return FALSE;
        }
        if (dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_SEEK_ACTION);
        } else {
            setAction(MK_HIND_ACTION);
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        }
        actor_status = actor_status | 0x40; /* fopAcM_OnStatus(this, fopAcStts_UNK40_e) */
        break;
    default:
        setAction(MK_WAIT_ACTION);
        break;
    }
    mAttnBasePos.copy(current.pos);
    mEyePos.copy(current.pos);
    if (mDoLib_clipper_getFar() > 1.0f) {
        cullSizeFar = 5000.0f / mDoLib_clipper_getFar(); /* fopAcM_setCullSizeFar */
    }
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}

/* 02295C18 */
u8 daNpc_Mk_c::getType() {
    return fopAcM_GetParam(this) & 0xFF;
}

/* 02295C24 */
u8 daNpc_Mk_c::getPath() {
    return fopAcM_GetParam(this) >> 8 & 0xFF;
}

/* 02297990 */
u8 daNpc_Mk_c::nextVisitMode() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    switch (mVisitMode) {
    case VISIT_START:
        return VISIT_WALK_PATH;
    case VISIT_TALK:
        if (field_0x6A4 != 0) {
            return VISIT_WALK_PATH_FAST;
        }
        return VISIT_RUN_LINK;
    case VISIT_WALK_PATH:
        if (mk_abs2XZ(pLink, this) < 160000.0f && dNpc_PathRun_chkInside(&field_0x698, &pLink->current.pos)) {
            return VISIT_NOTICE_LINK;
        }
        return VISIT_WALK_PATH;
    case VISIT_RUN_LINK:
        if (!dNpc_PathRun_chkInside(&field_0x690, &pLink->current.pos)) {
            return VISIT_LEFT_PATH;
        }
        if (mk_abs2XZ(pLink, this) < 22500.0f) {
            return VISIT_REACHED_LINK;
        }
        return VISIT_RUN_LINK;
    case VISIT_WALK_AROUND_LINK:
    case VISIT_REACHED_LINK:
        if (!dNpc_PathRun_chkInside(&field_0x690, &pLink->current.pos)) {
            return VISIT_LEFT_PATH;
        }
        if (mk_abs2XZ(pLink, this) > 32400.0f) {
            return VISIT_RUN_LINK;
        }
        if (mVisitMode == VISIT_REACHED_LINK) {
            if (mRunAroundLinkTimer != 0) {
                mRunAroundLinkTimer = mRunAroundLinkTimer - 1;
            } else {
                return VISIT_WALK_AROUND_LINK;
            }
        }
        return mVisitMode;
    case VISIT_NOTICE_LINK:
    case VISIT_LEFT_PATH:
        if (mWaitTimer != 0) {
            mWaitTimer = mWaitTimer - 1;
            break;
        }
        if (mk_abs2XZ(pLink, this) < 160000.0f && dNpc_PathRun_chkInside(&field_0x698, &pLink->current.pos)) {
            return VISIT_RUN_LINK;
        }
        return VISIT_WALK_PATH;
    case VISIT_WALK_PATH_FAST:
        if (field_0x6A4 != 0) {
            field_0x6A4 = field_0x6A4 - 1;
            break;
        }
        return VISIT_WALK_PATH_IGNORE_LINK;
    case VISIT_WALK_PATH_IGNORE_LINK:
        if (field_0x6A4 != 0) {
            field_0x6A4 = field_0x6A4 - 1;
            break;
        }
        return VISIT_WALK_PATH;
    }
    return mVisitMode;
}

/* 02297D78 */
void daNpc_Mk_c::visitInit(u8 i_nextVisitMode) {
    switch (i_nextVisitMode) {
    case VISIT_WALK_PATH:
        dNpc_PathRun_setNearPathIndx(&field_0x688, &current.pos, 100.0f);
        setAnm(4);
        break;
    case VISIT_RUN_LINK:
        setAnm(5);
        if (mVisitMode == 6) {
            mTimerToReachLink = 55;
        } else {
            mTimerToReachLink = 0;
        }
        break;
    case VISIT_WALK_AROUND_LINK:
        setAnm(4);
        break;
    case VISIT_REACHED_LINK:
        mRunAroundLinkTimer = mTimerToReachLink;
        setAnm(5);
        break;
    case VISIT_NOTICE_LINK:
        setAnm(0);
        speedF = 0.0f;
        mWaitTimer = 0xF;
        break;
    case VISIT_LEFT_PATH:
        setAnm(0);
        speedF = 0.0f;
        mWaitTimer = 0x19;
        break;
    case VISIT_WALK_PATH_FAST:
        dNpc_PathRun_setNearPathIndx(&field_0x688, &current.pos, 100.0f);
        setAnm(5);
        field_0x6A4 = 0x64;
        break;
    case VISIT_WALK_PATH_IGNORE_LINK:
        setAnm(4);
        field_0x6A4 = 0x258;
        break;
    default: /* JUT_ASSERT(1113, 0) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x459, "0");
        break;
    }
    mVisitMode = i_nextVisitMode;
}

/* 02298058 */
void daNpc_Mk_c::visitProc() {
    switch (mVisitMode) {
    case VISIT_WALK_PATH:
    case VISIT_WALK_PATH_IGNORE_LINK:
        walkPath(0);
        break;
    case VISIT_WALK_PATH_FAST:
        walkPath(2);
        break;
    case VISIT_RUN_LINK:
        runLink();
        if (mTimerToReachLink < 55) {
            mTimerToReachLink = mTimerToReachLink + 1;
        }
        break;
    case VISIT_WALK_AROUND_LINK:
    case VISIT_REACHED_LINK:
        aroundLink();
        break;
    case VISIT_NOTICE_LINK:
    case VISIT_LEFT_PATH:
        setFlag(0x20);
        break;
    }
}

/* 0229814C */
void daNpc_Mk_c::runawayInit() {
    switch (mMkStatic.m0) {
    case 1:
    case 2:
    case 5:
        if (mAnmIdx == 6) {
            mk_seStart(this, 0x4905 /* JA_SE_CV_TR_KO_A_FOUND */);
        }
        speedF = 0.0f;
        setAnm(5);
        break;
    case 3:
        speedF = 0.0f;
        setAnm(6);
        break;
    case 4:
        speed.y = 25.0f;
        gravity = -3.3f;
        speedF = 8.0f;
        setFlag(0x40);
        setAnm(9);
        mState = STATE_JUMP;
        current.angle.y = field_0x6F0;
        mk_seStart(this, 0x4907 /* JA_SE_CV_TR_KO_A_CAUGHT */);
        break;
    }
}

/* 02297F40 */
void daNpc_Mk_c::walkPath(u8 param_1) {
    if (mMkStatic.walkPath(this, &field_0x688, param_1)) {
        setFlag(0x80);
    }
}

/* 02297F90 */
void daNpc_Mk_c::runLink() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> temp;
    *a = current.pos.get();
    *b = pLink->current.pos.get();
    dNpc_calc_DisXZ_AngY(a, b, nullptr, temp);
    cLib_addCalcAngleS2(&current.angle.y, *temp, 8, 0x800);
}

/* 02298018 */
void daNpc_Mk_c::aroundLink() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    mMkStatic.aroundWalk(this, pLink, mRunAroundLinkTimer);
}

/* 02298654 */
void daNpc_Mk_c::remove_Um2() {
    fopAc_ac_c* ac = mk_fopAcM_searchFromName("Um2" , 0, 0);
    if (ac != nullptr) {
        ac->mParameters = ac->mParameters | 0x80000000; /* fopAcM_SetParam */
        ac->actor_status = ac->actor_status | 0x800;     /* fopAcM_OnStatus(ac, fopAcStts_UNK800_e) */
    }
}

/* 022986A4 */
void daNpc_Mk_c::demoInitCom() {
    void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "RM_Um2" );
    if (a_intP != nullptr) {
        remove_Um2();
    }
    a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "SOUND" );
    if (a_intP != nullptr) {
        switch ((u32)*static_cast<be<u32>*>(a_intP)) {
        case 1:
            mk_seStart(this, 0x4904 /* JA_SE_CV_TR_KO_A_RUN_AWAY */);
            break;
        case 2:
            mk_seStart(this, 0x4905 /* JA_SE_CV_TR_KO_A_FOUND */);
            break;
        }
    }
}

/* 022987B0 */
bool daNpc_Mk_c::demoProc() {
    BOOL temp = FALSE;
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        switch ((u32)mEventAction) {
        case 0:
        case 1:
        case 10:
            speedF = 0.0f;
            setAnm(0);
            break;
        case 2:
            current.angle.y = home.angle.y;
            old.pos.copy(home.pos);
            current.pos.copy(home.pos);
            speedF = 0.0f;
            break;
        case 3:
        case 4: {
            if (mAnmIdx == 5) {
                speedF = 12.0f;
            } else if (mAnmIdx == 11) {
                speedF = 6.0f;
                setAnm(5);
            } else {
                setAnm(5);
            }
            cXyz* a_xyz = (cXyz*)mk_dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos" );
            if (a_xyz == nullptr) /* JUT_ASSERT(1306, a_xyz) */
                JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x51A, "a_xyz");
            field_0x6D0.copy(current.pos);
            field_0x6DC.copy(*a_xyz);
            void* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Timer" );
            if (pTimer != nullptr) {
                field_0x6E8 = static_cast<MkEventInteger*>(pTimer)->mLowHalf; /* (s16)*pTimer */
            } else {
                field_0x6E8 = -1;
            }
            break;
        }
        case 5: {
            talkInit();
            dComIfGp_event_setTalkPartner(this);
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "MsgNo" );
            if (a_intP == nullptr) /* JUT_ASSERT(1320, a_intP) */
                JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x528, "a_intP");
            mMsgNo = (u32)*static_cast<be<u32>*>(a_intP);
            temp = TRUE;
            break;
        }
        case 6:
            dComIfGp_event_setTalkPartner(this);
            break;
        case 8:
            setFlag(0x2 | 0x40);
            gravity = -3.3f;
            speed.y = 20.0f;
            speedF = 8.0f;
            setAnm(10);
            break;
        case 7:
            setFlag(0x2 | 0x40);
            gravity = -2.1f;
            speed.y = 30.0f;
            speedF = 10.0f;
            setAnm(10);
            break;
        case 9:
            setFlag(0x2);
            setAnm(7);
            break;
        case 11:
            speedF = 0.0f;
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
            mPrevState = 13;
            break;
        case 12:
            /* mCurrActionFunc = NULL; setAction(&daNpc_Mk_c::visit_action, NULL) */
            mCurrActionFunc.i = -1;
            mCurrActionFunc.f = MK_VISIT_ACTION;
            mCurrActionFunc.d = 0;
            mActionStatus = ACTION_STARTING;
            (this->*mCurrActionFunc)(nullptr);
            clrFlag(0x10);
            actor_status = actor_status | 0x20; /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
            mPrevState = mState;
            mState = STATE_DEMO01;
            break;
        case 13: {
            s32 itemID = mk_fopAcM_createItemForPresentDemo(&current.pos, 7 /* dItemNo_HEART_PIECE_e */, 0, -1, -1, nullptr, nullptr);
            if (itemID != -1) {
                dComIfGp_event_setItemPartnerId(itemID);
            }
            dComIfGs_onEventBit(0x1340);
            break;
        }
        case 14:
            dComIfGs_offTmpBit(0x0040);
            dComIfGs_onTmpBit(0x0120);
            break;
        case 15: {
            speedF = 0.0f;
            cXyz* a_xyz = (cXyz*)mk_dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos" );
            if (a_xyz == nullptr) /* JUT_ASSERT(1386, a_xyz) */
                JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x56A, "a_xyz");
            current.pos.copy(*a_xyz);
            old.pos.copy(*a_xyz);
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Angle" );
            if (a_intP == nullptr) /* JUT_ASSERT(1392, a_intP) */
                JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x570, "a_intP");
            current.angle.y = (s16)(s32)*static_cast<be<s32>*>(a_intP);
            shape_angle.y = (s16)(s32)*static_cast<be<s32>*>(a_intP);
            break;
        }
        case 16: {
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Angle" );
            if (a_intP == nullptr) /* JUT_ASSERT(1398, a_intP) */
                JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x576, "a_intP");
            field_0x6F0 = (s16)(s32)*static_cast<be<s32>*>(a_intP);
            break;
        }
        }
    }
    switch ((u32)mEventAction) {
    case 3:
    case 4: {
        gabi::Local<cXyz> from, to;
        gabi::Local<be<f32>> temp2;
        gabi::Local<be<s16>> temp3;
        *from = current.pos.get();
        *to = field_0x6DC.get();
        dNpc_calc_DisXZ_AngY(from, to, temp2, temp3);
        if (*temp2 < speedF || mMkStatic.chkPointPass(&field_0x6D0, &field_0x6DC, &current.pos) != 0) {
            if (mEventAction == 4) {
                current.pos.x = field_0x6DC.x;
                current.pos.z = field_0x6DC.z;
                speedF = 0.0f;
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            cLib_addCalcAngleS(&current.angle.y, *temp3, 8, 0x1000, 0x400);
            cLib_chaseF(&speedF, 12.0f, 1.1f);
            if (field_0x6E8 < 0) {
                return true;
            }
            if (field_0x6E8 > 0) {
                s16 t = (s16)(field_0x6E8 - 1);
                field_0x6E8 = t;
                if (t != 0) {
                    return true;
                }
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return true;
    }
    case 5: {
        u16 temp4 = talk();
        if (temp4 == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp4 == 0xFE) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        setFlag(0x20);
        break;
    }
    case 2:
    case 6:
    case 15:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return true;
    case 1:
        setFlag(0x20);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 10:
        setFlag(0x200);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 7:
    case 8:
        if (!chkFlag(0x40)) {
            setAnm(11);
            gravity = -3.3f; // HD: GameCube -30.0
            speedF = 0.0f;
            if (chkFlag(0x2)) {
                clrFlag(0x2);
                if (mEventAction == 7) {
                    gabi::Local<cXyz> up;
                    up->x = 0.0f;
                    up->y = 1.0f;
                    up->z = 0.0f;
                    dComIfGp_getVibration_StartShock(4, -0x21, up);
                }
            } else if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
            }
        }
        return true;
    case 9:
        if (chkFlag(0x2)) {
            if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
                clrFlag(0x2);
            }
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    case 16:
        if (cLib_addCalcAngleS(&current.angle.y, field_0x6F0, 8, 0x1000, 0x400) == 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    if (temp) {
        return true;
    }
    return isMorf();
}

/* 0229764C */
void daNpc_Mk_c::setAttention(bool param_1) {
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 45.0f;
    attPos->z = mAttnBasePos.z;
}

/* 022976A0 */
u8 daNpc_Mk_c::getLookBackMode() {
    if (chkFlag(0x200)) {
        return 3;
    }
    if (chkFlag(0x20)) {
        return 0;
    }
    if (field_0x6A4 != 0) {
        return 2;
    }
    if (chkFlag(0x400) || mAnmIdx == 7 || mAnmIdx == 8) {
        return 2;
    }
    if (chkFlag(0x4)) {
        return 1;
    }
    if (mState == STATE_VISIT && (mVisitMode == VISIT_WALK_AROUND_LINK || mVisitMode == VISIT_REACHED_LINK)) {
        return 1;
    }
    return 2;
}

/* 0229772C */
void daNpc_Mk_c::lookBack() {
    gabi::Local<cXyz> eye;   /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;  /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (getLookBackMode()) {
    case 0:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
        break;
    case 1:
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
        headOnlyFollow = true;
        break;
    case 2:
        /* desiredYRot = current.angle.y (the value read on entry) */
        headOnlyFollow = true;
        break;
    case 3: {
        fopAc_ac_c* pLink = mk_fopAcM_getTalkEventPartner(dComIfGp_getLinkPlayer());
        if (pLink == nullptr) {
            pLink = dComIfGp_getLinkPlayer();
        }
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        temp2->copy(pLink->eyePos);
        dstPos = temp2;
        tz = current.pos.z;
        tx = current.pos.x;
        ty = eyePos.y;
        break;
    }
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x5DC, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, mMaxHeadTurnVelocity, headOnlyFollow);
}

/* 022982B4 */
bool daNpc_Mk_c::wait01() {
    if (chkFlag(0x1)) {
        mPrevState = mState;
        mState = STATE_TALK01;
        setAnmStatus();
    } else {
        SetOrder(0x1);
    }
    return isMorf();
}

/* 02298348 */
bool daNpc_Mk_c::talk01() {
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (chkFlag(0x8)) {
            u32 idx = mk_dComIfGp_evmng_getEventIdx_r("MK_GAMESTART" , 0xFF);
            mEventIdx = (s16)idx;
            mk_fopAcM_orderChangeEventId(this, idx, 0, 0xFFFF);
            mState = STATE_DEMO02;
            setFlag(0x400);
            dComIfGs_onTmpBit(0x0040);
            dComIfGs_onEventBit(0x2201);
        } else {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
            clrFlag(0x1);
            mAttentionTimer = 5;
        }
    }
    setFlag(0x20);
    return isMorf();
}

/* 02298484 */
bool daNpc_Mk_c::talk02() {
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (!mMkStatic.chkGameSet()) {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
        } else {
            u32 idx = mk_dComIfGp_evmng_getEventIdx_r("MK_GAMESET" , 0xFF);
            mEventIdx = (s16)idx;
            mk_fopAcM_orderChangeEventId(this, idx, 0, 0xFFFF);
            mDoAud_seStart_noPos(0x8F1 /* JA_SE_END_WHISTLE */);
            mState = STATE_DEMO02;
            setFlag(0x400);
        }
        clrFlag(0x1);
        mAttentionTimer = 5;
    }
    setFlag(0x20);
    return isMorf();
}

/* 02299358 */
u8 daNpc_Mk_c::visitTalkInit() {
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        mEventIdx = eventInfo.getEventId();
        mState = STATE_DEMO03;
        demo03();
        dComIfGs_onEventBit(0x1E04);
        return 12;
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23 /* dItemNo_PICTO_BOX_e */) &&
        !dComIfGs_checkGetItem(0x26 /* dItemNo_DELUXE_PICTO_BOX_e */)) {
        mEventIdx = eventInfo.getEventId();
        mState = STATE_DEMO03;
        demo03();
        dComIfGs_onEventBit(0x1602);
        return 12;
    }
    return 1;
}

/* 022994F0 */
void daNpc_Mk_c::visitSetEvent() {
    if (dComIfGs_isEventBit(0x1F80)) {
        if (!dComIfGs_isEventBit(0x1E02)) {
            mk_setEventName(&eventInfo, "MK_TALK3" );
            return;
        }
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23 /* dItemNo_PICTO_BOX_e */) &&
        !dComIfGs_checkGetItem(0x26 /* dItemNo_DELUXE_PICTO_BOX_e */)) {
        mk_setEventName(&eventInfo, "MK_TALK2" );
        return;
    }
    mk_setEventName(&eventInfo, "MK_TALK" );
}

/* 022995C4 */
bool daNpc_Mk_c::visit01() {
    f32 temp = 0.0f; // HD: initialised even when mAnmIdx is neither 4 nor 5.
    if (chkFlag(0x1)) {
        mPrevState = mState;
        mState = visitTalkInit();
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = VISIT_TALK;
        if (dComIfGs_isTmpBit(0x0580)) {
            dComIfGs_offTmpBit(0x0580);
            actor_status = actor_status & ~0x4000u; /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
        }
    } else if (checkDemoStart()) {
        mPrevState = mState;
        mState = STATE_DEMO01;
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = VISIT_TALK;
        demo01();
    } else {
        u8 nextvisitMode = nextVisitMode();
        if (nextvisitMode != mVisitMode) {
            visitInit(nextvisitMode);
        }
        visitProc();
        if (mAnmIdx == 4) {
            temp = 2.5f;
        } else if (mAnmIdx == 5) {
            if (mVisitMode == 5) {
                temp = 9.0f;
            } else {
                temp = 12.0f;
            }
        }
        /* if (speedF > temp) step 2.8 else 1.1 (fsel on temp - speedF) */
        f32 step = (temp - speedF >= 0.0f) ? 1.1f : 2.8f;
        cLib_chaseF(&speedF, temp, step);
        SetOrder(0x1);
        if (mVisitMode == 4 || mVisitMode == 5) {
            fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
            s16 angle = fopAcM_searchActorAngleY(pLink, this);
            angle = (s16)(angle - pLink->shape_angle.y);
            if (angle < 0) {
                angle = (s16)-angle;
            }
            if (angle > 0x1800) {
                mOrderFlags = (u8)(mOrderFlags & ~0x01);
            }
        }
        if (field_0x6A4 != 0) {
            mOrderFlags = (u8)(mOrderFlags & ~0x01);
        }
        if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02) && dComIfGs_isTmpBit(0x0580)) {
            SetOrder(0x2);
            mk_setEventName(&eventInfo, "MK_TALK4" );
        } else if (ChkOrder(0x1)) {
            visitSetEvent();
        }
    }
    return true;
}

/* 022998A8 */
bool daNpc_Mk_c::climb01() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    if (chkFlag(0x40)) {
        return false;
    }
    if (((MkPlayerView*)pLink)->mPlayerFlag & 0x2000) { /* pLink->checkFrontRollCrash() */
        gabi::Local<cXyz> a, b;
        gabi::Local<be<f32>> temp;
        *a = current.pos.get();
        *b = pLink->current.pos.get();
        dNpc_calc_DisXZ_AngY(a, b, temp, nullptr);
        if (*temp < 200.0f) {
            mState = STATE_DROP01;
            u32 idx = mk_dComIfGp_evmng_getEventIdx_r("MK_DROP" , 0xFF);
            mEventIdx = (s16)idx;
            mk_fopAcM_orderOtherEventId_r(this, idx, 0xFF, 0xFFFF, 0, 1);
            dComIfGs_onTmpBit(0x0002);
        }
    }
    return isMorf();
}

/* 022999D4 */
bool daNpc_Mk_c::drop01() {
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mPrevState = STATE_RUNAWAY;
        mState = STATE_DEMO03;
        mMkStatic.init(0x50, 0x12C);
        return demo03();
    }
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    return isMorf();
}

/* 02299A80 */
bool daNpc_Mk_c::runaway() {
    u8 temp = mMkStatic.runAwayProc(this, &field_0x688, &mCyl, &field_0x6F0);
    if (temp != mMkStatic.m0) {
        if ((temp == 1 && mMkStatic.m0 == 2) || (temp == 2 && mMkStatic.m0 == 1)) {
            if (cM_rndF(1.0f) < 0.5f) {
                mk_seStart(this, 0x4906 /* JA_SE_CV_TR_KO_A_TURN */);
            }
        }
        mMkStatic.m0 = temp;
        runawayInit();
    }
    if (temp == 3) {
        setFlag(0x20);
    }
    if (mAnmIdx == 5) {
        cLib_chaseF(&speedF, mMkStatic.getSpeedF(15.0f, 18.0f), 2.8f);
    }
    return true;
}

/* 02299B94 */
bool daNpc_Mk_c::jump() {
    if (!chkFlag(0x40)) {
        setAnm(0);
        gravity = -3.3f; // HD: GameCube -30.0
        speedF = 0.0f;
        if (isMorf()) {
            mState = STATE_JITANDA01;
            setAnm(7);
            SetOrder(0x2);
        }
    }
    return true;
}

/* 02299C28 */
bool daNpc_Mk_c::jitanda01() {
    if (chkFlag(0x1)) {
        setFlag(0x100);
        mPrevState = STATE_JITANDA02;
        mState = STATE_TALK02;
    } else {
        SetOrder(0x2);
    }
    return true;
}

/* 02299C68 */
bool daNpc_Mk_c::jitanda02() {
    if (chkFlag(0x1)) {
        mPrevState = 9;
        mState = STATE_TALK02;
    } else if (checkDemoStart()) {
        mPrevState = mState;
        mState = STATE_DEMO01;
        speedF = 0.0f;
        demo01();
    } else {
        SetOrder(0x1);
    }
    return true;
}

/* 02299490 */
bool daNpc_Mk_c::demo01() {
    if (!checkDemoStart()) {
        mState = mPrevState;
        setAnmStatus();
        return true;
    }
    return demoProc();
}

/* 02299D00 */
bool daNpc_Mk_c::demo02() {
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        dComIfGp_event_reset();
        setAction(MK_HIND_ACTION);
        setFlag(0x10);
        actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        clrFlag(0x1);
        clrFlag(0x400);
        return true;
    }
    if (!checkDemoStart()) { /* JUT_ASSERT(2074, NULL) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x81A, "0");
    }
    return demoProc();
}

/* 02299290 */
bool daNpc_Mk_c::demo03() {
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mPrevState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x1);
        mAttentionTimer = 5;
        return true;
    }
    if (!checkDemoStart()) { /* JUT_ASSERT(2093, NULL) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0x82D, "0");
    }
    return demoProc();
}

/* 02299EAC */
BOOL daNpc_Mk_c::wait_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        mState = STATE_WAIT;
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x200 | 0x20);
        bool temp;
        switch ((u32)(s32)mState) {
        case STATE_WAIT:
            temp = wait01();
            break;
        case STATE_TALK01:
            temp = talk01();
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

/* 0229A3C8 */
BOOL daNpc_Mk_c::seek_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x0002)) {
            mState = STATE_RUNAWAY;
            mMkStatic.init(0x50, 0x12C);
            mMkStatic.setRndPathPos(this, &field_0x688);
        } else {
            mState = STATE_CLIMB01;
        }
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x200 | 0x20);
        bool temp;
        switch ((u32)(s32)mState) {
        case STATE_CLIMB01:
            temp = climb01();
            break;
        case STATE_DROP01:
            temp = drop01();
            break;
        case STATE_RUNAWAY:
            temp = runaway();
            break;
        case STATE_JUMP:
            temp = jump();
            break;
        case STATE_JITANDA01:
            temp = jitanda01();
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        case STATE_DEMO01:
            temp = demo01();
            break;
        case STATE_DEMO02:
            temp = demo02();
            break;
        case STATE_DEMO03:
            temp = demo03();
            break;
        case STATE_TALK02:
            temp = talk02();
            break;
        case STATE_13:
            temp = false;
            setAction(MK_HIND_ACTION);
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        if (chkFlag(0x40)) {
            temp = true;
        }
        setAttention(temp);
    }
    return TRUE;
}

/* 02299F8C */
BOOL daNpc_Mk_c::hind_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
    } else if (mActionStatus != ACTION_ENDING) {
        clrFlag(0x200 | 0x20);
        if (mType == TYPE_MINIGAME && dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_SEEK_ACTION);
            clrFlag(0x10);
            actor_status = actor_status | 0x20; /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
        }
        if ((mType == TYPE_NORMAL || mType == TYPE_MINIGAME) && checkDemoStart()) {
            bool temp = demoProc();
            lookBack();
            setAttention(temp);
        }
    }
    return TRUE;
}

/* 0229A17C */
BOOL daNpc_Mk_c::visit_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x0040)) {
            mState = STATE_JITANDA02;
            setAnm(7);
            mStts.m_weight = 0xFE; /* SetWeight */
        } else {
            mState = STATE_VISIT;
            setAnmStatus();
            mStts.m_weight = 0xA0; /* SetWeight */
        }
        mActionStatus = mActionStatus + 1;
        mVisitMode = 0;
    } else if (mActionStatus != ACTION_ENDING) {
        bool temp;
        chkAttention();
        clrFlag(0x200 | 0x20);
        switch ((u32)(s32)mState) {
        case STATE_VISIT:
            temp = visit01();
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        case STATE_TALK01:
            temp = talk01();
            break;
        case STATE_TALK02:
            temp = talk02();
            break;
        case STATE_DEMO01:
            temp = demo01();
            break;
        case STATE_DEMO02:
            temp = demo02();
            break;
        case STATE_DEMO03:
            temp = demo03();
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

/* 02296FAC */
BOOL daNpc_Mk_c::_draw() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = pModel->getModelData();
    if (chkFlag(0x10)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, pModelData, mBlinkFrame);
    McaMorf_entry(mpMorf);
    ((MkModelDataView*)pModelData)->mpTexNoAnimator = 0; /* mBtpAnm.remove(pModelData) */
    // HD: no dComIfGd_setShadow
    dSnap_RegistFig(0x61 /* DSNAP_TYPE_NPC_MK */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02296A9C */
BOOL daNpc_Mk_c::_execute() {
    m_jnt.setParam(0, 7000, 0, -7000, 8000, 9000, -2000, -9000, 1000);
    playTexPatternAnm();
    // HD: the walk and run animations play at a rate following the speed (at least 1.0)
    if (mAnmIdx == 4) {
        f32 rate = speedF * 0.4f;
        mpMorf->setPlaySpeed(rate - 1.0f >= 0.0f ? rate : 1.0f);
    } else if (mAnmIdx == 5) {
        f32 rate = speedF * 0.08f;
        mpMorf->setPlaySpeed(rate - 1.0f >= 0.0f ? rate : 1.0f);
    }
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((u32)(s32)mAnmIdx) {
        case 7:
            if (cM_rnd() < 0.4f) {
                setAnm(8);
            }
            break;
        case 8:
            setAnm(7);
            break;
        }
    }
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    if (!chkFlag(0x80)) {
        shape_angle.y = current.angle.y;
    }
    clrFlag(0x80);
    mMtrlSndId = 0;
    if (!chkFlag(0x10)) {
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        void* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* mObjAcch.m_gnd */
        if (mObjAcch.ChkGroundHit()) {
            clrFlag(0x40);
            mMtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
        } else {
            setFlag(0x40);
        }
        tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
        tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), gnd); /* tevStr.mEnvrIdxOverride */
    } else {
        setFlag(0x40);
    }
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    if (chkFlag(0x10)) {
        mCyl.OffCoSPrmBit(1); /* OffCoSetBit */
    } else {
        mCyl.OnCoSPrmBit(1); /* OnCoSetBit */
        setCollision();
    }
    return TRUE;
}

/* 022967C0 */
BOOL daNpc_Mk_c::_delete() {
    dComIfG_resDelete(&mPhs, "Mk" );
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 02295C14 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Mk_c*)i_this)->CreateHeap();
}

/* 022964F8 */
cPhs_State daNpc_Mk_c::_create() {
    /* fopAcM_ct(this, daNpc_Mk_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = MK_VTBL;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            __ct(&mObjAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1001E200);
            mk_constructJntCtrl(&m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Mk" );
    if (state == cPhs_COMPLEATE_e) {
        mpName = gabi::guest_addr("Mk"); /* "Mk" (the same literal) */
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_MK_e:
            switch (getType()) {
            case TYPE_NORMAL:
                mType = TYPE_NORMAL;
                break;
            case TYPE_MINIGAME:
                mType = TYPE_MINIGAME;
                argument = 4;
                mpName = gabi::guest_addr("Mk2");
                break;
            default:
                mType = TYPE_NONE;
                break;
            }
            break;
        default:
            return cPhs_ERROR_e;
        }
        tevStr.mRoomNo = current.roomNo;
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xB7B0)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(this, -35.0f, -10.0f, -35.0f, 35.0f, 100.0f, 35.0f);
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}

/* 02295974 */
BOOL daNpc_Mk_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Mk", dRes_INDEX_MK_BDL_MK_e);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Mk", dRes_INDEX_MK_BCK_MK_WAIT_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "head2" );
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(2603, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0xA2B, "m_jnt.getHeadJntNum() >= 0");
    s8 backbone = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "backbone" );
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(2608, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_mk.cpp", 0xA30, "m_jnt.getBackboneJntNum() >= 0");
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Mk) */
            J3DModelData* md = mpMorf->getModel()->getModelData();
            md->getJointNodePointer(i)->setCallBack(nodeCallBack_Mk);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(60.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mVisitMode = 0;
    mObjAcch.SetGroundCheckOffset(100.0f);
    maxFallSpeed = -90.0f; /* fopAcM_SetMaxFallSpeed */
    return TRUE;
}

/* 022967BC */
static cPhs_State daNpc_Mk_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Mk_c*)i_this)->_create();
}

/* 0229680C */
static BOOL daNpc_Mk_Delete(daNpc_Mk_c* i_this) {
    return i_this->_delete();
}

/* 02296FA8 */
static BOOL daNpc_Mk_Execute(daNpc_Mk_c* i_this) {
    return i_this->_execute();
}

/* 02297060 */
static BOOL daNpc_Mk_Draw(daNpc_Mk_c* i_this) {
    return i_this->_draw();
}

/* 02297064 */
static BOOL daNpc_Mk_IsDelete(daNpc_Mk_c*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 0229A7A4 */
static void __sinit_d_a_npc_mk_cpp() {
    sinit_header_statics(0x10467C6C, 0x101C1FA4);
}

/* 0229A838 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0229A84C */
static void daNpc_Mk_c_dt(daNpc_Mk_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mObjAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0229A8E8 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue (tools/verify/autoglue.py): unit-local bindings and data
data l_messageManager 101F4B5C nocheck
data l_clipperFar 1048D04C nocheck
str 1001E348 in 022964F8
ext 0253E9B0 void mk_setEventName(dEvt_info_c* info, const char* name)
ext 0259DAA0 void mk_constructJntCtrl(dNpc_JntCtrl_c* jnt)
ext 02542EDC s32 mk_getMyActIdx(dEvent_manager_c* manager, s32 staff, u32 names, s32 n, s32 force, s32 p)
ext 02543F10 u32 mk_getEventIdx(dEvent_manager_c* manager, const char* name, u8 number)
data action_table 101C1F60
str 1001E3BC
str 1001E3C4
str 1001E3CC
str 1001E3D4
str 1001E3D8
str 1001E3E0
str 1001E3E8
str 1001E3F0
str 1001E3F8
str 1001E400
str 1001E40C
str 1001E414
str 1001E41C
str 1001E424
str 1001E430
str 1001E438
str 1001E440
str 1001E290 in 02295870
str 1001E2B0 in 02295870
str 1001E294 in 02295870
str 1001E2D8 in 02295974
str 1001E2D0 in 02295974
str 1001E2DC in 02295974
str 1001E2EC in 02295974
str 1001E308 in 02295974
str 1001E314 in 02295974
str 1001E34C in 022964F8
str 1001E34F in 022967C0
str 1001E358 in 022968D4
str 1001E38C in 02297D78
str 1001E388 in 02297D78
str 1001E3A0 in 02298348
str 1001E3B0 in 02298484
str 1001E448 in 02298654
str 1001E454 in 022986A4
str 1001E44C in 022986A4
str 1001E470 in 022987B0
str 1001E49C in 022987B0
str 1001E474 in 022987B0
str 1001E47C in 022987B0
str 1001E484 in 022987B0
str 1001E494 in 022987B0
str 1001E48C in 022987B0
str 1001E4B0 in 02299290
str 1001E4AC in 02299290
str 1001E4C8 in 022994F0
str 1001E4D4 in 022994F0
str 1001E4C0 in 022994F0
str 1001E4EC in 022995C4
str 1001E4FC in 022998A8
str 1001E514 in 02299D00
str 1001E510 in 02299D00
data l_bck_ix_tbl 1001E250
data l_btp_ix_tbl 1001E1E4
data msgAnm_table 1001E36C
data l_msgId 10467C68 nocheck
ext 025D7874 BOOL mk_fopAcM_orderChangeEventId(fopAc_ac_c* a, u32 idx, u16 flag, u16 hind)
ext 025D7A58 BOOL mk_fopAcM_orderOtherEventId_r(fopAc_ac_c* a, u32 idx, u8 tool, u16 p3, u16 prio, u16 flag)
ext 025D9F38 fopAc_ac_c* mk_fopAcM_searchFromName(const char* name, u32 mask, u32 prm)
ext 025D7C6C fopAc_ac_c* mk_fopAcM_getTalkEventPartner(fopAc_ac_c* a)
ext 025D7DEC s32 mk_fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale)
ext 025E1988 void mDoAud_seStart_noPos(u32 id)
ext 025F795C u16 msgMgr_getStatus(u32 m)
ext 025F74D0 void msgMgr_setStatus(u32 m, u32 st)
ext 025F7DB0 u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos)
ext 0259E6D0 void dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd)
ext 0259E744 u32 dNpc_PathRun_nextPath(MkPath_l* p, s8 room)
ext 0259E730 void dNpc_PathRun_setInfDrct(MkPath_l* p, u32 path)
ext 0259F220 BOOL dNpc_PathRun_chkInside(MkPath_l* p, cXyz* pos)
ext 0259EE7C void dNpc_PathRun_setNearPathIndx(MkPath_l* p, cXyz* pos, f32 r)
ext 0229AB24 BOOL daNpc_Mk_Static_c::walkPath(fopAc_ac_c* a, MkPath_l* p, u8 o)
ext 0229AB78 void daNpc_Mk_Static_c::aroundWalk(fopAc_ac_c* a, fopAc_ac_c* t, u8 o)
ext 0229ACC0 f32 daNpc_Mk_Static_c::getSpeedF(f32 a, f32 b)
ext 0229AD60 void daNpc_Mk_Static_c::init(u8 w, u16 d)
ext 0229B164 u8 daNpc_Mk_Static_c::runAwayProc(fopAc_ac_c* a, MkPath_l* p, void* cyl, be<s16>* ang)
ext 0229B54C BOOL daNpc_Mk_Static_c::chkGameSet()
ext 0229B5F0 void daNpc_Mk_Static_c::setRndPathPos(fopAc_ac_c* a, MkPath_l* p)
ext 0229B768 BOOL daNpc_Mk_Static_c::chkPointPass(cXyz* a, cXyz* b, cXyz* c)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool dAttention_LockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 024EF130 s8 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 u8 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 024EECAC u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly)
ext 025E5580 void McaMorf_entry(mDoExt_McaMorf* m)
*/
