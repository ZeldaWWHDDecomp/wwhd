// Per-translation-unit parameter payloads and resource-match hooks.
// Layouts and behaviour are inferred from this game's binary; no external layout reference used.
// The payload types own this unit's copies of the value accessors (as env::EnvironmentScalarParameter
// does for its unit); kinds are agl::ParameterType, the colour payload is env::ParameterColor, and the
// hooks are this unit's method copies on AGL#9's parameter_tree::Value / Node.
#pragma once
#include "wwhd.h"
#include "agl/ParameterBase.h"
#include "agl/ParameterTree.h"

namespace agl { namespace parameter_value {
struct BooleanParameter {
    agl::env::ParameterBinding mBinding;
    be<u8> mValue;
    u8 mPadding[3];
    u32 valueKind() const;
    be<u8>* value();
    const be<u8>* value() const;
    u32 valueSize() const;
};
struct IntegerParameter {
    agl::env::ParameterBinding mBinding;
    be<s32> mValue;
    u32 valueKind() const;
    be<s32>* value();
    const be<s32>* value() const;
    u32 valueSize() const;
};
struct FloatParameter {
    agl::env::ParameterBinding mBinding;
    be<f32> mValue;
    u32 valueKind() const;
    be<f32>* value();
    const be<f32>* value() const;
    u32 valueSize() const;
};
struct ColorParameter {
    agl::env::ParameterBinding mBinding;
    agl::env::ParameterColor mValue;
    u32 valueKind() const;
    agl::env::ParameterColor* value();
    const agl::env::ParameterColor* value() const;
    u32 valueSize() const;
};

struct ParameterValueHooks { void onApply(); };
// A parameter object's hooks: the identifier it matches is its name hash, compared with the
// record's comparison word.
struct ParameterArchiveHooks : agl::parameter_tree::Value {
    void onApply();
    u32 acceptsRead() const;
    void beforeRead();
    u32 acceptsWrite() const;
    void beforeWrite();
    u32 matchesIdentifier(const gptr<agl::parameter_tree::MatchRecord>* record) const;
};
// A parameter list's hooks: the category it matches is its name hash, compared with the record's
// secondary word.
struct ParameterCategoryHooks : agl::parameter_tree::Node {
    u32 acceptsCategory() const;
    void beforeCategory();
    u32 matchesCategory(const gptr<agl::parameter_tree::MatchRecord>* record) const;
    void afterCategory();
    void beginTraversal();
    void endTraversal();
};
WWHD_OFFSET(BooleanParameter, mValue, 0xC);
WWHD_OFFSET(IntegerParameter, mValue, 0xC);
WWHD_OFFSET(FloatParameter, mValue, 0xC);
WWHD_OFFSET(ColorParameter, mValue, 0xC);
WWHD_SIZE(BooleanParameter, 0x10);
WWHD_SIZE(IntegerParameter, 0x10);
WWHD_SIZE(FloatParameter, 0x10);
WWHD_SIZE(ColorParameter, 0x1C);
WWHD_OFFSET(ParameterArchiveHooks, mNameHash, 0x50);
WWHD_OFFSET(ParameterCategoryHooks, mNameHash, 0x54);
} }
