#pragma once
#include "f_op/f_op_actor.h"
struct LpalmQuaternion {
  be<f32> x, y, z, w;
};
WWHD_SIZE(LpalmQuaternion, 0x10);
struct daObjLpalm_c : fopAc_ac_c {
  /* 3AC */ u8 reserved[4];
  /* 3B0 */ LpalmQuaternion baseRotation;
  /* 3C0 */ LpalmQuaternion targetRotation;
  /* 3D0 */ LpalmQuaternion leafRotation[2];
  /* 3F0 */ be<s16> bendAngle[2];
  /* 3F4 */ be<s16> waveAngle[2];
  /* 3F8 */ request_of_phase_process_class phase;
  /* 400 */ be<u32> palmModel;
  /* 404 */ be<u32> background;
  /* 408 */ Mtx34 backgroundMatrix; // HD separate narrower collision transform.
};
WWHD_SIZE(daObjLpalm_c, 0x438);
WWHD_OFFSET(daObjLpalm_c, baseRotation, 0x3B0);
WWHD_OFFSET(daObjLpalm_c, phase, 0x3F8);
WWHD_OFFSET(daObjLpalm_c, backgroundMatrix, 0x408);
