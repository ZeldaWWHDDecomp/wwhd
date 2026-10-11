/* Camera framework, tww fields after the HD process vtable (+4 bytes). */
#pragma once
#include "f_op/f_op_view.h"

struct camera_class {
    /* 0x000 */ view_class view;
    // Only the merged view prefix is needed here; its remaining matrices/viewport are opaque.
    u8 padView[0x214 - sizeof(view_class)];
    /* 0x214 */ create_tag_class draw_tag;
    /* 0x228 */ gptr<leafdraw_method_class> mpMtd;
    /* 0x22C */ u8 field_0x228[4];
    /* 0x230 */ be<s8> mPrm1;
    /* 0x231 */ be<s8> mPrm2;
    /* 0x232 */ be<s8> mPrm3;
    /* 0x233 */ be<s8> field_0x22f;
    /* 0x234 */ csXyz mAngle;
    u8 pad23A[2];
};
WWHD_OFFSET(camera_class, view, 0);
WWHD_OFFSET(camera_class, draw_tag, 0x214);
WWHD_OFFSET(camera_class, mpMtd, 0x228);
WWHD_OFFSET(camera_class, mPrm1, 0x230);
WWHD_OFFSET(camera_class, mAngle, 0x234);
WWHD_SIZE(camera_class, 0x23C);

struct camera_process_profile_definition {
    /* 0x00 */ view_process_profile_definition base;
    u8 padBase[0x3C - sizeof(view_process_profile_definition)];
    /* 0x3C */ gptr<leafdraw_method_class> sub_method;
    /* 0x40 */ be<u32> unk40;
};
WWHD_OFFSET(camera_process_profile_definition, sub_method, 0x3C);
WWHD_SIZE(camera_process_profile_definition, 0x44);
struct fopCamM_prm_class {
    // The camera only reads the first parameter word of the tww append base.
    struct {
        be<u32> parameters;
        u8 pad4[0x18 - 4];
    } base;
};
WWHD_SIZE(fopCamM_prm_class, 0x18);
WWHD_OFFSET(fopCamM_prm_class, base, 0);

s32 dMenu_flag();
s32 dScnPly_isPauseHD();
class dScnPly_ply_c {
  public:
    // HD: free adapter for a static method; no hidden this register.
    static s32 isPause() { return dScnPly_isPauseHD(); }
};
s32 fpcM_DrawPriority(leafdraw_class*);
void fopDwTg_DrawQTo(create_tag_class*);
void fopDwTg_InitHD(create_tag_class*, void*);
void fopDwTg_ToDrawQHD(create_tag_class*, s32);

// HD: the caller retains these full register arguments (GC narrows the process name).
struct layer_class;
layer_class* fopCamM_CurrentLayerHD(u32);
fpc_ProcID fopCamM_RequestHD(layer_class*, u32, u32, u32, void*);

struct fopCamM_IDTableHD {
    be<fpc_ProcID> mID[4];
};
WWHD_SIZE(fopCamM_IDTableHD, 0x10);
