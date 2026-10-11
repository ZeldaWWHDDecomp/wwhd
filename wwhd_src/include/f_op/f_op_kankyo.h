/** Environment process framework. */
#pragma once
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_xyz.h"

struct leafdraw_method_class;
struct kankyo_method_class;
struct kankyo_class {
    leafdraw_class base;
    // HD: base process vtable shifts GameCube's environment fields by four bytes.
    be<s32> mBsType;
    create_tag_class draw_tag;
    gptr<kankyo_method_class> sub_method;
    cXyz mPos;
    cXyz mScale;
    be<u32> mParam;
};
WWHD_OFFSET(kankyo_class, mBsType, 0xC4);
WWHD_OFFSET(kankyo_class, draw_tag, 0xC8);
WWHD_OFFSET(kankyo_class, sub_method, 0xDC);
WWHD_OFFSET(kankyo_class, mPos, 0xE0);
WWHD_OFFSET(kankyo_class, mScale, 0xEC);
WWHD_OFFSET(kankyo_class, mParam, 0xF8);
WWHD_SIZE(kankyo_class, 0xFC);

struct kankyo_process_profile_definition : leaf_process_profile_definition {
    gptr<kankyo_method_class> sub_method;
};
WWHD_OFFSET(kankyo_process_profile_definition, sub_method, 0x24);
struct fopKyM_prm_class {
    cXyz mPos;
    cXyz mScale;
    be<u32> mParam;
};
WWHD_SIZE(fopKyM_prm_class, 0x1C);

// HD: process numbers differ from GameCube's 0x17 and 0x1A.
enum : s16 { fpcNm_ENVSE_e = 21, fpcNm_LEVEL_SE_e = 24 };
// HD: local static-method adapter; autoglue ext stubs accept free functions.
s32 dScnPly_isPause();
class dScnPly_ply_c {
public:
    static s32 isPause() { return dScnPly_isPause(); }
};
s32 dMenu_flag();
/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
s32 fpcBs_MakeOfType(be<s32>* type);
s32 fpcLf_DrawMethod(leafdraw_method_class* method, void* process);
s32 fpcM_DrawPriority(void* process);
void fopDwTg_Init(create_tag_class* tag, void* process);
void fopDwTg_DrawQTo(create_tag_class* tag);
void fopDwTg_ToDrawQ(create_tag_class* tag, s32 priority);
