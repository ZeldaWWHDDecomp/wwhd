// Own spatial resource draw caller; borrowed shader/provider state stays qualified.
#include "agl/DrawResourceRendering.h"
#include "agl/DrawRenderingCalls.h"
#include "port_native_draw.h"
void updateDrawResourceSurface(agl::DrawReleaseResource_l* target, const agl::TextureData* source);
void bindDrawLargeCurveUniforms(agl::ShaderProgram* program, const agl::LargeDrawResourceRecord_l* record, u32 kind, u32 index);
namespace agl {
/* 027CF2B0 */
void agl::DrawLargeRendering_l::drawLargeResource(LargeDrawResourceRecord_l* record, u32 slot,
    u32 unused, DrawPairRenderContext_l* context) {
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
        const sead::RenderVector3* color = gabi::at<sead::RenderVector3>(gabi::ea(&record->mColor));
        if (isZeroDrawVector(color) != 0)
            return;
    }
    DrawResourceConfiguration_l* configuration = mConfiguration;
    prepareOpaquePairRender(configuration, context, u32(alternate), 0, 0);
    u8 primaryEnabled = record->mEnabled;
    configuration = mConfiguration;
    u32 valueCount = record->mValueCount;
    u8 recordFlags = record->mFlags;
    u32 resourceMode = 1;
    if (primaryEnabled == 0 && u8(record->mBoundsEnabled) == 0)
        resourceMode = 0;
    u32 recordMode = (recordFlags >> 4) & 1;
    DrawResourceValue_l* value = record->mValues;
    if (slot < valueCount)
        value += slot;
    TextureData* source = value->mSource;
    u32 selectorDigit = 0;
    if (gabi::ea(source) != 0)
        selectorDigit = record->mSelectorDigit;
    u32 recordOption = (recordFlags >> 2) & 1;
    u32 flags = configuration->mFlags;
    u8 format = context->mFormat;
    u32 alternateMode = alternate && (flags & 0x200) ? 1u : 0u;
    ShaderProgram* program = selectOpaqueResourceProgram(2, format, u8(alternateMode), 0, 0,
        u8(resourceMode), recordMode, selectorDigit, recordOption, (flags >> 8) & 1, 0,
        (flags >> 13) & 1, (flags >> 20) & 1, 0);
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
    mIndexBuffer = &sPrimitiveShapes->mIndexedShapes[2].mIndexBuffer;
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
    if (u8(record->mBoundsEnabled) != 0 && (u8(record->mFlags) & 8)) {
        samplerCount = program->samplerCount;
        samplers = program->samplers;
        if (samplerCount > 3)
            samplers += 3;
        bindDrawPairBuffer(&record->mSecondaryResource, samplers, -1, 0);
    } else if (u8(record->mEnabled) != 0) {
        samplerCount = program->samplerCount;
        samplers = program->samplers;
        if (samplerCount > 3)
            samplers += 3;
        bindDrawPairBuffer(&record->mPrimaryResource, samplers, -1, 0);
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
    bindDrawLargeCurveUniforms(program, record, 2, slot);
    IndexBufferLayout* indexBuffer = mIndexBuffer;
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
ext 027D3B4C void bindDrawLargeCurveUniforms(agl::ShaderProgram* program, const agl::LargeDrawResourceRecord_l* record, u32 kind, u32 index)
*/
