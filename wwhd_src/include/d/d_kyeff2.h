/* dKyeff2_c: volumetric-cloud environment process, WWHD layout.
 * GameCube -> WWHD: kankyo_class is 0xFC rather than 0xF8; no extra derived members.
 * The verified entry points retain u32 results (GameCube BOOL/cPhs_State). */
#pragma once
#include "gc_api/f_op/f_op_kankyo.h"

class dKyeff2_c : public kankyo_class {
public:
    enum : u32 { __vt = 0x100505B4 }; /* HD: virtual destructor table. */
    u32 execute();
};
WWHD_SIZE(dKyeff2_c, 0xFC);
