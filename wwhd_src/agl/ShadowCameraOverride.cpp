// Shadow-camera orientation and optional view/projection replacement.
#include "agl/ShadowCameraOverride.h"
#include "bindings.h"
using namespace agl::shadow_override;
static Vector3 sDefaultUp;
static Vector3 sDefaultForward;
static Vector3 sDefaultPosition;
static Vector3 sDefaultViewAxis;
// Preserve the original word accesses through typed big-endian float fields.
static u32 readFloatWord(const be<f32>& value) {
    return static_cast<u32>(reinterpret_cast<const be<u32>&>(value));
}
static void writeFloatWord(be<f32>& value, u32 bits) {
    reinterpret_cast<be<u32>&>(value) = bits;
}
static void copyMatrixWords(Matrix34*, const Matrix34*);
static void copyVectorWords(Vector3*, const Vector3*);

/* 0278D2E8 */
void CameraOverride::setDirection(const Vector3* direction) {
    gabi::AutoFrame frame{0x0278D2E8u};
    gabi::FrameLocal<Vector3> normal(0x14);
    gabi::FrameLocal<Vector3> up(8);
    gabi::FrameLocal<Vector3> firstCross(0x2C);
    gabi::FrameLocal<Vector3> secondCross(0x38);
    gabi::FrameLocal<Vector3> secondCopy(0x20);
    u32 y = readFloatWord(direction->mY);
    u32 x = readFloatWord(direction->mX);
    u32 z = readFloatWord(direction->mZ);
    writeFloatWord(normal->mX, x);
    writeFloatWord(normal->mZ, z);
    writeFloatWord(normal->mY, y);
    normalizeShadowVector(normal.get());
    crossShadowVectors(normal.get(), &sDefaultUp, firstCross.get());
    x = readFloatWord(firstCross->mX);
    y = readFloatWord(firstCross->mY);
    z = readFloatWord(firstCross->mZ);
    writeFloatWord(up->mY, y);
    writeFloatWord(up->mZ, z);
    writeFloatWord(up->mX, x);
    f64 length = normalizeShadowVector(up.get());
    bool fallback = length < 1.1920928955078125e-07;
    if (!fallback) {
        f32 nx = normal->mX, ny = normal->mY;
        f32 ux = up->mX, uy = up->mY;
        f32 productX = gabi::fmuls_ppc(ux, nx);
        f32 productY = gabi::fmuls_ppc(uy, ny);
        f32 sum = gabi::fadds_ppc(productX, productY);
        f32 uz = up->mZ;
        f32 nz = normal->mZ;
        f32 dot = gabi::fmadds(uz, nz, sum);
        fallback = std::fabs(dot) > 0.9999998807907104f;
    }
    if (fallback) {
        crossShadowVectors(normal.get(), &sDefaultForward, secondCross.get());
        x = readFloatWord(secondCross->mX);
        y = readFloatWord(secondCross->mY);
        writeFloatWord(secondCopy->mX, x);
        x = readFloatWord(secondCopy->mX);
        z = readFloatWord(secondCross->mZ);
        writeFloatWord(secondCopy->mY, y);
        y = readFloatWord(secondCopy->mY);
        writeFloatWord(secondCopy->mZ, z);
        z = readFloatWord(secondCopy->mZ);
        writeFloatWord(up->mY, y);
        writeFloatWord(up->mZ, z);
        writeFloatWord(up->mX, x);
    }
    mView.setLookAt(&sDefaultPosition, direction, up.get());
    setShadowOverrideProjection(&mProjection, 1.0, 1.0, -1.0);
    mEnabled = 1;
}

// Each streamed matrix word is stored before the next is read.
static void copyMatrixWords(Matrix34* destination, const Matrix34* source) {
    for (u32 row = 0; row != 3; ++row) {
        writeFloatWord(destination->mRows[row].mX, readFloatWord(source->mRows[row].mX));
        writeFloatWord(destination->mRows[row].mY, readFloatWord(source->mRows[row].mY));
        writeFloatWord(destination->mRows[row].mZ, readFloatWord(source->mRows[row].mZ));
        writeFloatWord(destination->mRows[row].mW, readFloatWord(source->mRows[row].mW));
    }
}
static void copyVectorWords(Vector3* destination, const Vector3* source) {
    u32 z = readFloatWord(source->mZ);
    u32 x = readFloatWord(source->mX);
    u32 y = readFloatWord(source->mY);
    writeFloatWord(destination->mZ, z);
    writeFloatWord(destination->mY, y);
    writeFloatWord(destination->mX, x);
}

/* 0278D434 */
void CameraOverride::applyWithReference(Matrix34* view, sead::RegistryMatrix44* projection, const Matrix34* reference) {
    gabi::AutoFrame frame{0x0278D434u};
    if (view != nullptr) {
        if (mEnabled != 0) {
            gabi::FrameLocal<Matrix34> referenceFirst(0x50), referenceCopy(0xE0);
            gabi::FrameLocal<Matrix34> directionFirst(0x80), directionCopy(0x110);
            gabi::FrameLocal<Matrix34> positionFirst(0xB0), positionCopy(0x140);
            gabi::FrameLocal<Vector3> upResult(8), directionResult(0x14), positionResult(0x20);
            gabi::FrameLocal<Vector3> up(0x2C), direction(0x38), position(0x44);
            copyMatrixWords(referenceFirst.get(), reference);
            copyMatrixWords(referenceCopy.get(), referenceFirst.get());
            upResult->setNegativeForward(referenceCopy.get());
            copyVectorWords(up.get(), upResult.get());
            copyMatrixWords(directionFirst.get(), &mView);
            copyMatrixWords(directionCopy.get(), directionFirst.get());
            directionResult->setNegativeForward(directionCopy.get());
            copyVectorWords(direction.get(), directionResult.get());
            copyMatrixWords(positionFirst.get(), &mView);
            copyMatrixWords(positionCopy.get(), positionFirst.get());
            positionResult->setInverseTranslation(positionCopy.get());
            copyVectorWords(position.get(), positionResult.get());
            view->setLookAt(position.get(), direction.get(), up.get());
        } else {
            copyShadowView(&mView, view);
        }
    }
    if (projection != nullptr) copyShadowProjection(&mProjection, projection);
}

/* 0278D668 */
void CameraOverride::applyWithUp(Matrix34* view, sead::RegistryMatrix44* projection, const Vector3* up) {
    gabi::AutoFrame frame{0x0278D668u};
    if (view != nullptr) {
        if (mEnabled != 0) {
            gabi::FrameLocal<Matrix34> directionFirst(0x38), directionCopy(0x98);
            gabi::FrameLocal<Matrix34> positionFirst(0x68), positionCopy(0xC8);
            gabi::FrameLocal<Vector3> directionResult(8), positionResult(0x14);
            gabi::FrameLocal<Vector3> direction(0x20), position(0x2C);
            copyMatrixWords(directionFirst.get(), &mView);
            copyMatrixWords(directionCopy.get(), directionFirst.get());
            directionResult->setNegativeForward(directionCopy.get());
            copyVectorWords(direction.get(), directionResult.get());
            copyMatrixWords(positionFirst.get(), &mView);
            copyMatrixWords(positionCopy.get(), positionFirst.get());
            positionResult->setInverseTranslation(positionCopy.get());
            copyVectorWords(position.get(), positionResult.get());
            view->setLookAt(position.get(), direction.get(), up);
        } else {
            copyShadowView(&mView, view);
        }
    }
    if (projection != nullptr) copyShadowProjection(&mProjection, projection);
}

/* 0278D81C */
CameraOverride* CameraOverride::create(CameraOverride* state) {
    gabi::AutoFrame frame{0x0278D81Cu};
    if (state == nullptr) state = static_cast<CameraOverride*>(operator_new(sizeof(CameraOverride)));
    if (state == nullptr) return state;
    if (gabi::ea(state) == 0) operator_new(sizeof(Matrix34));
    if ((gabi::ea(state) + static_cast<u32>(offsetof(CameraOverride, mProjection))) == 0)
        operator_new(sizeof(sead::RegistryMatrix44));
    gabi::FrameLocal<Vector3> negative(8), direction(0x14);
    state->mEnabled = 1;
    f32 x = sDefaultViewAxis.mX;
    f32 z = sDefaultViewAxis.mZ;
    f32 negativeX = -x;
    f32 y = sDefaultViewAxis.mY;
    f32 negativeZ = -z;
    f32 negativeY = -y;
    negative->mX = negativeX;
    u32 xBits = readFloatWord(negative->mX);
    negative->mY = negativeY;
    u32 yBits = readFloatWord(negative->mY);
    negative->mZ = negativeZ;
    u32 zBits = readFloatWord(negative->mZ);
    writeFloatWord(direction->mY, yBits);
    writeFloatWord(direction->mX, xBits);
    writeFloatWord(direction->mZ, zBits);
    state->setDirection(direction.get());
    return state;
}

/* WWHD glue
data sDefaultUp 104A0934
data sDefaultForward 104A094C
data sDefaultPosition 104A0928
data sDefaultViewAxis 104A0940
ext 028E8B04 void setShadowOverrideProjection(sead::RegistryMatrix44*,f64,f64,f64)
*/
