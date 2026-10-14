/** HD map pane visibility interpolation. Private reconstruction. */
#include "d/hd_screen_map_display_coordinator.h"
#include <cmath>
enum { PANE_VISIBLE = 1, PANE_MAX_ALPHA = 255 };
void hdMapChaseValue(be<f32>*, f32, f32);
f32 hdMapChaseScaled(be<f32>* value, f32 target, f32 rate, f32 maximum,
                     f32 minimum);
/* 026898F0 */
static void hdMapPaneShow(hdMapPaneView_l* pane) {
    if (!pane)
        return;
    u32 flags = pane->mFlags;
    pane->mAlpha = PANE_MAX_ALPHA;
    pane->mFlags = (flags & ~PANE_VISIBLE) + PANE_VISIBLE;
}
/* 02689914 */
static void hdMapPaneHide(hdMapPaneView_l* pane) {
    if (!pane)
        return;
    u32 flags = pane->mFlags;
    pane->mAlpha = 0;
    pane->mFlags = flags & ~PANE_VISIBLE;
}
/* 02689934 */
f32 hdMapChaseScaled(be<f32>* value, f32 target, f32 rate, f32 maximum,
                     f32 minimum) {
    f32 delta = gabi::fsubs_ppc(target, value->get());
    f32 step = std::fabs(gabi::fmuls_ppc(delta, rate));
    if (step < minimum)
        step = minimum;
    else if (step > maximum)
        step = maximum;
    hdMapChaseValue(value, target, step);
    return std::fabs(gabi::fsubs_ppc(target, value->get()));
}
/* 026899D4 */
static f32 hdMapPaneFadeIn(hdMapPaneView_l* pane) {
    f32 alpha = 0.0f;
    if (pane) {
        gabi::Local<be<f32>> value;
        alpha = f32(f64(u8(pane->mAlpha)) / f64(PANE_MAX_ALPHA));
        *value = alpha;
        if (!(1.0f <= alpha)) {
            hdMapChaseScaled(value.get(), 1.0f, 0.8f, 0.3f, 0.01f);
            alpha = *value;
            u32 flags = pane->mFlags;
            pane->mAlpha =
                u8(gabi::ftoi(gabi::fmuls_ppc(f32(PANE_MAX_ALPHA), alpha)));
            pane->mFlags = (flags & ~PANE_VISIBLE) + PANE_VISIBLE;
        }
    }
    return alpha;
}
/* 02689AB4 */
static f32 hdMapPaneFadeOut(hdMapPaneView_l* pane) {
    f32 alpha = 0.0f;
    if (pane) {
        gabi::Local<be<f32>> value;
        alpha = f32(f64(u8(pane->mAlpha)) / f64(PANE_MAX_ALPHA));
        *value = alpha;
        if (alpha > 0.0f) {
            hdMapChaseScaled(value.get(), 0.0f, 0.8f, 0.3f, 0.01f);
            alpha = *value;
            u32 flags = pane->mFlags;
            pane->mAlpha =
                u8(gabi::ftoi(gabi::fmuls_ppc(f32(PANE_MAX_ALPHA), alpha)));
            pane->mFlags = (flags & ~PANE_VISIBLE) + PANE_VISIBLE;
        } else
            pane->mFlags = u32(pane->mFlags) & ~PANE_VISIBLE;
    }
    return alpha;
}

void getMapPaneLocalRect(hdMapPaneMatrix_l*, HdPadRect*);

/* 02689528 */
static void computeMapPaneBounds(HdPadRect* bounds, hdMapPaneMatrix_l* pane) {
    if (!bounds || !pane)
        return;
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = reinterpret_cast<be<f32>*>(&pane->mMatrix[i])->get();
    gabi::Local<HdPadRect> rectStorage;
    auto* rect = rectStorage.get();
    getMapPaneLocalRect(pane, rect);
    f32 left = rect->mLeft, top = rect->mTop, right = rect->mRight,
        bottom = rect->mBottom;
    f32 cornerX[4] = {left, right, left, right};
    f32 cornerY[4] = {top, top, bottom, bottom};
    f32 minimumX = 3.4028234663852886e38f, minimumY = minimumX;
    f32 maximumX = -minimumX, maximumY = maximumX;
    for (u32 i = 0; i < 4; ++i) {
        f32 x = cornerX[i], y = cornerY[i];
        f32 a = gabi::fmadds(matrix[2], 0.0f, f32(matrix[0]) * x);
        f32 b = gabi::fmadds(matrix[3], 1.0f, f32(matrix[1]) * y);
        f32 transformedX = a + b;
        a = gabi::fmadds(matrix[6], 0.0f, f32(matrix[4]) * x);
        b = gabi::fmadds(matrix[7], 1.0f, f32(matrix[5]) * y);
        f32 transformedY = a + b;
        if (transformedX < minimumX)
            minimumX = transformedX;
        if (transformedY < minimumY)
            minimumY = transformedY;
        if (maximumX < transformedX)
            maximumX = transformedX;
        if (maximumY < transformedY)
            maximumY = transformedY;
    }
    if (minimumX < maximumX) {
        bounds->mRight = maximumX;
        bounds->mLeft = minimumX;
    } else {
        bounds->mRight = minimumX;
        bounds->mLeft = maximumX;
    }
    if (minimumY < maximumY) {
        bounds->mBottom = maximumY;
        bounds->mTop = minimumY;
    } else {
        bounds->mTop = maximumY;
        bounds->mBottom = minimumY;
    }
}
// Runtime view of the existing interpolation constants; no table contents are
// embedded.
static hdMapTrigTable_l sMapTrigTable;

/* 026893C8 */
static void rotateMapPoint(HdPadPoint* out, HdPadPoint* point,
                           HdPadPoint* center, f32 degrees) {
    f32 pointX = point->x, pointY = point->y;
    f32 x = pointX - f32(center->x), y = pointY - f32(center->y);
    if (180.0f < degrees)
        degrees = degrees - 360.0f;
    else if (degrees < -180.0f)
        degrees = degrees + 360.0f;
    f32 scaled = gabi::fmuls_ppc(degrees, 2147483648.0f);
    u32 phase = u32(gabi::ftoi((scaled / 180.0f)));
    f32 fraction =
        gabi::fmuls_ppc(f32(phase & 0xFFFFFF), 5.960464477539063e-08f);
    auto* sample = &sMapTrigTable.mSamples[phase >> 24];
    f32 sine = gabi::fmadds(sample->mSinDelta, fraction, sample->mSin);
    f32 cosine = gabi::fmadds(sample->mCosDelta, fraction, sample->mCos);
    f32 resultX = gabi::fmsubs(x, cosine, gabi::fmuls_ppc(y, sine));
    f32 resultY = gabi::fmadds(x, sine, gabi::fmuls_ppc(y, cosine));
    out->x = resultX;
    out->y = resultY;
    f32 centerY = center->y, centerX = center->x;
    resultY = resultY + centerY;
    resultX = resultX + centerX;
    out->x = resultX;
    out->y = resultY;
}
/* WWHD glue
data sMapTrigTable 10145FA0
ext 0268B070 void hdMapChaseValue(be<f32>*, f32, f32)
ext 02876368 void getMapPaneLocalRect(hdMapPaneMatrix_l*, HdPadRect*)
*/
