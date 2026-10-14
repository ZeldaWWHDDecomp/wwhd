// HD keyboard's question, answer and subordinate text panes. Private source.
#include "d/hd_keyboard_screen.h"
static u32 sTextAreaVtable;
static u32 sTextAreaDelegateA;
static u32 sTextAreaDelegateB;
static u32 sTextPaneVtable;
static u32 sTextStringVtable;
static hdKeyboardStringTriple_c sTextPaneNames;
static u32 sTextPaneNamesInitialized;
static hdKeyboardRegistration_c sTextPaneNameRegistration;
static hdScreenType sTextBoxType;
static hdScreenType sPaneType;
static hdScreenType sDrawableType;
static hdScreenType sBaseType;
static u32 sTextBoxTypeInitialized;
static u32 sPaneTypeInitialized;
static u32 sDrawableTypeInitialized;
static u32 sBaseTypeInitialized;
static f32 sTextAnimationInitialFrame;
using KeyboardArrayDestructor = void (*)(void*);
extern void keyboardStringArrayDestructor(void*);
extern void keyboardDestroyStringArray(void*, u32, u32, KeyboardArrayDestructor, u32, u32);
extern void keyboardTextPaneEnter(void*);
extern void keyboardTextPaneLeave(void*);
/* 02672B94 */
static hdScreenType* keyboardTextBaseType(hdScreenType* type) {
    return type->mParent;
}
/* 02672C94 */
static void destroyKeyboardTextPaneNames() {
    keyboardDestroyStringArray(&sTextPaneNames, 3, sizeof(hdUiSafeString),
                               keyboardStringArrayDestructor, 0, 0);
}
/* 02672CB8 */
void hdKeyboardTextArea_c::destroy(u32 flags) {
    if (!this)
        return;
    mStateHolder.mHolderVtable.v = gabi::ea(&sTextAreaDelegateA);
    mSecondaryVtable.v = gabi::ea(&sTextAreaDelegateB);
    mTextPaneVtable = gabi::ea(&sTextPaneVtable);
    keyboardLoopDestroy(this, 0);
    if (flags & 1)
        keyboardFree(this);
}
/* 02672D30 */
static void keyboardTextPaneEnterThunk(void* subobject) {
    keyboardTextPaneEnter(static_cast<u8*>(subobject) - 0x50);
}
/* 02672D38 */
static void keyboardTextPaneLeaveThunk(void* subobject) {
    keyboardTextPaneLeave(static_cast<u8*>(subobject) - 0x50);
}
/* 02672D40 */
void hdKeyboardTextArea_c::enterIn() {
    keyboardSetAnimationFrame(mpLayout->animSet(), 0, 0, sTextAnimationInitialFrame);
}
/* 02672D5C */
void hdKeyboardTextArea_c::startIn() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02672D6C */
void hdKeyboardTextArea_c::enterOut() {
    keyboardSetAnimationFrame(mpLayout->animSet(), 1, 0, sTextAnimationInitialFrame);
}
/* 02672D88 */
void hdKeyboardTextArea_c::startOut() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02672D98 */
static hdKeyboardTextArea_c* constructKeyboardTextArea(hdKeyboardTextArea_c* self) {
    if (!self) {
        self = static_cast<hdKeyboardTextArea_c*>(keyboardAllocate(sizeof(hdKeyboardTextArea_c)));
        if (!self)
            return NULL;
    }
    keyboardLoopConstruct(self);
    self->mSecondaryVtable.v = gabi::ea(&sTextAreaDelegateB);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sTextAreaDelegateA);
    self->mVtable.v = gabi::ea(&sTextAreaVtable);
    self->mTextPaneVtable = gabi::ea(&sTextPaneVtable);
    // Preserve the original 32-bit embedded-subobject wrap check.
    if (u32(gabi::ea(self) + offsetof(hdKeyboardTextArea_c, mTextPanes)) == 0)
        keyboardAllocate(12);
    return self;
}
static hdScreenType* keyboardTextBoxType() {
    if (!sTextBoxTypeInitialized) {
        u32 paneReady = sPaneTypeInitialized;
        sTextBoxTypeInitialized = 1;
        if (!paneReady) {
            u32 drawableReady = sDrawableTypeInitialized;
            sPaneTypeInitialized = 1;
            if (!drawableReady) {
                u32 baseReady = sBaseTypeInitialized;
                sDrawableTypeInitialized = 1;
                if (!baseReady) {
                    sBaseType.mParent = NULL;
                    sBaseTypeInitialized = 1;
                }
                sDrawableType.mParent = &sBaseType;
            }
            sPaneType.mParent = &sDrawableType;
        }
        sTextBoxType.mParent = &sPaneType;
    }
    return &sTextBoxType;
}
/* 02672E20 */
void hdKeyboardTextArea_c::findTextPanes() {
    if (!sTextPaneNamesInitialized) {
        sTextPaneNamesInitialized = 1;
        sTextPaneNames.mItems[0].mVtable.v = gabi::ea(&sTextStringVtable);
        sTextPaneNames.mItems[0].mText.v = gabi::guest_str("T_Question_00");
        sTextPaneNames.mItems[1].mVtable.v = gabi::ea(&sTextStringVtable);
        sTextPaneNames.mItems[1].mText.v = gabi::guest_str("T_Answer_00");
        sTextPaneNames.mItems[2].mVtable.v = gabi::ea(&sTextStringVtable);
        sTextPaneNames.mItems[2].mText.v = gabi::guest_str("T_Sub_00");
        keyboardRegister(&sTextPaneNameRegistration);
    }
    hdKeyboardPane_l* root = reinterpret_cast<hdKeyboardPane_l*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i < 3; i++) {
        hdUiSafeString* name = &sTextPaneNames.mItems[i];
        u32 assure = name->mVtable->mAssureTermination.get();
        hdKeyboardPaneVtable_l* rootVtable =
            reinterpret_cast<hdKeyboardPaneVtable_l*>(root->mVtable.get());
        gabi::call_ptr<void>(assure, name);
        u32 find = rootVtable->mFindPane.get();
        hdKeyboardPane_l* pane =
            gabi::call_ptr<hdKeyboardPane_l*>(find, root, name->mText.get(), 1, find);
        hdScreenType* textType = keyboardTextBoxType();
        bool isText = false;
        if (pane) {
            hdScreenType* type =
                gabi::call_ptr<hdScreenType*>(pane->mVtable->mGetRuntimeTypeInfo.get(), pane);
            while (type) {
                if (type == textType) {
                    isText = true;
                    break;
                }
                type = type->mParent;
            }
        }
        mTextPanes[i] = isText ? reinterpret_cast<hdTextPane_c*>(pane) : nullptr;
    }
}
/* WWHD glue (tools/verify/autoglue.py)
 data sTextAreaVtable 100F6AC8 nocheck
 data sTextAreaDelegateA 100F6BD0 nocheck
 data sTextAreaDelegateB 100F6BE0 nocheck
 data sTextPaneVtable 100F6BF0 nocheck
 data sTextStringVtable 100F68EC nocheck
 data sTextPaneNames 10494E98
 data sTextPaneNamesInitialized 10494EF0
 data sTextPaneNameRegistration 101F60D0
 data sTextBoxType 101FDD3C
 data sTextBoxTypeInitialized 101FDD40
 data sPaneType 101FD8F4
 data sPaneTypeInitialized 101FD8F0
 data sDrawableType 101FD8F8
 data sDrawableTypeInitialized 101FD89C
 data sBaseType 101FD8FC
 data sBaseTypeInitialized 101FD888
 data sTextAnimationInitialFrame 100F69E0
 str 100F69F0 "T_Question_00"
 str 100F69E4 "T_Answer_00"
 str 100F6A00 "T_Sub_00"
 ext 0267391C void keyboardStringArrayDestructor(void*) callback
 ext 028F0164 void keyboardDestroyStringArray(void*,u32,u32,KeyboardArrayDestructor,u32,u32)
 ext 02673930 void keyboardTextPaneEnter(void*)
 ext 02673934 void keyboardTextPaneLeave(void*)
 ext 026F88F0 void keyboardLoopDestroy(void*,u32)
 ext 026F89E8 void keyboardLoopConstruct(void*)
 ext 020053E4 void keyboardSetAnimationFrame(void*,u32,u32,f32)
 ext 026F91F8 void keyboardLoopRemove(void*)
 ext 026F90D8 void keyboardSetLayoutName(void*,hdUiSafeString*,u32)
 ext 02006364 void keyboardExecuteState(hdStateMachine*)
 ext 020029EC u32 keyboardLayoutFinished(hdPartsLayout_c*)
 ext 0273B050 void* keyboardAllocateAligned(u32,void*,u32)
 ext 026FADB0 hdPartsLayout_c* keyboardConstructLayout(hdPartsLayout_c*)
 ext 02004E04 void keyboardBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 027045B0 void keyboardResetPaneMessageWithContext(hdTextPane_c*,hdUiSafeString*,hdUiSafeString*)
 ext 027046D4 void keyboardSetPaneWideText(hdTextPane_c*,PaneString_l*,PaneWork_l*)
 ext 02759D3C void keyboardFormatWide(hdKeyboardWideBuffer_l*,const char*,const be<u16>*)
*/
