#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
namespace camera_follow {
using GuestWord = be<u32>;
using MatrixPointer = gptr<Mtx34>;
using GuestByte = be<u8>;
using GuestFloat = be<f32>;
using GuestShort = be<s16>;
struct Follow {
    /* 0x00 */ be<u32> m378; /* 'FLLW' */
    /* 0x04 */ be<s32> m37C; /* transition frames */
    /* 0x08 */ be<f32> m380; /* remaining weight sum */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388; /* frames since free movement */
    /* 0x14 */ be<s32> m38C; /* turn-behind counter */
    /* 0x18 */ be<s16> m390;
    /* 0x1A */ be<s16> m392; /* charge frames */
    /* 0x1C */ be<f32> m394;
    /* 0x20 */ be<f32> m398; /* min radius */
    /* 0x24 */ be<f32> m39C; /* max radius */
    /* 0x28 */ be<f32> m3A0; /* latitude (deg) */
    /* 0x2C */ be<f32> m3A4; /* last player y */
    /* 0x30 */ be<f32> m3A8;
    /* 0x34 */ be<f32> m3AC; /* centre offset x */
    /* 0x38 */ be<f32> m3B0; /* centre offset z */
    /* 0x3C */ be<s32> m3B4;
    /* 0x40 */ be<f32> m3B8; /* longitude follow ratio */
    /* 0x44 */ be<f32> m3BC;
    /* 0x48 */ cXyz m3C0; /* smoothed centre */
    /* 0x54 */ cXyz m3CC; /* target eye */
    /* 0x60 */ be<u8> m3D8;
    /* 0x61 */ be<u8> m3D9;
    /* 0x62 */ be<u8> m3DA;
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<f32> m3DC;
    /* 0x68 */ be<f32> m3E0; /* latitude follow ratio */
    /* 0x6C */ be<f32> m3E4;
    /* 0x70 */ be<f32> m3E8; /* fovy follow ratio */
    /* 0x74 */ be<f32> m3EC; /* centre follow ratio xz */
    /* 0x78 */ be<f32> m3F0; /* centre follow ratio y */
};
WWHD_SIZE(Follow, 0x7C);
struct SetupView {
    u8 _00[0x80];
    GuestFloat m078, m07C;
    u8 _88[0xAC - 0x88];
    GuestFloat mChargeLatitude;
    be<s32> mChargeTimer;
    GuestFloat mChargeBRatio;
};
WWHD_OFFSET(SetupView, mChargeLatitude, 0xAC);
inline SetupView* setup(dCamera_c* c) {
    return reinterpret_cast<SetupView*>(c->mCamSetup);
}
struct Style {
    u8 _00[0x80];
    be<u16> mFlags;
    u8 _82[2];
};
WWHD_SIZE(Style, 0x84);
inline Style* styleEntry(s32 index) {
    return gabi::at<Style>(CAM_STYLES + index * 0x84);
}
inline cSAngle_l& angle0() {
    return *gabi::at<cSAngle_l>(0x101FF354);
}
inline cSAngle_l& angle90() {
    return *gabi::at<cSAngle_l>(0x101FF358);
}
inline cSAngle_l& angle270() {
    return *gabi::at<cSAngle_l>(0x101FF35C);
}
enum CheckVtable : u32 {
    CAM_LINE = 0x1004A66C,
    CAM_POLY_INFO = 0x1004A67C,
    CAM_POLY_PASS = 0x1004A69C,
    CAM_GROUP_PASS = 0x1004A68C,
    BASE_POLY_PASS = 0x1004A61C,
    BASE_GROUP_PASS = 0x1004A4EC,
    BASE_POLY_INFO = 0x1004A4DC
};
enum CameraParamFlag { dCamPrmFlg_UNK001 = 1, dCamPrmFlg_UNK200 = 0x200 };
enum WorkTag : u32 { FOLLOW_WORK = 0x464C4C57 };
enum PlayerStatus0 : u32 {
    daPyStts0_UNK1_e = 0x00000001,
    daPyStts0_UNK2_e = 0x00000002,
    daPyStts0_UNK4_e = 0x00000004,
    daPyStts0_UNK10_e = 0x00000010,
    daPyStts0_UNK20_e = 0x00000020,
    daPyStts0_UNK40_e = 0x00000040,
    daPyStts0_UNK80_e = 0x00000080,
    daPyStts0_HANG_e = 0x00000100,
    daPyStts0_UNK200_e = 0x00000200,
    daPyStts0_UNK400_e = 0x00000400,
    daPyStts0_UNK800_e = 0x00000800,
    daPyStts0_BOW_AIM_e = 0x00001000,
    daPyStts0_SUBJECT_e = 0x00002000,
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
    daPyStts0_UNK80000000_e = 0x80000000
};
enum PlayerStatus1 : u32 {
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
    daPyStts1_UNK80000_e = 0x00080000
};
} // namespace camera_follow
using namespace camera_follow;
void cameraFollowLineCtor(dBgS_LinChk*);
void cameraFollowLineDtor(dBgS_LinChk*, s32);
void cameraFollowGlobeXyz(cSGlobe_l*, cXyz*);
void cameraFollowGlobeFromVector(cSGlobe_l*, cXyz*);
cSAngle_l* cameraFollowAngleShortCtor(cSAngle_l*, s32);
cSAngle_l* cameraFollowAngleFloatCtor(cSAngle_l*, f32);
void cameraFollowAngleScale(cSAngle_l*, cSAngle_l*, f32);
void cameraFollowAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraFollowAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraFollowGlobeCtor(cSGlobe_l*, f32, s16, s16);
void cameraFollowVectorMultiply(cXyz*, cXyz*, cXyz*);
void cameraFollowShortSubtractAngle(cSAngle_l*, s16, cSAngle_l*);
void cameraFollowAngleVal(cSAngle_l*, cSAngle_l*);
bool cameraFollowLineBlocked(dCamera_c*, cXyz*, cXyz*, u32);
void cameraFollowMoveBgPosition(dBgS*, cBgS_PolyInfo*, s32, cXyz*, u32, u32);
f32 cameraFollowBezierRatio(f32, f32);
f32 cameraFollowAngleDegree(const cSAngle_l*);
f32 cameraFollowAngleSin(cSAngle_l*);
f32 cameraFollowAngleCos(cSAngle_l*);
void cameraFollowAngleScaleAssign(cSAngle_l*, f32);
void cameraFollowAngleValFloat(cSAngle_l*, f32);
void cameraFollowGlobeCtorAngles(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
f32 cameraFollowHorizontalDistance(cXyz*, cXyz*);
f32 cameraFollowLimit(f32, f32, f32);
void cameraFollowAngleAddAssign(cSAngle_l*, cSAngle_l*);
void cameraFollowGlobeCopyCtor(cSGlobe_l*, cSGlobe_l*);
f32 cameraFollowFanBank(u8*);
cM3dGPla* cameraFollowGetTriPla(dBgS*, u32, u32);
