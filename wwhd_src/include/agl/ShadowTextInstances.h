// This translation unit's copies of the sead::SafeText methods (per-TU, as agl/ParameterObject.h's parameter_object_text).
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"

namespace agl::shadow_prepass_text {
struct Text : sead::SafeText {
    static Text* create(Text* text, u32 contentAddress);
    void destroy(u32 flags);
    void assureTermination();
};
}
