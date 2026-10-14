/**
 * hd_msg_only_choice_panes.cpp
 * HD - the message-only window: its two choice panes
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
using namespace gabi;
struct hdMsgOnlyChoiceNames_c { hdUiSafeString mItems[2]; };
struct hdMsgOnlyChoiceRegistration_c { be<u32> mWords[3]; };
struct hdMsgOnlyChoiceAliases_l { u8 mReserved[0x74]; gptr<hdPartsPane_c> mpChoices[2]; };
WWHD_OFFSET(hdMsgOnlyChoiceAliases_l, mpChoices, 0x74);
static hdMsgOnlyChoiceNames_c sMsgOnlyChoicePaneNames;
static hdUiStringTable sMsgOnlyChoicePaneStringVtable;
static u32 sMsgOnlyChoicePaneNamesInitialized;
static hdMsgOnlyChoiceRegistration_c sMsgOnlyChoicePaneRegistration;
extern void msgOnlyChoiceRegister(hdMsgOnlyChoiceRegistration_c*);

/* 026AFF38 */
void hdMsgOnlyWindow_c::findChoicePanes() {
    if (!sMsgOnlyChoicePaneNamesInitialized) {
        sMsgOnlyChoicePaneNamesInitialized = 1;
        sMsgOnlyChoicePaneNames.mItems[0].mVtable = &sMsgOnlyChoicePaneStringVtable;
        sMsgOnlyChoicePaneNames.mItems[0].mText = at<be<u8>>(guest_addr("W_Choise2_00"));
        sMsgOnlyChoicePaneNames.mItems[1].mVtable = &sMsgOnlyChoicePaneStringVtable;
        sMsgOnlyChoicePaneNames.mItems[1].mText = at<be<u8>>(guest_addr("W_Choise3_00"));
        msgOnlyChoiceRegister(&sMsgOnlyChoicePaneRegistration);
    }
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i<2; ++i) {
        hdUiSafeString* name = &sMsgOnlyChoicePaneNames.mItems[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        mpChoicePanes[i] = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
    auto* aliases = reinterpret_cast<hdMsgOnlyChoiceAliases_l*>(this);
    aliases->mpChoices[0] = mpChoicePanes[0];
    aliases->mpChoices[1] = mpChoicePanes[1];
}

/* WWHD glue
 data sMsgOnlyChoicePaneNames 10497EE0
 data sMsgOnlyChoicePaneStringVtable 100FCF14
 data sMsgOnlyChoicePaneNamesInitialized 10497F78
 data sMsgOnlyChoicePaneRegistration 101F6570
 str 100FD110 "W_Choise2_00" in 026AFF38
 str 100FD120 "W_Choise3_00" in 026AFF38
 ext 028F026C void msgOnlyChoiceRegister(hdMsgOnlyChoiceRegistration_c*)
*/
