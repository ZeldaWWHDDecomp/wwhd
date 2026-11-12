/* The program and draw helpers GHS inlined into the SSAO and shadow-filter passes: selecting a built-in shader's
 * variant from the shader holder, ShaderProgram::activate, a uniform upload on both stages, and the draw of the
 * full-screen shape. The types are the merged agl ones (ShaderHolder, ShaderProgram, PrimitiveShapes). */
#pragma once
#include "agl/ShaderProgramActivation.h"
#include "agl/ShaderHolder.h"
#include "agl/PrimitiveShapes.h"
#include "agl/DrawProgram.h"
#include "port_native_draw.h"
// Bound by each source that uses these helpers (glue `ext` lines; the first is file-local in its own source).
void refreshEffectProgram(agl::ShaderProgram* program, u32 unused);
void GX2CallDisplayList(const void* list, u32 size);
void GX2DrawIndexedEx(u32 primitive, u32 count, u32 indexType, const void* indices, u32 baseVertex, u32 instances);
void GX2SetVertexUniformReg(u32 offset, u32 count, const void* values);
void GX2SetPixelUniformReg(u32 offset, u32 count, const void* values);
namespace agl {
// A built-in shader's variant: index <= 0 is the root program, otherwise variant index - 1 (0 past the end).
// A NULL variant table keeps PPC's NULL + index * stride address.
inline ShaderProgram* effectProgramVariant(ShaderProgram* shader, s32 index) {
    ShaderProgramSharedData* data = programSharedData(shader);
    if (index <= 0) return data->variation.root;
    u32 count = data->variation.programCount;
    u32 programs = data->variation.programs.v;
    u32 variant = u32(index) - 1 < count ? u32(index) - 1 : 0;
    return gabi::at<ShaderProgram>(programs + variant * u32(sizeof(ShaderProgram)));
}
// agl::ShaderProgram::activate as GHS inlines it: apply a pending update, switch the shader mode only on a change,
// then replay the display list (or bind directly when there is none). Returns the program's mode.
inline u32 activateEffectProgram(ShaderProgram* program, u32 currentMode) {
    u32 flags = program->flags;
    if ((flags & ShaderProgramFlag_UpdatePending) != 0) {
        program->flags = flags & ~u32(ShaderProgramFlag_UpdatePending);
        refreshEffectProgram(program, 0);
    }
    u32 mode = programSharedData(program)->mode;
    if (currentMode != mode) setProgramShaderMode(mode);
    u32 size = programDisplayList(program)->mUsed;
    if (size != 0) GX2CallDisplayList(programDisplayList(program)->mData, size);
    else program->bindStagePrograms();
    return mode;
}
// A vec4 uniform on both stages (the bounds-checked lookup; a NULL table keeps NULL + index * stride).
inline void setEffectUniform(ShaderProgram* program, u32 index, const void* values) {
    u32 count = program->uniformCount;
    u32 uniforms = gabi::ea(program->uniforms.get());
    u32 slot = count > index ? index : 0;
    auto* uniform = gabi::at<UniformLocations_l>(uniforms + slot * u32(sizeof(UniformLocations_l)));
    if ((s32)uniform->mVertexRegister != -1) GX2SetVertexUniformReg((u32)(s32)uniform->mVertexRegister, 4, values);
    if ((s32)uniform->mPixelRegister != -1) GX2SetPixelUniformReg((u32)(s32)uniform->mPixelRegister, 4, values);
}
// The full-screen shape's indexed draw.
inline void drawFullscreenShape(PrimitiveShapes* shapes) {
    const IndexBufferLayout* packet = &shapes->mIndexedShapes[DrawFullscreenShape].mIndexBuffer;
    u32 count = packet->mIndexCount;
#if PORT_NATIVE_DRAW
    // agl's full-screen shape (SSAO, shadow filter) as a packet of its own (port_native_draw.h)
    if (count != 0 && port_native_draw_single(PORT_NATIVE_AGL_QUAD, 1, gabi::ea(packet), 0, 0, packet->mPrimitiveMode,
                                              count, packet->mFormat,
                                              gabi::ea(static_cast<const void*>(packet->mStorage)), 0, 1))
        return;
#endif
    if (count != 0) GX2DrawIndexedEx(packet->mPrimitiveMode, count, packet->mFormat, packet->mStorage, 0, 1);
}
}
