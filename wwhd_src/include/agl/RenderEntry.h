// WWHD render-entry layout; private views await their owners' canonical headers.
#pragma once
#include "agl/ParameterHooks.h"
#include "sead/Foundation.h"
namespace agl {
struct RenderEntry;
struct RenderEntryMethods {
    u8 _00[0x14];
    be<u32> mAttach;
    u8 _18[4];
    be<u32> mActivate;
    u8 _20[0x14];
    be<u32> mQueued;
};
struct RenderQueueOwner_l {
    u8 _00[0x1CC];
    sead::LinkList mIdleEntries;
};
struct EntryNameObject_l {
    u8 mFields[0x60];
    be<u32> mVtable;
};
struct RenderEntry : sead::Link {
    gptr<RenderEntry> mSelf;
    gptr<sead::LinkList> mCurrentList;
    be<u32> mVtable;
    EntryNameObject_l mNameObject;
    u8 mParameterContainer[0x5C];
    gptr<RenderQueueOwner_l> mOwner;
    gptr<sead::LinkList> mActiveList;
    be<u32> mKind;
    be<u32> mTag;
    ByteParameter mIdle;
    ByteParameter mQueued;
    WordParameter mIndex;
    gptr<RenderEntryMethods> mMethods;
    void attach(RenderQueueOwner_l*, sead::LinkList*, void*);
    void notifyQueued();
    void queue();
    void activate();
    void returnToIdle();
};
}
WWHD_SIZE(agl::RenderEntryMethods,0x38);
WWHD_OFFSET(agl::RenderQueueOwner_l,mIdleEntries,0x1CC);
WWHD_OFFSET(agl::RenderEntry,mSelf,8);
WWHD_OFFSET(agl::RenderEntry,mCurrentList,0xC);
WWHD_OFFSET(agl::RenderEntry,mNameObject,0x14);
WWHD_OFFSET(agl::RenderEntry,mParameterContainer,0x78);
WWHD_OFFSET(agl::RenderEntry,mOwner,0xD4);
WWHD_OFFSET(agl::RenderEntry,mActiveList,0xD8);
WWHD_OFFSET(agl::RenderEntry,mIdle,0xE4);
WWHD_OFFSET(agl::RenderEntry,mQueued,0xF4);
WWHD_OFFSET(agl::RenderEntry,mIndex,0x104);
WWHD_OFFSET(agl::RenderEntry,mMethods,0x114);
WWHD_SIZE(agl::RenderEntry,0x118);

WWHD_SIZE(agl::EntryNameObject_l,0x64);
