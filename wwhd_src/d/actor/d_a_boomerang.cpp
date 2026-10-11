/**
 * d_a_boomerang.cpp (WWHD)
 * Item - Boomerang (flight, lock-on sight, blur trail)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_boomerang.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_boomerang.h"
using namespace gabi;

// HD: immediate member-function identities for the verified partial GHS PTMF stores.
static const struct {
    u32 procWait, procMove;
} boomerangTU = {0x020CF4CC, 0x020CEE10};
static const u32 boomerangTUJudgeByID = 0x025E1234;

struct boomerangIdentity_c {
    be<u32> mWord;
};
struct boomerangSinCosTable_c {
    be<f32> mValues[8192][2];
};
struct boomerangSoundTable_c {
    be<u32> mSounds[5];
};
struct boomerangColor_c {
    be<u8> r, g, b, a;
};
struct boomerangColorWords_c {
    be<u32> mWords[4];
};
struct boomerangDestructorRecord_c {
    u8 mBytes[12];
};

static f32 l_viewportHeight;
static f32 l_halfAnimLength;
static f32 l_one;
static f32 l_sightAlphaRange;
static f32 l_sightAlphaBase;
static f32 l_sightSizeRange;
static f32 l_sightSizeBase;
static f32 l_sightSizeScale;
static f32 l_sightViewportDivisor;
static f32 l_quarterTurn;
static f32 l_halfTurn;
static f32 l_noGround;
static f32 l_minusOne;
static f32 l_zero;
static f32 l_keepOffsetX;
static f32 l_keepOffsetY;
static f32 l_keepOffsetZ;
static f32 l_colorDivisor;
static f32 l_blurInterpolationStep;
static f32 l_longFlyMax;
static f32 l_normalFlyMax;
static f32 l_normalizeThreshold;
static f32 l_turnFactor;
static f32 l_turnThreshold;
static f32 l_turnAcceleration;
static f32 l_turnScale;
static f32 l_turnBase;
static f32 l_throwSpeed;
static f32 l_sightLineRadius;
static f32 l_minAngle;
static f32 l_maxAngle;
static f32 l_blurRootY;
static f32 l_uniformOne;
static f32 l_uniformZero;
static Mtx34 l_matrixStack;
static Mtx34 l_viewMatrix;
static cXyz l_blurTop;
static cXyz l_blurRoot;
static boomerangSinCosTable_c l_sinCosTable;
static boomerangSoundTable_c l_lockSounds;
static Mtx34 l_projectionMatrix;
static u32 l_renderSystem;
static u32 l_opaqueBuffer;
static u32 l_translucentBuffer;
static u32 l_graphicsContext;
static u32 l_heapManager;
static u32 l_resourceControl;
static boomerangIdentity_c l_cameraProjectionOwner;
static boomerangIdentity_c l_shaderCacheOwner;
static u8 l_stayRoom;
static u32 l_offsetInitialized;
static f32 l_minRegisteredAngle;
static f32 l_maxRegisteredAngle;
static u8 l_headerByte;
static u8 l_headerFlag;
static f32 l_capsuleRadius;
static boomerangColor_c l_blurColor;
static boomerangColorWords_c l_sightUniformColor;
static boomerangIdentity_c l_attackCapsuleSource;
static boomerangDestructorRecord_c l_angleRegistration;
static boomerangDestructorRecord_c l_headerByteRegistration;
static boomerangDestructorRecord_c l_headerFlagRegistration;
static u32 l_headerWord0;
static u32 l_headerWord1;
static u32 l_headerWord2;
static u32 l_headerWord3;
static boomerangIdentity_c l_safeStringVtable;
static boomerangIdentity_c l_actorVtable;
static boomerangIdentity_c l_sightVtable;
static boomerangIdentity_c l_blurVtable;
static boomerangIdentity_c l_collisionStatusVtable;
static boomerangIdentity_c l_globalCollisionStatusVtable;
static boomerangIdentity_c l_capsuleShapeVtable;
static boomerangIdentity_c l_capsuleObjectVtable;
static boomerangIdentity_c l_capsuleGeometryVtable;
static boomerangIdentity_c l_collisionHitVtable;
static boomerangIdentity_c l_capsuleAttributeVtable;
static boomerangIdentity_c l_linePolyVtable;
static boomerangIdentity_c l_lineGroupVtable;
static boomerangIdentity_c l_linePassVtable;
static boomerangIdentity_c l_lineCheckVtable;
static boomerangIdentity_c l_groundCheckVtable;
static boomerangIdentity_c l_groundGroupVtable;
static boomerangIdentity_c l_groundPassVtable;
static boomerangIdentity_c l_groundPolyVtable;
static boomerangIdentity_c l_deletingGroundGroupVtable;
static boomerangIdentity_c l_deletingGroupVtable;
static boomerangIdentity_c l_deletingGroundPassVtable;
static boomerangIdentity_c l_deletingLineCheckVtable;
static boomerangIdentity_c l_deletingLinePassVtable;
static boomerangIdentity_c l_matrixUniformVtable;
static boomerangIdentity_c l_lightingUniformVtable;
static boomerangIdentity_c l_sightVertexDeclaration;
static boomerangIdentity_c l_blurVertexDeclaration;

// GHS word copies preserve signaling NaN payloads.
template <class T, class U> static be<T> &boomerangBits(be<U> &value) {
    return *at<be<T>>(ea(&value));
}

// HD: native shader, buffer and SafeString support.

static bool boomerangStageEquals(u32 literal) {
    Local<SafeString> lhs, rhs;
    lhs->mStringTop = literal;
    lhs->__vtbl = ea(&l_safeStringVtable);
    u32 play = ea(dComIfGp_get());
    rhs->mStringTop = play + 0x5134;
    rhs->__vtbl = ea(&l_safeStringVtable);
    u32 vt = lhs->__vtbl;
    call_ptr<void>(static_cast<u32>(at<boomerangSafeStringVtable_c>(vt)->mAssureTermination.v),
                   lhs.get());
    vt = lhs->__vtbl;
    call_ptr<void>(static_cast<u32>(at<boomerangSafeStringVtable_c>(vt)->mAssureTermination.v),
                   lhs.get());
    vt = rhs->__vtbl;
    u32 first = lhs->mStringTop;
    call_ptr<void>(static_cast<u32>(at<boomerangSafeStringVtable_c>(vt)->mAssureTermination.v),
                   rhs.get());
    u32 second = rhs->mStringTop;
    if (first == second)
        return true;
    first = lhs->mStringTop;
    second = rhs->mStringTop;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = static_cast<u8>(at<be<u8>>(first)[i]), b = static_cast<u8>(at<be<u8>>(second)[i]);
        if (a != b)
            return false;
        if (!a)
            return true;
    }
    return false;
}

static void boomerangConstructRenderObject(u32 a) {
    boomerang_constructRenderObject(at<void>(a));
    boomerang_constructUniformTransform(at<void>(a + 0xC));
    at<boomerangRenderObject_c>(a)->mTransform.mpVtable.v = ea(&l_matrixUniformVtable);
    boomerang_clearMemory(at<void>(a + 0x80), 0x34);
    if (a + 0x80 == 0)
        boomerang_allocate_noResult(0x30);
    boomerang_constructUniformTransform(at<void>(a + 0xB4));
    at<boomerangRenderObject_c>(a)->mLighting.mpVtable.v = ea(&l_lightingUniformVtable);
    boomerang_clearMemory(at<void>(a + 0x128), 0x2F0);
    f32 zero = static_cast<f32>(l_uniformZero);
    at<boomerangRenderObject_c>(a)->mLighting.mColors[0].mValues[0] = zero;
    at<boomerangRenderObject_c>(a)->mLighting.mColors[0].mValues[1] = zero;
    at<boomerangRenderObject_c>(a)->mLighting.mColors[0].mValues[2] = zero;
    f32 one = static_cast<f32>(l_uniformOne);
    const u16 offsets[] = {0x138, 0x154, 0x160, 0x144, 0x140, 0x190, 0x1B0, 0x180, 0x1A4,
                           0x158, 0x17C, 0x1AC, 0x19C, 0x16C, 0x148, 0x15C, 0x164, 0x14C,
                           0x170, 0x13C, 0x198, 0x1A0, 0x1B4, 0x184, 0x194, 0x18C, 0x178,
                           0x1A8, 0x150, 0x188, 0x134, 0x168, 0x174, 0x1B8, 0x1BC, 0x1C0,
                           0x1C4, 0x1C8, 0x1CC, 0x1D0, 0x1D4};
    for (u16 offset : offsets) {
        bool identity = offset == 0x154 || offset == 0x144 || offset == 0x1A4 || offset == 0x164 ||
                        offset == 0x1B4 || offset == 0x184 || offset == 0x194 || offset == 0x134 ||
                        offset == 0x174 || offset == 0x1C4 || offset == 0x1D4;
        at<boomerangRenderObject_c>(a)
            ->mLighting.mColors[(offset - 0x128) / 16]
            .mValues[((offset - 0x128) % 16) / 4] = identity ? one : zero;
    }
    for (u32 offset : {0x1D8u, 0x1F8u, 0x218u})
        boomerang_constructArray(at<void>(a + offset), 2, 0x10,
                                 guest_fn((const void *)boomerangSmallCtor));
    for (u32 offset = 0x238; offset <= 0x388; offset += 0x30)
        if (a + offset == 0)
            boomerang_allocate_noResult(0x30);
    for (u32 offset = 0x3B8; offset <= 0x408; offset += 0x10)
        if (a + offset == 0)
            boomerang_allocate_noResult(0x10);
    at<boomerangRenderObject_c>(a)->mLighting.mEnabled = 0;
}

static void boomerangConstructTrailArray(u32 storage, u32 count, u32 ctor) {
    u32 bytes = count * 0x254;
    if (!storage)
        storage = boomerang_allocate_u32(bytes + 0x18);
    if (!storage)
        return;
    boomerang_constructArray_voidPtr_u32_s32_u32(at<void>(storage), count, 0x254, ctor);
    at<boomerangTrailArrayTail_c>(storage + bytes)->mWriteBuffer = 0;
    at<boomerangTrailArrayTail_c>(storage + bytes)->mInitialized = 0;
    at<boomerangTrailArrayTail_c>(storage + bytes)->mDeclarationStride = 0x14;
    at<boomerangTrailArrayTail_c>(storage + bytes)->mDeclarationType = 0;
    at<boomerangTrailArrayTail_c>(storage + bytes)->mPrevious.v = 0;
    for (u32 i = 0; i < count; ++i)
        at<boomerangRenderPoint_c>(storage)[i].mVertices.v = 0;
}

static u32 boomerangFindRenderResource(u32 name) {
    Local<SafeString> key;
    key->__vtbl = ea(&l_safeStringVtable);
    key->mStringTop = name;
    u32 manager = boomerang_getMaterialManager();
    u32 archive = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mpArchive.v);
    s32 index = boomerang_findResourceIndex(at<void>(archive), key.get());
    if (index < 0)
        return 0;
    u32 count = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mCount),
        records = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mpMaterials.v);
    u32 record = records + (static_cast<u32>(index) < count ? static_cast<u32>(index) * 0x24 : 0);
    if (!static_cast<u8>(at<boomerangShaderMaterial_c>(record)->mReady)) {
        archive = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mpArchive.v);
        u32 available = static_cast<u32>(at<boomerangShaderArchive_c>(archive)->mCount);
        u32 data = static_cast<u32>(index) < available
                       ? static_cast<u32>(at<boomerangShaderArchive_c>(archive)->mpEntries.v) +
                             static_cast<u32>(index) * 0x84
                       : 0;
        boomerang_initializeMaterialInstance(at<void>(record), at<void>(data), 0);
        count = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mCount);
        records = static_cast<u32>(at<boomerangMaterialManager_c>(manager)->mpMaterials.v);
    }
    return records + (static_cast<u32>(index) < count ? static_cast<u32>(index) * 0x24 : 0);
}

static void boomerangZeroCacheRange(u32 pointer, u32 bytes) {
    u32 end = pointer + bytes;
    while (pointer < end) {
        u32 block = pointer & ~31u;
        for (u32 i = 0; i < 32; ++i)
            at<be<u8>>(block)[i] = 0;
        pointer += 32;
    }
}

static void boomerangBindRenderResource(be<u32> *resourceSlot) {
    u32 state = boomerang_getShaderCache(at<void>(ea(&l_shaderCacheOwner)));
    u32 resource = static_cast<u32>(*resourceSlot);
    u32 current = static_cast<u32>(at<boomerangShaderCache_c>(state)->mpProgram.v),
        material = static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mpProgram.v);
    if (material == current)
        return;
    u8 flags = static_cast<u8>(at<boomerangShaderProgram_c>(material)->mFlags);
    u32 oldShader = static_cast<u32>(at<boomerangShaderCache_c>(state)->mpShader.v);
    if (flags & 2) {
        at<boomerangShaderProgram_c>(material)->mFlags = flags & ~2u;
        boomerang_updateMaterial(at<void>(material), 0);
    }
    u32 shaderOwner = static_cast<u32>(at<boomerangShaderProgram_c>(material)->mpOwner.v),
        shader = static_cast<u32>(at<boomerangShaderOwner_c>(shaderOwner)->mpShader.v);
    if (oldShader != shader)
        boomerang_bindShader(at<void>(shader));
    u32 bytes = static_cast<u32>(at<boomerangShaderProgram_c>(material)->mDisplayListSize);
    if (bytes) {
        u32 data = static_cast<u32>(at<boomerangShaderProgram_c>(material)->mpDisplayList.v);
        boomerang_callDisplayList(at<void>(data), bytes);
        at<boomerangShaderCache_c>(state)->mpShader.v = shader;
        at<boomerangShaderCache_c>(state)->mpProgram.v = material;
    } else {
        boomerang_applyMaterial(at<void>(material));
        at<boomerangShaderCache_c>(state)->mpProgram.v = material;
        at<boomerangShaderCache_c>(state)->mpShader.v = shader;
    }
}

static void boomerangReleaseBlurPoint(u32 point) {
    boomerang_releaseVertexDeclaration(at<void>(point + 0x158));
    u32 buffer = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mAllocation.v);
    at<boomerangRenderPoint_c>(point)->mVertices.v = 0;
    if (buffer) {
        u32 heap = boomerang_findAllocationHeap(at<void>(static_cast<u32>(l_heapManager)),
                                                at<void>(buffer));
        u32 table = static_cast<u32>(at<boomerangHeap_c>(heap)->mpVtable.v);
        call_ptr<void>(
            static_cast<u32>(at<boomerangHeapVtable_c>(table)->mFree.v), at<void>(heap),
            at<void>(static_cast<u32>(at<boomerangRenderPoint_c>(point)->mAllocation.v)));
        at<boomerangRenderPoint_c>(point)->mAllocationBytes = 0;
        at<boomerangRenderPoint_c>(point)->mAllocation.v = 0;
    }
}

static void boomerangReleaseBlurRing(u32 ring) {
    for (u32 i = 0; i < 60; ++i) {
        boomerangReleaseBlurPoint(ring + i * 0x4A8);
        boomerangReleaseBlurPoint(ring + i * 0x4A8 + 0x254);
    }
}

static void boomerangDestroyBlurContents(u32 a) {
    u32 first = a + 0xBEC, second = a + 0x12364;
    at<daBoomerang_blur_c>(a)->mVtable = ea(&l_blurVtable);
    boomerangReleaseBlurRing(first);
    at<boomerangBlurRing_c>(first)->mPrevious.v = 0;
    boomerangReleaseBlurRing(second);
    at<boomerangBlurRing_c>(second)->mPrevious.v = 0;
    u32 render = a + 0x23ADC;
    for (s32 i = 0; i < static_cast<s32>(at<boomerangRenderObject_c>(render)->mCount); ++i) {
        u32 element = static_cast<u32>(at<boomerangRenderObject_c>(render)->mpEntries.v);
        if (static_cast<u32>(i) < static_cast<u32>(at<boomerangRenderObject_c>(render)->mCount))
            element += static_cast<u32>(i) * 0x23C;
        boomerang_releaseUniform(at<void>(ea(&at<boomerangRenderEntry_c>(element)->mUniforms[0])));
        boomerang_releaseUniform(at<void>(ea(&at<boomerangRenderEntry_c>(element)->mUniforms[1])));
    }
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform.mUniforms[0])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform.mUniforms[1])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting.mUniforms[0])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting.mUniforms[1])));
    boomerang_destroySampler(at<void>(a + 0x23F18), 2);
    boomerang_destroyIndexBuffer(at<void>(a + 0x23EF8), 2);
    boomerang_destroyUniformTransform(at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting)),
                                      0);
    boomerang_destroyUniformTransform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform)), 0);
    boomerang_destroyRenderObject(at<void>(render), 2);
    if (second) {
        boomerangReleaseBlurRing(second);
        at<boomerangBlurRing_c>(second)->mPrevious.v = 0;
        boomerang_destroyArray(at<void>(second), 120, 0x254,
                               guest_fn((const void *)boomerangBlurPointDtor), 0, 0);
    }
    if (first) {
        boomerangReleaseBlurRing(first);
        at<boomerangBlurRing_c>(first)->mPrevious.v = 0;
        boomerang_destroyArray(at<void>(first), 120, 0x254,
                               guest_fn((const void *)boomerangBlurPointDtor), 0, 0);
    }
    boomerang_destroyPacket(at<void>(a), 0);
}

static void boomerangDestroyRenderBuffers(u32 render) {
    s32 count = static_cast<s32>(at<boomerangRenderObject_c>(render)->mCount);
    for (s32 i = 0; i < count; ++i) {
        u32 element = static_cast<u32>(at<boomerangRenderObject_c>(render)->mpEntries.v);
        if (static_cast<u32>(i) < static_cast<u32>(count))
            element += static_cast<u32>(i) * 0x23C;
        boomerang_releaseUniform(at<void>(ea(&at<boomerangRenderEntry_c>(element)->mUniforms[0])));
        boomerang_releaseUniform(at<void>(ea(&at<boomerangRenderEntry_c>(element)->mUniforms[1])));
        count = static_cast<s32>(at<boomerangRenderObject_c>(render)->mCount);
    }
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform.mUniforms[0])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform.mUniforms[1])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting.mUniforms[0])));
    boomerang_releaseUniform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting.mUniforms[1])));
}

static void boomerangDrawBlurRing(u32 a, u32 ring, u32 top, u32 root, s32 count, f32 step, f32 zero,
                                  f32 one) {
    f32 previous = zero, current = step;
    for (s32 i = count; i >= 0; --i) {
        u32 selector = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer);
        u32 buffer = static_cast<u32>(
            at<boomerangRenderPoint_c>(ring)[selector + 2u * static_cast<u32>(i)].mVertices.v);
        boomerangZeroCacheRange(buffer, 64);
        selector = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer);
        buffer = static_cast<u32>(
            at<boomerangRenderPoint_c>(ring)[selector + 2u * static_cast<u32>(i)].mVertices.v);
        u32 t = top + static_cast<u32>(i) * 12, r = root + static_cast<u32>(i) * 12;
        f32 x = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[0].x),
            y = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[0].y),
            z = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[0].z);
        at<boomerangQuad_c>(buffer)->mVertices[0].pos.x = x;
        at<boomerangQuad_c>(buffer)->mVertices[0].pos.y = y;
        at<boomerangQuad_c>(buffer)->mVertices[0].u = current;
        at<boomerangQuad_c>(buffer)->mVertices[0].v = zero;
        at<boomerangQuad_c>(buffer)->mVertices[0].pos.z = z;
        x = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[1].x);
        y = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[1].y);
        z = static_cast<f32>(at<boomerangVectorPair_c>(t)->mPoints[1].z);
        at<boomerangQuad_c>(buffer)->mVertices[1].pos.y = y;
        at<boomerangQuad_c>(buffer)->mVertices[1].pos.x = x;
        at<boomerangQuad_c>(buffer)->mVertices[1].pos.z = z;
        at<boomerangQuad_c>(buffer)->mVertices[1].v = zero;
        at<boomerangQuad_c>(buffer)->mVertices[1].u = previous;
        x = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[0].x);
        y = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[0].y);
        z = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[0].z);
        at<boomerangQuad_c>(buffer)->mVertices[2].pos.y = y;
        at<boomerangQuad_c>(buffer)->mVertices[2].pos.x = x;
        at<boomerangQuad_c>(buffer)->mVertices[2].pos.z = z;
        at<boomerangQuad_c>(buffer)->mVertices[2].v = one;
        at<boomerangQuad_c>(buffer)->mVertices[2].u = current;
        y = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[1].y);
        x = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[1].x);
        z = static_cast<f32>(at<boomerangVectorPair_c>(r)->mPoints[1].z);
        at<boomerangQuad_c>(buffer)->mVertices[3].pos.x = x;
        at<boomerangQuad_c>(buffer)->mVertices[3].u = previous;
        at<boomerangQuad_c>(buffer)->mVertices[3].pos.y = y;
        at<boomerangQuad_c>(buffer)->mVertices[3].v = one;
        at<boomerangQuad_c>(buffer)->mVertices[3].pos.z = z;
        previous = current;
        current += step;
    }
    u32 selectedOffset = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer) * 0x254;
    for (u32 i = 0; i < 60; ++i) {
        u32 vertexBuffer = ring + selectedOffset + 4 + i * 0x4A8;
        boomerang_uploadVertexBuffer_voidPtr_s32_u32(
            at<void>(vertexBuffer), 0,
            static_cast<u32>(at<boomerangVertexBuffer_c>(vertexBuffer)->mUploadSource));
    }
    at<boomerangBlurRing_c>(ring)->mWriteBuffer =
        static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer) == 0;
    count = static_cast<s32>(at<daBoomerang_blur_c>(a)->numTrailSegments);
    u32 selector = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer);
    for (s32 i = count; i >= 0; --i) {
        u32 pointIndex = (selector == 0) + 2u * static_cast<u32>(i);
        boomerang_bindVertexDeclaration(at<void>(ring + 0x254 * pointIndex + 0x158));
        u32 primitive = static_cast<u32>(at<daBoomerang_blur_c>(a)->mIndexBuffer.mPrimitive);
        if (primitive) {
            u32 vertices = static_cast<u32>(at<daBoomerang_blur_c>(a)->mIndexBuffer.mpIndices.v),
                offset = static_cast<u32>(at<daBoomerang_blur_c>(a)->mIndexBuffer.mOffset),
                number = static_cast<u32>(at<daBoomerang_blur_c>(a)->mIndexBuffer.mCount);
            boomerang_drawIndexed(number, primitive, at<void>(vertices), offset, 0, 1);
        }
        if (i != 0)
            selector = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer);
    }
}

static void boomerangSetupBlurRing(u32 ring, u32 material) {
    u32 declaration = ring + 0x11764, previous = ring + 0x11770;
    at<boomerangBlurRing_c>(ring)->mDeclarationWords[0] = 0x11;
    at<boomerangBlurRing_c>(ring)->mDeclarationWords[2] = ea(&l_blurVertexDeclaration);
    for (u32 side = 0; side < 2; ++side) {
        for (u32 i = 0; i < 60; ++i) {
            u32 point = ring + 0x254 * side + 0x4A8 * i;
            u32 buffer = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mVertices.v);
            if (!buffer) {
                u32 allocator = static_cast<u32>(l_heapManager);
                u32 heap = boomerang_getCurrentHeap(at<void>(allocator));
                u32 vtable = static_cast<u32>(at<boomerangHeap_c>(heap)->mpVtable.v);
                u32 allocation =
                    call_ptr<u32>(static_cast<u32>(at<boomerangHeapVtable_c>(vtable)->mAllocate.v),
                                  at<void>(heap), 0x50, 0x40);
                if (allocation) {
                    at<boomerangRenderPoint_c>(point)->mAllocation.v = allocation;
                    at<boomerangRenderPoint_c>(point)->mAllocationBytes = 4;
                }
                buffer = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mAllocation.v);
                at<boomerangRenderPoint_c>(point)->mVertices.v = buffer;
            }
            boomerang_setupVertexBuffer(at<void>(point + 4), at<void>(buffer), 4,
                                        at<void>(declaration));
            if (material &&
                material != static_cast<u32>(at<boomerangBlurRing_c>(ring)->mPrevious.v))
                boomerang_setupVertexDeclaration(at<void>(material), at<void>(point + 0x158),
                                                 at<void>(point + 4), at<void>(declaration), 0);
        }
    }
    at<boomerangBlurRing_c>(ring)->mPrevious.v = material;
    at<boomerangBlurRing_c>(ring)->mInitialized = 1;
}

static void boomerangInitializeBlurRing(u32 ring, f32 zero, f32 one) {
    u32 selector = ring + 0x11760;
    const u16 order[] = {0x14, 0x38, 0x40, 0x24, 0x48, 0x18, 0x28, 4,    0x44, 0x34,
                         0x4C, 0x30, 0x1C, 0,    0xC,  0x10, 0x3C, 0x2C, 8,    0x20};
    for (u32 i = 0; i < 60; ++i) {
        u32 index = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer) + 2 * i;
        u32 buffer = static_cast<u32>(at<boomerangRenderPoint_c>(ring)[index].mVertices.v);
        boomerangZeroCacheRange(buffer, 0x40);
        index = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer) + 2 * i;
        buffer = static_cast<u32>(at<boomerangRenderPoint_c>(ring)[index].mVertices.v);
        for (u16 offset : order)
            at<boomerangQuad_c>(buffer)->mValues[offset / 4] =
                offset == 0x38 || offset == 0x48 || offset == 0x4C || offset == 0x20 ? one : zero;
    }
    u32 index = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer),
        opposite = index == 0;
    for (u32 i = 0; i < 60; ++i) {
        u32 sourcePoint = ring + (index + 2 * i) * 0x254,
            destinationPoint = ring + opposite * 0x254 + i * 0x4A8;
        for (u32 group = 0; group < 4; ++group) {
            u32 source = static_cast<u32>(at<boomerangRenderPoint_c>(sourcePoint)->mVertices.v),
                destination =
                    static_cast<u32>(at<boomerangRenderPoint_c>(destinationPoint)->mVertices.v);
            for (u32 j = 0; j < 5; ++j) {
                u32 offset = group * 20 + j * 4;
                at<boomerangQuad_c>(destination)->mValues[offset / 4] =
                    static_cast<f32>(at<boomerangQuad_c>(source)->mValues[offset / 4]);
            }
        }
        index = static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer);
    }
    index *= 0x254;
    for (u32 i = 0; i < 60; ++i) {
        u32 point = ring + index + i * 0x4A8;
        u32 size = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mVertexBuffer.mUploadSource);
        boomerang_uploadVertexBuffer(at<void>(point + 4), 0, at<void>(size));
    }
    at<boomerangBlurRing_c>(ring)->mWriteBuffer =
        static_cast<u32>(at<boomerangBlurRing_c>(ring)->mWriteBuffer) == 0;
}

/* 020CE6CC */
void daBoomerang_blur_c::initBlur(void *matrix, s16 angle) {
    u32 a = ea(this), m = ea(matrix);
    boomerang_multiplyMatrixVector(matrix, at<void>(ea(&l_blurTop)),
                                   at<void>(ea(&this->mTrail[0][0])));
    boomerang_multiplyMatrixVector(matrix, at<void>(ea(&l_blurRoot)),
                                   at<void>(ea(&this->mTrail[1][0])));
    u32 z1 = static_cast<u32>(boomerangBits<u32>(this->mTrail[1][0].z)),
        y0 = static_cast<u32>(boomerangBits<u32>(this->mTrail[0][0].y));
    boomerangBits<u32>(this->mTrail[1][1].z) = z1;
    u32 x0 = static_cast<u32>(boomerangBits<u32>(this->mTrail[0][0].x));
    boomerangBits<u32>(this->mTrail[0][1].y) = y0;
    boomerangBits<u32>(this->mTrail[0][1].x) = x0;
    u32 y1 = static_cast<u32>(boomerangBits<u32>(this->mTrail[1][0].y)),
        z0 = static_cast<u32>(boomerangBits<u32>(this->mTrail[0][0].z));
    boomerangBits<u32>(this->mTrail[1][1].y) = y1;
    boomerangBits<u32>(this->mTrail[0][1].z) = z0;
    u32 x1 = static_cast<u32>(boomerangBits<u32>(this->mTrail[1][0].x));
    this->numTrailSegments = 0;
    boomerangBits<u32>(this->mTrail[1][1].x) = x1;
    this->pos.x = static_cast<f32>(at<Mtx34>(m)->m[0][3]);
    this->pos.y = static_cast<f32>(at<Mtx34>(m)->m[1][3]);
    this->pos.z = static_cast<f32>(at<Mtx34>(m)->m[2][3]);
    boomerang_copyMatrix(matrix, at<void>(ea(&l_matrixStack)));
    boomerang_rotateY(at<void>(ea(&l_matrixStack)), static_cast<s16>(-2 * static_cast<s32>(angle)));
    boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurTop)),
                                   at<void>(ea(&this->mTrail[2][0])));
    boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurRoot)),
                                   at<void>(ea(&this->mTrail[3][0])));
    u32 x2 = static_cast<u32>(boomerangBits<u32>(this->mTrail[2][0].x)),
        z3 = static_cast<u32>(boomerangBits<u32>(this->mTrail[3][0].z));
    boomerangBits<u32>(this->mTrail[2][1].x) = x2;
    boomerangBits<u32>(this->mTrail[3][1].z) = z3;
    u32 y3 = static_cast<u32>(boomerangBits<u32>(this->mTrail[3][0].y)),
        x3 = static_cast<u32>(boomerangBits<u32>(this->mTrail[3][0].x));
    boomerangBits<u32>(this->mTrail[3][1].y) = y3;
    boomerangBits<u32>(this->mTrail[3][1].x) = x3;
    u32 z2 = static_cast<u32>(boomerangBits<u32>(this->mTrail[2][0].z)),
        y2 = static_cast<u32>(boomerangBits<u32>(this->mTrail[2][0].y));
    boomerangBits<u32>(this->mTrail[2][1].z) = z2;
    boomerangBits<u32>(this->mTrail[2][1].y) = y2;
}

/* 020CE7E8 */
void daBoomerang_blur_c::copyBlur(void *matrix, s16 angle) {
    u32 a = ea(this);
    for (s32 i = 54; i >= 0; --i) {
        for (u32 lane = 0; lane < 4; ++lane) {
            cXyz *source = &this->mTrail[lane][static_cast<u32>(i)];
            u32 x = boomerangBits<u32>(source->x);
            u32 y = boomerangBits<u32>(source->y);
            u32 z = boomerangBits<u32>(source->z);
            boomerangBits<u32>(this->mTrail[lane][static_cast<u32>(i) + 5].x) = x;
            boomerangBits<u32>(this->mTrail[lane][static_cast<u32>(i) + 5].y) = y;
            boomerangBits<u32>(this->mTrail[lane][static_cast<u32>(i) + 5].z) = z;
        }
    }
    Local<cXyz> position, temporaryDifference, difference, scaled0, scaled1;
    f32 x = static_cast<f32>(at<Mtx34>(ea(matrix))->m[0][3]),
        y = static_cast<f32>(at<Mtx34>(ea(matrix))->m[1][3]);
    f32 zero = static_cast<f32>(l_zero), z = static_cast<f32>(at<Mtx34>(ea(matrix))->m[2][3]);
    position->y = y;
    position->x = x;
    position->z = z;
    boomerang_subtractVector(at<cXyz>(ea(&this->pos)), temporaryDifference.get(), position.get());
    boomerangBits<u32>(this->pos.x) = static_cast<u32>(boomerangBits<u32>(position->x));
    boomerangBits<u32>(difference->x) =
        static_cast<u32>(boomerangBits<u32>(temporaryDifference->x));
    boomerangBits<u32>(difference->y) =
        static_cast<u32>(boomerangBits<u32>(temporaryDifference->y));
    boomerangBits<u32>(this->pos.y) = static_cast<u32>(boomerangBits<u32>(position->y));
    boomerangBits<u32>(this->pos.z) = static_cast<u32>(boomerangBits<u32>(position->z));
    boomerangBits<u32>(difference->z) =
        static_cast<u32>(boomerangBits<u32>(temporaryDifference->z));
    boomerang_pushMatrix();
    f32 step = static_cast<f32>(l_blurInterpolationStep), t = zero;
    for (u32 i = 0; i < 5; ++i) {
        u32 top = a + 0xA8 + 12 * i, root = a + 0x378 + 12 * i;
        boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurTop)),
                                       at<void>(top));
        boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurRoot)),
                                       at<void>(root));
        boomerang_scaleVector(difference.get(), scaled0.get(), t);
        boomerang_addVector(at<cXyz>(top), scaled0.get(), at<cXyz>(top));
        boomerang_scaleVector(difference.get(), scaled0.get(), t);
        boomerang_addVector(at<cXyz>(root), scaled0.get(), at<cXyz>(root));
        t += step;
        boomerang_rotateY(at<void>(ea(&l_matrixStack)), static_cast<s16>(0x633));
    }
    t = zero;
    boomerang_popMatrix();
    boomerang_rotateY(at<void>(ea(&l_matrixStack)), static_cast<s16>(-2 * static_cast<s32>(angle)));
    for (u32 i = 0; i < 5; ++i) {
        u32 top = a + 0x648 + 12 * i, root = a + 0x918 + 12 * i;
        boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurTop)),
                                       at<void>(top));
        boomerang_multiplyMatrixVector(at<void>(ea(&l_matrixStack)), at<void>(ea(&l_blurRoot)),
                                       at<void>(root));
        boomerang_scaleVector(difference.get(), scaled1.get(), t);
        boomerang_addVector(at<cXyz>(top), scaled1.get(), at<cXyz>(top));
        boomerang_scaleVector(difference.get(), scaled1.get(), t);
        boomerang_addVector(at<cXyz>(root), scaled1.get(), at<cXyz>(root));
        t += step;
        boomerang_rotateY(at<void>(ea(&l_matrixStack)), static_cast<s16>(-0x633));
    }
    s32 count = static_cast<s32>(static_cast<u32>(this->numTrailSegments) + 5u);
    this->numTrailSegments = count >= 59 ? 58 : count;
}

/* 020CDF78 */
void daBoomerang_blur_c::draw() {
    u32 a = ea(this);
    boomerangBindRenderResource(&this->mResource.v);
    struct boomerangMatrix_c {
        be<f32> values[12];
    };
    Local<boomerangMatrix_c> matrix;
    boomerangCopyMatrix(matrix.get(), at<void>(ea(&l_viewMatrix)));
    u32 render = a + 0x23ADC, system = static_cast<u32>(l_renderSystem);
    boomerang_setRenderMatrices(at<void>(render), 0, matrix.get(),
                                at<void>(ea(&l_projectionMatrix)), at<void>(system + 0x240));
    boomerang_selectRenderPass(at<void>(render), 0);
    u32 output = static_cast<u32>(at<boomerangRenderObject_c>(render)->mpEntries.v),
        resource = static_cast<u32>(this->mResource.v);
    u32 ordinal = static_cast<u32>(at<boomerangUniformOutput_c>(output)->mSelectionIndex),
        selection = output + 0x10 + ordinal * 0x1C;
    u32 slots = static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mUniformCount)
                    ? static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mpUniforms.v)
                    : 0;
    s16 vertex = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mVertex),
        pixel = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mPixel),
        geometry = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mGeometry);
    u32 data = static_cast<u32>(at<boomerangUniformSelection_c>(selection)->mpData.v),
        stride = static_cast<u32>(at<boomerangUniformSelection_c>(selection)->mSize);
    if (pixel != -1)
        boomerang_setPixelUniform(pixel, at<void>(data), stride);
    if (vertex != -1)
        boomerang_setVertexUniform(vertex, at<void>(data), stride);
    if (geometry != -1)
        boomerang_setGeometryUniform(geometry, at<void>(data), stride);
    f32 divisor = static_cast<f32>(l_colorDivisor);
    f32 red = static_cast<f32>(static_cast<u8>(l_blurColor.r)) / divisor;
    f32 green = static_cast<f32>(static_cast<u8>(l_blurColor.g)) / divisor;
    f32 blue = static_cast<f32>(static_cast<u8>(l_blurColor.b)) / divisor;
    f32 alpha = static_cast<f32>(static_cast<u8>(l_blurColor.a)) / divisor;
    at<boomerangRenderObject_c>(render)->mLighting.mColors[4].mValues[1] = green;
    at<boomerangRenderObject_c>(render)->mLighting.mColors[4].mValues[2] = blue;
    at<boomerangRenderObject_c>(render)->mLighting.mColors[4].mValues[0] = red;
    at<boomerangRenderObject_c>(render)->mLighting.mColors[4].mValues[3] = alpha;
    boomerang_updateUniformTransform(at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting)));
    call_ptr<void>(
        static_cast<u32>(at<boomerangRenderObject_c>(render)->mLighting.mpVtable.get()->mBind.v),
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mLighting)),
        at<void>(static_cast<u32>(this->mResource.v)));
    boomerang_updateUniformTransform(
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform)));
    call_ptr<void>(
        static_cast<u32>(at<boomerangRenderObject_c>(render)->mTransform.mpVtable.get()->mBind.v),
        at<void>(ea(&at<boomerangRenderObject_c>(render)->mTransform)),
        at<void>(static_cast<u32>(this->mResource.v)));
    resource = static_cast<u32>(this->mResource.v);
    u32 texture = static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mSamplerCount)
                      ? static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mpSamplers.v)
                      : 0;
    boomerang_bindSampler(at<void>(ea(&this->mSampler)), at<void>(texture + 4), -1, 0);
    Local<boomerangGraphicsState_c> state;
    boomerang_getGraphicsState(state.get());
    u32 s = ea(state.get()),
        flags = static_cast<u32>(at<boomerangGraphicsState_c>(s)->mStencilBits);
    at<boomerangGraphicsState_c>(s)->mColorEnabled = 1;
    at<boomerangGraphicsState_c>(s)->mDepthEnabled = 0;
    at<boomerangGraphicsState_c>(s)->mAlphaEnabled = 0;
    at<boomerangGraphicsState_c>(s)->mBlendMode = 3;
    at<boomerangGraphicsState_c>(s)->mCullMode = 2;
    at<boomerangGraphicsState_c>(s)->mStencilBits = (((flags & ~15u) + 7u) & 0xFFFFFF0Fu) + 0x10;
    boomerang_setGraphicsState(state.get());
    s32 count = static_cast<s32>(this->numTrailSegments), denominator = (count >> 1) + 1;
    f32 step = (divisor / static_cast<f32>(denominator)) / divisor;
    f32 zero = static_cast<f32>(l_zero), one = static_cast<f32>(l_one);
    boomerangDrawBlurRing(a, a + 0xBEC, a + 0xA8, a + 0x378, count, step, zero, one);
    boomerangDrawBlurRing(a, a + 0x12364, a + 0x648, a + 0x918,
                          static_cast<s32>(this->numTrailSegments), step, zero, one);
}

/* 020CDBFC */
void daBoomerang_sightPacket_c::draw() {
    u32 a = ea(this);
    boomerangBindRenderResource(&this->mResource.v);
    struct boomerangMatrix_c {
        be<f32> values[12];
    };
    struct boomerangProjection_c {
        u8 bytes[0x40];
    };
    Local<boomerangGraphicsState_c> state;
    boomerang_getGraphicsState(state.get());
    u32 s = ea(state.get());
    u32 flags = static_cast<u32>(at<boomerangGraphicsState_c>(s)->mStencilBits);
    at<boomerangGraphicsState_c>(s)->mBlendMode = 3;
    at<boomerangGraphicsState_c>(s)->mCullMode = 2;
    at<boomerangGraphicsState_c>(s)->mColorEnabled = 0;
    at<boomerangGraphicsState_c>(s)->mAlphaEnabled = 0;
    at<boomerangGraphicsState_c>(s)->mDepthEnabled = 0;
    at<boomerangGraphicsState_c>(s)->mStencilBits = ((flags & ~15u) + 7u & 0xFFFFFF0Fu) + 0x10;
    boomerang_setGraphicsState(state.get());
    Local<boomerangMatrix_c> matrix;
    boomerangCopyMatrix(matrix.get(), at<void>(ea(&l_viewMatrix)));
    boomerang_identityMatrix(matrix.get());
    u32 camera = boomerang_getCameraProjection(at<void>(ea(&l_cameraProjectionOwner)));
    Local<boomerangProjection_c> projection;
    boomerang_copyProjection(at<void>(camera), projection.get());
    u32 render = a + 0x5F0, system = static_cast<u32>(l_renderSystem);
    boomerang_setRenderMatrices(at<void>(render), 0, matrix.get(), projection.get(),
                                at<void>(system + 0x240));
    boomerang_selectRenderPass(at<void>(render), 0);
    u32 output = static_cast<u32>(this->mRenderObjects[0].mpEntries.v),
        resource = static_cast<u32>(this->mResource.v);
    u32 ordinal = static_cast<u32>(at<boomerangUniformOutput_c>(output)->mSelectionIndex),
        selection = output + 0x10 + ordinal * 0x1C;
    u32 slots = static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mUniformCount)
                    ? static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mpUniforms.v)
                    : 0;
    s16 vertexSlot = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mVertex);
    u32 stride = static_cast<u32>(at<boomerangUniformSelection_c>(selection)->mSize);
    s16 pixelSlot = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mPixel);
    u32 data = static_cast<u32>(at<boomerangUniformSelection_c>(selection)->mpData.v);
    s16 geometrySlot = static_cast<s16>(at<boomerangUniformSlots_c>(slots)->mGeometry);
    if (vertexSlot != -1 || pixelSlot != -1 || geometrySlot != -1) {
        if (pixelSlot != -1)
            boomerang_setPixelUniform(pixelSlot, at<void>(data), stride);
        if (vertexSlot != -1)
            boomerang_setVertexUniform(vertexSlot, at<void>(data), stride);
        if (geometrySlot != -1)
            boomerang_setGeometryUniform(geometrySlot, at<void>(data), stride);
        resource = static_cast<u32>(this->mResource.v);
    }
    u32 texture = static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mSamplerCount)
                      ? static_cast<u32>(at<boomerangShaderMaterial_c>(resource)->mpSamplers.v)
                      : 0;
    boomerang_bindSampler(at<void>(ea(&this->mSampler)), at<void>(texture + 4), -1, 0);
    Local<be<u8>[8]> colors;
    Local<boomerangMatrix_c> localMatrix;
    for (u32 i = 0; i < 5; ++i) {
        if (!(static_cast<u32>(this->mSightOnFlg) & (1u << i)))
            continue;
        u8 alpha = static_cast<u8>(this->mScaleArr[i]);
        u32 c = ea(colors.get());
        at<be<u8>>(c)[0] = 255;
        at<be<u8>>(c)[1] = 255;
        at<be<u8>>(c)[2] = 50;
        at<be<u8>>(c)[3] = alpha;
        at<be<u8>>(c)[4] = 255;
        at<be<u8>>(c)[5] = 255;
        at<be<u8>>(c)[6] = 50;
        at<be<u8>>(c)[7] = alpha;
        u32 item = render + i * 0x41C;
        boomerangColorToFloat(
            at<void>(ea(&at<boomerangRenderObject_c>(item)->mLighting.mColors[4])), at<void>(c));
        boomerangColorToFloat(
            at<void>(ea(&at<boomerangRenderObject_c>(item)->mLighting.mColors[5])),
            at<void>(c + 4));
        for (u32 j = 0; j < 4; ++j)
            at<boomerangRenderObject_c>(item)->mLighting.mColors[2].mWords[j] =
                static_cast<u32>(l_sightUniformColor.mWords[j]);
        boomerang_updateUniformTransform(
            at<void>(ea(&at<boomerangRenderObject_c>(item)->mLighting)));
        u32 vtable = static_cast<u32>(at<boomerangRenderObject_c>(item)->mLighting.mpVtable.v);
        resource = static_cast<u32>(this->mResource.v);
        call_ptr<void>(static_cast<u32>(at<boomerangUniformVtable_c>(vtable)->mBind.v),
                       at<void>(ea(&at<boomerangRenderObject_c>(item)->mLighting)),
                       at<void>(resource));
        boomerangCopyMatrix(localMatrix.get(), at<void>(ea(&this->mMtxArr[i])));
        boomerang_copyMatrix(localMatrix.get(),
                             at<void>(ea(&at<boomerangRenderObject_c>(item)->mTransform.mMatrix)));
        boomerang_updateUniformTransform(
            at<void>(ea(&at<boomerangRenderObject_c>(item)->mTransform)));
        vtable = static_cast<u32>(at<boomerangRenderObject_c>(item)->mTransform.mpVtable.v);
        resource = static_cast<u32>(this->mResource.v);
        call_ptr<void>(static_cast<u32>(at<boomerangUniformVtable_c>(vtable)->mBind.v),
                       at<void>(ea(&at<boomerangRenderObject_c>(item)->mTransform)),
                       at<void>(resource));
        u32 selected = static_cast<u32>(this->mWriteBuffer) == 0;
        boomerang_bindVertexDeclaration(at<void>(ea(&this->mPoints[selected].mDeclaration)));
        u32 primitive = static_cast<u32>(this->mIndexBuffer.mPrimitive);
        if (primitive) {
            u32 vertices = static_cast<u32>(this->mIndexBuffer.mpIndices.v),
                vertexOffset = static_cast<u32>(this->mIndexBuffer.mOffset),
                count = static_cast<u32>(this->mIndexBuffer.mCount);
            boomerang_drawIndexed(count, primitive, at<void>(vertices), vertexOffset, 0, 1);
        }
    }
}

/* 020CBB70 */
void daBoomerang_sightPacket_c::setSight(cXyz *position, s32 index) {
    u32 a = ea(this);
    u32 bit = (static_cast<u32>(index) & 0x20) ? 0 : 1u << (static_cast<u32>(index) & 31);
    if (!position) {
        this->mSightOnFlg = static_cast<u32>(this->mSightOnFlg) & ~bit;
        return;
    }
    struct boomerangViewport_c {
        be<f32> x, y, width, height, nearZ, farZ;
    };
    Local<boomerangViewport_c> viewport;
    Local<cXyz> screen;
    boomerang_getViewport(viewport.get());
    f32 viewportScale = static_cast<f32>(viewport->height) / static_cast<f32>(l_viewportHeight);
    boomerang_projectPosition(position, screen.get(), viewport.get());
    f32 x = screen->x, y = screen->y, z = screen->z;
    boomerang_translateMatrixInto(at<void>(ea(&l_matrixStack)), x, y, z);
    f32 half = static_cast<f32>(l_halfAnimLength);
    u8 frame = static_cast<u8>(this->mFrameArr[static_cast<u32>(index)]);
    f32 phase, rotation;
    if (frame < 13) {
        phase = static_cast<f32>(l_one) - static_cast<f32>(frame) / half;
        f32 alpha =
            fmadds(static_cast<f32>(l_sightAlphaRange), phase, static_cast<f32>(l_sightAlphaBase));
        this->mScaleArr[static_cast<u32>(index)] = static_cast<u8>(ftoi(alpha));
        rotation = -phase;
    } else {
        phase = static_cast<f32>(static_cast<s32>(frame) - 13) / half;
        f32 alpha =
            fmadds(static_cast<f32>(l_sightAlphaRange), phase, static_cast<f32>(l_sightAlphaBase));
        this->mScaleArr[static_cast<u32>(index)] = static_cast<u8>(ftoi(alpha));
        rotation = phase;
    }
    f32 divisor = static_cast<f32>(l_sightViewportDivisor) / viewportScale;
    u32 image = static_cast<u32>(this->mImage.v);
    u16 height = static_cast<u16>(at<boomerangImage_c>(image)->mHeight),
        width = static_cast<u16>(at<boomerangImage_c>(image)->mWidth);
    f32 size = fmadds(static_cast<f32>(l_sightSizeRange), phase, static_cast<f32>(l_sightSizeBase));
    f32 scale = size * static_cast<f32>(l_sightSizeScale);
    f32 scaleX = (scale * static_cast<f32>(width)) / divisor;
    f32 scaleY = (scale * static_cast<f32>(height)) / divisor;
    boomerang_scaleMatrix(scaleX, scaleY, scale);
    u16 turn = static_cast<u16>(ftoi(static_cast<f32>(l_quarterTurn) * rotation));
    f32 sine = static_cast<f32>(l_sinCosTable.mValues[turn >> 3][0]);
    s16 angle = static_cast<s16>(ftoi(static_cast<f32>(l_halfTurn) * sine));
    boomerang_rotateZ(at<void>(ea(&l_matrixStack)), angle);
    boomerang_copyMatrix(at<void>(ea(&l_matrixStack)),
                         at<void>(ea(&this->mMtxArr[static_cast<u32>(index)])));
    this->mSightOnFlg = static_cast<u32>(this->mSightOnFlg) | bit;
}

/* 020CC2EC */
void daBoomerang_sightPacket_c::play(s32 count) {
    for (s32 i = 0; i < count; ++i) {
        u8 frame = static_cast<u8>(static_cast<u8>(this->mFrameArr[i]) + 1);
        this->mFrameArr[i] = frame == 26 ? 0 : frame;
    }
}

/* 020CBE34 */
// HD: Cafe draw-list passes and the Link keep-matrix update replace the GX draw path.
BOOL daBoomerang_c::draw() {
    u32 a = ea(this), play = ea(dComIfGp_get());
    l_opaqueBuffer = static_cast<u32>(at<boomerangGameInfo_c>(play)->mDrawList.mpOpaListP1);
    play = ea(dComIfGp_get());
    l_translucentBuffer =
        static_cast<u32>(at<boomerangGameInfo_c>(play)->mDrawList.mpOpaListPlayerExtra);
    if (!static_cast<u8>(this->mThirdPerson) && static_cast<u8>(this->mLockCnt)) {
        this->mSightPacket.mSightOnFlg = 0;
        for (u32 index = static_cast<u8>(this->mCurLockIdx);
             index < static_cast<u8>(this->mLockCnt); ++index) {
            u32 target = static_cast<u32>(this->mLockActors[index].v);
            at<daBoomerang_sightPacket_c>(ea(at<void>(ea(&this->mSightPacket))))
                ->setSight(target ? at<cXyz>(target + 0x37C) : nullptr, index);
        }
        play = ea(dComIfGp_get());
        u32 list = play + 0x5D30;
        boomerang_entrySight(at<void>(list), at<void>(list + 0x264), at<void>(list + 0x268),
                             at<void>(ea(&this->mSightPacket)));
    }
    if (static_cast<s32>(this->mBlur.numTrailSegments) > 0) {
        play = ea(dComIfGp_get());
        u32 buffer = static_cast<u32>(at<boomerangGameInfo_c>(play)->mDrawList.mpXluList);
        boomerang_entryBlur(at<void>(play + 0x5D30), at<void>(buffer), at<void>(ea(&this->mBlur)),
                            at<cXyz>(ea(&this->current.pos)));
    }
    play = ea(dComIfGp_get());
    u32 player = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    u32 vtable = static_cast<u32>(at<daPy_lk_c>(player)->__vtbl);
    u32 hidden = call_ptr<u32>(
        static_cast<u32>(at<boomerangVisibilityVtable_c>(vtable)->mCheckHide.v), at<void>(player));
    if (!(hidden && !static_cast<u32>(this->mParameters))) {
        u32 light = boomerang_getEnvironmentLight();
        boomerang_settingTevStruct(at<void>(light), 0, at<cXyz>(ea(&this->current.pos)),
                                   at<void>(ea(&this->tevStr)));
        light = boomerang_getEnvironmentLight();
        u32 model = static_cast<u32>(this->mpModel.v);
        boomerang_setLightTevColorType(at<void>(light), at<void>(model),
                                       at<void>(ea(&this->tevStr)));
        bool ice = false;
        if (!static_cast<u32>(this->mParameters)) {
            play = ea(dComIfGp_get());
            u32 link = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mpPlayerPtr[0].v);
            ice = (static_cast<u32>(at<daPy_lk_c>(link)->mNoResetFlg1) & 0x800) != 0;
        }
        model = static_cast<u32>(this->mpModel.v);
        if (ice)
            boomerang_updateIceModel(at<void>(model), -1, 0);
        else {
            boomerang_modelUpdateDL(at<void>(model), 0);
            play = ea(dComIfGp_get());
            model = static_cast<u32>(this->mpModel.v);
            u32 link = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mpPlayerPtr[0].v);
            boomerang_updateKeepMatrix(at<void>(link), at<void>(ea(&this->mKeep)), at<void>(model));
        }
    }
    play = ea(dComIfGp_get());
    l_opaqueBuffer = static_cast<u32>(at<boomerangGameInfo_c>(play)->mDrawList.mpOpaList);
    play = ea(dComIfGp_get());
    l_translucentBuffer = static_cast<u32>(at<boomerangGameInfo_c>(play)->mDrawList.mpXluList);
    return 1;
}

/* 020CC01C */
BOOL daBoomerang_Draw(daBoomerang_c *actor) { return actor->draw(); }

/* 020CECF0 */
f32 daBoomerang_c::getFlyMax() {
    u32 play = ea(dComIfGp_get());
    if (static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mPlayerStatus[0][0]) & 0x10000)
        return static_cast<f32>(l_longFlyMax);
    return (boomerangStageEquals(guest_addr("GanonK")) ? static_cast<f32>(l_longFlyMax)
                                                       : static_cast<f32>(l_normalFlyMax));
}

/* 020CC13C */
void daBoomerang_c::rockLineCallback(fopAc_ac_c *target) {
    if (!static_cast<u32>(this->mParameters)) {
        if (target)
            this->setLockActor(target, 1);
        return;
    }
    for (u32 i = 0; i < 5; ++i) {
        if (this->mLockActors[i].get() != target)
            continue;
        this->mLockActorIDs[i] = 0xFFFFFFFF;
        this->mLockActors[i] = nullptr;
        if (i == static_cast<u8>(this->mCurLockIdx)) {
            this->mJustHit = 1;
            this->mCurLockIdx = static_cast<u8>(static_cast<u8>(this->mCurLockIdx) + 1);
        }
    }
}

/* 020CC218 */
void boomerangRockLineThunk(daBoomerang_c *actor, void *ignored, fopAc_ac_c *target) {
    actor->rockLineCallback(target);
}

/* 020CF87C */
void daBoomerang_c::setAimActor(fopAc_ac_c *target) {
    this->resetLockActor();
    this->mThirdPerson = 1;
    this->setLockActor(target, 0);
}

/* 020CC020 */
BOOL daBoomerang_c::setLockActor(fopAc_ac_c *target, s32 sound) {
    u8 count = this->mLockCnt;
    if (count >= 5)
        return 0;
    u32 id = target ? static_cast<u32>(target->mBsPcId) : 0xFFFFFFFF;
    for (u32 i = 0; i < count; ++i)
        if (static_cast<u32>(this->mLockActorIDs[i]) == id)
            return 0;
    this->mLockActorIDs[count] = id;
    this->mLockActors[static_cast<u8>(this->mLockCnt)] = target;
    this->mSightPacket.mFrameArr[static_cast<u8>(this->mLockCnt)] = 0;
    if (sound) {
        u8 index = this->mLockCnt;
        boomerang_startLockSound(static_cast<u32>(l_lockSounds.mSounds[index]));
    }
    this->mLockCnt = static_cast<u8>(static_cast<u8>(this->mLockCnt) + 1);
    return 1;
}

/* 020CEAB4 */
void daBoomerang_c::resetLockActor() {
    for (u32 i = 0; i < 5; ++i) {
        this->mLockActorIDs[i] = 0xFFFFFFFF;
        this->mLockActors[i] = nullptr;
    }
    this->mLockCnt = 0;
    this->mCurLockIdx = 0;
}

/* 020CC220 */
// HD: retain the unsigned room byte used by the collision and lighting state.
void daBoomerang_c::setRoomInfo() {
    u32 a = ea(this);
    boomerangBits<u32>(this->mGndChk.m_pos.x) =
        static_cast<u32>(boomerangBits<u32>(this->current.pos.x));
    boomerangBits<u32>(this->mGndChk.m_pos.y) =
        static_cast<u32>(boomerangBits<u32>(this->current.pos.y));
    boomerangBits<u32>(this->mGndChk.m_pos.z) =
        static_cast<u32>(boomerangBits<u32>(this->current.pos.z));
    u32 play = ea(dComIfGp_get());
    f32 ground = boomerang_groundCross(at<void>(play + 0x12A0), at<void>(ea(&this->mGndChk)));
    u8 room;
    if (ground != static_cast<f32>(l_noGround)) {
        play = ea(dComIfGp_get());
        room = boomerang_getRoomId(at<void>(play + 0x12A0), at<void>(a + 0x26480));
        play = ea(dComIfGp_get());
        u8 color = boomerang_getPolyColor(at<void>(play + 0x12A0), at<void>(a + 0x26480));
        this->tevStr.mEnvrIdxOverride = color;
    } else
        room = static_cast<u8>(l_stayRoom);
    this->tevStr.mRoomNo = room;
    this->mStts.mRoomId = room;
    this->current.roomNo = room;
}

/* 020CD264 */
// HD: obtain the held-item transform through Link's virtual keep-matrix accessor.
void daBoomerang_c::setKeepMatrix() {
    u32 play = ea(dComIfGp_get());
    u32 player = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    u32 vtable = static_cast<u32>(at<daPy_lk_c>(player)->__vtbl);
    u32 target = static_cast<u32>(at<boomerangTransformVtable_c>(vtable)->mGetKeepMatrix.v);
    u32 source = call_ptr<u32>(target, at<void>(player));
    boomerang_copyMatrix(at<void>(source), at<void>(ea(&l_matrixStack)));
    f32 z = static_cast<f32>(l_keepOffsetZ), x = static_cast<f32>(l_keepOffsetX),
        y = static_cast<f32>(l_keepOffsetY);
    boomerang_translateMatrix(x, y, z);
    boomerang_rotateXYZ(at<void>(ea(&l_matrixStack)), static_cast<s16>(-0x39F4),
                        static_cast<s16>(-0x357), static_cast<s16>(-0x2AF3));
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = static_cast<f32>(l_matrixStack.m[i / 4][i % 4]);
    u32 model = static_cast<u32>(this->mpModel.v);
    // All source values are captured before any destination store, including overlap.
    for (u32 i = 0; i < 12; ++i)
        at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    x = static_cast<f32>(l_matrixStack.m[0][3]);
    z = static_cast<f32>(l_matrixStack.m[2][3]);
    y = static_cast<f32>(l_matrixStack.m[1][3]);
    this->current.pos.y = y;
    this->current.pos.z = z;
    this->current.pos.x = x;
}

/* 020CEB00 */
void daBoomerang_c::setAimPos() {
    u32 a = ea(this);
    if (static_cast<u8>(this->mIsReturning)) {
        u32 play = ea(dComIfGp_get());
        u32 player = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mpPlayerPtr[0].v);
        f32 x = static_cast<f32>(at<daPy_lk_c>(player)->mBoomerangCatchPos.x),
            y = static_cast<f32>(at<daPy_lk_c>(player)->mBoomerangCatchPos.y),
            z = static_cast<f32>(at<daPy_lk_c>(player)->mBoomerangCatchPos.z);
        this->mTargetPos.y = y;
        this->mTargetPos.x = x;
        this->mTargetPos.z = z;
        return;
    }
    u8 current = this->mCurLockIdx;
    s32 index = current;
    while (index < static_cast<u8>(this->mLockCnt)) {
        u32 target = ea(this->mLockActors[index].get());
        if (target) {
            boomerangBits<u32>(this->mTargetPos.x) =
                static_cast<u32>(boomerangBits<u32>(at<fopAc_ac_c>(target)->eyePos.x));
            boomerangBits<u32>(this->mTargetPos.y) =
                static_cast<u32>(boomerangBits<u32>(at<fopAc_ac_c>(target)->eyePos.y));
            boomerangBits<u32>(this->mTargetPos.z) =
                static_cast<u32>(boomerangBits<u32>(at<fopAc_ac_c>(target)->eyePos.z));
            return;
        }
        ++index;
        this->mCurLockIdx = ++current;
    }
}

/* 020CEBDC */
void daBoomerang_c::checkBgHit(cXyz *from, cXyz *to) {
    u32 a = ea(this), check = a + 0x26400;
    boomerang_setLineCheck(at<void>(check), from, to, this);
    u32 play = ea(dComIfGp_get());
    if (!boomerang_lineCross(at<void>(play + 0x12A0), at<void>(check)))
        return;
    boomerangBits<u32>(this->current.pos.x) =
        static_cast<u32>(boomerangBits<u32>(this->mLinChk.mLin.mEnd.x));
    boomerangBits<u32>(this->current.pos.y) =
        static_cast<u32>(boomerangBits<u32>(this->mLinChk.mLin.mEnd.y));
    boomerangBits<u32>(this->current.pos.z) =
        static_cast<u32>(boomerangBits<u32>(this->mLinChk.mLin.mEnd.z));
    play = ea(dComIfGp_get());
    u32 particle = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mParticle.v);
    boomerang_setParticle(at<void>(particle), 1, 0xC, at<cXyz>(ea(&this->current.pos)), 0, 0, 0xFF,
                          0, -1, 0, 0, 0);
    this->mIsReturning = 1;
    s16 angle =
        static_cast<s16>(static_cast<u16>(static_cast<s16>(this->current.angle.y)) - 0x8000u);
    this->current.angle.y = angle;
    this->shape_angle.y = angle;
    this->resetLockActor();
    play = ea(dComIfGp_get());
    u32 material = boomerang_getMaterialSound(at<void>(play + 0x12A0), at<void>(check + 0x14));
    s32 room = static_cast<s8>(this->current.roomNo);
    s32 reverb = boomerang_getReverbRegister(room);
    boomerang_startSound(0x2833, at<cXyz>(ea(&this->eyePos)), material, reverb);
}

/* 020CF4CC */
BOOL daBoomerang_c::procWait() {
    u32 a = ea(this), play = ea(dComIfGp_get());
    f32 zero = static_cast<f32>(l_zero);
    u32 player = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mpPlayerPtr[0].v);
    this->speedF = zero;
    this->setKeepMatrix();
    s32 segments = static_cast<s32>(this->mBlur.numTrailSegments);
    u32 mode = static_cast<u32>(this->mParameters);
    if (segments > 0)
        this->mBlur.numTrailSegments = segments - 5;
    if (mode == 1) {
        f32 speed = static_cast<f32>(l_throwSpeed);
        this->speedF = speed;
        boomerang_resetAttackHit(at<void>(ea(&this->mCps)));
        u32 flags = static_cast<u32>(this->mCps.mObjAt.mSPrm);
        this->mIsReturning = 0;
        this->mJustHit = 0;
        this->mCps.mObjAt.mSPrm = flags & ~0x10u;
        this->unused_0xF2D = 1;
        this->mCurLockIdx = 0;
        this->setAimPos();
        if (static_cast<u8>(this->mCurLockIdx) == static_cast<u8>(this->mLockCnt)) {
            this->resetLockActor();
            s16 yaw = static_cast<s16>(at<daPy_lk_c>(player)->shape_angle.y),
                aimYaw = static_cast<s16>(at<daPy_lk_c>(player)->mBodyAngle.y);
            u16 pitch = static_cast<u16>(at<daPy_lk_c>(player)->mBodyAngle.x);
            u16 direction = static_cast<u16>(static_cast<s32>(yaw) + aimYaw);
            f32 range = this->getFlyMax();
            u32 yawTable = ea(&l_sinCosTable.mValues[direction >> 3]),
                pitchTable = ea(&l_sinCosTable.mValues[pitch >> 3]);
            f32 x = static_cast<f32>(this->current.pos.x);
            f32 sinYaw = static_cast<f32>(at<be<f32>>(yawTable)[0]),
                cosYaw = static_cast<f32>(at<be<f32>>(yawTable)[1]);
            f32 horizontalX = range * sinYaw;
            f32 cosPitch = static_cast<f32>(at<be<f32>>(pitchTable)[1]),
                sinPitch = static_cast<f32>(at<be<f32>>(pitchTable)[0]);
            f32 horizontalZ = range * cosYaw;
            f32 y = static_cast<f32>(this->current.pos.y),
                z = static_cast<f32>(this->current.pos.z);
            this->mTargetPos.x = fmadds(horizontalX, cosPitch, x);
            this->mTargetPos.y = -fmadds(range, sinPitch, -y);
            this->mTargetPos.z = fmadds(horizontalZ, cosPitch, z);
        }
        Local<cXyz> difference, horizontal;
        this->mModelRotY = 0;
        boomerang_subtractVector(at<cXyz>(ea(&this->mTargetPos)), difference.get(),
                                 at<cXyz>(ea(&this->current.pos)));
        f32 dx = difference->x, dz = difference->z;
        horizontal->x = dx;
        horizontal->z = dz;
        horizontal->y = zero;
        f32 square = boomerang_squareMagnitude(horizontal.get());
        f32 magnitude = boomerang_squareRoot(square);
        f32 dy = difference->y;
        s16 pitch = boomerang_atan2s(-dy, magnitude);
        this->current.angle.x = pitch;
        u8 count = this->mLockCnt;
        dx = difference->x;
        dz = difference->z;
        s16 direction = boomerang_atan2s(dx, dz);
        if (!count) {
            this->current.angle.y = static_cast<s16>(static_cast<s32>(direction) + 0x3000);
            this->mFreeFlyOut = static_cast<u8>(this->mThirdPerson) == 0;
        } else {
            this->current.angle.y = direction;
            this->mFreeFlyOut = 0;
        }
        s16 yaw = static_cast<s16>(this->current.angle.y),
            vertical = static_cast<s16>(this->current.angle.x);
        this->shape_angle.y = yaw;
        this->shape_angle.x = vertical;
        this->shape_angle.z = 0x2000;
        this->mProcAdjust = 0;
        this->mProcTarget = boomerangTU.procMove;
        this->mModelRotZ = 0x2000;
        this->mProcSelector = -1;
        this->mCancelFlg = 0;
        u32 model = static_cast<u32>(this->mpModel.v);
        s16 spin = static_cast<s16>(this->mModelRotY);
        at<daBoomerang_blur_c>(ea(at<void>(ea(&this->mBlur))))
            ->initBlur(model ? at<void>(model + 0xC8) : nullptr, spin);
        f32 playerY = static_cast<f32>(at<daPy_lk_c>(player)->current.pos.y),
            playerX = static_cast<f32>(at<daPy_lk_c>(player)->current.pos.x),
            playerZ = static_cast<f32>(at<daPy_lk_c>(player)->current.pos.z);
        Local<cXyz> start;
        start->x = playerX;
        start->y = playerY + speed;
        start->z = playerZ;
        this->checkBgHit(start.get(), at<cXyz>(ea(&this->current.pos)));
        boomerang_moveFlight(this);
        return 1;
    }
    u32 camera = boomerang_getCameraBody();
    if (static_cast<u32>(at<boomerangCameraBody_c>(camera)->mMode) != 0xB) {
        this->resetLockActor();
        return 1;
    }
    u32 procedure = static_cast<u32>(at<daPy_lk_c>(player)->mCurProc);
    if ((procedure != 0x80 && procedure != 0x8B) ||
        !static_cast<u8>(
            at<boomerangPlayerSight_c>(ea(&at<daPy_lk_c>(player)->mSightPacket))->mDraw))
        return 1;
    play = ea(dComIfGp_get());
    s32 cameraIndex = static_cast<s8>(at<boomerangGameInfo_c>(play)->play.mPlayerInfo[0].mCameraID);
    play = ea(dComIfGp_get());
    f32 length = static_cast<f32>(l_sightLineRadius);
    u32 flags = static_cast<u32>(this->mCps.mObjAt.mSPrm);
    u32 cameraData =
        static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mCameraInfo[cameraIndex].mpCamera);
    Local<cXyz> cameraPosition, targetPosition;
    boomerangBits<u32>(cameraPosition->x) =
        boomerangBits<u32>(at<boomerangCamera_c>(cameraData)->mCenter.x);
    boomerangBits<u32>(cameraPosition->y) =
        boomerangBits<u32>(at<boomerangCamera_c>(cameraData)->mCenter.y);
    boomerangBits<u32>(cameraPosition->z) =
        boomerangBits<u32>(at<boomerangCamera_c>(cameraData)->mCenter.z);
    f32 x = static_cast<f32>(
            at<boomerangPlayerSight_c>(ea(&at<daPy_lk_c>(player)->mSightPacket))->mLineTop.x),
        y = static_cast<f32>(
            at<boomerangPlayerSight_c>(ea(&at<daPy_lk_c>(player)->mSightPacket))->mLineTop.y),
        z = static_cast<f32>(
            at<boomerangPlayerSight_c>(ea(&at<daPy_lk_c>(player)->mSightPacket))->mLineTop.z);
    this->mCps.mObjAt.mSPrm = flags | 0x10;
    targetPosition->x = x;
    targetPosition->y = y;
    targetPosition->z = z;
    boomerang_setCapsuleGeometry(at<cXyz>(a + 0x263E0), cameraPosition.get(), targetPosition.get(),
                                 length);
    boomerang_subtractVectorInto(at<cXyz>(a + 0x263EC), at<cXyz>(a + 0x263E0),
                                 at<cXyz>(a + 0x26344));
    this->mCps.mGObjAt.mHitCallback = guest_fn((const void *)boomerangRockLineThunk);
    play = ea(dComIfGp_get());
    boomerang_setCollision(at<void>(play + 0x26A4), at<void>(ea(&this->mCps)));
    return 1;
}

/* 020CEE10 */
BOOL daBoomerang_c::procMove() {
    u32 a = ea(this), lock = a + 0x26244;
    if (!static_cast<u32>(l_offsetInitialized))
        l_offsetInitialized = 1;
    if (static_cast<u8>(this->mCatchAndDelete)) {
        boomerang_deleteActor(this);
        return 1;
    }
    u32 play = ea(dComIfGp_get());
    if (!static_cast<u8>(at<boomerangGameInfo_c>(play)->play.mEvtCtrl.mMode)) {
        play = ea(dComIfGp_get());
        s16 oldSpin = static_cast<s16>(this->mModelRotY),
            spin = static_cast<s16>(static_cast<s32>(oldSpin) - 0x1F00);
        u32 player = static_cast<u32>(at<boomerangGameInfo_c>(play)->play.mpPlayerPtr[0].v);
        this->mModelRotY = spin;
        if (oldSpin >= 0 && spin < 0) {
            s32 reverb = boomerang_getReverbRegister(
                static_cast<s32>(static_cast<s8>(this->current.roomNo)));
            boomerang_startSound_s32_cXyzPtr_s32_s32(0x2814, at<cXyz>(ea(&this->eyePos)), 0,
                                                     reverb);
        }
        this->setAimPos();
        Local<cXyz> difference, normalized;
        boomerang_subtractVector(at<cXyz>(lock + 0x3C), difference.get(),
                                 at<cXyz>(ea(&this->current.pos)));
        f32 square = boomerang_squareMagnitude(difference.get());
        f32 distance = boomerang_squareRoot(square);
        cXyz *direction = difference.get();
        if (distance > static_cast<f32>(l_normalizeThreshold)) {
            boomerang_divideVector(difference.get(), normalized.get(), distance);
            direction = normalized.get();
        }
        f32 dz = direction->z, dx = direction->x, dy = direction->y;
        s32 desiredYaw = boomerang_atan2s(dx, dz);
        s16 currentYaw = static_cast<s16>(this->current.angle.y);
        s16 delta = static_cast<s16>(desiredYaw - currentYaw);
        if (static_cast<u8>(this->mJustHit)) {
            this->mJustHit = 0;
            s16 yaw = static_cast<s16>(this->current.angle.y);
            s16 remaining = static_cast<s16>(desiredYaw - yaw);
            s32 adjusted = desiredYaw + (remaining > 0 ? -0x3000 : 0x3000);
            delta = static_cast<s16>(desiredYaw - adjusted);
            this->current.angle.y = static_cast<s16>(adjusted);
        }
        f32 speed = static_cast<f32>(this->speedF);
        if (static_cast<u8>(this->mIsReturning)) {
            if (distance < speed + speed) {
                if (boomerang_returnBoomerang(at<void>(player))) {
                    this->mParameters = 0;
                    u8 returning = static_cast<u8>(this->mIsReturning);
                    this->mThirdPerson = 0;
                    this->mProcTarget = boomerangTU.procWait;
                    this->mProcAdjust = 0;
                    this->mCps.mGObjAt.mHitCallback = 0;
                    this->mProcSelector = -1;
                    (void)returning;
                } else
                    this->mCatchAndDelete = 1;
            }
        } else {
            bool advance = distance < speed;
            if (!advance && boomerang_checkAttackHit(at<void>(lock + 0x84))) {
                u32 hit = boomerang_getHitActor(at<void>(lock + 0xD4));
                u8 index = static_cast<u8>(this->mCurLockIdx);
                advance = hit == static_cast<u32>(this->mLockActors[index].v);
            }
            if (advance) {
                u8 index = static_cast<u8>(this->mCurLockIdx);
                this->mJustHit = 1;
                this->mLockActors[index].v = 0;
                index = static_cast<u8>(this->mCurLockIdx);
                this->mLockActorIDs[index] = fpcM_ERROR_PROCESS_ID_e;
                index = static_cast<u8>(this->mCurLockIdx);
                this->mCurLockIdx = static_cast<u8>(index + 1);
                this->mFreeFlyOut = 0;
            }
        }
        if (!static_cast<u8>(this->mIsReturning)) {
            bool turnBack = static_cast<u8>(this->mCancelFlg) != 0;
            if (!turnBack) {
                bool remaining =
                    static_cast<u8>(this->mFreeFlyOut) ||
                    static_cast<u8>(this->mLockCnt) > static_cast<u8>(this->mCurLockIdx);
                turnBack = !remaining || (static_cast<u32>(this->mCps.mGObjAt.mRPrm) & 1);
            }
            if (turnBack) {
                this->mJustHit = 1;
                this->mIsReturning = 1;
                this->resetLockActor();
                this->mCancelFlg = 0;
                this->mFreeFlyOut = 0;
            }
        }
        f32 zero = static_cast<f32>(l_zero);
        if (static_cast<u8>(this->mJustHit))
            this->current.angle.y = static_cast<s16>(desiredYaw);
        else {
            s32 limit;
            if (static_cast<u8>(this->mThirdPerson))
                limit = 0x4000;
            else {
                f32 speedNow = static_cast<f32>(this->speedF);
                f32 factor = static_cast<f32>(l_turnFactor) - (distance + distance) / speedNow;
                if (factor < zero)
                    factor = zero;
                else if (factor > static_cast<f32>(l_turnThreshold))
                    factor = fmadds(static_cast<f32>(l_turnAcceleration),
                                    factor - static_cast<f32>(l_turnThreshold),
                                    static_cast<f32>(l_turnFactor));
                limit = static_cast<s16>(ftoi(
                    fmadds(factor, static_cast<f32>(l_turnScale), static_cast<f32>(l_turnBase))));
            }
            s16 yaw = static_cast<s16>(this->current.angle.y);
            if (delta > limit)
                delta = static_cast<s16>(limit);
            else if (delta < -limit)
                delta = static_cast<s16>(-limit);
            this->current.angle.y = static_cast<s16>(static_cast<s32>(yaw) + delta);
        }
        Local<cXyz> horizontal;
        horizontal->y = zero;
        horizontal->z = dz;
        horizontal->x = dx;
        square = boomerang_squareMagnitude(horizontal.get());
        f32 horizontalLength = boomerang_squareRoot(square);
        s16 pitch = boomerang_atan2s(-dy, horizontalLength);
        u16 pitchBits = static_cast<u16>(pitch), yawBits = static_cast<u16>(this->current.angle.y);
        u32 pitchTable = ea(&l_sinCosTable.mValues[pitchBits >> 3]),
            yawTable = ea(&l_sinCosTable.mValues[yawBits >> 3]);
        speed = static_cast<f32>(this->speedF);
        this->current.angle.x = pitch;
        f32 cosPitch = static_cast<f32>(at<be<f32>>(pitchTable)[1]),
            x = static_cast<f32>(this->current.pos.x);
        f32 horizontalSpeed = speed * cosPitch, sinYaw = static_cast<f32>(at<be<f32>>(yawTable)[0]);
        f32 nextX = fmadds(horizontalSpeed, sinYaw, x);
        f32 y = static_cast<f32>(this->current.pos.y);
        this->current.pos.x = nextX;
        f32 sinPitch = static_cast<f32>(at<be<f32>>(pitchTable)[0]),
            nextY = -fmadds(speed, sinPitch, -y);
        f32 z = static_cast<f32>(this->current.pos.z);
        this->current.pos.y = nextY;
        cosPitch = static_cast<f32>(at<be<f32>>(pitchTable)[1]);
        f32 cosYaw = static_cast<f32>(at<be<f32>>(yawTable)[1]);
        f32 nextZ = fmadds(speed * cosPitch, cosYaw, z);
        this->shape_angle.y = static_cast<s16>(yawBits);
        this->shape_angle.x = pitch;
        this->current.pos.z = nextZ;
        if (!static_cast<u8>(this->mIsReturning))
            this->checkBgHit(at<cXyz>(ea(&this->old.pos)), at<cXyz>(ea(&this->current.pos)));
        s16 rollChange = static_cast<s16>(-2 * static_cast<s32>(delta));
        s16 roll;
        if (rollChange > 0x100)
            roll = 0x2000;
        else if (rollChange < -0x100)
            roll = -0x2000;
        else
            roll = static_cast<s16>(this->mModelRotZ);
        this->mModelRotZ = roll;
        boomerang_addCalcAngleS(at<be<s16>>(a + 0x32C), roll, 0x10, 0x1000, 0x10);
    }
    f32 x = static_cast<f32>(this->current.pos.x), y = static_cast<f32>(this->current.pos.y),
        z = static_cast<f32>(this->current.pos.z);
    boomerang_translateMatrixInto(at<void>(ea(&l_matrixStack)), x, y, z);
    s16 rx = static_cast<s16>(this->shape_angle.x), ry = static_cast<s16>(this->shape_angle.y),
        rz = static_cast<s16>(this->shape_angle.z);
    boomerang_rotateZXY(at<void>(ea(&l_matrixStack)), rx, ry, rz);
    s16 spin = static_cast<s16>(this->mModelRotY);
    boomerang_rotateY(at<void>(ea(&l_matrixStack)), spin);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = static_cast<f32>(l_matrixStack.m[i / 4][i % 4]);
    u32 model = static_cast<u32>(this->mpModel.v);
    for (u32 i = 0; i < 12; ++i)
        at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    f32 radius = static_cast<f32>(l_sightAlphaRange);
    boomerang_setCapsuleGeometry(at<cXyz>(lock + 0x19C), at<cXyz>(ea(&this->old.pos)),
                                 at<cXyz>(ea(&this->current.pos)), radius);
    boomerang_subtractVectorInto(at<cXyz>(lock + 0x1A8), at<cXyz>(lock + 0x19C),
                                 at<cXyz>(lock + 0x100));
    play = ea(dComIfGp_get());
    if (!static_cast<u8>(at<boomerangGameInfo_c>(play)->play.mEvtCtrl.mMode)) {
        spin = static_cast<s16>(this->mModelRotY);
        at<daBoomerang_blur_c>(ea(at<void>(ea(&this->mBlur))))
            ->copyBlur(at<void>(ea(&l_matrixStack)), spin);
        play = ea(dComIfGp_get());
        boomerang_setCollision(at<void>(play + 0x26A4), at<void>(lock + 0x84));
        play = ea(dComIfGp_get());
        boomerang_setMassCollision(at<void>(play + 0x4EF8), at<void>(lock + 0x84), 1);
    } else {
        at<daBoomerang_blur_c>(ea(at<void>(ea(&this->mBlur))))
            ->copyBlur(at<void>(ea(&l_matrixStack)), 0);
        boomerang_resetAttackHit(at<void>(lock + 0x84));
    }
    u8 water = static_cast<u8>(this->mInWater);
    this->mInWater = boomerang_setItemWaterEffect(this, water, 1);
    return 1;
}

/* 020CC328 */
BOOL daBoomerang_c::execute() {
    ea(dComIfGp_get());
    ea(dComIfGp_get());
    for (u32 i = 0; i < static_cast<u8>(this->mLockCnt); ++i) {
        Local<be<u32>> id;
        *id = static_cast<u32>(this->mLockActorIDs[i]);
        u32 found = 0;
        if (static_cast<u32>(*id) != 0xFFFFFFFF)
            found = boomerang_judgeActor(boomerangTUJudgeByID, id.get());
        this->mLockActors[i] = at<fopAc_ac_c>(found);
    }
    u32 a = ea(this);
    s16 selector = static_cast<s16>(this->mProcSelector);
    if (selector) {
        u32 receiver = a + static_cast<s16>(static_cast<s16>(this->mProcAdjust));
        u32 target;
        if (selector < 0)
            target = static_cast<u32>(this->mProcTarget);
        else {
            s16 tableOffset = static_cast<s16>(this->mProcVtableOffset);
            u32 table =
                static_cast<u32>(at<gptr<boomerangVirtualMethod_c>>(receiver + tableOffset)->v);
            target = static_cast<u32>(
                at<boomerangVirtualMethod_c>(table)[static_cast<u32>(selector)].mFunction);
        }
        call_ptr<void>(target, at<void>(receiver));
    }
    u32 y = static_cast<u32>(boomerangBits<u32>(this->current.pos.y)),
        z = static_cast<u32>(boomerangBits<u32>(this->current.pos.z));
    boomerangBits<u32>(this->eyePos.y) = y;
    u32 x = static_cast<u32>(boomerangBits<u32>(this->current.pos.x));
    boomerangBits<u32>(this->eyePos.z) = z;
    boomerangBits<u32>(this->attention_info.position.x) = x;
    boomerangBits<u32>(this->attention_info.position.y) = y;
    boomerangBits<u32>(this->eyePos.x) = x;
    boomerangBits<u32>(this->attention_info.position.z) = z;
    this->setRoomInfo();
    at<daBoomerang_sightPacket_c>(ea(at<daBoomerang_sightPacket_c>(a + 0x3B0)))
        ->play(this->mLockCnt);
    return 1;
}

/* 020CC454 */
BOOL daBoomerang_Execute(daBoomerang_c *actor) { return actor->execute(); }

/* 020CC458 */
BOOL daBoomerang_IsDelete(daBoomerang_c *actor) { return 1; }

/* 020CC460 */
BOOL daBoomerang_Delete(daBoomerang_c *actor) { return 1; }

/* 020CC468 */
BOOL daBoomerang_c::createHeap() {
    Local<SafeString> resource;
    resource->__vtbl = ea(&l_safeStringVtable);
    resource->mStringTop = guest_addr("Link");
    u32 resourceManager = static_cast<u32>(l_resourceControl);
    u32 data = boomerang_getArchiveResource(at<void>(resourceManager), resource.get(), 0x14);
    if (!data)
        boomerang_showAssert(at<void>(guest_addr("d_a_boomerang.cpp")), 0x7BE,
                             at<void>(guest_addr("modelData != (0)")));
    u32 model = boomerang_createModel(at<void>(data), 0x80000, 0x37220202);
    this->mpModel.v = model;
    if (!model)
        return 0;
    if (boomerangStageEquals(guest_addr("GanonK")))
        boomerang_setKeepActor(at<void>(ea(&this->mKeep)), 0);
    return 1;
}

/* 020CC5E8 */
BOOL daBoomerang_CreateHeap(daBoomerang_c *actor) { return actor->createHeap(); }

/* 020CD368 */
BOOL daBoomerang_c::create() {
    u32 a = ea(this);
    if (!(static_cast<u32>(this->actor_condition) & 8)) {
        if (a) {
            boomerang_constructActor(this);
            this->__vtbl = ea(&l_actorVtable);
            boomerang_constructDrawList(at<void>(ea(&this->mSightPacket)));
            at<daBoomerang_c>(a)->mSightPacket.mVtable.v = ea(&l_sightVtable);
            boomerangConstructTrailArray(a + 0x4C0, 2,
                                         guest_fn((const void *)boomerangTrailPointCtor));
            boomerang_constructIndexBuffer(at<void>(ea(&this->mSightPacket.mIndexBuffer)));
            boomerang_constructArray(at<void>(ea(&this->mSightPacket.mRenderObjects)), 5, 0x41C,
                                     guest_fn((const void *)boomerangRenderObjectCtor));
            boomerang_constructTexture(at<void>(ea(&this->mSightPacket.mTextureSurface)));
            boomerang_constructSampler(at<void>(ea(&this->mSightPacket.mSampler)));
            boomerang_constructPacket(at<void>(ea(&this->mBlur)));
            this->mBlur.mVtable = ea(&l_blurVtable);
            boomerangConstructTrailArray(a + 0x2C40, 120,
                                         guest_fn((const void *)boomerangBlurPointCtor));
            boomerangConstructTrailArray(a + 0x143B8, 120,
                                         guest_fn((const void *)boomerangBlurPointCtor));
            boomerangConstructRenderObject(a + 0x25B30);
            boomerang_constructIndexBuffer(at<void>(ea(&this->mBlur.mIndexBuffer)));
            boomerang_constructSampler(at<void>(ea(&this->mBlur.mSampler)));
            boomerang_constructTexture(at<void>(ea(&this->mBlur.mTextureSurface)));
            boomerang_constructKeepActor(at<void>(ea(&this->mKeep)));
            u32 status = a + 0x2628C;
            boomerang_constructCollisionStatus(at<void>(status));
            boomerang_constructGlobalCollisionStatus(at<void>(status + 0x1C));
            this->mStts.__vtbl = ea(&l_collisionStatusVtable);
            this->mStts.__vtbl_gstts = ea(&l_globalCollisionStatusVtable);
            u32 capsule = a + 0x262C8;
            boomerang_constructCollisionObject(at<void>(capsule));
            at<boomerangCapsuleAttribute_c>(ea(&this->mCps.mShapeAttr))->mShapeVtable =
                ea(&l_capsuleShapeVtable);
            at<boomerangCapsuleAttribute_c>(ea(&this->mCps.mShapeAttr))->mObjectVtable =
                ea(&l_capsuleObjectVtable);
            boomerang_constructCapsuleGeometry(at<void>(capsule + 0x118));
            at<boomerangCapsuleGeometry_c>(ea(&this->mCps.mCps))->mVtable =
                ea(&l_capsuleGeometryVtable);
            this->mCps.__vtbl_hitinf = ea(&l_collisionHitVtable);
            at<boomerangCapsuleAttribute_c>(ea(&this->mCps.mShapeAttr))->mShapeVtable =
                ea(&l_capsuleAttributeVtable);
            u32 line = a + 0x26400;
            boomerang_constructLineCheck(at<void>(line));
            this->mLinChk.mbLinkThrough = 0;
            this->mLinChk.mbBombThrough = 0;
            this->mLinChk.mbObjThrough = 0;
            this->mLinChk.cBgS_PolyInfo::__vtbl = ea(&l_linePolyVtable);
            this->mLinChk.mbArrowThrough = 0;
            this->mLinChk.mbCamThrough = 0;
            this->mLinChk.mGrp = 1;
            this->mLinChk.mbRopeThrough = 0;
            this->mLinChk.mPolyPassChk = line + 0x58;
            this->mLinChk.dBgS_GrpPassChk::__vtbl = ea(&l_lineGroupVtable);
            this->mLinChk.dBgS_PolyPassChk::__vtbl = ea(&l_linePassVtable);
            this->mLinChk.mGrpPassChk = line + 0x64;
            this->mLinChk.cBgS_Chk::__vtbl = ea(&l_lineCheckVtable);
            this->mLinChk.mbBoomerangThrough = 1;
            u32 ground = a + 0x2646C;
            boomerang_constructGroundCheck(at<void>(ground));
            this->mGndChk.mbArrowThrough = 0;
            this->mGndChk.mGrp = 1;
            this->mGndChk.mbLinkThrough = 0;
            this->mGndChk.mbObjThrough = 1;
            this->mGndChk.mbBombThrough = 0;
            this->mGndChk.mPolyPassChk = ground + 0x40;
            this->mGndChk.mGrpPassChk = ground + 0x4C;
            this->mGndChk.cBgS_Chk::__vtbl = ea(&l_groundCheckVtable);
            this->mGndChk.dBgS_GrpPassChk::__vtbl = ea(&l_groundGroupVtable);
            this->mGndChk.dBgS_PolyPassChk::__vtbl = ea(&l_groundPassVtable);
            this->mGndChk.mbRopeThrough = 0;
            this->mGndChk.cBgS_PolyInfo::__vtbl = ea(&l_groundPolyVtable);
            this->mGndChk.mbBoomerangThrough = 0;
            this->mGndChk.mbCamThrough = 0;
        }
        this->actor_condition = static_cast<u32>(this->actor_condition) | 8;
    }
    boomerang_setupSight(at<void>(ea(&this->mSightPacket)));
    boomerang_setupBlur(at<void>(ea(&this->mBlur)));
    if (!boomerang_entrySolidHeap(this, guest_fn((const void *)daBoomerang_CreateHeap), 0xD40))
        return 5;
    this->setKeepMatrix();
    u32 model = static_cast<u32>(this->mpModel.v);
    this->cullMtx = model ? model + 0xC8 : 0;
    this->mProcAdjust = 0;
    this->mProcSelector = -1;
    this->mProcTarget = boomerangTU.procWait;
    boomerang_initCollisionStatus(at<void>(ea(&this->mStts)), 0x3C, 0xFF, this);
    boomerang_setCollisionCapsule(at<void>(ea(&this->mCps)), at<void>(ea(&l_attackCapsuleSource)));
    this->mCps.mStts.v = a + 0x2628C;
    this->mCps.mGObjAt.mHitCallback = guest_fn((const void *)boomerangRockLineThunk);
    this->mLinChk.mFlag = static_cast<u32>(this->mLinChk.mFlag) & ~0x20000000u;
    for (u32 i = 0; i < 5; ++i)
        this->mLockActorIDs[i] = 0xFFFFFFFF;
    this->setRoomInfo();
    this->model = static_cast<u32>(this->mpModel.v);
    return 4;
}

/* 020CDAA4 */
BOOL daBoomerang_Create(daBoomerang_c *actor) { return actor->create(); }

// HD: compiler-generated and template code of this translation unit.

/* 020CF98C */
void boomerangEmptyDestructor(void *object, s32 flags) {
    if (object && (flags & 1))
        boomerang_deallocate(object);
}

/* 020D0A5C */
void boomerangStringAssureTermination(void *string) {}

/* 020CF9A0 */
void *boomerangTrailPointCtor(void *object) {
    if (!object)
        object = boomerang_allocate(0x254);
    if (object) {
        boomerang_constructVertexBuffer(at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mVertexBuffer)));
        boomerang_constructVertexDeclaration(
            at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mDeclaration)));
        at<boomerangRenderPoint_c>(ea(object))->mAllocation.v = 0;
        at<boomerangRenderPoint_c>(ea(object))->mAllocationBytes = 0;
    }
    return object;
}

/* 020CF9FC */
void *boomerangSmallCtor(void *object) {
    if (!object)
        object = boomerang_allocate(0x10);
    return object;
}

/* 020CFCEC */
void *boomerangBlurPointCtor(void *object) {
    if (!object)
        object = boomerang_allocate(0x254);
    if (object) {
        boomerang_constructVertexBuffer(at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mVertexBuffer)));
        boomerang_constructVertexDeclaration(
            at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mDeclaration)));
        at<boomerangRenderPoint_c>(ea(object))->mAllocation.v = 0;
        at<boomerangRenderPoint_c>(ea(object))->mAllocationBytes = 0;
    }
    return object;
}

/* 020CFD48 */
void boomerangRenderObjectDtor(void *object, s32 flags) {
    if (!object)
        return;
    boomerang_destroyUniformTransform(
        at<void>(ea(&at<boomerangRenderObject_c>(ea(object))->mLighting)), 0);
    boomerang_destroyUniformTransform(
        at<void>(ea(&at<boomerangRenderObject_c>(ea(object))->mTransform)), 0);
    boomerang_destroyRenderObject(object, 2);
    if (flags & 1)
        boomerang_deallocate(object);
}

/* 020CFDB4 */
void boomerangTrailPointDtor(void *object, s32 flags) {
    if (!object)
        return;
    boomerang_destroyVertexDeclaration(
        at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mDeclaration)), 2);
    boomerang_destroyVertexBuffer(at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mVertexBuffer)), 2);
    if (flags & 1)
        boomerang_deallocate(object);
}

/* 020CFE14 */
void boomerangBlurPointDtor(void *object, s32 flags) {
    if (!object)
        return;
    boomerang_destroyVertexDeclaration(
        at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mDeclaration)), 2);
    boomerang_destroyVertexBuffer(at<void>(ea(&at<boomerangRenderPoint_c>(ea(object))->mVertexBuffer)), 2);
    if (flags & 1)
        boomerang_deallocate(object);
}

/* 020CDAA8 */
void boomerangColorToFloat(void *destination, void *color) {
    u32 c = ea(color), d = ea(destination);
    u8 red = static_cast<u8>(at<boomerangColor_c>(c)->r),
       green = static_cast<u8>(at<boomerangColor_c>(c)->g),
       blue = static_cast<u8>(at<boomerangColor_c>(c)->b),
       alpha = static_cast<u8>(at<boomerangColor_c>(c)->a);
    f32 divisor = static_cast<f32>(l_colorDivisor);
    at<boomerangVector4_c>(d)->mValues[0] = static_cast<f32>(red) / divisor;
    at<boomerangVector4_c>(d)->mValues[1] = static_cast<f32>(green) / divisor;
    at<boomerangVector4_c>(d)->mValues[2] = static_cast<f32>(blue) / divisor;
    at<boomerangVector4_c>(d)->mValues[3] = static_cast<f32>(alpha) / divisor;
}

/* 020CDB5C */
void boomerangCopyMatrix(void *destination, void *source) {
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = static_cast<f32>(at<Mtx34>(ea(source))->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; ++i)
        at<Mtx34>(ea(destination))->m[i / 4][i % 4] = matrix[i];
}

/* 020CF8B0 */
void __sinit_d_a_boomerang_cpp() {
    l_headerWord1 = 0;
    l_headerWord3 = 0;
    l_headerWord0 = 0;
    l_headerWord2 = 0;
    boomerang_registerGlobalObject(at<void>(ea(&l_angleRegistration)));
    f32 low = static_cast<f32>(l_minAngle), high = static_cast<f32>(l_maxAngle);
    l_minRegisteredAngle = low;
    l_maxRegisteredAngle = high;
    boomerang_initializeHeaderByte(at<void>(ea(&l_headerByte)));
    boomerang_registerGlobalObject(at<void>(ea(&l_headerByteRegistration)));
    boomerang_initializeHeaderFlag(at<void>(ea(&l_headerFlag)));
    boomerang_registerGlobalObject(at<void>(ea(&l_headerFlagRegistration)));
    f32 zero = static_cast<f32>(l_zero), top = static_cast<f32>(l_turnAcceleration),
        root = static_cast<f32>(l_blurRootY), depth = static_cast<f32>(l_sightLineRadius);
    l_blurTop.x = top;
    l_blurRoot.x = root;
    l_blurRoot.y = zero;
    l_blurRoot.z = zero;
    l_blurTop.y = zero;
    l_capsuleRadius = depth;
    l_blurTop.z = zero;
}

/* 020CFA28 */
void *boomerangRenderObjectCtor(void *object) {
    if (!object)
        object = boomerang_allocate(0x41C);
    if (object)
        boomerangConstructRenderObject(ea(object));
    return object;
}

/* 020CC5EC */
void daBoomerang_sightPacket_c::setup() {
    u32 a = ea(this), resource = boomerangFindRenderResource(guest_addr("default_sight"));
    this->mResource.v = resource;
    this->mVertexDeclaration[0] = 0x11;
    this->mVertexDeclaration[2] = ea(&l_sightVertexDeclaration);
    u32 points = a + 0x110;
    for (u32 i = 0; i < 2; ++i) {
        u32 point = points + i * 0x254;
        u32 vertices = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mVertices.v);
        if (!vertices) {
            u32 allocator = static_cast<u32>(l_heapManager);
            u32 heap = boomerang_getCurrentHeap(at<void>(allocator));
            u32 vtable = static_cast<u32>(at<boomerangHeap_c>(heap)->mpVtable.v);
            u32 buffer =
                call_ptr<u32>(static_cast<u32>(at<boomerangHeapVtable_c>(vtable)->mAllocate.v),
                              at<void>(heap), 0x50, 0x40);
            if (buffer) {
                at<boomerangRenderPoint_c>(point)->mAllocation.v = buffer;
                at<boomerangRenderPoint_c>(point)->mAllocationBytes = 4;
            }
            vertices = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mAllocation.v);
            at<boomerangRenderPoint_c>(point)->mVertices.v = vertices;
        }
        boomerang_setupVertexBuffer(at<void>(point + 4), at<void>(vertices), 4,
                                    at<void>(points + 0x4AC));
        if (resource &&
            resource != static_cast<u32>(at<boomerangSightBuffers_c>(points)->mPreviousResource.v))
            boomerang_setupVertexDeclaration(at<void>(resource), at<void>(point + 0x158),
                                             at<void>(point + 4), at<void>(points + 0x4AC), 0);
    }
    at<boomerangSightBuffers_c>(points)->mPreviousResource.v = resource;
    at<boomerangSightBuffers_c>(points)->mInitialized = 1;
    for (u32 i = 0; i < 5; ++i)
        boomerang_initializeRenderObject(at<void>(a + 0x5F0 + 0x41C * i), 1, 0);
    this->mIndices[0] = 0;
    this->mIndices[1] = 2;
    this->mIndices[2] = 1;
    this->mIndices[3] = 3;
    boomerang_setupIndexBuffer(at<void>(ea(&this->mIndexBuffer)), at<void>(ea(&this->mIndices)), 4,
                               4);
    this->mIndexBuffer.mCount = 6;
    u32 index = static_cast<u32>(at<boomerangSightBuffers_c>(points)->mWriteBuffer);
    u32 buffer = static_cast<u32>(at<boomerangRenderPoint_c>(points)[index].mVertices.v);
    boomerangZeroCacheRange(buffer, 0x40);
    index = static_cast<u32>(this->mWriteBuffer);
    buffer = static_cast<u32>(at<boomerangRenderPoint_c>(points)[index].mVertices.v);
    f32 zero = static_cast<f32>(l_zero), one = static_cast<f32>(l_one),
        minus = static_cast<f32>(l_minusOne);
    const u16 order[] = {0x4C, 0x40, 0,    0x2C, 0x28, 0x1C, 0x20, 0xC,  0x38, 0x30,
                         8,    0x34, 0x48, 0x10, 4,    0x3C, 0x14, 0x18, 0x44, 0x24};
    for (u16 offset : order) {
        f32 value = (offset == 0x40 || offset == 0 || offset == 0x2C || offset == 0x28) ? minus
                    : (offset == 0x4C || offset == 0x20 || offset == 0x38 || offset == 0x48 ||
                       offset == 4 || offset == 0x3C || offset == 0x14 || offset == 0x18)
                        ? one
                        : zero;
        at<boomerangQuad_c>(buffer)->mValues[offset / 4] = value;
    }
    index = static_cast<u32>(at<boomerangSightBuffers_c>(points)->mWriteBuffer);
    u32 point = points + index * 0x254;
    u32 storage = static_cast<u32>(at<boomerangRenderPoint_c>(point)->mVertexBuffer.mUploadSource);
    boomerang_uploadVertexBuffer(at<void>(point + 4), 0, at<void>(storage));
    index = static_cast<u32>(at<boomerangSightBuffers_c>(points)->mWriteBuffer);
    at<boomerangSightBuffers_c>(points)->mWriteBuffer = index == 0;
    u32 graphics = static_cast<u32>(l_graphicsContext);
    boomerang_beginGraphics(at<void>(graphics));
    Local<SafeString> textureName;
    u32 manager = static_cast<u32>(l_resourceControl);
    textureName->__vtbl = ea(&l_safeStringVtable);
    textureName->mStringTop = guest_addr("Link");
    u32 texture = boomerang_getArchiveResource(at<void>(manager), textureName.get(), 0x72);
    if (!texture)
        boomerang_showAssert(at<void>(guest_addr("d_a_boomerang.cpp")), 0x2DB,
                             at<void>(guest_addr("tmp_img != (0)")));
    u32 textureData = static_cast<u32>(at<boomerangImage_c>(texture)->mpSurface.v);
    boomerang_initializeTexture(at<void>(ea(&this->mTextureSurface)), at<void>(textureData));
    this->mImage.v = texture;
    graphics = static_cast<u32>(l_graphicsContext);
    boomerang_endGraphics(at<void>(graphics));
    bool equal = true;
    for (u32 offset : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u}) {
        if (static_cast<u32>(this->mSampler.mSurface.mWords[offset / 4]) !=
            static_cast<u32>(this->mTextureSurface.mWords[offset / 4])) {
            equal = false;
            break;
        }
    }
    if (!equal)
        boomerang_copySampler(at<void>(ea(&this->mSampler)), at<void>(ea(&this->mTextureSurface)));
    else {
        u32 data = static_cast<u32>(this->mTextureSurface.mImage),
            size = static_cast<u32>(this->mTextureSurface.mMipmap);
        this->mSampler.mSurface.mImage = data;
        this->mSampler.mCachedMipmap = size;
        this->mSampler.mSurface.mMipmap = size;
        this->mSampler.mCachedImage = data;
    }
    u8 flags = static_cast<u8>(this->mSampler.mFlags);
    this->mSampler.mMagFilter = 2;
    this->mSampler.mMinFilter = 2;
    this->mSampler.mMipFilter = 2;
    this->mSampler.mFlags = flags | 2;
}

/* 020CFE74 */
void boomerangBlurDestructor(void *object, s32 flags) {
    if (!object)
        return;
    boomerangDestroyBlurContents(ea(object));
    if (flags & 1)
        boomerang_deallocate(object);
}

/* 020D02FC */
void boomerangDestructor(daBoomerang_c *actor, s32 flags) {
    if (!actor)
        return;
    u32 a = ea(actor), render = a + 0x23ADC;
    actor->mGndChk.dBgS_PolyPassChk::__vtbl = ea(&l_deletingGroundGroupVtable);
    actor->mGndChk.dBgS_GrpPassChk::__vtbl = ea(&l_deletingGroupVtable);
    actor->mGndChk.cBgS_PolyInfo::__vtbl = ea(&l_deletingGroundPassVtable);
    boomerang_destroyBackgroundCheck(at<void>(render + 0x2990), 0);
    actor->mLinChk.cBgS_PolyInfo::__vtbl = ea(&l_deletingLineCheckVtable);
    actor->mLinChk.dBgS_PolyPassChk::__vtbl = ea(&l_deletingLinePassVtable);
    actor->mLinChk.dBgS_GrpPassChk::__vtbl = ea(&l_deletingGroupVtable);
    boomerang_destroyPolyInfo(at<void>(render + 0x2924), 0);
    boomerang_destroyCollisionObject(at<void>(render + 0x27EC), 2);
    boomerang_destroyCollisionStatus(at<void>(render + 0x27B0), 2);
    boomerang_destroyKeepActor(at<void>(render + 0x26B8), 2);
    boomerangDestroyBlurContents(a + 0x2054);
    if (a + 0x3B0) {
        at<daBoomerang_c>(a)->mSightPacket.mVtable.v = ea(&l_sightVtable);
        u32 points = a + 0x4C0;
        boomerangReleaseBlurPoint(points);
        boomerangReleaseBlurPoint(points + 0x254);
        at<boomerangSightBuffers_c>(points)->mPreviousResource.v = 0;
        for (u32 i = 0; i < 5; ++i)
            boomerangDestroyRenderBuffers(a + 0x9A0 + i * 0x41C);
        boomerang_destroySampler(at<void>(a + 0x1EBC), 2);
        boomerang_destroyArray(at<void>(a + 0x9A0), 5, 0x41C,
                               guest_fn((const void *)boomerangRenderObjectDtor), 0, 0);
        boomerang_destroyIndexBuffer(at<void>(a + 0x980), 2);
        if (points) {
            boomerangReleaseBlurPoint(points);
            boomerangReleaseBlurPoint(points + 0x254);
            at<boomerangSightBuffers_c>(points)->mPreviousResource.v = 0;
            boomerang_destroyArray(at<void>(points), 2, 0x254,
                                   guest_fn((const void *)boomerangTrailPointDtor), 0, 0);
        }
        boomerang_destroyDrawList(at<void>(a + 0x3B0), 0);
    }
    boomerang_destroyActor(actor, 0);
    if (flags & 1)
        boomerang_deallocate_daBoomerang_cPtr(actor);
}

/* 020CCA50 */
void daBoomerang_blur_c::setup() {
    u32 a = ea(this), resource = boomerangFindRenderResource(guest_addr("blur_default"));
    this->mResource.v = resource;
    boomerangSetupBlurRing(a + 0xBEC, static_cast<u32>(this->mResource.v));
    boomerangSetupBlurRing(a + 0x12364, static_cast<u32>(this->mResource.v));
    boomerang_initializeRenderObject(at<void>(ea(&this->mRenderObject)), 1, 0);
    for (u32 i = 0; i < 4; ++i)
        this->mIndices[i] = static_cast<u16>(i);
    boomerang_setupIndexBuffer(at<void>(ea(&this->mIndexBuffer)), at<void>(ea(&this->mIndices)), 4,
                               4);
    f32 zero = static_cast<f32>(l_zero), one = static_cast<f32>(l_one);
    this->mIndexBuffer.mCount = 6;
    boomerangInitializeBlurRing(a + 0xBEC, zero, one);
    boomerangInitializeBlurRing(a + 0x12364, zero, one);
    u32 graphics = static_cast<u32>(l_graphicsContext);
    boomerang_beginGraphics(at<void>(graphics));
    Local<SafeString> name;
    name->__vtbl = ea(&l_safeStringVtable);
    u32 manager = static_cast<u32>(l_resourceControl);
    name->mStringTop = guest_addr("Link");
    u32 texture = boomerang_getArchiveResource(at<void>(manager), name.get(), 0x70);
    if (!texture)
        boomerang_showAssert(at<void>(guest_addr("d_a_boomerang.cpp")), 0xFA,
                             at<void>(guest_addr("tmp_img != (0)")));
    u32 data = static_cast<u32>(at<boomerangImage_c>(texture)->mpSurface.v);
    boomerang_initializeTexture(at<void>(ea(&this->mTextureSurface)), at<void>(data));
    graphics = static_cast<u32>(l_graphicsContext);
    boomerang_endGraphics(at<void>(graphics));
    u32 original = a + 0x240B0, copy = a + 0x23F18;
    bool equal = true;
    for (u32 offset : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u}) {
        if (static_cast<u32>(at<boomerangSampler_c>(copy)->mSurface.mWords[offset / 4]) !=
            static_cast<u32>(at<boomerangTextureSurface_c>(original)->mWords[offset / 4])) {
            equal = false;
            break;
        }
    }
    if (!equal)
        boomerang_copySampler(at<void>(copy), at<void>(original));
    else {
        data = static_cast<u32>(at<boomerangTextureSurface_c>(original)->mImage);
        u32 size = static_cast<u32>(at<boomerangTextureSurface_c>(original)->mMipmap);
        at<boomerangSampler_c>(copy)->mSurface.mImage = data;
        at<boomerangSampler_c>(copy)->mCachedMipmap = size;
        at<boomerangSampler_c>(copy)->mSurface.mMipmap = size;
        at<boomerangSampler_c>(copy)->mCachedImage = data;
    }
    u8 flags = static_cast<u8>(at<boomerangSampler_c>(copy)->mFlags);
    at<boomerangSampler_c>(copy)->mMagFilter = 2;
    at<boomerangSampler_c>(copy)->mMinFilter = 2;
    at<boomerangSampler_c>(copy)->mMipFilter = 2;
    at<boomerangSampler_c>(copy)->mFlags = flags | 2;
}
/* WWHD glue
ext 025E1988 void boomerang_startLockSound(u32 a0)
ext 0273AF40 void boomerang_deallocate(void* a0)
ext 0273AD10 void* boomerang_allocate(s32 a0)
ext 027B5BD8 void boomerang_constructVertexBuffer(void* a0)
ext 027BF734 void boomerang_constructVertexDeclaration(void* a0)
ext 02008974 f32 boomerang_groundCross(void* a0, void* a1)
ext 024EF130 u8 boomerang_getRoomId(void* a0, void* a1)
ext 024EEEB8 u8 boomerang_getPolyColor(void* a0, void* a1)
ext 025D5218 u32 boomerang_judgeActor(s32 a0, gabi::be<unsigned int>* a1)
ext 027FB528 void boomerang_destroyUniformTransform(void* a0, s32 a1)
ext 027FD764 void boomerang_destroyRenderObject(void* a0, s32 a1)
ext 027BF880 void boomerang_destroyVertexDeclaration(void* a0, s32 a1)
ext 027B5CBC void boomerang_destroyVertexBuffer(void* a0, s32 a1)
ext 028E90D4 void boomerang_copyMatrix(void* a0, void* a1)
ext 025F24E0 void boomerang_translateMatrix(f32 a0, f32 a1, f32 a2)
ext 025F19F8 void boomerang_rotateXYZ(void* a0, s16 a1, s16 a2, s16 a3)
ext 026066C4 u32 boomerang_getArchiveResource(void* a0, void* a1, s32 a2)
ext 0273AA24 void boomerang_showAssert(void* a0, s32 a1, void* a2)
ext 025E38E0 u32 boomerang_createModel(void* a0, s32 a1, s32 a2)
ext 0207FD38 void boomerang_setKeepActor(void* a0, s32 a1)
ext 024F1AFC void boomerang_setLineCheck(void* a0, cXyz* a1, cXyz* a2, daBoomerang_c* a3)
ext 02008860 u32 boomerang_lineCross(void* a0, void* a1)
ext 025A847C void boomerang_setParticle(void* a0, s32 a1, s32 a2, cXyz* a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 024EECAC u32 boomerang_getMaterialSound(void* a0, void* a1)
ext 02520540 s32 boomerang_getReverbRegister(s32 a0)
ext 025E1A40 void boomerang_startSound(s32 a0, cXyz* a1, u32 a2, s32 a3)
ext 028E8F64 void boomerang_multiplyMatrixVector(void* a0, void* a1, void* a2)
ext 025F1C28 void boomerang_rotateY(void* a0, s16 a1)
ext 025F1084 void boomerang_getViewport(void* a0)
ext 025F1018 void boomerang_projectPosition(cXyz* a0, cXyz* a1, void* a2)
ext 028E93CC void boomerang_translateMatrixInto(void* a0, f32 a1, f32 a2, f32 a3)
ext 025F2518 void boomerang_scaleMatrix(f32 a0, f32 a1, f32 a2)
ext 025F1C5C void boomerang_rotateZ(void* a0, s16 a1)
ext 0252CDC0 void boomerang_entrySight(void* a0, void* a1, void* a2, void* a3)
ext 0252F3B0 void boomerang_entryBlur(void* a0, void* a1, void* a2, cXyz* a3)
ext 02555D0C u32 boomerang_getEnvironmentLight()
ext 025626A4 void boomerang_settingTevStruct(void* a0, s32 a1, cXyz* a2, void* a3)
ext 02562F5C void boomerang_setLightTevColorType(void* a0, void* a1, void* a2)
ext 02591200 void boomerang_updateIceModel(void* a0, s32 a1, s32 a2)
ext 025E2DE0 void boomerang_modelUpdateDL(void* a0, s32 a1)
ext 023D9340 void boomerang_updateKeepMatrix(void* a0, void* a1, void* a2)
ext 028F026C void boomerang_registerGlobalObject(void* a0)
ext 028ED6F8 void boomerang_initializeHeaderByte(void* a0)
ext 028EAB2C void boomerang_initializeHeaderFlag(void* a0)
ext 02516138 void boomerang_resetAttackHit(void* a0)
ext 0201ADE0 void boomerang_subtractVector(cXyz* a0, cXyz* a1, cXyz* a2)
ext 028E8DD0 f32 boomerang_squareMagnitude(cXyz* a0)
ext 028F4384 f32 boomerang_squareRoot(f32 a0)
ext 020195B0 s16 boomerang_atan2s(f32 a0, f32 a1)
ext 020CEE10 void boomerang_moveFlight(daBoomerang_c* a0)
ext 024F8044 u32 boomerang_getCameraBody()
ext 020181B0 void boomerang_setCapsuleGeometry(cXyz* a0, cXyz* a1, cXyz* a2, f32 a3)
ext 028E8DAC void boomerang_subtractVectorInto(cXyz* a0, cXyz* a1, cXyz* a2)
ext 0200E240 void boomerang_setCollision(void* a0, void* a1)
ext 025F23EC void boomerang_pushMatrix()
ext 0201AE48 void boomerang_scaleVector(cXyz* a0, cXyz* a1, f32 a2)
ext 028E8D88 void boomerang_addVector(cXyz* a0, cXyz* a1, cXyz* a2)
ext 025F2468 void boomerang_popMatrix()
ext 027FD6F4 void boomerang_constructRenderObject(void* a0)
ext 027FB40C void boomerang_constructUniformTransform(void* a0)
ext 028F521C void boomerang_clearMemory(void* a0, s32 a1)
ext 0273AD10 void boomerang_allocate_noResult(s32 a0)
ext 028EFFD0 void boomerang_constructArray(void* a0, s32 a1, s32 a2, s32 a3)
ext 025D57E0 void boomerang_deleteActor(daBoomerang_c* a0)
ext 025E1A40 void boomerang_startSound_s32_cXyzPtr_s32_s32(s32 a0, cXyz* a1, s32 a2, s32 a3)
ext 0201AEAC void boomerang_divideVector(cXyz* a0, cXyz* a1, f32 a2)
ext 02442ED8 u32 boomerang_returnBoomerang(void* a0)
ext 025160DC u32 boomerang_checkAttackHit(void* a0)
ext 02515BBC u32 boomerang_getHitActor(void* a0)
ext 0200F378 void boomerang_addCalcAngleS(gabi::be<short>* a0, s16 a1, s32 a2, s32 a3, s32 a4)
ext 025F1B48 void boomerang_rotateZXY(void* a0, s16 a1, s16 a2, s16 a3)
ext 02516C14 void boomerang_setMassCollision(void* a0, void* a1, s32 a2)
ext 02443208 u8 boomerang_setItemWaterEffect(daBoomerang_c* a0, u8 a1, s32 a2)
ext 0273AD10 u32 boomerang_allocate_u32(u32 a0)
ext 028EFFD0 void boomerang_constructArray_voidPtr_u32_s32_u32(void* a0, u32 a1, s32 a2, u32 a3)
ext 025D4ED0 void boomerang_constructActor(daBoomerang_c* a0)
ext 0252CCBC void boomerang_constructDrawList(void* a0)
ext 027B5430 void boomerang_constructIndexBuffer(void* a0)
ext 027BE6B8 void boomerang_constructTexture(void* a0)
ext 027BDF7C void boomerang_constructSampler(void* a0)
ext 027F1278 void boomerang_constructPacket(void* a0)
ext 02080404 void boomerang_constructKeepActor(void* a0)
ext 0200BD2C void boomerang_constructCollisionStatus(void* a0)
ext 02515DA0 void boomerang_constructGlobalCollisionStatus(void* a0)
ext 02515FB8 void boomerang_constructCollisionObject(void* a0)
ext 02018150 void boomerang_constructCapsuleGeometry(void* a0)
ext 02008FEC void boomerang_constructLineCheck(void* a0)
ext 02008E0C void boomerang_constructGroundCheck(void* a0)
ext 020CC5EC void boomerang_setupSight(void* a0)
ext 020CCA50 void boomerang_setupBlur(void* a0)
ext 025D63E8 u32 boomerang_entrySolidHeap(daBoomerang_c* a0, s32 a1, s32 a2)
ext 02515F14 void boomerang_initCollisionStatus(void* a0, s32 a1, s32 a2, daBoomerang_c* a3)
ext 025164C0 void boomerang_setCollisionCapsule(void* a0, void* a1)
ext 027FFCBC u32 boomerang_getMaterialManager()
ext 027B90AC s32 boomerang_findResourceIndex(void* a0, void* a1)
ext 02800B0C void boomerang_initializeMaterialInstance(void* a0, void* a1, s32 a2)
ext 02756140 u32 boomerang_getCurrentHeap(void* a0)
ext 027FF478 void boomerang_setupVertexBuffer(void* a0, void* a1, s32 a2, void* a3)
ext 027FF530 void boomerang_setupVertexDeclaration(void* a0, void* a1, void* a2, void* a3, s32 a4)
ext 027FE084 void boomerang_initializeRenderObject(void* a0, s32 a1, s32 a2)
ext 027B54E0 void boomerang_setupIndexBuffer(void* a0, void* a1, s32 a2, s32 a3)
ext 027B5E94 void boomerang_uploadVertexBuffer(void* a0, s32 a1, void* a2)
ext 0274FBF8 void boomerang_beginGraphics(void* a0)
ext 02773798 void boomerang_initializeTexture(void* a0, void* a1)
ext 0274FCCC void boomerang_endGraphics(void* a0)
ext 027BDEB4 void boomerang_copySampler(void* a0, void* a1)
ext 027F29D4 u32 boomerang_getShaderCache(void* a0)
ext 027BB9E0 void boomerang_updateMaterial(void* a0, s32 a1)
ext 027B9F68 void boomerang_bindShader(void* a0)
ext C00060E0 void boomerang_callDisplayList(void* a0, u32 a1)
ext 027BB7CC void boomerang_applyMaterial(void* a0)
ext 02750250 void boomerang_getGraphicsState(void* a0)
ext 02750370 void boomerang_setGraphicsState(void* a0)
ext 028E9098 void boomerang_identityMatrix(void* a0)
ext 0274D80C u32 boomerang_getCameraProjection(void* a0)
ext 028E8970 void boomerang_copyProjection(void* a0, void* a1)
ext 027FDA54 void boomerang_setRenderMatrices(void* a0, s32 a1, void* a2, void* a3, void* a4)
ext 027FDFF4 void boomerang_selectRenderPass(void* a0, s32 a1)
ext C0006900 void boomerang_setPixelUniform(s16 a0, void* a1, u32 a2)
ext C0006A38 void boomerang_setVertexUniform(s16 a0, void* a1, u32 a2)
ext C00068A8 void boomerang_setGeometryUniform(s16 a0, void* a1, u32 a2)
ext 027BE53C void boomerang_bindSampler(void* a0, void* a1, s32 a2, s32 a3)
ext 027FB678 void boomerang_updateUniformTransform(void* a0)
ext 027BFE5C void boomerang_bindVertexDeclaration(void* a0)
ext C0006178 void boomerang_drawIndexed(u32 a0, u32 a1, void* a2, u32 a3, s32 a4, s32 a5)
ext 027BF7E8 void boomerang_releaseVertexDeclaration(void* a0)
ext 02755FEC u32 boomerang_findAllocationHeap(void* a0, void* a1)
ext 027BEBEC void boomerang_releaseUniform(void* a0)
ext 027BE2B0 void boomerang_destroySampler(void* a0, s32 a1)
ext 027B54A0 void boomerang_destroyIndexBuffer(void* a0, s32 a1)
ext 028F0164 void boomerang_destroyArray(void* a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
ext 027F13DC void boomerang_destroyPacket(void* a0, s32 a1)
ext 02008DAC void boomerang_destroyBackgroundCheck(void* a0, s32 a1)
ext 02008B4C void boomerang_destroyPolyInfo(void* a0, s32 a1)
ext 02515980 void boomerang_destroyCollisionObject(void* a0, s32 a1)
ext 02515860 void boomerang_destroyCollisionStatus(void* a0, s32 a1)
ext 02082DDC void boomerang_destroyKeepActor(void* a0, s32 a1)
ext 0252CCFC void boomerang_destroyDrawList(void* a0, s32 a1)
ext 025D50BC void boomerang_destroyActor(daBoomerang_c* a0, s32 a1)
ext 0273AF40 void boomerang_deallocate_daBoomerang_cPtr(daBoomerang_c* a0)
ext 027B5E94 void boomerang_uploadVertexBuffer_voidPtr_s32_u32(void* a0, s32 a1, u32 a2)
data l_viewportHeight 1000AD20
data l_halfAnimLength 1000AE84
data l_one 1000AE88
data l_sightAlphaRange 1000AE98
data l_sightAlphaBase 1000AE9C
data l_sightSizeRange 1000AEA0
data l_sightSizeBase 1000AEA4
data l_sightSizeScale 1000AEA8
data l_sightViewportDivisor 1000AEAC
data l_quarterTurn 1000AEB8
data l_halfTurn 1000AEBC
data l_noGround 1000AED4
data l_minusOne 1000AF10
data l_zero 1000AF14
data l_keepOffsetX 1000AF90
data l_keepOffsetY 1000AF94
data l_keepOffsetZ 1000AF98
data l_colorDivisor 1000AF9C
data l_blurInterpolationStep 1000AFA0
data l_longFlyMax 1000AFA4
data l_normalFlyMax 1000AFA8
data l_normalizeThreshold 1000AFB4
data l_turnFactor 1000AFBC
data l_turnThreshold 1000AFC0
data l_turnAcceleration 1000AFC4
data l_turnScale 1000AFC8
data l_turnBase 1000AFCC
data l_throwSpeed 1000AFD0
data l_sightLineRadius 1000AFD4
data l_minAngle 1000AFD8
data l_maxAngle 1000AFDC
data l_blurRootY 1000AFE0
data l_uniformOne 1014517C
data l_uniformZero 10145180
data l_matrixStack 1048D0CC
data l_viewMatrix 104B45F8
data l_blurTop 10462724
data l_blurRoot 10462730
data l_sinCosTable 104A44F8
data l_lockSounds 1000AEC0
data l_projectionMatrix 104B470C
data l_renderSystem 104B4708
data l_opaqueBuffer 104B4634
data l_translucentBuffer 104B4638
data l_graphicsContext 101F8B18
data l_heapManager 101F8B4C
data l_resourceControl 101F4F28
data l_cameraProjectionOwner 104B465C
data l_shaderCacheOwner 104B45C0
data l_stayRoom 1047E6C8
data l_offsetInitialized 1046273C
data l_minRegisteredAngle 10462708
data l_maxRegisteredAngle 1046270C
data l_headerByte 10462710
data l_headerFlag 10462711
data l_capsuleRadius 10192464
data l_blurColor 10192468
data l_sightUniformColor 104A01EC
data l_attackCapsuleSource 1019241C
data l_angleRegistration 1019246C
data l_headerByteRegistration 10192478
data l_headerFlagRegistration 10192484
data l_headerWord0 10462714
data l_headerWord1 10462718
data l_headerWord2 1046271C
data l_headerWord3 10462720
data l_safeStringVtable 1000AD2C
data l_actorVtable 1000AE74
data l_sightVtable 1000AFFC
data l_blurVtable 1000B014
data l_collisionStatusVtable 1004AE88
data l_globalCollisionStatusVtable 1004AEC0
data l_capsuleShapeVtable 100015A8
data l_capsuleObjectVtable 1000AD44
data l_capsuleGeometryVtable 1004AF60
data l_collisionHitVtable 1004AF18
data l_capsuleAttributeVtable 1004AF70
data l_linePolyVtable 1000AE44
data l_lineGroupVtable 1000AE54
data l_linePassVtable 1000AE64
data l_lineCheckVtable 1000AE34
data l_groundCheckVtable 1000ADB4
data l_groundGroupVtable 1000ADD4
data l_groundPassVtable 1000ADE4
data l_groundPolyVtable 1000ADC4
data l_deletingGroundGroupVtable 1000ADA4
data l_deletingGroupVtable 1000AD64
data l_deletingGroundPassVtable 1000AD84
data l_deletingLineCheckVtable 1000AD54
data l_deletingLinePassVtable 1000AE24
data l_matrixUniformVtable 1016EF84
data l_lightingUniformVtable 1016EFB4
data l_sightVertexDeclaration 1000AFE8
data l_blurVertexDeclaration 1000AFF0
str 1000AF54 "Link" in 020CCA50
str 1000AF5C "d_a_boomerang.cpp" in 020CCA50
str 1000AF70 "blur_default" in 020CCA50
str 1000AF80 "tmp_img != (0)" in 020CCA50
str 1000AF18 "Link" in 020CC5EC
str 1000AF20 "default_sight" in 020CC5EC
str 1000AF30 "d_a_boomerang.cpp" in 020CC5EC
str 1000AF44 "tmp_img != (0)" in 020CC5EC
str 1000AED8 "Link" in 020CC468
str 1000AEE0 "GanonK" in 020CC468
str 1000AEE8 "d_a_boomerang.cpp" in 020CC468
str 1000AEFC "modelData != (0)" in 020CC468
str 1000AFAC "GanonK" in 020CECF0
*/
