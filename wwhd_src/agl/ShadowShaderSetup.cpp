// Shadow shader selection and resource setup on the merged agl program types (AGL#12 fold pattern).
// SDK boundary declarations: public wut GX2 headers (types only; no code copied).
#include "agl/ShadowShaderSetup.h"
#include "sead/RenderState.h"
#include "agl/EffectMatrixSupport.h"
#include "bindings.h"
using namespace agl::shadow_shader;
using agl::TextureSampler;
using agl::ShaderProgram;
using agl::detail::ShaderHolder;
// The shader holder singleton (101F8D6C) and the primitive-shape singletons (101F8DDC / 101F8DE4); their one
// definitions are in agl/TimingStatics.cpp and agl/PrimitiveShapesStatics.cpp, which this TU does not link:
// borrowed per-TU bindings, never initialized here.
using ShaderHolderPointer = gptr<ShaderHolder>;
using ShapeInputsPointer = gptr<agl::PrimitiveShapeInputs>;
using ShapesPointer = gptr<agl::PrimitiveShapes>;
static ShaderHolderPointer sShadowShaderHolder;
static ShapeInputsPointer sShadowShapeInputs;
static ShapesPointer sShadowShapes;
void updateShadowVariation(ShaderProgram*, u32);
void callShadowDisplayList(const void*, u32);
void bindShadowSampler(TextureSampler*, const UniformLocations_l*, u32, u32);
void uploadShadowPixelBlock(u32, u32, const void*);
void uploadShadowVertexBlock(u32, u32, const void*);
void uploadShadowGeometryBlock(u32, u32, const void*);
void submitShadowDraw(u32, u32, u32, const void*, u32, u32);
static ShaderProgram* selectShadowVariation(ShaderProgram*, u32);
static void activateShadowVariation(ShaderProgram*);
static void uploadShadowBlocks(const UniformLocations_l*, u32, const void*);
static const UniformLocations_l* uniformBlockLocations(const ShaderProgram* program) {
    return reinterpret_cast<const UniformLocations_l*>(program->uniformBlocks.get());
}
static const UniformLocations_l* samplerLocations(const ShaderProgram* program) {
    return reinterpret_cast<const UniformLocations_l*>(program->samplers.get());
}
static void drawShadowGeometry();
struct SignedDimensionScratch { be<u32> mHigh, mLow; };
WWHD_SIZE(SignedDimensionScratch,8);


/* 027904D0 */
u32 ShaderSetup::primary(shadow_prepass::ShadowPrePass* shadow, shadow_prepass::FrameResources* resources,
                           u32 previousMode, TextureSampler* depthSampler, TextureSampler* normalSampler, TextureSampler* decalSampler,
                           u32 sampleCount, TextureSampler* extraSampler, u8 useFirstSample, s32 width, s32 height) {
    gabi::AutoFrame frame{0x027904D0u};
    ShaderHolder* holder = sShadowShaderHolder;
    ShaderProgram* program = nullptr;
    if (u32(holder->mPrograms.mCount) > ShaderHolder::cShader_shadow_pre_pass)
        program = holder->programSlots()[ShaderHolder::cShader_shadow_pre_pass];
    gabi::FrameLocal<sead::CompactRenderState> state(0x18);
    gabi::FrameLocal<SignedDimensionScratch> conversion(0x10);
    gabi::FrameLocal<sead::RenderVector2> inverseSize(8);
    sead::CompactRenderState::create(state.get());
    u8 direct = shadow->mParameters.mSsaoDirectDraw.mValue;
    state->mDepthTest = 0;
    state->mBlendEnable = 0;
    state->mDepthWrite = 0;
    state->mWriteRed = direct == 0 ? 1 : 0;
    state->mWriteAlpha = 1;
    state->mWriteGreen = 1;
    state->mWriteBlue = 1;
    state->apply();
    conversion->mHigh = 0x43300000u;
    conversion->mLow = static_cast<u32>(width) ^ 0x80000000u;
    conversion->mLow = static_cast<u32>(height) ^ 0x80000000u;
    f32 heightFloat = static_cast<f32>(height);
    f32 widthFloat = static_cast<f32>(width);
    inverseSize->y = gabi::fdivs_fpr(1.0, heightFloat);
    inverseSize->x = gabi::fdivs_fpr(1.0, widthFloat);
    agl::UniformBuffer* uniforms = &resources->mBuffers[0];
    uniforms->writeWords(uniforms->cpuData, 14, reinterpret_cast<const be<u32>*>(inverseSize.get()), 0, 1);
    uniforms->flushNoSync(uniforms->cpuData, 0);
    u32 sequence = shadow->mFrameSequence;
    if (sequence >= 4) sequence = 3;
    u32 token = shadow->mFrameToken;
    if (token != 0) sampleCount = token;
    s32 sample = static_cast<s32>(sampleCount - 1u);
    u8 normal = shadow->mParameters.mGenerateNormals.mValue;
    if (sample >= 2) sample = 2;
    else if ((static_cast<u32>(sample) | useFirstSample) == 0) sample = 3;
    u8 depth = shadow->mParameters.mStaticDepth.mValue;
    u8 fade = shadow->mParameters.mFarFade.mValue;
    u32 resolution = shadow->mParameters.mResolution.mValue;
    u8 combine = shadow->mParameters.mCombineSsao.mValue;
    direct = shadow->mParameters.mSsaoDirectDraw.mValue;
    u32 combined = combine != 0 ? 1u : 0u;
    if (direct != 0) combined = 0;
    u32 choice = agl::effect_matrix::packShaderChoice(sequence, static_cast<u32>(sample),
        normal != 0 ? 1 : 0, depth != 0 ? 1 : 0, fade != 0 ? 1 : 0,
        resolution != 0 ? 1 : 0, combined);
    ShaderProgram* variation = selectShadowVariation(program, choice);
    activateShadowVariation(variation);
    u32 shaderMode = programSharedData(variation)->mode;
    if (previousMode != shaderMode) agl::setProgramShaderMode(shaderMode);
    if (programDisplayList(variation)->mUsed != 0)
        callShadowDisplayList(programDisplayList(variation)->mData, programDisplayList(variation)->mUsed);
    else variation->bindStagePrograms();
    if (combined != 0) {
        const UniformLocations_l* locations = samplerLocations(variation);
        if (variation->samplerCount > 1) locations = &locations[1];
        TextureSampler* sampler = resources->mCombinedSampler.get();
        bindShadowSampler(sampler, locations, 0xFFFFFFFFu, 0);
    }
    bindShadowSampler(depthSampler, samplerLocations(variation), 0xFFFFFFFFu, 0);
    const UniformLocations_l* locations = samplerLocations(variation);
    if (variation->samplerCount > 3) locations = &locations[3];
    bindShadowSampler(normalSampler, locations, 0xFFFFFFFFu, 0);
    locations = samplerLocations(variation);
    if (variation->samplerCount > 2) locations = &locations[2];
    bindShadowSampler(decalSampler, locations, 0xFFFFFFFFu, 0);
    locations = samplerLocations(variation);
    if (variation->samplerCount > 4) locations = &locations[4];
    bindShadowSampler(extraSampler, locations, 0xFFFFFFFFu, 0);
    const UniformLocations_l* slots = uniformBlockLocations(variation);
    const void* data = resources->mBuffers[0].cpuData;
    u32 bytes = resources->mBuffers[0].stride;
    uploadShadowBlocks(slots, bytes, data);
    drawShadowGeometry();
    return shaderMode;
}

/* 027908F4 */
u32 ShaderSetup::paired(void* unused, const agl::shadow_prepass::FrameResources* resources, u32 pair, u32 member,
                          TextureSampler* firstSampler, TextureSampler* secondSampler, u32 previousMode) {
    gabi::AutoFrame frame{0x027908F4u};
    ShaderHolder* holder = sShadowShaderHolder;
    ShaderProgram* program = nullptr;
    if (u32(holder->mPrograms.mCount) > ShaderHolder::cShader_shadow_pre_pass_blur)
        program = holder->programSlots()[ShaderHolder::cShader_shadow_pre_pass_blur];
    gabi::FrameLocal<sead::CompactRenderState> state(8);
    sead::CompactRenderState::create(state.get());
    state->mDepthTest = 0;
    state->mDepthWrite = 0;
    state->mWriteBlue = 1;
    state->mBlendEnable = 0;
    state->mWriteGreen = 1;
    state->mWriteAlpha = 1;
    state->mWriteRed = 1;
    state->apply();
    ShaderProgram* variation = selectShadowVariation(program, pair * 2u + member);
    activateShadowVariation(variation);
    u32 shaderMode = programSharedData(variation)->mode;
    if (previousMode != shaderMode) agl::setProgramShaderMode(shaderMode);
    if (programDisplayList(variation)->mUsed != 0)
        callShadowDisplayList(programDisplayList(variation)->mData, programDisplayList(variation)->mUsed);
    else variation->bindStagePrograms();
    const UniformLocations_l* locations = samplerLocations(variation);
    if (variation->samplerCount > 1)
        locations = &locations[1];
    bindShadowSampler(firstSampler, locations, 0xFFFFFFFFu, 0);
    bindShadowSampler(secondSampler, samplerLocations(variation), 0xFFFFFFFFu, 0);
    const UniformLocations_l* slots = uniformBlockLocations(variation);
    const void* data = resources->mBuffers[1].cpuData;
    u32 bytes = resources->mBuffers[1].stride;
    uploadShadowBlocks(slots, bytes, data);
    drawShadowGeometry();
    return shaderMode;
}

/* 02790C28 */
u32 ShaderSetup::combined(void* unused, const agl::shadow_prepass::FrameResources* resources, u32 previousMode,
                            TextureSampler* firstSampler, TextureSampler* secondSampler, u32 firstEnabled, u32 secondEnabled) {
    gabi::AutoFrame frame{0x02790C28u};
    ShaderHolder* holder = sShadowShaderHolder;
    ShaderProgram* program = nullptr;
    if (u32(holder->mPrograms.mCount) > ShaderHolder::cShader_shadow_pre_pass_post)
        program = holder->programSlots()[ShaderHolder::cShader_shadow_pre_pass_post];
    gabi::FrameLocal<sead::CompactRenderState> state(8);
    sead::CompactRenderState::create(state.get());
    state->mDepthTest = 0;
    state->mDepthWrite = 0;
    state->mWriteBlue = 1;
    state->mBlendEnable = 0;
    state->mWriteGreen = 1;
    state->mWriteAlpha = 1;
    state->mWriteRed = 1;
    state->apply();
    u32 selector;
    if (secondEnabled != 0) selector = firstEnabled != 0 ? 2u : 1u;
    else selector = firstEnabled != 0 ? 0u : 3u;
    ShaderProgram* variation = selectShadowVariation(program, selector);
    activateShadowVariation(variation);
    u32 shaderMode = programSharedData(variation)->mode;
    if (previousMode != shaderMode) agl::setProgramShaderMode(shaderMode);
    if (programDisplayList(variation)->mUsed != 0)
        callShadowDisplayList(programDisplayList(variation)->mData, programDisplayList(variation)->mUsed);
    else variation->bindStagePrograms();
    bindShadowSampler(firstSampler, samplerLocations(variation), 0xFFFFFFFFu, 0);
    const UniformLocations_l* locations = samplerLocations(variation);
    if (variation->samplerCount > 1)
        locations = &locations[1];
    bindShadowSampler(secondSampler, locations, 0xFFFFFFFFu, 0);
    const UniformLocations_l* slots = uniformBlockLocations(variation);
    const void* data = resources->mBuffers[2].cpuData;
    u32 bytes = resources->mBuffers[2].stride;
    uploadShadowBlocks(slots, bytes, data);
    drawShadowGeometry();
    return shaderMode;
}

static ShaderProgram* selectShadowVariation(ShaderProgram* program, u32 selector) {
    agl::ShaderProgramSharedData* data = programSharedData(program);
    if (static_cast<s32>(selector) <= 0) return data->variation.root;
    u32 count = data->variation.programCount;
    ShaderProgram* variations = data->variation.programs;
    if (selector - 1u < count)
        variations = agl::shadow_prepass::guestElement(variations, selector - 1u);
    return variations;
}
static void activateShadowVariation(ShaderProgram* variation) {
    if ((variation->flags & ShaderProgramFlag_UpdatePending) != 0) {
        variation->flags = static_cast<u8>(variation->flags & ~u32(ShaderProgramFlag_UpdatePending));
        updateShadowVariation(variation, 0);
    }
}
static void uploadShadowBlocks(const UniformLocations_l* slots, u32 bytes, const void* data) {
    s32 vertex = slots->mVertexRegister;
    s32 pixel = slots->mPixelRegister;
    if (vertex == -1 && pixel == -1 && static_cast<s16>(slots->mGeometryRegister) == -1) return;
    s32 geometry = slots->mGeometryRegister;
    vertex = slots->mVertexRegister;
    if (pixel != -1) uploadShadowPixelBlock(pixel, bytes, data);
    if (vertex != -1) uploadShadowVertexBlock(vertex, bytes, data);
    if (geometry != -1) uploadShadowGeometryBlock(geometry, bytes, data);
}
static void drawShadowGeometry() {
    sShadowShapeInputs->mInputs[agl::DrawQuadInput].bind();
    const agl::IndexBufferLayout* packet = &sShadowShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer;
    u32 count = packet->mIndexCount;
    if (count != 0)
        submitShadowDraw(packet->mPrimitiveMode, count, packet->mFormat, packet->mStorage, 0, 1);
}
/* WWHD glue
data sShadowShaderHolder 101F8D6C
data sShadowShapeInputs 101F8DDC
data sShadowShapes 101F8DE4
ext 027BB9E0 void updateShadowVariation(agl::ShaderProgram*,u32)
ext C00060E0 void callShadowDisplayList(const void*,u32)
ext 027BE53C void bindShadowSampler(agl::TextureSampler*,const UniformLocations_l*,u32,u32)
ext C0006900 void uploadShadowPixelBlock(u32,u32,const void*)
ext C0006A38 void uploadShadowVertexBlock(u32,u32,const void*)
ext C00068A8 void uploadShadowGeometryBlock(u32,u32,const void*)
ext C0006178 void submitShadowDraw(u32,u32,u32,const void*,u32,u32)
*/
