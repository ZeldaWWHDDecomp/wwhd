/** Deletion queue tags. */
#include "f_pc/f_pc_delete_tag.h"

static node_list_class g_fpcDtTg_Queue;
static DeleteTagHeaderStatics l_headerStatics;
static const DeleteTagPiPair l_piPair;

/* 025DDE18 */
BOOL fpcDtTg_IsEmpty() {
    return g_fpcDtTg_Queue.mSize == 0;
}

/* 025DDE2C */
u32 fpcDtTg_ToDeleteQ(delete_tag_class* i_deleteTag) {
    // HD: retain the callee's return register; GameCube declares this void.
    i_deleteTag->mTimer = 1;
    return cTg_Addition(&g_fpcDtTg_Queue, &i_deleteTag->base);
}

/* 025DDE44 */
s32 fpcDtTg_Do(delete_tag_class* i_deleteTag, delete_tag_func i_func) {
    s16 timer = i_deleteTag->mTimer;
    if (timer > 0) {
        i_deleteTag->mTimer = static_cast<u16>(static_cast<u32>(timer) - 1);
        return 0;
    }
    cTg_SingleCut(&i_deleteTag->base);
    // The shared tag layout retains the guest data pointer as a u32 field.
    void* data = gabi::at<void>(i_deleteTag->base.mpTagData);
    s32 ret = gabi::call_ptr<s32>(i_func, data);
    if (ret == 0) {
        fpcDtTg_ToDeleteQ(i_deleteTag);
        return 0;
    }
    return 1;
}

/* 025DDEEC */
s32 fpcDtTg_Init(delete_tag_class* i_deleteTag, void* i_data) {
    cTg_Create(&i_deleteTag->base, i_data);
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 025DDF10 */
static void __sinit_f_pc_delete_tag_cpp() {
    // Preserve the constructor's nonsequential zeroing and pi load/store order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F39F8);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    DeleteTagFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3A04);
    DeleteTagSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3A10);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A748
 data l_piPair 10058274
 data g_fpcDtTg_Queue 101F3A1C
 ext 0201A8B4 u32 cTg_Addition(node_list_class*, create_tag_class*)
 ext 0201A868 void cTg_SingleCut(create_tag_class*)
 ext 0201A918 void cTg_Create(create_tag_class*, void*)
 ext 028ED6F8 void DeleteTagFirstHeaderObject_ct(u8*)
 ext 028EAB2C void DeleteTagSecondHeaderObject_ct(u8*)
*/
