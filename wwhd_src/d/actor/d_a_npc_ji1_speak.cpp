/**
 * d_a_npc_ji1_speak.cpp (WWHD)
 * NPC - Orca: the training start/end conversations (startspeakAction, endspeakAction, reiAction,
 * plmoveAction).
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_speak.h"

// HD: the current message manager replaces GameCube's l_msg. Cache it once per action.
static Ji1MessageManager* l_messageManager = {0};
static u32 l_msgId = 0;
enum {
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_CONTINUES_e = 0xF,
    fopMsgStts_BOX_CLOSING_e = 0x11,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
};
// HD: preserve the inlined update order and the already-fetched play object.
static dComIfG_play_c& Ji1_play() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
}
static u8 Ji1_miniGameType() { return Ji1_play().mMiniGameType; }
static void Ji1_endMiniGame(u16 bit) {
    auto& play = Ji1_play();
    play.mMiniGameType = 0;
    play.mMinigameResult = 0;
    play.mMinigameFlags = (u16)play.mMinigameFlags ^ bit;
}
static void Ji1_startMiniGame(dComIfG_play_c& play, u8 type, u16 bit) {
    play.mMiniGameType = type;
    play.mMinigameFlags = (u16)play.mMinigameFlags | bit;
}
static void Ji1_messageCount(s32 count) { Ji1_play().mMsgCountNumber = (s16)count; }
static void Ji1_onCameraAttention() {
    auto& status = Ji1_play().mCameraInfo[0].mCameraAttentionStatus;
    status = (u32)status | 4;
}
static void Ji1_offCameraAttention() {
    auto& status = Ji1_play().mCameraInfo[0].mCameraAttentionStatus;
    status = (u32)status & ~4u;
}
/* fopAcM_orderOtherEventId(this, idx) with the defaults (0xFF, 0xFFFF, 0, 1) */
static inline void ji1_orderOtherEvent(daNpc_Ji1_c* a, s16 idx) { fopAcM_orderOtherEventId(a, idx, 0xFF, 0xFFFF, 0, 1); }
static inline BOOL ji1_checkFrameEnd(mDoExt_McaMorf* m, f32 d) { return m->checkFrame(m->getEndFrame() - d); }

enum {
    JA_SE_CV_JI_ATTACK = 0x4843,
    JA_SE_CM_JI_ATTACK = 0x5822,
    JA_SE_VS_JI_ENDING = 0x85C,
    JA_BGM_HOUSE_G = 0x80000018,
    JA_BGM_JI_TRAINING = 0x80000011,
};

/* 02253880 */
// HD: the message is driven through the message manager (status get/set); the animation is set only once the message exists
BOOL daNpc_Ji1_c::startspeakAction(void* arg) {
    Ji1MessageManager* mgr = l_messageManager;
    if (field_0xC78 == 0) {
        if (!eventInfo.checkCommandDemoAccrpt()) {
            ji1_orderOtherEvent(this, mEventIdx[0]);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
            return FALSE;
        }
        dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0));
        if (dComIfGs_isEventBit(0x0520)) {
            if (field_0xD7B) {
                mMsgNo = 0x9B5;
            } else if (!dComIfGs_isEventBit(0x0F20)) {
                switch (dComIfGs_getEventReg(0xD003)) {
                case 0: mMsgNo = 0x98B; break;
                case 1: mMsgNo = 0x98D; break;
                case 2: mMsgNo = 0x98F; break;
                case 3: mMsgNo = 0x991; break;
                }
            } else {
                mMsgNo = 0x993;
            }
        } else {
            mMsgNo = 0x969;
        }
        field_0xC78 = field_0xC78 + 1;
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
    } else if (field_0xC78 != -1) {
        if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId = Ji1_messageSet(mgr, mMsgNo, &eyePos);
            if (l_msgId != fpcM_ERROR_PROCESS_ID_e) {
                setAnm(5, 4.0f, 0);
            }
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            Ji1_setMessageStatus(mgr, Ji1_nextMessageStatus(this, &mMsgNo));
            if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                Ji1_messageSet(mgr, mMsgNo, nullptr);
            }
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
            int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
            if (staffIdx != -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
            if (dComIfGp_evmng_endCheck(mEventIdx[0])) {
                Ji1_setMessageStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                u32 no = mMsgNo;
                field_0xD70 = 0;
                if (no == 0x969) {
                    field_0xC94 = 0;
                    field_0xC98 = (s16)l_HIO().field_0x54[0];
                    ji1_setAction(this, ACT_teachAction, nullptr);
                } else if (no == 0x98C || no == 0x98E || no == 0x990 || no == 0x992 || no == 0x994) {
                    ji1_setAction(this, ACT_battleAction, nullptr);
                } else {
                    field_0xC94 = 0;
                    ji1_setAction(this, ACT_teachSPRollCutAction, nullptr);
                }
                dComIfGp_event_reset();
            }
        }
    }
    return TRUE;
}

/* 02253EE8 */
// HD: message manager; the animation is set only once the message exists
BOOL daNpc_Ji1_c::endspeakAction(void* arg) {
    Ji1MessageManager* mgr = l_messageManager;
    int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (!eventInfo.checkCommandDemoAccrpt()) {
            ji1_orderOtherEvent(this, mEventIdx[2]);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
            return false;
        }
        if (actionNo == 0) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (dComIfGs_isEventBit(0x0520)) {
            mMsgNo = 0x996;
        } else if (dComIfGs_isEventBit(0x0001)) {
            mMsgNo = 0x96D;
        } else if (!dComIfGs_isEventBit(0x2F40)) {
            mMsgNo = 0x950;
        } else {
            mMsgNo = 0x954;
        }
        field_0xC78 = field_0xC78 + 1;
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
    } else if (field_0xC78 != -1) {
        if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId = Ji1_messageSet(mgr, mMsgNo, &eyePos);
            if (l_msgId != fpcM_ERROR_PROCESS_ID_e) {
                setAnm(5, 4.0f, 0);
            }
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            setAnimFromMsgNo(mMsgNo);
            Ji1_setMessageStatus(mgr, Ji1_nextMessageStatus(this, &mMsgNo));
            if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                Ji1_messageSet(mgr, mMsgNo, nullptr);
            }
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
            Ji1_setMessageStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            mDoAud_bgmStop(45);
            ji1_setAction(this, ACT_reiAction, nullptr);
        }
    }
    return true;
}

/* 02254434 */
// HD: message manager
BOOL daNpc_Ji1_c::reiAction(void* arg) {
    Ji1MessageManager* mgr = l_messageManager;
    int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (actionNo == 1) {
            dComIfGp_evmng_cutEnd(staffIdx);
            return FALSE;
        }
        if (actionNo == 2) {
            initPos(0);
            if (Ji1_miniGameType() == 6) {
                Ji1_endMiniGame(0x20);
            }
            if (Ji1_miniGameType() == 2) {
                Ji1_endMiniGame(0x2);
            }
            dComIfGp_evmng_cutEnd(staffIdx);
            return FALSE;
        }
        mDoAud_bgmStart(JA_BGM_HOUSE_G);
        if (dComIfGs_isEventBit(0x0520)) {
            s32 n = field_0xD70;
            if (n == 0) {
                mMsgNo = 0x997;
            } else {
                Ji1_messageCount(n);
                mMsgNo = 0x999;
            }
        } else if (dComIfGs_isEventBit(0x0001)) {
            mMsgNo = 0x96E;
        } else if (!dComIfGs_isEventBit(0x2F40)) {
            dComIfGs_onEventBit(0x2F40);
            mMsgNo = 0x951;
        } else {
            mMsgNo = 0x955;
        }
        field_0xC78 = field_0xC78 + 1;
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        Ji1_onCameraAttention();
    } else if (field_0xC78 != -1) {
        if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId = Ji1_messageSet(mgr, mMsgNo, &eyePos);
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            Ji1_setMessageStatus(mgr, Ji1_nextMessageStatus(this, &mMsgNo));
            if (Ji1_messageStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                Ji1_messageSet(mgr, mMsgNo, nullptr);
                setAnimFromMsgNo(mMsgNo);
            }
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_BOX_CLOSING_e) {
            if (actionNo == 3) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        } else if (Ji1_messageStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
            if (actionNo == 4) {
                if (mAnimation == 0xC && ji1_checkFrameEnd(mpOrcaMorf, 2.0f)) {
                    dComIfGp_evmng_cutEnd(staffIdx);
                    setAnm(0, 0.0f, 0);
                } else {
                    if (mAnimation != 0xC) {
                        ji1_seStart(this, JA_SE_VS_JI_ENDING, 0);
                    }
                    setAnm(0xC, 8.0f, 0);
                }
            } else {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
            if (dComIfGp_evmng_endCheck(mEventIdx[2])) {
                Ji1_setMessageStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                setAnm(0, 16.0f, 0);
                ji1_setAction(this, ACT_normalAction, nullptr);
                dComIfGp_event_reset();
                Ji1_offCameraAttention();
                field_0xD74 = 0;
            }
        }
    }
    return TRUE;
}

/* 02257C9C */
BOOL daNpc_Ji1_c::plmoveAction(void* arg) {
    int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (dComIfGp_evmng_startCheck(mEventIdx[4])) {
            dComIfGp_evmng_cutEnd(staffIdx);
            field_0xC78 = field_0xC78 + 1;
        } else {
            ji1_orderOtherEvent(this, mEventIdx[4]);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
    } else if (field_0xC78 != -1) {
        if (mAnimation == 0xB) {
            if (ji1_checkFrameEnd(mpOrcaMorf, 14.0f)) {
                ji1_seStart(this, JA_SE_CM_JI_ATTACK, 0);
                setParticle(3, 3.0f, 0.5f);
            } else {
                mDoExt_McaMorf* m = mpOrcaMorf;
                if (m->getFrame() < m->getEndFrame() - 14.0f && m->getFrame() > 5.0f) {
                    current.pos.x = gabi::fnmsubs(8.0f, cM_ssin(current.angle.y), current.pos.x);
                    current.pos.z = gabi::fnmsubs(8.0f, cM_scos(current.angle.y), current.pos.z);
                } else if (m->checkFrame(m->getEndFrame() - 2.0f)) {
                    setAnm(5, 4.0f, 0);
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
            }
        } else {
            if (mAnimation == 0xC && ji1_checkFrameEnd(mpOrcaMorf, 2.0f)) {
                setAnm(0xB, 4.0f, 0);
                mDoAud_bgmStart(JA_BGM_JI_TRAINING);
                dComIfGp_evmng_cutEnd(staffIdx);
            } else if (actionNo == 2 && mAnimation != 0xC) {
                setAnm(0xC, 8.0f, 0);
            } else if (actionNo == 1) {
                dComIfGp_evmng_cutEnd(staffIdx);
                field_0xD84 = 1;
                initPos(0);
                BOOL ev = dComIfGs_isEventBit(0x0520);
                if (field_0xD7B == 0) {
                    auto& play = Ji1_play();
                    if (ev) {
                        Ji1_startMiniGame(play, 6, 0x20);
                        game_life_point() = 3;
                    } else {
                        Ji1_startMiniGame(play, 2, 0x2);
                    }
                }
            }
        }
        if (dComIfGp_evmng_endCheck(mEventIdx[4])) {
            ji1_orderOtherEvent(this, mEventIdx[0]);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
            ji1_setAction(this, ACT_startspeakAction, nullptr);
            dComIfGp_event_reset();
            dtParticle();
        }
    }
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
data l_messageManager 101F4B5C
data l_msgId 10467228 nocheck # BSS: the harness supplies the current message id.
str 1001B280 "Ji1" in 02253880
str 1001B284 "Ji1" in 02253EE8
str 1001B288 "Ji1" in 02254434
str 1001B2B8 "Ji1" in 02257C9C
ext 025F7DB0 u32 Ji1_messageSet(Ji1MessageManager*, u32, cXyz*)
ext 025F795C s32 Ji1_messageStatus(Ji1MessageManager*)
ext 025F74D0 void Ji1_setMessageStatus(Ji1MessageManager*, u32)
ext 0224DA74 u32 Ji1_nextMessageStatus(daNpc_Ji1_c*, be<u32>*)
ext 025E18EC void mDoAud_bgmStart(u32)
ext 025E1904 void mDoAud_bgmStop(u32)
*/
