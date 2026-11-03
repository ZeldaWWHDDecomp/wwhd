// agl colour correction: hue/saturation/brightness/gamma, a four-channel tone curve and a "toy camera"
// filter, baked into an 8x8x8 colour map by the map shader and applied with the colour correction shader.
// Reconstructed from WWHD's own instructions and strings.
#include "bindings.h"
#include "agl/ColorCorrection.h"
using namespace agl;
using namespace agl::color_correction;

// Per-translation-unit data (image copies; tables have no initializer here).
static u8 sEmptyText = 0;
static sead::SafeTextVtable sLiteralTextMethods;
static u32 sColorCorrectionBaseMethods;
static u32 sColorCorrectionMethods;
static u32 sColorCorrectionRootMethods;
static u32 sParameterObjectBaseMethods;
static ParameterMethods_l sBoolParameterMethods;
static ParameterMethods_l sFloatParameterMethods;
static ParameterMethods_l sColorParameterMethods;
static const CurveDefaults sToneCurveDefaults;
static const CurveEvaluatorTable_l sToneCurveEvaluators;
// Singletons and shared tables of other units, read only here.
// (The shader holder is sShaderHolder, defined once in agl/TimingStatics.cpp.)
typedef gptr<DynamicTextureCache> TextureCachePointer;
typedef gptr<PrimitiveShapeInputs> ShapeInputsPointer;
typedef gptr<PrimitiveShapes> ShapesPointer;
// Core index -> slot table (sead's, filled at start-up).
struct CoreSlotTable_l { be<u32> mSlots[3]; };
static TextureCachePointer sCorrectionTextureCache;
static ShapeInputsPointer sCorrectionShapeInputs;
static ShapesPointer sCorrectionShapes;
static CoreSlotTable_l sCoreSlots;
static sead::ColorChannels sColorBlack;
static sead::ColorChannels sColorWhite;

void* allocate(u32 size);
void* allocateFromHeap(u32 size, void* heap, u32 alignment);
void freeMapImage(void* image);
void releaseDebugEntry(ResourceRecord* entry);
void releaseToneCurve(ToneCurveParameter* curve, u32 flags);
void destroySampler(TextureSampler* sampler, u32 flags);
void destroyRenderBuffer(RenderBuffer* buffer, u32 flags);
void createParameterRoot(ParameterRoot* root, sead::SafeText* name, u32 context);
void constructParameterObject(parameter_tree::Value* object);
void initializeParameterBase(void* parameter);
void describeParameter(void* parameter, sead::SafeText* key, sead::SafeText* label, sead::SafeText* hint,
    parameter_tree::Value* owner);
void createCompactRenderState(sead::CompactRenderState* state);
void constructRenderBuffer(RenderBuffer* buffer);
void constructArray(void* array, u32 count, u32 size, u32 constructor);
ColorSurface* constructColorSurface(ColorSurface* surface);
void createTextureData(TextureData* data);
void createTextureSampler(TextureSampler* sampler);
void createToneCurve(ToneCurveParameter* curve, sead::SafeText* name, sead::SafeText* label,
    parameter_tree::Value* owner);
void setRootNodeMeta(void* node, sead::SafeText* meta);
void appendParameterValue(parameter_tree::Node* node, parameter_tree::Value* value, sead::SafeText* name);
void configureTextureData(TextureData* data, u32 dimension, u32 format, u32 width, u32 height, u32 depth,
    s32 mipCount, u32 tileMode, u32 antiAlias);
void invalidateDataCache(void* address, u32 size);
void assignColorTexture(ColorSurface* surface, const TextureData* texture);
void copySamplerTexture(TextureSampler* sampler, const TextureData* texture);
u32 currentCoreId();
u32 refreshProgram(ShaderProgram* program, u32 unused);
void setShaderMode(u32 mode);
void callDisplayList(void* list, u32 size);
void bindStagePrograms(ShaderProgram* program);
void setVertexUniform(s32 location, u32 count, const void* values);
void setPixelUniform(s32 location, u32 count, const void* values);
void premultiplyColor(sead::ColorChannels* result, const sead::ColorChannels* color, f32 factor);
void createRenderState(sead::RenderState* state);
void applyRenderState(sead::RenderState* state);
void createViewport(sead::Viewport* viewport, const RenderBuffer* size);
void applyViewport(sead::Viewport* viewport, const RenderBuffer* surface);
void activateRenderBuffer(RenderBuffer* buffer);
void bindVertexInput(VertexInputSet* input);
void drawIndexed(u32 primitiveMode, u32 count, u32 indexFormat, const void* indices, u32 offset, u32 instances);
void invalidateColorSurface(ColorSurface* surface);
void applyCompactRenderState(sead::CompactRenderState* state);
u32 bindSampler(TextureSampler* sampler, const UniformLocations_l* location, s32 slot, u32 unused);
void* moveBytes(void* destination, const void* source, u32 size, u32 flags);

static constexpr f32 kEpsilon = 1.1920928955078125e-07f;
static constexpr f32 kMapLast = 7.0f;            // kMapSize - 1: tone table positions are i / 7
static constexpr f32 kLutScale = 0.875f;         // (kMapSize - 1) / kMapSize
static constexpr f32 kLutOffset = 0.0625f;       // 1 / (2 * kMapSize)
enum : u32 { kSurface3D = 2, kFormatRGBA8 = 0x1A };

// ---- helpers ----
static void setText(sead::SafeText* text, const char* value) {
    text->mText.v = gabi::guest_str(value);
    text->mVtable = &sLiteralTextMethods;
}
static void setEmptyText(sead::SafeText* text) {
    text->mText.v = gabi::ea(&sEmptyText);
    text->mVtable = &sLiteralTextMethods;
}
static void copyColor(sead::ColorChannels* destination, const sead::ColorChannels* source) {
    destination->r.bits = (u32)source->r.bits;
    destination->g.bits = (u32)source->g.bits;
    destination->b.bits = (u32)source->b.bits;
    destination->a.bits = (u32)source->a.bits;
}
// Construction of an embedded parameter: GHS keeps the null-object allocation path of the template.
static void* beginParameter(void* parameter, u32 size, ParameterMethods_l* methods) {
    if (gabi::ea(parameter) == 0) parameter = allocate(size);
    if (gabi::ea(parameter) != 0) {
        initializeParameterBase(parameter);
        static_cast<env::ParameterBinding*>(parameter)->mMethods = (sead::VtableSlot*)methods;
    }
    return parameter;
}
static ShaderProgram* holderProgram(u32 index) { return sShaderHolder->program(index); }
static const ShaderVariantRadix* macroAt(const ShaderProgramSharedData* shared, u32 index) {
    const ShaderVariantRadix* macros = shared->variation.radices;
    return index < shared->variation.count ? &macros[index] : &macros[0];
}
// Bounds-checked uniform / sampler entries (count read first): past the table, the first entry.
static const UniformLocations_l* uniformAt(ShaderProgram* program, u32 index) {
    u32 count = program->uniformCount;
    auto* uniforms = programUniforms(program);
    return index < count ? &uniforms[index] : &uniforms[0];
}
static const UniformLocations_l* samplerAt(ShaderProgram* program, u32 index) {
    u32 count = program->samplerCount;
    auto* samplers = reinterpret_cast<const UniformLocations_l*>(program->samplers.get());
    return index < count ? &samplers[index] : &samplers[0];
}
static void setUniform(const UniformLocations_l* location, u32 count, const void* values) {
    if (location->mVertexRegister != -1) setVertexUniform(location->mVertexRegister, count, values);
    if (location->mPixelRegister != -1) setPixelUniform(location->mPixelRegister, count, values);
}
// ShaderProgram activation (inlined in every draw): refresh, shader mode, display list or stage binding.
static u32 activateProgram(ShaderProgram* program, u32 shaderMode) {
    if (program->flags & ShaderProgramFlag_UpdatePending) {
        program->flags = program->flags & ~ShaderProgramFlag_UpdatePending;
        refreshProgram(program, 0);
    }
    u32 programMode = programSharedData(program)->mode;
    if (shaderMode != programMode) setShaderMode(programMode);
    const DisplayListStorage* displayList = programDisplayList(program);
    u32 size = displayList->mUsed;
    if (size != 0) callDisplayList(displayList->mData, size);
    else bindStagePrograms(program);
    return programMode;
}
// Draws the shared full-screen quad of the primitive shape singleton.
static void drawQuad() {
    bindVertexInput(&sCorrectionShapeInputs->mInputs[6]);
    IndexBufferLayout* quad = &sCorrectionShapes->mIndexedShapes[0].mIndexBuffer;
    u32 count = quad->mIndexCount;
    if (count != 0)
        drawIndexed(quad->mPrimitiveMode, count, quad->mFormat, quad->mStorage, 0, 1);
}
static bool surfacesMatch(const sead::GraphicsSurface& a, const sead::GraphicsSurface& b) {
    return (u32)a.mDimension == (u32)b.mDimension && (u32)a.mWidth == (u32)b.mWidth
        && (u32)a.mHeight == (u32)b.mHeight && (u32)a.mDepth == (u32)b.mDepth
        && (u32)a.mMipCount == (u32)b.mMipCount && (u32)a.mFormat == (u32)b.mFormat
        && (u32)a.mSwizzle == (u32)b.mSwizzle && (u32)a.mTileMode == (u32)b.mTileMode
        && (u32)a.mSamples == (u32)b.mSamples;
}
// TextureSampler::copyTexture's fast path: same surface, only the image pointers change.
static void refreshSamplerImages(TextureSampler* sampler, const TextureData* texture) {
    void* image = texture->surface.mImage;
    void* mipImage = texture->surface.mMipImage;
    auto* native = reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
    sampler->texture.surface.mImage = image;
    sampler->texture.surface.mMipImage = mipImage;
    native->mImage = image;
    native->mMipImage = mipImage;
}
static bool outsideEpsilon(f32 difference) { return -kEpsilon > difference || difference > kEpsilon; }

// Toy camera uniforms, built in one stack block (each upload reads four words from its start).
struct ToycamUniforms_l {
    be<f32> mSaturation1, mSaturation2, mBrightness, mContrast;
    sead::ColorChannels mLevel1, mLevel2, mMulColor;
};
WWHD_SIZE(ToycamUniforms_l, 0x40);

/* 027833D8 */
void ColorCorrection::updateToneTable() {
    gabi::AutoFrame frame{gabi::current_function()};
    bool changed = false;
    for (s32 i = 0; i < (s32)kMapSize; ++i) {
        f32 position = (f32)i / kMapLast;
        mToneCurve.evaluateAll(&mToneTable[i], position);
        const sead::ColorChannels& entry = mToneTable[i];
        if (outsideEpsilon(position - entry.r.value) || outsideEpsilon(position - entry.g.value)
            || outsideEpsilon(position - entry.b.value))
            changed = true;
    }
    u32 flags = mFlags;
    if (((flags & kFlagToneCurve) != 0) != changed) {
        if (changed) flags |= kFlagToneCurve | kFlagVariationDirty;
        else flags = (flags & ~kFlagToneCurve) | kFlagVariationDirty;
    }
    mFlags = flags | kFlagDirty;
}
/* 027835B0 */
u32 ColorCorrection::hueEnabled() { return mHue.mValue != 0.0f; }
/* 027835D0 */
u32 ColorCorrection::saturationEnabled() { return mSaturation.mValue != 1.0f; }
/* 027835F0 */
u32 ColorCorrection::brightnessEnabled() { return mBrightness.mValue != 1.0f; }
/* 02783610 */
u32 ColorCorrection::gammaEnabled() { return mGamma.mValue != 1.0f; }
/* 02783630 */
void ColorCorrection::updateFlags() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 hue = hueEnabled();
    u32 flags = mFlags;
    flags = hue != 0 ? flags | kFlagHue : flags & ~kFlagHue;
    mFlags = flags;
    flags = saturationEnabled() != 0 ? flags | kFlagSaturation : flags & ~kFlagSaturation;
    mFlags = flags;
    flags = brightnessEnabled() != 0 ? flags | kFlagBrightness : flags & ~kFlagBrightness;
    mFlags = flags;
    u32 gamma = gammaEnabled();
    u8 toycam = mToycamEnable.mValue;
    flags = gamma != 0 ? flags | kFlagGamma : flags & ~kFlagGamma;
    mFlags = flags;
    if (toycam != 0) flags |= kFlagToycam | kFlagVariationDirty;
    else flags = (flags & ~kFlagToycam) | kFlagVariationDirty;
    mFlags = flags | kFlagDirty;
}
/* 0278373C */
void ColorCorrection::applyParameters() {
    gabi::AutoFrame frame{gabi::current_function()};
    updateToneTable();
    updateFlags();
}
// Entered through the parameter root's table with this = &mRoot.
/* 02783770 */
void ColorCorrection::applyParametersFromRoot() {
    gabi::AutoFrame frame{gabi::current_function()};
    ColorCorrection* owner = gabi::at<ColorCorrection>(gabi::ea(this) - (u32)offsetof(ColorCorrection, mRoot));
    owner->applyParameters();
}
/* 02783778 */
void ColorCorrection::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* image = mMapImage;
    if (image != NULL) {
        freeMapImage(image);
        mMapImage = NULL;
    }
    if (mDebugEntry.v.get() != 0) {
        releaseDebugEntry(mDebugEntry);
        ResourceRecord* entry = mDebugEntry;
        if (entry != NULL) gabi::call_ptr<void>(entry->mVtable->mDestroy.mFunction, entry, 3);
        mDebugEntry = NULL;
    }
}
/* 027837F4 */
void ColorCorrection::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sColorCorrectionMethods;
    mRoot.mTree.mTable = gabi::ea(&sColorCorrectionRootMethods);
    finalize();
    releaseToneCurve(&mToneCurve, 2);
    destroySampler(&mSourceSampler, 2);
    destroySampler(&mMapSampler, 2);
    destroyRenderBuffer(&mMapBuffer, 2);
    mParameters.mVtable = reinterpret_cast<parameter_tree::ValueVtable*>(&sParameterObjectBaseMethods);
    if (flags & 1) operator_delete(this);
}
/* 02783898 */
void ColorCorrection::resetParameters() {
    gabi::AutoFrame frame{gabi::current_function()};
    mHue.mValue = 0.0f;
    mSaturation.mValue = 1.0f;
    mGamma.mValue = 1.0f;
    mBrightness.mValue = 1.0f;
    mToneCurve.initializeCurves();
    for (s32 i = 0; i < (s32)kMapSize; ++i) {
        f32 position = (f32)i / kMapLast;
        sead::ColorChannels& entry = mToneTable[i];
        entry.g.value = position;
        entry.b.value = position;
        entry.r.value = position;
        entry.a.value = position;
    }
    copyColor(&mToycamOffset1.mValue, &sColorBlack);
    copyColor(&mToycamOffset2.mValue, &sColorBlack);
    copyColor(&mToycamLevel1.mValue, &sColorWhite);
    copyColor(&mToycamLevel2.mValue, &sColorWhite);
    mToycamSaturation1.mValue = 1.0f;
    mToycamSaturation2.mValue = 1.0f;
    mToycamBrightness.mValue = 1.0f;
    mToycamContrast.mValue = 1.0f;
    copyColor(&mToycamMulColor.mValue, &sColorWhite);
    mMapVariation = 0;
    mFlags = kFlagDirty;
}

static void registerFloat(ColorCorrection* self, FloatParameter* parameter, sead::SafeText* key,
        sead::SafeText* label, sead::SafeText* hint, f32 value) {
    parameter = (FloatParameter*)beginParameter(parameter, sizeof(FloatParameter), &sFloatParameterMethods);
    if (gabi::ea(parameter) == 0) return;
    describeParameter(parameter, key, label, hint, &self->mParameters);
    parameter->mValue = value;
}
static void registerColor(ColorCorrection* self, ColorParameter* parameter, sead::SafeText* key,
        sead::SafeText* label, sead::SafeText* hint, const sead::ColorChannels* value) {
    parameter = (ColorParameter*)beginParameter(parameter, sizeof(ColorParameter), &sColorParameterMethods);
    if (gabi::ea(parameter) == 0) return;
    parameter->mValue.b.value = 0.0f;
    parameter->mValue.r.value = 0.0f;
    parameter->mValue.g.value = 0.0f;
    parameter->mValue.a.value = 1.0f;
    describeParameter(parameter, key, label, hint, &self->mParameters);
    copyColor(&parameter->mValue, value);
}
static void registerBool(ColorCorrection* self, BoolParameter* parameter, sead::SafeText* key,
        sead::SafeText* label, sead::SafeText* hint, u8 value) {
    parameter = (BoolParameter*)beginParameter(parameter, sizeof(BoolParameter), &sBoolParameterMethods);
    if (gabi::ea(parameter) == 0) return;
    setEmptyText(hint);
    describeParameter(parameter, key, label, hint, &self->mParameters);
    parameter->mValue = value;
}

/* 02783A70 */
ColorCorrection* ColorCorrection::create(ColorCorrection* self) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(self) == 0) self = (ColorCorrection*)allocate(sizeof(ColorCorrection));
    if (gabi::ea(self) == 0) return self;
    self->mVtable = &sColorCorrectionBaseMethods;
    gabi::FrameLocal<sead::SafeText> rootName(0x10);
    setText(rootName, "aglccr");
    createParameterRoot(&self->mRoot, rootName, 0);
    self->mVtable = &sColorCorrectionMethods;
    self->mRoot.mTree.mTable = gabi::ea(&sColorCorrectionRootMethods);
    constructParameterObject(&self->mParameters);

    gabi::FrameLocal<sead::SafeText> enableKey(0x18), enableLabel(0x20), enableHint(0x178);
    setText(enableKey, "enable");
    setText(enableLabel, "有効");
    registerBool(self, &self->mEnable, enableKey, enableLabel, enableHint, 1);

    gabi::FrameLocal<sead::SafeText> hueKey(0x28), hueLabel(0x30), hueHint(0x38);
    setText(hueKey, "hue");
    setText(hueLabel, "色相");
    setText(hueHint, "Min=-180, Max=180");
    registerFloat(self, &self->mHue, hueKey, hueLabel, hueHint, 0.0f);

    gabi::FrameLocal<sead::SafeText> saturationKey(0x40), saturationLabel(0x48), saturationHint(0x50);
    setText(saturationKey, "saturation");
    setText(saturationLabel, "彩度");
    setText(saturationHint, "Min=0, Max=2");
    registerFloat(self, &self->mSaturation, saturationKey, saturationLabel, saturationHint, 1.0f);

    gabi::FrameLocal<sead::SafeText> brightnessKey(0x58), brightnessLabel(0x60), brightnessHint(0x68);
    setText(brightnessKey, "brightness");
    setText(brightnessLabel, "明度");
    setText(brightnessHint, "Min=0, Max=2");
    registerFloat(self, &self->mBrightness, brightnessKey, brightnessLabel, brightnessHint, 1.0f);

    gabi::FrameLocal<sead::SafeText> gammaKey(0x70), gammaLabel(0x78), gammaHint(0x80);
    setText(gammaKey, "gamma");
    setText(gammaLabel, "ガンマ補正");
    setText(gammaHint, "Min=0.25, Max=4");
    registerFloat(self, &self->mGamma, gammaKey, gammaLabel, gammaHint, 1.0f);

    gabi::FrameLocal<sead::SafeText> toycamKey(0x88), toycamLabel(0x90), toycamHint(0x180);
    setText(toycamKey, "toycam_enable");
    setText(toycamLabel, "トイカメラフィルタ");
    registerBool(self, &self->mToycamEnable, toycamKey, toycamLabel, toycamHint, 0);

    gabi::FrameLocal<sead::SafeText> offset1Key(0x98), offset1Label(0xA0), offset1Hint(0xA8);
    setText(offset1Key, "toycam_offset1");
    setText(offset1Label, "トーンオフセット１");
    setText(offset1Hint, "Min=0, Max=1");
    registerColor(self, &self->mToycamOffset1, offset1Key, offset1Label, offset1Hint, &sColorBlack);

    gabi::FrameLocal<sead::SafeText> offset2Key(0xB0), offset2Label(0xB8), offset2Hint(0xC0);
    setText(offset2Key, "toycam_offset2");
    setText(offset2Label, "トーンオフセット２");
    setText(offset2Hint, "Min=0, Max=1");
    registerColor(self, &self->mToycamOffset2, offset2Key, offset2Label, offset2Hint, &sColorBlack);

    gabi::FrameLocal<sead::SafeText> level1Key(0xC8), level1Label(0xD0), level1Hint(0xD8);
    setText(level1Key, "toycam_level1");
    setText(level1Label, "レベル補正１");
    setText(level1Hint, "Min=0, Max=2");
    registerColor(self, &self->mToycamLevel1, level1Key, level1Label, level1Hint, &sColorWhite);

    gabi::FrameLocal<sead::SafeText> level2Key(0xE0), level2Label(0xE8), level2Hint(0xF0);
    setText(level2Key, "toycam_level2");
    setText(level2Label, "レベル補正２");
    setText(level2Hint, "Min=0, Max=2");
    registerColor(self, &self->mToycamLevel2, level2Key, level2Label, level2Hint, &sColorWhite);

    gabi::FrameLocal<sead::SafeText> saturation1Key(0xF8), saturation1Label(0x100), saturation1Hint(0x108);
    setText(saturation1Key, "toycam_saturation1");
    setText(saturation1Label, "彩度１");
    setText(saturation1Hint, "Min=0, Max=2");
    registerFloat(self, &self->mToycamSaturation1, saturation1Key, saturation1Label, saturation1Hint, 1.0f);

    gabi::FrameLocal<sead::SafeText> saturation2Key(0x110), saturation2Label(0x118), saturation2Hint(0x120);
    setText(saturation2Key, "toycam_saturation2");
    setText(saturation2Label, "彩度２");
    setText(saturation2Hint, "Min=0, Max=2");
    registerFloat(self, &self->mToycamSaturation2, saturation2Key, saturation2Label, saturation2Hint, 1.0f);

    gabi::FrameLocal<sead::SafeText> toycamBrightnessKey(0x128), toycamBrightnessLabel(0x130),
        toycamBrightnessHint(0x138);
    setText(toycamBrightnessKey, "toycam_brightness");
    setText(toycamBrightnessLabel, "明度");
    setText(toycamBrightnessHint, "Min=0, Max=2");
    registerFloat(self, &self->mToycamBrightness, toycamBrightnessKey, toycamBrightnessLabel,
        toycamBrightnessHint, 1.0f);

    gabi::FrameLocal<sead::SafeText> contrastKey(0x140), contrastLabel(0x148), contrastHint(0x150);
    setText(contrastKey, "toycam_contrast");
    setText(contrastLabel, "コントラスト");
    setText(contrastHint, "Min=0, Max=2");
    registerFloat(self, &self->mToycamContrast, contrastKey, contrastLabel, contrastHint, 1.0f);

    gabi::FrameLocal<sead::SafeText> mulColorKey(0x158), mulColorLabel(0x160), mulColorHint(0x168);
    setText(mulColorKey, "toycam_mul_color");
    setText(mulColorLabel, "乗算カラー");
    setText(mulColorHint, "Min=0, Max=1");
    registerColor(self, &self->mToycamMulColor, mulColorKey, mulColorLabel, mulColorHint, &sColorWhite);

    createCompactRenderState(&self->mDrawState);
    LutTransform* lut = &self->mLutTransform;
    if (gabi::ea(lut) == 0) lut = (LutTransform*)allocate(sizeof(LutTransform));
    if (gabi::ea(lut) != 0) {
        lut->mOffset = 0.0f;
        lut->mScale = 0.0f;
    }
    constructRenderBuffer(&self->mMapBuffer);
    ColorSurface* slices = self->mMapSlices;
    if (gabi::ea(slices) == 0) slices = (ColorSurface*)allocate(sizeof(self->mMapSlices));
    if (gabi::ea(slices) != 0)
        constructArray(slices, kMapSize, sizeof(ColorSurface), gabi::guest_fn((const void*)constructColorSurface));
    createTextureData(&self->mMapData);
    createTextureSampler(&self->mMapSampler);
    self->mMapImage = NULL;
    createTextureSampler(&self->mSourceSampler);
    self->mMapVariation = 0;
    // The tone table's element constructors are empty; only the null-object allocation remains.
    if (gabi::ea(self->mToneTable) == 0) allocate(sizeof(self->mToneTable));

    gabi::FrameLocal<sead::SafeText> curveName(0x170), text(8);
    setText(curveName, "level");
    setText(text, "レベル補正");
    createToneCurve(&self->mToneCurve, curveName, text, &self->mParameters);
    self->mDebugEntry = NULL;
    be<u32>* flags = &self->mFlags;
    if (gabi::ea(flags) == 0) flags = (be<u32>*)allocate(sizeof(be<u32>));
    if (gabi::ea(flags) != 0) *flags = 0;
    self->mDrawState.mBlendEnable = 0;
    self->mDrawState.mDepthTest = 0;
    self->mDrawState.mDepthWrite = 0;
    self->mToneCurve.mCurveCount = 3;
    setText(text, "Icon=COLOR");
    setRootNodeMeta(self, text);
    setText(text, "color_correction");
    appendParameterValue(&self->mRoot.mTree, &self->mParameters, text);
    self->resetParameters();
    return self;
}
/* 02784400 */
void ColorCorrection::initializeMap(void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureTextureData(&mMapData, kSurface3D, kFormatRGBA8, kMapSize, kMapSize, kMapSize, 1, 0, 0);
    void* image = allocateFromHeap(mMapData.surface.mImageBytes, heap, mMapData.surface.mAlignment);
    mMapData.surface.mImage = image;
    mMapImage = image;
    invalidateDataCache(image, mMapData.surface.mImageBytes);
    sead::VirtualFrameBuffer& buffer = mMapBuffer.mBase;
    buffer.mArea.mMinY = 0.0f;
    buffer.mArea.mMaxX = (f32)kMapSize;
    buffer.mSize.mWidth = (f32)kMapSize;
    buffer.mArea.mMinX = 0.0f;
    buffer.mSize.mHeight = (f32)kMapSize;
    buffer.mArea.mMaxY = (f32)kMapSize;
    // One colour target per depth slice of the 3D map.
    for (u32 slice = 0; slice < kMapSize; ++slice) {
        ColorSurface* surface = &mMapSlices[slice];
        assignColorTexture(surface, &mMapData);
        if (surface->mBase.mViewFirstSlice != slice) {
            surface->mBase.mViewFirstSlice = slice;
            surface->mBase.mDirty = 1;
        }
        if (surface->mBase.mViewMip != 0) {
            surface->mBase.mViewMip = 0;
            surface->mBase.mDirty = 1;
        }
        mMapBuffer.mColors[slice] = surface;
    }
    if (surfacesMatch(mMapSampler.texture.surface, mMapData.surface)) refreshSamplerImages(&mMapSampler, &mMapData);
    else copySamplerTexture(&mMapSampler, &mMapData);
    mLutTransform.mScale = kLutScale;
    mLutTransform.mOffset = kLutOffset;
}
/* 02784670 */
void ColorCorrection::updateMapVariation() {
    ShaderProgram* program = holderProgram(kProgramColorCorrectionMap);
    u32 flags = mFlags;
    mMapVariation = 0;
    s32 variation = 0;
    if (flags & kFlagHue) {
        variation = programSharedData(program)->variation.radices->divisor * 2;
        mMapVariation = variation;
    } else if (flags & (kFlagSaturation | kFlagBrightness)) {
        variation = programSharedData(program)->variation.radices->divisor;
        mMapVariation = variation;
    }
    if (flags & kFlagToneCurve) {
        variation += macroAt(programSharedData(program), 1)->divisor;
        mMapVariation = variation;
    }
    if (flags & kFlagGamma) {
        variation += macroAt(programSharedData(program), 2)->divisor;
        mMapVariation = variation;
    }
    if (flags & kFlagToycam) {
        variation += macroAt(programSharedData(program), 3)->divisor;
        mMapVariation = variation;
    }
    mFlags = flags & ~kFlagVariationDirty;
}
/* 02784778 */
u32 ColorCorrection::drawMap(u32 shaderMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mEnable.mValue == 0) return shaderMode;
    u32 flags = mFlags;
    if ((flags & kFlagDirty) == 0 && (flags & kFlagAlwaysDraw) == 0) return shaderMode;
    // Skipped while the dynamic texture owner's slot of this core is busy.
    DynamicTextureCache* cache = sCorrectionTextureCache;
    u32 slot = sCoreSlots.mSlots[currentCoreId()];
    if (cache->mPools[slot].mCoreFlags & 1) return shaderMode;
    if (mFlags & kFlagVariationDirty) updateMapVariation();

    ShaderProgram* program = holderProgram(kProgramColorCorrectionMap);
    s32 variation = mMapVariation;
    ShaderProgramSharedData* shared = programSharedData(program);
    ShaderProgram* active;
    if (variation <= 0) {
        active = shared->variation.root;
    } else {
        u32 index = variation - 1;
        ShaderProgram* variants = shared->variation.programs;
        active = index < shared->variation.programCount ? &variants[index] : &variants[0];
    }
    shaderMode = activateProgram(active, shaderMode);

    gabi::FrameLocal<sead::ColorChannels> constants(8);
    constants->r.value = 0.5714285969734192f;
    constants->g.value = 0.5001999735832214f;
    constants->b.value = 0.5001000165939331f;
    constants->a.value = 0.5001000165939331f;
    setUniform(uniformAt(active, 0), 4, constants);
    constants->r.value = mHue.mValue / 60.0f;
    f32 inverseGamma = 1.0f / mGamma.mValue;
    constants->g.value = mSaturation.mValue;
    constants->b.value = mBrightness.mValue;
    constants->a.value = inverseGamma;
    setUniform(uniformAt(active, 1), 4, constants);
    setUniform(uniformAt(active, 2), sizeof(mToneTable) / 4, &mToneTable[0]);
    setUniform(uniformAt(active, 3), sizeof(mToneTable) / 4, &mToneTable[1]);
    if (mFlags & kFlagToycam) {
        gabi::FrameLocal<ToycamUniforms_l> toycam(0x28);
        premultiplyColor(&toycam->mLevel1, &mToycamLevel1.mValue, mToycamLevel1.mValue.a.value);
        premultiplyColor(&toycam->mLevel2, &mToycamLevel2.mValue, mToycamLevel2.mValue.a.value);
        // The level colours are uploaded as reciprocals.
        sead::ColorChannels* levels[2] = {&toycam->mLevel1, &toycam->mLevel2};
        for (sead::ColorChannels* level : levels) {
            level->r.value = 1.0f / level->r.value;
            level->g.value = 1.0f / level->g.value;
            level->b.value = 1.0f / level->b.value;
            level->a.value = 1.0f;
        }
        premultiplyColor(&toycam->mMulColor, &mToycamMulColor.mValue, mToycamMulColor.mValue.a.value);
        setUniform(uniformAt(active, 4), 4, &mToycamOffset1.mValue);
        setUniform(uniformAt(active, 5), 4, &mToycamOffset2.mValue);
        setUniform(uniformAt(active, 6), 4, &toycam->mLevel1);
        setUniform(uniformAt(active, 7), 4, &toycam->mLevel2);
        toycam->mSaturation1 = mToycamSaturation1.mValue;
        setUniform(uniformAt(active, 8), 4, &toycam->mSaturation1);
        toycam->mSaturation2 = mToycamSaturation2.mValue;
        setUniform(uniformAt(active, 9), 4, &toycam->mSaturation2);
        toycam->mBrightness = gabi::fnmsubs(1.0f - mToycamBrightness.mValue, 0.5f, 0.5f);
        setUniform(uniformAt(active, 10), 4, &toycam->mBrightness);
        toycam->mContrast = mToycamContrast.mValue;
        setUniform(uniformAt(active, 11), 4, &toycam->mContrast);
        setUniform(uniformAt(active, 12), 4, &toycam->mMulColor);
    }
    gabi::FrameLocal<sead::RenderState> state(0x80);
    createRenderState(state);
    state->mDepthWrite = 0;
    state->mBlendMask = 0;
    state->mDepthTest = 0;
    applyRenderState(state);
    gabi::FrameLocal<sead::Viewport> viewport(0x68);
    createViewport(viewport, &mMapBuffer);
    applyViewport(viewport, &mMapBuffer);
    activateRenderBuffer(&mMapBuffer);
    drawQuad();
    invalidateColorSurface(&mMapSlices[0]);
    mFlags = mFlags & ~kFlagDirty;
    return shaderMode;
}
/* 02784E8C */
u32 ColorCorrection::drawWithSampler(RenderBuffer* target, TextureSampler* source, u32 shaderMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mEnable.mValue == 0) return shaderMode;
    applyCompactRenderState(&mDrawState);
    gabi::FrameLocal<sead::Viewport> viewport(0x18);
    createViewport(viewport, target);
    applyViewport(viewport, target);
    activateRenderBuffer(target);
    ShaderProgram* program = holderProgram(kProgramColorCorrection);
    shaderMode = activateProgram(program, shaderMode);
    gabi::FrameLocal<sead::ColorChannels> lut(8);
    lut->r.value = mLutTransform.mScale;
    lut->g.value = mLutTransform.mOffset;
    lut->b.value = 0.0f;
    lut->a.value = 0.0f;
    setUniform(uniformAt(program, 0), 4, lut);
    bindSampler(source, samplerAt(program, 0), -1, 0);
    bindSampler(&mMapSampler, samplerAt(program, 1), -1, 0);
    drawQuad();
    return shaderMode;
}
/* 02785050 */
u32 ColorCorrection::drawWithTexture(RenderBuffer* target, const TextureData* source, u32 shaderMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mEnable.mValue == 0) return shaderMode;
    if (surfacesMatch(mSourceSampler.texture.surface, source->surface)) refreshSamplerImages(&mSourceSampler, source);
    else copySamplerTexture(&mSourceSampler, source);
    return drawWithSampler(target, &mSourceSampler, shaderMode);
}
// The render buffer's first colour target is the source image.
/* 0278519C */
u32 ColorCorrection::drawRenderBuffer(RenderBuffer* target, u32 shaderMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    return drawWithTexture(target, reinterpret_cast<const TextureData*>(target->mColors[0].get()), shaderMode);
}

// ---- HD: compiler-generated and template code of this translation unit ----
/* 027851A8 */
void LiteralText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 027851BC */
void LiteralText::assureTermination() {}
/* 027851C0 */
void ParameterValueHooks::emptyHook() {}
/* 027851C4 */
u32 BoolParameter::typeCode() { return 0; }
/* 027851CC */
be<u8>* BoolParameter::valueAddress() { return &mValue; }
/* 027851D4 */
be<u8>* BoolParameter::alternateValueAddress() { return &mValue; }
/* 027851DC */
u32 BoolParameter::byteSize() { return sizeof(mValue); }
/* 027851E4 */
u32 FloatParameter::typeCode() { return 1; }
/* 027851EC */
be<f32>* FloatParameter::valueAddress() { return &mValue; }
/* 027851F4 */
be<f32>* FloatParameter::alternateValueAddress() { return &mValue; }
/* 027851FC */
u32 FloatParameter::byteSize() { return sizeof(mValue); }
/* 02785204 */
u32 ColorParameter::typeCode() { return 6; }
/* 0278520C */
sead::ColorChannels* ColorParameter::valueAddress() { return &mValue; }
/* 02785214 */
sead::ColorChannels* ColorParameter::alternateValueAddress() { return &mValue; }
/* 0278521C */
u32 ColorParameter::byteSize() { return sizeof(mValue); }
/* 02785224 */
u32 ParameterObjectHooks::constantOneHook() { return 1; }
/* 0278522C */
void ParameterObjectHooks::emptyHook() {}
/* 02785230 */
u32 ParameterObjectHooks::alternateConstantOneHook() { return 1; }
/* 02785238 */
void ParameterObjectHooks::alternateEmptyHook() {}
/* 0278523C */
u32 ParameterObjectHooks::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    const parameter_tree::MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == (u32)value->mComparisonValue;
}
/* 02785258 */
u32 ParameterNodeHooks::constantOneHook() { return 1; }
/* 02785260 */
u32 ParameterNodeHooks::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    const parameter_tree::MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == (u32)value->mSecondaryValue;
}
/* 0278527C */
void ParameterNodeHooks::trailingEmptyHook() {}
/* 02785280 */
void ParameterNodeHooks::alternateTrailingEmptyHook() {}
/* 02785284 */
void ParameterNodeHooks::finalEmptyHook() {}
/* 02785288 */
f32 ToneCurveParameter::evaluate(u32 index, f32 position) const {
    const CurveDescriptor& descriptor = mDescriptors[index];
    u32 evaluator = sToneCurveEvaluators.mFunctions[descriptor.mKind];
    auto samples = reinterpret_cast<const agl_gx2_support::CurveSamples*>(&descriptor.mKind);
    auto values = descriptor.mValues.get();
    return gabi::tail_ptr<f32>(evaluator, samples, values, position);
}
/* 027852B4 */
void ToneCurveParameter::evaluateAll(sead::ColorChannels* result, f32 position) const {
    gabi::AutoFrame frame{gabi::current_function()};
    be<f32>* channels = reinterpret_cast<be<f32>*>(result);
    for (u32 channel = 0; channel < 4; ++channel) channels[channel] = evaluate(channel, position);
}
/* 0278533C */
void ToneCurveParameter::initializeCurves() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 index = 0; index < 4; ++index) {
        auto values = mCurves[index].mValues;
        moveBytes(values, &sToneCurveDefaults, sizeof(sToneCurveDefaults), 0);
        for (u32 value = 9; value < 30; ++value) values[value] = 1.0f;
        mCurves[index].mKind = 7;
        mCurves[index].mCount = 9;
        mDescriptors[index].mCapacity = 30;
        mDescriptors[index].mKind = 7;
        mDescriptors[index].mCount = 9;
        mDescriptors[index].mValues = values;
    }
}
/* WWHD glue
str 101632E8 "aglccr"
str 101632F0 "enable"
str 101632F8 "有効"
str 101632D4 "hue"
str 10163300 "色相"
str 10163348 "Min=-180, Max=180"
str 101633A0 "saturation"
str 10163308 "彩度"
str 101633AC "Min=0, Max=2"
str 101633BC "brightness"
str 10163310 "明度"
str 101632D8 "gamma"
str 101633C8 "ガンマ補正"
str 10163328 "Min=0.25, Max=4"
str 1016335C "toycam_enable"
str 101633D4 "トイカメラフィルタ"
str 101633E8 "toycam_offset1"
str 101633F8 "トーンオフセット１"
str 1016340C "Min=0, Max=1"
str 1016341C "toycam_offset2"
str 1016342C "トーンオフセット２"
str 1016336C "toycam_level1"
str 10163440 "レベル補正１"
str 1016337C "toycam_level2"
str 10163450 "レベル補正２"
str 10163460 "toycam_saturation1"
str 10163318 "彩度１"
str 10163474 "toycam_saturation2"
str 10163320 "彩度２"
str 1016338C "toycam_brightness"
str 10163338 "toycam_contrast"
str 10163488 "コントラスト"
str 10163498 "toycam_mul_color"
str 101634AC "乗算カラー"
str 101632E0 "level"
str 101634B8 "レベル補正"
str 101634C4 "Icon=COLOR"
str 101634D0 "color_correction"
data sEmptyText 101630D0
data sLiteralTextMethods 101630D4
data sColorCorrectionBaseMethods 101630FC
data sColorCorrectionMethods 1016350C
data sColorCorrectionRootMethods 1016352C
data sParameterObjectBaseMethods 1016310C
data sBoolParameterMethods 1016314C
data sFloatParameterMethods 101631C4
data sColorParameterMethods 1016323C
data sToneCurveDefaults 10163594
data sToneCurveEvaluators 101F96C8
data sCorrectionTextureCache 101F8DD4
data sCorrectionShapeInputs 101F8DDC
data sCorrectionShapes 101F8DE4
data sCoreSlots 104A0BC0
data sColorBlack 104A01CC
data sColorWhite 104A01EC
ext 0273AD10 void* allocate(u32)
ext 0273B0D4 void* allocateFromHeap(u32, void*, u32)
ext 0273AFC8 void freeMapImage(void*)
ext 027A0998 void releaseDebugEntry(ResourceRecord*)
ext 0278C794 void releaseToneCurve(ToneCurveParameter*, u32)
ext 027BE2B0 void destroySampler(TextureSampler*, u32)
ext 027B9694 void destroyRenderBuffer(RenderBuffer*, u32)
ext 0279A07C void createParameterRoot(ParameterRoot*, sead::SafeText*, u32)
ext 027A84B8 void constructParameterObject(parameter_tree::Value*)
ext 0279E674 void initializeParameterBase(void*)
ext 0279E618 void describeParameter(void*, sead::SafeText*, sead::SafeText*, sead::SafeText*, parameter_tree::Value*)
ext 0274FDBC void createCompactRenderState(sead::CompactRenderState*)
ext 027B95B4 void constructRenderBuffer(RenderBuffer*)
ext 028EFFD0 void constructArray(void*, u32, u32, u32)
ext 027B6CB8 ColorSurface* constructColorSurface(ColorSurface*) callback
ext 027BE6B8 void createTextureData(TextureData*)
ext 027BDF7C void createTextureSampler(TextureSampler*)
ext 0278C6E0 void createToneCurve(ToneCurveParameter*, sead::SafeText*, sead::SafeText*, parameter_tree::Value*)
ext 02785CB4 void setRootNodeMeta(void*, sead::SafeText*)
ext 0279CB9C void appendParameterValue(parameter_tree::Node*, parameter_tree::Value*, sead::SafeText*)
ext 027BE930 void configureTextureData(TextureData*, u32, u32, u32, u32, u32, s32, u32, u32)
ext C00088A8 void invalidateDataCache(void*, u32)
ext 025C0928 void assignColorTexture(ColorSurface*, const TextureData*)
ext 027BDEB4 void copySamplerTexture(TextureSampler*, const TextureData*)
ext C0009BB0 u32 currentCoreId()
ext 027BB9E0 u32 refreshProgram(ShaderProgram*, u32)
ext 027B9F68 void setShaderMode(u32)
ext C00060E0 void callDisplayList(void*, u32)
ext 027BB7CC void bindStagePrograms(ShaderProgram*)
ext C0006A40 void setVertexUniform(s32, u32, const void*)
ext C0006908 void setPixelUniform(s32, u32, const void*)
ext 0274D458 void premultiplyColor(sead::ColorChannels*, const sead::ColorChannels*, f32)
ext 02750250 void createRenderState(sead::RenderState*)
ext 02750370 void applyRenderState(sead::RenderState*)
ext 0274F5F0 void createViewport(sead::Viewport*, const RenderBuffer*)
ext 0274F964 void applyViewport(sead::Viewport*, const RenderBuffer*)
ext 0274D690 void activateRenderBuffer(RenderBuffer*)
ext 027BFE5C void bindVertexInput(VertexInputSet*)
ext C0006178 void drawIndexed(u32, u32, u32, const void*, u32, u32)
ext 027B6D64 void invalidateColorSurface(ColorSurface*)
ext 0274FEB0 void applyCompactRenderState(sead::CompactRenderState*)
ext 027BE53C u32 bindSampler(TextureSampler*, const UniformLocations_l*, s32, u32)
ext C0009988 void* moveBytes(void*, const void*, u32, u32)
*/
