/**
 * d_a_npc_cb1_act.cpp (WWHD)
 * NPC - Makar (Korok cellist): route checks, NPC/player actions, event actions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1.h"

/* ---- this TU's statics (HD addresses) ---- */
#define CB1_M_STATUS 0x101BDC2A /* u16 daNpc_Cb1_c::m_status */
#define CB1_L_MSGID 0x10466B28  /* fpc_ProcID l_msgId */
#define CB1_L_HIO 0x10466B98    /* daNpc_Cb1_HIO_c l_HIO (0xF0, vtable at 0, GameCube layout) */
#define CB1_REG 0x1047B608      /* g_regHIO (HD debug registers) */
static inline f32 cb1_hioF(u32 off) { return gabi::load<f32>(CB1_L_HIO + off); }
static inline s16 cb1_hioS(u32 off) { return gabi::load<s16>(CB1_L_HIO + off); }
/* l_HIO fields (GameCube names) */
enum : u32 {
    HIO_mMaxAttnDistXZ = 0x08 + 0x0C, /* dNpc_HIO_c at 0x08 */
    HIO_mPlayerChaseDistance = 0x34,
    HIO_mChaseDistScale = 0x38,
    HIO_mMaxWalkSpeed = 0x3C,
    HIO_mMinWalkSpeed = 0x40,
    HIO_mForwardAccel = 0x44,
    HIO_mDecel = 0x50,
    HIO_mWalkAnmSpeedScale = 0x54,
    HIO_mMaxWalkAnmSpeed = 0x58,
    HIO_mHitSpeedScaleF = 0x68,
    HIO_mHitSpeedScaleY = 0x6C,
    HIO_field_0x80 = 0x80,
    HIO_field_0x84 = 0x84,
    HIO_mStickWalkSpeedScale = 0x88,
    HIO_mStickFlySpeedScale = 0xAC,
    HIO_mFlyLaunchSpeedY = 0xBC,
    HIO_mPlayerFlyTimer = 0xCA,
    HIO_field_0xE2 = 0xE2,
    HIO_field_0xE4 = 0xE4,
    HIO_field_0xE6 = 0xE6,
    HIO_field_0xE8 = 0xE8,
    HIO_field_0xEA = 0xEA,
    HIO_field_0xEC = 0xEC,
};
static inline f32 cb1_REG_F(u32 off) { return gabi::load<f32>(CB1_REG + off); }
static inline s16 cb1_REG_S(u32 off) { return gabi::load<s16>(CB1_REG + off); }

static inline u16 cb1_status() { return gabi::load<u16>(CB1_M_STATUS); }
static inline void cb1_setStatus(u16 v) { gabi::store<u16>(CB1_M_STATUS, v); }

/* ---- calls into the other parts of the unit (by address) ---- */
static inline BOOL cb1_setAnm(daNpc_Cb1_c* t, u8 n) { return gabi::call<BOOL>(0x02221978, t, n); }
static inline u8 cb1_getAnmType(daNpc_Cb1_c* t, int n) { return gabi::call<u8>(0x02221958, t, n); }
static inline void cb1_setWaitAction(daNpc_Cb1_c* t, void* p) { gabi::call(0x02222020, t, p); }
static inline void cb1_lookBack(daNpc_Cb1_c* t, BOOL b) { gabi::call(0x02221EF8, t, b); }
static inline BOOL cb1_initTalk(daNpc_Cb1_c* t) { return gabi::call<BOOL>(0x02223308, t); }
static inline BOOL cb1_execTalk(daNpc_Cb1_c* t, BOOL b) { return gabi::call<BOOL>(0x02223380, t, b); }
static inline BOOL cb1_walkAction(daNpc_Cb1_c* t, f32 spd, f32 acc, s16 ang) { return gabi::call<BOOL>(0x02222DD0, t, spd, acc, ang); }
static inline void cb1_setNpcAction(daNpc_Cb1_c* t, ProcFunc_l* f, void* p) { gabi::call(0x0221F2C4, t, f, p); }
static inline void cb1_setPlayerAction(daNpc_Cb1_c* t, ProcFunc_l* f, void* p) { gabi::call(0x022205DC, t, f, p); }
static inline void cb1_checkLanding(daNpc_Cb1_c* t) { gabi::call(0x0221FCE4, t); }
static inline s16 cb1_getStickAngY(daNpc_Cb1_c* t) { return gabi::call<s16>(0x022220A8, t); }
static inline int cb1_calcStickPos(daNpc_Cb1_c* t, s16 a, cXyz* p) { return gabi::call<int>(0x022220FC, t, a, p); }
static inline BOOL cb1_flyCheck(daNpc_Cb1_c* t) { return gabi::call<BOOL>(0x02222234, t); }
static inline void cb1_breaking(daNpc_Cb1_c* t) { gabi::call(0x022222AC, t); }
static inline BOOL cb1_flyAction(daNpc_Cb1_c* t, BOOL a, f32 s, s16 ang, BOOL b) { return gabi::call<BOOL>(0x022222D0, t, a, s, ang, b); }
static inline BOOL cb1_sowCheck(daNpc_Cb1_c* t) { return gabi::call<BOOL>(0x02222EF4, t); }
static inline BOOL cb1_chkAttention(daNpc_Cb1_c* t, f32 d, s32 a) { return gabi::call<BOOL>(0x02221BD0, t, d, a); }
static inline u32 cb1_getMsg(daNpc_Cb1_c* t) { return gabi::call<u32>(0x02221DE4, t); }

/* pointers to member (8-byte constants in .data) */
enum : u32 {
    CB1_PMF_waitNpcAction = 0x10018BE0,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* cb1_evmng_getMyP(s32 staffId, u32 name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type);
}
/* dComIfGp_event_reset(): play + 0x52B8 |= 8 */
static inline void cb1_event_reset() {
    u32 p = dComIfGp_ea();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
}
/* dComIfGs_isEventBit / onEventBit: save + 0x644 */
static inline BOOL cb1_isEventBit(u16 f) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
static inline void cb1_onEventBit(u16 f) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
/* fopAcM_onDraw(a): fopDwTg_ToDrawQ(&a->draw_tag, fpcM_DrawPriority(a)) */
static inline void cb1_onDraw(fopAc_ac_c* a) {
    s32 prio = gabi::call<s32>(0x025DF2B8, a);
    gabi::call(0x025DA874, gabi::ea(a) + 0xDC, prio);
}
/* dComIfGp_event_getTalkPartner(): dEvt_control_c::convPId(play + 0x51D0, *(play + 0x529C)) */
static inline fopAc_ac_c* cb1_getTalkPartner() {
    u32 p = dComIfGp_ea();
    return gabi::call<fopAc_ac_c*>(0x0253EE04, p + 0x51D0, gabi::load<u32>(p + 0x529C));
}
/* dEvent_manager_c::getGoal (play + 0x52C4) */
static inline cXyz* cb1_evmng_getGoal() { return gabi::call<cXyz*>(0x02544900, dComIfGp_ea() + 0x52C4); }
/* cLib_offsetPos(out, pos, angle, offset) */
static inline void cb1_offsetPos(cXyz* out, cXyz* pos, s16 ang, const cXyz* ofs) { gabi::call(0x0200F9D4, out, pos, ang, ofs); }
static inline BOOL cb1_chaseS(be<s16>* v, s16 target, s16 step) { return gabi::call<BOOL>(0x0200F564, v, target, step); }
/* cLib_calcTimer<s16>: the whole r3 is tested */
static inline s32 cb1_calcTimerS16(be<s16>* t) { return gabi::call<s32>(0x02055B64, t); }
/* daPy_py_c::setPlayerPosAndAngle (virtual, HD vtable slot 0x114) */
static inline void cb1_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 ang) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x114);
    gabi::call_ptr(fn, pl, pos, ang);
}
/* daPy_py_c::getTactMusic (virtual, HD vtable slot 0x2C) */
static inline s32 cb1_getTactMusic(fopAc_ac_c* pl) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x2C);
    return gabi::call_ptr<s32>(fn, pl);
}
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline fopAc_ac_c* cb1_getLink() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); }
/* inline strcmp (GHS) */
static inline int cb1_strcmp(u32 a, u32 b) {
    for (;;) {
        u8 x = gabi::load<u8>(a++);
        u8 y = gabi::load<u8>(b++);
        if (x != y || x == 0) return (int)x - (int)y;
    }
}
/* cM_deg2s(deg): fctiwz(deg * 182.04445) */
static inline s16 cb1_deg2s(f32 deg) { return (s16)gabi::ftoi(deg * gabi::load<f32>(0x10019018)); }
/* sqrtf of the XZ length through a local (x, 0, z) and PSVECSquareMag */
/* (f1 results are used unrounded: GHS compares and passes them as doubles) */
static inline f64 cb1_absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    f64 sq = gabi::call<f64>(0x028E8DD0, v.get());
    return gabi::call<f64>(0x028F4384, sq);
}
/* daPy_npc_c / mAcch helpers */
static inline u32 cb1_acchFlags(daNpc_Cb1_c* t) { return gabi::load<u32>(gabi::ea(t) + 0x464); }

/* ====================================================================== */
/* event actions                                                          */
/* ====================================================================== */

/* 022256D4 */
void daNpc_Cb1_c::evCheckDisp(int staffIdx) {
    WWHD_FUNC(0x022256D4, void, this, staffIdx);
    be<s32>* pDisp = (be<s32>*)cb1_evmng_getMyP(staffIdx, 0x10018FF8 /* "Disp" */, 3);
    if (pDisp) {
        if (*pDisp) {
            cb1_onDraw(this);
        } else {
            fopAcM_offDraw(this);
        }
    } else {
        cb1_onDraw(this);
    }
}
VERIFY(0x022256D4, &daNpc_Cb1_c::evCheckDisp);

/* 02225798 */
void daNpc_Cb1_c::evInitWait(int staffIdx) {
    WWHD_FUNC(0x02225798, void, this, staffIdx);
    evCheckDisp(staffIdx);
    void* pTimer = cb1_evmng_getMyP(staffIdx, 0x10019000 /* "Timer" */, 3);
    if (pTimer) {
        m8EE = gabi::load<s16>(gabi::ea(pTimer) + 2); /* (s16)*pTimer */
    } else {
        m8EE = 0;
    }
    cb1_setAnm(this, (cb1_status() & daCbStts_MUSIC) ? 8 /* ANM_08 */ : 0 /* ANM_00 */);
}
VERIFY(0x02225798, &daNpc_Cb1_c::evInitWait);

/* 02225820 */
BOOL daNpc_Cb1_c::evActWait(int) {
    WWHD_FUNC(0x02225820, BOOL, this);
    s16 temp = m8EE;
    if (cb1_chaseS(&m8EE, 0, 1)) {
        if (temp < 0) {
            cb1_event_reset();
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02225820, &daNpc_Cb1_c::evActWait);

/* 02225890 */
void daNpc_Cb1_c::evInitMsgSet(int staffIdx) {
    WWHD_FUNC(0x02225890, void, this, staffIdx);
    gabi::store<u32>(CB1_L_MSGID, 0xFFFFFFFF); /* l_msgId = fpcM_ERROR_PROCESS_ID_e */
    be<u32>* pMsgNo = (be<u32>*)cb1_evmng_getMyP(staffIdx, 0x10019008 /* "MsgNo" */, 3);
    if (pMsgNo) {
        u32 no = *pMsgNo;
        mMsgNo = no;
        if (no == 0x1520 && cb1_isEventBit(0x1840)) {
            mMsgNo = 0x1522;
        }
    }
}
VERIFY(0x02225890, &daNpc_Cb1_c::evInitMsgSet);

/* 02225928 */
BOOL daNpc_Cb1_c::evActMsgSet(int) {
    WWHD_FUNC(0x02225928, BOOL, this);
    return cb1_initTalk(this);
}
VERIFY(0x02225928, &daNpc_Cb1_c::evActMsgSet);

/* 0222592C */
BOOL daNpc_Cb1_c::evActMsgEnd(int staffIdx) {
    WWHD_FUNC(0x0222592C, BOOL, this, staffIdx);
    BOOL result = cb1_execTalk(this, 0);
    if (result) {
        cb1_evmng_getMyP(staffIdx, 0x10019010 /* "EndMode" */, 3);
        cb1_setWaitAction(this, nullptr);
    } else {
        mJntCtrl.mbTrn = 1; /* mJntCtrl.setTrn() */
        cb1_lookBack(this, 1);
    }
    return result;
}
VERIFY(0x0222592C, &daNpc_Cb1_c::evActMsgEnd);

/* 022259B8 */
void daNpc_Cb1_c::evInitMovePos(int staffIdx) {
    WWHD_FUNC(0x022259B8, void, this, staffIdx);
    void* pAngle = cb1_evmng_getMyP(staffIdx, 0x10019024 /* "Angle" */, 0);
    if (pAngle) {
        s16 angle = cb1_deg2s(gabi::load<f32>(gabi::ea(pAngle)));
        current.angle.y = angle;
        shape_angle.y = angle;
    }
    void* pPos = cb1_evmng_getMyP(staffIdx, 0x1001901C /* "Pos" */, 1);
    if (pPos) {
        /* current.pos.set(*pPos): float copies */
        u32 p = gabi::ea(pPos);
        current.pos.x = gabi::load<f32>(p);
        current.pos.y = gabi::load<f32>(p + 4);
        current.pos.z = gabi::load<f32>(p + 8);
    }
    void* pShipRide = cb1_evmng_getMyP(staffIdx, 0x10019030 /* "ShipRide" */, 4);
    if (pShipRide) {
        if (cb1_strcmp(gabi::ea(pShipRide), 0x1001902C /* "on" */) == 0) {
            cb1_setStatus(cb1_status() | daCbStts_SHIP_RIDE);
        } else if (cb1_strcmp(gabi::ea(pShipRide), 0x10019020 /* "off" */) == 0) {
            cb1_setStatus(cb1_status() & ~daCbStts_SHIP_RIDE);
        }
    }
}
VERIFY(0x022259B8, &daNpc_Cb1_c::evInitMovePos);

/* 02225B24 HD: without an "Offset" mode flag (m8E3 == 0) Link is placed at a fixed offset */
void daNpc_Cb1_c::evInitOffsetLink(int staffIdx) {
    WWHD_FUNC(0x02225B24, void, this, staffIdx);
    fopAc_ac_c* pLink = cb1_getLink();
    void* pTo = cb1_evmng_getMyP(staffIdx, 0x1001903C /* "To" */, 1);
    if (m8E3 == 0) {
        gabi::Local<cXyz> dest;
        cb1_offsetPos(dest, &current.pos, shape_angle.y, gabi::at<cXyz>(0x10019048));
        s16 a = cLib_targetAngleY(dest, &current.pos);
        cb1_setPlayerPosAndAngle(pLink, dest, a);
    } else if (pTo) {
        gabi::Local<cXyz> dest;
        cb1_offsetPos(dest, &current.pos, shape_angle.y, (cXyz*)pTo);
        s16 a = cLib_targetAngleY(dest, &current.pos);
        cb1_setPlayerPosAndAngle(pLink, dest, a);
    }
    void* pFrom = cb1_evmng_getMyP(staffIdx, 0x10019040 /* "From" */, 1);
    if (pFrom) {
        fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
        cb1_offsetPos(&current.pos, &pLink->current.pos, pPlayer->shape_angle.y, (cXyz*)pFrom);
    }
    current.angle.y = fopAcM_searchPlayerAngleY(this);
    shape_angle.y = current.angle.y;
}
VERIFY(0x02225B24, &daNpc_Cb1_c::evInitOffsetLink);

/* 02225C74 */
void daNpc_Cb1_c::evInitWalk(int) {
    WWHD_FUNC(0x02225C74, void, this);
    cb1_setAnm(this, 1 /* ANM_01 */);
    speedF = 0.0f;
}
VERIFY(0x02225C74, &daNpc_Cb1_c::evInitWalk);

/* GHS struct copy of a cXyz (integer words) */
static inline void cb1_copyXyz(u32 dst, u32 src) {
    gabi::store<u32>(dst, gabi::load<u32>(src));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}

/* 02225CB0 */
BOOL daNpc_Cb1_c::evActWalk(int staffIdx) {
    WWHD_FUNC(0x02225CB0, BOOL, this, staffIdx);
    be<f32>* speed_p = (be<f32>*)cb1_evmng_getMyP(staffIdx, 0x10019058 /* "Speed" */, 0);
    be<f32>* pDist = (be<f32>*)cb1_evmng_getMyP(staffIdx, 0x10019060 /* "Dist" */, 0);
    if (speed_p == nullptr) {
        JUT_ASSERT_fail(STR(0x10019070), 0x858, STR(0x10019090));
    }
    void* pPos = cb1_evmng_getMyP(staffIdx, 0x10019054 /* "Pos" */, 1);
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> temp2;
    if (pPos) {
        cb1_copyXyz(gabi::ea(temp.get()), gabi::ea(pPos));
    } else {
        void* pOffset = cb1_evmng_getMyP(staffIdx, 0x10019068 /* "Offset" */, 1);
        u32 play = dComIfGp_ea();
        if (pOffset) {
            fopAc_ac_c* target = gabi::call<fopAc_ac_c*>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x529C));
            if (target == nullptr) {
                JUT_ASSERT_fail(STR(0x10019070), 0x869, STR(0x10019080));
            }
            s16 a = fopAcM_searchActorAngleY(this, target);
            cb1_offsetPos(temp, &target->current.pos, a, (cXyz*)pOffset);
        } else {
            cXyz* goal = gabi::call<cXyz*>(0x02544900, play + 0x52C4);
            cb1_copyXyz(gabi::ea(temp.get()), gabi::ea(goal));
        }
    }
    cXyz_mi(temp, temp2, &current.pos);
    f32 dx = temp2->x;
    f32 dz = temp2->z;
    s16 angle = cM_atan2s(dx, dz);
    f32 walkSpeed;
    if (pDist == nullptr) {
        f64 len = cb1_absXZ(dx, dz);
        walkSpeed = *speed_p;
        if (!(len > walkSpeed)) {
            cb1_setAnm(this, 0 /* ANM_00 */);
            cb1_copyXyz(gabi::ea(&current.pos), gabi::ea(temp.get()));
            speedF = 0.0f;
            return TRUE;
        }
    } else {
        f64 len = cb1_absXZ(dx, dz);
        if (len < (f32)*pDist) {
            cb1_setAnm(this, 0 /* ANM_00 */);
            speedF = 0.0f;
            return TRUE;
        }
        walkSpeed = *speed_p;
    }
    if (cLib_distanceAngleS(shape_angle.y, angle) > 0x4000) {
        angle = (s16)(angle - 0x8000);
        walkSpeed = -walkSpeed;
    }
    if (cb1_acchFlags(this) & 0x20 /* mAcch.ChkGroundHit() */) {
        cb1_walkAction(this, walkSpeed, cb1_hioF(HIO_mForwardAccel), angle);
    } else if (m8E0) {
        speed.y = 10.0f;
    }
    return FALSE;
}
VERIFY(0x02225CB0, &daNpc_Cb1_c::evActWalk);

/* 02226078 */
void daNpc_Cb1_c::evInitToLink(int) {
    WWHD_FUNC(0x02226078, void, this);
    cb1_setAnm(this, 1 /* ANM_01 */);
    speedF = 0.0f;
    s16 a = fopAcM_searchPlayerAngleY(this);
    current.angle.y = a;
    shape_angle.y = a;
}
VERIFY(0x02226078, &daNpc_Cb1_c::evInitToLink);

/* 022260D0 */
BOOL daNpc_Cb1_c::evActToLink(int staffIdx) {
    WWHD_FUNC(0x022260D0, BOOL, this, staffIdx);
    be<f32>* speed_p = (be<f32>*)cb1_evmng_getMyP(staffIdx, 0x100190A0 /* "Speed" */, 0);
    be<f32>* dist_p = (be<f32>*)cb1_evmng_getMyP(staffIdx, 0x100190A8 /* "Dist" */, 0);
    if (speed_p == nullptr || dist_p == nullptr) {
        JUT_ASSERT_fail(STR(0x100190B0), 0x8B0, STR(0x100190C0));
    }
    f64 d = gabi::call<f64>(0x025D6958, this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ (f1 unrounded) */
    if (d < (f32)*dist_p) { /* bge: NaN walks */
        cb1_setAnm(this, 0 /* ANM_00 */);
        speedF = 0.0f;
        return TRUE;
    }
    s16 a = fopAcM_searchPlayerAngleY(this);
    f32 spd = *speed_p;
    cb1_walkAction(this, spd, cb1_hioF(HIO_mForwardAccel), a);
    return FALSE;
}
VERIFY(0x022260D0, &daNpc_Cb1_c::evActToLink);

/* 022261F4 */
u32 daNpc_Cb1_c::evInitTact(int) {
    WWHD_FUNC(0x022261F4, u32, this);
    return cb1_setAnm(this, 8 /* ANM_08 */);
}
VERIFY(0x022261F4, &daNpc_Cb1_c::evInitTact);

/* 022261FC */
BOOL daNpc_Cb1_c::evActTact(int staffIdx) {
    WWHD_FUNC(0x022261FC, BOOL, this, staffIdx);
    be<s32>* pPrm0 = (be<s32>*)cb1_evmng_getMyP(staffIdx, 0x100190E0 /* "prm0" */, 3);
    s32 prm = 0;
    if (pPrm0) {
        prm = *pPrm0;
    }
    s32 song = cb1_getTactMusic(dComIfGp_getPlayer(0));
    if (song >= 0) {
        cb1_setStatus(cb1_status() | daCbStts_TACT_CORRECT);
    }
    BOOL result = cb1_execTalk(this, 1);
    if (result) {
        u16 st = cb1_status();
        if (st & daCbStts_TACT_CORRECT) {
            cb1_setStatus(st & ~daCbStts_TACT_CORRECT);
            mMsgNo = (song == prm) ? 0x1526 : 0x1523;
        } else {
            m8DD = 3;
            if (cb1_status() & daCbStts_TACT_CANCEL) {
                mMsgNo = 0x1525;
                cb1_setStatus(cb1_status() & ~daCbStts_TACT_CANCEL);
            }
            cb1_onDraw(this);
            cb1_event_reset();
        }
    }
    return result;
}
VERIFY(0x022261FC, &daNpc_Cb1_c::evActTact);

/* 02226320 HD: the cello animation is ANM_17 (as on the non-demo GameCube) */
u32 daNpc_Cb1_c::evInitCelloPlay(int) {
    WWHD_FUNC(0x02226320, u32, this);
    return cb1_setAnm(this, 0x17 /* ANM_17 */);
}
VERIFY(0x02226320, &daNpc_Cb1_c::evInitCelloPlay);

/* 02226328 */
BOOL daNpc_Cb1_c::evActCelloPlay(int) {
    WWHD_FUNC(0x02226328, BOOL, this);
    return gabi::call<s32>(0x0244307C, cb1_getLink()) != 0 ? TRUE : FALSE; /* checkEndTactMusic */
}
VERIFY(0x02226328, &daNpc_Cb1_c::evActCelloPlay);

/* 02226358 */
void daNpc_Cb1_c::evInitTurn(int staffIdx) {
    WWHD_FUNC(0x02226358, void, this, staffIdx);
    evCheckDisp(staffIdx);
    cb1_setAnm(this, 0 /* ANM_00 */);
}
VERIFY(0x02226358, &daNpc_Cb1_c::evInitTurn);

/* 02226390 */
BOOL daNpc_Cb1_c::evActTurn(int staffIdx) {
    WWHD_FUNC(0x02226390, BOOL, this, staffIdx);
    void* pAngle = cb1_evmng_getMyP(staffIdx, 0x100190E8 /* "Angle" */, 0);
    s16 angle;
    if (pAngle) {
        angle = cb1_deg2s(gabi::load<f32>(gabi::ea(pAngle)));
    } else {
        void* pTarget = cb1_evmng_getMyP(staffIdx, 0x100190F0 /* "Target" */, 3);
        u32 play = dComIfGp_ea();
        fopAc_ac_c* target;
        if (pTarget) {
            target = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B34)); /* dComIfGp_getLinkPlayer() */
        } else {
            target = gabi::call<fopAc_ac_c*>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x529C));
        }
        if (target == nullptr) {
            JUT_ASSERT_fail(STR(0x100190F8), 0x962, STR(0x10019108));
        }
        angle = fopAcM_searchActorAngleY(this, target);
    }
    if (cLib_chaseAngleS(&shape_angle.y, angle, 0x800)) {
        shape_angle.y = angle;
        current.angle.y = angle;
        return TRUE;
    }
    current.angle.y = shape_angle.y;
    return FALSE;
}
VERIFY(0x02226390, &daNpc_Cb1_c::evActTurn);

/* 022264E4 */
void daNpc_Cb1_c::evInitSow(int staffIdx) {
    WWHD_FUNC(0x022264E4, void, this, staffIdx);
    cb1_setAnm(this, 0xC /* ANM_0C */);
    /* mNutBckAnim.getBckAnm()->getFrameMax() (virtual, slot 0x14) */
    u32 anm = gabi::load<u32>(gabi::ea(&mNutBckAnim) + 0x88);
    u32 fn = gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14);
    u32 frameMax = gabi::call_ptr<u32>(fn, anm);
    /* initPlay(frameMax, EMode_NONE, 1.0f, 0, -1, true) */
    gabi::call(0x025E72D4, &mNutBckAnim, frameMax, 0, 1.0f, 0, -1, 1);
    cb1_setStatus(cb1_status() | daCbStts_NUT);
    void* timer_p = cb1_evmng_getMyP(staffIdx, 0x10019118 /* "Timer" */, 3);
    if (timer_p == nullptr) {
        JUT_ASSERT_fail(STR(0x10019120), 0x98F, STR(0x10019130));
    }
    m8EE = (s16)gabi::load<u32>(gabi::ea(timer_p));
}
VERIFY(0x022264E4, &daNpc_Cb1_c::evInitSow);

/* 022265AC HD: a missing talk partner only asserts (no grow call through null) */
BOOL daNpc_Cb1_c::evActSow(int) {
    WWHD_FUNC(0x022265AC, BOOL, this);
    if (mNutBckAnim.play()) {
        u16 st = cb1_status();
        if (st & daCbStts_NUT) {
            cb1_setStatus(st & ~daCbStts_NUT);
            fopAc_ac_c* partner = cb1_getTalkPartner();
            if (partner == nullptr) {
                JUT_ASSERT_fail(STR(0x10019140), 0x99D, STR(0x10019150));
            } else {
                gabi::store<u8>(gabi::ea(partner) + 0x6D6, 1); /* daObjVmc::Act_c::daObjVmc_ChangeGrow() */
            }
        }
    }
    if (m8D7) {
        cb1_setAnm(this, 0 /* ANM_00 */);
    }
    return cb1_calcTimerS16(&m8EE) == 0;
}
VERIFY(0x022265AC, &daNpc_Cb1_c::evActSow);

/* 0222666C */
void daNpc_Cb1_c::evInitSetAnm(int staffIdx) {
    WWHD_FUNC(0x0222666C, void, this, staffIdx);
    be<s32>* pNumber = (be<s32>*)cb1_evmng_getMyP(staffIdx, 0x1001915C /* "Number" */, 3);
    if (pNumber) {
        u8 t = cb1_getAnmType(this, *pNumber);
        cb1_setAnm(this, t);
    }
}
VERIFY(0x0222666C, &daNpc_Cb1_c::evInitSetAnm);

/* 022266DC HD: with m8E3 == 0 the goal is a fixed offset (.data 0x1001916C) from Makar */
void daNpc_Cb1_c::evInitSetGoal(int staffIdx) {
    WWHD_FUNC(0x022266DC, void, this, staffIdx);
    void* pOffset = cb1_evmng_getMyP(staffIdx, 0x10019164 /* "Offset" */, 1);
    if (m8E3 == 0) {
        cXyz* goal = cb1_evmng_getGoal();
        cb1_offsetPos(goal, &current.pos, shape_angle.y, gabi::at<cXyz>(0x1001916C));
    } else if (pOffset) {
        cXyz* goal = cb1_evmng_getGoal();
        cb1_offsetPos(goal, &current.pos, shape_angle.y, (cXyz*)pOffset);
    }
}
VERIFY(0x022266DC, &daNpc_Cb1_c::evInitSetGoal);

/* 02226784 HD: evActSetGoal waits until Link is within 50 of the goal; after 150 frames
 * (counter m8F3) it puts Link at the goal, facing away from Makar's direction */
BOOL daNpc_Cb1_c::evActSetGoal(int staffIdx) {
    WWHD_FUNC(0x02226784, BOOL, this, staffIdx);
    evInitSetGoal(staffIdx);
    fopAc_ac_c* pl = cb1_getLink();
    cXyz* goal = cb1_evmng_getGoal();
    gabi::Local<cXyz> d;
    cXyz_mi(goal, d, &pl->current.pos);
    gabi::Local<cXyz> v;
    v->x = (f32)d->x;
    v->y = 0.0f;
    v->z = (f32)d->z;
    if (gabi::call<f64>(0x028E8DD0, v.get()) > 2500.0f) { /* PSVECSquareMag (f1 unrounded); ble: NaN returns */
        u8 n = (u8)(m8F3 + 1);
        m8F3 = n;
        if (n < 150) {
            return FALSE;
        }
        u32 vt = gabi::load<u32>(gabi::ea(pl) + 0xB4);
        cXyz* g = cb1_evmng_getGoal();
        u32 fn = gabi::load<u32>(vt + 0x114);
        gabi::call_ptr(fn, pl, g, (s16)(shape_angle.y + 0x8000)); /* setPlayerPosAndAngle */
    }
    return TRUE;
}
VERIFY(0x02226784, &daNpc_Cb1_c::evActSetGoal);

/* 02226884 */
void daNpc_Cb1_c::evInitWarp(int) {
    WWHD_FUNC(0x02226884, void, this);
    cb1_setAnm(this, 0 /* ANM_00 */);
    speed.y = cb1_REG_F(0xA5C) /* REG18_F(13) */ + 2.5f;
    m8EE = (s16)-(cb1_REG_S(0xAAA) /* REG18_S(5) */ + 0x14);
}
VERIFY(0x02226884, &daNpc_Cb1_c::evInitWarp);

/* 022268E0 */
BOOL daNpc_Cb1_c::evActWarp(int) {
    WWHD_FUNC(0x022268E0, BOOL, this);
    gravity = 0.0f;
    if (m8EE < 0) {
        if (cLib_chaseF(&speed.y, 0.0f, cb1_REG_F(0xA60) + 0.1f) && cb1_chaseS(&m8EE, 0, 1)) {
            m8EE = 0;
        }
    } else {
        if (cb1_chaseS(&m8EE, (s16)(cb1_REG_S(0xAAC) + 8000), (s16)(cb1_REG_S(0xAAE) + 400))) {
            cLib_chaseF(&speed.y, cb1_REG_F(0xA64) + 20.0f, cb1_REG_F(0xA68) + 0.5f);
            cLib_chaseF(&scale.x, 0.0f, cb1_REG_F(0xA6C) + 0.05f);
            scale.z = (f32)scale.x;
            if (cLib_chaseF(&scale.y, cb1_REG_F(0xA70) + 4.0f, cb1_REG_F(0xA74) + 0.05f)) {
                return TRUE;
            }
        }
        s16 a = (s16)(current.angle.y + m8EE);
        shape_angle.y = a;
        current.angle.y = a;
    }
    return FALSE;
}
VERIFY(0x022268E0, &daNpc_Cb1_c::evActWarp);

/* 02226A88 HD: the Otkura warp (EndMode -2 after msg 0x1526) does not reset the event */
void daNpc_Cb1_c::evInitEnd(int staffIdx) {
    WWHD_FUNC(0x02226A88, void, this, staffIdx);
    be<s32>* pEndMode = (be<s32>*)cb1_evmng_getMyP(staffIdx, 0x10019188 /* "EndMode" */, 3);
    s32 mode = pEndMode ? (s32)*pEndMode : 0;
    if (mode == -2) {
        if (mMsgNo == 0x1526) {
            gabi::call(0x0252012C, STR(0x10019190) /* "Otkura" */, 0xE6, 0, 8, 0.0f, 0, 1, 0); /* dComIfGp_setNextStage */
            cb1_onEventBit(0x1610);
            cb1_onEventBit(0x1604);
        } else {
            cb1_event_reset();
        }
        cb1_setWaitAction(this, nullptr);
    } else if (mode == -1) {
        cb1_event_reset();
        cb1_setWaitAction(this, nullptr);
    } else {
        cb1_setWaitAction(this, nullptr);
    }
}
VERIFY(0x02226A88, &daNpc_Cb1_c::evInitEnd);

/* ====================================================================== */
/* route checks                                                           */
/* ====================================================================== */

/* this TU's dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_GndChk_vt CB1_GNDCHK_VT = {0x10018D5C, 0x10018D6C, 0x10018D8C, 0x10018D7C};
static const dBgS_LinChk_vt CB1_LINCHK_VT = {0x10018D9C, 0x10018DAC, 0x10018DCC, 0x10018DBC};
struct cb1_chk54 { u8 _[0x54]; };
struct cb1_chk6C { u8 _[0x6C]; };
/* ~dBgS_LinChk (inline vtable stores, then cBgS_LinChk's destructor) */
static inline void cb1_linChk_dt(void* chk) {
    u32 b = gabi::ea(chk);
    gabi::store<u32>(b + 0x58, 0x10018DCC);
    gabi::store<u32>(b + 0x64, 0x10018D4C);
    gabi::store<u32>(b + 0x20, 0x10018D14);
    cBgS_LinChk_dt(chk, 0);
}

/* 02223BB4 */
f32 daNpc_Cb1_c::checkForwardGroundY(s16 param_1) {
    WWHD_FUNC(0x02223BB4, f32, this, param_1);
    dBgS* bgs = dComIfG_Bgsp();
    u16 bg = gabi::load<u16>(gabi::ea(&mAcchCir[0]) + 2);
    u16 poly = gabi::load<u16>(gabi::ea(&mAcchCir[0]));
    void* pla = cBgS_GetTriPla(bgs, bg, poly);
    if (pla) {
        u32 pp = gabi::ea(pla);
        s16 a = cM_atan2s(gabi::load<f32>(pp), gabi::load<f32>(pp + 8));
        if (cLib_distanceAngleS(param_1, a) > 0x4000) {
            gabi::Local<cb1_chk54> gnd_chk;
            dBgS_GndChk_ct(gnd_chk, CB1_GNDCHK_VT, false);
            u32 g = gabi::ea(gnd_chk.get());
            gabi::store<u32>(g + 0x30, gabi::load<u32>(g + 0x30) & ~2u); /* OffWall */
            f32 x = gabi::fmadds(80.0f, cM_ssin(param_1), current.pos.x);
            f32 y = current.pos.y + 80.0f;
            f32 z = gabi::fmadds(80.0f, cM_scos(param_1), current.pos.z);
            gabi::store<f32>(g + 0x24, x); /* SetPos */
            gabi::store<f32>(g + 0x28, y);
            gabi::store<f32>(g + 0x2C, z);
            f32 r = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
            /* ~dBgS_GndChk */
            gabi::store<u32>(g + 0x20, 0x10018D6C);
            gabi::store<u32>(g + 0x40, 0x10018D8C);
            gabi::store<u32>(g + 0x4C, 0x10018D4C);
            gabi::call(0x02008DAC, gnd_chk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
            return r;
        }
    }
    return -1e+7f;
}
VERIFY(0x02223BB4, &daNpc_Cb1_c::checkForwardGroundY);

/* 02223D5C */
f32 daNpc_Cb1_c::checkWallJump(s16 param_1) {
    WWHD_FUNC(0x02223D5C, f32, this, param_1);
    f64 g = gabi::call<f64>(0x02223BB4, this, param_1); /* checkForwardGroundY (f1 unrounded) */
    f32 temp = (f32)(g - (f64)(f32)current.pos.y);
    if (0.0f < temp && temp < 80.0f) {
        f64 s = gabi::call<f64>(0x028F4384, temp); /* std::sqrtf */
        return (f32)(s * (f64)3.8f);
    }
    return -1.0f;
}
VERIFY(0x02223D5C, &daNpc_Cb1_c::checkWallJump);

/* 02223DDC HD: no check of mAcchCir[1]'s wall flag (the GameCube's uninitialised case) */
BOOL daNpc_Cb1_c::chkWallHit() {
    WWHD_FUNC(0x02223DDC, BOOL, this);
    if (cb1_acchFlags(this) & 0x10 /* mAcch.ChkWallHit() */) {
        s16 a;
        if (mAcchCir[0].ChkWallHit()) {
            a = gabi::load<s16>(gabi::ea(&mAcchCir[0]) + 0x3C); /* GetWallAngleY */
        } else {
            a = gabi::load<s16>(gabi::ea(&mAcchCir[1]) + 0x3C);
        }
        return cLib_distanceAngleS(shape_angle.y, a) > 0x6000;
    }
    return FALSE;
}
VERIFY(0x02223DDC, &daNpc_Cb1_c::chkWallHit);

/* 02223E64 */
void daNpc_Cb1_c::routeAngCheck(cXyz* param_1, be<s16>* param_2) {
    WWHD_FUNC(0x02223E64, void, this, param_1, param_2);
    gabi::Local<cXyz> temp;
    gabi::call(0x0201B080, &m910, temp.get(), param_1); /* m910.outprod(param_1) */
    s16 angle = cM_atan2s(temp->x, temp->z);
    bool flip;
    if (!(m910.y < 1.0f) && cLib_distanceAngleS(angle, *param_2) > 0x4000) {
        flip = true;
    } else {
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        f32 y = current.pos.y;
        f32 dy = pl->current.pos.y - y;
        flip = temp->y * dy < 0.0f;
    }
    if (flip) {
        angle = (s16)(angle - 0x8000);
    }
    *param_2 = angle;
}
VERIFY(0x02223E64, &daNpc_Cb1_c::routeAngCheck);

/* 02223F20 */
void daNpc_Cb1_c::routeWallCheck(cXyz* param_1, cXyz* param_2, be<s16>* param_3) {
    WWHD_FUNC(0x02223F20, void, this, param_1, param_2, param_3);
    gabi::Local<cb1_chk6C> lin_chk;
    dBgS_LinChk_ct(lin_chk, CB1_LINCHK_VT, false);
    dBgS_LinChk_Set(lin_chk, param_1, param_2, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
        dBgS* bgs = dComIfG_Bgsp();
        u32 c = gabi::ea(lin_chk.get());
        u16 poly = gabi::load<u16>(c + 0x14);
        u16 bg = gabi::load<u16>(c + 0x16);
        void* pla = cBgS_GetTriPla(bgs, bg, poly);
        if (pla) {
            routeAngCheck((cXyz*)pla, param_3);
        }
    }
    cb1_linChk_dt(lin_chk);
}
VERIFY(0x02223F20, &daNpc_Cb1_c::routeWallCheck);

/* 0222403C HD: the line check is destroyed before each return */
BOOL daNpc_Cb1_c::routeCheck(f32 param_1, be<s16>* param_2) {
    WWHD_FUNC(0x0222403C, BOOL, this, param_1, param_2);
    if (!(cb1_acchFlags(this) & 0x20 /* mAcch.ChkGroundHit() */)) {
        gabi::Local<cXyz> temp;
        temp->x = (f32)current.pos.x;
        temp->z = (f32)current.pos.z;
        u32 cp = gabi::ea(&current.pos);
        u32 op = gabi::ea(&old.pos);
        gabi::store<u32>(cp + 8, gabi::load<u32>(op + 8)); /* current.pos = old.pos */
        gabi::store<u32>(cp, gabi::load<u32>(op));
        temp->y = (f32)current.pos.y;
        speedF = 0.0f;
        m8E0 = 1;
        gabi::store<u32>(cp + 4, gabi::load<u32>(op + 4));
        gabi::Local<cb1_chk6C> lin_chk;
        dBgS_LinChk_ct(lin_chk, CB1_LINCHK_VT, false);
        dBgS_LinChk_Set(lin_chk, temp, &current.pos, nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
            dBgS* bgs = dComIfG_Bgsp();
            u32 c = gabi::ea(lin_chk.get());
            u16 poly = gabi::load<u16>(c + 0x14);
            u16 bg = gabi::load<u16>(c + 0x16);
            void* pla = cBgS_GetTriPla(bgs, bg, poly);
            if (pla) {
                u32 pp = gabi::ea(pla);
                s16 a = cM_atan2s(gabi::load<f32>(pp), gabi::load<f32>(pp + 8));
                if (cLib_distanceAngleS(*param_2, a) > 0x4000) {
                    cb1_linChk_dt(lin_chk);
                    return TRUE;
                }
            }
        }
        if ((f32)mAcch.GetGroundH() - (f32)temp->y < -30.0f) {
            cb1_linChk_dt(lin_chk);
            return FALSE;
        }
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x10018C18 /* &daNpc_Cb1_c::jumpNpcAction */);
        cb1_setNpcAction(this, fn, nullptr);
        cb1_linChk_dt(lin_chk);
        return TRUE;
    }
    if (chkWallHit()) {
        f64 jump = gabi::call<f64>(0x02223D5C, this, (s16)*param_2); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp;
        *temp = (f32)jump;
        if (!(jump < 0.0f)) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, 0x10018C18 /* &daNpc_Cb1_c::jumpNpcAction */);
            cb1_setNpcAction(this, fn, temp.get());
            return TRUE;
        }
        if (param_1 > 360000.0f /* SQUARE(600.0f) */) {
            return FALSE;
        }
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        f32 y = current.pos.y;
        if (std::fabs(pl->current.pos.y - y) > 100.0f) {
            return FALSE;
        }
    }
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> temp2;
    f32 y = current.pos.y + 100.0f;
    s16 a = *param_2;
    temp->x = (f32)current.pos.x;
    temp->y = y;
    temp->z = (f32)current.pos.z;
    temp2->x = gabi::fmadds(80.0f, cM_ssin(a), current.pos.x);
    temp2->y = y;
    temp2->z = gabi::fmadds(80.0f, cM_scos(a), current.pos.z);
    routeWallCheck(temp, temp2, param_2);
    return TRUE;
}
VERIFY(0x0222403C, &daNpc_Cb1_c::routeCheck);

/* ====================================================================== */
/* NPC actions                                                            */
/* ====================================================================== */

static inline u32 cb1_attnFlagsEA(daNpc_Cb1_c* t) { return gabi::ea(t) + 0x39C; } /* attention_info.flags */
static inline void cb1_onAttnBit(daNpc_Cb1_c* t, u32 b) { gabi::store<u32>(cb1_attnFlagsEA(t), gabi::load<u32>(cb1_attnFlagsEA(t)) | b); }
static inline void cb1_offAttnBit(daNpc_Cb1_c* t, u32 b) { gabi::store<u32>(cb1_attnFlagsEA(t), gabi::load<u32>(cb1_attnFlagsEA(t)) & ~b); }
/* daPy_npc_c NPC call command (+0x3BC bit 1) */
static inline u32 cb1_npcCallEA(daNpc_Cb1_c* t) { return gabi::ea(t) + 0x3BC; }
static inline void cb1_setNpcActionPmf(daNpc_Cb1_c* t, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    cb1_setNpcAction(t, fn, arg);
}
static inline void cb1_setPlayerActionPmf(daNpc_Cb1_c* t, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    cb1_setPlayerAction(t, fn, arg);
}
enum : u32 {
    CB1_PMF_searchNpcAction = 0x10018BE8,
    CB1_PMF_jumpNpcAction = 0x10018C18,
    CB1_PMF_doorNpcAction = 0x10018C20, /* HD-only 0222487C */
    CB1_PMF_hitNpcAction = 0x10018C28,
    CB1_PMF_walkPlayerAction = 0x10018C30,
    CB1_PMF_jumpPlayerAction = 0x10018C38,
};

/* 022244A0 HD: the call command is tested first; a door command from the player switches to the
 * HD door action (0222487C) */
BOOL daNpc_Cb1_c::searchNpcAction(void*) {
    WWHD_FUNC(0x022244A0, BOOL, this);
    if (m8F0 == 0) {
        cb1_onAttnBit(this, 0x10 /* fopAc_Attn_ACTION_CARRY_e */);
        cb1_setAnm(this, 1 /* ANM_01 */);
        return TRUE;
    }
    if (m8F0 == -1) {
        return TRUE;
    }
    gabi::Local<be<s16>> angle;
    f32 temp2 = 0.0f;
    if (!(gabi::load<u32>(cb1_npcCallEA(this)) & 2) /* !checkNpcCallCommand() */) {
        if (speedF == 0.0f) {
            cb1_setNpcActionPmf(this, CB1_PMF_waitNpcAction, nullptr);
            return TRUE;
        }
        *angle = current.angle.y;
    } else {
        fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
        if (gabi::load<u16>(gabi::ea(pPlayer) + 0xF8) == 3 /* eventInfo.checkCommandDoor() */) {
            cb1_setNpcActionPmf(this, CB1_PMF_doorNpcAction, nullptr);
            return TRUE;
        }
        if (m8E1 != 0) {
            s16 q = m8E1 < 0 ? -0x4000 : 0x4000;
            *angle = (s16)(cM_atan2s(m91C.x, m91C.z) + q);
        } else {
            mHasAttention = 1;
            f64 dist_sq = gabi::call<f64>(0x025D6924, this, dComIfGp_getPlayer(0));    /* fopAcM_searchPlayerDistance2 */
            f64 dist_xz_sq = gabi::call<f64>(0x025D69AC, this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ2 */
            f32 chase = cb1_hioF(HIO_mPlayerChaseDistance);
            if (dist_sq < (f32)(chase * chase)) {
                temp2 = 0.0f;
            } else {
                f64 s = gabi::call<f64>(0x028F4384, dist_xz_sq);
                f32 v = (f32)(s * (f64)cb1_hioF(HIO_mChaseDistScale));
                f32 mx = cb1_hioF(HIO_mMaxWalkSpeed);
                temp2 = (v - mx >= 0.0f) ? mx : v; /* fsel: cLib_maxLimit */
            }
            *angle = fopAcM_searchPlayerAngleY(this);
            BOOL stop = !gabi::call<BOOL>(0x0222403C, this, dist_xz_sq, angle.get()); /* routeCheck */
            if (!stop) {
                u32 st0 = gabi::load<u32>(dComIfGp_ea() + 0x5CD8);
                stop = (st0 & 0x02000101) != 0 /* daPyStts0_UNK2000000_e | HANG | UNK1 */ ||
                       (gabi::load<u32>(gabi::ea(pPlayer) + 0x3C0) & 0x10000) != 0 /* checkAttentionLock() */;
            }
            if (stop) {
                if (speedF == 0.0f) {
                    gabi::store<u32>(cb1_npcCallEA(this), gabi::load<u32>(cb1_npcCallEA(this)) & ~2u); /* offNpcCallCommand */
                    cb1_setNpcActionPmf(this, CB1_PMF_waitNpcAction, this);
                    return TRUE;
                }
                *angle = current.angle.y;
                temp2 = 0.0f;
            }
            if (dist_xz_sq < 160000.0f /* SQUARE(400.0f) */ && cLib_distanceAngleS(shape_angle.y, *angle) < 0x2000) {
                fopAc_ac_c* pl = dComIfGp_getPlayer(0);
                f32 y = current.pos.y;
                if (std::fabs(pl->current.pos.y - y) < 100.0f) {
                    cb1_setStatus(cb1_status() | daCbStts_PLAYER_FIND);
                }
            }
            if (gabi::call<s32>(0x0207A9A0, &m8DF) == 0) { /* cLib_calcTimer<u8> */
                m8DE = m8DE ^ 1;
                m8DF = (u8)gabi::call<s32>(0x022270A0, cb1_hioS(HIO_field_0xE2), cb1_hioS(HIO_field_0xE4)); /* cLib_getRndValue<s16> */
            }
            u8 side = m8DE;
            s16 d = cb1_hioS(HIO_field_0xE6);
            if (side != 0) {
                d = -d;
            }
            *angle = (s16)(*angle + d);
        }
    }
    if (gabi::call<BOOL>(0x02516464, &mCyl)) { /* mCyl.ChkCoHit() */
        cb1_setNpcActionPmf(this, CB1_PMF_hitNpcAction, nullptr);
    } else {
        cb1_walkAction(this, temp2, cb1_hioF(HIO_mForwardAccel), *angle);
    }
    return TRUE;
}
VERIFY(0x022244A0, &daNpc_Cb1_c::searchNpcAction);

/* 0222487C HD-only NPC action (entered from searchNpcAction on a door command): Makar is put at a
 * fixed offset (-100, 0, -150) behind the player, in the current room, and waits */
static BOOL daNpc_Cb1_doorNpcAction(daNpc_Cb1_c* i_this, void*) {
    WWHD_FUNC(0x0222487C, BOOL, i_this);
    if (i_this->m8F0 == 0) {
        i_this->speedF = 0.0f;
        return TRUE;
    }
    if (i_this->m8F0 == -1) {
        return TRUE;
    }
    s8 room = gabi::load<s8>(0x1047E6C8); /* dStage_roomControl_c::mStayNo */
    i_this->current.roomNo = room;
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) {
        return TRUE;
    }
    /* function-local static cXyz offset (guard 0x10466B94, value 0x10466B78) */
    if (gabi::load<u32>(0x10466B94) == 0) {
        gabi::store<u32>(0x10466B94, 1);
        gabi::store<f32>(0x10466B78, -100.0f);
        gabi::store<f32>(0x10466B7C, 0.0f);
        gabi::store<f32>(0x10466B80, -150.0f);
    }
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    s16 a = pl->shape_angle.y;
    i_this->shape_angle.y = a;
    gabi::call(0x0200FA40, &i_this->current.pos, &pl->current.pos, a, gabi::at<cXyz>(0x10466B78)); /* cLib_offsetPos */
    u32 t = gabi::ea(i_this);
    u32 z = gabi::load<u32>(t + 0x31C);
    u32 x = gabi::load<u32>(t + 0x314);
    gabi::store<u32>(t + 0x308, z); /* old.pos = home.pos = current.pos */
    gabi::store<u32>(t + 0x2EC, x);
    gabi::store<u32>(t + 0x300, x);
    u32 y = gabi::load<u32>(t + 0x318);
    i_this->home.roomNo = room;
    gabi::store<u32>(t + 0x304, y);
    gabi::store<u32>(t + 0x2F4, z);
    gabi::store<u32>(t + 0x2F0, y);
    cb1_setNpcActionPmf(i_this, CB1_PMF_waitNpcAction, nullptr);
    return TRUE;
}
VERIFY(0x0222487C, daNpc_Cb1_doorNpcAction);

enum : u32 {
    CB1_PMF_waitPlayerAction = 0x10018BF8,
};
/* CPad_GET_STICK_VALUE(0): f1 used unrounded */
static inline f64 cb1_stickValue() { return gabi::call<f64>(0x020079B4, 0); }
/* mDoAud_monsSeStart(id, pos, actorId (base.mBsPcId at +4), param, reverb) */
static inline void cb1_monsSeStart(daNpc_Cb1_c* t, u32 id) {
    s32 reverb = dComIfGp_getReverb(t->current.roomNo);
    u32 pid = gabi::load<u32>(gabi::ea(t) + 4);
    gabi::call(0x025E1AA4, id, &t->eyePos, pid, 0, reverb);
}
/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void cb1_lookAtTarget(daNpc_Cb1_c* t, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel) {
    gabi::call(0x0259DED0, &t->mJntCtrl, outY, target, eye, yrot, vel, 0);
}
/* the hit reaction shared by hitNpcAction and hitPlayerAction */
static inline void cb1_hitStart(daNpc_Cb1_c* t) {
    f32 lim = gabi::fmuls_ppc(cb1_hioF(HIO_mMaxWalkSpeed), 0.5f);
    f32 spd = t->speedF;
    f32 s = (gabi::fsubs_ppc(spd, lim) >= 0.0f) ? lim : spd; /* fsel: cLib_maxLimit */
    f32 a = std::fabs(s);
    t->speedF = s;
    t->speed.y = gabi::fmuls_ppc(a, cb1_hioF(HIO_mHitSpeedScaleY));
    t->speedF = gabi::fmuls_ppc(s, cb1_hioF(HIO_mHitSpeedScaleF));
    s32 reverb = dComIfGp_getReverb(t->current.roomNo);
    mDoAud_seStart(0x5945 /* JA_SE_CM_CB_BOUND */, &t->eyePos, 0, reverb);
    cb1_setAnm(t, 2 /* ANM_02 */);
}

/* 022249C0 */
BOOL daNpc_Cb1_c::hitNpcAction(void*) {
    WWHD_FUNC(0x022249C0, BOOL, this);
    if (m8F0 == 0) {
        cb1_offAttnBit(this, 0x10 /* fopAc_Attn_ACTION_CARRY_e */);
        cb1_hitStart(this);
    } else if (m8F0 != -1 && (cb1_acchFlags(this) & 0x20)) {
        speedF = 0.0f;
        cb1_setNpcActionPmf(this, CB1_PMF_waitNpcAction, nullptr);
    }
    return TRUE;
}
VERIFY(0x022249C0, &daNpc_Cb1_c::hitNpcAction);

/* 02224AC0 */
BOOL daNpc_Cb1_c::jumpNpcAction(void* param_1) {
    WWHD_FUNC(0x02224AC0, BOOL, this, param_1);
    if (m8F0 == 0) {
        if (param_1) {
            f32 v = gabi::load<f32>(gabi::ea(param_1));
            m900 = 0.0f;
            speed.y = v;
        } else {
            m900 = 0.0f;
            speed.y = 10.0f;
        }
        speedF = 4.0f;
        cb1_setAnm(this, 0x16 /* ANM_16 */);
    } else if (m8F0 != -1 && (cb1_acchFlags(this) & 0x20)) {
        cb1_checkLanding(this);
        speedF = 0.0f;
        cb1_setNpcActionPmf(this, CB1_PMF_waitNpcAction, nullptr);
    }
    return TRUE;
}
VERIFY(0x02224AC0, &daNpc_Cb1_c::jumpNpcAction);

/* 02224BC0 */
BOOL daNpc_Cb1_c::rescueNpcAction(void*) {
    WWHD_FUNC(0x02224BC0, BOOL, this);
    if (m8F0 != -1) {
        if (m8F0 == 0) {
            cb1_setAnm(this, 0 /* ANM_00 */);
            fopAcM_offDraw(this);
        }
        m8DD = 0;
    }
    return TRUE;
}
VERIFY(0x02224BC0, &daNpc_Cb1_c::rescueNpcAction);

/* 02224C20 */
BOOL daNpc_Cb1_c::musicNpcAction(void*) {
    WWHD_FUNC(0x02224C20, BOOL, this);
    if (m8F0 == 0) {
        cb1_setAnm(this, 9 /* ANM_09 */);
        return TRUE;
    }
    if (m8F0 != -1 && fopAcM_GetParam(this) != 5 /* !isTypeKazeBoss() */) {
        cLib_addCalcAngleS(&shape_angle.y, home.angle.y, 8, 0x2000, 0x400);
        current.angle.y = shape_angle.y;
        BOOL att = cb1_chkAttention(this, cb1_hioF(0x28) /* l_HIO.mNpc.mMaxAttnDistXZ */, 0x10000);
        mHasAttention = (u8)att;
        if (att) {
            m8DD = fopAcM_GetParam(this) == 2 /* isTypeWaterFall() */ ? 7 : 6;
            cb1_onAttnBit(this, 0xA /* ACTION_SPEAK | LOCKON_TALK */);
        } else {
            cb1_offAttnBit(this, 0xA);
        }
    }
    return TRUE;
}
VERIFY(0x02224C20, &daNpc_Cb1_c::musicNpcAction);

/* 02224CFC */
BOOL daNpc_Cb1_c::shipNpcAction(void*) {
    WWHD_FUNC(0x02224CFC, BOOL, this);
    if (m8F0 == 0) {
        gabi::store<u32>(gabi::ea(this) + 0x3BC, gabi::load<u32>(gabi::ea(this) + 0x3BC) | 0x40); /* onNpcNotChange() */
        cb1_setAnm(this, 0xD /* ANM_0D */);
        speedF = 0.0f;
        m8F0 = (s8)(m8F0 + 1);
        return TRUE;
    }
    if (m8F0 != -1) {
        if (!cb1_isEventBit(0x1604)) {
            cb1_setNpcActionPmf(this, CB1_PMF_waitNpcAction, nullptr);
            cb1_setStatus(cb1_status() & ~daCbStts_SHIP_RIDE);
            return TRUE;
        }
        BOOL att = cb1_chkAttention(this, 500.0f, cb1_hioS(0x24) /* l_HIO.mNpc.mMaxAttnAngleY */);
        mHasAttention = (u8)att;
        if (att && !(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) /* !checkPlayerStatus0(SHIP_RIDE) */ && cb1_getMsg(this)) {
            m8DD = 6;
            cb1_onAttnBit(this, 0xA);
        } else {
            cb1_offAttnBit(this, 0xA);
        }
    }
    return TRUE;
}
VERIFY(0x02224CFC, &daNpc_Cb1_c::shipNpcAction);

/* ====================================================================== */
/* player actions                                                         */
/* ====================================================================== */

/* local copy of current.pos for lookAtTarget's by-value cXyz */
static inline void cb1_posCopy(daNpc_Cb1_c* t, cXyz* d) {
    d->x = (f32)t->current.pos.x;
    d->y = (f32)t->current.pos.y;
    d->z = (f32)t->current.pos.z;
}

/* 02224E38 HD: the attention test also accepts flag 0x20000000 (dAttention_c::Lockon) */
BOOL daNpc_Cb1_c::waitPlayerAction(void*) {
    WWHD_FUNC(0x02224E38, BOOL, this);
    if (m8F0 == 0) {
        cb1_setAnm(this, 0 /* ANM_00 */);
        cb1_onAttnBit(this, 0xFFFFFFFF);
        return TRUE;
    }
    if (m8F0 == -1 || cb1_sowCheck(this)) {
        return TRUE;
    }
    u32 att = dComIfGp_ea() + 0x5804;
    bool active = !(cb1_stickValue() < cb1_hioF(HIO_field_0x80));
    if (!active) {
        active = gabi::call<bool>(0x024EDFCC, att) /* LockonTruth */ || (gabi::load<u32>(att + 0x20) & 0x20000000);
    }
    if (active) {
        s16 target = cb1_getStickAngY(this);
        cLib_addCalcAngleS(&current.angle.y, target, 0x19, 0x7FFF, 1);
        gabi::Local<cXyz> temp;
        int stickPos = cb1_calcStickPos(this, target, temp);
        gabi::Local<be<s16>> temp3;
        gabi::Local<cXyz> eye;
        if (stickPos != 0 && cb1_stickValue() < cb1_hioF(HIO_field_0x84)) {
            cb1_posCopy(this, eye);
            s16 sy = shape_angle.y;
            s16 vel = cb1_hioS(0x1C) /* l_HIO.mNpc.mMaxTurnStep */;
            *temp3 = sy;
            cb1_lookAtTarget(this, temp3, temp, eye, sy, vel);
        } else {
            s16 cy = current.angle.y;
            cb1_posCopy(this, eye);
            shape_angle.y = cy;
            s16 vel = cb1_hioS(0x1C);
            *temp3 = cy;
            cb1_lookAtTarget(this, temp3, temp, eye, cy, vel);
        }
        if (stickPos > 0) {
            s16 a = *temp3;
            current.angle.y = a;
            shape_angle.y = a;
        } else {
            current.angle.y = shape_angle.y;
        }
        if (!(cb1_stickValue() < cb1_hioF(HIO_field_0x84)) && stickPos == 0) {
            current.angle.y = target;
            cb1_setPlayerActionPmf(this, CB1_PMF_walkPlayerAction, nullptr);
        }
    } else {
        s16 sy = shape_angle.y;
        gabi::Local<cXyz> zero;
        zero->x = gabi::load<f32>(0x101FFBA8); /* cXyz::Zero */
        zero->y = gabi::load<f32>(0x101FFBAC);
        zero->z = gabi::load<f32>(0x101FFBB0);
        cb1_lookAtTarget(this, &shape_angle.y, nullptr, zero, sy, 0);
        current.angle.y = shape_angle.y;
    }
    cb1_breaking(this);
    return TRUE;
}
VERIFY(0x02224E38, &daNpc_Cb1_c::waitPlayerAction);

/* 022250AC */
BOOL daNpc_Cb1_c::walkPlayerAction(void*) {
    WWHD_FUNC(0x022250AC, BOOL, this);
    if (m8F0 == 0) {
        cb1_setAnm(this, 1 /* ANM_01 */);
        cb1_onAttnBit(this, 0xFFFFFFFF);
        return TRUE;
    }
    if (m8F0 == -1 || cb1_sowCheck(this)) {
        return TRUE;
    }
    f64 st = cb1_stickValue();
    f32 temp = (f32)(st * (f64)cb1_hioF(HIO_mStickWalkSpeedScale));
    s16 temp7 = cb1_getStickAngY(this);
    s32 temp2 = cLib_distanceAngleS(temp7, current.angle.y);
    f32 spd = speedF;
    f32 accel = cb1_hioF(HIO_mForwardAccel);
    if (temp > spd) {
        f32 mn = cb1_hioF(HIO_mMinWalkSpeed);
        if (temp < mn) {
            temp = mn;
        }
    } else {
        accel = cb1_hioF(HIO_mDecel);
    }
    if (temp2 > 0x6000) {
        if (spd != 0.0f) {
            temp7 = current.angle.y;
        }
        temp = 0.0f;
        accel = 2.0f;
    }
    cLib_addCalcAngleS(&current.angle.y, temp7, cb1_hioS(HIO_field_0xEC), cb1_hioS(HIO_field_0xE8), cb1_hioS(HIO_field_0xEA));
    gabi::Local<cXyz> temp4;
    int stickPos = cb1_calcStickPos(this, temp7, temp4);
    if (stickPos == 0) {
        cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x2000, 0x400);
    }
    gabi::Local<cXyz> eye;
    cb1_posCopy(this, eye);
    gabi::Local<be<s16>> temp3;
    s16 sy = shape_angle.y;
    *temp3 = sy;
    cb1_lookAtTarget(this, temp3, temp4, eye, sy, cb1_hioS(0x1C) /* l_HIO.mNpc.mMaxTurnStep */);
    if (stickPos > 0) {
        shape_angle.y = (s16)*temp3;
    }
    f32 mx = cb1_hioF(HIO_mMaxWalkSpeed);
    temp = (temp - mx >= 0.0f) ? mx : temp; /* fsel: cLib_maxLimit */
    if (cLib_chaseF(&speedF, temp, accel) && temp == 0.0f) {
        cb1_setPlayerActionPmf(this, CB1_PMF_waitPlayerAction, nullptr);
        return TRUE;
    }
    f32 a = gabi::fmuls_ppc(speedF, cb1_hioF(HIO_mWalkAnmSpeedScale));
    f32 mn = cb1_hioF(HIO_mMaxWalkAnmSpeed);
    f32 rate = (a - mn >= 0.0f) ? a : mn; /* fsel: cLib_minLimit */
    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, rate); /* mpMorf->setPlaySpeed() */
    if (!(cb1_acchFlags(this) & 0x20)) {
        cb1_setPlayerActionPmf(this, CB1_PMF_jumpPlayerAction, nullptr);
        return TRUE;
    }
    if (chkWallHit()) {
        f64 jump = gabi::call<f64>(0x02223D5C, this, (s16)current.angle.y); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp5;
        *temp5 = (f32)jump;
        if (!(jump < 0.0f)) {
            cb1_setPlayerActionPmf(this, CB1_PMF_jumpPlayerAction, temp5.get());
        }
    }
    return TRUE;
}
VERIFY(0x022250AC, &daNpc_Cb1_c::walkPlayerAction);

/* 02225388 */
BOOL daNpc_Cb1_c::hitPlayerAction(void*) {
    WWHD_FUNC(0x02225388, BOOL, this);
    if (m8F0 == 0) {
        cb1_hitStart(this);
    } else if (m8F0 != -1 && (cb1_acchFlags(this) & 0x20)) {
        m4E4 = m4E4 | 1; /* returnLink() */
        speedF = 0.0f;
    }
    return TRUE;
}
VERIFY(0x02225388, &daNpc_Cb1_c::hitPlayerAction);

/* 02225464 */
BOOL daNpc_Cb1_c::jumpPlayerAction(void* param_1) {
    WWHD_FUNC(0x02225464, BOOL, this, param_1);
    if (m8F0 == 0) {
        if (param_1) {
            f32 v = gabi::load<f32>(gabi::ea(param_1));
            m900 = 0.0f;
            speed.y = v;
        } else {
            m900 = 0.0f;
            speed.y = 10.0f;
        }
        cb1_setAnm(this, 0x16 /* ANM_16 */);
    } else if (m8F0 != -1 && !cb1_flyCheck(this) && (cb1_acchFlags(this) & 0x20)) {
        cb1_checkLanding(this);
        speedF = 0.0f;
        cb1_setPlayerActionPmf(this, CB1_PMF_waitPlayerAction, nullptr);
    }
    return TRUE;
}
VERIFY(0x02225464, &daNpc_Cb1_c::jumpPlayerAction);

/* 0222555C */
BOOL daNpc_Cb1_c::flyPlayerAction(void*) {
    WWHD_FUNC(0x0222555C, BOOL, this);
    if (m8F0 == 0) {
        speed.y = cb1_hioF(HIO_mFlyLaunchSpeedY);
        cb1_setAnm(this, 4 /* ANM_04 */);
        gabi::store<s16>(0x101BDC28, cb1_hioS(HIO_mPlayerFlyTimer)); /* setFlyingTimer() */
        cb1_monsSeStart(this, 0x48C0 /* JA_SE_CV_CB_LEAF_OUT */);
    } else if (m8F0 != -1) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 6); /* dComIfGp_setAStatus(dActStts_LET_GO_e) */
        BOOL trigA = gabi::call<BOOL>(0x02007898, 0); /* CPad_CHECK_TRIG_A(0) */
        f32 spd = (f32)(cb1_stickValue() * (f64)cb1_hioF(HIO_mStickFlySpeedScale));
        s16 ang = cb1_getStickAngY(this);
        BOOL trigB = gabi::call<BOOL>(0x020078BC, 0); /* CPad_CHECK_TRIG_B(0) */
        cb1_flyAction(this, trigA, spd, ang, trigB);
    }
    return TRUE;
}
VERIFY(0x0222555C, &daNpc_Cb1_c::flyPlayerAction);

/* 0222566C */
BOOL daNpc_Cb1_c::carryPlayerAction(void*) {
    WWHD_FUNC(0x0222566C, BOOL, this);
    if (m8F0 == 0) {
        cb1_setAnm(this, 0xB /* ANM_0B */);
        cb1_monsSeStart(this, 0x48BE /* JA_SE_CV_CB_DAMAGE_S */);
    }
    return TRUE;
}
VERIFY(0x0222566C, &daNpc_Cb1_c::carryPlayerAction);
