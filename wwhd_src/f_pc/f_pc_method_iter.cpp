/** Process method iteration, GameCube structure verified against WWHD. Local only. */
#include "f_pc/f_pc_method_iter.h"

/* 025DFD78 */
// HD: callbacks are represented by guest code addresses.
BOOL fpcMtdIt_Method(node_list_class* i_nodeList, u32 i_methods) {
    return cLsIt_Method(i_nodeList, i_methods, nullptr);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025DFD80 */
static void __sinit_f_pc_method_iter_cpp() {
    sinit_header_statics_rodata(0x1048AA7C, 0x101F3CD0, 0x1048AA88, 0x100583D0);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 020100A0 s32 cLsIt_Method(node_list_class*, u32, void*)
*/
