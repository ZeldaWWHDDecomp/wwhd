// agl primitive shapes: the shape-buffer and vertex-input singletons (construction, binding of the inputs
// to the shapes' vertex buffers, teardown) and the vertex/index counts and vertex generation of circles,
// spheres, hemispheres, cylinders and cones. Written from WWHD's instructions; GX2 attribute formats: wut facts.
#include "agl/PrimitiveShapes.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void destroyRegistered(sead::RegisteredObject* object, u32 flags);
static void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
static sead::RegistryHeap* findHeap(sead::RegistryManagerState* manager, void* allocation);
static void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection);
static void constructArray(void* objects, u32 count, u32 stride, u32 constructor);
static void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
static VertexInputSet* constructInputSet(VertexInputSet* input);
static void destroyInputSet(VertexInputSet* input, u32 flags);
static VertexBufferLayout* constructVertexBuffer(VertexBufferLayout* buffer);
static IndexBufferLayout* constructIndexBuffer(IndexBufferLayout* buffer);
static void destroyIndexBuffer(IndexBufferLayout* buffer, u32 flags);
static void destroyLodIndexBuffers(ShapeLodIndexBuffers* buffers, u32 flags);
static void normalizeVector(sead::RenderVector3* vector);
static f64 cosineOf(f64 angle);
static f64 sineOf(f64 angle);
static sead::DisposerVtable sInputsMethods;
static sead::DisposerVtable sInputsDisposerMethods;
static sead::DisposerVtable sShapesMethods;

/* inlined into the constructors: placement construction of a member array */
static void constructMemberArray(void* objects, u32 count, u32 stride, u32 constructor) {
    if (objects == nullptr) objects = allocateObject(count * stride);
    if (objects != nullptr) constructArray(objects, count, stride, constructor);
}

/* inlined into the shapes constructor: an empty storage */
static void initializeStorage(ShapeStorage* storage) {
    if (storage == nullptr) storage = static_cast<ShapeStorage*>(allocateObject(sizeof(ShapeStorage)));
    if (storage != nullptr) { storage->mData = nullptr; storage->mSize = 0; }
}

/* 027A0A08 */
PrimitiveShapeInputs* PrimitiveShapeInputs::create(PrimitiveShapeInputs* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveShapeInputs* inputs = storage;
    if (inputs == nullptr) inputs = static_cast<PrimitiveShapeInputs*>(allocateObject(sizeof(PrimitiveShapeInputs)));
    if (inputs != nullptr) {
        inputs->mMethods = &sInputsMethods;
        constructMemberArray(inputs->mInputs, kInputCount, sizeof(VertexInputSet), gabi::guest_fn((const void*)constructInputSet));
    }
    return inputs;
}

/* 027A0A80 */
PrimitiveShapeInputs* PrimitiveShapeInputs::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveShapeInputs* inputs = sPrimitiveShapeInputs;
    if (inputs != nullptr) return inputs;
    inputs = static_cast<PrimitiveShapeInputs*>(allocateInHeap(sizeof(PrimitiveShapeInputs), heap, 4));
    if (inputs != nullptr) {
        registerInHeap(&inputs->mDisposer, heap, 3);
        inputs->mDisposer.mVtable = &sInputsDisposerMethods;
    }
    sPrimitiveShapeInputsDisposer = inputs == nullptr ? nullptr : &inputs->mDisposer;
    if (inputs != nullptr) inputs = create(inputs);
    sPrimitiveShapeInputs = inputs;
    return inputs;
}

/* 027A0B14 */
void PrimitiveShapeInputs::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    destroyArray(mInputs, kInputCount, sizeof(VertexInputSet), gabi::guest_fn((const void*)destroyInputSet), 0, 0);
    if (flags & 1) releaseObject(this);
}

/* inlined into initialize: one input set over a shape's vertex buffer (position, plus one more attribute) */
static void bindShape(VertexInputSet* input, sead::RegistryHeap* heap, VertexBufferLayout* shape, bool second, u32 attribute) {
    VertexBufferPrefix_l* source = reinterpret_cast<VertexBufferPrefix_l*>(shape);
    input->allocate(1, heap);
    input->setBinding(0, source, PrimitiveAttribute_Position);
    if (second) input->setBinding(1, source, attribute);
    input->prepare();
}

/* 027A0B7C */
void PrimitiveShapeInputs::initialize(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveShapes* shapes = sPrimitiveShapes;
    bindShape(&mInputs[6], heap, &shapes->mIndexedShapes[0].mVertexBuffer, false, 0);
    bindShape(&mInputs[1], heap, &shapes->mIndexedShapes[2].mVertexBuffer, true, PrimitiveAttribute_Normal);
    bindShape(&mInputs[2], heap, &shapes->mDualLodShapes[1].mVertexBuffer, true, PrimitiveAttribute_Normal);
    bindShape(&mInputs[3], heap, &shapes->mDualLodShapes[0].mVertexBuffer, true, PrimitiveAttribute_Normal);
    bindShape(&mInputs[5], heap, &shapes->mLodShapes[1].mVertexBuffer, true, PrimitiveAttribute_Normal);
    bindShape(&mInputs[7], heap, &shapes->mIndexedShapes[1].mVertexBuffer, true, PrimitiveAttribute_TexCoord);
    bindShape(&mInputs[8], heap, &shapes->mLodShapes[0].mVertexBuffer, true, PrimitiveAttribute_TexCoord);
    bindShape(&mInputs[0], heap, &shapes->mIndexedShapes[2].mVertexBuffer, false, 0);
    bindShape(&mInputs[4], heap, &shapes->mLodShapes[1].mVertexBuffer, false, 0);
}

/* 027A0DEC */
void PrimitiveShapeInputsDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sInputsDisposerMethods;
    if (this == sPrimitiveShapeInputsDisposer.get()) {
        PrimitiveShapeInputs* inputs = sPrimitiveShapeInputs;
        sPrimitiveShapeInputsDisposer = nullptr;
        gabi::call_ptr<void>(inputs->mMethods->mDestroy.mFunction, inputs, 2);
        sPrimitiveShapeInputs = nullptr;
    }
    destroyRegistered(this, 0);
    if (flags & 1) releaseObject(this);
}

/* 027A0E94 */
PrimitiveShapes* PrimitiveShapes::create(PrimitiveShapes* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveShapes* shapes = storage;
    if (shapes == nullptr) shapes = static_cast<PrimitiveShapes*>(allocateObject(sizeof(PrimitiveShapes)));
    if (shapes == nullptr) return shapes;
    shapes->mMethods = &sShapesMethods;
    for (u32 index = 0; index < kShapeCount; ++index) {
        IndexedShape* shape = &shapes->mIndexedShapes[index];
        initializeStorage(&shape->mVertices);
        initializeStorage(&shape->mIndices);
        initializeStorage(&shape->mOutlineIndices);
        constructVertexBuffer(&shape->mVertexBuffer);
        constructIndexBuffer(&shape->mIndexBuffer);
        constructIndexBuffer(&shape->mOutlineIndexBuffer);
    }
    for (u32 index = 0; index < kShapeCount; ++index) {
        LodShape* shape = &shapes->mLodShapes[index];
        initializeStorage(&shape->mVertices);
        constructMemberArray(shape->mIndices, 3, sizeof(ShapeLodStorage),
            gabi::guest_fn(reinterpret_cast<const void*>(&ShapeLodStorage::create)));
        constructVertexBuffer(&shape->mVertexBuffer);
        constructMemberArray(shape->mIndexBuffers, 3, sizeof(ShapeLodIndexBuffers),
            gabi::guest_fn(reinterpret_cast<const void*>(&ShapeLodIndexBuffers::create)));
    }
    for (u32 index = 0; index < kShapeCount; ++index) {
        DualLodShape* shape = &shapes->mDualLodShapes[index];
        initializeStorage(&shape->mVertices);
        constructMemberArray(shape->mIndices, 3, sizeof(ShapeStorage), gabi::guest_fn(reinterpret_cast<const void*>(&ShapeStorage::create)));
        constructMemberArray(shape->mOutlineIndices, 3, sizeof(ShapeStorage),
            gabi::guest_fn(reinterpret_cast<const void*>(&ShapeStorage::create)));
        constructVertexBuffer(&shape->mVertexBuffer);
        constructMemberArray(shape->mIndexBuffers, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)constructIndexBuffer));
        constructMemberArray(shape->mOutlineIndexBuffers, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)constructIndexBuffer));
    }
    return shapes;
}

/* 027A145C */
PrimitiveShapes* PrimitiveShapes::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    PrimitiveShapes* shapes = sPrimitiveShapes;
    if (shapes != nullptr) return shapes;
    shapes = static_cast<PrimitiveShapes*>(allocateInHeap(sizeof(PrimitiveShapes), heap, 4));
    // GHS tests the member address itself (also for a failed allocation).
    PrimitiveShapesDisposer* disposer = gabi::at<PrimitiveShapesDisposer>(gabi::ea(shapes) + offsetof(PrimitiveShapes, mDisposer));
    if (disposer != nullptr) {
        registerInHeap(disposer, heap, 3);
        disposer->mVtable = &sShapesDisposerMethods;
    }
    sPrimitiveShapesDisposer = disposer;
    if (shapes != nullptr) shapes = create(shapes);
    sPrimitiveShapes = shapes;
    return shapes;
}

/* inlined into the shapes destructor: free a storage's heap block */
static void releaseStorage(ShapeStorage* storage) {
    void* data = storage->mData;
    if (data == nullptr) return;
    sead::RegistryHeap* heap = findHeap(sRegistryManager, data);
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage->mData.get());
    storage->mSize = 0;
    storage->mData = nullptr;
}

/* 027A14FC */
void PrimitiveShapes::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sShapesMethods;
    for (u32 index = 0; index < kShapeCount; ++index) releaseStorage(&mIndexedShapes[index].mVertices);
    releaseStorage(&mLodShapes[1].mVertices);
    releaseStorage(&mLodShapes[2].mVertices);
    for (u32 index = 0; index < kShapeCount; ++index) releaseStorage(&mDualLodShapes[index].mVertices);
    releaseStorage(&mLodShapes[0].mVertices);
    for (u32 index = 0; index < kShapeCount; ++index) {
        releaseStorage(&mIndexedShapes[index].mIndices);
        releaseStorage(&mIndexedShapes[index].mOutlineIndices);
    }
    for (u32 level = 0; level < 3; ++level) {
        for (u32 index = 0; index < kShapeCount; ++index) {
            releaseStorage(&mDualLodShapes[index].mIndices[level]);
            releaseStorage(&mDualLodShapes[index].mOutlineIndices[level]);
        }
    }
    for (u32 set = 0; set < 3; ++set) {
        for (u32 level = 0; level < 3; ++level) {
            releaseStorage(&mLodShapes[1].mIndices[set].mLevels[level]);
            releaseStorage(&mLodShapes[2].mIndices[set].mLevels[level]);
            releaseStorage(&mLodShapes[0].mIndices[set].mLevels[level]);
        }
    }
    for (s32 index = kShapeCount - 1; index >= 0; --index) {
        destroyArray(mDualLodShapes[index].mOutlineIndexBuffers, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)destroyIndexBuffer), 0, 0);
        destroyArray(mDualLodShapes[index].mIndexBuffers, 3, sizeof(IndexBufferLayout), gabi::guest_fn((const void*)destroyIndexBuffer), 0, 0);
        mDualLodShapes[index].mVertexBuffer.destroy(2);
    }
    for (s32 index = kShapeCount - 1; index >= 0; --index) {
        destroyArray(mLodShapes[index].mIndexBuffers, 3, sizeof(ShapeLodIndexBuffers), gabi::guest_fn((const void*)destroyLodIndexBuffers), 0, 0);
        mLodShapes[index].mVertexBuffer.destroy(2);
    }
    for (s32 index = kShapeCount - 1; index >= 0; --index) {
        destroyIndexBuffer(&mIndexedShapes[index].mOutlineIndexBuffer, 2);
        destroyIndexBuffer(&mIndexedShapes[index].mIndexBuffer, 2);
        mIndexedShapes[index].mVertexBuffer.destroy(2);
    }
    if (flags & 1) releaseObject(this);
}

/* 027A1E68 */
u32 agl::circleVertexCount(u32 segments) { return segments + 1; }

/* 027A20B8 */
u32 agl::sphereVertexCount(u32 slices, u32 stacks) { return slices * stacks + 2; }

/* 027A25F8 */
u32 agl::hemisphereVertexCount(u32 slices, u32 stacks) { return slices * ((stacks >> 1) + 1) + 1; }

/* 027A2B08 */
u32 agl::cylinderVertexCount(u32 slices, u32 stacks) { return slices * (stacks + 3) + 2; }

/* 027A308C */
u32 agl::coneVertexCount(u32 slices, u32 stacks) { return slices * (stacks + 2) + 1; }

/* 027A1E70 */
u32 agl::indexCount(u32 count, u32 mode, u32 lod) {
    u32 reduced = (lod & 63) < 32 ? count >> (lod & 31) : 0;
    switch (mode) {
    case 0: return reduced * 3;
    case 1: return reduced * 4;
    case 2: return reduced;
    default: return 0;
    }
}

/* inlined into the primitive index-count methods */
static u32 lodStep(u32 lod) {
    return (lod & 63) < 32 ? 1u << (lod & 31) : 0;
}

/* inlined into the primitive index-count methods */
static void countRow(u32* count, u32 columns, u32 step, u32 increment) {
    for (u32 column = 0; column < columns; column += step) *count += increment;
}

/* 027A20C4 */
u32 agl::sphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod) {
    u32 count = 0;
    u32 step = lodStep(lod);
    switch (mode) {
    case 0:
        countRow(&count, slices, step, 6);
        for (u32 row = 0; row < stacks - 1; row += step) countRow(&count, slices, step, 6);
        break;
    case 1:
        countRow(&count, slices, step, 6);
        for (u32 row = 0; row < stacks - 1; row += step) countRow(&count, slices, step, 4);
        break;
    case 2:
        for (u32 row = 0; row < stacks; row += step) countRow(&count, slices, step, 2);
        count += 2;
        break;
    }
    return count;
}

/* 027A260C */
u32 agl::hemisphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod) {
    u32 rows = (stacks >> 1) + 1;
    u32 count = 0;
    u32 step = lodStep(lod);
    switch (mode) {
    case 0:
        countRow(&count, slices, step, 3);
        for (u32 row = 0; row < rows - 1; row += step) countRow(&count, slices, step, 6);
        break;
    case 1:
        countRow(&count, slices, step, 4);
        for (u32 row = 0; row < rows - 1; row += step) countRow(&count, slices, step, 4);
        break;
    case 2:
        for (u32 row = 0; row < rows; row += step) countRow(&count, slices, step, 2);
        count += 1;
        break;
    }
    return count;
}

/* 027A2B18 */
u32 agl::cylinderIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod) {
    u32 count = 0;
    u32 step = lodStep(lod);
    switch (mode) {
    case 0:
        countRow(&count, slices, step, 3);
        countRow(&count, slices, step, 6);
        countRow(&count, slices, step, 3);
        break;
    case 1:
        for (u32 row = 1; row <= stacks + 1; row += step) countRow(&count, slices, step, 2);
        countRow(&count, slices, step, 6);
        break;
    }
    return count;
}

/* 027A309C */
u32 agl::coneIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod) {
    u32 count = 0;
    u32 step = lodStep(lod);
    switch (mode) {
    case 0:
        countRow(&count, slices, step, 3);
        for (u32 row = 1; row < stacks; row += step) countRow(&count, slices, step, 6);
        countRow(&count, slices, step, 3);
        break;
    case 1:
        for (u32 row = 1; row <= stacks; row += step) countRow(&count, slices, step, 2);
        countRow(&count, slices, step, 4);
        break;
    }
    return count;
}

/* 027A1DFC */
void agl::setVertexAttributes(VertexBufferLayout* buffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    buffer->setAttribute(PrimitiveAttribute_Position, GX2AttribFormat_Float32x3, offsetof(PrimitiveVertex, mX), 0);
    buffer->setAttribute(PrimitiveAttribute_Normal, GX2AttribFormat_Float32x3, offsetof(PrimitiveVertex, mNormalX), 0);
    buffer->setAttribute(PrimitiveAttribute_TexCoord, GX2AttribFormat_Float32x2, offsetof(PrimitiveVertex, mU), 0);
}

/* 027A1EAC */
void agl::buildCircleVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 segments) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = circleVertexCount(segments);
    if (segments != 0) {
        f32 count = static_cast<f32>(segments);
        for (u32 index = 0; index < segments; ++index) {
            f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(index)), count);
            f64 cosine = cosineOf(angle);
            f32 x = gabi::fmuls_fpr(cosine, 0.5f);
            f64 sine = sineOf(angle);
            f32 y = gabi::fmuls_fpr(sine, 0.5f);
            PrimitiveVertex* vertex = &vertices[index];
            vertex->mX = x;
            vertex->mY = y;
            f32 v = gabi::fsubs_ppc(0.5f, y);
            vertex->mZ = 0;
            vertex->mNormalY = 0;
            f32 u = gabi::fadds_ppc(x, 0.5f);
            vertex->mV = v;
            vertex->mNormalZ = 1;
            vertex->mU = u;
            vertex->mNormalX = 0;
        }
    }
    PrimitiveVertex* center = &vertices[segments];
    center->mX = 0;
    center->mY = 0;
    center->mZ = 0;
    center->mNormalY = 0;
    center->mNormalZ = 1;
    center->mU = 0.5f;
    center->mV = 0.5f;
    center->mNormalX = 0;
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* 027A21F8 */
void agl::buildSphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = sphereVertexCount(slices, stacks);
    if (stacks != 0) {
        f32 rows = gabi::fadds_ppc(static_cast<f32>(stacks), 1);
        u32 base = 0;
        for (u32 row = 0; row < stacks; ++row) {
            f32 t = gabi::fdivs_ppc(static_cast<f32>(row + 1), rows);
            f32 latitude = gabi::fmuls_ppc(gabi::fsubs_ppc(t, 0.5f), 3.1415927410125732f);
            f64 sine = sineOf(latitude);
            f32 y = gabi::fmuls_fpr(sine, 0.5f);
            f64 cosine = cosineOf(latitude);
            f32 radius = gabi::fmuls_fpr(cosine, 0.5f);
            if (slices != 0) {
                f32 columns = static_cast<f32>(slices);
                for (u32 column = 0; column < slices; ++column) {
                    f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
                    f64 ringCosine = cosineOf(angle);
                    f32 x = gabi::fmuls_fpr(ringCosine, radius);
                    f64 ringSine = sineOf(angle);
                    f32 z = gabi::fmuls_fpr(ringSine, radius);
                    PrimitiveVertex* vertex = &vertices[base + column];
                    vertex->mX = x;
                    f32 v = gabi::fadds_ppc(z, 0.5f);
                    vertex->mY = y;
                    f32 u = gabi::fadds_ppc(x, 0.5f);
                    vertex->mV = v;
                    vertex->mU = u;
                    // GHS reads back the stored position before computing its radial normal.
                    f32 storedX = vertex->mX, storedY = vertex->mY;
                    f32 normalX = gabi::fmuls_ppc(storedX, 2), normalY = gabi::fmuls_ppc(storedY, 2);
                    vertex->mZ = z;
                    f32 storedZ = vertex->mZ;
                    f32 normalZ = gabi::fadds_ppc(storedZ, storedZ);
                    vertex->mNormalX = normalX;
                    vertex->mNormalY = normalY;
                    vertex->mNormalZ = normalZ;
                }
            }
            base += slices;
        }
    }
    u32 pole = slices * stacks;
    PrimitiveVertex* lower = &vertices[pole];
    lower->mX = 0;
    lower->mY = -0.5f;
    f32 lowerX = lower->mX, lowerY = lower->mY;
    lower->mU = 0.5f;
    f32 lowerNormalX = gabi::fmuls_ppc(lowerX, 2), lowerNormalY = gabi::fmuls_ppc(lowerY, 2);
    lower->mZ = 0;
    lower->mV = 0.5f;
    f32 lowerZ = lower->mZ;
    lower->mNormalX = lowerNormalX;
    lower->mNormalY = lowerNormalY;
    lower->mNormalZ = gabi::fadds_ppc(lowerZ, lowerZ);
    PrimitiveVertex* upper = &vertices[pole + 1];
    upper->mX = 0;
    upper->mY = 0.5f;
    f32 upperX = upper->mX, upperY = upper->mY;
    upper->mV = 0.5f;
    f32 upperNormalX = gabi::fmuls_ppc(upperX, 2), upperNormalY = gabi::fmuls_ppc(upperY, 2);
    upper->mZ = 0;
    upper->mU = 0.5f;
    f32 upperZ = upper->mZ;
    upper->mNormalX = upperNormalX;
    upper->mNormalY = upperNormalY;
    upper->mNormalZ = gabi::fadds_ppc(upperZ, upperZ);
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* 027A2748 */
void agl::buildHemisphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = hemisphereVertexCount(slices, stacks);
    u32 rows = (stacks >> 1) + 1;
    f32 denominator = gabi::fadds_ppc(static_cast<f32>(stacks), 1);
    f32 latitudeStep = gabi::fdivs_ppc(3.1415927410125732f, denominator);
    u32 base = 0;
    for (u32 row = 1, remaining = rows; remaining != 0; --remaining, ++row) {
        f32 latitude = gabi::fnmsubs(latitudeStep, static_cast<f32>(row), 1.5707963705062866f);
        latitude = latitude >= 0 ? latitude : 0;
        f64 sine = sineOf(latitude);
        f32 y = gabi::fmuls_fpr(sine, 0.5f);
        f64 cosine = cosineOf(latitude);
        f32 radius = gabi::fmuls_fpr(cosine, 0.5f);
        if (slices != 0) {
            f32 columns = static_cast<f32>(slices);
            for (u32 column = 0; column < slices; ++column) {
                f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
                f64 ringCosine = cosineOf(angle);
                f32 x = gabi::fmuls_fpr(ringCosine, radius);
                f64 ringSine = sineOf(angle);
                f32 z = gabi::fmuls_fpr(ringSine, radius);
                PrimitiveVertex* vertex = &vertices[base + column];
                vertex->mX = x;
                f32 u = gabi::fadds_ppc(x, 0.5f);
                vertex->mY = y;
                f32 v = gabi::fadds_ppc(z, 0.5f);
                f32 storedX = vertex->mX, storedY = vertex->mY;
                vertex->mU = u;
                vertex->mV = v;
                f32 normalX = gabi::fmuls_ppc(storedX, 2), normalY = gabi::fmuls_ppc(storedY, 2);
                vertex->mZ = z;
                f32 storedZ = vertex->mZ;
                f32 normalZ = gabi::fadds_ppc(storedZ, storedZ);
                vertex->mNormalX = normalX;
                vertex->mNormalY = normalY;
                vertex->mNormalZ = normalZ;
            }
        }
        base += slices;
    }
    PrimitiveVertex* pole = &vertices[slices * rows];
    pole->mY = 0.5f;
    pole->mX = 0;
    f32 poleX = pole->mX, poleY = pole->mY;
    pole->mU = 0.5f;
    f32 normalX = gabi::fmuls_ppc(poleX, 2), normalY = gabi::fmuls_ppc(poleY, 2);
    pole->mZ = 0;
    pole->mV = 0.5f;
    f32 poleZ = pole->mZ;
    pole->mNormalX = normalX;
    pole->mNormalY = normalY;
    pole->mNormalZ = gabi::fadds_ppc(poleZ, poleZ);
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* 027A2C08 */
void agl::buildCylinderVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = cylinderVertexCount(slices, stacks);
    vertices[0].mV = 0.5f;
    vertices[0].mY = 0.5f;
    vertices[0].mZ = 0;
    vertices[0].mNormalY = 1;
    vertices[0].mNormalZ = 0;
    vertices[0].mX = 0;
    vertices[0].mU = 0.5f;
    vertices[0].mNormalX = 0;
    u32 cursor = 1;
    if (slices != 0) {
        f32 columns = static_cast<f32>(slices);
        for (u32 column = 0; column < slices; ++column) {
            f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
            f64 cosine = cosineOf(angle);
            f32 x = gabi::fmuls_fpr(cosine, 0.5f);
            f64 sine = sineOf(angle);
            PrimitiveVertex* vertex = &vertices[cursor + column];
            vertex->mX = x;
            f32 z = gabi::fmuls_fpr(sine, 0.5f);
            vertex->mY = 0.5f;
            f32 u = gabi::fadds_ppc(x, 0.5f);
            vertex->mZ = z;
            f32 v = gabi::fadds_ppc(z, 0.5f);
            vertex->mNormalY = 1;
            vertex->mU = u;
            vertex->mV = v;
            vertex->mNormalX = 0;
            vertex->mNormalZ = 0;
        }
        cursor += slices;
    }
    f32 rows = static_cast<f32>(stacks);
    for (u32 row = 0; row <= stacks; ++row) {
        f32 y = gabi::fsubs_ppc(0.5f, gabi::fdivs_ppc(static_cast<f32>(row), rows));
        if (slices != 0) {
            f32 columns = static_cast<f32>(slices);
            f32 v = gabi::fadds_ppc(y, 0.5f);
            for (u32 column = 0; column < slices; ++column) {
                f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
                f64 cosine = cosineOf(angle);
                f32 x = gabi::fmuls_fpr(cosine, 0.5f);
                f64 sine = sineOf(angle);
                f32 u = gabi::fadds_ppc(x, 0.5f);
                PrimitiveVertex* vertex = &vertices[cursor + column];
                vertex->mX = x;
                f32 normalX = gabi::fadds_ppc(x, x);
                vertex->mV = v;
                vertex->mU = u;
                f32 z = gabi::fmuls_fpr(sine, 0.5f);
                vertex->mNormalX = normalX;
                vertex->mY = y;
                vertex->mZ = z;
                f32 normalZ = gabi::fadds_ppc(z, z);
                vertex->mNormalY = 0;
                vertex->mNormalZ = normalZ;
            }
            cursor += slices;
        }
    }
    if (slices != 0) {
        f32 columns = static_cast<f32>(slices);
        for (u32 column = 0; column < slices; ++column) {
            f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
            f64 cosine = cosineOf(angle);
            f32 x = gabi::fmuls_fpr(cosine, 0.5f);
            f64 sine = sineOf(angle);
            f32 z = gabi::fmuls_fpr(sine, 0.5f);
            PrimitiveVertex* vertex = &vertices[cursor + column];
            vertex->mX = x;
            vertex->mY = -0.5f;
            vertex->mZ = z;
            f32 u = gabi::fadds_ppc(x, 0.5f);
            vertex->mNormalY = -1;
            vertex->mNormalZ = 0;
            f32 v = gabi::fadds_ppc(z, 0.5f);
            vertex->mU = u;
            vertex->mNormalX = 0;
            vertex->mV = v;
        }
        cursor += slices;
    }
    PrimitiveVertex* bottom = &vertices[cursor];
    bottom->mX = 0;
    bottom->mY = -0.5f;
    bottom->mZ = 0;
    bottom->mNormalY = -1;
    bottom->mNormalZ = 0;
    bottom->mU = 0.5f;
    bottom->mNormalX = 0;
    bottom->mV = 0.5f;
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* 027A31A0 */
void agl::buildConeVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 vertexCount = coneVertexCount(slices, stacks);
    u32 cursor = 0;
    f32 rows = static_cast<f32>(stacks);
    for (u32 row = 0; row <= stacks; ++row) {
        f32 y = gabi::fsubs_ppc(0.5f, gabi::fdivs_ppc(static_cast<f32>(row), rows));
        f32 radius = gabi::fmuls_ppc(0.5f, gabi::fsubs_ppc(0.5f, y));
        if (slices != 0) {
            f32 columns = static_cast<f32>(slices);
            for (u32 column = 0; column < slices; ++column) {
                f32 segment = static_cast<f32>(column);
                if (row == 0) segment = gabi::fadds_ppc(segment, 0.5f);
                f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, segment), columns);
                f64 cosine = cosineOf(angle);
                f32 x = gabi::fmuls_fpr(cosine, radius);
                f64 sine = sineOf(angle);
                f32 z = gabi::fmuls_fpr(sine, radius);
                PrimitiveVertex* vertex = &vertices[cursor + column];
                vertex->mX = x;
                vertex->mY = y;
                vertex->mZ = z;
                f32 v = gabi::fadds_ppc(z, 0.5f);
                vertex->mNormalY = 0.5f;
                f32 u = gabi::fadds_ppc(x, 0.5f);
                vertex->mNormalZ = z;
                vertex->mV = v;
                vertex->mU = u;
                vertex->mNormalX = x;
                normalizeVector(reinterpret_cast<sead::RenderVector3*>(&vertex->mNormalX));
            }
            cursor += slices;
        }
    }
    if (slices != 0) {
        f32 columns = static_cast<f32>(slices);
        for (u32 column = 0; column < slices; ++column) {
            f32 angle = gabi::fdivs_ppc(gabi::fmuls_ppc(6.2831854820251465f, static_cast<f32>(column)), columns);
            f64 cosine = cosineOf(angle);
            f32 x = gabi::fmuls_fpr(cosine, 0.5f);
            f64 sine = sineOf(angle);
            f32 z = gabi::fmuls_fpr(sine, 0.5f);
            PrimitiveVertex* vertex = &vertices[cursor + column];
            vertex->mX = x;
            vertex->mY = -0.5f;
            vertex->mZ = z;
            f32 u = gabi::fadds_ppc(x, 0.5f);
            vertex->mNormalY = -1;
            vertex->mNormalZ = 0;
            f32 v = gabi::fadds_ppc(z, 0.5f);
            vertex->mU = u;
            vertex->mNormalX = 0;
            vertex->mV = v;
        }
        cursor += slices;
    }
    PrimitiveVertex* bottom = &vertices[cursor];
    bottom->mX = 0;
    bottom->mY = -0.5f;
    bottom->mZ = 0;
    bottom->mNormalY = -1;
    bottom->mNormalZ = 0;
    bottom->mU = 0.5f;
    bottom->mV = 0.5f;
    bottom->mNormalX = 0;
    buffer->setStorage(vertices, sizeof(PrimitiveVertex), vertexCount * sizeof(PrimitiveVertex));
    setVertexAttributes(buffer);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object)
ext 0273B0D4 void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment)
ext 02755FEC sead::RegistryHeap* findHeap(sead::RegistryManagerState* manager, void* allocation)
ext 02752B0C void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection)
ext 02752BEC void destroyRegistered(sead::RegisteredObject* object, u32 flags)
ext 028EFFD0 void constructArray(void* objects, u32 count, u32 stride, u32 constructor)
ext 028F0164 void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 027BF734 agl::VertexInputSet* constructInputSet(VertexInputSet* input) callback
ext 027BF880 void destroyInputSet(VertexInputSet* input, u32 flags) callback
ext 027BF960 void agl::VertexInputSet::allocate(s32 count, sead::RegistryHeap* heap)
ext 027BFB60 void agl::VertexInputSet::setBinding(s32 location, VertexBufferPrefix_l* source, s32 attribute)
ext 027BFC74 void agl::VertexInputSet::prepare()
ext 027B5BD8 agl::VertexBufferLayout* constructVertexBuffer(VertexBufferLayout* buffer)
ext 027B5CBC void agl::VertexBufferLayout::destroy(u32 flags)
ext 027B5CFC void agl::VertexBufferLayout::setStorage(void* storage, u32 stride, u32 bytes)
ext 027B5D60 void agl::VertexBufferLayout::setAttribute(u32 index, u32 format, u32 offset, u32 bufferIndex)
ext 027B5430 agl::IndexBufferLayout* constructIndexBuffer(IndexBufferLayout* buffer) callback
ext 027B54A0 void destroyIndexBuffer(IndexBufferLayout* buffer, u32 flags) callback
ext 027A5F68 agl::ShapeStorage* agl::ShapeStorage::create(ShapeStorage* storage) callback
ext 027A5FA8 agl::ShapeLodStorage* agl::ShapeLodStorage::create(ShapeLodStorage* storage) callback
ext 027A6000 agl::ShapeLodIndexBuffers* agl::ShapeLodIndexBuffers::create(ShapeLodIndexBuffers* storage) callback
ext 027A6058 void destroyLodIndexBuffers(ShapeLodIndexBuffers* buffers, u32 flags) callback
ext 025155D8 void normalizeVector(sead::RenderVector3* vector)
ext 028F4BE0 f64 cosineOf(f64 angle)
ext 028F43F8 f64 sineOf(f64 angle)
data sInputsMethods 10168050
data sInputsDisposerMethods 10168040
data sShapesMethods 10168088
*/
