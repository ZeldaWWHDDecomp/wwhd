/**
 * d_a_npc_os_b.cpp (WWHD)
 * Servant of the Tower, part B: destructor, event functions, talk, attention/area checks,
 * lookBack, stick helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_b.h"

void osDestroyAcchCircle(void*, s32);
BOOL osWaitPlayerAction(daNpc_Os_c*, void*);
BOOL osWalkPlayerAction(daNpc_Os_c*, void*);
BOOL osThrowNpcAction(daNpc_Os_c*, void*);
BOOL osCarryNpcAction(daNpc_Os_c*, void*);
using namespace os_b;

void os_setAnm(daNpc_Os_c*, s32);
BOOL os_initBrkAnm(daNpc_Os_c*, u32, u32);
void os_endBeam(daNpc_Os_c*);
void osResetRoomHD(s32, s32);
void osDestroyCylinder(dCcD_Cyl*, s32);
void osDestroyStatus(dCcD_Stts*, s32);
void osDestroyArray(void*, s32, s32, u32, s32, s32);
void osDestroyNpc(daNpc_Os_c*, s32);
BOOL osStartShock(u32, s32, s32, cXyz*);
s16 osCalcTimer(be<s16>*);
u32 osAddCalcAngle(be<s16>*, s16, s16, s16, s16);
void os_walkProc(daNpc_Os_c*, f32, s16);
void os_setAttention(daNpc_Os_c*, u32);
void os_setFinish(daNpc_Os_c*);
void os_setNpcAction(daNpc_Os_c*, ProcFunc_l*, void*);
void osLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, u32, cXyz*, s16, s32, s32);
void osMakeBeam(daNpc_Os_c*, s32);
u32 osMessageSet(u32, u32, cXyz*);
u32 osMessageStatus(u32);
void osMessageSetStatus(u32, u32);
BOOL osIsSwitch(u8*, u32, s32);
void osOnSwitch(u8*, u32, s32);
s8 osRestartNumber(daNpc_Os_c*);
void osSetRestart(u8*, s32, cXyz*, s16, s32);
void osSetPriest(u8*, u32, cXyz*, s16, s32);
void osPlayerEyePos(cXyz*, f32);
s16 osStickAngle(s32);
s16 osCameraAngle(void*);
u32 osLockonList(Attention*, s32);
BOOL osLockonTruth(Attention*);
u32 osActionList(Attention*, s32);
fopAc_ac_c* osAttentionActor(u32);

// HD: external callbacks need registry entries until their owner source is linked.
static gabi::HostFn osDestroyAcchCircleBinding((const void*)&osDestroyAcchCircle, 0x022B050C);
static gabi::HostFn osWaitPlayerActionBinding((const void*)&osWaitPlayerAction, 0x022AFCE0);
static gabi::HostFn osWalkPlayerActionBinding((const void*)&osWalkPlayerAction, 0x022AFF94);
static gabi::HostFn osThrowNpcActionBinding((const void*)&osThrowNpcAction, 0x022AEF70);
static gabi::HostFn osCarryNpcActionBinding((const void*)&osCarryNpcAction, 0x022AECD4);

// HD: private views of TU-wide Os storage; the main actor remains its owner.
static daNpc_Os_HIO_c l_osHIO;
static s32 l_hio_counter;
static Smoke m_smoke;
static PlayerRooms m_playerRoom;
static s8 m_cattleRoomNo;
static s32 l_msgId;
static cXyz l_zero;
static ProcFunc_l l_finish02NpcAction;
static SinCosTable l_sincosTable;
static BrkTableEntry anmBrkTbl[8] = {{0, 0}, {0, 1}, {2, 2}, {3, 3}, {0, 4}, {0, 5}, {0, 6}, {4, 7}};
static u32 l_messageManager;

/* ---- functions ---- */

// HD: compact GHS descriptor comparisons retain the repeated selector reads.
static inline bool os_isPmf(ProcFunc_l* p, u32 f) {
    return p->i == -1 && (p->i == 0 || (p->d == 0 && p->f == f));
}

/* 022AD540 */
void daNpc_Os_c::setWakeup() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (argument == 0) {
        os_b::onEventBit(0x1780);
    } else if (argument == 1) {
        os_b::onEventBit(0x1740);
    } else if (argument == 2) {
        os_b::onEventBit(0x1720);
    }
}

/* 022AE394 */
s8 daNpc_Os_c::getWakeupOrderEventNum() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (argument == 0)
        return 1;
    if (argument == 1)
        return 3;
    if (argument == 2)
        return 5;
    return -1;
}

/* 022AE304 */
s32 daNpc_Os_c::wallHitCheck() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mAcch.m_flags & dBgS_Acch::WALL_HIT /* ChkWallHit */) {
        for (int i = 0; i < 2; i++) {
            if (u32(mAcchCir[i].m_flags) & dBgS_AcchCir::WALL_HIT) { /* mAcchCir[i].ChkWallHit() */
                return i;
            }
        }
    }
    return -1;
}

// HD: preserve the angle helper result in the return register.
/* 022AD668 */
u32 daNpc_Os_c::walkProc(f32 param_1, s16 param_2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 f = mPrevMorfFrame;
    if (f > f32(l_osHIO.field_0xB0) && f < f32(l_osHIO.field_0xAC)) {
        speedF = f32(l_osHIO.mOs2.field_0x08) * param_1;
    } else {
        speedF = 0.0f;
    }
    return osAddCalcAngle(&current.angle.y, param_2, s16(l_osHIO.mOs2.field_0x28),
                          s16(l_osHIO.mOs2.field_0x24),
                          s16(l_osHIO.mOs2.field_0x26)); /* cLib_addCalcAngleS */
}

/* 022AE340 */
s16 daNpc_Os_c::getStickAngY() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* camera = os_b::camera();        /* os_b::camera() */
    s32 stick = osStickAngle(0) + 0x8000; /* CPad_GET_STICK_ANGLE(0) + 0x8000 */
    return (s16)(osCameraAngle(camera) /* dCam_getControledAngleY */ + stick);
}

/* 022AE3CC */
int daNpc_Os_c::calcStickPos(s16 param_1, cXyz* param_2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    Attention* att = attention();       /* dComIfGp_getAttention() */
    u32 attList = osLockonList(att, 0); /* GetLockonList(0) */
    bool lockon = true;                 /* attention.Lockon() */
    if (!osLockonTruth(att) /* LockonTruth */ && !((u32(att->flags) >> 29) & 1)) {
        lockon = false;
    }
    int ret;
    if (!lockon) {
        ret = 0;
    } else {
        ret = osLockonTruth(att) ? 1 : -1;
    }
    if (attList == 0) {
        attList = osActionList(att, 0); /* GetActionList(0) */
    }
    if (attList != 0) {
        fopAc_ac_c* a = osAttentionActor(attList); /* getActor() */
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
    const SinCosTable::Entry* e = &l_sincosTable.m[ang >> 3]; /* cM_ssin / cM_scos */
    f32 s = f32(e->sin);
    f32 c = f32(e->cos);
    param_2->y = y;
    param_2->x = gabi::fmadds(100.0f, s, x);
    param_2->z = gabi::fmadds(100.0f, c, z);
    return ret;
}

/* 022AD360 */
void daNpc_Os_c::initialDefault(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 022AD364 */
BOOL daNpc_Os_c::actionDefault(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 022AD36C */
void daNpc_Os_c::initialWaitEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    field_0x7C0 = -1;

    cXyz* posData = (cXyz*)dComIfGp_evmng_getMySubstanceP(staffIdx, "pos", 1);
    if (posData) {
        f32 y = posData->y;
        f32 z = posData->z;
        f32 x = posData->x;
        current.pos.z = z;
        current.pos.y = y;
        current.pos.x = x;
    }

    be<s32>* angleData = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "angle", 3);
    if (angleData) {
        s16 angle = (s16)(s32)*angleData;
        shape_angle.y = angle;
        current.angle.y = angle;
    }

    if (dComIfGp_evmng_getMySubstanceP(staffIdx, "gravity", 3)) {
        field_0x784 |= STATUS_GRAVITY; /* onGravity */
        maxFallSpeed = -100.0f;
        gravity = f32(l_osHIO.field_0x8C);
    }

    be<s32>* quakeData = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "quake", 3);
    if (quakeData) {
        u32 play = dComIfGp_ea();
        gabi::Local<cXyz> pos;
        pos->x = 0.0f;
        pos->y = 1.0f;
        pos->z = 0.0f;
        osStartShock(play + PLAY_VIBRATION, (s32)*quakeData, -0x11, pos.get()); /* StartShock */
    }

    be<s32>* timerData = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "timer", 3);
    if (timerData) {
        field_0x7C0 = (s16)(s32)*timerData;
    }
}

/* 022AD4F0 */
BOOL daNpc_Os_c::actionWaitEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x7C0 < 0) {
        return TRUE;
    }
    return osCalcTimer(&field_0x7C0) == 0 ? TRUE : FALSE; /* cLib_calcTimer<s16> */
}

/* 022AD598 */
void daNpc_Os_c::initialWakeupEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (os_b::partner() == this) { /* os_b::partner() == this */
        setWakeup();
        maxFallSpeed = -100.0f;
        gravity = f32(l_osHIO.field_0x8C);
        mNoResetFlg1 &= ~u32(NPC_NOT_CHANGE); /* offNpcNotChange */
        os_initBrkAnm(this, 4, 1);
        field_0x7A1 = 0;
    }
}

/* 022AD614 */
BOOL daNpc_Os_c::actionWakeupEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (field_0x7A1) {
        os_initBrkAnm(this, 6, 1);
        return TRUE;
    }
    return FALSE;
}

/* 022AD660 */
void daNpc_Os_c::initialMoveEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    os_setAnm(this, 1);
}

/* 022AD720 */
BOOL daNpc_Os_c::actionMoveEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 value = 1.0f;
    be<f32>* data = (be<f32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "Stick", 0);
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
            mpMorf->mFrameCtrl.mRate = value + value; /* mpMorf->setPlaySpeed(value * 2) */
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, s16(l_osHIO.mOs2.field_0x28),
                               (s16)(s16(l_osHIO.mOs2.field_0x24) * 2),
                               (s16)(s16(l_osHIO.mOs2.field_0x26) * 2));
            s16 yrot = shape_angle.y;
            gabi::Local<cXyz> eye; /* cXyz::Zero (copy) */
            cXyz* z = &l_zero;
            f32 zx = z->x;
            f32 zz = z->z;
            f32 zy = z->y;
            eye->z = zz;
            eye->y = zy;
            eye->x = zx;
            osLookAtTarget(&mJntCtrl, &shape_angle.y, (u32)0, eye.get(), yrot, 0, 0); /* lookAtTarget */
        }
        os_setAttention(this, 1);
    }
    return TRUE;
}

/* 022AD89C */
void daNpc_Os_c::initialMoveEndEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* data = dComIfGp_evmng_getMySubstanceP(staffIdx, "Daiza", 3);
    if (data && mpPedestal) {
        current.pos.x = mpPedestal->current.pos.x;
        fopAc_ac_c* p = mpPedestal;
        current.pos.y = p->current.pos.y + 240.0f;
        current.pos.z = p->current.pos.z;
    }
    speedF = 0.0f;
    os_setAnm(this, 0);
}

/* 022AD940 */
void daNpc_Os_c::initialEndEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    os_setFinish(this);
    gabi::Local<be<u32>> temp;
    *temp = 0;
    gabi::Local<ProcFunc_l> fn;
    fn->d = l_finish02NpcAction.d;
    fn->i = l_finish02NpcAction.i;
    fn->f = l_finish02NpcAction.f; /* &daNpc_Os_c::finish02NpcAction */
    os_setNpcAction(this, fn, temp.get());
}

/* 022AD998 */
void daNpc_Os_c::initialTurnEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 022AD99C */
BOOL daNpc_Os_c::actionTurnEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* data = dComIfGp_evmng_getMySubstanceP(staffIdx, "Angle", 3);
    if (data != nullptr) {
        s16 target = lowHalf(data); /* (s16)*(int*)data */
        s16 temp = cLib_addCalcAngleS(&shape_angle.y, target, 0x1E, 0x2000, 0x800);
        current.angle.y = shape_angle.y;
        if (temp == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022ADAA4 */
void daNpc_Os_c::initialFinishEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* data = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "Type", 3);
    u32 value = 0;
    if (data) {
        value = *data;
    }
    setAnm_brkAnm(value);
    if (field_0x78C == 2 || field_0x78C == 4) {
        osMakeBeam(this, 1); /* makeBeam(1) */
    } else {
        os_endBeam(this);
    }
}

/* 022ADB4C */
BOOL daNpc_Os_c::actionFinishEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return field_0x7A1 != 0 ? TRUE : FALSE;
}

/* 022ADB60 */
void daNpc_Os_c::initialMsgSetEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_msgId = -1;
    be<u32>* data = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "MsgNo", 3);
    if (data) {
        field_0x780 = *data;
    }
}

/* 022ADC38 */
BOOL daNpc_Os_c::actionMsgSetEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return talk_init();
}

/* 022ADD68 */
BOOL daNpc_Os_c::actionMsgEndEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return talk();
}

/* 022ADD6C */
void daNpc_Os_c::initialSwitchOnEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 roomNo = current.roomNo;
    if (roomNo == 7 && !osIsSwitch(save()->savedata, (u32)field_0x794, 7) /* isSwitch */) {
        osOnSwitch(save()->savedata, (u32)field_0x794, 7); /* onSwitch */
    }
}

/* 022ADDE0 */
void daNpc_Os_c::initialNextEvent(int staffIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_evmng_getMySubstanceP(staffIdx, "SE", 3)) {
        s32 reverb = dComIfGp_getReverb(current.roomNo); /* fopAcM_seStartCurrent */
        mDoAud_seStart(JA_SE_OBJ_OSTATUE_PUT, &current.pos, 0, reverb);
    }
    if (argument == 0) {
        field_0x7A5 = 2;
    } else if (argument == 1) {
        field_0x7A5 = 4;
    } else if (argument == 2) {
        field_0x7A5 = 6;
    }
}

/* 022ADE94 */
void daNpc_Os_c::initialSaveEvent(int) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    home.pos.copy(current.pos);
    s8 roomNo = current.roomNo;
    s16 angle = home.angle.y;
    s8 num = osRestartNumber(this); /* getRestartNumber() */
    /* dComIfGs_setRestartOption(&current.pos, home.angle.y, roomNo, num) (HD inline: restart + priest) */
    osSetRestart(save()->restart, (s32)num, &current.pos, angle, (s32)roomNo);
    osSetPriest(save()->priest, (u32)(u8)num, &current.pos, angle, (s32)roomNo);
}

/* 022ADBCC */
BOOL daNpc_Os_c::talk_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = u32(l_messageManager);
    if (s32(l_msgId) == -1) {
        l_msgId = osMessageSet(mgr, (u32)field_0x780, &eyePos);
        return FALSE;
    }
    return TRUE; // HD: the manager owns the message; no actor search.
}

/* 022ADC88 */
BOOL daNpc_Os_c::talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = u32(l_messageManager);
    u16 status = (u16)osMessageStatus(mgr);
    dComIfGp_get(); // HD: retain the unused play accessor call.
    if (status == MSG_DISPLAYED_e /* MSG_DISPLAYED */) {
        u16 next = next_msgStatus(&field_0x780);
        osMessageSetStatus(mgr, (u32)next);
        if ((s32)osMessageStatus(mgr) == MSG_CONTINUES_e) {
            osMessageSet(mgr, (u32)field_0x780, nullptr);
        }
    } else if (status != MSG_OTHER_e && status != MSG_WAIT_e && status == MSG_BOX_CLOSED_e /* BOX_CLOSED */) {
        osMessageSetStatus(mgr, MSG_DESTROYED_e); /* MSG_DESTROYED */
        return TRUE;
    }
    return FALSE;
}

/* 022ADA3C */
void daNpc_Os_c::setAnm_brkAnm(int param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    BrkTableEntry* e = brkEntry(anmBrkTbl, param_1); /* anmBrkTbl[param_1] */
    os_setAnm(this, u8(e->unk_1));
    os_initBrkAnm(this, u8(e->unk_2), 1);
    field_0x7A1 = 0;
}

// HD: preserve the byte result without changing the shared GameCube BOOL declaration.
/* 022AE014 */
static u8 osCheckAttention(daNpc_Os_c* actor, cXyz* param_1, s16 param_2) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = f32(l_osHIO.mNpc.mMaxAttnDistXZ);
    s32 maxAngle = s16(l_osHIO.mNpc.mMaxAttnAngleY);
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 angle = cM_atan2s(dx, dz);
    if (actor->field_0x7A4) {
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

/* 022AE140 */
bool daNpc_Os_c::chkArea(cXyz* param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = f32(l_osHIO.mNpc.mMaxAttnDistXZ);
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    maxDist = maxDist + 40.0f;
    return maxDist > dist;
}

/* 022ADC3C */
u16 daNpc_Os_c::next_msgStatus(be<u32>* pMsgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 status = MSG_CONTINUES_e; /* fopMsgStts_MSG_CONTINUES_e */
    if (*pMsgNo == 0) {
        status = MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
    } else if (*pMsgNo == 0xEF4) {
        os_b::onEventBit(0x2510);
        status = MSG_ENDS_e;
    }
    return status;
}

/* 022AE1CC */
u32 daNpc_Os_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 022AD6CC */
void daNpc_Os_c::setAttention(bool param_1) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!param_1 && field_0x7A3 >= 2) {
        return;
    }
    eyePos.x = field_0x748.x;
    eyePos.y = field_0x748.y;
    eyePos.z = field_0x748.z;
    cXyz* att = &attention_info.position;
    f32 y = current.pos.y + f32(l_osHIO.mNpc.mAttnYOffset);
    att->z = current.pos.z;
    att->x = current.pos.x;
    att->y = y;
}

/* 022AE1D4 */
void daNpc_Os_c::lookBack(int param_1, int param_2, int param_3) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    u32 dstPos = 0;
    s16 targetY = shape_angle.y;
    if (mJntCtrl.mbTrn != 0) { /* mJntCtrl.trnChk() */
        cLib_addCalcAngleS2(&field_0x798, s16(l_osHIO.mNpc.mMaxHeadTurnVel), 4, 0x800);
    } else {
        field_0x798 = 0;
    }
    gabi::Local<cXyz> eye, dst, temp;
    if (param_1) {
        osPlayerEyePos(eye.get(), f32(l_osHIO.mNpc.m04)); /* dNpc_playerEyePos(m04) */
        dst->copy(*eye);
        tx = current.pos.x;
        ty = eyePos.y;
        tz = current.pos.z;
        dstPos = gabi::ea(dst.get());
    }
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    osLookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, temp.get(), targetY, (s32)(s16)field_0x798,
                   param_3 != 0 ? 1 : 0);
}

// HD: compiler-generated and virtual adapter entries retain their verified ABI.
// HD: deleting destructor, including callback teardown and trailing delete flag.
/* 022AD218 */
static void daNpc_Os_c_dtor(daNpc_Os_c* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        return;
    }
    i_this->__vtbl = OS_VTBL;
    dComIfG_resDelete(&i_this->mPhs, "Os");
    if (i_this->heap.get() != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    os_endBeam(i_this);
    removeSmoke(&m_smoke); /* m_smoke.remove() */
    s32 cnt = s32(l_hio_counter);
    bool del = true;
    if (cnt != 0) {
        cnt = s32(u32(cnt) - 1);
        l_hio_counter = cnt;
        del = cnt <= 0;
    }
    if (del) {
        s8 no = s8(l_osHIO.mNo);
        if (no >= 0) {
            mDoHIO_deleteChild(no);
            l_osHIO.mNo = -1;
        }
    }
    offPlayerRoom(&m_playerRoom, i_this->argument); /* offPlayerRoom(argument) */
    m_cattleRoomNo = -1;
    osResetRoomHD(0, (s32)i_this->argument); // HD: clear the room state in the additional HD subsystem.
    osDestroyCylinder(&i_this->mCyl, 2);     /* dCcD_Cyl::~dCcD_Cyl */
    osDestroyStatus(&i_this->mStts, 2);      /* dCcD_Stts::~dCcD_Stts */
    osDestroyArray(&i_this->mAcchCir, 2, 0x40, gabi::guest_fn((const void*)&osDestroyAcchCircle), 0,
                   0);       /* __destroy_arr(mAcchCir, ~dBgS_AcchCir) */
    osDestroyNpc(i_this, 0); /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 022ADF24 */
static u8 osCheckAirbornePlayerAction(daNpc_Os_c* t) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((os_isPmf(&t->mPlayerAction,
                  gabi::guest_fn((const void*)&osWaitPlayerAction) /* waitPlayerAction */) ||
         os_isPmf(&t->mPlayerAction,
                  gabi::guest_fn((const void*)&osWalkPlayerAction) /* walkPlayerAction */)) &&
        !(t->mAcch.m_flags & dBgS_Acch::GROUND_HIT /* ChkGroundHit */)) {
        return TRUE;
    }
    return os_isPmf(&t->mNpcAction, gabi::guest_fn((const void*)&osThrowNpcAction) /* throwNpcAction */)
               ? TRUE
               : FALSE;
}

/* 022ADFCC */
static BOOL osCheckCarryNpcAction(daNpc_Os_c* t) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return os_isPmf(&t->mNpcAction, gabi::guest_fn((const void*)&osCarryNpcAction) /* carryNpcAction */)
               ? TRUE
               : FALSE;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 1001F640 "Os"
str 1001F650 "pos"
str 1001F654 "angle"
str 1001F648 "gravity"
str 1001F65C "quake"
str 1001F664 "timer"
str 1001F670 "Stick"
str 1001F678 "Daiza"
str 1001F680 "Angle"
str 1001F688 "Type"
str 1001F690 "MsgNo"
str 1001F698 "SE"
data l_osHIO 10467E44
data l_hio_counter 10467DEC
data m_smoke 10475440
data m_playerRoom 101D5F4B
data m_cattleRoomNo 101D5F46
data l_msgId 10467DF0
data l_zero 101FFBA8
data l_finish02NpcAction 1001F298
data l_sincosTable 104A44F8
data anmBrkTbl 101C2B1C
data l_messageManager 101F4B5C
ext 022AA88C void os_setAnm(daNpc_Os_c*, s32)
ext 022A9F4C BOOL os_initBrkAnm(daNpc_Os_c*, u32, u32)
ext 022AB28C void os_endBeam(daNpc_Os_c*)
ext 0268A8DC void osResetRoomHD(s32, s32)
ext 02515A70 void osDestroyCylinder(dCcD_Cyl*, s32)
ext 02515860 void osDestroyStatus(dCcD_Stts*, s32)
ext 028F0164 void osDestroyArray(void*, s32, s32, u32, s32, s32)
ext 0244513C void osDestroyNpc(daNpc_Os_c*, s32)
ext 025CB374 BOOL osStartShock(u32, s32, s32, cXyz*)
ext 02055B64 s16 osCalcTimer(be<s16>*)
ext 0200F378 u32 osAddCalcAngle(be<s16>*, s16, s16, s16, s16)
ext 022AD668 void os_walkProc(daNpc_Os_c*, f32, s16)
ext 022AD6CC void os_setAttention(daNpc_Os_c*, u32)
ext 022AB618 void os_setFinish(daNpc_Os_c*)
ext 022AA778 void os_setNpcAction(daNpc_Os_c*, ProcFunc_l*, void*)
ext 0259DED0 void osLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, u32, cXyz*, s16, s32, s32)
ext 022ABF7C void osMakeBeam(daNpc_Os_c*, s32)
ext 025F7DB0 u32 osMessageSet(u32, u32, cXyz*)
ext 025F795C u32 osMessageStatus(u32)
ext 025F74D0 void osMessageSetStatus(u32, u32)
ext 025BA0C0 BOOL osIsSwitch(u8*, u32, s32)
ext 025B9E38 void osOnSwitch(u8*, u32, s32)
ext 022AA42C s8 osRestartNumber(daNpc_Os_c*)
ext 025B9834 void osSetRestart(u8*, s32, cXyz*, s16, s32)
ext 025B8880 void osSetPriest(u8*, u32, cXyz*, s16, s32)
ext 0259D54C void osPlayerEyePos(cXyz*, f32)
ext 02007A1C s16 osStickAngle(s32)
ext 024F8018 s16 osCameraAngle(void*)
ext 024EE058 u32 osLockonList(Attention*, s32)
ext 024EDFCC BOOL osLockonTruth(Attention*)
ext 024EE020 u32 osActionList(Attention*, s32)
ext 024EBA14 fopAc_ac_c* osAttentionActor(u32)
ext 022B050C void osDestroyAcchCircle(void*,s32)
ext 022AFCE0 BOOL osWaitPlayerAction(daNpc_Os_c*,void*)
ext 022AFF94 BOOL osWalkPlayerAction(daNpc_Os_c*,void*)
ext 022AEF70 BOOL osThrowNpcAction(daNpc_Os_c*,void*)
ext 022AECD4 BOOL osCarryNpcAction(daNpc_Os_c*,void*)
*/
// clang-format on
