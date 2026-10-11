#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"

namespace event_camera_style {
using EventInt = be<s32>;
struct StageEvent {
    u8 _00[0x10];
    be<u8> field_0x10;
    be<u8> field_0x11;
    be<u8> field_0x12;
    be<u8> mSpawnSwitchNo;
    be<s8> field_0x14;
    u8 _15[3];
};
WWHD_SIZE(StageEvent, 0x18);
struct StageEventInfo {
    be<s32> num;
    gptr<StageEvent> events;
};
WWHD_SIZE(StageEventInfo, 8);
struct StageVtable {
    u8 _00[0x1CC];
    gfn getEventInfo;
};
WWHD_OFFSET(StageVtable, getEventInfo, 0x1CC);
struct StageView {
    gptr<StageVtable> mpVtable;
};
struct CameraInfoView {
    u8 _00[0x14];
    cXyz mCameraPos;
    cXyz mCameraTarget;
    be<f32> mCameraFovy;
    be<s16> mCameraBank;
    u8 _32[2];
};
WWHD_SIZE(CameraInfoView, 0x34);
WWHD_OFFSET(CameraInfoView, mCameraPos, 0x14);
WWHD_OFFSET(CameraInfoView, mCameraFovy, 0x2C);
WWHD_OFFSET(CameraInfoView, mCameraBank, 0x30);
struct TactWork {
    be<u32> mMode;
    be<u32> mTimer;
    be<u32> mMirror;
};
WWHD_SIZE(TactWork, 0xC);
struct VectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(VectorWords, 0xC);
struct EngineEntry {
    be<s16> mThisDelta;
    be<s16> mVtableIndex;
    union {
        be<u32> mFunction;
        struct {
            be<s16> _04;
            be<s16> mVtableOffset;
        };
    };
};
WWHD_SIZE(EngineEntry, 8);
WWHD_OFFSET(EngineEntry, mFunction, 4);
WWHD_OFFSET(EngineEntry, mVtableOffset, 6);
struct EngineVtableEntry {
    be<u32> _00;
    gfn mFunction;
};
struct EngineObject {
    gptr<EngineVtableEntry> mpVtable;
};
// HD: GHS encodes this pointer-to-member table in eight-byte entries.
inline EngineEntry* engineEntry(u32 algorithm) {
    return gabi::at<EngineEntry>(0x101D5678 + (algorithm << 3));
}
inline EngineObject* engineObject(u8* object, s16 offset) {
    return gabi::at<EngineObject>(gabi::ea(object) + static_cast<u32>(static_cast<s32>(offset)));
}
struct EventString {
    char text[8];
};
enum : u32 {
    MAP_KEEP_EVENT = 0xFF,
    MAP_CLEAR_EVENT_FLAG = 1,
    MAP_CLEAR_COUNTER = 2,
    MAP_SOUND_NOW = 0x80,
    MAP_SOUND_LATER = 0x40,
    MAP_CAMERA_SOUND = 0x806
};
enum : s32 { PLAY_CAMERA_SLOT = 9 };
} // namespace event_camera_style
using namespace event_camera_style;
u32 cameraStyleGetEvIntData(dCamera_c*, be<s32>*, const char*, s32);
be<u32>* cameraStyleGetEvStringPntData(dCamera_c*, const char*, const char*);
StageEvent* cameraStyleGetStageEventDt(dEvt_control_c*);
StageEvent* cameraStyleNextStageEventDt(dEvt_control_c*, StageEvent*);
s32 cameraStyleGetCameraTypeFromMapToolID(dCamera_c*, u32, s32);
s32 cameraStyleSearchStyle(void*, u32);
void cameraStyleSoundStart(u32);
void cameraStyleRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*);
bool cameraStyleLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
void cameraStyleSubtract(cXyz*, cXyz*, cXyz*);
cSAngle_l* cameraStyleAngleCtor(cSAngle_l*, s32);
