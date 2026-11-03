/* agl SSAO: the occlusion kernel (spiral sample placement and nearest-sample disk integration) and the sixteen
 * 4x4 rotation-noise textures. */
// GX2 surface values: wut facts.
#include "agl/SsaoTargetSet.h"
#include "agl/BitmapTransfer.h"
#include <atomic>
void constructSsaoKernelArray(void*,u32,u32,u32);
f64 ssaoKernelCosine(f64);
f64 ssaoKernelSine(f64);
struct SsaoKernelPoint { be<f32> mX,mY; };
struct SsaoKernelPoints { SsaoKernelPoint mPoints[19]; };
struct SsaoKernelWeights { be<f32> mValues[19]; };
/* 02793908 */
void initializeSsaoKernel(agl::SsaoEffect* effect,u32 resolutionBits) {
    gabi::AutoFrame frame{0x02793908};
    struct KernelScratch {
        be<u32> mConversionHigh,mConversionLow;
        u8 mPadding[4];
        be<f32> mRowY,mNegativeX,mNegativeY,mCopyX,mCopyY;
    };
    gabi::FrameLocal<KernelScratch> scratch(8);
    auto readConverted = [&]() -> f32 {
        u64 bits = ((u64)(u32)scratch->mConversionHigh << 32) | (u32)scratch->mConversionLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return (f32)(encoded - 4503601774854144.0);
    };
    auto ssaoKernelDepthExtent = [](f32 squaredDepth) -> f32 {
    if (!(squaredDepth > 0.0f)) {
        f32 half = gabi::fmuls_ppc(0.0f,squaredDepth);
        return gabi::fadds_ppc(half,half);
    }
    f64 estimate = ppc_frsqrte((f64)squaredDepth);
    f32 estimateSquared = gabi::fmuls_fpr(estimate,estimate);
    f32 halfEstimate = gabi::fmuls_fpr(estimate,0.5);
    f32 correction = gabi::fnmsubs(estimateSquared,squaredDepth,3.0f);
    f32 root = gabi::fmuls_ppc(gabi::fmuls_ppc(correction,halfEstimate),squaredDepth);
    return gabi::fadds_ppc(root,root);
};
    s32 pairs = effect->mSamplePairCount.mValue;
    if (pairs > 9) pairs = 9;
    effect->mSamplePairCount.mValue = pairs;
    s32 sampleCount = (s32)((u32)pairs * 2 + 1);
    gabi::FrameLocal<SsaoKernelPoints> samples(0x28);
    gabi::FrameLocal<SsaoKernelWeights> weights(0xC0);
    constructSsaoKernelArray(samples.get(),19,sizeof(SsaoKernelPoint),
                            gabi::guest_fn((const void*)agl::ssao_storage::allocateOrReuseStorage));
    f32 totalWeight = 0.0f;
    weights->mValues[0] = 0.0f;
    samples->mPoints[0].mX = 0.0f;
    samples->mPoints[0].mY = 0.0f;
    if (pairs > 0) {
        scratch->mConversionHigh = 0x43300000u;
        scratch->mConversionLow = (u32)pairs ^ 0x80000000u;
        f32 pairCount = readConverted();
        f32 radialStep = gabi::fdivs_ppc(1.0f,gabi::fadds_ppc(pairCount,1.0f));
        for (s32 pair = 0; pair < pairs; ++pair) {
            scratch->mConversionLow = (u32)pair ^ 0x80000000u;
            scratch->mConversionHigh = 0x43300000u;
            f32 position = readConverted();
            f32 angle = gabi::fadds_ppc(gabi::fdivs_ppc(gabi::fmuls_ppc(position,3.1415927f),pairCount),0.1f);
            f32 radius = gabi::fmuls_ppc(radialStep,gabi::fadds_ppc(position,1.2f));
            weights->mValues[pair * 2 + 1] = 0.0f;
            f64 cosine = ssaoKernelCosine((f64)angle);
            f64 sine = ssaoKernelSine((f64)angle);
            f32 x = gabi::fmuls_fpr((f64)radius,cosine);
            weights->mValues[pair * 2 + 2] = 0.0f;
            samples->mPoints[pair * 2 + 1].mX = x;
            f32 y = gabi::fmuls_fpr((f64)radius,sine);
            scratch->mNegativeX = -x;
            f32 negativeX = scratch->mNegativeX;
            samples->mPoints[pair * 2 + 1].mY = y;
            scratch->mCopyX = negativeX;
            scratch->mNegativeY = -y;
            f32 negativeY = scratch->mNegativeY;
            negativeX = scratch->mCopyX;
            scratch->mCopyY = negativeY;
            negativeY = scratch->mCopyY;
            samples->mPoints[pair * 2 + 2].mX = negativeX;
            samples->mPoints[pair * 2 + 2].mY = negativeY;
        }
    }
    s32 resolution = (s32)resolutionBits;
    scratch->mConversionHigh = 0x43300000u;
    scratch->mConversionLow = (u32)resolution ^ 0x80000000u;
    f32 resolutionFloat = readConverted();
    f32 step = gabi::fdivs_ppc(2.0f,resolutionFloat);
    f32 areaScale = gabi::fdivs_ppc(gabi::fadds_ppc(step,step),resolutionFloat);
    if (resolution > 0) {
        s32 halfResolution = resolution / 2;
        for (s32 row = 0; row < resolution; ++row) {
            scratch->mConversionHigh = 0x43300000u;
            scratch->mConversionLow = (u32)halfResolution ^ 0x80000000u;
            f32 half = readConverted();
            scratch->mConversionLow = (u32)(row - halfResolution) ^ 0x80000000u;
            f32 rowValue = readConverted();
            f32 y = gabi::fdivs_ppc(rowValue,half);
            scratch->mRowY = y;
            for (s32 column = 0; column < resolution; ++column) {
                if (column != 0) {
                    scratch->mConversionHigh = 0x43300000u;
                    scratch->mConversionLow = (u32)halfResolution ^ 0x80000000u;
                    half = readConverted();
                    y = scratch->mRowY;
                }
                scratch->mConversionLow = (u32)(column - halfResolution) ^ 0x80000000u;
                scratch->mConversionHigh = 0x43300000u;
                f32 columnValue = readConverted();
                f32 x = gabi::fdivs_ppc(columnValue,half);
                f32 radiusSquared = gabi::fmadds(x,x,gabi::fmuls_ppc(y,y));
                if (radiusSquared > 1.0f) continue;
                f32 dy = gabi::fsubs_ppc(y,samples->mPoints[0].mY);
                f32 dx = gabi::fsubs_ppc(x,samples->mPoints[0].mX);
                f32 nearestDistance = gabi::fmadds(dx,dx,gabi::fmuls_ppc(dy,dy));
                s32 nearest = 0;
                for (s32 sample = 1; sample < sampleCount; ++sample) {
                    f32 sampleX = samples->mPoints[sample].mX;
                    f32 sampleY = samples->mPoints[sample].mY;
                    f32 deltaY = gabi::fsubs_ppc(y,sampleY);
                    f32 deltaX = gabi::fsubs_ppc(x,sampleX);
                    f32 distance = gabi::fmadds(deltaX,deltaX,gabi::fmuls_ppc(deltaY,deltaY));
                    if (nearestDistance > distance) {
                        nearestDistance = distance;
                        nearest = sample;
                    }
                }
                f32 depthExtent = ssaoKernelDepthExtent(gabi::fsubs_ppc(1.0f,radiusSquared));
                f32 weight = gabi::fmuls_ppc(depthExtent,areaScale);
                f32 oldWeight = weights->mValues[nearest];
                f32 newWeight = gabi::fadds_ppc(oldWeight,weight);
                totalWeight = gabi::fadds_ppc(totalWeight,weight);
                weights->mValues[nearest] = newWeight;
            }
        }
    }
    for (s32 sample = 0; sample < sampleCount; ++sample)
        weights->mValues[sample] = gabi::fdivs_ppc(weights->mValues[sample],totalWeight);
    effect->mCenterWeight = weights->mValues[0];
    for (s32 pair = 0; pair < pairs; ++pair) {
        const SsaoKernelPoint* point = &samples->mPoints[pair * 2 + 1];
        f32 y = point->mY;
        f32 x = point->mX;
        f32 depthSquared = gabi::fsubs_ppc(1.0f,gabi::fmadds(x,x,gabi::fmuls_ppc(y,y)));
        f32 depthExtent = ssaoKernelDepthExtent(depthSquared);
        if (depthSquared > 0.0f) { y = point->mY; x = point->mX; }
        f32 weight = weights->mValues[pair * 2 + 1];
        effect->mKernel[pair].mY = y;
        effect->mKernel[pair].mDepthExtent = depthExtent;
        effect->mKernel[pair].mX = x;
        effect->mKernel[pair].mWeight = weight;
        effect->mKernelDepths[pair] = depthExtent;
    }
    std::atomic_thread_fence(std::memory_order_acquire);
}
struct SsaoSampleOrder { be<u32> mIndices[16]; };
struct SsaoSampleTrigEntry { be<f32> mFirstBase,mFirstSlope,mSecondBase,mSecondSlope; };
struct SsaoSampleTrigTable { SsaoSampleTrigEntry mEntries[256]; };
WWHD_SIZE(SsaoSampleOrder,64);
WWHD_SIZE(SsaoSampleTrigTable,0x1000);
// Owned by AGL#8. Permutation values stay in the supplied image, per source-data rule.
static const SsaoSampleOrder sSsaoSampleOrder;
// Borrowed read-only view of the merged map animation table, without an initializer.
static const SsaoSampleTrigTable sSsaoSampleTrigTable;
/* 0279345C */
void initializeSsaoSampleData(agl::SsaoEffect* effect) {
    gabi::AutoFrame frame{0x0279345C};
    struct IntToFloatSlot { be<u32> mHigh,mLow; };
    gabi::FrameLocal<IntToFloatSlot> conversion(0x10);
    for (u32 texture = 0; texture < 16; ++texture) {
        auto* pixels = (be<u16>*)(void*)effect->mSampleImages[texture];
        for (u32 pixel = 0; pixel < 16; ++pixel) {
            u32 firstIndex = sSsaoSampleOrder.mIndices[texture];
            u32 phaseOffset = sSsaoSampleOrder.mIndices[firstIndex];
            s32 phaseIndex = (s32)(pixel + phaseOffset) % 16;
            u32 phase = sSsaoSampleOrder.mIndices[phaseIndex];
            u32 entry = (phase & 31) * 8;
            const auto& coefficients = sSsaoSampleTrigTable.mEntries[entry];
            f32 secondSlope = coefficients.mSecondSlope;
            f32 firstSlope = coefficients.mFirstSlope;
            f32 firstBase = coefficients.mFirstBase;
            f32 secondBase = coefficients.mSecondBase;
            // The disjoint fractional masks produce zero, but the unsigned conversion writes its slot.
            conversion->mHigh = 0x43300000u;
            conversion->mLow = 0;
            u64 encodedBits = ((u64)(u32)conversion->mHigh << 32) | (u32)conversion->mLow;
            f64 encodedFraction;
            __builtin_memcpy(&encodedFraction,&encodedBits,sizeof(encodedFraction));
            f32 phaseFraction = (f32)(encodedFraction - 4503599627370496.0);
            f32 first = gabi::fmadds(firstSlope,phaseFraction,firstBase);
            f32 second = gabi::fmadds(secondSlope,phaseFraction,secondBase);
            f32 firstBiased = gabi::fadds_ppc(first,1.0f);
            f32 secondBiased = gabi::fadds_ppc(second,1.0f);
            f32 secondUnit = gabi::fmuls_ppc(secondBiased,0.5f);
            f32 firstUnit = gabi::fmuls_ppc(firstBiased,0.5f);
            s32 firstInteger = gabi::ftoi(gabi::fmuls_ppc(255.0f,firstUnit));
            f32 secondScaled = gabi::fmuls_ppc(255.0f,secondUnit);
            conversion->mHigh = (u32)firstInteger;
            u32 firstByte = (u32)conversion->mHigh & 0xFF;
            s32 secondInteger = gabi::ftoi(secondScaled);
            conversion->mHigh = (u32)secondInteger;
            u32 secondByte = (u32)conversion->mHigh & 0xFF;
            // The 4x4 noise image is written row by row, 16 contiguous pixels.
            pixels[pixel] = firstByte | secondByte << 8;
        }
        // A 4x4 texture (dimension 2D, format 0xA) over the image, bound to its sampler with wrap and point modes.
        agl::TextureData* noise = &effect->mSampleTextures[texture];
        noise->configure(1,0xA,4,4,1,1,0,0);
        void* image = effect->mSampleImages[texture];
        noise->surface.mImage = image;
        image = effect->mSampleImages[texture];
        agl::replaceBitmapMip(reinterpret_cast<agl::SurfaceData_l*>(noise),image,0);
        agl::TextureSampler* sampler = &effect->mSampleSamplers[texture];
        if (!agl::sameTextureLayout(sampler->texture,*noise)) {
            sampler->copyTexture(noise);
        } else {
            u32 imageWord = noise->surface.mImage.v;
            u32 mipImage = noise->surface.mMipImage.v;
            agl::nativeSurface(sampler)->mImage.v = imageWord;
            sampler->texture.surface.mImage.v = imageWord;
            sampler->texture.surface.mMipImage.v = mipImage;
            agl::nativeSurface(sampler)->mMipImage.v = mipImage;
        }
        u32 dirty = sampler->dirty;
        sampler->settings.modes[0] = 0;
        sampler->settings.modes[1] = 0;
        sampler->dirty = dirty | 4;
        sampler->settings.modes[2] = 0;
        dirty = sampler->dirty;
        sampler->settings.modes[3] = 0;
        sampler->settings.modes[4] = 0;
        sampler->dirty = dirty | 2;
        sampler->settings.modes[5] = 0;
    }
    std::atomic_thread_fence(std::memory_order_acquire);
}
/* WWHD glue
ext 028EFFD0 void constructSsaoKernelArray(void*,u32,u32,u32)
ext 028F4BE0 f64 ssaoKernelCosine(f64)
ext 028F43F8 f64 ssaoKernelSine(f64)
data sSsaoSampleOrder 101F8D80
data sSsaoSampleTrigTable 10145FA0
*/
