// Shared own-unit API bridge declarations; definitions stay in DrawPairRendering.cpp glue.
#pragma once
#include "agl/DrawPairRendering.h"
#include "agl/PrimitiveShapes.h"
void prepareOpaquePairRender(agl::DrawResourceConfiguration_l* configuration, agl::DrawPairRenderContext_l* context,
                            u32 alternate, u32 option, u32 extra);
// ShaderProgram refresh of a pending resource update (the program-flags bit 2 path).
void initializeDrawPairProgram(agl::ShaderProgram* program, u32 option);
void callDrawPairDisplayList(void* data, u32 bytes);
// Sampler activation at one of the program's sampler locations (the buffers are the context's samplers).
void bindDrawPairBuffer(void* buffer, agl::ProgramResource16_l* binding, s32 index, u32 option);
void uploadDrawPairUniformA(s32 location, u32 bytes, const void* data);
void uploadDrawPairUniformB(s32 location, u32 bytes, const void* data);
void uploadDrawPairUniformC(s32 location, u32 bytes, const void* data);
void bindDrawPairCurveUniforms(agl::ShaderProgram* program, const agl::DrawPairParameterRecord_l* record, u32 kind, u32 index);
void drawPairIndexed(u32 primitive, u32 count, u32 indexType, void* indices, u32 baseVertex, u32 instances);
agl::ShaderProgram* selectOpaqueResourceProgram(u32 kind, u32 format, u32 alternate, u32 optionA,
    u32 optionB, u32 optionC, u32 optionD, u32 selectorDigit, u32 stackA, u32 configurationA,
    u32 stackB, u32 configurationB, u32 configurationC, u32 stackC);
