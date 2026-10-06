/**
 * d_a_npc_p2_act.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): messages, talk, the action functions and their modes.
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02588034 dLib_setFirstMsg(eventBit, firstMsg, secondMsg) */
static inline u32 p2_setFirstMsg(u16 bit, u32 first, u32 second) { return gabi::call<u32>(0x02588034, bit, first, second); }
/* 025881A4 dLib_checkPlayerInCircle(cXyz center (copy), f32 radius, f32 height) */
static inline BOOL p2_checkPlayerInCircle(cXyz* c, f32 r, f32 h) { return gabi::call<BOOL>(0x025881A4, c, r, h); }
/* 0211D2F8 cLib_calcTimer<int> (out of line) */
static inline s32 p2_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline void p2_floatCopy(cXyz* dst, cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}
/* the player is in the circle and not busy (virtual, vtable +0x4C): stop the goal ship (0x1DB) */
static inline void p2_goalCheck(daNpc_P2_c* a, fopAc_ac_c* player, cXyz* center, f32 r, f32 h, s8 mode) {
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), center);
    if (p2_checkPlayerInCircle(pos.get(), r, h)) {
        if (!gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(player) + 0xB4) + 0x4C), player)) {
            a->mMode = mode;
            gabi::Local<be<s16>> name;
            *name = 0x1DB;
            fopAc_ac_c* ship = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
            if (ship != nullptr) {
                gabi::store<u8>(gabi::ea(ship) + 0x3B4, 1);
            }
        }
    }
}
static inline void p2_setAttnDist(daNpc_P2_c* a, u8 v) {
    u32 attn = gabi::ea(a) + 0x388; /* attention_info.distances[] */
    gabi::store<u8>(attn + 1, v);
    gabi::store<u8>(attn + 3, v);
}

/* 022B89B8 */
u32 daNpc_P2_c::getMsg() {
    WWHD_FUNC(0x022B89B8, u32, this);
    u32 msg = 0;
    switch ((u8)mType) {
    case TYPE_P2A_e:
        if (p2_isEventBit(0x808)) {
            msg = 0x1010;
        } else {
            msg = p2_setFirstMsg(0x702, 0x100E, 0x100F);
        }
        break;
    case TYPE_P2B_e:
        if (mSubType == 1) {
            if (p2_isEventBit(0xF02)) {
                msg = p2_setFirstMsg(0x1502, 0x1B35, 0x1B36);
            } else {
                msg = 0x1028;
            }
        } else if (!p2_isEventBit(0x720)) {
            msg = p2_setFirstMsg(0x940, 0xC96, 0xC97);
        } else if (!p2_isEventBit(0x808)) {
            if (p2_isEventBit(0x710)) msg = 0x1028;
        } else {
            msg = p2_setFirstMsg(0x704, 0x1029, 0x102A);
        }
        break;
    case TYPE_P2C_e:
        if (p2_isEventBit(0x808)) {
            msg = 0x1013;
        } else {
            msg = p2_setFirstMsg(0x701, 0x1011, 0x1012);
        }
        break;
    }
    return msg;
}
VERIFY(0x022B89B8, &daNpc_P2_c::getMsg);

/* 022B8B24 */
void daNpc_P2_c::anmAtr(u16 i_status) {
    WWHD_FUNC(0x022B8B24, void, this, i_status);
    if (i_status == 6 /* fopMsgStts_MSG_TYPING_e */) {
        u8 attr = gabi::load<u8>(dComIfGp_ea() + 0x5BC5); /* dComIfGp_getMesgAnimeAttrInfo() */
        if (attr >= 0x15) {
            mAnm = 1;
            return;
        }
        s8 anm = (s8)gabi::load<u8>(0x1001FE14 + attr); /* l_msg_anm_tbl */
        u8 type = mType;
        if (type == TYPE_P2A_e) {
            if (mAnm == 0xD && anm == 0x17) return;
        } else if (type == TYPE_P2B_e) {
            if (anm == 7) {
                if (m8C9 == 0) {
                    m8C9 = 1;
                    mAnm = 7;
                }
                return;
            }
        }
        if (mAnm != anm) {
            mAnm = anm;
        }
    } else if (i_status == 0x10 /* fopMsgStts_MSG_ENDS_e */) {
        mAnm = 1;
    }
}
VERIFY(0x022B8B24, &daNpc_P2_c::anmAtr);

/* 022B8C14 */
u32 daNpc_P2_c::next_msgStatus(u32* pMsgNo) {
    WWHD_FUNC(0x022B8C14, u32, this, pMsgNo);
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
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}
VERIFY(0x022B8C14, &daNpc_P2_c::next_msgStatus);

/* 022B8E24 */
u16 daNpc_P2_c::talk(bool i_noGetMsg) {
    WWHD_FUNC(0x022B8E24, u16, this, i_noGetMsg);
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
        anmAtr(0x12);
        return 0x12;
    }
    if (p2_msgId() == 0xFFFFFFFF) {
        p2_msgId() = p2_msgSet(mgr, mMsgNo, &eyePos);
        return 0xFF;
    }
    if ((u32)(s32)state == 1) {
        mTalkState = 2;
        return 0xFF;
    }
    if ((u32)(s32)state != 2) {
        return 0xFF;
    }
    u16 status = (u16)p2_msgGetStatus(mgr);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        p2_msgSetStatus(mgr, next_msgStatus((u32*)&mMsgNo));
        if (p2_msgGetStatus(mgr) == 0xF) {
            p2_msgSet(mgr, mMsgNo, nullptr);
            anmAtr(status);
            return status;
        }
    } else if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        p2_msgSetStatus(mgr, 0x13);
        mTalkState = -1;
        m8C9 = 0;
    }
    anmAtr(status);
    return status;
}
VERIFY(0x022B8E24, &daNpc_P2_c::talk);

/* 022BC1D8 */
void daNpc_P2_c::wait01() {
    WWHD_FUNC(0x022BC1D8, void, this);
    if (mbTalk) {
        mMode = 2;
    } else {
        mOrder = 2;
    }
}
VERIFY(0x022BC1D8, &daNpc_P2_c::wait01);

/* 022BC1FC */
void daNpc_P2_c::moccowait() {
    WWHD_FUNC(0x022BC1FC, void, this);
    u32 pid = parentActorID;
    if (pid != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = pid;
        fopAc_ac_c* ship = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
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
        if (p2_calcTimer(&mMoccoTimer) == 0) {
            mAnm = 0x15;
            mMoccoTimer = (s32)(s16)gabi::ftoi(cM_rndF(100.0f) + 200.0f);
        }
    }
    if (mAnm == 0x15 && p2_isStop(mpMorf)) {
        mAnm = 1;
    }
    if (mbTalk && mAnm == 1) {
        mMode = 2;
    } else {
        mOrder = 2;
    }
}
VERIFY(0x022BC1FC, &daNpc_P2_c::moccowait);

/* 022BC3AC */
void daNpc_P2_c::zukotelescope() {
    WWHD_FUNC(0x022BC3AC, void, this);
    u32 pid = parentActorID;
    mAnm = 0x16;
    if (pid != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = pid;
        fopAc_ac_c* ship = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
        s16 y = ship != nullptr ? (s16)ship->current.angle.y : (s16)0;
        cLib_addCalcAngleS2(&current.angle.y, (s16)(mShipAngleOffs + y), 4, 0x800);
    }
    if (mbTalk) {
        mAnm = 1;
        m_jnt.mbBackBoneLock = 0;
        mMode = 2;
        m_jnt.mbHeadLock = 0;
    } else {
        mOrder = 2;
    }
}
VERIFY(0x022BC3AC, &daNpc_P2_c::zukotelescope);

/* 022BC45C */
void daNpc_P2_c::talk01() {
    WWHD_FUNC(0x022BC45C, void, this);
    if (talk(false) == 0x12) {
        u8 type = mType;
        if (type == TYPE_P2C_e) {
            mAnm = 1;
            mMode = 0x10;
        } else if (type == TYPE_P2A_e && !p2_isEventBit(0x808)) {
            mMode = 0x11;
            m_jnt.mbHeadLock = 1;
            m_jnt.mbBackBoneLock = 1;
        } else {
            mAnm = 1;
            mMode = 1;
        }
        p2_event_reset();
        mbTalk = 0;
    }
}
VERIFY(0x022BC45C, &daNpc_P2_c::talk01);

/* 022BC544 */
BOOL daNpc_P2_c::wait_action(void*) {
    WWHD_FUNC(0x022BC544, BOOL, this, (u32)0);
    s8 status = mActionStatus;
    if (status == 0) {
        u8 type = mType;
        if (type == TYPE_P2C_e) {
            mMode = 0x10; /* moccowait */
        } else if (type == TYPE_P2A_e && !p2_isEventBit(0x808)) {
            mMode = 0x11; /* zukotelescope */
        } else {
            mMode = 1;
        }
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status != -1) {
        mbAttention = (u8)chkAttention();
        mOrder = 0;
        switch ((u32)(s32)mMode) {
        case 1: wait01(); break;
        case 2: talk01(); break;
        case 0x10: moccowait(); break;
        case 0x11: zukotelescope(); break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}
VERIFY(0x022BC544, &daNpc_P2_c::wait_action);

/* 022BC6BC */
void daNpc_P2_c::demo_wait() {
    WWHD_FUNC(0x022BC6BC, void, this);
    daNpc_P2_childHIO_c* c = p2_child(mType);
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), &c->mDemoWaitPos);
    if (p2_checkPlayerInCircle(pos.get(), c->mDemoWaitR, c->mDemoWaitH)) {
        mOrder = 3;
        mMode = 4;
    }
}
VERIFY(0x022BC6BC, &daNpc_P2_c::demo_wait);

/* 022BC738 */
void daNpc_P2_c::demo_intro() {
    WWHD_FUNC(0x022BC738, void, this);
    if (p2_endCheckOld(0x1001FFA4 /* "P2B_INTRO" */)) {
        mMode = 5;
        mOrder = 0;
        p2_event_reset();
    }
}
VERIFY(0x022BC738, &daNpc_P2_c::demo_intro);

/* 022BC79C */
void daNpc_P2_c::demo_lift() {
    WWHD_FUNC(0x022BC79C, void, this);
    if (p2_endCheckOld(0x1001FFB0 /* "Hlift_up" */)) {
        mMode = 6;
        mOrder = 4;
        m978 = 0;
    }
}
VERIFY(0x022BC79C, &daNpc_P2_c::demo_lift);

/* 022BC7F8 */
void daNpc_P2_c::demo_jump() {
    WWHD_FUNC(0x022BC7F8, void, this);
    if (p2_endCheckOld(0x1001FFBC /* "P2B_TO_GOAL" */)) {
        p2_onEventBit(0x720);
        mMode = 9;
        p2_event_reset();
    }
}
VERIFY(0x022BC7F8, &daNpc_P2_c::demo_jump);

/* 022BC868 */
void daNpc_P2_c::goal_talkpos_wait() {
    WWHD_FUNC(0x022BC868, void, this);
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((current.pos.y - player->current.pos.y) - 300.0f < 0.0f) {
        mMode = 0xA;
        return;
    }
    p2_setAttnDist(this, 0x22);
    if (m8C2 == 0) {
        m8C2 = 1;
        mOrder = 1;
        return;
    }
    if (mbTalk) {
        mMode = 8;
        p2_event_reset();
        mOrder = 5;
    } else if (player->speedF < 1.0f) {
        mOrder = 2;
    }
    daNpc_P2_childHIO_c* c = p2_child(mType);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, 0xC);
}
VERIFY(0x022BC868, &daNpc_P2_c::goal_talkpos_wait);

/* 022BCA04 */
void daNpc_P2_c::goal_talkpos_talk() {
    WWHD_FUNC(0x022BCA04, void, this);
    if (p2_endCheckOld(0x1001FFCC /* "P2B_GOAL_WAIT_TALK" */)) {
        mMode = 7;
        p2_event_reset();
        mbTalk = 0;
    }
}
VERIFY(0x022BCA04, &daNpc_P2_c::goal_talkpos_talk);

/* 022BCA68 */
void daNpc_P2_c::goal_goalpos_to_talkpos() {
    WWHD_FUNC(0x022BCA68, void, this);
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((current.pos.y - player->current.pos.y) - 300.0f < 0.0f) {
        mMode = 0xA;
        return;
    }
    p2_setAttnDist(this, 0);
    /* static cXyz l_goal_pos(0, -450, -2915), l_talk_pos(0, -450, -2715) */
    if (gabi::load<u32>(0x10468300) == 0) {
        gabi::store<f32>(0x10468018, 0.0f);
        gabi::store<u32>(0x10468300, 1);
        gabi::store<f32>(0x1046801C, -450.0f);
        gabi::store<f32>(0x10468020, -2915.0f);
    }
    if (gabi::load<u32>(0x10468304) == 0) {
        gabi::store<f32>(0x10468024, 0.0f);
        gabi::store<u32>(0x10468304, 1);
        gabi::store<f32>(0x1046802C, -2715.0f);
        gabi::store<f32>(0x10468028, -450.0f);
    }
    cXyz* goal = gabi::at<cXyz>(0x10468018);
    s16 ang = cLib_targetAngleY(&current.pos, goal);
    s32 d = (s32)ang - (s32)current.angle.y;
    p2_copy12(gabi::ea(&mLookPos), 0x10468024);
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
        mMode = 7;
    }
    daNpc_P2_childHIO_c* c = p2_child(type);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, 0xC);
}
VERIFY(0x022BCA68, &daNpc_P2_c::goal_goalpos_to_talkpos);

/* 022BCD60 */
void daNpc_P2_c::goal_talkpos_to_goalpos() {
    WWHD_FUNC(0x022BCD60, void, this);
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    if (dist < p2_child(mType)->mGoalDist) {
        if ((current.pos.y - player->current.pos.y) - 300.0f > 0.0f) {
            mMode = 9;
            return;
        }
    }
    p2_setAttnDist(this, 0);
    /* static cXyz l_pos1(0, -450, -3130), l_pos2(0, -450, -3380), l_pos3(0, -450, -2715) */
    if (gabi::load<u32>(0x10468308) == 0) {
        gabi::store<f32>(0x10468030, 0.0f);
        gabi::store<u32>(0x10468308, 1);
        gabi::store<f32>(0x10468034, -450.0f);
        gabi::store<f32>(0x10468038, -3130.0f);
    }
    if (gabi::load<u32>(0x1046830C) == 0) {
        gabi::store<f32>(0x10468040, -450.0f);
        gabi::store<f32>(0x1046803C, 0.0f);
        gabi::store<u32>(0x1046830C, 1);
        gabi::store<f32>(0x10468044, -3380.0f);
    }
    if (gabi::load<u32>(0x10468310) == 0) {
        gabi::store<f32>(0x1046804C, -450.0f);
        gabi::store<f32>(0x10468048, 0.0f);
        gabi::store<u32>(0x10468310, 1);
        gabi::store<f32>(0x10468050, -2715.0f);
    }
    cXyz* pos1 = gabi::at<cXyz>(0x10468030);
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
        p2_copy12(gabi::ea(&mLookPos), 0x10468048);
        ang = cLib_targetAngleY(&current.pos, &mLookPos);
    } else {
        p2_copy12(gabi::ea(&mLookPos), 0x1046803C);
        ang = cLib_targetAngleY(&current.pos, &mLookPos);
    }
    s32 da = (s32)ang - (s32)current.angle.y;
    if (d < 5.0f) {
        if (fabsf((f32)da) < 5376.0f) {
            mAnm = 1;
            mMode = 0xB;
        }
    } else if (fabsf((f32)da) < 5376.0f) {
        mAnm = 4;
        cLib_addCalc2(&current.pos.x, pos1->x, 0.1f, 4.0f);
        cLib_addCalc2(&current.pos.z, pos1->z, 0.1f, 4.0f);
    }
    daNpc_P2_childHIO_c* c = p2_child(mType);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, 0xC);
}
VERIFY(0x022BCD60, &daNpc_P2_c::goal_talkpos_to_goalpos);

/* 022BD14C */
void daNpc_P2_c::goal_goalpos_wait() {
    WWHD_FUNC(0x022BD14C, void, this);
    mbBigCyl = 1;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    if (dist < p2_child(mType)->mGoalDist) {
        if ((current.pos.y - player->current.pos.y) - 300.0f > 0.0f) {
            mMode = 9;
            return;
        }
    }
    u8 type = mType;
    p2_setAttnDist(this, 2);
    daNpc_P2_childHIO_c* c = p2_child(type);
    p2_goalCheck(this, player, &c->mGoalTalkPos, c->mGoalTalkR, c->mGoalTalkH, 0xC);
}
VERIFY(0x022BD14C, &daNpc_P2_c::goal_goalpos_wait);

/* 022BD29C */
void daNpc_P2_c::demo_goal() {
    WWHD_FUNC(0x022BD29C, void, this);
    mbBigCyl = 0;
    p2_setAttnDist(this, 0xA9);
    if (p2_endCheckOld(0x1001FFFC /* "P2B_GOAL" */)) {
        p2_onEventBit(0x710);
        mOrder = 0;
        mMode = 0xD;
        p2_event_reset();
    } else {
        mOrder = 6;
    }
}
VERIFY(0x022BD29C, &daNpc_P2_c::demo_goal);

/* 022BD34C */
void daNpc_P2_c::treasure_wait() {
    WWHD_FUNC(0x022BD34C, void, this);
    if (p2_endCheckOld(0x10020008 /* "DEFAULT_TREASURE" */)) {
        u8 sub = mSubType;
        if (sub == 0) {
            mMode = 0xF;
            mOrder = 7;
        } else if (sub == 1) {
            mMode = 0x16;
            mOrder = 0xA;
        }
        return;
    }
    if (mbTalk) {
        mMode = 0xE;
    } else {
        mOrder = 2;
    }
}
VERIFY(0x022BD34C, &daNpc_P2_c::treasure_wait);

/* 022BD41C */
void daNpc_P2_c::treasure_wait_talk() {
    WWHD_FUNC(0x022BD41C, void, this);
    mbNoAttention = 1;
    if (talk(false) == 0x12) {
        mMode = 0xD;
        mbTalk = 0;
        p2_event_reset();
    }
}
VERIFY(0x022BD41C, &daNpc_P2_c::treasure_wait_talk);

/* 022BD47C */
void daNpc_P2_c::demo_arrive() {
    WWHD_FUNC(0x022BD47C, void, this);
    if (p2_endCheckOld(0x1002001C /* "P2B_ARRIVE_MAJYU" */)) {
        gabi::call(0x0256076C, 300.0f); /* dKy_instant_timechg */
        p2_onEventBit(0x808);
        mMode = 1;
        mOrder = 0;
        p2_event_reset();
    }
}
VERIFY(0x022BD47C, &daNpc_P2_c::demo_arrive);

/* 022BD500 */
void daNpc_P2_c::demo_wait_2() {
    WWHD_FUNC(0x022BD500, void, this);
    daNpc_P2_childHIO_c* c = p2_child(mType);
    gabi::Local<cXyz> pos;
    p2_floatCopy(pos.get(), &c->mDemoWaitPos);
    if (p2_checkPlayerInCircle(pos.get(), c->mDemoWaitR, c->mDemoWaitH)) {
        mOrder = 8;
        mMode = 0x13;
    }
}
VERIFY(0x022BD500, &daNpc_P2_c::demo_wait_2);

/* 022BD57C */
void daNpc_P2_c::demo_intro_2() {
    WWHD_FUNC(0x022BD57C, void, this);
    if (p2_endCheckOld(0x10020030 /* "P2B_INTRO_2" */)) {
        p2_onEventBit(0x1A04);
        mMode = 0x14;
        mOrder = 0;
        p2_event_reset();
    }
}
VERIFY(0x022BD57C, &daNpc_P2_c::demo_intro_2);

/* 022BD5F4 */
void daNpc_P2_c::goal_wait_2() {
    WWHD_FUNC(0x022BD5F4, void, this);
    dComIfGp_get(); /* HD: an unused call */
    if (dComIfGs_isSwitch(mSwitchNo, home.roomNo)) {
        daNpc_P2_childHIO_c* c = p2_child(mType);
        gabi::Local<cXyz> pos;
        p2_floatCopy(pos.get(), &c->mGoalPos2);
        if (p2_checkPlayerInCircle(pos.get(), c->mGoal2R, c->mGoal2H)) {
            mMode = 0x15;
            gabi::Local<be<s16>> name;
            *name = 0x1DB;
            fopAc_ac_c* ship = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
            if (ship != nullptr) {
                gabi::store<u8>(gabi::ea(ship) + 0x3B4, 1);
            }
        }
    }
}
VERIFY(0x022BD5F4, &daNpc_P2_c::goal_wait_2);

/* 022BD6B8 */
void daNpc_P2_c::demo_goal_2() {
    WWHD_FUNC(0x022BD6B8, void, this);
    if (p2_endCheckOld(0x1002003C /* "P2B_GOAL_2" */)) {
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> res;
        p2_floatCopy(pos.get(), &mEventCut.mPos);
        mbBigCyl = 1;
        mOrder = 0;
        mMode = 0xD;
        cXyz_pl(pos.get(), res.get(), &mLookOffs);
        p2_copy12(gabi::ea(&mGoalLookPos), gabi::ea(res.get()));
        p2_event_reset();
    } else {
        mOrder = 9;
    }
}
VERIFY(0x022BD6B8, &daNpc_P2_c::demo_goal_2);

/* 022BD780 */
void daNpc_P2_c::demo_bomb_get() {
    WWHD_FUNC(0x022BD780, void, this);
    if (p2_endCheckOld(0x10020048 /* "P2B_BOMB_GET" */)) {
        p2_onEventBit(0xF02);
        mMode = 1;
        mbBigCyl = 0;
        mOrder = 0;
        p2_event_reset();
    }
}
VERIFY(0x022BD780, &daNpc_P2_c::demo_bomb_get);

/* 022BD7FC */
BOOL daNpc_P2_c::intro_action(void*) {
    WWHD_FUNC(0x022BD7FC, BOOL, this, (u32)0);
    s8 status = mActionStatus;
    if (status == 0) {
        s8 mode;
        if (mSubType == 0) {
            if (p2_isEventBit(0x720)) {
                mMode = 0xA;
                mActionStatus = (s8)(mActionStatus + 1);
                return TRUE;
            }
            mode = 3;
        } else {
            mode = p2_isEventBit(0x1A04) ? 0x14 : 0x12;
        }
        mMode = mode;
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status != -1) {
        mbAttention = (u8)chkAttention();
        mOrder = 0;
        switch ((u32)(s32)mMode) {
        case 1: wait01(); break;
        case 2: talk01(); break;
        case 3: demo_wait(); break;
        case 4: demo_intro(); break;
        case 5: demo_lift(); break;
        case 6: demo_jump(); break;
        case 7: goal_talkpos_wait(); break;
        case 8: goal_talkpos_talk(); break;
        case 9: goal_goalpos_to_talkpos(); break;
        case 0xA: goal_talkpos_to_goalpos(); break;
        case 0xB: goal_goalpos_wait(); break;
        case 0xC: demo_goal(); break;
        case 0xD: treasure_wait(); break;
        case 0xE: treasure_wait_talk(); break;
        case 0xF: demo_arrive(); break;
        case 0x12: demo_wait_2(); break;
        case 0x13: demo_intro_2(); break;
        case 0x14: goal_wait_2(); break;
        case 0x15: demo_goal_2(); break;
        case 0x16: demo_bomb_get(); break;
        }
        lookBack();
        setAttention();
    }
    return TRUE;
}
VERIFY(0x022BD7FC, &daNpc_P2_c::intro_action);
