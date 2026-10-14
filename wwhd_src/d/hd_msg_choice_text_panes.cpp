/**
 * hd_msg_choice_text_panes.cpp
 * HD - the talk window with choices: its four text panes ("T_Msg_00", "T_MsgLine1_00",
 * "T_ChoiseL_00", "T_ChoiseR_00"), each with room for 335 characters
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_choice_window.h"
using namespace gabi;
struct hdTalkTextPaneNames_c { hdUiSafeString mItems[4]; };
struct hdTalkTextRegistration_c { be<u32> mWords[3]; };
static hdTalkTextPaneNames_c sTalkTextPaneNames;
static u32 sTalkTextPaneNamesInitialized;
static hdTalkTextRegistration_c sTalkTextPaneNameRegistration;
extern void talkTextRegister(hdTalkTextRegistration_c*);
static hdUiStringTable sTalkTextStringVtable;
enum { TalkTextCapacity = 0x14F };

/* 026AD9F0 */
void hdMsgChoiceWindow_c::findTextPanes() {
    if (!sTalkTextPaneNamesInitialized) {
        sTalkTextPaneNamesInitialized = 1;
        sTalkTextPaneNames.mItems[0].mVtable = &sTalkTextStringVtable;
        sTalkTextPaneNames.mItems[0].mText.v = guest_str("T_Msg_00");
        sTalkTextPaneNames.mItems[1].mVtable = &sTalkTextStringVtable;
        sTalkTextPaneNames.mItems[1].mText.v = guest_str("T_MsgLine1_00");
        sTalkTextPaneNames.mItems[2].mVtable = &sTalkTextStringVtable;
        sTalkTextPaneNames.mItems[2].mText.v = guest_str("T_ChoiseL_00");
        sTalkTextPaneNames.mItems[3].mVtable = &sTalkTextStringVtable;
        sTalkTextPaneNames.mItems[3].mText.v = guest_str("T_ChoiseR_00");
        talkTextRegister(&sTalkTextPaneNameRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 4; ++index) {
        hdUiSafeString* name = &sTalkTextPaneNames.mItems[index];
        hdPartsPaneSRTVtbl_c* vtable = root->srtVtbl();
        name->assureTermination();
        mpPanes[index] = call_ptr<hdTextPane_c*>(vtable->mFindPane.get(), root, name->mText.get(), 1);
        hdTextPane_c* pane = mpPanes[index];
        if (pane) pane->setCharacterCapacity(TalkTextCapacity);
    }
}

/* WWHD glue
 data sTalkTextStringVtable 100FCA8C
 data sTalkTextPaneNames 10497CC8
 data sTalkTextPaneNamesInitialized 10497D3C
 data sTalkTextPaneNameRegistration 101F6504
 str 100FCCA0 "T_Msg_00"
 str 100FCC90 "T_MsgLine1_00"
 str 100FCCAC "T_ChoiseL_00"
 str 100FCCBC "T_ChoiseR_00"
 ext 028F026C void talkTextRegister(hdTalkTextRegistration_c*)
 ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
*/
