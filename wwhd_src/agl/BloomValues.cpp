// Bloom luminance initialization reconstructed from the game's constant loads.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "agl/DepthBlurContext.h"
#include "agl/PrimitiveShapes.h"
#include "port_native_draw.h"

static agl::depth_blur_value::Vector4 sBloomLuminanceWeights;
struct BloomPassState {
    be<u8> mEnabled;
    u8 mPadding[3];
    agl::RenderBuffer mOutput;
    agl::ColorSurface mInputColor;
    agl::TextureSampler mTemporary;
    u8 mOtherState[0x4AC];
};
using BloomTexture = agl::TextureSampler;
struct BloomRuntime {
    u8 mPrefix[0x1CC];
    u8 mColorState[0x2DC];
    be<u32> mDrawMode;
    u8 mOtherMode[12];
    be<u8> mEnabled;
    u8 mOtherEnabled[15];
    be<f32> mStrength;
    u8 mOtherParameters[0xB4];
    be<u8> mForceViewport;
    u8 mOtherViewport[3];
    be<u32> mPassCount;
    gptr<BloomPassState> mPassStates;
};
static sead::RenderVector2 sBloomCopyScale;
static sead::RenderVector2 sBloomCopyOffset;
static void applyBloomColorState(void* state);
static void invalidateBloomTexture(u32 flags, void* image, u32 size);
static u32 drawPrimaryBloom(BloomRuntime* bloom, u32 pass, void* viewport, void* bounds,
    BloomTexture* texture, u32 first, u32 second, u32 third, u32 slot);
static u32 drawSecondaryBloom(BloomRuntime* bloom, u32 pass, void* viewport, void* bounds,
    BloomTexture* texture, u32 slot);
static u32 copyBloomTexture(BloomTexture* texture, void* bounds, sead::RenderVector2* scale, sead::RenderVector2* offset, u32 slot);
static TextureCachePointer sBloomTextureCache; // dynamic texture cache singleton
static ShaderHolderPointer sBloomShaderHolder; // shader program holder singleton
using ShapeInputsPointer = gptr<agl::PrimitiveShapeInputs>;
using ShapesPointer = gptr<agl::PrimitiveShapes>;
static ShapeInputsPointer sBloomShapeInputs; // primitive shape vertex inputs singleton
static ShapesPointer sBloomShapes; // primitive shapes singleton (quad index buffer)
// Text bytes of the temporary texture's name (guest string bound by the data line).
typedef be<u8> TextByte;
static TextByte sBloomTemporaryName;
static sead::SafeTextVtable sBloomStringMethods;
static void calculateBloomExtent(void* viewport, sead::RenderVector2* output, const sead::RenderSurface* surface);
static agl::TextureCacheEntry* acquireBloomSurface(agl::DynamicTextureCache* cache, sead::SafeText* name, u32 format,
    u32 width, u32 height, u32 mips, u32 first, u32 second, u32 third, u32 fourth);
static void assignBloomSurface(agl::TextureSampler* destination, const agl::TextureData* source);
static void bindBloomSourceSurface(agl::ColorSurface* color, const agl::TextureData* texture);
static void applyBloomSurface(sead::RenderSurface* surface);
static sead::Viewport* constructBloomViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyBloomViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyBloomViewportOnly(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyBloomScissor(void* viewport, const sead::RenderSurface* surface);
static void clearBloomSurface(sead::RenderSurface* surface, u32 mask);
static void prepareBloomProgram(agl::ShaderProgram* program, u32 flags);
static void selectBloomShaderMode(u32 mode);
static void callBloomDisplayList(void* list, u32 size);
static void activateBloomProgram(agl::ShaderProgram* program);
static void setBloomVertexUniform(s32 location, u32 count, const void* values);
static void setBloomPixelUniform(s32 location, u32 count, const void* values);
static void bindBloomSampler(agl::TextureSampler* sampler, agl::ProgramResource16_l* location, s32 slot, u32 flags);
static void activateBloomVertices(agl::VertexInputSet* inputs);
static void drawBloomIndices(u32 primitive, u32 count, u32 indexType, void* indices, s32 vertexOffset, u32 instances);
static void releaseBloomSurface(agl::DynamicTextureCache* cache, agl::TextureCacheEntry* texture);

static u32 bloomUnsignedExtent(f32 value, be<u32>* scratch) {
    constexpr f32 limit = 2147483648.0f;
    if (!(value < limit)) {
        const u32 truncated = u32(gabi::ftoi(gabi::fsubs_ppc(value, limit)));
        *scratch = truncated;
        return truncated + 0x80000000;
    }
    const u32 truncated = u32(gabi::ftoi(value));
    *scratch = truncated;
    return truncated;
}
static u32 bloomProgramAddress(u32 index) {
    agl::detail::ShaderHolder* holder = sBloomShaderHolder;
    if (u32(holder->mPrograms.mCount) > index) return gabi::ea(holder->programSlots()[index].get());
    return 0;
}
// Program lookup may return guest address zero; raw guest access preserves its page-zero behavior.
template<class T> static T bloomProgramField(u32 program, u32 offset) { return gabi::load<T>(program + offset); }
static u32 useBloomProgram(u32 program, u32 currentMode) {
    const u8 flags = bloomProgramField<u8>(program, offsetof(agl::ShaderProgram, flags));
    if (flags & 2) {
        gabi::store<u8>(program + offsetof(agl::ShaderProgram, flags), flags & ~2);
        prepareBloomProgram(gabi::at<agl::ShaderProgram>(program), 0);
    }
    agl::ShaderProgramSharedData* shared = gabi::at<agl::ShaderProgramSharedData>(bloomProgramField<u32>(program, offsetof(agl::ShaderProgram, binary)));
    const u32 mode = shared->mode;
    if (currentMode != mode) selectBloomShaderMode(mode);
    const u32 listSize = bloomProgramField<u32>(program, kProgramDisplayListSize);
    if (listSize) callBloomDisplayList(gabi::at<void>(bloomProgramField<u32>(program, kProgramDisplayListData)), listSize);
    else activateBloomProgram(gabi::at<agl::ShaderProgram>(program));
    return mode;
}

/* 0277BF94 */
static u32 drawSecondaryBloom(BloomRuntime* bloom, u32 pass, void* surfaceArgument, void* viewport,
    BloomTexture* texture, u32 unusedSlot) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::RenderSurface* surface = static_cast<sead::RenderSurface*>(surfaceArgument);
    BloomPassState* state = bloom->mPassStates;
    if (pass < bloom->mPassCount) state += pass;
    gabi::FrameLocal<sead::RenderVector2> extent(0x28);
    calculateBloomExtent(viewport, extent, surface);
    gabi::FrameLocal<be<u32>> scratch(0x10);
    gabi::FrameLocal<sead::SafeText> name(0x40);
    name->mText = &sBloomTemporaryName;
    name->mVtable = &sBloomStringMethods;
    const u32 width = bloomUnsignedExtent(gabi::fmuls_ppc(extent->x, 0.5f), scratch);
    const u32 height = bloomUnsignedExtent(gabi::fmuls_ppc(extent->y, 0.5f), scratch);
    agl::TextureCacheEntry* temporary = acquireBloomSurface(sBloomTextureCache, name, 0x1D, width, height, 1, 0, 0, 1, 0);
    if (contextSurfaceMatches(&state->mTemporary, temporary)) {
        void* image = temporary->surface.mImage;
        void* mipmaps = temporary->surface.mMipImage;
        state->mTemporary.texture.surface.mImage = image;
        state->mTemporary.texture.surface.mMipImage = mipmaps;
        nativeSurface(&state->mTemporary)->mImage = image;
        nativeSurface(&state->mTemporary)->mMipImage = mipmaps;
    } else assignBloomSurface(&state->mTemporary, temporary);
    state->mTemporary.overrideEnabled = 1;
    bindBloomSourceSurface(&state->mInputColor, temporary);
    const f32 halfWidth = gabi::fmuls_ppc(extent->x, 0.5f);
    const f32 halfHeight = gabi::fmuls_ppc(extent->y, 0.5f);
    gabi::FrameLocal<agl::depth_blur_value::Vector4> bounds(0x18);
    if (0.0f < halfWidth) { bounds->z = halfWidth; bounds->x = 0.0f; }
    else { bounds->z = 0.0f; bounds->x = halfWidth; }
    if (0.0f < halfHeight) { bounds->y = 0.0f; bounds->w = halfHeight; }
    else { bounds->y = halfHeight; bounds->w = 0.0f; }
    state->mOutput.mBase.mArea.mMinX = bounds->x;
    state->mOutput.mBase.mArea.mMinY = bounds->y;
    state->mOutput.mBase.mArea.mMaxX = bounds->z;
    state->mOutput.mBase.mArea.mMaxY = bounds->w;
    gabi::FrameLocal<sead::RenderVector2> scaled(0x38), copied(0x48);
    scaled->y = gabi::fmuls_ppc(extent->y, 0.5f);
    scaled->x = gabi::fmuls_ppc(extent->x, 0.5f);
    copied->x = scaled->x;
    copied->y = scaled->y;
    state->mOutput.mBase.mSize.mWidth = copied->x;
    state->mOutput.mBase.mSize.mHeight = copied->y;
    state->mOutput.mColors[0] = &state->mInputColor;
    applyBloomSurface(frameSurface(&state->mOutput));
    gabi::FrameLocal<sead::Viewport> firstViewport(0x50), secondViewport(0x68);
    sead::Viewport* calculated = constructBloomViewport(firstViewport, frameSurface(&state->mOutput));
    applyBloomViewport(calculated, frameSurface(&state->mOutput));
    u32 mode = copyBloomTexture(texture, viewport, &sBloomCopyScale, &sBloomCopyOffset, 3);
    if (bloom->mForceViewport) clearBloomSurface(surface, 0xFF);
    else applyBloomSurface(surface);
    calculated = constructBloomViewport(secondViewport, frameSurface(&state->mOutput));
    applyBloomViewportOnly(calculated, surface);
    applyBloomScissor(viewport, surface);
    s32 imageWidth = texture->texture.minimumWidth;
    if (s32(texture->texture.surface.mWidth) > imageWidth) imageWidth = s32(texture->texture.surface.mWidth);
    struct UnsignedBias { be<u32> exponent, value; };
    gabi::FrameLocal<UnsignedBias> unsignedBias(0x10);
    unsignedBias->exponent = 0x43300000;
    unsignedBias->value = u32(imageWidth);
    s32 imageHeight = texture->texture.minimumHeight;
    if (s32(texture->texture.surface.mHeight) > imageHeight) imageHeight = s32(texture->texture.surface.mHeight);
    // The uniform's last two words are the half extent already stored at frame 0x38.
    gabi::FrameLocal<agl::depth_blur_value::Vector4> texelSize(0x30);
    texelSize->x = gabi::fdivs_ppc(1.0f, f32(u32(imageWidth)));
    unsignedBias->value = u32(imageHeight);
    texelSize->y = gabi::fdivs_ppc(1.0f, f32(u32(imageHeight)));
    const u32 program = bloomProgramAddress(56);
    mode = useBloomProgram(program, mode);
    UniformLocations_l* uniform = gabi::at<UniformLocations_l>(bloomProgramField<u32>(program, offsetof(agl::ShaderProgram, uniforms)));
    if (uniform->mVertexRegister != -1) setBloomVertexUniform(uniform->mVertexRegister, 4, texelSize.get());
    if (uniform->mPixelRegister != -1) setBloomPixelUniform(uniform->mPixelRegister, 4, texelSize.get());
    auto* samplers = gabi::at<agl::ProgramResource16_l>(bloomProgramField<u32>(program, offsetof(agl::ShaderProgram, samplers)));
    bindBloomSampler(texture, samplers, -1, 0);
    samplers = gabi::at<agl::ProgramResource16_l>(bloomProgramField<u32>(program, offsetof(agl::ShaderProgram, samplers)));
    if (bloomProgramField<u32>(program, offsetof(agl::ShaderProgram, samplerCount)) > 1) ++samplers;
    bindBloomSampler(&state->mTemporary, samplers, -1, 0);
    activateBloomVertices(&sBloomShapeInputs->mInputs[6]);
    agl::IndexBufferLayout* indices = &sBloomShapes->mIndexedShapes[0].mIndexBuffer;
    const u32 count = indices->mIndexCount;
#if PORT_NATIVE_DRAW
    // the bloom value pass's draw as a packet of its own (port_native_draw.h)
    if (count && port_native_draw_single(PORT_NATIVE_POST, PORT_NATIVE_POST_BLOOM_VALUES, gabi::ea(state), program,
                                         gabi::ea(texture), indices->mPrimitiveMode, count, indices->mFormat,
                                         gabi::ea(static_cast<const void*>(indices->mStorage)), 0, 1)) {
        releaseBloomSurface(sBloomTextureCache, temporary);
        return mode;
    }
#endif
    if (count) drawBloomIndices(indices->mPrimitiveMode, count, indices->mFormat, indices->mStorage, 0, 1);
    releaseBloomSurface(sBloomTextureCache, temporary);
    return mode;
}

/* 0277C578 */
static u32 drawBloom(BloomRuntime* bloom, u32 pass, void* viewport, void* bounds,
    BloomTexture* texture, u32 first, u32 second, u32 third, u32 slot) {
    gabi::AutoFrame frame{gabi::current_function()};
    applyBloomColorState(bloom->mColorState);
    invalidateBloomTexture(16, texture->texture.surface.mImage, texture->texture.surface.mImageBytes);
    if (bloom->mEnabled) {
        BloomPassState* state = bloom->mPassStates;
        if (pass < bloom->mPassCount) state += pass;
        if (state->mEnabled && bloom->mStrength > 0.0f) {
            const u32 mode = bloom->mDrawMode;
            if (mode == 0)
                return drawPrimaryBloom(bloom, pass, viewport, bounds, texture, first, second, third, slot);
            if (mode == 1)
                return drawSecondaryBloom(bloom, pass, viewport, bounds, texture, slot);
            return slot;
        }
    }
    return copyBloomTexture(texture, bounds, &sBloomCopyScale, &sBloomCopyOffset, slot);
}

/* 0277C6AC */
static void initializeBloomLuminanceWeights() {
    gabi::AutoFrame frame{gabi::current_function()};
    sBloomLuminanceWeights.x = 0.299f;
    sBloomLuminanceWeights.y = 0.587f;
    sBloomLuminanceWeights.z = 0.114f;
    sBloomLuminanceWeights.w = 0.0f;
}

WWHD_SIZE(BloomPassState, 0x7D0);
WWHD_OFFSET(BloomPassState, mOutput, 4);
WWHD_OFFSET(BloomPassState, mInputColor, 0x44);
WWHD_OFFSET(BloomPassState, mTemporary, 0x18C);
WWHD_OFFSET(BloomRuntime, mDrawMode, 0x4A8);
WWHD_OFFSET(BloomRuntime, mForceViewport, 0x580);
WWHD_OFFSET(BloomRuntime, mPassCount, 0x584);

/* WWHD glue
 data sBloomLuminanceWeights 104A1F80
 data sBloomCopyScale 104A0920
 data sBloomCopyOffset 104A0908
 ext 0274FEB0 void applyBloomColorState(void* state)
 ext C00064A8 void invalidateBloomTexture(u32 flags, void* image, u32 size)
 ext 0277AD60 u32 drawPrimaryBloom(BloomRuntime* bloom, u32 pass, void* viewport, void* bounds, BloomTexture* texture, u32 first, u32 second, u32 third, u32 slot)
 ext 027A90AC u32 copyBloomTexture(BloomTexture* texture, void* bounds, sead::RenderVector2* scale, sead::RenderVector2* offset, u32 slot)
 data sBloomTextureCache 101F8DD4
 data sBloomShaderHolder 101F8D6C
 data sBloomShapeInputs 101F8DDC
 data sBloomShapes 101F8DE4
 data sBloomTemporaryName 10162568 nocheck
 data sBloomStringMethods 10161F1C nocheck
 ext 0274F8D8 void calculateBloomExtent(void* viewport, sead::RenderVector2* output, const sead::RenderSurface* surface)
 ext 0279FC6C agl::TextureCacheEntry* acquireBloomSurface(agl::DynamicTextureCache* cache, sead::SafeText* name, u32 format, u32 width, u32 height, u32 mips, u32 first, u32 second, u32 third, u32 fourth)
 ext 027BDEB4 void assignBloomSurface(agl::TextureSampler* destination, const agl::TextureData* source)
 ext 025C0928 void bindBloomSourceSurface(agl::ColorSurface* color, const agl::TextureData* texture)
 ext 0274D690 void applyBloomSurface(sead::RenderSurface* surface)
 ext 0274F5F0 sead::Viewport* constructBloomViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274F964 void applyBloomViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274FA00 void applyBloomViewportOnly(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274FA70 void applyBloomScissor(void* viewport, const sead::RenderSurface* surface)
 ext 027B9D00 void clearBloomSurface(sead::RenderSurface* surface, u32 mask)
 ext 027BB9E0 void prepareBloomProgram(agl::ShaderProgram* program, u32 flags)
 ext 027B9F68 void selectBloomShaderMode(u32 mode)
 ext C00060E0 void callBloomDisplayList(void* list, u32 size)
 ext 027BB7CC void activateBloomProgram(agl::ShaderProgram* program)
 ext C0006A40 void setBloomVertexUniform(s32 location, u32 count, const void* values)
 ext C0006908 void setBloomPixelUniform(s32 location, u32 count, const void* values)
 ext 027BE53C void bindBloomSampler(agl::TextureSampler* sampler, agl::ProgramResource16_l* location, s32 slot, u32 flags)
 ext 027BFE5C void activateBloomVertices(agl::VertexInputSet* inputs)
 ext C0006178 void drawBloomIndices(u32 primitive, u32 count, u32 indexType, void* indices, s32 vertexOffset, u32 instances)
 ext 027A0040 void releaseBloomSurface(agl::DynamicTextureCache* cache, agl::TextureCacheEntry* texture)
*/
