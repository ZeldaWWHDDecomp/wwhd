/** HD renderer layer factories; library construction and layer registration stay external. */
#include "hd/hd_render_layers.h"
#include "hd/hd_fader.h"
static hdLayerBase_c* allocateRenderLayer(u32, void *, u32);
static void
registerRenderLayer(hdRenderLayerFactory_c *, hdLayerBase_c *, u32, void *, u32, void *);
static hdLayerBase_c* constructShadowLayer(hdLayerBase_c *);

/* 0272B364 */
hdLayerBase_c *
hdRenderLayerFactory_c::createShadow(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x1BE4, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructShadowLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructSceneLayer(hdLayerBase_c *);

/* 0272B3E0 */
hdLayerBase_c *
hdRenderLayerFactory_c::createScene(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x6C00, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructSceneLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructSceneMaskLayer(hdLayerBase_c *);

/* 0272B45C */
hdLayerBase_c *
hdRenderLayerFactory_c::createSceneMask(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x610, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructSceneMaskLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructUILayer(hdLayerBase_c *);

/* 0272B4D8 */
hdLayerBase_c* hdRenderLayerFactory_c::createUI(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x880, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructUILayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructParticleLayer(hdLayerBase_c *);

/* 0272B554 */
hdLayerBase_c *
hdRenderLayerFactory_c::createParticle(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x1AF4, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructParticleLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructUIForegroundLayer(hdLayerBase_c *);

/* 0272B5D0 */
hdLayerBase_c *
hdRenderLayerFactory_c::createUIForeground(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x880, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructUIForegroundLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructFadeLayer(hdLayerBase_c *);

/* 0272B64C */
hdLayerBase_c* hdRenderLayerFactory_c::createFade(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = sizeof(hdFaderLayer_c), LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructFadeLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructDRCSceneLayer(hdLayerBase_c *);

/* 0272B6C8 */
hdLayerBase_c *
hdRenderLayerFactory_c::createDRCScene(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x880, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructDRCSceneLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructDRCSceneMaskLayer(hdLayerBase_c *);

/* 0272B744 */
hdLayerBase_c *
hdRenderLayerFactory_c::createDRCSceneMask(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x610, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructDRCSceneMaskLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructDRCUILayer(hdLayerBase_c *);

/* 0272B7C0 */
hdLayerBase_c *
hdRenderLayerFactory_c::createDRCUI(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x880, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructDRCUILayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}
static hdLayerBase_c* constructDRCUIForegroundLayer(hdLayerBase_c *);

/* 0272B83C */
hdLayerBase_c *
hdRenderLayerFactory_c::createDRCUIForeground(u32 index, void* owner, u32 flags, void* heap) {
    enum { LAYER_SIZE = 0x92C, LAYER_ALIGNMENT = 4 };
    hdLayerBase_c* layer = allocateRenderLayer(LAYER_SIZE, heap, LAYER_ALIGNMENT);
    if (layer != NULL)
        layer = constructDRCUIForegroundLayer(layer);
    registerRenderLayer(this, layer, index, owner, flags, heap);
    return layer;
}

/* 0272B8B8 */
void hdLayerMatrixBuffer_l::copyMatrices(hdLayerMatrix_l* matrices) {
    enum { MATRIX_COUNT = 10, MATRIX_WORDS = 9 };
    for (u32 i = 0; i < MATRIX_COUNT; i++) {
        hdLayerMatrix_l* destination = NULL;
        if (i < u32(mCount))
            destination = mpMatrices[i];
        for (u32 word = 0; word < MATRIX_WORDS; word++) {
            destination->mValues[word] = matrices[i].mValues[word];
        }
    }
}

static hdShaderBackend_c* sShaderBackend;
static hdShaderBackend_c* sShaderBackendRegistered;
static u8 sShaderBackendDisposerVtable;
static u8 sShaderLocationVtable;
static void constructShaderTextureState(void *);
static void constructShaderVertexState(void *);
static void constructShaderIndexState(void *);
static void destroyShaderTextureState(void *, u32);
static void destroyShaderArchive(void *, u32);
static void destroyShaderIndexState(void *, u32);
static void destroyShaderDisposer(void *, u32);
static void* allocateShaderBackend(u32, void *, s32);
static void constructShaderDisposer(void *, void *, u32);
/* 0272B944 */
static hdShaderBackend_c* hdShaderBackend_construct(hdShaderBackend_c* self) {
    if (self == NULL) {
        self = (hdShaderBackend_c*)operator_new(sizeof(hdShaderBackend_c));
        if (self == NULL)
            return NULL;
    }
    const u32 address = gabi::ea(self);
    self->mArchiveHeader[0] = 0;
    self->mArchiveHeader[1] = 0;
    be<u32>* archivePair = gabi::at<be<u32>>(address + offsetof(hdShaderBackend_c, mArchiveHeader) +
                                             2 * sizeof(be<u32>));
    if (archivePair == NULL)
        archivePair = (be<u32>*)operator_new(8);
    if (archivePair != NULL) {
        archivePair[0] = 0;
        archivePair[1] = 0;
    }
    for (u32 i = 0; i < 7; ++i)
        self->mpPrograms[i] = NULL;
    constructShaderTextureState(self->mTextureState);
    for (u32 i = 0; i < 14; ++i) {
        const u32 locationAddress =
            address + offsetof(hdShaderBackend_c, mLocations) + i * sizeof(hdShaderLocation_l);
        be<u32>* name = gabi::at<be<u32>>(locationAddress);
        if (name == NULL)
            name = (be<u32>*)operator_new(8);
        if (name != NULL) {
            name[0] = gabi::guest_addr("Undefined");
            name[1] = gabi::ea(&sShaderLocationVtable);
        }
        for (u32 j = 0; j < 4; ++j)
            self->mLocations[i].mLocations[j] = 0xFFFF;
    }
    constructShaderVertexState(self->mVertexState);
    constructShaderIndexState(self->mIndexState);
    self->mReady = 0;
    return self;
}
/* 0272BCDC */
hdShaderBackend_c* hdShaderBackend_c::create(void* heap) {
    if (sShaderBackend == NULL) {
        hdShaderBackend_c* backend =
            (hdShaderBackend_c *)allocateShaderBackend(sizeof(hdShaderBackend_c), heap, 4);
        if (backend != NULL) {
            constructShaderDisposer(backend, heap, 3);
            backend->mVtable = gabi::ea(&sShaderBackendDisposerVtable);
        }
        sShaderBackendRegistered = backend;
        if (backend != NULL)
            backend = hdShaderBackend_construct(backend);
        sShaderBackend = backend;
    }
    return sShaderBackend;
}
/* 0272D2E4 */
void hdShaderBackend_c::destroy(u32 flags) {
    if (this != NULL) {
        mVtable = gabi::ea(&sShaderBackendDisposerVtable);
        if (sShaderBackendRegistered == this) {
            hdShaderBackend_c* backend = sShaderBackend;
            sShaderBackendRegistered = NULL;
            destroyShaderIndexState(backend->mIndexState, 2);
            destroyShaderTextureState(backend->mTextureState, 2);
            destroyShaderArchive(backend->mArchiveHeader, 2);
            sShaderBackend = NULL;
        }
        destroyShaderDisposer(this, 0);
        if (flags & 1)
            operator_delete(this);
    }
}
/* 0272D5D4 */
static void __sinit_hd_render_layers_cpp() {
    sinit_header_statics_rodata(0x1049F954, 0x101F86EC, 0x1049F960, 0x10141610);
}

struct hdShaderProgramOwner_l {
    u8 mReserved[0x28];
    gptr<void> mpProgram;
};
struct hdShaderProgramState_l {
    be<u8> mFlags;
    u8 mPadding[3];
    gptr<void> mpBinary;
    u8 mReserved[4];
    gptr<void> mpCompiled;
    u8 mBeforeOwner[0x6C];
    gptr<hdShaderProgramOwner_l> mpOwner;
};
struct hdShaderProgram_l {
    gptr<hdShaderProgramState_l> mpState;
    u8 mReserved[0x10];
    be<u32> mSamplerCount;
    gptr<u8> mpSamplers;
    u8 mBeforeReady[4];
    be<u8> mReady;
    u8 mTail[3];
};
struct hdShaderState_l {
    be<u8> mDepthTest;
    be<u8> mDepthWrite;
    u8 mPad[2];
    be<u32> mCompare;
    u8 mBeforeBlend[4];
    be<u8> mBlend;
    u8 mBeforeColor[0x2B];
    be<u8> mCull;
    u8 mBeforeMask[0xB];
    be<u8> mColorMask[4];
    u8 mTail[0x28];
    be<u32> mVtable;
};
WWHD_OFFSET(hdShaderState_l, mColorMask, 0x44);
WWHD_SIZE(hdShaderState_l, 0x74);
struct hdShaderDraw_l {
    be<u32> mPrimitive;
    be<u32> mStart;
    be<u32> mCount;
    be<u32> mInstances;
};
static hdShaderDraw_l sShaderDraw;
struct hdShaderUniform_l {
    be<f32> mValues[4];
};
static hdShaderUniform_l sShaderDefaultColor;
static void rebuildShaderProgram(hdShaderProgramState_l *, u32);
static void bindShaderProgram(void *);
static void bindShaderBinary(void *, void *);
static void bindShaderState(hdShaderProgramState_l *);
static void setShaderVertexUniform(s32, u32, const void *);
static void setShaderPixelUniform(s32, u32, const void *);
static void bindShaderTexture(void *, void *, s32, u32);
static void constructShaderState(hdShaderState_l *);
static void applyShaderState(hdShaderState_l *);
static void bindShaderTextureState(void *);
static void drawShaderVertices(u32, u32, u32, u32, u32, u32);
static void* prepareBackendProgram(hdShaderProgram_l* program, void* previous) {
    hdShaderProgramState_l* state = program->mpState;
    if (u8(state->mFlags) & 2) {
        state->mFlags = u8(state->mFlags) & ~2;
        rebuildShaderProgram(state, 0);
    }
    void* bound = state->mpOwner->mpProgram;
    if (previous != bound)
        bindShaderProgram(bound);
    if (state->mpCompiled != NULL)
        bindShaderBinary(state->mpBinary, state->mpCompiled);
    else
        bindShaderState(state);
    return bound;
}
static void setBackendUniform(hdShaderLocation_l* location, u32 count, const void* values) {
    s32 vertex = s16(u16(location->mLocations[0]));
    if (vertex != -1)
        setShaderVertexUniform(vertex, count, values);
    s32 pixel = s16(u16(location->mLocations[1]));
    if (pixel != -1)
        setShaderPixelUniform(pixel, count, values);
}
static void bindBackendTexture(hdShaderProgram_l* program, void* texture) {
    u32 samplerAddress = 0;
    if (u32(program->mSamplerCount) != 0)
        samplerAddress = gabi::ea((u8 *)program->mpSamplers);
    bindShaderTexture(texture, gabi::at<void>(samplerAddress + 4), -1, 0);
}
static void finishBackendDraw(hdShaderBackend_c* backend) {
    bindShaderTextureState(backend->mTextureState);
    if (sShaderDraw.mInstances)
        drawShaderVertices(sShaderDraw.mStart,
                           sShaderDraw.mInstances,
                           sShaderDraw.mPrimitive,
                           sShaderDraw.mCount,
                           0,
                           1);
}
static void setupBackendDraw(
    bool depth, bool depthWrite, s32 blend, s32 cull, bool red, bool green, bool blue, bool alpha) {
    gabi::Local<hdShaderState_l> stateStorage;
    hdShaderState_l &state = *stateStorage.get();
    constructShaderState(&state);
    state.mCompare = 7;
    state.mDepthTest = depth;
    state.mDepthWrite = depthWrite;
    if (blend >= 0)
        state.mBlend = blend;
    if (cull >= 0)
        state.mCull = cull;
    state.mColorMask[0] = red;
    state.mColorMask[1] = green;
    state.mColorMask[2] = blue;
    state.mColorMask[3] = alpha;
    applyShaderState(&state);
}
/* 0272C4E0 */
void* hdShaderBackend_c::prepareDepth(void* texture, void* previous, f32 nearClip, f32 farClip) {
    void* bound = prepareBackendProgram(mpPrograms[0], previous);
    hdShaderUniform_l values;
    values.mValues[0] = nearClip / (farClip - nearClip);
    values.mValues[1] = 1.0f - nearClip / farClip;
    values.mValues[2] = 0;
    values.mValues[3] = 0;
    setBackendUniform(&mLocations[0], 4, &values);
    bindBackendTexture(mpPrograms[0], texture);
    setupBackendDraw(true, true, false, false, true, false, false, false);
    finishBackendDraw(this);
    return bound;
}
/* 0272C6C0 */
void* hdShaderBackend_c::prepareColor(
    void* texture, void* unused, void* color, void* transform, void* previous) {
    void* bound = prepareBackendProgram(mpPrograms[1], previous);
    setBackendUniform(&mLocations[1], 4, transform);
    setBackendUniform(&mLocations[2], 4, color);
    setBackendUniform(&mLocations[3], 4, &sShaderDefaultColor);
    bindBackendTexture(mpPrograms[1], texture);
    setupBackendDraw(true, true, -1, -1, false, false, false, false);
    finishBackendDraw(this);
    return bound;
}
/* 0272CAE0 */
void* hdShaderBackend_c::prepareMatrix(void* texture, void* matrix, void* previous) {
    void* bound = prepareBackendProgram(mpPrograms[6], previous);
    setBackendUniform(&mLocations[13], 12, matrix);
    bindBackendTexture(mpPrograms[6], texture);
    setupBackendDraw(true, false, false, -1, true, true, true, false);
    finishBackendDraw(this);
    return bound;
}

struct hdShaderTextureDimensions_l {
    u8 mReserved[8];
    be<s32> mWidth;
    be<s32> mHeight;
    u8 mBeforeLimits[0x68];
    be<u8> mWidthLimit;
    be<u8> mHeightLimit;
};
struct hdShaderScale_l {
    be<f32> x, y;
    be<u32> mConversionExponent, mHeightBits;
};
WWHD_SIZE(hdShaderScale_l, 0x10);
struct hdShaderTextureMatrix_l {
    be<f32> mValues[16];
};
struct hdShaderImageSize_l {
    u8 mReserved[8];
    be<f32> mWidth;
    be<f32> mHeight;
};
/* 0272C898 */
void* hdShaderBackend_c::prepareScaled(
    void* texture, void* size, void* color, void* transform, void* previous) {
    void* bound = prepareBackendProgram(mpPrograms[2], previous);
    setBackendUniform(&mLocations[4], 4, transform);
    setBackendUniform(&mLocations[5], 4, color);
    hdShaderTextureDimensions_l* dimensions = (hdShaderTextureDimensions_l *)texture;
    hdShaderImageSize_l* imageSize = (hdShaderImageSize_l *)size;
    s32 width = dimensions->mWidth;
    if (s32(u8(dimensions->mWidthLimit)) > width)
        width = u8(dimensions->mWidthLimit);
    s32 height = u8(dimensions->mHeightLimit);
    if (s32(dimensions->mHeight) > height)
        height = dimensions->mHeight;
    hdShaderScale_l scale;
    scale.x = f32(imageSize->mWidth) / f32(u32(width));
    scale.y = f32(imageSize->mHeight) / f32(u32(height));
    // The original uploads the adjacent integer-to-double conversion words too.
    scale.mConversionExponent = 0x43300000;
    scale.mHeightBits = u32(height);
    setBackendUniform(&mLocations[6], 4, &scale);
    bindBackendTexture(mpPrograms[2], texture);
    setupBackendDraw(true, false, false, -1, true, true, true, false);
    finishBackendDraw(this);
    return bound;
}
/* 0272CC50 */
static void* prepareBackendTransform(
    hdShaderBackend_c* self, void* texture, void* matrix, void* color, void* previous) {
    void* bound = prepareBackendProgram(self->mpPrograms[3], previous);
    setBackendUniform(&self->mLocations[7], 4, color);
    setBackendUniform(&self->mLocations[8], 12, matrix);
    bindBackendTexture(self->mpPrograms[3], texture);
    setupBackendDraw(true, false, true, -1, true, true, true, false);
    finishBackendDraw(self);
    return bound;
}
static void makeBackendTextureMatrix(void *, void *, void *);
/* 0272CDF4 */
static void* prepareBackendTextureTransform(hdShaderBackend_c* self,
                                            void* texture,
                                            void* scale,
                                            void* transform,
                                            void* color,
                                            u32 mode,
                                            void* previous) {
    void* bound = prepareBackendProgram(self->mpPrograms[4], previous);
    hdShaderTextureMatrix_l matrix;
    makeBackendTextureMatrix(&matrix, transform, scale);
    setBackendUniform(&self->mLocations[9], 16, &matrix);
    setBackendUniform(&self->mLocations[10], 4, color);
    gabi::Local<be<u32>> option;
    *option.get() = mode;
    setBackendUniform(&self->mLocations[11], 4, option.get());
    bindBackendTexture(self->mpPrograms[4], texture);
    finishBackendDraw(self);
    return bound;
}
/* 0272CF9C */
static void* prepareBackendCoordinates(
    hdShaderBackend_c* self, void* texture, cXy* position, void* previous, f32 scale, f32 divisor) {
    void* bound = prepareBackendProgram(self->mpPrograms[5], previous);
    hdShaderUniform_l values;
    values.mValues[0] = position->x;
    values.mValues[1] = position->y;
    values.mValues[2] = scale;
    values.mValues[3] = 1.0f / divisor;
    setBackendUniform(&self->mLocations[12], 4, &values);
    bindBackendTexture(self->mpPrograms[5], texture);
    setupBackendDraw(true, false, false, -1, true, true, true, true);
    finishBackendDraw(self);
    return bound;
}

struct hdShaderMaterialRecord_l {
    be<u32> mWords[9];
};
struct hdShaderMaterialTable_l {
    be<u16> mCount;
    u8 mPadding[2];
    gptr<hdShaderMaterialRecord_l> mpRecords;
    be<u32> mFlags[4];
    be<u32> mMask[2];
};
struct hdShaderModelResource_l {
    u8 mReserved[0x30];
    gptr<hdShaderMaterialTable_l> mpTable;
    gptr<void> mpNames;
};
static hdShaderMaterialRecord_l* sShaderMaterialA;
static hdShaderMaterialRecord_l* sShaderMaterialB;
static hdShaderModelResource_l* getShaderModelResource(void *);
static be<u8> *getShaderMaterialName(void *, u32);
static u32 shaderShiftLeft(u32 value, u32 amount) {
    amount &= 63;
    return amount < 32 ? value << amount : 0;
}
static u32 shaderShiftRight(u32 value, u32 amount) {
    amount &= 63;
    return amount < 32 ? value >> amount : 0;
}
/* 0272D384 */
static hdShaderModelResource_l* replaceBackendMaterials(void* resource) {
    hdShaderModelResource_l* model = getShaderModelResource(resource);
    hdShaderMaterialTable_l* table = model->mpTable;
    void* names = model->mpNames;
    if (table != NULL && names != NULL) {
        for (u32 index = 0; index < u16(table->mCount); index = u16(index + 1)) {
            be<u8> *name = getShaderMaterialName(names, index);
            if (name == NULL || u8(name[0]) != 'Z')
                continue;
            hdShaderMaterialRecord_l* replacement = NULL;
            if (u8(name[1]) == 'A')
                replacement = sShaderMaterialA;
            else if (u8(name[1]) == 'B')
                replacement = sShaderMaterialB;
            else
                continue;
            hdShaderMaterialRecord_l* records = table->mpRecords;
            for (u32 word = 0; word < 9; ++word)
                records[index].mWords[word] = replacement->mWords[word];
            records = table->mpRecords;
            records[index].mWords[7] =
                u32(records[index].mWords[7]) + gabi::ea(replacement) - gabi::ea(&records[index]);
            records = table->mpRecords;
            records[index].mWords[3] =
                u32(records[index].mWords[3]) + gabi::ea(replacement) - gabi::ea(&records[index]);
            records = table->mpRecords;
            records[index].mWords[8] = replacement->mWords[8];
            u32 high = table->mMask[0], low = table->mMask[1];
            u32 shift = index < 64 ? index : index - 64;
            u32 highMask = shaderShiftLeft(high, shift) | shaderShiftRight(low, 32 - shift) |
                           shaderShiftLeft(low, shift + 32);
            u32 lowMask = shaderShiftLeft(low, shift);
            u32 flag = index < 64 ? 0 : 2;
            table->mFlags[flag] = u32(table->mFlags[flag]) | highMask;
            table->mFlags[flag + 1] = u32(table->mFlags[flag + 1]) | lowMask;
        }
    }
    return model;
}
static void* sShaderResourceManager;
static void* sShaderGraphicsContext;
static void* loadShaderModel(void *, SafeString *, SafeString *, u32);
static void beginShaderModelLoad(void *);
static void endShaderModelLoad(void *);
static void initializeShaderVertexState(void *, void *, const char *);
static void copyShaderIndexState(void *, void *);
/* 0272D16C */
static void loadBackendModel(hdShaderBackend_c* self) {
    if (!self->mReady) {
        SafeString folder;
        SafeString file;
        folder.__vtbl = gabi::ea(&sShaderLocationVtable);
        folder.mStringTop = gabi::guest_addr("Particle");
        file.__vtbl = gabi::ea(&sShaderLocationVtable);
        file.mStringTop = gabi::guest_addr("Particle.bfres");
        void* resource = loadShaderModel(sShaderResourceManager, &folder, &file, 0);
        beginShaderModelLoad(sShaderGraphicsContext);
        initializeShaderVertexState(self->mVertexState, resource, (const char *)"zel_tingle");
        endShaderModelLoad(sShaderGraphicsContext);
        bool equal = true;
        for (u32 i = 1; i <= 7; ++i)
            if (u32(self->mVertexState[i]) != u32(self->mIndexState[i]))
                equal = false;
        if (u32(self->mVertexState[14]) != u32(self->mIndexState[14]))
            equal = false;
        if (u32(self->mVertexState[13]) != u32(self->mIndexState[13]))
            equal = false;
        if (!equal)
            copyShaderIndexState(self->mIndexState, self->mVertexState);
        else {
            self->mIndexState[10] = self->mVertexState[10];
            self->mIndexState[55] = self->mVertexState[12];
            self->mIndexState[53] = self->mVertexState[10];
            self->mIndexState[12] = self->mVertexState[12];
        }
    }
    self->mReady = 1;
}

struct hdShaderResourceArray_l {
    u8 mReserved[0x10];
    be<u32> mCount;
    gptr<u8> mpEntries;
};
static hdShaderResourceArray_l* sShaderResources;
struct hdShaderArchiveProgram_l {
    u8 mReserved[0x1C];
    be<u32> mCount;
    gptr<u8> mpEntries;
};
static u8 sShaderBindingState;
static void initializeShaderArchive(void *, void *, void *);
static s32 findShaderProgram(hdShaderArchiveProgram_l *, SafeString *);
static void initializeShaderProgram(hdShaderProgram_l *, void *, u32);
static void initializeShaderLocation(hdShaderLocation_l *, hdShaderProgramState_l *);
static void initializeShaderBindings(hdShaderProgram_l *, void *, void *, void *);
/* 0272BD70 */
void initializeBackendShaders(hdShaderBackend_c* self, void* heap) {
    void* resource = NULL;
    if (u32(sShaderResources->mCount) > 2)
        resource = (u8 *)sShaderResources->mpEntries + 0xB8;
    initializeShaderArchive(self->mArchiveHeader, resource, heap);
    const char* programNames[] = {"draw_linear_depth",
                                  "copy_depth_texcoord",
                                  "copy_color_texcoord",
                                  "draw_tact_blur",
                                  "draw_circle_wipe",
                                  "draw_mosaic_wipe",
                                  "draw_mtx_texcoord"};
    for (u32 program = 0; program < 7; ++program) {
        hdShaderArchiveProgram_l* archive =
            gabi::at<hdShaderArchiveProgram_l>(self->mArchiveHeader[1]);
        SafeString name;
        name.__vtbl = gabi::ea(&sShaderLocationVtable);
        name.mStringTop = gabi::guest_addr(programNames[program]);
        s32 index = findShaderProgram(archive, &name);
        hdShaderProgram_l* selected = NULL;
        if (index >= 0) {
            hdShaderProgram_l* programs = gabi::at<hdShaderProgram_l>(self->mArchiveHeader[3]);
            u32 count = self->mArchiveHeader[2];
            hdShaderProgram_l* entry = &programs[u32(index) < count ? index : 0];
            struct Entry {
                u8 mReserved[0x20];
                be<u8> mReady;
            };
            if (!((Entry *)entry)->mReady) {
                archive = gabi::at<hdShaderArchiveProgram_l>(self->mArchiveHeader[1]);
                void* input = NULL;
                if (u32(index) < u32(archive->mCount))
                    input = (u8 *)archive->mpEntries + u32(index) * 0x84;
                initializeShaderProgram(entry, input, 0);
            }
            programs = gabi::at<hdShaderProgram_l>(self->mArchiveHeader[3]);
            count = self->mArchiveHeader[2];
            selected = &programs[u32(index) < count ? index : 0];
        }
        self->mpPrograms[program] = selected;
    }
    beginShaderModelLoad(sShaderGraphicsContext);
    const char* uniformNames[] = {"cLinearDepth_Param",
                                  "uTexTrans",
                                  "uTexScale",
                                  "uTexSize",
                                  "uTexTrans",
                                  "uTexScale",
                                  "uTexSize",
                                  "uBlendColor",
                                  "uTexMtx",
                                  "uProjViewWorld",
                                  "uTexScale",
                                  "uPos",
                                  "cMosaicWipe_Param",
                                  "uTexMtx"};
    const u32 owners[] = {0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 6};
    for (u32 uniform = 0; uniform < 14; ++uniform) {
        self->mLocations[uniform].mName = gabi::guest_addr(uniformNames[uniform]);
        initializeShaderLocation(&self->mLocations[uniform],
                                 self->mpPrograms[owners[uniform]]->mpState);
    }
    endShaderModelLoad(sShaderGraphicsContext);
    initializeShaderBindings(self->mpPrograms[0], self->mTextureState, &sShaderBindingState, heap);
}
/* WWHD glue
 str 101414E4 "uPos"
 data sShaderResources 101F8A94 nocheck
 str 10141520 "draw_linear_depth"
 str 101414EC "copy_depth_texcoord"
 str 10141500 "copy_color_texcoord"
 str 10141574 "draw_tact_blur"
 str 10141584 "draw_circle_wipe"
 str 10141598 "draw_mosaic_wipe"
 str 10141534 "draw_mtx_texcoord"
 str 101415AC "cLinearDepth_Param"
 str 10141548 "uTexTrans"
 str 10141554 "uTexScale"
 str 101415C0 "uTexSize"
 str 10141514 "uBlendColor"
 str 101414DC "uTexMtx"
 str 101415CC "uProjViewWorld"
 str 10141560 "cMosaicWipe_Param"
 data sShaderBindingState 104B4D30 nocheck
 ext 028010F8 void initializeShaderArchive(void*,void*,void*)
 ext 027B90AC s32 findShaderProgram(hdShaderArchiveProgram_l*,SafeString*)
 ext 02800B0C void initializeShaderProgram(hdShaderProgram_l*,void*,u32)
 ext 027BD3CC void initializeShaderLocation(hdShaderLocation_l*,hdShaderProgramState_l*)
 ext 027FEFE0 void initializeShaderBindings(hdShaderProgram_l*,void*,void*,void*)
 data sShaderResourceManager 101F4F7C nocheck
 data sShaderGraphicsContext 101F8B18 nocheck
 str 101415E8 "Particle"
 str 101415F4 "Particle.bfres"
 str 10141604 "zel_tingle"
 ext 026124B0 void* loadShaderModel(void*,SafeString*,SafeString*,u32)
 ext 0274FBF8 void beginShaderModelLoad(void*)
 ext 0274FCCC void endShaderModelLoad(void*)
 ext 02773870 void initializeShaderVertexState(void*,void*,const char*)
 ext 027BDEB4 void copyShaderIndexState(void*,void*)
 data sShaderMaterialA 10475800 nocheck
 data sShaderMaterialB 10475804 nocheck
 ext 027F7B50 hdShaderModelResource_l* getShaderModelResource(void*)
 ext 027ED1F0 be<u8>* getShaderMaterialName(void*,u32)
 ext 0272D680 void makeBackendTextureMatrix(void*,void*,void*)
 data sShaderDraw 104B4CFC nocheck
 data sShaderDefaultColor 104A0920 nocheck
 ext 027BB9E0 void rebuildShaderProgram(hdShaderProgramState_l*,u32)
 ext 027B9F68 void bindShaderProgram(void*)
 ext C00060E0 void bindShaderBinary(void*,void*)
 ext 027BB7CC void bindShaderState(hdShaderProgramState_l*)
 ext C0006A40 void setShaderVertexUniform(s32,u32,const void*)
 ext C0006908 void setShaderPixelUniform(s32,u32,const void*)
 ext 027BE53C void bindShaderTexture(void*,void*,s32,u32)
 ext 0274FDBC void constructShaderState(hdShaderState_l*)
 ext 0274FEB0 void applyShaderState(hdShaderState_l*)
 ext 027BFE5C void bindShaderTextureState(void*)
 ext C0006178 void drawShaderVertices(u32,u32,u32,u32,u32,u32)
 data sShaderBackend 101F8710 nocheck
 data sShaderBackendRegistered 101F8714 nocheck
 data sShaderBackendDisposerVtable 10141618 nocheck
 data sShaderLocationVtable 101414B8 nocheck
 str 101414D0 "Undefined"
 ext 027BF734 void constructShaderTextureState(void*)
 ext 027BE6B8 void constructShaderVertexState(void*)
 ext 027BDF7C void constructShaderIndexState(void*)
 ext 027BE2B0 void destroyShaderIndexState(void*,u32)
 ext 027BF880 void destroyShaderTextureState(void*,u32)
 ext 02801024 void destroyShaderArchive(void*,u32)
 ext 02752BEC void destroyShaderDisposer(void*,u32)
 ext 0273B0D4 void* allocateShaderBackend(u32,void*,s32)
 ext 02752B0C void constructShaderDisposer(void*,void*,u32)
 ext 0273B050 hdLayerBase_c* allocateRenderLayer(u32, void*, u32)
 ext 027C2060 void registerRenderLayer(hdRenderLayerFactory_c*, hdLayerBase_c*, u32, void*, u32, void*)
 ext 02735F0C hdLayerBase_c* constructShadowLayer(hdLayerBase_c*)
 ext 0272FB18 hdLayerBase_c* constructSceneLayer(hdLayerBase_c*)
 ext 02733994 hdLayerBase_c* constructSceneMaskLayer(hdLayerBase_c*)
 ext 0272DD2C hdLayerBase_c* constructUILayer(hdLayerBase_c*)
 ext 027354A4 hdLayerBase_c* constructParticleLayer(hdLayerBase_c*)
 ext 0272F644 hdLayerBase_c* constructUIForegroundLayer(hdLayerBase_c*)
 ext 02734FC8 hdLayerBase_c* constructFadeLayer(hdLayerBase_c*)
 ext 027346A8 hdLayerBase_c* constructDRCSceneLayer(hdLayerBase_c*)
 ext 02734B68 hdLayerBase_c* constructDRCSceneMaskLayer(hdLayerBase_c*)
 ext 0272E8CC hdLayerBase_c* constructDRCUILayer(hdLayerBase_c*)
 ext 0272EE3C hdLayerBase_c* constructDRCUIForegroundLayer(hdLayerBase_c*)
*/
