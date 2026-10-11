// Molgera demo-camera views.
#pragma once
#include "d/actor/d_a_bwd.h"
#include "d/actor/d_a_player.h"
#include "d/d_save.h"
#include "gc_api/f_op/f_op_actor.h"

namespace bwd_camera {
// HD: the camera process embeds its controller at 0x248. Only the fields used
// by this sequence are exposed; the controller remains opaque here.
struct CameraController {};
struct CameraLookatView {
    /* 0x00 */ cXyz mEye;
    /* 0x0C */ cXyz mCenter;
};
WWHD_SIZE(CameraLookatView, 0x18);
struct CameraView {
    u8 prefix[0xDC];
    /* 0xDC */ CameraLookatView mLookat;
    u8 remaining[0x248 - 0xF4];
};
WWHD_OFFSET(CameraView, mLookat, 0xDC);
WWHD_SIZE(CameraView, 0x248);
struct camera_process_class {
    /* 0x000 */ CameraView view;
    /* 0x248 */ CameraController mCamera;
};
WWHD_OFFSET(camera_process_class, view, 0);
WWHD_OFFSET(camera_process_class, mCamera, 0x248);
struct CameraSlot {
    gptr<camera_process_class> camera;
    u8 remaining[0x30];
};
WWHD_SIZE(CameraSlot, 0x34);
struct PlayCameraView {
    u8 prefix[0x5AF8];
    CameraSlot cameras[1];
    u8 gap[4];
    /* 0x5B30 */ be<s8> playerCameraID;
};
WWHD_OFFSET(PlayCameraView, cameras, 0x5AF8);
WWHD_OFFSET(PlayCameraView, playerCameraID, 0x5B30);
inline camera_process_class* getCamera(s32 id) {
    // Preserve the original unsigned address arithmetic, including invalid IDs.
    auto* slots = &gabi::at<PlayCameraView>(dComIfGp_ea())->cameras[0];
    return gabi::at<CameraSlot>(gabi::ea(slots) + u32(id) * sizeof(CameraSlot))->camera.get();
}
inline s32 getPlayerCameraID() { return gabi::at<PlayCameraView>(dComIfGp_ea())->playerCameraID; }
inline cXyz* getSetPosition() { return gabi::at<cXyz>(0x10462AE8); }
} // namespace bwd_camera

// HD: fixed-signature report bindings retain the original integer/float registers.
void bwdCameraReportFloat(s32, s32, const char*, f32);
void bwdCameraReportInt(s32, s32, const char*, s32);
void bwdCameraStop(bwd_camera::CameraController*);
void bwdCameraStart(bwd_camera::CameraController*);
void bwdCameraTrim(bwd_camera::CameraController*, s32);
void bwdCameraReset(bwd_camera::CameraController*, cXyz*, cXyz*);
void bwdCameraSet(bwd_camera::CameraController*, cXyz*, cXyz*, s16, f32);
void bwdCameraOrderEvent(fopAc_ac_c*, s32, s32, s32);
void bwdCameraVectorAdd(cXyz*, cXyz*, cXyz*);
void bwdCameraBgmStart(u32);
void bwdCameraStartQuake(void*, s32, s32, cXyz*);
void bwdCameraStopQuake(void*, s32);
u32 bwdCameraCreateItem(cXyz*, s32, s32, csXyz*, cXyz*, s32);
void bwdCameraCreateWarp(cXyz*, csXyz*, s32, s32);
void bwdCameraOnDungeonItem(void*, s32);
namespace bwd_camera {
struct SaveInfoView {
    u8 prefix[0x798];
    u8 memory;
};
WWHD_OFFSET(SaveInfoView, memory, 0x798);
inline void* getStageMemory() { return &gabi::at<gptr<SaveInfoView>>(0x101F84DC)->get()->memory; }
enum EventStaff { dEvtFlag_STAFF_ALL_e = 2 };
enum DemoPhase {
    DEMO_IDLE = 0,
    DEMO_START = 1,
    DEMO_TRACK = 2,
    DEMO_TRACK_PLAYER = 3,
    DEMO_INTRO_START = 50,
    DEMO_INTRO_SHAKE = 51,
    DEMO_INTRO_SAND = 52,
    DEMO_INTRO_SURPRISE = 53,
    DEMO_INTRO_REVEAL = 60,
    DEMO_DEATH_START = 100,
    DEMO_DEATH_TRACK = 101,
    DEMO_DEATH_SEGMENT = 102,
    DEMO_HEART = 104,
    DEMO_HEART_PLAYER = 105,
    DEMO_WARP = 106,
    DEMO_END = 150,
};
enum SoundID : u32 {
    JA_SE_OBJ_BWD_SANDFALL_S = 0x69E1,
    JA_SE_ATM_JINARI = 0x105E,
    JA_SE_ATM_BWD_SAND_FLOW = 0x1077,
    JA_SE_ATM_BWD_SAND_EBB = 0x1076,
    JA_BGM_PAST_RANE = 0x8000004C,
    JA_BGM_RANE_BATTLE = 0x80000029,
};
} // namespace bwd_camera
