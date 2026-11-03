// Shadow pre-pass selection and per-frame double-buffer state.
#include "agl/ShadowPrePass.h"
#include "agl/ShadowControlInstances.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
// The dynamic texture cache singleton (101F8DD4; owned by AGL#10): a borrowed per-TU binding, never initialized here.
using TextureCachePointer = gptr<agl::DynamicTextureCache>;
static TextureCachePointer sShadowTextureCache;
struct DimensionScratch { be<u32> mHigh, mLow; };
WWHD_OFFSET(DimensionScratch, mHigh, 0);
WWHD_OFFSET(DimensionScratch, mLow, 4);
WWHD_SIZE(DimensionScratch, 8);
static u32 scaledDimension(DimensionScratch*, u32, f32);
static void destroyFrameResources(FrameResources*);
static u8 sPrePassObjectTable;
static u8 sPrePassControlTable;
static agl::parameter_tree::ValueVtable sPrePassParameterTable;
static sead::DisposerVtable sPrePassParameterControlTable;
static u8 sPrePassParameterLinkTable;

/* 0278D8D4 */
void ShadowPrePass::destroy(ShadowPrePass* shadow, u32 flags) {
    gabi::AutoFrame frameContext{0x0278D8D4u};
    if (shadow == nullptr) return;
    shadow->mObjectTable = &sPrePassObjectTable;
    FrameResources* frames = shadow->mFrames;
    shadow->mRoot.mTree.mTable = gabi::ea(&sPrePassControlTable);
    if (frames != nullptr) {
        s32 count = static_cast<s32>(shadow->mFrameCount);
        u32 index = 0;
        while (static_cast<s32>(index) < count) {
            FrameResources* frame = guestElement(frames, index);
            if (frame != nullptr) {
                destroyFrameResources(frame);
                count = static_cast<s32>(shadow->mFrameCount);
                frames = shadow->mFrames;
            }
            ++index;
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), frames);
        u32 release = heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(release, heap, shadow->mFrames.get());
        shadow->mFrameCount = 0;
        shadow->mFrames = nullptr;
    }
    agl::ResourceRecord* record = &shadow->mParameterState.mRecord;
    record->detach();
    shadow->mParameterState.mObject.mVtable = &sPrePassParameterTable;
    record->mVtable = &sPrePassParameterControlTable;
    record->mInterfaceMethods = &sPrePassParameterLinkTable;
    record->detach();
    static_cast<sead::RegisteredObject*>(record)->destroy(0);
    if (flags & 1) operator_delete(shadow);
}

static void destroyFrameResources(FrameResources* frame) {
    agl::TextureSampler::destroy(&frame->mFinalSampler, 2);
    agl::TextureSampler::destroy(&frame->mAuxiliarySampler, 2);
    frame->mExtraTarget.mFrame.destroy(2);
    destroyShadowFrameArray(frame->mPassTargets, 2, sizeof(agl::RenderBuffer),
                            gabi::guest_fn((const void*)&destroyShadowFrameTarget), 0, 0);
    destroyShadowFrameArray(frame->mMipSamplers, 2, sizeof(agl::TextureSampler),
                            gabi::guest_fn((const void*)&agl::TextureSampler::destroy), 0, 0);
    frame->mAlternateTarget.mFrame.destroy(2);
    agl::TextureSampler::destroy(&frame->mAlternateSampler, 2);
    frame->mPrimaryTarget.mFrame.destroy(2);
    agl::TextureSampler::destroy(&frame->mPrimarySampler, 2);
    for (u32 i = 4; i != 0; --i)
        agl::TextureSampler::destroy(&frame->mInitialSamplers[i - 1], 2);
    agl::UniformBuffer::destroy(&frame->mBuffers[2], 2);
    agl::UniformBuffer::destroy(&frame->mBuffers[1], 2);
    agl::UniformBuffer::destroy(&frame->mBuffers[0], 2);
}

/* 0278FB08 */
u32 ShadowPrePass::passSelection() const {
    bool first = mParameters.mBlurType.mValue != 0;
    u32 flags = mRegistryQuery.mValue;
    u8 secondary = mParameters.mCombineSsao.mValue;
    bool second = (flags & 1) != 0 || mParameters.mEdgeCutoff.mValue != 0;
    if ((secondary != 0 || mParameters.mSsaoDirectDraw.mValue != 0) && mParameters.mEdgeCutoff.mValue == 0)
        second = false;
    return (first ? 2 : 0) + (second ? 1 : 0);
}

enum class ReadTargetSelection { Unchanged, Primary, Alternate };
static ReadTargetSelection chooseReadTarget(u32 mask, u32 pass) {
    switch (mask) {
    case 0:
        if (pass == 0) return ReadTargetSelection::Primary;
        break;
    case 1:
        if (pass == 0) return ReadTargetSelection::Alternate;
        if (pass == 1) return ReadTargetSelection::Primary;
        break;
    case 2:
        if (pass == 1) return ReadTargetSelection::Alternate;
        if (pass == 0 || pass == 2) return ReadTargetSelection::Primary;
        break;
    case 3:
        if (pass == 0 || pass == 2) return ReadTargetSelection::Alternate;
        if (pass == 1 || pass == 3) return ReadTargetSelection::Primary;
        break;
    }
    return ReadTargetSelection::Unchanged;
}

/* 0278EE50 */
u32 ShadowPrePass::configurationKey(u32 frameIndex) const {
    u32 count = mFrameCount;
    u32 mode = mParameters.mResolution.mValue;
    const FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    u32 key = 0;
    if (mode == 0) key = 1;
    else if (mode == 1) key = 2;
    else if (mode == 2) key = 4;
    if ((mRegistryQuery.mValue & 1) != 0 && frame->mCombinedSampler != nullptr) key += 8;
    u32 first = mParameters.mBlurType.mValue;
    u8 depthToNormal = mParameters.mMipBlur.mValue;
    if (first != 0) key += 16;
    if (depthToNormal != 0) key += 32;
    return key;
}

/* 0278EEF8 */
void ShadowPrePass::releaseTransientStorage(u32 frameIndex) {
    gabi::AutoFrame frameContext{0x0278EEF8u};
    u32 count = mFrameCount;
    FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    if (frame->mPrimaryStorage.get() != nullptr) {
        agl::DynamicTextureCache* cache = sShadowTextureCache;
        agl::TextureCacheEntry* storage = frame->mPrimaryStorage;
        cache->release(storage);
        frame->mPrimaryStorage = nullptr;
        frame->mStorageChanged = 1;
    }
    agl::TextureCacheEntry* storage = frame->mAlternateStorage;
    if (storage != nullptr) {
        sShadowTextureCache->release(storage);
        frame->mAlternateStorage = nullptr;
    }
    if (mParameters.mMipBlur.mValue != 0) {
        for (u32 i = 0; i != 2; ++i) {
            storage = frame->mMipStorage[i];
            if (storage != nullptr) {
                sShadowTextureCache->release(storage);
                frame->mMipStorage[i] = nullptr;
            }
        }
    }
    frame->mReady = 0;
}

/* 0278EFC0 */
void ShadowPrePass::clearPrimaryTarget(u32 frameIndex) {
    gabi::AutoFrame frameContext{0x0278EFC0u};
    gabi::FrameLocal<sead::Viewport> viewport(0x18);
    gabi::FrameLocal<sead::ColorChannels> color(8);
    u32 count = mFrameCount;
    FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    ColorTarget* target = &frame->mPrimaryTarget;
    sead::activateFrameBuffer(frameDisplay(&target->mFrame));
    sead::Viewport::createSurface(viewport.get(), frameSize(&target->mFrame));
    viewport->apply(frameSurface(&target->mFrame));
    color->g.value = 1.0f;
    color->a.value = 1.0f;
    color->b.value = 1.0f;
    color->r.value = 1.0f;
    clearShadowRenderTarget(&target->mFrame, 0, 1, color.get(), 0, 1.0);
}

/* 0278FB94 */
ColorTarget* ShadowPrePass::outputTarget(u32 frameIndex, u32 pass) {
    gabi::AutoFrame frameContext{0x0278FB94u};
    u32 count = mFrameCount;
    FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    ReadTargetSelection selection = chooseReadTarget(passSelection(), pass);
    if (selection == ReadTargetSelection::Alternate) {
        frame->mAlternateTarget.mSurface.invalidate();
        return &frame->mAlternateTarget;
    }
    if (selection == ReadTargetSelection::Primary)
        frame->mPrimaryTarget.mSurface.invalidate();
    return &frame->mPrimaryTarget;
}

/* 0278FC80 */
ColorTarget* ShadowPrePass::ensureOutput(u32 frameIndex, u32 width, u32 height) {
    gabi::AutoFrame frameContext{0x0278FC80u};
    if (mParameters.mEnabled.mValue == 0) return nullptr;
    gabi::FrameLocal<DimensionScratch> conversion(8);
    u32 count = mFrameCount;
    FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    u32 key = configurationKey(frameIndex);
    bool ready;
    if (frame->mConfigurationKey == key) {
        ready = frame->mReady != 0;
    } else {
        frame->mConfigurationKey = key;
        frame->mReady = 0;
        ready = false;
    }
    f32 scale = frame->mResolutionScale;
    u32 scaledWidth = scaledDimension(conversion.get(), width, scale);
    u32 scaledHeight = scaledDimension(conversion.get(), height, scale);
    if (!ready) {
        u32 mode = mParameters.mResolution.mValue;
        if (mode == 1) {
            scaledWidth >>= 1;
            scaledHeight >>= 1;
        } else if (mode == 2) {
            scaledWidth >>= 2;
            scaledHeight >>= 2;
        }
        prepareFrame(frameIndex, scaledWidth, scaledHeight, 0);
    }
    return outputTarget(frameIndex, 0);
}

// GHS's unsigned conversion uses the original frame's two-word scratch area.
static u32 scaledDimension(DimensionScratch* scratch, u32 dimension, f32 scale) {
    scratch->mLow = dimension;
    scratch->mHigh = 0x43300000;
    f64 wide = *reinterpret_cast<const be<f64>*>(&scratch->mHigh) - 4503599627370496.0;
    f32 rounded = gabi::f32_from_fpr(wide);
    f32 value = gabi::fmuls_ppc(rounded, scale);
    constexpr f32 kUnsignedSplit = 2147483648.0f;
    if (!(value < kUnsignedSplit)) {
        f32 low = gabi::fsubs_ppc(value, kUnsignedSplit);
        scratch->mHigh = static_cast<u32>(gabi::ftoi(low));
        return static_cast<u32>(scratch->mHigh) + 0x80000000u;
    }
    scratch->mHigh = static_cast<u32>(gabi::ftoi(value));
    return scratch->mHigh;
}

/* 0278FEBC */
void ShadowPrePass::swapBuffers() {
    u32 count = mFrameCount;
    u8 previous = mSwapIndex;
    FrameResources* frame = mFrames;
    u8 index = static_cast<u8>(1 - previous);
    FrameResources* end = guestElement(frame, count);
    mSwapIndex = index;
    while (frame != end) {
        u32 firstStride = frame->mBuffers[0].stride;
        u32 firstBase = gabi::ea(frame->mBuffers[0].gpuData.get());
        u32 firstOffset = firstStride * index;
        u32 secondStride = frame->mBuffers[1].stride;
        frame->mBuffers[0].cpuData.v = firstBase + firstOffset;
        index = mSwapIndex;
        u32 secondBase = gabi::ea(frame->mBuffers[1].gpuData.get());
        u32 secondOffset = secondStride * index;
        u32 thirdStride = frame->mBuffers[2].stride;
        frame->mBuffers[1].cpuData.v = secondBase + secondOffset;
        index = mSwapIndex;
        u32 thirdOffset = thirdStride * index;
        u32 thirdBase = gabi::ea(frame->mBuffers[2].gpuData.get());
        frame->mBuffers[2].cpuData.v = thirdBase + thirdOffset;
        ++frame;
        if (frame == end) break;
        index = mSwapIndex;
    }
}

/* 02790450 */
ColorTarget* ShadowPrePass::bindOutputTarget(u32 frameIndex, u32 pass) {
    gabi::AutoFrame frameContext{0x02790450u};
    gabi::FrameLocal<sead::Viewport> viewport(8);
    ColorTarget* target = outputTarget(frameIndex, pass);
    sead::activateFrameBuffer(frameDisplay(&target->mFrame));
    sead::Viewport::createSurface(viewport.get(), frameSize(&target->mFrame));
    viewport->apply(frameSurface(&target->mFrame));
    return target;
}

/* 02790B3C */
agl::TextureSampler* ShadowPrePass::outputSampler(u32 frameIndex, u32 pass) {
    gabi::AutoFrame frameContext{0x02790B3Cu};
    u32 count = mFrameCount;
    FrameResources* frame = mFrames;
    if (frameIndex < count) frame = guestElement(frame, frameIndex);
    ReadTargetSelection selection = chooseReadTarget(passSelection(), pass);
    if (selection == ReadTargetSelection::Alternate) {
        frame->mAlternateTarget.mSurface.invalidate();
        return &frame->mAlternateSampler;
    }
    if (selection == ReadTargetSelection::Primary)
        frame->mPrimaryTarget.mSurface.invalidate();
    return &frame->mPrimarySampler;
}

agl::DynamicTextureCache* shadowTextureCache() { return sShadowTextureCache; }

/* WWHD glue
data sShadowTextureCache 101F8DD4
data sPrePassObjectTable 10165FC4
data sPrePassControlTable 10165FDC
data sPrePassParameterTable 1016590C
data sPrePassParameterControlTable 101658EC
data sPrePassParameterLinkTable 101658FC
ext 027B997C void clearShadowRenderTarget(agl::RenderBuffer*,u32,u32,const sead::ColorChannels*,u32,f64)
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
ext 027B9694 void destroyShadowFrameTarget(agl::RenderBuffer*,u32) callback
ext 028F0164 void destroyShadowFrameArray(void*,u32,u32,u32,u32,u32)
*/
