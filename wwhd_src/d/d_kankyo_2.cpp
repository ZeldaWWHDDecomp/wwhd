/* d_kankyo: environment / lighting core, part 2 of 6 (025560C0..02557228), WWHD.
 * Effect lights, point light registration, fog distances, the vrbox addcol setters, time of
 * day, the HD colour conversion helpers and the dice weather. See d_kankyo.cpp for the TU. */
#include "d/d_kankyo_2.h"
using namespace d_kankyo_part2;

namespace d_kankyo_cpp {

f32 fl_data_ratio_set(f32 a, f32 b, f32 ratio);

/* 025560C0: HD: float_kankyo_color_ratio_set without the all-colour ratio and the clamp */
/* 025560C0 */
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4,
                                         f32 p5, f32 p6, f32 p7) {
  f32 a = fl_data_ratio_set(p0, p1, p2);
  f32 b = fl_data_ratio_set(p3, p4, p2);
  f32 c = fl_data_ratio_set(a, b, p5);
  return gabi::fmadds(p6 - c, p7, c);
}

/* 02556108: dKy_eflight_influence_id(cXyz pos, int nth); effect lights mpEfLights[10] at +0x790 */
/* 02556108 */
s32 dKy_eflight_influence_id(u32 pos, s32 nth) {
  const f32 far = constants()->farDistance;
  s32 first = -1, second = -1;
  f32 best = far;
  for (s32 i = 0; i <= nth; i++) {
    const f32 minPower = constants()->minPower;
    for (s32 j = 0; j < 10; j++) {
      if (environment()->mpEfLights[j] == 0)
        continue;
      if (i != 0 && (u32)j == (u32)first)
        continue;
      f32 d = squareRoot(squareDistance(pos, environment()->mpEfLights[j]));
      if (!(best > d))
        continue;
      if (!(view<LightInfluence>(environment()->mpEfLights[j])->mPower >
            minPower))
        continue;
      best = squareRoot(squareDistance(pos, environment()->mpEfLights[j]));
      if (i == 0)
        first = j;
      else
        second = j;
    }
    best = far;
  }
  if (nth == 0)
    second = first;
  return second;
}

/* 02556288: dKy_eflight_influence_pos(int) returning cXyz (r3 = result storage; HD allocates when NULL) */
/* 02556288 */
void dKy_eflight_influence_pos(u32 ret, s32 idx) {
  if (idx < 0)
    idx = 0;
  u32 l = environment()->mpEfLights[idx];
  if (ret == 0) {
    ret = allocateVector();
    if (ret == 0)
      return;
  }
  view<cXyz>(ret)->x = (f32)view<cXyz>(l)->x;
  view<cXyz>(ret)->y = (f32)view<cXyz>(l)->y;
  view<cXyz>(ret)->z = (f32)view<cXyz>(l)->z;
}

/* 02556304 */
f32 dKy_eflight_influence_power(s32 idx) {
  if (idx < 0)
    idx = 0;
  return view<LightInfluence>(environment()->mpEfLights[idx])->mPower;
}

/* 0255634C */
f32 dKy_eflight_influence_yuragi(s32 idx) {
  if (idx < 0)
    idx = 0;
  return view<LightInfluence>(environment()->mpEfLights[idx])->mFluctuation;
}

/* 02556394 */
f32 dKy_eflight_influence_distance(u32 pos, s32 idx) {
  if (idx < 0)
    idx = 0;
  return squareRoot(squareDistance(pos, environment()->mpEfLights[idx]));
}

/* 025563F0: plight_init: mLightInfluence[0].mPower (+0x4C), mpPLights[200] (+0x470), mpEfLights[10]
 * (+0x790), mpWaveInfl[10] (+0xAEC), mPlayerPLightIdx (+0x1074), mPlayerEflightIdx (+0x1078) */
/* 025563F0 */
void plight_init() {
  Environment *env = environment();
  env->mLightInfluence[0].mPower = (f32)constants()->initialPower;
  for (u32 i = 0; i < 200; i++)
    environment()->mpPLights[i] = 0;
  for (u32 i = 0; i < 10; i++)
    environment()->mpEfLights[i] = 0;
  for (u32 i = 0; i < 10; i++)
    environment()->mpWaveInfl[i] = 0;
  environment()->mPlayerPLightIdx = -1;
  environment()->mPlayerEflightIdx = -1;
}

/* 025564B4 */
void dKy_plight_set(u32 light) {
  for (u32 i = 0; i < 200; i++)
    if (environment()->mpPLights[i] == light)
      return;
  for (u32 i = 0; i < 200; i++) {
    if (environment()->mpPLights[i] == 0) {
      environment()->mpPLights[i] = light;
      view<LightInfluence>(environment()->mpPLights[i])->mIdx = i + 1;
      return;
    }
  }
}

/* 02556588: plight_set: stage point lights (HD entries 0x1C: pos, radius +0xC, colour +0x18,
 * fluctuation u8 +0x1B) into mLightInfluence[30] (+0x38, 0x24 each) */
/* 02556588 */
void plight_set() {
  s32 idx = 0;
  StageData *sd = stageData();
  u32 info = sd->getPlightInfo();
  if (info == 0)
    return;
  environment()->mpPlightInfo = info;
  s32 i = 0;
  sd = stageData();
  if (!(i < sd->getPlightNumInfo()))
    return;
  const f32 scale = constants()->stageRadiusScale;
  for (;;) {
    if (idx < 30) {
      Environment *env = environment();
      env->mLightInfluence[idx].mPos.x =
          (f32)view<StageLight>(info)->position.x;
      env = environment();
      env->mLightInfluence[idx].mPos.y =
          (f32)view<StageLight>(info)->position.y;
      env = environment();
      env->mLightInfluence[idx].mPos.z =
          (f32)view<StageLight>(info)->position.z;
      env = environment();
      env->mLightInfluence[idx].mColor.r = view<StageLight>(info)->r;
      env = environment();
      env->mLightInfluence[idx].mColor.g = view<StageLight>(info)->g;
      env = environment();
      env->mLightInfluence[idx].mColor.b = view<StageLight>(info)->b;
      env = environment();
      env->mLightInfluence[idx].mPower =
          (f32)view<StageLight>(info)->radius * scale;
      env = environment();
      env->mLightInfluence[idx].mFluctuation =
          (f32)view<StageLight>(info)->fluctuation;
      dKy_plight_set(gabi::ea(&environment()->mLightInfluence[idx]));
      idx++;
    }
    info += 0x1C;
    i++;
    sd = stageData();
    if (!(i < sd->getPlightNumInfo()))
      return;
  }
}

/* 0255671C: mFogStartZ +0xFD8, mFogEndZ +0xFDC, mFogRatio +0xFE0 */
/* 0255671C */
void dKy_fog_startendz_set(f32 startZ, f32 endZ, f32 ratio) {
  const f32 zero = constants()->zero;
  if (ratio < zero || ratio > constants()->one) {
    warnFogRatio(); /* OSReport_Warning("\ndKy_fog_startendz_set ratio error!\n") */
    ratio = zero;
  } else if (ratio < constants()->tinyRatio) {
    ratio = zero;
  }
  environment()->mFogStartZ = startZ;
  environment()->mFogEndZ = endZ;
  environment()->mFogRatio = ratio;
}

static inline void addcol_set2(GXColorS10 Environment::*color, s32 r, s32 g,
                               s32 b, f32 factor) {
  Environment *env = environment();
  (env->*color).r = (s16)gabi::ftoi(signedToFloat(r) * factor);
  env = environment();
  (env->*color).g = (s16)gabi::ftoi(signedToFloat(g) * factor);
  env = environment();
  (env->*color).b = (s16)gabi::ftoi(signedToFloat(b) * factor);
}

/* 025567FC */
void dKy_vrbox_addcol_sky0_set(s32 r, s32 g, s32 b, f32 factor) {
  addcol_set2(&Environment::mVrboxAddColSky0, r, g, b, factor);
}

/* 02556910 */
void dKy_vrbox_addcol_kasumi_set(s32 r, s32 g, s32 b, f32 factor) {
  addcol_set2(&Environment::mVrboxAddColKasumi, r, g, b, factor);
}

/* 02556A24 */
void dKy_addcol_fog_set(s32 r, s32 g, s32 b, f32 factor) {
  addcol_set2(&Environment::mAddColFog, r, g, b, factor);
}

/* 02556B38 */
void dKy_vrbox_addcol_set(s32 r, s32 g, s32 b, f32 factor) {
  dKy_vrbox_addcol_sky0_set(r, g, b, factor);
  dKy_vrbox_addcol_kasumi_set(r, g, b, factor);
  dKy_addcol_fog_set(r, g, b, factor);
}

/* 02556BC0: ENDLESS_NIGHT (0x0A02) and not dSymbol_NAYRU */
/* 02556BC0 */
BOOL dKy_checkEventNightStop() {
  if (eventFlag(save()->event, EndlessNight) && !hasSymbol(save()->inventory))
    return TRUE;
  return FALSE;
}

/* 02556C34 */
s32 dKy_getdaytime_hour() {
  f32 k = constants()->degreesPerHour;
  return gabi::ftoi((f32)save()->time / k);
}

/* 02556C68 */
s32 dKy_getdaytime_minute() {
  f32 m = constants()->farDistance;
  s32 t = gabi::ftoi((f32)save()->time * m);
  f32 tmp = signedToFloat(t % 15000000) / m;
  return gabi::ftoi(tmp / constants()->degreesPerHour *
                    constants()->minutesPerHour);
}

/* 02556D14 */
s32 dKy_daynight_check() {
  s32 hour = dKy_getdaytime_hour();
  return (u32)(hour - 6) >= 12 ? 1 : 0;
}

/* 02556D44: HD: copy of a 12-float structure through FPRs (lfs quiets signalling NaNs) */
/* 02556D44 */
void copy_f32x12(u32 dst, u32 src) {
  f32 v[12];
  for (u32 i = 0; i < 12; i++)
    v[i] = view<FloatMatrix>(src)->elements[i];
  for (u32 i = 0; i < 12; i++)
    view<FloatMatrix>(dst)->elements[i] = v[i];
}

/* 02556DE4: HD: GXColor (u8 RGBA) -> 4 floats / 255 */
/* 02556DE4 */
void GXColor_to_f32(u32 dst, u32 src) {
  f32 k = constants()->colorScale;
  f32 r = (f32)view<GXColor>(src)->r / k, g = (f32)view<GXColor>(src)->g / k,
      b = (f32)view<GXColor>(src)->b / k, a = (f32)view<GXColor>(src)->a / k;
  view<FloatColor>(dst)->r = r;
  view<FloatColor>(dst)->g = g;
  view<FloatColor>(dst)->b = b;
  view<FloatColor>(dst)->a = a;
}

/* 02556E98: HD: GXColorS10 (s16 RGBA) -> 4 floats / 255 */
/* 02556E98 */
void GXColorS10_to_f32(u32 dst, u32 src) {
  f32 k = constants()->colorScale;
  f32 r = signedToFloat(view<GXColorS10>(src)->r) / k,
      g = signedToFloat(view<GXColorS10>(src)->g) / k,
      b = signedToFloat(view<GXColorS10>(src)->b) / k,
      a = signedToFloat(view<GXColorS10>(src)->a) / k;
  view<FloatColor>(dst)->r = r;
  view<FloatColor>(dst)->g = g;
  view<FloatColor>(dst)->b = b;
  view<FloatColor>(dst)->a = a;
}

/* 02556F5C: mDiceWeatherMode +0x1093, mDiceWeatherTime +0xFC4 */
/* 02556F5C */
void dice_wether_init(u32 mode, f32 weatherTime, f32 curTime) {
  Environment *env = environment();
  f32 t = curTime + weatherTime;
  f32 k = constants()->dayCycle;
  env->mDiceWeatherMode = (u8)mode;
  if (!(t < k))
    t = t - k;
  env->mDiceWeatherTime = t;
}

/* 02556FE0: mDiceWeatherCounter +0x1068, mDiceWeatherState +0x1094 */
/* 02556FE0 */
void dice_wether_execute(u32 mode, f32 weatherTime, f32 curTime) {
  Environment *env = environment();
  if (mode == 0xFF) {
    u8 s = env->mDiceWeatherState;
    env->mDiceWeatherMode = 0;
    env->mDiceWeatherState = (u8)(s + 1);
    return;
  }
  f32 t = curTime + weatherTime;
  f32 k = constants()->dayCycle;
  env->mDiceWeatherMode = (u8)mode;
  u32 cnt = env->mDiceWeatherCounter;
  if (!(t < k))
    t = t - k;
  env->mDiceWeatherTime = t;
  env->mDiceWeatherCounter = cnt + 1;
}

/* 025570B4: mRainCount +0xA40 */
/* 025570B4 */
void dice_rain_minus() {
  Environment *env = environment();
  s32 n = (s32)env->mRainCount;
  if (n > 40) {
    n -= 3;
    env->mRainCount = (u32)n;
  } else if (n != 0) {
    n -= 1;
    env->mRainCount = (u32)n;
  }
  setRain(n); /* dKyw_rain_set */
}

s32 dKy_get_dayofweek();

/* 02557108: phantomship_wether (HD = GameCube USA: 285.0) */
/* 02557108 */
u8 phantomship_wether() {
  f32 t = (f32)save()->time;
  s32 weekday = dKy_get_dayofweek();
  if (t < constants()->ghostDayStart) {
    if (weekday != 0)
      weekday--;
    else
      weekday = 6;
  }
  u8 rt = 0;
  if (t > constants()->ghostNightStart || t < constants()->ghostNightEnd) {
    s32 roomNo = stayRoom();
    if ((roomNo == CrescentMoonIsland && weekday == 0) ||
        (roomNo == DiamondSteppeIsland && weekday == 1) ||
        (roomNo == BombIsland && weekday == 2) ||
        (roomNo == SpectacleIsland && weekday == 3) ||
        (roomNo == FiveStarIsles && weekday == 4) ||
        (roomNo == StarBeltArchipelago && weekday == 5) ||
        (roomNo == GreatfishIsle && weekday == 6))
      rt = 1;
  }
  return rt;
}

} // namespace d_kankyo_cpp

using namespace d_kankyo_cpp;

/* WWHD glue (tools/verify/autoglue.py): private data views, no TU static objects.
*/
