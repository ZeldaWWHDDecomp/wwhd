/**
 * hd_screen_staff_roll_bg.cpp
 * HD - the staff roll background "StaffRollBG_00": plays its "All" animation in a loop; state
 * ::StateID_All
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_title_staff_roll). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and state names
 * are HD strings; all other names are ours.
 */

#include "d/hd_screen_staff_roll_bg.h"

static hdPartsVtbl_c sStaffRollBGVtable;
static u32 sStaffRollBGHolderVtable;
static u32 sStaffRollBGSecondaryVtable;
static hdUiStringTable sStaffRollBGSafeStringVtable;
static hdPartsStateIdVtbl_c sStaffRollBGStateIdVtable;
static hdStateId sStaffRollBGNoStateId;
static hdScreenUiSystem* sStaffRollBGUiSystem;

static hdUiSafeString sStaffRollBGGroupNames[1] = {};
static hdUiSafeString sStaffRollBGAnimNames[1] = {};
static hdUiSafeString sStaffRollBGArchiveName;
static f32 sStaffRollBGConst150;
static f32 sStaffRollBGScreenHeight;    /* 720 */
static f32 sStaffRollBGScreenWidth;     /* 1280 */
static f32 sStaffRollBGConst510;
static f32 sStaffRollBGConstMinus660;
static f32 sStaffRollBGConst790;
static f32 sStaffRollBGConstMinus790;
/* pane names of the background layout (not used by this screen's code) */
static hdUiSafeString sStaffRollBGNullPaneNames[2] = {};
static hdUiSafeString sStaffRollBGPictureNames[3] = {};
static hdPartsStateId_c sStaffRollBGAll;
static hdUiSafeString sStaffRollBGLayoutName;
static const u32 sStaffRollBGAnimGroups[1] = {0};
static u32 sStaffRollBGStateNumberReady;
static s32 sStaffRollBGStateNumber;

/* 026E236C */
// hdScreenStaffRollBG_c::~hdScreenStaffRollBG_c() (GHS deleting destructor)
void hdScreenStaffRollBG_c::destroy(hdScreenStaffRollBG_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sStaffRollBGHolderVtable;
    i_this->mSecondaryVtable = &sStaffRollBGSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026E23D8 */
void hdScreenStaffRollBG_c::allEnter() {
    hdAnimSet_play(animSet(), STAFF_ROLL_BG_ANIM_ALL, 0, 1.0f);
}

/* 026E23F4 */
void hdScreenStaffRollBG_c::allExecute() {
    vCalcLayout();
}

/* 026E2404 */
// hdScreenStaffRollBG_c::hdScreenStaffRollBG_c() (GHS: allocates when called with NULL)
hdScreenStaffRollBG_c* hdScreenStaffRollBG_c::construct(hdScreenStaffRollBG_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenStaffRollBG_c*)operator_new(sizeof(hdScreenStaffRollBG_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sStaffRollBGVtable;
    i_this->mSecondaryVtable = &sStaffRollBGSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sStaffRollBGHolderVtable;
    return i_this;
}

/* 026E2470 */
u32 hdScreenStaffRollBG_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(&sStaffRollBGArchiveName, 1);
    if (!vLoad(&sStaffRollBGLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    startState(&sStaffRollBGAll);
    return TRUE;
}

/* 026E2558 */
void hdScreenStaffRollBG_c::finalize() {
    vDestroyLayout();
    detachArchives();
}

/* 026E2598 */
void hdScreenStaffRollBG_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026E25A0 */
u32 hdScreenStaffRollBG_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sStaffRollBGUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 1, 1, desc)) {
        return FALSE;
    }
    hdStaffRollBG_bindGroup(animSet(), STAFF_ROLL_BG_ANIM_ALL, &sStaffRollBGAnimNames[0], &sStaffRollBGGroupNames[sStaffRollBGAnimGroups[0]]);
    vCalcLayout();
    return TRUE;
}

/* 026E26A0 */
// the length of the "All" animation in frames
s32 hdScreenStaffRollBG_c::getAllFrameCount() {
    u32 frames = hdStaffRollBG_getAnimFrameCount(animSet()->mEntries.mData.get()->get()->mController.get());
    return gabi::ftoi((f32)frames);
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 026E2704 */
static void __sinit_hd_screen_staff_roll_bg_cpp() {
    hdStaffRoll_initHeaderStatics(0x1049C210, 0x101F6BE0, 0x1049C1E8, 0x101062C0, 0x1049C20C);
    hdParts_setSafeString(&sStaffRollBGGroupNames[0], &sStaffRollBGSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sStaffRollBGAnimNames[0], &sStaffRollBGSafeStringVtable, "All");
    hdParts_setSafeString(&sStaffRollBGArchiveName, &sStaffRollBGSafeStringVtable, "StaffRollBG_00");
    sStaffRollBGConst150 = 150.0f;
    sStaffRollBGScreenHeight = 720.0f;
    sStaffRollBGScreenWidth = 1280.0f;
    sStaffRollBGConst510 = 510.0f;
    sStaffRollBGConstMinus660 = -660.0f;
    sStaffRollBGConst790 = 790.0f;
    sStaffRollBGConstMinus790 = -790.0f;
    hdParts_setSafeString(&sStaffRollBGNullPaneNames[0], &sStaffRollBGSafeStringVtable, "N_Wave00");
    hdParts_setSafeString(&sStaffRollBGNullPaneNames[1], &sStaffRollBGSafeStringVtable, "N_All_00");
    hdParts_setSafeString(&sStaffRollBGPictureNames[0], &sStaffRollBGSafeStringVtable, "P_Wave00");
    hdParts_setSafeString(&sStaffRollBGPictureNames[1], &sStaffRollBGSafeStringVtable, "P_Wave00_00");
    hdParts_setSafeString(&sStaffRollBGPictureNames[2], &sStaffRollBGSafeStringVtable, "P_BG_00");
    s32 number;
    if (sStaffRollBGStateNumberReady) {
        number = sStaffRollBGStateNumber;
    } else {
        number = 0;
        sStaffRollBGStateNumberReady = 1;
    }
    sStaffRollBGAll.setup(number + 1, "::StateID_All", &sStaffRollBGStateIdVtable, 0x19, &sStaffRollBGNoStateId);
    sStaffRollBGStateNumber = number + 1;
    hdParts_setSafeString(&sStaffRollBGLayoutName, &sStaffRollBGSafeStringVtable, "StaffRollBG_00");
}

/* 026E2A04 */
static s32 hdScreenStaffRollBG_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sStaffRollBGVtable 10106350 nocheck
data sStaffRollBGHolderVtable 10106430 nocheck
data sStaffRollBGSecondaryVtable 10106440 nocheck
data sStaffRollBGSafeStringVtable 10106204 nocheck
data sStaffRollBGStateIdVtable 10106264 nocheck
data sStaffRollBGNoStateId 101FF32C nocheck
data sStaffRollBGUiSystem 1018C404 nocheck
data sStaffRollBGGroupNames 1049C1D0 nocheck
data sStaffRollBGAnimNames 1049C1D8 nocheck
data sStaffRollBGArchiveName 1049C1E0 nocheck
data sStaffRollBGConst150 1049C1F0 nocheck
data sStaffRollBGScreenHeight 1049C1F4 nocheck
data sStaffRollBGScreenWidth 1049C1F8 nocheck
data sStaffRollBGConst510 1049C1FC nocheck
data sStaffRollBGConstMinus660 1049C200 nocheck
data sStaffRollBGConst790 1049C204 nocheck
data sStaffRollBGConstMinus790 1049C208 nocheck
data sStaffRollBGNullPaneNames 1049C220 nocheck
data sStaffRollBGPictureNames 1049C230 nocheck
data sStaffRollBGAll 1049C248 nocheck
data sStaffRollBGLayoutName 1049C278 nocheck
data sStaffRollBGAnimGroups 101061FC
data sStaffRollBGStateNumberReady 101FD8E4 nocheck
data sStaffRollBGStateNumber 101FDD50 nocheck
str 1010630C "G_All_00" in 026E2704
str 101062EC "All" in 026E2704
str 10106318 "StaffRollBG_00" in 026E2704
str 10106328 "N_Wave00" in 026E2704
str 10106334 "N_All_00" in 026E2704
str 10106340 "P_Wave00" in 026E2704
str 101062F0 "P_Wave00_00" in 026E2704
str 101062E4 "P_BG_00" in 026E2704
str 101062FC "::StateID_All" in 026E2704
ext 02004E04 void hdStaffRollBG_bindGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
ext 028713B8 u32 hdStaffRollBG_getAnimFrameCount(hdAnimSetController*)
*/
