/** HD Valid UI part: change animation and valid/invalid idle loops. Private decomp source. */
#include "d/hd_parts_valid.h"
static hdPartsStateId_c sStateChangeValid;
static hdPartsStateId_c sStateValid;
static hdPartsStateId_c sStateInvalid;

/* 02707290 */
void hdPartsValid_c::initializeState_ChangeValid() {
    f32 rate = mValid ? -1.0f : 1.0f;
    hdAnimSet_play(animSet(), CHANGE_VALID, CHANGE_VALID, rate);
}

/* 027072C0 */
void hdPartsValid_c::executeState_ChangeValid() {
    if (hdAnimSet_isEnd(animSet(), CHANGE_VALID)) {
        changeState(mValid ? &sStateValid : &sStateInvalid);
    }
}

/* 02707324 */
void hdPartsValid_c::initializeState_Valid() {
    hdAnimSet_play(animSet(), LOOP_ALL, LOOP_ALL, 1.0f);
    hdAnimSet_play(animSet(), LOOP_VALID, LOOP_VALID, 1.0f);
}

/* 0270739C */
void hdPartsValid_c::finalizeState_Valid() {
    ValidAnimation_stop(animSet(), LOOP_ALL);
    ValidAnimation_stop(animSet(), LOOP_VALID);
}

/* 027073E4 */
void hdPartsValid_c::initializeState_Invalid() {
    hdAnimSet_play(animSet(), LOOP_ALL, LOOP_ALL, 1.0f);
    hdAnimSet_play(animSet(), LOOP_INVALID, LOOP_VALID, 1.0f);
}

/* 0270745C */
void hdPartsValid_c::finalizeState_Invalid() {
    ValidAnimation_stop(animSet(), LOOP_ALL);
    ValidAnimation_stop(animSet(), LOOP_VALID);
}

/* 0270762C */
void hdPartsValid_c::changeValid(u32 valid) {
    mValid = valid;
    changeState(&sStateChangeValid);
}

/* 02707640 */
void hdPartsValid_c::setValid(u32 valid) {
    hdPartsLayout_c* layout = mpLayout;
    mValid = valid;
    hdAnimSet* animation = layout->animSet();
    if (valid) {
        ValidAnimation_setEnd(animation, CHANGE_VALID, CHANGE_VALID);
        changeState(&sStateValid);
    } else {
        ValidAnimation_setStart(animation, CHANGE_VALID, CHANGE_VALID);
        changeState(&sStateInvalid);
    }
}

/* 027076B0 */
u8 hdPartsValid_c::isValid() const {
    return mValid;
}

static u32 sDelegateVTableA;
static u32 sDelegateVTableB;
static hdPartsVtbl_c sValidVTable;
static hdScreenUiSystem* sHeap;
extern void* Valid_allocate(u32);
extern void Valid_delete(void*);

/* 02707224 */
static void hdPartsValid_destroy(hdPartsValid_c* part, u32 flags) {
    if (part) {
        part->mStateHolder.mHolderVtable = &sDelegateVTableA;
        part->mSecondaryVtable = &sDelegateVTableB;
        hdParts_c::destroy(part, 0);
        if (flags & 1) Valid_delete(part);
    }
}

/* 027074A4 */
static hdPartsValid_c* hdPartsValid_createPart(hdPartsValid_c* part) {
    if (!part) {
        part = static_cast<hdPartsValid_c*>(Valid_allocate(sizeof(hdPartsValid_c)));
        if (!part) return NULL;
    }
    hdParts_c::construct(part);
    part->mStateHolder.mHolderVtable = &sDelegateVTableA;
    part->mVtable = &sValidVTable;
    part->mSecondaryVtable = &sDelegateVTableB;
    part->mValid = 0;
    return part;
}

/* 02707518 */
u32 hdPartsValid_c::create(void* archive, void* name, void* parent, u32 flags) {
    if (!gabi::call_ptr<u32>(mVtable->mLoad.get(), this, archive, 0, name, parent, flags)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    hdPartsLayout_c* layout = mpLayout;
    mValid = 0;
    ValidAnimation_setStart(layout->animSet(), CHANGE_VALID, CHANGE_VALID);
    hdStateOwner* owner = mStateHolder.mMachine.mpOwner;
    hdStateObject* action = owner->createState((hdStateId*)&sStateInvalid);
    mStateHolder.mMachine.mpCurrentState = action;
    action->enter();
    return 1;
}

/* 027075E8 */
void hdPartsValid_c::update() {
    hdParts_executeStateMachine(stateMachine());
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
}

/* 027076B8 */
u32 hdPartsValid_c::createLayout(void* archive, void* name, void* parent, u32 flags, void* heap) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sHeap->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    gabi::call_ptr<void>(layout->vtbl()->mLoad.get(), layout, archive, name, parent, flags, 4, 3, heap);
    return 1;
}

struct ValidHeaderWords { be<u32> mWords[4]; };
struct ValidRange { be<f32> mLow, mHigh; };
struct ValidRegistration { u8 mBytes[12]; };
static ValidHeaderWords sHeaderWords;
static ValidRange sHeaderRange;
static f32 sLow;
static f32 sHigh;
static u8 sFirstObject;
static u8 sSecondObject;
static ValidRegistration sRegistrationA;
static ValidRegistration sRegistrationB;
static ValidRegistration sRegistrationC;
static hdUiSafeString sTypeName;
static hdUiStringTable sStringVTable;
static hdPartsStateIdVtbl_c sStateVTable;
static hdStateId sStateOwner;
static u32 sStateIdGuard;
static u32 sNextStateId;
extern void Valid_register(ValidRegistration*);
extern void Valid_constructFirst(be<u8>*);
extern void Valid_constructSecond(be<u8>*);

/* 0270776C */
static void __sinit_hd_parts_valid_cpp() {
    sHeaderWords.mWords[3] = 0;
    sHeaderWords.mWords[2] = 0;
    sHeaderWords.mWords[1] = 0;
    sHeaderWords.mWords[0] = 0;
    Valid_register(&sRegistrationA);
    f32 low = sLow;
    f32 high = sHigh;
    sHeaderRange.mLow = low;
    sHeaderRange.mHigh = high;
    Valid_constructFirst(&sFirstObject);
    Valid_register(&sRegistrationB);
    Valid_constructSecond(&sSecondObject);
    Valid_register(&sRegistrationC);
    u32 guard = sStateIdGuard;
    sTypeName.mVtable = &sStringVTable;
    sTypeName.mText.v = gabi::guest_addr("Valid");
    u32 id;
    if (guard) id = sNextStateId;
    else {
        id = 0;
        sStateIdGuard = 1;
    }
    sStateChangeValid.setup(id + 1, "::StateID_ChangeValid", &sStateVTable, 0x19, &sStateOwner);
    sStateValid.setup(id + 2, "::StateID_Valid", &sStateVTable, 0x1C, &sStateOwner);
    sStateInvalid.setup(id + 3, "::StateID_Invalid", &sStateVTable, 0x1F, &sStateOwner);
    sNextStateId = id + 3;
}

/* WWHD glue
 data sHeaderWords 1049E84C
 data sHeaderRange 1049E840
 data sLow 1013E618
 data sHigh 1013E61C
 data sFirstObject 1049E848
 data sSecondObject 1049E849
 data sRegistrationA 101F81FC
 data sRegistrationB 101F8208
 data sRegistrationC 101F8214
 data sTypeName 1049E8EC
 data sStringVTable 1013E564
 data sStateVTable 1013E5C4
 data sStateOwner 101FF32C
 data sStateIdGuard 101FD8E4
 data sNextStateId 101FDD50
 str 1013E620
 str 1013E638
 str 1013E628
 str 1013E650
 ext 028F026C void Valid_register(ValidRegistration*)
 ext 028ED6F8 void Valid_constructFirst(be<u8>*)
 ext 028EAB2C void Valid_constructSecond(be<u8>*)
 data sStateChangeValid 1049E85C
 data sStateValid 1049E88C
 data sStateInvalid 1049E8BC
 data sDelegateVTableA 1013E778
 data sDelegateVTableB 1013E788
 data sValidVTable 1013E668
 data sHeap 1018C404
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*, s32)
 ext 0273AD10 void* Valid_allocate(u32)
 ext 0273AF40 void Valid_delete(void*)
 ext 02005334 void ValidAnimation_stop(hdAnimSet*, u32)
 ext 02005674 void ValidAnimation_setEnd(hdAnimSet*, u32, u32)
 ext 02005708 void ValidAnimation_setStart(hdAnimSet*, u32, u32)
*/
