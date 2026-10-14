/* HD GameOverChar, reconstructed from the HD binary. */
#include "d/hd_screen_game_over_char.h"
#include "d/d_com_inf_game.h"
#include "d/hd_parts_text_pane.h"
extern pa::JPABaseEmitter* hdGameOverCharCreateParticle(dPa_control_c*, u32, u32, pa::EmitterVector*, void*, void*, u32, void*, s32, void*, void*, void*);
extern void hdCharUpdateMessage(hdWindowPart_c*);
extern u32 hdCharGlyphTriggered(hdWindowPart_c*, hdUiSafeString*, u32);
extern void hdCharGlyphPosition(pa::EmitterVector*, hdWindowPart_c*, u32);
extern u32 hdCharMarkerTriggered(hdWindowPart_c*, hdUiSafeString*);
extern void hdCharResetPane(hdTextPane_c*, hdUiSafeString*, hdUiSafeString*);
extern hdWindowPart_c* hdCharConstructWindow(hdWindowPart_c*, f32);
extern void hdCharSetupWindow(hdWindowPart_c*, void*, hdPartsLyt_c*, u32, u32);
extern void hdCharCreateConfig(hdWindowPart_c*, hdUiSafeString*, hdScreenHeap*);
extern void hdCharConstructConfig(hdWindowConfig_l*);
extern void hdCharCopyConfig(hdWindowPart_c*, hdUiSafeString*, hdWindowConfig_l*);
extern void hdCharDestroyConfig(hdWindowConfig_l*, u32);
extern void hdCharSelectConfig(hdWindowPart_c*, hdUiSafeString*);
static u32 sScreenVtable;
static u32 sStateHolderVtable;
static u32 sSecondaryVtable;
static u32 sCallbackStateVtable;
static u32 sStringVtable;
static hdGameOverCharCallbackState_l sGameOverCharDelayState;
static hdGameOverCharCallbackState_l sGameOverCharOutState;
static hdGameOverCharCallbackState_l sGameOverCharCompleteState;
static hdGameOverCharCallbackState_l sGameOverCharInState;
extern void hdGameOverCharDestroyBase(hdParts_c*, u32);
extern void hdGameOverCharConstructBase(hdParts_c*);
extern hdGameOverChar_c* hdGameOverCharAllocate(u32);
extern void hdGameOverCharDelete(void*);
extern void hdGameOverCharPlayMessage(hdWindowPart_c*, hdUiSafeString*);
extern u32 hdGameOverCharMessageFinished(hdWindowPart_c*, hdUiSafeString*);
typedef gptr<hdScreenUiSystem> hdCharUiPointer;
static hdCharUiPointer sCharUiSystem;
static hdGameOverCharRuntime sCharRuntime;
static hdGameOverCharRegistration sCharRuntimeRegistration;
static hdGameOverCharRegistration sCharFirstRegistration;
static hdGameOverCharRegistration sCharSecondRegistration;
static hdGameOverCharContext sCharFirstContext;
static hdGameOverCharContext sCharSecondContext;
static f32 sCharMinimumAngle;
static f32 sCharMaximumAngle;
static hdUiSafeString sCharLayoutName;
static hdUiSafeString sCharTextName;
static hdUiSafeString sCharQualifiedTextName;
static hdPartsStateCounter_c sCharCounterReady;
static hdPartsStateCounter_c sCharNextState;
extern void hdCharRegister(hdGameOverCharRegistration*);
extern void hdCharConstructFirst(hdGameOverCharContext*);
extern void hdCharConstructSecond(hdGameOverCharContext*);
extern void hdCharInEnter(hdGameOverChar_c*);
extern void hdCharInMove(hdGameOverChar_c*);
extern void hdCharInLeave(hdGameOverChar_c*);
extern u32 hdCharInDone(hdGameOverChar_c*);
extern void hdCharDelayEnter(hdGameOverChar_c*);
extern void hdCharDelayMove(hdGameOverChar_c*);
extern void hdCharDelayLeave(hdGameOverChar_c*);
extern u32 hdCharDelayDone(hdGameOverChar_c*);
extern void hdCharOutEnter(hdGameOverChar_c*);
extern void hdCharOutMove(hdGameOverChar_c*);
extern void hdCharOutLeave(hdGameOverChar_c*);
extern u32 hdCharOutDone(hdGameOverChar_c*);
extern void hdCharCompleteEnter(hdGameOverChar_c*);
extern void hdCharCompleteMove(hdGameOverChar_c*);
extern void hdCharCompleteLeave(hdGameOverChar_c*);
extern u32 hdCharCompleteDone(hdGameOverChar_c*);
/* 0266B768 */
u32 hdGameOverCharStateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 0266B868 */
void hdGameOverChar_c::destroy(hdGameOverChar_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    hdGameOverCharDestroyBase(self, 0);
    if (flags & 1) hdGameOverCharDelete(self);
}
/* 0266B8D4 */
void hdGameOverChar_c::inEnter() {
    gabi::LocalOf<hdUiSafeString> name;
    name->mText.v = gabi::guest_addr("In");
    name->mVtable.v = gabi::ea(&sStringVtable);
    hdGameOverCharPlayMessage(mpMessage, name);
    mFinished = 0;
    mVisible = 0;
}
/* 0266B92C */
void hdGameOverChar_c::inMove() {
    if (!mpMessage->mScheduling) changeState(sGameOverCharDelayState.stateId());
}
/* 0266B94C */
void hdGameOverChar_c::delayEnter() {
    mDelay = 90;
}
/* 0266B958 */
void hdGameOverChar_c::delayMove() {
    const u32 delay = mDelay;
    if (!delay) changeState(sGameOverCharOutState.stateId());
    else mDelay = delay - 1;
}
/* 0266B980 */
void hdGameOverChar_c::outEnter() {
    gabi::LocalOf<hdUiSafeString> name;
    name->mText.v = gabi::guest_addr("Out");
    name->mVtable.v = gabi::ea(&sStringVtable);
    hdGameOverCharPlayMessage(mpMessage, name);
}
/* 0266B9C0 */
void hdGameOverChar_c::outMove() {
    gabi::LocalOf<hdUiSafeString> name;
    name->mText.v = gabi::guest_addr("Out");
    name->mVtable.v = gabi::ea(&sStringVtable);
    if (hdGameOverCharMessageFinished(mpMessage, name) != 1) changeState(sGameOverCharCompleteState.stateId());
}
/* 0266BA28 */
void hdGameOverChar_c::completeEnter() {
    mFinished = 1;
}
/* 0266BA34 */
void hdGameOverChar_c::waitEnter() {
    mRequested = 0;
}
/* 0266BA40 */
void hdGameOverChar_c::waitMove() {
    if (mRequested) changeState(sGameOverCharInState.stateId());
}
/* 0266BA5C */
hdGameOverChar_c* hdGameOverChar_c::construct(hdGameOverChar_c* self) {
    if (!self) {
        self = hdGameOverCharAllocate(sizeof(hdGameOverChar_c));
        if (!self) return nullptr;
    }
    hdGameOverCharConstructBase(self);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    self->mVtable.v = gabi::ea(&sScreenVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    self->mpMessage = nullptr;
    self->mVisible = 0;
    self->mRequested = 0;
    self->mpFirstEmitter = nullptr;
    self->mpCharacterLayout = nullptr;
    self->mDelay = 0;
    self->mFinished = 0;
    self->mpSecondEmitter = nullptr;
    return self;
}
/* 0266BD54 */
void hdGameOverChar_c::releaseResources() {
    hdWindowPart_c* message = mpMessage;
    if (message) {
        gabi::call_ptr(reinterpret_cast<hdGameOverMessageVtable_l*>(message->mVtable.get())->mDelete.get(), message, 3);
        mpMessage = nullptr;
    }
    hdPartsLayout_c* layout = mpCharacterLayout;
    if (layout) {
        layout->release();
        layout = mpCharacterLayout;
        if (layout) layout->destroy(3);
        mpCharacterLayout = nullptr;
    }
}
/* 0266BDEC */
void hdGameOverChar_c::spawnCharacterEffect(pa::EmitterVector* position) {
    gabi::LocalOf<pa::EmitterVector> copy;
    const f32 x = position->x;
    const f32 y = position->y;
    copy->x = x;
    const f32 z = position->z;
    copy->y = y;
    copy->z = z;
    dPa_control_c* particles = dComIfGp_getParticle();
    hdGameOverCharCreateParticle(particles, 7, 46, copy, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
/* 0266BE5C */
void hdGameOverChar_c::spawnFirstEffect(pa::EmitterVector* position) {
    gabi::LocalOf<pa::EmitterVector> copy;
    const f32 x = position->x;
    const f32 y = position->y;
    copy->x = x;
    const f32 z = position->z;
    copy->y = y;
    copy->z = z;
    dPa_control_c* particles = dComIfGp_getParticle();
    mpFirstEmitter = hdGameOverCharCreateParticle(particles, 9, 47, copy, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
/* 0266BEDC */
void hdGameOverChar_c::spawnSecondEffect(pa::EmitterVector* position) {
    gabi::LocalOf<pa::EmitterVector> copy;
    const f32 x = position->x;
    const f32 y = position->y;
    copy->x = x;
    const f32 z = position->z;
    copy->y = y;
    copy->z = z;
    dPa_control_c* particles = dComIfGp_getParticle();
    mpSecondEmitter = hdGameOverCharCreateParticle(particles, 9, 48, copy, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
/* 0266BF5C */
void hdGameOverChar_c::stopFirstEffect() {
    pa::JPABaseEmitter* emitter = mpFirstEmitter;
    if (emitter) emitter->mFlags = u32(emitter->mFlags) & ~4u;
}
/* 0266BF78 */
void hdGameOverChar_c::stopSecondEffect() {
    pa::JPABaseEmitter* emitter = mpSecondEmitter;
    if (emitter) emitter->mFlags = u32(emitter->mFlags) & ~4u;
}
/* 0266BF94 */
void hdGameOverChar_c::finishFirstEffect() {
    pa::JPABaseEmitter* emitter = mpFirstEmitter;
    if (emitter) {
        const u32 flags = emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags | 1;
    }
}
/* 0266BFB8 */
void hdGameOverChar_c::finishSecondEffect() {
    pa::JPABaseEmitter* emitter = mpSecondEmitter;
    if (emitter) {
        const u32 flags = emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags | 1;
    }
}
/* 0266BAEC */
u32 hdGameOverChar_c::initializeCharacter(hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* context) {
    mpResAccessor = accessor;
    hdScreenHeap* heap = sCharUiSystem->mHeap;
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpCharacterLayout = layout;
    layout->vLoad(&sCharLayoutName, gabi::ea(mpResAccessor.get()), resource, option, 0, 0, context);
    hdPartsLyt_c* lyt = mpCharacterLayout->lyt();
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(lyt->mpRootPane.get());
    hdPartsPaneSRTVtbl_c* rootVtable = root->srtVtbl();
    sCharTextName.assureTermination();
    auto* text = gabi::call_ptr<hdTextPane_c*>(rootVtable->mFindPane.get(), root, sCharTextName.mText.get(), 1);
    lyt = mpCharacterLayout->lyt();
    gabi::LocalOf<hdGameOverCharLoadLocals> locals;
    locals->mActiveName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mActiveName.mText.v = hdButtonResourceName(lyt->mpResourceAccessor);
    hdCharResetPane(text, &locals->mActiveName, &sCharQualifiedTextName);
    auto* message = static_cast<hdWindowPart_c*>(hdParts_new(sizeof(hdWindowPart_c), heap, 4));
    if (message) message = hdCharConstructWindow(message, 5.0f);
    mpMessage = message;
    hdCharSetupWindow(message, context, mpCharacterLayout->lyt(), resource, option);
    locals->mActiveName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mActiveName.mText.v = gabi::guest_addr("In");
    hdCharCreateConfig(mpMessage, &locals->mActiveName, heap);
    locals->mActiveName.mText.v = gabi::guest_addr("Out");
    locals->mActiveName.mVtable.v = gabi::ea(&sStringVtable);
    hdCharCreateConfig(mpMessage, &locals->mActiveName, heap);
    hdCharConstructConfig(&locals->mConfig);
    locals->mInName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mConfig.mResetLayout = 0;
    locals->mConfig.mDuration = 0x39;
    locals->mConfig.mMode = 1;
    locals->mConfig.mFinishEnabled = 0;
    locals->mInName.mText.v = gabi::guest_addr("In");
    locals->mConfig.mOrder = 4;
    hdCharCopyConfig(mpMessage, &locals->mInName, &locals->mConfig);
    hdCharDestroyConfig(&locals->mConfig, 2);
    hdCharConstructConfig(&locals->mConfig);
    locals->mConfig.mResetLayout = 0;
    locals->mOutName.mText.v = gabi::guest_addr("Out");
    locals->mOutName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mConfig.mOrder = 4;
    locals->mConfig.mFinishEnabled = 0;
    locals->mConfig.mMode = 3;
    locals->mConfig.mDuration = 3;
    hdCharCopyConfig(mpMessage, &locals->mOutName, &locals->mConfig);
    hdCharDestroyConfig(&locals->mConfig, 2);
    locals->mActiveName.mText.v = gabi::guest_addr("In");
    locals->mActiveName.mVtable.v = gabi::ea(&sStringVtable);
    hdCharSelectConfig(mpMessage, &locals->mActiveName);
    startState(sGameOverCharCompleteState.stateId());
    return 1;
}
/* 0266BFDC */
void hdGameOverChar_c::executeCharacter() {
    hdCharUpdateMessage(mpMessage);
    hdParts_executeStateMachine(stateMachine());
    hdWindowPart_c* message = mpMessage;
    const u32 count = message->mpTextPane->mCharacterCount;
    gabi::LocalOf<hdGameOverCharEffectLocals> locals;
    for (u32 i = 0; i < count; ++i) {
        locals->mGlyphName.mVtable.v = gabi::ea(&sStringVtable);
        locals->mGlyphName.mText.v = gabi::guest_addr("In");
        if (hdCharGlyphTriggered(message, &locals->mGlyphName, i)) {
            hdCharGlyphPosition(&locals->mSampled, mpMessage, i);
            spawnCharacterEffect(&locals->mSampled);
        }
        message = mpMessage;
    }
    locals->mMarkerName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mMarkerName.mText.v = gabi::guest_addr("In");
    if (hdCharMarkerTriggered(message, &locals->mMarkerName)) {
        locals->mOrigin.z = 0.0f;
        locals->mOrigin.x = 0.0f;
        locals->mOrigin.y = 0.0f;
        spawnFirstEffect(&locals->mOrigin);
        locals->mOrigin.y = 0.0f;
        locals->mOrigin.x = 0.0f;
        locals->mOrigin.z = 0.0f;
        spawnSecondEffect(&locals->mOrigin);
        mVisible = 1;
        stopFirstEffect();
        stopSecondEffect();
    } else if (mVisible) {
        stopFirstEffect();
        stopSecondEffect();
    }
    message = mpMessage;
    locals->mMarkerName.mVtable.v = gabi::ea(&sStringVtable);
    locals->mMarkerName.mText.v = gabi::guest_addr("Out");
    if (hdCharMarkerTriggered(message, &locals->mMarkerName)) {
        finishFirstEffect();
        finishSecondEffect();
        mVisible = 0;
    }
}
/* Direct callback delegates are the original game state variant (no trailing null-state). */
static void setCharDelegate(hdPartsPtmf_c& delegate, const void* function) {
    delegate.mThisDelta = 0;
    delegate.mVirtualIndex = -1;
    delegate.mFunction = gabi::guest_fn(function);
}
/* 0266C154 */
static void initializeGameOverCharStatics() {
    sCharRuntime.mWords[2] = 0;
    sCharRuntime.mWords[0] = 0;
    sCharRuntime.mWords[3] = 0;
    sCharRuntime.mWords[1] = 0;
    hdCharRegister(&sCharRuntimeRegistration);
    sCharMinimumAngle = -3.1415927410125732f;
    sCharMaximumAngle = 3.1415927410125732f;
    hdCharConstructFirst(&sCharFirstContext);
    hdCharRegister(&sCharFirstRegistration);
    hdCharConstructSecond(&sCharSecondContext);
    hdCharRegister(&sCharSecondRegistration);
    sCharLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sCharLayoutName.mText.v = gabi::guest_addr("GameOverChar_00");
    sCharTextName.mVtable.v = gabi::ea(&sStringVtable);
    sCharTextName.mText.v = gabi::guest_addr("T_Text_00");
    sCharQualifiedTextName.mVtable.v = gabi::ea(&sStringVtable);
    sCharQualifiedTextName.mText.v = gabi::guest_addr("L_GameOver_00-T_Text_00");
    u32 number;
    if (sCharCounterReady.mValue) number = sCharNextState.mValue;
    else { sCharCounterReady.mValue = 1; number = 0; }
    sGameOverCharInState.mNumber = ++number;
    sGameOverCharInState.mName.v = gabi::guest_addr("::StateID_In");
    sGameOverCharInState.mVtable.v = gabi::ea(&sCallbackStateVtable);
    setCharDelegate(sGameOverCharInState.mEnter, reinterpret_cast<const void*>(hdCharInEnter));
    setCharDelegate(sGameOverCharInState.mMove, reinterpret_cast<const void*>(hdCharInMove));
    setCharDelegate(sGameOverCharInState.mLeave, reinterpret_cast<const void*>(hdCharInLeave));
    setCharDelegate(sGameOverCharInState.mDone, reinterpret_cast<const void*>(hdCharInDone));
    sGameOverCharDelayState.mNumber = ++number;
    sGameOverCharDelayState.mName.v = gabi::guest_addr("::StateID_Wait");
    sGameOverCharDelayState.mVtable.v = gabi::ea(&sCallbackStateVtable);
    setCharDelegate(sGameOverCharDelayState.mEnter, reinterpret_cast<const void*>(hdCharDelayEnter));
    setCharDelegate(sGameOverCharDelayState.mMove, reinterpret_cast<const void*>(hdCharDelayMove));
    setCharDelegate(sGameOverCharDelayState.mLeave, reinterpret_cast<const void*>(hdCharDelayLeave));
    setCharDelegate(sGameOverCharDelayState.mDone, reinterpret_cast<const void*>(hdCharDelayDone));
    sGameOverCharOutState.mNumber = ++number;
    sGameOverCharOutState.mName.v = gabi::guest_addr("::StateID_Out");
    sGameOverCharOutState.mVtable.v = gabi::ea(&sCallbackStateVtable);
    setCharDelegate(sGameOverCharOutState.mEnter, reinterpret_cast<const void*>(hdCharOutEnter));
    setCharDelegate(sGameOverCharOutState.mMove, reinterpret_cast<const void*>(hdCharOutMove));
    setCharDelegate(sGameOverCharOutState.mLeave, reinterpret_cast<const void*>(hdCharOutLeave));
    setCharDelegate(sGameOverCharOutState.mDone, reinterpret_cast<const void*>(hdCharOutDone));
    sGameOverCharCompleteState.mNumber = ++number;
    sGameOverCharCompleteState.mName.v = gabi::guest_addr("::StateID_Sleep");
    sGameOverCharCompleteState.mVtable.v = gabi::ea(&sCallbackStateVtable);
    sCharNextState.mValue = number;
    setCharDelegate(sGameOverCharCompleteState.mEnter, reinterpret_cast<const void*>(hdCharCompleteEnter));
    setCharDelegate(sGameOverCharCompleteState.mMove, reinterpret_cast<const void*>(hdCharCompleteMove));
    setCharDelegate(sGameOverCharCompleteState.mLeave, reinterpret_cast<const void*>(hdCharCompleteLeave));
    setCharDelegate(sGameOverCharCompleteState.mDone, reinterpret_cast<const void*>(hdCharCompleteDone));
}
/* WWHD glue
data sScreenVtable 100F5090 nocheck
data sStateHolderVtable 100F5158 nocheck
data sSecondaryVtable 100F5168 nocheck
data sCallbackStateVtable 100F4F9C nocheck
data sStringVtable 100F4F3C nocheck
data sCharUiSystem 1018C404
ext 027045B0 void hdCharResetPane(hdTextPane_c* pane, hdUiSafeString* message, hdUiSafeString* label)
ext 026F31CC hdWindowPart_c* hdCharConstructWindow(hdWindowPart_c* self, f32 interval)
ext 026F4174 void hdCharSetupWindow(hdWindowPart_c* self, void* context, hdPartsLyt_c* layout, u32 resource, u32 option)
ext 026F459C void hdCharCreateConfig(hdWindowPart_c* self, hdUiSafeString* name, hdScreenHeap* heap)
ext 026F43DC void hdCharConstructConfig(hdWindowConfig_l* config)
ext 026F55DC void hdCharCopyConfig(hdWindowPart_c* self, hdUiSafeString* name, hdWindowConfig_l* config)
ext 026F30D4 void hdCharDestroyConfig(hdWindowConfig_l* config, u32 flags)
ext 026F5344 void hdCharSelectConfig(hdWindowPart_c* self, hdUiSafeString* name)
str 100F5000 "In" in 0266BAEC
str 100F4FFC "Out" in 0266BAEC

ext 026F4FD8 void hdCharUpdateMessage(hdWindowPart_c* message)
ext 026F5410 u32 hdCharGlyphTriggered(hdWindowPart_c* message, hdUiSafeString* name, u32 index)
ext 026F53A8 void hdCharGlyphPosition(pa::EmitterVector* position, hdWindowPart_c* message, u32 index)
ext 026F549C u32 hdCharMarkerTriggered(hdWindowPart_c* message, hdUiSafeString* name)
str 100F500C "In" in 0266BFDC
str 100F5008 "Out" in 0266BFDC
data sCharRuntime 104943B8
data sCharRuntimeRegistration 101F5F68
data sCharFirstRegistration 101F5F74
data sCharSecondRegistration 101F5F80
data sCharFirstContext 104943B4
data sCharSecondContext 104943B5
data sCharMinimumAngle 104943AC
data sCharMaximumAngle 104943B0
data sCharLayoutName 104943C8
data sCharTextName 1049439C
data sCharQualifiedTextName 104943A4
data sCharCounterReady 101FD8E4
data sCharNextState 101FDD50
str 100F5018 "GameOverChar_00" in 0266C154
str 100F5050 "T_Text_00" in 0266C154
str 100F5028 "L_GameOver_00-T_Text_00" in 0266C154
str 100F506C "::StateID_In" in 0266C154
str 100F507C "::StateID_Wait" in 0266C154
str 100F505C "::StateID_Out" in 0266C154
str 100F5040 "::StateID_Sleep" in 0266C154
ext 028F026C void hdCharRegister(hdGameOverCharRegistration* registration)
ext 028ED6F8 void hdCharConstructFirst(hdGameOverCharContext* context)
ext 028EAB2C void hdCharConstructSecond(hdGameOverCharContext* context)
ext 0266B8D4 void hdCharInEnter(hdGameOverChar_c* self) callback
ext 0266B92C void hdCharInMove(hdGameOverChar_c* self) callback
ext 0266C444 void hdCharInLeave(hdGameOverChar_c* self) callback
ext 0266C448 u32 hdCharInDone(hdGameOverChar_c* self) callback
ext 0266B94C void hdCharDelayEnter(hdGameOverChar_c* self) callback
ext 0266B958 void hdCharDelayMove(hdGameOverChar_c* self) callback
ext 0266C450 void hdCharDelayLeave(hdGameOverChar_c* self) callback
ext 0266C454 u32 hdCharDelayDone(hdGameOverChar_c* self) callback
ext 0266B980 void hdCharOutEnter(hdGameOverChar_c* self) callback
ext 0266B9C0 void hdCharOutMove(hdGameOverChar_c* self) callback
ext 0266BA28 void hdCharOutLeave(hdGameOverChar_c* self) callback
ext 0266C45C u32 hdCharOutDone(hdGameOverChar_c* self) callback
ext 0266BA34 void hdCharCompleteEnter(hdGameOverChar_c* self) callback
ext 0266BA40 void hdCharCompleteMove(hdGameOverChar_c* self) callback
ext 0266C464 void hdCharCompleteLeave(hdGameOverChar_c* self) callback
ext 0266C468 u32 hdCharCompleteDone(hdGameOverChar_c* self) callback

ext 025A847C pa::JPABaseEmitter* hdGameOverCharCreateParticle(dPa_control_c* particles, u32 group, u32 id, pa::EmitterVector* position, void* angle, void* scale, u32 alpha, void* callback, s32 setup, void* primary, void* environment, void* scale2D)
data sGameOverCharDelayState 104943FC
data sGameOverCharOutState 10494428
data sGameOverCharCompleteState 10494454
data sGameOverCharInState 104943D0
str 100F4FEC "In" in 0266B8D4
str 100F4FF0 "Out" in 0266B980
str 100F4FF4 "Out" in 0266B9C0
ext 026F88F0 void hdGameOverCharDestroyBase(hdParts_c* screen, u32 flags)
ext 026F89E8 void hdGameOverCharConstructBase(hdParts_c* screen)
ext 0273AD10 hdGameOverChar_c* hdGameOverCharAllocate(u32 size)
ext 0273AF40 void hdGameOverCharDelete(void* screen)
ext 026F5258 void hdGameOverCharPlayMessage(hdWindowPart_c* message, hdUiSafeString* name)
ext 026F5534 u32 hdGameOverCharMessageFinished(hdWindowPart_c* message, hdUiSafeString* name)
*/
