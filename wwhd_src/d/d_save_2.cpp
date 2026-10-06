/* d_save part 2: dSv_player_bag_item_c, dSv_player_get_bag_item_c, dSv_player_bag_item_record_c,
 * dSv_player_collect_c (025B6214..025B7E73). WWHD. 
 * See d_save.cpp for the unit's range and the save layout.
 *
 * dSv_player_bag_item_c (info+0x56): beast [8] 0, bait [8] 8, reserve [8] 0x10. Bait counts
 * (dSv_player_bag_item_record_c bait part) at save base + 0xC4 (info+0xA4).
 * dSv_player_get_bag_item_c: reserve flags (u32) 0, beast flags 4, bait flags 5.
 * dSv_player_collect_c: collect [8] 0, 8, tact 9, triforce 0xA, symbol 0xB, 0xC. */
#include "bindings.h"

namespace d_save_2_cpp {
#include "d_save_local.h"

/* change of a bag item on an item button (inline body of setBaitItemChange /
 * setReserveItemChange): the inventory slot, the play item slot, then the select item */
static inline void bagChange_inl(u32 btn, u8 invIdx, u32 itemNo) {
    setItem_inl(svbase(), invIdx, (u8)itemNo);
    u32 pl = play();
    st8(pl + PL_ITEMNO, (u8)itemNo);
    st8(pl + PL_ITEMSLOT, invIdx);
    u8 sel = ld8(svbase() + 0x29 + btn);
    setSelectItem_inl(btn, sel);
}

/* 025B6214 */
static void dSv_player_bag_item_c_init(u32 t) {
    WWHD_FUNC(0x025B6214, void, t);
    for (u32 i = 0; i < 8; i++) st8(t + i, 0xFF);
    for (u32 i = 0; i < 8; i++) st8(t + 8 + i, 0xFF);
    for (u32 i = 0; i < 8; i++) st8(t + 0x10 + i, 0xFF);
}
VERIFY(0x025B6214, dSv_player_bag_item_c_init);

/* 025B6274 */
static bool dSv_player_bag_item_c_checkBeastItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6274, bool, t, itemNo);
    for (u32 i = 0; i < 8; i++)
        if (ld8(t + i) == itemNo) return true;
    return false;
}
VERIFY(0x025B6274, dSv_player_bag_item_c_checkBeastItem);

/* 025B62A4 */
static void dSv_player_bag_item_c_setBeastItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B62A4, void, t, itemNo);
    if (!dSv_player_bag_item_c_checkBeastItem(t, itemNo)) {
        for (u32 i = 0; i < 8; i++) {
            if (ld8(t + i) == 0xFF) {
                st8(t + i, (u8)itemNo);
                return;
            }
        }
    }
}
VERIFY(0x025B62A4, dSv_player_bag_item_c_setBeastItem);

/* 025B6308 */
static void dSv_player_bag_item_c_setBeastItemEmpty(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6308, void, t, itemNo);
    if (!dSv_player_bag_item_c_checkBeastItem(t, itemNo)) return;
    for (u32 i = 0; i < 8; i++) {
        if (ld8(t + i) == itemNo) {
            st8(t + i, 0xFF);
            for (u32 btn = 0; btn < 3; btn++) {
                if (ld8(play() + PL_SELITEM + btn) == itemNo) {
                    u32 b = svbase();
                    u8 invIdx = ld8(b + 0x29 + btn);
                    setItem_inl(b, invIdx, 0xFF);
                    u32 pl = play();
                    st8(pl + PL_ITEMSLOT, invIdx);
                    st8(pl + PL_ITEMNO, 0xFF);
                    u8 sel = ld8(svbase() + 0x29 + btn);
                    setSelectItem_inl(btn, sel);
                }
            }
            return;
        }
    }
}
VERIFY(0x025B6308, dSv_player_bag_item_c_setBeastItemEmpty);

/* 025B6638 */
static void dSv_player_bag_item_c_setBaitItemChange(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6638, void, t, itemNo);
    u32 btn = talkXYBtn_inl();
    if (btn == 0xFF) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    bagChange_inl(btn, invIdx, itemNo);
}
VERIFY(0x025B6638, dSv_player_bag_item_c_setBaitItemChange);

/* 025B6AB0 dSv_player_bag_item_c::setBaitItemChange(u8 btn, u8 itemNo) (not named by the matcher) */
static void dSv_player_bag_item_c_setBaitItemChange2(u32 t, u32 btn, u32 itemNo) {
    WWHD_FUNC(0x025B6AB0, void, t, btn, itemNo);
    if (btn > 2) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    bagChange_inl(btn, invIdx, itemNo);
}
VERIFY(0x025B6AB0, dSv_player_bag_item_c_setBaitItemChange2);

/* bait use (inline body of both setBaitItemEmpty): HD rereads the play select item for each
 * compare; Hyoi pear (0x83) empties the slot, bird bait (0x82) counts down */
static inline bool baitEmpty_inl(u32 btn, u8 invIdx) {
    u32 slot = (u32)invIdx - 0x24;
    if (ld8(play() + PL_SELITEM + btn) == 0x83) return true;
    if (ld8(play() + PL_SELITEM + btn) != 0x82) return false;
    u32 b = svbase();
    u8 num = ld8(b + 0xC4 + slot);
    if (num != 0) num = (u8)(num - 1);
    st8(b + 0xC4 + slot, num);
    return num == 0;
}

/* 025B6994 */
static void dSv_player_bag_item_c_setBaitItemEmpty(u32 t) {
    WWHD_FUNC(0x025B6994, void, t);
    u32 btn = talkXYBtn_inl();
    if (btn == 0xFF) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    if (baitEmpty_inl(btn, invIdx)) dSv_player_bag_item_c_setBaitItemChange(t, 0xFF);
}
VERIFY(0x025B6994, dSv_player_bag_item_c_setBaitItemEmpty);

/* 025B6DA0 */
static void dSv_player_bag_item_c_setBaitItemEmpty1(u32 t, u32 btn) {
    WWHD_FUNC(0x025B6DA0, void, t, btn);
    if (btn > 2) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    if (baitEmpty_inl(btn, invIdx)) dSv_player_bag_item_c_setBaitItemChange2(t, btn, 0xFF);
}
VERIFY(0x025B6DA0, dSv_player_bag_item_c_setBaitItemEmpty1);

/* 025B6E5C */
static u8 dSv_player_bag_item_c_checkBaitItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6E5C, u8, t, itemNo);
    u8 ret = 0;
    for (u32 i = 0; i < 8; i++)
        if (ld8(t + 8 + i) == itemNo) ret++;
    return ret;
}
VERIFY(0x025B6E5C, dSv_player_bag_item_c_checkBaitItem);

/* 025B6E90 */
static u8 dSv_player_bag_item_c_checkBaitItemEmpty(u32 t) {
    WWHD_FUNC(0x025B6E90, u8, t);
    return dSv_player_bag_item_c_checkBaitItem(t, 0xFF);
}
VERIFY(0x025B6E90, dSv_player_bag_item_c_checkBaitItemEmpty);

/* 025B6E98 */
static void dSv_player_bag_item_c_setBaitItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6E98, void, t, itemNo);
    if (dSv_player_bag_item_c_checkBaitItemEmpty(t) != 0) {
        for (u32 i = 0; i < 8; i++) {
            if (ld8(t + 8 + i) == 0xFF) {
                st8(t + 8 + i, (u8)itemNo);
                st8(svbase() + 0xC4 + i, 3); /* dComIfGs_setBaitNum(i, 3) */
                return;
            }
        }
    }
}
VERIFY(0x025B6E98, dSv_player_bag_item_c_setBaitItem);

/* 025B6F14 */
static void dSv_player_bag_item_c_setReserveItemChange(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B6F14, void, t, itemNo);
    u32 btn = talkXYBtn_inl();
    if (btn == 0xFF) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x30) >= 8) return;
    bagChange_inl(btn, invIdx, itemNo);
}
VERIFY(0x025B6F14, dSv_player_bag_item_c_setReserveItemChange);

/* 025B7270 */
static void dSv_player_bag_item_c_setReserveItemEmpty(u32 t) {
    WWHD_FUNC(0x025B7270, void, t);
    dSv_player_bag_item_c_setReserveItemChange(t, 0xFF);
}
VERIFY(0x025B7270, dSv_player_bag_item_c_setReserveItemEmpty);

/* 025B7278 */
static void dSv_player_bag_item_c_setReserveItemChange2(u32 t, u32 btn, u32 itemNo) {
    WWHD_FUNC(0x025B7278, void, t, btn, itemNo);
    if (btn > 2) return;
    u8 invIdx = ld8(svbase() + 0x29 + btn);
    if ((u32)(invIdx - 0x30) >= 8) return;
    bagChange_inl(btn, invIdx, itemNo);
}
VERIFY(0x025B7278, dSv_player_bag_item_c_setReserveItemChange2);

/* 025B7568 dSv_player_bag_item_c::setReserveItemEmpty(u8 btn) (not named by the matcher) */
static void dSv_player_bag_item_c_setReserveItemEmpty1(u32 t, u32 btn) {
    WWHD_FUNC(0x025B7568, void, t, btn);
    dSv_player_bag_item_c_setReserveItemChange2(t, btn, 0xFF);
}
VERIFY(0x025B7568, dSv_player_bag_item_c_setReserveItemEmpty1);

/* 025B7570 */
static u8 dSv_player_bag_item_c_checkReserveItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B7570, u8, t, itemNo);
    u8 ret = 0;
    for (u32 i = 0; i < 8; i++)
        if (ld8(t + 0x10 + i) == itemNo) ret++;
    return ret;
}
VERIFY(0x025B7570, dSv_player_bag_item_c_checkReserveItem);

/* 025B75A4 */
static u8 dSv_player_bag_item_c_checkReserveItemEmpty(u32 t) {
    WWHD_FUNC(0x025B75A4, u8, t);
    return dSv_player_bag_item_c_checkReserveItem(t, 0xFF);
}
VERIFY(0x025B75A4, dSv_player_bag_item_c_checkReserveItemEmpty);

/* 025B75AC */
static void dSv_player_bag_item_c_setReserveItem(u32 t, u32 itemNo) {
    WWHD_FUNC(0x025B75AC, void, t, itemNo);
    if (dSv_player_bag_item_c_checkReserveItemEmpty(t) != 0) {
        for (u32 i = 0; i < 8; i++) {
            if (ld8(t + 0x10 + i) == 0xFF) {
                st8(t + 0x10 + i, (u8)itemNo);
                return;
            }
        }
    }
}
VERIFY(0x025B75AC, dSv_player_bag_item_c_setReserveItem);

/* 025B7614 */
static void dSv_player_get_bag_item_c_init(u32 t) {
    WWHD_FUNC(0x025B7614, void, t);
    st8(t + 4, 0);
    st8(t + 5, 0);
    st(t + 0, 0);
}
VERIFY(0x025B7614, dSv_player_get_bag_item_c_init);

/* 025B7628 */
static void dSv_player_get_bag_item_c_onBeast(u32 t, u32 no) {
    WWHD_FUNC(0x025B7628, void, t, no);
    bitOn8_l(t + 4, no, 8, 0x100536B8, 0x4F8, 0x100536A0);
}
VERIFY(0x025B7628, dSv_player_get_bag_item_c_onBeast);

/* 025B7690 */
static BOOL dSv_player_get_bag_item_c_isBeast(u32 t, u32 no) {
    WWHD_FUNC(0x025B7690, BOOL, t, no);
    return bitIs8_l(t + 4, no, 8, 0x100536E0, 0x516, 0x100536C8);
}
VERIFY(0x025B7690, dSv_player_get_bag_item_c_isBeast);

/* 025B7700 */
static void dSv_player_get_bag_item_c_onBait(u32 t, u32 no) {
    WWHD_FUNC(0x025B7700, void, t, no);
    bitOn8_l(t + 5, no, 8, 0x10053704, 0x525, 0x100536EC);
}
VERIFY(0x025B7700, dSv_player_get_bag_item_c_onBait);

/* 025B7768 */
static BOOL dSv_player_get_bag_item_c_isBait(u32 t, u32 no) {
    WWHD_FUNC(0x025B7768, BOOL, t, no);
    return bitIs8_l(t + 5, no, 8, 0x1005372C, 0x543, 0x10053714);
}
VERIFY(0x025B7768, dSv_player_get_bag_item_c_isBait);

/* 025B77D8 */
static void dSv_player_get_bag_item_c_onReserve(u32 t, u32 no) {
    WWHD_FUNC(0x025B77D8, void, t, no);
    bitOn32_l(t, no, 0x20, 0x10053738, 0x552, 0x10053744);
}
VERIFY(0x025B77D8, dSv_player_get_bag_item_c_onReserve);

/* 025B7840 */
static BOOL dSv_player_get_bag_item_c_isReserve(u32 t, u32 no) {
    WWHD_FUNC(0x025B7840, BOOL, t, no);
    return bitIs32_l(t, no, 0x20, 0x10053760, 0x570, 0x1005376C);
}
VERIFY(0x025B7840, dSv_player_get_bag_item_c_isReserve);

/* 025B78AC */
static void dSv_player_bag_item_record_c_init(u32 t) {
    WWHD_FUNC(0x025B78AC, void, t);
    for (u32 i = 0; i < 8; i++) st8(t + i, 0);
    for (u32 i = 0; i < 8; i++) st8(t + 8 + i, 0);
    for (u32 i = 0; i < 8; i++) st8(t + 0x10 + i, 0);
}
VERIFY(0x025B78AC, dSv_player_bag_item_record_c_init);

/* 025B790C */
static void dSv_player_collect_c_init(u32 t) {
    WWHD_FUNC(0x025B790C, void, t);
    for (u32 i = 0; i < 8; i++) st8(t + i, 0);
    st8(t + 0xC, 0);
    st8(t + 0xB, 0);
    st8(t + 8, 0);
    st8(t + 0xA, 0);
    st8(t + 9, 0);
}
VERIFY(0x025B790C, dSv_player_collect_c_init);

/* 025B7944 */
static void dSv_player_collect_c_onCollect(u32 t, u32 field, u32 no) {
    WWHD_FUNC(0x025B7944, void, t, field, no);
    bitOn8_l(t + field, no, 8, 0x100537A0, 0x5A9, 0x10053784);
}
VERIFY(0x025B7944, dSv_player_collect_c_onCollect);

/* 025B79B8 */
static void dSv_player_collect_c_offCollect(u32 t, u32 field, u32 no) {
    WWHD_FUNC(0x025B79B8, void, t, field, no);
    bitOff8_l(t + field, no, 8, 0x100537C8, 0x5B8, 0x100537AC);
}
VERIFY(0x025B79B8, dSv_player_collect_c_offCollect);

/* 025B7A2C */
static BOOL dSv_player_collect_c_isCollect(u32 t, u32 field, u32 no) {
    WWHD_FUNC(0x025B7A2C, BOOL, t, field, no);
    return bitIs8_l(t + field, no, 8, 0x100537F0, 0x5C7, 0x100537D4);
}
VERIFY(0x025B7A2C, dSv_player_collect_c_isCollect);

/* 025B7AA8 */
static void dSv_player_collect_c_onTact(u32 t, u32 no) {
    WWHD_FUNC(0x025B7AA8, void, t, no);
    bitOn8_l(t + 9, no, 8, 0x10053820, 0x603, 0x10053808);
}
VERIFY(0x025B7AA8, dSv_player_collect_c_onTact);

/* 025B7B10 */
static BOOL dSv_player_collect_c_isTact(u32 t, u32 no) {
    WWHD_FUNC(0x025B7B10, BOOL, t, no);
    return bitIs8_l(t + 9, no, 8, 0x10053848, 0x621, 0x10053830);
}
VERIFY(0x025B7B10, dSv_player_collect_c_isTact);

/* 025B7B80 */
static void dSv_player_collect_c_onTriforce(u32 t, u32 no) {
    WWHD_FUNC(0x025B7B80, void, t, no);
    bitOn8_l(t + 0xA, no, 8, 0x1005386C, 0x630, 0x10053854);
}
VERIFY(0x025B7B80, dSv_player_collect_c_onTriforce);

/* 025B7BE8 dSv_player_collect_c::offTriforce (not named by the matcher) */
static void dSv_player_collect_c_offTriforce(u32 t, u32 no) {
    WWHD_FUNC(0x025B7BE8, void, t, no);
    bitOff8_l(t + 0xA, no, 8, 0x10053890, 0x63F, 0x10053878);
}
VERIFY(0x025B7BE8, dSv_player_collect_c_offTriforce);

/* 025B7C50 */
static BOOL dSv_player_collect_c_isTriforce(u32 t, u32 no) {
    WWHD_FUNC(0x025B7C50, BOOL, t, no);
    return bitIs8_l(t + 0xA, no, 8, 0x100538B4, 0x64E, 0x1005389C);
}
VERIFY(0x025B7C50, dSv_player_collect_c_isTriforce);

/* 025B7CC0 */
static void dSv_player_collect_c_onSymbol(u32 t, u32 no) {
    WWHD_FUNC(0x025B7CC0, void, t, no);
    bitOn8_l(t + 0xB, no, 8, 0x100538D8, 0x65D, 0x100538C0);
}
VERIFY(0x025B7CC0, dSv_player_collect_c_onSymbol);

/* 025B7D28 dSv_player_collect_c::offSymbol (not named by the matcher) */
static void dSv_player_collect_c_offSymbol(u32 t, u32 no) {
    WWHD_FUNC(0x025B7D28, void, t, no);
    bitOff8_l(t + 0xB, no, 8, 0x100538FC, 0x66C, 0x100538E4);
}
VERIFY(0x025B7D28, dSv_player_collect_c_offSymbol);

/* 025B7D90 */
static BOOL dSv_player_collect_c_isSymbol(u32 t, u32 no) {
    WWHD_FUNC(0x025B7D90, BOOL, t, no);
    return bitIs8_l(t + 0xB, no, 8, 0x10053920, 0x67B, 0x10053908);
}
VERIFY(0x025B7D90, dSv_player_collect_c_isSymbol);

/* 025B7E00 */
static s32 dSv_player_collect_c_getTriforceNum(u32 t) {
    WWHD_FUNC(0x025B7E00, s32, t);
    s32 n = 0;
    for (u32 i = 0; i < 8; i++)
        if (dSv_player_collect_c_isTriforce(t, (u8)i)) n++;
    return n;
}
VERIFY(0x025B7E00, dSv_player_collect_c_getTriforceNum);

}  // namespace d_save_2_cpp
