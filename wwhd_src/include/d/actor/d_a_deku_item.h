#pragma once
#include "f_op/f_op_actor.h"
struct daDekuItem_c : fopAc_ac_c {
  request_of_phase_process_class phase;
  gptr<void> model;
  u8 bck1[0x8c], bck2[0x8c], acch[0x1c4], acchCir[0x40], status[0x3c],
      cylinder[0x130];
  be<s32> mode, eventPending, itemBit;
  be<u32> itemPID;
  gptr<void> emitter;
};
WWHD_OFFSET(daDekuItem_c, model, 0x3b4);
WWHD_OFFSET(daDekuItem_c, mode, 0x840);
WWHD_OFFSET(daDekuItem_c, emitter, 0x850);
WWHD_SIZE(daDekuItem_c, 0x854);
