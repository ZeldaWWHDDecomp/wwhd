/* mDoExt_3DlineMat0 update: tww names, HD mesh buffers and vertex attributes.
 */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JUTAssert.h"
namespace line_layout {
struct Vertex {
    cXyz mPosition;
    cXyz mTangent;
    cXyz mNormal;
    be<f32> mNormalW;
    u8 _028[0x98 - 0x28];
};
WWHD_SIZE(Vertex, 0x98);
struct VertexTable {
    be<u32> mCount;
    gptr<Vertex> mpVertices;
    Vertex* getVertex(u32 index) {
        u32 count = mCount;
        Vertex* vertices = mpVertices;
        return index < count ? &vertices[index] : vertices;
    }
};
struct MeshBuffer {
    struct Buffer { u8 _000[4]; gptr<VertexTable> mpVertices; };
    Buffer mBuffers[2];
    u8 _010[0x1C - 0x10];
    be<u32> mCurArr;
    VertexTable* getVertices() { return mBuffers[mCurArr].mpVertices; }
};
WWHD_OFFSET(MeshBuffer, mCurArr, 0x1C);
struct mDoExt_3Dline_c {
    gptr<cXyz> mpSegments;
    gptr<be<u8>> mpSize;
    gptr<MeshBuffer> mpMeshBuffer; // HD: replaces the GameCube position arrays.
    u8 _00C[4];
};
WWHD_SIZE(mDoExt_3Dline_c, 0x10);
struct mDoExt_3DlineMat0_c {
    // HD: preserve the full-width arguments and verified GPR/FPR order.
    void update(u32 count, const be<u32>* color, u32 taper, dKy_tevstr_c* lighting, f32 width);
    // HD: name the second overload separately for unambiguous harness registration.
    void updateWidths(u32 count, const be<u32>* color, dKy_tevstr_c* lighting);
    u8 _000[0x134];
    be<u32> mColor; // HD: packed RGBA word, copied without channel conversions.
    gptr<dKy_tevstr_c> mpTevStr;
    be<u16> mNumLines;
    be<u16> mMaxSegments;
    be<u16> mNumSegments;
    u8 _142[2];
    gptr<mDoExt_3Dline_c> mpLines;
};
WWHD_OFFSET(mDoExt_3DlineMat0_c, mColor, 0x134);
WWHD_OFFSET(mDoExt_3DlineMat0_c, mpLines, 0x144);
struct LineWork {
    cXyz tangent, partial, cross, normal, scaled, norm2;
    cXyz averagePlus, averageMinus, temp, eye, out, norm;
};
struct WidthWork {
    cXyz partial, tangent, cross, unused, normal, scaled, norm2;
    cXyz averagePlus, averageMinus, temp, eye, out, norm;
};
WWHD_SIZE(LineWork, 144);
WWHD_SIZE(WidthWork, 156);
WWHD_OFFSET(LineWork, normal, 0x24);
WWHD_OFFSET(WidthWork, normal, 0x30);
struct CameraView { u8 _000[0xDC]; cXyz mEye; };
struct PlayView { u8 _0000[0x5FA4]; gptr<CameraView> mView; };
}
inline line_layout::CameraView* lineGetView() {
    return reinterpret_cast<line_layout::PlayView*>(dComIfGp_get())->mView;
}
u32 lineNonzero(f32 value);
f32 lineSquareRoot(); // HD: consumes the preceding vector magnitude in guest FPR1.
void lineNormalize(cXyz* vector, cXyz* result);
void lineScale(cXyz* vector, cXyz* result, f32 scale);
void lineAddVectors(cXyz* a, cXyz* b, cXyz* result);
void lineFinalize(line_layout::MeshBuffer* buffer, u32 first, u32 count);
void lineMaterialSetup(line_layout::mDoExt_3DlineMat0_c* material, dKy_tevstr_c* lighting, be<u32>* color);
