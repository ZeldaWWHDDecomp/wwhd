/** HD button-part event routing, receiver ownership and cursor/touch sounds. */
#include "d/hd_parts_button.h"
static u32 sButtonPartVtable;
static u32 sButtonInterfaceVtable;
static u32 sButtonButtonVtable;
static u32 sButtonStringVtable;
static hdUiSafeString sButtonTouchSound;
static hdUiSafeString sButtonCursorSound;
static hdButtonEventManager_l* sButtonEventManager;
static hdScreenUiSystem* sButtonHeapManager;
static hdButtonInputMode_l* sButtonInputMode;
static hdButtonMenu_l* sButtonMenu;
static u8 sButtonTouchReleasePending;
void hdButton_baseDestroy(hdButtonPart_c*, u32);
void hdButton_baseConstruct(hdButtonPart_c*);
u32 hdButton_getFocus(hdButtonEventManager_l*);
void hdButton_registerReceiver(hdButtonEventManager_l*, hdButtonReceiver_l*);
u32 hdButton_receiverButton(hdButtonReceiver_l*, u32, u32);
void* hdButton_allocate(u32, void*, u32);
hdButtonReceiver_l* hdButton_receiverConstruct(hdButtonReceiver_l*, u32, u32);
void hdButton_setFocus(hdButtonEventManager_l*, hdButtonReceiver_l*);
void hdButton_setEvent(hdButtonEventManager_l*, hdButtonReceiver_l*, u32);
u32 hdButton_getMode(hdButtonEventManager_l*);
u32 hdButton_getSuppressedButton(hdButtonEventManager_l*);
u32 hdButton_getSuppressedType(hdButtonEventManager_l*);
u32 hdButton_receiverDispatch(hdButtonReceiver_l*, u32);
void hdButton_unlinkEvent(hdButtonEventLink_l*);
void hdButton_playSoundName(be<u8>*);
void hdButton_playSoundId(u32);
void hdButton_deleteConfig(void*, hdUiSafeString*, void*);
static u32 hdButton_buttonIndex(u32);
static BOOL hdButton_canDispatch(u32);
static u32 hdButton_getSuppression(u32);
static u32 hdButton_dispatch(hdButtonReceiver_l*, u32);
static BOOL hdButton_inputEnabled();
/* 026F5788 */
static void hdButton_deleteConfigNode(hdButtonConfigDeleteContext_l* context,
                                      hdButtonConfigTreeNode_l* node) {
    void* config = node->mpConfig;
    hdButton_deleteConfig(context->mpOwner, &node->mKey, config);
}
/* 026F59F4 */
void hdButtonPart_c::destroy(hdButtonPart_c* part, u32 flags) {
    if (!part)
        return;
    part->mStateHolder.mHolderVtable = &sButtonPartVtable;
    part->mSecondaryVtable = &sButtonInterfaceVtable;
    hdButton_baseDestroy(part, 0);
    if (flags & 1)
        operator_delete(part);
}
/* 026F5A60 */
static hdButtonPart_c* hdButtonPart_c__ct(hdButtonPart_c* part) {
    if (!part) {
        part = static_cast<hdButtonPart_c*>(operator_new(sizeof(hdButtonPart_c)));
        if (!part)
            return NULL;
    }
    hdButton_baseConstruct(part);
    part->mVtable.v = gabi::ea(&sButtonButtonVtable);
    part->mStateHolder.mHolderVtable = &sButtonPartVtable;
    part->mSecondaryVtable = &sButtonInterfaceVtable;
    part->mpReceiver = NULL;
    part->mTouchEnabled = 0;
    part->mEnableText = 1;
    part->mEnablePane = 1;
    part->mReserved = 0;
    part->mAcceptedButtons = 1;
    // GHS tests the wrapped guest address of each placement-new member.
    hdUiSafeString* sound = gabi::at<hdUiSafeString>(gabi::ea(part) + offsetof(hdButtonPart_c, mTouchSound));
    if (!sound)
        sound = static_cast<hdUiSafeString*>(operator_new(sizeof(hdUiSafeString)));
    if (sound) {
        sound->mText.v = gabi::guest_addr("");
        sound->mVtable.v = gabi::ea(&sButtonStringVtable);
    }
    sound = gabi::at<hdUiSafeString>(gabi::ea(part) + offsetof(hdButtonPart_c, mCursorSound));
    if (!sound)
        sound = static_cast<hdUiSafeString*>(operator_new(sizeof(hdUiSafeString)));
    if (sound) {
        sound->mText.v = gabi::guest_addr("");
        sound->mVtable.v = gabi::ea(&sButtonStringVtable);
    }
    part->mCursorSoundId = 0;
    part->mTouchSoundId = 0;
    part->mTouchSound.mText = sButtonTouchSound.mText;
    part->mCursorSound.mText = sButtonCursorSound.mText;
    return part;
}
/* 026F5B74 */
u32 hdButtonPart_c::getReceiverId() {
    return mpReceiver->mId;
}
/* 026F5B80 */
void hdButtonPart_c::registerReceiver() {
    if (!sButtonEventManager || !mpReceiver)
        return;
    if (hdButton_getFocus(sButtonEventManager)) {
        u32 focus = hdButton_getFocus(sButtonEventManager);
        if (focus != getReceiverId())
            return;
    }
    hdButton_registerReceiver(sButtonEventManager, mpReceiver);
}
/* 026F5C00 */
u32 hdButtonPart_c::setReceiverButton(u32 button, u32 value) {
    return hdButton_receiverButton(mpReceiver, button, value);
}
/* 026F5C08 */
void hdButtonPart_c::setReceiverButtons(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h) {
    hdButton_receiverButton(mpReceiver, 0, a);
    hdButton_receiverButton(mpReceiver, 1, b);
    hdButton_receiverButton(mpReceiver, 2, c);
    hdButton_receiverButton(mpReceiver, 3, d);
    hdButton_receiverButton(mpReceiver, 4, e);
    hdButton_receiverButton(mpReceiver, 5, f);
    hdButton_receiverButton(mpReceiver, 6, g);
    hdButton_receiverButton(mpReceiver, 7, h);
}
/* 026F5CCC */
void hdButtonPart_c::setPaneEnabled(u32 value) {
    mEnablePane = value;
}
/* 026F5CD4 */
void hdButtonPart_c::setTextEnabled(u32 value) {
    mEnableText = value;
}
/* 026F61CC */
static BOOL hdButton_isDecide(u32 event) {
    return event == HD_BUTTON_EVENT_DECIDE_e;
}
/* 026F61DC */
static BOOL hdButton_isCancel(u32 event) {
    return event == HD_BUTTON_EVENT_CANCEL_e;
}
/* 026F61EC */
static BOOL hdButton_isMenu(u32 event) {
    return event == HD_BUTTON_EVENT_MENU_e ? ((u32(sButtonMenu->mFlags) >> 15) & 1) : FALSE;
}
/* 026F620C */
static BOOL hdButton_isSpecial(u32 event) {
    return event == HD_BUTTON_EVENT_SPECIAL_e;
}
/* 026F621C */
static BOOL hdButton_canDispatch(u32 event) {
    if (hdButton_getFocus(sButtonEventManager))
        return FALSE;
    return u32(event - 5) <= 7;
}
/* 026F6288 */
static BOOL hdButton_isTouchBegin(hdButtonEvent_l* event) {
    if (hdButton_getFocus(sButtonEventManager))
        return FALSE;
    u32 type = event->mType;
    u8 button = event->mButton;
    return button == 0 && type == HD_BUTTON_EVENT_TOUCH_e;
}
/* 026F630C */
static BOOL hdButton_isTouchEnd(hdButtonEvent_l* event) {
    u32 type = event->mType;
    u8 button = event->mButton;
    return button == 0 && type == HD_BUTTON_EVENT_TOUCH_END_e;
}
/* 026F6340 */
static BOOL hdButton_isAlternateTouch(hdButtonEvent_l* event) {
    if (hdButton_getFocus(sButtonEventManager))
        return FALSE;
    u32 type = event->mType;
    u8 button = event->mButton;
    return button == 1 && type == HD_BUTTON_EVENT_TOUCH_e;
}
static const u8 sButtonButtonRemap[8] = {3, 5, 4, 0, 2, 1, 7, 6};
/* 026F63C4 */
static u32 hdButton_remap(u32 index) {
    return index <= 7 ? sButtonButtonRemap[index] : 8;
}
/* 026F63E0 */
static u32 hdButton_buttonIndex(u32 event) {
    return event - 5;
}
/* 026F63E8 */
static u32 hdButton_getSuppression(u32 button) {
    u32 suppressed = hdButton_getSuppressedButton(sButtonEventManager);
    u32 type = hdButton_getSuppressedType(sButtonEventManager);
    if (suppressed && hdButton_remap(button) == type)
        return suppressed;
    return 0;
}
/* 026F6478 */
static u32 hdButton_dispatchReceiver(hdButtonReceiver_l* receiver, u32 button) {
    return hdButton_receiverDispatch(receiver, button);
}
/* 026F647C */
static u32 hdButton_dispatch(hdButtonReceiver_l* receiver, u32 event) {
    if (!receiver || !hdButton_canDispatch(event))
        return 0;
    u32 button = hdButton_buttonIndex(event);
    u32 suppressed = hdButton_getSuppression(button);
    if (suppressed)
        return suppressed;
    return hdButton_dispatchReceiver(receiver, button);
}
/* 026F6504 */
static void hdButton_markTouchRelease() {
    sButtonTouchReleasePending = 1;
}
/* 026F6514 */
static void hdButton_clearTouchRelease() {
    sButtonTouchReleasePending = 0;
}
/* 026F6524 */
void hdButtonPart_c::setTouchSound(hdUiSafeString* sound, u32 id) {
    u32 text = sound->mText.v;
    mTouchSoundId = id;
    mTouchSound.mText.v = text;
}
/* 026F6534 */
void hdButtonPart_c::setCursorSound(hdUiSafeString* sound, u32 id) {
    u32 text = sound->mText.v;
    mCursorSoundId = id;
    mCursorSound.mText.v = text;
}
/* 026F6544 */
BOOL hdButtonPart_c::createReceiver(u32 type, u32 argument) {
    hdButtonReceiver_l* receiver = static_cast<hdButtonReceiver_l*>(
        hdButton_allocate(sizeof(hdButtonReceiver_l), sButtonHeapManager->mHeap, 4));
    if (receiver)
        receiver = hdButton_receiverConstruct(receiver, type, argument);
    mpReceiver = receiver;
    return receiver != NULL;
}
/* 026F65BC */
void hdButtonPart_c::destroyReceiver() {
    hdButtonReceiver_l* receiver = mpReceiver;
    if (receiver) {
        gabi::call_ptr<void>(receiver->mVtable->mDestroy.get(), receiver, 3);
        mpReceiver = NULL;
    }
}
/* 026F660C */
u32 hdButtonPart_c::dispatchButton(u32 event) {
    return hdButton_dispatch(mpReceiver, event);
}
/* 026F6614 */
BOOL hdButtonPart_c::requestFocus(u32 event) {
    hdButtonReceiver_l* receiver =
        gabi::call_ptr<hdButtonReceiver_l*>(hdButtonVtable(this)->mAcceptFocus.get(), this, event);
    if (!receiver)
        return FALSE;
    hdButton_setFocus(sButtonEventManager, receiver);
    u32 button = hdButton_buttonIndex(event);
    hdButton_setEvent(sButtonEventManager, mpReceiver, button);
    return TRUE;
}
/* 026F66B0 */
void hdButtonPart_c::clearFocus() {
    hdButtonReceiver_l* receiver = mpReceiver;
    hdButton_setFocus(sButtonEventManager, receiver);
    hdButton_setEvent(sButtonEventManager, NULL, 8);
}
/* 026F66F4 */
void hdButtonPart_c::synchronizeFocus() {
    u32 focused = hdButton_getFocus(sButtonEventManager);
    u32 id = getReceiverId();
    if (focused != id) {
        hdButtonEventManager_l* manager = sButtonEventManager;
        hdButtonEventLink_l* sentinel = &manager->mEvents;
        hdButtonEventLink_l* event = manager->mEvents.mpNext;
        while (event != sentinel) {
            hdButtonEventLink_l* current = event;
            u32 linked = current->mLinked;
            event = current->mpNext;
            if (linked) {
                current->mLinked = 0;
                hdButton_unlinkEvent(current);
                manager->mEvents.mCount = u32(manager->mEvents.mCount) - 1;
            }
        }
        gabi::call_ptr<void>(hdButtonVtable(this)->mOnFocusChanged.get(), this);
        id = getReceiverId();
    }
    sButtonEventManager->mCurrentReceiver = id;
}
/* 026F67A4 */
static BOOL hdButton_inputEnabled() {
    return sButtonInputMode->mEnabled;
}
/* 026F67B4 */
BOOL hdButtonPart_c::isDecideEvent(u32 event) {
    return hdButton_isDecide(event);
}
/* 026F67BC */
BOOL hdButtonPart_c::isCancelEvent(u32 event) {
    return hdButton_isCancel(event);
}
/* 026F67C4 */
BOOL hdButtonPart_c::hasAcceptedButton(u32 button) {
    u32 mask = (button & 0x20) ? 0u : 1u << (button & 31);
    return (u32(mAcceptedButtons) & mask) == mask;
}
/* 026F67E4 */
BOOL hdButtonPart_c::isTouchEvent(hdButtonEvent_l* event) {
    if (hdButton_getFocus(sButtonEventManager))
        return FALSE;
    BOOL accepted = hasAcceptedButton(event->mButton);
    u32 type = event->mType;
    return accepted && type == HD_BUTTON_EVENT_TOUCH_e;
}
/* 026F6880 */
BOOL hdButtonPart_c::isTouchEndEvent(hdButtonEvent_l* event) {
    if (!hdButton_inputEnabled() || !mTouchEnabled)
        return FALSE;
    BOOL accepted = hasAcceptedButton(event->mButton);
    u32 type = event->mType;
    return accepted && type == HD_BUTTON_EVENT_TOUCH_END_e;
}
/* 026F6918 */
void hdButtonPart_c::handleTouchRelease() {
    if (!hdButton_inputEnabled() || !mTouchEnabled)
        return;
    if (hdButton_getMode(sButtonEventManager) != gabi::ea(mpReceiver.get()))
        return;
    if (sButtonTouchReleasePending) {
        gabi::call_ptr<void>(hdButtonVtable(this)->mOnTouchRelease.get(), this);
        sButtonTouchReleasePending = 0;
    }
}
/* 026F69A4 */
static void hdButton_markTouchReleaseFromEvent() {
    sButtonTouchReleasePending = 1;
}
/* 026F69B4 */
void hdButtonPart_c::playTouchSound() {
    if (*mTouchSound.mText) {
        gabi::call_ptr<void>(mTouchSound.mVtable->mAssureTermination.get(), &mTouchSound);
        hdButton_playSoundName(mTouchSound.mText);
    }
    u32 id = mTouchSoundId;
    if (id)
        hdButton_playSoundId(id);
}
/* 026F6A18 */
void hdButtonPart_c::playCursorSound() {
    if (*mCursorSound.mText) {
        gabi::call_ptr<void>(mCursorSound.mVtable->mAssureTermination.get(), &mCursorSound);
        hdButton_playSoundName(mCursorSound.mText);
    }
    u32 id = mCursorSoundId;
    if (id)
        hdButton_playSoundId(id);
}
static u32 sButtonPaneTypeReady;
static u32 sButtonPaneTypeGuard;
static u32 sButtonLayoutPaneTypeGuard;
static u32 sButtonBaseTypeGuard;
static hdScreenType sButtonPaneType;
static hdScreenType sButtonPaneDerivedType;
static hdScreenType sButtonLayoutPaneType;
static hdScreenType sButtonBaseType;
static u32 sButtonTextTypeReady;
static u32 sButtonTextTypeGuard;
static u32 sButtonTextPaneTypeGuard;
static hdScreenType sButtonTextType;
static hdScreenType sButtonTextDerivedType;
static hdScreenType sButtonTextPaneType;
void hdButton_setTextResource(hdButtonPane_l*, hdUiSafeString*, void*);
void hdButton_setTextStyle(hdButtonPane_l*, u32, u32);
static void ensureBaseType() {
    if (!sButtonBaseTypeGuard) {
        sButtonBaseTypeGuard = 1;
        sButtonBaseType.mParent = NULL;
    }
}
static void ensurePaneType() {
    if (!sButtonPaneTypeReady) {
        sButtonPaneTypeReady = 1;
        if (!sButtonPaneTypeGuard) {
            sButtonPaneTypeGuard = 1;
            if (!sButtonLayoutPaneTypeGuard) {
                sButtonLayoutPaneTypeGuard = 1;
                ensureBaseType();
                sButtonLayoutPaneType.mParent = &sButtonBaseType;
            }
            sButtonPaneDerivedType.mParent = &sButtonLayoutPaneType;
        }
        sButtonPaneType.mParent = &sButtonPaneDerivedType;
    }
}
static void ensureTextType() {
    if (!sButtonTextTypeReady) {
        sButtonTextTypeReady = 1;
        if (!sButtonTextTypeGuard) {
            sButtonTextTypeGuard = 1;
            if (!sButtonTextPaneTypeGuard) {
                sButtonTextPaneTypeGuard = 1;
                ensureBaseType();
                sButtonTextPaneType.mParent = &sButtonBaseType;
            }
            sButtonTextDerivedType.mParent = &sButtonTextPaneType;
        }
        sButtonTextType.mParent = &sButtonTextDerivedType;
    }
}
static bool isPaneKind(hdButtonPane_l* pane, hdScreenType* kind) {
    if (!pane)
        return false;
    hdScreenType* type = gabi::call_ptr<hdScreenType*>(pane->mPane.mVtable->mGetRuntimeTypeInfo.get(), pane);
    while (type) {
        if (type == kind)
            return true;
        type = type->mParent;
    }
    return false;
}
/* 026F5CDC */
void hdButtonPart_c::applyPaneFlags() {
    hdButtonPane_l* pane = reinterpret_cast<hdButtonPane_l*>(mpLayout->lyt()->mpRootPane.get());
    ensurePaneType();
    if (!isPaneKind(pane, &sButtonPaneType))
        pane = NULL;
    if (pane)
        pane->mAnimated = mEnablePane;
    setPaneEnabled(0);
    if (pane)
        pane->mFinished = mEnableText;
    setTextEnabled(0);
}
/* 026F5E80 */
void hdButtonPart_c::setText(hdUiSafeString* name, void* text) {
    hdPartsLyt_c* info = mpLayout->lyt();
    gabi::call_ptr<void>(name->mVtable->mAssureTermination.get(), name);
    hdButtonPane_l* root = reinterpret_cast<hdButtonPane_l*>(info->mpRootPane.get());
    hdButtonPane_l* pane = gabi::call_ptr<hdButtonPane_l*>(hdButtonPaneVtable(root)->mFindPane.get(), root,
                                                           static_cast<be<u8>*>(name->mText), 1);
    ensureTextType();
    if (!isPaneKind(pane, &sButtonTextType))
        return;
    info = mpLayout->lyt();
    u32 hasParent = gabi::ea(info->mpRootPane->mpParent.get());
    u32 resourceName = hdButtonResourceName(info);
    if (hasParent)
        resourceName = hdButtonResourceName(mpLayout->lyt()->mpResourceAccessor);
    hdUiSafeString resource;
    resource.mVtable.v = gabi::ea(&sButtonStringVtable);
    resource.mText.v = resourceName;
    hdButton_setTextResource(pane, &resource, text);
}
/* 026F6048 */
void hdButtonPart_c::setTextStyle(hdUiSafeString* name, u32 style) {
    hdPartsLyt_c* info = mpLayout->lyt();
    gabi::call_ptr<void>(name->mVtable->mAssureTermination.get(), name);
    hdButtonPane_l* root = reinterpret_cast<hdButtonPane_l*>(info->mpRootPane.get());
    hdButtonPane_l* pane = gabi::call_ptr<hdButtonPane_l*>(hdButtonPaneVtable(root)->mFindPane.get(), root,
                                                           static_cast<be<u8>*>(name->mText), 1);
    ensureTextType();
    if (isPaneKind(pane, &sButtonTextType))
        hdButton_setTextStyle(pane, style, 1);
}
static hdHeartInitLists_l sButtonInitLists;
static hdHeartInitAngles_l sButtonInitAngles;
static hdHeartAngleLimits_l sButtonAngleLimits;
static hdHeartInitRecord_l sButtonFirstInitRecord;
static hdHeartInitRecord_l sButtonSecondInitRecord;
static hdHeartInitRecord_l sButtonThirdInitRecord;
static f32 sButtonDefaultScale;
static f32 sButtonTouchScale;
static f32 sButtonCursorScale;
void hdButton_registerStatic(hdHeartInitRecord_l*);
void hdButton_initFlag(be<u8>*);
void hdButton_initByte(be<u8>*);
/* 026F6A7C */
static void hdButton_staticInit() {
    sButtonInitLists.mThird = 0;
    sButtonInitLists.mFirst = 0;
    sButtonInitLists.mFourth = 0;
    sButtonInitLists.mSecond = 0;
    hdButton_registerStatic(&sButtonFirstInitRecord);
    f32 minimum = sButtonAngleLimits.mMinimum, maximum = sButtonAngleLimits.mMaximum;
    sButtonInitAngles.mMinimum = minimum;
    sButtonInitAngles.mMaximum = maximum;
    hdButton_initFlag(&sButtonInitAngles.mFlag);
    hdButton_registerStatic(&sButtonSecondInitRecord);
    hdButton_initByte(&sButtonInitAngles.mByte);
    hdButton_registerStatic(&sButtonThirdInitRecord);
    sButtonTouchSound.mVtable.v = gabi::ea(&sButtonStringVtable);
    sButtonTouchSound.mText.v = gabi::guest_addr("SE_DRC_TOUCH_ON");
    f32 scale = sButtonDefaultScale;
    sButtonCursorSound.mVtable.v = gabi::ea(&sButtonStringVtable);
    sButtonCursorSound.mText.v = gabi::guest_addr("SE_MENU_CURSOR");
    sButtonTouchScale = scale;
    sButtonCursorScale = scale;
}
/* WWHD glue
 data sButtonPaneTypeReady 101FDDA0
 data sButtonPaneTypeGuard 101FD914
 data sButtonLayoutPaneTypeGuard 101FD890
 data sButtonBaseTypeGuard 101FD888
 data sButtonPaneType 101FDDA4
 data sButtonPaneDerivedType 101FD918
 data sButtonLayoutPaneType 101FD91C
 data sButtonBaseType 101FD8FC
 data sButtonTextTypeReady 101FDD40
 data sButtonTextTypeGuard 101FD8F0
 data sButtonTextPaneTypeGuard 101FD89C
 data sButtonTextType 101FDD3C
 data sButtonTextDerivedType 101FD8F4
 data sButtonTextPaneType 101FD8F8
 data sButtonInitLists 1049DA80
 data sButtonInitAngles 1049DA74
 data sButtonAngleLimits 10109B5C
 data sButtonFirstInitRecord 101F7150
 data sButtonSecondInitRecord 101F715C
 data sButtonThirdInitRecord 101F7168
 data sButtonDefaultScale 10109B64
 data sButtonTouchScale 1049DA90
 data sButtonCursorScale 1049DA94
 str 10109B68 "SE_DRC_TOUCH_ON"
 str 10109B78 "SE_MENU_CURSOR"
 ext 027045B0 void hdButton_setTextResource(hdButtonPane_l*,hdUiSafeString*,void*)
 ext 02704504 void hdButton_setTextStyle(hdButtonPane_l*,u32,u32)
 ext 028F026C void hdButton_registerStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void hdButton_initFlag(be<u8>*)
 ext 028EAB2C void hdButton_initByte(be<u8>*)
 ext 026F3130 void hdButton_deleteConfig(void*,hdUiSafeString*,void*)
 data sButtonPartVtable 10109CA0
 data sButtonInterfaceVtable 10109CB0
 data sButtonButtonVtable 10109B88
 data sButtonStringVtable 10109AEC
 data sButtonTouchSound 1049DA64
 data sButtonCursorSound 1049DA6C
 data sButtonEventManager 101F4FF0
 data sButtonHeapManager 1018C404
 data sButtonInputMode 101F8378
 data sButtonMenu 101F5088
 data sButtonTouchReleasePending 101F7174
 data sButtonButtonRemap 10109B54
 str 10000160 ""
 ext 026F88F0 void hdButton_baseDestroy(hdButtonPart_c*,u32)
 ext 026F89E8 void hdButton_baseConstruct(hdButtonPart_c*)
 ext 02615398 u32 hdButton_getFocus(hdButtonEventManager_l*)
 ext 026150C0 void hdButton_registerReceiver(hdButtonEventManager_l*,hdButtonReceiver_l*)
 ext 02001BE4 u32 hdButton_receiverButton(hdButtonReceiver_l*,u32,u32)
 ext 0273B050 void* hdButton_allocate(u32,void*,u32)
 ext 02616ED4 hdButtonReceiver_l* hdButton_receiverConstruct(hdButtonReceiver_l*,u32,u32)
 ext 02615180 void hdButton_setFocus(hdButtonEventManager_l*,hdButtonReceiver_l*)
 ext 02615148 void hdButton_setEvent(hdButtonEventManager_l*,hdButtonReceiver_l*,u32)
 ext 02615140 u32 hdButton_getMode(hdButtonEventManager_l*)
 ext 02615190 u32 hdButton_getSuppressedButton(hdButtonEventManager_l*)
 ext 02615198 u32 hdButton_getSuppressedType(hdButtonEventManager_l*)
 ext 02001C00 u32 hdButton_receiverDispatch(hdButtonReceiver_l*,u32)
 ext 0273B310 void hdButton_unlinkEvent(hdButtonEventLink_l*)
 ext 02030B38 void hdButton_playSoundName(be<u8>*)
 ext 025E1988 void hdButton_playSoundId(u32)
*/
