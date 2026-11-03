/* Uniform-block binding for the light records (the record draw hooks call one per record kind): the record's
 * value `valueIndex` (the first value when out of range) is bound to the program's block for `kind` (the first block
 * when out of range) in the pixel, vertex and geometry stages that use it. One stride-sized slot of the value's
 * uniform buffer, at its current CPU pointer, is bound.
 * References: WWHD's own instructions; GX2 uniform-block import arguments (location, size, data) from wut. */
#include "agl/LightCollections.h"
using namespace agl;
void GX2SetPixelUniformBlock(u32 location, u32 size, const void* data);
void GX2SetVertexUniformBlock(u32 location, u32 size, const void* data);
void GX2SetGeometryUniformBlock(u32 location, u32 size, const void* data);

// The uniform-block layouts of the light shaders, indexed by record kind (point, directed, spatial, paired).
struct LightUniformLayoutTable_l { LightUniformLayout mKinds[4]; };
static LightUniformLayoutTable_l sLightUniformLayouts;

template <class Record, class Value>
static void bindLightUniforms(ShaderProgram* program, const Record* record, u32 kind, u32 valueIndex) {
    const Value* value = clampedElement(record->mValues.get(), record->mValueCount, valueIndex);
    u32 blockCount = program->uniformBlockCount;
    u32 blockIndex = sLightUniformLayouts.mKinds[kind].mBlockIndex;
    const UniformBlockLocations_l* block = clampedElement(
        reinterpret_cast<UniformBlockLocations_l*>(program->uniformBlocks.get()), blockCount, blockIndex);
    s32 vertex = block->mVertex;
    u32 size = value->mUniform.stride;
    s32 pixel = block->mPixel;
    const void* data = value->mUniform.cpuData;
    s32 geometry = block->mGeometry;
    if (vertex == -1 && pixel == -1 && geometry == -1) return;
    vertex = block->mVertex;
    if (pixel != -1) GX2SetPixelUniformBlock(pixel, size, data);
    if (vertex != -1) GX2SetVertexUniformBlock(vertex, size, data);
    if (geometry != -1) GX2SetGeometryUniformBlock(geometry, size, data);
}
/* 027D395C */
void agl::bindPointUniforms(ShaderProgram* program, const CompactRecord* record, u32 kind, u32 valueIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    bindLightUniforms<CompactRecord, CompactValue>(program, record, kind, valueIndex);
}
/* 027D3A54 */
void agl::bindDirectedUniforms(ShaderProgram* program, const ExtendedRecord* record, u32 kind, u32 valueIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    bindLightUniforms<ExtendedRecord, ExtendedValue>(program, record, kind, valueIndex);
}
/* 027D3B4C */
void agl::bindSpatialUniforms(ShaderProgram* program, const SpatialRecord* record, u32 kind, u32 valueIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    bindLightUniforms<SpatialRecord, ExtendedValue>(program, record, kind, valueIndex);
}
/* 027D3C44 */
void agl::bindPairedUniforms(ShaderProgram* program, const CurveRecord* record, u32 kind, u32 valueIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    bindLightUniforms<CurveRecord, CurveValue>(program, record, kind, valueIndex);
}
/* WWHD glue
data sLightUniformLayouts 1016A9A8
ext C0006900 void GX2SetPixelUniformBlock(u32,u32,const void*)
ext C0006A38 void GX2SetVertexUniformBlock(u32,u32,const void*)
ext C00068A8 void GX2SetGeometryUniformBlock(u32,u32,const void*)
*/
