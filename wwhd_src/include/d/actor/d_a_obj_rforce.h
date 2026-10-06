#pragma once
#include "f_op/f_op_actor.h"
// Stationary rotating-force platform, HD profile size 0x3EC.
namespace daObjRforce {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class phase;
  be<u32> model;
  be<u32> background;
  Mtx34 matrix;
};
} // namespace daObjRforce
WWHD_SIZE(daObjRforce::Act_c, 0x3EC);
WWHD_OFFSET(daObjRforce::Act_c, phase, 0x3AC);
WWHD_OFFSET(daObjRforce::Act_c, model, 0x3B4);
WWHD_OFFSET(daObjRforce::Act_c, background, 0x3B8);
WWHD_OFFSET(daObjRforce::Act_c, matrix, 0x3BC);
