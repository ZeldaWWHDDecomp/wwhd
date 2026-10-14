/**
 * hd_msg_only_selection_panes.cpp
 * HD - the message-only window: the selection marker pane and its animation resource
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
using namespace gabi;
struct hdMsgOnlySelectionAliases_l { u8 mReserved[0x7C]; gptr<void> mpResource; gptr<hdPartsPane_c> mpMarker; };
struct hdMsgOnlySelectionRegistration_c { be<u32> mWords[3]; };
WWHD_OFFSET(hdMsgOnlySelectionAliases_l, mpResource, 0x7C);
WWHD_OFFSET(hdMsgOnlySelectionAliases_l, mpMarker, 0x80);
static hdUiSafeString sMsgOnlySelectionMarkerName;
static hdUiSafeString sMsgOnlySelectionResourceName;
static hdUiStringTable sMsgOnlySelectionStringVtable;
static u32 sMsgOnlySelectionResourceInitialized;
static hdMsgOnlySelectionRegistration_c sMsgOnlySelectionRegistration;
extern void msgOnlySelectionRegister(hdMsgOnlySelectionRegistration_c*);

/* 026B00A8 */
void hdMsgOnlyWindow_c::findSelectionPane() {
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
    sMsgOnlySelectionMarkerName.assureTermination();
    auto* pane = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, sMsgOnlySelectionMarkerName.mText.get(), 1);
    mpSelectionPane = pane;
    reinterpret_cast<hdMsgOnlySelectionAliases_l*>(this)->mpMarker = pane;
}

/* 026B011C */
void hdMsgOnlyWindow_c::findSelectionResource() {
    if (!sMsgOnlySelectionResourceInitialized) {
        sMsgOnlySelectionResourceInitialized = 1;
        sMsgOnlySelectionResourceName.mVtable = &sMsgOnlySelectionStringVtable;
        sMsgOnlySelectionResourceName.mText = at<be<u8>>(guest_addr("P_SelNumber_00"));
        msgOnlySelectionRegister(&sMsgOnlySelectionRegistration);
    }
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
    sMsgOnlySelectionResourceName.assureTermination();
    auto* pane = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, sMsgOnlySelectionResourceName.mText.get(), 1);
    mpSelectionResource = pane;
    pane->mFlags = pane->mFlags&~1u;
    reinterpret_cast<hdMsgOnlySelectionAliases_l*>(this)->mpResource = mpSelectionResource;
    windowStates()->startAnimation(1, 4);
}

/* WWHD glue
 data sMsgOnlySelectionMarkerName 10497E8C
 data sMsgOnlySelectionResourceName 10497E84
 data sMsgOnlySelectionStringVtable 100FCF14
 data sMsgOnlySelectionResourceInitialized 10497F7C
 data sMsgOnlySelectionRegistration 101F657C
 str 100FD130 "P_SelNumber_00" in 026B011C
 ext 028F026C void msgOnlySelectionRegister(hdMsgOnlySelectionRegistration_c*)
*/
