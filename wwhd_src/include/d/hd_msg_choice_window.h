/* HD: the talk window with choices (hd_msg_choice_window.cpp, _states, _text_panes, _control): a message
 * window with a two-choice part, opened on the TV or the GamePad. No GameCube counterpart (the GameCube
 * message UI is a different design). Layout, pane and sound names are HD strings; class, member and method
 * names are ours. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"
struct hdTalkChoiceWidth_c { u8 mReserved[0x688]; be<u32> mWidthBits; };
struct hdTalkPaneWidth_c { u8 mReserved[0x3C]; be<u32> mWidthBits; };
struct hdTalkChoiceText_c { u8 mReserved[0x6C0]; PaneString_l mRight; u8 mReserved6CC[0x100]; PaneString_l mLeft; };
struct hdMsgChoiceWindow_c : hdParts_c {
    /* 0x50 */ be<u32> mFlags;
    /* 0x54 */ be<u32> mInputFlags;
    /* 0x58 */ be<u32> mInputMode;
    /* 0x5C */ be<u32> mMessageId;
    /* 0x60 */ gptr<hdTextPane_c> mpWindow;
    /* 0x64 */ gptr<hdTextPane_c> mpChoiceRoot;
    /* 0x68 */ gptr<hdTextPane_c> mpPanes[4];
    /* 0x78 */ be<u32> mChoiceMode;
    /* 0x7C */ be<u8> mChoiceActive;
    /* 0x7D */ be<u8> mClosing;
    /* 0x7E */ u8 mReserved7E[2];
    /* 0x80 */ be<u32> mSelectedChoice;
    /* 0x84 */ be<u32> mSelection;
    /* 0x88 */ be<u8> mOnTv;            /* shown on the TV (open sound 0x80C); otherwise on the GamePad ("SE_TALK_WIN_OPEN_DRC") */
    /* 0x89 */ be<u8> mCancelAllowed;   /* cancel closes the selection (choice 0) */
    /* 0x8A */ be<u8> mChoiceChanged;
    /* 0x8B */ u8 mReserved8B;
    /* 0x8C */ gptr<PaneWork_l> mpMessage;
    /* 0x90 */ be<u32> mSecondChoiceSound; /* on deciding the second choice (0: 0x80D) */
    /* 0x94 */ be<u32> mFirstChoiceSound;  /* on deciding the first choice (0: 0x880) */
    static hdMsgChoiceWindow_c* construct(hdMsgChoiceWindow_c*);
    void destroy(u32);
    void prepareChoices();
    void playAnimation(u32);
    u32 isAnimationEnded(u32);
    void updateOpen();
    void updateReopen();
    void enterClose();
    void updateClose();
    void setAnimationFrame(u32, f32);
    void hide();
    void finishMessage();
    void resetMessage();
    void enterReopen();
    void updateWait();
    void updateSelection();
    void enterSelection();
    void findWindowPanes();
    void findTextPanes();
    void enterOpen();
    void clearSounds();
    u32 initialize(u32, u32, void*);
    void release();
    void updateCursor();
    void execute();
    void drawTv(void*);
    void drawGamePad(void*);
    void setSounds(u32, u32);
    void requestClose();
    u32 startMessage(u32, u32, u32, u32);
    void showCustomMessage(u32, u32);
    u32 loadLayout(void*, void*, u32, u32, void*);
};
WWHD_OFFSET(hdMsgChoiceWindow_c, mpPanes, 0x68);
WWHD_OFFSET(hdMsgChoiceWindow_c, mChoiceMode, 0x78);
WWHD_OFFSET(hdMsgChoiceWindow_c, mpMessage, 0x8C);
WWHD_SIZE(hdMsgChoiceWindow_c, 0x98);
WWHD_OFFSET(hdTalkChoiceWidth_c, mWidthBits, 0x688);
WWHD_OFFSET(hdTalkChoiceText_c, mLeft, 0x7CC);
