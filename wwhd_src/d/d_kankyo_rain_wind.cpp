/* Wind lines and inlined seabirds on the HD weather layout. */
#include "d/d_kankyo_rain_weather.h"
using namespace weather_move;
using namespace gabi;
using WeatherWordHD = be<u32>;
using ParticleListPointerHD = gptr<WeatherParticleListHD>;
static WeatherWordHD windFrameCounter;
static ParticleListPointerHD windParticleList;
static WeatherWordHD windAllGroupVtable;
static WeatherWordHD windAllCheckVtable;
static WeatherWordHD windAllPolyInfoVtable;
static WeatherWordHD windAllPolyPassVtable;
static WeatherWordHD windBasePolyInfoVtable;
static WeatherWordHD windBasePolyPassVtable;
static WeatherWordHD windBaseGroupPassVtable;
static SafeStringVtableHD windSafeStringVtable;
namespace weather_move {
static WeatherTrigHD windWeatherTrig;
static f32 weatherSin(u32 angle) {
  return windWeatherTrig.entries[(u16)angle >> 3].sin;
}
static f32 weatherCos(u32 angle) {
  return windWeatherTrig.entries[(u16)angle >> 3].cos;
}
} // namespace weather_move
static void windInvalidate(gptr<WeatherEmitterHD> *slot) {
  WeatherEmitterHD *p = *slot;
  weatherEmitterDelete(p, 1);
  p = *slot;
  u32 flags = p->mStatus;
  p->mMaxFrame = -1;
  p->mStatus = flags | 1;
  *slot = nullptr;
}
static void windScale(WeatherEmitterHD *emitter, f32 scale) {
  emitter->mGlobalScale.x = scale;
  emitter->mGlobalScale.y = scale;
  emitter->mGlobalScale.z = scale;
  emitter->mGlobalParticleScale.x = scale;
  emitter->mGlobalParticleScale.y = scale;
  emitter->mGlobalParticleScale.z = scale;
}
static void windTranslate(WeatherEmitterHD *emitter,
                          WeatherVectorHD *position) {
  f32 x = position->x, y = position->y, z = position->z;
  emitter->mGlobalTranslation.x = x;
  u32 flip = emitter->mVolumeType;
  emitter->mGlobalTranslation.y = y;
  emitter->mGlobalTranslation.z = z;
  if (flip >= 7)
    emitter->mGlobalTranslation.y = -(f32)emitter->mGlobalTranslation.y;
}
static void windTotal(WindEffHD *slot, WeatherVectorHD *pos) {
  pos->x = fadds_ppc(slot->mBasePos.x, slot->mPos.x);
  pos->y = fadds_ppc(slot->mBasePos.y, slot->mPos.y);
  pos->z = fadds_ppc(slot->mBasePos.z, slot->mPos.z);
}
static void windEmitterAlpha(WeatherEmitterHD *p, u8 alpha) {
  p->mGlobalAlpha = alpha;
}
// HD: camera motion scales wind speed, player event status selects ten lines,
// and custom wind changes scale/range. Seabird movement remains inlined in this
// entry. Emitter writes retain native order, volume-dependent Y inversion and
// both global scale triplets. Ground-query base vtables remain TU-specific.
/* 025642AC */
void dKyr_wind_move() {
  WindScratchHD scratch;
  // HD: keep GHS frame positions, including base-subobject aliases.
  WindWorkHD *work = at<WindWorkHD>(ea(&scratch) - 8);
  WeatherEnvironmentHD *env = weatherEnvironment();
  WindPacketHD *pWind = env->mpWind;
  WeatherCameraHD *pCamera = weatherPlay()->mpCamera;
  WeatherPlayerHD *pPlayer = weatherPlay()->mpPlayer;
  weatherGroundConstruct(&work->mGroundCheck);
  work->mpPolyVtable = &work->mGroundCheck.dBgS_PolyPassChk::__vtbl;
  work->mpGroupVtable = &work->mGroundCheck.dBgS_GrpPassChk::__vtbl;
  work->mGroundCheck.mbCamThrough = 0;
  work->mGroundCheck.mbLinkThrough = 0;
  work->mGroundCheck.mbArrowThrough = 0;
  work->mGroundCheck.mbBombThrough = 0;
  work->mGroundCheck.mbBoomerangThrough = 0;
  work->mGroundCheck.mbRopeThrough = 0;
  work->mGroundCheck.dBgS_GrpPassChk::__vtbl = ea(&windAllGroupVtable);
  work->mGroundCheck.mGrpPassChk =
      ea(&work->mGroundCheck.dBgS_GrpPassChk::__vtbl);
  work->mGroundCheck.cBgS_Chk::__vtbl = ea(&windAllCheckVtable);
  work->mGroundCheck.cBgS_PolyInfo::__vtbl = ea(&windAllPolyInfoVtable);
  work->mGroundCheck.mPolyPassChk =
      ea(&work->mGroundCheck.dBgS_PolyPassChk::__vtbl);
  work->mBasePolyInfoVtable = ea(&windBasePolyInfoVtable);
  work->mGroundCheck.mGrp = 15;
  work->mBaseGroupPassVtable = ea(&windBaseGroupPassVtable);
  work->mGroundCheck.mbObjThrough = 1;
  work->mBasePolyPassVtable = ea(&windBasePolyPassVtable);
  work->mGroundCheck.dBgS_PolyPassChk::__vtbl = ea(&windAllPolyPassVtable);
  WeatherVectorHD *direction = dKyw_get_wind_vecHD();
  f32 power = dKyw_get_wind_powHD();
  f32 windX = (f32)direction->x, windZ = (f32)direction->z,
      windY = (f32)direction->y;
  weatherPlay();
  s32 count = (u32)env->mWindlineCount;
  WeatherParticleListHD *particlesRoot = windParticleList;
  u32 particles =
      (u32)particlesRoot->mEndCount - (u32)particlesRoot->mBeginCount;
  f32 range = 2e+03f, spawnDistance = 4e+03f, offsetY = range, scale = 1.0f;
  work->mPosWindScale = 2.5e+03f;
  work->mStateStep = 0.15f;
  work->mOne = scale;
  work->mPhaseStep = range;
  work->mLoopChance = 0.2f;
  work->mSwayAmplitude = 2.5e+02f;
  f32 distance = weatherDistanceSquared(&pWind->mOldEyePosHD, &pCamera->mEye);
  distance = weatherSqrtFloat(distance);
  distance /= 1e+02f;
  if (distance > (f32)work->mOne)
    distance = (f32)work->mOne;
  f32 speed = fmadds(96.0f, distance, 8e+01f);
  pWind->mOldEyePosHD.words[0] = (u32)pCamera->mEye.words[0];
  pWind->mOldEyePosHD.words[1] = (u32)pCamera->mEye.words[1];
  pWind->mOldEyePosHD.words[2] = (u32)pCamera->mEye.words[2];
  work->mMoveSpeed = speed;
  if (weatherPlay()->mEventStatus.get() & WindlinePlayerEventHD)
    count = 10;
  f32 custom = (f32)env->mCustomWindPower, zero = 0.0f;
  u32 oldCustom = (u8)pWind->mbHasCustomWindPower;
  work->mZero = zero;
  work->mFadeDistance = 2e+02f;
  bool reset = false;
  if (custom > zero) {
    range = 1.6e+02f;
    offsetY = (f32)work->mFadeDistance;
    spawnDistance = 5e+01f;
    scale = 0.14f;
    count = 9;
    work->mSwayAmplitude = offsetY;
    work->mMoveSpeed = 8.0f;
    work->mPhaseStep = 1.5e+02f;
    work->mLoopChance = (f32)work->mZero;
    work->mPosWindScale = range;
    work->mStateStep = 0.18f;
    if (!oldCustom) {
      pWind->mbHasCustomWindPower = 1;
      reset = true;
    }
  } else if (oldCustom) {
    pWind->mbHasCustomWindPower = 0;
    reset = true;
  }
  if (reset)
    for (u32 i = 0; i < 30; i++) {
      WindEffHD *slot = &pWind->mWindEff[i];
      if (slot->mpEmitter.get())
        windInvalidate(&slot->mpEmitter);
      slot->mState = 0;
    }
  dKy_set_eyevect_calc2HD(pCamera, &work->mEyeVector, spawnDistance,
                          spawnDistance);
  work->mWindLengthXZSquared = fmadds(windX, windX, fmuls_ppc(windZ, windZ));
  const f32 half = 0.5f, epsilon = 0.01f, fifth = 0.2f, third = 0.3f,
            tenth = 0.1f, fourth = 0.4f, angleMaximum = 65535.0f,
            height = 1e+04f;
  work->mAlphaMinimum = 0.001f;
  work->mAlphaStep = 0.05f;
  f32 lineZero = (f32)work->mZero, one = (f32)work->mOne;
  bool enough = !(power < third);
  for (s32 i = 0; i < 30; i++) {
    WindEffHD *slot = &pWind->mWindEff[i];
    u32 state = (u32)slot->mState;
    if (i >= count && !state) {
      if (slot->mpEmitter.get())
        windInvalidate(&slot->mpEmitter);
      continue;
    }
    if (!state) {
      if (!enough || particles > 1500)
        continue;
      u32 tick = windFrameCounter.get();
      if (((tick >> 4) & 7) == ((u32)i & 3) && !(u8)pWind->mbHasCustomWindPower)
        continue;
      slot->mStateTimer = lineZero;
      slot->mAlpha = lineZero;
      slot->mSwerveAnimCounter = 0;
      WeatherVectorHD *base = pWind->mbHasCustomWindPower.get() == 1
                                  ? &pPlayer->mCurrentPos
                                  : &work->mEyeVector;
      f32 x = (f32)base->x, y = (f32)base->y, z = (f32)base->z;
      slot->mBasePos.x = x;
      slot->mBasePos.y = fadds_ppc(y, offsetY);
      slot->mBasePos.z = z;
      slot->mPos.x = cM_rndFX(range);
      slot->mPos.y = cM_rndFX(range);
      slot->mPos.z = cM_rndFX(range);
      f32 random = cM_rndF(one);
      f32 spread =
          fmadds((f32)work->mPosWindScale, random, (f32)work->mPosWindScale);
      x = fnmsubs(windX, spread, (f32)slot->mPos.x);
      y = fnmsubs(windY, spread, (f32)slot->mPos.y);
      z = fnmsubs(windZ, spread, (f32)slot->mPos.z);
      slot->mPos.x = x;
      slot->mPos.y = y;
      slot->mPos.z = z;
      random = cM_rndF(angleMaximum);
      slot->mSwerveAnimCounter = (u16)ftoi(random);
      windTotal(slot, &work->mPosition);
      if ((u8)pWind->mbHasCustomWindPower != 1) {
        reinterpret_cast<WeatherVectorHD *>(&work->mGroundCheck.m_pos)
            ->words[0] = (u32)work->mPosition.words[0];
        work->mGroundCheck.m_pos.y = fadds_ppc((f32)work->mPosition.y, height);
        reinterpret_cast<WeatherVectorHD *>(&work->mGroundCheck.m_pos)
            ->words[2] = (u32)work->mPosition.words[2];
        WeatherPlayHD *game = weatherPlay();
        f32 ground =
            weatherGroundCross(game->mBackgroundSystem, &work->mGroundCheck);
        if ((f32)work->mPosition.y < ground) {
          f32 raised = fadds_ppc(ground, offsetY);
          random = cM_rndF(offsetY);
          slot->mPos.y =
              fsubs_ppc(fadds_ppc(raised, random), (f32)slot->mBasePos.y);
        }
      }
      windTotal(slot, &work->mPosition);
      WeatherPlayHD *game = weatherPlay();
      WeatherEmitterHD *emitter =
          weatherParticleSet(game->mpParticle, 0, ID_AK_JN_WINDLINE00,
                             &work->mPosition, 0, 0, 255, 0, -1, 0, 0, 0);
      slot->mpEmitter = emitter;
      if (emitter) {
        emitter->mGlobalAlpha = 0;
        windScale(slot->mpEmitter.get(), scale);
        slot->mState = 1;
      }
      f32 horizontal = weatherSqrtFloat((f32)work->mWindLengthXZSquared);
      work->mFloatScratch = horizontal;
      u32 angle = weatherAtan2Angle(windX, windZ);
      slot->mAngleXZ = angle;
      angle = weatherAtan2Angle(windY, (f32)work->mFloatScratch);
      slot->mLoopCounter = 0;
      slot->mAngleY = angle;
      random = cM_rndF(one);
      slot->mLoopEnabled = !(random > (f32)work->mLoopChance);
      continue;
    }
    if (state == 3) {
      if (!(u8)pWind->mbHasCustomWindPower)
        slot->mState = 0;
      continue;
    }
    if (state > 3)
      continue;
    s32 oldAngle = (s16)slot->mSwerveAnimCounter, oldPitch = (s16)slot->mAngleY;
    u16 phase = (u16)ftoi(fmadds((f32)work->mPhaseStep, power, (f32)oldAngle));
    f32 deficit = fsubs_ppc(one, power);
    f32 sway = (f32)work->mSwayAmplitude;
    sway = fnmsubs(fmuls_ppc(sway, fifth), deficit, sway);
    slot->mSwerveAnimCounter = phase;
    slot->mAngleY = (u16)ftoi(fmadds(weatherSin(phase), sway, (f32)oldPitch));
    s32 yaw = (s16)slot->mAngleXZ;
    slot->mAngleXZ =
        (u16)ftoi(i & 1 ? fmadds(weatherSin((u16)slot->mSwerveAnimCounter),
                                 sway, (f32)yaw)
                        : fnmsubs(weatherSin((u16)slot->mSwerveAnimCounter),
                                  sway, (f32)yaw));
    if ((f32)slot->mStateTimer > fourth && (u8)slot->mLoopEnabled == 1) {
      s32 add = 3600 + i * 200;
      u32 counter = (u32)slot->mLoopCounter + add;
      slot->mLoopCounter = counter;
      slot->mAngleY = (s16)slot->mAngleY + add;
      if ((s32)counter > 60535)
        slot->mLoopEnabled = 0;
    } else {
      f32 horizontal = weatherSqrtFloat((f32)work->mWindLengthXZSquared);
      work->mFloatScratch = horizontal;
      u32 targetYaw = weatherAtan2Angle(windX, windZ);
      u32 targetPitch = weatherAtan2Angle(windY, (f32)work->mFloatScratch);
      weatherAddCalcAngle(&slot->mAngleY, targetPitch, 10, 1000, 1);
      weatherAddCalcAngle(&slot->mAngleXZ, targetYaw, 10, 1000, 1);
    }
    u32 pitch = (u16)slot->mAngleY, heading = (u16)slot->mAngleXZ;
    f32 moveX = fmuls_ppc(weatherCos(pitch), weatherSin(heading));
    work->mMoveDirection.x = moveX;
    work->mMoveDirection.y = weatherSin((u16)slot->mAngleY);
    work->mMoveDirection.z = fmuls_ppc(weatherCos((u16)slot->mAngleY),
                                       weatherCos((u16)slot->mAngleXZ));
    f32 fraction = (f32)(s16)slot->mSwerveAnimCounter / angleMaximum;
    f32 baseSpeed = (f32)work->mMoveSpeed;
    f32 effective = fnmsubs(fmuls_ppc(baseSpeed, fifth), deficit, baseSpeed);
    if (fraction > one)
      fraction = one;
    else
      fraction = fraction >= 0 ? fraction : lineZero;
    effective = fmadds(fmuls_ppc(effective, third), fraction, effective);
    slot->mPos.x = fmadds(moveX, effective, (f32)slot->mPos.x);
    slot->mPos.y =
        fmadds((f32)work->mMoveDirection.y, effective, (f32)slot->mPos.y);
    slot->mPos.z =
        fmadds((f32)work->mMoveDirection.z, effective, (f32)slot->mPos.z);
    windTotal(slot, &work->mPosition);
    windTranslate(slot->mpEmitter.get(), &work->mPosition);
    f32 step = fmadds(fmuls_ppc((f32)work->mStateStep, tenth), (f32)(i / 30),
                      (f32)work->mStateStep);
    f32 distanceToEye =
        weatherDistanceSquared(&work->mPosition, &pCamera->mEye);
    distanceToEye = weatherSqrtFloat(distanceToEye);
    distanceToEye /= (f32)work->mFadeDistance;
    if (distanceToEye > one)
      distanceToEye = one;
    u32 red = (u8)env->mBG0_K0[0], green = (u8)env->mBG0_K0[1],
        blue = (u8)env->mBG0_K0[2];
    f32 color = fmuls_ppc((f32)(red + green + blue), 0.33333334f) / 255.0f;
    color = fmuls_ppc(color, color);
    f32 alpha = fmuls_ppc(power, fmuls_ppc(distanceToEye, color));
    work->mFloatScratch = alpha;
    if (alpha < half)
      work->mFloatScratch = half;
    weatherVectorScale(&work->mMoveDirection, &work->mGroundOffset, 2.5e+02f);
    weatherVectorAdd(&work->mPosition, &work->mGroundPosition,
                     &work->mGroundOffset);
    reinterpret_cast<WeatherVectorHD *>(&work->mGroundCheck.m_pos)->words[0] =
        (u32)work->mGroundPosition.words[0];
    work->mGroundCheck.m_pos.y =
        fadds_ppc((f32)work->mGroundPosition.y, height);
    reinterpret_cast<WeatherVectorHD *>(&work->mGroundCheck.m_pos)->words[2] =
        (u32)work->mGroundPosition.words[2];
    WeatherPlayHD *game = weatherPlay();
    f32 ground =
        weatherGroundCross(game->mBackgroundSystem, &work->mGroundCheck);
    if (ground > (f32)work->mPosition.y) {
      step = fmadds((f32)i, epsilon, fifth);
      slot->mState = 2;
    }
    windEmitterAlpha(slot->mpEmitter,
                     (u8)ftoi(fmuls_ppc(fmuls_ppc((f32)slot->mAlpha, 255.0f),
                                        (f32)work->mFloatScratch)));
    if ((u32)slot->mState == 1) {
      weatherAddCalc(&slot->mStateTimer, one, third, fmuls_ppc(step, tenth),
                     epsilon);
      f32 timer = (f32)slot->mStateTimer;
      if (!(timer < one)) {
        slot->mState = 2;
        timer = (f32)slot->mStateTimer;
      }
      if (timer > half)
        weatherAddCalc(&slot->mAlpha, one, half, (f32)work->mAlphaStep,
                       (f32)work->mAlphaMinimum);
    } else {
      weatherAddCalc(&slot->mStateTimer, lineZero, fourth,
                     fmuls_ppc(step, fmadds((f32)i, epsilon, tenth)), epsilon);
      f32 timer = (f32)slot->mStateTimer;
      if (!(timer > lineZero)) {
        windInvalidate(&slot->mpEmitter);
        slot->mState = 0;
        u32 custom = (u8)pWind->mbHasCustomWindPower;
        timer = (f32)slot->mStateTimer;
        if (custom == 1)
          slot->mState = 4;
      }
      if (timer < half)
        weatherAddCalc(&slot->mAlpha, lineZero, half, (f32)work->mAlphaStep,
                       (f32)work->mAlphaMinimum);
    }
  }
  // dKyr_kamome_move is inlined in this HD entry.
  env = weatherEnvironment();
  pWind = env->mpWind;
  pCamera = weatherPlay()->mpCamera;
  dKyw_get_wind_vecHD();
  work->mSeaName.mStringTop = at<be<u8>>(guest_addr("sea"));
  work->mSeaName.__vtbl = &windSafeStringVtable;
  WeatherPlayHD *game = weatherPlay();
  work->mStageName.mStringTop = game->mStageName;
  work->mStageName.__vtbl = &windSafeStringVtable;
  work->mSeaName.assureTermination();
  work->mSeaName.assureTermination();
  be<u8> *name = work->mSeaName.mStringTop;
  work->mStageName.assureTermination();
  bool sea = name == work->mStageName.mStringTop.get();
  if (!sea) {
    be<u8> *a = work->mSeaName.mStringTop;
    be<u8> *b = work->mStageName.mStringTop;
    for (u32 j = 0; j < 0x40001; j++) {
      u8 left = a[j], right = b[j];
      if (left != right)
        break;
      if (!left) {
        sea = true;
        break;
      }
    }
  }
  bool spawn = false;
  if (sea) {
    bool bad = false;
    WeatherEnvironmentHD *e = weatherEnvironment();
    if ((u8)e->mColpatCurr == 1) {
      e = weatherEnvironment();
      bad = (f32)e->mColPatBlend > (f32)work->mZero;
    }
    if (!bad) {
      e = weatherEnvironment();
      if ((u8)e->mColpatPrev == 1) {
        e = weatherEnvironment();
        bad = (f32)e->mColPatBlend < (f32)work->mOne;
      }
    }
    if (!bad) {
      e = weatherEnvironment();
      if ((u8)e->mColpatCurr == 2) {
        e = weatherEnvironment();
        bad = (f32)e->mColPatBlend > (f32)work->mZero;
      }
    }
    if (!bad) {
      e = weatherEnvironment();
      if ((u8)e->mColpatPrev == 2) {
        e = weatherEnvironment();
        bad = (f32)e->mColPatBlend < (f32)work->mOne;
      }
    }
    spawn = !bad;
  }
  f32 birdZero = (f32)work->mZero, birdOne = (f32)work->mOne,
      negativeMaximum = -1e+02f, radius = 7e+03f, vertical = 3.2e+03f,
      maximum = 1e+02f, minimum = 8e+01f, negativeMinimum = -8e+01f,
      fadeFactor = 0.1f, fadeStep = 0.003f, fadeMinimum = 1e-06f,
      birdScale = 1.6f, birdDelay = 3e+02f, resetDelay = 9e+02f,
      resetSpread = 6e+02f, angleRange = 65535.0f;
  for (u32 i = 0; i < 2; i++) {
    KamomeEffHD *bird = &pWind->mKamomeEff[i];
    u32 state = (u8)bird->mStatus;
    if (state == 0) {
      if (spawn) {
        s32 timer = (s16)bird->mTimer;
        if (!timer) {
          bird->mAngleY = (u16)ftoi(cM_rndFX(angleRange));
          f32 r = cM_rndFX(angleRange);
          u32 angle = (u16)bird->mAngleY;
          bird->mAngleX = (u16)ftoi(r);
          f32 x = fmadds(weatherSin(angle), radius, (f32)pCamera->mEye.x),
              z = fmadds(weatherCos(angle), radius, (f32)pCamera->mEye.z);
          bird->mPos.y = 4.5e+03f;
          bird->mPos.x = x;
          bird->mPos.z = z;
          bird->mAngleYSpeed = cM_rndFX(birdOne);
          bird->mScale = birdZero;
          r = cM_rndF(1.8e+02f);
          bird->mTimer = (u16)ftoi(fadds_ppc(r, birdDelay));
          game = weatherPlay();
          WeatherEmitterHD *emitter =
              weatherParticleSet(game->mpParticle, 0, ID_AK_JN_KAMOME00,
                                 &bird->mPos, 0, 0, 255, 0, -1, 0, 0, 0);
          u8 old = (u8)bird->mStatus;
          bird->mpEmitter = emitter;
          bird->mStatus = old + 1;
        } else
          bird->mTimer = timer - 1;
      }
    } else if (state == 1 && bird->mpEmitter.get()) {
      f32 old = (f32)bird->mAngleYSpeed, r = cM_rndFX(birdOne),
          speed = fadds_ppc(old, r);
      if (old < birdZero) {
        if (speed < negativeMaximum)
          speed = negativeMaximum;
        else if (speed > negativeMinimum)
          speed = negativeMinimum;
      } else {
        if (speed > maximum)
          speed = maximum;
        else if (speed < minimum)
          speed = minimum;
      }
      bird->mAngleYSpeed = speed;
      work->mOldBirdTarget.x =
          fmuls_ppc(weatherSin((u16)bird->mAngleY), radius);
      work->mOldBirdTarget.y =
          std::fabs(fmuls_ppc(weatherSin((u16)bird->mAngleX), vertical));
      work->mOldBirdTarget.z =
          fmuls_ppc(weatherCos((u16)bird->mAngleY), radius);
      s32 angle = (s16)bird->mAngleY;
      u16 heading = (u16)ftoi(
          fmadds((f32)bird->mAngleYSpeed, (f32)bird->mScale, (f32)angle));
      s32 pitch = (s16)bird->mAngleX;
      bird->mAngleY = heading;
      bird->mAngleX = pitch + 15;
      f32 x = fmuls_ppc(weatherSin(heading), radius),
          y = std::fabs(fmuls_ppc(weatherSin((u16)bird->mAngleX), vertical)),
          z = fmuls_ppc(weatherCos((u16)bird->mAngleY), radius);
      work->mNewBirdTarget.x = x;
      work->mNewBirdTarget.y = y;
      work->mNewBirdTarget.z = z;
      y = fadds_ppc(y, 4.8e+03f);
      x = fadds_ppc((f32)pCamera->mEye.x, x);
      z = fadds_ppc((f32)pCamera->mEye.z, z);
      bird->mPos.y = y;
      bird->mPos.x = x;
      bird->mPos.z = z;
      work->mBirdPosition.x = x;
      work->mBirdPosition.y = y;
      work->mBirdPosition.z = z;
      windTranslate(bird->mpEmitter.get(), &work->mBirdPosition);
      if ((s16)bird->mTimer && spawn) {
        weatherAddCalc(&bird->mScale, birdOne, fadeFactor, fadeStep,
                       fadeMinimum);
        bird->mTimer = (s16)bird->mTimer - 1;
      } else {
        weatherAddCalc(&bird->mScale, birdZero, fadeFactor, fadeStep,
                       fadeMinimum);
        if ((f32)bird->mScale < 0.001f)
          bird->mStatus = (u8)bird->mStatus + 1;
      }
      dKyr_get_vectle_calcHD(&work->mOldBirdTarget, &work->mNewBirdTarget,
                             &work->mBirdTargetRotation);
      u32 rotation = weatherAtan2Angle((f32)work->mBirdTargetRotation.x,
                                       (f32)work->mBirdTargetRotation.z);
      weatherRotationMatrix(0, rotation, 0, &bird->mpEmitter->mGlobalRotation);
    } else if (state == 2) {
      windInvalidate(&bird->mpEmitter);
      bird->mStatus = 0;
      f32 r = cM_rndF(resetSpread);
      bird->mTimer = (u16)ftoi(fadds_ppc(r, resetDelay));
    }
    if (bird->mpEmitter.get()) {
      game = weatherPlay();
      if (game->mpView.get()) {
        game = weatherPlay();
        WeatherViewHD *view = game->mpView;
        f32 fov = (f32)view->mFovy / 26.0f;
        f32 scale = (f32)bird->mScale;
        if (!(fov < birdOne))
          fov = birdOne;
        windScale(bird->mpEmitter.get(),
                  fmuls_ppc(fmuls_ppc(scale, fov), birdScale));
      }
      WeatherEmitterHD *emitter = bird->mpEmitter;
      u32 red = (u8)weatherEnvironment()->mBG0_K0[0];
      u32 green = (u8)weatherEnvironment()->mBG0_K0[1];
      u32 blue = (u8)weatherEnvironment()->mBG0_K0[2];
      emitter->mGlobalPrmColor[0] = red;
      emitter->mGlobalPrmColor[1] = green;
      emitter->mGlobalPrmColor[2] = blue;
    }
  }
  work->mGroundCheck.cBgS_PolyInfo::__vtbl = work->mBasePolyInfoVtable.get();
  be<u32> *first = work->mpPolyVtable;
  be<u32> *second = work->mpGroupVtable;
  u32 a = work->mBasePolyPassVtable, b = work->mBaseGroupPassVtable;
  *first = a;
  *second = b;
  weatherGroundDestruct(&work->mGroundCheck, 0);
}
/* WWHD glue (tools/verify/autoglue.py)
str 1004F3A8 "sea"
data windFrameCounter 101FF558
data windParticleList 1047B2D4
data windAllGroupVtable 1004F494
data windAllCheckVtable 1004F474
data windAllPolyInfoVtable 1004F484
data windAllPolyPassVtable 1004F4A4
data windBasePolyInfoVtable 1004F404
data windBasePolyPassVtable 1004F424
data windBaseGroupPassVtable 1004F3E4
data windSafeStringVtable 1004F3AC
data windWeatherTrig 104A44F8
ext 02555D0C WeatherEnvironmentHD *weatherEnvironment()
ext 025200D4 WeatherPlayHD *weatherPlay()
ext 02008E0C void weatherGroundConstruct(dBgS_GndChk*)
ext 02008DAC void weatherGroundDestruct(dBgS_GndChk*,s32)
ext 0257DAA8 WeatherVectorHD *dKyw_get_wind_vecHD()
ext 02578348 f32 dKyw_get_wind_powHD()
ext 0256401C void dKy_set_eyevect_calc2HD(WeatherCameraHD *, WeatherVectorHD *, f32, f32)
ext 028E8DE8 f32 weatherDistanceSquared(WeatherVectorHD *, WeatherVectorHD *)
ext 028F4384 f32 weatherSqrtFloat(f32)
ext 02008974 f32 weatherGroundCross(void*, dBgS_GndChk*)
ext 025A847C WeatherEmitterHD* weatherParticleSet(WeatherParticleControllerHD*,u32,u32,WeatherVectorHD*,u32,u32,u32,u32,s32,u32,u32,u32)
ext 020195B0 u32 weatherAtan2Angle(f32,f32)
ext 0200F378 void weatherAddCalcAngle(be<s16>*,u32,s32,s32,s32)
ext 0201AE48 void weatherVectorScale(WeatherVectorHD*,WeatherVectorHD*,f32)
ext 0201AD78 void weatherVectorAdd(WeatherVectorHD*,WeatherVectorHD*,WeatherVectorHD*)
ext 0200ECD4 void weatherAddCalc(be<f32> *, f32, f32, f32, f32)
ext 02563F64 void dKyr_get_vectle_calcHD(WeatherVectorHD*, WeatherVectorHD*, WeatherVectorHD*)
ext 028245AC void weatherRotationMatrix(u32,u32,u32,WeatherMatrixHD*)
ext 0281DE68 void weatherEmitterDelete(WeatherEmitterHD*,u32)
*/
