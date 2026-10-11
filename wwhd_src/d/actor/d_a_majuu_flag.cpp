// Forsaken Fortress flag: GameCube actor/cloth names with the HD GX2 packet.
// HD: enlarged packet, two material sides, GX2 uniform/vertex buffers and texture surfaces.
// GHS wrapped placement-new tests, word copies and call/reload order are preserved.
// HD: opaque SDK boundaries retain guest address casts; owned records use typed fields.
#include "d/actor/d_a_majuu_flag.h"
#include "bindings.h"
static u32 lHioVtable;
static u32 lActorVtable;
static u32 lPacketVtable;
static u32 lTransformVtable;
static u32 lLightingVtable;
static u32 lVertexFormat;
static u32 lGraphicsSystem;
static majuu::GraphicsState lDefaultRenderState;
static majuu::InitialPositionsTable lInitialPositions;
static majuu::TrianglesTable lTriangles;
static majuu::TexCoordTable lTexCoord;
static majuu::IndicesTable lIndices;
static majuu::NormalNeighborsTable lNormalNeighbors;
static majuu::SpringNeighborsTable lSpringNeighbors;
static majuu::HeaderStatics lHeaderStatics;
static f32 lHeaderFirst;
static f32 lHeaderSecond;
static u8 lHeaderFloatGuard;
static u8 lHeaderByteGuard;
static majuu::DestructorRecord lHeaderDestructor0;
static majuu::DestructorRecord lHeaderDestructor1;
static majuu::DestructorRecord lHeaderDestructor2;
static f32 lZero;
static f32 lParentOffsetZ;
static f32 lDegreesToAngle;
static f32 lFlagOffsetZ;
static f32 lColorDivisor;
static f32 lNormalRipple;
static f32 lWindBlend;
static f32 lOne;
static f32 lDrag;
static f32 lLongRestLength;
static f32 lShortRestLength;
static f32 lInitialJitter;
static f32 lSmallScale;
static f32 lLargeScale;
static f32 lPlayerFlagScale;
static f32 lParameterScale;
static f32 lCullMinX;
static f32 lCullMinY;
static f32 lCullMinZ;
static f32 lCullMaxX;
static f32 lCullMaxY;
static f32 lCullMaxZ;
static f32 lHeaderFirstValue;
static f32 lHeaderSecondValue;
static f32 lWindSpeed1;
static f32 lHioOffsetZ;
static f32 lGravity;
static f32 lSpring;
static f32 lGraphicsOne;
static f32 lGraphicsZero;
static u32 lCalcMtx;
static u32 lVertexCount;
static u32 lIndexCount;
static u8 lMonotone;
static u32 lResourceControl;
static u32 lTextureRepository;
static u32 lGraphicsMutex;
static u32 lHeapManager;
static u32 lOpaqueDrawBuffer;
static u32 lEnvironment;
static majuu::Vector lZeroVector;
static majuu::Vector lBaseZ;
static majuu::HIO l_HIO;
static majuu::SinCosTable lSinCosTable;
static majuu::MatrixWords lMatrixStack;
static majuu::MatrixWords lViewMatrix;
static majuu::MatrixWords lProjectionMatrix;

namespace {
// The pointer is reloaded after heap lookup: a guest callback may update its owner.
/* inlined into daMajuu_Flag_packet_c__dt */
void freeOwnedField(be<u32>* field) {
    u32 allocation = u32(*field);
    u32 manager = lHeapManager;
    majuu::Heap* heap = gabi::at<majuu::Heap>(
        majuuApi_findContainingHeap(gabi::at<void>(manager), gabi::at<void>(allocation)));
    majuu::HeapVtable* vtable = gabi::at<majuu::HeapVtable>(u32(heap->mpVtable));
    u32 target = u32(vtable->mFree.v);
    allocation = u32(*field);
    gabi::call_ptr(target, heap, gabi::at<void>(allocation));
}
/* inlined into daMajuu_Flag_packet_c__dt */
void releaseUniformBuffer(daMajuuFlagUniformBuffer_c* buffer) {
    majuuApi_releaseVertexBuffer(gabi::at<void>(gabi::ea(buffer) + 0x158));
    u32 allocation = u32(buffer->mpAllocation);
    (buffer->mpData = 0);
    if (allocation) {
        // GHS reads the length as well, although the deallocation only needs the pointer.
        (void)u32(buffer->mElementCount);
        freeOwnedField(&buffer->mpAllocation);
        (buffer->mElementCount = 0);
        (buffer->mpAllocation = 0);
    }
}
/* inlined into daMajuu_Flag_packet_c__dt */
void releaseUniformPairs(u32 pairs) {
    for (u32 i = 0; i < 2; ++i) {
        u32 pair = pairs + i * 0x4A8;
        releaseUniformBuffer(gabi::at<daMajuuFlagUniformBuffer_c>(pair));
        releaseUniformBuffer(gabi::at<daMajuuFlagUniformBuffer_c>(pair + 0x254));
    }
}
/* inlined into daMajuu_Flag_packet_c__dt */
void releasePacketResources(daMajuu_Flag_packet_c* packet) {
    daMajuuFlagUniformPairs_c* pairs = gabi::at<daMajuuFlagUniformPairs_c>(gabi::ea(packet) + 0xA4);
    releaseUniformPairs(gabi::ea(pairs));
    (pairs->mFlags = 0);
    for (s32 index = 0; index < s32(packet->mTransformCount); ++index) {
        u32 count = u32(packet->mTransformCount), buffers = u32(packet->mpTransforms);
        if (u32(index) < count)
            buffers += u32(index) * 0x23C;
        majuuApi_releaseMaterialAttribute(gabi::at<void>(buffers + 0x10));
        majuuApi_releaseMaterialAttribute(gabi::at<void>(buffers + 0x2C));
    }
    for (u32 offset : {0xA40u, 0xA5Cu, 0xAE8u, 0xB04u})
        majuuApi_releaseMaterialAttribute(gabi::at<void>(gabi::ea(packet) + offset));
    majuuApi_destroyTexture(&packet->mToonTex, 2);
    majuuApi_destroyTexture(&packet->mFlagTex, 2);
    majuuApi_destroyMaterialBlock(&packet->mLighting, 0);
    majuuApi_destroyMaterialBlock(&packet->mTransform, 0);
    majuuApi_destroyUniformBlock(&packet->mTransformCount, 2);
    majuuApi_destroyIndexBuffer(&packet->mIndexBuffer, 2);
    if (gabi::ea(pairs)) {
        releaseUniformPairs(gabi::ea(pairs));
        (pairs->mFlags = 0);
        majuuApi_destroy_arr(
            pairs, 4, 0x254,
            s32(gabi::guest_fn(reinterpret_cast<const void*>(&FlagUniformBuffer__dt))), 0, 0);
    }
    u32 materials = u32(packet->mpMaterials);
    if (materials) {
        for (s32 index = 0; index < s32(packet->mMaterialCount); ++index) {
            majuu::ShapeGroup* material = gabi::at<majuu::ShapeGroup>(materials + u32(index) * 20);
            if (gabi::ea(material)) {
                u32 objects = u32(material->mShapes[0].mpShapes);
                (material->mpMaterial = 0);
                if (objects) {
                    for (s32 element = 0; element < s32(material->mShapes[0].mCount); ++element) {
                        majuu::Shape* object =
                            gabi::at<majuu::Shape>(objects + u32(element) * 0xF4);
                        majuu::ShapeVtable* table =
                            gabi::at<majuu::ShapeVtable>(u32(object->mpVtable));
                        u32 target = u32(table->mDestroy.v);
                        gabi::call_ptr(target, object, 2);
                        objects = u32(material->mShapes[0].mpShapes);
                    }
                    freeOwnedField(&material->mShapes[0].mpShapes);
                    (material->mShapes[0].mCount = 0);
                    (material->mShapes[0].mpShapes = 0);
                }
                objects = u32(material->mShapes[1].mpShapes);
                if (objects) {
                    for (s32 element = 0; element < s32(material->mShapes[1].mCount); ++element) {
                        majuu::Shape* object =
                            gabi::at<majuu::Shape>(objects + u32(element) * 0xF4);
                        majuu::ShapeVtable* table =
                            gabi::at<majuu::ShapeVtable>(u32(object->mpVtable));
                        u32 target = u32(table->mDestroy.v);
                        gabi::call_ptr(target, object, 2);
                        objects = u32(material->mShapes[1].mpShapes);
                    }
                    freeOwnedField(&material->mShapes[1].mpShapes);
                    (material->mShapes[1].mCount = 0);
                    (material->mShapes[1].mpShapes = 0);
                }
                materials = u32(packet->mpMaterials);
            }
        }
        freeOwnedField(&packet->mpMaterials);
        (packet->mMaterialCount = 0);
        (packet->mpMaterials = 0);
    }
}
} // namespace

namespace {
/* inlined into daMajuu_Flag_packet_c::setNrmVtx */
void copyVectorWords(majuu::Vector* destination, majuu::Vector* source) {
    u32 x = u32(source->mWords[0]), y = u32(source->mWords[1]), z = u32(source->mWords[2]);
    (destination->mWords[0] = x);
    (destination->mWords[1] = y);
    (destination->mWords[2] = z);
}
} // namespace

namespace {
/* inlined into daMajuu_Flag_packet_c::draw */
void bindFlagProgram(majuu::ShaderMaterial* program) {
    majuu::ShaderCache* state =
        gabi::at<majuu::ShaderCache>(majuuApi_getShaderCache(&lGraphicsSystem));
    majuu::ShaderProgram* shader = gabi::at<majuu::ShaderProgram>(u32(program->mpProgram));
    u32 previous = u32(state->mpProgram);
    if (gabi::ea(shader) == previous)
        return;
    u8 flags = u8(shader->mFlags);
    u32 previousFetch = u32(state->mState);
    if (flags & 2) {
        (shader->mFlags = flags & ~2);
        majuuApi_updateShader(shader, 0);
    }
    u32 fetch = u32(gabi::at<majuu::ShaderProgramState>(u32(shader->mpResource))->mState);
    if (previousFetch != fetch)
        majuuApi_bindShaderProgram(gabi::at<void>(fetch));
    u32 displayListSize = u32(shader->mBinarySize);
    if (displayListSize) {
        majuuApi_bindDisplayList(gabi::at<void>(u32(shader->mpBinary)), displayListSize);
        (state->mState = fetch);
        (state->mpProgram = gabi::ea(shader));
    } else {
        majuuApi_bindShader(shader);
        (state->mpProgram = gabi::ea(shader));
        (state->mState = fetch);
    }
}
/* inlined into daMajuu_Flag_packet_c::draw */
void bindFlagUniform(majuu::UniformData* object, majuu::ShaderMaterial* program) {
    majuu::UniformRecord* block =
        gabi::at<majuu::UniformRecord>(gabi::ea(object) + 0x10 + u32(object->mRecordIndex) * 28);
    majuu::UniformSlots* slots =
        gabi::at<majuu::UniformSlots>(u32(program->mUniformCount) ? u32(program->mpUniforms) : 0);
    s16 vertexSlot = s16(slots->mVertex);
    u32 size = u32(block->mSize), data = u32(block->mpBuffer);
    s16 pixelSlot = s16(slots->mPixel), geometrySlot = s16(slots->mGeometry);
    if (pixelSlot != -1)
        majuuApi_bindPixelUniform(pixelSlot, gabi::at<void>(data), size);
    if (vertexSlot != -1)
        majuuApi_bindVertexUniform(vertexSlot, gabi::at<void>(data), size);
    if (geometrySlot != -1)
        majuuApi_bindGeometryUniform(geometrySlot, gabi::at<void>(data), size);
}
/* inlined into daMajuu_Flag_packet_c::draw */
void bindFlagObject(majuu::UniformObjectRoot* object, u32 program) {
    majuu::UniformObjectVtable* table = gabi::at<majuu::UniformObjectVtable>(u32(object->mpVtable));
    u32 target = u32(table->mBind.v);
    gabi::call_ptr(target, object, gabi::at<void>(program));
}
/* inlined into daMajuu_Flag_packet_c::draw */
void drawFlagIndexed(daMajuu_Flag_packet_c* packet) {
    u32 count = u32(packet->mIndexBuffer.mCount);
    if (count) {
        u32 type = u32(packet->mIndexBuffer.mIndexType),
            indices = u32(packet->mIndexBuffer.mpIndices);
        u32 primitive = u32(packet->mIndexBuffer.mPrimitive);
        majuuApi_drawIndexed(primitive, count, type, gabi::at<void>(indices), 0, 1);
    }
}
} // namespace

namespace {
/* inlined into daMajuu_Flag_packet_c::setupMaterial */
u32 allocateFlagBuffer(u32 size, s32 alignment) {
    majuu::Heap* heap =
        gabi::at<majuu::Heap>(majuuApi_getCurrentHeap(gabi::at<void>(lHeapManager)));
    majuu::HeapVtable* table = gabi::at<majuu::HeapVtable>(u32(heap->mpVtable));
    u32 target = u32(table->mAllocate.v);
    return gabi::call_ptr<u32>(target, heap, size, alignment);
}
/* inlined into daMajuu_Flag_packet_c::setupMaterial */
void releaseFlagAttributes(majuu::ShapeGroup* material, u32 countOffset) {
    u32 objects = u32(material->mShapes[(countOffset - 4) / 8].mpShapes);
    if (!objects)
        return;
    for (s32 i = 0; i < s32(material->mShapes[(countOffset - 4) / 8].mCount); ++i) {
        majuu::Shape* object = gabi::at<majuu::Shape>(objects + u32(i) * 0xF4);
        majuu::ShapeVtable* table = gabi::at<majuu::ShapeVtable>(u32(object->mpVtable));
        u32 target = u32(table->mDestroy.v);
        gabi::call_ptr(target, object, 2);
        objects = u32(material->mShapes[(countOffset - 4) / 8].mpShapes);
    }
    freeOwnedField(&material->mShapes[(countOffset - 4) / 8].mpShapes);
    (material->mShapes[(countOffset - 4) / 8].mCount = 0);
    (material->mShapes[(countOffset - 4) / 8].mpShapes = 0);
}
/* inlined into daMajuu_Flag_packet_c::setupMaterial */
void zeroFlagCacheLines(u32 address, u32 size) {
    u32 end = address + size;
    if (address < end)
        do {
            majuu::CacheLine* line = gabi::at<majuu::CacheLine>(address & ~31u);
            for (u32 offset = 0; offset < 32; offset += 4)
                (line->mWords[offset / 4] = 0);
            address += 32;
        } while (address < end);
}
/* inlined into daMajuu_Flag_packet_c::setupMaterial */
bool flagTextureMatches(majuu::TexturePrefix* texture, majuu::TexturePrefix* image) {
    const u32 offsets[] = {4, 8, 12, 16, 20, 24, 0x38, 0x34, 0x1C};
    for (u32 offset : offsets)
        if (u32(texture->mWords[offset / 4]) != u32(image->mWords[offset / 4]))
            return false;
    return true;
}
/* inlined into daMajuu_Flag_packet_c::setupMaterial */
void updateFlagTexture(u32 packet, u32 textureOffset, u32 imageOffset) {
    majuu::Texture* texture = gabi::at<majuu::Texture>(packet + textureOffset);
    majuu::Surface* image = gabi::at<majuu::Surface>(packet + imageOffset);
    if (!flagTextureMatches(gabi::at<majuu::TexturePrefix>(gabi::ea(texture)),
                            gabi::at<majuu::TexturePrefix>(gabi::ea(image)))) {
        majuuApi_copyTextureSurface(texture, image);
        u8 flags = u8(texture->mFlags);
        (texture->mMagFilter = 2);
        (texture->mMinFilter = 2);
        (texture->mMipFilter = 2);
        (texture->mFlags = flags | 2);
    } else {
        u32 data = u32(image->mImage), mipmaps = u32(image->mMipmap);
        (texture->mCachedImage = data);
        (texture->mCachedMipmap = mipmaps);
        (texture->mSurface.mMipmap = mipmaps);
        u8 flags = u8(texture->mFlags);
        (texture->mMinFilter = 2);
        (texture->mSurface.mImage = data);
        (texture->mFlags = flags | 2);
        (texture->mMagFilter = 2);
        (texture->mMipFilter = 2);
    }
}
} // namespace

/* 021B8520 */
void set_mtx(daMajuu_Flag_c* actor) {
    u32 parent = u32(actor->mpParentMtx);
    const u32 stackMatrix = gabi::ea(&lMatrixStack);
    f32 zero = lZero;
    if (parent && u32(actor->mpParentPos)) {
        majuuApi_copyMatrix(gabi::at<void>(parent), gabi::at<void>(stackMatrix));
        majuu::Vector* position = gabi::at<majuu::Vector>(u32(actor->mpParentPos));
        f32 x = f32(position->mValues[0]), y = f32(position->mValues[1]),
            z = f32(position->mValues[2]);
        majuuApi_mDoMtx_stack_c_transM(x, y, z);
        s16 angle = s16(actor->current.angle.y);
        majuuApi_mDoMtx_YrotM(gabi::at<void>(stackMatrix), angle);
        f32 transZ = lParentOffsetZ;
        majuuApi_mDoMtx_stack_c_transM(zero, zero, transZ);
        f32 scale = f32(actor->mFlagScale) * f32(l_HIO.mFlagScale);
        majuuApi_scaleMatrix(scale, scale, scale);
        gabi::Local<cXyz> origin;
        origin->x = f32(lZeroVector.mValues[0]);
        origin->y = f32(lZeroVector.mValues[1]);
        origin->z = f32(lZeroVector.mValues[2]);
        majuuApi_transformVector(gabi::at<void>(stackMatrix), origin.get(), &actor->current.pos);
        majuuApi_copyMatrix(gabi::at<void>(stackMatrix), &actor->mMtx);
    } else {
        f32 x = f32(actor->current.pos.x), z = f32(actor->current.pos.z),
            y = f32(actor->current.pos.y);
        majuuApi_MtxTrans(0, x, y, z);
        u8 custom = u8(l_HIO.mbHasCustomRotation);
        u32 matrix = lCalcMtx;
        if (custom) {
            f32 degrees = lDegreesToAngle;
            s16 angle = s16(gabi::ftoi(f32(l_HIO.mCustomRotation.z) * degrees));
            majuuApi_mDoMtx_ZrotM(gabi::at<void>(matrix), angle);
            angle = s16(gabi::ftoi(f32(l_HIO.mCustomRotation.x) * degrees));
            matrix = lCalcMtx;
            majuuApi_mDoMtx_XrotM(gabi::at<void>(matrix), angle);
            angle = s16(gabi::ftoi(f32(l_HIO.mCustomRotation.y) * degrees));
            matrix = lCalcMtx;
            majuuApi_mDoMtx_YrotM(gabi::at<void>(matrix), angle);
        } else {
            s16 angle = s16(actor->current.angle.z);
            majuuApi_mDoMtx_ZrotM(gabi::at<void>(matrix), angle);
            matrix = lCalcMtx;
            angle = s16(actor->current.angle.x);
            majuuApi_mDoMtx_XrotM(gabi::at<void>(matrix), angle);
            matrix = lCalcMtx;
            angle = s16(actor->current.angle.y);
            majuuApi_mDoMtx_YrotM(gabi::at<void>(matrix), angle);
            if (u8(actor->mFlagType) == 4 || u8(actor->mTexType)) {
                f32 transZ = lFlagOffsetZ;
                majuuApi_MtxTrans(1, zero, zero, transZ);
            } else {
                matrix = lCalcMtx;
                s16 tilt = s16(l_HIO.mRotXHalves[1]);
                majuuApi_mDoMtx_XrotM(gabi::at<void>(matrix), tilt);
                f32 transZ = f32(l_HIO.mTransZ);
                majuuApi_MtxTrans(1, zero, zero, transZ);
            }
        }
        f32 scale = f32(actor->mFlagScale) * f32(l_HIO.mFlagScale);
        majuuApi_MtxScale(1, scale, scale, scale);
        matrix = lCalcMtx;
        majuuApi_copyMatrix(gabi::at<void>(matrix), &actor->mMtx);
    }
}

/* 021B8D98 */
void daMajuu_Flag_packet_c::setNrmMtx() {
    auto* packet = this;
    s16 rotation = s16(packet->mRotateY);
    u32 matrix = lCalcMtx;
    majuuApi_rotateMatrixY(gabi::at<void>(matrix), rotation);
}

/* 021B9244 */
void daMajuu_Flag_packet_c::setBackNrm() {
    auto* packet = this;
    u32 base = gabi::ea(packet), offset = u8(packet->mCurArr) * 252;
    majuu::Vector* destination = gabi::at<majuu::Vector>(base + 0x1734 + offset);
    u32 source = base + 0x153C + offset;
    for (u32 i = 0; i < 21; ++i) {
        (destination->mWords[0] = u32(lZeroVector.mWords[0]));
        (destination->mWords[1] = u32(lZeroVector.mWords[1]));
        (destination->mWords[2] = u32(lZeroVector.mWords[2]));
        majuuApi_PSVECSubtract(destination, gabi::at<void>(source), destination);
        destination = gabi::at<majuu::Vector>(gabi::ea(destination) + (12));
        source += 12;
    }
}

/* 021B8DA8 */
void daMajuu_Flag_packet_c::setNrmVtx(cXyz* output, s32 vertex) {
    auto* packet = this;
    u32 base = gabi::ea(packet);
    u32 positions = base + 0x1344 + u8(packet->mCurArr) * 252;
    gabi::Local<cXyz> temporary, cross, sum, center, first, second, normalized;
    majuu::Vector* position = gabi::at<majuu::Vector>(positions + u32(vertex) * 12);
    (center->x = f32(position->mValues[0]));
    (center->y = f32(position->mValues[1]));
    (center->z = f32(position->mValues[2]));
    f32 zero = lZero;
    (sum->x = zero);
    (sum->y = zero);
    (sum->z = zero);
    u32 neighbors = gabi::ea(&lNormalNeighbors) + u32(vertex) * 28;
    for (u32 face = 0; face < 6; ++face) {
        if (s32(gabi::at<be<s32>>(neighbors)[1]) == -1)
            break;
        majuuApi_subtractVector(
            gabi::at<void>(positions + u32(gabi::at<be<u32>>(neighbors)[0]) * 12), temporary.get(),
            center.get());
        copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(first.get())),
                        gabi::at<majuu::Vector>(gabi::ea(temporary.get())));
        neighbors += 4;
        majuuApi_subtractVector(
            gabi::at<void>(positions + u32(gabi::at<be<u32>>(neighbors)[0]) * 12), temporary.get(),
            center.get());
        copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(second.get())),
                        gabi::at<majuu::Vector>(gabi::ea(temporary.get())));
        majuuApi_crossVector(second.get(), temporary.get(), first.get());
        copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(cross.get())),
                        gabi::at<majuu::Vector>(gabi::ea(temporary.get())));
        majuuApi_cXyz_normZC(cross.get(), temporary.get());
        copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(cross.get())),
                        gabi::at<majuu::Vector>(gabi::ea(temporary.get())));
        majuuApi_addVector(sum.get(), cross.get(), sum.get());
    }
    majuuApi_cXyz_normZC(sum.get(), normalized.get());
    copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(sum.get())),
                    gabi::at<majuu::Vector>(gabi::ea(normalized.get())));
    majuuApi_MtxPush();
    // The triangular mesh row changes the phase origin for each row.
    u32 column = vertex == 0   ? 0
                 : vertex < 3  ? u32(vertex)
                 : vertex < 6  ? u32(vertex) - 1
                 : vertex < 10 ? u32(vertex) - 3
                 : vertex < 15 ? u32(vertex) - 6
                               : u32(vertex) - 10;
    u16 phase = u16(column * u32(-800));
    f32 strength = lNormalRipple;
    f32 sine = f32(lSinCosTable.mValues[u32(phase) >> 3][0]);
    s16 angle = s16(gabi::ftoi(strength * sine));
    majuuApi_mDoMtx_YrotM(gabi::at<void>(lCalcMtx), angle);
    majuuApi_MtxPosition(sum.get(), cross.get());
    majuuApi_cXyz_normZC(cross.get(), normalized.get());
    copyVectorWords(gabi::at<majuu::Vector>(gabi::ea(output)),
                    gabi::at<majuu::Vector>(gabi::ea(normalized.get())));
    majuuApi_MtxPull();
}

/* 021BAB14 */
void daMajuu_Flag_packet_c::draw(void* context) {
    auto* packet = this;
    u32 p = gabi::ea(packet);
    majuu::DrawInfo* ctx = gabi::at<majuu::DrawInfo>(gabi::ea(context));
    majuu::ShaderMaterial* program = gabi::at<majuu::ShaderMaterial>(0);
    s32 pass = s32(ctx->mPass);
    if (pass < 4) {
        u32 count = u32(packet->mMaterialCount);
        majuu::ShapeGroup* material = gabi::at<majuu::ShapeGroup>(u32(packet->mpMaterials));
        if (u32(pass) < count)
            material = gabi::at<majuu::ShapeGroup>(gabi::ea(material) + (u32(pass) * 20));
        program = gabi::at<majuu::ShaderMaterial>(u32(material->mpMaterial));
    }
    bindFlagProgram(gabi::at<majuu::ShaderMaterial>(gabi::ea(program)));
    pass = s32(ctx->mPass);
    if (pass == 0) {
        bindFlagObject(gabi::at<majuu::UniformObjectRoot>(p + 0xA30), gabi::ea(program));
        majuu::DrawObject* scene = gabi::at<majuu::DrawObject>(u32(ctx->mpDrawObject));
        if (gabi::ea(scene))
            bindFlagUniform(gabi::at<majuu::UniformData>(u32(scene->mpUniformData)),
                            gabi::at<majuu::ShaderMaterial>(gabi::ea(program)));
    } else if (pass == 1 || pass == 2) {
        bindFlagUniform(gabi::at<majuu::UniformData>(u32(packet->mpTransforms)),
                        gabi::at<majuu::ShaderMaterial>(gabi::ea(program)));
        bindFlagObject(gabi::at<majuu::UniformObjectRoot>(p + 0xAD8), gabi::ea(program));
        bindFlagObject(gabi::at<majuu::UniformObjectRoot>(p + 0xA30), gabi::ea(program));
        if (pass == 2) {
            u32 extra = u32(ctx->mpObject);
            if (extra)
                bindFlagObject(gabi::at<majuu::UniformObjectRoot>(extra), gabi::ea(program));
            majuuApi_bindDrawMaterial(context, program);
        }
    }
    u32 texture = u32(program->mSamplerCount) ? u32(program->mpSamplers) : 0;
    majuuApi_bindTexture(&packet->mFlagTex, gabi::at<void>(texture + 4), -1, 0);
    texture = u32(program->mSamplerCount) > 1 ? u32(program->mpSamplers) + 0x14 : 0;
    majuuApi_bindTexture(&packet->mToonTex, gabi::at<void>(texture + 4), -1, 0);
    gabi::Local<majuu::DrawState> drawState; // frame r1+8 .. saved registers at r1+0x124
    majuu::DrawState* state = drawState.get();
    majuuApi_constructRenderState(drawState.get());
    u32 flags = u32(state->mStencilBits);
    (state->mCullMode = 0);
    (state->mBlendMode = 2);
    (state->mDepthEnabled = 0);
    (state->mStencilBits = ((((flags & ~0xFu) + 7) & 0xFFFFFF0Fu) + 0x10));
    majuuApi_configureMaterialState(u32(ctx->mPass), drawState.get());
    majuuApi_applyRenderState(drawState.get());
    u32 count = u32(packet->mMaterialCount);
    u32 index = u32(ctx->mPass);
    majuu::ShapeGroup* material = gabi::at<majuu::ShapeGroup>(u32(packet->mpMaterials));
    u32 current = u32(packet->mUniforms.mCurrent);
    if (index < count)
        material = gabi::at<majuu::ShapeGroup>(gabi::ea(material) + (index * 20));
    u32 sideOffset = current == 0 ? 8 : 0;
    majuuApi_bindVertexBuffer(gabi::at<void>(u32(material->mShapes[sideOffset / 8].mpShapes)));
    drawFlagIndexed(gabi::at<daMajuu_Flag_packet_c>(p));
    (state->mCullMode = 1);
    majuuApi_applyCullMode(drawState.get());
    count = u32(packet->mMaterialCount);
    index = u32(ctx->mPass);
    current = u32(packet->mUniforms.mCurrent);
    material = gabi::at<majuu::ShapeGroup>(u32(packet->mpMaterials));
    if (index < count)
        material = gabi::at<majuu::ShapeGroup>(gabi::ea(material) + (index * 20));
    sideOffset = current == 0 ? 8 : 0;
    count = u32(material->mShapes[sideOffset / 8].mCount);
    u32 attributes = u32(material->mShapes[sideOffset / 8].mpShapes);
    if (count > 1)
        attributes += 0xF4;
    majuuApi_bindVertexBuffer(gabi::at<void>(attributes));
    drawFlagIndexed(gabi::at<daMajuu_Flag_packet_c>(p));
    majuuApi_applyRenderState(&lDefaultRenderState);
}

/* 021B8CF4 */
BOOL daMajuu_Flag_Draw(daMajuu_Flag_c* actor) {
    u32 a = gabi::ea(actor);
    gabi::Local<majuu::MatrixWords> temporaryMatrix;
    majuuApi_PSMTXIdentity(temporaryMatrix.get());
    majuuApi_PSMTXConcat(temporaryMatrix.get(), &actor->mMtx, &actor->mPacket.mPosMtx);
    u32 environment = majuuApi_getEnvironment();
    u32 lighting = a + 0x110;
    majuuApi_dScnKy_env_light_c_settingTevStruct(gabi::at<void>(environment), 0,
                                                 &actor->current.pos, gabi::at<void>(lighting));
    if (u8(actor->mbUsePlayerTevStr) == 1) {
        dComIfG_inf_c* game = gabi::at<dComIfG_inf_c>(majuuApi_getPlay());
        lighting = u32(game->play.mPlayerInfo[0].mpPlayer.v) + 0x110;
    }
    (actor->mPacket.mpTevStr = lighting);
    u32 drawBuffer = lOpaqueDrawBuffer;
    majuuApi_entryDrawBuffer(gabi::at<void>(drawBuffer), &actor->mPacket, 0);
    majuuApi_updatePacketUniforms(&actor->mPacket);
    return 1;
}

/* 021B92DC */
void majuu_flag_move(daMajuu_Flag_c* actor) {
    u32 a = gabi::ea(actor);
    majuuApi_sampleWind();
    u8 previous = u8(actor->mPacket.mCurArr), current = previous ^ 1;
    s16 oldPhase = s16(actor->mWave);
    (actor->mPacket.mCurArr = current);
    u32 phase = u32(oldPhase) + u32(l_HIO.mWaveSpeed);
    f32 amplitude = lWindBlend;
    (actor->mWave = u16(phase));
    f32 sine = f32(lSinCosTable.mValues[(phase & 0xFFFF) >> 3][0]);
    f32 one = lOne;
    f32 blend = gabi::fmadds(amplitude, sine, amplitude);
    f32 secondWind = f32(l_HIO.mWindSpeed2), complement = one - blend;
    f32 firstWind = f32(l_HIO.mWindSpeed1);
    f32 zero = lZero;
    f32 windZ = gabi::fmadds(firstWind, blend, secondWind * complement);
    u32 positions = a + 0x16F0;
    u32 source = positions + previous * 252, destination = positions + current * 252;
    u32 oldNormals = a + 0x18E8 + previous * 252, velocities = a + 0x1CD8;
    gabi::Local<cXyz> wind, transformedWind, center, bias, scratch, difference, normalized, sum;
    wind->x = zero;
    wind->y = zero;
    wind->z = windZ;
    f32 strength = majuuApi_dKyw_get_wind_pow();
    majuuApi_scaleVector(wind.get(), wind.get(), strength + strength);
    s16 angle = s16(-s32(s16(actor->current.angle.z)));
    majuuApi_mDoMtx_ZrotS(gabi::at<void>(lCalcMtx), angle);
    angle = s16(-s32(s16(actor->current.angle.x)));
    majuuApi_mDoMtx_XrotM(gabi::at<void>(lCalcMtx), angle);
    majuuApi_MtxPosition(wind.get(), transformedWind.get());
    f32 damping = lDrag;
    f32 longRest = lLongRestLength, shortRest = lShortRestLength;
    for (u32 vertex = 0; vertex < 21; ++vertex) {
        u32 offset = vertex * 12;
        majuu::Vector* input = gabi::at<majuu::Vector>(source + offset);
        majuu::Vector* output = gabi::at<majuu::Vector>(destination + offset);
        // Preserve the original per-word copy; current and previous buffers may overlap in diagnostics.
        (output->mWords[0] = u32(input->mWords[0]));
        (output->mWords[1] = u32(input->mWords[1]));
        (output->mWords[2] = u32(input->mWords[2]));
        center->x = f32(input->mValues[0]);
        center->y = f32(input->mValues[1]);
        center->z = f32(input->mValues[2]);
        majuuApi_dotVector(transformedWind.get(), gabi::at<void>(oldNormals + offset));
        u32 velocity = velocities + offset;
        if (vertex == 15 || vertex == 20) {
            bias->z = f32(lZeroVector.mValues[2]);
            bias->x = f32(lZeroVector.mValues[0]);
            bias->y = f32(lZeroVector.mValues[1]);
            majuuApi_addVector(gabi::at<void>(velocity), bias.get(), gabi::at<void>(velocity));
            majuuApi_scaleVector(gabi::at<void>(velocity), gabi::at<void>(velocity), damping);
            majuuApi_addVector(output, gabi::at<void>(velocity), output);
            continue;
        }
        majuuApi_scaleNormalByDotResult(gabi::at<void>(oldNormals + offset), scratch.get());
        f32 gravity = f32(l_HIO.mGravity);
        sum->x = scratch->x;
        sum->z = scratch->z;
        sum->y = f32(scratch->y) + gravity;
        be<s32>* neighbors = gabi::at<be<s32>>(gabi::ea(&lSpringNeighbors) + vertex * 24);
        for (u32 edge = 0; edge < 5; ++edge) {
            s32 other = s32(neighbors[(edge * 4) / 4]);
            if (other == -1)
                break;
            // Adjacent vertices in a row have the short rest length; row links use the long length.
            u32 delta = u32(other) - vertex;
            u32 sign = u32(s32(delta) >> 31);
            u32 distance = (delta ^ sign) - sign;
            f32 rest = other == 0 || vertex == 0 || s32(distance) > 1 ? longRest : shortRest;
            majuuApi_subtractVector(gabi::at<void>(source + u32(other) * 12), difference.get(),
                                    center.get());
            majuuApi_cXyz_normZC(difference.get(), normalized.get());
            f32 square = majuuApi_squareMagnitude(difference.get());
            f32 length = majuuApi_squareRoot(square);
            f32 spring = (length - rest) * f32(l_HIO.mSpring);
            majuuApi_scaleVector(normalized.get(), normalized.get(), spring);
            majuuApi_addVector(sum.get(), normalized.get(), sum.get());
        }
        bias->z = sum->z;
        bias->x = sum->x;
        bias->y = sum->y;
        majuuApi_addVector(gabi::at<void>(velocity), bias.get(), gabi::at<void>(velocity));
        majuuApi_scaleVector(gabi::at<void>(velocity), gabi::at<void>(velocity), damping);
        majuuApi_addVector(output, gabi::at<void>(velocity), output);
    }
    u32 normals = a + 0x18E8 + u8(actor->mPacket.mCurArr) * 252;
    majuuApi_setNormalMatrix(gabi::at<void>(a + 0x3AC));
    for (s32 vertex = 0; vertex < 21; ++vertex) {
        majuuApi_daMajuu_Flag_packet_c_setNrmVtx(gabi::at<void>(a + 0x3AC), gabi::at<void>(normals),
                                                 vertex);
        normals += 12;
    }
    majuuApi_daMajuu_Flag_packet_c_setBackNrm(gabi::at<void>(a + 0x3AC));
}

/* 021B9730 */
BOOL daMajuu_Flag_Execute(daMajuu_Flag_c* actor) {
    u32 a = gabi::ea(actor);
    if (!lMonotone) {
        majuu::Vector* wind = gabi::at<majuu::Vector>(majuuApi_dKyw_get_wind_vec());
        f32 windX = f32(wind->mValues[0]), windZ = f32(wind->mValues[2]);
        s32 target = majuuApi_cM_atan2s(windX, windZ);
        u32 parent = u32(actor->mpParentMtx);
        if (parent && u32(actor->mpParentPos)) {
            gabi::Local<cXyz> forward, transformed;
            forward->x = f32(lBaseZ.mValues[0]);
            forward->y = f32(lBaseZ.mValues[1]);
            forward->z = f32(lBaseZ.mValues[2]);
            majuuApi_copyMatrix(gabi::at<void>(parent), &lMatrixStack);
            majuuApi_PSMTXMultVecSR(&lMatrixStack, forward.get(), transformed.get());
            f32 x = transformed->x, z = transformed->z;
            target = s16(u32(target) - u32(majuuApi_cM_atan2s(x, z)));
        }
        majuuApi_cLib_addCalcAngleS2(gabi::at<void>(a + 0x322), target, 8, 0x400);
        majuuApi_moveFlagCloth(actor);
        majuuApi_set_mtx(actor);
    }
    return 1;
}

/* 021B981C */
BOOL daMajuu_Flag_IsDelete(daMajuu_Flag_c* actor) { return 1; }

/* 021B9824 */
BOOL daMajuu_Flag_Delete(daMajuu_Flag_c* actor) {
    majuuApi_dComIfG_resDelete(&actor->mPhsCloth, gabi::at<void>(gabi::guest_str("Cloth")));
    u8 texture = u8(actor->mTexType);
    if (texture == 1)
        majuuApi_dComIfG_resDelete(&actor->mPhsFlag, gabi::at<void>(gabi::guest_str("Matif")));
    else if (texture == 2)
        majuuApi_dComIfG_resDelete(&actor->mPhsFlag, gabi::at<void>(gabi::guest_str("Vsvfg")));
    else if (texture == 3)
        majuuApi_dComIfG_resDelete(&actor->mPhsFlag, gabi::at<void>(gabi::guest_str("Xhcf")));
    return 1;
}

/* 021BA650 */
s32 daMajuu_Flag_Create(daMajuu_Flag_c* actor) {
    u32 a = gabi::ea(actor), flags = u32(actor->actor_condition);
    if (!(flags & 8)) {
        if (a) {
            majuuApi_fopAc_ac_c_fopAc_ac_c(actor);
            (actor->__vtbl = gabi::ea(&lActorVtable));
            majuuApi_constructFlagPacket(&actor->mPacket);
            flags = u32(actor->actor_condition);
        }
        (actor->actor_condition = flags | 8);
    }
    u32 parameters = u32(actor->mParameters);
    u8 flagType = u8(parameters), textureType = u8(parameters >> 24);
    (actor->mTexType = textureType);
    s32 phase =
        majuuApi_dComIfG_resLoad(&actor->mPhsCloth, gabi::at<void>(gabi::guest_str("Cloth")));
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    textureType = u8(actor->mTexType);
    if (textureType >= 1 && textureType <= 3) {
        u32 archive = textureType == 1   ? gabi::guest_str("Matif")
                      : textureType == 2 ? gabi::guest_str("Vsvfg")
                                         : gabi::guest_str("Xhcf");
        phase = majuuApi_dComIfG_resLoad(&actor->mPhsFlag, gabi::at<void>(archive));
        if (phase != cPhs_COMPLEATE_e)
            return phase;
    }
    (actor->mFlagType = flagType);
    u8 current = u8(actor->mPacket.mCurArr);
    f32 randomExtent = lInitialJitter, one = lOne, zero = lZero;
    u32 positions = a + 0x16F0 + current * 252;
    majuu::Vector* normals = gabi::at<majuu::Vector>(a + 0x18E8 + current * 252);
    for (u32 vertex = 0; vertex < 21; ++vertex) {
        majuu::Vector* initial =
            gabi::at<majuu::Vector>(gabi::ea(&lInitialPositions) + vertex * 12);
        majuu::Vector* position = gabi::at<majuu::Vector>(positions + vertex * 12);
        (position->mValues[0] = f32(initial->mValues[0]));
        (position->mValues[1] = f32(initial->mValues[1]));
        (position->mValues[2] = f32(initial->mValues[2]));
        (normals->mValues[0] = one);
        (normals->mValues[1] = zero);
        (normals->mValues[2] = zero);
        majuu::Vector* velocity = gabi::at<majuu::Vector>(a + 0x1CD8 + vertex * 12);
        (velocity->mValues[0] = zero);
        (velocity->mValues[1] = zero);
        (velocity->mValues[2] = zero);
        if (vertex != 15 && vertex != 20) {
            f32 jitter = majuuApi_cM_rndFX(randomExtent);
            (position->mValues[0] = f32(position->mValues[0]) + jitter);
            jitter = majuuApi_cM_rndFX(randomExtent);
            (position->mValues[1] = f32(position->mValues[1]) + jitter);
            jitter = majuuApi_cM_rndFX(randomExtent);
            (position->mValues[2] = f32(position->mValues[2]) + jitter);
        }
    }
    majuuApi_daMajuu_Flag_packet_c_setBackNrm(&actor->mPacket);
    textureType = u8(actor->mTexType);
    (actor->mbUsePlayerTevStr = 0);
    f32 scale = lSmallScale;
    if (!textureType) {
        if (flagType == 2)
            scale = lLargeScale;
        else if (flagType == 3) {
            scale = lPlayerFlagScale;
            (actor->mbUsePlayerTevStr = 1);
        } else if (flagType != 4)
            scale = one;
    } else if (flagType != 255) {
        // The byte parameter is converted to float before the fused multiply-add.
        scale = gabi::fmadds(lParameterScale, f32(flagType), scale);
    }
    (actor->mFlagScale = scale);
    majuuApi_set_mtx(actor);
    f32 minZ = lCullMinZ, minX = lCullMinX;
    f32 maxZ = lCullMaxZ, maxY = lCullMaxY;
    f32 maxX = lCullMaxX, minY = lCullMinY;
    (actor->cullMtx = a + 0x1E04);
    majuuApi_fopAcM_setCullSizeBox(actor, minX, minY, minZ, maxX, maxY, maxZ);
    (actor->cullSizeFar = randomExtent);
    majuu::Vector* wind = gabi::at<majuu::Vector>(majuuApi_dKyw_get_wind_vec());
    f32 windX = f32(wind->mValues[0]), windZ = f32(wind->mValues[2]);
    s32 angle = majuuApi_cM_atan2s(windX, windZ);
    textureType = u8(actor->mTexType);
    (actor->current.angle.y = u16(angle));
    if (textureType == 3)
        for (u32 i = 0; i < 20; ++i)
            majuuApi_moveFlagCloth(actor);
    (actor->mPacket.mpActor = a);
    majuuApi_setupFlagPacket(&actor->mPacket);
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and template code of this translation unit.
/* 021B879C */
void majuu_copyMatrix(void* destination, void* source) {
    majuu::MatrixWords* dst = gabi::at<majuu::MatrixWords>(gabi::ea(destination));
    majuu::MatrixWords* src = gabi::at<majuu::MatrixWords>(gabi::ea(source));
    f32 snapshot[12];
    for (u32 i = 0; i < 12; ++i)
        snapshot[i] = f32(src->mValues[(i * 4) / 4]);
    for (u32 i = 0; i < 12; ++i)
        (dst->mValues[(i * 4) / 4] = snapshot[i]);
}

/* 021B883C */
void majuu_colorS10(void* destination, void* source) {
    majuu::Vector4* dst = gabi::at<majuu::Vector4>(gabi::ea(destination));
    majuu::ColorS10* src = gabi::at<majuu::ColorS10>(gabi::ea(source));
    f32 divisor = lColorDivisor;
    f32 red = f32(s16(src->mValues[0])) / divisor;
    f32 blue = f32(s16(src->mValues[2])) / divisor;
    f32 green = f32(s16(src->mValues[1])) / divisor;
    f32 alpha = f32(s16(src->mValues[3])) / divisor;
    (dst->mValues[0] = red);
    (dst->mValues[1] = green);
    (dst->mValues[2] = blue);
    (dst->mValues[3] = alpha);
}

/* 021B8900 */
void majuu_colorU8(void* destination, void* source) {
    majuu::Vector4* dst = gabi::at<majuu::Vector4>(gabi::ea(destination));
    majuu::ColorU8* src = gabi::at<majuu::ColorU8>(gabi::ea(source));
    u8 red = u8(src->mValues[0]), green = u8(src->mValues[1]);
    f32 divisor = lColorDivisor;
    u8 blue = u8(src->mValues[2]), alpha = u8(src->mValues[3]);
    f32 rgba[4] = {f32(red) / divisor, f32(green) / divisor, f32(blue) / divisor,
                   f32(alpha) / divisor};
    for (u32 i = 0; i < 4; ++i)
        (dst->mValues[(i * 4) / 4] = rgba[i]);
}

/* 021BB174 */
void* FlagUniformBuffer__ct(void* object) {
    daMajuuFlagUniformBuffer_c* a = gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(object));
    if (!gabi::ea(a))
        a = gabi::at<daMajuuFlagUniformBuffer_c>(majuuApi_allocate(0x254));
    if (gabi::ea(a)) {
        majuuApi_constructVertexBuffer(gabi::at<void>(gabi::ea(a) + 4));
        majuuApi_constructAttribute(gabi::at<void>(gabi::ea(a) + 0x158));
        (a->mpAllocation = 0);
        (a->mElementCount = 0);
    }
    return a;
}

/* 021BB1D0 */
void* FlagVector4__ct(void* object) {
    return object ? object : gabi::at<void>(majuuApi_allocate(16));
}

/* 021BB1FC */
void FlagVector4__dt(void* object, u32 flags) {
    if (object && (flags & 1))
        majuuApi_deallocate(object);
}

/* 021BB210 */
void FlagUniformBuffer__dt(void* object, u32 flags) {
    u32 a = gabi::ea(object);
    if (a) {
        majuuApi_destroyVertexBuffer(gabi::at<void>(a + 0x158), 2);
        majuuApi_destroyVertexResource(gabi::at<void>(a + 4), 2);
        if (flags & 1)
            majuuApi_deallocate(object);
    }
}

/* 021BB698 */
void majuu_emptyPacketCallback(void* object) {}

/* 021BBACC */
void majuu_emptyActorCallback(void* object) {}

/* 021BB050 */
void __sinit_d_a_majuu_flag_cpp() {
    for (u32 offset : {8u, 0u, 12u, 4u})
        (lHeaderStatics.mWords[offset / 4] = 0);
    majuuApi_registerDestructor(&lHeaderDestructor0);
    (lHeaderFirst = lHeaderFirstValue);
    (lHeaderSecond = lHeaderSecondValue);
    majuuApi_initializeHeaderFloat(&lHeaderFloatGuard);
    majuuApi_registerDestructor(&lHeaderDestructor1);
    majuuApi_initializeHeaderByte(&lHeaderByteGuard);
    majuuApi_registerDestructor(&lHeaderDestructor2);
    f32 wind1 = lWindSpeed1, wind2 = lInitialJitter;
    f32 scale = lOne;
    (l_HIO.mWindSpeed1 = wind1);
    f32 translation = lHioOffsetZ;
    (l_HIO.mFlagScale = scale);
    f32 gravity = lGravity;
    (l_HIO.mTransZ = translation);
    f32 spring = lSpring;
    (l_HIO.mWindSpeed2 = wind2);
    (l_HIO.mGravity = gravity);
    (l_HIO.mSpring = spring);
    (l_HIO.mbUseToonTex = 1);
    (l_HIO.m06 = 0);
    (l_HIO.m05 = 0);
    (l_HIO.mbHasCustomRotation = 0);
    (l_HIO.m07 = 0);
    (l_HIO.mpVtable = gabi::ea(&lHioVtable));
    (l_HIO.mWaveSpeed = 0x400);
    (l_HIO.mRotX = -1500);
    (l_HIO.m08 = 0);
}

/* 021B98B4 */
void* daMajuu_Flag_packet_c__ct(daMajuu_Flag_packet_c* object) {
    daMajuu_Flag_packet_c* a = gabi::at<daMajuu_Flag_packet_c>(gabi::ea(object));
    if (!gabi::ea(a))
        a = gabi::at<daMajuu_Flag_packet_c>(majuuApi_allocate(0x1A30));
    if (!gabi::ea(a))
        return a;
    majuuApi_constructPacketBase(a);
    (a->mProgramCount = 0);
    (a->mpVtable = gabi::ea(&lPacketVtable));
    majuu::ShapeArray* embedded = gabi::at<majuu::ShapeArray>(gabi::ea(a) + 0x9C);
    if (!gabi::ea(embedded))
        embedded = gabi::at<majuu::ShapeArray>(majuuApi_allocate(8));
    if (gabi::ea(embedded)) {
        (embedded->mpShapes = 0);
        (embedded->mCount = 0);
    }
    daMajuuFlagUniformPairs_c* pairs = gabi::at<daMajuuFlagUniformPairs_c>(gabi::ea(a) + 0xA4);
    if (!gabi::ea(pairs))
        pairs = gabi::at<daMajuuFlagUniformPairs_c>(majuuApi_allocate(0x968));
    if (gabi::ea(pairs)) {
        majuuApi_constructArray(
            pairs, 4, 0x254,
            s32(gabi::guest_fn(reinterpret_cast<const void*>(&FlagUniformBuffer__ct))));
        (pairs->mCurrent = 0);
        (pairs->mFlags = 0);
        (pairs->mStride = 32);
        (pairs->mReady = 0);
        (pairs->mFormat = 0);
        for (u32 offset : {0u, 0x4A8u, 0x254u, 0x6FCu})
            (gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(pairs) + offset)->mpData = 0);
    }
    majuuApi_constructIndexBuffer(&a->mIndexBuffer);
    majuuApi_constructUniformBlock(&a->mTransformCount);
    majuuApi_constructMaterialBlock(&a->mTransform);
    (a->mTransform.mpVtable.v = gabi::ea(&lTransformVtable));
    majuuApi_clearMemory(&a->mTransform.mMatrix, 0x34);
    if (u32(gabi::ea(a) + 0xAA4) == 0)
        majuuApi_allocateDiscardResult(48);
    majuuApi_constructMaterialBlock(&a->mLighting);
    (a->mLighting.mpVtable.v = gabi::ea(&lLightingVtable));
    majuuApi_clearMemory(&a->mLighting.mColors[0].mValues[0], 0x2F0);
    f32 zero = lGraphicsZero, one = lGraphicsOne;
    (a->mLighting.mColors[0].mValues[0] = zero);
    (a->mLighting.mColors[0].mValues[1] = zero);
    (a->mLighting.mColors[0].mValues[2] = zero);
    (a->mLighting.mColors[1].mValues[0] = zero);
    (a->mLighting.mColors[2].mValues[3] = one);
    (a->mLighting.mColors[3].mValues[2] = zero);
    (a->mLighting.mColors[1].mValues[3] = one);
    (a->mLighting.mColors[1].mValues[2] = zero);
    (a->mLighting.mColors[6].mValues[2] = zero);
    (a->mLighting.mColors[8].mValues[2] = zero);
    (a->mLighting.mColors[5].mValues[2] = zero);
    (a->mLighting.mColors[7].mValues[3] = one);
    (a->mLighting.mColors[3].mValues[0] = zero);
    (a->mLighting.mColors[5].mValues[1] = zero);
    (a->mLighting.mColors[8].mValues[1] = zero);
    (a->mLighting.mColors[7].mValues[1] = zero);
    (a->mLighting.mColors[4].mValues[1] = zero);
    (a->mLighting.mColors[2].mValues[0] = zero);
    (a->mLighting.mColors[3].mValues[1] = zero);
    (a->mLighting.mColors[3].mValues[3] = one);
    (a->mLighting.mColors[2].mValues[1] = zero);
    (a->mLighting.mColors[4].mValues[2] = zero);
    (a->mLighting.mColors[1].mValues[1] = zero);
    (a->mLighting.mColors[7].mValues[0] = zero);
    (a->mLighting.mColors[7].mValues[2] = zero);
    (a->mLighting.mColors[8].mValues[3] = one);
    (a->mLighting.mColors[5].mValues[3] = one);
    (a->mLighting.mColors[6].mValues[3] = one);
    (a->mLighting.mColors[6].mValues[1] = zero);
    (a->mLighting.mColors[5].mValues[0] = zero);
    (a->mLighting.mColors[8].mValues[0] = zero);
    (a->mLighting.mColors[2].mValues[2] = zero);
    (a->mLighting.mColors[6].mValues[0] = zero);
    (a->mLighting.mColors[0].mValues[3] = one);
    (a->mLighting.mColors[4].mValues[0] = zero);
    (a->mLighting.mColors[4].mValues[3] = one);
    (a->mLighting.mColors[9].mValues[0] = zero);
    (a->mLighting.mColors[9].mValues[1] = zero);
    (a->mLighting.mColors[9].mValues[2] = zero);
    (a->mLighting.mColors[9].mValues[3] = one);
    (a->mLighting.mColors[10].mValues[0] = zero);
    (a->mLighting.mColors[10].mValues[1] = zero);
    (a->mLighting.mColors[10].mValues[2] = zero);
    (a->mLighting.mColors[10].mValues[3] = one);

    for (u32 offset : {0xBFCu, 0xC1Cu, 0xC3Cu})
        majuuApi_constructArray(
            gabi::at<void>(gabi::ea(a) + offset), 2, 16,
            s32(gabi::guest_fn(reinterpret_cast<const void*>(&FlagVector4__ct))));
    for (u32 offset = 0xC5C; offset <= 0xDAC; offset += 0x30)
        if (u32(gabi::ea(a) + offset) == 0)
            majuuApi_allocateDiscardResult(48);
    for (u32 offset = 0xDDC; offset <= 0xE2C; offset += 0x10)
        if (u32(gabi::ea(a) + offset) == 0)
            majuuApi_allocateDiscardResult(16);
    (a->mLighting.mEnabled = 0);
    majuuApi_constructSurfaceTransfer(&a->mFlagSurface);
    majuuApi_constructSurfaceTransfer(&a->mToonSurface);
    majuuApi_constructTexture(&a->mFlagTex);
    majuuApi_constructTexture(&a->mToonTex);
    (a->mRotateY = 0);
    (a->m796 = 0);
    (a->m79B = 1);
    (a->mCurArr = 0);
    (a->m798 = 0);
    return a;
}

/* 021BB270 */
void daMajuu_Flag_packet_c__dt(daMajuu_Flag_packet_c* object, u32 flags) {
    u32 packet = gabi::ea(object);
    if (!packet)
        return;
    (object->mpVtable = gabi::ea(&lPacketVtable));
    releasePacketResources(gabi::at<daMajuu_Flag_packet_c>(packet));
    majuuApi_destroyPacketBase(object, 0);
    if (flags & 1)
        majuuApi_deallocate(object);
}

/* 021BB69C */
void daMajuu_Flag_c__dt(daMajuu_Flag_c* actor, u32 flags) {
    u32 a = gabi::ea(actor);
    if (!a)
        return;
    (actor->mPacket.mpVtable = gabi::ea(&lPacketVtable));
    releasePacketResources(gabi::at<daMajuu_Flag_packet_c>(a + 0x3AC));
    majuuApi_destroyPacketBase(&actor->mPacket, 0);
    majuuApi_fopAc_ac_c_dtor(actor, 0);
    if (flags & 1)
        majuuApi_deallocate(actor);
}

/* 021B89B4 */
void daMajuu_Flag_packet_c::updateUniforms() {
    auto* packet = this;
    u32 p = gabi::ea(packet);
    daMajuuFlagUniformPairs_c* pairs = gabi::at<daMajuuFlagUniformPairs_c>(p + 0xA4);
    for (u32 side = 0; side < 2; ++side) {
        u32 current = u32(pairs->mCurrent);
        majuu::Vertex* destination =
            gabi::at<majuu::Vertex>(u32(pairs->mBuffers[side * 2 + current].mpData) - 0x20);
        majuu::Triangle* triangles = gabi::at<majuu::Triangle>(gabi::ea(&lTriangles) - 3);
        u32 normals = p + (side == 0 ? 0x153C : 0x1734);
        for (u32 vertex = 0; vertex < 35; ++vertex) {
            u32 index = u8(packet->mCurArr) * 21;
            triangles = gabi::at<majuu::Triangle>(gabi::ea(triangles) + (3));
            index += u8(triangles->mPosition);
            majuu::Vector* position = gabi::at<majuu::Vector>(p + 0x1344 + index * 12);
            f32 z = f32(position->mValues[2]), x = f32(position->mValues[0]),
                y = f32(position->mValues[1]);
            destination = gabi::at<majuu::Vertex>(gabi::ea(destination) + (0x20));
            (destination->mValues[0] = x);
            if (!side) {
                (destination->mValues[1] = y);
                (destination->mValues[2] = z);
            } else {
                (destination->mValues[2] = z);
                (destination->mValues[1] = y);
            }
            index = u8(packet->mCurArr) * 21 + u8(triangles->mNormal);
            majuu::Vector* normal = gabi::at<majuu::Vector>(normals + index * 12);
            f32 nx = f32(normal->mValues[0]);
            f32 nz = f32(normal->mValues[2]), ny = f32(normal->mValues[1]);
            (destination->mValues[5] = nz);
            (destination->mValues[3] = nx);
            (destination->mValues[4] = ny);
        }
    }
    u32 current = u32(pairs->mCurrent);
    decltype(daMajuuFlagUniformBuffer_c::mUniformObject)* uniform =
        gabi::at<decltype(daMajuuFlagUniformBuffer_c::mUniformObject)>(gabi::ea(pairs) +
                                                                       current * 0x254 + 4);
    for (u32 side = 0; side < 2; ++side) {
        u32 size = u32(uniform->mSize);
        majuuApi_flushVertexBuffer(uniform, 0, size);
        uniform = gabi::at<decltype(daMajuuFlagUniformBuffer_c::mUniformObject)>(gabi::ea(uniform) +
                                                                                 (0x4A8));
    }
    current = u32(pairs->mCurrent);
    (pairs->mCurrent = current == 0 ? 1 : 0);
    majuuApi_setFogTevstr(gabi::at<void>(u32(packet->mpTevStr)));
    majuuApi_setLightTevstr(gabi::at<void>(u32(packet->mpTevStr)));
    gabi::Local<majuu::MatrixWords> view;
    majuuApi_copyMatrixSnapshot(view.get(), &lViewMatrix);
    u32 environment = lEnvironment;
    majuuApi_setViewUniforms(&packet->mTransformCount, 0, view.get(), &lProjectionMatrix,
                             gabi::at<void>(environment + 0x240));
    gabi::Local<majuu::Vector4> color, scaled;
    majuu::Light* lighting = gabi::at<majuu::Light>(u32(packet->mpTevStr));
    majuu::LightUniforms* buffers = gabi::at<majuu::LightUniforms>(u32(packet->mpTransforms));
    majuuApi_colorS10ToFloat(color.get(), gabi::at<void>(gabi::ea(lighting) + 0x90));
    f32 scale = f32(gabi::at<majuu::Light>(u32(packet->mpTevStr))->mLightScale);
    majuuApi_scaleColor(scaled.get(), color.get(), scale);
    for (u32 i = 0; i < 4; ++i)
        (buffers->mColors[0][i] = u32(scaled.get()->mWords[i]));
    lighting = gabi::at<majuu::Light>(u32(packet->mpTevStr));
    buffers = gabi::at<majuu::LightUniforms>(u32(packet->mpTransforms));
    majuuApi_colorS10ToFloat(color.get(), gabi::at<void>(gabi::ea(lighting) + 0x160));
    scale = f32(gabi::at<majuu::ExtraLight>(u32(packet->mpTevStr))->mScale);
    gabi::Local<majuu::Vector4> secondary;
    majuuApi_scaleColor(secondary.get(), color.get(), scale);
    for (u32 i = 0; i < 4; ++i)
        (buffers->mColors[1][i] = u32(secondary.get()->mWords[i]));
    majuuApi_submitViewUniforms(&packet->mTransformCount, 0);
    lighting = gabi::at<majuu::Light>(u32(packet->mpTevStr));
    majuuApi_colorS10ToFloat(&packet->mLighting.mColors[4].mValues[0],
                             gabi::at<void>(gabi::ea(lighting) + 0x90));
    majuuApi_colorU8ToFloat(&packet->mLighting.mColors[5].mValues[0],
                            gabi::at<void>(gabi::ea(lighting) + 0x98));
    scale = f32(gabi::at<majuu::Light>(u32(packet->mpTevStr))->mColorScale);
    majuuApi_scaleColorInPlace(&packet->mLighting.mColors[5].mValues[0], scale);
    if (u8(lighting->mAlpha)) {
        majuuApi_colorU8ToFloat(&packet->mLighting.mColors[6].mValues[0],
                                gabi::at<void>(gabi::ea(lighting) + 0x9C));
    } else {
        f32 zero = lZero;
        (packet->mLighting.mColors[6].mValues[1] = zero);
        (packet->mLighting.mColors[6].mValues[0] = zero);
        (packet->mLighting.mColors[6].mValues[2] = zero);
        (packet->mLighting.mColors[6].mValues[3] = zero);
    }
    majuuApi_submitMaterialUniforms(&packet->mLighting);
    gabi::Local<majuu::MatrixWords> model;
    majuuApi_copyMatrixSnapshot(model.get(), &packet->mPosMtx);
    majuuApi_copyMatrix(model.get(), &packet->mTransform.mMatrix);
    majuuApi_submitMaterialUniforms(&packet->mTransform);
}

/* 021B9C78 */
void daMajuu_Flag_packet_c::setupMaterial() {
    auto* packet = this;
    u32 p = gabi::ea(packet), nameSpace = gabi::guest_str("");
    gabi::Local<majuu::ResourceName> shaderName;
    shaderName->mpVtable = nameSpace;
    shaderName->mpText = gabi::guest_str("flag_default");
    daMajuu_Flag_c* actor = gabi::at<daMajuu_Flag_c>(u32(packet->mpActor));
    majuu::MaterialManager* repository =
        gabi::at<majuu::MaterialManager>(majuuApi_getMaterialManager());
    majuu::MaterialResource* shaderArchive =
        gabi::at<majuu::MaterialResource>(u32(repository->mpResource));
    s32 index = majuuApi_findMaterial(shaderArchive, shaderName.get());
    u32 shader = 0;
    if (index >= 0) {
        u32 count = u32(repository->mCount), records = u32(repository->mpInstances);
        majuu::MaterialInstance* record =
            gabi::at<majuu::MaterialInstance>(records + (u32(index) < count ? u32(index) * 36 : 0));
        if (!u8(record->mReady)) {
            shaderArchive = gabi::at<majuu::MaterialResource>(u32(repository->mpResource));
            u32 shaderCount = u32(shaderArchive->mCount), compiled = 0;
            if (u32(index) < shaderCount)
                compiled = u32(shaderArchive->mpBindings) + u32(index) * 132;
            majuuApi_initializeMaterial(record, gabi::at<void>(compiled), 0);
            count = u32(repository->mCount);
            records = u32(repository->mpInstances);
        }
        shader = records + (u32(index) < count ? u32(index) * 36 : 0);
    }
    majuuApi_loadMaterialInstances(&packet->mProgramCount, gabi::at<void>(shader), 0);
    (packet->mUniforms.mFormat = 19);
    daMajuuFlagUniformPairs_c* pairs = gabi::at<daMajuuFlagUniformPairs_c>(p + 0xA4);
    (packet->mUniforms.mReserved = gabi::ea(&lVertexFormat));
    for (u32 kind = 0; kind < 2; ++kind)
        for (u32 side = 0; side < 2; ++side) {
            daMajuuFlagUniformBuffer_c* buffer =
                gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(pairs) + kind * 0x254 + side * 0x4A8);
            u32 data = u32(buffer->mpData);
            if (!data) {
                u32 allocation = allocateFlagBuffer(0x460, 0x40);
                if (allocation) {
                    (buffer->mpAllocation = allocation);
                    (buffer->mElementCount = 35);
                }
                data = u32(buffer->mpAllocation);
                (buffer->mpData = data);
            }
            majuuApi_configureVertexBuffer(&buffer->mUniformObject, gabi::at<void>(data), 35,
                                           &pairs->mFormat);
        }
    (pairs->mFlags = 0);
    (pairs->mReady = 1);
    for (u32 materialIndex = 0; materialIndex < u32(packet->mProgramCount); ++materialIndex) {
        u32 count = u32(packet->mMaterialCount);
        majuu::ShapeGroup* material = gabi::at<majuu::ShapeGroup>(u32(packet->mpMaterials));
        if (materialIndex < count)
            material = gabi::at<majuu::ShapeGroup>(gabi::ea(material) + (materialIndex * 20));
        u32 program = u32(material->mpMaterial);
        (material->mpMaterial = 0);
        releaseFlagAttributes(gabi::at<majuu::ShapeGroup>(gabi::ea(material)), 4);
        releaseFlagAttributes(gabi::at<majuu::ShapeGroup>(gabi::ea(material)), 12);
        (material->mpMaterial = program);
        for (u32 kind = 0; kind < 2; ++kind) {
            u32 allocation = allocateFlagBuffer(0x1E8, 4);
            for (u32 side = 0; side < 2; ++side) {
                u32 object = allocation + side * 0xF4;
                if (object)
                    majuuApi_constructAttribute(gabi::at<void>(object));
            }
            if (allocation) {
                (material->mShapes[kind].mpShapes = allocation);
                (material->mShapes[kind].mCount = 2);
            }
        }
        for (u32 kind = 0; kind < 2; ++kind)
            for (u32 side = 0; side < 2; ++side) {
                majuu::ShapeArray* descriptor =
                    gabi::at<majuu::ShapeArray>(gabi::ea(material) + 4 + kind * 8);
                u32 objectCount = u32(descriptor->mCount), object = u32(descriptor->mpShapes);
                if (side < objectCount)
                    object += side * 0xF4;
                majuuApi_configureMaterialAttribute(
                    gabi::at<void>(program), gabi::at<void>(object),
                    gabi::at<void>(gabi::ea(pairs) + 4 + kind * 0x254 + side * 0x4A8),
                    &pairs->mFormat, 0);
            }
    }
    majuuApi_allocateUniformBlock(&packet->mTransformCount, 1, 0);
    u32 indicesCount = lIndexCount;
    majuuApi_setIndexBuffer(&packet->mIndexBuffer, &lIndices, 4, indicesCount);
    f32 zero = lZero;
    (packet->mIndexBuffer.mPrimitive = 4);
    u32 current = u32(pairs->mCurrent);
    for (u32 side = 0; side < 2; ++side) {
        daMajuuFlagUniformBuffer_c* buffer =
            gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(pairs) + (side * 2 + current) * 0x254);
        u32 data = u32(buffer->mpData);
        u32 end = data + 0x460;
        if (data < end) {
            zeroFlagCacheLines(data, 0x460);
            current = u32(packet->mUniforms.mCurrent);
            pairs = gabi::at<daMajuuFlagUniformPairs_c>(p + 0xA4);
            buffer = gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(pairs) +
                                                          (side * 2 + current) * 0x254);
        }
        u32 vertices = lVertexCount;
        data = u32(buffer->mpData);
        for (u32 vertex = 0; vertex < vertices; ++vertex) {
            majuu::Triangle* triangle =
                gabi::at<majuu::Triangle>(gabi::ea(&lTriangles) + vertex * 3);
            majuu::Vector* position = gabi::at<majuu::Vector>(gabi::ea(&lInitialPositions) +
                                                              u8(triangle->mPosition) * 12);
            f32 z = f32(position->mValues[2]), x = f32(position->mValues[0]),
                y = f32(position->mValues[1]);
            u32 output = data + vertex * 32;
            (gabi::at<majuu::Vector>(output)->mValues[0] = x);
            (gabi::at<majuu::Vector>(output)->mValues[1] = y);
            (gabi::at<majuu::Vector>(output)->mValues[2] = z);
            (gabi::at<majuu::Vertex>(output)->mValues[3] = zero);
            (gabi::at<majuu::Vertex>(output)->mValues[4] = zero);
            (gabi::at<majuu::Vertex>(output)->mValues[5] = zero);
            majuu::TexCoord* uv =
                gabi::at<majuu::TexCoord>(gabi::ea(&lTexCoord) + u8(triangle->mTexCoord) * 8);
            f32 u = f32(uv->u), v = f32(uv->v);
            (gabi::at<majuu::Vertex>(output)->mValues[6] = u);
            (gabi::at<majuu::Vertex>(output)->mValues[7] = v);
            vertices = lVertexCount;
        }
        if (vertices)
            current = u32(pairs->mCurrent);
    }
    u32 alternate = gabi::ea(pairs) + (current == 0 ? 0x254 : 0);
    for (u32 side = 0; side < 2; ++side) {
        daMajuuFlagUniformBuffer_c* original =
            gabi::at<daMajuuFlagUniformBuffer_c>(gabi::ea(pairs) + (side * 2 + current) * 0x254);
        daMajuuFlagUniformBuffer_c* opposite =
            gabi::at<daMajuuFlagUniformBuffer_c>(alternate + side * 0x4A8);
        for (u32 vertex = 0; vertex < 35; ++vertex) {
            majuu::Vertex* input = gabi::at<majuu::Vertex>(u32(original->mpData) + vertex * 32);
            majuu::Vertex* output = gabi::at<majuu::Vertex>(u32(opposite->mpData) + vertex * 32);
            for (u32 component = 0; component < 8; ++component)
                (output->mWords[component] = u32(input->mWords[component]));
        }
        current = u32(pairs->mCurrent);
    }
    decltype(daMajuuFlagUniformBuffer_c::mUniformObject)* uniform =
        gabi::at<decltype(daMajuuFlagUniformBuffer_c::mUniformObject)>(gabi::ea(pairs) +
                                                                       current * 0x254 + 4);
    for (u32 side = 0; side < 2; ++side) {
        u32 size = u32(uniform->mSize);
        majuuApi_flushVertexBuffer(uniform, 0, size);
        uniform = gabi::at<decltype(daMajuuFlagUniformBuffer_c::mUniformObject)>(gabi::ea(uniform) +
                                                                                 (0x4A8));
    }
    current = u32(pairs->mCurrent);
    (pairs->mCurrent = current == 0 ? 1 : 0);
    majuuApi_lockGraphics(gabi::at<void>(lGraphicsMutex));
    u8 textureType = u8(actor->mTexType);
    majuu::ResourceImage* image = gabi::at<majuu::ResourceImage>(0);
    if (!textureType) {
        gabi::Local<majuu::ResourceName> archive, texture;
        archive->mpVtable = nameSpace;
        texture->mpVtable = nameSpace;
        archive->mpText = gabi::guest_str("ProgramTexture");
        texture->mpText = gabi::guest_str("ProgramTexture.bfres");
        image = gabi::at<majuu::ResourceImage>(majuuApi_findResourceTexture(
            gabi::at<void>(lTextureRepository), archive.get(), texture.get(), 0));
        majuuApi_decodeTextureSurfaceResource(&packet->mFlagSurface, image,
                                              gabi::at<void>(gabi::guest_str("flag02")));
    } else {
        if (textureType <= 3) {
            gabi::Local<majuu::ResourceName> texture;
            texture->mpVtable = nameSpace;
            texture->mpText = textureType == 1   ? gabi::guest_str("Matif")
                              : textureType == 2 ? gabi::guest_str("Vsvfg")
                                                 : gabi::guest_str("Xhcf");
            image = gabi::at<majuu::ResourceImage>(
                majuuApi_dRes_control_c_getRes(gabi::at<void>(lResourceControl), texture.get(), 3));
        }
        if (!gabi::ea(image))
            majuuApi_assertResource(gabi::at<void>(gabi::guest_str("d_a_majuu_flag.cpp")), 0x197,
                                    gabi::at<void>(gabi::guest_str("tmp_img != (0)")));
        majuuApi_decodeImageSurface(&packet->mFlagSurface, gabi::at<void>(u32(image->mpSurface)));
    }
    gabi::Local<majuu::ResourceName> texture;
    texture->mpVtable = nameSpace;
    texture->mpText = gabi::guest_str("Cloth");
    image = gabi::at<majuu::ResourceImage>(
        majuuApi_dRes_control_c_getRes(gabi::at<void>(lResourceControl), texture.get(), 3));
    if (!gabi::ea(image))
        majuuApi_assertResource(gabi::at<void>(gabi::guest_str("d_a_majuu_flag.cpp")), 0x19F,
                                gabi::at<void>(gabi::guest_str("tmp_img != (0)")));
    majuuApi_decodeImageSurface(&packet->mToonSurface, gabi::at<void>(u32(image->mpSurface)));
    majuuApi_unlockGraphics(gabi::at<void>(lGraphicsMutex));
    updateFlagTexture(p, 0xF60, 0xE40);
    updateFlagTexture(p, 0x10F8, 0xED0);
}

/* WWHD glue (tools/verify/autoglue.py): typed call boundaries.
str 10014418 "Cloth" in 021B9824
str 10014420 "Matif" in 021B9824
str 10014428 "Vsvfg" in 021B9824
str 10014430 "Xhcf" in 021B9824
str 100144E8 "Cloth" in 021BA650
str 100144F0 "Matif" in 021BA650
str 100144F8 "Vsvfg" in 021BA650
str 10014500 "Xhcf" in 021BA650
str 100143A4 "" in 021B9C78
str 10014460 "flag_default" in 021B9C78
str 10014470 "ProgramTexture" in 021B9C78
str 10014480 "ProgramTexture.bfres" in 021B9C78
str 10014438 "Matif" in 021B9C78
str 10014440 "Vsvfg" in 021B9C78
str 10014458 "Xhcf" in 021B9C78
str 10014448 "Cloth" in 021B9C78
str 10014450 "flag02" in 021B9C78
str 10014498 "d_a_majuu_flag.cpp" in 021B9C78
str 100144AC "tmp_img != (0)" in 021B9C78
data lHioVtable 100143CC
data lActorVtable 100143BC
data lPacketVtable 1001452C
data lTransformVtable 1016EF84
data lLightingVtable 1016EFB4
data lVertexFormat 10014520
data lGraphicsSystem 104B45C0
data lDefaultRenderState 104B474C
data lInitialPositions 101B9338
data lTriangles 101B94DC
data lTexCoord 101B9434
data lIndices 101B9548
data lNormalNeighbors 101B97D8
data lSpringNeighbors 101B95E0
data lHeaderStatics 10464ECC
data lHeaderFirst 10464EC0
data lHeaderSecond 10464EC4
data lHeaderFloatGuard 10464EC8
data lHeaderByteGuard 10464EC9
data lHeaderDestructor0 101B9A24
data lHeaderDestructor1 101B9A30
data lHeaderDestructor2 101B9A3C
data lZero 100143DC
data lParentOffsetZ 100143E0
data lDegreesToAngle 100143E4
data lFlagOffsetZ 100143E8
data lColorDivisor 100143EC
data lNormalRipple 10014400
data lWindBlend 10014404
data lOne 10014408
data lDrag 1001440C
data lLongRestLength 10014410
data lShortRestLength 10014414
data lInitialJitter 100144BC
data lSmallScale 100144C0
data lLargeScale 100144C4
data lPlayerFlagScale 100144C8
data lParameterScale 100144CC
data lCullMinX 100144D0
data lCullMinY 100144D4
data lCullMinZ 100144D8
data lCullMaxX 100144DC
data lCullMaxY 100144E0
data lCullMaxZ 100144E4
data lHeaderFirstValue 10014508
data lHeaderSecondValue 1001450C
data lWindSpeed1 10014510
data lHioOffsetZ 10014514
data lGravity 10014518
data lSpring 1001451C
data lGraphicsOne 1014517C
data lGraphicsZero 10145180
data lCalcMtx 1018C7B0
data lVertexCount 101B9310
data lIndexCount 101B9314
data lMonotone 101F4829
data lResourceControl 101F4F28
data lTextureRepository 101F4F7C
data lGraphicsMutex 101F8B18
data lHeapManager 101F8B4C
data lOpaqueDrawBuffer 104B4634
data lEnvironment 104B4708
data lZeroVector 101FFBA8
data lBaseZ 101FFBCC
data l_HIO 10464EDC
data lSinCosTable 104A44F8
data lMatrixStack 1048D0CC
data lViewMatrix 104B45F8
data lProjectionMatrix 104B470C
ext 025F1884 void majuuApi_rotateMatrixY(void*, s16)
ext 028E8DAC void majuuApi_PSVECSubtract(void*, void*, void*)
ext 0273AD10 unsigned int majuuApi_allocate(int)
ext 027B5BD8 void majuuApi_constructVertexBuffer(void*)
ext 027BF734 void majuuApi_constructAttribute(void*)
ext 0273AF40 void majuuApi_deallocate(void*)
ext 027BF880 void majuuApi_destroyVertexBuffer(void*, int)
ext 027B5CBC void majuuApi_destroyVertexResource(void*, int)
ext 028E9098 void majuuApi_PSMTXIdentity(void*)
ext 028E9108 void majuuApi_PSMTXConcat(void*, void*, void*)
ext 02555D0C unsigned int majuuApi_getEnvironment()
ext 025626A4 void majuuApi_dScnKy_env_light_c_settingTevStruct(void*, int, void*, void*)
ext 025200D4 unsigned int majuuApi_getPlay()
ext 027F0E04 void majuuApi_entryDrawBuffer(void*, void*, int)
ext 021B89B4 void majuuApi_updatePacketUniforms(void*)
ext 025204C8 void majuuApi_dComIfG_resDelete(void*, void*)
ext 0257DAA8 unsigned int majuuApi_dKyw_get_wind_vec()
ext 020195B0 int majuuApi_cM_atan2s(f32, f32)
ext 028E90D4 void majuuApi_copyMatrix(void*, void*)
ext 028E9044 void majuuApi_PSMTXMultVecSR(void*, void*, void*)
ext 0200F428 void majuuApi_cLib_addCalcAngleS2(void*, s32, int, int)
ext 021B92DC void majuuApi_moveFlagCloth(void*)
ext 021B8520 void majuuApi_set_mtx(void*)
ext 025F24E0 void majuuApi_mDoMtx_stack_c_transM(f32, f32, f32)
ext 025F1C28 void majuuApi_mDoMtx_YrotM(void*, s16)
ext 025F2518 void majuuApi_scaleMatrix(f32, f32, f32)
ext 028E8F64 void majuuApi_transformVector(void*, void*, void*)
ext 0200FAD8 void majuuApi_MtxTrans(int, f32, f32, f32)
ext 025F1C5C void majuuApi_mDoMtx_ZrotM(void*, s16)
ext 025F1BF4 void majuuApi_mDoMtx_XrotM(void*, s16)
ext 0200FC74 void majuuApi_MtxScale(int, f32, f32, f32)
ext 028F026C void majuuApi_registerDestructor(void*)
ext 028ED6F8 void majuuApi_initializeHeaderFloat(void*)
ext 028EAB2C void majuuApi_initializeHeaderByte(void*)
ext 027F1278 void majuuApi_constructPacketBase(void*)
ext 028EFFD0 void majuuApi_constructArray(void*, int, int, int)
ext 027B5430 void majuuApi_constructIndexBuffer(void*)
ext 027FD6F4 void majuuApi_constructUniformBlock(void*)
ext 027FB40C void majuuApi_constructMaterialBlock(void*)
ext 028F521C void majuuApi_clearMemory(void*, int)
ext 0273AD10 void majuuApi_allocateDiscardResult(int)
ext 027BE6B8 void majuuApi_constructSurfaceTransfer(void*)
ext 027BDF7C void majuuApi_constructTexture(void*)
ext 02755FEC unsigned int majuuApi_findContainingHeap(void*, void*)
ext 027BF7E8 void majuuApi_releaseVertexBuffer(void*)
ext 027BEBEC void majuuApi_releaseMaterialAttribute(void*)
ext 027BE2B0 void majuuApi_destroyTexture(void*, int)
ext 027FB528 void majuuApi_destroyMaterialBlock(void*, int)
ext 027FD764 void majuuApi_destroyUniformBlock(void*, int)
ext 027B54A0 void majuuApi_destroyIndexBuffer(void*, int)
ext 028F0164 void majuuApi_destroy_arr(void*, int, int, int, int, int)
ext 027F13DC void majuuApi_destroyPacketBase(void*, int)
ext 025D50BC void majuuApi_fopAc_ac_c_dtor(void*, int)
ext 0201ADE0 void majuuApi_subtractVector(void*, void*, void*)
ext 0201B080 void majuuApi_crossVector(void*, void*, void*)
ext 0201B1E4 void majuuApi_cXyz_normZC(void*, void*)
ext 028E8D88 void majuuApi_addVector(void*, void*, void*)
ext 0200FCF0 void majuuApi_MtxPush()
ext 0200FCD8 void majuuApi_MtxPosition(void*, void*)
ext 0200FD38 void majuuApi_MtxPull()
ext 027B5E94 void majuuApi_flushVertexBuffer(void*, int, u32)
ext 0255F8F4 void majuuApi_setFogTevstr(void*)
ext 0255FE90 void majuuApi_setLightTevstr(void*)
ext 021B879C void majuuApi_copyMatrixSnapshot(void*, void*)
ext 027FDA54 void majuuApi_setViewUniforms(void*, int, void*, void*, void*)
ext 021B883C void majuuApi_colorS10ToFloat(void*, void*)
ext 0274D458 void majuuApi_scaleColor(void*, void*, f32)
ext 027FDFF4 void majuuApi_submitViewUniforms(void*, int)
ext 021B8900 void majuuApi_colorU8ToFloat(void*, void*)
ext 0274D2AC void majuuApi_scaleColorInPlace(void*, f32)
ext 027FB678 void majuuApi_submitMaterialUniforms(void*)
ext 0257DAA8 void majuuApi_sampleWind()
ext 02578348 float majuuApi_dKyw_get_wind_pow()
ext 028E8E64 void majuuApi_scaleVector(void*, void*, f32)
ext 025F181C void majuuApi_mDoMtx_ZrotS(void*, s16)
ext 028E8F44 void majuuApi_dotVector(void*, void*)
ext 0201AE48 void majuuApi_scaleNormalByDotResult(void*, void*)
ext 028E8DD0 float majuuApi_squareMagnitude(void*)
ext 028F4384 float majuuApi_squareRoot(f32)
ext 021B8D98 void majuuApi_setNormalMatrix(void*)
ext 021B8DA8 void majuuApi_daMajuu_Flag_packet_c_setNrmVtx(void*, void*, s32)
ext 021B9244 void majuuApi_daMajuu_Flag_packet_c_setBackNrm(void*)
ext 025D4ED0 void majuuApi_fopAc_ac_c_fopAc_ac_c(void*)
ext 021B98B4 void majuuApi_constructFlagPacket(void*)
ext 02520460 int majuuApi_dComIfG_resLoad(void*, void*)
ext 02019918 float majuuApi_cM_rndFX(f32)
ext 025D674C void majuuApi_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32)
ext 021B9C78 void majuuApi_setupFlagPacket(void*)
ext 027F29D4 unsigned int majuuApi_getShaderCache(void*)
ext 027BB9E0 void majuuApi_updateShader(void*, int)
ext 027B9F68 void majuuApi_bindShaderProgram(void*)
ext C00060E0 void majuuApi_bindDisplayList(void*, u32)
ext 027BB7CC void majuuApi_bindShader(void*)
ext C0006900 void majuuApi_bindPixelUniform(s16, void*, u32)
ext C0006A38 void majuuApi_bindVertexUniform(s16, void*, u32)
ext C00068A8 void majuuApi_bindGeometryUniform(s16, void*, u32)
ext C0006178 void majuuApi_drawIndexed(u32, u32, u32, void*, int, int)
ext 027FFE54 void majuuApi_bindDrawMaterial(void*, void*)
ext 027BE53C void majuuApi_bindTexture(void*, void*, int, int)
ext 02750250 void majuuApi_constructRenderState(void*)
ext 0280037C void majuuApi_configureMaterialState(unsigned int, void*)
ext 02750370 void majuuApi_applyRenderState(void*)
ext 027BFE5C void majuuApi_bindVertexBuffer(void*)
ext 02750684 void majuuApi_applyCullMode(void*)
ext 02756140 unsigned int majuuApi_getCurrentHeap(void*)
ext 027BDEB4 void majuuApi_copyTextureSurface(void*, void*)
ext 027FFCBC unsigned int majuuApi_getMaterialManager()
ext 027B90AC int majuuApi_findMaterial(void*, void*)
ext 02800B0C void majuuApi_initializeMaterial(void*, void*, int)
ext 0280068C void majuuApi_loadMaterialInstances(void*, void*, int)
ext 027FF478 void majuuApi_configureVertexBuffer(void*, void*, int, void*)
ext 027FF530 void majuuApi_configureMaterialAttribute(void*, void*, void*, void*, int)
ext 027FE084 void majuuApi_allocateUniformBlock(void*, int, int)
ext 027B54E0 void majuuApi_setIndexBuffer(void*, void*, int, u32)
ext 0274FBF8 void majuuApi_lockGraphics(void*)
ext 026124B0 unsigned int majuuApi_findResourceTexture(void*, void*, void*, int)
ext 02773870 void majuuApi_decodeTextureSurfaceResource(void*, void*, void*)
ext 026066C4 unsigned int majuuApi_dRes_control_c_getRes(void*, void*, int)
ext 0273AA24 void majuuApi_assertResource(void*, int, void*)
ext 02773798 void majuuApi_decodeImageSurface(void*, void*)
ext 0274FCCC void majuuApi_unlockGraphics(void*)
*/
