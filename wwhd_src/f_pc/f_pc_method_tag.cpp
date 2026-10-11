/** Process method queue tags, GameCube structure verified against WWHD. Local only. */
#include "f_pc/f_pc_method_tag.h"

/* 025DFE14 */
BOOL fpcMtdTg_Do(process_method_tag_class* i_methodTag) {
    u32 method = i_methodTag->mpFunc.get();
    void* data = i_methodTag->mpMthdData.get();
    return gabi::call_ptr<s32>(method, data);
}

/* 025DFE24 */
s32 fpcMtdTg_ToMethodQ(node_list_class* pList, process_method_tag_class* i_methodTag) {
    return cTg_Addition(pList, &i_methodTag->mCreateTag);
}

/* 025DFE28 */
void fpcMtdTg_MethodQTo(process_method_tag_class* i_methodTag) {
    cTg_SingleCut(&i_methodTag->mCreateTag);
}

/* 025DFE2C */
// HD: the callback remains a guest code address.
s32 fpcMtdTg_Init(process_method_tag_class* i_methodTag, u32 i_mtdTgFunc, void* i_data) {
    cTg_Create(&i_methodTag->mCreateTag, i_methodTag);
    i_methodTag->mpFunc.v = i_mtdTgFunc;
    i_methodTag->mpMthdData = i_data;
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025DFE80 */
static void __sinit_f_pc_method_tag_cpp() {
    sinit_header_statics_rodata(0x1048AA98, 0x101F3CF4, 0x1048AAA4, 0x100583D8);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0201A8B4 s32 cTg_Addition(node_list_class*, create_tag_class*)
ext 0201A868 s32 cTg_SingleCut(create_tag_class*)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
*/
