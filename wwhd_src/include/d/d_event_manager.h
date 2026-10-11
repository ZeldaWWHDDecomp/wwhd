/* GameCube event-manager names; HD appends a current-event pointer and uses SafeStrings in debug action lookup. */
#pragma once
#include "d/d_event_data.h"
#include "gc_api/f_op/f_op_actor.h"
namespace event_manager {
using namespace gabi;
template <class T> inline T *view(u32 address) { return at<T>(address); }
struct dEvent_exception_c {
  be<s32> mEventInfoIdx, field_0x4;
  be<u32> mState;
  void init();
  u32 getEventName();
  s32 setStartDemo(s32 index);
};
struct dEvent_manager_c {
  union {
    dEvDtBase_c mList;
    be<u32> mListWords[8];
  };
  be<u32> mCameraPlay;
  dEvent_exception_c mException;
  cXyz mGoal;
  dEvDtFlag_c mFlag;
  be<u32> mCurrentEvent;
  u32 getCurrentEventNameHD();
  u32 getCurrentCutHD(s32 staff);
  void closeProc(u32 event);
  void setGoal(u32 vec);
  u32 getEventData(s32 index);
  u32 order(s32 index);
  u32 endCheckOld(u32 name);
  u32 startCheckOld(u32 name);
  u32 getEventPrio(s32 index);
  u32 getEventEndSound(s32 index);
  u32 getIsAddvance(s32 staff);
  u32 getMyActName(s32 staff);
  u32 getMyNowCutName(s32 staff);
  u32 getGoal();
  u32 cancelStaff(u32 data);
  u32 ChkPresentEnd();
  u32 CancelPresent();
  u32 checkStartDemo();
  void debugBfProc();
  u32 startCheck(s32 id);
  u32 endCheck(s32 id);
  void runProc();
  void setData(u32 data);
  u32 create();
  void remove();
  u32 getMySubstanceP(s32 staff, u32 name, s32 type);
  u32 getMySubstanceNum(s32 staff, u32 name);
  void cutEnd(s32 staff);
  u32 getMyDataP(s32 staff, u32 name, u32 initial);
  u32 getSubstance(u32 data, s32 requested);
  u32 specialCast(u32 name, u32 move);
  u32 issueStaff(u32 name);
  void setPrmStaff(u32 param, s32 staff);
  void startProc(u32 event);
  void mainProc();
  s32 getEventIdx(u32 name, s32 mapid);
  void endProc(s32 id, u32 force);
  u32 getToolId(s32 id, s32 target);
  u32 specialCast_Shutter(s32 profile, u32 move);
  s32 getMyStaffId(u32 name, u32 actor, u32 id);
  void exceptionProc();
  s32 getMyActIdx(s32 staff, u32 names, s32 count, u32 force, u32 prefix);
};
struct dEv_seach_prm {
  be<u32> mName, mMask, mValue;
  be<u16> mCastInFlag;
  u8 pad[2];
};
struct find_shutter_param {
  be<u16> profName;
  u8 pad[2];
  be<u32> actor;
};
struct ShutterWork {
  cXyz goal;
  find_shutter_param param;
};
struct DebugStrings {
  EventSafeString a, b, c, d;
};
struct dStage_Event_dt_c {
  be<u8> field_0x0;
  be<char> mName[15];
  be<u8> field_0x10, field_0x11, field_0x12, mSpawnSwitchNo;
  u8 tail[4];
};
struct dStage_EventInfo_c {
  be<s32> num;
  be<u32> events;
};
struct StageMethods {
  u8 prefix[0x15c];
  gfn getStageInfo;
  u8 gap[0x1cc - 0x160];
  gfn getEventInfo;
};
struct Stage {
  be<u32> vtable;
};
struct StageInfo {
  u8 prefix[12];
  be<u32> mFlags;
};
struct GameInfo {
  u8 prefix[0x514c];
  be<s8> mDebugEvent;
  u8 gap[3];
  Stage stage;
  u8 controlGap[0x51d0 - 0x5154];
  dEvt_control_c mEvent;
  dEvent_manager_c mEventManager;
  u8 playerGap[0x5b2c - 0x52c4 - sizeof(dEvent_manager_c)];
  be<u32> player;
};
struct EventInitializers {
  be<f32> first, second;
  be<u8> byteState, flagState;
  u8 pad[2];
  be<u32> words[4];
};
WWHD_SIZE(dEvent_exception_c, 12);
WWHD_SIZE(dEvent_manager_c, 0x540);
WWHD_OFFSET(dEvent_manager_c, mCurrentEvent, 0x53c);
WWHD_SIZE(dEv_seach_prm, 16);
WWHD_SIZE(find_shutter_param, 8);
WWHD_SIZE(ShutterWork, 20);
WWHD_SIZE(DebugStrings, 32);
WWHD_SIZE(dStage_Event_dt_c, 24);
WWHD_OFFSET(GameInfo, mEvent, 0x51d0);
WWHD_OFFSET(GameInfo, mEventManager, 0x52c4);
enum EventState : u32 { NONE_e, ORDER_e, PLAY_e, UNK3_e, CLOSE_e };
enum TalkAction : u32 { ACT_WAIT, ACT_TALK0, ACT_TALK1 };
enum ExceptionState : u32 {
  EXCEPTION_ORDER,
  EXCEPTION_WAIT_START,
  EXCEPTION_WAIT_END
};
u32 dEv_extra_createCB(u32 actor);
u32 constructExceptionHD(u32 self);
u32 constructSearchParamHD(u32 self, u32 name, u32 actor, u32 flags);
u32 constructManagerHD(u32 self);
void eventManagerStaticInitializer();
void destroyManagerHD(u32 self, u32 flags);
u32 findObjectCallBack(u32 actor, u32 param);
u32 extraOnObjectCallBack(u32 actor, u32 param);
u32 extraOffObjectCallBack(u32 actor, u32 name);
u32 allOffObjectCallBack(u32 actor);
u32 findShutterCallBack(u32 actor, u32 param);
s32 dEv_talkman_get_action(u32 action);
enum GuestIdentity : u32 {
  EmptyCurrentEvent = 0x1004DD00,
  AllActorName = 0x1004DCF0,
  AllReleaseName = 0x1004DCF4,
  ShutterUpperBound = 0x1004DCF8,
  ShutterLowerBound = 0x1004DCFC,
  CutAssertionFile = 0x1004DFAC,
  CutAssertionMessage = 0x1004DF9C,
  InitialFloat0 = 0x1004DFC4,
  InitialFloat1 = 0x1004DFC8,
  InitializerVector0 = 0x101D6408,
  InitializerVector1 = 0x101D6414,
  InitializerVector2 = 0x101D6420,
  InitializerByte = 0x10475920,
  InitializerFlag = 0x10475921,
  StageArchiveName = 0x1004DF68,
  EventResourceName = 0x1004DF70,
  SubstanceAssertionFile = 0x1004DED4,
  SubstanceAssertionMessage = 0x1004DED0,
  ShutterDoorName = 0x1004DEC0,
  StaffAssertionFile = 0x1004DEFC,
  StaffAssertionMessage = 0x1004DF10,
  SearchParamAssertionMessage = 0x1004DEF0,
  DevParamMaskName = 0x1004DF1C,
  IntegerDataAssertionMessage = 0x1004DEF4,
  CastInName = 0x1004DEE8,
  EndAssertionFile = 0x1004DF88,
  EndAssertionMessage = 0x1004DF84,
  AllEndName = 0x1004DF80,
  TalkmanName = 0x1004DD84,
  TalkmanActions = 0x101D63C8,
  ExceptionAssertionFile = 0x1004DDAC,
  StageInfoAssertionMessage = 0x1004DDD4,
  BossShutterStartName = 0x1004DDC0,
  ShutterAssertionFile = 0x1004DEAC,
  PlayerAssertionMessage = 0x1004DEA8,
  ShutterGoalDistance = 0x1004DEA4,
  ExceptionProcAssertionFile = 0x1004DF2C,
  ExceptionNameAssertionMessage = 0x1004DF40,
  MeetShipName = 0x1004DF4C,
  LookTetraName = 0x1004DF5C,
  SafeStringVtable = 0x1004DD18,
  DebugStaffName = 0x1004DD44,
  DebugEventName = 0x1004DD6C,
  ActionAssertionFile = 0x1004DD4C,
  ActionAssertionMessage = 0x1004DD60,
  PrefixAssertionFile = 0x1004DD30,
  PrefixAssertionMessage = 0x1004DD14
};
constexpr u32 debugBeforeCallbackAddress = 0x02544AB8;
inline EventInitializers *initializers() {
  return view<EventInitializers>(0x10475918);
}
inline be<u32> &saveAddress() { return *view<be<u32>>(0x101F84DC); }
inline be<s8> &stayRoom() { return *view<be<s8>>(0x1047E6C8); }
inline f32 constant(u32 address) { return *view<be<f32>>(address); }
inline be<u32> *exceptionNames() { return view<be<u32>>(0x101D60B0); }

struct ChunkDescriptor {
  be<u32> offset;
  be<s32> count;
};
struct ResourceChunks {
  ChunkDescriptor chunks[7];
};
struct SinCosSample {
  be<f32> sine, cosine;
};
inline u32 sineSampleAddress(u32 angle) {
  return 0x104A44F8 + (angle & 0xfff8);
}
inline u32 saveDataAddress() { return u32(saveAddress()) + 0x20; }
inline u32 eventBitsAddress() { return u32(saveAddress()) + 0x644; }
enum ShutterProfile : u32 {
  fpcNm_DOOR10_e = 0x12c,
  fpcNm_DOOR12_e = 0x12d,
  fpcNm_KDDOOR_e = 0x130,
  fpcNm_KNOB00_e = 0x131
};

WWHD_OFFSET(dEvent_manager_c, mList, 0);
WWHD_OFFSET(dEvent_manager_c, mCameraPlay, 0x20);
WWHD_OFFSET(dEvent_manager_c, mException, 0x24);
WWHD_OFFSET(dEvent_manager_c, mGoal, 0x30);
WWHD_OFFSET(dEvent_manager_c, mFlag, 0x3c);
WWHD_OFFSET(dEv_seach_prm, mMask, 4);
WWHD_OFFSET(dEv_seach_prm, mCastInFlag, 12);
WWHD_OFFSET(find_shutter_param, actor, 4);
WWHD_OFFSET(ShutterWork, param, 12);
WWHD_OFFSET(dStage_Event_dt_c, mName, 1);
WWHD_OFFSET(dStage_Event_dt_c, mSpawnSwitchNo, 0x13);
WWHD_OFFSET(StageMethods, getStageInfo, 0x15c);
WWHD_OFFSET(StageMethods, getEventInfo, 0x1cc);
WWHD_OFFSET(GameInfo, mDebugEvent, 0x514c);
WWHD_OFFSET(GameInfo, stage, 0x5150);
WWHD_OFFSET(GameInfo, player, 0x5b2c);
WWHD_SIZE(EventInitializers, 28);
WWHD_SIZE(ChunkDescriptor, 8);
WWHD_SIZE(ResourceChunks, 56);
WWHD_SIZE(SinCosSample, 8);
namespace sdk {
/* 0273AD10 [v d_event_manager] */
template <class R = void, class... A> inline R allocate(A... args) {
  return gabi::call<R>(0x0273AD10, args...);
}
/* 0254273C [v d_event_manager] */
template <class R = void, class... A> inline R initializeEventList(A... args) {
  return gabi::call<R>(0x0254273C, args...);
}
/* 02543378 [v d_event_manager] */
template <class R = void, class... A> inline R constructException(A... args) {
  return gabi::call<R>(0x02543378, args...);
}
/* 02544044 [v d_event_manager] */
template <class R = void, class... A> inline R getEventData(A... args) {
  return gabi::call<R>(0x02544044, args...);
}
/* 02543F10 [v d_event_manager] */
template <class R = void, class... A> inline R getEventIdx(A... args) {
  return gabi::call<R>(0x02543F10, args...);
}
/* 025440C8 [v d_event_manager] */
template <class R = void, class... A> inline R endCheck(A... args) {
  return gabi::call<R>(0x025440C8, args...);
}
/* 0254407C [v d_event_manager] */
template <class R = void, class... A> inline R startCheck(A... args) {
  return gabi::call<R>(0x0254407C, args...);
}
/* 02542EAC [v d_event_manager] */
template <class R = void, class... A> inline R getCurrentCut(A... args) {
  return gabi::call<R>(0x02542EAC, args...);
}
/* 0273AA24 [v d_event_manager] */
template <class R = void, class... A> inline R assertion(A... args) {
  return gabi::call<R>(0x0273AA24, args...);
}
/* 025D5218 [v d_event_manager] */
template <class R = void, class... A> inline R actorJudge(A... args) {
  return gabi::call<R>(0x025D5218, args...);
}
/* 025432B4 [v d_event_manager] */
template <class R = void, class... A> inline R talkmanAction(A... args) {
  return gabi::call<R>(0x025432B4, args...);
}
/* 025200D4 [v d_event_manager] */
template <class R = void, class... A> inline R getGameInfo(A... args) {
  return gabi::call<R>(0x025200D4, args...);
}
/* 028F026C [v d_event_manager] */
template <class R = void, class... A> inline R constructVector(A... args) {
  return gabi::call<R>(0x028F026C, args...);
}
/* 028ED6F8 [v d_event_manager] */
template <class R = void, class... A> inline R initializeByte(A... args) {
  return gabi::call<R>(0x028ED6F8, args...);
}
/* 028EAB2C [v d_event_manager] */
template <class R = void, class... A> inline R initializeFlag(A... args) {
  return gabi::call<R>(0x028EAB2C, args...);
}
/* 0273AF40 [v d_event_manager] */
template <class R = void, class... A> inline R deallocate(A... args) {
  return gabi::call<R>(0x0273AF40, args...);
}
/* 025C109C [v d_event_manager] */
template <class R = void, class... A> inline R searchObjectName(A... args) {
  return gabi::call<R>(0x025C109C, args...);
}
/* 0201ADE0 [v d_event_manager] */
template <class R = void, class... A> inline R subtractVector(A... args) {
  return gabi::call<R>(0x0201ADE0, args...);
}
/* 02544114 [v d_event_manager] */
template <class R = void, class... A> inline R exceptionProc(A... args) {
  return gabi::call<R>(0x02544114, args...);
}
/* 0253FF34 [v d_event_manager] */
template <class R = void, class... A> inline R checkEvent(A... args) {
  return gabi::call<R>(0x0253FF34, args...);
}
/* 02543D80 [v d_event_manager] */
template <class R = void, class... A> inline R mainProc(A... args) {
  return gabi::call<R>(0x02543D80, args...);
}
/* 0254351C [v d_event_manager] */
template <class R = void, class... A> inline R initializeException(A... args) {
  return gabi::call<R>(0x0254351C, args...);
}
/* 025408CC [v d_event_manager] */
template <class R = void, class... A> inline R initializeFlags(A... args) {
  return gabi::call<R>(0x025408CC, args...);
}
/* 0252447C [v d_event_manager] */
template <class R = void, class... A> inline R getStageResource(A... args) {
  return gabi::call<R>(0x0252447C, args...);
}
/* 025443B4 [v d_event_manager] */
template <class R = void, class... A> inline R setData(A... args) {
  return gabi::call<R>(0x025443B4, args...);
}
/* 02543944 [v d_event_manager] */
template <class R = void, class... A> inline R getMyData(A... args) {
  return gabi::call<R>(0x02543944, args...);
}
/* 025439E0 [v d_event_manager] */
template <class R = void, class... A> inline R getSubstance(A... args) {
  return gabi::call<R>(0x025439E0, args...);
}
/* 02540848 [v d_event_manager] */
template <class R = void, class... A> inline R setFlag(A... args) {
  return gabi::call<R>(0x02540848, args...);
}
/* 02543730 [v d_event_manager] */
template <class R = void, class... A> inline R castShutter(A... args) {
  return gabi::call<R>(0x02543730, args...);
}
/* 02543638 [v d_event_manager] */
template <class R = void, class... A> inline R constructSearchParam(A... args) {
  return gabi::call<R>(0x02543638, args...);
}
/* 02543B38 [v d_event_manager] */
template <class R = void, class... A> inline R setParamStaff(A... args) {
  return gabi::call<R>(0x02543B38, args...);
}
/* 02543838 [v d_event_manager] */
template <class R = void, class... A> inline R specialCast(A... args) {
  return gabi::call<R>(0x02543838, args...);
}
/* 025409B4 [v d_event_manager] */
template <class R = void, class... A> inline R initializeStaff(A... args) {
  return gabi::call<R>(0x025409B4, args...);
}
/* 02542624 [v d_event_manager] */
template <class R = void, class... A> inline R eventFinished(A... args) {
  return gabi::call<R>(0x02542624, args...);
}
/* 02543708 [v d_event_manager] */
template <class R = void, class... A> inline R closeProc(A... args) {
  return gabi::call<R>(0x02543708, args...);
}
/* 02543C6C [v d_event_manager] */
template <class R = void, class... A> inline R startProc(A... args) {
  return gabi::call<R>(0x02543C6C, args...);
}
/* 02542878 [v d_event_manager] */
template <class R = void, class... A> inline R advanceCut(A... args) {
  return gabi::call<R>(0x02542878, args...);
}
/* 025426C4 [v d_event_manager] */
template <class R = void, class... A> inline R specialStaffProc(A... args) {
  return gabi::call<R>(0x025426C4, args...);
}
/* 02542D88 [v d_event_manager] */
template <class R = void, class... A> inline R getMyStaffId(A... args) {
  return gabi::call<R>(0x02542D88, args...);
}
/* 02542EDC [v d_event_manager] */
template <class R = void, class... A> inline R getMyActIdx(A... args) {
  return gabi::call<R>(0x02542EDC, args...);
}
/* 02543280 [v d_event_manager] */
template <class R = void, class... A> inline R cutEnd(A... args) {
  return gabi::call<R>(0x02543280, args...);
}
/* 025BA0C0 [v d_event_manager] */
template <class R = void, class... A> inline R checkSwitch(A... args) {
  return gabi::call<R>(0x025BA0C0, args...);
}
/* 025B9E38 [v d_event_manager] */
template <class R = void, class... A> inline R onSwitch(A... args) {
  return gabi::call<R>(0x025B9E38, args...);
}
/* 02543714 [v d_event_manager] */
template <class R = void, class... A> inline R setGoal(A... args) {
  return gabi::call<R>(0x02543714, args...);
}
/* 0253F124 [v d_event_manager] */
template <class R = void, class... A> inline R getActorProcessId(A... args) {
  return gabi::call<R>(0x0253F124, args...);
}
/* 025433C0 [v d_event_manager] */
template <class R = void, class... A> inline R getExceptionName(A... args) {
  return gabi::call<R>(0x025433C0, args...);
}
/* 025D7A58 [v d_event_manager] */
template <class R = void, class... A> inline R orderOtherEventId(A... args) {
  return gabi::call<R>(0x025D7A58, args...);
}
/* 025D77DC [v d_event_manager] */
template <class R = void, class... A> inline R orderOtherEvent(A... args) {
  return gabi::call<R>(0x025D77DC, args...);
}
/* 025B8B68 [v d_event_manager] */
template <class R = void, class... A> inline R onEventBit(A... args) {
  return gabi::call<R>(0x025B8B68, args...);
}
/* 02542E94 [v d_event_manager] */
template <class R = void, class... A> inline R getCurrentEventName(A... args) {
  return gabi::call<R>(0x02542E94, args...);
}
} // namespace sdk
} // namespace event_manager
