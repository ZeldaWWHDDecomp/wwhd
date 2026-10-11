/** Stone sliding doors, ported from zeldaret/tww and adapted to WWHD.
 * Derived game source: */
#include "d/actor/d_a_door10.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1000DE68,
    .headerStatics = {0x10463BB8, 0x101B45FC, 0x10463BC4},
};

static Door10RoomTableHD l_roomStatus;
static u8 l_mapSendStopFlag;
static u32 l_actionInitialized;
static Door10ActionTableHD l_action;
// tww order: actionInit, actionWait, actionStopClose, actionDemo, actionHkyo.
static Door10ActionTableHD l_actionInitial;
static dComIfG_inf_c* door10Play() { return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get()); }
static bool roomStatus(u8 room) {
    // HD: retain the play lookup preceding the static room flag read.
    door10Play();
    return (l_roomStatus.entries[room].mFlags & 1) != 0;
}
static void sound(daDoor10_c* i_this, u32 id) {
    s32 reverb = dComIfGp_getReverb((s8)i_this->current.roomNo);
    mDoAud_seStart(id, &i_this->eyePos, 0, reverb);
}
static void cutEnd(daDoor10_c* i_this) {
    s32 staff = i_this->mStaffId;
    dComIfGp_evmng_cutEnd(staff);
}
static void orderEvent(daDoor10_c* i_this) {
    u8 idx = i_this->m2C6;
    s16 evt = i_this->mEventIdx[idx];
    u8 tool = i_this->mToolId[idx];
    Door10_orderEvent(i_this, evt, tool, 65535, 0, 1);
}

/* 02125E84 */
s32 daDoor10_c::chkMakeKey() {
    u8 type = getType();
    if (type == 4 || type == 5)
        return 1;
    return type == 1 ? 2 : 0;
}

/* 02127554 */
void daDoor10_c::setKey() {
    if (chkMakeKey() == 1) {
        u8 sw = getSwbit();
        if (!dComIfGs_isSwitch(sw, -1)) {
            mKeyLock.keyOn();
            return;
        }
    }
    if (chkMakeKey() == 2) {
        if (getSwbit() == 255) {
            mKeyLock.keyOff();
            return;
        }
        if (getSwbit() >= 128) {
            mKeyLock.keyOn();
            return;
        }
        u8 sw = getSwbit();
        if (!dComIfGs_isSwitch(sw, -1)) {
            mKeyLock.keyOn();
            return;
        }
    }
    mKeyLock.keyOff();
}

/* 02125ED8 */
BOOL daDoor10_c::chkMakeStop() {
    if (getSwbit2() != 255)
        return TRUE;
    return !chkMakeKey() && getSwbit() != 255;
}

/* 021263D0 */
s32 daDoor10_c::chkStopF() {
    u8 type = getType(), sw = getSwbit(), room = getFRoomNo();
    if (sw == 255)
        return 0;
    if (type == 0 || type == 2 || type == 3) {
        if (!roomStatus(room))
            return -1;
        return !dComIfGs_isSwitch(sw, room);
    }
    return 0;
}

/* 021264C4 */
s32 daDoor10_c::chkStopB() {
    u8 sw = getSwbit2(), room = getBRoomNo();
    if (sw == 255)
        return 0;
    if (!roomStatus(room))
        return -1;
    return !dComIfGs_isSwitch(sw, room);
}

/* 02126594 */
void daDoor10_c::setStop() {
    if (chkMakeStop() && mStopBars.mpModel) {
        u8 front = mFrontCheck;
        mStopBars.mFrontCheck = front;
        if (front == 0) {
            mStopBars.m8 = chkStopF();
            mStopBars.mA = chkStopB();
        } else {
            mStopBars.m8 = chkStopB();
            mStopBars.mA = chkStopF();
        }
        mStopBars.mOffsY = 0;
    }
}

/* 02126D8C */
BOOL daDoor10_c::chkStopOpen() {
    u8 type = getType(), sw, room;
    if (mFrontCheck == 0) {
        sw = getSwbit();
        room = getFRoomNo();
    } else {
        sw = getSwbit2();
        room = getBRoomNo();
    }
    u8 front = mFrontCheck;
    if ((front == 0 && type == 2) || (front == 1 && type == 5)) {
        dComIfG_inf_c* p = door10Play();
        bool running = p->play.mEvtCtrl.mMode != 0;
        if (!running || m2A1 == 0) {
            p = door10Play();
            if (Door10_roomIsDisplay(&p->play.mRoomCtrl, room) && !Door10_searchEnemy((s8)room)) {
                if (m2A1 != 0) {
                    m2A1 = m2A1 - 1;
                    return FALSE;
                }
                if (sw != 255)
                    dComIfGs_onSwitch(sw, room);
                return TRUE;
            }
            m2A1 = getArg1() == 15 ? 0 : 65;
        }
    } else if (sw != 255 && dComIfGs_isSwitch(sw, room))
        return TRUE;
    return FALSE;
}

/* 02126F48 */
void daDoor10_c::setStopDemo() { m2C6 = mFrontCheck != 0; }

/* 02126F5C */
BOOL daDoor10_c::chkStopClose() {
    u8 type = getType();
    if (!mStopBars.mpModel || type == 1)
        return FALSE;
    u8 sw, room;
    if (mFrontCheck == 0) {
        if (type == 2)
            return FALSE;
        sw = getSwbit();
        room = getFRoomNo();
    } else {
        if (type == 5)
            return FALSE;
        sw = getSwbit2();
        room = getBRoomNo();
    }
    if (sw == 255)
        return FALSE;
    return !dComIfGs_isSwitch(sw, room);
}

/* 02125E28 */
const char* daDoor10_c::getBdlName() {
    u8 type = getType();
    // HD: model files use the bmd extension.
    return Door10_guestString(type == 1 ? "door20.bmd" : type == 3 ? "door11.bmd" : "door10.bmd");
}

/* 02125F48 */
const char* daDoor10_c::getDzbName() {
    u8 type = getType();
    return Door10_guestString(type == 1 ? "door20.dzb" : type == 3 ? "door11.dzb" : "door10.dzb");
}

/* 02127038 */
f32 daDoor10_c::getSize2X() { return getType() == 3 ? 225.0f : 12100.0f; }

/* 021262D4 */
static BOOL CheckCreateHeap(daDoor10_c* i_this) { return i_this->CreateHeap(); }

/* 02126074 */
BOOL daDoor10_c::CreateHeap() {
    auto object = [](s32 id) { return dComIfG_getObjectRes("DoorBs", id); };
    auto stage = [](const char* name) { return dComIfG_getStageRes("Stage", name); };
    J3DModelData* data;
    if (m364 != 0)
        data = (J3DModelData*)object(m364);
    else {
        const char* name = getBdlName();
        data = (J3DModelData*)stage(name);
    }
    if (!data)
        JUT_ASSERT_fail("d_a_door10.cpp", 356, "modelData");
    mpModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    if (!mpModel)
        return FALSE;
    switch (chkMakeKey()) {
    case 1:
        if (!mKeyLock.keyCreate(0))
            return FALSE;
        break;
    case 2:
        if (!mKeyLock.keyCreate(1))
            return FALSE;
        break;
    }
    if (mHkyo.chkUse() && !mHkyo.create())
        return FALSE;
    if (chkMakeStop() && !mStopBars.create())
        return FALSE;
    mpBgW = new_dBgW();
    if (!mpBgW)
        return FALSE;
    cBgD_t* bg;
    if (m364 != 0)
        bg = (cBgD_t*)object(m364 == 4 ? 8 : 9);
    else {
        const char* name = getDzbName();
        bg = (cBgD_t*)stage(name);
    }
    if (!bg)
        return FALSE;
    calcMtx();
    mKeyLock.calcMtx(this);
    mStopBars.calcMtx(this);
    if (mHkyo.chkUse())
        mHkyo.calcMtx(this, 0);
    J3DModel* model = mpModel;
    dBgW* world = mpBgW;
    return !cBgW_Set(world, bg, 1, J3DModel_getBaseTRMtx(model));
}

/* 02127080 */
void daDoor10_c::setEventPrm() {
    if (mFrontCheck == 0) {
        m2C6 = 2;
        if (mStopBars.mA == 255)
            mStopBars.mA = chkStopB();
    } else {
        m2C6 = 3;
        if (getType() == 1 && getArg1() != 16)
            return;
        if (mStopBars.mA == 255)
            mStopBars.mA = chkStopF();
    }
    if (mStopBars.m8 != 0)
        return;
    if (getType() == 1 && getArg1() != 16)
        m2C6 = 6;
    else if (mStopBars.mA == 1)
        m2C6 = m2C6 + 2;
    if (mKeyLock.mbEnabled) {
        u8 type = getType();
        dSv_info_c* s = dComIfGs_info();
        if (type == 1) {
            if (!Door10_isDungeonItem(&s->mMemory, dSv_memBit_c::BOSS_KEY))
                return;
        } else if (!s->mMemory.mMembit.mKeyNum)
            return;
    }
    f32 size = getSize2X();
    if (checkArea(size, 12100.0f, 62500.0f)) {
        u8 index = m2C6;
        u16 condition = eventInfo.mCondition;
        eventInfo.mEventId = mEventIdx[index];
        u8 tool = mToolId[index];
        eventInfo.mCondition = condition | dEvtCnd_CANDOOR_e;
        eventInfo.mMapToolId = tool;
    }
}

/* 021262D8 */
void daDoor10_c::openInit() {
    openInitCom(1);
    onFlag(DOOR10_OPEN);
    dComIfG_inf_c* p = door10Play();
    cBgS_Release(&p->play.mBgS, mpBgW);
    m358 = 0;
    // HD: delay the start sound until openProc actually moves.
    speedF = 0;
}

/* 0212662C */
BOOL daDoor10_c::openProc() {
    u8 room = mToRoomNo;
    if (room != 63 && !roomStatus(room))
        return FALSE;
    if (std::fabs((f32)speedF) < 0.000003814697265625f)
        sound(this, JA_SE_OBJ_STN_DOOR_MOVE_U);
    cLib_chaseF(&speedF, 30, 4);
    BOOL done = cLib_chaseF(&m358, 300, speedF) != 0;
    calcMtx();
    if (mHkyo.chkUse())
        mHkyo.calcMtx(this, m358);
    return done;
}

/* 0212673C */
void daDoor10_c::openEnd() {
    sound(this, JA_SE_OBJ_STN_DOOR_STOP_U);
    offFlag(DOOR10_OPEN);
    m358 = 300;
    speedF = 0;
}

/* 02126334 */
void daDoor10_c::closeInit() {
    onFlag(DOOR10_CLOSE);
    dComIfG_inf_c* p = door10Play();
    if (dBgS_Regist(&p->play.mBgS, mpBgW, this))
        JUT_ASSERT_fail("d_a_door10.cpp", 557, "!rt");
    l_mapSendStopFlag = 0;
    sound(this, JA_SE_OBJ_STN_DOOR_MOVE_D);
}

/* 021267A8 */
BOOL daDoor10_c::closeProc() {
    cLib_chaseF(&speedF, 60, 6);
    BOOL done = cLib_chaseF(&m358, 0, speedF) != 0;
    calcMtx();
    if (mHkyo.chkUse())
        mHkyo.calcMtx(this, m358);
    return done;
}

/* 0212683C */
void daDoor10_c::closeEnd() {
    offFlag(DOOR10_CLOSE);
    closeEndCom();
    dComIfG_inf_c* p = door10Play();
    gabi::Local<cXyz> dir;
    dir->set(0, 1, 0);
    p->play.mVibration.StartShock(4, -33, dir.get());
    sound(this, JA_SE_OBJ_STN_DOOR_STOP_D);
}

/* 02125FA4 */
void daDoor10_c::calcMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(home.angle.y);
    mDoMtx_stack_c::transM(0, m358, 0);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 02127998 */
BOOL daDoor10_c::CreateInit() {
    dComIfG_inf_c* p = door10Play();
    if (dBgS_Regist(&p->play.mBgS, mpBgW, this))
        JUT_ASSERT_fail("d_a_door10.cpp", 623, "0");
    tevStr.mRoomNo = current.roomNo;
    m358 = 0;
    setAction(DOOR10_INIT);
    u8 type = getType();
    f32 a = attention_info.position.y, b = eyePos.y, height = type == 1 ? 250 : 150;
    attention_info.flags = fopAc_Attn_ACTION_DOOR_e;
    attention_info.position.y = a + height;
    eyePos.y = b + height;
    calcMtx();
    mpBgW->Move();
    Door10BackgroundHD* world = reinterpret_cast<Door10BackgroundHD*>(mpBgW.get());
    u8 room = getFRoomNo();
    world->mRoomId = room;
    initProc(1);
    if (mHkyo.chkUse()) {
        mHkyo.init();
        mHkyo.calcMtx(this, 0);
    }
    // HD: countdown before the enemy-clear stop can open.
    m2A1 = 65;
    return TRUE;
}

/* 02127B20 */
cPhs_State daDoor10_c::create() {
    u32 arg = getArg1();
    if (arg >= 1 && arg <= 4)
        mHkyo.onUse(getArg1());
    else
        mHkyo.offUse();
    arg = getArg1();
    m364 = arg == 13 ? 4 : arg == 14 ? 5 : 0;
    cPhs_State state;
    if (m364) {
        state = dComIfG_resLoad(&mPhase, "DoorBs");
        if (state != cPhs_COMPLEATE_e)
            return state;
    }
    if (chkMakeKey()) {
        state = mKeyLock.keyResLoad();
        if (state != cPhs_COMPLEATE_e)
            return state;
    }
    if (mHkyo.chkUse()) {
        state = mHkyo.resLoad();
        if (state != cPhs_COMPLEATE_e)
            return state;
    }
    current.roomNo = getFRoomNo();
    if (!fopAcM_entrySolidHeap(this, reinterpret_cast<BOOL (*)(fopAc_ac_c*)>(CheckCreateHeap), 0x3800))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 021268C8 */
void daDoor10_c::demoProc() {
    s32 action = getDemoAction();
    s32 staff = mStaffId;
    dComIfG_inf_c* p = door10Play();
    if (Door10_isAdvance(&p->play.mEvtManager, staff)) {
        switch (action) {
        case 3:
            openInit();
            break;
        case 4:
            closeInit();
            break;
        case 7:
            setGoal();
            break;
        case 5:
            m2D0.smokeInit(this);
            break;
        case 6:
            m2D0.smokeEnd();
            break;
        case 8:
            mKeyLock.keyInit(this);
            break;
        case 2:
            setStop();
            if (mStopBars.m8)
                mStopBars.closeInit(this);
            break;
        case 1:
            if (mHkyo.chkUse() && mHkyo.chkFirst())
                mHkyo.onFirst();
            else
                mStopBars.openInit(this);
            break;
        }
    }
    switch (action) {
    case 3:
        if (!checkFlag(DOOR10_OPEN)) {
            cutEnd(this);
        } else if (openProc()) {
            openEnd();
            cutEnd(this);
        }
        break;
    case 4:
        if (!checkFlag(DOOR10_CLOSE))
            cutEnd(this);
        else if (closeProc()) {
            closeEnd();
            cutEnd(this);
        }
        break;
    case 5:
        m2D0.smokeProc(this);
        cutEnd(this);
        break;
    case 8:
        if (mKeyLock.keyProc())
            cutEnd(this);
        mKeyLock.calcMtx(this);
        break;
    case 2:
        if (mStopBars.closeProc(this))
            cutEnd(this);
        mStopBars.calcMtx(this);
        break;
    case 1:
        if (mHkyo.chkUse() && mHkyo.chkFirst())
            cutEnd(this);
        else if (mStopBars.openProc(this))
            cutEnd(this);
        mStopBars.calcMtx(this);
        break;
    case 19:
        p = door10Play();
        if (p->play.mEvtCtrl.mEventFlag & 1) {
            setAction(DOOR10_WAIT);
            p = door10Play();
            p->play.mEvtCtrl.mEventFlag = p->play.mEvtCtrl.mEventFlag | 8;
            shape_angle.y = current.angle.y;
            p = door10Play();
            p->play.mEvtCtrl.mEventFlag = p->play.mEvtCtrl.mEventFlag & ~1;
            p = door10Play();
            if (Door10_checkStartDemo(&p->play.mEvtManager)) {
                p = door10Play();
                Door10_cancelStartDemo(
                    &reinterpret_cast<Door10EventManagerHD*>(&p->play.mEvtManager)->mStartDemo);
            }
        }
        cutEnd(this);
        break;
    default:
        cutEnd(this);
        break;
    }
}

/* 02127218 */
BOOL daDoor10_actionWait(daDoor10_c* i_this) {
    u16 command = i_this->eventInfo.mCommand;
    if (command == dEvtCmd_INDOOR_e) {
        i_this->initOpenDemo(1);
        i_this->setAction(DOOR10_DEMO);
        i_this->demoProc();
    } else {
        if (i_this->mStopBars.m8 != 0) {
            if (command == dEvtCmd_INDEMO_e) {
                dComIfG_inf_c* p = door10Play();
                i_this->mStaffId = Door10_staffId(&p->play.mEvtManager, "SHUTTER_DOOR", 0, 0);
                i_this->shape_angle.y = i_this->current.angle.y;
                if (i_this->mFrontCheck == 1)
                    i_this->shape_angle.y = i_this->shape_angle.y + 0x7fff;
                i_this->setAction(DOOR10_DEMO);
                i_this->demoProc();
                return TRUE;
            }
            if (i_this->chkStopOpen()) {
                i_this->setStopDemo();
                orderEvent(i_this);
                return TRUE;
            }
        }
        if (i_this->mStopBars.m8 == 0 && i_this->chkStopClose()) {
            i_this->mStopBars.m8 = 1;
            i_this->mStopBars.closeInit(i_this);
            i_this->mStopBars.calcMtx(i_this);
            i_this->setAction(DOOR10_STOP_CLOSE);
            return TRUE;
        }
        if (i_this->mHkyo.chkUse() && i_this->mHkyo.chkFirst()) {
            i_this->setAction(DOOR10_HKYO);
            return TRUE;
        }
        if (i_this->mStopBars.m8 == 0)
            i_this->setEventPrm();
    }
    return TRUE;
}

/* 021273B4 */
BOOL daDoor10_actionStopClose(daDoor10_c* i_this) {
    if (i_this->mStopBars.closeProc(i_this))
        i_this->setAction(DOOR10_WAIT);
    i_this->mStopBars.calcMtx(i_this);
    return TRUE;
}

/* 02127408 */
BOOL daDoor10_actionDemo(daDoor10_c* i_this) {
    s16 evt = i_this->mEventIdx[i_this->m2C6];
    dComIfG_inf_c* p = door10Play();
    if (Door10_endCheck(&p->play.mEvtManager, evt)) {
        i_this->setAction(DOOR10_WAIT);
        p = door10Play();
        p->play.mEvtCtrl.mEventFlag = p->play.mEvtCtrl.mEventFlag | 8;
        i_this->shape_angle.y = i_this->current.angle.y;
    } else
        i_this->demoProc();
    return TRUE;
}

/* 02127494 */
BOOL daDoor10_actionHkyo(daDoor10_c* i_this) {
    if (!i_this->mHkyo.chkUse()) {
        i_this->setAction(DOOR10_WAIT);
        return TRUE;
    }
    if (i_this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        dComIfG_inf_c* p = door10Play();
        i_this->mStaffId = Door10_staffId(&p->play.mEvtManager, "SHUTTER_DOOR", 0, 0);
        i_this->setAction(DOOR10_DEMO);
        i_this->demoProc();
    } else if (i_this->mHkyo.chkStart()) {
        i_this->m2C6 = 10;
        orderEvent(i_this);
    }
    return TRUE;
}

/* 02127634 */
BOOL daDoor10_actionInit(daDoor10_c* i_this) {
    i_this->setKey();
    i_this->mKeyLock.calcMtx(i_this);
    i_this->setStop();
    i_this->mStopBars.calcMtx(i_this);
    if (i_this->mHkyo.chkUse())
        i_this->mHkyo.calcMtx(i_this, 0);
    daDoor10_actionWait(i_this);
    i_this->setAction(DOOR10_WAIT);
    return TRUE;
}

/* 021276B0 */
BOOL daDoor10_c::draw() {
    if (!drawCheck(getType() == 1))
        return TRUE;
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    if (mKeyLock.mbEnabled)
        mKeyLock.draw(this);
    if (mStopBars.m8 && mStopBars.mpModel) {
        g_env_light.setLightTevColorType(mStopBars.mpModel, &tevStr);
        mDoExt_modelUpdateDL(mStopBars.mpModel, 0);
    }
    if (mHkyo.chkUse())
        mHkyo.draw(this);
    return TRUE;
}

/* 021277CC */
BOOL daDoor10_Draw(daDoor10_c* i_this) { return i_this->draw(); }

/* 021277D0 */
BOOL daDoor10_Execute(daDoor10_c* i_this) {
    if (!l_actionInitialized) {
        l_actionInitialized = 1;
        memcpy_g(&l_action, &l_actionInitial, sizeof(l_action));
    }
    s32 state = i_this->checkExecute();
    if (i_this->mHkyo.chkUse())
        i_this->mHkyo.proc(i_this);
    switch (state) {
    case 0:
        i_this->setAction(DOOR10_INIT);
        break;
    case 2:
        l_action.invoke(i_this->m354, i_this);
        break;
    case 1:
        i_this->startDemoProc();
        i_this->demoProc();
        break;
    default:
        JUT_ASSERT_fail("d_a_door10.cpp", 1049, "0");
        break;
    }
    i_this->mRoomNo2 = dComIfGp_roomControl_getStayNo();
    return TRUE;
}

/* 021278F8 */
BOOL daDoor10_IsDelete(daDoor10_c* i_this) { return TRUE; }

/* 02127900 */
BOOL daDoor10_Delete(daDoor10_c* i_this) {
    if (i_this->heap) {
        dComIfG_inf_c* p = door10Play();
        cBgS_Release(&p->play.mBgS, i_this->mpBgW);
    }
    if (i_this->m364 != 0)
        dComIfG_resDelete(&i_this->mPhase, "DoorBs");
    if (i_this->chkMakeKey())
        i_this->mKeyLock.keyResDelete();
    if (i_this->mHkyo.chkUse())
        i_this->mHkyo.resDelete();
    i_this->m2D0.smokeEnd();
    return TRUE;
}

/* 02127C80 */
cPhs_State daDoor10_Create(daDoor10_c* i_this) {
    u32 condition = i_this->actor_condition;
    if (!(condition & 8)) {
        if (i_this) {
            Door10_infoConstruct(i_this);
            i_this->__vtbl = daDoor10_c::__vt;
            Door10_smokeConstruct(&i_this->m2D0);
            Door10_keyConstruct(&i_this->mKeyLock);
            Door10_stopConstruct(&i_this->mStopBars);
            Door10_hkyoConstruct(&i_this->mHkyo);
            condition = i_this->actor_condition;
        }
        i_this->actor_condition = condition | 8;
    }
    return i_this->create();
}

// HD: compiler-generated helpers of this translation unit.

/* 02127D04 */
void door10_sinit() { __sinit_header_statics(); }

/* 02127D98 */
void door10_genericDtor(void* i_this, s32 flags) {
    if (i_this && (flags & 1))
        operator_delete(i_this);
}

/* 02127DAC */
void door10_actorDtor(daDoor10_c* i_this, s32 flags) {
    if (i_this) {
        Door10_animationDestroy(&i_this->mKeyLock.mBckAnim.mMatrixCalc, 0);
        Door10_actorDestroy(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02127E0C */
void door10_empty(void* i_this) {}

// clang-format off
/* WWHD glue
data l_actionInitialized 101FDA94
data l_action 101FDA98
data l_actionInitial 101B45E8
ext 0252AA2C u8 Door10InfoHD::getType()
ext 0252AA14 u8 Door10InfoHD::getSwbit()
ext 0252AA20 u8 Door10InfoHD::getSwbit2()
ext 0252AA58 u8 Door10InfoHD::getArg1()
ext 0252A37C u8 Door10InfoHD::getFRoomNo()
ext 0252A388 u8 Door10InfoHD::getBRoomNo()
ext 0252A3A0 void Door10InfoHD::openInitCom(s32 x)
ext 0252A550 void Door10InfoHD::closeEndCom()
ext 0252A2DC void Door10InfoHD::initOpenDemo(s32 x)
ext 0252A684 s32 Door10InfoHD::getDemoAction()
ext 0252A6D0 void Door10InfoHD::setGoal()
ext 0252AE04 BOOL Door10InfoHD::checkArea(f32 a, f32 b, f32 c)
ext 0252A9E0 void Door10InfoHD::initProc(s32 x)
ext 0252B0FC BOOL Door10InfoHD::drawCheck(s32 x)
ext 0252AC98 s32 Door10InfoHD::checkExecute()
ext 0252AD50 void Door10InfoHD::startDemoProc()
ext 0252B1E8 void Door10SmokeHD::smokeInit(void *p)
ext 0252B2D8 void Door10SmokeHD::smokeProc(void *p)
ext 0252B3CC void Door10SmokeHD::smokeEnd()
ext 0252B834 BOOL Door10KeyHD::keyCreate(s32 x)
ext 0252B484 cPhs_State Door10KeyHD::keyResLoad()
ext 0252B494 void Door10KeyHD::keyResDelete()
ext 0252B4A4 void Door10KeyHD::keyInit(void *p)
ext 0252B5B4 BOOL Door10KeyHD::keyProc()
ext 0252B854 void Door10KeyHD::calcMtx(void *p)
ext 0252B9D4 void Door10KeyHD::draw(void *p)
ext 0252B848 void Door10KeyHD::keyOn()
ext 0252B5A8 void Door10KeyHD::keyOff()
ext 0252BA9C void Door10StopHD::calcMtx(void *p)
ext 0252BB9C void Door10StopHD::closeInit(void *p)
ext 0252BC3C BOOL Door10StopHD::closeProc(void *p)
ext 0252BCF4 void Door10StopHD::openInit(void *p)
ext 0252BD8C BOOL Door10StopHD::openProc(void *p)
ext 0252BE48 BOOL Door10StopHD::create()
ext 0252C11C cPhs_State Door10HkyoHD::resLoad()
ext 0252C13C void Door10HkyoHD::resDelete()
ext 0252C154 BOOL Door10HkyoHD::create()
ext 0252C274 void Door10HkyoHD::init()
ext 0252C280 void Door10HkyoHD::calcMtx(void *p, f32 x)
ext 0252C3F8 void Door10HkyoHD::draw(void *p)
ext 0252C574 void Door10HkyoHD::proc(void *p)
ext 0252C390 BOOL Door10HkyoHD::chkFirst()
ext 0252C748 void Door10HkyoHD::onFirst()
ext 0252C788 BOOL Door10HkyoHD::chkStart()
str 1000DED4 "d_a_door10.cpp" in 02126334
str 1000DED0 "!rt" in 02126334
str 1000DEAC "DoorBs" in 02126074
str 1000DEA4 "Stage" in 02126074
str 1000DEC0 "d_a_door10.cpp" in 02126074
str 1000DEB4 "modelData" in 02126074
str 1000DF08 "SHUTTER_DOOR" in 02127218
str 1000DF18 "SHUTTER_DOOR" in 02127494
str 1000DF28 "DoorBs" in 02127900
str 1000DE90 "d_a_door10.cpp" in 021277D0
str 1000DE64 "0" in 021277D0
str 1000DF3C "d_a_door10.cpp" in 02127998
str 1000DF38 "0" in 02127998
str 1000DF4C "DoorBs" in 02127B20
str 1000DE18 "door20.bmd" in 02125E28
str 1000DE24 "door11.bmd" in 02125E28
str 1000DE30 "door10.bmd" in 02125E28
str 1000DE3C "door20.dzb" in 02125F48
str 1000DE48 "door11.dzb" in 02125F48
str 1000DE54 "door10.dzb" in 02125F48
ext 025C4334 BOOL Door10_roomIsDisplay(dStage_roomControl_c* p0, u8 p1)
ext 025D98E8 void* Door10_searchEnemy(s8 p0)
ext 025B9100 BOOL Door10_isDungeonItem(dSv_memory_c* p0, s32 p1)
ext 025D7A58 void Door10_orderEvent(daDoor10_c* p0, s16 p1, u8 p2, s32 p3, s32 p4, s32 p5)
ext 02542D88 s32 Door10_staffId(dEvent_manager_c* p0, const char* p1, s32 p2, s32 p3)
ext 025440C8 BOOL Door10_endCheck(dEvent_manager_c* p0, s16 p1)
ext 025447C8 BOOL Door10_isAdvance(dEvent_manager_c* p0, s32 p1)
ext 025449B0 BOOL Door10_checkStartDemo(dEvent_manager_c* p0)
ext 0254351C void Door10_cancelStartDemo(u8* p0)
ext 0252A244 void Door10_infoConstruct(daDoor10_c* p0)
ext 0252B190 void Door10_smokeConstruct(Door10SmokeHD* p0)
ext 0252B3D0 void Door10_keyConstruct(Door10KeyHD* p0)
ext 0252BA44 void Door10_stopConstruct(Door10StopHD* p0)
ext 0252C0BC void Door10_hkyoConstruct(Door10HkyoHD* p0)
ext 027F3628 void Door10_animationDestroy(void* p0, s32 p1)
ext 025D50BC void Door10_actorDestroy(daDoor10_c* p0, s32 p1)
data l_roomStatus 1047E6CC
data l_mapSendStopFlag 1047A964
*/
// clang-format on
