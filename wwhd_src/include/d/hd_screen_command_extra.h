/** HD-only command prompts; layouts recovered from the game binary. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_parts_no_control.h"
#include "d/hd_screen_item_screen_tu3.h"
#include "d/hd_screen_btn_collect_icon.h"

extern gptr<hdScreenUiSystem>& sCommandUiSystem;
extern hdUiSafeString& sCommandALayoutName;
extern hdUiSafeString& sCommandGuideLayoutName;
extern hdStateId& sCommandNullState;
extern be<u32>& sCommandStateCounterReady;
extern be<u32>& sCommandStateCounter;
void hdCommandInitializeAngle(be<u8>*);
void hdCommandInitializeShortVector(be<u8>*);

struct hdCommandPane_c : hdPartsPane_c {
    be<f32> mPosition[3];
    u8 _padding28[0x14];
    be<f32> mWidth;
    u8 _padding40[4];
    be<u8> mFlags;
};
WWHD_OFFSET(hdCommandPane_c, mPosition, 0x1C);
WWHD_OFFSET(hdCommandPane_c, mWidth, 0x3C);
WWHD_OFFSET(hdCommandPane_c, mFlags, 0x44);

struct hdCommandPromptA_c : NoControlPart_l {
    gptr<hdCommandPane_c> mpPanes[2];
    gptr<hdTextPane_c> mpTextPane;
    be<f32> mTextPosition[3];
    be<f32> mPaneWidths[2];
    be<u32> mWaitTimer;
    be<u8> mEnabled;
    u8 _padding75[3];
    static hdCommandPromptA_c* construct(hdCommandPromptA_c* self);
    static void destroy(hdCommandPromptA_c* self, u32 flags);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor);
    void bindPanes();
    void bindTextPane();
    u32 initialize(u32 first, u32 second, hdPartsDesc_c* descriptor);
    void alignText();
    void setMessage(PaneWork_l* message);
    void enterHidden();
    void enterIn();
    void enterWait();
    void executeIn();
    void executeOut();
    void enterOut();
    void hide();
    void disable();
};
WWHD_SIZE(hdCommandPromptA_c, 0x78);
WWHD_OFFSET(hdCommandPromptA_c, mpTextPane, 0x58);
WWHD_OFFSET(hdCommandPromptA_c, mTextPosition, 0x5C);
WWHD_OFFSET(hdCommandPromptA_c, mPaneWidths, 0x68);
WWHD_OFFSET(hdCommandPromptA_c, mWaitTimer, 0x70);
WWHD_OFFSET(hdCommandPromptA_c, mEnabled, 0x74);

struct hdCommandGuideVtbl_c : hdPartsVtbl_c {
    u8 _paddingC8[0x64];
    gfn mRefreshMessages;
    u8 _padding130[4];
    gfn mGetAvailableButtons;
    u8 _padding138[4];
    gfn mGetCommandStyle;
    u8 _padding140[4];
    gfn mGetButtonCommand;
    u8 _padding148[4];
    gfn mGetButtons;
    u8 _padding150[4];
    gfn mGetInAnimation;
    u8 _padding158[4];
    gfn mGetOutAnimation;
};
WWHD_OFFSET(hdCommandGuideVtbl_c, mRefreshMessages, 0x12C);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetAvailableButtons, 0x134);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetCommandStyle, 0x13C);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetButtonCommand, 0x144);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetButtons, 0x14C);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetInAnimation, 0x154);
WWHD_OFFSET(hdCommandGuideVtbl_c, mGetOutAnimation, 0x15C);
struct hdCommandGuide_c : hdParts_c {
    gptr<hdCommandPane_c> mpRootPane;
    gptr<hdTextPane_c> mpTextPanes[4];
    be<u32> mButtonFrames[6];
    be<u32> mButtonCounters[6];
    be<u32> mPreviousButtonFrames[6];
    be<u32> mPendingButtonFrames[6];
    be<u8> mButtonTransitions[6];
    be<u8> mButtonCommands[6];
    be<u32> mWaitTimer;
    be<u32> mInAnimation;
    be<u32> mOutAnimation;
    be<u8> mCachedCommand;
    be<u8> mCachedButtons;
    be<u8> mButtonGroup;
    be<u8> mCommandChanged;
    static hdCommandGuide_c* construct(hdCommandGuide_c* self);
    static void destroy(hdCommandGuide_c* self, u32 flags);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor);
    hdCommandGuideVtbl_c* guideVtable() {
        return hdPartsView<hdCommandGuideVtbl_c>(gabi::ea(mVtable.get()));
    }
    u32 getButtons() { return gabi::call_ptr<u32>(guideVtable()->mGetButtons.get(), this); }
    void refreshMessages() { gabi::call_ptr(guideVtable()->mRefreshMessages.get(), this); }
    void attachGuideArchives();
    void bindRootPane();
    void bindTextPanes();
    void initializePaneMessages();
    void resetButtons();
    void enterIn();
    void executeDisplay();
    void executeOut();
    void hide();
    void finalizeGuide();
    void drawGuide(void* info);
    void enterOut();
    void stopAnimation(u32 animation);
    void setAnimationFrame(u32 animation, u32 frame);
    void setButtonFrame(u32 button, u32 frame);
    void playAnimation(u32 animation);
    void executeIn();
    u32 getButtonAnimation(u32 button);
    void enterWait();
    u32 initializeGuide(u32 first, u32 second, hdPartsDesc_c* descriptor);
    void executeGuideState();
    u32 createCommandChild(hdPartsDesc_c* descriptor);
    u32 applyItemCommand(u32 button, u32 command);
    u32 applyButtonCommand(u32 button, u32 command);
    void setButtonMessage(u32 button);
    void hideMessageGroup();
    void updateCommandButton(u32 button);
    void updateItemButton(u32 button);
    void updateGuideMessages();
    u32 getButtonCommand(u32 button);
    u32 getAvailableButtons();
    u32 getCommandMode();
    u32 getButtonCommandStyle(u32 button, u32 command);
    u32 canShowCommandText(u32 command);
    u32 getContextPrimaryCommand();
    u32 getContextSecondaryCommand();
    u32 getPrimaryCommand();
    u32 getSecondaryCommand();
};
WWHD_OFFSET(hdCommandGuide_c, mWaitTimer, 0xD0);
WWHD_OFFSET(hdCommandGuide_c, mCachedCommand, 0xDC);
WWHD_OFFSET(hdCommandGuide_c, mCachedButtons, 0xDD);

struct hdCommandButtonGuide_c : hdCommandGuide_c {
    be<u32> mInputTimer;
    static hdCommandButtonGuide_c* construct(hdCommandButtonGuide_c* self);
    static void destroy(hdCommandButtonGuide_c* self, u32 flags);
    u32 initializeButtonGuide(u32 first, u32 second, hdPartsDesc_c* descriptor);
    void updateButtonGuideMessages();
    u32 getCurrentCommand();
    void executeHidden();
    void enterIn();
    void enterOut();
    void executeDisplay();
    void executeIn();
    void executeOut();
    void finalizeGuide();
    void executeState();
    void drawGuide(void* info);
    u32 getPrimaryCommand();
    u32 getSecondaryCommand();
    u32 getGuideKind();
    u32 getVisibleButtons();
    u32 getEnabledButtons();
    u32 getButtonCommand(u32 button);
};

WWHD_OFFSET(hdCommandGuide_c, mpRootPane, 0x50);
WWHD_OFFSET(hdCommandGuide_c, mpTextPanes, 0x54);
WWHD_OFFSET(hdCommandGuide_c, mButtonFrames, 0x64);
WWHD_OFFSET(hdCommandGuide_c, mPreviousButtonFrames, 0x94);
WWHD_OFFSET(hdCommandGuide_c, mButtonCommands, 0xCA);
WWHD_SIZE(hdCommandGuide_c, 0xE0);
WWHD_OFFSET(hdCommandGuide_c, mButtonCounters, 0x7C);
WWHD_OFFSET(hdCommandGuide_c, mPendingButtonFrames, 0xAC);
WWHD_OFFSET(hdCommandGuide_c, mButtonTransitions, 0xC4);
WWHD_OFFSET(hdCommandGuide_c, mButtonGroup, 0xDE);
WWHD_OFFSET(hdCommandButtonGuide_c, mInputTimer, 0xE0);
WWHD_SIZE(hdCommandButtonGuide_c, 0xE4);

// The item buttons are the merged item-screen Decide parts (hdItemDecide_c, DRC-item_screen). The collection
// button (MENU-btn_collect_icon) is not merged yet: only its allocation extent and the one word the guide sets.
hdParts_c* createCommandItemButton(void* context, u32 kind, hdPartsDesc_c* descriptor);
hdParts_c* createCommandCollectButton(void* context, u32 kind, hdPartsDesc_c* descriptor);
hdParts_c* createCommandAPrompt(void* context, u32 kind, hdPartsDesc_c* descriptor);
