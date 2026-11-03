// agl primitive shapes, second half of the translation unit (first half: agl/PrimitiveShapes.cpp): the
// set-up routines that fill the singleton's vertex and index buffers (quad, quad triangle, cube; circle,
// sphere and hemisphere with three levels of detail; cylinder, cone and a torus-knot tube with separate
// triangle and line indices), the index builders, the torus counts and vertices, the singleton's
// registered disposer and the storage constructors the first half passes to the array constructor.
// Written from WWHD's instructions; GX2 index and primitive enums: wut facts.
#include "agl/PrimitiveShapeBuilders.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager);
static void destroyRegistered(sead::RegisteredObject* object, u32 flags);
static void constructArray(void* objects, u32 count, u32 stride, u32 constructor);
static void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
static IndexBufferLayout* constructIndexBuffer(IndexBufferLayout* buffer);
static void destroyIndexBuffer(IndexBufferLayout* buffer, u32 flags);
static void normalizeVector(sead::RenderVector3* vector);
static void crossProduct(const sead::RenderVector3* a, const sead::RenderVector3* b, sead::RenderVector3* result);
static void transformPoint(const sead::RenderMatrix34* matrix, const sead::RenderVector3* point, sead::RenderVector3* result);
static void transformDirection(const sead::RenderMatrix34* matrix, const sead::RenderVector3* direction, sead::RenderVector3* result);
static Pair32 angleToIndex(f32 angle);
static f64 sineOf(f64 angle);
static f64 cosineOf(f64 angle);

// one record of sead's sine/cosine table: values and slopes, indexed by the top byte of an angle index
struct SineCosineSample { be<f32> mSine, mSineSlope, mCosine, mCosineSlope; };
struct SineCosineTable { SineCosineSample mSamples[256]; };
WWHD_SIZE(SineCosineSample, 16);
static SineCosineTable sSineCosineTable;
static sead::RenderMatrix34 sIdentityMatrix;
static sead::RenderVector3 sCurveExtent;

static const f32 kTwoPi = 6.2831854820251465f;

// The quad, the quad triangle and the cube: vertices (position, normal, texture coordinate) and indices.
static const f32 kQuadVertices[4][8] = {
    {-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
    {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
};
static const u16 kQuadTriangles[6] = {0, 2, 1, 1, 2, 3};
static const u16 kQuadOutline[4] = {0, 1, 3, 2};
static const f32 kQuadTriangleVertices[3][8] = {
    {-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    {1.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 0.0f},
    {-0.5f, -1.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f},
};
static const u16 kQuadTriangleIndices[3] = {0, 2, 1};
static const f32 kCubeVertices[24][8] = {
    {-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
};
// The cube's triangle and line indices (four vertices per face).
static const u16 kCubeTriangles[36] = {
    0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11,
    12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23,
};
static const u16 kCubeLines[48] = {
    0, 1, 0, 2, 3, 1, 3, 2, 4, 5, 4, 6, 7, 5, 7, 6, 8, 9, 8, 10, 11, 9, 11, 10,
    12, 13, 12, 14, 15, 13, 15, 14, 16, 17, 16, 18, 19, 17, 19, 18, 20, 21, 20, 22, 23, 21, 23, 22,
};

/* inlined into the set-up routines: an allocation from the given heap, or the current one */
static void* allocateFrom(sead::RegistryHeap* heap, u32 size, s32 alignment) {
    if (heap == nullptr) heap = currentHeap(sRegistryManager);
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, alignment);
}

/* inlined into the set-up routines: a storage of `count` elements (nothing for a count below 1 or a failed
 * allocation, which keeps the old storage) */
static void allocateStorage(ShapeStorage* storage, s32 count, u32 elementSize, s32 alignment, sead::RegistryHeap* heap) {
    if (count <= 0) return;
    void* data = allocateFrom(heap, count * elementSize, alignment);
    if (data == nullptr) return;
    storage->mSize = count;
    storage->mData = data;
}

/* inlined: element access of a storage; an index past its size gives the first element */
static PrimitiveVertex* vertexAt(ShapeStorage* storage, u32 index) {
    PrimitiveVertex* vertices = static_cast<PrimitiveVertex*>((void*)storage->mData);
    return index < storage->mSize ? &vertices[index] : vertices;
}
static be<u16>* indexAt(ShapeStorage* storage, u32 index) {
    be<u16>* indices = static_cast<be<u16>*>((void*)storage->mData);
    return index < storage->mSize ? &indices[index] : indices;
}

/* inlined: copies a table of vertices (position, normal, texture coordinate) into a storage */
static void copyVertices(ShapeStorage* storage, const f32 (*table)[8], u32 count) {
    for (u32 index = 0; index < count; ++index) {
        PrimitiveVertex* vertex = vertexAt(storage, index);
        vertex->mX = table[index][0];
        vertex->mY = table[index][1];
        vertex->mZ = table[index][2];
        vertex = vertexAt(storage, index);
        vertex->mNormalX = table[index][3];
        vertex->mNormalY = table[index][4];
        vertex->mNormalZ = table[index][5];
        vertex = vertexAt(storage, index);
        vertex->mU = table[index][6];
        vertex->mV = table[index][7];
    }
}
template <class Index> static void copyIndices(ShapeStorage* storage, const Index* table, u32 count) {
    for (u32 index = 0; index < count; ++index) *indexAt(storage, index) = table[index];
}

/* inlined into the index builders: index step of a level of detail (slw: shifts of 32..63 give 0) */
static u32 lodStep(u32 lod) {
    return (lod & 63) < 32 ? 1u << (lod & 31) : 0;
}
/* inlined into the torus index count (srw: shifts of 32..63 give 0) */
static u32 lodReduced(u32 count, u32 lod) {
    return (lod & 63) < 32 ? count >> (lod & 31) : 0;
}
/* inlined into the index builders: the next row, clamped to the last one (a signed compare) */
static u32 nextRow(u32 row, u32 step, u32 lastRow) {
    return (s32)lastRow >= (s32)(row + step) ? row + step : lastRow;
}

/* fmadds/fmsubs with a double operand (a libm result): the product rounds frC to 25 bits first */
static f32 multiplyAddFpr(f64 a, f64 c, f64 b) {
    return (f32)gabi::ppc_nan3(a, b, c, a * round25(c) + b, gabi::ppc_dnan64());
}
static f32 multiplySubtractFpr(f64 a, f64 c, f64 b) {
    return (f32)gabi::ppc_nan3(a, b, c, a * round25(c) - b, gabi::ppc_dnan64());
}
/* fsel: b where a >= 0, else c (a NaN selects c) */
static f32 selectNonNegative(f32 a, f32 b, f32 c) {
    return a >= 0.0f ? b : c;
}

/* 027A35AC */
u32 agl::torusVertexCount(u32 segments, u32 rings) {
    return segments * (rings + 1);
}

/* 027A35B8 */
u32 agl::torusIndexCount(u32 segments, u32 rings, u32 mode, u32 lod) {
    u32 perQuad = 4;
    if (mode == ShapeIndexMode_Triangles) perQuad = 6;
    return lodReduced(segments, lod) * lodReduced(rings, lod) * perQuad;
}

/* 027A35DC */
void agl::torusKnotPoint(sead::RenderVector3* point, s32 turns, s32 windings, f32 t, f32 scale) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 turnAngle = gabi::fmuls_ppc(gabi::fmuls_ppc((f32)turns, t), kTwoPi);
    f64 turnSine = sineOf(turnAngle);
    f64 turnCosine = cosineOf(turnAngle);
    f32 windAngle = gabi::fmuls_ppc(gabi::fmuls_ppc((f32)windings, t), kTwoPi);
    f64 windSine = sineOf(windAngle);
    f64 windCosine = cosineOf(windAngle);
    f32 radius = multiplyAddFpr(0.5f, turnCosine, 1.0f);
    f32 x = multiplySubtractFpr(radius, windCosine, 1.0f);
    f32 y = gabi::fmuls_fpr(radius, windSine);
    x = gabi::fmuls_ppc(x, scale);
    y = gabi::fmuls_ppc(y, scale);
    f32 z = gabi::fmuls_ppc(gabi::fmuls_fpr(0.5f, turnSine), scale);
    point->x = gabi::fmuls_ppc(x, 0.6666666865348816f);
    point->y = gabi::fmuls_ppc(y, 0.6666666865348816f);
    point->z = gabi::fmuls_ppc(z, 0.6666666865348816f);
}

/* 027A37A8 */
void agl::buildTorusVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 segments, u32 rings,
                             s32 turns, s32 windings, f32 scale, f32 tubeRadius) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = torusVertexCount(segments, rings);
    // The frame along the curve starts as the identity (every axis is set before it is used).
    // Stack objects at the original frame offsets (its callees see them there).
    gabi::FrameLocal<sead::RenderMatrix34> basis(0x158);
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column) basis->mRows[row][column] = sIdentityMatrix.mRows[row][column];

    // The curve's bounding box, starting inverted at the extent.
    f32 extentX = sCurveExtent.x, extentY = sCurveExtent.y, extentZ = sCurveExtent.z;
    f32 maximumX = gabi::fmuls_ppc(-extentX, scale);
    f32 maximumY = gabi::fmuls_ppc(-extentY, scale);
    f32 maximumZ = gabi::fmuls_ppc(-extentZ, scale);
    f32 minimumX = gabi::fmuls_ppc(extentX, scale);
    f32 minimumY = gabi::fmuls_ppc(extentY, scale);
    f32 minimumZ = gabi::fmuls_ppc(extentZ, scale);
    f32 ringStep = gabi::fdivs_ppc(1.0f, (f32)rings);
    gabi::FrameLocal<sead::RenderVector3> point(0xC8);
    for (u32 ring = 0; ring < rings; ++ring) {
        torusKnotPoint(point, turns, windings, gabi::fmuls_ppc((f32)ring, ringStep), scale);
        f32 x = point->x, y = point->y, z = point->z;
        maximumX = selectNonNegative(gabi::fsubs_ppc(x, maximumX), x, maximumX);
        maximumZ = selectNonNegative(gabi::fsubs_ppc(z, maximumZ), z, maximumZ);
        maximumY = selectNonNegative(gabi::fsubs_ppc(y, maximumY), y, maximumY);
        minimumZ = selectNonNegative(gabi::fsubs_ppc(z, minimumZ), minimumZ, z);
        minimumX = selectNonNegative(gabi::fsubs_ppc(x, minimumX), minimumX, x);
        minimumY = selectNonNegative(gabi::fsubs_ppc(y, minimumY), minimumY, y);
    }
    f32 centerX = gabi::fmuls_ppc(gabi::fadds_ppc(maximumX, minimumX), 0.5f);
    f32 centerY = gabi::fmuls_ppc(gabi::fadds_ppc(maximumY, minimumY), 0.5f);
    f32 centerZ = gabi::fmuls_ppc(gabi::fadds_ppc(maximumZ, minimumZ), 0.5f);

    // The direction into the curve's start comes from the point just before its end.
    gabi::FrameLocal<sead::RenderVector3> last(0x188);
    torusKnotPoint(last, turns, windings, gabi::fsubs_ppc(1.0f, ringStep), scale);
    f32 previousX = gabi::fsubs_ppc(last->x, centerX);
    f32 previousY = gabi::fsubs_ppc(last->y, centerY);
    f32 previousZ = gabi::fsubs_ppc(last->z, centerZ);

    gabi::FrameLocal<sead::RenderVector3> up(0x50);
    up->x = previousX;
    up->y = previousY;
    up->z = previousZ;
    gabi::FrameLocal<sead::RenderVector3> side(0x20);
    gabi::FrameLocal<sead::RenderVector3> current(0xD4);
    gabi::FrameLocal<sead::RenderVector3> offset(0x74);
    gabi::FrameLocal<sead::RenderVector3> tangent(0x98);
    gabi::FrameLocal<sead::RenderVector3> reversed(0xA4);
    gabi::FrameLocal<sead::RenderVector3> normal(0x5C);
    gabi::FrameLocal<sead::RenderVector3> crossed(0x104);
    gabi::FrameLocal<sead::RenderVector3> binormal(0x110);
    u32 cursor = 0;
    for (u32 ring = 0; ring != rings + 1; ++ring) {
        normalizeVector(up);
        torusKnotPoint(current, turns, windings, gabi::fmuls_ppc((f32)ring, ringStep), scale);
        offset->x = gabi::fsubs_ppc(current->x, centerX);
        offset->y = gabi::fsubs_ppc(current->y, centerY);
        offset->z = gabi::fsubs_ppc(current->z, centerZ);
        tangent->x = gabi::fsubs_ppc(offset->x, previousX);
        tangent->y = gabi::fsubs_ppc(offset->y, previousY);
        tangent->z = gabi::fsubs_ppc(offset->z, previousZ);
        normalizeVector(tangent);
        f32 tangentX = tangent->x, tangentY = tangent->y, tangentZ = tangent->z;
        side->x = up->x;
        side->y = up->y;
        side->z = up->z;
        reversed->x = -tangentX;
        reversed->y = -tangentY;
        reversed->z = -tangentZ;
        crossProduct(side, reversed, crossed);
        normal->x = crossed->x;
        normal->y = crossed->y;
        normal->z = crossed->z;
        normalizeVector(normal);
        crossProduct(tangent, normal, binormal);
        reversed->x = -binormal->x;
        reversed->y = -binormal->y;
        reversed->z = -binormal->z;
        side->x = reversed->x;
        side->y = reversed->y;
        side->z = reversed->z;
        setMatrixAxes(basis, normal, side, tangent);
        up->z = side->z;
        previousZ = offset->z;
        previousY = offset->y;
        up->y = side->y;
        previousX = offset->x;
        up->x = side->x;
        basis->setTranslation(offset);

        // One ring of the tube around the curve point.
        if (segments != 0) {
            f32 columns = (f32)segments;
            PrimitiveVertex* vertex = &vertices[cursor];
            cursor += segments;
            for (u32 segment = 0; segment < segments; ++segment, ++vertex) {
                f32 angle = gabi::fmuls_ppc(gabi::fdivs_ppc(gabi::fmuls_ppc((f32)segment, kTwoPi), columns), 683565248.0f);
                u32 index = angleToIndex(angle).r4;
                f32 fraction = gabi::fmuls_ppc((f32)(index & 0xFFFFFF), 5.960464477539063e-8f);
                SineCosineSample* sample = &sSineCosineTable.mSamples[index >> 24];
                f32 cosine = gabi::fmadds(sample->mCosineSlope, fraction, sample->mCosine);
                f32 sine = gabi::fmadds(sample->mSineSlope, fraction, sample->mSine);
                vertex->mX = gabi::fmuls_ppc(cosine, tubeRadius);
                vertex->mY = gabi::fmuls_ppc(sine, tubeRadius);
                vertex->mZ = 0.0f;
                sead::RenderVector3* position = reinterpret_cast<sead::RenderVector3*>(&vertex->mX);
                transformPoint(basis, position, position);
                f32 x = vertex->mX;
                vertex->mNormalY = sine;
                f32 u = gabi::fadds_ppc(x, 0.5f);
                f32 y = vertex->mY;
                vertex->mNormalZ = 0.0f;
                f32 v = gabi::fadds_ppc(y, 0.5f);
                vertex->mU = u;
                vertex->mNormalX = cosine;
                vertex->mV = v;
                sead::RenderVector3* direction = reinterpret_cast<sead::RenderVector3*>(&vertex->mNormalX);
                transformDirection(basis, direction, direction);
            }
        }
    }
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* 027A4050 */
void agl::setUpQuad(PrimitiveShapes* shapes, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    IndexedShape* shape = &shapes->mIndexedShapes[IndexedShape_Quad];
    allocateStorage(&shape->mVertices, 4, sizeof(PrimitiveVertex), 4, heap);
    copyVertices(&shape->mVertices, kQuadVertices, 4);
    shape->mVertexBuffer.setStorage(shape->mVertices.mData, sizeof(PrimitiveVertex), 4 * sizeof(PrimitiveVertex));
    setVertexAttributes(&shape->mVertexBuffer);
    allocateStorage(&shape->mIndices, 6, sizeof(u16), 0x20, heap);
    copyIndices(&shape->mIndices, kQuadTriangles, 6);
    shape->mIndexBuffer.setStorage(shape->mIndices.mData, GX2IndexType_U16, 6);
    allocateStorage(&shape->mOutlineIndices, 4, sizeof(u16), 0x20, heap);
    copyIndices(&shape->mOutlineIndices, kQuadOutline, 4);
    shape->mOutlineIndexBuffer.setStorage(shape->mOutlineIndices.mData, GX2IndexType_U16, 4);
    shape->mOutlineIndexBuffer.mPrimitiveMode = GX2PrimitiveMode_LineLoop;
}

/* 027A4420 */
void agl::setUpQuadTriangle(PrimitiveShapes* shapes, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    IndexedShape* shape = &shapes->mIndexedShapes[IndexedShape_QuadTriangle];
    allocateStorage(&shape->mVertices, 3, sizeof(PrimitiveVertex), 4, heap);
    copyVertices(&shape->mVertices, kQuadTriangleVertices, 3);
    shape->mVertexBuffer.setStorage(shape->mVertices.mData, sizeof(PrimitiveVertex), 3 * sizeof(PrimitiveVertex));
    setVertexAttributes(&shape->mVertexBuffer);
    allocateStorage(&shape->mIndices, 3, sizeof(u16), 0x20, heap);
    copyIndices(&shape->mIndices, kQuadTriangleIndices, 3);
    shape->mIndexBuffer.setStorage(shape->mIndices.mData, GX2IndexType_U16, 3);
    allocateStorage(&shape->mOutlineIndices, 3, sizeof(u16), 0x20, heap);
    copyIndices(&shape->mOutlineIndices, kQuadTriangleIndices, 3);
    shape->mOutlineIndexBuffer.setStorage(shape->mOutlineIndices.mData, GX2IndexType_U16, 3);
    shape->mOutlineIndexBuffer.mPrimitiveMode = GX2PrimitiveMode_LineLoop;
}

/* 027A47BC */
void agl::setUpCube(PrimitiveShapes* shapes, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    IndexedShape* shape = &shapes->mIndexedShapes[IndexedShape_Cube];
    allocateStorage(&shape->mVertices, 24, sizeof(PrimitiveVertex), 4, heap);
    copyVertices(&shape->mVertices, kCubeVertices, 24);
    shape->mVertexBuffer.setStorage(shape->mVertices.mData, sizeof(PrimitiveVertex), 24 * sizeof(PrimitiveVertex));
    setVertexAttributes(&shape->mVertexBuffer);
    allocateStorage(&shape->mIndices, 36, sizeof(u16), 4, heap);
    copyIndices(&shape->mIndices, kCubeTriangles, 36);
    shape->mIndexBuffer.setStorage(shape->mIndices.mData, GX2IndexType_U16, 36);
    allocateStorage(&shape->mOutlineIndices, 48, sizeof(u16), 0x20, heap);
    copyIndices(&shape->mOutlineIndices, kCubeLines, 48);
    shape->mOutlineIndexBuffer.setStorage(shape->mOutlineIndices.mData, GX2IndexType_U16, 48);
    shape->mOutlineIndexBuffer.mPrimitiveMode = GX2PrimitiveMode_Lines;
}

/* 027A4DF8 */
void agl::setUpCircle(PrimitiveShapes* shapes, u32 segments, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    LodShape* shape = &shapes->mLodShapes[LodShape_Circle];
    allocateStorage(&shape->mVertices, circleVertexCount(segments), sizeof(PrimitiveVertex), 4, heap);
    buildCircleVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), segments);
    for (u32 lod = 0; lod < 3; ++lod) {
        for (u32 mode = 0; mode < 3; ++mode) {
            s32 count = indexCount(segments, mode, lod);
            ShapeStorage* storage = &shape->mIndices[lod].mLevels[mode];
            allocateStorage(storage, count, sizeof(u16), 4, heap);
            buildCircleIndices(&shape->mIndexBuffers[lod].mLevels[mode], static_cast<be<u16>*>((void*)storage->mData),
                               segments, mode, lod);
        }
    }
}

/* 027A5000 */
void agl::setUpSphere(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    LodShape* shape = &shapes->mLodShapes[LodShape_Sphere];
    allocateStorage(&shape->mVertices, sphereVertexCount(slices, stacks), sizeof(PrimitiveVertex), 4, heap);
    buildSphereVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), slices, stacks);
    for (u32 lod = 0; lod < 3; ++lod) {
        for (u32 mode = 0; mode < 3; ++mode) {
            s32 count = sphereIndexCount(slices, stacks, mode, lod);
            ShapeStorage* storage = &shape->mIndices[lod].mLevels[mode];
            allocateStorage(storage, count, sizeof(u16), 4, heap);
            buildSphereIndices(&shape->mIndexBuffers[lod].mLevels[mode], static_cast<be<u16>*>((void*)storage->mData),
                               slices, stacks, mode, lod);
        }
    }
}

/* 027A5220 */
void agl::setUpHemisphere(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    LodShape* shape = &shapes->mLodShapes[LodShape_Hemisphere];
    allocateStorage(&shape->mVertices, hemisphereVertexCount(slices, stacks), sizeof(PrimitiveVertex), 4, heap);
    buildHemisphereVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), slices, stacks);
    for (u32 lod = 0; lod < 3; ++lod) {
        for (u32 mode = 0; mode < 3; ++mode) {
            s32 count = hemisphereIndexCount(slices, stacks, mode, lod);
            ShapeStorage* storage = &shape->mIndices[lod].mLevels[mode];
            allocateStorage(storage, count, sizeof(u16), 4, heap);
            buildHemisphereIndices(&shape->mIndexBuffers[lod].mLevels[mode], static_cast<be<u16>*>((void*)storage->mData),
                                   slices, stacks, mode, lod);
        }
    }
}

/* 027A5440 */
void agl::setUpCylinder(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DualLodShape* shape = &shapes->mDualLodShapes[DualLodShape_Cylinder];
    allocateStorage(&shape->mVertices, cylinderVertexCount(slices, stacks), sizeof(PrimitiveVertex), 4, heap);
    buildCylinderVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), slices, stacks);
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = cylinderIndexCount(slices, stacks, ShapeIndexMode_Triangles, lod);
        allocateStorage(&shape->mIndices[lod], count, sizeof(u16), 4, heap);
        buildCylinderIndices(&shape->mIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mIndices[lod].mData),
                             slices, stacks, ShapeIndexMode_Triangles, lod);
    }
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = cylinderIndexCount(slices, stacks, ShapeIndexMode_Lines, lod);
        allocateStorage(&shape->mOutlineIndices[lod], count, sizeof(u16), 4, heap);
        buildCylinderIndices(&shape->mOutlineIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mOutlineIndices[lod].mData),
                             slices, stacks, ShapeIndexMode_Lines, lod);
    }
}

/* 027A575C */
void agl::setUpCone(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DualLodShape* shape = &shapes->mDualLodShapes[DualLodShape_Cone];
    allocateStorage(&shape->mVertices, coneVertexCount(slices, stacks), sizeof(PrimitiveVertex), 4, heap);
    buildConeVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), slices, stacks);
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = coneIndexCount(slices, stacks, ShapeIndexMode_Triangles, lod);
        allocateStorage(&shape->mIndices[lod], count, sizeof(u16), 4, heap);
        buildConeIndices(&shape->mIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mIndices[lod].mData),
                         slices, stacks, ShapeIndexMode_Triangles, lod);
    }
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = coneIndexCount(slices, stacks, ShapeIndexMode_Lines, lod);
        allocateStorage(&shape->mOutlineIndices[lod], count, sizeof(u16), 4, heap);
        buildConeIndices(&shape->mOutlineIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mOutlineIndices[lod].mData),
                         slices, stacks, ShapeIndexMode_Lines, lod);
    }
}

/* 027A5A78 */
void agl::setUpTorus(PrimitiveShapes* shapes, u32 segments, u32 rings, sead::RegistryHeap* heap, f32 scale,
                     f32 tubeRadius, s32 turns, s32 windings) {
    gabi::AutoFrame frame{gabi::current_function()};
    DualLodShape* shape = &shapes->mDualLodShapes[DualLodShape_Torus];
    allocateStorage(&shape->mVertices, torusVertexCount(segments, rings), sizeof(PrimitiveVertex), 4, heap);
    buildTorusVertices(&shape->mVertexBuffer, static_cast<PrimitiveVertex*>((void*)shape->mVertices.mData), segments, rings,
                       turns, windings, scale, tubeRadius);
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = torusIndexCount(segments, rings, ShapeIndexMode_Triangles, lod);
        allocateStorage(&shape->mIndices[lod], count, sizeof(u16), 4, heap);
        buildTorusIndices(&shape->mIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mIndices[lod].mData),
                          segments, rings, ShapeIndexMode_Triangles, lod);
    }
    for (u32 lod = 0; lod < 3; ++lod) {
        s32 count = torusIndexCount(segments, rings, ShapeIndexMode_Lines, lod);
        allocateStorage(&shape->mOutlineIndices[lod], count, sizeof(u16), 4, heap);
        buildTorusIndices(&shape->mOutlineIndexBuffers[lod], static_cast<be<u16>*>((void*)shape->mOutlineIndices[lod].mData),
                          segments, rings, ShapeIndexMode_Lines, lod);
    }
}

/* 027A5DF0 */
void agl::initializePrimitiveShapes(PrimitiveShapes* shapes, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    setUpQuad(shapes, heap);
    setUpQuadTriangle(shapes, heap);
    setUpCube(shapes, heap);
    setUpCircle(shapes, 32, heap);
    setUpSphere(shapes, 32, 16, heap);
    setUpHemisphere(shapes, 32, 16, heap);
    setUpCylinder(shapes, 32, 16, heap);
    setUpCone(shapes, 32, 80, heap);
    setUpTorus(shapes, 32, 32, heap, 0.3333333432674408f, 0.1666666716337204f, 0, 1);
}

/* 027A5ECC */
void agl::destroyPrimitiveShapesDisposer(PrimitiveShapesDisposer* disposer, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (disposer == nullptr) return;
    disposer->mVtable = &sShapesDisposerMethods;
    if (disposer == sPrimitiveShapesDisposer.get()) {
        PrimitiveShapes* shapes = sPrimitiveShapes;
        sPrimitiveShapesDisposer = nullptr;
        shapes->destroy(2);
        sPrimitiveShapes = nullptr;
    }
    destroyRegistered(disposer, 0);
    if (flags & 1) releaseObject(disposer);
}

/* 027A5F68 */
ShapeStorage* ShapeStorage::create(ShapeStorage* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage == nullptr) {
        storage = static_cast<ShapeStorage*>(allocateObject(sizeof(ShapeStorage)));
        if (storage == nullptr) return storage;
    }
    storage->mData = nullptr;
    storage->mSize = 0;
    return storage;
}

/* 027A5FA8 */
ShapeLodStorage* ShapeLodStorage::create(ShapeLodStorage* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage == nullptr) {
        storage = static_cast<ShapeLodStorage*>(allocateObject(sizeof(ShapeLodStorage)));
        if (storage == nullptr) return storage;
    }
    constructArray(storage, 3, sizeof(ShapeStorage), gabi::guest_fn(reinterpret_cast<const void*>(&ShapeStorage::create)));
    return storage;
}

/* 027A6000 */
ShapeLodIndexBuffers* ShapeLodIndexBuffers::create(ShapeLodIndexBuffers* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage == nullptr) {
        storage = static_cast<ShapeLodIndexBuffers*>(allocateObject(sizeof(ShapeLodIndexBuffers)));
        if (storage == nullptr) return storage;
    }
    constructArray(storage, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)constructIndexBuffer));
    return storage;
}

/* 027A6058 */
void ShapeLodIndexBuffers::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    destroyArray(this, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)destroyIndexBuffer), 0, 0);
    if (flags & 1) releaseObject(this);
}

/* 027A60C0 */
void agl::setMatrixAxes(sead::RenderMatrix34* matrix, const sead::RenderVector3* x, const sead::RenderVector3* y, const sead::RenderVector3* z) {
    gabi::AutoFrame frame{gabi::current_function()};
    matrix->setColumn(0, x);
    matrix->setColumn(1, y);
    matrix->setColumn(2, z);
}

/* 027A612C */
void agl::buildCircleIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 segments, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = 0;
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 segment = 0; segment < segments; segment += lodStep(lod)) {
            indices[count++] = segments;
            indices[count++] = segment;
            indices[count++] = (segment + lodStep(lod)) % segments;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 segment = 0; segment < segments; segment += lodStep(lod)) {
            indices[count++] = segments;
            indices[count++] = segment;
            indices[count++] = segment;
            indices[count++] = (segment + lodStep(lod)) % segments;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    case ShapeIndexMode_Points:
        for (u32 segment = 0; segment < segments; segment += lodStep(lod)) indices[count++] = segment;
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Points;
        break;
    }
}

/* 027A62B0 */
void agl::buildSphereIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = 0;
    u32 step = lodStep(lod);
    u32 lastRow = stacks - 1;
    u32 topPole = slices * stacks;
    u32 bottomPole = slices * stacks + 1;
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = topPole;
            indices[count++] = slice;
            indices[count++] = (slice + step) % slices;
        }
        for (u32 row = 0; row < lastRow; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                u32 corner = row * slices + slice;
                u32 below = next * slices + slice % slices;
                u32 beside = row * slices + (slice + step) % slices;
                u32 diagonal = next * slices + (slice + step) % slices;
                indices[count++] = corner;
                indices[count++] = below;
                indices[count++] = beside;
                indices[count++] = below;
                indices[count++] = diagonal;
                indices[count++] = beside;
            }
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = bottomPole;
            indices[count++] = slices * lastRow + (slice + step) % slices;
            indices[count++] = slices * lastRow + slice;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = topPole;
            indices[count++] = slice;
            indices[count++] = bottomPole;
            indices[count++] = slices * lastRow + (slice + step) % slices;
            indices[count++] = slices * lastRow + (slice + step) % slices;
            indices[count++] = slices * lastRow + slice;
        }
        for (u32 row = 0; row < lastRow; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = row * slices + slice;
                indices[count++] = row * slices + (slice + step) % slices;
                indices[count++] = row * slices + slice;
                indices[count++] = next * slices + slice % slices;
            }
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    case ShapeIndexMode_Points:
        for (u32 row = 0; row < stacks; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = row * slices + (slice + step) % slices;
                indices[count++] = next * slices + slice % slices;
            }
        }
        indices[count++] = topPole;
        indices[count++] = bottomPole;
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Points;
        break;
    }
}

/* 027A6724 */
void agl::buildHemisphereIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = 0;
    u32 rows = (stacks >> 1) + 1;
    u32 step = lodStep(lod);
    u32 lastRow = rows - 1;
    u32 pole = slices * rows;
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = pole;
            indices[count++] = (slice + step) % slices;
            indices[count++] = slice;
        }
        for (u32 row = 0; row < lastRow; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                u32 corner = row * slices + slice;
                u32 beside = row * slices + (slice + step) % slices;
                u32 below = next * slices + slice % slices;
                u32 diagonal = next * slices + (slice + step) % slices;
                indices[count++] = corner;
                indices[count++] = beside;
                indices[count++] = below;
                indices[count++] = below;
                indices[count++] = beside;
                indices[count++] = diagonal;
            }
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = pole;
            indices[count++] = slice;
            indices[count++] = slices * lastRow + (slice + step) % slices;
            indices[count++] = slices * lastRow + slice;
        }
        for (u32 row = 0; row < lastRow; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = row * slices + slice;
                indices[count++] = row * slices + (slice + step) % slices;
                indices[count++] = row * slices + slice;
                indices[count++] = next * slices + slice % slices;
            }
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    case ShapeIndexMode_Points:
        for (u32 row = 0; row < rows; row += step) {
            u32 next = nextRow(row, step, lastRow);
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = row * slices + (slice + step) % slices;
                indices[count++] = next * slices + slice % slices;
            }
        }
        indices[count++] = pole;
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Points;
        break;
    }
}

/* 027A6B00 */
void agl::buildCylinderIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = cylinderVertexCount(slices, stacks);
    u32 count = 0;
    u32 step = lodStep(lod);
    u32 topRing = slices + 1;
    u32 bottomRing = slices * (stacks + 1) + 1;
    u32 bottomCenter = vertexCount - 1;
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = 1 + slice;
            indices[count++] = 0;
            indices[count++] = (slice + step) % slices + 1;
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            u32 next = (slice + step) % slices;
            indices[count++] = topRing + slice;
            indices[count++] = topRing + next;
            indices[count++] = bottomRing + slice;
            indices[count++] = bottomRing + slice;
            indices[count++] = topRing + next;
            indices[count++] = bottomRing + next;
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = vertexCount - step - 1 - slice;
            indices[count++] = bottomCenter;
            indices[count++] = vertexCount - step - 1 - (slice + step) % slices;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 row = 1; row <= stacks + 1; row += step) {
            u32 ring = slices * row + 1;
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = ring + slice;
                indices[count++] = ring + (slice + step) % slices;
            }
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = 0;
            indices[count++] = 1 + slice;
            indices[count++] = topRing + slice;
            indices[count++] = bottomRing + slice;
            indices[count++] = slices * (stacks + 2) + 1 + slice;
            indices[count++] = bottomCenter;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    }
}

/* 027A6E28 */
void agl::buildConeIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = coneVertexCount(slices, stacks);
    u32 count = 0;
    u32 step = lodStep(lod);
    u32 bottomCenter = vertexCount - 1;
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = slices + slice;
            indices[count++] = slice;
            indices[count++] = (slice + step) % slices + slices;
        }
        for (u32 row = 1; row < stacks; row += step) {
            u32 next = nextRow(row, step, stacks);
            for (u32 slice = 0; slice < slices; slice += step) {
                u32 below = next * slices + slice;
                u32 corner = row * slices + slice;
                u32 beside = row * slices + (slice + step) % slices;
                indices[count++] = below;
                indices[count++] = corner;
                indices[count++] = beside;
                indices[count++] = below;
                indices[count++] = beside;
                indices[count++] = next * slices + (slice + step) % slices;
            }
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = vertexCount - step - 1 - slice;
            indices[count++] = bottomCenter;
            indices[count++] = vertexCount - step - 1 - (slice + step) % slices;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 row = step; row <= stacks; row += step) {
            for (u32 slice = 0; slice < slices; slice += step) {
                indices[count++] = row * slices + slice;
                indices[count++] = row * slices + (slice + step) % slices;
            }
        }
        for (u32 slice = 0; slice < slices; slice += step) {
            indices[count++] = slice;
            indices[count++] = slices * stacks + slice;
            indices[count++] = slices * (stacks + 1) + slice;
            indices[count++] = bottomCenter;
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    }
}

/* 027A7144 */
void agl::buildTorusIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 segments, u32 rings, u32 mode, u32 lod) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = 0;
    u32 step = lodStep(lod);
    switch (mode) {
    case ShapeIndexMode_Triangles:
        for (u32 ring = 0; ring < rings; ring += step) {
            u32 base = ring * segments;
            u32 nextBase = (ring + step) * segments;
            for (u32 segment = 0; segment < segments; segment += step) {
                u32 next = (segment + step) % segments;
                indices[count++] = base + next;
                indices[count++] = base + segment;
                indices[count++] = nextBase + segment;
                indices[count++] = base + next;
                indices[count++] = nextBase + segment;
                indices[count++] = nextBase + next;
            }
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Triangles;
        break;
    case ShapeIndexMode_Lines:
        for (u32 ring = 0; ring < rings; ring += step) {
            u32 base = ring * segments;
            u32 nextBase = (ring + step) * segments;
            for (u32 segment = 0; segment < segments; segment += step) {
                indices[count++] = base + segment;
                indices[count++] = base + (segment + step) % segments;
                indices[count++] = base + segment;
                indices[count++] = nextBase + segment;
            }
        }
        buffer->setStorage(indices, GX2IndexType_U16, count);
        buffer->mPrimitiveMode = GX2PrimitiveMode_Lines;
        break;
    }
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object)
ext 02756140 sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager)
ext 02752BEC void destroyRegistered(sead::RegisteredObject* object, u32 flags)
ext 028EFFD0 void constructArray(void* objects, u32 count, u32 stride, u32 constructor)
ext 028F0164 void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 027B5430 agl::IndexBufferLayout* constructIndexBuffer(IndexBufferLayout* buffer) callback
ext 027B54A0 void destroyIndexBuffer(IndexBufferLayout* buffer, u32 flags) callback
ext 027B54E0 void agl::IndexBufferLayout::setStorage(void* storage, u32 format, u32 count)
ext 027B5CFC void agl::VertexBufferLayout::setStorage(void* storage, u32 stride, u32 bytes)
ext 027A14FC void agl::PrimitiveShapes::destroy(u32 flags)
ext 027A1DFC void agl::setVertexAttributes(VertexBufferLayout* buffer)
ext 027A1E68 u32 agl::circleVertexCount(u32 segments)
ext 027A1E70 u32 agl::indexCount(u32 count, u32 mode, u32 lod)
ext 027A1EAC void agl::buildCircleVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 segments)
ext 027A20B8 u32 agl::sphereVertexCount(u32 slices, u32 stacks)
ext 027A20C4 u32 agl::sphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod)
ext 027A21F8 void agl::buildSphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks)
ext 027A25F8 u32 agl::hemisphereVertexCount(u32 slices, u32 stacks)
ext 027A260C u32 agl::hemisphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod)
ext 027A2748 void agl::buildHemisphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks)
ext 027A2B08 u32 agl::cylinderVertexCount(u32 slices, u32 stacks)
ext 027A2B18 u32 agl::cylinderIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod)
ext 027A2C08 void agl::buildCylinderVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks)
ext 027A308C u32 agl::coneVertexCount(u32 slices, u32 stacks)
ext 027A309C u32 agl::coneIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod)
ext 027A31A0 void agl::buildConeVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks)
ext 025155D8 void normalizeVector(sead::RenderVector3* vector)
ext 028E8D4C void crossProduct(const sead::RenderVector3* a, const sead::RenderVector3* b, sead::RenderVector3* result)
ext 028E8F64 void transformPoint(const sead::RenderMatrix34* matrix, const sead::RenderVector3* point, sead::RenderVector3* result)
ext 028E9044 void transformDirection(const sead::RenderMatrix34* matrix, const sead::RenderVector3* direction, sead::RenderVector3* result)
ext 02750CDC void sead::RenderMatrix34::setColumn(u32 column, const RenderVector3* vector)
ext 02750D00 void sead::RenderMatrix34::setTranslation(const RenderVector3* vector)
ext 028F5680 Pair32 angleToIndex(f32 angle)
ext 028F43F8 f64 sineOf(f64 angle)
ext 028F4BE0 f64 cosineOf(f64 angle)
data sSineCosineTable 10145FA0
data sIdentityMatrix 104A041C
data sCurveExtent 104A0958
data kCubeTriangles 101680DC
data kCubeLines 10168124
*/
