// First-person aiming camera.
#pragma once
#include "d/d_camera.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
/* work area of subjectCamera (mWork, GC Work::Subject) */
struct SubjectWork {
    /* 0x00 */ be<u32> m378; /* 'SUBJ' */
    /* 0x04 */ be<u8> m37C;
    /* 0x05 */ be<u8> m37D;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<s32> m380; /* style */
    /* 0x0C */ be<f32> m384; /* yaw input */
    /* 0x10 */ be<f32> m388; /* pitch input */
    /* 0x14 */ be<f32> m38C; /* zoom */
    /* 0x18 */ be<f32> m390;
    /* 0x1C */ u8 _1C[4];
    /* 0x20 */ be<s32> m398;
    /* 0x24 */ be<f32> m39C; /* zoomed-in fovy */
    /* 0x28 */ be<f32> m3A0; /* fovy at entry */
    /* 0x2C */ u8 _2C[4];
    /* 0x30 */ be<s32> m3A8;
    /* 0x34 */ u8 _34[0xC];
    /* 0x40 */ cSAngle_l m3B8; /* player body angle x */
    /* 0x42 */ cSAngle_l m3BA; /* player angle y */
    /* 0x44 */ be<u8> m3BC;    /* scope view set */
    /* 0x45 */ be<u8> m3BD;    /* low centre */
    /* 0x46 */ u8 _46[2];
    /* 0x48 */ be<s32> m3C0; /* centre mode: 0 player, 1 eye, 2 extended position */
    /* 0x4C */ be<s32> m3C4; /* C-stick down state */
};
WWHD_SIZE(SubjectWork, 0x50);

// HD: swimming setup fields extend the GameCube camera setup.
struct SubjectSetupHD {
    u8 _00[0x34];
    be<f32> m030;
    u8 _38[0x18];
    be<f32> mCurveWeight;
    u8 _54[0x80];
    be<f32> mSwimOffsetX, mSwimOffsetY, mSwimOffsetZ;
    be<f32> mSwimRadius, mSwimLatitude, mSwimFovy, mSwimBank;
};
WWHD_OFFSET(SubjectSetupHD, mSwimOffsetX, 0xD4);
WWHD_OFFSET(SubjectSetupHD, mSwimLatitude, 0xE4);
WWHD_SIZE(SubjectSetupHD, 0xF0);
struct SubjectExtendedAngleHD {
    u8 _00[0xFA];
    cSAngle_l m0FA;
};
WWHD_OFFSET(SubjectExtendedAngleHD, m0FA, 0xFA);
struct SubjectEventManagerHD {
    u8 _00[0x20];
    be<s32> mCameraPlay;
};
WWHD_OFFSET(SubjectEventManagerHD, mCameraPlay, 0x20);
struct SubjectGamepadHD {
    u8 _00[0x18];
    be<u32> mHold;
    u8 _1C[0x11C];
    struct {
        be<f32> x, y;
    } mRightStick;
};
WWHD_OFFSET(SubjectGamepadHD, mRightStick, 0x138);
struct SubjectRotationHD {
    be<f32> m[3][3];
};
WWHD_SIZE(SubjectRotationHD, 0x24);
struct SubjectAimOptionsHD {
    u8 _00[2];
    be<u8> mInvertY;
};
struct SubjectStyleHD {
    u8 _00[0x80];
    be<u16> mFlags;
    u8 _82[2];
};
WWHD_SIZE(SubjectStyleHD, 0x84);
struct SubjectStylesHD {
    SubjectStyleHD mStyle[145];
};
struct SubjectCameraInfoHD {
    u8 _00[0x10];
    be<f32> mCameraZoomForcus;
};
WWHD_OFFSET(SubjectCameraInfoHD, mCameraZoomForcus, 0x10);
namespace subjectDetail {
enum PlayerStatus0 {
    daPyStts0_SUBJECT_e = 0x2000,
    daPyStts0_UNK40000_e = 0x40000,
    daPyStts0_TELESCOPE_LOOK_e = 0x200000,
    daPyStts0_CRAWL_e = 0x8000000,
};
enum PlayerStatus1 {
    daPyStts1_PICTO_BOX_AIM_e = 8,
    // HD: swimming is tested in the second status word.
    daPyStts1_SWIM_HD_e = 0x100000,
};
enum CameraParamFlag {
    dCamPrmFlg_UNK010 = 0x10,
    dCamPrmFlg_UNK020 = 0x20,
    dCamPrmFlg_UNK080 = 0x80,
    dCamPrmFlg_UNK100 = 0x100
};
enum AttentionStatus {
    dCamAttnStts_TELESCOPE_LOOK_e = 8,
    dCamAttnStts_PICTO_BOX_AIM_e = 0x40,
    dCamAttnStts_00000080_e = 0x80,
    dCamAttnStts_00002000_e = 0x2000
};
enum CenterMode { SUBJECT_CENTER_PLAYER, SUBJECT_CENTER_EYE, SUBJECT_CENTER_EXTENDED };
enum StickDownState { STICK_IDLE, STICK_HELD, STICK_TRIGGERED };
// HD: player profile and controller button values.
enum { fpcNm_PLAYER_e = 0xA8, SUBJECT_CAMERA_BUTTON_HD = 0x40, SUBJECT_WORK_TAG = 0x5355424A };
} // namespace subjectDetail
void subject_angleFromDegrees(cSAngle_l*, f32);
void subject_shortPlusAngle(cSAngle_l*, s16, cSAngle_l*);
u32 subject_getAimOptions(u32);
f32 subject_CPadGetStickX(s32);
f32 subject_CPadGetStickY(s32);
void subject_getGyroOrientation(SubjectRotationHD*, u32);
void subject_getMatrixRow(cXyz*, SubjectRotationHD*, s32);
void subject_updateGyro(u32);
cSAngle_l* subject_angleFromShort(cSAngle_l*, s16);
f32 subject_rationalBezierRatio(f32, f32);
f32 subject_atan2f(f32, f32);
s16 subject_cM_rad2s(f32);
void subject_angleSubtract(const void*, cSAngle_l*, const void*);
void subject_angleMultiply(const void*, cSAngle_l*, f32);
void subject_angleAdd(const void*, cSAngle_l*, const void*);
f32 subject_angleDegree(const void*);
void subject_globeFromVector(cSGlobe_l*, cXyz*);
void subject_globeXyz(cSGlobe_l*, cXyz*);
void subject_globeDefault(cSGlobe_l*);
void subject_angleVal(cSAngle_l*, cSAngle_l*);
void subject_globeVal(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
cSAngle_l* subject_angleCopy(cSAngle_l*, cSAngle_l*);
void subject_xyzRotateX(cXyz*, cXyz*, cSAngle_l*);
void subject_xyzRotateY(cXyz*, cXyz*, cSAngle_l*);
void subject_cBgS_LinChkCt(dBgS_LinChk*);
cM3dGPla* subject_GetTriPla(u32, u32, u32);
void subject_cBgS_LinChkDtor(dBgS_LinChk*, s32);
f32 subject_zoomFovy(f32, f32);
void subject_angleValDegrees(cSAngle_l*, f32);
