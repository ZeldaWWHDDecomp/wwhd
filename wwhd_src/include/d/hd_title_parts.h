/* HD title and controller-selection parts. Names describe binary-observed roles;
 * the shared framework base uses the merged HD layout. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_parts_basic_button.h"

struct hdTitleScrollKeyIcon_c : hdPartsKeyIcon_c {
    static hdTitleScrollKeyIcon_c* construct(hdTitleScrollKeyIcon_c* self);
    static void destroy(hdTitleScrollKeyIcon_c* self, u32 flags);
    u32 initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc);
};
WWHD_SIZE(hdTitleScrollKeyIcon_c, 0x58);

struct hdTitleControllerScreen_c : hdParts_c {
    u8 _paddingSelection[4];
    be<u32> mSelectedController;
    u32 initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc);
    void connectControllerReceivers();
    void bindControllerSounds();
    void finalizeParts();
    void updateState();
    void draw(void* info);
    u32 isPrimarySelected();
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    u32 createNamedChild(hdPartsDesc_c* desc);
};
WWHD_OFFSET(hdTitleControllerScreen_c, mSelectedController, 0x54);

/* The selection screen can override the two controller-availability queries. */
struct hdTitleControllerAvailability {
    be<u32> mOverrideEnabled;
    be<u32> mPrimaryAvailable;
    be<u32> mSecondaryAvailable;
};
WWHD_SIZE(hdTitleControllerAvailability, 12);
WWHD_OFFSET(hdTitleControllerAvailability, mOverrideEnabled, 0);
WWHD_OFFSET(hdTitleControllerAvailability, mPrimaryAvailable, 4);
WWHD_OFFSET(hdTitleControllerAvailability, mSecondaryAvailable, 8);

/* Provisional names: the three current setting bytes are saved and restored together. */
struct hdTitleControllerPlay_l {
    u8 _padding[0x5C28];
    be<u8> mCurrentSettings[3];
    be<u8> mSavedSettings[3];
};
WWHD_OFFSET(hdTitleControllerPlay_l, mCurrentSettings, 0x5C28);
WWHD_OFFSET(hdTitleControllerPlay_l, mSavedSettings, 0x5C2B);
WWHD_SIZE(hdTitleControllerPlay_l, 0x5C2E);

struct hdTitleControllerUi_l {
    u8 _padding[0x40];
    be<u8> mMode;
    u8 _paddingMode[0x19];
    be<u8> mSettingsRestored;
};
WWHD_OFFSET(hdTitleControllerUi_l, mMode, 0x40);
WWHD_OFFSET(hdTitleControllerUi_l, mSettingsRestored, 0x5A);
WWHD_SIZE(hdTitleControllerUi_l, 0x5B);

struct hdTitleControllerSelection_c : hdParts_c {
    be<u8> mSelectionPending;
    be<u8> mInAnimationFinished;
    u8 _paddingSelection[2];
    be<u32> mSelectedController;
    gptr<void> mpControllerRoot;
    gptr<void> mControllerPanes[2];
    u8 _paddingTail[4];
    static hdTitleControllerSelection_c* construct(hdTitleControllerSelection_c* self);
    static void destroy(hdTitleControllerSelection_c* self, u32 flags);
    void calculateLayout();
    void restorePrimarySettings();
    void restoreSecondarySettings();
    void beginInAnimation();
    void waitInAnimation();
    void waitInitialAnimation();
    void beginInitialAnimation();
    void resetControllerButtons();
    void updateControllerPrompt();
    void updateSelection();
    void savePrimarySettings();
    void saveSecondarySettings();
    void waitPrimaryConfirmation();
    void waitSecondaryConfirmation();
    void findControllerRoot();
    void findControllerPane();
};
WWHD_OFFSET(hdTitleControllerSelection_c, mSelectionPending, 0x50);
WWHD_OFFSET(hdTitleControllerSelection_c, mInAnimationFinished, 0x51);
WWHD_OFFSET(hdTitleControllerSelection_c, mSelectedController, 0x54);
WWHD_OFFSET(hdTitleControllerSelection_c, mpControllerRoot, 0x58);
WWHD_OFFSET(hdTitleControllerSelection_c, mControllerPanes, 0x5C);
WWHD_SIZE(hdTitleControllerSelection_c, 0x68);

struct hdTitleStartBackground_c : hdParts_c {
    enum { HEADER_ANIMATION = 4 };
    be<u32> mCaptionIndex;
    be<u32> mAnimationPhase;
    gptr<void> mpHeaderText;
    gptr<struct hdTitleBackgroundPane_l> mpHeaderWindow;
    static hdTitleStartBackground_c* construct(hdTitleStartBackground_c* self);
    u32 initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc);
    void destroy(u32 flags);
    void calculateLayout();
    void startHeaderAnimation();
    void waitHeaderAnimation();
    void finalizeParts();
    void updateState();
    void requestHeaderState();
    void setCaptionIndex(u32 index);
    void showHeader();
    void hideHeader();
    void beginBackgroundAnimation();
    void waitBackgroundAnimation();
    void beginCaptionAnimation();
    void updateCaptionAnimation();
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdTitleStartBackground_c, mCaptionIndex, 0x50);
WWHD_OFFSET(hdTitleStartBackground_c, mAnimationPhase, 0x54);
WWHD_OFFSET(hdTitleStartBackground_c, mpHeaderText, 0x58);
WWHD_OFFSET(hdTitleStartBackground_c, mpHeaderWindow, 0x5C);
WWHD_SIZE(hdTitleStartBackground_c, 0x60);

/* Data views of the library-owned layout and root pane; library code stays external. */
struct hdTitleBackgroundPane_l {
    u8 _padding[0x3C];
    be<f32> mX;
    be<f32> mY;
    be<u8> mFlags;
};
WWHD_OFFSET(hdTitleBackgroundPane_l, mX, 0x3C);
WWHD_OFFSET(hdTitleBackgroundPane_l, mY, 0x40);
WWHD_OFFSET(hdTitleBackgroundPane_l, mFlags, 0x44);
struct hdTitleBackgroundLayoutData_l {
    u8 _padding[0xC];
    gptr<hdTitleBackgroundPane_l> mpRootPane;
    u8 _paddingRoot[0xC];
    gptr<be<u8>> mpTextResources;
};
WWHD_OFFSET(hdTitleBackgroundLayoutData_l, mpRootPane, 0xC);
WWHD_OFFSET(hdTitleBackgroundLayoutData_l, mpTextResources, 0x1C);

struct hdTitleLogo_c : hdParts_c {
    be<u8> mIntroStarted;
    be<u8> mAdvanceRequested;
    be<u8> mSkipRequested;
    be<u8> mShortIntro;
    gptr<void> mTextPanes[8];
    gptr<void> mLogoPanes[8];
    gptr<void> mSpecialPanes[3];
    static hdTitleLogo_c* construct(hdTitleLogo_c* self);
    u32 initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc);
    void destroy(u32 flags);
    void calculateLayout();
    void findLogoPanes();
    void findTextPanes();
    void findSpecialPanes();
    void clearAdvanceRequest();
    void updateIdle();
    void beginIntro();
    void updateIntro();
    void beginLoop();
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void finalizeParts();
    void updateState();
};
WWHD_OFFSET(hdTitleLogo_c, mIntroStarted, 0x50);
WWHD_OFFSET(hdTitleLogo_c, mAdvanceRequested, 0x51);
WWHD_OFFSET(hdTitleLogo_c, mSkipRequested, 0x52);
WWHD_OFFSET(hdTitleLogo_c, mShortIntro, 0x53);
WWHD_OFFSET(hdTitleLogo_c, mTextPanes, 0x54);
WWHD_OFFSET(hdTitleLogo_c, mLogoPanes, 0x74);
WWHD_OFFSET(hdTitleLogo_c, mSpecialPanes, 0x94);
WWHD_SIZE(hdTitleLogo_c, 0xA0);
