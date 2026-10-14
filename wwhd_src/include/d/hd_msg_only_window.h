/* HD: the message-only window "MsgOnly_00" (hd_msg_only_window.cpp, _statics, _text_panes, _choice_panes,
 * _selection_panes, _control). It derives from the merged message-only base HdMessageOnly
 * (d/hd_msg_window_a.h), which is FW-parts_msg_base's message window (hdMsgWindowStates_c, reached
 * through windowStates()); this header adds the window's own members from 0x130. No GameCube counterpart.
 * Layout and pane names are HD strings; class, member and method names are ours. Local game-derived
 * */
#pragma once
#include "d/hd_msg_window_a.h"
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_msg_window_states.h"
struct hdMsgOnlyWindow_c : HdMessageOnly {
    /* the merged message-window base (FW-parts_msg_base) of the same object */
    hdMsgWindowStates_c* windowStates() { return (hdMsgWindowStates_c*)(void*)this; }
    /* 0x130 */ gptr<hdTextPane_c> mpAdditionalText[2];
    /* 0x138 */ be<u32> mSavedTextXBits;      /* the message pane's original position (raw float words) */
    /* 0x13C */ be<u32> mSavedTextYBits;
    /* 0x140 */ be<u32> mSavedTextZBits;
    /* 0x144 */ gptr<hdPartsPane_c> mpChoicePanes[2];
    /* 0x14C */ gptr<hdPartsPane_c> mpSelectionPane;
    /* 0x150 */ gptr<void> mpSelectionResource;
    /* 0x154 */ be<u32> mRevision;
    /* 0x158 */ gptr<void> mpRequest;
    void destroy(u32);
    u32 isSpecialMessage(); void remapMessage(); void prepareMessage();
    void updateRequest(); void findWindowPanes();
    void findTextPanes(); void findChoicePanes();
    void findSelectionPane(); void findSelectionResource();
    void hideTextPanes(); void hideChoicePanes();
    void updateAttributeAnimation(); void resetAndHide();
    void release();
    void updateSelectionSound();
    void execute();
    void stepText();
    u32 initialize(u32, u32, void*);
    u32 loadLayout(void*, void*, u32, u32, void*);
    u32 createChild(hdPartsDesc_c*);
    void resetAndOpenMessage();
    void playOpen(); u32 isOpenEnded(); void playClose(); u32 isCloseEnded();
    void playTextChange(); void playSubWindowIn(); u32 isSubWindowInEnded();
    void playSubWindowOut(); u32 isSubWindowOutEnded();
    void hideSelection(); void showSelection(); void decideSelection(); void closeSelection();
    u32 isSelectionWaiting(); u32 isSelectionHidden(); u32 isSelectionNotDeciding();
    void enterOpen(); void updateWaitMode(); void enterClose();
    void enterAdvance(); void enterInputAdvance(); void updateAdvance();
    void finishAdvance(); void updateReopen(); void updateStoppedAdvance();
    static hdMsgOnlyWindow_c* construct(hdMsgOnlyWindow_c*);
};
WWHD_OFFSET(hdMsgOnlyWindow_c, mpAdditionalText, 0x130);
WWHD_OFFSET(hdMsgOnlyWindow_c, mpChoicePanes, 0x144);
WWHD_OFFSET(hdMsgOnlyWindow_c, mRevision, 0x154);
WWHD_SIZE(hdMsgOnlyWindow_c, 0x15C);

WWHD_OFFSET(hdMsgOnlyWindow_c, mSavedTextXBits, 0x138);
