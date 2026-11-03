/* The parameter-tree hooks of the SSAO translation unit: its copies of the inline virtuals of a parameter-tree
 * value (parameter object), node (parameter list) and owned parameter object, emitted twice (effect_parameters, ssao_parameters), each
 * with its own vtables. matchesRecord compares a resource record's name hash with the value's / node's own. */
#pragma once
#include "agl/ScreenEffectParameters.h"
#include "agl/ParameterTree.h"
namespace agl {
namespace effect_parameters {
struct ParameterObject : parameter_tree::Value {
    u32 constantOneHook() const;
    void emptyHook();
    u32 alternateConstantOneHook() const;
    void alternateEmptyHook();
    bool matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const;
};
struct ParameterList : parameter_tree::Node {
    u32 constantOneHook() const;
    bool matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const;
    void emptyHook();
    void trailingEmptyHook();
    void alternateTrailingEmptyHook();
};
// A parameter object that belongs to an effect: after a resource was applied it calls the owner's after-apply hook.
struct OwnedParameterObject : ParameterObject {
    gptr<parameter_tree::Node> mOwner;
    void notifyOwnerApplied();
};
}
namespace ssao_parameters {
struct ParameterObject : parameter_tree::Value {
    u32 constantOneHook() const;
    void emptyHook();
    u32 alternateConstantOneHook() const;
    void alternateEmptyHook();
    bool matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const;
};
struct ParameterList : parameter_tree::Node {
    u32 constantOneHook() const;
    bool matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const;
    void emptyHook();
    void trailingEmptyHook();
    void alternateTrailingEmptyHook();
};
// A parameter object that belongs to an effect: after a resource was applied it calls the owner's after-apply hook.
struct OwnedParameterObject : ParameterObject {
    gptr<parameter_tree::Node> mOwner;
    void notifyOwnerApplied();
};
}
}
WWHD_SIZE(agl::effect_parameters::ParameterObject, 0x5C);
WWHD_SIZE(agl::effect_parameters::ParameterList, 0x64);
WWHD_OFFSET(agl::effect_parameters::OwnedParameterObject, mOwner, 0x5C);
WWHD_SIZE(agl::effect_parameters::OwnedParameterObject, 0x60);
WWHD_SIZE(agl::ssao_parameters::ParameterObject, 0x5C);
WWHD_SIZE(agl::ssao_parameters::ParameterList, 0x64);
WWHD_OFFSET(agl::ssao_parameters::OwnedParameterObject, mOwner, 0x5C);
WWHD_SIZE(agl::ssao_parameters::OwnedParameterObject, 0x60);
