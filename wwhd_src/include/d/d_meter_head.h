/* HUD head helpers. The GameCube m2dShow byte is at the HD game-info offset
 * below; the fade state machines are HD additions. */
#pragma once
#include "bindings.h"
#include "d/d_com_inf_game.h"

struct dMeter_gameInfoView {
    /* 0x0000 */ u8 mUnaccessed[0x5C22];
    /* 0x5C22 */ be<u8> m2dShow;
};
WWHD_OFFSET(dMeter_gameInfoView, m2dShow, 0x5C22);
WWHD_SIZE(dMeter_gameInfoView, 0x5C23);

enum dMeter_fadeState {
    dMeter_FADE_HIDDEN_e = 0,
    dMeter_FADE_IN_e = 7,
    dMeter_FADE_VISIBLE_e = 8,
    dMeter_FADE_OUT_e = 9,
};
f32 fopMsgM_valueIncrease(s32 i_max, s32 i_value, u8 i_mode);
