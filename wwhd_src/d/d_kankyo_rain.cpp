/* d_kankyo_rain, WWHD: weather-vector and camera helpers.
 * Full TU inventory and remaining rendering/weather bodies are tracked separately. */
// HD: shader resources, scoped heaps and shifted packet layouts differ from GX weather.
#include "d/d_kankyo_rain.h"
using namespace WeatherHelpers;
#include <bit>
namespace d_kankyo_rain_cpp {

/* 02563E48 */
static void vectle_calc(u32 pos, u32 out) {
  f64 y = (f64)view<DOUBLE_POS>(pos)->y, x = (f64)view<DOUBLE_POS>(pos)->x,
      z = (f64)view<DOUBLE_POS>(pos)->z;
  f64 sum = gabi::fmadd(z, z, gabi::fmadd(x, x, y * y));
  f64 magnitude = sdk::squareRoot<f64>((f64)(f32)sum);
  if (std::fabs((f32)magnitude) < (f32)constant(VectorEpsilon)) {
    f32 zero = (f32)constant(Zero);
    view<cXyz>(out)->y = zero;
    view<cXyz>(out)->x = zero;
    view<cXyz>(out)->z = zero;
  } else {
    f64 reciprocal = (f64)doubleConstant(DoubleOne) / (f64)magnitude;
    view<cXyz>(out)->x = (f32)((f64)view<DOUBLE_POS>(pos)->x * reciprocal);
    view<cXyz>(out)->y = (f32)((f64)view<DOUBLE_POS>(pos)->y * reciprocal);
    view<cXyz>(out)->z = (f32)((f64)view<DOUBLE_POS>(pos)->z * reciprocal);
  }
}

static f32 subtract_fpr(f32 a, f32 b) {
  volatile f64 left = (f64)a, right = (f64)b;
  return (f32)(left - right);
}
/* 02563F0C */
static void get_vectle_calc(u32 eye, u32 center, u32 out) {
  gabi::Local<DOUBLE_POS> delta;
  f32 x = subtract_fpr((f32)view<cXyz>(center)->x, (f32)view<cXyz>(eye)->x),
      y = subtract_fpr((f32)view<cXyz>(center)->y, (f32)view<cXyz>(eye)->y);
  f32 z = subtract_fpr((f32)view<cXyz>(center)->z, (f32)view<cXyz>(eye)->z);
  delta->x = (f64)x;
  delta->y = (f64)y;
  delta->z = (f64)z;
  sdk::normalizeDoubleVector<void>(gabi::ea(delta.get()), out);
}

/* 02563F64 */
static void dKyr_get_vectle_calc(u32 eye, u32 center, u32 out) {
  sdk::directionVector<void>(eye, center, out);
}

/* 02563F68 */
static void dKy_set_eyevect_calc(u32 camera, u32 out, f32 distance,
                                 f32 height) {
  gabi::Local<cXyz> direction;
  sdk::directionVector<void>(gabi::ea(&view<Camera>(camera)->mEye),
                             gabi::ea(&view<Camera>(camera)->mCenter),
                             gabi::ea(direction.get()));
  view<cXyz>(out)->x = gabi::fmadds((f32)direction->x, distance,
                                    (f32)view<Camera>(camera)->mEye.x);
  f32 y = gabi::fmadds((f32)direction->y, height,
                       (f32)view<Camera>(camera)->mEye.y);
  view<cXyz>(out)->y = y - (f32)constant(EyeHeightOffset);
  view<cXyz>(out)->z = gabi::fmadds((f32)direction->z, distance,
                                    (f32)view<Camera>(camera)->mEye.z);
}

/* 0256401C */
static void dKy_set_eyevect_calc2(u32 camera, u32 out, f32 distance,
                                  f32 height) {
  gabi::Local<cXyz> direction;
  gabi::Local<DOUBLE_POS> delta;
  f32 zero = (f32)constant(Zero);
  f32 x =
      (f32)view<Camera>(camera)->mCenter.x - (f32)view<Camera>(camera)->mEye.x;
  f32 centerZ = (f32)view<Camera>(camera)->mCenter.z;
  delta->x = (f64)x;
  if (height == zero) {
    delta->y = (f64)doubleConstant(DoubleZero);
    delta->z = (f64)(centerZ - (f32)view<Camera>(camera)->mEye.z);
  } else {
    delta->y = (f64)((f32)view<Camera>(camera)->mCenter.y -
                     (f32)view<Camera>(camera)->mEye.y);
    delta->z = (f64)(centerZ - (f32)view<Camera>(camera)->mEye.z);
  }
  sdk::normalizeDoubleVector<void>(gabi::ea(delta.get()),
                                   gabi::ea(direction.get()));
  view<cXyz>(out)->x = gabi::fmadds((f32)direction->x, distance,
                                    (f32)view<Camera>(camera)->mEye.x);
  view<cXyz>(out)->y = gabi::fmadds((f32)direction->y, height,
                                    (f32)view<Camera>(camera)->mEye.y);
  view<cXyz>(out)->z = gabi::fmadds((f32)direction->z, distance,
                                    (f32)view<Camera>(camera)->mEye.z);
  if (height == zero)
    view<cXyz>(out)->y = zero;
}

static inline u32 environment() { return sdk::getEnvironment<u32>(); }
static inline u32 gameInfo() { return sdk::getGameInfo<u32>(); }
static inline u32 windPacket() {
  return (u32)view<EnvironmentPackets>(environment())->mpWind;
}
/* 02564184 */
static void dKyr_wind_init() {
  u32 camera = (u32)view<GameInfo>(gameInfo())->camera;
  view<WINDEFF_SET>(windPacket())->mbHasCustomWindPower = 0;
  view<WINDEFF_SET>(windPacket())->field_0x759 = 0;
  u32 packet = windPacket();
  f32 randomScale = (f32)constant(WindTimerRange), zero = (f32)constant(Zero);
  wordComponent(&view<WINDEFF_SET>(packet)->mOldEyePos, 0) =
      (u32)wordComponent(&view<Camera>(camera)->mEye, 0);
  wordComponent(&view<WINDEFF_SET>(packet)->mOldEyePos, 1) =
      (u32)wordComponent(&view<Camera>(camera)->mEye, 1);
  wordComponent(&view<WINDEFF_SET>(packet)->mOldEyePos, 2) =
      (u32)wordComponent(&view<Camera>(camera)->mEye, 2);
  for (u32 offset = 0; offset < 0x40; offset += 0x20) {
    view<WINDEFF_SET>(windPacket())
        ->mKamomeEff[offset / sizeof(KAMOME_EFF)]
        .mStatus = 0;
    view<WINDEFF_SET>(windPacket())
        ->mKamomeEff[offset / sizeof(KAMOME_EFF)]
        .mScale = zero;
    u32 particle = gabi::ea(&view<WINDEFF_SET>(windPacket())
                                 ->mKamomeEff[offset / sizeof(KAMOME_EFF)]);
    f32 angle = sdk::randomF<f32>(randomScale);
    view<KAMOME_EFF>(particle)->mTimer = (u16)gabi::ftoi(angle);
    view<WINDEFF_SET>(windPacket())
        ->mKamomeEff[offset / sizeof(KAMOME_EFF)]
        .mpEmitter = 0;
  }
}
/* 02565DAC */
static u32 dKyr_moon_arrival_check() {
  if ((f32)view<Environment>(environment())->mCurTime >
      (f32)constant(MoonEvening))
    return 1;
  return (f32)view<Environment>(environment())->mCurTime <
         (f32)constant(MoonMorning);
}
/* 0256691C */
static void rain_bg_chk(u32 packet, s32 index) {
  u32 camera = (u32)view<GameInfo>(gameInfo())->camera;
  view<dKankyo_rain_Packet>(packet)->mEff[index].field_0x30 = gabi::fadds_ppc(
      (f32)view<Camera>(camera)->mCenter.y, (f32)constant(RainFloorOffset));
}
/* 0256C8BC */
static u32 dKyr_poison_live_check() {
  return (u32)view<EnvironmentPackets>(environment())->mPoisonCount != 0;
}
/* 02577110 */
static void dKyr_thunder_init() {
  view<EnvironmentPackets>(environment())->thunderState = 0;
}

// 0x02565B70 dKyr_lenzflare_move: body lives in d_kankyo_rain_movement_qa.cpp (identical code).

static inline u32 rainPacket() {
  return (u32)view<EnvironmentPackets>(environment())->mpRainPacket;
}
/* 0256670C */
static void dKyr_rain_init() {
  u32 camera = (u32)view<GameInfo>(gameInfo())->camera;
  gabi::Local<SafeString> archiveName, resourceName;
  archiveName->text = RainArchiveName;
  archiveName->vtable = SafeStringVtable;
  resourceName->text = RainResourceName;
  resourceName->vtable = SafeStringVtable;
  u32 resource = sdk::loadArchiveResource<u32>(
      (u32)resourceManager(), archiveName.get(), resourceName.get(), 0);
  u32 shader = sdk::getShader<u32>(resource);
  u32 relative = (u32)view<Shader>(shader)->uniformRelativeOffset,
      array = relative
                  ? gabi::ea(&view<Shader>(shader)->uniformRelativeOffset) +
                        relative
                  : 0;
  u32 uniform = sdk::findUniform<u32>(array, RainSnowUniform);
  view<dKankyo_rain_Packet>(rainPacket())->mpTxSnow01 = uniform;
  relative = (u32)view<Shader>(shader)->uniformRelativeOffset;
  array = relative ? gabi::ea(&view<Shader>(shader)->uniformRelativeOffset) +
                         relative
                   : 0;
  uniform = sdk::findUniform<u32>(array, RainRingUniform);
  view<dKankyo_rain_Packet>(rainPacket())->mpTxRingAHalf = uniform;
  sdk::initializeRainRenderer<void>(rainPacket());
  u32 packet = rainPacket();
  for (u32 component = 0; component < 12; component += 4)
    wordComponent(&view<dKankyo_rain_Packet>(packet)->mCamEyePos,
                  component / 4) =
        (u32)wordComponent(&view<Camera>(camera)->mEye, component / 4);
  packet = rainPacket();
  for (u32 component = 0; component < 12; component += 4)
    wordComponent(&view<dKankyo_rain_Packet>(packet)->mCamCenterPos,
                  component / 4) =
        (u32)wordComponent(&view<Camera>(camera)->mCenter, component / 4);
  f32 zero = (f32)constant(Zero);
  for (u32 resetIndex = 0; resetIndex < 6; resetIndex++)
    view<dKankyo_rain_Packet>(rainPacket())->resetFields[resetIndex] = zero;
  view<dKankyo_rain_Packet>(rainPacket())->mStatus = 0;
  for (u32 componentIndex = 0; componentIndex < 3; componentIndex++)
    component(&view<dKankyo_rain_Packet>(rainPacket())->mCenterDelta,
              componentIndex) = zero;
  for (u32 i = 0; i < 250; i++)
    view<dKankyo_rain_Packet>(rainPacket())->mEff[i].mStatus = 0;
  view<dKankyo_rain_Packet>(rainPacket())->mRainCount = 0;
}
/* 0256A25C */
static void dKyr_star_init() {
  // The native scoped heap guard constructor allocates four bytes when needed.
  gabi::Local<be<u32>> heapGuard;
  sdk::beginScopedHeap<void>(heapGuard.get(), StarHeapName, StarHeapBytes, 0);
  u32 packet = sdk::allocate<u32>(StarPacketBytes, 32);
  if (packet)
    packet = sdk::constructStarPacket<u32>(packet);
  view<EnvironmentPackets>(environment())->mpStarPacket = packet;
  if ((u32)view<EnvironmentPackets>(environment())->mpStarPacket) {
    gabi::Local<SafeString> name;
    name->text = AlwaysArchiveName;
    name->vtable = SafeStringVtable;
    u32 resource = sdk::getObjectResource<u32>((u32)objectResourceManager(),
                                               name.get(), 0x81);
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mpTexture = resource;
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mSin = (f32)constant(One);
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mAnimCounter = 0;
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mEffectNum = 0;
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mFrameCounter = 0;
  } else {
    sdk::cancelScopedHeap<void>(heapGuard.get());
    view<dKankyo_star_Packet>(
        (u32)view<EnvironmentPackets>(environment())->mpStarPacket)
        ->mFrameCounter = 0;
  }
  sdk::endScopedHeap<void>(heapGuard.get(), 2);
}
/* 0256EF7C */
static void dKy_wave_chan_init() {
  view<Environment>(environment())->mWaveChan.mWaveCount = 0;
  u32 env = environment();
  f32 zero = (f32)constant(Zero);
  view<Environment>(env)->mWaveChan.field_0x0 = (f32)constant(WaveInitial);
  view<Environment>(environment())->mWaveChan.field_0x4 = zero;
  view<Environment>(environment())->mWaveChan.field_0x8 = zero;
  view<Environment>(environment())->mWaveChan.mWaveSpeed =
      (f32)constant(WaveFirstSpeed);
  view<Environment>(environment())->mWaveChan.mWaveSpawnDist =
      (f32)constant(WaveSpawnDistance);
  view<Environment>(environment())->mWaveChan.mWaveSpawnRadius =
      (f32)constant(WaveSpawnRadius);
  view<Environment>(environment())->mWaveChan.mWaveReset = 0;
  view<Environment>(environment())->mWaveChan.mWaveScale =
      (f32)constant(WaveScale);
  view<Environment>(environment())->mWaveChan.mWaveScaleBottom =
      (f32)constant(WaveBottomScale);
  view<Environment>(environment())->mWaveChan.mWaveScaleRand =
      (f32)constant(WaveRandomScale);
  view<Environment>(environment())->mWaveChan.mWaveCounterSpeedScale =
      (f32)constant(WaveCounterSpeed);
  view<Environment>(environment())->mWaveChan.field_0x2f = 0;
  view<Environment>(environment())->mWaveChan.mWaveSpeed =
      (f32)constant(WaveSpeed);
  view<Environment>(environment())->mWaveChan.mWaveFlatInter = zero;
}

/* 0257015C */
static void color_to_float(u32 out, u32 color) {
  f32 divisor = (f32)constant(ColorScale);
  f32 red = (f32)(u8)view<Color>(color)->channels[0] / divisor;
  f32 green = (f32)(u8)view<Color>(color)->channels[1] / divisor;
  f32 blue = (f32)(u8)view<Color>(color)->channels[2] / divisor;
  f32 alpha = (f32)(u8)view<Color>(color)->channels[3] / divisor;
  view<ColorF>(out)->channels[0] = red;
  view<ColorF>(out)->channels[1] = green;
  view<ColorF>(out)->channels[2] = blue;
  view<ColorF>(out)->channels[3] = alpha;
}
/* 02572E9C */
static f64 color_gamma_component(u32 color) {
  f32 normalized = (f32)color / (f32)constant(ColorScale);
  f64 result =
      sdk::power<f64>((f64)normalized, (f64)(f32)constant(GammaExponent));
  f32 zero = (f32)constant(Zero);
  if (result < zero)
    return zero;
  f32 one = (f32)constant(One);
  if (result > one)
    return one;
  return result;
}
/* 02572F1C */
static void color_gamma(u32 color) {
  f32 scale = (f32)constant(ColorScale);
  u32 component = (u8)view<Color>(color)->channels[0];
  for (u32 i = 0; i < 3; i++) {
    f64 converted = sdk::gammaComponent<f64>(component);
    f32 scaled = (f32)(converted * (f64)scale);
    s32 value = gabi::ftoi(scaled);
    // GHS loads the next byte before storing the converted current component.
    if (i < 2)
      component = (u8)view<Color>(color)->channels[i + 1];
    view<Color>(color)->channels[i] = (u8)value;
  }
}
/* 02575ACC */
static void matrix_float_copy(u32 out, u32 source) {
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = (f32)view<Matrix>(source)->cells[i];
  for (u32 i = 0; i < 12; i++)
    view<Matrix>(out)->cells[i] = matrix[i];
}
/* 0256C8FC */
static void dKyr_poison_light_colision() {
  u32 packet = (u32)view<EnvironmentPackets>(environment())->mpPoisonPacket;
  if (!sdk::poisonLiveCheck<u32>())
    return;
  u32 game = gameInfo();
  u32 manager = gabi::ea(&view<GameInfo>(game)->collision);
  sdk::setMassRadius<void>(gabi::ea(&view<CollisionSpace>(manager)->mass),
                           (f32)constant(PoisonMassRadius));
  sdk::setMassHeight<void>(gabi::ea(&view<CollisionSpace>(manager)->mass),
                           (f32)constant(PoisonMassHeight));
  view<GameInfo>(game)->collision.mass.targetType = 11;
  view<GameInfo>(game)->collision.mass.targetMask = 3;
  s32 count = (s32)(u32)view<EnvironmentPackets>(environment())->mPoisonCount;
  if (count <= 0)
    return;
  f32 verticalOffset = (f32)constant(PoisonVerticalOffset);
  gabi::Local<cXyz> summed, pos;
  gabi::Local<be<u32>> mask;
  gabi::Local<MassCollisionResult> result;
  for (s32 i = 0; i < count; i++) {
    u32 particle = gabi::ea(&view<dKankyo_poison_Packet>(packet)->mEff[i]);
    sdk::addVector<void>(
        gabi::ea(&view<dKankyo_poison_Packet>(packet)->mBasePos), summed.get(),
        gabi::ea(&view<POISON_EFF>(particle)->mPos));
    pos->x = (f32)summed->x;
    pos->y = gabi::fsubs_ppc((f32)summed->y, verticalOffset);
    pos->z = (f32)summed->z;
    sdk::constructMassResult<void>(result.get());
    u32 hit =
        sdk::massCheck<u32>(gabi::ea(&view<GameInfo>(gameInfo())->collision),
                            pos.get(), mask.get(), result.get());
    u32 object = (u32)result->object;
    if ((hit & 1) && object &&
        ((u32)view<CollisionObject>(object)->attackType & 0x800000) &&
        (u8)view<POISON_EFF>(particle)->mStatus == PoisonActive) {
      view<POISON_EFF>(particle)->field_0x2e = 60;
      view<POISON_EFF>(particle)->mStatus = PoisonHit;
    }
    count = (s32)(u32)view<EnvironmentPackets>(environment())->mPoisonCount;
  }
}

/* 02577664 */
static void rain_static_initializer() {
  initializerWord(2) = 0;
  initializerWord(0) = 0;
  initializerWord(3) = 0;
  initializerWord(1) = 0;
  sdk::constructVector<void>(InitializerVector0);
  initialScale(0) = (f32)constant(InitialScaleX);
  initialScale(1) = (f32)constant(InitialScaleY);
  sdk::initializeByteState<void>(InitializerByte0);
  sdk::constructVector<void>(InitializerVector1);
  sdk::initializeSecondByteState<void>(InitializerByte1);
  sdk::constructVector<void>(InitializerVector2);
}

static inline u32 poisonPacket() {
  return (u32)view<EnvironmentPackets>(environment())->mpPoisonPacket;
}
/* 0256DC04 */
static void poison_init() {
  gabi::Local<be<u32>> heapGuard;
  sdk::beginScopedHeap<void>(heapGuard.get(), PoisonHeapName, PoisonHeapBytes,
                             0);
  u32 packet = sdk::allocate<u32>(PoisonPacketBytes, 32);
  if (packet)
    packet = sdk::constructPoisonPacket<u32>(packet);
  view<EnvironmentPackets>(environment())->mpPoisonPacket = packet;
  if (!poisonPacket()) {
    sdk::cancelScopedHeap<void>(heapGuard.get());
    sdk::endScopedHeap<void>(heapGuard.get(), 2);
    return;
  }
  f32 zero = (f32)constant(Zero);
  view<dKankyo_poison_Packet>(poisonPacket())->field_0xbb9c.x = zero;
  view<dKankyo_poison_Packet>(poisonPacket())->field_0xbb9c.y = zero;
  view<dKankyo_poison_Packet>(poisonPacket())->field_0xbb9c.z = zero;
  view<dKankyo_poison_Packet>(poisonPacket())->mCount = 0;
  packet = poisonPacket();
  view<dKankyo_poison_Packet>(packet)->mRot = zero;
  gabi::Local<SafeString> archiveName, resourceName;
  archiveName->text = PoisonArchiveName;
  archiveName->vtable = SafeStringVtable;
  resourceName->text = PoisonResourceName;
  resourceName->vtable = SafeStringVtable;
  u32 resource = sdk::loadArchiveResource<u32>(
      (u32)resourceManager(), archiveName.get(), resourceName.get(), 0);
  u32 shader = sdk::getShader<u32>(resource);
  u32 relative = (u32)view<Shader>(shader)->uniformRelativeOffset;
  u32 uniform = sdk::findUniform<u32>(
      relative
          ? gabi::ea(&view<Shader>(shader)->uniformRelativeOffset) + relative
          : 0,
      PoisonUniformName);
  view<dKankyo_poison_Packet>(poisonPacket())->mpTexture = uniform;
  sdk::initializePoisonRenderer<void>(poisonPacket());
  for (u32 i = 0; i < 1000; i++)
    view<dKankyo_poison_Packet>(poisonPacket())->mEff[i].mStatus = 0;
  sdk::movePoison<void>();
  now_room() = (s8)(u8)stayRoom();
  sdk::endScopedHeap<void>(heapGuard.get(), 2);
}

/* 0256F084 */
static void snap_sunmoon_proc(u32 pos, s32 type) {
  gabi::Local<dSnap_Obj> entry;
  sdk::constructSnapObject<void>(entry.get());
  u32 camera = (u32)view<GameInfo>(gameInfo())->camera;
  if (!((u32)view<GameInfo>(gameInfo())->playerStatus & 8))
    return;
  gabi::Local<cXyz> scaledPos;
  f32 scale = (f32)constant(SnapScale);
  f32 x = gabi::fsubs_ppc((f32)view<cXyz>(pos)->x,
                          (f32)view<Camera>(camera)->mEye.x);
  f32 y = gabi::fsubs_ppc((f32)view<cXyz>(pos)->y,
                          (f32)view<Camera>(camera)->mEye.y);
  f32 z = gabi::fsubs_ppc((f32)view<cXyz>(pos)->z,
                          (f32)view<Camera>(camera)->mEye.z);
  x = gabi::fmuls_ppc(x, scale);
  y = gabi::fmuls_ppc(y, scale);
  z = gabi::fmuls_ppc(z, scale);
  scaledPos->x = gabi::fadds_ppc(x, (f32)view<Camera>(camera)->mEye.x);
  scaledPos->y = gabi::fadds_ppc(y, (f32)view<Camera>(camera)->mEye.y);
  scaledPos->z = gabi::fadds_ppc(z, (f32)view<Camera>(camera)->mEye.z);
  sdk::setSnapSphere<void>(entry.get(), scaledPos.get(),
                           (f32)constant(SnapRadius));
  u32 entryType = type == DSNAP_TYPE_UNK09 ? DSNAP_TYPE_UNK09
                  : type == 0              ? DSNAP_TYPE_UNK07
                                           : DSNAP_TYPE_UNK08;
  sdk::setSnapInfo<void>(entry.get(), entryType, 0, 255, 4, 0x7FFF);
  sdk::registerSnapObject<void>(entry.get());
}

/* 0256A388 */
static void dKyr_star_move() {
  environment();
  u32 packet = (u32)view<EnvironmentPackets>(environment())->mpStarPacket;
  gameInfo();
  gameInfo();
  s16 count = (s16)view<EnvironmentPackets>(environment())->mStarCount;
  view<dKankyo_star_Packet>(packet)->mEffectNum = (u16)count;
  if (!count)
    return;
  u32 counter = (u32)view<dKankyo_star_Packet>(packet)->mFrameCounter + 1;
  u32 angle = (u16)view<dKankyo_star_Packet>(packet)->mAnimCounter;
  if (counter > 719)
    counter = 0;
  view<dKankyo_star_Packet>(packet)->mFrameCounter = counter;
  f32 sine = std::fabs((f32)sampleSine(angle));
  view<dKankyo_star_Packet>(packet)->mSin = gabi::fmadds(
      (f32)constant(StarWaveScale), sine, (f32)constant(StarWaveBase));
  f32 random = sdk::randomFX<f32>((f32)constant(StarRandomPhase));
  s32 oldAngle = (s16)view<dKankyo_star_Packet>(packet)->mAnimCounter;
  view<dKankyo_star_Packet>(packet)->mAnimCounter =
      (u16)((u32)oldAngle + (u32)gabi::ftoi(random) + 200);
}

static f32 forward_fmadds(f32 a, f32 c, f32 b) {
  // Native FMA adds the camera operand after the product; retain its NaN payload.
  if (b != b)
    return gabi::ppc_qnan(b);
  if (a != a)
    return gabi::ppc_qnan(a);
  if (c != c)
    return gabi::ppc_qnan(c);
  return gabi::fmadds(a, c, b);
}
/* 02566974 */
static u32 forward_overhead_bg_chk(u32 out, f32 distance) {
  u32 camera = (u32)view<GameInfo>(gameInfo())->camera;
  gameInfo();
  gabi::Local<dBgS_ObjGndChk_All> groundQuery;
  gabi::Local<dBgS_RoofChk> roofQuery;
  gabi::Local<cXyz> direction;
  sdk::constructGroundQuery<void>(groundQuery.get());
  groundQuery->mPolyPassChk = gabi::ea(&groundQuery->poly);
  groundQuery->mGrpPassChk = gabi::ea(&groundQuery->group);
  groundQuery->poly.mbCamThrough = 0;
  groundQuery->poly.mbLinkThrough = 0;
  groundQuery->group.vtable = GroundGroupVtable;
  groundQuery->poly.mbRopeThrough = 0;
  groundQuery->poly.mbBoomerangThrough = 0;
  groundQuery->vtable = DerivedGroundVtable;
  groundQuery->group.mGrp = FULL_GRP;
  groundQuery->polyVtable = DerivedGroundPolyVtable;
  groundQuery->poly.mbArrowThrough = 0;
  groundQuery->poly.mbBombThrough = 0;
  groundQuery->poly.mbObjThrough = 1;
  groundQuery->poly.vtable = GroundPassVtable;
  sdk::constructRoofQuery<void>(roofQuery.get());
  sdk::weatherDirectionVector<void>(gabi::ea(&view<Camera>(camera)->mEye),
                                    gabi::ea(&view<Camera>(camera)->mCenter),
                                    direction.get());
  f32 y = gabi::fadds_ppc((f32)view<Camera>(camera)->mEye.y,
                          (f32)constant(OverheadOffset));
  f32 x = forward_fmadds(distance, (f32)direction->x,
                         (f32)view<Camera>(camera)->mEye.x);
  f32 z = forward_fmadds(distance, (f32)direction->z,
                         (f32)view<Camera>(camera)->mEye.z);
  view<cXyz>(out)->y = y;
  view<cXyz>(out)->x = x;
  view<cXyz>(out)->z = z;
  roofQuery->m_pos.y = y;
  roofQuery->m_pos.z = z;
  roofQuery->m_pos.x = x;
  f32 roofHeight = sdk::roofCheck<f32>(
      gabi::ea(view<GameInfo>(gameInfo())->background), roofQuery.get());
  u32 result = roofHeight != (f32)constant(InvalidRoofHeight);
  groundQuery->m_pos.x = x;
  groundQuery->m_pos.z = z;
  groundQuery->m_pos.y = gabi::fadds_ppc(y, (f32)constant(GroundQueryOffset));
  f32 floorHeight = sdk::groundCross<f32>(
      gabi::ea(view<GameInfo>(gameInfo())->background), groundQuery.get());
  if (floorHeight > gabi::fadds_ppc((f32)view<Camera>(camera)->mEye.y,
                                    (f32)constant(OverheadOffset)))
    result = 1;
  roofQuery->vtable = BaseGroupVtable;
  roofQuery->polyVtable = RoofPolyVtable;
  roofQuery->groupVtable = RoofGroupVtable;
  sdk::destroyBackgroundQuery<void>(gabi::ea(roofQuery->queryBase), 0);
  groundQuery->poly.vtable = BasePassVtable;
  groundQuery->group.vtable = BaseGroupVtable;
  groundQuery->polyVtable = BaseGroundPolyVtable;
  sdk::destroyGroundQuery<void>(groundQuery.get(), 0);
  return result;
}

static bool equal_terminated_strings(u32 left, u32 right) {
  if (left == right)
    return true;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 a = (u8)view<be<u8>>(left)[i], b = (u8)view<be<u8>>(right)[i];
    if (a != b)
      return false;
    if (!a)
      return true;
  }
  return false;
}
static void safe_string_callback(SafeString *string) {
  gabi::call_ptr((u32)view<SafeStringMethods>(string->vtable)->cstrCallback,
                 string);
}
static inline u32 snowPacket() {
  return (u32)view<EnvironmentPackets>(environment())->mpSnowPacket;
}
// 0x025688C4 dKyr_snow_init: body lives in d_kankyo_rain_movement_qa.cpp (identical code).

static void thunder_color(u32 target, u32 thunder, u32 scale, u32 red = 90,
                          u32 green = 160, u32 blue = 245) {
  gabi::call(target, red, green, blue,
             gabi::fmuls_ppc((f32)view<Thunder>(thunder)->mFlashTimer,
                             constant(scale)));
}
// 0x02577138 dKyr_thunder_move: body lives in d_kankyo_rain_movement_qa.cpp (identical code).

static f32 wave_multiply_double(f32 a, f64 c) {
  u64 bits = std::bit_cast<u64>(c);
  bits = (bits & 0xFFFFFFFFF8000000ull) + (bits & 0x08000000ull);
  return (f32)((f64)a * std::bit_cast<f64>(bits));
}

} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
