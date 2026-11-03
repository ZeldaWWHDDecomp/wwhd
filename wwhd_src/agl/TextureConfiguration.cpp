/* Texture description conversion and forward copying from assigned WWHD code. */
#include "agl/TextureData.h"
using namespace agl;
static u32 toNativeTextureFormat(u32);
static u32 fromNativeTextureFormat(u32);
static u32 textureComponentMapping(u32, u32);
/* 027BE930 */
void TextureData::configure(
    u32 dimension, u32 inputFormat, u32 width, u32 height, u32 depth, s32 mipCount, u32 tileMode, u32 antiAlias) {
    gabi::AutoFrame frame{gabi::current_function()};
    surface.mDimension = dimension;
    format = inputFormat;
    surface.mFormat = toNativeTextureFormat(inputFormat);
    surface.mSamples = antiAlias;
    bool depthFormat = inputFormat >= 0x39 && inputFormat <= 0x3B;
    surface.mTileMode = tileMode;
    surface.mUsage = depthFormat ? 5 : 1;
    surface.mSwizzle = 0;
    setDimensions(width, height, depth);
    setMipCount(mipCount);
    componentMapping[0] = textureComponentMapping(inputFormat, 0);
    componentMapping[1] = textureComponentMapping(inputFormat, 1);
    componentMapping[2] = textureComponentMapping(inputFormat, 2);
    u32 lastMapping = textureComponentMapping(inputFormat, 3);
    if (depthFormat) {
        componentMapping[3] = lastMapping;
        surface.mImage = nullptr;
        surface.mMipImage = nullptr;
    } else {
        surface.mMipImage = nullptr;
        componentMapping[3] = lastMapping;
        surface.mImage = nullptr;
    }
}
/* 027BEA90 */
void TextureData::copySurface(const sead::GraphicsSurface* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 sourceAddress = gabi::ea(source);
    const be<u32>* input = gabi::at<const be<u32>>(sourceAddress);
    be<u32>* output = gabi::at<be<u32>>(gabi::ea(&surface));
    for (u32 i = 0; i < 29; ++i)
        output[i] = input[i];
    u32 convertedFormat = fromNativeTextureFormat(surface.mFormat);
    u32 height = surface.mHeight;
    u32 depth = surface.mDepth;
    u32 width = surface.mWidth;
    format = convertedFormat;
    setDimensions(width, height, depth);
    setMipCount(static_cast<s32>(source->mMipCount));
    componentMapping[0] = textureComponentMapping(format, 0);
    componentMapping[1] = textureComponentMapping(format, 1);
    componentMapping[2] = textureComponentMapping(format, 2);
    componentMapping[3] = textureComponentMapping(format, 3);
}
/* WWHD glue
ext 02786348 u32 toNativeTextureFormat(u32)
ext 02786314 u32 fromNativeTextureFormat(u32)
ext 027BFF9C u32 textureComponentMapping(u32,u32)
*/
