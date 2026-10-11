/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1_dialogue.h"

static Kg1SavePointer save;
static void resetTalkEvent() {
    Kg1_getPlay()->mEvtCtrl.reset();
}
static dSv_event_c *saveEvent() {
    return &save.info->mSave.mSavedata.mEvent;
}

/* 02266D60 */
void daNpc_Kg1_wait_action(daNpc_Kg1_c *actor) {
    Kg1Actor *a = reinterpret_cast<Kg1Actor *>(actor);
    Kg1Play *play = Kg1_getPlay();
    s32 staff = Kg1_getMyStaffId(play->mEvtManager, "Kg1", 0, 0);
    gabi::Local<be<s16>> process;
    *process = fpcNm_MGBOARD_e;
    Kg1Board *board = Kg1_searchBoard(fpcSch_JudgeForPName, process.get());
    switch (a->mWaitMode) {
    case 0: {
        a->mTalkEnabled = 0;
        a->m_jnt.mbHeadLock = 1;
        a->m_jnt.mbBackBoneLock = 1;
        if (board)
            daMgBoard_clrGInfoDraw(board);
        daNpc_Kg1_clr_seq_flag(actor);
        u8 attentive = a->mbAttention, talk = a->mTalkActive;
        a->mEventOrder = attentive != 0;
        if (!talk)
            break;
        a->mWaitMode = 1;
        a->mTalkAnimationWeight = 0.0f;
        [[fallthrough]];
    }
    case 1: {
        s32 status = Kg1_talk(actor, 1);
        u32 message = a->mCurrMsgNo;
        bool speaking = true;
        if (message < 0x1D56 && a->mRequestedAnimation == 4)
            speaking = false;
        message = a->mCurrMsgNo;
        a->m_jnt.mbHeadLock = (u8)speaking;
        if (message == 0x1D5B)
            a->mResponseComplete = 0;
        else if ((message == 0x1D5C || message == 0x1D57) && status == 6)
            a->mResponseComplete = 1;
        if (status == 0x12) {
            resetTalkEvent();
            if (a->mSequenceFlags[0]) {
                a->mTalkEnabled = 0;
                a->mTalkActive = 0;
                a->mEventOrder = 3;
                a->mWaitMode = 2;
            } else {
                a->mTalkActive = 0;
                a->mWaitMode = 0;
                a->mRequestedAnimation = 2;
            }
        }
        break;
    }
    case 2: {
        a->mTalkEnabled = 0;
        if (board) {
            daMgBoard_reqStartGame(board);
            daMgBoard_setGInfoDraw(board);
        }
        s8 room = a->current.roomNo;
        a->mWaitMode = 3;
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(KG1_START_GAME_SOUND, &a->eyePos, 0, reverb);
        a->mEndGameTimer = 60;
        break;
    }
    case 3: {
        if (a->mEventOrder || !board)
            break;
        u32 finished = daMgBoard_checkEndGame(board);
        a->mSequenceFlags[2] = finished != 0;
        if (!finished)
            break;
        if (Kg1_calcTimer(&a->mEndGameTimer))
            break;
        u32 success = daMgBoard_checkClearGame(board);
        a->mSequenceFlags[1] = (u8)success;
        if (success) {
            u32 score = daMgBoard_getScore(board);
            a->mGameBoardScore = (u8)score;
            a->m778 = 0;
            a->mWaitMode = 4;
            a->mEndGameTimer = 90;
        } else {
            a->m778 = 0;
            a->mRequestedAnimation = 4;
            a->mEndGameTimer = 90;
            a->mWaitMode = 4;
        }
        play = Kg1_getPlay();
        play->mEvtCtrl.mHindFlag = u16(play->mEvtCtrl.mHindFlag) & 0xFFFD;
        a->m779 = 1;
        break;
    }
    case 4:
        if (!Kg1_rewardMessageReady(0))
            break;
        a->m779 = 0;
        Kg1_onEventBit(saveEvent(), 0x2540);
        if (board) {
            board->mbForceEnd = 1;
            daMgBoard_clrGInfoDraw(board);
        }
        a->mWaitMode = 5;
        a->mEventOrder = 2;
        play = Kg1_getPlay();
        Kg1_cutEnd(play->mEvtManager, staff);
        resetTalkEvent();
        break;
    case 5:
        if (a->mTalkActive != 1)
            break;
        a->mTalkEnabled = 1;
        if (Kg1_talk(actor, 1) != 0x12)
            break;
        a->mEventOrder = 0;
        resetTalkEvent();
        {
            u8 success = a->mSequenceFlags[1];
            a->mTalkActive = 0;
            if (success) {
                a->mEventOrder = 5;
                a->mWaitMode = 6;
            } else if (a->mSequenceFlags[0]) {
                a->mEventOrder = 3;
                a->mWaitMode = 2;
            } else {
                a->mWaitMode = 0;
                a->mRequestedAnimation = 2;
            }
        }
        break;
    case 6: {
        if (a->mEventOrder)
            break;
        a->mTalkEnabled = 0;
        struct Rewards {
            be<u8> values[3];
        };
        gabi::Local<Rewards> rewards;
        rewards->values[0] = 7;
        rewards->values[1] = 0xCC;
        rewards->values[2] = 5;
        u32 wins = Kg1_getEventReg(saveEvent(), 0xFE07);
        u32 index = (u8)(wins - 1);
        if (index >= 3) {
            JUT_ASSERT_fail("d_a_npc_kg1.cpp", 0x3C8, "num < (sizeof(clr_itemno_tbl) / sizeof(clr_itemno_tbl[0]))");
            index = 2;
        }
        s8 room = a->current.roomNo;
        u8 item = u8(rewards->values[index]);
        u32 present = Kg1_createItemForPresentDemo(&a->current.pos, item, 0, -1, room, nullptr, nullptr);
        a->mPresentActor = present;
        if (present != 0xFFFFFFFF)
            Kg1_getPlay()->mEvtCtrl.mPtItem = present;
        a->mWaitMode = 7;
        break;
    }
    case 7: {
        s16 event = a->mEventIds[2];
        play = Kg1_getPlay();
        if (!Kg1_endCheck(play->mEvtManager, event))
            break;
        resetTalkEvent();
        a->mEventOrder = 2;
        a->mRequestedAnimation = 11;
        a->mWaitMode = 8;
        a->mSequenceFlags[3] = 1;
        break;
    }
    case 8:
        a->m_jnt.mbBackBoneLock = 1;
        a->m_jnt.mbHeadLock = 1;
        if (a->mTalkActive == 1) {
            bool wasReward = a->mRequestedAnimation == 11;
            a->mTalkEnabled = 1;
            if (Kg1_talk(actor, 1) == 0x12) {
                resetTalkEvent();
                u8 highscore = a->mSequenceFlags[5];
                a->mTalkActive = 0;
                a->mEventOrder = 0;
                if (highscore) {
                    a->mEventOrder = 5;
                    a->mWaitMode = 9;
                } else {
                    a->mWaitMode = 0;
                    a->mRequestedAnimation = 2;
                }
            } else if (wasReward && a->mRequestedAnimation == 4)
                a->mRequestedAnimation = 11;
        }
        break;
    case 9: {
        if (a->mEventOrder)
            break;
        a->mTalkEnabled = 0;
        struct Rewards {
            be<u8> values[2];
        };
        gabi::Local<Rewards> rewards;
        rewards->values[0] = 0xF1;
        rewards->values[1] = 6;
        u32 wins = Kg1_getEventReg(saveEvent(), 0xFF07), index = (u8)(wins - 1);
        s8 room = a->current.roomNo;
        if (index > 1)
            index = 1;
        u8 item = u8(rewards->values[index]);
        u32 present = Kg1_createItemForPresentDemo(&a->current.pos, item, 0, -1, room, nullptr, nullptr);
        a->mPresentActor = present;
        if (present != 0xFFFFFFFF)
            Kg1_getPlay()->mEvtCtrl.mPtItem = present;
        a->mWaitMode = 10;
        break;
    }
    case 10: {
        s16 event = a->mEventIds[2];
        play = Kg1_getPlay();
        if (!Kg1_endCheck(play->mEvtManager, event))
            break;
        resetTalkEvent();
        a->mEventOrder = 2;
        a->mSequenceFlags[4] = 1;
        a->mWaitMode = 11;
        break;
    }
    case 11:
        if (a->mTalkActive == 1) {
            a->mTalkEnabled = 1;
            if (Kg1_talk(actor, 1) == 0x12) {
                resetTalkEvent();
                a->mRequestedAnimation = 2;
                a->mEventOrder = 0;
                a->mWaitMode = 0;
                a->mTalkActive = 0;
            }
        }
        break;
    }
    daNpc_Kg1_kg1_talk_camera(actor);
}
/* WWHD glue
 data save 101F84DC
 ext 025200D4 Kg1Play* Kg1_getPlay()
 ext 02542D88 s32 Kg1_getMyStaffId(void*, const char*, u32, u32)
 ext 025D5218 Kg1Board* Kg1_searchBoard(u32, be<s16>*)
 ext 021C5CD4 void daMgBoard_clrGInfoDraw(Kg1Board*)
 ext 021C5C8C void daMgBoard_reqStartGame(Kg1Board*)
 ext 021C5CC8 void daMgBoard_setGInfoDraw(Kg1Board*)
 ext 021C5CB8 u32 daMgBoard_checkEndGame(Kg1Board*)
 ext 021C5C74 u32 daMgBoard_checkClearGame(Kg1Board*)
 ext 021C5C84 u32 daMgBoard_getScore(Kg1Board*)
 ext 02265DC8 void daNpc_Kg1_clr_seq_flag(daNpc_Kg1_c*)
 ext 02266C90 void daNpc_Kg1_kg1_talk_camera(daNpc_Kg1_c*)
 ext 025A11EC s32 Kg1_talk(daNpc_Kg1_c*, s32)
 ext 0207A9A0 u32 Kg1_calcTimer(be<u8>*)
 ext 02007898 u32 Kg1_rewardMessageReady(u32)
 ext 025B8B68 void Kg1_onEventBit(dSv_event_c*, u16)
 ext 025B8BB0 u32 Kg1_getEventReg(dSv_event_c*, u16)
 ext 025D7DEC u32 Kg1_createItemForPresentDemo(cXyz*, s32, u32, s32, s32, void*, void*)
 ext 02543280 void Kg1_cutEnd(void*, s32)
 ext 025440C8 u32 Kg1_endCheck(void*, s16)
 str 1001BEC8 "Kg1"
 str 1001BECC "d_a_npc_kg1.cpp"
 str 1001BEDC "num < (sizeof(clr_itemno_tbl) / sizeof(clr_itemno_tbl[0]))"
*/
