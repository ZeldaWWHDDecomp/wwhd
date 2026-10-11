/**
 * d_a_npc_cb1_act.cpp (WWHD)
 * NPC - Makar (Korok cellist): route checks, NPC/player actions, event actions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1_action.h"

// HD: this TU's query subobjects use the original inline-constructor vtables.
const gc_tu_data gc_tu = {
    .gndChk = {0x10018D5C, 0x10018D6C, 0x10018D8C, 0x10018D7C},
    .gndChkDt = {0x10018D6C, 0x10018D8C, cb1_action::BaseGroupVtable},
};

static cb1_action::ActionPtmf cbAct_waitNpcAction;
static cb1_action::ActionPtmf cbAct_searchNpcAction;
static cb1_action::ActionPtmf cbAct_jumpNpcAction;
static cb1_action::ActionPtmf cbAct_doorNpcAction;
static cb1_action::ActionPtmf cbAct_hitNpcAction;
static cb1_action::ActionPtmf cbAct_walkPlayerAction;
static cb1_action::ActionPtmf cbAct_jumpPlayerAction;
static cb1_action::ActionPtmf cbAct_waitPlayerAction;
static f32 cbAction_degreeScale;
static cXyz cbAction_linkOffset;
static cXyz cbAction_goalOffset;
static cXyz cbAction_doorOffset;
static u32 cbAction_doorGuard;
static cXyz cbAction_zero;
static s8 cbAction_stayNo;
static s16 cbAction_flyingTimer;

// HD: named aliases bind the original shared guest storage, never host copies.
static cb1_action::daNpc_Cb1_HIO_c cbAction_HIO;
static u16 cbAction_status;
static u32 cbAction_msgId;
static cb1_action::DebugRegisters cbAction_registers;

static inline u16 cb1_status() { return (u16)cbAction_status; }
static inline void cb1_setStatus(u16 v) { cbAction_status = v; }

// Cross-part calls use the shared actor declarations.

/* pointers to member (8-byte constants in .data) */

// Private helpers preserve the verified HD call and copy order.
static inline void *cb1_evmng_getMyP(s32 staffId, const char *name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, name, type);
}
/* dComIfGp_event_reset(): play + 0x52B8 |= 8 */
static inline void cb1_event_reset() { dComIfGp_event_reset(); }
/* dComIfGs_isEventBit / onEventBit: save + 0x644 */
static inline BOOL cb1_isEventBit(u16 f) { return dComIfGs_isEventBit(f); }
static inline void cb1_onEventBit(u16 f) { dComIfGs_onEventBit(f); }
/* fopAcM_onDraw(a): fopDwTg_ToDrawQ(&a->draw_tag, fpcM_DrawPriority(a)) */
static inline void cb1_onDraw(fopAc_ac_c *a) {
    s32 prio = CbAction_drawPriority(a);
    CbAction_enqueueDraw(gabi::ea(&a->draw_tag), prio);
}
/* dComIfGp_event_getTalkPartner(): dEvt_control_c::convPId(play + 0x51D0, *(play + 0x529C)) */
static inline fopAc_ac_c *cb1_getTalkPartner() { return dComIfGp_event_getTalkPartner(); }
/* dEvent_manager_c::getGoal (play + 0x52C4) */
static inline cXyz *cb1_evmng_getGoal() { return dComIfGp_evmng_getGoal(); }
/* cLib_offsetPos(out, pos, angle, offset) */
static inline void cb1_offsetPos(cXyz *out, cXyz *pos, s16 ang, const cXyz *ofs) {
    CbAction_offsetPos(out, pos, ang, ofs);
}
static inline BOOL cb1_chaseS(be<s16> *v, s16 target, s16 step) {
    return CbAction_chaseS(v, target, step);
}
/* cLib_calcTimer<s16>: the whole r3 is tested */
static inline s32 cb1_calcTimerS16(be<s16> *t) { return CbAction_calcTimerS16(t); }
/* daPy_py_c::setPlayerPosAndAngle (virtual, HD vtable slot 0x114) */
static inline void cb1_setPlayerPosAndAngle(fopAc_ac_c *pl, cXyz *pos, s16 ang) {
    u32 fn = (u32)gabi::at<cb1_action::PlayerVtable>(pl->__vtbl)->mSetPlayerPosAndAngle;
    gabi::call_ptr(fn, pl, pos, ang);
}
/* daPy_py_c::getTactMusic (virtual, HD vtable slot 0x2C) */
static inline s32 cb1_getTactMusic(fopAc_ac_c *pl) {
    u32 fn = (u32)gabi::at<cb1_action::PlayerVtable>(pl->__vtbl)->mGetTactMusic;
    return gabi::call_ptr<s32>(fn, pl);
}
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline fopAc_ac_c *cb1_getLink() { return dComIfGp_getLinkPlayer(); }
/* inline strcmp (GHS) */
static inline int cb1_strcmp(u32 a, const char *literal) {
    u32 b = gabi::guest_str(literal);
    for (;;) {
        u8 x = (u8)*gabi::at<be<u8>>(a++);
        u8 y = (u8)*gabi::at<be<u8>>(b++);
        if (x != y || x == 0) {
            return (int)x - (int)y;
        }
    }
}
/* cM_deg2s(deg): fctiwz(deg * 182.04445) */
static inline s16 cb1_deg2s(f32 deg) { return (s16)gabi::ftoi(deg * (f32)cbAction_degreeScale); }
/* sqrtf of the XZ length through a local (x, 0, z) and PSVECSquareMag */
/* (f1 results are used unrounded: GHS compares and passes them as doubles) */
/* inlined into evActWalk */
static inline f64 cb1_absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    f64 sq = CbAction_squareMag(v.get());
    return CbAction_sqrt(sq);
}
/* daPy_npc_c / mAcch helpers */

/* ====================================================================== */
/* event actions                                                          */
/* ====================================================================== */

/* 022256D4 */
void daNpc_Cb1_c::evCheckDisp(int staffIdx) {
    be<s32> *pDisp = (be<s32> *)cb1_evmng_getMyP(staffIdx, "Disp", 3);
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

/* 02225798 */
void daNpc_Cb1_c::evInitWait(int staffIdx) {
    evCheckDisp(staffIdx);
    void *pTimer = cb1_evmng_getMyP(staffIdx, "Timer", 3);
    if (pTimer) {
        m8EE = (s16)(s32) * static_cast<be<s32> *>(pTimer); /* (s16)*pTimer */
    } else {
        m8EE = 0;
    }
    setAnm((cb1_status() & daCbStts_MUSIC) ? cb1_action::ANM_08 : cb1_action::ANM_00);
}

/* 02225820 */
BOOL daNpc_Cb1_c::evActWait(int) {
    s16 temp = m8EE;
    if (cb1_chaseS(&m8EE, 0, 1)) {
        if (temp < 0) {
            cb1_event_reset();
        }
        return TRUE;
    }
    return FALSE;
}

/* 02225890 */
void daNpc_Cb1_c::evInitMsgSet(int staffIdx) {
    cbAction_msgId = 0xFFFFFFFF; /* l_msgId = fpcM_ERROR_PROCESS_ID_e */
    be<u32> *pMsgNo = (be<u32> *)cb1_evmng_getMyP(staffIdx, "MsgNo", 3);
    if (pMsgNo) {
        u32 no = *pMsgNo;
        mMsgNo = no;
        if (no == 0x1520 && cb1_isEventBit(0x1840)) {
            mMsgNo = 0x1522;
        }
    }
}

/* 02225928 */
BOOL daNpc_Cb1_c::evActMsgSet(int) { return initTalk(); }

/* 0222592C */
BOOL daNpc_Cb1_c::evActMsgEnd(int staffIdx) {
    BOOL result = execTalk(0);
    if (result) {
        cb1_evmng_getMyP(staffIdx, "EndMode", 3);
        setWaitAction(nullptr);
    } else {
        mJntCtrl.mbTrn = 1; /* mJntCtrl.setTrn() */
        lookBack(1);
    }
    return result;
}

/* 022259B8 */
void daNpc_Cb1_c::evInitMovePos(int staffIdx) {
    void *pAngle = cb1_evmng_getMyP(staffIdx, "Angle", 0);
    if (pAngle) {
        s16 angle = cb1_deg2s((f32) * static_cast<be<f32> *>(pAngle));
        current.angle.y = angle;
        shape_angle.y = angle;
    }
    void *pPos = cb1_evmng_getMyP(staffIdx, "Pos", 1);
    if (pPos) {
        /* current.pos.set(*pPos): float copies */
        auto *pos = static_cast<cXyz *>(pPos);
        current.pos.x = (f32)pos->x;
        current.pos.y = (f32)pos->y;
        current.pos.z = (f32)pos->z;
    }
    void *pShipRide = cb1_evmng_getMyP(staffIdx, "ShipRide", 4);
    if (pShipRide) {
        if (cb1_strcmp(gabi::ea(pShipRide), "on") == 0) {
            cb1_setStatus(cb1_status() | daCbStts_SHIP_RIDE);
        } else if (cb1_strcmp(gabi::ea(pShipRide), "off") == 0) {
            cb1_setStatus(cb1_status() & ~daCbStts_SHIP_RIDE);
        }
    }
}

// HD: without an "Offset" mode flag (m8E3 == 0) Link is placed at a fixed offset
/* 02225B24 */
void daNpc_Cb1_c::evInitOffsetLink(int staffIdx) {
    fopAc_ac_c *pLink = cb1_getLink();
    void *pTo = cb1_evmng_getMyP(staffIdx, "To", 1);
    if (m8E3 == 0) {
        gabi::Local<cXyz> dest;
        cb1_offsetPos(dest, &current.pos, shape_angle.y, &cbAction_linkOffset);
        s16 a = cLib_targetAngleY(dest, &current.pos);
        cb1_setPlayerPosAndAngle(pLink, dest, a);
    } else if (pTo) {
        gabi::Local<cXyz> dest;
        cb1_offsetPos(dest, &current.pos, shape_angle.y, (cXyz *)pTo);
        s16 a = cLib_targetAngleY(dest, &current.pos);
        cb1_setPlayerPosAndAngle(pLink, dest, a);
    }
    void *pFrom = cb1_evmng_getMyP(staffIdx, "From", 1);
    if (pFrom) {
        fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
        cb1_offsetPos(&current.pos, &pLink->current.pos, pPlayer->shape_angle.y, (cXyz *)pFrom);
    }
    current.angle.y = fopAcM_searchPlayerAngleY(this);
    shape_angle.y = current.angle.y;
}

/* 02225C74 */
void daNpc_Cb1_c::evInitWalk(int) {
    setAnm(cb1_action::ANM_01);
    speedF = 0.0f;
}

/* GHS struct copy of a cXyz (integer words) */
static inline void cb1_copyXyz(u32 dst, u32 src) {
    auto *to = gabi::at<cb1_action::WordVector>(dst);
    auto *from = gabi::at<cb1_action::WordVector>(src);
    to->x = (u32)from->x;
    to->y = (u32)from->y;
    to->z = (u32)from->z;
}

/* 02225CB0 */
BOOL daNpc_Cb1_c::evActWalk(int staffIdx) {
    be<f32> *speed_p = (be<f32> *)cb1_evmng_getMyP(staffIdx, "Speed", 0);
    be<f32> *pDist = (be<f32> *)cb1_evmng_getMyP(staffIdx, "Dist", 0);
    if (speed_p == nullptr) {
        JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x858, "speed_p != (0)");
    }
    void *pPos = cb1_evmng_getMyP(staffIdx, "Pos", 1);
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> temp2;
    if (pPos) {
        cb1_copyXyz(gabi::ea(temp.get()), gabi::ea(pPos));
    } else {
        void *pOffset = cb1_evmng_getMyP(staffIdx, "Offset", 1);
        auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        if (pOffset) {
            fopAc_ac_c *target =
                CbAction_convPId(gabi::ea(&play->play.mEvtCtrl), play->play.mEvtCtrl.mPtTalk);
            if (target == nullptr) {
                JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x869, "target != (0)");
            }
            s16 a = fopAcM_searchActorAngleY(this, target);
            cb1_offsetPos(temp, &target->current.pos, a, (cXyz *)pOffset);
        } else {
            cXyz *goal = CbAction_getGoal(gabi::ea(&play->play.mEvtManager));
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
            setAnm(cb1_action::ANM_00);
            cb1_copyXyz(gabi::ea(&current.pos), gabi::ea(temp.get()));
            speedF = 0.0f;
            return TRUE;
        }
    } else {
        f64 len = cb1_absXZ(dx, dz);
        if (len < (f32)*pDist) {
            setAnm(cb1_action::ANM_00);
            speedF = 0.0f;
            return TRUE;
        }
        walkSpeed = *speed_p;
    }
    if (cLib_distanceAngleS(shape_angle.y, angle) > 0x4000) {
        angle = (s16)(angle - 0x8000);
        walkSpeed = -walkSpeed;
    }
    if (mAcch.ChkGroundHit()) {
        walkAction(walkSpeed, cbAction_HIO.mForwardAccel, angle);
    } else if (m8E0) {
        speed.y = 10.0f;
    }
    return FALSE;
}

/* 02226078 */
void daNpc_Cb1_c::evInitToLink(int) {
    setAnm(cb1_action::ANM_01);
    speedF = 0.0f;
    s16 a = fopAcM_searchPlayerAngleY(this);
    current.angle.y = a;
    shape_angle.y = a;
}

/* 022260D0 */
BOOL daNpc_Cb1_c::evActToLink(int staffIdx) {
    be<f32> *speed_p = (be<f32> *)cb1_evmng_getMyP(staffIdx, "Speed", 0);
    be<f32> *dist_p = (be<f32> *)cb1_evmng_getMyP(staffIdx, "Dist", 0);
    if (speed_p == nullptr || dist_p == nullptr) {
        JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x8B0, "speed_p != (0) && dist_p != (0)");
    }
    f64 d = CbAction_searchDistanceXZ(
        this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ (f1 unrounded) */
    if (d < (f32)*dist_p) {           /* bge: NaN walks */
        setAnm(cb1_action::ANM_00);
        speedF = 0.0f;
        return TRUE;
    }
    s16 a = fopAcM_searchPlayerAngleY(this);
    f32 spd = *speed_p;
    walkAction(spd, cbAction_HIO.mForwardAccel, a);
    return FALSE;
}

/* 022261F4 */
u32 daNpc_Cb1_c::evInitTact(int) { return setAnm(cb1_action::ANM_08); }

/* 022261FC */
BOOL daNpc_Cb1_c::evActTact(int staffIdx) {
    be<s32> *pPrm0 = (be<s32> *)cb1_evmng_getMyP(staffIdx, "prm0", 3);
    s32 prm = 0;
    if (pPrm0) {
        prm = *pPrm0;
    }
    s32 song = cb1_getTactMusic(dComIfGp_getPlayer(0));
    if (song >= 0) {
        cb1_setStatus(cb1_status() | daCbStts_TACT_CORRECT);
    }
    BOOL result = execTalk(1);
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

// HD: the cello animation is ANM_17 (as on the non-demo GameCube)
/* 02226320 */
u32 daNpc_Cb1_c::evInitCelloPlay(int) { return setAnm(cb1_action::ANM_17); }

/* 02226328 */
BOOL daNpc_Cb1_c::evActCelloPlay(int) {
    return CbAction_checkEndTactMusic(cb1_getLink()) != 0 ? TRUE : FALSE; /* checkEndTactMusic */
}

/* 02226358 */
void daNpc_Cb1_c::evInitTurn(int staffIdx) {
    evCheckDisp(staffIdx);
    setAnm(cb1_action::ANM_00);
}

/* 02226390 */
BOOL daNpc_Cb1_c::evActTurn(int staffIdx) {
    void *pAngle = cb1_evmng_getMyP(staffIdx, "Angle", 0);
    s16 angle;
    if (pAngle) {
        angle = cb1_deg2s((f32) * static_cast<be<f32> *>(pAngle));
    } else {
        void *pTarget = cb1_evmng_getMyP(staffIdx, "Target", 3);
        auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        fopAc_ac_c *target;
        if (pTarget) {
            target = play->play.mpPlayerPtr[0].get(); /* dComIfGp_getLinkPlayer() */
        } else {
            target = CbAction_convPId(gabi::ea(&play->play.mEvtCtrl), play->play.mEvtCtrl.mPtTalk);
        }
        if (target == nullptr) {
            JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x962, "target != (0)");
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

/* 022264E4 */
void daNpc_Cb1_c::evInitSow(int staffIdx) {
    setAnm(cb1_action::ANM_0C);
    /* mNutBckAnim.getBckAnm()->getFrameMax() (virtual, slot 0x14) */
    cb1_action::Animation *anm = reinterpret_cast<cb1_action::BckAnimation *>(&mNutBckAnim)->mpAnm;
    u32 fn = (u32)anm->mVtable->mGetFrameMax;
    u32 frameMax = gabi::call_ptr<u32>(fn, anm);
    /* initPlay(frameMax, EMode_NONE, 1.0f, 0, -1, true) */
    CbAction_initPlay(&mNutBckAnim, frameMax, 0, 1.0f, 0, -1, 1);
    cb1_setStatus(cb1_status() | daCbStts_NUT);
    void *timer_p = cb1_evmng_getMyP(staffIdx, "Timer", 3);
    if (timer_p == nullptr) {
        JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x98F, "timer_p != (0)");
    }
    m8EE = (s16)(u32) * static_cast<be<u32> *>(timer_p);
}

// HD: a missing talk partner only asserts (no grow call through null)
/* 022265AC */
BOOL daNpc_Cb1_c::evActSow(int) {
    if (mNutBckAnim.play()) {
        u16 st = cb1_status();
        if (st & daCbStts_NUT) {
            cb1_setStatus(st & ~daCbStts_NUT);
            fopAc_ac_c *partner = cb1_getTalkPartner();
            if (partner == nullptr) {
                JUT_ASSERT_fail("d_a_npc_cb1.cpp", 0x99D, "tree != (0)");
            } else {
                static_cast<cb1_action::VmcView *>(partner)->mHasTree =
                    1; /* daObjVmc::Act_c::daObjVmc_ChangeGrow() */
            }
        }
    }
    if (m8D7) {
        setAnm(cb1_action::ANM_00);
    }
    return cb1_calcTimerS16(&m8EE) == 0;
}

/* 0222666C */
void daNpc_Cb1_c::evInitSetAnm(int staffIdx) {
    be<s32> *pNumber = (be<s32> *)cb1_evmng_getMyP(staffIdx, "Number", 3);
    if (pNumber) {
        u8 t = getAnmType(*pNumber);
        setAnm(t);
    }
}

// HD: with m8E3 == 0 the goal uses a fixed offset from Makar.
/* 022266DC */
void daNpc_Cb1_c::evInitSetGoal(int staffIdx) {
    void *pOffset = cb1_evmng_getMyP(staffIdx, "Offset", 1);
    if (m8E3 == 0) {
        cXyz *goal = cb1_evmng_getGoal();
        cb1_offsetPos(goal, &current.pos, shape_angle.y, &cbAction_goalOffset);
    } else if (pOffset) {
        cXyz *goal = cb1_evmng_getGoal();
        cb1_offsetPos(goal, &current.pos, shape_angle.y, (cXyz *)pOffset);
    }
}

// HD: evActSetGoal waits until Link is within 50 of the goal; after 150 frames
// (counter m8F3) it puts Link at the goal, facing away from Makar's direction
/* 02226784 */
BOOL daNpc_Cb1_c::evActSetGoal(int staffIdx) {
    evInitSetGoal(staffIdx);
    fopAc_ac_c *pl = cb1_getLink();
    cXyz *goal = cb1_evmng_getGoal();
    gabi::Local<cXyz> d;
    cXyz_mi(goal, d, &pl->current.pos);
    gabi::Local<cXyz> v;
    v->x = (f32)d->x;
    v->y = 0.0f;
    v->z = (f32)d->z;
    if (CbAction_squareMag(v.get()) >
        2500.0f) { /* PSVECSquareMag (f1 unrounded); ble: NaN returns */
        u8 n = (u8)(m8F3 + 1);
        m8F3 = n;
        if (n < 150) {
            return FALSE;
        }
        auto *vt = gabi::at<cb1_action::PlayerVtable>(pl->__vtbl);
        cXyz *g = cb1_evmng_getGoal();
        u32 fn = (u32)vt->mSetPlayerPosAndAngle;
        gabi::call_ptr(fn, pl, g, (s16)(shape_angle.y + 0x8000)); /* setPlayerPosAndAngle */
    }
    return TRUE;
}

/* 02226884 */
void daNpc_Cb1_c::evInitWarp(int) {
    setAnm(cb1_action::ANM_00);
    speed.y = cbAction_registers.mFloat[13] /* REG18_F(13) */ + 2.5f;
    m8EE = (s16) - (cbAction_registers.mShort[5] /* REG18_S(5) */ + 0x14);
}

/* 022268E0 */
BOOL daNpc_Cb1_c::evActWarp(int) {
    gravity = 0.0f;
    if (m8EE < 0) {
        if (cLib_chaseF(&speed.y, 0.0f, cbAction_registers.mFloat[14] + 0.1f) &&
            cb1_chaseS(&m8EE, 0, 1)) {
            m8EE = 0;
        }
    } else {
        if (cb1_chaseS(&m8EE, (s16)(cbAction_registers.mShort[6] + 8000),
                       (s16)(cbAction_registers.mShort[7] + 400))) {
            cLib_chaseF(&speed.y, cbAction_registers.mFloat[15] + 20.0f,
                        cbAction_registers.mFloat[16] + 0.5f);
            cLib_chaseF(&scale.x, 0.0f, cbAction_registers.mFloat[17] + 0.05f);
            scale.z = (f32)scale.x;
            if (cLib_chaseF(&scale.y, cbAction_registers.mFloat[18] + 4.0f,
                            cbAction_registers.mFloat[19] + 0.05f)) {
                return TRUE;
            }
        }
        s16 a = (s16)(current.angle.y + m8EE);
        shape_angle.y = a;
        current.angle.y = a;
    }
    return FALSE;
}

// HD: the Otkura warp (EndMode -2 after msg 0x1526) does not reset the event
/* 02226A88 */
void daNpc_Cb1_c::evInitEnd(int staffIdx) {
    be<s32> *pEndMode = (be<s32> *)cb1_evmng_getMyP(staffIdx, "EndMode", 3);
    s32 mode = pEndMode ? (s32)*pEndMode : 0;
    if (mode == -2) {
        if (mMsgNo == 0x1526) {
            CbAction_setNextStage("Otkura" /* "Otkura" */, 0xE6, 0, 8, 0.0f, 0, 1,
                                  0); /* dComIfGp_setNextStage */
            cb1_onEventBit(0x1610);
            cb1_onEventBit(0x1604);
        } else {
            cb1_event_reset();
        }
        setWaitAction(nullptr);
    } else if (mode == -1) {
        cb1_event_reset();
        setWaitAction(nullptr);
    } else {
        setWaitAction(nullptr);
    }
}

/* ====================================================================== */
/* route checks                                                           */
/* ====================================================================== */

/* this TU's dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_LinChk_vt CB1_LINCHK_VT = {cb1_action::LineCheckVtable,
                                             cb1_action::LinePolyVtable, cb1_action::LinePassVtable,
                                             cb1_action::LineGroupVtable};

/* ~dBgS_LinChk (inline vtable stores, then cBgS_LinChk's destructor) */
static inline void cb1_linChk_dt(void *chk) {
    auto *check = static_cast<cb1_action::LineCheck *>(chk);
    check->dBgS_PolyPassChk::__vtbl = cb1_action::LinePassVtable;
    check->dBgS_GrpPassChk::__vtbl = cb1_action::BaseGroupVtable;
    check->cBgS_PolyInfo::__vtbl = cb1_action::BaseLinePolyVtable;
    cBgS_LinChk_dt(chk, 0);
}

/* 02223E64 */
void daNpc_Cb1_c::routeAngCheck(cXyz *param_1, be<s16> *param_2) {
    gabi::Local<cXyz> temp;
    CbAction_outprod(&m910, temp.get(), param_1); /* m910.outprod(param_1) */
    s16 angle = cM_atan2s(temp->x, temp->z);
    bool flip;
    if (!(m910.y < 1.0f) && cLib_distanceAngleS(angle, *param_2) > 0x4000) {
        flip = true;
    } else {
        fopAc_ac_c *pl = dComIfGp_getPlayer(0);
        f32 y = current.pos.y;
        f32 dy = pl->current.pos.y - y;
        flip = temp->y * dy < 0.0f;
    }
    if (flip) {
        angle = (s16)(angle - 0x8000);
    }
    *param_2 = angle;
}

/* 02223F20 */
void daNpc_Cb1_c::routeWallCheck(cXyz *param_1, cXyz *param_2, be<s16> *param_3) {
    gabi::Local<cb1_action::LineCheck> lin_chk;
    dBgS_LinChk_ct(lin_chk, CB1_LINCHK_VT, false);
    dBgS_LinChk_Set(lin_chk, param_1, param_2, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
        dBgS *bgs = dComIfG_Bgsp();

        u16 poly = lin_chk->mPolyIndex;
        u16 bg = lin_chk->mBgIndex;
        void *pla = cBgS_GetTriPla(bgs, bg, poly);
        if (pla) {
            routeAngCheck((cXyz *)pla, param_3);
        }
    }
    cb1_linChk_dt(lin_chk);
}

/* 02223BB4 */
f32 daNpc_Cb1_c::checkForwardGroundY(s16 param_1) {
    dBgS *bgs = dComIfG_Bgsp();
    u16 bg = (u16)mAcchCir[0].mBgIndex;
    u16 poly = (u16)mAcchCir[0].mPolyIndex;
    void *pla = cBgS_GetTriPla(bgs, bg, poly);
    if (pla) {
        auto *plane = static_cast<cb1_action::Plane *>(pla);
        s16 a = cM_atan2s((f32)plane->mNormal.x, (f32)plane->mNormal.z);
        if (cLib_distanceAngleS(param_1, a) > 0x4000) {
            gabi::Local<dBgS_GndChk> gnd_chk;
            dBgS_GndChk_ct(gnd_chk, gc_tu.gndChk, false);

            gnd_chk->mFlag = (u32)gnd_chk->mFlag & ~2u; /* OffWall */
            f32 x = gabi::fmadds(80.0f, cM_ssin(param_1), current.pos.x);
            f32 y = current.pos.y + 80.0f;
            f32 z = gabi::fmadds(80.0f, cM_scos(param_1), current.pos.z);
            gnd_chk->m_pos.x = x; /* SetPos */
            gnd_chk->m_pos.y = y;
            gnd_chk->m_pos.z = z;
            f32 r = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
            /* ~dBgS_GndChk */
            gnd_chk->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
            gnd_chk->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
            gnd_chk->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
            CbAction_destroyBgCheck(gnd_chk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
            return r;
        }
    }
    return -1e+7f;
}

/* 02223D5C */
f32 daNpc_Cb1_c::checkWallJump(s16 param_1) {
    f64 g = CbAction_checkForwardGroundY(this, param_1); /* checkForwardGroundY (f1 unrounded) */
    f32 temp = (f32)(g - (f64)(f32)current.pos.y);
    if (0.0f < temp && temp < 80.0f) {
        f64 s = CbAction_sqrt(temp); /* std::sqrtf */
        return (f32)(s * (f64)3.8f);
    }
    return -1.0f;
}

// HD: the second wall angle is used without checking its wall-hit flag.
/* 02223DDC */
BOOL daNpc_Cb1_c::chkWallHit() {
    if (mAcch.ChkWallHit()) {
        s16 a;
        if (mAcchCir[0].ChkWallHit()) {
            a = mAcchCir[0].GetWallAngleY(); /* GetWallAngleY */
        } else {
            a = mAcchCir[1].GetWallAngleY();
        }
        return cLib_distanceAngleS(shape_angle.y, a) > 0x6000;
    }
    return FALSE;
}

// HD: destroy the line check before every early return.
/* 0222403C */
BOOL daNpc_Cb1_c::routeCheck(f32 param_1, be<s16> *param_2) {
    if (!(mAcch.ChkGroundHit())) {
        gabi::Local<cXyz> temp;
        temp->x = (f32)current.pos.x;
        temp->z = (f32)current.pos.z;
        auto *cp = reinterpret_cast<cb1_action::WordVector *>(&current.pos);
        auto *op = reinterpret_cast<cb1_action::WordVector *>(&old.pos);
        cp->z = (u32)op->z; /* current.pos = old.pos */
        cp->x = (u32)op->x;
        temp->y = (f32)current.pos.y;
        speedF = 0.0f;
        m8E0 = 1;
        cp->y = (u32)op->y;
        gabi::Local<cb1_action::LineCheck> lin_chk;
        dBgS_LinChk_ct(lin_chk, CB1_LINCHK_VT, false);
        dBgS_LinChk_Set(lin_chk, temp, &current.pos, nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
            dBgS *bgs = dComIfG_Bgsp();

            u16 poly = lin_chk->mPolyIndex;
            u16 bg = lin_chk->mBgIndex;
            void *pla = cBgS_GetTriPla(bgs, bg, poly);
            if (pla) {
                auto *plane = static_cast<cb1_action::Plane *>(pla);
                s16 a = cM_atan2s((f32)plane->mNormal.x, (f32)plane->mNormal.z);
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
        md_pmf_load(fn, gabi::ea(&cbAct_jumpNpcAction));
        setNpcAction(fn, nullptr);
        cb1_linChk_dt(lin_chk);
        return TRUE;
    }
    if (chkWallHit()) {
        f64 jump = CbAction_checkWallJump(this, (s16)*param_2); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp;
        *temp = (f32)jump;
        if (!(jump < 0.0f)) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, gabi::ea(&cbAct_jumpNpcAction));
            setNpcAction(fn, temp.get());
            return TRUE;
        }
        if (param_1 > 360000.0f /* SQUARE(600.0f) */) {
            return FALSE;
        }
        fopAc_ac_c *pl = dComIfGp_getPlayer(0);
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

/* ====================================================================== */
/* NPC actions                                                            */
/* ====================================================================== */

static inline void cb1_onAttnBit(daNpc_Cb1_c *t, u32 b) {
    t->attention_info.flags = (u32)t->attention_info.flags | b;
}
static inline void cb1_offAttnBit(daNpc_Cb1_c *t, u32 b) {
    t->attention_info.flags = (u32)t->attention_info.flags & ~b;
}
/* daPy_npc_c NPC call command (+0x3BC bit 1) */
/* inlined into NPC actions */
static inline void cb1_setNpcActionPmf(daNpc_Cb1_c *t, u32 pmf, void *arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    t->setNpcAction(fn, arg);
}
/* inlined into player actions */
static inline void cb1_setPlayerActionPmf(daNpc_Cb1_c *t, u32 pmf, void *arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    t->setPlayerAction(fn, arg);
}

// HD: the call command is tested first; a door command from the player switches to the
// HD door action.
/* 022244A0 */
BOOL daNpc_Cb1_c::searchNpcAction(void *) {
    if (m8F0 == 0) {
        cb1_onAttnBit(this, fopAc_Attn_ACTION_CARRY_e);
        setAnm(cb1_action::ANM_01);
        return TRUE;
    }
    if (m8F0 == -1) {
        return TRUE;
    }
    gabi::Local<be<s16>> angle;
    f32 temp2 = 0.0f;
    if (!((u32)mNoResetFlg1 & 2) /* !checkNpcCallCommand() */) {
        if (speedF == 0.0f) {
            cb1_setNpcActionPmf(this, gabi::ea(&cbAct_waitNpcAction), nullptr);
            return TRUE;
        }
        *angle = current.angle.y;
    } else {
        fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
        if ((u16)pPlayer->eventInfo.mCommand == 3 /* eventInfo.checkCommandDoor() */) {
            cb1_setNpcActionPmf(this, gabi::ea(&cbAct_doorNpcAction), nullptr);
            return TRUE;
        }
        if (m8E1 != 0) {
            s16 q = m8E1 < 0 ? -0x4000 : 0x4000;
            *angle = (s16)(cM_atan2s(m91C.x, m91C.z) + q);
        } else {
            mHasAttention = 1;
            f64 dist_sq = CbAction_searchDistance2(
                this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistance2 */
            f64 dist_xz_sq = CbAction_searchDistanceXZ2(
                this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ2 */
            f32 chase = cbAction_HIO.mPlayerChaseDistance;
            if (dist_sq < (f32)(chase * chase)) {
                temp2 = 0.0f;
            } else {
                f64 s = CbAction_sqrt(dist_xz_sq);
                f32 v = (f32)(s * (f64)cbAction_HIO.mChaseDistScale);
                f32 mx = cbAction_HIO.mMaxWalkSpeed;
                temp2 = (v - mx >= 0.0f) ? mx : v; /* fsel: cLib_maxLimit */
            }
            *angle = fopAcM_searchPlayerAngleY(this);
            BOOL stop = !CbAction_routeCheck(this, dist_xz_sq, angle.get()); /* routeCheck */
            if (!stop) {
                u32 st0 = dComIfGp_getPlayerStatus0();
                stop = (st0 & 0x02000101) != 0 /* daPyStts0_UNK2000000_e | HANG | UNK1 */ ||
                       ((u32) static_cast<daPy_py_l *>(pPlayer)->mResetFlg0 & 0x10000) !=
                           0 /* checkAttentionLock() */;
            }
            if (stop) {
                if (speedF == 0.0f) {
                    mNoResetFlg1 = (u32)mNoResetFlg1 & ~2u; /* offNpcCallCommand */
                    cb1_setNpcActionPmf(this, gabi::ea(&cbAct_waitNpcAction), this);
                    return TRUE;
                }
                *angle = current.angle.y;
                temp2 = 0.0f;
            }
            if (dist_xz_sq < 160000.0f /* SQUARE(400.0f) */ &&
                cLib_distanceAngleS(shape_angle.y, *angle) < 0x2000) {
                fopAc_ac_c *pl = dComIfGp_getPlayer(0);
                f32 y = current.pos.y;
                if (std::fabs(pl->current.pos.y - y) < 100.0f) {
                    cb1_setStatus(cb1_status() | daCbStts_PLAYER_FIND);
                }
            }
            if (CbAction_calcTimerU8(&m8DF) == 0) { /* cLib_calcTimer<u8> */
                m8DE = m8DE ^ 1;
                m8DF = (u8)CbAction_getRndValue(
                    cbAction_HIO.field_0xE2, cbAction_HIO.field_0xE4); /* cLib_getRndValue<s16> */
            }
            u8 side = m8DE;
            s16 d = cbAction_HIO.field_0xE6;
            if (side != 0) {
                d = -d;
            }
            *angle = (s16)(*angle + d);
        }
    }
    if (CbAction_checkCoHit(&mCyl)) { /* mCyl.ChkCoHit() */
        cb1_setNpcActionPmf(this, gabi::ea(&cbAct_hitNpcAction), nullptr);
    } else {
        walkAction(temp2, cbAction_HIO.mForwardAccel, *angle);
    }
    return TRUE;
}

// HD-only NPC action (entered from searchNpcAction on a door command): Makar is put at a
// fixed offset (-100, 0, -150) behind the player, in the current room, and waits
/* 0222487C */
static BOOL daNpc_Cb1_doorNpcAction(daNpc_Cb1_c *i_this, void *) {
    if (i_this->m8F0 == 0) {
        i_this->speedF = 0.0f;
        return TRUE;
    }
    if (i_this->m8F0 == -1) {
        return TRUE;
    }
    s8 room = (s8)cbAction_stayNo; /* dStage_roomControl_c::mStayNo */
    i_this->current.roomNo = room;
    if (dComIfGp_event_runCheck() != 0 /* dComIfGp_event_runCheck() */) {
        return TRUE;
    }
    // HD: retain the guard store before the vector initialization.
    if ((u32)cbAction_doorGuard == 0) {
        cbAction_doorGuard = 1;
        cbAction_doorOffset.x = -100.0f;
        cbAction_doorOffset.y = 0.0f;
        cbAction_doorOffset.z = -150.0f;
    }
    fopAc_ac_c *pl = dComIfGp_getPlayer(0);
    s16 a = pl->shape_angle.y;
    i_this->shape_angle.y = a;
    CbAction_doorOffsetPos(&i_this->current.pos, &pl->current.pos, a,
                           &cbAction_doorOffset); /* cLib_offsetPos */
    auto *currentWords = reinterpret_cast<cb1_action::WordVector *>(&i_this->current.pos);
    auto *oldWords = reinterpret_cast<cb1_action::WordVector *>(&i_this->old.pos);
    auto *homeWords = reinterpret_cast<cb1_action::WordVector *>(&i_this->home.pos);
    u32 z = (u32)currentWords->z;
    u32 x = (u32)currentWords->x;
    oldWords->z = z; /* old.pos = home.pos = current.pos */
    homeWords->x = x;
    oldWords->x = x;
    u32 y = (u32)currentWords->y;
    i_this->home.roomNo = room;
    oldWords->y = y;
    homeWords->z = z;
    homeWords->y = y;
    cb1_setNpcActionPmf(i_this, gabi::ea(&cbAct_waitNpcAction), nullptr);
    return TRUE;
}

/* CPad_GET_STICK_VALUE(0): f1 used unrounded */
static inline f64 cb1_stickValue() { return CbAction_stickValue(0); }
/* mDoAud_monsSeStart(id, pos, actorId (base.mBsPcId at +4), param, reverb) */
static inline void cb1_monsSeStart(daNpc_Cb1_c *t, u32 id) {
    s32 reverb = dComIfGp_getReverb(t->current.roomNo);
    u32 pid = (u32)t->mBsPcId;
    CbAction_monsSeStart(id, &t->eyePos, pid, 0, reverb);
}
/* dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool
 * headOnly) */
static inline void cb1_lookAtTarget(daNpc_Cb1_c *t, be<s16> *outY, cXyz *target, cXyz *eye,
                                    s16 yrot, s16 vel) {
    CbAction_lookAtTarget(&t->mJntCtrl, outY, target, eye, yrot, vel, 0);
}
/* the hit reaction shared by hitNpcAction and hitPlayerAction */
static inline void cb1_hitStart(daNpc_Cb1_c *t) {
    f32 lim = gabi::fmuls_ppc(cbAction_HIO.mMaxWalkSpeed, 0.5f);
    f32 spd = t->speedF;
    f32 s = (gabi::fsubs_ppc(spd, lim) >= 0.0f) ? lim : spd; /* fsel: cLib_maxLimit */
    f32 a = std::fabs(s);
    t->speedF = s;
    t->speed.y = gabi::fmuls_ppc(a, cbAction_HIO.mHitSpeedScaleY);
    t->speedF = gabi::fmuls_ppc(s, cbAction_HIO.mHitSpeedScaleF);
    s32 reverb = dComIfGp_getReverb(t->current.roomNo);
    mDoAud_seStart(cb1_action::JA_SE_CM_CB_BOUND, &t->eyePos, 0, reverb);
    t->setAnm(cb1_action::ANM_02);
}

/* 022249C0 */
BOOL daNpc_Cb1_c::hitNpcAction(void *) {
    if (m8F0 == 0) {
        cb1_offAttnBit(this, fopAc_Attn_ACTION_CARRY_e);
        cb1_hitStart(this);
    } else if (m8F0 != -1 && (mAcch.ChkGroundHit())) {
        speedF = 0.0f;
        cb1_setNpcActionPmf(this, gabi::ea(&cbAct_waitNpcAction), nullptr);
    }
    return TRUE;
}

/* 02224AC0 */
BOOL daNpc_Cb1_c::jumpNpcAction(void *param_1) {
    if (m8F0 == 0) {
        if (param_1) {
            f32 v = (f32) * static_cast<be<f32> *>(param_1);
            m900 = 0.0f;
            speed.y = v;
        } else {
            m900 = 0.0f;
            speed.y = 10.0f;
        }
        speedF = 4.0f;
        setAnm(cb1_action::ANM_16);
    } else if (m8F0 != -1 && (mAcch.ChkGroundHit())) {
        checkLanding();
        speedF = 0.0f;
        cb1_setNpcActionPmf(this, gabi::ea(&cbAct_waitNpcAction), nullptr);
    }
    return TRUE;
}

/* 02224BC0 */
BOOL daNpc_Cb1_c::rescueNpcAction(void *) {
    if (m8F0 != -1) {
        if (m8F0 == 0) {
            setAnm(cb1_action::ANM_00);
            fopAcM_offDraw(this);
        }
        m8DD = 0;
    }
    return TRUE;
}

/* 02224C20 */
BOOL daNpc_Cb1_c::musicNpcAction(void *) {
    if (m8F0 == 0) {
        setAnm(cb1_action::ANM_09);
        return TRUE;
    }
    if (m8F0 != -1 && fopAcM_GetParam(this) != 5 /* !isTypeKazeBoss() */) {
        cLib_addCalcAngleS(&shape_angle.y, home.angle.y, 8, 0x2000, 0x400);
        current.angle.y = shape_angle.y;
        BOOL att =
            chkAttention(cbAction_HIO.mNpc.mMaxAttnDistXZ /* l_HIO.mNpc.mMaxAttnDistXZ */, 0x10000);
        mHasAttention = (u8)att;
        if (att) {
            m8DD = fopAcM_GetParam(this) == 2 /* isTypeWaterFall() */ ? 7 : 6;
            cb1_onAttnBit(this, (fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e));
        } else {
            cb1_offAttnBit(this, 0xA);
        }
    }
    return TRUE;
}

/* 02224CFC */
BOOL daNpc_Cb1_c::shipNpcAction(void *) {
    if (m8F0 == 0) {
        mNoResetFlg1 = (u32)mNoResetFlg1 | 0x40; /* onNpcNotChange() */
        setAnm(cb1_action::ANM_0D);
        speedF = 0.0f;
        m8F0 = (s8)(m8F0 + 1);
        return TRUE;
    }
    if (m8F0 != -1) {
        if (!cb1_isEventBit(0x1604)) {
            cb1_setNpcActionPmf(this, gabi::ea(&cbAct_waitNpcAction), nullptr);
            cb1_setStatus(cb1_status() & ~daCbStts_SHIP_RIDE);
            return TRUE;
        }
        BOOL att =
            chkAttention(500.0f, cbAction_HIO.mNpc.mMaxAttnAngleY /* l_HIO.mNpc.mMaxAttnAngleY */);
        mHasAttention = (u8)att;
        if (att && !(dComIfGp_getPlayerStatus0() & 0x10000) /* !checkPlayerStatus0(SHIP_RIDE) */ &&
            getMsg()) {
            m8DD = 6;
            cb1_onAttnBit(this, 0xA);
        } else {
            cb1_offAttnBit(this, 0xA);
        }
    }
    return TRUE;
}

/* ====================================================================== */
/* player actions                                                         */
/* ====================================================================== */

/* local copy of current.pos for lookAtTarget's by-value cXyz */
static inline void cb1_posCopy(daNpc_Cb1_c *t, cXyz *d) {
    d->x = (f32)t->current.pos.x;
    d->y = (f32)t->current.pos.y;
    d->z = (f32)t->current.pos.z;
}

// HD: the attention test also accepts flag 0x20000000 (dAttention_c::Lockon)
/* 02224E38 */
BOOL daNpc_Cb1_c::waitPlayerAction(void *) {
    if (m8F0 == 0) {
        setAnm(cb1_action::ANM_00);
        cb1_onAttnBit(this, 0xFFFFFFFF);
        return TRUE;
    }
    if (m8F0 == -1 || sowCheck()) {
        return TRUE;
    }
    auto *att = reinterpret_cast<cb1_action::Attention *>(dComIfGp_getAttention());
    bool active = !(cb1_stickValue() < cbAction_HIO.field_0x80);
    if (!active) {
        active = CbAction_lockonTruth(gabi::ea(att)) /* LockonTruth */ ||
                 ((u32)att->mFlags & 0x20000000);
    }
    if (active) {
        s16 target = getStickAngY();
        cLib_addCalcAngleS(&current.angle.y, target, 0x19, 0x7FFF, 1);
        gabi::Local<cXyz> temp;
        int stickPos = calcStickPos(target, temp);
        gabi::Local<be<s16>> temp3;
        gabi::Local<cXyz> eye;
        if (stickPos != 0 && cb1_stickValue() < cbAction_HIO.field_0x84) {
            cb1_posCopy(this, eye);
            s16 sy = shape_angle.y;
            s16 vel = cbAction_HIO.mNpc.mMaxTurnStep /* l_HIO.mNpc.mMaxTurnStep */;
            *temp3 = sy;
            cb1_lookAtTarget(this, temp3, temp, eye, sy, vel);
        } else {
            s16 cy = current.angle.y;
            cb1_posCopy(this, eye);
            shape_angle.y = cy;
            s16 vel = cbAction_HIO.mNpc.mMaxTurnStep;
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
        if (!(cb1_stickValue() < cbAction_HIO.field_0x84) && stickPos == 0) {
            current.angle.y = target;
            cb1_setPlayerActionPmf(this, gabi::ea(&cbAct_walkPlayerAction), nullptr);
        }
    } else {
        s16 sy = shape_angle.y;
        gabi::Local<cXyz> zero;
        zero->x = (f32)cbAction_zero.x; /* cXyz::Zero */
        zero->y = (f32)cbAction_zero.y;
        zero->z = (f32)cbAction_zero.z;
        cb1_lookAtTarget(this, &shape_angle.y, nullptr, zero, sy, 0);
        current.angle.y = shape_angle.y;
    }
    breaking();
    return TRUE;
}

/* 022250AC */
BOOL daNpc_Cb1_c::walkPlayerAction(void *) {
    if (m8F0 == 0) {
        setAnm(cb1_action::ANM_01);
        cb1_onAttnBit(this, 0xFFFFFFFF);
        return TRUE;
    }
    if (m8F0 == -1 || sowCheck()) {
        return TRUE;
    }
    f64 st = cb1_stickValue();
    f32 temp = (f32)(st * (f64)cbAction_HIO.mStickWalkSpeedScale);
    s16 temp7 = getStickAngY();
    s32 temp2 = cLib_distanceAngleS(temp7, current.angle.y);
    f32 spd = speedF;
    f32 accel = cbAction_HIO.mForwardAccel;
    if (temp > spd) {
        f32 mn = cbAction_HIO.mMinWalkSpeed;
        if (temp < mn) {
            temp = mn;
        }
    } else {
        accel = cbAction_HIO.mDecel;
    }
    if (temp2 > 0x6000) {
        if (spd != 0.0f) {
            temp7 = current.angle.y;
        }
        temp = 0.0f;
        accel = 2.0f;
    }
    cLib_addCalcAngleS(&current.angle.y, temp7, cbAction_HIO.field_0xEC, cbAction_HIO.field_0xE8,
                       cbAction_HIO.field_0xEA);
    gabi::Local<cXyz> temp4;
    int stickPos = calcStickPos(temp7, temp4);
    if (stickPos == 0) {
        cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x2000, 0x400);
    }
    gabi::Local<cXyz> eye;
    cb1_posCopy(this, eye);
    gabi::Local<be<s16>> temp3;
    s16 sy = shape_angle.y;
    *temp3 = sy;
    cb1_lookAtTarget(this, temp3, temp4, eye, sy,
                     cbAction_HIO.mNpc.mMaxTurnStep /* l_HIO.mNpc.mMaxTurnStep */);
    if (stickPos > 0) {
        shape_angle.y = (s16)*temp3;
    }
    f32 mx = cbAction_HIO.mMaxWalkSpeed;
    temp = (temp - mx >= 0.0f) ? mx : temp; /* fsel: cLib_maxLimit */
    if (cLib_chaseF(&speedF, temp, accel) && temp == 0.0f) {
        cb1_setPlayerActionPmf(this, gabi::ea(&cbAct_waitPlayerAction), nullptr);
        return TRUE;
    }
    f32 a = gabi::fmuls_ppc(speedF, cbAction_HIO.mWalkAnmSpeedScale);
    f32 mn = cbAction_HIO.mMaxWalkAnmSpeed;
    f32 rate = (a - mn >= 0.0f) ? a : mn; /* fsel: cLib_minLimit */
    mpMorf->setPlaySpeed(rate);           /* mpMorf->setPlaySpeed() */
    if (!(mAcch.ChkGroundHit())) {
        cb1_setPlayerActionPmf(this, gabi::ea(&cbAct_jumpPlayerAction), nullptr);
        return TRUE;
    }
    if (chkWallHit()) {
        f64 jump =
            CbAction_checkWallJump(this, (s16)current.angle.y); /* checkWallJump (f1 unrounded) */
        gabi::Local<be<f32>> temp5;
        *temp5 = (f32)jump;
        if (!(jump < 0.0f)) {
            cb1_setPlayerActionPmf(this, gabi::ea(&cbAct_jumpPlayerAction), temp5.get());
        }
    }
    return TRUE;
}

/* 02225388 */
BOOL daNpc_Cb1_c::hitPlayerAction(void *) {
    if (m8F0 == 0) {
        cb1_hitStart(this);
    } else if (m8F0 != -1 && (mAcch.ChkGroundHit())) {
        m4E4 = m4E4 | 1; /* returnLink() */
        speedF = 0.0f;
    }
    return TRUE;
}

/* 02225464 */
BOOL daNpc_Cb1_c::jumpPlayerAction(void *param_1) {
    if (m8F0 == 0) {
        if (param_1) {
            f32 v = (f32) * static_cast<be<f32> *>(param_1);
            m900 = 0.0f;
            speed.y = v;
        } else {
            m900 = 0.0f;
            speed.y = 10.0f;
        }
        setAnm(cb1_action::ANM_16);
    } else if (m8F0 != -1 && !flyCheck() && (mAcch.ChkGroundHit())) {
        checkLanding();
        speedF = 0.0f;
        cb1_setPlayerActionPmf(this, gabi::ea(&cbAct_waitPlayerAction), nullptr);
    }
    return TRUE;
}

/* 0222555C */
BOOL daNpc_Cb1_c::flyPlayerAction(void *) {
    if (m8F0 == 0) {
        speed.y = cbAction_HIO.mFlyLaunchSpeedY;
        setAnm(cb1_action::ANM_04);
        cbAction_flyingTimer = (s16)cbAction_HIO.mPlayerFlyTimer; /* setFlyingTimer() */
        cb1_monsSeStart(this, cb1_action::JA_SE_CV_CB_LEAF_OUT);
    } else if (m8F0 != -1) {
        dComIfGp_setAStatus(6);              /* dComIfGp_setAStatus(dActStts_LET_GO_e) */
        BOOL trigA = CbAction_checkTrigA(0); /* CPad_CHECK_TRIG_A(0) */
        f32 spd = (f32)(cb1_stickValue() * (f64)cbAction_HIO.mStickFlySpeedScale);
        s16 ang = getStickAngY();
        BOOL trigB = CbAction_checkTrigB(0); /* CPad_CHECK_TRIG_B(0) */
        flyAction(trigA, spd, ang, trigB);
    }
    return TRUE;
}

/* 0222566C */
BOOL daNpc_Cb1_c::carryPlayerAction(void *) {
    if (m8F0 == 0) {
        setAnm(cb1_action::ANM_0B);
        cb1_monsSeStart(this, cb1_action::JA_SE_CV_CB_DAMAGE_S);
    }
    return TRUE;
}

// clang-format off
/* WWHD glue
str 10018FF8 in 022256D4
str 10019000 in 02225798
str 10019008 in 02225890
str 10019010 in 0222592C
str 10019024 in 022259B8
str 1001901C in 022259B8
str 10019030 in 022259B8
str 1001902C in 022259B8
str 10019020 in 022259B8
str 1001903C in 02225B24
str 10019040 in 02225B24
str 10019058 in 02225CB0
str 10019060 in 02225CB0
str 10019054 in 02225CB0
str 10019068 in 02225CB0
str 100190A0 in 022260D0
str 100190A8 in 022260D0
str 100190E0 in 022261FC
str 100190E8 in 02226390
str 100190F0 in 02226390
str 10019118 in 022264E4
str 1001915C in 0222666C
str 10019164 in 022266DC
str 10019188 in 02226A88
str 10019070 in 02225CB0
str 10019090 in 02225CB0
str 10019080 in 02225CB0
str 100190B0 in 022260D0
str 100190C0 in 022260D0
str 100190F8 in 02226390
str 10019108 in 02226390
str 10019120 in 022264E4
str 10019130 in 022264E4
str 10019140 in 022265AC
str 10019150 in 022265AC
str 10019190 in 02226A88
data cbAct_waitNpcAction 10018BE0
data cbAct_searchNpcAction 10018BE8
data cbAct_jumpNpcAction 10018C18
data cbAct_doorNpcAction 10018C20
data cbAct_hitNpcAction 10018C28
data cbAct_walkPlayerAction 10018C30
data cbAct_jumpPlayerAction 10018C38
data cbAct_waitPlayerAction 10018BF8
data cbAction_degreeScale 10019018
data cbAction_linkOffset 10019048
data cbAction_goalOffset 1001916C
data cbAction_doorOffset 10466B78
data cbAction_doorGuard 10466B94
data cbAction_zero 101FFBA8
data cbAction_stayNo 1047E6C8
data cbAction_flyingTimer 101BDC28
ext 02544900 cXyz* CbAction_getGoal(u32 manager)
ext 0200F9D4 void CbAction_offsetPos(cXyz* out, cXyz* pos, s16 ang, const cXyz* ofs)
ext 0200F564 BOOL CbAction_chaseS(be<s16>* v, s16 target, s16 step)
ext 02055B64 s32 CbAction_calcTimerS16(be<s16>* t)
ext 020079B4 f64 CbAction_stickValue(s32 pad)
ext 025DF2B8 s32 CbAction_drawPriority(fopAc_ac_c* actor)
ext 025DA874 void CbAction_enqueueDraw(u32 tag, s32 priority)
ext 0253EE04 fopAc_ac_c* CbAction_convPId(u32 control, u32 id)
ext 028E8DD0 f64 CbAction_squareMag(const cXyz* vector)
ext 028F4384 f64 CbAction_sqrt(f64 value)
ext 025D6958 f64 CbAction_searchDistanceXZ(fopAc_ac_c* actor, fopAc_ac_c* player)
ext 025D6924 f64 CbAction_searchDistance2(fopAc_ac_c* actor, fopAc_ac_c* player)
ext 025D69AC f64 CbAction_searchDistanceXZ2(fopAc_ac_c* actor, fopAc_ac_c* player)
ext 0244307C s32 CbAction_checkEndTactMusic(fopAc_ac_c* player)
ext 025E72D4 void CbAction_initPlay(mDoExt_bckAnm* animation, u32 end, s32 mode, f32 speed, s32 start, s32 last, s32 modify)
ext 0252012C void CbAction_setNextStage(const char* stage, s32 start, s32 room, s32 layer, f32 speed, s32 wipe, s32 mode, s32 param)
ext 02008DAC void CbAction_destroyBgCheck(void* check, s32 flags)
ext 02223BB4 f64 CbAction_checkForwardGroundY(daNpc_Cb1_c* actor, s16 angle)
ext 02223D5C f64 CbAction_checkWallJump(daNpc_Cb1_c* actor, s16 angle)
ext 0201B080 void CbAction_outprod(cXyz* vector, cXyz* out, cXyz* other)
ext 0222403C BOOL CbAction_routeCheck(daNpc_Cb1_c* actor, f64 distance, be<s16>* angle)
ext 0207A9A0 s32 CbAction_calcTimerU8(be<u8>* timer)
ext 022270A0 s32 CbAction_getRndValue(s16 minimum, s16 range)
ext 02516464 BOOL CbAction_checkCoHit(dCcD_Cyl* cylinder)
ext 0200FA40 void CbAction_doorOffsetPos(cXyz* out, cXyz* base, s16 angle, cXyz* offset)
ext 025E1AA4 void CbAction_monsSeStart(u32 sound, cXyz* pos, u32 actorId, s32 param, s32 reverb)
ext 0259DED0 void CbAction_lookAtTarget(dNpc_JntCtrl_c* joint, be<s16>* angle, cXyz* target, cXyz* eye, s16 yaw, s16 velocity, s32 headOnly)
ext 024EDFCC bool CbAction_lockonTruth(u32 attention)
ext 02007898 BOOL CbAction_checkTrigA(s32 pad)
ext 020078BC BOOL CbAction_checkTrigB(s32 pad)
data cbAction_HIO 10466B98
data cbAction_status 101BDC2A
data cbAction_msgId 10466B28
data cbAction_registers 1047B608
*/
