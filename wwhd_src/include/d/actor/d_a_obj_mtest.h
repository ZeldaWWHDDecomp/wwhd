#pragma once
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
namespace daObjMtest {
struct Act_c : fopAc_ac_c {
  be<u32> background;
  u8 moveBgState[0x30];
  Mtx34 matrix;
  request_of_phase_process_class phase;
  be<u32> model;
  dCcD_Stts status;
  dCcD_Cyl cylinder;
  be<s32> type;
  be<u8> appear;
  u8 padding[3];
};
} // namespace daObjMtest
WWHD_SIZE(daObjMtest::Act_c, 0x590);
WWHD_OFFSET(daObjMtest::Act_c, background, 0x3AC);
WWHD_OFFSET(daObjMtest::Act_c, matrix, 0x3E0);
WWHD_OFFSET(daObjMtest::Act_c, phase, 0x410);
WWHD_OFFSET(daObjMtest::Act_c, model, 0x418);
WWHD_OFFSET(daObjMtest::Act_c, status, 0x41C);
WWHD_OFFSET(daObjMtest::Act_c, cylinder, 0x458);
WWHD_OFFSET(daObjMtest::Act_c, type, 0x588);
WWHD_OFFSET(daObjMtest::Act_c, appear, 0x58C);
