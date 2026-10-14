#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static HdPadUiPointer l_hdMapUi;
static f32 l_hdMapOne;
static f32 l_hdMapChaseRate;
static f32 l_hdMapChaseMax;
static HdPadState l_mapWaitState;
static HdPadState l_mapActiveState;

/* 0267C34C */
void hdMapZoom(HdPadMap *self) {
    f32 t = hdPadZoomRatio(self, u32(self->mZoomMode), self->mZoomStart, self->mScale);
    f32 scale = self->mScale, x = -fmuls_ppc(self->mTarget.x, scale),
        y = -fmuls_ppc(self->mTarget.y, scale);
    self->mTarget.x = x;
    self->mTarget.y = y;
    hdPadZoomPosition(self, &self->mCurrent, &self->mTarget, u32(self->mZoomMode));
    hdPadZoomScale(self, u32(self->mZoomMode), self->mZoomX, self->mZoomY, t);
    call_ptr<void>(self->mVTable[HD_PAD_ZOOM].get(), self);
    if (!(l_hdMapOne > t)) {
        HdPadUi *map = l_hdMapUi.mOwner->mUi;
        u32 mode = hdPadMapMode(map->mMapMode);
        hdPadChangeState(&self->mState, mode ? &l_mapActiveState : &l_mapWaitState);
    }
    call_ptr<void>(self->mVTable[HD_PAD_UPDATE].get(), self);
}
/* 0267C6C4 */
void hdMapSetPosition(HdPadMap *self, HdPadPoint *position, u32 index) {
    // HD: retain original frame offsets and uninitialized callback storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    HdPadMoveCell *cell = call_ptr<HdPadMoveCell *>(self->mVTable[HD_PAD_CELL].get(), self, index);
    if (!cell)
        return;
    cell->mPosition.x = position->x;
    cell->mPosition.y = position->y;
    gabi::FrameLocal<HdPadPoint> target(8);
    gabi::FrameLocal<HdPadPoint> current(0x10);
    f32 scale = self->mScale;
    target->x = -fmuls_ppc(position->x, scale);
    target->y = -fmuls_ppc(position->y, scale);
    current->x = cell->mCurrent.x;
    current->y = cell->mCurrent.y;
    hdMapDisplayChase(current, target, l_hdMapChaseRate, l_hdMapChaseMax, l_hdMapOne);
    cell->mCurrent.x = target->x;
    cell->mCurrent.y = target->y;
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdMapUi 101F8344
data l_hdMapOne 100F8D18
data l_hdMapChaseRate 100F8D30
data l_hdMapChaseMax 100F8D34
data l_mapWaitState 10496060
data l_mapActiveState 104960F0
*/
