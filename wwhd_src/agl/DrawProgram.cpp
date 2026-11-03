// WWHD draw-program selection from this unit's own instructions.
#include "agl/DrawProgram.h"
#include "sead/ElementAt.h"
#include "agl/QuaternionOps.h"
#include "sead/RenderMath.h"
#include "sead/HeapRegistry.h"
#include "agl/EntryParameters.h"
#include "agl/EmbeddedEntry.h"
#include "sead/Viewport.h"
#include "sead/RenderState.h"
#include "sead/FileDevice.h"
using ShaderHolderPointer = gptr<agl::detail::ShaderHolder>;
// The shader holder singleton (101F8D6C; its one definition is agl/TimingStatics.cpp's sShaderHolder, which
// AGL#12's specs do not link): a borrowed per-TU binding, never initialized here.
static ShaderHolderPointer sDrawShaderHolder;
/* 027A9408 */
static agl::ShaderProgram* selectIndexedDrawProgram(s32 index) {
    agl::detail::ShaderHolder* holder = sDrawShaderHolder;
    agl::ShaderProgram* record = NULL;
    if (u32(holder->mPrograms.mCount) > 12) record = holder->programSlots()[12];
    agl::ShaderProgramSharedData* data = programSharedData(record);
    if (index <= 0) return data->variation.root;
    u32 variantIndex = u32(index) - 1;
    agl::ShaderProgram* variants = data->variation.programs;
    if (variantIndex >= u32(data->variation.programCount)) return variants;
    return variants + variantIndex;
}
void finishDrawPrimitive();
void prepareDrawProgram(agl::ShaderProgram*, u32);
void applyDrawMode(u32);
void callDrawCommands(void*, u32);
void selectDrawProgram(agl::ShaderProgram*);
void setDrawVertexUniform(s32, u32, const void*);
void setDrawPixelUniform(s32, u32, const void*);
void drawBoundQuad(agl::ShaderProgram*, agl::TextureSampler*, const sead::ViewportBounds*, const sead::RenderVector2*, const sead::RenderVector2*);
void drawProgramTexture(agl::ShaderProgram*, agl::TextureSampler*);
static f32 sDrawUnit;
static sead::RenderVector2 sDefaultDrawScale;
struct TextureSelectionTable { be<u8> mValues[4]; };
static TextureSelectionTable sTextureSelection;

static agl::ShaderProgram* getRegistryProgram(u32 index) {
    agl::detail::ShaderHolder* holder = sDrawShaderHolder;
    if (index < u32(holder->mPrograms.mCount)) return holder->programSlots()[index];
    return NULL;
}
static u32 bindDrawProgram(agl::ShaderProgram* program, u32 current) {
    u32 flags = program->flags;
    if (flags & 2) {
        program->flags = flags & ~2u;
        prepareDrawProgram(program, 0);
    }
    u32 activeMode = programSharedData(program)->mode;
    if (current != activeMode) applyDrawMode(activeMode);
    u32 size = programDisplayList(program)->mUsed;
    if (size != 0) callDrawCommands(programDisplayList(program)->mData, size);
    else selectDrawProgram(program);
    return activeMode;
}
static UniformLocations_l* getDrawUniform(agl::ShaderProgram* program, u32 index) {
    u32 count = program->uniformCount;
    UniformLocations_l* first = programUniforms(program);
    return first + (count > index ? index : 0);
}
static void sendDrawUniform(UniformLocations_l* locations, u32 size, const void* values) {
    s32 vertex = locations->mVertexRegister;
    if (vertex != -1) setDrawVertexUniform(vertex, size, values);
    s32 pixel = locations->mPixelRegister;
    if (pixel != -1) setDrawPixelUniform(pixel, size, values);
}

/* 027A90AC */
static u32 drawDefaultQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds,
        const sead::RenderVector2* scale, const sead::RenderVector2* position, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgram* program = programSharedData(getRegistryProgram(2))->variation.root;
    u32 activeMode = bindDrawProgram(program, current);
    drawBoundQuad(program, texture, bounds, scale, position);
    return activeMode;
}
/* 027A919C */
static u32 drawScaledQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds,
        const sead::RenderVector2* scale, const sead::RenderVector2* position, u32 current, f64 amount) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgramSharedData* data = programSharedData(getRegistryProgram(2));
    u32 count = data->variation.programCount;
    agl::ShaderProgram* program = data->variation.programs;
    if (count > 1) ++program;
    u32 activeMode = bindDrawProgram(program, current);
    UniformLocations_l* locations = getDrawUniform(program, 2);
    gabi::FrameLocal<be<f32>> value(0x8);
    *value.get() = gabi::f32_from_bits(ppc_stfs_bits(amount));
    sendDrawUniform(locations, 4, value.get());
    drawBoundQuad(program, texture, bounds, scale, position);
    return activeMode;
}
/* 027A9304 */
static u32 drawSelectedQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds,
        const sead::RenderVector2* scale, const sead::RenderVector2* position, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 selection = texture->texture.surface.mSamples;
    u32 index = 2;
    if (selection <= 3) index = sTextureSelection.mValues[selection];
    agl::ShaderProgram* program = getRegistryProgram(index);
    u32 activeMode = bindDrawProgram(program, current);
    drawBoundQuad(program, texture, bounds, scale, position);
    return activeMode;
}
/* 027A9468 */
static u32 drawIndexedQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds, s32 index,
        const void* values, const sead::RenderVector2* scale, const sead::RenderVector2* position, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgram* program = selectIndexedDrawProgram(index);
    u32 activeMode = bindDrawProgram(program, current);
    sendDrawUniform(getDrawUniform(program, 17), 4, values);
    drawBoundQuad(program, texture, bounds, scale, position);
    return activeMode;
}
/* 027A9584 */
static u32 drawCombinationQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds,
        u32 first, u32 second, u32 third, const sead::RenderVector2* position, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgramSharedData* data = programSharedData(getRegistryProgram(10));
    s32 selection = s32((first << 2) + (second << 1) + third);
    agl::ShaderProgram* program;
    if (selection <= 0) program = data->variation.root;
    else {
        u32 index = u32(selection) - 1;
        program = data->variation.programs;
        if (index < u32(data->variation.programCount)) program += index;
    }
    u32 activeMode = bindDrawProgram(program, current);
    drawBoundQuad(program, texture, bounds, &sDefaultDrawScale, position);
    return activeMode;
}
/* 027A96C0 */
static u32 drawTripleUniformQuad(agl::TextureSampler* texture, const sead::ViewportBounds* bounds,
        const void* first, const void* second, const void* third,
        const sead::RenderVector2* scale, const sead::RenderVector2* position, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgram* program = getRegistryProgram(32);
    u32 activeMode = bindDrawProgram(program, current);
    sendDrawUniform(getDrawUniform(program, 11), 4, first);
    sendDrawUniform(getDrawUniform(program, 12), 4, second);
    sendDrawUniform(getDrawUniform(program, 13), 4, third);
    drawBoundQuad(program, texture, bounds, scale, position);
    return activeMode;
}
/* 027A9A24 */
static u32 drawDefaultTexture(agl::TextureSampler* texture, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgram* program = programSharedData(getRegistryProgram(2))->variation.programs;
    u32 activeMode = bindDrawProgram(program, current);
    drawProgramTexture(program, texture);
    return activeMode;
}
/* 027A9B0C */
static u32 drawScaledTexture(agl::TextureSampler* texture, u32 current, f64 amount) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (amount == f64(sDrawUnit)) return drawDefaultTexture(texture, current);
    agl::ShaderProgram* program = programSharedData(getRegistryProgram(36))->variation.programs;
    u32 activeMode = bindDrawProgram(program, current);
    UniformLocations_l* locations = getDrawUniform(program, 22);
    gabi::FrameLocal<be<f32>> value(0x8);
    *value.get() = gabi::f32_from_bits(ppc_stfs_bits(amount));
    sendDrawUniform(locations, 4, value.get());
    drawProgramTexture(program, texture);
    return activeMode;
}
using ShapeInputsPointer = gptr<agl::PrimitiveShapeInputs>;
using ShapesPointer = gptr<agl::PrimitiveShapes>;
// The primitive-shape singletons (101F8DDC / 101F8DE4; one definition each in agl/PrimitiveShapesStatics.cpp,
// which AGL#12's specs do not link): borrowed per-TU bindings, without initializers.
static ShapeInputsPointer sDrawShapeInputs;
static ShapesPointer sDrawShapes;
static sead::ColorChannels sDefaultColor;
void bindDrawGeometry(agl::VertexInputSet*);
void bindDrawTexture(agl::TextureSampler*, void*, u32, u32);
void drawIndexedPacket(u32, u32, u32, const void*, s32, u32);

static void issueDrawPacket(const agl::IndexBufferLayout* packet) {
    u32 count = packet->mIndexCount;
    if (count != 0) {
        u32 indexType = packet->mFormat;
        const void* indices = packet->mStorage;
        u32 primitive = packet->mPrimitiveMode;
        drawIndexedPacket(primitive, count, indexType, indices, 0, 1);
    }
}
/* 027A9D24 */
static u32 drawTransformedPrimitive(const sead::RegistryMatrix34* model,
        const sead::RegistryMatrix44* projection, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RegistryMatrix44> matrix(0x8);
    combineRegistryMatrices(matrix.get(), projection, model);
    agl::ShaderProgram* program = getRegistryProgram(37);
    u32 activeMode = bindDrawProgram(program, current);
    sendDrawUniform(programUniforms(program), 16, matrix.get());
    finishDrawPrimitive();
    return activeMode;
}
/* 027AAABC */
static u32 drawColoredPrimitive(const sead::ColorChannels* color, const sead::RegistryMatrix34* model,
        const sead::RegistryMatrix44* projection, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ShaderProgram* program = programSharedData(getRegistryProgram(33))->variation.root;
    u32 activeMode = bindDrawProgram(program, current);
    gabi::FrameLocal<sead::RegistryMatrix44> matrix(0x8);
    combineRegistryMatrices(matrix.get(), projection, model);
    sendDrawUniform(programUniforms(program), 16, matrix.get());
    sendDrawUniform(getDrawUniform(program, 8), 4, color);
    bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawQuadInput]);
    issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer);
    return activeMode;
}
/* 027AAC50 */
static void drawMatrixQuad(agl::ShaderProgram* program, agl::TextureSampler* texture,
        const sead::RegistryMatrix34* model, const sead::RegistryMatrix44* projection, const sead::ColorChannels* color) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = program->flags;
    if (flags & 2) {
        program->flags = flags & ~2u;
        prepareDrawProgram(program, 0);
    }
    bindDrawTexture(texture, program->samplers, 0xFFFFFFFF, 0);
    gabi::FrameLocal<sead::RegistryMatrix44> matrix(0x8);
    combineRegistryMatrices(matrix.get(), projection, model);
    sendDrawUniform(programUniforms(program), 16, matrix.get());
    sendDrawUniform(getDrawUniform(program, 8), 4, color);
    bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawQuadInput]);
    issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer);
}
/* 027AAD8C */
static u32 drawColorSelectedQuad(agl::TextureSampler* texture, const sead::RegistryMatrix34* model,
        const sead::RegistryMatrix44* projection, const sead::ColorChannels* color, u32 current) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::detail::ShaderHolder* holder = sDrawShaderHolder;
    u32 index = color->equals(&sDefaultColor) != 0 ? 2 : 13;
    agl::ShaderProgram* program = NULL;
    if (index < u32(holder->mPrograms.mCount)) program = holder->programSlots()[index];
    u32 activeMode = bindDrawProgram(program, current);
    drawMatrixQuad(program, texture, model, projection, color);
    return activeMode;
}
/* 027A9888 */
void drawProgramTexture(agl::ShaderProgram* program, agl::TextureSampler* texture) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 width = texture->texture.surface.mWidth;
    u32 minimumWidth = texture->texture.minimumWidth;
    if (width > s32(minimumWidth)) minimumWidth = u32(width);
    u32 minimumHeight = texture->texture.minimumHeight;
    s32 height = texture->texture.surface.mHeight;
    if (height > s32(minimumHeight)) minimumHeight = u32(height);
    f32 widthValue = f32(minimumWidth);
    f32 heightValue = f32(minimumHeight);
    f32 unit = sDrawUnit;
    gabi::FrameLocal<agl::DrawFloat4> dimensions(0x10);
    dimensions->mValues[2] = gabi::fdivs_ppc(unit, widthValue);
    dimensions->mValues[3] = gabi::fdivs_ppc(unit, heightValue);
    dimensions->mValues[0] = widthValue;
    dimensions->mValues[1] = heightValue;
    sendDrawUniform(getDrawUniform(program, 1), 4, dimensions.get());
    u32 flags = program->flags;
    if (flags & 2) {
        program->flags = flags & ~2u;
        prepareDrawProgram(program, 0);
    }
    bindDrawTexture(texture, program->samplers, 0xFFFFFFFF, 0);
    bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawFullscreenInput]);
    issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawFullscreenShape].mIndexBuffer);
}
static f32 sDrawZero;
static f32 sDrawHalf;
sead::ProjectionState* createDrawOrthographicProjection(sead::ProjectionState*, const sead::ViewportBounds*, f32, f32);
void destroyDrawOrthographicProjection(sead::ProjectionState*, u32);
void makeDrawScaleTranslation(sead::ViewTransform*, const sead::ViewVector*, const sead::ViewVector*);
void combineDrawMatrices(sead::ProjectionMatrix*, const sead::ProjectionMatrix*, const sead::ViewTransform*);
/* 027A8D08 */
void drawBoundQuad(agl::ShaderProgram* program, agl::TextureSampler* texture,
        const sead::ViewportBounds* bounds, const sead::RenderVector2* scale, const sead::RenderVector2* position) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 width = texture->texture.surface.mWidth;
    u32 selectedWidth = texture->texture.minimumWidth;
    if (width > s32(selectedWidth)) selectedWidth = u32(width);
    u32 selectedHeight = texture->texture.minimumHeight;
    s32 height = texture->texture.surface.mHeight;
    if (height > s32(selectedHeight)) selectedHeight = u32(height);
    f32 unit = sDrawUnit;
    f32 widthValue = f32(selectedWidth);
    f32 heightValue = f32(selectedHeight);
    gabi::FrameLocal<agl::DrawFloat4> dimensions(0x10);
    dimensions->mValues[0] = widthValue;
    dimensions->mValues[2] = gabi::fdivs_ppc(unit, widthValue);
    dimensions->mValues[3] = gabi::fdivs_ppc(unit, heightValue);
    dimensions->mValues[1] = heightValue;
    sendDrawUniform(getDrawUniform(program, 1), 4, dimensions.get());
    f32 zero = sDrawZero;
    gabi::FrameLocal<sead::ProjectionState> projection(0xD8);
    createDrawOrthographicProjection(projection.get(), bounds, zero, unit);
    f32 left = bounds->mLeft.value;
    f32 right = bounds->mRight.value;
    f32 bottom = bounds->mBottom.value;
    f32 top = bounds->mTop.value;
    f32 half = sDrawHalf;
    f32 centerX = gabi::fmuls_ppc(gabi::fadds_ppc(left, right), half);
    f32 centerY = gabi::fmuls_ppc(gabi::fadds_ppc(top, bottom), half);
    f32 halfWidth = gabi::fsubs_ppc(centerX, left);
    f32 positionY = position->y;
    f32 halfHeight = gabi::fsubs_ppc(centerY, top);
    f32 positionX = position->x;
    f32 offsetX = gabi::fsubs_ppc(positionX, halfWidth);
    f32 offsetY = gabi::fsubs_ppc(positionY, halfHeight);
    selectedWidth = texture->texture.minimumWidth;
    width = texture->texture.surface.mWidth;
    if (width > s32(selectedWidth)) selectedWidth = u32(width);
    selectedHeight = texture->texture.minimumHeight;
    height = texture->texture.surface.mHeight;
    if (height > s32(selectedHeight)) selectedHeight = u32(height);
    widthValue = f32(selectedWidth);
    heightValue = f32(selectedHeight);
    f32 scaleX = scale->x;
    f32 scaleY = scale->y;
    f32 scaledWidth = gabi::fmuls_ppc(scaleX, widthValue);
    f32 scaledHeight = gabi::fmuls_ppc(scaleY, heightValue);
    gabi::FrameLocal<sead::ViewVector> scaledSize(0x38);
    gabi::FrameLocal<sead::ViewVector> translation(0x44);
    scaledSize->x.value = scaledWidth;
    scaledSize->y.value = scaledHeight;
    translation->z.value = zero;
    scaledSize->z.value = zero;
    f32 translatedY = gabi::fmadds(scaledHeight, half, offsetY);
    f32 translatedX = gabi::fmadds(scaledWidth, half, offsetX);
    translation->x.value = translatedX;
    translation->y.value = -translatedY;
    gabi::FrameLocal<sead::ViewTransform> model(0xA8);
    makeDrawScaleTranslation(model.get(), scaledSize.get(), translation.get());
    sead::ProjectionMatrix* projectionMatrix = projection->deviceProjection();
    gabi::FrameLocal<sead::ProjectionMatrix> matrix(0x68);
    combineDrawMatrices(matrix.get(), projectionMatrix, model.get());
    u32 flags = program->flags;
    if (flags & 2) {
        program->flags = flags & ~2u;
        prepareDrawProgram(program, 0);
    }
    bindDrawTexture(texture, program->samplers, 0xFFFFFFFF, 0);
    sendDrawUniform(programUniforms(program), 16, matrix.get());
    bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawQuadInput]);
    issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer);
    destroyDrawOrthographicProjection(projection.get(), 2);
}
// Original cache records are constructed by the shared AGL9 setup; this is a read-only prefix.
struct EffectCacheRecord_l { u8 _00[0xBCC]; be<u32> flags; };
struct EffectCacheOwner_l { u8 _00[0x18]; EffectCacheRecord_l records[4]; };
struct EffectCoreMap_l { be<u32> slots[3]; };
using EffectCachePointer = gptr<EffectCacheOwner_l>;
static EffectCachePointer sEffectCache;
static EffectCoreMap_l sEffectCoreMap;
static sead::SafeTextVtable sEffectTextVtable;
struct EffectLabel_l { be<u8> first; };
static EffectLabel_l sReduceLabel;
static EffectLabel_l sExpandLabel;
static f32 sEffectThreshold;
static f32 sEffectUnit;
agl::TextureData* acquireEffectTexture(agl::EffectTexturePool_l*,const sead::SafeText*,u32,s32,s32,u32,u32,u32,u32);
void releaseEffectTexture(agl::EffectTexturePool_l*,agl::TextureData*);
void copyEffectColor(agl::ColorSurface*,const agl::TextureData*);
void copyEffectSampler(agl::TextureSampler*,const agl::TextureData*);
void invalidateEffectColor(agl::ColorSurface*);
u32 effectCoreId();
void activateEffectFrame(sead::VirtualFrameBuffer*);
sead::Viewport* createEffectViewport(sead::Viewport*,const sead::FrameBufferSize*);
void applyEffectViewport(const sead::Viewport*,const sead::VirtualFrameBuffer*);

static s32 effectWidth(const agl::TextureData* texture) {
    s32 value = texture->surface.mWidth;
    s32 minimum = texture->minimumWidth;
    return value > minimum ? value : minimum;
}
static s32 effectHeight(const agl::TextureData* texture) {
    s32 value = texture->surface.mHeight;
    s32 minimum = texture->minimumHeight;
    return value > minimum ? value : minimum;
}
static void setEffectFrameBounds(agl::EffectContext_l* context,s32 width,s32 height) {
    f32 w = f32(width), h = f32(height), threshold = sEffectThreshold;
    context->frame.mBase.mArea.mMinX = w > threshold ? threshold : w;
    context->frame.mBase.mArea.mMinY = h > threshold ? threshold : h;
    context->frame.mBase.mArea.mMaxX = w > threshold ? w : threshold;
    context->frame.mBase.mArea.mMaxY = h > threshold ? h : threshold;
    context->frame.mBase.mSize.mHeight = h;
    context->frame.mBase.mSize.mWidth = w;
}
static bool effectSamplerMatches(const agl::TextureSampler* sampler,const agl::TextureData* source) {
    const sead::GraphicsSurface* a = &sampler->texture.surface;
    const sead::GraphicsSurface* b = &source->surface;
    return u32(a->mDimension)==u32(b->mDimension) && u32(a->mWidth)==u32(b->mWidth) &&
        u32(a->mHeight)==u32(b->mHeight) && u32(a->mDepth)==u32(b->mDepth) &&
        u32(a->mMipCount)==u32(b->mMipCount) && u32(a->mFormat)==u32(b->mFormat) &&
        u32(a->mSwizzle)==u32(b->mSwizzle) && u32(a->mTileMode)==u32(b->mTileMode) && u32(a->mSamples)==u32(b->mSamples);
}
static void updateEffectImages(agl::TextureSampler* sampler,const agl::TextureData* source) {
    if (!effectSamplerMatches(sampler,source)) copyEffectSampler(sampler,source);
    else {
        void* image = source->surface.mImage;
        void* mipData = source->surface.mMipImage;
        // TextureSampler::nativeTexture starts with the sampler's own GX2 surface.
        auto* native = reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
        native->mImage = image;
        native->mMipImage = mipData;
        sampler->texture.surface.mImage = image;
        sampler->texture.surface.mMipImage = mipData;
    }
}
static bool effectDrawDisabled() {
    EffectCacheOwner_l* owner = sEffectCache;
    u32 core = effectCoreId();
    u32 slot = sEffectCoreMap.slots[core];
    return (u32(sead::elementAt<EffectCacheRecord_l>(gabi::ea(owner) + u32(offsetof(EffectCacheOwner_l, records)), slot).flags) & 1) != 0;
}
static agl::ShaderProgram* effectProgram(u32 registryIndex,u32 variant) {
    agl::ShaderProgramSharedData* data = programSharedData(getRegistryProgram(registryIndex));
    if (s32(variant)<=0) return data->variation.root;
    u32 index = variant-1;
    agl::ShaderProgram* first = data->variation.programs;
    if (index < u32(data->variation.programCount)) return first+index;
    return first;
}
static void resetEffectMapping(agl::EffectContext_l* context) {
    if (context->mappingChanged != 0) {
        context->mappingChanged = 0;
        u32 dirty = context->sampler.dirty;
        context->sampler.componentMapping[0] = 0;
        context->sampler.componentMapping[1] = 1;
        context->sampler.dirty = dirty | 1;
        context->sampler.customMapping = 1;
        context->sampler.componentMapping[2] = 2;
        context->sampler.componentMapping[3] = 5;
    }
}

/* 027AC390 */
u32 agl::ScalarModeEntry::applyMode(EffectContext_l* context,u32 shift,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous = context->working;
    TextureData* source = previous != NULL ? previous : context->source;
    u32 width = u32(effectWidth(source));
    u32 height = u32(effectHeight(source));
    width = (shift & 32) ? 0 : width >> (shift & 31);
    height = (shift & 32) ? 0 : height >> (shift & 31);
    if (s32(width)<1) width=1;
    if (s32(height)<1) height=1;
    gabi::FrameLocal<sead::SafeText> name(0x40);
    name->mVtable = &sEffectTextVtable;
    name->mText = &sReduceLabel.first;
    TextureData* target = acquireEffectTexture(&context->pool,name.get(),context->format,s32(width),s32(height),1,0,0,1);
    context->working = target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,s32(width),s32(height));
    updateEffectImages(&context->sampler,source);
    u32 result=initial;
    if (!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x48);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        ShaderProgram* program=effectProgram(67,shift);
        result=bindDrawProgram(program,initial);
        gabi::FrameLocal<be<f32>> scalar(0xC);
        *scalar.get()=mScalar.mValue;
        sendDrawUniform(getDrawUniform(program,2),4,scalar.get());
        f32 reciprocal=gabi::fdivs_ppc(sEffectUnit,f32(effectWidth(source)));
        gabi::FrameLocal<sead::RenderVector2> step(0x28);
        step->x=reciprocal;step->y=reciprocal;
        sendDrawUniform(getDrawUniform(program,1),4,step.get());
        gabi::FrameLocal<DrawFloat4> range(0x30);
        range->mValues[1]=sEffectThreshold;
        range->mValues[3]=sEffectUnit;
        range->mValues[2]=sEffectUnit;
        range->mValues[0]=sEffectThreshold;
        sendDrawUniform(programUniforms(program),4,range.get());
        u32 flags=program->flags;
        if(flags&2){program->flags=flags&~2u;prepareDrawProgram(program,0);}
        bindDrawTexture(&context->sampler,program->samplers,0xFFFFFFFF,0);
        bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawQuadInput]);
        issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer);
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    return result;
}
/* 027ACA30 */
u32 agl::ScalarModeEntry::applyCount(EffectContext_l* context,u32 shift,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous = context->working;
    TextureData* source = previous != NULL ? previous : context->source;
    TextureData* original = context->source;
    s32 originalWidth = effectWidth(original);
    s32 originalHeight = effectHeight(original);
    u32 shiftedWidth = (shift & 32) ? 0 : u32(effectWidth(source)) << (shift & 31);
    u32 shiftedHeight = (shift & 32) ? 0 : u32(effectHeight(source)) << (shift & 31);
    s32 width = s32(shiftedWidth) < originalWidth ? s32(shiftedWidth) : originalWidth;
    s32 height = s32(shiftedHeight) < originalHeight ? s32(shiftedHeight) : originalHeight;
    gabi::FrameLocal<sead::SafeText> name(0x40);
    name->mVtable = &sEffectTextVtable;
    name->mText = &sExpandLabel.first;
    TextureData* target = acquireEffectTexture(&context->pool,name.get(),context->format,s32(width),s32(height),1,0,0,1);
    context->working = target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,s32(width),s32(height));
    updateEffectImages(&context->sampler,source);
    u32 result=initial;
    if (!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x48);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        ShaderProgram* program=effectProgram(68,shift);
        result=bindDrawProgram(program,initial);
        gabi::FrameLocal<be<f32>> scalar(0xC);
        *scalar.get()=mScalar.mValue;
        sendDrawUniform(getDrawUniform(program,2),4,scalar.get());
        f32 reciprocal=gabi::fdivs_ppc(sEffectUnit,f32(effectWidth(source)));
        gabi::FrameLocal<sead::RenderVector2> step(0x28);
        step->x=reciprocal;step->y=reciprocal;
        sendDrawUniform(getDrawUniform(program,1),4,step.get());
        gabi::FrameLocal<DrawFloat4> range(0x30);
        range->mValues[1]=sEffectThreshold;
        range->mValues[3]=sEffectUnit;
        range->mValues[2]=sEffectUnit;
        range->mValues[0]=sEffectThreshold;
        sendDrawUniform(programUniforms(program),4,range.get());
        u32 flags=program->flags;
        if(flags&2){program->flags=flags&~2u;prepareDrawProgram(program,0);}
        bindDrawTexture(&context->sampler,program->samplers,0xFFFFFFFF,0);
        bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawQuadInput]);
        issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawQuadShape].mIndexBuffer);
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    return result;
}
static EffectLabel_l sSelectorLabel;
static sead::RenderVector2 sEffectPosition;
static sead::RenderVector2 sEffectScale;
u32 drawIndexedViewport(agl::TextureSampler*, const sead::Viewport*, s32, const void*, const sead::RenderVector2*, const sead::RenderVector2*,u32);
u32 drawCombinationViewport(agl::TextureSampler*, const sead::Viewport*, u32, u32, u32, const sead::RenderVector2*,u32);

/* 027AD0D4 */
u32 agl::TripleSelectorEntry::applySelector(EffectContext_l* context,u32 kind,u32 mode,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous=context->working;
    TextureData* source=previous!=NULL ? previous : context->source;
    s32 width=effectWidth(source),height=effectHeight(source);
    gabi::FrameLocal<sead::SafeText> name(0x28);
    name->mVtable=&sEffectTextVtable;
    name->mText=&sSelectorLabel.first;
    TextureData* target=acquireEffectTexture(&context->pool,name.get(),context->format,width,height,1,0,0,1);
    context->working=target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,width,height);
    updateEffectImages(&context->sampler,source);
    u32 result=initial;
    if(!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x30);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        if(kind==1) {
            result=drawIndexedViewport(&context->sampler,viewport.get(),1,&sEffectPosition,&sEffectScale,&sEffectPosition,result);
        } else if(kind==3) {
            result=drawCombinationViewport(&context->sampler,viewport.get(),mode,0,1,&sEffectPosition,result);
        } else if(kind==2 || kind==4) {
            result=drawCombinationViewport(&context->sampler,viewport.get(),mode,1,1,&sEffectPosition,result);
        }
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    if(kind==4) result=applySelector(context,3,mode,result);
    return result;
}
static EffectLabel_l sStorageLabel;
static EffectLabel_l sPairsLabel;
u32 prepareEmbeddedEffect(agl::color_correction::ColorCorrection*,u32);
u32 drawEmbeddedEffect(agl::color_correction::ColorCorrection*,agl::RenderBuffer*,agl::TextureSampler*,u32);
u32 drawTripleViewport(agl::TextureSampler*,const sead::Viewport*,const void*,const void*,const void*,const sead::RenderVector2*,const sead::RenderVector2*,u32);

/* 027AD658 */
u32 agl::EmbeddedStorageEntry::applyStorage(EffectContext_l* context,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous=context->working;
    TextureData* source=previous!=NULL ? previous : context->source;
    updateEffectImages(&context->sampler,source);
    s32 width=effectWidth(source),height=effectHeight(source);
    u32 result=initial;
    if(!effectDrawDisabled()) result=prepareEmbeddedEffect(&mStorage,result);
    gabi::FrameLocal<sead::SafeText> name(0x28);
    name->mVtable=&sEffectTextVtable;
    name->mText=&sStorageLabel.first;
    TextureData* target=acquireEffectTexture(&context->pool,name.get(),context->format,width,height,1,0,0,1);
    context->working=target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,width,height);
    // The second cache/core observation is fresh after the allocator and descriptor calls.
    if(!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x30);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        result=drawEmbeddedEffect(&mStorage,&context->frame,&context->sampler,result);
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    return result;
}
/* 027ADA7C */
u32 agl::TriplePairEntry::applyPairs(EffectContext_l* context,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous=context->working;
    TextureData* source=previous!=NULL ? previous : context->source;
    s32 width=effectWidth(source),height=effectHeight(source);
    gabi::FrameLocal<sead::SafeText> name(0x28);
    name->mVtable=&sEffectTextVtable;
    name->mText=&sPairsLabel.first;
    TextureData* target=acquireEffectTexture(&context->pool,name.get(),context->format,width,height,1,0,0,1);
    context->working=target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,width,height);
    updateEffectImages(&context->sampler,source);
    u32 result=initial;
    if(!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x30);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        result=drawTripleViewport(&context->sampler,viewport.get(),mPairs[0].mValue,mPairs[1].mValue,mPairs[2].mValue,&sEffectScale,&sEffectPosition,result);
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    return result;
}
static EffectLabel_l sSizeLabel;
u32 drawDefaultViewport(agl::TextureSampler*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32);
/* 027ADE6C */
u32 agl::DoublePairEntry::applySize(EffectContext_l* context,u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureData* previous=context->working;
    TextureData* source=previous!=NULL ? previous : context->source;
    s32 width=gabi::ftoi(gabi::fmuls_ppc(f32(u32(effectWidth(source))),mPairs[1].mValue[0]));
    s32 height=gabi::ftoi(gabi::fmuls_ppc(f32(u32(effectHeight(source))),mPairs[1].mValue[1]));
    if(width<1) width=1;
    if(height<1) height=1;
    gabi::FrameLocal<sead::SafeText> name(0x30);
    name->mVtable=&sEffectTextVtable;
    name->mText=&sSizeLabel.first;
    TextureData* target=acquireEffectTexture(&context->pool,name.get(),context->format,width,height,1,0,0,1);
    context->working=target;
    copyEffectColor(&context->color,target);
    setEffectFrameBounds(context,width,height);
    updateEffectImages(&context->sampler,source);
    context->sampler.settings.modes[3]=6;
    context->sampler.settings.modes[4]=6;
    context->sampler.settings.modes[5]=6;
    context->sampler.dirty=u32(context->sampler.dirty)|2;
    u32 result=initial;
    if(!effectDrawDisabled()) {
        activateEffectFrame(&context->frame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0x38);
        createEffectViewport(viewport.get(),&context->frame.mBase.mSize);
        applyEffectViewport(viewport.get(),&context->frame.mBase);
        gabi::FrameLocal<sead::RenderVector2> position(0x28);
        position->x=gabi::fmuls_ppc(f32(u32(effectWidth(source))),mPairs[0].mValue[0]);
        position->y=gabi::fmuls_ppc(f32(u32(effectHeight(source))),mPairs[0].mValue[1]);
        result=drawDefaultViewport(&context->sampler,viewport.get(),&sEffectScale,position.get(),result);
        invalidateEffectColor(&context->color);
    }
    if(previous!=NULL) releaseEffectTexture(&context->pool,previous);
    resetEffectMapping(context);
    context->sampler.settings.modes[3]=2;
    context->sampler.settings.modes[4]=2;
    context->sampler.settings.modes[5]=2;
    context->sampler.dirty=u32(context->sampler.dirty)|2;
    return result;
}
struct EffectGraphics_l { u8 opaque; };
using EffectGraphicsPointer = gptr<EffectGraphics_l>;
static EffectGraphicsPointer sEffectGraphics;
void initializeEffectDepth(agl::DepthSurface*);
void expandEffectDepth(agl::PlatformDepthBuffer_l*);
void restoreEffectGraphics(EffectGraphics_l*);
agl::TextureSampler* constructEffectSampler(agl::TextureSampler*);
void destroyEffectSampler(agl::TextureSampler*,u32);
void constructEffectArray(void*,u32,u32,u32);
struct EffectWeightArray { Quaternion values[32]; };

/* 027A9E28 */
static u32 drawDepthWeights(agl::RenderBuffer* buffer,u32 count,const be<f32>* weights,
        const agl::DrawFloat4* colors,u32 initial,f64 first,f64 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::DepthSurface* depth=buffer->mDepth;
    if(depth==NULL) return initial;
    if(depth->mHiZBuffer!=NULL) {
        if(depth->mBase.mDirty!=0) {
            initializeEffectDepth(depth);
            depth->mBase.mDirty=0;
        }
        expandEffectDepth(&depth->mDepth);
        restoreEffectGraphics(sEffectGraphics);
    }
    gabi::FrameLocal<agl::TextureSampler> sampler(0x88);
    constructEffectSampler(sampler.get());
    depth=buffer->mDepth;
    updateEffectImages(sampler.get(),agl::surfaceTexture(&depth->mBase));
    gabi::FrameLocal<sead::CompactRenderState> state(0x14);
    sead::CompactRenderState::create(state.get());
    state->mDepthTest=0;
    state->mDepthWrite=0;
    state->mBlendEnable=1;
    state->apply();
    gabi::FrameLocal<EffectWeightArray> sortedWeights(0x220);
    gabi::FrameLocal<EffectWeightArray> sortedColors(0x420);
    constructEffectArray(sortedWeights.get(),32,16,gabi::guest_fn((const void*)createQuaternionStorage));
    constructEffectArray(sortedColors.get(),32,16,gabi::guest_fn((const void*)createIdentityQuaternion));
    for(u32 i=0;i<count;++i) {
        u32 rank=0;
        for(u32 j=0;j<count;++j) {
            // bge tests !LT, so unordered comparisons skip the increment as well.
            if(j!=i && f32(weights[j])<f32(weights[i])) ++rank;
        }
        sortedWeights->values[rank].x=weights[i];
        sortedColors->values[rank].x=colors[i].mValues[0];
        sortedColors->values[rank].y=colors[i].mValues[1];
        sortedColors->values[rank].z=colors[i].mValues[2];
        sortedColors->values[rank].w=colors[i].mValues[3];
    }
    agl::ShaderProgram* program=getRegistryProgram(1);
    u32 result=bindDrawProgram(program,initial);
    gabi::FrameLocal<be<f32>> firstValue(0x8);
    // Raw stfs selects/truncates FPR bits; there is no frsp rounding instruction here.
    *firstValue.get()=gabi::f32_from_bits(ppc_stfs_bits(first));
    sendDrawUniform(programUniforms(program),4,firstValue.get());
    gabi::FrameLocal<be<f32>> secondValue(0xC);
    *secondValue.get()=gabi::f32_from_bits(ppc_stfs_bits(second));
    sendDrawUniform(getDrawUniform(program,1),4,secondValue.get());
    gabi::FrameLocal<be<u32>> countValue(0x10);
    *countValue.get()=count;
    sendDrawUniform(getDrawUniform(program,4),4,countValue.get());
    sendDrawUniform(getDrawUniform(program,2),count*4,sortedWeights.get());
    sendDrawUniform(getDrawUniform(program,3),count*4,sortedColors.get());
    bindDrawTexture(sampler.get(),program->samplers,0xFFFFFFFF,0);
    bindDrawGeometry(&sDrawShapeInputs->mInputs[agl::DrawFullscreenInput]);
    issueDrawPacket(&sDrawShapes->mIndexedShapes[agl::DrawFullscreenShape].mIndexBuffer);
    destroyEffectSampler(sampler.get(),2);
    return result;
}
/* WWHD glue
 data sEffectGraphics 101F8BD8
 ext 027B6F90 void initializeEffectDepth(agl::DepthSurface*)
 ext C00061A0 void expandEffectDepth(agl::PlatformDepthBuffer_l*)
 ext 0276B114 void restoreEffectGraphics(EffectGraphics_l*)
 ext 027BDF7C agl::TextureSampler* constructEffectSampler(agl::TextureSampler*)
 ext 027BE2B0 void destroyEffectSampler(agl::TextureSampler*,u32)
 ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
 ext 0274FEB0 void sead::CompactRenderState::apply()
 ext 028EFFD0 void constructEffectArray(void*,u32,u32,u32)
 data sSizeLabel 10168B18
 ext 027A90AC u32 drawDefaultViewport(agl::TextureSampler*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
 data sStorageLabel 10168AD0
 data sPairsLabel 10168B00
 ext 02784778 u32 prepareEmbeddedEffect(agl::color_correction::ColorCorrection*,u32)
 ext 02784E8C u32 drawEmbeddedEffect(agl::color_correction::ColorCorrection*,agl::RenderBuffer*,agl::TextureSampler*,u32)
 ext 027A96C0 u32 drawTripleViewport(agl::TextureSampler*,const sead::Viewport*,const void*,const void*,const void*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
 data sSelectorLabel 10168AC0
 data sEffectPosition 104A0908
 data sEffectScale 104A0920
 ext 027A9468 u32 drawIndexedViewport(agl::TextureSampler*, const sead::Viewport*, s32, const void*, const sead::RenderVector2*, const sead::RenderVector2*,u32)
 ext 027A9584 u32 drawCombinationViewport(agl::TextureSampler*, const sead::Viewport*, u32, u32, u32, const sead::RenderVector2*,u32)
 data sEffectCache 101F8DD4
 data sEffectCoreMap 104A0BC0
 data sEffectTextVtable 1016858C
 data sReduceLabel 10168A98
 data sExpandLabel 10168AAC
 data sEffectThreshold 10168874
 data sEffectUnit 10168A88
 ext 0279ADD4 agl::TextureData* acquireEffectTexture(agl::EffectTexturePool_l*, const sead::SafeText*, u32, s32, s32, u32, u32, u32, u32)
 ext 0279AE68 void releaseEffectTexture(agl::EffectTexturePool_l*, agl::TextureData*)
 ext 025C0928 void copyEffectColor(agl::ColorSurface*, const agl::TextureData*)
 ext 027BDEB4 void copyEffectSampler(agl::TextureSampler*, const agl::TextureData*)
 ext 027B6D64 void invalidateEffectColor(agl::ColorSurface*)
 ext C0009BB0 u32 effectCoreId()
 ext 0274D690 void activateEffectFrame(sead::VirtualFrameBuffer*)
 ext 0274F5F0 sead::Viewport* createEffectViewport(sead::Viewport*, const sead::FrameBufferSize*)
 ext 0274F964 void applyEffectViewport(const sead::Viewport*, const sead::VirtualFrameBuffer*)
 data sDrawZero 101684C8
 data sDrawHalf 101684D8
 ext 0274E364 sead::ProjectionState* createDrawOrthographicProjection(sead::ProjectionState*, const sead::ViewportBounds*, f32, f32)
 ext 0274D83C sead::ProjectionMatrix* sead::ProjectionState::deviceProjection()
 ext 0274E474 void destroyDrawOrthographicProjection(sead::ProjectionState*, u32)
 ext 025C0AC4 void makeDrawScaleTranslation(sead::ViewTransform*, const sead::ViewVector*, const sead::ViewVector*)
 ext 0272D680 void combineDrawMatrices(sead::ProjectionMatrix*, const sead::ProjectionMatrix*, const sead::ViewTransform*)
 data sDrawShapeInputs 101F8DDC
 data sDrawShapes 101F8DE4
 data sDefaultColor 104A01EC
 ext 0272D680 void combineRegistryMatrices(sead::RegistryMatrix44*, const sead::RegistryMatrix44*, const sead::RegistryMatrix34*)
 ext 027BFEF4 void finishDrawPrimitive()
 ext 027BFE5C void bindDrawGeometry(agl::VertexInputSet*)
 ext 027BE53C void bindDrawTexture(agl::TextureSampler*, void*, u32, u32)
 ext C0006178 void drawIndexedPacket(u32, u32, u32, const void*, s32, u32)
 ext 0274D4C0 bool sead::ColorChannels::equals(const sead::ColorChannels*) const
 data sDrawUnit 101684CC
 data sDefaultDrawScale 104A0920
 data sTextureSelection 101684E0
 ext 027BB9E0 void prepareDrawProgram(agl::ShaderProgram*, u32)
 ext 027B9F68 void applyDrawMode(u32)
 ext C00060E0 void callDrawCommands(void*, u32)
 ext 027BB7CC void selectDrawProgram(agl::ShaderProgram*)
 ext C0006A40 void setDrawVertexUniform(s32, u32, const void*)
 ext C0006908 void setDrawPixelUniform(s32, u32, const void*)
 data sDrawShaderHolder 101F8D6C
*/
