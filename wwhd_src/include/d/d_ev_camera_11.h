// Event-camera watch-target work.
#pragma once
#include "d/d_camera.h"

// HD: the shared work union starts four bytes later than on GameCube.
struct WatchActorWork {
    cXyz mCtrGap;
    cXyz mCenter;
    be<f32> mCushion;
    be<u32> mNearTimer;
    be<f32> mNearDist;
    be<u32> mFarTimer;
    be<f32> mFarDist;
    be<f32> mZoomDist;
    be<f32> mZoomVAngle;
    gptr<fopAc_ac_c> mpTarget;
    be<u32> mTargetId;
    cSGlobe_l mEyeDirection;
    u8 _44[8];
    cSGlobe_l mTargetDirection;
    be<u32> mMode;
    be<s32> mBlure;
    be<f32> mFrontAngle;
};
WWHD_OFFSET(WatchActorWork, mCenter, 0xC);
WWHD_OFFSET(WatchActorWork, mpTarget, 0x34);
WWHD_OFFSET(WatchActorWork, mEyeDirection, 0x3C);
WWHD_OFFSET(WatchActorWork, mTargetDirection, 0x4C);
WWHD_OFFSET(WatchActorWork, mMode, 0x54);
WWHD_SIZE(WatchActorWork, 0x60);

// Unit-local extension: the shared camera class/layout is unchanged.
struct WatchActorCamera_c : dCamera_c {
    u32 watchActorEvCamera();
};
WWHD_SIZE(WatchActorCamera_c, 0x8E0);

// HD: explicit GHS out-of-line operations, including hidden result pointers.
u32 gi();
u32 getEvIntData(u32 c, u32 out, u32 name, s32 def);
u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def);
u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def);
u32 getEvActor2(u32 c, u32 name, u32 def);
void relationalPos(u32 c, u32 out, u32 actor, u32 ofs);
void cXyz_mi(u32 a, u32 out, u32 b);
void cXyz_pl(u32 a, u32 out, u32 b);
void cXyz_ml(u32 a, u32 out, f32 s);
void cSGlobe_Val(u32 g, u32 v);
void cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u);
void cSGlobe_Xyz(u32 g, u32 out);
void cSGlobe_ct(u32 g, u32 v);
void cSAngle_ct0(u32 a);
void cSAngle_mi(u32 a, u32 out, u32 b);
void cSAngle_pl(u32 a, u32 out, u32 b);
void cSAngle_ml(u32 a, u32 out, f32 f);
u32 cSAngleF(u32 tmp, f32 deg);
u32 cSAngleS(u32 tmp, s16 v);
void positionOf(u32 c, u32 out, u32 actor);
void directionOf(u32 c, u32 out, u32 actor);
void attentionPos(u32 c, u32 out, u32 actor);
bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags);
bool cameraCollisionCheck(u32, u32, u32, f32, u32, u32);
f32 cSAngle_Degree(u32);
void cSAngle_Val(u32, f32);
void PSVECAdd(u32, u32, u32);
u32 pointInSight(u32, u32);
u32 judgeActorById(u32, u32);
void cSGlobe_copy(u32, u32);
void cSAngle_AddShort(u32, u32, s32);
