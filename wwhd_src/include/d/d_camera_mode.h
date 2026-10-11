#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
namespace camera_mode {
using GuestWord = be<u32>;
using GuestSignedWord = be<s32>;
using GuestByte = be<u8>;
struct CameraType {
    be<char> name[24];
    be<s16> mStyles[20];
};
struct CameraTypes {
    CameraType mEntries[63];
};
WWHD_SIZE(CameraType, 0x40);
struct CameraName {
    char text[16];
};
struct CameraEntry {
    be<char> m_cam_type[16];
    be<u8> m_arrow_idx, field_0x11, field_0x12, field_0x13;
};
struct ArrowEntry {
    cXyz position;
    csXyz angle;
    be<s16> field_0x12;
};
WWHD_SIZE(CameraEntry, 0x14);
WWHD_SIZE(ArrowEntry, 0x14);
struct StageCamera {
    be<s32> num;
    gptr<CameraEntry> m_entries;
};
struct StageArrow {
    be<s32> num;
    gptr<ArrowEntry> m_entries;
};
struct StageVtable {
    u8 _00[0x24];
    GuestWord getCamera;
    u8 _28[0xC];
    GuestWord getArrow;
};
struct StageData {
    gptr<StageVtable> mpVtable;
};
struct EntryWords {
    GuestWord mWords[5];
};
struct AttentionView {
    u8 _00[0x20];
    GuestWord mFlags;
};
struct EventManagerView {
    u8 _00[0x20];
    be<s32> mCameraPlay;
};
struct PlayerVtable {
    u8 _00[0xB4];
    GuestWord getThrowBoomerangID;
};
struct PadOption {
    u8 _00[0x18];
    GuestWord mFlags;
};
using PadOptionPointer = gptr<PadOption>;
struct MoveBackgroundTypes {
    gptr<const char> mNames[64];
};
// HD: dCamSetup_c has added fields before the GameCube m098/manual-start/Cstick fields.
struct CameraSetupView {
    u8 _00[0xA0];
    be<f32> m098, mManualStartCThreshold;
    u8 _A8[0xF4 - 0xA8];
    struct {
        be<f32> m00;
    } mCstick;
};
WWHD_OFFSET(CameraSetupView, m098, 0xA0);
WWHD_OFFSET(CameraSetupView, mManualStartCThreshold, 0xA4);
WWHD_OFFSET(CameraSetupView, mCstick, 0xF4);
enum dCamAlgorythmn_e {
    dCamAlg_LET_CAMERA_e = 0,
    dCamAlg_FOLLOW_CAMERA_e = 1,
    dCamAlg_LOCKON_CAMERA_e = 2,
    dCamAlg_TALKTO_CAMERA_e = 3,
    dCamAlg_SUBJECT_CAMERA_e = 4,
    dCamAlg_FIXED_POSITION_CAMERA_e = 5,
    dCamAlg_FIXED_FRAME_CAMERA_e = 6,
    dCamAlg_TOWER_CAMERA_e = 7,
    dCamAlg_RIDE_CAMERA_e = 8,
    dCamAlg_HUNG_CAMERA_e = 9,
    dCamAlg_MANUAL_CAMERA_e = 10,
    dCamAlg_EVENT_CAMERA_e = 11,
    dCamAlg_CRAWL_CAMERA_e = 12,
    dCamAlg_HOOKSHOT_CAMERA_e = 13
};
enum ProcessName : s16 { fpcNm_PLAYER_e = 0xA8, fpcNm_NPC_MD_e = 0x16F };
enum daPy__PlayerStatus0 : u32 {
    daPyStts0_UNK1_e = 0x00000001,
    daPyStts0_UNK2_e = 0x00000002,
    daPyStts0_UNK4_e = 0x00000004,
    daPyStts0_UNK10_e = 0x00000010,
    daPyStts0_UNK20_e = 0x00000020,
    daPyStts0_UNK40_e = 0x00000040,
    daPyStts0_UNK80_e = 0x00000080, // Maybe inside a Baba Bud before being spat out?
    daPyStts0_HANG_e = 0x00000100,
    daPyStts0_UNK200_e = 0x00000200,
    daPyStts0_UNK400_e = 0x00000400,
    daPyStts0_UNK800_e = 0x00000800,
    daPyStts0_BOW_AIM_e = 0x00001000,
    daPyStts0_SUBJECT_e = 0x00002000, // First person
    daPyStts0_HOOKSHOT_AIM_e = 0x00004000,
    daPyStts0_SWORD_SWING_e = 0x00008000,
    daPyStts0_SHIP_RIDE_e = 0x00010000,
    daPyStts0_ROPE_AIM_e = 0x00020000,
    daPyStts0_UNK40000_e = 0x00040000,
    daPyStts0_BOOMERANG_AIM_e = 0x00080000,
    daPyStts0_SWIM_e = 0x00100000,
    daPyStts0_TELESCOPE_LOOK_e = 0x00200000,
    daPyStts0_BOOMERANG_WAIT_e = 0x00400000,
    daPyStts0_UNK800000_e = 0x00800000,
    daPyStts0_UNK1000000_e = 0x01000000,
    daPyStts0_UNK2000000_e = 0x02000000,
    daPyStts0_UNK4000000_e = 0x04000000,
    daPyStts0_CRAWL_e = 0x08000000,
    daPyStts0_UNK10000000_e = 0x10000000,
    daPyStts0_UNK20000000_e = 0x20000000,
    daPyStts0_SPIN_ATTACK_e = 0x40000000,
    daPyStts0_UNK80000000_e = 0x80000000,

    // This is some combination of flags which is seemingly related to "judgement", used in
    // dAttention_c
    daPyStts0_UNK37a02371_e =
        daPyStts0_UNK1_e | daPyStts0_UNK10_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e |
        daPyStts0_HANG_e | daPyStts0_UNK200_e | daPyStts0_SUBJECT_e | daPyStts0_TELESCOPE_LOOK_e |
        daPyStts0_UNK800000_e | daPyStts0_UNK1000000_e | daPyStts0_UNK2000000_e |
        daPyStts0_UNK4000000_e | daPyStts0_UNK10000000_e | daPyStts0_UNK20000000_e
};

enum daPy__PlayerStatus1 : u32 {
    daPyStts1_WIND_WAKER_CONDUCT_e = 0x00000001,
    daPyStts1_UNK2_e = 0x00000002,
    daPyStts1_UNK4_e = 0x00000004,
    daPyStts1_PICTO_BOX_AIM_e = 0x00000008,
    daPyStts1_UNK10_e = 0x00000010,
    daPyStts1_DEKU_LEAF_FLY_e = 0x00000020,
    daPyStts1_DEKU_LEAF_FAN_e = 0x00000040,
    daPyStts1_UNK80_e = 0x00000080,
    daPyStts1_UNK100_e = 0x00000100,
    daPyStts1_UNK200_e = 0x00000200,
    daPyStts1_SAIL_e = 0x00000400,
    daPyStts1_UNK800_e = 0x00000800,
    daPyStts1_UNK1000_e = 0x00001000,
    daPyStts1_UNK2000_e = 0x00002000,
    daPyStts1_UNK4000_e = 0x00004000,
    daPyStts1_UNK8000_e = 0x00008000,
    daPyStts1_UNK10000_e = 0x00010000,
    daPyStts1_UNK20000_e = 0x00020000,
    daPyStts1_UNK40000_e = 0x00040000,
    daPyStts1_UNK80000_e = 0x00080000,
};

enum dCamera__AttentionStatus {
    dCamAttnStts_00000001_e = 0x00000001, // just locked on?
    dCamAttnStts_SUBJECT_e = 0x00000002,  // First person
    dCamAttnStts_00000004_e = 0x00000004,
    dCamAttnStts_TELESCOPE_LOOK_e = 0x00000008,
    dCamAttnStts_00000010_e = 0x00000010,
    dCamAttnStts_00000020_e = 0x00000020,
    dCamAttnStts_PICTO_BOX_AIM_e = 0x00000040,
    dCamAttnStts_00000080_e = 0x00000080,
    dCamAttnStts_00000100_e = 0x00000100,
    // dCamAttnStts_00000200_e = 0x00000200,
    dCamAttnStts_00000400_e =
        0x00000400, // manual camera control, as opposed to free camera control?
    // dCamAttnStts_00000800_e = 0x00000800,
    dCamAttnStts_00001000_e = 0x00001000, // just pressed c-stick up to enter first person?
    dCamAttnStts_00002000_e = 0x00002000,
};

enum ModeExit { CLEAR_LOCK_ID, NORMAL_TAIL, STYLE_ONLY };
// These typed views preserve the direct HD reads; no shared accessor is redefined.
inline bool cameraModeIsPlaying() {
    return reinterpret_cast<EventManagerView*>(dComIfGp_getPEvtManager())->mCameraPlay != 0;
}
inline dComIfG_inf_c* cameraModeGameInfo() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
}
inline CameraSetupView* cameraModeSetup(dCamera_c* c) {
    return reinterpret_cast<CameraSetupView*>(c->mCamSetup);
}
} // namespace camera_mode
using namespace camera_mode;
bool cameraModeLockonTruth(dAttention_c*);
fopAc_ac_c* cameraModeActionTarget(dAttention_c*, s32);
StageCamera* cameraModeRoomCamera(s32);
StageArrow* cameraModeRoomArrow(s32);
cSAngle_l* cameraModeAngleCtor(cSAngle_l*, s32);
