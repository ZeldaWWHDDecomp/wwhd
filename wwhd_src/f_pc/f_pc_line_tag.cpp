/** Process-line tag operations. Private decomp source. */
#include "bindings.h"
#include "f_pc/f_pc_line_tag.h"

static node_lists_tree_class g_fpcLn_Queue;

/* 025DF694 */
void fpcLnTg_QueueTo(line_tag* i_lineTag) {
    cTg_SingleCutFromTree(&i_lineTag->base);
    i_lineTag->mLineListID = -1;
}

/* 025DF6C8 */
s32 fpcLnTg_ToQueue(line_tag* i_lineTag, s32 lineListID) {
    s32 ret = cTg_AdditionToTree(&g_fpcLn_Queue, lineListID, &i_lineTag->base);
    if (ret) {
        i_lineTag->mLineListID = lineListID;
        return 1;
    }
    return 0;
}

/* 025DF738 */
s32 fpcLnTg_Move(line_tag* i_lineTag, s32 i_newListID) {
    if (i_lineTag->mLineListID != i_newListID) {
        fpcLnTg_QueueTo(i_lineTag);
        return fpcLnTg_ToQueue(i_lineTag, i_newListID);
    }
    return 1;
}

/* 025DF7A8 */
void fpcLnTg_Init(line_tag* i_lineTag, void* i_data) {
    cTg_Create(&i_lineTag->base, i_data);
    i_lineTag->mLineListID = -1;
}

/* 025DF7DC */
static void __sinit_f_pc_line_tag_cpp() {
    sinit_header_statics_rodata(0x1048A958, 0x101F3C40, 0x1048A964, 0x1005836C);
}

/* WWHD glue (tools/verify/autoglue.py)
 data g_fpcLn_Queue 101F3C14
 ext 0201A724 s32 cTg_SingleCutFromTree(create_tag_class*)
 ext 0201A770 s32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*)
 ext 0201A918 void cTg_Create(create_tag_class*, void*)
*/
