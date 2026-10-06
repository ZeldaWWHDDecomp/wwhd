/* d_save part 5: dSv_info_c and the unit's __sinit (025B99DC..025BAF8F). WWHD. See d_save.cpp for the unit's range and the save layout.
 *
 * dSv_info_c (HD): save data (dSv_save_c, 0x778) 0, memory 0x778, dan 0x79C, zones [32] (0x4C
 * each) 0x7A8, tmp event 0x1158, 0x1290..0x1292, 0x1298, 0x129C (HD, zeroed by init); the
 * HD-only save area at info + 0x12A0 (save base + 0x12C0) is reached through 027200D0 /
 * 02720144. Room zone numbers: the room status table at 1047E8EB + room * 0x22C (s8), read after
 * a dComIfGp_get call (dComIfGp_roomControl_getZoneNo inlined).
 * Card data (0xA94 per slot, 0xA8C used): status a 0x18, status b 0x18, return place 0xC, item
 * 0x15, get item 0x15, item record 8, item max 8, bag item 0x18, get bag item 0xC, bag item
 * record 0x18, collect 0xD, map 0x84, info 0x5C, config 5, priest 0x10, status c [4] 0x70,
 * memory [16] 0x24, ocean 0x64, event 0x100, reserve 0x50. */
#include "bindings.h"

namespace d_save_5_cpp {
#include "d_save_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void memcpy_l(u32 dst, u32 src, u32 n) { gabi::call(0xC000A848, dst, src, n); } /* memcpy (import) */
static inline void memset_l(u32 dst, u32 v, u32 n) { gabi::call(0xC000A858, dst, v, n); } /* memset (import) */
static inline void __register_global_object_l(u32 d) { gabi::call(0x028F026C, d); }
/* 025E1C74 probably mDoAud_setOutputMode */
static inline void mDoAud_setOutputMode_l(u32 mode) { gabi::call(0x025E1C74, mode); }
/* HD save area accessors (argument: save base + 0x12C0) */
static inline u32 hdSave_027200D0_l(u32 p) { return gabi::call<u32>(0x027200D0, p); }
static inline u32 dComIfGs_getPictureInfo_l(u32 p) { return gabi::call<u32>(0x02720144, p); }
static inline void hdPicture_setNum_027262B0_l(u32 p, u32 num) { gabi::call(0x027262B0, p, num); }
/* calls into the other parts (by address) */
static inline u8 dSv_event_getEventReg_l(u32 ev, u32 reg) { return gabi::call<u8>(0x025B8BB0, ev, reg); }
static inline void dSv_event_setEventReg_l(u32 ev, u32 reg, u32 no) { gabi::call(0x025B8AF4, ev, reg, no); }
static inline void dSv_event_onEventBit_l(u32 ev, u32 no) { gabi::call(0x025B8B68, ev, no); }
static inline void dSv_zone_init_l(u32 z, s32 roomNo) { gabi::call(0x025B97DC, z, roomNo); }

template <u32 N> struct blk_l { u8 b[N]; };

/* dComIfGp_roomControl_getZoneNo(roomNo) (inline): HD calls dComIfGp_get, then reads the static
 * room status table */
static inline s32 getZoneNo_inl(u32 roomNo) {
    play();
    return (s8)ld8(0x1047E8EB + roomNo * 0x22C);
}
static inline u32 zone(u32 t, s32 zoneNo) { return t + 0x7A8 + (u32)zoneNo * 0x4C; }

/* 025B99DC */
static void dSv_info_c_initZone(u32 t) {
    WWHD_FUNC(0x025B99DC, void, t);
    for (u32 i = 0; i < 32; i++) dSv_zone_init_l(t + 0x7A8 + 0x4C * i, -1);
}
VERIFY(0x025B99DC, dSv_info_c_initZone);

/* 025B9A18: HD also clears five HD fields */
static void dSv_info_c_init(u32 t) {
    WWHD_FUNC(0x025B9A18, void, t);
    gabi::call(0x025B9938, t);          /* mSavedata.init() */
    gabi::call(0x025B9170, t + 0x778);  /* mMemory.init() */
    gabi::call(0x025B9174, t + 0x79C, -1); /* mDan.init(-1) */
    dSv_info_c_initZone(t);
    gabi::call(0x025B8B10, t + 0x1158); /* mTmp.init() */
    st(t + 0x129C, 0);
    st8(t + 0x1291, 0);
    st(t + 0x1298, 0);
    st8(t + 0x1292, 0);
    st8(t + 0x1290, 0);
    st(0x101D5F1C, 0); /* daNpc_Sarace_c::ship_race_rupee */
    st(0x101D5F18, 0); /* daNpc_Sarace_c::ship_race_result */
}
VERIFY(0x025B9A18, dSv_info_c_init);

/* 025B9A90: HD keeps the sound mode, vibration and two values of the HD save area (when < 2),
 * the clear count, the picture count and register 0x89FF; the player name, attention type and
 * ruby are not kept */
static void dSv_info_c_reinit(u32 t) {
    WWHD_FUNC(0x025B9A90, void, t);
    const u32 l_holdEventReg = 0x101EADA8, l_onEventBit = 0x101EADCC;
    u8 hold[17];
    u32 b = svbase(); /* kept in a register across the getEventReg calls */
    for (u32 i = 0; i < 17; i++) hold[i] = dSv_event_getEventReg_l(b + 0x644, ld16(l_holdEventReg + 2 * i));
    b = svbase();
    u8 clearCount = ld8(b + 0x1C0);
    u8 hd0 = ld8(hdSave_027200D0_l(b + 0x12C0) + 0);
    b = svbase();
    u8 hd3 = ld8(hdSave_027200D0_l(b + 0x12C0) + 3);
    b = svbase();
    u8 vib = ld8(b + 0x1C7);
    u8 sound = ld8(b + 0x1C5);
    u32 pic = dComIfGs_getPictureInfo_l(b + 0x12C0);
    u8 pictureNum = ld8(pic + 0x3C030C);
    u8 reg89 = dSv_event_getEventReg_l(svbase() + 0x644, 0x89FF);

    dSv_info_c_init(t);

    for (u32 i = 0; i < 17; i++) dSv_event_setEventReg_l(svbase() + 0x644, ld16(l_holdEventReg + 2 * i), hold[i]);
    for (u32 i = 0; i < 5; i++) dSv_event_onEventBit_l(svbase() + 0x644, ld16(l_onEventBit + 2 * i));
    dSv_event_setEventReg_l(svbase() + 0x644, 0xC407, 7);
    st8(svbase() + 0x1C0, clearCount);
    gabi::call(0x025B8C0C, svbase() + 0x52C, 0);    /* onSaveTbox(STAGE_MISC, 0) */
    gabi::call(0x025B8CE0, svbase() + 0x3A0, 0x47); /* onSaveSwitch(STAGE_SEA, 0x47) */
    gabi::call(0x025B8CE0, svbase() + 0x3A0, 0x5E); /* onSaveSwitch(STAGE_SEA, 0x5E) */
    u32 p = hdSave_027200D0_l(svbase() + 0x12C0);
    if (hd0 < 2) st8(p + 0, hd0);
    p = hdSave_027200D0_l(svbase() + 0x12C0);
    if (hd3 < 2) st8(p + 3, hd3);
    st8(svbase() + 0x1C5, sound);
    st8(svbase() + 0x1C7, vib);
    st8(svbase() + 0x1C1, 3); /* setRandomSalvagePoint(3) */
    st8(svbase() + 0x64, 0x26); /* setItem(CAMERA, DELUXE_PICTO_BOX) */
    u32 pl = play();
    st8(pl + PL_ITEMNO, 0x26);
    st8(pl + PL_ITEMSLOT, 8);
    gabi::call(0x025B5CCC, svbase() + 0x71, 8, 0); /* onGetItem(CAMERA, 0) */
    gabi::call(0x025B5CCC, svbase() + 0x71, 8, 1); /* onGetItem(CAMERA, 1) */
    pic = dComIfGs_getPictureInfo_l(svbase() + 0x12C0);
    hdPicture_setNum_027262B0_l(pic, pictureNum);
    dSv_event_setEventReg_l(svbase() + 0x644, 0x89FF, reg89);
}
VERIFY(0x025B9A90, dSv_info_c_reinit);

/* 025B9C9C */
static void dSv_info_c_getSave(u32 t, u32 stageNo) {
    WWHD_FUNC(0x025B9C9C, void, t, stageNo);
    if (stageNo >= 0x10) JUT_ASSERT_l(0x10054034, 0xD77, 0x10054000);
    u32 src = t + stageNo * 0x24 + 0x380;
    for (u32 i = 0; i < 9; i++) st(t + 0x778 + 4 * i, ld(src + 4 * i));
}
VERIFY(0x025B9C9C, dSv_info_c_getSave);

/* 025B9D24: the memory goes through a stack copy */
static void dSv_info_c_putSave(u32 t, u32 stageNo) {
    WWHD_FUNC(0x025B9D24, void, t, stageNo);
    if (stageNo >= 0x10) JUT_ASSERT_l(0x10054074, 0xD87, 0x10054040);
    u32 tmp[9];
    for (u32 i = 0; i < 9; i++) tmp[i] = ld(t + 0x778 + 4 * i);
    u32 dst = t + stageNo * 0x24 + 0x380;
    for (u32 i = 0; i < 9; i++) st(dst + 4 * i, tmp[i]);
}
VERIFY(0x025B9D24, dSv_info_c_putSave);

/* 025B9DD8 */
static s32 dSv_info_c_createZone(u32 t, u32 roomNo) {
    WWHD_FUNC(0x025B9DD8, s32, t, roomNo);
    u32 z = t + 0x7A8;
    for (s32 i = 0; i < 32; i++) {
        if ((s8)ld8(z) < 0) {
            dSv_zone_init_l(z, (s32)roomNo);
            return i;
        }
        z += 0x4C;
    }
    return -1;
}
VERIFY(0x025B9DD8, dSv_info_c_createZone);

/* switch numbers: memory 0..0x7F, dungeon 0x80..0xBF, zone 0xC0..0xEF */
/* 025B9E38 */
static void dSv_info_c_onSwitch(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025B9E38, void, t, no, roomNo);
    if (no == -1 || no == 0xFF) return;
    if ((u32)no >= 0xF0) JUT_ASSERT_l(0x10054080, 0xDBF, 0x100540D0);
    if (no < 0x80) {
        gabi::call(0x025B8CE0, t + 0x778, no);
    } else if (no < 0xC0) {
        gabi::call(0x025B91AC, t + 0x79C, no - 0x80);
    } else {
        if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x10054080, 0xDD0, 0x1005408C);
        s32 zoneNo = getZoneNo_inl((u32)roomNo);
        if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x10054080, 0xDD2, 0x100540AC);
        gabi::call(0x025B93BC, zone(t, zoneNo) + 2, no - 0xC0);
    }
}
VERIFY(0x025B9E38, dSv_info_c_onSwitch);

/* 025B9F7C */
static void dSv_info_c_offSwitch(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025B9F7C, void, t, no, roomNo);
    if (no == -1 || no == 0xFF) return;
    if ((u32)no >= 0xF0) JUT_ASSERT_l(0x1005412C, 0xDE4, 0x1005417C);
    if (no < 0x80) {
        gabi::call(0x025B8D54, t + 0x778, no);
    } else if (no < 0xC0) {
        gabi::call(0x025B9220, t + 0x79C, no - 0x80);
    } else {
        if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x1005412C, 0xDF5, 0x10054138);
        s32 zoneNo = getZoneNo_inl((u32)roomNo);
        if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x1005412C, 0xDF7, 0x10054158);
        gabi::call(0x025B9430, zone(t, zoneNo) + 2, no - 0xC0);
    }
}
VERIFY(0x025B9F7C, dSv_info_c_offSwitch);

/* 025BA0C0 */
static BOOL dSv_info_c_isSwitch(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025BA0C0, BOOL, t, no, roomNo);
    if (no == -1 || no == 0xFF) return FALSE;
    if (no < 0x80) return gabi::call<BOOL>(0x025B8DC8, t + 0x778, no);
    if (no < 0xC0) return gabi::call<BOOL>(0x025B9294, t + 0x79C, no - 0x80);
    if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x100541D8, 0xE25, 0x100541E4);
    s32 zoneNo = getZoneNo_inl((u32)roomNo);
    if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x100541D8, 0xE27, 0x10054204);
    return gabi::call<BOOL>(0x025B94A4, zone(t, zoneNo) + 2, no - 0xC0);
}
VERIFY(0x025BA0C0, dSv_info_c_isSwitch);

/* 025BA20C */
static BOOL dSv_info_c_revSwitch(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025BA20C, BOOL, t, no, roomNo);
    if (no == -1 || no == 0xFF) return FALSE;
    if ((u32)no >= 0xF0) JUT_ASSERT_l(0x10054228, 0xE38, 0x10054278);
    if (no < 0x80) return gabi::call<BOOL>(0x025B8E40, t + 0x778, no);
    if (no < 0xC0) return gabi::call<BOOL>(0x025B930C, t + 0x79C, no - 0x80);
    if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x10054228, 0xE48, 0x10054234);
    s32 zoneNo = getZoneNo_inl((u32)roomNo);
    if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x10054228, 0xE4A, 0x10054254);
    return gabi::call<BOOL>(0x025B951C, zone(t, zoneNo) + 2, no - 0xC0);
}
VERIFY(0x025BA20C, dSv_info_c_revSwitch);

/* item numbers: memory 0..0x3F, zone 0x40..0x4F */
/* 025BA384 */
static void dSv_info_c_onItem(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025BA384, void, t, no, roomNo);
    if (no == -1 || no == 0x7F) return;
    if ((u32)no >= 0x50) JUT_ASSERT_l(0x100542D4, 0xE5B, 0x10054324);
    if (no < 0x40) {
        gabi::call(0x025B8EC0, t + 0x778, no);
    } else {
        if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x100542D4, 0xE67, 0x100542E0);
        s32 zoneNo = getZoneNo_inl((u32)roomNo);
        if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x100542D4, 0xE69, 0x10054300);
        gabi::call(0x025B95A0, zone(t, zoneNo) + 2, no - 0x40);
    }
}
VERIFY(0x025BA384, dSv_info_c_onItem);

/* 025BA494 */
static BOOL dSv_info_c_isItem(u32 t, s32 no, s32 roomNo) {
    WWHD_FUNC(0x025BA494, BOOL, t, no, roomNo);
    if (no == -1 || no == 0x7F) return FALSE;
    if ((u32)no >= 0x50) JUT_ASSERT_l(0x1005437C, 0xE9B, 0x100543CC);
    if (no < 0x40) return gabi::call<BOOL>(0x025B8F34, t + 0x778, no);
    if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x1005437C, 0xEA6, 0x10054388);
    s32 zoneNo = getZoneNo_inl((u32)roomNo);
    if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x1005437C, 0xEA8, 0x100543A8);
    return gabi::call<BOOL>(0x025B9608, zone(t, zoneNo) + 2, no - 0x40);
}
VERIFY(0x025BA494, dSv_info_c_isItem);

/* 025BA5D4 */
static void dSv_info_c_onActor(u32 t, s32 id, s32 roomNo) {
    WWHD_FUNC(0x025BA5D4, void, t, id, roomNo);
    if (id == -1 || (u32)id == 0xFFFF || roomNo == -1) return;
    if ((u32)id >= 0x200 || (u32)roomNo >= 0x40) JUT_ASSERT_l(0x10054424, 0xEDC, 0x10054454);
    s32 zoneNo = getZoneNo_inl((u32)roomNo);
    if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x10054424, 0xEDF, 0x10054430);
    gabi::call(0x025B9690, zone(t, zoneNo) + 0xC, id);
}
VERIFY(0x025BA5D4, dSv_info_c_onActor);

/* 025BA6A4 */
static BOOL dSv_info_c_isActor(u32 t, s32 id, s32 roomNo) {
    WWHD_FUNC(0x025BA6A4, BOOL, t, id, roomNo);
    if (id == -1 || (u32)id == 0xFFFF || roomNo == -1) return FALSE;
    if ((u32)id >= 0x200) JUT_ASSERT_l(0x100544B4, 0xF10, 0x100544C0);
    if ((u32)roomNo >= 0x40) JUT_ASSERT_l(0x100544B4, 0xF12, 0x100544F0);
    s32 zoneNo = getZoneNo_inl((u32)roomNo);
    if ((u32)zoneNo >= 0x20) JUT_ASSERT_l(0x100544B4, 0xF14, 0x10054510);
    return gabi::call<BOOL>(0x025B9700, zone(t, zoneNo) + 0xC, id);
}
VERIFY(0x025BA6A4, dSv_info_c_isActor);

/* the card layout: (save base offset, size) per block, status c and memory handled apart */
struct cardBlk_l { u32 ofs, size; };
static constexpr cardBlk_l kCardHead[] = {
    {0x20, 0x18}, {0x38, 0x18}, {0x50, 0xC}, {0x5C, 0x15}, {0x71, 0x15}, {0x86, 8}, {0x8E, 8},
    {0x96, 0x18}, {0xB0, 0xC}, {0xBC, 0x18}, {0xD4, 0xD}, {0xE4, 0x84}, {0x168, 0x5C}, {0x1C4, 5}};

/* 025BA7B0: HD always sets the sound mode to 2 */
static s32 dSv_info_c_card_to_memory(u32 t, u32 card, s32 dataNum) {
    WWHD_FUNC(0x025BA7B0, s32, t, card, dataNum);
    u32 start = card + (u32)dataNum * 0xA94;
    u32 buf = start;
    for (const cardBlk_l& c : kCardHead) {
        memcpy_l(svbase() + c.ofs, buf, c.size);
        buf += c.size;
    }
    st8(svbase() + 0x1C5, 2);
    mDoAud_setOutputMode_l(2);
    memcpy_l(svbase() + 0x1CC, buf, 0x10); /* priest */
    buf += 0x10;
    for (u32 i = 0; i < 4; i++) {
        memcpy_l(svbase() + 0x1DC + 0x70 * i, buf, 0x70); /* status c */
        buf += 0x70;
    }
    memcpy_l(svbase() + 0x3A0, buf, 0x240); /* memory [16] */
    buf += 0x240;
    memcpy_l(svbase() + 0x5E0, buf, 0x64); /* ocean */
    buf += 0x64;
    memcpy_l(svbase() + 0x644, buf, 0x100); /* event */
    buf += 0x100;
    memcpy_l(svbase() + 0x744, buf, 0x50); /* reserve */
    buf += 0x50;
    return (buf - start) > 0xA8C ? -1 : 0;
}
VERIFY(0x025BA7B0, dSv_info_c_card_to_memory);

/* 025BA9FC: HD stores no IPL date */
static s32 dSv_info_c_memory_to_card(u32 t, u32 card, s32 dataNum) {
    WWHD_FUNC(0x025BA9FC, s32, t, card, dataNum);
    u32 start = card + (u32)dataNum * 0xA94;
    u32 buf = start;
    bool first = true;
    for (const cardBlk_l& c : kCardHead) {
        memcpy_l(buf, svbase() + c.ofs, c.size);
        if (first) {
            if (ld16(buf + 2) < 0xC) st16(buf + 2, 0xC); /* life */
            first = false;
        }
        buf += c.size;
    }
    memcpy_l(buf, svbase() + 0x1CC, 0x10);
    buf += 0x10;
    for (u32 i = 0; i < 4; i++) {
        memcpy_l(buf, svbase() + 0x1DC + 0x70 * i, 0x70);
        buf += 0x70;
    }
    memcpy_l(buf, svbase() + 0x3A0, 0x240);
    buf += 0x240;
    memcpy_l(buf, svbase() + 0x5E0, 0x64);
    buf += 0x64;
    memcpy_l(buf, svbase() + 0x644, 0x100);
    buf += 0x100;
    memcpy_l(buf, svbase() + 0x744, 0x50);
    buf += 0x50;
    return (buf - start) > 0xA8C ? -1 : 0;
}
VERIFY(0x025BA9FC, dSv_info_c_memory_to_card);

/* 025BAC50: HD copies the one initialised memory 16 times (GameCube copied 15 uninitialised
 * stack memories after it).
 * The init functions leave padding and some fields of their objects unset, and those bytes go to
 * the card as they are (status B padding, return place +0xA, get bag item +6, priest, status C
 * padding, ...): what the stack held before. So the objects sit where the original's frame
 * (stwu r1,-0x420) has them and are not initialised (gabi::NativeFrame; game test 2026-10-05: the
 * empty slots of cking.sav must keep these stale bytes), and the frame's own stores (back chain,
 * saved r28..r31, LR) are written like the original's. */
static s32 dSv_info_c_initdata_to_card(u32 t, u32 card, s32 dataNum) {
    WWHD_FUNC(0x025BAC50, s32, t, card, dataNum);
    gabi::NativeFrame<0x420> f;
    const u32 sp = f.sp();
    st(f.entry() + 4, gabi::cpu->lr);           /* stw r0,0x424(r1) */
    st(sp, f.entry());                          /* stwu back chain */
    st(sp + 0x418, gabi::cpu->r[30]);
    st(sp + 0x410, gabi::cpu->r[28]);
    st(sp + 0x414, gabi::cpu->r[29]);
    st(sp + 0x41C, gabi::cpu->r[31]);
    u32 start = card + (u32)dataNum * 0xA94;
    u32 buf = start;
    auto copy = [&](u32 off, u32 n, u32 initFn) {
        if (initFn) gabi::call(initFn, sp + off);
        memcpy_l(buf, sp + off, n);
        buf += n;
    };
    copy(0x88, 0x18, 0x025B4DC0);  /* status a */
    copy(0xD0, 0x18, 0x025B5028);  /* status b */
    copy(0x20, 0xC, 0x025B5060);   /* return place */
    copy(0x58, 0x15, 0x025B51BC);  /* item */
    copy(0x70, 0x15, 0x025B5CAC);  /* get item */
    copy(0x10, 8, 0x025B60EC);     /* item record */
    copy(0x18, 8, 0x025B61E4);     /* item max */
    copy(0xA0, 0x18, 0x025B6214);  /* bag item */
    copy(0x2C, 0xC, 0x025B7614);   /* get bag item */
    copy(0xB8, 0x18, 0x025B78AC);  /* bag item record */
    copy(0x38, 0xD, 0x025B790C);   /* collect */
    copy(0x230, 0x84, 0x025B7E74); /* map */
    gabi::call(0x025B8630, sp + 0x2B4); /* player info: initialised, then the card gets zeros (as on GameCube) */
    memset_l(buf, 0, 0x5C);
    buf += 0x5C;
    copy(8, 5, 0x025B8800);        /* config */
    copy(0x48, 0x10, 0x025B8874);  /* priest */
    gabi::call(0x025B88A8, sp + 0x1C0); /* dSv_player_status_c_c::init, copied four times */
    for (u32 i = 0; i < 4; i++) copy(0x1C0, 0x70, 0);
    gabi::call(0x025B9170, sp + 0xE8);  /* dSv_memory_c::init, twice, copied 16 times */
    gabi::call(0x025B9170, sp + 0xE8);
    for (u32 i = 0; i < 0x10; i++) copy(0xE8, 0x24, 0);
    copy(0x15C, 0x64, 0x025B8998); /* ocean */
    copy(0x310, 0x100, 0x025B8B10); /* event */
    copy(0x10C, 0x50, 0x025B9918); /* reserve */
    return (buf - start) > 0xA8C ? -1 : 0;
}
VERIFY(0x025BAC50, dSv_info_c_initdata_to_card);

/* 025BAEFC __sinit_d_save_cpp: the per-TU header statics (as in every unit) */
static void __sinit_d_save_cpp() {
    WWHD_FUNC(0x025BAEFC, void, (u32)0);
    for (int i = 0; i < 4; i++) st(0x1047C90C + 4 * i, 0);
    __register_global_object_l(0x101EADD8);
    stf(0x1047C900, -3.1415927f);
    stf(0x1047C904, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1047C908);
    __register_global_object_l(0x101EADE4);
    gabi::call(0x028EAB2C, 0x1047C909);
    __register_global_object_l(0x101EADF0);
}
VERIFY(0x025BAEFC, __sinit_d_save_cpp);

}  // namespace d_save_5_cpp
