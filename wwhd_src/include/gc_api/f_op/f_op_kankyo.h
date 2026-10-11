/* f_op_kankyo: GameCube names and WWHD environment process layout.
 * HD: leafdraw_class grows by four bytes, moving the environment members by +4.
 * Offsets are exercised by the verified f_op_kankyo lifecycle functions; d_kyeff2
 * uses the base process's virtual destructor table (0x02586C48 [v d_kyeff2]). */
#pragma once
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_xyz.h"

struct kankyo_method_class;
struct kankyo_class {
    /* 0x00 */ leafdraw_class base;
    /* 0xC4 */ be<s32> mBsType;
    /* 0xC8 */ create_tag_class draw_tag;
    /* 0xDC */ gptr<kankyo_method_class> sub_method;
    /* 0xE0 */ cXyz mPos;
    /* 0xEC */ cXyz mScale;
    /* 0xF8 */ be<u32> mParam;
};
WWHD_OFFSET(kankyo_class, mBsType, 0xC4);
WWHD_OFFSET(kankyo_class, draw_tag, 0xC8);
WWHD_OFFSET(kankyo_class, sub_method, 0xDC);
WWHD_OFFSET(kankyo_class, mPos, 0xE0);
WWHD_OFFSET(kankyo_class, mScale, 0xEC);
WWHD_OFFSET(kankyo_class, mParam, 0xF8);
WWHD_SIZE(kankyo_class, 0xFC);
