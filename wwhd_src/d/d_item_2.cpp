/* d_item part 2: item_func_hummer .. item_func_collectmap62 (0254E8E4..0255030F). WWHD.
 * See d_item.cpp for the unit's range and layout. */
#include "d/d_item_2.h"

static ItemAcquireMapGlobal l_mapMenu;

/* 0254E8E4 */
void item_func_hummer() {
    itemAcquireOnGetItem(dInvSlot_SKULL_HAMMER_e, 0);
    dComIfGs_setItem(dInvSlot_SKULL_HAMMER_e, dItemNo_SKULL_HAMMER_e);
}

/* 0254E92C */
void item_func_deku_leaf() {
    itemAcquireOnGetItem(dInvSlot_DEKU_LEAF_e, 0);
    dComIfGs_setItem(dInvSlot_DEKU_LEAF_e, dItemNo_DEKU_LEAF_e);
    dComIfGp_setItemMaxMagicCount(16);
    dComIfGp_setItemMagicCount(16);
}

/* 0254E994 */
void item_func_magic_arrow() {
    itemAcquireOnGetItem(dInvSlot_BOW_e, 1);
    dComIfGs_setItem(dInvSlot_BOW_e, dItemNo_MAGIC_ARROW_e);
    // If the regular bow was equipped on an X/Y/Z button, force it to update.
    itemAcquireRefreshSelectItem(dItemNo_BOW_e);
}

/* 0254EBD4 */
void item_func_light_arrow() {
    itemAcquireOnGetItem(dInvSlot_BOW_e, 2);
    dComIfGs_setItem(dInvSlot_BOW_e, dItemNo_LIGHT_ARROW_e);
    // If the fire/ice bow was equipped on an X/Y/Z button, force it to update.
    itemAcquireRefreshSelectItem(dItemNo_MAGIC_ARROW_e);
}

/* 0254EE14 */
void item_func_sword() {
    dComIfGs_onCollect(0x0, 0);
    itemAcquireSelectEquip(0x0, dItemNo_SWORD_e);
}

/* 0254EE54 */
void item_func_master_sword() {
    dComIfGs_onCollect(0x0, 1);
    itemAcquireSelectEquip(0x0, dItemNo_MASTER_SWORD_1_e);
}

/* 0254EE94 */
void item_func_lv3_sword() {
    dComIfGs_onCollect(0x0, 2);
    itemAcquireSelectEquip(0x0, dItemNo_MASTER_SWORD_2_e);
}

/* 0254EED4 */
void item_func_shield() {
    dComIfGs_onCollect(0x1, 0);
    itemAcquireSelectEquip(0x1, dItemNo_SHIELD_e);
}

/* 0254EF14 */
void item_func_mirror_shield() {
    dComIfGs_onCollect(0x1, 1);
    itemAcquireSelectEquip(0x1, dItemNo_MIRROR_SHIELD_e);
}

/* 0254EF54 */
void item_func_dropped_sword() {
    dComIfGs_onCollect(0x0, 0);
    itemAcquireSelectEquip(0x0, dItemNo_SWORD_e);
}

/* 0254EF94 */
void item_func_master_sword_ex() {
    dComIfGs_onCollect(0x0, 3);
    itemAcquireSelectEquip(0x0, dItemNo_MASTER_SWORD_3_e);
}

/* 0254EFD4 */
void item_func_pirates_omamori() {
    dComIfGs_onCollect(0x3, 0);
}

/* 0254EFEC */
void item_func_heros_omamori() {
    dComIfGs_onCollect(0x4, 0);
}

/* 0254F004 */
void item_func_skull_necklace() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SKULL_NECKLACE_e);
    dComIfGs_onGetItemBeast(dBeastIdx_SKULL_NECKLACE_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_SKULL_NECKLACE_e, 1);
}

/* 0254F05C */
void item_func_bokobaba_seed() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_BOKOBABA_SEED_e);
    dComIfGs_onGetItemBeast(dBeastIdx_BOKOBABA_SEED_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_BOKOBABA_SEED_e, 1);
}

/* 0254F0B4 */
void item_func_golden_feather() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_GOLDEN_FEATHER_e);
    dComIfGs_onGetItemBeast(dBeastIdx_GOLDEN_FEATHER_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_GOLDEN_FEATHER_e, 1);
}

/* 0254F10C */
void item_func_boko_belt() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_KNIGHTS_CREST_e);
    dComIfGs_onGetItemBeast(dBeastIdx_KNIGHTS_CREST_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_KNIGHTS_CREST_e, 1);
}

/* 0254F164 */
void item_func_red_jerry() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_RED_JELLY_e);
    dComIfGs_onGetItemBeast(dBeastIdx_RED_JELLY_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_RED_JELLY_e, 1);
}

/* 0254F1BC */
void item_func_green_jerry() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_GREEN_JELLY_e);
    dComIfGs_onGetItemBeast(dBeastIdx_GREEN_JELLY_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_GREEN_JELLY_e, 1);
}

/* 0254F214 */
void item_func_blue_jerry() {
    itemAcquireSetBeast(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_BLUE_JELLY_e);
    dComIfGs_onGetItemBeast(dBeastIdx_BLUE_JELLY_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_BLUE_JELLY_e, 1);
}

// HD: also sets a flag at *101F8344 + 0x228
/* 0254F26C */
void item_func_map() {
    itemAcquireDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::MAP);
    l_mapMenu.mMenu->mNewMap = 1; // HD: notify the map-menu object of a new map.
}

/* 0254F2AC */
void item_func_compass() {
    itemAcquireDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::COMPASS);
}

/* 0254F2C0 */
void item_func_boss_key() {
    itemAcquireDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::BOSS_KEY);
}

/* 0254F2D4 */
void item_func_empty_bship() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_EMPTY_BSHIP_e);
}

/* 0254F2E8 */
void item_func_empty_bottle() {
    itemAcquireEmptyBottle(&dComIfGs_info()->mSavedata.mPlayer.mPlayerItem);
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_EMPTY_BOTTLE_e);
}

/* 0254F32C */
void item_func_red_bottle() {
    dComIfGs_setEmptyBottleItemIn(dItemNo_RED_POTION_e);
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_RED_POTION_e);
}

/* 0254F374 */
void item_func_green_bottle() {
    dComIfGs_setEmptyBottleItemIn(dItemNo_GREEN_POTION_e);
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_GREEN_POTION_e);
}

/* 0254F3BC */
void item_func_blue_bottle() {
    dComIfGs_setEmptyBottleItemIn(dItemNo_BLUE_POTION_e);
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_BLUE_POTION_e);
}

/* 0254F404 */
void item_func_bottleship() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_HALF_SOUP_BOTTLE_e);
}

/* 0254F418 */
void item_func_soup_bottle() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_SOUP_BOTTLE_e);
}

/* 0254F42C */
void item_func_bin_in_water() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_WATER_BOTTLE_e);
}

/* 0254F440 */
void item_func_fairy_bottle() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_FAIRY_BOTTLE_e);
}

/* 0254F454 */
void item_func_firefly_bottle() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_FIREFLY_BOTTLE_e);
}

/* 0254F468 */
void item_func_fwater_bottle() {
    itemAcquireBottleFlag(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, dItemNo_FOREST_WATER_e);
}

/* 0254F47C */
void item_func_triforce1() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0);
}

/* 0254F490 */
void item_func_triforce2() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 1);
}

/* 0254F4A4 */
void item_func_triforce3() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 2);
}

/* 0254F4B8 */
void item_func_triforce4() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 3);
}

/* 0254F4CC */
void item_func_triforce5() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 4);
}

/* 0254F4E0 */
void item_func_triforce6() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 5);
}

/* 0254F4F4 */
void item_func_triforce7() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 6);
}

/* 0254F508 */
void item_func_triforce8() {
    itemAcquireTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 7);
}

/* 0254F51C */
void item_func_pearl1() {
    itemAcquireSymbol(&dComIfGs_info()->mSavedata.mPlayer.mCollect, dSymbol_NAYRU_e);
}

/* 0254F530 */
void item_func_pearl2() {
    itemAcquireSymbol(&dComIfGs_info()->mSavedata.mPlayer.mCollect, dSymbol_DIN_e);
}

/* 0254F544 */
void item_func_pearl3() {
    itemAcquireSymbol(&dComIfGs_info()->mSavedata.mPlayer.mCollect, dSymbol_FARORE_e);
}

/* 0254F558 */
void item_func_tact_song1() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0);
}

/* 0254F56C */
void item_func_tact_song2() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 1);
}

/* 0254F580 */
void item_func_tact_song3() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 2);
}

/* 0254F594 */
void item_func_tact_song4() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 3);
}

/* 0254F5A8 */
void item_func_tact_song5() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 4);
}

/* 0254F5BC */
void item_func_tact_song6() {
    itemAcquireTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 5);
}

// HD: additional item 0x77 (GameCube: noentry): the swift sail, a second sail bit
/* 0254F5D0 */
void item_func_swift_sail() {
    dComIfGs_setItem(dInvSlot_SAIL_e, ITEM_HD_SWIFT_SAIL);
    itemAcquireOnGetItem(dInvSlot_SAIL_e, 1);
    dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[4] = dInvSlot_SAIL_e;
    itemAcquireSelectItem(4);
}

// HD: puts the sail on the HD-only item button 4
/* 0254F7D0 */
void item_func_normal_sail() {
    dComIfGs_setItem(dInvSlot_SAIL_e, dItemNo_SAIL_e);
    itemAcquireOnGetItem(dInvSlot_SAIL_e, 0);
    dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[4] = dInvSlot_SAIL_e;
    itemAcquireSelectItem(4);
}

/* 0254F9D0 */
void item_func_bird_esa_5() {
    dComIfGs_onGetItemBait(0x0);
    itemAcquireSetBait(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_BIRD_BAIT_5_e);
}

/* 0254FA18 */
void item_func_animal_esa() {
    dComIfGs_onGetItemBait(0x1);
    itemAcquireSetBait(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_HYOI_PEAR_e);
}

/* 0254FA60 */
void item_func_flower_1() {
    dComIfGs_onGetItemReserve(0x0);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_TOWN_FLOWER_e);
}

/* 0254FAA8 */
void item_func_flower_2() {
    dComIfGs_onGetItemReserve(0x1);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SEA_FLOWER_e);
}

/* 0254FAF0 */
void item_func_flower_3() {
    dComIfGs_onGetItemReserve(0x2);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_EXOTIC_FLOWER_e);
}

/* 0254FB38 */
void item_func_heros_flag() {
    dComIfGs_onGetItemReserve(0x3);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_HEROS_FLAG_e);
}

/* 0254FB80 */
void item_func_tairyo_flag() {
    dComIfGs_onGetItemReserve(0x4);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_BIG_CATCH_FLAG_e);
}

/* 0254FBC8 */
void item_func_sales_flag() {
    dComIfGs_onGetItemReserve(0x5);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_BIG_SALE_FLAG_e);
}

/* 0254FC10 */
void item_func_wind_flag() {
    dComIfGs_onGetItemReserve(0x6);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_PINWHEEL_e);
}

/* 0254FC58 */
void item_func_red_flag() {
    dComIfGs_onGetItemReserve(0x7);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SICKLE_MOON_FLAG_e);
}

/* 0254FCA0 */
void item_func_fossil_head() {
    dComIfGs_onGetItemReserve(0x8);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SKULL_TOWER_IDOL_e);
}

/* 0254FCE8 */
void item_func_water_statue() {
    dComIfGs_onGetItemReserve(0x9);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_FOUNTAIN_IDOL_e);
}

/* 0254FD30 */
void item_func_postman_statue() {
    dComIfGs_onGetItemReserve(0xA);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_POSTMAN_STATUE_e);
}

/* 0254FD78 */
void item_func_president_statue() {
    dComIfGs_onGetItemReserve(0xB);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SHOP_GURU_STATUE_e);
}

/* 0254FDC0 */
void item_func_letter00() {
    dComIfGs_onGetItemReserve(0xC);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_FATHER_LETTER_e);
}

/* 0254FE08 */
void item_func_magic_seed() {
    dComIfGs_onGetItemReserve(0xD);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_NOTE_TO_MOM_e);
}

/* 0254FE50 */
void item_func_magys_letter() {
    dComIfGs_onGetItemReserve(0xE);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_MAGGIES_LETTER_e);
}

/* 0254FE98 */
void item_func_mo_letter() {
    dComIfGs_onGetItemReserve(0xF);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_MOBLINS_LETTER_e);
}

/* 0254FEE0 */
void item_func_cottage_paper() {
    dComIfGs_onGetItemReserve(0x10);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_CABANA_DEED_e);
}

/* 0254FF28 */
void item_func_kaisen_present1() {
    dComIfGs_onGetItemReserve(0x11);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_COMPLIMENTARY_ID_e);
}

/* 0254FF70 */
void item_func_kaisen_present2() {
    dComIfGs_onGetItemReserve(0x12);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_FILL_UP_COUPON_e);
}

/* 0254FFB8 */
void item_func_salvage_item2() {
    dComIfGs_onGetItemReserve(0x14);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SALVAGE_ITEM_2_e);
}

/* 02550000 */
void item_func_salvage_item3() {
    dComIfGs_onGetItemReserve(0x15);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_SALVAGE_ITEM_3_e);
}

/* 02550048 */
void item_func_xxx_039() {
    dComIfGs_onGetItemReserve(0x16);
    itemAcquireSetReserve(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_XXX_039_e);
}

/* 02550090 */
void item_func_max_rupee_up1() {
    dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mWalletSize = 0x1;
}

/* 025500A4 */
void item_func_max_rupee_up2() {
    dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mWalletSize = 0x2;
}

/* 025500B8 */
void item_func_max_bomb_up1() {
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum = 60;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum = 60;
}

/* 025500D4 */
void item_func_max_bomb_up2() {
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum = 99;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum = 99;
}

/* 025500F0 */
void item_func_max_arrow_up1() {
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum = 60;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum = 60;
}

/* 0255010C */
void item_func_max_arrow_up2() {
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum = 99;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum = 99;
}

/* 02550128 */
void item_func_max_mp_up1() {
    dComIfGp_setItemMaxMagicCount(32);
}

/* 02550154 */
void item_func_tincle_rupee1() {
    dComIfGp_setItemRupeeCount(50);
}

/* 02550180 */
void item_func_tincle_rupee2() {
    dComIfGp_setItemRupeeCount(100);
}

/* 025501AC */
void item_func_tincle_rupee3() {
    dComIfGp_setItemRupeeCount(150);
}

/* 025501D8 */
void item_func_tincle_rupee4() {
    dComIfGp_setItemRupeeCount(200);
}

/* 02550204 */
void item_func_tincle_rupee5() {
    dComIfGp_setItemRupeeCount(250);
}

/* 02550230 */
void item_func_tincle_rupee6() {
    dComIfGp_setItemRupeeCount(500);
}

/* 0255025C */
void item_func_lithograph1() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_1);
}

/* 02550270 */
void item_func_lithograph2() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_2);
}

/* 02550284 */
void item_func_lithograph3() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_3);
}

/* 02550298 */
void item_func_lithograph4() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_4);
}

/* 025502AC */
void item_func_lithograph5() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_5);
}

/* 025502C0 */
void item_func_lithograph6() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_6);
}

/* 025502D4 */
void item_func_collectmap64() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_7);
}

/* 025502E8 */
void item_func_collectmap63() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_8);
}

/* 025502FC */
void item_func_collectmap62() {
    dComIfGs_onEventBit(dSv_event_flag_c::LITHOGRAPH_9);
}
/* WWHD glue
data l_mapMenu 101F8344
ext 025B5CCC void itemAcquireMarkGet(dSv_player_get_item_c* items, s32 slot, u32 bit)
ext 02522398 void itemAcquireSelectEquip(s32 equip, u32 item)
ext 025B62A4 void itemAcquireSetBeast(dSv_player_bag_item_c* bag, u32 item)
ext 025B6E98 void itemAcquireSetBait(dSv_player_bag_item_c* bag, u32 item)
ext 025B75AC void itemAcquireSetReserve(dSv_player_bag_item_c* bag, u32 item)
ext 025B9098 void itemAcquireDungeonItem(dSv_memBit_c* bits, s32 item)
ext 025B5DBC void itemAcquireBottleFlag(dSv_player_get_item_c* items, u32 item)
ext 025B5454 void itemAcquireEmptyBottle(dSv_player_item_c* items)
ext 025B7B80 void itemAcquireTriforce(dSv_player_collect_c* collect, u32 piece)
ext 025B7CC0 void itemAcquireSymbol(dSv_player_collect_c* collect, u32 symbol)
ext 025B7AA8 void itemAcquireTact(dSv_player_collect_c* collect, u32 song)
*/
