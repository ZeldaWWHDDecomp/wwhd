/* d_a_obj_barrel: daObjBarrel::Act_c, the part other actors use (HD offsets). */
#pragma once
#include "bindings.h"

namespace daObjBarrel {
struct Act_c : fopAc_ac_c {
    enum Mode_e { MODE_WAIT = 0 };
    /* 0x3AC */ u8 _3AC[0x728 - 0x3AC];
    /* 0x728 */ be<s32> mMode; /* GameCube 0x60C [v nz] */
    /* GameCube inline */
    bool pos_init() {
        if (mMode == MODE_WAIT) {
            current.pos = home.pos;
            current.angle = home.angle;
            shape_angle = home.angle;
            return true;
        } else {
            return false;
        }
    }
};
}  // namespace daObjBarrel
