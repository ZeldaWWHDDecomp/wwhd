/* Wave movement with HD stage shorelines and weather packet layout. */
#include "d/d_kankyo_rain_weather.h"
using namespace weather_move;
using namespace gabi;
using SavedWindPointerHD = gptr<WeatherSavedWindHD>;
using WeatherRoomNumberHD = be<s8>;
static SavedWindPointerHD waveSavedWind;
static WeatherRoomNumberHD waveStayRoom;
static SafeStringVtableHD waveSafeStringVtable;
static WeatherTrigHD waveWeatherTrig;
static f32 waveDistance(WeatherVectorHD *a, WeatherVectorHD *b) {
  // HD: the squared-distance result is forwarded in the native FPR; neither
  // wrapper may materialize or re-pack the intermediate float.
  weatherDistanceForward(a, b);
  return weatherSqrtForward();
}
static void wavePosition(WeatherVectorHD *out, WaveEffHD *p) {
  out->x = fadds_ppc(p->mBasePos.x, p->mPos.x);
  out->y = fadds_ppc(p->mBasePos.y, p->mPos.y);
  out->z = fadds_ppc(p->mBasePos.z, p->mPos.z);
}
static bool waveStage(const char *name, WeatherSafeStringHD *a,
                      WeatherSafeStringHD *b) {
  a->mStringTop = at<be<u8>>(guest_addr(name));
  a->__vtbl = &waveSafeStringVtable;
  b->mStringTop = weatherPlay()->mStageName;
  b->__vtbl = &waveSafeStringVtable;
  a->assureTermination();
  a->assureTermination();
  be<u8> *first = a->mStringTop;
  b->assureTermination();
  be<u8> *second = b->mStringTop;
  if (first == second)
    return true;
  for (u32 i = 0; i < 0x40001; i++) {
    u8 x = first[i], y = second[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
// HD: room-specific sea levels, the Name wind bypass and ADMumi's three
// shoreline exclusion regions are additional to the GameCube movement.
// HD also keeps moving when the wave-flat setting reaches one.
/* 0256A448 */
void wave_move() {
  WeatherEnvironmentHD *env0 = weatherEnvironment();
  f32 zero = 0.0f, sea = zero;
  WaveWorkHD work;
  WeatherVectorHD *wind = &work.mWindVectorPower;
  WeatherVectorHD *eye = &work.mEyeVector;
  WeatherVectorHD *point = &work.mPosition;
  WeatherVectorHD *scratch = &work.mDistancePosition;
  WeatherVectorHD *finalpos = &work.mFinalPosition;
  WeatherVectorHD *direction = &work.mWrapDirection;
  WeatherVectorHD *normal = &work.mNormal;
  WeatherDoublePosHD *doublePos = &work.mDeltaXZ;
  dKyw_get_wind_vecpowHD(wind);
  WeatherEnvironmentHD *env = weatherEnvironment();
  WavePacketHD *pPkt = env->mpWavePacket;
  WeatherPlayHD *game = weatherPlay();
  WeatherCameraHD *pCamera = game->mpCamera;
  game = weatherPlay();
  s32 room = waveStayRoom.get();
  WeatherPlayerHD *pPlayer = game->mpPlayer;
  if (room >= 0) {
    game = weatherPlay();
    WeatherRoomDtHD *roomData = weatherRoomStatus(&game->mRoomControl, room);
    u32 target = roomData->__vtbl->getFileListInfo.get();
    WeatherFileListHD *info =
        call_ptr<WeatherFileListHD *>(target, roomData, target);
    if (info)
      sea = info->mSeaLevel;
  }
  if (waveStage("A_umikz", &work.mSeaStageName, &work.mUmikzStage))
    sea = 1e+02f;
  else if (waveStage("Opub", &work.mOpubName, &work.mOpubStage))
    sea = -9.4e+02f;
  else if (waveStage("Orichh", &work.mOrichName, &work.mOrichStage))
    sea = -1.4e+02f;
  else if (waveStage("Ojhous2", &work.mOjhousName, &work.mOjhousStage))
    sea = -1e+02f;
  else if (waveStage("LinkRM", &work.mLinkRoomName, &work.mLinkRoomStage))
    sea = -1e+02f;
  else if (waveStage("Onobuta", &work.mOnobutaName, &work.mOnobutaStage))
    sea = -1e+02f;
  game = weatherPlay();
  if (!game->mpView.get())
    return;
  game = weatherPlay();
  WeatherViewHD *view = game->mpView;
  weatherMatrixInverse(&view->mViewRotMtx, &work.mDrawMtx);
  env = weatherEnvironment();
  dKy_set_eyevect_calc2HD(pCamera, eye, (f32)env->mWaveSpawnDist, zero);
  WeatherVectorHD *windptr = dKyw_get_wind_vecHD();
  f32 power = dKyw_get_wind_powHD(), wz = (f32)windptr->z, wx = (f32)windptr->x,
      wy = (f32)windptr->y, side = 0.6f;
  if (!waveStage("Name", &work.mSeaStageName, &work.mNameStage)) {
    game = weatherPlay();
    StageInfoHD *info = game->mStage.getStagInfo();
    if (((info->mTypeFlags.get() >> 16) & 7) == 2) {
      s32 stageY = 0;
      if (waveStage("LinkRM", &work.mLinkRoomWindName,
                    &work.mLinkRoomWindStage))
        stageY = 0x4000;
      else if (waveStage("Orichh", &work.mOrichWindName, &work.mOrichWindStage))
        stageY = -0x4000;
      else if (waveStage("Ojhous2", &work.mOjhousWindName,
                         &work.mOjhousWindStage))
        stageY = 0x7FFF;
      else if (waveStage("Omasao", &work.mOmasaoWindName,
                         &work.mOmasaoWindStage))
        stageY = -0x4000;
      else if (waveStage("Onobuta", &work.mOnobutaWindName,
                         &work.mOnobutaWindStage))
        stageY = 0x4000;
      WeatherSavedWindHD *save = waveSavedWind;
      s32 ax, ay;
      if ((s16)save->mWindX == -1 && (s16)save->mWindY == -1) {
        ax = 0;
        ay = stageY;
      } else {
        env = weatherEnvironment();
        ax = (u16)env->mTactWindAngleX;
        env = weatherEnvironment();
        ay = (s16)((s16)env->mTactWindAngleY + stageY);
      }
      WeatherTrigPairHD *tx = &waveWeatherTrig.entries[((u16)ax) >> 3];
      WeatherTrigPairHD *ty = &waveWeatherTrig.entries[((u16)ay) >> 3];
      wx = fmuls_ppc((f32)tx->cos, (f32)ty->cos);
      wy = (f32)tx->sin;
      wz = fmuls_ppc((f32)tx->cos, (f32)ty->sin);
      wind->x = fmuls_ppc(wx, side);
      wind->y = fmuls_ppc(wy, side);
      wind->z = fmuls_ppc(wz, side);
      power = side;
    }
  }
  doublePos->x = (f64)fsubs_ppc((f32)pCamera->mCenter.x, (f32)pCamera->mEye.x);
  doublePos->y = 0.0;
  doublePos->z = (f64)fsubs_ppc((f32)pCamera->mCenter.z, (f32)pCamera->mEye.z);
  vectle_calcHD(doublePos, normal);
  f32 cross = cM3d_VectorProduct2dHD(zero, zero, -wx, -wz, (f32)normal->x,
                                     (f32)normal->z);
  pPkt->mSkewDir = cross;
  f32 one = 1.0f,
      dot = fmadds(wx, (f32)normal->x, fmuls_ppc(wz, (f32)normal->z));
  pPkt->mSkewWidth =
      fmuls_ppc(fmuls_ppc(fmuls_ppc(fsubs_ppc(one, std::fabs(dot)), power),
                          fsubs_ppc(one, std::fabs(wy))),
                fmuls_ppc(std::fabs(cross), side));
  env = weatherEnvironment();
  s32 count = (s16)env->mWaveCount;
  if (count <= 0)
    return;
  f32 half = 0.5f, alphaFactor = 0.8f, bias = 0.2f, gain = 1.03f;
  work.mFadeMinimum = 0.001f;
  for (s32 i = 0; i < count; i++) {
    WaveEffHD *p = &pPkt->mEff[i];
    env = weatherEnvironment();
    u8 state = (u8)p->mStatus;
    if ((u8)env->mWaveReset) {
      p->mStatus = 0;
      state = 0;
    }
    if (state == 0) {
      p->mBasePos.x = (f32)eye->x;
      p->mBasePos.y = sea;
      p->mBasePos.z = (f32)eye->z;
      env = weatherEnvironment();
      p->mPos.x = cM_rndFX((f32)env->mWaveSpawnRadius);
      p->mPos.y = zero;
      env = weatherEnvironment();
      p->mPos.z = cM_rndFX((f32)env->mWaveSpawnRadius);
      p->mCounter = cM_rndF(65536.0f);
      p->mAlpha = zero;
      f32 random = cM_rndF(65535.0f);
      p->mStrengthEnv = one;
      p->mAngle = (u16)ftoi(random);
      WeatherEnvironmentHD *held = weatherEnvironment();
      env = weatherEnvironment();
      random = cM_rndF(fsubs_ppc(one, (f32)env->mWaveScaleRand));
      f32 scale = fadds_ppc((f32)held->mWaveScaleRand, random);
      p->mScale = scale;
      p->mSpeed = scale;
      f32 rate = fmadds(0.05f, fsubs_ppc(one, scale), 0.02f);
      env = weatherEnvironment();
      p->mCounterSpeed = fmuls_ppc(rate, (f32)env->mWaveCounterSpeedScale);
      p->mTimer = 0;
      p->mStatus = (u8)p->mStatus + 1;
      state = 1;
    }
    if (state == 1 || state == 2) {
      env = weatherEnvironment();
      f32 strength = (f32)p->mStrengthEnv, speedEnv = (f32)env->mWaveSpeed,
          windx = (f32)wind->x, speed = (f32)p->mSpeed, alpha = (f32)p->mAlpha;
      f32 motion = fmuls_ppc(fmuls_ppc(fmuls_ppc(windx, speedEnv), speed),
                             fmadds(strength, half, half)),
          af = fmadds(alpha, alphaFactor, bias);
      p->mPos.x = fmadds(motion, af, (f32)p->mPos.x);
      env = weatherEnvironment();
      strength = (f32)p->mStrengthEnv;
      speedEnv = (f32)env->mWaveSpeed;
      f32 windz = (f32)wind->z;
      speed = (f32)p->mSpeed;
      alpha = (f32)p->mAlpha;
      motion = fmuls_ppc(fmuls_ppc(fmuls_ppc(windz, speedEnv), speed),
                         fmadds(strength, half, half));
      af = fmadds(alpha, alphaFactor, bias);
      f32 z = fmadds(motion, af, (f32)p->mPos.z),
          counterSpeed = (f32)p->mCounterSpeed, counter = (f32)p->mCounter;
      p->mPos.z = z;
      p->mCounter = fadds_ppc(counter, counterSpeed);
      wavePosition(point, p);
      f32 d = waveDistance(point, eye);
      env = weatherEnvironment();
      if (d > (f32)env->mWaveSpawnRadius) {
        p->mBasePos.x = (f32)eye->x;
        p->mBasePos.z = (f32)eye->z;
        d = waveDistance(point, eye);
        env = weatherEnvironment();
        if (d > fadds_ppc((f32)env->mWaveSpawnRadius, 3.5e+02f)) {
          env = weatherEnvironment();
          p->mPos.x = cM_rndFX((f32)env->mWaveSpawnRadius);
          env = weatherEnvironment();
          p->mPos.z = cM_rndFX((f32)env->mWaveSpawnRadius);
        } else {
          get_vectle_calcHD(point, eye, direction);
          env = weatherEnvironment();
          p->mPos.x = fmuls_ppc((f32)direction->x, (f32)env->mWaveSpawnRadius);
          env = weatherEnvironment();
          p->mPos.z = fmuls_ppc((f32)direction->z, (f32)env->mWaveSpawnRadius);
        }
        p->mAlpha = zero;
      }
      wavePosition(point, p);
      p->mStrengthEnv = one;
      for (u32 j = 0; j < 10; j++) {
        WaveInfluenceHD *influence = env0->mpWaveInfl[j];
        if (!influence)
          continue;
        influence->mPos.y = (f32)point->y;
        influence = env0->mpWaveInfl[j];
        d = waveDistance(point, &influence->mPos);
        influence = env0->mpWaveInfl[j];
        f32 outer = (f32)influence->mOuterRadius,
            inner = (f32)influence->mInnerRadius;
        if (d < outer) {
          if (d < inner) {
            p->mStrengthEnv = zero;
            break;
          }
          f32 range = fsubs_ppc(outer, inner);
          if (range > zero) {
            f32 ratio = fsubs_ppc(d, inner) / range;
            if ((f32)p->mStrengthEnv > ratio)
              p->mStrengthEnv = ratio;
          } else
            p->mStrengthEnv = zero;
        }
      }
      env = weatherEnvironment();
      if ((f32)env->mWaveFlatInter > zero) {
        scratch->words[0] = (u32)pCamera->mEye.words[0];
        scratch->words[1] = (u32)pCamera->mEye.words[1];
        scratch->words[2] = (u32)pCamera->mEye.words[2];
        scratch->y = (f32)point->y;
        d = waveDistance(point, scratch);
        WeatherEnvironmentHD *held = weatherEnvironment();
        env = weatherEnvironment();
        f32 inner = fmuls_ppc((f32)held->mWaveFlatInter,
                              fmuls_ppc((f32)env->mWaveSpawnRadius, 1.5f)),
            range = fsubs_ppc(fadds_ppc(inner, 1e+03f), inner);
        if (range > zero) {
          f32 ratio = fsubs_ppc(d, inner) / range;
          if ((f32)p->mStrengthEnv > ratio)
            p->mStrengthEnv = ratio;
        } else
          p->mStrengthEnv = zero;
      }
      scratch->words[0] = (u32)pPlayer->mCurrentPos.words[0];
      scratch->words[1] = (u32)pPlayer->mCurrentPos.words[1];
      scratch->words[2] = (u32)pPlayer->mCurrentPos.words[2];
      scratch->y = (f32)point->y;
      d = waveDistance(point, scratch);
      if (d < 2e+03f) {
        if (d < 2e+02f)
          p->mStrengthEnv = zero;
        else
          p->mStrengthEnv =
              fmuls_ppc((f32)p->mStrengthEnv, fsubs_ppc(d, 2e+02f) / 1.8e+03f);
      }
      if (waveStage("ADMumi", &work.mAdmumiName, &work.mAdmumiStage)) {
        work.mShorelineCenter.x = -2.187e+04f;
        work.mShorelineCenter.y = 5.4e+02f;
        work.mShorelineCenter.z = 28619.0f;
        work.mShorelineZeroZ = zero;
        work.mShorelineThirdZ = -95962.0f;
        scratch->x = (f32)work.mShorelineCenter.x;
        scratch->y = (f32)point->y;
        scratch->z = (f32)work.mShorelineCenter.z;
        d = waveDistance(point, scratch);
        if (d < 5e+03f) {
          if (d < 3e+03f)
            p->mStrengthEnv = zero;
          else
            p->mStrengthEnv =
                fmuls_ppc((f32)p->mStrengthEnv, fsubs_ppc(d, 3e+03f) / 2e+03f);
        }
        scratch->x = zero;
        scratch->y = (f32)point->y;
        scratch->z = (f32)work.mShorelineZeroZ;
        d = waveDistance(point, scratch);
        if (d < 1.1e+04f) {
          if (d < 8e+03f)
            p->mStrengthEnv = zero;
          else
            p->mStrengthEnv =
                fmuls_ppc((f32)p->mStrengthEnv, fsubs_ppc(d, 8e+03f) / 3e+03f);
        }
        scratch->x = -5.06e+04f;
        scratch->y = (f32)point->y;
        scratch->z = (f32)work.mShorelineThirdZ;
        d = waveDistance(point, scratch);
        if (d < 8e+03f) {
          if (d < 5e+03f)
            p->mStrengthEnv = zero;
          else
            p->mStrengthEnv =
                fmuls_ppc((f32)p->mStrengthEnv, fsubs_ppc(d, 5e+03f) / 3e+03f);
        }
      }
    } else if (state == 3)
      p->mStatus = 0;
    wavePosition(finalpos, p);
    f32 d = waveDistance(finalpos, &pCamera->mEye);
    if (!(d >= 0))
      d = zero;
    env = weatherEnvironment();
    f32 radius = (f32)env->mWaveSpawnDist,
        alpha = fsubs_ppc(one, d / fadds_ppc(radius, radius)),
        angle = (f32)p->mCounter;
    alpha = fmuls_ppc(alpha, gain);
    f32 sine = weatherSinRadians(angle);
    alpha = fmuls_ppc(alpha, sine);
    if (alpha > one)
      alpha = one;
    else if (!(alpha >= 0))
      alpha = zero;
    weatherAddCalc(&p->mAlpha, alpha, half, half, (f32)work.mFadeMinimum);
    p->mBasePos.y = sea;
    env = weatherEnvironment();
    count = (s16)env->mWaveCount;
  }
}
/* WWHD glue (tools/verify/autoglue.py)
data waveSavedWind 101F84DC
data waveStayRoom 1047E6C8
data waveSafeStringVtable 1004F3AC
data waveWeatherTrig 104A44F8
str 1004F878 "A_umikz"
str 1004F890 "Opub"
str 1004F898 "Orichh"
str 1004F880 "Ojhous2"
str 1004F8A0 "LinkRM"
str 1004F888 "Onobuta"
str 1004F8A8 "Name"
str 1004F8B0 "Omasao"
str 1004F8B8 "ADMumi"
ext 02555D0C WeatherEnvironmentHD *weatherEnvironment()
ext 025200D4 WeatherPlayHD *weatherPlay()
ext 0257DB28 void dKyw_get_wind_vecpowHD(WeatherVectorHD *)
ext 025C11DC WeatherRoomDtHD* weatherRoomStatus(WeatherRoomControlHD*, s32)
ext 028E91EC void weatherMatrixInverse(WeatherMatrixHD*,WeatherMatrixHD*)
ext 0256401C void dKy_set_eyevect_calc2HD(WeatherCameraHD *, WeatherVectorHD *, f32, f32)
ext 0257DAA8 WeatherVectorHD *dKyw_get_wind_vecHD()
ext 02578348 f32 dKyw_get_wind_powHD()
ext 02563E48 void vectle_calcHD(WeatherDoublePosHD *, WeatherVectorHD *)
ext 02010CFC f32 cM3d_VectorProduct2dHD(f32, f32, f32, f32, f32, f32)
ext 02563F0C void get_vectle_calcHD(WeatherVectorHD *, WeatherVectorHD *, WeatherVectorHD *)
ext 028F43F8 f32 weatherSinRadians(f32)
ext 0200ECD4 void weatherAddCalc(be<f32> *, f32, f32, f32, f32)
ext 028E8DE8 void weatherDistanceForward(WeatherVectorHD*, WeatherVectorHD*)
ext 028F4384 f32 weatherSqrtForward()
*/
