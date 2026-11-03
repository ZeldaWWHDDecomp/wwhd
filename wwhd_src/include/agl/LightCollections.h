/* The directed, spatial and paired light collections of the light prepass (agl::LightPrepass::configure 027AF16C
 * constructs them inline: method tables 1016AEF8, 1016B070 and 1016B1E8, then initializes them through their
 * `initialize` slot). They are this translation unit's instances of the agl::ParameterCollection bodies: the record
 * walks call back into the record hooks through the method table. Also the uniform-block binders the
 * record hooks call for each record kind.
 * Layouts: the merged agl/ParameterCollections.h records and values (checked against these bodies' offsets) and
 * agl::LightPrepass's construction sizes (0x118 directed, 0x114 spatial and paired). */
#pragma once
#include "wwhd.h"
#include "sead/RuntimeType.h"
#include "sead/HeapRegistry.h"
#include "agl/ParameterCollections.h"
#include "agl/ShaderProgram.h"
#include "agl/VertexInput.h"
namespace agl {

// The directed lights: extended records (0x60).
struct DirectedLightCollection : ExtendedCollection {
    be<u32> mMode;
    VertexInputSet mVertexInput;
    u8 mTail[4];
    u32 matchesType(const sead::RuntimeType* type) const;
    s32 kind() const;
    void setCount(u32 count);
    u32 isEnabled(u32 index) const;
    void setRecordEnabled(u32 index, u32 enabled);
    u32 canApply(u32 index, u32 valueIndex) const;
    // Empty hooks of the record walks.
    void beforeUpdate();
    void beforeRefresh();
    void refreshRecord();
    void prepareApply();
    void beginApply();
    void prepareContext();
};

// The spatial lights: spatial records (0x4A8) with extended values.
struct SpatialLightCollection : SpatialCollection {
    VertexInputSet mVertexInput;
    u8 mTail[4];
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap);
    void release();
    void update();
    u32 prepare(u32 valueIndex, void* context, u32 outputIndex);
    void refresh();
    void apply(u32 valueIndex, void* context, u32 invalidateGpu);
    void applyWithContext(u32 valueIndex, void* context, void* output);
    s32 kind() const;
    void setCount(u32 count);
    u32 isEnabled(u32 index) const;
    void setRecordEnabled(u32 index, u32 enabled);
    u32 canApply(u32 index, u32 valueIndex) const;
    void beforeUpdate();
    void beforeRefresh();
    void refreshRecord();
    void prepareApply();
    void beginApply();
    void prepareContext();
};

// The paired point lights: curve records (0x1C) naming two point records of the point collection each
// (agl::LightPrepass::update 027AF91C pairs them), with curve values.
struct PairedLightCollection : CurveCollection {
    VertexInputSet mVertexInput;
    u8 mTail[4];
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap);
    void release();
    void update();
    u32 prepare(u32 valueIndex, void* context, u32 outputIndex);
    void refresh();
    void apply(u32 valueIndex, void* context, u32 invalidateGpu);
    void applyWithContext(u32 valueIndex, void* context, void* output);
    s32 kind() const;
    void setCount(u32 count);
    u32 count() const;
    u32 capacity() const;
    u32 isEnabled(u32 index) const;
    void setRecordEnabled(u32 index, u32 enabled);
    u32 canApply(u32 index, u32 valueIndex) const;
    void releaseRecord();
    void beforeUpdate();
    void beforeRefresh();
    void refreshRecord();
    void prepareApply();
    void beginApply();
    void prepareContext();
};

// One uniform-block layout of the light shaders, indexed by record kind (table 1016A9A8): the program's
// uniform-block index, then the block's field list.
struct LightUniformLayout {
    be<u32> mBlockIndex;
    gptr<const void> mFields;
    be<u32> mFieldCount;
};
// One entry of ShaderProgram::uniformBlocks (ProgramResource16_l): name and methods, then the vertex, pixel and
// geometry block locations, -1 when the stage does not use the block. Read signed.
struct UniformBlockLocations_l {
    u8 mNameAndMethods[8];
    be<s16> mVertex;
    be<s16> mPixel;
    be<s16> mGeometry;
    u8 mTail[2];
};

// Binds the value's uniform buffer to the program's block for `kind` in every stage that uses it.
void bindPointUniforms(ShaderProgram* program, const CompactRecord* record, u32 kind, u32 valueIndex);
void bindDirectedUniforms(ShaderProgram* program, const ExtendedRecord* record, u32 kind, u32 valueIndex);
void bindSpatialUniforms(ShaderProgram* program, const SpatialRecord* record, u32 kind, u32 valueIndex);
void bindPairedUniforms(ShaderProgram* program, const CurveRecord* record, u32 kind, u32 valueIndex);
}

WWHD_SIZE(agl::DirectedLightCollection, 0x118);
WWHD_OFFSET(agl::DirectedLightCollection, mMode, 0x1C);
WWHD_OFFSET(agl::DirectedLightCollection, mVertexInput, 0x20);
WWHD_SIZE(agl::SpatialLightCollection, 0x114);
WWHD_OFFSET(agl::SpatialLightCollection, mVertexInput, 0x1C);
WWHD_SIZE(agl::PairedLightCollection, 0x114);
WWHD_OFFSET(agl::PairedLightCollection, mVertexInput, 0x1C);
WWHD_SIZE(agl::LightUniformLayout, 12);
WWHD_SIZE(agl::UniformBlockLocations_l, 16);
WWHD_OFFSET(agl::UniformBlockLocations_l, mVertex, 8);
WWHD_OFFSET(agl::UniformBlockLocations_l, mPixel, 0xA);
WWHD_OFFSET(agl::UniformBlockLocations_l, mGeometry, 0xC);
WWHD_SIZE(agl::ExtendedRecord, 0x60);
WWHD_OFFSET(agl::ExtendedRecord, mValueCount, 0x58);
WWHD_SIZE(agl::SpatialRecord, 0x4A8);
WWHD_OFFSET(agl::SpatialRecord, mPrimaryTexture, 0x8C);
WWHD_OFFSET(agl::SpatialRecord, mValueCount, 0x23C);
WWHD_OFFSET(agl::SpatialRecord, mProjectionTexture, 0x280);
WWHD_OFFSET(agl::SpatialRecord, mFollowingTexture, 0x310);
WWHD_SIZE(agl::CurveRecord, 0x1C);
WWHD_OFFSET(agl::CurveRecord, mValueCount, 0x14);
WWHD_SIZE(agl::CompactRecord, 0x48);
WWHD_OFFSET(agl::CompactRecord, mValueCount, 0x40);
WWHD_SIZE(agl::CompactValue, 0x5C);
WWHD_SIZE(agl::ExtendedValue, 0x68);
WWHD_OFFSET(agl::ExtendedValue, mUniform, 0x4C);
WWHD_SIZE(agl::CurveValue, 0x20);
