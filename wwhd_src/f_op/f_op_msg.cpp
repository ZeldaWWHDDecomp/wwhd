/** f_op_msg.cpp — message process lifecycle.
 * Based on tww f_op/f_op_msg.cpp.
 */
#include "f_op/f_op_msg.h"

static s32 fopMsg_MSG_TYPE;

/* 025DB050 */
int fopMsg_Draw(void* i_this) {
    fopMsgLifecycle::msg_class* self = static_cast<fopMsgLifecycle::msg_class*>(i_this);
    return fpcLf_DrawMethod(self->sub_method, self);
}

/* 025DB05C */
int fopMsg_Execute(void* i_this) {
    fopMsgLifecycle::msg_class* self = static_cast<fopMsgLifecycle::msg_class*>(i_this);
    int status = 1;
    if (!dScnPly_ply_c::isPause())
        status = fpcMtd_Execute(self->sub_method, self);
    return status;
}

/* 025DB0B0 */
int fopMsg_IsDelete(void* i_this) {
    fopMsgLifecycle::msg_class* self = static_cast<fopMsgLifecycle::msg_class*>(i_this);
    int status = fpcMtd_IsDelete(self->sub_method, self);
    if (status == 1)
        fopDwTg_DrawQTo(&self->draw_tag);
    return status;
}

/* 025DB104 */
int fopMsg_Delete(void* i_this) {
    fopMsgLifecycle::msg_class* self = static_cast<fopMsgLifecycle::msg_class*>(i_this);
    int status = fpcMtd_Delete(self->sub_method, self);
    fopDwTg_DrawQTo(&self->draw_tag);
    return status;
}

/* 025DB150 */
cPhs_State fopMsg_Create(void* i_this) {
    fopMsgLifecycle::msg_class* self = static_cast<fopMsgLifecycle::msg_class*>(i_this);
    if (self->mInitState == 0) {
        fopMsgLifecycle::msg_process_profile_definition* profile =
            reinterpret_cast<fopMsgLifecycle::msg_process_profile_definition*>(self->mpProf.get());
        self->mMsgType = fpcBs_MakeOfType(&fopMsg_MSG_TYPE);
        self->sub_method = profile->sub_method;
        fopDwTg_Init(&self->draw_tag, self);
        fopMsgLifecycle::fopMsg_prm_class* prm = static_cast<fopMsgLifecycle::fopMsg_prm_class*>(self->mpUserData.get());
        if (prm) {
            // HD: integer word copies retain signaling NaNs and the original copy order.
            self->mpActor = prm->mpActor;
            self->mPosWords.x = prm->mPosWords.x;
            self->mPosWords.y = prm->mPosWords.y;
            self->mPosWords.z = prm->mPosWords.z;
            self->mMsgNo = prm->mMsgNo;
            self->field_0xf0 = prm->field_0x14;
            self->field_0xf4 = prm->field_0x18;
        }
    }
    cPhs_State status = static_cast<cPhs_State>(fpcMtd_Create(self->sub_method, self));
    if (status == cPhs_COMPLEATE_e) {
        s32 priority = fpcM_DrawPriority(self);
        fopDwTg_ToDrawQ(&self->draw_tag, priority);
    }
    return status;
}

/* 025DB228 */
static void __sinit_f_op_msg_cpp() {
    sinit_header_statics(0x1048A4C4, 0x101F3444);
}

/* WWHD glue (tools/verify/autoglue.py)
data fopMsg_MSG_TYPE 101F3468
ext 025DF2C0 int fpcLf_DrawMethod(process_method_class* methods, void* process)
ext 025DF2B8 s32 fpcM_DrawPriority(leafdraw_class* process)
ext 025DD268 u32 fpcBs_MakeOfType(be<s32>* type)
ext 025DA884 void fopDwTg_DrawQTo(create_tag_class* tag)
ext 025DA888 void fopDwTg_Init(create_tag_class* tag, void* data)
ext 025DA874 void fopDwTg_ToDrawQ(create_tag_class* tag, s32 priority)
ext 025AF2A4 s32 dScnPly_isPause()
*/
