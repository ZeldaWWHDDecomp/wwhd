/* Message process lifecycle, with tww names and HD layout (+4 after leafdraw).
 */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"

// HD: isolate the lifecycle type from the legacy NPC message-manager view.
namespace fopMsgLifecycle {
struct position_words { be<u32> x, y, z; };
struct msg_class : leafdraw_class {
    /* 0xC4 */ be<s32> mMsgType;
    /* 0xC8 */ create_tag_class draw_tag;
    /* 0xDC */ gptr<process_method_class> sub_method;
    /* 0xE0 */ gptr<fopAc_ac_c> mpActor;
    /* 0xE4 */ union { cXyz mPos; position_words mPosWords; };
    /* 0xF0 */ be<u32> mMsgNo;
    /* 0xF4 */ be<u32> field_0xf0;
    /* 0xF8 */ be<u32> field_0xf4;
    /* 0xFC */ be<u16> mStatus;
    /* 0xFE */ be<u8> mSelectNum;
    /* 0xFF */ u8 _0FF;
};
WWHD_OFFSET(msg_class, mMsgType, 0xC4);
WWHD_OFFSET(msg_class, draw_tag, 0xC8);
WWHD_OFFSET(msg_class, sub_method, 0xDC);
WWHD_OFFSET(msg_class, mpActor, 0xE0);
WWHD_SIZE(msg_class, 0x100);
struct msg_process_profile_definition : leaf_process_profile_definition {
    /* 0x24 */ gptr<process_method_class> sub_method;
};
WWHD_OFFSET(msg_process_profile_definition, sub_method, 0x24);
struct fopMsg_prm_class {
    /* 0x00 */ gptr<fopAc_ac_c> mpActor;
    /* 0x04 */ union { cXyz mPos; position_words mPosWords; };
    /* 0x10 */ be<u32> mMsgNo;
    /* 0x14 */ be<u32> field_0x14;
    /* 0x18 */ be<u32> field_0x18;
};
WWHD_SIZE(fopMsg_prm_class, 0x1C);
} // namespace fopMsgLifecycle
int fpcLf_DrawMethod(process_method_class* methods, void* process);
s32 fpcM_DrawPriority(leafdraw_class* process);
u32 fpcBs_MakeOfType(be<s32>* type);
void fopDwTg_DrawQTo(create_tag_class* tag);
void fopDwTg_Init(create_tag_class* tag, void* data);
void fopDwTg_ToDrawQ(create_tag_class* tag, s32 priority);
// HD: free ABI bridge for the static method (no hidden this argument).
s32 dScnPly_isPause();
struct dScnPly_ply_c { static s32 isPause() { return dScnPly_isPause(); } };
