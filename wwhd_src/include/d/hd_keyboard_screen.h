// HD keyboard text panes and cursor screens. Private binary-derived views.
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_parts_text_pane.h"
struct hdKeyboardStringTriple_c {
    hdUiSafeString mItems[3];
};
struct hdKeyboardStringPair_c {
    hdUiSafeString mItems[2];
};
struct hdKeyboardStringSingle_c {
    hdUiSafeString mItems[1];
};
struct hdKeyboardAnimationGroups_c {
    be<u32> mItems[2];
};
// Additional library Pane fields beyond the merged shared base view.
struct hdKeyboardPaneVtable_l : hdPartsPaneVtbl_c {
    u8 mReserved18[0x5C - 0x18];
    gfn mFindPane;
};
struct hdKeyboardPane_l : hdPartsPane_c {
    be<u32> mPosition[3];
    u8 mReserved28[0x1C];
    be<u8> mFlags;
};
// The merged hdPartsLyt_c leaves this text-resource slot in its opaque +10..+1F region.
struct hdKeyboardTextResourceView_l {
    u8 mReserved[0x1C];
    gptr<void> mTextResource;
};
struct hdKeyboardTextArea_c : hdParts_c {
    be<u32> mTextPaneVtable;
    gptr<hdTextPane_c> mTextPanes[3];
    void destroy(u32);
    void enterIn();
    void enterOut();
    void startIn();
    void startOut();
    void findTextPanes();
    u32 create(u32, u32, u32);
    void update();
    void updateSubText();
    void setAnswer(const be<u16>*);
    void setQuestion(u32);
    void setQuestionVisible(u32);
    u32 isAnimationFinished();
    u32 createLayout(u32, u32, u32, u32, u32);
};
struct hdKeyboardCursor_c : hdParts_c {
    void destroy(u32);
    void enterLoop();
    void startLoop();
    u32 create(u32, u32, u32);
    void update();
    void executeState();
    void setPosition(const be<u32>*);
    u32 isAnimationFinished();
    u32 createLayout(u32, u32, u32, u32, u32);
};
struct hdKeyboardKeyIcon_c {
    hdPartsKeyIcon_c mBase;
    void destroy(u32);
    u32 create(u32, u32, u32);
};
struct hdKeyboardFlag_c {
    be<u8> mValue;
};
struct hdKeyboardStatWords_c {
    be<u32> mValues[4];
};
struct hdKeyboardStatRange_c {
    be<f32> mMinimum, mMaximum;
    be<u8> mFlagA, mFlagB;
    u8 mReserved[2];
    hdKeyboardStatWords_c mWords;
};
struct hdKeyboardRegistration_c {
    be<u32> mWords[3];
};
struct hdKeyboardStatRegistrations_c {
    hdKeyboardRegistration_c mEntries[3];
};
struct hdKeyboardStatLimits_c {
    be<f32> mMinimum, mMaximum;
};
struct hdKeyboardWideBuffer_l : PaneString_l {
    be<u16> mBuffer[256];
};
WWHD_OFFSET(hdKeyboardPane_l, mVtable, 8);
WWHD_OFFSET(hdKeyboardPane_l, mPosition, 0x1C);
WWHD_OFFSET(hdKeyboardPane_l, mFlags, 0x44);
WWHD_OFFSET(hdKeyboardTextResourceView_l, mTextResource, 0x1C);
WWHD_OFFSET(hdKeyboardPaneVtable_l, mFindPane, 0x5C);
WWHD_OFFSET(hdKeyboardTextArea_c, mTextPaneVtable, 0x50);
WWHD_OFFSET(hdKeyboardTextArea_c, mTextPanes, 0x54);
WWHD_SIZE(hdKeyboardTextArea_c, 0x60);
WWHD_SIZE(hdKeyboardCursor_c, 0x50);
WWHD_SIZE(hdKeyboardKeyIcon_c, 0x58);
WWHD_OFFSET(hdKeyboardStatRange_c, mFlagA, 8);
WWHD_OFFSET(hdKeyboardStatRange_c, mFlagB, 9);
WWHD_OFFSET(hdKeyboardStatRange_c, mWords, 0xC);
WWHD_OFFSET(hdKeyboardKeyIcon_c, mBase, 0);
WWHD_OFFSET(hdKeyboardStatLimits_c, mMinimum, 0);
WWHD_OFFSET(hdKeyboardStatLimits_c, mMaximum, 4);
WWHD_OFFSET(hdKeyboardStatRange_c, mMinimum, 0);
WWHD_OFFSET(hdKeyboardStatRange_c, mMaximum, 4);
WWHD_OFFSET(hdKeyboardStatWords_c, mValues, 0);
WWHD_OFFSET(hdKeyboardStatRegistrations_c, mEntries, 0);
WWHD_OFFSET(hdKeyboardRegistration_c, mWords, 0);
WWHD_SIZE(hdKeyboardRegistration_c, 0xC);
WWHD_OFFSET(hdKeyboardWideBuffer_l, mpBuffer, 0);
WWHD_OFFSET(hdKeyboardWideBuffer_l, mpVTable, 4);
WWHD_OFFSET(hdKeyboardWideBuffer_l, mCapacity, 8);
WWHD_OFFSET(hdKeyboardWideBuffer_l, mBuffer, 0xC);
WWHD_SIZE(hdKeyboardWideBuffer_l, 0x20C);
void* keyboardAllocate(u32);
void* keyboardAllocateAligned(u32, void*, u32);
void keyboardFree(void*);
void keyboardLoopConstruct(void*);
void keyboardLoopDestroy(void*, u32);
void keyboardLoopRemove(void*);
void keyboardSetLayoutName(void*, hdUiSafeString*, u32);
void keyboardExecuteState(hdStateMachine*);
u32 keyboardLayoutFinished(hdPartsLayout_c*);
hdPartsLayout_c* keyboardConstructLayout(hdPartsLayout_c*);
void keyboardBindAnimation(void*, u32, hdUiSafeString*, hdUiSafeString*);
void keyboardSetAnimationFrame(void*, u32, u32, f32);
// resetMessage forwards incoming r4/r5 to the message formatter. Preserve both SafeString contexts.
void keyboardResetPaneMessageWithContext(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*);
void keyboardSetPaneWideText(hdTextPane_c*, PaneString_l*, PaneWork_l*);
void keyboardFormatWide(hdKeyboardWideBuffer_l*, const char*, const be<u16>*);
void keyboardRegister(hdKeyboardRegistration_c*);
void keyboardFlagA(be<u8>*);
void keyboardFlagB(be<u8>*);
inline void initializeKeyboardStatics(hdKeyboardStatRange_c* range,
                                      hdKeyboardStatRegistrations_c* regs,
                                      const hdKeyboardStatLimits_c* limits) {
    range->mWords.mValues[2] = 0;
    range->mWords.mValues[0] = 0;
    range->mWords.mValues[3] = 0;
    range->mWords.mValues[1] = 0;
    keyboardRegister(&regs->mEntries[0]);
    range->mMinimum = limits->mMinimum;
    range->mMaximum = limits->mMaximum;
    keyboardFlagA(&range->mFlagA);
    keyboardRegister(&regs->mEntries[1]);
    keyboardFlagB(&range->mFlagB);
    keyboardRegister(&regs->mEntries[2]);
}
inline void initializeKeyboardState(hdPartsStateId_c* state, u32 id, const char* name, u32 vtable,
                                    u16 firstIndex, hdStateId* nullState) {
    state->mNumber = id;
    state->mName.v = gabi::guest_str(name);
    state->mVtable.v = vtable;
    state->mEnter.setVirtual(firstIndex);
    state->mExecute.setVirtual(firstIndex + 1);
    state->mExit.setVirtual(firstIndex + 2);
    state->mUnused.clear();
    state->mpNullStateId = nullState;
}

/* 02674164 (hd_keyboard_key_icon.cpp): the key icon's constructor (allocates 0x58 when self is NULL); the message
 * event factory builds keyboard child 15 with it (append-only) */
hdKeyboardKeyIcon_c* constructKeyboardKeyIcon(hdKeyboardKeyIcon_c* self);
