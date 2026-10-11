/* Orca spin lesson: source-private player view and shared GameCube APIs. */
#pragma once
// HD: older Orca companions still need the common header's legacy event
// inlines. Give those unused inlines private names here, so this source can use
// the shared d_com_inf_game accessors.
#define dComIfGs_isEventBit ji1TeachspLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1TeachspLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1TeachspLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1TeachspLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct Ji1TeachspPlayerVtableHD {
  u8 _0[0x94];
  be<u32> mCheckComboCutTurn;
};
WWHD_OFFSET(Ji1TeachspPlayerVtableHD, mCheckComboCutTurn, 0x94);
WWHD_SIZE(Ji1TeachspPlayerVtableHD, 0x98);
struct Ji1TeachspPlayerHD : fopAc_ac_c {
  /* 0x3AC */ be<u8> mCutType;
  u8 _3AD[0xB];
  /* 0x3B8 */ be<u32> mNoResetFlg0;
  u8 getCutType() const { return mCutType; }
  u32 getCutAtFlg() const { return u32(mNoResetFlg0) & 0x40; }
  BOOL checkComboCutTurn() {
    const auto *table = gabi::at<Ji1TeachspPlayerVtableHD>(__vtbl);
    return gabi::call_ptr<BOOL>(table->mCheckComboCutTurn, this);
  }
};
WWHD_OFFSET(Ji1TeachspPlayerHD, mCutType, 0x3AC);
WWHD_OFFSET(Ji1TeachspPlayerHD, mNoResetFlg0, 0x3B8);
WWHD_SIZE(Ji1TeachspPlayerHD, 0x3BC);
using Ji1TeachspGuardHD = be<u32>;
enum Ji1TeachspAnimation_e {
  Ji1Teachsp_KAMAE = 5,
  Ji1Teachsp_KROT = 6,
  Ji1Teachsp_TATEGUARD = 8,
  Ji1Teachsp_YKGUARD = 9,
  Ji1Teachsp_JPGUARD = 10
};
enum Ji1TeachspCut_e {
  Ji1Teachsp_CUT_NEW_RECORD = 7,
  Ji1Teachsp_CUT_HIT_LIMIT = 8,
  Ji1Teachsp_CUT_LEAVE_ROOM = 9,
  Ji1Teachsp_CUT_SPIN_SUCCESS = 10,
  Ji1Teachsp_CUT_TURN = 11
};
enum { Ji1Teachsp_KNIGHTS_CREST = 3, Ji1Teachsp_EVENT_BAD_CUT = 3 };
namespace Ji1TeachspEventFlag {
enum { UNK_0B20 = 0x0B20, UNK_0F20 = 0x0F20, UNK_0F10 = 0x0F10 };
}
namespace Ji1TeachspTmpFlag {
enum { UNK_0402 = 0x0402 };
}
