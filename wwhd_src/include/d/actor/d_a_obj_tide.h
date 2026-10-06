#pragma once
#include "f_op/f_op_actor.h"
namespace daObjTide {
struct Act_c : fopAc_ac_c {
  be<u32> background;
  u8 moveBGReserved[0x30];
  request_of_phase_process_class phase;
  be<u32> model, bck, brk, btk, secondModel, secondBrk;
  be<s32> type, mode;
  be<f32> step;
  be<s16> timer;
  be<u8> stage, registered;
  be<u8> gurgle, outflow, upward, finished;
  be<s32> gurgleId, outflowId, upwardId;
  be<u8> latched, ready, deleteRequested, hasDungeonItem;
  be<s32> countdown;
};
struct Archive {
  be<u32> name, vtable;
};
} // namespace daObjTide
WWHD_SIZE(daObjTide::Act_c, 0x428);
WWHD_OFFSET(daObjTide::Act_c, background, 0x3AC);
WWHD_OFFSET(daObjTide::Act_c, phase, 0x3E0);
WWHD_OFFSET(daObjTide::Act_c, model, 0x3E8);
WWHD_OFFSET(daObjTide::Act_c, bck, 0x3EC);
WWHD_OFFSET(daObjTide::Act_c, brk, 0x3F0);
WWHD_OFFSET(daObjTide::Act_c, btk, 0x3F4);
WWHD_OFFSET(daObjTide::Act_c, secondModel, 0x3F8);
WWHD_OFFSET(daObjTide::Act_c, secondBrk, 0x3FC);
WWHD_OFFSET(daObjTide::Act_c, type, 0x400);
WWHD_OFFSET(daObjTide::Act_c, mode, 0x404);
WWHD_OFFSET(daObjTide::Act_c, step, 0x408);
WWHD_OFFSET(daObjTide::Act_c, timer, 0x40C);
WWHD_OFFSET(daObjTide::Act_c, stage, 0x40E);
WWHD_OFFSET(daObjTide::Act_c, registered, 0x40F);
WWHD_OFFSET(daObjTide::Act_c, gurgle, 0x410);
WWHD_OFFSET(daObjTide::Act_c, gurgleId, 0x414);
WWHD_OFFSET(daObjTide::Act_c, latched, 0x420);
WWHD_OFFSET(daObjTide::Act_c, countdown, 0x424);
WWHD_SIZE(daObjTide::Archive, 8);
