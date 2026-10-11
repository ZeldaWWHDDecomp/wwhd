/** Environment Process Framework. */
#include "bindings.h"
#include "f_op/f_op_kankyo.h"
#include "gc_api/f_pc/f_pc.h"

static s32 fopKy_KANKYO_TYPE;

/* 025DA958 */
static s32 fopKy_IsKankyo(kankyo_class* i_this) {
    s32 type = i_this->mBsType;
    return fpcBs_Is_JustOfType(fopKy_KANKYO_TYPE, type);
}

/* 025DA968 */
static s32 fopKy_Draw(kankyo_class* i_this) {
    // HD: return TRUE when menu state skips drawing (GameCube leaves the result uninitialized).
    s32 result = TRUE;
    if (dMenu_flag() == 0) {
        result = fpcLf_DrawMethod((leafdraw_method_class*)(kankyo_method_class*)i_this->sub_method, i_this);
    }
    return result;
}

/* 025DA9BC */
static s32 fopKy_Execute(kankyo_class* i_this) {
    // HD: return TRUE when paused or skipped; keep null checks before reading a process name.
    if (dScnPly_ply_c::isPause() != 0) {
        return TRUE;
    }
    bool allowed = dMenu_flag() == 0;
    if (!allowed && i_this != NULL) {
        allowed = i_this->base.mProcName == fpcNm_ENVSE_e;
        if (!allowed) {
            allowed = i_this->base.mProcName == fpcNm_LEVEL_SE_e;
        }
    }
    if (allowed) {
        return fpcMtd_Execute((process_method_class*)(kankyo_method_class*)i_this->sub_method, i_this);
    }
    return TRUE;
}

/* 025DAA44 */
static s32 fopKy_IsDelete(kankyo_class* i_this) {
    s32 result = fpcMtd_IsDelete((process_method_class*)(kankyo_method_class*)i_this->sub_method, i_this);
    if (result == TRUE) {
        fopDwTg_DrawQTo(&i_this->draw_tag);
    }
    return result;
}

/* 025DAA98 */
static s32 fopKy_Delete(kankyo_class* i_this) {
    s32 result = fpcMtd_Delete((process_method_class*)(kankyo_method_class*)i_this->sub_method, i_this);
    fopDwTg_DrawQTo(&i_this->draw_tag);
    return result;
}

/* 025DAAE4 */
static s32 fopKy_Create(kankyo_class* i_this) {
    if (i_this->base.mInitState == 0) {
        kankyo_process_profile_definition* profile = (kankyo_process_profile_definition*)(process_profile_definition*)i_this->base.mpProf;
        i_this->mBsType = fpcBs_MakeOfType(&fopKy_KANKYO_TYPE);
        i_this->sub_method = (kankyo_method_class*)profile->sub_method;
        fopDwTg_Init(&i_this->draw_tag, i_this);
        fopKyM_prm_class* append = (fopKyM_prm_class*)(void*)i_this->base.mpUserData;
        if (append != NULL) {
            i_this->mPos = append->mPos;
            i_this->mScale = append->mScale;
            i_this->mParam = (u32)append->mParam;
        }
    }
    s32 result = fpcMtd_Create((process_method_class*)(kankyo_method_class*)i_this->sub_method, i_this);
    if (result == cPhs_COMPLEATE_e) {
        s32 priority = fpcM_DrawPriority(i_this);
        fopDwTg_ToDrawQ(&i_this->draw_tag, priority);
    }
    return result;
}

/* 025DABBC */
// HD: this translation unit's header statics initializer.
static void __sinit_f_op_kankyo_cpp() {
    sinit_header_statics(0x1048A48C, 0x101F33E4);
}

/* WWHD glue (tools/verify/autoglue.py): data and local GameCube forms
 data fopKy_KANKYO_TYPE 101F3408
 ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
 ext 025DD268 s32 fpcBs_MakeOfType(be<s32>*)
 ext 025986BC s32 dMenu_flag()
 ext 025AF2A4 s32 dScnPly_isPause()
 ext 025DF2C0 s32 fpcLf_DrawMethod(leafdraw_method_class*, void*)
 ext 025DF2B8 s32 fpcM_DrawPriority(void*)
 ext 025DA888 void fopDwTg_Init(create_tag_class*, void*)
 ext 025DA884 void fopDwTg_DrawQTo(create_tag_class*)
 ext 025DA874 void fopDwTg_ToDrawQ(create_tag_class*, s32)
*/
