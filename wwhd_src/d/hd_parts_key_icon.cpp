/**
 * hd_parts_key_icon.cpp
 * HD - the "KeyIcon" UI part: In, Wait, Decide (back to Wait or Out), Out
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_fw_parts_loop_display). Each function is marked with its WWHD address; the glue block at the
 * end lists this translation unit's data (tools/verify/autoglue.py). "KeyIcon", the animation, group
 * and state names are HD strings; all other names are ours.
 */

#include "d/hd_parts_key_icon.h"
#include "d/hd_tu_header_statics.h"

static hdPartsVtbl_c sKeyIconVtable;
static u32 sKeyIconHolderVtable;
static u32 sKeyIconSecondaryVtable;
static hdUiStringTable sKeyIconSafeStringVtable;
static hdPartsStateIdVtbl_c sKeyIconStateIdVtable;
static hdStateId sKeyIconNoStateId;
static hdScreenUiSystem* sKeyIconUiSystem;

static hdUiSafeString sKeyIconAnimNames[4] = {};
static hdUiSafeString sKeyIconGroupNames[2] = {};
static hdUiSafeString sKeyIconTypeName;
static hdPartsStateId_c sKeyIconIn;
static hdPartsStateId_c sKeyIconWait;
static hdPartsStateId_c sKeyIconDecide;
static hdPartsStateId_c sKeyIconOut;
static hdPartsStateId_c sKeyIconBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static u32 sKeyIconStateNumberReady;
static s32 sKeyIconStateNumber;

/* 026F7DAC */
// hdPartsKeyIcon_c::~hdPartsKeyIcon_c() (GHS deleting destructor)
void hdPartsKeyIcon_c::destroy(hdPartsKeyIcon_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sKeyIconHolderVtable;
    i_this->mSecondaryVtable = &sKeyIconSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026F7E18 */
void hdPartsKeyIcon_c::inEnter() {
    hdAnimSet_play(animSet(), KEY_ICON_ANIM_IN, 0, 1.0f);
    hdAnimSet_operationCC(animSet(), KEY_ICON_ANIM_LOOP, 1);
    mRequest = KEY_ICON_REQUEST_NONE;
}

/* 026F7E78 */
void hdPartsKeyIcon_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), KEY_ICON_ANIM_IN)) {
        changeState(&sKeyIconWait);
    }
}

/* 026F7EC8 */
void hdPartsKeyIcon_c::waitEnter() {
    hdAnimSet_playAlternate(animSet(), KEY_ICON_ANIM_LOOP, 1, 1.0f);
    mRequest = KEY_ICON_REQUEST_NONE;
}

/* 026F7F14 */
void hdPartsKeyIcon_c::waitExecute() {
    if (mRequest == KEY_ICON_REQUEST_DECIDE) {
        mRequest = KEY_ICON_REQUEST_NONE;
        changeState(&sKeyIconDecide);
    } else if (mRequest == KEY_ICON_REQUEST_OUT) {
        mRequest = KEY_ICON_REQUEST_NONE;
        changeState(&sKeyIconOut);
    }
}

/* 026F7F58 */
void hdPartsKeyIcon_c::decideEnter() {
    hdAnimSet_play(animSet(), KEY_ICON_ANIM_DECIDE, 0, 1.0f);
    hdAnimSet_operationC4(animSet(), KEY_ICON_ANIM_LOOP, 1);
}

/* 026F7FB0 */
void hdPartsKeyIcon_c::decideExecute() {
    if (mDecideToWait) {
        if (hdAnimSet_isEnd(animSet(), KEY_ICON_ANIM_DECIDE)) {
            changeState(&sKeyIconWait);
        } else if (mRequest == KEY_ICON_REQUEST_DECIDE) {
            mRequest = KEY_ICON_REQUEST_NONE;
            changeState(&sKeyIconDecide);
        }
    } else if (hdAnimSet_isEnd(animSet(), KEY_ICON_ANIM_DECIDE)) {
        changeState(&sKeyIconOut);
    }
}

/* 026F8054 */
void hdPartsKeyIcon_c::outEnter() {
    hdAnimSet_play(animSet(), KEY_ICON_ANIM_OUT, 0, 1.0f);
    hdAnimSet_operationC4(animSet(), KEY_ICON_ANIM_LOOP, 1);
}

/* 026F80AC */
void hdPartsKeyIcon_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), KEY_ICON_ANIM_OUT)) {
        changeState(&sKeyIconBlank);
    }
}

/* 026F80FC */
// hdPartsKeyIcon_c::hdPartsKeyIcon_c() (GHS: allocates when called with NULL)
hdPartsKeyIcon_c* hdPartsKeyIcon_c::construct(hdPartsKeyIcon_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdPartsKeyIcon_c*)operator_new(sizeof(hdPartsKeyIcon_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mStateHolder.mHolderVtable = &sKeyIconHolderVtable;
    i_this->mVtable = &sKeyIconVtable;
    i_this->mSecondaryVtable = &sKeyIconSecondaryVtable;
    i_this->mDecideToWait = FALSE;
    i_this->mRequest = KEY_ICON_REQUEST_NONE;
    return i_this;
}

/* 026F8174 */
u32 hdPartsKeyIcon_c::init(void* resource, u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    if (!vLoad(resource, 0, arg1, arg2, desc)) {
        return FALSE;
    }
    startState(&sKeyIconBlank);
    return TRUE;
}

/* 026F8210 */
void hdPartsKeyIcon_c::finalize() {
    vDestroyLayout();
}

/* 026F8220 */
void hdPartsKeyIcon_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026F8228 */
void hdPartsKeyIcon_c::requestIn() {
    changeState(&sKeyIconIn);
}

/* 026F8238 */
void hdPartsKeyIcon_c::requestDecide() {
    mRequest = KEY_ICON_REQUEST_DECIDE;
    mDecideToWait = FALSE;
}

/* 026F824C */
void hdPartsKeyIcon_c::requestDecideAndWait() {
    mRequest = KEY_ICON_REQUEST_DECIDE;
    mDecideToWait = TRUE;
}

/* 026F8260 */
void hdPartsKeyIcon_c::requestOut() {
    mRequest = KEY_ICON_REQUEST_OUT;
}

/* 026F826C */
void hdPartsKeyIcon_c::hide() {
    hdAnimSet_operationCC(animSet(), KEY_ICON_ANIM_IN, 0);
    hdAnimSet_operationCC(animSet(), KEY_ICON_ANIM_LOOP, 1);
    changeState(&sKeyIconBlank);
}

/* inlined: is the current state `stateId` (its vtable is read before the current state's key) */
static bool hdPartsKeyIcon_isState(hdParts_c* parts, hdPartsStateId_c* stateId) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(parts->stateMachine());
    u32 getCurrentKey = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* vtable = stateId->mVtable;
    u32 key = gabi::call_ptr<u32>(getCurrentKey, current);
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), stateId);
}

/* 026F82CC */
u32 hdPartsKeyIcon_c::isWaiting() {
    return hdPartsKeyIcon_isState(this, &sKeyIconWait);
}

/* 026F833C */
u8 hdPartsKeyIcon_c::isNotDeciding() {
    return !hdPartsKeyIcon_isState(this, &sKeyIconDecide);
}

/* 026F83B4 */
u32 hdPartsKeyIcon_c::isHidden() {
    return hdPartsKeyIcon_isState(this, &sKeyIconBlank);
}

/* 026F8424 */
u32 hdPartsKeyIcon_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sKeyIconUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 4, 4, desc)) {
        return FALSE;
    }
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

static hdTuHeaderStatics_c sKeyIconHeaderStatics;
static hdTuHeaderDescriptors_c sKeyIconHeaderDescriptors;
static hdTuHeaderZero_c sKeyIconHeaderZero;
static hdTuPiPair_c sKeyIconPiConstants;
/* 026F84E0 */
static void __sinit_hd_parts_key_icon_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sKeyIconHeaderStatics), gabi::ea(&sKeyIconHeaderDescriptors), gabi::ea(&sKeyIconHeaderZero),
                                gabi::ea(&sKeyIconPiConstants));
    hdParts_setSafeString(&sKeyIconAnimNames[0], &sKeyIconSafeStringVtable, "In");
    hdParts_setSafeString(&sKeyIconAnimNames[1], &sKeyIconSafeStringVtable, "Out");
    hdParts_setSafeString(&sKeyIconAnimNames[2], &sKeyIconSafeStringVtable, "Loop");
    hdParts_setSafeString(&sKeyIconAnimNames[3], &sKeyIconSafeStringVtable, "Decide");
    hdParts_setSafeString(&sKeyIconGroupNames[0], &sKeyIconSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sKeyIconGroupNames[1], &sKeyIconSafeStringVtable, "G_Loop_00");
    hdParts_setSafeString(&sKeyIconTypeName, &sKeyIconSafeStringVtable, "KeyIcon");
    s32 number;
    if (sKeyIconStateNumberReady) {
        number = sKeyIconStateNumber;
    } else {
        number = 0;
        sKeyIconStateNumberReady = 1;
    }
    sKeyIconIn.setup(number + 1, "::StateID_In", &sKeyIconStateIdVtable, 0x19, &sKeyIconNoStateId);
    sKeyIconWait.setup(number + 2, "::StateID_Wait", &sKeyIconStateIdVtable, 0x1C, &sKeyIconNoStateId);
    sKeyIconDecide.setup(number + 3, "::StateID_Decide", &sKeyIconStateIdVtable, 0x1F, &sKeyIconNoStateId);
    sKeyIconOut.setup(number + 4, "::StateID_Out", &sKeyIconStateIdVtable, 0x22, &sKeyIconNoStateId);
    sKeyIconStateNumber = number + 4;
}

/* 026F87F0 */
static s32 hdPartsKeyIcon_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sKeyIconHeaderStatics 1049DC30
data sKeyIconHeaderDescriptors 101F71C0
data sKeyIconHeaderZero 1049DC3C
data sKeyIconPiConstants 1010A22C
data sKeyIconVtable 1010A2B0 nocheck
data sKeyIconHolderVtable 1010A3D8 nocheck
data sKeyIconSecondaryVtable 1010A3E8 nocheck
data sKeyIconSafeStringVtable 1010A17C nocheck
data sKeyIconStateIdVtable 1010A1DC nocheck
data sKeyIconNoStateId 101FF32C nocheck
data sKeyIconUiSystem 1018C404 nocheck
data sKeyIconAnimNames 1049DC5C nocheck
data sKeyIconGroupNames 1049DC4C nocheck
data sKeyIconTypeName 1049DD3C nocheck
data sKeyIconIn 1049DC7C nocheck
data sKeyIconWait 1049DCAC nocheck
data sKeyIconDecide 1049DCDC nocheck
data sKeyIconOut 1049DD0C nocheck
data sKeyIconBlank 1049DD60 nocheck
data sKeyIconStateNumberReady 101FD8E4 nocheck
data sKeyIconStateNumber 101FDD50 nocheck
str 1010A240 "In" in 026F84E0
str 1010A23C "Out" in 026F84E0
str 1010A244 "Loop" in 026F84E0
str 1010A24C "Decide" in 026F84E0
str 1010A270 "G_All_00" in 026F84E0
str 1010A254 "G_Loop_00" in 026F84E0
str 1010A234 "KeyIcon" in 026F84E0
str 1010A27C "::StateID_In" in 026F84E0
str 1010A28C "::StateID_Wait" in 026F84E0
str 1010A29C "::StateID_Decide" in 026F84E0
str 1010A260 "::StateID_Out" in 026F84E0
*/
