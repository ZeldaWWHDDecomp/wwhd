/* d_a_door12.cpp: WWHD double door; based on zeldaret/tww.
 * HD: enlarged key lock, singleton accessors, double-door animation and event ordering. */
#include "d/actor/d_a_door12.h"

s32 daDoor12_c::getShapeType() {
    WWHD_FUNC(0x02127E10, s32, this);
    u32 arg = getArg1();
    return arg >= 8 && arg <= 12 ? gabi::load<u8>(0x1000DF58 + arg) : 0;
}
VERIFY(0x02127E10, &daDoor12_c::getShapeType);
const char *daDoor12_c::getArcName() {
    WWHD_FUNC(0x02127E5C, const char *, this);
    return STR(getShapeType() < 3 ? 0x1000DF70 : 0x1000DF68);
}
VERIFY(0x02127E5C, &daDoor12_c::getArcName);
s32 daDoor12_c::getBdlLf() {
    WWHD_FUNC(0x02127EA0, s32, this);
    return gabi::load<s32>(0x101B4650 + 4 * getShapeType());
}
VERIFY(0x02127EA0, &daDoor12_c::getBdlLf);
s32 daDoor12_c::getDzb() {
    WWHD_FUNC(0x02127ED0, s32, this);
    s32 shape = getShapeType();
    return shape == 2 || shape == 5 ? 12 : 13;
}
VERIFY(0x02127ED0, &daDoor12_c::getDzb);
s32 daDoor12_c::getBdlRt() {
    WWHD_FUNC(0x02127F18, s32, this);
    return gabi::load<s32>(0x101B4668 + 4 * getShapeType());
}
VERIFY(0x02127F18, &daDoor12_c::getBdlRt);
f32 daDoor12_c::openWide() {
    WWHD_FUNC(0x02127F48, f32, this);
    u32 shape = getShapeType();
    return shape == 3 || shape == 4 ? 140.0f : shape == 5 ? 220.0f : 200.0f;
}
VERIFY(0x02127F48, &daDoor12_c::openWide);
void daDoor12_c::calcMtx() {
    WWHD_FUNC(0x02127FC0, void, this);
    f32 opening = (f32)(mOpen * 0.005f);
    f32 wide = openWide();
    f32 offset = (f32)(opening * wide);
    f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(home.angle.y);
    mDoMtx_stack_c::transM(offset, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpModelRt, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transM(-(f32)(offset + offset), 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpModelLf, mDoMtx_stack_c::get());
}
VERIFY(0x02127FC0, &daDoor12_c::calcMtx);
s32 daDoor12_c::chkMakeKey() {
    WWHD_FUNC(0x02128150, s32, this);
    u8 type = getType();
    return type == 1 ? 1 : type == 3 ? 2 : 0;
}
VERIFY(0x02128150, &daDoor12_c::chkMakeKey);
BOOL daDoor12_c::chkMakeStop() {
    WWHD_FUNC(0x0212819C, BOOL, this);
    if (getSwbit2() != 255)
        return TRUE;
    if (chkMakeKey() != 0)
        return FALSE;
    return getSwbit() != 255;
}
VERIFY(0x0212819C, &daDoor12_c::chkMakeStop);
BOOL daDoor12_c::CreateHeap() {
    WWHD_FUNC(0x0212820C, BOOL, this);
    const char *arc = getArcName();
    s32 idx = getBdlLf();
    auto *data = (J3DModelData *)dComIfG_getObjectRes(arc, idx, 0x1000DF90);
    if (!data)
        JUT_ASSERT_fail(STR(0x1000DFCC), 0x16A, STR(0x1000DFDC));
    mpModelLf = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (mpModelLf == nullptr)
        return FALSE;
    arc = getArcName();
    idx = getBdlRt();
    data = (J3DModelData *)dComIfG_getObjectRes(arc, idx, 0x1000DF90);
    if (!data)
        JUT_ASSERT_fail(STR(0x1000DFCC), 0x173, STR(0x1000DFDC));
    mpModelRt = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (mpModelRt == nullptr)
        return FALSE;
    mpBgW = new_dBgW();
    if (mpBgW == nullptr)
        return FALSE;
    arc = getArcName();
    idx = getDzb();
    auto *bg = (cBgD_t *)dComIfG_getObjectRes(arc, idx, 0x1000DF90);
    if (!bg)
        return FALSE;
    calcMtx();
    J3DModel *left = mpModelLf;
    dBgW *w = mpBgW;
    Mtx34 *mtx = left ? gabi::at<Mtx34>(gabi::ea(left) + 0xC8) : nullptr;
    if (cBgW_Set(w, bg, 1, mtx))
        return FALSE;
    s32 key = chkMakeKey();
    if (key == 1 && !mKey.create(0))
        return FALSE;
    if (key == 2 && !mKey.create(1))
        return FALSE;
    if (chkMakeStop() && !mStop.create())
        return FALSE;
    mKey.calcMtx(this);
    mStop.calcMtx(this);
    return TRUE;
}
VERIFY(0x0212820C, &daDoor12_c::CreateHeap);
static BOOL CheckCreateHeap(daDoor12_c *p) {
    WWHD_FUNC(0x0212842C, BOOL, p);
    return p->CreateHeap();
}
VERIFY(0x0212842C, CheckCreateHeap);
void daDoor12_c::openInit() {
    WWHD_FUNC(0x02128430, void, this);
    gabi::call(0x0252A3A0, this, 1);
    mFlags |= 1;
    dBgS *bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, mpBgW);
    mOpen = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x02128430, &daDoor12_c::openInit);
void daDoor12_c::closeInit() {
    WWHD_FUNC(0x0212848C, void, this);
    mFlags |= 2;
    dBgS *bgs = dComIfG_Bgsp();
    if (dBgS_Regist(bgs, mpBgW, this))
        JUT_ASSERT_fail(STR(0x1000DFF4), 0x2AF, STR(0x1000DFF0));
    gabi::store<u8>(0x1047A964, 0);
    if ((getArg1() == 8 || getArg1() == 11) && mExecuteState != 1)
        return;
    sound(0x6908);
}
VERIFY(0x0212848C, &daDoor12_c::closeInit);
s32 daDoor12_c::chkStopF() {
    WWHD_FUNC(0x0212854C, s32, this);
    u8 type = getType(), sw = getSwbit(), room = getFRoomNo();
    if (sw == 255 || (type != 0 && type != 2))
        return 0;
    dComIfGp_ea();
    if (!(gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 1))
        return -1;
    return dComIfGs_isSwitch(sw, room) == 0;
}
VERIFY(0x0212854C, &daDoor12_c::chkStopF);
s32 daDoor12_c::chkStopB() {
    WWHD_FUNC(0x02128638, s32, this);
    u8 sw = getSwbit2(), room = getBRoomNo();
    if (sw == 255)
        return 0;
    dComIfGp_ea();
    if (!(gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 1))
        return -1;
    return dComIfGs_isSwitch(sw, room) == 0;
}
VERIFY(0x02128638, &daDoor12_c::chkStopB);
void daDoor12_c::setStop() {
    WWHD_FUNC(0x02128708, void, this);
    if (chkMakeStop() && mStop.mpModel != nullptr) {
        u8 front = mFrontCheck;
        mStop.mFrontCheck = front;
        if (front == 0) {
            mStop.mEnabled = chkStopF();
            mStop.mOtherEnabled = chkStopB();
        } else {
            mStop.mEnabled = chkStopB();
            mStop.mOtherEnabled = chkStopF();
        }
        mStop.mOffsetY = 0.0f;
    }
}
VERIFY(0x02128708, &daDoor12_c::setStop);
BOOL daDoor12_c::openProc() {
    WWHD_FUNC(0x021287A0, BOOL, this);
    u8 action = mEventAction;
    if (action == 2 || action == 3 || action == 11)
        gabi::call(0x0252A48C, this);
    u8 room = mToRoomNo;
    if (room != 63) {
        dComIfGp_ea();
        if (!(gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 1))
            return FALSE;
    }
    if (__builtin_fabsf(speedF) < 3.814697265625e-6f) {
        if (!((getArg1() == 8 || getArg1() == 11) && mExecuteState == 1))
            sound(0x6907);
    }
    cLib_chaseF(&speedF, 20.0f, 2.0f);
    BOOL done = cLib_chaseF(&mOpen, 200.0f, speedF) != 0;
    calcMtx();
    return done;
}
VERIFY(0x021287A0, &daDoor12_c::openProc);
void daDoor12_c::openEnd() {
    WWHD_FUNC(0x021288E0, void, this);
    if (!((getArg1() == 8 || getArg1() == 11) && mExecuteState == 1))
        sound(0x6909);
    mFlags &= (u16)~1;
    mOpen = 200.0f;
    speedF = 0.0f;
}
VERIFY(0x021288E0, &daDoor12_c::openEnd);
BOOL daDoor12_c::closeProc() {
    WWHD_FUNC(0x02128974, BOOL, this);
    cLib_chaseF(&speedF, 20.0f, 2.0f);
    BOOL done = cLib_chaseF(&mOpen, 0.0f, speedF) != 0;
    calcMtx();
    return done;
}
VERIFY(0x02128974, &daDoor12_c::closeProc);
void daDoor12_c::closeEnd() {
    WWHD_FUNC(0x021289EC, void, this);
    mFlags &= (u16)~2;
    gabi::call(0x0252A550, this);
    u32 play = dComIfGp_ea();
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    gabi::call(0x025CB374, gabi::at<u8>(play + 0x599C), 4, -33, v.get());
    if ((getArg1() != 8 && getArg1() != 11) || mExecuteState == 1)
        sound(0x690A);
}
VERIFY(0x021289EC, &daDoor12_c::closeEnd);
void daDoor12_c::demoProc() {
    WWHD_FUNC(0x02128AA4, void, this);
    s32 action = gabi::call<s32>(0x0252A684, this);
    s32 staff = mStaffId;
    if (dComIfGp_evmng_getIsAddvance(staff)) {
        switch (action) {
        case 3:
            openInit();
            break;
        case 4:
            closeInit();
            break;
        case 7:
            gabi::call(0x0252A6D0, this);
            break;
        case 8:
            gabi::call(0x0252B4A4, &mKey, this);
            break;
        case 2:
            setStop();
            if (mStop.mEnabled)
                mStop.closeInit(this);
            break;
        case 1:
            mStop.openInit(this);
            break;
        case 20:
            gabi::call(0x0252A798, this, 0);
            break;
        case 21:
            gabi::call(0x0252A798, this, 1);
            break;
        }
    }
    switch (action) {
    case 3:
        if (!(mFlags & 1))
            cutEnd();
        else if (openProc()) {
            openEnd();
            cutEnd();
        }
        break;
    case 4:
        if (!(mFlags & 2))
            cutEnd();
        else if (closeProc()) {
            closeEnd();
            cutEnd();
        }
        break;
    case 8:
        if (gabi::call<BOOL>(0x0252B5B4, &mKey))
            cutEnd();
        mKey.calcMtx(this);
        break;
    case 2: {
        s32 result = mStop.closeProc(this);
        if (result == 2 && getArg1() == 8)
            gabi::call(0x025B8B68, gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x1178), 0x440);
        if (result)
            cutEnd();
        mStop.calcMtx(this);
        break;
    }
    case 1:
        if (mStop.openProc(this))
            cutEnd();
        mStop.calcMtx(this);
        break;
    case 19: {
        u32 play = dComIfGp_ea();
        if (gabi::load<u16>(play + 0x52B8) & 1) {
            mAction = 1;
            play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | 8);
            shape_angle.y = current.angle.y;
            play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) & (u16)~1);
            play = dComIfGp_ea();
            if (gabi::call<BOOL>(0x025449B0, gabi::at<u8>(play + 0x52C4))) {
                play = dComIfGp_ea();
                gabi::call(0x0254351C, gabi::at<u8>(play + 0x52E8));
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
VERIFY(0x02128AA4, &daDoor12_c::demoProc);
BOOL daDoor12_c::chkStopOpen() {
    WWHD_FUNC(0x02128F20, BOOL, this);
    u8 type = getType(), sw, room;
    if (mFrontCheck == 0) {
        sw = getSwbit();
        room = getFRoomNo();
    } else {
        sw = getSwbit2();
        room = getBRoomNo();
    }
    if (mFrontCheck == 0 && type == 2) {
        u32 play = dComIfGp_ea();
        if (gabi::load<u8>(play + 0x5292) != 0 && mEnemyTimer != 0)
            return FALSE;
        play = dComIfGp_ea();
        if (gabi::call<BOOL>(0x025C4334, gabi::at<u8>(play + 0x51CC), room) &&
            gabi::call<u32>(0x025D98E8, (s8)room) == 0) {
            u8 timer = mEnemyTimer;
            if (timer) {
                mEnemyTimer = timer - 1;
                return FALSE;
            }
            if (sw != 255)
                dComIfGs_onSwitch(sw, room);
            return TRUE;
        }
        mEnemyTimer = 65;
        return FALSE;
    }
    return sw != 255 && dComIfGs_isSwitch(sw, room);
}
VERIFY(0x02128F20, &daDoor12_c::chkStopOpen);
void daDoor12_c::setStopDemo() {
    WWHD_FUNC(0x021290B4, void, this);
    mEventAction = mFrontCheck != 0;
}
VERIFY(0x021290B4, &daDoor12_c::setStopDemo);
BOOL daDoor12_c::chkStopClose() {
    WWHD_FUNC(0x021290C8, BOOL, this);
    u8 type = getType();
    if (mStop.mpModel == nullptr || type == 3)
        return FALSE;
    u8 sw, room;
    if (mFrontCheck == 0) {
        if (type == 2)
            return FALSE;
        sw = getSwbit();
        room = getFRoomNo();
    } else {
        sw = getSwbit2();
        room = getBRoomNo();
    }
    return sw != 255 && !dComIfGs_isSwitch(sw, room);
}
VERIFY(0x021290C8, &daDoor12_c::chkStopClose);
void daDoor12_c::setEventPrm() {
    WWHD_FUNC(0x0212919C, void, this);
    if (mFrontCheck == 0) {
        mEventAction = 2;
        if (mStop.mOtherEnabled == 255)
            mStop.mOtherEnabled = chkStopB();
    } else {
        mEventAction = 3;
        if (getType() == 3)
            return;
        if (mStop.mOtherEnabled == 255)
            mStop.mOtherEnabled = chkStopF();
    }
    if (mStop.mEnabled != 0)
        return;
    if (getType() == 3) {
        mEventAction = 6;
    } else if (mStop.mOtherEnabled == 1)
        mEventAction = mEventAction + 2;
    if (getShapeType() == 1 || getShapeType() == 2 || getShapeType() == 4 || getShapeType() == 5) {
        u32 player = gabi::load<u32>(dComIfGp_ea() + 0x5B2C);
        u32 vt = gabi::load<u32>(player + 0xB4);
        s32 id = gabi::call<s32>(gabi::load<u32>(vt + 0xBC), gabi::at<u8>(player));
        if (id != -1) {
            gabi::Local<be<s32>> key;
            *key = id;
            if (gabi::call<u32>(0x025D5218, gabi::at<u8>(0x025E1234), key.get()))
                mEventAction = 11;
        }
    }
    if (mKey.mEnabled) {
        u8 type = getType();
        u32 save = gabi::load<u32>(0x101F84DC);
        if (type == 3) {
            if (!gabi::call<BOOL>(0x025B9100, gabi::at<u8>(save + 0x798), 2))
                return;
        } else if (gabi::load<u8>(save + 0x7B8) == 0)
            return;
    }
    if (!gabi::call<BOOL>(0x0252AE04, this, 12100.0f, 12100.0f, 62500.0f))
        return;
    u8 action = mEventAction;
    u16 flags = gabi::load<u16>(gabi::ea(this) + 0xFA);
    gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIdx[action]);
    u8 tool = mToolId[action];
    gabi::store<u16>(gabi::ea(this) + 0xFA, flags | 4);
    gabi::store<u8>(gabi::ea(this) + 0xFE, tool);
}
VERIFY(0x0212919C, &daDoor12_c::setEventPrm);
s32 daDoor12_c::actionWait() {
    WWHD_FUNC(0x02129398, s32, this);
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8);
    if (command == 3) {
        gabi::call(0x0252A2DC, this, 1);
        mAction = 3;
        demoProc();
        return TRUE;
    }
    if (mStop.mEnabled) {
        if (command == 2) {
            s32 staff = dComIfGp_evmng_getMyStaffId(STR(0x1000E018), nullptr, 0);
            u8 front = mFrontCheck;
            s16 y = current.angle.y;
            mStaffId = staff;
            shape_angle.y = y;
            if (front == 1)
                shape_angle.y = y + 0x7FFF;
            mAction = 3;
            demoProc();
            return TRUE;
        }
        if (chkStopOpen()) {
            setStopDemo();
            u8 action = mEventAction, tool = mToolId[action];
            s16 event = mEventIdx[action];
            fopAcM_orderOtherEventId(this, event, tool, 0xFFFF, 0, 1);
        }
        return TRUE;
    }
    if (chkStopClose()) {
        mStop.mEnabled = 1;
        mStop.closeInit(this);
        mStop.calcMtx(this);
        mAction = 2;
    } else
        setEventPrm();
    return TRUE;
}
VERIFY(0x02129398, &daDoor12_c::actionWait);
s32 daDoor12_c::actionDemo() {
    WWHD_FUNC(0x021294F0, s32, this);
    s16 event = mEventIdx[mEventAction];
    if (dComIfGp_evmng_endCheck(event)) {
        mAction = 1;
        u32 play = dComIfGp_ea();
        gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | 8);
        shape_angle.y = current.angle.y;
        return TRUE;
    }
    demoProc();
    return TRUE;
}
VERIFY(0x021294F0, &daDoor12_c::actionDemo);
s32 daDoor12_c::actionStopClose() {
    WWHD_FUNC(0x0212957C, s32, this);
    if (mStop.closeProc(this))
        mAction = 1;
    mStop.calcMtx(this);
    return TRUE;
}
VERIFY(0x0212957C, &daDoor12_c::actionStopClose);
void daDoor12_c::setKey() {
    WWHD_FUNC(0x021295D0, void, this);
    if (chkMakeKey() == 1) {
        u8 sw = getSwbit();
        if (!dComIfGs_isSwitch(sw, -1)) {
            gabi::call(0x0252B848, &mKey);
            return;
        }
    }
    if (chkMakeKey() == 2 && getSwbit() != 255) {
        if (getSwbit() >= 128) {
            gabi::call(0x0252B848, &mKey);
            return;
        }
        u8 sw = getSwbit();
        if (!dComIfGs_isSwitch(sw, -1)) {
            gabi::call(0x0252B848, &mKey);
            return;
        }
    }
    gabi::call(0x0252B5A8, &mKey);
}
VERIFY(0x021295D0, &daDoor12_c::setKey);
s32 daDoor12_c::actionInit() {
    WWHD_FUNC(0x021296B0, s32, this);
    setKey();
    mKey.calcMtx(this);
    setStop();
    mStop.calcMtx(this);
    actionWait();
    mAction = 1;
    return TRUE;
}
VERIFY(0x021296B0, &daDoor12_c::actionInit);
s32 daDoor12_c::draw() {
    WWHD_FUNC(0x0212970C, s32, this);
    u8 type = getType();
    if (!gabi::call<BOOL>(0x0252B0FC, this, type == 3))
        return TRUE;
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D70));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D74));
    auto *env = dKy_getEnvlight();
    setLightTevColorType(env, mpModelLf, &tevStr);
    mDoExt_modelUpdateDL(mpModelLf, 0);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModelRt, &tevStr);
    mDoExt_modelUpdateDL(mpModelRt, 0);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
    if (mKey.mEnabled)
        mKey.draw(this);
    if (mStop.mEnabled && mStop.mpModel != nullptr) {
        env = dKy_getEnvlight();
        setLightTevColorType(env, mStop.mpModel, &tevStr);
        mDoExt_modelUpdateDL(mStop.mpModel, 0);
    }
    return TRUE;
}
VERIFY(0x0212970C, &daDoor12_c::draw);
static s32 daDoor12_Draw(daDoor12_c *p) {
    WWHD_FUNC(0x0212982C, s32, p);
    return p->draw();
}
VERIFY(0x0212982C, daDoor12_Draw);
static s32 daDoor12_Execute(daDoor12_c *p) {
    WWHD_FUNC(0x02129830, s32, p);
    if (gabi::load<u32>(0x101FDAAC) == 0) {
        gabi::store<u32>(0x101FDAAC, 1);
        memcpy_g(gabi::at<u8>(0x101FDAB0), gabi::at<u8>(0x101B4680), 16);
    }
    s32 state = gabi::call<s32>(0x0252AC98, p);
    p->mExecuteState = state;
    switch (state) {
    case 0:
        p->mAction = 0;
        break;
    case 2:
        gabi::call(gabi::load<u32>(0x101FDAB0 + 4 * (u8)p->mAction), p);
        break;
    case 1:
        gabi::call(0x0252AD50, p);
        p->demoProc();
        break;
    default:
        JUT_ASSERT_fail(STR(0x1000DFB8), 0x450, STR(0x1000DF8C));
        break;
    }
    p->mRoomNo2 = gabi::load<u8>(0x1047E6C8);
    return TRUE;
}
VERIFY(0x02129830, daDoor12_Execute);
static s32 daDoor12_IsDelete(daDoor12_c *p) {
    WWHD_FUNC(0x02129938, s32, p);
    return TRUE;
}
VERIFY(0x02129938, daDoor12_IsDelete);
static s32 daDoor12_Delete(daDoor12_c *p) {
    WWHD_FUNC(0x02129940, s32, p);
    if (p->heap != nullptr && p->mpBgW != nullptr && gabi::load<u32>(gabi::ea(p->mpBgW)) < 0x100) {
        dBgS *bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, p->mpBgW);
    }
    const char *arc = p->getArcName();
    dComIfG_resDelete(&p->mPhase, arc);
    if (p->chkMakeKey())
        gabi::call(0x0252B494, &p->mKey);
    return TRUE;
}
VERIFY(0x02129940, daDoor12_Delete);
BOOL daDoor12_c::CreateInit() {
    WWHD_FUNC(0x021299CC, BOOL, this);
    dBgS *bgs = dComIfG_Bgsp();
    if (dBgS_Regist(bgs, mpBgW, this))
        JUT_ASSERT_fail(STR(0x1000E030), 0x306, STR(0x1000E02C));
    f32 attY = gabi::load<f32>(gabi::ea(this) + 0x394), eyeY = eyePos.y;
    gabi::store<s8>(gabi::ea(this) + 0x1C9, current.roomNo);
    mOpen = 0.0f;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0x20);
    eyePos.y = eyeY + 150.0f;
    mAction = 0;
    gabi::store<f32>(gabi::ea(this) + 0x394, attY + 150.0f);
    calcMtx();
    dBgW_Move(mpBgW);
    dBgW *w = mpBgW;
    u8 room = getFRoomNo();
    gabi::store<u16>(gabi::ea(w) + 0xB8, room);
    gabi::call(0x0252A9E0, this, 2);
    mEnemyTimer = 65;
    return TRUE;
}
VERIFY(0x021299CC, &daDoor12_c::CreateInit);
cPhs_State daDoor12_c::create() {
    WWHD_FUNC(0x02129AAC, cPhs_State, this);
    const char *arc = getArcName();
    cPhs_State phase = dComIfG_resLoad(&mPhase, arc);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (getArg1() == 9 || getArg1() == 12)
        gabi::call(0x0252AA38, this, 3);
    if (getArg1() == 8)
        gabi::call(0x025B8B7C, gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x1178), 0x440);
    if (chkMakeKey()) {
        phase = gabi::call<cPhs_State>(0x0252B484, &mKey);
        if (phase != cPhs_COMPLEATE_e)
            return phase;
    }
    current.roomNo = getFRoomNo();
    if (!fopAcM_entrySolidHeap(this, 0x0212842C, 0x2700))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02129AAC, &daDoor12_c::create);
static cPhs_State daDoor12_Create(daDoor12_c *p) {
    WWHD_FUNC(0x02129BA8, cPhs_State, p);
    if (!(p->actor_condition & 8)) {
        if (p != nullptr) {
            gabi::call(0x0252A244, p);
            p->__vtbl = 0x1000DFA8;
            gabi::call(0x0252B3D0, &p->mKey);
            gabi::call(0x0252BA44, &p->mStop);
        }
        p->actor_condition |= 8;
    }
    return p->create();
}
VERIFY(0x02129BA8, daDoor12_Create);
static void __sinit_d_a_door12_cpp() {
    WWHD_FUNC(0x02129C1C, void, (u32)0);
    sinit_header_statics(0x10463BD4, 0x101B46B0);
}
VERIFY(0x02129C1C, __sinit_d_a_door12_cpp);
static void trivial_dt(void *p, s32 flags) {
    WWHD_FUNC(0x02129CB0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02129CB0, trivial_dt);
static void daDoor12_c_dt(daDoor12_c *p, s32 flags) {
    WWHD_FUNC(0x02129CC4, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027F3628, gabi::at<u8>(gabi::ea(p) + 0x418), 0);
        gabi::call(0x025D50BC, p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02129CC4, daDoor12_c_dt);
static void empty_virtual(void *p) {
    WWHD_FUNC(0x02129D24, void, p);
}
VERIFY(0x02129D24, empty_virtual);
