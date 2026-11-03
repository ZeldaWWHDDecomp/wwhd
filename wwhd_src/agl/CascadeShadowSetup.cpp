// Cascade array setup and callback selection, reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/CascadeShadow.h"
#include "agl/CascadeFormattedLabel.h"
sead::RegistryHeap* currentCascadeSetupHeap(sead::RegistryManagerState*);
void constructCascadeSetupUnit(agl::ShadowUnit_l*);
void initializeCascadeSetupUnit(agl::ShadowUnit_l*, sead::RegistryHeap*);
void* allocateCascadeSplitArray(u32, sead::RegistryHeap*, u32);
void configureCascadeSetupMap(agl::CascadeMapStorage_l*, const be<s32>*, sead::RegistryHeap*);
void configureCascadeSetupRange(agl::ShadowUnit_l*, agl::CascadeSupportStorage_l*, void*, void*, f32, f32, f32);
u32 cascadeDirectInputCallback(agl::CascadeShadow*, const agl::CascadeBounds*, u32);
u32 cascadeAlternateInputCallback(agl::CascadeShadow*, void*, u32);
u32 cascadeMemberInputCallback(agl::CascadeShadow*, void*, u32);
u32 cascadeAlternateMemberInputCallback(agl::CascadeShadow*, void*, u32);
u32 cascadeSecondaryMemberInputCallback(agl::CascadeShadow*, void*, u32);
u32 cascadeSecondaryAlternateMemberInputCallback(agl::CascadeShadow*, void*, u32);
u32 cascadeMatchingMaskCallback(agl::CascadeShadow*, void*);
u32 cascadeAlternateMatchingMaskCallback(agl::CascadeShadow*, void*);
u32 cascadeFullMaskCallback(agl::CascadeShadow*);
u32 cascadeAlternateFullMaskCallback(agl::CascadeShadow*);
static u32 sCascadeSetupTextTable;
static agl::ShadowUnit_l* cascadeSetupUnitAt(agl::CascadeShadow* shadow, u32 index) {
    agl::ShadowUnit_l* unit = shadow->mUnits.mData;
    if (index < (u32)shadow->mUnits.mCount)
        unit = gabi::at<agl::ShadowUnit_l>(gabi::ea(unit) + index * sizeof(agl::ShadowUnit_l));
    return unit;
}
/* 027987BC */
void agl::CascadeShadow::updateCascadeRanges(void* firstContext, void* secondContext) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = (s32)(u32)mCascadeCount.mValue;
    s32 available = (s32)(u32)mUnits.mCount;
    if (count < 1) count = 1;
    if (count < available) available = count;
    mCascadeCount.mValue = available;
    for (s32 index = 0; index < (s32)(u32)mCascadeCount.mValue; ++index) {
        ShadowUnit_l* unit = cascadeSetupUnitAt(this, (u32)index);
        f32 epsilon = mPointEpsilon.mValue;
        u32 data = gabi::ea((void*)mCascadeNear.mData) + ((u32)index << 2);
        f32 nearSplit = *gabi::at<be<f32>>(data);
        f32 farSplit = *gabi::at<be<f32>>(data + 4);
        configureCascadeSetupRange(unit, &mSupport, firstContext, secondContext, nearSplit, farSplit, epsilon);
    }
    mSecondCount = 0; mFirstCount = 0;
    mFirstIndex = -1; mSecondIndex = -1;
    u32 mode = mBoundingCalculation.mValue;
    if (mode == 0) {
        mFirstBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeDirectInputCallback));
        mSecondBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeAlternateInputCallback));
    } else if (mode == 1) {
        if ((u8)mMapFlag != 0) {
            mFirstBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeMemberInputCallback));
            mSecondBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeAlternateMemberInputCallback));
        } else {
            mFirstBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeSecondaryMemberInputCallback));
            mSecondBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeSecondaryAlternateMemberInputCallback));
        }
    } else if (mode == 2) {
        mFirstBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeMatchingMaskCallback));
        mSecondBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeAlternateMatchingMaskCallback));
    } else {
        mSecondBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeAlternateFullMaskCallback));
        mFirstBoundsCallback = gabi::guest_fn(reinterpret_cast<const void*>(&cascadeFullMaskCallback));
    }
}
/* 027984FC */
void agl::CascadeShadow::initializeUnits(const be<s32>* requestedCount, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCascadeCount.mValue = (u32)(s32)*requestedCount;
    s32 count = *requestedCount;
    if (count > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = currentCascadeSetupHeap(sRegistryManager);
        auto* units = gabi::call_ptr<ShadowUnit_l*>(allocationHeap->dispatch()->mAllocate.mFunction,
            allocationHeap, (u32)count * sizeof(ShadowUnit_l), 4u);
        u32 remaining = count > 0 ? (u32)count : 1;
        u32 offset = 0;
        do {
            auto* unit = gabi::at<ShadowUnit_l>(gabi::ea(units) + offset);
            if (unit != NULL) constructCascadeSetupUnit(unit);
            offset += sizeof(ShadowUnit_l);
        } while (--remaining != 0);
        if (units != NULL) { mUnits.mData = units; mUnits.mCount = count; }
        count = *requestedCount;
    }
    u32 splitCount = (u32)count + 1;
    mCascadeNear.mData = allocateCascadeSplitArray(splitCount << 2, heap, 4);
    mCascadeNear.mCount = splitCount;
    for (u32 index = 0; index != splitCount; ++index) {
        u32 data = gabi::ea((void*)mCascadeNear.mData);
        *gabi::at<be<f32>>(data + (index << 2)) = 0.0f;
    }
    count = *requestedCount;
    for (s32 index = 0; index < count; ++index) {
        u32 data = gabi::ea((void*)mCascadeNear.mData);
        auto* split = gabi::at<be<f32>>(data + ((u32)index << 2));
        if (index == 0) *split = 10.0f;
        else {
            f32 previous = *gabi::at<be<f32>>(gabi::ea(split) - 4);
            *split = gabi::fadds_ppc(previous, 500.0f);
        }
        initializeCascadeSetupUnit(cascadeSetupUnitAt(this, (u32)index), heap);
        count = *requestedCount;
    }
    u32 data = gabi::ea((void*)mCascadeNear.mData);
    u32 lastOffset = (u32)count << 2;
    f32 previous = *gabi::at<be<f32>>(data + lastOffset - 4);
    f32 farSplit = gabi::fadds_ppc(previous, 500.0f);
    f32 limitDifference = gabi::fsubs_ppc(farSplit, 20000.0f);
    if (!(limitDifference >= 0.0f)) farSplit = 20000.0f;
    *gabi::at<be<f32>>(data + lastOffset) = farSplit;
    gabi::FrameLocal<be<s32>> mapCount(16);
    *mapCount = *requestedCount;
    configureCascadeSetupMap(&mMap, mapCount.get(), heap);
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sCascadeSetupTextTable);
    name->mText = gabi::at<be<u8>>(gabi::guest_str("depth_shadow"));
    mRoot.mTree.appendValue(&mParameters, name.get());
    for (s32 index = 0; index < (s32)*requestedCount; ++index) {
        ShadowUnit_l* unit = cascadeSetupUnitAt(this, (u32)index);
        gabi::FrameLocal<CascadeFormattedLabel> label(20);
        gabi::cpu->cr[6] = 0;
        // One integer vararg is supplied; the remaining saved registers are not format inputs.
        auto* formatted = gabi::call_ptr<CascadeFormattedLabel*>(
            gabi::guest_fn(reinterpret_cast<const void*>(&CascadeFormattedLabel::createFormatted)),
            label.get(), reinterpret_cast<const char*>(gabi::guest_str("shadow_unit_%d")), (u32)index);
        mRoot.mTree.appendValue(reinterpret_cast<parameter_tree::Value*>(unit),
            reinterpret_cast<sead::SafeText*>(formatted));
    }
    name->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sCascadeSetupTextTable);
    name->mText = gabi::at<be<u8>>(gabi::guest_str("shadow_map"));
    mRoot.mTree.appendValue(reinterpret_cast<parameter_tree::Value*>(&mMap), name.get());
}
/* WWHD glue
 data sCascadeSetupTextTable 10166D98
 str 10167224 "depth_shadow"
 str 10167234 "shadow_unit_%d"
 str 10167244 "shadow_map"
 ext 02756140 sead::RegistryHeap* currentCascadeSetupHeap(sead::RegistryManagerState*)
 ext 027C70A8 void constructCascadeSetupUnit(agl::ShadowUnit_l*)
 ext 027C7590 void initializeCascadeSetupUnit(agl::ShadowUnit_l*, sead::RegistryHeap*)
 ext 0273B0D4 void* allocateCascadeSplitArray(u32, sead::RegistryHeap*, u32)
 ext 027966F0 void configureCascadeSetupMap(agl::CascadeMapStorage_l*, const be<s32>*, sead::RegistryHeap*)
 ext 027C93A8 void configureCascadeSetupRange(agl::ShadowUnit_l*, agl::CascadeSupportStorage_l*, void*, void*, f32, f32, f32)
 ext 02798C68 u32 cascadeDirectInputCallback(agl::CascadeShadow*, const agl::CascadeBounds*, u32) callback
 ext 02798D98 u32 cascadeAlternateInputCallback(agl::CascadeShadow*, void*, u32) callback
 ext 027991D0 u32 cascadeMemberInputCallback(agl::CascadeShadow*, void*, u32) callback
 ext 027993A4 u32 cascadeAlternateMemberInputCallback(agl::CascadeShadow*, void*, u32) callback
 ext 02799578 u32 cascadeSecondaryMemberInputCallback(agl::CascadeShadow*, void*, u32) callback
 ext 0279974C u32 cascadeSecondaryAlternateMemberInputCallback(agl::CascadeShadow*, void*, u32) callback
 ext 02799920 u32 cascadeMatchingMaskCallback(agl::CascadeShadow*, void*) callback
 ext 027999AC u32 cascadeAlternateMatchingMaskCallback(agl::CascadeShadow*, void*) callback
 ext 02799A38 u32 cascadeFullMaskCallback(agl::CascadeShadow*) callback
 ext 02799A6C u32 cascadeAlternateFullMaskCallback(agl::CascadeShadow*) callback
*/
