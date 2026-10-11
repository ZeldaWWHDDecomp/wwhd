/** Scene Process Tag. WWHD. */
#include "f_op/f_op_scene_tag.h"
static node_list_class g_fopScnTg_SceneList;
static SceneTagHeaderStatics l_headerStatics;
static const SceneTagPiPair l_piPair;

/* 025DCFA4 */
void fopScnTg_QueueTo(scene_tag_class* i_sceneTag) {
    cTg_SingleCut(&i_sceneTag->base);
}

/* 025DCFA8 */
u32 fopScnTg_ToQueue(scene_tag_class* i_sceneTag) {
    // HD: the tail call preserves the addition result.
    return cTg_Addition(&g_fopScnTg_SceneList, &i_sceneTag->base);
}

/* 025DCFB8 */
void fopScnTg_Init(scene_tag_class* i_sceneTag, void* pData) {
    cTg_Create(&i_sceneTag->base, pData);
}

/* 025DCFBC */
static void __sinit_f_op_scene_tag_cpp() {
    // HD: common-header initialization retains the verified store/read/call order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3858);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    SceneTagFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3864);
    SceneTagSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3870);
}

/* WWHD glue: data and other-unit calls
 data g_fopScnTg_SceneList 101F387C
 data l_headerStatics 1048A630
 data l_piPair 10057DBC
 ext 0201A868 s32 cTg_SingleCut(create_tag_class*)
 ext 0201A8B4 s32 cTg_Addition(node_list_class*, create_tag_class*)
 ext 0201A918 void cTg_Create(create_tag_class*, void*)
 ext 028ED6F8 void SceneTagFirstHeaderObject_ct(u8*)
 ext 028EAB2C void SceneTagSecondHeaderObject_ct(u8*)
*/
