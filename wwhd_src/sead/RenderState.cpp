// WWHD render state: reconstructed from the game's PowerPC instructions.
#include "sead/RenderState.h"

static u32 sCompactStateTable;
static u32 sRenderStateTable;
static sead::RasterModeTable sCompactCullModes;
static sead::RasterModeTable sCompactPolygonModes;
static sead::RasterModeTable sCullModes;
static sead::RasterModeTable sPolygonModes;

void* allocateRenderObject(u32 size);
void constructRenderArray(void* array, u32 count, u32 size, u32 constructor);
void setDepthStencil(u32 depth, u32 write, u32 compare, u32 stencil, u32 backStencil,
                     u32 stencilCompare, u32 stencilFail, u32 depthFail,
                     u32 stencilPass, u32 backCompare, u32 backFail, u32 backDepthFail, u32 backPass);
void setStencilMasks(u32 frontMask, u32 frontWrite, u32 frontReference,
                     u32 backMask, u32 backWrite, u32 backReference);
void setPolygon(u32 frontFace, u32 cullFront, u32 cullBack, u32 polygonMode,
                u32 frontMode, u32 backMode, u32 offset, u32 lineOffset, u32 pointOffset);
void setColorControl(u32 logicOp, u32 blendMask, u32 multiwrite, u32 colorWrite);
void setBlendControl(u32 target, u32 source, u32 destination, u32 equation,
                     u32 separateAlpha, u32 alphaSource, u32 alphaDestination, u32 alphaEquation);
void setBlendConstant(f32 red, f32 green, f32 blue, f32 alpha);
void setAlphaTest(u32 enable, u32 comparison, f32 reference);
void setChannelMasks(u32 red, u32 green, u32 blue, u32 alpha,
                     u32 targetFour, u32 targetFive, u32 targetSix, u32 targetSeven);
void setAlphaToMask(u32 enable, u32 mode);

/* 0274FDBC */
sead::CompactRenderState* sead::CompactRenderState::create(CompactRenderState* object) {
    if (object == NULL) object = (CompactRenderState*)allocateRenderObject(sizeof(CompactRenderState));
    if (object != NULL) {
        object->mCullMode = 1;
        object->mBlend.mAlphaSource = 4;
        object->mDepthComparison = 3;
        object->mBlendGreen = 1.0f;
        object->mTable = &sCompactStateTable;
        object->mBlendAlpha = 1.0f;
        object->mBlend.mAlphaDestination = 5;
        object->mBlend.mColorDestination = 5;
        object->mBlend.mAlphaEquation = 0;
        object->mLineOffset = 0;
        object->mBlend.mColorEquation = 0;
        object->mPointOffset = 0;
        object->mDepthWrite = 1;
        object->mStencilPass = 0;
        object->mStencilMask = ~0u;
        object->mWriteBlue = 1;
        object->mAlphaReference = 0.0f;
        object->mBackPolygon = 2;
        object->mWriteGreen = 1;
        object->mFrontPolygon = 2;
        object->mDepthTest = 1;
        object->mBlend.mColorSource = 4;
        object->mWriteRed = 1;
        object->mAlphaTest = 0;
        object->mBlendEnable = 1;
        object->mBlendRed = 1.0f;
        object->mStencilReference = 0;
        object->mBlendBlue = 1.0f;
        object->mStencilComparison = 0;
        object->mAlphaComparison = 4;
        object->mStencilFail = 0;
        object->mStencilEnable = 0;
        object->mWriteAlpha = 1;
        object->mPolygonOffset = 0;
        object->mStencilDepthFail = 0;
    }
    return object;
}

/* 0274FEB0 */
void sead::CompactRenderState::apply() {
    gabi::AutoFrame frame{gabi::current_function()};
    setDepthStencil(mDepthTest, mDepthWrite, mDepthComparison, mStencilEnable, mStencilEnable,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail);
    setStencilMasks((u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference,
                    (u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference);
    RasterState* cull = &sCompactCullModes.mEntries[(u32)mCullMode];
    u32 front = mFrontPolygon, back = mBackPolygon;
    setPolygon(0, cull->mFrontMode, cull->mBackMode, !(front == 2 && back == 2),
               front, back, mPolygonOffset, mLineOffset, mPointOffset);
    setColorControl(0xCC, mBlendEnable != 0 ? 255 : 0, 0,
                    mWriteRed != 0 || mWriteGreen != 0 || mWriteBlue != 0 || mWriteAlpha != 0);
    setBlendControl(0, mBlend.mColorSource, mBlend.mColorDestination, mBlend.mColorEquation,
                    1, mBlend.mAlphaSource, mBlend.mAlphaDestination, mBlend.mAlphaEquation);
    setBlendConstant(mBlendRed, mBlendGreen, mBlendBlue, mBlendAlpha);
    setAlphaTest(mAlphaTest, mAlphaComparison, mAlphaReference);
    u32 mask = (u32)mWriteRed + ((u32)mWriteGreen << 1) + ((u32)mWriteBlue << 2) + ((u32)mWriteAlpha << 3);
    setChannelMasks(mask, 0, 0, 0, 0, 0, 0, 0);
    setAlphaToMask(0, 0);
}

/* 02750070 */
void sead::CompactRenderState::applyDepthStencil() {
    gabi::AutoFrame frame{gabi::current_function()};
    setDepthStencil(mDepthTest, mDepthWrite, mDepthComparison, mStencilEnable, mStencilEnable,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail);
    setStencilMasks((u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference,
                    (u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference);
}

/* 027500F0 */
void sead::CompactRenderState::applyChannelMask() {
    u32 mask = (u32)mWriteRed + ((u32)mWriteGreen << 1) + ((u32)mWriteBlue << 2) + ((u32)mWriteAlpha << 3);
    setChannelMasks(mask, 0, 0, 0, 0, 0, 0, 0);
}

/* 02750138 */
void sead::CompactRenderState::applyBlend() {
    setColorControl(0xCC, mBlendEnable != 0 ? 255 : 0, 0,
                    mWriteRed != 0 || mWriteGreen != 0 || mWriteBlue != 0 || mWriteAlpha != 0);
    setBlendControl(0, mBlend.mColorSource, mBlend.mColorDestination, mBlend.mColorEquation,
                    1, mBlend.mAlphaSource, mBlend.mAlphaDestination, mBlend.mAlphaEquation);
}

/* 027501D8 */
void sead::CompactRenderState::applyPolygon() {
    gabi::AutoFrame frame{gabi::current_function()};
    RasterState* cull = &sCompactPolygonModes.mEntries[(u32)mCullMode];
    u32 front = mFrontPolygon, back = mBackPolygon;
    setPolygon(0, cull->mFrontMode, cull->mBackMode, !(front == 2 && back == 2),
               front, back, mPolygonOffset, mLineOffset, mPointOffset);
}

/* 02750250 */
sead::RenderState* sead::RenderState::create(RenderState* object) {
    if (object == NULL) object = (RenderState*)allocateRenderObject(sizeof(RenderState));
    if (object == NULL) return object;
    object->mCullMode = 1;
    object->mTable = &sRenderStateTable;
    object->mDepthWrite = 1;
    object->mDepthTest = 1;
    object->mDepthComparison = 3;
    be<u32>* blendMask = &object->mBlendMask;
    if (gabi::ea(blendMask) == 0) blendMask = (be<u32>*)allocateRenderObject(4);
    if (gabi::ea(blendMask) != 0) *blendMask = 255;
    BlendState* blend = &object->mBlend[0];
    if (gabi::ea(blend) == 0) blend = (BlendState*)allocateRenderObject(0xC0);
    if (gabi::ea(blend) != 0) constructRenderArray(blend, 8, sizeof(BlendState), 0x027506FC);
    object->mStencilEnable = 0;
    object->mAlphaComparison = 4;
    object->mStencilMask = ~0u;
    object->mBackPolygon = 2;
    object->mFrontPolygon = 2;
    object->mAlphaReference = 0.0f;
    object->mStencilPass = 0;
    object->mPointOffset = 0;
    object->mBlendBlue = 1.0f;
    object->mStencilFail = 0;
    object->mStencilDepthFail = 0;
    object->mLineOffset = 0;
    object->mStencilReference = 0;
    object->mBlendAlpha = 1.0f;
    object->mBlendRed = 1.0f;
    object->mPolygonOffset = 0;
    object->mAlphaTest = 0;
    object->mBlendGreen = 1.0f;
    object->mStencilComparison = 0;
    object->mChannelMask = ~0u;
    return object;
}

/* 02750370 */
void sead::RenderState::apply() {
    gabi::AutoFrame frame{gabi::current_function()};
    setDepthStencil(mDepthTest, mDepthWrite, mDepthComparison, mStencilEnable, mStencilEnable,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail);
    setStencilMasks((u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference,
                    (u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference);
    RasterState* cull = &sCullModes.mEntries[(u32)mCullMode];
    u32 front = mFrontPolygon, back = mBackPolygon;
    setPolygon(0, cull->mFrontMode, cull->mBackMode, !(front == 2 && back == 2),
               front, back, mPolygonOffset, mLineOffset, mPointOffset);
    setColorControl(0xCC, (u8)(u32)mBlendMask, 0, mChannelMask != 0);
    for (u32 target = 0; target < 8; ++target) {
        BlendState* blend = &mBlend[target];
        setBlendControl(target, blend->mColorSource, blend->mColorDestination, blend->mColorEquation,
                        1, blend->mAlphaSource, blend->mAlphaDestination, blend->mAlphaEquation);
    }
    setBlendConstant(mBlendRed, mBlendGreen, mBlendBlue, mBlendAlpha);
    setAlphaTest(mAlphaTest, mAlphaComparison, mAlphaReference);
    u32 mask = mChannelMask;
    setChannelMasks(mask & 15, (mask >> 4) & 15, (mask >> 8) & 15, (mask >> 12) & 15,
                    (mask >> 16) & 15, (mask >> 20) & 15, (mask >> 24) & 15, mask >> 28);
    setAlphaToMask(0, 0);
}

/* 02750520 */
void sead::RenderState::applyAlpha() {
    setAlphaTest(mAlphaTest, mAlphaComparison, mAlphaReference);
}

/* 02750534 */
void sead::RenderState::applyDepthStencil() {
    gabi::AutoFrame frame{gabi::current_function()};
    setDepthStencil(mDepthTest, mDepthWrite, mDepthComparison, mStencilEnable, mStencilEnable,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail,
                    mStencilComparison, mStencilPass, mStencilDepthFail, mStencilFail);
    setStencilMasks((u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference,
                    (u8)(u32)mStencilMask, 255, (u8)(u32)mStencilReference);
}

/* 027505B4 */
void sead::RenderState::applyChannelMask() {
    u32 mask = mChannelMask;
    setChannelMasks(mask & 15, (mask >> 4) & 15, (mask >> 8) & 15, (mask >> 12) & 15,
                    (mask >> 16) & 15, (mask >> 20) & 15, (mask >> 24) & 15, mask >> 28);
}

/* 027505DC */
void sead::RenderState::applyBlend() {
    setColorControl(0xCC, (u8)(u32)mBlendMask, 0, mChannelMask != 0);
    for (u32 target = 0; target < 8; ++target) {
        BlendState* blend = &mBlend[target];
        setBlendControl(target, blend->mColorSource, blend->mColorDestination, blend->mColorEquation,
                        1, blend->mAlphaSource, blend->mAlphaDestination, blend->mAlphaEquation);
    }
}

/* 02750684 */
void sead::RenderState::applyPolygon() {
    gabi::AutoFrame frame{gabi::current_function()};
    RasterState* cull = &sPolygonModes.mEntries[(u32)mCullMode];
    u32 front = mFrontPolygon, back = mBackPolygon;
    setPolygon(0, cull->mFrontMode, cull->mBackMode, !(front == 2 && back == 2),
               front, back, mPolygonOffset, mLineOffset, mPointOffset);
}

/* 027506FC */
sead::BlendState* sead::BlendState::create(BlendState* object) {
    if (object == NULL) object = (BlendState*)allocateRenderObject(sizeof(BlendState));
    if (object != NULL) {
        object->mColorSource = 4;
        object->mColorEquation = 0;
        object->mColorDestination = 5;
        object->mAlphaSource = 4;
        object->mAlphaDestination = 5;
        object->mAlphaEquation = 0;
    }
    return object;
}

/* WWHD glue
 data sCompactStateTable 10145468
 data sRenderStateTable 101454C0
 data sCompactCullModes 10145480
 data sCompactPolygonModes 101454A0
 data sCullModes 101454D8
 data sPolygonModes 101454F8
 ext 0273AD10 void* allocateRenderObject(u32)
 ext 028EFFD0 void constructRenderArray(void*, u32, u32, u32)
 ext C0006848 void setDepthStencil(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32)
 ext C0006990 void setStencilMasks(u32,u32,u32,u32,u32,u32)
 ext C0006930 void setPolygon(u32,u32,u32,u32,u32,u32,u32,u32,u32)
 ext C00067D0 void setColorControl(u32,u32,u32,u32)
 ext C00067B0 void setBlendControl(u32,u32,u32,u32,u32,u32,u32,u32)
 ext C00067A0 void setBlendConstant(f32,f32,f32,f32)
 ext C0006778 void setAlphaTest(u32,u32,f32)
 ext C00069F0 void setChannelMasks(u32,u32,u32,u32,u32,u32,u32,u32)
 ext C0006788 void setAlphaToMask(u32,u32)
*/
