// Matrix and shader-choice helpers emitted with WWHD's effect code.
#include "agl/EffectMatrixSupport.h"
#include "bindings.h"
using namespace agl::effect_matrix;
static sead::ViewTransform sDefaultLookAtView;
static LookAtCameraMethods sLookAtTable;

namespace agl::effect_matrix {
/* 0278D208 */
Vector3* Vector3::create(Vector3* vector) {
    gabi::AutoFrame frame{0x0278D208u};
    if (vector == nullptr) vector = static_cast<Vector3*>(operator_new(sizeof(Vector3)));
    return vector;
}

// The original's lfs/stfs copies preserve signaling-NaN bits.
static void copyWord(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) =
        static_cast<u32>(reinterpret_cast<const be<u32>&>(source));
}

// The original indexes the rows by `column * 4` with no bound check (slwi/lfsx): every column value reads
// source + 4 * column (32-bit wrap) and the same offset in rows 1 and 2, one word load and store per row.
/* 0278D234 */
void Vector3::copyColumn(const Matrix34* source, u32 column) {
    u32 element = gabi::ea(source) + column * 4;
    gabi::store<u32>(gabi::ea(&mX), gabi::load<u32>(element));
    gabi::store<u32>(gabi::ea(&mY), gabi::load<u32>(element + 0x10));
    gabi::store<u32>(gabi::ea(&mZ), gabi::load<u32>(element + 0x20));
}

/* 0279049C */
u32 packShaderChoice(u32 inner, u32 horizontal, u32 vertical, u32 outer,
                     u32 blur, u32 fade, u32 output) {
    return (((((outer * 2 + vertical) * 4 + horizontal) * 4 + inner) * 2 + blur)
              * 2 + fade) * 2 + output;
}

/* 027919D0 */
Matrix4* Matrix4::create(Matrix4* matrix) {
    gabi::AutoFrame frame{0x027919D0u};
    if (matrix == nullptr) matrix = static_cast<Matrix4*>(operator_new(sizeof(Matrix4)));
    return matrix;
}

/* 02791B98 */
void Matrix3::copyRotation(const Matrix34* source) {
    for (u32 row = 0; row != 3; ++row) {
        copyWord(mElements[row][0], source->mRows[row].mX);
        copyWord(mElements[row][1], source->mRows[row].mY);
        copyWord(mElements[row][2], source->mRows[row].mZ);
    }
}

/* 02791BE4 */
void Matrix4::setRows(const Matrix34* affine, const Row4* lastRow) {
    for (u32 row = 0; row != 3; ++row) {
        copyWord(mRows[row].mX, affine->mRows[row].mX);
        copyWord(mRows[row].mY, affine->mRows[row].mY);
        copyWord(mRows[row].mZ, affine->mRows[row].mZ);
        copyWord(mRows[row].mW, affine->mRows[row].mW);
    }
    copyWord(mRows[3].mX, lastRow->mX);
    copyWord(mRows[3].mY, lastRow->mY);
    copyWord(mRows[3].mZ, lastRow->mZ);
    copyWord(mRows[3].mW, lastRow->mW);
}

/* 02791C68 */
f64 Vector3::setNegativeForward(const Matrix34* source) {
    gabi::AutoFrame frame{0x02791C68u};
    f32 x = source->mRows[2].mX;
    f32 y = source->mRows[2].mY;
    f32 negativeX = -x;
    f32 z = source->mRows[2].mZ;
    f32 negativeY = -y;
    f32 negativeZ = -z;
    mX = negativeX;
    mY = negativeY;
    mZ = negativeZ;
    return normalizeEffectDirection(this);
}

/* 02791C90 */
void Vector3::setInverseTranslation(const Matrix34* source) {
    f32 xx = source->mRows[0].mX;
    f32 tx = source->mRows[0].mW;
    f32 xy = source->mRows[0].mY;
    f32 xFirst = -gabi::fmuls_ppc(xx, tx);
    f32 ty = source->mRows[1].mW;
    f32 yx = source->mRows[1].mX;
    f32 yFirst = -gabi::fmuls_ppc(xy, tx);
    f32 xz = source->mRows[0].mZ;
    f32 xSecond = gabi::fnmsubs(yx, ty, xFirst);
    f32 zFirst = -gabi::fmuls_ppc(xz, tx);
    f32 zx = source->mRows[2].mX;
    f32 tz = source->mRows[2].mW;
    f32 yy = source->mRows[1].mY;
    f32 x = gabi::fnmsubs(zx, tz, xSecond);
    f32 yz = source->mRows[1].mZ;
    f32 ySecond = gabi::fnmsubs(yy, ty, yFirst);
    f32 zy = source->mRows[2].mY;
    f32 zSecond = gabi::fnmsubs(yz, ty, zFirst);
    f32 zz = source->mRows[2].mZ;
    f32 y = gabi::fnmsubs(zy, tz, ySecond);
    mX = x;
    f32 z = gabi::fnmsubs(zz, tz, zSecond);
    mY = y;
    mZ = z;
}

static sead::ViewTransform* viewStorage(Matrix34*);

/* 02791D00 */
void Matrix34::setLookAt(const Vector3* position, const Vector3* direction, const Vector3* up) {
    gabi::AutoFrame frame{0x02791D00u};
    gabi::FrameLocal<sead::ViewCamera> camera(0x14);
    gabi::FrameLocal<Vector3> targetSum(8);
    gabi::FrameLocal<Vector3> targetCopy(0x6C);
    for (u32 row = 0; row != 3; ++row) {
        for (u32 column = 0; column != 4; ++column) {
            u32 bits = sDefaultLookAtView.mRows[row][column].bits;
            camera->mTransform.mRows[row][column].bits = bits;
        }
    }
    u32 eyeY = static_cast<u32>(reinterpret_cast<const be<u32>&>(position->mY));
    f32 positionX = position->mX;
    f32 positionY = position->mY;
    camera->mEye.y.bits = eyeY;
    f32 directionX = direction->mX;
    f32 directionY = direction->mY;
    u32 eyeZ = static_cast<u32>(reinterpret_cast<const be<u32>&>(position->mZ));
    f32 targetX = gabi::fadds_ppc(positionX, directionX);
    f32 targetY = gabi::fadds_ppc(positionY, directionY);
    f32 positionZ = position->mZ;
    camera->mEye.z.bits = eyeZ;
    u32 eyeX = static_cast<u32>(reinterpret_cast<const be<u32>&>(position->mX));
    targetSum->mX = targetX;
    targetSum->mY = targetY;
    u32 targetYBits = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetSum->mY));
    camera->mEye.x.bits = eyeX;
    f32 directionZ = direction->mZ;
    reinterpret_cast<be<u32>&>(targetCopy->mY) = targetYBits;
    f32 targetZ = gabi::fadds_ppc(positionZ, directionZ);
    camera->mVtable = &sLookAtTable;
    u32 targetXBits = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetSum->mX));
    targetSum->mZ = targetZ;
    u32 targetZBits = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetSum->mZ));
    reinterpret_cast<be<u32>&>(targetCopy->mX) = targetXBits;
    u32 copiedX = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetCopy->mX));
    reinterpret_cast<be<u32>&>(targetCopy->mZ) = targetZBits;
    u32 copiedZ = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetCopy->mZ));
    camera->mTarget.x.bits = copiedX;
    camera->mTarget.z.bits = copiedZ;
    camera->mUp.x.bits = static_cast<u32>(reinterpret_cast<const be<u32>&>(up->mX));
    u32 upZ = static_cast<u32>(reinterpret_cast<const be<u32>&>(up->mZ));
    u32 upY = static_cast<u32>(reinterpret_cast<const be<u32>&>(up->mY));
    u32 copiedY = static_cast<u32>(reinterpret_cast<const be<u32>&>(targetCopy->mY));
    camera->mUp.y.bits = upY;
    camera->mTarget.y.bits = copiedY;
    camera->mUp.z.bits = upZ;
    seadNormalizeViewVector(&camera->mUp);
    const auto* methods = static_cast<const LookAtCameraMethods*>(camera->mVtable.get());
    u32 update = methods->mUpdateView.mFunction;
    gabi::call_ptr<void>(update, camera.get(), &camera->mTransform);
    seadCopyViewMatrix(&camera->mTransform, viewStorage(this));
    sead::destroyViewTransform(&camera->mTransform, 2);
}
// Scalar-layout ABI adapter for this TU's matrix method carrier, not type identity.
static sead::ViewTransform* viewStorage(Matrix34* matrix) {
    static_assert(sizeof(Matrix34) == sizeof(sead::ViewTransform));
    static_assert(offsetof(Matrix34, mRows) == offsetof(sead::ViewTransform, mRows));
    static_assert(sizeof(Row4) == 4 * sizeof(sead::ViewScalar));
    static_assert(offsetof(Row4, mX) == 0 * sizeof(sead::ViewScalar));
    static_assert(offsetof(Row4, mY) == 1 * sizeof(sead::ViewScalar));
    static_assert(offsetof(Row4, mZ) == 2 * sizeof(sead::ViewScalar));
    static_assert(offsetof(Row4, mW) == 3 * sizeof(sead::ViewScalar));
    return reinterpret_cast<sead::ViewTransform*>(matrix);
}
}

/* WWHD glue
data sDefaultLookAtView 104A041C
data sLookAtTable 101450F8
ext 025155D8 f64 normalizeEffectDirection(agl::effect_matrix::Vector3*)
ext 025155D8 void seadNormalizeViewVector(sead::ViewVector*)
ext 028E90D4 void seadCopyViewMatrix(const sead::ViewTransform*,sead::ViewTransform*)
*/
