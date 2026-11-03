// agl depth of field: the multi-context parameter host it derives from. It owns one draw context per
// graphics context and one settings object per view (the first embedded, the others allocated together).
#include "bindings.h"
#include "agl/DepthOfFieldDraw.h"

using namespace agl::dof;

static u32 sContextHostMethods;
using HeapManagerPointer = gptr<sead::RegistryManagerState>;
static HeapManagerPointer sHeapManager;
static void* allocateObject(u32 size);
static void constructParameterIo(ContextHost* host);
static void initializeParameterIo(ContextHost* host, s32 contextCount, u32 sharedSettings, sead::RegistryHeap* heap);
static void destroyParameterIo(ContextHost* host, u32 flags);
static sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager);
static sead::RegistryHeap* findContainingHeap(sead::RegistryManagerState* manager, void* address);
static agl::TextureSampler* constructSampler(agl::TextureSampler* sampler);
static agl::RenderBuffer* constructRenderBuffer(agl::RenderBuffer* buffer);
static void constructColorTarget(agl::ColorSurface* target);
static void destroySampler(agl::TextureSampler* sampler, u32 flags);
static void destroyRenderBuffer(agl::RenderBuffer* buffer, u32 flags);
// AGL#4's half of this translation unit: the parameter object's constructor and registration and the
// per-context target reset (the reset ignores the heap this caller passes).
static Parameters* constructDepthBlurParameters(Parameters* parameters);
static void setupDepthBlurParameters(Parameters* parameters, agl::parameter_tree::Value* archive, sead::RegistryHeap* heap);
static void resetDepthBlurTargets(DepthOfField* runtime, ContextTargets* targets, sead::RegistryHeap* heap);

static void* allocateFrom(sead::RegistryHeap* heap, u32 size) {
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, 4u);
}
static void releaseTo(void* storage, sead::RegistryHeap* heap) {
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
}

/* 02782EF8 */
void ContextHost::destroy(ContextHost* host, u32 flags) {
    if (host == NULL) return;
    host->mMethods = &sContextHostMethods;
    if (host->mContexts) {
        for (s32 index = 0; index < (s32)host->mContextArrayCount; ++index) {
            ContextTargets* context = gabi::at<ContextTargets>(gabi::ea(host->mContexts.get()) + index * sizeof(ContextTargets));
            if (context == NULL) continue;
            destroySampler(&context->mCombined, 2);
            destroySampler(&context->mDepthTarget, 2);
            destroySampler(&context->mBlurTarget, 2);
            destroyRenderBuffer(&context->mMipOutput, 2);
            destroySampler(&context->mSecondary, 2);
            destroySampler(&context->mPrimary, 2);
        }
        sead::RegistryHeap* heap = findContainingHeap(sHeapManager, host->mContexts);
        releaseTo(host->mContexts, heap);
        host->mContextArrayCount = 0;
        host->mContexts = NULL;
    }
    if (host->mViewSettings) {
        sead::RegistryHeap* heap = findContainingHeap(sHeapManager, host->mViewSettings);
        releaseTo(host->mViewSettings, heap);
        host->mViewSettingsCount = 0;
        host->mViewSettings = NULL;
    }
    if (host->mExtraSettings) {
        sead::RegistryHeap* heap = findContainingHeap(sHeapManager, host->mExtraSettings);
        releaseTo(host->mExtraSettings, heap);
        host->mExtraSettingsCount = 0;
        host->mExtraSettings = NULL;
    }
    destroyParameterIo(host, 0);
    if (flags & 1) operator_delete(host);
}

/* 02783074 */
ContextHost* ContextHost::construct(ContextHost* host) {
    if (host == NULL) host = (ContextHost*)allocateObject(sizeof(ContextHost));
    if (host == NULL) return host;
    constructParameterIo(host);
    constructDepthBlurParameters(&host->mSettings);
    host->mContexts = NULL;
    host->mExtraSettings = NULL;
    host->mViewSettings = NULL;
    host->mExtraSettingsCount = 0;
    host->mContextArrayCount = 0;
    host->mViewSettingsCount = 0;
    return host;
}

/* 027830E0 */
void ContextHost::initialize(s32 contextCount, u32 sharedSettings, sead::RegistryHeap* heap) {
    // With a single graphics context every view can share the first settings object.
    if (sharedSettings == 0 && contextCount <= 1) sharedSettings = 1;
    initializeParameterIo(this, contextCount, sharedSettings, heap);

    s32 count = mContextCount;
    if (count > 0) {
        sead::RegistryHeap* from = heap != NULL ? heap : currentHeap(sHeapManager);
        ContextTargets* contexts = (ContextTargets*)allocateFrom(from, count * sizeof(ContextTargets));
        u32 remaining = count > 0 ? (u32)count : 1;
        u32 offset = 0;
        do {
            ContextTargets* context = gabi::at<ContextTargets>(gabi::ea(contexts) + offset);
            if (context != NULL) {
                constructSampler(&context->mPrimary);
                constructSampler(&context->mSecondary);
                constructRenderBuffer(&context->mMipOutput);
                constructColorTarget(&context->mMipColor);
                constructSampler(&context->mBlurTarget);
                constructSampler(&context->mDepthTarget);
                constructSampler(&context->mCombined);
            }
            offset += sizeof(ContextTargets);
        } while (--remaining != 0);
        if (contexts != NULL) {
            mContextArrayCount = count;
            mContexts = contexts;
        }
    }
    u32 cursor = gabi::ea(mContexts.get());
    u32 end = cursor + mContextArrayCount * sizeof(ContextTargets);
    for (; cursor != end; cursor += sizeof(ContextTargets))
        resetDepthBlurTargets(static_cast<DepthOfField*>(this), gabi::at<ContextTargets>(cursor), heap);

    if (sharedSettings == 0) {
        s32 extraCount = mViewCount - 1;
        if (extraCount > 0) {
            sead::RegistryHeap* from = heap != NULL ? heap : currentHeap(sHeapManager);
            Parameters* extra = (Parameters*)allocateFrom(from, extraCount * sizeof(Parameters));
            u32 remaining = extraCount > 0 ? (u32)extraCount : 1;
            u32 offset = 0;
            do {
                Parameters* settings = gabi::at<Parameters>(gabi::ea(extra) + offset);
                if (settings != NULL) constructDepthBlurParameters(settings);
                offset += sizeof(Parameters);
            } while (--remaining != 0);
            if (extra != NULL) {
                mExtraSettings = extra;
                mExtraSettingsCount = extraCount;
            }
        }
    }
    s32 viewCount = mViewCount;
    if (viewCount > 0) {
        sead::RegistryHeap* from = heap != NULL ? heap : currentHeap(sHeapManager);
        gptr<Parameters>* table = (gptr<Parameters>*)allocateFrom(from, viewCount * sizeof(gptr<Parameters>));
        if (table != NULL) {
            mViewSettings = table;
            mViewSettingsCount = viewCount;
        }
    }
    for (s32 view = 0; view < mViewCount; ++view) {
        gptr<Parameters>* slot = mViewSettings;
        if ((u32)view < mViewSettingsCount) slot += view;
        Parameters* settings;
        if (view == 0) {
            settings = gabi::ea(this) != 0 ? &mSettings : NULL;   // null-preserving base conversion
        } else {
            settings = mExtraSettings;
            if ((u32)(view - 1) < mExtraSettingsCount) settings += view - 1;
        }
        *slot = settings;
        gptr<Parameters>* registered = mViewSettings;
        if ((u32)view < mViewSettingsCount) registered += view;
        agl::parameter_tree::Value* name = mViewNames;
        if ((u32)view < mViewNameCount) name += view;
        setupDepthBlurParameters(*registered, name, heap);
    }
}

/* WWHD glue
 data sContextHostMethods 101626C8
 data sHeapManager 101F8B4C
 ext 0273AD10 void* allocateObject(u32)
 ext 0279A730 void constructParameterIo(agl::dof::ContextHost*)
 ext 0279A86C void initializeParameterIo(agl::dof::ContextHost*, s32, u32, sead::RegistryHeap*)
 ext 0279A7D8 void destroyParameterIo(agl::dof::ContextHost*, u32)
 ext 02756140 sead::RegistryHeap* currentHeap(sead::RegistryManagerState*)
 ext 02755FEC sead::RegistryHeap* findContainingHeap(sead::RegistryManagerState*, void*)
 ext 027BDF7C agl::TextureSampler* constructSampler(agl::TextureSampler*)
 ext 027B95B4 agl::RenderBuffer* constructRenderBuffer(agl::RenderBuffer*)
 ext 027B6CB8 void constructColorTarget(agl::ColorSurface*)
 ext 027BE2B0 void destroySampler(agl::TextureSampler*, u32)
 ext 027B9694 void destroyRenderBuffer(agl::RenderBuffer*, u32)
 ext 0277D354 Parameters* constructDepthBlurParameters(Parameters* parameters)
 ext 0277DD2C void setupDepthBlurParameters(Parameters* parameters, agl::parameter_tree::Value* archive, sead::RegistryHeap* heap)
 ext 0277DA88 void resetDepthBlurTargets(DepthOfField* runtime, ContextTargets* targets, sead::RegistryHeap* heap)
*/
