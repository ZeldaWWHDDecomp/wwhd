/** HD: companion handlers for the in-game UI scene.
 * The scene owner's canonical declarations are shared through hd_scene_ingame_ui.h. */
#pragma once
#include "d/hd_scene_ingame_ui.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_parts.h"

struct hdIngameDemo_l;
struct hdIngamePlay_l {
    u8 mReserved0[0x5134];
    be<u8> mStageName[8];
    u8 mReserved513C[0x10];
    be<s8> mRoom;
    u8 mReserved514D[0x145];
    be<u8> mEventRunning;
    u8 mReserved5293[0x899];
    gptr<hdIngameDemo_l> mpDemo;
    u8 mReserved5B30[0x4];
    gptr<void> mpPlayer;
    u8 mReserved5B38[0x14];
    be<u32> mDemoState;
    u8 mReserved5B50[0x5C];
    be<u16> mMenuAllowed;
    u8 mReserved5BAE[0x2];
    be<u8> mDemoRequested;
    u8 mReserved5BB1[2];
    be<u8> mCurrentItem;
    u8 mReserved5BB4[7];
    be<u8> mAssignedItems[3];
    u8 mReserved5BBE[0x12];
    be<u8> mWindControlState;
    u8 mReserved5BD1[0xA];
    be<u8> mFishManDemo;
    be<u8> mPasswordState;
    u8 mReserved5BDD[0x7];
    be<u8> mTriforceDemo;
    u8 mReserved5BE5[0x3];
    be<u8> mPictographListState;
    u8 mReserved5BE9[0xEF];
    be<u32> mUiFlags;
    be<u32> mUiRequests;
};
WWHD_OFFSET(hdIngamePlay_l, mStageName, 0x5134);
WWHD_OFFSET(hdIngamePlay_l, mRoom, 0x514C);
WWHD_OFFSET(hdIngamePlay_l, mEventRunning, 0x5292);
WWHD_OFFSET(hdIngamePlay_l, mpDemo, 0x5B2C);
WWHD_OFFSET(hdIngamePlay_l, mpPlayer, 0x5B34);
WWHD_OFFSET(hdIngamePlay_l, mDemoState, 0x5B4C);
WWHD_OFFSET(hdIngamePlay_l, mMenuAllowed, 0x5BAC);
WWHD_OFFSET(hdIngamePlay_l, mDemoRequested, 0x5BB0);
WWHD_OFFSET(hdIngamePlay_l, mAssignedItems, 0x5BBB);
WWHD_OFFSET(hdIngamePlay_l, mFishManDemo, 0x5BDB);
WWHD_OFFSET(hdIngamePlay_l, mPasswordState, 0x5BDC);
WWHD_OFFSET(hdIngamePlay_l, mTriforceDemo, 0x5BE4);
WWHD_OFFSET(hdIngamePlay_l, mPictographListState, 0x5BE8);
WWHD_OFFSET(hdIngamePlay_l, mUiFlags, 0x5CD8);
WWHD_OFFSET(hdIngamePlay_l, mUiRequests, 0x5CDC);
WWHD_SIZE(hdIngamePlay_l, 0x5CE0);
struct hdIngameDemoVtable_l {
    u8 mPrefix[0x2C];
    gfn mGetSong;
};
struct hdIngameDemo_l {
    u8 mReserved0[0xB4];
    gptr<hdIngameDemoVtable_l> mpVtable;
    u8 mReservedB8[0x300];
    be<u32> mPictureFlags;
    u8 mReserved3BC[4];
    be<u32> mFlags;
};
WWHD_OFFSET(hdIngameDemo_l, mFlags, 0x3C0);
WWHD_SIZE(hdIngameDemo_l, 0x3C4);
struct hdIngameController_l {
    u8 mReserved0[0x18];
    be<u32> mHold;
    u8 mReserved1C[0x1B4];
    be<u32> mMode;
    u8 mReserved1D4[0x98];
    be<u8> mEnabled;
    u8 mReserved26D[0x3];
};
WWHD_OFFSET(hdIngameController_l, mHold, 0x18);
WWHD_OFFSET(hdIngameController_l, mMode, 0x1D0);
WWHD_OFFSET(hdIngameController_l, mEnabled, 0x26C);
WWHD_SIZE(hdIngameController_l, 0x270);

struct hdIngameEvent_l {
    be<u32> mKind;
    u8 mReserved[4];
    be<u8> mPayload;
    u8 mTail[0x28 - 9];
};
WWHD_SIZE(hdIngameEvent_l, 0x28);
struct hdIngameCollect_l {
    u8 mOpaque[1];
};
struct hdIngamePictures_l {
    u8 mOpaque[1];
};
struct hdIngameSave_l {
    u8 mPrefix[0x20];
    be<u8> mInventory[0xB4];
    hdIngameCollect_l mCollect;
    u8 mReservedD5[0x12C0 - 0xD5];
    hdIngamePictures_l mPictures;
};
WWHD_OFFSET(hdIngameSave_l, mInventory, 0x20);
WWHD_OFFSET(hdIngameSave_l, mCollect, 0xD4);
WWHD_OFFSET(hdIngameSave_l, mPictures, 0x12C0);
struct hdIngameComment_l {
    u8 mOpaque[1];
};
struct hdIngameSocialEntry_l {
    u8 mPrefix[0x3C4];
    hdIngameComment_l mComment;
    u8 mTail[0x27];
};
WWHD_SIZE(hdIngameSocialEntry_l, 0x3EC);
struct hdIngameSocialData_l {
    u8 mPrefix[0x14];
    hdIngameSocialEntry_l mComments[10];
    u8 mReserved[0x4FD5 - 0x14 - 10 * 0x3EC];
    be<u8> mProfileJumpPending;
};
WWHD_OFFSET(hdIngameSocialData_l, mProfileJumpPending, 0x4FD5);
struct hdIngameMiiverse_l {
    u8 mPrefix[0x38];
    be<u32> mState;
};
struct hdIngamePictureSave_l {
    u8 mPrefix[0x3C030C];
    be<u8> mCount;
};
struct hdIngamePictureCapture_l {
    u8 mPrefix[0x14];
    be<u32> mState;
};


WWHD_OFFSET(hdIngameEvent_l, mPayload, 8);
WWHD_OFFSET(hdIngameSocialData_l, mComments, 0x14);
WWHD_OFFSET(hdIngameSocialEntry_l, mComment, 0x3C4);
WWHD_OFFSET(hdIngameDemo_l, mpVtable, 0xB4);
WWHD_OFFSET(hdIngameDemo_l, mPictureFlags, 0x3B8);
WWHD_OFFSET(hdIngamePlay_l, mCurrentItem, 0x5BB3);
WWHD_OFFSET(hdIngamePlay_l, mWindControlState, 0x5BD0);
