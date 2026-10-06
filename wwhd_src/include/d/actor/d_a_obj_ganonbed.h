#pragma once
#include "f_op/f_op_actor.h"
// Puppet Ganon's bed: HD actor base followed by the phase and two heap objects.
struct daObjGbed_c : fopAc_ac_c {
  request_of_phase_process_class phase;
  be<u32> model;
  be<u32> background;
};
WWHD_SIZE(daObjGbed_c, 0x3BC);
WWHD_OFFSET(daObjGbed_c, phase, 0x3AC);
WWHD_OFFSET(daObjGbed_c, model, 0x3B4);
WWHD_OFFSET(daObjGbed_c, background, 0x3B8);
