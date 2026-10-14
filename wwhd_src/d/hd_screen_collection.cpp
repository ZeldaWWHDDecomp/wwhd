/** HD collection child identities, creation and directional navigation. */
#include "d/hd_screen_collection.h"
static hdPartsVtbl_c sCollectionVtable;
static u32 sCollectionHolderVtable;
static u32 sCollectionSecondaryVtable;
static hdUiStringTable sCollectionStringVtable;
static hdPartsStateId_c sCollectionPartsDisplayState;
static hdUiSafeString sCollectionChildNames[4] = {};
static hdUiSafeString sCollectionChildTypes[4] = {};
static hdUiSafeString sCollectionLayoutName;
static hdUiSafeString sCollectionPearlType;
static hdUiSafeString sCollectionTriforceType;
struct hdCollectionUiSlot_l { gptr<hdScreenUiSystem> mpSystem; };
static hdCollectionUiSlot_l sCollectionUi;
/* 026470E4 */
static u32 collectionStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026471E4 */
void hdCollection_c::destroy(hdCollection_c* screen, s32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &sCollectionHolderVtable;
    screen->mSecondaryVtable = &sCollectionSecondaryVtable;
    screen->NoControlPart_l::destroy(0);
    if (flags&1) operator_delete(screen);
}
/* 02647250 */
hdCollection_c* hdCollection_c::construct(hdCollection_c* screen) {
    if (!screen) { screen = static_cast<hdCollection_c*>(operator_new(sizeof(hdCollection_c))); if (!screen) return nullptr; }
    NoControlPart_l::construct(screen);
    screen->mVtable = &sCollectionVtable;
    screen->mStateHolder.mHolderVtable = &sCollectionHolderVtable;
    screen->mSecondaryVtable = &sCollectionSecondaryVtable;
    return screen;
}
static hdParts_c* collectionChild(gptr<hdParts_c>* data, u32 count, u32 index) { return data[index<count?index:0].get(); }
static hdButtonPart_c* collectionButton(hdParts_c* child) { return hdPartsView<hdButtonPart_c>(gabi::ea(child)); }
/* 026472BC */
void hdCollection_c::configureNeighbors() {
    u32 count = mChildren.mNum;
    auto* data = mChildren.mpData.get();
    auto* button = collectionButton(collectionChild(data, count, 3));
    u32 last = collectionButton(collectionChild(data, count, 2))->mpReceiver.v;
    button->setReceiverButtons(0, 0, 0, 0, 0, 0, 0, last);
    count = mChildren.mNum; data = mChildren.mpData.get();
    button = collectionButton(collectionChild(data, count, 0));
    u32 first = collectionButton(collectionChild(data, count, 2))->mpReceiver.v;
    u32 next = collectionButton(collectionChild(data, count, 3))->mpReceiver.v;
    last = collectionButton(collectionChild(data, count, 1))->mpReceiver.v;
    button->setReceiverButtons(first, 0, 0, 0, 0, 0, next, last);
    count = mChildren.mNum; data = mChildren.mpData.get();
    button = collectionButton(collectionChild(data, count, 1));
    first = collectionButton(collectionChild(data, count, 2))->mpReceiver.v;
    next = collectionButton(collectionChild(data, count, 0))->mpReceiver.v;
    button->setReceiverButtons(first, 0, 0, 0, 0, 0, next, 0);
    count = mChildren.mNum; data = mChildren.mpData.get();
    button = collectionButton(collectionChild(data, count, 2));
    u32 previous = collectionButton(collectionChild(data, count, 1))->mpReceiver.v;
    next = collectionButton(collectionChild(data, count, 3))->mpReceiver.v;
    button->setReceiverButtons(0, 0, 0, previous, 0, 0, next, 0);
}
/* 026474A8 */
u32 hdCollection_c::load(void* resource, u32 options, u32 descriptor) {
    if (!NoControlPart_l::create(&sCollectionLayoutName, resource, options, descriptor)) return 0;
    configureNeighbors();
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(0)->get()))->mPearlIndex = 1;
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(1)->get()))->mPearlIndex = 2;
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(2)->get()))->mPearlIndex = 0;
    startState(&sCollectionPartsDisplayState);
    return 1;
}
/* 0264759C */
void hdCollection_c::synchronizeButtons() {
    for (u32 i = 0; i<4; ++i) {
        hdParts_c* child = childSlot(i)->get();
        if (child) gabi::call_ptr<void>(hdButtonVtable(collectionButton(child))->mOnFocusChanged.get(), child);
    }
}
/* 0264761C */
u32 hdCollection_c::findReceiver(u32 receiver) {
    for (u32 i = 0; i<4; ++i) {
        hdParts_c* child = childSlot(i)->get();
        if (child && collectionButton(child)->getReceiverId() == receiver) return i;
    }
    return 4;
}
/* 026476AC */
void hdCollection_c::updateObtained() {
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(0)->get()))->updateObtained();
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(1)->get()))->updateObtained();
    hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(2)->get()))->updateObtained();
    hdPartsView<hdTriforceButton_c>(gabi::ea(childSlot(3)->get()))->updateFragments();
}
static bool collectionStringEquals(hdUiSafeString* incoming, hdUiSafeString* expected) {
    incoming->assureTermination();
    incoming->assureTermination();
    auto* text = incoming->mText.get();
    expected->assureTermination();
    if (text == expected->mText.get()) return true;
    text = incoming->mText.get();
    auto* reference = expected->mText.get();
    for (u32 i = 0; i<0x40001; ++i) {
        const u8 byte = *gabi::at<be<u8>>(gabi::ea(text)+i);
        const u8 other = *gabi::at<be<u8>>(gabi::ea(reference)+i);
        if (byte != other) return false;
        if (!byte) return true;
    }
    return false;
}
/* 0264773C */
u32 hdCollection_c::createCollectionChild(hdPartsDesc_c* descriptor) {
    hdScreenHeap* heap = sCollectionUi.mpSystem->mHeap;
    auto* type = hdPartsView<hdUiSafeString>(gabi::ea(descriptor)+8);
    auto* name = hdPartsView<hdUiSafeString>(gabi::ea(descriptor)+0x14);
    for (u32 i = 0; i<4; ++i) {
        if (!collectionStringEquals(name, &sCollectionChildNames[i])) continue;
        if (collectionStringEquals(type, &sCollectionPearlType)) {
            auto* slot = childSlot(i);
            auto* button = static_cast<hdPearlButton_c*>(hdParts_new(sizeof(hdPearlButton_c), heap, 4));
            if (button) button = hdPearlButton_c::construct(button);
            *slot=button;
            if (!childSlot(i)->get()) return 0;
            return hdPartsView<hdPearlButton_c>(gabi::ea(childSlot(i)->get()))->load(0, 0, gabi::ea(descriptor));
        }
        if (!collectionStringEquals(type, &sCollectionTriforceType)) return createChild(i, descriptor);
        auto* slot = childSlot(i);
        auto* button = static_cast<hdTriforceButton_c*>(hdParts_new(sizeof(hdTriforceButton_c), heap, 4));
        if (button) button = hdTriforceButton_c::construct(button);
        *slot=button;
        if (!childSlot(i)->get()) return 0;
        return hdPartsView<hdTriforceButton_c>(gabi::ea(childSlot(i)->get()))->load(0, 0, gabi::ea(descriptor));
    }
    return 1;
}
static u32 sCollectionInitializerWords[4] = {};
static u32 sCollectionRegistration[3][3] = {};
static f32 sCollectionHeaderScale;
static f32 sCollectionHeaderRate;
static f32 sCollectionScaleSource;
static f32 sCollectionRateSource;
static u8 sCollectionHeaderByte;
static u8 sCollectionHeaderBool;
void collectionRegisterStatic(void*);
void collectionConstructByte(void*);
void collectionConstructBool(void*);
/* 02647ABC */
static void initializeCollectionStatics() {
    sCollectionInitializerWords[3] = 0; sCollectionInitializerWords[2] = 0;
    sCollectionInitializerWords[1] = 0; sCollectionInitializerWords[0] = 0;
    collectionRegisterStatic(sCollectionRegistration[0]);
    sCollectionHeaderScale = sCollectionScaleSource; sCollectionHeaderRate = sCollectionRateSource;
    collectionConstructByte(&sCollectionHeaderByte); collectionRegisterStatic(sCollectionRegistration[1]);
    collectionConstructBool(&sCollectionHeaderBool); collectionRegisterStatic(sCollectionRegistration[2]);
    sCollectionChildNames[0].mVtable = &sCollectionStringVtable; sCollectionChildNames[0].mText.v = gabi::guest_addr("L_Pearl_00");
    sCollectionChildNames[1].mVtable = &sCollectionStringVtable; sCollectionChildNames[1].mText.v = gabi::guest_addr("L_Pearl_01");
    sCollectionChildNames[2].mVtable = &sCollectionStringVtable; sCollectionChildNames[2].mText.v = gabi::guest_addr("L_Pearl_02");
    sCollectionChildNames[3].mVtable = &sCollectionStringVtable; sCollectionChildNames[3].mText.v = gabi::guest_addr("L_Triforce_00");
    sCollectionLayoutName.mVtable = &sCollectionStringVtable; sCollectionLayoutName.mText.v = gabi::guest_addr("Collection_00");
    for (u32 i = 0; i<3; ++i) { sCollectionChildTypes[i].mVtable = &sCollectionStringVtable; sCollectionChildTypes[i].mText.v = gabi::guest_addr("BtnPearl_00"); }
    sCollectionChildTypes[3].mVtable = &sCollectionStringVtable; sCollectionChildTypes[3].mText.v = gabi::guest_addr("BtnTriforce_00");
}
/* WWHD glue
 data sCollectionVtable 100ED6C8 nocheck
 data sCollectionHolderVtable 100ED790 nocheck
 data sCollectionSecondaryVtable 100ED7A0 nocheck
 data sCollectionStringVtable 100ED63C nocheck
 data sCollectionPartsDisplayState 1049DD90 nocheck
 ext 028F026C void collectionRegisterStatic(void*)
 ext 028ED6F8 void collectionConstructByte(void*)
 ext 028EAB2C void collectionConstructBool(void*)
 data sCollectionChildNames 104910B0 nocheck
 data sCollectionChildTypes 104910D0 nocheck
 data sCollectionLayoutName 104910F0 nocheck
 data sCollectionPearlType 10490A30 nocheck
 data sCollectionTriforceType 10490E40 nocheck
 data sCollectionUi 1018C404 nocheck
 data sCollectionInitializerWords 104910A0 nocheck
 data sCollectionRegistration 101F5840 nocheck
 data sCollectionHeaderScale 10491094 nocheck
 data sCollectionHeaderRate 10491098 nocheck
 data sCollectionScaleSource 100ED65C nocheck
 data sCollectionRateSource 100ED660 nocheck
 data sCollectionHeaderByte 1049109C nocheck
 data sCollectionHeaderBool 1049109D nocheck
 str 100ED690 "L_Pearl_00"
 str 100ED69C "L_Pearl_01"
 str 100ED6A8 "L_Pearl_02"
 str 100ED670 "L_Triforce_00"
 str 100ED680 "Collection_00"
 str 100ED664 "BtnPearl_00" in 02647ABC
 str 100ED6B4 "BtnTriforce_00" in 02647ABC
 ext 02700A70 void NoControlPart_l::destroy(u32 flags)
 ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l* part)
 ext 02700B48 u32 NoControlPart_l::create(void* archive,void* resource,u32 mode,u32 flags)
 ext 026F5C08 void hdButtonPart_c::setReceiverButtons(u32,u32,u32,u32,u32,u32,u32,u32)
 ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
 ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
*/
