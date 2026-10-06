/* d_save: save data (dSv_player_*, dSv_memBit_c, dSv_event_c, dSv_zone_c, dSv_info_c ...), WWHD.
 *
 * Translation unit 025B4DC0..025BAF8F, from the image: d_salvage ends with its member getters
 * (025B4C54..025B4D28) and its __sinit (025B4D2C); d_save starts with
 * dSv_player_status_a_c::init (025B4DC0), follows the GameCube order and ends with its __sinit
 * (025BAEFC). d_save_init follows with only its __sinit (025BAF90: setInitEventBit is not in the
 * image), then d_scope (5 empty process methods 025BB024..025BB048 and __sinit 025BB04C).
 * Ported from the GameCube d_save.cpp; HD-only functions are written from the WWHD code.
 *
 * Parts: d_save.cpp (player status / return place / item / get item / item record / item max),
 * d_save_2.cpp (bag items, get bag items, bag record, collect), d_save_3.cpp (map, player info,
 * config, priest, player, ocean, event), d_save_4.cpp (memBit, danBit, zoneBit, zoneActor, zone,
 * restart, reserve, save, turnRestart), d_save_5.cpp (dSv_info_c, __sinit).
 *
 * Save layout (HD, from the code): the save info is at *(101F84DC) + 0x20 (dSv_info_c; its
 * dSv_save_c first). dSv_player_status_a_c at info+0: max life 0, life 2, rupees 4, 6, 8,
 * select items [5] at 9 (HD: five item buttons, GameCube three), select equip [4] at 0xE,
 * 0x12..0x16. dSv_player_status_b_c: date (u64) 0, 8 (f32), time 0xC (f32; HD initial 150.0,
 * GameCube 165.0), date 0x10, tact wind angles 0x12/0x14. dSv_player_item_c at info+0x3C
 * (21 slots, bottles at 0xE..0x11); bag beast/bait/reserve items (dSv_player_bag_item_c) at
 * info+0x76/0x7E/0x86 (inventory slots 0x18..0x1F, 0x24..0x2B, 0x30..0x37).
 * The play object (025200D4): select item numbers at +0x5BBB [5], item slot/number at
 * +0x5BC7/+0x5BC8, forest water timer flag at +0x5BE0, event talk XY button at +0x52B0. */
#include "bindings.h"

namespace d_save_cpp {

#include "d_save_local.h"

/* 025B4DC0 */
static void dSv_player_status_a_c_init(u32 t) {
    WWHD_FUNC(0x025B4DC0, void, t);
    st16(t + 2, 12);
    st16(t + 0, 12);
    st8(t + 8, 0);
    for (u32 i = 0; i < 5; i++) {
        st8(t + 9 + i, 0xFF);
        u8 sel = ld8(svbase() + 0x29 + i);
        setSelectItem_inl(i, sel);
    }
    for (u32 i = 0; i < 4; i++) st8(t + 0xE + i, 0xFF);
    st16(t + 4, 0);
    st8(t + 0x12, 0);
    st8(t + 0x13, 0);
    st8(t + 0x14, 0);
    st8(t + 0x15, 0);
    st16(t + 6, 0);
    st8(t + 0x16, 0);
}
VERIFY(0x025B4DC0, dSv_player_status_a_c_init);

/* 025B5028: HD initial time 150.0 (GameCube 165.0) */
static void dSv_player_status_b_c_init(u32 t) {
    WWHD_FUNC(0x025B5028, void, t);
    st(t + 4, 0);
    st16(t + 0x14, 0xFFFF);
    st16(t + 0x12, 0xFFFF);
    st(t + 0, 0);
    stf(t + 0xC, 150.0f);
    st16(t + 0x10, 0);
    stf(t + 8, 0.0f);
}
VERIFY(0x025B5028, dSv_player_status_b_c_init);

/* strncpy(dst, src, 8) (inline) */
static inline void strncpy8_inl(u32 dst, u32 src) {
    u32 i = 0;
    for (; i < 8; i++) {
        u8 c = ld8(src + i);
        st8(dst + i, c);
        if (c == 0) break;
    }
    for (; i < 8; i++) st8(dst + i, 0);
}

/* 025B5060 */
static void dSv_player_return_place_c_init(u32 t) {
    WWHD_FUNC(0x025B5060, void, t);
    strncpy8_inl(t, 0x10053624); /* "sea" */
    st8(t + 8, 0x2C);
    st8(t + 9, 0xCE);
}
VERIFY(0x025B5060, dSv_player_return_place_c_init);

/* 025B50DC */
static void dSv_player_return_place_c_set(u32 t, u32 name, u32 roomNo, u32 status) {
    WWHD_FUNC(0x025B50DC, void, t, name, roomNo, status);
    u32 len = 0;
    while (ld8(name + len) != 0) len++;
    if (len > 7) JUT_ASSERT_l(0x10053628, 0xBF, 0x10053634);
    strncpy8_inl(t, name);
    st8(t + 8, (u8)roomNo);
    st8(t + 9, (u8)status);
}
VERIFY(0x025B50DC, dSv_player_return_place_c_set);

/* 025B51BC */
static void dSv_player_item_c_init(u32 t) {
    WWHD_FUNC(0x025B51BC, void, t);
    for (u32 i = 0; i < 0x15; i++) st8(t + i, 0xFF);
}
VERIFY(0x025B51BC, dSv_player_item_c_init);

/* 025B51DC: HD checks the three X/Y/Z buttons */
static void dSv_player_item_c_setBottleItemIn(u32 t, u32 prevItemNo, u32 newItemNo) {
    WWHD_FUNC(0x025B51DC, void, t, prevItemNo, newItemNo);
    for (u32 i = 0; i < 4; i++) {
        if (ld8(t + 0xE + i) == prevItemNo) {
            st8(t + 0xE + i, (u8)newItemNo);
            for (u32 btn = 0; btn < 3; btn++) {
                u8 sel = ld8(svbase() + 0x29 + btn);
                if (sel == 0xE + i) setSelectItem_inl(btn, sel);
            }
            return;
        }
    }
}
VERIFY(0x025B51DC, dSv_player_item_c_setBottleItemIn);

/* 025B5448 */
static void dSv_player_item_c_setEmptyBottleItemIn(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B5448, void, t, itemNo);
    dSv_player_item_c_setBottleItemIn(t, 0x50, itemNo);
}
VERIFY(0x025B5448, dSv_player_item_c_setEmptyBottleItemIn);

/* 025B5454 */
static void dSv_player_item_c_setEmptyBottle(u32 t) {
    WWHD_FUNC(0x025B5454, void, t);
    u32 b = svbase();
    for (u32 i = 0; i < 4; i++) {
        u32 idx = (u8)(0xE + i);
        if (getItem_inl(b, idx) == 0xFF) {
            setItem_inl(b, idx, 0x50);
            return;
        }
    }
}
VERIFY(0x025B5454, dSv_player_item_c_setEmptyBottle);

/* body shared by both setEquipBottleItemIn (inline) */
static inline void equipBottle_inl(u32 t, u32 btn, u8 invIdx, u32 itemNo) {
    st8(t + invIdx, (u8)itemNo);
    setItem_inl(svbase(), invIdx, (u8)itemNo);
    u32 pl = play();
    st8(pl + PL_ITEMSLOT, invIdx);
    st8(pl + PL_ITEMNO, (u8)itemNo);
    u8 sel = ld8(svbase() + 0x29 + btn);
    setSelectItem_inl(btn, sel);
}

/* 025B5578 */
static void dSv_player_item_c_setEquipBottleItemIn(u32 t, u32 btn, u32 itemNo) {
    WWHD_FUNC(0x025B5578, void, t, btn, itemNo);
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0xE) >= 4) return;
    equipBottle_inl(t, btn, invIdx, itemNo);
}
VERIFY(0x025B5578, dSv_player_item_c_setEquipBottleItemIn);

/* 025B58B8 */
static void dSv_player_item_c_setEquipBottleItemEmpty(u32 t, u32 btn) {
    WWHD_FUNC(0x025B58B8, void, t, btn);
    dSv_player_item_c_setEquipBottleItemIn(t, btn, 0x50);
}
VERIFY(0x025B58B8, dSv_player_item_c_setEquipBottleItemEmpty);

/* 025B58C0 */
static void dSv_player_item_c_setEquipBottleItemIn1(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B58C0, void, t, itemNo);
    u32 btn;
    if (ld8(play() + PL_TALKXY) == 1) btn = 0;
    else if (ld8(play() + PL_TALKXY) == 2) btn = 1;
    else if (ld8(play() + PL_TALKXY) == 3) btn = 2;
    else return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0xE) >= 4) return;
    equipBottle_inl(t, btn, invIdx, itemNo);
}
VERIFY(0x025B58C0, dSv_player_item_c_setEquipBottleItemIn1);

/* 025B5C4C dSv_player_item_c::setEquipBottleItemEmpty() (not named by the matcher) */
static void dSv_player_item_c_setEquipBottleItemEmpty0(u32 t) {
    WWHD_FUNC(0x025B5C4C, void, t);
    dSv_player_item_c_setEquipBottleItemIn1(t, 0x50);
}
VERIFY(0x025B5C4C, dSv_player_item_c_setEquipBottleItemEmpty0);

/* 025B5C54 */
static u8 dSv_player_item_c_checkEmptyBottle(u32 t) {
    WWHD_FUNC(0x025B5C54, u8, t);
    u8 ret = 0;
    for (u32 i = 0; i < 4; i++)
        if (ld8(t + 0xE + i) == 0x50) ret++;
    return ret;
}
VERIFY(0x025B5C54, dSv_player_item_c_checkEmptyBottle);

/* 025B5C80 */
static u8 dSv_player_item_c_checkBottle(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B5C80, u8, t, itemNo);
    u8 ret = 0;
    for (u32 i = 0; i < 4; i++)
        if (ld8(t + 0xE + i) == itemNo) ret++;
    return ret;
}
VERIFY(0x025B5C80, dSv_player_item_c_checkBottle);

/* 025B5CAC */
static void dSv_player_get_item_c_init(u32 t) {
    WWHD_FUNC(0x025B5CAC, void, t);
    for (u32 i = 0; i < 0x15; i++) st8(t + i, 0);
}
VERIFY(0x025B5CAC, dSv_player_get_item_c_init);

/* 025B5CCC */
static void dSv_player_get_item_c_onItem(u32 t, u32 field, u32 item) {
    WWHD_FUNC(0x025B5CCC, void, t, field, item);
    if (item >= 8) JUT_ASSERT_l(0x1005366C, 0x1B4, 0x10053650);
    st8(t + field, (u8)(ld8(t + field) | slw(1, item)));
}
VERIFY(0x025B5CCC, dSv_player_get_item_c_onItem);

/* 025B5D40 */
static BOOL dSv_player_get_item_c_isItem(u32 t, u32 field, u32 item) {
    WWHD_FUNC(0x025B5D40, BOOL, t, field, item);
    if (item >= 8) JUT_ASSERT_l(0x10053694, 0x1D2, 0x10053678);
    return (ld8(t + field) & (u8)slw(1, item)) ? TRUE : FALSE;
}
VERIFY(0x025B5D40, dSv_player_get_item_c_isItem);

/* bottle items 0x4F..0x60: byte idx/7, bit 1 + idx%7 */
/* 025B5DBC */
static void dSv_player_get_item_c_onBottleItem(u32 t, u32 item) {
    WWHD_FUNC(0x025B5DBC, void, t, item);
    u32 idx = item - 0x4F;
    if (idx > 0x11) return;
    u32 byte = idx / 7, bit = 1 + idx % 7;
    st8(t + byte, (u8)(ld8(t + byte) | (1u << bit)));
}
VERIFY(0x025B5DBC, dSv_player_get_item_c_onBottleItem);

/* 025B5F44 */
static BOOL dSv_player_get_item_c_isBottleItem(u32 t, u32 item) {
    WWHD_FUNC(0x025B5F44, BOOL, t, item);
    u32 idx = item - 0x4F;
    if (idx > 0x11) return FALSE;
    u32 byte = idx / 7, bit = 1 + idx % 7;
    return (ld8(t + byte) & (1u << bit)) ? TRUE : FALSE;
}
VERIFY(0x025B5F44, dSv_player_get_item_c_isBottleItem);

/* 025B60D8 dSv_player_item_record2_c::init (probably; HD out-of-line, called by 025B88A8) */
static void dSv_player_item_record2_c_init(u32 t) {
    WWHD_FUNC(0x025B60D8, void, t);
    st8(t + 0, 0);
    st8(t + 1, 0);
    st8(t + 2, 0);
}
VERIFY(0x025B60D8, dSv_player_item_record2_c_init);

/* 025B60EC */
static void dSv_player_item_record_c_init(u32 t) {
    WWHD_FUNC(0x025B60EC, void, t);
    st16(t + 0, 0);
    st8(t + 3, 0);
    st8(t + 2, 0);
    st8(t + 4, 0);
    for (u32 i = 0; i < 3; i++) st8(t + 5 + i, 0);
}
VERIFY(0x025B60EC, dSv_player_item_record_c_init);

/* 025B6120 */
static void dSv_player_item_record_c_resetTimer(u32 t, u32 timer) {
    WWHD_FUNC(0x025B6120, void, t, timer);
    st16(t, (u16)timer);
    st8(play() + PL_FWATER, 0); /* dComIfGs_stopFwaterTimer */
}
VERIFY(0x025B6120, dSv_player_item_record_c_resetTimer);

/* 025B614C */
static void dSv_player_item_record_c_decTimer(u32 t) {
    WWHD_FUNC(0x025B614C, void, t);
    if (ld8(play() + PL_FWATER) != 1) return;
    u16 v = ld16(t);
    if (v != 0) {
        st16(t, (u16)(v - 1));
        return;
    }
    st16(t, 0);
    st8(play() + PL_FWATER, 0);
}
VERIFY(0x025B614C, dSv_player_item_record_c_decTimer);

/* 025B61C8 */
static u16 dSv_player_item_record_c_getTimer(u32 t) {
    WWHD_FUNC(0x025B61C8, u16, t);
    return ld16(t);
}
VERIFY(0x025B61C8, dSv_player_item_record_c_getTimer);

/* 025B61D0 dSv_player_item_max2_c::init (probably; HD out-of-line, called by 025B88A8) */
static void dSv_player_item_max2_c_init(u32 t) {
    WWHD_FUNC(0x025B61D0, void, t);
    st8(t + 0, 0);
    st8(t + 1, 0);
    st8(t + 2, 0);
}
VERIFY(0x025B61D0, dSv_player_item_max2_c_init);

/* 025B61E4 */
static void dSv_player_item_max_c_init(u32 t) {
    WWHD_FUNC(0x025B61E4, void, t);
    st8(t + 1, 0);
    st8(t + 0, 0);
    st8(t + 2, 0);
    for (u32 i = 0; i < 5; i++) st8(t + 3 + i, 0);
}
VERIFY(0x025B61E4, dSv_player_item_max_c_init);

}  // namespace d_save_cpp
