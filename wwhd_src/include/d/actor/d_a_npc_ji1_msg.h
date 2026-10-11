/* Orca message actions: private HD message-manager view; common actor layout
 * unchanged. */
#pragma once
// HD: unused legacy event inlines stay private while this source uses the shared d_com_inf_game accessors.
#define dComIfGs_isEventBit ji1MsgLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1MsgLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1MsgLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1MsgLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct Ji1MsgManagerHD {
  u8 _0[0x948];
  be<s32> mSelectNum;
};
WWHD_OFFSET(Ji1MsgManagerHD, mSelectNum, 0x948);
WWHD_SIZE(Ji1MsgManagerHD, 0x94C);
using Ji1MsgManagerPointerHD = gptr<Ji1MsgManagerHD>;
using Ji1MsgIdHD = be<u32>;
using Ji1MsgReplyFlagHD = be<u8>;
void ji1MsgSwingVerticalHD(void *, s16, s16, s16, s32);
void ji1MsgSwingHorizoneHD(void *, s16, s16, s16, s32);
struct Ji1MsgHeadAnmHD {
  u8 _0[0x24];
  void swing_vertical_init(s16 a, s16 b, s16 c, s32 d) {
    ji1MsgSwingVerticalHD(this, a, b, c, d);
  }
  void swing_horizone_init(s16 a, s16 b, s16 c, s32 d) {
    ji1MsgSwingHorizoneHD(this, a, b, c, d);
  }
};
WWHD_SIZE(Ji1MsgHeadAnmHD, 0x24);
enum Ji1MsgAnimation_e {
  Ji1Msg_KAMAE = 5,
  Ji1Msg_INASI = 7,
  Ji1Msg_BIBIRI = 22
};
enum Ji1MsgEvent_e {
  Ji1Msg_EVENT_REPLY = 1,
  Ji1Msg_EVENT_BAD_CUT = 3,
  Ji1Msg_EVENT_SPIN = 6
};
enum { Ji1Msg_KNIGHTS_CREST = 3 };
namespace Ji1MsgEventFlag {
enum {
  UNK_0001 = 0x0001,
  UNK_0002 = 0x0002,
  UNK_0108 = 0x0108,
  UNK_0501 = 0x0501,
  UNK_0640 = 0x0640,
  UNK_0B20 = 0x0B20,
  UNK_0D80 = 0x0D80,
  UNK_0F10 = 0x0F10,
  UNK_0F20 = 0x0F20,
  UNK_2F10 = 0x2F10,
  UNK_2F40 = 0x2F40,
  UNK_CF03 = 0xCF03,
  UNK_D003 = 0xD003
};
}
