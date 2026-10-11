/**
 * d_a_npc_p1_action.cpp (WWHD): normalAction, confuseAction, talkAction of daNpc_P1_c.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

using namespace p1_cpp;

/* 022B43EC */
BOOL daNpc_P1_c::normalAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 fVar4 = p1_absXZ_mi(&player->current.pos, &current.pos);
    if (mActionStatus == ACTION_STARTING_e) {
        if (mType == TYPE_P1A_e) {
            if (m671 == 1) {
                if (p1_checkEnd(mpMorf, 1.0f)) {
                    setAnm(9, -1.0f);
                } else {
                    return FALSE;
                }
            } else {
                setAnm(0, -1.0f);
            }
        } else if (mType == TYPE_P1B_e) {
            if (mParam == 2) {
                setAnm(0xE, -1.0f);
            } else if (mParam == 3) {
                setAnm(0x10, -1.0f);
            } else {
                setAnm(0, -1.0f);
            }
        } else {
            setAnm(4, -1.0f);
        }
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (eventInfo.checkCommandTalk()) {
            p1_setAction(this, P1_talkAction);
        }
        if (fVar4 < p1_child(mType)->mMaxTalkDist) {
            eventInfo.onCondition(dEvtCnd_CANTALK_e);
        }
        if (mType == TYPE_P1C_e) {
            cLib_addCalcAngleS2(&current.angle.y, home.angle.y, 8, 0x400);
        }
    }
    return TRUE;
}

/* 022B4764 */
BOOL daNpc_P1_c::confuseAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == ACTION_STARTING_e) {
        if (mType == TYPE_P1A_e) {
            setAnm(1, -1.0f);
            mpMorf->setPlaySpeed(2.0f);
        } else {
            setAnm(2, -1.0f);
            mpMorf->setPlaySpeed(2.0f);
        }
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (p1_checkEnd(mpMorf, 2.0f)) {
        m66C = m66C + 1;
        if (m66C > 3) {
            if (mAnmNum == 1) {
                setAnm(2, -1.0f);
                mpMorf->setPlaySpeed(2.0f);
            } else {
                setAnm(1, -1.0f);
                mpMorf->setPlaySpeed(2.0f);
            }
            m66C = 0;
        }
    }
    return TRUE;
}

/* 022B48D0 */
BOOL daNpc_P1_c::talkAction(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    msg_class* mgr = l_messageManager; /* HD: the message manager, read once */
    if (mActionStatus == ACTION_STARTING_e) {
        if (mAnmNum == 10 || mAnmNum == 11) {
            return FALSE;
        }
        mPrevMesg = mCurrMesg;
        if (mType == TYPE_P1A_e) {
            if (mParam == 1) {
                if (dComIfGs_isEventBit(0x910)) {
                    mCurrMesg = 0xC95;
                } else {
                    dComIfGs_onEventBit(0x910);
                    mCurrMesg = 0xC94;
                }
            } else if (mParam == 3) {
                mCurrMesg = 0x100A;
            } else {
                if (!dComIfGs_isEventBit(0x880)) {
                    mCurrMesg = 0x1007;
                    dComIfGs_onEventBit(0x880);
                } else {
                    mCurrMesg = 0x1009;
                }
            }
        } else if (mType == TYPE_P1B_e) {
            if (mParam == 1) {
                mCurrMesg = 0xFA1;
            } else if (mParam == 3) {
                mCurrMesg = 0x100D;
            } else {
                if (!dComIfGs_isEventBit(0x840)) {
                    mCurrMesg = 0x100B;
                    dComIfGs_onEventBit(0x840);
                } else if (dComIfGs_getClearCount() == 0) {
                    mCurrMesg = 0x100C;
                } else {
                    mCurrMesg = 0x1034;
                }
            }
            if (mParam == 2) {
                m_jnt.mbHeadLock = 0; /* offHeadLock */
            }
        } else {
            if (dComIfGs_isEventBit(0x808)) {
                mCurrMesg = 0x1017;
            } else {
                if (!dComIfGs_isEventBit(0x820)) {
                    mCurrMesg = 0x1014;
                    dComIfGs_onEventBit(0x820);
                } else {
                    mCurrMesg = 0x1015;
                }
            }
        }
        mActionStatus = (s8)(mActionStatus + 1);
        l_msgId = 0xFFFFFFFF;
    } else if (mActionStatus != ACTION_ENDING_e) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = p1_msgSet(mgr, mCurrMesg, &eyePos);
        } else {
            if (mType == TYPE_P1B_e) {
                if (mParam != 2 && mParam != 3) {
                    m_jnt.mbTrn = 1; /* setTrn */
                }
            } else if (mType == TYPE_P1A_e) {
                if (mParam != 2) {
                    m_jnt.mbTrn = 1;
                }
            } else {
                m_jnt.mbTrn = 1;
            }
            setAnimFromMsg();
            if (mActionStatus == ACTION_ONGOING_e) {
                /* HD: no fopMsgM_SearchByID */
                mActionStatus = (s8)(mActionStatus + 1);
            } else if (p1_msgGetStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e /* fopMsgStts_MSG_DISPLAYED_e */) {
                if (getNextMsgNo(1) != 0xFFFFFFFF) {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_CONTINUES_e /* fopMsgStts_MSG_CONTINUES_e */);
                    p1_msgSet(mgr, mCurrMesg, nullptr);
                } else {
                    p1_msgSetStatus(mgr, fopMsgStts_MSG_ENDS_e /* fopMsgStts_MSG_ENDS_e */);
                }
            } else if (p1_msgGetStatus(mgr) == fopMsgStts_BOX_CLOSED_e /* fopMsgStts_BOX_CLOSED_e */) {
                if (mParam == 2) {
                    m_jnt.mbHeadLock = 1; /* onHeadLock */
                }
                p1_msgSetStatus(mgr, fopMsgStts_MSG_DESTROYED_e /* fopMsgStts_MSG_DESTROYED_e */);
                p1_setActionPrev(this);
                dComIfGp_event_reset();
            }
        }
    }
    return TRUE;
}

/* WWHD glue */
