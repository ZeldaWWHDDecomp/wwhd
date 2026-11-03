// agl::utl::PrimitiveTexture (name from the bootstrap heap string): the singleton (pointer 101F8DEC, its
// registered disposer at 101F8DF0) holding sixteen tiny built-in textures (single colours, depth values, a
// mip-level colour pattern) with a sampler each. Layout from WWHD's constructor (size 0x94) and initializer.
#pragma once
#include "agl/TextureSampler.h"
#include "sead/HeapRegistry.h"
namespace agl {

// the built-in textures, by sampler slot
enum PrimitiveTextureType : u32 {
    PrimitiveTexture_White2D,
    PrimitiveTexture_Gray2D,
    PrimitiveTexture_Black2D,
    PrimitiveTexture_Clear2D,
    PrimitiveTexture_BlackCube,
    PrimitiveTexture_WhiteCube,
    PrimitiveTexture_Black1D,
    PrimitiveTexture_Black1DArray,
    PrimitiveTexture_Black2DArray,
    PrimitiveTexture_Black3D,
    PrimitiveTexture_BlackCubeArray,
    PrimitiveTexture_Depth0,
    PrimitiveTexture_Depth1,
    PrimitiveTexture_MipLevel,
    PrimitiveTexture_DepthShadow,
    PrimitiveTexture_DepthShadowArray,
    PrimitiveTexture_Count
};
// texture formats (agl codes) and surface dimensions the initializer uses
enum : u32 { PrimitiveTextureFormat_Color = 0x1D, PrimitiveTextureFormat_Depth = 0x3A };
enum : u32 {
    TextureDimension_1D = 0, TextureDimension_2D = 1, TextureDimension_3D = 2, TextureDimension_Cube = 3,
    TextureDimension_1DArray = 4, TextureDimension_2DArray = 5,
};

struct PrimitiveTextureDisposer : sead::RegisteredObject { void destroy(u32 flags); };

struct PrimitiveTextureVtable {
    sead::VtableSlot mHeader;
    sead::VtableSlot mReserved;
    sead::VtableSlot mDestroy;
};

struct PrimitiveTexture {
    gptr<PrimitiveTextureVtable> mMethods;
    PrimitiveTextureDisposer mDisposer;
    gptr<TextureSampler> mSamplers[PrimitiveTexture_Count];
    gptr<void> mImages[PrimitiveTexture_Count];
    static PrimitiveTexture* create(PrimitiveTexture* storage);
    static PrimitiveTexture* instance(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void initializeMipLevelTexture(sead::RegistryHeap* heap);
    void initialize(sead::RegistryHeap* heap);
};

// an interface emitted in this translation unit with only a deleting destructor and an empty hook; no WWHD code
// constructs it
struct PrimitiveTextureInterface {
    gptr<void> mMethods;
    void destroy(u32 flags);
    void emptyHook();
};
}
WWHD_OFFSET(agl::PrimitiveTexture, mDisposer, 4);
WWHD_OFFSET(agl::PrimitiveTexture, mSamplers, 0x14);
WWHD_OFFSET(agl::PrimitiveTexture, mImages, 0x54);
WWHD_SIZE(agl::PrimitiveTexture, 0x94);
WWHD_OFFSET(agl::PrimitiveTextureVtable, mDestroy, 0x10);
