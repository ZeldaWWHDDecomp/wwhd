/** HD rupee counter animation states and particle visibility. Binary-derived game source. */
#include "d/hd_pane_util.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_rupy.h"
static u8 sRupyVisible;
static hdPartsStateId_c sRupyInState;
static u32 sRupyStateInterfaceVtable;
static u32 sRupyDrawInterfaceVtable;
static hdPartsStateId_c sRupyWaitState;
static hdPartsStateId_c sRupySteadyState;
static hdPartsStateId_c sRupyOutState;
struct hdRupyWallet_l { u8 mPrefix[0x24]; be<u16> mRupees; u8 mToWallet[0x32 - 0x26]; be<u8> mWallet; };
typedef gptr<hdRupyWallet_l> hdRupyWalletPointer_l;
static hdRupyWalletPointer_l sRupyWallet;
struct hdRupyPlay_l {
    u8 mPrefix[0x5AB0]; gptr<void> mpParticles;
    u8 mToRupees[0x5BAE - 0x5AB4]; be<u16> mRupees;
};
struct hdRupyVector_l { be<f32> mX, mY, mZ; };
WWHD_OFFSET(hdRupyPlay_l, mRupees, 0x5BAE);
hdRupyPlay_l* hdRupy_getPlay();
u32 hdRupy_animationEnded(hdAnimSet*, u32);
static inline void hdRupy_getPaneVector(hdRupyVector_l* out, void* pane) {
    hdPaneUtil_c::getGlobalTranslation(reinterpret_cast<cXyz*>(out), reinterpret_cast<PaneUtilPane_l*>(pane));
}
hdRupyEmitter_l* hdRupy_createEmitter(void*,u32,u32,hdRupyVector_l*,u32,u32,u32,u32,s32,u32,u32,u32);
static inline void hdRupy_baseFinalize(hdRupyScreen_c* receiver) { receiver->hdParts_c::finalize(); }
struct hdRupyNumberText_l {
    gptr<be<u16>> mText;
    gptr<void> mVtable;
    be<u32> mCapacity;
    be<u16> mBuffer[5];
};
WWHD_OFFSET(hdRupyNumberText_l, mText, 0);
WWHD_OFFSET(hdRupyNumberText_l, mVtable, 4);
WWHD_OFFSET(hdRupyNumberText_l, mCapacity, 8);
static u32 sRupyNumberTextVtable;
struct RupyThreeDigitFormatView_l {
    be<u16> mValues[5];
    be<u16>& operator[](u32 index) { return mValues[index]; }
};
static RupyThreeDigitFormatView_l sRupyThreeDigitFormat;
struct RupyFourDigitFormatView_l {
    be<u16> mValues[5];
    be<u16>& operator[](u32 index) { return mValues[index]; }
};
static RupyFourDigitFormatView_l sRupyFourDigitFormat;
void hdRupy_formatNumber(hdRupyNumberText_l*, const be<u16>*, u32);
static inline void hdRupy_setPaneText(void* pane, hdRupyNumberText_l* text, u32 work) {
    reinterpret_cast<hdTextPane_c*>(pane)->setStringPrepared(reinterpret_cast<PaneString_l*>(text), work ? hdPartsView<PaneWork_l>(work) : nullptr);
}
void hdRupy_baseDestroy(NoControlPart_l*, u32);
void hdRupy_delete(void*);



/* 026CA38C */
static u32 hdRupy_stateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 026CA48C */
void hdRupyScreen_c::destroy(hdRupyScreen_c* screen, u32 flags) {
    if (!screen)
        return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sRupyStateInterfaceVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sRupyDrawInterfaceVtable);
    hdRupy_baseDestroy(screen, 0);
    if (flags & 1)
        hdRupy_delete(screen);
}
/* 026CA4F8 */
void hdRupyScreen_c::resetInAnimation() {
    hdAnimSet_setFrame(mpLayout->animSet(), 0, 1, 0.0f);
}
/* 026CA514 */
void hdRupyScreen_c::waitExecute() {
    if (sRupyVisible)
        hdParts_changeState(stateMachine(), &sRupyInState);
}
/* 026CA820 */
void hdRupyScreen_c::hideEmitter() {
    hdRupyEmitter_l* emitter = mpEmitter;
    if (!emitter)
        return;
    emitter->mFlags = u32(emitter->mFlags) & ~4u;
    updateEmitter();
}
/* 026CA944 */
void hdRupyScreen_c::showEmitter() {
    hdRupyEmitter_l* emitter = mpEmitter;
    if (emitter)
        emitter->mFlags = u32(emitter->mFlags) | 4;
}
/* 026CA960 */
void hdRupyScreen_c::startOutAnimation() {
    hdAnimSet_play(mpLayout->animSet(), 2, 1, 1.0f);
}
/* 026CA73C */
void hdRupyScreen_c::inExecute() {
    u16 rupees = hdRupy_getPlay()->mRupees;
    if (u16(mRupees) != rupees) {
        mRupees = hdRupy_getPlay()->mRupees;
        updateNumber();
    }
    if (hdRupy_animationEnded(mpLayout->animSet(), 0))
        hdParts_changeState(stateMachine(), &sRupySteadyState);
}
/* 026CA7B4 */
void hdRupyScreen_c::updateEmitter() {
    if (!mpEmitter)
        return;
    hdRupyVector_l position;
    hdRupy_getPaneVector(&position, mpAnchorPane);
    hdRupyEmitter_l* emitter = mpEmitter;
    f32 x = position.mX;
    u8 layer = emitter->mLayer;
    f32 y = position.mY;
    f32 z = position.mZ;
    if (layer >= 7)
        y = -y;
    emitter->mY = y;
    emitter->mX = x;
    emitter->mZ = z;
}
/* 026CA97C */
void hdRupyScreen_c::outExecute() {
    u16 rupees = hdRupy_getPlay()->mRupees;
    if (u16(mRupees) != rupees) {
        mRupees = hdRupy_getPlay()->mRupees;
        updateNumber();
    }
    if (hdRupy_animationEnded(mpLayout->animSet(), 2))
        hdParts_changeState(stateMachine(), &sRupyWaitState);
}
/* 026CAC60 */
void hdRupyScreen_c::endEmitter() {
    hdRupy_baseFinalize(this);
    hdRupyEmitter_l* emitter = mpEmitter;
    if (emitter) {
        u32 flags = emitter->mFlags;
        emitter->mLifetime = -1;
        emitter->mFlags = flags | 1;
        emitter = mpEmitter;
        emitter->mFlags = u32(emitter->mFlags) & ~0x40u;
    }
}
/* 026CACBC */
void hdRupyScreen_c::endExecute() {
    if (mpEmitter)
        mpEmitter = NULL;
    hdParts_changeState(stateMachine(), &sRupyWaitState);
}
/* 026CA534 */
void hdRupyScreen_c::updateNumber() {
    u8 wallet = mWallet;
    u32 rupees = mRupees;
    hdRupyNumberText_l text;
    text.mBuffer[0] = 0;
    text.mBuffer[4] = 0;
    text.mCapacity = 5;
    text.mVtable.v = gabi::ea(&sRupyNumberTextVtable);
    text.mText = &text.mBuffer[0];
    if (!wallet) {
        hdRupy_formatNumber(&text, sRupyThreeDigitFormat.mValues, rupees);
        hdRupy_setPaneText(mpNumberPanes[0], &text, 0);
    } else {
        hdRupy_formatNumber(&text, sRupyFourDigitFormat.mValues, rupees);
        hdRupy_setPaneText(mpNumberPanes[1], &text, 0);
    }
}
/* 026CA610 */
void hdRupyScreen_c::inEnter() {
    mRupees = hdRupy_getPlay()->mRupees;
    u8 wallet = sRupyWallet->mWallet;
    hdAnimSet* animations = mpLayout->animSet();
    mWallet = wallet;
    hdAnimSet_setFrame(animations, 1, 0, f32(wallet));
    hdAnimSet_play(mpLayout->animSet(), 0, 1, 1.0f);
    updateNumber();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    if (!mpEmitter) {
        hdRupyVector_l source;
        hdRupy_getPaneVector(&source, mpAnchorPane);
        hdRupyVector_l position;
        position.mX = f32(source.mX);
        position.mY = f32(source.mY);
        position.mZ = f32(source.mZ);
        void* particles = hdRupy_getPlay()->mpParticles;
        mpEmitter = hdRupy_createEmitter(particles, 7, 0x2D, &position, 0, 0, 0xFF, 0, -1, 0, 0, 0);
        hdRupyEmitter_l* emitter = mpEmitter;
        emitter->mFlags = u32(emitter->mFlags) | 4;
    }
}
/* 026CA83C */
void hdRupyScreen_c::displayExecute() {
    bool changed = false;
    u8 wallet = sRupyWallet->mWallet;
    mWallet = wallet;
    if (wallet) {
        u8 currentWallet = sRupyWallet->mWallet;
        hdAnimSet* animations = mpLayout->animSet();
        mWallet = currentWallet;
        hdAnimSet_setFrame(animations, 1, 0, f32(currentWallet));
        changed = true;
        gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
        updateEmitter();
    }
    u16 rupees = hdRupy_getPlay()->mRupees;
    if (u16(mRupees) != rupees) {
        mRupees = hdRupy_getPlay()->mRupees;
        updateNumber();
    } else if (changed) {
        updateNumber();
    }
    if (!sRupyVisible)
        hdParts_changeState(stateMachine(), &sRupyOutState);
}
typedef gptr<hdScreenUiSystem> hdRupyUiSystemPointer_l;
static hdRupyUiSystemPointer_l sRupyUiSystem;
struct RupyGroupNamesView_l {
    hdUiSafeString mValues[2];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static RupyGroupNamesView_l sRupyGroupNames;
struct RupyAnimationNamesView_l {
    hdUiSafeString mValues[3];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static RupyAnimationNamesView_l sRupyAnimationNames;
struct RupyGroupIndicesView_l {
    be<u32> mValues[3];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static RupyGroupIndicesView_l sRupyGroupIndices;
void* hdRupy_allocateLayout(u32,hdScreenHeap*,u32);

void hdRupy_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 026CACE0 */
BOOL hdRupyScreen_c::loadLayout(void* name,u32 arg1,u32 arg2,u32 arg3,void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdRupy_allocateLayout(sizeof(hdPartsLayout_c),sRupyUiSystem->mHeap,4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return FALSE;
    layout->vLoad(name,arg1,arg2,arg3,3,2,desc);
    for (u32 i=0; i<3; ++i) {
        u32 groupAddress = gabi::ea(&sRupyGroupNames) + u32(sRupyGroupIndices[i]) * sizeof(hdUiSafeString);
        hdRupy_registerAnimation(mpLayout->animSet(),i,&sRupyAnimationNames[i],hdPartsView<hdUiSafeString>(groupAddress));
    }
    return TRUE;
}
static hdPartsVtbl_c sRupyVtable;
void* hdRupy_allocate(u32);
void hdRupy_baseConstruct(NoControlPart_l*);
/* 026CA9F4 */
hdRupyScreen_c* hdRupyScreen_c::construct(hdRupyScreen_c* screen) {
    if (!screen) screen = (hdRupyScreen_c*)hdRupy_allocate(sizeof(hdRupyScreen_c));
    if (screen) {
        hdRupy_baseConstruct(screen);
        screen->mVtable = &sRupyVtable;
        screen->mSecondaryVtable.v = gabi::ea(&sRupyDrawInterfaceVtable);
        u32 panesAddress = gabi::ea(screen) + offsetof(hdRupyScreen_c, mpNumberPanes);
        screen->mStateHolder.mHolderVtable.v = gabi::ea(&sRupyStateInterfaceVtable);
        if (!panesAddress) hdRupy_allocate(8);
        screen->mpEmitter = NULL;
        screen->mpAnchorPane = NULL;
        screen->mRupees = sRupyWallet->mRupees;
        screen->mWallet = sRupyWallet->mWallet;
        for (u32 i=0; i<2; ++i) hdPartsView<gptr<void>>(panesAddress)[i] = NULL;
    }
    return screen;
}
struct hdRupyFindPaneVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdRupyFindPaneVtbl_l, mFindPane, 0x5C);
struct RupyPaneNamesView_l {
    hdUiSafeString mValues[2];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static RupyPaneNamesView_l sRupyPaneNames;
static hdUiSafeString sRupyCreateDescriptor;
u32 hdRupy_baseCreate(NoControlPart_l*,void*,u32,u32,u32);
static hdUiSafeString sRupyAnchorName;
/* 026CAAD4 */
BOOL hdRupyScreen_c::create(u32 arg1,u32 arg2,u32 arg3) {
    if (!hdRupy_baseCreate(this,&sRupyCreateDescriptor,arg1,arg2,arg3)) return FALSE;
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i=0; i<2; ++i) {
        hdUiSafeString* name = &sRupyPaneNames[i];
        hdRupyFindPaneVtbl_l* table = hdPartsView<hdRupyFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
        name->assureTermination();
        mpNumberPanes[i] = gabi::call_ptr<void*>(table->mFindPane.get(),root,name->mText.get(),1u);
    }
    hdRupyFindPaneVtbl_l* table = hdPartsView<hdRupyFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
    sRupyAnchorName.assureTermination();
    mpAnchorPane = gabi::call_ptr<void*>(table->mFindPane.get(),root,sRupyAnchorName.mText.get(),1u);
    startState(&sRupyWaitState);
    return TRUE;
}
struct hdRupyGlobalRecord_l { be<u32> mWords[3]; };
struct RupyStaticWordsView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static RupyStaticWordsView_l sRupyStaticWords;
typedef be<u32> hdRupyWord_l;
static hdRupyWord_l sRupyStateCounter;
static hdRupyWord_l sRupyStateCounterReady;
typedef be<f32> hdRupyFloat_l;
static hdRupyFloat_l sRupyMinimumAngle;
static hdRupyFloat_l sRupyMaximumAngle;
static u8 sRupyFirstLibraryStatic;
static u8 sRupySecondLibraryStatic;
static hdRupyGlobalRecord_l sRupyWordsRegistration;
static hdRupyGlobalRecord_l sRupyFirstRegistration;
static hdRupyGlobalRecord_l sRupySecondRegistration;
static hdUiStringTable sRupyStringVtable;
static hdPartsStateIdVtbl_c sRupyStateVtable;
static hdStateId sRupyNullState;
void hdRupy_registerGlobal(void*);
void hdRupy_constructFirstStatic(void*);
void hdRupy_constructSecondStatic(void*);
/* 026CADF0 */
void hdRupyScreen_c::initializeStatics() {
    sRupyStaticWords[3] = 0;
    sRupyStaticWords[2] = 0;
    sRupyStaticWords[1] = 0;
    sRupyStaticWords[0] = 0;
    hdRupy_registerGlobal(&sRupyWordsRegistration);
    sRupyMinimumAngle = -3.1415927410125732f;
    sRupyMaximumAngle = 3.1415927410125732f;
    hdRupy_constructFirstStatic(&sRupyFirstLibraryStatic);
    hdRupy_registerGlobal(&sRupyFirstRegistration);
    hdRupy_constructSecondStatic(&sRupySecondLibraryStatic);
    hdRupy_registerGlobal(&sRupySecondRegistration);
    sRupyPaneNames[0].mVtable = &sRupyStringVtable;
    sRupyPaneNames[0].mText.v = gabi::guest_addr("T_Num_00");
    sRupyPaneNames[1].mVtable = &sRupyStringVtable;
    sRupyPaneNames[1].mText.v = gabi::guest_addr("T_Num_01");
    sRupyGroupNames[0].mVtable = &sRupyStringVtable;
    sRupyGroupNames[0].mText.v = gabi::guest_addr("G_Color_00");
    sRupyGroupNames[1].mVtable = &sRupyStringVtable;
    sRupyGroupNames[1].mText.v = gabi::guest_addr("G_All_00");
    sRupyAnimationNames[0].mVtable = &sRupyStringVtable;
    sRupyAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sRupyAnimationNames[1].mVtable = &sRupyStringVtable;
    sRupyAnimationNames[1].mText.v = gabi::guest_addr("Color");
    sRupyAnimationNames[2].mVtable = &sRupyStringVtable;
    sRupyAnimationNames[2].mText.v = gabi::guest_addr("OutAll");
    sRupyAnchorName.mVtable = &sRupyStringVtable;
    sRupyAnchorName.mText.v = gabi::guest_addr("P_Rupy_00");
    sRupyCreateDescriptor.mVtable = &sRupyStringVtable;
    sRupyCreateDescriptor.mText.v = gabi::guest_addr("Rupy_00");
    u32 number = 0;
    if (sRupyStateCounterReady) number = sRupyStateCounter;
    else sRupyStateCounterReady = 1;
    sRupyWaitState.setup(++number,"::StateID_Wait",&sRupyStateVtable,0x19,&sRupyNullState);
    sRupyInState.setup(++number,"::StateID_In",&sRupyStateVtable,0x1C,&sRupyNullState);
    sRupySteadyState.setup(++number,"::StateID_Disp",&sRupyStateVtable,0x1F,&sRupyNullState);
    sRupyOutState.setup(++number,"::StateID_Out",&sRupyStateVtable,0x22,&sRupyNullState);
    sRupyStateCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sRupyStaticWords 1049A1B4
 data sRupyMinimumAngle 1049A1A8
 data sRupyMaximumAngle 1049A1AC
 data sRupyFirstLibraryStatic 1049A1B0
 data sRupySecondLibraryStatic 1049A1B1
 data sRupyWordsRegistration 101F6974
 data sRupyFirstRegistration 101F6980
 data sRupySecondRegistration 101F698C
 data sRupyStringVtable 10101A3C
 data sRupyStateVtable 10101AD8
 data sRupyNullState 101FF32C
 data sRupyStateCounterReady 101FD8E4
 data sRupyStateCounter 101FDD50
 str 10101B70 "P_Rupy_00" in 026CADF0
 str 10101B50 "Rupy_00" in 026CADF0
 str 10101B8C "G_Color_00" in 026CADF0
 str 10101B98 "G_All_00" in 026CADF0
 str 10101B58 "InAll" in 026CADF0
 str 10101B60 "Color" in 026CADF0
 str 10101B68 "OutAll" in 026CADF0
 str 10101BA4 "T_Num_00" in 026CADF0
 str 10101BB0 "T_Num_01" in 026CADF0
 str 10101BBC "::StateID_Wait" in 026CADF0
 str 10101BCC "::StateID_In" in 026CADF0
 str 10101BDC "::StateID_Disp" in 026CADF0
 str 10101B7C "::StateID_Out" in 026CADF0
 ext 028F026C void hdRupy_registerGlobal(void*)
 ext 028ED6F8 void hdRupy_constructFirstStatic(void*)
 ext 028EAB2C void hdRupy_constructSecondStatic(void*)
 data sRupyPaneNames 1049A1EC
 data sRupyCreateDescriptor 1049A2BC
 ext 02700B48 u32 hdRupy_baseCreate(NoControlPart_l*,void*,u32,u32,u32)
 data sRupyAnchorName 1049A1A0
 data sRupyUiSystem 1018C404
 data sRupyGroupNames 1049A1C4
 data sRupyAnimationNames 1049A1D4
 data sRupyGroupIndices 10101A84
 ext 0273B050 void* hdRupy_allocateLayout(u32,hdScreenHeap*,u32)

 ext 02004E04 void hdRupy_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sRupyVtable 10101BF0
 ext 0273AD10 void* hdRupy_allocate(u32)
 ext 02700ADC void hdRupy_baseConstruct(NoControlPart_l*)
 data sRupyStateInterfaceVtable 10101D18
 data sRupyDrawInterfaceVtable 10101D28
 data sRupyWallet 101F84DC
 data sRupyOutState 1049A28C
 data sRupyWaitState 1049A1FC
 data sRupySteadyState 1049A25C
 data sRupyVisible 1047B091
 data sRupyInState 1049A22C
 ext 02700A70 void hdRupy_baseDestroy(NoControlPart_l*,u32)
 ext 0273AF40 void hdRupy_delete(void*)

 ext 025200D4 hdRupyPlay_l* hdRupy_getPlay()
 ext 02005840 u32 hdRupy_animationEnded(hdAnimSet*,u32)

 ext 025A847C hdRupyEmitter_l* hdRupy_createEmitter(void*,u32,u32,hdRupyVector_l*,u32,u32,u32,u32,s32,u32,u32,u32)

 data sRupyNumberTextVtable 10101A6C
 data sRupyThreeDigitFormat 10101B28
 data sRupyFourDigitFormat 10101B34
 ext 02759D3C void hdRupy_formatNumber(hdRupyNumberText_l*,const be<u16>*,u32)


 ext 026F8B00 void hdParts_c::finalize()
 ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*,PaneWork_l*)
 ext 02704D2C void hdPaneUtil_c::getGlobalTranslation(cXyz*,PaneUtilPane_l*)
*/
