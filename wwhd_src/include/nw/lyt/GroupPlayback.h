/* Group and group-array playback extensions proven by their binding operations. */
#pragma once
#include "nw/lyt/AnimationPlayback.h"
#include "nw/lyt/AnimationArchive.h"
#include "nw/lyt/PaneGroups.h"
namespace nw { namespace lyt {
struct GroupPlayback : PlaybackController {
    gptr<NamedPaneGroup> group;
    void destroy(u32 flags);
    void bindGroup(NamedPaneGroup* group, u32 enabled);
    void bindArchiveGroup(AnimationArchive* archive, PaneGroupList* groups, u32 index, u32 enabled);
};
struct GroupArrayPlayback : PlaybackController {
    gptr<gptr<NamedPaneGroup>> groups;
    be<u32> count;
    void destroy(u32 flags);
    void bindArchiveGroups(AnimationArchive* archive, PaneGroupList* groups, gptr<NamedPaneGroup>* storage, u32 enabled);
};
WWHD_OFFSET(GroupPlayback, group, 0x30);
WWHD_OFFSET(GroupArrayPlayback, groups, 0x30);
WWHD_OFFSET(GroupArrayPlayback, count, 0x34);
} }
