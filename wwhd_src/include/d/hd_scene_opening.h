#pragma once
#include "bindings.h"
#include "d/hd_screen_opening.h"
// HD-only opening scene; the screen remains an external UI object.
using hdOpeningScreen_l = hdOpening_screen_c;
struct hdOpeningDelegate_l {
    be<u32> mVtable;
    gptr<void> mpObject;
    be<u16> mThisAdjustment;
    be<u16> mVirtualIndex;
    be<u32> mFunction;
};
struct hdOpeningSafeString_l { gptr<const char> mpText; be<u32> mVtable; };
struct hdOpeningListener_l {
    u8 mDisposer[0xC];
    be<u32> mVtable;
    hdOpeningSafeString_l mName;
    be<u32> mEnabled;
    gptr<void> mpUser;
    hdOpeningDelegate_l mDelegate;
    u8 mStorage[0x10];
};
WWHD_SIZE(hdOpeningListener_l, 0x40);
struct hdOpeningDrawInfo_l { u8 mBase[0xC]; be<u32> mSlot; };
struct hdOpeningScene_c {
    be<s32> mStartDelay;
    u8 mDisposer[0x10];
    hdOpeningListener_l mTvListener;
    hdOpeningListener_l mDrcListener;
    gptr<hdOpeningScreen_l> mpScreen;
    u32 receiveTouch();
    u32 receiveKey();
    u32 receiveController();
    u32 receiveCursor();
    u32 receiveFocus();
    u32 receiveUnfocus();
    u32 receiveRelease();
    u32 receiveCancel();
    void createScreen();
    void initialize();
    void drawTv(hdOpeningDrawInfo_l*);
    void drawDrc(hdOpeningDrawInfo_l*);
    void unloadLayout();
    void finish();
    void updateScreen();
    void update();
    void destroyScreen();
};
WWHD_OFFSET(hdOpeningScreen_l, mVtable, 4);
WWHD_OFFSET(hdOpeningScene_c, mpScreen, 0x94);
WWHD_SIZE(hdOpeningScene_c, 0x98);

struct hdOpeningGameInfo_l {
    u8 mStorage[0x1684];
    be<u32> mSceneMode;
};
WWHD_OFFSET(hdOpeningGameInfo_l, mSceneMode, 0x1684);

WWHD_SIZE(hdOpeningSafeString_l, 8);
void openingLayoutRelease(void*, hdOpeningSafeString_l*);
void* openingFrameworkGet();
void openingFrameworkSetEnabled(void*, u32);

struct hdOpeningSaveSlot_l {
    u8 mBase[0x48];
    gptr<void> mpDefaultMovie;
    gptr<void> mpDefaultResource;
    be<u32> mFlags;
    u8 mMoviePadding[0x30];
    u8 mCustomMovie[0xE0];
    gptr<void> mpCustomResource;
    u8 mRenderPadding[0x3E0];
    u8 mRenderContext[4];
};
struct hdOpeningSaveArray_l { u8 mBase[0x1020]; be<u32> mSlotCount; gptr<gptr<hdOpeningSaveSlot_l>> mpSlots; };
WWHD_OFFSET(hdOpeningSaveSlot_l, mCustomMovie, 0x84);
WWHD_OFFSET(hdOpeningSaveSlot_l, mpCustomResource, 0x164);
WWHD_OFFSET(hdOpeningSaveArray_l, mpSlots, 0x1024);
hdOpeningScreen_l* openingScreenConstruct(hdOpeningScreen_l*);

void openingSetViewport(u32, u32, u32, u32, u32, u32, u32);
void openingRegisterListener(hdOpeningSaveSlot_l*, u32, void*);

hdOpeningScene_c* openingSceneConstruct(hdOpeningScene_c*);
void openingListenerConstruct(void*);
void openingListenerDestroy(void*, u32);
void openingDisposerConstruct(void*, void*, u32);
void openingDisposerDestroy(void*, u32);

struct hdOpeningDisposer_l { u8 mBase[0xC]; be<u32> mVtable; };
WWHD_SIZE(hdOpeningDisposer_l, 0x10);

void* openingHeapAllocate(u32, void*, u32);
struct hdOpeningStateId_l { be<u32> mId; be<u32> mName; gptr<be<u32>> mpVtable; };
struct hdOpeningCallbackState_l {
    gptr<be<u32>> mpVtable;
    gptr<void> mpOwner;
    gptr<hdOpeningStateId_l> mpStateId;
    hdOpeningStateId_l* getStateId();
    void enter();
    void execute();
    void leave();
};
struct hdOpeningStateFactory_l {
    gptr<be<u32>> mpVtable;
    hdOpeningCallbackState_l mState;
    hdOpeningCallbackState_l* createState(hdOpeningStateId_l*);
    void clearPrevious(gptr<hdOpeningCallbackState_l>*);
};
WWHD_OFFSET(hdOpeningStateId_l, mId, 0);
WWHD_OFFSET(hdOpeningStateId_l, mpVtable, 8);
WWHD_SIZE(hdOpeningStateId_l, 12);
WWHD_OFFSET(hdOpeningCallbackState_l, mpVtable, 0);
WWHD_OFFSET(hdOpeningCallbackState_l, mpOwner, 4);
WWHD_OFFSET(hdOpeningCallbackState_l, mpStateId, 8);
WWHD_SIZE(hdOpeningCallbackState_l, 12);
WWHD_OFFSET(hdOpeningStateFactory_l, mpVtable, 0);
WWHD_OFFSET(hdOpeningStateFactory_l, mState, 4);
WWHD_SIZE(hdOpeningStateFactory_l, 16);
void openingSaveSceneDestroy(void*, u32);

WWHD_OFFSET(hdOpeningScene_c, mStartDelay, 0x0);
WWHD_OFFSET(hdOpeningScene_c, mDisposer, 0x4);
WWHD_OFFSET(hdOpeningScene_c, mTvListener, 0x14);
WWHD_OFFSET(hdOpeningScene_c, mDrcListener, 0x54);
WWHD_OFFSET(hdOpeningListener_l, mVtable, 0xC);
WWHD_OFFSET(hdOpeningListener_l, mName, 0x10);
WWHD_OFFSET(hdOpeningListener_l, mEnabled, 0x18);
WWHD_OFFSET(hdOpeningListener_l, mpUser, 0x1C);
WWHD_OFFSET(hdOpeningListener_l, mDelegate, 0x20);
WWHD_OFFSET(hdOpeningDelegate_l, mVtable, 0x0);
WWHD_OFFSET(hdOpeningDelegate_l, mpObject, 0x4);
WWHD_OFFSET(hdOpeningDelegate_l, mThisAdjustment, 0x8);
WWHD_OFFSET(hdOpeningDelegate_l, mVirtualIndex, 0xA);
WWHD_OFFSET(hdOpeningDelegate_l, mFunction, 0xC);
WWHD_OFFSET(hdOpeningSaveSlot_l, mpDefaultMovie, 0x48);
WWHD_OFFSET(hdOpeningSaveSlot_l, mpDefaultResource, 0x4C);
WWHD_OFFSET(hdOpeningSaveSlot_l, mFlags, 0x50);
WWHD_OFFSET(hdOpeningSaveArray_l, mSlotCount, 0x1020);
WWHD_OFFSET(hdOpeningDrawInfo_l, mSlot, 0xC);

WWHD_OFFSET(hdOpeningSaveSlot_l, mRenderContext, 0x548);

WWHD_OFFSET(hdOpeningSafeString_l, mpText, 0x0);
WWHD_OFFSET(hdOpeningSafeString_l, mVtable, 0x4);
WWHD_OFFSET(hdOpeningDisposer_l, mVtable, 0xC);
