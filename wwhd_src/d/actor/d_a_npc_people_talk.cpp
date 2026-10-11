/**
 * d_a_npc_people_talk.cpp (WWHD)
 * NPC - Windfall townspeople: talk, message status, attention, look-back, animation playback.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people_talk.h"

enum { JA_SE_START_WHISTLE = 0x8F0, JA_SE_MINIGAME_RIGHT = 0x8A5, JA_SE_MINIGAME_WRONG = 0x8A6 };
enum { dItemNo_SKULL_NECKLACE_e = 0x45, dBeastIdx_SKULL_NECKLACE_e = 0 };
enum { fopMsgStts_BOX_OPENING_e = 2, fopMsgStts_MSG_TYPING_e = 6, fopMsgStts_CLOSE_WAIT_e = 0xA, fopMsgStts_MSG_DISPLAYED_e = 0xE, fopMsgStts_MSG_DESTROYED_e = 0x13 };

static PeopleInitProc l_execute_init[] = {{&daNpcPeople_c::executeWaitInit}, {&daNpcPeople_c::executeTalkInit}, {&daNpcPeople_c::executeWalkInit}, {&daNpcPeople_c::executeTurnInit}, {&daNpcPeople_c::executeBikkuriInit}, {&daNpcPeople_c::executeFurueInit}, {&daNpcPeople_c::executeKyoroInit}, {&daNpcPeople_c::executeLetterInit}, {&daNpcPeople_c::executeLookInit}, {&daNpcPeople_c::executeLook2Init}, {&daNpcPeople_c::executeUgWalkInit}, {&daNpcPeople_c::executeUgTurnInit}, {&daNpcPeople_c::executeUgLookInit}, {&daNpcPeople_c::executeUgLook2Init}, {&daNpcPeople_c::executeUgSitInit}};

/* 022C06F4 */
void daNpcPeople_c::executeSetMode(u32 proc) {
    m740 = 0.0f;
    m78F = (this->*l_execute_init[proc].fn)();
}

/* 022C2CC0 */
void daNpcPeople_c::playTexPatternAnm() {
    if (m_head_tex_pattern.get() != nullptr && peopleCalcTimer(&m780) == 0) {
        s32 frame_max = peopleFrameMax(m_head_tex_pattern);
        if ((s32)m78E >= frame_max) {
            s32 max = peopleFrameMax(m_head_tex_pattern);
            m780 = 0x78;
            m78E = m78E - max;
        } else {
            m78E = m78E + 1;
        }
    }
}

/* 022C2D6C */
void daNpcPeople_c::playAnm() {
    if (mpMorf->play(nullptr, 0, 0)) {
        if (m728.get() != nullptr) {
            if (m796 > 0) {
                m796 = m796 - 1;
                if (m796 == 0) {
                    m728 = m728.get() + 1;
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

/* 022C192C */
void daNpcPeople_c::chkMsg() {
    switch (mCurrMsgNo) {
    case 0x3278: {
        gabi::Local<cXyz> p1;
        dVibration_c* vib = dComIfGp_getVibration();
        p1->x = 0.0f;
        p1->y = 1.0f;
        p1->z = 0.0f;
        vib->StartShock(8, 1, p1.get()); /* StartShock(8, 1, cXyz(0, 1, 0)) */
        gabi::Local<cXyz> p2;
        vib = dComIfGp_getVibration();
        p2->x = 0.0f;
        p2->y = 1.0f;
        p2->z = 0.0f;
        vib->StartShock(4, 0x1E, p2.get());
        mDoAud_seStart(JA_SE_START_WHISTLE, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        resetPig();
        break;
    }
    case 0x3287:
    case 0x328F:
    case 0x3291:
    case 0x3292:
        mDoAud_seStart(JA_SE_MINIGAME_RIGHT, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    case 0x328D:
    case 0x3293:
    case 0x3294:
    case 0x3295:
        mDoAud_seStart(JA_SE_MINIGAME_WRONG, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    }
}

/* 022C1BBC */
u16 daNpcPeople_c::talk2(int param_1, fopAc_ac_c* param_2) {
    u16 status = 0xFF;
    // HD: use the message manager, read once, in place of GameCube mpCurrMsg.
    PeopleTalkMessageManager* mgr = peopleMessageManager();
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = peopleGetMsg(this);
        }
        mCurrMsgBsPcId = peopleMsgMessageSet(mgr, mCurrMsgNo, &param_2->eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) { /* HD: only when the message was set */
            m7A8 = 0;
            mbHasMsg = 0;
            m77C = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)peopleMsgGetStatus(mgr);
        switch (status) {
        case fopMsgStts_MSG_DISPLAYED_e:
            if (m7A8 == 0) {
                chkMsg();
            }
            m7A8 = 0;
            peopleMsgSetStatus(mgr, peopleNextStatus(this, &mCurrMsgNo));
            if (peopleMsgGetStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                peopleMsgMessageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        case fopMsgStts_MSG_TYPING_e:
            if (m77C == fopMsgStts_MSG_CONTINUES_e || m77C == fopMsgStts_BOX_OPENING_e) {
                chkMsg();
                m7A8 = 1;
            }
            break;
        case fopMsgStts_CLOSE_WAIT_e:
            if (m7A8 == 0) {
                chkMsg();
                m7A8 = 1;
            }
            break;
        case fopMsgStts_BOX_CLOSED_e:
            if (mEtcFlag & 0x800) {
                mEtcFlag = mEtcFlag & ~0x800;
                initUgSearchArea();
                dComIfGp_event_reset();
            }
            peopleMsgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        m77C = status;
        peopleAnmAtr(this, status);
    } else {
        /* HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag */
        mbHasMsg = 1;
        if (m730.get() != nullptr && mNpcNo == NPC_UB1) {
            if (dComIfGp_event_getTalkPartner() == this) {
                m730 = (daNpcPeople_c*)peopleSearchFromName("Ub2" /* "Ub2" */, 0, 0);
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

/* 022C1FD8 */
u16 daNpcPeople_c::talk3(int param_1) {
    u16 status = 0xFF;
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = getMsg3();
        }
        if (mCurrMsgNo != 0) {
            mCurrMsgBsPcId = peopleMsgScopeMessageSet(peopleMessageManager(), mCurrMsgNo);
            if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
                mbHasMsg = 0;
                m77C = 0xFFFF;
            }
        } else {
            status = fopMsgStts_BOX_CLOSED_e;
        }
    } else if (mbHasMsg) {
        status = dComIfGp_getScopeMesgStatus();
        switch (status) {
        case fopMsgStts_MSG_DISPLAYED_e:
            dComIfGp_setScopeMesgStatus(peopleNextStatus(this, &mCurrMsgNo));
            if (dComIfGp_getScopeMesgStatus() == fopMsgStts_MSG_CONTINUES_e) {
                peopleMsgScopeMessageSet(peopleMessageManager(), mCurrMsgNo);
            }
            break;
        case fopMsgStts_BOX_CLOSED_e:
            dComIfGp_setScopeMesgStatus(fopMsgStts_BOX_CLOSING_e);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        m77C = status;
        peopleAnmAtr(this, status);
    } else {
        mbHasMsg = 1;
    }
    return status;
}

/* 022C2E50 */
void daNpcPeople_c::lookBack() {
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
    peopleLookAtTarget(&m_jnt, &current.angle.y, dstTemp, dstPos.get(), targetY, m784, temp);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}

/* 022C0084 */
void daNpcPeople_c::chkAttention() {
    u8 npc_no = mNpcNo;
    m78A = 0;
    // HD: assert the NPC table bound; the debug string calls npc_no a_npc_no.
    if (npc_no > NPC_UG2) {
        JUT_ASSERT_fail("d_a_npc_people.cpp", 0x234B, "a_npc_no >= 0 && (a_npc_no < daNpcPeople_c::Npc_No_Max)");
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
        peopleCalcDisXZAngY(pos.get(), ppos.get(), temp4.get(), temp5.get());
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
            peoplePlayerEyePos(eye.get(), mpNpcDat->field_0x14);
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
                setAnmTbl(peopleTalkData().l_npc_anm_wait, 1);
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
                peoplePlayerEyePos(eye.get(), mpNpcDat->field_0x14);
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

/* 022C6108 */
u16 daNpcPeople_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 status = fopMsgStts_MSG_CONTINUES_e;
    // HD: read the message manager once in place of GameCube mpCurrMsg.
    PeopleTalkMessageManager* msg = peopleMessageManager();
    switch (*pMsgNo) {
    case 0x358B: {
        daNpcPeople_c* pActor = (daNpcPeople_c*)peopleSearchFromName(peopleUg1StaffId(), 0, 0);
        if (pActor == nullptr) { /* HD: JUT_ASSERT(pActor != NULL) */
            // HD: the assertion calls pActor act.
            JUT_ASSERT_fail("d_a_npc_people.cpp", 0x1D43, "pActor != (0)");
            return fopMsgStts_MSG_ENDS_e;
        }
        dComIfG_inf_c* play;
        if (msg->mSelectNum == 0) {
            pActor->mCurrMsgNo = 0x358C;
        } else if (play = peopleTalkGame(), (s32)dComIfGs_getRupee() < play->play.mMessageRupee) { /* getRupee() < getMessageRupee() */
            pActor->mCurrMsgNo = 0x359B;
        } else {
            pActor->mCurrMsgNo = 0x358D;
            s16 rupee = dComIfGp_getMessageRupee();
            dComIfG_inf_c* play = peopleTalkGame();
            play->play.mItemRupeeCount = play->play.mItemRupeeCount - rupee; /* setItemRupeeCount(-rupee) */
        }
        status = fopMsgStts_MSG_ENDS_e;
        break;
    }
    case 0x3024:
        dComIfGs_onEventBit(0x2220);
        status = fopMsgStts_MSG_ENDS_e;
        break;
    default:
        if (m738.get() != nullptr) {
            m738 = m738.get() + 1;
            if (m738->field_0x00 != 0) {
                setMessageUb(m738);
            } else {
                status = fopMsgStts_MSG_ENDS_e;
            }
        } else if (m734.get() != nullptr) {
            be<u32>* p = m734.get() + 1;
            m734 = p;
            u32 v = *p;
            switch (v) {
            case 1:
                m734 = nullptr;
                m77E = m77E | 0x1;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 2:
                m734 = nullptr;
                m77E = m77E | 0x2;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 3:
                m734 = nullptr;
                m77E = m77E | 0x4;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 4:
                m734 = nullptr;
                m77E = m77E | 0x8;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 5:
                m77E = m77E | 0x10;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 6: {
                p += 1;
                m734 = p;
                u8 no = peopleMessageLowByte(p);
                m792 = no;
                if (!dComIfGs_isEventBit(peopleTalkData().l_item_chk_sa3[no])) {
                    p = m734.get() + 1;
                    m734 = p;
                    *pMsgNo = *p;
                    u8 no2 = m792;
                    m75C = peopleTalkData().l_item_id_sa3[no2]; /* l_item_id_sa3 */
                    dComIfGs_onEventBit(peopleTalkData().l_item_chk_sa3[no2]);
                } else {
                    m734 = nullptr;
                    status = fopMsgStts_MSG_ENDS_e;
                }
                break;
            }
            case 7:
                m77E = m77E | 0x20;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 8: {
                p += 1;
                m734 = p;
                u8 item = *p == 0 ? (u8)0x29 : (u8)0x28;
                m734 = p + 1;
                BOOL got = dComIfGs_checkGetItem(item);
                p = m734.get();
                if (!got) {
                    p += 2;
                    m734 = p;
                }
                *pMsgNo = *p;
                break;
            }
            case 9:
                m75C = 4;
                m77E = m77E | 0x40;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 0xA:
                // HD: the reward is a bag upgrade only when not yet owned (item 0xFD).
                m75C = 5;
                m77E = m77E | 0x40;
                if (!dComIfGs_checkGetItem(0xFD)) {
                    m75C = 0xFD;
                }
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 0:
                m734 = nullptr;
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 0xB:
                if (msg->mSelectNum == 0) {
                    if (dComIfGs_isEventBit(0x1808)) {
                        m734 = peopleTalkData().l_msg_uw1_done_gp1_arasoi;
                    } else if (dComIfGs_isSymbol(0 /* dSymbol_NAYRU_e */)) {
                        m734 = peopleTalkData().l_msg_uw1_get_pearl1;
                    } else if (dComIfGs_isEventBit(0x1E10)) {
                        m734 = peopleTalkData().l_msg_uw1_talked_night;
                    } else {
                        m734 = peopleTalkData().l_msg_uw1_not_talked_night;
                    }
                } else {
                    m734 = peopleTalkData().l_msg_uw1_talk_next;
                }
                *pMsgNo = *m734;
                break;
            case 0xC:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_uw2_request_yes;
                    dComIfGs_onEventBit(0x2240);
                } else {
                    m734 = peopleTalkData().l_msg_uw2_request_no;
                }
                *pMsgNo = *m734;
                break;
            case 0xD: {
                s32 dir = getWindDir();
                *pMsgNo = peopleTalkData().l_msg_um1_wind[dir]; /* l_msg_um1_wind */
                break;
            }
            case 0xE:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_um3_nazo_talk;
                    dComIfGs_onEventBit(0x2310);
                } else {
                    m734 = peopleTalkData().l_msg_um3_no_nazo_talk2;
                }
                *pMsgNo = *m734;
                break;
            case 0xF:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_sa4_night_yes;
                } else {
                    m734 = peopleTalkData().l_msg_sa4_night_no;
                }
                *pMsgNo = *m734;
                break;
            case 0x10:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_sa5_yes;
                    dComIfGs_setTmpReg(0xFF03, (u8)getRand(3));
                    BOOL b = dComIfGs_isEventBit(0x2A04);
                    dComIfGp_resetItemTimer(b ? 0x708 : 0xE10);
                    mEtcFlag = mEtcFlag | 0x100000;
                } else {
                    m734 = peopleTalkData().l_msg_sa5_no;
                }
                *pMsgNo = *m734;
                break;
            case 0x11:
                if (msg->mSelectNum == 0) {
                    if (peopleCheckGetItemNum(dItemNo_SKULL_NECKLACE_e) < 3) {
                        m734 = peopleTalkData().l_msg_xy_sa5_yes_ng;
                    } else {
                        m734 = peopleTalkData().l_msg_xy_sa5_yes;
                        dComIfG_inf_c* play = peopleTalkGame();
                        play->play.mItemBeastNumCounts[dBeastIdx_SKULL_NECKLACE_e] = play->play.mItemBeastNumCounts[dBeastIdx_SKULL_NECKLACE_e] - 3; /* setItemBeastNumCount(SKULL_NECKLACE, -3) */
                        dComIfGp_resetItemTimer(0xE10);
                        mEtcFlag = mEtcFlag | 0x100000;
                    }
                } else {
                    m734 = peopleTalkData().l_msg_xy_sa5_no;
                }
                status = fopMsgStts_MSG_ENDS_e;
                break;
            case 0x12: {
                u8 reg = dComIfGs_getTmpReg(0xFF03);
                if (reg >= 3) { /* HD: bounds assertion, the list is kept */
                    // HD: the assertion calls reg a_tempReg.
                    JUT_ASSERT_fail("d_a_npc_people.cpp", 0x1E1B, "(reg >= 0) && (reg < (sizeof(l_msg_sa5_explain) / sizeof(l_msg_sa5_explain[0])))");
                } else {
                    m734 = peopleTalkData().l_msg_sa5_explain[reg]; /* l_msg_sa5_explain[reg] */
                }
                if (m734.get() != nullptr) { /* HD: null check */
                    *pMsgNo = *m734;
                }
                break;
            }
            case 0x13:
                m734 = peopleTalkData().l_msg_xy_sa5_explain;
                *pMsgNo = *m734;
                break;
            case 0x14:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_sa5_ok;
                    dComIfGs_onTmpBit(0x0280);
                    dComIfGs_onEventBit(0x2680);
                    dComIfGs_onEventBit(0x2640);
                    dComIfGs_offTmpBit(0x0240);
                    dComIfGs_setTmpReg(0xFE03, 0);
                    dComIfGs_setTmpReg(0xFD07, 7);
                } else {
                    m734 = peopleTalkData().l_msg_sa5_wait;
                }
                *pMsgNo = *m734;
                break;
            case 0x15:
                if (msg->mSelectNum == 0) {
                    m734 = peopleTalkData().l_msg_xy_sa5_ok;
                    dComIfGs_onTmpBit(0x0280);
                    dComIfGs_onEventBit(0x2680);
                    dComIfGs_onEventBit(0x2620);
                    dComIfGs_setTmpReg(0xFE03, 3);
                    dComIfGs_setTmpReg(0xFF03, 3);
                    dComIfGs_setTmpReg(0xFD07, 7);
                } else {
                    m734 = peopleTalkData().l_msg_xy_sa5_wait;
                }
                *pMsgNo = *m734;
                break;
            case 0x17:
                dComIfGs_offTmpBit(0x0280);
                status = fopMsgStts_MSG_ENDS_e;
                break;
            default:
                *pMsgNo = v;
                break;
            }
        } else {
            status = fopMsgStts_MSG_ENDS_e;
        }
    }
    return status;
}

/* WWHD glue
data l_execute_init 101C4B6C nocheck
ext 025F795C u32 peopleMsgGetStatus(PeopleTalkMessageManager*)
ext 025F74D0 void peopleMsgSetStatus(PeopleTalkMessageManager*, u32)
ext 025F7DB0 u32 peopleMsgMessageSet(PeopleTalkMessageManager*, u32, cXyz*)
ext 025F7E68 u32 peopleMsgScopeMessageSet(PeopleTalkMessageManager*, u32)
ext 0259D624 void peopleCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*)
ext 02520FE4 u32 peopleCheckGetItemNum(u8)
ext 025D9F38 fopAc_ac_c* peopleSearchFromName(const char*, u32, u32)
ext 02055B64 s16 peopleCalcTimer(be<s16>*)
ext 0259DED0 void peopleLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 0259D54C void peoplePlayerEyePos(cXyz*, f32)
str 100205BC "a_npc_no >= 0 && (a_npc_no < daNpcPeople_c::Npc_No_Max)"
str 100205F4 "d_a_npc_people.cpp" in 022C0084
str 100206A0 "Ub2"
str 100207EC "d_a_npc_people.cpp" in 022C6108
str 10020800 "pActor != (0)" renamed
str 1002080C "(reg >= 0) && (reg < (sizeof(l_msg_sa5_explain) / sizeof(l_msg_sa5_explain[0])))" renamed
*/
