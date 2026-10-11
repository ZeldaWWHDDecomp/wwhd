#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "gabi.h"
namespace WeatherHelpers {
using namespace gabi;
template <class T> inline T *view(u32 address) { return at<T>(address); }
struct DOUBLE_POS {
  be<f64> x, y, z;
};
struct Matrix {
  be<f32> cells[12];
};
struct ColorF {
  be<f32> channels[4];
};
struct Quad {
  cXyz vertices[4];
};
struct Color {
  be<u8> channels[4];
};
struct Thunder {
  be<u32> status, mode;
  be<f32> mFlashTimer;
};
struct SafeString {
  be<u32> text, vtable;
};
struct SafeStringMethods {
  u8 prefix[0x14];
  be<u32> cstrCallback;
};
struct Shader {
  u8 prefix[0x24];
  be<u32> uniformRelativeOffset;
};
struct ProjectParams {
  u8 bytes[40];
};
struct MassCollisionResult {
  be<u32> flags, object;
  u8 tail[12];
};
struct CollisionObject {
  u8 prefix[0x10];
  be<u32> attackType;
};
struct dSnap_Obj {
  u8 data[52];
};
struct Camera {
  u8 prefix[0xdc];
  cXyz mEye, mCenter;
};
struct WAVECHAN {
  be<f32> field_0x0, field_0x4, field_0x8, mWaveSpeed, mWaveSpawnDist,
      mWaveSpawnRadius, mWaveScale, mWaveScaleRand, mWaveCounterSpeedScale,
      mWaveScaleBottom, mWaveFlatInter;
  be<u16> mWaveCount;
  be<u8> mWaveReset, field_0x2f;
};
// HD: environment packet pointers and camera/controller members have moved.
struct Environment {
  u8 prefix[0x9cc];
  WAVECHAN mWaveChan;
  u8 timeData[0x1020 - 0x9fc];
  be<f32> mCurTime;
};
struct EnvironmentPackets {
  u8 prefix[0xa44];
  be<u32> mpRainPacket;
  u8 gap0[8];
  be<u32> mpSnowPacket;
  u8 gap1[6];
  be<s16> mStarCount;
  u8 gap2[4];
  be<u32> mpStarPacket;
  u8 gap3[4];
  be<s32> mPoisonCount;
  be<u32> mpPoisonPacket;
  u8 gap4[0xaac - 0xa70];
  be<u32> mpWind;
  be<u8> thunderStatus, thunderState;
};
struct MassAttr {
  u8 prefix[0x18];
  be<u8> targetType, targetMask;
};
struct CollisionSpace {
  u8 prefix[0x110];
  MassAttr mass;
};
struct GameInfo {
  u8 prefix[0x12a0];
  u8 background[0x4ef8 - 0x12a0];
  CollisionSpace collision;
  u8 cameraGap[0x5af8 - 0x4ef8 - sizeof(CollisionSpace)];
  be<u32> camera;
  u8 playerGap[0x5cdc - 0x5afc];
  be<u32> playerStatus;
  u8 viewGap[0x5fa4 - 0x5ce0];
  be<u32> view;
};
struct KAMOME_EFF {
  be<u32> mpEmitter;
  cXyz mPos;
  be<s16> mAngleY, mAngleX;
  be<f32> mAngleYSpeed, mScale;
  be<u16> mTimer;
  be<u8> mStatus, pad;
};
struct WINDEFF_SET {
  u8 prefix[0x618];
  KAMOME_EFF mKamomeEff[10];
  cXyz mOldEyePos;
  be<u8> mbHasCustomWindPower, field_0x759;
};
struct RAIN_EFF {
  be<u8> mStatus;
  u8 pad[3];
  cXyz mPos, mBasePos;
  be<f32> field_0x1c, field_0x20, field_0x24, mAlpha, field_0x2c, field_0x30;
  be<s16> mTimer;
  u8 tail[2];
};
struct dKankyo_rain_Packet {
  u8 prefix[0x98];
  be<u32> mpTxSnow01, mpTxRingAHalf;
  RAIN_EFF mEff[250];
  u8 countGap[8];
  be<u16> mRainCount;
  u8 eyeGap[6];
  cXyz mCamEyePos, mCamCenterPos, mCenterDelta;
  union {
    struct {
      be<f32> mCenterDeltaMul, field_0x3700, mSibukiAlpha, mOverheadFade,
          mFwdFade1, mFwdFade2;
    };
    be<f32> resetFields[6];
  };
  be<u8> mStatus;
};
struct POISON_EFF {
  be<u8> mStatus;
  u8 pad[3];
  cXyz mPos;
  u8 data[0x24 - 0x10];
  be<f32> mAlpha, mSize;
  be<s16> field_0x2c;
  be<u16> field_0x2e;
};
struct dKankyo_poison_Packet {
  u8 prefix[0x98];
  POISON_EFF mEff[1000];
  cXyz mBasePos, field_0xbb9c;
  be<u32> mCount, mpTexture;
  be<f32> mRot;
};
// HD: star brightness, phase and an integer720-frame counter replace the GC effect animation.
struct dKankyo_star_Packet {
  u8 prefix[0x98];
  be<u32> mpTexture;
  u8 gap0[0xc0 - 0x9c];
  be<f32> mSin;
  u8 gap1[6];
  be<u16> mAnimCounter, mEffectNum;
  u8 gap2[2];
  be<u32> mFrameCounter;
};
struct PolyPass {
  be<u32> vtable;
  be<u8> mbObjThrough, mbCamThrough, mbLinkThrough, mbArrowThrough,
      mbBombThrough, mbBoomerangThrough, mbRopeThrough;
  u8 padding;
};
struct GroupPass {
  be<u32> vtable, mGrp;
};
struct dBgS_ObjGndChk_All {
  be<u32> mPolyPassChk, mGrpPassChk;
  u8 baseData[8];
  be<u32> vtable;
  u8 polyData[12];
  be<u32> polyVtable;
  cXyz m_pos;
  u8 groundData[16];
  PolyPass poly;
  GroupPass group;
};
// HD: RoofChk adds4 bytes at the end (80 bytes total).
struct dBgS_RoofChk {
  u8 prefix[0x10], queryBase[0x10];
  be<u32> polyVtable, groupVtable;
  u8 baseData[8];
  be<u32> vtable;
  u8 padding[4];
  cXyz m_pos;
  u8 tail[12];
};
WWHD_SIZE(DOUBLE_POS, 24);
WWHD_SIZE(Matrix, 48);
WWHD_SIZE(ColorF, 16);
WWHD_SIZE(Quad, 48);
WWHD_SIZE(SafeString, 8);
WWHD_SIZE(MassCollisionResult, 20);
WWHD_SIZE(dSnap_Obj, 52);
WWHD_SIZE(KAMOME_EFF, 32);
WWHD_SIZE(RAIN_EFF, 56);
WWHD_SIZE(POISON_EFF, 48);
WWHD_SIZE(dBgS_ObjGndChk_All, 84);
WWHD_SIZE(dBgS_RoofChk, 80);
WWHD_OFFSET(WINDEFF_SET, mOldEyePos, 0x758);
WWHD_OFFSET(WINDEFF_SET, mbHasCustomWindPower, 0x764);
WWHD_OFFSET(dKankyo_rain_Packet, mCamEyePos, 0x3760);
WWHD_OFFSET(dKankyo_rain_Packet, resetFields, 0x3784);
WWHD_OFFSET(dKankyo_star_Packet, mFrameCounter, 0xd0);
WWHD_OFFSET(GameInfo, camera, 0x5af8);
inline be<f32> &component(cXyz *p, u32 index) {
  return *view<be<f32>>(ea(p) + index * 4);
}
inline be<u32> &wordComponent(cXyz *p, u32 index) {
  return *view<be<u32>>(ea(p) + index * 4);
}
enum ConstantAddress : u32 {
  VectorEpsilon = 0x100030B8,
  Zero = 0x1004F528,
  DoubleOne = 0x1004F520,
  DoubleZero = 0x1004F530,
  EyeHeightOffset = 0x1004F52C,
  WindTimerRange = 0x1004F538,
  MoonEvening = 0x1004F60C,
  MoonMorning = 0x1004F610,
  RainFloorOffset = 0x1004F6B4,
  One = 0x1004F550,
  WaveInitial = 0x1004FAEC,
  WaveFirstSpeed = 0x1004F584,
  WaveSpawnDistance = 0x1004F7E8,
  WaveSpawnRadius = 0x1004FB6C,
  WaveScale = 0x1004F54C,
  WaveBottomScale = 0x1004F730,
  WaveRandomScale = 0x1004FB70,
  WaveCounterSpeed = 0x1004F5B4,
  WaveSpeed = 0x1004F588,
  ColorScale = 0x1004F5AC,
  GammaExponent = 0x1004FC90,
  PoisonMassRadius = 0x1004F918,
  PoisonMassHeight = 0x1004F91C,
  PoisonVerticalOffset = 0x1004F920,
  InitialScaleX = 0x1004FD28,
  InitialScaleY = 0x1004FD2C,
  SnapScale = 0x1004F6B8,
  SnapRadius = 0x1004F614,
  StarWaveScale = 0x1004F844,
  StarWaveBase = 0x1004F640,
  StarRandomPhase = 0x1004F5D0,
  OverheadOffset = 0x1004F564,
  InvalidRoofHeight = 0x1004F630,
  GroundQueryOffset = 0x1004F594
};
enum GuestIdentity : u32 {
  RainArchiveName = 0x1004F67C,
  RainResourceName = 0x1004F6A4,
  SafeStringVtable = 0x1004F3AC,
  RainSnowUniform = 0x1004F684,
  RainRingUniform = 0x1004F690,
  StarHeapName = 0x1004F828,
  AlwaysArchiveName = 0x1004F820,
  PoisonHeapName = 0x1004FA98,
  PoisonArchiveName = 0x1004FA90,
  PoisonResourceName = 0x1004FAB8,
  PoisonUniformName = 0x1004FAC8,
  GroundGroupVtable = 0x1004F494,
  DerivedGroundVtable = 0x1004F474,
  DerivedGroundPolyVtable = 0x1004F484,
  GroundPassVtable = 0x1004F4A4,
  BaseGroupVtable = 0x1004F3E4,
  RoofPolyVtable = 0x1004F4B4,
  RoofGroupVtable = 0x1004F4D4,
  BasePassVtable = 0x1004F424,
  BaseGroundPolyVtable = 0x1004F404,
  InitializerVector0 = 0x101E9A28,
  InitializerVector1 = 0x101E9A34,
  InitializerVector2 = 0x101E9A40,
  InitializerByte0 = 0x1047741C,
  InitializerByte1 = 0x1047741D
};
constexpr u32 StarHeapBytes = 0x49AED0, StarPacketBytes = 0x49ADD8,
              PoisonHeapBytes = 0x4FAA10, PoisonPacketBytes = 0x20BCA8;
enum {
  DSNAP_TYPE_UNK07 = 7,
  DSNAP_TYPE_UNK08 = 8,
  DSNAP_TYPE_UNK09 = 9,
  FULL_GRP = 15,
  PoisonActive = 1,
  PoisonHit = 2
};
inline f32 constant(u32 address) { return *view<be<f32>>(address); }
inline f64 doubleConstant(u32 address) { return *view<be<f64>>(address); }
inline be<u32> &resourceManager() { return *view<be<u32>>(0x101F4F7C); }
inline be<u32> &objectResourceManager() { return *view<be<u32>>(0x101F4F28); }
inline be<s32> &now_room() { return *view<be<s32>>(0x101E99F0); }
inline be<u8> &stayRoom() { return *view<be<u8>>(0x1047E6C8); }
inline be<u32> &initializerWord(u32 i) { return view<be<u32>>(0x104774B0)[i]; }
inline be<f32> &initialScale(u32 i) { return view<be<f32>>(0x10477410)[i]; }
inline f32 sampleSine(u32 angle) {
  return *view<be<f32>>(0x104A44F8 + (angle & 0xfff8));
}
namespace sdk {
/* 02572E9C [v d_kankyo_rain] */
template <class R = void, class... A> inline R gammaComponent(A... args) {
  return gabi::call<R>(0x02572E9C, args...);
}
/* 028F4384 [v d_kankyo_rain] */
template <class R = void, class... A> inline R squareRoot(A... args) {
  return gabi::call<R>(0x028F4384, args...);
}
/* 02563E48 [v d_kankyo_rain] */
template <class R = void, class... A>
inline R normalizeDoubleVector(A... args) {
  return gabi::call<R>(0x02563E48, args...);
}
/* 02563F0C [v d_kankyo_rain] */
template <class R = void, class... A> inline R directionVector(A... args) {
  return gabi::call<R>(0x02563F0C, args...);
}
/* 02563F64 [v d_kankyo_rain] */
template <class R = void, class... A>
inline R weatherDirectionVector(A... args) {
  return gabi::call<R>(0x02563F64, args...);
}
/* 02555D0C [v d_kankyo_rain] */
template <class R = void, class... A> inline R getEnvironment(A... args) {
  return gabi::call<R>(0x02555D0C, args...);
}
/* 025200D4 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getGameInfo(A... args) {
  return gabi::call<R>(0x025200D4, args...);
}
/* 020198D8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R randomF(A... args) {
  return gabi::call<R>(0x020198D8, args...);
}
/* 026124B0 [v d_kankyo_rain] */
template <class R = void, class... A> inline R loadArchiveResource(A... args) {
  return gabi::call<R>(0x026124B0, args...);
}
/* 027E2DC0 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getShader(A... args) {
  return gabi::call<R>(0x027E2DC0, args...);
}
/* 027DFA24 [v d_kankyo_rain] */
template <class R = void, class... A> inline R findUniform(A... args) {
  return gabi::call<R>(0x027DFA24, args...);
}
/* 0257F490 [v d_kankyo_rain] */
template <class R = void, class... A>
inline R initializeRainRenderer(A... args) {
  return gabi::call<R>(0x0257F490, args...);
}
/* 025F01D8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R beginScopedHeap(A... args) {
  return gabi::call<R>(0x025F01D8, args...);
}
/* 0273AE48 [v d_kankyo_rain] */
template <class R = void, class... A> inline R allocate(A... args) {
  return gabi::call<R>(0x0273AE48, args...);
}
/* 02580ED8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructStarPacket(A... args) {
  return gabi::call<R>(0x02580ED8, args...);
}
/* 026066C4 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getObjectResource(A... args) {
  return gabi::call<R>(0x026066C4, args...);
}
/* 025F02D0 [v d_kankyo_rain] */
template <class R = void, class... A> inline R cancelScopedHeap(A... args) {
  return gabi::call<R>(0x025F02D0, args...);
}
/* 025F0270 [v d_kankyo_rain] */
template <class R = void, class... A> inline R endScopedHeap(A... args) {
  return gabi::call<R>(0x025F0270, args...);
}
/* 028F4560 [v d_kankyo_rain] */
template <class R = void, class... A> inline R power(A... args) {
  return gabi::call<R>(0x028F4560, args...);
}
/* 0256C8BC [v d_kankyo_rain] */
template <class R = void, class... A> inline R poisonLiveCheck(A... args) {
  return gabi::call<R>(0x0256C8BC, args...);
}
/* 020184DC [v d_kankyo_rain] */
template <class R = void, class... A> inline R setMassRadius(A... args) {
  return gabi::call<R>(0x020184DC, args...);
}
/* 02018428 [v d_kankyo_rain] */
template <class R = void, class... A> inline R setMassHeight(A... args) {
  return gabi::call<R>(0x02018428, args...);
}
/* 0201AD78 [v d_kankyo_rain] */
template <class R = void, class... A> inline R addVector(A... args) {
  return gabi::call<R>(0x0201AD78, args...);
}
/* 0251694C [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructMassResult(A... args) {
  return gabi::call<R>(0x0251694C, args...);
}
/* 025170D8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R massCheck(A... args) {
  return gabi::call<R>(0x025170D8, args...);
}
/* 028F026C [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructVector(A... args) {
  return gabi::call<R>(0x028F026C, args...);
}
/* 028ED6F8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R initializeByteState(A... args) {
  return gabi::call<R>(0x028ED6F8, args...);
}
/* 028EAB2C [v d_kankyo_rain] */
template <class R = void, class... A>
inline R initializeSecondByteState(A... args) {
  return gabi::call<R>(0x028EAB2C, args...);
}
/* 025812EC [v d_kankyo_rain] */
template <class R = void, class... A>
inline R constructPoisonPacket(A... args) {
  return gabi::call<R>(0x025812EC, args...);
}
/* 02581518 [v d_kankyo_rain] */
template <class R = void, class... A>
inline R initializePoisonRenderer(A... args) {
  return gabi::call<R>(0x02581518, args...);
}
/* 0256CA54 [v d_kankyo_rain] */
template <class R = void, class... A> inline R movePoison(A... args) {
  return gabi::call<R>(0x0256CA54, args...);
}
/* 025BD71C [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructSnapObject(A... args) {
  return gabi::call<R>(0x025BD71C, args...);
}
/* 025BEF3C [v d_kankyo_rain] */
template <class R = void, class... A> inline R setSnapSphere(A... args) {
  return gabi::call<R>(0x025BEF3C, args...);
}
/* 025BEB90 [v d_kankyo_rain] */
template <class R = void, class... A> inline R setSnapInfo(A... args) {
  return gabi::call<R>(0x025BEB90, args...);
}
/* 025BEB4C [v d_kankyo_rain] */
template <class R = void, class... A> inline R registerSnapObject(A... args) {
  return gabi::call<R>(0x025BEB4C, args...);
}
/* 02019918 [v d_kankyo_rain] */
template <class R = void, class... A> inline R randomFX(A... args) {
  return gabi::call<R>(0x02019918, args...);
}
/* 02008E0C [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructGroundQuery(A... args) {
  return gabi::call<R>(0x02008E0C, args...);
}
/* 024EE7AC [v d_kankyo_rain] */
template <class R = void, class... A> inline R constructRoofQuery(A... args) {
  return gabi::call<R>(0x024EE7AC, args...);
}
/* 024EF6E8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R roofCheck(A... args) {
  return gabi::call<R>(0x024EF6E8, args...);
}
/* 02008974 [v d_kankyo_rain] */
template <class R = void, class... A> inline R groundCross(A... args) {
  return gabi::call<R>(0x02008974, args...);
}
/* 02008B4C [v d_kankyo_rain] */
template <class R = void, class... A>
inline R destroyBackgroundQuery(A... args) {
  return gabi::call<R>(0x02008B4C, args...);
}
/* 02008DAC [v d_kankyo_rain] */
template <class R = void, class... A> inline R destroyGroundQuery(A... args) {
  return gabi::call<R>(0x02008DAC, args...);
}
} // namespace sdk
} // namespace WeatherHelpers
