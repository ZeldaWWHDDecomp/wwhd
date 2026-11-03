// Value layouts and type tags reconstructed from the local parameter accessors.
#pragma once
#include "sead/FileDevice.h"
#include "agl/ParameterBase.h"
namespace agl { namespace env {
// The 12-byte named value every agl parameter derives from (name hash +0, next parameter of its owner +4, methods +8):
// agl::ParameterBase (agl/ParameterBase.h), which also declares its methods.
using ParameterBinding = agl::ParameterBase;
enum class ParameterValueKind : u32 { Scalar = 1, Integer = 2, Vector3 = 4, Color = 6 };
struct ParameterVector3 { be<f32> x, y, z; };
struct ParameterColor { be<f32> r, g, b, a; };
}}
WWHD_SIZE(agl::env::ParameterVector3,12);
WWHD_SIZE(agl::env::ParameterColor,16);
