/* Graphics utility matrix bases with instruction-matched floating loads and store order. */
#include "agl/gx2/MatrixUtility.h"
using namespace agl::gx2util;

/* 027DC078 */
void agl::gx2util::MatrixUtility::fromPlanarDirection(const Vector3* direction) {
    gabi::AutoFrame frame{gabi::current_function()};
    mRows[0].x.mValue = direction->y.mValue;
    f32 copiedX = direction->x.mValue;
    mRows[0].z.mValue = 0.0f;
    mRows[0].y.mValue = copiedX;
    f32 x = direction->x.mValue;
    mRows[1].x.mValue = -x;
    mRows[1].y.mValue = direction->y.mValue;
    mRows[1].z.mValue = 0.0f;
    mRows[2].y.mValue = 0.0f;
    mRows[2].z.mValue = 1.0f;
    mRows[2].x.mValue = 0.0f;
}

/* 027DC1B4 */
void agl::gx2util::MatrixUtility::fromColumns(const Vector3* x, const Vector3* y, const Vector3* z) {
    gabi::AutoFrame frame{gabi::current_function()};
    mRows[0].x.mBits = x->x.mBits;
    mRows[0].y.mBits = y->x.mBits;
    mRows[0].z.mBits = z->x.mBits;
    mRows[1].x.mBits = x->y.mBits;
    mRows[1].y.mBits = y->y.mBits;
    mRows[1].z.mBits = z->y.mBits;
    mRows[2].x.mBits = x->z.mBits;
    mRows[2].y.mBits = y->z.mBits;
    mRows[2].z.mBits = z->z.mBits;
}

/* 027DC598 */
void agl::gx2util::MatrixUtility::fromPlanarColumn(const MatrixUtility* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Vector3> directionStorage(0x8);
    auto& direction = *directionStorage;
    f32 x = input->mRows[0].y.mValue;
    f32 y = input->mRows[1].y.mValue;
    f32 xSquared = gabi::fmuls_ppc(x, x);
    f32 ySquared = gabi::fmuls_ppc(y, y);
    f32 sum = gabi::fadds_ppc(xSquared, ySquared);
    f32 lengthSquared = gabi::fmadds(0.0f, 0.0f, sum);
    direction.z.mValue = 0.0f;
    bool usable = false;
    if (lengthSquared > 0.0f) {
        f32 estimate = (f32)gabi::frsqrte(lengthSquared);
        f32 halfLength = gabi::fmuls_ppc(0.5f, lengthSquared);
        f32 product = gabi::fmuls_ppc(halfLength, estimate);
        f32 correction = gabi::fnmsubs(product, estimate, 1.5f);
        f32 reciprocal = gabi::fmuls_ppc(estimate, correction);
        direction.x.mValue = gabi::fmuls_ppc(x, reciprocal);
        direction.y.mValue = gabi::fmuls_ppc(y, reciprocal);
        direction.z.mValue = gabi::fmuls_ppc(0.0f, reciprocal);
        f32 magnitude = gabi::fmuls_ppc(lengthSquared, reciprocal);
        usable = magnitude > 0.0f;
    }
    if (!usable) {
        direction.x.mValue = 0.0f;
        direction.y.mValue = 1.0f;
        direction.z.mValue = 0.0f;
    }
    fromPlanarDirection(&direction);
}

struct BasisVector { f32 x, y, z; };

static bool normalizeBasis(BasisVector& value) {
    f32 xSquared = gabi::fmuls_ppc(value.x, value.x);
    f32 ySquared = gabi::fmuls_ppc(value.y, value.y);
    f32 sum = gabi::fadds_ppc(xSquared, ySquared);
    f32 lengthSquared = gabi::fmadds(value.z, value.z, sum);
    if (!(lengthSquared > 0.0f)) return false;
    f32 estimate = (f32)gabi::frsqrte(lengthSquared);
    f32 halfLength = gabi::fmuls_ppc(0.5f, lengthSquared);
    f32 product = gabi::fmuls_ppc(halfLength, estimate);
    f32 correction = gabi::fnmsubs(product, estimate, 1.5f);
    f32 reciprocal = gabi::fmuls_ppc(estimate, correction);
    value.x = gabi::fmuls_ppc(value.x, reciprocal);
    value.y = gabi::fmuls_ppc(value.y, reciprocal);
    value.z = gabi::fmuls_ppc(value.z, reciprocal);
    f32 magnitude = gabi::fmuls_ppc(lengthSquared, reciprocal);
    return magnitude > 0.0f;
}

static BasisVector crossBasis(const BasisVector& left, const BasisVector& right) {
    f32 yz = gabi::fmuls_ppc(left.z, right.y);
    f32 xz = gabi::fmuls_ppc(left.x, right.z);
    f32 xy = gabi::fmuls_ppc(left.y, right.x);
    return {gabi::fmsubs(left.y, right.z, yz),
            gabi::fmsubs(left.z, right.x, xz),
            gabi::fmsubs(left.x, right.y, xy)};
}

static BasisVector transformedUp(const MatrixUtility* left, const MatrixUtility* right) {
    f32 bx = right->mRows[0].y.mValue;
    f32 by = right->mRows[1].y.mValue;
    f32 bz = right->mRows[2].y.mValue;
    f32 xy = gabi::fmuls_ppc(left->mRows[0].y.mValue, by);
    f32 xx = gabi::fmadds(left->mRows[0].x.mValue, bx, xy);
    f32 x = gabi::fmadds(left->mRows[0].z.mValue, bz, xx);
    f32 yy = gabi::fmuls_ppc(left->mRows[1].y.mValue, by);
    f32 yx = gabi::fmadds(left->mRows[1].x.mValue, bx, yy);
    f32 y = gabi::fmadds(left->mRows[1].z.mValue, bz, yx);
    f32 zy = gabi::fmuls_ppc(left->mRows[2].y.mValue, by);
    f32 zx = gabi::fmadds(left->mRows[2].x.mValue, bx, zy);
    f32 z = gabi::fmadds(left->mRows[2].z.mValue, bz, zx);
    return {x, y, z};
}

static void storeBasis(Vector3* destination, const BasisVector& value) {
    destination->x.mValue = value.x;
    destination->y.mValue = value.y;
    destination->z.mValue = value.z;
}

/* 027DC0C4 */
void agl::gx2util::MatrixUtility::fromTransformedPlanarColumn(const MatrixUtility* left, const MatrixUtility* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto up = transformedUp(left, right);
    up.z = 0.0f;
    if (!normalizeBasis(up)) up = {0.0f, 1.0f, 0.0f};
    gabi::FrameLocal<Vector3> directionStorage(0x8);
    auto& direction = *directionStorage;
    storeBasis(&direction, up);
    fromPlanarDirection(&direction);
}

/* 027DC200 */
void agl::gx2util::MatrixUtility::faceTranslationWithTransformedUp(const MatrixUtility* left, const MatrixUtility* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto up = transformedUp(left, right);
    auto forward = BasisVector{-mRows[0].w.mValue, -mRows[1].w.mValue, -mRows[2].w.mValue};
    if (!normalizeBasis(forward)) forward = {0.0f, 0.0f, 1.0f};
    auto side = crossBasis(up, forward);
    gabi::FrameLocal<Vector3> sideCrossStorage(0x2C);
    gabi::FrameLocal<Vector3> directCrossStorage(0x44);
    gabi::FrameLocal<Vector3> fallbackCrossStorage(0x38);
    storeBasis(sideCrossStorage, side);
    bool valid = normalizeBasis(side);
    bool direct = valid;
    if (!valid) {
        side = {forward.z, 0.0f, -forward.x};
        valid = normalizeBasis(side);
    }
    if (valid) {
        up = crossBasis(forward, side);
        if (direct) storeBasis(directCrossStorage, up);
        else storeBasis(fallbackCrossStorage, up);
    }
    else { side = {1.0f, 0.0f, 0.0f}; up = {0.0f, 0.0f, -forward.y}; }
    gabi::FrameLocal<Vector3> xStorage(0x8);
    auto& x = *xStorage;
    gabi::FrameLocal<Vector3> yStorage(0x20);
    auto& y = *yStorage;
    gabi::FrameLocal<Vector3> zStorage(0x14);
    auto& z = *zStorage;
    storeBasis(&x, side);
    storeBasis(&y, up);
    storeBasis(&z, forward);
    fromColumns(&x, &y, &z);
}

/* 027DC650 */
void agl::gx2util::MatrixUtility::faceTranslationWithPlanarUp(const MatrixUtility* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto forward = BasisVector{-mRows[0].w.mValue, -mRows[1].w.mValue, -mRows[2].w.mValue};
    auto up = BasisVector{input->mRows[0].y.mValue, input->mRows[1].y.mValue, 0.0f};
    if (!normalizeBasis(forward)) forward = {0.0f, 0.0f, 1.0f};
    auto side = crossBasis(up, forward);
    gabi::FrameLocal<Vector3> sideCrossStorage(0x2C);
    gabi::FrameLocal<Vector3> directCrossStorage(0x44);
    gabi::FrameLocal<Vector3> fallbackCrossStorage(0x38);
    storeBasis(sideCrossStorage, side);
    bool valid = normalizeBasis(side);
    bool direct = valid;
    if (!valid) { side = {forward.z, 0.0f, -forward.x}; valid = normalizeBasis(side); }
    if (valid) {
        up = crossBasis(forward, side);
        if (direct) storeBasis(directCrossStorage, up);
        else storeBasis(fallbackCrossStorage, up);
    }
    else { side = {1.0f, 0.0f, 0.0f}; up = {0.0f, 0.0f, -forward.y}; }
    gabi::FrameLocal<Vector3> xStorage(0x8);
    auto& x = *xStorage;
    gabi::FrameLocal<Vector3> yStorage(0x20);
    auto& y = *yStorage;
    gabi::FrameLocal<Vector3> zStorage(0x14);
    auto& z = *zStorage;
    storeBasis(&x, side); storeBasis(&y, up); storeBasis(&z, forward);
    fromColumns(&x, &y, &z);
}

/* 027DC9A0 */
void agl::gx2util::MatrixUtility::fromTransformedUp(const MatrixUtility* left, const MatrixUtility* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto up = transformedUp(left, right);
    auto side = BasisVector{up.y, -up.x, 0.0f};
    if (!normalizeBasis(up)) { up = {0.0f, 1.0f, 0.0f}; side = {1.0f, 0.0f, 0.0f}; }
    auto forward = BasisVector{};
    if (normalizeBasis(side)) {
        forward = crossBasis(side, up);
        gabi::FrameLocal<Vector3> crossStorage(0x2C);
        storeBasis(crossStorage, forward);
    } else { side = {1.0f, 0.0f, 0.0f}; forward = {0.0f, -up.z, 0.0f}; }
    gabi::FrameLocal<Vector3> xStorage(0x8);
    auto& x = *xStorage;
    gabi::FrameLocal<Vector3> yStorage(0x14);
    auto& y = *yStorage;
    gabi::FrameLocal<Vector3> zStorage(0x20);
    auto& z = *zStorage;
    storeBasis(&x, side); storeBasis(&y, up); storeBasis(&z, forward);
    fromColumns(&x, &y, &z);
}

/* 027DCBBC */
void agl::gx2util::MatrixUtility::fromTransformedUpFacingTranslation(const MatrixUtility* left, const MatrixUtility* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto up = transformedUp(left, right);
    auto forward = BasisVector{-mRows[0].w.mValue, -mRows[1].w.mValue, -mRows[2].w.mValue};
    if (!normalizeBasis(up)) up = {0.0f, 1.0f, 0.0f};
    auto side = crossBasis(up, forward);
    gabi::FrameLocal<Vector3> sideCrossStorage(0x2C);
    gabi::FrameLocal<Vector3> directCrossStorage(0x44);
    gabi::FrameLocal<Vector3> fallbackCrossStorage(0x38);
    storeBasis(sideCrossStorage, side);
    bool valid = normalizeBasis(side);
    bool direct = valid;
    if (!valid) { side = {up.y, -up.x, 0.0f}; valid = normalizeBasis(side); }
    if (valid) {
        forward = crossBasis(side, up);
        if (direct) storeBasis(directCrossStorage, forward);
        else storeBasis(fallbackCrossStorage, forward);
    }
    else { side = {1.0f, 0.0f, 0.0f}; forward = {0.0f, -up.z, 0.0f}; }
    gabi::FrameLocal<Vector3> xStorage(0x8);
    auto& x = *xStorage;
    gabi::FrameLocal<Vector3> yStorage(0x14);
    auto& y = *yStorage;
    gabi::FrameLocal<Vector3> zStorage(0x20);
    auto& z = *zStorage;
    storeBasis(&x, side); storeBasis(&y, up); storeBasis(&z, forward);
    fromColumns(&x, &y, &z);
}

struct ScaledPoseBuilderTable { be<u32> mFunctions[8]; };
static ScaledPoseBuilderTable sScaledPoseBuilders;
static f32 poseColumnLength(const MatrixUtility* matrix, u32 column) {
    const be<f32>* values = reinterpret_cast<const be<f32>*>(matrix);
    f32 y = values[column + 4];
    f32 x = values[column];
    f32 z = values[column + 8];
    f32 square = gabi::fmadds(z, z, gabi::fmadds(x, x, gabi::fmuls_ppc(y, y)));
    if (square == 0.0f) return 0.0f;
    f32 estimate = gabi::frsqrte(square);
    f32 half = gabi::fmuls_ppc(0.5f, square);
    f32 term = gabi::fmuls_ppc(half, estimate);
    f32 correction = gabi::fnmsubs(term, estimate, 1.5f);
    estimate = gabi::fmuls_ppc(estimate, correction);
    return gabi::fmuls_ppc(square, estimate);
}

struct PoseColumnScales { be<f32> mX; be<f32> mY; };
WWHD_OFFSET(PoseColumnScales, mX, 0);
WWHD_OFFSET(PoseColumnScales, mY, 4);
WWHD_SIZE(PoseColumnScales, 8);

/* 027DB320 */
static void buildScaledPoseMatrix(u32 flags, MatrixUtility* output, const MatrixUtility* local, const MatrixUtility* parent, void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 x = poseColumnLength(parent, 0);
    f32 y = poseColumnLength(parent, 1);
    f32 z = poseColumnLength(parent, 2);
    gabi::FrameLocal<PoseColumnScales> scalesStorage(0x08);
    scalesStorage->mX = x;
    scalesStorage->mY = y;
    u32 adjusted = flags - 0x20000;
    u32 offset = ((adjusted << 18) | (adjusted >> 14)) & 0x3FFFC;
    u32 function = *reinterpret_cast<be<u32>*>(reinterpret_cast<u8*>(&sScaledPoseBuilders) + offset);
    gabi::call_ptr<void>(function, output, local, parent, context);
    be<f32>* values = reinterpret_cast<be<f32>*>(output);
    f32 r10 = gabi::fmuls_ppc(values[4], x);
    f32 r11 = gabi::fmuls_ppc(values[5], y);
    values[4] = r10; values[5] = r11;
    f32 r02 = gabi::fmuls_ppc(values[2], z);
    f32 r03 = gabi::fmuls_ppc(values[3], 1.0f);
    values[2] = r02; values[3] = r03;
    f32 r12 = gabi::fmuls_ppc(values[6], z);
    f32 r13 = gabi::fmuls_ppc(values[7], 1.0f);
    f32 r20 = gabi::fmuls_ppc(values[8], x);
    f32 r21 = gabi::fmuls_ppc(values[9], y);
    f32 r00 = gabi::fmuls_ppc(values[0], x);
    f32 r01 = gabi::fmuls_ppc(values[1], y);
    values[6] = r12; values[7] = r13;
    values[8] = r20; values[9] = r21;
    f32 r22 = gabi::fmuls_ppc(values[10], z);
    f32 r23 = gabi::fmuls_ppc(values[11], 1.0f);
    values[0] = r00; values[1] = r01;
    values[10] = r22; values[11] = r23;
}

/* WWHD glue
 data sScaledPoseBuilders 1016C5D4
*/
