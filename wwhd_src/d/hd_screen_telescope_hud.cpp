/** Telescope HUD animation transitions. */
#include "d/hd_screen_telescope_hud.h"
struct hdTelescopePlay_l {
    u8 mPrefix[0x5292]; be<u8> mEventRunning;
    u8 mToButtons[0x5B00 - 0x5293]; be<u32> mButtons; be<f32> mZoomRatio;
    u8 mToMode[0x5BB2 - 0x5B08]; be<u8> mMode, mNextMode;
    u8 mToSound[5]; be<u8> mSoundMode;
    u8 mToDemo[0x5BCF - 0x5BBA]; be<u8> mDemoMode;
    u8 mToZoom[0x5C21 - 0x5BD0]; be<u8> mZoomActive;
    u8 mToFlags[0x5CD8 - 0x5C22]; be<u32> mDisplayFlags;
};
struct hdTelescopeMessage_l {
    u8 mPrefix[0x10]; be<u32> mSelection;
    u8 mToActive[0x9A0 - 0x14]; be<u8> mTelescopeActive;
};
WWHD_OFFSET(hdTelescopePlay_l, mButtons, 0x5B00);
WWHD_OFFSET(hdTelescopePlay_l, mMode, 0x5BB2);
WWHD_OFFSET(hdTelescopePlay_l, mNextMode, 0x5BB3);
WWHD_OFFSET(hdTelescopePlay_l, mDemoMode, 0x5BCF);
WWHD_OFFSET(hdTelescopePlay_l, mZoomActive, 0x5C21);
WWHD_OFFSET(hdTelescopePlay_l, mDisplayFlags, 0x5CD8);
WWHD_OFFSET(hdTelescopeMessage_l, mTelescopeActive, 0x9A0);
static u32 sTelescopeStateInterfaceVtable;
static u32 sTelescopeDrawInterfaceVtable;
struct TelescopePaneNamesView_l {
    hdUiSafeString mValues[2];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static TelescopePaneNamesView_l sTelescopePaneNames;
static hdUiSafeString sTelescopeScaleName;
typedef gptr<hdTelescopeMessage_l> hdTelescopeMessagePointer_l;
static hdTelescopeMessagePointer_l sTelescopeMessage;
typedef be<u32> hdTelescopeWord_l;
static hdTelescopeWord_l sTelescopeEvent;
static const u32 telescopeNameDestructor = 0x026E9224;
static hdPartsStateId_c sTelescopeWait;
static hdPartsStateId_c sTelescopeIn;
static hdPartsStateId_c sTelescopeDemoIn;
static hdPartsStateId_c sTelescopeDemoWait;
static hdPartsStateId_c sTelescopeNormal;
static hdPartsStateId_c sTelescopeCoverWait;
static hdPartsStateId_c sTelescopeZoom;
static hdPartsStateId_c sTelescopeZoomEnd;
static hdPartsStateId_c sTelescopeZoomOut;
static hdPartsVtbl_c sTelescopeVtable;
struct hdTelescopeCameraSlot_l { gptr<void> mpCamera; u8 mPadding[0x30]; };
// The camera's letterbox trim controller, handed to hdTelescope_trimCamera.
struct hdTelescopeCamera_l { u8 mPrefix[0x248]; u8 mTrimControl; };
WWHD_OFFSET(hdTelescopeCamera_l, mTrimControl, 0x248);
WWHD_SIZE(hdTelescopeCameraSlot_l, 0x34);
struct hdTelescopeCameraPlayer_l { u8 mPrefix[0x5B30]; be<s8> mCameraIndex; };
WWHD_OFFSET(hdTelescopeCameraPlayer_l, mCameraIndex, 0x5B30);
void hdTelescope_trimCamera(void*,u32);
void* hdTelescope_allocate(u32);
static inline void hdTelescope_baseConstruct(hdTelescopeHud_c* receiver) { hdParts_c::construct(receiver); }
hdTelescopePlay_l* hdTelescope_getPlay();
void hdTelescope_setMessageMode(hdTelescopeMessage_l*,u32);
void hdTelescope_delete(void*);
static inline void hdTelescope_baseDestroy(hdTelescopeHud_c* receiver, u32 flags) { hdParts_c::destroy(receiver, flags); }
void hdTelescope_destroyStrings(void*,u32,u32,u32,u32,u32);



void hdTelescope_animationStop(hdAnimSet*,u32,u32);
void hdTelescope_animationSeek(hdAnimSet*,u32,u32,f32);
u32 hdTelescope_animationEnded(hdAnimSet*,u32);
void hdTelescope_playSound(u32);
/* 026E778C */
u32 hdTelescopeHud_c::stateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 026E788C */
void hdTelescopeHud_c::destroyPaneNames() {
    hdTelescope_destroyStrings(&sTelescopePaneNames,2,8,telescopeNameDestructor,0,0);
}
/* 026E78B0 */
void hdTelescopeHud_c::destroyScaleName() {
    hdTelescope_destroyStrings(&sTelescopeScaleName,1,8,telescopeNameDestructor,0,0);
}
/* 026E78D4 */
void hdTelescopeHud_c::destroy(hdTelescopeHud_c* screen,u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sTelescopeStateInterfaceVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sTelescopeDrawInterfaceVtable);
    hdTelescope_baseDestroy(screen,0);
    if (flags & 1) hdTelescope_delete(screen);
}
/* 026E7940 */
void hdTelescopeHud_c::waitEnter() {
    sTelescopeMessage->mTelescopeActive = 0;
    hdTelescope_setMessageMode(sTelescopeMessage,0);
    hdTelescope_getPlay()->mNextMode = 0;
    hdTelescope_getPlay()->mDemoMode = 0;
    hdTelescope_getPlay()->mZoomActive = 0;
    mpZoomPane->mFlags = (u8(mpZoomPane->mFlags) & 0xFE) + 1;
}
/* 026E79B4 */
void hdTelescopeHud_c::waitExecute() {
    if (u32(hdTelescope_getPlay()->mDisplayFlags) & 0x00200000) {
        hdPartsStateId_c* state = &sTelescopeIn;
        if (hdTelescope_getPlay()->mDemoMode == 1) state = &sTelescopeDemoIn;
        hdParts_changeState(stateMachine(),state);
        sTelescopeMessage->mTelescopeActive = 1;
    }
}
/* 026E7A24 */
void hdTelescopeHud_c::inEnter() {
    hdAnimSet_play(mpLayout->animSet(),0,0,1.0f);
    hdAnimSet_operationCC(mpLayout->animSet(),1,2);
    hdTelescope_getPlay()->mNextMode = 0;
    hdTelescope_animationSeek(mpLayout->animSet(),2,1,0.0f);
    hdTelescope_getPlay()->mZoomActive = 1;
}
/* 026E7AB0 */
void hdTelescopeHud_c::inExecute() {
    if (hdTelescope_animationEnded(mpLayout->animSet(),0)) {
        hdTelescope_getPlay()->mMode = 11;
        hdTelescope_getPlay()->mNextMode = 11;
        hdTelescope_playSound(0x823);
        hdParts_changeState(stateMachine(),&sTelescopeNormal);
    }
}
/* 026E7B24 */
void hdTelescopeHud_c::demoInEnter() {
    hdTelescope_animationStop(mpLayout->animSet(),0,0);
    hdAnimSet_play(mpLayout->animSet(),5,3,1.0f);
    hdTelescope_animationStop(mpLayout->animSet(),1,2);
    hdTelescope_animationSeek(mpLayout->animSet(),2,1,0.0f);
    hdTelescope_getPlay()->mNextMode = 0;
}
/* 026E7BB8 */
void hdTelescopeHud_c::demoInExecute() {
    if (hdTelescope_animationEnded(mpLayout->animSet(),5))
        hdParts_changeState(stateMachine(),&sTelescopeDemoWait);
}
/* 026E7C08 */
void hdTelescopeHud_c::normalDemoEnter() {
    hdAnimSet_play(mpLayout->animSet(),6,3,1.0f);
    hdTelescope_getPlay()->mZoomActive = 1;
}
/* 026E7C4C */
void hdTelescopeHud_c::normalDemoExecute() {
    if (hdTelescope_animationEnded(mpLayout->animSet(),6)) {
        hdTelescope_getPlay()->mMode = 11;
        hdTelescope_getPlay()->mNextMode = 11;
        hdTelescope_playSound(0x823);
        hdParts_changeState(stateMachine(),&sTelescopeNormal);
    }
}
/* 026E7CC0 */
void hdTelescopeHud_c::resetScale() { mScale = 0.0f; }
/* 026E7FB0 */
void hdTelescopeHud_c::endZoom() { hdTelescope_getPlay()->mZoomActive = 0; }
/* 026E7FD8 */
void hdTelescopeHud_c::outEnter() { hdAnimSet_play(mpLayout->animSet(),4,0,1.0f); }
/* 026E7FF4 */
void hdTelescopeHud_c::outExecute() {
    if (hdTelescope_animationEnded(mpLayout->animSet(),4))
        hdParts_changeState(stateMachine(),&sTelescopeWait);
}
/* 026E8044 */
void hdTelescopeHud_c::zoomInEnter() { mFrame = 0; }
/* 026E80E4 */
void hdTelescopeHud_c::coverOutEnter() {
    sTelescopeMessage->mSelection = 0;
    mpZoomPane->mFlags = u8(mpZoomPane->mFlags) & 0xFE;
    hdAnimSet_play(mpLayout->animSet(),3,2,1.0f);
}
/* 026E8120 */
void hdTelescopeHud_c::coverOutExecute() {
    if (hdTelescope_animationEnded(mpLayout->animSet(),3))
        hdParts_changeState(stateMachine(),&sTelescopeCoverWait);
}
/* 026E8174 */
void hdTelescopeHud_c::demoWaitExecute() {
    if (sTelescopeEvent == 0x1A9)
        hdParts_changeState(stateMachine(),&sTelescopeZoom);
}
/* 026E8214 */
void hdTelescopeHud_c::zoomExecute() {
    if ((u8(mpAllPane->mFlags) & 1) && hdTelescope_animationEnded(mpLayout->animSet(),4))
        mpAllPane->mFlags = u8(mpAllPane->mFlags) & 0xFE;
    if (sTelescopeEvent == 0x460)
        hdParts_changeState(stateMachine(),&sTelescopeZoomEnd);
}
/* 026E838C */
void hdTelescopeHud_c::zoomOutEnter() {
    mFrame = 0;
    mZoomOut = 1;
    hdTelescope_getPlay()->mMode = 14;
}
/* 026E841C */
void hdTelescopeHud_c::resetAnimations() {
    hdAnimSet_operationCC(mpLayout->animSet(),0,0);
    hdAnimSet_operationCC(mpLayout->animSet(),1,2);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(),this);
}
/* 026E8480 */
void hdTelescopeHud_c::resetState() { hdParts_changeState(stateMachine(),&sTelescopeWait); }
/* 026E8194 */
void hdTelescopeHud_c::zoomEnter() {
    s32 index = ((hdTelescopeCameraPlayer_l*)hdTelescope_getPlay())->mCameraIndex;
    hdTelescopePlay_l* play = hdTelescope_getPlay();
    u32 slotAddress = gabi::ea(play) + u32(index * 0x34) + 0x5AF8;
    void* camera = hdPartsView<hdTelescopeCameraSlot_l>(slotAddress)->mpCamera;
    hdTelescope_trimCamera(hdPartsView<void>(gabi::ea(camera) + offsetof(hdTelescopeCamera_l, mTrimControl)),3);
    hdTelescope_animationStop(mpLayout->animSet(),4,0);
    hdTelescope_animationStop(mpLayout->animSet(),3,2);
}
/* 026E82A0 */
void hdTelescopeHud_c::zoomEndEnter() {
    s32 index = ((hdTelescopeCameraPlayer_l*)hdTelescope_getPlay())->mCameraIndex;
    hdTelescopePlay_l* play = hdTelescope_getPlay();
    u32 slotAddress = gabi::ea(play) + u32(index * 0x34) + 0x5AF8;
    void* camera = hdPartsView<hdTelescopeCameraSlot_l>(slotAddress)->mpCamera;
    hdTelescope_trimCamera(hdPartsView<void>(gabi::ea(camera) + offsetof(hdTelescopeCamera_l, mTrimControl)),4);
    mpAllPane->mFlags = (u8(mpAllPane->mFlags) & 0xFE) + 1;
    hdTelescope_animationStop(mpLayout->animSet(),0,0);
    hdTelescope_animationStop(mpLayout->animSet(),3,2);
}
/* 026E8334 */
void hdTelescopeHud_c::normalExecute() {
    if (hdTelescope_getPlay()->mNextMode != 13 && !(u32(hdTelescope_getPlay()->mButtons) & 8))
        hdParts_changeState(stateMachine(),&sTelescopeZoomOut);
}
/* 026E8490 */
hdTelescopeHud_c* hdTelescopeHud_c::construct(hdTelescopeHud_c* screen) {
    if (!screen) screen = (hdTelescopeHud_c*)hdTelescope_allocate(sizeof(hdTelescopeHud_c));
    if (screen) {
        hdTelescope_baseConstruct(screen);
        screen->mSecondaryVtable.v = gabi::ea(&sTelescopeDrawInterfaceVtable);
        screen->mVtable = &sTelescopeVtable;
        screen->mStateHolder.mHolderVtable.v = gabi::ea(&sTelescopeStateInterfaceVtable);
        if (gabi::ea(screen) + u32(offsetof(hdTelescopeHud_c, mpAllPane)) == 0) hdTelescope_allocate(8);
        if (gabi::ea(screen) + u32(offsetof(hdTelescopeHud_c, mpScalePane)) == 0) hdTelescope_allocate(4);
        screen->mFrame = 0;
        screen->mScale = 0.0f;
        screen->mZoomOut = 0;
    }
    return screen;
}
static hdTelescopeWord_l sTelescopePaneNamesReady;
static hdUiStringTable sTelescopeStringVtable;
struct hdTelescopeGlobalRecord_l { be<u32> mWords[3]; };
static hdTelescopeGlobalRecord_l sTelescopePaneNameRegistration;
struct hdTelescopeFindVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdTelescopeFindVtbl_l, mFindPane, 0x5C);
void hdTelescope_registerGlobal(void*);
/* 026E8534 */
void hdTelescopeHud_c::findPanes() {
    if (!sTelescopePaneNamesReady) {
        sTelescopePaneNamesReady = 1;
        sTelescopePaneNames[0].mVtable = &sTelescopeStringVtable;
        sTelescopePaneNames[0].mText.v = gabi::guest_addr("N_All_00");
        sTelescopePaneNames[1].mVtable = &sTelescopeStringVtable;
        sTelescopePaneNames[1].mText.v = gabi::guest_addr("N_Zoom_00");
        hdTelescope_registerGlobal(&sTelescopePaneNameRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i=0; i<2; ++i) {
        hdUiSafeString* name = &sTelescopePaneNames[i];
        hdTelescopeFindVtbl_l* table = hdPartsView<hdTelescopeFindVtbl_l>(gabi::ea(root->mVtable.get()));
        name->assureTermination();
        hdTelescopePane_l* pane = gabi::call_ptr<hdTelescopePane_l*>(table->mFindPane.get(),root,name->mText.get(),1u);
        hdPartsView<gptr<hdTelescopePane_l>>(gabi::ea(this) + offsetof(hdTelescopeHud_c, mpAllPane))[i] = pane; // mpAllPane, mpZoomPane
    }
}
struct hdTelescopeScaleText_l {
    gptr<be<u16>> mText; gptr<hdUiStringTable> mVtable;
    be<u32> mCapacity; be<u16> mBuffer[3];
};
struct hdTelescopeTextPaneVtbl_l { u8 mPrefix[0xC4]; gfn mSetText; };
struct hdTelescopeTextPane_l { u8 mPrefix[8]; gptr<hdTelescopeTextPaneVtbl_l> mVtable; };
static hdUiStringTable sTelescopeScaleTextVtable;
struct TelescopeScaleFormatView_l {
    be<u16> mValues[5];
    be<u16>& operator[](u32 index) { return mValues[index]; }
};
static TelescopeScaleFormatView_l sTelescopeScaleFormat;
static hdPartsStateId_c sTelescopeOut;
static hdPartsStateId_c sTelescopeCoverOut;
void hdTelescope_formatScale(hdTelescopeScaleText_l*,const be<u16>*,u32,u32);
void hdTelescope_playScaleSound(u32,u32,u32);
static u32 telescopeUnsignedFloat(f32 value) {
    // PPC fcmpu+bge takes the high path for unordered values too.
    if (!(value < 2147483648.0f))
        return u32(gabi::ftoi(gabi::fsubs_ppc(value,2147483648.0f))) + 0x80000000u;
    return u32(gabi::ftoi(value));
}
/* 026E7CD0 */
void hdTelescopeHud_c::updateScaleText(f32 scale) {
    if (!mpScalePane) return;
    hdTelescopeScaleText_l text;
    text.mText = text.mBuffer;
    text.mCapacity = 3;
    text.mBuffer[0] = 0;
    text.mBuffer[2] = 0;
    text.mVtable = &sTelescopeScaleTextVtable;
    hdTelescope_formatScale(&text,sTelescopeScaleFormat.mValues,0xD7,telescopeUnsignedFloat(scale));
    hdTelescopeTextPane_l* pane = (hdTelescopeTextPane_l*)mpScalePane.get();
    hdTelescopeTextPaneVtbl_l* table = pane->mVtable;
    gabi::call_ptr<void>(text.mVtable->mAssureTermination.get(),&text);
    gabi::call_ptr<void>(table->mSetText.get(),pane,text.mText.get(),0u);
}
/* 026E7DD4 */
void hdTelescopeHud_c::displayExecute() {
    if (hdTelescope_getPlay()->mNextMode == 13 && hdTelescope_getPlay()->mEventRunning) {
        hdTelescope_getPlay()->mNextMode = 11;
        hdParts_changeState(stateMachine(),&sTelescopeCoverOut);
        return;
    }
    if (!(u32(hdTelescope_getPlay()->mDisplayFlags) & 0x00200000)) {
        hdParts_changeState(stateMachine(),&sTelescopeOut);
        return;
    }
    f32 scale = hdTelescope_getPlay()->mZoomRatio;
    updateScaleText(scale);
    scale = gabi::fsubs_ppc(scale,1.0f);
    hdTelescope_animationSeek(mpLayout->animSet(),2,1,gabi::fmuls_ppc(scale,10.0f));
    f32 previous = mScale;
    if (scale != previous) {
        if (scale == 0.0f || scale == 8.0f) {
            hdTelescope_playSound(0x809);
        } else {
            f32 difference = gabi::fsubs_ppc(scale,previous);
            f32 volume = gabi::fmuls_ppc(32767.0f,__builtin_fabsf(difference));
            hdTelescope_playScaleSound(8,0,telescopeUnsignedFloat(volume));
        }
    }
    mScale = scale;
    hdTelescope_getPlay()->mSoundMode = 7;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sTelescopeScaleTextVtable 1010749C
 data sTelescopeScaleFormat 1010756C
 data sTelescopeOut 1049CA38
 data sTelescopeCoverOut 1049CA68
 ext 02759D3C void hdTelescope_formatScale(hdTelescopeScaleText_l*,const be<u16>*,u32,u32)
 ext 025E1A04 void hdTelescope_playScaleSound(u32,u32,u32)
 data sTelescopePaneNamesReady 1049C940
 data sTelescopeStringVtable 1010746C
 str 10107590 "N_All_00" in 026E8534
 str 10107584 "N_Zoom_00" in 026E8534
 data sTelescopePaneNameRegistration 101F6F40
 ext 028F026C void hdTelescope_registerGlobal(void*)
 data sTelescopeStateInterfaceVtable 10107918
 data sTelescopeDrawInterfaceVtable 10107928
 data sTelescopePaneNames 1049C8C8
 data sTelescopeScaleName 1049C8AC
 data sTelescopeMessage 101F4B5C
 data sTelescopeEvent 101D600C
 data sTelescopeZoomOut 1049CB58
 data sTelescopeVtable 10107730
 data sTelescopeWait 1049C948
 data sTelescopeIn 1049C978
 data sTelescopeDemoIn 1049C9A8
 data sTelescopeDemoWait 1049C9D8
 data sTelescopeNormal 1049CA08
 data sTelescopeCoverWait 1049CAC8
 data sTelescopeZoom 1049CAF8
 data sTelescopeZoomEnd 1049CB28
 ext 02515280 void hdTelescope_trimCamera(void*,u32)
 ext 0273AD10 void* hdTelescope_allocate(u32)

 ext 025200D4 hdTelescopePlay_l* hdTelescope_getPlay()
 ext 025F74D0 void hdTelescope_setMessageMode(hdTelescopeMessage_l*,u32)
 ext 0273AF40 void hdTelescope_delete(void*)

 ext 028F0164 void hdTelescope_destroyStrings(void*,u32,u32,u32,u32,u32)

 ext 02005708 void hdTelescope_animationStop(hdAnimSet*,u32,u32)
 ext 0200579C void hdTelescope_animationSeek(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 hdTelescope_animationEnded(hdAnimSet*,u32)
 ext 025E1988 void hdTelescope_playSound(u32)

 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
*/
