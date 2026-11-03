// WWHD render-entry list transitions, from this unit's instructions.
#include "agl/RenderEntry.h"
/* 027AC144 */
void agl::RenderEntry::attach(RenderQueueOwner_l* owner, sead::LinkList* list, void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::LinkList* oldList = mCurrentList;
    mActiveList = list;
    mOwner = owner;
    if (oldList != NULL) {
        mCurrentList = NULL;
        detach();
        oldList->mCount = u32(oldList->mCount) - 1;
    }
    mCurrentList = list;
    list->insertBefore(this);
    list->mCount = u32(list->mCount) + 1;
    gabi::call_ptr<void>(mMethods->mAttach, this, context);
}
/* 027AC1F0 */
void agl::RenderEntry::notifyQueued() {
    return gabi::tail_ptr<void>(mMethods->mQueued, this);
}
/* 027AC200 */
void agl::RenderEntry::queue() {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::LinkList* oldList = mCurrentList;
    sead::LinkList* target = mActiveList;
    if (oldList != NULL) {
        mCurrentList = NULL;
        detach();
        oldList->mCount = u32(oldList->mCount) - 1;
    }
    mCurrentList = target;
    target->insertAfter(this);
    target->mCount = u32(target->mCount) + 1;
    mQueued.mValue = 0;
    notifyQueued();
}
/* 027AC294 */
void agl::RenderEntry::activate() {
    gabi::AutoFrame frame{gabi::current_function()};
    queue();
    gabi::call_ptr<void>(mMethods->mActivate, this);
}
/* 027AC2D4 */
void agl::RenderEntry::returnToIdle() {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::LinkList* oldList = mCurrentList;
    sead::LinkList* target = &mOwner->mIdleEntries;
    if (oldList != NULL) {
        mCurrentList = NULL;
        detach();
        oldList->mCount = u32(oldList->mCount) - 1;
    }
    mCurrentList = target;
    target->insertBefore(this);
    target->mCount = u32(target->mCount) + 1;
    mQueued.mValue = 1;
    mIdle.mValue = 1;
}
static u32 sEntryVtable;
static u32 sNameVtable;
static u32 sDerivedEntryVtable;
void releaseAglEntryStorage(void*);
/* 027AC364 */
static void destroyRenderEntry(agl::RenderEntry* entry, u32 flags) {
    if (entry != NULL) {
        entry->mVtable = gabi::ea(&sEntryVtable);
        entry->mNameObject.mVtable = gabi::ea(&sNameVtable);
        if (flags & 1) return gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseAglEntryStorage), entry);
    }
}
/* 027ADE00 */
static void destroyDerivedRenderEntry(agl::RenderEntry* entry, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry != NULL) {
        entry->mVtable = gabi::ea(&sDerivedEntryVtable);
        entry->mNameObject.mVtable = gabi::ea(&sNameVtable);
        destroyRenderEntry(entry, 0);
        if (flags & 1) releaseAglEntryStorage(entry);
    }
}
/* WWHD glue
 data sEntryVtable 10168BA4
 data sNameVtable 1016860C
 data sDerivedEntryVtable 10168C54
 ext 0273AF40 void releaseAglEntryStorage(void*) callback
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B2D0 sead::Link* sead::Link::insertAfter(sead::Link*)
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
*/
