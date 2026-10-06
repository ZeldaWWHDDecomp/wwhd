/* d_item: helpers shared by the parts of the unit (d_item*.cpp). WWHD. See d_item.cpp for the unit's range and layout notes. */
/* Included inside each part's own namespace, after bindings.h. */

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

/* the play object (dComIfGp_get, 025200D4) and the save object (*101F84DC; save info = +0x20) */
static inline u32 play() { return gabi::ea(dComIfGp_get()); }
static inline u32 svbase() { return ld(0x101F84DC); }

/* play-object item counters (dComIfGp_setItem*Count adds to them) */
enum : u32 {
    PL_LIFE = 0x5B44,      /* f32 mItemLifeCount */
    PL_RUPEE = 0x5B48,     /* s32 mItemRupeeCount */
    PL_KEY = 0x5B5C,       /* s16 mItemKeyNumCount */
    PL_MAXLIFE = 0x5B5E,   /* s16 mItemMaxLifeCount */
    PL_MAGIC = 0x5B60,     /* s16 mItemMagicCount */
    PL_MAXMAGIC = 0x5B64,  /* s16 mItemMaxMagicCount */
    PL_ARROW = 0x5B68,     /* s16 mItemArrowNumCount */
    PL_BOMB = 0x5B6C,      /* s16 mItemBombNumCount */
    PL_BEAST = 0x5B70,     /* s16 mItemBeastNumCounts[8] */
    PL_SELITEM = 0x5BBB,   /* u8 select item [5] (HD: five item buttons) */
    PL_ITEMSLOT = 0x5BC7,  /* u8 dComIfGp_setItem slot */
    PL_ITEMNO = 0x5BC8,    /* u8 dComIfGp_setItem item */
    PL_STAGE = 0x5150,     /* dStage_stageDt_c (vtable first) */
};

/* save-object offsets (base = *101F84DC; GameCube info offset + 0x20) */
enum : u32 {
    SV_MAXLIFE = 0x20,     /* u16 status_a.mMaxLife */
    SV_LIFE = 0x22,        /* u16 status_a.mLife */
    SV_SELITEM = 0x29,     /* u8 status_a.mSelectItem[5] */
    SV_WALLET = 0x32,      /* u8 status_a.mWalletSize */
    SV_MAXMAGIC = 0x33,    /* u8 status_a.mMaxMagic */
    SV_ITEM = 0x5C,        /* dSv_player_item_c (21 slots) */
    SV_GETITEM = 0x71,     /* dSv_player_get_item_c */
    SV_BAGITEM = 0x96,     /* dSv_player_bag_item_c */
    SV_GETBAG = 0xB0,      /* dSv_player_get_bag_item_c */
    SV_ARROWNUM = 0x89,    /* u8 item_record.mArrowNum */
    SV_BOMBNUM = 0x8A,     /* u8 item_record.mBombNum */
    SV_ARROWMAX = 0x8F,    /* u8 item_max.mArrowNum */
    SV_BOMBMAX = 0x90,     /* u8 item_max.mBombNum */
    SV_COLLECT = 0xD4,     /* dSv_player_collect_c */
    SV_MAP = 0xE4,         /* dSv_player_map_c */
    SV_EVENT = 0x644,      /* dSv_event_c */
    SV_MEMBIT = 0x798,     /* dSv_memBit_c of the current stage (info.mMemory) */
    SV_HDSTATS = 0x12C0,   /* HD-only block; 027201DC returns its counters at +0x64C */
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void dSv_get_item_onItem_l(u32 t, s32 slot, u32 bit) { gabi::call(0x025B5CCC, t, slot, bit); }
static inline s32 dSv_get_item_isItem_l(u32 t, s32 slot, u32 bit) { return gabi::call<s32>(0x025B5D40, t, slot, bit); }
static inline void dSv_get_item_onBottleItem_l(u32 t, u32 item) { gabi::call(0x025B5DBC, t, item); }
static inline s32 dSv_get_item_isBottleItem_l(u32 t, u32 item) { return gabi::call<s32>(0x025B5F44, t, item); }
static inline void dSv_item_setEmptyBottleItemIn_l(u32 t, u32 item) { gabi::call(0x025B5448, t, item); }
static inline void dSv_item_setEmptyBottle_l(u32 t) { gabi::call(0x025B5454, t); }
static inline void dSv_bag_setBeastItem_l(u32 t, u32 item) { gabi::call(0x025B62A4, t, item); }
static inline void dSv_bag_setBaitItem_l(u32 t, u32 item) { gabi::call(0x025B6E98, t, item); }
static inline s32 dSv_bag_checkReserveItem_l(u32 t, u32 item) { return gabi::call<s32>(0x025B7570, t, item); }
static inline void dSv_bag_setReserveItem_l(u32 t, u32 item) { gabi::call(0x025B75AC, t, item); }
static inline void dSv_getbag_onBeast_l(u32 t, u32 i) { gabi::call(0x025B7628, t, i); }
static inline s32 dSv_getbag_isBeast_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7690, t, i); }
static inline void dSv_getbag_onBait_l(u32 t, u32 i) { gabi::call(0x025B7700, t, i); }
static inline s32 dSv_getbag_isBait_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7768, t, i); }
static inline void dSv_getbag_onReserve_l(u32 t, u32 i) { gabi::call(0x025B77D8, t, i); }
static inline s32 dSv_getbag_isReserve_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7840, t, i); }
static inline void dSv_collect_onCollect_l(u32 t, s32 i, u32 bit) { gabi::call(0x025B7944, t, i, bit); }
static inline s32 dSv_collect_isCollect_l(u32 t, s32 i, u32 bit) { return gabi::call<s32>(0x025B7A2C, t, i, bit); }
static inline void dSv_collect_onTact_l(u32 t, u32 i) { gabi::call(0x025B7AA8, t, i); }
static inline s32 dSv_collect_isTact_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7B10, t, i); }
static inline void dSv_collect_onTriforce_l(u32 t, u32 i) { gabi::call(0x025B7B80, t, i); }
static inline s32 dSv_collect_isTriforce_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7C50, t, i); }
static inline void dSv_collect_onSymbol_l(u32 t, u32 i) { gabi::call(0x025B7CC0, t, i); }
static inline s32 dSv_collect_isSymbol_l(u32 t, u32 i) { return gabi::call<s32>(0x025B7D90, t, i); }
static inline void dSv_map_onGetMap_l(u32 t, s32 i) { gabi::call(0x025B7EF4, t, i); }
static inline s32 dSv_map_isGetMap_l(u32 t, s32 i) { return gabi::call<s32>(0x025B7F68, t, i); }
static inline void dSv_map_offOpenMap_l(u32 t, s32 i) { gabi::call(0x025B8054, t, i); }
static inline void dSv_map_offCompleteMap_l(u32 t, s32 i) { gabi::call(0x025B81B4, t, i); }
static inline void dSv_event_onEventBit_l(u32 t, u32 flag) { gabi::call(0x025B8B68, t, flag); }
static inline s32 dSv_event_isEventBit_l(u32 t, u32 flag) { return gabi::call<s32>(0x025B8B94, t, flag); }
static inline void dSv_memBit_onDungeonItem_l(u32 t, s32 i) { gabi::call(0x025B9098, t, i); }
static inline s32 dSv_memBit_isDungeonItem_l(u32 t, s32 i) { return gabi::call<s32>(0x025B9100, t, i); }
static inline void dComIfGs_onStageLife_l(s32 stageNo) { gabi::call(0x02520B08, stageNo); }
static inline void dComIfGs_setSelectEquip_l(s32 i, u32 item) { gabi::call(0x02522398, i, item); }
/* HD statistics: 027201DC returns this+0x64C; 02726DCC / 02726E0C add to the counters at +0x14 / +0x1C */
static inline u32 dSv_hdstats_get_l(u32 t) { return gabi::call<u32>(0x027201DC, t); }
static inline void dSv_hdstats_add14_l(u32 s, s32 n) { gabi::call(0x02726DCC, s, n); }
static inline void dSv_hdstats_add1C_l(u32 s, s32 n) { gabi::call(0x02726E0C, s, n); }
static inline u8 cDT_GetInf_l(u32 t, u32 col, u32 row) { return gabi::call<u8>(0x0200EA74, t, col, row); }

/* ---- inline dComIfG* accessors (the save pointer is re-read at each use, as GHS does) ---- */
static inline void dComIfGs_onGetItem(s32 slot, u32 bit) { dSv_get_item_onItem_l(svbase() + SV_GETITEM, slot, bit); }
static inline s32 dComIfGs_isGetItem(s32 slot, u32 bit) { return dSv_get_item_isItem_l(svbase() + SV_GETITEM, slot, bit); }
/* dComIfGs_setItem with a constant inventory slot below 0x15 (the range checks fold) */
static inline void dComIfGs_setItem(u32 slot, u8 item) { st8(svbase() + SV_ITEM + slot, item); }
static inline void dComIfGs_onCollect(s32 i, u32 bit) { dSv_collect_onCollect_l(svbase() + SV_COLLECT, i, bit); }
static inline s32 dComIfGs_isCollect(s32 i, u32 bit) { return dSv_collect_isCollect_l(svbase() + SV_COLLECT, i, bit); }

/* counters: lha/lwz, add, sth/stw on the play object fetched right before */
static inline void plAdd16(u32 off, s32 n) { u32 p = play(); st16(p + off, (u16)(ld16(p + off) + n)); }
static inline void plAdd32(u32 off, s32 n) { u32 p = play(); st(p + off, ld(p + off) + (u32)n); }
static inline void dComIfGp_setItemLifeCount(f32 n) { u32 p = play(); stf(p + PL_LIFE, gabi::fadds_ppc(ldf(p + PL_LIFE), n)); }
static inline void dComIfGp_setItemRupeeCount(s32 n) { plAdd32(PL_RUPEE, n); }
static inline void dComIfGp_setItemMagicCount(s16 n) { plAdd16(PL_MAGIC, n); }
static inline void dComIfGp_setItemMaxMagicCount(s16 n) { plAdd16(PL_MAXMAGIC, n); }
static inline void dComIfGp_setItemArrowNumCount(s16 n) { plAdd16(PL_ARROW, n); }
static inline void dComIfGp_setItemBombNumCount(s16 n) { plAdd16(PL_BOMB, n); }
static inline void dComIfGp_setItemKeyNumCount(s16 n) { plAdd16(PL_KEY, n); }
static inline void dComIfGp_setItemMaxLifeCount(s16 n) { plAdd16(PL_MAXLIFE, n); }
static inline void dComIfGp_setItemBeastNumCount(u32 i, s16 n) { plAdd16(PL_BEAST + 2 * i, n); }

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
/* dComIfGp_setSelectItem(btn) (inline; same body as in d_save); `sel` is the select item read
 * before dComIfGp_get */
static inline void setSelectItem_inl(u32 btn, u8 sel) {
    u32 pl = play();
    if (sel == 0xFF) {
        st8(pl + PL_SELITEM + btn, 0xFF);
        return;
    }
    u32 b = svbase();
    st8(pl + PL_SELITEM + btn, getItem_inl(b, ld8(b + SV_SELITEM + btn)));
    b = svbase();
    if (getItem_inl(b, ld8(b + SV_SELITEM + btn)) == 0xFF) st8(b + SV_SELITEM + btn, 0xFF);
}
static inline void dComIfGp_setSelectItem(u32 btn) {
    u8 sel = ld8(svbase() + SV_SELITEM + btn);
    setSelectItem_inl(btn, sel);
}
/* the three X/Y/Z buttons: refresh the buttons that show `oldItem` */
static inline void refreshSelectItem_inl(u8 oldItem) {
    for (u32 btn = 0; btn < 3; btn++) {
        if (ld8(play() + PL_SELITEM + btn) == oldItem) dComIfGp_setSelectItem(btn);
    }
}
