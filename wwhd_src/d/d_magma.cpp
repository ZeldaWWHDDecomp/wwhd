/* Magma balls and surfaces, with the verified HD resource and rendering path.
 * Verification preserves GameCube simulation names and HD call/evaluation order.
 */
#include "d/d_magma.h"
#include "d/d_path.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gabi.h"
using namespace gabi;
using MagmaBall = dMagma::dMagma_ball_c;
using MagmaBallPath = dMagma::dMagma_ballPath_c;
using MagmaFloor = dMagma::dMagma_floor_c;
using MagmaRoom = dMagma::dMagma_room_c;
using MagmaPacket = dMagma::dMagma_packet_c;
using MagmaArrayConstructor = void *(*)(void *);
using MagmaArrayDestructor = void (*)(void *, u32);

static constexpr MagmaTuVtables magmaTu = {0x100507AC, 0x100507C4, 0x100446B8, 0x100509D0,
                                           0x100509F8, 0x1016EF84, 0x1016EFB4};

static MagmaFloat magmaZero;
static MagmaFloat magmaRangeRadius;
static MagmaFloat magmaSphereRadius;
static MagmaFloat magmaSphereBase;
static MagmaFloat magmaOne;
static MagmaFloat magmaWaveAmplitude;
static MagmaFloat magmaBaseVariation;
static MagmaFloat magmaWaveVariation;
static MagmaFloat magmaWaveScale;
static MagmaFloat magmaMinimumY;
static MagmaFloat magmaFloorWidth;
static MagmaFloat magmaFloorHeight;

static MagmaSinCosTable magmaSinCos;
using MagmaPoint = path_layout::dPnt;
using MagmaPath = path_layout::dPath;
static dMagma::Mtx magmaViewMtx;
static dMagma::Mtx magmaMatrixStack;
f32 magmaSquareDistance(cXyz *, cXyz *);
f32 magmaSqrt(f32);
s32 magmaCalcTimer(be<u8> *);
void magmaConcat(void *, void *, void *);
void magmaPrepareBall(MagmaBall *);
MagmaPath *magmaGetRoomPath(u32, s32);
void magmaAssert(const char *, s32, const char *);
f32 magmaRandom(f32);
f32 magmaRandomSigned(f32);
void magmaTranslate(void *, f32, f32, f32);
void magmaScaleM(f32, f32, f32);
void magmaCopyMatrix(void *, void *);

static MagmaDrawBufferPointer magmaOpaDrawBuffer;
static GXColor magmaColor;
static MagmaFloat magmaBallDensity;
void magmaPrepareFloor(MagmaFloor *);
void magmaRemoveFloor(MagmaFloor *);
void magmaEntryPacket(void *, MagmaPacket *, u32);
void magmaDrawFloor(MagmaFloor *, void *);
void *magmaCreateFloor(MagmaFloor *, cXyz *, cXyz *, s32, u32, s32);

void magmaDestroyEmpty(void *self, u32 flag);
void *magmaConstructColorVector(void *self);
void magmaDestroyEmbeddedBuffer(void *self, u32 flag);
void *magmaConstructRoom(void *self);
void magmaDestroyRoom(void *self, u32 flag);
void magmaStaticInit();
void magmaCopyMatrixWords(void *out, void *in);
f32 magmaLinearColorChannel(u32 value);
void magmaConvertColor(void *out, void *color);
void magmaModuleInit();
void *magmaConstructPacket(void *self);
void magmaDestroyPacket(void *self, u32 flag);
void magmaDestroyResource(void *self, u32 flag);
void *magmaConstructBossResource(void *self);
void *magmaConstructPathResource(void *self);
void *magmaConstructFloor(void *self);
void magmaDestroyFloor(void *self, u32 flag);
void magmaDestroyBoss(void *self, u32 flag);
void magmaDestroyPath(void *self, u32 flag);
void magmaMountObjectResource(const char *, u32, u32);
void magmaDeleteObjectResource(const char *);
void magmaSetupRenderObject(void *);
f32 magmaMorfCalc(f32, f32, f32);
void magmaDrawBallGuest(void *, void *);
void magmaCreateBallResourcesGuest(void *);
void magmaCopyMatrixWordsGuest(void *, void *);
f32 magmaLinearColorChannelGuest(u32);
void magmaConvertColorGuest(void *, void *);
void magmaCalcFloorGuest(void *, u32);
void magmaUpdateFloorGuest(void *);
void magmaCreateFloorResourcesGuest(void *);
void magmaReleaseFloorsGuest(void *, u32);
void magmaReleaseAllocation(void *);
void magmaConstructAllocator(void *, const char *, u32, u32);
void magmaDestroyAllocator(void *, u32);
void magmaCancelAllocator(void *);
void magmaRotateX(void *, u32);
void magmaTranslateM(f32, f32, f32);
void *magmaGetResource(void *, void *, u32);
void *magmaNew(u32);
void *magmaNewArray(u32);
void magmaDelete(void *);
void magmaDeleteArray(void *);
void magmaLockResources(void *);
void magmaUnlockResources(void *);
void magmaConstructGraphicsState(void *);
void magmaApplyGraphicsState(void *);
void *magmaFindHeap(void *, void *);
void *magmaGetCurrentHeap(void *);
void magmaLoadTextureHandle(void *, void *);
void magmaConstructIndexBuffer(void *);
void magmaDestroyIndexBuffer(void *, u32);
void magmaSetIndexBuffer(void *, void *, u32, u32);
void magmaConstructVertexBuffer(void *);
void magmaDestroyVertexBuffer(void *, u32);
void magmaFlushVertexBuffer(void *, u32, u32);
s32 magmaFindShader(void *, void *);
void magmaBindCompiledShader(void *);
void magmaBindShader(void *);
void magmaUpdateShader(void *, u32);
void magmaCopyTextureSampler(void *, void *);
void magmaConstructTextureSampler(void *);
void magmaDestroyTextureSampler(void *, u32);
void magmaBindTextureSampler(void *, void *, s32, u32);
void magmaConstructTextureHandle(void *);
void magmaReleaseRenderBuffer(void *);
void magmaConstructResourceHandle(void *);
void magmaReleaseResourceHandle(void *);
void magmaDestroyResourceHandle(void *, u32);
void magmaBindResourceHandle(void *);
void magmaConstructBasePacket(void *);
void magmaDestroyBasePacket(void *, u32);
void *magmaGetRenderCache(void *, void *);
void magmaConstructRenderObject(void *);
void magmaDestroyRenderObject(void *, u32);
void magmaInvalidateRenderObject(void *);
void magmaConstructShapeArray(void *);
void magmaDestroyShapeArray(void *, u32);
void magmaPrepareShapeArray(void *, u32, void *, void *, void *);
void magmaUpdateShapeArray(void *, u32);
void magmaResizeShapeArray(void *, u32, u32);
void magmaSetVertexBuffer(void *, void *, u32, void *);
void magmaSetMaterialResources(void *, void *, void *, void *, u32);
void *magmaGetResourceArchive(void *);
void magmaSetContextMaterial(void *, void *);
void magmaSetGraphicsMode(u32, void *);
void magmaSetShader(void *, void *, u32);
void magmaLoadShader(void *, void *, u32);
void magmaIdentity(void *);
void magmaInverse(void *, void *);
void magmaScale(void *, f32, f32, f32);
void magmaLightOrtho(void *, f32, f32, f32, f32, f32, f32, f32, f32);
void magmaInitializeModuleByte(void *);
void magmaInitializeModuleFlag(void *);
void magmaConstructArray(void *, u32, u32, MagmaArrayConstructor);
void magmaDestroyArray(void *, u32, u32, MagmaArrayDestructor, u32, u32);
void magmaRegisterModuleObject(void *);
f32 magmaPow(f32, f32);
void magmaClear(void *, u32);
void magmaSubmitShaderCommands(void *, u32);
void magmaDrawIndexed(u32, void *, u32, u32, u32, u32);
void magmaBindGeometryBuffer(s32, u32, u32);
void magmaBindVertexBuffer(s32, u32, u32);
void magmaBindIndexBuffer(s32, u32, u32);

static MagmaFloat magmaSamplerParameter;
static MagmaFloat magmaColorMaximum;
static MagmaFloat magmaColorExponent;
static MagmaFloat magmaProjectionNear;
static MagmaFloat magmaFloorProjectionScaleY;
static MagmaFloat magmaOrdinaryFloorOffset;
static MagmaFloat magmaFloorSpawnOffset;
static MagmaFloat magmaProjectionSpan;
static MagmaFloat magmaProjectionFar;
static MagmaFloat magmaTextureScrollSpeed;
static MagmaFloat magmaTextureScrollLimit;
static MagmaFloat magmaTextureScaleX;
static MagmaFloat magmaTextureScaleY;
static MagmaFloat magmaTextureScaleZ;
static MagmaFloat magmaColorPeriod;
static MagmaFloat magmaInitialModuleX;
static MagmaFloat magmaInitialModuleY;
static MagmaFloat magmaConstructorOne;
static MagmaFloat magmaConstructorZero;
static dMagma::Mtx l_kuroOrthoMtx;
static dMagma::Mtx l_colOrthoMtx;
static dMagma::Mtx magmaKuroMtx;
static dMagma::Mtx magmaFloorMtx;
static dMagma::Mtx magmaBallMtx;
static MagmaWord magmaBallStripCount;
static MagmaWord magmaBallStripVertices;
static MagmaWord magmaBallVertexCount;
static MagmaDrawBufferPointer magmaResourceControl;
static MagmaDrawBufferPointer magmaResourceMutex;
static MagmaDrawBufferPointer magmaRootHeap;

static MagmaModuleState magmaModuleState;

static MagmaPackedState magmaPackedState;
// HD: GHS static-destructor registration records.
static MagmaDestructorRecord magmaDestructorRecords0;
static MagmaDestructorRecord magmaDestructorRecords1;
static MagmaDestructorRecord magmaDestructorRecords2;

static void magmaReleaseResourceBuffers(dMagma::HdResources *);

static MagmaKeyColorTable magmaKeyColors;

static void *magmaConstructResource(void *self) {
    if (!self)
        self = magmaNew(sizeof(dMagma::HdResourceBuffer));
    if (!self)
        return self;
    auto *resource = static_cast<dMagma::HdResourceBuffer *>(self);
    magmaConstructVertexBuffer(&resource->mVertexBuffer);
    magmaConstructResourceHandle(&resource->mResource);
    resource->mpStorage = nullptr;
    resource->mCapacity = 0;
    return self;
}

static void magmaResolveString(MagmaStringRef *ref) {
    auto *table = at<MagmaStringVtable>(ref->vtable);
    gabi::call_ptr<void>(table->resolve.get(), ref);
}
static bool magmaSameString(MagmaStringRef *a, MagmaStringRef *b) {
    u32 first = a->value, second = b->value;
    if (first == second)
        return true;
    auto *lhs = at<be<u8>>(first);
    auto *rhs = at<be<u8>>(second);
    // HD: retain the verified bounded comparison even for unterminated strings.
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = lhs[i], y = rhs[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}

static void magmaFreeField(be<u32> *field) {
    auto *heap = static_cast<MagmaHeap *>(magmaFindHeap(magmaRootHeap.value.get(), at<void>(*field)));
    auto *table = heap->__vtbl.get();
    gabi::call_ptr<void>(table->free.get(), heap, at<void>(*field));
}

static void magmaClearGraphicsArray(dMagma::HdResourceArray *array) {
    if (!array->mpItems.get())
        return;
    for (s32 i = 0; i < s32(array->mCount); ++i) {
        auto *item = array->mpItems.get() + i;
        auto *table = item->__vtbl.get();
        gabi::call_ptr<void>(table->destroy.get(), item, u32(2));
    }
    magmaFreeField(&array->mpItems.v);
    array->mCount = 0;
    array->mpItems = nullptr;
}

// HD: compiler-inlined ownership cleanup of the two per-material resource arrays.
static void magmaReleaseMaterials(dMagma::HdMaterialResources *material) {
    if (!material->mpMaterials.get())
        return;
    for (s32 i = 0; i < s32(material->mMaterialCapacity); ++i) {
        auto *item = material->mpMaterials.get() + i;
        if (!ea(item))
            continue;
        item->mpMaterial = nullptr;
        magmaClearGraphicsArray(&item->mFirst);
        magmaClearGraphicsArray(&item->mSecond);
    }
    magmaFreeField(&material->mpMaterials.v);
    material->mMaterialCapacity = 0;
    material->mpMaterials = nullptr;
}

static void magmaReleaseResourceBuffers(dMagma::HdResources *resources) {
    auto &first = resources->mBuffer[0];
    magmaReleaseResourceHandle(&first.mResource);
    first.mpVertices = nullptr;
    if (first.mpStorage.get()) {
        magmaFreeField(&first.mpStorage.v);
        first.mCapacity = 0;
        first.mpStorage = nullptr;
    }
    auto &second = resources->mBuffer[1];
    magmaReleaseResourceHandle(&second.mResource);
    second.mpVertices = nullptr;
    if (second.mpStorage.get()) {
        magmaFreeField(&second.mpStorage.v);
        second.mCapacity = 0;
        second.mpStorage = nullptr;
    }
    resources->mMode = 0;
}

static void magmaCleanupBall(void *self, u32 flag) {
    auto *ball = static_cast<MagmaBall *>(self);
    if (!self)
        return;
    u32 s = ea(self), resource = s + 12;
    ball->__vtbl = magmaTu.baseBall;
    magmaReleaseResourceBuffers(at<dMagma::HdResources>(resource));
    for (s32 i = 0; i < s32(ball->mRender.mShapes.mCount); i++) {
        u32 data = ea(ball->mRender.mShapes.mpShapes.get());
        if (u32(i) < u32(ball->mRender.mShapes.mCount))
            data += u32(i) * 0x23C;
        magmaReleaseRenderBuffer(at<void>(data + 16));
        magmaReleaseRenderBuffer(at<void>(data + 44));
    }
    for (u32 off : {0x500u, 0x51Cu, 0x5A8u, 0x5C4u})
        magmaReleaseRenderBuffer(at<void>(s + off));
    magmaDestroyTextureSampler(&ball->mRender.mKuroSampler.mDescriptor[0], u32(2));
    magmaDestroyTextureSampler(&ball->mRender.mColSampler, u32(2));
    magmaDestroyRenderObject(&ball->mRender.mSecondObject, u32(0));
    magmaDestroyRenderObject(&ball->mRender.mFirstObject, u32(0));
    magmaDestroyShapeArray(&ball->mRender.mShapes, u32(2));
    magmaDestroyIndexBuffer(&ball->mRender.mMaterial.mIndexBuffer, u32(2));
    if (resource) {
        magmaReleaseResourceBuffers(at<dMagma::HdResources>(resource));
        magmaDestroyArray(at<void>(resource), u32(2), u32(0x254), magmaDestroyResource, u32(0), u32(0));
    }
    magmaReleaseMaterials(&ball->mRender.mMaterial);
    if (flag & 1)
        magmaDelete(self);
}

static u32 magmaAllocate(u32 size, u32 alignment) {
    auto *heap = static_cast<MagmaHeap *>(magmaGetCurrentHeap(magmaRootHeap.value.get()));
    auto *table = heap->__vtbl.get();
    return ea(gabi::call_ptr<void *>(table->allocate.get(), heap, size, alignment));
}

static bool magmaTextureEqual(u32 a, u32 b) {
    auto *lhs = at<be<u32>>(a);
    auto *rhs = at<be<u32>>(b);
    // HD: descriptor comparison order is visible when inputs alias other state.
    for (u32 i : {1u, 2u, 3u, 4u, 5u, 6u, 14u, 13u, 7u})
        if (u32(lhs[i]) != u32(rhs[i]))
            return false;
    return true;
}

static MagmaGraphicsSystem magmaGraphicsSystem;
static MagmaGraphicsState magmaDefaultGraphicsState;

static MagmaBallStripTable magmaBallStrips;

static bool magmaDrawMaterial(u32 self, u32 context, bool floor) {
    auto *material = at<dMagma::HdMaterialResources>(self);
    auto *draw = at<MagmaDrawContext>(context);
    MagmaMaterial *selected = nullptr;
    u32 mode = draw->mMode;
    if (s32(mode) < 4) {
        auto *table = material->mpMaterials.get();
        if (mode < u32(material->mMaterialCapacity))
            table += mode;
        selected = static_cast<MagmaMaterial *>(table->mpMaterial.get());
    }
    auto *cache = static_cast<MagmaRenderCache *>(magmaGetRenderCache(&magmaGraphicsSystem, draw));
    MagmaShader *shader = selected->mpShader;
    if (shader != cache->mpShader.get()) {
        u8 flag = shader->mFlags;
        void *previous = cache->mpProgram;
        if (flag & 2) {
            shader->mFlags = flag & ~2;
            magmaUpdateShader(shader, 0);
        }
        void *compiled = shader->mpCompiled->mpProgram;
        if (previous != compiled)
            magmaBindCompiledShader(compiled);
        u32 dl = shader->mDisplayList;
        if (dl)
            magmaSubmitShaderCommands(shader->mpCommands.get(), dl);
        else
            magmaBindShader(shader);
        cache->mpProgram = compiled;
        cache->mpShader = shader;
    }
    mode = draw->mMode;
    if (!mode)
        return false;
    Local<MagmaGraphicsState> graphics;
    if (mode == 1 || mode == 2) {
        auto &shapes =
            floor ? at<dMagma::HdFloorRenderState>(self)->mShapes : at<dMagma::HdBallRenderState>(self)->mShapes;
        auto *packet = static_cast<dMagma::HdShapeRecord *>(shapes.mpShapes.get());
        // HD: the descriptor index arithmetic wraps in guest address space.
        u32 bufferBase = ea(&packet->mBuffers[0]);
        auto *buffer =
            at<dMagma::HdRenderBuffer>(bufferBase + u32(packet->mActiveBuffer) * u32(sizeof(dMagma::HdRenderBuffer)));
        MagmaBindingDescriptor *desc = u32(selected->mBindingCount) ? selected->mpBindings.get() : nullptr;
        s32 first = desc->mIndexBuffer, second = desc->mVertexBuffer, third = desc->mGeometryBuffer;
        u32 size = buffer->mSize, data = ea(buffer->mpData.get());
        if (second != -1)
            magmaBindVertexBuffer(second, data, size);
        if (first != -1)
            magmaBindIndexBuffer(first, data, size);
        if (third != -1)
            magmaBindGeometryBuffer(third, data, size);
        auto *render = floor ? &at<dMagma::HdFloorRenderState>(self)->mSecondObject
                             : &at<dMagma::HdBallRenderState>(self)->mSecondObject;
        auto *firstRender = floor ? &at<dMagma::HdFloorRenderState>(self)->mFirstObject
                                  : &at<dMagma::HdBallRenderState>(self)->mFirstObject;
        for (auto *object : {render, firstRender}) {
            auto *table = at<MagmaRenderVtable>(object->__vtbl);
            gabi::call_ptr<void>(table->bindMaterial.get(), object, selected);
        }
        if (mode == 2) {
            auto *object = draw->mpExtraObject.get();
            if (object) {
                auto *table = at<MagmaRenderVtable>(object->__vtbl);
                gabi::call_ptr<void>(table->bindMaterial.get(), object, selected);
            }
        }
        MagmaMaterialTexture *texture = u32(selected->mTextureCount) ? selected->mpTextures.get() : nullptr;
        auto *sampler = floor ? &at<dMagma::HdFloorRenderState>(self)->mColSampler
                              : &at<dMagma::HdBallRenderState>(self)->mColSampler;
        magmaBindTextureSampler(sampler, &texture->mTexture, -1, 0);
        if (!floor) {
            texture = u32(selected->mTextureCount) > 1 ? selected->mpTextures.get() + 1 : nullptr;
            magmaBindTextureSampler(&at<dMagma::HdBallRenderState>(self)->mKuroSampler, &texture->mTexture, -1, 0);
        }
        if (mode == 2)
            magmaSetContextMaterial(draw, selected);
    }
    magmaConstructGraphicsState(graphics.get());
    graphics->mState = 2;
    graphics->mFloor = floor ? 1 : 0;
    graphics->mFlags = 0;
    if (floor) {
        graphics->mProjectionMode = 4;
        graphics->mProjectionNear = f32(magmaProjectionNear);
    }
    u32 packed = ((u32(graphics->mBlendFlags) & 0xFFFFFFF0u) + 7) & 0xFFFFFF0Fu;
    graphics->mBlendFlags = packed + 16;
    magmaSetGraphicsMode(draw->mMode, graphics.get());
    magmaApplyGraphicsState(graphics.get());
    auto *table = material->mpMaterials.get();
    u32 kind = draw->mMode;
    if (kind < u32(material->mMaterialCapacity))
        table += kind;
    u32 active = material->mResources.mActiveBuffer;
    magmaBindResourceHandle(active == 0 ? table->mSecond.mpItems.get() : table->mFirst.mpItems.get());
    auto &indexBuffer = material->mIndexBuffer;
    if (floor) {
        void *indices = indexBuffer.mpIndices;
        if (indices)
            magmaDrawIndexed(indexBuffer.mPrimitiveType, indices, indexBuffer.mIndexCount, indexBuffer.mIndexOffset, 0,
                             1);
    } else {
        for (s32 i = 0; i < s32(magmaBallStripCount.value); ++i) {
            void *indices = magmaBallStrips.indices[u32(i)];
            u32 offset = u32(magmaBallStripVertices.value) * u32(i);
            if (indices)
                magmaDrawIndexed(indexBuffer.mPrimitiveType, indices, indexBuffer.mIndexCount,
                                 u32(indexBuffer.mIndexOffset) + u32(indexBuffer.mIndexStride) * offset, 0, 1);
        }
    }
    magmaApplyGraphicsState(&magmaDefaultGraphicsState);
    return true;
}

static u32 magmaConstructBall(bool boss, f32 zero, f32 one) {
    u32 s = ea(magmaNew(u32(0xDD4)));
    if (!s)
        return 0;
    auto *ball = at<MagmaBall>(s);
    ball->mRender.mMaterial.mMaterialCount = 0;
    ball->__vtbl = magmaTu.baseBall;
    u32 small = ea(&ball->mRender.mMaterial.mMaterialCapacity);
    if (!small)
        small = ea(magmaNew(u32(8)));
    if (small) {
        auto *table = at<MagmaMaterialTable>(small);
        table->mpMaterials = nullptr;
        table->mCapacity = 0;
    }
    u32 resource = ea(&ball->mRender.mMaterial.mResources);
    if (!resource)
        resource = ea(magmaNew(u32(0x4C0)));
    if (resource) {
        magmaConstructArray(at<void>(resource), u32(2), u32(0x254), magmaConstructBossResource);
        at<dMagma::HdResources>(resource)->mActiveBuffer = 0;
        at<dMagma::HdResources>(resource)->mVertexStride = 12;
        at<dMagma::HdResources>(resource)->mMode = 0;
        at<dMagma::HdResources>(resource)->mDirty = 0;
        at<dMagma::HdResources>(resource)->mVertexLayout = 0;
        at<dMagma::HdResources>(resource)->mBuffer[0].mpVertices.v = 0;
        at<dMagma::HdResources>(resource)->mBuffer[1].mpVertices.v = 0;
    }
    magmaConstructIndexBuffer(&ball->mRender.mMaterial.mIndexBuffer);
    magmaConstructShapeArray(&ball->mRender.mShapes);
    magmaConstructRenderObject(&ball->mRender.mFirstObject);
    ball->mRender.mFirstObject.__vtbl = magmaTu.firstRender;
    magmaClear(&ball->mRender.mModelMtx, u32(0x34));
    if (!u32(ea(&ball->mRender.mModelMtx)))
        magmaNew(u32(48));
    magmaConstructRenderObject(&ball->mRender.mSecondObject);
    ball->mRender.mSecondObject.__vtbl = magmaTu.secondRender;
    magmaClear(&ball->mRender.mShader, u32(sizeof(dMagma::HdShaderParameters)));
    for (auto &color : ball->mRender.mShader.mColors) {
        color.r = zero;
        color.g = zero;
        color.b = zero;
        color.a = one;
    }
    for (auto &pair : ball->mRender.mShader.mColorPairs)
        magmaConstructArray(&pair, 2, sizeof(dMagma::HdColorVector), magmaConstructColorVector);
    for (auto &matrix : ball->mRender.mShader.mMatrices)
        if (!u32(ea(&matrix)))
            magmaNew(sizeof(matrix));
    for (auto &vector : ball->mRender.mShader.mVectors)
        if (!u32(ea(&vector)))
            magmaNew(sizeof(vector));
    ball->mRender.mShaderFlags = 0;
    magmaConstructTextureHandle(&ball->mRender.mColTexture);
    magmaConstructTextureHandle(&ball->mRender.mKuroTexture.mDescriptor[0]);
    magmaConstructTextureSampler(&ball->mRender.mColSampler);
    magmaConstructTextureSampler(&ball->mRender.mKuroSampler.mDescriptor[0]);
    ball->__vtbl = boss ? magmaTu.bossBall : magmaTu.pathBall;
    return s;
}

static MagmaBallPositions magmaBallPositions;
static MagmaFloorPositions magmaFloorPositions;

static MagmaBallIndexData magmaBallIndexData;
static MagmaWord magmaBallVertexFormat;
static MagmaWord magmaFloorVertexFormat;

// HD: the shader archive lazily constructs its selected entry. Out-of-range
// indices retain the original fallback to the first archive entry.
static MagmaShaderEntry *magmaLookupShader(void *object, MagmaStringRef *name) {
    auto *archive = static_cast<MagmaShaderArchive *>(magmaGetResourceArchive(object));
    s32 index = magmaFindShader(archive->mpFile.get(), name);
    MagmaShaderEntry *shader = nullptr;
    if (index >= 0) {
        u32 count = archive->mCount;
        auto *entries = archive->mpEntries.get();
        auto *entry = entries + (u32(index) < count ? u32(index) : 0);
        if (!u8(entry->mLoaded)) {
            auto *file = archive->mpFile.get();
            auto *data = u32(index) < u32(file->mCount) ? file->mpRecords.get() + u32(index) : nullptr;
            magmaLoadShader(entry, data, 0);
            count = archive->mCount;
            entries = archive->mpEntries.get();
        }
        shader = entries + (u32(index) < count ? u32(index) : 0);
    }
    return shader;
}

static void magmaPrepareResources(dMagma::HdMaterialResources *material, u32 size, u32 vertexCount) {
    auto &resources = material->mResources;
    for (u32 i = 0; i < 2; ++i) {
        auto &item = resources.mBuffer[i];
        void *pointer = item.mpVertices;
        if (!pointer) {
            u32 data = magmaAllocate(size, 64);
            if (data) {
                item.mpStorage = at<void>(data);
                item.mCapacity = vertexCount;
            }
            pointer = item.mpStorage;
            item.mpVertices = pointer;
        }
        magmaSetVertexBuffer(&item.mVertexBuffer, pointer, vertexCount, &resources.mVertexLayout);
    }
    resources.mMode = 0;
    resources.mDirty = 1;
    for (u32 i = 0; i < u32(material->mMaterialCount); ++i) {
        auto *item = material->mpMaterials.get();
        if (i < u32(material->mMaterialCapacity))
            item += i;
        void *original = item->mpMaterial;
        item->mpMaterial = nullptr;
        magmaClearGraphicsArray(&item->mFirst);
        magmaClearGraphicsArray(&item->mSecond);
        item->mpMaterial = original;
        for (auto *array : {&item->mFirst, &item->mSecond}) {
            u32 data = magmaAllocate(sizeof(dMagma::HdResourceHandle), 4);
            if (data)
                magmaConstructResourceHandle(at<void>(data));
            if (data) {
                array->mpItems = at<dMagma::HdResourceHandle>(data);
                array->mCount = 1;
            }
        }
        for (u32 j = 0; j < 2; ++j) {
            auto *array = j ? &item->mSecond : &item->mFirst;
            magmaSetMaterialResources(original, array->mpItems.get(), &resources.mBuffer[j].mVertexBuffer,
                                      &resources.mVertexLayout, 0);
        }
    }
}

static void magmaClearCacheLines(u32 data, u32 size) {
    // HD: clearing aligns each line down and compares wrapping 32-bit guest
    // addresses. A normal host memset would change the boundary behaviour.
    u32 limit = data + size;
    for (u32 p = data; p < limit; p += 32) {
        auto *line = at<be<u8>>(p & ~31u);
        for (u32 k = 0; k < 32; ++k)
            line[k] = 0;
    }
}
static void magmaFlushResources(dMagma::HdResources *resources) {
    u32 active = resources->mActiveBuffer;
    auto &item = resources->mBuffer[active];
    magmaFlushVertexBuffer(&item.mVertexBuffer, 0, item.mVertexBuffer.mFlushSize);
    resources->mActiveBuffer = u32(resources->mActiveBuffer) == 0 ? 1 : 0;
}

// GameCube surface simulation and packet traversal.

/* 02588DF8 */
void MagmaBall::draw(void *context) { magmaDrawMaterial(ea(this), ea(context), false); }

/* 02589280 */
s32 MagmaBall::rangeCheck(cXyz *pos, be<f32> *dst) {
    gabi::Local<cXyz> a, b;
    a->x = mPos.x;
    a->y = magmaZero;
    a->z = mPos.z;
    b->x = pos->x;
    b->y = magmaZero;
    b->z = pos->z;
    f32 distSq = magmaSquareDistance(a.get(), b.get());
    f32 scale = mScale;
    f32 rad1 = f32(magmaRangeRadius) * scale;
    if (!(distSq < rad1 * rad1))
        return 0;
    f32 rad2 = f32(magmaSphereRadius) * scale;
    f32 base = f32(mPos.y) - (rad2 - f32(magmaSphereBase));
    f32 dist = magmaSqrt(gabi::fmsubs(rad2, rad2, distSq));
    *dst = base + dist;
    return 1;
}

/* 02589E98 */
void MagmaBallPath::calc(f32 offsY, u32 pathNo, s32 roomNo) {
    if (s16(mWave) < 0) {
        auto *table = gabi::at<MagmaBallVtable>(__vtbl);
        // HD: setup remains virtual and accepts the full incoming path register.
        gabi::call_ptr<void>(table->setup.get(), this, pathNo, roomNo, offsY);
        mWave = 0;
        mWaveTimer = 0;
    }
    if (magmaCalcTimer(&mWaveTimer) == 0) {
        u16 wave = u16(s16(mWave) + 200);
        mWave = s16(wave);
        f32 sine = magmaSinCos.entries[wave >> 3].sine;
        mPos.y = gabi::fmadds(magmaWaveAmplitude, sine - f32(magmaOne), mBaseY);
    }
}

/* 02589F40 */
void MagmaBallPath::update() {
    // HD: preserve the integer word copy, including signalling NaN bits.
    reinterpret_cast<be<u32> *>(&mTexProjMtx[1][3])->set(reinterpret_cast<be<u32> *>(&mPos.y)->get());
    magmaConcat(&magmaViewMtx, &mTexProjMtx, &mPosMtx);
    magmaPrepareBall(this);
}

/* 02589F8C */
void MagmaBallPath::setup(f32 offsY, u32 pathNo, s32 roomNo) {
    MagmaPath *path = magmaGetRoomPath(pathNo, roomNo);
    // HD assert text: path != (0).
    if (!path)
        magmaAssert("d_magma.cpp", 750, "path != NULL");
    MagmaPoint *points = path->m_points;
    // HD assert text: path->m_pnt != (0).
    if (!points) {
        magmaAssert("d_magma.cpp", 751, "path->m_points != NULL");
        points = path->m_points;
    }
    f32 choice = magmaRandom(f32(s32(u16(path->m_num)) - 1));
    s32 index = gabi::ftoi(choice);
    MagmaPoint *pt = points + index;
    f32 amplitude = magmaWaveAmplitude;
    f32 dx = magmaRandomSigned(f32(u8(pt->mArg3)) * amplitude);
    mPos.x = f32(pt->m_position.x) + dx;
    f32 dz = magmaRandomSigned(f32(u8(pt->mArg3)) * amplitude);
    f32 one = magmaOne;
    mPos.z = f32(pt->m_position.z) + dz;
    f32 scale = magmaRandom(one) + one;
    mScale = scale;
    f32 base = offsY - magmaRandom(magmaBaseVariation);
    mBaseY = base;
    f32 waveRandom = magmaRandom(magmaWaveVariation);
    u16 wave = u16(gabi::ftoi(f32(magmaWaveScale) * waveRandom));
    mWave = s16(wave);
    mWaveTimer = 0;
    f32 sine = magmaSinCos.entries[wave >> 3].sine;
    f32 height = gabi::fmadds(amplitude, sine - one, mBaseY);
    mPos.y = height;
    magmaTranslate(&magmaMatrixStack, mPos.x, height, mPos.z);
    scale = mScale;
    magmaScaleM(scale, one, scale);
    magmaCopyMatrix(&magmaMatrixStack, &mTexProjMtx);
}

/* 0258A880 */
void MagmaFloor::draw(void *context) {
    u32 s = ea(this);
    if (ea(mpBalls.get()) && !ea(mpNext.get()) && !ea(mpRoom.get()))
        return;
    if (!magmaDrawMaterial(s, ea(context), true))
        return;
    auto *balls = mpBalls.get();
    for (u32 i = 0; i < u8(mBallNum); ++i)
        balls[i]->draw(context);
}

/* 0258ACD4 */
void MagmaFloor::calc(s32 room) {
    f32 one = f32(magmaOne);
    magmaScale(&magmaMatrixStack, one, f32(magmaFloorProjectionScaleY), one);
    Local<MagmaStringRef> a, b, c, d;
    a->value = gabi::guest_addr("M_DragB");
    a->vtable = magmaTu.safeString;
    b->value = ea(dComIfGp_getStartStageName());
    b->vtable = magmaTu.safeString;
    magmaResolveString(a.get());
    magmaResolveString(a.get());
    magmaResolveString(b.get());
    bool special = magmaSameString(a.get(), b.get());
    if (!special) {
        c->value = gabi::guest_addr("Xboss0");
        c->vtable = magmaTu.safeString;
        d->value = ea(dComIfGp_getStartStageName());
        d->vtable = magmaTu.safeString;
        magmaResolveString(c.get());
        magmaResolveString(c.get());
        magmaResolveString(d.get());
        special = magmaSameString(c.get(), d.get());
    }
    f32 zero = f32(magmaZero), offset = (special ? f32(magmaBaseVariation) : f32(magmaOrdinaryFloorOffset));
    magmaTranslateM(zero, -(f32(mPos.y) + offset), zero);
    magmaConcat(&l_colOrthoMtx, &magmaMatrixStack, &mPostMtx0);
    auto *balls = mpBalls.get();
    for (u32 i = 0; i < u8(mBallNum); ++i) {
        MagmaBall *ball = balls[i];
        auto *table = at<MagmaBallVtable>(ball->__vtbl);
        gabi::call_ptr<void>(table->calc.get(), ball, u32(u8(mPathNo)), room, f32(mPos.y));
        balls[i]->mpFloorTexMtx = &mPostMtx0;
    }
}

/* 0258B068 */
void MagmaFloor::update() {
    if (mpBalls.get() && !mpNext.get() && !mpRoom.get())
        return;
    magmaConcat(&magmaViewMtx, &mTexMtx0, &mPosMtx);
    magmaPrepareFloor(this);
    auto *balls = mpBalls.get();
    for (u32 i = 0; i < u8(mBallNum); ++i) {
        MagmaBall *ball = balls[i];
        auto *table = gabi::at<MagmaBallVtable>(ball->__vtbl);
        gabi::call_ptr<void>(table->update.get(), ball);
    }
}

/* 0258B830 */
void *MagmaFloor::create(void *position, void *scale, s32 path, u32 count, s32 room) {
    auto *pos = static_cast<cXyz *>(position);
    auto *scl = static_cast<cXyz *>(scale);
    f32 z = pos->z, y = pos->y, x = pos->x;
    mPos.z = z;
    mPos.x = x;
    mPos.y = y + f32(magmaFloorSpawnOffset);
    Local<u32> allocator;
    // HD: ASCII spelling of the original Shift-JIS allocator label; the guest string is retained.
    magmaConstructAllocator(allocator.get(), "(magma surface)dMagma_ball_c", 0, 0);
    auto *balls = static_cast<gptr<MagmaBall> *>(magmaNewArray(count * 4));
    mpBalls = balls;
    if (!balls) {
        magmaCancelAllocator(allocator.get());
        magmaDestroyAllocator(allocator.get(), 2);
        return nullptr;
    }
    balls = mpBalls.get();
    mBallNum = u8(count);
    if (count) {
        f32 zero = magmaConstructorZero, one = magmaConstructorOne;
        u32 i = 0;
        do {
            auto *ball = at<MagmaBall>(magmaConstructBall(path < 0, zero, one));
            *balls = ball;
            if (!ball) {
                mBallNum = u8(i);
                i = u8(i + 1);
                break;
            }
            auto *table = at<MagmaBallVtable>(ball->__vtbl);
            gabi::call_ptr<void>(table->setup.get(), ball, u32(path < 0 ? i : u8(path)), room, f32(mPos.y));
            (*balls)->createResources();
            i = u8(i + 1);
            ++balls;
        } while (i < u8(mBallNum));
    }
    mScaleX = f32(scl->x);
    mScaleZ = f32(scl->z);
    mPathNo = u8(path);
    magmaDestroyAllocator(allocator.get(), 2);
    createResources();
    magmaTranslate(&magmaMatrixStack, mPos.x, mPos.y, mPos.z);
    magmaScaleM(mScaleX, magmaOne, mScaleZ);
    magmaCopyMatrix(&magmaMatrixStack, &mTexMtx0);
    update();
    return mpBalls.get();
}

/* 0258C170 */
void MagmaFloor::remove() {
    u32 s = ea(this);
    auto *balls = mpBalls.get();
    if (!balls)
        return;
    for (u32 i = 0; i < u8(mBallNum); ++i) {
        MagmaBall *ball = balls[i];
        if (ball) {
            auto *table = at<MagmaBallVtable>(ball->__vtbl);
            gabi::call_ptr<void>(table->destroy.get(), ball, u32(3));
        }
    }
    // HD: retain the table through iteration, then reload after each deletion call.
    balls = mpBalls.get();
    magmaDeleteArray(balls);
    magmaReleaseAllocation(mpBalls.get());
    mpBalls = nullptr;
    magmaReleaseResourceBuffers(&mRender.mMaterial.mResources);
    for (s32 i = 0; i < s32(mRender.mShapes.mCount); i++) {
        u32 data = ea(mRender.mShapes.mpShapes.get());
        if (u32(i) < u32(mRender.mShapes.mCount))
            data += u32(i) * 0x23C;
        magmaReleaseRenderBuffer(at<void>(data + 16));
        magmaReleaseRenderBuffer(at<void>(data + 44));
    }
    for (u32 off : {0x508u, 0x524u, 0x5B0u, 0x5CCu})
        magmaReleaseRenderBuffer(at<void>(s + off));
    magmaReleaseMaterials(&mRender.mMaterial);
}

/* 0258C474 */
void MagmaRoom::newFloor(MagmaFloor *floor) {
    MagmaFloor *head = mpFirst;
    // HD: floors also retain their owning room for deferred release.
    floor->mpRoom = this;
    floor->mpNext = head;
    mpFirst = floor;
}

/* 0258C488 */
void MagmaRoom::deleteFloor() {
    MagmaFloor *floor = mpFirst;
    mpFirst = nullptr;
    while (floor) {
        MagmaFloor *next = floor->mpNext;
        floor->mpNext = nullptr;
        floor->mpRoom = nullptr;
        // HD: unlink now; packet release later disposes of the render resources.
        floor->mReleaseTimer = 2;
        floor = next;
    }
}

/* 0258C72C */
void MagmaPacket::draw(void *context) {
    auto *draw = static_cast<MagmaDrawContext *>(context);
    if (!u32(draw->mMode))
        return;
    // HD: the material colour is shared between floor and ball shader setup.
    u8 color[4] = {mColor1.r, mColor1.g, mColor1.b, mColor1.a};
    magmaColor.r = color[0];
    magmaColor.g = color[1];
    magmaColor.b = color[2];
    magmaColor.a = color[3];
    for (auto &floor : mFloor) {
        if (floor.mpBalls.get() && (floor.mpNext.get() || floor.mpRoom.get()))
            magmaDrawFloor(&floor, context);
    }
}

/* 0258C7C8 */
void MagmaPacket::calc() {
    f32 one = f32(magmaOne);
    f32 t = f32(l_kuroOrthoMtx[0][3]) + f32(magmaTextureScrollSpeed);
    if (!(t < f32(magmaTextureScrollLimit)))
        t -= one;
    l_kuroOrthoMtx[0][3] = t;
    l_kuroOrthoMtx[1][3] = t;
    magmaScale(&magmaMatrixStack, f32(magmaTextureScaleX), f32(magmaTextureScaleY), f32(magmaTextureScaleZ));
    magmaRotateX(&magmaMatrixStack, u32(0x4000));
    magmaConcat(&l_kuroOrthoMtx, &magmaMatrixStack, &magmaKuroMtx);
    magmaReleaseFloorsGuest(this, u32(0));
    for (u32 room = 0; room < 64; ++room) {
        MagmaFloor *floor = mRoom[room].mpFirst;
        while (floor) {
            MagmaFloor *next = floor->mpNext;
            if (!floor->mpBalls.get())
                next = floor->mpNext;
            else if (next || floor->mpRoom.get()) {
                floor->calc(s32(room));
                next = floor->mpNext;
            }
            floor = next;
        }
    }
    f32 frame = f32(mTimer) + one, period = f32(magmaColorPeriod);
    if (!(frame < period))
        frame -= period;
    mTimer = frame;
    GXColor *color = &magmaKeyColors.colors[1];
    f32 end = 0;
    for (u32 i = 0; i < 2; i++) {
        end = f32(color->a);
        if (frame < end)
            break;
        ++color;
    }
    f32 start = f32(color[-1].a);
    f32 fraction = (frame - start) / (end - start);
    f32 r = magmaMorfCalc(f32(color[-1].r), f32(color->r), fraction);
    mColor1.r = u8(ftoi(r));
    f32 g = magmaMorfCalc(f32(color[-1].g), f32(color->g), f32(cpu->f[3].ps0));
    mColor1.g = u8(ftoi(g));
    f32 b = magmaMorfCalc(f32(color[-1].b), f32(color->b), f32(cpu->f[3].ps0));
    mColor1.b = u8(ftoi(b));
}

/* 0258CA60 */
void MagmaPacket::update() {
    for (auto &floor : mFloor) {
        if (floor.mpBalls.get() && (floor.mpNext.get() || floor.mpRoom.get()))
            floor.update();
    }
    magmaEntryPacket(magmaOpaDrawBuffer.value, this, 0);
}

/* 0258CAEC */
f32 MagmaPacket::checkYpos(cXyz *pos) {
    f32 ret = magmaMinimumY, width = magmaFloorWidth, vertical = magmaFloorHeight;
    gabi::Local<be<f32>> hit;
    for (auto &floor : mFloor) {
        auto *balls = floor.mpBalls.get();
        if (!balls || (!floor.mpNext.get() && !floor.mpRoom.get()))
            continue;
        if (std::fabs(f32(pos->y) - f32(floor.mPos.y)) > vertical)
            continue;
        if (std::fabs(f32(pos->x) - f32(floor.mPos.x)) > width * f32(floor.mScaleX))
            continue;
        if (std::fabs(f32(pos->z) - f32(floor.mPos.z)) > width * f32(floor.mScaleZ))
            continue;
        for (u32 j = 0; j < u8(floor.mBallNum); ++j) {
            if (balls[j]->rangeCheck(pos, hit.get())) {
                f32 y = *hit.get(), floorY = floor.mPos.y;
                if (y < floorY) {
                    y = floorY;
                    *hit.get() = y;
                }
                if (y > ret)
                    ret = y;
            }
        }
    }
    return ret;
}

/* 0258CC64 */
MagmaFloor *MagmaPacket::newFloor(cXyz *pos, cXyz *scale, u32 roomNo, s32 pathNo) {
    // HD assert text: 0 <= i_roomNo && i_roomNo < 64; the argument is unsigned here.
    if (roomNo >= 64)
        magmaAssert("d_magma.cpp", 1723, "roomNo < 64");
    f32 multiplier = magmaBallDensity;
    for (auto &floor : mFloor) {
        if (floor.mpBalls.get() || floor.mpNext.get() || floor.mpRoom.get())
            continue;
        u32 ballNum = 0;
        if (pathNo < 0)
            ballNum = u8(0 - u32(pathNo));
        else {
            MagmaPath *path = magmaGetRoomPath(u32(pathNo), s32(roomNo));
            if (!path)
                return nullptr;
            u32 n = u16(path->m_num);
            if (n) {
                MagmaPoint *pnt = path->m_points;
                ballNum = u32(gabi::ftoi(f32(u8(pnt->mArg3)) * multiplier)) * n;
            }
            // HD: the count is narrowed only after the full path multiplication.
            ballNum = u8(ballNum);
        }
        if (!magmaCreateFloor(&floor, pos, scale, pathNo, ballNum, s32(roomNo)))
            return nullptr;
        mRoom[roomNo].newFloor(&floor);
        return &floor;
    }
    return nullptr;
}

// HD: render resources, compiler-generated and template code of this translation unit.

/* 02589380 */
void MagmaBall::createResources() {
    Local<MagmaStringRef> shaderName;
    shaderName->value = gabi::guest_addr("magma_ball");
    shaderName->vtable = magmaTu.safeString;
    auto *shader = magmaLookupShader(this, shaderName.get());
    magmaSetShader(this, shader, 0);
    auto &material = mRender.mMaterial;
    auto &resources = material.mResources;
    resources.mVertexLayout = 1;
    resources.mHdReserved = ea(&magmaBallVertexFormat);
    magmaPrepareResources(&material, 0x18C, 33);
    magmaResizeShapeArray(&mRender.mShapes, 1, 0);
    magmaSetIndexBuffer(&material.mIndexBuffer, &magmaBallIndexData, 4, magmaBallVertexCount.value);
    material.mIndexBuffer.mPrimitiveType = 6;
    u32 active = resources.mActiveBuffer;
    u32 data = ea(resources.mBuffer[active].mpVertices.get());
    magmaClearCacheLines(data, 0x180);
    active = resources.mActiveBuffer;
    data = ea(resources.mBuffer[active].mpVertices.get());
    auto *vertices = at<cXyz>(data);
    for (u32 i = 0; i < 33; ++i) {
        f32 x = magmaBallPositions.pos[i].x, z = magmaBallPositions.pos[i].z, y = magmaBallPositions.pos[i].y;
        vertices[i].x = x;
        vertices[i].z = z;
        vertices[i].y = y;
    }
    active = resources.mActiveBuffer;
    for (u32 i = 0; i < 33; ++i) {
        auto *from = static_cast<cXyz *>(resources.mBuffer[active].mpVertices.get()) + i;
        auto *to = static_cast<cXyz *>(resources.mBuffer[active == 0 ? 1 : 0].mpVertices.get()) + i;
        to->x = f32(from->x);
        to->y = f32(from->y);
        to->z = f32(from->z);
    }
    magmaFlushResources(&resources);
    magmaLockResources(magmaResourceMutex.value.get());
    Local<MagmaStringRef> texture1, texture2;
    texture1->value = gabi::guest_addr("Magma");
    texture1->vtable = magmaTu.safeString;
    u32 first = ea(magmaGetResource(magmaResourceControl.value.get(), texture1.get(), u32(3)));
    if (!first)
        magmaAssert("d_magma.cpp", 426, "tmp_img != (0)");
    magmaLoadTextureHandle(&mRender.mKuroTexture.mDescriptor[0], at<MagmaResourceFile>(first)->mpImage.get());
    texture2->value = gabi::guest_addr("Magma");
    texture2->vtable = magmaTu.safeString;
    u32 second = ea(magmaGetResource(magmaResourceControl.value.get(), texture2.get(), u32(4)));
    if (!second)
        magmaAssert("d_magma.cpp", 433, "tmp_img != (0)");
    magmaLoadTextureHandle(&mRender.mColTexture, at<MagmaResourceFile>(second)->mpImage.get());
    magmaUnlockResources(magmaResourceMutex.value.get());
    if (!magmaTextureEqual(ea(&mRender.mKuroSampler), ea(&mRender.mKuroTexture)))
        magmaCopyTextureSampler(&mRender.mKuroSampler.mDescriptor[0], &mRender.mKuroTexture.mDescriptor[0]);
    else {
        u32 a = u32(mRender.mKuroTexture.mDescriptor[10]), b = u32(mRender.mKuroTexture.mDescriptor[12]);
        mRender.mKuroSampler.mDescriptor[53] = a;
        mRender.mKuroSampler.mDescriptor[10] = a;
        mRender.mKuroSampler.mDescriptor[55] = b;
        mRender.mKuroSampler.mDescriptor[12] = b;
    }
    mRender.mKuroSampler.mDescriptor[87] = 0;
    mRender.mKuroSampler.mDescriptor[89] = 2;
    mRender.mKuroSampler.mDescriptor[88] = 2;
    mRender.mKuroSampler.mFlags = u8(mRender.mKuroSampler.mFlags) | 2;
    if (!magmaTextureEqual(ea(&mRender.mColSampler), ea(&mRender.mColTexture)))
        magmaCopyTextureSampler(&mRender.mColSampler, &mRender.mColTexture);
    else {
        u32 b = u32(mRender.mColTexture.mDescriptor[12]), a = u32(mRender.mColTexture.mDescriptor[10]);
        mRender.mColSampler.mDescriptor[12] = b;
        mRender.mColSampler.mDescriptor[10] = a;
        mRender.mColSampler.mDescriptor[55] = b;
        mRender.mColSampler.mDescriptor[53] = a;
    }
    mRender.mColSampler.mDescriptor[89] = 0;
    mRender.mColSampler.mDescriptor[87] = 0;
    mRender.mColSampler.mDescriptor[88] = 0;
    mRender.mKuroSampler.mFlags = u8(mRender.mKuroSampler.mFlags) | 8;
    mRender.mKuroSampler.mParameter = f32(magmaSamplerParameter);
    mRender.mColSampler.mFlags = u8(mRender.mColSampler.mFlags) | 2;
    mpFloorTexMtx.v = 0;
}

/* 02589B30 */
void magmaCopyMatrixWords(void *out, void *in) {
    auto *src = static_cast<be<u32> *>(in);
    auto *dst = static_cast<be<u32> *>(out);
    u32 words[12];
    for (u32 i = 0; i < 12; ++i)
        words[i] = src[i];
    for (u32 i = 0; i < 12; ++i)
        dst[i] = words[i];
}

/* 02589BD0 */
f32 magmaLinearColorChannel(u32 value) {
    f32 x = f32(value) / f32(magmaColorMaximum);
    x = magmaPow(x, f32(magmaColorExponent));
    if (x < f32(magmaZero))
        return f32(magmaZero);
    if (x > f32(magmaOne))
        return f32(magmaOne);
    return x;
}

/* 02589C50 */
void magmaConvertColor(void *out, void *color) {
    auto *src = static_cast<GXColor *>(color);
    auto *dst = static_cast<dMagma::HdColorVector *>(out);
    f32 r = magmaLinearColorChannel(u32(src->r));
    f32 g = magmaLinearColorChannel(u32(src->g));
    f32 b = magmaLinearColorChannel(u32(src->b));
    f32 a = f32(src->a) / f32(magmaColorMaximum);
    dst->r = r;
    dst->g = g;
    dst->b = b;
    dst->a = a;
}

/* 02589D30 */
void MagmaBall::prepareRendering() {
    Local<u32[12]> view, inverse, work, extra;
    magmaSetupRenderObject(this);
    cpu->r[6] = ea(&magmaGraphicsSystem);
    magmaCopyMatrixWordsGuest(view.get(), &magmaViewMtx);
    u32 base = cpu->r[6];
    auto *system = at<MagmaGraphicsSystem>(base);
    magmaPrepareShapeArray(&mRender.mShapes.mCount, u32(0), view.get(), &system->mShapeParameters,
                           &system->mpProjection->mParameters);
    magmaUpdateShapeArray(&mRender.mShapes.mCount, u32(0));
    magmaConvertColorGuest(&mRender.mShader.mColors[5].r, &magmaColor);
    magmaInverse(view.get(), inverse.get());
    f32 one = f32(magmaOne);
    magmaScale(&mRender.mShader.mMatrices[0], one, one, one);
    magmaCopyMatrixWordsGuest(work.get(), &magmaKuroMtx);
    magmaConcat(&mRender.mShader.mMatrices[0], work.get(), &mRender.mShader.mMatrices[0]);
    magmaConcat(&mRender.mShader.mMatrices[0], inverse.get(), &mRender.mShader.mMatrices[0]);
    magmaScale(&mRender.mShader.mMatrices[1], one, one, one);
    if (u32 matrix = ea(mpFloorTexMtx.get())) {
        magmaCopyMatrixWordsGuest(extra.get(), at<void>(matrix));
        magmaConcat(&mRender.mShader.mMatrices[1], extra.get(), &mRender.mShader.mMatrices[1]);
    }
    magmaConcat(&mRender.mShader.mMatrices[1], inverse.get(), &mRender.mShader.mMatrices[1]);
    magmaInvalidateRenderObject(&mRender.mSecondObject);
    magmaCopyMatrixWordsGuest(work.get(), &mTexProjMtx);
    magmaCopyMatrix(work.get(), &mRender.mModelMtx);
    magmaInvalidateRenderObject(&mRender.mFirstObject);
}

/* 0258A1C4 */
void *magmaConstructFloor(void *self) {
    if (!self)
        self = magmaNew(u32(0xBE4));
    if (!self)
        return self;
    auto *floor = static_cast<MagmaFloor *>(self);
    floor->mRender.mMaterial.mMaterialCount = 0;
    u32 small = ea(&floor->mRender.mMaterial.mMaterialCapacity);
    if (!small)
        small = ea(magmaNew(u32(8)));
    if (small) {
        auto *table = at<MagmaMaterialTable>(small);
        table->mpMaterials = nullptr;
        table->mCapacity = 0;
    }
    u32 resources = ea(&floor->mRender.mMaterial.mResources);
    if (!resources)
        resources = ea(magmaNew(u32(0x4C0)));
    if (resources) {
        magmaConstructArray(at<void>(resources), u32(2), u32(0x254), magmaConstructPathResource);
        at<dMagma::HdResources>(resources)->mActiveBuffer = 0;
        at<dMagma::HdResources>(resources)->mVertexLayout = 0;
        at<dMagma::HdResources>(resources)->mMode = 0;
        at<dMagma::HdResources>(resources)->mVertexStride = 32;
        at<dMagma::HdResources>(resources)->mDirty = 0;
        at<dMagma::HdResources>(resources)->mBuffer[0].mpVertices.v = 0;
        at<dMagma::HdResources>(resources)->mBuffer[1].mpVertices.v = 0;
    }
    magmaConstructIndexBuffer(&floor->mRender.mMaterial.mIndexBuffer);
    magmaClear(&floor->mRender.mIndices[0], u32(8));
    magmaConstructShapeArray(&floor->mRender.mShapes);
    magmaConstructRenderObject(&floor->mRender.mFirstObject);
    floor->mRender.mFirstObject.__vtbl = magmaTu.firstRender;
    magmaClear(&floor->mRender.mModelMtx, u32(0x34));
    if (!u32(ea(&floor->mRender.mModelMtx)))
        magmaNew(u32(48));
    magmaConstructRenderObject(&floor->mRender.mSecondObject);
    floor->mRender.mSecondObject.__vtbl = magmaTu.secondRender;
    magmaClear(&floor->mRender.mShader, u32(sizeof(dMagma::HdShaderParameters)));
    f32 zero = f32(magmaConstructorZero), one = f32(magmaConstructorOne);
    // Initial color vectors and four identity matrices at their actual HD offsets.
    for (be<f32> *channel : {&floor->mRender.mShader.mColors[0].r,  &floor->mRender.mShader.mColors[0].g,
                             &floor->mRender.mShader.mColors[0].b,  &floor->mRender.mShader.mColors[1].r,
                             &floor->mRender.mShader.mColors[3].b,  &floor->mRender.mShader.mColors[1].b,
                             &floor->mRender.mShader.mColors[6].b,  &floor->mRender.mShader.mColors[8].b,
                             &floor->mRender.mShader.mColors[5].b,  &floor->mRender.mShader.mColors[3].r,
                             &floor->mRender.mShader.mColors[5].g,  &floor->mRender.mShader.mColors[8].g,
                             &floor->mRender.mShader.mColors[7].g,  &floor->mRender.mShader.mColors[4].g,
                             &floor->mRender.mShader.mColors[2].r,  &floor->mRender.mShader.mColors[3].g,
                             &floor->mRender.mShader.mColors[2].g,  &floor->mRender.mShader.mColors[4].b,
                             &floor->mRender.mShader.mColors[1].g,  &floor->mRender.mShader.mColors[7].r,
                             &floor->mRender.mShader.mColors[7].b,  &floor->mRender.mShader.mColors[6].g,
                             &floor->mRender.mShader.mColors[5].r,  &floor->mRender.mShader.mColors[8].r,
                             &floor->mRender.mShader.mColors[2].b,  &floor->mRender.mShader.mColors[6].r,
                             &floor->mRender.mShader.mColors[4].r,  &floor->mRender.mShader.mColors[9].r,
                             &floor->mRender.mShader.mColors[9].g,  &floor->mRender.mShader.mColors[9].b,
                             &floor->mRender.mShader.mColors[10].r, &floor->mRender.mShader.mColors[10].g,
                             &floor->mRender.mShader.mColors[10].b})
        *channel = zero;
    for (be<f32> *channel : {&floor->mRender.mShader.mColors[0].a, &floor->mRender.mShader.mColors[1].a,
                             &floor->mRender.mShader.mColors[2].a, &floor->mRender.mShader.mColors[3].a,
                             &floor->mRender.mShader.mColors[4].a, &floor->mRender.mShader.mColors[5].a,
                             &floor->mRender.mShader.mColors[6].a, &floor->mRender.mShader.mColors[7].a,
                             &floor->mRender.mShader.mColors[8].a, &floor->mRender.mShader.mColors[9].a,
                             &floor->mRender.mShader.mColors[10].a})
        *channel = one;
    for (auto &pair : floor->mRender.mShader.mColorPairs)
        magmaConstructArray(&pair, 2, sizeof(dMagma::HdColorVector), magmaConstructColorVector);
    for (auto &matrix : floor->mRender.mShader.mMatrices)
        if (!u32(ea(&matrix)))
            magmaNew(sizeof(matrix));
    for (auto &vector : floor->mRender.mShader.mVectors)
        if (!u32(ea(&vector)))
            magmaNew(sizeof(vector));
    floor->mRender.mShaderFlags = 0;
    magmaConstructTextureHandle(&floor->mRender.mColTexture);
    magmaConstructTextureSampler(&floor->mRender.mColSampler);
    floor->mPathNo = 0;
    floor->mpBalls.v = 0;
    floor->mBallNum = 0;
    floor->mReleaseTimer = 0;
    f32 z = f32(magmaZero);
    floor->mScaleX = z;
    floor->mScaleZ = z;
    for (auto *matrix : {&floor->mPosMtx, &floor->mTexMtx0, &floor->mPostMtx0})
        magmaClear(matrix, sizeof(*matrix));
    floor->mpRoom.v = 0;
    floor->mpNext.v = 0;
    return self;
}

/* 0258A58C */
void magmaDestroyFloor(void *self, u32 flag) {
    auto *floor = static_cast<MagmaFloor *>(self);
    if (!self)
        return;
    u32 s = ea(self);
    if (ea(floor->mpBalls.get()))
        magmaAssert("d_magma.cpp", 812, "mBall == (0)");
    if (ea(floor->mpNext.get()))
        magmaAssert("d_magma.cpp", 813, "mNext == (0)");
    if (ea(floor->mpRoom.get()))
        magmaAssert("d_magma.cpp", 814, "mAttachedRoom == (0)");
    magmaDestroyTextureSampler(&floor->mRender.mColSampler, u32(2));
    magmaDestroyRenderObject(&floor->mRender.mSecondObject, u32(0));
    magmaDestroyRenderObject(&floor->mRender.mFirstObject, u32(0));
    magmaDestroyShapeArray(&floor->mRender.mShapes, u32(2));
    magmaDestroyIndexBuffer(&floor->mRender.mMaterial.mIndexBuffer, u32(2));
    if (s + 12) {
        magmaReleaseResourceBuffers(&floor->mRender.mMaterial.mResources);
        magmaDestroyArray(&floor->mRender.mMaterial.mResources, 2, 0x254, magmaDestroyEmbeddedBuffer, 0, 0);
    }
    magmaReleaseMaterials(&floor->mRender.mMaterial);
    if (flag & 1)
        magmaDelete(self);
}

/* 0258AF68 */
void MagmaFloor::prepareRendering() {
    Local<u32[12]> view, inverse, work;
    magmaSetupRenderObject(this);
    cpu->r[6] = ea(&magmaGraphicsSystem);
    magmaCopyMatrixWordsGuest(view.get(), &magmaViewMtx);
    // Matrix-copy helper does not modify r6; retain the original shared graphics base.
    u32 base = u32(cpu->r[6]);
    auto *system = at<MagmaGraphicsSystem>(base);
    magmaPrepareShapeArray(&mRender.mShapes.mCount, u32(0), view.get(), &system->mShapeParameters,
                           &system->mpProjection->mParameters);
    magmaUpdateShapeArray(&mRender.mShapes.mCount, u32(0));
    magmaConvertColorGuest(&mRender.mShader.mColors[5].r, &magmaColor);
    magmaInverse(view.get(), inverse.get());
    f32 one = f32(magmaOne);
    magmaScale(&mRender.mShader.mMatrices[2], one, one, one);
    magmaCopyMatrixWordsGuest(work.get(), &magmaKuroMtx);
    magmaConcat(&mRender.mShader.mMatrices[2], work.get(), &mRender.mShader.mMatrices[2]);
    magmaConcat(&mRender.mShader.mMatrices[2], inverse.get(), &mRender.mShader.mMatrices[2]);
    magmaInvalidateRenderObject(&mRender.mSecondObject);
    magmaCopyMatrixWordsGuest(work.get(), &mTexMtx0);
    magmaCopyMatrix(work.get(), &mRender.mModelMtx);
    magmaInvalidateRenderObject(&mRender.mFirstObject);
}

/* 0258B11C */
void MagmaFloor::createResources() {
    Local<MagmaStringRef> shaderName;
    shaderName->value = gabi::guest_addr("magma_floor");
    shaderName->vtable = magmaTu.safeString;
    auto *shader = magmaLookupShader(this, shaderName.get());
    magmaSetShader(this, shader, 0);
    auto &material = mRender.mMaterial;
    auto &resources = material.mResources;
    resources.mVertexLayout = 19;
    resources.mHdReserved = ea(&magmaFloorVertexFormat);
    magmaPrepareResources(&material, 0x80, 4);
    magmaResizeShapeArray(&mRender.mShapes, 1, 0);
    mRender.mIndices[0] = 0;
    mRender.mIndices[1] = 2;
    mRender.mIndices[2] = 1;
    mRender.mIndices[3] = 3;
    magmaSetIndexBuffer(&material.mIndexBuffer, &mRender.mIndices, 4, 4);
    material.mIndexBuffer.mPrimitiveType = 6;
    u32 active = resources.mActiveBuffer;
    u32 data = ea(resources.mBuffer[active].mpVertices.get());
    magmaClearCacheLines(data, 0x80);
    active = resources.mActiveBuffer;
    data = ea(resources.mBuffer[active].mpVertices.get());
    auto *vertices = at<be<f32>[8]>(data);
    for (u32 i = 0; i < 4; ++i)
        for (u32 j = 0; j < 3; ++j)
            vertices[i][j] = f32(magmaFloorPositions.pos[i][j]);
    f32 zero = magmaZero, one = magmaOne;
    auto *components = at<be<f32>>(data);
    // HD: keep the original interleaved attribute initialisation order.
    for (u32 index : {7u, 27u, 3u, 6u, 29u, 13u, 15u, 11u, 20u, 19u, 4u, 12u, 22u, 28u, 5u, 21u})
        components[index] = zero;
    for (u32 index : {31u, 14u, 23u, 30u})
        components[index] = one;
    active = resources.mActiveBuffer;
    for (u32 i = 0; i < 4; ++i) {
        auto *from = static_cast<be<f32> *>(resources.mBuffer[active].mpVertices.get()) + i * 8;
        auto *to = static_cast<be<f32> *>(resources.mBuffer[active == 0 ? 1 : 0].mpVertices.get()) + i * 8;
        for (u32 j = 0; j < 8; ++j)
            to[j] = f32(from[j]);
    }
    magmaFlushResources(&resources);
    magmaLockResources(magmaResourceMutex.value.get());
    Local<MagmaStringRef> texture;
    texture->value = gabi::guest_addr("Magma");
    texture->vtable = magmaTu.safeString;
    u32 file = ea(magmaGetResource(magmaResourceControl.value.get(), texture.get(), u32(4)));
    if (!file)
        magmaAssert("d_magma.cpp", 885, "tmp_img != (0)");
    magmaLoadTextureHandle(&mRender.mColTexture, at<MagmaResourceFile>(file)->mpImage.get());
    magmaUnlockResources(magmaResourceMutex.value.get());
    if (!magmaTextureEqual(ea(&mRender.mColSampler), ea(&mRender.mColTexture)))
        magmaCopyTextureSampler(&mRender.mColSampler, &mRender.mColTexture);
    else {
        u32 b = u32(mRender.mColTexture.mDescriptor[12]), a = u32(mRender.mColTexture.mDescriptor[10]);
        mRender.mColSampler.mDescriptor[55] = b;
        mRender.mColSampler.mDescriptor[53] = a;
        mRender.mColSampler.mDescriptor[12] = b;
        mRender.mColSampler.mDescriptor[10] = a;
    }
    mRender.mColSampler.mDescriptor[87] = 0;
    mRender.mColSampler.mDescriptor[88] = 0;
    mRender.mColSampler.mDescriptor[89] = 0;
    mRender.mColSampler.mFlags = u8(mRender.mColSampler.mFlags) | 2;
}

/* 0258C4C0 */
void *magmaConstructPacket(void *self) {
    if (!self)
        self = magmaNew(u32(0x60C0));
    if (!self)
        return self;
    auto *packet = static_cast<MagmaPacket *>(self);
    magmaConstructBasePacket(self);
    packet->__vtbl = magmaTu.packet;
    magmaConstructArray(&packet->mFloor, u32(8), u32(0xBE4), magmaConstructFloor);
    magmaConstructArray(&packet->mRoom, u32(64), u32(4), magmaConstructRoom);
    magmaMountObjectResource("Magma", u32(0), u32(0));
    f32 one = f32(magmaOne), span = f32(magmaProjectionSpan), zero = f32(magmaProjectionNear),
        far = f32(magmaProjectionFar);
    magmaLightOrtho(&l_kuroOrthoMtx, one, span, span, one, zero, far, zero, zero);
    magmaCopyMatrix(&l_kuroOrthoMtx, &l_colOrthoMtx);
    magmaIdentity(&magmaFloorMtx);
    magmaIdentity(&magmaBallMtx);
    packet->mTimer = f32(magmaZero);
    return self;
}

/* 0258C5D0 */
void MagmaPacket::releaseFloors(s32 force) {
    for (auto &floor : mFloor) {
        if (!floor.mpBalls.get() || floor.mpNext.get() || floor.mpRoom.get())
            continue;
        if (force || !u8(floor.mReleaseTimer))
            magmaRemoveFloor(&floor);
        else
            floor.mReleaseTimer = u8(u8(floor.mReleaseTimer) - 1);
    }
}

/* 0258C674 */
void magmaDestroyPacket(void *self, u32 flag) {
    if (!self)
        return;
    auto *packet = static_cast<MagmaPacket *>(self);
    packet->__vtbl = magmaTu.packet;
    magmaReleaseFloorsGuest(self, u32(1));
    magmaDeleteObjectResource("Magma");
    magmaDestroyArray(&packet->mRoom, u32(64), u32(4), magmaDestroyRoom, u32(0), u32(0));
    magmaDestroyArray(&packet->mFloor, u32(8), u32(0xBE4), magmaDestroyFloor, u32(0), u32(0));
    magmaDestroyBasePacket(self, u32(0));
    if (flag & 1)
        magmaDelete(self);
}

/* 0258CE1C */
void MagmaPacket::deleteRoom(u32 roomNo) { mRoom[roomNo].deleteFloor(); }

/* 0258CE2C */
void magmaModuleInit() {
    magmaPackedState.words[2] = 0;
    magmaPackedState.words[0] = 0;
    magmaPackedState.words[3] = 0;
    magmaPackedState.words[1] = 0;
    magmaRegisterModuleObject(&magmaDestructorRecords0);
    f32 x = f32(magmaInitialModuleX), y = f32(magmaInitialModuleY);
    magmaModuleState.x = x;
    magmaModuleState.y = y;
    magmaInitializeModuleFlag(&magmaModuleState.flag);
    magmaRegisterModuleObject(&magmaDestructorRecords1);
    magmaInitializeModuleByte(&magmaModuleState.byte);
    magmaRegisterModuleObject(&magmaDestructorRecords2);
    magmaBallVertexCount.value = u32(magmaBallStripCount.value) * u32(magmaBallStripVertices.value);
}

/* 0258CEDC */
void magmaDestroyEmpty(void *self, u32 flag) {
    if (self && (flag & 1))
        magmaDelete(self);
}

/* 0258CEF0 */
void magmaDestroyResource(void *self, u32 flag) {
    if (!self)
        return;
    auto *resource = static_cast<dMagma::HdResourceBuffer *>(self);
    magmaDestroyResourceHandle(&resource->mResource, 2);
    magmaDestroyVertexBuffer(&resource->mVertexBuffer, 2);
    if (flag & 1)
        magmaDelete(self);
}

/* 0258CF50 */
void magmaDestroyBoss(void *self, u32 flag) { magmaCleanupBall(self, flag); }

/* 0258D314 */
void *magmaConstructBossResource(void *self) { return magmaConstructResource(self); }

/* 0258D370 */
void magmaDestroyPath(void *self, u32 flag) { magmaCleanupBall(self, flag); }

/* 0258D734 */
void *magmaConstructPathResource(void *self) { return magmaConstructResource(self); }

/* 0258D790 */
void *magmaConstructColorVector(void *self) { return self ? self : magmaNew(u32(16)); }

/* 0258D7BC */
void magmaDestroyEmbeddedBuffer(void *self, u32 flag) {
    if (!self)
        return;
    auto *resource = static_cast<dMagma::HdResourceBuffer *>(self);
    magmaDestroyResourceHandle(&resource->mResource, 2);
    magmaDestroyVertexBuffer(&resource->mVertexBuffer, 2);
    if (flag & 1)
        magmaDelete(self);
}

/* 0258D81C */
void *magmaConstructRoom(void *self) {
    if (!self)
        self = magmaNew(sizeof(MagmaRoom));
    if (self)
        static_cast<MagmaRoom *>(self)->mpFirst = nullptr;
    return self;
}

/* 0258D858 */
void magmaDestroyRoom(void *self, u32 flag) {
    if (!self)
        return;
    if (static_cast<MagmaRoom *>(self)->mpFirst.get())
        // HD assert text: mFloor == (0), renamed to the GameCube member.
        magmaAssert("d_magma.h", 231, "mpFirst == NULL");
    if (flag & 1)
        magmaDelete(self);
}

/* 0258D8C4 */
void magmaStaticInit() {}

/* WWHD glue
data magmaDestructorRecords2 101E9F24
data magmaDestructorRecords1 101E9F18
data magmaDestructorRecords0 101E9F0C
str 1005081C "Magma" in 02589380
str 10050908 "Magma" in 0258B11C
str 1005095C "Magma" in 0258C4C0
str 10050964 "Magma" in 0258C674
str 1005093C "(magma surface)dMagma_ball_c" renamed
str 10050824 "d_magma.cpp" in 02589380
str 1005083C "tmp_img != (0)" in 02589380
str 1005091C "d_magma.cpp" in 0258B11C
str 10050928 "tmp_img != (0)" in 0258B11C
str 100508A8 "d_magma.cpp" in 0258A58C
str 100508B4 "mBall == (0)" in 0258A58C
str 100508C4 "mNext == (0)" in 0258A58C
str 100508D4 "mAttachedRoom == (0)" in 0258A58C
data magmaBallIndexData 101E9E28
data magmaGraphicsSystem 104B45C0
data magmaDefaultGraphicsState 104B474C
data magmaBallStrips 101E9C84
data magmaBallPositions 101E9C9C
data magmaFloorPositions 101E9C54
data magmaBallVertexFormat 100509C8
data magmaFloorVertexFormat 100509CC
str 10050830 "magma_ball"
str 10050910 "magma_floor"
data magmaKeyColors 101E9F00
str 100508F8 "M_DragB"
str 10050900 "Xboss0"
str 100507EC "d_magma.h" in 0258D858
str 100507F8 "mpFirst == NULL" renamed
data magmaSamplerParameter 10050818
data magmaColorMaximum 10050858
data magmaColorExponent 1005085C
data magmaProjectionNear 100508EC
data magmaFloorProjectionScaleY 100508F0
data magmaOrdinaryFloorOffset 100508F4
data magmaFloorSpawnOffset 10050938
data magmaProjectionSpan 10050954
data magmaProjectionFar 10050958
data magmaTextureScrollSpeed 1005096C
data magmaTextureScrollLimit 10050970
data magmaTextureScaleX 10050974
data magmaTextureScaleY 10050978
data magmaTextureScaleZ 1005097C
data magmaColorPeriod 10050980
data magmaInitialModuleX 100509C0
data magmaInitialModuleY 100509C4
data magmaConstructorOne 1014517C
data magmaConstructorZero 10145180
data l_kuroOrthoMtx 10477674
data l_colOrthoMtx 104776A4
data magmaKuroMtx 104776E4
data magmaFloorMtx 10477714
data magmaBallMtx 10477744
data magmaBallStripCount 101E9C4C
data magmaBallStripVertices 101E9C50
data magmaBallVertexCount 10477664
data magmaResourceControl 101F4F28
data magmaResourceMutex 101F8B18
data magmaRootHeap 101F8B4C
data magmaModuleState 10477668
data magmaPackedState 104776D4
ext 02520354 void magmaMountObjectResource(const char*,u32,u32)
ext 02520488 void magmaDeleteObjectResource(const char*)
ext 0255F84C void magmaSetupRenderObject(void*)
ext 02588DEC f32 magmaMorfCalc(f32,f32,f32)
ext 02588DF8 void magmaDrawBallGuest(void*,void*)
ext 02589380 void magmaCreateBallResourcesGuest(void*)
ext 02589B30 void magmaCopyMatrixWordsGuest(void*,void*)
ext 02589BD0 f32 magmaLinearColorChannelGuest(u32)
ext 02589C50 void magmaConvertColorGuest(void*,void*)
ext 0258ACD4 void magmaCalcFloorGuest(void*,u32)
ext 0258B068 void magmaUpdateFloorGuest(void*)
ext 0258B11C void magmaCreateFloorResourcesGuest(void*)
ext 0258C5D0 void magmaReleaseFloorsGuest(void*,u32)
ext 025F0148 void magmaReleaseAllocation(void*)
ext 025F01D8 void magmaConstructAllocator(void*,const char*,u32,u32)
ext 025F0270 void magmaDestroyAllocator(void*,u32)
ext 025F02D0 void magmaCancelAllocator(void*)
ext 025F1BF4 void magmaRotateX(void*,u32)
ext 025F24E0 void magmaTranslateM(f32,f32,f32)
ext 026066C4 void* magmaGetResource(void*,void*,u32)
ext 0273AD10 void* magmaNew(u32)
ext 0273ADAC void* magmaNewArray(u32)
ext 0273AF40 void magmaDelete(void*)
ext 0273AFC8 void magmaDeleteArray(void*)
ext 0274FBF8 void magmaLockResources(void*)
ext 0274FCCC void magmaUnlockResources(void*)
ext 02750250 void magmaConstructGraphicsState(void*)
ext 02750370 void magmaApplyGraphicsState(void*)
ext 02755FEC void* magmaFindHeap(void*,void*)
ext 02756140 void* magmaGetCurrentHeap(void*)
ext 02773798 void magmaLoadTextureHandle(void*,void*)
ext 027B5430 void magmaConstructIndexBuffer(void*)
ext 027B54A0 void magmaDestroyIndexBuffer(void*,u32)
ext 027B54E0 void magmaSetIndexBuffer(void*,void*,u32,u32)
ext 027B5BD8 void magmaConstructVertexBuffer(void*)
ext 027B5CBC void magmaDestroyVertexBuffer(void*,u32)
ext 027B5E94 void magmaFlushVertexBuffer(void*,u32,u32)
ext 027B90AC s32 magmaFindShader(void*,void*)
ext 027B9F68 void magmaBindCompiledShader(void*)
ext 027BB7CC void magmaBindShader(void*)
ext 027BB9E0 void magmaUpdateShader(void*,u32)
ext 027BDEB4 void magmaCopyTextureSampler(void*,void*)
ext 027BDF7C void magmaConstructTextureSampler(void*)
ext 027BE2B0 void magmaDestroyTextureSampler(void*,u32)
ext 027BE53C void magmaBindTextureSampler(void*,void*,s32,u32)
ext 027BE6B8 void magmaConstructTextureHandle(void*)
ext 027BEBEC void magmaReleaseRenderBuffer(void*)
ext 027BF734 void magmaConstructResourceHandle(void*)
ext 027BF7E8 void magmaReleaseResourceHandle(void*)
ext 027BF880 void magmaDestroyResourceHandle(void*,u32)
ext 027BFE5C void magmaBindResourceHandle(void*)
ext 027F1278 void magmaConstructBasePacket(void*)
ext 027F13DC void magmaDestroyBasePacket(void*,u32)
ext 027F29D4 void* magmaGetRenderCache(void*,void*)
ext 027FB40C void magmaConstructRenderObject(void*)
ext 027FB528 void magmaDestroyRenderObject(void*,u32)
ext 027FB678 void magmaInvalidateRenderObject(void*)
ext 027FD6F4 void magmaConstructShapeArray(void*)
ext 027FD764 void magmaDestroyShapeArray(void*,u32)
ext 027FDA54 void magmaPrepareShapeArray(void*,u32,void*,void*,void*)
ext 027FDFF4 void magmaUpdateShapeArray(void*,u32)
ext 027FE084 void magmaResizeShapeArray(void*,u32,u32)
ext 027FF478 void magmaSetVertexBuffer(void*,void*,u32,void*)
ext 027FF530 void magmaSetMaterialResources(void*,void*,void*,void*,u32)
ext 027FFCBC void* magmaGetResourceArchive(void*)
ext 027FFE54 void magmaSetContextMaterial(void*,void*)
ext 0280037C void magmaSetGraphicsMode(u32,void*)
ext 0280068C void magmaSetShader(void*,void*,u32)
ext 02800B0C void magmaLoadShader(void*,void*,u32)
ext 028E9098 void magmaIdentity(void*)
ext 028E91EC void magmaInverse(void*,void*)
ext 028E945C void magmaScale(void*,f32,f32,f32)
ext 028E97BC void magmaLightOrtho(void*,f32,f32,f32,f32,f32,f32,f32,f32)
ext 028EAB2C void magmaInitializeModuleByte(void*)
ext 028ED6F8 void magmaInitializeModuleFlag(void*)
ext 028EFFD0 void magmaConstructArray(void*,u32,u32,MagmaArrayConstructor)
ext 028F0164 void magmaDestroyArray(void*,u32,u32,MagmaArrayDestructor,u32,u32)
ext 028F026C void magmaRegisterModuleObject(void*)
ext 028F4560 f32 magmaPow(f32,f32)
ext 028F521C void magmaClear(void*,u32)
ext C00060E0 void magmaSubmitShaderCommands(void*,u32)
ext C0006178 void magmaDrawIndexed(u32,void*,u32,u32,u32,u32)
ext C00068A8 void magmaBindGeometryBuffer(s32,u32,u32)
ext C0006900 void magmaBindVertexBuffer(s32,u32,u32)
ext C0006A38 void magmaBindIndexBuffer(s32,u32,u32)
ext 0258AF68 void magmaPrepareFloor(MagmaFloor*)
ext 0258C170 void magmaRemoveFloor(MagmaFloor*)
ext 027F0E04 void magmaEntryPacket(void*, MagmaPacket*, u32)
ext 0258A880 void magmaDrawFloor(MagmaFloor*, void*)
ext 0258B830 void* magmaCreateFloor(MagmaFloor*, cXyz*, cXyz*, s32, u32, s32)
data magmaOpaDrawBuffer 104B4634
data magmaColor 101E9C48
data magmaBallDensity 10050990
str 10050994 "d_magma.cpp" in 0258CC64
str 100509A0 "roomNo < 64" renamed
ext 028E8DE8 f32 magmaSquareDistance(cXyz*, cXyz*)
ext 028F4384 f32 magmaSqrt(f32)
ext 0207A9A0 s32 magmaCalcTimer(be<u8>*)
ext 028E9108 void magmaConcat(void*, void*, void*)
ext 02589D30 void magmaPrepareBall(MagmaBall*)
ext 025AAF88 MagmaPath* magmaGetRoomPath(u32, s32)
ext 0273AA24 void magmaAssert(const char*, s32, const char*)
ext 020198D8 f32 magmaRandom(f32)
ext 02019918 f32 magmaRandomSigned(f32)
ext 028E93CC void magmaTranslate(void*, f32, f32, f32)
ext 025F2518 void magmaScaleM(f32, f32, f32)
ext 028E90D4 void magmaCopyMatrix(void*, void*)
data magmaZero 10050808
data magmaRangeRadius 1005080C
data magmaSphereRadius 10050810
data magmaSphereBase 10050814
data magmaOne 10050860
data magmaWaveAmplitude 10050864
data magmaBaseVariation 10050870
data magmaWaveVariation 10050874
data magmaWaveScale 10050878
data magmaMinimumY 10050984
data magmaFloorWidth 10050988
data magmaFloorHeight 1005098C
data magmaSinCos 104A44F8
data magmaViewMtx 104B45F8
data magmaMatrixStack 1048D0CC
str 1005087C "d_magma.cpp" in 02589F8C
str 10050888 "path != NULL" renamed
str 10050894 "path->m_points != NULL" renamed
*/
