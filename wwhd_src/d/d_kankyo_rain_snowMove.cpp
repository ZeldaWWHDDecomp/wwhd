/* Snow weather simulation. GameCube particle names on the HD packet layout.
 * Verification preserves fused arithmetic, native call order and word copies. */
#include "d/d_kankyo_rain_weather.h"
using namespace weather_move;
namespace weather_move {
static WeatherTrigHD snowWeatherTrig;
static f32 weatherSin(u32 angle) {
  return snowWeatherTrig.entries[(u16)angle >> 3].sin;
}
static f32 weatherCos(u32 angle) {
  return snowWeatherTrig.entries[(u16)angle >> 3].cos;
}
} // namespace weather_move
// HD: requested snow count is read separately from the 16-bit active count.
// HD: native distance square roots remain double precision at boundary
// compares.
/* 02568BD4 */
void dKyr_snow_move() {
  SnowPacketHD *snow_packet = weatherEnvironment()->mpSnowPacket;
  WeatherCameraHD *camera = weatherPlay()->mpCamera;
  weatherPlay();
  WeatherVectorHD wind;
  WeatherVectorHD center;
  WeatherVectorHD heading;
  WeatherVectorHD position;
  WeatherVectorHD wrapDirection;
  WeatherDoublePosHD delta;
  dKyw_get_wind_vecpowHD(&wind);
  u32 wanted = weatherEnvironment()->mSnowRequestHD;
  s32 count = (s16)snow_packet->mEffCount;
  if (count > (s32)wanted) {
    if (!count)
      return;
  } else {
    count = (s16)weatherEnvironment()->mSnowCount;
    snow_packet->mEffCount = (u16)count;
    if (!count)
      return;
  }
  f32 depth = 5e+02f;
  dKy_set_eyevect_calc2HD(camera, &center, depth, depth);
  f32 squared = weatherDistanceSquared(&snow_packet->mOldEyePos, &camera->mEye);
  if (weatherSqrtDouble((f64)squared) > 1.5e+03f)
    snow_packet->mEffCount = 0;
  for (u32 axis = 0; axis < 3; axis++)
    snow_packet->mOldEyePos.words[axis] = camera->mEye.words[axis].get();
  WeatherVectorHD *windDirection = dKyw_get_wind_vecHD();
  f32 power = dKyw_get_wind_powHD();
  delta.x = (f64)gabi::fsubs_ppc((f32)camera->mCenter.x, (f32)camera->mEye.x);
  delta.y = 0.0;
  delta.z = (f64)gabi::fsubs_ppc((f32)camera->mCenter.z, (f32)camera->mEye.z);
  vectle_calcHD(&delta, &heading);
  f32 zero = 0.0f;
  f32 turn = cM3d_VectorProduct2dHD(zero, zero, -(f32)windDirection->x,
                                    -(f32)windDirection->z, (f32)heading.x,
                                    (f32)heading.z);
  snow_packet->mWindTurn = turn;
  f32 projection =
      gabi::fmadds((f32)windDirection->x, (f32)heading.x,
                   gabi::fmuls_ppc((f32)windDirection->z, (f32)heading.z));
  f32 one = 1.0f;
  f32 strength = gabi::fmuls_ppc(
      gabi::fmuls_ppc(
          gabi::fmuls_ppc(gabi::fsubs_ppc(one, std::fabs(projection)), power),
          gabi::fsubs_ppc(one, std::fabs((f32)windDirection->y))),
      std::fabs(turn));
  count = (s16)snow_packet->mEffCount;
  snow_packet->mWindStrength = strength;
  if (count - 1 < 0)
    return;
  f32 approach = 0.2f, phaseStep = 0.02f, phaseRange = 3.6e+02f,
      randomStep = 0.1f, half = 0.5f, wrapJitter = 27.5f, fallRange = 8.5f,
      sizeBase = 5.0f, small = 0.01f, range = 5.5e+02f, unit = 6e+02f,
      fadeOffset = 62.5f, jitterScale = 1.5f;
  for (s32 i = count - 1; i >= 0; i--) {
    SnowEffHD *particle = &snow_packet->mEff[i];
    f32 fall = -gabi::fadds_ppc(cM_rndF(fallRange), jitterScale);
    f32 size = gabi::fadds_ppc((f32)(i & 15), sizeBase);
    u8 state = (u8)particle->mStatus;
    if (!state) {
      particle->mWindSpeed = size;
      particle->mGravity = fall;
      particle->mTimer = 0;
      particle->mBasePos.x = (f32)center.x;
      particle->mBasePos.y = (f32)center.y;
      particle->mBasePos.z = (f32)center.z;
      particle->mPos.x = cM_rndFX(range);
      particle->mPos.y = range;
      particle->mPos.z = cM_rndFX(range);
      particle->mScale = zero;
      particle->mPosWaveX = cM_rndF(phaseRange);
      f32 phase = cM_rndF(phaseRange);
      particle->mStatus = (u8)particle->mStatus + 1;
      particle->mPosWaveZ = phase;
    } else if (state <= 3) {
      f32 noise = cM_rndFX(randomStep);
      weatherAddCalc(&particle->mWindSpeed,
                     gabi::fsubs_ppc((f32)particle->mWindSpeed, noise), half,
                     randomStep, small);
      if ((u8)particle->mStatus != 3 || (f32)particle->mScale < small) {
        f32 sizeNow = (f32)particle->mWindSpeed;
        particle->mPos.x =
            gabi::fmadds((f32)wind.x, sizeNow, (f32)particle->mPos.x);
        particle->mPos.z =
            gabi::fmadds((f32)wind.z, sizeNow, (f32)particle->mPos.z);
        particle->mPos.y = gabi::fadds_ppc(
            (f32)particle->mPos.y,
            gabi::fmadds((f32)wind.y, sizeNow, (f32)particle->mGravity));
        u32 angle = weatherFloatAngle((f32)particle->mPosWaveX);
        particle->mPos.x =
            gabi::fmadds(weatherSin(angle), jitterScale, (f32)particle->mPos.x);
        angle = weatherFloatAngle((f32)particle->mPosWaveZ);
        f32 phase = gabi::fadds_ppc((f32)particle->mPosWaveX, phaseStep),
            phaseZ = gabi::fadds_ppc((f32)particle->mPosWaveZ, phaseStep);
        particle->mPos.z =
            gabi::fmadds(weatherSin(angle), jitterScale, (f32)particle->mPos.z);
        particle->mPosWaveX = phase;
        particle->mPosWaveZ = phaseZ;
      }
      position.x =
          gabi::fadds_ppc((f32)particle->mBasePos.x, (f32)particle->mPos.x);
      position.y =
          gabi::fadds_ppc((f32)particle->mBasePos.y, (f32)particle->mPos.y);
      position.z =
          gabi::fadds_ppc((f32)particle->mBasePos.z, (f32)particle->mPos.z);
      squared = weatherDistanceSquared(&position, &center);
      f64 distance = weatherSqrtDouble((f64)squared);
      s16 delay = (s16)particle->mTimer;
      if (!delay && distance > range) {
        particle->mWindSpeed = size;
        particle->mGravity = fall;
        particle->mTimer = 10;
        particle->mBasePos.x = (f32)center.x;
        particle->mBasePos.y = (f32)center.y;
        particle->mBasePos.z = (f32)center.z;
        squared = weatherDistanceSquared(&position, &center);
        distance = weatherSqrtDouble((f64)squared);
        if (distance > unit) {
          particle->mPos.x = cM_rndFX(range);
          particle->mPos.y = cM_rndFX(range);
          f32 z = cM_rndFX(range);
          particle->mStatus = 1;
          particle->mPos.z = z;
        } else {
          f32 noise = cM_rndFX(wrapJitter);
          get_vectle_calcHD(&position, &center, &wrapDirection);
          f32 radius = gabi::fadds_ppc(noise, range);
          particle->mPos.x = gabi::fmuls_ppc((f32)wrapDirection.x, radius);
          particle->mPos.y = gabi::fmuls_ppc((f32)wrapDirection.y, radius);
          f32 z = gabi::fmuls_ppc((f32)wrapDirection.z, radius);
          particle->mStatus = 1;
          particle->mPos.z = z;
        }
      } else if (delay)
        particle->mTimer = (u16)(delay - 1);
    }
    position.x =
        gabi::fadds_ppc((f32)particle->mBasePos.x, (f32)particle->mPos.x);
    position.y =
        gabi::fadds_ppc((f32)particle->mBasePos.y, (f32)particle->mPos.y);
    position.z =
        gabi::fadds_ppc((f32)particle->mBasePos.z, (f32)particle->mPos.z);
    f32 alpha;
    bool fadeOut;
    if ((u8)particle->mStatus == 3) {
      alpha = zero;
      fadeOut = i > (s32)((u32)weatherEnvironment()->mSnowRequestHD - 1);
    } else {
      squared = weatherDistanceSquared(&position, &camera->mEye);
      f32 value = (f32)(weatherSqrtDouble((f64)squared) + (f64)fadeOffset);
      value = value >= 0.0f ? value : zero;
      alpha = gabi::fsubs_ppc(one, value / 1125.0f);
      if (alpha > one) {
        alpha = one;
        fadeOut = i > (s32)((u32)weatherEnvironment()->mSnowRequestHD - 1);
      } else {
        if (alpha < zero)
          alpha = zero;
        fadeOut = i > (s32)((u32)weatherEnvironment()->mSnowRequestHD - 1);
      }
    }
    if (fadeOut)
      weatherAddCalc(&particle->mScale, zero, approach, randomStep, small);
    else
      particle->mScale = alpha;
    if (i > (s32)((u32)weatherEnvironment()->mSnowRequestHD - 1) &&
        (f32)particle->mScale < small &&
        (u32)i == (u32)((s16)snow_packet->mEffCount - 1))
      snow_packet->mEffCount = (u16)((s16)snow_packet->mEffCount - 1);
  }
}
/* WWHD glue (tools/verify/autoglue.py)
ext 02555D0C WeatherEnvironmentHD *weatherEnvironment()
ext 025200D4 WeatherPlayHD *weatherPlay()
ext 0257DB28 void dKyw_get_wind_vecpowHD(WeatherVectorHD *)
ext 0257DAA8 WeatherVectorHD *dKyw_get_wind_vecHD()
ext 02578348 f32 dKyw_get_wind_powHD()
ext 0256401C void dKy_set_eyevect_calc2HD(WeatherCameraHD *, WeatherVectorHD *, f32, f32)
ext 028E8DE8 f32 weatherDistanceSquared(WeatherVectorHD *, WeatherVectorHD *)
ext 028F4384 f64 weatherSqrtDouble(f64)
ext 02563E48 void vectle_calcHD(WeatherDoublePosHD *, WeatherVectorHD *)
ext 02010CFC f32 cM3d_VectorProduct2dHD(f32, f32, f32, f32, f32, f32)
ext 0200ECD4 void weatherAddCalc(be<f32> *, f32, f32, f32, f32)
ext 02019510 u32 weatherFloatAngle(f32)
ext 02563F0C void get_vectle_calcHD(WeatherVectorHD *, WeatherVectorHD *, WeatherVectorHD *)
data snowWeatherTrig 104A44F8
*/
