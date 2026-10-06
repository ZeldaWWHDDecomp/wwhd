/* d_item part 4: the item_getcheck_func_* that query the save (0255055C..02550F37). WWHD.
 * See d_item.cpp for the unit's range. The 25
 * `return -1` getchecks are grouped at the end of the unit (d_item_5.cpp). */
#include "bindings.h"

namespace d_item_4_cpp {
#include "d_item_local.h"

/* 0255055C */
s32 item_getcheck_func_bomb_5() {
    WWHD_FUNC(0x0255055C, s32);
    return dComIfGs_isGetItem(0xD, 0);
}
VERIFY(0x0255055C, item_getcheck_func_bomb_5);

/* 02550574 */
s32 item_getcheck_func_bomb_10() {
    WWHD_FUNC(0x02550574, s32);
    return dComIfGs_isGetItem(0xD, 0);
}
VERIFY(0x02550574, item_getcheck_func_bomb_10);

/* 0255058C */
s32 item_getcheck_func_bomb_20() {
    WWHD_FUNC(0x0255058C, s32);
    return dComIfGs_isGetItem(0xD, 0);
}
VERIFY(0x0255058C, item_getcheck_func_bomb_20);

/* 025505A4 */
s32 item_getcheck_func_bomb_30() {
    WWHD_FUNC(0x025505A4, s32);
    return dComIfGs_isGetItem(0xD, 0);
}
VERIFY(0x025505A4, item_getcheck_func_bomb_30);

/* 025505BC */
s32 item_getcheck_func_arrow_10() {
    WWHD_FUNC(0x025505BC, s32);
    return dComIfGs_isGetItem(0xC, 0);
}
VERIFY(0x025505BC, item_getcheck_func_arrow_10);

/* 025505D4 */
s32 item_getcheck_func_arrow_20() {
    WWHD_FUNC(0x025505D4, s32);
    return dComIfGs_isGetItem(0xC, 0);
}
VERIFY(0x025505D4, item_getcheck_func_arrow_20);

/* 025505EC */
s32 item_getcheck_func_arrow_30() {
    WWHD_FUNC(0x025505EC, s32);
    return dComIfGs_isGetItem(0xC, 0);
}
VERIFY(0x025505EC, item_getcheck_func_arrow_30);

/* 02550604 */
s32 item_getcheck_func_pendant() {
    WWHD_FUNC(0x02550604, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 7 /* dBeastIdx_JOY_PENDANT_e */);
}
VERIFY(0x02550604, item_getcheck_func_pendant);

/* 02550618 */
s32 item_getcheck_func_telescope() {
    WWHD_FUNC(0x02550618, s32);
    return dComIfGs_isGetItem(0x0, 0);
}
VERIFY(0x02550618, item_getcheck_func_telescope);

/* 02550630 */
s32 item_getcheck_func_tncl_whitsl() {
    WWHD_FUNC(0x02550630, s32);
    return dComIfGs_isGetItem(0x7, 0);
}
VERIFY(0x02550630, item_getcheck_func_tncl_whitsl);

/* 02550648 */
s32 item_getcheck_func_wind_tact() {
    WWHD_FUNC(0x02550648, s32);
    return dComIfGs_isGetItem(0x2, 0);
}
VERIFY(0x02550648, item_getcheck_func_wind_tact);

/* 02550660 */
s32 item_getcheck_func_camera() {
    WWHD_FUNC(0x02550660, s32);
    return dComIfGs_isGetItem(0x8, 0);
}
VERIFY(0x02550660, item_getcheck_func_camera);

/* 02550678 */
s32 item_getcheck_func_emono_bag() {
    WWHD_FUNC(0x02550678, s32);
    return dComIfGs_isGetItem(0x4, 0);
}
VERIFY(0x02550678, item_getcheck_func_emono_bag);

/* 02550690 */
s32 item_getcheck_func_rope() {
    WWHD_FUNC(0x02550690, s32);
    return dComIfGs_isGetItem(0x3, 0);
}
VERIFY(0x02550690, item_getcheck_func_rope);

/* 025506A8 */
s32 item_getcheck_func_camera2() {
    WWHD_FUNC(0x025506A8, s32);
    return dComIfGs_isGetItem(0x8, 1);
}
VERIFY(0x025506A8, item_getcheck_func_camera2);

/* 025506C0 */
s32 item_getcheck_func_bow() {
    WWHD_FUNC(0x025506C0, s32);
    return dComIfGs_isGetItem(0xC, 0);
}
VERIFY(0x025506C0, item_getcheck_func_bow);

/* 025506D8 */
s32 item_getcheck_func_pwr_groove() {
    WWHD_FUNC(0x025506D8, s32);
    return dComIfGs_isCollect(0x2, 0);
}
VERIFY(0x025506D8, item_getcheck_func_pwr_groove);

/* 025506F0 */
s32 item_getcheck_func_hvy_boots() {
    WWHD_FUNC(0x025506F0, s32);
    return dComIfGs_isGetItem(0x9, 0);
}
VERIFY(0x025506F0, item_getcheck_func_hvy_boots);

/* 02550708 */
s32 item_getcheck_func_drgn_shield() {
    WWHD_FUNC(0x02550708, s32);
    return dComIfGs_isGetItem(0xA, 0);
}
VERIFY(0x02550708, item_getcheck_func_drgn_shield);

/* 02550720 */
s32 item_getcheck_func_esa_bag() {
    WWHD_FUNC(0x02550720, s32);
    return dComIfGs_isGetItem(0xB, 0);
}
VERIFY(0x02550720, item_getcheck_func_esa_bag);

/* 02550738 */
s32 item_getcheck_func_boomerang() {
    WWHD_FUNC(0x02550738, s32);
    return dComIfGs_isGetItem(0x5, 0);
}
VERIFY(0x02550738, item_getcheck_func_boomerang);

/* 02550750 */
s32 item_getcheck_func_hookshot() {
    WWHD_FUNC(0x02550750, s32);
    return dComIfGs_isGetItem(0x13, 0);
}
VERIFY(0x02550750, item_getcheck_func_hookshot);

/* 02550768 */
s32 item_getcheck_func_warasibe_bag() {
    WWHD_FUNC(0x02550768, s32);
    return dComIfGs_isGetItem(0x12, 0);
}
VERIFY(0x02550768, item_getcheck_func_warasibe_bag);

/* 02550780 */
s32 item_getcheck_func_hummer() {
    WWHD_FUNC(0x02550780, s32);
    return dComIfGs_isGetItem(0x14, 0);
}
VERIFY(0x02550780, item_getcheck_func_hummer);

/* 02550798 */
s32 item_getcheck_func_deku_leaf() {
    WWHD_FUNC(0x02550798, s32);
    return dComIfGs_isGetItem(0x6, 0);
}
VERIFY(0x02550798, item_getcheck_func_deku_leaf);

/* 025507B0 */
s32 item_getcheck_func_magic_arrow() {
    WWHD_FUNC(0x025507B0, s32);
    return dComIfGs_isGetItem(0xC, 1);
}
VERIFY(0x025507B0, item_getcheck_func_magic_arrow);

/* 025507C8 */
s32 item_getcheck_func_light_arrow() {
    WWHD_FUNC(0x025507C8, s32);
    return dComIfGs_isGetItem(0xC, 2);
}
VERIFY(0x025507C8, item_getcheck_func_light_arrow);

/* 025507E0 */
s32 item_getcheck_func_sword() {
    WWHD_FUNC(0x025507E0, s32);
    return dComIfGs_isCollect(0x0, 0);
}
VERIFY(0x025507E0, item_getcheck_func_sword);

/* 025507F8 */
s32 item_getcheck_func_master_sword() {
    WWHD_FUNC(0x025507F8, s32);
    return dComIfGs_isCollect(0x0, 1);
}
VERIFY(0x025507F8, item_getcheck_func_master_sword);

/* 02550810 */
s32 item_getcheck_func_lv3_sword() {
    WWHD_FUNC(0x02550810, s32);
    return dComIfGs_isCollect(0x0, 2);
}
VERIFY(0x02550810, item_getcheck_func_lv3_sword);

/* 02550828 */
s32 item_getcheck_func_shield() {
    WWHD_FUNC(0x02550828, s32);
    return dComIfGs_isCollect(0x1, 0);
}
VERIFY(0x02550828, item_getcheck_func_shield);

/* 02550840 */
s32 item_getcheck_func_mirror_shield() {
    WWHD_FUNC(0x02550840, s32);
    return dComIfGs_isCollect(0x1, 1);
}
VERIFY(0x02550840, item_getcheck_func_mirror_shield);

/* 02550858 */
s32 item_getcheck_func_master_sword_ex() {
    WWHD_FUNC(0x02550858, s32);
    return dComIfGs_isCollect(0x0, 3);
}
VERIFY(0x02550858, item_getcheck_func_master_sword_ex);

/* 02550870 */
s32 item_getcheck_func_pirates_omamori() {
    WWHD_FUNC(0x02550870, s32);
    return dComIfGs_isCollect(0x3, 0);
}
VERIFY(0x02550870, item_getcheck_func_pirates_omamori);

/* 02550888 */
s32 item_getcheck_func_heros_omamori() {
    WWHD_FUNC(0x02550888, s32);
    return dComIfGs_isCollect(0x4, 0);
}
VERIFY(0x02550888, item_getcheck_func_heros_omamori);

/* 025508A0 */
s32 item_getcheck_func_skull_necklace() {
    WWHD_FUNC(0x025508A0, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 0 /* dBeastIdx_SKULL_NECKLACE_e */);
}
VERIFY(0x025508A0, item_getcheck_func_skull_necklace);

/* 025508B4 */
s32 item_getcheck_func_bokobaba_seed() {
    WWHD_FUNC(0x025508B4, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 1 /* dBeastIdx_BOKOBABA_SEED_e */);
}
VERIFY(0x025508B4, item_getcheck_func_bokobaba_seed);

/* 025508C8 */
s32 item_getcheck_func_golden_feather() {
    WWHD_FUNC(0x025508C8, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 2 /* dBeastIdx_GOLDEN_FEATHER_e */);
}
VERIFY(0x025508C8, item_getcheck_func_golden_feather);

/* 025508DC */
s32 item_getcheck_func_boko_belt() {
    WWHD_FUNC(0x025508DC, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 3 /* dBeastIdx_KNIGHTS_CREST_e */);
}
VERIFY(0x025508DC, item_getcheck_func_boko_belt);

/* 025508F0 */
s32 item_getcheck_func_red_jerry() {
    WWHD_FUNC(0x025508F0, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 4 /* dBeastIdx_RED_JELLY_e */);
}
VERIFY(0x025508F0, item_getcheck_func_red_jerry);

/* 02550904 */
s32 item_getcheck_func_green_jerry() {
    WWHD_FUNC(0x02550904, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 5 /* dBeastIdx_GREEN_JELLY_e */);
}
VERIFY(0x02550904, item_getcheck_func_green_jerry);

/* 02550918 */
s32 item_getcheck_func_blue_jerry() {
    WWHD_FUNC(0x02550918, s32);
    return dSv_getbag_isBeast_l(svbase() + SV_GETBAG, 6 /* dBeastIdx_BLUE_JELLY_e */);
}
VERIFY(0x02550918, item_getcheck_func_blue_jerry);

/* 0255092C */
s32 item_getcheck_func_map() {
    WWHD_FUNC(0x0255092C, s32);
    return dSv_memBit_isDungeonItem_l(svbase() + SV_MEMBIT, 0 /* MAP */);
}
VERIFY(0x0255092C, item_getcheck_func_map);

/* 02550940 */
s32 item_getcheck_func_boss_key() {
    WWHD_FUNC(0x02550940, s32);
    return dSv_memBit_isDungeonItem_l(svbase() + SV_MEMBIT, 2 /* BOSS_KEY */);
}
VERIFY(0x02550940, item_getcheck_func_boss_key);

/* 02550954 */
s32 item_getcheck_func_empty_bship() {
    WWHD_FUNC(0x02550954, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x4F /* dItemNo_EMPTY_BSHIP_e */);
}
VERIFY(0x02550954, item_getcheck_func_empty_bship);

/* 02550968 */
s32 item_getcheck_func_empty_bottle() {
    WWHD_FUNC(0x02550968, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x50 /* dItemNo_EMPTY_BOTTLE_e */);
}
VERIFY(0x02550968, item_getcheck_func_empty_bottle);

/* 0255097C */
s32 item_getcheck_func_red_bottle() {
    WWHD_FUNC(0x0255097C, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x51 /* dItemNo_RED_POTION_e */);
}
VERIFY(0x0255097C, item_getcheck_func_red_bottle);

/* 02550990 */
s32 item_getcheck_func_green_bottle() {
    WWHD_FUNC(0x02550990, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x52 /* dItemNo_GREEN_POTION_e */);
}
VERIFY(0x02550990, item_getcheck_func_green_bottle);

/* 025509A4 */
s32 item_getcheck_func_blue_bottle() {
    WWHD_FUNC(0x025509A4, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x53 /* dItemNo_BLUE_POTION_e */);
}
VERIFY(0x025509A4, item_getcheck_func_blue_bottle);

/* 025509B8 */
s32 item_getcheck_func_bottleship() {
    WWHD_FUNC(0x025509B8, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x54 /* dItemNo_HALF_SOUP_BOTTLE_e */);
}
VERIFY(0x025509B8, item_getcheck_func_bottleship);

/* 025509CC */
s32 item_getcheck_func_bin_in_bottleship() {
    WWHD_FUNC(0x025509CC, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x55 /* dItemNo_SOUP_BOTTLE_e */);
}
VERIFY(0x025509CC, item_getcheck_func_bin_in_bottleship);

/* 025509E0 */
s32 item_getcheck_func_bin_in_water() {
    WWHD_FUNC(0x025509E0, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x56 /* dItemNo_WATER_BOTTLE_e */);
}
VERIFY(0x025509E0, item_getcheck_func_bin_in_water);

/* 025509F4 */
s32 item_getcheck_func_bin() {
    WWHD_FUNC(0x025509F4, s32);
    return dSv_get_item_isBottleItem_l(svbase() + SV_GETITEM, 0x57 /* dItemNo_FAIRY_BOTTLE_e */);
}
VERIFY(0x025509F4, item_getcheck_func_bin);

/* 02550A08 */
s32 item_getcheck_func_triforce1() {
    WWHD_FUNC(0x02550A08, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 0);
}
VERIFY(0x02550A08, item_getcheck_func_triforce1);

/* 02550A1C */
s32 item_getcheck_func_triforce2() {
    WWHD_FUNC(0x02550A1C, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 1);
}
VERIFY(0x02550A1C, item_getcheck_func_triforce2);

/* 02550A30 */
s32 item_getcheck_func_triforce3() {
    WWHD_FUNC(0x02550A30, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 2);
}
VERIFY(0x02550A30, item_getcheck_func_triforce3);

/* 02550A44 */
s32 item_getcheck_func_triforce4() {
    WWHD_FUNC(0x02550A44, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 3);
}
VERIFY(0x02550A44, item_getcheck_func_triforce4);

/* 02550A58 */
s32 item_getcheck_func_triforce5() {
    WWHD_FUNC(0x02550A58, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 4);
}
VERIFY(0x02550A58, item_getcheck_func_triforce5);

/* 02550A6C */
s32 item_getcheck_func_triforce6() {
    WWHD_FUNC(0x02550A6C, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 5);
}
VERIFY(0x02550A6C, item_getcheck_func_triforce6);

/* 02550A80 */
s32 item_getcheck_func_triforce7() {
    WWHD_FUNC(0x02550A80, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 6);
}
VERIFY(0x02550A80, item_getcheck_func_triforce7);

/* 02550A94 */
s32 item_getcheck_func_triforce8() {
    WWHD_FUNC(0x02550A94, s32);
    return dSv_collect_isTriforce_l(svbase() + SV_COLLECT, 7);
}
VERIFY(0x02550A94, item_getcheck_func_triforce8);

/* 02550AA8 */
s32 item_getcheck_func_pearl1() {
    WWHD_FUNC(0x02550AA8, s32);
    return dSv_collect_isSymbol_l(svbase() + SV_COLLECT, 0 /* dSymbol_NAYRU_e */);
}
VERIFY(0x02550AA8, item_getcheck_func_pearl1);

/* 02550ABC */
s32 item_getcheck_func_pearl2() {
    WWHD_FUNC(0x02550ABC, s32);
    return dSv_collect_isSymbol_l(svbase() + SV_COLLECT, 1 /* dSymbol_DIN_e */);
}
VERIFY(0x02550ABC, item_getcheck_func_pearl2);

/* 02550AD0 */
s32 item_getcheck_func_pearl3() {
    WWHD_FUNC(0x02550AD0, s32);
    return dSv_collect_isSymbol_l(svbase() + SV_COLLECT, 2 /* dSymbol_FARORE_e */);
}
VERIFY(0x02550AD0, item_getcheck_func_pearl3);

/* 02550AE4 */
s32 item_getcheck_func_tact_song1() {
    WWHD_FUNC(0x02550AE4, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 0);
}
VERIFY(0x02550AE4, item_getcheck_func_tact_song1);

/* 02550AF8 */
s32 item_getcheck_func_tact_song2() {
    WWHD_FUNC(0x02550AF8, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 1);
}
VERIFY(0x02550AF8, item_getcheck_func_tact_song2);

/* 02550B0C */
s32 item_getcheck_func_tact_song3() {
    WWHD_FUNC(0x02550B0C, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 2);
}
VERIFY(0x02550B0C, item_getcheck_func_tact_song3);

/* 02550B20 */
s32 item_getcheck_func_tact_song4() {
    WWHD_FUNC(0x02550B20, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 3);
}
VERIFY(0x02550B20, item_getcheck_func_tact_song4);

/* 02550B34 */
s32 item_getcheck_func_tact_song5() {
    WWHD_FUNC(0x02550B34, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 4);
}
VERIFY(0x02550B34, item_getcheck_func_tact_song5);

/* 02550B48 */
s32 item_getcheck_func_tact_song6() {
    WWHD_FUNC(0x02550B48, s32);
    return dSv_collect_isTact_l(svbase() + SV_COLLECT, 5);
}
VERIFY(0x02550B48, item_getcheck_func_tact_song6);

/* 02550B5C: HD-only item 0x77 (GameCube: noentry) */
s32 item_getcheck_func_swift_sail() {
    WWHD_FUNC(0x02550B5C, s32);
    return dComIfGs_isGetItem(0x1, 1);
}
VERIFY(0x02550B5C, item_getcheck_func_swift_sail);

/* 02550B74 */
s32 item_getcheck_func_normal_sail() {
    WWHD_FUNC(0x02550B74, s32);
    return dComIfGs_isGetItem(0x1, 0);
}
VERIFY(0x02550B74, item_getcheck_func_normal_sail);

/* 02550B8C */
s32 item_getcheck_func_bird_esa_5() {
    WWHD_FUNC(0x02550B8C, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x0);
}
VERIFY(0x02550B8C, item_getcheck_func_bird_esa_5);

/* 02550BA0 */
s32 item_getcheck_func_animal_esa() {
    WWHD_FUNC(0x02550BA0, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x1);
}
VERIFY(0x02550BA0, item_getcheck_func_animal_esa);

/* 02550BB4 */
s32 item_getcheck_func_esa1() {
    WWHD_FUNC(0x02550BB4, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x2);
}
VERIFY(0x02550BB4, item_getcheck_func_esa1);

/* 02550BC8 */
s32 item_getcheck_func_esa2() {
    WWHD_FUNC(0x02550BC8, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x3);
}
VERIFY(0x02550BC8, item_getcheck_func_esa2);

/* 02550BDC */
s32 item_getcheck_func_esa3() {
    WWHD_FUNC(0x02550BDC, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x4);
}
VERIFY(0x02550BDC, item_getcheck_func_esa3);

/* 02550BF0 */
s32 item_getcheck_func_esa4() {
    WWHD_FUNC(0x02550BF0, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x5);
}
VERIFY(0x02550BF0, item_getcheck_func_esa4);

/* 02550C04 */
s32 item_getcheck_func_esa5() {
    WWHD_FUNC(0x02550C04, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x6);
}
VERIFY(0x02550C04, item_getcheck_func_esa5);

/* 02550C18 */
s32 item_getcheck_func_bird_esa_10() {
    WWHD_FUNC(0x02550C18, s32);
    return dSv_getbag_isBait_l(svbase() + SV_GETBAG, 0x7);
}
VERIFY(0x02550C18, item_getcheck_func_bird_esa_10);

/* 02550C2C */
s32 item_getcheck_func_flower_1() {
    WWHD_FUNC(0x02550C2C, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x0);
}
VERIFY(0x02550C2C, item_getcheck_func_flower_1);

/* 02550C40 */
s32 item_getcheck_func_flower_2() {
    WWHD_FUNC(0x02550C40, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x1);
}
VERIFY(0x02550C40, item_getcheck_func_flower_2);

/* 02550C54 */
s32 item_getcheck_func_flower_3() {
    WWHD_FUNC(0x02550C54, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x2);
}
VERIFY(0x02550C54, item_getcheck_func_flower_3);

/* 02550C68 */
s32 item_getcheck_func_heros_flag() {
    WWHD_FUNC(0x02550C68, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x3);
}
VERIFY(0x02550C68, item_getcheck_func_heros_flag);

/* 02550C7C */
s32 item_getcheck_func_tairyo_flag() {
    WWHD_FUNC(0x02550C7C, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x4);
}
VERIFY(0x02550C7C, item_getcheck_func_tairyo_flag);

/* 02550C90 */
s32 item_getcheck_func_sales_flag() {
    WWHD_FUNC(0x02550C90, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x5);
}
VERIFY(0x02550C90, item_getcheck_func_sales_flag);

/* 02550CA4 */
s32 item_getcheck_func_wind_flag() {
    WWHD_FUNC(0x02550CA4, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x6);
}
VERIFY(0x02550CA4, item_getcheck_func_wind_flag);

/* 02550CB8 */
s32 item_getcheck_func_red_flag() {
    WWHD_FUNC(0x02550CB8, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x7);
}
VERIFY(0x02550CB8, item_getcheck_func_red_flag);

/* 02550CCC */
s32 item_getcheck_func_fossil_head() {
    WWHD_FUNC(0x02550CCC, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x8);
}
VERIFY(0x02550CCC, item_getcheck_func_fossil_head);

/* 02550CE0 */
s32 item_getcheck_func_water_statue() {
    WWHD_FUNC(0x02550CE0, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x9);
}
VERIFY(0x02550CE0, item_getcheck_func_water_statue);

/* 02550CF4 */
s32 item_getcheck_func_postman_statue() {
    WWHD_FUNC(0x02550CF4, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xA);
}
VERIFY(0x02550CF4, item_getcheck_func_postman_statue);

/* 02550D08 */
s32 item_getcheck_func_president_statue() {
    WWHD_FUNC(0x02550D08, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xB);
}
VERIFY(0x02550D08, item_getcheck_func_president_statue);

/* 02550D1C */
s32 item_getcheck_func_letter00() {
    WWHD_FUNC(0x02550D1C, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xC);
}
VERIFY(0x02550D1C, item_getcheck_func_letter00);

/* 02550D30 */
s32 item_getcheck_func_magic_seed() {
    WWHD_FUNC(0x02550D30, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xD);
}
VERIFY(0x02550D30, item_getcheck_func_magic_seed);

/* 02550D44 */
s32 item_getcheck_func_magys_letter() {
    WWHD_FUNC(0x02550D44, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xE);
}
VERIFY(0x02550D44, item_getcheck_func_magys_letter);

/* 02550D58 */
s32 item_getcheck_func_mo_letter() {
    WWHD_FUNC(0x02550D58, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0xF);
}
VERIFY(0x02550D58, item_getcheck_func_mo_letter);

/* 02550D6C */
s32 item_getcheck_func_cottage_paper() {
    WWHD_FUNC(0x02550D6C, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x10);
}
VERIFY(0x02550D6C, item_getcheck_func_cottage_paper);

/* 02550D80 */
s32 item_getcheck_func_kaisen_present1() {
    WWHD_FUNC(0x02550D80, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x11);
}
VERIFY(0x02550D80, item_getcheck_func_kaisen_present1);

/* 02550D94 */
s32 item_getcheck_func_kaisen_present2() {
    WWHD_FUNC(0x02550D94, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x12);
}
VERIFY(0x02550D94, item_getcheck_func_kaisen_present2);

/* 02550DA8 */
s32 item_getcheck_func_salvage_item1() {
    WWHD_FUNC(0x02550DA8, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x13);
}
VERIFY(0x02550DA8, item_getcheck_func_salvage_item1);

/* 02550DBC */
s32 item_getcheck_func_salvage_item2() {
    WWHD_FUNC(0x02550DBC, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x14);
}
VERIFY(0x02550DBC, item_getcheck_func_salvage_item2);

/* 02550DD0 */
s32 item_getcheck_func_salvage_item3() {
    WWHD_FUNC(0x02550DD0, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x15);
}
VERIFY(0x02550DD0, item_getcheck_func_salvage_item3);

/* 02550DE4 */
s32 item_getcheck_func_xxx_039() {
    WWHD_FUNC(0x02550DE4, s32);
    return dSv_getbag_isReserve_l(svbase() + SV_GETBAG, 0x16);
}
VERIFY(0x02550DE4, item_getcheck_func_xxx_039);

/* 02550DF8 */
s32 item_getcheck_func_lithograph1() {
    WWHD_FUNC(0x02550DF8, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3508 /* LITHOGRAPH_1 */);
}
VERIFY(0x02550DF8, item_getcheck_func_lithograph1);

/* 02550E0C */
s32 item_getcheck_func_lithograph2() {
    WWHD_FUNC(0x02550E0C, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3504 /* LITHOGRAPH_2 */);
}
VERIFY(0x02550E0C, item_getcheck_func_lithograph2);

/* 02550E20 */
s32 item_getcheck_func_lithograph3() {
    WWHD_FUNC(0x02550E20, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3502 /* LITHOGRAPH_3 */);
}
VERIFY(0x02550E20, item_getcheck_func_lithograph3);

/* 02550E34 */
s32 item_getcheck_func_lithograph4() {
    WWHD_FUNC(0x02550E34, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3501 /* LITHOGRAPH_4 */);
}
VERIFY(0x02550E34, item_getcheck_func_lithograph4);

/* 02550E48 */
s32 item_getcheck_func_lithograph5() {
    WWHD_FUNC(0x02550E48, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3680 /* LITHOGRAPH_5 */);
}
VERIFY(0x02550E48, item_getcheck_func_lithograph5);

/* 02550E5C */
s32 item_getcheck_func_lithograph6() {
    WWHD_FUNC(0x02550E5C, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3640 /* LITHOGRAPH_6 */);
}
VERIFY(0x02550E5C, item_getcheck_func_lithograph6);

/* 02550E70 */
s32 item_getcheck_func_lithograph7() {
    WWHD_FUNC(0x02550E70, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3620 /* LITHOGRAPH_7 */);
}
VERIFY(0x02550E70, item_getcheck_func_lithograph7);

/* 02550E84 */
s32 item_getcheck_func_lithograph8() {
    WWHD_FUNC(0x02550E84, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3610 /* LITHOGRAPH_8 */);
}
VERIFY(0x02550E84, item_getcheck_func_lithograph8);

/* 02550E98 */
s32 item_getcheck_func_lithograph9() {
    WWHD_FUNC(0x02550E98, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3608 /* LITHOGRAPH_9 */);
}
VERIFY(0x02550E98, item_getcheck_func_lithograph9);

/* 02550EAC */
s32 item_getcheck_func_lithograph10() {
    WWHD_FUNC(0x02550EAC, s32);
    return dSv_event_isEventBit_l(svbase() + SV_EVENT, 0x3604 /* LITHOGRAPH_10 */);
}
VERIFY(0x02550EAC, item_getcheck_func_lithograph10);

/* 02550EC0 */
s32 item_getcheck_func_lithograph11() {
    WWHD_FUNC(0x02550EC0, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 60 - 1) /* dComIfGs_isGetCollectMap(60) */;
}
VERIFY(0x02550EC0, item_getcheck_func_lithograph11);

/* 02550ED4 */
s32 item_getcheck_func_lithograph12() {
    WWHD_FUNC(0x02550ED4, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 59 - 1) /* dComIfGs_isGetCollectMap(59) */;
}
VERIFY(0x02550ED4, item_getcheck_func_lithograph12);

/* 02550EE8 */
s32 item_getcheck_func_lithograph13() {
    WWHD_FUNC(0x02550EE8, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 58 - 1) /* dComIfGs_isGetCollectMap(58) */;
}
VERIFY(0x02550EE8, item_getcheck_func_lithograph13);

/* 02550EFC */
s32 item_getcheck_func_lithograph14() {
    WWHD_FUNC(0x02550EFC, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 57 - 1) /* dComIfGs_isGetCollectMap(57) */;
}
VERIFY(0x02550EFC, item_getcheck_func_lithograph14);

/* 02550F10 */
s32 item_getcheck_func_lithograph15() {
    WWHD_FUNC(0x02550F10, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 56 - 1) /* dComIfGs_isGetCollectMap(56) */;
}
VERIFY(0x02550F10, item_getcheck_func_lithograph15);

/* 02550F24 */
s32 item_getcheck_func_lithograph16() {
    WWHD_FUNC(0x02550F24, s32);
    return dSv_map_isGetMap_l(svbase() + SV_MAP, 55 - 1) /* dComIfGs_isGetCollectMap(55) */;
}
VERIFY(0x02550F24, item_getcheck_func_lithograph16);

}  // namespace d_item_4_cpp
