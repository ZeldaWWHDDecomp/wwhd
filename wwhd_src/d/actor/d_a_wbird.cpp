/**
 * d_a_wbird.cpp (WWHD)
 * Wind Waker "wind bird" (the wind direction demo after playing the Wind's Requiem)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_wbird.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_wbird.h"
const gc_tu_data gc_tu = {.headerStatics = {0x1046EBC8, 0x101D332C, 0x1046EBD4}};

// HD: this actor has a separate deleting destructor in its vtable.
static constexpr u32 WBIRD_VTBL = 0x10042A2C;
enum Action { ACTION_WAIT, ACTION_END, ACTION_MOVE, ACTION_SELECT };
enum { daPyStts0_SHIP_RIDE_e = 0x10000, DEMO_TYPE_SYSTEM = 2, DEMO_WAIT = 1 };

enum {
    JA_SE_TAKT_WIND_DISP = 0x872,
    JA_SE_TAKT_WIND_CANCEL = 0x873,
    JA_SE_TAKT_WIND_DECIDE = 0x875,
    JA_SE_TAKT_WIND_DEMO = 0x876,
    JA_SE_TAKT_WIND_END = 0x877,
    JA_SE_SYS_WTAKT_WIND_AMB = 0x1074,
};

// HD: the sound overload has no position argument.
void mDoAud_seStartWind(u32);
void dKyw_custom_windpower(f32);
void dKyw_tact_wind_set_go();
BOOL Wbird_orderChangeEvent(fopAc_ac_c *, const char *, u16, u16);
void Wbird_globeConstruct(WbirdGlobeHD *, const cXyz *);
void Wbird_angleCopy(be<s16> *, const be<s16> *);
void Wbird_angleSubtract(const be<s16> *, be<s16> *, s16);
void Wbird_angleAddAssign(be<s16> *, const be<s16> *);
void Wbird_angleSubtractAssign(be<s16> *, const be<s16> *);
f32 Wbird_angleSin(const be<s16> *);
f32 Wbird_angleCos(const be<s16> *);
BOOL Wbird_endCheck(dEvent_manager_c *, s16);
s32 Wbird_getStaffId(dEvent_manager_c *, const char *, void *, s32);
// HD: event lookup and ordering pass the entire return register, including its upper half.
s32 Wbird_getEventIdx(dEvent_manager_c *, const char *, u8);
BOOL Wbird_orderChangeEventId(fopAc_ac_c *, s32, u16, u16);
using WbirdAngleHD = be<s16>;
static WbirdAngleHD l_angleZero;
static WbirdAngleHD l_angleQuarterTurn;
static WbirdSinCosTableHD l_sinCosTable;

/* 024DE930 */
void daWbird_c::calcMtx() {
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    eyePos.z = z;
    attention_info.position.y = y;
    attention_info.position.x = x;
    attention_info.position.z = z;
    eyePos.x = x;
    eyePos.y = y;
}

/* 024DE334 */
void daWbird_c::setStartPos() {
    WbirdPlayerHD *player = static_cast<WbirdPlayerHD *>(dComIfGp_getPlayer(0));
    cXyz *wind = dKyw_get_wind_vec();
    WbirdEnvLightHD *envLight = (WbirdEnvLightHD *)dKy_getEnvlight();

    gabi::Local<WbirdGlobeHD> globe;
    Wbird_globeConstruct(globe, wind);
    gabi::Local<be<s16>> angle;
    Wbird_angleCopy(angle, &globe->mInclination);
    gabi::Local<be<s16>> a;
    Wbird_angleSubtract(&globe->mInclination, a, player->current.angle.y);
    if (*a.get() > l_angleZero) {
        Wbird_angleAddAssign(angle, &l_angleQuarterTurn);
    } else {
        Wbird_angleSubtractAssign(angle, &l_angleQuarterTurn);
    }

    f32 px = player->current.pos.x;
    current.pos.x = px;
    current.pos.y = player->current.pos.y;
    current.pos.z = player->current.pos.z;
    f32 c = Wbird_angleCos(angle);
    current.pos.x = gabi::fmadds(20.0f, c, px);
    f32 s = Wbird_angleSin(angle);
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    home.pos.x = x;
    f32 y = current.pos.y;
    z = gabi::fmadds(20.0f, s, z);
    home.pos.y = y;
    home.pos.z = z;
    current.pos.z = z;
    current.pos.y = y + 150.0f;

    u16 wx = envLight->mTactWindAngleX; /* mWind.mTactWindAngleX */
    u16 wy = envLight->mTactWindAngleY; /* mWind.mTactWindAngleY */
    f32 cx = l_sinCosTable.mValues[wx >> 3][1];
    f32 windX = cx * l_sinCosTable.mValues[wy >> 3][1];
    f32 windZ = cx * l_sinCosTable.mValues[wy >> 3][0];
    s16 windAngle = cM_atan2s(windX, windZ);

    // HD: the flight time is 60 frames (GameCube 80).
    f32 halfFlightTime = (f32)(s16)60 * 0.5f;
    f32 velocityX = 40.0f * windX;
    f32 velocityZ = 40.0f * windZ;
    f32 g = 1.0f * halfFlightTime;
    speed.x = velocityX;
    f32 pz = current.pos.z;
    f32 startX = current.pos.x;
    speed.z = velocityZ;
    f32 py = current.pos.y;
    current.angle.y = windAngle;
    current.pos.z = gabi::fnmsubs(halfFlightTime, velocityZ, pz);
    current.pos.x = gabi::fnmsubs(halfFlightTime, velocityX, startX);
    field_0x29E = 60 + 30;
    shape_angle.y = windAngle;
    field_0x2A0 = 1.0f;
    current.pos.y = gabi::fmadds(halfFlightTime * halfFlightTime, 0.5f, py);
    speed.y = -g;
}

/* 024DE9F0 */
BOOL daWbird_c::CreateInit() {
    s8 room = current.roomNo;
    setAction(ACTION_SELECT);
    tevStr.mRoomNo = room;
    Wbird_orderChangeEvent(this, "TACT_WINDOW", 0, 0xFFFF);
    field_0x29E = 0;
    calcMtx();
    return TRUE;
}

/* 024DEA54 */
cPhs_State daWbird_c::create() {
    /* fopAcM_ct(this, daWbird_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = WBIRD_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 024DE330 */
static void daWbird_actionWaitCopy() {}

/* 024DE880 */
void daWbird_c::actionEnd() {
    s16 idx = mEventIdx;
    BOOL end = Wbird_endCheck(dComIfGp_getPEvtManager(), idx); /* endCheck */
    dComIfG_inf_c *play = (dComIfG_inf_c *)dComIfGp_get();
    if (end) {
        /* dComIfGp_event_reset() */
        play->play.mEvtCtrl.mEventFlag |= 8;
        fopAcM_delete(this);
    } else {
        s32 staff = Wbird_getStaffId(&play->play.mEvtManager, "WINDMAN", nullptr, 0);
        dComIfGp_evmng_cutEnd(staff);
    }
}

/* 024DE768 */
void daWbird_c::actionMove() {
    WbirdPlayerHD *player = static_cast<WbirdPlayerHD *>(dComIfGp_getPlayer(0));
    if (field_0x29D) {
        field_0x29D = false;
        mDoAud_seStartWind(JA_SE_TAKT_WIND_DEMO);
    }
    if (field_0x29E > 0) {
        field_0x29E = field_0x29E - 1;
        // HD: thresholds are 30 / 20; GameCube uses 60 / 70 and fades wind below 15.
        if (field_0x29E > 30) {
            fopAcM_posMove(this, nullptr);
            f32 g = field_0x2A0;
            speed.y = speed.y + g;
        }
        if (field_0x29E == 20) {
            dKyw_tact_wind_set_go();
        }
    } else {
        dKyw_custom_windpower(0.0f);
        dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId("WINDMAN", nullptr, 0));
        /* player->changeDemoMoveAngle(mAngle) */
        player->mDemo.mDemoMoveAngle = mAngle;
        setAction(ACTION_END);
        mDoAud_seStartWind(JA_SE_TAKT_WIND_END);
    }
}

/* 024DE55C */
void daWbird_c::actionSelect() {
    s16 timer = field_0x29E;
    if (timer == 10) {
        /* dComIfGp_setOperateWindOn() */
        ((dComIfG_inf_c *)dComIfGp_get())->play.mOperateWind = 2;
        mDoAud_seStartWind(JA_SE_TAKT_WIND_DISP);
        field_0x29E = field_0x29E + 1;
    } else if (timer > 10) {
        mDoAud_seStartWind(JA_SE_SYS_WTAKT_WIND_AMB);
        u8 op = ((dComIfG_inf_c *)dComIfGp_get())->play.mOperateWind; /* dComIfGp_getOperateWind() */
        if (op == 1) {
            mDoAud_seStartWind(JA_SE_TAKT_WIND_DECIDE);
            setStartPos();
            setAction(ACTION_MOVE);
            WbirdPlayerHD *player = static_cast<WbirdPlayerHD *>(dComIfGp_getPlayer(0));
            mAngle = player->shape_angle.y;
            /* player->setPlayerPosAndAngle(&player->current.pos, current.angle.y + 0x7FFF) (virtual) */
            ((daPy_py_c *)player)
                ->setPlayerPosAndAngle(&player->current.pos, (s16)(current.angle.y + 0x7FFF));
            u32 ship = dComIfGp_getPlayerStatus0() & daPyStts0_SHIP_RIDE_e; /* daPyStts0_SHIP_RIDE_e */
            /* the event index is passed on as the raw r3 of getEventIdx (not re-extended) */
            s32 idx;
            if (ship != 0) {
                idx = Wbird_getEventIdx(dComIfGp_getPEvtManager(), "TACT_WINDOW2_SHIP", 0xFF);
            } else {
                idx = Wbird_getEventIdx(dComIfGp_getPEvtManager(), "TACT_WINDOW2", 0xFF);
            }
            mEventIdx = (s16)idx;
            Wbird_orderChangeEventId(this, idx, 0, 0xFFFF); /* fopAcM_orderChangeEventId */
            /* player->cancelOriginalDemo() */
            WbirdPlayerHD *demoPlayer = static_cast<WbirdPlayerHD *>(dComIfGp_getPlayer(0));
            demoPlayer->mDemo.mDemoType = DEMO_TYPE_SYSTEM;
            demoPlayer->mDemo.mDemoMode = DEMO_WAIT;
            dKyw_custom_windpower(1.0f);
            field_0x29D = true;
        } else if (op < 1) {
            mDoAud_seStartWind(JA_SE_TAKT_WIND_CANCEL);
            dComIfG_inf_c *play = (dComIfG_inf_c *)dComIfGp_get();
            play->play.mEvtCtrl.mEventFlag |= 8; /* dComIfGp_event_reset() */
            fopAcM_delete(this);
        }
    } else {
        field_0x29E = timer + 1;
    }
}

/* 024DE958 */
static BOOL daWbird_Execute(daWbird_c *i_this) {
    switch (i_this->mAction) {
    case ACTION_SELECT:
        i_this->actionSelect();
        break;
    case ACTION_MOVE:
        i_this->actionMove();
        break;
    case ACTION_END:
        i_this->actionEnd();
        break;
    default:
        break; /* actionWait */
    }
    i_this->calcMtx();
    return TRUE;
}

/* 024DE9E0 */
static BOOL daWbird_IsDelete(daWbird_c *) {
    return TRUE;
}

// HD: actor deletion performs no destructor call.
/* 024DE9E8 */
static BOOL daWbird_Delete(daWbird_c *) {
    return TRUE;
}

/* 024DEABC */
static cPhs_State daWbird_Create(fopAc_ac_c *i_this) {
    return static_cast<daWbird_c *>(i_this)->create();
}

/* 024DEB54 */
static BOOL daWbird_Draw(daWbird_c *) {
    return TRUE;
}

/* 024DE2DC */
static void daWbird_c_dt(daWbird_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024DEAC0 */
static void __sinit_d_a_wbird_cpp() {
    __sinit_header_statics();
}

// HD: compiler-generated deleting destructor of this translation unit.

// HD: deleting destructor used by this actor; the earlier copy belongs to a neighboring vtable.
/* 024DEB5C */
static void daWbird_deletingDestructor(void *p, s32 flags) {
    if (p != nullptr) {
        __dt((fopAc_ac_c *)p); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue
data l_angleZero 101FF354
data l_angleQuarterTurn 101FF358
data l_sinCosTable 104A44F8
str 10042A94 in 024DE9F0 # "TACT_WINDOW"
str 10042A8C in 024DE880 # "WINDMAN"
str 10042A84 in 024DE768 # "WINDMAN"
str 10042A5C in 024DE55C # "TACT_WINDOW2_SHIP"
str 10042A70 in 024DE55C # "TACT_WINDOW2"
ext 025E1988 void mDoAud_seStartWind(u32)
ext 0257E594 void dKyw_custom_windpower(f32)
ext 0257E52C void dKyw_tact_wind_set_go()
ext 025D78FC BOOL Wbird_orderChangeEvent(fopAc_ac_c*, const char*, u16, u16)
ext 02007324 void Wbird_globeConstruct(WbirdGlobeHD*, const cXyz*)
ext 02006644 void Wbird_angleCopy(be<s16>*, const be<s16>*)
ext 02006908 void Wbird_angleSubtract(const be<s16>*, be<s16>*, s16)
ext 020068CC void Wbird_angleAddAssign(be<s16>*, const be<s16>*)
ext 020068E0 void Wbird_angleSubtractAssign(be<s16>*, const be<s16>*)
ext 02006814 f32 Wbird_angleSin(const be<s16>*)
ext 02006838 f32 Wbird_angleCos(const be<s16>*)
ext 025440C8 BOOL Wbird_endCheck(dEvent_manager_c*, s16)
ext 02542D88 s32 Wbird_getStaffId(dEvent_manager_c*, const char*, void*, s32)
ext 02543F10 s32 Wbird_getEventIdx(dEvent_manager_c*, const char*, u8)
ext 025D7874 BOOL Wbird_orderChangeEventId(fopAc_ac_c*, s32, u16, u16)
*/
