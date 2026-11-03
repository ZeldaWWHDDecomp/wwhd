// WWHD effect-matrix helpers; layouts reconstructed from this unit's instructions.
// Value types with this TU's method copies. Layout-equivalent merged types: Matrix34 (48 bytes) ~
// sead::ViewTransform, Matrix4 (64 bytes) ~ sead::RegistryMatrix44, Vector3 (12 bytes) ~ sead::ViewVector.
#pragma once
#include "wwhd.h"
#include "sead/ViewTransform.h"
#include "sead/FileDevice.h"

namespace agl::effect_matrix {
struct Vector3;
struct Row4 { be<f32> mX, mY, mZ, mW; };
struct Matrix34 {
    Row4 mRows[3];
    void setLookAt(const Vector3* position, const Vector3* direction, const Vector3* up);
};
struct Matrix4 {
    Row4 mRows[4];
    static Matrix4* create(Matrix4* matrix);
    void setRows(const Matrix34* affine, const Row4* lastRow);
};
struct Matrix3 {
    be<f32> mElements[3][3];
    void copyRotation(const Matrix34* source);
};
struct Vector3 {
    be<f32> mX, mY, mZ;
    static Vector3* create(Vector3* vector);
    void copyColumn(const Matrix34* source, u32 column);
    void setInverseTranslation(const Matrix34* source);
    f64 setNegativeForward(const Matrix34* source);
};
// The look-at camera's method table (GHS, 8-byte slots; sead::ViewCamera::mVtable): slot 4 rebuilds the view matrix.
// No merged unit models this table; only the slot this unit calls is named.
struct LookAtCameraMethods { sead::VtableSlot mSlots[4]; sead::VtableSlot mUpdateView; };
u32 packShaderChoice(u32 inner, u32 horizontal, u32 vertical, u32 outer,
                     u32 blur, u32 fade, u32 output);
}
WWHD_OFFSET(agl::effect_matrix::Row4, mX, 0);
WWHD_OFFSET(agl::effect_matrix::Row4, mY, 4);
WWHD_OFFSET(agl::effect_matrix::Row4, mZ, 8);
WWHD_OFFSET(agl::effect_matrix::Row4, mW, 12);
WWHD_SIZE(agl::effect_matrix::Row4, 16);
WWHD_OFFSET(agl::effect_matrix::Matrix34, mRows, 0);
WWHD_SIZE(agl::effect_matrix::Matrix34, 48);
WWHD_OFFSET(agl::effect_matrix::Matrix4, mRows, 0);
WWHD_SIZE(agl::effect_matrix::Matrix4, 64);
WWHD_OFFSET(agl::effect_matrix::Matrix3, mElements, 0);
WWHD_SIZE(agl::effect_matrix::Matrix3, 36);
WWHD_OFFSET(agl::effect_matrix::Vector3, mX, 0);
WWHD_OFFSET(agl::effect_matrix::Vector3, mY, 4);
WWHD_OFFSET(agl::effect_matrix::Vector3, mZ, 8);
WWHD_SIZE(agl::effect_matrix::Vector3, 12);
f64 normalizeEffectDirection(agl::effect_matrix::Vector3*);

WWHD_OFFSET(agl::effect_matrix::LookAtCameraMethods,mUpdateView,0x20);
WWHD_SIZE(agl::effect_matrix::LookAtCameraMethods,0x28);
