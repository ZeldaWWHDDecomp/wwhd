/* HD Loop part specialized for layouts and deferred animation commands. */
#include "d/hd_layout_mgr.h"
static u32 sLoopHeaderStatics;
static u32 sLoopHeaderDescriptors;
static u32 sLoopHeaderZero;
static u32 sLoopPiConstants;
static u32 sLoopDelegateTable;
static u32 sLoopOwnerTable;
static hdPartsVtbl_c sLoopTable;
static hdLayoutUiSystem_c* sLoopUi;
static u32 sLoopAnimation;
static hdScreenString sLoopName;
static hdScreenStringTable sLoopStringTable;
void loopConstructBase(hdLayoutLoop_c*);
void loopDestroyBase(hdParts_c*, u32);
void* loopAllocate(u32, hdLayoutHeap_c*, u32);
hdPartsLayout_c* loopConstructLayout(hdPartsLayout_c*);
void loopAnimationStop(hdAnimSet*, u32, u32);
void loopAnimationPlay(hdAnimSet*, u32, u32, f32);
void loopAnimationFrame(hdAnimSet*, u32, u32, f32);
void loopAnimationReset(hdAnimSet*, u32, u32, f32);
/* 026FC140 */
static void destroyLayoutLoop(hdParts_c* object, u32 flags) {
    if (!object)
        return;
    object->mStateHolder.mHolderVtable = &sLoopDelegateTable;
    object->mSecondaryVtable = &sLoopOwnerTable;
    loopDestroyBase(object, 0);
    if (flags & 1)
        operator_delete(object);
}
/* 026FC1AC */
static hdLayoutLoop_c* constructLayoutLoop(hdLayoutLoop_c* object) {
    if (!object) {
        object = (hdLayoutLoop_c*)operator_new(sizeof(hdLayoutLoop_c));
        if (!object)
            return NULL;
    }
    loopConstructBase(object);
    object->mStateHolder.mHolderVtable = &sLoopDelegateTable;
    object->mVtable = &sLoopTable;
    object->mSecondaryVtable = &sLoopOwnerTable;
    object->mAnimationRequest = hdLayoutLoop_c::NoRequest;
    return object;
}
/* 026FC220 */
u32 hdLayoutLoop_c::initialize(void* resource, u32 first, u32 second, u32 third) {
    if (!gabi::call_ptr<u32>(mVtable->mLoad.get(), this, resource, 0u, first, second, third))
        return 0;
    loopAnimationStop(mpLayout->animSet(), 0, sLoopAnimation);
    mAnimationRequest = Play;
    return 1;
}
/* 026FC2AC */
u32 hdLayoutLoop_c::update() {
    return gabi::call_ptr<u32>(mVtable->mDestroyLayout.get(), this);
}
/* 026FC2BC */
void hdLayoutLoop_c::applyRequest() {
    u32 request = mAnimationRequest;
    if (request > NoRequest - 1)
        return;
    hdPartsLayout_c* layout = mpLayout;
    mAnimationRequest = NoRequest;
    u32 animation = sLoopAnimation;
    hdAnimSet* set = layout->animSet();
    switch (request) {
    case Play:
        loopAnimationPlay(set, 0, animation, 1.0f);
        break;
    case Stop:
        loopAnimationStop(set, 0, animation);
        break;
    case SetFrame:
        loopAnimationFrame(set, 0, animation, 1.0f);
        break;
    case Reset:
        loopAnimationReset(set, 0, animation, 1.0f);
        break;
    }
}
/* 026FC358 */
u32 hdLayoutLoop_c::createLayout(void* resource, u32 first, u32 second, u32 third, u32 last) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)loopAllocate(sizeof(hdPartsLayout_c), sLoopUi->mpHeap, 4);
    if (layout)
        layout = loopConstructLayout(layout);
    mpLayout = (hdPartsLayout_c*)layout;
    if (!layout)
        return 0;
    // The layout's most-derived vtable has the build entry at +0x14.
    hdPartsLayoutVtbl_c* table = (hdPartsLayoutVtbl_c*)layout->mVtable.get();
    gabi::call_ptr(table->mLoad.get(), layout, resource, first, second, third, 1u, 1u, last);
    return 1;
}
/* 026FC40C */
static void __sinit_hd_layout_loop_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sLoopHeaderStatics), gabi::ea(&sLoopHeaderDescriptors), gabi::ea(&sLoopHeaderZero),
                                gabi::ea(&sLoopPiConstants));
    sLoopName.mVtable = &sLoopStringTable;
    sLoopName.mText.v = gabi::guest_addr("Loop");
}
/* WWHD glue
data sLoopHeaderStatics 1049DE14 nocheck
data sLoopHeaderDescriptors 101F727C nocheck
data sLoopHeaderZero 1049DE20 nocheck
data sLoopPiConstants 1010AA34 nocheck
data sLoopDelegateTable 1010AB10
data sLoopOwnerTable 1010AB20
data sLoopTable 1010AA48
data sLoopUi 1018C404
data sLoopAnimation 1010AA14
data sLoopName 1049DE30
data sLoopStringTable 1010AA18
str 1010AA3C "Loop"
ext 026F89E8 void loopConstructBase(hdLayoutLoop_c*)
ext 026F88F0 void loopDestroyBase(hdParts_c*,u32)
ext 0273B050 void* loopAllocate(u32,hdLayoutHeap_c*,u32)
ext 026FADB0 hdPartsLayout_c* loopConstructLayout(hdPartsLayout_c*)
ext 02005674 void loopAnimationStop(hdAnimSet*,u32,u32)
ext 02005488 void loopAnimationPlay(hdAnimSet*,u32,u32,f32)
ext 020055E0 void loopAnimationFrame(hdAnimSet*,u32,u32,f32)
ext 020053E4 void loopAnimationReset(hdAnimSet*,u32,u32,f32)
*/
