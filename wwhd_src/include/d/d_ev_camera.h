#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
namespace event_camera {
using GuestByte = be<u8>;
using GuestWord = be<u32>;
struct EventParameter {
    GuestByte mName[16];
    gptr<GuestWord> mValue;
};
WWHD_SIZE(EventParameter, 0x14);
WWHD_OFFSET(EventParameter, mValue, 0x10);
struct PauseWork {
    GuestByte mTimerSet;
    u8 _01[3];
    GuestWord mStay;
    GuestWord mTimer;
};
WWHD_SIZE(PauseWork, 0xC);
struct VectorWords {
    GuestWord x, y, z;
};
WWHD_SIZE(VectorWords, 0xC);
struct ActorStringFrame {
    u8 _00[8];
    GuestByte mBuffer[0x18];
};
WWHD_SIZE(ActorStringFrame, 0x20);
struct EventVaList {
    GuestByte mGpr;
    GuestByte mFpr;
    u8 _02[2];
    gptr<GuestWord> mpOverflow;
    gptr<GuestWord> mpRegisterSave;
};
WWHD_SIZE(EventVaList, 0xC);
struct EventVarArgsFrame {
    u8 _00[8];
    EventVaList mArgs;
    u8 _14[4];
    GuestWord mRegisters[8];
    u8 _38[0x58];
};
WWHD_SIZE(EventVarArgsFrame, 0x90);
WWHD_OFFSET(EventVarArgsFrame, mArgs, 8);
WWHD_OFFSET(EventVarArgsFrame, mRegisters, 0x18);
// HD: retain the GHS entry stack and explicit register-save area for this variadic API.
inline GuestWord* entryStackArguments() {
    return gabi::at<GuestWord>(gabi::cpu->r[1] + 8);
}
struct HeaderStatics {
    be<f32> mPi[2];
    GuestByte mFirstObject;
    GuestByte mSecondObject;
    u8 _0A[2];
    GuestWord mZeroObject[4];
};
WWHD_SIZE(HeaderStatics, 0x1C);
WWHD_OFFSET(HeaderStatics, mZeroObject, 0xC);
struct DestructorRecord {
    u8 mBytes[0xC];
};
struct DestructorRecords {
    DestructorRecord mRecords[3];
};
WWHD_SIZE(DestructorRecords, 0x24);
struct EventString {
    char text[16];
};
enum : u32 { DEBUG_EVENT_PARAMS = 0x20000000, CLEAR_EVENT_FLAG = 0x200000 };
enum ActorName : u32 {
    ACTOR_PLAYER = 0x40504C41,
    ACTOR_STARTER = 0x40535441,
    ACTOR_PARTNER = 0x40504152,
    ACTOR_TALK_PARTNER = 0x4054414C,
    ACTOR_TARGET = 0x40544152,
    ACTOR_ITEM = 0x40495445,
    ACTOR_LINK = 0x4C696E6B
};
} // namespace event_camera
using namespace event_camera;
u32 eventCameraAssert(const char*, s32, const char*);
void eventCameraStrcpy(char*, const char*);
fopAc_ac_c* eventCameraConvPId(dEvt_control_c*, u32);
u32 eventCameraActorByName(const char*, u32, u32);
s32 eventCameraSearchStyle(void*, u32);
void eventCameraTalktoCamera(dCamera_c*, s32);
void eventCameraSubtract(cXyz*, cXyz*, cXyz*);
void eventCameraNormalize(cXyz*, cXyz*);
void eventCameraScale(cXyz*, cXyz*, f32);
void eventCameraAdd(cXyz*, cXyz*, cXyz*);
GuestWord* eventCameraVaArg(EventVaList*, u32);
void eventCameraRegisterGlobal(DestructorRecord*);
void eventCameraConstructFirst(GuestByte*);
void eventCameraConstructSecond(GuestByte*);
void eventCameraFree(void*);
