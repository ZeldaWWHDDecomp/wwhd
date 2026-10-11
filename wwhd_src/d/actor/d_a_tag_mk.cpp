/** Mrs. Marie event tag. HD behaviour verified against the original; local source. */
#include "d/actor/d_a_tag_mk.h"

static u32 l_msgId;
static char* action_table[4] = {"WAIT", "MAKE", "DELETE", "SETGOAL"};
// HD: dialogue uses a global message manager rather than a searched msg_class.
static TagMkMessageManager* l_msgManager;
static TagMkHeaderZeros headerZeros;
static f32 headerMin;
static f32 headerMax;
static u8 headerByte;
static u8 headerFlag;
// HD: compiler-generated actor vtable for this translation unit.
static constexpr u32 tagMkVtable = 0x1003FA8C;


// HD: the order helper passes the full event register, unlike the shared s16 binding.
/* inlined into daTag_Mk_c event actions */
static void orderEvent(daTag_Mk_c* a, s32 event) {
    orderOtherEventRegister(a, event, 0xFF, 0xFFFF, 0, 1);
}

/* 024B0BBC */
u8 daTag_Mk_c::getSwbit() {
    return u32(mParameters) >> 8;
}
/* 024B0BC8 */
u8 daTag_Mk_c::getSwbit2() {
    return u32(mParameters) >> 16;
}
/* 024B0E94 */
u8 daTag_Mk_c::getType() {
    return u32(mParameters);
}
/* 024B0668 */
BOOL daTag_Mk_c::checkArea(f32 width, f32 depth, f32 radius) {
    daPy_py_c* player = static_cast<daPy_py_c*>(((dComIfG_inf_c*)dComIfGp_get())->play.mpPlayerPtr[0].get());
    cXyz relative;
    cXyz xz;
    cXyz normalized;
    subtractPosition(&player->current.pos, &relative, &current.pos);
    xz.x = relative.x;
    xz.y = 0.0f;
    xz.z = relative.z;
    f32 distance = PSVECSquareMag(&xz);
    if (distance > radius)
        return FALSE;
    // HD: normalization writes a hidden-result copy as well as its receiver.
    normalizePosition(&relative, &normalized);
    f32 dot = gabi::fmadds(f32(relative.x), f32(m29C.x),
                           f32(f32(relative.z) * f32(m29C.z)));
    f32 extent = f32(f32(distance * dot) * dot);
    if (extent > depth || f32(distance - extent) > width)
        return FALSE;
    s16 delta =
        s16(s32(s16(current.angle.y)) - s32(s16(player->current.angle.y)));
    s32 magnitude = delta < 0 ? -s32(delta) : s32(delta);
    return magnitude >= 0x5000;
}
/* 024B08BC */
u16 daTag_Mk_c::next_msgStatus(be<u32> *message) {
    u32 id = *message;
    if (id >= 0x1BC0 && id <= 0x1BC2) {
        *message = id + 1;
        return fopMsgStts_MSG_CONTINUES_e;
    }
    return fopMsgStts_MSG_ENDS_e;
}
/* 024B0864 */
u32 daTag_Mk_c::getMsg() {
    if (dComIfGp_event_getPreItemNo() == 0x9C) {
        dComIfGs_onEventBit(0x2D80);
        return 0x1BC0;
    }
    return 0x1BBF;
}
/* 024B065C */
void daTag_Mk_c::talkInit() {
    m294 = 0;
}
/* 024B08E8 */
u16 daTag_Mk_c::talk() {
    // HD: snapshot the manager before getMsg and the subsequent message calls.
    TagMkMessageManager* manager = l_msgManager;
    s8 state = m294;
    u16 status = 0xFF;
    if (state == 0) {
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        m290 = getMsg();
        m294 = 1;
    } else if (state != -1) {
        if (u32(l_msgId) == 0xFFFFFFFF) {
            u32 id = setMessage(manager, u32(m290), &eyePos);
            l_msgId = id;
        } else if (state == 1)
            m294 = 2;
        else if (state == 2) {
            status = u16(getMessageStatus(manager));
            if (status == 0xE) {
                u16 next = next_msgStatus(&m290);
                setMessageStatus(manager, next);
                if (getMessageStatus(manager) == fopMsgStts_MSG_CONTINUES_e)
                    setMessage(manager, u32(m290), nullptr);
            } else if (status == fopMsgStts_BOX_CLOSED_e) {
                setMessageStatus(manager, 0x13);
                m294 = -1;
            }
        }
    }
    return status;
}
/* 024B0EA0 */
void daTag_Mk_c::setTagWpEvId() {
    // HD: retain the otherwise unused play lookup before snapshotting save data.
    dComIfGp_ea();
    dSv_info_c* save = &((TagMkSaveRoot*)(uintptr_t)dComIfGs_ea())->mInfo;
    if (u8(save->mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[0]) != 0x3E)
        mEventIdx = s16(dComIfGp_evmng_getEventIdx("tagwp2", 0xFF));
    else {
        s32 count = getTriforceNum(&save->mSavedata.mPlayer.mCollect);
        dEvent_manager_c* events = dComIfGp_getPEvtManager();
        mEventIdx =
            s16(getEventIndexRegister(events, count < 8 ? "tagwp" : "tagwp3", 0xFF));
    }
}
/* 024B031C */
void daTag_Mk_c::demoInitSetgoal() {
    fopAc_ac_c* item = fopAcM_SearchByID(m2B4);
    if (item)
        dComIfGp_evmng_setGoal(&item->current.pos);
}
/* 024B02D0 */
void daTag_Mk_c::demoInitDelete() {
    fopAc_ac_c* item = fopAcM_SearchByID(m2B4);
    if (item)
        fopAcM_delete(item);
}
/* 024B01C4 */
void daTag_Mk_c::demoInitMake() {
    cXyz pos;
    pos.x = -3310.0f;
    pos.y = 1463.0f;
    pos.z = -198484.0f;
    csXyz angle;
    angle.x = 0;
    angle.y = 0x4000;
    angle.z = 0;
    s32 room = s8(current.roomNo);
    fopAc_ac_c* item =
        fopAcM_createItemForKP2(&pos, dItemNo_JOY_PENDANT_e, room, &angle, nullptr, 10.0f, 10.0f, -2.1f, 0);
    if (!item)
        JUTAssertion::showAssert("d_a_tag_mk.cpp", 0x167, "a_item");
    m2B4 = fopAcM_GetID(item);
    dComIfGp_event_setTalkPartner(item);
}
/* 024B03D8 */
BOOL daTag_Mk_c::demoProcMake() {
    s32 staff = mStaffIdx;
    dComIfGp_evmng_cutEnd(staff);
    return FALSE;
}
/* 024B0160 */
void daTag_Mk_c::demoInitWait() {
    s32 staff = mStaffIdx;
    // HD: GHS reads the low signed half of the integer timer.
    be<s16>* timer = (be<s16>*)dComIfGp_evmng_getMyIntegerP(staff, "Timer");
    mCutEndTimer = timer ? s16(timer[1]) : 0;
}
/* 024B0380 */
BOOL daTag_Mk_c::demoProcWait() {
    s16 timer = mCutEndTimer;
    if (timer > 0)
        mCutEndTimer = s16(timer - 1);
    else {
        s32 staff = mStaffIdx;
        dComIfGp_evmng_cutEnd(staff);
    }
    return FALSE;
}
/* 024B0114 */
s32 daTag_Mk_c::getNowEventAction() {
    s32 staff = mStaffIdx;
    return dComIfGp_evmng_getMyActIdx(staff, action_table, 4, FALSE, 1);
}
/* 024B0414 */
void daTag_Mk_c::demoProc() {
    s32 action = getNowEventAction(), staff = mStaffIdx;
    if (dComIfGp_evmng_getIsAddvance(staff)) {
        switch (action) {
        case 0:
            demoInitWait();
            break;
        case 1:
            demoInitMake();
            break;
        case 2:
            demoInitDelete();
            break;
        case 3:
            demoInitSetgoal();
            break;
        }
    }
    switch (action) {
    case 0:
        demoProcWait();
        break;
    case 1:
        demoProcMake();
        break;
    default:
        staff = mStaffIdx;
        dComIfGp_evmng_cutEnd(staff);
        break;
    }
}
/* 024B05E4 */
BOOL daTag_Mk_c::actionEvent() {
    s32 event = mEventIdx;
    if (dComIfGp_evmng_endCheck(event)) {
        mActionTypes = ACT_WAIT_e;
        dComIfGp_event_reset();
    } else
        demoProc();
    return TRUE;
}
/* 024B0538 */
BOOL daTag_Mk_c::actionReady() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        mActionTypes = ACT_EVENT_e;
        dComIfGs_onEventBit(0x1E02);
        const char* name = m2B8;
        mStaffIdx = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
        demoProc();
    } else
        orderEvent(this, s16(mEventIdx));
    return TRUE;
}
/* 024B0078 */
BOOL daTag_Mk_c::actionHunt() {
    daPy_py_c* player = static_cast<daPy_py_c*>(((dComIfG_inf_c*)dComIfGp_get())->play.mpPlayerPtr[0].get());
    if ((player->checkResetFlg0(daPy_py_c::daPyRFlg0_FRONT_ROLL_CRASH)) &&
        fopAcM_searchActorDistanceXZ(this, player) < 150.0f) {
        // HD: keep the full returned register for ordering before the stored s16 truncation.
        s32 event = getEventIndexRegister(dComIfGp_getPEvtManager(), "MK_PENDANT", 0xFF);
        mEventIdx = s16(event);
        orderEvent(this, event);
        mActionTypes = ACT_READY_e;
    }
    return TRUE;
}
/* 024B001C */
BOOL daTag_Mk_c::actionArrival() {
    if (dComIfGs_isEventBit(0x1E04) && !dKy_daynight_check())
        mActionTypes = ACT_HUNT_e;
    return TRUE;
}
/* 024B0A20 */
BOOL daTag_Mk_c::actionVillaTalk() {
    if (!dComIfGp_evmng_ChkPresentEnd())
        return FALSE;
    if (talk() == fopMsgStts_BOX_CLOSED_e) {
        mActionTypes = dComIfGs_isEventBit(0x2D80) ? ACT_WAIT_e : ACT_VILLA_e;
        dComIfGp_event_reset();
    }
    return TRUE;
}
/* 024B07E8 */
BOOL daTag_Mk_c::actionVilla() {
    if (eventInfo.checkCommandTalk()) {
        mActionTypes = ACT_VILLATALK_e;
        talkInit();
    } else if (checkArea(6400.0f, 12100.0f, 62500.0f))
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e);
    return TRUE;
}
/* 024B0B54 */
BOOL daTag_Mk_c::actionTagWp2() {
    s32 event = mEventIdx;
    if (dComIfGp_evmng_endCheck(event)) {
        mActionTypes = ACT_WAIT_e;
        dComIfGp_event_reset();
    }
    return TRUE;
}
/* 024B0ACC */
BOOL daTag_Mk_c::actionTagWp() {
    dComIfG_inf_c* play = (dComIfG_inf_c*)dComIfGp_get();
    // HD: snapshot the command before the player pointer lookup.
    u16 command = u16(eventInfo.mCommand);
    daPy_py_c* player = static_cast<daPy_py_c*>(play->play.mpPlayerPtr[0].get());
    if (command == dEvtCmd_INDEMO_e)
        mActionTypes = ACT_TAGWP2_e;
    // HD: compare the distance in double precision.
    else if (f64(fopAcM_searchActorDistanceXZ(this, player)) < 5000.0)
        orderEvent(this, s16(mEventIdx));
    return TRUE;
}
/* 024B0BD4 */
BOOL daTag_Mk_c::actionDaichi() {
    s16 state = m296;
    switch (state) {
    case 0:
    case 10:
        if (dComIfGs_isTmpBit(0x440)) {
            m296 = s16(s16(m296) + 1);
            mDoAud_bgmAllMute(30);
        }
        break;
    case 1: {
        u8 sw = getSwbit();
        s32 room = s8(current.roomNo);
        if (dComIfGs_isSwitch(sw, room))
            m296 = s16(s16(m296) + 1);
        break;
    }
    case 2:
    case 11:
        if (s16(mCutEndTimer) > 0)
            mCutEndTimer = s16(s16(mCutEndTimer) - 1);
        else {
            m296 = s16(state + 1);
            if (dComIfGs_isTmpBit(0x440)) {
                mDoAud_subBgmStart(0x80000019);
                dComIfGs_onTmpBit(0x420);
            }
        }
        break;
    case 3:
    case 12: {
        u8 sw = getSwbit2();
        s32 room = s8(current.roomNo);
        if (dComIfGs_isSwitch(sw, room)) {
            m296 = s16(s16(m296) + 1);
            if (dComIfGs_isTmpBit(0x420)) {
                mDoAud_subBgmStop();
                dComIfGs_onTmpBit(0x410);
            }
        }
        break;
    }
    }
    return TRUE;
}
/* 024B0D90 */
BOOL daTag_Mk_c::execute() {
    switch (u8(mActionTypes)) {
    case ACT_ARRIVAL_e:
        actionArrival();
        break;
    case ACT_HUNT_e:
        actionHunt();
        break;
    case ACT_READY_e:
        actionReady();
        break;
    case ACT_EVENT_e:
        actionEvent();
        break;
    case ACT_VILLA_e:
        actionVilla();
        break;
    case ACT_VILLATALK_e:
        actionVillaTalk();
        break;
    case ACT_TAGWP_e:
        actionTagWp();
        break;
    case ACT_TAGWP2_e:
        actionTagWp2();
        break;
    case ACT_DAICHI_e:
        actionDaichi();
        break;
    }
    // HD: the empty actionWait and demo common helpers have no separate entries.
    return TRUE;
}
/* 024B1324 */
BOOL daTag_Mk_Draw(daTag_Mk_c *a) {
    return TRUE;
}
/* 024B0E60 */
BOOL daTag_Mk_Execute(daTag_Mk_c *a) {
    a->execute();
    return TRUE;
}
/* 024B0E84 */
BOOL daTag_Mk_IsDelete(daTag_Mk_c *a) {
    return TRUE;
}
/* 024B0E8C */
// HD: deletion callback is empty; GHS emits the deleting destructor separately.
BOOL daTag_Mk_Delete(daTag_Mk_c *a) {
    return TRUE;
}
/* 024B0F4C */
s32 daTag_Mk_Create(daTag_Mk_c *a) {
    u32 flags = a->actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (a) {
            fopAc_ac_c_ct(a);
            flags = a->actor_condition;
            a->__vtbl = tagMkVtable;
        }
        a->actor_condition = flags | fopAcCnd_INIT_e;
    }
    u8 type = a->getType();
    if (type == 3) {
        if (dComIfGs_isEventBit(0x2D80))
            a->mActionTypes = daTag_Mk_c::ACT_WAIT_e;
        else {
            a->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB;
            u16 heading = a->current.angle.y;
            f32 attention = f32(a->attention_info.position.y), eye = f32(a->eyePos.y),
                height = 210.0f;
            a->mActionTypes = daTag_Mk_c::ACT_VILLA_e;
            a->attention_info.flags = fopAc_Attn_TALKFLAG_NOTALK_e | fopAc_Attn_ACTION_SPEAK_e;
            a->m29C.y = 0.0f;
            a->m29C.x = cM_ssin(heading);
            a->m29C.z = cM_scos(heading);
            a->attention_info.position.y = f32(attention + height);
            a->eyePos.y = f32(eye + height);
        }
    } else if (type == 2) {
        if (dComIfGs_isEventBit(0x3380) &&
            !dComIfGs_isEventBit(0x2D08) &&
            dComIfGs_isEventBit(0x2D02)) {
            a->mActionTypes = daTag_Mk_c::ACT_TAGWP_e;
            a->setTagWpEvId();
        } else
            a->mActionTypes = daTag_Mk_c::ACT_WAIT_e;
    } else if (type == 4) {
        a->mActionTypes = daTag_Mk_c::ACT_DAICHI_e;
        dComIfGs_offTmpBit(0x420);
        dComIfGs_offTmpBit(0x410);
        u8 second = a->getSwbit2();
        s32 room = s8(a->current.roomNo);
        if (dComIfGs_isSwitch(second, room))
            a->m296 = 20;
        else {
            u8 first = a->getSwbit();
            room = s8(a->current.roomNo);
            if (dComIfGs_isSwitch(first, room)) {
                a->mCutEndTimer = 60;
                a->m296 = 10;
            } else {
                a->m296 = 0;
                a->mCutEndTimer = 180;
            }
        }
    } else
        a->mActionTypes = dComIfGs_isEventBit(0x1E02) ? daTag_Mk_c::ACT_WAIT_e : daTag_Mk_c::ACT_ARRIVAL_e;
    a->current.angle.z = 0;
    a->current.angle.x = 0;
    a->m2B8.v = gabi::guest_str("TagMk");
    a->shape_angle.x = 0;
    a->shape_angle.z = 0;
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated code of this translation unit.
/* 024B1290 */
void __sinit_d_a_tag_mk_cpp() {
    // HD: keep the compiler's nonsequential zero-store order.
    headerZeros.word2 = 0;
    headerZeros.word0 = 0;
    headerZeros.word3 = 0;
    headerZeros.word1 = 0;
    __register_global_object(0x101D1ED0);
    headerMin = -3.1415927f;
    headerMax = 3.1415927f;
    initializeHeaderByte(&headerByte);
    __register_global_object(0x101D1EDC);
    initializeHeaderFlag(&headerFlag);
    __register_global_object(0x101D1EE8);
}

/* 024B132C */
void daTag_Mk_dt(daTag_Mk_c *a, s32 flags) {
    if (a) {
        __dt(static_cast<fopAc_ac_c*>(a));
        if (flags & 1)
            operator_delete(a);
    }
}


/* WWHD glue
str 1003FAA0 "MK_PENDANT"
str 1003FB20 "tagwp2"
str 1003FA84 "TagMk"
str 1003FACC "Timer"
str 1003FAE8 "a_item"
str 1003FAF0 "d_a_tag_mk.cpp"
str 1003FB18 "tagwp"
str 1003FB28 "tagwp3"
data action_table 101D1EC0
data l_msgId 1046E5D8
data l_msgManager 101F4B5C
data headerZeros 1046E5E8
data headerMin 1046E5DC
data headerMax 1046E5E0
data headerByte 1046E5E4
data headerFlag 1046E5E5
ext 025D7A58 void orderOtherEventRegister(daTag_Mk_c*, s32, s32, s32, s32, s32)
ext 025D8E6C fopAc_ac_c* fopAcM_createItemForKP2(cXyz*, s32, s32, csXyz*, cXyz*, f32, f32, f32, s32)
ext 025F7DB0 u32 setMessage(TagMkMessageManager*, u32, cXyz*)
ext 025F795C u32 getMessageStatus(TagMkMessageManager*)
ext 025F74D0 void setMessageStatus(TagMkMessageManager*, u32)
ext 025B7E00 s32 getTriforceNum(dSv_player_collect_c*)
ext 02543F10 s32 getEventIndexRegister(dEvent_manager_c*, const char*, s32)
ext 0201ADE0 void subtractPosition(const cXyz*, cXyz*, const cXyz*)
ext 0201B31C void normalizePosition(cXyz*, cXyz*)
ext 028ED6F8 void initializeHeaderByte(be<u8>*)
ext 028EAB2C void initializeHeaderFlag(be<u8>*)
ext 025E1960 void mDoAud_bgmAllMute(s32)
ext 025E1918 void mDoAud_subBgmStart(u32)
ext 025E1928 void mDoAud_subBgmStop()
*/
