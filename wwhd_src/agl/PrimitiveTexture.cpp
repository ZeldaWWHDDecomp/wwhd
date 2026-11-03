// agl::utl::PrimitiveTexture: the singleton of sixteen built-in 4x4 textures (white, gray, black and clear 2D
// textures, black textures of every dimension, two depth values, depth-compare shadow textures and a 16x8
// mip-level colour pattern), each with its sampler; construction, initialization, teardown and the registered
// disposer. Written from WWHD's instructions.
#include "agl/PrimitiveTexture.h"
#include "agl/BitmapTransfer.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void releaseImage(void* image);
static void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
static void* allocateSampler(u32 size, sead::RegistryHeap* heap, s32 alignment);
static void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection);
static void destroyRegistered(sead::RegisteredObject* object, u32 flags);
static TextureSampler* createSampler(TextureSampler* sampler, const TextureData* texture);
static void updateSampler(TextureSampler* sampler);
static void destroyArray(void* objects, s32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
static void destroySampler(TextureSampler* sampler, u32 flags);
static void flushRange(void* data, u32 bytes);
static PrimitiveTexture* sPrimitiveTexture;
static PrimitiveTextureDisposer* sPrimitiveTextureDisposer;
static PrimitiveTextureVtable sPrimitiveTextureMethods;
static sead::DisposerVtable sPrimitiveTextureDisposerMethods;
// the 16 texels of one row of the mip-level pattern
static const u32 sMipLevelColors[16] = {
    0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFF00FF, 0xFF00FFFF, 0x00FFFFFF, 0x7F0000FF, 0x007F00FF,
    0x00007FFF, 0x7F7F00FF, 0x7F007FFF, 0x007F7FFF, 0x3F0000FF, 0x003F00FF, 0x00003FFF, 0x000000FF,
};

/* 027A7640 */
PrimitiveTexture* PrimitiveTexture::create(PrimitiveTexture* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveTexture* texture = storage;
    if (texture == nullptr) texture = static_cast<PrimitiveTexture*>(allocateObject(sizeof(PrimitiveTexture)));
    if (texture == nullptr) return texture;
    texture->mMethods = &sPrimitiveTextureMethods;
    // GHS array construction of the two pointer arrays: tests the member addresses themselves
    if (gabi::ea(texture->mSamplers) == 0) allocateObject(sizeof(texture->mSamplers));
    if (gabi::ea(texture->mImages) == 0) allocateObject(sizeof(texture->mImages));
    for (u32 i = 0; i < PrimitiveTexture_Count; ++i) {
        texture->mSamplers[i] = nullptr;
        texture->mImages[i] = nullptr;
    }
    return texture;
}

/* 027A76C8 */
PrimitiveTexture* PrimitiveTexture::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveTexture* texture = sPrimitiveTexture;
    if (texture != nullptr) return texture;
    texture = static_cast<PrimitiveTexture*>(allocateInHeap(sizeof(PrimitiveTexture), heap, 4));
    // GHS tests the member address itself (also for a failed allocation).
    PrimitiveTextureDisposer* disposer = gabi::at<PrimitiveTextureDisposer>(gabi::ea(texture) + offsetof(PrimitiveTexture, mDisposer));
    if (disposer != nullptr) {
        registerInHeap(disposer, heap, 3);
        disposer->mVtable = &sPrimitiveTextureDisposerMethods;
    }
    sPrimitiveTextureDisposer = disposer;
    if (texture != nullptr) texture = create(texture);
    sPrimitiveTexture = texture;
    return texture;
}

/* 027A7768 */
void PrimitiveTexture::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sPrimitiveTextureMethods;
    for (u32 i = 0; i < PrimitiveTexture_Count; ++i) {
        releaseImage(mImages[i]);
        mImages[i] = nullptr;
        destroyArray(mSamplers[i], -1, sizeof(TextureSampler), gabi::guest_fn((const void*)destroySampler), 1, 0);
        mSamplers[i] = nullptr;
    }
    if (flags & 1) releaseObject(this);
}

/* 027A77F8 */
void PrimitiveTexture::initializeMipLevelTexture(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<TextureData> texture(0xC);
    TextureData::create(texture);
    texture->configure(TextureDimension_2D, PrimitiveTextureFormat_Color, 16, 8, 1, 1, 0, 0);
    void* image = allocateInHeap(texture->surface.mImageBytes, heap, texture->surface.mAlignment);
    mImages[PrimitiveTexture_MipLevel] = image;
    be<u32>* texel = static_cast<be<u32>*>(image);
    for (u32 row = 0; row < 8; ++row) {
        for (u32 column = 0; column < 16; ++column) *texel++ = sMipLevelColors[column];
    }
    prepareBitmapImage(texture, image, texture->surface.mImageBytes, PrimitiveTextureFormat_Color, 16, 8, nullptr);
    TextureSampler* sampler = static_cast<TextureSampler*>(allocateSampler(sizeof(TextureSampler), heap, 4));
    if (sampler != nullptr) sampler = createSampler(sampler, texture);
    mSamplers[PrimitiveTexture_MipLevel] = sampler;
}

/* inlined into initialize: the texel value of a single-colour or depth texture */
static u32 texelValue(u32 type) {
    switch (type) {
    case PrimitiveTexture_White2D: case PrimitiveTexture_WhiteCube: return 0xFFFFFFFF;
    case PrimitiveTexture_Gray2D: return 0x808080FF;
    case PrimitiveTexture_Clear2D: return 0;
    case PrimitiveTexture_Depth0: return 0x00000000;       // 0.0f
    case PrimitiveTexture_Depth1:
    case PrimitiveTexture_DepthShadow:
    case PrimitiveTexture_DepthShadowArray: return 0x3F800000;   // 1.0f
    default: return 0x000000FF;
    }
}

/* inlined into initialize: the sampler's wrap modes (clamp on all three axes) */
static void setClampWrap(TextureSampler* sampler) {
    sampler->settings.modes[3] = 2;
    sampler->settings.modes[5] = 2;
    sampler->dirty = sampler->dirty | 2;
    sampler->settings.modes[4] = 2;
}

/* inlined into initialize: depth compare on, then its comparison function */
static void setDepthCompare(TextureSampler* sampler) {
    sampler->settings.enabled = 1;
    sampler->dirty = sampler->dirty | 0x10;
}
static void setDepthCompareFunction(TextureSampler* sampler, u32 function) {
    sampler->settings.tail = function;
    sampler->settings.enabled = 1;
    sampler->dirty = sampler->dirty | 0x10;
}

/* 027A78F8 */
void PrimitiveTexture::initialize(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 type = 0; type < PrimitiveTexture_Count; ++type) {
        if (type == PrimitiveTexture_MipLevel) {
            initializeMipLevelTexture(heap);
            continue;
        }
        u32 format = type == PrimitiveTexture_Depth0 || type == PrimitiveTexture_Depth1 || type >= PrimitiveTexture_DepthShadow
            ? PrimitiveTextureFormat_Depth : PrimitiveTextureFormat_Color;
        u32 texel = texelValue(type);
        gabi::FrameLocal<TextureData> texture(0xC);
        TextureData::create(texture);
        switch (type) {
        case PrimitiveTexture_Black1D: texture->configure(TextureDimension_1D, format, 4, 1, 1, 1, 0, 0); break;
        case PrimitiveTexture_Black1DArray: texture->configure(TextureDimension_1DArray, format, 4, 4, 1, 1, 0, 0); break;
        case PrimitiveTexture_Black2DArray:
        case PrimitiveTexture_DepthShadowArray: texture->configure(TextureDimension_2DArray, format, 4, 4, 1, 1, 0, 0); break;
        case PrimitiveTexture_Black3D: texture->configure(TextureDimension_3D, format, 4, 4, 1, 1, 0, 0); break;
        case PrimitiveTexture_BlackCube:
        case PrimitiveTexture_WhiteCube:
        case PrimitiveTexture_BlackCubeArray: texture->configure(TextureDimension_Cube, format, 4, 4, 6, 1, 0, 0); break;
        default: texture->configure(TextureDimension_2D, format, 4, 4, 1, 1, 0, 0); break;
        }
        mImages[type] = allocateInHeap(texture->surface.mImageBytes, heap, texture->surface.mAlignment);
        be<u32>* image = static_cast<be<u32>*>(mImages[type].get());
        for (u32 i = 0; i < texture->surface.mImageBytes / 4; ++i) image[i] = texel;
        texture->surface.mImage = image;
        flushRange(image, texture->surface.mImageBytes);
        TextureSampler* sampler = static_cast<TextureSampler*>(allocateSampler(sizeof(TextureSampler), heap, 4));
        if (sampler != nullptr) sampler = createSampler(sampler, texture);
        mSamplers[type] = sampler;
        if (type >= PrimitiveTexture_DepthShadow && type <= PrimitiveTexture_DepthShadowArray) {
            setClampWrap(mSamplers[type]);
            setDepthCompare(mSamplers[type]);
            setDepthCompareFunction(mSamplers[type], 7);
        }
        if (mSamplers[type]->dirty != 0) updateSampler(mSamplers[type]);
    }
}

/* 027A7DAC */
void PrimitiveTextureDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sPrimitiveTextureDisposerMethods;
    if (this == sPrimitiveTextureDisposer) {
        sPrimitiveTextureDisposer = nullptr;
        PrimitiveTexture* texture = sPrimitiveTexture;
        gabi::call_ptr<void>(texture->mMethods->mDestroy.mFunction, texture, 2);
        sPrimitiveTexture = nullptr;
    }
    destroyRegistered(this, 0);
    if (flags & 1) releaseObject(this);
}

// HD: compiler-generated and template code of this translation unit

/* 027A7E54 */
void PrimitiveTextureInterface::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) releaseObject(this);
}

/* 027A7E68 */
void PrimitiveTextureInterface::emptyHook() {}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object)
ext 0273AFC8 void releaseImage(void* image)
ext 0273B0D4 void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment)
ext 0273B050 void* allocateSampler(u32 size, sead::RegistryHeap* heap, s32 alignment)
ext 02752B0C void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection)
ext 02752BEC void destroyRegistered(sead::RegisteredObject* object, u32 flags)
ext 027BE6B8 agl::TextureData* agl::TextureData::create(TextureData* object)
ext 027BE930 void agl::TextureData::configure(u32 dimension, u32 format, u32 width, u32 height, u32 depth, s32 mipCount, u32 tileMode, u32 antiAlias)
ext 027B5EEC void agl::prepareBitmapImage(void* target, void* pixels, u32 bytes, u32 format, u32 width, u32 height, sead::RegistryHeap* heap)
ext 027BE114 agl::TextureSampler* createSampler(TextureSampler* sampler, const TextureData* texture)
ext 027BE494 void updateSampler(TextureSampler* sampler)
ext 027BE2B0 void destroySampler(TextureSampler* sampler, u32 flags) callback
ext 028F0164 void destroyArray(void* objects, s32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext C00088A0 void flushRange(void* data, u32 bytes)
data sPrimitiveTexture 101F8DEC
data sPrimitiveTextureDisposer 101F8DF0
data sPrimitiveTextureMethods 10168254
data sPrimitiveTextureDisposerMethods 10168244
data sMipLevelColors 101681D4
*/
