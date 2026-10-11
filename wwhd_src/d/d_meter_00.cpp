/* HUD hearts, magic and item controls. Verified HD behavior.  HD: native dispatch, removed J2D picture side effects, full
 * register widths and GHS fused/NaN arithmetic follow the verified HD prefix.
 */
#include "d/d_meter_00.h"
using namespace gabi;
using namespace meter00Item;
using namespace meter00Status;
using namespace meter00Stage;
static f32 sNowHeartScale;
static f32 texRateX;
static f32 texRateY;
static f32 texScaleX;
static f32 texScaleY;
static f32 sOne;
static f32 sZero;
static f32 sHeartWidth;
static f32 sHeartHeightScale;
static f32 sHeartRate;
static f32 sHalf;
static f32 sMagicSegments;
static f32 sLowLifeMagicY;
static f32 sMagicUnits;
static f32 sFlyGaugeWidth;
static f32 sAlphaPeak;
static f32 sAlphaSlope;
static f32 sAlphaFloor;
static f32 sMagicUpperBound;
static u8 sGaugeVisible;
using dMeterSavePointerHD = gptr<dMeterSaveHD>;
static dMeterSavePointerHD sSaveInfo;
static dMeterHeartTimersHD sHeartTimers;
static dMeterItemResourcesHD sItemResources;
static constexpr u32 sSafeStringVTable = 0x1005107C;
static constexpr const char *sBossDRC = "Xboss0";
static constexpr const char *sBossFW = "Xboss1";
static constexpr const char *sBossET = "Xboss2";
static constexpr const char *sBossWT = "Xboss3";
static constexpr const char *sEarthTemple = "Otkura";
static constexpr const char *sWindTemple = "Abesso";
dMeterGameInfoHD *meter00_getGameInfo();
u32 meter00_isBossEnemy(void *, u32);
u32 meter00_isEventBit(void *, u32);
void meter00_copyPlayerRecollectionData();
void meter00_onEventBit(void *, u32);
void meter00_cposMove(fopMsgM_pane_class *);
void meter00_paneTrans(fopMsgM_pane_class *, f32, f32);
void meter00_paneScaleXY(fopMsgM_pane_class *, f32);
void meter00_magicTransNowInitNative(dMeterPrefixHD *);
f32 meter00_valueIncrease(s32, s32, u32);
void meter00_heartScaleCountdownNative(u32);
void meter00_photoCheck(void *);
void meter00_loadTexture(void *, u32, u32, u32, void *);
void meter00_storeDataCache(void *, u32);
void meter00_setNowAlphaZero(fopMsgM_pane_class *);
void meter00_setHeartScaleNative(fopMsgM_pane_class *, u32);
void meter00_setNowHeartScaleNative(fopMsgM_pane_class *);
void meter00_magicInitTransNative(dMeterPrefixHD *);
void meter00_magicChangeNative(dMeterPrefixHD *, f32);
void meter00_maxLifeCountdownNative(u32);
void meter00_project(cXyz *, cXyz *);
void meter00_magicTransScaleNative(dMeterPrefixHD *, f32, f32, f32);
void meter00_itemUseCheckNative(dMeterPrefixHD *, u32);
static dMeterStageInfoHD *dMeter_getStageInfoHD() {
  dMeterStageAccessHD *stage = &meter00_getGameInfo()->mStage;
  return gabi::call_ptr<dMeterStageInfoHD *>(stage->__vtbl->getStagInfo, stage);
}

// HD: GHS nonfused multiply/subtract keeps this NaN sign and operand order.
static f32 dMeter_negativeMultiplySubtractHD(f32 a, f32 b, f32 c) {
  if (c != c)
    return -ppc_qnan(c);
  volatile f64 product = (f64)a * (f64)b;
  volatile f64 difference = product - (f64)c;
  return (f32)-difference;
}

// HD: SafeString termination dispatches and the bounded byte walk are inlined.
static bool meterSceneEqual(dMeterSafeStringHD *left,
                            dMeterSafeStringHD *right) {
  gabi::call_ptr<void>(left->__vtbl->assureTermination, left);
  gabi::call_ptr<void>(left->__vtbl->assureTermination, left);
  u32 saved = left->mText.v;
  gabi::call_ptr<void>(right->__vtbl->assureTermination, right);
  if (saved == right->mText.v)
    return true;
  const be<u8> *a = reinterpret_cast<const be<u8> *>(left->mText.get());
  const be<u8> *b = reinterpret_cast<const be<u8> *>(right->mText.get());
  for (u32 i = 0; i < 0x40001; i++) {
    u8 x = a[i], y = b[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}

/* 0259188C */
void dMeter_recollect_boss_data() {
  if ((((u32)dMeter_getStageInfoHD()->mStageFlags >> 16) & 7) !=
      dStageType_BOSS_e)
    return;
  if (!meter00_isBossEnemy(&sSaveInfo->mStageSave, 3u))
    return;
  const u32 types[] = {STAGE_DRC, STAGE_FW, STAGE_ET, STAGE_WT};
  const u32 flags[] = {UNK_3D80, UNK_3D40, UNK_3D20, UNK_3D10};
  for (u32 i = 0; i < 4; i++) {
    if (((u8)dMeter_getStageInfoHD()->mSaveTable >> 1) != types[i])
      continue;
    if (meter00_isEventBit(&sSaveInfo->mEvent, flags[i]))
      continue;
    meter00_copyPlayerRecollectionData();
    meter00_onEventBit(&sSaveInfo->mEvent, flags[i]);
    return;
  }
}

// HD: the signature retains the full s32 scale register.
/* 02591A64 */
void dMeter_setHeartScaleXY(fopMsgM_pane_class *pane, s32 level) {
  f32 width = (f32)pane->mSizeOrig.x, height = (f32)pane->mSizeOrig.y;
  f32 x, y;
  if (level == 4) {
    x = fmuls_ppc(width, (f32)texRateX);
    y = fmuls_ppc(height, (f32)texRateY);
  } else {
    f32 scale = (f32)sOne;
    x = fmuls_ppc(width, scale);
    y = fmuls_ppc(height, scale);
  }
  pane->mSize.x = x;
  pane->mSize.y = y;
  meter00_cposMove(pane);
}

/* 02591A3C */
void dMeter_setNowHeartScaleXY(fopMsgM_pane_class *pane) {
  f32 x = fmuls_ppc((f32)pane->mSizeOrig.x, (f32)sNowHeartScale);
  f32 height = (f32)pane->mSizeOrig.y;
  pane->mSize.x = x;
  pane->mSize.y = fmuls_ppc(height, (f32)sNowHeartScale);
  meter00_cposMove(pane);
}

/* 02591AB4 */
void dMeter_heartScaleCountdownHD(u32 index) {

  if ((u8)sHeartTimers.mScaleActive[index]) {
    u8 count = (u8)sHeartTimers.mCloseTimer[index];
    if (count)
      sHeartTimers.mCloseTimer[index] = count - 1;
    else
      sHeartTimers.mScaleActive[index] = 0;
  } else
    sHeartTimers.mOpenTimer[index] = 1;
}

/* 02591B00 */
void dMeter_maxLifeCountdownHD(u32 index) {

  if (!(u8)sHeartTimers.mScaleActive[index]) {
    u8 count = (u8)sHeartTimers.mOpenTimer[index];
    if (count)
      sHeartTimers.mOpenTimer[index] = count - 1;
    else
      sHeartTimers.mScaleActive[index] = 1;
  } else
    sHeartTimers.mCloseTimer[index] = 1;
}

// HD: the heart initialization loop includes the scale/light setup inline.
/* 02591B4C */
void dMeter_heartInit(dMeterPrefixHD *i_Meter) {
  texScaleX =
      (f32)((f64)(f32)i_Meter->mHeart[0].mSizeOrig.x / (f64)(f32)sHeartWidth);
  f32 half = (f32)sHalf, base = (f32)sHeartRate;
  texRateY = half;
  texRateX = base;
  texScaleY =
      fmuls_ppc((f32)i_Meter->mHeart[0].mSizeOrig.y, (f32)sHeartHeightScale);
  f32 zero = (f32)sZero;
  i_Meter->field_0x2f70 = (f32)sOne;
  i_Meter->field_0x2fbc = zero;
  i_Meter->mMaxHP = (u16)sSaveInfo->mMaxLife;
  i_Meter->mCurrHP = (u16)sSaveInfo->mLife;
  i_Meter->field_0x3014 = 5;
  i_Meter->field_0x0f00.mUserArea = 0;
  meter00_setNowAlphaZero(&i_Meter->field_0x0f00);
  u32 maximum = (u8)i_Meter->mMaxHP >> 2;
  s32 current = ((s32)(u8)i_Meter->mCurrHP - 1) / 4;
  for (u32 i = 0; i < 20; i++) {
    fopMsgM_pane_class *front = &i_Meter->mHeart[i];
    fopMsgM_pane_class *back = &i_Meter->mHeartShadow[i];
    u32 count = (u8)i_Meter->mCurrHP, level;
    if (maximum && count && (s32)i <= current)
      level = (s32)i == current ? (count & 3) : 0;
    else
      level = i < maximum ? 4 : 5;
    front->mUserArea = level;
    meter00_setHeartScaleNative(front, level);
    meter00_setHeartScaleNative(back, (s32)(s16)front->mUserArea);
    meter00_paneTrans(front, zero, zero);
    meter00_paneTrans(back, zero, zero);
    count = (u8)i_Meter->mCurrHP;
    if (i == (u32)(((s32)count - 1) / 4) && count) {
      meter00_setNowHeartScaleNative(front);
      meter00_setNowHeartScaleNative(back);
      auto *dst =
          reinterpret_cast<dMeterHeartWordCopyHD *>(&i_Meter->field_0x0f00);
      auto *src = reinterpret_cast<dMeterHeartWordCopyHD *>(front);
      dst->mTopLeft[0] = (u32)src->mTopLeft[0];
      dst->mTopLeft[1] = (u32)src->mTopLeft[1];
      dst->mSize[0] = (u32)src->mSize[0];
      dst->mSize[1] = (u32)src->mSize[1];
      dst->mCenter[0] = (u32)src->mCenter[0];
      dst->mCenter[1] = (u32)src->mCenter[1];
    }
  }
  u32 count = (u8)i_Meter->mCurrHP;
  dMeterGameInfoHD *state = meter00_getGameInfo();
  state->mItemNowLife = count;
}

/* 02591E60 */
void dMeter_itemUseCheckHD(dMeterPrefixHD *i_Meter, u32 index) {
  dMeterGameInfoHD *state = meter00_getGameInfo();
  bool special = (u8)state->mSelectItem[index] == dItemNo_PICTO_BOX_e;
  if (!special) {
    state = meter00_getGameInfo();
    special = (u8)state->mSelectItem[index] == dItemNo_DELUXE_PICTO_BOX_e;
  }
  if (special) {
    meter00_photoCheck(&sSaveInfo->mPhoto);
    meter00_photoCheck(&sSaveInfo->mPhoto);
    return;
  }
  for (u8 item : {dItemNo_BOW_e, dItemNo_MAGIC_ARROW_e, dItemNo_LIGHT_ARROW_e,
                  dItemNo_BOMB_BAG_e}) {
    state = meter00_getGameInfo();
    if ((u8)state->mSelectItem[index] == item)
      return;
  }
  u32 item = (u8)sSaveInfo->mSelectItem[index];
  if (item < dInvSlot_BeastFirst_e || item <= dInvSlot_BeastLast_e - 1 ||
      (u32)(item - dInvSlot_BaitFirst_e) >=
          dInvSlot_BaitLast_e - dInvSlot_BaitFirst_e)
    return;
  meter00_getGameInfo();
}

/* 02591F68 */
void dMeter_loadItemTextureHD(dMeterPrefixHD *i_Meter, u32 index) {
  dMeterGameInfoHD *state = meter00_getGameInfo();
  if ((u8)state->mSelectItem[index] != dItemNo_NONE_e) {
    u32 slot = (u8)sSaveInfo->mSelectItem[index];
    u32 resource;
    void *buffer;
    gptr<void> *address = &i_Meter->xyIconTex[index];
    if (slot >= dInvSlot_BeastFirst_e) {
      buffer = address->get();
      state = meter00_getGameInfo();
      u32 item = (u8)state->mSelectItem[index];
      resource = (u32)sItemResources.mItem[item].mIconName;
    } else {
      state = meter00_getGameInfo();
      u32 item = (u8)state->mSelectItem[index];
      if (item == dItemNo_MAGIC_ARROW_e || item == dItemNo_LIGHT_ARROW_e)
        item = dItemNo_BOW_e;
      buffer = address->get();
      resource = (u32)sItemResources.mItem[item].mIconName;
    }
    state = meter00_getGameInfo();
    void *archive = state->mpItemIconArchive.get();
    meter00_loadTexture(buffer, 0xC00, 0x54494D47, resource, archive);
    meter00_storeDataCache(address->get(), 0xC00);
  }
  meter00_itemUseCheckNative(i_Meter, index);
}

// HD: extracted item predicate returns the full native register.
/* 02592144 */
u32 dMeter_itemAlphaCheckHD(dMeterPrefixHD *i_Meter, s32 item) {
  u32 flags = (u32)i_Meter->mStatusFlags;
  if ((flags & dMtrStts_UNK1000_e) && item != dItemNo_GRAPPLING_HOOK_e)
    return 1;
  if ((flags & dMtrStts_UNK80_e) && item != dItemNo_TELESCOPE_e)
    return 1;
  if (flags & dMtrStts_UNK20000_e)
    return 1;
  if (flags & dMtrStts_UNK400_e) {
    dMeterGameInfoHD *state = meter00_getGameInfo();
    if ((u8)state->mEventMode != 1) {
      bool exempt = false;
      for (s32 x : {dItemNo_SAIL_e,         dItemNo_TELESCOPE_e,
                    dItemNo_WIND_WAKER_e,   dItemNo_GRAPPLING_HOOK_e,
                    dItemNo_BOOMERANG_e,    dItemNo_TINGLE_TUNER_e,
                    dItemNo_PICTO_BOX_e,    dItemNo_DELUXE_PICTO_BOX_e,
                    dItemNo_MAGIC_ARMOR_e,  dItemNo_BAIT_BAG_e,
                    dItemNo_HYOI_PEAR_e,    dItemNo_BIRD_BAIT_5_e,
                    dItemNo_BOW_e,          dItemNo_MAGIC_ARROW_e,
                    dItemNo_LIGHT_ARROW_e,  dItemNo_BOMB_BAG_e,
                    dItemNo_HOOKSHOT_e,     dItemNo_EMPTY_BOTTLE_e,
                    dItemNo_RED_POTION_e,   dItemNo_GREEN_POTION_e,
                    dItemNo_BLUE_POTION_e,  dItemNo_HALF_SOUP_BOTTLE_e,
                    dItemNo_SOUP_BOTTLE_e,  dItemNo_WATER_BOTTLE_e,
                    dItemNo_FAIRY_BOTTLE_e, dItemNo_FIREFLY_BOTTLE_e,
                    dItemNo_FOREST_WATER_e})
        if (item == x)
          exempt = true;
      if (!exempt)
        return 1;
    }
  }
  dMeterGameInfoHD *state = meter00_getGameInfo();
  if ((u8)state->mEventMode == 1 && item != dItemNo_SAIL_e &&
      item != dItemNo_TELESCOPE_e)
    return 1;
  dMeterStageInfoHD *stage = dMeter_getStageInfoHD();
  dMeterComparisonHD comparisons;
  auto scene = [&](u32 slot, const char *name) {
    dMeterSafeStringHD *left = &comparisons.mPairs[slot];
    dMeterSafeStringHD *right = &comparisons.mPairs[6 + slot];
    left->mText.v = gabi::guest_addr(name);
    left->__vtbl.v = sSafeStringVTable;
    dMeterGameInfoHD *game = meter00_getGameInfo();
    right->mText.v = gabi::guest_addr(game->mStartStageName);
    right->__vtbl.v = sSafeStringVTable;
    return meterSceneEqual(left, right);
  };
  if ((((u32)stage->mStageFlags >> 16) & 7) == dStageType_BOSS_e) {
    bool boss = false;
    const char *const names[] = {sBossDRC, sBossFW, sBossET, sBossWT};
    for (u32 i = 0; i < 4; i++)
      if (scene(i, names[i])) {
        boss = true;
        break;
      }
    if (boss &&
        (item == dItemNo_WATER_BOTTLE_e || item == dItemNo_FIREFLY_BOTTLE_e ||
         item == dItemNo_FOREST_WATER_e))
      return 1;
  }
  flags = (u32)i_Meter->mStatusFlags;
  if (!(flags & dMtrStts_UNK2_e))
    return 0;
  if (item == dItemNo_TINGLE_TUNER_e)
    return 0;
  if (item == dItemNo_WIND_WAKER_e)
    return scene(4, sEarthTemple) ? 0 : 1;
  if (item == dItemNo_GRAPPLING_HOOK_e)
    return scene(5, sWindTemple) ? 0 : 1;
  for (s32 x : {dItemNo_PICTO_BOX_e,        dItemNo_DELUXE_PICTO_BOX_e,
                dItemNo_SPOILS_BAG_e,       dItemNo_SKULL_NECKLACE_e,
                dItemNo_BOKOBABA_SEED_e,    dItemNo_GOLDEN_FEATHER_e,
                dItemNo_KNIGHTS_CREST_e,    dItemNo_RED_JELLY_e,
                dItemNo_GREEN_JELLY_e,      dItemNo_BLUE_JELLY_e,
                dItemNo_JOY_PENDANT_e,      dItemNo_BAIT_BAG_e,
                dItemNo_BIRD_BAIT_5_e,      dItemNo_HYOI_PEAR_e,
                dItemNo_EMPTY_BOTTLE_e,     dItemNo_RED_POTION_e,
                dItemNo_GREEN_POTION_e,     dItemNo_BLUE_POTION_e,
                dItemNo_HALF_SOUP_BOTTLE_e, dItemNo_SOUP_BOTTLE_e,
                dItemNo_WATER_BOTTLE_e,     dItemNo_FAIRY_BOTTLE_e,
                dItemNo_FIREFLY_BOTTLE_e,   dItemNo_FOREST_WATER_e,
                dItemNo_DELIVERY_BAG_e,     dItemNo_TOWN_FLOWER_e,
                dItemNo_SEA_FLOWER_e,       dItemNo_EXOTIC_FLOWER_e,
                dItemNo_HEROS_FLAG_e,       dItemNo_BIG_CATCH_FLAG_e,
                dItemNo_BIG_SALE_FLAG_e,    dItemNo_PINWHEEL_e,
                dItemNo_SICKLE_MOON_FLAG_e, dItemNo_SKULL_TOWER_IDOL_e,
                dItemNo_FOUNTAIN_IDOL_e,    dItemNo_POSTMAN_STATUE_e,
                dItemNo_SHOP_GURU_STATUE_e, dItemNo_FATHER_LETTER_e,
                dItemNo_NOTE_TO_MOM_e,      dItemNo_MAGGIES_LETTER_e,
                dItemNo_MOBLINS_LETTER_e,   dItemNo_CABANA_DEED_e,
                dItemNo_COMPLIMENTARY_ID_e, dItemNo_FILL_UP_COUPON_e})
    if (item == x)
      return 0;
  return 1;
}

/* 02592878 */
void dMeter_magicFlagClearHD() { sHeartTimers.mMagicFlag = 0; }

// HD: picture color writes are absent; parameter registers stay full width.
/* 02592EE8 */
void dMeter_flyGaugeMove(dMeterPrefixHD *i_Meter, u32 active, s32 maximum,
                         s32 current) {
  f32 zero = (f32)sZero, alpha;
  if (active) {
    f32 ratio = (f32)((f64)(f32)current / (f64)(f32)maximum);
    f32 length =
        fmuls_ppc(fmuls_ppc(ratio, (f32)sFlyGaugeWidth), (f32)sMagicSegments);
    meter00_magicInitTransNative(i_Meter);
    meter00_magicChangeNative(i_Meter, length);
    if (!(s16)i_Meter->field_0x0f38[0].mUserArea) {
      i_Meter->field_0x1130.mUserArea = 0;
      i_Meter->field_0x0f38[0].mUserArea = 1;
      i_Meter->field_0x0f38[1].mUserArea = 20;
    }
    meter00_maxLifeCountdownNative(1);
    s32 counter = (s16)i_Meter->field_0x11a0.mUserArea;
    f32 one = (f32)sOne;
    if (counter < 5) {
      counter = (s16)(counter + 1);
      i_Meter->field_0x11a0.mUserArea = counter;
      alpha = meter00_valueIncrease(5, counter, 0);
    } else
      alpha = one;
    counter = (s16)i_Meter->field_0x1130.mUserArea;
    if (counter < 50) {
      i_Meter->field_0x1130.mUserArea = counter + 1;
      dMeterGameInfoHD *state = meter00_getGameInfo();
      cXyz point;
      meter00_project(&state->mpCb1Player->eyePos, &point);
      sHeartTimers.mProjectedEye.x = (f32)point.x;
      sHeartTimers.mProjectedEye.y = (f32)point.y;
      sHeartTimers.mProjectedEye.z = (f32)point.z;
      counter = (s16)i_Meter->field_0x1130.mUserArea;
      f32 half = (f32)sHalf, blend, fade;
      if (counter > 30) {
        blend = meter00_valueIncrease(20, 50 - counter, 0);
        counter = (s16)i_Meter->field_0x1130.mUserArea;
        fade = meter00_valueIncrease(20, counter - 30, 0);
        f32 x = fmuls_ppc(fsubs_ppc((f32)point.x,
                                    (f32)i_Meter->field_0x11a0.mPosCenter.x),
                          blend),
            y = fmuls_ppc(fsubs_ppc((f32)point.y,
                                    (f32)i_Meter->field_0x11a0.mPosCenter.y),
                          blend);
        meter00_magicTransScaleNative(i_Meter, x, y, fmadds(half, fade, half));
      } else {
        fade = zero;
        blend = one;
        f32 x = fsubs_ppc((f32)point.x,
                          (f32)i_Meter->field_0x11a0.mPosCenter.x),
            y = fsubs_ppc((f32)point.y,
                          (f32)i_Meter->field_0x11a0.mPosCenter.y);
        meter00_magicTransScaleNative(i_Meter, x, y, half);
      }
      sHeartTimers.mFlyBlend = blend;
      sHeartTimers.mFlyFade = fade;
      i_Meter->field_0x2fc0 = alpha;
      return;
    }
  } else {
    meter00_heartScaleCountdownNative(1);
    s32 counter = (s16)i_Meter->field_0x11a0.mUserArea;
    if (counter > 0) {
      counter = (s16)(counter - 1);
      i_Meter->field_0x11a0.mUserArea = counter;
      alpha = meter00_valueIncrease(5, counter, 0);
    } else {
      i_Meter->field_0x11a0.mUserArea = 0;
      alpha = zero;
      meter00_magicTransNowInitNative(i_Meter);
    }
    if ((s16)i_Meter->field_0x1130.mUserArea)
      i_Meter->field_0x1130.mUserArea = 0;
  }
  i_Meter->field_0x2fc0 = alpha;
}

/* 02592A88 */
void dMeter_magicChange(dMeterPrefixHD *i_Meter, f32 remaining) {
  f32 ratio =
      (f32)((f64)remaining / (f64)(f32)i_Meter->field_0x0f38[0].mSizeOrig.x);
  s16 amount = (s16)ftoi(fmuls_ppc(ratio, (f32)sMagicUnits));
  dMeterGameInfoHD *state = meter00_getGameInfo();
  state->mItemNowMagic = (u16)amount;
  meter00_magicTransNowInitNative(i_Meter);
  f32 one = (f32)sOne, zero = (f32)sZero;
  for (u32 i = 0; i < 8; ++i) {
    fopMsgM_pane_class *pane = &i_Meter->field_0x0f38[i];
    if (!(remaining < (f32)pane->mSizeOrig.x)) {
      meter00_paneScaleXY(pane, one);
      remaining = fsubs_ppc(remaining, (f32)pane->mSizeOrig.x);
    } else if (!(remaining > zero))
      meter00_paneScaleXY(pane, one);
    else
      remaining = zero;
  }
}

/* 0259295C */
void dMeter_magicTransNowInit(dMeterPrefixHD *i_Meter) {
  u16 life = (u16)sSaveInfo->mMaxLife;
  f32 x = fsubs_ppc((f32)i_Meter->field_0x10f8.mPosCenter.x,
                    (f32)i_Meter->field_0x10f8.mPosCenterOrig.x);
  f32 y = (life <= 43 ? (f32)sLowLifeMagicY : (f32)sZero);
  meter00_paneTrans(&i_Meter->field_0x10f8, x, y);
  x = fsubs_ppc((f32)i_Meter->field_0x11a0.mPosCenter.x,
                (f32)i_Meter->field_0x11a0.mPosCenterOrig.x);
  meter00_paneTrans(&i_Meter->field_0x11a0, x, y);
  x = fsubs_ppc((f32)i_Meter->field_0x11d8.mPosCenter.x,
                (f32)i_Meter->field_0x11d8.mPosCenterOrig.x);
  meter00_paneTrans(&i_Meter->field_0x11d8, x, y);
  for (u32 i = 0; i < 8; ++i) {
    fopMsgM_pane_class *pane = &i_Meter->field_0x0f38[i];
    f32 origin = (f32)pane->mPosCenterOrig.x,
        position = (f32)pane->mPosCenter.x;
    meter00_paneTrans(pane, fsubs_ppc(position, origin), y);
  }
}

/* 02592BC4 */
void dMeter_magicInitTrans(dMeterPrefixHD *i_Meter) {
  u16 life = (u16)sSaveInfo->mMaxLife;
  f32 zero = (f32)sZero;
  f32 y = life <= 43 ? (f32)sLowLifeMagicY : zero;
  meter00_paneTrans(&i_Meter->field_0x10f8, zero, y);
  meter00_paneTrans(&i_Meter->field_0x11a0, zero, y);
  meter00_paneTrans(&i_Meter->field_0x11d8, zero, y);
  for (u32 i = 0; i < 8; ++i)
    meter00_paneTrans(&i_Meter->field_0x0f38[i], zero, y);
}

/* 02592CD8 */
void dMeter_magicTransScale(dMeterPrefixHD *i_Meter, f32 x, f32 y, f32 scale) {
  if ((u16)sSaveInfo->mMaxLife <= 43)
    y = fadds_ppc(y, (f32)sLowLifeMagicY);
  meter00_paneScaleXY(&i_Meter->field_0x11a0, scale);
  meter00_paneTrans(&i_Meter->field_0x11a0, x, y);
  f32 a = (f32)i_Meter->field_0x1130.mPosCenterOrig.x,
      b = (f32)i_Meter->field_0x1130.mPosCenterOrig.y;
  i_Meter->field_0x1130.mPosCenter.x = fmuls_ppc(a, scale);
  i_Meter->field_0x1130.mPosCenter.y = fmuls_ppc(b, scale);
  meter00_paneScaleXY(&i_Meter->field_0x1130, scale);
  a = (f32)i_Meter->field_0x10f8.mPosCenterOrig.x;
  f32 originX = (f32)i_Meter->field_0x11a0.mPosCenterOrig.x,
      originY = (f32)i_Meter->field_0x11a0.mPosCenterOrig.y;
  f32 dx = fsubs_ppc(a, originX);
  b = (f32)i_Meter->field_0x10f8.mPosCenterOrig.y;
  f32 nx = fadds_ppc(fmadds(dx, scale, originX), x);
  f32 ny = fadds_ppc(fmadds(fsubs_ppc(b, originY), scale, originY), y);
  i_Meter->field_0x10f8.mPosCenter.x = nx;
  i_Meter->field_0x10f8.mPosCenter.y = ny;
  meter00_paneScaleXY(&i_Meter->field_0x10f8, scale);
  a = (f32)i_Meter->field_0x11d8.mPosCenterOrig.x;
  originX = (f32)i_Meter->field_0x11a0.mPosCenterOrig.x;
  originY = (f32)i_Meter->field_0x11a0.mPosCenterOrig.y;
  dx = fsubs_ppc(a, originX);
  b = (f32)i_Meter->field_0x11d8.mPosCenterOrig.y;
  nx = fadds_ppc(fmadds(dx, scale, originX), x);
  ny = fadds_ppc(fmadds(fsubs_ppc(b, originY), scale, originY), y);
  i_Meter->field_0x11d8.mPosCenter.x = nx;
  i_Meter->field_0x11d8.mPosCenter.y = ny;
  meter00_paneScaleXY(&i_Meter->field_0x11d8, scale);
  a = (f32)i_Meter->field_0x1168.mPosCenterOrig.x;
  b = (f32)i_Meter->field_0x1168.mPosCenterOrig.y;
  i_Meter->field_0x1168.mPosCenter.x = fmuls_ppc(a, scale);
  i_Meter->field_0x1168.mPosCenter.y = fmuls_ppc(b, scale);
  meter00_paneScaleXY(&i_Meter->field_0x1168, scale);
  f32 half = (f32)sHalf;
  for (u32 i = 0; i < 8; ++i) {
    fopMsgM_pane_class *pane = &i_Meter->field_0x0f38[i];
    a = (f32)pane->mPosCenterOrig.x;
    originX = (f32)i_Meter->field_0x11a0.mPosCenterOrig.x;
    nx = fadds_ppc(fmadds(fsubs_ppc(a, originX), scale, originX), x);
    f32 width = (f32)pane->mSizeOrig.x;
    b = (f32)pane->mPosCenterOrig.y;
    f32 scaledWidth = fmuls_ppc(width, scale);
    pane->mPosCenter.x = nx;
    originY = (f32)i_Meter->field_0x11a0.mPosCenterOrig.y;
    f32 dy = fsubs_ppc(b, originY),
        left = dMeter_negativeMultiplySubtractHD(scaledWidth, half, nx),
        offset = (f32)pane->mSize.x;
    f32 py = fmadds(dy, scale, originY);
    nx = fmadds(offset, half, left);
    ny = fadds_ppc(py, y);
    pane->mPosCenter.x = nx;
    pane->mPosCenter.y = ny;
    meter00_cposMove(pane);
    a = (f32)pane->mSizeOrig.x;
    b = (f32)pane->mSizeOrig.y;
    pane->mSize.x = a;
    pane->mSize.y = b;
  }
}

// HD: picture texture-coordinate writes are absent; NaN precedence is explicit.
/* 02592888 */
void dMeter_magicLength(dMeterPrefixHD *i_Meter, f32 scale) {
  f32 edge =
      fmuls_ppc((f32)i_Meter->field_0x0f38[0].mSizeOrig.x, (f32)sMagicSegments);
  f32 width = (f32)i_Meter->field_0x10f8.mSizeOrig.x;
  f32 delta = fsubs_ppc(width, edge);
  f32 half = (f32)sHalf;
  f32 value;
  if (delta != delta)
    value = ppc_qnan(delta);
  else if (edge != edge)
    value = ppc_qnan(edge);
  else if (scale != scale)
    value = ppc_qnan(scale);
  else
    value = fmadds(edge, scale, delta);
  f32 left = fmadds(value, half, (f32)i_Meter->field_0x10f8.mPosTopLeftOrig.x);
  i_Meter->field_0x10f8.mSize.x = value;
  i_Meter->field_0x10f8.mPosCenter.x = left;
  meter00_cposMove(&i_Meter->field_0x10f8);
  f32 origin = (f32)i_Meter->field_0x10f8.mSizeOrig.x,
      length = (f32)i_Meter->field_0x11a0.mSizeOrig.x;
  value = (f32)((f64)(f32)i_Meter->field_0x10f8.mSize.x +
                (f64)fsubs_ppc(length, origin));
  left = fmadds(value, half, (f32)i_Meter->field_0x11a0.mPosTopLeftOrig.x);
  i_Meter->field_0x11a0.mSize.x = value;
  i_Meter->field_0x11a0.mPosCenter.x = left;
  meter00_cposMove(&i_Meter->field_0x11a0);
  length = (f32)i_Meter->field_0x1130.mSizeOrig.x;
  origin = (f32)i_Meter->field_0x10f8.mSizeOrig.x;
  value = (f32)((f64)(f32)i_Meter->field_0x10f8.mSize.x +
                (f64)fsubs_ppc(length, origin));
  left = fmadds(value, half, (f32)i_Meter->field_0x1130.mPosTopLeftOrig.x);
  i_Meter->field_0x1130.mSize.x = value;
  i_Meter->field_0x1130.mPosCenter.x = left;
  meter00_cposMove(&i_Meter->field_0x1130);
}

/* 025931A8 */
s32 dMeter_countStepHD(s32 value) {
  if (value >= 200)
    return 100;
  if (value < 50)
    return 1;
  return 10;
}

/* 025931CC */
void dMeter_setMagicPositionHD(dMeterPrefixHD *i_Meter) {
  u8 mode = (u8)sSaveInfo->mMagicMode;
  f32 y = (f32)sZero, x;
  if (!mode)
    x = fsubs_ppc((f32)i_Meter->field_0x19f0[1].mPosTopLeftOrig.x,
                  (f32)i_Meter->field_0x19f0[0].mPosTopLeftOrig.x);
  else
    x = y;
  meter00_paneTrans(&i_Meter->field_0x2a20, x, y);
  meter00_paneTrans(&i_Meter->field_0x2a58, x, y);
}

/* 02593284 */
void dMeter_paneAlphaCurveHD(fopMsgM_pane_class *pane,
                             const dMeterAlphaThresholdsHD *thresholds,
                             f32 scale) {
  s32 count = (s16)pane->mUserArea, first = (s16)thresholds->mFirst;
  f32 alpha;
  s32 last;
  if (count < first) {
    last = (s16)thresholds->mLast;
    alpha = (f32)sZero;
  } else {
    s32 second = (s16)thresholds->mSecond;
    f32 peak = (f32)sAlphaPeak;
    if (count < second) {
      f32 t = meter00_valueIncrease(second - first, count - first, 0);
      count = (s16)pane->mUserArea;
      last = (s16)thresholds->mLast;
      alpha = fmuls_ppc(peak, t);
    } else {
      s32 third = (s16)thresholds->mThird;
      f32 slope = (f32)sAlphaSlope;
      if (count < third) {
        f32 t = meter00_valueIncrease(third - second, count - second, 0);
        last = (s16)thresholds->mLast;
        count = (s16)pane->mUserArea;
        alpha = fnmsubs(slope, t, peak);
      } else {
        s32 fourth = (s16)thresholds->mFourth;
        if (count < fourth) {
          f32 t = meter00_valueIncrease(fourth - third, count - third, 0);
          count = (s16)pane->mUserArea;
          last = (s16)thresholds->mLast;
          alpha = fmadds(slope, t, (f32)sAlphaFloor);
        } else {
          last = (s16)thresholds->mLast;
          f32 t = meter00_valueIncrease(last - fourth, count - fourth, 0);
          last = (s16)thresholds->mLast;
          count = (s16)pane->mUserArea;
          alpha = fnmsubs(peak, t, peak);
        }
      }
    }
  }
  f32 result = fmuls_ppc(alpha, scale);
  pane->mUserArea = count >= last ? 1 : (u16)(count + 1);
  pane->mNowAlpha = (u8)ftoi(result);
}

/* 02593434 */
void dMeter_gaugeInitialPositionHD(dMeterPrefixHD *i_Meter) {
  meter00_heartScaleCountdownNative(4);
  meter00_getGameInfo();
  bool visible = (u8)sGaugeVisible != 0;
  f32 zero = (f32)sZero;
  i_Meter->field_0x2f68 = visible ? (f32)sOne : zero;
  f32 lower = (f32)sAlphaSlope, mana = (f32)sSaveInfo->mMagic;
  bool mode = mana < lower || !(mana < (f32)sMagicUpperBound);
  f32 x = -fadds_ppc((f32)i_Meter->field_0x1948.mPosTopLeftOrig.x,
                     (f32)i_Meter->field_0x1948.mSizeOrig.x);
  if (mode) {
    i_Meter->field_0x3012 = 1;
    i_Meter->mGaugeModeHD = 0;
    i_Meter->field_0x12f0.mUserArea = 0;
  } else {
    i_Meter->field_0x12f0.mUserArea = 0;
    i_Meter->mGaugeModeHD = 0;
    i_Meter->field_0x3012 = 0;
  }
  meter00_paneTrans(&i_Meter->field_0x12f0, x, zero);
  meter00_paneTrans(&i_Meter->field_0x1948, x, zero);
  i_Meter->field_0x3020 = 1;
}
/* WWHD glue
str 100512CC "Xboss0"
str 100512D4 "Xboss1"
str 100512DC "Xboss2"
str 100512E4 "Xboss3"
str 100512EC "Otkura"
str 100512F4 "Abesso"
data sNowHeartScale 1047AD24
data texRateX 101EA030
data texRateY 101EA034
data texScaleX 101EA038
data texScaleY 101EA03C
data sOne 100512B4
data sZero 100512B8
data sHeartWidth 100512BC
data sHeartHeightScale 100512C0
data sHeartRate 100512C4
data sHalf 100512C8
data sMagicSegments 100512FC
data sLowLifeMagicY 10051300
data sMagicUnits 10051304
data sFlyGaugeWidth 10051310
data sAlphaPeak 10051314
data sAlphaSlope 10051318
data sAlphaFloor 1005131C
data sMagicUpperBound 10051320
data sGaugeVisible 101EBFD8
data sSaveInfo 101F84DC
data sHeartTimers 1047B03C
data sItemResources 101E4674
ext 025200D4 dMeterGameInfoHD* meter00_getGameInfo()
ext 025B9100 u32 meter00_isBossEnemy(void*, u32)
ext 025B8B94 u32 meter00_isEventBit(void*, u32)
ext 025221F0 void meter00_copyPlayerRecollectionData()
ext 025B8B68 void meter00_onEventBit(void*, u32)
ext 025DB614 void meter00_cposMove(fopMsgM_pane_class*)
ext 025DB648 void meter00_paneTrans(fopMsgM_pane_class*, f32, f32)
ext 025DB654 void meter00_paneScaleXY(fopMsgM_pane_class*, f32)
ext 0259295C void meter00_magicTransNowInitNative(dMeterPrefixHD*)
ext 025DB6F4 f32 meter00_valueIncrease(s32, s32, u32)
ext 02591AB4 void meter00_heartScaleCountdownNative(u32)
ext 02720144 void meter00_photoCheck(void*)
ext 027EAA2C void meter00_loadTexture(void*, u32, u32, u32, void*)
ext C00088B8 void meter00_storeDataCache(void*, u32)
ext 025DB6E0 void meter00_setNowAlphaZero(fopMsgM_pane_class*)
ext 02591A64 void meter00_setHeartScaleNative(fopMsgM_pane_class*, u32)
ext 02591A3C void meter00_setNowHeartScaleNative(fopMsgM_pane_class*)
ext 02592BC4 void meter00_magicInitTransNative(dMeterPrefixHD*)
ext 02592A88 void meter00_magicChangeNative(dMeterPrefixHD*, f32)
ext 02591B00 void meter00_maxLifeCountdownNative(u32)
ext 025F0EA4 void meter00_project(cXyz*, cXyz*)
ext 02592CD8 void meter00_magicTransScaleNative(dMeterPrefixHD*, f32, f32, f32)
ext 02591E60 void meter00_itemUseCheckNative(dMeterPrefixHD*, u32)
*/
