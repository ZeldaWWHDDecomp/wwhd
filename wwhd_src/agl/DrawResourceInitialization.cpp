// Own resource records; canonical AGL#15 child resource construction remains opaque.
#include "agl/DrawResourceCleanup.h"
void* allocateDrawRecordResource(u32 bytes, sead::AllocationHeap* heap, s32 alignment);
agl::DrawReleaseResource_l* constructDrawRecordResource(agl::DrawReleaseResource_l* object);
namespace agl {
struct DrawDefaultOffsets_l { be<f32> x, y; };
static sead::RenderVector3 sResourceDefaultPosition;
static sead::RenderVector3 sResourceDefaultAxis;
static sead::ColorChannels sResourceDefaultColor;
static sead::ColorChannels sResourceDefaultAlternateColor;
static DrawDefaultOffsets_l sResourceDefaultOffsets;
// Caller-written resource fields, with the same post-construction pointer reloads as both originals.
static void initializeDrawRecordResource(DrawResourceValue_l* value, sead::AllocationHeap* heap,
                                         const sead::ColorChannels* border) {
    value->mSource = nullptr;
    DrawReleaseResource_l* resource = static_cast<DrawReleaseResource_l*>(
        allocateDrawRecordResource(0x198, heap, 4));
    if (gabi::ea(resource) != 0)
        resource = constructDrawRecordResource(resource);
    value->mResource = resource;
    u8 dirty = resource->mDirtyFlags;
    resource->mAddressW = 6;
    dirty |= 2;
    resource->mAddressV = 6;
    resource->mDirtyFlags = dirty;
    resource->mAddressU = 6;
    resource = value->mResource;
    u32 red = border->r.bits;
    resource->mBorderColor.r.bits = red;
    u32 green = border->g.bits;
    dirty = resource->mDirtyFlags;
    resource->mBorderColor.g.bits = green;
    u32 blue = border->b.bits;
    dirty |= 0x20;
    resource->mBorderColor.b.bits = blue;
    u32 alpha = border->a.bits;
    resource->mDirtyFlags = dirty;
    resource->mBorderColor.a.bits = alpha;
    resource = value->mResource;
    dirty = resource->mDirtyFlags;
    resource->mCompareEnabled = 1;
    resource->mDirtyFlags = dirty | 0x10;
    resource = value->mResource;
    dirty = resource->mDirtyFlags;
    resource->mCompareEnabled = 1;
    resource->mCompareMode = 3;
    resource->mDirtyFlags = dirty | 0x10;
}
/* 027CCC80 */
void agl::DrawResourceCleanup_l::initializeRecordResources(DrawResourceRecord_l* record,
                                                          sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> negativeAxis(8);
    gabi::FrameLocal<sead::RenderVector3> copiedAxis(0x14);
    record->mFlags = 3;
    record->mPosition.x = sResourceDefaultPosition.x;
    record->mPosition.y = sResourceDefaultPosition.y;
    record->mPosition.z = sResourceDefaultPosition.z;
    f32 x = sResourceDefaultAxis.x;
    f32 y = sResourceDefaultAxis.y;
    f32 z = sResourceDefaultAxis.z;
    negativeAxis->x = -x;
    copiedAxis->x = negativeAxis->x;
    negativeAxis->y = -y;
    copiedAxis->y = negativeAxis->y;
    negativeAxis->z = -z;
    record->mDirection.y = copiedAxis->y;
    record->mDirection.x = copiedAxis->x;
    copiedAxis->z = negativeAxis->z;
    u32 count = record->mValueCount;
    record->mDirection.z = copiedAxis->z;
    DrawResourceValue_l* value = record->mValues;
    record->mFalloff = 0.5f;
    DrawResourceValue_l* end = sead::elementPtr(value, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    record->mRadius = 5.0f;
    record->mAngle = 1.5707963705062866f;
    record->mScale = 1.0f;
    while (value != end) {
        initializeDrawRecordResource(value, heap, &sResourceDefaultColor);
        value = sead::nextElement(value);
    }
}
/* 027CDE3C */
void agl::DrawResourceCleanup_l::initializeLargeRecordResources(LargeDrawResourceRecord_l* record,
                                                               sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> negativeAxis(8);
    gabi::FrameLocal<sead::RenderVector3> copiedAxis(0x14);
    record->mFlags = 0xB;
    record->mPosition.x = sResourceDefaultPosition.x;
    record->mPosition.y = sResourceDefaultPosition.y;
    record->mPosition.z = sResourceDefaultPosition.z;
    f32 x = sResourceDefaultAxis.x;
    f32 z = sResourceDefaultAxis.z;
    f32 y = sResourceDefaultAxis.y;
    record->mAxis.z = z;
    negativeAxis->x = -x;
    record->mAxis.x = x;
    negativeAxis->y = -y;
    copiedAxis->x = negativeAxis->x;
    copiedAxis->y = negativeAxis->y;
    negativeAxis->z = -z;
    copiedAxis->z = negativeAxis->z;
    record->mDirection.y = copiedAxis->y;
    record->mDirection.z = copiedAxis->z;
    record->mDirection.x = copiedAxis->x;
    record->mAxis.y = y;
    record->mColor.r.bits = sResourceDefaultColor.r.bits;
    record->mColor.g.bits = sResourceDefaultColor.g.bits;
    record->mColor.b.bits = sResourceDefaultColor.b.bits;
    record->mColor.a.bits = sResourceDefaultColor.a.bits;
    record->mAlternateColor.r.bits = sResourceDefaultAlternateColor.r.bits;
    record->mAlternateColor.g.bits = sResourceDefaultAlternateColor.g.bits;
    record->mAlternateColor.b.bits = sResourceDefaultAlternateColor.b.bits;
    u32 alpha = sResourceDefaultAlternateColor.a.bits;
    record->mRight = 0.8f;
    record->mFar = 2.0f;
    record->mAspectRatio = 0.7f;
    record->mBottom = -0.5f;
    record->mLeft = -0.8f;
    record->mTop = 0.5f;
    record->mNear = 0.4f;
    record->mVerticalAngle = 0.7853981852531433f;
    record->mAlternateColor.a.bits = alpha;
    u32 count = record->mValueCount;
    record->mPositiveScale = 1.0f;
    record->mOffsetX = sResourceDefaultOffsets.x;
    f32 offsetY = sResourceDefaultOffsets.y;
    DrawResourceValue_l* value = record->mValues;
    record->mBoundsEnabled = 0;
    DrawResourceValue_l* end = sead::elementPtr(value, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    record->mOffsetY = offsetY;
    record->mScaleX = 1.0f;
    record->mBoundsState = 0;
    record->mScaleY = 1.0f;
    record->mEnabled = 0;
    record->mValuesEnabled = 1;
    while (value != end) {
        initializeDrawRecordResource(value, heap, &sResourceDefaultAlternateColor);
        value = sead::nextElement(value);
    }
}
}
/* WWHD glue
data sResourceDefaultPosition 104A0928
data sResourceDefaultAxis 104A0940
data sResourceDefaultColor 104A01EC
data sResourceDefaultAlternateColor 104A01CC
data sResourceDefaultOffsets 104A0908
ext 0273B050 void* allocateDrawRecordResource(u32 bytes, sead::AllocationHeap* heap, s32 alignment)
ext 027BDF7C agl::DrawReleaseResource_l* constructDrawRecordResource(agl::DrawReleaseResource_l* object)
*/
