#pragma once
#include "d/actor/d_a_npc_ji1.h"
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/f_op/f_op_actor.h"

void mDoAud_changeBgmStatus(u32);

// HD: checkCutCharge is dispatched through slot 0x6C of the actor vtable.
// Keep its full BOOL return register; no narrowing to a host bool.
struct Ji1PlayerVtable {
    u8 _00[0x6C];
    be<u32> checkCutCharge;
};
WWHD_OFFSET(Ji1PlayerVtable, checkCutCharge, 0x6C);
// HD: use guest endian fields even for the byte; the legacy player layout's
// native u8 member cannot be dereferenced through a guest virtual address.
struct Ji1Player : fopAc_ac_c {
    be<u8> mCutType;
    u8 _3AD[0x3B8 - 0x3AD];
    be<u32> mNoResetFlg0;
    u8 getCutType() { return mCutType; }
    u32 getCutAtFlg() { return mNoResetFlg0 & 0x40; }
};
WWHD_OFFSET(Ji1Player, mCutType, 0x3AC);
WWHD_OFFSET(Ji1Player, mNoResetFlg0, 0x3B8);
static BOOL Ji1_checkCutCharge(Ji1Player* player) {
    return gabi::call_ptr<BOOL>(gabi::at<Ji1PlayerVtable>(player->__vtbl)->checkCutCharge, player);
}

// GameCube daPy_py_c::daPy_CUT_TYPE; the shared layout does not yet expose these names.
enum Ji1CutType {
    CUT_TYPE_CUT_A = 1,
    CUT_TYPE_CUT_EA = 6,
    CUT_TYPE_CUT_TURN = 8,
    CUT_TYPE_CUT_ROLL = 9,
    CUT_TYPE_JUMPCUT_SWORD = 10,
    CUT_TYPE_BT_ROLLCUT = 15,
};
