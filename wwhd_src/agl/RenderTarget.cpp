/* Render-target operations from the three assigned WWHD bodies. */
#include "agl/RenderTarget.h"
#include "agl/TextureData.h"
#include "sead/HeapRegistry.h"
using namespace agl;
namespace agl {
struct RenderColorTarget_l {
    TextureData texture;
    be<u8> dirty;
    u8 reserved[3];
    be<u32> viewMip, viewFirstSlice;
    u8 auxiliaryState[16];
    sead::GraphicsColorBuffer native;
};
struct RenderDepthTarget_l {
    TextureData texture;
    be<u8> dirty;
    u8 reserved[3];
    be<u32> viewMip, viewFirstSlice;
    u8 hizState[12];
    sead::GraphicsDepthBuffer native;
};
}
WWHD_OFFSET(RenderColorTarget_l, dirty, 0x90);
WWHD_OFFSET(RenderColorTarget_l, viewMip, 0x94);
WWHD_OFFSET(RenderColorTarget_l, viewFirstSlice, 0x98);
WWHD_OFFSET(RenderColorTarget_l, native, 0xAC);
WWHD_SIZE(RenderColorTarget_l, 0x148);
WWHD_OFFSET(RenderDepthTarget_l, dirty, 0x90);
WWHD_OFFSET(RenderDepthTarget_l, native, 0xA8);
WWHD_SIZE(RenderDepthTarget_l, 0x154);
struct CoreTargetSlots_l {
    be<u32> count;
    gptr<gptr<RenderTarget>> data;
};
struct CoreTargetCount_l {
    be<s32> value;
};
struct CoreTargetMapping_l {
    be<u32> indices[3];
};
struct RenderEnginePointer_l {
    gptr<void> value;
};
static CoreTargetSlots_l sCoreTargetSlots;
static CoreTargetCount_l sCoreTargetCount;
static CoreTargetMapping_l sCoreTargetMapping;
static RenderEnginePointer_l sRenderEngine;
static u32 currentCore();
static void setDepthClear(sead::GraphicsDepthBuffer*, u32, f64);
static void clearColor(sead::GraphicsColorBuffer*, f32, f32, f32, f32);
static void clearColorDepth(sead::GraphicsColorBuffer*, sead::GraphicsDepthBuffer*, u32, u32, f32, f32, f32, f32, f64);
static void clearDepth(sead::GraphicsDepthBuffer*, u32, u32, f64);
static void flushRenderEngine(void*);
static void invalidateColorTarget(RenderColorTarget_l*);
static void invalidateDepthTarget(RenderDepthTarget_l*);
static void initializeColorTarget(RenderColorTarget_l*);
static void initializeDepthTarget(RenderDepthTarget_l*);
static void initializeColorBuffer(sead::GraphicsColorBuffer*);
static void setColorBuffer(sead::GraphicsColorBuffer*, u32);
static void setDepthBuffer(sead::GraphicsDepthBuffer*);
/* 027B997C */
static void clearRenderTarget(
    RenderTarget* target, u32 index, u32 flags, const RenderClearColor_l* color, u32 stencil, f64 depth) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!flags) return;
    u32 selected = index < 8 ? index : 0;
    if (index == 0) {
        RenderDepthTarget_l* d = target->depth;
        if (d) setDepthClear(&d->native, stencil & 255, depth);
        if (flags & 1) {
            RenderColorTarget_l* c = target->colors[0];
            if (!(flags & 6))
                clearColor(&c->native, color->red, color->green, color->blue, color->alpha);
            else {
                d = target->depth;
                u32 mode = (flags & 2) ? ((flags & 4) ? 3 : 1) : 2;
                clearColorDepth(&c->native, &d->native, stencil & 255, mode, color->red, color->green, color->blue,
                    color->alpha, depth);
            }
        } else {
            d = target->depth;
            u32 mode = (flags & 2) ? ((flags & 4) ? 3 : 1) : 2;
            clearDepth(&d->native, stencil & 255, mode, depth);
        }
    } else if (flags & 1) {
        RenderColorTarget_l* c = target->colors[selected];
        clearColor(&c->native, color->red, color->green, color->blue, color->alpha);
    }
    flushRenderEngine(sRenderEngine.value);
    RenderColorTarget_l* c = target->colors[selected];
    if (c && (flags & 1)) invalidateColorTarget(c);
    RenderDepthTarget_l* d = target->depth;
    if (d && (flags & 6)) invalidateDepthTarget(d);
}
/* 027B9C1C */
static void initializeCoreTargets(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 desired = sCoreTargetCount.value;
    if (desired > 0) {
        if (!heap) heap = registryCurrentHeap(sRegistryManager.get());
        auto* allocation = gabi::call_ptr<gptr<RenderTarget>*>(
            heap->dispatch()->mAllocate.mFunction.get(), heap, static_cast<u32>(desired) << 2, 4u);
        if (allocation) {
            sCoreTargetSlots.count = static_cast<u32>(desired);
            sCoreTargetSlots.data = allocation;
        }
    }
    u32 i = 0;
    while (static_cast<s32>(i) < static_cast<s32>(static_cast<u32>(sCoreTargetSlots.count))) {
        u32 count = sCoreTargetSlots.count;
        gptr<RenderTarget>* entries = sCoreTargetSlots.data;
        entries[i < count ? i : 0] = nullptr;
        ++i;
    }
}
/* 027B9D00 */
static void bindRenderTarget(RenderTarget* target, u32 mask) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::GraphicsColorBuffer> temporary(8);
    u32 core = currentCore(), slot = sCoreTargetMapping.indices[core], count = sCoreTargetSlots.count;
    gptr<RenderTarget>* entries = sCoreTargetSlots.data;
    entries[slot < count ? slot : 0] = target;
    for (u32 index = 0; index < 8; ++index) {
        RenderColorTarget_l* c = target->colors[index];
        if (!c) continue;
        u32 mip = c->viewMip;
        if (mask & (1u << index)) {
            *temporary.get() = c->native;
            temporary->mSurface.mFormat = static_cast<u32>(temporary->mSurface.mFormat) | 0x400;
            temporary->mViewMip = mip;
            c = target->colors[index];
            temporary->mFirstSlice = c->viewFirstSlice;
            temporary->mSliceCount = 1;
            initializeColorBuffer(temporary.get());
            setColorBuffer(temporary.get(), index);
        } else {
            if (static_cast<u8>(c->dirty)) {
                initializeColorTarget(c);
                c->dirty = 0;
                c = target->colors[index];
            }
            setColorBuffer(&c->native, index);
        }
    }
    RenderDepthTarget_l* d = target->depth;
    if (d) {
        if (static_cast<u8>(d->dirty)) {
            initializeDepthTarget(d);
            d->dirty = 0;
            d = target->depth;
        }
        setDepthBuffer(&d->native);
    }
}
/* WWHD glue
data sCoreTargetSlots 104A2090
data sCoreTargetCount 101F8B5C
data sCoreTargetMapping 104A0BC0
data sRenderEngine 101F8BD8
ext C0009BB0 u32 currentCore()
ext C00067C0 void setDepthClear(sead::GraphicsDepthBuffer*,u32,f64)
ext C00060F8 void clearColor(sead::GraphicsColorBuffer*,f32,f32,f32,f32)
ext C00060F0 void clearColorDepth(sead::GraphicsColorBuffer*,sead::GraphicsDepthBuffer*,u32,u32,f32,f32,f32,f32,f64)
ext C0006100 void clearDepth(sead::GraphicsDepthBuffer*,u32,u32,f64)
ext 0276B114 void flushRenderEngine(void*)
ext 027B6D64 void invalidateColorTarget(RenderColorTarget_l*)
ext 027B6F18 void invalidateDepthTarget(RenderDepthTarget_l*)
ext 027B6DDC void initializeColorTarget(RenderColorTarget_l*)
ext 027B6F90 void initializeDepthTarget(RenderDepthTarget_l*)
ext C00063D0 void initializeColorBuffer(sead::GraphicsColorBuffer*)
ext C00067C8 void setColorBuffer(sead::GraphicsColorBuffer*,u32)
ext C0006838 void setDepthBuffer(sead::GraphicsDepthBuffer*)
*/
