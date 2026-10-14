/* HD opening picture story; uses the merged parts framework. */
#pragma once
#include "d/hd_parts.h"
/* The library pane vtable extends the framework view with name lookup. */
struct hdOpening_paneVtable_l : hdPartsPaneVtbl_c {
    u8 _18[0x5C-0x18]; be<u32> mFindPane;
};
inline void openingSetString(hdUiSafeString& string, const char* text) {
    string.mText.v = gabi::guest_addr(text);
}
struct hdOpening_textPane_l { u8 _00[0x69C]; be<u8> mTextChanged; };
struct hdOpening_textBox_l {
    u8 _00[0x3C]; union { be<f32> mInitialScale; be<u32> mInitialScaleBits; };
    u8 _40[0x23C-0x40];
    union {
        hdOpening_textPane_l mMessagePane;
        struct { u8 _23C[0x8C4-0x23C]; union { be<f32> mSavedScale; be<u32> mSavedScaleBits; }; };
    };
};
struct hdOpening_heapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
struct hdOpening_screen_c : hdParts_c {
    be<s32> mPictureMessageIndex;
    gptr<hdOpening_textPane_l> mpMessagePane;
    gptr<void> mpAllPane, mpPicturePane;
    gptr<hdOpening_textBox_l> mpTextBox; gptr<void> mpBackgroundPane; u8 _68[0x20];
    be<u8> mFinished, mSkipRequested; u8 _8A[2];
    void findRootPanes();
    s32 setup(u32, u32, u32);
    s32 loadLayout(hdUiSafeString*, void*, u32, u32, u32);
    void finish();
    void update();
    void showSkip();
    void hideSkip();
    void findMessagePane();
    void findPicturePanes();
    void startPictureMessage();
    void enterPictureStory();
    s32 advancePictureMessage();
    void picMsg00Move();
    void picMsg01Enter();
    void picMsg01Move();
    void picMsg02Enter();
    void picMsg02Move();
    void picMsg03Enter();
    void picMsg03Move();
    void picMsg04Enter();
    void picMsg04Move();
    void picMsg05Enter();
    void picMsg05Move();
    void picMsg06Enter();
    void picMsg06Move();
    void picMsg07Enter();
    void picMsg07Move();
    void picMsg08Enter();
    void picMsg08Move();
    void picMsg09Enter();
    void picMsg09Move();
    void picMsg10Enter();
    void picMsg10Move();
    void picMsg11Enter();
    void picMsg11Move();
    void picMsg12Enter();
    void picMsg12Move();
    void picMsg13Enter();
    void picMsg13Move();
    void picMsg14Enter();
    void picMsg14Move();
    void picMsg15Enter();
    void picMsg15Move();
    void picMsg16Enter();
    void picMsg16Move();
    void picMsg17Enter();
    void picMsg17Move();
    void picMsg18Enter();
    void picMsg18Move();
    void picMsg19Enter();
    void picMsg19Move();
    void picMsg20Enter();
    void picMsg20Move();
    void picMsg21Enter();
    void picMsg21Move();
    void outAllEnter();
    void outAllMove();
};
WWHD_OFFSET(hdOpening_screen_c, mVtable, 0x4);
WWHD_OFFSET(hdOpening_screen_c, mpLayout, 0x44);
WWHD_OFFSET(hdOpening_screen_c, mPictureMessageIndex, 0x50);
WWHD_OFFSET(hdOpening_screen_c, mpMessagePane, 0x54);
WWHD_OFFSET(hdOpening_screen_c, mpAllPane, 0x58);
WWHD_OFFSET(hdOpening_screen_c, mpPicturePane, 0x5C);
WWHD_OFFSET(hdOpening_screen_c, mpTextBox, 0x60);
WWHD_OFFSET(hdOpening_screen_c, mpBackgroundPane, 0x64);
WWHD_OFFSET(hdOpening_screen_c, mFinished, 0x88);
WWHD_OFFSET(hdOpening_screen_c, mSkipRequested, 0x89);
WWHD_SIZE(hdOpening_screen_c, 0x8C);
WWHD_OFFSET(hdOpening_textPane_l, mTextChanged, 0x69C);
WWHD_OFFSET(hdOpening_paneVtable_l, mFindPane, 0x5C);
WWHD_OFFSET(hdOpening_textBox_l, mInitialScale, 0x3C);
WWHD_OFFSET(hdOpening_textBox_l, mMessagePane, 0x23C);
WWHD_OFFSET(hdOpening_textBox_l, mSavedScale, 0x8C4);
WWHD_OFFSET(hdOpening_heapManager_l, mpHeap, 0x10);
/* Unit-local address bindings. */
void openingRegisterRootPanes(void*);
void openingDestroyStringArray(void*, u32, u32, u32, u32, u32);
hdOpening_screen_c* openingAllocateScreen(u32);
void openingConstructBase(hdOpening_screen_c*);
void openingDestroyBase(hdOpening_screen_c*, u32);
void openingDelete(void*);
void openingPlayAnimation(void*, u32, u32, f32);
void openingSetMessage(hdOpening_textPane_l*, u32, u32);
void openingBindText(void*, hdOpening_textPane_l*, u32);
s32 openingAnimationFinished(void*, u32);
void openingChangeState(void*, void*);

struct hdOpening_stateSlot_l { gptr<hdPartsStateId_c> mpState; };
WWHD_SIZE(hdOpening_stateSlot_l, 4);
WWHD_OFFSET(hdOpening_textBox_l, mInitialScaleBits, 0x3C);
WWHD_OFFSET(hdOpening_textBox_l, mSavedScaleBits, 0x8C4);
