// Narrow value views for the projection object's parameter accessors.
// Layouts reconstructed from local WWHD loads and address-return instructions.
#pragma once
#include "agl/ParameterValue.h"

namespace agl { namespace env {
using ProjectionVectorValue = ParameterVector3;
struct ProjectionIntegerParameter {
    ParameterBinding mBinding;
    be<s32> mValue;
    u32 valueKind() const;
    be<s32>* value();
    const be<s32>* value() const;
    u32 valueBytes() const;
};
struct ProjectionScalarParameter {
    ParameterBinding mBinding;
    be<f32> mValue;
    u32 valueKind() const;
    be<f32>* value();
    const be<f32>* value() const;
    u32 valueBytes() const;
};
struct ProjectionVectorParameter {
    ParameterBinding mBinding;
    ParameterVector3 mValue;
    u32 valueKind() const;
    ParameterVector3* value();
    const ParameterVector3* value() const;
    u32 valueBytes() const;
};
}}
WWHD_OFFSET(agl::env::ProjectionIntegerParameter,mValue,12);
WWHD_SIZE(agl::env::ProjectionIntegerParameter,16);
WWHD_OFFSET(agl::env::ProjectionScalarParameter,mValue,12);
WWHD_SIZE(agl::env::ProjectionScalarParameter,16);
WWHD_OFFSET(agl::env::ProjectionVectorParameter,mValue,12);
WWHD_SIZE(agl::env::ProjectionVectorParameter,24);
