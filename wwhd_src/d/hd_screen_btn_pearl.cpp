/** HD collection pearl button: named pane, acquired flag and item/event identities. */
#include "d/hd_screen_btn_pearl.h"
static hdPartsVtbl_c sPearlVtable;
static u32 sPearlHolderVtable;
static u32 sPearlSecondaryVtable;
static hdUiStringTable sPearlStringVtable;
struct hdPearlItemIds_l { be<u32> mItem, mEvent; };
static hdPearlItemIds_l sPearlItemIds[3] = {};
static hdUiSafeString sPearlAnimationName;
static hdUiSafeString sPearlPaneName;
static hdUiSafeString sPearlLayoutName;
static u32 sPearlInitializerWords[4] = {};
static u32 sPearlRegistration[3][3] = {};
static f32 sPearlHeaderScale;
static f32 sPearlHeaderRate;
static u8 sPearlHeaderByte;
static u8 sPearlHeaderBool;
struct hdPearlUiSlot_l { gptr<hdScreenUiSystem> mpSystem; };
struct hdPearlHeapSlot_l { gptr<void> mpManager; };
struct hdPearlSavePrefix_l { u8 mPrefix[0xD4]; be<u8> mPearlFlags; };
WWHD_OFFSET(hdPearlSavePrefix_l, mPearlFlags, 0xD4);
static hdPearlUiSlot_l sPearlUi;
static hdPearlHeapSlot_l sPearlHeap;
struct hdPearlSaveSlot_l { gptr<hdPearlSavePrefix_l> mpInfo; };
static hdPearlSaveSlot_l sPearlSave;
static f32 sPearlHeaderScaleSource;
static f32 sPearlHeaderRateSource;
u32 pearlIsObtained(void*, u32);
void pearlRegisterStatic(void*);
void pearlConstructByte(void*);
void pearlConstructBool(void*);
struct hdPearlButtonVtable_l { u8 mBaseSlots[0x1F4]; gfn mSetEnabled; };
static void pearlSetEnabled(hdPearlButton_c* button, u32 enabled) {
    auto* table = hdPartsView<hdPearlButtonVtable_l>(button->mVtable.v);
    gabi::call_ptr<void>(table->mSetEnabled.get(), button, enabled);
}
/* 02641A08 */
void hdPearlButton_c::destroy(hdPearlButton_c* button, s32 flags) {
    if (!button) return;
    button->mStateHolder.mHolderVtable = &sPearlHolderVtable;
    button->mSecondaryVtable = &sPearlSecondaryVtable;
    button->NoDecideButton_l::destroy(0);
    if (flags & 1) operator_delete(button);
}
/* 02641A74 */
hdPearlButton_c* hdPearlButton_c::construct(hdPearlButton_c* button) {
    if (!button) { button = static_cast<hdPearlButton_c*>(operator_new(sizeof(hdPearlButton_c))); if (!button) return nullptr; }
    NoDecideButton_l::construct(button);
    button->mVtable = &sPearlVtable;
    button->mStateHolder.mHolderVtable = &sPearlHolderVtable;
    auto* panes = gabi::at<hdScreenPaneArray>(gabi::ea(button) + offsetof(hdPearlButton_c, mPearlPanes));
    button->mSecondaryVtable = &sPearlSecondaryVtable;
    if (!panes) panes = static_cast<hdScreenPaneArray*>(operator_new(8));
    if (panes) { panes->mData = nullptr; panes->mCount = 0; }
    button->mObtained = 0;
    button->mPearlIndex = 0;
    return button;
}
/* 02641B14 */
u32 hdPearlButton_c::load(void* resource, u32 options, u32 descriptor) {
    if (!NoDecideButton_l::create(&sPearlLayoutName, resource, options, descriptor)) return 0;
    hdScreenHeap* heap = sPearlUi.mpSystem->mHeap;
    if (!heap) heap = hdParts_getCurrentHeap(sPearlHeap.mpManager);
    void* storage = heap->allocate(4);
    if (storage) { mPearlPanes.mData = static_cast<gptr<void>*>(storage); mPearlPanes.mCount = 1; }
    const s32 count = s32(mPearlPanes.mCount);
    for (s32 i = 0; i < count; ++i) mPearlPanes.mData.get()[i] = nullptr;
    for (u32 i = 0; s32(i) < s32(mPearlPanes.mCount); ++i) {
        auto* slot = hdPartsView<gptr<void>>(gabi::ea(mPearlPanes.mData.get()) + (i < u32(mPearlPanes.mCount) ? i * 4 : 0));
        hdPartsLyt_c* layout = mpLayout->lyt();
        sPearlPaneName.assureTermination();
        hdPartsPane_c* root = layout->mpRootPane;
        auto* table = hdPartsView<hdScreenResourceTable>(root->mVtable.v);
        *slot = gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, sPearlPaneName.mText.get(), 1);
        slot = hdPartsView<gptr<void>>(gabi::ea(mPearlPanes.mData.get()) + (i < u32(mPearlPanes.mCount) ? i * 4 : 0));
        auto* pane = hdPartsView<hdCollectionPane_l>(gabi::ea(slot->get()));
        pane->mFlags = u8(pane->mFlags) & 0xFE;
    }
    pearlSetEnabled(this, 0);
    return 1;
}
/* 02641CAC */
void hdPearlButton_c::unload() {
    const s32 count = s32(mPearlPanes.mCount);
    for (s32 i = 0; i < count; ++i) mPearlPanes.mData.get()[i] = nullptr;
    if (s32(mPearlPanes.mCount) > 0 || mPearlPanes.mData.get()) {
        hdScreenHeap* heap = hdParts_findContainHeap(sPearlHeap.mpManager, mPearlPanes.mData.get());
        heap->free(mPearlPanes.mData.get());
        mPearlPanes.mCount = 0;
        mPearlPanes.mData = nullptr;
    }
    NoDecideButton_l::reset();
}
/* 02641D54 */
void hdPearlButton_c::updateObtained() {
    const u32 obtained = pearlIsObtained(hdPartsView<void>(gabi::ea(sPearlSave.mpInfo.get()) + offsetof(hdPearlSavePrefix_l, mPearlFlags)), u8(mPearlIndex)) != 0;
    if (u8(mObtained) == obtained) return;
    mObtained = obtained;
    auto* pane = hdPartsView<hdCollectionPane_l>(gabi::ea(mPearlPanes.mData.get()[0].get()));
    pane->mFlags = (u8(pane->mFlags) & 0xFE) + obtained;
    pearlSetEnabled(this, obtained);
}
/* 02641E1C */
u32 hdPearlButton_c::itemId() const {
    u32 index = mPearlIndex;
    if (index == 3) return 0xFFFFFFFF;
    return hdPartsView<hdPearlItemIds_l>(gabi::ea(sPearlItemIds) + index * 8)->mItem;
}
/* 02641E3C */
u32 hdPearlButton_c::eventId() const {
    u32 index = mPearlIndex;
    if (index == 3) return 0xFFFFFFFF;
    return hdPartsView<hdPearlItemIds_l>(gabi::ea(sPearlItemIds) + index * 8)->mEvent;
}
/* 02641E60 */
static void initializePearlButtonStatics() {
    sPearlInitializerWords[2] = 0; sPearlInitializerWords[0] = 0;
    sPearlInitializerWords[3] = 0; sPearlInitializerWords[1] = 0;
    pearlRegisterStatic(sPearlRegistration[0]);
    sPearlHeaderScale = sPearlHeaderScaleSource;
    sPearlHeaderRate = sPearlHeaderRateSource;
    pearlConstructByte(&sPearlHeaderByte);
    pearlRegisterStatic(sPearlRegistration[1]);
    pearlConstructBool(&sPearlHeaderBool);
    pearlRegisterStatic(sPearlRegistration[2]);
    sPearlAnimationName.mVtable = &sPearlStringVtable;
    sPearlAnimationName.mText.v = gabi::guest_addr("G_Btn_00");
    sPearlPaneName.mVtable = &sPearlStringVtable;
    sPearlPaneName.mText.v = gabi::guest_addr("N_Btn_00");
    sPearlLayoutName.mVtable = &sPearlStringVtable;
    sPearlLayoutName.mText.v = gabi::guest_addr("BtnPearl_00");
}
/* WWHD glue
 data sPearlVtable 100EBB60 nocheck
 data sPearlHolderVtable 100EBD60 nocheck
 data sPearlSecondaryVtable 100EBD70 nocheck
 data sPearlStringVtable 100EBAFC nocheck
 ext 02700C9C void NoDecideButton_l::destroy(u32 flags)
 ext 02701718 NoDecideButton_l* NoDecideButton_l::construct(NoDecideButton_l* part)
 ext 0270178C u32 NoDecideButton_l::create(void* archive,void* resource,u32 mode,u32 flags)
 ext 02701984 void NoDecideButton_l::reset()
 ext 025B7D90 u32 pearlIsObtained(void*,u32)
 ext 028F026C void pearlRegisterStatic(void*)
 ext 028ED6F8 void pearlConstructByte(void*)
 ext 028EAB2C void pearlConstructBool(void*)
 data sPearlItemIds 100EBB14 nocheck
 data sPearlAnimationName 10490A04 nocheck
 data sPearlPaneName 10490A0C nocheck
 data sPearlLayoutName 10490A30 nocheck
 data sPearlInitializerWords 10490A20 nocheck
 data sPearlRegistration 101F5720 nocheck
 data sPearlHeaderScale 10490A14 nocheck
 data sPearlHeaderRate 10490A18 nocheck
 data sPearlHeaderByte 10490A1C nocheck
 data sPearlHeaderBool 10490A1D nocheck
 data sPearlUi 1018C404 nocheck
 data sPearlHeap 101F8B4C nocheck
 data sPearlSave 101F84DC nocheck
 data sPearlHeaderScaleSource 100EBB34 nocheck
 data sPearlHeaderRateSource 100EBB38 nocheck
 str 100EBB48 "G_Btn_00" in 02641E60
 str 100EBB54 "N_Btn_00"
 str 100EBB3C "BtnPearl_00" in 02641E60
*/
