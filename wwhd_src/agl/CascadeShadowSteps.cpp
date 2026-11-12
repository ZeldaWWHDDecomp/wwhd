// Cascade update, layer submission and input-bound accumulation from assigned WWHD routines.
#include "bindings.h"
#include "agl/CascadeShadow.h"
#include "port_native_draw.h"
void resetCascadeUnitState(agl::ShadowUnit_l*);
void adjustCascadeUnitMargins(agl::ShadowUnit_l*, s32, s32, f32, f32);
void setCascadeMapLayerLimit(agl::CascadeMapStorage_l*, s32);
void configureCascadeMapLayer(agl::CascadeMapStorage_l*, u32, const be<u32>*, const be<u32>*);
void finishCascadeMapLayer(agl::CascadeMapStorage_l*, u32);
void* finishCascadeMapRendering(agl::CascadeMapStorage_l*, void*);
void setCascadeRasterizerClip(u32, u32);
u32 matchesCascadeInputBounds(agl::ShadowUnit_l*, const agl::CascadeBounds*);
u32 matchesAlternateCascadeInput(agl::ShadowUnit_l*, void*);
void calculateAlternateCascadeInput(agl::ShadowUnit_l*, agl::CascadeBounds*, void*);
static agl::ShadowUnit_l* shadowStepUnitAt(agl::CascadeShadow* shadow, u32 index) {
    agl::ShadowUnit_l* unit = shadow->mUnits.mData;
    if (index < (u32)shadow->mUnits.mCount)
        unit = gabi::at<agl::ShadowUnit_l>(gabi::ea(unit) + index * sizeof(agl::ShadowUnit_l));
    return unit;
}
static s32 divideCascadeDimension(s32 numerator, s32 denominator) {
    if (denominator == 0 || ((u32)numerator == 0x80000000u && denominator == -1))
        return numerator < 0 ? -1 : 0;
    return numerator / denominator;
}
/* 02798978 */
void agl::CascadeShadow::resetUnits() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (s32 index = 0; index < (s32)(u32)mCascadeCount.mValue; ++index)
        resetCascadeUnitState(shadowStepUnitAt(this, (u32)index));
}
/* 027989F0 */
void agl::CascadeShadow::adjustUnitMargins() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = (s32)(u32)mCascadeCount.mValue;
    for (s32 index = 0; index < count; ++index) {
        f32 nearMargin = index == 0 ? (f32)mNearOffset.mValue : 0.0f;
        f32 farMargin = (u32)index == (u32)count - 1 ? (f32)mFarOffset.mValue : 0.0f;
        ShadowUnit_l* unit = shadowStepUnitAt(this, (u32)index);
        s32 width = mMap.mWidth;
        s32 divisor = (s32)(u32)mStableTexelWidth.mValue;
        s32 height = mMap.mHeight;
        s32 outputHeight = divideCascadeDimension(height, divisor);
        s32 outputWidth = divideCascadeDimension(width, divisor);
        adjustCascadeUnitMargins(unit, outputWidth, outputHeight, nearMargin, farMargin);
        count = (s32)(u32)mCascadeCount.mValue;
    }
}
/* 02798AE4 */
void agl::CascadeShadow::limitMapLayers() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = (s32)(u32)mCascadeCount.mValue;
    s32 available = (s32)(u32)mUnits.mCount;
    if (count < 1) count = 1;
    if (available < count) count = available;
    setCascadeMapLayerLimit(&mMap, count);
}
/* 02798B0C */
void* agl::CascadeShadow::renderLayers(void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<void>> contextSlot(20);
    *contextSlot = context;
    s32 remaining = (s32)(u32)mCascadeCount.mValue;
    s32 available = (s32)(u32)mUnits.mCount;
    if (remaining < 1) remaining = 1;
    gabi::FrameLocal<CascadeIndexRecord> record(8);
    record->mIndex = 0;
    if (available < remaining) remaining = available;
    record->mOwner = this;
    record->mContextSlot = contextSlot.get();
    if ((u8)mDepthClamp.mValue != 0) setCascadeRasterizerClip(1, 0);
    for (u32 index = 0; remaining > 0; --remaining, ++index) {
        ShadowUnit_l* unit = shadowStepUnitAt(this, index);
        configureCascadeMapLayer(&mMap, index, unit->mFirstSurfaceWords, unit->mSecondSurfaceWords);
        CascadeIndexCallback* callback = mSupport.mIndexCallback;
        record->mIndex = index;
        if (callback != NULL) gabi::call_ptr<void>(callback->mTable->mInvoke, callback, record.get());
        finishCascadeMapLayer(&mMap, index);
    }
    if ((u8)mDepthClamp.mValue != 0) setCascadeRasterizerClip(1, 1);
    void* selected = *contextSlot;
    if ((u32)mMap.mRenderMode == 1) selected = finishCascadeMapRendering(&mMap, selected);
#if PORT_NATIVE_DRAW
    wwhd_native_pass_end();  // pass description (port_native_draw.h): the cascade layers are rendered
#endif
    return selected;
}
/* 02798C68 */
u32 agl::CascadeShadow::mergeInputBounds(const CascadeBounds* bounds, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mask = 0;
    for (s32 index = 0; index < (s32)(u32)mCascadeCount.mValue; ++index) {
        ShadowUnit_l* unit = shadowStepUnitAt(this, (u32)index);
        if (matchesCascadeInputBounds(unit, bounds) == 0) continue;
        unit = shadowStepUnitAt(this, (u32)index);
        CascadeBounds* destination = &unit->mInputBounds[lane < 3 ? lane : 0];
        f32 value = bounds->mMinimum[0];
        if ((f32)destination->mMinimum[0] > value) destination->mMinimum[0] = value;
        value = bounds->mMinimum[1];
        f32 previous = destination->mMinimum[1];
        f32 next = destination->mMinimum[2];
        if (previous > value) destination->mMinimum[1] = value;
        value = bounds->mMinimum[2];
        previous = destination->mMaximum[0];
        if (next > value) destination->mMinimum[2] = value;
        value = bounds->mMaximum[0];
        next = destination->mMaximum[1];
        if (previous < value) destination->mMaximum[0] = value;
        value = bounds->mMaximum[1];
        previous = destination->mMaximum[2];
        if (next < value) destination->mMaximum[1] = value;
        value = bounds->mMaximum[2];
        if (previous < value) destination->mMaximum[2] = value;
        if ((index & 32) == 0) mask |= 1u << (index & 31);
    }
    return mask;
}
/* 02798D98 */
u32 agl::CascadeShadow::gatherAlternateInputBounds(void* context, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<CascadeBounds> bounds(8);
    for (u32 axis = 0; axis < 3; ++axis) {
        bounds->mMinimum[axis] = 0x1.fffffep127f;
        bounds->mMaximum[axis] = -0x1.fffffep127f;
    }
    u32 mask = 0;
    bool collected = false;
    for (s32 index = 0; index < (s32)(u32)mCascadeCount.mValue; ++index) {
        ShadowUnit_l* unit = shadowStepUnitAt(this, (u32)index);
        if (matchesAlternateCascadeInput(unit, context) == 0) continue;
        if (!collected) {
            unit = shadowStepUnitAt(this, (u32)index);
            calculateAlternateCascadeInput(unit, bounds.get(), context);
            collected = true;
        }
        unit = shadowStepUnitAt(this, (u32)index);
        CascadeBounds* destination = &unit->mInputBounds[lane < 3 ? lane : 0];
        f32 value = bounds->mMinimum[0];
        f32 previous = destination->mMinimum[0];
        f32 next = destination->mMinimum[1];
        if (previous > value) destination->mMinimum[0] = value;
        value = bounds->mMinimum[1];
        previous = destination->mMinimum[2];
        if (next > value) destination->mMinimum[1] = value;
        value = bounds->mMinimum[2];
        next = destination->mMaximum[0];
        if (previous > value) destination->mMinimum[2] = value;
        value = bounds->mMaximum[0];
        previous = destination->mMaximum[1];
        if (next < value) destination->mMaximum[0] = value;
        value = bounds->mMaximum[1];
        next = destination->mMaximum[2];
        if (previous < value) destination->mMaximum[1] = value;
        value = bounds->mMaximum[2];
        if (next < value) destination->mMaximum[2] = value;
        if ((index & 32) == 0) mask |= 1u << (index & 31);
    }
    return mask;
}
/* WWHD glue
 ext 027C9770 void resetCascadeUnitState(agl::ShadowUnit_l*)
 ext 027C9AFC void adjustCascadeUnitMargins(agl::ShadowUnit_l*, s32, s32, f32, f32)
 ext 027967C4 void setCascadeMapLayerLimit(agl::CascadeMapStorage_l*, s32)
 ext 02796B3C void configureCascadeMapLayer(agl::CascadeMapStorage_l*, u32, const be<u32>*, const be<u32>*)
 ext 02796EEC void finishCascadeMapLayer(agl::CascadeMapStorage_l*, u32)
 ext 02796F74 void* finishCascadeMapRendering(agl::CascadeMapStorage_l*, void*)
 ext C0006958 void setCascadeRasterizerClip(u32, u32)
 ext 027C988C u32 matchesCascadeInputBounds(agl::ShadowUnit_l*, const agl::CascadeBounds*)
 ext 027C99BC u32 matchesAlternateCascadeInput(agl::ShadowUnit_l*, void*)
 ext 027C9A50 void calculateAlternateCascadeInput(agl::ShadowUnit_l*, agl::CascadeBounds*, void*)
*/
