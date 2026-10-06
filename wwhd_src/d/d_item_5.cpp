/* d_item part 5: getRotenItemNumInBag .. getItemFromLifeBallTableWithoutEmono, __sinit, the
 * grouped empty item_func_* and the `return -1` item_getcheck_func_* (02550F38..02551B8F).
 * WWHD. See d_item.cpp for the unit's range.
 *
 * The life-ball tables: the character data table (cDT) is at play + 0x50A0; its ITEM0..7
 * column numbers at +0x70.. and NITEM0..15 at +0x2C.. (GameCube dComIfGp_CharTbl()). */
#include "bindings.h"

namespace d_item_5_cpp {
#include "d_item_local.h"

/* checkItemGet (d_item.cpp) */
static inline s32 checkItemGet(u32 itemNo, s32 defaultVal) { return gabi::call<s32>(0x0254DA50, itemNo, defaultVal); }

/* 02550F38 */
s32 getRotenItemNumInBag() {
    WWHD_FUNC(0x02550F38, s32);
    s32 num = 0;
    u8 i = 0x8C; /* dItemNo_TOWN_FLOWER_e .. dItemNo_SHOP_GURU_STATUE_e */
    for (s32 n = 12; n != 0; n--) {
        num += dSv_bag_checkReserveItem_l(svbase() + SV_BAGITEM, i); /* dComIfGs_checkReserveItem */
        i++;
    }
    return num;
}
VERIFY(0x02550F38, getRotenItemNumInBag);

/* 02550FAC */
BOOL isDaizaItem(u8 itemNo) {
    WWHD_FUNC(0x02550FAC, BOOL, itemNo);
    BOOL isDaiza = FALSE;
    if (itemNo == 0x8C /* dItemNo_TOWN_FLOWER_e */ || itemNo == 0x8D || itemNo == 0x8E || itemNo == 0x8F ||
        itemNo == 0x90 || itemNo == 0x91 || itemNo == 0x92 || itemNo == 0x93 || itemNo == 0x94 ||
        itemNo == 0x95 || itemNo == 0x96 || itemNo == 0x97 /* dItemNo_SHOP_GURU_STATUE_e */)
    {
        isDaiza = TRUE;
    }
    return isDaiza;
}
VERIFY(0x02550FAC, isDaizaItem);

/* 02550FC4 */
BOOL isBomb(u8 itemNo) {
    WWHD_FUNC(0x02550FC4, BOOL, itemNo);
    BOOL isBomb = FALSE;
    if (itemNo == 0xB /* dItemNo_BOMB_5_e */ || itemNo == 0xC || itemNo == 0xD || itemNo == 0xE /* dItemNo_BOMB_30_e */) {
        isBomb = TRUE;
    }
    return isBomb;
}
VERIFY(0x02550FC4, isBomb);

/* 02550FDC */
BOOL isArrow(u8 itemNo) {
    WWHD_FUNC(0x02550FDC, BOOL, itemNo);
    BOOL isArrow = FALSE;
    if (itemNo == 0x10 /* dItemNo_ARROW_10_e */ || itemNo == 0x11 || itemNo == 0x12 /* dItemNo_ARROW_30_e */) {
        isArrow = TRUE;
    }
    return isArrow;
}
VERIFY(0x02550FDC, isArrow);

/* 02550FF4 */
BOOL isEmono(u8 itemNo) {
    WWHD_FUNC(0x02550FF4, BOOL, itemNo);
    BOOL isEmono = FALSE;
    if (itemNo == 0x1F /* dItemNo_JOY_PENDANT_e */ || itemNo == 0x45 /* dItemNo_SKULL_NECKLACE_e */ ||
        itemNo == 0x46 || itemNo == 0x47 || itemNo == 0x48 || itemNo == 0x49 || itemNo == 0x4A ||
        itemNo == 0x4B /* dItemNo_BLUE_JELLY_e */)
    {
        isEmono = TRUE;
    }
    return isEmono;
}
VERIFY(0x02550FF4, isEmono);

/* 0255101C */
BOOL isEsa(u8 itemNo) {
    WWHD_FUNC(0x0255101C, BOOL, itemNo);
    BOOL isEsa = FALSE;
    if (itemNo == 0x82 /* dItemNo_BIRD_BAIT_5_e */ || itemNo == 0x83 /* dItemNo_HYOI_PEAR_e */ || itemNo == 0x89 /* dItemNo_MAGIC_BEAN_e */) {
        isEsa = TRUE;
    }
    return isEsa;
}
VERIFY(0x0255101C, isEsa);

/* 02551044 */
BOOL isRupee(u8 itemNo) {
    WWHD_FUNC(0x02551044, BOOL, itemNo);
    BOOL isRupee = FALSE;
    if (itemNo == 0x1 /* dItemNo_GREEN_RUPEE_e */ || itemNo == 0x2 || itemNo == 0x3 || itemNo == 0x4 ||
        itemNo == 0x5 || itemNo == 0x6 /* dItemNo_ORANGE_RUPEE_e */ || itemNo == 0xF /* dItemNo_SILVER_RUPEE_e */)
    {
        isRupee = TRUE;
    }
    return isRupee;
}
VERIFY(0x02551044, isRupee);

/* 0255106C */
BOOL isLimitedItem(u8 itemNo) {
    WWHD_FUNC(0x0255106C, BOOL, itemNo);
    BOOL isLimited = FALSE;
    if (itemNo == 0x4B /* dItemNo_BLUE_JELLY_e */) {
        isLimited = TRUE;
    }
    return isLimited;
}
VERIFY(0x0255106C, isLimitedItem);

/* 02551080 */
BOOL isNonSavedEmono(u8 itemNo) {
    WWHD_FUNC(0x02551080, BOOL, itemNo);
    BOOL isEmono = FALSE;
    if (itemNo == 0x1F || itemNo == 0x45 || itemNo == 0x46 || itemNo == 0x47 ||
        itemNo == 0x48 || itemNo == 0x49 || itemNo == 0x4A) // No dItemNo_BLUE_JELLY_e
    {
        isEmono = TRUE;
    }
    return isEmono;
}
VERIFY(0x02551080, isNonSavedEmono);

/* 025510A8 */
BOOL isUseClothPacket(u8 itemNo) {
    WWHD_FUNC(0x025510A8, BOOL, itemNo);
    BOOL isCloth = FALSE;
    if (itemNo == 0x8F /* dItemNo_HEROS_FLAG_e */ || itemNo == 0x90 /* dItemNo_BIG_CATCH_FLAG_e */ ||
        itemNo == 0x91 /* dItemNo_BIG_SALE_FLAG_e */ || itemNo == 0x93 /* dItemNo_SICKLE_MOON_FLAG_e */) {
        isCloth = TRUE;
    }
    return isCloth;
}
VERIFY(0x025510A8, isUseClothPacket);

/* 025510D0 (unnamed by the matcher) */
BOOL isTriforce(u8 itemNo) {
    WWHD_FUNC(0x025510D0, BOOL, itemNo);
    BOOL isTriforce = FALSE;
    if (itemNo == 0x61 /* dItemNo_TRIFORCE1_e */ || itemNo == 0x62 || itemNo == 0x63 || itemNo == 0x64 ||
        itemNo == 0x65 || itemNo == 0x66 || itemNo == 0x67 || itemNo == 0x68 /* dItemNo_TRIFORCE8_e */)
    {
        isTriforce = TRUE;
    }
    return isTriforce;
}
VERIFY(0x025510D0, isTriforce);

/* 025510E8 */
BOOL isHeart(u8 itemNo) {
    WWHD_FUNC(0x025510E8, BOOL, itemNo);
    BOOL isHeart = FALSE;
    if (itemNo == 0x0 /* dItemNo_HEART_e */ || itemNo == 0x1E /* dItemNo_TRIPLE_HEART_e */) {
        isHeart = TRUE;
    }
    return isHeart;
}
VERIFY(0x025510E8, isHeart);

/* 02551108 */
u8 getItemNoByLife(u8 itemNo) {
    WWHD_FUNC(0x02551108, u8, itemNo);
    u32 b = svbase();
    s32 life = ld16(b + SV_LIFE);           /* dComIfGs_getLife() */
    s32 max = ld16(b + SV_MAXLIFE) & 0xFC;  /* dComIfGs_getMaxLife() & 0xFC */
    u8 lifePercent = (u8)ppc_divw((u32)(100 * life), (u32)max);
    if (lifePercent != 100) {
        return itemNo;
    }
    if (itemNo == 0x0 /* dItemNo_HEART_e */) {
        itemNo = 0x1; /* dItemNo_GREEN_RUPEE_e */
    }
    if (itemNo == 0x1E /* dItemNo_TRIPLE_HEART_e */) {
        return 0x3; /* dItemNo_YELLOW_RUPEE_e */
    }
    return itemNo;
}
VERIFY(0x02551108, getItemNoByLife);

/* 02551150 */
u8 check_itemno(s32 itemNo) {
    WWHD_FUNC(0x02551150, u8, itemNo);
    u32 b = svbase();
    if (ld8(b + SV_MAXMAGIC) == 0 /* dComIfGs_getMaxMagic() */ && (itemNo == 0x9 /* dItemNo_SMALL_MAGIC_e */ || itemNo == 0xA /* dItemNo_LARGE_MAGIC_e */)) {
        return 0x1; /* dItemNo_GREEN_RUPEE_e */
    }
    if (!dSv_get_item_isItem_l(b + SV_GETITEM, 0xC, 0) && !dComIfGs_isGetItem(0xC, 1) && !dComIfGs_isGetItem(0xC, 2)) {
        // Does not own any bow.
        if (isArrow((u8)itemNo)) {
            return 0x1;
        }
    }
    if (!dComIfGs_isGetItem(0xD, 0)) {
        // Does not own bombs.
        if (isBomb((u8)itemNo)) {
            return 0x1;
        }
    }
    if (!checkItemGet(0x2C /* dItemNo_BAIT_BAG_e */, TRUE)) {
        if (isEsa((u8)itemNo)) {
            return 0x1;
        }
    }
    if (!checkItemGet(0x24 /* dItemNo_SPOILS_BAG_e */, TRUE)) {
        if (isEmono((u8)itemNo)) {
            return 0x1;
        }
    }
    if (itemNo == 0x1E /* dItemNo_TRIPLE_HEART_e */) {
        itemNo = 0x0; /* dItemNo_HEART_e */
    }
    return (u8)itemNo;
}
VERIFY(0x02551150, check_itemno);

/* dComIfGp_CharTbl()->GetInf(dComIfGp_CharTbl()->Get<column>(), row): the table pointer is
 * taken first, then the column number through a second dComIfGp_get */
static inline u8 lifeBallItem_inl(u32 col, u16 row) {
    u32 tbl = play() + 0x50A0;
    u32 c = ld(play() + 0x50A0 + col);
    return cDT_GetInf_l(tbl, c, row);
}

/* 025512A4 */
u8 getEmonoItemFromLifeBallTable(u16 itemTableIdx) {
    WWHD_FUNC(0x025512A4, u8, itemTableIdx);
    u8 items[16];

    for (int i = 0; i < 8; i++) {
        items[i] = lifeBallItem_inl(0x70 + 4 * i, itemTableIdx); /* GetITEM0..7 */
    }
    for (int i = 0; i < 8; i++) {
        if (isEmono(items[i])) {
            return items[i];
        }
    }

    for (int i = 0; i < 16; i++) {
        items[i] = lifeBallItem_inl(0x2C + 4 * i, itemTableIdx); /* GetNITEM0..15 */
    }
    for (int i = 0; i < 16; i++) {
        if (isEmono(items[i])) {
            return items[i];
        }
    }

    return 0xFF; /* dItemNo_NONE_e */
}
VERIFY(0x025512A4, getEmonoItemFromLifeBallTable);

/* 02551684 */
u8 getItemFromLifeBallTableWithoutEmono(u16 itemTableIdx) {
    WWHD_FUNC(0x02551684, u8, itemTableIdx);
    u8 items[16];

    for (int i = 0; i < 16; i++) {
        items[i] = lifeBallItem_inl(0x2C + 4 * i, itemTableIdx); /* GetNITEM0..15 */
    }

    for (int i = 0; i < 8; i++) {
        int randIdx = gabi::ftoi(cM_rndF(15.999f));
        if (!isEmono(items[randIdx]) && items[randIdx] != 0xFF) {
            return items[randIdx];
        }
    }

    for (int i = 0; i < 16; i++) {
        if (!isEmono(items[i]) && items[i] != 0xFF) {
            return items[i];
        }
    }

    return 0xFF; /* dItemNo_NONE_e */
}
VERIFY(0x02551684, getItemFromLifeBallTableWithoutEmono);

/* 0255198C: __sinit_d_item_cpp (new: compiler-generated static initialisation of the header
 * globals this unit includes; the same pattern as the neighbouring units' __sinit) */
void __sinit_d_item_cpp() {
    WWHD_FUNC(0x0255198C, void);
    st(0x104759E8 + 0xC, 0);
    st(0x104759E8 + 0x8, 0);
    st(0x104759E8 + 0x4, 0);
    st(0x104759E8 + 0x0, 0);
    gabi::call(0x028F026C, 0x101E4298); /* __register_global_object */
    f32 lo = ldf(0x1004E3FC), hi = ldf(0x1004E400); /* -pi, pi */
    stf(0x104759CC, lo);
    stf(0x104759D0, hi);
    gabi::call(0x028ED6F8, 0x104759E4);
    gabi::call(0x028F026C, 0x101E42A4);
    gabi::call(0x028EAB2C, 0x104759E5);
    gabi::call(0x028F026C, 0x101E42B0);
    f32 a = ldf(0x1004E404), c = ldf(0x1004E408); /* 50000.0, 10000.0 */
    stf(0x104759D4, a);
    stf(0x104759DC, c);
    stf(0x104759D8, a);
    stf(0x104759E0, c);
}
VERIFY(0x0255198C, __sinit_d_item_cpp);

/* GHS grouped the empty item functions here (one copy per GameCube function) */

/* 02551A50 */
void item_func_triple_heart() {
    WWHD_FUNC(0x02551A50, void);
}
VERIFY(0x02551A50, item_func_triple_heart);

/* 02551A54 */
void item_func_water_boots() {
    WWHD_FUNC(0x02551A54, void);
}
VERIFY(0x02551A54, item_func_water_boots);

/* 02551A58 */
void item_func_fuku() {
    WWHD_FUNC(0x02551A58, void);
}
VERIFY(0x02551A58, item_func_fuku);

/* 02551A5C */
void item_func_grass_ball() {
    WWHD_FUNC(0x02551A5C, void);
}
VERIFY(0x02551A5C, item_func_grass_ball);

/* 02551A60 */
void item_func_bin() {
    WWHD_FUNC(0x02551A60, void);
}
VERIFY(0x02551A60, item_func_bin);

/* 02551A64 */
void item_func_knowledge_tf() {
    WWHD_FUNC(0x02551A64, void);
}
VERIFY(0x02551A64, item_func_knowledge_tf);

/* 02551A68 */
void item_func_triforce_map1() {
    WWHD_FUNC(0x02551A68, void);
}
VERIFY(0x02551A68, item_func_triforce_map1);

/* 02551A6C */
void item_func_triforce_map2() {
    WWHD_FUNC(0x02551A6C, void);
}
VERIFY(0x02551A6C, item_func_triforce_map2);

/* 02551A70 */
void item_func_triforce_map3() {
    WWHD_FUNC(0x02551A70, void);
}
VERIFY(0x02551A70, item_func_triforce_map3);

/* 02551A74 */
void item_func_triforce_map4() {
    WWHD_FUNC(0x02551A74, void);
}
VERIFY(0x02551A74, item_func_triforce_map4);

/* 02551A78 */
void item_func_triforce_map5() {
    WWHD_FUNC(0x02551A78, void);
}
VERIFY(0x02551A78, item_func_triforce_map5);

/* 02551A7C */
void item_func_triforce_map6() {
    WWHD_FUNC(0x02551A7C, void);
}
VERIFY(0x02551A7C, item_func_triforce_map6);

/* 02551A80 */
void item_func_triforce_map7() {
    WWHD_FUNC(0x02551A80, void);
}
VERIFY(0x02551A80, item_func_triforce_map7);

/* 02551A84 */
void item_func_triforce_map8() {
    WWHD_FUNC(0x02551A84, void);
}
VERIFY(0x02551A84, item_func_triforce_map8);

/* 02551A88 */
void item_func_esa1() {
    WWHD_FUNC(0x02551A88, void);
}
VERIFY(0x02551A88, item_func_esa1);

/* 02551A8C */
void item_func_esa2() {
    WWHD_FUNC(0x02551A8C, void);
}
VERIFY(0x02551A8C, item_func_esa2);

/* 02551A90 */
void item_func_esa3() {
    WWHD_FUNC(0x02551A90, void);
}
VERIFY(0x02551A90, item_func_esa3);

/* 02551A94 */
void item_func_esa4() {
    WWHD_FUNC(0x02551A94, void);
}
VERIFY(0x02551A94, item_func_esa4);

/* 02551A98 */
void item_func_esa5() {
    WWHD_FUNC(0x02551A98, void);
}
VERIFY(0x02551A98, item_func_esa5);

/* 02551A9C */
void item_func_magic_bean() {
    WWHD_FUNC(0x02551A9C, void);
}
VERIFY(0x02551A9C, item_func_magic_bean);

/* 02551AA0 */
void item_func_bird_esa_10() {
    WWHD_FUNC(0x02551AA0, void);
}
VERIFY(0x02551AA0, item_func_bird_esa_10);

/* 02551AA4 */
void item_func_salvage_item1() {
    WWHD_FUNC(0x02551AA4, void);
}
VERIFY(0x02551AA4, item_func_salvage_item1);

/* 02551AA8 */
void item_func_tincle_statue01() {
    WWHD_FUNC(0x02551AA8, void);
}
VERIFY(0x02551AA8, item_func_tincle_statue01);

/* 02551AAC */
void item_func_tincle_statue02() {
    WWHD_FUNC(0x02551AAC, void);
}
VERIFY(0x02551AAC, item_func_tincle_statue02);

/* 02551AB0 */
void item_func_tincle_statue03() {
    WWHD_FUNC(0x02551AB0, void);
}
VERIFY(0x02551AB0, item_func_tincle_statue03);

/* 02551AB4 */
void item_func_tincle_statue04() {
    WWHD_FUNC(0x02551AB4, void);
}
VERIFY(0x02551AB4, item_func_tincle_statue04);

/* 02551AB8 */
void item_func_tincle_statue05() {
    WWHD_FUNC(0x02551AB8, void);
}
VERIFY(0x02551AB8, item_func_tincle_statue05);

/* 02551ABC */
void item_func_tincle_statue06() {
    WWHD_FUNC(0x02551ABC, void);
}
VERIFY(0x02551ABC, item_func_tincle_statue06);

/* 02551AC0 */
void item_func_magic_power() {
    WWHD_FUNC(0x02551AC0, void);
}
VERIFY(0x02551AC0, item_func_magic_power);

/* 02551AC4 */
void item_func_noentry() {
    WWHD_FUNC(0x02551AC4, void);
}
VERIFY(0x02551AC4, item_func_noentry);

/* ... and the item_getcheck_func_* that return -1 (no get check) */

/* 02551AC8 */
s32 item_getcheck_func_heart() {
    WWHD_FUNC(0x02551AC8, s32);
    return -1;
}
VERIFY(0x02551AC8, item_getcheck_func_heart);

/* 02551AD0 */
s32 item_getcheck_func_green_rupee() {
    WWHD_FUNC(0x02551AD0, s32);
    return -1;
}
VERIFY(0x02551AD0, item_getcheck_func_green_rupee);

/* 02551AD8 */
s32 item_getcheck_func_blue_rupee() {
    WWHD_FUNC(0x02551AD8, s32);
    return -1;
}
VERIFY(0x02551AD8, item_getcheck_func_blue_rupee);

/* 02551AE0 */
s32 item_getcheck_func_white_rupee() {
    WWHD_FUNC(0x02551AE0, s32);
    return -1;
}
VERIFY(0x02551AE0, item_getcheck_func_white_rupee);

/* 02551AE8 */
s32 item_getcheck_func_red_rupee() {
    WWHD_FUNC(0x02551AE8, s32);
    return -1;
}
VERIFY(0x02551AE8, item_getcheck_func_red_rupee);

/* 02551AF0 */
s32 item_getcheck_func_purple_rupee() {
    WWHD_FUNC(0x02551AF0, s32);
    return -1;
}
VERIFY(0x02551AF0, item_getcheck_func_purple_rupee);

/* 02551AF8 */
s32 item_getcheck_func_silver_rupee() {
    WWHD_FUNC(0x02551AF8, s32);
    return -1;
}
VERIFY(0x02551AF8, item_getcheck_func_silver_rupee);

/* 02551B00 */
s32 item_getcheck_func_kakera_heart() {
    WWHD_FUNC(0x02551B00, s32);
    return -1;
}
VERIFY(0x02551B00, item_getcheck_func_kakera_heart);

/* 02551B08 */
s32 item_getcheck_func_utuwa_heart() {
    WWHD_FUNC(0x02551B08, s32);
    return -1;
}
VERIFY(0x02551B08, item_getcheck_func_utuwa_heart);

/* 02551B10 */
s32 item_getcheck_func_s_magic() {
    WWHD_FUNC(0x02551B10, s32);
    return -1;
}
VERIFY(0x02551B10, item_getcheck_func_s_magic);

/* 02551B18 */
s32 item_getcheck_func_l_magic() {
    WWHD_FUNC(0x02551B18, s32);
    return -1;
}
VERIFY(0x02551B18, item_getcheck_func_l_magic);

/* 02551B20 */
s32 item_getcheck_func_noentry() {
    WWHD_FUNC(0x02551B20, s32);
    return -1;
}
VERIFY(0x02551B20, item_getcheck_func_noentry);

/* 02551B28: HD-only item 0x14 (GameCube: noentry) */
s32 item_getcheck_func_hd_14() {
    WWHD_FUNC(0x02551B28, s32);
    return -1;
}
VERIFY(0x02551B28, item_getcheck_func_hd_14);

/* 02551B30 */
s32 item_getcheck_func_small_key() {
    WWHD_FUNC(0x02551B30, s32);
    return -1;
}
VERIFY(0x02551B30, item_getcheck_func_small_key);

/* 02551B38 */
s32 item_getcheck_func_recover_faily() {
    WWHD_FUNC(0x02551B38, s32);
    return -1;
}
VERIFY(0x02551B38, item_getcheck_func_recover_faily);

/* 02551B40 */
s32 item_getcheck_func_triple_heart() {
    WWHD_FUNC(0x02551B40, s32);
    return -1;
}
VERIFY(0x02551B40, item_getcheck_func_triple_heart);

/* 02551B48 */
s32 item_getcheck_func_water_boots() {
    WWHD_FUNC(0x02551B48, s32);
    return -1;
}
VERIFY(0x02551B48, item_getcheck_func_water_boots);

/* 02551B50 */
s32 item_getcheck_func_bare_hand() {
    WWHD_FUNC(0x02551B50, s32);
    return -1;
}
VERIFY(0x02551B50, item_getcheck_func_bare_hand);

/* 02551B58 */
s32 item_getcheck_func_bomb_bag() {
    WWHD_FUNC(0x02551B58, s32);
    return -1;
}
VERIFY(0x02551B58, item_getcheck_func_bomb_bag);

/* 02551B60 */
s32 item_getcheck_func_grass_ball() {
    WWHD_FUNC(0x02551B60, s32);
    return -1;
}
VERIFY(0x02551B60, item_getcheck_func_grass_ball);

/* 02551B68 */
s32 item_getcheck_func_compass() {
    WWHD_FUNC(0x02551B68, s32);
    return -1;
}
VERIFY(0x02551B68, item_getcheck_func_compass);

/* 02551B70 */
s32 item_getcheck_func_zora_sail() {
    WWHD_FUNC(0x02551B70, s32);
    return -1;
}
VERIFY(0x02551B70, item_getcheck_func_zora_sail);

/* 02551B78 */
s32 item_getcheck_func_tincle_sail() {
    WWHD_FUNC(0x02551B78, s32);
    return -1;
}
VERIFY(0x02551B78, item_getcheck_func_tincle_sail);

/* 02551B80 */
s32 item_getcheck_func_sail() {
    WWHD_FUNC(0x02551B80, s32);
    return -1;
}
VERIFY(0x02551B80, item_getcheck_func_sail);

/* 02551B88 */
s32 item_getcheck_func_magic_bean() {
    WWHD_FUNC(0x02551B88, s32);
    return -1;
}
VERIFY(0x02551B88, item_getcheck_func_magic_bean);

}  // namespace d_item_5_cpp
