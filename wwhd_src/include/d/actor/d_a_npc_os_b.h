/* Os part B private declarations.
 * The actor uses the shared GameCube field names at their measured HD offsets.
 * HD: the HIO child starts four bytes earlier and both HIO vtables trail their fields;
 * compact member-function descriptors remain eight bytes. No shared actor declarations change.
 */
#pragma once
#include "d/actor/d_a_npc_os_local.h"
namespace os_b {
// HD: HIO vtables trail their fields; the child starts at offset four.
struct daNpc_Os_HIO2_c {
    be<f32> field_0x04, mWalkSpeed, field_0x0C, field_0x10, field_0x14, field_0x18, field_0x1C, mPlayerWalkRate;  /* mWalkSpeed was field_0x08: walkProc 022AD668: speedF = it * the rate while the morf frame is inside (mWalkFrameStart, mWalkFrameEnd); walkPlayerAction 022AFF94 the same with the stick value (tww the same); mPlayerWalkRate was field_0x20: walkPlayerAction 022AFF94: speedF = mWalkSpeed * the stick value * it (tww the same) */
    be<s16> mTurnMaxStep, mTurnMinStep, mTurnScale;  /* mTurnMaxStep was field_0x24: walkProc 022AD668 / walkPlayerAction 022AFF94: cLib_addCalcAngleS's maxStep (doubled for shape_angle in actionMoveEvent 022AD720, searchNpcAction 022AF8D4, walkPlayerAction) (tww the same; c_lib.h argument order); mTurnMinStep was field_0x26: walkProc 022AD668 / walkPlayerAction 022AFF94: cLib_addCalcAngleS's minStep (doubled for shape_angle, as mTurnMaxStep) (tww the same); mTurnScale was field_0x28: walkProc 022AD668 / walkPlayerAction 022AFF94 / actionMoveEvent 022AD720 / searchNpcAction 022AF8D4: cLib_addCalcAngleS's scale (tww the same) */
    u8 _pad[2];
    be<u32> vtable;
};
WWHD_SIZE(daNpc_Os_HIO2_c, 0x2C);
struct daNpc_Os_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 _pad[3];
    /* 0x04 */ daNpc_Os_HIO2_c mOs2;
    /* 0x30 */ dNpc_HIO_l mNpc;
    /* 0x58 */ gptr<daNpc_Os_c> field_0x5C;
    /* 0x5C */ be<f32> mFollowStopDist;  /* was field_0x60: waitNpcAction 022AE504: once called, the statue starts searching while the player is farther than it; searchNpcAction 022AF8D4 walks at rate 0 inside it, else mFollowWalkRate (tww the same) */
    /* 0x60 */ be<f32> mCallDist;  /* was field_0x64: waitNpcAction 022AE504: while the player is closer than it, they may call the statue (onNpcCall) (tww the same) */
    /* 0x64 */ be<f32> field_0x68;
    /* 0x68 */ be<f32> field_0x6C;
    /* 0x6C */ be<f32> field_0x70;
    /* 0x70 */ be<f32> field_0x74;
    /* 0x74 */ be<f32> field_0x78;
    /* 0x78 */ be<f32> field_0x7C;
    /* 0x7C */ be<f32> mThrowSpeedF;  /* was field_0x80: throwNpcAction 022AEF70: speedF when thrown (tww the same) */
    /* 0x80 */ be<f32> mThrowSpeedY;  /* was field_0x84: throwNpcAction 022AEF70: speed.y when thrown (tww the same) */
    /* 0x84 */ be<f32> field_0x88;
    /* 0x88 */ be<f32> mGravity;  /* was field_0x8C: init 022AA9A4, waitNpcAction 022AE504, finish01NpcAction 022AE834, finish02NpcAction 022AE980, throwNpcAction 022AEF70, jumpNpcAction 022AFBF4, initialWaitEvent 022AD36C, initialWakeupEvent 022AD598: gravity = it (tww the same) */
    /* 0x8C */ be<f32> mCarryOffsetX;  /* was field_0x90: carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.x toward it (tww the same) */
    /* 0x90 */ be<f32> mCarryOffsetY;  /* was field_0x94: carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.y toward it (tww the same) */
    /* 0x94 */ be<f32> mCarryOffsetZ;  /* was field_0x98: carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.z toward it (tww the same) */
    /* 0x98 */ be<f32> mStickTurnMin;  /* was field_0x9C: waitPlayerAction 022AFCE0: turns toward the stick while the stick value is at least it (or locked on) (tww the same) */
    /* 0x9C */ be<f32> mStickWalkMin;  /* was field_0xA0: waitPlayerAction 022AFCE0: switches to walkPlayerAction once the stick value is at least it (below it the statue only looks) (tww the same) */
    /* 0xA0 */ be<f32> field_0xA4;
    /* 0xA4 */ be<f32> mFollowWalkRate;  /* was field_0xA8: searchNpcAction 022AF8D4: the walkProc rate while the player is farther than mFollowStopDist (tww the same) */
    /* 0xA8 */ be<f32> mWalkFrameEnd;  /* was field_0xAC: walkProc 022AD668 / walkPlayerAction 022AFF94: the statue moves only while the morf frame is below it (and above mWalkFrameStart) (tww the same) */
    /* 0xAC */ be<f32> mWalkFrameStart;  /* was field_0xB0: walkProc 022AD668 / walkPlayerAction 022AFF94: the statue moves only while the morf frame is above it (and below mWalkFrameEnd) (tww the same) */
    be<u32> vtable;
};
WWHD_SIZE(daNpc_Os_HIO_c, 0xB4);
WWHD_OFFSET(daNpc_Os_HIO_c, mNo, 0x00);
WWHD_OFFSET(daNpc_Os_HIO_c, mOs2, 0x04);
WWHD_OFFSET(daNpc_Os_HIO_c, mNpc, 0x30);
WWHD_OFFSET(daNpc_Os_HIO_c, field_0x5C, 0x58);
WWHD_OFFSET(daNpc_Os_HIO_c, mGravity, 0x88);
WWHD_OFFSET(daNpc_Os_HIO_c, mWalkFrameEnd, 0xA8);
WWHD_OFFSET(daNpc_Os_HIO_c, mWalkFrameStart, 0xAC);
WWHD_OFFSET(daNpc_Os_HIO2_c, mWalkSpeed, 0x04);
WWHD_OFFSET(daNpc_Os_HIO2_c, mTurnMaxStep, 0x20);
WWHD_OFFSET(daNpc_Os_HIO2_c, mTurnMinStep, 0x22);
WWHD_OFFSET(daNpc_Os_HIO2_c, mTurnScale, 0x24);
enum : u32 { STATUS_GRAVITY = 8, NPC_NOT_CHANGE = 0x40, JA_SE_OBJ_OSTATUE_PUT = 0x58FE };
enum MessageStatus : u16 {
    MSG_WAIT_e = 6,
    MSG_DISPLAYED_e = 0xE,
    MSG_CONTINUES_e = 0xF,
    MSG_ENDS_e = 0x10,
    MSG_BOX_CLOSED_e = 0x12,
    MSG_DESTROYED_e = 0x13,
    MSG_OTHER_e = 0x15
};
struct BrkTableEntry {
    be<u8> mAnmPrmIdx;  /* was unk_1: setAnm_brkAnm 022ADA3C passes it to setAnm (the l_anmPrm index); static anmBrkTbl, never written (tww setAnm_brkAnm the same) */
    be<u8> mBrkAnmIdx;  /* was unk_2: setAnm_brkAnm 022ADA3C passes it to initBrkAnm (the brkAnmTbl index); static anmBrkTbl, never written (tww setAnm_brkAnm the same) */
};
WWHD_SIZE(BrkTableEntry, 2);
struct PlayerRooms {
    be<u8> rooms[256];
};
WWHD_SIZE(PlayerRooms, 256);
struct SinCosTable {
    struct Entry {
        be<f32> sin, cos;
    };
    Entry m[8192];
};
WWHD_SIZE(SinCosTable, 0x10000);
struct Smoke {
    be<u32> vtable;
};
WWHD_SIZE(Smoke, 4);
struct Attention {
    u8 _pad[0x20];
    be<u32> flags;
};
WWHD_SIZE(Attention, 0x24);
WWHD_OFFSET(Attention, flags, 0x20);
struct Save {
    u8 _pad0[0x20];
    u8 savedata[0x1AC];
    u8 priest[0x47C];
    u8 event[0xB00];
    u8 restart[1];
};
WWHD_SIZE(Save, 0x1149);
WWHD_OFFSET(Save, savedata, 0x20);
WWHD_OFFSET(Save, restart, 0x1148);
WWHD_OFFSET(Save, priest, 0x1CC);
// HD: byte table indices wrap in the 32-bit guest address space, including invalid signed inputs.
inline BrkTableEntry* brkEntry(BrkTableEntry* table, s32 index) {
    return gabi::at<BrkTableEntry>(gabi::ea(table) + u32(index) * 2);
}
inline void offPlayerRoom(PlayerRooms* rooms, s8 index) {
    gabi::store<u8>(gabi::ea(rooms) + u32(s32(index)), 0);
}
inline s16 lowHalf(void* data) {
    return gabi::load<s16>(gabi::ea(data) + 2);
}
inline Save* save() {
    return gabi::at<Save>(gabi::load<u32>(0x101F84DC));
}
inline fopAc_ac_c* partner() {
    return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mpPlayerPtr[1];
}
inline void* camera() {
    return gabi::at<void>(gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0].mpCamera);
}
inline void onEventBit(u16 flag) {
    gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->onEventBit(flag);
}
inline Attention* attention() {
    return gabi::at<Attention>(dComIfGp_ea() + PLAY_OFF(mAttention));
}
inline void removeSmoke(Smoke* smoke) {
    gabi::call_ptr(gabi::load<u32>(u32(smoke->vtable) + 0x44), gabi::ea(smoke));
}
} // namespace os_b
