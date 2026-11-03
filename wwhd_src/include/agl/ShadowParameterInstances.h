// This translation unit's copies of the agl parameter methods (GHS emits them per TU, as agl/ParameterValues.h's
// payload types own theirs): the payload types derive their data layout from the merged agl::ParameterBase.
#pragma once
#include "wwhd.h"
#include "agl/ParameterBase.h"

namespace agl::shadow_parameter {
// agl::ParameterBase with this unit's copies of two of its methods.
struct ParameterBaseHooks : agl::ParameterBase {
    void emptyHook();
    void* valueAddress();
};
struct BooleanParameter {
    ParameterBaseHooks mBase;
    be<u8> mValue;
    u8 mPadding[3];
    u32 kind() const;
    void* valueAddress();
    void* alternateValueAddress();
    u32 valueSize() const;
};
struct IntegerParameter {
    ParameterBaseHooks mBase;
    be<u32> mValue;
    u32 kind() const;
    void* valueAddress();
    void* alternateValueAddress();
    u32 valueSize() const;
};
struct FloatParameter {
    ParameterBaseHooks mBase;
    be<f32> mValue;
    u32 kind() const;
    void* valueAddress();
    void* alternateValueAddress();
    u32 valueSize() const;
};
}
WWHD_SIZE(agl::shadow_parameter::ParameterBaseHooks, 12);
WWHD_OFFSET(agl::shadow_parameter::BooleanParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameter::BooleanParameter, 16);
WWHD_OFFSET(agl::shadow_parameter::IntegerParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameter::IntegerParameter, 16);
WWHD_OFFSET(agl::shadow_parameter::FloatParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameter::FloatParameter, 16);
