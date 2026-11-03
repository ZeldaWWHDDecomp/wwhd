/* Model builder fields and resource dictionaries observed in this unit. */
#pragma once
#include "agl/gx2/BufferRuntime.h"
namespace agl { namespace gx2util {
struct VertexResource;
/* A model of a resource file: its skeleton, vertex buffers, and the dictionaries of its parameter views
   (ParameterDescriptor values) and materials (UniformDescriptor values), with their counts. */
struct ModelDescriptor {
    u8 mPrefix[12];
    /* +0x0C */ ResourceLink<PoseResource> mPose;
    /* +0x10 */ ResourceLink<VertexResource> mVertices;
    /* +0x14 */ ResourceLink<ResourceDictionary> mViews;
    /* +0x18 */ ResourceLink<ResourceDictionary> mUniforms;
    u8 mCountsHeader[4];
    /* +0x20 */ be<u16> mVertexCount;
    /* +0x22 */ be<u16> mViewCount;
    /* +0x24 */ be<u16> mUniformCount;
    void initialize(); u32 resolveTextures(ResourceArchive* archive);
};
struct ModelBuilder;
struct ModelLayout {
    gptr<LayoutBlock> mBlocks; be<u32> mTotal; LayoutBlock mSlots[9];
    void calculate(ModelBuilder* builder);
};
struct ModelBuilder {
    gptr<ModelDescriptor> mResource; gptr<PaletteBuffer> mExternalPose;
    be<u32> mPoseViews; be<u32> mParameterViews; be<u32> mUniformViews;
    be<u32> mInstanceCount; be<u32> mCustomBytes;
    be<u8> mCached; u8 mReserved[3]; ModelLayout mLayout;
    u32 workSize();
};
WWHD_OFFSET(ModelDescriptor, mPose, 0xC); WWHD_OFFSET(ModelDescriptor, mVertices, 0x10);
WWHD_OFFSET(ModelDescriptor, mViews, 0x14); WWHD_OFFSET(ModelDescriptor, mUniforms, 0x18);
WWHD_OFFSET(ModelDescriptor, mVertexCount, 0x20); WWHD_OFFSET(ModelDescriptor, mViewCount, 0x22);
WWHD_OFFSET(ModelDescriptor, mUniformCount, 0x24);
WWHD_OFFSET(ModelBuilder, mLayout, 0x20);
WWHD_SIZE(ModelLayout, 0x50);
WWHD_SIZE(ModelBuilder, 0x70);
} }
