// agl primitive shapes, second half of the translation unit (the first half is agl/PrimitiveShapes.cpp):
// the torus vertex/index counts and the torus-knot curve, the index builders of the circle, sphere,
// hemisphere, cylinder, cone and torus, the shapes' set-up routines that allocate and fill every vertex
// and index buffer, the singleton's registered disposer and the small storage constructors.
#pragma once
#include "agl/PrimitiveShapes.h"
namespace agl {

// What the index builders produce (GX2 facts, wut): the primitive mode stored after the buffer is set up.
enum : u32 {
    GX2IndexType_U16 = 4,
    GX2PrimitiveMode_Points = 1,
    GX2PrimitiveMode_Lines = 2,
    GX2PrimitiveMode_Triangles = 4,
    GX2PrimitiveMode_LineLoop = 0x12,
};
// The index kinds of the levelled shapes: filled triangles, wire lines, points.
enum : u32 { ShapeIndexMode_Triangles = 0, ShapeIndexMode_Lines = 1, ShapeIndexMode_Points = 2 };

// shapes of PrimitiveShapes, in member order
enum : u32 { IndexedShape_Quad = 0, IndexedShape_QuadTriangle = 1, IndexedShape_Cube = 2 };
enum : u32 { LodShape_Circle = 0, LodShape_Sphere = 1, LodShape_Hemisphere = 2 };
enum : u32 { DualLodShape_Cylinder = 0, DualLodShape_Cone = 1, DualLodShape_Torus = 2 };

u32 torusVertexCount(u32 segments, u32 rings);
u32 torusIndexCount(u32 segments, u32 rings, u32 mode, u32 lod);
void torusKnotPoint(sead::RenderVector3* point, s32 turns, s32 windings, f32 t, f32 scale);
void buildTorusVertices(VertexBufferLayout* buffer, PrimitiveVertex* vertices, u32 segments, u32 rings,
                        s32 turns, s32 windings, f32 scale, f32 tubeRadius);
void setMatrixAxes(sead::RenderMatrix34* matrix, const sead::RenderVector3* x, const sead::RenderVector3* y, const sead::RenderVector3* z);
void buildCircleIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 segments, u32 mode, u32 lod);
void buildSphereIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod);
void buildHemisphereIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod);
void buildCylinderIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod);
void buildConeIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 slices, u32 stacks, u32 mode, u32 lod);
void buildTorusIndices(IndexBufferLayout* buffer, be<u16>* indices, u32 segments, u32 rings, u32 mode, u32 lod);

// The set-up routines of the singleton (methods of PrimitiveShapes, written as functions on it because
// PrimitiveShapes.h belongs to the merged first half of the unit).
void initializePrimitiveShapes(PrimitiveShapes* shapes, sead::RegistryHeap* heap);
void setUpQuad(PrimitiveShapes* shapes, sead::RegistryHeap* heap);
void setUpQuadTriangle(PrimitiveShapes* shapes, sead::RegistryHeap* heap);
void setUpCube(PrimitiveShapes* shapes, sead::RegistryHeap* heap);
void setUpCircle(PrimitiveShapes* shapes, u32 segments, sead::RegistryHeap* heap);
void setUpSphere(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap);
void setUpHemisphere(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap);
void setUpCylinder(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap);
void setUpCone(PrimitiveShapes* shapes, u32 slices, u32 stacks, sead::RegistryHeap* heap);
void setUpTorus(PrimitiveShapes* shapes, u32 segments, u32 rings, sead::RegistryHeap* heap, f32 scale,
                f32 tubeRadius, s32 turns, s32 windings);
void destroyPrimitiveShapesDisposer(PrimitiveShapesDisposer* disposer, u32 flags);
}
