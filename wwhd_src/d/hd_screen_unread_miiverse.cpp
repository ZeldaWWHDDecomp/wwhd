/** HD notification count display, suppressed during messages and selected play modes. */
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_unread_miiverse.h"
static u32 sUnreadStateInterfaceVtable;
static u32 sUnreadDrawInterfaceVtable;
static hdPartsVtbl_c sUnreadVtable;
static hdPartsStateId_c sUnreadWaitState;
static hdPartsStateId_c sUnreadInState;
static hdPartsStateId_c sUnreadDisplayState;
static hdPartsStateId_c sUnreadOutState;
static u8 sUnreadEnabled;
typedef gptr<void> hdUnreadObjectPointer_l;
static hdUnreadObjectPointer_l sUnreadManager;
static hdUnreadObjectPointer_l sUnreadMessage;
struct hdUnreadPlay_l { u8 mPrefix[0x5CD8]; be<u32> mDisplayFlags; };
WWHD_OFFSET(hdUnreadPlay_l, mDisplayFlags, 0x5CD8);
struct hdUnreadNumberText_l {
    gptr<be<u16>> mText; gptr<void> mVtable; be<u32> mCapacity; be<u16> mBuffer[3];
};
WWHD_OFFSET(hdUnreadNumberText_l, mText, 0);
WWHD_OFFSET(hdUnreadNumberText_l, mVtable, 4);
WWHD_OFFSET(hdUnreadNumberText_l, mCapacity, 8);
static u32 sUnreadNumberVtable;
struct UnreadNumberFormatView_l {
    be<u16> mValues[4];
    be<u16>& operator[](u32 index) { return mValues[index]; }
};
static UnreadNumberFormatView_l sUnreadNumberFormat;
hdUnreadPlay_l* hdUnread_getPlay();
u32 hdUnread_getCount(void*);
u32 hdUnread_messageActive(void*);
void* hdUnread_allocate(u32);
void hdUnread_delete(void*);
void hdUnread_baseConstruct(NoControlPart_l*);
void hdUnread_baseDestroy(NoControlPart_l*, u32);



u32 hdUnread_animationEnded(hdAnimSet*, u32);
void hdUnread_format(hdUnreadNumberText_l*, const be<u16>*, s32);
static inline void hdUnread_setPaneText(void* pane, hdUnreadNumberText_l* text, u32 work) {
    reinterpret_cast<hdTextPane_c*>(pane)->setStringPrepared(reinterpret_cast<PaneString_l*>(text), work ? hdPartsView<PaneWork_l>(work) : nullptr);
}
/* 026EE138 */
void hdUnreadMiiverse_c::destroy(hdUnreadMiiverse_c* screen, u32 flags) {
    if (!screen)
        return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sUnreadStateInterfaceVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sUnreadDrawInterfaceVtable);
    hdUnread_baseDestroy(screen, 0);
    if (flags & 1)
        hdUnread_delete(screen);
}
/* 026EE1A4 */
void hdUnreadMiiverse_c::resetAnimation() {
    hdAnimSet_setFrame(mpLayout->animSet(), 0, 0, 0.0f);
}
/* 026EE1C0 */
BOOL hdUnreadMiiverse_c::canDisplay() {
    u32 count = hdUnread_getCount(sUnreadManager);
    if (!sUnreadEnabled)
        return FALSE;
    if (u32(hdUnread_getPlay()->mDisplayFlags) & 0x10)
        return FALSE;
    if (hdUnread_messageActive(sUnreadMessage))
        return FALSE;
    return count != 0;
}
/* 026EE24C */
void hdUnreadMiiverse_c::waitExecute() {
    if (canDisplay()) {
        mUnreadCount = hdUnread_getCount(sUnreadManager);
        hdParts_changeState(stateMachine(), &sUnreadInState);
    }
}
/* 026EE2A0 */
void hdUnreadMiiverse_c::updateText() {
    hdUnreadNumberText_l text;
    text.mBuffer[2] = 0;
    text.mCapacity = 3;
    text.mText = &text.mBuffer[0];
    text.mBuffer[0] = 0;
    s32 count = mUnreadCount;
    text.mVtable.v = gabi::ea(&sUnreadNumberVtable);
    if (count >= 100)
        count = 99;
    hdUnread_format(&text, sUnreadNumberFormat.mValues, count);
    hdUnread_setPaneText(mpNumberPane, &text, 0);
}
/* 026EE338 */
void hdUnreadMiiverse_c::inEnter() {
    hdAnimSet_play(mpLayout->animSet(), 0, 0, 1.0f);
    updateText();
}
/* 026EE384 */
void hdUnreadMiiverse_c::inExecute() {
    if (hdUnread_animationEnded(mpLayout->animSet(), 0))
        hdParts_changeState(stateMachine(), &sUnreadDisplayState);
}
/* 026EE3D4 */
void hdUnreadMiiverse_c::displayExecute() {
    u32 count = hdUnread_getCount(sUnreadManager);
    if (u32(mUnreadCount) != count) {
        mUnreadCount = count;
        if (count)
            updateText();
    }
    if (!canDisplay())
        hdParts_changeState(stateMachine(), &sUnreadOutState);
}
/* 026EE448 */
void hdUnreadMiiverse_c::outEnter() {
    hdAnimSet_play(mpLayout->animSet(), 1, 0, 1.0f);
}
/* 026EE464 */
void hdUnreadMiiverse_c::outExecute() {
    if (hdUnread_animationEnded(mpLayout->animSet(), 1))
        hdParts_changeState(stateMachine(), &sUnreadWaitState);
}
/* 026EE4B4 */
hdUnreadMiiverse_c* hdUnreadMiiverse_c::construct(hdUnreadMiiverse_c* screen) {
    if (!screen)
        screen = static_cast<hdUnreadMiiverse_c*>(hdUnread_allocate(sizeof(hdUnreadMiiverse_c)));
    if (screen) {
        hdUnread_baseConstruct(screen);
        screen->mStateHolder.mHolderVtable.v = gabi::ea(&sUnreadStateInterfaceVtable);
        screen->mVtable = &sUnreadVtable;
        screen->mSecondaryVtable.v = gabi::ea(&sUnreadDrawInterfaceVtable);
        screen->mUnreadCount = 0;
        screen->mpNumberPane = NULL;
    }
    return screen;
}
/* 026EE658 */
void hdUnreadMiiverse_c::resetState() {
    hdParts_changeState(stateMachine(), &sUnreadWaitState);
}
typedef gptr<hdScreenUiSystem> hdUnreadUiSystemPointer_l;
static hdUnreadUiSystemPointer_l sUnreadUiSystem;
struct UnreadGroupNamesView_l {
    hdUiSafeString mValues[1];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static UnreadGroupNamesView_l sUnreadGroupNames;
struct UnreadAnimationNamesView_l {
    hdUiSafeString mValues[2];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static UnreadAnimationNamesView_l sUnreadAnimationNames;
struct UnreadGroupIndicesView_l {
    be<u32> mValues[2];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static UnreadGroupIndicesView_l sUnreadGroupIndices;
void* hdUnread_allocateLayout(u32,hdScreenHeap*,u32);

void hdUnread_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 026EE668 */
BOOL hdUnreadMiiverse_c::loadLayout(void* name,u32 arg1,u32 arg2,u32 arg3,void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdUnread_allocateLayout(sizeof(hdPartsLayout_c),sUnreadUiSystem->mHeap,4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return FALSE;
    if (!layout->vLoad(name,arg1,arg2,arg3,2,1,desc)) return FALSE;
    for (u32 i=0; i<2; ++i) {
        u32 groupAddress = gabi::ea(&sUnreadGroupNames) + u32(sUnreadGroupIndices[i]) * sizeof(hdUiSafeString);
        hdUnread_registerAnimation(mpLayout->animSet(),i,&sUnreadAnimationNames[i],hdPartsView<hdUiSafeString>(groupAddress));
    }
    return TRUE;
}
struct hdUnreadFindPaneVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdUnreadFindPaneVtbl_l, mFindPane, 0x5C);
static hdUiSafeString sUnreadPaneName;
static hdUiSafeString sUnreadCreateDescriptor;
u32 hdUnread_baseCreate(NoControlPart_l*,void*,u32,u32,u32);
/* 026EE52C */
void hdUnreadMiiverse_c::findNumberPane() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    hdUnreadFindPaneVtbl_l* table = hdPartsView<hdUnreadFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
    sUnreadPaneName.assureTermination();
    mpNumberPane = gabi::call_ptr<void*>(table->mFindPane.get(),root,sUnreadPaneName.mText.get(),1u);
}
/* 026EE5B4 */
BOOL hdUnreadMiiverse_c::create(u32 arg1,u32 arg2,u32 arg3) {
    if (!hdUnread_baseCreate(this,&sUnreadCreateDescriptor,arg1,arg2,arg3)) return FALSE;
    findNumberPane();
    updateText();
    startState(&sUnreadWaitState);
    return TRUE;
}
struct hdUnreadStaticRegistration_l { be<u32> mWords[3]; };
struct UnreadStaticWordsView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static UnreadStaticWordsView_l sUnreadStaticWords;
typedef be<u32> hdUnreadWord_l;
static hdUnreadWord_l sUnreadCounterReady;
static hdUnreadWord_l sUnreadCounter;
typedef be<f32> hdUnreadFloat_l;
static hdUnreadFloat_l sUnreadMinimumAngle;
static hdUnreadFloat_l sUnreadMaximumAngle;
static u8 sUnreadFirstLibraryStatic;
static u8 sUnreadSecondLibraryStatic;
static hdUnreadStaticRegistration_l sUnreadWordsRegistration;
static hdUnreadStaticRegistration_l sUnreadFirstRegistration;
static hdUnreadStaticRegistration_l sUnreadSecondRegistration;
static hdUiStringTable sUnreadStringVtable;
static hdPartsStateIdVtbl_c sUnreadStateVtable;
static hdStateId sUnreadNullState;
struct UnreadInitializedStatesView_l {
    hdPartsStateId_c mValues[4];
    hdPartsStateId_c& operator[](u32 index) { return mValues[index]; }
};
static UnreadInitializedStatesView_l sUnreadInitializedStates;
void hdUnread_registerGlobal(void*);
void hdUnread_constructFirstStatic(void*);
void hdUnread_constructSecondStatic(void*);
static hdUiSafeString sUnreadArchiveName;
/* 026EE780 */
void hdUnreadMiiverse_c::initializeStatics() {
    sUnreadStaticWords[3] = 0;
    sUnreadStaticWords[2] = 0;
    sUnreadStaticWords[1] = 0;
    sUnreadStaticWords[0] = 0;
    hdUnread_registerGlobal(&sUnreadWordsRegistration);
    sUnreadMinimumAngle = -3.1415927410125732f;
    sUnreadMaximumAngle = 3.1415927410125732f;
    hdUnread_constructFirstStatic(&sUnreadFirstLibraryStatic);
    hdUnread_registerGlobal(&sUnreadFirstRegistration);
    hdUnread_constructSecondStatic(&sUnreadSecondLibraryStatic);
    hdUnread_registerGlobal(&sUnreadSecondRegistration);
    sUnreadGroupNames[0].mVtable = &sUnreadStringVtable;
    sUnreadGroupNames[0].mText.v = gabi::guest_addr("G_All_00");
    sUnreadArchiveName.mVtable = &sUnreadStringVtable;
    sUnreadArchiveName.mText.v = gabi::guest_addr("P_Miiverse_00");
    sUnreadPaneName.mVtable = &sUnreadStringVtable;
    sUnreadPaneName.mText.v = gabi::guest_addr("T_NumMiiverse_00");
    sUnreadAnimationNames[0].mVtable = &sUnreadStringVtable;
    sUnreadAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sUnreadAnimationNames[1].mVtable = &sUnreadStringVtable;
    sUnreadAnimationNames[1].mText.v = gabi::guest_addr("OutAll");
    sUnreadCreateDescriptor.mVtable = &sUnreadStringVtable;
    sUnreadCreateDescriptor.mText.v = gabi::guest_addr("UnreadMiiverse_00");
    u32 number = 0;
    if (sUnreadCounterReady) number = sUnreadCounter;
    else sUnreadCounterReady = 1;
    sUnreadInitializedStates[0].setup(++number,"::StateID_Wait",&sUnreadStateVtable,0x19,&sUnreadNullState);
    sUnreadInitializedStates[1].setup(++number,"::StateID_In",&sUnreadStateVtable,0x1C,&sUnreadNullState);
    sUnreadInitializedStates[2].setup(++number,"::StateID_Disp",&sUnreadStateVtable,0x1F,&sUnreadNullState);
    sUnreadInitializedStates[3].setup(++number,"::StateID_Out",&sUnreadStateVtable,0x22,&sUnreadNullState);
    sUnreadCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 str 10108D54 "G_All_00" in 026EE780
 str 10108D20 "P_Miiverse_00" in 026EE780
 str 10108D60 "T_NumMiiverse_00" in 026EE780
 str 10108D10 "InAll" in 026EE780
 str 10108D18 "OutAll" in 026EE780
 str 10108D30 "UnreadMiiverse_00" in 026EE780
 str 10108D74 "::StateID_Wait" in 026EE780
 str 10108D84 "::StateID_In" in 026EE780
 str 10108D94 "::StateID_Disp" in 026EE780
 str 10108D44 "::StateID_Out" in 026EE780
 data sUnreadStaticWords 1049D43C
 data sUnreadMinimumAngle 1049D430
 data sUnreadMaximumAngle 1049D434
 data sUnreadFirstLibraryStatic 1049D438
 data sUnreadSecondLibraryStatic 1049D439
 data sUnreadWordsRegistration 101F709C
 data sUnreadFirstRegistration 101F70A8
 data sUnreadSecondRegistration 101F70B4
 data sUnreadStringVtable 10108C1C
 data sUnreadStateVtable 10108CAC
 data sUnreadNullState 101FF32C
 data sUnreadInitializedStates 1049D45C
 data sUnreadCounterReady 101FD8E4
 data sUnreadCounter 101FDD50
 data sUnreadArchiveName 1049D420
 ext 028F026C void hdUnread_registerGlobal(void*)
 ext 028ED6F8 void hdUnread_constructFirstStatic(void*)
 ext 028EAB2C void hdUnread_constructSecondStatic(void*)
 data sUnreadPaneName 1049D428
 data sUnreadCreateDescriptor 1049D51C
 ext 02700B48 u32 hdUnread_baseCreate(NoControlPart_l*,void*,u32,u32,u32)
 data sUnreadUiSystem 1018C404
 data sUnreadGroupNames 1049D418
 data sUnreadAnimationNames 1049D44C
 data sUnreadGroupIndices 10108C10
 ext 0273B050 void* hdUnread_allocateLayout(u32,hdScreenHeap*,u32)

 ext 02004E04 void hdUnread_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sUnreadStateInterfaceVtable 10108ED0
 data sUnreadDrawInterfaceVtable 10108EE0
 data sUnreadVtable 10108DA8
 data sUnreadWaitState 1049D45C
 data sUnreadInState 1049D48C
 data sUnreadDisplayState 1049D4BC
 data sUnreadOutState 1049D4EC
 data sUnreadEnabled 1047B095
 data sUnreadManager 1018F4AC
 data sUnreadMessage 101F4B5C
 data sUnreadNumberVtable 10108C4C
 data sUnreadNumberFormat 10108D00
 ext 025200D4 hdUnreadPlay_l* hdUnread_getPlay()
 ext 020398B8 u32 hdUnread_getCount(void*)
 ext 025F8DD0 u32 hdUnread_messageActive(void*)
 ext 0273AD10 void* hdUnread_allocate(u32)
 ext 0273AF40 void hdUnread_delete(void*)
 ext 02700ADC void hdUnread_baseConstruct(NoControlPart_l*)
 ext 02700A70 void hdUnread_baseDestroy(NoControlPart_l*,u32)

 ext 02005840 u32 hdUnread_animationEnded(hdAnimSet*,u32)
 ext 02759D3C void hdUnread_format(hdUnreadNumberText_l*,const be<u16>*,s32)

*/
