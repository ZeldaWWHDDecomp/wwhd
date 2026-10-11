/* WWHD double doors. */
#include "d/actor/d_a_mbdoor.h"
static MbdoorSaveData* sSaveData;
static void* sDemoManager;
static u32 sDispatchInitialized;
static MbdoorDispatch sDispatch;
static MbdoorDispatch sDispatchInitial;
static MbdoorDemoActions sDemoActions;
static MbdoorSinCosTable sSinCos;
enum DemoAction {
    ACT_WAIT,
    ACT_SET_START,
    ACT_SET_ANGLE,
    ACT_END,
    ACT_OPEN,
    ACT_STOP_OPEN,
    ACT_SET_GOAL,
    ACT_SET_GOAL2,
    ACT_ADJUSTMENT
};
enum State {
    STATE_WAIT,
    STATE_LOCK_WAIT,
    STATE_LOCK_OFF,
    STATE_LOCK_DEMO,
    STATE_CLOSE_WAIT,
    STATE_OPEN
};
// HD: float snapshots preserve matrix overlap and signaling-NaN behavior.
/* 021BF70C */
u8 daMbdoor_c::getShapeType() {
    return (mParameters >> 8) & 15;
}

/* 021BF718 */
const char* daMbdoor_c::getArcName() {
    // HD: callers and the verified return value require a guest archive pointer.
    return gabi::at<const char>(gabi::guest_addr(getShapeType() == 1 ? "Gbdoor" : "Mbdoor"));
}

/* 021BF754 */
u32 daMbdoor_c::getFuBdl() {
    return dRes_INDEX_MBDOOR_BDL_S_MBDFU_e;
}

/* 021BF75C */
u32 daMbdoor_c::getDzb() {
    return dRes_INDEX_MBDOOR_DZB_S_MBDFU_e;
}

/* 021BF764 */
u32 daMbdoor_c::getLBdl() {
    return dRes_INDEX_MBDOOR_BDL_S_MBD_L_e;
}

/* 021BF76C */
u32 daMbdoor_c::getRBdl() {
    return dRes_INDEX_MBDOOR_BDL_S_MBD_R_e;
}

/* 021BF774 */
u32 daMbdoor_c::getToBdl() {
    return dRes_INDEX_MBDOOR_BDL_S_MBDTO_e;
}

/* 021BF77C */
f32 daMbdoor_c::getROffset() {
    return getShapeType() == 1 ? 300.0f : 277.0f;
}

/* 021BF7C4 */
f32 daMbdoor_c::getLOffset() {
    return getShapeType() == 1 ? -300.0f : -255.0f;
}

/* 021BF80C */
f32 daMbdoor_c::getToOffset() {
    return getShapeType() == 1 ? 335.0f : 300.0f;
}

/* inlined into calcMtx */
static void copyMatrix(J3DModel* model) {
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}
/* 021BF854 */
void daMbdoor_c::calcMtx() {
    gabi::Local<cXyz> offset;
    gabi::Local<u8[16]> linkage;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), home.angle.y);
    copyMatrix(mpFuModel);
    offset->set(0.0f, 0.0f, -150.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &mGoalPos);
    offset->set(0.0f, 0.0f, -400.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &mGoal2Pos);
    if ((u8)field_0x2ad == 0) {
        copyMatrix(mpLModel);
        if ((s16)field_0x2b2 != 0) {
            f32 dx = getROffset();
            mDoMtx_stack_transM(dx, 0.0f, 0.0f);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), field_0x2b2);
            dx = getROffset();
            mDoMtx_stack_transM(-dx, 0.0f, 0.0f);
        }
        offset->set(80.0f, 0.0f, 75.0f);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &field_0x2c4);
    }
    copyMatrix(mpRModel);
    if ((u8)field_0x2ad == 1) {
        if ((s16)field_0x2b2 != 0) {
            f32 dx = getLOffset();
            mDoMtx_stack_transM(dx, 0.0f, 0.0f);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16) - (s16)field_0x2b2);
            dx = getLOffset();
            mDoMtx_stack_transM(-dx, 0.0f, 0.0f);
        }
        offset->set(-80.0f, 0.0f, 75.0f);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &field_0x2c4);
        copyMatrix(mpLModel);
    }
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), home.angle.y);
    if ((s16)field_0x2b0 != 0) {
        f32 dx = getToOffset();
        mDoMtx_stack_transM(dx, 231.0f, 0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), field_0x2b0);
        dx = getToOffset();
        mDoMtx_stack_transM(-dx, -231.0f, 0.0f);
    }
    copyMatrix(mpToModel);
}

/* inlined into CreateHeap */
static void* objectResource(const char* archive, s32 index) {
    gabi::Local<SafeString> name;
    gabi::Local<u8[16]> linkage;
    name->mStringTop = gabi::ea(archive);
    name->__vtbl = 0x10014770;
    return mbdoorGetResource(dComIfG_resControl(), name.get(), index);
}
/* 021BFC80 */
BOOL daMbdoor_c::CreateHeap() {
    const char* archive = getArcName();
    u32 index = getFuBdl();
    void* data = objectResource(archive, index);
    if (!data)
        JUT_ASSERT_fail("d_a_mbdoor.cpp", 198, "modelData != (0)");
    mpFuModel = mbdoorCreateModel(data, 0x80000, 0x11000022);
    if (!gabi::ea(mpFuModel))
        return FALSE;
    archive = getArcName();
    index = getLBdl();
    data = objectResource(archive, index);
    if (!data)
        JUT_ASSERT_fail("d_a_mbdoor.cpp", 209, "modelData != (0)");
    mpLModel = mbdoorCreateModel(data, 0x80000, 0x11000022);
    if (!gabi::ea(mpLModel))
        return FALSE;
    archive = getArcName();
    index = getRBdl();
    data = objectResource(archive, index);
    if (!data)
        JUT_ASSERT_fail("d_a_mbdoor.cpp", 220, "modelData != (0)");
    mpRModel = mbdoorCreateModel(data, 0x80000, 0x11000022);
    if (!gabi::ea(mpRModel))
        return FALSE;
    archive = getArcName();
    index = getToBdl();
    data = objectResource(archive, index);
    if (!data)
        JUT_ASSERT_fail("d_a_mbdoor.cpp", 231, "modelData != (0)");
    mpToModel = mbdoorCreateModel(data, 0x80000, 0x11000022);
    if (!gabi::ea(mpToModel))
        return FALSE;
    mpBgW = mbdoorCreateBgW((u32)0);
    if (!gabi::ea(mpBgW))
        return FALSE;
    archive = getArcName();
    index = getDzb();
    data = objectResource(archive, index);
    if (!data)
        return FALSE;
    calcMtx();
    J3DModel* model = mpFuModel;
    dBgW* bg = mpBgW;
    return mbdoorSetBackground(bg, data, 1, J3DModel_getBaseTRMtx(model)) ? FALSE : TRUE;
}

/* 021BFED4 */
static BOOL CheckCreateHeap(daMbdoor_c* a) {
    return a->CreateHeap();
}

/* 021BFED8 */
static BOOL daMbdoor_actionWait(daMbdoor_c* a) {
    a->calcMtx();
    return TRUE;
}

/* 021BFEFC */
u8 daMbdoor_c::getSwbit() {
    return (u8)mParameters;
}

/* 021BFF08 */
u8 daMbdoor_c::getType() {
    return (u16)(s16)home.angle.z & 63;
}

/* 021BFF14 */
BOOL daMbdoor_c::checkUnlock() {
    u8 sw = getSwbit(), type = getType();
    if (type == 0) {
        MbdoorSaveData* save = sSaveData;
        s8 room = current.roomNo;
        return mbdoorIsSwitch(&save->mSwitch, sw, room);
    }
    if (type == 1 && !mbdoorSearchEnemy((s8)current.roomNo)) {
        s32 timer = field_0x2bc;
        if (timer > 0) {
            field_0x2bc = timer - 1;
            return FALSE;
        }
        if (sw != 0xFF) {
            MbdoorSaveData* save = sSaveData;
            s8 room = current.roomNo;
            mbdoorOnSwitch(&save->mSwitch, sw, room);
        }
        return TRUE;
    }
    return FALSE;
}

static void orderEvent(daMbdoor_c* a, const char* name) {
    mbdoorOrderEvent(a, name, 1, 0xFFFF);
}
/* 021C001C */
static BOOL daMbdoor_actionLockWait(daMbdoor_c* a) {
    if (a->checkUnlock()) {
        a->mCurActionIdx = STATE_LOCK_OFF;
        orderEvent(a, "MBDOOR_STOP_OPEN");
    }
    return TRUE;
}

/* 021C0078 */
s32 daMbdoor_c::getDemoAction() {
    s32 staff = mEvtStaffId;
    auto manager = dComIfGp_getPEvtManager();
    return mbdoorGetMyActIdx(manager, staff, &sDemoActions, 9, 0, 0);
}

static void cutEnd(daMbdoor_c* a) {
    s32 staff = a->mEvtStaffId;
    auto manager = dComIfGp_getPEvtManager();
    mbdoorCutEnd(manager, staff);
}
static void sound(daMbdoor_c* a, u32 id) {
    s8 room = a->current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(id, &a->eyePos, 0, reverb);
}
static void setGoal(cXyz* goal) {
    auto manager = dComIfGp_getPEvtManager();
    mbdoorSetGoal(manager, goal);
}
static void setPlayerGoal(daPy_py_c* player, cXyz* goal, s16 angle) {
    player->setPlayerPosAndAngle(goal, angle);
}
/* 021C00C4 */
void daMbdoor_c::demoProc() {
    gabi::Local<be<s16>> angle;
    gabi::Local<cXyz> goal;
    gabi::Local<u8[16]> linkage;
    auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    daPy_py_c* player = static_cast<daPy_py_c*>(play->play.getPlayer(0));
    s32 action = getDemoAction(), staff = mEvtStaffId;
    auto manager = dComIfGp_getPEvtManager();
    if (mbdoorGetIsAdvance(manager, staff)) {
        switch (action) {
        case ACT_SET_START:
            calcMtx();
            goal->copy(field_0x2c4);
            setGoal(goal.get());
            break;
        case ACT_SET_ANGLE:
            *angle = (s16)((s16)current.angle.y + 0x7FFF);
            player->mDemo.setMoveAngle(*angle);
            break;
        case ACT_OPEN:
            sound(this, 0x2863);
            field_0x2b4 = 0;
            break;
        case ACT_STOP_OPEN:
            sound(this, 0x2864);
            field_0x2b4 = 0;
            field_0x2b6 = 1;
            break;
        case ACT_SET_GOAL:
            goal->copy(mGoalPos);
            setGoal(goal.get());
            break;
        case ACT_SET_GOAL2:
            goal->copy(mGoal2Pos);
            setGoal(goal.get());
            break;
        case ACT_ADJUSTMENT: {
            calcMtx();
            mAdjustmentTimer = 0;
            s32 id = mEvtStaffId;
            auto ev = dComIfGp_getPEvtManager();
            be<s32>* timer = mbdoorGetSubstance(ev, id, "Timer", 3);
            if (timer)
                mAdjustmentTimer = (s32)*timer;
            break;
        }
        }
    }
    switch (action) {
    case ACT_END:
        return;
    case ACT_OPEN: {
        s16 speed = field_0x2b4, openingAngle = field_0x2b2;
        if (speed < 250) {
            speed = (s16)(speed + 50);
            field_0x2b4 = speed;
        }
        s32 next = (s32)openingAngle - speed;
        if (next < -0x1C71) {
            field_0x2b2 = -0x1C71;
            cutEnd(this);
        } else
            field_0x2b2 = (s16)next;
        calcMtx();
        u8 side = field_0x2ad;
        s16 yaw = current.angle.y, door = field_0x2b2;
        *angle = (s16)(yaw + 0x7FFF + (side ? -(s32)door : (s32)door));
        setPlayerGoal(player, &field_0x2c4, *angle);
        return;
    }
    case ACT_STOP_OPEN: {
        if (!(u8)field_0x2b6) {
            cutEnd(this);
            return;
        }
        s16 speed = field_0x2b4, bar = field_0x2b0;
        if (speed < 400) {
            speed = (s16)(speed + 40);
            field_0x2b4 = speed;
        }
        s32 next = (s32)bar - speed;
        if (next < -0x3F65) {
            field_0x2b0 = -0x3F65;
            cutEnd(this);
            field_0x2b6 = 0;
            sound(this, 0x2865);
        } else
            field_0x2b0 = (s16)next;
        calcMtx();
        return;
    }
    case ACT_ADJUSTMENT: {
        s16 yaw = current.angle.y;
        *angle = (s16)player->shape_angle.y;
        mbdoorAddCalcAngle(angle.get(), (s16)(yaw + 0x7FFF), 10, 0x800);
        f32 px = (f32)player->current.pos.x, gx = field_0x2c4.x, gz = field_0x2c4.z;
        f32 ax = gx * 0.1f, az = gz * 0.1f;
        goal->x = px;
        goal->y = (f32)player->current.pos.y;
        f32 pz = (f32)player->current.pos.z;
        goal->x = gabi::fmadds(px, 0.9f, ax);
        goal->z = gabi::fmadds(pz, 0.9f, az);
        setPlayerGoal(player, goal.get(), *angle);
        s16 timer = mAdjustmentTimer;
        if (timer > 0)
            mAdjustmentTimer = (s16)(timer - 1);
        else
            cutEnd(this);
        return;
    }
    default:
        cutEnd(this);
        return;
    }
}

/* 021C05DC */
static BOOL daMbdoor_actionLockOff(daMbdoor_c* a) {
    if ((u16)a->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        auto manager = dComIfGp_getPEvtManager();
        a->mEvtStaffId = mbdoorGetStaffId(manager, "MBDOOR", 0, 0);
        a->demoProc();
        a->mCurActionIdx = STATE_LOCK_DEMO;
    } else
        orderEvent(a, "MBDOOR_STOP_OPEN");
    return TRUE;
}

/* 021C0668 */
static BOOL daMbdoor_actionLockDemo(daMbdoor_c* a) {
    auto manager = dComIfGp_getPEvtManager();
    if (mbdoorEndCheck(manager, "MBDOOR_STOP_OPEN")) {
        auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        play->play.mEvtCtrl.mEventFlag = (u16)play->play.mEvtCtrl.mEventFlag | 8;
        a->mCurActionIdx = STATE_CLOSE_WAIT;
    } else
        a->demoProc();
    return TRUE;
}

/* 021C06D8 */
BOOL daMbdoor_c::checkArea() {
    gabi::Local<cXyz> relative;
    gabi::Local<u8[16]> linkage;
    auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    daPy_py_c* player = static_cast<daPy_py_c*>(play->play.getPlayer(0));
    mbdoorSubtract(&player->current.pos, relative.get(), &home.pos);
    u16 yaw = current.angle.y;
    MbdoorSinCos* table = &sSinCos.mEntries[(u32)yaw >> 3];
    f32 x = relative->x, sine = (f32)table->mSin, z = relative->z, cosine = (f32)table->mCos;
    f32 depth = gabi::fmadds(z, cosine, x * sine);
    sine = (f32)table->mSin;
    f32 lateral = gabi::fmsubs(z, sine, x * cosine);
    if (depth > 160.0f || lateral > 200.0f || lateral < -200.0f)
        return FALSE;
    if (mbdoorSeenAngle(player, this) > 0x2000)
        return FALSE;
    field_0x2ad = lateral > 0.0f ? 1 : 0;
    return TRUE;
}

/* 021C07FC */
static BOOL daMbdoor_actionCloseWait(daMbdoor_c* a) {
    if ((u16)a->eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        auto manager = dComIfGp_getPEvtManager();
        a->mEvtStaffId = mbdoorGetStaffId(manager, "MBDOOR", 0, 0);
        a->demoProc();
        a->mCurActionIdx = STATE_OPEN;
        auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dBgW* bg = a->mpBgW;
        mbdoorRelease(&play->play.mBgS, bg);
        a->field_0x2ac = 0;
    } else if (a->checkArea()) {
        mbdoorSetEventName(&a->eventInfo, "MBDOOR_OPEN");
        a->eventInfo.mCondition = (u16)a->eventInfo.mCondition | dEvtCnd_CANDOOR_e;
    }
    return TRUE;
}

/* 021C08B0 */
static BOOL daMbdoor_actionOpen(daMbdoor_c* a) {
    a->demoProc();
    return TRUE;
}

/* 021C08D4 */
BOOL daMbdoor_c::draw() {
    auto env = dKy_getEnvlight();
    mbdoorSetLight(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel* model = mpFuModel;
    mbdoorApplyLight(env, model, &tevStr);
    mbdoorUpdateModel((J3DModel*)mpFuModel, 0);
    env = dKy_getEnvlight();
    model = mpLModel;
    mbdoorApplyLight(env, model, &tevStr);
    mbdoorUpdateModel((J3DModel*)mpLModel, 0);
    env = dKy_getEnvlight();
    model = mpRModel;
    mbdoorApplyLight(env, model, &tevStr);
    mbdoorUpdateModel((J3DModel*)mpRModel, 0);
    env = dKy_getEnvlight();
    model = mpToModel;
    mbdoorApplyLight(env, model, &tevStr);
    mbdoorUpdateModel((J3DModel*)mpToModel, 0);
    return TRUE;
}

/* 021C0984 */
BOOL daMbdoor_c::execute() {
    if (!(u32)sDispatchInitialized) {
        sDispatchInitialized = 1;
        mbdoorCopyDispatch(&sDispatch, &sDispatchInitial, 24);
    }
    u8 demo = (u8)demoActorID;
    MbdoorDemoActor* actor = nullptr;
    if (demo && demo <= 32) {
        void* manager = sDemoManager;
        if (!manager) {
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            manager = sDemoManager;
        }
        actor = mbdoorGetDemoActor(manager, demo);
    }
    if (actor) {
        field_0x2ad = 0;
        if ((u16)actor->mEnabled & 8) {
            field_0x2b2 = (s16)actor->mRotate.y;
            field_0x2b0 = (s16)actor->mRotate.z;
        }
        calcMtx();
    } else {
        gabi::call_ptr(sDispatch.mActions[(u8)mCurActionIdx].get(), this);
    }
    return TRUE;
}

/* 021C0A98 */
static BOOL daMbdoor_IsDelete(daMbdoor_c* a) {
    return TRUE;
}

/* 021C0AA0 */
static BOOL daMbdoor_Delete(daMbdoor_c* a) {
    if (gabi::ea(a->heap)) {
        auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dBgW* bg = a->mpBgW;
        mbdoorRelease(&play->play.mBgS, bg);
    }
    const char* archive = a->getArcName();
    dComIfG_resDelete(&a->mPhs, archive);
    return TRUE;
}

/* 021C0AFC */
BOOL daMbdoor_c::CreateInit() {
    u8 sw = getSwbit(), type = getType();
    auto play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dBgW* bg = mpBgW;
    if (mbdoorRegister(&play->play.mBgS, bg, this))
        JUT_ASSERT_fail("d_a_mbdoor.cpp", 334, "0");
    u8 room = current.roomNo;
    field_0x2ac = 1;
    tevStr.mRoomNo = room;
    bool locked = type == 2;
    if (type == 0 && sw != 0xFF) {
        MbdoorSaveData* save = sSaveData;
        s8 r = current.roomNo;
        locked = !mbdoorIsSwitch(&save->mSwitch, sw, r);
    }
    if (type == 1) {
        locked = sw == 0xFF;
        if (!locked) {
            MbdoorSaveData* save = sSaveData;
            s8 r = current.roomNo;
            locked = mbdoorIsSwitch(&save->mSwitch, sw, r) != 0;
        }
    }
    field_0x2b0 = locked ? 0 : -0x3F65;
    field_0x2b2 = 0;
    attention_info.flags = fopAc_Attn_ACTION_DOOR_e;
    field_0x2bc = 30;
    mCurActionIdx = locked ? STATE_LOCK_WAIT : STATE_CLOSE_WAIT;
    f32 attention = (f32)attention_info.position.y, eye = eyePos.y;
    attention_info.position.y = attention + 250.0f;
    eyePos.y = eye + 250.0f;
    calcMtx();
    mbdoorMoveBackground((dBgW*)mpBgW);
    u32 status = actor_status;
    actor_status = (status & ~63u) | 0x2B;
    return TRUE;
}

/* 021C0CD0 */
s32 daMbdoor_c::create() {
    const char* archive = getArcName();
    s32 phase = dComIfG_resLoad(&mPhs, archive);
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            mbdoorConstructActor(this);
            condition = actor_condition;
            __vtbl = 0x10014788;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x8200))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 021C0D9C */
static s32 daMbdoor_Create(daMbdoor_c* a) {
    return a->create();
}

// HD: compiler-generated and template code of this translation unit.
/* 021C0DA0 */
static void sinit() {
    sinit_header_statics_z(0x10464F84, 0x101BA348, 0x10464F90);
}

/* 021C0E34 */
static void stringDtor(void* self, u32 flags) {
    if (self && (flags & 1))
        mbdoorDeleteObject(self);
}

/* 021C0E48 */
static void actorDtor(daMbdoor_c* self, u32 flags) {
    if (self) {
        mbdoorDestroyActor(self, 0);
        if (flags & 1)
            mbdoorDeleteObject(self);
    }
}

/* 021C0E9C */
static void stringTerminate(void* self) {}

/* WWHD glue
data sSaveData 101F84DC
data sDemoManager 101D5FFC
data sDispatchInitialized 101FDADC
data sDispatch 101FDAE0
data sDispatchInitial 101BA2EC
data sDemoActions 101BA324
data sSinCos 104A44F8
ext 026066C4 void* mbdoorGetResource(void*, SafeString*, s32)
ext 025E38E0 J3DModel* mbdoorCreateModel(void*, u32, u32)
ext 024F23F4 dBgW* mbdoorCreateBgW(u32)
ext 0200A030 BOOL mbdoorSetBackground(dBgW*, void*, u32, Mtx34*)
ext 025BA0C0 BOOL mbdoorIsSwitch(void*, u32, s8)
ext 025D98E8 BOOL mbdoorSearchEnemy(s8)
ext 025B9E38 void mbdoorOnSwitch(void*, u32, s8)
ext 025D77DC void mbdoorOrderEvent(daMbdoor_c*, const char*, u32, u32)
ext 02542EDC s32 mbdoorGetMyActIdx(void*, s32, void*, u32, u32, u32)
ext 02543280 void mbdoorCutEnd(void*, s32)
ext 02543714 void mbdoorSetGoal(void*, cXyz*)
ext 025447C8 BOOL mbdoorGetIsAdvance(void*, s32)
ext 0254487C be<s32>* mbdoorGetSubstance(void*, s32, const char*, u32)
ext 0200F428 void mbdoorAddCalcAngle(be<s16>*, s16, s32, s16)
ext 02542D88 s32 mbdoorGetStaffId(void*, const char*, u32, u32)
ext 0254457C BOOL mbdoorEndCheck(void*, const char*)
ext 0201ADE0 void mbdoorSubtract(const cXyz*, cXyz*, const cXyz*)
ext 025D68A0 s32 mbdoorSeenAngle(daPy_py_c*, daMbdoor_c*)
ext 020087EC void mbdoorRelease(void*, dBgW*)
ext 0253E9B0 void mbdoorSetEventName(dEvt_info_c*, const char*)
ext 025626A4 void mbdoorSetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void mbdoorApplyLight(void*, J3DModel*, dKy_tevstr_c*)
ext 025E2DE0 void mbdoorUpdateModel(J3DModel*, u32)
ext C000A848 void mbdoorCopyDispatch(void*, void*, u32)
ext 02526E70 MbdoorDemoActor* mbdoorGetDemoActor(void*, u32)
ext 024EEA6C BOOL mbdoorRegister(void*, dBgW*, daMbdoor_c*)
ext 024F43DC void mbdoorMoveBackground(dBgW*)
ext 025D4ED0 void mbdoorConstructActor(daMbdoor_c*)
ext 025D63E8 BOOL mbdoorEntryHeap(daMbdoor_c*, void*, u32)
ext 0273AF40 void mbdoorDeleteObject(void*)
ext 025D50BC void mbdoorDestroyActor(daMbdoor_c*, s32)
str 10014748
str 10014750
str 10014798
str 100147A8
str 100147D4 in 021BFC80
str 100147E4
str 100147F8 in 021C001C
str 10014870
str 10014878 in 021C05DC
str 10014880 in 021C05DC
str 10014894 in 021C0668
str 100148B4 in 021C07FC
str 100148BC
str 100148CC
str 100148D0 in 021C0AFC
*/
