// HD: Cafe-only state setup for the extra child and parent passes.
#include "JSystem/JParticle/JPADraw.h"
#include "JSystem/JParticle/JPADraw_gpu_state.h"
#include "JSystem/JParticle/JPAEmitter.h"
using namespace JPADrawExtension;
void JPADrawStateConstruct(JPADrawRenderState_l*);
void JPADrawStateApply(JPADrawRenderState_l*);
void JPADrawHDChild(JPADraw_l*);
void JPADrawHDParticle(JPADraw_l*);
using JPADrawManagerPointer = gptr<void>;
static JPADrawManagerPointer sFrameTextureManager;
JPAMathDraw::JPATextureDescriptor* JPADrawGetFrameTexture(void*,u32);
JPAMathDraw::JPATextureState* JPADrawGetParticleTextureState(void*,u32);
void JPADrawCopyTextureState(JPAMathDraw::JPATextureState*,JPAMathDraw::JPATextureDescriptor*);
// GX-to-GX2 mappings are retained as data tables from the HD image.
static u32 sBlendSourceFactors[10] = {0x0,0x1,0x8,0x9,0x4,0x5,0x6,0x7,0x8,0x9};
static u32 sBlendDestinationFactors[10] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9};
static u32 sDepthCompareFunctions[8] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};
static u32 sAlphaCompareFunctions[8] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};
/* 0282D680 */
void JPADrawHDChildWithState(JPADraw_l* draw) {
    JPADrawRenderState_l state;
    JPADrawStateConstruct(&state);
    state.mCullMode = 2;
    state.mDepthEnabled = 1;
    state.mDepthFunction = 3;
    state.mDepthWriteEnabled = 1;
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(draw->dc.pbe.get());
    f32 alpha = f32(u8(reinterpret_cast<be<u8>*>(&emitter->scaleAndColors().mGlobalPrmColor)[3]));
    state.mBlendMask = 0;
    state.mChannelEnabled[2] = 0;
    state.mLogicMask = 0;
    f32 reference = gabi::fsubs_ppc(alpha / 255.0f,0.001f);
    state.mChannelEnabled[0] = 1;
    state.mAlphaFunction = 6;
    state.mAlphaEnabled = 1;
    state.mChannelEnabled[1] = 1;
    state.mAlphaReference = reference >= 0.0f ? reference : 0.0f;
    JPADrawStateApply(&state);
    JPADrawHDChild(draw);
}
/* 0282D8EC */
void JPADrawHDParticleWithState(JPADraw_l* draw) {
    JPADrawRenderState_l state;
    JPADrawStateConstruct(&state);
    state.mCullMode = 2;
    state.mDepthEnabled = 1;
    state.mDepthFunction = 3;
    state.mDepthWriteEnabled = 1;
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(draw->dc.pbe.get());
    f32 alpha = f32(u8(reinterpret_cast<be<u8>*>(&emitter->scaleAndColors().mGlobalPrmColor)[3]));
    state.mBlendMask = 0;
    state.mChannelEnabled[2] = 0;
    state.mLogicMask = 0;
    f32 reference = gabi::fsubs_ppc(alpha / 255.0f,0.001f);
    state.mChannelEnabled[0] = 1;
    state.mAlphaFunction = 6;
    state.mAlphaEnabled = 1;
    state.mChannelEnabled[1] = 1;
    state.mAlphaReference = reference >= 0.0f ? reference : 0.0f;
    JPADrawStateApply(&state);
    JPADrawHDParticle(draw);
}
/* 0282D3A0 */
void JPADraw_l::updateFrameTexture(JPAMathDraw::JPAParticleHD* particle) {
    if (dc.pbsp->textureIsEmpty()) return;
    if (!u8(mTextureCopyEnabled)) return;
    JPAMathDraw::JPATextureDescriptor* source = JPADrawGetFrameTexture(sFrameTextureManager,6);
    if (!source) return;
    JPAMathDraw::JPATextureState* state = JPADrawGetParticleTextureState(particle->mpTextureBindings,0);
    bool same = true;
    for (u32 i = 0; i < 6; ++i) {
        if (u32(state->mDescriptor.mLayout[i]) != u32(source->mLayout[i])) { same = false; break; }
    }
    if (same) same = u32(state->mDescriptor.mSwizzle) == u32(source->mSwizzle);
    if (same) same = u32(state->mDescriptor.mTileMode) == u32(source->mTileMode);
    if (same) same = u32(state->mDescriptor.mAA) == u32(source->mAA);
    if (!same) { JPADrawCopyTextureState(state,source); return; }
    u32 address = source->mImageAddress;
    u32 mipAddress = source->mMipAddress;
    state->mDescriptor.mImageAddress = address;
    state->mCachedMipAddress = mipAddress;
    state->mCachedImageAddress = address;
    state->mDescriptor.mMipAddress = mipAddress;
}
/* 0282CDBC */
void JPADraw_l::setupRenderState() {
    JPADrawRenderState_l state;
    JPADrawStateConstruct(&state);
    state.mBlendMask = 0;
    if (baseShape()->getBlendMode1() == 1) state.mBlendMask = u32(state.mBlendMask) | 1;
    else state.mBlendMask = u32(state.mBlendMask) & ~1;
    if (baseShape()->getBlendMode1() == 1) state.mBlendMask = u32(state.mBlendMask) | 2;
    else state.mBlendMask = u32(state.mBlendMask) & ~2;
    u32 source = sBlendSourceFactors[baseShape()->getSrcBlendFactor1()];
    u32 destination = sBlendDestinationFactors[baseShape()->getDstBlendFactor1()];
    state.mColorDestination[0] = destination;
    state.mColorDestination[1] = destination;
    state.mColorSource[1] = source;
    state.mColorSource[0] = source;
    source = sBlendSourceFactors[baseShape()->getSrcBlendFactor1()];
    destination = sBlendDestinationFactors[baseShape()->getDstBlendFactor1()];
    state.mAlphaSource[1] = source;
    state.mAlphaDestination[1] = destination;
    state.mCullMode = 2;
    state.mAlphaSource[0] = source;
    state.mAlphaDestination[0] = destination;
    u8 depth = baseShape()->isEnableZCmp() != 0;
    u8 writeDepth = baseShape()->isEnableZCmpUpdate() != 0;
    state.mDepthEnabled = depth;
    state.mDepthWriteEnabled = writeDepth;
    state.mDepthFunction = u32(sDepthCompareFunctions[baseShape()->getZCmpFunction()]);
    state.mAlphaEnabled = 1;
    u32 alphaFunction = sAlphaCompareFunctions[baseShape()->getAlphaCmpComp0()];
    u32 alphaReference = baseShape()->getAlphaCmpRef0();
    state.mLogicMask = 7;
    state.mAlphaFunction = alphaFunction;
    state.mAlphaReference = f32(alphaReference) / 255.0f;
    auto emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    if (u8(emitter->mGpuState.mGroupID) <= 6) state.mLogicMask = 0x17;
    JPADrawStateApply(&state);
}
/* WWHD glue
data sBlendSourceFactors 10172124
data sBlendDestinationFactors 1017214C
data sDepthCompareFunctions 101720E4
data sAlphaCompareFunctions 10172104
data sFrameTextureManager 101F9968
ext 027F81A4 JPAMathDraw::JPATextureDescriptor* JPADrawGetFrameTexture(void*,u32)
ext 027FE640 JPAMathDraw::JPATextureState* JPADrawGetParticleTextureState(void*,u32)
ext 027BDEB4 void JPADrawCopyTextureState(JPAMathDraw::JPATextureState*,JPAMathDraw::JPATextureDescriptor*)
ext 02750250 void JPADrawStateConstruct(JPADrawRenderState_l*)
ext 02750370 void JPADrawStateApply(JPADrawRenderState_l*)
ext 0282D4E8 void JPADrawHDChild(JPADraw_l*)
ext 0282D754 void JPADrawHDParticle(JPADraw_l*)
*/
