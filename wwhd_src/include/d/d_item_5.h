/* Item classifiers and drop tables: tww fields, verified HD layouts. */
#pragma once
#include "d/d_s_actor_data_mng.h"
#include "gc_api/d/d_com_inf_game.h"
struct ItemDropPlayView {
    u8 mUnaccessed[0x50A0];
    dADM_CharTbl mCharTbl;
};
WWHD_OFFSET(ItemDropPlayView, mCharTbl, 0x50A0);
inline dADM_CharTbl* itemCharTbl() {
    return &((ItemDropPlayView*)dComIfGp_get())->mCharTbl;
}
struct ItemHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    be<f32> mRanges[4];
    be<u8> mFirstObject, mSecondObject;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(ItemHeaderStatics, mFirstObject, 0x18);
WWHD_OFFSET(ItemHeaderStatics, mZero, 0x1C);
WWHD_SIZE(ItemHeaderStatics, 0x2C);
struct ItemDestructorRecord {
    be<u32> mWords[3];
};
WWHD_SIZE(ItemDestructorRecord, 12);
s32 checkItemGet(u32 itemNo, s32 defaultVal);
s32 itemDrop_isGetItem(dSv_player_get_item_c* items, s32 slot, u32 bit);
u8 itemTable_GetInf(cDT* table, u32 column, u32 row);
void itemHeader_constructFirst(be<u8>* object);
void itemHeader_constructSecond(be<u8>* object);
enum ItemDropId {
    dItemNo_HEART_e = 0x0,
    dItemNo_GREEN_RUPEE_e = 0x1,
    dItemNo_BLUE_RUPEE_e = 0x2,
    dItemNo_YELLOW_RUPEE_e = 0x3,
    dItemNo_RED_RUPEE_e = 0x4,
    dItemNo_PURPLE_RUPEE_e = 0x5,
    dItemNo_ORANGE_RUPEE_e = 0x6,
    dItemNo_SMALL_MAGIC_e = 0x9,
    dItemNo_LARGE_MAGIC_e = 0xA,
    dItemNo_BOMB_5_e = 0xB,
    dItemNo_BOMB_10_e = 0xC,
    dItemNo_BOMB_20_e = 0xD,
    dItemNo_BOMB_30_e = 0xE,
    dItemNo_SILVER_RUPEE_e = 0xF,
    dItemNo_ARROW_10_e = 0x10,
    dItemNo_ARROW_20_e = 0x11,
    dItemNo_ARROW_30_e = 0x12,
    dItemNo_TRIPLE_HEART_e = 0x1E,
    dItemNo_JOY_PENDANT_e = 0x1F,
    dItemNo_SPOILS_BAG_e = 0x24,
    dItemNo_BAIT_BAG_e = 0x2C,
    dItemNo_SKULL_NECKLACE_e = 0x45,
    dItemNo_BOKOBABA_SEED_e = 0x46,
    dItemNo_GOLDEN_FEATHER_e = 0x47,
    dItemNo_KNIGHTS_CREST_e = 0x48,
    dItemNo_RED_JELLY_e = 0x49,
    dItemNo_GREEN_JELLY_e = 0x4A,
    dItemNo_BLUE_JELLY_e = 0x4B,
    dItemNo_TRIFORCE1_e = 0x61,
    dItemNo_TRIFORCE2_e = 0x62,
    dItemNo_TRIFORCE3_e = 0x63,
    dItemNo_TRIFORCE4_e = 0x64,
    dItemNo_TRIFORCE5_e = 0x65,
    dItemNo_TRIFORCE6_e = 0x66,
    dItemNo_TRIFORCE7_e = 0x67,
    dItemNo_TRIFORCE8_e = 0x68,
    dItemNo_BIRD_BAIT_5_e = 0x82,
    dItemNo_HYOI_PEAR_e = 0x83,
    dItemNo_MAGIC_BEAN_e = 0x89,
    dItemNo_TOWN_FLOWER_e = 0x8C,
    dItemNo_SEA_FLOWER_e = 0x8D,
    dItemNo_EXOTIC_FLOWER_e = 0x8E,
    dItemNo_HEROS_FLAG_e = 0x8F,
    dItemNo_BIG_CATCH_FLAG_e = 0x90,
    dItemNo_BIG_SALE_FLAG_e = 0x91,
    dItemNo_PINWHEEL_e = 0x92,
    dItemNo_SICKLE_MOON_FLAG_e = 0x93,
    dItemNo_SKULL_TOWER_IDOL_e = 0x94,
    dItemNo_FOUNTAIN_IDOL_e = 0x95,
    dItemNo_POSTMAN_STATUE_e = 0x96,
    dItemNo_SHOP_GURU_STATUE_e = 0x97,
    dItemNo_NONE_e = 0xFF,
};
