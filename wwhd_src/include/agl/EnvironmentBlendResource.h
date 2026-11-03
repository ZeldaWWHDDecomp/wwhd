// Relative resource prefixes read by the manager's blending routine and local list accessors.
#pragma once
#include "agl/EnvironmentSet.h"
#include "agl/ParameterRoot.h"
namespace agl { namespace env {
// A parameter archive (its header is the parameter root's input record; the root list follows the header at the
// payload offset) and its lists (parameter-tree input records, linked by their byte step).
struct EnvironmentArchiveReference { gptr<ParameterRootInput_l> mArchive; };
inline parameter_tree::InputRecord_l* nextBlendList(const parameter_tree::InputRecord_l* list) {
    return relativeNode<parameter_tree::InputRecord_l>(list, u32(list->mByteStep));
}
struct EnvironmentBlendListReference { gptr<parameter_tree::InputRecord_l> mList; };
}}
WWHD_SIZE(agl::env::EnvironmentBlendListReference,4);
WWHD_SIZE(agl::env::EnvironmentArchiveReference,4);
