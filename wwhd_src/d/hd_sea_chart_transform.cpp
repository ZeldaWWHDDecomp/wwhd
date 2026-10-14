#include "d/hd_sea_chart_components.h"
using namespace gabi;

static f32 sChartTransformZero;
static f32 sChartTransformOne;
struct hdSeaChartTransformStatics_c {
    be<f32> mMinimum;
    be<f32> mMaximum;
    HdPadPoint mDefaultOrigin;
    HdPadPoint mDefaultOffset;
    be<u8> mFirstFlag;
    be<u8> mSecondFlag;
    u8 mPadding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdSeaChartTransformStatics_c, mMinimum, 0);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mMaximum, 4);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mDefaultOrigin, 8);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mDefaultOffset, 0x10);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mFirstFlag, 0x18);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mSecondFlag, 0x19);
WWHD_OFFSET(hdSeaChartTransformStatics_c, mStorage, 0x1C);
struct hdSeaChartStaticRegistration_c {
    be<u32> mWords[3];
};
WWHD_SIZE(hdSeaChartStaticRegistration_c, 0xC);
static hdSeaChartTransformStatics_c sChartTransformStatics;
static hdSeaChartStaticRegistration_c sChartTransformRegistrations[3] = {
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static f32 sChartTransformMinimum;
static f32 sChartTransformMaximum;
static f32 sChartTransformDefaultOrigin;
static f32 sChartTransformDefaultOffset;
void seaChartRegisterStatic(hdSeaChartStaticRegistration_c* record);
void seaChartInitializeFirstFlag(be<u8>* flag);
void seaChartInitializeSecondFlag(be<u8>* flag);
static f32 chartDivide(f32 value, f32 divisor) {
    return f32(f64(value) / f64(divisor));
}

/* Embedded constructors test the 32-bit guest address for zero before writing. Preserve their
 * allocation fallback when a member crosses the guest address-space boundary. */
template <class T> static T* chartEmbeddedMember(hdSeaChartTransform_c* self, u32 offset) {
    const u32 address = u32(gabi::ea(self) + offset);
    return address ? gabi::at<T>(address) : static_cast<T*>(seaChartAllocate(sizeof(T)));
}

static void chartInitializePoint(HdPadPoint* point, f32 value) {
    if (point) {
        point->y = value;
        point->x = value;
    }
}

/* 0267F39C */
hdScreenType* seaChartTransformParentType(hdScreenType* type) {
    return type->mParent;
}

/* 0267F49C */
hdSeaChartTransform_c* seaChartTransformConstruct(hdSeaChartTransform_c* self) {
    if (!self)
        self = static_cast<hdSeaChartTransform_c*>(seaChartAllocate(sizeof(*self)));
    if (!self)
        return nullptr;
    const f32 zero = sChartTransformZero;
    self->mOwner = nullptr;
    self->mDisplayPane = nullptr;
    self->mAnchorPane = nullptr;
    self->mChart = nullptr;
    chartInitializePoint(
        chartEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartTransform_c, mPosition)), zero);
    chartInitializePoint(
        chartEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartTransform_c, mDisplacement)),
        zero);
    chartInitializePoint(
        chartEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartTransform_c, mPreviousPosition)),
        zero);
    self->mDepth = zero;
    self->mSize.x = zero;
    const f32 one = sChartTransformOne;
    self->mDepthOffset = zero;
    self->mScale = one;
    self->mScaledSize.x = zero;
    self->mScaledSize.y = zero;
    self->mSize.y = zero;
    self->mScaleMultiplier = one;
    HdPadRect* bounds =
        chartEmbeddedMember<HdPadRect>(self, offsetof(hdSeaChartTransform_c, mBounds));
    if (bounds) {
        bounds->mRight = zero;
        bounds->mTop = zero;
        bounds->mLeft = zero;
        bounds->mBottom = zero;
    }
    self->mMode = 10;
    self->mVisible = 1;
    chartInitializePoint(
        chartEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartTransform_c, mParentScale)), one);
    return self;
}

/* 0267F604 */
hdSeaChartTransform_c* hdSeaChartTransform_c::reset() {
    const f32 zero = sChartTransformZero;
    const f32 one = sChartTransformOne;
    mBounds.mLeft = zero;
    mBounds.mRight = zero;
    mMode = 10;
    mPosition.y = zero;
    mVisible = 1;
    mDepthOffset = zero;
    mParentScale.x = one;
    mDepth = zero;
    mParentScale.y = one;
    mScaledSize.y = zero;
    mDisplacement.y = zero;
    mAnchorPane = nullptr;
    mScaledSize.x = zero;
    mChart = nullptr;
    mSize.x = zero;
    mOwner = nullptr;
    mDisplacement.x = zero;
    mDisplayPane = nullptr;
    mSize.y = zero;
    mScale = one;
    mBounds.mBottom = zero;
    mScaleMultiplier = one;
    mBounds.mTop = zero;
    mPosition.x = zero;
    return this;
}

/* 0267F684 */
void hdSeaChartTransform_c::destroy(u32 flags) {
    if (this) {
        hdSeaChartTransform_c* allocation = reset();
        if (flags & 1)
            seaChartFree(allocation);
    }
}

/* 0267F6B8 */
void hdSeaChartTransform_c::applyTransform() {
    hdSeaChartPane_l* pane = mDisplayPane;
    if (!pane)
        return;
    const f32 scale = fmuls_ppc(mScale, mScaleMultiplier);
    pane->mFlags = u8(pane->mFlags) | 0x10;
    pane->mScale.y = scale;
    pane->mScale.x = scale;
    pane = mDisplayPane;
    const f32 rotationX = pane->mRotation.x;
    const f32 rotationY = pane->mRotation.y;
    const f32 depth = fadds_ppc(mDepth, mDepthOffset);
    // Original lwz-to-stack followed by lfs/stfs converts the retained X/Y rotations.
    pane->mRotation.x = rotationX;
    const u8 flags = pane->mFlags;
    pane->mRotation.y = rotationY;
    pane->mRotation.z = depth;
    pane->mFlags = flags | 0x10;
    const f32 x = fadds_ppc(mPosition.x, mDisplacement.x);
    const f32 y = fadds_ppc(mPosition.y, mDisplacement.y);
    hdSeaChartOriginView_l* chart = mChart;
    const f32 translatedX = fadds_ppc(x, chart->mOrigin.x);
    const f32 translatedY = fadds_ppc(y, chart->mOrigin.y);
    const f32 paneX = chartDivide(translatedX, mParentScale.x);
    pane = mDisplayPane;
    const u8 positionFlags = pane->mFlags;
    const f32 paneY = chartDivide(translatedY, mParentScale.y);
    pane->mFlags = positionFlags | 0x10;
    pane->mPosition.x = paneX;
    pane->mPosition.z = sChartTransformZero;
    pane->mPosition.y = paneY;
    pane = mDisplayPane;
    const u8 hiddenFlags = u8(pane->mFlags) & 0xFE;
    pane->mFlags = hiddenFlags;
    pane->mFlags = hiddenFlags | u8(mVisible);
}

/* 0267F7F4 */
HdPadPoint* hdSeaChartTransform_c::calculateScaledSize() {
    const f32 scale = fmuls_ppc(mScale, mScaleMultiplier);
    const f32 x = fmuls_ppc(mSize.x, scale);
    const f32 y = fmuls_ppc(mSize.y, scale);
    mScaledSize.x = x;
    mScaledSize.y = y;
    return &mScaledSize;
}

/* 0267F824 */
void hdSeaChartTransform_c::updateParentScale() {
    hdSeaChartPane_l* pane = mDisplayPane;
    if (pane && pane->mpParent) {
        Pair32 scale{seaChartPaneScale(pane->mpParent)};
        // These are lwz/stw copies, preserving every floating-point bit.
        reinterpret_cast<be<u32>&>(mParentScale.x) = scale.r3;
        reinterpret_cast<be<u32>&>(mParentScale.y) = scale.r4;
    }
}

/* 0267F878 */
hdSeaChartTransform_c* hdSeaChartTransform_c::copyWorldPosition(HdPadPoint* output) {
    if (output && mChart) {
        hdSeaChartOriginView_l* chart = mChart;
        const f32 originX = chart->mOrigin.x;
        const f32 originY = chart->mOrigin.y;
        const f32 offsetX = chart->mOffset.x;
        const f32 offsetY = chart->mOffset.y;
        output->x = fadds_ppc(fadds_ppc(fadds_ppc(mPosition.x, mDisplacement.x), originX), offsetX);
        output->y = fadds_ppc(fadds_ppc(fadds_ppc(mPosition.y, mDisplacement.y), originY), offsetY);
    }
    // The caller reads mAnchorPane through the retained r3 after this call.
    return this;
}

/* 0267F8F8 */
void hdSeaChartTransform_c::copyAnchorPosition(HdPadPoint* output) {
    if (output && mAnchorPane) {
        gabi::Local<HdPadPoint> world;
        world->y = sChartTransformZero;
        world->x = sChartTransformZero;
        hdSeaChartTransform_c* transformed = copyWorldPosition(world);
        hdSeaChartPane_l* anchor = transformed->mAnchorPane;
        gabi::Local<HdPadPoint3> origin;
        seaChartPaneOrigin(origin, anchor);
        output->x = fadds_ppc(origin->x, world->x);
        output->y = fadds_ppc(origin->y, world->y);
    }
}

/* 0267F974 */
void hdSeaChartTransform_c::applySize() {
    const f32 width = chartDivide(mSize.x, mParentScale.x);
    const f32 height = chartDivide(mSize.y, mParentScale.y);
    hdSeaChartPane_l* pane = mDisplayPane;
    pane->mFlags = u8(pane->mFlags) | 0x10;
    pane->mSize.x = width;
    pane->mSize.y = height;
    seaChartPaneBounds(&mBounds, reinterpret_cast<hdMapPaneMatrix_l*>(pane));
}

/* 0267F9B0 */
void seaChartInitializeTransformStatics() {
    sChartTransformStatics.mStorage[3] = 0;
    sChartTransformStatics.mStorage[2] = 0;
    sChartTransformStatics.mStorage[1] = 0;
    sChartTransformStatics.mStorage[0] = 0;
    seaChartRegisterStatic(&sChartTransformRegistrations[0]);
    const f32 minimum = sChartTransformMinimum;
    const f32 maximum = sChartTransformMaximum;
    sChartTransformStatics.mMinimum = minimum;
    sChartTransformStatics.mMaximum = maximum;
    seaChartInitializeFirstFlag(&sChartTransformStatics.mFirstFlag);
    seaChartRegisterStatic(&sChartTransformRegistrations[1]);
    seaChartInitializeSecondFlag(&sChartTransformStatics.mSecondFlag);
    seaChartRegisterStatic(&sChartTransformRegistrations[2]);
    const f32 origin = sChartTransformDefaultOrigin;
    const f32 offset = sChartTransformDefaultOffset;
    sChartTransformStatics.mDefaultOrigin.x = origin;
    sChartTransformStatics.mDefaultOffset.x = offset;
    sChartTransformStatics.mDefaultOrigin.y = origin;
    sChartTransformStatics.mDefaultOffset.y = offset;
}

/* WWHD glue (tools/verify/autoglue.py)
data sChartTransformZero 100F914C
data sChartTransformOne 100F9148
data sChartTransformStatics 10496150
data sChartTransformRegistrations 101F6224 nocheck
data sChartTransformMinimum 100F9164
data sChartTransformMaximum 100F9168
data sChartTransformDefaultOrigin 100F916C
data sChartTransformDefaultOffset 100F9170
ext 028F026C void seaChartRegisterStatic(hdSeaChartStaticRegistration_c*)
ext 028ED6F8 void seaChartInitializeFirstFlag(be<u8>*)
ext 028EAB2C void seaChartInitializeSecondFlag(be<u8>*)
ext 0273AF40 void seaChartFree(void*)
ext 0273AD10 void* seaChartAllocate(u32)
ext 02704EA8 Pair32 seaChartPaneScale(hdPartsPane_c*)
ext 02704D2C void seaChartPaneOrigin(HdPadPoint3*, hdSeaChartPane_l*)
ext 02689528 void seaChartPaneBounds(HdPadRect*, hdMapPaneMatrix_l*)
*/
