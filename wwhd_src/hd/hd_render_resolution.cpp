/* HD render resolutions: television, internal render buffer, and DRC. Private decomp source. */
#include "hd/hd_render_resolution.h"
static hdRenderResolutionHeaderZero_c sRenderResolutionHeaderZero;
static hdRenderResolutionHeaderDescriptor_c sRenderResolutionHeaderDescriptor;
static hdRenderResolutionHeaderDescriptor_c sRenderResolutionRotationDescriptor;
static hdRenderResolutionHeaderDescriptor_c sRenderResolutionPositiveRotationDescriptor;
static f32 sRenderResolutionMinRotation;
static f32 sRenderResolutionMaxRotation;
static u8 sRenderResolutionRotationGuard;
static u8 sRenderResolutionPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static hdRenderSize_c sRenderResolutionTvSize;
static hdRenderSize_c sRenderResolutionRenderSize;
static hdRenderSize_c sRenderResolutionDrcSize;

/* inlined into render dimension accessors */
static u32 dimensionToUnsigned(f32 dimension) {
    const f32 unsignedBias = 2147483648.0f;
    if (dimension < unsignedBias) {
        return u32(gabi::ftoi(dimension));
    }
    return u32(gabi::ftoi(gabi::fsubs_ppc(dimension, unsignedBias))) + 0x80000000u;
}
/* 02739398 */
static u32 getTvWidth() { return dimensionToUnsigned(sRenderResolutionTvSize.mWidth); }
/* 027393E8 */
static u32 getTvHeight() { return dimensionToUnsigned(sRenderResolutionTvSize.mHeight); }
/* 02739438 */
static u32 getDisplayWidth() { return dimensionToUnsigned(sRenderResolutionTvSize.mWidth); }
/* 02739488 */
static u32 getDisplayHeight() { return dimensionToUnsigned(sRenderResolutionTvSize.mHeight); }
/* 027394D8 */
static u32 getDrcWidth() { return dimensionToUnsigned(sRenderResolutionDrcSize.mWidth); }
/* 02739528 */
static u32 getDrcHeight() { return dimensionToUnsigned(sRenderResolutionDrcSize.mHeight); }
/* 02739578 */
static hdRenderSize_c* getRenderSize(s32 television) {
    return television == 0 ? &sRenderResolutionDrcSize : &sRenderResolutionRenderSize;
}
/* 02739594 */
static u32 getTvTargetWidth() { return dimensionToUnsigned(sRenderResolutionTvSize.mWidth); }
/* 027395E4 */
static u32 getTvTargetHeight() { return dimensionToUnsigned(sRenderResolutionTvSize.mHeight); }
/* 02739634 */
static void __sinit_hd_render_resolution_cpp() {
    for (u32 i = 0; i < 4; ++i) sRenderResolutionHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sRenderResolutionHeaderDescriptor));
    sRenderResolutionMinRotation = -3.1415927410125732f;
    sRenderResolutionMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sRenderResolutionRotationGuard);
    __register_global_object(gabi::ea(&sRenderResolutionRotationDescriptor));
    initializePositiveRotationGuard(&sRenderResolutionPositiveRotationGuard);
    __register_global_object(gabi::ea(&sRenderResolutionPositiveRotationDescriptor));
    sRenderResolutionTvSize.mWidth = 1920.0f;
    sRenderResolutionTvSize.mHeight = 1080.0f;
    sRenderResolutionDrcSize.mWidth = 854.0f;
    sRenderResolutionRenderSize.mWidth = 1280.0f;
    sRenderResolutionRenderSize.mHeight = 720.0f;
    sRenderResolutionDrcSize.mHeight = 480.0f;
}
/* WWHD glue
 data sRenderResolutionHeaderZero 1049FB5C
 data sRenderResolutionHeaderDescriptor 101F8A9C
 data sRenderResolutionRotationDescriptor 101F8AA8
 data sRenderResolutionPositiveRotationDescriptor 101F8AB4
 data sRenderResolutionMinRotation 1049FB50
 data sRenderResolutionMaxRotation 1049FB54
 data sRenderResolutionRotationGuard 1049FB58
 data sRenderResolutionPositiveRotationGuard 1049FB59
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sRenderResolutionTvSize 1049FB6C
 data sRenderResolutionRenderSize 1049FB74
 data sRenderResolutionDrcSize 1049FB7C
*/
