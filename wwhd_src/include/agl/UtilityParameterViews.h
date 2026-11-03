// Local value-accessor families using the canonical parameter binding and tags.
#pragma once
#include "agl/ParameterValue.h"
namespace agl { namespace utility_parameters {
struct ScalarParameter {
    env::ParameterBinding mBinding;
    be<f32> mValue;
    u32 valueKind() const;
    be<f32>* value();
    const be<f32>* value() const;
    u32 valueBytes() const;
};
struct VectorParameter {
    env::ParameterBinding mBinding;
    env::ParameterVector3 mValue;
    u32 valueKind() const;
    env::ParameterVector3* value();
    const env::ParameterVector3* value() const;
    u32 valueBytes() const;
};
struct MatrixModeParameter {
    env::ParameterBinding mBinding;
    be<s32> mValue;
    u32 valueKind() const;
    be<s32>* value();
    const be<s32>* value() const;
    u32 valueBytes() const;
};
} }
WWHD_OFFSET(agl::utility_parameters::ScalarParameter, mValue, 12);
WWHD_SIZE(agl::utility_parameters::ScalarParameter, 16);
WWHD_OFFSET(agl::utility_parameters::VectorParameter, mValue, 12);
WWHD_SIZE(agl::utility_parameters::VectorParameter, 24);
WWHD_OFFSET(agl::utility_parameters::MatrixModeParameter, mValue, 12);
WWHD_SIZE(agl::utility_parameters::MatrixModeParameter, 16);
