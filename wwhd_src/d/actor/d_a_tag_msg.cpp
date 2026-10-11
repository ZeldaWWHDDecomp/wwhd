/* Message trigger tag, audited WWHD translation unit. */
#include "d/actor/d_a_tag_msg.h"

// HD: the message manager replaces tww's msg_class lookup and l_msg state.
static TagMsgControllerPtr messageController;
static TagMsgSavePtr saveData;
static fpc_ProcID l_msgId;
static u8 msg_mode;
static u32 actionGuard;
static TagMsgActionTable l_action;
static const TagMsgActionTable actionInitial = {{0x024B1A04, 0x024B19A8, 0x024B1730, 0x024B1394}};
static TagMsgZeroStorage headerZero;
static TagMsgPiStorage headerPi;
static TagMsgObjectStorage headerObjects;
static TagMsgRecordStorage headerRecords;

/* 024B18DC */
u32 daTag_Msg_c::getEventNo() { return (u32)mParameters >> 24; }
/* 024B153C */
u32 daTag_Msg_c::getSwbit() { return ((u32)mParameters >> 8) & 255; }
/* 024B181C */
u32 daTag_Msg_c::getSwbit2() { return ((u32)mParameters >> 16) & 255; }
/* 024B1388 */
u32 daTag_Msg_c::getType2() { return ((u32)mParameters >> 6) & 3; }
/* 024B18E8 */
const char* daTag_Msg_c::myDemoName() {

    dComIfG_inf_c* game = TagMsg_getGame();
    TagMsgEventInfo* info = TagMsg_getEventInfo(game);
    u32 event = getEventNo();
    if (getMessage() == MSG_RED_BOAT)
        return TagMsg_guestString("RT_TALK");
    if (!info || event == 255 || info->num < (s32)event)
        return TagMsg_guestString("DEFAULT_MSG");
    return info->events.get()[event].mName;
}
/* 024B1380 */
u16 daTag_Msg_c::getMessage() { return (u16)home.angle.x; }
/* 024B1828 */
u16 daTag_Msg_c::getEventFlag() { return (u16)home.angle.z; }
/* 024B1830 */
s32 daTag_Msg_c::arrivalTerms() {

    u32 sw = getSwbit2();
    u32 flag = getEventFlag();
    if (sw != 255) {
        TagMsgSaveData* save = saveData;
        s32 room = (s8)current.roomNo;
        if (!save->info.isSwitch(sw, room))
            return 0;
    }
    if (flag != 65535) {
        TagMsgSaveData* save = saveData;
        if (!save->info.mSavedata.mEvent.isEventBit(flag))
            return 0;
    }
    return 1;
}
/* 024B1548 */
s32 daTag_Msg_c::rangeCheck() {

    dComIfG_inf_c* game = TagMsg_getGame();
    daPy_py_c* player = (daPy_py_c*)game->play.mpPlayerPtr[0].get();
    gabi::Local<cXyz> diff, flat;
    cXyz_mi(&player->current.pos, diff.get(), &current.pos);
    f32 height = diff->y;
    f32 x = diff->x, z = diff->z;
    if (height < 0.0f)
        height = -height;
    flat->y = 0.0f;
    flat->z = z;
    flat->x = x;
    f32 horizontal = PSVECSquareMag(flat.get());
    f32 sx = scale.x;
    sx = sx * sx;
    f32 radius = sx * 10000.0f;
    if (!(horizontal < radius))
        return 0;
    f32 sy = scale.y;
    f32 top = sy * 100.0f;
    // HD: reject only greater height; unordered NaN comparisons retain the verified result.
    if (height > top)
        return 0;
    return 1;
}
/* 024B1630 */
s32 daTag_Msg_c::otherCheck() {

    dComIfG_inf_c* game = TagMsg_getGame();
    fopAc_ac_c* target = game->play.mPlayerInfo[0].mpPlayer;
    s32 angle = TagMsg_actorAngle(this, target);
    game = TagMsg_getGame();
    daPy_py_c* player = (daPy_py_c*)game->play.mpPlayerPtr[0].get();
    if (getType2() & 1)
        return 1;
    JUT_ASSERT(0xCC, player); // HD: assertion line
    if (getMessage() == MSG_RED_BOAT) {
        s32 diff = (s16)(angle - (s16)home.angle.y);
        if (diff < 0)
            diff = (s16)-diff;
        if (diff > 0x3000)
            return 0;
    }
    s32 diff = (s16)(angle + 0x7FFF - player->current.angle.y);
    if (diff < 0)
        diff = (s16)-diff;
    if (diff > 0x1000)
        return 0;
    return 1;
}
/* 024B1394 */
static s32 daTag_Msg_actionEvent(daTag_Msg_c* a_this) {

    msg_class* controller = messageController;
    u32 message = a_this->getMessage();
    u32 mode = msg_mode;
    bool advance = false;
    switch (mode) {
    case 0: {
        dComIfG_inf_c* game = TagMsg_getGame();
        s32 camera = game->play.mPlayerInfo[0].mCameraID;
        game = TagMsg_getGame();
        u32 flags = TagMsg_getCameraStatus(game, camera)->mFlags;
        advance = (flags & 4) != 0;
        break;
    }
    case 1: {
        u32 id = TagMsg_messageSet(controller, message, &a_this->attention_info.position);
        l_msgId = id;
        advance = id != 0xFFFFFFFF;
        break;
    }
    case 2:
        advance = true;
        break;
    case 3:
        advance = TagMsg_messageStatus(controller) == fopMsgStts_MSG_TYPING_e;
        break;
    case 4: {
        if (TagMsg_messageStatus(controller) == fopMsgStts_MSG_DISPLAYED_e) {
            u32 old = msg_mode;
            msg_mode = old + 1;
            TagMsg_setMessageStatus(controller, fopMsgStts_MSG_ENDS_e);
        }
        break;
    }
    }
    if (advance) {
        u32 old = msg_mode;
        msg_mode = old + 1;
    }
    if (TagMsg_messageStatus(controller) == fopMsgStts_BOX_CLOSED_e) {
        TagMsg_setMessageStatus(controller, fopMsgStts_MSG_DESTROYED_e);
        dComIfG_inf_c* game = TagMsg_getGame();
        game->play.mEvtCtrl.onEventFlag(8);
        if (a_this->getMessage() == MSG_RED_BOAT) {
            game = TagMsg_getGame();
            daPy_py_c* player = (daPy_py_c*)game->play.mpPlayerPtr[0].get();
            u32 state = player->mNoResetFlg0;
            player->mNoResetFlg0 = state & ~0x08000000u;
        }
        u32 type = a_this->getType2();
        a_this->mAction = (type & 1) ? ACTION_WAIT : ACTION_HUNT;
    }
    return 1;
}
/* 024B1730 */
static s32 daTag_Msg_actionHunt(daTag_Msg_c* a_this) {

    if (a_this->eventInfo.mCommand == 1) {
        a_this->mAction = ACTION_EVENT;
        u32 sw = a_this->getSwbit();
        if (sw != 255) {
            s32 room = (s8)a_this->current.roomNo;
            TagMsgSaveData* save = saveData;
            save->info.onSwitch(sw, room);
        }
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        msg_mode = 0; // HD: no l_msg reset
        if (a_this->getMessage() == MSG_RED_BOAT) {
            dComIfG_inf_c* game = TagMsg_getGame();
            daPy_py_c* player = (daPy_py_c*)game->play.mpPlayerPtr[0].get();
            u32 flags = player->mNoResetFlg0;
            player->mNoResetFlg0 = flags | 0x08000000;
        }
    } else if (a_this->rangeCheck() && a_this->otherCheck()) {
        if (a_this->getType2() & 1)
            fopAcM_orderSpeakEvent(a_this);
        a_this->eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }
    return 1;
}
/* 024B19A8 */
static s32 daTag_Msg_actionArrival(daTag_Msg_c* a_this) {

    if (a_this->arrivalTerms()) {
        const char* name = a_this->myDemoName();
        TagMsg_setEventName(&a_this->eventInfo, name);
        a_this->mAction = ACTION_HUNT;
        daTag_Msg_actionHunt(a_this);
    }
    return 1;
}
/* 024B1A04 */
static s32 daTag_Msg_actionWait(daTag_Msg_c* a_this) { return 1; }
/* 024B1C90 */
static s32 daTag_Msg_Draw(daTag_Msg_c* a_this) { return 1; }
/* 024B1A0C */
static s32 daTag_Msg_Execute(daTag_Msg_c* a_this) {

    // HD: set the guard before copying the initial function words into mutable storage.
    if (!actionGuard) {
        actionGuard = 1;
        TagMsg_copyActions(&l_action, &actionInitial, sizeof(l_action));
    }
    u32 action = a_this->mAction;
    gabi::call_ptr(l_action.functions[action], a_this);
    return TRUE;
}
/* 024B1A8C */
static s32 daTag_Msg_IsDelete(daTag_Msg_c* a_this) { return 1; }
/* 024B1A94 */
static s32 daTag_Msg_Delete(daTag_Msg_c* a_this) { return 1; }
/* 024B1A9C */
static s32 daTag_Msg_Create(daTag_Msg_c* a_this) {

    u32 condition = a_this->actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(a_this)) {
            fopAc_ac_c_ct(a_this);
            condition = a_this->actor_condition;
            a_this->__vtbl = 0x1003FB4C;
        }
        a_this->actor_condition = condition | fopAcCnd_INIT_e;
    }
    u32 sw = a_this->getSwbit();
    bool inactive = false;
    if (a_this->getMessage() == MSG_EVENT_BLOCKED) {
        TagMsgSaveData* save = saveData;
        inactive = save->info.mSavedata.mEvent.isEventBit(0x502) != 0;
    }
    if (!inactive && sw != 255) {
        TagMsgSaveData* save = saveData;
        s32 room = (s8)a_this->current.roomNo;
        inactive = save->info.isSwitch(sw, room) != 0;
    }
    if (inactive) {
        a_this->shape_angle.x = 0;
        a_this->current.angle.x = 0;
        a_this->shape_angle.z = 0;
        a_this->attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_CHECK_e;
        a_this->current.angle.z = 0;
        a_this->mAction = ACTION_WAIT;
    } else {
        a_this->current.angle.z = 0;
        a_this->mAction = ACTION_ARRIVAL;
        a_this->shape_angle.z = 0;
        a_this->current.angle.x = 0;
        a_this->shape_angle.x = 0;
        a_this->attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_CHECK_e;
    }
    if (a_this->getMessage() != MSG_LOW_ATTENTION) {
        f32 attention = a_this->attention_info.position.y, eye = a_this->eyePos.y;
        attention = attention + 150.0f;
        eye = eye + 150.0f;
        a_this->attention_info.position.y = attention;
        a_this->eyePos.y = eye;
    }
    return 4;
}
/* 024B1BFC */
static void __sinit_d_a_tag_msg_cpp() {

    // HD: GHS zeroes the common header object before constructing the others.
    headerZero[2] = 0;
    headerZero[0] = 0;
    headerZero[3] = 0;
    headerZero[1] = 0;
    TagMsg_registerDestructor(&headerRecords[0]);
    headerPi[0] = -3.1415927410125732f;
    headerPi[1] = 3.1415927410125732f;
    TagMsg_constructHeaderA(&headerObjects[0]);
    TagMsg_registerDestructor(&headerRecords[1]);
    TagMsg_constructHeaderB(&headerObjects[1]);
    TagMsg_registerDestructor(&headerRecords[2]);
}
/* 024B1C98 */
static void daTag_Msg_c_dtor(daTag_Msg_c* a_this, u32 flags) {

    if (a_this) {
        __dt(static_cast<fopAc_ac_c*>(a_this));
        if (flags & 1)
            operator_delete(a_this);
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 1003FB70
str 1003FB68 "player"
str 1003FB80
str 1003FB88
data messageController 101F4B5C
data saveData 101F84DC
data l_msgId 1046E5F8
data msg_mode 1046E604
data actionGuard 101FDCB8
data l_action 101FDCBC
data actionInitial 101D1F24
data headerZero 1046E608
data headerPi 1046E5FC
data headerObjects 1046E605
data headerRecords 101D1F54
ext 0253E9B0 void TagMsg_setEventName(dEvt_info_c*, const char*)
ext 025200D4 dComIfG_inf_c* TagMsg_getGame()
ext 025F7DB0 u32 TagMsg_messageSet(msg_class*, u32, cXyz*)
ext 025F795C u32 TagMsg_messageStatus(msg_class*)
ext 025F74D0 void TagMsg_setMessageStatus(msg_class*, s32)
ext 025D6894 s32 TagMsg_actorAngle(fopAc_ac_c*, fopAc_ac_c*)
ext C000A848 void TagMsg_copyActions(TagMsgActionTable*, const TagMsgActionTable*, u32)
ext 028F026C void TagMsg_registerDestructor(TagMsgDtorRecord*)
ext 028ED6F8 void TagMsg_constructHeaderA(be<u8>*)
ext 028EAB2C void TagMsg_constructHeaderB(be<u8>*)
*/
// clang-format on
