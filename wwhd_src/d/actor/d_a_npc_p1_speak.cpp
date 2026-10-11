/**
 * d_a_npc_p1_speak.cpp (WWHD): daNpc_P1_c::speakAction, p1c_speakAction
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

using namespace p1_cpp;

/* 0200F164 cLib_addCalcPos2(cXyz* pos, const cXyz& target, f32 scale, f32 maxStep) */

static inline BOOL p1_fopAcM_orderOtherEvent2(fopAc_ac_c* a, u32 name, u16 flag, u16 hind) {
    return p1OrderOtherEvent(a, name, (u32)flag, (u32)hind);
}

static inline void p1_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 wipe,
                                   u8 wipeSpeed) {
    p1NextStage(stage, (s32)point, (s32)roomNo, (s32)layer, speed, mode, wipe, (u32)wipeSpeed);
}
/* dComIfGp_getCamera(0)->mCamera.SkipSmoother() (three flag bytes of the camera) */

/* |(a - b).xz| with the {x, 0, z} copy made through FPRs (lfs/stfs) */

/* 022B4E54 */
BOOL daNpc_P1_c::speakAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager; /* HD: the message manager replaces l_msg */
    if (mActionStatus == ACTION_STARTING_e) {
        if (mPrevMesg == 0xFAE) {
            p1_skipSmoother_a(p1_camera0());
        }
        if (!eventInfo.checkCommandTalk()) {
            f32 d = p1_absXZ_f(&dComIfGp_getPlayer(0)->current.pos, &current.pos);
            if (d < 400.0f) {
                fopAcM_orderSpeakEvent(this);
                eventInfo.onCondition(dEvtCnd_CANTALK_e);
            }
            return FALSE;
        }
        if (mPrevMesg == 0xFAE) {
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFA5;
        } else {
            s16 point = dComIfGp_getStartStagePoint();
            mPrevMesg = mCurrMesg;
            mCurrMesg = point == 2 ? 0xFAA : 0xFA1;
        }
        mActionStatus = (s8)(mActionStatus + 1);
        l_msgId = 0xFFFFFFFF;
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = p1_msgSet(mgr, mCurrMesg, &eyePos);
        } else {
            m_jnt.mbTrn = 1; /* setTrn() */
            setAnimFromMsg();
            if (mActionStatus == ACTION_ONGOING_e) {
                if (mCurrMesg == 0xFA5) {
                    u32 cam = p1_camera0();
                    p1_cpp::camera(cam)->mSkipFov = 1;
                    p1_cpp::camera(cam)->mSkipTarget = 1;
                    p1_cpp::camera(cam)->mSkipPosition = 1;
                }
                /* HD: no fopMsgM_SearchByID; the next step follows directly */
                mActionStatus = (s8)(mActionStatus + 1);
            } else if (p1_msgGetStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e /* fopMsgStts_MSG_DISPLAYED_e */) {
                if (getNextMsgNo(1) != 0xFFFFFFFF) {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_CONTINUES_e /* fopMsgStts_MSG_CONTINUES_e */);
                    p1_msgSet(mgr, mCurrMesg, nullptr);
                } else {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_ENDS_e /* fopMsgStts_MSG_ENDS_e */);
                }
            } else if (p1_msgGetStatus(mgr) == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
                p1_msgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e /* fopMsgStts_MSG_DESTROYED_e */);
                p1_setAction(this, P1_normalAction);
                if (mPrevMesg == 0xFA4) {
                    dComIfGp_event_reset();
                    p1_fopAcM_orderOtherEvent2(this, 0x1001FAB8, 1, 0xFFFF);
                    eventInfo.onCondition(dEvtCnd_UNK2_e);
                    p1_setAction(this, P1_explainAction);
                } else {
                    p1_setNextStage(0x1001FAB0, 1, fopAcM_GetRoomNo(this), -1, 0.0f, 0, 1, 0);
                }
            }
        }
    }
    return TRUE;
}

/* 022B53A0 */
BOOL daNpc_P1_c::p1c_speakAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager;
    if (mActionStatus == ACTION_STARTING_e) {
        if (!eventInfo.checkCommandTalk()) {
            f32 d = p1_absXZ_f(&dComIfGp_getPlayer(0)->current.pos, &home.pos);
            if (d < 200.0f) {
                fopAcM_orderSpeakEvent(this);
                eventInfo.onCondition(dEvtCnd_CANTALK_e);
            }
            return FALSE;
        }
        mCurrMesg = 0x1014;
        mPrevMesg = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        l_msgId = 0xFFFFFFFF; /* HD: no l_msg */
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = p1_msgSet(mgr, mCurrMesg, &eyePos);
        } else {
            setAnimFromMsg();
            /* HD: no fopMsgM_SearchByID step */
            if (p1_msgGetStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
                if (getNextMsgNo(1) != 0xFFFFFFFF) {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_CONTINUES_e);
                    p1_msgSet(mgr, mCurrMesg, nullptr);
                } else {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_ENDS_e);
                }
            } else if (p1_msgGetStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
                setAnm(4, 8.0f);
                p1_cLib_addCalcPos2(&current.pos, &home.pos, 0.75f, 5.0f);
                f32 dist_xz = p1_absXZ_f(&home.pos, &current.pos);
                if (dist_xz < 1.0f) {
                    dComIfGs_onEventBit(0x820);
                    mCyl.SetR(100.0f);
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                    p1_setAction(this, P1_normalAction);
                    dComIfGp_event_reset();
                }
            }
        }
    }
    return TRUE;
}

/* WWHD glue */
