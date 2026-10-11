/** Framework - Scene Process Iteration. WWHD; local only. */
#include "f_op/f_op_scene_iter.h"

static node_list_class g_fopScnTg_SceneList;
static SceneIterHeaderStatics l_headerStatics;
static const SceneIterPiPair l_piPair;

/* 025DC73C */
void* fopScnIt_Judge(u32 pFunc, void* pUserData) {
    SceneIterParams iterParams;
    iterParams.mFunc = pFunc;
    iterParams.mpData = pUserData;
    return cLsIt_Judge(&g_fopScnTg_SceneList, cTgIt_JudgeFilter, &iterParams);
}

/* 025DC778 */
static void __sinit_f_op_scene_iter_cpp() {
    // HD: keep the common-header store/read/registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3770);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    SceneIterFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F377C);
    SceneIterSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3788);
}

/* WWHD glue: this translation unit's data and other-unit calls
 data g_fopScnTg_SceneList 101F387C
 data l_headerStatics 1048A5C0
 data l_piPair 10057D54
 ext 020100BC void* cLsIt_Judge(node_list_class*, u32, void*)
 ext 028ED6F8 void SceneIterFirstHeaderObject_ct(u8*)
 ext 028EAB2C void SceneIterSecondHeaderObject_ct(u8*)
*/
