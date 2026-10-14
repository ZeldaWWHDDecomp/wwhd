/* HD sequence-window text-pane lookup; separate original translation unit.
 * */
#include "d/hd_sequence_window.h"
extern void sequenceRegisterTextNames(hdSequenceRegistration_c*);
static hdSequenceTextPaneNameTable_c sSequenceTextNames;
static hdSequenceGuard_c sSequenceTextNamesGuard;
static hdSequenceRegistration_c sSequenceTextNamesRegistration;
static hdUiStringTable sSequenceTextStringVtable;

/* 026DB284 */
void hdSequenceWindow_c::findTextPanes() {
    if (!sSequenceTextNamesGuard.mInitialized) {
        sSequenceTextNamesGuard.mInitialized = 1;
        hdParts_setSafeString(&sSequenceTextNames.mNames[0], &sSequenceTextStringVtable, "T_Msg_00");
        hdParts_setSafeString(&sSequenceTextNames.mNames[1], &sSequenceTextStringVtable, "T_Msg_01");
        hdParts_setSafeString(&sSequenceTextNames.mNames[2], &sSequenceTextStringVtable, "T_AreaName_00");
        sequenceRegisterTextNames(&sSequenceTextNamesRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
        hdUiSafeString* name = &sSequenceTextNames.mNames[i];
        hdSequencePaneLookupTable_l* table = reinterpret_cast<hdSequencePaneLookupTable_l*>(root->mVtable.get());
        name->assureTermination();
        u32 function = table->mFindPane.get();
        mpTextPanes[i] = gabi::call_ptr<hdSequencePane_c*>(function, root, name->mText.get(), 1);
    }
    reinterpret_cast<hdTextPane_c*>(mpTextPanes[0].get())->setCharacterCapacity(100);
}

/* WWHD glue (tools/verify/autoglue.py)
data sSequenceTextNames 1049B000
data sSequenceTextNamesGuard 1049B41C
data sSequenceTextNamesRegistration 101F6AB4
data sSequenceTextStringVtable 1010394C
str 10104038 "T_Msg_00"
str 10104044 "T_Msg_01"
str 10104028 "T_AreaName_00"
ext 028F026C void sequenceRegisterTextNames(hdSequenceRegistration_c*)
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
*/
