/* HD-only item icon button and its singleton allocator. */
#include "d/hd_screen_btn_item_icon.h"
static hdPartsVtbl_c sIconVtable;
static u32 sIconHolderVtable;
static u32 sIconSecondaryVtable;
static hdItemIconAllocatorVtbl_l sIconAllocatorVtable;
static hdItemIcon_c* sIcon;
static hdItemIconAllocator_l* sIconAllocator;
static hdUiSafeString sIconArchive;
static hdUiSafeString sMenuLayoutName;
static hdUiStringTable sIconStringVtable;
static hdPartsStateId_c sPartsBlankState;
void* hdItemIcon_allocate(u32 size, hdScreenHeap* heap, s32 alignment);
void hdItemIcon_constructAllocator(hdItemIconAllocator_l* self, hdScreenHeap* heap, u32 options);
void hdItemIcon_destroyAllocatorBase(hdItemIconAllocator_l* self, s32 flags);
void hdItemPane_getGlobalPosition(hdItemPosition3_c* out, hdPartsPaneSRT_c* pane);



/* 026364F8 */
static s32 hdItemIcon_stateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026365F8 */
void hdItemIcon_c::destroy(hdItemIcon_c* self, s32 flags) {
    if (!self) return;
    self->mButton.mStateHolder.mHolderVtable = &sIconHolderVtable;
    self->mButton.mSecondaryVtable = &sIconSecondaryVtable;
    hdMenuItem_c::destroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 02636664 */
hdItemIcon_c* hdItemIcon_c::construct(hdItemIcon_c* self) {
    if (!self) self = (hdItemIcon_c*)operator_new(sizeof(hdItemIcon_c));
    if (self) {
        hdMenuItem_c::construct(self);
        self->mButton.mStateHolder.mHolderVtable = &sIconHolderVtable;
        self->mButton.mVtable = &sIconVtable;
        self->mButton.mSecondaryVtable = &sIconSecondaryVtable;
        self->mShown = 0;
    }
    return self;
}
/* 026366D8 */
hdItemIcon_c* hdItemIcon_c::create(hdScreenHeap* heap) {
    if (!sIcon) {
        hdItemIcon_c* icon = (hdItemIcon_c*)hdItemIcon_allocate(sizeof(hdItemIcon_c), heap, 4);
        // HD: adding the embedded allocator offset happens even if allocation returned NULL.
        hdItemIconAllocator_l* allocator = gabi::at<hdItemIconAllocator_l>(gabi::ea(icon) + offsetof(hdItemIcon_c, mAllocator));
        if (allocator) {
            hdItemIcon_constructAllocator(allocator, heap, 3);
            allocator->mpVtable = &sIconAllocatorVtable;
        }
        sIconAllocator = allocator;
        if (icon) icon = construct(icon);
        sIcon = icon;
    }
    return sIcon;
}
/* 02636778 */
void hdItemIcon_c::release() {
    hdItemIconAllocator_l* allocator = sIconAllocator;
    if (allocator) {
        sIconAllocator = NULL;
        allocator->vDelete(2);
        hdItemIcon_c* icon = sIcon;
        if (icon) icon->mButton.vDelete(3);
        sIcon = NULL;
    }
}
/* 026367F8 */
u32 hdItemIcon_c::load(void* resource, u32 arg1, u32 arg2) {
    mButton.attachArchives(&sIconArchive, 1);
    if (!mButton.vLoad(&sMenuLayoutName, gabi::ea(mButton.mpResAccessor.get()), gabi::ea(resource), arg1, (void*)gabi::at<void>(arg2))) return 0;
    mButton.vCalcLayout();
    if (!gabi::call_ptr<u32>(mButton.mVtable->mCreateChildren.get(), this)) return 0;
    if (!preparePanes()) return 0;
    preparePane();
    vCachePosition();
    mButton.startState(&sPartsBlankState);
    return 1;
}
/* 0263693C */
void hdItemIcon_c::finalize() {
    releasePanes();
    mButton.vDeleteChildren();
    mButton.vDestroyLayout();
    mButton.detachArchives();
}
/* 02636998 */
void hdItemIcon_c::execute() {
    updateItem();
    updateAnimation();
    mButton.vExecuteChildren();
    mButton.vCalcLayout();
}
/* 026369F4 */
void hdItemIcon_c::draw(void* info) {
    if (mShown) { mButton.vDrawLayout(info); mButton.vDrawChildren(info); }
}
/* 02636A64 */
void hdItemIcon_c::showFromTouch(hdItemPoint_c* point) {
    mShown = 1;
    vTranslatePoint(point);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_ITEM_SLOT_FLOAT, 1.0f);
}
/* 02636AC0 */
void hdItemIcon_c::hideFromUnselect() { mShown = 0; hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f); }
/* 02636AE4 */
void hdItemIcon_c::showFromCursor(hdItemPoint_c* point) {
    mShown = 1;
    vTranslatePoint(point);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_ITEM_SLOT_FLOAT, 1.0f);
}
/* 02636B40 */
void hdItemIcon_c::positionAtPoint(hdItemPoint_c* point) { gabi::tail_ptr(iconVtable()->mTranslatePoint.get(), this, point); }
/* 02636B50 */
void hdItemIcon_c::hideAfterCancel() { mShown = 0; hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f); }
/* 02636B74 */
void hdItemIcon_c::hideAfterFloat() { mShown = 0; hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f); }
/* 02636B98 */
void hdItemIcon_c::animateToPoint(hdItemPoint_c* point) {
    vTranslatePoint(point);
    hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT, HD_ITEM_SLOT_FLOAT, 1.0f);
}
/* 02636BEC */
void hdItemIcon_c::showAtPoint(hdItemPoint_c* point) {
    mShown = 1;
    gabi::tail_ptr(iconVtable()->mTranslatePoint.get(), this, point);
}
/* 02636C04 */
void hdItemIcon_c::hideAfterKey() { mShown = 0; hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT, 1.0f); }
/* 02636C28 */
void hdItemIcon_c::getContext(hdItemIconContext_c* context) {
    context->mpAnimation = mButton.animSet();
    context->mpRootPane = layoutRootPane();
    context->mpPaneGroups[0] = &mPaneGroups[0];
    context->mpPaneGroups[1] = &mPaneGroups[1];
    context->mpPaneGroups[2] = &mPaneGroups[2];
    context->mpVisible = &mShown;
}
/* 02636C68 */
void hdItemIcon_c::cachePosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdPartsPaneSRT_c* pane = layoutRootPane();
    gabi::FrameLocal<hdItemPosition3_c> position(8);
    hdItemPane_getGlobalPosition(position, pane);
    u32 x = position->xBits, y = position->yBits;
    mPanePosition.xBits = x;
    mPanePosition.yBits = y;
    u32 translateX = pane->mTranslationXBits, translateY = pane->mTranslationYBits;
    mPaneTranslation.xBits = translateX;
    mPaneTranslation.yBits = translateY;
}
/* 02636CD0 */
void hdItemIcon_c::translatePoint(hdItemPoint_c* point) {
    f32 dx = point->x - mPanePosition.x;
    f32 dy = point->y - mPanePosition.y;
    hdPartsPaneSRT_c* pane = layoutRootPane();
    f32 x = mPaneTranslation.x + dx;
    f32 y = mPaneTranslation.y + dy;
    u32 z = pane->mTranslationZBits;
    hdItemPaneSetTranslateY(pane, y);
    hdItemPaneSetTranslateX(pane, x);
    pane->mFlags = pane->mFlags | 0x10;
    pane->mTranslationZBits = z;
}
/* 02636D84 */
static void hdItemIcon_destroyAllocator(hdItemIconAllocator_l* self, s32 flags) {
    if (!self) return;
    self->mpVtable = &sIconAllocatorVtable;
    if (self == sIconAllocator) {
        sIconAllocator = NULL;
        sIcon->mButton.vDelete(2);
        sIcon = NULL;
    }
    hdItemIcon_destroyAllocatorBase(self, 0);
    if (flags & 1) operator_delete(self);
}
static hdItemHeaderStatics_c sIconHeaderStatics;
static hdItemHeaderDescriptors_c sIconHeaderDescriptors;
static hdItemHeaderZero_c sIconHeaderZero;
static hdItemPoint_c sIconPiConstants;
/* 02636E2C */
static void __sinit_hd_screen_btn_item_icon_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sIconHeaderStatics), gabi::ea(&sIconHeaderDescriptors), gabi::ea(&sIconHeaderZero),
                                gabi::ea(&sIconPiConstants));
    hdParts_setSafeString(&sIconArchive, &sIconStringVtable, "BtnItemIcon_00");
}
/* WWHD glue
data sIconHeaderStatics 1049019C
data sIconHeaderDescriptors 101F5554
data sIconHeaderZero 104901A8
data sIconPiConstants 100E90CC
data sIconVtable 100E90F8
data sIconHolderVtable 100E9458
data sIconSecondaryVtable 100E9468
data sIconAllocatorVtable 100E90E8
data sIcon 101F5578
data sIconAllocator 101F557C
data sIconArchive 10490194
data sMenuLayoutName 1048FEDC
data sIconStringVtable 100E90AC
data sPartsBlankState 1049DD90
str 100E90D4 "BtnItemIcon_00" in 02636E2C


ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*, u32)
ext 026F91F8 void hdParts_c::detachArchives()
ext 0273B0D4 void* hdItemIcon_allocate(u32, hdScreenHeap*, s32)
ext 02752B0C void hdItemIcon_constructAllocator(hdItemIconAllocator_l*, hdScreenHeap*, u32)
ext 02752BEC void hdItemIcon_destroyAllocatorBase(hdItemIconAllocator_l*, s32)
ext 02704DE4 void hdItemPane_getGlobalPosition(hdItemPosition3_c*, hdPartsPaneSRT_c*)
*/
