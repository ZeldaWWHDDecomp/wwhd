// Projection-shadow fields reconstructed from this unit's parameter constructor.
#pragma once
#include "agl/ShadowView.h"
#include "agl/ShadowParameterInstances.h"
#include "agl/TextureSampler.h"
#include "agl/ParameterRoot.h"
#include "agl/ParameterObject.h"
namespace agl::projection_shadow {
using effect_matrix::Matrix34;
using effect_matrix::Vector3;
using shadow_view::Pair2;
struct PairParameter {
    shadow_parameter::ParameterBaseHooks mBase;
    Pair2 mValue;
};
struct ProjectionShadow {
    gptr<void> mObjectTable;
    ParameterRoot mRoot;
    TextureSampler mTexture;
    Matrix34 mBiasMatrix;
    Pair2 mAnimationOffset;
    be<f32> mAnimationAngle;
    Pair2 mSwingOffset;
    Pair2 mTranslationOffset;
    ParameterObject mParameterContainer;
    gptr<void> mParameterIoOwner;
    PairParameter mScaleBias;
    PairParameter mAnimationVelocity;
    shadow_parameter::IntegerParameter mSwingCycleX;
    shadow_parameter::IntegerParameter mSwingCycleY;
    PairParameter mSwingAmplitude;
    PairParameter mTranslationBias;
    shadow_parameter::FloatParameter mRotationSpeed;
    shadow_parameter::FloatParameter mRotationBias;
    shadow_parameter::BooleanParameter mRepeat;
    be<u8> mAnimationEnabled;
    u8 mAnimationPadding[3];
    be<f32> mRotationBiasDegrees;
    be<f32> mRotationSpeedDegrees;
    Matrix34* rebuildBiasMatrix();
    Matrix34* resetAnimation();
    Matrix34* applySettings();
    static Matrix34* applyFromParameterIo(void* parameterIo);
    static void destroy(ProjectionShadow* shadow, u32 flags);
    static ProjectionShadow* create(ProjectionShadow* shadow);
};
}
WWHD_OFFSET(agl::projection_shadow::PairParameter, mValue, 12);
WWHD_SIZE(agl::projection_shadow::PairParameter, 20);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mTexture, 0x1CC);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mObjectTable, 0);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRoot, 4);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mBiasMatrix, 0x364);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mAnimationOffset, 0x394);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mAnimationAngle, 0x39C);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mSwingOffset, 0x3A0);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mTranslationOffset, 0x3A8);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mParameterContainer, 0x3B0);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mParameterIoOwner, 0x40C);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mScaleBias, 0x410);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mAnimationVelocity, 0x424);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mSwingCycleX, 0x438);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mSwingCycleY, 0x448);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mSwingAmplitude, 0x458);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mTranslationBias, 0x46C);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRotationSpeed, 0x480);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRotationBias, 0x490);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRepeat, 0x4A0);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mAnimationEnabled, 0x4B0);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRotationBiasDegrees, 0x4B4);
WWHD_OFFSET(agl::projection_shadow::ProjectionShadow, mRotationSpeedDegrees, 0x4B8);
WWHD_SIZE(agl::projection_shadow::ProjectionShadow, 0x4BC);
void setProjectionBiasRotation(agl::projection_shadow::Matrix34*, const agl::projection_shadow::Vector3*);
void scaleProjectionBias(agl::projection_shadow::Matrix34*, const agl::projection_shadow::Matrix34*, const agl::projection_shadow::Vector3*);
void translateProjectionBias(agl::projection_shadow::Matrix34*, const agl::projection_shadow::Vector3*, const agl::projection_shadow::Matrix34*);
