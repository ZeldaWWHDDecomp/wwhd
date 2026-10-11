/* HUD visibility, byte flags and HD fade transitions, verified against WWHD. */
#include "d/d_meter_head.h"

static u8 l_hudFlag = 0;
static u8 menu_status = 0;
static const f32 l_alphaOpaque = 1.0f;
static const f32 l_alphaTransparent = 0.0f;

/* 0259169C */
void dMeter_mtrShow() {
    ((dMeter_gameInfoView*)dComIfGp_get())->m2dShow = 1;
}

/* 025916C4 */
void dMeter_mtrHide() {
    ((dMeter_gameInfoView*)dComIfGp_get())->m2dShow = 0;
}

// HD: additional HUD flag, independent of the GameCube menu status.
/* 025916EC */
void dMeter_onHudFlag() {
    l_hudFlag = 1;
}

/* 025916FC */
void dMeter_offHudFlag() {
    l_hudFlag = 0;
}

/* 0259170C */
void dMenu_setMenuStatus(u32 status) {
    // HD: the entry accepts a full register, then narrows to the menu byte.
    menu_status = status;
}

/* 02591718 */
u32 dMeter_isBit8(be<u8>* flag, u32 bit) {
    // HD: preserve the full mask register rather than taking the GameCube u8.
    return (*flag & bit) != 0;
}

/* 0259172C */
void dMeter_onBit8(be<u8>* flag, u32 bit) {
    *flag |= bit;
}

/* 0259173C */
void dMeter_offBit8(be<u8>* flag, u32 bit) {
    *flag &= ~bit;
}

// HD: six-frame fade state machines absent from the GameCube HUD head.
/* 0259174C */
f32 dMeter_fadeOut(be<s16>* state, be<s16>* counter) {
    s32 mode = *state;
    f32 one = l_alphaOpaque;
    if (mode == dMeter_FADE_VISIBLE_e) {
        *state = dMeter_FADE_OUT_e;
        *counter = 1;
        return one;
    }
    f32 result = l_alphaTransparent;
    if (mode == dMeter_FADE_OUT_e) {
        s32 phase = *counter;
        if (phase < 6) {
            result = fopMsgM_valueIncrease(6, phase, 1);
            // HD: reload the counter after the easing call, before storing it.
            u16 next = (u16)(*counter + 1);
            result = gabi::fsubs_ppc(one, result);
            *counter = next;
        } else {
            *state = dMeter_FADE_HIDDEN_e;
        }
    }
    return result;
}

/* 025917FC */
f32 dMeter_fadeIn(be<s16>* state, be<s16>* counter) {
    s32 mode = *state;
    if (mode == dMeter_FADE_HIDDEN_e) {
        f32 zero = l_alphaTransparent;
        *state = dMeter_FADE_IN_e;
        *counter = 0;
        return zero;
    }
    f32 result = l_alphaOpaque;
    if (mode == dMeter_FADE_IN_e) {
        s32 phase = *counter;
        if (phase < 6) {
            result = fopMsgM_valueIncrease(6, phase, 1);
            *counter = (u16)(*counter + 1);
        } else {
            *state = dMeter_FADE_VISIBLE_e;
        }
    }
    return result;
}

/* WWHD glue
data l_hudFlag 101EA06C
data menu_status 101EA069
data l_alphaOpaque 100512B4
data l_alphaTransparent 100512B8
ext 025DB6F4 f32 fopMsgM_valueIncrease(s32 i_max, s32 i_value, u8 i_mode)
*/
