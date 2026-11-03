// Narrow parameter value views for the environment-object family.
// Only the value at byte12 is accessed here; metadata is preserved as opaque storage.
#pragma once
#include "agl/ParameterValue.h"
namespace agl { namespace env {
struct EnvironmentScalarParameter {
    ParameterBinding mBinding;
    be<f32> mValue;
    u32 valueKind() const;
    be<f32>* value();
    const be<f32>* value() const;
    u32 valueBytes() const;
};
struct EnvironmentVectorParameter {
    ParameterBinding mBinding;
    ParameterVector3 mValue;
    u32 valueKind() const;
    ParameterVector3* value();
    const ParameterVector3* value() const;
    u32 valueBytes() const;
};
struct EnvironmentColorParameter {
    ParameterBinding mBinding;
    ParameterColor mValue;
    u32 valueKind() const;
    ParameterColor* value();
    const ParameterColor* value() const;
    u32 valueBytes() const;
};
}}
WWHD_OFFSET(agl::env::EnvironmentScalarParameter,mValue,12);
WWHD_SIZE(agl::env::EnvironmentScalarParameter,16);
WWHD_OFFSET(agl::env::EnvironmentVectorParameter,mValue,12);
WWHD_SIZE(agl::env::EnvironmentVectorParameter,24);
WWHD_OFFSET(agl::env::EnvironmentColorParameter,mValue,12);
WWHD_SIZE(agl::env::EnvironmentColorParameter,28);
