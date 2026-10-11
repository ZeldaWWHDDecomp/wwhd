/** Draw queue iteration. WWHD, verified unit f_op_draw_iter. */
#include "f_op/f_op_draw_iter.h"

// HD: keep unsigned increment/wrap before interpreting the index as signed.
static u32 l_fopDwTg_id;
static node_lists_tree_class g_fopDwTg_Queue;

/* 025DA758 */
static create_tag_class* fopDwIt_GetTag() {
    u32 index = l_fopDwTg_id + 1;
    s32 count = g_fopDwTg_Queue.mNumLists;
    if (s32(index) >= count) {
        return NULL;
    }
    node_list_class* lists = g_fopDwTg_Queue.mpLists;
    do {
        l_fopDwTg_id = index;
        create_tag_class* tag = reinterpret_cast<create_tag_class*>(lists[index].mpHead.get());
        if (tag != NULL) {
            return tag;
        }
        ++index;
    } while (s32(index) < count);
    return NULL;
}

/* 025DA7AC */
create_tag_class* fopDwIt_Begin() {
    node_list_class* lists = g_fopDwTg_Queue.mpLists;
    create_tag_class* createTagClass = reinterpret_cast<create_tag_class*>(lists->mpHead.get());
    l_fopDwTg_id = 0;
    if (createTagClass != NULL) {
        return createTagClass;
    }
    return fopDwIt_GetTag();
}

/* 025DA7D0 */
create_tag_class* fopDwIt_Next(create_tag_class* i_createTag) {
    // The canonical shared tag stores its node member as bytes.
    node_class* node = reinterpret_cast<node_class*>(i_createTag->mNode);
    create_tag_class* createTagClass = reinterpret_cast<create_tag_class*>(node->mpNextNode.get());
    if (createTagClass != NULL) {
        return createTagClass;
    }
    return fopDwIt_GetTag();
}

/* 025DA7E0 */
// HD: this translation unit's header statics and destructor registrations.
static void __sinit_f_op_draw_iter_cpp() {
    sinit_header_statics_rodata(0x10487574, 0x101F3394, 0x10487580, 0x1005797C);
}

/* WWHD glue (tools/verify/autoglue.py): shared draw queue and iterator cursor
data l_fopDwTg_id 101F3390
data g_fopDwTg_Queue 101F33DC
*/
