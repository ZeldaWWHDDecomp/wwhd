/* d_save: helpers shared by the parts of the unit (d_save*.cpp). WWHD. See d_save.cpp for the unit's range and the save layout. */
/* Included inside each part's own namespace, after bindings.h. */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 play() { return gabi::ea(dComIfGp_get()); }
/* PowerPC slw: shift counts 32..63 give 0 */
static inline u32 slw(u32 v, u32 n) { n &= 0x3F; return n >= 32 ? 0 : v << n; }

/* the save object (save info = base + 0x20) */
static inline u32 svbase() { return ld(0x101F84DC); }
enum : u32 { PL_SELITEM = 0x5BBB, PL_ITEMSLOT = 0x5BC7, PL_ITEMNO = 0x5BC8, PL_FWATER = 0x5BE0, PL_TALKXY = 0x52B0 };

/* dComIfGs_getItem(idx) (inline): the item, bag beast, bag bait and bag reserve slots */
static inline u8 getItem_inl(u32 b, u32 idx) {
    if ((s32)idx < 0x15) return ld8(b + 0x5C + idx);
    if ((s32)idx < 0x18) return 0xFF;
    if ((s32)idx < 0x20) return ld8(b + 0x7E + idx);
    if ((s32)idx < 0x24) return 0xFF;
    if ((s32)idx < 0x2C) return ld8(b + 0x7A + idx);
    if (idx - 0x30 < 8) return ld8(b + 0x76 + idx);
    return 0xFF;
}
/* dComIfGs_setItem(idx, item) (inline) */
static inline void setItem_inl(u32 b, u32 idx, u8 v) {
    if ((s32)idx < 0x15) { st8(b + 0x5C + idx, v); return; }
    if ((s32)idx < 0x18) return;
    if ((s32)idx < 0x20) { st8(b + 0x7E + idx, v); return; }
    if ((s32)idx < 0x24) return;
    if ((s32)idx < 0x2C) { st8(b + 0x7A + idx, v); return; }
    if (idx - 0x30 < 8) st8(b + 0x76 + idx, v);
}
/* dComIfGp_setSelectItem(btn) (inline); `sel` is the select item read before dComIfGp_get */
static inline void setSelectItem_inl(u32 btn, u8 sel) {
    u32 pl = play();
    if (sel == 0xFF) {
        st8(pl + PL_SELITEM + btn, 0xFF);
        return;
    }
    u32 b = svbase();
    st8(pl + PL_SELITEM + btn, getItem_inl(b, ld8(b + 0x29 + btn)));
    b = svbase();
    if (getItem_inl(b, ld8(b + 0x29 + btn)) == 0xFF) st8(b + 0x29 + btn, 0xFF);
}

/* dComIfGp_event_getTalkXYBtn() mapped to an item button (inline): 1/2/3 -> X/Y/Z; the event
 * flag is reread through dComIfGp_get for each compare. Returns 0xFF for none. */
static inline u32 talkXYBtn_inl() {
    if (ld8(play() + PL_TALKXY) == 1) return 0;
    if (ld8(play() + PL_TALKXY) == 2) return 1;
    if (ld8(play() + PL_TALKXY) == 3) return 2;
    return 0xFF;
}

/* bit flag accessors with the GameCube JUT_ASSERT (each with its own string literals) */
static inline void bitOn8_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    st8(a, (u8)(ld8(a) | slw(1, bit)));
}
static inline void bitOff8_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    st8(a, (u8)(ld8(a) & ~slw(1, bit)));
}
static inline BOOL bitIs8_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    return (ld8(a) & (u8)slw(1, bit)) ? TRUE : FALSE;
}
static inline void bitOn32_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    st(a, ld(a) | slw(1, bit));
}
static inline void bitOff32_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    st(a, ld(a) & ~slw(1, bit));
}
static inline BOOL bitIs32_l(u32 a, u32 bit, u32 lim, u32 file, s32 line, u32 msg) {
    if (bit >= lim) JUT_ASSERT_l(file, line, msg);
    return (ld(a) & slw(1, bit)) ? TRUE : FALSE;
}

