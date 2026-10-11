/** Process profile lookup. */
#include "bindings.h"
#include "f_pc/f_pc_base.h"

/* 025E10E0 */
static process_profile_definition* fpcPf_Get_HD(u32 i_profName) {
    // HD: embedded profile table; the incoming index stays unsigned and address arithmetic wraps.
    const u32 profileListAddress = 0x101F3EE0;
    return gabi::at<process_profile_definition>(gabi::load<u32>(profileListAddress + (i_profName << 2)));
}

/* 025E10F4 */
// HD: this translation unit's header statics copy their pi pair from rodata.
static void __sinit_f_pc_profile_cpp() {
    sinit_header_statics_rodata(0x1048AB4C, 0x101F3E98, 0x1048AB58, 0x1005846C);
}

/* WWHD glue (tools/verify/autoglue.py): no data bindings. */
