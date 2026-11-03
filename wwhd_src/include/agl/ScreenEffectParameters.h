/* The per-translation-unit copies of the agl parameter classes' inline virtuals that GHS emitted for the SSAO
 * (effect_parameters, ssao_parameters) and shadow-map (shadow_parameters) sources: the value accessors of the
 * Boolean/Integer/Scalar/Pair parameters and their text class (the parameter-tree hooks are in
 * agl/ParameterComparisons.h). Each
 * namespace is one set of copies with its own vtables. The parameters derive from the merged agl::ParameterBase. */
#pragma once
#include "sead/FileDevice.h"
#include "agl/ParameterBase.h"
namespace agl {
namespace effect_parameters {
struct Parameter : ParameterBase { void emptyHook(); };
struct Text : sead::SafeText { static void destroy(Text* text, u32 flags); void assureTerminationImpl(); };
struct BooleanParameter : Parameter {
    be<u8> mValue;
    u8 mPadding[3];
    u32 getType() const;
    be<u8>* getValue();
    const be<u8>* getValue() const;
    u32 getValueSize() const;
};
struct IntegerParameter : Parameter {
    be<s32> mValue;
    u32 getType() const;
    be<s32>* getValue();
    const be<s32>* getValue() const;
    u32 getValueSize() const;
};
struct ScalarParameter : Parameter {
    be<f32> mValue;
    u32 getType() const;
    be<f32>* getValue();
    const be<f32>* getValue() const;
    u32 getValueSize() const;
};
// A two-component value (eight bytes).
struct PairParameter : Parameter {
    be<u32> mValue[2];
    u32 getType() const;
    be<u32>* getValue();
    const be<u32>* getValue() const;
    u32 getValueSize() const;
};
}
namespace ssao_parameters {
struct Parameter : ParameterBase { void emptyHook(); };
struct Text : sead::SafeText { static void destroy(Text* text, u32 flags); void assureTerminationImpl(); };
struct BooleanParameter : Parameter {
    be<u8> mValue;
    u8 mPadding[3];
    u32 getType() const;
    be<u8>* getValue();
    const be<u8>* getValue() const;
    u32 getValueSize() const;
};
struct IntegerParameter : Parameter {
    be<s32> mValue;
    u32 getType() const;
    be<s32>* getValue();
    const be<s32>* getValue() const;
    u32 getValueSize() const;
};
struct ScalarParameter : Parameter {
    be<f32> mValue;
    u32 getType() const;
    be<f32>* getValue();
    const be<f32>* getValue() const;
    u32 getValueSize() const;
};
}
namespace shadow_parameters {
struct Parameter : ParameterBase { void emptyHook(); };
struct Text : sead::SafeText { static void destroy(Text* text, u32 flags); void assureTerminationImpl(); };
struct BooleanParameter : Parameter {
    be<u8> mValue;
    u8 mPadding[3];
    u32 getType() const;
    be<u8>* getValue();
};
// The shadow source has no out-of-line accessors of these two; only their layout is used.
struct IntegerParameter : Parameter { be<s32> mValue; };
struct FloatParameter : Parameter { be<f32> mValue; };
}
namespace screen_effect {
struct Vector3 { be<f32> mComponents[3]; };
struct AffineMatrix { be<f32> mRows[3][4]; };
void addTranslationToMatrix(AffineMatrix* output, const Vector3* translation, const AffineMatrix* source);
}
namespace ssao_storage { void* allocateOrReuseStorage(void* storage); }
}
WWHD_SIZE(agl::effect_parameters::Parameter, 12);
WWHD_OFFSET(agl::effect_parameters::BooleanParameter, mValue, 12);
WWHD_SIZE(agl::effect_parameters::BooleanParameter, 16);
WWHD_OFFSET(agl::effect_parameters::IntegerParameter, mValue, 12);
WWHD_SIZE(agl::effect_parameters::IntegerParameter, 16);
WWHD_OFFSET(agl::effect_parameters::ScalarParameter, mValue, 12);
WWHD_SIZE(agl::effect_parameters::ScalarParameter, 16);
WWHD_OFFSET(agl::effect_parameters::PairParameter, mValue, 12);
WWHD_SIZE(agl::effect_parameters::PairParameter, 20);
WWHD_SIZE(agl::ssao_parameters::Parameter, 12);
WWHD_OFFSET(agl::ssao_parameters::BooleanParameter, mValue, 12);
WWHD_SIZE(agl::ssao_parameters::BooleanParameter, 16);
WWHD_OFFSET(agl::ssao_parameters::IntegerParameter, mValue, 12);
WWHD_SIZE(agl::ssao_parameters::IntegerParameter, 16);
WWHD_OFFSET(agl::ssao_parameters::ScalarParameter, mValue, 12);
WWHD_SIZE(agl::ssao_parameters::ScalarParameter, 16);
WWHD_SIZE(agl::shadow_parameters::Parameter, 12);
WWHD_OFFSET(agl::shadow_parameters::BooleanParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameters::BooleanParameter, 16);
WWHD_OFFSET(agl::shadow_parameters::IntegerParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameters::IntegerParameter, 16);
WWHD_OFFSET(agl::shadow_parameters::FloatParameter, mValue, 12);
WWHD_SIZE(agl::shadow_parameters::FloatParameter, 16);
WWHD_SIZE(agl::screen_effect::Vector3, 12);
WWHD_SIZE(agl::screen_effect::AffineMatrix, 48);
