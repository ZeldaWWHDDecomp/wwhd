// Camera inputs and storage used by WWHD's shadow effects.
#include "agl/ShadowView.h"
#include "bindings.h"
using namespace agl::shadow_view;
// Borrowed: the default model matrix, as SEAD#13's sDefaultModel.
static sead::RegistryMatrix44 sDefaultProjection;
static Matrix34 sDefaultView;
static Pair2 sDefaultCenter;
static Vector3 sForwardAxis;
// Read and write float payloads as typed big-endian words to preserve NaN bits.
static u32 readFloatWord(const be<f32>& value) {
    return static_cast<u32>(reinterpret_cast<const be<u32>&>(value));
}
static void writeFloatWord(be<f32>& value, u32 bits) {
    reinterpret_cast<be<u32>&>(value) = bits;
}

/* 0278CB8C */
void CameraView::setPerspective(const sead::RegistryMatrix44* projection, const Pair2* center,
                                f64 nearPlane, f64 farPlane, f64 fieldOfView, f64 aspect) {
    gabi::AutoFrame frame{0x0278CB8Cu};
    copyShadowProjection(projection, &mProjection);
    writeFloatWord(mNear, ppc_stfs_bits(nearPlane));
    writeFloatWord(mAspect, ppc_stfs_bits(aspect));
    writeFloatWord(mFieldOfView, ppc_stfs_bits(fieldOfView));
    writeFloatWord(mFar, ppc_stfs_bits(farPlane));
    writeFloatWord(mCenter.mX, readFloatWord(center->mX));
    f64 halfAngle = gabi::fmuls_fpr(fieldOfView, 0.5);
    writeFloatWord(mCenter.mY, readFloatWord(center->mY));
    f64 tangent = shadowViewTangent(halfAngle);
    f64 cotangent = gabi::fdivs_fpr(1.0, tangent);
    writeFloatWord(mHalfAngleTangent, ppc_stfs_bits(tangent));
    writeFloatWord(mHalfAngleCotangent, ppc_stfs_bits(cotangent));
}

/* 0278CC74 */
void CameraView::rebuildVolume() {
    gabi::AutoFrame frame{0x0278CC74u};
    struct Corners { Vector3 mPoints[8]; };
    gabi::FrameLocal<Corners> local(0x48);
    gabi::FrameLocal<Corners> world(0xA8);
    gabi::FrameLocal<Pair2> nearExtent(8);
    gabi::FrameLocal<Pair2> farExtent(0x10);
    gabi::FrameLocal<Vector3> negativeAxis(0x18);
    gabi::FrameLocal<Vector3> translationCopy(0x24);
    gabi::FrameLocal<Vector3> negativeAxisCopy(0x30);
    gabi::FrameLocal<Vector3> translation(0x3C);
    Vector3* points = local->mPoints;
    Vector3* worldPoints = world->mPoints;
    constructShadowVectors(points, 8, sizeof(Vector3), gabi::guest_fn((const void*)&Vector3::create));
    constructShadowVectors(worldPoints, 8, sizeof(Vector3), gabi::guest_fn((const void*)&Vector3::create));
    f32 nearPlane = mNear;
    f32 tangent = mHalfAngleTangent;
    f32 nearZ = -nearPlane;
    f32 aspect = mAspect;
    f32 halfHeight = gabi::fmuls_ppc(nearPlane, tangent);
    f32 centerY = mCenter.mY;
    f32 centerX = mCenter.mX;
    f32 halfWidth = gabi::fmuls_ppc(halfHeight, aspect);
    points[3].mZ = nearZ;
    f32 yOffset = gabi::fmuls_ppc(halfHeight, centerY);
    points[1].mZ = nearZ;
    f32 xOffset = gabi::fmuls_ppc(halfWidth, centerX);
    points[0].mZ = nearZ;
    f32 doubledX = gabi::fadds_ppc(xOffset, xOffset);
    f32 doubledY = gabi::fadds_ppc(yOffset, yOffset);
    nearExtent->mY = halfHeight;
    f32 left = gabi::fsubs_ppc(doubledX, halfWidth);
    nearExtent->mX = halfWidth;
    f32 top = gabi::fadds_ppc(halfHeight, doubledY);
    points[2].mZ = nearZ;
    f32 right = gabi::fadds_ppc(halfWidth, doubledX);
    points[1].mX = left;
    points[1].mY = top;
    f32 bottom = gabi::fsubs_ppc(doubledY, halfHeight);
    points[2].mY = top;
    points[2].mX = right;
    points[3].mY = bottom;
    points[3].mX = right;
    points[0].mX = left;
    points[0].mY = bottom;
    crossShadowVectors(&points[1], &points[0], &mLocalPlaneNormals[0]);
    normalizeShadowVector(&mLocalPlaneNormals[0]);
    crossShadowVectors(&points[2], &points[1], &mLocalPlaneNormals[2]);
    normalizeShadowVector(&mLocalPlaneNormals[2]);
    crossShadowVectors(&points[3], &points[2], &mLocalPlaneNormals[1]);
    normalizeShadowVector(&mLocalPlaneNormals[1]);
    crossShadowVectors(&points[0], &points[3], &mLocalPlaneNormals[3]);
    normalizeShadowVector(&mLocalPlaneNormals[3]);

    tangent = mHalfAngleTangent;
    f32 farPlane = mFar;
    aspect = mAspect;
    halfHeight = gabi::fmuls_ppc(farPlane, tangent);
    centerY = mCenter.mY;
    halfWidth = gabi::fmuls_ppc(halfHeight, aspect);
    centerX = mCenter.mX;
    yOffset = gabi::fmuls_ppc(halfHeight, centerY);
    xOffset = gabi::fmuls_ppc(halfWidth, centerX);
    doubledY = gabi::fadds_ppc(yOffset, yOffset);
    f32 farZ = -farPlane;
    farExtent->mY = halfHeight;
    doubledX = gabi::fadds_ppc(xOffset, xOffset);
    farExtent->mX = halfWidth;
    top = gabi::fadds_ppc(halfHeight, doubledY);
    points[4].mZ = farZ;
    left = gabi::fsubs_ppc(doubledX, halfWidth);
    points[6].mY = top;
    right = gabi::fadds_ppc(halfWidth, doubledX);
    points[5].mZ = farZ;
    bottom = gabi::fsubs_ppc(doubledY, halfHeight);
    points[5].mY = top;
    points[7].mX = right;
    points[4].mY = bottom;
    points[7].mY = bottom;
    points[4].mX = left;
    points[6].mX = right;
    points[5].mX = left;
    points[6].mZ = farZ;
    points[7].mZ = farZ;
    transformShadowPoint(&mInverseView, &points[0], &worldPoints[0]);
    f32 z = worldPoints[0].mZ;
    f32 x = worldPoints[0].mX;
    mBounds.mMinimum.mZ = z;
    f32 y = worldPoints[0].mY;
    mBounds.mMinimum.mX = x;
    mBounds.mMaximum.mY = y;
    mBounds.mMaximum.mZ = z;
    mBounds.mMinimum.mY = y;
    mBounds.mMaximum.mX = x;
    for (u32 i = 1; i != 8; ++i) {
        transformShadowPoint(&mInverseView, &points[i], &worldPoints[i]);
        z = worldPoints[i].mZ;
        f32 minimumZ = mBounds.mMinimum.mZ;
        y = worldPoints[i].mY;
        f32 zFromMinimum = gabi::fsubs_ppc(z, minimumZ);
        f32 minimumY = mBounds.mMinimum.mY;
        f32 minimumX = mBounds.mMinimum.mX;
        f32 yFromMinimum = gabi::fsubs_ppc(y, minimumY);
        x = worldPoints[i].mX;
        f32 newMinimumZ = zFromMinimum >= 0 ? minimumZ : z;
        f32 xFromMinimum = gabi::fsubs_ppc(x, minimumX);
        f32 maximumX = mBounds.mMaximum.mX;
        f32 newMinimumY = yFromMinimum >= 0 ? minimumY : y;
        f32 xFromMaximum = gabi::fsubs_ppc(x, maximumX);
        f32 maximumY = mBounds.mMaximum.mY;
        f32 newMinimumX = xFromMinimum >= 0 ? minimumX : x;
        mBounds.mMinimum.mY = newMinimumY;
        f32 yFromMaximum = gabi::fsubs_ppc(y, maximumY);
        f32 maximumZ = mBounds.mMaximum.mZ;
        f32 newMaximumX = xFromMaximum >= 0 ? x : maximumX;
        mBounds.mMinimum.mZ = newMinimumZ;
        f32 zFromMaximum = gabi::fsubs_ppc(z, maximumZ);
        mBounds.mMinimum.mX = newMinimumX;
        f32 newMaximumY = yFromMaximum >= 0 ? y : maximumY;
        mBounds.mMaximum.mX = newMaximumX;
        f32 newMaximumZ = zFromMaximum >= 0 ? z : maximumZ;
        mBounds.mMaximum.mY = newMaximumY;
        mBounds.mMaximum.mZ = newMaximumZ;
    }
    transformShadowDirection(&mInverseView, &mLocalPlaneNormals[0], &mWorldPlaneNormals[0]);
    transformShadowDirection(&mInverseView, &mLocalPlaneNormals[1], &mWorldPlaneNormals[1]);
    transformShadowDirection(&mInverseView, &sForwardAxis, &mWorldPlaneNormals[2]);
    f32 axisY = sForwardAxis.mY;
    f32 axisX = sForwardAxis.mX;
    f32 negativeY = -axisY;
    f32 axisZ = sForwardAxis.mZ;
    f32 negativeX = -axisX;
    negativeAxis->mY = negativeY;
    u32 negativeYBits = readFloatWord(negativeAxis->mY);
    f32 negativeZ = -axisZ;
    negativeAxis->mX = negativeX;
    u32 negativeXBits = readFloatWord(negativeAxis->mX);
    writeFloatWord(negativeAxisCopy->mY, negativeYBits);
    negativeAxis->mZ = negativeZ;
    u32 negativeZBits = readFloatWord(negativeAxis->mZ);
    writeFloatWord(negativeAxisCopy->mX, negativeXBits);
    writeFloatWord(negativeAxisCopy->mZ, negativeZBits);
    transformShadowDirection(&mInverseView, negativeAxisCopy.get(), &mWorldPlaneNormals[3]);
    transformShadowDirection(&mInverseView, &mLocalPlaneNormals[2], &mWorldPlaneNormals[4]);
    transformShadowDirection(&mInverseView, &mLocalPlaneNormals[3], &mWorldPlaneNormals[5]);
    translation->copyColumn(&mInverseView, 3);
    u32 translationY = readFloatWord(translation->mY);
    u32 translationZ = readFloatWord(translation->mZ);
    u32 translationX = readFloatWord(translation->mX);
    writeFloatWord(translationCopy->mZ, translationZ);
    writeFloatWord(translationCopy->mX, translationX);
    writeFloatWord(translationCopy->mY, translationY);
    f32 tx = translationCopy->mX;
    f32 ty = translationCopy->mY;
    f32 tz = translationCopy->mZ;
    for (u32 i = 0; i != 6; ++i) {
        f32 px = tx, py = ty, pz = tz;
        if (i == 2 || i == 3) {
            const Vector3* point = &worldPoints[i == 2 ? 0 : 4];
            px = point->mX;
            py = point->mY;
            pz = point->mZ;
        }
        f32 nx = mWorldPlaneNormals[i].mX;
        f32 ny = mWorldPlaneNormals[i].mY;
        f32 nz = mWorldPlaneNormals[i].mZ;
        f32 productX = gabi::fmuls_ppc(px, nx);
        f32 productY = gabi::fmuls_ppc(py, ny);
        f32 sum = gabi::fadds_ppc(productX, productY);
        mPlaneOffsets[i] = -gabi::fmadds(pz, nz, sum);
    }
}

/* 0278D07C */
void CameraView::setView(const Matrix34* view) {
    gabi::AutoFrame frame{0x0278D07Cu};
    copyShadowView(view, &mView);
    invertShadowView(view, &mInverseView);
    rebuildVolume();
}

/* 0278D0D0 */
CameraView* CameraView::create(CameraView* view) {
    gabi::AutoFrame frame{0x0278D0D0u};
    if (view == nullptr) view = static_cast<CameraView*>(operator_new(sizeof(CameraView)));
    if (view == nullptr) return view;
    if (gabi::ea(view) == 0) operator_new(sizeof(Matrix34));
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mLocalPlaneNormals))) == 0) operator_new(sizeof(Matrix34));
    Bounds3* bounds = &view->mBounds;
    if (bounds == nullptr) bounds = static_cast<Bounds3*>(operator_new(sizeof(Bounds3)));
    if (bounds != nullptr) {
        bounds->mMinimum.mY = 3.4028234663852886e+38f;
        bounds->mMaximum.mX = -3.4028234663852886e+38f;
        bounds->mMinimum.mX = 3.4028234663852886e+38f;
        bounds->mMaximum.mY = -3.4028234663852886e+38f;
        bounds->mMinimum.mZ = 3.4028234663852886e+38f;
        bounds->mMaximum.mZ = -3.4028234663852886e+38f;
    }
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mWorldPlaneNormals))) == 0) operator_new(sizeof(view->mWorldPlaneNormals));
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mPlaneOffsets))) == 0) operator_new(sizeof(view->mPlaneOffsets));
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mCenter))) == 0) operator_new(sizeof(Pair2));
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mInverseView))) == 0) operator_new(sizeof(Matrix34));
    if ((gabi::ea(view) + static_cast<u32>(offsetof(CameraView, mProjection))) == 0) operator_new(sizeof(Matrix4));
    view->setPerspective(&sDefaultProjection, &sDefaultCenter,
                         static_cast<f64>(0.10000000149011612f), 10000.0,
                         static_cast<f64>(0.7853981852531433f),
                         static_cast<f64>(1.7777777910232544f));
    view->setView(&sDefaultView);
    return view;
}

/* WWHD glue
data sDefaultProjection 104A048C
data sDefaultView 104A041C
data sDefaultCenter 104A0908
data sForwardAxis 104A094C
ext 028E8970 void copyShadowProjection(const sead::RegistryMatrix44*,sead::RegistryMatrix44*)
ext 028E90D4 void copyShadowView(const agl::shadow_view::Matrix34*,agl::shadow_view::Matrix34*)
ext 028E91EC u32 invertShadowView(const agl::shadow_view::Matrix34*,agl::shadow_view::Matrix34*)
ext 028F423C f64 shadowViewTangent(f64)
ext 028EFFD0 void constructShadowVectors(agl::shadow_view::Vector3*,u32,u32,u32)
ext 028E8D4C void crossShadowVectors(const agl::shadow_view::Vector3*,const agl::shadow_view::Vector3*,agl::shadow_view::Vector3*)
ext 025155D8 f64 normalizeShadowVector(agl::shadow_view::Vector3*)
ext 028E8F64 void transformShadowPoint(const agl::shadow_view::Matrix34*,const agl::shadow_view::Vector3*,agl::shadow_view::Vector3*)
ext 028E9044 void transformShadowDirection(const agl::shadow_view::Matrix34*,const agl::shadow_view::Vector3*,agl::shadow_view::Vector3*)
*/
