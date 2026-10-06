#pragma once
#include "f_op/f_op_actor.h"
// HD removes the GameCube 2D display-list objects; profile size is0x6E8.
struct daMgBoard_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    be<u32> boardModel, cursorModel;
    be<u32> hitModels[20], missModels[32], shipModels[6];
    u8 _4A4[0x584-0x4A4];
    be<s8> cursorX, cursorY;
    u8 _586[2];
    be<s32> lastFireX, lastFireY;
    u8 gameInfo[0x69C-0x590];
    be<s32> missCount, hitCount;
    be<s16> startEvent, endEvent;
    u8 stickControl[0x28];
    cXyz npcPosition;
    be<s32> state;
    be<u8> drawInfo, startGame, endGame, forceEnd, timer;
    u8 _6E5[3];
};
WWHD_OFFSET(daMgBoard_c, boardModel, 0x3B4);
WWHD_OFFSET(daMgBoard_c, cursorX, 0x584);
WWHD_OFFSET(daMgBoard_c, missCount, 0x69C);
WWHD_OFFSET(daMgBoard_c, stickControl, 0x6A8);
WWHD_OFFSET(daMgBoard_c, npcPosition, 0x6D0);
WWHD_OFFSET(daMgBoard_c, state, 0x6DC);
WWHD_SIZE(daMgBoard_c, 0x6E8);
