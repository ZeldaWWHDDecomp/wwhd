/* d_vibration: dVibration_c (the play object's rumble/quake control, play + PLAY_VIBRATION), GameCube
 * methods. Layout not needed (handled through dComIfGp_getVibration() only). */
#pragma once
#include "wwhd.h"

struct cXyz;

struct dVibration_c {
    u8 _opaque;
    /* 025CB374 dVibration_c::StartShock(int, int, cXyz): the cXyz by value is a pointer to a copy [v ladder] */
    BOOL StartShock(int strength, int flags, cXyz* pos) { return gabi::call<BOOL>(0x025CB374, this, strength, flags, pos); }
    /* 025CB66C [v lamp] */
    BOOL CheckQuake() { return gabi::call<BOOL>(0x025CB66C, this); }
};
