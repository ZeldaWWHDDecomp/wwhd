/* Camera process dispatch; tww flow with verified HD pause and creation semantics. Local only. */
#include "f_op/f_op_camera.h"

/* 025DA3C4 */
BOOL fopCam_Draw(camera_class* camera) {
    BOOL cam_proc = TRUE;
    if (!dMenu_flag())
        cam_proc = fpcLf_DrawMethod(camera->mpMtd, &camera->view);
    return cam_proc;
}

/* 025DA418 */
BOOL fopCam_Execute(camera_class* camera) {
    // HD returns TRUE on either pause path; the GC source leaves ret uninitialized there.
    if (dMenu_flag() || dScnPly_ply_c::isPause())
        return TRUE;
    return fpcMtd_Execute(&camera->mpMtd->base, camera);
}

/* 025DA478 */
BOOL fopCam_IsDelete(camera_class* camera) {
    BOOL ret = fpcMtd_IsDelete(&camera->mpMtd->base, camera);
    if (ret == TRUE)
        fopDwTg_DrawQTo(&camera->draw_tag);
    return ret;
}

/* 025DA4CC */
BOOL fopCam_Delete(camera_class* camera) {
    BOOL ret = fpcMtd_Delete(&camera->mpMtd->base, camera);
    if (ret == TRUE)
        fopDwTg_DrawQTo(&camera->draw_tag);
    return ret;
}

/* 025DA520 */
cPhs_State fopCam_Create(void* pProc) {
    camera_class* camera = static_cast<camera_class*>(pProc);
    if (camera->view.mInitState == 0) {
        auto* profile =
            reinterpret_cast<camera_process_profile_definition*>(camera->view.mpProf.get());
        camera->mpMtd = profile->sub_method;
        fopDwTg_InitHD(&camera->draw_tag, camera);
        auto* append = static_cast<fopCamM_prm_class*>(camera->view.mpUserData.get());
        if (append)
            camera->view.mParameters = append->base.parameters;
    }
    cPhs_State ret = fpcMtd_Create(&camera->mpMtd->base, camera);
    if (ret == cPhs_COMPLEATE_e) {
        s32 priority = fpcM_DrawPriority(&camera->view);
        fopDwTg_ToDrawQHD(&camera->draw_tag, priority);
    }
    return ret;
}

// HD: compiler-generated common-header initialization for this TU.
/* 025DA5B8 */
static void __sinit_f_op_camera_cpp() {
    sinit_header_statics_rodata(0x1048752C, 0x101F3334, 0x10487538, 0x10057950);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025986BC s32 dMenu_flag()
ext 025AF2A4 s32 dScnPly_isPauseHD()
ext 025DF2C0 s32 fpcLf_DrawMethod(leafdraw_method_class*, view_class*)
ext 025DA884 void fopDwTg_DrawQTo(create_tag_class*)
ext 025DA888 void fopDwTg_InitHD(create_tag_class*, void*)
ext 025DF2B8 s32 fpcM_DrawPriority(leafdraw_class*)
ext 025DA874 void fopDwTg_ToDrawQHD(create_tag_class*, s32)
*/
