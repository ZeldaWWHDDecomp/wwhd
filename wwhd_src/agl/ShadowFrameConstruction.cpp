// Per-frame shadow resources and their streamed sampler copy operations.
#include "agl/ShadowPrePass.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
static sead::SafeTextVtable sShadowFrameTextTable;
static sead::RenderQuaternion sShadowFrameBorder;
static const char sShadowPrePassName[14] = {0x53, 0x68, 0x61, 0x64, 0x6F, 0x77, 0x50, 0x72, 0x65, 0x50, 0x61, 0x73, 0x73, 0};
sead::RegistryHeap* currentShadowFrameHeap(sead::RegistryManagerState*);
// agl::RenderBuffer::construct / agl::ColorSurface::construct as the plain functions __construct_array calls back
// (a member function has no plain address).
void createShadowFrameTarget(agl::RenderBuffer*);
void createShadowFrameSurface(agl::ColorSurface*);
void constructShadowFrameArray(void*,u32,u32,u32);
// 027A063C is a bare `blr` shared by several callers (agl/TextureResource.cpp names it TextureReference::notify); this
// call site passes the parameter state, the frame count, a name and the heap, which it ignores.
void initializeShadowControlFrames(ParameterState*,s32,const sead::SafeText*,sead::RegistryHeap*);
static void constructFrameResources(FrameResources*);
static void copyFrameSampler(agl::TextureSampler*,const agl::TextureSampler*);
static void setShadowBorder(agl::TextureSampler*);

/* 0278E730 */
void ShadowPrePass::initializeFrames(s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frameContext{0x0278E730u};
    FrameResources* frames = nullptr;
    if (count > 0) {
        sead::RegistryHeap* allocator = heap;
        if (allocator == nullptr) allocator = currentShadowFrameHeap(sRegistryManager.get());
        u32 allocate = allocator->dispatch()->mAllocate.mFunction;
        frames = gabi::call_ptr<FrameResources*>(allocate, allocator, static_cast<u32>(count) * sizeof(FrameResources), 4);
        u32 index = 0;
        s32 remaining = count > 0 ? count : 1;
        do {
            FrameResources* current = guestElement(frames, index);
            if (current != nullptr) constructFrameResources(current);
            ++index;
        } while (--remaining != 0);
    }
    FrameResources* end;
    if (frames != nullptr) {
        mFrames = frames;
        mFrameCount = static_cast<u32>(count);
        end = guestElement(frames, static_cast<u32>(count));
    } else {
        u32 oldCount = mFrameCount;
        frames = mFrames;
        end = guestElement(frames, oldCount);
    }
    FrameResources* begin = frames;
    while (frames != end) {
        u32 index = static_cast<u32>(frames - begin);
        if (index == 0) {
            frames->mBuffers[0].allocateFormat(19, heap);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(3, 1);
            frames->mBuffers[0].appendField(5, 1);
            frames->mBuffers[0].appendField(5, 4);
            frames->mBuffers[0].appendField(5, 4);
            frames->mBuffers[0].appendField(5, 12);
            frames->mBuffers[0].appendField(2, 3);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(3, 1);
            frames->mBuffers[0].appendField(5, 4);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(3, 1);
            frames->mBuffers[0].appendField(5, 1);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(2, 1);
            frames->mBuffers[0].appendField(3, 1);
        } else frames->mBuffers[0].adoptFormat(&mFrames->mBuffers[0]);
        frames->mBuffers[0].allocateStorage(heap, 2);
        if (index == 0) {
            frames->mBuffers[1].allocateFormat(2, heap);
            frames->mBuffers[1].appendField(3, 1);
            frames->mBuffers[1].appendField(2, 1);
        } else frames->mBuffers[1].adoptFormat(&mFrames->mBuffers[1]);
        frames->mBuffers[1].allocateStorage(heap, 2);
        if (index == 0) {
            frames->mBuffers[2].allocateFormat(2, heap);
            frames->mBuffers[2].appendField(2, 1);
            frames->mBuffers[2].appendField(2, 1);
        } else frames->mBuffers[2].adoptFormat(&mFrames->mBuffers[2]);
        frames->mBuffers[2].allocateStorage(heap, 2);
        agl::TextureSampler* depth = &frames->mInitialSamplers[0];
        agl::TextureSampler* normal = &frames->mInitialSamplers[1];
        depth->settings.modes[SamplerFilterMode + 1] = 0;
        depth->settings.modes[SamplerFilterMode + 2] = 0;
        depth->settings.modes[SamplerFilterMode] = 0;
        depth->dirty = static_cast<u8>(depth->dirty | SamplerDirtyFilter);
        normal->settings.modes[SamplerClampMode] = SamplerClampBorder;
        normal->dirty = static_cast<u8>(normal->dirty | SamplerDirtyClamp);
        normal->settings.modes[SamplerClampMode + 1] = SamplerClampBorder;
        normal->settings.modes[SamplerClampMode + 2] = SamplerClampBorder;
        setShadowBorder(normal);
        normal->settings.enabled = 1;
        normal->settings.tail = SamplerCompareGreaterEqual;
        normal->dirty = static_cast<u8>(normal->dirty | SamplerDirtyCompare | SamplerDirtyBorder);
        copyFrameSampler(&frames->mInitialSamplers[2], normal);
        agl::TextureSampler* decal = &frames->mInitialSamplers[3];
        decal->settings.modes[SamplerClampMode + 1] = SamplerClampBorder;
        decal->settings.modes[SamplerClampMode + 2] = SamplerClampBorder;
        decal->dirty = static_cast<u8>(decal->dirty | SamplerDirtyClamp);
        decal->settings.modes[SamplerClampMode] = SamplerClampBorder;
        setShadowBorder(decal);
        frames->mCombinedSampler = nullptr;
        frames->mMipStorage[1] = nullptr;
        decal->dirty = static_cast<u8>(decal->dirty | SamplerDirtyBorder);
        frames->mFinalSampler.settings.modes[SamplerFilterMode] = 0;
        frames->mConfigurationKey = 0xFFFFFFFFu;
        frames->mReady = 0;
        frames->mFinalSampler.dirty = static_cast<u8>(frames->mFinalSampler.dirty | SamplerDirtyFilter);
        frames->mPrimaryStorage = nullptr;
        frames->mAlternateStorage = nullptr;
        frames->mFinalSampler.settings.modes[SamplerFilterMode + 2] = 0;
        frames->mMipStorage[0] = nullptr;
        frames->mFinalSampler.settings.modes[SamplerFilterMode + 1] = 0;
        frames->mResolutionScale = 1.0f;
        frames->mStorageChanged = 0;
        ++frames;
    }
    gabi::FrameLocal<sead::SafeText> label(8);
    label->mVtable = &sShadowFrameTextTable;
    label->mText = const_cast<be<u8>*>(reinterpret_cast<const be<u8>*>(sShadowPrePassName));
    initializeShadowControlFrames(&mParameterState, count, label.get(), heap);
}
static void constructFrameResources(FrameResources* frame) {
    agl::UniformBuffer::create(&frame->mBuffers[0]);
    agl::UniformBuffer::create(&frame->mBuffers[1]);
    agl::UniformBuffer::create(&frame->mBuffers[2]);
    for (u32 i = 0; i != 4; ++i) agl::TextureSampler::create(&frame->mInitialSamplers[i]);
    agl::TextureSampler::create(&frame->mPrimarySampler);
    frame->mPrimaryTarget.mFrame.construct();
    frame->mPrimaryTarget.mSurface.construct();
    agl::TextureSampler::create(&frame->mAlternateSampler);
    frame->mAlternateTarget.mFrame.construct();
    frame->mAlternateTarget.mSurface.construct();
    constructShadowFrameArray(frame->mMipSamplers, 2, sizeof(agl::TextureSampler), gabi::guest_fn((const void*)&agl::TextureSampler::create));
    constructShadowFrameArray(frame->mPassTargets, 2, sizeof(agl::RenderBuffer), gabi::guest_fn((const void*)&createShadowFrameTarget));
    constructShadowFrameArray(frame->mMipSurfaces, 2, sizeof(agl::ColorSurface), gabi::guest_fn((const void*)&createShadowFrameSurface));
    frame->mExtraTarget.mFrame.construct();
    frame->mExtraTarget.mSurface.construct();
    frame->mExtraDepth.construct();
    agl::TextureSampler::create(&frame->mAuxiliarySampler);
    agl::TextureSampler::create(&frame->mFinalSampler);
}
static void setShadowBorder(agl::TextureSampler* sampler) {
    be<f32>* border = &sampler->settings.scalarParameters[SamplerBorderColor];
    reinterpret_cast<be<u32>&>(border[0]) = (u32)reinterpret_cast<const be<u32>&>(sShadowFrameBorder.x);
    reinterpret_cast<be<u32>&>(border[1]) = (u32)reinterpret_cast<const be<u32>&>(sShadowFrameBorder.y);
    reinterpret_cast<be<u32>&>(border[2]) = (u32)reinterpret_cast<const be<u32>&>(sShadowFrameBorder.z);
    reinterpret_cast<be<u32>&>(border[3]) = (u32)reinterpret_cast<const be<u32>&>(sShadowFrameBorder.w);
}
static void copyFrameSampler(agl::TextureSampler* destination, const agl::TextureSampler* source) {
    const be<u32>* sourceWords = reinterpret_cast<const be<u32>*>(source);
    be<u32>* destinationWords = reinterpret_cast<be<u32>*>(destination);
    destinationWords[0] = (u32)sourceWords[0];
    for (u32 offset = 4; offset <= 0x74; offset += 4) destinationWords[offset / 4] = (u32)sourceWords[offset / 4];
    destination->texture.minimumWidth = source->texture.minimumWidth;
    destination->texture.minimumHeight = source->texture.minimumHeight;
    destination->texture.minimumDepth = source->texture.minimumDepth;
    destination->texture.maximumMipCount = source->texture.maximumMipCount;
    for (u32 offset = 0x7C; offset <= 0x88; offset += 4) destinationWords[offset / 4] = (u32)sourceWords[offset / 4];
    destination->customMapping = source->customMapping;
    destination->overrideEnabled = source->overrideEnabled;
    for (u32 offset = 0x94; offset <= 0x148; offset += 4) destinationWords[offset / 4] = (u32)sourceWords[offset / 4];
    // byte +0x14C: the sampler's custom-border flag, right after the 0x9C-byte GX2 texture in nativeTexture
    reinterpret_cast<be<u8>&>(destination->nativeTexture[0x9C]) = (u8)reinterpret_cast<const be<u8>&>(source->nativeTexture[0x9C]);
    for (u32 offset = 0x150; offset <= 0x18C; offset += 4) destinationWords[offset / 4] = (u32)sourceWords[offset / 4];
    destination->dirty = source->dirty;
}
/* WWHD glue
data sShadowFrameTextTable 101658B4
data sShadowFrameBorder 104A01EC
ext 02756140 sead::RegistryHeap* currentShadowFrameHeap(sead::RegistryManagerState*)
ext 027B95B4 void createShadowFrameTarget(agl::RenderBuffer*) callback
ext 027B6CB8 void createShadowFrameSurface(agl::ColorSurface*) callback
ext 028EFFD0 void constructShadowFrameArray(void*,u32,u32,u32)
ext 027A063C void initializeShadowControlFrames(agl::shadow_prepass::ParameterState*,s32,const sead::SafeText*,sead::RegistryHeap*)
data sShadowPrePassName 10165F20
*/
