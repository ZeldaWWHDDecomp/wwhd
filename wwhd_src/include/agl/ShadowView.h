// Shadow-view layout reconstructed from this unit's constructor and matrix accesses.
#pragma once
#include "agl/EffectMatrixSupport.h"
#include "sead/HeapRegistry.h"
namespace agl::shadow_view {
using effect_matrix::Matrix34;
using effect_matrix::Matrix4;
using effect_matrix::Vector3;
struct Pair2 { be<f32> mX, mY; };
struct Bounds3 { Vector3 mMinimum, mMaximum; };
struct CameraView {
    Matrix34 mView;
    be<f32> mNear, mFar;
    Vector3 mLocalPlaneNormals[4];
    Bounds3 mBounds;
    Vector3 mWorldPlaneNormals[6];
    be<f32> mPlaneOffsets[6];
    be<f32> mFieldOfView, mAspect, mHalfAngleTangent, mHalfAngleCotangent;
    Pair2 mCenter;
    Matrix34 mInverseView;
    sead::RegistryMatrix44 mProjection;
    static CameraView* create(CameraView* view);
    void setPerspective(const sead::RegistryMatrix44* projection, const Pair2* center,
                        f64 nearPlane, f64 farPlane, f64 fieldOfView, f64 aspect);
    void setView(const Matrix34* view);
    void rebuildVolume();
};
}
WWHD_OFFSET(agl::shadow_view::Pair2, mX, 0);
WWHD_OFFSET(agl::shadow_view::Pair2, mY, 4);
WWHD_SIZE(agl::shadow_view::Pair2, 8);
WWHD_OFFSET(agl::shadow_view::Bounds3, mMinimum, 0);
WWHD_OFFSET(agl::shadow_view::Bounds3, mMaximum, 12);
WWHD_SIZE(agl::shadow_view::Bounds3, 24);
WWHD_OFFSET(agl::shadow_view::CameraView, mView, 0);
WWHD_OFFSET(agl::shadow_view::CameraView, mNear, 0x30);
WWHD_OFFSET(agl::shadow_view::CameraView, mFar, 0x34);
WWHD_OFFSET(agl::shadow_view::CameraView, mLocalPlaneNormals, 0x38);
WWHD_OFFSET(agl::shadow_view::CameraView, mBounds, 0x68);
WWHD_OFFSET(agl::shadow_view::CameraView, mWorldPlaneNormals, 0x80);
WWHD_OFFSET(agl::shadow_view::CameraView, mPlaneOffsets, 0xC8);
WWHD_OFFSET(agl::shadow_view::CameraView, mFieldOfView, 0xE0);
WWHD_OFFSET(agl::shadow_view::CameraView, mAspect, 0xE4);
WWHD_OFFSET(agl::shadow_view::CameraView, mHalfAngleTangent, 0xE8);
WWHD_OFFSET(agl::shadow_view::CameraView, mHalfAngleCotangent, 0xEC);
WWHD_OFFSET(agl::shadow_view::CameraView, mCenter, 0xF0);
WWHD_OFFSET(agl::shadow_view::CameraView, mInverseView, 0xF8);
WWHD_OFFSET(agl::shadow_view::CameraView, mProjection, 0x128);
WWHD_SIZE(agl::shadow_view::CameraView, 0x168);
void copyShadowProjection(const sead::RegistryMatrix44*, sead::RegistryMatrix44*);
void copyShadowView(const agl::shadow_view::Matrix34*, agl::shadow_view::Matrix34*);
u32 invertShadowView(const agl::shadow_view::Matrix34*, agl::shadow_view::Matrix34*);
f64 shadowViewTangent(f64);
void constructShadowVectors(agl::shadow_view::Vector3*, u32, u32, u32);
void crossShadowVectors(const agl::shadow_view::Vector3*, const agl::shadow_view::Vector3*, agl::shadow_view::Vector3*);
f64 normalizeShadowVector(agl::shadow_view::Vector3*);
void transformShadowPoint(const agl::shadow_view::Matrix34*, const agl::shadow_view::Vector3*, agl::shadow_view::Vector3*);
void transformShadowDirection(const agl::shadow_view::Matrix34*, const agl::shadow_view::Vector3*, agl::shadow_view::Vector3*);
