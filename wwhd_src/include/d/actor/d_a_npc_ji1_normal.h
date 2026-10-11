/* Orca normal/spin-action private HD views. Actor layout remains in
 * d_a_npc_ji1.h. */
#pragma once
#include "d/actor/d_a_npc_ji1.h"
#include "gc_api/f_op/f_op_actor.h"
struct Ji1NormalPlayerVtableHD {
  u8 _0[0x5C];
  be<u32> mCheckFrontRoll;
};
WWHD_OFFSET(Ji1NormalPlayerVtableHD, mCheckFrontRoll, 0x5C);
WWHD_SIZE(Ji1NormalPlayerVtableHD, 0x60);
struct Ji1NormalPlayerHD : fopAc_ac_c {
  u8 _3AC[0x14];
  be<u32> mModeFlg2;
  BOOL checkFrontRoll() {
    const auto *table = gabi::at<Ji1NormalPlayerVtableHD>(__vtbl);
    return gabi::call_ptr<BOOL>(table->mCheckFrontRoll, this);
  }
  bool checkFrontRollCrash() const { return (u32(mModeFlg2) & 0x2000) != 0; }
};
WWHD_OFFSET(Ji1NormalPlayerHD, mModeFlg2, 0x3C0);
WWHD_SIZE(Ji1NormalPlayerHD, 0x3C4);
// HD: the message manager replaces the GameCube msg_class object.
struct Ji1NormalMessageHD;
using Ji1NormalMessagePointerHD = gptr<Ji1NormalMessageHD>;
using Ji1NormalMessageIdHD = be<u32>;
enum Ji1NormalAnimation_e {
  Ji1Normal_WAIT01 = 0,
  Ji1Normal_WAIT02 = 1,
  Ji1Normal_TALK01 = 2,
  Ji1Normal_TALK02 = 3,
  Ji1Normal_AKIRE = 4,
  Ji1Normal_KAMAE = 5,
  Ji1Normal_YKGUARD = 9,
  Ji1Normal_TOKAMAE = 11,
  Ji1Normal_WARAI = 13,
  Ji1Normal_GUARD = 15,
  Ji1Normal_BTKAMASI = 16,
  Ji1Normal_BTKAMAE = 17,
  Ji1Normal_UDEGUMI = 18
};
enum Ji1NormalEvent_e {
  Ji1Normal_EVENT_SPIN = 5,
  Ji1Normal_EVENT_SPIN_EXPLAIN = 6,
  Ji1Normal_CUT_WAIT = 9,
  Ji1Normal_CUT_GUARD_THREE = 12,
  Ji1Normal_CUT_GUARD_FIVE = 13,
  Ji1Normal_CUT_GUARD_MANY = 14,
  Ji1Normal_CUT_HEADLOCK = 1,
  Ji1Normal_CUT_TALK = 2,
  Ji1Normal_CUT_SEND = 3,
  Ji1Normal_CUT_FACE_PLAYER = 4
};
enum Ji1NormalMessageNo_e {
  Ji1Normal_MSG_OPENING = 0x967,
  Ji1Normal_MSG_SPIN_START = 0x974,
  Ji1Normal_MSG_SPIN_END = 0x975,
  Ji1Normal_MSG_SPIN_WAIT = 0x976
};
namespace dPa_name {
enum { Ji1Normal_ID_AK_JN_NG = 0xC };
}
namespace Ji1NormalEventFlag {
enum { UNK_0520 = 0x0520, UNK_0501 = 0x0501 };
}
