/**
 * AND switch actor 2: six states for switch conditions, delays and events.
 * GameCube names with HD behavior and layouts; verified against WWHD.
 */
#include "d/actor/d_a_andsw2.h"

static Andsw2SaveData* l_saveInfo;
static u32 l_actionGuard;
static Andsw2ActionTable l_action;
// HD: checked immutable function-pointer words copied into the guarded action table.
static Andsw2Action l_actionInit[] = {
    {0x02051B24}, {0x02051BD8}, {0x02051CB0}, {0x02051D6C}, {0x02051DE8}, {0x02051E4C},
};
static Andsw2HeaderStatics l_headerStatics;
static Andsw2HeaderRecords l_headerRecords;

/* 020519EC */
u8 daAndsw2_c::getSwbit2() {
    return u32(mParameters) >> 24;
}
/* 020519F8 */
u8 daAndsw2_c::getSwbit() {
    return u32(mParameters) >> 16;
}
/* 02051A04 */
u8 daAndsw2_c::getTopSw() {
    // HD: cache each getter result; GameCube calls the selected getter twice.
    u8 sw = getSwbit2();
    if (sw != 0xFF)
        return sw;
    sw = getSwbit();
    return sw != 0xFF ? u8(sw + 1) : u8(0xFF);
}
/* 02051A54 */
u8 daAndsw2_c::getNum() {
    return u32(mParameters);
}
/* 02051A60 */
BOOL daAndsw2_c::chkAllSw2() {
    s32 top = getTopSw();
    s32 remaining = getNum();
    if (remaining == 0xFF || remaining == 0 || top == 0xFF)
        return FALSE;
    for (s32 i = 0; remaining > 0; i++, remaining--) {
        Andsw2SaveData* save = l_saveInfo;
        s32 room = current.roomNo;
        if (!Andsw2HD::isSwitch(&save->info, top + i, room))
            return FALSE;
    }
    return TRUE;
}
/* 02051B0C */
u8 daAndsw2_c::getTimer() {
    return u16(home.angle.z);
}
/* 02051B18 */
u8 daAndsw2_c::getType() {
    return u32(mParameters) >> 8;
}
/* 02051B24 */
BOOL daAndsw2_actionOnAll(daAndsw2_c* i_this) {
    if (i_this->chkAllSw2()) {
        u8 timer = i_this->getTimer();
        if (timer != 0xFF) {
            // HD: store the action before the timer value.
            i_this->mAction = ACT_TIMER;
            i_this->mTimer = s16(u32(timer) * 15);
        } else if (s16(i_this->mEventIdx) != -1) {
            i_this->mAction = ACT_ORDER;
        } else {
            u8 sw = i_this->getSwbit();
            Andsw2SaveData* save = l_saveInfo;
            s32 room = i_this->current.roomNo;
            Andsw2HD::onSwitch(&save->info, sw, room);
            u8 type = i_this->getType();
            i_this->mAction = type == TYPE_CONTINUOUS ? ACT_OFF : ACT_WAIT;
        }
    }
    return TRUE;
}
/* 02051BD8 */
BOOL daAndsw2_actionTimer(daAndsw2_c* i_this) {
    if (i_this->getType() == TYPE_CONTINUOUS && !i_this->chkAllSw2()) {
        i_this->mAction = ACT_ON_ALL;
    } else if (s16(i_this->mTimer) > 0) {
        i_this->mTimer = s16(s16(i_this->mTimer) - 1);
    } else if (s16(i_this->mEventIdx) != -1) {
        i_this->mAction = ACT_ORDER;
    } else {
        u8 sw = i_this->getSwbit();
        Andsw2SaveData* save = l_saveInfo;
        s32 room = i_this->current.roomNo;
        Andsw2HD::onSwitch(&save->info, sw, room);
        if (i_this->getType() == TYPE_CONTINUOUS)
            i_this->mAction = ACT_WAIT;
    }
    return TRUE;
}
/* 02051CA4 */
u8 daAndsw2_c::getEventNo() {
    return u16(home.angle.x);
}
/* 02051CB0 */
BOOL daAndsw2_actionOrder(daAndsw2_c* i_this) {
    if (i_this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        i_this->mAction = ACT_EVENT;
        u8 sw = i_this->getSwbit();
        Andsw2SaveData* save = l_saveInfo;
        s32 room = i_this->current.roomNo;
        Andsw2HD::onSwitch(&save->info, sw, room);
    } else if (i_this->getType() == TYPE_CONTINUOUS && !i_this->chkAllSw2()) {
        i_this->mAction = ACT_ON_ALL;
    } else {
        u8 ev = i_this->getEventNo();
        s32 idx = i_this->mEventIdx;
        Andsw2HD::orderOtherEventId(i_this, idx, ev);
    }
    return TRUE;
}
/* 02051D6C */
BOOL daAndsw2_actionEvent(daAndsw2_c* i_this) {
    s32 idx = i_this->mEventIdx;
    dComIfG_inf_c* play = Andsw2HD::getGameInfo();
    if (Andsw2HD::endCheck(&play->play.mEvtManager, idx)) {
        u8 type = i_this->getType();
        i_this->mAction = type == TYPE_CONTINUOUS ? ACT_OFF : ACT_WAIT;
        play = Andsw2HD::getGameInfo();
        play->play.mEvtCtrl.reset();
    }
    return TRUE;
}
/* 02051DE8 */
BOOL daAndsw2_actionOff(daAndsw2_c* i_this) {
    if (!i_this->chkAllSw2()) {
        i_this->mAction = ACT_ON_ALL;
        u8 sw = i_this->getSwbit();
        Andsw2SaveData* save = l_saveInfo;
        s32 room = i_this->current.roomNo;
        Andsw2HD::offSwitch(&save->info, sw, room);
    }
    return TRUE;
}
/* 02051E4C */
BOOL daAndsw2_actionWait(daAndsw2_c* i_this) {
    return TRUE;
}
/* 02051E54 */
BOOL daAndsw2_c::execute() {
    if (!l_actionGuard) {
        l_actionGuard = 1;
        Andsw2HD::copyActionTable(&l_action, l_actionInit);
    }
    u8 action = mAction;
    l_action[action](this);
    return TRUE;
}
/* 02051ED4 */
BOOL daAndsw2_IsDelete(daAndsw2_c* i_this) {
    return TRUE;
}
/* 02051EDC */
// HD: this entry is empty; the compiler-generated deleting destructor is separate.
BOOL daAndsw2_Delete(daAndsw2_c* i_this) {
    return TRUE;
}
/* 02051EE4 */
s32 daAndsw2_c::create() {
    u8 sw = getSwbit();
    fopAcM_ct(this, daAndsw2_c);
    u8 type = getType();
    if (type == TYPE_ONE_OFF) {
        BOOL on = FALSE;
        if (sw != 0xFF) {
            Andsw2SaveData* save = l_saveInfo;
            s32 room = current.roomNo;
            on = Andsw2HD::isSwitch(&save->info, sw, room);
        }
        mAction = (sw == 0xFF || on) ? ACT_WAIT : ACT_ON_ALL;
        u8 ev = getEventNo();
        dComIfG_inf_c* play = Andsw2HD::getGameInfo();
        mEventIdx = s16(Andsw2HD::getEventIdx(&play->play.mEvtManager, 0, ev));
    } else if (type == TYPE_CONTINUOUS) {
        if (sw == 0xFF)
            mAction = ACT_WAIT;
        else {
            s32 room = current.roomNo;
            Andsw2SaveData* save = l_saveInfo;
            BOOL on = Andsw2HD::isSwitch(&save->info, sw, room);
            mAction = on ? ACT_OFF : ACT_ON_ALL;
        }
        u8 ev = getEventNo();
        dComIfG_inf_c* play = Andsw2HD::getGameInfo();
        mEventIdx = s16(Andsw2HD::getEventIdx(&play->play.mEvtManager, 0, ev));
    } else
        mAction = ACT_WAIT;
    // HD: retain the rotation-store order used by this build.
    shape_angle.z = 0;
    current.angle.z = 0;
    current.angle.x = 0;
    shape_angle.x = 0;
    return cPhs_COMPLEATE_e;
}
// HD: compiler-generated common-header initializer; preserve its nonsequential zero-store order.
/* 020520CC */
void __sinit_d_a_andsw2_cpp() {
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    Andsw2HD::registerGlobalObject(l_headerRecords.records[0]);
    l_headerStatics.mPiLo = -3.1415927410125732f;
    l_headerStatics.mPiHi = 3.1415927410125732f;
    Andsw2HD::initHeaderObjectA(&l_headerStatics.mObjectA);
    Andsw2HD::registerGlobalObject(l_headerRecords.records[1]);
    Andsw2HD::initHeaderObjectB(&l_headerStatics.mObjectB);
    Andsw2HD::registerGlobalObject(l_headerRecords.records[2]);
}
/* 02052160 */
BOOL daAndsw2_Draw(daAndsw2_c* i_this) {
    return TRUE;
}
// HD: compiler-generated deleting destructor.
/* 02052168 */
void daAndsw2_c_dtor(daAndsw2_c* i_this, u32 flags) {
    if (!i_this)
        return;
    Andsw2HD::destroyActor(i_this);
    if (flags & 1)
        Andsw2HD::deleteActor(i_this);
}

/* WWHD glue (tools/verify/autoglue.py):
data l_saveInfo 101F84DC
data l_actionGuard 101FD9F4
data l_action 101FD9F8
data l_actionInit 1018FD18
data l_headerStatics 10461450
data l_headerRecords 1018FD50
*/
