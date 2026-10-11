/* d_item part 4: the item_getcheck_func_* that query the save (0255055C..02550F37). WWHD.
 * See d_item.cpp for the unit's range. The 25
 * `return -1` getchecks are grouped at the end of the unit (d_item_5.cpp). */
#include "d/d_item_4.h"

/* 0255055C */
s32 item_getcheck_func_bomb_5() {
    return dComIfGs_isGetItem(0xD, 0);
}

/* 02550574 */
s32 item_getcheck_func_bomb_10() {
    return dComIfGs_isGetItem(0xD, 0);
}

/* 0255058C */
s32 item_getcheck_func_bomb_20() {
    return dComIfGs_isGetItem(0xD, 0);
}

/* 025505A4 */
s32 item_getcheck_func_bomb_30() {
    return dComIfGs_isGetItem(0xD, 0);
}

/* 025505BC */
s32 item_getcheck_func_arrow_10() {
    return dComIfGs_isGetItem(0xC, 0);
}

/* 025505D4 */
s32 item_getcheck_func_arrow_20() {
    return dComIfGs_isGetItem(0xC, 0);
}

/* 025505EC */
s32 item_getcheck_func_arrow_30() {
    return dComIfGs_isGetItem(0xC, 0);
}

/* 02550604 */
s32 item_getcheck_func_pendant() {
    return dComIfGs_isGetItemBeast(dBeastIdx_JOY_PENDANT_e);
}

/* 02550618 */
s32 item_getcheck_func_telescope() {
    return dComIfGs_isGetItem(0x0, 0);
}

/* 02550630 */
s32 item_getcheck_func_tncl_whitsl() {
    return dComIfGs_isGetItem(0x7, 0);
}

/* 02550648 */
s32 item_getcheck_func_wind_tact() {
    return dComIfGs_isGetItem(0x2, 0);
}

/* 02550660 */
s32 item_getcheck_func_camera() {
    return dComIfGs_isGetItem(0x8, 0);
}

/* 02550678 */
s32 item_getcheck_func_emono_bag() {
    return dComIfGs_isGetItem(0x4, 0);
}

/* 02550690 */
s32 item_getcheck_func_rope() {
    return dComIfGs_isGetItem(0x3, 0);
}

/* 025506A8 */
s32 item_getcheck_func_camera2() {
    return dComIfGs_isGetItem(0x8, 1);
}

/* 025506C0 */
s32 item_getcheck_func_bow() {
    return dComIfGs_isGetItem(0xC, 0);
}

/* 025506D8 */
s32 item_getcheck_func_pwr_groove() {
    return dComIfGs_isCollect(0x2, 0);
}

/* 025506F0 */
s32 item_getcheck_func_hvy_boots() {
    return dComIfGs_isGetItem(0x9, 0);
}

/* 02550708 */
s32 item_getcheck_func_drgn_shield() {
    return dComIfGs_isGetItem(0xA, 0);
}

/* 02550720 */
s32 item_getcheck_func_esa_bag() {
    return dComIfGs_isGetItem(0xB, 0);
}

/* 02550738 */
s32 item_getcheck_func_boomerang() {
    return dComIfGs_isGetItem(0x5, 0);
}

/* 02550750 */
s32 item_getcheck_func_hookshot() {
    return dComIfGs_isGetItem(0x13, 0);
}

/* 02550768 */
s32 item_getcheck_func_warasibe_bag() {
    return dComIfGs_isGetItem(0x12, 0);
}

/* 02550780 */
s32 item_getcheck_func_hummer() {
    return dComIfGs_isGetItem(0x14, 0);
}

/* 02550798 */
s32 item_getcheck_func_deku_leaf() {
    return dComIfGs_isGetItem(0x6, 0);
}

/* 025507B0 */
s32 item_getcheck_func_magic_arrow() {
    return dComIfGs_isGetItem(0xC, 1);
}

/* 025507C8 */
s32 item_getcheck_func_light_arrow() {
    return dComIfGs_isGetItem(0xC, 2);
}

/* 025507E0 */
s32 item_getcheck_func_sword() {
    return dComIfGs_isCollect(0x0, 0);
}

/* 025507F8 */
s32 item_getcheck_func_master_sword() {
    return dComIfGs_isCollect(0x0, 1);
}

/* 02550810 */
s32 item_getcheck_func_lv3_sword() {
    return dComIfGs_isCollect(0x0, 2);
}

/* 02550828 */
s32 item_getcheck_func_shield() {
    return dComIfGs_isCollect(0x1, 0);
}

/* 02550840 */
s32 item_getcheck_func_mirror_shield() {
    return dComIfGs_isCollect(0x1, 1);
}

/* 02550858 */
s32 item_getcheck_func_master_sword_ex() {
    return dComIfGs_isCollect(0x0, 3);
}

/* 02550870 */
s32 item_getcheck_func_pirates_omamori() {
    return dComIfGs_isCollect(0x3, 0);
}

/* 02550888 */
s32 item_getcheck_func_heros_omamori() {
    return dComIfGs_isCollect(0x4, 0);
}

/* 025508A0 */
s32 item_getcheck_func_skull_necklace() {
    return dComIfGs_isGetItemBeast(dBeastIdx_SKULL_NECKLACE_e);
}

/* 025508B4 */
s32 item_getcheck_func_bokobaba_seed() {
    return dComIfGs_isGetItemBeast(dBeastIdx_BOKOBABA_SEED_e);
}

/* 025508C8 */
s32 item_getcheck_func_golden_feather() {
    return dComIfGs_isGetItemBeast(dBeastIdx_GOLDEN_FEATHER_e);
}

/* 025508DC */
s32 item_getcheck_func_boko_belt() {
    return dComIfGs_isGetItemBeast(dBeastIdx_KNIGHTS_CREST_e);
}

/* 025508F0 */
s32 item_getcheck_func_red_jerry() {
    return dComIfGs_isGetItemBeast(dBeastIdx_RED_JELLY_e);
}

/* 02550904 */
s32 item_getcheck_func_green_jerry() {
    return dComIfGs_isGetItemBeast(dBeastIdx_GREEN_JELLY_e);
}

/* 02550918 */
s32 item_getcheck_func_blue_jerry() {
    return dComIfGs_isGetItemBeast(dBeastIdx_BLUE_JELLY_e);
}

/* 0255092C */
s32 item_getcheck_func_map() {
    return dComIfGs_isDungeonItem(dSv_memBit_c::MAP);
}

/* 02550940 */
s32 item_getcheck_func_boss_key() {
    return dComIfGs_isDungeonItem(dSv_memBit_c::BOSS_KEY);
}

/* 02550954 */
s32 item_getcheck_func_empty_bship() {
    return dComIfGs_isGetBottleItem(dItemNo_EMPTY_BSHIP_e);
}

/* 02550968 */
s32 item_getcheck_func_empty_bottle() {
    return dComIfGs_isGetBottleItem(dItemNo_EMPTY_BOTTLE_e);
}

/* 0255097C */
s32 item_getcheck_func_red_bottle() {
    return dComIfGs_isGetBottleItem(dItemNo_RED_POTION_e);
}

/* 02550990 */
s32 item_getcheck_func_green_bottle() {
    return dComIfGs_isGetBottleItem(dItemNo_GREEN_POTION_e);
}

/* 025509A4 */
s32 item_getcheck_func_blue_bottle() {
    return dComIfGs_isGetBottleItem(dItemNo_BLUE_POTION_e);
}

/* 025509B8 */
s32 item_getcheck_func_bottleship() {
    return dComIfGs_isGetBottleItem(dItemNo_HALF_SOUP_BOTTLE_e);
}

/* 025509CC */
s32 item_getcheck_func_bin_in_bottleship() {
    return dComIfGs_isGetBottleItem(dItemNo_SOUP_BOTTLE_e);
}

/* 025509E0 */
s32 item_getcheck_func_bin_in_water() {
    return dComIfGs_isGetBottleItem(dItemNo_WATER_BOTTLE_e);
}

/* 025509F4 */
s32 item_getcheck_func_bin() {
    return dComIfGs_isGetBottleItem(dItemNo_FAIRY_BOTTLE_e);
}

/* 02550A08 */
s32 item_getcheck_func_triforce1() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 0);
}

/* 02550A1C */
s32 item_getcheck_func_triforce2() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 1);
}

/* 02550A30 */
s32 item_getcheck_func_triforce3() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 2);
}

/* 02550A44 */
s32 item_getcheck_func_triforce4() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 3);
}

/* 02550A58 */
s32 item_getcheck_func_triforce5() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 4);
}

/* 02550A6C */
s32 item_getcheck_func_triforce6() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 5);
}

/* 02550A80 */
s32 item_getcheck_func_triforce7() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 6);
}

/* 02550A94 */
s32 item_getcheck_func_triforce8() {
    return itemQuery_isTriforce(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 7);
}

/* 02550AA8 */
s32 item_getcheck_func_pearl1() {
    return dComIfGs_isSymbol(dSymbol_NAYRU_e);
}

/* 02550ABC */
s32 item_getcheck_func_pearl2() {
    return dComIfGs_isSymbol(dSymbol_DIN_e);
}

/* 02550AD0 */
s32 item_getcheck_func_pearl3() {
    return dComIfGs_isSymbol(dSymbol_FARORE_e);
}

/* 02550AE4 */
s32 item_getcheck_func_tact_song1() {
    return dComIfGs_isTact(0);
}

/* 02550AF8 */
s32 item_getcheck_func_tact_song2() {
    return dComIfGs_isTact(1);
}

/* 02550B0C */
s32 item_getcheck_func_tact_song3() {
    return dComIfGs_isTact(2);
}

/* 02550B20 */
s32 item_getcheck_func_tact_song4() {
    return dComIfGs_isTact(3);
}

/* 02550B34 */
s32 item_getcheck_func_tact_song5() {
    return dComIfGs_isTact(4);
}

/* 02550B48 */
s32 item_getcheck_func_tact_song6() {
    return dComIfGs_isTact(5);
}

// HD: Swift Sail replaces GameCube item 0x77 (noentry).
/* 02550B5C */
s32 item_getcheck_func_swift_sail() {
    return dComIfGs_isGetItem(0x1, 1);
}

/* 02550B74 */
s32 item_getcheck_func_normal_sail() {
    return dComIfGs_isGetItem(0x1, 0);
}

/* 02550B8C */
s32 item_getcheck_func_bird_esa_5() {
    return dComIfGs_isGetItemBait(0x0);
}

/* 02550BA0 */
s32 item_getcheck_func_animal_esa() {
    return dComIfGs_isGetItemBait(0x1);
}

/* 02550BB4 */
s32 item_getcheck_func_esa1() {
    return dComIfGs_isGetItemBait(0x2);
}

/* 02550BC8 */
s32 item_getcheck_func_esa2() {
    return dComIfGs_isGetItemBait(0x3);
}

/* 02550BDC */
s32 item_getcheck_func_esa3() {
    return dComIfGs_isGetItemBait(0x4);
}

/* 02550BF0 */
s32 item_getcheck_func_esa4() {
    return dComIfGs_isGetItemBait(0x5);
}

/* 02550C04 */
s32 item_getcheck_func_esa5() {
    return dComIfGs_isGetItemBait(0x6);
}

/* 02550C18 */
s32 item_getcheck_func_bird_esa_10() {
    return dComIfGs_isGetItemBait(0x7);
}

/* 02550C2C */
s32 item_getcheck_func_flower_1() {
    return dComIfGs_isGetItemReserve(0x0);
}

/* 02550C40 */
s32 item_getcheck_func_flower_2() {
    return dComIfGs_isGetItemReserve(0x1);
}

/* 02550C54 */
s32 item_getcheck_func_flower_3() {
    return dComIfGs_isGetItemReserve(0x2);
}

/* 02550C68 */
s32 item_getcheck_func_heros_flag() {
    return dComIfGs_isGetItemReserve(0x3);
}

/* 02550C7C */
s32 item_getcheck_func_tairyo_flag() {
    return dComIfGs_isGetItemReserve(0x4);
}

/* 02550C90 */
s32 item_getcheck_func_sales_flag() {
    return dComIfGs_isGetItemReserve(0x5);
}

/* 02550CA4 */
s32 item_getcheck_func_wind_flag() {
    return dComIfGs_isGetItemReserve(0x6);
}

/* 02550CB8 */
s32 item_getcheck_func_red_flag() {
    return dComIfGs_isGetItemReserve(0x7);
}

/* 02550CCC */
s32 item_getcheck_func_fossil_head() {
    return dComIfGs_isGetItemReserve(0x8);
}

/* 02550CE0 */
s32 item_getcheck_func_water_statue() {
    return dComIfGs_isGetItemReserve(0x9);
}

/* 02550CF4 */
s32 item_getcheck_func_postman_statue() {
    return dComIfGs_isGetItemReserve(0xA);
}

/* 02550D08 */
s32 item_getcheck_func_president_statue() {
    return dComIfGs_isGetItemReserve(0xB);
}

/* 02550D1C */
s32 item_getcheck_func_letter00() {
    return dComIfGs_isGetItemReserve(0xC);
}

/* 02550D30 */
s32 item_getcheck_func_magic_seed() {
    return dComIfGs_isGetItemReserve(0xD);
}

/* 02550D44 */
s32 item_getcheck_func_magys_letter() {
    return dComIfGs_isGetItemReserve(0xE);
}

/* 02550D58 */
s32 item_getcheck_func_mo_letter() {
    return dComIfGs_isGetItemReserve(0xF);
}

/* 02550D6C */
s32 item_getcheck_func_cottage_paper() {
    return dComIfGs_isGetItemReserve(0x10);
}

/* 02550D80 */
s32 item_getcheck_func_kaisen_present1() {
    return dComIfGs_isGetItemReserve(0x11);
}

/* 02550D94 */
s32 item_getcheck_func_kaisen_present2() {
    return dComIfGs_isGetItemReserve(0x12);
}

/* 02550DA8 */
s32 item_getcheck_func_salvage_item1() {
    return dComIfGs_isGetItemReserve(0x13);
}

/* 02550DBC */
s32 item_getcheck_func_salvage_item2() {
    return dComIfGs_isGetItemReserve(0x14);
}

/* 02550DD0 */
s32 item_getcheck_func_salvage_item3() {
    return dComIfGs_isGetItemReserve(0x15);
}

/* 02550DE4 */
s32 item_getcheck_func_xxx_039() {
    return dComIfGs_isGetItemReserve(0x16);
}

/* 02550DF8 */
s32 item_getcheck_func_lithograph1() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_1);
}

/* 02550E0C */
s32 item_getcheck_func_lithograph2() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_2);
}

/* 02550E20 */
s32 item_getcheck_func_lithograph3() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_3);
}

/* 02550E34 */
s32 item_getcheck_func_lithograph4() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_4);
}

/* 02550E48 */
s32 item_getcheck_func_lithograph5() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_5);
}

/* 02550E5C */
s32 item_getcheck_func_lithograph6() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_6);
}

/* 02550E70 */
s32 item_getcheck_func_lithograph7() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_7);
}

/* 02550E84 */
s32 item_getcheck_func_lithograph8() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_8);
}

/* 02550E98 */
s32 item_getcheck_func_lithograph9() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_9);
}

/* 02550EAC */
s32 item_getcheck_func_lithograph10() {
    return dComIfGs_isEventBit(dSv_event_flag_c::LITHOGRAPH_10);
}

/* 02550EC0 */
s32 item_getcheck_func_lithograph11() {
    return dComIfGs_isGetCollectMap(60);
}

/* 02550ED4 */
s32 item_getcheck_func_lithograph12() {
    return dComIfGs_isGetCollectMap(59);
}

/* 02550EE8 */
s32 item_getcheck_func_lithograph13() {
    return dComIfGs_isGetCollectMap(58);
}

/* 02550EFC */
s32 item_getcheck_func_lithograph14() {
    return dComIfGs_isGetCollectMap(57);
}

/* 02550F10 */
s32 item_getcheck_func_lithograph15() {
    return dComIfGs_isGetCollectMap(56);
}

/* 02550F24 */
s32 item_getcheck_func_lithograph16() {
    return dComIfGs_isGetCollectMap(55);
}
/* WWHD glue
ext 025B7C50 s32 itemQuery_isTriforce(dSv_player_collect_c* collect, u32 piece)
*/
