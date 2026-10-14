/* HD GameOver aggregate screen. */
#include "d/hd_screen_game_over.h"
#include "d/hd_screen_map_display_coordinator.h"
static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static u32 sScreenVtable;
static u32 sStateHolderVtable;
static u32 sSecondaryVtable;
typedef gptr<hdMapHeapManager_l> hdGameOverContextManagerPointer;
static hdGameOverContextManagerPointer sGameOverContextManager;
static hdGameOverContext sGameOverDefaultPrimary;
static hdGameOverContext sGameOverDefaultSecondary;
extern hdGameOverChar_c* hdGameOverConstructCharacter(hdGameOverChar_c*);
extern u32 hdGameOverInitializeCharacter(hdGameOverChar_c*, hdPartsResAccessor_c*, u32, u32, void*);
typedef gptr<hdScreenUiSystem> hdGameOverUiPointer;
typedef gptr<void> hdGameOverHeapManagerPointer;
static hdGameOverUiPointer sGameOverUiSystem;
static hdGameOverHeapManagerPointer sGameOverHeapManager;
extern u32 hdGameOverBindVisitor(hdParts_c* self, hdPartsDescList_c* descriptions);
extern void hdGameOverFinalizeBase(hdParts_c* self);
extern void hdGameOverAttachArchives(hdParts_c* self, hdUiSafeString* names, u32 count);
static hdGameOverRuntimeWords sGameOverRuntime;
static hdGameOverRegistration sGameOverRuntimeRegistration;
static hdGameOverRegistration sGameOverFirstRegistration;
static hdGameOverRegistration sGameOverSecondRegistration;
static hdGameOverContext sGameOverFirstContext;
static hdGameOverContext sGameOverSecondContext;
static f32 sGameOverMinimumAngle;
static f32 sGameOverMaximumAngle;
static hdGameOverNames sGameOverArchives;
static hdUiSafeString sGameOverLayoutName;
static hdPartsStateCounter_c sGameOverCounterReady;
static hdPartsStateCounter_c sGameOverNextState;
static hdStateId sGameOverNullState;
extern void hdGameOverRegisterStatics(hdGameOverRegistration* registration);
extern void hdGameOverConstructFirstContext(hdGameOverContext* context);
extern void hdGameOverConstructSecondContext(hdGameOverContext* context);
static hdPartsStateId_c sGameOverOutState;
static hdPartsStateId_c sGameOverCompleteState;
extern void hdGameOverDestroyBase(hdParts_c*, u32);
extern void hdGameOverConstructBase(hdParts_c*);
extern hdGameOver_c* hdGameOverAllocate(u32);
extern hdGameOverCharacters* hdGameOverAllocateCharacters(u32);
extern void hdGameOverDelete(void*);
/* 0266ACDC */
void hdGameOver_c::destroy(hdGameOver_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    hdGameOverDestroyBase(self, 0);
    if (flags & 1) hdGameOverDelete(self);
}
/* 0266AD48 */
void hdGameOver_c::waitEnter() {
    mRequested = 0;
}
/* 0266AD54 */
void hdGameOver_c::waitMove() {
    if (mRequested) changeState(&sGameOverOutState);
}
/* 0266AD70 */
void hdGameOver_c::outEnter() {
    hdGameOverChar_c* character = *mCharacters.slot(0);
    character->mRequested = 1;
    mFinished = 0;
}
/* 0266AD8C */
void hdGameOver_c::outMove() {
    hdGameOverChar_c* character = *mCharacters.slot(0);
    if (character->mFinished) changeState(&sGameOverCompleteState);
}
/* 0266ADB0 */
void hdGameOver_c::completeEnter() {
    mFinished = 1;
}
/* 0266ADBC */
hdGameOver_c* hdGameOver_c::construct(hdGameOver_c* self) {
    if (!self) {
        self = hdGameOverAllocate(sizeof(hdGameOver_c));
        if (!self) return nullptr;
    }
    hdGameOverConstructBase(self);
    self->mVtable.v = gabi::ea(&sScreenVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    self->mpGameLayout = nullptr;
    auto* characters = gabi::at<hdGameOverCharacters>(gabi::ea(self) + u32(offsetof(hdGameOver_c, mCharacters)));
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    if (!characters) characters = hdGameOverAllocateCharacters(sizeof(hdGameOverCharacters));
    if (characters) { characters->mpItems = nullptr; characters->mCount = 0; }
    self->mRequested = 0;
    self->mCharacterCount = 0;
    self->mFinished = 0;
    return self;
}
/* 0266AE64 */
u32 hdGameOver_c::createCharacters() {
    hdPartsLayout_c* layout = mpGameLayout;
    if (!layout || !layout->mAccessor.v) return 0;
    hdPartsLyt_c* lyt = layout->lyt();
    auto* descriptions = gabi::at<hdPartsDescList_c>(gabi::ea(lyt) + u32(offsetof(hdPartsLyt_c, mPartsDescs)));
    if (!descriptions) return 1;
    const s32 count = descriptions->mCount;
    mCharacterCount = count;
    hdScreenHeap* heap = sGameOverUiSystem->mHeap;
    if (count > 0) {
        if (!heap) heap = hdParts_getCurrentHeap(sGameOverHeapManager);
        void* data = heap->allocate(u32(count) * sizeof(gptr<hdGameOverChar_c>));
        if (data) {
            mCharacters.mpItems = static_cast<gptr<hdGameOverChar_c>*>(data);
            mCharacters.mCount = count;
        }
    }
    const s32 initialized = mCharacters.mCount;
    for (u32 i = 0; i < u32(initialized) && initialized > 0; ++i) *mCharacters.slot(i) = nullptr;
    return hdGameOverBindVisitor(this, descriptions);
}
/* 0266AF9C */
u32 hdGameOver_c::initializeGameOver(u32 resource, u32 option) {
    hdGameOverAttachArchives(this, &sGameOverArchives.mNames[0], 2);
    hdScreenHeap* heap = sGameOverUiSystem->mHeap;
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpGameLayout = layout;
    layout->vLoad(&sGameOverLayoutName, gabi::ea(mpResAccessor.get()), resource, option, 0, 0, nullptr);
    createCharacters();
    startState(&sGameOverCompleteState);
    return 1;
}
/* 0266B080 */
void hdGameOver_c::releaseGameOver() {
    s32 count = mCharacters.mCount;
    gptr<hdGameOverChar_c>* items = mCharacters.mpItems;
    for (u32 i = 0; s32(i) < count; ++i) {
        const u32 index = i < u32(count) ? i : 0;
        hdGameOverChar_c* character = *mCharacters.slot(index);
        if (character) {
            character = *mCharacters.slot(index);
            character->vFinalize();
            count = mCharacters.mCount;
            character = *mCharacters.slot(i < u32(count) ? i : 0);
            if (character) {
                character->vDelete(3);
                count = mCharacters.mCount;
            }
            *mCharacters.slot(i < u32(count) ? i : 0) = nullptr;
            count = mCharacters.mCount;
            items = mCharacters.mpItems;
        }
    }
    if (items) {
        hdScreenHeap* heap = hdParts_findContainHeap(sGameOverHeapManager, items);
        heap->free(mCharacters.mpItems);
        mCharacters.mCount = 0;
        mCharacters.mpItems = nullptr;
    }
    hdPartsLayout_c* layout = mpGameLayout;
    if (layout) {
        layout->release();
        layout = mpGameLayout;
        if (layout) layout->destroy(3);
        mpGameLayout = nullptr;
    }
    hdGameOverFinalizeBase(this);
}
/* 0266B204 */
void hdGameOver_c::executeCharacters() {
    s32 count = mCharacters.mCount;
    for (u32 i = 0; s32(i) < count; ++i) {
        const u32 index = i < u32(count) ? i : 0;
        hdGameOverChar_c* character = *mCharacters.slot(index);
        if (character) {
            character = *mCharacters.slot(index);
            character->vExecuteState();
            count = mCharacters.mCount;
        }
    }
}
/* 0266B2AC */
void hdGameOver_c::executeGameOver() {
    hdParts_executeStateMachine(stateMachine());
    hdPartsLayout_c* layout = mpGameLayout;
    if (layout) layout->calc();
    executeCharacters();
}
/* 0266B300 */
void hdGameOver_c::drawGameOver(void* info) {
    hdPartsLayout_c* layout = mpGameLayout;
    if (layout) layout->draw(info);
}
/* 0266B31C */
void hdGameOver_c::createCharacter(void* context) {
    hdScreenHeap* heap = sGameOverUiSystem->mHeap;
    hdMapHeapManager_l* manager = sGameOverContextManager;
    const u32 index = manager->mCount > 3 ? 3 : 0;
    auto* selectedSlot = gabi::at<gptr<hdMapHeapContext_l>>(gabi::ea(manager->mpContexts.get()) + index * sizeof(gptr<hdMapHeapContext_l>));
    hdMapHeapContext_l* selected = *selectedSlot;
    gptr<hdGameOverChar_c>* items = mCharacters.mpItems;
    auto* character = static_cast<hdGameOverChar_c*>(hdParts_new(sizeof(hdGameOverChar_c), heap, 4));
    if (character) character = hdGameOverConstructCharacter(character);
    *items = character;
    const u32 flags = selected->mFlags;
    items = mCharacters.mpItems;
    void* primary;
    if (flags & 0x1000) primary = selected->mpSpecial;
    else {
        primary = selected->mpPrimary;
        if (!primary) primary = &sGameOverDefaultPrimary;
    }
    void* secondary;
    if ((flags & 0x40) && (flags & 0x80)) {
        secondary = gabi::at<void>(gabi::ea(selected) + u32(offsetof(hdMapHeapContext_l, mEmbedded)));
    } else {
        secondary = selected->mpSecondary;
        if (!secondary) secondary = &sGameOverDefaultSecondary;
    }
    character = *items;
    hdGameOverInitializeCharacter(character, mpResAccessor, gabi::ea(primary), gabi::ea(secondary), context);
}
/* 0266B45C */
static void initializeGameOverStatics() {
    sGameOverRuntime.mWords[3] = 0;
    sGameOverRuntime.mWords[2] = 0;
    sGameOverRuntime.mWords[1] = 0;
    sGameOverRuntime.mWords[0] = 0;
    hdGameOverRegisterStatics(&sGameOverRuntimeRegistration);
    sGameOverMinimumAngle = -3.1415927410125732f;
    sGameOverMaximumAngle = 3.1415927410125732f;
    hdGameOverConstructFirstContext(&sGameOverFirstContext);
    hdGameOverRegisterStatics(&sGameOverFirstRegistration);
    hdGameOverConstructSecondContext(&sGameOverSecondContext);
    hdGameOverRegisterStatics(&sGameOverSecondRegistration);
    sGameOverLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sGameOverLayoutName.mText.v = gabi::guest_addr("GameOver_00");
    sGameOverArchives.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sGameOverArchives.mNames[0].mText.v = gabi::guest_addr("GameOver_00");
    sGameOverArchives.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sGameOverArchives.mNames[1].mText.v = gabi::guest_addr("GameOverChar_00");
    u32 number;
    if (sGameOverCounterReady.mValue) number = u32(sGameOverNextState.mValue) + 1;
    else { sGameOverCounterReady.mValue = 1; number = 1; }
    auto* stateVtable = &sStateVtable;
    sGameOverCompleteState.setup(number, "::StateID_Wait", stateVtable, 0x19, &sGameOverNullState);
    sGameOverNextState.mValue = number + 1;
    sGameOverOutState.setup(number + 1, "::StateID_Disp", stateVtable, 0x1C, &sGameOverNullState);
}
/* WWHD glue
data sStringVtable 100F4CE4 nocheck
data sStateVtable 100F4D8C nocheck
data sScreenVtable 100F4E20 nocheck
data sStateHolderVtable 100F4F18 nocheck
data sSecondaryVtable 100F4F28 nocheck
data sGameOverContextManager 101F95D0
data sGameOverDefaultPrimary 104A20FC
data sGameOverDefaultSecondary 104A2098
ext 0266BA5C hdGameOverChar_c* hdGameOverConstructCharacter(hdGameOverChar_c* self)
ext 0266BAEC u32 hdGameOverInitializeCharacter(hdGameOverChar_c* self, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* context)
data sGameOverUiSystem 1018C404
data sGameOverHeapManager 101F8B4C
ext 026F91F8 void hdGameOverFinalizeBase(hdParts_c* self)
ext 02003350 u32 hdGameOverBindVisitor(hdParts_c* self, hdPartsDescList_c* descriptions)
ext 026F90D8 void hdGameOverAttachArchives(hdParts_c* self, hdUiSafeString* names, u32 count)
data sGameOverRuntime 10494314
data sGameOverRuntimeRegistration 101F5F44
data sGameOverFirstRegistration 101F5F50
data sGameOverSecondRegistration 101F5F5C
data sGameOverFirstContext 10494310
data sGameOverSecondContext 10494311
data sGameOverMinimumAngle 10494308
data sGameOverMaximumAngle 1049430C
data sGameOverArchives 10494324
data sGameOverLayoutName 10494394
data sGameOverCounterReady 101FD8E4
data sGameOverNextState 101FDD50
data sGameOverNullState 101FF32C
str 100F4DF4 "GameOver_00" in 0266B45C
str 100F4DE4 "GameOverChar_00" in 0266B45C
str 100F4E00 "::StateID_Wait" in 0266B45C
str 100F4E10 "::StateID_Disp" in 0266B45C
ext 028F026C void hdGameOverRegisterStatics(hdGameOverRegistration* registration)
ext 028ED6F8 void hdGameOverConstructFirstContext(hdGameOverContext* context)
ext 028EAB2C void hdGameOverConstructSecondContext(hdGameOverContext* context)
data sGameOverOutState 10494364
data sGameOverCompleteState 10494334
ext 026F88F0 void hdGameOverDestroyBase(hdParts_c* screen, u32 flags)
ext 026F89E8 void hdGameOverConstructBase(hdParts_c* screen)
ext 0273AD10 hdGameOver_c* hdGameOverAllocate(u32 size)
ext 0273AD10 hdGameOverCharacters* hdGameOverAllocateCharacters(u32 size)
ext 0273AF40 void hdGameOverDelete(void* screen)
*/
