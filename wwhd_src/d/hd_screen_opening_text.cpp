/* HD opening message pane lookup. */
#include "d/hd_screen_opening_text.h"
static u32 sOpeningTextStringVtable;
static u32 sMessagePaneReady;
static hdUiSafeString sMessagePaneName;
static u32 sMessagePaneRegistration[3] = {0, 0, 0};

/* 026BEF88 */
void hdOpening_screen_c::findMessagePane() {
    if (!sMessagePaneReady) {
        sMessagePaneReady = 1;
        sMessagePaneName.mVtable.v = gabi::ea(&sOpeningTextStringVtable);
        openingSetString(sMessagePaneName, "T_Msg_00");
        openingRegisterMessagePane(sMessagePaneRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    u32 getText = sMessagePaneName.mVtable->mAssureTermination.get();
    hdOpening_paneVtable_l* paneVtable = hdPartsView<hdOpening_paneVtable_l>(root->mVtable.v);
    gabi::call_ptr<void>(getText, &sMessagePaneName);
    hdOpening_textBox_l* textBox = gabi::call_ptr<hdOpening_textBox_l*>(paneVtable->mFindPane, root, sMessagePaneName.mText.get(), 1);
    mpTextBox = textBox;
    openingSetTextCapacity(textBox, 0x100);
}

/* WWHD glue (tools/verify/autoglue.py): opening text pane
 data sOpeningTextStringVtable 100FF91C nocheck
 data sMessagePaneReady 10499038 nocheck
 data sMessagePaneName 10498ED0 nocheck
 data sMessagePaneRegistration 101F67BC nocheck
 str 100FFB00 "T_Msg_00"
 ext 028F026C void openingRegisterMessagePane(void*)
 ext 02704AFC void openingSetTextCapacity(void*, u32)
*/
