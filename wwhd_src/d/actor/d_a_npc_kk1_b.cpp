/**
 * d_a_npc_kk1_b.cpp (WWHD)
 * NPC - Mila (poor, Windfall)
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 * Part B: event cuts (cut_init_* / cut_move_*), event_move, privateCut, event_proc, kyoroPos,
 * kyorokyoro, lookBack, eventOrder, _execute, _draw.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_kk1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline bool dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* p) { return gabi::call<bool>(0x0259ED58, p); }
static inline bool dNpc_PathRun_decIdxAuto(dNpc_PathRun_l* p) { return gabi::call<bool>(0x0259EC94, p); }
static inline u8 dNpc_PathRun_maxPoint(dNpc_PathRun_l* p) { return gabi::call<u8>(0x0259EDB8, p); }
static inline s8 dNpc_PathRun_pointArg(dNpc_PathRun_l* p, u8 idx) { return gabi::call<s8>(0x0259EE4C, p, idx); }
static inline bool dNpc_PathRun_setNearPathIndx(dNpc_PathRun_l* p, cXyz* pos, f32 r) { return gabi::call<bool>(0x0259EE7C, p, pos, r); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_l* p, cXyz* pos, bool dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
/* dEvt_control_c (play + 0x51D0) */
static inline u32 dComIfGp_evtCtrl() { return dComIfGp_ea() + 0x51D0; }
static inline u32 dEvt_control_getPId(u32 evt, void* actor) { return gabi::call<u32>(0x0253F124, evt, actor); }
/* daPy_py_c (the link player, play + 0x5B34): mDemo at 0x420 (u16 mDemoType, +8 s32 mParam0,
 * +0x10 u32 mDemoMode), mNoResetFlg0 at 0x3B8; setPlayerPosAndAngle is vtable slot +0x114 */
static inline u32 daPy_getPlayerActorClass() { return gabi::load<u32>(dComIfGp_ea() + 0x5B34); }
static inline void daPy_onNoResetFlg0(u32 f) {
    u32 p = daPy_getPlayerActorClass();
    gabi::store<u32>(p + 0x3B8, gabi::load<u32>(p + 0x3B8) | f);
}
static inline void daPy_offNoResetFlg0(u32 f) {
    u32 p = daPy_getPlayerActorClass();
    gabi::store<u32>(p + 0x3B8, gabi::load<u32>(p + 0x3B8) & ~f);
}
static inline void daPy_setDemoType(u16 t) { gabi::store<u16>(daPy_getPlayerActorClass() + 0x420, t); }
static inline void daPy_setDemoParam0(s32 v) { gabi::store<s32>(daPy_getPlayerActorClass() + 0x428, v); }
static inline void daPy_setDemoMode(u32 m) { gabi::store<u32>(daPy_getPlayerActorClass() + 0x430, m); }
/* the inline daPy_py_c::changeDemoMode-like pairs: one player fetch for both stores */
static inline void daPy_setDemoTypeParam0(u16 t, s32 v) {
    u32 p = daPy_getPlayerActorClass();
    gabi::store<u16>(p + 0x420, t);
    gabi::store<s32>(p + 0x428, v);
}
static inline void daPy_setDemoTypeMode(u16 t, u32 m) {
    u32 p = daPy_getPlayerActorClass();
    gabi::store<u16>(p + 0x420, t);
    gabi::store<u32>(p + 0x430, m);
}
/* dComIfGp_evmng_setGoal */
static inline void dComIfGp_evmng_setGoal_l(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline cXyz* daPy_getPos() { return gabi::at<cXyz>(daPy_getPlayerActorClass() + 0x314); }
/* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16): virtual; the play object is re-read for the argument */
static inline void daPy_setPlayerPosAndAngle_ang(s16 ang) {
    u32 p = daPy_getPlayerActorClass();
    u32 vt = gabi::load<u32>(p + 0xB4);
    cXyz* pos = gabi::at<cXyz>(daPy_getPlayerActorClass() + 0x314);
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), p, pos, ang);
}

/* ---- other parts of d_a_npc_kk1 (called by address) ---- */
/* 0226CC2C searchByID(fpc_ProcID, int* o_notFound) (the matcher calls it cLib_getRndValue<i>) */
static inline fopAc_ac_c* kk1_searchByID(daNpc_Kk1_c* i_this, u32 id, be<s32>* res) {
    return gabi::call<fopAc_ac_c*>(0x0226CC2C, i_this, id, res);
}
/* 0226B5B4 (unnamed): the current path point, moved by two debug REGs on point 0x11 (hidden result pointer) */
static inline void kk1_getPathPoint(cXyz* out, dNpc_PathRun_l* p) { gabi::call(0x0226B5B4, out, p); }

/* ---- file statics ---- */
/* l_HIO (0x10467698, constructed in part A): the parameters read here */
static inline s16 l_HIO_s16(u32 off) { return gabi::load<s16>(0x10467698 + off); }
static inline f32 l_HIO_f32(u32 off) { return gabi::load<f32>(0x10467698 + off); }
/* debug REGs (g_regHIO, 0x1047B608) */
static inline s16 REG_S(u32 off) { return gabi::load<s16>(0x1047B608 + off); }

/* 0226CC80 */
bool daNpc_Kk1_c::cut_init_RUN_START(int staffIdx) {
    WWHD_FUNC(0x0226CC80, bool, this, staffIdx);
    gabi::Local<be<s32>> res;
    fopAc_ac_c* a_actor = kk1_searchByID(this, m86C, res);
    if (a_actor == nullptr) /* JUT_ASSERT(1420, a_actor != NULL) */
        JUT_ASSERT_fail(STR(0x1001C4A0), 0x58C, STR(0x1001C4B0));
    u32 evt = dComIfGp_evtCtrl();
    gabi::store<u32>(evt + 0xD0, dEvt_control_getPId(evt, a_actor)); /* mPtItem */
    return dNpc_PathRun_nextIdxAuto(&mPathRun);
}
VERIFY(0x0226CC80, &daNpc_Kk1_c::cut_init_RUN_START);

/* 0226CD04 */
void daNpc_Kk1_c::cut_init_RUN(int staffIdx) {
    WWHD_FUNC(0x0226CD04, void, this, staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C4C0) /* "Timer" */);
    m908 = -1;
    if (timer != nullptr) {
        m908 = (s16)(s32)*timer;
    }
    setAnm_NUM(4, 1);
    m926 = 1;
    mAC1 = 2;
    mAC2 = 0;
}
VERIFY(0x0226CD04, &daNpc_Kk1_c::cut_init_RUN);

/* 0226CD94 */
void daNpc_Kk1_c::cut_init_CATCH_START(int staffIdx) {
    WWHD_FUNC(0x0226CD94, void, this, staffIdx);
    daPy_onNoResetFlg0(0x08000000);
    setAnm_NUM(8, 1);
    mpMorf->setMorf(0.0f);
}
VERIFY(0x0226CD94, &daNpc_Kk1_c::cut_init_CATCH_START);

/* 0226CDF0 */
void daNpc_Kk1_c::cut_init_CATCH_END(int staffIdx) {
    WWHD_FUNC(0x0226CDF0, void, this, staffIdx);
    daPy_offNoResetFlg0(0x08000000);
    current.angle.y = current.angle.y - 0x8000;
    setAnm_NUM(0, 1);
    mpMorf->setMorf(0.0f);
    m_jnt.mbTrn = 1;
    m934 = 0;
    mACA = 1;
}
VERIFY(0x0226CDF0, &daNpc_Kk1_c::cut_init_CATCH_END);

/* 0226D0A4 */
void daNpc_Kk1_c::cut_init_BYE_START(int staffIdx) {
    WWHD_FUNC(0x0226D0A4, void, this, staffIdx);
    daPy_onNoResetFlg0(0x08000000);
    m92B = 1;
}
VERIFY(0x0226D0A4, &daNpc_Kk1_c::cut_init_BYE_START);

/* 0226D0E8 */
void daNpc_Kk1_c::cut_init_BYE(int staffIdx) {
    WWHD_FUNC(0x0226D0E8, void, this, staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C4E0) /* "Timer" */);
    be<s32>* delay = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C4E8) /* "Delay" */);
    be<s32>* prm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C4F0) /* "prm_0" */);
    m908 = 30;
    if (timer != nullptr) {
        m908 = (s16)(s32)*timer;
    }
    m90A = -1;
    if (delay != nullptr) {
        m90A = (s16)(s32)*delay;
    }
    m920 = 0;
    if (prm0 != nullptr) {
        m920 = (s16)(s32)*prm0;
    }
    setAnm_NUM(4, 1);
    mAC1 = 2;
    m926 = 1;
}
VERIFY(0x0226D0E8, &daNpc_Kk1_c::cut_init_BYE);

/* 0226D1F0 */
void daNpc_Kk1_c::cut_init_BYE_END(int staffIdx) {
    WWHD_FUNC(0x0226D1F0, void, this, staffIdx);
    daPy_setDemoTypeParam0(3, 0);
    daPy_setDemoMode(4);
    s16 ang = cLib_targetAngleY(daPy_getPos(), &current.pos);
    daPy_setPlayerPosAndAngle_ang(ang);
    daPy_offNoResetFlg0(0x08000000);
    m930 = 1;
}
VERIFY(0x0226D1F0, &daNpc_Kk1_c::cut_init_BYE_END);

/* 0226D2B8 */
void daNpc_Kk1_c::cut_init_OTOBOKE(int staffIdx) {
    WWHD_FUNC(0x0226D2B8, void, this, staffIdx);
    daPy_setDemoTypeParam0(3, 0);
    daPy_setDemoMode(4);
    u32 p = daPy_getPlayerActorClass();
    u32 vt = gabi::load<u32>(p + 0xB4);
    cXyz* pos = gabi::at<cXyz>(daPy_getPlayerActorClass() + 0x314);
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), p, pos, (s16)current.angle.y); /* setPlayerPosAndAngle */
    m908 = 2;
}
VERIFY(0x0226D2B8, &daNpc_Kk1_c::cut_init_OTOBOKE);

/* 0226D34C */
void daNpc_Kk1_c::cut_init_PLYER_MOV(int staffIdx) {
    WWHD_FUNC(0x0226D34C, void, this, staffIdx);
    s16 diff = (s16)(cLib_targetAngleY(&current.pos, daPy_getPos()) - current.angle.y);
    s32 adiff = diff < 0 ? -diff : diff;
    if (adiff > 0x2000) {
        cXyz* pos = daPy_getPos();
        dComIfGp_evmng_setGoal_l(pos);
        return;
    }
    gabi::Local<cXyz> offs;
    offs->y = 0.0f;
    offs->x = 0.0f;
    s16 turn = -0x2800;
    offs->z = 0.0f;
    if (diff > 0) {
        turn = 0x2800;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM((s16)(current.angle.y + turn));
    offs->z = 150.0f;
    gabi::Local<cXyz> goal;
    PSMTXMultVec(mDoMtx_stack_c::get(), offs, goal);
    dComIfGp_evmng_setGoal_l(goal);
}
VERIFY(0x0226D34C, &daNpc_Kk1_c::cut_init_PLYER_MOV);

/* 0226D454 */
void daNpc_Kk1_c::cut_init_RUNAWAY_START(int staffIdx) {
    WWHD_FUNC(0x0226D454, void, this, staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C4FC) /* "Timer" */);
    daPy_setDemoTypeParam0(3, 0);
    daPy_setDemoMode(4);
    s16 ang = cLib_targetAngleY(daPy_getPos(), &current.pos);
    daPy_setPlayerPosAndAngle_ang(ang);
    s16 toPlayer = cLib_targetAngleY(&current.pos, daPy_getPos());
    m_jnt.mAngles[0][0] = 0;
    s16 away = (s16)(toPlayer - 0x8000);
    mACA = 0;
    s16 diff = (s16)(toPlayer - current.angle.y);
    m92F = 1;
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
    m908 = 0x26;
    if (timer != nullptr) {
        m908 = (s16)(s32)*timer;
    }
    current.angle.y = toPlayer;
    setAnm_NUM(0, 1);
    gabi::Local<cXyz> pos;
    pos->x = 0.0f;
    pos->z = 0.0f;
    pos->y = -50.0f;
    setBikon(pos);
}
VERIFY(0x0226D454, &daNpc_Kk1_c::cut_init_RUNAWAY_START);

/* 0226D60C */
void daNpc_Kk1_c::cut_init_RUNAWAY_END(int staffIdx) {
    WWHD_FUNC(0x0226D60C, void, this, staffIdx);
    daPy_offNoResetFlg0(0x08000000);
    m926 = 0;
    m930 = 1;
    speedF = 0.0f;
}
VERIFY(0x0226D60C, &daNpc_Kk1_c::cut_init_RUNAWAY_END);

/* 0226D664 */
void daNpc_Kk1_c::cut_init_BYE_CONTINUE(int staffIdx) {
    WWHD_FUNC(0x0226D664, void, this, staffIdx);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001C504) /* "Timer" */);
    m908 = 30;
    if (timer != nullptr) {
        m908 = (s16)(s32)*timer;
    }
}
VERIFY(0x0226D664, &daNpc_Kk1_c::cut_init_BYE_CONTINUE);

/* 0226D6CC */
bool daNpc_Kk1_c::cut_move_RUN_START() {
    WWHD_FUNC(0x0226D6CC, bool, this);
    gabi::Local<cXyz> pnt;
    kk1_getPathPoint(pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, pnt);
    cLib_addCalcAngleS(&current.angle.y, ang, l_HIO_s16(0x30), l_HIO_s16(0x32), 0x80);
    if ((u32)(s32)current.angle.y == (u32)(s32)ang) {
        daPy_setDemoTypeMode(2, 1);
        return true;
    }
    return false;
}
VERIFY(0x0226D6CC, &daNpc_Kk1_c::cut_move_RUN_START);

/* 0226D77C (unnamed): has the actor passed the current path point? (point 0x11 moved by two REGs) */
static BOOL kk1_chkPointPass(cXyz* i_pos, dNpc_PathRun_l* i_path) {
    WWHD_FUNC(0x0226D77C, BOOL, i_pos, i_path);
    f32 x = i_pos->x;
    f32 z = i_pos->z;
    f32 y = i_pos->y;
    if (i_path->mIdx == 0x11) {
        f32 r0 = (f32)REG_S(0x1164);
        f32 r1 = (f32)REG_S(0x1166);
        x = x - (r0 + 20.0f);
        z = z + (r1 + 50.0f);
    }
    gabi::Local<cXyz> pos;
    pos->x = x;
    pos->y = y;
    pos->z = z;
    return dNpc_PathRun_chkPointPass(i_path, pos, i_path->mbDir != 0);
}
VERIFY(0x0226D77C, kk1_chkPointPass);

/* 0226D83C */
bool daNpc_Kk1_c::event_move(bool i_chkArg) {
    WWHD_FUNC(0x0226D83C, bool, this, i_chkArg);
    u32 path = gabi::ea(mPathRun.mPath.get());
    if (path == 0 || !(gabi::load<u8>(path + 5) & 1)) {
        return true;
    }
    if (m926 != 0 && kk1_chkPointPass(&current.pos, &mPathRun)) {
        dNpc_PathRun_nextIdxAuto(&mPathRun);
        if (i_chkArg) {
            s8 arg = dNpc_PathRun_pointArg(&mPathRun, mPathRun.mIdx);
            if (arg >= 0) {
                arg = (s8)(arg + 1);
                if (arg == 2 || arg == 3) {
                    mAC1 = arg;
                    mAC2 = 0;
                    m926 = 1;
                    goto turn;
                }
            }
            m926 = 0;
        }
    }
turn:
    gabi::Local<cXyz> pnt;
    kk1_getPathPoint(pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, pnt);
    s16 oldY = current.angle.y;
    cLib_addCalcAngleS(&current.angle.y, ang, l_HIO_s16(0x30), l_HIO_s16(0x32), 0x80);
    f32 factor, step, target;
    if ((s8)mAC1 == 2) {
        factor = l_HIO_f32(0x4C);
        step = l_HIO_f32(0x48);
        target = m926 == 0 ? 0.0f : l_HIO_f32(0x44);
    } else {
        factor = l_HIO_f32(0x58);
        step = l_HIO_f32(0x54);
        target = m926 == 0 ? 0.0f : l_HIO_f32(0x50);
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
    s8 arg = dNpc_PathRun_pointArg(&mPathRun, mPathRun.mIdx);
    if (arg >= 0) {
        arg = (s8)(arg + 1);
    }
    mAC2 = arg;
    return true;
}
VERIFY(0x0226D83C, &daNpc_Kk1_c::event_move);

/* 0226DB64 */
s32 daNpc_Kk1_c::cut_move_RUN() {
    WWHD_FUNC(0x0226DB64, s32, this);
    s32 ret = event_move(false);
    if (m908 >= 0) {
        ret = cLib_calcTimer(&m908) == 0;
    }
    return ret;
}
VERIFY(0x0226DB64, &daNpc_Kk1_c::cut_move_RUN);

/* 0226DBB0 */
bool daNpc_Kk1_c::cut_move_CATCH_START() {
    WWHD_FUNC(0x0226DBB0, bool, this);
    if ((s8)m922 != 0) {
        m8E8 = 0;
        return true;
    }
    return false;
}
VERIFY(0x0226DBB0, &daNpc_Kk1_c::cut_move_CATCH_START);

/* 0226DBD4 */
bool daNpc_Kk1_c::cut_move_TRN() {
    WWHD_FUNC(0x0226DBD4, bool, this);
    gabi::Local<cXyz> pnt;
    kk1_getPathPoint(pnt, &mPathRun);
    s16 ang = cLib_targetAngleY(&current.pos, pnt);
    cLib_addCalcAngleS(&current.angle.y, ang, l_HIO_s16(0x30), l_HIO_s16(0x32), 0x80);
    s16 y = current.angle.y;
    if ((u32)(s32)y == (u32)(s32)ang) {
        shape_angle.y = y;
        return true;
    }
    return false;
}
VERIFY(0x0226DBD4, &daNpc_Kk1_c::cut_move_TRN);

/* 0226DC70 */
bool daNpc_Kk1_c::cut_move_BYE() {
    WWHD_FUNC(0x0226DC70, bool, this);
    event_move(false);
    if (m90A > 0 && cLib_calcTimer(&m90A) == 0) {
        daPy_setPlayerPosAndAngle_ang(-0x3217);
        daPy_setDemoTypeParam0(3, 0);
        daPy_setDemoMode(9);
        daPy_setDemoParam0(0x3217);
    }
    if (cLib_calcTimer(&m908) == 0) {
        if (m920 == 0) {
            speedF = 0.0f;
        }
        return true;
    }
    return false;
}
VERIFY(0x0226DC70, &daNpc_Kk1_c::cut_move_BYE);

/* 0226DD7C */
bool daNpc_Kk1_c::cut_move_OTOBOKE() {
    WWHD_FUNC(0x0226DD7C, bool, this);
    if (cLib_calcTimer(&m908) == 0) {
        daPy_setDemoTypeMode(2, 1);
        return true;
    }
    return false;
}
VERIFY(0x0226DD7C, &daNpc_Kk1_c::cut_move_OTOBOKE);

/* 0226DDD8 */
bool daNpc_Kk1_c::cut_move_RUNAWAY_START() {
    WWHD_FUNC(0x0226DDD8, bool, this);
    if ((s8)mAC6 != 8) {
        return true;
    }
    if ((s8)m922 == 0) {
        return false;
    }
    current.angle.y = cLib_targetAngleY(&current.pos, daPy_getPos());
    setAnm_NUM(0, 1);
    mpMorf->setMorf(0.0f);
    return true;
}
VERIFY(0x0226DDD8, &daNpc_Kk1_c::cut_move_RUNAWAY_START);

/* 0226DE88 */
bool daNpc_Kk1_c::cut_move_BYE_CONTINUE() {
    WWHD_FUNC(0x0226DE88, bool, this);
    event_move(false);
    if (cLib_calcTimer(&m908) == 0) {
        speedF = 0.0f;
        return true;
    }
    return false;
}
VERIFY(0x0226DE88, &daNpc_Kk1_c::cut_move_BYE_CONTINUE);

/* 0226E92C */
void daNpc_Kk1_c::eventOrder() {
    WWHD_FUNC(0x0226E92C, void, this);
    s8 order = (s8)mAC7;
    if (order == 1 || order == 2) {
        s8 v = (s8)mAC7;
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (v == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        s16 idx = (s16)(order - 3);
        m8FC = idx;
        fopAcM_orderOtherEventId(this, gabi::load<s16>(gabi::ea(this) + 0x8EC + idx * 2), 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x0226E92C, &daNpc_Kk1_c::eventOrder);

/* 0226CE6C */
void daNpc_Kk1_c::cut_init_TRN(int staffIdx) {
    WWHD_FUNC(0x0226CE6C, void, this, staffIdx);
    if ((s8)mAC2 != 5) {
        u8 orgIdx = mPathRun.mIdx;
        if ((u32)(orgIdx - 0x19) < 0x10) {
            /* a local table (.rodata 0x1001C4D0, copied to the stack): the turn-around point */
            u8 next = gabi::load<u8>(0x1001C4D0 + (orgIdx - 0x19));
            mACA = 0;
            m934 = 1;
            mPathRun.mIdx = next;
            return;
        }
        dNpc_PathRun_setNearPathIndx(&mPathRun, daPy_getPos(), 100.0f);
        u8 plIdx = mPathRun.mIdx;
        dNpc_PathRun_setNearPathIndx(&mPathRun, &current.pos, 100.0f);
        u8 myIdx = mPathRun.mIdx;
        s32 maxPnt = dNpc_PathRun_maxPoint(&mPathRun);
        s16 half = (s16)gabi::ftoi(gabi::fmadds((f32)maxPnt, 0.5f, 0.5f));
        if (myIdx > plIdx) {
            plIdx = (u8)(plIdx + dNpc_PathRun_maxPoint(&mPathRun));
        }
        s16 diff = (s16)(plIdx - myIdx);
        if (diff > half) {
            diff = (s16)(diff - maxPnt);
        }
        mPathRun.mIdx = orgIdx;
        if (diff >= 0) {
            bool back = true;
            if (diff <= 0) {
                gabi::Local<cXyz> pnt;
                kk1_getPathPoint(pnt, &mPathRun);
                gabi::Local<cXyz> d;
                cXyz_mi(&current.pos, d, pnt);
                gabi::Local<cXyz> xz1;
                xz1->x = d->x;
                xz1->y = 0.0f;
                xz1->z = d->z;
                f32 myDist = std_sqrtf(PSVECSquareMag(xz1));
                cXyz_mi(daPy_getPos(), d, pnt);
                gabi::Local<cXyz> xz2;
                xz2->x = d->x;
                xz2->y = 0.0f;
                xz2->z = d->z;
                back = std_sqrtf(PSVECSquareMag(xz2)) < myDist; /* mfcr: the LT bit */
            }
            if (back) {
                dNpc_PathRun_decIdxAuto(&mPathRun);
                dNpc_PathRun_decIdxAuto(&mPathRun);
                mPathRun.mbDir = mPathRun.mbDir ^ 1;
            }
        }
    }
    mACA = 0;
    m934 = 1;
}
VERIFY(0x0226CE6C, &daNpc_Kk1_c::cut_init_TRN);

/* 0226DEF0 */
void daNpc_Kk1_c::privateCut(int staffIdx) {
    WWHD_FUNC(0x0226DEF0, void, this, staffIdx);
    if (staffIdx == -1) {
        return;
    }
    /* cut_name_tbl (.data 0x101BF454): RUN_START, RUN, CATCH_START, CATCH_END, TRN, BYE_START, BYE,
     * BYE_END, PLYER_TRN, OTOBOKE, PLYER_MOV, RUNAWAY_START, RUNAWAY_END, BYE_CONTINUE */
    s8 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BF454, 14, TRUE, 0);
    mAC0 = actIdx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        gabi::call(0x02543280, evmng, staffIdx); /* cutEnd */
        return;
    }
    if (gabi::call<BOOL>(0x025447C8, evmng, staffIdx)) { /* getIsAddvance */
        switch ((u32)(s32)(s8)mAC0) {
        case 0: cut_init_RUN_START(staffIdx); break;
        case 1: cut_init_RUN(staffIdx); break;
        case 2: cut_init_CATCH_START(staffIdx); break;
        case 3: cut_init_CATCH_END(staffIdx); break;
        case 4: cut_init_TRN(staffIdx); break;
        case 5: cut_init_BYE_START(staffIdx); break;
        case 6: cut_init_BYE(staffIdx); break;
        case 7: cut_init_BYE_END(staffIdx); break;
        case 9: cut_init_OTOBOKE(staffIdx); break;
        case 10: cut_init_PLYER_MOV(staffIdx); break;
        case 11: cut_init_RUNAWAY_START(staffIdx); break;
        case 12: cut_init_RUNAWAY_END(staffIdx); break;
        case 13: cut_init_BYE_CONTINUE(staffIdx); break;
        }
    }
    bool done = true;
    switch ((u32)(s32)(s8)mAC0) {
    case 0: done = cut_move_RUN_START(); break;
    case 1: done = cut_move_RUN() != 0; break;
    case 2: done = cut_move_CATCH_START(); break;
    case 4: done = cut_move_TRN(); break;
    case 6: done = cut_move_BYE(); break;
    case 9: done = cut_move_OTOBOKE(); break;
    case 11: done = cut_move_RUNAWAY_START(); break;
    case 13: done = cut_move_BYE_CONTINUE(); break;
    }
    if (done) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}
VERIFY(0x0226DEF0, &daNpc_Kk1_c::privateCut);

/* 0226E21C */
void daNpc_Kk1_c::event_proc(int staffIdx) {
    WWHD_FUNC(0x0226E21C, void, this, staffIdx);
    s16 evId = gabi::load<s16>(gabi::ea(this) + 0x8EC + m8FC * 2);
    if (!dComIfGp_evmng_endCheck(evId)) {
        if (!mEventCut.cutProc()) {
            privateCut(staffIdx);
        }
        return;
    }
    u32 ev = (u32)(s32)m8FC;
    if (ev < 3) {
        if (ev <= 1) {
            setStt(5);
        } else {
            gabi::store<s16>(gabi::ea(this) + 0xFC, -1); /* eventInfo.mEventId */
            switch (mCurrMsgNo) {
            case 0x1C98:
            case 0x1C9C:
                mAC7 = 7;
                mACA = 1;
                m934 = 1;
                break;
            case 0x1C9A:
                mACA = 0;
                m934 = 1;
                mAC7 = 10;
                break;
            case 0x1C9F:
                setStt(6);
                mACA = 0;
                m934 = 1;
                mAC7 = 6;
                break;
            }
        }
    } else if (ev < 4) {
        mAC7 = 1;
        dComIfGs_onEventBit(0xE08);
        m92A = 1;
    } else if (ev == 4 || (ev >= 6 && ev <= 7)) {
        fopAcM_delete(this);
    }
    endEvent();
}
VERIFY(0x0226E21C, &daNpc_Kk1_c::event_proc);

/* 0226E498: returns a cXyz (hidden result pointer; GHS allocates it when NULL) */
void daNpc_Kk1_c::kyoroPos(cXyz* o_pos, int i_idx) {
    WWHD_FUNC(0x0226E498, void, this, o_pos, i_idx);
    cXyz* tbl = gabi::at<cXyz>(0x101BF48C + i_idx * 0xC); /* look-around offsets */
    gabi::Local<cXyz> offs;
    offs->z = tbl->z;
    offs->y = tbl->y;
    offs->x = tbl->x;
    mDoMtx_stack_c::transS(eyePos.x, eyePos.y, eyePos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), offs, pos);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(0xC);
        if (o_pos == nullptr) {
            return;
        }
    }
    o_pos->x = pos->x;
    o_pos->y = pos->y;
    o_pos->z = pos->z;
}
VERIFY(0x0226E498, &daNpc_Kk1_c::kyoroPos);

/* 0226E560 */
bool daNpc_Kk1_c::kyorokyoro() {
    WWHD_FUNC(0x0226E560, bool, this);
    if (cLib_calcTimer(&m904) != 0) {
        gabi::Local<cXyz> pos;
        kyoroPos(pos, m906);
        u32 dst = gabi::ea(&m8A8);
        gabi::store<u32>(dst + 8, gabi::load<u32>(gabi::ea(pos.get()) + 8));
        gabi::store<u32>(dst + 4, gabi::load<u32>(gabi::ea(pos.get()) + 4));
        gabi::store<u32>(dst + 0, gabi::load<u32>(gabi::ea(pos.get()) + 0));
        return true;
    }
    m906 = (s16)cLib_getRndValue(1, 10);
    m904 = l_HIO_s16(0x28);
    m902 = l_HIO_s16(0x2A);
    return false;
}
VERIFY(0x0226E560, &daNpc_Kk1_c::kyorokyoro);

/* integer word copy of a cXyz (GHS struct copy) */
static inline void icopy_xyz(u32 dst, u32 src) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
}
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}

/* 0226E604 */
void daNpc_Kk1_c::lookBack() {
    WWHD_FUNC(0x0226E604, void, this);
    m8E0.y = m_jnt.mAngles[0][1];
    s16 yrot = current.angle.y;
    f32 srcX = current.pos.x;
    m8E0.x = yrot;
    gabi::Local<cXyz> dst;
    dst->x = 0.0f;
    u8 headOnly = m934;
    f32 srcZ = current.pos.z;
    dst->y = 0.0f;
    f32 srcY = eyePos.y;
    cXyz* target = nullptr;
    dst->z = 0.0f;
    m8E0.z = m_jnt.mAngles[1][1];
    switch ((u32)(s32)(s8)mACA) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        icopy_xyz(gabi::ea(dst.get()), gabi::ea(eye.get()));
        icopy_xyz(gabi::ea(&m8A8), gabi::ea(eye.get()));
        target = dst;
        break;
    }
    case 2:
        icopy_xyz(gabi::ea(dst.get()), gabi::ea(&m8A8));
        target = dst;
        break;
    case 3:
        yrot = m91C;
        break;
    case 4: {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* actor = kk1_searchByID(this, m870, notFound);
        if (actor != nullptr && *notFound == 0) {
            icopy_xyz(gabi::ea(&m8A8), gabi::ea(&actor->current.pos));
            f32 x = m8A8.x;
            f32 eyeY = actor->eyePos.y;
            f32 z = m8A8.z;
            dst->y = eyeY;
            dst->z = z;
            m8A8.y = eyeY;
            dst->x = x;
            target = dst;
        }
        break;
    }
    case 5:
        kyorokyoro();
        icopy_xyz(gabi::ea(dst.get()), gabi::ea(&m8A8));
        target = dst;
        break;
    }
    gabi::Local<cXyz> eye; /* passed by value: a copy */
    eye->x = srcX;
    eye->z = srcZ;
    eye->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, target, eye, yrot, l_HIO_s16(0x1E), headOnly);
}
VERIFY(0x0226E604, &daNpc_Kk1_c::lookBack);

/* 0226E99C */
BOOL daNpc_Kk1_c::_execute() {
    WWHD_FUNC(0x0226E99C, BOOL, this);
    if (m931 == 0) {
        icopy_xyz(gabi::ea(&m87C), gabi::ea(&current.pos));
        m888.x = current.angle.x;
        m888.y = current.angle.y;
        m888.z = current.angle.z;
        m931 = 1;
    }
    m_jnt.setParam(l_HIO_s16(0x14), l_HIO_s16(0x16), l_HIO_s16(0x18), l_HIO_s16(0x1A), l_HIO_s16(0x0C), l_HIO_s16(0x0E),
                   l_HIO_s16(0x10), l_HIO_s16(0x12), l_HIO_s16(0x1C));
    if (m92D != 0 && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && !checkCommandTalk() && (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            pmf_call(this, &mAction, nullptr); /* (this->*mAction)(NULL) */
        }
        field_0x7d6 = 0;
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
    } else {
        m92D = 0;
        eventOrder();
    }
    m88E.x = current.angle.x;
    m88E.y = current.angle.y;
    m88E.z = current.angle.z;
    if (m92F == 0) {
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gnd));
    setMtx(false);
    if (m936 == 0 && m92B == 0) {
        if ((s8)mAC6 == 1) {
            setCollision(60.0f, 140.0f);
        } else {
            setCollision(40.0f, 140.0f);
        }
    }
    return TRUE;
}
VERIFY(0x0226E99C, &daNpc_Kk1_c::_execute);

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* 0226ECA8 */
BOOL daNpc_Kk1_c::_draw() {
    WWHD_FUNC(0x0226ECA8, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData_l(model);
    if (m92D != 0 || m930 != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBtpFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mBtpAnm.remove() */
    if (m92E == 0) {
        /* the second model (mAB4): brk/btk animations */
        J3DModel* m2 = gabi::at<J3DModel>(mAB4);
        gabi::call(0x025E779C, mBrkAnm, J3DModel_getModelData_l(m2), (f32)mAAC.x); /* mDoExt_brkAnm::entry */
        f32 btkFrame = (f32)mAAC.y;
        m2 = gabi::at<J3DModel>(mAB4);
        mBtkAnm.entry(J3DModel_getModelData_l(m2), btkFrame);
        mDoExt_modelEntryDL(gabi::at<J3DModel>(mAB4));
        gabi::store<u32>(gabi::ea(J3DModel_getModelData_l(gabi::at<J3DModel>(mAB4))) + 0x44, 0); /* btk remove */
        gabi::store<u32>(gabi::ea(J3DModel_getModelData_l(gabi::at<J3DModel>(mAB4))) + 0x40, 0); /* brk remove */
    }
    dSnap_RegistFig(0x58 /* DSNAP_TYPE_NPC_KM1_KK1 */, this, 1.0f, 1.0f, 1.0f);
    if (gabi::load<u8>(0x10467698 + 0x24)) {
        /* debug leftovers: function-local static colours, and the path point */
        gabi::Local<cXyz> pos;
        icopy_xyz(gabi::ea(pos.get()), gabi::ea(&current.pos));
        pos->y = eyePos.y;
        local_static_init(0x101FDA48, 0x101FEBEC, 0x1001C228);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001C22C);
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001C230);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001C234);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001C22C);
        kk1_getPathPoint(pos, &mPathRun);
        local_static_init(0x101FDA48, 0x101FEBEC, 0x1001C228);
    }
    return TRUE;
}
VERIFY(0x0226ECA8, &daNpc_Kk1_c::_draw);
