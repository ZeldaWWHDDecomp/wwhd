/* d_a_door12.cpp: WWHD double door; based on zeldaret/tww.
 * HD: enlarged key lock, singleton accessors, double-door animation and event ordering. */
#include "d/actor/d_a_door12.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

const gc_tu_data gc_tu = {.safeString = 0x1000DF90};
u8 door12Arg1(daDoor12_c*);
u8 door12Type(daDoor12_c*);
u8 door12Swbit(daDoor12_c*);
u8 door12Swbit2(daDoor12_c*);
u8 door12FRoomNo(daDoor12_c*);
u8 door12BRoomNo(daDoor12_c*);
void door12OpenInitCom(daDoor12_c*, s32);
void door12OpenProcCom(daDoor12_c*);
void door12CloseEndCom(daDoor12_c*);
void door12Shock(void*, u32, s32, cXyz*);
s32 door12DemoAction(daDoor12_c*);
void door12SetGoal(daDoor12_c*);
void door12KeyInit(Door12Key*, daDoor12_c*);
void door12PlayerAngle(daDoor12_c*, s32);
BOOL door12KeyProc(Door12Key*);
void door12OnTmpBit(void*, u32);
BOOL door12CheckStartDemo(Door12EventManager*);
void door12CancelStartDemo(void*);
BOOL door12RoomDisplayed(void*, u32);
u32 door12RoomEnemy(s8);
u32 door12SearchActor(void*, be<s32>*);
BOOL door12DungeonItem(void*, u32);
BOOL door12CheckArea(daDoor12_c*, f32, f32, f32);
void door12InitOpenDemo(daDoor12_c*, s32);
void door12KeyOn(Door12Key*);
void door12KeyOff(Door12Key*);
BOOL door12DrawCheck(daDoor12_c*, s32);
s32 door12CheckExecute(daDoor12_c*);
void door12StartDemo(daDoor12_c*);
void door12KeyResDelete(Door12Key*);
void door12InitProc(daDoor12_c*, u32);
void door12SetType(daDoor12_c*, u32);
void door12OffTmpBit(void*, u32);
cPhs_State door12KeyResLoad(Door12Key*);
void door12ConstructActor(daDoor12_c*);
void door12ConstructKey(Door12Key*);
void door12ConstructStop(Door12StopBars*);
void door12DestroyMatrixCalc(void*, s32);
void door12DestroyActor(daDoor12_c*, s32);
Door12GameInfo* door12GameInfo();
static Door12ShapeTable sShapeTable;
static Door12ModelTable sLeftResources;
static Door12ModelTable sRightResources;
static Door12RoomTable sRooms;
static Door12Save* sSave;
static u8 sMapSendStop;
static u8 sStayRoom;
static u32 sDrawList;
static u32 sXluDrawList;
static u32 sActionGuard;
static Door12ActionTable sActions;
static Door12ActionTable sActionPrototype;
static u8 sHeaderStatics;
static u8 sHeaderPrototype;
static u8 sActorVtable;
static u8 sSearchById;

static BOOL CheckCreateHeap(daDoor12_c*);
/* 02128150 */
s32 daDoor12_c::chkMakeKey() {
    u8 type = door12Type(this);
    return type == 1 ? 1 : type == 3 ? 2 : 0;
}
/* 021295D0 */
void daDoor12_c::setKey() {
    if (chkMakeKey() == 1) {
        u8 sw = door12Swbit(this);
        if (!dComIfGs_isSwitch(sw, -1)) {
            door12KeyOn(&mKeyLock);
            return;
        }
    }
    if (chkMakeKey() == 2 && door12Swbit(this) != 255) {
        if (door12Swbit(this) >= 128) {
            door12KeyOn(&mKeyLock);
            return;
        }
        u8 sw = door12Swbit(this);
        if (!dComIfGs_isSwitch(sw, -1)) {
            door12KeyOn(&mKeyLock);
            return;
        }
    }
    door12KeyOff(&mKeyLock);
}
/* 0212819C */
BOOL daDoor12_c::chkMakeStop() {
    if (door12Swbit2(this) != 255)
        return TRUE;
    if (chkMakeKey() != 0)
        return FALSE;
    return door12Swbit(this) != 255;
}
/* 0212854C */
s32 daDoor12_c::chkStopF() {
    u8 type = door12Type(this), sw = door12Swbit(this), room = door12FRoomNo(this);
    if (sw == 255 || (type != 0 && type != 2))
        return 0;
    dComIfGp_ea();
    if (!(sRooms.mRooms[room].mFlags & FLAG_OPEN))
        return -1;
    return dComIfGs_isSwitch(sw, room) == 0;
}
/* 02128638 */
s32 daDoor12_c::chkStopB() {
    u8 sw = door12Swbit2(this), room = door12BRoomNo(this);
    if (sw == 255)
        return 0;
    dComIfGp_ea();
    if (!(sRooms.mRooms[room].mFlags & FLAG_OPEN))
        return -1;
    return dComIfGs_isSwitch(sw, room) == 0;
}
/* 02128708 */
void daDoor12_c::setStop() {
    if (chkMakeStop() && mStopBars.mpModel != nullptr) {
        u8 front = mFrontCheck;
        mStopBars.mFrontCheck = front;
        if (front == 0) {
            mStopBars.mbEnabled = chkStopF();
            mStopBars.mA = chkStopB();
        } else {
            mStopBars.mbEnabled = chkStopB();
            mStopBars.mA = chkStopF();
        }
        mStopBars.mOffsY = 0.0f;
    }
}
/* 02128F20 */
BOOL daDoor12_c::chkStopOpen() {
    u8 type = door12Type(this), sw, room;
    if (mFrontCheck == 0) {
        sw = door12Swbit(this);
        room = door12FRoomNo(this);
    } else {
        sw = door12Swbit2(this);
        room = door12BRoomNo(this);
    }
    if (mFrontCheck == 0 && type == 2) {
        auto* play = door12GameInfo();
        // HD: an event blocks the enemy countdown only while its timer is nonzero.
        if (play->mEvent.mMode != 0 && m2A1 != 0)
            return FALSE;
        play = door12GameInfo();
        if (door12RoomDisplayed(play->mRoomControl, room) && door12RoomEnemy((s8)room) == 0) {
            u8 timer = m2A1;
            if (timer) {
                m2A1 = timer - 1;
                return FALSE;
            }
            if (sw != 255)
                dComIfGs_onSwitch(sw, room);
            return TRUE;
        }
        m2A1 = 65;
        return FALSE;
    }
    return sw != 255 && dComIfGs_isSwitch(sw, room);
}
/* 021290B4 */
void daDoor12_c::setStopDemo() { m2C6 = mFrontCheck != 0; }
/* 021290C8 */
BOOL daDoor12_c::chkStopClose() {
    u8 type = door12Type(this);
    if (mStopBars.mpModel == nullptr || type == 3)
        return FALSE;
    u8 sw, room;
    if (mFrontCheck == 0) {
        if (type == 2)
            return FALSE;
        sw = door12Swbit(this);
        room = door12FRoomNo(this);
    } else {
        sw = door12Swbit2(this);
        room = door12BRoomNo(this);
    }
    return sw != 255 && !dComIfGs_isSwitch(sw, room);
}
/* 0212842C */
static BOOL CheckCreateHeap(daDoor12_c* p) { return p->CreateHeap(); }
/* 0212820C */
BOOL daDoor12_c::CreateHeap() {
    const char* arc = getArcName();
    s32 idx = getBdlLf();
    auto* data = (J3DModelData*)dComIfG_getObjectRes(arc, idx, gc_tu.safeString);
    if (!data)
        JUT_ASSERT_fail("d_a_door12.cpp", 0x16A, "modelData != (0)");
    mpLeftModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (mpLeftModel == nullptr)
        return FALSE;
    arc = getArcName();
    idx = getBdlRt();
    data = (J3DModelData*)dComIfG_getObjectRes(arc, idx, gc_tu.safeString);
    if (!data)
        JUT_ASSERT_fail("d_a_door12.cpp", 0x173, "modelData != (0)");
    mpRightModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (mpRightModel == nullptr)
        return FALSE;
    mpBgw = new_dBgW();
    if (mpBgw == nullptr)
        return FALSE;
    arc = getArcName();
    idx = getDzb();
    auto* bg = (cBgD_t*)dComIfG_getObjectRes(arc, idx, gc_tu.safeString);
    if (!bg)
        return FALSE;
    calcMtx();
    J3DModel* left = mpLeftModel;
    dBgW* w = mpBgw;
    Mtx34* mtx = left ? &left->mBaseTransformMtx : nullptr;
    if (cBgW_Set(w, bg, 1, mtx))
        return FALSE;
    s32 key = chkMakeKey();
    if (key == 1 && !mKeyLock.create(0))
        return FALSE;
    if (key == 2 && !mKeyLock.create(1))
        return FALSE;
    if (chkMakeStop() && !mStopBars.create())
        return FALSE;
    mKeyLock.calcMtx(this);
    mStopBars.calcMtx(this);
    return TRUE;
}
/* 02127E10 */
s32 daDoor12_c::getShapeType() {
    u32 arg = door12Arg1(this);
    return arg >= 8 && arg <= 12 ? sShapeTable.mShapes[arg] : 0;
}
/* 02127E5C */
const char* daDoor12_c::getArcName() {
    return getShapeType() < 3 ? gabi::at<char>(gabi::guest_addr("door12"))
                              : gabi::at<char>(gabi::guest_addr("door13"));
}
/* 02127EA0 */
s32 daDoor12_c::getBdlLf() { return sLeftResources.mResources[getShapeType()]; }
/* 02127F18 */
s32 daDoor12_c::getBdlRt() { return sRightResources.mResources[getShapeType()]; }
/* 02127ED0 */
s32 daDoor12_c::getDzb() {
    s32 shape = getShapeType();
    return shape == SHAPE_BOSS || shape == SHAPE_BOSS_WIDE ? DZB_BOSS : DZB_NORMAL;
}
/* 02127F48 */
f32 daDoor12_c::openWide() {
    u32 shape = getShapeType();
    return shape == SHAPE_NORMAL_WIDE || shape == SHAPE_STONE_WIDE ? 140.0f
           : shape == SHAPE_BOSS_WIDE                              ? 220.0f
                                                                   : 200.0f;
}
/* 0212919C */
void daDoor12_c::setEventPrm() {
    if (mFrontCheck == 0) {
        m2C6 = 2;
        if (mStopBars.mA == 255)
            mStopBars.mA = chkStopB();
    } else {
        m2C6 = 3;
        if (door12Type(this) == 3)
            return;
        if (mStopBars.mA == 255)
            mStopBars.mA = chkStopF();
    }
    if (mStopBars.mbEnabled != 0)
        return;
    if (door12Type(this) == 3) {
        m2C6 = 6;
    } else if (mStopBars.mA == 1)
        m2C6 = m2C6 + 2;
    if (getShapeType() == 1 || getShapeType() == 2 || getShapeType() == 4 || getShapeType() == 5) {
        Door12Player* player = door12GameInfo()->mpPlayer;
        Door12PlayerVtable* vt = player->__vtbl;
        s32 id = gabi::call_ptr<s32>(vt->mGetGrabActorId.get(), player);
        if (id != -1) {
            gabi::Local<be<s32>> key;
            *key = id;
            if (door12SearchActor(&sSearchById, key.get()))
                m2C6 = 11;
        }
    }
    if (mKeyLock.mbEnabled) {
        u8 type = door12Type(this);
        Door12Save* save = sSave;
        if (type == 3) {
            if (!door12DungeonItem(save->mDungeonCollect, 2))
                return;
        } else if (save->mKeyNum == 0)
            return;
    }
    if (!door12CheckArea(this, 12100.0f, 12100.0f, 62500.0f))
        return;
    u8 action = m2C6;
    u16 flags = eventInfo.mCondition;
    eventInfo.mEventId = mEventIdx[action];
    u8 tool = mToolId[action];
    eventInfo.mCondition = flags | dEvtCnd_CANDOOR_e;
    eventInfo.mMapToolId = tool;
}
/* 02128430 */
void daDoor12_c::openInit() {
    door12OpenInitCom(this, 1);
    mFlags |= FLAG_OPEN;
    dBgS* bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, mpBgw);
    mOpen = 0.0f;
    speedF = 0.0f;
}
/* 021287A0 */
BOOL daDoor12_c::openProc() {
    u8 action = m2C6;
    if (action == 2 || action == 3 || action == 11)
        door12OpenProcCom(this);
    // HD: opening waits until the destination room is active.
    u8 room = mToRoomNo;
    if (room != 63) {
        dComIfGp_ea();
        if (!(sRooms.mRooms[room].mFlags & FLAG_OPEN))
            return FALSE;
    }
    // HD: the move sound begins on the first moving frame.
    if (__builtin_fabsf(speedF) < 3.814697265625e-6f) {
        if (!((door12Arg1(this) == 8 || door12Arg1(this) == 11) && mExecuteState == 1))
            sound(0x6907);
    }
    cLib_chaseF(&speedF, 20.0f, 2.0f);
    BOOL done = cLib_chaseF(&mOpen, 200.0f, speedF) != 0;
    calcMtx();
    return done;
}
/* 021288E0 */
void daDoor12_c::openEnd() {
    if (!((door12Arg1(this) == 8 || door12Arg1(this) == 11) && mExecuteState == 1))
        sound(0x6909);
    mFlags &= (u16)~FLAG_OPEN;
    mOpen = 200.0f;
    speedF = 0.0f;
}
/* 0212848C */
void daDoor12_c::closeInit() {
    mFlags |= FLAG_CLOSE;
    dBgS* bgs = dComIfG_Bgsp();
    if (dBgS_Regist(bgs, mpBgw, this))
        JUT_ASSERT_fail("d_a_door12.cpp", 0x2AF, "!rt");
    sMapSendStop = 0;
    if ((door12Arg1(this) == 8 || door12Arg1(this) == 11) && mExecuteState != 1)
        return;
    sound(0x6908);
}
/* 02128974 */
BOOL daDoor12_c::closeProc() {
    cLib_chaseF(&speedF, 20.0f, 2.0f);
    BOOL done = cLib_chaseF(&mOpen, 0.0f, speedF) != 0;
    calcMtx();
    return done;
}
/* 021289EC */
void daDoor12_c::closeEnd() {
    mFlags &= (u16)~FLAG_CLOSE;
    door12CloseEndCom(this);
    auto* play = door12GameInfo();
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    door12Shock(play->mVibration, 4, -33, v.get());
    if ((door12Arg1(this) != 8 && door12Arg1(this) != 11) || mExecuteState == 1)
        sound(0x690A);
}
/* 02127FC0 */
void daDoor12_c::calcMtx() {
    f32 opening = (f32)(mOpen * 0.005f);
    f32 wide = openWide();
    f32 offset = (f32)(opening * wide);
    f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(home.angle.y);
    mDoMtx_stack_c::transM(offset, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpRightModel, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transM(-(f32)(offset + offset), 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpLeftModel, mDoMtx_stack_c::get());
}
/* 021299CC */
BOOL daDoor12_c::CreateInit() {
    dBgS* bgs = dComIfG_Bgsp();
    if (dBgS_Regist(bgs, mpBgw, this))
        JUT_ASSERT_fail("d_a_door12.cpp", 0x306, "0");
    f32 attY = attention_info.position.y, eyeY = eyePos.y;
    tevStr.mRoomNo = current.roomNo;
    mOpen = 0.0f;
    attention_info.flags = fopAc_Attn_ACTION_DOOR_e;
    eyePos.y = eyeY + 150.0f;
    mAction = daDoor12_c::ACTION_INIT;
    attention_info.position.y = attY + 150.0f;
    calcMtx();
    dBgW_Move(mpBgw);
    dBgW* w = mpBgw;
    u8 room = door12FRoomNo(this);
    reinterpret_cast<Door12BackgroundRoom*>(w)->mRoomId = room;
    door12InitProc(this, 2);
    m2A1 = 65;
    return TRUE;
}
/* 02129AAC */
cPhs_State daDoor12_c::create() {
    const char* arc = getArcName();
    cPhs_State phase = dComIfG_resLoad(&mPhase, arc);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (door12Arg1(this) == 9 || door12Arg1(this) == 12)
        door12SetType(this, 3);
    if (door12Arg1(this) == 8)
        door12OffTmpBit(sSave->mTmpEvent, 0x440);
    if (chkMakeKey()) {
        phase = door12KeyResLoad(&mKeyLock);
        if (phase != cPhs_COMPLEATE_e)
            return phase;
    }
    current.roomNo = door12FRoomNo(this);
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x2700))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
/* 02128AA4 */
void daDoor12_c::demoProc() {
    s32 action = door12DemoAction(this);
    s32 staff = mStaffId;
    if (dComIfGp_evmng_getIsAddvance(staff)) {
        switch (action) {
        case DEMO_OPEN:
            openInit();
            break;
        case DEMO_CLOSE:
            closeInit();
            break;
        case DEMO_GOAL:
            door12SetGoal(this);
            break;
        case DEMO_KEY:
            door12KeyInit(&mKeyLock, this);
            break;
        case DEMO_CLOSE_STOP:
            setStop();
            if (mStopBars.mbEnabled)
                mStopBars.closeInit(this);
            break;
        case DEMO_OPEN_STOP:
            mStopBars.openInit(this);
            break;
        case DEMO_PLAYER_FRONT:
            door12PlayerAngle(this, 0);
            break;
        case DEMO_PLAYER_BACK:
            door12PlayerAngle(this, 1);
            break;
        }
    }
    switch (action) {
    case DEMO_OPEN:
        if (!(mFlags & FLAG_OPEN))
            cutEnd();
        else if (openProc()) {
            openEnd();
            cutEnd();
        }
        break;
    case DEMO_CLOSE:
        if (!(mFlags & FLAG_CLOSE))
            cutEnd();
        else if (closeProc()) {
            closeEnd();
            cutEnd();
        }
        break;
    case DEMO_KEY:
        if (door12KeyProc(&mKeyLock))
            cutEnd();
        mKeyLock.calcMtx(this);
        break;
    case DEMO_CLOSE_STOP: {
        s32 result = mStopBars.closeProc(this);
        if (result == 2 && door12Arg1(this) == 8)
            door12OnTmpBit(sSave->mTmpEvent, 0x440);
        if (result)
            cutEnd();
        mStopBars.calcMtx(this);
        break;
    }
    case DEMO_OPEN_STOP:
        if (mStopBars.openProc(this))
            cutEnd();
        mStopBars.calcMtx(this);
        break;
    case DEMO_END: {
        auto* play = door12GameInfo();
        if (play->mEvent.mEventFlag & 1) {
            mAction = ACTION_WAIT;
            play = door12GameInfo();
            play->mEvent.mEventFlag = play->mEvent.mEventFlag | 8;
            shape_angle.y = current.angle.y;
            play = door12GameInfo();
            play->mEvent.mEventFlag = play->mEvent.mEventFlag & (u16)~FLAG_OPEN;
            play = door12GameInfo();
            if (door12CheckStartDemo(&play->mEventManager)) {
                play = door12GameInfo();
                door12CancelStartDemo(play->mEventManager.mStartDemo);
            }
        }
        cutEnd();
        break;
    }
    default:
        cutEnd();
        break;
    }
}
/* 02129398 */
s32 daDoor12_c::actionWait() {
    u16 command = eventInfo.mCommand;
    if (command == dEvtCmd_INDOOR_e) {
        door12InitOpenDemo(this, 1);
        mAction = ACTION_DEMO;
        demoProc();
        return TRUE;
    }
    if (mStopBars.mbEnabled) {
        if (command == dEvtCmd_INDEMO_e) {
            s32 staff = dComIfGp_evmng_getMyStaffId("SHUTTER_DOOR", nullptr, 0);
            u8 front = mFrontCheck;
            s16 y = current.angle.y;
            mStaffId = staff;
            shape_angle.y = y;
            if (front == 1)
                shape_angle.y = y + 0x7FFF;
            mAction = ACTION_DEMO;
            demoProc();
            return TRUE;
        }
        if (chkStopOpen()) {
            setStopDemo();
            u8 action = m2C6, tool = mToolId[action];
            s16 event = mEventIdx[action];
            fopAcM_orderOtherEventId(this, event, tool, 0xFFFF, 0, 1);
        }
        return TRUE;
    }
    if (chkStopClose()) {
        mStopBars.mbEnabled = 1;
        mStopBars.closeInit(this);
        mStopBars.calcMtx(this);
        mAction = ACTION_STOP_CLOSE;
    } else
        setEventPrm();
    return TRUE;
}
/* 021294F0 */
s32 daDoor12_c::actionDemo() {
    s16 event = mEventIdx[m2C6];
    if (dComIfGp_evmng_endCheck(event)) {
        mAction = ACTION_WAIT;
        auto* play = door12GameInfo();
        play->mEvent.mEventFlag = play->mEvent.mEventFlag | 8;
        shape_angle.y = current.angle.y;
        return TRUE;
    }
    demoProc();
    return TRUE;
}
/* 0212957C */
s32 daDoor12_c::actionStopClose() {
    if (mStopBars.closeProc(this))
        mAction = ACTION_WAIT;
    mStopBars.calcMtx(this);
    return TRUE;
}
/* 021296B0 */
s32 daDoor12_c::actionInit() {
    setKey();
    mKeyLock.calcMtx(this);
    setStop();
    mStopBars.calcMtx(this);
    actionWait();
    mAction = ACTION_WAIT;
    return TRUE;
}
/* 0212970C */
s32 daDoor12_c::draw() {
    u8 type = door12Type(this);
    if (!door12DrawCheck(this, type == 3))
        return TRUE;
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    sDrawList = door12GameInfo()->mBgDrawList;
    sXluDrawList = door12GameInfo()->mBgXluDrawList;
    auto* env = dKy_getEnvlight();
    setLightTevColorType(env, mpLeftModel, &tevStr);
    mDoExt_modelUpdateDL(mpLeftModel, 0);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpRightModel, &tevStr);
    mDoExt_modelUpdateDL(mpRightModel, 0);
    sDrawList = door12GameInfo()->mDrawList;
    sXluDrawList = door12GameInfo()->mXluDrawList;
    if (mKeyLock.mbEnabled)
        mKeyLock.draw(this);
    if (mStopBars.mbEnabled && mStopBars.mpModel != nullptr) {
        env = dKy_getEnvlight();
        setLightTevColorType(env, mStopBars.mpModel, &tevStr);
        mDoExt_modelUpdateDL(mStopBars.mpModel, 0);
    }
    return TRUE;
}
/* 0212982C */
static s32 daDoor12_Draw(daDoor12_c* p) { return p->draw(); }
/* 02129830 */
static s32 daDoor12_Execute(daDoor12_c* p) {
    if (sActionGuard == 0) {
        sActionGuard = 1;
        memcpy_g(&sActions, &sActionPrototype, 16);
    }
    s32 state = door12CheckExecute(p);
    // HD: sound suppression uses the current execute state.
    p->mExecuteState = state;
    switch (state) {
    case 0:
        p->mAction = daDoor12_c::ACTION_INIT;
        break;
    case 2:
        gabi::call_ptr(sActions.mActions[(u8)p->mAction].get(), p);
        break;
    case 1:
        door12StartDemo(p);
        p->demoProc();
        break;
    default:
        JUT_ASSERT_fail("d_a_door12.cpp", 0x450, "0");
        break;
    }
    p->mRoomNo2 = sStayRoom;
    return TRUE;
}
/* 02129938 */
static s32 daDoor12_IsDelete(daDoor12_c* p) { return TRUE; }
/* 02129940 */
static s32 daDoor12_Delete(daDoor12_c* p) {
    // HD: release collision only after a heap was created and registration succeeded.
    if (p->heap != nullptr && p->mpBgw != nullptr && p->mpBgw->m_id < 0x100) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, p->mpBgw);
    }
    const char* arc = p->getArcName();
    dComIfG_resDelete(&p->mPhase, arc);
    if (p->chkMakeKey())
        door12KeyResDelete(&p->mKeyLock);
    return TRUE;
}
/* 02129BA8 */
static cPhs_State daDoor12_Create(daDoor12_c* p) {
    if (!(p->actor_condition & 8)) {
        if (p != nullptr) {
            door12ConstructActor(p);
            p->__vtbl = gabi::ea(&sActorVtable);
            door12ConstructKey(&p->mKeyLock);
            door12ConstructStop(&p->mStopBars);
        }
        p->actor_condition |= 8;
    }
    return p->create();
}

// HD: compiler-generated and template code of this translation unit.
/* 02129C1C */
static void __sinit_d_a_door12_cpp() {
    sinit_header_statics(gabi::ea(&sHeaderStatics), gabi::ea(&sHeaderPrototype));
}
/* 02129CB0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 02129CC4 */
static void daDoor12_c_dt(daDoor12_c* p, s32 flags) {
    if (p != nullptr) {
        door12DestroyMatrixCalc(p->mKeyLock.mMatrixCalc, 0);
        door12DestroyActor(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 02129D24 */
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue
str 1000DFDC in 0212820C
str 1000DFCC in 0212820C
str 1000DFF4 in 0212848C
str 1000DFF0 in 0212848C
str 1000E018 in 02129398
str 1000DFB8 in 02129830
str 1000DF8C in 02129830
str 1000E02C in 021299CC
str 1000E030 in 021299CC
str 1000DF70 in 02127E5C
str 1000DF68 in 02127E5C
data sShapeTable 1000DF58
data sLeftResources 101B4650
data sRightResources 101B4668
data sRooms 1047E8E8
data sSave 101F84DC
data sMapSendStop 1047A964
data sStayRoom 1047E6C8
data sDrawList 104B4634
data sXluDrawList 104B4638
data sActionGuard 101FDAAC
data sActions 101FDAB0
data sActionPrototype 101B4680
data sHeaderStatics 10463BD4
data sHeaderPrototype 101B46B0
data sActorVtable 1000DFA8
data sSearchById 025E1234
ext 0252AA58 u8 door12Arg1(daDoor12_c*)
ext 0252AA2C u8 door12Type(daDoor12_c*)
ext 0252AA14 u8 door12Swbit(daDoor12_c*)
ext 0252AA20 u8 door12Swbit2(daDoor12_c*)
ext 0252A37C u8 door12FRoomNo(daDoor12_c*)
ext 0252A388 u8 door12BRoomNo(daDoor12_c*)
ext 0252A3A0 void door12OpenInitCom(daDoor12_c*, s32)
ext 0252A48C void door12OpenProcCom(daDoor12_c*)
ext 0252A550 void door12CloseEndCom(daDoor12_c*)
ext 025CB374 void door12Shock(void*, u32, s32, cXyz*)
ext 0252A684 s32 door12DemoAction(daDoor12_c*)
ext 0252A6D0 void door12SetGoal(daDoor12_c*)
ext 0252B4A4 void door12KeyInit(Door12Key*, daDoor12_c*)
ext 0252A798 void door12PlayerAngle(daDoor12_c*, s32)
ext 0252B5B4 BOOL door12KeyProc(Door12Key*)
ext 025B8B68 void door12OnTmpBit(void*, u32)
ext 025449B0 BOOL door12CheckStartDemo(Door12EventManager*)
ext 0254351C void door12CancelStartDemo(void*)
ext 025C4334 BOOL door12RoomDisplayed(void*, u32)
ext 025D98E8 u32 door12RoomEnemy(s8)
ext 025D5218 u32 door12SearchActor(void*, be<s32>*)
ext 025B9100 BOOL door12DungeonItem(void*, u32)
ext 0252AE04 BOOL door12CheckArea(daDoor12_c*, f32, f32, f32)
ext 0252A2DC void door12InitOpenDemo(daDoor12_c*, s32)
ext 0252B848 void door12KeyOn(Door12Key*)
ext 0252B5A8 void door12KeyOff(Door12Key*)
ext 0252B0FC BOOL door12DrawCheck(daDoor12_c*, s32)
ext 0252AC98 s32 door12CheckExecute(daDoor12_c*)
ext 0252AD50 void door12StartDemo(daDoor12_c*)
ext 0252B494 void door12KeyResDelete(Door12Key*)
ext 0252A9E0 void door12InitProc(daDoor12_c*, u32)
ext 0252AA38 void door12SetType(daDoor12_c*, u32)
ext 025B8B7C void door12OffTmpBit(void*, u32)
ext 0252B484 cPhs_State door12KeyResLoad(Door12Key*)
ext 0252A244 void door12ConstructActor(daDoor12_c*)
ext 0252B3D0 void door12ConstructKey(Door12Key*)
ext 0252BA44 void door12ConstructStop(Door12StopBars*)
ext 027F3628 void door12DestroyMatrixCalc(void*, s32)
ext 025D50BC void door12DestroyActor(daDoor12_c*, s32)
ext 0252B834 BOOL Door12Key::create(s32)
ext 0252B854 void Door12Key::calcMtx(dDoor_info_c*)
ext 0252B9D4 void Door12Key::draw(dDoor_info_c*)
ext 0252BE48 BOOL Door12StopBars::create()
ext 0252BA9C void Door12StopBars::calcMtx(dDoor_info_c*)
ext 0252BB9C void Door12StopBars::closeInit(dDoor_info_c*)
ext 0252BC3C s32 Door12StopBars::closeProc(dDoor_info_c*)
ext 0252BCF4 void Door12StopBars::openInit(dDoor_info_c*)
ext 0252BD8C s32 Door12StopBars::openProc(dDoor_info_c*)
ext 025200D4 Door12GameInfo* door12GameInfo()
*/
// clang-format on
