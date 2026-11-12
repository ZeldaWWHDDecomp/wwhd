// Own resource rendering caller; protected shader selection/state remain opaque.
#include "agl/DrawResourceRendering.h"
#include "agl/DrawRenderingCalls.h"
#include "port_native_draw.h"
agl::ShaderProgram* selectOpaqueResourceProgram(u32 kind, u32 format, u32 alternate, u32 zeroA,
    u32 zeroB, u32 zeroC, u32 zeroD, u32 selectorDigit, u32 zeroE, u32 configurationA,
    u32 zeroF, u32 configurationB, u32 configurationC, u32 zeroG);
void updateDrawResourceSurface(agl::DrawReleaseResource_l* target, const agl::TextureData* source);
void bindDrawResourceCurveUniforms(agl::ShaderProgram* program, const agl::DrawResourceRecord_l* record, u32 kind, u32 index);
namespace agl {
/* 027CD8EC */
void agl::DrawResourceRendering_l::drawResource(DrawResourceRecord_l* record, u32 slot, u32 unused,
                                                DrawPairRenderContext_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    (void)unused;
    bool alternate = false;
    if (u8(record->mFlags) & 2) {
        const sead::RenderVector3* color = gabi::at<sead::RenderVector3>(gabi::ea(&record->mAlternateColor));
        if (isZeroDrawVector(color) == 0) {
            DrawResourceConfiguration_l* configuration = mConfiguration;
            if (!(u32(configuration->mFlags) & 0x10000))
                alternate = true;
        }
    }
    if (!alternate) {
        const sead::RenderVector3* color = gabi::at<sead::RenderVector3>(gabi::ea(&record->mPrimaryColor));
        if (isZeroDrawVector(color) != 0)
            return;
    }
    DrawResourceConfiguration_l* configuration = mConfiguration;
    prepareOpaquePairRender(configuration, context, u32(alternate), 0, 0);
    u32 valueCount = record->mValueCount;
    configuration = mConfiguration;
    DrawResourceValue_l* value = record->mValues;
    if (slot < valueCount)
        value += slot;
    TextureData* source = value->mSource;
    u32 selectorDigit = 0;
    if (gabi::ea(source) != 0)
        selectorDigit = record->mSelectorDigit;
    u32 flags = configuration->mFlags;
    u8 format = context->mFormat;
    u32 alternateMode = alternate && (flags & 0x200) ? 1u : 0u;
    ShaderProgram* program = selectOpaqueResourceProgram(1, format, u8(alternateMode), 0, 0, 0, 0,
        selectorDigit, 0, (flags >> 8) & 1, 0, (flags >> 13) & 1, (flags >> 20) & 1, 0);
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
    IndexBufferLayout* indexBuffer = sPrimitiveShapes->mDualLodShapes[1].mIndexBuffers;
    if (formatIndex < 3)
        indexBuffer += formatIndex;
    mIndexBuffer = indexBuffer;
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
    valueCount = record->mValueCount;
    value = record->mValues;
    if (slot < valueCount)
        value += slot;
    source = value->mSource;
    if (gabi::ea(source) != 0) {
        DrawReleaseResource_l* target = value->mResource;
        const sead::GraphicsSurface* a = &target->mTexture.surface;
        const sead::GraphicsSurface* b = &source->surface;
        bool same = a->mDimension == b->mDimension && a->mWidth == b->mWidth &&
            a->mHeight == b->mHeight && a->mDepth == b->mDepth &&
            a->mMipCount == b->mMipCount && a->mFormat == b->mFormat &&
            a->mSwizzle == b->mSwizzle && a->mTileMode == b->mTileMode &&
            a->mSamples == b->mSamples;
        if (!same) {
            updateDrawResourceSurface(target, source);
        } else {
            void* image = source->surface.mImage;
            void* mipImage = source->surface.mMipImage;
            target->mTexture.surface.mImage = image;
            target->mCachedMipImage = mipImage;
            target->mCachedImage = image;
            target->mTexture.surface.mMipImage = mipImage;
        }
        valueCount = record->mValueCount;
        value = record->mValues;
        samplerCount = program->samplerCount;
        if (slot < valueCount)
            value += slot;
        target = value->mResource;
        samplers = program->samplers;
        if (samplerCount > 4)
            samplers += 4;
        bindDrawPairBuffer(target, samplers, -1, 0);
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
    bindDrawResourceCurveUniforms(program, record, 1, slot);
    indexBuffer = mIndexBuffer;
    u32 count = indexBuffer->mIndexCount;
    if (count != 0) {
        u32 indexType = indexBuffer->mFormat;
        u32 primitive = indexBuffer->mPrimitiveMode;
        void* indices = indexBuffer->mStorage;
#if PORT_NATIVE_DRAW
        // the draw pair's pass as a packet of its own (port_native_draw.h)
        if (port_native_draw_single(PORT_NATIVE_AGL_PAIR, 0, gabi::ea(this), gabi::ea(program), 0, primitive, count,
                                    indexType, gabi::ea(indices), 0, 1))
            return;
#endif
        drawPairIndexed(primitive, count, indexType, indices, 0, 1);
    }
}
}
/* WWHD glue
ext 027B1938 agl::ShaderProgram* selectOpaqueResourceProgram(u32 kind, u32 format, u32 alternate, u32 zeroA, u32 zeroB, u32 zeroC, u32 zeroD, u32 selectorDigit, u32 zeroE, u32 configurationA, u32 zeroF, u32 configurationB, u32 configurationC, u32 zeroG)
ext 027BDEB4 void updateDrawResourceSurface(agl::DrawReleaseResource_l* target, const agl::TextureData* source)
ext 027D3A54 void bindDrawResourceCurveUniforms(agl::ShaderProgram* program, const agl::DrawResourceRecord_l* record, u32 kind, u32 index)
*/
