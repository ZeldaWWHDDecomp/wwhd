/** HD collection Triforce acquisition panes and completion animation. */
#include "d/hd_screen_btn_triforce.h"
static hdPartsVtbl_c sTriforceVtable;
static u32 sTriforceHolderVtable;
static u32 sTriforceSecondaryVtable;
static hdUiStringTable sTriforceStringVtable;
static hdPartsStateIdVtbl_c sTriforceStateVtable;
static hdStateId sTriforceNullState;
static hdPartsStateId_c sTriforceCompleteDemo;
static hdPartsStateId_c sTriforceComplete;
static hdUiSafeString sTriforceLayoutName;
static hdUiSafeString sTriforceCompletePaneNames[1] = {};
static hdUiSafeString sTriforceFragmentPaneNames[10] = {};
static hdUiSafeString sTriforceAnimationNames[2] = {};
static hdUiSafeString sTriforceGroupNames[3] = {};
static u32 sTriforceAnimationSlots[2] = {};
struct hdTriforceIdentity_l { be<u32> mItem, mEvent; };
static hdTriforceIdentity_l sTriforceIdentities[2] = {};
static f32 sTriforceFrame;
struct hdTriforceUiSlot_l { gptr<hdScreenUiSystem> mpSystem; };
struct hdTriforceHeapSlot_l { gptr<void> mpManager; };
struct hdTriforceSavePrefix_l { u8 mPrefix[0xD4]; be<u8> mFragments; };
struct hdTriforceSaveSlot_l { gptr<hdTriforceSavePrefix_l> mpInfo; };
static hdTriforceUiSlot_l sTriforceUi;
static hdTriforceHeapSlot_l sTriforceHeap;
static hdTriforceSaveSlot_l sTriforceSave;
void triforceSetFrame(hdAnimSet*, u32, u32, f32);
u32 triforceAnimationEnded(hdAnimSet*, u32);
void triforceStartAnimation(hdAnimSet*, u32, u32);
void triforceStopAnimation(hdAnimSet*, u32);
void triforcePlaySound(const char*);
void triforceNotify(u32);
u32 triforceIsObtained(void*, u32);
u32 triforceFindPanes(hdPartsLayout_c*, hdScreenPaneArray*, hdUiSafeString*);
u32 triforceFindPicturePanes(hdPartsLayout_c*, hdScreenPaneArray*, hdUiSafeString*);
void triforceBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
struct hdTriforceVtable_l { u8 mPrefix[0x1F4]; gfn mSetEnabled; };
static void triforceSetEnabled(hdTriforceButton_c* button, u32 enabled) {
    auto* table = hdPartsView<hdTriforceVtable_l>(button->mVtable.v);
    gabi::call_ptr<void>(table->mSetEnabled.get(), button, enabled);
}
static hdCollectionPane_l* triforcePane(hdScreenPaneArray& panes, u32 index) {
    auto* slot = hdPartsView<gptr<void>>(gabi::ea(panes.mData.get())+(index<u32(panes.mCount)?index*4:0));
    return hdPartsView<hdCollectionPane_l>(gabi::ea(slot->get()));
}
/* 02644740 */
void hdTriforceButton_c::destroy(hdTriforceButton_c* button, s32 flags) {
    if (!button) return;
    button->mStateHolder.mHolderVtable = &sTriforceHolderVtable;
    button->mSecondaryVtable = &sTriforceSecondaryVtable;
    button->NoDecideButton_l::destroy(0);
    if (flags&1) operator_delete(button);
}
/* 026447AC */
void hdTriforceButton_c::markComplete() { mFragmentFlags = u32(mFragmentFlags)|0x100; }
/* 026447BC */
void hdTriforceButton_c::initializeCompleteAnimation() { triforceSetFrame(mpLayout->animSet(), 5, 1, sTriforceFrame); }
/* 026447D8 */
void hdTriforceButton_c::calculateLayout() { vCalcLayout(); }
/* 026447E8 */
void hdTriforceButton_c::completeDemoEnter() {
    mFragmentFlags = u32(mFragmentFlags)&~0x100u;
    auto* pane = triforcePane(mFragmentPanes, 8);
    pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
    const f32 frame = sTriforceFrame;
    triforceSetFrame(mpLayout->animSet(), 6, 2, frame);
    pane = triforcePane(mCompletePanes, 0);
    pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
    triforceSetFrame(mpLayout->animSet(), 5, 1, frame);
    triforcePlaySound("SE_TRIFORCE_COMPLETE");
    triforceNotify(0x80000061);
}
/* 026448C4 */
void hdTriforceButton_c::completeDemoExecute() {
    if (triforceAnimationEnded(mpLayout->animSet(), 6)) changeState(&sTriforceComplete);
}
/* 02644914 */
hdTriforceButton_c* hdTriforceButton_c::construct(hdTriforceButton_c* button) {
    if (!button) { button = static_cast<hdTriforceButton_c*>(operator_new(sizeof(hdTriforceButton_c))); if (!button) return nullptr; }
    NoDecideButton_l::construct(button);
    button->mVtable = &sTriforceVtable;
    button->mStateHolder.mHolderVtable = &sTriforceHolderVtable;
    button->mSecondaryVtable = &sTriforceSecondaryVtable;
    auto* complete = gabi::at<hdScreenPaneArray>(gabi::ea(button)+offsetof(hdTriforceButton_c, mCompletePanes));
    if (!complete) complete = static_cast<hdScreenPaneArray*>(operator_new(8));
    if (complete) { complete->mData = nullptr; complete->mCount = 0; }
    auto* fragments = gabi::at<hdScreenPaneArray>(gabi::ea(button)+offsetof(hdTriforceButton_c, mFragmentPanes));
    if (!fragments) fragments = static_cast<hdScreenPaneArray*>(operator_new(8));
    if (fragments) { fragments->mData = nullptr; fragments->mCount = 0; }
    auto* flags = gabi::at<be<u32>>(gabi::ea(button)+offsetof(hdTriforceButton_c, mFragmentFlags));
    if (!flags) flags = static_cast<be<u32>*>(operator_new(4));
    if (flags) *flags = 0;
    return button;
}
/* 026449E8 */
u32 hdTriforceButton_c::load(void* resource, u32 options, u32 descriptor) {
    if (!NoDecideButton_l::create(&sTriforceLayoutName, resource, options, descriptor)) return 0;
    hdScreenHeap* ownerHeap = sTriforceUi.mpSystem->mHeap;
    hdScreenHeap* heap = ownerHeap;
    if (!heap) heap = hdParts_getCurrentHeap(sTriforceHeap.mpManager);
    void* memory = heap->allocate(4);
    if (memory) { mCompletePanes.mData = hdPartsView<gptr<void>>(gabi::ea(memory)); mCompletePanes.mCount = 1; }
    s32 count = s32(mCompletePanes.mCount);
    for (s32 i = 0; i<count; ++i) mCompletePanes.mData.get()[i] = nullptr;
    if (!triforceFindPanes(mpLayout, &mCompletePanes, sTriforceCompletePaneNames)) return 0;
    heap = ownerHeap;
    if (!heap) heap = hdParts_getCurrentHeap(sTriforceHeap.mpManager);
    memory = heap->allocate(0x28);
    if (memory) { mFragmentPanes.mData = hdPartsView<gptr<void>>(gabi::ea(memory)); mFragmentPanes.mCount = 10; }
    count = s32(mFragmentPanes.mCount);
    for (s32 i = 0; i<count; ++i) mFragmentPanes.mData.get()[i] = nullptr;
    if (!triforceFindPicturePanes(mpLayout, &mFragmentPanes, sTriforceFragmentPaneNames)) return 0;
    for (u32 i = 0; s32(i)<s32(mCompletePanes.mCount); ++i) { auto* pane = triforcePane(mCompletePanes, i); pane->mFlags = u8(pane->mFlags)&0xFE; }
    for (u32 i = 0; s32(i)<s32(mFragmentPanes.mCount); ++i) { auto* pane = triforcePane(mFragmentPanes, i); pane->mFlags = u8(pane->mFlags)&0xFE; }
    triforceSetEnabled(this, 0);
    return 1;
}
/* 02644C1C */
void hdTriforceButton_c::unload() {
    s32 count = s32(mFragmentPanes.mCount);
    for (s32 i = 0; i<count; ++i) mFragmentPanes.mData.get()[i] = nullptr;
    if (s32(mFragmentPanes.mCount)>0 || mFragmentPanes.mData.get()) {
        hdScreenHeap* heap = hdParts_findContainHeap(sTriforceHeap.mpManager, mFragmentPanes.mData.get());
        heap->free(mFragmentPanes.mData.get());
        mFragmentPanes.mCount = 0; mFragmentPanes.mData = nullptr;
    }
    count = s32(mCompletePanes.mCount);
    for (s32 i = 0; i<count; ++i) mCompletePanes.mData.get()[i] = nullptr;
    if (s32(mCompletePanes.mCount)>0 || mCompletePanes.mData.get()) {
        hdScreenHeap* heap = hdParts_findContainHeap(sTriforceHeap.mpManager, mCompletePanes.mData.get());
        heap->free(mCompletePanes.mData.get());
        mCompletePanes.mCount = 0; mCompletePanes.mData = nullptr;
    }
    NoDecideButton_l::reset();
}
/* 02644D44 */
void hdTriforceButton_c::updateFragment(u32 index) {
    const u32 obtained = triforceIsObtained(hdPartsView<void>(gabi::ea(sTriforceSave.mpInfo.get())+offsetof(hdTriforceSavePrefix_l, mFragments)), u8(index));
    const u32 bit = (index&0x20)?0u:(1u<<(index&31));
    const u32 flags = mFragmentFlags;
    if (obtained) {
        if (flags&bit) return;
        mFragmentFlags = flags|bit;
        auto* pane = triforcePane(mFragmentPanes, index); pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
        pane = triforcePane(mFragmentPanes, 9); pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
        triforceSetEnabled(this, 1);
    } else {
        if (!(flags&bit)) return;
        mFragmentFlags = flags&~bit;
        auto* pane = triforcePane(mFragmentPanes, index); pane->mFlags = u8(pane->mFlags)&0xFE;
        if (u8(mFragmentFlags)) return;
        pane = triforcePane(mFragmentPanes, 9); pane->mFlags = u8(pane->mFlags)&0xFE;
        triforceSetEnabled(this, 0);
    }
}
/* 02644EB4 */
void hdTriforceButton_c::updateFragments() { for (u32 i = 0; i<8; ++i) updateFragment(i); }
/* 02644F0C */
void hdTriforceButton_c::updateComplete() {
    const u32 flags = mFragmentFlags;
    if (u8(flags) == 0xFF) {
        if (flags&0x100) return;
        mFragmentFlags = flags|0x100;
        auto* pane = triforcePane(mFragmentPanes, 8); pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
        triforceStartAnimation(mpLayout->animSet(), 6, 2);
        pane = triforcePane(mCompletePanes, 0); pane->mFlags = (u8(pane->mFlags)&0xFE)+1;
        triforceSetFrame(mpLayout->animSet(), 5, 1, sTriforceFrame);
    } else {
        if (!(flags&0x100)) return;
        mFragmentFlags = flags&~0x100u;
        auto* pane = triforcePane(mFragmentPanes, 8); pane->mFlags = u8(pane->mFlags)&0xFE;
        triforceStopAnimation(mpLayout->animSet(), 2);
        pane = triforcePane(mCompletePanes, 0); pane->mFlags = u8(pane->mFlags)&0xFE;
        triforceStopAnimation(mpLayout->animSet(), 1);
    }
}
/* 02645048 */
u32 hdTriforceButton_c::hasFragments() const { return u8(mFragmentFlags) != 0; }
/* 02645058 */
u32 hdTriforceButton_c::isComplete() const { return (u32(mFragmentFlags)>>8)&1; }
/* 02645064 */
u32 hdTriforceButton_c::itemId() const { return sTriforceIdentities[isComplete()].mItem; }
/* 02645094 */
u32 hdTriforceButton_c::eventId() const { return sTriforceIdentities[isComplete()].mEvent; }
/* 026450C4 */
void hdTriforceButton_c::requestCompleteDemo() { changeState(&sTriforceCompleteDemo); }
/* 026450D4 */
u32 hdTriforceButton_c::loadLayout(void* name, void* accessor, u32 resource, u32 options, void* descriptor) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sTriforceUi.mpSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, gabi::ea(accessor), resource, options, 7, 3, descriptor)) return 0;
    for (u32 i = 0; i<2; ++i) {
        auto* group = hdPartsView<hdUiSafeString>(gabi::ea(sTriforceGroupNames)+sTriforceAnimationSlots[i]*8);
        triforceBindAnimation(mpLayout->animSet(), i+5, &sTriforceAnimationNames[i], group);
    }
    return 1;
}
/* 026451EC */
void hdTriforceButton_c::updateEnabled() {
    bool enabled = (u32(mFragmentFlags)&0x100) != 0;
    if (!enabled) {
        hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
        auto* expectedTable = sTriforceCompleteDemo.mVtable.get();
        const u32 key = current->getKey();
        const u32 expected = gabi::call_ptr<u32>(expectedTable->mGetKey.get(), &sTriforceCompleteDemo);
        enabled = key == expected;
    }
    if (enabled) {
        auto* button = hdPartsView<hdButtonPart_c>(gabi::ea(this));
        button->setPaneEnabled(1); button->setTextEnabled(1);
    } else enableWhileActive();
}
static hdPartsStateId_c sTriforceLoop;
static u32 sTriforceInitializerWords[4] = {};
static u32 sTriforceRegistration[3][3] = {};
static f32 sTriforceHeaderScale;
static f32 sTriforceHeaderRate;
static f32 sTriforceScaleSource;
static f32 sTriforceRateSource;
static u8 sTriforceHeaderByte;
static u8 sTriforceHeaderBool;
static u32 sTriforceStateGuard;
static u32 sTriforceStateCounter;
void triforceRegisterStatic(void*);
void triforceConstructByte(void*);
void triforceConstructBool(void*);
/* 026452B4 */
static void initializeTriforceButtonStatics() {
    sTriforceInitializerWords[3] = 0; sTriforceInitializerWords[2] = 0;
    sTriforceInitializerWords[1] = 0; sTriforceInitializerWords[0] = 0;
    triforceRegisterStatic(sTriforceRegistration[0]);
    sTriforceHeaderScale = sTriforceScaleSource; sTriforceHeaderRate = sTriforceRateSource;
    triforceConstructByte(&sTriforceHeaderByte); triforceRegisterStatic(sTriforceRegistration[1]);
    triforceConstructBool(&sTriforceHeaderBool); triforceRegisterStatic(sTriforceRegistration[2]);
    sTriforceGroupNames[0].mVtable = &sTriforceStringVtable; sTriforceGroupNames[0].mText.v = gabi::guest_addr("G_Btn_00");
    sTriforceGroupNames[1].mVtable = &sTriforceStringVtable; sTriforceGroupNames[1].mText.v = gabi::guest_addr("G_Loop_00");
    sTriforceGroupNames[2].mVtable = &sTriforceStringVtable; sTriforceGroupNames[2].mText.v = gabi::guest_addr("G_Demo_00");
    sTriforceAnimationNames[0].mVtable = &sTriforceStringVtable; sTriforceAnimationNames[0].mText.v = gabi::guest_addr("Loop");
    sTriforceAnimationNames[1].mVtable = &sTriforceStringVtable; sTriforceAnimationNames[1].mText.v = gabi::guest_addr("TriforceDemo");
    sTriforceCompletePaneNames[0].mVtable = &sTriforceStringVtable; sTriforceCompletePaneNames[0].mText.v = gabi::guest_addr("N_Light_00");
    sTriforceFragmentPaneNames[0].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[0].mText.v = gabi::guest_addr("P_Shards_00");
    sTriforceFragmentPaneNames[1].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[1].mText.v = gabi::guest_addr("P_Shards_01");
    sTriforceFragmentPaneNames[2].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[2].mText.v = gabi::guest_addr("P_Shards_02");
    sTriforceFragmentPaneNames[3].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[3].mText.v = gabi::guest_addr("P_Shards_03");
    sTriforceFragmentPaneNames[4].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[4].mText.v = gabi::guest_addr("P_Shards_04");
    sTriforceFragmentPaneNames[5].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[5].mText.v = gabi::guest_addr("P_Shards_05");
    sTriforceFragmentPaneNames[6].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[6].mText.v = gabi::guest_addr("P_Shards_06");
    sTriforceFragmentPaneNames[7].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[7].mText.v = gabi::guest_addr("P_Shards_07");
    sTriforceFragmentPaneNames[8].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[8].mText.v = gabi::guest_addr("P_Triforce_00");
    sTriforceFragmentPaneNames[9].mVtable = &sTriforceStringVtable; sTriforceFragmentPaneNames[9].mText.v = gabi::guest_addr("P_TriforceBase_00");
    sTriforceLayoutName.mVtable = &sTriforceStringVtable; sTriforceLayoutName.mText.v = gabi::guest_addr("BtnTriforce_00");
    u32 number = 0;
    if (sTriforceStateGuard) number = sTriforceStateCounter; else sTriforceStateGuard = 1;
    auto* table = &sTriforceStateVtable;
    auto* nullState = &sTriforceNullState;
    sTriforceComplete.setup(++number, "::StateID_TriforceDemoEnd", table, 64, nullState);
    sTriforceLoop.setup(++number, "::StateID_Loop", table, 67, nullState);
    sTriforceCompleteDemo.setup(++number, "::StateID_TriforceDemo", table, 70, nullState);
    sTriforceStateCounter = number;
}
/* WWHD glue
 data sTriforceVtable 100ECC28 nocheck
 data sTriforceHolderVtable 100ECE70 nocheck
 data sTriforceSecondaryVtable 100ECE80 nocheck
 data sTriforceStringVtable 100ECA24 nocheck
 data sTriforceStateVtable 100ECA94 nocheck
 data sTriforceNullState 101FF32C nocheck
 data sTriforceLoop 10490DE0 nocheck
 data sTriforceInitializerWords 10490D28 nocheck
 data sTriforceRegistration 101F57B0 nocheck
 data sTriforceHeaderScale 10490D1C nocheck
 data sTriforceHeaderRate 10490D20 nocheck
 data sTriforceScaleSource 100ECAFC nocheck
 data sTriforceRateSource 100ECB00 nocheck
 data sTriforceHeaderByte 10490D24 nocheck
 data sTriforceHeaderBool 10490D25 nocheck
 data sTriforceStateGuard 101FD8E4 nocheck
 data sTriforceStateCounter 101FDD50 nocheck
 ext 028F026C void triforceRegisterStatic(void*)
 ext 028ED6F8 void triforceConstructByte(void*)
 ext 028EAB2C void triforceConstructBool(void*)
 data sTriforceCompleteDemo 10490E10 nocheck
 data sTriforceComplete 10490DB0 nocheck
 data sTriforceLayoutName 10490E40 nocheck
 data sTriforceCompletePaneNames 10490D14 nocheck
 data sTriforceFragmentPaneNames 10490D60 nocheck
 data sTriforceAnimationNames 10490D50 nocheck
 data sTriforceGroupNames 10490D38 nocheck
 data sTriforceAnimationSlots 100ECA18 nocheck
 data sTriforceIdentities 100ECA3C nocheck
 data sTriforceFrame 100ECAE0 nocheck
 data sTriforceUi 1018C404 nocheck
 data sTriforceHeap 101F8B4C nocheck
 data sTriforceSave 101F84DC nocheck
 str 100ECAE4 "SE_TRIFORCE_COMPLETE"
 ext 020053E4 void triforceSetFrame(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 triforceAnimationEnded(hdAnimSet*,u32)
 ext 02005708 void triforceStartAnimation(hdAnimSet*,u32,u32)
 ext 02005334 void triforceStopAnimation(hdAnimSet*,u32)
 ext 02030B38 void triforcePlaySound(const char*)
 ext 025E1918 void triforceNotify(u32)
 ext 025B7C50 u32 triforceIsObtained(void*,u32)
 ext 02634088 u32 triforceFindPanes(hdPartsLayout_c*,hdScreenPaneArray*,hdUiSafeString*)
 ext 0263420C u32 triforceFindPicturePanes(hdPartsLayout_c*,hdScreenPaneArray*,hdUiSafeString*)
 ext 02004E04 void triforceBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 026F5CCC void hdButtonPart_c::setPaneEnabled(u32)
 ext 026F5CD4 void hdButtonPart_c::setTextEnabled(u32)
 ext 02701CD8 void NoDecideButton_l::enableWhileActive()
 str 100ECBC4 "G_Btn_00" in 026452B4
 str 100ECB6C "G_Loop_00"
 str 100ECB78 "G_Demo_00"
 str 100ECB04 "Loop"
 str 100ECBD0 "TriforceDemo"
 str 100ECBE0 "N_Light_00"
 str 100ECBEC "BtnTriforce_00" in 026452B4
 str 100ECBA8 "::StateID_TriforceDemoEnd"
 str 100ECBFC "::StateID_Loop"
 str 100ECC0C "::StateID_TriforceDemo"
 str 100ECB0C "P_Shards_00"
 str 100ECB18 "P_Shards_01"
 str 100ECB24 "P_Shards_02"
 str 100ECB30 "P_Shards_03"
 str 100ECB3C "P_Shards_04"
 str 100ECB48 "P_Shards_05"
 str 100ECB54 "P_Shards_06"
 str 100ECB60 "P_Shards_07"
 str 100ECB84 "P_Triforce_00"
 str 100ECB94 "P_TriforceBase_00"
*/
