/* d_a_wall.h (WWHD): daWall_c layout (0x700). */
#pragma once
#include "f_op/f_op_actor.h"
struct daWall_c : fopAc_ac_c {
 request_of_phase_process_class phase;
 gptr<void> model;
 u8 status[0x3C];
 u8 triangles[2][0x150];
 gptr<void> background;
 be<f32> backgroundMatrix[12];
 be<u8> mode; u8 padMode[3];
 u8 smokeCallback[0x20];
 gptr<void> emitter;
 be<f32> smokeAlpha;
 be<u8> breakCounter; u8 padCounter[3];
 be<s32> switchNo;
 be<u8> type; u8 padType[3];
};
WWHD_OFFSET(daWall_c, phase, 0x3AC);
WWHD_OFFSET(daWall_c, model, 0x3B4);
WWHD_OFFSET(daWall_c, triangles, 0x3F4);
WWHD_OFFSET(daWall_c, background, 0x694);
WWHD_OFFSET(daWall_c, backgroundMatrix, 0x698);
WWHD_OFFSET(daWall_c, mode, 0x6C8);
WWHD_OFFSET(daWall_c, smokeCallback, 0x6CC);
WWHD_OFFSET(daWall_c, smokeAlpha, 0x6F0);
WWHD_OFFSET(daWall_c, switchNo, 0x6F8);
WWHD_OFFSET(daWall_c, type, 0x6FC);
WWHD_SIZE(daWall_c, 0x700);
