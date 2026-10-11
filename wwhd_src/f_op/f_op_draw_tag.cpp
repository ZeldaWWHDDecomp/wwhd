/** Draw queue tags. WWHD, verified unit f_op_draw_tag. */
#include "f_op/f_op_draw_tag.h"

static node_lists_tree_class g_fopDwTg_Queue;
static draw_list_table drawLists;

/* 025DA874 */
u32 fopDwTg_ToDrawQ(create_tag_class* i_createTag, s32 i_priority) {
    return cTg_AdditionToTree(&g_fopDwTg_Queue, i_priority, i_createTag);
}

/* 025DA884 */
void fopDwTg_DrawQTo(create_tag_class* i_createTag) {
    cTg_SingleCutFromTree(i_createTag);
}

/* 025DA888 */
bool fopDwTg_Init(create_tag_class* i_createTag, void* i_process) {
    cTg_Create(i_createTag, i_process);
    return true;
}

/* 025DA8AC */
void fopDwTg_CreateQueue() {
    cTr_Create(&g_fopDwTg_Queue, drawLists.lists, 1000);
}

/* 025DA8C4 */
// HD: this translation unit's header statics and destructor registrations.
static void __sinit_f_op_draw_tag_cpp() {
    sinit_header_statics_rodata(0x10487590, 0x101F33B8, 0x1048759C, 0x10057994);
}

/* WWHD glue (tools/verify/autoglue.py): shared draw queue and external tag operations
data g_fopDwTg_Queue 101F33DC
data drawLists 104875AC
ext 0201A770 u32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*)
ext 0201A724 void cTg_SingleCutFromTree(create_tag_class*)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
ext 0201AB04 void cTr_Create(node_lists_tree_class*, node_list_class*, s32)
*/
