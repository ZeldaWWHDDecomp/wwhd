/* Texture lifecycle and mip extents recovered from assigned WWHD instructions. */
#include "agl/TextureData.h"
using namespace agl;
static u8 sTextureDataMethods;
static void* allocateTextureData(u32);
static void releaseTextureData(void*);
static void clearTextureSurface(sead::GraphicsSurface*, u32, u32);
static void invalidateTextureMemory(u32, const void*, u32);
namespace {
enum SurfaceDimension : u32 { Cube = 3, Array1D = 4, Array2D = 5, Multisampled2D = 6, MultisampledArray2D = 7 };

}
/* 027BE6B8 */
TextureData* TextureData::create(TextureData* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object = static_cast<TextureData*>(allocateTextureData(sizeof(TextureData)));
    if (object) {
        object->minimumDepth = 1;
        object->componentMapping[3] = 4;
        object->componentMapping[2] = 4;
        object->methods = &sTextureDataMethods;
        object->minimumHeight = 1;
        object->minimumWidth = 1;
        object->componentMapping[0] = 4;
        object->componentMapping[1] = 4;
        object->maximumMipCount = 1;
        clearTextureSurface(&object->surface, 0, sizeof(sead::GraphicsSurface));
        object->format = 0;
    }
    return object;
}
/* 027BE74C */
void TextureData::setMipCount(s32 count) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 dimension = surface.mDimension;
    if (dimension == Multisampled2D || dimension == MultisampledArray2D)
        surface.mMipCount = 1;
    else {
        u32 maximum = maximumMipCount;
        if (count <= 0)
            count = 1;
        else if (count > static_cast<s32>(maximum))
            count = static_cast<s32>(maximum);
        surface.mMipCount = static_cast<u32>(count);
    }
    updateSurface();
}
/* 027BE794 */
void TextureData::invalidate() const {
    gabi::AutoFrame frame{gabi::current_function()};
    void* image = surface.mImage;
    if (image) invalidateTextureMemory(2, image, surface.mImageBytes);
    if (surface.mMipImage.get()) invalidateTextureMemory(2, surface.mMipImage.get(), surface.mMipBytes);
}
static u32 mipExtent(u32 extent, u32 shift, u32 minimum) {
    u32 reduced = (shift & 32u) ? 0u : extent >> (shift & 31u);
    // The original clamps each shifted word with a signed comparison.
    return static_cast<s32>(minimum) > static_cast<s32>(reduced) ? minimum : reduced;
}
/* 027BE7F0 */
void TextureData::setDimensions(u32 width, u32 height, u32 depth) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 dimension = surface.mDimension;
    surface.mHeight = height;
    surface.mWidth = width;
    minimumWidth = 1;
    surface.mDepth = depth;
    u32 heightMinimum = dimension == Array1D ? static_cast<u8>(height) : 1;
    minimumHeight = heightMinimum;
    u32 depthMinimum = 1;
    if (dimension == Cube || dimension == Array2D || dimension == MultisampledArray2D)
        depthMinimum = static_cast<u8>(depth);
    minimumDepth = depthMinimum;
    if (dimension == Multisampled2D || dimension == MultisampledArray2D) {
        maximumMipCount = 1;
        return;
    }
    u32 storedDepth = surface.mDepth;
    u32 storedWidth = surface.mWidth;
    u32 storedHeight = surface.mHeight;
    u32 widthMinimum = minimumWidth;
    heightMinimum = minimumHeight;
    for (u32 previous = 0, next = 1;; ++previous, ++next) {
        if (mipExtent(storedWidth, next, widthMinimum) != mipExtent(storedWidth, previous, widthMinimum)) continue;
        if (mipExtent(storedHeight, next, heightMinimum) != mipExtent(storedHeight, previous, heightMinimum)) continue;
        if (mipExtent(storedDepth, next, depthMinimum) != mipExtent(storedDepth, previous, depthMinimum)) continue;
        maximumMipCount = static_cast<u8>(next);
        return;
    }
}
/* 027BEB48 */
void TextureData::destroy(TextureData* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object && (flags & 1)) releaseTextureData(object);
}
/* WWHD glue
data sTextureDataMethods 10169D40
ext 0273AD10 void* allocateTextureData(u32)
ext 0273AF40 void releaseTextureData(void*)
ext C0009990 void clearTextureSurface(sead::GraphicsSurface*,u32,u32)
ext C00064A8 void invalidateTextureMemory(u32,const void*,u32)
*/
