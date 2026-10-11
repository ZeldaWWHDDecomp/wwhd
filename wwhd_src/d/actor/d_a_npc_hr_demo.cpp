/**
 * d_a_npc_hr_demo.cpp (WWHD)
 * NPC - Zephos & Cyclos: event (demo*), tornado and state functions, compiler-generated code
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_hr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_hr.h"

static u32 l_staticListHead;
static u32 l_staticConstructors;

static inline f32 tornado_getJointPos(fopAc_ac_c* actor, int joint, int component);
static inline f32 tornado_getJointXPos(fopAc_ac_c* actor, int joint);
static inline f32 tornado_getJointYPos(fopAc_ac_c* actor, int joint);
static inline f32 tornado_getJointZPos(fopAc_ac_c* actor, int joint);
static inline u8 hr_gammaByte(u8 c);
static void __sinit_d_a_npc_hr_cpp();
static void SafeString_dt(SafeString* i_this, s32 flags);
void daNpc_Wind_Eff_dt(daNpc_Wind_Eff* i_this, s32 flags);
static void daNpc_Hr_c_dt(daNpc_Hr_c* i_this, s32 flags);
static void SafeString_assureTerminationImpl(SafeString*);

/* 022445D4 */
void daNpc_Hr_c::demoInitWind() {
    envlight_setWindVec(0.0f, 0.0f, 1.0f); /* env_light.mWind.mWindVec.set(0, 0, 1) */
    dKyw_tact_wind_set(0, 0x4000);
    dKyw_tact_wind_set_go();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    cXyz* pWindVec = dKyw_get_wind_vec();
    cSGlobe globe; /* cSGlobe globe(*pWindVec) */
    cSAngle angle;
    cSGlobe_ct(&globe, pWindVec);
    cSAngle_ct_copy(&angle.mAngle, &globe.mInclination.mAngle); /* cSAngle &angle(globe.U()) */
    cSAngle_addeq(&angle.mAngle, &l_hrRightAngle.mAngle);       /* &angle += cSAngle::_90 */
    f32 _40 = 40.0f;
    mScaleFactor = 1.0f;
    speed.y = 0.0f;
    speed.x = 0.0f;
    speed.z = _40;
    f32 lx = pLink->current.pos.x;
    current.pos.x = lx;
    current.pos.y = pLink->current.pos.y;
    current.pos.z = pLink->current.pos.z;
    f32 cs = cSAngle_Cos(&angle.mAngle);
    f32 k = gabi::fmadds(mScaleFactor * _40 * _40, 0.5f, 150.0f);
    f32 y = current.pos.y;
    current.pos.x = gabi::fmadds(20.0f, cs, lx);
    current.pos.y = y + k;
    f32 sn = cSAngle_Sin(&angle.mAngle);
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
    mDoAud_seStart_noPos(JA_SE_TAKT_WIND_DEMO);
}

/* 02244E58 */
bool daNpc_Hr_c::demoProcWind(int param_1) {
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
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->y = y;
    attPos->z = z;
    f32 x = current.pos.x;
    eyePos.z = z;
    eyePos.x = x;
    eyePos.y = y;
    attPos->x = x;
    return false;
}

/* 022447D0 */
void daNpc_Hr_c::demoInitWait() {
    be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Timer");
    if (pTimer != nullptr) {
        mMoveTimer = (s16)*pTimer;
    } else {
        mMoveTimer = 0;
    }
}

/* 022450E0 */
BOOL daNpc_Hr_c::demoProcWait() {
    if (mMoveTimer > 0) {
        mMoveTimer = (s16)(mMoveTimer - 1);
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return FALSE;
}

/* 02244554 */
void daNpc_Hr_c::demoInitSpeak() {
    talkInit();
    be<s32>* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "MsgNo");
    if (a_intP == nullptr) /* JUT_ASSERT(884, a_intP) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x374, "a_intP");
    mMsgNo = *a_intP;
}

/* 02244D8C */
BOOL daNpc_Hr_c::demoProcSpeak() {
    u16 temp = talk();
    if (temp == fopMsgStts_BOX_CLOSED_e || temp == HR_MSG_NO_MESSAGE) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        if (chkFlag(HR_FLAG_00000002)) {
            clrFlag(HR_FLAG_00000002);
        }
    }
    return TRUE;
}

/* 02244DFC */
BOOL daNpc_Hr_c::demoProcPatten() {
    u16 temp = talk();
    if (temp == fopMsgStts_BOX_CLOSED_e || temp == HR_MSG_NO_MESSAGE ||
        temp == fopMsgStts_INPUT_e) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}

/* 02245138 */
BOOL daNpc_Hr_c::demoProcTact0() {
    if (chkFlag(HR_FLAG_00000002)) {
        // HD: the message is the message manager (GameCube: l_msg)
        u32 mgr = msgMgr();
        if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            msgMgr_setStatus(mgr, fopMsgStts_MSG_ENDS_e);
            gabi::at<HrMessageManagerHD>(mgr)->mSendOn = 1; /* fopMsgM_messageSendOn() */
            clrFlag(HR_FLAG_00000200);
            if (dComIfGp_checkMesgCancelButton()) {
                setFlag(HR_FLAG_00000200);
            } else {
                execItemGet(dItemNo_WINDS_REQUIEM_e);
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
            clrFlag(HR_FLAG_00000002);
        }
        return TRUE;
    }
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}

/* 02245238 */
BOOL daNpc_Hr_c::demoProcTact1() {
    u16 temp = talk();
    if (temp == fopMsgStts_BOX_CLOSED_e || temp == fopMsgStts_MSG_DESTROYED_e ||
        temp == HR_MSG_NO_MESSAGE) {
        if (chkFlag(HR_FLAG_00000200)) {
            endTalk();
            endTact();
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            dComIfGs_onEventBit(dSv_event_flag_c::UNK_2708);
        }
    }
    return TRUE;
}

/* 022452CC */
BOOL daNpc_Hr_c::demoProcTact2() {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}

/* 02245308 */
BOOL daNpc_Hr_c::demoProcTact3() {
    if (daPy_getTactTimerCancel(dComIfGp_getLinkPlayer()) > 0) {
        mEventIdx = dComIfGp_evmng_getEventIdx("TACTM_RT", 0xFF);
        setEvFlag(EVFLAG_EVENT_CHANGE_REQ);
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        fopAcM_orderChangeEventId(pLink, this, mEventIdx, 0, 0xFFFF);
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}

/* 02244834 */
int daNpc_Hr_c::calcKaijou(int param_1) {
    int sum = 0;
    for (int i = 1; param_1 >= 1; i++, param_1--) {
        sum += i;
    }
    return sum;
}

/* 02244858 */
void daNpc_Hr_c::demoInitMove() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    cXyz* pPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos");
    if (pPos != nullptr) {
        mTargetPos.x = pPos->x;
        mTargetPos.y = pPos->y;
        mTargetPos.z = pPos->z;
    } else {
        cXyz* pPos3 = dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos3");
        if (pPos3 != nullptr && pShip != nullptr) {
            fpoAcM_absolutePos(pShip, pPos3, &mTargetPos);
        } else {
            cXyz* pPos2 = dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos2");
            if (pPos2 != nullptr) {
                fpoAcM_absolutePos(pLink, pPos2, &mTargetPos);
            }
        }
    }
    be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Timer");
    if (pTimer != nullptr) {
        mMoveTimer = (s16)*pTimer;
        if (mMoveTimer < 10) {
            mMoveTimer = 10;
        }
    } else {
        mMoveTimer = 0x14;
    }
    be<s32>* pBrake = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Brake");
    if (pBrake != nullptr) {
        mBrakeTime = (s16)*pBrake;
    } else {
        mBrakeTime = 0;
    }
    be<s32>* pAngleY = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "AngleY");
    if (pAngleY != nullptr) {
        s16 a = (s16)*pAngleY;
        mTargetAngle = a;
        if (chkEvFlag(EVFLAG_LOCK_ROTATION)) {
            current.angle.y = a;
        }
    } else {
        mTargetAngle = current.angle.y;
    }
    be<s32>* pSoundOff = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "SOUND_OFF");
    if (pSoundOff == nullptr) {
        /* fopAcM_seStart(this, JA_SE_CM_HU_RC_MOVE_RAPID, 0) (HD: at current.pos) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(JA_SE_CM_HU_RC_MOVE_RAPID, &current.pos, 0, reverb);
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

/* 02244C58 */
void daNpc_Hr_c::demoInitSmall() {
    mScaleFactor = 11.0f;
    mMoveTimer = 0;
}

/* 022453B8 */
BOOL daNpc_Hr_c::demoProcSmall() {
    f64 r = hrAddCalcWide(&mScaleFactor, 1.0f, 0.1f, 100.0f, 0.3f);
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

/* 02244FA4 */
bool daNpc_Hr_c::demoProcMove() {
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

/* 02244C70 */
void daNpc_Hr_c::demoInitChange() {
    be<s32>* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "prm0");
    if (a_intP == nullptr) { /* JUT_ASSERT(1209, a_intP) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x4B9, "a_intP");
        return;
    }
    s32 prm = *a_intP;
    switch (prm) {
    case 1:
        mEventIdx = dComIfGp_evmng_getEventIdx("TACT1_RT", 0xFF);
        break;
    default:
        mEventIdx = dComIfGp_evmng_getEventIdx("PATTEN_RT", 0xFF);
        break;
    }
    setEvFlag(EVFLAG_EVENT_CHANGE_REQ);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    fopAcM_orderChangeEventId(pLink, this, mEventIdx, 0, 0xFFFF);
}

/* 02244068 */
void daNpc_Hr_c::demoInitCom() {
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();

    be<s32>* turnPl = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "TURN_PL");
    if (turnPl != nullptr) {
        if (*turnPl != 0) {
            setEvFlag(EVFLAG_LOCK_ROTATION);
        } else {
            clrEvFlag(EVFLAG_LOCK_ROTATION);
        }
    }
    be<s32>* tornado = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "TORNADO");
    if (tornado != nullptr) {
        if (*tornado != 0) {
            setEvFlag(EVFLAG_TORNADO_ACTIVE);
        } else {
            clrEvFlag(EVFLAG_TORNADO_ACTIVE);
        }
    }
    be<s32>* update = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "UPDATE");
    if (update != nullptr) {
        if (*update != 0) {
            clrEvFlag(EVFLAG_SKIP_UPDATE);
        } else {
            setEvFlag(EVFLAG_SKIP_UPDATE);
        }
    }
    be<s32>* tornado_off = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "TORNADE_OFF");
    if (tornado_off != nullptr) {
        dComIfGs_onEventBit(dSv_event_flag_c::UNK_2710);
        s32 sw = getSwbit();
        dComIfGs_onSwitch(sw, fopAcM_GetRoomNo(this));
        mReturnState = 1;
    }
    be<s32>* anm = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "ANM");
    if (anm != nullptr) {
        setAnm((s8)*anm);
    }
    be<s32>* texAnm = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "TEXANM");
    if (texAnm != nullptr) {
        setTexPtn((s8)*texAnm);
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Sound_Init") != nullptr) {
        mDoAud_tact_reset();
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Sound_Play") != nullptr) {
        mDoAud_tact_melodyPlay(1);
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Sound_End") != nullptr) {
        mDoAud_tact_reset();
        clrEvFlag(EVFLAG_TACT_SOUND_ACTIVE);
    }
    cXyz* startPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, "StartPos");
    if (startPos != nullptr) {
        current.pos.x = startPos->x;
        current.pos.y = startPos->y;
        current.pos.z = startPos->z;
    }
    cXyz* startPos2 = dComIfGp_evmng_getMyXyzP(mStaffIdx, "StartPos2");
    if (startPos2 != nullptr) {
        fpoAcM_absolutePos(pLink, startPos2, &current.pos);
    }
    cXyz* startPos3 = dComIfGp_evmng_getMyXyzP(mStaffIdx, "StartPos3");
    if (startPos3 != nullptr && pShip != nullptr) {
        fpoAcM_absolutePos(pShip, startPos3, &current.pos);
    }
    be<s32>* wind = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Wind");
    be<f32>* windPrm = dComIfGp_evmng_getMyFloatP(mStaffIdx, "WindPrm");
    if (wind != nullptr) {
        switch ((s32)*wind) {
        case 1:
            if (windPrm != nullptr) {
                s32 n = dComIfGp_evmng_getMySubstanceNum(mStaffIdx, "WindPrm");
                mClothes.create(this, (u8)*wind, windPrm, n);
            }
            break;
        default:
            mClothes.end();
            break;
        }
    }
    if (dComIfGp_evmng_getMyIntegerP(mStaffIdx, "AWAY") != nullptr) {
        mDoAud_seStart_noPos(JA_SE_CM_HU_RC_FLY_AWAY);
        if (getShapeType() == 1) {
            mDoAud_seStart_noPos(JA_SE_CV_RC_FLY_AWAY);
        } else {
            mDoAud_seStart_noPos(JA_SE_CV_HU_FLY_AWAY);
        }
    }
}

/* 022454A4 */
void daNpc_Hr_c::demoProcCom() {
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
        f64 sq = hrVectorSquareMagWide(&speed);
        f64 temp = hrSqrtWide(sq);
        if (temp > 15.0) {
            temp = 15.0;
        }
        f32 ratio = (f32)(temp / 15.0);
        f32 v = 100.0f * ratio;
        u32 param = !(v < 2147483648.0f) ? (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u
                                         : (u32)gabi::ftoi(v);
        /* fopAcM_seStart(this, JA_SE_CM_HU_RC_FLYING, (temp / 15) * 100) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(JA_SE_CM_HU_RC_FLYING, &eyePos, param, reverb);
    }
}

// HD: full r3 forwarding is required for the bool state methods below.
/* 022455CC */
u32 daNpc_Hr_c::demoProc() {
    u32 temp; /* bool: the results are passed on unnormalised */
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    temp = true;
    if (chkEvFlag(EVFLAG_EVENT_CHANGE_REQ)) {
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Hr2", nullptr, 0);
        clrEvFlag(EVFLAG_EVENT_CHANGE_REQ);
    }
    s32 eventAction = getNowEventAction();
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        setFlag(HR_FLAG_00000002);
        switch ((u32)eventAction) {
        case HR_DEMO_SPEAK:
        case HR_DEMO_PATTEN:
            demoInitSpeak();
            break;
        case HR_DEMO_WIND0:
            demoInitWind();
            break;
        case HR_DEMO_HIDE_LINK: {
            daPy_onPlayerNoDraw(pLink);
            s32 sw = getSwbit();
            dComIfGs_offSwitch(sw, fopAcM_GetRoomNo(this));
            break;
        }
        case HR_DEMO_DISP_LINK:
            daPy_offPlayerNoDraw(pLink);
            break;
        case HR_DEMO_DISP_MY: {
            cXyz* pPos = dComIfGp_evmng_getMyXyzP(mStaffIdx, "Pos");
            if (pPos != nullptr) {
                current.pos.x = pPos->x;
                current.pos.y = pPos->y;
                current.pos.z = pPos->z;
            }
            be<s32>* pAngleY = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "AngleY");
            if (pAngleY != nullptr) {
                current.angle.y = (s16)*pAngleY;
            }
            offHide(0);
            demoInitWait();
            mAnmIdx = -1;
            setAnm(0);
            break;
        }
        case HR_DEMO_MOVE:
            demoInitMove();
            break;
        case HR_DEMO_TURN_LINK: {
            mTargetAngle = fopAcM_searchActorAngleY(pLink, this);
            be<s32>* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, "Timer");
            if (pTimer != nullptr) {
                mMoveTimer = (s16)*pTimer;
            } else {
                mMoveTimer = 0;
            }
            break;
        }
        case HR_DEMO_LOOK:
            setAnm(0);
            demoInitWait();
            break;
        case HR_DEMO_WAIT:
            demoInitWait();
            break;
        case HR_DEMO_SMALL:
            demoInitSmall();
            break;
        case HR_DEMO_INTRO:
            speed.y = 0.0f;
            break;
        case HR_DEMO_CHANGE:
            demoInitChange();
            break;
        case HR_DEMO_TACT2:
            clrFlag(HR_FLAG_00000200);
            break;
        }
    }

    switch ((u32)eventAction) {
    case HR_DEMO_SPEAK:
        demoProcSpeak();
        break;
    case HR_DEMO_PATTEN:
        demoProcPatten();
        break;
    case HR_DEMO_WIND0:
        temp = hrDispatch_demoProcWind(this, 0);
        break;
    case HR_DEMO_WIND1:
        temp = hrDispatch_demoProcWind(this, 1);
        break;
    case HR_DEMO_MOVE:
        temp = hrDispatch_demoProcMove(this);
        break;
    case HR_DEMO_DISP_MY:
        demoProcWait();
        break;
    case HR_DEMO_TURN_LINK:
        if (mMoveTimer > 0) {
            mMoveTimer = (s16)(mMoveTimer - 1);
        } else {
            cSAngle angle;
            angle.mAngle = pLink->shape_angle.y;
            cLib_addCalcAngleS(&angle.mAngle, mTargetAngle, 1, 0x800, 0x800);
            daPy_setPlayerPosAndAngle(pLink, nullptr, angle.mAngle);
            if (angle.mAngle == mTargetAngle) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
            }
        }
        break;
    case HR_DEMO_LOOK:
        demoProcWait();
        setFlag(HR_FLAG_00000040);
        break;
    case HR_DEMO_WAIT:
        demoProcWait();
        break;
    case HR_DEMO_TACT0:
        demoProcTact0();
        break;
    case HR_DEMO_TACT1:
        demoProcTact1();
        break;
    case HR_DEMO_TACT2:
        demoProcTact2();
        break;
    case HR_DEMO_TACT3:
        demoProcTact3();
        break;
    case HR_DEMO_SMALL:
        demoProcSmall();
        break;
    case HR_DEMO_INTRO: {
        f64 r = hrAddCalcWide(&current.pos.y, home.pos.y + 6000.0f, 0.5f, 100.0f, 5.0f);
        if (r < 5.0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    }
    case HR_DEMO_CHANGE:
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    demoProcCom();
    return temp;
}

/* 02245C84 */
int daNpc_Hr_c::getNowJointY() {
    fopAc_ac_c* tornadoActor = fopAcM_SearchByID(mProcId);
    // HD: asserts the tornado (and returns 10 without it)
    if (tornadoActor == nullptr) {
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x8EA, "a_tornado != (0)");
        return 10;
    }
    int i = 0;
    while (i < 10) {
        J3DModel* m = ((HrTornadoHD*)tornadoActor)->mpModel.get();
        if (m != nullptr) {
            f32 jointY = getAnmMtx(m, i)->m[1][3];
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

/* 02245D80 */
void daNpc_Hr_c::getTornadoPos(int jointY, cXyz* tornadoPos) {
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

/* 02246088 */
bool daNpc_Hr_c::rideTornado() {
    fopAc_ac_c* actor = fopAcM_SearchByID(mProcId);
    if (actor == nullptr) {
        speed.x = speed.x * 0.95f;
        speed.y = 0.0f;
        speed.z = speed.z * 0.95f;
        return false;
    }
    cXyz tornadoPos;
    getTornadoPos(getNowJointY(), &tornadoPos);
    speed.x = gabi::fmadds(tornadoPos.x - current.pos.x, 0.8f, speed.x * 0.2f);
    speed.z = gabi::fmadds(tornadoPos.z - current.pos.z, 0.8f, speed.z * 0.2f);
    return true;
}

/* 022467CC */
bool daNpc_Hr_c::rt_search() {
    fopAc_ac_c* ac = fopAcM_searchFromName("Ytrnd00", 0, 0);
    if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_2710)) {
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
        if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_3D01)) {
            mEventIdx = dComIfGp_evmng_getEventIdx("INTRO_RT", 0xFF);
        } else {
            mEventIdx = dComIfGp_evmng_getEventIdx("INTRO_RT_F", 0xFF);
        }
        mMoveTimer = 0x14;
    }
    return false;
}

/* 02246660 */
bool daNpc_Hr_c::rt_hide() {
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    current.angle.y = fopAcM_searchActorAngleY(this, pLink);
    cXyz tornadoPos;
    getTornadoPos(0, &tornadoPos);
    current.pos.x = tornadoPos.x;
    current.pos.y = tornadoPos.y - 1500.0f;
    current.pos.z = tornadoPos.z;
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mReturnState = HR_STATE_RT_ANGRY;
        mState = HR_STATE_RT_INTRO;
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Hr2", nullptr, 0);
        setEvFlag(EVFLAG_TORNADO_ACTIVE);
        demoProc();
        mDoAud_seStart_noPos(JA_SE_CV_RC_ENTER);
        mDoAud_bgmAllMute(90);
        dComIfGs_onEventBit(dSv_event_flag_c::UNK_3D01);
    } else if (pShip != nullptr && daShip_checkTornadoFlg(pShip)) {
        if (mMoveTimer > 0) {
            mMoveTimer = (s16)(mMoveTimer - 1);
        } else {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
            dComIfGs_onTmpBit(dSv_event_tmp_flag_c::UNK_0404);
        }
    }
    return true;
}

/* 022467C8 */
bool daNpc_Hr_c::rt_intro() {
    return rt_hit1();
}

/* 022461B0 */
bool daNpc_Hr_c::rt_angry() {
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
        mDoAud_seStart_noPos(JA_SE_CV_RC_ENTER);
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
        cXyz scl;
        scl.x = 11.0f;
        scl.y = 11.0f;
        scl.z = 11.0f;
        cXyz diff;
        cXyz_mi(hitPos, &diff, &pLink->eyePos);
        cXyz temp;
        temp.copy(diff);
        if (!hrVectorIsZero(&temp)) {
            cXyz nres;
            hrVectorNormalize(&temp, &nres);
            csXyz temp2;
            cM3d_CalcVecZAngle(&temp, &temp2);
            dComIfGp_particle_set(dPa_name::ID_AK_JN_OK, hitPos, &temp2, &scl);
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

/* 02246180 */
bool daNpc_Hr_c::rt_win() {
    speed.y = 0.0f;
    rideTornado();
    return true;
}

/* 02246484 */
bool daNpc_Hr_c::rt_hit0() {
    fopAc_ac_c* pShip = dComIfGp_getShipActor();
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mState = HR_STATE_RT_HIT_1;
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Hr2", nullptr, 0);
        setEvFlag(EVFLAG_TORNADO_ACTIVE);
        demoProc();
    } else if (daShip_checkTornadoUp(pShip)) {
        mState = HR_STATE_RT_WIN;
        actor_status = actor_status | 0x800; /* fopAcM_OnStatus(this, fopAcStts_UNK800_e) */
        setAnm(12);
        mDoAud_seStart_noPos(JA_SE_CV_RC_ENTER);
    } else {
        fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    }
    speed.y = 0.0f;
    rideTornado();
    return true;
}

/* 02246598 */
bool daNpc_Hr_c::rt_hit1() {
    if (chkEvFlag(EVFLAG_TORNADO_ACTIVE)) {
        rideTornado();
    }
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        if (mReturnState == 1) {
            onHide(0);
        }
        if (mState == HR_STATE_RT_INTRO) {
            mDoAud_subBgmStart(JA_BGM_DIOCTA_BATTLE);
        }
        endTalk();
        return false;
    }
    demoProc();
    return true;
}

/* tail call */
/* 02246950 */
bool daNpc_Hr_c::ht_hide() {
    if (chkFlag(HR_FLAG_00000008)) {
        /* fopAcM_seStart(this, JA_SE_PRE_TAKT, 0) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(JA_SE_PRE_TAKT, &eyePos, 0, reverb);
        mReturnState = mState;
        mState = HR_STATE_HT_TACT;
        setAnmStatus();
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Hr", nullptr, 0);
        demoProc();
        s32 sw = getSwbit();
        dComIfGs_onSwitch(sw, fopAcM_GetRoomNo(this));
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mHindFlag =
            0xFFFF; /* dComIfGp_event_onHindFlag(-1) */
    } else if (chkFlag(HR_FLAG_00000001)) {
        mReturnState = mState;
        mState = HR_STATE_TALK;
        setAnmStatus();
        daPy_onPlayerNoDraw(dComIfGp_getLinkPlayer());
    } else {
        if (fopAcM_seenActorAngleY(dComIfGp_getLinkPlayer(), this) < 0x2AAA) {
            SetOrder(1);
            if (!dComIfGs_isEventBit(dSv_event_flag_c::UNK_2708)) {
                SetOrder(4);
            }
        }
    }
    return true;
}

// HD: JPABaseEmitter::setGlobalPrmColor / setGlobalEnvColor store gamma-corrected colours
static inline u8 hr_gammaByte(u8 c) {
    return (u8)gabi::ftoi(hr_colorGamma(c) * 255.0f);
}

/* 02246AB0 */
void daNpc_Hr_c::smokeProc() {
    u16 fl = mFlags;
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    if (!(fl & HR_FLAG_00000010) && pEmitter != nullptr && mType == 1 && mHitDelayTimer != 0) {
        u8 t = (u8)(mHitDelayTimer - 1);
        mHitDelayTimer = t;
        if (t >= 0x50) {
            HrEmitterHD* e = (HrEmitterHD*)pEmitter;
            u8 r = hr_gammaByte(0x7F);
            u8 g = hr_gammaByte(0x5E);
            u8 b = hr_gammaByte(0x91);
            e->mPrmR = r; /* setGlobalPrmColor(0x7F, 0x5E, 0x91) */
            e->mPrmG = g;
            e->mPrmB = b;
            r = hr_gammaByte(0x6C);
            g = hr_gammaByte(0x58);
            b = hr_gammaByte(0x7B);
            e->mEnvR = r; /* setGlobalEnvColor(0x6C, 0x58, 0x7B) */
            e->mEnvG = g;
            e->mEnvB = b;
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

/* 02245BB8 */
u32 daNpc_Hr_c::ht_tact01() {
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_2708)) {
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

/* daTornado_c::getJointXPos/YPos/ZPos (HD inline): the joint's animation matrix translation, or the
 * tornado's position without a model (mpModel at +0x3B4) */
static inline f32 tornado_getJointPos(fopAc_ac_c* actor, int joint, int component) {
    J3DModel* model = ((HrTornadoHD*)actor)->mpModel;
    if (model != nullptr)
        return getAnmMtx(model, joint)->m[component][3];
    if (component == 0)
        return actor->current.pos.x;
    if (component == 1)
        return actor->current.pos.y;
    return actor->current.pos.z;
}
static inline f32 tornado_getJointXPos(fopAc_ac_c* actor, int joint) {
    return tornado_getJointPos(actor, joint, 0);
}
static inline f32 tornado_getJointYPos(fopAc_ac_c* actor, int joint) {
    return tornado_getJointPos(actor, joint, 1);
}
static inline f32 tornado_getJointZPos(fopAc_ac_c* actor, int joint) {
    return tornado_getJointPos(actor, joint, 2);
}

/* 02246D58 */
BOOL daNpc_Hr_c::wait_action(void*) {
    if (mActionStatus == ACTION_STARTING /* ACTION_STARTING */) {
        switch ((u32)(s32)mType) {
        case 0:
            mState = HR_STATE_HT_HIDE;
            eventInfo.setXyCheckCB(
                daNpc_hr_XyCheckCB); /* eventInfo.setXyCheckCB(&daNpc_hr_XyCheckCB) */
            eventInfo.setXyEventCB(
                daNpc_hr_XyEventCB); /* eventInfo.setXyEventCB(&daNpc_hr_XyEventCB) */
            break;
        case 1:
            if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_2710)) {
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
    } else if (mActionStatus != ACTION_ENDING /* ACTION_ENDING */) {
        clrFlag(HR_FLAG_00000100 | HR_FLAG_00000080 | HR_FLAG_00000040);
        u32 temp; /* bool: the state function's r3 is passed on unnormalised */
        switch ((u32)(s32)mState) {
        case HR_STATE_WAIT_01:
            temp = hrDispatch_wait01(this);
            break;
        case HR_STATE_WAIT_02:
            temp = true; /* wait02() (inline) */
            break;
        case HR_STATE_TALK:
            temp = hrDispatch_talk01(this);
            break;
        case HR_STATE_HT_TACT:
            temp = ht_tact01();
            break;
        case HR_STATE_RT_SEARCH:
            temp = hrDispatch_rt_search(this);
            break;
        case HR_STATE_RT_HIDE:
            temp = hrDispatch_rt_hide(this);
            break;
        case HR_STATE_RT_INTRO:
            temp = hrDispatch_rt_intro(this);
            break;
        case HR_STATE_RT_ANGRY:
            temp = hrDispatch_rt_angry(this);
            break;
        case HR_STATE_RT_WIN:
            temp = hrDispatch_rt_win(this);
            break;
        case HR_STATE_RT_HIT_0:
            temp = hrDispatch_rt_hit0(this);
            break;
        case HR_STATE_RT_HIT_1:
            temp = hrDispatch_rt_hit1(this);
            break;
        case HR_STATE_HT_HIDE:
            temp = hrDispatch_ht_hide(this);
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

/* static initialisation of the translation unit (only the per-TU header statics) */

// HD: compiler-generated and template code of this translation unit.
/* 022470AC */
static void __sinit_d_a_npc_hr_cpp() {
    sinit_header_statics(gabi::ea(&l_staticListHead), gabi::ea(&l_staticConstructors));
}

/* sead::SafeString deleting destructor (this TU's copy; vtable 0x1001A76C slot +0xC) */
/* 02247140 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* daNpc_Wind_Eff constructor (compiler-generated; allocates when this == NULL) */
/* 02247154 */
daNpc_Wind_Eff* daNpc_Wind_Eff_ct(daNpc_Wind_Eff* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Wind_Eff*)operator_new(0x38);
        if (i_this == nullptr)
            return i_this;
    }
    dPa_followEcallBack_ct(&i_this->mpFollowECallBack, 0, 0);
    return i_this;
}

/* daNpc_Wind_Eff deleting destructor (compiler-generated) */
/* 022471A4 */
void daNpc_Wind_Eff_dt(daNpc_Wind_Eff* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* daNpc_Hr_c deleting destructor (compiler-generated, HD virtual destructor) */
/* 022471B8 */
static void daNpc_Hr_c_dt(daNpc_Hr_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        hrDestroyArray(&i_this->mClothes, 4, sizeof(daNpc_Wind_Eff),
                       gabi::guest_fn((const void*)daNpc_Wind_Eff_dt), 0,
                       0); /* __destroy_arr(mWindEff) */
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        hrActorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
/* 0224725C */
static void SafeString_assureTerminationImpl(SafeString*) {}

// clang-format off
/* WWHD glue
data l_staticListHead 104671D0
data l_staticConstructors 101BE958
ext 028F0164 void hrDestroyArray(void* array, u32 count, u32 size, u32 callback, u32 p4, u32 p5)
ext 025D50BC void hrActorDestroy(fopAc_ac_c* actor, s32 flags)
ext 0200ECD4 f64 hrAddCalcWide(be<f32>* value, f32 target, f32 scale, f32 maximum, f32 minimum)
ext 028E8DD0 f64 hrVectorSquareMagWide(const cXyz* vector)
ext 028F4384 f64 hrSqrtWide(f64 value)
ext 02244E58 u32 hrDispatch_demoProcWind(daNpc_Hr_c* actor, int mode)
ext 02244FA4 u32 hrDispatch_demoProcMove(daNpc_Hr_c* actor)
ext 02243E34 u32 hrDispatch_wait01(daNpc_Hr_c* actor)
ext 02243F20 u32 hrDispatch_talk01(daNpc_Hr_c* actor)
ext 022467CC u32 hrDispatch_rt_search(daNpc_Hr_c* actor)
ext 02246660 u32 hrDispatch_rt_hide(daNpc_Hr_c* actor)
ext 022467C8 u32 hrDispatch_rt_intro(daNpc_Hr_c* actor)
ext 022461B0 u32 hrDispatch_rt_angry(daNpc_Hr_c* actor)
ext 02246180 u32 hrDispatch_rt_win(daNpc_Hr_c* actor)
ext 02246484 u32 hrDispatch_rt_hit0(daNpc_Hr_c* actor)
ext 02246598 u32 hrDispatch_rt_hit1(daNpc_Hr_c* actor)
ext 02246950 u32 hrDispatch_ht_hide(daNpc_Hr_c* actor)
str 1001AB14 "Timer" in 022447D0
str 1001AAEC "MsgNo" in 02244554
str 1001AAFC "d_a_npc_hr.cpp" in 02244554
str 1001AAF4 "a_intP" in 02244554
str 1001AB90 "TACTM_RT" in 02245308
str 1001AB1C "Pos" in 02244858
str 1001AB30 "Pos3" in 02244858
str 1001AB38 "Pos2" in 02244858
str 1001AB20 "Timer" in 02244858
str 1001AB28 "Brake" in 02244858
str 1001AB40 "AngleY" in 02244858
str 1001AB48 "SOUND_OFF" in 02244858
str 1001AB58 "prm0" in 02244C70
str 1001AB74 "d_a_npc_hr.cpp" in 02244C70
str 1001AB60 "a_intP" in 02244C70
str 1001AB84 "TACT1_RT" in 02244C70
str 1001AB68 "PATTEN_RT" in 02244C70
str 1001AA5C "TURN_PL" in 02244068
str 1001AA64 "TORNADO" in 02244068
str 1001AA78 "UPDATE" in 02244068
str 1001AA98 "TORNADE_OFF" in 02244068
str 1001AA74 "ANM" in 02244068
str 1001AA80 "TEXANM" in 02244068
str 1001AAC8 "Sound_Init" in 02244068
str 1001AAD4 "Sound_Play" in 02244068
str 1001AAA4 "Sound_End" in 02244068
str 1001AAE0 "StartPos" in 02244068
str 1001AAB0 "StartPos2" in 02244068
str 1001AABC "StartPos3" in 02244068
str 1001AA88 "Wind" in 02244068
str 1001AA6C "WindPrm" in 02244068
str 1001AA90 "AWAY" in 02244068
str 1001ABB0 "Hr2" in 022455CC
str 1001ABB4 "Pos" in 022455CC
str 1001ABC0 "AngleY" in 022455CC
str 1001ABB8 "Timer" in 022455CC
str 1001ABC8 "d_a_npc_hr.cpp" in 02245C84
str 1001ABD8 "a_tornado != (0)" in 02245C84
str 1001ABF4 "Hr2" in 02246484
str 1001ABFC "Hr2" in 02246660
str 1001AC00 "Ytrnd00" in 022467CC
str 1001AC08 "INTRO_RT" in 022467CC
str 1001AC14 "INTRO_RT_F" in 022467CC
str 1001AC20 "Hr" in 02246950
*/
// clang-format on
