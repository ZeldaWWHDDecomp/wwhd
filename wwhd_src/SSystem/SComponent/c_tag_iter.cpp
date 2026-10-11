/** Tagged-node iteration. WWHD, verified unit c_tag_iter. */
#include "SSystem/SComponent/c_tag_iter.h"

/* 0201A9F4 */
s32 cTgIt_MethodCall(create_tag_class* tag, method_filter* filter) {
    u32 function = filter->mpMethodFunc.get();
    void* userData = filter->mpUserData;
    // The shared process layout keeps tag data as a guest address.
    node_class* tagData = gabi::at<node_class>(tag->mpTagData);
    return gabi::call_ptr<s32>(function, tagData, userData);
}

/* 0201AA08 */
void* cTgIt_JudgeFilter(create_tag_class* tag, judge_filter* filter) {
    u32 function = filter->mpJudgeFunc.get();
    void* userData = filter->mpUserData;
    node_class* tagData = gabi::at<node_class>(tag->mpTagData);
    return gabi::call_ptr<void*>(function, tagData, userData);
}

/* 0201AA1C */
// HD: header statics and destructor registrations.
static void __sinit_c_tag_iter_cpp() {
    sinit_header_statics_rodata(0x101FFB38, 0x1018D3F4, 0x101FFB44, 0x100036D0);
}

/* WWHD glue (tools/verify/autoglue.py): no local data bindings
*/
