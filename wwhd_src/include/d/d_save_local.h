/* d_save: helpers shared by the parts of the unit (d_save*.cpp). WWHD.  See d_save.cpp for the unit's range and the save layout. */
/* Included inside each part's own namespace, after bindings.h. */

static inline u32 play() { return gabi::ea(dComIfGp_get()); }
/* PowerPC slw: shift counts 32..63 give 0 */
static inline u32 slw(u32 v, u32 n) { n &= 0x3F; return n >= 32 ? 0 : v << n; }

/* the save object (save info = base + dInvSlot_BeastLast_e) */
static inline u32 svbase() { return gabi::ea(saveBase()); }

/* dComIfGs_getItem(idx) (inline): the item, bag beast, bag bait and bag reserve slots */
static inline u8 getInventoryItem(u32 b, u32 idx) {
    if ((s32)idx < dInvSlot_ItemLast_e) return (u8)indexed(savePlayer(b).mPlayerItem.mItems, idx);
    if ((s32)idx < dInvSlot_BeastFirst_e) return dInvSlot_NONE_e;
    if ((s32)idx < dInvSlot_BeastLast_e) return (u8)indexed(savePlayer(b).mBagItem.mBeast, idx - dInvSlot_BeastFirst_e);
    if ((s32)idx < dInvSlot_BaitFirst_e) return dInvSlot_NONE_e;
    if ((s32)idx < dInvSlot_BaitLast_e) return (u8)indexed(savePlayer(b).mBagItem.mBait, idx - dInvSlot_BaitFirst_e);
    if (idx - dInvSlot_ReserveFirst_e < 8) return (u8)indexed(savePlayer(b).mBagItem.mReserve, idx - dInvSlot_ReserveFirst_e);
    return dInvSlot_NONE_e;
}
/* dComIfGs_setItem(idx, item) (inline) */
static inline void setInventoryItem(u32 b, u32 idx, u8 v) {
    if ((s32)idx < dInvSlot_ItemLast_e) { indexed(savePlayer(b).mPlayerItem.mItems, idx) = v; return; }
    if ((s32)idx < dInvSlot_BeastFirst_e) return;
    if ((s32)idx < dInvSlot_BeastLast_e) { indexed(savePlayer(b).mBagItem.mBeast, idx - dInvSlot_BeastFirst_e) = v; return; }
    if ((s32)idx < dInvSlot_BaitFirst_e) return;
    if ((s32)idx < dInvSlot_BaitLast_e) { indexed(savePlayer(b).mBagItem.mBait, idx - dInvSlot_BaitFirst_e) = v; return; }
    if (idx - dInvSlot_ReserveFirst_e < 8) indexed(savePlayer(b).mBagItem.mReserve, idx - dInvSlot_ReserveFirst_e) = v;
}
/* dComIfGp_setSelectItem(btn) (inline); `sel` is the select item read before dComIfGp_get */
static inline void refreshSelectedItem(u32 btn, u8 sel) {
    u32 pl = play();
    if (sel == dInvSlot_NONE_e) {
        indexed(playInfo(pl)->play.mSelectItem, btn) = dInvSlot_NONE_e;
        return;
    }
    u32 b = svbase();
    indexed(playInfo(pl)->play.mSelectItem, btn) = getInventoryItem(b, (u8)indexed(savePlayer(b).mPlayerStatusA.mSelectItem, btn));
    b = svbase();
    if (getInventoryItem(b, (u8)indexed(savePlayer(b).mPlayerStatusA.mSelectItem, btn)) == dInvSlot_NONE_e) indexed(savePlayer(b).mPlayerStatusA.mSelectItem, btn) = dInvSlot_NONE_e;
}

/* dComIfGp_event_getTalkXYBtn() mapped to an item button (inline): 1/2/3 -> X/Y/Z; the event
 * flag is reread through dComIfGp_get for each compare. Returns dInvSlot_NONE_e for none. */
static inline u32 talkItemButton() {
    if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 1) return 0;
    if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 2) return 1;
    if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 3) return 2;
    return dInvSlot_NONE_e;
}

/* bit flag accessors with the GameCube JUT_ASSERT (each with its own string literals) */
static inline void setByteBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    *view<be<u8>>(a) = (u8)((u8)*view<be<u8>>(a) | slw(1, bit));
}
static inline void clearByteBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    *view<be<u8>>(a) = (u8)((u8)*view<be<u8>>(a) & ~slw(1, bit));
}
static inline BOOL testByteBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    return ((u8)*view<be<u8>>(a) & (u8)slw(1, bit)) ? TRUE : FALSE;
}
static inline void setScalarWordBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    *view<be<u32>>(a) = (u32)*view<be<u32>>(a) | slw(1, bit);
}
static inline void clearScalarWordBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    *view<be<u32>>(a) = (u32)*view<be<u32>>(a) & ~slw(1, bit);
}
static inline BOOL testScalarWordBit(u32 a, u32 bit, u32 lim, const char* file, s32 line, const char* msg) {
    if (bit >= lim) saveAssert(file, line, msg);
    return ((u32)*view<be<u32>>(a) & slw(1, bit)) ? TRUE : FALSE;
}

