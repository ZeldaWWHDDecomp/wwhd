/* HD baton-sign list: input animation states and the cursor particle effect. */
#include "d/hd_screen_baton_sign_list.h"
static hdPartsVtbl_c sBatonListMainVtable;
static u8 sBatonListHolderVtable;
static u8 sBatonListSecondaryVtable;
static hdUiStringTable sBatonListStringVtable;
static void destroyBatonListPaneName(hdUiSafeString* string, s32 flags);
enum { BATON_INPUT_MASK = 0x8000, BATON_CURSOR_VALID = 1, BATON_PARTICLE_GROUP = 11, BATON_PARTICLE_ID = 2 };
struct hdBatonListInput_l {
    u8 _00[0x18];
    be<u32> mHeldButtons;
    u8 _1C[8];
    be<u32> mCursorFlags;
    u8 _28[0xFC];
    be<u32> mPressedButtons;
    u8 _128[0x8C];
    hdBatonListOffset_c mCursor;
    u8 _1BC[0x14];
    be<u32> mMode;
};
WWHD_OFFSET(hdBatonListInput_l, mHeldButtons, 0x18);
WWHD_OFFSET(hdBatonListInput_l, mCursorFlags, 0x24);
WWHD_OFFSET(hdBatonListInput_l, mPressedButtons, 0x124);
WWHD_OFFSET(hdBatonListInput_l, mCursor, 0x1B4);
WWHD_OFFSET(hdBatonListInput_l, mMode, 0x1D0);
using hdBatonListInputPointer = gptr<hdBatonListInput_l>;
static hdBatonListInputPointer sBatonListInput;
static hdPartsStateId_c sBatonListInputState;
static hdPartsStateId_c sBatonListSlideState;
static hdPartsStateId_c sBatonListBaseBlank;
static void destroyBatonListBase(hdParts_c* self, s32 flags);
static void deleteBatonList(void* object);
static u32 batonListInputBusy(hdBatonListInput_l* input);
static u32 batonListUiBlocked();
static u32 queryBatonListInput(hdBatonSignList_c* self);
static void runBatonListEffect(hdBatonSignList_c* self);
static void setBatonListInput(hdBatonSignList_c* self, u32 enabled);
static void setBatonListVisible(hdBatonSignList_c* self, u32 enabled);

struct hdBatonListPlay_l {
    u8 _00[0x5AB0];
    gptr<dPa_control_c> mpParticles;
};
WWHD_OFFSET(hdBatonListPlay_l, mpParticles, 0x5AB0);
static hdBatonListOffset_c sBatonListDefaultCursor;
static u32 batonListSceneBlocked();
static hdBatonListPlay_l* getBatonListPlay();
static hdBatonListEmitter_l* spawnBatonListEffect(dPa_control_c* manager, u32 group, u32 id, cXyz* position, csXyz* angle, cXyz* scale, u32 alpha, dPa_followEcallBack* callback, s32 setup, GXColor* primary, GXColor* environment, cXyz* scale2D);
static void* allocateBatonListStorage(u32 size);
static void constructBatonListBase(hdParts_c* self);
static void constructBatonListCallback(dPa_followEcallBack* callback, u32 first, u32 second);

struct hdBatonListPaneNames_c { hdUiSafeString mEntries[9]; };
struct hdBatonListRegistration_c { u8 mStorage[12]; };
static hdBatonListPaneNames_c sBatonListPaneNames;
static u32 sBatonListPaneNamesReady;
static hdBatonListRegistration_c sBatonListPaneRegistration;
static void registerBatonListPanes(hdBatonListRegistration_c* registration);
static void destroyBatonListNameArray(void* array, u32 count, u32 size, u32 destructor, u32 first, u32 second);
/* 02621874 */
static void destroyBatonListPaneNames() {
    destroyBatonListNameArray(&sBatonListPaneNames, 9, sizeof(hdUiSafeString), gabi::guest_fn((const void*)destroyBatonListPaneName), 0, 0);
}
/* 02621774 */
static u32 batonListStateKey(hdPartsStateId_c* state) { return u32(s32(state->mNumber)); }
/* 02621898 */
void hdBatonSignList_c::destroy(hdBatonSignList_c* self, s32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sBatonListHolderVtable;
    self->mSecondaryVtable = &sBatonListSecondaryVtable;
    destroyBatonListBase(self, 0);
    if (flags & 1) deleteBatonList(self);
}
/* 02621904 */
u32 hdBatonSignList_c::inputAllowed() {
    if (!mInputEnabled) return 0;
    if (batonListInputBusy(sBatonListInput.get())) return 0;
    if (batonListUiBlocked()) return 0;
    return 1;
}
/* 02621964 */
void hdBatonSignList_c::inputEnter() {
    if (queryBatonListInput(this)) hdAnimSet_playAlternate(animSet(), 3, 1, -1.0f);
}
/* 026219B4 */
void hdBatonSignList_c::updateInputEffect() {
    if (!batonListSceneBlocked()) {
        cXyz scale;
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
        hdBatonListInput_l* input = hdPartsView(sBatonListInput.get());
        if (queryBatonListInput(this) && (input->mPressedButtons & BATON_INPUT_MASK)) {
            const f32 offsetX = mEffectOffset.x;
            const f32 cursorX = (input->mCursorFlags & BATON_CURSOR_VALID) ? f32(input->mCursor.x) : f32(sBatonListDefaultCursor.x);
            mEffectPosition.x = gabi::fadds_ppc(cursorX, offsetX);
            const f32 offsetY = mEffectOffset.y;
            const f32 cursorY = (input->mCursorFlags & BATON_CURSOR_VALID) ? f32(input->mCursor.y) : f32(sBatonListDefaultCursor.y);
            mEffectPosition.y = gabi::fadds_ppc(cursorY, offsetY);
            if (input->mHeldButtons & BATON_INPUT_MASK) {
                hdBatonListPlay_l* play = hdPartsView(getBatonListPlay());
                hdBatonListEmitter_l* emitter = spawnBatonListEffect(play->mpParticles, BATON_PARTICLE_GROUP, BATON_PARTICLE_ID, &mEffectPosition, NULL, &scale, 255, &mEffectCallback, -1, NULL, NULL, NULL);
                mpEmitter = emitter;
                if (emitter != NULL) emitter->mEmissionRate = mEffectRate;
            }
            return;
        }
    }
    mEffectPosition.y = 0.0f;
    mEffectPosition.x = 0.0f;
    mEffectPosition.z = 0.0f;
    mEffectCallback.end();
}
/* 02621B08 */
void hdBatonSignList_c::inputExecute() {
    runBatonListEffect(this);
    if (queryBatonListInput(this) && (hdPartsView(sBatonListInput.get())->mPressedButtons & BATON_INPUT_MASK)) {
        changeState(&sBatonListSlideState);
        return;
    }
    vCalcLayout();
}
/* 02621B90 */
void hdBatonSignList_c::slideEnter() { hdAnimSet_playAlternate(animSet(), 3, 1, 1.0f); }
/* 02621BAC */
void hdBatonSignList_c::slideExecute() {
    runBatonListEffect(this);
    if (!(hdPartsView(sBatonListInput.get())->mPressedButtons & BATON_INPUT_MASK)) {
        changeState(&sBatonListInputState);
        return;
    }
    vCalcLayout();
}
/* 02621C2C */
void hdBatonSignList_c::setInputEnabled(u32 enabled) {
    hdPartsLayout_c* layout = mpLayout;
    mInputEnabled = u8(enabled);
    hdAnimSet_playAlternate(layout->animSet(), enabled ? 1 : 5, 1, 1.0f);
}
/* 02621C58 */
void hdBatonSignList_c::setListEnabled(u32 enabled) {
    hdPartsLayout_c* layout = mpLayout;
    mListEnabled = u8(enabled);
    hdAnimSet_playAlternate(layout->animSet(), enabled ? 2 : 4, 2, 1.0f);
}
/* 02621C84 */
void hdBatonSignList_c::hideElementsForMode() {
    const u32 mode = hdPartsView(sBatonListInput.get())->mMode;
    if (mode == 1) setBatonListInput(this, 0);
    else if (mode == 2) { setBatonListInput(this, 0); setBatonListVisible(this, 1); }
}
/* 02621D14 */
void hdBatonSignList_c::calculateLayout() { vCalcLayout(); }
/* 02621D24 */
void hdBatonSignList_c::showElementsForMode() {
    const u32 mode = hdPartsView(sBatonListInput.get())->mMode;
    if (mode == 1) setBatonListInput(this, 1);
    else if (mode == 2) { setBatonListInput(this, 1); setBatonListVisible(this, 0); }
}
/* 02621DB4 */
void hdBatonSignList_c::allEnter() {
    hdAnimSet_play(animSet(), 0, 0, 1.0f);
    const u32 mode = hdPartsView(sBatonListInput.get())->mMode;
    if (mode == 0) {
        hdAnimSet_operationCC(animSet(), 1, 1);
        setBatonListVisible(this, 1);
    } else if (mode == 1) {
        setBatonListVisible(this, 1);
        setBatonListInput(this, 1);
    } else if (mode == 2) {
        setBatonListInput(this, 1);
        hdAnimSet_operationCC(animSet(), 2, 2);
    }
    mEffectCallback.end();
}
/* 02621E9C */
void hdBatonSignList_c::allExecute() {
    const u32 mode = hdPartsView(sBatonListInput.get())->mMode;
    bool finished = false;
    if (mode == 0) finished = hdAnimSet_isEnd(animSet(), 2) != 0;
    else if (mode == 1) {
        if (hdAnimSet_isEnd(animSet(), 1)) finished = hdAnimSet_isEnd(animSet(), 2) != 0;
    } else if (mode == 2) finished = hdAnimSet_isEnd(animSet(), 1) != 0;
    if (finished) changeState(&sBatonListInputState);
    vCalcLayout();
}
/* 02621F70 */
void hdBatonSignList_c::outEnter() {
    hdAnimSet_play(animSet(), 6, 0, 1.0f);
    if (mListEnabled) setBatonListVisible(this, 0);
    if (mInputEnabled) setBatonListInput(this, 0);
    hdBatonListEmitter_l* emitter = mpEmitter;
    if (emitter != NULL) {
        const u32 flags = emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags | 1;
    }
}
/* 02622004 */
void hdBatonSignList_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), 4) && hdAnimSet_isEnd(animSet(), 5)) changeState(&sBatonListBaseBlank);
    vCalcLayout();
}

/* 02622080 */
hdBatonSignList_c* hdBatonSignList_c::construct(hdBatonSignList_c* self) {
    if (self == NULL) {
        self = static_cast<hdBatonSignList_c*>(allocateBatonListStorage(sizeof(hdBatonSignList_c)));
        if (self == NULL) return self;
    }
    constructBatonListBase(self);
    self->mSecondaryVtable = &sBatonListSecondaryVtable;
    self->mVtable = &sBatonListMainVtable;
    self->mStateHolder.mHolderVtable = &sBatonListHolderVtable;
    // Embedded constructors test the wrapped guest address, including a zero subobject.
    if (u32(gabi::ea(self) + offsetof(hdBatonSignList_c, mPanes)) == 0)
        allocateBatonListStorage(sizeof(self->mPanes));
    constructBatonListCallback(&self->mEffectCallback, 0, 0);
    self->mInputEnabled = 0;
    self->mListEnabled = 0;
    const u32 offsetAddress = u32(gabi::ea(self) + offsetof(hdBatonSignList_c, mEffectOffset));
    hdBatonListOffset_c* offset;
    if (offsetAddress == 0) offset = static_cast<hdBatonListOffset_c*>(allocateBatonListStorage(sizeof(hdBatonListOffset_c)));
    else offset = &self->mEffectOffset;
    if (offset != NULL) {
        offset->x = 0.0f;
        offset->y = 120.0f;
    }
    self->mpEmitter = NULL;
    self->mEffectPosition.x = 0.0f;
    self->mEffectPosition.y = 0.0f;
    self->mEffectRate = 2.0f;
    self->mEffectPosition.z = 0.0f;
    return self;
}

/* 02622184 */
void hdBatonSignList_c::bindPanes() {
    if (!sBatonListPaneNamesReady) {
        sBatonListPaneNamesReady = 1;
        sBatonListPaneNames.mEntries[0].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[0].mText.v = gabi::guest_addr("N_All_00");
        sBatonListPaneNames.mEntries[1].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[1].mText.v = gabi::guest_addr("N_BatonSignAll_00");
        sBatonListPaneNames.mEntries[2].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[2].mText.v = gabi::guest_addr("N_BatonSignA_00");
        sBatonListPaneNames.mEntries[3].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[3].mText.v = gabi::guest_addr("N_BatonSignB_00");
        sBatonListPaneNames.mEntries[4].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[4].mText.v = gabi::guest_addr("N_BatonSignC_00");
        sBatonListPaneNames.mEntries[5].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[5].mText.v = gabi::guest_addr("N_BatonSignD_00");
        sBatonListPaneNames.mEntries[6].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[6].mText.v = gabi::guest_addr("N_BatonSignE_00");
        sBatonListPaneNames.mEntries[7].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[7].mText.v = gabi::guest_addr("N_BatonSignF_00");
        sBatonListPaneNames.mEntries[8].mVtable = &sBatonListStringVtable;
        sBatonListPaneNames.mEntries[8].mText.v = gabi::guest_addr("N_SlideInput_00");
        registerBatonListPanes(&sBatonListPaneRegistration);
    }
    hdPartsPaneSRT_c* root = static_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 9; ++index) {
        hdUiSafeString* name = &sBatonListPaneNames.mEntries[index];
        hdPartsPaneSRTVtbl_c* paneVtable = root->srtVtbl();
        name->assureTermination();
        hdPartsPaneSRT_c* pane = gabi::call_ptr<hdPartsPaneSRT_c*>(paneVtable->mFindPane.get(), root, name->mText.get(), 1);
        mPanes[index] = pane;
    }
}

/* WWHD glue
data sBatonListPaneNames 1048E860
data sBatonListPaneNamesReady 1048EA48
data sBatonListPaneRegistration 101F5248
ext 028F026C void registerBatonListPanes(hdBatonListRegistration_c* registration)
ext 028F0164 void destroyBatonListNameArray(void* array, u32 count, u32 size, u32 destructor, u32 first, u32 second)
str 100E4268 "N_All_00" in 02622184
str 100E4254 "N_BatonSignAll_00" in 02622184
str 100E41E4 "N_BatonSignA_00" in 02622184
str 100E41F4 "N_BatonSignB_00" in 02622184
str 100E4204 "N_BatonSignC_00" in 02622184
str 100E4214 "N_BatonSignD_00" in 02622184
str 100E4224 "N_BatonSignE_00" in 02622184
str 100E4234 "N_BatonSignF_00" in 02622184
str 100E4244 "N_SlideInput_00" in 02622184
data sBatonListDefaultCursor 1049FFA0
data sBatonListInput 101F5088
data sBatonListInputState 1048EA4C
data sBatonListSlideState 1048EA7C
data sBatonListBaseBlank 1049DD60
ext 02715178 u32 batonListSceneBlocked()
ext 025200D4 hdBatonListPlay_l* getBatonListPlay()
ext 025A847C hdBatonListEmitter_l* spawnBatonListEffect(dPa_control_c* manager, u32 group, u32 id, cXyz* position, csXyz* angle, cXyz* scale, u32 alpha, dPa_followEcallBack* callback, s32 setup, GXColor* primary, GXColor* environment, cXyz* scale2D)
ext 0273AD10 void* allocateBatonListStorage(u32 size)
ext 026F89E8 void constructBatonListBase(hdParts_c* self)
ext 025A5894 void constructBatonListCallback(dPa_followEcallBack* callback, u32 first, u32 second)
ext 026F88F0 void destroyBatonListBase(hdParts_c* self, s32 flags)
ext 0273AF40 void deleteBatonList(void* object)
ext 02617AE4 u32 batonListInputBusy(hdBatonListInput_l* input)
ext 027172E4 u32 batonListUiBlocked()
ext 02621904 u32 queryBatonListInput(hdBatonSignList_c* self)
ext 026219B4 void runBatonListEffect(hdBatonSignList_c* self)
ext 02621C2C void setBatonListInput(hdBatonSignList_c* self, u32 enabled)
ext 02621C58 void setBatonListVisible(hdBatonSignList_c* self, u32 enabled)
data sBatonListMainVtable 100E4530
data sBatonListHolderVtable 100E4658
data sBatonListSecondaryVtable 100E4668
data sBatonListStringVtable 100E410C

ext 0262307C void destroyBatonListPaneName(hdUiSafeString* string, s32 flags) callback
*/
