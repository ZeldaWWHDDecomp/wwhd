/**
 * hd_parts.cpp
 * HD - base class of the UI parts: layout screen, Blank / Display states, the child parts a layout
 * lists (the parts factory), resource archives
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_fw_parts_loop_display). Each function is marked with its WWHD address; the glue block at the
 * end lists this translation unit's data (tools/verify/autoglue.py). State and parts class names are
 * HD strings; all class, member and function names are ours.
 */

#include "d/hd_parts.h"
#include "d/hd_parts_in_out_loop.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_tu_header_statics.h"

static hdPartsVtbl_c sPartsVtable;
static u32 sPartsHolderVtable;
static u32 sPartsSecondaryVtable;
static u32 sOwnerVtable;
static u32 sStateVtable;
static u32 sHolderVtable;
static hdUiStringTable sSafeStringVtable;
static hdPartsStateIdVtbl_c sStateIdVtable;
static hdStateId sNoStateId;
static hdScreenUiSystem* sUiSystem;
static void* sHeapMgr;
static void* sLayoutResMgr;
static void* sFontMgr;

static hdPartsStateId_c sBlank;
static hdPartsStateId_c sDisplay;
static u32 sStateNumberReady;
static s32 sStateNumber;

/* the parts class names (statics of the parts classes' TUs) */
static hdUiSafeString sBasicButtonName;
static hdUiSafeString sHeartName;
static hdUiSafeString sInOutLoopName;
static hdUiSafeString sKeyIconName;
static hdUiSafeString sLoopName;
static hdUiSafeString sNoControlName;
static hdUiSafeString sNoDecideButtonName;
static hdUiSafeString sOneTimeName;
static hdUiSafeString sProgressBarName;
static hdUiSafeString sValidName;

/* 026F88F0 */
// hdParts_c::~hdParts_c() (GHS deleting destructor)
void hdParts_c::destroy(hdParts_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sPartsHolderVtable;
    i_this->mSecondaryVtable = &sPartsSecondaryVtable;
    // HD: the state machine's destructor (inlined) lets its owner drop the previous and current state
    hdPartsStateHolder_c* holder = &i_this->mStateHolder;
    if (gabi::ea(holder) != 0) {
        holder->mMachine.mpOwner->onStateChange(&holder->mMachine.mpPreviousState);
        holder->mMachine.mpOwner->onStateChange(&holder->mMachine.mpCurrentState);
    }
    hdParts_destroyVisitor(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026F899C */
// ::StateID_Display execute: the children run, then the layout is calculated
void hdParts_c::displayExecute() {
    vExecuteChildren();
    vCalcLayout();
}

/* 026F89E8 */
// hdParts_c::hdParts_c() (GHS: allocates when called with NULL)
hdParts_c* hdParts_c::construct(hdParts_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdParts_c*)operator_new(sizeof(hdParts_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_constructVisitor(i_this);
    // HD: GHS tests the address of each embedded member before constructing it
    hdPartsStateHolder_c* holder = &i_this->mStateHolder;
    if (gabi::ea(holder) == 0) {
        holder = (hdPartsStateHolder_c*)operator_new(sizeof(hdPartsStateHolder_c));
    }
    if (holder != NULL) {
        holder->mState.mpParts = i_this;
        holder->mState.mpStateId = NULL;
        holder->mState.mVtable = &sStateVtable;
        holder->mOwnerVtable = &sOwnerVtable;
        holder->mHolderVtable = &sHolderVtable;
        hdParts_createStateMachine(&holder->mMachine, holder, &sNoStateId);
    }
    i_this->mpResAccessor = NULL;
    i_this->mVtable = &sPartsVtable;
    i_this->mStateHolder.mHolderVtable = &sPartsHolderVtable;
    i_this->mSecondaryVtable = &sPartsSecondaryVtable;
    hdPartsArcLinkArray_c* links = &i_this->mArcLinks;
    if (gabi::ea(links) == 0) {
        links = (hdPartsArcLinkArray_c*)operator_new(sizeof(hdPartsArcLinkArray_c));
    }
    if (links != NULL) {
        links->mMax = 0;
        links->mNum = 0;
        links->mpLinks = NULL;
    }
    i_this->mpLayout = NULL;
    hdPartsChildArray_c* children = &i_this->mChildren;
    if (gabi::ea(children) == 0) {
        children = (hdPartsChildArray_c*)operator_new(sizeof(hdPartsChildArray_c));
    }
    if (children != NULL) {
        children->mpData = NULL;
        children->mNum = 0;
    }
    return i_this;
}

/* 026F8B00 */
void hdParts_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
}

/* 026F8B4C */
void hdParts_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026F8B54 */
void hdParts_c::draw(void* info) {
    vDrawLayout(info);
    vDrawChildren(info);
}

/* 026F8BB0 */
u32 hdParts_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL) {
        return FALSE;
    }
    layout->vLoad(name, arg1, arg2, arg3, 0, 0, desc);
    return TRUE;
}

/* 026F8C64 */
void hdParts_c::destroyLayout() {
    if (mpLayout == NULL) {
        return;
    }
    mpLayout->release();
    if (mpLayout != NULL) {
        mpLayout->destroy(3);
    }
    mpLayout = NULL;
}

/* 026F8CD0 */
void hdParts_c::calcLayout() {
    hdPartsLayout_c* layout = mpLayout;
    if (layout != NULL) {
        layout->calc();
    }
}

/* 026F8CEC */
void hdParts_c::updateLayout() {
    hdPartsLayout_c* layout = mpLayout;
    if (layout != NULL) {
        layout->update();
    }
}

/* 026F8D08 */
void hdParts_c::drawLayout(void* info) {
    hdPartsLayout_c* layout = mpLayout;
    if (layout != NULL) {
        layout->draw(info);
    }
}

/* 026F8D24 */
// one child slot per parts descriptor of the layout; the visitor (visit, slot +0x1C) builds them
u32 hdParts_c::createChildren() {
    hdPartsDescList_c* descs = &mpLayout->lyt()->mPartsDescs;
    if (gabi::ea(descs) == 0) {
        return TRUE;
    }
    s32 count = descs->mCount;
    if (count <= 0) {
        return TRUE;
    }
    hdScreenHeap* heap = sUiSystem->mHeap;
    if (heap == NULL) {
        heap = hdParts_getCurrentHeap(sHeapMgr);
    }
    gptr<hdParts_c>* data = (gptr<hdParts_c>*)heap->allocate(count * 4);
    if (data != NULL) {
        mChildren.mpData = data;
        mChildren.mNum = count;
    }
    s32 num = mChildren.mNum;
    for (s32 i = 0; i < num; i++) {
        *mChildren.slot(i) = NULL;
    }
    return hdParts_visitAll(this, descs);
}

/* 026F8E24 */
void hdParts_c::deleteChildren() {
    for (s32 i = 0; i < mChildren.mNum; i++) {
        if (child(i) == NULL) {
            continue;
        }
        child(i)->vFinalize();
        if (child(i) != NULL) {
            child(i)->vDelete(3);
        }
        *childSlot(i) = NULL;
    }
    if (mChildren.mpData != NULL) {
        hdScreenHeap* heap = hdParts_findContainHeap(sHeapMgr, mChildren.mpData);
        heap->free(mChildren.mpData);
        mChildren.mNum = 0;
        mChildren.mpData = NULL;
    }
}

/* 026F8F78 */
void hdParts_c::executeChildren() {
    for (s32 i = 0; i < mChildren.mNum; i++) {
        if (child(i) != NULL) {
            child(i)->vExecuteState();
        }
    }
}

/* 026F9020 */
void hdParts_c::drawChildren(void* info) {
    for (s32 i = 0; i < mChildren.mNum; i++) {
        if (child(i) != NULL) {
            child(i)->vDraw(info);
        }
    }
}

/* 026F90D8 */
// the named archives of the layout resource manager, attached to a new multi-archive accessor that the
// font manager uses as well
void hdParts_c::attachArchives(hdUiSafeString* names, u32 count) {
    hdScreenHeap* heap = sUiSystem->mHeap;
    hdPartsResAccessor_c* accessor = (hdPartsResAccessor_c*)hdParts_new(sizeof(hdPartsResAccessor_c), heap, 4);
    if (accessor != NULL) {
        accessor = hdPartsResAccessor_c::construct(accessor);
    }
    mpResAccessor = accessor;
    hdParts_allocArcLinks(&mArcLinks, count, heap, 4);
    for (; count != 0; count--, names++) {
        void* archive = hdParts_findArchive(sLayoutResMgr, names);
        if (archive == NULL) {
            continue;
        }
        hdPartsArcLink_c* link = (hdPartsArcLink_c*)hdParts_new(sizeof(hdPartsArcLink_c), heap, 4);
        if (link != NULL) {
            link->mLink[0] = 0;
            link->mLink[1] = 0;
            hdParts_constructArcAccessor(link->mAccessor);
        }
        hdParts_setArcLink(link, archive, ".");
        hdParts_attachArcLink(mpResAccessor, link);
        s32 num = mArcLinks.mNum;
        if (num < mArcLinks.mMax) {
            *mArcLinks.slot(num) = link;
            mArcLinks.mNum = mArcLinks.mNum + 1;
        }
    }
    hdParts_setFontResAccessor(sFontMgr, mpResAccessor);
}

/* 026F91F8 */
void hdParts_c::detachArchives() {
    hdParts_detachAllArcLinks(mpResAccessor);
    for (s32 i = 0; i < mArcLinks.mNum; i++) {
        hdPartsArcLink_c* link = mArcLinks.at(i);
        if (link == NULL) {
            continue;
        }
        hdParts_destroyArcAccessor(link->mAccessor, 2);
        operator_delete(link);
    }
    hdParts_freeArcLinks(&mArcLinks);
    if (mpResAccessor != NULL) {
        mpResAccessor->destroy(3);
    }
}

/* inlined into createChild: sead SafeString equality (both strings terminated first) */
static bool hdParts_isSameName(hdUiSafeString* name, hdUiSafeString* className) {
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    className->assureTermination();
    if (text == gabi::ea(className->mText.get())) {
        return true;
    }
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(className->mText.get());
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c = *hdPartsView<be<u8>>(left + n);
        if (c != *hdPartsView<be<u8>>(right + n)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* inlined into createChild: the part of one class in slot `index`, initialised from the descriptor */
template <class Construct, class Init>
static u32 hdParts_newChild(hdParts_c* i_this, u32 index, hdPartsDesc_c* desc, hdScreenHeap* heap, u32 size, Construct construct, Init init) {
    gptr<hdParts_c>* slot = i_this->childSlot(index);
    hdParts_c* parts = (hdParts_c*)hdParts_new(size, heap, 4);
    if (parts != NULL) {
        parts = construct(parts);
    }
    *slot = parts;
    if (i_this->child(index) == NULL) {
        return FALSE;
    }
    return init(i_this->child(index), desc);
}

/* 026F92C0 */
// the parts factory: the descriptor's class name selects the parts class built in slot `index`;
// an unknown name builds nothing
u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sUiSystem->mHeap;
    gabi::Local<hdUiSafeString> className;
    className->mText.v = desc->mClassName.v;
    className->mVtable = &sSafeStringVtable;
    // GHS: the new string's type is known here, so its first termination call is direct
    hdParts_assureTermination(className);
    if (hdParts_isSameName(className, &sBasicButtonName)) {
        return hdParts_newChild(this, index, desc, heap, 0x8C, hdPartsBasicButton_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsBasicButton_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sHeartName)) {
        return hdParts_newChild(this, index, desc, heap, 0x60, hdPartsHeart_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsHeart_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sInOutLoopName)) {
        return hdParts_newChild(this, index, desc, heap, sizeof(hdPartsInOutLoop_c),
                                [](hdParts_c* p) { return (hdParts_c*)hdPartsInOutLoop_c::construct((hdPartsInOutLoop_c*)p); },
                                [](hdParts_c* p, hdPartsDesc_c* d) { return ((hdPartsInOutLoop_c*)p)->init(d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sKeyIconName)) {
        return hdParts_newChild(this, index, desc, heap, sizeof(hdPartsKeyIcon_c),
                                [](hdParts_c* p) { return (hdParts_c*)hdPartsKeyIcon_c::construct((hdPartsKeyIcon_c*)p); },
                                [](hdParts_c* p, hdPartsDesc_c* d) { return ((hdPartsKeyIcon_c*)p)->init(d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sLoopName)) {
        return hdParts_newChild(this, index, desc, heap, 0x54, hdPartsLoop_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsLoop_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sNoControlName)) {
        return hdParts_newChild(this, index, desc, heap, 0x50, hdPartsNoControl_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsNoControl_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sNoDecideButtonName)) {
        return hdParts_newChild(this, index, desc, heap, 0x78, hdPartsNoDecideButton_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsNoDecideButton_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sOneTimeName)) {
        return hdParts_newChild(this, index, desc, heap, 0x54, hdPartsOneTime_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsOneTime_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sProgressBarName)) {
        return hdParts_newChild(this, index, desc, heap, 0x58, hdPartsProgressBar_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsProgressBar_init(p, d->mResource, 0, 0, d); });
    }
    className->assureTermination();
    if (hdParts_isSameName(className, &sValidName)) {
        return hdParts_newChild(this, index, desc, heap, 0x54, hdPartsValid_construct,
                                [](hdParts_c* p, hdPartsDesc_c* d) { return hdPartsValid_init(p, d->mResource, 0, 0, d); });
    }
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

static hdTuHeaderStatics_c sPartsHeaderStatics;
static hdTuHeaderDescriptors_c sPartsHeaderDescriptors;
static hdTuHeaderZero_c sPartsHeaderZero;
static hdTuPiPair_c sPartsPiConstants;
/* 026F9EFC */
static void __sinit_hd_parts_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sPartsHeaderStatics), gabi::ea(&sPartsHeaderDescriptors), gabi::ea(&sPartsHeaderZero),
                                gabi::ea(&sPartsPiConstants));
    s32 number;
    if (sStateNumberReady) {
        number = sStateNumber;
    } else {
        number = 0;
        sStateNumberReady = 1;
    }
    sBlank.setup(number + 1, "::StateID_Blank", &sStateIdVtable, 4, &sNoStateId);
    sDisplay.setup(number + 2, "::StateID_Display", &sStateIdVtable, 7, &sNoStateId);
    sStateNumber = number + 2;
}

/* 026FA16C */
// sead SafeString::assureTerminationImpl_ (this TU's copy; a literal is always terminated)
void hdParts_assureTermination(hdUiSafeString* string) {}

/* 026FA184 */
static s32 hdParts_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* 026FA284 */
// the state machine asks its owner for the state object of a state id: one object, re-targeted
hdStateObject* hdPartsStateHolder_c::createState(hdPartsStateId_c* stateId) {
    mState.mpStateId = stateId;
    return (hdStateObject*)&mState;
}

/* 026FA290 */
void hdPartsStateHolder_c::onStateChange(gptr<hdStateObject>* state) {
    *state = NULL;
}

/* 026FA29C */
hdPartsStateId_c* hdPartsState_c::getStateId() {
    return mpStateId;
}

/* 026FA2A4 */
void hdPartsState_c::enter() {
    mpStateId->vEnter(mpParts);
}

/* 026FA2C0 */
void hdPartsState_c::execute() {
    mpStateId->vExecute(mpParts);
}

/* 026FA2DC */
void hdPartsState_c::exit() {
    mpStateId->vExit(mpParts);
    mpStateId = NULL;
}

/* WWHD glue
data sPartsHeaderStatics 1049DD44
data sPartsHeaderDescriptors 101F71E4
data sPartsHeaderZero 1049DD50
data sPartsPiConstants 1010A540
data sPartsVtable 1010A570 nocheck
data sPartsHolderVtable 1010A638 nocheck
data sPartsSecondaryVtable 1010A648 nocheck
data sOwnerVtable 1010A484 nocheck
data sStateVtable 1010A444 nocheck
data sHolderVtable 1010A434 nocheck
data sSafeStringVtable 1010A3FC nocheck
data sStateIdVtable 1010A4EC nocheck
data sNoStateId 101FF32C nocheck
data sUiSystem 1018C404 nocheck
data sHeapMgr 101F8B4C nocheck
data sLayoutResMgr 101F7274 nocheck
data sFontMgr 101F4A50 nocheck
data sBlank 1049DD60 nocheck
data sDisplay 1049DD90 nocheck
data sStateNumberReady 101FD8E4 nocheck
data sStateNumber 101FDD50 nocheck
data sBasicButtonName 1049DA40 nocheck
data sHeartName 1049DB44 nocheck
data sInOutLoopName 1049DC28 nocheck
data sKeyIconName 1049DD3C nocheck
data sLoopName 1049DE30 nocheck
data sNoControlName 1049E518 nocheck
data sNoDecideButtonName 1049E6EC nocheck
data sOneTimeName 1049E710 nocheck
data sProgressBarName 1049E750 nocheck
data sValidName 1049E8EC nocheck
str 1010A53C "." in 026F90D8
str 1010A548 "::StateID_Blank" in 026F9EFC
str 1010A558 "::StateID_Display" in 026F9EFC
*/
