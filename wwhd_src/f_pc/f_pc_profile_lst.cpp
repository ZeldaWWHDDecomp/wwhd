/** Process profile list header initializer. */
#include "bindings.h"

/* 025E1188 */
// HD: the profile list is embedded data; this translation unit only initializes header statics.
static void __sinit_f_pc_profile_lst_cpp() {
    sinit_header_statics_rodata(0x1048AB68, 0x101F3EBC, 0x1048AB74, 0x10058478);
}

/* WWHD glue (tools/verify/autoglue.py): no data bindings. */
