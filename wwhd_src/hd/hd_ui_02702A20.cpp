/* HD message states. */
#include "d/hd_msg_window_base.h"
using namespace gabi;
static WindowSavePointer l_windowSave;
static u32 l_windowProgressFlags;
extern WindowPlay *hdMsgProgressGetPlay();
extern void windowRefreshProgress(WindowProgress *);

/* 02702D50 */
void hdMsgProgress_updateProgress(WindowProgress *self) {
    WindowSave *save = l_windowSave.get();
    u32 flags = l_windowProgressFlags;
    u32 target = save->mProgressTarget;
    u32 old = self->mTarget;
    if (flags & 0x40000)
        target = 32;
    bool changed = old != target;
    if (changed)
        self->mTarget = target;
    WindowPlay *play = hdMsgProgressGetPlay();
    u32 actual = u32(s32(play->mProgress));
    u32 shown = self->mShown;
    if (actual > shown) {
        self->mShown = shown + 1;
        windowRefreshProgress(self);
    } else if (actual < shown) {
        self->mShown = shown - 1;
        windowRefreshProgress(self);
    } else if (changed)
        windowRefreshProgress(self);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_windowSave 101F84DC
data l_windowProgressFlags 1047B074
ext 025200D4 WindowPlay* hdMsgProgressGetPlay()
ext 02702BB8 void windowRefreshProgress(WindowProgress*)
*/
