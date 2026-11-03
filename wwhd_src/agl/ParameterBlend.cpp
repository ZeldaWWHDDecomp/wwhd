// Distinct blend-tree callback bodies reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/ParameterBlend.h"
void updateUngroupedBlendEntry(agl::ParameterBlendEntryPrefix*);
void updateGroupedBlendEntry(agl::ParameterBlendEntryPrefix*);
/* 0279D78C */
u32 agl::ParameterBlendTreePrefix::constantOneHook() { return 1; }
/* 0279D794 */
void agl::ParameterBlendTreePrefix::emptyHook() {}
/* 0279D798 */
u32 agl::ParameterBlendTreePrefix::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    const parameter_tree::MatchRecord* selected = *record;
    u32 ownHash = mTree.mNameHash;
    return ownHash == (u32)selected->mSecondaryValue;
}
/* 0279D7B4 */
void agl::ParameterBlendTreePrefix::trailingEmptyHook() {}
/* 0279D7B8 */
void agl::ParameterBlendTreePrefix::alternateTrailingEmptyHook() {}
/* 0279D7BC */
u32 agl::compareParameterBlendEntries(const gptr<ParameterBlendEntryPrefix>* first,
                                     const gptr<ParameterBlendEntryPrefix>* second) {
    ParameterBlendEntryPrefix* left = *first;
    ParameterBlendEntryPrefix* right = *second;
    u32 leftOrder = left->mSortWord;
    return leftOrder - (u32)right->mSortWord;
}
/* 0279D7D4 */
void agl::ParameterBlendTreePrefix::updateGroupedEntries() {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::Link* sentinel = &mUngrouped;
    sead::Link* current = mUngrouped.mNext;
    sead::Link* next = current->mNext;
    while (current != sentinel) {
        auto* node = reinterpret_cast<ParameterBlendEntryLink*>(current);
        ParameterBlendEntryPrefix* entry = node->mEntry;
        if ((u8)entry->mGrouped == 0) updateUngroupedBlendEntry(entry);
        current = next;
        next = next->mNext;
    }
    for (u32 group = 0; group < 7; ++group) {
        sentinel = &mGroups[group];
        current = mGroups[group].mNext;
        next = current->mNext;
        while (current != sentinel) {
            auto* node = reinterpret_cast<ParameterBlendEntryLink*>(current);
            ParameterBlendEntryPrefix* entry = node->mEntry;
            if ((u8)entry->mGrouped != 0) updateGroupedBlendEntry(entry);
            current = next;
            next = next->mNext;
        }
    }
    mUngrouped.sort(-8, gabi::guest_fn(reinterpret_cast<const void*>(&compareParameterBlendEntries)));
}
/* WWHD glue
 ext 027AC200 void updateUngroupedBlendEntry(agl::ParameterBlendEntryPrefix*)
 ext 027AC2D4 void updateGroupedBlendEntry(agl::ParameterBlendEntryPrefix*)
 ext 0273B378 void sead::LinkList::sort(s32, u32)
*/
