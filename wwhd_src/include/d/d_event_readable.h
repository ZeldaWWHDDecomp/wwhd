// Unit-local typed order view; the shared dEvt_order_c is opaque.
// HD: the control layout remains GameCube-sized; event types include a fourth item button.
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/MSL_C/string.h"

// HD: the fourth item button shifts the following GameCube event types.
enum EventMode {
    dEvtMode_NONE_e,
    dEvtMode_TALK_e,
    dEvtMode_DEMO_e,
    dEvtMode_COMPULSORY_e,
};
enum EventMove { dEvtMove_NOMOVE_e, dEvtMove_MOVE_e, dEvtMove_FORCE_e };

enum EventType {
    dEvtType_TALK_e = 0,
    dEvtType_DOOR_e = 1,
    dEvtType_OTHER_e = 2,
    dEvtType_COMPULSORY_e = 3,
    dEvtType_POTENTIAL_e = 4,
    dEvtType_ITEM_e = 5,
    dEvtType_SHOWITEM_X_e = 6,
    dEvtType_SHOWITEM_Y_e = 7,
    dEvtType_SHOWITEM_Z_e = 8,
    dEvtType_SHOWITEM_HD_e = 9,
    dEvtType_CATCH_e = 10,
    dEvtType_TREASURE_e = 11,
    dEvtType_PHOTO_e = 12,
    dEvtType_CHANGE_e = 13,
};
struct EventOrder {
    be<u16> mEventType, mFlag, mHindFlag;
    u8 _006[2];
    be<u32> mActor1, mActor2;
    be<s16> mEventId;
    be<u16> mPriority;
    be<s8> mNextOrderIdx;
    be<u8> mEventInfoIdx;
    u8 _016[2];
};
WWHD_OFFSET(EventOrder, mEventType, 0);
WWHD_OFFSET(EventOrder, mFlag, 2);
WWHD_OFFSET(EventOrder, mHindFlag, 4);
WWHD_OFFSET(EventOrder, mActor1, 8);
WWHD_OFFSET(EventOrder, mActor2, 0xC);
WWHD_OFFSET(EventOrder, mEventId, 0x10);
WWHD_OFFSET(EventOrder, mPriority, 0x12);
WWHD_OFFSET(EventOrder, mNextOrderIdx, 0x14);
WWHD_OFFSET(EventOrder, mEventInfoIdx, 0x15);
WWHD_SIZE(EventOrder, 0x18);

struct EventStageEntry {
    be<u8> field_0x0;
    be<char> mName[15];
    be<u8> field_0x10, field_0x11, field_0x12, mSpawnSwitchNo;
    u8 _014[4];
};
struct EventStageInfo {
    be<s32> num;
    be<u32> events;
};
WWHD_OFFSET(EventStageEntry, field_0x10, 0x10);
WWHD_SIZE(EventStageEntry, 0x18);
WWHD_SIZE(EventStageInfo, 8);

struct EventControl : dEvt_control_c {
    void remove();
    s32 giveItemCut(u32 item);
    s32 order(u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2, u32 eventIdx);
    s32 orderOld(u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                 u32 eventName);
    u32 convPId(u32 pid);
    BOOL talkEnd();
    BOOL demoEnd();
    BOOL endProc();
    BOOL checkChange();
    BOOL beforeFlagProc(u32 order);
    u32 getPId(u32 ac);
    void setParam(u32 order);
    void afterFlagProc(u32 order);
    BOOL demoCheck(u32 order);
    BOOL changeProc();
    BOOL commonCheck(u32 order, u32 cond, u32 cmd);
    BOOL talkCheck(u32 order);
    BOOL doorCheck(u32 order);
    BOOL potentialCheck(u32 order);
    BOOL itemCheck(u32 order);
    BOOL talkXyCheck(u32 order);
    // HD: unique binding name distinguishes the order-taking overload for autoglue.
    BOOL photoCheckOrder(u32 order);
    BOOL catchCheck(u32 order);
    BOOL checkStart();
    BOOL soundProc();
    BOOL check();
    BOOL photoCheck();
    s32 moveApproval(u32 ac);
    s32 compulsory(u32 actor, u32 eventName, u32 p3);
    u32 getStageEventDt();
    u32 nextStageEventDt(u32 idxp);
    u8 getTactFreeMStick(s32 which);
    u8 getTactFreeCStick(s32 which);
};
WWHD_SIZE(EventControl, 0xF4);

struct EventInfo : dEvt_info_c {
    void setEventName(u32 evtName);
    u32 getEventName();
};
WWHD_SIZE(EventInfo, 0x18);

// HD: preserve full argument/result registers where the shared GameCube API narrows them.
// This unit-local manager view does not read or change the shared manager layout.
struct EventManager {
    u32 getEventIdx(const char* name, u32 evNo) {
        return gabi::call<u32>(0x02543F10, this, name, evNo);
    }
    u32 getEventData(u32 idx) { return gabi::call<u32>(0x02544044, this, idx); }
    void endProc(u32 id, s32 x) { gabi::call(0x0254465C, this, id, x); }
    void cancelStaff(const char* name) { gabi::call(0x02544944, this, name); }
    void issueStaff(const char* name) { gabi::call(0x02544908, this, name); }
    BOOL order(u32 id) { return gabi::call<BOOL>(0x02544534, this, id); }
    u32 getEventEndSound(u32 id) { return gabi::call<u32>(0x02544628, this, id); }
};

// HD: narrow views of opaque shared objects; only the members used here are exposed.
struct EventStageMethods {
    u8 _000[0x1CC];
    be<u32> getEventInfo;
};
struct EventStageData {
    gptr<EventStageMethods> methods;
    u8 _004[0x7C - 4];
};
struct EventAttention {
    u8 _000[0x144];
    be<u8> mCatchChgItem;
};
WWHD_OFFSET(EventStageMethods, getEventInfo, 0x1CC);
WWHD_SIZE(EventStageMethods, 0x1D0);
WWHD_OFFSET(EventStageData, methods, 0);
WWHD_SIZE(EventStageData, 0x7C);
WWHD_OFFSET(EventAttention, mCatchChgItem, 0x144);
WWHD_SIZE(EventAttention, 0x145);

// HD: this call site reads the archive count beyond the GameCube picture-info prefix.
struct EventPictureArchive {
    u8 _000[0x3C030C];
    be<u8> mPictureNum;
};
struct EventPictureStore {
    EventPictureArchive* getPictureInfo() {
        return gabi::call<EventPictureArchive*>(0x02720144, this);
    }
};
struct EventSaveContext {
    u8 _000[0x12C0];
    EventPictureStore pictures;
};
WWHD_OFFSET(EventPictureArchive, mPictureNum, 0x3C030C);
WWHD_SIZE(EventPictureArchive, 0x3C030D);
WWHD_OFFSET(EventSaveContext, pictures, 0x12C0);
WWHD_SIZE(EventSaveContext, 0x12C1);

// HD: unit-local bindings retain the original one- and two-register call forms.
inline u32 eventItemPartner(u32 ac) { return gabi::call<u32>(0x025D7C98, ac); }
inline void eventDeleteItem(u32 ac) { gabi::call(0x0218432C, ac); }
inline void eventStopQuake(u32 v, s32 x) { gabi::call(0x025CB610, v, x); }
inline void eventStartSound(u32 id) { gabi::call(0x025E1988, id); }
