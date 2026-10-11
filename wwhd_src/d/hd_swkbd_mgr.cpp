/* HD: cking::ui::input::SwkbdMgr, GamePad software keyboard for name entry. */
#include "d/hd_swkbd_mgr.h"
static hdSwkbd_InstancePointer_c sInstance;
static hdSwkbd_RegisteredDisposerPointer_c sRegisteredDisposer;
static hdSwkbd_ControllerManagerPointer_c sControllerManager;
static hdSwkbd_FontManagerPointer_c sFontManager;
static hdSwkbd_FsClientPointer_c sFsClient;
static hdSwkbd_ConfigPointer_c sConfig;
static hdSwkbd_InputModeManagerPointer_c sInputModeManager;
static u32 sPadTypeReady;
static u32 sDrcTypeReady;
static hdSwkbd_typeInfo_c sPadType;
static hdSwkbd_typeInfo_c sDrcType;
static u8 sFallbackPrimary[1] = {};
static u8 sFallbackSecondary[1] = {};
static u8 sOpenState[1] = {};
static u8 sCloseState[1] = {};
static const u32 pointerVtable = 0x100E2A78;
static const u32 managerVtable = 0x100E2AC0;
static const u32 disposerVtable = 0x100E2AF8;
static const u32 stringVtable = 0x100E2A88;
static const u32 controllerTypeVtable = 0x100E2AB0;
enum hdSwkbd_region_e { CONSOLE_JAPAN = 1, CONSOLE_EUROPE = 4, KEYBOARD_JAPAN = 0, KEYBOARD_AMERICA = 1, KEYBOARD_EUROPE = 2 };
enum hdSwkbd_fontFlags_e { FONT_ALTERNATE = 1 << 6, FONT_EMBEDDED_SECONDARY = 1 << 7, FONT_EXTENDED_PRIMARY = 1 << 12 };

/* 02619220 */
static void hdSwkbd_destroyPointer(hdSwkbd_pointer_c* pointer, u32 flags) {
    if (!pointer) return;
    pointer->mSecondaryVtable = pointerVtable;
    hdSwkbd_destroyPointerBase(pointer, 0);
    if (flags & 1) operator_delete(pointer);
}
/* 02619280 */
static hdSwkbd_mgr_c* hdSwkbd_construct(hdSwkbd_mgr_c* manager) {
    if (!manager) {
        manager = static_cast<hdSwkbd_mgr_c*>(operator_new(sizeof(hdSwkbd_mgr_c)));
        if (!manager) return nullptr;
    }
    manager->mpWorkMemory = nullptr;
    manager->mpLayouts[1] = nullptr;
    manager->mpLayouts[0] = nullptr;
    manager->mVtable.v = managerVtable;
    hdSwkbd_zeroMemory(manager->mControllerInfo, 0xAC);
    // HD: embedded member addresses can wrap to null, retaining their original allocation paths.
    gptr<void>* controllers = gabi::at<gptr<void>>(gabi::ea(manager) + 0xEC);
    if (!controllers) controllers = static_cast<gptr<void>*>(operator_new(0x14));
    if (controllers) for (u32 i = 0; i < 5; ++i) controllers[i] = nullptr;
    hdSwkbd_appearArgs_c* args = gabi::at<hdSwkbd_appearArgs_c>(gabi::ea(manager) + 0x100);
    if (!args) args = static_cast<hdSwkbd_appearArgs_c*>(operator_new(sizeof(hdSwkbd_appearArgs_c)));
    if (args) {
        args->mUnknown30 = 0;
        args->mLanguage = 0;
        args->mCursorPosition = 0xFFFFFFFF;
        args->mKeyboardType = 0x13;
        args->mEnable = 0;
        args->mUnknownA4 = 0xFFFFFFFF;
        args->mUnknown28 = 0;
        args->mFlags = 0x7FFFF;
        args->mMode = 4;
        args->mUnknown2C = 0;
        args->mUnknownA0 = 0;
        args->mUnknown22 = 0;
        args->mUnknown08 = 0;
        args->mUnknown20 = 0;
        args->mUnknown24 = 0;
        args->mUnknown1C = 0;
        args->mInputMode = 1;
        for (u32 i = 0; i < 13; ++i) {
            args->mTextParameters[i] = 0;
            args->mPredictionParameters[i] = 0;
        }
    }
    manager->mFormArgs.mpParameters = nullptr;
    manager->mFormArgs.mUnknown10 = 0;
    manager->mFormArgs.mCursorPosition = 0xFFFFFFFF;
    manager->mMode = hdSwkbd_mgr_c::MODE_NAME;
    manager->mFormArgs.mMaxLength = 0;
    manager->mFormArgs.mpText = nullptr;
    manager->mFormArgs.mUnknown14 = 0xFFFFFFFF;
    return manager;
}
/* 026193D4 */
static void hdSwkbd_createInstance(void* heap) {
    if (sInstance.mpValue) return;
    hdSwkbd_mgr_c* manager = static_cast<hdSwkbd_mgr_c*>(hdSwkbd_allocate(sizeof(hdSwkbd_mgr_c), heap, 4));
    if (manager) {
        hdSwkbd_constructDisposer(manager, heap, 3);
        manager->hdSwkbd_disposer_c::mVtable = disposerVtable;
    }
    sRegisteredDisposer.mpValue = manager;
    sInstance.mpValue = manager ? hdSwkbd_construct(manager) : nullptr;
}
/* 02619468 */
void hdSwkbd_mgr_c::createLayouts() {
    void* heap = hdSwkbd_getLayoutHeap();
    hdSwkbd_layout_c* first = static_cast<hdSwkbd_layout_c*>(hdSwkbd_allocateLayout(0x60, heap, 4));
    if (first) first = hdSwkbd_constructLayout(first);
    mpLayouts[0] = first;
    hdSwkbd_layout_c* second = static_cast<hdSwkbd_layout_c*>(hdSwkbd_allocateLayout(0x50, heap, 4));
    if (second) second = hdSwkbd_constructSubLayout(second);
    mpLayouts[1] = second;
    hdSwkbd_fontManager_c* fonts = sFontManager.mpValue;
    u32 count = fonts->mFontCount;
    gptr<hdSwkbd_font_c>* table = fonts->mpFonts;
    if (count > 7) table = gabi::at<gptr<hdSwkbd_font_c>>(gabi::ea(table) + 0x1C);
    hdSwkbd_font_c* font = *table;
    u32 flags = font->mFlags;
    void* primary;
    if (flags & FONT_EXTENDED_PRIMARY) primary = font->mpExtendedPrimary;
    else {
        primary = sFallbackPrimary;
        void* loaded = font->mpPrimary;
        if (loaded) primary = loaded;
    }
    void* secondary;
    if ((flags & FONT_ALTERNATE) && (flags & FONT_EMBEDDED_SECONDARY)) secondary = font->mEmbeddedSecondary;
    else {
        void* loaded = font->mpSecondary;
        secondary = loaded ? loaded : sFallbackSecondary;
    }
    hdSwkbd_setLayoutFonts(mpLayouts[0], primary, secondary, 0);
    hdSwkbd_setSubLayoutFonts(mpLayouts[1], primary, secondary, 0);
}
/* 026195C4 */
void hdSwkbd_mgr_c::prepare() {
    createLayouts();
    gabi::Local<hdSwkbd_prepareStack_c> frame;
    hdSwkbd_string_c* name = &frame->mHeapName;
    name->mVtable = stringVtable;
    name->mpText.v = gabi::guest_addr("cking::ui::input::SwkbdMgr");
    hdSwkbd_heap_c* heap = hdSwkbd_createHeap(0x1900000, name, hdSwkbd_getWorkHeap(), 1, 0);
    hdSwkbd_createArgs_c* args = &frame->mCreateArgs;
    args->mpWork = nullptr;
    args->mpFsClient = nullptr;
    args->mRegion = 0;
    args->mType = 2;
    u32 size = hdSwkbd_getWorkSize(2);
    void* work = hdSwkbd_allocate(size, heap, 4);
    args->mpWork = work;
    mpWorkMemory = work;
    void* fs = gabi::at<void>(gabi::ea(sFsClient.mpValue) + 0x24);
    mAppearArgs.mInputMode = 2;
    mAppearArgs.mUnknownA0 = 0;
    mAppearArgs.mUnknown24 = 0;
    mAppearArgs.mEnable = 1;
    hdSwkbd_config_c* config = sConfig.mpValue;
    args->mpFsClient = fs;
    u32 region = config->mRegion, language = config->mLanguage;
    if (region == CONSOLE_JAPAN) {
        args->mRegion = KEYBOARD_JAPAN;
        mAppearArgs.mLanguage = 0;
        mAppearArgs.mFlags = 0x6001;
        mAppearArgs.mUnknown28 = 0xAD;
        mAppearArgs.mCursorPosition = 0xFFFFFFFF;
        mAppearArgs.mKeyboardType = 0xD;
        mAppearArgs.mUnknown08 = 0;
    } else if (region == CONSOLE_EUROPE) {
        args->mRegion = KEYBOARD_EUROPE;
        mAppearArgs.mFlags = 0x3E0;
        u32 keyboard = 5, layout = 1;
        if (language == 2) { keyboard = 6; layout = 2; }
        else if (language == 3) { keyboard = 7; layout = 3; }
        else if (language == 4) { keyboard = 8; layout = 4; }
        else if (language == 5) { keyboard = 9; layout = 5; }
        mAppearArgs.mUnknown08 = 0;
        mAppearArgs.mUnknown28 = 0xAD;
        mAppearArgs.mCursorPosition = 0xFFFFFFFF;
        mAppearArgs.mKeyboardType = keyboard;
        mAppearArgs.mLanguage = layout;
    } else {
        args->mRegion = KEYBOARD_AMERICA;
        mAppearArgs.mFlags = 0xE;
        u32 keyboard = 1, layout = 1;
        if (language == 2) { keyboard = 2; layout = 2; }
        else if (language == 5) { keyboard = 3; layout = 5; }
        mAppearArgs.mLanguage = layout;
        mAppearArgs.mKeyboardType = keyboard;
        mAppearArgs.mUnknown08 = 0;
        mAppearArgs.mCursorPosition = 0xFFFFFFFF;
        mAppearArgs.mUnknown28 = 0xAD;
    }
    hdSwkbd_createKeyboard(args);
    gabi::call_ptr<void>(heap->mVtable->mAdjust, heap);
}
/* 02619978 */
void hdSwkbd_mgr_c::setControllers() {
    hdSwkbd_pad_c* pad = static_cast<hdSwkbd_pad_c*>(hdSwkbd_getController(sControllerManager.mpValue, 8));
    if (!sPadTypeReady) { sPadTypeReady = 1; sPadType.mVtable = controllerTypeVtable; }
    if (pad && gabi::call_ptr<u32>(pad->mVtable->mIsDerived, pad, &sPadType) && pad && s32(pad->mConnected) > 0) {
        for (u32 i = 0; i < 43; ++i) mControllerInfo[i] = u32(pad->mInfo[i]);
        // HD: VPAD calibration uses the same packed point for input and output (+0x52 within the copied info).
        void* point = gabi::at<void>(gabi::ea(this) + 0x92);
        hdSwkbd_calibrateTouch(0, point, point);
        mpControllers[0] = mControllerInfo;
    } else mpControllers[0] = nullptr;
    hdSwkbd_drc_c* drc = static_cast<hdSwkbd_drc_c*>(hdSwkbd_getController(sControllerManager.mpValue, 7));
    if (!sDrcTypeReady) { sDrcType.mVtable = controllerTypeVtable; sDrcTypeReady = 1; }
    if (drc && gabi::call_ptr<u32>(drc->mVtable->mIsDerived, drc, &sDrcType) && drc) {
        mpControllers[1] = s32(drc->mConnected) > 0 ? drc->mInfo : nullptr;
    } else for (u32 i = 1; i < 5; ++i) mpControllers[i] = nullptr;
    hdSwkbd_setControllerInfo(mpControllers);
    if (hdSwkbd_needsSoundUpdate()) hdSwkbd_updateSound();
}
/* 02619B68 */
void hdSwkbd_mgr_c::calc() {
    if (!hdSwkbd_getState()) return;
    setControllers();
    hdSwkbd_setLayoutText(mpLayouts[0], mText);
    hdSwkbd_layout_c* layout = mpLayouts[0];
    gabi::call_ptr<void>(layout->mVtable->mCalc, layout);
    gabi::Local<hdSwkbd_layoutState_c> state;
    hdSwkbd_getLayoutState(mpLayouts[0], state);
    hdSwkbd_setSubLayoutState(mpLayouts[1], state);
    layout = mpLayouts[1];
    gabi::call_ptr<void>(layout->mVtable->mCalc, layout);
}
/* 02619BF0 */
u32 hdSwkbd_mgr_c::isShown() { return hdSwkbd_getState() == STATE_SHOWN; }
static void hdSwkbd_drawBoth(hdSwkbd_mgr_c* manager, u32 port) {
    hdSwkbd_layout_c* layout = manager->mpLayouts[0];
    gabi::call_ptr<void>(layout->mVtable->mDraw, layout, port);
    if (manager->isShown()) {
        layout = manager->mpLayouts[1];
        gabi::call_ptr<void>(layout->mVtable->mDraw, layout, port);
    }
}
/* 02619C1C */
void hdSwkbd_mgr_c::drawTV(u32 port) { hdSwkbd_drawBoth(this, port); hdSwkbd_drawKeyboardTV(); }
/* 02619C90 */
void hdSwkbd_mgr_c::drawDRC(u32 port) { hdSwkbd_drawBoth(this, port); hdSwkbd_drawKeyboardDRC(); }
/* 02619D04 */
void hdSwkbd_mgr_c::drawLayouts(u32 port) { hdSwkbd_drawBoth(this, port); }
/* 02619D74 */
static void hdSwkbd_drawTV() { hdSwkbd_drawKeyboardTV(); }
/* 02619D78 */
static void hdSwkbd_drawDRC() { hdSwkbd_drawKeyboardDRC(); }
/* 02619D7C */
void hdSwkbd_mgr_c::open() {
    hdSwkbd_setLayoutMode(mpLayouts[0], mMode);
    hdSwkbd_changeState(gabi::at<void>(gabi::ea(mpLayouts[0]) + 0x18), sOpenState);
    mText[0] = 0;
    u32 mode = hdSwkbd_getInputMode(sInputModeManager.mpValue);
    hdSwkbd_layout_c* layout = mpLayouts[0];
    mAppearArgs.mMode = mode ? 0 : 4;
    u32 inputMode = mMode;
    mFormArgs.mCursorPosition = 0xFFFFFFFF;
    mFormArgs.mUnknown14 = 0xFFFFFFFF;
    mFormArgs.mpText = mText;
    mFormArgs.mMaxLength = inputMode == MODE_LONG_NAME ? 0x11 : 9;
    mFormArgs.mpParameters = layout ? gabi::at<void>(gabi::ea(layout) + 0x50) : nullptr;
    mFormArgs.mUnknown10 = 0;
    hdSwkbd_appear(&mAppearArgs);
}
/* 02619E84 */
void hdSwkbd_mgr_c::close() {
    hdSwkbd_changeState(gabi::at<void>(gabi::ea(mpLayouts[0]) + 0x18), sCloseState);
    hdSwkbd_setDisappearArgs(&mAppearArgs.mKeyboardType);
    hdSwkbd_disappear();
}
/* 02619ECC */
static u32 hdSwkbd_disappearA() { return hdSwkbd_getDisappearA(0); }
/* 02619ED4 */
static u32 hdSwkbd_disappearB() { return hdSwkbd_getDisappearB(0); }
/* 02619EDC */
be<u16>* hdSwkbd_mgr_c::getText() { return gabi::at<be<u16>>(gabi::ea(this) + 0x10); }
/* 02619EE4 */
void hdSwkbd_mgr_c::layoutCall(u32 value) { hdSwkbd_layoutParameter(mpLayouts[0], value); }
/* 02619EEC */
void hdSwkbd_mgr_c::destroy() {
    for (u32 i = 0; i < 2; ++i) {
        hdSwkbd_layout_c* layout = mpLayouts[i];
        if (!layout) continue;
        gabi::call_ptr<void>(layout->mVtable->mFinalize, layout);
        layout = mpLayouts[i];
        if (layout) gabi::call_ptr<void>(layout->mVtable->mDestroy, layout, 3);
        mpLayouts[i] = nullptr;
    }
    hdSwkbd_destroyKeyboard();
    hdSwkbd_freeWork(mpWorkMemory);
    mpWorkMemory = nullptr;
}
/* 02619FB0 */
void hdSwkbd_mgr_c::setBoth(u32 first, u32 second) {
    hdSwkbd_setLayoutParameters(mpLayouts[0], first, second);
    hdSwkbd_setSubLayoutParameters(mpLayouts[1], first, second);
}
/* 0261A008 */
u32 hdSwkbd_mgr_c::isHidden() { return hdSwkbd_getState() == STATE_HIDDEN; }
/* 0261A030 */
static void hdSwkbd_destroyDisposer(hdSwkbd_disposer_c* disposer, u32 flags) {
    if (!disposer) return;
    disposer->mVtable = disposerVtable;
    if (disposer == (hdSwkbd_disposer_c*)sRegisteredDisposer.mpValue) {
        hdSwkbd_mgr_c* instance = sInstance.mpValue;
        sRegisteredDisposer.mpValue = nullptr;
        gabi::call_ptr<void>(instance->mVtable->mDestroy, instance, 2);
        sInstance.mpValue = nullptr;
    }
    hdSwkbd_destroyDisposerBase(disposer, 0);
    if (flags & 1) operator_delete(disposer);
}
/* 0261A0D8 */
static void hdSwkbd_initStatics() { sinit_header_statics_rodata(0x1048DEA0, 0x101F50B4, 0x1048DEAC, 0x100E2AEC); }
/* 0261A184 */
static void hdSwkbd_destroyManager(hdSwkbd_mgr_c* manager, u32 flags) { if (manager && (flags & 1)) operator_delete(manager); }

/* WWHD glue
data sInstance 101F50D8 nocheck
data sRegisteredDisposer 101F50DC nocheck
data sControllerManager 101F8AE8 nocheck
data sFontManager 101F95D0 nocheck
data sFsClient 101F8B08 nocheck
data sConfig 101F4BAC nocheck
data sInputModeManager 101F5088 nocheck
data sPadTypeReady 101FD694 nocheck
data sDrcTypeReady 101FD698 nocheck
data sPadType 101FD968 nocheck
data sDrcType 101FD978 nocheck
data sFallbackPrimary 104A20FC nocheck
data sFallbackSecondary 104A2098 nocheck
data sOpenState 10494EF4 nocheck
data sCloseState 10494F24 nocheck
str 100E2AD0 "cking::ui::input::SwkbdMgr" in 026195C4
ext 02001728 void hdSwkbd_destroyPointerBase(hdSwkbd_pointer_c* pointer, u32 flags)
ext 028F521C void hdSwkbd_zeroMemory(void* data, u32 size)
ext 0273B0D4 void* hdSwkbd_allocate(u32 size, void* heap, u32 alignment)
ext 02752B0C void hdSwkbd_constructDisposer(hdSwkbd_disposer_c* disposer, void* heap, u32 flags)
ext 0203E8E8 void* hdSwkbd_getLayoutHeap()
ext 0273B050 void* hdSwkbd_allocateLayout(u32 size, void* heap, u32 alignment)
ext 02672D98 hdSwkbd_layout_c* hdSwkbd_constructLayout(hdSwkbd_layout_c* layout)
ext 02673AFC hdSwkbd_layout_c* hdSwkbd_constructSubLayout(hdSwkbd_layout_c* layout)
ext 026730B8 void hdSwkbd_setLayoutFonts(hdSwkbd_layout_c* layout, void* primary, void* secondary, u32 flags)
ext 02673B68 void hdSwkbd_setSubLayoutFonts(hdSwkbd_layout_c* layout, void* primary, void* secondary, u32 flags)
ext 0203E918 void* hdSwkbd_getWorkHeap()
ext 02753004 hdSwkbd_heap_c* hdSwkbd_createHeap(u32 size, hdSwkbd_string_c* name, void* parent, u32 flags, u32 alignment)
ext 0286CD54 u32 hdSwkbd_getWorkSize(u32 type)
ext 0286CD7C void hdSwkbd_createKeyboard(hdSwkbd_createArgs_c* args)
ext 0273CB8C void* hdSwkbd_getController(void* manager, u32 index)
ext C00075D0 void hdSwkbd_calibrateTouch(u32 channel, void* output, void* input)
ext 0286D280 void hdSwkbd_setControllerInfo(gptr<void>* controllers)
ext 0286D2D4 u32 hdSwkbd_needsSoundUpdate()
ext 0286D2E4 void hdSwkbd_updateSound()
ext 0286D270 u32 hdSwkbd_getState()
ext 026732B8 void hdSwkbd_setLayoutText(hdSwkbd_layout_c* layout, be<u16>* text)
ext 0267341C void hdSwkbd_getLayoutState(hdSwkbd_layout_c* layout, hdSwkbd_layoutState_c* state)
ext 02673C98 void hdSwkbd_setSubLayoutState(hdSwkbd_layout_c* layout, hdSwkbd_layoutState_c* state)
ext 0286D2F4 void hdSwkbd_drawKeyboardTV()
ext 0286D338 void hdSwkbd_drawKeyboardDRC()
ext 02673344 void hdSwkbd_setLayoutMode(hdSwkbd_layout_c* layout, u32 mode)
ext 020063C0 void hdSwkbd_changeState(void* machine, void* state)
ext 02617AE4 u32 hdSwkbd_getInputMode(void* manager)
ext 0286D3E0 void hdSwkbd_appear(hdSwkbd_appearArgs_c* args)
ext 0286D400 void hdSwkbd_setDisappearArgs(be<u32>* args)
ext 0286D3F0 void hdSwkbd_disappear()
ext 0286D3D0 u32 hdSwkbd_getDisappearA(u32 flags)
ext 0286D3C0 u32 hdSwkbd_getDisappearB(u32 flags)
ext 026733AC void hdSwkbd_layoutParameter(hdSwkbd_layout_c* layout, u32 value)
ext 0286D37C void hdSwkbd_destroyKeyboard()
ext 0273AFC8 void hdSwkbd_freeWork(void* work)
ext 02673414 void hdSwkbd_setLayoutParameters(hdSwkbd_layout_c* layout, u32 first, u32 second)
ext 02673CCC void hdSwkbd_setSubLayoutParameters(hdSwkbd_layout_c* layout, u32 first, u32 second)
ext 02752BEC void hdSwkbd_destroyDisposerBase(hdSwkbd_disposer_c* disposer, u32 flags)
*/
