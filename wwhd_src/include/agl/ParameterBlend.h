// Tree prefix observed in assigned blend callbacks; complete derived extent is not asserted.
#pragma once
#include "agl/ParameterTree.h"
namespace agl {
struct ParameterBlendEntryPrefix {
    u8 mOpaque[0x100]; be<u8> mGrouped; u8 mPadding[15]; be<u32> mSortWord;
};
struct ParameterBlendEntryLink { sead::Link mLink; gptr<ParameterBlendEntryPrefix> mEntry; };
u32 compareParameterBlendEntries(const gptr<ParameterBlendEntryPrefix>* first,
                                const gptr<ParameterBlendEntryPrefix>* second);
struct ParameterBlendTreePrefix {
    parameter_tree::Node mTree;
    u8 mOpaque[0x168];
    sead::LinkList mUngrouped;
    sead::LinkList mGroups[7];
    u32 constantOneHook(); void emptyHook();
    u32 matchesRecord(const gptr<parameter_tree::MatchRecord>* record);
    void trailingEmptyHook(); void alternateTrailingEmptyHook();
    void updateGroupedEntries();
};
}
WWHD_OFFSET(agl::ParameterBlendTreePrefix, mTree, 0);

WWHD_OFFSET(agl::ParameterBlendTreePrefix, mUngrouped, 0x1CC);
WWHD_OFFSET(agl::ParameterBlendTreePrefix, mGroups, 0x1D8);
WWHD_OFFSET(agl::ParameterBlendEntryPrefix, mGrouped, 0x100);
WWHD_OFFSET(agl::ParameterBlendEntryPrefix, mSortWord, 0x110);
WWHD_OFFSET(agl::ParameterBlendEntryLink, mEntry, 8);
