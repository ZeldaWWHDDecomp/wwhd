/* HD failure-screen owner: one character part, separate layout and display/wait states. */
#include "d/hd_screen_boat_race_failed.h"
static u32 failedOwnerSScreenVtable;
static u32 failedOwnerSDelegateVtable;
static u32 failedOwnerSSecondaryVtable;
static hdPartsStateId_c failedOwnerSWaitState;
static hdPartsStateId_c failedOwnerSDisplayState;
static hdUiSafeString failedOwnerSLayoutName;
struct sArchiveNamesTable { hdUiSafeString mNames[2]; };
static sArchiveNamesTable failedOwnerSArchiveNames;
struct sUiPointer { gptr<hdScreenUiSystem> mInstance; };
static sUiPointer failedOwnerSUi;
struct sHeapManagerPointer { gptr<void> mInstance; };
static sHeapManagerPointer failedOwnerSHeapManager;
static hdUiStringTable failedOwnerSStringVtable;
static hdPartsStateIdVtbl_c failedOwnerSStateVtable;
static hdStateId failedOwnerSNullState;
static u32 failedOwnerSStateGuard;
static u32 failedOwnerSNextState;
static hdBoatRaceWords failedOwnerSWords;
static hdBoatRaceRange failedOwnerSRange;
static f32 failedOwnerSLower;
static f32 failedOwnerSUpper;
static u8 failedOwnerSFirstObject;
static u8 failedOwnerSSecondObject;
static hdBoatRaceRegistration failedOwnerSFirstRegistration;
static hdBoatRaceRegistration failedOwnerSSecondRegistration;
static hdBoatRaceRegistration failedOwnerSThirdRegistration;
void failedOwnerBoatRaceBaseConstruct(hdParts_c*);
void failedOwnerBoatRaceBaseDestroy(hdParts_c*, u32);
void failedOwnerBoatRaceBaseRelease(hdParts_c*);
void failedOwnerBoatRaceSetArchives(hdParts_c*, hdUiSafeString*, u32);
void* failedOwnerBoatRaceAllocate(u32);
void failedOwnerBoatRaceDelete(void*);
void failedOwnerBoatRaceResetCharacter(hdBoatRaceFailedCharView*);
void failedOwnerBoatRaceRegister(hdBoatRaceRegistration*);
void failedOwnerBoatRaceConstructFirst(be<u8>*);
void failedOwnerBoatRaceConstructSecond(be<u8>*);
struct hdBoatRaceLayoutResource {
    u8 mPadding[0x3C]; hdPartsDescList_c mChildren;
};
/* 02625F70 */
u32 boatRaceFailedStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 02626070 */
void hdScreenBoatRaceFailed_c::destroy(hdScreenBoatRaceFailed_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &failedOwnerSDelegateVtable;
    screen->mSecondaryVtable = &failedOwnerSSecondaryVtable;
    failedOwnerBoatRaceBaseDestroy(screen, 0);
    if (flags & 1) failedOwnerBoatRaceDelete(screen);
}
/* 026260DC */
void hdScreenBoatRaceFailed_c::enterWait() { mAdvance = 0; mFinished = 0; }
/* 026260EC */
void hdScreenBoatRaceFailed_c::executeWait() { if (mAdvance) changeState(&failedOwnerSDisplayState); }
/* 02626108 */
void hdScreenBoatRaceFailed_c::enterDisplay() {
    mCharacters.mData[0]->mAdvance = 1;
    mVisible = 1;
}
/* 02626120 */
void hdScreenBoatRaceFailed_c::executeDisplay() {
    if (mCharacters.mData[0]->mFinished) changeState(&failedOwnerSWaitState);
}
/* 02626144 */
void hdScreenBoatRaceFailed_c::leaveDisplay() { mFinished = 1; mVisible = 0; }
/* 02626158 */
hdScreenBoatRaceFailed_c* hdScreenBoatRaceFailed_c::construct(hdScreenBoatRaceFailed_c* screen) {
    if (!screen) {
        screen = static_cast<hdScreenBoatRaceFailed_c*>(failedOwnerBoatRaceAllocate(sizeof(hdScreenBoatRaceFailed_c)));
        if (!screen) return NULL;
    }
    failedOwnerBoatRaceBaseConstruct(screen);
    screen->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&failedOwnerSScreenVtable);
    screen->mSecondaryVtable = &failedOwnerSSecondaryVtable;
    screen->mpOwnLayout = NULL;
    hdBoatRaceChildArray* characters = gabi::at<hdBoatRaceChildArray>(gabi::ea(screen) + offsetof(hdScreenBoatRaceFailed_c, mCharacters));
    screen->mStateHolder.mHolderVtable = &failedOwnerSDelegateVtable;
    if (!characters) characters = static_cast<hdBoatRaceChildArray*>(failedOwnerBoatRaceAllocate(sizeof(hdBoatRaceChildArray)));
    if (characters) { characters->mData = NULL; characters->mCount = 0; }
    screen->mAdvance = 0;
    screen->mFinished = 0;
    screen->mCharacterCount = 0;
    screen->mVisible = 0;
    return screen;
}
/* 02626204 */
u32 hdScreenBoatRaceFailed_c::createChildren() {
    hdPartsLayout_c* layout = mpOwnLayout;
    if (!layout || !layout->mAccessor) return 0;
    hdPartsDescList_c* descriptions = gabi::at<hdPartsDescList_c>(gabi::ea(layout->mAccessor.get()) + offsetof(hdBoatRaceLayoutResource, mChildren));
    if (!descriptions) return 1;
    s32 count = descriptions->mCount;
    mCharacterCount = count;
    hdScreenHeap* heap = failedOwnerSUi.mInstance->mHeap;
    if (count > 0) {
        if (!heap) heap = hdParts_getCurrentHeap(failedOwnerSHeapManager.mInstance);
        void* data = heap->allocate(u32(count) * 4);
        if (data) {
            mCharacters.mData = static_cast<gptr<hdBoatRaceFailedCharView>*>(data);
            mCharacters.mCount = count;
        }
    }
    s32 capacity = mCharacters.mCount;
    for (u32 index = 0; s32(index) < capacity; ++index) {
        // HD: each store reloads the array pointer and wraps its guest address.
        *hdPartsView<gptr<hdBoatRaceFailedCharView>>(gabi::ea(mCharacters.mData.get()) + index * 4) = NULL;
    }
    return hdParts_visitAll(this, descriptions);
}
/* 0262633C */
u32 hdScreenBoatRaceFailed_c::setup(u32 first, u32 second) {
    failedOwnerBoatRaceSetArchives(this, failedOwnerSArchiveNames.mNames, 2);
    hdScreenHeap* heap = failedOwnerSUi.mInstance->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(0x110, heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpOwnLayout = layout;
    layout->vLoad(&failedOwnerSLayoutName, gabi::ea(mpResAccessor.get()), first, second, 0, 0, NULL);
    createChildren();
    startState(&failedOwnerSWaitState);
    return 1;
}
static gptr<hdBoatRaceFailedCharView>* boatRaceChildSlot(gptr<hdBoatRaceFailedCharView>* data, u32 index, s32 count) {
    return hdPartsView<gptr<hdBoatRaceFailedCharView>>(gabi::ea(data) + (index < u32(count) ? index * 4 : 0));
}
/* 02626420 */
void hdScreenBoatRaceFailed_c::release() {
    s32 count = mCharacters.mCount;
    gptr<hdBoatRaceFailedCharView>* data = mCharacters.mData;
    for (u32 index = 0; s32(index) < count; ++index) {
        if (*boatRaceChildSlot(data, index, count)) {
            hdBoatRaceFailedCharView* child = *boatRaceChildSlot(data, index, count);
            child->vFinalize();
            child = *mCharacters.slot(index);
            if (child) {
                child->vDelete(3);
                count = mCharacters.mCount;
                data = mCharacters.mData;
            } else {
                count = mCharacters.mCount;
                data = mCharacters.mData;
            }
            *boatRaceChildSlot(data, index, count) = NULL;
            count = mCharacters.mCount;
            data = mCharacters.mData;
        }
    }
    if (data) {
        hdScreenHeap* heap = hdParts_findContainHeap(failedOwnerSHeapManager.mInstance, data);
        heap->free(mCharacters.mData);
        mCharacters.mCount = 0;
        mCharacters.mData = NULL;
    }
    hdPartsLayout_c* layout = mpOwnLayout;
    if (layout) {
        layout->release();
        layout = mpOwnLayout;
        if (layout) layout->destroy(3);
        mpOwnLayout = NULL;
    }
    failedOwnerBoatRaceBaseRelease(this);
}
/* 026265A4 */
void hdScreenBoatRaceFailed_c::executeChildren() {
    s32 count = mCharacters.mCount;
    for (u32 index = 0; s32(index) < count; ++index) {
        gptr<hdBoatRaceFailedCharView>* slot = mCharacters.slot(index);
        if (*slot) {
            (*slot)->vExecuteState();
            count = mCharacters.mCount;
        }
    }
}
/* 0262664C */
void hdScreenBoatRaceFailed_c::execute() {
    hdParts_executeStateMachine(stateMachine());
    hdPartsLayout_c* layout = mpOwnLayout;
    if (layout) layout->calc();
    executeChildren();
}
/* 026266A0 */
void hdScreenBoatRaceFailed_c::draw(void* info) {
    if (!mVisible) return;
    hdPartsLayout_c* layout = mpOwnLayout;
    if (layout) layout->draw(info);
}
/* 026266C8 */
void hdScreenBoatRaceFailed_c::reset() {
    changeState(&failedOwnerSWaitState);
    failedOwnerBoatRaceResetCharacter(mCharacters.mData[0]);
}
struct hdBoatRaceCharacterDescriptor {
    u8 mPadding[0x48];
    gptr<void> mpSecondResource;
    gptr<void> mpFirstResource;
    be<u32> mFlags;
    u8 mPaddingToEmbedded[0x84 - 0x54];
    u8 mEmbeddedResource[0x164 - 0x84];
    gptr<void> mpAlternateResource;
};
struct hdBoatRaceDescriptorRegistry {
    u8 mPadding[0x1020];
    be<u32> mCount;
    gptr<gptr<hdBoatRaceCharacterDescriptor>> mData;
};
struct sDescriptorsPointer { gptr<hdBoatRaceDescriptorRegistry> mInstance; };
static sDescriptorsPointer failedOwnerSDescriptors;
static u32 failedOwnerSDefaultFirstResource;
static u32 failedOwnerSDefaultSecondResource;
hdBoatRaceFailedCharView* failedOwnerBoatRaceConstructCharacter(hdBoatRaceFailedCharView*);
u32 failedOwnerBoatRaceSetupCharacter(hdBoatRaceFailedCharView*, void*, void*, void*, hdPartsDesc_c*);
/* 0262670C */
u32 hdScreenBoatRaceFailed_c::createChild(hdPartsDesc_c* descriptor) {
    hdScreenHeap* heap = failedOwnerSUi.mInstance->mHeap;
    hdBoatRaceDescriptorRegistry* registry = failedOwnerSDescriptors.mInstance;
    u32 count = registry->mCount;
    gptr<hdBoatRaceCharacterDescriptor>* entries = registry->mData;
    hdBoatRaceCharacterDescriptor* record = entries[count > 3 ? 3 : 0];
    gptr<hdBoatRaceFailedCharView>* destination = mCharacters.mData;
    hdBoatRaceFailedCharView* child = static_cast<hdBoatRaceFailedCharView*>(hdParts_new(0x74, heap, 4));
    if (child) child = failedOwnerBoatRaceConstructCharacter(child);
    *destination = child;
    u32 flags = record->mFlags;
    gptr<hdBoatRaceFailedCharView>* characters = mCharacters.mData;
    void* first;
    if (flags & 0x1000) first = record->mpAlternateResource;
    else { first = record->mpFirstResource; if (!first) first = &failedOwnerSDefaultFirstResource; }
    void* second;
    if ((flags & 0x40) && (flags & 0x80)) {
        second = hdPartsView<void>(gabi::ea(record) + offsetof(hdBoatRaceCharacterDescriptor, mEmbeddedResource));
    } else {
        second = record->mpSecondResource;
        if (!second) second = &failedOwnerSDefaultSecondResource;
    }
    return failedOwnerBoatRaceSetupCharacter(characters[0], mpResAccessor, first, second, descriptor);
}
/* 0262684C */
void boatRaceFailedStaticInit() {
    failedOwnerSWords.mWords[3] = 0;
    failedOwnerSWords.mWords[2] = 0;
    failedOwnerSWords.mWords[1] = 0;
    failedOwnerSWords.mWords[0] = 0;
    failedOwnerBoatRaceRegister(&failedOwnerSFirstRegistration);
    f32 lower = failedOwnerSLower, upper = failedOwnerSUpper;
    failedOwnerSRange.mLower = lower;
    failedOwnerSRange.mUpper = upper;
    failedOwnerBoatRaceConstructFirst(&failedOwnerSFirstObject);
    failedOwnerBoatRaceRegister(&failedOwnerSSecondRegistration);
    failedOwnerBoatRaceConstructSecond(&failedOwnerSSecondObject);
    failedOwnerBoatRaceRegister(&failedOwnerSThirdRegistration);
    u32 guard = failedOwnerSStateGuard;
    failedOwnerSLayoutName.mVtable = &failedOwnerSStringVtable;
    failedOwnerSLayoutName.mText.v = gabi::guest_addr("BoatRaceFailed_00");
    failedOwnerSArchiveNames.mNames[0].mVtable = &failedOwnerSStringVtable;
    failedOwnerSArchiveNames.mNames[0].mText.v = gabi::guest_addr("BoatRaceFailed_00");
    failedOwnerSArchiveNames.mNames[1].mVtable = &failedOwnerSStringVtable;
    failedOwnerSArchiveNames.mNames[1].mText.v = gabi::guest_addr("BoatRaceFailedChar_00");
    u32 id;
    if (guard) id = failedOwnerSNextState;
    else { failedOwnerSStateGuard = 1; id = 0; }
    failedOwnerSWaitState.setup(id + 1, "::StateID_Wait", &failedOwnerSStateVtable, 0x19, &failedOwnerSNullState);
    failedOwnerSDisplayState.setup(id + 2, "::StateID_Disp", &failedOwnerSStateVtable, 0x1C, &failedOwnerSNullState);
    failedOwnerSNextState = id + 2;
}
/* WWHD glue
data failedOwnerSDescriptors 101F95D0
data failedOwnerSDefaultFirstResource 104A20FC
data failedOwnerSDefaultSecondResource 104A2098
str 100E54F4 "BoatRaceFailed_00" in 0262684C
str 100E5508 "BoatRaceFailedChar_00" in 0262684C
str 100E5520 "::StateID_Wait" in 0262684C
str 100E5530 "::StateID_Disp" in 0262684C
ext 02626E70 hdBoatRaceFailedCharView* failedOwnerBoatRaceConstructCharacter(hdBoatRaceFailedCharView*)
ext 02627020 u32 failedOwnerBoatRaceSetupCharacter(hdBoatRaceFailedCharView*,void*,void*,void*,hdPartsDesc_c*)
data failedOwnerSScreenVtable 100E5540
data failedOwnerSDelegateVtable 100E5638
data failedOwnerSSecondaryVtable 100E5648
data failedOwnerSWaitState 1048F030
data failedOwnerSDisplayState 1048F060
data failedOwnerSLayoutName 1048F090
data failedOwnerSArchiveNames 1048F020
data failedOwnerSUi 1018C404
data failedOwnerSHeapManager 101F8B4C
data failedOwnerSStringVtable 100E53F4
data failedOwnerSStateVtable 100E549C
data failedOwnerSNullState 101FF32C
data failedOwnerSStateGuard 101FD8E4
data failedOwnerSNextState 101FDD50
data failedOwnerSWords 1048F010
data failedOwnerSRange 1048F004
data failedOwnerSLower 100E54EC
data failedOwnerSUpper 100E54F0
data failedOwnerSFirstObject 1048F00C
data failedOwnerSSecondObject 1048F00D
data failedOwnerSFirstRegistration 101F5338
data failedOwnerSSecondRegistration 101F5344
data failedOwnerSThirdRegistration 101F5350
ext 026F89E8 void failedOwnerBoatRaceBaseConstruct(hdParts_c*)
ext 026F88F0 void failedOwnerBoatRaceBaseDestroy(hdParts_c*,u32)
ext 026F91F8 void failedOwnerBoatRaceBaseRelease(hdParts_c*)
ext 026F90D8 void failedOwnerBoatRaceSetArchives(hdParts_c*,hdUiSafeString*,u32)
ext 0273AD10 void* failedOwnerBoatRaceAllocate(u32)
ext 0273AF40 void failedOwnerBoatRaceDelete(void*)
ext 02627264 void failedOwnerBoatRaceResetCharacter(hdBoatRaceFailedCharView*)
ext 028F026C void failedOwnerBoatRaceRegister(hdBoatRaceRegistration*)
ext 028ED6F8 void failedOwnerBoatRaceConstructFirst(be<u8>*)
ext 028EAB2C void failedOwnerBoatRaceConstructSecond(be<u8>*)
*/
