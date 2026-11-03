// J3D HD renderer: shader uniform-block location tables and the binding helper.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderUniformPair.h"
namespace j3d {
// Uniform-block locations of one shader program per stage; -1 = not used by that stage.
struct RenderUniformBinding_l {
    u8 mBindingIdentity[12];
    be<s16> mVertexLocation;
    be<s16> mPixelLocation;
    be<s16> mGeometryLocation;
    u8 mPadding[2];
};
struct RenderUniformLocations_l {
    u8 mOwnerState[12];
    be<u32> mCount;
    gptr<RenderUniformBinding_l> mBindings;
};
struct RenderUniformBindEntries {
    static void bindFrame(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations);
    static void bindMatrix(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations);
    static void bindMaterial(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations);
    static void bindScene(RenderUniformPair* uniforms, const RenderUniformLocations_l* locations);
};
}
WWHD_OFFSET(j3d::RenderUniformBinding_l, mVertexLocation, 12);
WWHD_OFFSET(j3d::RenderUniformBinding_l, mPixelLocation, 14);
WWHD_OFFSET(j3d::RenderUniformBinding_l, mGeometryLocation, 16);
WWHD_SIZE(j3d::RenderUniformBinding_l, 20);
WWHD_OFFSET(j3d::RenderUniformLocations_l, mCount, 12);
WWHD_OFFSET(j3d::RenderUniformLocations_l, mBindings, 16);
WWHD_SIZE(j3d::RenderUniformLocations_l, 20);
// Cafe SDK (gx2) imports.
void GX2SetVertexUniformBlock(u32 location, u32 size, const void* data);
void GX2SetPixelUniformBlock(u32 location, u32 size, const void* data);
void GX2SetGeometryUniformBlock(u32 location, u32 size, const void* data);

// Uniform-block slots of a program's binding table.
enum RenderUniformSlot {
    RENDER_UNIFORM_SLOT_FRAME = 0,
    RENDER_UNIFORM_SLOT_MATRIX = 2,
    RENDER_UNIFORM_SLOT_MATERIAL = 3,
    RENDER_UNIFORM_SLOT_SCENE = 4,
};

// Binds the buffer the pair uploaded last (the previous one) at the slot's locations.
// The slot record is read without a NULL test, as in the original.
static inline void bindPreviousUniformBlock(j3d::RenderUniformPair* uniforms,
                                            const j3d::RenderUniformLocations_l* locations, u32 slot) {
    agl::RenderUniformBuffer_l* buffer = &uniforms->mBuffers[(u32)uniforms->mPreviousIndex];
    const j3d::RenderUniformBinding_l* binding = j3d::guestNull<j3d::RenderUniformBinding_l>();
    if (slot < (u32)locations->mCount) binding = &locations->mBindings.get()[slot];
    s16 vertex = binding->mVertexLocation;
    s16 pixel = binding->mPixelLocation;
    s16 geometry = binding->mGeometryLocation;
    const void* data = buffer->mCpuStorage;
    u32 size = buffer->mStride;
    if (vertex == -1 && pixel == -1 && geometry == -1) return;
    if (pixel != -1) GX2SetPixelUniformBlock(pixel, size, data);
    if (vertex != -1) GX2SetVertexUniformBlock(vertex, size, data);
    if (geometry != -1) GX2SetGeometryUniformBlock(geometry, size, data);
}
