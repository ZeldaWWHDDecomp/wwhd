/**
 * d_a_npc_p2_act.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): messages, talk, the action functions and their modes.
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2_act.h"

namespace {
enum MessageStatus {
    MSG_TYPING = 6, MSG_DISPLAYED = 0xE, MSG_CONTINUES = 0xF,
    MSG_ENDS = 0x10, BOX_CLOSED = 0x12, BOX_FINISHED = 0x13,
};
enum ActionMode {
    MODE_WAIT = 1,
    MODE_TALK = 2,
    MODE_DEMO_WAIT = 3,
    MODE_DEMO_INTRO = 4,
    MODE_LIFT = 5,
    MODE_JUMP = 6,
    MODE_GOAL_TALK_WAIT = 7,
    MODE_GOAL_TALK = 8,
    MODE_GOAL_TO_TALK = 9,
    MODE_TALK_TO_GOAL = 10,
    MODE_GOAL_WAIT = 11,
    MODE_DEMO_GOAL = 12,
    MODE_TREASURE_WAIT = 13,
    MODE_TREASURE_TALK = 14,
    MODE_ARRIVE = 15,
    MODE_MOCCO_WAIT = 16,
    MODE_TELESCOPE = 17,
    MODE_DEMO_WAIT_2 = 18,
    MODE_INTRO_2 = 19,
    MODE_GOAL_WAIT_2 = 20,
    MODE_GOAL_2 = 21,
    MODE_BOMB_GET = 22,
};
// HD: external actor judge callbacks retain their guest addresses.
static constexpr u32 P2_JudgeByName = 0x025E121C;
static constexpr u32 P2_JudgeByID = 0x025E1234;
static const s8 l_msg_anm_tbl[21] = {1, 2, 3, 15, 14, 13, 16, 17, 18, 19, 20, 1, 2, 13, 14, 1, 22, 13, 23, 7, 6};
static cXyz l_goal_pos;
static u32 l_goal_pos_init;
static cXyz l_talk_pos;
static u32 l_talk_pos_init;
static cXyz l_pos1;
static u32 l_pos1_init;
static cXyz l_pos2;
static u32 l_pos2_init;
static cXyz l_pos3;
static u32 l_pos3_init;
} // namespace

static inline void p2_floatCopy(cXyz* dst, cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}
/* the player is in the circle and not busy (virtual, vtable +0x4C): stop the goal ship (0x1DB) */
static inline void p2_goalCheck(daNpc_P2_c* a, fopAc_ac_c* player, cXyz* center, f32 r, f32 h, s8 mode) {
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), center);
    if (dLib_checkPlayerInCircle(pos.get(), r, h)) {
        if (!gabi::call_ptr<u32>(reinterpret_cast<P2ActionPlayer*>(player)->vtable->isBusy, player)) {
            a->mMode = mode;
            gabi::Local<be<s16>> name;
            *name = 0x1DB;
            fopAc_ac_c* ship = fopAcIt_Judge(P2_JudgeByName, name.get());
            if (ship != nullptr) {
                reinterpret_cast<P2ActionShip*>(ship)->mbStopped = 1;
            }
        }
    }
}
static inline void p2_setAttnDist(daNpc_P2_c* a, u8 v) {
    a->attention_info.distances[1] = v;
    a->attention_info.distances[3] = v;
}

/* 022B89B8 */
u32 daNpc_P2_c::getMsg() {
    u32 msg = 0;
    switch ((u8)mType) {
    case TYPE_P2A_e:
        if (dComIfGs_isEventBit(0x808)) {
            msg = 0x1010;
        } else {
            msg = dLib_setFirstMsg(0x702, 0x100E, 0x100F);
        }
        break;
    case TYPE_P2B_e:
        if (mSubType == 1) {
            if (dComIfGs_isEventBit(0xF02)) {
                msg = dLib_setFirstMsg(0x1502, 0x1B35, 0x1B36);
            } else {
                msg = 0x1028;
            }
        } else if (!dComIfGs_isEventBit(0x720)) {
            msg = dLib_setFirstMsg(0x940, 0xC96, 0xC97);
        } else if (!dComIfGs_isEventBit(0x808)) {
            if (dComIfGs_isEventBit(0x710)) msg = 0x1028;
        } else {
            msg = dLib_setFirstMsg(0x704, 0x1029, 0x102A);
        }
        break;
    case TYPE_P2C_e:
        if (dComIfGs_isEventBit(0x808)) {
            msg = 0x1013;
        } else {
            msg = dLib_setFirstMsg(0x701, 0x1011, 0x1012);
        }
        break;
    }
    return msg;
}

/* 022B8B24 */
void daNpc_P2_c::anmAtr(u16 i_status) {
    if (i_status == MSG_TYPING) {
        u8 attr = dComIfGp_getMesgAnimeAttrInfo();
        if (attr >= 0x15) {
            mAnm = 1;
            return;
        }
        s8 anm = l_msg_anm_tbl[attr];
        u8 type = mType;
        if (type == TYPE_P2A_e) {
            if (mAnm == 0xD && anm == 0x17) return;
        } else if (type == TYPE_P2B_e) {
            if (anm == 7) {
                if (mbMessageGestureStarted == 0) {
                    mbMessageGestureStarted = 1;
                    mAnm = 7;
                }
                return;
            }
        }
        if (mAnm != anm) {
            mAnm = anm;
        }
    } else if (i_status == MSG_ENDS) {
        mAnm = 1;
    }
}

/* 022B8C14 */
u32 daNpc_P2_c::next_msgStatus(u32* pMsgNo) {
    be<u32>* p = (be<u32>*)pMsgNo;
    u32 next;
    switch ((u32)*p) {
    case 0xC96: next = 0xC98; break;
    case 0x1011: next = 0x102F; break;
    case 0x102F: next = 0x1030; break;
    case 0x1012: next = 0x1031; break;
    case 0x1031: next = 0x1032; break;
    case 0x1018: next = 0x1019; break;
    case 0x1020: next = 0x1021; break;
    case 0x1022: next = 0x102C; break;
    case 0x1023: next = 0x1024; break;
    case 0x1025: next = 0x1026; break;
    case 0x1026: next = 0x1027; break;
    case 0x1027: next = 0x102D; break;
    case 0x1B20: next = 0x1B21; break;
    case 0x1B21: next = 0x1B22; break;
    case 0x1B22: next = 0x1B23; break;
    case 0x1B23: next = 0x1B24; break;
    case 0x1B28: next = 0x1B29; break;
    case 0x1B29: next = 0x1B2A; break;
    case 0x1B2A: next = 0x1B2B; break;
    case 0x1B2B: next = 0x1B2C; break;
    case 0x1B2C: next = 0x1B2D; break;
    case 0x1B2D: next = 0x1B2E; break;
    default: return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    *p = next;
    return MSG_CONTINUES;
}

/* 022B8E24 */
u16 daNpc_P2_c::talk(bool i_noGetMsg) {
    u32 mgr = p2_msgMgr(); /* HD: the message manager */
    s8 state = mTalkState;
    if (state == 0) {
        p2_msgId() = 0xFFFFFFFF;
        if (!i_noGetMsg) {
            mMsgNo = getMsg();
        }
        mTalkState = 1;
        return 0xFF;
    }
    if (state == -1) {
        anmAtr(BOX_CLOSED);
        return BOX_CLOSED;
    }
    if (p2_msgId() == 0xFFFFFFFF) {
        p2_msgId() = P2_msgSet(mgr, mMsgNo, &eyePos);
        return 0xFF;
    }
    if ((u32)(s32)state == 1) {
        mTalkState = 2;
        return 0xFF;
    }
    if ((u32)(s32)state != 2) {
        return 0xFF;
    }
    u16 status = (u16)P2_msgGetStatus(mgr);
    if (status == MSG_DISPLAYED) {
        P2_msgSetStatus(mgr, next_msgStatus((u32*)&mMsgNo));
        if (P2_msgGetStatus(mgr) == MSG_CONTINUES) {
            P2_msgSet(mgr, mMsgNo, nullptr);
            anmAtr(status);
            return status;
        }
    } else if (status == BOX_CLOSED) {
        P2_msgSetStatus(mgr, BOX_FINISHED);
        mTalkState = -1;
        mbMessageGestureStarted = 0;
    }
    anmAtr(status);
    return status;
}

/* 022BC1D8 */
void daNpc_P2_c::wait01() {
    if (mbTalk) {
        mMode = MODE_TALK;
    } else {
        mOrder = 2;
    }
}

/* 022BC1FC */
void daNpc_P2_c::moccowait() {
    u32 pid = parentActorID;
    if (pid != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = pid;
        fopAc_ac_c* ship = fopAcIt_Judge(P2_JudgeByID, key.get());
        s16 y = ship != nullptr ? (s16)ship->current.angle.y : (s16)0;
        cLib_addCalcAngleS2(&current.angle.y, (s16)(mShipAngleOffs + y), 4, 0x800);
    }
    mDoExt_McaMorf* book = mpBookMorf;
    if (mAnm == 0x15) {
        /* the book's frame follows the body's (as an integer) */
        book->mFrameCtrl.mFrame = (f32)(s32)(s16)gabi::ftoi(mpMorf->getFrame());
    } else {
        book->mFrameCtrl.mFrame = 0.0f;
    }
    if (mAnm == 1) {
        if (P2_calcTimer(&mMoccoTimer) == 0) {
            mAnm = 0x15;
            mMoccoTimer = (s32)(s16)gabi::ftoi(cM_rndF(100.0f) + 200.0f);
        }
    }
    if (mAnm == 0x15 && p2_isStop(mpMorf)) {
        mAnm = 1;
    }
    if (mbTalk && mAnm == 1) {
        mMode = MODE_TALK;
    } else {
        mOrder = 2;
    }
}

/* 022BC3AC */
void daNpc_P2_c::zukotelescope() {
    u32 pid = parentActorID;
    mAnm = 0x16;
    if (pid != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = pid;
        fopAc_ac_c* ship = fopAcIt_Judge(P2_JudgeByID, key.get());
        s16 y = ship != nullptr ? (s16)ship->current.angle.y : (s16)0;
        cLib_addCalcAngleS2(&current.angle.y, (s16)(mShipAngleOffs + y), 4, 0x800);
    }
    if (mbTalk) {
        mAnm = 1;
        m_jnt.mbBackBoneLock = 0;
        mMode = MODE_TALK;
        m_jnt.mbHeadLock = 0;
    } else {
        mOrder = 2;
    }
}

/* 022BC45C */
void daNpc_P2_c::talk01() {
    if (talk(false) == BOX_CLOSED) {
        u8 type = mType;
        if (type == TYPE_P2C_e) {
            mAnm = 1;
            mMode = MODE_MOCCO_WAIT;
        } else if (type == TYPE_P2A_e && !dComIfGs_isEventBit(0x808)) {
            mMode = MODE_TELESCOPE;
            m_jnt.mbHeadLock = 1;
            m_jnt.mbBackBoneLock = 1;
        } else {
            mAnm = 1;
            mMode = MODE_WAIT;
        }
        dComIfGp_event_reset();
        mbTalk = 0;
    }
}

/* 022BC544 */
BOOL daNpc_P2_c::wait_action(void*) {
    s8 status = mActionStatus;
    if (status == 0) {
        u8 type = mType;
        if (type == TYPE_P2C_e) {
            mMode = MODE_MOCCO_WAIT; /* moccowait */
        } else if (type == TYPE_P2A_e && !dComIfGs_isEventBit(0x808)) {
            mMode = MODE_TELESCOPE; /* zukotelescope */
        } else {
            mMode = MODE_WAIT;
        }
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status != -1) {
        mbAttention = (u8)chkAttention();
        mOrder = 0;
        switch ((u32)(s32)mMode) {
        case MODE_WAIT: wait01(); break;
        case MODE_TALK: talk01(); break;
        case MODE_MOCCO_WAIT: moccowait(); break;
        case MODE_TELESCOPE: zukotelescope(); break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}

/* 022BC6BC */
void daNpc_P2_c::demo_wait() {
    daNpc_P2_childHIO_c* c = p2_child(mType);
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), &c->mDemoWaitPos);
    if (dLib_checkPlayerInCircle(pos.get(), c->mDemoWaitR, c->mDemoWaitH)) {
        mOrder = 3;
        mMode = MODE_DEMO_INTRO;
    }
}

/* 022BC738 */
void daNpc_P2_c::demo_intro() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_INTRO")) {
        mMode = MODE_LIFT;
        mOrder = 0;
        dComIfGp_event_reset();
    }
}

/* 022BC79C */
void daNpc_P2_c::demo_lift() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "Hlift_up")) {
        mMode = MODE_JUMP;
        mOrder = 4;
        mLiftPhase = 0;
    }
}

/* 022BC7F8 */
void daNpc_P2_c::demo_jump() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_TO_GOAL")) {
        dComIfGs_onEventBit(0x720);
        mMode = MODE_GOAL_TO_TALK;
        dComIfGp_event_reset();
    }
}

/* 022BC868 */
void daNpc_P2_c::goal_talkpos_wait() {
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((current.pos.y - player->current.pos.y) - 300.0f < 0.0f) {
        mMode = MODE_TALK_TO_GOAL;
        return;
    }
    p2_setAttnDist(this, 0x22);
    if (mbGoalTalkStarted == 0) {
        mbGoalTalkStarted = 1;
        mOrder = 1;
        return;
    }
    if (mbTalk) {
        mMode = MODE_GOAL_TALK;
        dComIfGp_event_reset();
        mOrder = 5;
    } else if (player->speedF < 1.0f) {
        mOrder = 2;
    }
    daNpc_P2_childHIO_c* c = p2_child(mType);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, MODE_DEMO_GOAL);
}

/* 022BCA04 */
void daNpc_P2_c::goal_talkpos_talk() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_GOAL_WAIT_TALK")) {
        mMode = MODE_GOAL_TALK_WAIT;
        dComIfGp_event_reset();
        mbTalk = 0;
    }
}

/* 022BCA68 */
void daNpc_P2_c::goal_goalpos_to_talkpos() {
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((current.pos.y - player->current.pos.y) - 300.0f < 0.0f) {
        mMode = MODE_TALK_TO_GOAL;
        return;
    }
    p2_setAttnDist(this, 0);
    /* static cXyz l_goal_pos(0, -450, -2915), l_talk_pos(0, -450, -2715) */
    if (l_goal_pos_init == 0) {
        l_goal_pos.x = 0.0f;
        l_goal_pos_init = 1;
        l_goal_pos.y = -450.0f;
        l_goal_pos.z = -2915.0f;
    }
    if (l_talk_pos_init == 0) {
        l_talk_pos.x = 0.0f;
        l_talk_pos_init = 1;
        l_talk_pos.z = -2715.0f;
        l_talk_pos.y = -450.0f;
    }
    cXyz* goal = &l_goal_pos;
    s16 ang = cLib_targetAngleY(&current.pos, goal);
    s32 d = (s32)ang - (s32)current.angle.y;
    p2_copyPosition(&mLookPos, &l_talk_pos);
    f32 fd = (f32)d;
    if (fabsf(fd) < 5376.0f) {
        mAnm = 4;
        cLib_addCalc2(&current.pos.x, goal->x, 0.1f, 4.0f);
        cLib_addCalc2(&current.pos.z, goal->z, 0.1f, 4.0f);
    }
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(goal, diff.get(), &current.pos);
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz.get()));
    u8 type = mType;
    if (dist < 5.0f) {
        mAnm = 1;
        mMode = MODE_GOAL_TALK_WAIT;
    }
    daNpc_P2_childHIO_c* c = p2_child(type);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, MODE_DEMO_GOAL);
}

/* 022BCD60 */
void daNpc_P2_c::goal_talkpos_to_goalpos() {
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    if (dist < p2_child(mType)->mGoalDist) {
        if ((current.pos.y - player->current.pos.y) - 300.0f > 0.0f) {
            mMode = MODE_GOAL_TO_TALK;
            return;
        }
    }
    p2_setAttnDist(this, 0);
    /* static cXyz l_pos1(0, -450, -3130), l_pos2(0, -450, -3380), l_pos3(0, -450, -2715) */
    if (l_pos1_init == 0) {
        l_pos1.x = 0.0f;
        l_pos1_init = 1;
        l_pos1.y = -450.0f;
        l_pos1.z = -3130.0f;
    }
    if (l_pos2_init == 0) {
        l_pos2.y = -450.0f;
        l_pos2.x = 0.0f;
        l_pos2_init = 1;
        l_pos2.z = -3380.0f;
    }
    if (l_pos3_init == 0) {
        l_pos3.y = -450.0f;
        l_pos3.x = 0.0f;
        l_pos3_init = 1;
        l_pos3.z = -2715.0f;
    }
    cXyz* pos1 = &l_pos1;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(pos1, diff.get(), &current.pos);
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    f32 d = std_sqrtf(PSVECSquareMag(xz.get()));
    s16 ang;
    if (d < 5.0f) {
        p2_copyPosition(&mLookPos, &l_pos3);
        ang = cLib_targetAngleY(&current.pos, &mLookPos);
    } else {
        p2_copyPosition(&mLookPos, &l_pos2);
        ang = cLib_targetAngleY(&current.pos, &mLookPos);
    }
    s32 da = (s32)ang - (s32)current.angle.y;
    if (d < 5.0f) {
        if (fabsf((f32)da) < 5376.0f) {
            mAnm = 1;
            mMode = MODE_GOAL_WAIT;
        }
    } else if (fabsf((f32)da) < 5376.0f) {
        mAnm = 4;
        cLib_addCalc2(&current.pos.x, pos1->x, 0.1f, 4.0f);
        cLib_addCalc2(&current.pos.z, pos1->z, 0.1f, 4.0f);
    }
    daNpc_P2_childHIO_c* c = p2_child(mType);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, MODE_DEMO_GOAL);
}

/* 022BD14C */
void daNpc_P2_c::goal_goalpos_wait() {
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    if (dist < p2_child(mType)->mGoalDist) {
        if ((current.pos.y - player->current.pos.y) - 300.0f > 0.0f) {
            mMode = MODE_GOAL_TO_TALK;
            return;
        }
    }
    u8 type = mType;
    p2_setAttnDist(this, 2);
    daNpc_P2_childHIO_c* c = p2_child(type);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, MODE_DEMO_GOAL);
}

/* 022BD29C */
void daNpc_P2_c::demo_goal() {
    mbBigCyl = 0;
    p2_setAttnDist(this, 0xA9);
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_GOAL")) {
        dComIfGs_onEventBit(0x710);
        mOrder = 0;
        mMode = MODE_TREASURE_WAIT;
        dComIfGp_event_reset();
    } else {
        mOrder = 6;
    }
}

/* 022BD34C */
void daNpc_P2_c::treasure_wait() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_TREASURE")) {
        u8 sub = mSubType;
        if (sub == 0) {
            mMode = MODE_ARRIVE;
            mOrder = 7;
        } else if (sub == 1) {
            mMode = MODE_BOMB_GET;
            mOrder = 0xA;
        }
        return;
    }
    if (mbTalk) {
        mMode = MODE_TREASURE_TALK;
    } else {
        mOrder = 2;
    }
}

/* 022BD41C */
void daNpc_P2_c::treasure_wait_talk() {
    mbNoAttention = 1;
    if (talk(false) == BOX_CLOSED) {
        mMode = MODE_TREASURE_WAIT;
        mbTalk = 0;
        dComIfGp_event_reset();
    }
}

/* 022BD47C */
void daNpc_P2_c::demo_arrive() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_ARRIVE_MAJYU")) {
        dKy_instant_timechg(300.0f);
        dComIfGs_onEventBit(0x808);
        mMode = MODE_WAIT;
        mOrder = 0;
        dComIfGp_event_reset();
    }
}

/* 022BD500 */
void daNpc_P2_c::demo_wait_2() {
    daNpc_P2_childHIO_c* c = p2_child(mType);
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), &c->mDemoWaitPos);
    if (dLib_checkPlayerInCircle(pos.get(), c->mDemoWaitR, c->mDemoWaitH)) {
        mOrder = 8;
        mMode = MODE_INTRO_2;
    }
}

/* 022BD57C */
void daNpc_P2_c::demo_intro_2() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_INTRO_2")) {
        dComIfGs_onEventBit(0x1A04);
        mMode = MODE_GOAL_WAIT_2;
        mOrder = 0;
        dComIfGp_event_reset();
    }
}

/* 022BD5F4 */
void daNpc_P2_c::goal_wait_2() {
    dComIfGp_get(); /* HD: an unused call */
    if (dComIfGs_isSwitch(mSwitchNo, home.roomNo)) {
        daNpc_P2_childHIO_c* c = p2_child(mType);
        gabi::Local<cXyz> pos;
        p2_floatCopy(pos.get(), &c->mGoalPos2);
        if (dLib_checkPlayerInCircle(pos.get(), c->mGoal2R, c->mGoal2H)) {
            mMode = MODE_GOAL_2;
            gabi::Local<be<s16>> name;
            *name = 0x1DB;
            fopAc_ac_c* ship = fopAcIt_Judge(P2_JudgeByName, name.get());
            if (ship != nullptr) {
                reinterpret_cast<P2ActionShip*>(ship)->mbStopped = 1;
            }
        }
    }
}

/* 022BD6B8 */
void daNpc_P2_c::demo_goal_2() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_GOAL_2")) {
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> res;
        p2_floatCopy(pos.get(), &mEventCut.mPos);
        mbBigCyl = 1;
        mOrder = 0;
        mMode = MODE_TREASURE_WAIT;
        cXyz_pl(pos.get(), res.get(), &mLookOffs);
        p2_copyPosition(&mGoalLookPos, res.get());
        dComIfGp_event_reset();
    } else {
        mOrder = 9;
    }
}

/* 022BD780 */
void daNpc_P2_c::demo_bomb_get() {
    if (P2_endCheckOld(dComIfGp_getPEvtManager(), "P2B_BOMB_GET")) {
        dComIfGs_onEventBit(0xF02);
        mMode = MODE_WAIT;
        mbBigCyl = 0;
        mOrder = 0;
        dComIfGp_event_reset();
    }
}

/* 022BD7FC */
BOOL daNpc_P2_c::intro_action(void*) {
    s8 status = mActionStatus;
    if (status == 0) {
        s8 mode;
        if (mSubType == 0) {
            if (dComIfGs_isEventBit(0x720)) {
                mMode = MODE_TALK_TO_GOAL;
                mActionStatus = (s8)(mActionStatus + 1);
                return TRUE;
            }
            mode = MODE_DEMO_WAIT;
        } else {
            mode = dComIfGs_isEventBit(0x1A04) ? MODE_GOAL_WAIT_2 : MODE_DEMO_WAIT_2;
        }
        mMode = mode;
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status != -1) {
        mbAttention = (u8)chkAttention();
        mOrder = 0;
        switch ((u32)(s32)mMode) {
        case MODE_WAIT: wait01(); break;
        case MODE_TALK: talk01(); break;
        case MODE_DEMO_WAIT: demo_wait(); break;
        case MODE_DEMO_INTRO: demo_intro(); break;
        case MODE_LIFT: demo_lift(); break;
        case MODE_JUMP: demo_jump(); break;
        case MODE_GOAL_TALK_WAIT: goal_talkpos_wait(); break;
        case MODE_GOAL_TALK: goal_talkpos_talk(); break;
        case MODE_GOAL_TO_TALK: goal_goalpos_to_talkpos(); break;
        case MODE_TALK_TO_GOAL: goal_talkpos_to_goalpos(); break;
        case MODE_GOAL_WAIT: goal_goalpos_wait(); break;
        case MODE_DEMO_GOAL: demo_goal(); break;
        case MODE_TREASURE_WAIT: treasure_wait(); break;
        case MODE_TREASURE_TALK: treasure_wait_talk(); break;
        case MODE_ARRIVE: demo_arrive(); break;
        case MODE_DEMO_WAIT_2: demo_wait_2(); break;
        case MODE_INTRO_2: demo_intro_2(); break;
        case MODE_GOAL_WAIT_2: goal_wait_2(); break;
        case MODE_GOAL_2: demo_goal_2(); break;
        case MODE_BOMB_GET: demo_bomb_get(); break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): private bindings and translation-unit data
ext 02588034 u32 dLib_setFirstMsg(u16 bit, u32 first, u32 second)
ext 025881A4 BOOL dLib_checkPlayerInCircle(cXyz* center, f32 radius, f32 height)
ext 0211D2F8 s32 P2_calcTimer(be<s32>* timer)
ext 025F795C u32 P2_msgGetStatus(u32 manager)
ext 025F74D0 void P2_msgSetStatus(u32 manager, u32 status)
ext 025F7DB0 u32 P2_msgSet(u32 manager, u32 message, cXyz* position)
ext 0254457C BOOL P2_endCheckOld(dEvent_manager_c* manager, const char* name)
ext 0256076C void dKy_instant_timechg(f32 time)
data l_msg_anm_tbl 1001FE14
data l_goal_pos 10468018 nocheck
data l_goal_pos_init 10468300 nocheck
data l_talk_pos 10468024 nocheck
data l_talk_pos_init 10468304 nocheck
data l_pos1 10468030 nocheck
data l_pos1_init 10468308 nocheck
data l_pos2 1046803C nocheck
data l_pos2_init 1046830C nocheck
data l_pos3 10468048 nocheck
data l_pos3_init 10468310 nocheck
str 1001FFA4 "P2B_INTRO" in 022BC738
str 1001FFB0 "Hlift_up" in 022BC79C
str 1001FFBC "P2B_TO_GOAL" in 022BC7F8
str 1001FFCC "P2B_GOAL_WAIT_TALK" in 022BCA04
str 1001FFFC "P2B_GOAL" in 022BD29C
str 10020008 "DEFAULT_TREASURE" in 022BD34C
str 1002001C "P2B_ARRIVE_MAJYU" in 022BD47C
str 10020030 "P2B_INTRO_2" in 022BD57C
str 1002003C "P2B_GOAL_2" in 022BD6B8
str 10020048 "P2B_BOMB_GET" in 022BD780
*/
