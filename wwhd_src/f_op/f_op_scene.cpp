/** Scene Process Framework. WWHD. */
#include "f_op/f_op_scene.h"

/* 025DC5B8 */
static BOOL fopScn_Draw(scene_class* i_this) {
    return fpcNd_DrawMethod(i_this->mpMtd, i_this);
}

/* 025DC5C4 */
static BOOL fopScn_Execute(scene_class* i_this) {
    return fpcMtd_Execute(i_this->mpMtd, i_this);
}

/* 025DC5D0 */
static BOOL fopScn_IsDelete(scene_class* i_this) {
    return fpcMtd_IsDelete(i_this->mpMtd, i_this);
}

/* 025DC5DC */
static BOOL fopScn_Delete(scene_class* i_this) {
    BOOL result = fpcMtd_Delete(i_this->mpMtd, i_this);
    if (result == TRUE) {
        fopScnTg_QueueTo(&i_this->mScnTg);
    }
    mDoHIO_root_update();
    return result;
}

/* 025DC634 */
static cPhs_State fopScn_Create(scene_class* i_this) {
    if (i_this->mInitState == 0) {
        scene_process_profile_definition* profile = reinterpret_cast<scene_process_profile_definition*>(i_this->mpProf.get());
        i_this->mpMtd = profile->sub_method;
        fopScnTg_Init(&i_this->mScnTg, i_this);
        fopScnTg_ToQueue(&i_this->mScnTg);
        be<u32>* append = reinterpret_cast<be<u32>*>(i_this->mpUserData.get());
        if (append) {
            i_this->mParameters = *append;
        }
    }
    return fpcMtd_Create(i_this->mpMtd, i_this);
}

/* 025DC6A8 */
static void __sinit_f_op_scene_cpp() {
    // HD: compiler-generated common-header initialization, attributed by adjacency.
    sinit_header_statics(0x1048A5A4, 0x101F3738);
}

/* WWHD glue: typed other-unit calls
ext 025DFFA8 s32 fpcNd_DrawMethod(scene_method_class*, scene_class*)
ext 025DFCC4 s32 fpcMtd_Execute(scene_method_class*, scene_class*)
ext 025DFCCC s32 fpcMtd_IsDelete(scene_method_class*, scene_class*)
ext 025DFCD4 s32 fpcMtd_Delete(scene_method_class*, scene_class*)
ext 025DFCDC s32 fpcMtd_Create(scene_method_class*, scene_class*)
ext 025DCFA4 void fopScnTg_QueueTo(scene_tag_class*)
ext 025DCFA8 void fopScnTg_ToQueue(scene_tag_class*)
ext 025DCFB8 void fopScnTg_Init(scene_tag_class*, scene_class*)
ext 025F0A0C void mDoHIO_root_update()
*/
