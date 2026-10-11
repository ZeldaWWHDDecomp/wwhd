/** d_mesg_controller.cpp — message text controller.
 * See tww d/d_mesg.cpp.
 */
#include "d/d_mesg_controller.h"

static s16 initialFontSize;
static f32 initialLineLength;

/* 025F7424 */
dMesg_tControl* dMesg_tControl_ct(dMesg_tControl* self) {
    // HD: GHS constructor allocation path and explicit base construction.
    if (!self) {
        self = static_cast<dMesg_tControl*>(operator_new(sizeof(dMesg_tControl)));
        if (!self)
            return nullptr;
    }
    JMessage_TControl_ct(self);
    self->mMainFont = nullptr;
    self->mRubyFont = nullptr; // HD: initializes the ruby font as well.
    self->__vtbl = dMesg_tControl::__vt;
    self->mLineCount = 0;
    self->mLineStart = 0;
    self->mInitFontSize = initialFontSize;
    f32 length = initialLineLength;
    self->mNowFontSize = initialFontSize;
    self->mLineLength[0] = length;
    self->mTextBoxWidth = 486;
    self->mLineLength[1] = length;
    self->mCode16Fg = 0;
    self->mLineLength[2] = length;
    self->mCharCode = 0;
    self->mCharSpace = 0;
    self->mbHeader = 0;
    self->mLineLength[3] = length;
    return self;
}

/* 025F74D0 */
void dMesg_tControl::setMesgStatus(u32 status) {
    // HD: status belongs to the play object; this method ignores its controller.
    reinterpret_cast<dMesg_play_view*>(dComIfGp_get())->mMesgStatus = status;
}

/* 025F9070 */
void dMesg_tControl_dt(dMesg_tControl* self, u32 flags) {
    if (self) {
        JMessage_TControl_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data initialFontSize 1047AFA0
data initialLineLength 100E0C4C
ext 028427B8 void JMessage_TControl_ct(JMessage::TControl* self)
ext 02842828 void JMessage_TControl_dt(JMessage::TControl* self, u32 flags)
*/
