/* Weather particles.
 * HD: shader uniforms and double-buffered geometry replace immediate GX rendering.
 */
// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace d_kankyo_rain_cpp {
/* 0256BB6C */
static void cloud_shadow_move() {
  u32 packet = (u32)view<Environment>(environment())->mpMoyaPacket,
      camera = (u32)view<GameInfo>(gameInfo())->camera;
  gabi::Local<cXyz> wind, center, position, pointWind, outputWind;
  gabi::Local<Matrix> inverse;
  sdk::getWindVectorPower<void>(wind.get());
  gameInfo();
  f32 power = sdk::getWindPower<f32>();
  if ((u32)view<GameInfo>(gameInfo())->view == 0)
    return;
  sdk::MTXInverse<void>(
      gabi::ea(&view<View>(view<GameInfo>(gameInfo())->view)->rotation),
      inverse.get());
  if ((s32)(u32)view<dKankyo_cloud_Packet>(packet)->mCount >
      (s32)(u32)view<Environment>(environment())->mMoyaCount) {
    if ((u32)view<dKankyo_cloud_Packet>(packet)->mCount == 0)
      return;
  } else {
    s16 wanted = (s16)view<Environment>(environment())->moyaCountParts.low;
    view<dKankyo_cloud_Packet>(packet)->mCount = (s32)wanted;
    if (wanted == 0)
      return;
  }
  f32 depth = (f32)constant(CloudDepth);
  sdk::setEyeVector<void>(camera, center.get(), depth, depth);
  u16 globalPhase = (u16)view<dKankyo_cloud_Packet>(packet)->mAnimCounter;
  f32 decay =
      gabi::fmadds((f32)constant(WindDecay), power, (f32)constant(WindDecay));
  f32 counter =
          gabi::fsubs_ppc((f32)view<dKankyo_cloud_Packet>(packet)->mRot, decay),
      zero = (f32)constant(Zero);
  f32 globalSine = sampleSine(globalPhase);
  view<dKankyo_cloud_Packet>(packet)->mAnimCounter = globalPhase + 0x104;
  if (counter < zero)
    counter = (f32)constant(CloudRotationWrap);
  view<dKankyo_cloud_Packet>(packet)->mRot = counter;
  if ((s32)(u32)view<dKankyo_cloud_Packet>(packet)->mCount <= 0)
    return;
  f32 countStep = gabi::fmadds((f32)constant(CloudPhasePower), power,
                               (f32)constant(CloudPhaseBase));
  u16 phaseStep = (u16)gabi::ftoi(countStep);
  const f32 small = (f32)constant(CloudPointStep),
            blend = (f32)constant(CloudBlend),
            wrapMul = (f32)constant(CloudWrapScale),
            pointMin = (f32)constant(CloudPointMinimum),
            one = (f32)constant(One), half = (f32)constant(CloudHalf),
            radius = (f32)constant(CloudRadius);
  f32 alphaScale = gabi::fmadds(gabi::fadds_ppc(globalSine, one),
                                (f32)constant(CloudAlphaScale),
                                (f32)constant(CloudAlphaBias));
  const f32 jitter = (f32)constant(CloudJitter),
            initRadius = (f32)constant(CloudInitialJitter),
            phaseMax = (f32)constant(CloudWanderPhase),
            innerRadius = (f32)constant(CloudInnerRadius),
            pointThreshold = (f32)constant(CloudPointThreshold),
            pointGain = (f32)constant(CloudPointGain),
            scatter = (f32)constant(CloudScatter);
  auto stageEquals = [&](u32 literal, bool ignore = false) {
    gabi::Local<SafeString> name, current;
    name->text = literal;
    name->vtable = StringVtable;
    current->text = gameInfo() + 0x5134;
    current->vtable = StringVtable;
    safeStringCallback(name.get());
    safeStringCallback(name.get());
    u32 left = name->text;
    safeStringCallback(current.get());
    u32 right = current->text;
    bool equal = left == right || equalTerminatedStrings(left, right);
    (void)ignore;
    return equal;
  };
  auto sumPosition = [&](u32 p) {
    position->x = gabi::fadds_ppc((f32)view<CLOUD_EFF>(p)->mBasePos.x,
                                  (f32)view<CLOUD_EFF>(p)->mPos.x);
    position->y = gabi::fadds_ppc((f32)view<CLOUD_EFF>(p)->mBasePos.y,
                                  (f32)view<CLOUD_EFF>(p)->mPos.y);
    position->z = gabi::fadds_ppc((f32)view<CLOUD_EFF>(p)->mBasePos.z,
                                  (f32)view<CLOUD_EFF>(p)->mPos.z);
  };
  auto copyCenter = [&](u32 p) {
    for (u32 off = 0; off < 12; off += 4)
      wordAxis(&view<CLOUD_EFF>(p)->mBasePos, off / 4) =
          (u32)wordAxis(center.get(), off / 4);
  };
  auto distance = [&](u32 a, u32 b) {
    sdk::distanceSquared<void>(a, b);
    return sdk::squareRoot<f32>();
  };
  for (s32 i = 0; i < (s32)(u32)view<dKankyo_cloud_Packet>(packet)->mCount;
       ++i) {
    u32 p = gabi::ea(&view<dKankyo_cloud_Packet>(packet)->mEff[i]);
    u8 state = (u8)view<CLOUD_EFF>(p)->mStatus;
    if (state == ParticleUninitialized) {
      f32 size = gabi::fadds_ppc(sdk::randomF<f32>(initRadius),
                                 (f32)constant(CloudSizeBias));
      view<CLOUD_EFF>(p)->mInitialSize = size;
      view<CLOUD_EFF>(p)->mSize = size;
      copyCenter(p);
      view<CLOUD_EFF>(p)->mPos.x = sdk::randomFX<f32>(radius);
      view<CLOUD_EFF>(p)->mPos.y = sdk::randomFX<f32>(radius);
      view<CLOUD_EFF>(p)->mPos.z = sdk::randomFX<f32>(radius);
      f32 speed = gabi::fadds_ppc(
          sdk::randomF<f32>((f32)constant(CloudSpeedRandomRange)),
          (f32)constant(CloudSpeedBias));
      view<CLOUD_EFF>(p)->mWindSpeed = speed;
      view<CLOUD_EFF>(p)->mAlpha = zero;
      view<CLOUD_EFF>(p)->mAnimCounter =
          (u16)gabi::ftoi(sdk::randomF<f32>((f32)constant(CloudCounterMax)));
      view<CLOUD_EFF>(p)->mVelRndm.x = sdk::randomF<f32>(phaseMax);
      view<CLOUD_EFF>(p)->mVelRndm.y = sdk::randomF<f32>(phaseMax);
      view<CLOUD_EFF>(p)->mVelRndm.z = sdk::randomF<f32>(phaseMax);
      view<CLOUD_EFF>(p)->mStatus = (u8)view<CLOUD_EFF>(p)->mStatus + 1;
      view<CLOUD_EFF>(p)->mPntWindVel.x = zero;
      view<CLOUD_EFF>(p)->mPntWindVel.y = zero;
      view<CLOUD_EFF>(p)->mPntWindVel.z = zero;
    }
    if (state <= ParticleSettled) {
      sumPosition(p);
      view<CLOUD_EFF>(p)->mAnimCounter =
          (u16)view<CLOUD_EFF>(p)->mAnimCounter + phaseStep;
      sdk::chaseF<void>(gabi::ea(&view<CLOUD_EFF>(p)->field_0x34),
                        (f32)view<CLOUD_EFF>(p)->mWindSpeed,
                        (f32)constant(RainAlphaScale));
      u8 mode = (u8)view<Environment>(environment())->mMoyaMode;
      if (mode & 1) {
        bool ordinary =
            !stageEquals(StageDragon) ||
            (u8)view<Environment>(environment())->mbVrboxInvisible != 0;
        if (ordinary) {
          if ((u8)view<Environment>(environment())->mMoyaMode == 1)
            view<CLOUD_EFF>(p)->mPos.y =
                gabi::fmadds((f32)constant(CloudVerticalGain),
                             (f32)view<CLOUD_EFF>(p)->mWindSpeed,
                             (f32)view<CLOUD_EFF>(p)->mPos.y);
        } else
          view<CLOUD_EFF>(p)->mPos.y =
              gabi::fadds_ppc((f32)view<CLOUD_EFF>(p)->mPos.y,
                              (f32)view<CLOUD_EFF>(p)->mWindSpeed);
      }
      f32 speed = (u8)view<Environment>(environment())->mMoyaMode == 1
                      ? (f32)constant(CloudMode1Speed)
                      : (f32)constant(CloudNormalSpeed);
      f32 windSpeed = (f32)view<CLOUD_EFF>(p)->mWindSpeed;
      view<CLOUD_EFF>(p)->mPos.x =
          gabi::fmadds(gabi::fmuls_ppc((f32)wind->x, speed), windSpeed,
                       (f32)view<CLOUD_EFF>(p)->mPos.x);
      view<CLOUD_EFF>(p)->mPos.y =
          gabi::fmadds(gabi::fmuls_ppc((f32)wind->y, speed), windSpeed,
                       (f32)view<CLOUD_EFF>(p)->mPos.y);
      view<CLOUD_EFF>(p)->mPos.z =
          gabi::fmadds(gabi::fmuls_ppc((f32)wind->z, speed), windSpeed,
                       (f32)view<CLOUD_EFF>(p)->mPos.z);
      sdk::getPointWindVectorPower<void>(outputWind.get(), position.get());
      pointWind->x = outputWind->x;
      pointWind->y = outputWind->y;
      pointWind->z = outputWind->z;
      for (u32 off = 0; off < 12; off += 4)
        if ((f32)vectorComponent(&view<CLOUD_EFF>(p)->mPntWindVel, off / 4) <
            pointThreshold)
          vectorComponent(&view<CLOUD_EFF>(p)->mPntWindVel, off / 4) =
              gabi::fmadds((f32)vectorComponent(pointWind.get(), off / 4),
                           pointGain,
                           (f32)vectorComponent(
                               &view<CLOUD_EFF>(p)->mPntWindVel, off / 4));
      for (u32 off = 0; off < 12; off += 4)
        sdk::addCalc<void>(gabi::ea(&vectorComponent(
                               &view<CLOUD_EFF>(p)->mPntWindVel, off / 4)),
                           zero, blend, small, pointMin);
      for (u32 off = 0; off < 12; off += 4) {
        f32 factor = gabi::fadds_ppc(sdk::randomFX<f32>(jitter), one);
        vectorComponent(&view<CLOUD_EFF>(p)->mPos, off / 4) = gabi::fmadds(
            (f32)vectorComponent(&view<CLOUD_EFF>(p)->mPntWindVel, off / 4),
            factor, (f32)vectorComponent(&view<CLOUD_EFF>(p)->mPos, off / 4));
      }
      stageEquals(StageKaze, true);
      bool special = stageEquals(StageAdanmae);
      f32 wanderSpeed = gabi::fmuls_ppc(
          speed, special ? initRadius : (f32)constant(CloudSpeedRandomRange));
      for (u32 off = 0; off < 12; off += 4)
        vectorComponent(&view<CLOUD_EFF>(p)->mPos, off / 4) = gabi::fmadds(
            gabi::fsubs_ppc(
                (f32)vectorComponent(&view<CLOUD_EFF>(p)->mVelRndm, off / 4) /
                    phaseMax,
                half),
            wanderSpeed,
            (f32)vectorComponent(&view<CLOUD_EFF>(p)->mPos, off / 4));
      copyCenter(p);
      sumPosition(p);
      if (distance(gabi::ea(position.get()), gabi::ea(center.get())) >
          (f32)constant(CloudWrapDistance)) {
        copyCenter(p);
        sdk::getVector<void>(position.get(), center.get(), pointWind.get());
        for (u32 off = 0; off < 12; off += 4) {
          f32 random = sdk::randomFX<f32>(scatter);
          vectorComponent(pointWind.get(), off / 4) = gabi::fadds_ppc(
              (f32)vectorComponent(pointWind.get(), off / 4), random);
        }
        for (u32 off = 0; off < 12; off += 4)
          vectorComponent(&view<CLOUD_EFF>(p)->mPos, off / 4) = gabi::fmuls_ppc(
              gabi::fmuls_ppc((f32)vectorComponent(pointWind.get(), off / 4),
                              radius),
              wrapMul);
        view<CLOUD_EFF>(p)->mAlpha = zero;
      }
      sumPosition(p);
    } else {
      if (state == ParticleExpired)
        view<CLOUD_EFF>(p)->mStatus = ParticleUninitialized;
      sumPosition(p);
    }
    f32 viewFactor = (f32)view<View>(view<GameInfo>(gameInfo())->view)->fovy /
                     (f32)constant(FovScale);
    if (viewFactor > one)
      viewFactor = one;
    f32 cameraDistance = distance(gabi::ea(position.get()),
                                  gabi::ea(&view<Camera>(camera)->eye)) /
                         (f32)constant(CloudDistanceScale);
    f32 sine = sampleSine(view<CLOUD_EFF>(p)->mAnimCounter),
        baseSize = (f32)view<CLOUD_EFF>(p)->mInitialSize;
    f32 animated =
        gabi::fmadds(gabi::fmuls_ppc(baseSize, blend), sine, baseSize);
    view<CLOUD_EFF>(p)->mSize = gabi::fmuls_ppc(
        gabi::fmuls_ppc(
            animated, gabi::fmadds(cameraDistance,
                                   (f32)constant(CloudSizeDistanceSlope), one)),
        viewFactor);
    f32 dist = distance(gabi::ea(position.get()), gabi::ea(center.get())),
        alpha = one;
    if (dist < radius) {
      if (dist > innerRadius)
        alpha =
            gabi::fsubs_ppc(one, gabi::fsubs_ppc(dist, innerRadius) /
                                     (f32)constant(CloudAlphaDistanceRange));
    } else
      alpha = zero;
    alpha = gabi::fmuls_ppc(
        gabi::fmuls_ppc(gabi::fmuls_ppc(alpha, alpha), alpha), cameraDistance);
    alpha = gabi::fmuls_ppc(alpha, alphaScale);
    f32 limit = (f32)constant(CloudAlphaLimit);
    if ((u8)view<Environment>(environment())->mMoyaMode == 3)
      limit = blend;
    else if ((u8)view<Environment>(environment())->mMoyaMode == 4)
      limit = (f32)constant(CloudMode4Alpha);
    if (i > (s32)((u32)view<Environment>(environment())->mMoyaCount - 1)) {
      alpha = zero;
      if ((f32)view<CLOUD_EFF>(p)->mAlpha < (f32)constant(CloudOffThreshold) &&
          (u32)i == (u32)view<dKankyo_cloud_Packet>(packet)->mCount - 1)
        view<dKankyo_cloud_Packet>(packet)->mCount =
            (u32)view<dKankyo_cloud_Packet>(packet)->mCount - 1;
    }
    view<CLOUD_EFF>(p)->mAlpha = gabi::fmuls_ppc(alpha, limit);
  }
}
} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
