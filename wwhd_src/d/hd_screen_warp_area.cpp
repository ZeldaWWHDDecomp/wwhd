/* HD WarpArea_00: Loop and Change layout animation tracks.
 * */
#include "d/hd_screen_warp_area.h"
static u32 sWarpAreaHeaderStatics;
static u32 sWarpAreaHeaderDescriptors;
static u32 sWarpAreaHeaderZero;
static u32 sWarpAreaPiConstants;

static u32 sWarpAreaVtable;
static u32 sWarpAreaHolderVtable;
static u32 sWarpAreaSecondaryVtable;
static f32 sAnimationEnd = 1.0f;
static f32 sAnimationStart = 0.0f;
static hdUiSafeString sScreenName;
static hdUiSafeString sAnimationNames[2] = {};
static hdUiSafeString sAnimationPanes[2] = {};
static hdPartsStateId_c sLoop;
static hdWarpAreaUiSystemPointer_l sUiSystem;
static u32 sAnimationPaneIndices[2] = {0, 1};
static u32 sStateCounterReady;
static u32 sStateCounter;
static u8 sNullStateContext;
static u32 sSafeStringVtable;
static u32 sLoopStateVtable;

/* 026EEAB4 */
static u32 hdWarpArea_stateKey(hdWarpAreaStateKey_l* state) {
    return state->mKey;
}

/* 026EEBB4 */
void hdWarpArea_c::destroy(hdWarpArea_c* area, u32 flags) {
    if (!area) return;
    area->mStateHolder.mHolderVtable.v = gabi::ea(&sWarpAreaHolderVtable);
    area->mSecondaryVtable.v = gabi::ea(&sWarpAreaSecondaryVtable);
    hdWarpArea_destroyBase(area, 0);
    if (flags & 1) operator_delete(area);
}

/* 026EEC20 */
void hdWarpArea_c::initializeAnimations() {
    hdWarpArea_startAnimation(mpLayout->animSet(), 0, 0, sAnimationEnd);
    hdWarpArea_setAnimationFrame(mpLayout->animSet(), 1, 1, sAnimationStart);
}

/* 026EEC80 */
void hdWarpArea_c::calculateLayout() {
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EEC90 */
hdWarpArea_c* hdWarpArea_c::construct(hdWarpArea_c* area) {
    if (!area) area = static_cast<hdWarpArea_c*>(operator_new(sizeof(hdWarpArea_c)));
    if (area) {
        hdWarpArea_constructBase(area);
        area->mVtable.v = gabi::ea(&sWarpAreaVtable);
        area->mStateHolder.mHolderVtable.v = gabi::ea(&sWarpAreaHolderVtable);
        area->mSecondaryVtable.v = gabi::ea(&sWarpAreaSecondaryVtable);
    }
    return area;
}

/* 026EED90 */
void hdWarpArea_c::showChange() {
    hdWarpArea_setAnimationFrame(mpLayout->animSet(), 1, 1, sAnimationEnd);
}

/* 026EEDAC */
void hdWarpArea_c::hideChange() {
    hdWarpArea_setAnimationFrame(mpLayout->animSet(), 1, 1, sAnimationStart);
}

/* 026EECFC */
u32 hdWarpArea_c::setup(u32 first, u32 second, u32 third) {
    if (!hdWarpArea_setupBase(this, &sScreenName, first, second, third)) return 0;
    hdStateObject* state = gabi::call_ptr<hdStateObject*>(mStateHolder.mMachine.mpOwner->mpVTable->mCreateState.get(), mStateHolder.mMachine.mpOwner.get(), &sLoop);
    mStateHolder.mMachine.mpCurrentState = state;
    state->enter();
    return 1;
}

/* 026EEDC8 */
u32 hdWarpArea_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* description) {
    hdPartsLayout_c* layout = hdWarpArea_allocateLayout(sizeof(hdPartsLayout_c), sUiSystem.mpSystem->mHeap.get(), 4);
    if (layout) layout = hdWarpArea_constructLayout(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 2, description)) return 0;
    for (u32 track = 0; track < 2; ++track) {
        hdWarpArea_bindAnimation(mpLayout->animSet(), track, &sAnimationNames[track], &sAnimationPanes[sAnimationPaneIndices[track]]);
    }
    return 1;
}

/* 026EEEE0 */
static void __sinit_hd_screen_warp_area_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sWarpAreaHeaderStatics), gabi::ea(&sWarpAreaHeaderDescriptors), gabi::ea(&sWarpAreaHeaderZero),
                                gabi::ea(&sWarpAreaPiConstants));
    sAnimationNames[0].mVtable.v = gabi::ea(&sSafeStringVtable);
    sAnimationNames[0].mText.v = gabi::guest_addr("Loop");
    sAnimationNames[1].mVtable.v = gabi::ea(&sSafeStringVtable);
    sAnimationPanes[0].mVtable.v = gabi::ea(&sSafeStringVtable);
    sAnimationPanes[0].mText.v = gabi::guest_addr("G_Loop_00");
    sAnimationPanes[1].mVtable.v = gabi::ea(&sSafeStringVtable);
    sAnimationNames[1].mText.v = gabi::guest_addr("Change");
    sAnimationPanes[1].mText.v = gabi::guest_addr("G_Change_00");
    u32 guard = sStateCounterReady;
    sScreenName.mVtable.v = gabi::ea(&sSafeStringVtable);
    sScreenName.mText.v = gabi::guest_addr("WarpArea_00");
    u32 id;
    if (guard) id = sStateCounter + 1;
    else {
        sStateCounterReady = 1;
        id = 1;
    }
    sLoop.mEnter.setVirtual(0x19);
    sLoop.mExecute.setVirtual(0x1A);
    sLoop.mNumber = id;
    sLoop.mExit.setVirtual(0x1B);
    sLoop.mName.v = gabi::guest_addr("::StateID_Loop");
    sStateCounter = id;
    sLoop.mUnused.clear();
    sLoop.mVtable.v = gabi::ea(&sLoopStateVtable);
    sLoop.mpNullStateId.v = gabi::ea(&sNullStateContext);
}

/* WWHD glue
data sWarpAreaHeaderStatics 1049D524 nocheck
data sWarpAreaHeaderDescriptors 101F70C0 nocheck
data sWarpAreaHeaderZero 1049D530 nocheck
data sWarpAreaPiConstants 10108FB0 nocheck
 data sWarpAreaVtable 10109000 nocheck
 data sWarpAreaHolderVtable 101090E0 nocheck
 data sWarpAreaSecondaryVtable 101090F0 nocheck
 data sSafeStringVtable 10108EFC nocheck
 data sLoopStateVtable 10108F5C nocheck
data sAnimationEnd 10108FA8
data sAnimationStart 10108FAC
data sScreenName 1049D590 nocheck
data sAnimationNames 1049D550 nocheck
data sAnimationPanes 1049D540 nocheck
data sLoop 1049D560 nocheck
data sUiSystem 1018C404 nocheck
data sAnimationPaneIndices 10108EF0
data sStateCounterReady 101FD8E4 nocheck
data sStateCounter 101FDD50 nocheck
data sNullStateContext 101FF32C nocheck
str 10108FB8 "Loop" in 026EEEE0
str 10108FC0 "Change" in 026EEEE0
str 10108FC8 "G_Change_00" in 026EEEE0
str 10108FD4 "WarpArea_00" in 026EEEE0
str 10108FE0 "G_Loop_00" in 026EEEE0
str 10108FEC "::StateID_Loop" in 026EEEE0
ext 02700B48 u32 hdWarpArea_setupBase(hdWarpArea_c* area, hdUiSafeString* name, u32 first, u32 second, u32 third)
ext 0273B050 hdPartsLayout_c* hdWarpArea_allocateLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 026FADB0 hdPartsLayout_c* hdWarpArea_constructLayout(hdPartsLayout_c* layout)
ext 02004E04 void hdWarpArea_bindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
ext 02700ADC void hdWarpArea_constructBase(hdWarpArea_c* area)
ext 02700A70 void hdWarpArea_destroyBase(hdWarpArea_c* area, u32 flags)
ext 020053E4 void hdWarpArea_startAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame)
ext 0200552C void hdWarpArea_setAnimationFrame(hdAnimSet* animation, u32 track, u32 mode, f32 frame)
*/
