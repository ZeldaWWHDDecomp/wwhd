/** Process-line queue initialization. Private decomp source. */
#include "bindings.h"
#include "f_pc/f_pc_line.h"

static fpcLn_LineArray l_fpcLn_Line;

/* 025DF468 */
void fpcLn_Create() {
    s32 i = 16;
    node_list_class* pLine = l_fpcLn_Line;
    while (i-- > 0) {
        cLs_Create(pLine++);
    }
}

/* 025DF4B4 */
static void __sinit_f_pc_line_cpp() {
    sinit_header_statics_rodata(0x1048A860, 0x101F3BF0, 0x1048A86C, 0x1005833C);
}

/* WWHD glue (tools/verify/autoglue.py)
 data l_fpcLn_Line 1048A87C
 ext 02010008 void cLs_Create(node_list_class*)
*/
