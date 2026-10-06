/**
 * d_a_npc_os_b.cpp (WWHD)
 * NPC - Os, part B (fork osB): destructor, event functions, talk, attention/area checks,
 * lookBack, stick helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_local.h"

/* ---- statics (HD addresses) ---- */
#define OS_L_HIO 0x10467E44u        /* daNpc_Os_HIO_c l_HIO (HD: mNo +0, mOs2 +4, mNpc +0x30, vtable +0xB0) */
#define OS_L_HIO_COUNTER 0x10467DECu
#define OS_M_SMOKE 0x10475440u      /* daNpc_Os_c::m_smoke (dPa_smokeEcallBack) */
#define OS_M_PLAYERROOM 0x101D5F4Bu /* bool daNpc_Os_c::m_playerRoom[] */
#define OS_M_CATTLEROOMNO 0x101D5F46u
#define OS_L_MSGID 0x10467DF0u      /* fpc_ProcID l_msgId */

/* l_HIO fields: GameCube offset -4 for daNpc_Os_HIO_c's own floats; mOs2 fields at GameCube offset;
 * mNpc (dNpc_HIO_c, HD) at +0x30 */
static inline f32 hio_f(u32 gcOff) { return gabi::load<f32>(OS_L_HIO + gcOff - 4); }
static inline f32 hio2_f(u32 gcOff) { return gabi::load<f32>(OS_L_HIO + gcOff); }
static inline s16 hio2_s(u32 gcOff) { return gabi::load<s16>(OS_L_HIO + gcOff); }
enum : u32 { NPC_m04 = 0x30, NPC_mMaxHeadTurnVel = 0x46, NPC_mAttnYOffset = 0x48, NPC_mMaxAttnAngleY = 0x4C, NPC_mMaxAttnDistXZ = 0x50 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* evmng_getMyP(s32 staffId, u32 name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type);
}
static inline void os_onEventBit(u16 f) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
/* calls into other parts */
static inline void os_setAnm(daNpc_Os_c* t, s32 i) { gabi::call(0x022AA88C, t, i); }
static inline BOOL os_initBrkAnm(daNpc_Os_c* t, u32 no, u32 b) { return gabi::call<BOOL>(0x022A9F4C, t, no, b); }
static inline void os_endBeam(daNpc_Os_c* t) { gabi::call(0x022AB28C, t); }

/* ---- functions ---- */

/* 022AD218 daNpc_Os_c::~daNpc_Os_c (deleting destructor) */
static void daNpc_Os_c_dtor(daNpc_Os_c* i_this, s32 flags) {
    WWHD_FUNC(0x022AD218, void, i_this, flags);
    if (i_this == nullptr) {
        return;
    }
    i_this->__vtbl = OS_VTBL;
    dComIfG_resDelete(&i_this->mPhs, STR(0x1001F640) /* "Os" */);
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    os_endBeam(i_this);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(OS_M_SMOKE) + 0x44), OS_M_SMOKE); /* m_smoke.remove() */
    s32 cnt = gabi::load<s32>(OS_L_HIO_COUNTER);
    bool del = true;
    if (cnt != 0) {
        cnt = cnt - 1;
        gabi::store<s32>(OS_L_HIO_COUNTER, cnt);
        del = cnt <= 0;
    }
    if (del) {
        s8 no = gabi::load<s8>(OS_L_HIO);
        if (no >= 0) {
            mDoHIO_deleteChild(no);
            gabi::store<s8>(OS_L_HIO, -1);
        }
    }
    gabi::store<u8>(OS_M_PLAYERROOM + i_this->argument, 0); /* offPlayerRoom(argument) */
    gabi::store<s8>(OS_M_CATTLEROOMNO, -1);
    gabi::call(0x0268A8DC, 0, (s32)i_this->argument); /* HD-only call (0, argument) */
    gabi::call(0x02515A70, &i_this->mCyl, 2);      /* dCcD_Cyl::~dCcD_Cyl */
    gabi::call(0x02515860, &i_this->mStts, 2);     /* dCcD_Stts::~dCcD_Stts */
    gabi::call(0x028F0164, &i_this->mAcchCir, 2, 0x40, 0x022B050C, 0, 0); /* __destroy_arr(mAcchCir, ~dBgS_AcchCir) */
    gabi::call(0x0244513C, i_this, 0);             /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}
VERIFY(0x022AD218, daNpc_Os_c_dtor);

/* 022AD360 (unnamed) */
void daNpc_Os_c::initialDefault(int) {
    WWHD_FUNC(0x022AD360, void, this, (u32)0);
}
VERIFY(0x022AD360, &daNpc_Os_c::initialDefault);

/* 022AD364 (unnamed) */
BOOL daNpc_Os_c::actionDefault(int) {
    WWHD_FUNC(0x022AD364, BOOL, this, (u32)0);
    return TRUE;
}
VERIFY(0x022AD364, &daNpc_Os_c::actionDefault);

/* 022AD36C */
void daNpc_Os_c::initialWaitEvent(int staffIdx) {
    WWHD_FUNC(0x022AD36C, void, this, staffIdx);
    field_0x7C0 = -1;

    cXyz* posData = (cXyz*)evmng_getMyP(staffIdx, 0x1001F650 /* "pos" */, 1);
    if (posData) {
        f32 y = posData->y;
        f32 z = posData->z;
        f32 x = posData->x;
        current.pos.z = z;
        current.pos.y = y;
        current.pos.x = x;
    }

    be<s32>* angleData = (be<s32>*)evmng_getMyP(staffIdx, 0x1001F654 /* "angle" */, 3);
    if (angleData) {
        s16 angle = (s16)(s32)*angleData;
        shape_angle.y = angle;
        current.angle.y = angle;
    }

    if (evmng_getMyP(staffIdx, 0x1001F648 /* "gravity" */, 3)) {
        field_0x784 |= 8; /* onGravity */
        maxFallSpeed = -100.0f;
        gravity = hio_f(0x8C);
    }

    be<s32>* quakeData = (be<s32>*)evmng_getMyP(staffIdx, 0x1001F65C /* "quake" */, 3);
    if (quakeData) {
        u32 play = dComIfGp_ea();
        gabi::Local<cXyz> pos;
        pos->x = 0.0f;
        pos->y = 1.0f;
        pos->z = 0.0f;
        gabi::call<BOOL>(0x025CB374, play + PLAY_VIBRATION, (s32)*quakeData, -0x11, pos.get()); /* StartShock */
    }

    be<s32>* timerData = (be<s32>*)evmng_getMyP(staffIdx, 0x1001F664 /* "timer" */, 3);
    if (timerData) {
        field_0x7C0 = (s16)(s32)*timerData;
    }
}
VERIFY(0x022AD36C, &daNpc_Os_c::initialWaitEvent);

/* 022AD4F0 (unnamed) */
BOOL daNpc_Os_c::actionWaitEvent(int) {
    WWHD_FUNC(0x022AD4F0, BOOL, this, (u32)0);
    if (field_0x7C0 < 0) {
        return TRUE;
    }
    return gabi::call<s16>(0x02055B64, &field_0x7C0) == 0 ? TRUE : FALSE; /* cLib_calcTimer<s16> */
}
VERIFY(0x022AD4F0, &daNpc_Os_c::actionWaitEvent);

/* 022AD540 */
void daNpc_Os_c::setWakeup() {
    WWHD_FUNC(0x022AD540, void, this);
    if (argument == 0) {
        os_onEventBit(0x1780);
    } else if (argument == 1) {
        os_onEventBit(0x1740);
    } else if (argument == 2) {
        os_onEventBit(0x1720);
    }
}
VERIFY(0x022AD540, &daNpc_Os_c::setWakeup);

/* 022AD598 */
void daNpc_Os_c::initialWakeupEvent(int) {
    WWHD_FUNC(0x022AD598, void, this, (u32)0);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this)) { /* dComIfGp_getCb1Player() == this */
        setWakeup();
        maxFallSpeed = -100.0f;
        gravity = hio_f(0x8C);
        gabi::store<u32>(gabi::ea(this) + 0x3BC, gabi::load<u32>(gabi::ea(this) + 0x3BC) & ~0x40u); /* offNpcNotChange */
        os_initBrkAnm(this, 4, 1);
        field_0x7A1 = 0;
    }
}
VERIFY(0x022AD598, &daNpc_Os_c::initialWakeupEvent);

/* 022AD614 */
BOOL daNpc_Os_c::actionWakeupEvent(int) {
    WWHD_FUNC(0x022AD614, BOOL, this, (u32)0);
    if (field_0x7A1) {
        os_initBrkAnm(this, 6, 1);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022AD614, &daNpc_Os_c::actionWakeupEvent);

/* 022AD660 (unnamed) */
void daNpc_Os_c::initialMoveEvent(int) {
    WWHD_FUNC(0x022AD660, void, this, (u32)0);
    os_setAnm(this, 1);
}
VERIFY(0x022AD660, &daNpc_Os_c::initialMoveEvent);

/* 022AD668 */
u32 daNpc_Os_c::walkProc(f32 param_1, s16 param_2) {
    WWHD_FUNC(0x022AD668, u32, this, param_1, param_2);
    f32 f = mPrevMorfFrame;
    if (f > hio_f(0xB0) && f < hio_f(0xAC)) {
        speedF = hio2_f(0x08) * param_1;
    } else {
        speedF = 0.0f;
    }
    return gabi::call<u32>(0x0200F378, &current.angle.y, param_2, hio2_s(0x28), hio2_s(0x24), hio2_s(0x26)); /* cLib_addCalcAngleS */
}
VERIFY(0x022AD668, &daNpc_Os_c::walkProc);

/* 022AD6CC */
void daNpc_Os_c::setAttention(bool param_1) {
    WWHD_FUNC(0x022AD6CC, void, this, param_1);
    if (!param_1 && field_0x7A3 >= 2) {
        return;
    }
    eyePos.x = field_0x748.x;
    eyePos.y = field_0x748.y;
    eyePos.z = field_0x748.z;
    cXyz* att = gabi::at<cXyz>(gabi::ea(this) + 0x390);
    f32 y = current.pos.y + gabi::load<f32>(OS_L_HIO + NPC_mAttnYOffset);
    att->z = current.pos.z;
    att->x = current.pos.x;
    att->y = y;
}
VERIFY(0x022AD6CC, &daNpc_Os_c::setAttention);

/* ---- continued ---- */
static inline void os_walkProc(daNpc_Os_c* t, f32 v, s16 a) { gabi::call(0x022AD668, t, v, a); }
static inline void os_setAttention(daNpc_Os_c* t, u32 b) { gabi::call(0x022AD6CC, t, b); }
static inline void os_setFinish(daNpc_Os_c* t) { gabi::call(0x022AB618, t); }
static inline void os_setNpcAction(daNpc_Os_c* t, ProcFunc_l* f, void* p) { gabi::call(0x022AA778, t, f, p); }

/* 022AD720 */
BOOL daNpc_Os_c::actionMoveEvent(int staffIdx) {
    WWHD_FUNC(0x022AD720, BOOL, this, staffIdx);
    f32 value = 1.0f;
    be<f32>* data = (be<f32>*)evmng_getMyP(staffIdx, 0x1001F670 /* "Stick" */, 0);
    fopAc_ac_c* ped = mpPedestal;
    if (data) {
        value = *data;
    }
    if (ped) {
        if (fopAcM_searchActorDistanceXZ(this, ped) < 10.0f) {
            fopAc_ac_c* p = mpPedestal;
            speedF = 0.0f;
            current.pos.x = p->current.pos.x;
            current.pos.z = p->current.pos.z;
            os_setAnm(this, 0);
        } else {
            s16 target_angle = fopAcM_searchActorAngleY(this, mpPedestal);
            os_walkProc(this, value, target_angle);
            gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, value + value); /* mpMorf->setPlaySpeed(value * 2) */
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, hio2_s(0x28), (s16)(hio2_s(0x24) * 2), (s16)(hio2_s(0x26) * 2));
            s16 yrot = shape_angle.y;
            gabi::Local<cXyz> eye; /* cXyz::Zero (copy) */
            cXyz* z = gabi::at<cXyz>(0x101FFBA8);
            f32 zx = z->x;
            f32 zz = z->z;
            f32 zy = z->y;
            eye->z = zz;
            eye->y = zy;
            eye->x = zx;
            gabi::call(0x0259DED0, &mJntCtrl, &shape_angle.y, (u32)0, eye.get(), yrot, 0, 0); /* lookAtTarget */
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}
VERIFY(0x022AD720, &daNpc_Os_c::actionMoveEvent);

/* 022AD89C */
void daNpc_Os_c::initialMoveEndEvent(int staffIdx) {
    WWHD_FUNC(0x022AD89C, void, this, staffIdx);
    void* data = evmng_getMyP(staffIdx, 0x1001F678 /* "Daiza" */, 3);
    if (data && mpPedestal) {
        current.pos.x = mpPedestal->current.pos.x;
        fopAc_ac_c* p = mpPedestal;
        current.pos.y = p->current.pos.y + 240.0f;
        current.pos.z = p->current.pos.z;
    }
    speedF = 0.0f;
    os_setAnm(this, 0);
}
VERIFY(0x022AD89C, &daNpc_Os_c::initialMoveEndEvent);

/* 022AD940 */
void daNpc_Os_c::initialEndEvent(int) {
    WWHD_FUNC(0x022AD940, void, this, (u32)0);
    os_setFinish(this);
    gabi::Local<be<u32>> temp;
    *temp = 0;
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, 0x1001F298); /* &daNpc_Os_c::finish02NpcAction */
    os_setNpcAction(this, fn, temp.get());
}
VERIFY(0x022AD940, &daNpc_Os_c::initialEndEvent);

/* 022AD998 (unnamed) */
void daNpc_Os_c::initialTurnEvent(int) {
    WWHD_FUNC(0x022AD998, void, this, (u32)0);
}
VERIFY(0x022AD998, &daNpc_Os_c::initialTurnEvent);

/* 022AD99C */
BOOL daNpc_Os_c::actionTurnEvent(int staffIdx) {
    WWHD_FUNC(0x022AD99C, BOOL, this, staffIdx);
    void* data = evmng_getMyP(staffIdx, 0x1001F680 /* "Angle" */, 3);
    if (data != nullptr) {
        s16 target = gabi::load<s16>(gabi::ea(data) + 2); /* (s16)*(int*)data */
        s16 temp = cLib_addCalcAngleS(&shape_angle.y, target, 0x1E, 0x2000, 0x800);
        current.angle.y = shape_angle.y;
        if (temp == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022AD99C, &daNpc_Os_c::actionTurnEvent);

/* 022ADA3C */
void daNpc_Os_c::setAnm_brkAnm(int param_1) {
    WWHD_FUNC(0x022ADA3C, void, this, param_1);
    u32 e = 0x101C2B1C + param_1 * 2; /* anmBrkTbl[param_1] */
    os_setAnm(this, gabi::load<u8>(e));
    os_initBrkAnm(this, gabi::load<u8>(e + 1), 1);
    field_0x7A1 = 0;
}
VERIFY(0x022ADA3C, &daNpc_Os_c::setAnm_brkAnm);

/* 022ADAA4 */
void daNpc_Os_c::initialFinishEvent(int staffIdx) {
    WWHD_FUNC(0x022ADAA4, void, this, staffIdx);
    be<u32>* data = (be<u32>*)evmng_getMyP(staffIdx, 0x1001F688 /* "Type" */, 3);
    u32 value = 0;
    if (data) {
        value = *data;
    }
    setAnm_brkAnm(value);
    if (field_0x78C == 2 || field_0x78C == 4) {
        gabi::call(0x022ABF7C, this, 1); /* makeBeam(1) */
    } else {
        os_endBeam(this);
    }
}
VERIFY(0x022ADAA4, &daNpc_Os_c::initialFinishEvent);

/* 022ADB4C (unnamed) */
BOOL daNpc_Os_c::actionFinishEvent(int) {
    WWHD_FUNC(0x022ADB4C, BOOL, this, (u32)0);
    return field_0x7A1 != 0 ? TRUE : FALSE;
}
VERIFY(0x022ADB4C, &daNpc_Os_c::actionFinishEvent);

/* 022ADB60 */
void daNpc_Os_c::initialMsgSetEvent(int staffIdx) {
    WWHD_FUNC(0x022ADB60, void, this, staffIdx);
    gabi::store<s32>(OS_L_MSGID, -1);
    be<u32>* data = (be<u32>*)evmng_getMyP(staffIdx, 0x1001F690 /* "MsgNo" */, 3);
    if (data) {
        field_0x780 = *data;
    }
}
VERIFY(0x022ADB60, &daNpc_Os_c::initialMsgSetEvent);

/* HD message manager (*(0x101F4B5C)): 025F7DB0 messageSet(mgr, msgNo, pos), 025F795C status (the matcher's
 * fopMsgM_SearchByID), 025F74D0 setStatus(mgr, status) */
static inline u32 os_msgMgr() { return gabi::load<u32>(0x101F4B5C); }

/* 022ADBCC */
BOOL daNpc_Os_c::talk_init() {
    WWHD_FUNC(0x022ADBCC, BOOL, this);
    u32 mgr = os_msgMgr();
    if (gabi::load<s32>(OS_L_MSGID) == -1) {
        gabi::store<u32>(OS_L_MSGID, gabi::call<u32>(0x025F7DB0, mgr, (u32)field_0x780, &eyePos));
        return FALSE;
    }
    return TRUE; /* HD: no fopMsgM_SearchByID(l_msgId) */
}
VERIFY(0x022ADBCC, &daNpc_Os_c::talk_init);

/* 022ADC38 (unnamed) */
BOOL daNpc_Os_c::actionMsgSetEvent(int) {
    WWHD_FUNC(0x022ADC38, BOOL, this, (u32)0);
    return talk_init();
}
VERIFY(0x022ADC38, &daNpc_Os_c::actionMsgSetEvent);

/* 022ADC3C */
u16 daNpc_Os_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022ADC3C, u16, this, pMsgNo);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    if (*pMsgNo == 0) {
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
    } else if (*pMsgNo == 0xEF4) {
        os_onEventBit(0x2510);
        status = 0x10;
    }
    return status;
}
VERIFY(0x022ADC3C, &daNpc_Os_c::next_msgStatus);

/* 022ADC88 */
BOOL daNpc_Os_c::talk() {
    WWHD_FUNC(0x022ADC88, BOOL, this);
    u32 mgr = os_msgMgr();
    u16 status = (u16)gabi::call<u32>(0x025F795C, mgr);
    dComIfGp_get(); /* HD: dead call */
    if (status == 0xE /* MSG_DISPLAYED */) {
        u16 next = next_msgStatus(&field_0x780);
        gabi::call(0x025F74D0, mgr, (u32)next);
        if (gabi::call<s32>(0x025F795C, mgr) == 0xF /* MSG_CONTINUES */) {
            gabi::call(0x025F7DB0, mgr, (u32)field_0x780, 0);
        }
    } else if (status != 0x15 && status != 6 && status == 0x12 /* BOX_CLOSED */) {
        gabi::call(0x025F74D0, mgr, 0x13); /* MSG_DESTROYED */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022ADC88, &daNpc_Os_c::talk);

/* 022ADD68 (unnamed) */
BOOL daNpc_Os_c::actionMsgEndEvent(int) {
    WWHD_FUNC(0x022ADD68, BOOL, this, (u32)0);
    return talk();
}
VERIFY(0x022ADD68, &daNpc_Os_c::actionMsgEndEvent);

/* 022ADD6C */
void daNpc_Os_c::initialSwitchOnEvent(int) {
    WWHD_FUNC(0x022ADD6C, void, this, (u32)0);
    s8 roomNo = current.roomNo;
    if (roomNo == 7 && !gabi::call<BOOL>(0x025BA0C0, gabi::load<u32>(0x101F84DC) + 0x20, (u32)field_0x794, 7) /* isSwitch */) {
        gabi::call(0x025B9E38, gabi::load<u32>(0x101F84DC) + 0x20, (u32)field_0x794, 7); /* onSwitch */
    }
}
VERIFY(0x022ADD6C, &daNpc_Os_c::initialSwitchOnEvent);

/* 022ADDE0 */
void daNpc_Os_c::initialNextEvent(int staffIdx) {
    WWHD_FUNC(0x022ADDE0, void, this, staffIdx);
    if (evmng_getMyP(staffIdx, 0x1001F698 /* "SE" */, 3)) {
        s32 reverb = dComIfGp_getReverb(current.roomNo); /* fopAcM_seStartCurrent */
        mDoAud_seStart(0x58FE /* JA_SE_OBJ_OSTATUE_PUT */, &current.pos, 0, reverb);
    }
    if (argument == 0) {
        field_0x7A5 = 2;
    } else if (argument == 1) {
        field_0x7A5 = 4;
    } else if (argument == 2) {
        field_0x7A5 = 6;
    }
}
VERIFY(0x022ADDE0, &daNpc_Os_c::initialNextEvent);

/* 022ADE94 */
void daNpc_Os_c::initialSaveEvent(int) {
    WWHD_FUNC(0x022ADE94, void, this, (u32)0);
    home.pos.copy(current.pos);
    s8 roomNo = current.roomNo;
    s16 angle = home.angle.y;
    s8 num = gabi::call<s8>(0x022AA42C, this); /* getRestartNumber() */
    /* dComIfGs_setRestartOption(&current.pos, home.angle.y, roomNo, num) (HD inline: restart + priest) */
    gabi::call(0x025B9834, gabi::load<u32>(0x101F84DC) + 0x1148, (s32)num, &current.pos, angle, (s32)roomNo);
    gabi::call(0x025B8880, gabi::load<u32>(0x101F84DC) + 0x1CC, (u32)(u8)num, &current.pos, angle, (s32)roomNo);
}
VERIFY(0x022ADE94, &daNpc_Os_c::initialSaveEvent);

/* 022ADF24 (unnamed, HD virtual): wait/walk player action while not on the ground, or the throw NPC action */
static inline bool os_isPmf(ProcFunc_l* p, u32 f) { return p->i == -1 && (p->i == 0 || (p->d == 0 && p->f == f)); }
static BOOL os_022ADF24(daNpc_Os_c* t) {
    WWHD_FUNC(0x022ADF24, u8, t);
    if ((os_isPmf(&t->mPlayerAction, 0x022AFCE0 /* waitPlayerAction */) || os_isPmf(&t->mPlayerAction, 0x022AFF94 /* walkPlayerAction */)) &&
        !(t->mAcch.m_flags & 0x20 /* ChkGroundHit */)) {
        return TRUE;
    }
    return os_isPmf(&t->mNpcAction, 0x022AEF70 /* throwNpcAction */) ? TRUE : FALSE;
}
VERIFY(0x022ADF24, os_022ADF24);

/* 022ADFCC (unnamed, HD virtual): chkNpcAction(&daNpc_Os_c::carryNpcAction) */
static BOOL os_022ADFCC(daNpc_Os_c* t) {
    WWHD_FUNC(0x022ADFCC, BOOL, t);
    return os_isPmf(&t->mNpcAction, 0x022AECD4 /* carryNpcAction */) ? TRUE : FALSE;
}
VERIFY(0x022ADFCC, os_022ADFCC);

/* 022AE014 (param_1 by pointer to the caller's copy) */
BOOL daNpc_Os_c::chkAttention(cXyz* param_1, s16 param_2) {
    WWHD_FUNC(0x022AE014, u8, this, param_1, param_2);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = gabi::load<f32>(OS_L_HIO + NPC_mMaxAttnDistXZ);
    s32 maxAngle = gabi::load<s16>(OS_L_HIO + NPC_mMaxAttnAngleY);
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 angle = cM_atan2s(dx, dz);
    if (field_0x7A4) {
        maxDist = maxDist + 40.0f;
        maxAngle += 0x71C;
    }
    angle = (s16)(angle - param_2);
    s32 a = angle < 0 ? -angle : angle;
    BOOL ret = FALSE;
    if (maxAngle > a && maxDist > dist) {
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x022AE014, &daNpc_Os_c::chkAttention);

/* 022AE140 */
bool daNpc_Os_c::chkArea(cXyz* param_1) {
    WWHD_FUNC(0x022AE140, bool, this, param_1);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = gabi::load<f32>(OS_L_HIO + NPC_mMaxAttnDistXZ);
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    maxDist = maxDist + 40.0f;
    return maxDist > dist;
}
VERIFY(0x022AE140, &daNpc_Os_c::chkArea);

/* 022AE1CC (unnamed) */
u32 daNpc_Os_c::getMsg() {
    WWHD_FUNC(0x022AE1CC, u32, this);
    return 0;
}
VERIFY(0x022AE1CC, &daNpc_Os_c::getMsg);

/* 022AE1D4 */
void daNpc_Os_c::lookBack(int param_1, int param_2, int param_3) {
    WWHD_FUNC(0x022AE1D4, void, this, param_1, param_2, param_3);
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    u32 dstPos = 0;
    s16 targetY = shape_angle.y;
    if (gabi::load<u8>(gabi::ea(&mJntCtrl) + 0xA) != 0) { /* mJntCtrl.trnChk() */
        cLib_addCalcAngleS2(&field_0x798, gabi::load<s16>(OS_L_HIO + NPC_mMaxHeadTurnVel), 4, 0x800);
    } else {
        field_0x798 = 0;
    }
    gabi::Local<cXyz> eye, dst, temp;
    if (param_1) {
        gabi::call(0x0259D54C, eye.get(), gabi::load<f32>(OS_L_HIO + NPC_m04)); /* dNpc_playerEyePos(m04) */
        dst->copy(*eye);
        tx = current.pos.x;
        ty = eyePos.y;
        tz = current.pos.z;
        dstPos = gabi::ea(dst.get());
    }
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    gabi::call(0x0259DED0, &mJntCtrl, &shape_angle.y, dstPos, temp.get(), targetY, (s32)(s16)field_0x798, param_3 != 0 ? 1 : 0);
}
VERIFY(0x022AE1D4, &daNpc_Os_c::lookBack);

/* 022AE304 */
s32 daNpc_Os_c::wallHitCheck() {
    WWHD_FUNC(0x022AE304, s32, this);
    if (mAcch.m_flags & 0x10 /* ChkWallHit */) {
        for (int i = 0; i < 2; i++) {
            if (gabi::load<u32>(gabi::ea(&mAcchCir[i]) + 0x10) & 2) { /* mAcchCir[i].ChkWallHit() */
                return i;
            }
        }
    }
    return -1;
}
VERIFY(0x022AE304, &daNpc_Os_c::wallHitCheck);

/* 022AE340 */
s16 daNpc_Os_c::getStickAngY() {
    WWHD_FUNC(0x022AE340, s16, this);
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    s32 stick = gabi::call<s16>(0x02007A1C, 0) + 0x8000; /* CPad_GET_STICK_ANGLE(0) + 0x8000 */
    return (s16)(gabi::call<s16>(0x024F8018, camera) /* dCam_getControledAngleY */ + stick);
}
VERIFY(0x022AE340, &daNpc_Os_c::getStickAngY);

/* 022AE394 */
s8 daNpc_Os_c::getWakeupOrderEventNum() {
    WWHD_FUNC(0x022AE394, s8, this);
    if (argument == 0) return 1;
    if (argument == 1) return 3;
    if (argument == 2) return 5;
    return -1;
}
VERIFY(0x022AE394, &daNpc_Os_c::getWakeupOrderEventNum);

/* 022AE3CC */
int daNpc_Os_c::calcStickPos(s16 param_1, cXyz* param_2) {
    WWHD_FUNC(0x022AE3CC, int, this, param_1, param_2);
    u32 att = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    u32 attList = gabi::call<u32>(0x024EE058, att, 0); /* GetLockonList(0) */
    bool lockon = true; /* attention.Lockon() */
    if (!gabi::call<BOOL>(0x024EDFCC, att) /* LockonTruth */ && !((gabi::load<u32>(att + 0x20) >> 29) & 1)) {
        lockon = false;
    }
    int ret;
    if (!lockon) {
        ret = 0;
    } else {
        ret = gabi::call<BOOL>(0x024EDFCC, att) ? 1 : -1;
    }
    if (attList == 0) {
        attList = gabi::call<u32>(0x024EE020, att, 0); /* GetActionList(0) */
    }
    if (attList != 0) {
        fopAc_ac_c* a = gabi::call<fopAc_ac_c*>(0x024EBA14, attList); /* getActor() */
        param_2->copy(a->eyePos);
        return ret;
    }
    if (lockon) {
        param_1 = shape_angle.y;
    }
    u16 ang = (u16)param_1;
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    u32 e = 0x104A44F8 + ((ang >> 3) << 3); /* cM_ssin / cM_scos */
    f32 s = gabi::load<f32>(e);
    f32 c = gabi::load<f32>(e + 4);
    param_2->y = y;
    param_2->x = gabi::fmadds(100.0f, s, x);
    param_2->z = gabi::fmadds(100.0f, c, z);
    return ret;
}
VERIFY(0x022AE3CC, &daNpc_Os_c::calcStickPos);
