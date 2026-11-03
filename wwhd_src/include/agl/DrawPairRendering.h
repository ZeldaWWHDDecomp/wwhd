// Own draw caller prefixes, not complete shader or context owners.
#pragma once
#include "agl/DrawParameterPreparation.h"
#include "agl/DrawFormatPreparation.h"
#include "agl/IndexBufferLayout.h"
#include "agl/UniformBuffer.h"
namespace agl {
struct DrawPairRenderContext_l {
    u8 mPrefix[0xC];
    gptr<UniformBuffer> mUniformData;
    u8 mReserved[4];
    gptr<void> mFirstBuffer, mSecondBuffer, mThirdBuffer, mFourthBuffer;
    // The last shader mode set (ShaderProgramSharedData::mode), compared before setProgramShaderMode.
    be<u32> mShaderMode;
    be<u8> mFormat;
};
// The draw callers' index buffer is one of agl::PrimitiveShapes' (sPrimitiveShapes) built-in shape buffers.
struct DrawPairRendering_l {
    u8 mPrefix[4];
    gptr<DrawResourceConfiguration_l> mConfiguration;
    u8 mBeforeBinding[0x14];
    VertexInputSet mInputs;
    gptr<IndexBufferLayout> mIndexBuffer;
    void drawParameters(DrawParameterRecord_l* record, u32 slot, const DrawCullVolume_l* volume, DrawPairRenderContext_l* context);
    void drawPair(DrawPairParameterRecord_l* record, u32 slot, u32 unused,
                  DrawPairRenderContext_l* context);
};
}
WWHD_OFFSET(agl::DrawPairRenderContext_l, mUniformData, 0xC);
WWHD_OFFSET(agl::DrawPairRenderContext_l, mFirstBuffer, 0x14);
WWHD_OFFSET(agl::DrawPairRenderContext_l, mShaderMode, 0x24);
WWHD_OFFSET(agl::DrawPairRenderContext_l, mFormat, 0x28);
WWHD_OFFSET(agl::DrawPairRendering_l, mInputs, 0x1C);
WWHD_OFFSET(agl::DrawPairRendering_l, mIndexBuffer, 0x110);
