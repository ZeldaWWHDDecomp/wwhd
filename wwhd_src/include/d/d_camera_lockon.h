#pragma once
#include "d/d_camera.h"

/* work area of lockonCamera (mWork, GC Work::Lockon) */
struct LockonWork {
    /* 0x00 */ be<u32> m378; /* 'LOCK' */
    /* 0x04 */ u8 _04[4];
    /* 0x08 */ be<s32> m380; /* charge frames */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388; /* frames the player is hidden */
    /* 0x14 */ be<u8> m38C;
    /* 0x15 */ u8 _15[3];
    /* 0x18 */ cXyz m390; /* centre base */
    /* 0x24 */ be<u8> m39C;
    /* 0x25 */ be<u8> m39D; /* shock given */
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<s32> m3A0;   /* camera side */
    /* 0x2C */ be<s32> m3A4;   /* side to turn to while hidden */
    /* 0x30 */ cSGlobe_l m3A8; /* centre offset */
    /* 0x38 */ be<f32> m3B0;
    /* 0x3C */ be<f32> m3B4;
    /* 0x40 */ be<f32> m3B8; /* centre height cushion */
};
WWHD_SIZE(LockonWork, 0x44);

// HD: the camera setup gains a four-byte field before these GameCube members.
struct LockonSetupHD {
    u8 _00[0x24];
    be<f32> mCusCus;
    u8 _28[4];
    be<f32> m028;
    u8 _30[0x0C];
    be<f32> mBaseCushion, mJumpCushion, mParallelDist;
    u8 _48[8];
    be<f32> mCurveWeight;
    u8 _54[0x58];
    be<f32> mChargeLatitude;
    be<s32> mChargeTimer;
    be<f32> mChargeBRatio;
    be<s32> m0B0;
    be<f32> m0B4;
};
WWHD_OFFSET(LockonSetupHD, mChargeLatitude, 0xAC);
WWHD_OFFSET(LockonSetupHD, m0B4, 0xBC);
WWHD_SIZE(LockonSetupHD, 0xC0);
struct LockonAttentionHD {
    u8 _00[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(LockonAttentionHD, mFlags, 0x20);
struct LockonPlayerVtableHD {
    u8 _00[0xB4];
    be<u32> getThrowBoomerangID;
};
WWHD_OFFSET(LockonPlayerVtableHD, getThrowBoomerangID, 0xB4);

// Calls retain the HD out-of-line ABI.
cSAngle_l* lockon_angleFromDegrees(cSAngle_l*, f32);
void lockon_MoveBgMatrixCrrPos(u32, u32, s32, cXyz*, u32, u32);
bool lockon_LockonTruth(u32);
void lockon_StartShock(u32, s32, s32, cXyz*);
void lockon_globeDefault(cSGlobe_l*);
f32 lockon_LockonReleaseDistanse(u32);
void lockon_globeValShort(cSGlobe_l*, f32, s16, s16);
f32 lockon_xyzHorizontalDistance(cXyz*, cXyz*);
void lockon_globeVal(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
s16 lockon_LockonLongitude(u32, f32);
void lockon_angleMultiplyAssign(cSAngle_l*, f32);
void lockon_shortMinusAngle(cSAngle_l*, s16, cSAngle_l*);
void lockon_angleNegate(cSAngle_l*, cSAngle_l*);
bool lockon_lineBGCheckBack(dCamera_c*, cXyz*, cXyz*, u32);
bool lockon_lineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
f32 lockon_RadiusRatio(u32, f32);
f32 lockon_LockonCenterHeight(u32, f32);
f32 lockon_angleCos(cSAngle_l*);
void lockon_angleVal(cSAngle_l*, cSAngle_l*);
void lockon_globeXyz(cSGlobe_l*, cXyz*);
void lockon_globeFromVector(cSGlobe_l*, cXyz*);
void lockon_angleValDegrees(cSAngle_l*, f32);
void lockon_shortPlusAngle(cSAngle_l*, s16, cSAngle_l*);
f32 lockon_angleDegree(cSAngle_l*);
s16 lockon_LockonLatitude(u32, f32);
bool lockon_CheckLatitudeRange(u32, be<s16>*);
bool lockon_DefaultRadius(u32, be<f32>*);
f32 lockon_LockonFovy(u32, f32);
f32 lockon_rationalBezierRatio(f32, f32);
f32 lockon_customRBRatio(f32, f32);
void lockon_angleSubtract(const void*, cSAngle_l*, const void*);
void lockon_angleMultiply(const void*, cSAngle_l*, f32);
void lockon_angleAdd(const void*, cSAngle_l*, const void*);
void lockon_angleAddAssign(void*, const void*);
void lockon_angleSubtractAssign(void*, const void*);

namespace lockonDetail {
// GameCube player status names, restricted to this camera's checks.
enum PlayerStatus0 {
    daPyStts0_UNK1_e = 0x1,
    daPyStts0_UNK20_e = 0x20,
    daPyStts0_UNK40_e = 0x40,
    daPyStts0_UNK400_e = 0x400,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyStts0_BOOMERANG_WAIT_e = 0x400000,
    daPyStts0_UNK1000000_e = 0x1000000,
};
enum PlayerStatus1 { daPyStts1_UNK20000_e = 0x20000 };
// HD: profile IDs differ from the GameCube profile table.
enum ActorProfile { fpcNm_PLAYER_e = 0xA8, fpcNm_BDK_e = 0xEE };
} // namespace lockonDetail
