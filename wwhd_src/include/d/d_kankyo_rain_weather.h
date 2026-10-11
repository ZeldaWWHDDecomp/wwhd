#pragma once
/* d_kankyo_rain movement group: HD packets retain the GameCube member names.
 * HD: J3D packet prefixes grow by 0x88; camera and environment views are
 * sparse. Integer vector channels retain original word-copy semantics,
 * including SNaNs. */
#include "bindings.h"
#include "d/d_bg_s.h"
#include "gc_api/JSystem/JPAEmitter.h"
namespace weather_move {
struct WeatherVectorHD {
  union {
    struct {
      be<f32> x, y, z;
    };
    be<u32> words[3];
  };
};
struct WeatherDoublePosHD {
  be<f64> x, y, z;
};
WWHD_SIZE(WeatherVectorHD, 12);
WWHD_SIZE(WeatherDoublePosHD, 24);
struct WeatherCameraHD {
  u8 padEye[0xDC];
  WeatherVectorHD mEye, mCenter;
  u8 padFov[0x844 - 0xF4];
  be<f32> mFovy;
};
WWHD_OFFSET(WeatherCameraHD, mEye, 0xDC);
WWHD_OFFSET(WeatherCameraHD, mCenter, 0xE8);
WWHD_OFFSET(WeatherCameraHD, mFovy, 0x844);
WWHD_SIZE(WeatherCameraHD, 0x848);
struct SnowEffHD {
  be<u8> mStatus;
  u8 padPosition[3];
  WeatherVectorHD mPos, mBasePos;
  be<f32> mPosWaveX, mPosWaveZ, mGravity, mWindSpeed, mScale;
  u8 padTimer[4];
  be<s16> mTimer;
  u8 padEnd[2];
};
WWHD_OFFSET(SnowEffHD, mPos, 4);
WWHD_OFFSET(SnowEffHD, mBasePos, 0x10);
WWHD_OFFSET(SnowEffHD, mPosWaveX, 0x1C);
WWHD_OFFSET(SnowEffHD, mPosWaveZ, 0x20);
WWHD_OFFSET(SnowEffHD, mGravity, 0x24);
WWHD_OFFSET(SnowEffHD, mWindSpeed, 0x28);
WWHD_OFFSET(SnowEffHD, mScale, 0x2C);
WWHD_OFFSET(SnowEffHD, mTimer, 0x34);
WWHD_SIZE(SnowEffHD, 0x38);
struct SnowPacketHD {
  u8 padEffects[0x9C];
  SnowEffHD mEff[250];
  WeatherVectorHD mOldEyePos;
  be<f32> mWindStrength, mWindTurn;
  be<s16> mEffCount;
  u8 padEnd[2];
};
WWHD_OFFSET(SnowPacketHD, mEff, 0x9C);
WWHD_OFFSET(SnowPacketHD, mOldEyePos, 0x374C);
WWHD_OFFSET(SnowPacketHD, mWindStrength, 0x3758);
WWHD_OFFSET(SnowPacketHD, mWindTurn, 0x375C);
WWHD_OFFSET(SnowPacketHD, mEffCount, 0x3760);
WWHD_SIZE(SnowPacketHD, 0x3764);
struct SunPacketHD {
  u8 pad0[0x98];
  /* 0x98 */ WeatherVectorHD mPos[2];
  /* 0xb0 */ be<u32> mVizChkData[5];
  /* 0xc4 */ be<u8> mOcclusionDelayHD;
  /* 0xc5 */ be<u8> mHasVisiblePointHD;
  u8 pad198[0x2];
  /* 0xc8 */ be<f32> mVisibility;
  /* 0xcc */ be<f32> mSunAlpha;
  /* 0xd0 */ be<f32> mMoonAlpha;
  u8 pad212[0x14];
  /* 0xe8 */ be<u32> mFullyVisibleDelayHD;
};
WWHD_OFFSET(SunPacketHD, mPos, 0x98);
WWHD_OFFSET(SunPacketHD, mVizChkData, 0xb0);
WWHD_OFFSET(SunPacketHD, mOcclusionDelayHD, 0xc4);
WWHD_OFFSET(SunPacketHD, mHasVisiblePointHD, 0xc5);
WWHD_OFFSET(SunPacketHD, mVisibility, 0xc8);
WWHD_OFFSET(SunPacketHD, mSunAlpha, 0xcc);
WWHD_OFFSET(SunPacketHD, mMoonAlpha, 0xd0);
WWHD_OFFSET(SunPacketHD, mFullyVisibleDelayHD, 0xe8);
WWHD_SIZE(SunPacketHD, 0xec);
struct SunLenzPacketHD {
  u8 pad0[0x110];
  /* 0x110 */ be<f32> mPreviousDistanceHD;
  /* 0x114 */ be<f32> mPreviousDepthHD;
  /* 0x118 */ be<f32> mDistanceHD;
  /* 0x11c */ be<f32> mDepthHD;
  u8 pad288[0x4];
  /* 0x124 */ be<f32> mDistFalloff;
  u8 pad296[0x2];
  /* 0x12a */ be<u8> mbDrawLenzInSky;
  u8 padEnd[0x1];
};
WWHD_OFFSET(SunLenzPacketHD, mPreviousDistanceHD, 0x110);
WWHD_OFFSET(SunLenzPacketHD, mPreviousDepthHD, 0x114);
WWHD_OFFSET(SunLenzPacketHD, mDistanceHD, 0x118);
WWHD_OFFSET(SunLenzPacketHD, mDepthHD, 0x11c);
WWHD_OFFSET(SunLenzPacketHD, mDistFalloff, 0x124);
WWHD_OFFSET(SunLenzPacketHD, mbDrawLenzInSky, 0x12a);
WWHD_SIZE(SunLenzPacketHD, 0x12c);
struct SafeStringVtableHD {
  u8 padTerminate[0x14];
  gfn assureTermination;
};
WWHD_SIZE(SafeStringVtableHD, 0x18);
struct WeatherSafeStringHD {
  gptr<be<u8>> mStringTop;
  gptr<SafeStringVtableHD> __vtbl;
  void assureTermination() {
    gabi::call_ptr<void>(__vtbl->assureTermination.get(), this);
  }
};
WWHD_OFFSET(WeatherSafeStringHD, __vtbl, 4);
WWHD_SIZE(WeatherSafeStringHD, 8);
struct WeatherProjectParamsHD {
  u8 bytes[40];
};
WWHD_SIZE(WeatherProjectParamsHD, 40);
struct StageInfoHD {
  u8 padType[12];
  be<u32> mTypeFlags;
};
WWHD_SIZE(StageInfoHD, 16);
struct StageVtableHD {
  u8 padInfo[0x15C];
  gfn getStagInfo;
};
WWHD_SIZE(StageVtableHD, 0x160);
struct WeatherStageDtHD {
  gptr<StageVtableHD> __vtbl;
  StageInfoHD *getStagInfo() {
    return gabi::call_ptr<StageInfoHD *>(__vtbl->getStagInfo.get(), this);
  }
};
WWHD_SIZE(WeatherStageDtHD, 4);
struct VrkumoEffHD {
  /* 0x0 */ be<u8> mStatus;
  u8 pad1[0x3];
  /* 0x4 */ WeatherVectorHD mPosition;
  u8 pad16[0xc];
  /* 0x1c */ be<f32> mHeight;
  /* 0x20 */ be<f32> mAlpha;
  /* 0x24 */ be<f32> mDistFalloff;
  /* 0x28 */ be<f32> mSpeed;
};
WWHD_OFFSET(VrkumoEffHD, mStatus, 0x0);
WWHD_OFFSET(VrkumoEffHD, mPosition, 0x4);
WWHD_OFFSET(VrkumoEffHD, mHeight, 0x1c);
WWHD_OFFSET(VrkumoEffHD, mAlpha, 0x20);
WWHD_OFFSET(VrkumoEffHD, mDistFalloff, 0x24);
WWHD_OFFSET(VrkumoEffHD, mSpeed, 0x28);
WWHD_SIZE(VrkumoEffHD, 0x2c);
struct VrkumoPacketHD {
  u8 pad0[0xa4];
  /* 0xa4 */ VrkumoEffHD mInst[100];
  /* 0x11d4 */ be<f32> mCloudAngleHD;
};
WWHD_OFFSET(VrkumoPacketHD, mInst, 0xa4);
WWHD_OFFSET(VrkumoPacketHD, mCloudAngleHD, 0x11d4);
WWHD_SIZE(VrkumoPacketHD, 0x11d8);
struct WeatherFileListHD {
  u8 padSeaLevel[4];
  be<f32> mSeaLevel;
};
WWHD_SIZE(WeatherFileListHD, 8);
struct WeatherRoomVtableHD {
  u8 padFileList[0x1DC];
  gfn getFileListInfo;
};
WWHD_SIZE(WeatherRoomVtableHD, 0x1E0);
struct WeatherRoomDtHD {
  gptr<WeatherRoomVtableHD> __vtbl;
  WeatherFileListHD *getFileListInfo() {
    return gabi::call_ptr<WeatherFileListHD *>(__vtbl->getFileListInfo.get(),
                                               this);
  }
};
WWHD_SIZE(WeatherRoomDtHD, 4);
struct WeatherRoomControlHD {
  u8 opaque[4];
};
WWHD_SIZE(WeatherRoomControlHD, 4);
struct WeatherSavedWindHD {
  u8 padAngles[0x4A];
  be<s16> mWindX, mWindY;
};
WWHD_SIZE(WeatherSavedWindHD, 0x4E);
struct CloudFrameHD {
  u8 bytes[0xE4];
};
WWHD_SIZE(CloudFrameHD, 0xE4);
struct CloudWorkHD {
  u8 pad0[0x8];
  /* 0x8 */ be<f32> mSmallStep;
  /* 0xc */ be<f32> mEpsilon;
  /* 0x10 */ be<f32> mMaximumAlpha;
  /* 0x14 */ be<f32> mHalf;
  /* 0x18 */ be<f32> mHeightStep;
  /* 0x1c */ be<f32> mVertical;
  /* 0x20 */ be<f32> mAlphaStep;
  u8 pad36[0xc];
  /* 0x30 */ WeatherSafeStringHD mParticleDragonName;
  /* 0x38 */ WeatherSafeStringHD mParticleStageName;
  /* 0x40 */ WeatherVectorHD mRadialPosition;
  /* 0x4c */ WeatherVectorHD mWindVectorPower;
  /* 0x58 */ WeatherVectorHD mMovementPosition;
  /* 0x64 */ WeatherSafeStringHD mSeaStageName;
  /* 0x6c */ WeatherSafeStringHD mDragonName;
  /* 0x74 */ WeatherSafeStringHD mNameName;
  /* 0x7c */ WeatherSafeStringHD mLinkRoomName;
  /* 0x84 */ WeatherSafeStringHD mOrichName;
  /* 0x8c */ WeatherSafeStringHD mOjhousName;
  /* 0x94 */ WeatherSafeStringHD mOmasaoName;
  /* 0x9c */ WeatherSafeStringHD mOnobutaName;
  /* 0xa4 */ WeatherSafeStringHD mDragonStage;
  /* 0xac */ WeatherSafeStringHD mNameStage;
  /* 0xb4 */ WeatherSafeStringHD mLinkRoomStage;
  /* 0xbc */ WeatherSafeStringHD mOrichStage;
  /* 0xc4 */ WeatherSafeStringHD mOjhousStage;
  /* 0xcc */ WeatherSafeStringHD mOmasaoStage;
  /* 0xd4 */ WeatherSafeStringHD mOnobutaStage;
  /* 0xdc */ WeatherSafeStringHD mSirenStage;
  /* 0xe4 */ WeatherSafeStringHD mAdmumiStage;
};
WWHD_OFFSET(CloudWorkHD, mSmallStep, 0x8);
WWHD_OFFSET(CloudWorkHD, mEpsilon, 0xc);
WWHD_OFFSET(CloudWorkHD, mMaximumAlpha, 0x10);
WWHD_OFFSET(CloudWorkHD, mHalf, 0x14);
WWHD_OFFSET(CloudWorkHD, mHeightStep, 0x18);
WWHD_OFFSET(CloudWorkHD, mVertical, 0x1c);
WWHD_OFFSET(CloudWorkHD, mAlphaStep, 0x20);
WWHD_OFFSET(CloudWorkHD, mParticleDragonName, 0x30);
WWHD_OFFSET(CloudWorkHD, mParticleStageName, 0x38);
WWHD_OFFSET(CloudWorkHD, mRadialPosition, 0x40);
WWHD_OFFSET(CloudWorkHD, mWindVectorPower, 0x4c);
WWHD_OFFSET(CloudWorkHD, mMovementPosition, 0x58);
WWHD_OFFSET(CloudWorkHD, mSeaStageName, 0x64);
WWHD_OFFSET(CloudWorkHD, mDragonName, 0x6c);
WWHD_OFFSET(CloudWorkHD, mNameName, 0x74);
WWHD_OFFSET(CloudWorkHD, mLinkRoomName, 0x7c);
WWHD_OFFSET(CloudWorkHD, mOrichName, 0x84);
WWHD_OFFSET(CloudWorkHD, mOjhousName, 0x8c);
WWHD_OFFSET(CloudWorkHD, mOmasaoName, 0x94);
WWHD_OFFSET(CloudWorkHD, mOnobutaName, 0x9c);
WWHD_OFFSET(CloudWorkHD, mDragonStage, 0xa4);
WWHD_OFFSET(CloudWorkHD, mNameStage, 0xac);
WWHD_OFFSET(CloudWorkHD, mLinkRoomStage, 0xb4);
WWHD_OFFSET(CloudWorkHD, mOrichStage, 0xbc);
WWHD_OFFSET(CloudWorkHD, mOjhousStage, 0xc4);
WWHD_OFFSET(CloudWorkHD, mOmasaoStage, 0xcc);
WWHD_OFFSET(CloudWorkHD, mOnobutaStage, 0xd4);
WWHD_OFFSET(CloudWorkHD, mSirenStage, 0xdc);
WWHD_OFFSET(CloudWorkHD, mAdmumiStage, 0xe4);
WWHD_SIZE(CloudWorkHD, 0xec);
struct WaveEffHD {
  /* 0x0 */ WeatherVectorHD mPos;
  /* 0xc */ WeatherVectorHD mBasePos;
  /* 0x18 */ be<f32> mSpeed;
  /* 0x1c */ be<f32> mScale;
  /* 0x20 */ be<f32> mCounterSpeed;
  /* 0x24 */ be<f32> mCounter;
  /* 0x28 */ be<f32> mAlpha;
  /* 0x2c */ be<f32> mStrengthEnv;
  /* 0x30 */ be<s16> mTimer;
  /* 0x32 */ be<s16> mAngle;
  /* 0x34 */ be<u8> mStatus;
  u8 padEnd[0x3];
};
WWHD_OFFSET(WaveEffHD, mPos, 0x0);
WWHD_OFFSET(WaveEffHD, mBasePos, 0xc);
WWHD_OFFSET(WaveEffHD, mSpeed, 0x18);
WWHD_OFFSET(WaveEffHD, mScale, 0x1c);
WWHD_OFFSET(WaveEffHD, mCounterSpeed, 0x20);
WWHD_OFFSET(WaveEffHD, mCounter, 0x24);
WWHD_OFFSET(WaveEffHD, mAlpha, 0x28);
WWHD_OFFSET(WaveEffHD, mStrengthEnv, 0x2c);
WWHD_OFFSET(WaveEffHD, mTimer, 0x30);
WWHD_OFFSET(WaveEffHD, mAngle, 0x32);
WWHD_OFFSET(WaveEffHD, mStatus, 0x34);
WWHD_SIZE(WaveEffHD, 0x38);
struct WavePacketHD {
  u8 pad0[0xa0];
  /* 0xa0 */ WaveEffHD mEff[300];
  /* 0x4240 */ be<f32> mSkewWidth;
  /* 0x4244 */ be<f32> mSkewDir;
};
WWHD_OFFSET(WavePacketHD, mEff, 0xa0);
WWHD_OFFSET(WavePacketHD, mSkewWidth, 0x4240);
WWHD_OFFSET(WavePacketHD, mSkewDir, 0x4244);
WWHD_SIZE(WavePacketHD, 0x4248);
struct WaveInfluenceHD {
  /* 0x0 */ WeatherVectorHD mPos;
  /* 0xc */ be<f32> mOuterRadius;
  /* 0x10 */ be<f32> mInnerRadius;
};
WWHD_OFFSET(WaveInfluenceHD, mPos, 0x0);
WWHD_OFFSET(WaveInfluenceHD, mOuterRadius, 0xc);
WWHD_OFFSET(WaveInfluenceHD, mInnerRadius, 0x10);
WWHD_SIZE(WaveInfluenceHD, 0x14);
struct WeatherPlayerHD {
  u8 pad0[0x314];
  /* 0x314 */ WeatherVectorHD mCurrentPos;
};
WWHD_OFFSET(WeatherPlayerHD, mCurrentPos, 0x314);
WWHD_SIZE(WeatherPlayerHD, 0x320);
struct WeatherMatrixHD {
  be<f32> cells[12];
};
WWHD_SIZE(WeatherMatrixHD, 48);
struct WeatherViewHD {
  u8 pad0[0xd4];
  /* 0xd4 */ be<f32> mFovy;
  u8 pad216[0x10c];
  /* 0x1e4 */ WeatherMatrixHD mViewRotMtx;
};
WWHD_OFFSET(WeatherViewHD, mFovy, 0xd4);
WWHD_OFFSET(WeatherViewHD, mViewRotMtx, 0x1e4);
WWHD_SIZE(WeatherViewHD, 0x214);
struct WaveWorkHD {
  /* 0x0 */ be<f32> mShorelineZeroZ;
  /* 0x4 */ be<f32> mShorelineThirdZ;
  /* 0x8 */ be<f32> mFadeMinimum;
  /* 0xc */ WeatherVectorHD mPosition;
  /* 0x18 */ WeatherVectorHD mDistancePosition;
  /* 0x24 */ WeatherSafeStringHD mAdmumiName;
  /* 0x2c */ WeatherSafeStringHD mAdmumiStage;
  /* 0x34 */ WeatherVectorHD mEyeVector;
  /* 0x40 */ WeatherVectorHD mFinalPosition;
  /* 0x4c */ WeatherVectorHD mShorelineCenter;
  /* 0x58 */ WeatherSafeStringHD mSeaStageName;
  /* 0x60 */ WeatherVectorHD mWindVectorPower;
  /* 0x6c */ WeatherVectorHD mWrapDirection;
  u8 pad120[0x8];
  /* 0x80 */ WeatherSafeStringHD mOpubName;
  /* 0x88 */ WeatherSafeStringHD mOrichName;
  /* 0x90 */ WeatherSafeStringHD mOjhousName;
  /* 0x98 */ WeatherSafeStringHD mLinkRoomName;
  /* 0xa0 */ WeatherSafeStringHD mOnobutaName;
  /* 0xa8 */ WeatherSafeStringHD mLinkRoomWindName;
  /* 0xb0 */ WeatherSafeStringHD mOrichWindName;
  /* 0xb8 */ WeatherSafeStringHD mOjhousWindName;
  /* 0xc0 */ WeatherSafeStringHD mOmasaoWindName;
  /* 0xc8 */ WeatherSafeStringHD mOnobutaWindName;
  /* 0xd0 */ WeatherSafeStringHD mUmikzStage;
  /* 0xd8 */ WeatherSafeStringHD mNameStage;
  /* 0xe0 */ WeatherSafeStringHD mOpubStage;
  /* 0xe8 */ WeatherSafeStringHD mOrichStage;
  /* 0xf0 */ WeatherSafeStringHD mOjhousStage;
  /* 0xf8 */ WeatherSafeStringHD mLinkRoomStage;
  /* 0x100 */ WeatherSafeStringHD mOnobutaStage;
  /* 0x108 */ WeatherSafeStringHD mLinkRoomWindStage;
  /* 0x110 */ WeatherSafeStringHD mOrichWindStage;
  /* 0x118 */ WeatherSafeStringHD mOjhousWindStage;
  /* 0x120 */ WeatherSafeStringHD mOmasaoWindStage;
  /* 0x128 */ WeatherSafeStringHD mOnobutaWindStage;
  /* 0x130 */ WeatherVectorHD mNormal;
  u8 pad316[0x4];
  /* 0x140 */ WeatherDoublePosHD mDeltaXZ;
  /* 0x158 */ WeatherMatrixHD mDrawMtx;
};
WWHD_OFFSET(WaveWorkHD, mShorelineZeroZ, 0x0);
WWHD_OFFSET(WaveWorkHD, mShorelineThirdZ, 0x4);
WWHD_OFFSET(WaveWorkHD, mFadeMinimum, 0x8);
WWHD_OFFSET(WaveWorkHD, mPosition, 0xc);
WWHD_OFFSET(WaveWorkHD, mDistancePosition, 0x18);
WWHD_OFFSET(WaveWorkHD, mAdmumiName, 0x24);
WWHD_OFFSET(WaveWorkHD, mAdmumiStage, 0x2c);
WWHD_OFFSET(WaveWorkHD, mEyeVector, 0x34);
WWHD_OFFSET(WaveWorkHD, mFinalPosition, 0x40);
WWHD_OFFSET(WaveWorkHD, mShorelineCenter, 0x4c);
WWHD_OFFSET(WaveWorkHD, mSeaStageName, 0x58);
WWHD_OFFSET(WaveWorkHD, mWindVectorPower, 0x60);
WWHD_OFFSET(WaveWorkHD, mWrapDirection, 0x6c);
WWHD_OFFSET(WaveWorkHD, mOpubName, 0x80);
WWHD_OFFSET(WaveWorkHD, mOrichName, 0x88);
WWHD_OFFSET(WaveWorkHD, mOjhousName, 0x90);
WWHD_OFFSET(WaveWorkHD, mLinkRoomName, 0x98);
WWHD_OFFSET(WaveWorkHD, mOnobutaName, 0xa0);
WWHD_OFFSET(WaveWorkHD, mLinkRoomWindName, 0xa8);
WWHD_OFFSET(WaveWorkHD, mOrichWindName, 0xb0);
WWHD_OFFSET(WaveWorkHD, mOjhousWindName, 0xb8);
WWHD_OFFSET(WaveWorkHD, mOmasaoWindName, 0xc0);
WWHD_OFFSET(WaveWorkHD, mOnobutaWindName, 0xc8);
WWHD_OFFSET(WaveWorkHD, mUmikzStage, 0xd0);
WWHD_OFFSET(WaveWorkHD, mNameStage, 0xd8);
WWHD_OFFSET(WaveWorkHD, mOpubStage, 0xe0);
WWHD_OFFSET(WaveWorkHD, mOrichStage, 0xe8);
WWHD_OFFSET(WaveWorkHD, mOjhousStage, 0xf0);
WWHD_OFFSET(WaveWorkHD, mLinkRoomStage, 0xf8);
WWHD_OFFSET(WaveWorkHD, mOnobutaStage, 0x100);
WWHD_OFFSET(WaveWorkHD, mLinkRoomWindStage, 0x108);
WWHD_OFFSET(WaveWorkHD, mOrichWindStage, 0x110);
WWHD_OFFSET(WaveWorkHD, mOjhousWindStage, 0x118);
WWHD_OFFSET(WaveWorkHD, mOmasaoWindStage, 0x120);
WWHD_OFFSET(WaveWorkHD, mOnobutaWindStage, 0x128);
WWHD_OFFSET(WaveWorkHD, mNormal, 0x130);
WWHD_OFFSET(WaveWorkHD, mDeltaXZ, 0x140);
WWHD_OFFSET(WaveWorkHD, mDrawMtx, 0x158);
WWHD_SIZE(WaveWorkHD, 0x188);
struct WeatherEmitterHD : JPABaseEmitter {
  u8 padRotation[0x1F0 - 0x70];
  /* 0x1f0 */ WeatherMatrixHD mGlobalRotation;
  /* 0x220 */ WeatherVectorHD mGlobalScale;
  /* 0x22c */ WeatherVectorHD mGlobalTranslation;
  /* 0x238 */ WeatherVectorHD mGlobalParticleScale;
  /* 0x244 */ be<u8> mGlobalPrmColor[3];
  /* 0x247 */ be<u8> mGlobalAlpha;
  u8 pad584[0xc];
  /* 0x254 */ be<u32> mStatus;
  u8 pad600[0xa];
  /* 0x262 */ be<u8> mVolumeType;
  u8 padEnd[0x1];
};
WWHD_OFFSET(WeatherEmitterHD, mGlobalRotation, 0x1f0);
WWHD_OFFSET(WeatherEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(WeatherEmitterHD, mGlobalTranslation, 0x22c);
WWHD_OFFSET(WeatherEmitterHD, mGlobalParticleScale, 0x238);
WWHD_OFFSET(WeatherEmitterHD, mGlobalPrmColor, 0x244);
WWHD_OFFSET(WeatherEmitterHD, mGlobalAlpha, 0x247);
WWHD_OFFSET(WeatherEmitterHD, mStatus, 0x254);
WWHD_OFFSET(WeatherEmitterHD, mVolumeType, 0x262);
WWHD_SIZE(WeatherEmitterHD, 0x264);
struct WindEffHD {
  /* 0x0 */ gptr<WeatherEmitterHD> mpEmitter;
  /* 0x4 */ WeatherVectorHD mBasePos;
  /* 0x10 */ WeatherVectorHD mPos;
  /* 0x1c */ be<f32> mStateTimer;
  /* 0x20 */ be<f32> mAlpha;
  /* 0x24 */ be<u32> mState;
  /* 0x28 */ be<u32> mLoopCounter;
  /* 0x2c */ be<s16> mSwerveAnimCounter;
  /* 0x2e */ be<s16> mAngleY;
  /* 0x30 */ be<s16> mAngleXZ;
  /* 0x32 */ be<u8> mLoopEnabled;
  u8 padEnd[0x1];
};
WWHD_OFFSET(WindEffHD, mpEmitter, 0x0);
WWHD_OFFSET(WindEffHD, mBasePos, 0x4);
WWHD_OFFSET(WindEffHD, mPos, 0x10);
WWHD_OFFSET(WindEffHD, mStateTimer, 0x1c);
WWHD_OFFSET(WindEffHD, mAlpha, 0x20);
WWHD_OFFSET(WindEffHD, mState, 0x24);
WWHD_OFFSET(WindEffHD, mLoopCounter, 0x28);
WWHD_OFFSET(WindEffHD, mSwerveAnimCounter, 0x2c);
WWHD_OFFSET(WindEffHD, mAngleY, 0x2e);
WWHD_OFFSET(WindEffHD, mAngleXZ, 0x30);
WWHD_OFFSET(WindEffHD, mLoopEnabled, 0x32);
WWHD_SIZE(WindEffHD, 0x34);
struct KamomeEffHD {
  /* 0x0 */ gptr<WeatherEmitterHD> mpEmitter;
  /* 0x4 */ WeatherVectorHD mPos;
  /* 0x10 */ be<s16> mAngleY;
  /* 0x12 */ be<s16> mAngleX;
  /* 0x14 */ be<f32> mAngleYSpeed;
  /* 0x18 */ be<f32> mScale;
  /* 0x1c */ be<s16> mTimer;
  /* 0x1e */ be<u8> mStatus;
  u8 padEnd[0x1];
};
WWHD_OFFSET(KamomeEffHD, mpEmitter, 0x0);
WWHD_OFFSET(KamomeEffHD, mPos, 0x4);
WWHD_OFFSET(KamomeEffHD, mAngleY, 0x10);
WWHD_OFFSET(KamomeEffHD, mAngleX, 0x12);
WWHD_OFFSET(KamomeEffHD, mAngleYSpeed, 0x14);
WWHD_OFFSET(KamomeEffHD, mScale, 0x18);
WWHD_OFFSET(KamomeEffHD, mTimer, 0x1c);
WWHD_OFFSET(KamomeEffHD, mStatus, 0x1e);
WWHD_SIZE(KamomeEffHD, 0x20);
struct WindPacketHD {
  /* 0x0 */ WindEffHD mWindEff[30];
  /* 0x618 */ KamomeEffHD mKamomeEff[10];
  /* 0x758 */ WeatherVectorHD mOldEyePosHD;
  /* 0x764 */ be<u8> mbHasCustomWindPower;
  u8 padEnd[0x3];
};
WWHD_OFFSET(WindPacketHD, mWindEff, 0x0);
WWHD_OFFSET(WindPacketHD, mKamomeEff, 0x618);
WWHD_OFFSET(WindPacketHD, mOldEyePosHD, 0x758);
WWHD_OFFSET(WindPacketHD, mbHasCustomWindPower, 0x764);
WWHD_SIZE(WindPacketHD, 0x768);
struct WeatherParticleListHD {
  u8 pad0[0x8];
  /* 0x8 */ be<u32> mBeginCount;
  u8 pad12[0x18];
  /* 0x24 */ be<u32> mEndCount;
};
WWHD_OFFSET(WeatherParticleListHD, mBeginCount, 0x8);
WWHD_OFFSET(WeatherParticleListHD, mEndCount, 0x24);
WWHD_SIZE(WeatherParticleListHD, 0x28);
struct WeatherParticleControllerHD {
  u8 opaque[4];
};
WWHD_SIZE(WeatherParticleControllerHD, 4);
struct WindScratchHD {
  u8 bytes[0x158];
};
WWHD_SIZE(WindScratchHD, 0x158);
struct WindWorkHD {
  u8 pad0[0x18];
  /* 0x18 */ be<f32> mPhaseStep;
  /* 0x1c */ be<f32> mSwayAmplitude;
  /* 0x20 */ be<f32> mFloatScratch;
  /* 0x24 */ be<f32> mFadeDistance;
  /* 0x28 */ be<f32> mPosWindScale;
  /* 0x2c */ be<f32> mZero;
  /* 0x30 */ be<f32> mStateStep;
  /* 0x34 */ be<f32> mOne;
  /* 0x38 */ be<f32> mMoveSpeed;
  /* 0x3c */ be<f32> mAlphaStep;
  /* 0x40 */ be<f32> mAlphaMinimum;
  /* 0x44 */ gptr<be<u32>> mpPolyVtable;
  /* 0x48 */ be<f32> mLoopChance;
  /* 0x4c */ gptr<be<u32>> mpGroupVtable;
  /* 0x50 */ be<u32> mBasePolyInfoVtable;
  /* 0x54 */ be<u32> mBasePolyPassVtable;
  /* 0x58 */ be<u32> mBaseGroupPassVtable;
  /* 0x5c */ be<f32> mWindLengthXZSquared;
  u8 pad96[0x8];
  /* 0x68 */ WeatherVectorHD mPosition;
  /* 0x74 */ WeatherVectorHD mMoveDirection;
  u8 pad128[0xc];
  /* 0x8c */ WeatherVectorHD mNewBirdTarget;
  /* 0x98 */ WeatherVectorHD mOldBirdTarget;
  u8 pad164[0xc];
  /* 0xb0 */ WeatherVectorHD mBirdPosition;
  u8 pad188[0xc];
  /* 0xc8 */ WeatherVectorHD mEyeVector;
  /* 0xd4 */ WeatherVectorHD mGroundPosition;
  /* 0xe0 */ WeatherVectorHD mBirdTargetRotation;
  /* 0xec */ WeatherSafeStringHD mSeaName;
  /* 0xf4 */ WeatherVectorHD mGroundOffset;
  /* 0x100 */ WeatherSafeStringHD mStageName;
  /* 0x108 */ dBgS_GndChk mGroundCheck;
  u8 padEnd[0x4];
};
WWHD_OFFSET(WindWorkHD, mPhaseStep, 0x18);
WWHD_OFFSET(WindWorkHD, mSwayAmplitude, 0x1c);
WWHD_OFFSET(WindWorkHD, mFloatScratch, 0x20);
WWHD_OFFSET(WindWorkHD, mFadeDistance, 0x24);
WWHD_OFFSET(WindWorkHD, mPosWindScale, 0x28);
WWHD_OFFSET(WindWorkHD, mZero, 0x2c);
WWHD_OFFSET(WindWorkHD, mStateStep, 0x30);
WWHD_OFFSET(WindWorkHD, mOne, 0x34);
WWHD_OFFSET(WindWorkHD, mMoveSpeed, 0x38);
WWHD_OFFSET(WindWorkHD, mAlphaStep, 0x3c);
WWHD_OFFSET(WindWorkHD, mAlphaMinimum, 0x40);
WWHD_OFFSET(WindWorkHD, mpPolyVtable, 0x44);
WWHD_OFFSET(WindWorkHD, mLoopChance, 0x48);
WWHD_OFFSET(WindWorkHD, mpGroupVtable, 0x4c);
WWHD_OFFSET(WindWorkHD, mBasePolyInfoVtable, 0x50);
WWHD_OFFSET(WindWorkHD, mBasePolyPassVtable, 0x54);
WWHD_OFFSET(WindWorkHD, mBaseGroupPassVtable, 0x58);
WWHD_OFFSET(WindWorkHD, mWindLengthXZSquared, 0x5c);
WWHD_OFFSET(WindWorkHD, mPosition, 0x68);
WWHD_OFFSET(WindWorkHD, mMoveDirection, 0x74);
WWHD_OFFSET(WindWorkHD, mNewBirdTarget, 0x8c);
WWHD_OFFSET(WindWorkHD, mOldBirdTarget, 0x98);
WWHD_OFFSET(WindWorkHD, mBirdPosition, 0xb0);
WWHD_OFFSET(WindWorkHD, mEyeVector, 0xc8);
WWHD_OFFSET(WindWorkHD, mGroundPosition, 0xd4);
WWHD_OFFSET(WindWorkHD, mBirdTargetRotation, 0xe0);
WWHD_OFFSET(WindWorkHD, mSeaName, 0xec);
WWHD_OFFSET(WindWorkHD, mGroundOffset, 0xf4);
WWHD_OFFSET(WindWorkHD, mStageName, 0x100);
WWHD_OFFSET(WindWorkHD, mGroundCheck, 0x108);
WWHD_SIZE(WindWorkHD, 0x160);
struct WeatherEnvironmentHD {
  u8 pad0[0x14];
  /* 0x14 */ WeatherVectorHD mBaseLightPos;
  /* 0x20 */ be<s16> mBaseLightRed;
  u8 padWaveSettings[0x9D8 - 0x22];
  be<f32> mWaveSpeed, mWaveSpawnDist, mWaveSpawnRadius;
  u8 padWaveScale[4];
  be<f32> mWaveScaleRand, mWaveCounterSpeedScale;
  u8 padWaveFlat[4];
  be<f32> mWaveFlatInter;
  be<s16> mWaveCount;
  be<u8> mWaveReset;
  u8 padCustomWind[0xA20 - 0x9FB];
  be<f32> mCustomWindPower;
  be<u16> mTactWindAngleX;
  be<s16> mTactWindAngleY;
  u8 padWindAngles[0xA34 - 0xA28];
  /* 0xa34 */ gptr<SunPacketHD> mpSunPacket;
  /* 0xa38 */ gptr<SunLenzPacketHD> mpSunlenzPacket;
  u8 pad2620[0x10];
  /* 0xa4c */ union {
    be<u32> mSnowRequestHD;
    struct {
      be<u16> mSnowRequestHighHD;
      be<s16> mSnowCount;
    };
  };
  /* 0xa50 */ gptr<SnowPacketHD> mpSnowPacket;
  u8 padVrkumo[0xA8C - 0xA54];
  be<s32> mVrkumoCount;
  be<f32> mVrkumoStrength;
  gptr<VrkumoPacketHD> mpVrkumoPacket;
  u8 padWavePacket[0xAA0 - 0xA98];
  gptr<WavePacketHD> mpWavePacket;
  u8 padWindCount[4];
  be<u32> mWindlineCount;
  gptr<WindPacketHD> mpWind;
  u8 padWaveInfluences[0xAEC - 0xAB0];
  gptr<WaveInfluenceHD> mpWaveInfl[10];
  u8 padSunPosition[0xB38 - 0xB14];
  /* 0xb38 */ WeatherVectorHD mSunPos2;
  u8 padBackgroundColor[0xB70 - 0xB44];
  be<u8> mBG0_K0[3];
  u8 padColorBlend[0xFC8 - 0xB73];
  /* 0xfc8 */ be<f32> mColPatBlend;
  u8 pad4044[0x54];
  /* 0x1020 */ be<f32> mCurTime;
  u8 pad4132[0x68];
  /* 0x108c */ be<u8> mColpatPrev;
  /* 0x108d */ be<u8> mColpatCurr;
  u8 pad4238[0x4];
  /* 0x1092 */ be<u8> mColpatWeather;
  u8 padEnd[0x1];
};
WWHD_OFFSET(WeatherEnvironmentHD, mBaseLightPos, 0x14);
WWHD_OFFSET(WeatherEnvironmentHD, mBaseLightRed, 0x20);
WWHD_OFFSET(WeatherEnvironmentHD, mpSunPacket, 0xa34);
WWHD_OFFSET(WeatherEnvironmentHD, mpSunlenzPacket, 0xa38);
WWHD_OFFSET(WeatherEnvironmentHD, mSnowRequestHD, 0xa4c);
WWHD_OFFSET(WeatherEnvironmentHD, mpSnowPacket, 0xa50);
WWHD_OFFSET(WeatherEnvironmentHD, mSunPos2, 0xb38);
WWHD_OFFSET(WeatherEnvironmentHD, mColPatBlend, 0xfc8);
WWHD_OFFSET(WeatherEnvironmentHD, mCurTime, 0x1020);
WWHD_OFFSET(WeatherEnvironmentHD, mColpatPrev, 0x108c);
WWHD_OFFSET(WeatherEnvironmentHD, mColpatCurr, 0x108d);
WWHD_OFFSET(WeatherEnvironmentHD, mColpatWeather, 0x1092);
WWHD_SIZE(WeatherEnvironmentHD, 0x1094);
WWHD_OFFSET(WeatherEnvironmentHD, mSnowCount, 0xA4E);
struct WeatherPlayHD {
  u8 padBackground[0x12A0];
  u8 mBackgroundSystem[4];
  u8 padStageName[0x5134 - 0x12A4];
  /* 0x5134 */ be<u8> mStageName[0x1C];
  /* 0x5150 */ WeatherStageDtHD mStage;
  u8 padRoom[0x51CC - 0x5154];
  WeatherRoomControlHD mRoomControl;
  u8 padParticles[0x5AB0 - 0x51D0];
  gptr<WeatherParticleControllerHD> mpParticle;
  u8 padCamera[0x5AF8 - 0x5AB4];
  /* 0x5af8 */ gptr<WeatherCameraHD> mpCamera;
  u8 padPlayer[0x5B2C - 0x5AFC];
  gptr<WeatherPlayerHD> mpPlayer;
  u8 padEventStatus[0x5CDC - 0x5B30];
  be<u32> mEventStatus;
  u8 padView[0x5FA4 - 0x5CE0];
  gptr<WeatherViewHD> mpView;
  u8 padPeekZ[0x60EC - 0x5FA8];
  /* 0x60ec */ u8 mPeekZService[4];
};
WWHD_OFFSET(WeatherPlayHD, mStageName, 0x5134);
WWHD_OFFSET(WeatherPlayHD, mStage, 0x5150);
WWHD_OFFSET(WeatherPlayHD, mpCamera, 0x5af8);
WWHD_OFFSET(WeatherPlayHD, mPeekZService, 0x60ec);
WWHD_SIZE(WeatherPlayHD, 0x60f0);
struct WeatherTrigPairHD {
  be<f32> sin, cos;
};
struct WeatherTrigHD {
  WeatherTrigPairHD entries[8192];
};
static f32 weatherSin(u32 angle);
static f32 weatherCos(u32 angle);
} // namespace weather_move
static weather_move::WeatherEnvironmentHD *weatherEnvironment();
static weather_move::WeatherPlayHD *weatherPlay();
static void dKyw_get_wind_vecpowHD(weather_move::WeatherVectorHD *);
static weather_move::WeatherVectorHD *dKyw_get_wind_vecHD();
static f32 dKyw_get_wind_powHD();
static void dKy_set_eyevect_calc2HD(weather_move::WeatherCameraHD *,
                                    weather_move::WeatherVectorHD *, f32, f32);
static f32 weatherDistanceSquared(weather_move::WeatherVectorHD *,
                                  weather_move::WeatherVectorHD *);
static f64 weatherSqrtDouble(f64);
static void vectle_calcHD(weather_move::WeatherDoublePosHD *,
                          weather_move::WeatherVectorHD *);
static f32 cM3d_VectorProduct2dHD(f32, f32, f32, f32, f32, f32);
static void weatherAddCalc(be<f32> *, f32, f32, f32, f32);
static u32 weatherFloatAngle(f32);
static void get_vectle_calcHD(weather_move::WeatherVectorHD *,
                              weather_move::WeatherVectorHD *,
                              weather_move::WeatherVectorHD *);
static void weatherProjectParams(weather_move::WeatherProjectParamsHD *);
static void weatherProject(weather_move::WeatherVectorHD *,
                           weather_move::WeatherVectorHD *,
                           weather_move::WeatherProjectParamsHD *);
static void dKyr_get_vectle_calcHD(weather_move::WeatherVectorHD *,
                                   weather_move::WeatherVectorHD *,
                                   weather_move::WeatherVectorHD *);
static void weatherPeekZ(void *, s32, s32, be<u32> *);
static void dKy_set_actcol_ratioHD(f32);
static void dKy_set_bgcol_ratioHD(f32);
static void dKy_set_fogcol_ratioHD(f32);
static void dKy_set_vrboxcol_ratioHD(f32);
static u32 dKyr_moon_arrival_checkHD();
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mTactWindAngleX, 0xa24);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mTactWindAngleY, 0xa26);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mVrkumoCount, 0xa8c);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mVrkumoStrength, 0xa90);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mpVrkumoPacket, 0xa94);
WWHD_OFFSET(weather_move::WeatherPlayHD, mRoomControl, 0x51cc);
WWHD_OFFSET(weather_move::WeatherPlayHD, mpView, 0x5fa4);
static weather_move::WeatherRoomDtHD *
weatherRoomStatus(weather_move::WeatherRoomControlHD *, s32);
static f32 weatherMagnitudeSquared(weather_move::WeatherVectorHD *);
static f32 weatherSqrtFloat(f32);
static void weatherDistanceForward(weather_move::WeatherVectorHD *,
                                   weather_move::WeatherVectorHD *);
static f32 weatherSqrtForward();
static void weatherMatrixInverse(weather_move::WeatherMatrixHD *,
                                 weather_move::WeatherMatrixHD *);
static f32 weatherSinRadians(f32);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveSpeed, 0x9d8);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveSpawnDist, 0x9dc);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveSpawnRadius, 0x9e0);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveScaleRand, 0x9e8);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveCounterSpeedScale, 0x9ec);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveFlatInter, 0x9f4);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveCount, 0x9f8);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWaveReset, 0x9fa);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mpWavePacket, 0xaa0);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mpWaveInfl, 0xaec);
WWHD_OFFSET(weather_move::WeatherPlayHD, mpPlayer, 0x5B2C);
static void weatherGroundConstruct(dBgS_GndChk *);
static void weatherGroundDestruct(dBgS_GndChk *, s32);
static f32 weatherGroundCross(void *, dBgS_GndChk *);
static void weatherEmitterDelete(weather_move::WeatherEmitterHD *, u32);
static weather_move::WeatherEmitterHD *
weatherParticleSet(weather_move::WeatherParticleControllerHD *, u32, u32,
                   weather_move::WeatherVectorHD *, u32, u32, u32, u32, s32,
                   u32, u32, u32);
static u32 weatherAtan2Angle(f32, f32);
static void weatherAddCalcAngle(be<s16> *, u32, s32, s32, s32);
static void weatherVectorScale(weather_move::WeatherVectorHD *,
                               weather_move::WeatherVectorHD *, f32);
static void weatherVectorAdd(weather_move::WeatherVectorHD *,
                             weather_move::WeatherVectorHD *,
                             weather_move::WeatherVectorHD *);
static void weatherRotationMatrix(u32, u32, u32,
                                  weather_move::WeatherMatrixHD *);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mCustomWindPower, 0xa20);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mWindlineCount, 0xaa8);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mpWind, 0xaac);
WWHD_OFFSET(weather_move::WeatherEnvironmentHD, mBG0_K0, 0xb70);
WWHD_OFFSET(weather_move::WeatherPlayHD, mBackgroundSystem, 0x12a0);
WWHD_OFFSET(weather_move::WeatherPlayHD, mpParticle, 0x5ab0);
WWHD_OFFSET(weather_move::WeatherPlayHD, mEventStatus, 0x5cdc);

// HD native entry adapters retain the exact return/argument widths. In
// particular, vector square roots have distinct double, single and
// forwarded-FPR forms; atan2/float-angle adapters preserve the full native
// register before narrowing.
WWHD_OFFSET(weather_move::WeatherVectorHD, x, 0);
WWHD_OFFSET(weather_move::WeatherVectorHD, y, 4);
WWHD_OFFSET(weather_move::WeatherVectorHD, z, 8);
WWHD_OFFSET(weather_move::WeatherDoublePosHD, x, 0);
WWHD_OFFSET(weather_move::WeatherDoublePosHD, y, 8);
WWHD_OFFSET(weather_move::WeatherDoublePosHD, z, 16);
WWHD_OFFSET(weather_move::StageInfoHD, mTypeFlags, 12);
WWHD_OFFSET(weather_move::StageVtableHD, getStagInfo, 0x15C);
WWHD_OFFSET(weather_move::WeatherRoomVtableHD, getFileListInfo, 0x1DC);
WWHD_OFFSET(weather_move::SafeStringVtableHD, assureTermination, 0x14);
WWHD_OFFSET(weather_move::WeatherSafeStringHD, mStringTop, 0);
WWHD_SIZE(weather_move::WeatherTrigPairHD, 8);
WWHD_SIZE(weather_move::WeatherTrigHD, 0x10000);
namespace weather_move {
enum WeatherParticleIdHD : u32 {
  ID_AK_JN_WINDLINE00 = 0x31,
  ID_AK_JN_KAMOME00 = 0x429,
};
constexpr u32 WindlinePlayerEventHD = 0x20;
} // namespace weather_move
