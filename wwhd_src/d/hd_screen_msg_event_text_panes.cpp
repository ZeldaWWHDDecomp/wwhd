/* HD event-window text-pane lookup; game setup around external SafeString and pane operations. */
#include "d/hd_msg_event_position.h"
#include "d/hd_msg_window_event_companion.h"
static hdMsgEventTextNames_l sEventTextNames;
static u32 sEventTextNamesGuard;
static hdUiStringTable sEventTextStringVtable;
static hdMsgEventFrameRegistration_l sEventTextNamesRegistration;

/* 026B37F4 */
void hdMsgEventPosition_c::bindTextPanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sEventTextNamesGuard) {
        sEventTextNamesGuard = 1;
        sEventTextNames.mNames[0].mVtable = &sEventTextStringVtable;
        sEventTextNames.mNames[0].mText.v = gabi::guest_addr("T_MsgLine1_00");
        sEventTextNames.mNames[1].mVtable = &sEventTextStringVtable;
        sEventTextNames.mNames[1].mText.v = gabi::guest_addr("T_MsgL_00");
        sEventTextNames.mNames[2].mVtable = &sEventTextStringVtable;
        sEventTextNames.mNames[2].mText.v = gabi::guest_addr("T_MsgS_00");
        hdMsgEventFrame_register(&sEventTextNamesRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i < 3; ++i) {
        hdUiSafeString* name = &sEventTextNames.mNames[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        u32 find = table->mFindPane.get();
        const char* text = reinterpret_cast<const char*>(name->mText.get());
        mTextPanes[i] = reinterpret_cast<hdMsgEventVisiblePane_l*>(gabi::call_ptr<hdPartsPaneSRT_c*>(find, root, text, 1));
        if (mTextPanes[i]) mTextPanes[i]->hide();
    }
    auto* smallText = reinterpret_cast<hdMsgEventTextPane_l*>(mTextPanes[2].get());
    mSmallTextColor = u32(smallText->mColorTop);
}
/* WWHD glue
 data sEventTextNames 104981F4
 data sEventTextNamesGuard 10498398
 data sEventTextStringVtable 100FD88C
 data sEventTextNamesRegistration 101F65C4
 str 100FDB78 "T_MsgLine1_00"
 str 100FDB88 "T_MsgL_00"
 str 100FDB94 "T_MsgS_00"
*/
