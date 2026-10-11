/* Ship sail: cloth simulation and the HD GX2 material packet. Verified WWHD behavior. */
#include "d/actor/d_a_grid.h"
// HD: GX2 resources and virtual tables are private to this sail unit.
namespace grid_tu {
inline constexpr u32 hioVtable = 0x10010718;
inline constexpr u32 actorVtable = 0x10010708;
inline constexpr u32 packetVtable = 0x10010870;
inline constexpr u32 toonArchiveName = 0x1001072C;
inline constexpr u32 safeStringVtable = 0x100106F0;
inline constexpr u32 shaderArchiveName = 0x100107CC;
inline constexpr u32 drawUniformVtable = 0x1016EF84;
inline constexpr u32 materialUniformVtable = 0x1016EFB4;
inline constexpr u32 vertexAttributes = 0x10010864;
inline constexpr u32 shipArchiveName = 0x100107DC;
inline constexpr u32 alternateSailName = 0x100107E4;
inline constexpr u32 alternateStageName = 0x1001082C;
inline constexpr u32 graphicsSystem = 0x104B45C0;
} // namespace gc_tu
// Model data (vertex positions, texture coordinates, index lists) generated from your copy of the
// game at build time (tools/verify/assets.py, build/verify/include/assets/); never committed.
#include "assets/grid_l_pos__d_a_grid.h"
#include "assets/grid_l_texCoord__d_a_grid.h"
#include "assets/grid_vertexIndices__d_a_grid.h"
#include "assets/grid_indices__d_a_grid.h"
static f32 grid_z_rate_tbl[13] = {0.05000000074505806f,
                                  0.125f,
                                  0.17499999701976776f,
                                  0.15000000596046448f,
                                  0.0625f,
                                  0.15000000596046448f,
                                  0.20000000298023224f,
                                  0.15000000596046448f,
                                  0.07500000298023224f,
                                  0.17499999701976776f,
                                  0.17499999701976776f,
                                  0.10000000149011612f,
                                  0.0f};
static f32 grid_z_rate_tbl2[13] = {1.0f,
                                   0.42500001192092896f,
                                   0.44999998807907104f,
                                   0.4000000059604645f,
                                   0.20000000298023224f,
                                   0.4000000059604645f,
                                   0.44999998807907104f,
                                   0.4000000059604645f,
                                   0.20000000298023224f,
                                   0.5f,
                                   0.75f,
                                   1.0f,
                                   1.0f};
static f32 grid_x_rate_tbl[13] = {1.0f,
                                  0.949999988079071f,
                                  0.8999999761581421f,
                                  0.8500000238418579f,
                                  0.800000011920929f,
                                  0.75f,
                                  0.699999988079071f,
                                  0.6499999761581421f,
                                  0.550000011920929f,
                                  0.4000000059604645f,
                                  0.25f,
                                  0.10000000149011612f,
                                  0.0f};
namespace {} // namespace

// Reserve the outgoing linkage words below each vector, so a real nested callee
// saving LR at incoming SP+4 cannot overwrite the first vector coordinates.
struct VectorFrame {
    u8 linkage[8];
    cXyz vector;
    u8 tail[12];
};

static void vectorCopy(cXyz* dst, const cXyz* src) {
    u32 x = static_cast<u32>(grid_words(src).x), y = static_cast<u32>(grid_words(src).y),
        z = static_cast<u32>(grid_words(src).z);
    grid_words(dst).x = x;
    grid_words(dst).y = y;
    grid_words(dst).z = z;
}

// inlined into daGrid_c::_draw
static void copySailLighting(daGrid_c* actor, const void* ship) {
    // HD: three light records; float copies and integer word copies remain distinct.
    auto& dst = gabi::object_at<GridActorLighting>(gabi::ea(actor));
    const auto& src = gabi::object_at<GridActorLighting>(gabi::ea(ship));
    for (u32 i = 0; i < 6; ++i)
        dst.primary.vectors[i] = src.primary.vectors[i];
    for (u32 i = 0; i < 4; ++i)
        dst.primary.color[i] = src.primary.color[i];
    for (u32 i = 0; i < 4; ++i)
        dst.primary.parameters[i] = src.primary.parameters[i];
    for (u32 i = 0; i < 8; ++i)
        dst.primary.attenuation[i] = src.primary.attenuation[i];
    dst.worldPosition.x = src.worldPosition.x;
    dst.worldPosition.y = src.worldPosition.y;
    dst.worldPosition.z = src.worldPosition.z;
    for (u32 i = 0; i < 4; ++i)
        dst.colorC0[i] = src.colorC0[i];
    dst.colorK0 = src.colorK0;
    dst.colorK1 = src.colorK1;
    for (u32 i = 0; i < 4; ++i)
        dst.fogColor[i] = src.fogColor[i];
    for (u32 i = 0; i < 3; ++i)
        dst.fogParameters[i] = src.fogParameters[i];
    for (u32 i = 0; i < 9; ++i)
        dst.state[i] = src.state[i];
    for (u32 i = 0; i < 6; ++i)
        dst.secondary.vectors[i] = src.secondary.vectors[i];
    for (u32 i = 0; i < 4; ++i)
        dst.secondary.color[i] = src.secondary.color[i];
    for (u32 i = 0; i < 4; ++i)
        dst.secondary.parameters[i] = src.secondary.parameters[i];
    for (u32 i = 0; i < 8; ++i)
        dst.secondary.attenuation[i] = src.secondary.attenuation[i];
    for (u32 i = 0; i < 6; ++i)
        dst.extra.vectors[i] = src.extra.vectors[i];
    for (u32 i = 0; i < 4; ++i)
        dst.extra.color[i] = src.extra.color[i];
    for (u32 i = 0; i < 4; ++i)
        dst.extra.parameters[i] = src.extra.parameters[i];
    for (u32 i = 0; i < 8; ++i)
        dst.extra.attenuation[i] = src.extra.attenuation[i];
}

static void heapFreeMember(void* object, u32 off) {
    void* heap = grid_findOwningHeap(
        static_cast<void*>(grid_pointer<GridHeapManager>(static_cast<u32>(grid_heapManager()))),
        static_cast<void*>(grid_pointer<GridAllocation>(static_cast<u32>(grid_ownedAddress(object, off)))));
    u32 vt = static_cast<u32>(gabi::object_at<GridHeap>(gabi::ea(heap)).vtable),
        target = static_cast<u32>(gabi::object_at<GridHeapVtable>(vt).free);
    gabi::call(
        target, heap,
        static_cast<void*>(grid_pointer<GridAllocation>(static_cast<u32>(grid_ownedAddress(object, off)))));
}
static void releaseMaterialPool(void* pool) {
    for (u32 i = 0; i < 4; ++i) {
        void* material = static_cast<void*>(&grid_pool(pool).materials[i]);
        grid_releaseShader(static_cast<void*>(&grid_material(material).shaderObject));
        bool allocated = static_cast<u32>(grid_material(material).allocation.v) != 0;
        grid_material(material).vertices.v = 0;
        if (allocated) {
            (void)static_cast<u32>(grid_material(material).allocationCount);
            heapFreeMember(material, 0x250);
            grid_material(material).allocationCount = 0;
            grid_material(material).allocation.v = 0;
        }
    }
}
static void releaseShaderArrays(void* collection) {
    u32 array = static_cast<u32>(grid_collection(collection).frontShaders.v);
    grid_collection(collection).program.v = 0;
    if (array) {
        for (s32 i = 0; i < static_cast<s32>(grid_collection(collection).frontShaderCount); ++i) {
            void* child = static_cast<void*>(grid_pointer<GridShader>(array + i * 0xF4));
            u32 vt = static_cast<u32>(gabi::object_at<GridShader>(gabi::ea(child)).vtable);
            gabi::call(static_cast<u32>(gabi::object_at<GridShaderVtable>(vt).destroy), child, 2);
            array = static_cast<u32>(grid_collection(collection).frontShaders.v);
        }
        heapFreeMember(collection, 8);
        grid_collection(collection).frontShaderCount = 0;
        grid_collection(collection).frontShaders.v = 0;
    }
    array = static_cast<u32>(grid_collection(collection).backShaders.v);
    if (array) {
        for (s32 i = 0; i < static_cast<s32>(grid_collection(collection).backShaderCount); ++i) {
            void* child = static_cast<void*>(grid_pointer<GridShader>(array + i * 0xF4));
            u32 vt = static_cast<u32>(gabi::object_at<GridShader>(gabi::ea(child)).vtable);
            gabi::call(static_cast<u32>(gabi::object_at<GridShaderVtable>(vt).destroy), child, 2);
            array = static_cast<u32>(grid_collection(collection).backShaders.v);
        }
        heapFreeMember(collection, 0x10);
        grid_collection(collection).backShaderCount = 0;
        grid_collection(collection).backShaders.v = 0;
    }
}
static void releaseSailPacket(void* packet) {
    void* pool = static_cast<void*>(&grid_packet(packet).materialPool);
    gabi::object_at<GridPacketPrefix>(gabi::ea(packet)).vtable = grid_tu::packetVtable;
    releaseMaterialPool(pool);
    grid_pool(pool).unused = 0;
    for (s32 i = 0; i < static_cast<s32>(grid_packet(packet).drawMaterialCount); ++i) {
        u32 base = static_cast<u32>(grid_packet(packet).drawMaterials.v);
        if ((u32)i < static_cast<u32>(grid_packet(packet).drawMaterialCount))
            base += i * 0x23C;
        grid_destroyUniformBinding(static_cast<void*>(&gabi::object_at<GridUniformObject>(base).bindings[0]));
        grid_destroyUniformBinding(static_cast<void*>(&gabi::object_at<GridUniformObject>(base).bindings[1]));
    }
    for (void* binding : {static_cast<void*>(&grid_packet(packet).drawUniform.bindings[0]),
                          static_cast<void*>(&grid_packet(packet).drawUniform.bindings[1]),
                          static_cast<void*>(&grid_packet(packet).materialUniformObject.bindings[0]),
                          static_cast<void*>(&grid_packet(packet).materialUniformObject.bindings[1])})
        grid_destroyUniformBinding(binding);
    for (void* texture : {static_cast<void*>(&grid_packet(packet).toonTexture),
                          static_cast<void*>(&grid_packet(packet).alternateTexture),
                          static_cast<void*>(&grid_packet(packet).frontTexture)})
        grid_destroyTexture(texture, 2);
    grid_destroyIndexBuffer(static_cast<void*>(&grid_packet(packet).indexBuffer), 2);
    grid_releaseUniformObject(static_cast<void*>(&grid_packet(packet).materialUniformObject), 0);
    grid_releaseUniformObject(static_cast<void*>(&grid_packet(packet).drawUniform), 0);
    grid_destroyMaterialList(static_cast<void*>(&grid_packet(packet).drawMaterialCount), 2);
    if (pool) {
        releaseMaterialPool(pool);
        grid_pool(pool).unused = 0;
        grid_destroy_arr(pool, 4, 0x254,
                         grid_callbackPointer(reinterpret_cast<const void*>(&materialDestroy)), 0, 0);
    }
    u32 collections = static_cast<u32>(grid_packet(packet).shaderCollections.v);
    if (collections) {
        for (s32 i = 0; i < static_cast<s32>(grid_packet(packet).shaderCollectionCount); ++i) {
            void* collection = static_cast<void*>(grid_pointer<GridShaderCollection>(collections + i * 0x14));
            if (collection)
                releaseShaderArrays(collection);
            collections = static_cast<u32>(grid_packet(packet).shaderCollections.v);
        }
        heapFreeMember(packet, 0xCE4);
        grid_packet(packet).shaderCollectionCount = 0;
        grid_packet(packet).shaderCollections.v = 0;
    }
    grid_destroyMaterialPacket(packet, 0);
}

struct MaterialFrame {
    u8 linkage[8];
    be<u32> words[12];
    u8 padding[8];
};
static bool textureEqual(void* packet, u32 first, u32 second) {
    for (u32 off : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
        if (static_cast<u32>(grid_textureWord(packet, first, off)) !=
            static_cast<u32>(grid_textureWord(packet, second, off)))
            return false;
    return true;
}

static f32 sailSin(u32 angle) { return static_cast<f32>(grid_sinCos(angle).sine); }
static f32 sailCos(u32 angle) { return static_cast<f32>(grid_sinCos(angle).cosine); }

static void* allocateFromHeap(u32 size, u32 alignment) {
    void* heap = grid_getCurrentHeap(
        static_cast<void*>(grid_pointer<GridHeapManager>(static_cast<u32>(grid_heapManager()))));
    u32 vt = static_cast<u32>(gabi::object_at<GridHeap>(gabi::ea(heap)).vtable);
    return gabi::call<void*>(static_cast<u32>(gabi::object_at<GridHeapVtable>(vt).allocate), heap, size,
                             alignment);
}
static void* shaderCollection(void* packet, u32 index) {
    u32 count = static_cast<u32>(grid_packet(packet).shaderCollectionCount),
        base = static_cast<u32>(grid_packet(packet).shaderCollections.v);
    return static_cast<void*>(grid_pointer<GridShaderCollection>(
        base + (index < count ? index * sizeof(GridShaderCollection) : 0)));
}
static void copyTextureHandle(void* packet, u32 dst, u32 src) {
    u32 handle = static_cast<u32>(grid_texture(packet, src).handle),
        offset = static_cast<u32>(grid_texture(packet, src).offset);
    grid_texture(packet, dst).duplicateHandle = handle;
    grid_texture(packet, dst).duplicateOffset = offset;
    grid_texture(packet, dst).handle = handle;
    grid_texture(packet, dst).offset = offset;
}

struct RenderFrame {
    u8 linkage[8];
    u8 state[0x120];
};
static void virtualMaterialApply(void* material, void* shader) {
    u32 vt = static_cast<u32>(gabi::object_at<GridMaterialInterface>(gabi::ea(material)).vtable);
    gabi::call(static_cast<u32>(gabi::object_at<GridMaterialVtable>(vt).apply), material, shader);
}
static void applyShaderLocations(void* shader, void* material) {
    u32 location =
        static_cast<u32>(gabi::object_at<GridMaterialBindings>(gabi::ea(material)).currentLocation);
    void* binding =
        static_cast<void*>(&gabi::object_at<GridUniformObject>(gabi::ea(material)).bindings[location]);
    void* info = static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).uniformCount)
                     ? static_cast<void*>(grid_pointer<GridShaderLocations>(
                           static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).uniformInfo)))
                     : nullptr;
    s16 vertex = static_cast<s16>(gabi::object_at<GridShaderLocations>(gabi::ea(info)).vertex),
        pixel = static_cast<s16>(gabi::object_at<GridShaderLocations>(gabi::ea(info)).pixel),
        geometry = static_cast<s16>(gabi::object_at<GridShaderLocations>(gabi::ea(info)).geometry);
    u32 size = static_cast<u32>(gabi::object_at<GridUniformBinding>(gabi::ea(binding)).size),
        data = static_cast<u32>(gabi::object_at<GridUniformBinding>(gabi::ea(binding)).data);
    if (pixel != -1)
        grid_GX2SetPixelUniformBlock(pixel, data, size);
    if (vertex != -1)
        grid_GX2SetVertexUniformBlock(vertex, data, size);
    if (geometry != -1)
        grid_GX2SetGeometryUniformBlock(geometry, data, size);
}
static void applyShaderTextures(void* packet, void* shader) {
    bool alternate = static_cast<u8>(grid_alternateSail()) != 0;
    u32 count = static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).textureCount);
    u32 textures = count ? static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).textures) : 0;
    grid_bindTexture(static_cast<void*>(&(alternate ? grid_packet(packet).alternateTexture
                                                    : grid_packet(packet).frontTexture)),
                     static_cast<void*>(grid_pointer<GridTextureBinding>(textures + 4)), -1, 0);
    count = static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).textureCount);
    textures =
        count > 1 ? static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).textures) + 0x14 : 0;
    grid_bindTexture(static_cast<void*>(&grid_packet(packet).toonTexture),
                     static_cast<void*>(grid_pointer<GridTextureBinding>(textures + 4)), -1, 0);
}

/* 021635F0 */
void daHo_packet_c::setBackNrm() {

    for (u32 i = 0; i < 85; ++i) {
        void* back = static_cast<void*>(&this->mBackNrm[i]);
        grid_vector(back).x = 0.f;
        grid_vector(back).y = 0.f;
        grid_vector(back).z = 0.f;
        grid_PSVECSubtract(back, static_cast<void*>(&this->mNrm[i]), back);
    }
}

/* 02162F6C */
s32 daHo_packet_c::setNrmMtx() {

    s16 angle = static_cast<s16>(this->normalAngle);
    void* matrix = static_cast<void*>(grid_pointer<Mtx34>(static_cast<u32>(grid_matrixAddress())));
    return grid_mDoMtx_YrotS(matrix, angle);
}

/* 02162F7C */
void daHo_packet_c::setNrmVtx(cXyz* out, s32 x, s32 y) {

    gabi::Local<VectorFrame> position, total, xDiff, yDiff, normal, scratch;
    void* source = static_cast<void*>(&this->mPos[x + y * 7]);
    position->vector.x = static_cast<f32>(grid_vector(source).x);
    position->vector.y = static_cast<f32>(grid_vector(source).y);
    position->vector.z = static_cast<f32>(grid_vector(source).z);
    total->vector.x = 0.f;
    total->vector.y = 0.f;
    total->vector.z = 0.f;
    auto subtract = [&](s32 index, cXyz* dst) {
        grid_cXyz____mi(static_cast<void*>(&this->mPos[index]), &scratch->vector, &position->vector);
        vectorCopy(dst, &scratch->vector);
    };
    auto accumulate = [&](bool reversed) {
        grid_cXyz__outprod(reversed ? &yDiff->vector : &xDiff->vector, &scratch->vector,
                           reversed ? &xDiff->vector : &yDiff->vector);
        vectorCopy(&normal->vector, &scratch->vector);
        grid_cXyz__normZC(&normal->vector, &scratch->vector);
        vectorCopy(&normal->vector, &scratch->vector);
        grid_PSVECAdd(&total->vector, &normal->vector, &total->vector);
    };
    if (x != 0) {
        subtract(y * 7 + x - 1, &xDiff->vector);
        if (y != 0 && y != 9) {
            subtract((y - 1) * 7 + x, &yDiff->vector);
            accumulate(false);
        }
        if (y == 11) {
            subtract(84, &yDiff->vector);
            accumulate(true);
        } else if (y != 8) {
            subtract((y + 1) * 7 + x, &yDiff->vector);
            accumulate(true);
        }
    }
    if (x != 6) {
        subtract(y * 7 + x + 1, &xDiff->vector);
        if (y != 0 && y != 9) {
            subtract((y - 1) * 7 + x, &yDiff->vector);
            accumulate(true);
        }
        if (y == 11) {
            subtract(84, &yDiff->vector);
            accumulate(false);
        } else if (y != 8) {
            subtract((y + 1) * 7 + x, &yDiff->vector);
            accumulate(false);
        }
    }
    if (y > 7)
        total->vector.y = 0.f;
    if (!grid_cXyz__normalizeRS(&total->vector)) {
        total->vector.x = 1.f;
        total->vector.y = 0.f;
        total->vector.z = 0.f;
    }
    grid_MtxPush();
    u16 angle = (u16)((u32)(x + y) * -800u);
    f32 sinAngle = static_cast<f32>(grid_sinCos(angle).sine);
    s16 rotation = (s16)gabi::ftoi(900.f * sinAngle);
    void* matrix = static_cast<void*>(grid_pointer<Mtx34>(static_cast<u32>(grid_matrixAddress())));
    grid_mDoMtx_YrotM(matrix, rotation);
    grid_MtxPosition(&total->vector, &normal->vector);
    grid_cXyz__normZC(&normal->vector, &scratch->vector);
    vectorCopy(out, &scratch->vector);
    grid_MtxPull();
}

/* 021634F0 */
void daHo_packet_c::setTopNrmVtx(cXyz* out) {

    gabi::Local<VectorFrame> scratch, first, second, normal, transformed;
    grid_cXyz____mi_2(static_cast<void*>(&this->mPos[77]), &scratch->vector,
                      static_cast<void*>(&this->mPos[84]));
    vectorCopy(&first->vector, &scratch->vector);
    grid_cXyz____mi_2(static_cast<void*>(&this->mPos[83]), &scratch->vector,
                      static_cast<void*>(&this->mPos[84]));
    vectorCopy(&second->vector, &scratch->vector);
    grid_cXyz__outprod(&first->vector, &scratch->vector, &second->vector);
    vectorCopy(&normal->vector, &scratch->vector);
    grid_cXyz__normZC(&normal->vector, &scratch->vector);
    vectorCopy(&normal->vector, &scratch->vector);
    grid_MtxPosition(&normal->vector, &transformed->vector);
    grid_cXyz__normZC(&transformed->vector, &scratch->vector);
    vectorCopy(out, &scratch->vector);
}

/* 021625DC */
void daHo_packet_c::updateMaterial() {

    if (!static_cast<u8>(this->textureInitialized)) {
        grid_lockTextureManager(
            static_cast<void*>(grid_pointer<GridTextureManager>(static_cast<u32>(grid_textureManager()))));
        gabi::Local<MaterialFrame> name;
        name->words[0] = grid_tu::toonArchiveName;
        name->words[1] = grid_tu::safeStringVtable;
        void* resource = grid_getObjectResource(
            static_cast<void*>(grid_pointer<GridResourceManager>(static_cast<u32>(grid_resourceManager()))),
            &name->words[0], 3);
        grid_loadTextureResource(static_cast<void*>(&this->toonTextureResource),
                                 static_cast<void*>(grid_pointer<GridTextureData>(static_cast<u32>(
                                     gabi::object_at<GridShaderResource>(gabi::ea(resource)).textureData))));
        if (!textureEqual(this, 0x1ED4, 0x206C))
            grid_assignTexture(static_cast<void*>(&this->toonTexture),
                               static_cast<void*>(&this->toonTextureResource));
        else {
            u32 handle = static_cast<u32>(grid_texture(this, 0x206C).handle),
                offset = static_cast<u32>(grid_texture(this, 0x206C).offset);
            grid_texture(this, 0x1ED4).duplicateHandle = handle;
            grid_texture(this, 0x1ED4).duplicateOffset = offset;
            grid_texture(this, 0x1ED4).handle = handle;
            grid_texture(this, 0x1ED4).offset = offset;
        }
        grid_unlockTextureManager(
            static_cast<void*>(grid_pointer<GridTextureManager>(static_cast<u32>(grid_textureManager()))));
        this->textureInitialized = 1;
    }
    void* pool = static_cast<void*>(&this->materialPool);
    for (u32 side = 0; side < 2; ++side) {
        u32 index = static_cast<u32>(grid_pool(pool).current) + side * 2;
        void* vertices = grid_pool(pool).materials[index].vertices.get();
        for (u32 i = 0; i < 172; ++i) {
            u32 pos = grid_vertexIndices[i * 3], normal = grid_vertexIndices[i * 3 + 1];
            void* source = static_cast<void*>(&this->mPos[pos]);
            f32 x = static_cast<f32>(grid_vector(source).x), y = static_cast<f32>(grid_vector(source).y),
                z = static_cast<f32>(grid_vector(source).z);
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).position.x = x;
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).position.y = y;
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).position.z = z;
            source = static_cast<void*>(&(side ? this->mBackNrm[normal] : this->mNrm[normal]));
            x = static_cast<f32>(grid_vector(source).x);
            y = static_cast<f32>(grid_vector(source).y);
            z = static_cast<f32>(grid_vector(source).z);
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).normal.x = x;
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).normal.y = y;
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).normal.z = z;
        }
    }
    u32 index = static_cast<u32>(grid_pool(pool).current);
    for (u32 side = 0; side < 2; ++side) {
        void* material = static_cast<void*>(&grid_pool(pool).materials[index + side * 2].vertexBuffer);
        u32 length = static_cast<u32>(gabi::object_at<GridVertexBufferView>(gabi::ea(material)).byteLength);
        grid_flushVertexBuffer(material, 0, length);
    }
    grid_pool(pool).current = static_cast<u32>(grid_pool(pool).current) == 0 ? 1 : 0;
    void* lighting = this->mpTevStr.get();
    grid_updateLighting(lighting);
    gabi::Local<MaterialFrame> view, color, model;
    grid_copyViewMatrix(&view->words[0], static_cast<void*>(&grid_viewMatrix()));
    u32 projection = static_cast<u32>(grid_projection());
    grid_setMaterialView(static_cast<void*>(&this->drawMaterialCount), 0, &view->words[0],
                         static_cast<void*>(&grid_projectionMatrix()),
                         static_cast<void*>(grid_pointer<GridProjection>(projection + 0x240)));
    lighting = this->mpTevStr.get();
    u32 materialBase = static_cast<u32>(this->drawMaterials.v);
    grid_signedColorToFloat(&color->words[0], static_cast<void*>(&grid_lighting(lighting).mColorC0));
    lighting = this->mpTevStr.get();
    f32 attenuation = static_cast<f32>(gabi::object_at<GridLightAttenuation>(gabi::ea(lighting)).material);
    grid_scaleMaterialColor(static_cast<void*>(grid_pointer<GridFloatColor>(materialBase + 0x1C4)),
                            &color->words[0], attenuation);
    lighting = this->mpTevStr.get();
    grid_signedColorToFloat(
        &color->words[0],
        static_cast<void*>(&gabi::object_at<GridLightExtraColor>(gabi::ea(lighting)).color));
    lighting = this->mpTevStr.get();
    attenuation = static_cast<f32>(gabi::object_at<GridLightAttenuation>(gabi::ea(lighting)).extra);
    grid_scaleMaterialColor(static_cast<void*>(grid_pointer<GridFloatColor>(materialBase + 0x1D4)),
                            &color->words[0], attenuation);
    this->modelFlags = 0;
    for (u32 i = 0; i < 12; ++i)
        gabi::object_at<Mtx34>(gabi::ea(&model->words[0])).m[i / 4][i % 4] = this->mMtx.m[i / 4][i % 4];
    grid_PSMTXCopy_2(&model->words[0], static_cast<void*>(&this->modelMatrix));
    u8 alpha = static_cast<u8>(this->mAlpha);
    lighting = this->mpTevStr.get();
    grid_lighting(lighting).mColorC0.a = alpha;
    lighting = this->mpTevStr.get();
    s16 r = static_cast<s16>(grid_lighting(lighting).mColorC0.r),
        g = static_cast<s16>(grid_lighting(lighting).mColorC0.g),
        b = static_cast<s16>(grid_lighting(lighting).mColorC0.b),
        a = static_cast<s16>(grid_lighting(lighting).mColorC0.a);
    grid_uniformColor(this, 0x17b8).r = r / 255.f;
    grid_uniformColor(this, 0x17b8).g = g / 255.f;
    grid_uniformColor(this, 0x17b8).b = b / 255.f;
    grid_uniformColor(this, 0x17b8).a = a / 255.f;
    u8 red = static_cast<u8>(grid_lighting(lighting).mColorK0.r),
       green = static_cast<u8>(grid_lighting(lighting).mColorK0.g),
       blue = static_cast<u8>(grid_lighting(lighting).mColorK0.b),
       opacity = static_cast<u8>(grid_lighting(lighting).mColorK0.a);
    grid_uniformColor(this, 0x17c8).r = red / 255.f;
    grid_uniformColor(this, 0x17c8).g = green / 255.f;
    grid_uniformColor(this, 0x17c8).b = blue / 255.f;
    grid_uniformColor(this, 0x17c8).a = opacity / 255.f;
    attenuation = static_cast<f32>(gabi::object_at<GridLightAttenuation>(gabi::ea(lighting)).color);
    grid_scaleUniformColor(static_cast<void*>(&grid_uniformColor(this, 0x17C8)), attenuation);
    if (static_cast<u8>(grid_lighting(lighting).mColorK1.a))
        grid_byteColorToFloat(static_cast<void*>(&grid_uniformColor(this, 0x17D8)),
                              static_cast<void*>(&grid_lighting(lighting).mColorK1));
    else
        for (u32 i = 0; i < 4; ++i)
            this->uniformState.initialColors[24 + i] = 0.f;
    grid_updateMaterialUniforms(static_cast<void*>(&this->drawMaterialCount), 0);
}

/* 021655E4 */
void daHo_packet_c::draw(void* drawInfo) {

    gabi::Local<MaterialFrame> names;
    names->words[0] = grid_tu::alternateStageName;
    names->words[1] = grid_tu::safeStringVtable;
    void* play = grid_getPlay();
    names->words[2] = gabi::ea(&gabi::object_at<GridStartStage>(gabi::ea(play)).name[0]);
    names->words[3] = grid_tu::safeStringVtable;
    auto stringAssure = [&](u32 off) {
        u32 vt = static_cast<u32>(names->words[off / 4 + 1]);
        gabi::call(static_cast<u32>(gabi::object_at<GridStringVtable>(vt).assureTermination),
                   static_cast<void*>(&names->words[off / 4]));
    };
    stringAssure(0);
    stringAssure(0);
    u32 first = names->words[0];
    stringAssure(8);
    u32 second = names->words[2];
    bool equal = first == second;
    if (!equal) {
        equal = true;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 a = static_cast<u8>(grid_stringByte(first, i)),
               b = static_cast<u8>(grid_stringByte(second, i));
            if (a != b) {
                equal = false;
                break;
            }
            if (!a)
                break;
        }
    }
    if (equal) {
        play = grid_getPlay();
        void* player = static_cast<void*>(grid_pointer<fopAc_ac_c>(
            static_cast<u32>(gabi::object_at<GridPlayPlayer>(gabi::ea(play)).player)));
        if (static_cast<s8>(gabi::object_at<fopAc_ac_c>(gabi::ea(player)).current.roomNo) >= 7)
            return;
    }
    s32 kind = static_cast<s32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).kind);
    void* shader = nullptr;
    if (kind < 4) {
        void* collection = shaderCollection(this, (u32)kind);
        shader = grid_collection(collection).program.get();
    }
    void* system = static_cast<void*>(grid_pointer<GridGraphicsSystem>(grid_tu::graphicsSystem));
    void* active = grid_getActiveShader(system);
    void* program = static_cast<void*>(grid_pointer<GridShaderProgram>(
        static_cast<u32>(gabi::object_at<GridShaderObject>(gabi::ea(shader)).program)));
    if (gabi::ea(program) != static_cast<u32>(gabi::object_at<GridActiveShader>(gabi::ea(active)).program)) {
        u8 flags = static_cast<u8>(gabi::object_at<GridShaderProgram>(gabi::ea(program)).flags);
        u32 previous = static_cast<u32>(gabi::object_at<GridActiveShader>(gabi::ea(active)).group);
        if (flags & 2) {
            gabi::object_at<GridShaderProgram>(gabi::ea(program)).flags = flags & ~2u;
            grid_updateShaderProgram(program, 0);
        }
        void* resource = static_cast<void*>(grid_pointer<GridShaderResource>(
            static_cast<u32>(gabi::object_at<GridShaderProgram>(gabi::ea(program)).resource)));
        u32 group = static_cast<u32>(gabi::object_at<GridShaderResource>(gabi::ea(resource)).group);
        if (previous != group)
            grid_bindShaderGroup(static_cast<void*>(grid_pointer<GridShaderGroup>(group)));
        if (static_cast<u32>(gabi::object_at<GridShaderProgram>(gabi::ea(program)).displayListSize))
            grid_GX2CallDisplayList(
                static_cast<void*>(grid_pointer<GridDisplayList>(
                    static_cast<u32>(gabi::object_at<GridShaderProgram>(gabi::ea(program)).displayList))),
                static_cast<u32>(gabi::object_at<GridShaderProgram>(gabi::ea(program))
                                     .displayListSize)); /* GX2CallDisplayList(list, size): r4 = size (game
                                                            test 2026-10-05: KoRL sail colours) */
        else
            grid_bindShaderProgram(program);
        gabi::object_at<GridActiveShader>(gabi::ea(active)).group = group;
        gabi::object_at<GridActiveShader>(gabi::ea(active)).program = gabi::ea(program);
    }
    gabi::Local<RenderFrame> frame;
    void* state = &frame->state[0];
    kind = static_cast<s32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).kind);
    if (kind == 0) {
        virtualMaterialApply(static_cast<void*>(&this->materialUniformObject), shader);
        virtualMaterialApply(static_cast<void*>(&this->drawUniform), shader);
        void* extra = static_cast<void*>(grid_pointer<GridMaterialExtra>(
            static_cast<u32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).extra)));
        if (extra)
            applyShaderLocations(shader, static_cast<void*>(grid_pointer<GridUniformObject>(static_cast<u32>(
                                             gabi::object_at<GridMaterialExtra>(gabi::ea(extra)).material))));
        applyShaderTextures(this, shader);
        grid_initializeRenderState(state);
    } else if (kind == 1 || kind == 2) {
        applyShaderLocations(shader, this->drawMaterials.get());
        virtualMaterialApply(static_cast<void*>(&this->materialUniformObject), shader);
        virtualMaterialApply(static_cast<void*>(&this->drawUniform), shader);
        if (kind == 2) {
            void* extra = static_cast<void*>(grid_pointer<GridUniformObject>(
                static_cast<u32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).alternateExtra)));
            if (extra) {
                u32 vt = static_cast<u32>(gabi::object_at<GridMaterialInterface>(gabi::ea(extra)).vtable);
                gabi::call(static_cast<u32>(gabi::object_at<GridMaterialVtable>(vt).apply), extra, shader);
            }
        }
        applyShaderTextures(this, shader);
        if (kind == 2)
            grid_applyExtraMaterial(drawInfo, shader);
        grid_initializeRenderState(state);
    } else
        grid_initializeRenderState(state);
    gabi::object_at<GridRenderState>(gabi::ea(state)).cullMode = 0;
    gabi::object_at<GridRenderState>(gabi::ea(state)).mode = 3;
    gabi::object_at<GridRenderState>(gabi::ea(state)).alphaTest = 0;
    u32 flags = static_cast<u32>(gabi::object_at<GridRenderState>(gabi::ea(state)).flags);
    u32 packed = (((flags & ~15u) + 7) & 0xFFFFFF0Fu) + 0x10;
    gabi::object_at<GridRenderState>(gabi::ea(state)).flags = packed;
    kind = static_cast<s32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).kind);
    grid_configureRenderState(kind, state);
    if (static_cast<u8>(this->mAlpha) != 255 &&
        static_cast<u32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).kind) == 1) {
        gabi::object_at<GridRenderState>(gabi::ea(state)).colorWrite = 0;
        gabi::object_at<GridRenderState>(gabi::ea(state)).blendEnabled = 1;
    }
    gabi::object_at<GridRenderState>(gabi::ea(state)).alphaTest = 0;
    grid_applyRenderState(state);
    auto selectedArray = [&]() {
        void* collection =
            shaderCollection(this, static_cast<u32>(gabi::object_at<GridDrawInfo>(gabi::ea(drawInfo)).kind));
        u32 current = static_cast<u32>(this->materialPool.current);
        return (current ? collection : static_cast<void*>(&grid_collection(collection).frontShaders));
    };
    void* array = selectedArray();
    grid_bindShader(static_cast<void*>(grid_pointer<GridShader>(
        static_cast<u32>(gabi::object_at<GridShaderArraySelection>(gabi::ea(array)).array))));
    auto draw = [&]() {
        u32 count =
            static_cast<u32>(gabi::object_at<GridIndexBuffer>(gabi::ea(this->indexBuffer)).indexCount);
        if (count) {
            u32 type =
                    static_cast<u32>(gabi::object_at<GridIndexBuffer>(gabi::ea(this->indexBuffer)).indexType),
                offset = static_cast<u32>(
                    gabi::object_at<GridIndexBuffer>(gabi::ea(this->indexBuffer)).indexOffset),
                indices = static_cast<u32>(
                    gabi::object_at<GridIndexBuffer>(gabi::ea(this->indexBuffer)).primitiveMode);
            grid_GX2DrawIndexedEx(indices, count, type, offset, 0, 1);
        }
    };
    draw();
    gabi::object_at<GridRenderState>(gabi::ea(state)).cullMode = 1;
    grid_applyRenderState(state);
    array = selectedArray();
    u32 count = static_cast<u32>(gabi::object_at<GridShaderArraySelection>(gabi::ea(array)).count),
        shaders = static_cast<u32>(gabi::object_at<GridShaderArraySelection>(gabi::ea(array)).array);
    if (count > 1)
        shaders += 0xF4;
    grid_bindShader(static_cast<void*>(grid_pointer<GridShader>(shaders)));
    draw();
    grid_applyRenderState(static_cast<void*>(&gabi::object_at<GridGraphicsSystem>(gabi::ea(system)).state));
}

/* 02162F68 */
static s32 daGrid_Draw(daGrid_c* a) { return grid_daGrid_c___draw(a); }

/* 02163678 */
static void ho_move(daGrid_c* actor) {
    if (static_cast<u8>(grid_hio().freeze))
        return;
    grid_getPlay();
    void* wind = grid_dKyw_get_wind_vec();
    f32 power = grid_dKyw_get_wind_pow();
    actor->waveStepSin = 7500;
    actor->waveStepCos = 7200;
    f32 windX = static_cast<f32>(grid_vector(wind).x), windZ = static_cast<f32>(grid_vector(wind).z);
    void* ship = static_cast<void*>(grid_pointer<GridShipView>(static_cast<u32>(grid_shipAddress())));
    s16 sail = static_cast<s16>(gabi::object_at<GridShipView>(gabi::ea(ship)).sailAngle);
    s32 direction = grid_cM_atan2s(windX, windZ);
    s32 relative = (s32)((u32)(s32)(s16)(static_cast<s16>(actor->current.angle.y) + sail) - (u32)direction);
    s16 opposing = (s16)((u32)relative + 0x8000);
    if (opposing > 0) {
        if (sail > 0 && sail < 0x4000)
            opposing = 0;
    } else if (sail < 0 && sail > -0x4000)
        opposing = 0;
    s16 desired = (s16)gabi::ftoi((f32)opposing * 0.6f);
    if (!static_cast<u8>(actor->mIsForceWindRelAngle))
        grid_cLib_addCalcAngleS2(static_cast<void*>(&actor->relativeWindAngle), desired, 4, 0x1000);
    else {
        ship = static_cast<void*>(grid_pointer<GridShipView>(static_cast<u32>(grid_shipAddress())));
        s16 forced =
            (s16)(static_cast<s16>(actor->mForceWindRelAngle) -
                  static_cast<s16>(gabi::object_at<GridShipView>(gabi::ea(ship)).sailAngle) + 0x8000);
        grid_cLib_addCalcAngleS2(static_cast<void*>(&actor->relativeWindAngle), forced, 2, 0x1400);
    }
    s16 angle = static_cast<s16>(actor->relativeWindAngle);
    actor->mIsForceWindRelAngle = 0;
    grid_mDoMtx_YrotS_2(static_cast<void*>(grid_pointer<Mtx34>(static_cast<u32>(grid_matrixAddress()))),
                        angle);
    gabi::Local<VectorFrame> input, output, light;
    input->vector.x = 0.f;
    input->vector.y = 0.f;
    input->vector.z = 0.064f;
    grid_MtxPosition(&input->vector, &output->vector);
    f32 impulse = output->vector.x, flutter = std::fabs((f32)output->vector.z) + 0.02f;
    input->vector.x = 1.f;
    input->vector.z = 0.f;
    grid_MtxPosition(&input->vector, &output->vector);
    f32 cross = std::fabs((f32)output->vector.z) * (1.f - static_cast<f32>(actor->furl));
    u32 phase = static_cast<u32>(actor->wavePhase);
    s32 advance = gabi::ftoi(gabi::fmadds(0.8f * power, 9000.f, 2500.f));
    if (advance > 10000)
        advance = 10000;
    f32 coefficient = sailSin(phase) * cross;
    f32 debugRate = static_cast<f32>(grid_debugFlutter()) + 0.01f;
    actor->wavePhase = phase + (u32)advance;
    s16 twist = (s16)(static_cast<s16>(actor->twistAngle) + (s16)gabi::ftoi(3000.f * sailCos((u32)relative)));
    f32 flutterScale = gabi::fmadds(coefficient, debugRate, 1.f);
    twist = (s16)(twist + (twist > 0 ? 300 : -300));
    actor->twistAngle = twist;
    f32 sideways = 0.5f * sailSin((u32)relative);
    input->vector.x = 0.f;
    input->vector.z = 1.6f;
    s16 oscillationTimer = static_cast<s16>(actor->oscillationTimer);
    if (oscillationTimer) {
        if (oscillationTimer == 15)
            actor->oscillationSign = impulse;
        u32 speed = (u32)(static_cast<s16>(grid_debugOscillationSpeed()) + 7000);
        f32 kick = gabi::fmadds(sailSin((u32)oscillationTimer * speed), 0.35f, 1.f);
        actor->oscillationGain = kick;
        bool enabled = static_cast<u8>(grid_alternateSail()) != 0;
        f32 sign = static_cast<f32>(actor->oscillationSign);
        if (!enabled) {
            actor->oscillationGain = kick * 0.75f;
            if (static_cast<s16>(actor->oscillationTimer) == 5)
                actor->oscillationTimer = 1;
        }
        if (sign < 0.f)
            actor->oscillationGain = -static_cast<f32>(actor->oscillationGain);
        actor->oscillationTimer = (s16)(static_cast<s16>(actor->oscillationTimer) - 1);
    } else
        grid_cLib_addCalc0(static_cast<void*>(&actor->oscillationGain), 0.2f, 0.05f);
    if (static_cast<f32>(actor->openingStarted) == 0.f) {
        actor->openingTimer = 15;
        actor->openingStarted = 1.f;
    } else if (static_cast<s16>(actor->openingTimer) > 0) {
        grid_cLib_addCalc2(static_cast<void*>(&actor->openingGain), 1.414f, 0.25f,
                           static_cast<f32>(grid_hio().openingStep));
        actor->openingTimer = (s16)(static_cast<s16>(actor->openingTimer) - 1);
    } else
        grid_cLib_addCalc2(static_cast<void*>(&actor->openingGain), 1.f, 0.1f, 0.05f);
    f32 maxX = gabi::fmadds(power, 0.75f, 0.25f);
    maxX = (maxX - 1.f) >= 0.f ? 1.f : maxX;
    f32 maxZ = gabi::fmadds(power, 0.25f, 0.5f);
    maxZ = (maxZ - 1.f) >= 0.f ? 1.f : maxZ;
    for (u32 i = 0; i < 85; ++i) {
        u32 column = i % 7, row = i / 7;
        f32 negativeRow = (f32)(s32)row - 2.f;
        f32 lateral = 3.f - (f32)column;
        f32 span = 1.f - static_cast<f32>(actor->furl);
        s16 offset = (s16)gabi::ftoi(10922.f * grid_x_rate_tbl[row]);
        phase = static_cast<u32>(actor->wavePhase);
        f32 waveX = sailSin(phase + i * (u32)(s32) static_cast<s16>(actor->waveStepSin));
        f32 waveZ = sailCos(phase + i * (u32)(s32) static_cast<s16>(actor->waveStepCos)) * flutter;
        grid_MtxPosition(&input->vector, &output->vector);
        f32 px = (f32)output->vector.x * maxX, pz = (f32)output->vector.z * maxZ;
        output->vector.x = px;
        output->vector.z = pz;
        f32 kick = static_cast<f32>(actor->oscillationGain), gain = static_cast<f32>(actor->openingGain);
        f32 wx = gabi::fmadds(waveX, flutter, ((px * flutterScale) * span) * gain);
        f32 wz = gabi::fmadds(waveZ, 0.5f, ((pz * cross) * span) * gain);
        f32 impulseWave = gabi::fmadds(std::fabs(sailCos((u32)(s32)desired)) * kick, 0.2f, kick * 0.8f);
        wx += impulseWave;
        f32 lift = grid_sqrt(gabi::fmadds(wx, wx, wz * wz)) * 0.05f;
        f32 weight = static_cast<f32>(actor->weights[i]);
        wx *= weight;
        wz *= weight;
        lift *= weight;
        negativeRow = negativeRow >= 0.f ? 0.f : negativeRow;
        f32 taper = (negativeRow * negativeRow) * 0.25f;
        f32 furl = static_cast<f32>(actor->furl);
        f32 rate = grid_z_rate_tbl[row];
        f32 folding = grid_z_rate_tbl2[row];
        f32 height = (float)(-(double)gabi::fmadds(gabi::fmadds(0.3f, taper, 0.67f), furl, -1.f));
        height *= gabi::fmadds(folding, furl, span);
        void* pos = static_cast<void*>(&actor->mPacket.mPos[i]);
        grid_vector(pos).x = grid_l_pos[i * 3];
        f32 baseY = grid_l_pos[row * 7 * 3 + 1] * (1.f - height);
        grid_vector(pos).y = gabi::fmadds(grid_l_pos[i * 3 + 1], height, baseY);
        grid_vector(pos).z = grid_l_pos[i * 3 + 2] * height;
        u32 bend = (u32)(s32) static_cast<s16>(actor->twistAngle) + (u32)(s32)offset * column;
        f32 bulge = (float)(-(double)gabi::fmadds(lateral, lateral, -9.f));
        f32 deflection = ((sailSin(bend) * furl) * bulge) / 9.f - (sideways * bulge) / 9.f;
        f32 xBend = (120.f * (rate * furl)) * deflection;
        f32 yBend = (((5.f * rate) * (f32)column) * furl) *
                    (((sailCos(bend) * static_cast<f32>(actor->furl)) * (f32)column) / 6.f);
        f32 decay = (float)(-(double)gabi::fmadds(0.5f, taper, -1.f));
        xBend *= decay;
        yBend *= decay;
        f32 zBend = -grid_sqrt(gabi::fmadds(xBend, xBend, yBend * yBend)) * 0.25f;
        if (column > 4) {
            f32 magnitude = std::fabs(
                sailSin((u32)(s32) static_cast<s16>(actor->twistAngle) + 2u * (u32)(s32)offset * column));
            zBend = gabi::fmadds(4.25f, (f32)(column - 4) * magnitude, zBend);
        }
        f32 roll = (6.f * grid_z_rate_tbl[row]) * ((f32)column / 6.f);
        f32 attenuation = gabi::fmadds(0.65f, span, 0.35f);
        f32 xDelta = gabi::fmadds(wx * static_cast<f32>(actor->openingGain), attenuation, xBend);
        f32 yDelta = gabi::fmadds(lift, attenuation, yBend);
        grid_vector(pos).x = static_cast<f32>(grid_vector(pos).x) + xDelta;
        grid_vector(pos).y = static_cast<f32>(grid_vector(pos).y) + yDelta;
        f32 zDelta =
            gabi::fmadds(zBend, roll * static_cast<f32>(actor->furl),
                         gabi::fmadds(wz * static_cast<f32>(actor->openingGain), attenuation, -13.75f));
        grid_vector(pos).z = static_cast<f32>(grid_vector(pos).z) + zDelta;
    }
    f32 width = std::fabs(static_cast<f32>(actor->mPacket.mPos[59].x));
    f32 rates[5];
    constexpr f32 defaults[] = {0.0015f, 0.000700000033f, 0.000200000068f, 0.0015f, 0.001200000057f};
    for (u32 j = 0; j < 5; ++j)
        rates[j] =
            (f32)(-(double)gabi::fmadds(width, static_cast<f32>(grid_debugRowRate(j)) + defaults[j], -1.f));
    for (u32 i = 1; i < 85; ++i) {
        s32 group = i >= 56 && i <= 62   ? 0
                    : i >= 49 && i <= 55 ? 1
                    : i >= 42 && i <= 48 ? 2
                    : i >= 63 && i <= 69 ? 3
                    : i >= 70 && i <= 77 ? 4
                                         : -1;
        if (group >= 0) {
            actor->mPacket.mPos[i].z = static_cast<f32>(actor->mPacket.mPos[i].z) * rates[group];
        }
    }
    grid_cXyz____mi_2(static_cast<void*>(&actor->tevStr.mLightPosWorld), &light->vector,
                      static_cast<void*>(&actor->current.pos));
    void* packet = static_cast<void*>(&actor->mPacket);
    grid_setSailNormalMatrix(packet, &light->vector);
    for (s32 row = 0; row < 12; ++row)
        for (s32 column = 0; column < 7; ++column)
            grid_daHo_packet_c__setNrmVtx(packet, static_cast<void*>(&actor->mPacket.mNrm[row * 7 + column]),
                                          column, row);
    grid_daHo_packet_c__setTopNrmVtx(packet, static_cast<void*>(&actor->mPacket.mNrm[84]));
    grid_daHo_packet_c__setBackNrm(packet);
    grid_DCStoreRangeNoSync(static_cast<void*>(&actor->mPacket.mPos), 0x3FC);
    grid_DCStoreRangeNoSync(static_cast<void*>(&actor->mPacket.mNrm), 0x3FC);
    grid_DCStoreRangeNoSync(static_cast<void*>(&actor->mPacket.mBackNrm), 0x3FC);
}

/* 021644BC */
static s32 daGrid_Execute(daGrid_c* a) { return grid_daGrid_c___execute(a); }

/* 021644C0 */
static bool daGrid_IsDelete(daGrid_c* a) { return true; }

/* 021644C8 */
bool daGrid_c::_delete() {

    grid_dComIfG_resDelete(static_cast<void*>(&this->mClothPhase), "Cloth");
    grid_dComIfG_resDelete(static_cast<void*>(&this->mShipPhase), "Ship");
    s8 no = static_cast<s8>(grid_hio().mNo);
    if (no >= 0) {
        grid_mDoHIO_subRoot_c__deleteChild(no);
        grid_hio().mNo = -1;
    }
    return true;
}

/* 02164530 */
static s32 daGrid_Delete(daGrid_c* a) { return grid_daGrid_c___delete(a); }

/* 021655E0 */
static s32 daGrid_Create(daGrid_c* actor) { return grid_daGrid_c___create(actor); }

/* 021650A0 */
s32 daGrid_c::_create() {

    u32 flags = static_cast<u32>(this->actor_condition);
    if (!(flags & 8)) {
        if (gabi::ea(this) != 0) {
            grid_fopAc_ac_c__fopAc_ac_c(this);
            this->__vtbl = grid_tu::actorVtable;
            grid_constructSailPacket(static_cast<void*>(&this->mPacket));
            flags = static_cast<u32>(this->actor_condition);
        }
        this->actor_condition = flags | 8;
    }
    grid_getPlay();
    this->parameter = (u8) static_cast<u32>(this->mParameters);
    s32 phase = grid_dComIfG_resLoad(static_cast<void*>(&this->mClothPhase), "Cloth");
    if (phase != 4)
        return phase;
    phase = grid_dComIfG_resLoad(static_cast<void*>(&this->mShipPhase), "Ship");
    if (phase != 4)
        return phase;
    if (static_cast<s8>(grid_hio().mNo) < 0) {
        s32 no = grid_mDoHIO_subRoot_c__createChild("DÌ¿", static_cast<void*>(&grid_hio()));
        grid_hio().mNo = (u8)no;
    }
    for (s32 i = 0; i < 85; ++i) {
        f32 amplitude = i < 7 ? 40.f : i < 14 ? 70.f : (i >= 42 && i < 56) ? 85.f : 80.f;
        s32 rowIndex = 6;
        f32 z0 = grid_l_pos[2], z6 = grid_l_pos[6 * 3 + 2], zi = grid_l_pos[i * 3 + 2];
        f32 first = std::fabs(z0 - zi), second = std::fabs(z6 - zi), half = std::fabs(z0 - z6) * 0.5f;
        f32 frequency = (1.5707963705062866f / half) * 1.05f;
        if (first > second)
            first = second;
        f32 wave = grid_sin(first * frequency);
        f32 xAmplitude = wave * amplitude;
        for (s32 j = 0; j < 7; ++j) {
            for (s32 row = 0; row < 12; ++row)
                if (i == j + row * 7)
                    rowIndex = j + 56;
        }
        f32 yi = grid_l_pos[i * 3 + 1], yr = grid_l_pos[rowIndex * 3 + 1];
        f32 yAmplitude;
        if (yi < yr) {
            f32 y0 = grid_l_pos[1];
            first = std::fabs(y0 - yi);
            second = std::fabs(yr - yi);
            half = std::fabs(yr - y0) * 0.5f;
            frequency = (1.5707963705062866f / half) * 1.05f;
            yAmplitude = rowIndex == 56 ? 35.f : rowIndex == 57 ? 70.f : 80.f;
        } else {
            f32 top = grid_l_pos[84 * 3 + 1];
            first = std::fabs(yr - yi);
            second = std::fabs(top - yi);
            half = std::fabs(yr - top) * 0.5f;
            frequency = (1.5707963705062866f / half) * 1.15f;
            yAmplitude = 20.f;
        }
        if (first > second)
            first = second;
        wave = grid_sin(first * frequency);
        f32 yWave = wave * yAmplitude;
        f32 weight = grid_sqrt(gabi::fmadds(xAmplitude, xAmplitude, yWave * yWave));
        this->weights[i] = weight;
    }
    grid_alternateSail() = 0;
    struct JudgeFrame {
        u8 linkage[8];
        be<s16> name;
        u8 padding[22];
    };
    gabi::Local<JudgeFrame> judge;
    judge->name = 0xA5;
    void* ship = grid_fopAcIt_Judge(grid_actorNameJudge(), &judge->name);
    grid_shipAddress() = gabi::ea(ship);
    grid_ho_move(this);
    return 4;
}

/* 02162B04 */
bool daGrid_c::_draw() {

    if (static_cast<f32>(this->scale.y) < 0.06f)
        return true;
    void* ship = static_cast<void*>(grid_pointer<GridShipView>(static_cast<u32>(grid_shipAddress())));
    copySailLighting(this, ship);
    f32 x = static_cast<f32>(this->current.pos.x), y = static_cast<f32>(this->current.pos.y),
        z = static_cast<f32>(this->current.pos.z);
    grid_MtxTrans(x, y, z, 0);
    auto matrix = []() {
        return static_cast<void*>(grid_pointer<Mtx34>(static_cast<u32>(grid_matrixAddress())));
    };
    s16 angle = static_cast<s16>(this->current.angle.y);
    grid_mDoMtx_YrotM(matrix(), angle);
    angle = static_cast<s16>(this->current.angle.x);
    grid_mDoMtx_XrotM(matrix(), angle);
    angle = static_cast<s16>(this->current.angle.z);
    grid_mDoMtx_ZrotM(matrix(), angle);
    ship = static_cast<void*>(grid_pointer<GridShipView>(static_cast<u32>(grid_shipAddress())));
    angle = static_cast<s16>(gabi::object_at<GridShipView>(gabi::ea(ship)).sailAngle);
    grid_mDoMtx_YrotM(matrix(), angle);
    y = static_cast<f32>(this->scale.y);
    grid_MtxScale(1.f, y, 1.f, 1);
    x = static_cast<f32>(grid_hio().scale.x);
    y = static_cast<f32>(grid_hio().scale.y);
    z = static_cast<f32>(grid_hio().scale.z);
    grid_MtxScale(x, y, z, 1);
    grid_PSMTXCopy(matrix(), static_cast<void*>(&this->mPacket.mMtx));
    this->mPacket.mpTevStr = &this->tevStr;
    void* buffer;
    if (static_cast<u8>(grid_monotone())) {
        void* play = grid_getPlay();
        buffer = static_cast<void*>(grid_pointer<GridDrawBuffer>(
            static_cast<u32>(gabi::object_at<GridPlayView>(gabi::ea(play)).monotoneBuffer)));
    } else {
        u8 alpha = static_cast<u8>(this->mPacket.mAlpha);
        void* play = grid_getPlay();
        buffer = static_cast<void*>(grid_pointer<GridDrawBuffer>(static_cast<u32>(
            (alpha == 255 ? gabi::object_at<GridPlayView>(gabi::ea(play)).opaqueBuffer
                          : gabi::object_at<GridPlayView>(gabi::ea(play)).translucentBuffer))));
    }
    gabi::object_at<GridDrawBuffer>(gabi::ea(buffer)).zMatrix = static_cast<u32>(grid_matrixAddress());
    grid_J3DDrawBuffer__entryImm(buffer, static_cast<void*>(&this->mPacket), 0);
    grid_updateSailMaterial(static_cast<void*>(&this->mPacket));
    return true;
}

/* 021642D4 */
bool daGrid_c::_execute() {

    grid_getPlay();
    s32 alpha = static_cast<u8>(this->mPacket.mAlpha);
    void* play = grid_getPlay();
    u8 target;
    if (!static_cast<u8>(gabi::object_at<GridPlayView>(gabi::ea(play)).eventRunning)) {
        play = grid_getPlay();
        void* camera = static_cast<void*>(grid_pointer<GridCameraView>(
            static_cast<u32>(gabi::object_at<GridPlayView>(gabi::ea(play)).camera)));
        gabi::Local<VectorFrame> eye, delta;
        grid_cXyz____pl(static_cast<void*>(&gabi::object_at<GridCameraView>(gabi::ea(camera)).eyeBasis),
                        &eye->vector,
                        static_cast<void*>(&gabi::object_at<GridCameraView>(gabi::ea(camera)).eyeOffset));
        grid_cXyz____mi(static_cast<void*>(&this->current.pos), &delta->vector, &eye->vector);
        f32 mag = grid_PSVECSquareMag(&delta->vector);
        f32 distance = grid_sqrt(mag);
        f32 cutoff = static_cast<f32>(grid_hio().alphaDistance);
        if (distance > cutoff)
            target = static_cast<u8>(grid_hio().farAlpha);
        else {
            f32 rate = distance / static_cast<f32>(grid_hio().alphaDistance);
            f32 low = static_cast<u8>(grid_hio().nearAlpha), high = static_cast<u8>(grid_hio().farAlpha);
            target = (u8)gabi::ftoi(gabi::fmadds(high, rate, low * (1.f - rate)));
        }
    } else
        target = static_cast<u8>(grid_hio().farAlpha);
    u8 result = target > alpha + 5 ? (u8)(alpha + 5) : target < alpha - 5 ? (u8)(alpha - 5) : target;
    this->mPacket.mAlpha = result;
    play = grid_getPlay();
    if (static_cast<u8>(gabi::object_at<GridPlayView>(gabi::ea(play)).alternateSail))
        grid_alternateSail() = 1;
    else {
        play = grid_getPlay();
        if (!static_cast<u8>(gabi::object_at<GridPlayView>(gabi::ea(play)).alternateSail))
            grid_alternateSail() = 0;
    }
    if (static_cast<f32>(this->scale.y) < 0.06f)
        this->openingStarted = 0.f;
    else
        grid_ho_move(this);
    return true;
}

// HD: Compiler-generated and GX2 material construction helpers.
/* 02166918 */
static void hioDestroy(void* hio, s32 flags) {
    if (hio) {
        gabi::object_at<daHo_HIO_c>(gabi::ea(hio)).mNo = -1;
        gabi::object_at<daHo_HIO_c>(gabi::ea(hio)).vtable = grid_tu::hioVtable;
        if (flags & 1)
            grid_dl(hio);
    }
}

/* 02165FA8 */
static void packetBaseDestroy(void* p, s32 flags) {
    if (p && (flags & 1))
        grid_dl(p);
}

/* 02165FBC */
static void* materialConstruct(void* p) {
    if (!p)
        p = grid_allocateObject(0x254);
    if (p) {
        grid_constructVertexBuffer(static_cast<void*>(&grid_material(p).vertexBuffer));
        grid_constructShader(static_cast<void*>(&grid_material(p).shaderObject));
        grid_material(p).allocation.v = 0;
        grid_material(p).allocationCount = 0;
    }
    return p;
}

/* 02166018 */
static void* vertexDescriptorConstruct(void* p) {
    if (!p)
        p = grid_allocateObject(0x10);
    return p;
}

/* 02166044 */
static void materialDestroy(void* p, s32 flags) {
    if (p) {
        grid_destroyShader(static_cast<void*>(&grid_material(p).shaderObject), 2);
        grid_destroyVertexBuffer(static_cast<void*>(&grid_material(p).vertexBuffer), 2);
        if (flags & 1)
            grid_dl(p);
    }
}

/* 02166940 */
static void packetVirtualNoop(void* p) {}

/* 021664D8 */
static void packetEmpty(void* packet) {}

/* 021623C4 */
static void copyMatrix(void* dst, const void* src) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(gabi::object_at<Mtx34>(gabi::ea(src)).m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; ++i)
        gabi::object_at<Mtx34>(gabi::ea(dst)).m[i / 4][i % 4] = values[i];
}

/* 02162464 */
static void colorS10ToFloat(void* out, const void* color) {
    s16 r = static_cast<s16>(gabi::object_at<GXColorS10>(gabi::ea(color)).r),
        g = static_cast<s16>(gabi::object_at<GXColorS10>(gabi::ea(color)).g),
        b = static_cast<s16>(gabi::object_at<GXColorS10>(gabi::ea(color)).b),
        a = static_cast<s16>(gabi::object_at<GXColorS10>(gabi::ea(color)).a);
    gabi::object_at<GridFloatColor>(gabi::ea(out)).r = r / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).g = g / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).b = b / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).a = a / 255.f;
}

/* 02162528 */
static void colorU8ToFloat(void* out, const void* color) {
    u8 r = static_cast<u8>(gabi::object_at<GXColor>(gabi::ea(color)).r),
       g = static_cast<u8>(gabi::object_at<GXColor>(gabi::ea(color)).g),
       b = static_cast<u8>(gabi::object_at<GXColor>(gabi::ea(color)).b),
       a = static_cast<u8>(gabi::object_at<GXColor>(gabi::ea(color)).a);
    gabi::object_at<GridFloatColor>(gabi::ea(out)).r = r / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).g = g / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).b = b / 255.f;
    gabi::object_at<GridFloatColor>(gabi::ea(out)).a = a / 255.f;
}

/* 02165DA8 */
static void gridStaticInit() {
    for (u32 off : {8u, 0u, 12u, 4u})
        grid_commonStatic(off) = 0;
    grid_register_global_object(grid_destructorRecordA());
    grid_minimumAngle() = -3.1415927410125732f;
    grid_maximumAngle() = 3.1415927410125732f;
    grid_constructCommonByteA(grid_commonByteA());
    grid_register_global_object(grid_destructorRecordB());
    grid_constructCommonByteB(grid_commonByteB());
    grid_register_global_object(grid_destructorRecordC());
    grid_hio().mNo = -1;
    grid_hio().lightingEnabled = 1;
    grid_hio().flag3 = 0;
    grid_hio().flag2 = 0;
    grid_hio().waveAmplitude = 40.f;
    grid_hio().gain = 0.5f;
    grid_hio().minimumGain = 0.1f;
    grid_hio().furlThreshold = 0.1f;
    grid_hio().openingStep = 0.4f;
    grid_hio().scale.x = 1.f;
    grid_hio().scale.y = 1.f;
    grid_hio().scale.z = 1.f;
    grid_hio().useForcedFurl = 0;
    grid_hio().alphaDistance = 900.f;
    constexpr f32 folds[] = {1.f, 0.425f, 0.45f, 0.4f, 0.2f, 0.4f, 0.45f, 0.4f, 0.2f, 0.5f, 0.75f, 1.f, 1.f};
    constexpr f32 rates[] = {0.05f, 0.125f, 0.175f, 0.15f,  0.0625f, 0.15f, 0.2f,
                             0.15f, 0.075f, 0.175f, 0.175f, 0.1f,    0.f};
    for (u32 i = 0; i < 13; ++i) {
        grid_hio().folds[i] = folds[i];
        grid_hio().rates[i] = rates[i];
    }
    grid_hio().farAlpha = 255;
    grid_hio().nearAlpha = 50;
    grid_hio().freeze = 0;
    grid_hio().useCustomFold = 0;
    grid_hio().vtable = grid_tu::hioVtable;
    grid_register_global_object(grid_destructorRecordHio());
}

/* 021660A4 */
static void packetDestroy(void* packet, s32 flags) {
    if (packet) {
        releaseSailPacket(packet);
        if (flags & 1)
            grid_dl(packet);
    }
}

/* 021664DC */
static void actorDestroy(daGrid_c* actor, s32 flags) {
    if (actor) {
        releaseSailPacket(static_cast<void*>(&actor->mPacket));
        grid_fopAc_ac_c___fopAc_ac_c(actor, 0);
        if (flags & 1)
            grid_dl_2(actor);
    }
}

/* 02164534 */
static void* packetConstruct(void* packet) {
    if (!packet)
        packet = grid_allocateObject(0x2100);
    if (!packet)
        return packet;
    grid_constructMaterialPacket(packet);
    gabi::object_at<GridPacketPrefix>(gabi::ea(packet)).vtable = grid_tu::packetVtable;
    grid_packet(packet).shaderCount = 0;
    void* list = static_cast<void*>(&grid_packet(packet).shaderCollectionCount);
    if (!list)
        list = grid_allocateObject(8);
    if (list) {
        gabi::object_at<GridList>(gabi::ea(list)).array = 0;
        gabi::object_at<GridList>(gabi::ea(list)).count = 0;
    }
    void *pool = static_cast<void*>(&grid_packet(packet).materialPool), *poolStorage = pool;
    if (!poolStorage)
        poolStorage = grid_allocateObject(0x968);
    if (poolStorage) {
        grid_construct_array(poolStorage, 4, 0x254,
                             grid_callbackPointer(reinterpret_cast<const void*>(&materialConstruct)));
        grid_pool(poolStorage).current = 0;
        grid_pool(poolStorage).unused = 0;
        grid_pool(poolStorage).stride = 32;
        grid_pool(poolStorage).initialized = 0;
        grid_pool(poolStorage).attributeCount = 0;
        for (u32 i = 0; i < 4; ++i)
            grid_pool(poolStorage).materials[i].vertices.v = 0;
    }
    grid_constructMaterialList(static_cast<void*>(&grid_packet(packet).drawMaterialCount));
    grid_constructUniformObject(static_cast<void*>(&grid_packet(packet).drawUniform));
    gabi::object_at<GridMaterialSlot>(gabi::ea(packet) + 0x165c).vtable = grid_tu::drawUniformVtable;
    grid_clearStorage(static_cast<void*>(&grid_packet(packet).modelMatrix), 0x34);
    if (!static_cast<void*>(&grid_packet(packet).modelMatrix))
        grid_allocateObject(0x30);
    grid_constructUniformObject(static_cast<void*>(&grid_packet(packet).materialUniformObject));
    gabi::object_at<GridMaterialSlot>(gabi::ea(packet) + 0x1704).vtable = grid_tu::materialUniformVtable;
    grid_clearStorage(static_cast<void*>(&grid_packet(packet).uniformState), 0x2F0);
    // Three groups of identity matrices and default material colours.
    for (u32 i = 0; i < 44; ++i)
        gabi::object_at<be<f32>[44]>(gabi::ea(&grid_packet(packet).uniformState))[i] =
            (i % 4 == 3) ? 1.f : 0.f;
    for (u32 group = 0; group < 3; ++group)
        grid_construct_array(static_cast<void*>(&grid_packet(packet).uniformState.descriptors[group]), 2,
                             sizeof(GridDescriptor),
                             grid_callbackPointer(reinterpret_cast<const void*>(&vertexDescriptorConstruct)));
    for (u32 i = 0; i < 8; ++i)
        if (gabi::ea(&grid_packet(packet).uniformState.matrices[i]) == 0)
            grid_allocateObject(0x30);
    for (u32 i = 0; i < 6; ++i)
        if (gabi::ea(&grid_packet(packet).uniformState.finalDescriptors[i]) == 0)
            grid_allocateObject(0x10);
    grid_packet(packet).uniformFlag = 0;
    grid_constructIndexBuffer(static_cast<void*>(&grid_packet(packet).indexBuffer));
    auto& sail = grid_packet(packet);
    void* textures[] = {&sail.frontTexture, &sail.alternateTexture, &sail.toonTexture};
    void* textureResources[] = {&sail.frontTextureResource, &sail.alternateTextureResource,
                                &sail.toonTextureResource};
    for (u32 i = 0; i < 3; ++i) {
        grid_constructTexture(textures[i]);
        grid_constructTextureResource(textureResources[i]);
    }
    grid_packet(packet).normalAngle = 0;
    grid_packet(packet).unusedAngle1 = 0;
    grid_packet(packet).unusedAngle2 = 0;
    grid_packet(packet).mAlpha = 255;
    gabi::Local<MaterialFrame> name;
    name->words[0] = grid_tu::shaderArchiveName;
    name->words[1] = grid_tu::safeStringVtable;
    void* archives = grid_getShaderArchives();
    void* root = static_cast<void*>(grid_pointer<GridArchiveRoot>(
        static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).root)));
    s32 selection = grid_findShaderArchive(root, &name->words[0]);
    void* selected = nullptr;
    if (selection >= 0) {
        u32 count = static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).count),
            entries = static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).entries);
        void* entry = static_cast<void*>(
            grid_pointer<GridArchiveEntry>(entries + ((u32)selection < count ? (u32)selection * 0x24 : 0)));
        if (!static_cast<u8>(gabi::object_at<GridArchiveEntry>(gabi::ea(entry)).initialized)) {
            root = static_cast<void*>(grid_pointer<GridArchiveRoot>(
                static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).root)));
            u32 programCount =
                static_cast<u32>(gabi::object_at<GridArchiveRoot>(gabi::ea(root)).programCount);
            void* program =
                (u32)selection < programCount
                    ? static_cast<void*>(grid_pointer<GridShaderProgram>(
                          static_cast<u32>(gabi::object_at<GridArchiveRoot>(gabi::ea(root)).programs) +
                          (u32)selection * 0x84))
                    : nullptr;
            grid_initializeShaderArchive(entry, program, 0);
            count = static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).count);
            entries = static_cast<u32>(gabi::object_at<GridArchiveCatalog>(gabi::ea(archives)).entries);
        }
        selected = static_cast<void*>(
            grid_pointer<GridArchiveEntry>(entries + ((u32)selection < count ? (u32)selection * 0x24 : 0)));
    }
    grid_createShaderCollections(static_cast<void*>(&grid_packet(packet).shaderCount), selected, 0);
    grid_pool(pool).attributeCount = 19;
    grid_pool(pool).attributes.v = grid_tu::vertexAttributes;
    for (u32 buffer = 0; buffer < 2; ++buffer)
        for (u32 side = 0; side < 2; ++side) {
            void* material = static_cast<void*>(&grid_pool(pool).materials[buffer + side * 2]);
            if (!static_cast<u32>(grid_material(material).vertices.v)) {
                void* vertices = allocateFromHeap(0x1580, 0x40);
                if (vertices) {
                    grid_material(material).allocation.v = gabi::ea(vertices);
                    grid_material(material).allocationCount = 172;
                }
                grid_material(material).vertices.v = static_cast<u32>(grid_material(material).allocation.v);
            }
            grid_initializeVertexBuffer(static_cast<void*>(&grid_material(material).vertexBuffer),
                                        grid_material(material).vertices.get(), 172,
                                        static_cast<void*>(&grid_pool(pool).attributeCount));
        }
    grid_pool(pool).unused = 0;
    grid_pool(pool).initialized = 1;
    for (u32 i = 0; i < static_cast<u32>(grid_packet(packet).shaderCount); ++i) {
        void* collection = shaderCollection(packet, i);
        u32 program = static_cast<u32>(grid_collection(collection).program.v);
        releaseShaderArrays(collection);
        grid_collection(collection).program.v = program;
        for (u32 side = 0; side < 2; ++side) {
            void* shaders = allocateFromHeap(0x1E8, 4);
            for (u32 j = 0; j < 2; ++j) {
                void* shader =
                    static_cast<void*>(grid_pointer<GridShader>(gabi::ea(shaders) + j * sizeof(GridShader)));
                if (shader)
                    grid_constructShader(shader);
            }
            if (shaders) {
                grid_shaderSide(collection, side).array = gabi::ea(shaders);
                grid_shaderSide(collection, side).count = 2;
            }
        }
        for (u32 buffer = 0; buffer < 2; ++buffer)
            for (u32 side = 0; side < 2; ++side) {
                u32 count = static_cast<u32>(grid_shaderSide(collection, buffer).count),
                    array = static_cast<u32>(grid_shaderSide(collection, buffer).array);
                void* shader =
                    static_cast<void*>(grid_pointer<GridShader>(array + (side < count ? side * 0xF4 : 0)));
                grid_initializeShader(
                    static_cast<void*>(grid_pointer<GridShaderProgram>(program)), shader,
                    static_cast<void*>(&grid_pool(pool).materials[buffer + side * 2].vertexBuffer),
                    static_cast<void*>(&grid_pool(pool).attributeCount), 0);
            }
    }
    grid_createDrawMaterials(static_cast<void*>(&grid_packet(packet).drawMaterialCount), 1, 0);
    grid_initializeIndexBuffer(static_cast<void*>(&grid_packet(packet).indexBuffer),
                               static_cast<void*>(grid_indices), 4, static_cast<u32>(grid_indexCount()));
    gabi::object_at<GridIndexBuffer>(gabi::ea(&grid_packet(packet).indexBuffer)).primitiveMode = 4;
    for (u32 side = 0; side < 2; ++side) {
        u32 current = static_cast<u32>(grid_pool(pool).current);
        void* vertices = grid_pool(pool).materials[(current + side * 2)].vertices.get();
        for (u32 i = 0; i < static_cast<u32>(grid_vertexCount()); ++i) {
            u32 uv = grid_vertexIndices[i * 3 + 2];
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).u =
                grid_l_texCoord[uv * 2];
            gabi::object_at<GridVertex>(gabi::ea(vertices) + i * sizeof(GridVertex)).v =
                grid_l_texCoord[uv * 2 + 1];
        }
    }
    u32 current = static_cast<u32>(grid_pool(pool).current), other = current ? 0 : 1;
    for (u32 side = 0; side < 2; ++side)
        for (u32 i = 0; i < 172; ++i)
            for (u32 j = 0; j < 8; ++j) {
                void* src = grid_pool(pool)
                                .materials[(static_cast<u32>(grid_pool(pool).current) + side * 2)]
                                .vertices.get();
                void* dst = grid_pool(pool).materials[(other + side * 2)].vertices.get();
                gabi::object_at<GridVertexWords>(gabi::ea(dst) + i * sizeof(GridVertexWords))
                    .words[j] = static_cast<u32>(
                    gabi::object_at<GridVertexWords>(gabi::ea(src) + i * sizeof(GridVertexWords)).words[j]);
            }
    current = static_cast<u32>(grid_pool(pool).current);
    for (u32 side = 0; side < 2; ++side) {
        void* material = static_cast<void*>(&grid_pool(pool).materials[current + side * 2].vertexBuffer);
        grid_flushVertexBuffer(
            material, 0,
            static_cast<u32>(gabi::object_at<GridVertexBufferView>(gabi::ea(material)).byteLength));
    }
    grid_pool(pool).current = static_cast<u32>(grid_pool(pool).current) ? 0 : 1;
    grid_lockTextureManager(
        static_cast<void*>(grid_pointer<GridTextureManager>(static_cast<u32>(grid_textureManager()))));
    name->words[0] = grid_tu::shipArchiveName;
    name->words[1] = grid_tu::safeStringVtable;
    void* resource = grid_getObjectResource(
        static_cast<void*>(grid_pointer<GridResourceManager>(static_cast<u32>(grid_resourceManager()))),
        &name->words[0], 23);
    grid_loadTextureResource(static_cast<void*>(&grid_packet(packet).frontTextureResource),
                             static_cast<void*>(grid_pointer<GridTextureData>(static_cast<u32>(
                                 gabi::object_at<GridShaderResource>(gabi::ea(resource)).textureData))));
    if (!textureEqual(packet, 0x1A84, 0x1C1C))
        grid_assignTexture(static_cast<void*>(&grid_packet(packet).frontTexture),
                           static_cast<void*>(&grid_packet(packet).frontTextureResource));
    else
        copyTextureHandle(packet, 0x1A84, 0x1C1C);
    name->words[0] = grid_tu::shipArchiveName;
    name->words[1] = grid_tu::safeStringVtable;
    name->words[2] = grid_tu::alternateSailName;
    name->words[3] = grid_tu::safeStringVtable;
    resource = grid_getArchiveResource(
        static_cast<void*>(grid_pointer<GridArchiveManager>(static_cast<u32>(grid_archiveManager()))),
        &name->words[0], &name->words[2], 0);
    if (resource)
        grid_loadAlternateTexture(static_cast<void*>(&grid_packet(packet).alternateTextureResource), resource,
                                  "new_ho2");
    if (!textureEqual(packet, 0x1CAC, 0x1E44))
        grid_assignTexture(static_cast<void*>(&grid_packet(packet).alternateTexture),
                           static_cast<void*>(&grid_packet(packet).alternateTextureResource));
    else
        copyTextureHandle(packet, 0x1CAC, 0x1E44);
    grid_packet(packet).textureInitialized = 0;
    grid_unlockTextureManager(
        static_cast<void*>(grid_pointer<GridTextureManager>(static_cast<u32>(grid_textureManager()))));
    return packet;
}
// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 026066C4 void* grid_getObjectResource(void*, be<u32>*, int)
str 100107BC in 021644C8
str 100107C4 in 021644C8
str 10010814 in 021650A0
str 1001081C in 021650A0
str 10010824 in 021650A0
str 100107D4 in 02164534
ext 028E8DAC void grid_PSVECSubtract(void *, void *, void *)
ext 025204C8 void grid_dComIfG_resDelete(void *, const char *)
ext 025F0A18 void grid_mDoHIO_subRoot_c__deleteChild(signed char)
ext 0273AF40 void grid_dl(void *)
ext 02162B04 int grid_daGrid_c___draw(daGrid_c *)
ext 025F1884 int grid_mDoMtx_YrotS(void *, short)
ext 021642D4 int grid_daGrid_c___execute(daGrid_c *)
ext 0273AD10 void * grid_allocateObject(int)
ext 027B5BD8 void grid_constructVertexBuffer(void *)
ext 027BF734 void grid_constructShader(void *)
ext 027BF880 void grid_destroyShader(void *, int)
ext 027B5CBC void grid_destroyVertexBuffer(void *, int)
ext 025200D4 void * grid_getPlay()
ext 0201AD78 void grid_cXyz____pl(void *, cXyz *, void *)
ext 0201ADE0 void grid_cXyz____mi(void *, cXyz *, cXyz *)
ext 028E8DD0 float grid_PSVECSquareMag(cXyz *)
ext 028F4384 float grid_sqrt(float)
ext 02163678 void grid_ho_move(daGrid_c *)
ext 021650A0 int grid_daGrid_c___create(daGrid_c *)
ext 0201ADE0 void grid_cXyz____mi_2(void *, cXyz *, void *)
ext 0201B080 void grid_cXyz__outprod(cXyz *, cXyz *, cXyz *)
ext 0201B1E4 void grid_cXyz__normZC(cXyz *, cXyz *)
ext 0200FCD8 void grid_MtxPosition(cXyz *, cXyz *)
ext 028E8D88 void grid_PSVECAdd(cXyz *, cXyz *, cXyz *)
ext 0201B47C int grid_cXyz__normalizeRS(cXyz *)
ext 0200FCF0 void grid_MtxPush()
ext 025F1C28 void grid_mDoMtx_YrotM(void *, short)
ext 0200FD38 void grid_MtxPull()
ext 0200FAD8 void grid_MtxTrans(float, float, float, int)
ext 025F1BF4 void grid_mDoMtx_XrotM(void *, short)
ext 025F1C5C void grid_mDoMtx_ZrotM(void *, short)
ext 0200FC74 void grid_MtxScale(float, float, float, int)
ext 028E90D4 void grid_PSMTXCopy(void *, void *)
ext 027F0E04 void grid_J3DDrawBuffer__entryImm(void *, void *, int)
ext 021625DC void grid_updateSailMaterial(void *)
ext 025D4ED0 void grid_fopAc_ac_c__fopAc_ac_c(daGrid_c *)
ext 02164534 void grid_constructSailPacket(void *)
ext 02520460 int grid_dComIfG_resLoad(void *, const char *)
ext 025F0A10 int grid_mDoHIO_subRoot_c__createChild(const char *, void *)
ext 028F43F8 float grid_sin(float)
ext 025D5218 void * grid_fopAcIt_Judge(void *, gabi::be<short> *)
ext 028F026C void grid_register_global_object(void *)
ext 028ED6F8 void grid_constructCommonByteA(void *)
ext 028EAB2C void grid_constructCommonByteB(void *)
ext 02755FEC void * grid_findOwningHeap(void *, void *)
ext 027BF7E8 void grid_releaseShader(void *)
ext 027BEBEC void grid_destroyUniformBinding(void *)
ext 027BE2B0 void grid_destroyTexture(void *, int)
ext 027B54A0 void grid_destroyIndexBuffer(void *, int)
ext 027FB528 void grid_releaseUniformObject(void *, int)
ext 027FD764 void grid_destroyMaterialList(void *, int)
ext 028F0164 void grid_destroy_arr(void *, int, int, void *, int, int)
ext 027F13DC void grid_destroyMaterialPacket(void *, int)
ext 025D50BC void grid_fopAc_ac_c___fopAc_ac_c(daGrid_c *, int)
ext 0273AF40 void grid_dl_2(daGrid_c *)
ext 0274FBF8 void grid_lockTextureManager(void *)
ext 02773798 void grid_loadTextureResource(void *, void *)
ext 027BDEB4 void grid_assignTexture(void *, void *)
ext 0274FCCC void grid_unlockTextureManager(void *)
ext 027B5E94 void grid_flushVertexBuffer(void *, int, unsigned int)
ext 0255F8F4 void grid_updateLighting(void *)
ext 021623C4 void grid_copyViewMatrix(gabi::be<unsigned int> *, void *)
ext 027FDA54 void grid_setMaterialView(void *, int, gabi::be<unsigned int> *, void *, void *)
ext 02162464 void grid_signedColorToFloat(gabi::be<unsigned int> *, void *)
ext 0274D458 void grid_scaleMaterialColor(void *, gabi::be<unsigned int> *, float)
ext 028E90D4 void grid_PSMTXCopy_2(gabi::be<unsigned int> *, void *)
ext 0274D2AC void grid_scaleUniformColor(void *, float)
ext 02162528 void grid_byteColorToFloat(void *, void *)
ext 027FE0DC void grid_updateMaterialUniforms(void *, int)
ext 0257DAA8 void * grid_dKyw_get_wind_vec()
ext 02578348 float grid_dKyw_get_wind_pow()
ext 020195B0 int grid_cM_atan2s(float, float)
ext 0200F428 void grid_cLib_addCalcAngleS2(void *, short, int, int)
ext 025F1884 void grid_mDoMtx_YrotS_2(void *, short)
ext 0200EDC8 void grid_cLib_addCalc0(void *, float, float)
ext 0200ED84 void grid_cLib_addCalc2(void *, float, float, float)
ext 02162F6C void grid_setSailNormalMatrix(void *, cXyz *)
ext 02162F7C void grid_daHo_packet_c__setNrmVtx(void *, void *, int, int)
ext 021634F0 void grid_daHo_packet_c__setTopNrmVtx(void *, void *)
ext 021635F0 void grid_daHo_packet_c__setBackNrm(void *)
ext C00088B8 void grid_DCStoreRangeNoSync(void *, int)
ext 02756140 void * grid_getCurrentHeap(void *)
ext 027F1534 void grid_constructMaterialPacket(void *)
ext 028EFFD0 void grid_construct_array(void *, int, int, void *)
ext 027FD6F4 void grid_constructMaterialList(void *)
ext 027FB40C void grid_constructUniformObject(void *)
ext 028F521C void grid_clearStorage(void *, int)
ext 027B5430 void grid_constructIndexBuffer(void *)
ext 027BDF7C void grid_constructTexture(void *)
ext 027BE6B8 void grid_constructTextureResource(void *)
ext 027FFCBC void * grid_getShaderArchives()
ext 027B90AC int grid_findShaderArchive(void *, gabi::be<unsigned int> *)
ext 02800B0C void grid_initializeShaderArchive(void *, void *, int)
ext 0280068C void grid_createShaderCollections(void *, void *, int)
ext 027FF478 void grid_initializeVertexBuffer(void *, void *, int, void *)
ext 027FF530 void grid_initializeShader(void *, void *, void *, void *, int)
ext 027FE084 void grid_createDrawMaterials(void *, int, int)
ext 027B54E0 void grid_initializeIndexBuffer(void *, void *, int, unsigned int)
ext 026124B0 void * grid_getArchiveResource(void *, gabi::be<unsigned int> *, gabi::be<unsigned int> *, int)
ext 02773870 void grid_loadAlternateTexture(void *, void *, const char *)
ext C0006900 void grid_GX2SetPixelUniformBlock(short, unsigned int, unsigned int)
ext C0006A38 void grid_GX2SetVertexUniformBlock(short, unsigned int, unsigned int)
ext C00068A8 void grid_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int)
ext 027BE53C void grid_bindTexture(void *, void *, int, int)
ext 027F29D4 void * grid_getActiveShader(void *)
ext 027BB9E0 void grid_updateShaderProgram(void *, int)
ext 027B9F68 void grid_bindShaderGroup(void *)
ext C00060E0 void grid_GX2CallDisplayList(void *, unsigned int)
ext 027BB7CC void grid_bindShaderProgram(void *)
ext 02750250 void grid_initializeRenderState(void *)
ext 027FFE54 void grid_applyExtraMaterial(void *, void *)
ext 0280037C void grid_configureRenderState(int, void *)
ext 02750370 void grid_applyRenderState(void *)
ext 027BFE5C void grid_bindShader(void *)
ext C0006178 void grid_GX2DrawIndexedEx(unsigned int, unsigned int, unsigned int, unsigned int, int, int)
ext 021644C8 int grid_daGrid_c___delete(daGrid_c *)
data grid_l_pos 101B5E58
data grid_l_texCoord 101B6254
data grid_vertexIndices 101B64FC
data grid_indices 101B6700
data grid_z_rate_tbl 101B6A3C
data grid_z_rate_tbl2 101B6A70
data grid_x_rate_tbl 101B6AA4
*/
// clang-format on
