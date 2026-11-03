// Own paired draw dispatch. Parked AGL#13/#16 calls stay opaque; no dependency implementation is reproduced.
#include "agl/DrawRenderingCalls.h"
namespace agl {
static sead::ColorChannels sDrawPairEmptyColor;
static ShaderProgram* selectedDrawPairProgram(const DrawProgramSelection_l* selection) {
    ShaderProgram* program = selection->mProgram;
    return gabi::at<ShaderProgram>(gabi::ea(program));
}
/* 027CFFF0 */
void agl::DrawPairRendering_l::drawPair(DrawPairParameterRecord_l* record, u32 slot, u32 unused,
                                       DrawPairRenderContext_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    (void)unused;
    gabi::FrameLocal<DrawProgramSelection_l> selection(8);
    bool alternate = true;
    DrawParameterState_l* first = record->mFirst;
    bool firstAlternate = false;
    if (u16(first->mFlags) & 2) {
        u32 equal = first->mAlternateColor.equals(&sDrawPairEmptyColor);
        if (u8(equal ^ 1) != 0) {
            DrawResourceConfiguration_l* configuration = mConfiguration;
            if (!(u32(configuration->mFlags) & 0x10000))
                firstAlternate = true;
        }
    }
    if (!firstAlternate) {
        DrawParameterState_l* second = record->mSecond;
        bool secondAlternate = false;
        if (u16(second->mFlags) & 2) {
            u32 equal = second->mAlternateColor.equals(&sDrawPairEmptyColor);
            if (u8(equal ^ 1) != 0) {
                DrawResourceConfiguration_l* configuration = mConfiguration;
                if (!(u32(configuration->mFlags) & 0x10000))
                    secondAlternate = true;
            }
        }
        alternate = secondAlternate;
    }
    DrawResourceConfiguration_l* configuration = mConfiguration;
    prepareOpaquePairRender(configuration, context, u8(alternate), 0, 0);
    configuration = mConfiguration;
    first = record->mFirst;
    u32 flags = configuration->mFlags;
    u32 bindFourth = (flags >> 13) & 1;
    u32 option = (flags >> 20) & 1;
    u32 alternateOption = (flags >> 8) & 1;
    u32 firstMode = 0;
    if (u16(first->mFlags) & 2) {
        u32 equal = first->mAlternateColor.equals(&sDrawPairEmptyColor);
        if (u8(equal ^ 1) != 0) {
            configuration = mConfiguration;
            if (!(u32(configuration->mFlags) & 0x10000))
                firstMode = 1;
        }
    }
    DrawParameterState_l* second = record->mSecond;
    u32 secondMode = 0;
    if (u16(second->mFlags) & 2) {
        u32 equal = second->mAlternateColor.equals(&sDrawPairEmptyColor);
        if (u8(equal ^ 1) != 0) {
            configuration = mConfiguration;
            if (!(u32(configuration->mFlags) & 0x10000))
                secondMode = 1;
        }
    }
    u8 format = context->mFormat;
    selection->selectPair(format, alternateOption, bindFourth, option, firstMode, secondMode);
    ShaderProgram* program = selectedDrawPairProgram(selection);
    u8 programFlags = program->flags;
    u32 cachedMode = context->mShaderMode;
    if (programFlags & 2) {
        program->flags = programFlags & ~u8(2);
        initializeDrawPairProgram(program, 0);
    }
    u32 shaderMode = programSharedData(program)->mode;
    if (cachedMode != shaderMode)
        agl::setProgramShaderMode(shaderMode);
    u32 displaySize = programDisplayList(program)->mUsed;
    if (displaySize != 0) {
        void* display = programDisplayList(program)->mData;
        callDrawPairDisplayList(display, displaySize);
    } else {
        program->bindStagePrograms();
    }
    context->mShaderMode = shaderMode;
    configuration = mConfiguration;
    u32 formatIndex = configuration->mFormatIndex;
    ShapeLodIndexBuffers* buffers = sPrimitiveShapes->mLodShapes[1].mIndexBuffers;
    if (formatIndex < 3)
        buffers += formatIndex;
    mIndexBuffer = &buffers->mLevels[0];
    mInputs.bind();
    program = selectedDrawPairProgram(selection);
    u32 samplerCount = program->samplerCount;
    void* buffer = context->mFirstBuffer;
    ProgramResource16_l* samplers = program->samplers;
    if (samplerCount > 1)
        ++samplers;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    program = selectedDrawPairProgram(selection);
    buffer = context->mSecondBuffer;
    samplers = program->samplers;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    program = selectedDrawPairProgram(selection);
    samplerCount = program->samplerCount;
    buffer = context->mThirdBuffer;
    samplers = program->samplers;
    if (samplerCount > 2)
        samplers += 2;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    program = selectedDrawPairProgram(selection);
    UniformBuffer* uniformData = context->mUniformData;
    UniformLocations_l* locations = programUniformBlocks(program);
    u32 bytes = uniformData->stride;
    void* data = uniformData->cpuData;
    s32 vertex = locations->mVertexRegister;
    s32 pixel = locations->mPixelRegister;
    s32 geometry = locations->mGeometryRegister;
    if (vertex != -1 || pixel != -1 || geometry != -1) {
        vertex = locations->mVertexRegister;
        if (pixel != -1)
            uploadDrawPairUniformA(pixel, bytes, data);
        if (vertex != -1)
            uploadDrawPairUniformB(vertex, bytes, data);
        if (geometry != -1)
            uploadDrawPairUniformC(geometry, bytes, data);
        program = selectedDrawPairProgram(selection);
    }
    configuration = mConfiguration;
    if (u32(configuration->mFlags) & 0x2000) {
        samplerCount = program->samplerCount;
        buffer = context->mFourthBuffer;
        samplers = program->samplers;
        if (samplerCount > 5)
            samplers += 5;
        bindDrawPairBuffer(buffer, samplers, -1, 0);
        program = selectedDrawPairProgram(selection);
    }
    bindDrawPairCurveUniforms(program, record, 3, slot);
    IndexBufferLayout* indexBuffer = mIndexBuffer;
    u32 count = indexBuffer->mIndexCount;
    if (count != 0) {
        u32 indexType = indexBuffer->mFormat;
        void* indices = indexBuffer->mStorage;
        u32 primitive = indexBuffer->mPrimitiveMode;
        drawPairIndexed(primitive, count, indexType, indices, 0, 1);
    }
}
}
/* WWHD glue
data sDrawPairEmptyColor 104A01CC
ext 0274D4C0 bool sead::ColorChannels::equals(const sead::ColorChannels* other) const
ext 027B0B9C void prepareOpaquePairRender(agl::DrawResourceConfiguration_l* configuration, agl::DrawPairRenderContext_l* context, u32 alternate, u32 option, u32 extra)
ext 027BB9E0 void initializeDrawPairProgram(agl::ShaderProgram* program, u32 option)
ext 027BB7CC void agl::ShaderProgram::bindStagePrograms()
ext 027B9F68 void agl::setProgramShaderMode(u32 mode)
ext C00060E0 void callDrawPairDisplayList(void* data, u32 bytes)
ext 027BFE5C void agl::VertexInputSet::bind()
ext 027BE53C void bindDrawPairBuffer(void* buffer, agl::ProgramResource16_l* binding, s32 index, u32 option)
ext C0006900 void uploadDrawPairUniformA(s32 location, u32 bytes, const void* data)
ext C0006A38 void uploadDrawPairUniformB(s32 location, u32 bytes, const void* data)
ext C00068A8 void uploadDrawPairUniformC(s32 location, u32 bytes, const void* data)
ext 027D3C44 void bindDrawPairCurveUniforms(agl::ShaderProgram* program, const agl::DrawPairParameterRecord_l* record, u32 kind, u32 index)
ext C0006178 void drawPairIndexed(u32 primitive, u32 count, u32 indexType, void* indices, u32 baseVertex, u32 instances)
*/
