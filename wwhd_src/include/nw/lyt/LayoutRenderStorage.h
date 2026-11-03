/* Layout rendering storage around shader programs and the shared quad buffers. */
#pragma once
#include "nw/lyt/LayoutGpuResources.h"
namespace nw { namespace lyt {
using LayoutShaderBundle = nw::ut::QuadRenderer;
struct LayoutRenderStorage {
    be<u32> vertexShaderCount,pixelShaderCount;
    LayoutShaderBundle bundle;
    gptr<LayoutTextureCoordinates> quadCoordinates;
    gptr<be<u16>> quadIndices;
    ShaderProgramCache programCache;
    be<u8> initialized; u8 padding[3];
    static LayoutRenderStorage* create(LayoutRenderStorage*);
    void createQuadBuffers();
    void initialize(void* shaderArchive,u32 mode);
    void drawQuad();
    void bindQuadBuffer();
};
WWHD_SIZE(LayoutShaderBundle,0x24C); WWHD_SIZE(LayoutRenderStorage,0x310);
WWHD_OFFSET(LayoutRenderStorage,quadCoordinates,0x254); WWHD_OFFSET(LayoutRenderStorage,quadIndices,0x258);
WWHD_OFFSET(LayoutRenderStorage,programCache,0x25C); WWHD_OFFSET(LayoutRenderStorage,initialized,0x30C);
} }
void* renderStorageAllocate(u32 size);
void initializeLayoutShaderBundle(nw::lyt::LayoutShaderBundle* bundle);
u32 layoutShaderBundleSize(void* archive,u32 mode);
void loadLayoutShaderBundle(nw::lyt::LayoutShaderBundle*,void* memory,void* archive,u32 mode);
void renderStorageInvalidate(u32,void*,u32);
void renderStorageDrawIndexed(u32,u32,u32,const void*,u32,u32);
void renderStorageAttributeBuffer(u32,u32,u32,const void*);
