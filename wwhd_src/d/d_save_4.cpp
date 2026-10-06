/* d_save part 4: dSv_memBit_c, dSv_memory_c, dSv_danBit_c, dSv_zoneBit_c, dSv_zoneActor_c,
 * dSv_zone_c, dSv_restart_c, dSv_reserve_c, dSv_save_c, dSv_turnRestart_c (025B8BC4..025B99DB).
 * WWHD. See d_save.cpp for the unit's range.
 *
 * dSv_memBit_c (0x24): tbox 0, switch [4] 4, item [1] 0x14, visited room [2] 0x18, key number
 * 0x20, dungeon item 0x21. dSv_danBit_c: stage number 0, 1, switch [2] 4. dSv_zoneBit_c: switch
 * u16 [3] 0, item u16 6. dSv_zone_c (0x4C): room number 0, zone bit 2, zone actor (u32 [16]) 0xC.
 * dSv_restart_c: room 0, option 1, option room 2, option point 4, option angle 6, option position
 * 8, restart angle 0x16, restart position 0x18. dSv_save_c: player 0, memory [16] (0x24 each)
 * 0x380, ocean 0x5C0, event 0x624. dSv_turnRestart_c: position 0, param 0xC, angle 0x10, room
 * 0x12, 0x13, ship position 0x24, ship angle 0x30, has ship 0x34. */
#include "bindings.h"

namespace d_save_4_cpp {
#include "d_save_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void memzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline void cLib_offsetPos_l(u32 out, u32 pos, s32 angle, u32 ofs) { gabi::call(0x0200F9D4, out, pos, angle, ofs); }

/* u32 bit arrays: word (no >> 5), bit (no & 31) */
static inline void wordsOn_l(u32 a, u32 no, u32 lim, u32 file, s32 line, u32 msg) {
    if (no >= lim) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    st(w, ld(w) | (1u << (no & 31)));
}
static inline void wordsOff_l(u32 a, u32 no, u32 lim, u32 file, s32 line, u32 msg) {
    if (no >= lim) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    st(w, ld(w) & ~(1u << (no & 31)));
}
static inline BOOL wordsIs_l(u32 a, u32 no, u32 lim, u32 file, s32 line, u32 msg) {
    if (no >= lim) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    return (ld(w) & (1u << (no & 31))) ? TRUE : FALSE;
}
static inline BOOL wordsRev_l(u32 a, u32 no, u32 lim, u32 file, s32 line, u32 msg) {
    if (no >= lim) JUT_ASSERT_l(file, line, msg);
    u32 w = a + (u32)(((s32)no >> 5) << 2);
    u32 sw = 1u << (no & 31);
    u32 v = ld(w) ^ sw;
    st(w, v);
    return (v & sw) ? TRUE : FALSE;
}

/* 025B8BC4 dSv_memBit_c::init (not named by the matcher) */
static void dSv_memBit_c_init(u32 t) {
    WWHD_FUNC(0x025B8BC4, void, t);
    st(t + 0, 0);
    for (u32 i = 0; i < 4; i++) st(t + 4 + 4 * i, 0);
    st(t + 0x14, 0);
    for (u32 i = 0; i < 2; i++) st(t + 0x18 + 4 * i, 0);
    st8(t + 0x20, 0);
    st8(t + 0x21, 0);
}
VERIFY(0x025B8BC4, dSv_memBit_c_init);

/* 025B8C0C */
static void dSv_memBit_c_onTbox(u32 t, u32 no) {
    WWHD_FUNC(0x025B8C0C, void, t, no);
    bitOn32_l(t, no, 0x20, 0x10053C1C, 0x912, 0x10053C28);
}
VERIFY(0x025B8C0C, dSv_memBit_c_onTbox);

/* 025B8C74 */
static BOOL dSv_memBit_c_isTbox(u32 t, u32 no) {
    WWHD_FUNC(0x025B8C74, BOOL, t, no);
    return bitIs32_l(t, no, 0x20, 0x10053C44, 0x92E, 0x10053C50);
}
VERIFY(0x025B8C74, dSv_memBit_c_isTbox);

/* 025B8CE0 */
static void dSv_memBit_c_onSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B8CE0, void, t, no);
    wordsOn_l(t + 4, no, 0x80, 0x10053C84, 0x94D, 0x10053C6C);
}
VERIFY(0x025B8CE0, dSv_memBit_c_onSwitch);

/* 025B8D54 */
static void dSv_memBit_c_offSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B8D54, void, t, no);
    wordsOff_l(t + 4, no, 0x80, 0x10053CA8, 0x95B, 0x10053C90);
}
VERIFY(0x025B8D54, dSv_memBit_c_offSwitch);

/* 025B8DC8 */
static BOOL dSv_memBit_c_isSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B8DC8, BOOL, t, no);
    return wordsIs_l(t + 4, no, 0x80, 0x10053CCC, 0x969, 0x10053CB4);
}
VERIFY(0x025B8DC8, dSv_memBit_c_isSwitch);

/* 025B8E40 */
static BOOL dSv_memBit_c_revSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B8E40, BOOL, t, no);
    return wordsRev_l(t + 4, no, 0x80, 0x10053CF0, 0x977, 0x10053CD8);
}
VERIFY(0x025B8E40, dSv_memBit_c_revSwitch);

/* 025B8EC0: HD limit 32 (GameCube 64) */
static void dSv_memBit_c_onItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B8EC0, void, t, no);
    wordsOn_l(t + 0x14, no, 0x20, 0x10053CFC, 0x98A, 0x10053D08);
}
VERIFY(0x025B8EC0, dSv_memBit_c_onItem);

/* 025B8F34 */
static BOOL dSv_memBit_c_isItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B8F34, BOOL, t, no);
    return wordsIs_l(t + 0x14, no, 0x20, 0x10053D2C, 0x9A6, 0x10053D38);
}
VERIFY(0x025B8F34, dSv_memBit_c_isItem);

/* 025B8FAC */
static void dSv_memBit_c_onVisitedRoom(u32 t, u32 no) {
    WWHD_FUNC(0x025B8FAC, void, t, no);
    wordsOn_l(t + 0x18, no, 0x40, 0x10053D5C, 0x9C6, 0x10053D68);
}
VERIFY(0x025B8FAC, dSv_memBit_c_onVisitedRoom);

/* 025B9020 */
static BOOL dSv_memBit_c_isVisitedRoom(u32 t, u32 no) {
    WWHD_FUNC(0x025B9020, BOOL, t, no);
    return wordsIs_l(t + 0x18, no, 0x40, 0x10053D84, 0x9E2, 0x10053D90);
}
VERIFY(0x025B9020, dSv_memBit_c_isVisitedRoom);

/* 025B9098 */
static void dSv_memBit_c_onDungeonItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B9098, void, t, no);
    bitOn8_l(t + 0x21, no, 6, 0x10053DC4, 0xA02, 0x10053DAC);
}
VERIFY(0x025B9098, dSv_memBit_c_onDungeonItem);

/* 025B9100 */
static BOOL dSv_memBit_c_isDungeonItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B9100, BOOL, t, no);
    return bitIs8_l(t + 0x21, no, 6, 0x10053DEC, 0xA1F, 0x10053DD4);
}
VERIFY(0x025B9100, dSv_memBit_c_isDungeonItem);

/* 025B9170 */
static void dSv_memory_c_init(u32 t) {
    WWHD_FUNC(0x025B9170, void, t);
    dSv_memBit_c_init(t);
}
VERIFY(0x025B9170, dSv_memory_c_init);

/* 025B9174 */
static s32 dSv_danBit_c_init(u32 t, u32 stageNo) {
    WWHD_FUNC(0x025B9174, s32, t, stageNo);
    if (stageNo != (u32)(s32)(s8)ld8(t)) {
        st8(t + 0, (u8)stageNo);
        st(t + 4, 0);
        st8(t + 1, 0);
        st(t + 8, 0);
        return 1;
    }
    return 0;
}
VERIFY(0x025B9174, dSv_danBit_c_init);

/* 025B91AC */
static void dSv_danBit_c_onSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B91AC, void, t, no);
    wordsOn_l(t + 4, no, 0x40, 0x10053E0C, 0xB4D, 0x10053E18);
}
VERIFY(0x025B91AC, dSv_danBit_c_onSwitch);

/* 025B9220 */
static void dSv_danBit_c_offSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B9220, void, t, no);
    wordsOff_l(t + 4, no, 0x40, 0x10053E30, 0xB5B, 0x10053E3C);
}
VERIFY(0x025B9220, dSv_danBit_c_offSwitch);

/* 025B9294 */
static BOOL dSv_danBit_c_isSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B9294, BOOL, t, no);
    return wordsIs_l(t + 4, no, 0x40, 0x10053E54, 0xB69, 0x10053E60);
}
VERIFY(0x025B9294, dSv_danBit_c_isSwitch);

/* 025B930C */
static BOOL dSv_danBit_c_revSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B930C, BOOL, t, no);
    return wordsRev_l(t + 4, no, 0x40, 0x10053E78, 0xB77, 0x10053E84);
}
VERIFY(0x025B930C, dSv_danBit_c_revSwitch);

/* 025B938C */
static void dSv_zoneBit_c_init(u32 t) {
    WWHD_FUNC(0x025B938C, void, t);
    for (u32 i = 0; i < 3; i++) st16(t + 2 * i, 0);
    st16(t + 6, 0);
}
VERIFY(0x025B938C, dSv_zoneBit_c_init);

/* 025B93B0 */
static void dSv_zoneBit_c_clearRoomSwitch(u32 t) {
    WWHD_FUNC(0x025B93B0, void, t);
    st16(t + 4, 0);
}
VERIFY(0x025B93B0, dSv_zoneBit_c_clearRoomSwitch);

/* u16 switch words of the zone: word (no >> 4), bit (no & 15) */
static inline u32 zsw(u32 t, u32 no) { return t + (u32)(((s32)no >> 4) << 1); }

/* 025B93BC */
static void dSv_zoneBit_c_onSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B93BC, void, t, no);
    if (no >= 0x30) JUT_ASSERT_l(0x10053E9C, 0xBA3, 0x10053EA8);
    u32 a = zsw(t, no);
    st16(a, (u16)(ld16(a) | (1u << (no & 15))));
}
VERIFY(0x025B93BC, dSv_zoneBit_c_onSwitch);

/* 025B9430 */
static void dSv_zoneBit_c_offSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B9430, void, t, no);
    if (no >= 0x30) JUT_ASSERT_l(0x10053EC8, 0xBB1, 0x10053ED4);
    u32 a = zsw(t, no);
    st16(a, (u16)(ld16(a) & ~(1u << (no & 15))));
}
VERIFY(0x025B9430, dSv_zoneBit_c_offSwitch);

/* 025B94A4 */
static BOOL dSv_zoneBit_c_isSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B94A4, BOOL, t, no);
    if (no >= 0x30) JUT_ASSERT_l(0x10053EF4, 0xBBF, 0x10053F00);
    return (ld16(zsw(t, no)) & (1u << (no & 15))) ? TRUE : FALSE;
}
VERIFY(0x025B94A4, dSv_zoneBit_c_isSwitch);

/* 025B951C */
static BOOL dSv_zoneBit_c_revSwitch(u32 t, u32 no) {
    WWHD_FUNC(0x025B951C, BOOL, t, no);
    if (no >= 0x30) JUT_ASSERT_l(0x10053F20, 0xBCD, 0x10053F2C);
    u32 a = zsw(t, no);
    u32 sw = 1u << (no & 15);
    u16 v = (u16)(ld16(a) ^ sw);
    st16(a, v);
    return (v & sw) ? TRUE : FALSE;
}
VERIFY(0x025B951C, dSv_zoneBit_c_revSwitch);

/* 025B95A0 */
static void dSv_zoneBit_c_onItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B95A0, void, t, no);
    if (no >= 0x10) JUT_ASSERT_l(0x10053F4C, 0xBE0, 0x10053F58);
    st16(t + 6, (u16)(ld16(t + 6) | slw(1, no)));
}
VERIFY(0x025B95A0, dSv_zoneBit_c_onItem);

/* 025B9608 */
static BOOL dSv_zoneBit_c_isItem(u32 t, u32 no) {
    WWHD_FUNC(0x025B9608, BOOL, t, no);
    if (no >= 0x10) JUT_ASSERT_l(0x10053F74, 0xBFC, 0x10053F80);
    return (ld16(t + 6) & slw(1, no)) ? TRUE : FALSE;
}
VERIFY(0x025B9608, dSv_zoneBit_c_isItem);

/* 025B9674 */
static void dSv_zoneActor_c_init(u32 t) {
    WWHD_FUNC(0x025B9674, void, t);
    for (u32 i = 0; i < 0x10; i++) st(t + 4 * i, 0);
}
VERIFY(0x025B9674, dSv_zoneActor_c_init);

/* 025B9690 */
static void dSv_zoneActor_c_on(u32 t, u32 id) {
    WWHD_FUNC(0x025B9690, void, t, id);
    wordsOn_l(t, id, 0x200, 0x10053FBC, 0xC29, 0x10053F9C);
}
VERIFY(0x025B9690, dSv_zoneActor_c_on);

/* 025B9700 */
static BOOL dSv_zoneActor_c_is(u32 t, u32 id) {
    WWHD_FUNC(0x025B9700, BOOL, t, id);
    return wordsIs_l(t, id, 0x200, 0x10053FEC, 0xC45, 0x10053FCC);
}
VERIFY(0x025B9700, dSv_zoneActor_c_is);

/* 025B9774 dSv_zone_c::dSv_zone_c() (not named by the matcher; allocates 0x4C when this == NULL) */
static u32 dSv_zone_c_ct(u32 t) {
    WWHD_FUNC(0x025B9774, u32, t);
    if (t == 0) {
        t = operator_new_l(0x4C);
        if (t == 0) return 0;
    }
    st8(t + 0, 0);
    memzero_l(t + 2, 8);
    memzero_l(t + 0xC, 0x40);
    st8(t + 0, 0xFF);
    return t;
}
VERIFY(0x025B9774, dSv_zone_c_ct);

/* 025B97DC */
static void dSv_zone_c_init(u32 t, u32 roomNo) {
    WWHD_FUNC(0x025B97DC, void, t, roomNo);
    st8(t + 0, (u8)roomNo);
    dSv_zoneBit_c_init(t + 2);
    dSv_zoneActor_c_init(t + 0xC);
}
VERIFY(0x025B97DC, dSv_zone_c_init);

/* 025B9810 */
static void dSv_restart_c_setRoom(u32 t, u32 pos, u32 angle, u32 roomNo) {
    WWHD_FUNC(0x025B9810, void, t, pos, angle, roomNo);
    st8(t + 0, (u8)roomNo);
    st(t + 0x18, ld(pos + 0));
    st(t + 0x1C, ld(pos + 4));
    u32 z = ld(pos + 8);
    st16(t + 0x16, (u16)angle);
    st(t + 0x20, z);
}
VERIFY(0x025B9810, dSv_restart_c_setRoom);

/* 025B9834 */
static void dSv_restart_c_setRestartOption4(u32 t, u32 option, u32 pos, u32 angle, u32 roomNo) {
    WWHD_FUNC(0x025B9834, void, t, option, pos, angle, roomNo);
    st(t + 8, ld(pos + 0));
    st(t + 0xC, ld(pos + 4));
    u32 z = ld(pos + 8);
    st16(t + 6, (u16)angle);
    st(t + 0x10, z);
    st8(t + 2, (u8)roomNo);
    st16(t + 4, 0xFFFF);
    st8(t + 1, (u8)option);
}
VERIFY(0x025B9834, dSv_restart_c_setRestartOption4);

/* 025B9864: the function-local static l_offsetPos (100, 0, 0) is initialised on first use
 * (guard 1047C928, data 1047C91C) */
static void dSv_restart_c_setRestartOption(u32 t, u32 option) {
    WWHD_FUNC(0x025B9864, void, t, option);
    const u32 guard = 0x1047C928, l_offsetPos = 0x1047C91C;
    if (ld(guard) == 0) {
        st(guard, 1);
        stf(l_offsetPos + 8, 0.0f);
        stf(l_offsetPos + 0, 100.0f);
        stf(l_offsetPos + 4, 0.0f);
    }
    gabi::Local<cXyz> pos;
    u32 p = gabi::ea(pos.get());
    stf(p + 0, gabi::load<f32>(t + 0x18));
    stf(p + 4, gabi::load<f32>(t + 0x1C));
    stf(p + 8, gabi::load<f32>(t + 0x20));
    cLib_offsetPos_l(p, p, (s32)(s16)ld16(t + 0x16), l_offsetPos);
    s32 room = (s8)ld8(t + 0);
    s32 angle = (s16)ld16(t + 0x16);
    dSv_restart_c_setRestartOption4(t, option, p, (u32)angle, (u32)room);
}
VERIFY(0x025B9864, dSv_restart_c_setRestartOption);

/* 025B9918: HD 0x50 bytes */
static void dSv_reserve_c_init(u32 t) {
    WWHD_FUNC(0x025B9918, void, t);
    for (u32 i = 0; i < 0x50; i++) st8(t + i, 0);
}
VERIFY(0x025B9918, dSv_reserve_c_init);

/* 025B9938: HD 16 memories */
static void dSv_save_c_init(u32 t) {
    WWHD_FUNC(0x025B9938, void, t);
    gabi::call(0x025B8904, t); /* dSv_player_c::init */
    for (u32 i = 0; i < 0x10; i++) dSv_memory_c_init(t + 0x380 + 0x24 * i);
    gabi::call(0x025B8998, t + 0x5C0); /* dSv_ocean_c::init */
    gabi::call(0x025B8B10, t + 0x624); /* dSv_event_c::init */
}
VERIFY(0x025B9938, dSv_save_c_init);

/* 025B998C */
static void dSv_turnRestart_c_set(u32 t, u32 pos, u32 angle, u32 roomNo, u32 param, u32 shipPos, u32 shipAngle,
                                  u32 hasShip) {
    WWHD_FUNC(0x025B998C, void, t, pos, angle, roomNo, param, shipPos, shipAngle, hasShip);
    st(t + 0, ld(pos + 0));
    st(t + 4, ld(pos + 4));
    u32 z = ld(pos + 8);
    st(t + 0xC, param);
    st(t + 8, z);
    st8(t + 0x12, (u8)roomNo);
    st16(t + 0x10, (u16)angle);
    st8(t + 0x13, 0);
    st(t + 0x24, ld(shipPos + 0));
    st(t + 0x28, ld(shipPos + 4));
    u32 sz = ld(shipPos + 8);
    st(t + 0x34, hasShip);
    st(t + 0x2C, sz);
    st16(t + 0x30, (u16)shipAngle);
}
VERIFY(0x025B998C, dSv_turnRestart_c_set);

}  // namespace d_save_4_cpp
