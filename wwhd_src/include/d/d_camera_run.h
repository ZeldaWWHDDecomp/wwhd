#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
#include <bit>
namespace camera_run {
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestByte = be<u8>;
struct VectorWords {
    GuestWord x, y, z;
};
struct RunName {
    char text[32];
};
struct EventString {
    GuestWord mPrefix;
    u8 _04[8];
};
WWHD_SIZE(EventString, 12);
struct EventManagerView {
    u8 _00[0x20];
    be<s32> mCameraPlay;
};
struct AttentionView {
    u8 _00[0x20];
    GuestWord mFlags;
};
struct CameraSetupView {
    u8 _00[0x4C];
    GuestFloat mPlayerHideDist;
    u8 _50[0x90 - 0x50];
    GuestFloat mDMCValue, mDMCAngle;
    u8 _98[0xC0 - 0x98];
    GuestFloat m0B8;
    u8 _C4[0xF0 - 0xC4];
    u8 mCstick[0x18];
    GuestFloat mFloorMargin;
    u8 _10C[0x130 - 0x10C];
    GuestFloat mFwdCushion;
};
WWHD_OFFSET(CameraSetupView, mPlayerHideDist, 0x4C);
WWHD_OFFSET(CameraSetupView, mDMCValue, 0x90);
WWHD_OFFSET(CameraSetupView, mCstick, 0xF0);
WWHD_OFFSET(CameraSetupView, mFloorMargin, 0x108);
WWHD_OFFSET(CameraSetupView, mFwdCushion, 0x130);
struct StyleView {
    u8 _00[4];
    GuestWord mAlgorithm;
};
struct ParamView {
    u8 _00[4];
    gptr<StyleView> mpStyle;
};
struct EnvLightView {
    u8 _00[0x104C];
    GuestFloat mHDZoom;
    u8 _1050[0x10A4 - 0x1050];
    GuestByte mHDZoomState;
};
WWHD_OFFSET(EnvLightView, mHDZoom, 0x104C);
WWHD_OFFSET(EnvLightView, mHDZoomState, 0x10A4);
struct DemoToken {
    u8 _00;
};
using DemoPointer = gptr<DemoToken>;
struct PadToken {
    u8 _00;
};
using PadPointer = gptr<PadToken>;
struct EngineEntry {
    be<s16> mThisDelta, mVtableIndex;
    union {
        GuestWord mFunction;
        struct {
            be<s16> _04, mVtableOffset;
        };
    };
};
WWHD_SIZE(EngineEntry, 8);
struct EngineVtableEntry {
    GuestWord _00;
    gfn mFunction;
};
struct EngineObject {
    gptr<EngineVtableEntry> mpVtable;
};
struct EventActions {
    gptr<char> mNames[28];
};
struct BackgroundMasks {
    GuestByte mValues[5];
};
// HD: member-function tables use eight-byte GHS entries. They are shared references, never
// definitions.
inline EngineEntry* runEngineEntry(u32 index) {
    return gabi::at<EngineEntry>(0x101D5678 + index * 8);
}
inline EngineEntry* eventEngineEntry(u32 index) {
    return gabi::at<EngineEntry>(0x1004A8EC + index * 8);
}
inline u8* adjustThis(dCamera_c* c, s16 delta) {
    return gabi::at<u8>(c->ea() + (s32)delta);
}
inline EngineObject* engineObject(u8* self, s16 offset) {
    return gabi::at<EngineObject>(gabi::ea(self) + (s32)offset);
}
inline EnvLightView* cameraRunEnv() {
    return reinterpret_cast<EnvLightView*>(dKy_getEnvlight());
}
inline CameraSetupView* cameraRunSetup(dCamera_c* c) {
    return reinterpret_cast<CameraSetupView*>(c->mCamSetup);
}
inline bool cameraRunIsPlaying() {
    return reinterpret_cast<EventManagerView*>(dComIfGp_getPEvtManager())->mCameraPlay != 0;
}
inline cSAngle_l& zeroAngle() {
    return *gabi::at<cSAngle_l>(0x101FF354);
}
enum dCamAlgorythmn_e { dCamAlg_SUBJECT_CAMERA_e = 4, dCamAlg_EVENT_CAMERA_e = 11 };
enum daPy__PlayerStatus0 : u32 { daPyStts0_UNK4000000_e = 0x04000000 };
enum daPy__PlayerStatus1 : u32 { daPyStts1_UNK40000_e = 0x00040000 };
enum dCamera__AttentionStatus {
    dCamAttnStts_SUBJECT_e = 2,
    dCamAttnStts_00000004_e = 4,
    dCamAttnStts_00000010_e = 0x10,
    dCamAttnStts_00000020_e = 0x20,
    dCamAttnStts_00000080_e = 0x80,
    dCamAttnStts_00000400_e = 0x400,
    dCamAttnStts_00001000_e = 0x1000,
    dCamAttnStts_00002000_e = 0x2000
};
enum EventAction { EVENT_PAUSE = 0, EVENT_WAIT = 1, EVENT_ACTION_COUNT = 28 };
enum TrimPrefix : u32 {
    TRIM_STANDARD = 0x5354414E,
    TRIM_VISTA = 0x56495354,
    TRIM_DEMO = 0x44454D4F,
    TRIM_NONE = 0x4E4F4E45,
    TRIM_KEEP = 0x4B454550
};
enum CheckVtable : u32 {
    LINE_CHECK = 0x1004A5EC,
    LINE_POLY_INFO = 0x1004A5FC,
    LINE_POLY_PASS = 0x1004A61C,
    LINE_GROUP_PASS = 0x1004A60C,
    BASE_GROUP_PASS = 0x1004A4EC,
    BASE_POLY_INFO = 0x1004A4DC
};
} // namespace camera_run
using namespace camera_run;

bool cameraRunFocusOff(dCamForcusLine*);
f32 cameraRunShipOffsetY(be<s16>*, be<s16>*, f32);
void cameraRunCstickShift(u8*, s32);
bool cameraRunLockonTruth(dAttention_c*);
void cameraRunParamChange(u8*, s32);
void cameraRunSubjectStart(PadToken*);
void cameraRunAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraRunAngleScale(cSAngle_l*, cSAngle_l*, f32);
void cameraRunAngleAddAssign(cSAngle_l*, cSAngle_l*);
f32 cameraRunStickValue(s32);
f32 cameraRunSubstickValue(s32);
u32 cameraRunTrigger(s32);
u32 cameraRunActiveDemoCamera(DemoToken*);
u32 cameraRunDemoCamera(dCamera_c*, s32);
void cameraRunAngleSubtractAssign(cSAngle_l*, cSAngle_l*);
f32 cameraRunShake(dCamera_c*);
f32 cameraRunAddCalc(GuestFloat*, f32, f32, f32, f32);
bool cameraRunBumpCheck(dCamera_c*, u32);
cSAngle_l* cameraRunAngleShortCtor(cSAngle_l*, s32);
cSAngle_l* cameraRunAngleFloatCtor(cSAngle_l*, f32);
void cameraRunSoundStart(s32);
bool cameraRunGetString(dCamera_c*, EventString*, const char*, const char*);
void cameraRunGetInt(dCamera_c*, be<s32>*, const char*, s32);
void cameraRunLineCtor(dBgS_LinChk*);
void cameraRunLineDtor(dBgS_LinChk*, s32);
