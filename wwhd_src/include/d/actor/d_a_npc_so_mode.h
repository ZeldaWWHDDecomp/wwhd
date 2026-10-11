/* Fishman mode implementation views.
 * HD retains the circle-path layout; the actor embeds it at mB10 (+0xC88). */
#pragma once
#include "d/actor/d_a_npc_so.h"

struct SoModeHIO {
    u8 prefix[0x22];
    /* 0x22 */ be<u8> debugDraw;
    u8 reserved[9];
    /* 0x2C */ be<u8> m2C;
    be<u8> m2D;
    be<u8> m2E;
    be<u8> m2F;
    be<u8> m30;
    be<u8> m31;
    u8 tuning[0x1E];
    /* 0x50 */ be<f32> m50;
    /* 0x54 */ be<f32> m54;
    u8 jumpTuning[0x10];
    /* 0x68 */ be<s16> m68;
    u8 hitTuning[0x12];
    /* 0x7C */ be<s16> m7C;
};
WWHD_OFFSET(SoModeHIO, debugDraw, 0x22);
WWHD_OFFSET(SoModeHIO, m2C, 0x2C);
WWHD_OFFSET(SoModeHIO, m50, 0x50);
WWHD_OFFSET(SoModeHIO, m54, 0x54);
WWHD_OFFSET(SoModeHIO, m68, 0x68);
WWHD_OFFSET(SoModeHIO, m7C, 0x7C);
WWHD_SIZE(SoModeHIO, 0x7E);

struct SoModeCirclePath {
    /* 0x00 */ cXyz mTranslation;
    /* 0x0C */ cXyz mPos;
    /* 0x18 */ be<f32> mRadius;
    /* 0x1C */ be<f32> mWobbleAmplitude;
    /* 0x20 */ be<s16> mAngle;
    /* 0x22 */ be<s16> mAngleSpeed;
};
WWHD_OFFSET(SoModeCirclePath, mTranslation, 0);
WWHD_OFFSET(SoModeCirclePath, mPos, 0xC);
WWHD_OFFSET(SoModeCirclePath, mRadius, 0x18);
WWHD_OFFSET(SoModeCirclePath, mWobbleAmplitude, 0x1C);
WWHD_OFFSET(SoModeCirclePath, mAngle, 0x20);
WWHD_OFFSET(SoModeCirclePath, mAngleSpeed, 0x22);
WWHD_SIZE(SoModeCirclePath, 0x24);

inline SoModeHIO& soModeHIO() { return gabi::object_at<SoModeHIO>(0x104686CC); }

// Bindings retain the verified HD return widths.
void soMode_seStart(u32, cXyz*, u32, s32);
void soMode_monsSeStart(u32, cXyz*, u32, u32, s32);
BOOL soMode_endCheckOld(u32, const char*);
BOOL soMode_endCheck(u32, s16);
f64 soMode_getWaterY(u32, u32);
void soMode_setCirclePath(u32);
void soMode_subtract(u32, u32, u32);
void soMode_multiply(u32, u32, f32);
void soMode_add(u32, u32, u32);
f64 soMode_squareMagnitude(cXyz*);
f64 soMode_sqrtUnrounded(f64);
s32 soMode_calcTimer(be<s32>*);
void soMode_setAnm(daNpc_So_c*, s8, bool);
void soMode_modeProc(daNpc_So_c*, s32, s32);
void soMode_copyColor(u32, u32, s32);
void soMode_matrixCopy(u32, u32);
void soMode_enterBtp(u8*, u32, s16);
void soMode_registerFigure(s32, daNpc_So_c*, cXyz*, s16, f32, f32, f32);
void soMode_enterMorf(mDoExt_McaMorf*);
void soMode_createWaterPillar(cXyz*, f32, f32, s32);
u32 soMode_cameraBody();
void soMode_cameraType(u32, const char*, s32);
s32 soMode_staffId(u32, const char*, s32, s32);
s32 soMode_actionIndex(u32, s32, u32, s32, s32, s32);
void soMode_cutEnd(u32, s32);
BOOL soMode_isAdvance(u32, s32);
void soMode_resetCamera(u32, cXyz*, cXyz*);
void soMode_startCamera(u32);
void soMode_moveStatus(u32);
BOOL soMode_checkTargetHit(dCcD_Sph*);
u32 soMode_targetHitObject(dCcD_Sph*);
BOOL soMode_mapCheck(u32, s32);
void soMode_searchMinigame(u32, daNpc_So_c*);
void soMode_mapNotify(u32);

struct SoModeWords {
    be<u32> x;
    be<u32> y;
    be<u32> z;
};
WWHD_OFFSET(SoModeWords, x, 0);
WWHD_OFFSET(SoModeWords, y, 4);
WWHD_OFFSET(SoModeWords, z, 8);
WWHD_SIZE(SoModeWords, 0xC);

inline SoModeWords& soModeWords(u32 address) { return gabi::object_at<SoModeWords>(address); }
struct SoModeSafeStringVtable {
    enum : u32 { ADDRESS = 0x10021B78 };
    u8 prefix[0x14];
    gptr<void> assureTermination;
};
WWHD_OFFSET(SoModeSafeStringVtable, assureTermination, 0x14);
WWHD_SIZE(SoModeSafeStringVtable, 0x18);
inline void soModeAssureTermination(SafeString* string) {
    auto& table = gabi::object_at<SoModeSafeStringVtable>(string->__vtbl);
    gabi::call_ptr(table.assureTermination.v.get(), string);
}
inline be<u8>* soModeBytes(u32 address) { return gabi::at<be<u8>>(address); }

struct SoModeModelData {
    u8 prefix[0x38];
    /* 0x38 */ be<u32> mpTexNoAnimator;
};
WWHD_OFFSET(SoModeModelData, mpTexNoAnimator, 0x38);
WWHD_SIZE(SoModeModelData, 0x3C);
inline SoModeModelData& soModeModelData(J3DModelData* data) {
    return *reinterpret_cast<SoModeModelData*>(data);
}
inline dCcD_GObjInf& soModeHitObject(u32 address) { return gabi::object_at<dCcD_GObjInf>(address); }

struct SoModeCameraEntry {
    be<u32> mpCamera;
    u8 reserved[0x30];
};
WWHD_OFFSET(SoModeCameraEntry, mpCamera, 0);
WWHD_SIZE(SoModeCameraEntry, 0x34);
struct SoModePlayCamera {
    u8 prefix[0x5AF8];
    SoModeCameraEntry mCamera[1];
};
WWHD_OFFSET(SoModePlayCamera, mCamera, 0x5AF8);
WWHD_SIZE(SoModePlayCamera, 0x5B2C);
inline SoModePlayCamera& soModePlayCamera(u32 address) {
    return gabi::object_at<SoModePlayCamera>(address);
}
struct SoModePlayerDemo {
    u8 prefix[0x420];
    be<u16> mDemoType;
    u8 reserved[0xE];
    be<u32> mDemoMode;
};
WWHD_OFFSET(SoModePlayerDemo, mDemoType, 0x420);
WWHD_OFFSET(SoModePlayerDemo, mDemoMode, 0x430);
WWHD_SIZE(SoModePlayerDemo, 0x434);
inline SoModePlayerDemo& soModePlayerDemo(u32 address) {
    return gabi::object_at<SoModePlayerDemo>(address);
}
struct SoModeMessage {
    u8 prefix[0x948];
    be<u32> mSelectNumber;
};
WWHD_OFFSET(SoModeMessage, mSelectNumber, 0x948);
WWHD_SIZE(SoModeMessage, 0x94C);
struct SoModeHDInfo {
    u8 prefix[0x218];
    be<u32> mpMap;
};
WWHD_OFFSET(SoModeHDInfo, mpMap, 0x218);
WWHD_SIZE(SoModeHDInfo, 0x21C);
inline SoModeMessage* soModeMessage() {
    return gabi::object_at<gptr<SoModeMessage>>(0x101F4B5C).get();
}
inline SoModeHDInfo* soModeHDInfo() {
    return gabi::object_at<gptr<SoModeHDInfo>>(0x101F8344).get();
}

struct SoModeCutEntry {
    gabi::ptmf<void (daNpc_So_c::*)()> init;
    gabi::ptmf<void (daNpc_So_c::*)()> run;
};
WWHD_OFFSET(SoModeCutEntry, init, 0);
WWHD_OFFSET(SoModeCutEntry, run, 8);
WWHD_SIZE(SoModeCutEntry, 0x10);
struct SoModeCutTable {
    SoModeCutEntry mEntries[21];
};
WWHD_OFFSET(SoModeCutTable, mEntries, 0);
WWHD_SIZE(SoModeCutTable, 0x150);

namespace SoModeConstants {
enum : u32 {
    STAGE_DRC = 3,
    dInvSlot_BOW_e = 0xC,
    JA_SE_CM_SO_JUMP_L = 0x5938,
    JA_SE_CM_SO_LANDING_L = 0x5939,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    JA_SE_CM_SO_DIVE = 0x593B,
    AT_TYPE_NORMAL_ARROW = 0x4000,
    JA_SE_LK_ARROW_HIT = 0x2879,
    JA_SE_CV_SO_DAMAGE = 0x4991,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    JA_SE_LK_LAST_HIT = 0x2828,
    daPyStts0_UNK01000000_e = 0x01000000,
    daPyStts0_SWIM_e = 0x00100000,
    dItemNo_SAIL_e = 0x78,
    dItemNo_SAIL_FAST_e = 0x77,
};
}
