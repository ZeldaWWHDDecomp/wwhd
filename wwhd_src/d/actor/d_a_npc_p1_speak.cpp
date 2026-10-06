/**
 * d_a_npc_p1_speak.cpp (WWHD): daNpc_P1_c::speakAction, p1c_speakAction
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200F164 cLib_addCalcPos2(cXyz* pos, const cXyz& target, f32 scale, f32 maxStep) */
static inline void p1_cLib_addCalcPos2(cXyz* pos, cXyz* target, f32 scale, f32 maxStep) {
    gabi::call(0x0200F164, pos, target, scale, maxStep);
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) (fopAcM_orderOtherEvent inline) */
static inline BOOL p1_fopAcM_orderOtherEvent2(fopAc_ac_c* a, u32 name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D77DC, a, name, (u32)flag, (u32)hind);
}
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, wipe, wipeSpeed) */
static inline void p1_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 wipe, u8 wipeSpeed) {
    gabi::call(0x0252012C, stage, (s32)point, (s32)roomNo, (s32)layer, speed, mode, wipe, (u32)wipeSpeed);
}
/* dComIfGp_getCamera(0)->mCamera.SkipSmoother() (three flag bytes of the camera) */
static inline void p1_skipSmoother_a(u32 cam) {
    gabi::store<u8>(cam + 0x34A, 1);
    gabi::store<u8>(cam + 0x348, 1);
    gabi::store<u8>(cam + 0x349, 1);
}
/* |(a - b).xz| with the {x, 0, z} copy made through FPRs (lfs/stfs) */
static inline f32 p1_absXZ_f(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(a, diff.get(), b);
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    f32 sq = PSVECSquareMag(xz.get());
    return std_sqrtf(sq);
}

/* 022B4E54 */
BOOL daNpc_P1_c::speakAction(void*) {
    WWHD_FUNC(0x022B4E54, BOOL, this, (u32)0);
    u32 mgr = p1_msgMgr(); /* HD: the message manager replaces l_msg */
    if (mActionStatus == ACTION_STARTING_e) {
        if (mPrevMesg == 0xFAE) {
            p1_skipSmoother_a(p1_camera0());
        }
        if (!p1_checkCommandTalk(this)) {
            f32 d = p1_absXZ_f(&dComIfGp_getPlayer(0)->current.pos, &current.pos);
            if (d < 400.0f) {
                fopAcM_orderSpeakEvent(this);
                p1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
            }
            return FALSE;
        }
        if (mPrevMesg == 0xFAE) {
            mPrevMesg = mCurrMesg;
            mCurrMesg = 0xFA5;
        } else {
            s16 point = p1_getStartStagePoint();
            mPrevMesg = mCurrMesg;
            mCurrMesg = point == 2 ? 0xFAA : 0xFA1;
        }
        mActionStatus = (s8)(mActionStatus + 1);
        p1_msgId() = 0xFFFFFFFF;
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (p1_msgId() == 0xFFFFFFFF) {
            p1_msgId() = p1_msgSet(mgr, mCurrMesg, &eyePos);
        } else {
            m_jnt.mbTrn = 1; /* setTrn() */
            setAnimFromMsg();
            if (mActionStatus == ACTION_ONGOING_e) {
                if (mCurrMesg == 0xFA5) {
                    u32 cam = p1_camera0();
                    gabi::store<u8>(cam + 0x34A, 1);
                    gabi::store<u8>(cam + 0x349, 1);
                    gabi::store<u8>(cam + 0x348, 1);
                }
                /* HD: no fopMsgM_SearchByID; the next step follows directly */
                mActionStatus = (s8)(mActionStatus + 1);
            } else if (p1_msgGetStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                if (getNextMsgNo(1) != 0xFFFFFFFF) {
                    p1_msgSetStatus(mgr, 0xF /* fopMsgStts_MSG_CONTINUES_e */);
                    p1_msgSet(mgr, mCurrMesg, nullptr);
                } else {
                    p1_msgSetStatus(mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
                }
            } else if (p1_msgGetStatus(mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                p1_msgSetStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                p1_setAction(this, P1_normalAction);
                if (mPrevMesg == 0xFA4) {
                    p1_event_reset();
                    p1_fopAcM_orderOtherEvent2(this, 0x1001FAB8 /* "sea_exp_cam" */, 1, 0xFFFF);
                    p1_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
                    p1_setAction(this, P1_explainAction);
                } else {
                    p1_setNextStage(0x1001FAB0 /* "Ocean" */, 1, fopAcM_GetRoomNo(this), -1, 0.0f, 0, 1, 0);
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x022B4E54, &daNpc_P1_c::speakAction);

/* 022B53A0 */
BOOL daNpc_P1_c::p1c_speakAction(void*) {
    WWHD_FUNC(0x022B53A0, BOOL, this, (u32)0);
    u32 mgr = p1_msgMgr();
    if (mActionStatus == ACTION_STARTING_e) {
        if (!p1_checkCommandTalk(this)) {
            f32 d = p1_absXZ_f(&dComIfGp_getPlayer(0)->current.pos, &home.pos);
            if (d < 200.0f) {
                fopAcM_orderSpeakEvent(this);
                p1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
            }
            return FALSE;
        }
        mCurrMesg = 0x1014;
        mPrevMesg = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        p1_msgId() = 0xFFFFFFFF; /* HD: no l_msg */
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (p1_msgId() == 0xFFFFFFFF) {
            p1_msgId() = p1_msgSet(mgr, mCurrMesg, &eyePos);
        } else {
            setAnimFromMsg();
            /* HD: no fopMsgM_SearchByID step */
            if (p1_msgGetStatus(mgr) == 0xE) {
                if (getNextMsgNo(1) != 0xFFFFFFFF) {
                    p1_msgSetStatus(mgr, 0xF);
                    p1_msgSet(mgr, mCurrMesg, nullptr);
                } else {
                    p1_msgSetStatus(mgr, 0x10);
                }
            } else if (p1_msgGetStatus(mgr) == 0x12) {
                setAnm(4, 8.0f);
                p1_cLib_addCalcPos2(&current.pos, &home.pos, 0.75f, 5.0f);
                f32 dist_xz = p1_absXZ_f(&home.pos, &current.pos);
                if (dist_xz < 1.0f) {
                    p1_onEventBit(0x820);
                    mCyl.SetR(100.0f);
                    p1_msgSetStatus(mgr, 0x13);
                    p1_setAction(this, P1_normalAction);
                    p1_event_reset();
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x022B53A0, &daNpc_P1_c::p1c_speakAction);
