/** HD sequence options screen. Game-derived */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_basic_button.h"
#include "hd/hd_ui_02001084.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_input_ctrl.h"
#include "d/hd_save_holders.h"
#include "d/hd_msg_text.h"
#include "d/d_operate_wind.h"

struct hdSequenceOption_c : hdParts_c {
    enum { SETTING_COUNT = 6, MODE_COUNT = 2, PANE_COUNT = 4, TEXT_PANE_COUNT = 9, ITEM_PANE_COUNT = 7,
           BUTTON_COUNT = 9, ANIMATION_COUNT = 12, ANIMATION_GROUP_COUNT = 7 };
    enum Mode { MODE_SETTINGS = 0, MODE_CONTROLLER = 1, MODE_UNOPENED = 2 };
    enum Category { CATEGORY_SCROLLING = 0, CATEGORY_CONTROLLER = 2 };
    enum Button { BUTTON_SCROLL_PREVIOUS = 7, BUTTON_SCROLL_NEXT = 8 };
    enum EventKind { EVENT_DECIDE = 4, EVENT_PREVIOUS = 11, EVENT_NEXT = 12 };
    /* 0x50 */ gptr<hdPartsPane_c> mPanes[PANE_COUNT];
    /* 0x60 */ gptr<hdTextPane_c> mTextPanes[TEXT_PANE_COUNT];
    /* 0x84 */ gptr<hdTextPane_c> mItemTextPanes[ITEM_PANE_COUNT];
    /* 0xA0 */ be<f32> mItemWidths[ITEM_PANE_COUNT];
    /* 0xBC */ be<f32> mItemHeights[ITEM_PANE_COUNT];
    /* 0xD8 */ be<f32> mItemHalfWidths[ITEM_PANE_COUNT];
    /* 0xF4 */ be<u32> mSavedSelections[MODE_COUNT];
    /* 0xFC */ be<u32> mSavedPageStarts[MODE_COUNT];
    /* 0x104 */ be<u8> mSettingValues[SETTING_COUNT];
    u8 mSettingPadding[2];
    /* 0x10C */ gptr<hdTextPane_c> mpSettingLabel;
    /* 0x110 */ gptr<hdTextPane_c> mpHelpLabel;
    /* 0x114 */ be<u32> mCategory;
    /* 0x118 */ be<u32> mMode;
    /* 0x11C */ be<u32> mPreviousCategory;
    /* 0x120 */ be<u32> mItemCount;
    /* 0x124 */ be<u32> mEntriesPerPage;
    /* 0x128 */ be<u32> mSelection;
    /* 0x12C */ be<u32> mPageStart;
    /* 0x130 */ be<u32> mScrollAnimation;
    /* 0x134 */ be<s32> mPreviousSelection;
    /* 0x138 */ be<u32> mConnectionDelay;
    /* 0x13C */ be<u32> mConnectionCounter;
    /* 0x140 */ be<u8> mConnectionMessage;
    /* 0x141 */ be<u8> mDirectionalInputEnabled;
    /* 0x142 */ be<u8> mTouchStarted;
    /* 0x143 */ be<u8> mScrollForward;
    /* 0x144 */ be<u8> mCloseRequested;
    /* 0x145 */ be<u8> mOutsideTouchArmed;
    u8 mTail[2];

    static u32 rejectInput();
    static u32 rejectTouch();
    static u32 rejectAction();
    static hdSequenceOption_c* construct(hdSequenceOption_c*);
    static void destroy(hdSequenceOption_c*, u32 flags);
    u32 settingValue(u32 index);
    void requestClose();
    void setSettingLabel(u32 selection);
    void setHelpLabel();
    void measureItem(u32 index);
    void setItemLabel(u32 selection);
    void setButtonLabel(u32 selection);
    u32 updateAlternateButton();
    void inAllExecute();
    void inAllEnter();
    void updateCursor();
    void scrollEnter();
    void scrollExit();
    void changeConnectExecute();
    void bindInputButtons();
    void buttonWaitExecute();
    u32 beginConnection();
    void open(u32 mode);
    void connectWaitEnter();
    void connectWaitExecute();
    void connectedWaitExecute();
    void outAllEnter();
    void outAllExit();
    void outAllExecute();
    void execute();
    void draw(void* info);
    void refreshLabels();
    u32 setup(u32 a, u32 b, u32 c);
    u32 load(void* name, u32 resource, u32 a, u32 b, void* descriptor);
    u32 createOptionChild(hdPartsDesc_c* descriptor);
    void findPanes();
    void findTextPanes();
    void findItemTextPanes();
    void changeConnectEnter();
    u32 buttonIndex(u32 selection);
    u32 settingValueCount(u32 selection);
    Pair32 settingAction(u32 selection);
    void changeSetting(u32 increment);
    void buttonWaitEnter();
    void bindButton(u32 index, u32 button, u32 action);
    void remove();
    void applySettings();
    void loadSettings();
    void clearButtonAnimations();
    void buttonWaitExit();
    void executeButtons();
    void scrollExecute();
    u32 pollCancel();
    s32 findFocusedSelection();
    u32 isPointOutside(struct hdOptionPoint_l* point);
};
WWHD_SIZE(hdSequenceOption_c, 0x148);
WWHD_OFFSET(hdSequenceOption_c, mSettingValues, 0x104);
WWHD_OFFSET(hdSequenceOption_c, mMode, 0x118);
WWHD_OFFSET(hdSequenceOption_c, mEntriesPerPage, 0x124);
WWHD_OFFSET(hdSequenceOption_c, mCloseRequested, 0x144);

struct hdOptionSettingRecord_l {
    be<u32> mNameIndex;
    be<u32> mDescriptionIndex;
    gptr<be<u32>> mpValueLabelIds;
    be<u8> mValueCount;
    u8 mReserved[3];
    hdPartsPtmf_c mAction;
};
WWHD_SIZE(hdOptionSettingRecord_l, 0x18);
WWHD_OFFSET(hdOptionSettingRecord_l, mValueCount, 0xC);
struct hdOptionSettingTables_l { gptr<hdOptionSettingRecord_l> mTables[3]; };
struct hdOptionButtonIds_l { be<u32> mIds[6]; };

struct hdOptionPoint_l { be<u32> mWords[2]; };
WWHD_SIZE(hdOptionPoint_l, 8);
WWHD_OFFSET(hdSequenceOption_c, mPanes, 0x50);

struct hdOptionArchiveName_l {
    u8 mReserved[0x1C];
    gptr<be<u8>> mpName;
};
WWHD_OFFSET(hdOptionArchiveName_l, mpName, 0x1C);
struct hdOptionLabels_l { hdUiSafeString mNames[38]; };
WWHD_OFFSET(hdSequenceOption_c, mpSettingLabel, 0x10C);
WWHD_OFFSET(hdSequenceOption_c, mpHelpLabel, 0x110);
WWHD_OFFSET(hdSequenceOption_c, mCategory, 0x114);

WWHD_OFFSET(hdSequenceOption_c, mConnectionDelay, 0x138);

WWHD_OFFSET(hdSequenceOption_c, mTextPanes, 0x60);
WWHD_OFFSET(hdSequenceOption_c, mItemTextPanes, 0x84);
WWHD_OFFSET(hdSequenceOption_c, mItemWidths, 0xA0);
WWHD_OFFSET(hdSequenceOption_c, mItemHeights, 0xBC);
WWHD_OFFSET(hdSequenceOption_c, mItemHalfWidths, 0xD8);
WWHD_OFFSET(hdSequenceOption_c, mSavedSelections, 0xF4);
WWHD_OFFSET(hdSequenceOption_c, mSavedPageStarts, 0xFC);
WWHD_OFFSET(hdSequenceOption_c, mPreviousCategory, 0x11C);
WWHD_OFFSET(hdSequenceOption_c, mItemCount, 0x120);
WWHD_OFFSET(hdSequenceOption_c, mSelection, 0x128);
WWHD_OFFSET(hdSequenceOption_c, mPageStart, 0x12C);
WWHD_OFFSET(hdSequenceOption_c, mScrollAnimation, 0x130);
WWHD_OFFSET(hdSequenceOption_c, mPreviousSelection, 0x134);
WWHD_OFFSET(hdSequenceOption_c, mConnectionCounter, 0x13C);
WWHD_OFFSET(hdSequenceOption_c, mConnectionMessage, 0x140);
WWHD_OFFSET(hdSequenceOption_c, mDirectionalInputEnabled, 0x141);
WWHD_OFFSET(hdSequenceOption_c, mTouchStarted, 0x142);
WWHD_OFFSET(hdSequenceOption_c, mScrollForward, 0x143);
WWHD_OFFSET(hdSequenceOption_c, mOutsideTouchArmed, 0x145);

struct hdOptionSaveRoot_l {
    u8 mSavePrefix[0x12C0];
    hdSaveHolders_c mHolders;
};
WWHD_OFFSET(hdOptionSaveRoot_l, mHolders, 0x12C0);

// Option callbacks beyond the canonical button-part slots.
struct hdOptionButtonVtable_l : hdPartsVtbl_c {
    u8 mKindSlots[0xE4 - sizeof(hdPartsVtbl_c)];
    gfn mAcceptsKind;
    u8 mEventSlots[0xF4 - 0xE8];
    gfn mAcceptsEvent;
    u8 mActionSlots[0x244 - 0xF8];
    gfn mSetEnabled;
};
WWHD_OFFSET(hdOptionButtonVtable_l, mAcceptsKind, 0xE4);
WWHD_OFFSET(hdOptionButtonVtable_l, mAcceptsEvent, 0xF4);

WWHD_OFFSET(hdOptionSettingRecord_l, mDescriptionIndex, 4);
WWHD_OFFSET(hdOptionSettingRecord_l, mpValueLabelIds, 8);

WWHD_OFFSET(hdOptionButtonVtable_l, mSetEnabled, 0x244);

struct hdOptionPaneNames_l { hdUiSafeString mNames[hdSequenceOption_c::PANE_COUNT]; };
struct hdOptionTextPaneNames_l { hdUiSafeString mNames[hdSequenceOption_c::TEXT_PANE_COUNT]; };
struct hdOptionItemPaneNames_l { hdUiSafeString mNames[hdSequenceOption_c::ITEM_PANE_COUNT]; };

WWHD_SIZE(hdOptionPaneNames_l, 0x20);
WWHD_SIZE(hdOptionTextPaneNames_l, 0x48);
WWHD_SIZE(hdOptionItemPaneNames_l, 0x38);


struct hdOptionMemberAction_l { be<u32> mWords[2]; };
WWHD_SIZE(hdOptionMemberAction_l, 8);

struct hdOptionAnimationGroups_l { be<u32> mGroups[hdSequenceOption_c::ANIMATION_COUNT]; };
WWHD_SIZE(hdOptionAnimationGroups_l, 0x30);




// Options extends the canonical BasicButton with its scrolling-arrow state.
struct hdOptionButtonState_l : hdBasicButton_c {
    be<u32> mScrollDirection;
    be<u8> mScrollButton;
};

WWHD_OFFSET(hdOptionButtonState_l, mScrollDirection, 0x8C);
WWHD_OFFSET(hdOptionButtonState_l, mScrollButton, 0x90);
WWHD_SIZE(hdOptionButtonState_l, 0x91);
struct hdOptionArchiveNames_l { hdUiSafeString mNames[3]; };
WWHD_SIZE(hdOptionArchiveNames_l, 0x18);

struct hdOptionAnimationNames_l { hdUiSafeString mNames[12]; };
struct hdOptionAnimationGroupNames_l { hdUiSafeString mNames[7]; };
WWHD_SIZE(hdOptionAnimationNames_l, 0x60);
WWHD_SIZE(hdOptionAnimationGroupNames_l, 0x38);

struct hdOptionChildDescriptor_l {
    u8 mDescriptorPrefix[8];
    hdUiSafeString mArchive;
    u8 mDescriptorMiddle[4];
    hdUiSafeString mPaneName;
    u8 mDescriptorTail[0xC];
};
WWHD_OFFSET(hdOptionChildDescriptor_l, mArchive, 8);
WWHD_OFFSET(hdOptionChildDescriptor_l, mPaneName, 0x14);
WWHD_SIZE(hdOptionChildDescriptor_l, 0x28);

// Narrow ABI views of the already merged message window and its owning HUD root.
struct hdOptionPromptStatus_l : hdParts_c {
    u8 mPromptPadding[0x81];
    be<u8> mAdvanceRequested;
    u8 mClosed;
    be<u8> mInputAccepted;
    u8 mRequestPadding[3];
    be<u8> mWaitComplete;
};
WWHD_OFFSET(hdOptionPromptStatus_l, mAdvanceRequested, 0xD1);
WWHD_OFFSET(hdOptionPromptStatus_l, mInputAccepted, 0xD3);
WWHD_OFFSET(hdOptionPromptStatus_l, mWaitComplete, 0xD7);
WWHD_SIZE(hdOptionPromptStatus_l, 0xD8);
struct hdOptionPromptRoot_l { u8 mRootPadding[0x1BC]; gptr<hdOptionPromptStatus_l> mpPrompt; };
WWHD_OFFSET(hdOptionPromptRoot_l, mpPrompt, 0x1BC);
WWHD_SIZE(hdOptionPromptRoot_l, 0x1C0);
struct hdOptionPromptIds_l { be<u32> mIds[4]; };
struct hdOptionConnectionSounds_l { gptr<const char> mNames[4]; };
WWHD_SIZE(hdOptionPromptIds_l, 0x10);
WWHD_SIZE(hdOptionConnectionSounds_l, 0x10);

WWHD_OFFSET(hdOptionButtonVtable_l, mAcceptsKind, 0xE4);
WWHD_OFFSET(hdOptionButtonVtable_l, mAcceptsEvent, 0xF4);

struct hdOptionChildArchiveNames_l { hdUiSafeString mNames[9]; };
struct hdOptionRegistration_l { be<u32> mLink[3]; };
struct hdOptionZeroVector_l { be<u32> mWords[4]; };
struct hdOptionLibraryFlag_l { u8 mFlag; };
WWHD_SIZE(hdOptionChildArchiveNames_l, 0x48);
WWHD_SIZE(hdOptionRegistration_l, 0xC);
WWHD_SIZE(hdOptionZeroVector_l, 0x10);
WWHD_SIZE(hdOptionLibraryFlag_l, 1);
WWHD_SIZE(hdOptionLabels_l, 0x130);

WWHD_OFFSET(hdOptionSettingRecord_l, mNameIndex, 0);
WWHD_OFFSET(hdOptionSettingRecord_l, mAction, 0x10);
WWHD_SIZE(hdOptionSettingTables_l, 0xC);
WWHD_SIZE(hdOptionButtonIds_l, 0x18);
WWHD_SIZE(hdOptionArchiveName_l, 0x20);
WWHD_SIZE(hdOptionButtonVtable_l, 0x248);


struct hdOptionEventQueue_l { u8 mQueuePrefix[0x1C]; hdUiEvent mCurrent; };
WWHD_OFFSET(hdOptionEventQueue_l, mCurrent, 0x1C);
WWHD_SIZE(hdOptionEventQueue_l, 0x44);
enum hdOptionPaneFlag { OPTION_PANE_VISIBLE = 1, OPTION_PANE_DIRTY = 0x10 };
enum hdOptionInputFlag { OPTION_INPUT_TOUCH = 0x8000 };

// HD cursor position temporary copied by GHS before floating-point reload.
struct hdOptionPosition_l { be<u32> mWords[3]; };
WWHD_SIZE(hdOptionPosition_l, 0xC);
