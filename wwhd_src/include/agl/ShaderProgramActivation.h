// Accessors for the parts of agl::ShaderProgram that GHS inlined program activation and uniform
// setters read in their callers (depth of field, bloom, colour correction). Offsets come from those
// callers' WWHD loads; the types are the merged canonical ones.
#pragma once
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
#include "agl/RenderRecording.h"

// A program's shared data (variant domain, shader mode) is ShaderProgram::binary.
inline agl::ShaderProgramSharedData* programSharedData(const agl::ShaderProgram* program) {
    return static_cast<agl::ShaderProgramSharedData*>(program->binary.get());
}
// ShaderProgram::configuration is the program's agl::DisplayListRecording (0x1C bytes; AGL#14's
// ShaderProgramConfiguration_l is an opaque view of it): its data pointer and used size are the
// recorded display list.
inline const agl::DisplayListStorage* programDisplayList(const agl::ShaderProgram* program) {
    return reinterpret_cast<const agl::DisplayListStorage*>(&program->configuration);
}
// Byte offsets of those words in the program, for reads through a guest address that may be 0.
constexpr u32 kProgramDisplayListData = offsetof(agl::ShaderProgram, configuration) + offsetof(agl::DisplayListStorage, mData);
constexpr u32 kProgramDisplayListSize = offsetof(agl::ShaderProgram, configuration) + offsetof(agl::DisplayListStorage, mUsed);
static_assert(kProgramDisplayListData == 4 && kProgramDisplayListSize == 0xC, "program display-list words");
// ShaderProgram::flags bit: a pending resource update, applied before the program is used.
enum : u8 { ShaderProgramFlag_UpdatePending = 2 };

// One entry of ShaderProgram::uniforms or ::uniformBlocks (ProgramResource16_l: name, methods, then the vertex,
// pixel and geometry register locations, -1 when unused). The locations are read signed.
struct UniformLocations_l {
    u8 mNameAndMethods[8];
    be<s16> mVertexRegister, mPixelRegister, mGeometryRegister;
    u8 mTail[2];
};
WWHD_SIZE(UniformLocations_l, 16);
WWHD_OFFSET(UniformLocations_l, mVertexRegister, 8);
WWHD_OFFSET(UniformLocations_l, mPixelRegister, 0xA);
WWHD_OFFSET(UniformLocations_l, mGeometryRegister, 0xC);
static_assert(sizeof(UniformLocations_l) == sizeof(agl::ProgramResource16_l), "uniform entry stride");
inline UniformLocations_l* programUniforms(const agl::ShaderProgram* program) {
    return reinterpret_cast<UniformLocations_l*>(program->uniforms.get());
}
inline UniformLocations_l* programUniformBlocks(const agl::ShaderProgram* program) {
    return reinterpret_cast<UniformLocations_l*>(program->uniformBlocks.get());
}
// The bounds-checked uniform lookup: an index past the table falls back to the first entry.
inline const UniformLocations_l* programUniform(const agl::ShaderProgram* program, u32 index) {
    UniformLocations_l* uniforms = programUniforms(program);
    if (program->uniformCount > index) uniforms += index;
    return uniforms;
}
