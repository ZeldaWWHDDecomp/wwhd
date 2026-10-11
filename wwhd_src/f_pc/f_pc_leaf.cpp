/** Leaf Process. */
#include "f_pc/f_pc_leaf.h"

static s32 g_fpcLf_type;
static LeafHeaderStatics l_headerStatics;
static const LeafPiPair l_piPair;

/* 025DF2B8 */
s32 fpcLf_GetPriority(leafdraw_class* i_leaf) {
    // HD: preserve the full-width return from fpcDwPi_Get (GameCube signature: s16).
    return fpcDwPi_Get(&i_leaf->mDwPi);
}

/* 025DF2C0 */
BOOL fpcLf_DrawMethod(leafdraw_method_class* i_methods, void* i_data) {
    return fpcMtd_Method(i_methods->mpDrawFunc.get(), i_data);
}

/* 025DF2C8 */
BOOL fpcLf_Draw(leafdraw_class* i_leaf) {
    if (i_leaf->mbUnk0 != 0) {
        return FALSE;
    }
    return fpcLf_DrawMethod(fpcLf_getMethods(i_leaf), i_leaf);
}

/* 025DF300 */
BOOL fpcLf_Execute(leafdraw_class* i_leaf) {
    return fpcMtd_Execute(&fpcLf_getMethods(i_leaf)->base, i_leaf);
}

/* 025DF30C */
BOOL fpcLf_IsDelete(leafdraw_class* i_leaf) {
    return fpcMtd_IsDelete(&fpcLf_getMethods(i_leaf)->base, i_leaf);
}

/* 025DF318 */
BOOL fpcLf_Delete(leafdraw_class* i_leaf) {
    BOOL ret = fpcMtd_Delete(&fpcLf_getMethods(i_leaf)->base, i_leaf);
    if (ret == TRUE) {
        i_leaf->mSubType = 0;
    }
    return ret;
}

/* 025DF35C */
cPhs_State fpcLf_Create(leafdraw_class* i_leaf) {
    if (i_leaf->mInitState == 0) {
        leaf_process_profile_definition* profDef =
            (leaf_process_profile_definition*)i_leaf->mpProf.get();
        i_leaf->mpDrawMtd = profDef->sub_method;
        i_leaf->mSubType = fpcBs_MakeOfType(&g_fpcLf_type);
        fpcDwPi_Init(&i_leaf->mDwPi, profDef->mPriority);
        i_leaf->mbUnk0 = 0;
    }
    return fpcMtd_Create(&fpcLf_getMethods(i_leaf)->base, i_leaf);
}

/* 025DF3D4 */
static void __sinit_f_pc_leaf_cpp() {
    // HD: preserve the header objects' zero-store and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3BB4);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    LeafFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3BC0);
    LeafSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3BCC);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_fpcLf_type 101F3BD8
data l_headerStatics 1048A844
data l_piPair 10058324
ext 025DE460 s32 fpcDwPi_Get(be<s16>*)
ext 025DE470 void fpcDwPi_Init(be<s16>*, s32)
ext 025DFCAC BOOL fpcMtd_Method(u32, void*)
ext 025DD268 s32 fpcBs_MakeOfType(be<s32>*)
ext 028ED6F8 void LeafFirstHeaderObject_ct(u8*)
ext 028EAB2C void LeafSecondHeaderObject_ct(u8*)
*/
