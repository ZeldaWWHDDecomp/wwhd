/* Material capacity changes and disposal, reconstructed from the WWHD binary. */
#include "nw/lyt/MaterialGpuStorage.h"
using namespace nw::lyt;
static u8 sMaterialStorageMethods;
LayoutMaterialMethods* materialGpuMethodTable() {
    return reinterpret_cast<LayoutMaterialMethods*>(&sMaterialStorageMethods);
}
static u32 materialTextures(u32 flags) { return flags>>30; }
static u32 materialTransforms(u32 flags) { return (flags>>28)&3; }
static u32 materialCoordinates(u32 flags) { return (flags>>26)&3; }
static u32 materialStages(u32 flags) { return (flags>>23)&7; }
static u32 materialAlpha(u32 flags) { return (flags>>22)&1; }
static u32 materialBlends(u32 flags) { return (flags>>20)&3; }
static u32 materialIndirect(u32 flags) { return (flags>>19)&1; }
static u32 materialIndirectTransforms(u32 flags) { return (flags>>16)&7; }
static u32 materialProjection(u32 flags) { return (flags>>15)&1; }
static u32 materialAlphaOffset(u32 flags) {
    return materialTextures(flags)*28+materialTransforms(flags)*20+materialCoordinates(flags)*8;
}
static u32 materialBlendOffset(u32 flags) { return materialAlphaOffset(flags)+materialAlpha(flags)*8; }
static u32 materialIndirectOffset(u32 flags) { return materialBlendOffset(flags)+materialBlends(flags)*4; }
static u32 materialProjectionOffset(u32 flags) {
    return materialIndirectOffset(flags)+materialIndirect(flags)*12+materialStages(flags)*4+materialIndirectTransforms(flags)*20;
}
/* 0288119C */
u32 MaterialGpu::reserve(u32 textures,u32 transforms,u32 coordinates,u32 stages,u32 alpha,u32 blends,u32 indirect,u8 indirectTransforms,u8 projection) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (materialTextures(layoutFlags)>=textures && materialTransforms(layoutFlags)>=transforms &&
        materialCoordinates(layoutFlags)>=coordinates && materialStages(layoutFlags)>=stages &&
        materialAlpha(layoutFlags)>=alpha && materialBlends(layoutFlags)>=blends &&
        materialIndirect(layoutFlags)>=indirect && materialIndirectTransforms(layoutFlags)>=indirectTransforms &&
        materialProjection(layoutFlags)>=projection) return 1;
    void* oldStorage=storage;
    if (oldStorage) {
        releaseLayoutStorage(oldStorage);
        storage=nullptr;
        clearCounts(&layoutFlags);
        clearCounts(&featureFlags);
    }
    u32 size=textures*28+transforms*20+(coordinates+alpha)*8+blends*4+indirect*12+stages*4+indirectTransforms*20+projection*8;
    void* memory=allocateLayoutStorage(size,4);
    storage=static_cast<be<u8>*>(memory);
    if (!memory) return 0;
    u32 capacity=((textures&3)<<30)|((transforms&3)<<28)|((coordinates&3)<<26)|((stages&7)<<23)|
        ((alpha&1)<<22)|((blends&3)<<20)|((indirect&1)<<19)|((static_cast<u32>(indirectTransforms)&7)<<16)|
        ((static_cast<u32>(projection)&1)<<15);
    layoutFlags=(static_cast<u32>(layoutFlags)&0x7FFF)|capacity;
    u32 count=materialTransforms(layoutFlags);
    featureFlags=(static_cast<u32>(featureFlags)&~0x30000000u)|(count<<28);
    auto* transform=layoutAtOffset<MaterialTextureTransform>(storage,materialTextures(layoutFlags)*28);
    for (u32 i=0;i<count;++i) {
        transform[i].translateX=0.0f;
        transform[i].translateY=0.0f;
        transform[i].scaleY=1.0f;
        transform[i].rotation=0.0f;
        transform[i].scaleX=1.0f;
    }
    count=materialAlpha(layoutFlags);
    featureFlags=(static_cast<u32>(featureFlags)&~0x00400000u)|(count<<22);
    if (static_cast<u32>(featureFlags)&0x00400000u) {
        auto* test=layoutAtOffset<MaterialAlphaTest>(storage,materialAlphaOffset(layoutFlags));
        test->function=7;
        test->reference=0.0f;
    }
    count=materialBlends(layoutFlags);
    featureFlags=(static_cast<u32>(featureFlags)&~0x00300000u)|(count<<20);
    if (static_cast<u32>(featureFlags)&0x00300000u) {
        auto* blend=layoutAtOffset<MaterialBlend>(storage,materialBlendOffset(layoutFlags));
        blend->value=0x00040500;
        if (materialBlends(featureFlags)==2) {
            auto* second=layoutAtOffset<MaterialBlend>(storage,materialBlendOffset(layoutFlags)+4);
            second->value=0x00040500;
        }
    }
    count=materialIndirect(layoutFlags);
    featureFlags=(static_cast<u32>(featureFlags)&~0x00080000u)|(count<<19);
    if (static_cast<u32>(featureFlags)&0x00080000u) {
        auto* parameters=layoutAtOffset<MaterialIndirect>(storage,materialIndirectOffset(layoutFlags));
        parameters->rotation=0.0f;
        parameters->scaleX=1.0f;
        parameters->scaleY=1.0f;
    }
    count=materialProjection(layoutFlags);
    featureFlags=(static_cast<u32>(featureFlags)&~0x00008000u)|(count<<15);
    if (static_cast<u32>(featureFlags)&0x00008000u) {
        auto* parameters=layoutAtOffset<MaterialProjection>(storage,materialProjectionOffset(layoutFlags));
        if (parameters) {
            materialStorageClear(parameters,0,3);
            materialStorageClear(&parameters->bytes[3],255,4);
        }
    }
    return 1;
}
/* 0288269C */
void MaterialGpu::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!this) return;
    void* memory=storage;
    methods=materialGpuMethodTable();
    if (memory) {
        releaseLayoutStorage(memory);
        storage=nullptr;
    }
    if (flags&1) materialStorageDelete(this);
}
/* WWHD glue
data sMaterialStorageMethods 10181B28
ext 0273AF40 void materialStorageDelete(void*)
ext C000A858 void materialStorageClear(void*,u32,u32)
*/
