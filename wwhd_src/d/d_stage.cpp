/* d_stage: stage/room data (dStage_*), part 1 of 5, WWHD. 
 *
 * Translation unit 025C1004..025C527C, from the image: d_spline_path ends with its __sinit
 * (025C0F70); d_stage starts with createRoomScene (025C1004); its __sinit is 025C4684 (it
 * constructs the 64 room statuses at 1047E6CC with 025C415C); the out-of-line copies of the
 * dStage_roomDt_c / dStage_stageDt_c virtuals (025C4734..025C51BC) and an HD debug dump
 * (025C51C0) follow it; d_throwstone starts at 025C527C. The string/rodata block of the unit is
 * 10055330..10055E50 (vtables: dStage_roomDt_c 10055B90, dStage_stageDt_c 10055DE0).
 * Parts: d_stage.cpp 025C1004..025C1384 (small helpers), d_stage_2.cpp 025C1384..025C1E44
 * (dStage_playerInit and the first chunk handlers), d_stage_3.cpp 025C1E44..025C2BC0 (chunk
 * handlers), d_stage_4.cpp 025C2BC0..025C4734 (loaders, create/delete, scene change, room
 * control, constructors, __sinit), d_stage_5.cpp 025C4734..025C527C (virtual set/get copies).
 *
 * HD: dStage_roomControl_c's statics: mStayNo 1047E6C8 (s8), mOldStayNo 1047E6C9,
 * mStatus[64] at 1047E6CC (stride 0x22C, mRoomDt at +0). The stage data object is embedded in
 * the play object at +0x5150 (vtable pointer first); the room control at +0x51CC. */
#include "bindings.h"

namespace d_stage_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline u32 JKRAlloc_l(u32 size, s32 align) { return gabi::call<u32>(0x027EC0AC, size, align); }
static inline void JKRFree_l(u32 p) { gabi::call(0x027EC0B0, p); }
static inline u32 fopScnM_CreateReq_l(s32 proc, s32 prio, u32 a2, u32 data) { return gabi::call<u32>(0x025DC91C, proc, prio, a2, data); }
static inline u32 fpcLy_CurrentLayer_l() { return gabi::call<u32>(0x025DED64); }
static inline u32 fpcSCtRq_Request_l(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm) { return gabi::call<u32>(0x025E14A8, layer, proc, a2, a3, prm); }
static inline void strcpy_l(u32 dst, u32 src) { gabi::call(0x028F040C, dst, src); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline u32 vcall(u32 obj, u32 off) { return gabi::call_ptr<u32>(ld(ld(obj) + off), obj); }

static constexpr u32 mStayNo = 0x1047E6C8;
static constexpr u32 mOldStayNo = 0x1047E6C9;
static constexpr u32 mStatus = 0x1047E6CC;
static constexpr u32 l_objectName = 0x101EBFDC; /* 809 entries of 0xC: name[8], procname s16 +8, argument s8 +0xA, gbaName +0xB */

/* 025C1004 */
static s32 createRoomScene(s32 param_0) {
    WWHD_FUNC(0x025C1004, s32, param_0);
    u32 ptr = JKRAlloc_l(4, -4);
    if (ptr == 0) return 0;
    st(ptr, (u32)param_0);
    return (s32)fopScnM_CreateReq_l(0x12, 0x7FFF, 0, ptr);
}
VERIFY(0x025C1004, createRoomScene);

/* 025C1070: dStage_roomControl_c::setStayNo(int), HD: static (no this); the 0xFF test of the
 * GameCube source (always false) is gone */
static void dStage_roomControl_c_setStayNo(s32 i_roomNo) {
    WWHD_FUNC(0x025C1070, void, i_roomNo);
    s8 no = (s8)i_roomNo;
    u8 old = ld8(mStayNo);
    st8(mStayNo, (u8)no);
    st8(mOldStayNo, old);
    st8(mStatus + no * 0x22C + 0x21D, 1); /* onStatusDraw */
}
VERIFY(0x025C1070, dStage_roomControl_c_setStayNo);

/* 025C109C */
static u32 dStage_searchName(u32 i_name) {
    WWHD_FUNC(0x025C109C, u32, i_name);
    u32 obj = l_objectName;
    for (u32 i = 0x329; i != 0; i--) {
        u32 a = obj, b = i_name;
        u8 ca, cb;
        for (;;) {
            ca = ld8(a);
            cb = ld8(b);
            if (ca != cb || ca == 0) break;
            a++;
            b++;
        }
        if (ca == cb) return obj;
        obj += 0xC;
    }
    return 0;
}
VERIFY(0x025C109C, dStage_searchName);

/* 025C10E8: HD-changed: an argument of -1 (in the table or the request) matches any, and when
 * only the process name matches, the last such entry is returned instead of "?". The registers
 * are compared whole (the callers pass them sign-extended). */
static u32 dStage_getName(u32 i_procName, u32 i_argument) {
    WWHD_FUNC(0x025C10E8, u32, i_procName, i_argument);
    u32 last = 0;
    u32 obj = l_objectName;
    for (u32 i = 0x329; i != 0; i--) {
        if ((u32)(s32)(s16)ld16(obj + 8) == i_procName) {
            s32 arg = (s8)ld8(obj + 0xA);
            if ((u32)arg == i_argument) return obj;
            if (arg == -1) return obj;
            if ((s32)i_argument == -1) return obj;
            last = obj;
        }
        obj += 0xC;
    }
    if (last == 0) return 0x100558E8; /* "誰？" */
    return last;
}
VERIFY(0x025C10E8, dStage_getName);

/* 025C1150 */
static u32 dStage_getName2(u32 i_procName, u32 i_argument) {
    WWHD_FUNC(0x025C1150, u32, i_procName, i_argument);
    return dStage_getName(i_procName, i_argument);
}
VERIFY(0x025C1150, dStage_getName2);

/* 025C1154: fopAcM_Create inlined (fpcSCtRq_Request in the current layer) */
static void dStage_actorCreate(u32 i_actorData, u32 i_actorPrm) {
    WWHD_FUNC(0x025C1154, void, i_actorData, i_actorPrm);
    u32 nameinf = dStage_searchName(i_actorData);
    if (nameinf == 0) {
        JKRFree_l(i_actorPrm);
        return;
    }
    st8(i_actorPrm + 0x20, ld8(nameinf + 0xA));
    st8(i_actorPrm + 0x1B, ld8(nameinf + 0xB));
    s32 proc = (s16)ld16(nameinf + 8);
    u32 layer = fpcLy_CurrentLayer_l();
    fpcSCtRq_Request_l(layer, proc, 0, 0, i_actorPrm);
}
VERIFY(0x025C1154, dStage_actorCreate);

/* 025C11DC: HD: an unsigned bound check with an assert */
static u32 dStage_roomControl_c_getStatusRoomDt(u32 self, s32 i_statusIdx) {
    WWHD_FUNC(0x025C11DC, u32, self, i_statusIdx);
    if ((u32)i_statusIdx >= 0x40) {
        JUT_ASSERT_l(0x100558F4, 0x19E, 0x100558F0);
        return 0;
    }
    return mStatus + i_statusIdx * 0x22C;
}
VERIFY(0x025C11DC, dStage_roomControl_c_getStatusRoomDt);

/* 025C123C */
static BOOL dStage_chkPlayerId(s32 playerId, s32 room_no) {
    WWHD_FUNC(0x025C123C, BOOL, playerId, room_no);
    u32 player;
    if (room_no == -1) {
        u32 p = dComIfGp_ea();
        player = vcall(p + 0x5150, 0x44); /* getPlayer */
    } else {
        if ((u32)room_no >= 0x40) JUT_ASSERT_l(0x10055900, 0x6F6, 0x1005590C);
        u32 p = dComIfGp_ea();
        u32 rd = dStage_roomControl_c_getStatusRoomDt(p + 0x51CC, room_no);
        player = vcall(rd, 0x44);
    }
    if (player == 0) return FALSE;
    s32 num = (s32)ld(player);
    u32 actor = ld(player + 4);
    if (num <= 0) return FALSE;
    for (; num != 0; num--) {
        if ((u32)ld8(actor + 0x1D) == (u32)playerId) return TRUE;
        actor += 0x20;
    }
    return FALSE;
}
VERIFY(0x025C123C, dStage_chkPlayerId);

/* 025C1328: dStage_startStage_c::set(const char*, s8, s16, s8) */
static void dStage_startStage_c_set(u32 self, u32 i_name, s32 i_roomNo, s32 i_point, s32 i_layer) {
    WWHD_FUNC(0x025C1328, void, self, i_name, i_roomNo, i_point, i_layer);
    strcpy_l(self, i_name);
    st8(self + 0xB, (u8)i_layer);
    st8(self + 0xA, (u8)i_roomNo);
    st16(self + 8, (u16)i_point);
}
VERIFY(0x025C1328, dStage_startStage_c_set);

} // namespace d_stage_cpp
