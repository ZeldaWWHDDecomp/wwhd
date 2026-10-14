// HD keyboard textarea layout, question alternatives and state descriptors. Private source.
#include "d/hd_input_ctrl.h"
#include "d/hd_keyboard_screen.h"
static u32 sTextAreaSafeStringVtable;
static u32 sTextAreaWideStringVtable;
static u32 sTextAreaStateVtable;
static hdUiSafeString sTextAreaPartName;
static hdUiSafeString sTextAreaLayoutName;
static hdKeyboardStringPair_c sTextAreaGroups;
static hdKeyboardStringPair_c sTextAreaAnimations;
static hdKeyboardStringPair_c sQuestionTexts;
static hdKeyboardStringPair_c sSubTexts;
static const hdKeyboardAnimationGroups_c sTextAreaAnimationGroups;
static hdKeyboardStatWords_c sTextAreaWords;
static hdKeyboardStatLimits_c sTextAreaRange;
static const hdKeyboardStatLimits_c sTextAreaLimits;
static hdKeyboardFlag_c sTextAreaFlagA;
static hdKeyboardFlag_c sTextAreaFlagB;
static hdKeyboardStatRegistrations_c sTextAreaRegistrations;
static hdPartsStateId_c sTextAreaIn;
static hdPartsStateId_c sTextAreaOut;
static hdPartsStateId_c sTextAreaInitialBlank;
static u32 sKeyboardStateGuard;
static u32 sKeyboardNextStateId;
static hdStateId sKeyboardNullState;
static hdScreenUiSystem* sKeyboardHeap;
static hdInputManager* sKeyboardInput;
static const char* sQuestionPaneName;
extern u32 keyboardIsGamePadMode(hdInputManager*);
/* 026730B8 */
u32 hdKeyboardTextArea_c::create(u32 parent, u32 archive, u32 layout) {
    keyboardSetLayoutName(this, &sTextAreaPartName, 1);
    u32 result = gabi::call_ptr<u32>(mVtable->mLoad.get(), this, &sTextAreaLayoutName,
                                     mpResAccessor.get(), parent, archive, layout);
    if (!result)
        return 0;
    findTextPanes();
    gabi::Local<hdUiSafeString> context;
    context->mText = reinterpret_cast<be<u8>*>(
        reinterpret_cast<hdKeyboardTextResourceView_l*>(mpLayout->lyt())
            ->mTextResource.get());
    context->mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    keyboardResetPaneMessageWithContext(mTextPanes[0], context.get(), &sQuestionTexts.mItems[0]);
    hdStateOwner* state = mStateHolder.mMachine.mpOwner;
    hdStateObject* action = gabi::call_ptr<hdStateObject*>(state->mpVTable->mCreateState.get(),
                                                           state, &sTextAreaInitialBlank);
    mStateHolder.mMachine.mpCurrentState = action;
    gabi::call_ptr<void>(action->mpVTable->mEnter.get(), action);
    return 1;
}
/* 026731D8 */
void hdKeyboardTextArea_c::update() {
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    keyboardLoopRemove(this);
}
/* 02673218 */
void hdKeyboardTextArea_c::updateSubText() {
    keyboardExecuteState(&mStateHolder.mMachine);
    u32 gamePad = keyboardIsGamePadMode(sKeyboardInput);
    gabi::Local<hdUiSafeString> context;
    context->mText = reinterpret_cast<be<u8>*>(
        reinterpret_cast<hdKeyboardTextResourceView_l*>(mpLayout->lyt())
            ->mTextResource.get());
    context->mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    keyboardResetPaneMessageWithContext(mTextPanes[2], context.get(),
                                        &sSubTexts.mItems[gamePad ? 0 : 1]);
}
/* 026732B8 */
void hdKeyboardTextArea_c::setAnswer(const be<u16>* text) {
    if (!mTextPanes[1] || !text)
        return;
    gabi::Local<hdKeyboardWideBuffer_l> buffer;
    buffer->mpBuffer = buffer->mBuffer;
    buffer->mpVTable.v = gabi::ea(&sTextAreaWideStringVtable);
    buffer->mCapacity = 256;
    buffer->mBuffer[0] = 0;
    buffer->mBuffer[255] = 0;
    // The original UTF-16 format is L"%s"; the glue alias preserves its address.
    keyboardFormatWide(buffer.get(), "keyboard-wide-format", text);
    keyboardSetPaneWideText(mTextPanes[1], buffer.get(), nullptr);
}
/* 02673344 */
void hdKeyboardTextArea_c::setQuestion(u32 index) {
    if (!mTextPanes[0] || index > 1)
        return;
    gabi::Local<hdUiSafeString> context;
    context->mText = reinterpret_cast<be<u8>*>(
        reinterpret_cast<hdKeyboardTextResourceView_l*>(mpLayout->lyt())
            ->mTextResource.get());
    context->mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    keyboardResetPaneMessageWithContext(mTextPanes[0], context.get(),
                                        &sQuestionTexts.mItems[index]);
}
/* 026733AC */
void hdKeyboardTextArea_c::setQuestionVisible(u32 visible) {
    hdKeyboardPane_l* root = reinterpret_cast<hdKeyboardPane_l*>(mpLayout->lyt()->mpRootPane.get());
    const char* paneName = sQuestionPaneName;
    hdKeyboardPane_l* pane = gabi::call_ptr<hdKeyboardPane_l*>(
        reinterpret_cast<hdKeyboardPaneVtable_l*>(root->mVtable.get())->mFindPane.get(), root,
        paneName, 1);
    if (pane)
        pane->mFlags = (pane->mFlags.get() & 0xFE) | visible;
}
/* 02673414 */
u32 hdKeyboardTextArea_c::isAnimationFinished() {
    return keyboardLayoutFinished(mpLayout);
}
/* 026734C8 */
u32 hdKeyboardTextArea_c::createLayout(u32 resource, u32 parent, u32 archive, u32 pane, u32 flags) {
    hdPartsLayout_c* layout =
        static_cast<hdPartsLayout_c*>(keyboardAllocateAligned(0x110, sKeyboardHeap->mHeap, 4));
    if (layout)
        layout = keyboardConstructLayout(layout);
    mpLayout = layout;
    if (!layout)
        return 0;
    u32 result = gabi::call_ptr<u32>(layout->vtbl()->mLoad.get(), layout, resource, parent,
                                     archive, pane, 2, 1, flags);
    if (!result)
        return 0;
    for (u32 i = 0; i < 2; i++) {
        keyboardBindAnimation(mpLayout->animSet(), i, &sTextAreaAnimations.mItems[i],
                              &sTextAreaGroups.mItems[sTextAreaAnimationGroups.mItems[i].get()]);
    }
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}
/* 026735F4 */
static void initializeKeyboardTextArea() {
    sTextAreaWords.mValues[2] = 0;
    sTextAreaWords.mValues[0] = 0;
    sTextAreaWords.mValues[3] = 0;
    sTextAreaWords.mValues[1] = 0;
    keyboardRegister(&sTextAreaRegistrations.mEntries[0]);
    sTextAreaRange.mMinimum = sTextAreaLimits.mMinimum;
    sTextAreaRange.mMaximum = sTextAreaLimits.mMaximum;
    keyboardFlagA(&sTextAreaFlagA.mValue);
    keyboardRegister(&sTextAreaRegistrations.mEntries[1]);
    keyboardFlagB(&sTextAreaFlagB.mValue);
    keyboardRegister(&sTextAreaRegistrations.mEntries[2]);
    sTextAreaAnimations.mItems[0].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaAnimations.mItems[0].mText.v = gabi::guest_str("InAll");
    sTextAreaAnimations.mItems[1].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaAnimations.mItems[1].mText.v = gabi::guest_str("OutAll");
    sTextAreaGroups.mItems[0].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaGroups.mItems[0].mText.v = gabi::guest_str("G_All_00");
    sTextAreaGroups.mItems[1].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaGroups.mItems[1].mText.v = gabi::guest_str("KeyboardTextArea_00");
    sTextAreaPartName.mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaPartName.mText.v = gabi::guest_str("KeyboardTextArea_00");
    sTextAreaLayoutName.mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sTextAreaLayoutName.mText.v = gabi::guest_str("KeyboardTextArea_00");
    sQuestionTexts.mItems[0].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sQuestionTexts.mItems[0].mText.v = gabi::guest_str("T_Question_00_01");
    sQuestionTexts.mItems[1].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sQuestionTexts.mItems[1].mText.v = gabi::guest_str("T_Question_00_02");
    sSubTexts.mItems[0].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sSubTexts.mItems[0].mText.v = gabi::guest_str("T_Sub_00_00");
    sSubTexts.mItems[1].mVtable.v = gabi::ea(&sTextAreaSafeStringVtable);
    sSubTexts.mItems[1].mText.v = gabi::guest_str("T_Sub_00_01");
    u32 previous;
    if (sKeyboardStateGuard)
        previous = sKeyboardNextStateId;
    else {
        sKeyboardStateGuard = 1;
        previous = 0;
    }
    initializeKeyboardState(&sTextAreaIn, previous + 1, "::StateID_InAll", gabi::ea(&sTextAreaStateVtable),
                            0x19, &sKeyboardNullState);
    initializeKeyboardState(&sTextAreaOut, previous + 2, "::StateID_OutAll", gabi::ea(&sTextAreaStateVtable),
                            0x1C, &sKeyboardNullState);
    sKeyboardNextStateId = previous + 2;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sTextAreaSafeStringVtable 100F68EC nocheck
 data sTextAreaWideStringVtable 100F6934 nocheck
 data sTextAreaStateVtable 100F6994 nocheck
 data sTextAreaPartName 10494E84
 data sTextAreaLayoutName 10494F54
 data sTextAreaGroups 10494E7C
 data sTextAreaAnimations 10494EC0
 data sQuestionTexts 10494ED0
 data sSubTexts 10494EE0
 data sTextAreaAnimationGroups 100F68E0
 data sTextAreaWords 10494EB0
 data sTextAreaRange 10494E8C
 data sTextAreaLimits 100F6A28
 data sTextAreaFlagA 10494E94
 data sTextAreaFlagB 10494E95
 data sTextAreaRegistrations 101F60E0
 data sTextAreaIn 10494EF4
 data sTextAreaOut 10494F24
 data sTextAreaInitialBlank 1049DD90
 data sKeyboardStateGuard 101FD8E4
 data sKeyboardNextStateId 101FDD50
 data sKeyboardNullState 101FF32C
 data sKeyboardHeap 1018C404
 data sKeyboardInput 101F5088
 data sQuestionPaneName 101F60DC
 str 100F6A0C "keyboard-wide-format" renamed
 str 100F6A30 "InAll"
 str 100F6A38 "OutAll"
 str 100F6A7C "G_All_00"
 str 100F6A50 "KeyboardTextArea_00"
 str 100F6A88 "T_Question_00_01"
 str 100F6A9C "T_Question_00_02"
 str 100F6A64 "T_Sub_00_00"
 str 100F6A70 "T_Sub_00_01"
 str 100F6A40 "::StateID_InAll"
 str 100F6AB0 "::StateID_OutAll"
 ext 02618498 u32 keyboardIsGamePadMode(hdInputManager*)
*/
