/** Creation queue tags. */
#include "f_pc/f_pc_create_tag.h"

static node_list_class g_fpcCtTg_Queue;
static CreateTagHeaderStatics l_headerStatics;
static const CreateTagPiPair l_piPair;

/* 025DDC18 */
u32 fpcCtTg_ToCreateQ(create_tag* i_createTag) {
    // HD: retain the callee's return register; GameCube declares this void.
    return cTg_Addition(&g_fpcCtTg_Queue, &i_createTag->base);
}

/* 025DDC28 */
void fpcCtTg_CreateQTo(create_tag* i_createTag) {
    cTg_SingleCut(&i_createTag->base);
}

/* 025DDC2C */
s32 fpcCtTg_Init(create_tag* i_createTag, void* i_data) {
    cTg_Create(&i_createTag->base, i_data);
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 025DDC50 */
static void __sinit_f_pc_create_tag_cpp() {
    // Preserve the constructor's nonsequential zeroing and pi load/store order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3980);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    CreateTagFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F398C);
    CreateTagSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3998);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A6F4
 data l_piPair 10057E94
 data g_fpcCtTg_Queue 101F39A4
 ext 0201A8B4 u32 cTg_Addition(node_list_class*, create_tag_class*)
 ext 0201A868 void cTg_SingleCut(create_tag_class*)
 ext 0201A918 void cTg_Create(create_tag_class*, void*)
 ext 028ED6F8 void CreateTagFirstHeaderObject_ct(u8*)
 ext 028EAB2C void CreateTagSecondHeaderObject_ct(u8*)
*/
