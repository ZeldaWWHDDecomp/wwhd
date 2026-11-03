// Shadow uniform updates and their original stack storage sequence.
#include "agl/ShadowPrePass.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
using namespace agl::effect_matrix;
static Row4 sShadowLastRow;
void initializeShadowMatrix(Matrix4*);
void convertShadowMatrix(Matrix4*, const Matrix4*);
void multiplyShadowMatrices(const Matrix4*, const Matrix4*, Matrix4*);
void constructShadowMatrices(Matrix4*, u32, u32, u32);
struct UniformScratch {
    sead::RenderVector2 mProduct, mQuarter, mOffset;
    Row4 mColor;
    be<f32> mNear, mDepthRatio, mNoise, mFadeStart, mFadeEnd, mFadeInverse;
    be<f32> mNormalRatio, mNormalMultiplier, mSecondNoise, mEdgeLow, mEdgeHigh;
    Matrix3 mRotation;
    Matrix4 mView, mConvertedView, mConvertedProjection, mCombined[3];
};
WWHD_OFFSET(UniformScratch,mNear,0x28);
WWHD_OFFSET(UniformScratch,mRotation,0x54);
WWHD_OFFSET(UniformScratch,mView,0x78);
WWHD_OFFSET(UniformScratch,mCombined,0x138);
WWHD_SIZE(UniformScratch,0x1F8);
static void setFrameUniform(agl::UniformBuffer*, u32, const void*, u32);
// An argument FPR stored with stfs: a value with more than single precision is truncated, not rounded.
static f32 storedSingle(f64 value) { return gabi::f32_from_bits(ppc_stfs_bits(value)); }

/* 0278FF3C */
void ShadowPrePass::updateUniforms(u32 frameIndex, const Matrix34* view, const Matrix4* projection,
                                  const Matrix4* transforms, const void* vectors,
                                  const agl::shadow_view::CameraView* cameraView, f64 nearPlane, f64 farPlane,
                                  f64 depthScale, f64 aspectScale) {
    gabi::AutoFrame frame{0x0278FF3Cu};
    FrameResources* resources = mFrames;
    if (frameIndex < mFrameCount)
        resources = guestElement(resources, frameIndex);
    if (mParameters.mEnabled.mValue == 0) return;
    gabi::FrameLocal<UniformScratch> scratch(8);
    scratch->mQuarter.y = 0.25f;
    scratch->mOffset.y = -0.5f;
    scratch->mQuarter.x = 0.25f;
    scratch->mOffset.x = -0.5f;
    initializeShadowMatrix(&scratch->mView);
    scratch->mView.setRows(view, &sShadowLastRow);
    convertShadowMatrix(&scratch->mConvertedView, &scratch->mView);
    convertShadowMatrix(&scratch->mConvertedProjection, projection);
    constructShadowMatrices(scratch->mCombined, 3, sizeof(Matrix4), gabi::guest_fn((const void*)&Matrix4::create));
    for (u32 i = 0; i != 3; ++i)
        multiplyShadowMatrices(&transforms[i], &scratch->mConvertedView, &scratch->mCombined[i]);
    resources->mDepthScale.x = storedSingle(depthScale);
    resources->mDepthScale.y = storedSingle(aspectScale);
    scratch->mNear = storedSingle(nearPlane);
    setFrameUniform(&resources->mBuffers[0], 0, &scratch->mNear, 1);
    f64 ratio = gabi::fdivs_fpr(nearPlane, farPlane);
    scratch->mDepthRatio = gabi::fsubs_fpr(1.0, ratio);
    setFrameUniform(&resources->mBuffers[0], 1, &scratch->mDepthRatio, 1);
    scratch->mProduct.y = storedSingle(depthScale);
    scratch->mProduct.x = gabi::fmuls_fpr(depthScale, aspectScale);
    setFrameUniform(&resources->mBuffers[0], 2, &scratch->mProduct, 1);
    setFrameUniform(&resources->mBuffers[0], 3, &scratch->mQuarter, 1);
    setFrameUniform(&resources->mBuffers[0], 4, &scratch->mConvertedView, 4);
    setFrameUniform(&resources->mBuffers[0], 5, &scratch->mConvertedProjection, 4);
    setFrameUniform(&resources->mBuffers[0], 6, scratch->mCombined, 12);
    setFrameUniform(&resources->mBuffers[0], 7, vectors, 3);
    scratch->mNoise = mParameters.mNoiseWidth.mValue;
    setFrameUniform(&resources->mBuffers[0], 8, &scratch->mNoise, 1);
    setFrameUniform(&resources->mBuffers[0], 9, &resources->mConfigurationVector, 1);
    f32 fadeEnd = mParameters.mFarFadeEnd.mValue;
    f32 fadeStart = mParameters.mFarFadeStart.mValue;
    f32 fadeRange = gabi::fsubs_ppc(fadeEnd, fadeStart);
    scratch->mFadeStart = fadeStart;
    f32 inverseFade = gabi::fdivs_ppc(1.0f, fadeRange);
    setFrameUniform(&resources->mBuffers[0], 11, &scratch->mFadeStart, 1);
    scratch->mFadeEnd = mParameters.mFarFadeEnd.mValue;
    setFrameUniform(&resources->mBuffers[0], 12, &scratch->mFadeEnd, 1);
    scratch->mFadeInverse = inverseFade;
    setFrameUniform(&resources->mBuffers[0], 13, &scratch->mFadeInverse, 1);
    scratch->mRotation.copyRotation(reinterpret_cast<const Matrix34*>(&scratch->mView));
    f32 green = mColor.y;
    f32 y0 = scratch->mRotation.mElements[0][1];
    f32 y2 = scratch->mRotation.mElements[2][1];
    f32 gy0 = gabi::fmuls_ppc(y0, green);
    f32 red = mColor.x;
    f32 y1 = scratch->mRotation.mElements[1][1];
    f32 gy2 = gabi::fmuls_ppc(y2, green);
    f32 x0 = scratch->mRotation.mElements[0][0];
    f32 blue = mColor.z;
    f32 gy1 = gabi::fmuls_ppc(y1, green);
    f32 x1 = scratch->mRotation.mElements[1][0];
    f32 xr0 = gabi::fmadds(x0, red, gy0);
    f32 x2 = scratch->mRotation.mElements[2][0];
    f32 xr1 = gabi::fmadds(x1, red, gy1);
    f32 z0 = scratch->mRotation.mElements[0][2];
    f32 xr2 = gabi::fmadds(x2, red, gy2);
    f32 z1 = scratch->mRotation.mElements[1][2];
    f32 z2 = scratch->mRotation.mElements[2][2];
    f32 colorX = gabi::fmadds(z0, blue, xr0);
    f32 alpha = mColor.w;
    f32 colorY = gabi::fmadds(z1, blue, xr1);
    scratch->mColor.mW = alpha;
    f32 colorZ = gabi::fmadds(z2, blue, xr2);
    scratch->mColor.mX = colorX;
    scratch->mColor.mY = colorY;
    scratch->mColor.mZ = colorZ;
    setFrameUniform(&resources->mBuffers[0], 15, &scratch->mColor, 1);
    scratch->mNormalRatio = mParameters.mNormalShadowRatio.mValue;
    setFrameUniform(&resources->mBuffers[0], 16, &scratch->mNormalRatio, 1);
    scratch->mNormalMultiplier = mParameters.mNormalShadowMultiplier.mValue;
    setFrameUniform(&resources->mBuffers[0], 17, &scratch->mNormalMultiplier, 1);
    f32 vertical = cameraView->mCenter.mY;
    f32 horizontal = cameraView->mCenter.mX;
    f32 doubledVertical = gabi::fadds_ppc(vertical, vertical);
    f32 doubledHorizontal = gabi::fadds_ppc(horizontal, horizontal);
    f64 horizontalDepth = gabi::fmuls_fpr(doubledHorizontal, depthScale);
    f64 verticalDepth = gabi::fmuls_fpr(doubledVertical, depthScale);
    f64 horizontalAspect = gabi::fmuls_fpr(horizontalDepth, aspectScale);
    scratch->mProduct.y = gabi::f32_from_fpr(verticalDepth);
    scratch->mProduct.x = gabi::f32_from_fpr(horizontalAspect);
    setFrameUniform(&resources->mBuffers[0], 18, &scratch->mProduct, 1);
    resources->mBuffers[0].flushNoSync(resources->mBuffers[0].cpuData, 0);
    setFrameUniform(&resources->mBuffers[1], 0, &resources->mConfigurationVector, 1);
    scratch->mSecondNoise = mParameters.mNoiseWidth.mValue;
    setFrameUniform(&resources->mBuffers[1], 1, &scratch->mSecondNoise, 1);
    resources->mBuffers[1].flushNoSync(resources->mBuffers[1].cpuData, 0);
    f32 threshold = mParameters.mEdgeThreshold.mValue;
    f32 edgeRange = mParameters.mEdgeRange.mValue;
    scratch->mEdgeLow = gabi::fnmsubs(edgeRange, 0.5f, threshold);
    setFrameUniform(&resources->mBuffers[2], 0, &scratch->mEdgeLow, 1);
    threshold = mParameters.mEdgeThreshold.mValue;
    edgeRange = mParameters.mEdgeRange.mValue;
    scratch->mEdgeHigh = gabi::fmadds(edgeRange, 0.5f, threshold);
    setFrameUniform(&resources->mBuffers[2], 1, &scratch->mEdgeHigh, 1);
    resources->mBuffers[2].flushNoSync(resources->mBuffers[2].cpuData, 0);
}
static void setFrameUniform(agl::UniformBuffer* buffer, u32 slot, const void* value, u32 count) {
    buffer->writeWords(buffer->cpuData, slot, static_cast<const be<u32>*>(value), 0, static_cast<s32>(count));
}
/* WWHD glue
data sShadowLastRow 104A09A4
ext 028E892C void initializeShadowMatrix(agl::effect_matrix::Matrix4*)
ext 0274EDD4 void convertShadowMatrix(agl::effect_matrix::Matrix4*,const agl::effect_matrix::Matrix4*)
ext 028E89B4 void multiplyShadowMatrices(const agl::effect_matrix::Matrix4*,const agl::effect_matrix::Matrix4*,agl::effect_matrix::Matrix4*)
ext 028EFFD0 void constructShadowMatrices(agl::effect_matrix::Matrix4*,u32,u32,u32)
*/
