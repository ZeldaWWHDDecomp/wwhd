#include "d/hd_msg_window_views.h"
using namespace gabi;
extern void hdMsgGetPanePosition(HdMsgPosition*,HdMsgPane*);
extern f32 hdMsgGetPaneWidth(HdMsgPane*);
static f32 l_cursorHalf;
static f32 l_cursorInset;
/* 0267341C */
void hdMsgCursorPosition(HdMsgKeyboard* self, HdMsgPosition* output) {
    // HD: the pane helper writes the original stack slot, including its third coordinate.
    gabi::NativeFrame<0x28> frame;
    HdMsgPosition* position = gabi::at<HdMsgPosition>(frame + 8);
    hdMsgGetPanePosition(position, self->mPane);
    const f32 x = position->x;
    const f32 y = position->y;
    output->x = x;
    const f32 z = position->z;
    output->y = y;
    output->z = z;
    const f32 half = l_cursorHalf;
    const f32 width = hdMsgGetPaneWidth(self->mPane);
    output->x = fmadds(width, half, output->x);
    HdMsgPane* pane = self->mPane;
    const f32 inset = l_cursorInset;
    const f32 offset = fmsubs(pane->mHeight, half, inset);
    output->y = fsubs_ppc(output->y, offset);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_cursorHalf 100F6A20
data l_cursorInset 100F6A24
ext 02704D2C void hdMsgGetPanePosition(HdMsgPosition*,HdMsgPane*)
ext 02704A08 f32 hdMsgGetPaneWidth(HdMsgPane*)
*/
