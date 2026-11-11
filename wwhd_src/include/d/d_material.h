/* HD replacement material state. */
#pragma once
#include "bindings.h"
namespace material_hd {
// HD: State is a view of agl::TextureData (agl/TextureData.h: format, then the sead::GraphicsSurface) and
// ExtendedState of agl::TextureSampler (agl/TextureSampler.h); the names follow those classes.
struct State {
    be<u32> format;  /* was field_0x0: view of agl::TextureData (agl/TextureData.h) 0x0 format: State is the TextureData that initState 027BE6B8 (TextureData::create) constructs and copyState 027BDEB4 (TextureSampler::copyTexture) copies */
    be<u32> mDimension;  /* was field_0x4: view of agl::TextureData 0x4 = surface (sead::GraphicsSurface) 0x0 mDimension; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mWidth;  /* was field_0x8: view of agl::TextureData 0x8 = surface (sead::GraphicsSurface) 0x4 mWidth; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mHeight;  /* was field_0xc: view of agl::TextureData 0xC = surface (sead::GraphicsSurface) 0x8 mHeight; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mDepth;  /* was field_0x10: view of agl::TextureData 0x10 = surface (sead::GraphicsSurface) 0xC mDepth; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mMipCount;  /* was field_0x14: view of agl::TextureData 0x14 = surface (sead::GraphicsSurface) 0x10 mMipCount; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mFormat;  /* was field_0x18: view of agl::TextureData 0x18 = surface (sead::GraphicsSurface) 0x14 mFormat; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mSamples;  /* was field_0x1c: view of agl::TextureData 0x1C = surface (sead::GraphicsSurface) 0x18 mSamples; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mUsage;  /* was field_0x20: view of agl::TextureData 0x20 = surface (sead::GraphicsSurface) 0x1C mUsage */
    be<u32> mImageBytes;  /* was field_0x24: view of agl::TextureData 0x24 = surface (sead::GraphicsSurface) 0x20 mImageBytes */
    be<u32> mImage;  /* was field_0x28: view of agl::TextureData 0x28 = surface (sead::GraphicsSurface) 0x24 mImage; transfer (in dMat_setup 02590FB8) copies it into the sampler's native texture (ExtendedState mNativeImage) when nothing else changed */
    be<u32> mMipBytes;  /* was field_0x2c: view of agl::TextureData 0x2C = surface (sead::GraphicsSurface) 0x28 mMipBytes */
    be<u32> mMipImage;  /* was field_0x30: view of agl::TextureData 0x30 = surface (sead::GraphicsSurface) 0x2C mMipImage; transfer (in dMat_setup 02590FB8) copies it into the sampler's native texture (ExtendedState mNativeMipImage) when nothing else changed */
    be<u32> mTileMode;  /* was field_0x34: view of agl::TextureData 0x34 = surface (sead::GraphicsSurface) 0x30 mTileMode; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    be<u32> mSwizzle;  /* was field_0x38: view of agl::TextureData 0x38 = surface (sead::GraphicsSurface) 0x34 mSwizzle; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update */
    u8 _03c[0x90-0x3c];
};
struct ExtendedState {
    State base;
    u8 _090[0xd4-0x90];
    be<u32> mNativeImage;  /* was field_0xd4: view of agl::TextureSampler (agl/TextureSampler.h): nativeTexture (0xB0, a GX2Texture) + 0x24, the surface image pointer; transfer (in dMat_setup 02590FB8) sets it to the source TextureData mImage when only the image pointers changed */
    be<u32> field_0xd8;
    be<u32> mNativeMipImage;  /* was field_0xdc: view of agl::TextureSampler: nativeTexture (0xB0) + 0x2C, the surface mip image pointer; transfer (in dMat_setup 02590FB8) sets it to the source TextureData mMipImage when only the image pointers changed */
    u8 _0e0[0x198-0xe0];
};
struct Control { State source[2]; ExtendedState target[2]; };
struct Model { u8 _000[0x78]; be<u8> mMaterialOverride; };
struct Morf { u8 _000[0x90]; gptr<Model> mpModel; };
struct Resource { u8 _000[0x20]; gptr<void> mpTextureRes; /* was field_0x20: dMat_setup 02590FB8 passes it (from dRes_control_c::getRes 026066C4, resources 0x92 / 0x93) to initTexture 02773798, which loads the State texture from it */ };
struct Play { u8 _0000[0x5d78]; be<u32> mDrawBufferOpa, mDrawBufferXlu; };
struct HeapVtable { u8 _000[0x24]; gfn destroy; u8 _028[4]; gfn adjustSize; };
struct Heap {
    u8 _000[0xc]; gptr<HeapVtable> __vtbl;
    void destroy() { gabi::call_ptr(__vtbl->destroy.get(), this); }
    void adjustSize() { gabi::call_ptr(__vtbl->adjustSize.get(), this); }
};
struct String { be<u32> mTextPtr, __vtbl; };
WWHD_SIZE(State,0x90); WWHD_SIZE(ExtendedState,0x198); WWHD_SIZE(Control,0x450);
WWHD_OFFSET(Model,mMaterialOverride,0x78); WWHD_OFFSET(Morf,mpModel,0x90);
}
void copyState(material_hd::ExtendedState* dst, material_hd::State* src);
void initTexture(material_hd::State* state, void* texture);
void initState(material_hd::State* state);
void initExtendedState(material_hd::ExtendedState* state);
material_hd::Resource* getResource(void* resources, material_hd::String* name, u32 index);
material_hd::Heap* createHeap(u32 size, material_hd::String* name, material_hd::Heap* parent, u32 direction, u32 flags);
material_hd::Control* allocateControl(u32 size, material_hd::Heap* heap, s32 alignment);
void updateModel(material_hd::Model* model, u32 flags);
void entryModel(material_hd::Model* model);
void updateMorf(material_hd::Morf* morf);
void entryMorf(material_hd::Morf* morf);
void entryInvisible(void* invisible);
void entryInvisibleAlternate(void* invisible);
