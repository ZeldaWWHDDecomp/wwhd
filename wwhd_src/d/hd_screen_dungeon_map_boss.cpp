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
        gabi::FrameLocal<HdPadPoint> point(8);
        gabi::FrameLocal<HdPadPoint> origin(0x10);
        point->x = fmuls_ppc(x, scale);
        point->y = fmuls_ppc(y, scale);
        origin->x = zero;
        origin->y = zero;
        hdPadRotatePoint(point, point, origin, map->mAngle);
        // HD: the recompiled original keeps a
        // signalling NaN's bits through `lfs f13; lfs f0; stfs f13; stfs f0`, so the position and the origin are
        // copied as raw words here (a typed float copy quietens it).
        gabi::store<u32>(gabi::ea(&self->mPosition.x), gabi::load<u32>(gabi::ea(&point->x)));
        gabi::store<u32>(gabi::ea(&self->mPosition.y), gabi::load<u32>(gabi::ea(&point->y)));
        map = self->mMap;
        gabi::store<u32>(gabi::ea(&self->mOrigin.x), gabi::load<u32>(gabi::ea(&map->mOrigin.x)));
        gabi::store<u32>(gabi::ea(&self->mOrigin.y), gabi::load<u32>(gabi::ea(&map->mOrigin.y)));
    }
    hdPadStateExecute(&self->mState);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdBossUi 101F8344
data l_hdBossZero 100F0A88
data l_bossPositions 104926EC
*/
