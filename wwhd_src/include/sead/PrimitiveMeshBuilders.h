// WWHD primitive mesh builders; the vertex layout is the canonical renderer layout.
#pragma once
#include "sead/PrimitiveBackend.h"
namespace sead {
struct PrimitiveMeshBuilders {
    static void quad(PrimitiveVertex_l* vertices,be<u16>* indices);
    static void cube(PrimitiveVertex_l* vertices,be<u16>* indices);
    static void wireCube(PrimitiveVertex_l* vertices,be<u16>* indices);
    static void sphere(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices,s32 rings);
    static void disk(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices);
    static void line(PrimitiveVertex_l* vertices,be<u16>* indices);
    static void cylinder(PrimitiveVertex_l* vertices,be<u16>* indices,s32 slices);
};
}
