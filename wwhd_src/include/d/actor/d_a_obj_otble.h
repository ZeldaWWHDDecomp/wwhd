#pragma once
#include "f_op/f_op_actor.h"

namespace daObj_Otble {
// HD actor base adds0x11C. The Acch and circle retain their 0x1C4/0x40 sizes.
struct Act_c : fopAc_ac_c {
  /* 3AC */ be<u32> tableModel;
  /* 3B0 */ be<s32> variant;
  /* 3B4 */ request_of_phase_process_class phase;
  /* 3BC */ be<u32> background;
  /* 3C0 */ Mtx34 backgroundMatrix;
  /* 3F0 */ u8 acch[0x1C4];
  /* 5B4 */ u8 acchCircle[0x40];
};
WWHD_SIZE(Act_c, 0x5F4);
WWHD_OFFSET(Act_c, backgroundMatrix, 0x3C0);
WWHD_OFFSET(Act_c, acch, 0x3F0);
WWHD_OFFSET(Act_c, acchCircle, 0x5B4);
} // namespace daObj_Otble
