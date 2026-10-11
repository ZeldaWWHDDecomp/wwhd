/** Overlap Process Framework. */
#include "bindings.h"
#include "f_op/f_op_overlap.h"
#include "gc_api/f_pc/f_pc.h"

/* 025DBBDC */
static s32 fopOvlp_Draw(overlap_task_class* i_this) {
    return fpcLf_DrawMethod((leafdraw_method_class*)(overlap_method_class*)i_this->sub_method, i_this);
}

/* 025DBBE8 */
static s32 fopOvlp_Execute(overlap_task_class* i_this) {
    return fpcMtd_Execute((process_method_class*)(overlap_method_class*)i_this->sub_method, i_this);
}

/* 025DBBF4 */
static s32 fopOvlp_IsDelete(overlap_task_class* i_this) {
    return fpcMtd_IsDelete((process_method_class*)(overlap_method_class*)i_this->sub_method, i_this);
}

/* 025DBC00 */
static s32 fopOvlp_Delete(overlap_task_class* i_this) {
    return fpcMtd_Delete((process_method_class*)(overlap_method_class*)i_this->sub_method, i_this);
}

/* 025DBC28 */
static s32 fopOvlp_Create(overlap_task_class* i_this) {
    if (i_this->base.mInitState == 0) {
        overlap_process_profile_definition* profile = (overlap_process_profile_definition*)(process_profile_definition*)i_this->base.mpProf;
        cReq_Create(&i_this->mRq, 1);
        // HD: preserve the original load before the two stores.
        overlap_method_class* method = profile->sub_method;
        i_this->mScenePId = fpcM_ERROR_PROCESS_ID_e;
        i_this->sub_method = method;
    }
    return fpcMtd_Create((process_method_class*)(overlap_method_class*)i_this->sub_method, i_this);
}

/* 025DBC90 */
// HD: this translation unit's header statics initializer.
static void __sinit_f_op_overlap_cpp() {
    sinit_header_statics(0x1048A524, 0x101F3694);
}

/* WWHD glue (tools/verify/autoglue.py): local GameCube calls
 ext 025DF2C0 s32 fpcLf_DrawMethod(leafdraw_method_class*, void*)
 ext 0201A33C void cReq_Create(request_base_class*, u32)
*/
