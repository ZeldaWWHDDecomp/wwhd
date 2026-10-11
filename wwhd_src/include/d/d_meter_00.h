/* HUD hearts, magic and item controls. HD: sub_meter_class gains four bytes
 * before its panes; private views preserve tww field names without defining
 * the unrelated portions of the shared HUD class.
 */
#pragma once
#include "f_op/f_op_msg_mng.h"
struct dMeterPrefixHD {
  /* 0x0000 */ u8 mUnaccessed0[0x644];
  /* 0x0644 */ fopMsgM_pane_class mHeart[20];
  /* 0x0AA4 */ fopMsgM_pane_class mHeartShadow[20];
  /* 0x0F04 */ fopMsgM_pane_class field_0x0f00;
  /* 0x0F3C */ fopMsgM_pane_class field_0x0f38[8];
  /* 0x10FC */ fopMsgM_pane_class field_0x10f8;
  /* 0x1134 */ fopMsgM_pane_class field_0x1130;
  /* 0x116C */ fopMsgM_pane_class field_0x1168;
  /* 0x11A4 */ fopMsgM_pane_class field_0x11a0;
  /* 0x11DC */ fopMsgM_pane_class field_0x11d8;
  /* 0x1214 */ u8 mUnaccessed1214[0xE0];
  /* 0x12F4 */ fopMsgM_pane_class field_0x12f0;
  /* 0x132C */ u8 mUnaccessed132C[0x620];
  /* 0x194C */ fopMsgM_pane_class field_0x1948;
  /* 0x1984 */ u8 mUnaccessed1984[0x70];
  /* 0x19F4 */ fopMsgM_pane_class field_0x19f0[4];
  /* 0x1AD4 */ u8 mUnaccessed1AD4[0xF50];
  /* 0x2A24 */ fopMsgM_pane_class field_0x2a20;
  /* 0x2A5C */ fopMsgM_pane_class field_0x2a58;
  /* 0x2A94 */ u8 mUnaccessed2A94[0x4B8];
  /* 0x2F4C */ gptr<void> xyIconTex[3];
  /* 0x2F58 */ u8 mUnaccessed2F58[0x14];
  /* 0x2F6C */ be<f32> field_0x2f68;
  /* 0x2F70 */ u8 mUnaccessed2F70[0x4];
  /* 0x2F74 */ be<f32> field_0x2f70;
  /* 0x2F78 */ u8 mUnaccessed2F78[0x48];
  /* 0x2FC0 */ be<f32> field_0x2fbc;
  /* 0x2FC4 */ be<f32> field_0x2fc0;
  /* 0x2FC8 */ u8 mUnaccessed2FC8[0x40];
  /* 0x3008 */ be<u32> mStatusFlags;
  /* 0x300C */ u8 mUnaccessed300C[0xA];
  /* 0x3016 */ be<u16> field_0x3012;
  /* 0x3018 */ be<u16> field_0x3014;
  /* 0x301A */ u8 mUnaccessed301A[0x8];
  /* 0x3022 */ be<u8> mMaxHP;
  /* 0x3023 */ be<u8> mCurrHP;
  /* 0x3024 */ be<u8> field_0x3020;
  /* 0x3025 */ u8 mUnaccessed3025[0x9];
  /* 0x302E */ be<u8> mGaugeModeHD;
};
WWHD_OFFSET(dMeterPrefixHD, mHeart, 0x644);
WWHD_OFFSET(dMeterPrefixHD, mHeartShadow, 0xAA4);
WWHD_OFFSET(dMeterPrefixHD, field_0x0f00, 0xF04);
WWHD_OFFSET(dMeterPrefixHD, field_0x0f38, 0xF3C);
WWHD_OFFSET(dMeterPrefixHD, field_0x10f8, 0x10FC);
WWHD_OFFSET(dMeterPrefixHD, field_0x1130, 0x1134);
WWHD_OFFSET(dMeterPrefixHD, field_0x1168, 0x116C);
WWHD_OFFSET(dMeterPrefixHD, field_0x11a0, 0x11A4);
WWHD_OFFSET(dMeterPrefixHD, field_0x11d8, 0x11DC);
WWHD_OFFSET(dMeterPrefixHD, field_0x12f0, 0x12F4);
WWHD_OFFSET(dMeterPrefixHD, field_0x1948, 0x194C);
WWHD_OFFSET(dMeterPrefixHD, field_0x19f0, 0x19F4);
WWHD_OFFSET(dMeterPrefixHD, field_0x2a20, 0x2A24);
WWHD_OFFSET(dMeterPrefixHD, field_0x2a58, 0x2A5C);
WWHD_OFFSET(dMeterPrefixHD, xyIconTex, 0x2F4C);
WWHD_OFFSET(dMeterPrefixHD, field_0x2f68, 0x2F6C);
WWHD_OFFSET(dMeterPrefixHD, field_0x2f70, 0x2F74);
WWHD_OFFSET(dMeterPrefixHD, field_0x2fbc, 0x2FC0);
WWHD_OFFSET(dMeterPrefixHD, field_0x2fc0, 0x2FC4);
WWHD_OFFSET(dMeterPrefixHD, mStatusFlags, 0x3008);
WWHD_OFFSET(dMeterPrefixHD, field_0x3012, 0x3016);
WWHD_OFFSET(dMeterPrefixHD, field_0x3014, 0x3018);
WWHD_OFFSET(dMeterPrefixHD, mMaxHP, 0x3022);
WWHD_OFFSET(dMeterPrefixHD, mCurrHP, 0x3023);
WWHD_OFFSET(dMeterPrefixHD, field_0x3020, 0x3024);
WWHD_OFFSET(dMeterPrefixHD, mGaugeModeHD, 0x302E);
WWHD_SIZE(dMeterPrefixHD, 0x302F);
struct dMeterStageInfoHD {
  u8 mUnaccessed[9];
  be<u8> mSaveTable;
  u8 mUnaccessedA[2];
  be<u32> mStageFlags;
};
WWHD_OFFSET(dMeterStageInfoHD, mSaveTable, 9);
WWHD_OFFSET(dMeterStageInfoHD, mStageFlags, 0xC);
WWHD_SIZE(dMeterStageInfoHD, 0x10);
struct dMeterStageAccessHD {
  struct VTable {
    u8 mUnaccessed[0x15C];
    be<u32> getStagInfo;
  };
  gptr<VTable> __vtbl;
};
WWHD_OFFSET(dMeterStageAccessHD::VTable, getStagInfo, 0x15C);
WWHD_SIZE(dMeterStageAccessHD::VTable, 0x160);
WWHD_OFFSET(dMeterStageAccessHD, __vtbl, 0);
WWHD_SIZE(dMeterStageAccessHD, 4);
struct dMeterPlayerHD {
  u8 mUnaccessed[0x37C];
  cXyz eyePos;
};
WWHD_OFFSET(dMeterPlayerHD, eyePos, 0x37C);
WWHD_SIZE(dMeterPlayerHD, 0x388);
struct dMeterGameInfoHD {
  /* 0x0000 */ u8 mUnaccessed0[0x5134];
  /* 0x5134 */ char mStartStageName[8];
  /* 0x513C */ u8 mUnaccessed513C[0x14];
  /* 0x5150 */ dMeterStageAccessHD mStage;
  /* 0x5154 */ u8 mUnaccessed5154[0x930];
  /* 0x5A84 */ gptr<void> mpItemIconArchive;
  /* 0x5A88 */ u8 mUnaccessed5A88[0xB0];
  /* 0x5B38 */ gptr<dMeterPlayerHD> mpCb1Player;
  /* 0x5B3C */ u8 mUnaccessed5B3C[0x26];
  /* 0x5B62 */ be<u16> mItemNowMagic;
  /* 0x5B64 */ u8 mUnaccessed5B64[0x48];
  /* 0x5BAC */ be<u16> mItemNowLife;
  /* 0x5BAE */ u8 mUnaccessed5BAE[0xD];
  /* 0x5BBB */ be<u8> mSelectItem[3];
  /* 0x5BBE */ u8 mUnaccessed5BBE[0x12C];
  /* 0x5CEA */ be<u8> mEventMode;
};
WWHD_OFFSET(dMeterGameInfoHD, mStartStageName, 0x5134);
WWHD_OFFSET(dMeterGameInfoHD, mStage, 0x5150);
WWHD_OFFSET(dMeterGameInfoHD, mpItemIconArchive, 0x5A84);
WWHD_OFFSET(dMeterGameInfoHD, mpCb1Player, 0x5B38);
WWHD_OFFSET(dMeterGameInfoHD, mItemNowMagic, 0x5B62);
WWHD_OFFSET(dMeterGameInfoHD, mItemNowLife, 0x5BAC);
WWHD_OFFSET(dMeterGameInfoHD, mSelectItem, 0x5BBB);
WWHD_OFFSET(dMeterGameInfoHD, mEventMode, 0x5CEA);
WWHD_SIZE(dMeterGameInfoHD, 0x5CEB);
struct dMeterSaveHD {
  /* 0x0000 */ u8 mUnaccessed0[0x20];
  /* 0x0020 */ be<u16> mMaxLife;
  /* 0x0022 */ be<u16> mLife;
  /* 0x0024 */ u8 mUnaccessed24[0x5];
  /* 0x0029 */ be<u8> mSelectItem[3];
  /* 0x002C */ u8 mUnaccessed2C[0x6];
  /* 0x0032 */ be<u8> mMagicMode;
  /* 0x0033 */ u8 mUnaccessed33[0x11];
  /* 0x0044 */ be<f32> mMagic;
  /* 0x0048 */ u8 mUnaccessed48[0x5FC];
  /* 0x0644 */ u8 mEvent;
  /* 0x0645 */ u8 mUnaccessed645[0x153];
  /* 0x0798 */ u8 mStageSave;
  /* 0x0799 */ u8 mUnaccessed799[0xB27];
  /* 0x12C0 */ u8 mPhoto;
};
WWHD_OFFSET(dMeterSaveHD, mMaxLife, 0x20);
WWHD_OFFSET(dMeterSaveHD, mLife, 0x22);
WWHD_OFFSET(dMeterSaveHD, mSelectItem, 0x29);
WWHD_OFFSET(dMeterSaveHD, mMagicMode, 0x32);
WWHD_OFFSET(dMeterSaveHD, mMagic, 0x44);
WWHD_OFFSET(dMeterSaveHD, mEvent, 0x644);
WWHD_OFFSET(dMeterSaveHD, mStageSave, 0x798);
WWHD_OFFSET(dMeterSaveHD, mPhoto, 0x12C0);
WWHD_SIZE(dMeterSaveHD, 0x12C1);
struct dMeterHeartTimersHD {
  u8 mUnaccessed[0xC];
  cXyz mProjectedEye;
  u8 mUnaccessed18[0x18];
  be<f32> mFlyBlend;
  be<f32> mFlyFade;
  u8 mUnaccessed38[0xD];
  be<u8> mOpenTimer[7];
  be<u8> mCloseTimer[7];
  be<u8> mScaleActive[7];
  u8 mUnaccessed5A[5];
  be<u8> mMagicFlag;
};
WWHD_OFFSET(dMeterHeartTimersHD, mProjectedEye, 0xC);
WWHD_OFFSET(dMeterHeartTimersHD, mFlyBlend, 0x30);
WWHD_OFFSET(dMeterHeartTimersHD, mFlyFade, 0x34);
WWHD_OFFSET(dMeterHeartTimersHD, mOpenTimer, 0x45);
WWHD_OFFSET(dMeterHeartTimersHD, mCloseTimer, 0x4C);
WWHD_OFFSET(dMeterHeartTimersHD, mScaleActive, 0x53);
WWHD_OFFSET(dMeterHeartTimersHD, mMagicFlag, 0x5F);
WWHD_SIZE(dMeterHeartTimersHD, 0x60);
struct dMeterItemResourceHD {
  be<u32> mUnused;
  be<u32> mIconName;
  u8 mUnaccessed[28];
};
WWHD_OFFSET(dMeterItemResourceHD, mIconName, 4);
WWHD_SIZE(dMeterItemResourceHD, 36);
struct dMeterItemResourcesHD {
  dMeterItemResourceHD mItem[256];
};
WWHD_SIZE(dMeterItemResourcesHD, 9216);
struct dMeterSafeStringHD {
  struct VTable {
    u8 mUnaccessed[0x14];
    be<u32> assureTermination;
  };
  gptr<const char> mText;
  gptr<VTable> __vtbl;
};
WWHD_OFFSET(dMeterSafeStringHD, mText, 0);
WWHD_OFFSET(dMeterSafeStringHD, __vtbl, 4);
WWHD_SIZE(dMeterSafeStringHD, 8);
WWHD_OFFSET(dMeterSafeStringHD::VTable, assureTermination, 0x14);
WWHD_SIZE(dMeterSafeStringHD::VTable, 0x18);
struct dMeterComparisonHD {
  dMeterSafeStringHD mPairs[12];
};
WWHD_SIZE(dMeterComparisonHD, 0x60);
struct dMeterAlphaThresholdsHD {
  be<s16> mFirst, mSecond, mThird, mFourth, mLast;
};
WWHD_OFFSET(dMeterAlphaThresholdsHD, mLast, 8);
WWHD_SIZE(dMeterAlphaThresholdsHD, 10);
struct dMeterHeartWordCopyHD {
  be<u32> mPane, mTopLeftOrig[2], mTopLeft[2], mCenterOrig[2], mCenter[2],
      mSizeOrig[2], mSize[2];
};
WWHD_OFFSET(dMeterHeartWordCopyHD, mTopLeft, 0xC);
WWHD_OFFSET(dMeterHeartWordCopyHD, mCenter, 0x1C);
WWHD_OFFSET(dMeterHeartWordCopyHD, mSize, 0x2C);
WWHD_SIZE(dMeterHeartWordCopyHD, 0x34);

namespace meter00Item {
enum : s32 {
  dItemNo_BAIT_BAG_e = 0x2C,
  dItemNo_BIG_CATCH_FLAG_e = 0x90,
  dItemNo_BIG_SALE_FLAG_e = 0x91,
  dItemNo_BIRD_BAIT_5_e = 0x82,
  dItemNo_BLUE_JELLY_e = 0x4B,
  dItemNo_BLUE_POTION_e = 0x53,
  dItemNo_BOKOBABA_SEED_e = 0x46,
  dItemNo_BOMB_BAG_e = 0x31,
  dItemNo_BOOMERANG_e = 0x2D,
  dItemNo_BOW_e = 0x27,
  dItemNo_CABANA_DEED_e = 0x9C,
  dItemNo_COMPLIMENTARY_ID_e = 0x9D,
  dItemNo_DELIVERY_BAG_e = 0x30,
  dItemNo_DELUXE_PICTO_BOX_e = 0x26,
  dItemNo_EMPTY_BOTTLE_e = 0x50,
  dItemNo_EXOTIC_FLOWER_e = 0x8E,
  dItemNo_FAIRY_BOTTLE_e = 0x57,
  dItemNo_FATHER_LETTER_e = 0x98,
  dItemNo_FILL_UP_COUPON_e = 0x9E,
  dItemNo_FIREFLY_BOTTLE_e = 0x58,
  dItemNo_FOREST_WATER_e = 0x59,
  dItemNo_FOUNTAIN_IDOL_e = 0x95,
  dItemNo_GOLDEN_FEATHER_e = 0x47,
  dItemNo_GRAPPLING_HOOK_e = 0x25,
  dItemNo_GREEN_JELLY_e = 0x4A,
  dItemNo_GREEN_POTION_e = 0x52,
  dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
  dItemNo_HEROS_FLAG_e = 0x8F,
  dItemNo_HOOKSHOT_e = 0x2F,
  dItemNo_HYOI_PEAR_e = 0x83,
  dItemNo_JOY_PENDANT_e = 0x1F,
  dItemNo_KNIGHTS_CREST_e = 0x48,
  dItemNo_LIGHT_ARROW_e = 0x36,
  dItemNo_MAGGIES_LETTER_e = 0x9A,
  dItemNo_MAGIC_ARMOR_e = 0x2A,
  dItemNo_MAGIC_ARROW_e = 0x35,
  dItemNo_MOBLINS_LETTER_e = 0x9B,
  dItemNo_NONE_e = 0xFF,
  dItemNo_NOTE_TO_MOM_e = 0x99,
  dItemNo_PICTO_BOX_e = 0x23,
  dItemNo_PINWHEEL_e = 0x92,
  dItemNo_POSTMAN_STATUE_e = 0x96,
  dItemNo_RED_JELLY_e = 0x49,
  dItemNo_RED_POTION_e = 0x51,
  dItemNo_SAIL_e = 0x78,
  dItemNo_SEA_FLOWER_e = 0x8D,
  dItemNo_SHOP_GURU_STATUE_e = 0x97,
  dItemNo_SICKLE_MOON_FLAG_e = 0x93,
  dItemNo_SKULL_NECKLACE_e = 0x45,
  dItemNo_SKULL_TOWER_IDOL_e = 0x94,
  dItemNo_SOUP_BOTTLE_e = 0x55,
  dItemNo_SPOILS_BAG_e = 0x24,
  dItemNo_TELESCOPE_e = 0x20,
  dItemNo_TINGLE_TUNER_e = 0x21,
  dItemNo_TOWN_FLOWER_e = 0x8C,
  dItemNo_TRIFORCE_MAP_8_e = 0x80,
  dItemNo_WATER_BOTTLE_e = 0x56,
  dItemNo_WIND_WAKER_e = 0x22,
};
}
namespace meter00Status {
enum : u32 {
  dMtrStts_UNK2_e = 0x2,
  dMtrStts_UNK80_e = 0x80,
  dMtrStts_UNK400_e = 0x400,
  dMtrStts_UNK1000_e = 0x1000,
  dMtrStts_UNK20000_e = 0x20000,
};
}

namespace meter00Stage {
enum : u32 {
  dStageType_BOSS_e = 3,
  STAGE_DRC = 3,
  STAGE_FW = 4,
  STAGE_ET = 6,
  STAGE_WT = 7,
  UNK_3D80 = 0x3D80,
  UNK_3D40 = 0x3D40,
  UNK_3D20 = 0x3D20,
  UNK_3D10 = 0x3D10
};
enum : u32 {
  dInvSlot_BeastFirst_e = 24,
  dInvSlot_BeastLast_e = 32,
  dInvSlot_BaitFirst_e = 36,
  dInvSlot_BaitLast_e = 44
};
} // namespace meter00Stage
WWHD_OFFSET(dMeterAlphaThresholdsHD, mFirst, 0);
WWHD_OFFSET(dMeterAlphaThresholdsHD, mSecond, 2);
WWHD_OFFSET(dMeterAlphaThresholdsHD, mThird, 4);
WWHD_OFFSET(dMeterAlphaThresholdsHD, mFourth, 6);
