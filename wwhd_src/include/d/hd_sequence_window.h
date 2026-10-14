/* HD: SequenceWindow_00 system and save/load prompts. No GameCube counterpart.
 * Layout names and state names come from the binary; descriptive member names are ours.
 * */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_basic_button.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_input_ctrl.h"
#include "d/hd_parts_in_out_loop.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_msg_text.h"
#include "d/d_operate_wind.h"
#include "d/hd_parts_text_pane.h"
#include "hd/hd_ui_02001084.h"
#include "hd/hd_ui_020015C8.h"

struct hdSequenceTextRecord_c;
struct hdSequencePaneLookupTable_l;
struct HdMsgText;
/* a layout pane as this window uses it: the vtable's find-by-name (+0x5C), the visibility flags (+0x44) */
struct hdSequencePane_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<hdSequencePaneLookupTable_l> mVtable;
    /* 0x0C */ u8 _0C[0x44 - 0xC];
    /* 0x44 */ be<u8> mFlags;
};
WWHD_OFFSET(hdSequencePane_c, mFlags, 0x44);
struct hdSysWinDesc_l;
/* the GamePad window "SequenceWindow_00" (one class, vtable 0x101046C8): the save / load / continue / warp /
 * Miiverse / second-screen / hard-mode dialogs. Its code is split over hd_screen_sequence_window*.cpp
 * (MSG-sequence_window: states, buttons, input) and hd_screen_system_window.cpp (HUD-shortcut: init, open,
 * the dialog messages and scripts), which used to model it as two classes (hdScreenSystemWindow_c is now an
 * alias, d/hd_screen_system_window.h). */
struct hdSequenceWindow_c : hdParts_c {
    /* 0x50 */ gptr<hdSequencePane_c> mpPanes[5];
    /* 0x64 */ gptr<hdSequencePane_c> mpTextPanes[3];
    /* 0x70 */ gptr<void> mpPartsPanes[5];    /* "L_BtnSeqenceL/R/C_00", "L_LoadingIcon_00", "L_KeyIcon_00" (findPartsPanes) */
    /* 0x84 */ be<u8> mPaneVisible[5];
    /* 0x89 */ u8 mPadding[3];
    /* 0x8C */ gptr<hdSequenceTextRecord_c> mpText;  /* the dialog's current text record (script entry) */
    /* 0x90 */ gptr<HdMsgText> mpMessage;
    /* 0x94 */ gptr<hdPartsStateId_c> mpNextState;
    /* 0x98 */ be<u32> mMode;
    /* 0x9C */ be<u32> mButtonCount;
    /* 0xA0 */ be<s32> mSelectedButton;
    /* 0xA4 */ be<u32> mButtonMode;
    /* 0xA8 */ be<u32> mTextKind;
    /* 0xAC */ be<u32> mWatchedStatus;  /* the input manager's +0x10C, copied every frame (executeState) */
    /* 0xB0 */ be<u32> mWatchedFirst;   /* ... and its first word */
    /* 0xB4 */ be<u32> mTimer;
    /* 0xB8 */ be<u32> mIoPhase;
    /* 0xBC */ be<u32> mWaitFrames;
    /* 0xC0 */ be<u32> mMessageIndex;       /* openMessage / openSecondScreen; the second screen's label is +8 */
    /* 0xC4 */ be<u32> mMessageLayoutMode;  /* the message's layout mode, kept across text restarts */
    /* 0xC8 */ be<u8> mSavedInputMode;      /* hard-mode dialog: the UI flags saved at open ... */
    /* 0xC9 */ be<u8> mButtonsEnabled;
    /* 0xCA */ be<u8> mMenuFlagSet;         /* open set the menu flag and the cursor; the out state clears them */
    /* 0xCB */ be<u8> mKeepPlaySettings;    /* open's keepPlay: the play buttons are not backed up / restored */
    /* 0xCC */ be<u8> mRestoreInputMode;    /* ... restored on close (set by the hard-mode dialog) */
    /* 0xCD */ be<u8> mSavedInputFlag;      /* ... */
    /* 0xCE */ be<u8> mCancelRequested;
    /* 0xCF */ be<u8> mMessageVisible;      /* message / Miiverse-info modes: the message text shows, a touch advances it */
    /* 0xD0 */ be<u8> mButtonSelectionEnabled;
    /* 0xD1 */ be<u8> mAdvanceRequested;
    /* 0xD2 */ be<u8> mCloseRequested;      /* requestClose; the children stop executing */
    /* 0xD3 */ be<u8> mActive;              /* modes 5 and 8, probably */
    /* 0xD4 */ be<u8> mLoadRequested;
    /* 0xD5 */ be<u8> mSaveRequested;
    /* 0xD6 */ be<u8> mSecondScreen;        /* openSecondScreen; the out state keeps the control mode */
    /* 0xD7 */ be<u8> mWaitComplete;
    /* 0xD8 */ be<u8> mOutsideTouchArmed;
    /* 0xD9 */ be<u8> mWaiting;             /* the script's start/stop-waiting commands */
    /* 0xDA */ be<u8> mOpenComplete;        /* the in animation ended: executeState selects the default button once */
    /* 0xDB */ u8 mTailPadding;

    static hdSequenceWindow_c* construct(hdSequenceWindow_c*);
    static void destroy(hdSequenceWindow_c*, s32);
    void showNextIcon();
    void hideNextIcon();
    void releaseButtons();
    void waitEnter();
    void waitFlagEnter();
    void buttonWaitEnter();
    void saveButtonWaitEnter();
    void saveEnter();
    void loadEnter();
    void selectButton(u32);
    void updateButtons();
    void setButtonLabel(u32, u32);
    void setButtonSound(u32, u32);
    void selectInputButton();
    void selectModeState();
    void connectedWaitExecute();
    void endWaitExecute();
    void resetEndExecute();
    void inEnter();
    void inExecute();
    void outEnter();
    void outExecute();
    void restorePlaySettings();
    u32 pollInput();
    u32 isPointOutside(const cking::ui::input::Vec2*);
    void executeButtons();
    void buttonWaitExecute();
    void drcWaitExecute();
    void proWaitExecute();
    void handleInput(u32);
    u32 pollCancel();
    void findPanes();
    void findTextPanes();
    void advance();
    void resetInputMode();
    void requestAdvance();
    void loadExit();
    void saveExit();
    void requestClose();
    void closeSavePart();
    void closePartExecute();
    /* hd_screen_system_window.cpp */
    void findPartsPanes();
    void linkButtons();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    void open(s32 mode, u32 keepPlay);
    void openSecondScreen(u32 index);
    u32 getMessageId(u32 index);
    void openMessage(u32 index);
    void openMiiverseInfo();
    void close();
    u32 visit(hdSysWinDesc_l* desc);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    s32 cmdReset();
    s32 cmdWaitMessage();
    s32 cmdStartWaiting();
    s32 cmdStopWaiting();
};
WWHD_OFFSET(hdSequenceWindow_c, mpPanes, 0x50);
WWHD_OFFSET(hdSequenceWindow_c, mpTextPanes, 0x64);
WWHD_OFFSET(hdSequenceWindow_c, mpPartsPanes, 0x70);
WWHD_OFFSET(hdSequenceWindow_c, mPaneVisible, 0x84);
WWHD_OFFSET(hdSequenceWindow_c, mpText, 0x8C);
WWHD_OFFSET(hdSequenceWindow_c, mTimer, 0xB4);
WWHD_OFFSET(hdSequenceWindow_c, mWaitComplete, 0xD7);
WWHD_SIZE(hdSequenceWindow_c, 0xDC);

struct hdSequenceInputPointer_c { gptr<cking::ui::input::EventMgr> mpInput; };
struct hdSequenceClosedPointer_c { gptr<be<u32>> mpFlag; };
struct hdSequenceInputFlag_c { be<u8> mEnabled; };
WWHD_SIZE(hdSequenceInputPointer_c, 4);
WWHD_SIZE(hdSequenceClosedPointer_c, 4);
WWHD_SIZE(hdSequenceInputFlag_c, 1);

WWHD_OFFSET(hdSequenceWindow_c, mWaitFrames, 0xBC);
WWHD_OFFSET(hdSequenceWindow_c, mButtonsEnabled, 0xC9);
WWHD_OFFSET(hdSequenceWindow_c, mMessageVisible, 0xCF);
WWHD_OFFSET(hdSequenceWindow_c, mAdvanceRequested, 0xD1);
WWHD_OFFSET(hdSequenceWindow_c, mCloseRequested, 0xD2);

struct hdSequenceInterfaceTable_c { be<u32> mFirstEntry; };
WWHD_SIZE(hdSequenceInterfaceTable_c, 4);

struct hdSequenceTextRecord_c {
    be<u32> mAnimation;
    be<u32> mButtonMode;
    be<s32> mDefaultButton;
    be<u32> mLabel;
    be<u32> mButtonLabels[2];
    hdPartsPtmf_c mCanAdvance[2];
};
WWHD_SIZE(hdSequenceTextRecord_c, 0x28);
WWHD_OFFSET(hdSequenceTextRecord_c, mButtonLabels, 0x10);
struct hdSequenceLabelTable_c {
    hdUiSafeString mLabels[51];
    hdUiSafeString* label(u32 index) {
        return gabi::at<hdUiSafeString>(gabi::ea(this) + index * sizeof(hdUiSafeString));
    }
};
WWHD_SIZE(hdSequenceLabelTable_c, 51 * 8);
struct hdSequenceButtonIndexTable_c {
    be<u32> mIndices[10];
    u32 index(u32 slot) { return *gabi::at<be<u32>>(gabi::ea(this) + slot * 4); }
};
WWHD_SIZE(hdSequenceButtonIndexTable_c, 40);
WWHD_OFFSET(hdSequenceWindow_c, mpMessage, 0x90);

// Narrow fields omitted by merged base views; no framework implementation copied.
struct hdSequenceLayoutLabel_l {
    u8 mBase[0x1C];
    gptr<be<u8>> mpArchiveLabel;
};
WWHD_OFFSET(hdSequenceLayoutLabel_l, mpArchiveLabel, 0x1C);

struct hdSequenceModeWordTable_c {
    be<u32> mWords[4];
    u32 word(u32 index) { return *gabi::at<be<u32>>(gabi::ea(this) + index * sizeof(be<u32>)); }
};
struct hdSequenceButtonVisibilityTable_c {
    be<u8> mVisible[12];
    u8 visible(u32 index) { return *gabi::at<be<u8>>(gabi::ea(this) + index); }
};
WWHD_SIZE(hdSequenceModeWordTable_c, 16);
WWHD_SIZE(hdSequenceButtonVisibilityTable_c, 12);
enum hdSequenceButtonMode_e { SEQUENCE_BUTTON_NONE, SEQUENCE_BUTTON_SINGLE, SEQUENCE_BUTTON_CONFIRM, SEQUENCE_BUTTON_PAIR };
enum hdSequenceAnimation_e { SEQUENCE_ANIM_IN, SEQUENCE_ANIM_BUTTON_SELECT = 4, SEQUENCE_ANIM_OUT = 6 };
enum { SEQUENCE_BUTTON_LABEL_COUNT = 51, SEQUENCE_BUTTON_COUNT = 3, SEQUENCE_PANE_COUNT = 5 };
/* mpPanes[4] "N_AreaName_00" (shown for the message modes), mpTextPanes[2] "T_AreaName_00" */
enum { SEQUENCE_PANE_AREA_NAME = 4, SEQUENCE_TEXT_AREA_NAME = 2 };

struct hdSequenceButtonSoundTable_c { hdUiSafeString mNames[32]; };
WWHD_SIZE(hdSequenceButtonSoundTable_c, 32 * 8);

struct hdSequenceUiSystemPointer_c { gptr<hdScreenUiSystem> mpSystem; };
WWHD_SIZE(hdSequenceUiSystemPointer_c, 4);
// Concrete sequence-button allocation observed at its factory; no merged concrete type exists.
// Framework button fields and methods use the merged canonical types.
enum { SEQUENCE_BUTTON_ALLOCATION_SIZE = 0x94 };
struct hdSequenceButton_c;

struct hdSequenceEventQueue_l {
    hdWidget_queue_c mQueue;
    hdUiEvent mCurrentEvent;
};
WWHD_OFFSET(hdSequenceEventQueue_l, mCurrentEvent, 0x1C);
WWHD_SIZE(hdSequenceEventQueue_l, 0x44);
inline hdUiEvent* sequenceCurrentEvent(cking::ui::input::EventMgr* manager) {
    return &reinterpret_cast<hdSequenceEventQueue_l*>(manager->mEventQueue)->mCurrentEvent;
}

struct hdSequenceModeRecord_c {
    be<u32> mUnused;
    gptr<hdPartsStateId_c> mpNextState;
    be<u8> mActionButtonA, mActionButtonB, mControlMode;
    u8 mPadding;
};
WWHD_OFFSET(hdSequenceModeRecord_c, mpNextState, 4);
WWHD_OFFSET(hdSequenceModeRecord_c, mActionButtonA, 8);
WWHD_SIZE(hdSequenceModeRecord_c, 12);
struct hdSequenceModeTable_c {
    gptr<hdSequenceModeRecord_c> mpModes[19];
    hdSequenceModeRecord_c* mode(u32 index) { return *gabi::at<gptr<hdSequenceModeRecord_c>>(gabi::ea(this) + index * 4); }
};
WWHD_SIZE(hdSequenceModeTable_c, 19 * 4);
struct hdSequenceOptionsPointer_c { gptr<cking::ui::input::Options> mpOptions; };
WWHD_SIZE(hdSequenceOptionsPointer_c, 4);

struct hdSequencePaneNameTable_c { hdUiSafeString mNames[5]; };
struct hdSequenceTextPaneNameTable_c { hdUiSafeString mNames[3]; };
WWHD_SIZE(hdSequencePaneNameTable_c, 40);
WWHD_SIZE(hdSequenceTextPaneNameTable_c, 24);

struct hdSequenceInputManagerPointer_c { gptr<hdInputManager> mpManager; };
WWHD_SIZE(hdSequenceInputManagerPointer_c, 4);
enum hdSequenceInput_e { SEQUENCE_INPUT_NONE, SEQUENCE_INPUT_POINTER, SEQUENCE_INPUT_EVENT, SEQUENCE_INPUT_NEXT };

struct hdSequencePoint_c { cking::ui::input::Vec2 mPosition; };
WWHD_SIZE(hdSequencePoint_c, 8);

WWHD_OFFSET(hdSequenceTextRecord_c, mCanAdvance, 0x18);

struct hdSequencePaneLookupTable_l {
    u8 mPrefix[0x5C];
    gfn mFindPane;
};
WWHD_OFFSET(hdSequencePaneLookupTable_l, mFindPane, 0x5C);
WWHD_SIZE(hdSequencePaneLookupTable_l, 0x60);
struct hdSequenceGuard_c { be<u32> mInitialized; };
struct hdSequenceRegistration_c { u8 mRecord[12]; };
WWHD_SIZE(hdSequenceGuard_c, 4);
WWHD_SIZE(hdSequenceRegistration_c, 12);

WWHD_OFFSET(hdSequenceWindow_c, mpNextState, 0x94);
WWHD_OFFSET(hdSequenceWindow_c, mMode, 0x98);
WWHD_OFFSET(hdSequenceWindow_c, mButtonCount, 0x9C);
WWHD_OFFSET(hdSequenceWindow_c, mSelectedButton, 0xA0);
WWHD_OFFSET(hdSequenceWindow_c, mButtonMode, 0xA4);
WWHD_OFFSET(hdSequenceWindow_c, mTextKind, 0xA8);
WWHD_OFFSET(hdSequenceWindow_c, mWatchedStatus, 0xAC);
WWHD_OFFSET(hdSequenceWindow_c, mWatchedFirst, 0xB0);
WWHD_OFFSET(hdSequenceWindow_c, mIoPhase, 0xB8);
WWHD_OFFSET(hdSequenceWindow_c, mMessageIndex, 0xC0);
WWHD_OFFSET(hdSequenceWindow_c, mMessageLayoutMode, 0xC4);
WWHD_OFFSET(hdSequenceWindow_c, mSavedInputMode, 0xC8);
WWHD_OFFSET(hdSequenceWindow_c, mMenuFlagSet, 0xCA);
WWHD_OFFSET(hdSequenceWindow_c, mKeepPlaySettings, 0xCB);
WWHD_OFFSET(hdSequenceWindow_c, mRestoreInputMode, 0xCC);
WWHD_OFFSET(hdSequenceWindow_c, mSavedInputFlag, 0xCD);
WWHD_OFFSET(hdSequenceWindow_c, mCancelRequested, 0xCE);
WWHD_OFFSET(hdSequenceWindow_c, mButtonSelectionEnabled, 0xD0);
WWHD_OFFSET(hdSequenceWindow_c, mActive, 0xD3);
WWHD_OFFSET(hdSequenceWindow_c, mLoadRequested, 0xD4);
WWHD_OFFSET(hdSequenceWindow_c, mSaveRequested, 0xD5);
WWHD_OFFSET(hdSequenceWindow_c, mSecondScreen, 0xD6);
WWHD_OFFSET(hdSequenceWindow_c, mOutsideTouchArmed, 0xD8);
WWHD_OFFSET(hdSequenceWindow_c, mWaiting, 0xD9);
WWHD_OFFSET(hdSequenceWindow_c, mOpenComplete, 0xDA);
WWHD_SIZE(hdSequenceLayoutLabel_l, 0x20);
WWHD_OFFSET(hdSequenceTextRecord_c, mAnimation, 0);
WWHD_OFFSET(hdSequenceTextRecord_c, mButtonMode, 4);
WWHD_OFFSET(hdSequenceTextRecord_c, mDefaultButton, 8);
WWHD_OFFSET(hdSequenceTextRecord_c, mLabel, 0xC);
WWHD_OFFSET(hdSequenceModeRecord_c, mActionButtonB, 9);
WWHD_OFFSET(hdSequenceModeRecord_c, mControlMode, 0xA);
WWHD_OFFSET(hdSequenceWindow_c, mOpenComplete, 0xDA);
