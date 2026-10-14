#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static HdPadUiPointer l_hdDungeonUi;
static HdPadEventPointer l_hdDungeonInput;
static HdPadStickPointer l_hdDungeonStick;
static HdPadActivePointer l_hdDungeonActive;
static f32 l_hdDungeonZero;
static f32 l_hdDungeonHalf;
static f32 l_hdDungeonThree;
static f32 l_hdDungeonDragScale;
static HdPadState l_dungeonWaitState;

static f32 clamp(f32 x, f32 lo, f32 hi) {
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}
/* 0265A3BC */
void hdDungeonBounds(HdPadDungeon *self, HdPadRect *output) {
    // HD: retain original frame offsets and uninitialized callback storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    HdPadPane *pane = self->mPane;
    if (!pane)
        return;
    gabi::FrameLocal<HdPadPoint3> origin(0x40);
    hdPadPaneOrigin(origin, pane);
    f32 x = origin->x, y = origin->y;
    HdPadLayout *layout = self->mAnimation->mLayout;
    f32 screenX = layout->mWidth, screenY = layout->mHeight;
    f32 paneX = pane->mWidth, paneY = pane->mHeight;
    f32 width = f32(f64(hdPadDisplayWidth()));
    f32 height = f32(f64(hdPadDisplayHeight()));
    f32 ratioX = fdivs(width, screenX), ratioY = fdivs(height, screenY), half = l_hdDungeonHalf,
        zero = l_hdDungeonZero;
    x = fmuls_ppc(x, ratioX);
    y = fmuls_ppc(y, ratioY);
    paneX = fmuls_ppc(paneX, ratioX);
    paneY = fmuls_ppc(paneY, ratioY);
    f32 centerX = fadds_ppc(x, fmuls_ppc(width, half)), centerY = fmadds(height, half, y);
    f32 hx = fmuls_ppc(paneX, half), hy = fmuls_ppc(paneY, half);
    f32 left = clamp(fsubs_ppc(centerX, hx), zero, width),
        top = clamp(fsubs_ppc(centerY, hy), zero, height);
    f32 right = clamp(fadds_ppc(centerX, hx), zero, width),
        bottom = clamp(fadds_ppc(centerY, hy), zero, height);
    output->mTop = top;
    output->mBottom = bottom;
    output->mRight = right;
    output->mLeft = left;
}
/* 02657790 */
void hdDungeonUpdateParts(HdPadDungeon *self) {
    hdPadDungeonUpdateLayout(self);
    call_ptr<void>(self->mVTable[HD_PAD_DRAW].get(), self);
    call_ptr<void>(self->mVTable[HD_PAD_UPDATE].get(), self);
    hdPadDungeonUpdateIcons(self);
    hdPadDungeonUpdateCursor(self);
    hdPadDungeonUpdatePane(self);
}
/* 0265799C */
void hdDungeonUpdateMarkers(HdPadDungeon *self) {
    HdPadUi *ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui) && self->mMarkersActive) {
        hdPadDungeonUpdateBoss(self);
        hdPadDungeonUpdatePlayer(self);
    }
}
/* 0265ADB4 */
void hdDungeonDrag(HdPadDungeon *self) {
    // HD: retain original frame offsets and uninitialized callback storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self->mMap) {
        hdPadChangeState(&self->mState, &l_dungeonWaitState);
        return;
    }
    gabi::FrameLocal<HdPadEvent> event(0x20);
    HdPadEventSource *input = l_hdDungeonInput.mInput;
    HdPadPoint *stick = &l_hdDungeonStick.mStick->mPosition;
    hdPadCopyEvent(event, &input->mEvent);
    if (event->mKind == HD_PAD_EVENT_DRAG && event->mId == self->mInputMode->mId) {
        gabi::FrameLocal<HdPadPoint> position(0x10);
        Pair32 coordinates{hdPadInputPosition(l_hdDungeonInput.mInput)};
        HdPadFloatBits xBits{coordinates.r3};
        HdPadFloatBits yBits{coordinates.r4};
        position->x = xBits.mValue;
        position->y = yBits.mValue;
        hdPadMapDrag(self->mMap, position);
    } else {
        f32 y = stick->y, x = stick->x, zero = l_hdDungeonZero;
        f32 squared = fmadds(x, x, fmuls_ppc(y, y)), length;
        if (squared > zero) {
            f64 inv = frsqrte(squared);
            f32 correction = fnmsubs(f32(inv * round25(inv)), squared, l_hdDungeonThree);
            length = fmuls_ppc(fmuls_ppc(correction, f32(inv * f64(l_hdDungeonHalf))), squared);
        } else
            length = fmuls_ppc(zero, squared);
        bool active = length > zero;
        if (active)
            active = l_hdDungeonActive.mInput->mActive != 0;
        if (active)
            active = hdPadGetInputMode(l_hdDungeonInput.mInput) != 0;
        if (active) {
            HdPadInputMode *mode = hdPadGetInputMode(l_hdDungeonInput.mInput);
            active = mode->mId == self->mInputMode->mId;
        }
        if (!active)
            hdPadChangeState(&self->mState, &l_dungeonWaitState);
        else {
            f32 factor = l_hdDungeonDragScale, dx = fmuls_ppc(stick->x, factor),
                dy = fmuls_ppc(stick->y, factor);
            f32 oldX = self->mDragPosition.x, oldY = self->mDragPosition.y;
            self->mDragPosition.x = fsubs_ppc(oldX, dx);
            self->mDragPosition.y = fsubs_ppc(oldY, dy);
            hdPadMapDrag(self->mMap, &self->mDragPosition);
        }
    }
    hdDungeonUpdateParts(self);
    hdDungeonUpdateMarkers(self);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdDungeonUi 101F8344
data l_hdDungeonInput 101F4FF0
data l_hdDungeonStick 101F5088
data l_hdDungeonActive 101F8378
data l_hdDungeonZero 100F0250
data l_hdDungeonHalf 100F0254
data l_hdDungeonThree 100F0258
data l_hdDungeonDragScale 100F0334
data l_dungeonWaitState 10492410
ext 02704DE4 void hdPadPaneOrigin(HdPadPoint3*,HdPadPane*)
ext 02739594 u32 hdPadDisplayWidth()
ext 027395E4 u32 hdPadDisplayHeight()
ext 02656830 void hdPadDungeonUpdateLayout(HdPadDungeon*)
ext 026568CC void hdPadDungeonUpdateIcons(HdPadDungeon*)
ext 026573F0 void hdPadDungeonUpdateCursor(HdPadDungeon*)
ext 02657440 void hdPadDungeonUpdatePane(HdPadDungeon*)
ext 0268936C u32 hdPadUiBusy(HdPadUi*)
ext 02657848 void hdPadDungeonUpdateBoss(HdPadDungeon*)
ext 02657914 void hdPadDungeonUpdatePlayer(HdPadDungeon*)
ext 020063C0 void hdPadChangeState(HdPadStateController*,HdPadState*)
ext 020013C0 void hdPadCopyEvent(HdPadEvent*,HdPadEvent*)
ext 026151A8 Pair32 hdPadInputPosition(HdPadEventSource*)
ext 0267D5D8 void hdPadMapDrag(HdPadMap*,HdPadPoint*)
ext 02615140 HdPadInputMode* hdPadGetInputMode(HdPadEventSource*)
ext 026893C8 void hdPadRotatePoint(HdPadPoint*,HdPadPoint*,HdPadPoint*,f32)
ext 02006364 void hdPadStateExecute(HdPadStateController*)
ext 0267BF30 f32 hdPadZoomRatio(HdPadMap*,u32,f32,f32)
ext 0267C068 void hdPadZoomPosition(HdPadMap*,HdPadPoint*,HdPadPoint*,u32)
ext 0267C188 void hdPadZoomScale(HdPadMap*,u32,f32,f32,f32)
ext 02655CE8 u32 hdPadMapMode(void*)
ext 02680E98 void hdPadGridSetup(HdPadGrid*)
ext 02680FEC void hdPadGridSetupPanes(HdPadGrid*)
ext 02704EA8 Pair32 hdPadPaneScale(HdPadPane*)
ext 025B8568 u32 hdPadMapDiscovered(void*,u32)
ext 02639C54 void hdPadIconsOpening(void*)
ext 02639E4C void hdPadIconsWaitInit(void*)
ext 02639E50 void hdPadIconsWaiting(void*)
ext 02639EE0 void hdPadIconsClosing(void*)
ext 02639CF0 void hdPadIconsFinish(void*)
ext 02030B38 void hdPadUiSound(void*)
ext 025E1918 void hdPadPlaySound(u32)
ext 02005840 u32 hdPadAnimationEnded(void*,u32)
ext 02639C44 u32 hdPadIconsEnded(void*)
ext 02680610 u32 hdPadCellEnded(HdPadMapCell*)
ext 02679D68 void hdPadMapBeginDraw(HdPadMapOwner*)
ext 02679D78 void hdPadMapEndDraw(HdPadMapOwner*)
*/
