/**
 * d_a_npc_kk1_b.cpp (WWHD)
 * NPC - Mila (poor, Windfall)
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 * Part B: event cuts (cut_init_* / cut_move_*), event_move, privateCut, event_proc, kyoroPos,
 * kyorokyoro, lookBack, eventOrder, _execute, _draw.
 */
#define SAFESTRING_VTBL 0x1001C250 // HD: this translation unit's sead::SafeString vtable.
#include "d/actor/d_a_npc_kk1_b.h"

static constexpr u32 kk1PlayerEventFlag = 0x08000000;

static const char* cutNames[] = {
    "RUN_START", "RUN", "CATCH_START", "CATCH_END", "TRN", "BYE_START", "BYE",
    "BYE_END", "PLYER_TRN", "OTOBOKE", "PLYER_MOV", "RUNAWAY_START", "RUNAWAY_END", "BYE_CONTINUE",
};

static u8 turnPointTable[16] = {40,39,38,37,36,35,34,33,34,35,36,37,38,39,40,41};
static cXyz lookOffsets[11] = {
    {0,0,0}, {-100,0,0}, {100,0,0}, {-100,0,100}, {100,0,100}, {0,0,100},
    {-100,40,0}, {100,40,0}, {-100,40,100}, {100,40,100}, {0,40,100},
};
static u32 debugGreenGuard;
static u32 debugGreen;
static u32 debugGreenInitial = 0x00FF0080;
static u32 debugYellowGuard;
static u32 debugYellow;
static u32 debugYellowInitial = 0xFFFF0080;
static u32 debugRedGuard;
static u32 debugRed;
static u32 debugRedInitial = 0xFF000080;
static u32 debugBlueGuard;
static u32 debugBlue;
static u32 debugBlueInitial = 0x0000FF80;

// HD: Link is fetched separately at each original access; combined demo stores use one fetch.
static daPy_py_c* kk1Player() { return daPy_getPlayerLinkActorClass(); }
static void kk1SetDemoTypeParam(u16 type, s32 param) {
    daPy_py_c* player = kk1Player();
    player->mDemo.mDemoType = type;
    player->mDemo.mParam0 = param;
}
static void kk1SetDemoTypeMode(u16 type, u32 mode) {
    daPy_py_c* player = kk1Player();
    player->mDemo.mDemoType = type;
    player->mDemo.mDemoMode = mode;
}
static void kk1SetPlayerAngle(s16 angle) {
    daPy_py_c* player = kk1Player();
    Kk1PlayerVtable* vtable = reinterpret_cast<Kk1PlayerDispatch*>(player)->mVtable.get();
    cXyz* pos = &kk1Player()->current.pos;
    // HD: retain the vtable load before the second player-pointer fetch.
    gabi::call_ptr(vtable->setPlayerPosAndAngle.get(), player, pos, angle);
}

// HD: a GHS cXyz assignment copies integer words, preserving NaN payloads.
static inline void copyPositionWords(cXyz* dst, const cXyz* src) {
    *dst = *src;
}

// HD: preserve explicit debug-color guards and the original memcpy call.
static inline void initDebugColor(be<u32>* guard, be<u32>* dst, const be<u32>* src) {
    if (*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}


/* 0226E92C */
void daNpc_Kk1_c::eventOrder() {
    Kk1EventState& state = kk1Event(this);
    s8 order = (s8)state.mEventOrder;
    if (order == 1 || order == 2) {
        s8 v = (s8)state.mEventOrder;
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (v == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        s16 idx = (s16)(order - 3);
        state.mEventIndex = idx;
        fopAcM_orderOtherEventId(this, state.mEventIds[idx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 0226E604 */
void daNpc_Kk1_c::lookBack() {
    Kk1EventState& state = kk1Event(this);
    state.mLookAngle.y = m_jnt.mAngles[0][1];
    s16 yrot = current.angle.y;
    f32 srcX = current.pos.x;
    state.mLookAngle.x = yrot;
    cXyz dst;
    dst.x = 0.0f;
    u8 headOnly = state.mHeadOnly;
    f32 srcZ = current.pos.z;
    dst.y = 0.0f;
    f32 srcY = eyePos.y;
    cXyz* target = nullptr;
    dst.z = 0.0f;
    state.mLookAngle.z = m_jnt.mAngles[1][1];
    switch ((u32)(s32)(s8)state.mLookMode) {
    case 1: {
        cXyz eye;
        dNpc_playerEyePos(&eye, -20.0f);
        copyPositionWords(&dst, &eye);
        copyPositionWords(&state.mLookPos, &eye);
        target = &dst;
        break;
    }
    case 2:
        copyPositionWords(&dst, &state.mLookPos);
        target = &dst;
        break;
    case 3:
        yrot = state.mLookYaw;
        break;
    case 4: {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* actor = searchByID(state.mLookActorId, notFound);
        if (actor != nullptr && *notFound == 0) {
            copyPositionWords(&state.mLookPos, &actor->current.pos);
            f32 x = state.mLookPos.x;
            f32 eyeY = actor->eyePos.y;
            f32 z = state.mLookPos.z;
            dst.y = eyeY;
            dst.z = z;
            state.mLookPos.y = eyeY;
            dst.x = x;
            target = &dst;
        }
        break;
    }
    case 5:
        kyorokyoro();
        copyPositionWords(&dst, &state.mLookPos);
        target = &dst;
        break;
    }
    cXyz eye; /* passed by value: a copy */
    eye.x = srcX;
    eye.z = srcZ;
    eye.y = srcY;
    kk1LookAtTarget(&m_jnt, &current.angle.y, target, &eye, yrot, kk1Parameters().mLookVelocity, headOnly);
}

/* 0226CC80 */
bool daNpc_Kk1_c::cut_init_RUN_START(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    gabi::Local<be<s32>> res;
    fopAc_ac_c* a_actor = searchByID(state.mPartnerId, res);
    if (a_actor == nullptr) /* JUT_ASSERT(1420, a_actor != NULL) */
        JUT_ASSERT_fail("d_a_npc_kk1.cpp", 0x58C, "a_actor != NULL");
    dEvt_control_c* event = dComIfGp_getEvent();
    event->mPtItem = kk1EventActorId(event, a_actor);
    return kk1PathNext(&mPathRun);
}

/* 0226D6CC */
bool daNpc_Kk1_c::cut_move_RUN_START() {
    cXyz pnt;
    kk1PathPoint(&pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, &pnt);
    cLib_addCalcAngleS(&current.angle.y, ang, kk1Parameters().mTurnRate, kk1Parameters().mTurnStep, 0x80);
    if ((u32)(s32)current.angle.y == (u32)(s32)ang) {
        kk1SetDemoTypeMode(2, 1);
        return true;
    }
    return false;
}

// HD: the special path point is shifted by two debug registers.
/* 0226CD04 */
void daNpc_Kk1_c::cut_init_RUN(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Timer" /* "Timer" */);
    state.mCutTimer = -1;
    if (timer != nullptr) {
        state.mCutTimer = (s16)(s32)*timer;
    }
    setAnm_NUM(4, 1);
    state.mFollowPath = 1;
    state.mMoveAnm = 2;
    state.mPathArg = 0;
}

/* 0226DB64 */
s32 daNpc_Kk1_c::cut_move_RUN() {
    Kk1EventState& state = kk1Event(this);
    s32 ret = event_move(false);
    if (state.mCutTimer >= 0) {
        ret = cLib_calcTimer(&state.mCutTimer) == 0;
    }
    return ret;
}

/* 0226CD94 */
void daNpc_Kk1_c::cut_init_CATCH_START(int staffIdx) {
    kk1Player()->onNoResetFlg0(kk1PlayerEventFlag);
    setAnm_NUM(8, 1);
    mpMorf->setMorf(0.0f);
}

/* 0226DBB0 */
bool daNpc_Kk1_c::cut_move_CATCH_START() {
    Kk1EventState& state = kk1Event(this);
    if ((s8)state.mAnmEnded != 0) {
        state.mMessageHandle = 0;
        return true;
    }
    return false;
}

/* 0226CDF0 */
void daNpc_Kk1_c::cut_init_CATCH_END(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    kk1Player()->offNoResetFlg0(kk1PlayerEventFlag);
    current.angle.y = current.angle.y - 0x8000;
    setAnm_NUM(0, 1);
    mpMorf->setMorf(0.0f);
    m_jnt.mbTrn = 1;
    state.mHeadOnly = 0;
    state.mLookMode = 1;
}

/* 0226CE6C */
void daNpc_Kk1_c::cut_init_TRN(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    if ((s8)state.mPathArg != 5) {
        u8 orgIdx = mPathRun.mIdx;
        if ((u32)(orgIdx - 0x19) < 0x10) {
            // HD: the local turn-around table is copied from read-only data.
            u8 next = turnPointTable[orgIdx - 0x19];
            state.mLookMode = 0;
            state.mHeadOnly = 1;
            mPathRun.mIdx = next;
            return;
        }
        kk1PathFindNearest(&mPathRun, &kk1Player()->current.pos, 100.0f);
        u8 plIdx = mPathRun.mIdx;
        kk1PathFindNearest(&mPathRun, &current.pos, 100.0f);
        u8 myIdx = mPathRun.mIdx;
        s32 maxPnt = kk1PathPointCount(&mPathRun);
        s16 half = (s16)gabi::ftoi(gabi::fmadds((f32)maxPnt, 0.5f, 0.5f));
        if (myIdx > plIdx) {
            plIdx = (u8)(plIdx + kk1PathPointCount(&mPathRun));
        }
        s16 diff = (s16)(plIdx - myIdx);
        if (diff > half) {
            diff = (s16)(diff - maxPnt);
        }
        mPathRun.mIdx = orgIdx;
        if (diff >= 0) {
            bool back = true;
            if (diff <= 0) {
                cXyz pnt;
                kk1PathPoint(&pnt, &mPathRun);
                cXyz d;
                cXyz_mi(&current.pos, &d, &pnt);
                cXyz xz1;
                xz1.x = d.x;
                xz1.y = 0.0f;
                xz1.z = d.z;
                f32 myDist = std_sqrtf(PSVECSquareMag(&xz1));
                cXyz_mi(&kk1Player()->current.pos, &d, &pnt);
                cXyz xz2;
                xz2.x = d.x;
                xz2.y = 0.0f;
                xz2.z = d.z;
                back = std_sqrtf(PSVECSquareMag(&xz2)) < myDist; /* mfcr: the LT bit */
            }
            if (back) {
                kk1PathPrevious(&mPathRun);
                kk1PathPrevious(&mPathRun);
                mPathRun.mbDir = mPathRun.mbDir ^ 1;
            }
        }
    }
    state.mLookMode = 0;
    state.mHeadOnly = 1;
}

/* 0226DBD4 */
bool daNpc_Kk1_c::cut_move_TRN() {
    cXyz pnt;
    kk1PathPoint(&pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, &pnt);
    cLib_addCalcAngleS(&current.angle.y, ang, kk1Parameters().mTurnRate, kk1Parameters().mTurnStep, 0x80);
    s16 y = current.angle.y;
    if ((u32)(s32)y == (u32)(s32)ang) {
        shape_angle.y = y;
        return true;
    }
    return false;
}

/* 0226D0A4 */
void daNpc_Kk1_c::cut_init_BYE_START(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    kk1Player()->onNoResetFlg0(kk1PlayerEventFlag);
    state.mHideBody = 1;
}

/* 0226D0E8 */
void daNpc_Kk1_c::cut_init_BYE(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Timer" /* "Timer" */);
    be<s32>* delay = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Delay" /* "Delay" */);
    be<s32>* prm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "prm_0" /* "prm_0" */);
    state.mCutTimer = 30;
    if (timer != nullptr) {
        state.mCutTimer = (s16)(s32)*timer;
    }
    state.mTurnTimer = -1;
    if (delay != nullptr) {
        state.mTurnTimer = (s16)(s32)*delay;
    }
    state.mKeepMoving = 0;
    if (prm0 != nullptr) {
        state.mKeepMoving = (s16)(s32)*prm0;
    }
    setAnm_NUM(4, 1);
    state.mMoveAnm = 2;
    state.mFollowPath = 1;
}

/* 0226DC70 */
bool daNpc_Kk1_c::cut_move_BYE() {
    Kk1EventState& state = kk1Event(this);
    event_move(false);
    if (state.mTurnTimer > 0 && cLib_calcTimer(&state.mTurnTimer) == 0) {
        kk1SetPlayerAngle(-0x3217);
        kk1SetDemoTypeParam(3, 0);
        kk1Player()->mDemo.setDemoMode(9);
        kk1Player()->mDemo.setParam0(0x3217);
    }
    if (cLib_calcTimer(&state.mCutTimer) == 0) {
        if (state.mKeepMoving == 0) {
            speedF = 0.0f;
        }
        return true;
    }
    return false;
}

/* 0226D664 */
void daNpc_Kk1_c::cut_init_BYE_CONTINUE(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Timer" /* "Timer" */);
    state.mCutTimer = 30;
    if (timer != nullptr) {
        state.mCutTimer = (s16)(s32)*timer;
    }
}

/* 0226DE88 */
bool daNpc_Kk1_c::cut_move_BYE_CONTINUE() {
    Kk1EventState& state = kk1Event(this);
    event_move(false);
    if (cLib_calcTimer(&state.mCutTimer) == 0) {
        speedF = 0.0f;
        return true;
    }
    return false;
}

/* 0226D1F0 */
void daNpc_Kk1_c::cut_init_BYE_END(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    kk1SetDemoTypeParam(3, 0);
    kk1Player()->mDemo.setDemoMode(4);
    s16 ang = cLib_targetAngleY(&kk1Player()->current.pos, &current.pos);
    kk1SetPlayerAngle(ang);
    kk1Player()->offNoResetFlg0(kk1PlayerEventFlag);
    state.mHide = 1;
}

/* 0226D2B8 */
void daNpc_Kk1_c::cut_init_OTOBOKE(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    kk1SetDemoTypeParam(3, 0);
    kk1Player()->mDemo.setDemoMode(4);
    daPy_py_c* player = kk1Player();
    Kk1PlayerVtable* vtable = reinterpret_cast<Kk1PlayerDispatch*>(player)->mVtable.get();
    cXyz* pos = &kk1Player()->current.pos;
    gabi::call_ptr(vtable->setPlayerPosAndAngle.get(), player, pos, (s16)current.angle.y);
    state.mCutTimer = 2;
}

/* 0226DD7C */
bool daNpc_Kk1_c::cut_move_OTOBOKE() {
    Kk1EventState& state = kk1Event(this);
    if (cLib_calcTimer(&state.mCutTimer) == 0) {
        kk1SetDemoTypeMode(2, 1);
        return true;
    }
    return false;
}

/* 0226D34C */
void daNpc_Kk1_c::cut_init_PLYER_MOV(int staffIdx) {
    s16 diff = (s16)(cLib_targetAngleY(&current.pos, &kk1Player()->current.pos) - current.angle.y);
    s32 adiff = diff < 0 ? -diff : diff;
    if (adiff > 0x2000) {
        cXyz* pos = &kk1Player()->current.pos;
        dComIfGp_evmng_setGoal(pos);
        return;
    }
    cXyz offs;
    offs.y = 0.0f;
    offs.x = 0.0f;
    s16 turn = -0x2800;
    offs.z = 0.0f;
    if (diff > 0) {
        turn = 0x2800;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM((s16)(current.angle.y + turn));
    offs.z = 150.0f;
    cXyz goal;
    PSMTXMultVec(mDoMtx_stack_c::get(), &offs, &goal);
    dComIfGp_evmng_setGoal(&goal);
}

/* 0226D454 */
void daNpc_Kk1_c::cut_init_RUNAWAY_START(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Timer" /* "Timer" */);
    kk1SetDemoTypeParam(3, 0);
    kk1Player()->mDemo.setDemoMode(4);
    s16 ang = cLib_targetAngleY(&kk1Player()->current.pos, &current.pos);
    kk1SetPlayerAngle(ang);
    s16 toPlayer = cLib_targetAngleY(&current.pos, &kk1Player()->current.pos);
    m_jnt.mAngles[0][0] = 0;
    s16 away = (s16)(toPlayer - 0x8000);
    state.mLookMode = 0;
    s16 diff = (s16)(toPlayer - current.angle.y);
    state.mPreserveShapeAngle = 1;
    m_jnt.mAngles[1][0] = 0;
    shape_angle.y = away;
    m_jnt.mAngles[0][1] = 0;
    speedF = 0.0f;
    m_jnt.mAngles[1][1] = 0;
    s32 adiff = diff < 0 ? -diff : diff;
    if (adiff > 0x3800) {
        current.angle.y = away;
        setAnm_NUM(8, 1);
        return;
    }
    state.mCutTimer = 0x26;
    if (timer != nullptr) {
        state.mCutTimer = (s16)(s32)*timer;
    }
    current.angle.y = toPlayer;
    setAnm_NUM(0, 1);
    cXyz pos;
    pos.x = 0.0f;
    pos.z = 0.0f;
    pos.y = -50.0f;
    setBikon(&pos);
}

/* 0226DDD8 */
bool daNpc_Kk1_c::cut_move_RUNAWAY_START() {
    Kk1EventState& state = kk1Event(this);
    if ((s8)state.mAnmState != 8) {
        return true;
    }
    if ((s8)state.mAnmEnded == 0) {
        return false;
    }
    current.angle.y = cLib_targetAngleY(&current.pos, &kk1Player()->current.pos);
    setAnm_NUM(0, 1);
    mpMorf->setMorf(0.0f);
    return true;
}

/* 0226D60C */
void daNpc_Kk1_c::cut_init_RUNAWAY_END(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    kk1Player()->offNoResetFlg0(kk1PlayerEventFlag);
    state.mFollowPath = 0;
    state.mHide = 1;
    speedF = 0.0f;
}

/* 0226DEF0 */
void daNpc_Kk1_c::privateCut(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    if (staffIdx == -1) {
        return;
    }
    
    s8 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, gabi::ea(cutNames), 14, TRUE, 0);
    state.mCutIndex = actIdx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        kk1EventCutEnd(evmng, staffIdx);
        return;
    }
    if (kk1EventAdvanced(evmng, staffIdx)) { 
        switch ((u32)(s32)(s8)state.mCutIndex) {
        case KK1_CUT_RUN_START: cut_init_RUN_START(staffIdx); break;
        case KK1_CUT_RUN: cut_init_RUN(staffIdx); break;
        case KK1_CUT_CATCH_START: cut_init_CATCH_START(staffIdx); break;
        case KK1_CUT_CATCH_END: cut_init_CATCH_END(staffIdx); break;
        case KK1_CUT_TRN: cut_init_TRN(staffIdx); break;
        case KK1_CUT_BYE_START: cut_init_BYE_START(staffIdx); break;
        case KK1_CUT_BYE: cut_init_BYE(staffIdx); break;
        case KK1_CUT_BYE_END: cut_init_BYE_END(staffIdx); break;
        case KK1_CUT_OTOBOKE: cut_init_OTOBOKE(staffIdx); break;
        case KK1_CUT_PLYER_MOV: cut_init_PLYER_MOV(staffIdx); break;
        case KK1_CUT_RUNAWAY_START: cut_init_RUNAWAY_START(staffIdx); break;
        case KK1_CUT_RUNAWAY_END: cut_init_RUNAWAY_END(staffIdx); break;
        case KK1_CUT_BYE_CONTINUE: cut_init_BYE_CONTINUE(staffIdx); break;
        }
    }
    bool done = true;
    switch ((u32)(s32)(s8)state.mCutIndex) {
    case KK1_CUT_RUN_START: done = cut_move_RUN_START(); break;
    case KK1_CUT_RUN: done = cut_move_RUN() != 0; break;
    case KK1_CUT_CATCH_START: done = cut_move_CATCH_START(); break;
    case KK1_CUT_TRN: done = cut_move_TRN(); break;
    case KK1_CUT_BYE: done = cut_move_BYE(); break;
    case KK1_CUT_OTOBOKE: done = cut_move_OTOBOKE(); break;
    case KK1_CUT_RUNAWAY_START: done = cut_move_RUNAWAY_START(); break;
    case KK1_CUT_BYE_CONTINUE: done = cut_move_BYE_CONTINUE(); break;
    }
    if (done) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}

/* 0226E21C */
void daNpc_Kk1_c::event_proc(int staffIdx) {
    Kk1EventState& state = kk1Event(this);
    s16 evId = state.mEventIds[state.mEventIndex];
    if (!dComIfGp_evmng_endCheck(evId)) {
        if (!mEventCut.cutProc()) {
            privateCut(staffIdx);
        }
        return;
    }
    u32 ev = (u32)(s32)state.mEventIndex;
    if (ev < 3) {
        if (ev <= 1) {
            setStt(5);
        } else {
            eventInfo.mEventId = -1;
            switch (mCurrMsgNo) {
            case 0x1C98:
            case 0x1C9C:
                state.mEventOrder = 7;
                state.mLookMode = 1;
                state.mHeadOnly = 1;
                break;
            case 0x1C9A:
                state.mLookMode = 0;
                state.mHeadOnly = 1;
                state.mEventOrder = 10;
                break;
            case 0x1C9F:
                setStt(6);
                state.mLookMode = 0;
                state.mHeadOnly = 1;
                state.mEventOrder = 6;
                break;
            }
        }
    } else if (ev < 4) {
        state.mEventOrder = 1;
        dComIfGs_onEventBit(0xE08);
        state.mByeComplete = 1;
    } else if (ev == 4 || (ev >= 6 && ev <= 7)) {
        fopAcM_delete(this);
    }
    endEvent();
}

// HD: explicit cXyz result pointer; GHS allocates storage when it is null.
/* 0226D77C */
static BOOL kk1_chkPointPass(cXyz* i_pos, dNpc_PathRun_l* i_path) {
    f32 x = i_pos->x;
    f32 z = i_pos->z;
    f32 y = i_pos->y;
    if (i_path->mIdx == 0x11) {
        f32 r0 = (f32)kk1PathDebug().mOffsetX;
        f32 r1 = (f32)kk1PathDebug().mOffsetZ;
        x = x - (r0 + 20.0f);
        z = z + (r1 + 50.0f);
    }
    cXyz pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return kk1PathPassed(i_path, &pos, i_path->mbDir != 0);
}

/* 0226D83C */
bool daNpc_Kk1_c::event_move(bool i_chkArg) {
    Kk1EventState& state = kk1Event(this);
    path_layout::dPath* path = reinterpret_cast<path_layout::dPath*>(mPathRun.mPath.get());
    if (path == nullptr || !(path->m_closed & 1)) {
        return true;
    }
    if (state.mFollowPath != 0 && kk1_chkPointPass(&current.pos, &mPathRun)) {
        kk1PathNext(&mPathRun);
        if (i_chkArg) {
            s8 arg = kk1PathPointArg(&mPathRun, mPathRun.mIdx);
            if (arg >= 0) {
                arg = (s8)(arg + 1);
                if (arg == 2 || arg == 3) {
                    state.mMoveAnm = arg;
                    state.mPathArg = 0;
                    state.mFollowPath = 1;
                } else {
                    state.mFollowPath = 0;
                }
            } else {
                state.mFollowPath = 0;
            }
        }
    }
    cXyz pnt;
    kk1PathPoint(&pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, &pnt);
    s16 oldY = current.angle.y;
    cLib_addCalcAngleS(&current.angle.y, ang, kk1Parameters().mTurnRate, kk1Parameters().mTurnStep, 0x80);
    f32 factor, step, target;
    if ((s8)state.mMoveAnm == 2) {
        factor = kk1Parameters().mWalkRate;
        step = kk1Parameters().mWalkStep;
        target = state.mFollowPath == 0 ? 0.0f : kk1Parameters().mWalkSpeed;
    } else {
        factor = kk1Parameters().mRunRate;
        step = kk1Parameters().mRunStep;
        target = state.mFollowPath == 0 ? 0.0f : kk1Parameters().mRunSpeed;
    }
    f32 rate = speedF * factor;
    cLib_chaseF(&speedF, target, step);
    s32 itarget = gabi::ftoi(target);
    rate = (rate - 0.5f) >= 0.0f ? rate : 0.5f;
    mpMorf->setPlaySpeed(rate);
    if (itarget != 0) {
        return false;
    }
    s32 ispeed = gabi::ftoi(speedF);
    current.angle.y = oldY;
    if (ispeed != 0) {
        return false;
    }
    speedF = 0.0f;
    s8 arg = kk1PathPointArg(&mPathRun, mPathRun.mIdx);
    if (arg >= 0) {
        arg = (s8)(arg + 1);
    }
    state.mPathArg = arg;
    return true;
}

/* 0226E498 */
void daNpc_Kk1_c::kyoroPos(cXyz* o_pos, int i_idx) {
    cXyz* tbl = &lookOffsets[i_idx]; /* look-around offsets */
    cXyz offs;
    offs.z = tbl->z;
    offs.y = tbl->y;
    offs.x = tbl->x;
    mDoMtx_stack_c::transS(eyePos.x, eyePos.y, eyePos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    cXyz pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), &offs, &pos);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(0xC);
        if (o_pos == nullptr) {
            return;
        }
    }
    o_pos->x = pos.x;
    o_pos->y = pos.y;
    o_pos->z = pos.z;
}

/* 0226E560 */
bool daNpc_Kk1_c::kyorokyoro() {
    Kk1EventState& state = kk1Event(this);
    if (cLib_calcTimer(&state.mLookTimer) != 0) {
        cXyz pos;
        kyoroPos(&pos, state.mLookPoint);
        // HD: reverse-order integer word copy preserves signaling NaN bits.
        be<u32>* dst = reinterpret_cast<be<u32>*>(&state.mLookPos);
        be<u32>* src = reinterpret_cast<be<u32>*>(&pos);
        dst[2] = src[2];
        dst[1] = src[1];
        dst[0] = src[0];
        return true;
    }
    state.mLookPoint = (s16)cLib_getRndValue(1, 10);
    state.mLookTimer = kk1Parameters().mLookTime;
    state.mLookDelay = kk1Parameters().mLookDelay;
    return false;
}

/* 0226ECA8 */
BOOL daNpc_Kk1_c::_draw() {
    Kk1EventState& state = kk1Event(this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = model->getModelData();
    if (state.mDemoSuspend != 0 || state.mHide != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    kk1Animations(model_data).mPattern = nullptr; /* mBtpAnm.remove() */
    if (state.mHideEffect == 0) {
        /* the second model (state.mEffectModel): brk/btk animations */
        J3DModel* m2 = state.mEffectModel.get();
        kk1EntryColorAnimation(mBrkAnm, m2->getModelData(), (f32)state.mEffectAnmFrame.x); /* mDoExt_brkAnm::entry */
        f32 btkFrame = (f32)state.mEffectAnmFrame.y;
        m2 = state.mEffectModel.get();
        mBtkAnm.entry(m2->getModelData(), btkFrame);
        mDoExt_modelEntryDL(state.mEffectModel.get());
        kk1Animations(state.mEffectModel.get()->getModelData()).mTextureSrt = nullptr; /* btk remove */
        kk1Animations(state.mEffectModel.get()->getModelData()).mColor = nullptr; /* brk remove */
    }
    dSnap_RegistFig(0x58 /* DSNAP_TYPE_NPC_KM1_KK1 */, this, 1.0f, 1.0f, 1.0f);
    if (kk1Parameters().mDebugDraw) {
        /* debug leftovers: function-local static colours, and the path point */
        cXyz pos;
        copyPositionWords(&pos, &current.pos);
        pos.y = eyePos.y;
        initDebugColor(&debugGreenGuard, &debugGreen, &debugGreenInitial);
        initDebugColor(&debugYellowGuard, &debugYellow, &debugYellowInitial);
        initDebugColor(&debugRedGuard, &debugRed, &debugRedInitial);
        initDebugColor(&debugBlueGuard, &debugBlue, &debugBlueInitial);
        initDebugColor(&debugYellowGuard, &debugYellow, &debugYellowInitial);
        kk1PathPoint(&pos, &mPathRun);
        initDebugColor(&debugGreenGuard, &debugGreen, &debugGreenInitial);
    }
    return TRUE;
}

/* 0226E99C */
BOOL daNpc_Kk1_c::_execute() {
    Kk1EventState& state = kk1Event(this);
    if (state.mHomeInitialized == 0) {
        copyPositionWords(&state.mHomePos, &current.pos);
        state.mHomeAngle.x = current.angle.x;
        state.mHomeAngle.y = current.angle.y;
        state.mHomeAngle.z = current.angle.z;
        state.mHomeInitialized = 1;
    }
    m_jnt.setParam(kk1Parameters().mBodyMinX, kk1Parameters().mBodyMaxX, kk1Parameters().mBodyMinY, kk1Parameters().mBodyMaxY, kk1Parameters().mHeadMinX, kk1Parameters().mHeadMaxX,
                   kk1Parameters().mHeadMinY, kk1Parameters().mHeadMaxY, kk1Parameters().mMaxTurn);
    if (state.mDemoSuspend != 0 && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && !checkCommandTalk() && (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            pmf_call(this, &mAction, nullptr); // HD: the GHS member pointer remains in the shared ProcFunc_l representation.
        }
        state.mFrameFlags = 0;
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
    } else {
        state.mDemoSuspend = 0;
        eventOrder();
    }
    state.mLastAngle.x = current.angle.x;
    state.mLastAngle.y = current.angle.y;
    state.mLastAngle.z = current.angle.z;
    if (state.mPreserveShapeAngle == 0) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    void* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), gnd);
    setMtx(false);
    if (state.mDisableCollision == 0 && state.mHideBody == 0) {
        if ((s8)state.mAnmState == 1) {
            setCollision(60.0f, 140.0f);
        } else {
            setCollision(40.0f, 140.0f);
        }
    }
    return TRUE;
}

/* WWHD glue
data cutNames 101BF454
ext 0253F124 u32 kk1EventActorId(dEvt_control_c* control, fopAc_ac_c* actor)
ext 02543280 void kk1EventCutEnd(dEvent_manager_c* manager, s32 staff)
ext 025447C8 BOOL kk1EventAdvanced(dEvent_manager_c* manager, s32 staff)
data turnPointTable 1001C4D0
data lookOffsets 101BF48C
data debugGreenGuard 101FDA48
data debugGreen 101FEBEC
data debugGreenInitial 1001C228
data debugYellowGuard 101FDA44
data debugYellow 101FEBE8
data debugYellowInitial 1001C22C
data debugRedGuard 101FDA50
data debugRed 101FEBF4
data debugRedInitial 1001C230
data debugBlueGuard 101FDAC0
data debugBlue 101FEBF8
data debugBlueInitial 1001C234

str 1001C4A0 "d_a_npc_kk1.cpp" in 0226CC80
str 1001C4B0 "a_actor != NULL" in 0226CC80
str 1001C4C0 "Timer" in 0226CD04
str 1001C4E0 "Timer" in 0226D0E8
str 1001C4E8 "Delay" in 0226D0E8
str 1001C4F0 "prm_0" in 0226D0E8
str 1001C4FC "Timer" in 0226D454
str 1001C504 "Timer" in 0226D664
ext 0259ED58 bool kk1PathNext(dNpc_PathRun_l* path)
ext 0259EC94 bool kk1PathPrevious(dNpc_PathRun_l* path)
ext 0259EDB8 u8 kk1PathPointCount(dNpc_PathRun_l* path)
ext 0259EE4C s8 kk1PathPointArg(dNpc_PathRun_l* path, u8 index)
ext 0259EE7C bool kk1PathFindNearest(dNpc_PathRun_l* path, cXyz* pos, f32 radius)
ext 0259E838 BOOL kk1PathPassed(dNpc_PathRun_l* path, cXyz* pos, bool forward)
ext 0226B5B4 void kk1PathPoint(cXyz* result, dNpc_PathRun_l* path)
ext 0259E2D4 void kk1LookAtTarget(dNpc_JntCtrl_c* control, be<s16>* angle, cXyz* target, cXyz* eye, s16 yaw, s16 velocity, u8 headOnly)
ext 025E779C void kk1EntryColorAnimation(void* animation, J3DModelData* model, f32 frame)
*/
