/* Sky-cloud movement on the HD weather layout. */
#include "d/d_kankyo_rain_weather.h"
using namespace weather_move;
using namespace gabi;
using SavedWindPointerHD = gptr<WeatherSavedWindHD>;
using WeatherRoomNumberHD = be<s8>;
static SavedWindPointerHD cloudSavedWind;
static WeatherRoomNumberHD cloudStayRoom;
static SafeStringVtableHD cloudSafeStringVtable;
namespace weather_move {
static WeatherTrigHD cloudWeatherTrig;
static f32 weatherSin(u32 angle) {
  return cloudWeatherTrig.entries[(u16)angle >> 3].sin;
}
static f32 weatherCos(u32 angle) {
  return cloudWeatherTrig.entries[(u16)angle >> 3].cos;
}
} // namespace weather_move
static bool cloudStage(const char *literal, WeatherSafeStringHD *left,
                       WeatherSafeStringHD *right) {
  left->mStringTop = gabi::at<be<u8>>(gabi::guest_addr(literal));
  left->__vtbl = &cloudSafeStringVtable;
  right->mStringTop = weatherPlay()->mStageName;
  right->__vtbl = &cloudSafeStringVtable;
  left->assureTermination();
  left->assureTermination();
  be<u8> *a = left->mStringTop;
  right->assureTermination();
  be<u8> *b = right->mStringTop;
  if (a == b)
    return true;
  a = left->mStringTop;
  b = right->mStringTop;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 x = a[i], y = b[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
static void copyCloudHorizontal(WeatherVectorHD *target, VrkumoEffHD *slot) {
  target->words[0] = slot->mPosition.words[0].get();
  target->y = 0.0f;
  target->words[2] = slot->mPosition.words[2].get();
}
// HD: named-stage overrides include the Name bypass, both Siren rooms and
// ADMumi. HD: Siren uses positive 14101 sea height rather than the GameCube
// negative height, and the override applies in rooms 17 and 18. The cloud
// rotation counter and initialized particles falling through to movement are
// retained, as are the ordered NaN-aware clamps and six-factor radial falloff.
/* 0256DDF8 */
void vrkumo_move() {
  CloudFrameHD frame;
  // HD: preserve the GHS frame bias and every SafeString/vector slot.
  CloudWorkHD *work = gabi::at<CloudWorkHD>(gabi::ea(&frame) - 8);
  dKyw_get_wind_vecpowHD(&work->mWindVectorPower);
  VrkumoPacketHD *vrkumo_packet = weatherEnvironment()->mpVrkumoPacket;
  WeatherCameraHD *camera = weatherPlay()->mpCamera;
  f32 strength = (f32)weatherEnvironment()->mVrkumoStrength, zero = 0.0f,
      one = 1.0f;
  f32 vertical = fmadds(-1e+03f, strength, 3e+03f),
      edge = fmadds(zero, strength, 0.98f);
  f32 maximum = fmadds(-0.55f, strength, one),
      speed = fmadds(4.3f, strength, 4.0f),
      height = fmadds(-5e+02f, strength, 1e+03f);
  work->mVertical = vertical;
  work->mMaximumAlpha = maximum;
  bool dragon = cloudStage("M_DragB", &work->mDragonName, &work->mDragonStage);
  if (dragon) {
    work->mWindVectorPower.y = zero;
    height = 3e+02f;
    work->mWindVectorPower.z = zero;
    work->mWindVectorPower.x = -1.0f;
  } else if (!cloudStage("Name", &work->mNameName, &work->mNameStage)) {
    WeatherPlayHD *play = weatherPlay();
    StageInfoHD *info = play->mStage.getStagInfo();
    if (((info->mTypeFlags.get() >> 16) & 7) == 2) {
      s32 offset = 0;
      if (cloudStage("LinkRM", &work->mLinkRoomName, &work->mLinkRoomStage))
        offset = 0x4000;
      else if (cloudStage("Orichh", &work->mOrichName, &work->mOrichStage))
        offset = -0x4000;
      else if (cloudStage("Ojhous2", &work->mOjhousName, &work->mOjhousStage))
        offset = 0x7FFF;
      else if (cloudStage("Omasao", &work->mOmasaoName, &work->mOmasaoStage))
        offset = -0x4000;
      else if (cloudStage("Onobuta", &work->mOnobutaName, &work->mOnobutaStage))
        offset = 0x4000;
      WeatherSavedWindHD *save = cloudSavedWind;
      u32 x = 0, y = (u16)offset;
      if ((s16)save->mWindX != -1 || (s16)save->mWindY != -1) {
        x = (u16)weatherEnvironment()->mTactWindAngleX;
        y = (u16)((s16)weatherEnvironment()->mTactWindAngleY + offset);
      }
      f32 scale = 0.6f, sy = weatherSin(x), cx = weatherCos(x),
          cy = weatherCos(y), sx = weatherSin(y);
      work->mWindVectorPower.y = fmuls_ppc(sy, scale);
      work->mWindVectorPower.x = fmuls_ppc(fmuls_ppc(cx, cy), scale);
      work->mWindVectorPower.z = fmuls_ppc(fmuls_ppc(cx, sx), scale);
    }
  }
  if (weatherPlay()->mpView.get()) {
    s32 stay = cloudStayRoom.get();
    f32 sea = zero;
    if (stay >= 0) {
      WeatherPlayHD *play = weatherPlay();
      WeatherRoomDtHD *room = weatherRoomStatus(&play->mRoomControl, stay);
      WeatherFileListHD *file = room->getFileListInfo();
      if (file)
        sea = file->mSeaLevel;
    }
    if (cloudStage("Siren", &work->mSeaStageName, &work->mSirenStage)) {
      s32 room = cloudStayRoom.get();
      if (room == 17 || room == 18)
        sea = 14101.0f;
    }
    if (cloudStage("ADMumi", &work->mSeaStageName, &work->mAdmumiStage))
      sea = 4.8e+04f;
    height = fnmsubs(fsubs_ppc((f32)camera->mEye.y, sea), 0.09f, height);
  }
  f32 fadeEdge = 0.88f, epsilon = 0.01f, alphaStep = 0.1f, smallStep = 0.005f,
      half = 0.5f, angleMax = 65535.0f, radius = 1.5e+04f, hundred = 1e+02f,
      respawn = 1.4e+04f, alphaFactor = 0.2f, spawnMax = 1.8e+04f,
      speedRange = 4.0f, low = 0.05f, randomHeight = 0.3f, heightStep = 5e+02f;
  f32 edgeSpan = fsubs_ppc(edge, fadeEdge);
  bool noSpan = !(edgeSpan > zero);
  work->mEpsilon = epsilon;
  work->mSmallStep = smallStep;
  work->mAlphaStep = alphaStep;
  work->mHalf = half;
  work->mHeightStep = heightStep;
  vrkumo_packet->mCloudAngleHD =
      fadds_ppc((f32)vrkumo_packet->mCloudAngleHD, hundred);
  for (s32 i = 0; i < 100; i++) {
    VrkumoEffHD *slot = &vrkumo_packet->mInst[i];
    u32 state = (u8)slot->mStatus;
    f32 fraction = (f32)i / hundred;
    if (state == 0) {
      u32 angle = (u16)ftoi(cM_rndF(angleMax));
      f32 distance = cM_rndF(spawnMax);
      if (distance > radius)
        distance = (f32)((f64)cM_rndF(1e+03f) + 14000.0);
      u32 a = weatherFloatAngle((f32)angle);
      slot->mPosition.x = fmuls_ppc(weatherSin(a), distance);
      slot->mPosition.y = zero;
      a = weatherFloatAngle((f32)angle);
      slot->mPosition.z = fmuls_ppc(weatherCos(a), distance);
      f32 random = cM_rndFX(randomHeight);
      slot->mHeight = fmuls_ppc(randomHeight, random);
      slot->mAlpha = zero;
      random = cM_rndF(speedRange);
      u8 old = (u8)slot->mStatus;
      slot->mStatus = old + 1;
      slot->mSpeed = fadds_ppc(random, (f32)work->mHalf);
      state = 1;
    }
    if (state == 1) {
      copyCloudHorizontal(&work->mMovementPosition, slot);
      f32 distance = weatherMagnitudeSquared(&work->mMovementPosition);
      distance = weatherSqrtFloat(distance);
      if (distance > radius) {
        distance = weatherMagnitudeSquared(&work->mMovementPosition);
        distance = weatherSqrtFloat(distance);
        if (distance > 1.51e+04f) {
          slot->mPosition.x = cM_rndFX(respawn);
          slot->mPosition.z = cM_rndFX(respawn);
        } else {
          f32 x = (f32)slot->mPosition.x, z = (f32)slot->mPosition.z;
          slot->mPosition.x = -x;
          slot->mPosition.z = -z;
        }
        slot->mAlpha = zero;
      }
      if (!((f32)slot->mAlpha > zero)) {
        f32 advance = fmadds(fraction, speed, speed);
        slot->mPosition.x = fmadds((f32)work->mWindVectorPower.x, advance,
                                   (f32)slot->mPosition.x);
        f32 dz = fmuls_ppc((f32)work->mWindVectorPower.z, advance);
        slot->mPosition.z = fadds_ppc((f32)slot->mPosition.z, dz);
      } else {
        f32 advance = fmuls_ppc(speed, (f32)slot->mSpeed),
            x = (f32)slot->mPosition.x, wx = (f32)work->mWindVectorPower.x,
            falloff = (f32)slot->mDistFalloff;
        slot->mPosition.x = fmadds(fmuls_ppc(wx, advance), falloff, x);
        f32 dz = fmuls_ppc(fmuls_ppc((f32)work->mWindVectorPower.z, advance),
                           falloff);
        slot->mPosition.z = fadds_ppc((f32)slot->mPosition.z, dz);
      }
    }
    copyCloudHorizontal(&work->mRadialPosition, slot);
    f32 distance = weatherMagnitudeSquared(&work->mRadialPosition);
    distance = weatherSqrtFloat(distance);
    f32 ratio = distance / radius;
    f32 remaining = fsubs_ppc(one, ratio);
    if (!(remaining >= 0))
      remaining = zero;
    f32 shape = fsubs_ppc(one, remaining);
    shape = fnmsubs(fmuls_ppc(shape, shape), shape, one);
    f32 base = fmadds((f32)work->mVertical, shape, height);
    slot->mPosition.y = fmadds(fraction, (f32)work->mHeightStep, base);
    if (ratio > one)
      ratio = one;
    f32 square = fmuls_ppc(ratio, ratio), cube = fmuls_ppc(square, ratio),
        fourth = fmuls_ppc(cube, ratio), fifth = fmuls_ppc(fourth, ratio);
    slot->mDistFalloff = fnmsubs(fifth, ratio, one);
    if (cloudStage("M_DragB", &work->mParticleDragonName,
                   &work->mParticleStageName)) {
      slot->mAlpha = one;
      continue;
    }
    f32 step = (f32)work->mAlphaStep, target;
    WeatherEnvironmentHD *env = weatherEnvironment();
    s32 count = (u32)env->mVrkumoCount;
    if (i >= count) {
      step = (f32)work->mSmallStep;
      target = zero;
    } else {
      f32 falloff = (f32)slot->mDistFalloff;
      if (falloff < alphaFactor) {
        target = !(falloff < low) ? fsubs_ppc(falloff, low) / 0.15f : zero;
      } else {
        env = weatherEnvironment();
        if (i >= (s32)(u32)env->mVrkumoCount) {
          falloff = (f32)slot->mDistFalloff;
          target = !(falloff < low) ? fsubs_ppc(falloff, low) / 0.15f : zero;
        } else
          target = (f32)work->mMaximumAlpha;
      }
    }
    if (shape > edge)
      target = zero;
    else if (shape > fadeEdge) {
      if (noSpan)
        target = zero;
      else
        target = fmuls_ppc(target, fsubs_ppc(edge, shape) / edgeSpan);
    }
    weatherAddCalc(&slot->mAlpha, target, alphaFactor, step,
                   (f32)work->mEpsilon);
  }
}
/* WWHD glue (tools/verify/autoglue.py)
data cloudSavedWind 101F84DC
data cloudStayRoom 1047E6C8
data cloudSafeStringVtable 1004F3AC
data cloudWeatherTrig 104A44F8
str 1004FB24 "M_DragB"
str 1004FB44 "Name"
str 1004FB4C "LinkRM"
str 1004FB54 "Orichh"
str 1004FB2C "Ojhous2"
str 1004FB5C "Omasao"
str 1004FB34 "Onobuta"
str 1004FB3C "Siren"
str 1004FB64 "ADMumi"
ext 02555D0C WeatherEnvironmentHD *weatherEnvironment()
ext 025200D4 WeatherPlayHD *weatherPlay()
ext 0257DB28 void dKyw_get_wind_vecpowHD(WeatherVectorHD *)
ext 025C11DC WeatherRoomDtHD* weatherRoomStatus(WeatherRoomControlHD*, s32)
ext 02019510 u32 weatherFloatAngle(f32)
ext 028E8DD0 f32 weatherMagnitudeSquared(WeatherVectorHD*)
ext 028F4384 f32 weatherSqrtFloat(f32)
ext 0200ECD4 void weatherAddCalc(be<f32> *, f32, f32, f32, f32)
*/
