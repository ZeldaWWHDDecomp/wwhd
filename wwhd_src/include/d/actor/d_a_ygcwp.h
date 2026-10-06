#pragma once
#include "bindings.h"
// HD embeds two 0x78-byte BRK controllers, rather than GC's 0x18-byte objects.
struct YgcwpBrk_l {
    be<f32> speed;
    be<f32> frame;
    u8 state[0x70];
};
static_assert(sizeof(YgcwpBrk_l)==0x78);
struct daYgcwp_c : fopAc_ac_c {
    gptr<J3DModel> model;                         // 0x3AC
    request_of_phase_process_class phase;        // 0x3B0
    YgcwpBrk_l brk[2];                           // 0x3B8
    be<s32> staff;                               // 0x4A8
    be<s32> timer;
    be<s32> currentBrk;
    be<s32> parameter;
};
WWHD_OFFSET(daYgcwp_c,model,0x3AC);
WWHD_OFFSET(daYgcwp_c,phase,0x3B0);
WWHD_OFFSET(daYgcwp_c,brk,0x3B8);
WWHD_OFFSET(daYgcwp_c,staff,0x4A8);
WWHD_OFFSET(daYgcwp_c,parameter,0x4B4);
WWHD_SIZE(daYgcwp_c,0x4B8);
