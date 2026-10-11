/* Message tag, tww member names with the HD actor base (+0x11C). */
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/gc_api.h"
struct daTag_Msg_c : fopAc_ac_c {
    be<u8> mAction;
    u8 padding[3];
    u16 getMessage();
    u32 getType2();
    u32 getSwbit();
    u32 getSwbit2();
    u16 getEventFlag();
    u32 getEventNo();
    s32 rangeCheck();
    s32 otherCheck();
    s32 arrivalTerms();
    const char* myDemoName();
};
WWHD_OFFSET(daTag_Msg_c, mAction, 0x3AC);
WWHD_SIZE(daTag_Msg_c, 0x3B0);

enum TagMsgAction { ACTION_WAIT, ACTION_ARRIVAL, ACTION_HUNT, ACTION_EVENT };
enum TagMsgStatus {
    fopMsgStts_MSG_TYPING_e = 6,
    fopMsgStts_MSG_DISPLAYED_e = 14,
    fopMsgStts_MSG_DESTROYED_e = 19
};
enum TagMsgMessage { MSG_RED_BOAT = 0x1902, MSG_EVENT_BLOCKED = 0x9C5, MSG_LOW_ATTENTION = 0x836 };
struct TagMsgSaveData {
    u8 prefix[0x20];
    dSv_info_c info;
};
WWHD_OFFSET(TagMsgSaveData, info, 0x20);
struct TagMsgCameraStatus {
    be<u32> mFlags;
    u8 rest[0x30];
};
WWHD_SIZE(TagMsgCameraStatus, 0x34);
struct TagMsgEvent {
    u8 flags;
    char mName[0x17];
};
WWHD_SIZE(TagMsgEvent, 0x18);
struct TagMsgEventInfo {
    be<s32> num;
    gptr<TagMsgEvent> events;
};
WWHD_SIZE(TagMsgEventInfo, 8);
struct TagMsgStageVtable {
    u8 baseSlots[0x1CC];
    be<u32> getEventInfo;
};
WWHD_OFFSET(TagMsgStageVtable, getEventInfo, 0x1CC);
struct TagMsgStage {
    gptr<TagMsgStageVtable> vtable;
};
inline TagMsgEventInfo* TagMsg_getEventInfo(dComIfG_inf_c* game) {
    auto* stage = reinterpret_cast<TagMsgStage*>(&game->play.mStageData);
    return gabi::call_ptr<TagMsgEventInfo*>(stage->vtable->getEventInfo, stage);
}
inline TagMsgCameraStatus* TagMsg_getCameraStatus(dComIfG_inf_c* game, s32 camera) {
    // HD camera-info array is not yet part of the shared play layout; preserve 32-bit wrapping.
    return gabi::at<TagMsgCameraStatus>(gabi::ea(game) + 0x5B00 + u32(camera * 0x34));
}
using TagMsgSavePtr = gptr<TagMsgSaveData>;
using TagMsgControllerPtr = gptr<msg_class>;
struct TagMsgActionTable {
    be<u32> functions[4];
};
WWHD_SIZE(TagMsgActionTable, 16);
struct TagMsgZeroStorage {
    be<u32> values[4];
    auto& operator[](u32 i) { return values[i]; }
};
struct TagMsgPiStorage {
    be<f32> values[2];
    auto& operator[](u32 i) { return values[i]; }
};
struct TagMsgObjectStorage {
    be<u8> values[2];
    auto& operator[](u32 i) { return values[i]; }
};
struct TagMsgDtorRecord {
    u8 bytes[12];
};
struct TagMsgRecordStorage {
    TagMsgDtorRecord values[3];
    auto& operator[](u32 i) { return values[i]; }
};
dComIfG_inf_c* TagMsg_getGame();
u32 TagMsg_messageSet(msg_class*, u32, cXyz*);
u32 TagMsg_messageStatus(msg_class*);
void TagMsg_setMessageStatus(msg_class*, s32);
s32 TagMsg_actorAngle(fopAc_ac_c*, fopAc_ac_c*);
void TagMsg_copyActions(TagMsgActionTable*, const TagMsgActionTable*, u32);
void TagMsg_registerDestructor(TagMsgDtorRecord*);
void TagMsg_constructHeaderA(be<u8>*);
void TagMsg_constructHeaderB(be<u8>*);

void TagMsg_setEventName(dEvt_info_c*, const char*);

inline const char* TagMsg_guestString(const char* text) {
    return gabi::at<const char>(gabi::guest_addr(text));
}
