/**
 * d_a_npc_ho.cpp (WWHD)
 * NPC - Mrs. Marie (Windfall Island teacher)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_ho.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_ho.h"

#define SAFESTRING_VTBL 0x1001A578 /* this TU's sead::SafeString vtable */
#define HO_VTBL 0x1001A5D0         /* daNpc_Ho_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_HO_e = 0x16E };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: dSv_event_c blocks at *(0x101F84DC) + 0x644 (event) and + 0x1178 (tmp event) */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* dComIfGs_getBeastNum(dBeastIdx_JOY_PENDANT_e): the save byte at *(0x101F84DC) + 0xC3 */
static inline u8 dComIfGs_getBeastNum_JoyPendant() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xC3); }
/* dComIfGp_setItemBeastNumCount(7, n): play + 0x5B7E (s16) += n */
static inline void dComIfGp_setItemBeastNumCount7(s32 n) {
    u32 a = dComIfGp_ea() + 0x5B7E;
    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n));
}
static inline BOOL dKy_checkEventNightStop() { return gabi::call<BOOL>(0x02556BC0); }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline void dComIfGp_evmng_CancelPresent() { gabi::call(0x02544980, dComIfGp_getPEvtManager()); }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_endCheck(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* 0253EB70 dEvt_control_c::giveItemCut(u8) */
static inline BOOL dComIfGp_event_giveItemCut(u8 item) { return gabi::call<BOOL>(0x0253EB70, dComIfGp_getEvent(), item); }
/* 025D79F4 fopAcM_orderChangeEvent(actor, partner, const char* name, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEvent(fopAc_ac_c* a, fopAc_ac_c* b, const char* name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D79F4, a, b, name, flag, hind);
}
static inline BOOL CPad_CHECK_TRIG_A(s32 port) { return gabi::call<BOOL>(0x02007898, port); }
static inline BOOL CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos) -> id; its select number (GameCube msg_class::mSelectNum) at +0x948 */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 msgMgr_getStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
static inline void msgMgr_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
static inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* 025E5580 mDoExt_McaMorf::entry (HD) */
static inline void McaMorf_entry(mDoExt_McaMorf* m) { gabi::call(0x025E5580, m); }

/* GHS pointer to member function call */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
/* l_msgId (HD: l_msg is gone, the message is reached through the message manager) */
static be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x104671AC); }
/* l_bck_ix_tbl[5] (.rodata 0x1001A5E0), l_btp_ix_tbl[1] (0x1001A574), msg_anm_table (0x101BE828) */

bool daNpc_Ho_c::isMorf() { return gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0) < 1.0f; }

/* 0223E47C */
static BOOL nodeCallBack_Ho(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0223E47C, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Ho_c* i_this = gabi::at<daNpc_Ho_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
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
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}
VERIFY(0x0223E47C, nodeCallBack_Ho);

/* 0223E704 */
s16 daNpc_Ho_c::XyCheckCB(int) {
    WWHD_FUNC(0x0223E704, s16, this, 0);
    return 1;
}
VERIFY(0x0223E704, &daNpc_Ho_c::XyCheckCB);

/* 0223E70C */
static s16 daNpc_ho_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0223E70C, s16, i_this, i_itemBtn);
    return ((daNpc_Ho_c*)i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x0223E70C, daNpc_ho_XyCheckCB);

/* 0223FA10 */
void daNpc_Ho_c::receivePendant(int numPendantsGiven) {
    WWHD_FUNC(0x0223FA10, void, this, numPendantsGiven);
    int totalGiven = dComIfGs_getEventReg(0xC0FF /* UNK_C0FF */);
    dComIfGp_setItemBeastNumCount7(-numPendantsGiven);
    totalGiven += numPendantsGiven;
    if (totalGiven > 99) {
        dComIfGp_setItemBeastNumCount7(totalGiven - 99);
        totalGiven = 99;
    }
    dComIfGs_setEventReg(0xC0FF, (u8)totalGiven);
}
VERIFY(0x0223FA10, &daNpc_Ho_c::receivePendant);

/* 0223E710 */
BOOL daNpc_Ho_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    WWHD_FUNC(0x0223E710, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData_l(mpMorf->getModel());
    /* HD: l_btp_ix_tbl[0] (not indexed with mTexPatternIdx) */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1001A604) /* "Ho" */, gabi::load<s32>(0x1001A574), SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(329, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x1001A624), 0x149, STR(0x1001A608));
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x0223E710, &daNpc_Ho_c::initTexPatternAnm);

/* 0223EFF0 */
void daNpc_Ho_c::playTexPatternAnm() {
    WWHD_FUNC(0x0223EFF0, void, this);
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
VERIFY(0x0223EFF0, &daNpc_Ho_c::playTexPatternAnm);

/* 0223F0B4 */
void daNpc_Ho_c::setAnm(s8 newAnmIdx) {
    WWHD_FUNC(0x0223F0B4, void, this, newAnmIdx);
    f32 morf = 8.0f;
    if (newAnmIdx != mCurrAnmIdx && newAnmIdx != -1) {
        mCurrAnmIdx = newAnmIdx;
        mAnmTimer = 0.0f;
        dNpc_setAnm(mpMorf, -1 /* EMode_NULL */, morf, 1.0f, gabi::load<s32>(0x1001A5E0 + 4 * newAnmIdx) /* l_bck_ix_tbl */, -1,
                    STR(0x1001A6D4) /* "Ho" */);
    }
}
VERIFY(0x0223F0B4, &daNpc_Ho_c::setAnm);

/* 0223F774 */
void daNpc_Ho_c::setAnmStatus() {
    WWHD_FUNC(0x0223F774, void, this);
    setAnm(0);
}
VERIFY(0x0223F774, &daNpc_Ho_c::setAnmStatus);

/* 0223F77C */
bool daNpc_Ho_c::chkAttentionLocal() {
    WWHD_FUNC(0x0223F77C, bool, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (chkFlag(HO_FLAG_00000001 | HO_FLAG_00000010)) {
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
VERIFY(0x0223F77C, &daNpc_Ho_c::chkAttentionLocal);

/* 0223F860 */
void daNpc_Ho_c::chkAttention() {
    WWHD_FUNC(0x0223F860, void, this);
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
VERIFY(0x0223F860, &daNpc_Ho_c::chkAttention);

/* 0223F1B8 */
void daNpc_Ho_c::eventOrder() {
    WWHD_FUNC(0x0223F1B8, void, this);
    if (ChkOrder(1)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
    }
    if (ChkOrder(2)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(4)) {
        eventInfo_onCondition(this, 0x20 /* dEvtCnd_CANTALKITEM_e */);
    }
}
VERIFY(0x0223F1B8, &daNpc_Ho_c::eventOrder);

/* 0223F130 */
void daNpc_Ho_c::checkOrder() {
    WWHD_FUNC(0x0223F130, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */ && ChkOrder(7)) {
        if (dComIfGp_event_chkTalkXY()) {
            setFlag(HO_FLAG_00000010);
        } else {
            setFlag(HO_FLAG_00000001);
        }
        talkInit();
    }
    ClrOrder();
}
VERIFY(0x0223F130, &daNpc_Ho_c::checkOrder);

/* 0223F8E4 */
u32 daNpc_Ho_c::next_msg_sub0(u32 msgNo) {
    WWHD_FUNC(0x0223F8E4, u32, this, msgNo);
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
VERIFY(0x0223F8E4, &daNpc_Ho_c::next_msg_sub0);

/* 0223FAB8 */
u32 daNpc_Ho_c::next_msgStatus(be<u32>* pMsgNo) { /* u16 (talk passes r3 on as is) */
    WWHD_FUNC(0x0223FAB8, u32, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
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
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
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
        if (dComIfGs_getBeastNum_JoyPendant() >= 20) {
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
        msgStatus = 0x10;
        break;
    case 0x2742:
        *pMsgNo = 0x2743;
        receivePendant(0x14);
        break;
    case 0x2743:
        mNextMessageId = 0x2744;
        mItemNum = 0x9C; /* dItemNo_CABANA_DEED_e */
        msgStatus = 0x10;
        dComIfGs_onEventBit(0x1C08);
        dComIfGs_onTmpBit(0x0104);
        break;
    case 0x274B:
        *pMsgNo = 0x274D;
        break;
    case 0x274D:
        *pMsgNo = 0x274E;
        receivePendant(dComIfGs_getBeastNum_JoyPendant());
        break;
    case 0x274E:
        if (dComIfGs_getEventReg(0xC0FF) >= 0x28 && !dComIfGs_isEventBit(0x1C04)) {
            *pMsgNo = 0x274F;
        } else {
            msgStatus = 0x10;
        }
        break;
    case 0x2754: {
        *pMsgNo = 0x2755;
        int numPendantsGiven = dComIfGs_getBeastNum_JoyPendant();
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
        msgStatus = 0x10;
        dComIfGs_onEventBit(0x1C04);
        break;
    case 0x2756:
        msgStatus = 0x10;
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
        msgStatus = 0x10;
        break;
    }
    return msgStatus;
}
VERIFY(0x0223FAB8, &daNpc_Ho_c::next_msgStatus);

/* 0223FFB4 */
u32 daNpc_Ho_c::getMsg() {
    WWHD_FUNC(0x0223FFB4, u32, this);
    switch (mState) {
    case HO_STATE_TALK_01:
        if (dKy_checkEventNightStop()) {
            return 0x2722;
        }
        if (!dComIfGs_isEventBit(0x1E01)) {
            dComIfGs_onEventBit(0x1E01);
            return 0x2711;
        }
        return (dComIfGs_getEventReg(0xC0FF) == 0 ? 0 : 1) + 0x2712;
    case HO_STATE_TALK_03:
        if (dComIfGp_event_getPreItemNo() != 0x1F /* dItemNo_JOY_PENDANT_e */ || !dComIfGs_isEventBit(0x1E04)) {
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
VERIFY(0x0223FFB4, &daNpc_Ho_c::getMsg);

/* 0223F230 */
void daNpc_Ho_c::setCollision() {
    WWHD_FUNC(0x0223F230, void, this);
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    f32 cylCollision = mCylCollisionRadius;
    f32 height = 140.0f;
    mCyl.SetC(centerPos);
    mCyl.SetR(cylCollision);
    mCyl.SetH(height);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x0223F230, &daNpc_Ho_c::setCollision);

/* 0223FF20 */
void daNpc_Ho_c::msgPushButton() {
    WWHD_FUNC(0x0223FF20, void, this);
    u32 mgr = msgMgr();
    switch (mCurrMsgNo) {
    case 0x2712:
    case 0x2713:
    case 0x2718:
    case 0x272B:
        mMsgSelectNum = (u8)gabi::load<u32>(mgr + 0x948); /* HD: l_msg->mSelectNum */
        break;
    }
}
VERIFY(0x0223FF20, &daNpc_Ho_c::msgPushButton);

/* 0223FF58 */
void daNpc_Ho_c::msgAnm(u8 param_1) {
    WWHD_FUNC(0x0223FF58, void, this, param_1);
    /* static s8 msg_anm_table[] = {0, 1, 2, 3, 4} (.data 0x101BE828) */
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = param_1;
        if (param_1 < 5) {
            setAnm(gabi::load<s8>(0x101BE828 + param_1));
        }
        mAnmLoopCount = 0;
    }
}
VERIFY(0x0223FF58, &daNpc_Ho_c::msgAnm);

/* 0223F11C */
void daNpc_Ho_c::talkInit() {
    WWHD_FUNC(0x0223F11C, void, this);
    mTalkState = 0; /* TALK_INIT */
    mMsgAnmIdx = 0xFF;
}
VERIFY(0x0223F11C, &daNpc_Ho_c::talkInit);

/* 022401E8 */
u16 daNpc_Ho_c::talk() {
    WWHD_FUNC(0x022401E8, u16, this);
    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    if (mTalkState == 0 /* TALK_INIT */) {
        l_msgId() = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = 1; /* TALK_MSG_CREATE */
    } else if (mTalkState != -1 /* TALK_FINISHED */) {
        if (l_msgId() == 0xFFFFFFFF) {
            l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            if (!chkFlag(HO_FLAG_00000008)) {
                msgAnm(dComIfGp_getMesgAnimeAttrInfo());
            }
            switch ((u32)(s32)mTalkState) {
            case 1:
                mTalkState = 2; /* HD: no fopMsgM_SearchByID */
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
                    mTalkState = -1;
                } else if (msgStatus == 0xF && (CPad_CHECK_TRIG_A(0) || CPad_CHECK_TRIG_B(0))) {
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
VERIFY(0x022401E8, &daNpc_Ho_c::talk);

/* 0223EB98 */
BOOL daNpc_Ho_c::init() {
    WWHD_FUNC(0x0223EB98, BOOL, this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x6F); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x6F); /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    switch (mType) {
    case 0: {
        /* setAction(&daNpc_Ho_c::wait_action, NULL) (inline) */
        ProcFunc_l* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == 0x02240B18;
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = -1; /* ACTION_ENDING */
                pmf_call(this, cur, nullptr);
            }
            cur->i = -1;
            cur->f = 0x02240B18; /* wait_action */
            cur->d = 0;
            mActionStatus = 0; /* ACTION_STARTING */
            wait_action(nullptr);
        }
        gabi::store<u32>(gabi::ea(this) + 0x104, 0x0223E70C); /* eventInfo.setXyCheckCB(daNpc_ho_XyCheckCB) */
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
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 50.0f;
    attPos->z = mAttnBasePos.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    mCylCollisionRadius = 40.0f;
    mtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    mCurrAnmIdx = -1; /* HD */
    return TRUE;
}
VERIFY(0x0223EB98, &daNpc_Ho_c::init);

/* 022403A8 */
void daNpc_Ho_c::setAttention(bool param_1) {
    WWHD_FUNC(0x022403A8, void, this, param_1);
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 90.0f;
    attPos->z = mAttnBasePos.z;
}
VERIFY(0x022403A8, &daNpc_Ho_c::setAttention);

/* 022403FC */
void daNpc_Ho_c::lookBack() {
    WWHD_FUNC(0x022403FC, void, this);
    gabi::Local<cXyz> eye;   /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;  /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (mState) {
    case HO_STATE_WAIT_01:
        if (chkFlag(HO_FLAG_00000004)) {
            dNpc_playerEyePos_l(eye, -20.0f);
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
        dNpc_playerEyePos_l(eye, -20.0f);
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
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, mMaxHeadTurnVelocity, headOnlyFollow);
    shape_angle.y = current.angle.y;
}
VERIFY(0x022403FC, &daNpc_Ho_c::lookBack);

/* 022405D4 */
bool daNpc_Ho_c::wait01() {
    WWHD_FUNC(0x022405D4, bool, this);
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
        if (dComIfGs_isEventBit(0x1E04) && !dKy_checkEventNightStop()) {
            SetOrder(HO_FLAG_00000004);
        }
    }
    return isMorf();
}
VERIFY(0x022405D4, &daNpc_Ho_c::wait01);

/* 022406E4 */
bool daNpc_Ho_c::talk01() {
    WWHD_FUNC(0x022406E4, bool, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (chkFlag(HO_FLAG_00000020)) {
            mState = HO_STATE_PREACH;
            fopAcM_orderChangeEvent(dComIfGp_getLinkPlayer(), this, STR(0x1001A6E0) /* "HO_PREACH" */, 0, 0xFFFF);
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
VERIFY(0x022406E4, &daNpc_Ho_c::talk01);

/* 022407D8 */
bool daNpc_Ho_c::talk02() {
    WWHD_FUNC(0x022407D8, bool, this);
    if (dComIfGp_evmng_ChkPresentEnd()) {
        mState = HO_STATE_TALK_03;
    }
    if (mCylCollisionRadius < 90.0f) {
        mCylCollisionRadius = mCylCollisionRadius + 5.0f;
    }
    return false;
}
VERIFY(0x022407D8, &daNpc_Ho_c::talk02);

/* 02240844 */
bool daNpc_Ho_c::talk03() {
    WWHD_FUNC(0x02240844, bool, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (mItemNum != 0xFF /* dItemNo_NONE_e */) {
            mState = HO_STATE_GIVE_01;
            fopAcM_orderChangeEvent(dComIfGp_getLinkPlayer(), this, STR(0x1001A6F0) /* "DEFAULT_GIVEITEM" */, 0, 0xFFFF);
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
VERIFY(0x02240844, &daNpc_Ho_c::talk03);

/* 0224092C */
bool daNpc_Ho_c::give01() {
    WWHD_FUNC(0x0224092C, bool, this);
    if (dComIfGp_event_giveItemCut(mItemNum) != 0) {
        mState = HO_STATE_GIVE_02;
    } else {
        JUT_ASSERT_fail(STR(0x1001A708), 0x4CA, STR(0x1001A704)); /* JUT_ASSERT(1226, NULL) */
    }
    return isMorf();
}
VERIFY(0x0224092C, &daNpc_Ho_c::give01);

/* 022409D4 */
bool daNpc_Ho_c::give02() {
    WWHD_FUNC(0x022409D4, bool, this);
    if (dComIfGp_evmng_endCheck(STR(0x1001A718) /* "DEFAULT_GIVEITEM" */)) {
        fopAcM_orderChangeEvent(dComIfGp_getLinkPlayer(), this, STR(0x1001A72C) /* "DEFAULT_TALK" */, 0, 0xFFFF);
        mState = HO_STATE_TALK_03_CONTINUE;
        mItemNum = 0xFF;
        talkInit();
    }
    return isMorf();
}
VERIFY(0x022409D4, &daNpc_Ho_c::give02);

/* 02240A70 */
bool daNpc_Ho_c::preach() {
    WWHD_FUNC(0x02240A70, bool, this);
    setFlag(HO_FLAG_00000004);
    if (dComIfGp_evmng_endCheck(STR(0x1001A73C) /* "HO_PREACH" */)) {
        fopAcM_orderChangeEvent(dComIfGp_getLinkPlayer(), this, STR(0x1001A748) /* "DEFAULT_TALK" */, 0, 0xFFFF);
        mItemNum = 0xFF;
        mState = HO_STATE_TALK_03_CONTINUE;
        talkInit();
    }
    return isMorf();
}
VERIFY(0x02240A70, &daNpc_Ho_c::preach);

/* 02240B18 */
BOOL daNpc_Ho_c::wait_action(void*) {
    WWHD_FUNC(0x02240B18, BOOL, this, (void*)nullptr);
    if (mActionStatus == 0 /* ACTION_STARTING */) {
        mState = HO_STATE_WAIT_01;
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != -1 /* ACTION_ENDING */) {
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
VERIFY(0x02240B18, &daNpc_Ho_c::wait_action);

/* 0223F5FC */
BOOL daNpc_Ho_c::_draw() {
    WWHD_FUNC(0x0223F5FC, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData_l(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpJoyPendentModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pModelData, mBlinkFrame);
    McaMorf_entry(mpMorf);
    gabi::store<u32>(gabi::ea(pModelData) + 0x38, 0); /* mBtpAnm.remove(pModelData) */
    s8 bb = m_jnt.mBackboneJntNum;
    J3DModel_setBaseTRMtx(mpJoyPendentModel, getAnmMtx(pModel, bb));
    if (dComIfGs_getEventReg(0xC0FF) != 0) {
        mDoExt_modelUpdateDL(mpJoyPendentModel);
    }
    /* HD: no dComIfGd_setShadow */
    dSnap_RegistFig(0x60 /* DSNAP_TYPE_NPC_HO */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0223F5FC, &daNpc_Ho_c::_draw);

/* 0223F2D0 */
BOOL daNpc_Ho_c::_execute() {
    WWHD_FUNC(0x0223F2D0, BOOL, this);
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
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    shape_angle.y = current.angle.y;
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    mtrlSndId = 0;
    void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* mObjAcch.m_gnd */
    if (!mObjAcch.ChkGroundHit()) {
        mtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gnd)); /* tevStr.mEnvrIdxOverride */
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    setCollision();
    return TRUE;
}
VERIFY(0x0223F2D0, &daNpc_Ho_c::_execute);

/* 0223EFA0 */
BOOL daNpc_Ho_c::_delete() {
    WWHD_FUNC(0x0223EFA0, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001A6CB) /* "Ho" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x0223EFA0, &daNpc_Ho_c::_delete);

/* 0223EB94 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0223EB94, BOOL, i_this);
    return ((daNpc_Ho_c*)i_this)->CreateHeap();
}
VERIFY(0x0223EB94, CheckCreateHeap);

/* 0223ED9C */
cPhs_State daNpc_Ho_c::_create() {
    WWHD_FUNC(0x0223ED9C, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Ho_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025D4ED0, this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = HO_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x1001A5A0, 0x1001A5C0, 0x1001A5B0});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1001A590);
            gabi::call(0x0259DAA0, &m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001A6C8) /* "Ho" */);
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_HO_e:
            mType = 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        actor_status = (actor_status & ~0x3Fu) | 0x28; /* fopAcM_SetStatusMap(this, 0x28) */
        actor_status = actor_status | 0x20;           /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
        if (!fopAcM_entrySolidHeap(this, 0x0223EB94 /* CheckCreateHeap */, 0xB7B0)) {
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
VERIFY(0x0223ED9C, &daNpc_Ho_c::_create);

/* 0223E814 */
BOOL daNpc_Ho_c::CreateHeap() {
    WWHD_FUNC(0x0223E814, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001A638) /* "Ho" */, 0xF /* dRes_INDEX_HO_BDL_HO_e */, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1576, modelData) */
        JUT_ASSERT_fail(STR(0x1001A650), 0x628, STR(0x1001A644));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001A638), 9 /* dRes_INDEX_HO_BCK_HO_WAIT01_e */, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001A63C) /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(1612, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x1001A650), 0x64C, STR(0x1001A660));
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001A67C) /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(1617, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x1001A650), 0x651, STR(0x1001A688));
    J3DModelData* penModelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001A638), 0xC /* dRes_INDEX_HO_BDL_HO_PEND_e */, SAFESTRING_VTBL);
    if (penModelData == nullptr) /* JUT_ASSERT(1634, penModelData) */
        JUT_ASSERT_fail(STR(0x1001A650), 0x662, STR(0x1001A6A8));
    mpJoyPendentModel = mDoExt_J3DModel__create(penModelData, 0, 0x11020203);
    if (mpJoyPendentModel.get() == nullptr) {
        return FALSE;
    }
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Ho) */
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x0223E47C);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x0223E814, &daNpc_Ho_c::CreateHeap);

/* 0223EF9C */
static cPhs_State daNpc_Ho_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0223EF9C, cPhs_State, i_this);
    return ((daNpc_Ho_c*)i_this)->_create();
}
VERIFY(0x0223EF9C, daNpc_Ho_Create);

/* 0223EFEC */
static BOOL daNpc_Ho_Delete(daNpc_Ho_c* i_this) {
    WWHD_FUNC(0x0223EFEC, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0223EFEC, daNpc_Ho_Delete);

/* 0223F5F8 */
static BOOL daNpc_Ho_Execute(daNpc_Ho_c* i_this) {
    WWHD_FUNC(0x0223F5F8, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0223F5F8, daNpc_Ho_Execute);

/* 0223F768 */
static BOOL daNpc_Ho_Draw(daNpc_Ho_c* i_this) {
    WWHD_FUNC(0x0223F768, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0223F768, daNpc_Ho_Draw);

/* 0223F76C */
static BOOL daNpc_Ho_IsDelete(daNpc_Ho_c*) {
    WWHD_FUNC(0x0223F76C, BOOL, (daNpc_Ho_c*)nullptr);
    return TRUE;
}
VERIFY(0x0223F76C, daNpc_Ho_IsDelete);

/* 02240CE4: static initialisation of the translation unit (only the per-TU header statics) */
static void __sinit_d_a_npc_ho_cpp() {
    WWHD_FUNC(0x02240CE4, void);
    sinit_header_statics(0x104671B0, 0x101BE830);
}
VERIFY(0x02240CE4, __sinit_d_a_npc_ho_cpp);

/* 02240D78: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001A578 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02240D78, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x02240D78, SafeString_dt);

/* 02240D8C: daNpc_Ho_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ho_c_dt(daNpc_Ho_c* i_this, s32 flags) {
    WWHD_FUNC(0x02240D8C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001A5B0);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001A5C0);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02240D8C, daNpc_Ho_c_dt);

/* 02240E28: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x02240E28, void, (SafeString*)nullptr);
}
VERIFY(0x02240E28, SafeString_assureTerminationImpl);
