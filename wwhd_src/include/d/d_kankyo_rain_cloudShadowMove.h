/* Weather renderer views: GameCube particle names, HD packet and draw-buffer layout.
 * HD: packets own double-buffered vertex managers and material uniforms instead of GX immediate draws.
 */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "gabi.h"
namespace RainReadable {
using namespace gabi;
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
struct Matrix {
  be<f32> cells[12];
};
struct ColorF {
  be<f32> channels[4];
};
union Color {
  be<u32> rgba;
  be<u8> channels[4];
};
union Quad {
  cXyz vertices[4];
  be<u32> words[4][3];
};
union VertexPosition {
  cXyz value;
  be<u32> words[3];
};
struct Vertex {
  VertexPosition position;
  be<f32> u, v;
};
struct BufferSlot {
  be<u32> buffer;
  u8 flushObject[0x14c];
  be<u32> size;
  u8 tail[0x254 - 0x154];
};
struct Geometry {
  BufferSlot slots[2];
  be<u32> selected;
  u8 tail[0x4c0 - 0x4ac];
};
struct Uniform {
  u8 data[0x370];
};
struct SafeString {
  be<u32> text, vtable;
};
struct CLOUD_EFF {
  be<u8> mStatus;
  u8 padding[3];
  cXyz mPos, mBasePos, mPntWindVel, mVelRndm;
  be<f32> field_0x34, mWindSpeed;
  be<u16> mAnimCounter;
  u8 pad2[2];
  be<f32> mAlpha, mSize, mInitialSize;
};
struct RAIN_EFF {
  be<u8> mStatus;
  u8 padding[3];
  cXyz mPos, mBasePos;
  be<f32> field_0x1c, field_0x20, field_0x24, mAlpha, field_0x2c, field_0x30;
  be<s16> mTimer;
  u8 tail[2];
};
struct SNOW_EFF {
  be<u8> mStatus;
  u8 padding[3];
  cXyz mPos, mBasePos;
  be<f32> mPosWaveX, mPosWaveZ, mGravity, mWindSpeed, mScale;
  u8 reserved[4];
  be<s16> mTimer;
  u8 tail[2];
};
struct POISON_EFF {
  be<u8> mStatus;
  u8 padding[3];
  cXyz mPos;
  be<f32> field_0x10, field_0x14, field_0x18, field_0x1c, field_0x20, mAlpha,
      mSize;
  be<s16> field_0x2c, field_0x2e;
};
struct HOUSI_EFF {
  be<u8> mStatus;
  u8 padding[3];
  cXyz mPos, mBasePos, mSpeed, mScale;
  be<f32> field_0x34, field_0x38;
  be<u16> field_0x3c;
  u8 reserved[2];
  be<f32> mAlpha, field_0x44, field_0x48;
  be<u16> field_0x4c;
  u8 tail[2];
};
struct dKankyo_cloud_Packet {
  u8 header[0x9c];
  be<s32> mCount;
  CLOUD_EFF mEff[30];
  be<u16> mAnimCounter;
  u8 padding[2];
  be<f32> mRot;
  u8 renderHeader[4];
  Geometry mGeometry[30];
  u8 padUniforms[4];
  Uniform mUniforms[30];
};
struct dKankyo_rain_Packet {
  u8 header[0xa0];
  RAIN_EFF mEff[250];
  u8 countData[8];
  be<s16> mRainCount;
  u8 cameraData[0x3778 - 0x375a];
  cXyz mCenterDelta;
  be<f32> mCenterDeltaMul;
  u8 statusData[0x379d - 0x3788];
  be<u8> mDrawn[250];
  u8 padding[5];
  Geometry mGeometry[1250];
  u8 padUniforms[4];
  Uniform mUniforms[250];
};
struct dKankyo_snow_Packet {
  u8 header[0x9c];
  SNOW_EFF mEff[250];
  cXyz mOldEyePos;
  be<f32> mWave, mSecondary;
  be<s16> mEffCount, mTamariStart;
  u8 state[8];
  Geometry mGeometry[1000], mAshGeometry[4000];
  u8 padding[4];
  Uniform mSnowUniform, mAshUniforms[4000];
};
struct dKankyo_poison_Packet {
  u8 header[0x98];
  POISON_EFF mEff[1000];
  cXyz mBasePos, previousPos;
  be<s32> mCount;
  be<u32> mpTexture;
  be<f32> mRot;
  be<u8> mDrawn[1000];
  u8 padding[4];
  Geometry mGeometry[1000];
  Uniform mUniforms[1000];
};
struct dKankyo_housi_Packet {
  u8 header[0x9c];
  HOUSI_EFF mEffect[300];
  u8 countData[0x5e68 - 0x5e5c];
  be<s16> mCount;
  u8 state[6];
  be<u8> mDrawn[600];
  u8 padding[4];
  Geometry mGeometry[600];
  u8 padUniforms[4];
  Uniform mUniforms[600];
};
struct Environment {
  u8 header[0xa44];
  be<u32> mpRainPacket;
  u8 rainState[4];
  be<u32> mSnowCount, mpSnowPacket;
  u8 snowState[0xa68 - 0xa54];
  be<s32> mPoisonCount;
  be<u32> mpPoisonPacket;
  u8 poisonState[8];
  be<u32> mpHousiPacket;
  be<u8> unknownMoya, mMoyaMode;
  u8 padding[2];
  union {
    be<u32> mMoyaCount;
    struct {
      be<u16> high;
      be<s16> low;
    } moyaCountParts;
  };
  be<u32> mpMoyaPacket;
  u8 colorData[0xb70 - 0xa88];
  Color mBG0_K0;
  u8 backgroundColors[0xb88 - 0xb74];
  Color mBG3_K0;
  u8 vrboxState[0x109e - 0xb8c];
  be<u8> mbVrboxInvisible;
};
struct GameInfo {
  u8 prefix[0x5134];
  be<u8> mStartStageName[0x5af8 - 0x5134];
  be<u32> camera;
  u8 cameraState[0x5b2c - 0x5afc];
  be<u32> player;
  u8 viewState[0x5fa4 - 0x5b30];
  be<u32> view;
};
struct Camera {
  u8 prefix[0xdc];
  cXyz eye;
};
struct Player {
  u8 prefix[0x314];
  cXyz currentPos;
};
struct View {
  u8 prefix[0xd4];
  be<f32> fovy;
  u8 matrices[0x1e4 - 0xd8];
  Matrix rotation;
};
// HD: both ash/snow branches reuse this exact 464-byte guest stack object.
struct AshWork {
  union {
    be<f32> size;
    struct {
      be<u16> reflectionCount;
      be<u8> tint, pad0;
    };
  };
  be<f32> floor, particleSize;
  union {
    be<u32> sineTable;
    be<f32> quarter;
  };
  be<f32> vertexZ2, vertexZ3, uvOne, one, alphaScale, zero, rotationScale,
      heightRange, phaseZero;
  be<u32> copyFlags;
  Color ashColor0, ashColor1;
  u8 padding[8];
  cXyz ashInput, ashOutput;
  Quad ashVertices;
  ColorF ashFloatColor;
  cXyz snowInput, snowOutput;
  Quad snowVertices;
  Color snowColor0, snowColor1;
  Matrix ashMatrix;
  SafeString name, stage;
  ColorF snowFloatColor;
  Matrix ashRotation, snowMatrix, snowRotation;
};
WWHD_SIZE(Matrix, 48);
WWHD_SIZE(ColorF, 16);
WWHD_SIZE(Color, 4);
WWHD_SIZE(Quad, 48);
WWHD_SIZE(Vertex, 20);
WWHD_SIZE(BufferSlot, 0x254);
WWHD_OFFSET(BufferSlot, size, 0x150);
WWHD_SIZE(Geometry, 0x4c0);
WWHD_OFFSET(Geometry, selected, 0x4a8);
WWHD_SIZE(CLOUD_EFF, 76);
WWHD_SIZE(RAIN_EFF, 56);
WWHD_SIZE(SNOW_EFF, 56);
WWHD_SIZE(POISON_EFF, 48);
WWHD_SIZE(HOUSI_EFF, 80);
WWHD_OFFSET(dKankyo_cloud_Packet, mEff, 0xa0);
WWHD_OFFSET(dKankyo_cloud_Packet, mGeometry, 0x994);
WWHD_OFFSET(dKankyo_cloud_Packet, mUniforms, 0x9818);
WWHD_OFFSET(dKankyo_rain_Packet, mRainCount, 0x3758);
WWHD_OFFSET(dKankyo_rain_Packet, mGeometry, 0x389c);
WWHD_OFFSET(dKankyo_rain_Packet, mUniforms, 0x176a20);
WWHD_OFFSET(dKankyo_snow_Packet, mEffCount, 0x3760);
WWHD_OFFSET(dKankyo_snow_Packet, mAshGeometry, 0x12c56c);
WWHD_OFFSET(dKankyo_snow_Packet, mSnowUniform, 0x5cfd70);
WWHD_OFFSET(dKankyo_snow_Packet, mAshUniforms, 0x5d00e0);
WWHD_OFFSET(dKankyo_poison_Packet, mCount, 0xbc30);
WWHD_OFFSET(dKankyo_poison_Packet, mGeometry, 0xc028);
WWHD_OFFSET(dKankyo_poison_Packet, mUniforms, 0x134e28);
WWHD_OFFSET(dKankyo_housi_Packet, mCount, 0x5e68);
WWHD_OFFSET(dKankyo_housi_Packet, mGeometry, 0x60cc);
WWHD_OFFSET(dKankyo_housi_Packet, mUniforms, 0xb82d0);
WWHD_OFFSET(Environment, mMoyaMode, 0xa7d);
WWHD_OFFSET(Environment, mpMoyaPacket, 0xa84);
WWHD_OFFSET(Environment, mBG0_K0, 0xb70);
WWHD_OFFSET(Environment, mbVrboxInvisible, 0x109e);
WWHD_OFFSET(GameInfo, camera, 0x5af8);
WWHD_OFFSET(GameInfo, player, 0x5b2c);
WWHD_OFFSET(GameInfo, view, 0x5fa4);
WWHD_SIZE(AshWork, 464);
WWHD_OFFSET(AshWork, ashMatrix, 0xf0);
WWHD_OFFSET(AshWork, name, 0x120);
WWHD_OFFSET(AshWork, snowRotation, 0x1a0);
enum ConstantAddress : u32 {
  Zero = 0x1004F528,
  AshHeightRange = 0x1004F52C,
  CloudPhasePower = 0x1004F538,
  WindDecay = 0x1004F56C,
  CloudDepth = 0x1004F8C0,
  CloudRotationWrap = 0x1004F8C4,
  CloudPhaseBase = 0x1004F8D8,
  CloudPointStep = 0x1004F588,
  CloudBlend = 0x1004F554,
  CloudWrapScale = 0x1004F8D4,
  CloudPointMinimum = 0x1004F724,
  One = 0x1004F550,
  CloudHalf = 0x1004F57C,
  CloudRadius = 0x1004F8C8,
  CloudAlphaScale = 0x1004F8CC,
  CloudAlphaBias = 0x1004F8D0,
  CloudJitter = 0x1004F590,
  CloudInitialJitter = 0x1004F598,
  CloudCounterMax = 0x1004F59C,
  CloudWanderPhase = 0x1004F6D8,
  CloudInnerRadius = 0x1004F8DC,
  CloudPointThreshold = 0x1004F728,
  CloudPointGain = 0x1004F72C,
  CloudScatter = 0x1004F544,
  CloudSizeBias = 0x1004F8E0,
  CloudSpeedRandomRange = 0x1004F584,
  CloudSpeedBias = 0x1004F8E4,
  CloudVerticalGain = 0x1004F730,
  CloudMode1Speed = 0x1004F5B4,
  CloudNormalSpeed = 0x1004F8E8,
  CloudWrapDistance = 0x1004F8EC,
  FovScale = 0x1004F7F8,
  CloudDistanceScale = 0x1004F8F0,
  CloudSizeDistanceSlope = 0x1004F8F4,
  CloudAlphaDistanceRange = 0x1004F8F8,
  CloudAlphaLimit = 0x1004F8FC,
  CloudOffThreshold = 0x1004F58C,
  CloudMode4Alpha = 0x1004F734,
  RenderRotationScale = 0x1004FB7C,
  DrawAlphaThreshold = 0x1004F5B8,
  AlphaByteScale = 0x1004F5AC,
  HousiSize = 0x1004F800,
  HousiPhaseScale = 0x1004F6B8,
  ReflectionOpacityScale = 0x1004F6EC,
  TexOne = 0x1004F7FC,
  ReflectionHeightRange = 0x1004F6D4,
  HousiDeform = 0x1004FC8C,
  ReflectionHeightMax = 0x1004F558,
  PoisonPhaseScale = 0x1004F7CC,
  PoisonColorCommon = 0x1004FB90,
  PoisonRedBase = 0x1004FC88,
  PoisonGreenBase = 0x1004FC84,
  PoisonBlueSlope = 0x1004FC7C,
  PoisonBlueBase = 0x1004FC80,
  PoisonSecondRedSlope = 0x1004FC74,
  PoisonSecondRedBase = 0x1004FC78,
  PoisonSecondGreenSlope = 0x1004FC6C,
  PoisonSecondGreenBase = 0x1004FC70,
  AshSizeBase = 0x1004FC94,
  AshSettledScale = 0x1004FC98,
  AshCounterPhase = 0x1004F950,
  AshOffsetStep = 0x1004F564,
  AshOffsetNegative = 0x1004FC9C,
  AshYOffset = 0x1004F5C8,
  AshOffsetExtent = 0x1004F5D4,
  OffsetExtent = 0x1004F574,
  RainFallVelocity = 0x1004FC5C,
  RainShift = 0x1004F6DC,
  RainDistanceScale = 0x1004F7D0,
  RainUvDepth = 0x1004FB84,
  RainOffsetStep = 0x1004F624,
  RainOffsetBase = 0x1004F5F0,
  RainCenterBias = 0x1004FC60,
  RainThicknessScale = 0x1004F920,
  ParticleIndexScale = 0x1004F54C,
  RainWidth = 0x1004F7F0,
  RainAlphaScale = 0x1004F808,
  SnowOffsetNegative = 0x1004FCA0
};
enum GuestIdentity : u32 {
  StringVtable = 0x1004f3ac,
  StageDragon = 0x1004f900,
  StageKaze = 0x1004f910,
  StageAdanmae = 0x1004f908,
  AshStageAdanmae = 0x1004fca4
};
inline be<f32> &vectorComponent(cXyz *v, u32 i) {
  return *view<be<f32>>(ea(v) + i * sizeof(f32));
}
inline be<u32> &wordAxis(cXyz *v, u32 i) {
  return *view<be<u32>>(ea(v) + i * sizeof(u32));
}
inline be<u32> &housiRotation() { return *view<be<u32>>(0x101e9a24); }
inline be<u32> &rainRotation() { return *view<be<u32>>(0x101e99f4); }
inline be<f32> &ashRotation() { return *view<be<f32>>(0x101e99f8); }
inline be<f32> &snowRotation() { return *view<be<f32>>(0x101e99fc); }
inline be<u32> &counter() { return *view<be<u32>>(0x101ff558); }
inline be<u32> &ashOffsetsInitialized() { return *view<be<u32>>(0x10477418); }
inline be<u32> &snowOffsetsInitialized() { return *view<be<u32>>(0x10477584); }
inline be<u32> &rainOffsetsInitialized() { return *view<be<u32>>(0x10477580); }
inline cXyz *ashOffsets() { return view<cXyz>(0x104774c0); }
inline cXyz *snowOffsets() { return view<cXyz>(0x10477450); }
inline cXyz *rainOffsets() { return view<cXyz>(0x10477420); }

struct CacheLine {
  be<u32> words[8];
};
inline BufferSlot *bufferSlot(u32 geometry, u32 selected) {
  return view<BufferSlot>(geometry + selected * u32(sizeof(BufferSlot)));
}
enum ParticleStatus : u8 {
  ParticleUninitialized = 0,
  ParticleActive = 1,
  ParticleSettled = 2,
  ParticleExpired = 3
};
enum AshPass : u32 { AshNormal = 0, AshReflection = 1, AshPassCount = 2 };
enum AshGroup : u32 { AshFalling = 0, AshAccumulated = 1, AshGroupCount = 2 };
constexpr u32 SineTableAddress = 0x104A44F8;
namespace sdk {
/* 02555D0C [v d_kankyo_rain] */
template <class R = void, class... A> inline R getEnvironment(A... args) {
  return gabi::call<R>(0x02555D0C, args...);
}
/* 025200D4 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getGameInfo(A... args) {
  return gabi::call<R>(0x025200D4, args...);
}
/* 0257DB28 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getWindVectorPower(A... args) {
  return gabi::call<R>(0x0257DB28, args...);
}
/* 02578348 [v d_kankyo_rain] */
template <class R = void, class... A> inline R getWindPower(A... args) {
  return gabi::call<R>(0x02578348, args...);
}
/* 028E91EC [v d_kankyo_rain] */
template <class R = void, class... A> inline R MTXInverse(A... args) {
  return gabi::call<R>(0x028E91EC, args...);
}
/* 0256401C [v d_kankyo_rain] */
template <class R = void, class... A> inline R setEyeVector(A... args) {
  return gabi::call<R>(0x0256401C, args...);
}
/* 028E8DE8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R distanceSquared(A... args) {
  return gabi::call<R>(0x028E8DE8, args...);
}
/* 028F4384 [v d_kankyo_rain] */
template <class R = void, class... A> inline R squareRoot(A... args) {
  return gabi::call<R>(0x028F4384, args...);
}
/* 020198D8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R randomF(A... args) {
  return gabi::call<R>(0x020198D8, args...);
}
/* 02019918 [v d_kankyo_rain] */
template <class R = void, class... A> inline R randomFX(A... args) {
  return gabi::call<R>(0x02019918, args...);
}
/* 0200F5C8 [v d_kankyo_rain] */
template <class R = void, class... A> inline R chaseF(A... args) {
  return gabi::call<R>(0x0200F5C8, args...);
}
/* 0257E128 [v d_kankyo_rain] */
template <class R = void, class... A>
inline R getPointWindVectorPower(A... args) {
  return gabi::call<R>(0x0257E128, args...);
}
/* 0200ECD4 [v d_kankyo_rain] */
template <class R = void, class... A> inline R addCalc(A... args) {
  return gabi::call<R>(0x0200ECD4, args...);
}
/* 02563F0C [v d_kankyo_rain] */
template <class R = void, class... A> inline R getVector(A... args) {
  return gabi::call<R>(0x02563F0C, args...);
}
/* 0255F84C [v d_kankyo_rain] */
template <class R = void, class... A> inline R reinitializeRenderer(A... args) {
  return gabi::call<R>(0x0255F84C, args...);
}
/* 028E98C0 [v d_kankyo_rain] */
template <class R = void, class... A> inline R MTXRotDeg(A... args) {
  return gabi::call<R>(0x028E98C0, args...);
}
/* 028E9108 [v d_kankyo_rain] */
template <class R = void, class... A> inline R MTXConcat(A... args) {
  return gabi::call<R>(0x028E9108, args...);
}
/* 0257015C [v d_kankyo_rain] */
template <class R = void, class... A> inline R convertColor(A... args) {
  return gabi::call<R>(0x0257015C, args...);
}
/* 027FC500 [v d_kankyo_rain] */
template <class R = void, class... A> inline R setUniformColor(A... args) {
  return gabi::call<R>(0x027FC500, args...);
}
/* 028E8F64 [v d_kankyo_rain] */
template <class R = void, class... A> inline R MTXMultVec(A... args) {
  return gabi::call<R>(0x028E8F64, args...);
}
/* 027B5E94 [v d_kankyo_rain] */
template <class R = void, class... A> inline R flushBuffer(A... args) {
  return gabi::call<R>(0x027B5E94, args...);
}
/* 025820F4 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawCloudPacket(A... args) {
  return gabi::call<R>(0x025820F4, args...);
}
/* 02019510 [v d_kankyo_rain] */
template <class R = void, class... A> inline R floatAngle(A... args) {
  return gabi::call<R>(0x02019510, args...);
}
/* 02581C34 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawHousiPacket(A... args) {
  return gabi::call<R>(0x02581C34, args...);
}
/* 02572F1C [v d_kankyo_rain] */
template <class R = void, class... A> inline R prepareAshColor(A... args) {
  return gabi::call<R>(0x02572F1C, args...);
}
/* 02580B98 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawAshPacket(A... args) {
  return gabi::call<R>(0x02580B98, args...);
}
/* 02580870 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawSnowPacket(A... args) {
  return gabi::call<R>(0x02580870, args...);
}
/* 025817B0 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawPoisonPacket(A... args) {
  return gabi::call<R>(0x025817B0, args...);
}
/* 0201AE48 [v d_kankyo_rain] */
template <class R = void, class... A> inline R scaleVector(A... args) {
  return gabi::call<R>(0x0201AE48, args...);
}
/* 028E8D88 [v d_kankyo_rain] */
template <class R = void, class... A> inline R addVector(A... args) {
  return gabi::call<R>(0x028E8D88, args...);
}
/* 0257F804 [v d_kankyo_rain] */
template <class R = void, class... A> inline R drawRainPacket(A... args) {
  return gabi::call<R>(0x0257F804, args...);
}
} // namespace sdk
inline u32 environment() { return sdk::getEnvironment<u32>(); }
inline u32 gameInfo() { return sdk::getGameInfo<u32>(); }
inline f32 constant(u32 address) { return *view<be<f32>>(address); }
inline f32 sampleSine(u32 angle) {
  return *view<be<f32>>(0x104a44f8 + (angle & 0xfff8));
}
inline f32 sampleCosine(u32 angle) {
  return *view<be<f32>>(0x104a44f8 + (angle & 0xfff8) + 4);
}
inline void safeStringCallback(SafeString *string) {
  gabi::call_ptr(*view<be<u32>>(string->vtable + 0x14), string);
}
inline bool equalTerminatedStrings(u32 a, u32 b) {
  if (a == b)
    return true;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 x = view<be<u8>>(a)[i], y = view<be<u8>>(b)[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
} // namespace RainReadable
