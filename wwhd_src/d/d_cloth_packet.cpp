/**
 * d_cloth_packet.cpp - spring cloth simulation and flag packets.
 * Written in GameCube style and verified against the original WWHD functions.
 * HD: GX2 vertex pools, shader materials, uniforms and graphics state replace GX display lists.
 * Guest stack sizes, register widths, float/word copies and call/reload order are retained.
 */
#include "d/d_cloth_packet.h"
static cloth::DestructorRecord lClothDestructor0;
static cloth::DestructorRecord lClothDestructor1;
static cloth::DestructorRecord lClothDestructor2;
static cloth::DestructorRecord lMathDestructor0;
static cloth::DestructorRecord lMathDestructor1;
static cloth::DestructorRecord lMathDestructor2;
static u8 lClothHeaderFloatGuard;
static u8 lClothHeaderByteGuard;
static u8 lMathHeaderFloatGuard;
static u8 lMathHeaderByteGuard;
static cloth::MatrixWords lViewMatrix;
static cloth::MatrixWords lProjectionMatrix;
static cloth::MatrixWords lIdentityMatrix;
static u32 lGraphicsSystem;
static cloth::GraphicsState lDefaultRenderState;
static u32 lPacketVtable;
static u32 lXluPacketVtable;
static u32 lFlag03Vtable;
static u32 lFlag04Vtable;
static u32 lFlag05Vtable;
static u32 lFlag07Vtable;
static u32 lMaterialStringVtable;
static u32 lXluStringVtable;
static u32 lTransformVtable;
static u32 lLightingVtable;
static cloth::SimulationOwner lFlag03Simulation;
static cloth::SimulationOwner lFlag04Simulation;
static cloth::SimulationOwner lFlag05Simulation;
static cloth::SimulationOwner lFlag07Simulation;
static u32 lFrameCounter;
static u32 lOpaqueDrawBuffer;
static u32 lGraphicsMutex;
static u32 lHeapManager;
static u32 lViewResource;
static u32 lIndexFormat;
static u32 lPrimitiveType;
static u32 lIndexData;
static cloth::Vector lZeroVector;
static cloth::Vector lBaseZ;
static cloth::Vector lBaseXYZ;
static cloth::MatrixWords lMatrixStack;
static cloth::SinCosTable lSinCosTable;
static cloth::HeaderStatics lMathHeaderStatics;
static cloth::HeaderStatics lClothHeaderStatics;
static f32 lMathHeaderFirst;
static f32 lMathHeaderSecond;
static f32 lClothHeaderFirst;
static f32 lClothHeaderSecond;
static f32 lFlagTuning100;
static f32 lFlagTuning180;
static f32 lFlagTuning184;
static f32 lFlagTuning188;
static f32 lFlagTuning192;
static f32 lFlagTuning196;
static f32 lFlagTuning200;
static f32 lFlagTuning204;
static f32 lFlagTuning208;
static f32 lFlagTuning212;
static f32 lFlagTuning216;
static f32 lFlagTuning220;
static f32 lFlagTuning224;
static f32 lFlagTuning228;
static f32 lFlagTuning232;
static f32 lFlagTuning236;
static f32 lFlagTuning240;
static f32 lFlagTuning244;

static f32 lTextureDepth;
static f32 lXluDepth;
static f32 lClothTuning2;
static f32 lClothTuning3;
static f32 lZero;
static f32 lColorDivisor;
static f32 lFlagFlyLength;
static f32 lFlagHoistLength;
static f32 lLongFlagFlyLength;
static f32 lLongFlagHoistLength;
static f32 lClothTuning10;
static f32 lClothTuning11;
static f32 lWindWaveAdjust;
static f32 lWindSpeedAdjust;
static f32 lUnit;
static f32 lGraphicsZero;
static f32 lGraphicsOne;
/* 0251B648 */
u32 dCloth_packet_c__ct(dCloth_packet_c* p, cloth::ResourceImage* image, u32 nx, u32 ny,
                        dKy_tevstr_c* lighting, cloth::SuppliedPositions* supplied, f64 width,
                        f64 height) {
    if (!gabi::ea(p)) {
        p = gabi::at<dCloth_packet_c>(clothApi_allocate(u32(0xA40)));
        if (!gabi::ea(p))
            return 0;
    }
    clothApi_constructPacketBase(gabi::ea(p));
    f32 zero = lZero;
    p->mHoistGridSize = 0;
    p->mpTevstr = 0;
    p->mPacket.mpVtable.v = gabi::ea(&lPacketVtable);
    p->mpFactorCheckCB = 0;
    p->mFlyGridSize = 0;
    p->mFlyLength = zero;
    p->mHoistLength = zero;
    clothApi_clearMemory(gabi::ea(p) + 0xB0, u32(8));
    clothApi_clearMemory(gabi::ea(p) + 0xB8, u32(8));
    clothApi_clearMemory(gabi::ea(p) + 0xC0, u32(8));
    p->mpSpeedArr = 0;
    p->mWindSpeedWave = zero;
    p->field_0x64 = 0;
    p->mWindSpeed = zero;
    clothApi_clearMemory(gabi::ea(p) + 0xF0, u32(64));
    clothApi_clearMemory(gabi::ea(p) + 0x130, u32(64));
    clothApi_clearMemory(gabi::ea(p) + 0x170, u32(48));
    p->mSpring = zero;
    p->mGravity = zero;
    p->mWave = 0;
    p->mRipple = 0;
    p->mHoistFlex = zero;
    p->field_0xF2 = 0;
    p->mWaveSpeed = 0;
    p->mMaterialCount = 0;
    p->mFlyFlex = zero;
    p->mDrag = zero;
    p->field_0xF0 = 0;
    p->mCurArr = 0;
    p->mRotateY = 0;
    cloth::ShapeArray* inlineArray = gabi::at<cloth::ShapeArray>(gabi::ea(p) + 0x1C8);
    if (!gabi::ea(inlineArray))
        inlineArray = gabi::at<cloth::ShapeArray>(clothApi_allocate(u32(8)));
    if (gabi::ea(inlineArray)) {
        inlineArray->mpShapes = 0;
        inlineArray->mCount = 0;
    }
    clothApi_constructUniformBlock(gabi::ea(p) + 0x1D4);
    clothApi_constructMaterialBlock(gabi::ea(p) + 0x1E0);
    p->mTransform.mpVtable.v = gabi::ea(&lTransformVtable);
    clothApi_clearMemory(gabi::ea(p) + 0x254, u32(52));
    if (gabi::ea(p) + 0x254 == 0)
        clothApi_allocate(u32(48));
    clothApi_constructMaterialBlock(gabi::ea(p) + 0x288);
    p->mLighting.mpVtable.v = gabi::ea(&lLightingVtable);
    clothApi_clearMemory(gabi::ea(p) + 0x2FC, u32(0x2F0));
    f32 z = lGraphicsZero;
    p->mLighting.mColors[0].mValues[0] = z;
    p->mLighting.mColors[0].mValues[1] = z;
    p->mLighting.mColors[0].mValues[2] = z;
    f32 one = lGraphicsOne;
    p->mLighting.mColors[1].mValues[0] = z;
    p->mLighting.mColors[2].mValues[3] = one;
    p->mLighting.mColors[3].mValues[2] = z;
    p->mLighting.mColors[1].mValues[3] = one;
    p->mLighting.mColors[1].mValues[2] = z;
    p->mLighting.mColors[6].mValues[2] = z;
    p->mLighting.mColors[8].mValues[2] = z;
    p->mLighting.mColors[5].mValues[2] = z;
    p->mLighting.mColors[7].mValues[3] = one;
    p->mLighting.mColors[3].mValues[0] = z;
    p->mLighting.mColors[5].mValues[1] = z;
    p->mLighting.mColors[8].mValues[1] = z;
    p->mLighting.mColors[7].mValues[1] = z;
    p->mLighting.mColors[4].mValues[1] = z;
    p->mLighting.mColors[2].mValues[0] = z;
    p->mLighting.mColors[3].mValues[1] = z;
    p->mLighting.mColors[3].mValues[3] = one;
    p->mLighting.mColors[2].mValues[1] = z;
    p->mLighting.mColors[4].mValues[2] = z;
    p->mLighting.mColors[1].mValues[1] = z;
    p->mLighting.mColors[7].mValues[0] = z;
    p->mLighting.mColors[7].mValues[2] = z;
    p->mLighting.mColors[8].mValues[3] = one;
    p->mLighting.mColors[5].mValues[3] = one;
    p->mLighting.mColors[6].mValues[3] = one;
    p->mLighting.mColors[6].mValues[1] = z;
    p->mLighting.mColors[5].mValues[0] = z;
    p->mLighting.mColors[8].mValues[0] = z;
    p->mLighting.mColors[2].mValues[2] = z;
    p->mLighting.mColors[6].mValues[0] = z;
    p->mLighting.mColors[0].mValues[3] = one;
    p->mLighting.mColors[4].mValues[0] = z;
    p->mLighting.mColors[4].mValues[3] = one;
    p->mLighting.mColors[9].mValues[0] = z;
    p->mLighting.mColors[9].mValues[1] = z;
    p->mLighting.mColors[9].mValues[2] = z;
    p->mLighting.mColors[9].mValues[3] = one;
    p->mLighting.mColors[10].mValues[0] = z;
    p->mLighting.mColors[10].mValues[1] = z;
    p->mLighting.mColors[10].mValues[2] = z;
    p->mLighting.mColors[10].mValues[3] = one;
    clothApi_constructArray(gabi::ea(p) + 0x3AC, u32(2), u32(16),
                            u32(gabi::guest_fn(reinterpret_cast<const void*>(&clothVector4__ct))));
    clothApi_constructArray(gabi::ea(p) + 0x3CC, u32(2), u32(16),
                            u32(gabi::guest_fn(reinterpret_cast<const void*>(&clothVector4__ct))));
    clothApi_constructArray(gabi::ea(p) + 0x3EC, u32(2), u32(16),
                            u32(gabi::guest_fn(reinterpret_cast<const void*>(&clothVector4__ct))));
    for (u32 offset : {0x40Cu, 0x43Cu, 0x46Cu, 0x49Cu, 0x4CCu, 0x4FCu, 0x52Cu, 0x55Cu})
        if (gabi::ea(p) + offset == 0)
            clothApi_allocate(u32(48));
    for (u32 offset : {0x58Cu, 0x59Cu, 0x5ACu, 0x5BCu, 0x5CCu, 0x5DCu})
        if (gabi::ea(p) + offset == 0)
            clothApi_allocate(u32(16));
    p->mLighting.mEnabled = 0;
    clothApi_constructTexture(gabi::ea(p) + 0x5F0);
    clothApi_constructTexture(gabi::ea(p) + 0x788);
    clothApi_constructSurfaceTransfer(gabi::ea(p) + 0x920);
    clothApi_constructSurfaceTransfer(gabi::ea(p) + 0x9B0);
    p->mHoistGridSize = ny;
    p->mFlyLength = f32(width);
    p->mpTevstr = gabi::ea(lighting);
    p->mHoistLength = f32(height);
    p->mFlyGridSize = nx;
    clothApi_setMatrix(gabi::ea(p), gabi::ea(&lIdentityMatrix));
    nx = u32(p->mFlyGridSize);
    ny = u32(p->mHoistGridSize);
    u32 count = nx * ny;
    if (!gabi::ea(supplied)) {
        p->mpPosArr[0] = clothApi_constructArray(u32(0), count, u32(12), u32(0));
        p->mpPosArr[1] = clothApi_constructArray(u32(0), count, u32(12), u32(0));
    } else {
        p->mpPosArr[0] = u32(supplied->mPositions[0]);
        p->mpPosArr[1] = u32(supplied->mPositions[1]);
    }
    for (u32 buffer = 0; buffer < 2; ++buffer)
        p->mpNrmArr[buffer] = clothApi_constructArray(u32(0), count, u32(12), u32(0));
    for (u32 buffer = 0; buffer < 2; ++buffer)
        p->mpNrmArrBack[buffer] = clothApi_constructArray(u32(0), count, u32(12), u32(0));
    p->mpSpeedArr = clothApi_constructArray(u32(0), count, u32(12), u32(0));
    cloth::Geometry* model = gabi::at<cloth::Geometry>(clothApi_constructGeometry(u32(0)));
    nx = u32(p->mFlyGridSize);
    ny = u32(p->mHoistGridSize);
    p->mpModel = gabi::ea(model);
    clothApi_allocateGeometry(gabi::ea(model), nx * 2 - 2, ny * 2, u32(0));
    model = gabi::at<cloth::Geometry>(u32(p->mpModel));
    clothApi_allocateVertexBuffers(gabi::ea(model), u32(0), u32(model->mRows), u32(0));
    clothApi_allocateUniformBlock(gabi::ea(p) + 0x1D4, u32(1), u32(0));
    clothApi_lockGraphics(lGraphicsMutex);
    clothApi_decodeTextureSurface(gabi::ea(p) + 0x9B0, u32(image->mSurface));
    bool mismatch = false;
    for (u32 offset : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
        if (u32(gabi::at<cloth::TexturePrefix>(gabi::ea(p) + 0x788)->mWords[offset / 4]) !=
            u32(gabi::at<cloth::TexturePrefix>(gabi::ea(p) + 0x9B0)->mWords[offset / 4])) {
            mismatch = true;
            break;
        }
    if (mismatch)
        clothApi_copyTextureSurface(gabi::ea(p) + 0x788, gabi::ea(p) + 0x9B0);
    else {
        u32 a = u32(p->mToonSurface.mImage), b = u32(p->mToonSurface.mMipmap);
        p->mToonTexture.mSurface.mImage = a;
        p->mToonTexture.mCachedMipmap = b;
        p->mToonTexture.mCachedImage = a;
        p->mToonTexture.mSurface.mMipmap = b;
    }
    clothApi_unlockGraphics(lGraphicsMutex);
    return gabi::ea(p);
}

namespace {} // namespace
/* 0251C558 */
void dCloth_packet_c__dt(dCloth_packet_c* p, u32 flags) {
    if (!gabi::ea(p))
        return;
    cloth::Geometry* model = gabi::at<cloth::Geometry>(u32(p->mpModel));
    p->mPacket.mpVtable.v = gabi::ea(&lPacketVtable);
    auto release = [&](be<u32>* pointer) {
        cloth::Heap* heap =
            gabi::at<cloth::Heap>(clothApi_findContainingHeap(lHeapManager, u32(*pointer)));
        u32 target = u32(gabi::at<cloth::HeapVtable>(u32(heap->mpVtable))->mFree.v);
        gabi::call_ptr<void>(target, gabi::ea(heap), u32(*pointer));
    };
    u32 count = u32(model->mRows);
    for (u32 i = 0; i < count;) {
        for (u32 side = 0; side < 2; ++side) {
            u32 n = u32(model->mPools[side].mCount), array = u32(model->mPools[side].mpParts);
            cloth::VertexPart* entry = gabi::at<cloth::VertexPart>(array + (i < n ? i * 592 : 0));
            clothApi_releaseVertexBuffer(gabi::ea(entry) + 0x15C);
            n = u32(model->mPools[side].mCount);
            array = u32(model->mPools[side].mpParts);
            entry = gabi::at<cloth::VertexPart>(array + (i < n ? i * 592 : 0));
            if (u32(entry->mpVertices)) {
                (void)u32(entry->mVertexCount);
                release(&entry->mpVertices);
                entry->mVertexCount = 0;
                entry->mpVertices = 0;
            }
        }
        count = u32(model->mRows);
        ++i;
    }
    for (u32 side = 0; side < 2; ++side) {
        u32 array = u32(model->mPools[side].mpParts);
        if (array) {
            u32 n = u32(model->mPools[side].mCount), i = 0, offset = 0;
            while (s32(i) < s32(n)) {
                u32 entry = array + offset;
                if (entry) {
                    clothApi_destroyVertexBuffer(entry + 0x15C, u32(2));
                    clothApi_destroyVertexResource(entry + 8, u32(2));
                    n = u32(model->mPools[side].mCount);
                    array = u32(model->mPools[side].mpParts);
                }
                ++i;
                offset += 592;
            }
            release(&model->mPools[side].mpParts);
            model->mPools[side].mCount = 0;
            model->mPools[side].mpParts = 0;
        }
    }
    model->mRows = 0;
    model->mFlags = 0;
    model->mReady = 0;
    clothApi_destroyGeometry(u32(p->mpModel), u32(3));
    p->mpModel = 0;
    clothApi_destroyTexture(gabi::ea(p) + 0x788, u32(2));
    clothApi_destroyTexture(gabi::ea(p) + 0x5F0, u32(2));
    clothApi_destroyMaterialBlock(gabi::ea(p) + 0x288, u32(0));
    clothApi_destroyMaterialBlock(gabi::ea(p) + 0x1E0, u32(0));
    clothApi_destroyUniformBlock(gabi::ea(p) + 0x1D4, u32(2));
    u32 array = u32(p->mpMaterials);
    if (array) {
        u32 n = u32(p->mMaterialCapacity), i = 0, offset = 0;
        while (s32(i) < s32(n)) {
            cloth::ShapeGroup* entry = gabi::at<cloth::ShapeGroup>(array + offset);
            if (gabi::ea(entry)) {
                u32 buffer = u32(entry->mShapes[0].mpShapes);
                entry->mpMaterial = 0;
                for (u32 side = 0; side < 2; ++side) {

                    if (side)
                        buffer = u32(entry->mShapes[side].mpShapes);
                    if (buffer) {
                        u32 vertices = u32(entry->mShapes[side].mCount), j = 0, vertexOffset = 0;
                        while (s32(j) < s32(vertices)) {
                            cloth::Shape* object = gabi::at<cloth::Shape>(buffer + vertexOffset);
                            u32 target = u32(
                                gabi::at<cloth::ShapeVtable>(u32(object->mpVtable))->mDestroy.v);
                            gabi::call_ptr<void>(target, gabi::ea(object), u32(2));
                            vertices = u32(entry->mShapes[side].mCount);
                            ++j;
                            vertexOffset += 244;
                            buffer = u32(entry->mShapes[side].mpShapes);
                        }
                        release(&entry->mShapes[side].mpShapes);
                        entry->mShapes[side].mCount = 0;
                        entry->mShapes[side].mpShapes = 0;
                    }
                }
                n = u32(p->mMaterialCapacity);
                array = u32(p->mpMaterials);
            }
            ++i;
            offset += 20;
        }
        release(&p->mpMaterials);
        p->mMaterialCapacity = 0;
        p->mpMaterials = 0;
    }
    clothApi_destroyPacketBase(gabi::ea(p), u32(0));
    if (flags & 1)
        clothApi_deallocate(gabi::ea(p));
}

/* 0251B560 */
u32 default_factor_checkCB(dCloth_packet_c* p, u32 x, u32 y) {
    return x == 0 && (y == 0 || y == u32(p->mHoistGridSize) - 1);
}
/* 0251CEC8 */
void dCloth_packet_c::init() {
    const u32 p = gabi::ea(this);
    u32 ny = u32(mHoistGridSize);
    cloth::Vector* speed = gabi::at<cloth::Vector>(u32(mpSpeedArr));
    cloth::Vector* second = gabi::at<cloth::Vector>(u32(mpPosArr[1]));
    cloth::Vector* first = gabi::at<cloth::Vector>(u32(mpPosArr[0]));
    u32 y = 0;
    if (s32(ny) > 0) {
        u32 nx = u32(mFlyGridSize);
        f32 zero = lZero;
        do {
            u32 x = 0;
            if (s32(nx) > 0) {
                f32 negativeY = f32(s32(0 - y));
                do {
                    f32 fy = f32(s32(ny - 1)), fx = f32(s32(x)), fw = f32(s32(nx - 1));
                    f32 vertical =
                        gabi::fmuls_ppc(f32(f64(negativeY) / f64(fy)), f32(mHoistLength));
                    f32 width = f32(mFlyLength), ratio = f32(f64(fx) / f64(fw));
                    first->mValues[0] = zero;
                    f32 horizontal = gabi::fmuls_ppc(ratio, width);
                    first->mValues[1] = vertical;
                    first->mValues[2] = horizontal;
                    u32 nextNy = u32(mHoistGridSize), nextNx = u32(mFlyGridSize);
                    f32 height = f32(mHoistLength),
                        vy = f32(f64(negativeY) / f64(f32(s32(nextNy - 1))));
                    f32 vx = f32(f64(fx) / f64(f32(s32(nextNx - 1)))), otherWidth = f32(mFlyLength);
                    f32 otherVertical = gabi::fmuls_ppc(vy, height);
                    second->mValues[0] = zero;
                    f32 otherHorizontal = gabi::fmuls_ppc(vx, otherWidth);
                    second->mValues[1] = otherVertical;
                    second->mValues[2] = otherHorizontal;
                    speed->mValues[1] = zero;
                    speed->mValues[2] = zero;
                    speed->mValues[0] = zero;
                    speed = gabi::at<cloth::Vector>(gabi::ea(speed) + (12));
                    ++x;
                    nx = u32(mFlyGridSize);
                    second = gabi::at<cloth::Vector>(gabi::ea(second) + (12));
                    ny = u32(mHoistGridSize);
                    first = gabi::at<cloth::Vector>(gabi::ea(first) + (12));
                } while (s32(x) < s32(nx));
            }
            ++y;
        } while (s32(y) < s32(ny));
    }
    clothApi_setNormals(p);
    u32 a = u32(lBaseXYZ.mWords[0]), current = u8(mCurArr);
    mScale.mWords[0] = a;
    a = u32(lBaseXYZ.mWords[1]);
    ny = u32(mHoistGridSize);
    mScale.mWords[1] = a;
    a = u32(lBaseXYZ.mWords[2]);
    u32 nx = u32(mFlyGridSize);
    mScale.mWords[2] = a;
    a = u32(lBaseZ.mWords[0]);
    mGlobalWind.mWords[0] = a;
    a = u32(lBaseZ.mWords[1]);
    mGlobalWind.mWords[1] = a;
    a = u32(lBaseZ.mWords[2]);
    mGlobalWind.mWords[2] = a;
    clothApi_storeRangeNoSync(u32(mpPosArr[current]), nx * ny * 12);
    current = u8(mCurArr);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    clothApi_storeRangeNoSync(u32(mpNrmArr[current]), nx * ny * 12);
    ny = u32(mHoistGridSize);
    nx = u32(mFlyGridSize);
    current = u8(mCurArr);
    clothApi_storeRangeNoSync(u32(mpNrmArrBack[current]), nx * ny * 12);
    cloth::PacketVtable* vtable = mPacket.mpVtable.get();
    mpFactorCheckCB = gabi::guest_fn(reinterpret_cast<const void*>(&default_factor_checkCB));
    gabi::call_ptr<void>(u32(vtable->mLoadMaterial.v), p);
}

/* 0251E7A8 */
void dCloth_packet_c::setGlobalWind(cloth::Vector* wind) {
    const u32 p = gabi::ea(this);
    gabi::Local<cloth::MatrixWords> matrix;
    clothApi_copyMatrix(p + 0x170, gabi::ea(matrix.get()));
    f32 zero = lZero;
    matrix->mValues[11] = zero;
    matrix->mValues[7] = zero;
    matrix->mValues[3] = zero;
    clothApi_copyMatrix(gabi::ea(matrix.get()), gabi::ea(&lMatrixStack));
    clothApi_invertMatrix(gabi::ea(&lMatrixStack), gabi::ea(&lMatrixStack));
    clothApi_transformVector(gabi::ea(&lMatrixStack), gabi::ea(wind), p + 0xCC);
}
/* 0251D4EC */
void dCloth_packet_c::cloth_move() {
    const u32 p = gabi::ea(this);
    gabi::Local<cloth::Vector> wind, force, temp;
    u32 angle = u16(mWave);
    f32 amplitude = f32(mWindSpeedWave);
    u32 wy = u32(mGlobalWind.mWords[1]);
    f32 base = f32(mWindSpeed);
    f32 sine = f32(lSinCosTable.mValues[((angle & ~7)) / 8][0]);
    u32 wx = u32(mGlobalWind.mWords[0]), wz = u32(mGlobalWind.mWords[2]);
    f32 scale = gabi::fmadds(amplitude, sine, base);
    wind->mWords[0] = wx;
    wind->mWords[1] = wy;
    wind->mWords[2] = wz;
    clothApi_scaleVector(wind.a, wind.a, scale);
    u32 nx = u32(mFlyGridSize), current = u8(mCurArr), ny = u32(mHoistGridSize);
    f32 width = f32(mFlyLength), horizontalFactor = f32(mFlyFlex);
    u32 positions = u32(mpPosArr[current]);
    f32 horizontal = f32(f64(width) / f64(f32(s32(nx - 1))));
    u32 normals = u32(mpNrmArr[current]);
    u32 next = (current ^ 1) & 1;
    mCurArr = next;
    horizontal = gabi::fmuls_ppc(horizontal, horizontalFactor);
    f32 height = f32(mHoistLength), vertical = f32(f64(height) / f64(f32(s32(ny - 1))));
    f32 verticalFactor = f32(mHoistFlex);
    vertical = gabi::fmuls_ppc(vertical, verticalFactor);
    f32 verticalSquare = gabi::fmuls_ppc(vertical, vertical);
    u32 speeds = u32(mpSpeedArr);
    f32 square = gabi::fmadds(horizontal, horizontal, verticalSquare);
    u32 destination = u32(mpPosArr[next]);
    f64 diagonal = clothApi_squareRootFloatInput(square);
    ny = u32(mHoistGridSize);
    u32 y = 0;
    if (s32(ny) > 0) {
        nx = u32(mFlyGridSize);
        do {
            u32 x = 0;
            if (s32(nx) > 0) {
                do {
                    clothApi_getFactor(p, force.a, positions, normals, wind.a, x, y, horizontal,
                                       vertical, diagonal);
                    nx = u32(mFlyGridSize);
                    u32 fx = u32(force->mWords[0]), fz = u32(force->mWords[2]),
                        fy = u32(force->mWords[1]);
                    u32 offset = (x + y * nx) * 12;
                    temp->mWords[0] = fx;
                    temp->mWords[1] = fy;
                    temp->mWords[2] = fz;
                    clothApi_addVector(speeds + offset, temp.a, speeds + offset);
                    nx = u32(mFlyGridSize);
                    offset = (x + y * nx) * 12;
                    f32 damping = f32(mDrag);
                    clothApi_scaleVector(speeds + offset, speeds + offset, damping);
                    nx = u32(mFlyGridSize);
                    offset = (x + y * nx) * 12;
                    gabi::at<cloth::Vector>(destination + offset)->mWords[0] =
                        u32(gabi::at<cloth::Vector>(positions + offset)->mWords[0]);
                    gabi::at<cloth::Vector>(destination + offset)->mWords[1] =
                        u32(gabi::at<cloth::Vector>(positions + offset)->mWords[1]);
                    gabi::at<cloth::Vector>(destination + offset)->mWords[2] =
                        u32(gabi::at<cloth::Vector>(positions + offset)->mWords[2]);
                    nx = u32(mFlyGridSize);
                    offset = (x + y * nx) * 12;
                    clothApi_addVector(destination + offset, speeds + offset, destination + offset);
                    nx = u32(mFlyGridSize);
                    ++x;
                } while (s32(x) < s32(nx));
                ny = u32(mHoistGridSize);
            }
            ++y;
        } while (s32(y) < s32(ny));
    }
    clothApi_setNormals(p);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    current = u8(mCurArr);
    clothApi_storeRangeNoSync(u32(mpPosArr[current]), nx * ny * 12);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    current = u8(mCurArr);
    clothApi_storeRangeNoSync(u32(mpNrmArr[current]), nx * ny * 12);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    current = u8(mCurArr);
    clothApi_storeRangeNoSync(u32(mpNrmArrBack[current]), nx * ny * 12);
}

/* 0251DDE8 */
void dCloth_packet_c::draw() {
    const u32 p = gabi::ea(this);
    gabi::Local<cloth::MatrixWords> view, model;
    gabi::Local<cloth::Vector4> color, first, second;
    clothApi_updateVertices(p);
    clothApi_setFogTevstr(u32(mpTevstr));
    clothApi_setLightTevstr(u32(mpTevstr));
    clothApi_snapshotMatrix(view.a, gabi::ea(&lViewMatrix));
    u32 resource = lViewResource;
    clothApi_setViewUniforms(p + 0x1D4, u32(0), view.a, gabi::ea(&lProjectionMatrix),
                             resource + 0x240);
    u32 env = u32(mpTevstr);
    cloth::FogUniform* uniform = gabi::at<cloth::FogUniform>(u32(mUniformBlock.mpUniform));
    clothApi_colorToFloat(color.a, env + 0x90);
    env = u32(mpTevstr);
    clothApi_scaleColor(first.a, color.a, f32(gabi::at<cloth::LightObject>(env)->mFactors[1]));
    for (u32 i = 0; i < 4; ++i)
        uniform->mColors[0][i] = u32(first->mWords[(i * 4) / 4]);
    env = u32(mpTevstr);
    uniform = gabi::at<cloth::FogUniform>(u32(mUniformBlock.mpUniform));
    clothApi_colorToFloat(color.a, env + 0x160);
    env = u32(mpTevstr);
    clothApi_scaleColor(second.a, color.a,
                        f32(gabi::at<cloth::LightObject>(env + 0x144)->mFactors[1]));
    uniform->mColors[1][0] = u32(second->mWords[0]);
    uniform->mColors[1][1] = u32(second->mWords[1]);
    uniform->mColors[1][2] = u32(second->mWords[2]);
    uniform->mColors[1][3] = u32(second->mWords[3]);
    mTransform.mFlags = 0;
    clothApi_copyMatrix(p + 0x170, gabi::ea(&lMatrixStack));
    f32 tx = f32(mScale.mValues[0]), ty = f32(mScale.mValues[1]), tz = f32(mScale.mValues[2]);
    clothApi_scaleMatrix(gabi::ea(&lMatrixStack), tx, ty, tz);
    for (u32 i = 0; i < 12; ++i)
        model->mValues[(i * 4) / 4] = f32(lMatrixStack.mValues[(i * 4) / 4]);
    clothApi_copyMatrix(model.a, p + 0x254);
    env = u32(mpTevstr);
    s16 a = s16(gabi::at<dKy_tevstr_c>(env)->mColorC0.r),
        b = s16(gabi::at<dKy_tevstr_c>(env)->mColorC0.g),
        c = s16(gabi::at<dKy_tevstr_c>(env)->mColorC0.b),
        d = s16(gabi::at<dKy_tevstr_c>(env)->mColorC0.a);
    f32 divisor = lColorDivisor, af = f32(f64(a) / f64(divisor)), bf = f32(f64(b) / f64(divisor)),
        cf = f32(f64(c) / f64(divisor)), df = f32(f64(d) / f64(divisor));
    mLighting.mColors[4].mValues[1] = bf;
    mLighting.mColors[4].mValues[0] = af;
    mLighting.mColors[4].mValues[2] = cf;
    mLighting.mColors[4].mValues[3] = df;
    u8 ba = u8(gabi::at<dKy_tevstr_c>(env)->mColorK0.r),
       bb = u8(gabi::at<dKy_tevstr_c>(env)->mColorK0.g),
       bc = u8(gabi::at<dKy_tevstr_c>(env)->mColorK0.b),
       bd = u8(gabi::at<dKy_tevstr_c>(env)->mColorK0.a);
    af = f32(f64(ba) / f64(divisor));
    bf = f32(f64(bb) / f64(divisor));
    cf = f32(f64(bc) / f64(divisor));
    df = f32(f64(bd) / f64(divisor));
    mLighting.mColors[5].mValues[1] = bf;
    mLighting.mColors[5].mValues[0] = af;
    mLighting.mColors[5].mValues[2] = cf;
    mLighting.mColors[5].mValues[3] = df;
    clothApi_scaleColorInPlace(p + 0x34C, f32(gabi::at<cloth::LightObject>(env)->mFactors[0]));
    bd = u8(gabi::at<dKy_tevstr_c>(env)->mColorK1.a);
    if (bd) {
        ba = u8(gabi::at<dKy_tevstr_c>(env)->mColorK1.r);
        bc = u8(gabi::at<dKy_tevstr_c>(env)->mColorK1.b);
        bb = u8(gabi::at<dKy_tevstr_c>(env)->mColorK1.g);
        af = f32(f64(ba) / f64(divisor));
        bf = f32(f64(bb) / f64(divisor));
        cf = f32(f64(bc) / f64(divisor));
        df = f32(f64(bd) / f64(divisor));
        mLighting.mColors[6].mValues[1] = bf;
        mLighting.mColors[6].mValues[2] = cf;
        mLighting.mColors[6].mValues[0] = af;
        mLighting.mColors[6].mValues[3] = df;
    } else {
        f32 zero = lZero;
        mLighting.mColors[6].mValues[1] = zero;
        mLighting.mColors[6].mValues[2] = zero;
        mLighting.mColors[6].mValues[0] = zero;
        mLighting.mColors[6].mValues[3] = zero;
    }
    clothApi_submitUniformBlock(p + 0x1D4, u32(0));
}

/* 0251B590 */
void get_cloth_anim_sub_factor(cloth::Vector* p, cloth::Vector* other, cloth::Vector* output,
                               f32 distance, f32 factor) {
    gabi::Local<cloth::Vector> delta, normal;
    clothApi_subtractVector(gabi::ea(other), gabi::ea(delta.get()), gabi::ea(p));
    clothApi_normalizeVector(gabi::ea(delta.get()), gabi::ea(normal.get()));
    f64 square = clothApi_squareMagnitude(gabi::ea(delta.get()));
    f64 length = clothApi_squareRoot(square);
    f32 scale = gabi::fmuls_ppc(f32(length - f64(distance)), factor);
    clothApi_scaleVector(gabi::ea(normal.get()), gabi::ea(normal.get()), scale);
    clothApi_addVector(gabi::ea(output), gabi::ea(normal.get()), gabi::ea(output));
}
/* 0251D128 */
void dCloth_packet_c::getFactor(cloth::Vector* output, cloth::Vector* positions,
                                cloth::Vector* normals, cloth::Vector* wind, u32 x, u32 y,
                                f64 horizontal, f64 vertical, f64 diagonal) {
    const u32 p = gabi::ea(this);
    if (gabi::call_ptr<u32>(u32(mpFactorCheckCB), p, x, y, horizontal, vertical, diagonal)) {
        if (!gabi::ea(output)) {
            output = gabi::at<cloth::Vector>(clothApi_allocate(u32(12)));
            if (!gabi::ea(output))
                return;
        }
        output->mValues[0] = f32(lZeroVector.mValues[0]);
        output->mValues[1] = f32(lZeroVector.mValues[1]);
        output->mValues[2] = f32(lZeroVector.mValues[2]);
        return;
    }
    gabi::Local<cloth::Vector> point, normal, force;
    u32 nx = u32(mFlyGridSize), offset = (x + y * nx) * 12;
    point->mValues[0] = f32(gabi::at<cloth::Vector>(gabi::ea(positions) + offset)->mValues[0]);
    point->mValues[1] = f32(gabi::at<cloth::Vector>(gabi::ea(positions) + offset)->mValues[1]);
    point->mValues[2] = f32(gabi::at<cloth::Vector>(gabi::ea(positions) + offset)->mValues[2]);
    clothApi_dotVector(gabi::ea(wind), gabi::ea(normals) + offset);
    nx = u32(mFlyGridSize);
    offset = (x + y * nx) * 12;
    clothApi_scaleNormalByDotResult(gabi::ea(normals) + offset, normal.a);
    f32 a = f32(normal->mValues[0]), gravity = f32(mGravity);
    force->mValues[0] = a;
    f32 b = f32(normal->mValues[1]), c = f32(normal->mValues[2]), by = gabi::fadds_ppc(b, gravity);
    nx = u32(mFlyGridSize);
    force->mValues[2] = c;
    force->mValues[1] = by;
    u32 flags = x ? 1 : 0;
    if (x != nx - 1)
        flags |= 2;
    u32 ny = u32(mHoistGridSize);
    if (y)
        flags |= 4;
    if (y != ny - 1)
        flags |= 8;
    auto spring = [&](u32 xx, u32 yy, f64 distance, u32 width) {
        u32 neighbor = gabi::ea(positions) + (xx + yy * width) * 12;
        f32 factor = f32(mSpring);
        clothApi_springFactor(point.a, neighbor, force.a, distance, factor);
    };
    if (flags & 1)
        spring(x - 1, y, horizontal, nx);
    if (flags & 2)
        spring(x + 1, y, horizontal, u32(mFlyGridSize));
    if (flags & 4)
        spring(x, y - 1, vertical, u32(mFlyGridSize));
    if (flags & 8)
        spring(x, y + 1, vertical, u32(mFlyGridSize));
    if ((flags & 5) == 5)
        spring(x - 1, y - 1, diagonal, u32(mFlyGridSize));
    if ((flags & 9) == 9)
        spring(x - 1, y + 1, diagonal, u32(mFlyGridSize));
    if ((flags & 6) == 6)
        spring(x + 1, y - 1, diagonal, u32(mFlyGridSize));
    if ((flags & 10) == 10)
        spring(x + 1, y + 1, diagonal, u32(mFlyGridSize));
    if (!gabi::ea(output)) {
        output = gabi::at<cloth::Vector>(clothApi_allocate(u32(12)));
        if (!gabi::ea(output))
            return;
    }
    output->mValues[0] = f32(force->mValues[0]);
    output->mValues[1] = f32(force->mValues[1]);
    output->mValues[2] = f32(force->mValues[2]);
}

namespace {} // namespace
/* 0251C968 */
void dCloth_packet_c::setNrm() {
    gabi::Local<cloth::NormalScratch> scratch;
    auto copy = [](u32 dst, u32 src) {
        u32 a = u32(gabi::at<cloth::Vector>(src)->mWords[0]),
            c = u32(gabi::at<cloth::Vector>(src)->mWords[2]),
            b = u32(gabi::at<cloth::Vector>(src)->mWords[1]);
        gabi::at<cloth::Vector>(dst)->mWords[0] = a;
        gabi::at<cloth::Vector>(dst)->mWords[1] = b;
        gabi::at<cloth::Vector>(dst)->mWords[2] = c;
    };
    u32 current = u8(mCurArr);
    s32 step = s16(mWaveSpeed);
    u32 ny = u32(mHoistGridSize);
    s32 angle = s16(mWave);
    cloth::Vector* normals = gabi::at<cloth::Vector>(u32(mpNrmArr[current]));
    s32 secondAngle = s16(field_0xF0), secondStep = s16(field_0xF2);
    u32 positions = u32(mpPosArr[current]);
    mWave = s16(angle + step);
    field_0xF0 = s16(secondAngle + secondStep);
    u32 y = 0;
    if (s32(ny) > 0) {
        u32 nx = u32(mFlyGridSize);
        do {
            u32 x = 0;
            if (s32(nx) > 0) {
                do {
                    u32 offset = (x + y * nx) * 12, c = u32(lZeroVector.mWords[2]),
                        b = u32(lZeroVector.mWords[1]), a = u32(lZeroVector.mWords[0]);
                    scratch->mPosition.mValues[0] =
                        f32(gabi::at<cloth::Vector>(positions + offset)->mValues[0]);
                    scratch->mPosition.mValues[1] =
                        f32(gabi::at<cloth::Vector>(positions + offset)->mValues[1]);
                    scratch->mPosition.mValues[2] =
                        f32(gabi::at<cloth::Vector>(positions + offset)->mValues[2]);
                    scratch->mTotal.mWords[0] = a;
                    scratch->mTotal.mWords[1] = b;
                    scratch->mTotal.mWords[2] = c;
                    auto addTriangle = [&](u32 verticalDelta, u32 horizontalDelta, bool upward,
                                           bool left) {
                        copy(gabi::ea(&scratch->mCrossCopy), verticalDelta);
                        clothApi_crossVector(
                            (upward == left) ? gabi::ea(&scratch->mCrossCopy) : horizontalDelta,
                            verticalDelta,
                            (upward == left) ? horizontalDelta : gabi::ea(&scratch->mCrossCopy));
                        copy(gabi::ea(&scratch->mTemp), verticalDelta);
                        clothApi_normalizeVector(gabi::ea(&scratch->mTemp), verticalDelta);
                        copy(gabi::ea(&scratch->mTemp), verticalDelta);
                        clothApi_addVector(gabi::ea(&scratch->mTotal), gabi::ea(&scratch->mTemp),
                                           gabi::ea(&scratch->mTotal));
                    };
                    if (x != 0) {
                        clothApi_subtractVector(positions + (x - 1 + y * nx) * 12,
                                                gabi::ea(&scratch->mLeftDifference),
                                                gabi::ea(&scratch->mPosition));
                        copy(gabi::ea(&scratch->mHorizontalDifference),
                             gabi::ea(&scratch->mLeftDifference));
                        if (y != 0) {
                            nx = u32(mFlyGridSize);
                            clothApi_subtractVector(positions + (x + (y - 1) * nx) * 12,
                                                    gabi::ea(&scratch->mUpLeftDifference),
                                                    gabi::ea(&scratch->mPosition));
                            addTriangle(gabi::ea(&scratch->mUpLeftDifference),
                                        gabi::ea(&scratch->mHorizontalDifference), true, true);
                        }
                        ny = u32(mHoistGridSize);
                        if (y != ny - 1) {
                            nx = u32(mFlyGridSize);
                            clothApi_subtractVector(positions + (x + (y + 1) * nx) * 12,
                                                    gabi::ea(&scratch->mDownLeftDifference),
                                                    gabi::ea(&scratch->mPosition));
                            addTriangle(gabi::ea(&scratch->mDownLeftDifference),
                                        gabi::ea(&scratch->mHorizontalDifference), false, true);
                        }
                        nx = u32(mFlyGridSize);
                    }
                    if (x != nx - 1) {
                        clothApi_subtractVector(positions + (x + 1 + y * nx) * 12,
                                                gabi::ea(&scratch->mRightDifference),
                                                gabi::ea(&scratch->mPosition));
                        copy(gabi::ea(&scratch->mHorizontalDifference),
                             gabi::ea(&scratch->mRightDifference));
                        if (y != 0) {
                            nx = u32(mFlyGridSize);
                            clothApi_subtractVector(positions + (x + (y - 1) * nx) * 12,
                                                    gabi::ea(&scratch->mUpRightDifference),
                                                    gabi::ea(&scratch->mPosition));
                            addTriangle(gabi::ea(&scratch->mUpRightDifference),
                                        gabi::ea(&scratch->mHorizontalDifference), true, false);
                        }
                        ny = u32(mHoistGridSize);
                        if (y != ny - 1) {
                            nx = u32(mFlyGridSize);
                            clothApi_subtractVector(positions + (x + (y + 1) * nx) * 12,
                                                    gabi::ea(&scratch->mDownRightDifference),
                                                    gabi::ea(&scratch->mPosition));
                            addTriangle(gabi::ea(&scratch->mDownRightDifference),
                                        gabi::ea(&scratch->mHorizontalDifference), false, false);
                        }
                    }
                    clothApi_normalizeVector(gabi::ea(&scratch->mTotal),
                                             gabi::ea(&scratch->mNormal));
                    s32 phaseStep = s16(mRipple), phase = s16(mWave), amplitude = s16(mRotateY);
                    u32 phaseIndex = u16((y + x) * u32(phaseStep) + u32(phase)) & ~7u;
                    f32 sine = f32(lSinCosTable.mValues[(phaseIndex) / 8][0]),
                        rotation = gabi::fmuls_ppc(f32(amplitude), sine);
                    u32 by = u32(scratch->mNormal.mWords[1]), bx = u32(scratch->mNormal.mWords[0]),
                        bz = u32(scratch->mNormal.mWords[2]);
                    scratch->mTotal.mWords[0] = bx;
                    scratch->mTotal.mWords[1] = by;
                    scratch->mTotal.mWords[2] = bz;
                    clothApi_rotateMatrixY(gabi::ea(&lMatrixStack), s32(s16(gabi::ftoi(rotation))));
                    clothApi_transformVector(gabi::ea(&lMatrixStack), gabi::ea(&scratch->mTotal),
                                             gabi::ea(&scratch->mTemp));
                    clothApi_normalizeVector(gabi::ea(&scratch->mTemp),
                                             gabi::ea(&scratch->mNormal));
                    nx = u32(mFlyGridSize);
                    offset = (x + y * nx) * 12;
                    gabi::at<cloth::Vector>(gabi::ea(normals) + offset)->mWords[0] =
                        u32(scratch->mNormal.mWords[0]);
                    gabi::at<cloth::Vector>(gabi::ea(normals) + offset)->mWords[1] =
                        u32(scratch->mNormal.mWords[1]);
                    gabi::at<cloth::Vector>(gabi::ea(normals) + offset)->mWords[2] =
                        u32(scratch->mNormal.mWords[2]);
                    nx = u32(mFlyGridSize);
                    ++x;
                } while (s32(x) < s32(nx));
                ny = u32(mHoistGridSize);
            }
            ++y;
        } while (s32(y) < s32(ny));
    }
    current = u8(mCurArr);
    cloth::Vector* back = gabi::at<cloth::Vector>(u32(mpNrmArrBack[current]));
    y = 0;
    if (s32(ny) > 0) {
        u32 nx = u32(mFlyGridSize);
        do {
            u32 x = 0;
            if (s32(nx) > 0) {
                do {
                    f32 a = f32(normals->mValues[0]), b = f32(normals->mValues[1]),
                        c = f32(normals->mValues[2]);
                    back->mValues[0] = -a;
                    back->mValues[1] = -b;
                    back->mValues[2] = -c;
                    nx = u32(mFlyGridSize);
                    ++x;
                    back = gabi::at<cloth::Vector>(gabi::ea(back) + (12));
                    normals = gabi::at<cloth::Vector>(gabi::ea(normals) + (12));
                } while (s32(x) < s32(nx));
                ny = u32(mHoistGridSize);
            }
            ++y;
        } while (s32(y) < s32(ny));
    }
}

/* 0251B638 */
void dCloth_packet_c::setMtx(cloth::MatrixWords* matrix) {
    const u32 p = gabi::ea(this);
    clothApi_copyMatrix(gabi::ea(matrix), p + 0x170);
}
/* 0251E194 */
void dCloth_packet_c::cloth_draw() {
    const u32 p = gabi::ea(this);
    clothApi_entryDrawBuffer(lOpaqueDrawBuffer, p, u32(0));
    clothApi_prepareDraw(p);
}
/* 0251E830 */
void dCloth_packet_c::TexObjInit(cloth::ResourceImage* image) {
    const u32 p = gabi::ea(this);
    clothApi_lockGraphics(lGraphicsMutex);
    clothApi_decodeTextureSurface(p + 0x920, u32(image->mSurface));
    bool same = true;
    for (u32 o : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u}) {
        if (u32(gabi::at<cloth::TexturePrefix>(p + 0x5F0)->mWords[o / 4]) !=
            u32(gabi::at<cloth::TexturePrefix>(p + 0x920)->mWords[o / 4])) {
            same = false;
            break;
        }
    }
    if (!same)
        clothApi_copyTextureSurface(p + 0x5F0, p + 0x920);
    else {
        u32 first = u32(mSurface.mImage), second = u32(mSurface.mMipmap);
        mTexture.mSurface.mImage = first;
        mTexture.mCachedMipmap = second;
        mTexture.mCachedImage = first;
        mTexture.mSurface.mMipmap = second;
    }
    clothApi_unlockGraphics(lGraphicsMutex);
}
/* 0251E94C */
void dCloth_packet_c::TexObjLoad(cloth::GraphicsState* state) {
    f32 value = lTextureDepth;
    state->mDepthFunction = 4;
    state->mDepthValue = value;
}
/* 0251E964 */
void dCloth_packet_c::TevSetting() {
    const u32 p = gabi::ea(this);
    gabi::Local<cloth::SafeString> name;
    name->mpText = gabi::guest_str("");
    name->mVtable = gabi::ea(&lMaterialStringVtable);
    cloth::MaterialManager* table = gabi::at<cloth::MaterialManager>(clothApi_getMaterialManager());
    s32 index = clothApi_findMaterial(u32(table->mpResource), gabi::ea(name.get()));
    if (index >= 0) {
        u32 count = u32(table->mCount), materials = u32(table->mpInstances);
        cloth::MaterialInstance* entry = gabi::at<cloth::MaterialInstance>(
            materials + (u32(index) < count ? u32(index) * 36 : 0));
        if (!u8(entry->mReady)) {
            cloth::MaterialResource* resource =
                gabi::at<cloth::MaterialResource>(u32(table->mpResource));
            u32 limit = u32(resource->mCount);
            u32 image = u32(index) < limit ? u32(resource->mpBindings) + u32(index) * 132 : 0;
            clothApi_initializeMaterial(gabi::ea(entry), image, u32(0));
            count = u32(table->mCount);
            materials = u32(table->mpInstances);
        }
        entry = gabi::at<cloth::MaterialInstance>(materials +
                                                  (u32(index) < count ? u32(index) * 36 : 0));
        clothApi_loadMaterialInstances(p + 0x1C4, gabi::ea(entry), u32(0));
    } else
        clothApi_loadMaterialInstances(p + 0x1C4, u32(0), u32(0));
    u32 model = u32(mpModel), count = u32(mMaterialCount);
    for (u32 i = 0; i < count; ++i) {
        u32 limit = u32(mMaterialCapacity);
        cloth::ShapeGroup* entry =
            gabi::at<cloth::ShapeGroup>(u32(mpMaterials) + (i < limit ? i * 20 : 0));
        clothApi_initializeMaterialVertices(gabi::ea(entry), u32(entry->mpMaterial), model, u32(0));
        count = u32(mMaterialCount);
    }
}

/* 0251BD54 */
u32 dCloth_packetXlu_c__ct(dCloth_packetXlu_c* p, cloth::ResourceImage* toon, u32 nx, u32 ny,
                           dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions, f32 width,
                           f32 height) {
    if (!gabi::ea(p)) {
        p = gabi::at<dCloth_packetXlu_c>(clothApi_allocate(u32(0xA44)));
        if (!gabi::ea(p))
            return gabi::ea(p);
    }
    clothApi_constructPacket(gabi::ea(p), gabi::ea(toon), nx, ny, gabi::ea(lighting),
                             gabi::ea(positions), width, height);
    p->mAlpha = 0;
    p->mPacket.mpVtable.v = gabi::ea(&lXluPacketVtable);
    return gabi::ea(p);
}

/* 0251FAD8 */
void dCloth_packetXlu_c__dt(dCloth_packetXlu_c* p, u32 flags) {
    if (gabi::ea(p)) {
        clothApi_destroyPacket(gabi::ea(p), u32(0));
        if (flags & 1)
            clothApi_deallocate(gabi::ea(p));
    }
}

/* 0251EB4C */
void dCloth_packetXlu_c::TevSetting(cloth::GraphicsState* state) {
    u32 flags = u32(state->mBlendMode);
    f32 value = lXluDepth;
    state->mBlendMode = flags | 1;
    state->mDepthFunction = 4;
    state->mDepthValue = value;
}
/* 0251EADC */
void dCloth_packetXlu_c::cloth_draw() {
    const u32 p = gabi::ea(this);
    cloth::Vector* positions = gabi::at<cloth::Vector>(u32(mpPosArr[u8(mCurArr)]));
    gabi::Local<cloth::Vector> position;
    position->x = f32(positions->mValues[0]);
    position->y = f32(positions->mValues[1]);
    position->z = f32(positions->mValues[2]);
    dComIfG_inf_c* game = gabi::at<dComIfG_inf_c>(gabi::ea(dComIfGp_get()));
    clothApi_entryZSortXluDrawList(
        gabi::ea(game) + 0x5D30,
        u32(gabi::at<dDlstHD::dDlst_list_c>(gabi::ea(&game->drawlist))->mpXluList), p,
        gabi::ea(position.get()));
    clothApi_prepareDraw(p);
}
/* 0251BEE8 */
u32 dClothVobj03_c__ct(dClothVobj03_c* p, cloth::ResourceImage* toon, dKy_tevstr_c* lighting,
                       cloth::SuppliedPositions* positions) {
    if (!gabi::ea(p)) {
        p = gabi::at<dClothVobj03_c>(clothApi_allocate(u32(0xA50)));
        if (!gabi::ea(p))
            return gabi::ea(p);
    }
    f32 width = lFlagFlyLength, height = lFlagHoistLength;
    clothApi_constructPacket(gabi::ea(p), gabi::ea(toon), u32(5), u32(5), gabi::ea(lighting),
                             gabi::ea(positions), width, height);
    p->mPacket.mpVtable.v = gabi::ea(&lFlag03Vtable);
    clothApi_clearMemory(gabi::ea(p) + 0xA40, u32(12));
    p->mIsIndoors = 0;
    p->mIsStandItem = gabi::ea(positions) != 0;
    return gabi::ea(p);
}

/* 0251FB2C */
void dClothVobj03_c__dt(dClothVobj03_c* p, u32 flags) {
    if (gabi::ea(p)) {
        clothApi_destroyPacket(gabi::ea(p), u32(0));
        if (flags & 1)
            clothApi_deallocate(gabi::ea(p));
    }
}

/* 0251C078 */
u32 dClothVobj04_c__ct(dClothVobj04_c* p, cloth::ResourceImage* toon, dKy_tevstr_c* lighting,
                       cloth::SuppliedPositions* positions) {
    if (!gabi::ea(p)) {
        p = gabi::at<dClothVobj04_c>(clothApi_allocate(u32(0xA50)));
        if (!gabi::ea(p))
            return gabi::ea(p);
    }
    f32 width = lFlagFlyLength, height = lFlagHoistLength;
    clothApi_constructPacket(gabi::ea(p), gabi::ea(toon), u32(5), u32(5), gabi::ea(lighting),
                             gabi::ea(positions), width, height);
    p->mPacket.mpVtable.v = gabi::ea(&lFlag04Vtable);
    clothApi_clearMemory(gabi::ea(p) + 0xA40, u32(12));
    p->mIsIndoors = 0;
    p->mIsStandItem = gabi::ea(positions) != 0;
    return gabi::ea(p);
}

/* 0251FB80 */
void dClothVobj04_c__dt(dClothVobj04_c* p, u32 flags) {
    if (gabi::ea(p)) {
        clothApi_destroyPacket(gabi::ea(p), u32(0));
        if (flags & 1)
            clothApi_deallocate(gabi::ea(p));
    }
}

/* 0251C214 */
u32 dClothVobj05_c__ct(dClothVobj05_c* p, cloth::ResourceImage* toon, dKy_tevstr_c* lighting,
                       cloth::SuppliedPositions* positions) {
    if (!gabi::ea(p)) {
        p = gabi::at<dClothVobj05_c>(clothApi_allocate(u32(0xA50)));
        if (!gabi::ea(p))
            return gabi::ea(p);
    }
    f32 width = lLongFlagFlyLength, height = lFlagFlyLength;
    clothApi_constructPacket(gabi::ea(p), gabi::ea(toon), u32(5), u32(5), gabi::ea(lighting),
                             gabi::ea(positions), width, height);
    p->mPacket.mpVtable.v = gabi::ea(&lFlag05Vtable);
    clothApi_clearMemory(gabi::ea(p) + 0xA40, u32(12));
    p->mIsIndoors = 0;
    p->mIsStandItem = gabi::ea(positions) != 0;
    return gabi::ea(p);
}

/* 0251FBD4 */
void dClothVobj05_c__dt(dClothVobj05_c* p, u32 flags) {
    if (gabi::ea(p)) {
        clothApi_destroyPacket(gabi::ea(p), u32(0));
        if (flags & 1)
            clothApi_deallocate(gabi::ea(p));
    }
}

/* 0251C3BC */
u32 dClothVobj07_0_c__ct(dClothVobj07_0_c* p, cloth::ResourceImage* toon, dKy_tevstr_c* lighting,
                         cloth::SuppliedPositions* positions) {
    if (!gabi::ea(p)) {
        p = gabi::at<dClothVobj07_0_c>(clothApi_allocate(u32(0xA50)));
        if (!gabi::ea(p))
            return gabi::ea(p);
    }
    f32 width = lFlagFlyLength, height = lLongFlagHoistLength;
    clothApi_constructPacket(gabi::ea(p), gabi::ea(toon), u32(5), u32(5), gabi::ea(lighting),
                             gabi::ea(positions), width, height);
    p->mPacket.mpVtable.v = gabi::ea(&lFlag07Vtable);
    clothApi_clearMemory(gabi::ea(p) + 0xA40, u32(12));
    p->mIsIndoors = 0;
    p->mIsStandItem = gabi::ea(positions) != 0;
    return gabi::ea(p);
}

/* 0251FC28 */
void dClothVobj07_0_c__dt(dClothVobj07_0_c* p, u32 flags) {
    if (gabi::ea(p)) {
        clothApi_destroyPacket(gabi::ea(p), u32(0));
        if (flags & 1)
            clothApi_deallocate(gabi::ea(p));
    }
}

/* 0251EE3C */
void dClothVobj03_c::cloth_copy() {
    u32 current = u8(mCurArr) ^ 1;
    u32 ny = u32(mHoistGridSize), nx = u32(mFlyGridSize);
    mCurArr = current & 1;
    u32 size = nx * ny * 12;
    dCloth_packet_c* source = lFlag03Simulation.top_pointer.get();
    u32 which = u8(source->mCurArr);
    u32 destination = u32(mpNrmArr[(current & 1)]), origin = u32(source->mpNrmArr[which]);
    clothApi_copyMemory(destination, origin, size);
    source = lFlag03Simulation.top_pointer.get();
    which = u8(source->mCurArr);
    ny = u32(mHoistGridSize);
    nx = u32(mFlyGridSize);
    current = u8(mCurArr);
    origin = u32(source->mpNrmArrBack[which]);
    size = nx * ny * 12;
    destination = u32(mpNrmArrBack[current]);
    clothApi_copyMemory(destination, origin, size);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    source = lFlag03Simulation.top_pointer.get();
    destination = u32(mpSpeedArr);
    size = nx * ny * 12;
    origin = u32(source->mpSpeedArr);
    clothApi_copyMemory(destination, origin, size);
}

/* 0251ECE8 */
void dClothVobj03_c::init() {
    const u32 p = gabi::ea(this);
    u32 frame = lFrameCounter, last = lFlag03Simulation.cloth_counter;
    if (last == frame) {
        u32 current = u8(mCurArr);
        u32 nx = u32(mFlyGridSize);
        dCloth_packet_c* source = lFlag03Simulation.top_pointer.get();
        u32 ny = u32(mHoistGridSize);
        u32 other = u8(source->mCurArr), destination = u32(mpNrmArr[current]),
            origin = u32(source->mpNrmArr[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        current = u8(mCurArr);
        source = lFlag03Simulation.top_pointer.get();
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        other = u8(source->mCurArr);
        destination = u32(mpNrmArrBack[current]);
        origin = u32(source->mpNrmArrBack[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        destination = u32(mpSpeedArr);
        source = lFlag03Simulation.top_pointer.get();
        origin = u32(source->mpSpeedArr);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
    } else
        clothApi_initializePacket(p);
    u32 game = gabi::ea(dComIfGp_get());
    cloth::Stage* stage = gabi::at<cloth::Stage>(game + 0x5150);
    cloth::StageInfo* info = gabi::at<cloth::StageInfo>(gabi::call_ptr<u32>(
        u32(gabi::at<cloth::StageVtable>(u32(stage->mpVtable))->mGetStagInfo.v), gabi::ea(stage)));
    mIsIndoors = ((u32(info->mParameters) >> 16) & 7) == 2;
    gabi::call_ptr<void>(u32(mPacket.mpVtable.get()->mLoadMaterial.v), p);
}

/* 0251EF0C */
void dClothVobj03_c::cloth_move() {
    const u32 p = gabi::ea(this);
    if (u8(mIsStandItem)) {
        u32 frame = lFrameCounter, last = lFlag03Simulation.cloth_counter;
        if (frame == last) {
            clothApi_copyFlag03(p);
            return;
        }
        lFlag03Simulation.cloth_counter = frame;
        lFlag03Simulation.top_pointer.v = p;
    }
    bool inside = u8(mIsIndoors) != 0;
    f32 f9 = lFlagTuning184, f11 = lFlagTuning192, f8 = lWindWaveAdjust, f7 = lFlagTuning180,
        f0 = lWindSpeedAdjust;
    f8 = gabi::fadds_ppc(f8, f9);
    f9 = lFlagTuning196;
    f32 f10 = lFlagTuning188;
    f7 = gabi::fadds_ppc(f0, f7);
    f0 = lFlagTuning200;
    if (inside) {
        mSpring = f10;
        mGravity = f11;
        field_0xF2 = 0;
        f32 f12 = lFlagTuning204;
        f8 = lFlagTuning100;
        mFlyFlex = f0;
        f7 = gabi::fmuls_ppc(f7, f12);
        mWindSpeedWave = f8;
        mRotateY = -800;
        mWindSpeed = f7;
        f32 f13 = lFlagTuning208;
        mDrag = f9;
        mRipple = 900;
        mWaveSpeed = 0;
        mHoistFlex = f13;
    } else {
        mSpring = f10;
        mFlyFlex = f0;
        field_0xF2 = 0;
        mDrag = f9;
        mRipple = 900;
        f32 f13 = lFlagTuning208;
        mWindSpeed = f7;
        mWaveSpeed = 1024;
        mRotateY = -800;
        mHoistFlex = f13;
        mWindSpeedWave = f8;
        mGravity = f11;
    }
    clothApi_movePacket(p);
}

/* 0251F188 */
void dClothVobj04_c::cloth_copy() {
    u32 current = u8(mCurArr) ^ 1;
    u32 ny = u32(mHoistGridSize), nx = u32(mFlyGridSize);
    mCurArr = current & 1;
    u32 size = nx * ny * 12;
    dCloth_packet_c* source = lFlag04Simulation.top_pointer.get();
    u32 which = u8(source->mCurArr);
    u32 destination = u32(mpNrmArr[(current & 1)]), origin = u32(source->mpNrmArr[which]);
    clothApi_copyMemory(destination, origin, size);
    source = lFlag04Simulation.top_pointer.get();
    which = u8(source->mCurArr);
    ny = u32(mHoistGridSize);
    nx = u32(mFlyGridSize);
    current = u8(mCurArr);
    origin = u32(source->mpNrmArrBack[which]);
    size = nx * ny * 12;
    destination = u32(mpNrmArrBack[current]);
    clothApi_copyMemory(destination, origin, size);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    source = lFlag04Simulation.top_pointer.get();
    destination = u32(mpSpeedArr);
    size = nx * ny * 12;
    origin = u32(source->mpSpeedArr);
    clothApi_copyMemory(destination, origin, size);
}

/* 0251F034 */
void dClothVobj04_c::init() {
    const u32 p = gabi::ea(this);
    u32 frame = lFrameCounter, last = lFlag04Simulation.cloth_counter;
    if (last == frame) {
        u32 current = u8(mCurArr);
        u32 nx = u32(mFlyGridSize);
        dCloth_packet_c* source = lFlag04Simulation.top_pointer.get();
        u32 ny = u32(mHoistGridSize);
        u32 other = u8(source->mCurArr), destination = u32(mpNrmArr[current]),
            origin = u32(source->mpNrmArr[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        current = u8(mCurArr);
        source = lFlag04Simulation.top_pointer.get();
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        other = u8(source->mCurArr);
        destination = u32(mpNrmArrBack[current]);
        origin = u32(source->mpNrmArrBack[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        destination = u32(mpSpeedArr);
        source = lFlag04Simulation.top_pointer.get();
        origin = u32(source->mpSpeedArr);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
    } else
        clothApi_initializePacket(p);
    u32 game = gabi::ea(dComIfGp_get());
    cloth::Stage* stage = gabi::at<cloth::Stage>(game + 0x5150);
    cloth::StageInfo* info = gabi::at<cloth::StageInfo>(gabi::call_ptr<u32>(
        u32(gabi::at<cloth::StageVtable>(u32(stage->mpVtable))->mGetStagInfo.v), gabi::ea(stage)));
    mIsIndoors = ((u32(info->mParameters) >> 16) & 7) == 2;
    gabi::call_ptr<void>(u32(mPacket.mpVtable.get()->mLoadMaterial.v), p);
}

/* 0251F258 */
void dClothVobj04_c::cloth_move() {
    const u32 p = gabi::ea(this);
    if (u8(mIsStandItem)) {
        u32 frame = lFrameCounter, last = lFlag04Simulation.cloth_counter;
        if (frame == last) {
            clothApi_copyFlag04(p);
            return;
        }
        lFlag04Simulation.cloth_counter = frame;
        lFlag04Simulation.top_pointer.v = p;
    }
    bool inside = u8(mIsIndoors) != 0;
    f32 f8 = lFlagTuning184, f11 = lFlagTuning212, f10 = lWindWaveAdjust, f12 = lFlagTuning216,
        f9 = gabi::fadds_ppc(f10, f8);
    f10 = lWindSpeedAdjust;
    f32 f13 = lFlagTuning180, f0 = lFlagTuning192;
    f8 = gabi::fadds_ppc(f10, f13);
    f13 = lFlagTuning208;
    if (inside) {
        mDrag = f13;
        mHoistFlex = f11;
        f9 = lFlagTuning100;
        mSpring = f12;
        mWindSpeedWave = f9;
        f10 = lFlagTuning204;
        field_0xF2 = 0;
        mRipple = 900;
        f8 = gabi::fmuls_ppc(f8, f10);
        mGravity = f0;
        mWaveSpeed = 0;
        mWindSpeed = f8;
        mRotateY = -800;
        mFlyFlex = f11;
    } else {
        mDrag = f13;
        mHoistFlex = f11;
        mWindSpeed = f8;
        field_0xF2 = 0;
        mRotateY = -800;
        mRipple = 900;
        mFlyFlex = f11;
        mWaveSpeed = 1024;
        mSpring = f12;
        mGravity = f0;
        mWindSpeedWave = f9;
    }
    clothApi_movePacket(p);
}

/* 0251F4C4 */
void dClothVobj05_c::cloth_copy() {
    u32 current = u8(mCurArr) ^ 1;
    u32 ny = u32(mHoistGridSize), nx = u32(mFlyGridSize);
    mCurArr = current & 1;
    u32 size = nx * ny * 12;
    dCloth_packet_c* source = lFlag05Simulation.top_pointer.get();
    u32 which = u8(source->mCurArr);
    u32 destination = u32(mpNrmArr[(current & 1)]), origin = u32(source->mpNrmArr[which]);
    clothApi_copyMemory(destination, origin, size);
    source = lFlag05Simulation.top_pointer.get();
    which = u8(source->mCurArr);
    ny = u32(mHoistGridSize);
    nx = u32(mFlyGridSize);
    current = u8(mCurArr);
    origin = u32(source->mpNrmArrBack[which]);
    size = nx * ny * 12;
    destination = u32(mpNrmArrBack[current]);
    clothApi_copyMemory(destination, origin, size);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    source = lFlag05Simulation.top_pointer.get();
    destination = u32(mpSpeedArr);
    size = nx * ny * 12;
    origin = u32(source->mpSpeedArr);
    clothApi_copyMemory(destination, origin, size);
}

/* 0251F370 */
void dClothVobj05_c::init() {
    const u32 p = gabi::ea(this);
    u32 frame = lFrameCounter, last = lFlag05Simulation.cloth_counter;
    if (last == frame) {
        u32 current = u8(mCurArr);
        u32 nx = u32(mFlyGridSize);
        dCloth_packet_c* source = lFlag05Simulation.top_pointer.get();
        u32 ny = u32(mHoistGridSize);
        u32 other = u8(source->mCurArr), destination = u32(mpNrmArr[current]),
            origin = u32(source->mpNrmArr[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        current = u8(mCurArr);
        source = lFlag05Simulation.top_pointer.get();
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        other = u8(source->mCurArr);
        destination = u32(mpNrmArrBack[current]);
        origin = u32(source->mpNrmArrBack[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        destination = u32(mpSpeedArr);
        source = lFlag05Simulation.top_pointer.get();
        origin = u32(source->mpSpeedArr);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
    } else
        clothApi_initializePacket(p);
    u32 game = gabi::ea(dComIfGp_get());
    cloth::Stage* stage = gabi::at<cloth::Stage>(game + 0x5150);
    cloth::StageInfo* info = gabi::at<cloth::StageInfo>(gabi::call_ptr<u32>(
        u32(gabi::at<cloth::StageVtable>(u32(stage->mpVtable))->mGetStagInfo.v), gabi::ea(stage)));
    mIsIndoors = ((u32(info->mParameters) >> 16) & 7) == 2;
    gabi::call_ptr<void>(u32(mPacket.mpVtable.get()->mLoadMaterial.v), p);
}

/* 0251F594 */
void dClothVobj05_c::cloth_move() {
    const u32 p = gabi::ea(this);
    if (u8(mIsStandItem)) {
        u32 frame = lFrameCounter, last = lFlag05Simulation.cloth_counter;
        if (frame == last) {
            clothApi_copyFlag05(p);
            return;
        }
        lFlag05Simulation.cloth_counter = frame;
        lFlag05Simulation.top_pointer.v = p;
    }
    bool inside = u8(mIsIndoors) != 0;
    f32 f12 = lFlagTuning220, f9 = lFlagTuning184, f11 = lWindWaveAdjust, f0 = lFlagTuning180,
        f8 = gabi::fadds_ppc(f11, f9), f10 = lWindSpeedAdjust;
    f32 f7 = lFlagTuning224, f6 = gabi::fadds_ppc(f10, f0), f13 = lFlagTuning216;
    if (inside) {
        mGravity = f7;
        mHoistFlex = f12;
        f8 = lFlagTuning100;
        f10 = lFlagTuning204;
        mWindSpeedWave = f8;
        f11 = lFlagTuning196;
        f6 = gabi::fmuls_ppc(f6, f10);
        mSpring = f13;
        mDrag = f11;
        mWindSpeed = f6;
        mRotateY = 0;
        mWaveSpeed = 0;
        field_0xF2 = 0;
        mFlyFlex = f12;
        mRipple = 0;
    } else {
        mHoistFlex = f12;
        mSpring = f13;
        mWaveSpeed = 512;
        mWindSpeed = f6;
        mGravity = f7;
        f0 = lFlagTuning232;
        f13 = lFlagTuning228;
        mFlyFlex = f0;
        mDrag = f13;
        mRipple = 900;
        mWindSpeedWave = f8;
        field_0xF2 = 0;
        mRotateY = -800;
    }
    clothApi_movePacket(p);
}

/* 0251C208 */
u32 dClothVobj05_VtxFactorCB(dCloth_packet_c* p, u32 x) { return x == 0; }
/* 0251F808 */
void dClothVobj07_0_c::cloth_copy() {
    u32 current = u8(mCurArr) ^ 1;
    u32 ny = u32(mHoistGridSize), nx = u32(mFlyGridSize);
    mCurArr = current & 1;
    u32 size = nx * ny * 12;
    dCloth_packet_c* source = lFlag07Simulation.top_pointer.get();
    u32 which = u8(source->mCurArr);
    u32 destination = u32(mpNrmArr[(current & 1)]), origin = u32(source->mpNrmArr[which]);
    clothApi_copyMemory(destination, origin, size);
    source = lFlag07Simulation.top_pointer.get();
    which = u8(source->mCurArr);
    ny = u32(mHoistGridSize);
    nx = u32(mFlyGridSize);
    current = u8(mCurArr);
    origin = u32(source->mpNrmArrBack[which]);
    size = nx * ny * 12;
    destination = u32(mpNrmArrBack[current]);
    clothApi_copyMemory(destination, origin, size);
    nx = u32(mFlyGridSize);
    ny = u32(mHoistGridSize);
    source = lFlag07Simulation.top_pointer.get();
    destination = u32(mpSpeedArr);
    size = nx * ny * 12;
    origin = u32(source->mpSpeedArr);
    clothApi_copyMemory(destination, origin, size);
}

/* 0251F6B4 */
void dClothVobj07_0_c::init() {
    const u32 p = gabi::ea(this);
    u32 frame = lFrameCounter, last = lFlag07Simulation.cloth_counter;
    if (last == frame) {
        u32 current = u8(mCurArr);
        u32 nx = u32(mFlyGridSize);
        dCloth_packet_c* source = lFlag07Simulation.top_pointer.get();
        u32 ny = u32(mHoistGridSize);
        u32 other = u8(source->mCurArr), destination = u32(mpNrmArr[current]),
            origin = u32(source->mpNrmArr[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        current = u8(mCurArr);
        source = lFlag07Simulation.top_pointer.get();
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        other = u8(source->mCurArr);
        destination = u32(mpNrmArrBack[current]);
        origin = u32(source->mpNrmArrBack[other]);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
        ny = u32(mHoistGridSize);
        nx = u32(mFlyGridSize);
        destination = u32(mpSpeedArr);
        source = lFlag07Simulation.top_pointer.get();
        origin = u32(source->mpSpeedArr);
        clothApi_copyMemory(destination, origin, nx * ny * 12);
    } else
        clothApi_initializePacket(p);
    u32 game = gabi::ea(dComIfGp_get());
    cloth::Stage* stage = gabi::at<cloth::Stage>(game + 0x5150);
    cloth::StageInfo* info = gabi::at<cloth::StageInfo>(gabi::call_ptr<u32>(
        u32(gabi::at<cloth::StageVtable>(u32(stage->mpVtable))->mGetStagInfo.v), gabi::ea(stage)));
    mIsIndoors = ((u32(info->mParameters) >> 16) & 7) == 2;
    gabi::call_ptr<void>(u32(mPacket.mpVtable.get()->mLoadMaterial.v), p);
}

/* 0251F8D8 */
void dClothVobj07_0_c::cloth_move() {
    const u32 p = gabi::ea(this);
    if (u8(mIsStandItem)) {
        u32 frame = lFrameCounter, last = lFlag07Simulation.cloth_counter;
        if (frame == last) {
            clothApi_copyFlag07(p);
            return;
        }
        lFlag07Simulation.cloth_counter = frame;
        lFlag07Simulation.top_pointer.v = p;
    }
    bool inside = u8(mIsIndoors) != 0;
    f32 f0 = lFlagTuning184, f12 = lFlagTuning180, f13 = lWindSpeedAdjust, f11 = lWindWaveAdjust;
    f12 = gabi::fadds_ppc(f13, f12);
    f32 f7 = lFlagTuning236, f8 = gabi::fadds_ppc(f11, f0), f9 = lFlagTuning196;
    if (inside) {
        mDrag = f9;
        mFlyFlex = f7;
        field_0xF2 = 0;
        mWaveSpeed = 0;
        f32 f10 = lFlagTuning204;
        f8 = lFlagTuning100;
        mHoistFlex = f7;
        f0 = lFlagTuning224;
        mRipple = 900;
        f11 = lFlagTuning240;
        mRotateY = -800;
        mWindSpeedWave = f8;
        f12 = gabi::fmuls_ppc(f12, f10);
        mGravity = f0;
        mSpring = f11;
        mWindSpeed = f12;
    } else {
        mDrag = f9;
        mHoistFlex = f7;
        field_0xF2 = 0;
        f0 = lFlagTuning244;
        mWaveSpeed = 256;
        f13 = lFlagTuning188;
        mWindSpeed = f12;
        mRotateY = -800;
        mGravity = f0;
        mWindSpeedWave = f8;
        mRipple = 900;
        mFlyFlex = f7;
        mSpring = f13;
    }
    clothApi_movePacket(p);
}

/* 0251C3B0 */
u32 dClothVobj07_0_VtxFactorCB(dCloth_packet_c* p, u32 x) { return x == 0; }
/* 0251FA94 */
u32 clothVector4__ct(cloth::Vector4* p) {
    return gabi::ea(p) ? gabi::ea(p) : clothApi_allocate(u32(0x10));
}
/* 0251FAC0 */
void clothVector4__dt(cloth::Vector4* p, u32 flags) {
    if (gabi::ea(p) && (flags & 1))
        clothApi_deallocate(gabi::ea(p));
}
/* 0251FAD4 */
void clothPacketNoOp() {}
/* 0251FC7C */
void clothHeaderNoOp() {}
/* 0251FC80 */
void __sinit_d_cloth_packet_cpp() {
    lClothHeaderStatics.mWords[2] = 0;
    lClothHeaderStatics.mWords[0] = 0;
    lClothHeaderStatics.mWords[3] = 0;
    lClothHeaderStatics.mWords[1] = 0;
    clothApi_registerDestructor(gabi::ea(&lClothDestructor0));
    f32 first = lClothTuning2, second = lClothTuning3;
    lClothHeaderFirst = first;
    lClothHeaderSecond = second;
    clothApi_initializeHeaderFloat(gabi::ea(&lClothHeaderFloatGuard));
    clothApi_registerDestructor(gabi::ea(&lClothDestructor1));
    clothApi_initializeHeaderByte(gabi::ea(&lClothHeaderByteGuard));
    clothApi_registerDestructor(gabi::ea(&lClothDestructor2));
}
/* 0251D7C4 */
void copyClothViewMatrix(cloth::MatrixWords* output, cloth::MatrixWords* input) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = f32(input->mValues[i]);
    for (u32 i = 0; i < 12; ++i)
        output->mValues[i] = values[i];
}
/* 0251DD24 */
void clothColorToFloat(cloth::Vector4* output, cloth::ColorS10* input) {
    s16 first = s16(input->mValues[0]), third = s16(input->mValues[2]),
        second = s16(input->mValues[1]);
    f32 divisor = lColorDivisor;
    s16 fourth = s16(input->mValues[3]);
    f32 a = f32(f64(first) / f64(divisor)), b = f32(f64(second) / f64(divisor));
    f32 c = f32(f64(third) / f64(divisor)), d = f32(f64(fourth) / f64(divisor));
    output->mValues[0] = a;
    output->mValues[1] = b;
    output->mValues[2] = c;
    output->mValues[3] = d;
}
/* 0251BC80 */
u32 dCloth_packet_create(cloth::ResourceImage* image, cloth::ResourceImage* toon, u32 nx, u32 ny,
                         dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions, f32 width,
                         f32 height) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(clothApi_constructPacket(
        u32(0), gabi::ea(toon), nx, ny, gabi::ea(lighting), gabi::ea(positions), width, height));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
    }
    return gabi::ea(p);
}

/* 0251BE14 */
u32 dCloth_packetXlu_create(cloth::ResourceImage* image, cloth::ResourceImage* toon, u32 nx, u32 ny,
                            dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions, f32 width,
                            f32 height) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(clothApi_constructXluPacket(
        u32(0), gabi::ea(toon), nx, ny, gabi::ea(lighting), gabi::ea(positions), width, height));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
    }
    return gabi::ea(p);
}

/* 0251BFA4 */
u32 dClothVobj03_create(cloth::ResourceImage* image, cloth::ResourceImage* toon,
                        dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(
        clothApi_constructFlag03(u32(0), gabi::ea(toon), gabi::ea(lighting), gabi::ea(positions)));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
    }
    return gabi::ea(p);
}

/* 0251C134 */
u32 dClothVobj04_create(cloth::ResourceImage* image, cloth::ResourceImage* toon,
                        dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(
        clothApi_constructFlag04(u32(0), gabi::ea(toon), gabi::ea(lighting), gabi::ea(positions)));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
    }
    return gabi::ea(p);
}

/* 0251C2D0 */
u32 dClothVobj05_create(cloth::ResourceImage* image, cloth::ResourceImage* toon,
                        dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(
        clothApi_constructFlag05(u32(0), gabi::ea(toon), gabi::ea(lighting), gabi::ea(positions)));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
        p->mpFactorCheckCB =
            gabi::guest_fn(reinterpret_cast<const void*>(&dClothVobj05_VtxFactorCB));
    }
    return gabi::ea(p);
}

/* 0251C478 */
u32 dClothVobj07_0_create(cloth::ResourceImage* image, cloth::ResourceImage* toon,
                          dKy_tevstr_c* lighting, cloth::SuppliedPositions* positions) {
    dCloth_packet_c* p = gabi::at<dCloth_packet_c>(
        clothApi_constructFlag07(u32(0), gabi::ea(toon), gabi::ea(lighting), gabi::ea(positions)));
    if (!gabi::ea(p))
        return gabi::ea(p);
    if (!u32(p->mpPosArr[0]))
        return 0;
    if (!u32(p->mpPosArr[1]))
        return 0;
    if (!u32(p->mpNrmArr[0]))
        return 0;
    if (!u32(p->mpNrmArr[1]))
        return 0;
    if (!u32(p->mpNrmArrBack[0]))
        return 0;
    if (!u32(p->mpNrmArrBack[1]))
        return 0;
    if (!u32(p->mpSpeedArr))
        return 0;
    if (gabi::ea(image)) {
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mTexObjInit.v), gabi::ea(p),
                             gabi::ea(image));
        gabi::call_ptr<void>(u32(p->mPacket.mpVtable.get()->mInit.v), gabi::ea(p));
        p->mpFactorCheckCB =
            gabi::guest_fn(reinterpret_cast<const void*>(&dClothVobj07_0_VtxFactorCB));
    }
    return gabi::ea(p);
}

/* 0251FA00 */
void __sinit_cloth_math_cpp() {
    lMathHeaderStatics.mWords[2] = 0;
    lMathHeaderStatics.mWords[0] = 0;
    lMathHeaderStatics.mWords[3] = 0;
    lMathHeaderStatics.mWords[1] = 0;
    clothApi_registerDestructor(gabi::ea(&lMathDestructor0));
    f32 first = lClothTuning10, second = lClothTuning11;
    lMathHeaderFirst = first;
    lMathHeaderSecond = second;
    clothApi_initializeHeaderFloat(gabi::ea(&lMathHeaderFloatGuard));
    clothApi_registerDestructor(gabi::ea(&lMathDestructor1));
    clothApi_initializeHeaderByte(gabi::ea(&lMathHeaderByteGuard));
    clothApi_registerDestructor(gabi::ea(&lMathDestructor2));
}

/* 0251EB70 */
void dCloth_packetXlu_c::loadMaterial() {
    const u32 p = gabi::ea(this);
    gabi::Local<cloth::SafeString> name;
    name->mpText = gabi::guest_str("");
    name->mVtable = gabi::ea(&lXluStringVtable);
    cloth::MaterialManager* table = gabi::at<cloth::MaterialManager>(clothApi_getMaterialManager());
    s32 index = clothApi_findMaterial(u32(table->mpResource), gabi::ea(name.get()));
    if (index >= 0) {
        u32 count = u32(table->mCount), materials = u32(table->mpInstances);
        cloth::MaterialInstance* entry = gabi::at<cloth::MaterialInstance>(
            materials + (u32(index) < count ? u32(index) * 36 : 0));
        if (!u8(entry->mReady)) {
            cloth::MaterialResource* resource =
                gabi::at<cloth::MaterialResource>(u32(table->mpResource));
            u32 limit = u32(resource->mCount);
            u32 image = u32(index) < limit ? u32(resource->mpBindings) + u32(index) * 132 : 0;
            clothApi_initializeMaterial(gabi::ea(entry), image, u32(0));
            count = u32(table->mCount);
            materials = u32(table->mpInstances);
        }
        entry = gabi::at<cloth::MaterialInstance>(materials +
                                                  (u32(index) < count ? u32(index) * 36 : 0));
        clothApi_loadMaterialInstances(p + 0x1C4, gabi::ea(entry), u32(0));
    } else
        clothApi_loadMaterialInstances(p + 0x1C4, u32(0), u32(0));
    u32 model = u32(mpModel), count = u32(mMaterialCount);
    for (u32 i = 0; i < count; ++i) {
        u32 limit = u32(mMaterialCapacity);
        cloth::ShapeGroup* entry =
            gabi::at<cloth::ShapeGroup>(u32(mpMaterials) + (i < limit ? i * 20 : 0));
        clothApi_initializeMaterialVertices(gabi::ea(entry), u32(entry->mpMaterial), model, u32(0));
        count = u32(mMaterialCount);
    }
}

/* 0251D864 */
void dCloth_packet_c::updateVertexBuffers() {
    u32 nx = u32(mFlyGridSize), ny = u32(mHoistGridSize), columns = nx - 1, current = u8(mCurArr);
    f32 unit = lUnit, du = f32(f64(unit) / f64(f32(s32(columns)))),
        dv = f32(f64(unit) / f64(f32(s32(ny - 1)))), zero = lZero;
    u32 positions = u32(mpPosArr[current]);
    u32 normal = u32(mpNrmArr[current]);
    cloth::Geometry* model = gabi::at<cloth::Geometry>(u32(mpModel));
    for (u32 side = 0; side < 2; ++side) {
        f32 u = zero;
        u32 x = 0;
        if (side) {
            current = u8(mCurArr);
            normal = u32(mpNrmArrBack[current]);
        }
        while (s32(x) < s32(columns)) {
            u32 selector = u32(model->mPoolIndex);
            cloth::VertexPool* table = gabi::at<cloth::VertexPool>(gabi::ea(model) + selector * 8);
            u32 poolCount = u32(table->mCount);
            u32 pool = u32(table->mpParts);
            u32 row = side ? columns + x : x;
            cloth::VertexPart* entry =
                gabi::at<cloth::VertexPart>(pool + (row < poolCount ? row * 592 : 0));
            u32 vertex = 0;
            u32 y = 0;
            f32 nextU = gabi::fadds_ppc(u, du), v = zero;
            auto destination = [&](u32 index) {
                u32 count = u32(entry->mVertexCount), base = u32(entry->mpVertices);
                return base + (index < count ? index * 152 : 0);
            };
            auto copy = [](u32 dest, u32 src, u32 offset) {
                f32 c = f32(gabi::at<cloth::Vector>(src)->mValues[2]),
                    a = f32(gabi::at<cloth::Vector>(src)->mValues[0]),
                    b = f32(gabi::at<cloth::Vector>(src)->mValues[1]);
                gabi::at<cloth::Vertex>(dest)->mPositionAndNormal[(offset) / 4] = a;
                gabi::at<cloth::Vertex>(dest)->mPositionAndNormal[(offset + 4) / 4] = b;
                gabi::at<cloth::Vertex>(dest)->mPositionAndNormal[(offset + 8) / 4] = c;
            };
            if (s32(ny) > 0) {
                do {
                    cloth::Vertex* dst = gabi::at<cloth::Vertex>(destination(vertex));
                    copy(gabi::ea(dst), positions + (x + y * nx) * 12, 0);
                    dst = gabi::at<cloth::Vertex>(destination(vertex));
                    nx = u32(mFlyGridSize);
                    copy(gabi::ea(dst), normal + (x + y * nx) * 12, 12);
                    dst = gabi::at<cloth::Vertex>(destination(vertex));
                    if (!side) {
                        dst->mTexCoord[1] = v;
                        dst->mTexCoord[0] = u;
                    } else {
                        dst->mTexCoord[0] = u;
                        dst->mTexCoord[1] = v;
                    }
                    ++vertex;
                    dst = gabi::at<cloth::Vertex>(destination(vertex));
                    nx = u32(mFlyGridSize);
                    copy(gabi::ea(dst), positions + (x + 1 + y * nx) * 12, 0);
                    dst = gabi::at<cloth::Vertex>(destination(vertex));
                    nx = u32(mFlyGridSize);
                    copy(gabi::ea(dst), normal + (x + 1 + y * nx) * 12, 12);
                    dst = gabi::at<cloth::Vertex>(destination(vertex));
                    dst->mTexCoord[1] = v;
                    dst->mTexCoord[0] = nextU;
                    ++y;
                    ny = u32(mHoistGridSize);
                    ++vertex;
                    v = gabi::fadds_ppc(v, dv);
                    nx = u32(mFlyGridSize);
                } while (s32(y) < s32(ny));
                model = gabi::at<cloth::Geometry>(u32(mpModel));
                columns = nx - 1;
            }
            u = nextU;
            ++x;
        }
    }
    clothApi_flushVertexBuffers(gabi::ea(model), u32(0), u32(model->mRows));
}

/* 0251E1D8 */
void dCloth_packet_c::drawMaterial(cloth::DrawInfo* state) {
    const u32 p = gabi::ea(this);
    u32 index = u32(gabi::at<cloth::GraphicsState>(gabi::ea(state))->mBlendMode);
    cloth::ShaderMaterial* material = gabi::at<cloth::ShaderMaterial>(0);
    if (s32(index) < 4) {
        u32 count = u32(mMaterialCapacity), array = u32(mpMaterials);
        material = gabi::at<cloth::ShaderMaterial>(
            u32(gabi::at<cloth::ShapeGroup>(array + (index < count ? index * 20 : 0))->mpMaterial));
    }
    cloth::ShaderCache* cache =
        gabi::at<cloth::ShaderCache>(clothApi_getShaderCache(gabi::ea(&lGraphicsSystem)));
    cloth::ShaderProgram* shader = gabi::at<cloth::ShaderProgram>(u32(material->mpProgram));
    u32 last = u32(cache->mpProgram);
    if (gabi::ea(shader) != last) {
        u8 flags = u8(shader->mFlags);
        u32 previous = u32(cache->mState);
        if (flags & 2) {
            shader->mFlags = flags & ~2;
            clothApi_updateShader(gabi::ea(shader), u32(0));
        }
        cloth::ShaderProgramState* shaderResource =
            gabi::at<cloth::ShaderProgramState>(u32(shader->mpResource));
        u32 program = u32(shaderResource->mState);
        if (previous != program)
            clothApi_bindShaderProgram(program);
        u32 size = u32(shader->mBinarySize);
        if (size) {
            clothApi_bindDisplayList(u32(shader->mpBinary), size);
            cache->mState = program;
            cache->mpProgram = gabi::ea(shader);
        } else {
            clothApi_bindShader(gabi::ea(shader));
            cache->mpProgram = gabi::ea(shader);
            cache->mState = program;
        }
    }
    index = u32(gabi::at<cloth::GraphicsState>(gabi::ea(state))->mBlendMode);
    auto bindUniforms = [&](u32 uniform) {
        u32 fieldIndex = u32(gabi::at<cloth::UniformData>(uniform)->mRecordIndex);
        u32 present = u32(material->mUniformCount);
        cloth::UniformSlots* description =
            gabi::at<cloth::UniformSlots>(present ? u32(material->mpUniforms) : 0);
        cloth::UniformRecord* block =
            gabi::at<cloth::UniformRecord>(uniform + 0x10 + fieldIndex * 28);
        s32 vertex = s16(description->mVertex);
        u32 buffer = u32(block->mpBuffer), size = u32(block->mSize);
        s32 pixel = s16(description->mPixel), geometry = s16(description->mGeometry);
        if (pixel != -1)
            clothApi_bindPixelUniform(pixel, size, buffer);
        if (vertex != -1)
            clothApi_bindVertexUniform(vertex, size, buffer);
        if (geometry != -1)
            clothApi_bindGeometryUniform(geometry, size, buffer);
        return geometry != -1;
    };
    auto bindMaterial = [&](u32 offset) {
        u32 target = u32(gabi::at<cloth::UniformVtable>(
                             u32(gabi::at<cloth::UniformObject>(p + offset)->mpVtable))
                             ->mBind.v);
        gabi::call_ptr<void>(target, p + offset, gabi::ea(material));
    };
    if (index == 0) {
        bindMaterial(0x1E0);
        cloth::DrawObject* extra = gabi::at<cloth::DrawObject>(u32(state->mpDrawObject));
        if (gabi::ea(extra)) {
            u32 uniform = u32(extra->mpUniformData);
            bindUniforms(uniform);
        }
    } else if (index == 1 || index == 2) {
        bindUniforms(u32(mUniformBlock.mpUniform));
        bindMaterial(0x288);
        bindMaterial(0x1E0);
        if (index == 2) {
            cloth::UniformObject* extra = gabi::at<cloth::UniformObject>(u32(state->mpObject));
            if (gabi::ea(extra))
                gabi::call_ptr<void>(
                    u32(gabi::at<cloth::UniformVtable>(u32(extra->mpVtable))->mBind.v),
                    gabi::ea(extra), gabi::ea(material));
            clothApi_bindDrawMaterial(gabi::ea(state), gabi::ea(material));
        }
    }
    u32 count = u32(material->mSamplerCount), textures = count ? u32(material->mpSamplers) : 0;
    clothApi_bindTexture(p + 0x5F0, textures + 4, u32(0), u32(0));
    count = u32(material->mSamplerCount);
    textures = count > 1 ? u32(material->mpSamplers) + 20 : 0;
    clothApi_bindTexture(p + 0x788, textures + 4, u32(1), u32(0));
    gabi::Local<cloth::GraphicsState> render;
    clothApi_constructRenderState(render.a);
    u32 flags = u32(gabi::at<cloth::GraphicsState>(render.a)->mStencilBits);
    gabi::at<cloth::GraphicsState>(render.a)->mBlendMode = 2;
    gabi::at<cloth::GraphicsState>(render.a)->mCullMode = 0;
    gabi::at<cloth::GraphicsState>(render.a)->mDepthEnabled = 1;
    flags = ((flags & ~15u) + 7) & 0xFFFFFF0F;
    gabi::at<cloth::GraphicsState>(render.a)->mStencilBits = flags + 16;
    gabi::call_ptr<void>(u32(mPacket.mpVtable.get()->mTexObjLoad.v), p, render.a);
    clothApi_configureMaterialState(
        u32(gabi::at<cloth::GraphicsState>(gabi::ea(state))->mBlendMode), render.a);
    clothApi_applyRenderState(render.a);
    auto draw = [&](u32 row) {
        u32 idx = u32(gabi::at<cloth::GraphicsState>(gabi::ea(state))->mBlendMode);
        u32 limit = u32(mMaterialCapacity);
        cloth::Geometry* model = gabi::at<cloth::Geometry>(u32(mpModel));
        u32 array = u32(mpMaterials);
        u32 selector = u32(model->mPoolIndex);
        u32 entry = array + (idx < limit ? idx * 20 : 0);
        cloth::ShapeGroup* block = gabi::at<cloth::ShapeGroup>(entry + (selector == 0 ? 8 : 0));
        u32 n = u32(block->mShapes[0].mCount);
        u32 buffer = u32(block->mShapes[0].mpShapes);
        clothApi_bindVertexBuffer(buffer + (row < n ? row * 244 : 0));
        u32 vertices = u32(mHoistGridSize) * 2;
        if (vertices)
            clothApi_drawIndexed(lIndexFormat, vertices, lPrimitiveType, lIndexData, u32(0),
                                 u32(1));
    };
    u32 nx = u32(mFlyGridSize), x = 0;
    while (s32(x) < s32(nx - 1)) {
        draw(x);
        nx = u32(mFlyGridSize);
        ++x;
    }
    gabi::at<cloth::GraphicsState>(render.a)->mCullMode = 1;
    clothApi_applyCullMode(render.a);
    nx = u32(mFlyGridSize);
    x = 0;
    while (s32(x) < s32(nx - 1)) {
        draw(nx - 1 + x);
        nx = u32(mFlyGridSize);
        ++x;
    }
    clothApi_applyRenderState(gabi::ea(&lDefaultRenderState));
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data lClothDestructor0 101D5DA4
data lClothDestructor1 101D5DB0
data lClothDestructor2 101D5DBC
data lMathDestructor0 101D5D60
data lMathDestructor1 101D5D6C
data lMathDestructor2 101D5D78
data lClothHeaderFloatGuard 1046F078
data lClothHeaderByteGuard 1046F079
data lMathHeaderFloatGuard 1046F05C
data lMathHeaderByteGuard 1046F05D
data lViewMatrix 104B45F8
data lProjectionMatrix 104B470C
data lIdentityMatrix 101F48F0
data lGraphicsSystem 104B45C0
data lDefaultRenderState 104B474C
data lPacketVtable 1004B600
data lXluPacketVtable 1004B668
data lFlag03Vtable 1004B6D0
data lFlag04Vtable 1004B738
data lFlag05Vtable 1004B7A0
data lFlag07Vtable 1004B808
data lMaterialStringVtable 1004B594
data lXluStringVtable 1004B5A8
data lTransformVtable 1016EF84
data lLightingVtable 1016EFB4
str 1004B54C "" in 0251E964
str 1004B54C "" in 0251EB70
ext 0201B080 void clothApi_crossVector(u32,u32,u32)
data lFlag03Simulation 101D5D84
data lFlag04Simulation 101D5D8C
data lFlag05Simulation 101D5D94
data lFlag07Simulation 101D5D9C
data lFrameCounter 101FF560
data lOpaqueDrawBuffer 104B4634
data lGraphicsMutex 101F8B18
data lHeapManager 101F8B4C
data lViewResource 104B4708
data lIndexFormat 104B4A88
data lPrimitiveType 104B4A84
data lIndexData 104B4A8C
data lZeroVector 101FFBA8
data lBaseZ 101FFBCC
data lBaseXYZ 101FFBFC
data lMatrixStack 1048D0CC
data lSinCosTable 104A44F8
data lMathHeaderStatics 1046F060
data lClothHeaderStatics 1046F07C
data lMathHeaderFirst 1046F054
data lMathHeaderSecond 1046F058
data lClothHeaderFirst 1046F070
data lClothHeaderSecond 1046F074
data lFlagTuning100 1004B564
data lFlagTuning180 1004B5B4
data lFlagTuning184 1004B5B8
data lFlagTuning188 1004B5BC
data lFlagTuning192 1004B5C0
data lFlagTuning196 1004B5C4
data lFlagTuning200 1004B5C8
data lFlagTuning204 1004B5CC
data lFlagTuning208 1004B5D0
data lFlagTuning212 1004B5D4
data lFlagTuning216 1004B5D8
data lFlagTuning220 1004B5DC
data lFlagTuning224 1004B5E0
data lFlagTuning228 1004B5E4
data lFlagTuning232 1004B5E8
data lFlagTuning236 1004B5EC
data lFlagTuning240 1004B5F0
data lFlagTuning244 1004B5F4
data lTextureDepth 1004B590
data lXluDepth 1004B5A4
data lClothTuning2 1004B870
data lClothTuning3 1004B874
data lZero 1004B564
data lColorDivisor 1004B584
data lFlagFlyLength 1004B568
data lFlagHoistLength 1004B56C
data lLongFlagFlyLength 1004B570
data lLongFlagHoistLength 1004B574
data lClothTuning10 1004B5F8
data lClothTuning11 1004B5FC
data lWindWaveAdjust 1047BBC8
data lWindSpeedAdjust 1047BBC4
data lUnit 1004B580
data lGraphicsZero 10145180
data lGraphicsOne 1014517C
ext 0201ADE0 void clothApi_subtractVector(u32, u32, u32)
ext 0201AE48 void clothApi_scaleNormalByDotResult(u32, u32)
ext 0201B12C void clothApi_normalizeVector(u32, u32)
ext 0251B590 void clothApi_springFactor(u32, u32, u32, f64, f32)
ext 0251B638 void clothApi_setMatrix(u32, u32)
ext 0251B648 u32 clothApi_constructPacket(u32, u32, u32, u32, u32, u32, f32, f32)
ext 0251BD54 u32 clothApi_constructXluPacket(u32, u32, u32, u32, u32, u32, f32, f32)
ext 0251BEE8 u32 clothApi_constructFlag03(u32, u32, u32, u32)
ext 0251C078 u32 clothApi_constructFlag04(u32, u32, u32, u32)
ext 0251C214 u32 clothApi_constructFlag05(u32, u32, u32, u32)
ext 0251C3BC u32 clothApi_constructFlag07(u32, u32, u32, u32)
ext 0251C558 void clothApi_destroyPacket(u32, u32)
ext 0251C968 void clothApi_setNormals(u32)
ext 0251CEC8 void clothApi_initializePacket(u32)
ext 0251D128 void clothApi_getFactor(u32, u32, u32, u32, u32, u32, u32, f32, f32, f64)
ext 0251D4EC void clothApi_movePacket(u32)
ext 0251D7C4 void clothApi_snapshotMatrix(u32, u32)
ext 0251D864 void clothApi_updateVertices(u32)
ext 0251DD24 void clothApi_colorToFloat(u32, u32)
ext 0251DDE8 void clothApi_prepareDraw(u32)
ext 0251EE3C void clothApi_copyFlag03(u32)
ext 0251F188 void clothApi_copyFlag04(u32)
ext 0251F4C4 void clothApi_copyFlag05(u32)
ext 0251F808 void clothApi_copyFlag07(u32)
ext 0252F3B0 void clothApi_entryZSortXluDrawList(u32, u32, u32, u32)
ext 0255F8F4 void clothApi_setFogTevstr(u32)
ext 0255FE90 void clothApi_setLightTevstr(u32)
ext 025F1884 void clothApi_rotateMatrixY(u32, s32)
ext 025F2518 void clothApi_scaleMatrix(u32, f32, f32, f32)
ext 0273AD10 u32 clothApi_allocate(u32)
ext 0273AF40 void clothApi_deallocate(u32)
ext 0274D2AC void clothApi_scaleColorInPlace(u32, f32)
ext 0274D458 void clothApi_scaleColor(u32, u32, f32)
ext 0274FBF8 void clothApi_lockGraphics(u32)
ext 0274FCCC void clothApi_unlockGraphics(u32)
ext 02750250 void clothApi_constructRenderState(u32)
ext 02750370 void clothApi_applyRenderState(u32)
ext 02750684 void clothApi_applyCullMode(u32)
ext 02755FEC u32 clothApi_findContainingHeap(u32, u32)
ext 02773798 void clothApi_decodeTextureSurface(u32, u32)
ext 027B5CBC void clothApi_destroyVertexResource(u32, u32)
ext 027B90AC s32 clothApi_findMaterial(u32, u32)
ext 027B9F68 void clothApi_bindShaderProgram(u32)
ext 027BB7CC void clothApi_bindShader(u32)
ext 027BB9E0 void clothApi_updateShader(u32, u32)
ext 027BDEB4 void clothApi_copyTextureSurface(u32, u32)
ext 027BDF7C void clothApi_constructTexture(u32)
ext 027BE2B0 void clothApi_destroyTexture(u32, u32)
ext 027BE53C void clothApi_bindTexture(u32, u32, u32, u32)
ext 027BE6B8 void clothApi_constructSurfaceTransfer(u32)
ext 027BF7E8 void clothApi_releaseVertexBuffer(u32)
ext 027BF880 void clothApi_destroyVertexBuffer(u32, u32)
ext 027BFE5C void clothApi_bindVertexBuffer(u32)
ext 027F0E04 void clothApi_entryDrawBuffer(u32, u32, u32)
ext 027F1278 void clothApi_constructPacketBase(u32)
ext 027F13DC void clothApi_destroyPacketBase(u32, u32)
ext 027F29D4 u32 clothApi_getShaderCache(u32)
ext 027FB40C void clothApi_constructMaterialBlock(u32)
ext 027FB528 void clothApi_destroyMaterialBlock(u32, u32)
ext 027FD6F4 void clothApi_constructUniformBlock(u32)
ext 027FD764 void clothApi_destroyUniformBlock(u32, u32)
ext 027FDA54 void clothApi_setViewUniforms(u32, u32, u32, u32, u32)
ext 027FE084 void clothApi_allocateUniformBlock(u32, u32, u32)
ext 027FE0DC void clothApi_submitUniformBlock(u32, u32)
ext 027FE64C u32 clothApi_constructGeometry(u32)
ext 027FE6BC void clothApi_allocateGeometry(u32, u32, u32, u32)
ext 027FEB60 void clothApi_destroyGeometry(u32, u32)
ext 027FF078 void clothApi_allocateVertexBuffers(u32, u32, u32, u32)
ext 027FF1D8 void clothApi_flushVertexBuffers(u32, u32, u32)
ext 027FFCBC u32 clothApi_getMaterialManager()
ext 027FFE54 void clothApi_bindDrawMaterial(u32, u32)
ext 0280037C void clothApi_configureMaterialState(u32, u32)
ext 028003E8 void clothApi_initializeMaterialVertices(u32, u32, u32, u32)
ext 0280068C void clothApi_loadMaterialInstances(u32, u32, u32)
ext 02800B0C void clothApi_initializeMaterial(u32, u32, u32)
ext 028E8D88 void clothApi_addVector(u32, u32, u32)
ext 028E8DD0 f64 clothApi_squareMagnitude(u32)
ext 028E8E64 void clothApi_scaleVector(u32, u32, f32)
ext 028E8F44 f64 clothApi_dotVector(u32, u32)
ext 028E8F64 void clothApi_transformVector(u32, u32, u32)
ext 028E90D4 void clothApi_copyMatrix(u32, u32)
ext 028E91EC void clothApi_invertMatrix(u32, u32)
ext 028EAB2C void clothApi_initializeHeaderByte(u32)
ext 028ED6F8 void clothApi_initializeHeaderFloat(u32)
ext 028EFFD0 u32 clothApi_constructArray(u32, u32, u32, u32)
ext 028F026C void clothApi_registerDestructor(u32)
ext 028F4384 f64 clothApi_squareRootFloatInput(f32)
ext 028F4384 f64 clothApi_squareRoot(f64)
ext 028F521C void clothApi_clearMemory(u32, u32)
ext C00060E0 void clothApi_bindDisplayList(u32, u32)
ext C0006178 void clothApi_drawIndexed(u32, u32, u32, u32, u32, u32)
ext C00068A8 void clothApi_bindGeometryUniform(s32, u32, u32)
ext C0006900 void clothApi_bindPixelUniform(s32, u32, u32)
ext C0006A38 void clothApi_bindVertexUniform(s32, u32, u32)
ext C00088B8 void clothApi_storeRangeNoSync(u32, u32)
ext C000A848 void clothApi_copyMemory(u32, u32, u32)
*/
