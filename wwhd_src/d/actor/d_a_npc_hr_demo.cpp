/**
 * d_a_npc_hr_demo.cpp (WWHD)
 * NPC - Zephos & Cyclos: event (demo*), tornado and state functions, compiler-generated code
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_hr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_hr.h"

/* 022445D4 */
void daNpc_Hr_c::demoInitWind() {
    WWHD_FUNC(0x022445D4, void, this);
    envlight_setWindVec(0.0f, 0.0f, 1.0f); /* env_light.mWind.mWindVec.set(0, 0, 1) */
    dKyw_tact_wind_set(0, 0x4000);
    dKyw_tact_wind_set_go();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    cXyz* pWindVec = dKyw_get_wind_vec();
    gabi::Local<u8[8]> globe; /* cSGlobe globe(*pWindVec) */
    gabi::Local<be<s16>> angle;
    cSGlobe_ct(globe, pWindVec);
    cSAngle_ct_copy(angle, gabi::at<be<s16>>(globe.a + 6)); /* cSAngle angle(globe.U()) */
    cSAngle_addeq(angle, gabi::at<be<s16>>(0x101FF358));    /* angle += cSAngle::_90 */
    f32 _40 = 40.0f;
    mScaleFactor = 1.0f;
    speed.y = 0.0f;
    speed.x = 0.0f;
    speed.z = _40;
    f32 lx = pLink->current.pos.x;
    current.pos.x = lx;
    current.pos.y = pLink->current.pos.y;
    current.pos.z = pLink->current.pos.z;
    f32 cs = cSAngle_Cos(angle);
    f32 k = gabi::fmadds(mScaleFactor * _40 * _40, 0.5f, 150.0f);
    f32 y = current.pos.y;
    current.pos.x = gabi::fmadds(20.0f, cs, lx);
    current.pos.y = y + k;
    f32 sn = cSAngle_Sin(angle);
    f32 dz = gabi::fmsubs(20.0f, sn, _40 * speed.z);
    f32 sy = mScaleFactor * _40;
    shape_angle.y = 0;
    current.angle.y = 0;
    mMoveTimer = 0x8C;
    current.pos.z = current.pos.z + dz;
    speed.y = -sy;
    dKyw_custom_windpower(1.0f);
    daPy_setPlayerPosAndAngle(pLink, &pLink->current.pos, 0x7FFF);
    s32 sw = getSwbit();
    dComIfGs_offSwitch(sw, fopAcM_GetRoomNo(this));
    mDoAud_seStart_noPos(0x876 /* JA_SE_TAKT_WIND_DEMO */);
}
VERIFY(0x022445D4, &daNpc_Hr_c::demoInitWind);

/* 02244E58 */
bool daNpc_Hr_c::demoProcWind(int param_1) {
    WWHD_FUNC(0x02244E58, bool, this, param_1);
    setFlag(HR_FLAG_00000020);
    if (param_1 == 0 && mMoveTimer > 0 && mMoveTimer <= 0x6E) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    if (mMoveTimer > 0) {
        s16 t = (s16)(mMoveTimer - 1);
        mMoveTimer = t;
        if (t > 0x3C) {
            fopAcM_posMove(this, nullptr);
            speed.y = speed.y + mScaleFactor;
            t = mMoveTimer;
        }
        if (t < 0xF) {
            dKyw_custom_windpower(0.0f);
        }
    } else {
        speed.z = 0.0f;
        speed.x = 0.0f;
        speed.y = 0.0f;
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    f32 y = current.pos.y;
    f32 z = current.pos.z;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->y = y;
    attPos->z = z;
    f32 x = current.pos.x;
    eyePos.z = z;
    eyePos.x = x;
    eyePos.y = y;
    attPos->x = x;
    return false;
}
VERIFY(0x02244E58, &daNpc_Hr_c::demoProcWind);

/* 022447D0 */
void daNpc_Hr_c::demoInitWait() {
    WWHD_FUNC(0x022447D0, void, this);
    be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB14) /* "Timer" */);
    if (pTimer != nullptr) {
        mMoveTimer = (s16)*pTimer;
    } else {
        mMoveTimer = 0;
    }
}
VERIFY(0x022447D0, &daNpc_Hr_c::demoInitWait);

/* 022450E0 */
BOOL daNpc_Hr_c::demoProcWait() {
    WWHD_FUNC(0x022450E0, BOOL, this);
    if (mMoveTimer > 0) {
        mMoveTimer = (s16)(mMoveTimer - 1);
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return FALSE;
}
VERIFY(0x022450E0, &daNpc_Hr_c::demoProcWait);

/* 02244554 */
void daNpc_Hr_c::demoInitSpeak() {
    WWHD_FUNC(0x02244554, void, this);
    talkInit();
    be<s32>* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AAEC) /* "MsgNo" */);
    if (a_intP == nullptr) /* JUT_ASSERT(884, a_intP) */
        JUT_ASSERT_fail(STR(0x1001AAFC), 0x374, STR(0x1001AAF4));
    mMsgNo = *a_intP;
}
VERIFY(0x02244554, &daNpc_Hr_c::demoInitSpeak);

/* 02244D8C */
BOOL daNpc_Hr_c::demoProcSpeak() {
    WWHD_FUNC(0x02244D8C, BOOL, this);
    u16 temp = talk();
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp == 0xFE) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        if (chkFlag(HR_FLAG_00000002)) {
            clrFlag(HR_FLAG_00000002);
        }
    }
    return TRUE;
}
VERIFY(0x02244D8C, &daNpc_Hr_c::demoProcSpeak);

/* 02244DFC */
BOOL daNpc_Hr_c::demoProcPatten() {
    WWHD_FUNC(0x02244DFC, BOOL, this);
    u16 temp = talk();
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp == 0xFE || temp == 0x15 /* fopMsgStts_INPUT_e */) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
VERIFY(0x02244DFC, &daNpc_Hr_c::demoProcPatten);

/* 02245138 */
BOOL daNpc_Hr_c::demoProcTact0() {
    WWHD_FUNC(0x02245138, BOOL, this);
    if (chkFlag(HR_FLAG_00000002)) {
        /* HD: the message is the message manager (GameCube: l_msg) */
        u32 mgr = msgMgr();
        if (msgMgr_getStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
            msgMgr_setStatus(mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
            gabi::store<u8>(mgr + 0x921, 1); /* fopMsgM_messageSendOn() */
            clrFlag(HR_FLAG_00000200);
            if (dComIfGp_checkMesgCancelButton()) {
                setFlag(HR_FLAG_00000200);
            } else {
                execItemGet(0x6D /* dItemNo_WINDS_REQUIEM_e */);
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
            clrFlag(HR_FLAG_00000002);
        }
        return TRUE;
    }
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
VERIFY(0x02245138, &daNpc_Hr_c::demoProcTact0);

/* 02245238 */
BOOL daNpc_Hr_c::demoProcTact1() {
    WWHD_FUNC(0x02245238, BOOL, this);
    u16 temp = talk();
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp == 0x13 /* fopMsgStts_MSG_DESTROYED_e */ || temp == 0xFE) {
        if (chkFlag(HR_FLAG_00000200)) {
            endTalk();
            endTact();
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            dComIfGs_onEventBit(0x2708 /* UNK_2708 */);
        }
    }
    return TRUE;
}
VERIFY(0x02245238, &daNpc_Hr_c::demoProcTact1);

/* 022452CC */
BOOL daNpc_Hr_c::demoProcTact2() {
    WWHD_FUNC(0x022452CC, BOOL, this);
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
VERIFY(0x022452CC, &daNpc_Hr_c::demoProcTact2);

/* 02245308 */
BOOL daNpc_Hr_c::demoProcTact3() {
    WWHD_FUNC(0x02245308, BOOL, this);
    if (daPy_getTactTimerCancel(dComIfGp_getLinkPlayer()) > 0) {
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001AB90) /* "TACTM_RT" */, 0xFF);
        setEvFlag(EVFLAG_EVENT_CHANGE_REQ);
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        fopAcM_orderChangeEventId(pLink, this, mEventIdx, 0, 0xFFFF);
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
VERIFY(0x02245308, &daNpc_Hr_c::demoProcTact3);

/* 02244834 */
int daNpc_Hr_c::calcKaijou(int param_1) {
    WWHD_FUNC(0x02244834, int, this, param_1);
    int sum = 0;
    for (int i = 1; param_1 >= 1; i++, param_1--) {
        sum += i;
    }
    return sum;
}
VERIFY(0x02244834, &daNpc_Hr_c::calcKaijou);

/* 02244858 */
void daNpc_Hr_c::demoInitMove() {
    WWHD_FUNC(0x02244858, void, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    cXyz* pPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AB1C) /* "Pos" */);
    if (pPos != nullptr) {
        mTargetPos.x = pPos->x;
        mTargetPos.y = pPos->y;
        mTargetPos.z = pPos->z;
    } else {
        cXyz* pPos3 = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AB30) /* "Pos3" */);
        if (pPos3 != nullptr && pShip != nullptr) {
            fpoAcM_absolutePos(pShip, pPos3, &mTargetPos);
        } else {
            cXyz* pPos2 = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AB38) /* "Pos2" */);
            if (pPos2 != nullptr) {
                fpoAcM_absolutePos(pLink, pPos2, &mTargetPos);
            }
        }
    }
    be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB20) /* "Timer" */);
    if (pTimer != nullptr) {
        mMoveTimer = (s16)*pTimer;
        if (mMoveTimer < 10) {
            mMoveTimer = 10;
        }
    } else {
        mMoveTimer = 0x14;
    }
    be<s32>* pBrake = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB28) /* "Brake" */);
    if (pBrake != nullptr) {
        mBrakeTime = (s16)*pBrake;
    } else {
        mBrakeTime = 0;
    }
    be<s32>* pAngleY = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB40) /* "AngleY" */);
    if (pAngleY != nullptr) {
        s16 a = (s16)*pAngleY;
        mTargetAngle = a;
        if (chkEvFlag(EVFLAG_LOCK_ROTATION)) {
            current.angle.y = a;
        }
    } else {
        mTargetAngle = current.angle.y;
    }
    be<s32>* pSoundOff = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB48) /* "SOUND_OFF" */);
    if (pSoundOff == nullptr) {
        /* fopAcM_seStart(this, JA_SE_CM_HU_RC_MOVE_RAPID, 0) (HD: at current.pos) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x69FD, &current.pos, 0, reverb);
    }
    setFlag(HR_FLAG_00000020);
    s16 brake = mBrakeTime;
    s16 timer = mMoveTimer;
    f32 temp2;
    if (brake == 0) {
        temp2 = 1.0f / (f32)timer;
    } else {
        f32 a = (f32)(timer - brake);
        int k = calcKaijou(brake);
        temp2 = 1.0f / (a + (f32)k / (f32)brake);
    }
    speed.x = temp2 * (mTargetPos.x - current.pos.x);
    speed.y = temp2 * (mTargetPos.y - current.pos.y);
    speed.z = temp2 * (mTargetPos.z - current.pos.z);
}
VERIFY(0x02244858, &daNpc_Hr_c::demoInitMove);

/* 02244C58 */
void daNpc_Hr_c::demoInitSmall() {
    WWHD_FUNC(0x02244C58, void, this);
    mScaleFactor = 11.0f;
    mMoveTimer = 0;
}
VERIFY(0x02244C58, &daNpc_Hr_c::demoInitSmall);

/* 022453B8 */
BOOL daNpc_Hr_c::demoProcSmall() {
    WWHD_FUNC(0x022453B8, BOOL, this);
    f64 r = gabi::call<f64>(0x0200ECD4 /* cLib_addCalc */, &mScaleFactor, 1.0f, 0.1f, 100.0f, 0.3f);
    if (r < (f64)0.3f) {
        mScaleFactor = 1.0f;
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    f32 sf = mScaleFactor;
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    scale.x = sf;
    scale.y = sf;
    scale.z = sf;
    if (pEmitter != nullptr) {
        JPABaseEmitter_setGlobalScale(pEmitter, scale.x, scale.y, sf);
    }
    return TRUE;
}
VERIFY(0x022453B8, &daNpc_Hr_c::demoProcSmall);

/* 02244FA4 */
bool daNpc_Hr_c::demoProcMove() {
    WWHD_FUNC(0x02244FA4, bool, this);
    setFlag(HR_FLAG_00000020);
    s16 t = mMoveTimer;
    if (t <= 0) {
        speed.x = 0.0f;
        speed.y = 0.0f;
        speed.z = 0.0f;
        dComIfGp_evmng_cutEnd(mStaffIdx);
    } else if (t < mBrakeTime) {
        f32 fac = (f32)t / (f32)(t + 1);
        speed.x = speed.x * fac;
        speed.y = speed.y * fac;
        speed.z = speed.z * fac;
    }
    if (mMoveTimer > 0) {
        fopAcM_posMove(this, nullptr);
        if (!chkEvFlag(EVFLAG_LOCK_ROTATION)) {
            cLib_addCalcAngleS2(&current.angle.y, mTargetAngle, mMoveTimer, 0x3FFF);
        }
        mMoveTimer = (s16)(mMoveTimer - 1);
    }
    return true;
}
VERIFY(0x02244FA4, &daNpc_Hr_c::demoProcMove);

/* 02244C70 */
void daNpc_Hr_c::demoInitChange() {
    WWHD_FUNC(0x02244C70, void, this);
    be<s32>* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AB58) /* "prm0" */);
    if (a_intP == nullptr) { /* JUT_ASSERT(1209, a_intP) */
        JUT_ASSERT_fail(STR(0x1001AB74), 0x4B9, STR(0x1001AB60));
        return;
    }
    s32 prm = *a_intP;
    switch (prm) {
    case 1:
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001AB84) /* "TACT1_RT" */, 0xFF);
        break;
    default:
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001AB68) /* "PATTEN_RT" */, 0xFF);
        break;
    }
    setEvFlag(EVFLAG_EVENT_CHANGE_REQ);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    fopAcM_orderChangeEventId(pLink, this, mEventIdx, 0, 0xFFFF);
}
VERIFY(0x02244C70, &daNpc_Hr_c::demoInitChange);

/* 02244068 */
void daNpc_Hr_c::demoInitCom() {
    WWHD_FUNC(0x02244068, void, this);
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();

    be<s32>* turnPl = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA5C) /* "TURN_PL" */);
    if (turnPl != nullptr) {
        if (*turnPl != 0) {
            setEvFlag(EVFLAG_LOCK_ROTATION);
        } else {
            clrEvFlag(EVFLAG_LOCK_ROTATION);
        }
    }
    be<s32>* tornado = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA64) /* "TORNADO" */);
    if (tornado != nullptr) {
        if (*tornado != 0) {
            setEvFlag(EVFLAG_TORNADO_ACTIVE);
        } else {
            clrEvFlag(EVFLAG_TORNADO_ACTIVE);
        }
    }
    be<s32>* update = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA78) /* "UPDATE" */);
    if (update != nullptr) {
        if (*update != 0) {
            clrEvFlag(EVFLAG_SKIP_UPDATE);
        } else {
            setEvFlag(EVFLAG_SKIP_UPDATE);
        }
    }
    be<s32>* tornado_off = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA98) /* "TORNADE_OFF" */);
    if (tornado_off != nullptr) {
        dComIfGs_onEventBit(0x2710 /* UNK_2710 */);
        s32 sw = getSwbit();
        dComIfGs_onSwitch(sw, fopAcM_GetRoomNo(this));
        mReturnState = 1;
    }
    be<s32>* anm = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA74) /* "ANM" */);
    if (anm != nullptr) {
        setAnm((s8)*anm);
    }
    be<s32>* texAnm = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA80) /* "TEXANM" */);
    if (texAnm != nullptr) {
        setTexPtn((s8)*texAnm);
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AAC8) /* "Sound_Init" */) != nullptr) {
        mDoAud_tact_reset();
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AAD4) /* "Sound_Play" */) != nullptr) {
        mDoAud_tact_melodyPlay(1);
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AAA4) /* "Sound_End" */) != nullptr) {
        mDoAud_tact_reset();
        clrEvFlag(EVFLAG_TACT_SOUND_ACTIVE);
    }
    cXyz* startPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AAE0) /* "StartPos" */);
    if (startPos != nullptr) {
        current.pos.x = startPos->x;
        current.pos.y = startPos->y;
        current.pos.z = startPos->z;
    }
    cXyz* startPos2 = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AAB0) /* "StartPos2" */);
    if (startPos2 != nullptr) {
        fpoAcM_absolutePos(pLink, startPos2, &current.pos);
    }
    cXyz* startPos3 = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001AABC) /* "StartPos3" */);
    if (startPos3 != nullptr && pShip != nullptr) {
        fpoAcM_absolutePos(pShip, startPos3, &current.pos);
    }
    be<s32>* wind = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA88) /* "Wind" */);
    be<f32>* windPrm = dComIfGp_evmng_getMyFloatP(mStaffIdx, STR(0x1001AA6C) /* "WindPrm" */);
    if (wind != nullptr) {
        switch ((s32)*wind) {
        case 1:
            if (windPrm != nullptr) {
                s32 n = dComIfGp_evmng_getMySubstanceNum(mStaffIdx, STR(0x1001AA6C) /* "WindPrm" */);
                mClothes.create(this, (u8)*wind, windPrm, n);
            }
            break;
        default:
            mClothes.end();
            break;
        }
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001AA90) /* "AWAY" */) != nullptr) {
        mDoAud_seStart_noPos(0x5926 /* JA_SE_CM_HU_RC_FLY_AWAY */);
        if (getShapeType() == 1) {
            mDoAud_seStart_noPos(0x48ED /* JA_SE_CV_RC_FLY_AWAY */);
        } else {
            mDoAud_seStart_noPos(0x48E9 /* JA_SE_CV_HU_FLY_AWAY */);
        }
    }
}
VERIFY(0x02244068, &daNpc_Hr_c::demoInitCom);

/* 022454A4 */
void daNpc_Hr_c::demoProcCom() {
    WWHD_FUNC(0x022454A4, void, this);
    if (chkEvFlag(EVFLAG_LOCK_ROTATION)) {
        setFlag(HR_FLAG_00000080);
    }
    if (chkEvFlag(EVFLAG_SKIP_UPDATE)) {
        setFlag(HR_FLAG_00000100);
    }
    if (chkEvFlag(EVFLAG_TACT_SOUND_ACTIVE)) {
        mDoAud_tact_ambientPlay();
    }
    if (!chkFlag(HR_FLAG_00000010)) {
        /* speed.abs(): the results are passed on in f1 unrounded */
        f64 sq = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, &speed);
        f64 temp = gabi::call<f64>(0x028F4384 /* sqrtf */, sq);
        if (temp > 15.0) {
            temp = 15.0;
        }
        f32 ratio = (f32)(temp / 15.0);
        f32 v = 100.0f * ratio;
        u32 param = !(v < 2147483648.0f) ? (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u : (u32)gabi::ftoi(v);
        /* fopAcM_seStart(this, JA_SE_CM_HU_RC_FLYING, (temp / 15) * 100) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x5125, &eyePos, param, reverb);
    }
}
VERIFY(0x022454A4, &daNpc_Hr_c::demoProcCom);

/* 022455CC */
u32 daNpc_Hr_c::demoProc() {
    WWHD_FUNC(0x022455CC, u32, this);
    u32 temp; /* bool: the results are passed on unnormalised */
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    temp = true;
    if (chkEvFlag(EVFLAG_EVENT_CHANGE_REQ)) {
        mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001ABB0) /* "Hr2" */, nullptr, 0);
        clrEvFlag(EVFLAG_EVENT_CHANGE_REQ);
    }
    s32 eventAction = getNowEventAction();
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        setFlag(HR_FLAG_00000002);
        switch ((u32)eventAction) {
        case 1:
        case 2:
            demoInitSpeak();
            break;
        case 7:
            demoInitWind();
            break;
        case 9: {
            daPy_onPlayerNoDraw(pLink);
            s32 sw = getSwbit();
            dComIfGs_offSwitch(sw, fopAcM_GetRoomNo(this));
            break;
        }
        case 10:
            daPy_offPlayerNoDraw(pLink);
            break;
        case 11: {
            cXyz* pPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001ABB4) /* "Pos" */);
            if (pPos != nullptr) {
                current.pos.x = pPos->x;
                current.pos.y = pPos->y;
                current.pos.z = pPos->z;
            }
            be<s32>* pAngleY = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001ABC0) /* "AngleY" */);
            if (pAngleY != nullptr) {
                current.angle.y = (s16)*pAngleY;
            }
            offHide(0);
            demoInitWait();
            mAnmIdx = -1;
            setAnm(0);
            break;
        }
        case 12:
            demoInitMove();
            break;
        case 13: {
            mTargetAngle = fopAcM_searchActorAngleY(pLink, this);
            be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001ABB8) /* "Timer" */);
            if (pTimer != nullptr) {
                mMoveTimer = (s16)*pTimer;
            } else {
                mMoveTimer = 0;
            }
            break;
        }
        case 14:
            setAnm(0);
            demoInitWait();
            break;
        case 0:
            demoInitWait();
            break;
        case 15:
            demoInitSmall();
            break;
        case 16:
            speed.y = 0.0f;
            break;
        case 17:
            demoInitChange();
            break;
        case 5:
            clrFlag(HR_FLAG_00000200);
            break;
        }
    }

    switch ((u32)eventAction) {
    case 1:
        demoProcSpeak();
        break;
    case 2:
        demoProcPatten();
        break;
    case 7:
        temp = gabi::call<u32>(0x02244E58 /* demoProcWind */, this, 0);
        break;
    case 8:
        temp = gabi::call<u32>(0x02244E58 /* demoProcWind */, this, 1);
        break;
    case 12:
        temp = gabi::call<u32>(0x02244FA4 /* demoProcMove */, this);
        break;
    case 11:
        demoProcWait();
        break;
    case 13:
        if (mMoveTimer > 0) {
            mMoveTimer = (s16)(mMoveTimer - 1);
        } else {
            gabi::Local<be<s16>> angle;
            *angle = pLink->shape_angle.y;
            cLib_addCalcAngleS(angle, mTargetAngle, 1, 0x800, 0x800);
            daPy_setPlayerPosAndAngle(pLink, nullptr, *angle);
            if (*angle == mTargetAngle) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
            }
        }
        break;
    case 14:
        demoProcWait();
        setFlag(HR_FLAG_00000040);
        break;
    case 0:
        demoProcWait();
        break;
    case 3:
        demoProcTact0();
        break;
    case 4:
        demoProcTact1();
        break;
    case 5:
        demoProcTact2();
        break;
    case 6:
        demoProcTact3();
        break;
    case 15:
        demoProcSmall();
        break;
    case 16: {
        f64 r = gabi::call<f64>(0x0200ECD4 /* cLib_addCalc */, &current.pos.y, home.pos.y + 6000.0f, 0.5f, 100.0f, 5.0f);
        if (r < 5.0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    }
    case 17:
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    demoProcCom();
    return temp;
}
VERIFY(0x022455CC, &daNpc_Hr_c::demoProc);

/* 02245BB8 */
u32 daNpc_Hr_c::ht_tact01() {
    WWHD_FUNC(0x02245BB8, u32, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        if (dComIfGs_isEventBit(0x2708 /* UNK_2708 */)) {
            envlight_setWindVec(1.0f, 0.0f, 0.0f);
            dKyw_tact_wind_set(0, 0);
            dKyw_tact_wind_set_go();
        }
        endTalk();
        endTact();
        return false;
    }
    return demoProc();
}
VERIFY(0x02245BB8, &daNpc_Hr_c::ht_tact01);

/* daTornado_c::getJointXPos/YPos/ZPos (HD inline): the joint's animation matrix translation, or the
 * tornado's position without a model (mpModel at +0x3B4) */
static inline f32 tornado_getJointPos(fopAc_ac_c* t, int i, u32 comp, u32 posOff) {
    J3DModel* m = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(t) + 0x3B4));
    if (m != nullptr) {
        return gabi::load<f32>(gabi::ea(getAnmMtx(m, i)) + comp);
    }
    return gabi::load<f32>(gabi::ea(t) + posOff);
}
static inline f32 tornado_getJointXPos(fopAc_ac_c* t, int i) { return tornado_getJointPos(t, i, 0x0C, 0x314); }
static inline f32 tornado_getJointYPos(fopAc_ac_c* t, int i) { return tornado_getJointPos(t, i, 0x1C, 0x318); }
static inline f32 tornado_getJointZPos(fopAc_ac_c* t, int i) { return tornado_getJointPos(t, i, 0x2C, 0x31C); }

/* 02245C84 */
int daNpc_Hr_c::getNowJointY() {
    WWHD_FUNC(0x02245C84, int, this);
    fopAc_ac_c* tornadoActor = fopAcM_SearchByID(mProcId);
    /* HD: asserts the tornado (and returns 10 without it) */
    if (tornadoActor == nullptr) {
        JUT_ASSERT_fail(STR(0x1001ABC8), 0x8EA, STR(0x1001ABD8));
        return 10;
    }
    int i = 0;
    while (i < 10) {
        J3DModel* m = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(tornadoActor) + 0x3B4));
        if (m != nullptr) {
            f32 jointY = gabi::load<f32>(gabi::ea(getAnmMtx(m, i)) + 0x1C);
            if (current.pos.y + speed.y < jointY) {
                return i;
            }
        } else {
            f32 a = current.pos.y + speed.y;
            if (a < tornadoActor->current.pos.y) {
                return i;
            }
        }
        i++;
    }
    return i;
}
VERIFY(0x02245C84, &daNpc_Hr_c::getNowJointY);

/* 02245D80 */
void daNpc_Hr_c::getTornadoPos(int jointY, cXyz* tornadoPos) {
    WWHD_FUNC(0x02245D80, void, this, jointY, tornadoPos);
    fopAc_ac_c* tornadoActor = fopAcM_SearchByID(mProcId);
    if (tornadoActor != nullptr) {
        if (jointY == 0) {
            tornadoPos->copy(tornadoActor->current.pos);
            return;
        }
        f32 temp = current.pos.y + speed.y;
        tornadoPos->y = temp;
        if (jointY == 10) {
            tornadoPos->x = tornado_getJointXPos(tornadoActor, 10);
            tornadoPos->z = tornado_getJointZPos(tornadoActor, 10);
        } else {
            f32 x1 = tornado_getJointXPos(tornadoActor, jointY - 1);
            f32 y1 = tornado_getJointYPos(tornadoActor, jointY - 1);
            f32 z1 = tornado_getJointZPos(tornadoActor, jointY - 1);
            f32 x2 = tornado_getJointXPos(tornadoActor, jointY);
            f32 y2 = tornado_getJointYPos(tornadoActor, jointY);
            f32 z2 = tornado_getJointZPos(tornadoActor, jointY);
            f32 ratio = (temp - y1) / (y2 - y1);
            f32 om = 1.0f - ratio;
            tornadoPos->x = gabi::fmadds(ratio, x2, om * x1);
            tornadoPos->z = gabi::fmadds(ratio, z2, om * z1);
        }
    }
}
VERIFY(0x02245D80, &daNpc_Hr_c::getTornadoPos);

/* 02246088 */
bool daNpc_Hr_c::rideTornado() {
    WWHD_FUNC(0x02246088, bool, this);
    fopAc_ac_c* actor = fopAcM_SearchByID(mProcId);
    if (actor == nullptr) {
        speed.x = speed.x * 0.95f;
        speed.y = 0.0f;
        speed.z = speed.z * 0.95f;
        return false;
    }
    gabi::Local<cXyz> tornadoPos;
    getTornadoPos(getNowJointY(), tornadoPos);
    speed.x = gabi::fmadds(tornadoPos->x - current.pos.x, 0.8f, speed.x * 0.2f);
    speed.z = gabi::fmadds(tornadoPos->z - current.pos.z, 0.8f, speed.z * 0.2f);
    return true;
}
VERIFY(0x02246088, &daNpc_Hr_c::rideTornado);

/* 02246180 */
bool daNpc_Hr_c::rt_win() {
    WWHD_FUNC(0x02246180, bool, this);
    speed.y = 0.0f;
    rideTornado();
    return true;
}
VERIFY(0x02246180, &daNpc_Hr_c::rt_win);

/* 022461B0 */
bool daNpc_Hr_c::rt_angry() {
    WWHD_FUNC(0x022461B0, bool, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    s16 ax = (s16)(current.angle.x + 0x100);
    current.angle.x = ax;
    scale.y = 11.0f;
    speed.y = gabi::fmadds(1000.0f, 1.0f - cM_scos(ax), home.pos.y + 6000.0f) - current.pos.y;
    scale.x = 11.0f;
    scale.z = 11.0f;
    current.angle.y = fopAcM_searchActorAngleY(this, dComIfGp_getLinkPlayer());
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    if (pEmitter != nullptr) {
        JPABaseEmitter_setGlobalScale(pEmitter, scale.x, scale.y, scale.z);
    }
    rideTornado();
    void* hitObj = nullptr;
    cXyz* hitPos = nullptr;
    if (mCyl2.ChkTgHit()) {
        hitObj = mCyl2.GetTgHitObj();
        hitPos = mCyl2.GetTgHitPosP();
    } else if (mCyl.ChkTgHit()) {
        hitObj = mCyl.GetTgHitObj();
        hitPos = mCyl.GetTgHitPosP();
    }
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    if (daShip_checkTornadoUp(pShip)) {
        mState = HR_STATE_RT_WIN;
        actor_status = actor_status | 0x800; /* fopAcM_OnStatus(this, fopAcStts_UNK800_e) */
        setAnm(12);
        mDoAud_seStart_noPos(0x48EE /* JA_SE_CV_RC_ENTER */);
        return true;
    }
    if (mHitDelayTimer != 0) {
        return true;
    }
    if (chkFlag(HR_FLAG_00000400)) {
        clrFlag(HR_FLAG_00000400);
        setAnm(2);
        setTexPtn(2);
    }
    /* ChkAtType(NORMAL_ARROW | FIRE_ARROW | ICE_ARROW | LIGHT_ARROW) */
    if (hitObj != nullptr && cCcD_Obj_ChkAtType(hitObj, 0x1C4000)) {
        gabi::Local<cXyz> scl;
        scl->x = 11.0f;
        scl->y = 11.0f;
        scl->z = 11.0f;
        gabi::Local<cXyz> diff;
        cXyz_mi(hitPos, diff, &pLink->eyePos);
        gabi::Local<cXyz> temp;
        temp->copy(*diff);
        if (!cXyz_isZero_l(temp)) {
            gabi::Local<cXyz> nres;
            cXyz_normalize_l(temp, nres);
            gabi::Local<csXyz> temp2;
            cM3d_CalcVecZAngle(temp, temp2);
            dComIfGp_particle_set(0xD /* dPa_name::ID_AK_JN_OK */, hitPos, temp2, scl);
        }
        mHitCount = (u8)(mHitCount + 1);
        if (mHitCount >= 3) {
            to_rt_tact();
        } else {
            to_rt_hit();
        }
    }
    return true;
}
VERIFY(0x022461B0, &daNpc_Hr_c::rt_angry);

/* 02246484 */
bool daNpc_Hr_c::rt_hit0() {
    WWHD_FUNC(0x02246484, bool, this);
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mState = HR_STATE_RT_HIT_1;
        mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001ABF4) /* "Hr2" */, nullptr, 0);
        setEvFlag(EVFLAG_TORNADO_ACTIVE);
        demoProc();
    } else if (daShip_checkTornadoUp(pShip)) {
        mState = HR_STATE_RT_WIN;
        actor_status = actor_status | 0x800; /* fopAcM_OnStatus(this, fopAcStts_UNK800_e) */
        setAnm(12);
        mDoAud_seStart_noPos(0x48EE /* JA_SE_CV_RC_ENTER */);
    } else {
        fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    }
    speed.y = 0.0f;
    rideTornado();
    return true;
}
VERIFY(0x02246484, &daNpc_Hr_c::rt_hit0);

/* 02246598 */
bool daNpc_Hr_c::rt_hit1() {
    WWHD_FUNC(0x02246598, bool, this);
    if (chkEvFlag(EVFLAG_TORNADO_ACTIVE)) {
        rideTornado();
    }
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        if (mReturnState == 1) {
            onHide(0);
        }
        if (mState == HR_STATE_RT_INTRO) {
            mDoAud_subBgmStart(0x8000002B /* JA_BGM_DIOCTA_BATTLE */);
        }
        endTalk();
        return false;
    }
    demoProc();
    return true;
}
VERIFY(0x02246598, &daNpc_Hr_c::rt_hit1);

/* 022467C8 (tail call) */
bool daNpc_Hr_c::rt_intro() {
    WWHD_FUNC(0x022467C8, bool, this);
    return rt_hit1();
}
VERIFY(0x022467C8, &daNpc_Hr_c::rt_intro);

/* 02246660 */
bool daNpc_Hr_c::rt_hide() {
    WWHD_FUNC(0x02246660, bool, this);
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    current.angle.y = fopAcM_searchActorAngleY(this, pLink);
    gabi::Local<cXyz> tornadoPos;
    getTornadoPos(0, tornadoPos);
    current.pos.x = tornadoPos->x;
    current.pos.y = tornadoPos->y - 1500.0f;
    current.pos.z = tornadoPos->z;
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mReturnState = HR_STATE_RT_ANGRY;
        mState = HR_STATE_RT_INTRO;
        mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001ABFC) /* "Hr2" */, nullptr, 0);
        setEvFlag(EVFLAG_TORNADO_ACTIVE);
        demoProc();
        mDoAud_seStart_noPos(0x48EE /* JA_SE_CV_RC_ENTER */);
        mDoAud_bgmAllMute(90);
        dComIfGs_onEventBit(0x3D01 /* UNK_3D01 */);
    } else if (pShip != nullptr && daShip_checkTornadoFlg(pShip)) {
        if (mMoveTimer > 0) {
            mMoveTimer = (s16)(mMoveTimer - 1);
        } else {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
            dComIfGs_onTmpBit(0x0404 /* UNK_0404 */);
        }
    }
    return true;
}
VERIFY(0x02246660, &daNpc_Hr_c::rt_hide);

/* 022467CC */
bool daNpc_Hr_c::rt_search() {
    WWHD_FUNC(0x022467CC, bool, this);
    fopAc_ac_c* ac = fopAcM_searchFromName(STR(0x1001AC00) /* "Ytrnd00" */, 0, 0);
    if (dComIfGs_isEventBit(0x2710 /* UNK_2710 */)) {
        return true;
    }
    if (ac != nullptr) {
        mProcId = fopAcM_GetID(ac);
        speed.x = 0.0f;
        speed.z = 0.0f;
        scale.z = 11.0f;
        scale.x = 11.0f;
        mHitCount = 0;
        current.pos.y = home.pos.y - 1500.0f;
        scale.y = 11.0f;
        mState = HR_STATE_RT_HIDE;
        speed.y = 0.0f;
        JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
        if (pEmitter != nullptr) {
            JPABaseEmitter_setGlobalScale(pEmitter, scale.x, scale.y, scale.z);
        }
        if (dComIfGs_isEventBit(0x3D01 /* UNK_3D01 */)) {
            mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001AC08) /* "INTRO_RT" */, 0xFF);
        } else {
            mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001AC14) /* "INTRO_RT_F" */, 0xFF);
        }
        mMoveTimer = 0x14;
    }
    return false;
}
VERIFY(0x022467CC, &daNpc_Hr_c::rt_search);

/* 02246950 */
bool daNpc_Hr_c::ht_hide() {
    WWHD_FUNC(0x02246950, bool, this);
    if (chkFlag(HR_FLAG_00000008)) {
        /* fopAcM_seStart(this, JA_SE_PRE_TAKT, 0) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x8A7, &eyePos, 0, reverb);
        mReturnState = mState;
        mState = HR_STATE_HT_TACT;
        setAnmStatus();
        mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001AC20) /* "Hr" */, nullptr, 0);
        demoProc();
        s32 sw = getSwbit();
        dComIfGs_onSwitch(sw, fopAcM_GetRoomNo(this));
        dComIfGp_event_onHindFlagAll(); /* dComIfGp_event_onHindFlag(-1) */
    } else if (chkFlag(HR_FLAG_00000001)) {
        mReturnState = mState;
        mState = HR_STATE_TALK;
        setAnmStatus();
        daPy_onPlayerNoDraw(dComIfGp_getLinkPlayer());
    } else {
        if (fopAcM_seenActorAngleY(dComIfGp_getLinkPlayer(), this) < 0x2AAA) {
            SetOrder(1);
            if (!dComIfGs_isEventBit(0x2708 /* UNK_2708 */)) {
                SetOrder(4);
            }
        }
    }
    return true;
}
VERIFY(0x02246950, &daNpc_Hr_c::ht_hide);

/* HD: JPABaseEmitter::setGlobalPrmColor / setGlobalEnvColor store gamma-corrected colours */
static inline u8 hr_gammaByte(u8 c) { return (u8)gabi::ftoi(hr_colorGamma(c) * 255.0f); }

/* 02246AB0 */
void daNpc_Hr_c::smokeProc() {
    WWHD_FUNC(0x02246AB0, void, this);
    u16 fl = mFlags;
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    if (!(fl & HR_FLAG_00000010) && pEmitter != nullptr && mType == 1 && mHitDelayTimer != 0) {
        u8 t = (u8)(mHitDelayTimer - 1);
        mHitDelayTimer = t;
        if (t >= 0x50) {
            u32 e = gabi::ea(pEmitter);
            u8 r = hr_gammaByte(0x7F);
            u8 g = hr_gammaByte(0x5E);
            u8 b = hr_gammaByte(0x91);
            gabi::store<u8>(e + 0x244, r); /* setGlobalPrmColor(0x7F, 0x5E, 0x91) */
            gabi::store<u8>(e + 0x245, g);
            gabi::store<u8>(e + 0x246, b);
            r = hr_gammaByte(0x6C);
            g = hr_gammaByte(0x58);
            b = hr_gammaByte(0x7B);
            gabi::store<u8>(e + 0x248, r); /* setGlobalEnvColor(0x6C, 0x58, 0x7B) */
            gabi::store<u8>(e + 0x249, g);
            gabi::store<u8>(e + 0x24A, b);
        } else {
            if (t > 0xF) {
                int temp = t - 0x10;
                setEmitFlash((f32)(temp % 8) / 7.0f);
            } else {
                setEmitFlash((f32)t / 15.0f);
            }
        }
        if (mHitDelayTimer < 0x50) {
            u8 t2 = mHitDelayTimer;
            if (t2 > 0xF) {
                f32 ratio = (f32)(s32)(t2 - 0xF) / 65.0f;
                dKy_actor_addcol_set(0x80, 0x80, 0x5A, ratio);
                dKy_vrbox_addcol_set(0x80, 0x80, 0x5A, ratio);
            } else {
                dKy_actor_addcol_set(0x80, 0x80, 0x5A, 0.0f);
                dKy_vrbox_addcol_set(0x80, 0x80, 0x5A, 0.0f);
            }
        }
    }
}
VERIFY(0x02246AB0, &daNpc_Hr_c::smokeProc);

/* 02246D58 */
BOOL daNpc_Hr_c::wait_action(void*) {
    WWHD_FUNC(0x02246D58, BOOL, this, (void*)nullptr);
    if (mActionStatus == 0 /* ACTION_STARTING */) {
        switch ((u32)(s32)mType) {
        case 0:
            mState = HR_STATE_HT_HIDE;
            gabi::store<u32>(gabi::ea(this) + 0x104, 0x02240E6C); /* eventInfo.setXyCheckCB(&daNpc_hr_XyCheckCB) */
            gabi::store<u32>(gabi::ea(this) + 0x100, 0x02240E70); /* eventInfo.setXyEventCB(&daNpc_hr_XyEventCB) */
            break;
        case 1:
            if (dComIfGs_isEventBit(0x2710 /* UNK_2710 */)) {
                mState = HR_STATE_WAIT_02;
            } else {
                mState = HR_STATE_RT_SEARCH;
            }
            break;
        default:
            mState = HR_STATE_WAIT_01;
            break;
        }
        setAnmStatus();
        mActionStatus = mActionStatus + 1; /* ACTION_ONGOING */
    } else if (mActionStatus != -1 /* ACTION_ENDING */) {
        clrFlag(HR_FLAG_00000100 | HR_FLAG_00000080 | HR_FLAG_00000040);
        u32 temp; /* bool: the state function's r3 is passed on unnormalised */
        switch ((u32)(s32)mState) {
        case HR_STATE_WAIT_01:
            temp = gabi::call<u32>(0x02243E34 /* wait01 */, this);
            break;
        case HR_STATE_WAIT_02:
            temp = true; /* wait02() (inline) */
            break;
        case HR_STATE_TALK:
            temp = gabi::call<u32>(0x02243F20 /* talk01 */, this);
            break;
        case HR_STATE_HT_TACT:
            temp = ht_tact01();
            break;
        case HR_STATE_RT_SEARCH:
            temp = gabi::call<u32>(0x022467CC /* rt_search */, this);
            break;
        case HR_STATE_RT_HIDE:
            temp = gabi::call<u32>(0x02246660 /* rt_hide */, this);
            break;
        case HR_STATE_RT_INTRO:
            temp = gabi::call<u32>(0x022467C8 /* rt_intro */, this);
            break;
        case HR_STATE_RT_ANGRY:
            temp = gabi::call<u32>(0x022461B0 /* rt_angry */, this);
            break;
        case HR_STATE_RT_WIN:
            temp = gabi::call<u32>(0x02246180 /* rt_win */, this);
            break;
        case HR_STATE_RT_HIT_0:
            temp = gabi::call<u32>(0x02246484 /* rt_hit0 */, this);
            break;
        case HR_STATE_RT_HIT_1:
            temp = gabi::call<u32>(0x02246598 /* rt_hit1 */, this);
            break;
        case HR_STATE_HT_HIDE:
            temp = gabi::call<u32>(0x02246950 /* ht_hide */, this);
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        setAttention(temp);
        smokeProc();
    }
    return TRUE;
}
VERIFY(0x02246D58, &daNpc_Hr_c::wait_action);

/* 022470AC: static initialisation of the translation unit (only the per-TU header statics) */
static void __sinit_d_a_npc_hr_cpp() {
    WWHD_FUNC(0x022470AC, void);
    sinit_header_statics(0x104671D0, 0x101BE958);
}
VERIFY(0x022470AC, __sinit_d_a_npc_hr_cpp);

/* 02247140: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001A76C slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x02247140, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x02247140, SafeString_dt);

/* 02247154: daNpc_Wind_Eff constructor (compiler-generated; allocates when this == NULL) */
static daNpc_Wind_Eff* daNpc_Wind_Eff_ct(daNpc_Wind_Eff* i_this) {
    WWHD_FUNC(0x02247154, daNpc_Wind_Eff*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Wind_Eff*)operator_new(0x38);
        if (i_this == nullptr)
            return i_this;
    }
    dPa_followEcallBack_ct(&i_this->mpFollowECallBack, 0, 0);
    return i_this;
}
VERIFY(0x02247154, daNpc_Wind_Eff_ct);

/* 022471A4: daNpc_Wind_Eff deleting destructor (compiler-generated) */
static void daNpc_Wind_Eff_dt(daNpc_Wind_Eff* i_this, s32 flags) {
    WWHD_FUNC(0x022471A4, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022471A4, daNpc_Wind_Eff_dt);

/* 022471B8: daNpc_Hr_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Hr_c_dt(daNpc_Hr_c* i_this, s32 flags) {
    WWHD_FUNC(0x022471B8, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x028F0164, &i_this->mClothes, 4, 0x38, 0x022471A4, 0, 0); /* __destroy_arr(mWindEff) */
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022471B8, daNpc_Hr_c_dt);

/* 0224725C: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0224725C, void, (SafeString*)nullptr);
}
VERIFY(0x0224725C, SafeString_assureTerminationImpl);
