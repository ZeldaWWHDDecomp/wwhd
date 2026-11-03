/* Group playback binding with callback-sensitive field reloads from WWHD. */
#include "nw/lyt/GroupPlayback.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/lyt/AnimationBinding.h"
using namespace nw::lyt;
struct GroupPlaybackMethods {
    u8 prefix[0x24]; be<u32> enabled; u8 middle[0x34]; be<u32> bind;
};

/* 02873058 */
void GroupPlayback::bindGroup(NamedPaneGroup* target, u32 enabled) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* dispatch = reinterpret_cast<GroupPlaybackMethods*>((AnimatorVtable*)__vtbl);
    gabi::call_ptr<void>(dispatch->bind, this, target);
    dispatch = reinterpret_cast<GroupPlaybackMethods*>((AnimatorVtable*)__vtbl);
    group = target;
    gabi::call_ptr<void>(dispatch->enabled, this, enabled);
}

/* 028730C4 */
void GroupPlayback::bindArchiveGroup(AnimationArchive* archive, PaneGroupList* groups, u32 index, u32 enabled) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 count = archive->groupCount();
    if (index >= count) return;
    const be<u8>* names = archive->name();
    auto* name = layoutAtOffset<be<u8>>(names, index * 28);
    NamedPaneGroup* target = groups->find(name);
    bindGroup(target, enabled);
}

/* 02873134 */
void GroupArrayPlayback::bindArchiveGroups(AnimationArchive* archive, PaneGroupList* sourceGroups, gptr<NamedPaneGroup>* storage, u32 enabled) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    count = archive->groupCount();
    u32 limit = count;
    groups = storage;
    u32 index = 0;
    while (index < limit) {
        const be<u8>* names = archive->name();
        auto* name = layoutAtOffset<be<u8>>(names, index * 28);
        gptr<NamedPaneGroup>* destinations = groups;
        NamedPaneGroup* target = sourceGroups->find(name);
        destinations[index] = target;
        auto* dispatch = reinterpret_cast<GroupPlaybackMethods*>((AnimatorVtable*)__vtbl);
        destinations = groups;
        target = destinations[index];
        gabi::call_ptr<void>(dispatch->bind, this, target);
        limit = count;
        ++index;
    }
    auto* dispatch = reinterpret_cast<GroupPlaybackMethods*>((AnimatorVtable*)__vtbl);
    gabi::call_ptr<void>(dispatch->enabled, this, enabled);
}

/* The allocation paths 0287F740 and 0287F858 install tables 101800B8 and
 * 10180198 after constructing 0x34 and 0x38-byte playback extensions. Their
 * binding calls establish the single-group and group-array ownership. */
/* 028731FC */
void GroupPlayback::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    PlaybackController::destroy(0);
    if (flags & 1) animationDelete(this);
}

/* 02873250 */
void GroupArrayPlayback::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    PlaybackController::destroy(0);
    if (flags & 1) animationDelete(this);
}

/* WWHD glue
*/
