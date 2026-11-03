/* Matrix views expose both raw word copies and floating-load conversions. */
#pragma once
#include "wwhd.h"
namespace agl { namespace gx2util {
union MatrixScalar { be<u32> mBits; be<f32> mValue; };
struct Vector3 { MatrixScalar x, y, z; };
struct MatrixRow { MatrixScalar x, y, z, w; };
struct MatrixUtility {
    MatrixRow mRows[3];
    void fromPlanarDirection(const Vector3* direction);
    void fromColumns(const Vector3* x, const Vector3* y, const Vector3* z);
    void fromPlanarColumn(const MatrixUtility* input);
    void fromTransformedPlanarColumn(const MatrixUtility* left, const MatrixUtility* right);
    void faceTranslationWithTransformedUp(const MatrixUtility* left, const MatrixUtility* right);
    void faceTranslationWithPlanarUp(const MatrixUtility* input);
    void fromTransformedUp(const MatrixUtility* left, const MatrixUtility* right);
    void fromTransformedUpFacingTranslation(const MatrixUtility* left, const MatrixUtility* right);
};
WWHD_SIZE(MatrixScalar, 4);
WWHD_SIZE(Vector3, 0xC);
WWHD_SIZE(MatrixRow, 0x10);
WWHD_SIZE(MatrixUtility, 0x30);
WWHD_OFFSET(MatrixRow, w, 0xC);
} }
