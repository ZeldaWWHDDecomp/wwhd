// This translation unit's copies of the parameter-tree and resource-record methods (GHS emits them per TU; the
// agl/ParameterValues.h precedent): hook types deriving from the merged parameter_tree::Value / Node and ResourceRecord.
#pragma once
#include "wwhd.h"
#include "agl/ParameterTree.h"
#include "agl/TextureResource.h"
namespace agl::shadow_control {
using parameter_tree::MatchRecord;
// parameter_tree::Value with this unit's copies of five of its methods.
struct ValueHooks : parameter_tree::Value {
    u32 constantOneHook() const;
    void emptyHook();
    u32 alternateConstantOneHook() const;
    void alternateEmptyHook();
    u32 matchesRecord(const gptr<MatchRecord>* record) const;
};
// parameter_tree::Node with this unit's copies of six of its methods.
struct NodeHooks : parameter_tree::Node {
    u32 constantOneHook() const;
    void emptyHook();
    u32 matchesRecord(const gptr<MatchRecord>* record) const;
    void alternateEmptyHook();
    void trailingEmptyHook();
    void finalEmptyHook();
};
// The pre-pass's control record (a ResourceRecord) with this unit's copy of its deleting destructor.
struct ControlRecord : ResourceRecord {
    static void destroy(ControlRecord* record, u32 flags);
};
}
