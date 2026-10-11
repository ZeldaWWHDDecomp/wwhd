/* Sun and lens visibility movement. HD widescreen occlusion and lighting.
 * Preserves native call order, double distance compares and fused arithmetic. */
#include "d/d_kankyo_rain_weather.h"
using namespace weather_move;
static SafeStringVtableHD sunSafeStringVtable;
struct SunCheckPointHD {
  be<f32> x, y;
};
static SunCheckPointHD sunSun_chkpnt[5] = {{0.0f, 0.0f},
                                        {-25.0f, -25.0f},
                                        {25.0f, -25.0f},
                                        {-25.0f, 25.0f},
                                        {25.0f, 25.0f}};
static bool equalWeatherStrings(be<u8> *left, be<u8> *right) {
  if (left == right)
    return true;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 a = left[i], b = right[i];
    if (a != b)
      return false;
    if (!a)
      return true;
  }
  return false;
}
// HD: SafeString stage-name override, widescreen sample positions and a short
// fully-visible delay replace the GameCube visibility ramp. Projection uses
// explicit HD parameters; the two screen-axis falloffs remain separate.
/* 02565E0C */
void dKyr_sun_move() {
  SunPacketHD *pSunPkt = weatherEnvironment()->mpSunPacket;
  SunLenzPacketHD *pLenzPkt = weatherEnvironment()->mpSunlenzPacket;
  WeatherCameraHD *pCamera = weatherPlay()->mpCamera;
  weatherPlay();
  WeatherPlayHD *info = weatherPlay();
  WeatherCameraHD *secondaryCamera = info->mpCamera;
  f32 zero = 0.0f, staringAtSunAmount = zero;
  u32 stageMode = 0, numCenterPointsVisible = 0, numPointsVisible = 0;
  WeatherSafeStringHD stage;
  WeatherSafeStringHD current;
  WeatherVectorHD direction;
  WeatherVectorHD screen;
  WeatherVectorHD point;
  WeatherProjectParamsHD params;
  stage.mStringTop = gabi::at<be<u8>>(gabi::guest_addr("Name"));
  stage.__vtbl = &sunSafeStringVtable;
  current.mStringTop = weatherPlay()->mStageName;
  current.__vtbl = &sunSafeStringVtable;
  stage.assureTermination();
  stage.assureTermination();
  be<u8> *expected = stage.mStringTop;
  current.assureTermination();
  bool same = equalWeatherStrings(expected, current.mStringTop);
  bool alternate;
  if (same)
    alternate = (s16)weatherEnvironment()->mBaseLightRed != 0;
  else {
    WeatherStageDtHD *object = &weatherPlay()->mStage;
    StageInfoHD *returned = object->getStagInfo();
    stageMode = (returned->mTypeFlags.get() >> 16) & 7;
    alternate = (s16)weatherEnvironment()->mBaseLightRed != 0;
  }
  if (stageMode == 2)
    alternate = true;
  WeatherEnvironmentHD *env = weatherEnvironment();
  dKyr_get_vectle_calcHD(&pCamera->mEye,
                         alternate ? &env->mSunPos2 : &env->mBaseLightPos,
                         &direction);
  f32 distanceScale = 8e+03f;
  pSunPkt->mPos[0].x =
      gabi::fmadds((f32)direction.x, distanceScale, (f32)pCamera->mEye.x);
  f32 sunY =
      gabi::fmadds((f32)direction.y, distanceScale, (f32)pCamera->mEye.y);
  pSunPkt->mPos[0].y = sunY;
  pSunPkt->mPos[0].z =
      gabi::fmadds((f32)direction.z, distanceScale, (f32)pCamera->mEye.z);
  f32 height = gabi::fsubs_ppc(sunY, (f32)pCamera->mEye.y) / distanceScale,
      one = 1.0f;
  if (height < zero)
    height = gabi::fsubs_ppc(one, zero);
  else {
    if (!(height < one))
      height = one;
    height = gabi::fsubs_ppc(one, height);
  }
  height = gabi::fnmsubs(height, height, one);
  u8 delay = (u8)pSunPkt->mOcclusionDelayHD;
  if (delay)
    pSunPkt->mOcclusionDelayHD = delay - 1;
  pSunPkt->mHasVisiblePointHD = 0;
  f32 half = 0.5f, small = 0.01f;
  if ((f32)weatherEnvironment()->mCurTime > 97.5f &&
      (f32)weatherEnvironment()->mCurTime < 292.5f) {
    weatherProjectParams(&params);
    weatherAddCalc(&pSunPkt->mSunAlpha, one, half, 0.1f, small);
    f32 top = secondaryCamera
                  ? gabi::fmuls_ppc((f32)secondaryCamera->mFovy, 0.44444445f)
                  : zero;
    weatherProject(&pSunPkt->mPos[0], &screen, &params);
    f32 depthLimitZero = zero, bottom = gabi::fsubs_ppc(4.8e+02f, top);
    f64 horizontalLimit = 640.0;
    u32 misses = 0;
    for (u32 i = 0; i < 5; i++) {
      f32 x = gabi::fsubs_ppc((f32)screen.x, (f32)sunSun_chkpnt[i].x);
      f32 y = gabi::fsubs_ppc((f32)screen.y, (f32)sunSun_chkpnt[i].y);
      if (x > depthLimitZero && (f64)x < horizontalLimit && y > top &&
          y < bottom) {
        be<u32> *result = &pSunPkt->mVizChkData[i];
        if (result->get() >= 0xFFFFFF) {
          numPointsVisible = (numPointsVisible + 1) & 255;
          if (i == 0)
            numCenterPointsVisible = (numCenterPointsVisible + 1) & 255;
        }
        weatherPeekZ(weatherPlay()->mPeekZService, (s32)(s16)gabi::ftoi(x),
                     (s32)(s16)gabi::ftoi(y), result);
      } else
        misses++;
    }
    if (numPointsVisible) {
      delay = (u8)pSunPkt->mOcclusionDelayHD;
      if (misses && numCenterPointsVisible) {
        numPointsVisible = 5;
        numCenterPointsVisible = 1;
      }
      if (delay < 5)
        pSunPkt->mOcclusionDelayHD = delay + 2;
      pSunPkt->mHasVisiblePointHD = 1;
    }
    pLenzPkt->mPreviousDistanceHD = (f32)pLenzPkt->mDistanceHD;
    pLenzPkt->mPreviousDepthHD = (f32)pLenzPkt->mDepthHD;
    pLenzPkt->mDistanceHD = 1e+09f;
    pLenzPkt->mDepthHD = zero;
    point.x = (f32)screen.x;
    point.y = 2.4e+02f;
    point.z = zero;
    f32 squared = weatherDistanceSquared(&point, &screen);
    staringAtSunAmount = (f32)(weatherSqrtDouble((f64)squared) / (f64)5.5e+02f);
    if (staringAtSunAmount > one)
      staringAtSunAmount = one;
    point.x = 3.2e+02f;
    point.y = (f32)screen.y;
    squared = weatherDistanceSquared(&point, &screen);
    f32 side = (f32)(weatherSqrtDouble((f64)squared) / (f64)6.5e+02f);
    if (side > one)
      side = one;
    f32 edge =
        gabi::fnmsubs(gabi::fadds_ppc(side, staringAtSunAmount), half, one);
    staringAtSunAmount = gabi::fmuls_ppc(edge, edge);
    pLenzPkt->mDistFalloff = gabi::fsubs_ppc(one, staringAtSunAmount);
    staringAtSunAmount =
        gabi::fmuls_ppc(staringAtSunAmount, staringAtSunAmount);
  } else {
    weatherAddCalc(&pSunPkt->mSunAlpha, zero, half, 0.1f, small);
    pSunPkt->mOcclusionDelayHD = 0;
    pSunPkt->mHasVisiblePointHD = 0;
    numPointsVisible = 0;
  }
  if ((u8)weatherEnvironment()->mColpatWeather)
    numCenterPointsVisible = numPointsVisible = 0;
  else if ((u8)weatherEnvironment()->mColpatCurr &&
           (f32)weatherEnvironment()->mColPatBlend > half)
    numCenterPointsVisible = numPointsVisible = 0;
  if (stageMode == 2)
    numCenterPointsVisible = numPointsVisible = 0;
  bool daylight = (f32)weatherEnvironment()->mCurTime < 1.2e+02f;
  if (!daylight)
    daylight = (f32)weatherEnvironment()->mCurTime > 2.7e+02f;
  u32 noFlare;
  if (daylight) {
    weatherAddCalc(&pSunPkt->mVisibility, zero, half, 0.4f, 0.1f);
    numPointsVisible = 0;
    pSunPkt->mVisibility = zero;
    pSunPkt->mFullyVisibleDelayHD = 3;
    noFlare = 1;
  } else if (numCenterPointsVisible && numPointsVisible == 5) {
    weatherAddCalc(&pSunPkt->mVisibility, one, half, half, 0.1f);
    u32 wait = (u32)pSunPkt->mFullyVisibleDelayHD;
    if (wait) {
      wait--;
      pSunPkt->mFullyVisibleDelayHD = wait;
    }
    if (wait)
      noFlare = 0;
    else {
      weatherAddCalc(&pSunPkt->mVisibility, 0.75f, 0.2f, 0.2f, small);
      noFlare = 0;
    }
  } else if (!numCenterPointsVisible) {
    weatherAddCalc(&pSunPkt->mVisibility, zero, half, 0.4f, 0.1f);
    if (!numPointsVisible) {
      pSunPkt->mVisibility = zero;
      pSunPkt->mFullyVisibleDelayHD = 3;
      noFlare = 1;
    } else
      noFlare = numPointsVisible < 2 ? 1 : 0;
  } else
    noFlare = numPointsVisible < 2 ? 1 : 0;
  weatherEnvironment()->mpSunlenzPacket->mbDrawLenzInSky = noFlare;
  if ((f32)pSunPkt->mPos[0].y > zero &&
      !(u8)weatherEnvironment()->mpSunlenzPacket->mbDrawLenzInSky) {
    dKy_set_actcol_ratioHD(
        gabi::fnmsubs(staringAtSunAmount, (f32)pSunPkt->mVisibility, one));
    dKy_set_bgcol_ratioHD(
        gabi::fnmsubs(staringAtSunAmount, (f32)pSunPkt->mVisibility, one));
    dKy_set_fogcol_ratioHD(
        gabi::fmadds(gabi::fmuls_ppc(gabi::fmuls_ppc(staringAtSunAmount,
                                                     (f32)pSunPkt->mVisibility),
                                     height),
                     half, one));
    dKy_set_vrboxcol_ratioHD(
        gabi::fmadds(gabi::fmuls_ppc(gabi::fmuls_ppc(staringAtSunAmount,
                                                     (f32)pSunPkt->mVisibility),
                                     height),
                     half, one));
  }
  if (dKyr_moon_arrival_checkHD()) {
    height = gabi::fsubs_ppc((f32)pSunPkt->mPos[0].y, (f32)pCamera->mEye.y) /
             -8e+03f;
    height = gabi::fmuls_ppc(gabi::fmuls_ppc(height, height), 6.0f);
    if (height > one)
      height = one;
    weatherAddCalc(&pSunPkt->mMoonAlpha, height, 0.2f, small, 0.001f);
  } else
    weatherAddCalc(&pSunPkt->mMoonAlpha, zero, 0.2f, small, 0.001f);
}
/* WWHD glue (tools/verify/autoglue.py)
str 1004F64C "Name"
data sunSafeStringVtable 1004F3AC
data sunSun_chkpnt 1004F654
ext 02555D0C WeatherEnvironmentHD *weatherEnvironment()
ext 025200D4 WeatherPlayHD *weatherPlay()
ext 02563F64 void dKyr_get_vectle_calcHD(WeatherVectorHD*, WeatherVectorHD*, WeatherVectorHD*)
ext 02563DD8 void weatherProjectParams(WeatherProjectParamsHD*)
ext 0200ECD4 void weatherAddCalc(be<f32> *, f32, f32, f32, f32)
ext 025F1018 void weatherProject(WeatherVectorHD*, WeatherVectorHD*, WeatherProjectParamsHD*)
ext 0252E34C void weatherPeekZ(void*, s32, s32, be<u32>*)
ext 028E8DE8 f32 weatherDistanceSquared(WeatherVectorHD *, WeatherVectorHD *)
ext 028F4384 f64 weatherSqrtDouble(f64)
ext 0255A3D8 void dKy_set_actcol_ratioHD(f32)
ext 0255A418 void dKy_set_bgcol_ratioHD(f32)
ext 0255A458 void dKy_set_fogcol_ratioHD(f32)
ext 0255A51C void dKy_set_vrboxcol_ratioHD(f32)
ext 02565DAC u32 dKyr_moon_arrival_checkHD()
*/
