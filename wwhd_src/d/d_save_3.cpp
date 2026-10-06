/* d_save part 3: dSv_player_map_c, dSv_player_info_c, dSv_player_config_c, dSv_player_priest_c,
 * dSv_player_c, dSv_ocean_c, dSv_event_c (025B7E74..025B8BC3). WWHD. See d_save.cpp for the unit's range and the save layout.
 *
 * dSv_player_c (HD offsets): status a 0, status b 0x18, return place 0x30, item 0x3C, get item
 * 0x51, item record 0x66, item max 0x6E, bag item 0x76, get bag item 0x90, bag item record 0x9C,
 * collect 0xB4, map 0xC4, info 0x148, config 0x1A4.
 * dSv_player_map_c: four u32[4] bit arrays at 0 / 0x10 (got) / 0x20 (opened) / 0x30 (completed),
 * fmap bits [0x31] at 0x40, 0x71 [16], triforce charts 0x81 (HD initial 0xEA, GameCube 0).
 * dSv_player_info_c: name 0x14 [17], 0x10, 0x12, names 0x25 / 0x36 [17], puzzle 0x47 [17],
 * 0x58, random salvage point 0x59. dSv_player_config_c: ruby 0, sound mode 1 (HD 2), 2, 3. */
#include "bindings.h"

namespace d_save_3_cpp {
#include "d_save_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline f32 cM_rndF_l(f32 max) { return gabi::call<f32>(0x020198D8, max); }
/* 025E1C74 probably mDoAud_setOutputMode (HD: called with 2) */
static inline void mDoAud_setOutputMode_l(u32 mode) { gabi::call(0x025E1C74, mode); }

/* the map bit arrays: word (no >> 5), bit (no & 31) */
static inline void mapOn_l(u32 a, u32 no, u32 file, s32 line, u32 msg) {
    if (no >= 0x80) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    st(w, ld(w) | (1u << (no & 31)));
}
static inline void mapOff_l(u32 a, u32 no, u32 file, s32 line, u32 msg) {
    if (no >= 0x80) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    st(w, ld(w) & ~(1u << (no & 31)));
}
static inline BOOL mapIs_l(u32 a, u32 no, u32 file, s32 line, u32 msg) {
    if (no >= 0x80) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    return (ld(w) & (1u << (no & 31))) ? TRUE : FALSE;
}

/* 025B7E74: HD marks triforce charts 1, 3, 5, 6, 7 (0xEA) from the start */
static void dSv_player_map_c_init(u32 t) {
    WWHD_FUNC(0x025B7E74, void, t);
    for (u32 i = 0; i < 4; i++) {
        st(t + 4 * i, 0);
        st(t + 0x10 + 4 * i, 0);
        st(t + 0x20 + 4 * i, 0);
        st(t + 0x30 + 4 * i, 0);
    }
    for (u32 i = 0; i < 0x31; i++) st8(t + 0x40 + i, 0);
    st8(t + 0x81, 0xEA);
    st8(t + 0x40 + 0xA, 3);
    st8(t + 0x40 + 0, 3);
    st8(t + 0x40 + 0x2B, 3);
    for (u32 i = 0; i < 0x10; i++) st8(t + 0x71 + i, 0);
}
VERIFY(0x025B7E74, dSv_player_map_c_init);

/* 025B7EF4 dSv_player_map_c::onGetMap (not named by the matcher) */
static void dSv_player_map_c_onGetMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B7EF4, void, t, no);
    mapOn_l(t + 0x10, no, 0x10053944, 0x6C9, 0x1005392C);
}
VERIFY(0x025B7EF4, dSv_player_map_c_onGetMap);

/* 025B7F68 */
static BOOL dSv_player_map_c_isGetMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B7F68, BOOL, t, no);
    return mapIs_l(t + 0x10, no, 0x1005396C, 0x6E5, 0x10053954);
}
VERIFY(0x025B7F68, dSv_player_map_c_isGetMap);

/* 025B7FE0 dSv_player_map_c::onOpenMap (not named by the matcher) */
static void dSv_player_map_c_onOpenMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B7FE0, void, t, no);
    mapOn_l(t + 0x20, no, 0x10053994, 0x706, 0x1005397C);
}
VERIFY(0x025B7FE0, dSv_player_map_c_onOpenMap);

/* 025B8054 dSv_player_map_c::offOpenMap (not named by the matcher) */
static void dSv_player_map_c_offOpenMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B8054, void, t, no);
    mapOff_l(t + 0x20, no, 0x100539B8, 0x714, 0x100539A0);
}
VERIFY(0x025B8054, dSv_player_map_c_offOpenMap);

/* 025B80C8 */
static BOOL dSv_player_map_c_isOpenMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B80C8, BOOL, t, no);
    return mapIs_l(t + 0x20, no, 0x100539DC, 0x722, 0x100539C4);
}
VERIFY(0x025B80C8, dSv_player_map_c_isOpenMap);

/* 025B8140 */
static void dSv_player_map_c_onCompleteMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B8140, void, t, no);
    mapOn_l(t + 0x30, no, 0x10053A04, 0x743, 0x100539EC);
}
VERIFY(0x025B8140, dSv_player_map_c_onCompleteMap);

/* 025B81B4 dSv_player_map_c::offCompleteMap (not named by the matcher) */
static void dSv_player_map_c_offCompleteMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B81B4, void, t, no);
    mapOff_l(t + 0x30, no, 0x10053A28, 0x751, 0x10053A10);
}
VERIFY(0x025B81B4, dSv_player_map_c_offCompleteMap);

/* 025B8228 */
static BOOL dSv_player_map_c_isCompleteMap(u32 t, u32 no) {
    WWHD_FUNC(0x025B8228, BOOL, t, no);
    return mapIs_l(t + 0x30, no, 0x10053A4C, 0x75F, 0x10053A34);
}
VERIFY(0x025B8228, dSv_player_map_c_isCompleteMap);

/* 025B82A0 dSv_player_map_c::onTriforce (not named by the matcher; no offTriforce in HD) */
static void dSv_player_map_c_onTriforce(u32 t, u32 no) {
    WWHD_FUNC(0x025B82A0, void, t, no);
    bitOn8_l(t + 0x81, no, 8, 0x10053A74, 0x781, 0x10053A5C);
}
VERIFY(0x025B82A0, dSv_player_map_c_onTriforce);

/* 025B8308 */
static BOOL dSv_player_map_c_isTriforce(u32 t, u32 no) {
    WWHD_FUNC(0x025B8308, BOOL, t, no);
    return bitIs8_l(t + 0x81, no, 8, 0x10053A9C, 0x79D, 0x10053A84);
}
VERIFY(0x025B8308, dSv_player_map_c_isTriforce);

/* 025B8378 */
static s32 dSv_player_map_c_getCollectMapNum(u32 t) {
    WWHD_FUNC(0x025B8378, s32, t);
    s32 num = 0;
    for (s32 i = 1; i <= 61; i++) {
        if (i != 35 && i != 36 && i < 52 && dSv_player_map_c_isGetMap(t, (u32)(i - 1))) num++;
    }
    return num;
}
VERIFY(0x025B8378, dSv_player_map_c_getCollectMapNum);

/* 025B840C dSv_player_map_c::onFmapBit (not named by the matcher) */
static void dSv_player_map_c_onFmapBit(u32 t, u32 idx, u32 no) {
    WWHD_FUNC(0x025B840C, void, t, idx, no);
    bitOn8_l(t + 0x40 + idx, no, 8, 0x10053AC4, 0x7D4, 0x10053AAC);
}
VERIFY(0x025B840C, dSv_player_map_c_onFmapBit);

/* 025B8484 dSv_player_map_c::onSaveArriveGrid (not named by the matcher) */
static void dSv_player_map_c_onSaveArriveGrid(u32 t, u32 no) {
    WWHD_FUNC(0x025B8484, void, t, no);
    if (no >= 0x31) JUT_ASSERT_l(0x10053AD0, 0x7FB, 0x10053ADC);
    dSv_player_map_c_onFmapBit(t, no, 0);
}
VERIFY(0x025B8484, dSv_player_map_c_onSaveArriveGrid);

/* 025B84E8 dSv_player_map_c::isFmapBit (not named by the matcher) */
static BOOL dSv_player_map_c_isFmapBit(u32 t, u32 idx, u32 no) {
    WWHD_FUNC(0x025B84E8, BOOL, t, idx, no);
    return bitIs8_l(t + idx + 0x40, no, 8, 0x10053B14, 0x7F2, 0x10053AFC);
}
VERIFY(0x025B84E8, dSv_player_map_c_isFmapBit);

/* 025B8568 dSv_player_map_c::isSaveArriveGrid (not named by the matcher) */
static BOOL dSv_player_map_c_isSaveArriveGrid(u32 t, u32 no) {
    WWHD_FUNC(0x025B8568, BOOL, t, no);
    if (no >= 0x31) JUT_ASSERT_l(0x10053B20, 0x809, 0x10053B2C);
    return dSv_player_map_c_isFmapBit(t, no, 0);
}
VERIFY(0x025B8568, dSv_player_map_c_isSaveArriveGrid);

/* 025B85CC dSv_player_map_c::onSaveArriveGridForAgb (not named by the matcher; no
 * isSaveArriveGridForAgb in HD) */
static void dSv_player_map_c_onSaveArriveGridForAgb(u32 t, u32 no) {
    WWHD_FUNC(0x025B85CC, void, t, no);
    if (no >= 0x31) JUT_ASSERT_l(0x10053B44, 0x810, 0x10053B50);
    dSv_player_map_c_onFmapBit(t, no, 1);
}
VERIFY(0x025B85CC, dSv_player_map_c_onSaveArriveGridForAgb);

/* strncpy(dst, src, 17) (inline) */
static inline void strncpy17_inl(u32 dst, u32 src) {
    u32 i = 0;
    for (; i < 0x11; i++) {
        u8 c = ld8(src + i);
        st8(dst + i, c);
        if (c == 0) break;
    }
    for (; i < 0x11; i++) st8(dst + i, 0);
}

/* 025B8630 */
static void dSv_player_info_c_init(u32 t) {
    WWHD_FUNC(0x025B8630, void, t);
    const u32 l_defaultName = 0x101EADA0;
    strncpy17_inl(t + 0x14, l_defaultName);
    st16(t + 0x10, 0);
    strncpy17_inl(t + 0x25, l_defaultName);
    strncpy17_inl(t + 0x36, l_defaultName);
    st16(t + 0x12, 0);
    st8(t + 0x58, 0);
    for (u32 i = 0; i < 0x11; i++) st8(t + 0x47 + i, 0);
    u8 pt = (u8)gabi::ftoi(cM_rndF_l(3.0f));
    if (pt >= 3) pt = 2;
    st8(t + 0x59, pt);
}
VERIFY(0x025B8630, dSv_player_info_c_init);

/* 025B8800: HD sound mode 2 without the OSGetSoundMode test */
static void dSv_player_config_c_init(u32 t) {
    WWHD_FUNC(0x025B8800, void, t);
    st8(t + 0, 1);
    st8(t + 1, 2);
    mDoAud_setOutputMode_l(2);
    st8(t + 3, 1);
    st8(t + 2, 0);
}
VERIFY(0x025B8800, dSv_player_config_c_init);

/* 025B8850: HD returns the play object's vibration setting without the rumble check */
static u8 dSv_player_config_c_checkVibration(u32 t) {
    WWHD_FUNC(0x025B8850, u8, t);
    return ld8(play() + 0x5BED);
}
VERIFY(0x025B8850, dSv_player_config_c_checkVibration);

/* 025B8874 */
static void dSv_player_priest_c_init(u32 t) {
    WWHD_FUNC(0x025B8874, void, t);
    st8(t + 0xF, 0);
}
VERIFY(0x025B8874, dSv_player_priest_c_init);

/* 025B8880 */
static void dSv_player_priest_c_set(u32 t, u32 a, u32 pos, u32 angle, u32 roomNo) {
    WWHD_FUNC(0x025B8880, void, t, a, pos, angle, roomNo);
    st8(t + 0xF, (u8)a);
    st(t + 0, ld(pos + 0)); /* cXyz copied as words */
    st(t + 4, ld(pos + 4));
    u32 z = ld(pos + 8);
    st8(t + 0xE, (u8)roomNo);
    st(t + 8, z);
    st16(t + 0xC, (u16)angle);
}
VERIFY(0x025B8880, dSv_player_priest_c_set);

/* 025B88A8 HD-only: initialises a compact copy of the player item data (status a, items, two
 * 3-byte records, bag items, bag item record, collect); purpose unknown */
static void dSv_player_sub_init(u32 t) {
    WWHD_FUNC(0x025B88A8, void, t);
    gabi::call(0x025B4DC0, t);        /* dSv_player_status_a_c::init */
    gabi::call(0x025B51BC, t + 0x18); /* dSv_player_item_c::init */
    gabi::call(0x025B60D8, t + 0x2D);
    gabi::call(0x025B61D0, t + 0x30);
    gabi::call(0x025B6214, t + 0x33); /* dSv_player_bag_item_c::init */
    gabi::call(0x025B78AC, t + 0x4B); /* dSv_player_bag_item_record_c::init */
    gabi::call(0x025B790C, t + 0x63); /* dSv_player_collect_c::init */
}
VERIFY(0x025B88A8, dSv_player_sub_init);

/* 025B8904 */
static void dSv_player_c_init(u32 t) {
    WWHD_FUNC(0x025B8904, void, t);
    gabi::call(0x025B4DC0, t);         /* mPlayerStatusA */
    gabi::call(0x025B5028, t + 0x18);  /* mPlayerStatusB */
    gabi::call(0x025B5060, t + 0x30);  /* mReturnPlace */
    gabi::call(0x025B51BC, t + 0x3C);  /* mPlayerItem */
    gabi::call(0x025B5CAC, t + 0x51);  /* mGetItem */
    gabi::call(0x025B60EC, t + 0x66);  /* mItemRecord */
    gabi::call(0x025B61E4, t + 0x6E);  /* mItemMax */
    gabi::call(0x025B6214, t + 0x76);  /* mBagItem */
    gabi::call(0x025B7614, t + 0x90);  /* mGetBagItem */
    gabi::call(0x025B78AC, t + 0x9C);  /* mBagItemRecord */
    gabi::call(0x025B790C, t + 0xB4);  /* mCollect */
    dSv_player_map_c_init(t + 0xC4);   /* mMap */
    dSv_player_info_c_init(t + 0x148); /* mInfo */
    dSv_player_config_c_init(t + 0x1A4); /* mConfig */
}
VERIFY(0x025B8904, dSv_player_c_init);

/* 025B8998 */
static void dSv_ocean_c_init(u32 t) {
    WWHD_FUNC(0x025B8998, void, t);
    for (u32 i = 0; i < 50; i++) st16(t + 2 * i, 0);
}
VERIFY(0x025B8998, dSv_ocean_c_init);

/* 025B89B4 */
static void dSv_ocean_c_onOceanSvBit(u32 t, u32 grid, u32 bit) {
    WWHD_FUNC(0x025B89B4, void, t, grid, bit);
    if (grid > 0x31) JUT_ASSERT_l(0x10053B98, 0xA96, 0x10053B74);
    if (bit >= 0x10) JUT_ASSERT_l(0x10053B98, 0xA97, 0x10053BA4);
    u32 a = t + (grid << 1);
    st16(a, (u16)(ld16(a) | slw(1, bit)));
}
VERIFY(0x025B89B4, dSv_ocean_c_onOceanSvBit);

/* 025B8A50 */
static BOOL dSv_ocean_c_isOceanSvBit(u32 t, u32 grid, u32 bit) {
    WWHD_FUNC(0x025B8A50, BOOL, t, grid, bit);
    if (grid > 0x31) JUT_ASSERT_l(0x10053BF0, 0xAB6, 0x10053BCC);
    if (bit >= 0x10) JUT_ASSERT_l(0x10053BF0, 0xAB7, 0x10053BFC);
    return (ld16((grid << 1) + t) & (u16)slw(1, bit)) ? TRUE : FALSE;
}
VERIFY(0x025B8A50, dSv_ocean_c_isOceanSvBit);

static inline u32 evIdx(u32 no) { return (u32)((s32)no >> 8); }

/* 025B8AF4 */
static void dSv_event_c_setEventReg(u32 t, u32 reg, u32 no) {
    WWHD_FUNC(0x025B8AF4, void, t, reg, no);
    u32 a = t + evIdx(reg);
    st8(a, (u8)((ld8(a) & ~(reg & 0xFF)) | no));
}
VERIFY(0x025B8AF4, dSv_event_c_setEventReg);

/* 025B8B10: setInitEventBit (d_save_init) inlined: the Kg1 high score (constant 1004BD18) and
 * register 0x7EFF = 0xE */
static void dSv_event_c_init(u32 t) {
    WWHD_FUNC(0x025B8B10, void, t);
    for (u32 i = 0; i < 0x100; i++) st8(t + i, 0);
    dSv_event_c_setEventReg(t, 0xBEFF, ld8(0x1004BD18));
    dSv_event_c_setEventReg(t, 0x7EFF, 0xE);
}
VERIFY(0x025B8B10, dSv_event_c_init);

/* 025B8B68 */
static void dSv_event_c_onEventBit(u32 t, u32 no) {
    WWHD_FUNC(0x025B8B68, void, t, no);
    u32 a = t + evIdx(no);
    st8(a, (u8)(ld8(a) | no));
}
VERIFY(0x025B8B68, dSv_event_c_onEventBit);

/* 025B8B7C */
static void dSv_event_c_offEventBit(u32 t, u32 no) {
    WWHD_FUNC(0x025B8B7C, void, t, no);
    u32 a = t + evIdx(no);
    st8(a, (u8)(ld8(a) & ~(no & 0xFF)));
}
VERIFY(0x025B8B7C, dSv_event_c_offEventBit);

/* 025B8B94 */
static BOOL dSv_event_c_isEventBit(u32 t, u32 no) {
    WWHD_FUNC(0x025B8B94, BOOL, t, no);
    return (ld8(t + evIdx(no)) & (no & 0xFF)) ? TRUE : FALSE;
}
VERIFY(0x025B8B94, dSv_event_c_isEventBit);

/* 025B8BB0 */
static u8 dSv_event_c_getEventReg(u32 t, u32 reg) {
    WWHD_FUNC(0x025B8BB0, u8, t, reg);
    return (u8)(ld8(t + evIdx(reg)) & reg);
}
VERIFY(0x025B8BB0, dSv_event_c_getEventReg);

}  // namespace d_save_3_cpp
