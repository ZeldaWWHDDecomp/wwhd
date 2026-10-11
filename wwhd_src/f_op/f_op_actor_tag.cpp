/* Actor queue tags; verified WWHD wrappers in tww layout. */
#include "f_op/f_op_actor_tag.h"

static node_list_class g_fopAcTg_Queue;

/* 025DA2F8 */
u32 fopAcTg_ToActorQ(create_tag_class* i_createTag) {
    return cTg_Addition(&g_fopAcTg_Queue, i_createTag);
}

/* 025DA308 */
u32 fopAcTg_ActorQTo(create_tag_class* i_createTag) { return cTg_SingleCutFromTree(i_createTag); }

/* 025DA30C */
u32 fopAcTg_Init(create_tag_class* i_createTag, void* i_data) {
    cTg_Create(i_createTag, i_data);
    return 1;
}

// HD: compiler-generated common-header initialization for this TU.
/* 025DA330 */
static void __sinit_f_op_actor_tag_cpp() {
    sinit_header_statics_rodata(0x10487510, 0x101F3304, 0x1048751C, 0x10057944);
}

/* WWHD glue (tools/verify/autoglue.py)
data g_fopAcTg_Queue 101F3328
ext 0201A8B4 s32 cTg_Addition(node_list_class*, create_tag_class*)
ext 0201A724 s32 cTg_SingleCutFromTree(create_tag_class*)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
*/
