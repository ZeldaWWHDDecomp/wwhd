/* d_item part 5: getRotenItemNumInBag .. getItemFromLifeBallTableWithoutEmono, __sinit, the
 * grouped empty item_func_* and the `return -1` item_getcheck_func_* (02550F38..02551B8F).
 * WWHD. See d_item.cpp for the unit's range.
 *
 * The life-ball tables: the character data table (cDT) is at play + 0x50A0; its ITEM0..7
 * column numbers at +0x70.. and NITEM0..15 at +0x2C.. (GameCube dComIfGp_CharTbl()). */
#include "d/d_item_5.h"
// HD: the header object is BSS; descriptors and constants retain their native storage.
static ItemHeaderStatics& l_itemStatics = *gabi::at<ItemHeaderStatics>(0x104759CC);
static ItemDestructorRecord* l_itemDestructors = gabi::at<ItemDestructorRecord>(0x101E4298);
static be<f32>* l_itemConstants = gabi::at<be<f32>>(0x1004E3FC);

/* 02550F38 */
s32 getRotenItemNumInBag() {
    s32 num = 0;
    u8 i = dItemNo_TOWN_FLOWER_e;
    for (s32 n = 12; n != 0; n--) {
        num += dComIfGs_checkReserveItem(i);
        i++;
    }
    return num;
}

/* 02550FAC */
BOOL isDaizaItem(u8 itemNo) {
    BOOL isDaiza = FALSE;
    if (itemNo == dItemNo_TOWN_FLOWER_e || itemNo == dItemNo_SEA_FLOWER_e ||
        itemNo == dItemNo_EXOTIC_FLOWER_e || itemNo == dItemNo_HEROS_FLAG_e ||
        itemNo == dItemNo_BIG_CATCH_FLAG_e || itemNo == dItemNo_BIG_SALE_FLAG_e ||
        itemNo == dItemNo_PINWHEEL_e || itemNo == dItemNo_SICKLE_MOON_FLAG_e ||
        itemNo == dItemNo_SKULL_TOWER_IDOL_e || itemNo == dItemNo_FOUNTAIN_IDOL_e ||
        itemNo == dItemNo_POSTMAN_STATUE_e || itemNo == dItemNo_SHOP_GURU_STATUE_e) {
        isDaiza = TRUE;
    }
    return isDaiza;
}

/* 02550FC4 */
BOOL isBomb(u8 itemNo) {
    BOOL isBomb = FALSE;
    if (itemNo == dItemNo_BOMB_5_e || itemNo == dItemNo_BOMB_10_e || itemNo == dItemNo_BOMB_20_e ||
        itemNo == dItemNo_BOMB_30_e) {
        isBomb = TRUE;
    }
    return isBomb;
}

/* 02550FDC */
BOOL isArrow(u8 itemNo) {
    BOOL isArrow = FALSE;
    if (itemNo == dItemNo_ARROW_10_e || itemNo == dItemNo_ARROW_20_e ||
        itemNo == dItemNo_ARROW_30_e) {
        isArrow = TRUE;
    }
    return isArrow;
}

/* 02550FF4 */
BOOL isEmono(u8 itemNo) {
    BOOL isEmono = FALSE;
    if (itemNo == dItemNo_JOY_PENDANT_e || itemNo == dItemNo_SKULL_NECKLACE_e ||
        itemNo == dItemNo_BOKOBABA_SEED_e || itemNo == dItemNo_GOLDEN_FEATHER_e ||
        itemNo == dItemNo_KNIGHTS_CREST_e || itemNo == dItemNo_RED_JELLY_e ||
        itemNo == dItemNo_GREEN_JELLY_e || itemNo == dItemNo_BLUE_JELLY_e) {
        isEmono = TRUE;
    }
    return isEmono;
}

/* 0255101C */
BOOL isEsa(u8 itemNo) {
    BOOL isEsa = FALSE;
    if (itemNo == dItemNo_BIRD_BAIT_5_e || itemNo == dItemNo_HYOI_PEAR_e ||
        itemNo == dItemNo_MAGIC_BEAN_e) {
        isEsa = TRUE;
    }
    return isEsa;
}

// HD: local implementation alias avoids the shared native isRupee binding.
/* 02551044 */
BOOL item_isRupee(u8 itemNo) {
    BOOL isRupee = FALSE;
    if (itemNo == dItemNo_GREEN_RUPEE_e || itemNo == dItemNo_BLUE_RUPEE_e ||
        itemNo == dItemNo_YELLOW_RUPEE_e || itemNo == dItemNo_RED_RUPEE_e ||
        itemNo == dItemNo_PURPLE_RUPEE_e || itemNo == dItemNo_ORANGE_RUPEE_e ||
        itemNo == dItemNo_SILVER_RUPEE_e) {
        isRupee = TRUE;
    }
    return isRupee;
}

/* 0255106C */
BOOL isLimitedItem(u8 itemNo) {
    BOOL isLimited = FALSE;
    if (itemNo == dItemNo_BLUE_JELLY_e) {
        isLimited = TRUE;
    }
    return isLimited;
}

/* 02551080 */
BOOL isNonSavedEmono(u8 itemNo) {
    BOOL isEmono = FALSE;
    if (itemNo == dItemNo_JOY_PENDANT_e || itemNo == dItemNo_SKULL_NECKLACE_e ||
        itemNo == dItemNo_BOKOBABA_SEED_e || itemNo == dItemNo_GOLDEN_FEATHER_e ||
        itemNo == dItemNo_KNIGHTS_CREST_e || itemNo == dItemNo_RED_JELLY_e ||
        itemNo == dItemNo_GREEN_JELLY_e) // No dItemNo_BLUE_JELLY_e
    {
        isEmono = TRUE;
    }
    return isEmono;
}

/* 025510A8 */
BOOL isUseClothPacket(u8 itemNo) {
    BOOL isCloth = FALSE;
    if (itemNo == dItemNo_HEROS_FLAG_e || itemNo == dItemNo_BIG_CATCH_FLAG_e ||
        itemNo == dItemNo_BIG_SALE_FLAG_e || itemNo == dItemNo_SICKLE_MOON_FLAG_e) {
        isCloth = TRUE;
    }
    return isCloth;
}

/* 025510D0 */
BOOL isTriforce(u8 itemNo) {
    BOOL isTriforce = FALSE;
    if (itemNo == dItemNo_TRIFORCE1_e || itemNo == dItemNo_TRIFORCE2_e ||
        itemNo == dItemNo_TRIFORCE3_e || itemNo == dItemNo_TRIFORCE4_e ||
        itemNo == dItemNo_TRIFORCE5_e || itemNo == dItemNo_TRIFORCE6_e ||
        itemNo == dItemNo_TRIFORCE7_e || itemNo == dItemNo_TRIFORCE8_e) {
        isTriforce = TRUE;
    }
    return isTriforce;
}

/* 025510E8 */
BOOL isHeart(u8 itemNo) {
    BOOL isHeart = FALSE;
    if (itemNo == dItemNo_HEART_e || itemNo == dItemNo_TRIPLE_HEART_e) {
        isHeart = TRUE;
    }
    return isHeart;
}

/* 02551108 */
u8 getItemNoByLife(u8 itemNo) {
    dSv_info_c* save = dComIfGs_info();
    s32 life = (u16)save->mSavedata.mPlayer.mPlayerStatusA.mLife;
    s32 max = (u16)save->mSavedata.mPlayer.mPlayerStatusA.mMaxLife & 0xFC;
    u8 lifePercent = (u8)ppc_divw((u32)(100 * life), (u32)max);
    if (lifePercent != 100) {
        return itemNo;
    }
    if (itemNo == dItemNo_HEART_e) {
        itemNo = dItemNo_GREEN_RUPEE_e;
    }
    if (itemNo == dItemNo_TRIPLE_HEART_e) {
        return dItemNo_YELLOW_RUPEE_e;
    }
    return itemNo;
}

/* 02551150 */
u8 check_itemno(s32 itemNo) {
    dSv_info_c* save = dComIfGs_info();
    if ((u8)save->mSavedata.mPlayer.mPlayerStatusA.mMaxMagic == 0 &&
        (itemNo == dItemNo_SMALL_MAGIC_e || itemNo == dItemNo_LARGE_MAGIC_e)) {
        return dItemNo_GREEN_RUPEE_e;
    }
    if (!itemDrop_isGetItem(&save->mSavedata.mPlayer.mGetItem, 0xC, 0) &&
        !dComIfGs_isGetItem(0xC, 1) && !dComIfGs_isGetItem(0xC, 2)) {
        // Does not own any bow.
        if (isArrow((u8)itemNo)) {
            return dItemNo_GREEN_RUPEE_e;
        }
    }
    if (!dComIfGs_isGetItem(0xD, 0)) {
        // Does not own bombs.
        if (isBomb((u8)itemNo)) {
            return dItemNo_GREEN_RUPEE_e;
        }
    }
    if (!checkItemGet(dItemNo_BAIT_BAG_e, TRUE)) {
        if (isEsa((u8)itemNo)) {
            return dItemNo_GREEN_RUPEE_e;
        }
    }
    if (!checkItemGet(dItemNo_SPOILS_BAG_e, TRUE)) {
        if (isEmono((u8)itemNo)) {
            return dItemNo_GREEN_RUPEE_e;
        }
    }
    if (itemNo == dItemNo_TRIPLE_HEART_e) {
        itemNo = dItemNo_HEART_e;
    }
    return (u8)itemNo;
}

// HD: fetch the table first, then reload the play object for its column index.
static u8 lifeBallItem(bool itemColumn, u32 index, u16 row) {
    dADM_CharTbl* table = itemCharTbl();
    u32 column = itemColumn ? (u32)itemCharTbl()->mIndex_ITEM[index]
                            : (u32)itemCharTbl()->mIndex_N_ITEM[index];
    return itemTable_GetInf(table, column, row);
}

/* 025512A4 */
u8 getEmonoItemFromLifeBallTable(u16 itemTableIdx) {
    u8 items[16];

    for (int i = 0; i < 8; i++) {
        items[i] = lifeBallItem(true, i, itemTableIdx); /* GetITEM0..7 */
    }
    for (int i = 0; i < 8; i++) {
        if (isEmono(items[i])) {
            return items[i];
        }
    }

    for (int i = 0; i < 16; i++) {
        items[i] = lifeBallItem(false, i, itemTableIdx); /* GetNITEM0..15 */
    }
    for (int i = 0; i < 16; i++) {
        if (isEmono(items[i])) {
            return items[i];
        }
    }

    return dItemNo_NONE_e;
}

/* 02551684 */
u8 getItemFromLifeBallTableWithoutEmono(u16 itemTableIdx) {
    u8 items[16];

    for (int i = 0; i < 16; i++) {
        items[i] = lifeBallItem(false, i, itemTableIdx); /* GetNITEM0..15 */
    }

    for (int i = 0; i < 8; i++) {
        int randIdx = gabi::ftoi(cM_rndF(15.999f));
        if (!isEmono(items[randIdx]) && items[randIdx] != dItemNo_NONE_e) {
            return items[randIdx];
        }
    }

    for (int i = 0; i < 16; i++) {
        if (!isEmono(items[i]) && items[i] != dItemNo_NONE_e) {
            return items[i];
        }
    }

    return dItemNo_NONE_e;
}

// HD: compiler-generated header statics include two extra float pairs.
/* 0255198C */
void __sinit_d_item_cpp() {
    l_itemStatics.mZero[3] = 0;
    l_itemStatics.mZero[2] = 0;
    l_itemStatics.mZero[1] = 0;
    l_itemStatics.mZero[0] = 0;
    __register_global_object(gabi::ea(&l_itemDestructors[0]));
    f32 lo = l_itemConstants[0], hi = l_itemConstants[1];
    l_itemStatics.mMinAngle = lo;
    l_itemStatics.mMaxAngle = hi;
    itemHeader_constructFirst(&l_itemStatics.mFirstObject);
    __register_global_object(gabi::ea(&l_itemDestructors[1]));
    itemHeader_constructSecond(&l_itemStatics.mSecondObject);
    __register_global_object(gabi::ea(&l_itemDestructors[2]));
    f32 a = l_itemConstants[2], c = l_itemConstants[3];
    l_itemStatics.mRanges[0] = a;
    l_itemStatics.mRanges[2] = c;
    l_itemStatics.mRanges[1] = a;
    l_itemStatics.mRanges[3] = c;
}

/* GHS grouped the empty item functions here (one copy per GameCube function) */

/* 02551A50 */
void item_func_triple_heart() {}

/* 02551A54 */
void item_func_water_boots() {}

/* 02551A58 */
void item_func_fuku() {}

/* 02551A5C */
void item_func_grass_ball() {}

/* 02551A60 */
void item_func_bin() {}

/* 02551A64 */
void item_func_knowledge_tf() {}

/* 02551A68 */
void item_func_triforce_map1() {}

/* 02551A6C */
void item_func_triforce_map2() {}

/* 02551A70 */
void item_func_triforce_map3() {}

/* 02551A74 */
void item_func_triforce_map4() {}

/* 02551A78 */
void item_func_triforce_map5() {}

/* 02551A7C */
void item_func_triforce_map6() {}

/* 02551A80 */
void item_func_triforce_map7() {}

/* 02551A84 */
void item_func_triforce_map8() {}

/* 02551A88 */
void item_func_esa1() {}

/* 02551A8C */
void item_func_esa2() {}

/* 02551A90 */
void item_func_esa3() {}

/* 02551A94 */
void item_func_esa4() {}

/* 02551A98 */
void item_func_esa5() {}

/* 02551A9C */
void item_func_magic_bean() {}

/* 02551AA0 */
void item_func_bird_esa_10() {}

/* 02551AA4 */
void item_func_salvage_item1() {}

/* 02551AA8 */
void item_func_tincle_statue01() {}

/* 02551AAC */
void item_func_tincle_statue02() {}

/* 02551AB0 */
void item_func_tincle_statue03() {}

/* 02551AB4 */
void item_func_tincle_statue04() {}

/* 02551AB8 */
void item_func_tincle_statue05() {}

/* 02551ABC */
void item_func_tincle_statue06() {}

/* 02551AC0 */
void item_func_magic_power() {}

/* 02551AC4 */
void item_func_noentry() {}

/* ... and the item_getcheck_func_* that return -1 (no get check) */

/* 02551AC8 */
s32 item_getcheck_func_heart() {
    return -1;
}

/* 02551AD0 */
s32 item_getcheck_func_green_rupee() {
    return -1;
}

/* 02551AD8 */
s32 item_getcheck_func_blue_rupee() {
    return -1;
}

/* 02551AE0 */
s32 item_getcheck_func_white_rupee() {
    return -1;
}

/* 02551AE8 */
s32 item_getcheck_func_red_rupee() {
    return -1;
}

/* 02551AF0 */
s32 item_getcheck_func_purple_rupee() {
    return -1;
}

/* 02551AF8 */
s32 item_getcheck_func_silver_rupee() {
    return -1;
}

/* 02551B00 */
s32 item_getcheck_func_kakera_heart() {
    return -1;
}

/* 02551B08 */
s32 item_getcheck_func_utuwa_heart() {
    return -1;
}

/* 02551B10 */
s32 item_getcheck_func_s_magic() {
    return -1;
}

/* 02551B18 */
s32 item_getcheck_func_l_magic() {
    return -1;
}

/* 02551B20 */
s32 item_getcheck_func_noentry() {
    return -1;
}

// HD: item 0x14 has a separate no-check entry.
/* 02551B28 */
s32 item_getcheck_func_hd_14() {
    return -1;
}

/* 02551B30 */
s32 item_getcheck_func_small_key() {
    return -1;
}

/* 02551B38 */
s32 item_getcheck_func_recover_faily() {
    return -1;
}

/* 02551B40 */
s32 item_getcheck_func_triple_heart() {
    return -1;
}

/* 02551B48 */
s32 item_getcheck_func_water_boots() {
    return -1;
}

/* 02551B50 */
s32 item_getcheck_func_bare_hand() {
    return -1;
}

/* 02551B58 */
s32 item_getcheck_func_bomb_bag() {
    return -1;
}

/* 02551B60 */
s32 item_getcheck_func_grass_ball() {
    return -1;
}

/* 02551B68 */
s32 item_getcheck_func_compass() {
    return -1;
}

/* 02551B70 */
s32 item_getcheck_func_zora_sail() {
    return -1;
}

/* 02551B78 */
s32 item_getcheck_func_tincle_sail() {
    return -1;
}

/* 02551B80 */
s32 item_getcheck_func_sail() {
    return -1;
}

/* 02551B88 */
s32 item_getcheck_func_magic_bean() {
    return -1;
}
/* WWHD glue
ext 0254DA50 s32 checkItemGet(u32 itemNo, s32 defaultVal)
ext 025B5D40 s32 itemDrop_isGetItem(dSv_player_get_item_c* items, s32 slot, u32 bit)
ext 0200EA74 u8 itemTable_GetInf(cDT* table, u32 column, u32 row)
ext 028ED6F8 void itemHeader_constructFirst(be<u8>* object)
ext 028EAB2C void itemHeader_constructSecond(be<u8>* object)
*/
