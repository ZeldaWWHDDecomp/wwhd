/**
 * hd_msg_only_text_panes.cpp
 * HD - the message-only window: its text panes, with the message pane's original position kept
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
using namespace gabi;
struct hdMsgOnlyTextNames_c { hdUiSafeString mItems[3]; };
struct hdMsgOnlyTextRegistration_c { be<u32> mWords[3]; };
struct hdMsgOnlyTextAliases_l { u8 mReserved[0x6C]; gptr<hdTextPane_c> mpAlternates[2]; };
struct hdMsgOnlyDefaultColor_l { u8 mReserved[0x112]; be<u32> mColorBits; };
struct hdMsgOnlyPaneColor_l { u8 mReserved[0xAC]; be<u32> mColorBits; };
WWHD_OFFSET(hdMsgOnlyTextAliases_l, mpAlternates, 0x6C);
WWHD_OFFSET(hdMsgOnlyDefaultColor_l, mColorBits, 0x112);
WWHD_OFFSET(hdMsgOnlyPaneColor_l, mColorBits, 0xAC);
static hdMsgOnlyTextNames_c sMsgOnlyTextPaneNames;
static hdUiStringTable sMsgOnlyTextPaneStringVtable;
static u32 sMsgOnlyTextPaneNamesInitialized;
static hdMsgOnlyTextRegistration_c sMsgOnlyTextPaneRegistration;
extern void msgOnlyTextRegister(hdMsgOnlyTextRegistration_c*);
extern void msgOnlyTextSetCapacity(hdTextPane_c*, u32);

/* 026AFD28 */
void hdMsgOnlyWindow_c::findTextPanes() {
    if (!sMsgOnlyTextPaneNamesInitialized) {
        sMsgOnlyTextPaneNamesInitialized = 1;
        sMsgOnlyTextPaneNames.mItems[0].mVtable = &sMsgOnlyTextPaneStringVtable;
        sMsgOnlyTextPaneNames.mItems[0].mText = at<be<u8>>(guest_addr("T_Msg_00"));
        sMsgOnlyTextPaneNames.mItems[1].mVtable = &sMsgOnlyTextPaneStringVtable;
        sMsgOnlyTextPaneNames.mItems[1].mText = at<be<u8>>(guest_addr("T_Choise2_00"));
        sMsgOnlyTextPaneNames.mItems[2].mVtable = &sMsgOnlyTextPaneStringVtable;
        sMsgOnlyTextPaneNames.mItems[2].mText = at<be<u8>>(guest_addr("T_Choise3_00"));
        msgOnlyTextRegister(&sMsgOnlyTextPaneRegistration);
    }
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i<3; ++i) {
        hdUiSafeString* name = &sMsgOnlyTextPaneNames.mItems[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        auto* pane = call_ptr<hdTextPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        if (i == 0) mWaitPane = reinterpret_cast<HdMessagePane*>(pane);
        else mpAdditionalText[i-1] = pane;
        pane = i == 0?reinterpret_cast<hdTextPane_c*>(mWaitPane.get()):mpAdditionalText[i-1].get();
        if (pane) msgOnlyTextSetCapacity(pane, HD_MESSAGE_ALL_SHOWN);
    }
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mWaitPane.get());
    mSavedTextXBits = pane->mTranslationXBits;
    mSavedTextYBits = pane->mTranslationYBits;
    mSavedTextZBits = pane->mTranslationZBits;
    pane = reinterpret_cast<hdPartsPaneSRT_c*>(mWaitPane.get());
    pane->mFlags = pane->mFlags&~1u;
    mPane = mWaitPane;
    auto* aliases = reinterpret_cast<hdMsgOnlyTextAliases_l*>(this);
    aliases->mpAlternates[0] = mpAdditionalText[0];
    aliases->mpAlternates[1] = mpAdditionalText[1];
    auto* color = reinterpret_cast<hdMsgOnlyPaneColor_l*>(mWaitPane.get());
    reinterpret_cast<hdMsgOnlyDefaultColor_l*>(this)->mColorBits = color->mColorBits.get();
}

/* WWHD glue
 data sMsgOnlyTextPaneNames 10497EC8
 data sMsgOnlyTextPaneStringVtable 100FCF14
 data sMsgOnlyTextPaneNamesInitialized 10497F74
 data sMsgOnlyTextPaneRegistration 101F6564
 str 100FD0E4 "T_Msg_00" in 026AFD28
 str 100FD0F0 "T_Choise2_00" in 026AFD28
 str 100FD100 "T_Choise3_00" in 026AFD28
 ext 028F026C void msgOnlyTextRegister(hdMsgOnlyTextRegistration_c*)
 ext 02704AFC void msgOnlyTextSetCapacity(hdTextPane_c*,u32)
*/
