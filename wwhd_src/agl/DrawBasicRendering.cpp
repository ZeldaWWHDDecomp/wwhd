// Own basic draw caller, including its inline sphere test and original parameter-stack ABI.
#include "agl/DrawRenderingCalls.h"
void bindDrawParameterCurveUniforms(agl::ShaderProgram* program, const agl::DrawParameterRecord_l* record, u32 kind, u32 index);
namespace agl {
// Extracted inline body; the caller supplies its original sp+0x20 scratch slot.
static u32 isBasicDrawVisible(const DrawParameterState_l* value, const DrawCullVolume_l* volume,
    u16 flags, sead::RenderVector2* transformed) {
    if (flags & 0x20)
        return 0;
    f32 positionY = value->mPosition.y;
    f32 depthY = volume->mTransform.mRows[2][1];
    f32 positionX = value->mPosition.x;
    f32 depthProduct = gabi::fmuls_ppc(depthY, positionY);
    f32 depthX = volume->mTransform.mRows[2][0];
    f32 positionZ = value->mPosition.z;
    f32 depthPartial = gabi::fmadds(depthX, positionX, depthProduct);
    f32 depthZ = volume->mTransform.mRows[2][2];
    f32 nearPlane = volume->mNear;
    f32 depthTranslation = volume->mTransform.mRows[2][3];
    f32 depthSum = gabi::fmadds(depthZ, positionZ, depthPartial);
    f32 radius = value->mScale;
    f32 depth = gabi::fadds_ppc(depthSum, depthTranslation);
    f32 nearLimit = gabi::fsubs_ppc(nearPlane, radius);
    f32 negativeDepth = -depth;
    if (negativeDepth < nearLimit)
        return 0;
    f32 farLimit = gabi::fsubs_ppc(negativeDepth, radius);
    f32 farPlane = volume->mFar;
    if (farPlane < farLimit)
        return 0;
    f32 xY = volume->mTransform.mRows[0][1];
    f32 yY = volume->mTransform.mRows[1][1];
    f32 xProduct = gabi::fmuls_ppc(xY, positionY);
    f32 xX = volume->mTransform.mRows[0][0];
    f32 yProduct = gabi::fmuls_ppc(yY, positionY);
    f32 yX = volume->mTransform.mRows[1][0];
    f32 xPartial = gabi::fmadds(xX, positionX, xProduct);
    f32 xZ = volume->mTransform.mRows[0][2];
    f32 yPartial = gabi::fmadds(yX, positionX, yProduct);
    f32 yZ = volume->mTransform.mRows[1][2];
    f32 xSum = gabi::fmadds(xZ, positionZ, xPartial);
    f32 xTranslation = volume->mTransform.mRows[0][3];
    f32 ySum = gabi::fmadds(yZ, positionZ, yPartial);
    f32 yTranslation = volume->mTransform.mRows[1][3];
    f32 x = gabi::fadds_ppc(xSum, xTranslation);
    f32 y = gabi::fadds_ppc(ySum, yTranslation);
    transformed->x = x;
    transformed->y = y;
    for (u32 index = 0; index < 4; ++index) {
        const sead::RenderVector3* plane = &volume->mSidePlanes[index];
        f32 xProduct = gabi::fmuls_ppc(x, plane->x);
        f32 yProduct = gabi::fmuls_ppc(y, plane->y);
        f32 sum = gabi::fadds_ppc(xProduct, yProduct);
        f32 distance = gabi::fmadds(depth, plane->z, sum);
        if (!(distance < radius))
            return 0;
    }
    return 1;
}
/* 027CC78C */
void agl::DrawPairRendering_l::drawParameters(DrawParameterRecord_l* record, u32 slot,
    const DrawCullVolume_l* volume, DrawPairRenderContext_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector2> transformed(0x20);
    u16 flags = record->mFlags;
    if (!isBasicDrawVisible(record, volume, flags, transformed.get()))
        return;
    bool alternate = false;
    if (flags & 2) {
        const sead::RenderVector3* color = gabi::at<sead::RenderVector3>(gabi::ea(&record->mAlternateColor));
        if (isZeroDrawVector(color) == 0) {
            DrawResourceConfiguration_l* configuration = mConfiguration;
            if (!(u32(configuration->mFlags) & 0x10000))
                alternate = true;
        }
    }
    if (!alternate) {
        const sead::RenderVector3* color = gabi::at<sead::RenderVector3>(gabi::ea(&record->mColor));
        if (isZeroDrawVector(color) != 0)
            return;
    }
    DrawResourceConfiguration_l* configuration = mConfiguration;
    prepareOpaquePairRender(configuration, context, u32(alternate), 0, 0);
    f32 threshold = record->mThreshold;
    configuration = mConfiguration;
    flags = record->mFlags;
    bool nonzeroThreshold = threshold != 0.0f;
    u32 recordMode = (flags >> 8) & 1;
    u32 enabledMode = (flags >> 7) & 1;
    u32 recordOption = (flags >> 2) & 1;
    u32 configurationFlags = configuration->mFlags;
    bool modeEnabled = (configurationFlags & 0x200) != 0;
    u32 alternateMode = alternate && modeEnabled ? 1u : 0u;
    u32 thresholdMode = nonzeroThreshold && modeEnabled ? 1u : 0u;
    u32 enabled = enabledMode && modeEnabled ? 1u : 0u;
    u8 format = context->mFormat;
    ShaderProgram* program = selectOpaqueResourceProgram(0, format, u8(alternateMode), u8(thresholdMode),
        u8(enabled), 0, 0, 0, 0, (configurationFlags >> 8) & 1, recordOption,
        (configurationFlags >> 13) & 1, (configurationFlags >> 20) & 1, recordMode);
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
    if (displaySize != 0)
        callDrawPairDisplayList(programDisplayList(program)->mData, displaySize);
    else
        program->bindStagePrograms();
    context->mShaderMode = shaderMode;
    configuration = mConfiguration;
    u32 formatIndex = configuration->mFormatIndex;
    ShapeLodIndexBuffers* buffers = sPrimitiveShapes->mLodShapes[1].mIndexBuffers;
    if (formatIndex < 3)
        buffers += formatIndex;
    mIndexBuffer = &buffers->mLevels[0];
    mInputs.bind();
    u32 samplerCount = program->samplerCount;
    void* buffer = context->mFirstBuffer;
    ProgramResource16_l* samplers = program->samplers;
    if (samplerCount > 1)
        ++samplers;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    buffer = context->mSecondBuffer;
    samplers = program->samplers;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    samplerCount = program->samplerCount;
    buffer = context->mThirdBuffer;
    samplers = program->samplers;
    if (samplerCount > 2)
        samplers += 2;
    bindDrawPairBuffer(buffer, samplers, -1, 0);
    UniformBuffer* uniformData = context->mUniformData;
    UniformLocations_l* locations = programUniformBlocks(program);
    u32 bytes = uniformData->stride;
    s32 vertex = locations->mVertexRegister;
    void* data = uniformData->cpuData;
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
    }
    configuration = mConfiguration;
    if (u32(configuration->mFlags) & 0x2000) {
        samplerCount = program->samplerCount;
        buffer = context->mFourthBuffer;
        samplers = program->samplers;
        if (samplerCount > 5)
            samplers += 5;
        bindDrawPairBuffer(buffer, samplers, -1, 0);
    }
    bindDrawParameterCurveUniforms(program, record, 0, slot);
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
ext 027D395C void bindDrawParameterCurveUniforms(agl::ShaderProgram* program, const agl::DrawParameterRecord_l* record, u32 kind, u32 index)
*/
