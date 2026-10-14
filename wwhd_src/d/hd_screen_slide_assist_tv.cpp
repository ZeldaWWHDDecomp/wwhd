/** HD television slide assistance. */
#include "d/hd_screen_slide_assist_tv.h"
namespace gc_tu {
static u32 sSlideStringVtable;
static hdPartsStateIdVtbl_c sSlideStateVtable;
static u32 sSlideVtable;
static u32 sSlideHolderVtable;
static u32 sSlideSecondaryVtable;
}
using namespace gc_tu;
struct hdSlideContextPointer_l { gptr<hdSlideMenuContext_l> mpContext; };
static hdSlideContextPointer_l sSlideContext;
static hdPartsStateId_c sSlideBlank;
static hdPartsStateId_c sSlideDisplay;
static hdPartsStateId_c sSlideInAll;
static hdPartsStateId_c sSlideOutAll;
struct hdSlideArchiveNames_l { hdUiSafeString mNames[3]; };
static hdSlideArchiveNames_l sSlideArchiveNames;
static hdUiSafeString sSlideName;
static f32 sSlideAnimationStart = 1.0f;

struct hdSlideHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdSlideHeapManager_l, mpHeap, 0x10);
struct hdSlideHeapPointer_l { gptr<hdSlideHeapManager_l> mpManager; };
static hdSlideHeapPointer_l sSlideHeapManager;
struct hdSlidePartPointer_l { gptr<hdParts_c> mpPart; };
static hdSlidePartPointer_l sSlideBindingSource;
struct hdSlideNames_l { hdUiSafeString mNames[2]; };
static hdSlideNames_l sSlideChildNames;
static hdSlideNames_l sSlideAnimationNames;
static hdUiSafeString sSlidePaneName;
static hdUiSafeString sSlideIconClass;
static hdUiSafeString sSlideButtonClass;
struct hdSlideIndices_l { be<u32> mIndices[2]; };
static hdSlideIndices_l sSlidePaneIndices;
struct hdSlideBindingLocals_l { hdSlideAnimationBinding_l mAnimation; hdItemIconContext_c mBindings; };
struct hdSlideResourceDescriptor_l { u8 _00[8]; hdUiSafeString mClass; u8 _10[4]; hdUiSafeString mName; };
WWHD_OFFSET(hdSlideResourceDescriptor_l, mClass, 8);
WWHD_OFFSET(hdSlideResourceDescriptor_l, mName, 0x14);
static bool slideNamesMatch(hdUiSafeString* input, hdUiSafeString* reference) {
    input->assureTermination();
    input->assureTermination();
    be<u8>* cached = input->mText;
    reference->assureTermination();
    be<u8>* other = reference->mText;
    if (cached == other) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 left = input->mText.get()[i];
        u8 right = reference->mText.get()[i];
        if (left != right) return false;
        if (!left) return true;
    }
    return false;
}

struct hdSlideHeaderAngles_l { be<f32> mMin, mMax; };
struct hdSlideHeaderRectangle_l { be<f32> mLeft, mTop, mRight, mBottom; };
struct hdSlideHeaderFlags_l { be<u8> mFirst, mSecond; };
struct hdSlideHeaderZeros_l { be<u32> mWords[4]; };
struct hdSlideRegistration_l { be<u32> mWords[3]; };
struct hdSlideRegistrations_l { hdSlideRegistration_l mEntries[3]; };
static hdSlideHeaderAngles_l sSlideHeaderAngles;
static hdSlideHeaderRectangle_l sSlideHeaderRectangle;
static hdSlideHeaderFlags_l sSlideHeaderFlags;
static hdSlideHeaderZeros_l sSlideHeaderZeros;
static hdSlideRegistrations_l sSlideRegistrations;
static f32 sSlideNegativePi = -3.1415927410125732f;
static f32 sSlidePositivePi = 3.1415927410125732f;
static f32 sSlideRectangleLimit = 50000.0f;
static f32 sSlideRectangleSize = 10000.0f;
static u32 sSlideStateCounterReady;
static u32 sSlideStateCounter;
static hdPartsStateId_c sSlideParentBlank;
static hdPartsStateId_c sSlideParentDisplay;
static hdStateId sSlideNullState;
void slideRegister(hdSlideRegistration_l* registration);
void slideConstructHeaderFirst(be<u8>* flag);
void slideConstructHeaderSecond(be<u8>* flag);

static hdPartsStateId_c sSlideOutWait;
static hdSlideNames_l sSlideChildClasses;

/* 026DE490 */
static u32 slideStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026DE590 */
static void destroySlide(hdSlideAssistTV_c* panel, u32 flags) {
    if (!panel) return;
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sSlideHolderVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sSlideSecondaryVtable);
    slideDestroyBase(panel, 0);
    if (flags & 1) operator_delete(panel);
}
/* 026DE5FC */
void hdSlideAssistTV_c::waitExecute() {
    hdMenuMain_c* menu = static_cast<hdMenuMain_c*>(sSlideContext.mpContext->mpItemScreen.get());
    if (menu->isCollectActionBlocked() || menu->isCollectInputBlocked()) changeState(&sSlideInAll);
}
/* 026DE668 */
void hdSlideAssistTV_c::displayExecute() {
    hdMenuMain_c* menu = static_cast<hdMenuMain_c*>(sSlideContext.mpContext->mpItemScreen.get());
    if (!menu->isCollectActionBlocked() && !menu->isCollectInputBlocked()) changeState(&sSlideOutAll);
    vExecuteChildren();
    vCalcLayout();
}
/* 026DE6FC */
void hdSlideAssistTV_c::delayEnter() { mDelay = 0; }
/* 026DE708 */
void hdSlideAssistTV_c::delayExecute() {
    u32 delay = mDelay;
    if (!delay) { changeState(&sSlideOutAll); return; }
    hdPartsVtbl_c* table = mVtable;
    mDelay = delay - 1;
    gabi::call_ptr(table->mExecuteChildren.get(), this);
    vCalcLayout();
}
/* 026DE790 */
void hdSlideAssistTV_c::inAllEnter() { hdAnimSet_play(animSet(), 0, 0, sSlideAnimationStart); }
/* 026DE7AC */
void hdSlideAssistTV_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sSlideDisplay);
    vExecuteChildren();
    vCalcLayout();
}
/* 026DE824 */
void hdSlideAssistTV_c::outAllEnter() { hdAnimSet_play(animSet(), 1, 0, sSlideAnimationStart); }
/* 026DE840 */
void hdSlideAssistTV_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 1)) changeState(&sSlideBlank);
    vExecuteChildren();
    vCalcLayout();
}
/* 026DE8B8 */
static hdSlideAssistTV_c* constructSlide(hdSlideAssistTV_c* panel) {
    if (!panel) { panel = (hdSlideAssistTV_c*)operator_new(sizeof(hdSlideAssistTV_c)); if (!panel) return nullptr; }
    slideConstructBase(panel);
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sSlideHolderVtable);
    panel->mVtable.v = gabi::ea(&sSlideVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sSlideSecondaryVtable);
    panel->mDelay = 0;
    return panel;
}
/* 026DEA90 */
u32 hdSlideAssistTV_c::setup(u32 archive, u32 resource, u32 flags) {
    slideAttachArchives(this, sSlideArchiveNames.mNames, 3);
    if (!vLoad(&sSlideName, mpResAccessor.v.get(), archive, resource, gabi::at<void>(flags))) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    slideConfigureChildren(this);
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sSlideBlank);
    return 1;
}
/* 026DEBC4 */
void hdSlideAssistTV_c::release() { vDeleteChildren(); vDestroyLayout(); slideDetachArchives(this); }
/* 026DEC18 */
void hdSlideAssistTV_c::stateExecute() { slideExecuteState(stateMachine()); }
/* 026DEC20 */
void hdSlideAssistTV_c::draw(void* context) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateIdVtbl_c* currentTable = current->mVtable;
    hdPartsStateIdVtbl_c* blankTable = sSlideBlank.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentTable->mGetKey.get(), current);
    u32 blankKey = gabi::call_ptr<u32>(blankTable->mGetKey.get(), &sSlideBlank);
    if (currentKey != blankKey) slideDrawBase(this, context);
}
/* 026DE92C */
void hdSlideAssistTV_c::configureChildren() {
    hdParts_c* item = sSlideContext.mpContext->mpItemScreen;
    hdParts_c* source = *item->childSlot(0x2D);
    hdParts_c* destination = *childSlot(0);
    hdSlideBindingLocals_l locals;
    ((hdItemIcon_c*)sSlideBindingSource.mpPart.get())->getContext(&locals.mBindings);
    ((hdTouchSlideDecide_c*)childSlot(1)->get())->setSource(&locals.mBindings);
    ((hdItemDecide_c*)source->childSlot(0)->get())->getContext(reinterpret_cast<hdItemDecideContext_c*>(&locals.mBindings));
    ((hdTouchSlideDecide_c*)destination->childSlot(0)->get())->setSource(&locals.mBindings);
    ((hdItemDecide_c*)source->childSlot(1)->get())->getContext(reinterpret_cast<hdItemDecideContext_c*>(&locals.mBindings));
    ((hdTouchSlideDecide_c*)destination->childSlot(1)->get())->setSource(&locals.mBindings);
    ((hdItemDecide_c*)source->childSlot(2)->get())->getContext(reinterpret_cast<hdItemDecideContext_c*>(&locals.mBindings));
    ((hdTouchSlideDecide_c*)destination->childSlot(2)->get())->setSource(&locals.mBindings);
    slideGetAnimation(source, &locals.mAnimation);
    slideInstallAnimation(destination, &locals.mAnimation);
}
/* 026DECB8 */
u32 hdSlideAssistTV_c::createChild(hdPartsDesc_c* descriptor) {
    void* heap = sSlideHeapManager.mpManager->mpHeap;
    hdSlideResourceDescriptor_l* request = (hdSlideResourceDescriptor_l*)descriptor;
    for (u32 i = 0; i < 2; ++i) {
        if (!slideNamesMatch(&request->mName, &sSlideChildNames.mNames[i])) continue;
        if (slideNamesMatch(&request->mClass, &sSlideIconClass)) {
            gptr<hdParts_c>* slot = childSlot(i);
            hdParts_c* part = (hdParts_c*)slideAllocate(0x54, heap, 4);
            if (part) part = slideConstructIcon(part);
            *slot = part;
            if (!*childSlot(i)) return 0;
            return slideSetupIcon(*childSlot(i), 0, 0, descriptor);
        }
        if (slideNamesMatch(&request->mClass, &sSlideButtonClass)) {
            gptr<hdParts_c>* slot = childSlot(i);
            hdTouchSlideDecide_c* part = (hdTouchSlideDecide_c*)slideAllocate(sizeof(hdTouchSlideDecide_c), heap, 4);
            if (part) part = hdTouchSlideDecide_c::construct(part);
            *slot = (hdParts_c*)part;
            if (!*childSlot(i)) return 0;
            return ((hdTouchSlideDecide_c*)childSlot(i)->get())->load(nullptr, 0, gabi::ea(descriptor));
        }
        return slideCreateBase(this, i, descriptor);
    }
    return 1;
}
/* 026DF038 */
u32 hdSlideAssistTV_c::loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)slideAllocate(sizeof(hdPartsLayout_c), sSlideHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(name, archive, resource, flags, 2, 1, descriptor)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        u32 paneIndex = sSlidePaneIndices.mIndices[i];
        hdUiSafeString* pane = gabi::at<hdUiSafeString>(gabi::ea(&sSlidePaneName) + paneIndex * sizeof(hdUiSafeString));
        slideBindAnimation(animSet(), i, &sSlideAnimationNames.mNames[i], pane);
    }
    vCalcLayout();
    return 1;
}
/* 026DF164 */
static void initializeSlideStatics() {
    for (u32 i = 0; i < 4; ++i) sSlideHeaderZeros.mWords[i] = 0;
    slideRegister(&sSlideRegistrations.mEntries[0]);
    sSlideHeaderAngles.mMin = sSlideNegativePi;
    sSlideHeaderAngles.mMax = sSlidePositivePi;
    slideConstructHeaderFirst(&sSlideHeaderFlags.mFirst);
    slideRegister(&sSlideRegistrations.mEntries[1]);
    slideConstructHeaderSecond(&sSlideHeaderFlags.mSecond);
    slideRegister(&sSlideRegistrations.mEntries[2]);
    f32 limit = sSlideRectangleLimit;
    f32 size = sSlideRectangleSize;
    sSlideHeaderRectangle.mBottom = size;
    sSlideHeaderRectangle.mLeft = limit;
    sSlideHeaderRectangle.mRight = size;
    sSlideHeaderRectangle.mTop = limit;
    sSlidePaneName.mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlidePaneName.mText.v = gabi::guest_addr("G_All_00");
    sSlideAnimationNames.mNames[0].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sSlideAnimationNames.mNames[1].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sSlideChildNames.mNames[0].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideChildNames.mNames[0].mText.v = gabi::guest_addr("L_IconSlideArea_00");
    sSlideChildNames.mNames[1].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideChildNames.mNames[1].mText.v = gabi::guest_addr("L_BtnItemIcon_00");
    sSlideChildClasses.mNames[0].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideChildClasses.mNames[0].mText.v = gabi::guest_addr("IconSlideArea_00");
    sSlideChildClasses.mNames[1].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideChildClasses.mNames[1].mText.v = gabi::guest_addr("BtnItemIcon_00");
    sSlideArchiveNames.mNames[0].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideArchiveNames.mNames[0].mText.v = gabi::guest_addr("SlideAssistTV_00");
    sSlideArchiveNames.mNames[1].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideArchiveNames.mNames[1].mText.v = gabi::guest_addr("IconSlideArea_00");
    sSlideArchiveNames.mNames[2].mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideArchiveNames.mNames[2].mText.v = gabi::guest_addr("BtnItemIcon_00");
    sSlideName.mVtable.v = gabi::ea(&sSlideStringVtable);
    sSlideName.mText.v = gabi::guest_addr("SlideAssistTV_00");
    u32 ready = sSlideStateCounterReady;
    u32 number;
    if (ready) number = sSlideStateCounter;
    else { number = 0; sSlideStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sSlideStateVtable;
    sSlideBlank.setup(number + 1, "::StateID_Blank", table, 4, (hdStateId*)&sSlideParentBlank);
    sSlideDisplay.setup(number + 2, "::StateID_Display", table, 7, (hdStateId*)&sSlideParentDisplay);
    sSlideOutWait.setup(number + 3, "::StateID_OutWait", table, 0x19, &sSlideNullState);
    sSlideInAll.setup(number + 4, "::StateID_InAll", table, 0x1C, &sSlideNullState);
    sSlideOutAll.setup(number + 5, "::StateID_OutAll", table, 0x1F, &sSlideNullState);
    sSlideStateCounter = number + 5;
}
/* WWHD glue
 data sSlideStringVtable 10104D04 nocheck
 data sSlideStateVtable 10104D64 nocheck
 data sSlideVtable 10104EA0 nocheck
 data sSlideHolderVtable 10104FB0 nocheck
 data sSlideSecondaryVtable 10104FC0 nocheck
data sSlideHeaderAngles 1049B8D4
data sSlideHeaderRectangle 1049B8DC
data sSlideHeaderFlags 1049B8EC
data sSlideHeaderZeros 1049B8F0
data sSlideRegistrations 101F6B50
data sSlideNegativePi 10104DB4
data sSlidePositivePi 10104DB8
data sSlideRectangleLimit 10104DBC
data sSlideRectangleSize 10104DC0
data sSlideStateCounterReady 101FD8E4
data sSlideStateCounter 101FDD50
data sSlideParentBlank 1049DD60
data sSlideParentDisplay 1049DD90
data sSlideNullState 101FF32C
data sSlideOutWait 1049B9A8
data sSlideChildClasses 1049B920
ext 028F026C void slideRegister(hdSlideRegistration_l* registration)
ext 028ED6F8 void slideConstructHeaderFirst(be<u8>* flag)
ext 028EAB2C void slideConstructHeaderSecond(be<u8>* flag)
str 10104E1C "G_All_00" in 026DF164
str 10104DC4 "InAll" in 026DF164
str 10104DCC "OutAll" in 026DF164
str 10104E28 "L_IconSlideArea_00" in 026DF164
str 10104E3C "L_BtnItemIcon_00" in 026DF164
str 10104E50 "IconSlideArea_00" in 026DF164
str 10104E64 "BtnItemIcon_00" in 026DF164
str 10104E74 "SlideAssistTV_00" in 026DF164
str 10104DD4 "::StateID_Blank" in 026DF164
str 10104DF4 "::StateID_Display" in 026DF164
str 10104E08 "::StateID_OutWait" in 026DF164
str 10104DE4 "::StateID_InAll" in 026DF164
str 10104E88 "::StateID_OutAll" in 026DF164
data sSlideHeapManager 1018C404
data sSlideBindingSource 101F5578
data sSlideChildNames 1049B910
data sSlideIconClass 10494CEC
data sSlideButtonClass 10490140
data sSlidePaneName 1049B8CC
data sSlideAnimationNames 1049B900
data sSlidePaneIndices 10104CE8
ext 02636C28 void hdItemIcon_c::getContext(hdItemIconContext_c* context)
ext 026355DC void hdTouchSlideDecide_c::setSource(hdItemIconContext_c* source)
ext 0263617C void hdItemDecide_c::getContext(hdItemDecideContext_c* context)
ext 02670570 void slideGetAnimation(hdParts_c* part, hdSlideAnimationBinding_l* binding)
ext 026713F8 void slideInstallAnimation(hdParts_c* part, hdSlideAnimationBinding_l* binding)
ext 0273B050 void* slideAllocate(u32 size, void* heap, u32 alignment)
ext 026711FC hdParts_c* slideConstructIcon(hdParts_c* part)
ext 0263489C hdTouchSlideDecide_c* hdTouchSlideDecide_c::construct(hdTouchSlideDecide_c* part)
ext 02671270 u32 slideSetupIcon(hdParts_c* part, u32 archive, u32 resource, hdPartsDesc_c* descriptor)
ext 02634B98 u32 hdTouchSlideDecide_c::load(void* resource, u32 arg1, u32 arg2)
ext 026F92C0 u32 slideCreateBase(hdSlideAssistTV_c* panel, u32 index, hdPartsDesc_c* descriptor)
ext 02004E04 void slideBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
data sSlideContext 101F8344
data sSlideBlank 1049B948
data sSlideDisplay 1049B978
data sSlideInAll 1049B9D8
data sSlideOutAll 1049BA08
data sSlideArchiveNames 1049B930
data sSlideName 1049BA38
data sSlideAnimationStart 10104DB0
ext 026F88F0 void slideDestroyBase(hdSlideAssistTV_c* panel, u32 flags)
ext 026F89E8 hdSlideAssistTV_c* slideConstructBase(hdSlideAssistTV_c* panel)
ext 02693E50 u32 hdMenuMain_c::isCollectActionBlocked()
ext 0268F2FC u32 hdMenuMain_c::isCollectInputBlocked()
ext 026F90D8 void slideAttachArchives(hdSlideAssistTV_c* panel, hdUiSafeString* names, u32 count)
ext 026F91F8 void slideDetachArchives(hdSlideAssistTV_c* panel)
ext 02006364 void slideExecuteState(hdStateMachine* machine)
ext 026DE92C void slideConfigureChildren(hdSlideAssistTV_c* panel)
ext 026F8B54 void slideDrawBase(hdSlideAssistTV_c* panel, void* context)
*/
