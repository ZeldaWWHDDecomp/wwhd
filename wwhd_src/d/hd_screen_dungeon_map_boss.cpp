#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static HdPadUiPointer l_hdBossUi;
static f32 l_hdBossZero;
static HdPadBossPositions l_bossPositions;

/* 0265FCB8 */
void hdBossUpdate(HdPadBoss *self) {
    // HD: retain original frame offsets and uninitialized callback storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x20> frame;
    HdPadMap *map = self->mMap;
    HdPadPane *pane;
    if (map && (pane = self->mPane) && map->mBossVisible) {
        HdPadUi *ui = l_hdBossUi.mOwner->mUi;
        s32 index = s32(ui->mBossIndex);
        f32 zero = l_hdBossZero, x = zero, y = zero;
        // HD: boss position indexing retains wrapping guest-address arithmetic.
        if (index >= 0) {
            HdPadPoint *boss = gabi::at<HdPadPoint>(gabi::ea(&l_bossPositions) + u32(index) * 8);
            x = boss->x;
            y = boss->y;
        }
        f32 scale = pane->mScale;
        HdPadPoint *point = gabi::at<HdPadPoint>(frame + 8);
        HdPadPoint *origin = gabi::at<HdPadPoint>(frame + 0x10);
        point->x = fmuls_ppc(x, scale);
        point->y = fmuls_ppc(y, scale);
        origin->x = zero;
        origin->y = zero;
        hdPadRotatePoint(point, point, origin, map->mAngle);
        self->mPosition.x = point->x;
        self->mPosition.y = point->y;
        map = self->mMap;
        self->mOrigin.x = map->mOrigin.x;
        self->mOrigin.y = map->mOrigin.y;
    }
    hdPadStateExecute(&self->mState);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdBossUi 101F8344
data l_hdBossZero 100F0A88
data l_bossPositions 104926EC
*/
