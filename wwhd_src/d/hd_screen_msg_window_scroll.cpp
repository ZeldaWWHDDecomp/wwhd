#include "d/hd_msg_window_views.h"
using namespace gabi;
extern void hdMsgSetLine(HdMsgScroll*,u32);
extern bool hdMsgPaneAnimationEnded(void*,u32);
extern f32 hdMsgOpeningAlpha(HdMsgScroll*,u32,u32);
extern f32 hdMsgClosingAlpha(HdMsgScroll*,u32,u32);
extern HdMsgPlay* hdMsgGetPlay();
extern void hdMsgPositionLines(HdMsgScroll*);
extern void hdMsgSetControlMode(void*,u32);
static HdMsgState l_scrollEmptyState;
static HdMsgState l_scrollTextState;
static HdMsgState l_scrollWaitState;
static HdMsgState l_scrollClosedState;
static f32 l_scrollSpeed;
static HdMsgControlPointer l_controlManager;
/* 026B9BD8 */
void hdMsgSetLines(HdMsgScroll* self, const char* text) {
    // HD: invalid line and pane indices select slot zero, including wrapped indices.
    u32 index = self->mLineIndex;
    self->mLineCount = 1;
    self->mLines[index < 20 ? index : 0] = text;
    hdMsgSetLine(self, 0u);
    index = self->mPaneIndices[0];
    self->mPanes[index < 3 ? index : 0]->mAlpha = 255;
    if (self->mMessage->mMode != HD_MSG_CONTINUE) return;
    index = self->mLineIndex + 1;
    const u32 count = self->mLineCount + 1;
    HdMsgMessage* message = self->mMessage;
    self->mLineCount = count;
    self->mLines[index < 20 ? index : 0] = message->mNextText;
    hdMsgSetLine(self, 1u);
    index = self->mPaneIndices[1];
    self->mPanes[index < 3 ? index : 0]->mAlpha = 100;
    if (self->mMessage->mMode != HD_MSG_CONTINUE) return;
    index = self->mLineIndex + 2;
    const u32 count3 = self->mLineCount + 1;
    HdMsgMessage* message3 = self->mMessage;
    self->mLineCount = count3;
    self->mLines[index < 20 ? index : 0] = message3->mNextText;
    hdMsgSetLine(self, 2u);
    index = self->mPaneIndices[2];
    self->mPanes[index < 3 ? index : 0]->mAlpha = 100;
}
/* 026BA0B4 */
bool hdMsgAnimationEnded(HdMsgScroll* self, u32 animation) {
    return hdMsgPaneAnimationEnded(self->mAnimation->mPlayer, animation);
}
/* 026BA308 */
void hdMsgOpeningMove(HdMsgScroll* self) {
    if (hdMsgAnimationEnded(self, 0u)) {
        HdMsgState* state = self->mOpeningMode == 0xFFFFFFFFu ? &l_scrollEmptyState : &l_scrollTextState;
        hdMsgChangeState(&self->mState, state);
    }
    const f32 value = hdMsgOpeningAlpha(self, 5u, self->mTimer);
    const u8 alpha = u8(ftoi(value));
    HdMsgPlay* play = hdMsgGetPlay();
    play->mWindowAlpha = alpha;
    self->mTimer = self->mTimer + 1;
}
/* 026BAA30 */
void hdMsgScrollUp(HdMsgScroll* self) {
    f32 position = fadds_ppc(self->mPosition, l_scrollSpeed);
    if (position > self->mUpperLimit) {
        for (u32 i = 0; i < 3; ++i) {
            HdMsgPane* pane = self->mPanes[i];
            pane->mFlags = pane->mFlags & 0xFE;
        }
        const u32 line = self->mLineIndex + 1;
        const f32 target = self->mTarget;
        const f32 spacing = self->mLineSpacing;
        self->mLineIndex = line;
        position = fsubs_ppc(position, spacing);
        self->mTarget = fsubs_ppc(target, spacing);
        hdMsgSetLines(self, self->mLines[line < 20 ? line : 0]);
    } else {
        const u32 line = self->mLineIndex;
        hdMsgSetLines(self, self->mLines[line < 20 ? line : 0]);
    }
    const f32 target = self->mTarget;
    if (position > target) {
        position = target;
        hdMsgChangeState(&self->mState, &l_scrollWaitState);
    }
    self->mPosition = position;
    hdMsgPositionLines(self);
}
/* 026BAC00 */
void hdMsgScrollDown(HdMsgScroll* self) {
    f32 position = fsubs_ppc(self->mPosition, l_scrollSpeed);
    if (position < self->mLowerLimit) {
        for (u32 i = 0; i < 3; ++i) {
            HdMsgPane* pane = self->mPanes[i];
            pane->mFlags = pane->mFlags & 0xFE;
        }
        u32 line = self->mLineIndex;
        if (s32(line) > 0) {
            const f32 spacing = self->mLineSpacing;
            const f32 target = self->mTarget;
            --line;
            self->mLineIndex = line;
            position = fadds_ppc(position, spacing);
            self->mTarget = fadds_ppc(target, spacing);
        }
        hdMsgSetLines(self, self->mLines[line < 20 ? line : 0]);
    } else {
        const u32 line = self->mLineIndex;
        hdMsgSetLines(self, self->mLines[line < 20 ? line : 0]);
    }
    const f32 target = self->mTarget;
    if (position < target) {
        position = target;
        hdMsgChangeState(&self->mState, &l_scrollWaitState);
    }
    self->mPosition = position;
    hdMsgPositionLines(self);
}
/* 026BB1D8 */
void hdMsgClosingMove(HdMsgScroll* self) {
    if (hdMsgAnimationEnded(self, 1u)) {
        hdMsgSetControlMode(l_controlManager.mManager, 18u);
        hdMsgChangeState(&self->mState, &l_scrollClosedState);
    }
    const f32 value = hdMsgClosingAlpha(self, 5u, self->mTimer);
    const u8 alpha = u8(ftoi(value));
    HdMsgPlay* play = hdMsgGetPlay();
    play->mWindowAlpha = alpha;
    const u32 timer = self->mTimer;
    if (timer) self->mTimer = timer - 1;
}

/* WWHD glue (tools/verify/autoglue.py)
data l_scrollEmptyState 10498CD8
data l_scrollTextState 10498BE8
data l_scrollWaitState 10498CA8
data l_scrollClosedState 10498D68
data l_scrollSpeed 100FF2C0
data l_controlManager 101F4B5C
ext 020063C0 void hdMsgChangeState(HdMsgController*,HdMsgState*)
ext 026B9AF0 void hdMsgSetLine(HdMsgScroll*,u32)
ext 02005840 bool hdMsgPaneAnimationEnded(void*,u32)
ext 026BA1A4 f32 hdMsgOpeningAlpha(HdMsgScroll*,u32,u32)
ext 026BB088 f32 hdMsgClosingAlpha(HdMsgScroll*,u32,u32)
ext 025200D4 HdMsgPlay* hdMsgGetPlay()
ext 026B9E1C void hdMsgPositionLines(HdMsgScroll*)
ext 025F74D0 void hdMsgSetControlMode(void*,u32)
*/
