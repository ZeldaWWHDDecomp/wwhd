/**
 * d_a_npc_people_talk.cpp (WWHD)
 * NPC - Windfall townspeople: talk, message status, attention, look-back, animation playback.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* GHS pointer-to-member call without arguments, returning r3 */
static inline s32 people_pmf_call0(void* self, u32 pmf) {
    s16 i = gabi::load<s16>(pmf + 2);
    u32 thisp = gabi::ea(self) + (s32)gabi::load<s16>(pmf);
    if (i < 0) {
        return gabi::call_ptr<s32>(gabi::load<u32>(pmf + 4), thisp);
    }
    s16 voff = gabi::load<s16>(pmf + 6);
    u32 vt = gabi::load<u32>(thisp + voff);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + i * 8 + 4), thisp);
}
enum : u32 { PEOPLE_l_execute_init = 0x101C4B6C };
/* HD: daNpcPeople_c's message functions are virtual (vtable at +0xB4): slot 0x14
 * next_msgStatus, 0x1C getMsg, 0x24 anmAtr(u16) */
static inline u32 people_vfn(void* self, u32 slot) { return gabi::load<u32>(gabi::load<u32>(gabi::ea(self) + 0xB4) + slot); }
static inline u32 people_v_next_msgStatus(daNpcPeople_c* self, be<u32>* msgNo) { return gabi::call_ptr<u32>(people_vfn(self, 0x14), self, msgNo); }
static inline u32 people_v_getMsg(daNpcPeople_c* self) { return gabi::call_ptr<u32>(people_vfn(self, 0x1C), self); }
static inline void people_v_anmAtr(daNpcPeople_c* self, u16 status) { gabi::call_ptr(people_vfn(self, 0x24), self, status); }
/* HD message manager (*(0x101F4B5C)) methods: 025F795C status, 025F74D0 setStatus,
 * 025F7DB0 messageSet(msgNo, cXyz* pos), 025F7E68 scopeMessageSet(msgNo) */
static inline u32 msgMng_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void msgMng_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
static inline u32 msgMng_scopeMessageSet(u32 mgr, u32 msgNo) { return gabi::call<u32>(0x025F7E68, mgr, msgNo); }
/* play + 0x5BB3: scope message status */
static inline u8 dComIfGp_getScopeMesgStatus() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB3); }
static inline void dComIfGp_setScopeMesgStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5BB3, v); }
/* dEvt_control_c (play + 0x51D0): mPtTalk at +0xCC; 0253EE04 convPId, 0253F124 getPId */
static inline fopAc_ac_c* dComIfGp_event_getTalkPartner() {
    u32 play = dComIfGp_ea();
    return gabi::call<fopAc_ac_c*>(0x0253EE04, play + PLAY_EVTCTRL, gabi::load<u32>(play + 0x529C));
}
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* save info helpers */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline u32 dComIfGs_checkGetItemNum(u8 item) { return gabi::call<u32>(0x02520FE4, item); }
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, dComIfGs_save() + 0xD4, i); }
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); } /* dSv_event_c::offEventBit */
/* dComIfGp_resetItemTimer(t): play+0x5BAA = t, play+0x5BDF = 0 */
static inline void dComIfGp_resetItemTimer(u32 play, s16 t) {
    gabi::store<s16>(play + 0x5BAA, t);
    gabi::store<u8>(play + 0x5BDF, 0);
}
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }

/* 022C06F4 */
void daNpcPeople_c::executeSetMode(u32 proc) {
    WWHD_FUNC(0x022C06F4, void, this, proc);
    m740 = 0.0f;
    m78F = people_pmf_call0(this, PEOPLE_l_execute_init + proc * 8); /* (this->*l_execute_init[proc])() */
}
VERIFY(0x022C06F4, &daNpcPeople_c::executeSetMode);

/* 022C2CC0 */
void daNpcPeople_c::playTexPatternAnm() {
    WWHD_FUNC(0x022C2CC0, void, this);
    if (m_head_tex_pattern.get() != nullptr && cLib_calcTimer(&m780) == 0) {
        s32 frame_max = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)m78E >= frame_max) {
            s32 max = J3DAnm_getFrameMax(m_head_tex_pattern);
            m780 = 0x78;
            m78E = m78E - max;
        } else {
            m78E = m78E + 1;
        }
    }
}
VERIFY(0x022C2CC0, &daNpcPeople_c::playTexPatternAnm);

/* 022C2D6C */
void daNpcPeople_c::playAnm() {
    WWHD_FUNC(0x022C2D6C, void, this);
    if (mpMorf->play(nullptr, 0, 0)) {
        if (m728.get() != nullptr) {
            if (m796 > 0) {
                m796 = m796 - 1;
                if (m796 == 0) {
                    m728 = gabi::at<sPeopleAnmDat>(gabi::ea(m728.get()) + 3);
                    if (setAnmTbl(m728, 1)) {
                        mAnmFlag = mAnmFlag | 1;
                    }
                } else {
                    setAnm(m728->field_0x00, 0, 0.0f, m750);
                }
            }
        }
    }
    if (mpHeadMorf.get() != nullptr) {
        mpHeadMorf->play(nullptr, 0, 0);
    }
}
VERIFY(0x022C2D6C, &daNpcPeople_c::playAnm);

/* 022C192C */
void daNpcPeople_c::chkMsg() {
    WWHD_FUNC(0x022C192C, void, this);
    switch (mCurrMsgNo) {
    case 0x3278: {
        gabi::Local<cXyz> p1;
        dVibration_c* vib = dComIfGp_getVibration();
        p1->x = 0.0f;
        p1->y = 1.0f;
        p1->z = 0.0f;
        gabi::call(0x025CB374, vib, 8, 1, p1.get()); /* StartShock(8, 1, cXyz(0, 1, 0)) */
        gabi::Local<cXyz> p2;
        vib = dComIfGp_getVibration();
        p2->x = 0.0f;
        p2->y = 1.0f;
        p2->z = 0.0f;
        gabi::call(0x025CB374, vib, 4, 0x1E, p2.get());
        mDoAud_seStart(0x8F0 /* JA_SE_START_WHISTLE */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        resetPig();
        break;
    }
    case 0x3287:
    case 0x328F:
    case 0x3291:
    case 0x3292:
        mDoAud_seStart(0x8A5 /* JA_SE_MINIGAME_RIGHT */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    case 0x328D:
    case 0x3293:
    case 0x3294:
    case 0x3295:
        mDoAud_seStart(0x8A6 /* JA_SE_MINIGAME_WRONG */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    }
}
VERIFY(0x022C192C, &daNpcPeople_c::chkMsg);

/* 022C1BBC */
u16 daNpcPeople_c::talk2(int param_1, fopAc_ac_c* param_2) {
    WWHD_FUNC(0x022C1BBC, u16, this, param_1, param_2);
    u16 status = 0xFF;
    /* HD: the message (GameCube mpCurrMsg) is the message manager, read once */
    u32 mgr = people_msgManager();
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = people_v_getMsg(this);
        }
        mCurrMsgBsPcId = msgMng_messageSet(mgr, mCurrMsgNo, &param_2->eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) { /* HD: only when the message was set */
            m7A8 = 0;
            mbHasMsg = 0;
            m77C = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)msgMng_getStatus(mgr);
        switch (status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            if (m7A8 == 0) {
                chkMsg();
            }
            m7A8 = 0;
            msgMng_setStatus(mgr, people_v_next_msgStatus(this, &mCurrMsgNo));
            if (msgMng_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                msgMng_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        case 6: /* fopMsgStts_MSG_TYPING_e */
            if (m77C == 0xF || m77C == 2 /* fopMsgStts_BOX_OPENING_e */) {
                chkMsg();
                m7A8 = 1;
            }
            break;
        case 0xA: /* fopMsgStts_CLOSE_WAIT_e */
            if (m7A8 == 0) {
                chkMsg();
                m7A8 = 1;
            }
            break;
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            if (mEtcFlag & 0x800) {
                mEtcFlag = mEtcFlag & ~0x800;
                initUgSearchArea();
                dComIfGp_event_reset();
            }
            msgMng_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        m77C = status;
        people_v_anmAtr(this, status);
    } else {
        /* HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag */
        mbHasMsg = 1;
        if (m730.get() != nullptr && mNpcNo == NPC_UB1) {
            if (dComIfGp_event_getTalkPartner() == this) {
                m730 = (daNpcPeople_c*)fopAcM_searchFromName(STR(0x100206A0) /* "Ub2" */, 0, 0);
            } else {
                m730 = this;
            }
            dComIfGp_event_setTalkPartner(m730);
            m730 = nullptr;
        }
    }
    if (!(mEtcFlag & 0x200000)) {
        setAnmFromMsgTag();
    }
    return status;
}
VERIFY(0x022C1BBC, &daNpcPeople_c::talk2);

/* 022C1FD8 */
u16 daNpcPeople_c::talk3(int param_1) {
    WWHD_FUNC(0x022C1FD8, u16, this, param_1);
    u16 status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = getMsg3();
        }
        if (mCurrMsgNo != 0) {
            mCurrMsgBsPcId = msgMng_scopeMessageSet(people_msgManager(), mCurrMsgNo);
            if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
                mbHasMsg = 0;
                m77C = 0xFFFF;
            }
        } else {
            status = 0x12; /* fopMsgStts_BOX_CLOSED_e */
        }
    } else if (mbHasMsg) {
        status = dComIfGp_getScopeMesgStatus();
        switch (status) {
        case 0xE:
            dComIfGp_setScopeMesgStatus(people_v_next_msgStatus(this, &mCurrMsgNo));
            if (dComIfGp_getScopeMesgStatus() == 0xF) {
                msgMng_scopeMessageSet(people_msgManager(), mCurrMsgNo);
            }
            break;
        case 0x12:
            dComIfGp_setScopeMesgStatus(0x11 /* fopMsgStts_BOX_CLOSING_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        m77C = status;
        people_v_anmAtr(this, status);
    } else {
        mbHasMsg = 1;
    }
    return status;
}
VERIFY(0x022C1FD8, &daNpcPeople_c::talk3);

/* 022C2E50 */
void daNpcPeople_c::lookBack() {
    WWHD_FUNC(0x022C2E50, void, this);
    s16 target = m782;
    s16 targetY = current.angle.y;
    cXyz* dstTemp = nullptr;
    gabi::Local<cXyz> temp2;
    /* cXyz dstPos = eyePos (through FPRs) */
    f32 eye_x = eyePos.x;
    f32 eye_y = eyePos.y;
    f32 eye_z = eyePos.z;
    u8 temp = m764;
    switch (m799) {
    case 1:
        temp2->copy(mLookAtPos);
        dstTemp = temp2.get();
        break;
    case 2:
        targetY = m786;
        break;
    case 0:
    default:
        break;
    }
    if (mTalk != 0 && m79D != 0) {
        temp = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        if (mEventCut.mTurnSpeed != 0) {
            target = mEventCut.mTurnSpeed;
        }
        cLib_addCalcAngleS2(&m784, target, 4, 0x800);
    } else {
        m784 = 0;
    }
    gabi::Local<cXyz> dstPos;
    dstPos->x = eye_x;
    dstPos->y = eye_y;
    dstPos->z = eye_z;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstTemp, dstPos.get(), targetY, m784, temp);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
VERIFY(0x022C2E50, &daNpcPeople_c::lookBack);

/* 022C0084 */
void daNpcPeople_c::chkAttention() {
    WWHD_FUNC(0x022C0084, void, this);
    u8 npc_no = mNpcNo;
    m78A = 0;
    if (npc_no >= 0x13) { /* HD: an array bound assertion (npc_no indexes a table) */
        JUT_ASSERT_fail(STR(0x100205F4), 0x234B, STR(0x100205BC));
        npc_no = 0;
    }
    if (mEventCut.mbAttention) { /* getAttnFlag() */
        mLookAtPos.z = mEventCut.mPos.z;
        mLookAtPos.x = mEventCut.mPos.x;
        mLookAtPos.y = mEventCut.mPos.y;
        m799 = 1;
        if (m79D != 0) {
            m764 = false;
            m_jnt.mbTrn = 1;
        } else {
            m764 = true;
        }
        if (m789 == 0) {
            m789 = 1;
            m78B = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> pos;
        pos->z = current.pos.z;
        pos->y = current.pos.y;
        pos->x = current.pos.x;
        gabi::Local<cXyz> ppos;
        ppos->x = player->current.pos.x;
        f32 temp = m748;
        ppos->y = player->current.pos.y;
        s32 temp2 = m776;
        ppos->z = player->current.pos.z;
        s32 temp3 = m778;
        gabi::Local<be<f32>> temp4;
        gabi::Local<be<s16>> temp5;
        dNpc_calc_DisXZ_AngY(pos.get(), ppos.get(), temp4.get(), temp5.get());
        if (m789 != 0) {
            temp += 40.0f;
            temp2 += 0x71C;
            temp3 += 0x71C;
        }
        s16 ang = (s16)(*temp5 - shape_angle.y);
        *temp5 = ang;
        f32 dist = *temp4;
        s32 abs_ang = ang < 0 ? -ang : ang;
        bool near = temp > dist && player->current.pos.y > current.pos.y - 200.0f;
        if (near && temp3 > abs_ang) {
            if (m789 == 0) {
                m789 = 1;
            }
            m78B = 1;
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye.get(), mpNpcDat->field_0x14);
            m764 = (m79D == 0);
            mLookAtPos.copy(*eye);
            m799 = 1;
            if (m79E == 0) {
                m764 = false;
                m786 = m77A;
                m799 = 2;
                m_jnt.mbTrn = 1;
            }
            if (npc_no == NPC_UB3 && mTalk == 0) {
                setAnmTbl(gabi::at<sPeopleAnmDat>(0x101C31B4) /* l_npc_anm_wait */, 1);
            }
        } else if (near && temp2 > abs_ang) {
            if (m78B == 0) {
                m78B = 1;
            }
            if (m789 == 1) {
                m789 = 0;
                m770 = mpNpcDat->field_0x58;
            }
            m799 = 0;
        } else {
            if (m789 == 1) {
                m789 = 0;
                m770 = mpNpcDat->field_0x58;
            }
            m78B = 0;
            if (mpNpcDat->field_0x2C > dist) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos_l(eye.get(), mpNpcDat->field_0x14);
                m799 = 1;
                m764 = (m79D == 0);
                mLookAtPos.copy(*eye);
                if (m79E == 0) {
                    m764 = false;
                    m786 = m77A;
                    m799 = 2;
                    m_jnt.mbTrn = 1;
                }
                m78A = 1;
            } else {
                m799 = 0;
                if (mPathRun.mPath.get() == nullptr) {
                    if (m770 != 0) {
                        m770 = m770 - 1;
                        if (m770 == 0 && npc_no == NPC_UB3) {
                            setWaitAnm();
                        }
                    } else {
                        m764 = false;
                        m786 = m77A;
                        m799 = 2;
                        m_jnt.mbTrn = 1;
                    }
                }
            }
        }
    }
    m782 = mpNpcDat->field_0x38;
}
VERIFY(0x022C0084, &daNpcPeople_c::chkAttention);

/* 022C6108 */
u16 daNpcPeople_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022C6108, u16, this, pMsgNo);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    /* HD: mpCurrMsg is the message manager (read once); mSelectNum at +0x948 */
    u32 msg = people_msgManager();
    switch (*pMsgNo) {
    case 0x358B: {
        daNpcPeople_c* pActor = (daNpcPeople_c*)fopAcM_searchFromName(gabi::at<const char>(gabi::load<u32>(0x101C3EC0)) /* l_npc_staff_id[17] */, 0, 0);
        if (pActor == nullptr) { /* HD: JUT_ASSERT(pActor != NULL) */
            JUT_ASSERT_fail(STR(0x100207EC), 0x1D43, STR(0x10020800));
            return 0x10;
        }
        u32 play;
        if (gabi::load<u32>(msg + 0x948) == 0) {
            pActor->mCurrMsgNo = 0x358C;
        } else if (play = dComIfGp_ea(), (s32)gabi::load<u16>(dComIfGs_save() + 0x24) < gabi::load<s16>(play + 0x5BA4)) { /* getRupee() < getMessageRupee() */
            pActor->mCurrMsgNo = 0x359B;
        } else {
            pActor->mCurrMsgNo = 0x358D;
            s16 rupee = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
            u32 play = dComIfGp_ea();
            gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - rupee); /* setItemRupeeCount(-rupee) */
        }
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    case 0x3024:
        dComIfGs_onEventBit(0x2220);
        status = 0x10;
        break;
    default:
        if (m738.get() != nullptr) {
            m738 = gabi::at<sUbMsgDat>(gabi::ea(m738.get()) + 8);
            if (m738->field_0x00 != 0) {
                setMessageUb(m738);
            } else {
                status = 0x10;
            }
        } else if (m734.get() != nullptr) {
            u32 p = gabi::ea(m734.get()) + 4;
            m734 = gabi::at<be<u32>>(p);
            u32 v = gabi::load<u32>(p);
            switch (v) {
            case 1:
                m734 = nullptr;
                m77E = m77E | 0x1;
                status = 0x10;
                break;
            case 2:
                m734 = nullptr;
                m77E = m77E | 0x2;
                status = 0x10;
                break;
            case 3:
                m734 = nullptr;
                m77E = m77E | 0x4;
                status = 0x10;
                break;
            case 4:
                m734 = nullptr;
                m77E = m77E | 0x8;
                status = 0x10;
                break;
            case 5:
                m77E = m77E | 0x10;
                status = 0x10;
                break;
            case 6: {
                p += 4;
                m734 = gabi::at<be<u32>>(p);
                u8 no = gabi::load<u8>(p + 3);
                m792 = no;
                if (!dComIfGs_isEventBit(gabi::load<u16>(0x101C305C + no * 2) /* l_item_chk_sa3 */)) {
                    p = gabi::ea(m734.get()) + 4;
                    m734 = gabi::at<be<u32>>(p);
                    *pMsgNo = gabi::load<u32>(p);
                    u8 no2 = m792;
                    m75C = gabi::load<s32>(0x101C3870 + no2 * 4); /* l_item_id_sa3 */
                    dComIfGs_onEventBit(gabi::load<u16>(0x101C305C + no2 * 2));
                } else {
                    m734 = nullptr;
                    status = 0x10;
                }
                break;
            }
            case 7:
                m77E = m77E | 0x20;
                status = 0x10;
                break;
            case 8: {
                p += 4;
                m734 = gabi::at<be<u32>>(p);
                u8 item = gabi::load<u32>(p) == 0 ? (u8)0x29 : (u8)0x28;
                m734 = gabi::at<be<u32>>(p + 4);
                BOOL got = dComIfGs_checkGetItem(item);
                p = gabi::ea(m734.get());
                if (!got) {
                    p += 8;
                    m734 = gabi::at<be<u32>>(p);
                }
                *pMsgNo = gabi::load<u32>(p);
                break;
            }
            case 9:
                m75C = 4;
                m77E = m77E | 0x40;
                status = 0x10;
                break;
            case 0xA:
                /* HD: the reward is a bag upgrade only when not yet owned (item 0xFD) */
                m75C = 5;
                m77E = m77E | 0x40;
                if (!dComIfGs_checkGetItem(0xFD)) {
                    m75C = 0xFD;
                }
                status = 0x10;
                break;
            case 0:
                m734 = nullptr;
                status = 0x10;
                break;
            case 0xB:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    if (dComIfGs_isEventBit(0x1808)) {
                        m734 = gabi::at<be<u32>>(0x101C2F94); /* l_msg_uw1_done_gp1_arasoi */
                    } else if (dComIfGs_isSymbol(0 /* dSymbol_NAYRU_e */)) {
                        m734 = gabi::at<be<u32>>(0x101C3A9C); /* l_msg_uw1_get_pearl1 */
                    } else if (dComIfGs_isEventBit(0x1E10)) {
                        m734 = gabi::at<be<u32>>(0x101C36B8); /* l_msg_uw1_talked_night */
                    } else {
                        m734 = gabi::at<be<u32>>(0x101C3AA8); /* l_msg_uw1_not_talked_night */
                    }
                } else {
                    m734 = gabi::at<be<u32>>(0x101C2F8C); /* l_msg_uw1_talk_next */
                }
                *pMsgNo = *m734;
                break;
            case 0xC:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C2F9C); /* l_msg_uw2_request_yes */
                    dComIfGs_onEventBit(0x2240);
                } else {
                    m734 = gabi::at<be<u32>>(0x101C2FA4); /* l_msg_uw2_request_no */
                }
                *pMsgNo = *m734;
                break;
            case 0xD: {
                s32 dir = getWindDir();
                *pMsgNo = gabi::load<u32>(0x101C36F8 + dir * 4); /* l_msg_um1_wind */
                break;
            }
            case 0xE:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C37A0); /* l_msg_um3_nazo_talk */
                    dComIfGs_onEventBit(0x2310);
                } else {
                    m734 = gabi::at<be<u32>>(0x101C2FF4); /* l_msg_um3_no_nazo_talk2 */
                }
                *pMsgNo = *m734;
                break;
            case 0xF:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C307C); /* l_msg_sa4_night_yes */
                } else {
                    m734 = gabi::at<be<u32>>(0x101C3940); /* l_msg_sa4_night_no */
                }
                *pMsgNo = *m734;
                break;
            case 0x10:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C30A4); /* l_msg_sa5_yes */
                    dComIfGs_setTmpReg(0xFF03, (u8)getRand(3));
                    BOOL b = dComIfGs_isEventBit(0x2A04);
                    dComIfGp_resetItemTimer(dComIfGp_ea(), b ? 0x708 : 0xE10);
                    mEtcFlag = mEtcFlag | 0x100000;
                } else {
                    m734 = gabi::at<be<u32>>(0x101C309C); /* l_msg_sa5_no */
                }
                *pMsgNo = *m734;
                break;
            case 0x11:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    if (dComIfGs_checkGetItemNum(0x45 /* dItemNo_SKULL_NECKLACE_e */) < 3) {
                        m734 = gabi::at<be<u32>>(0x101C3D18); /* l_msg_xy_sa5_yes_ng */
                    } else {
                        m734 = gabi::at<be<u32>>(0x101C3988); /* l_msg_xy_sa5_yes */
                        u32 play = dComIfGp_ea();
                        gabi::store<s16>(play + 0x5B70, gabi::load<s16>(play + 0x5B70) - 3); /* setItemBeastNumCount(SKULL_NECKLACE, -3) */
                        dComIfGp_resetItemTimer(dComIfGp_ea(), 0xE10);
                        mEtcFlag = mEtcFlag | 0x100000;
                    }
                } else {
                    m734 = gabi::at<be<u32>>(0x101C3D0C); /* l_msg_xy_sa5_no */
                }
                status = 0x10;
                break;
            case 0x12: {
                u8 r = dComIfGs_getTmpReg(0xFF03);
                if (r >= 3) { /* HD: bounds assertion, the list is kept */
                    JUT_ASSERT_fail(STR(0x100207EC), 0x1E1B, STR(0x1002080C));
                } else {
                    m734 = gabi::at<be<u32>>(gabi::load<u32>(0x101C3CAC + r * 4)); /* l_msg_sa5_explain[r] */
                }
                if (m734.get() != nullptr) { /* HD: null check */
                    *pMsgNo = *m734;
                }
                break;
            }
            case 0x13:
                m734 = gabi::at<be<u32>>(0x101C3D24); /* l_msg_xy_sa5_explain */
                *pMsgNo = *m734;
                break;
            case 0x14:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C3CB8); /* l_msg_sa5_ok */
                    dComIfGs_onTmpBit(0x0280);
                    dComIfGs_onEventBit(0x2680);
                    dComIfGs_onEventBit(0x2640);
                    dComIfGs_offTmpBit(0x0240);
                    dComIfGs_setTmpReg(0xFE03, 0);
                    dComIfGs_setTmpReg(0xFD07, 7);
                } else {
                    m734 = gabi::at<be<u32>>(0x101C30AC); /* l_msg_sa5_wait */
                }
                *pMsgNo = *m734;
                break;
            case 0x15:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    m734 = gabi::at<be<u32>>(0x101C3D30); /* l_msg_xy_sa5_ok */
                    dComIfGs_onTmpBit(0x0280);
                    dComIfGs_onEventBit(0x2680);
                    dComIfGs_onEventBit(0x2620);
                    dComIfGs_setTmpReg(0xFE03, 3);
                    dComIfGs_setTmpReg(0xFF03, 3);
                    dComIfGs_setTmpReg(0xFD07, 7);
                } else {
                    m734 = gabi::at<be<u32>>(0x101C30D4); /* l_msg_xy_sa5_wait */
                }
                *pMsgNo = *m734;
                break;
            case 0x17:
                dComIfGs_offTmpBit(0x0280);
                status = 0x10;
                break;
            default:
                *pMsgNo = v;
                break;
            }
        } else {
            status = 0x10;
        }
    }
    return status;
}
VERIFY(0x022C6108, &daNpcPeople_c::next_msgStatus);
