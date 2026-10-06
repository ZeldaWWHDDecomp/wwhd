/**
 * d_a_npc_ji1_speak.cpp (WWHD)
 * NPC - Orca: the training start/end conversations (startspeakAction, endspeakAction, reiAction,
 * plmoveAction).
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD message manager: the pointer at 0x101F4B5C replaces GameCube's msg_class* (l_msg); the
 * actor keeps only the message id. The manager is read once at the start of the function. */
WWHD_OPAQUE(dMsgMng_l);
static inline dMsgMng_l* dMsg_mgr() { return gabi::at<dMsgMng_l>(gabi::load<u32>(0x101F4B5C)); }
/* 025F7DB0 fopMsgM_messageSet(mgr, msgNo, cXyz* pos): message id, -1 on failure */
static inline u32 msg_set(dMsgMng_l* m, u32 no, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, no, pos); }
/* 025F795C (matcher: fopMsgM_SearchByID): the current message's status (fopMsgStts_*) */
static inline s32 msg_status(dMsgMng_l* m) { return gabi::call<s32>(0x025F795C, m); }
/* 025F74D0: set the current message's status */
static inline void msg_setStatus(dMsgMng_l* m, u32 s) { gabi::call(0x025F74D0, m, s); }
/* next_msgStatus returns a u16: GHS passes its r3 on without re-extending it, so the status is
 * forwarded as the whole register (a guest call to 0224DA74) */
static inline u32 ji1_next_msgStatus(daNpc_Ji1_c* a) { return gabi::call<u32>(0x0224DA74, a, &a->mMsgNo); }
enum {
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_CONTINUES_e = 0xF,
    fopMsgStts_BOX_CLOSING_e = 0x11,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
};
/* static fpc_ProcID l_msgId (0x10467228) */
static inline be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10467228); }

/* audio: 025E18EC mDoAud_bgmStart(id), 025E1904 mDoAud_bgmStop(frames) */
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
static inline void mDoAud_bgmStop(u32 t) { gabi::call(0x025E1904, t); }

/* mini games (play+0x5CE8 flags u16, +0x5CEA type u8, +0x5CEE u8): HD start/end set/toggle the
 * type's flag bit 1 << (type - 1) */
static inline u8 dComIfGp_getMiniGameType() { return gabi::load<u8>(dComIfGp_ea() + 0x5CEA); }
static inline void dComIfGp_endMiniGame(u16 bit) {
    u32 p = dComIfGp_ea();
    gabi::store<u8>(p + 0x5CEA, 0);
    gabi::store<u8>(p + 0x5CEE, 0);
    gabi::store<u16>(p + 0x5CE8, (u16)(gabi::load<u16>(p + 0x5CE8) ^ bit));
}
static inline void play_startMiniGame(u32 p, u8 type, u16 bit) {
    gabi::store<u8>(p + 0x5CEA, type);
    gabi::store<u16>(p + 0x5CE8, (u16)(gabi::load<u16>(p + 0x5CE8) | bit));
}
/* dComIfGp_setMessageCountNumber: play+0x5BA0 (s16) */
static inline void dComIfGp_setMessageCountNumber(s32 n) { gabi::store<s16>(dComIfGp_ea() + 0x5BA0, (s16)n); }
/* camera attention status (camera 0): play+0x5B00 */
static inline void dComIfGp_onCameraAttentionStatus4() {
    u32 p = dComIfGp_ea() + 0x5B00;
    gabi::store<u32>(p, gabi::load<u32>(p) | 4);
}
static inline void dComIfGp_offCameraAttentionStatus4() {
    u32 p = dComIfGp_ea() + 0x5B00;
    gabi::store<u32>(p, gabi::load<u32>(p) & ~4u);
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

/* 02253880 HD: the message is driven through the message manager (status get/set); the
 * animation is set only once the message exists */
BOOL daNpc_Ji1_c::startspeakAction(void* arg) {
    WWHD_FUNC(0x02253880, BOOL, this, arg);
    dMsgMng_l* mgr = dMsg_mgr();
    if (field_0xC78 == 0) {
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            ji1_orderOtherEvent(this, mEventIdx[0]);
            eventInfo_onCondition(this, 2);
            return FALSE;
        }
        dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId(STR(0x1001B280) /* "Ji1" */, nullptr, 0));
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
        l_msgId() = fpcM_ERROR_PROCESS_ID_e;
    } else if (field_0xC78 != -1) {
        if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId() = msg_set(mgr, mMsgNo, &eyePos);
            if (l_msgId() != fpcM_ERROR_PROCESS_ID_e) {
                setAnm(5, 4.0f, 0);
            }
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (msg_status(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            msg_setStatus(mgr, ji1_next_msgStatus(this));
            if (msg_status(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                msg_set(mgr, mMsgNo, nullptr);
            }
        } else if (msg_status(mgr) == fopMsgStts_BOX_CLOSED_e) {
            int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B280), nullptr, 0);
            if (staffIdx != -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
            if (dComIfGp_evmng_endCheck(mEventIdx[0])) {
                msg_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
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
VERIFY(0x02253880, &daNpc_Ji1_c::startspeakAction);

/* 02253EE8 HD: message manager; the animation is set only once the message exists */
BOOL daNpc_Ji1_c::endspeakAction(void* arg) {
    WWHD_FUNC(0x02253EE8, BOOL, this, arg);
    dMsgMng_l* mgr = dMsg_mgr();
    int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B284) /* "Ji1" */, nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            ji1_orderOtherEvent(this, mEventIdx[2]);
            eventInfo_onCondition(this, 2);
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
        l_msgId() = fpcM_ERROR_PROCESS_ID_e;
    } else if (field_0xC78 != -1) {
        if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId() = msg_set(mgr, mMsgNo, &eyePos);
            if (l_msgId() != fpcM_ERROR_PROCESS_ID_e) {
                setAnm(5, 4.0f, 0);
            }
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (msg_status(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            setAnimFromMsgNo(mMsgNo);
            msg_setStatus(mgr, ji1_next_msgStatus(this));
            if (msg_status(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                msg_set(mgr, mMsgNo, nullptr);
            }
        } else if (msg_status(mgr) == fopMsgStts_BOX_CLOSED_e) {
            msg_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            mDoAud_bgmStop(45);
            ji1_setAction(this, ACT_reiAction, nullptr);
        }
    }
    return true;
}
VERIFY(0x02253EE8, &daNpc_Ji1_c::endspeakAction);

/* 02254434 HD: message manager */
BOOL daNpc_Ji1_c::reiAction(void* arg) {
    WWHD_FUNC(0x02254434, BOOL, this, arg);
    dMsgMng_l* mgr = dMsg_mgr();
    int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B288) /* "Ji1" */, nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (actionNo == 1) {
            dComIfGp_evmng_cutEnd(staffIdx);
            return FALSE;
        }
        if (actionNo == 2) {
            initPos(0);
            if (dComIfGp_getMiniGameType() == 6) {
                dComIfGp_endMiniGame(0x20);
            }
            if (dComIfGp_getMiniGameType() == 2) {
                dComIfGp_endMiniGame(0x2);
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
                dComIfGp_setMessageCountNumber(n);
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
        l_msgId() = fpcM_ERROR_PROCESS_ID_e;
        dComIfGp_onCameraAttentionStatus4();
    } else if (field_0xC78 != -1) {
        if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
            l_msgId() = msg_set(mgr, mMsgNo, &eyePos);
        } else if (field_0xC78 == 1) {
            field_0xC78 = field_0xC78 + 1;
        } else if (msg_status(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            msg_setStatus(mgr, ji1_next_msgStatus(this));
            if (msg_status(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                msg_set(mgr, mMsgNo, nullptr);
                setAnimFromMsgNo(mMsgNo);
            }
        } else if (msg_status(mgr) == fopMsgStts_BOX_CLOSING_e) {
            if (actionNo == 3) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        } else if (msg_status(mgr) == fopMsgStts_BOX_CLOSED_e) {
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
                msg_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                setAnm(0, 16.0f, 0);
                ji1_setAction(this, ACT_normalAction, nullptr);
                dComIfGp_event_reset();
                dComIfGp_offCameraAttentionStatus4();
                field_0xD74 = 0;
            }
        }
    }
    return TRUE;
}
VERIFY(0x02254434, &daNpc_Ji1_c::reiAction);

/* 02257C9C */
BOOL daNpc_Ji1_c::plmoveAction(void* arg) {
    WWHD_FUNC(0x02257C9C, BOOL, this, arg);
    int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B2B8) /* "Ji1" */, nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (field_0xC78 == 0) {
        if (dComIfGp_evmng_startCheck(mEventIdx[4])) {
            dComIfGp_evmng_cutEnd(staffIdx);
            field_0xC78 = field_0xC78 + 1;
        } else {
            ji1_orderOtherEvent(this, mEventIdx[4]);
            eventInfo_onCondition(this, 2);
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
                    u32 play = dComIfGp_ea();
                    if (ev) {
                        play_startMiniGame(play, 6, 0x20);
                        game_life_point() = 3;
                    } else {
                        play_startMiniGame(play, 2, 0x2);
                    }
                }
            }
        }
        if (dComIfGp_evmng_endCheck(mEventIdx[4])) {
            ji1_orderOtherEvent(this, mEventIdx[0]);
            eventInfo_onCondition(this, 2);
            ji1_setAction(this, ACT_startspeakAction, nullptr);
            dComIfGp_event_reset();
            dtParticle();
        }
    }
    return TRUE;
}
VERIFY(0x02257C9C, &daNpc_Ji1_c::plmoveAction);
