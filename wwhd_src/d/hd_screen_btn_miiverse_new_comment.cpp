/* HD-only Miiverse new-comment button; local decomp */
#include "d/hd_screen_btn_miiverse_comment.h"
static u32 sBtnMiiverseNewCommentHeaderPiPair;
static u32 sBtnMiiverseNewCommentHeaderRegistration;
static u32 sBtnMiiverseNewCommentHeaderZeroObject;
static hdPartsVtbl_c sNewCommentVtable;
static u8 sNewCommentHolderVtable;
static u8 sNewCommentSecondaryVtable;
static hdUiSafeString sNewCommentResource;
static hdUiSafeString sNewCommentGroup;
struct NewCommentAnimationNames { hdUiSafeString mEntries[2]; };
static NewCommentAnimationNames sNewCommentAnimations;
struct NewCommentAnimationIndex { be<u32> mValue; };
static NewCommentAnimationIndex sLoopAnimationIndex;
static hdScreenUiSystem* sNewCommentUiSystem;
static hdUiStringTable sNewCommentStringVtable;
enum { LOOP_ENTRY = 7, LOOP_SLOT = 1, ANIMATION_COUNT = 8, GROUP_COUNT = 2 };

/* 02641000 */
static u8 invokeMiiverseTextWidthDelegate(hdMiiverseCommentDelegate_c* delegate, hdBreakString_c* text, u32 length) {
    void* owner = delegate->mpOwner;
    if (owner == NULL) return 0;
    s16 index = delegate->mMethod.mVirtualIndex;
    if (index == 0) return 0;
    void* target = hdPartsView<void>(gabi::ea(owner) + (s16)delegate->mMethod.mThisDelta);
    u32 function;
    if (index < 0) {
        function = delegate->mMethod.mFunction;
    } else {
        s16 vtableOffset = (s16)(u32)delegate->mMethod.mFunction;
        gptr<void>* vtablePointer = hdPartsView<gptr<void>>(gabi::ea(target) + vtableOffset);
        gfn* entry = hdPartsView<gfn>(gabi::ea(vtablePointer->get()) + u32(index) * 8 + 4);
        function = entry->get();
    }
    return (u8)gabi::call_ptr<u32>(function, target, text, length);
}

/* 02641084 */
static u32 getNewCommentStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 02641184 */
void hdBtnMiiverseNewComment_c::destroy(hdBtnMiiverseNewComment_c* button, s32 flags) {
    if (button == NULL) return;
    button->mButton.mStateHolder.mHolderVtable = &sNewCommentHolderVtable;
    button->mButton.mSecondaryVtable = &sNewCommentSecondaryVtable;
    hdBasicButton_c::destroy(button, 0);
    if (flags & 1) operator_delete(button);
}

/* 026411F0 */
hdBtnMiiverseNewComment_c* hdBtnMiiverseNewComment_c::construct(hdBtnMiiverseNewComment_c* button) {
    if (button == NULL) {
        button = (hdBtnMiiverseNewComment_c*)operator_new(sizeof(hdBtnMiiverseNewComment_c));
        if (button == NULL) return NULL;
    }
    hdPartsBasicButton_construct(&button->mButton);
    button->mButton.mVtable = &sNewCommentVtable;
    button->mButton.mStateHolder.mHolderVtable = &sNewCommentHolderVtable;
    button->mButton.mSecondaryVtable = &sNewCommentSecondaryVtable;
    return button;
}

/* 0264125C */
u32 hdBtnMiiverseNewComment_c::init(void* resource, u32 mode, hdPartsDesc_c* desc) {
    return hdPartsBasicButton_init(&mButton, &sNewCommentResource, (u32)gabi::ea(resource), mode, desc);
}

/* 02641274 */
void hdBtnMiiverseNewComment_c::stopLoop() { hdAnimSet_operationCC(mButton.mpLayout->animSet(), LOOP_ENTRY, LOOP_SLOT); }
/* 02641288 */
void hdBtnMiiverseNewComment_c::startLoop() { hdAnimSet_playAlternate(mButton.mpLayout->animSet(), LOOP_ENTRY, LOOP_SLOT, 1.0f); }
/* 026412A4 */
void hdBtnMiiverseNewComment_c::pauseLoop() { hdAnimSet_operationC4(mButton.mpLayout->animSet(), LOOP_ENTRY, LOOP_SLOT); }

/* 026412B8 */
u32 hdBtnMiiverseNewComment_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sNewCommentUiSystem->mHeap, 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, first, second, third, ANIMATION_COUNT, GROUP_COUNT, desc)) return 0;
    u32 index = sLoopAnimationIndex.mValue;
    hdAnimSet* animations = mButton.mpLayout->animSet();
    setupMiiverseAnimation(animations, LOOP_ENTRY, &sNewCommentGroup, hdPartsView<hdUiSafeString>(gabi::ea(&sNewCommentAnimations) + index * sizeof(hdUiSafeString)));
    return 1;
}

/* 026413A4 */
void hdBtnMiiverseNewComment_c::initializeStatics() {
    sinit_header_statics_z(gabi::ea(&sBtnMiiverseNewCommentHeaderPiPair), gabi::ea(&sBtnMiiverseNewCommentHeaderRegistration), gabi::ea(&sBtnMiiverseNewCommentHeaderZeroObject));
    sNewCommentAnimations.mEntries[0].mVtable = &sNewCommentStringVtable;
    sNewCommentAnimations.mEntries[0].mText.v = gabi::guest_addr("G_Btn_00");
    sNewCommentAnimations.mEntries[1].mVtable = &sNewCommentStringVtable;
    sNewCommentGroup.mVtable = &sNewCommentStringVtable;
    sNewCommentGroup.mText.v = gabi::guest_addr("Loop");
    sNewCommentResource.mVtable = &sNewCommentStringVtable;
    sNewCommentResource.mText.v = gabi::guest_addr("BtnMiiverseNewComment_00");
    sNewCommentAnimations.mEntries[1].mText.v = gabi::guest_addr("G_Loop_00");
}

/* WWHD glue
 data sBtnMiiverseNewCommentHeaderPiPair 10490988 nocheck
 data sBtnMiiverseNewCommentHeaderRegistration 101F56C8 nocheck
 data sBtnMiiverseNewCommentHeaderZeroObject 10490994 nocheck
 data sNewCommentVtable 100EB568
 data sNewCommentHolderVtable 100EB7C0
 data sNewCommentSecondaryVtable 100EB7D0
 data sNewCommentResource 104909B4
 data sNewCommentGroup 10490980
 data sNewCommentAnimations 104909A4
 data sLoopAnimationIndex 100EB504
 data sNewCommentUiSystem 1018C404
 data sNewCommentStringVtable 100EB508
 str 100EB540 "G_Btn_00" in 026413A4
 str 100EB52C "Loop"
 str 100EB54C "BtnMiiverseNewComment_00"
 str 100EB534 "G_Loop_00"
 ext 026F0B8C void hdBasicButton_c::destroy(hdBasicButton_c*, u32)
 ext 02004E04 void setupMiiverseAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
*/
