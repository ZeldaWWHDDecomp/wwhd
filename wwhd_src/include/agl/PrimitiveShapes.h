// agl primitive shapes: the singleton (pointer 101F8DE4) holding the vertex and index buffers of the
// library's built-in shapes, the singleton (101F8DDC) of nine vertex input sets bound to them, and the
// vertex/index count and vertex generation routines for circles, spheres, hemispheres, cylinders and cones.
// The translation unit continues into AGL#11 (its index builders and the shapes' disposer).
#pragma once
#include "agl/VertexBufferLayout.h"
#include "agl/IndexBufferLayout.h"
#include "agl/VertexInput.h"
#include "sead/HeapRegistry.h"
#include "sead/RenderMath.h"
namespace agl {

// One vertex of the generated shapes: position, normal, texture coordinate.
struct PrimitiveVertex {
    be<f32> mX, mY, mZ;
    be<f32> mNormalX, mNormalY, mNormalZ;
    be<f32> mU, mV;
};

// heap storage of a shape's vertices or indices
struct ShapeStorage {
    be<u32> mSize;
    gptr<void> mData;
    static ShapeStorage* create(ShapeStorage* storage);
};
struct ShapeLodStorage {
    ShapeStorage mLevels[3];
    static ShapeLodStorage* create(ShapeLodStorage* storage);
};
struct ShapeLodIndexBuffers {
    IndexBufferLayout mLevels[3];
    static ShapeLodIndexBuffers* create(ShapeLodIndexBuffers* storage);
    void destroy(u32 flags);
};

// a shape with one vertex buffer and two index buffers
struct IndexedShape {
    ShapeStorage mVertices;
    ShapeStorage mIndices;
    ShapeStorage mOutlineIndices;
    VertexBufferLayout mVertexBuffer;
    IndexBufferLayout mIndexBuffer;
    IndexBufferLayout mOutlineIndexBuffer;
};
// a shape whose index buffers come in three levels of detail
struct LodShape {
    ShapeStorage mVertices;
    ShapeLodStorage mIndices[3];
    VertexBufferLayout mVertexBuffer;
    ShapeLodIndexBuffers mIndexBuffers[3];
};
// a shape with two sets of three index buffers
struct DualLodShape {
    ShapeStorage mVertices;
    ShapeStorage mIndices[3];
    ShapeStorage mOutlineIndices[3];
    VertexBufferLayout mVertexBuffer;
    IndexBufferLayout mIndexBuffers[3];
    IndexBufferLayout mOutlineIndexBuffers[3];
};

struct PrimitiveShapesDisposer : sead::RegisteredObject {};

struct PrimitiveShapes {
    static constexpr u32 kShapeCount = 3;
    gptr<sead::DisposerVtable> mMethods;
    PrimitiveShapesDisposer mDisposer;
    IndexedShape mIndexedShapes[kShapeCount];
    LodShape mLodShapes[kShapeCount];
    DualLodShape mDualLodShapes[kShapeCount];
    static PrimitiveShapes* create(PrimitiveShapes* storage);
    static PrimitiveShapes* instance(sead::RegistryHeap* heap);
    void destroy(u32 flags);
};

struct PrimitiveShapeInputsDisposer : sead::RegisteredObject { void destroy(u32 flags); };

// nine vertex input sets over the shapes' vertex buffers (position only, with normal, with texture coordinate)
struct PrimitiveShapeInputs {
    static constexpr u32 kInputCount = 9;
    PrimitiveShapeInputsDisposer mDisposer;
    VertexInputSet mInputs[kInputCount];
    gptr<sead::DisposerVtable> mMethods;
    static PrimitiveShapeInputs* create(PrimitiveShapeInputs* storage);
    static PrimitiveShapeInputs* instance(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void initialize(sead::RegistryHeap* heap);
};

// vertex attributes of PrimitiveVertex (GX2 attribute formats, wut facts)
enum : u32 { GX2AttribFormat_Float32x2 = 0x80D, GX2AttribFormat_Float32x3 = 0x811 };
enum : u32 { PrimitiveAttribute_Position = 0, PrimitiveAttribute_Normal = 1, PrimitiveAttribute_TexCoord = 2 };

void setVertexAttributes(VertexBufferLayout* buffer);
u32 circleVertexCount(u32 segments);
u32 sphereVertexCount(u32 slices, u32 stacks);
u32 hemisphereVertexCount(u32 slices, u32 stacks);
u32 cylinderVertexCount(u32 slices, u32 stacks);
u32 coneVertexCount(u32 slices, u32 stacks);
u32 indexCount(u32 count, u32 mode, u32 lod);
u32 sphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod);
u32 hemisphereIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod);
u32 cylinderIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod);
u32 coneIndexCount(u32 slices, u32 stacks, u32 mode, u32 lod);
void buildCircleVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 segments);
void buildSphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks);
void buildHemisphereVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks);
void buildCylinderVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks);
void buildConeVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 slices, u32 stacks);
}
// The singletons, their registered disposers and the shapes disposer's method table (one per translation-unit
// half before), owned by agl/PrimitiveShapesStatics.cpp.
extern gptr<agl::PrimitiveShapeInputs>& sPrimitiveShapeInputs;
extern gptr<agl::PrimitiveShapeInputsDisposer>& sPrimitiveShapeInputsDisposer;
extern gptr<agl::PrimitiveShapes>& sPrimitiveShapes;
extern gptr<agl::PrimitiveShapesDisposer>& sPrimitiveShapesDisposer;
extern sead::DisposerVtable& sShapesDisposerMethods;
WWHD_OFFSET(agl::PrimitiveVertex, mNormalX, 12);
WWHD_OFFSET(agl::PrimitiveVertex, mU, 24);
WWHD_SIZE(agl::PrimitiveVertex, 32);
WWHD_SIZE(agl::ShapeStorage, 8);
WWHD_SIZE(agl::ShapeLodStorage, 0x18);
WWHD_SIZE(agl::ShapeLodIndexBuffers, 0x48);
WWHD_OFFSET(agl::IndexedShape, mVertexBuffer, 0x18);
WWHD_OFFSET(agl::IndexedShape, mIndexBuffer, 0x16C);
WWHD_SIZE(agl::IndexedShape, 0x19C);
WWHD_OFFSET(agl::LodShape, mVertexBuffer, 0x50);
WWHD_OFFSET(agl::LodShape, mIndexBuffers, 0x1A4);
WWHD_SIZE(agl::LodShape, 0x27C);
WWHD_OFFSET(agl::DualLodShape, mVertexBuffer, 0x38);
WWHD_OFFSET(agl::DualLodShape, mIndexBuffers, 0x18C);
WWHD_SIZE(agl::DualLodShape, 0x21C);
WWHD_OFFSET(agl::PrimitiveShapes, mDisposer, 4);
WWHD_OFFSET(agl::PrimitiveShapes, mIndexedShapes, 0x14);
WWHD_OFFSET(agl::PrimitiveShapes, mLodShapes, 0x4E8);
WWHD_OFFSET(agl::PrimitiveShapes, mDualLodShapes, 0xC5C);
WWHD_SIZE(agl::PrimitiveShapes, 0x12B0);
WWHD_OFFSET(agl::PrimitiveShapeInputs, mInputs, 0x10);
WWHD_OFFSET(agl::PrimitiveShapeInputs, mMethods, 0x8A4);
WWHD_SIZE(agl::PrimitiveShapeInputs, 0x8A8);
