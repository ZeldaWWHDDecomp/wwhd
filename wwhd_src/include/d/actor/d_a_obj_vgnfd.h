#pragma once
#include "f_op/f_op_actor.h"
// HD layout, audited from 023B1124..023B2290. Derived code: private only.
struct daObjVgnfd_c : fopAc_ac_c {
  gptr<void> model[5];
  gptr<void> door[2];
  u8 btk[0x74];
  request_of_phase_process_class phase;
  u8 brk[5][0x78];
  u8 smoke[0x20];
  be<s32> staff;
  be<s32> timer;
  be<s32> demo;
  be<s32> currentModel;
  be<s16> event;
  be<u8> state;
  be<u8> initialized;
  gptr<void> bgw;
};
WWHD_OFFSET(daObjVgnfd_c, model, 0x3ac);
WWHD_OFFSET(daObjVgnfd_c, door, 0x3c0);
WWHD_OFFSET(daObjVgnfd_c, btk, 0x3c8);
WWHD_OFFSET(daObjVgnfd_c, phase, 0x43c);
WWHD_OFFSET(daObjVgnfd_c, brk, 0x444);
WWHD_OFFSET(daObjVgnfd_c, smoke, 0x69c);
WWHD_OFFSET(daObjVgnfd_c, staff, 0x6bc);
WWHD_OFFSET(daObjVgnfd_c, state, 0x6ce);
WWHD_OFFSET(daObjVgnfd_c, bgw, 0x6d0);
WWHD_SIZE(daObjVgnfd_c, 0x6d4);
