/* HD dMenu_PlaceName_c layout and native UI state records; private verified views.
 * GameCube d_menu_window only retains the placename event helper. The HD screen uses BFRES UI,
 * two text boxes, StateProcessor callbacks and a 0x5C object. */
#pragma once
#include "wwhd.h"
#include "gabi.h"
namespace MenuHD {

struct SafeString {

    /* 0x000 */ be<u32> mpText;
    /* 0x004 */ be<u32> mVtable;
};
WWHD_SIZE(SafeString, 0x8);
WWHD_OFFSET(SafeString, mpText, 0x0);
WWHD_OFFSET(SafeString, mVtable, 0x4);

struct SafeStringVtable {

    u8 mPadding0[20];
    /* 0x014 */ be<u32> mAssureTermination;
};
WWHD_SIZE(SafeStringVtable, 0x18);
WWHD_OFFSET(SafeStringVtable, mAssureTermination, 0x14);

struct StringPair {

    /* 0x000 */ SafeString mEntries[2];
};
WWHD_SIZE(StringPair, 0x10);
WWHD_OFFSET(StringPair, mEntries, 0x0);

struct LocationNames {

    /* 0x000 */ SafeString mEntries[19];
};
WWHD_SIZE(LocationNames, 0x98);
WWHD_OFFSET(LocationNames, mEntries, 0x0);

struct Callback {

    /* 0x000 */ be<u16> mThisAdjust;
    /* 0x002 */ be<u16> mVirtualSlot;
    /* 0x004 */ be<u32> mFlags;
};
WWHD_SIZE(Callback, 0x8);
WWHD_OFFSET(Callback, mThisAdjust, 0x0);
WWHD_OFFSET(Callback, mVirtualSlot, 0x2);
WWHD_OFFSET(Callback, mFlags, 0x4);

struct StateID {

    /* 0x000 */ be<u32> mId;
    /* 0x004 */ be<u32> mpName;
    /* 0x008 */ be<u32> mVtable;
    /* 0x00C */ Callback mInitialize;
    /* 0x014 */ Callback mExecute;
    /* 0x01C */ Callback mFinalize;
    /* 0x024 */ Callback mReserved;
    /* 0x02C */ be<u32> mpNullState;
};
WWHD_SIZE(StateID, 0x30);
WWHD_OFFSET(StateID, mId, 0x0);
WWHD_OFFSET(StateID, mpName, 0x4);
WWHD_OFFSET(StateID, mVtable, 0x8);
WWHD_OFFSET(StateID, mInitialize, 0xC);
WWHD_OFFSET(StateID, mExecute, 0x14);
WWHD_OFFSET(StateID, mFinalize, 0x1C);
WWHD_OFFSET(StateID, mReserved, 0x24);
WWHD_OFFSET(StateID, mpNullState, 0x2C);

struct StateVtable {

    u8 mPadding0[20];
    /* 0x014 */ be<u32> mFindState;
    u8 mPadding24[4];
    /* 0x01C */ be<u32> mInitialize;
};
WWHD_SIZE(StateVtable, 0x20);
WWHD_OFFSET(StateVtable, mFindState, 0x14);
WWHD_OFFSET(StateVtable, mInitialize, 0x1C);

struct PlaceVtable {

    u8 mPadding0[124];
    /* 0x07C */ be<u32> mCreateLayout;
    u8 mPadding128[4];
    /* 0x084 */ be<u32> mClose;
    u8 mPadding136[4];
    /* 0x08C */ be<u32> mInitialize;
    u8 mPadding144[12];
    /* 0x09C */ be<u32> mDraw;
    u8 mPadding160[12];
    /* 0x0AC */ be<u32> mCalc;
};
WWHD_SIZE(PlaceVtable, 0xB0);
WWHD_OFFSET(PlaceVtable, mCreateLayout, 0x7C);
WWHD_OFFSET(PlaceVtable, mClose, 0x84);
WWHD_OFFSET(PlaceVtable, mInitialize, 0x8C);
WWHD_OFFSET(PlaceVtable, mDraw, 0x9C);
WWHD_OFFSET(PlaceVtable, mCalc, 0xAC);

struct PaneVtable {

    u8 mPadding0[20];
    /* 0x014 */ be<u32> mFindState;
    u8 mPadding24[4];
    /* 0x01C */ be<u32> mInitialize;
    u8 mPadding32[60];
    /* 0x05C */ be<u32> mFindPane;
};
WWHD_SIZE(PaneVtable, 0x60);
WWHD_OFFSET(PaneVtable, mFindState, 0x14);
WWHD_OFFSET(PaneVtable, mInitialize, 0x1C);
WWHD_OFFSET(PaneVtable, mFindPane, 0x5C);

struct Pane {

    /* 0x000 */ be<u32> mVtable;
    u8 mPadding4[4];
    /* 0x008 */ be<u32> mPaneVtable;
};
WWHD_SIZE(Pane, 0xC);
WWHD_OFFSET(Pane, mVtable, 0x0);
WWHD_OFFSET(Pane, mPaneVtable, 0x8);

struct LayoutData {

    u8 mPadding0[12];
    /* 0x00C */ be<u32> mpRootPane;
    u8 mPadding16[12];
    /* 0x01C */ be<u32> mpMessageArchive;
};
WWHD_SIZE(LayoutData, 0x20);
WWHD_OFFSET(LayoutData, mpRootPane, 0xC);
WWHD_OFFSET(LayoutData, mpMessageArchive, 0x1C);

struct Layout {

    u8 mPadding0[4];
    /* 0x004 */ be<u32> mpData;
    u8 mPadding8[204];
    /* 0x0D4 */ be<u32> mpAnimation;
    u8 mPadding216[8];
    /* 0x0E0 */ be<u32> mVtable;
    u8 mPadding228[44];
};
WWHD_SIZE(Layout, 0x110);
WWHD_OFFSET(Layout, mpData, 0x4);
WWHD_OFFSET(Layout, mpAnimation, 0xD4);
WWHD_OFFSET(Layout, mVtable, 0xE0);

struct LayoutVtable {

    u8 mPadding0[20];
    /* 0x014 */ be<u32> mCreate;
};
WWHD_SIZE(LayoutVtable, 0x18);
WWHD_OFFSET(LayoutVtable, mCreate, 0x14);

struct AllocationManager {

    u8 mPadding0[16];
    /* 0x010 */ be<u32> mpHeap;
};
WWHD_SIZE(AllocationManager, 0x14);
WWHD_OFFSET(AllocationManager, mpHeap, 0x10);

struct Play {

    u8 mPadding0[21138];
    /* 0x5292 */ be<u8> mEventRunning;
    u8 mPadding21139[49];
    /* 0x52C4 */ be<u8> mEventManager;
    u8 mPadding21189[2332];
    /* 0x5BE1 */ be<u8> mStageNameIndex;
    /* 0x5BE2 */ be<u8> mStageNameStatus;
    u8 mPadding23523[1];
};
WWHD_SIZE(Play, 0x5BE4);
WWHD_OFFSET(Play, mEventRunning, 0x5292);
WWHD_OFFSET(Play, mEventManager, 0x52C4);
WWHD_OFFSET(Play, mStageNameIndex, 0x5BE1);
WWHD_OFFSET(Play, mStageNameStatus, 0x5BE2);

struct Save {

    u8 mPadding0[1604];
    /* 0x644 */ be<u8> mEvent;
};
WWHD_SIZE(Save, 0x645);
WWHD_OFFSET(Save, mEvent, 0x644);

struct DestructorRecord {

    /* 0x000 */ be<u32> mNext;
    /* 0x004 */ be<u32> mpObject;
    /* 0x008 */ be<u32> mpDestructor;
    /* 0x00C */ be<u32> mFlags;
};
WWHD_SIZE(DestructorRecord, 0x10);
WWHD_OFFSET(DestructorRecord, mNext, 0x0);
WWHD_OFFSET(DestructorRecord, mpObject, 0x4);
WWHD_OFFSET(DestructorRecord, mpDestructor, 0x8);
WWHD_OFFSET(DestructorRecord, mFlags, 0xC);

struct AnimationIndices {

    /* 0x000 */ be<u32> mValues[2];
};
WWHD_SIZE(AnimationIndices, 0x8);
WWHD_OFFSET(AnimationIndices, mValues, 0x0);

struct StateProcessorHolder {
    be<u32> mpProcessor;
};
WWHD_SIZE(StateProcessorHolder, 4);
WWHD_OFFSET(StateProcessorHolder, mpProcessor, 0);

struct dMenu_PlaceName_c {
    u32 executeWait();
    u32 initializeInAll();
    u32 executeInAll();
    u32 initializeOutAll();
    u32 executeOutAll();
    u32 finalizeOutAll();
    u32 create(u32, u32, u32);
    u32 calc();
    u32 update();
    u32 draw();
    u32 createLayout(u32, u32, u32, u32, u32);
    u8 mPadding0[4];
    /* 0x004 */ be<u32> mVtable;
    /* 0x008 */ be<u8> mStateStorage[16];
    /* 0x018 */ StateProcessorHolder mStateProcessor;
    u8 mPadding28[4];
    /* 0x020 */ be<u32> mpState;
    u8 mPadding36[8];
    /* 0x02C */ be<u32> mStateInterfaceVtable;
    /* 0x030 */ be<u32> mDrawInterfaceVtable;
    /* 0x034 */ be<u32> mpHeap;
    u8 mPadding56[12];
    /* 0x044 */ be<u32> mpLayout;
    u8 mPadding72[8];
    /* 0x050 */ be<u32> mpTextBox[2];
    /* 0x058 */ be<u8> mVisible;
    u8 mPadding89[3];
};
WWHD_SIZE(dMenu_PlaceName_c, 0x5C);
WWHD_OFFSET(dMenu_PlaceName_c, mVtable, 0x4);
WWHD_OFFSET(dMenu_PlaceName_c, mStateStorage, 0x8);
WWHD_OFFSET(dMenu_PlaceName_c, mStateProcessor, 0x18);
WWHD_OFFSET(dMenu_PlaceName_c, mpState, 0x20);
WWHD_OFFSET(dMenu_PlaceName_c, mStateInterfaceVtable, 0x2C);
WWHD_OFFSET(dMenu_PlaceName_c, mDrawInterfaceVtable, 0x30);
WWHD_OFFSET(dMenu_PlaceName_c, mpHeap, 0x34);
WWHD_OFFSET(dMenu_PlaceName_c, mpLayout, 0x44);
WWHD_OFFSET(dMenu_PlaceName_c, mpTextBox, 0x50);
WWHD_OFFSET(dMenu_PlaceName_c, mVisible, 0x58);

struct StringFrame {

    u8 mPadding0[8];
    /* 0x008 */ SafeString mArchiveName;
    u8 mPadding16[32];
};
WWHD_SIZE(StringFrame, 0x30);
WWHD_OFFSET(StringFrame, mArchiveName, 0x8);

enum StageNameStatus : u8 { StageNameIdle = 0, StageNameClose = 1, StageNameOpen = 2 };
constexpr u32 kLocationCount = 19;
constexpr u32 kStateCallbackFlags = 4;
constexpr u32 kPlaceVtable = 0x10101908;
constexpr u32 kStateInterfaceVtable = 0x10101A18;
constexpr u32 kDrawInterfaceVtable = 0x10101A28;
constexpr u32 kSafeStringVtable = 0x1010163C;
constexpr u32 kStateVtable = 0x1010169C;
constexpr u32 kNullState = 0x101FF32C;
} // namespace MenuHD
u32 menuHD_getPlay();
u32 menuHD_baseDestruct(void*, u32);
u32 menuHD_operatorDelete(void*);
u32 menuHD_changeState(void*, void*);
u32 menuHD_setText(void*, void*, void*);
u32 menuHD_setAnimationEnabled(void*, u32, u32, f64);
u32 menuHD_isAnimationFinished(void*, u32);
u32 menuHD_operatorNew(u32);
u32 menuHD_baseConstruct(void*);
u32 menuHD_setModuleName(void*, void*, u32);
u32 menuHD_setAnimationFrame(void*, u32, u32, f64);
u32 menuHD_baseCalc(void*);
u32 menuHD_startCheck(void*, const char*);
u32 menuHD_onEventBit(void*, u32);
u32 menuHD_placenameMoveEntry();
u32 menuHD_updateState(void*);
u32 menuHD_heapNew(u32, void*, u32);
u32 menuHD_layoutConstruct(void*);
u32 menuHD_bindAnimation(void*, u32, void*, void*);
u32 menuHD_registerGlobal(void*);
u32 menuHD_constructHeaderByte(void*);
u32 menuHD_constructArchiveByte(void*);
