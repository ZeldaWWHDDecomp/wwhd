// Camera-override state and methods from this unit's 0x74-byte constructor.
#pragma once
#include "agl/ShadowView.h"
namespace agl::shadow_override {
using effect_matrix::Matrix34;
using effect_matrix::Vector3;
struct CameraOverride {
    Matrix34 mView;
    sead::RegistryMatrix44 mProjection;
    be<u8> mEnabled;
    u8 mPadding[3];
    static CameraOverride* create(CameraOverride* state);
    void setDirection(const Vector3* direction);
    void applyWithReference(Matrix34* view, sead::RegistryMatrix44* projection, const Matrix34* reference);
    void applyWithUp(Matrix34* view, sead::RegistryMatrix44* projection, const Vector3* up);
};
}
WWHD_OFFSET(agl::shadow_override::CameraOverride, mView, 0);
WWHD_OFFSET(agl::shadow_override::CameraOverride, mProjection, 0x30);
WWHD_OFFSET(agl::shadow_override::CameraOverride, mEnabled, 0x70);
WWHD_SIZE(agl::shadow_override::CameraOverride, 0x74);
void setShadowOverrideProjection(sead::RegistryMatrix44*, f64, f64, f64);
