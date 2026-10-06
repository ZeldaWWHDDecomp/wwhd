#pragma once
#include "f_op/f_op_actor.h"
struct daObjTower_c : fopAc_ac_c {
  /* 3AC */ request_of_phase_process_class phase;
  /* 3B4 */ be<u32> towerModel;
  /* 3B8 */ be<u32> background;
  /* 3BC */ Mtx34 backgroundMatrix;
  /* 3EC */ be<u8> backgroundRegistered;
  /* 3ED */ u8 reserved[3];
};
WWHD_SIZE(daObjTower_c, 0x3F0);
WWHD_OFFSET(daObjTower_c, backgroundMatrix, 0x3BC);
WWHD_OFFSET(daObjTower_c, backgroundRegistered, 0x3EC);
