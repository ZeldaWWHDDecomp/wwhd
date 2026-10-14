/**
 * hd_parts_in_out_loop.cpp
 * HD - the "InOutLoop" UI part: In animation, loop (with a looping sound), Out on request
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_fw_parts_loop_display). Each function is marked with its WWHD address; the glue block at the
 * end lists this translation unit's data (tools/verify/autoglue.py). "InOutLoop" and the state names
 * are HD strings; all other names are ours.
 */

#include "d/hd_parts_in_out_loop.h"
#include "d/hd_tu_header_statics.h"

static hdPartsVtbl_c sInOutLoopVtable;
static u32 sInOutLoopHolderVtable;
static u32 sInOutLoopSecondaryVtable;
static hdUiStringTable sInOutLoopSafeStringVtable;
static hdPartsStateIdVtbl_c sInOutLoopStateIdVtable;
static hdStateId sInOutLoopNoStateId;
static hdPartsChar_t* sInOutLoopNullString; /* the empty string a SafeString starts with */
static hdScreenUiSystem* sInOutLoopUiSystem;
static u32 sInOutLoopAnimSlots[3] = {0, 0, 1};

static hdUiSafeString sInOutLoopTypeName;
static hdPartsStateId_c sInOutLoopWait;
static hdPartsStateId_c sInOutLoopIn;
static hdPartsStateId_c sInOutLoopLoop;
static hdPartsStateId_c sInOutLoopOut;
static u32 sInOutLoopStateNumberReady;
static s32 sInOutLoopStateNumber;

/* 026F7520 */
// hdPartsInOutLoop_c::~hdPartsInOutLoop_c() (GHS deleting destructor)
void hdPartsInOutLoop_c::destroy(hdPartsInOutLoop_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sInOutLoopHolderVtable;
    i_this->mSecondaryVtable = &sInOutLoopSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026F758C */
void hdPartsInOutLoop_c::waitEnter() {
    mOutRequested = FALSE;
    mInRequested = FALSE;
}

/* 026F759C */
void hdPartsInOutLoop_c::waitExecute() {
    if (mInRequested) {
        changeState(&sInOutLoopIn);
    }
}

/* 026F75B8 */
void hdPartsInOutLoop_c::waitExit() {
    mIsClosed = FALSE;
}

/* 026F75C4 */
void hdPartsInOutLoop_c::startAnim(u32 anim) {
    hdAnimSet_play(animSet(), anim, sInOutLoopAnimSlots[anim], 1.0f);
}

/* 026F75E8 */
void hdPartsInOutLoop_c::inEnter() {
    startAnim(IN_OUT_LOOP_ANIM_IN);
    startAnim(IN_OUT_LOOP_ANIM_LOOP);
}

/* 026F7624 */
void hdPartsInOutLoop_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), IN_OUT_LOOP_ANIM_IN)) {
        changeState(&sInOutLoopLoop);
    }
}

/* 026F7674 */
void hdPartsInOutLoop_c::loopExecute() {
    if (*hdPartsView(mLoopSound.mText.get()) != 0) {
        mLoopSound.assureTermination();
        hdPartsInOutLoop_startSound(mLoopSound.mText, 1.0f, 1.0f);
    }
    if (mOutRequested) {
        changeState(&sInOutLoopOut);
    }
}

/* 026F76F0 */
void hdPartsInOutLoop_c::outEnter() {
    startAnim(IN_OUT_LOOP_ANIM_OUT);
}

/* 026F76F8 */
void hdPartsInOutLoop_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), IN_OUT_LOOP_ANIM_OUT)) {
        changeState(&sInOutLoopWait);
    }
}

/* 026F7748 */
void hdPartsInOutLoop_c::outExit() {
    mIsClosed = TRUE;
}

/* 026F7754 */
// hdPartsInOutLoop_c::hdPartsInOutLoop_c() (GHS: allocates when called with NULL)
hdPartsInOutLoop_c* hdPartsInOutLoop_c::construct(hdPartsInOutLoop_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdPartsInOutLoop_c*)operator_new(sizeof(hdPartsInOutLoop_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sInOutLoopVtable;
    i_this->mStateHolder.mHolderVtable = &sInOutLoopHolderVtable;
    i_this->mSecondaryVtable = &sInOutLoopSecondaryVtable;
    i_this->mIsActive = FALSE;
    i_this->mInRequested = FALSE;
    i_this->mOutRequested = FALSE;
    i_this->mIsClosed = FALSE;
    i_this->mLoopSound.mVtable = &sInOutLoopSafeStringVtable;
    i_this->mLoopSound.mText = sInOutLoopNullString;
    return i_this;
}

/* 026F77EC */
void hdPartsInOutLoop_c::setAnimFrame(u32 anim, u32 frame) {
    hdAnimSet_setFrame(animSet(), anim, sInOutLoopAnimSlots[anim], (f32)frame);
}

/* 026F7844 */
u32 hdPartsInOutLoop_c::init(void* resource, u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    if (!vLoad(resource, 0, arg1, arg2, desc)) {
        return FALSE;
    }
    setAnimFrame(IN_OUT_LOOP_ANIM_IN, 0);
    startState(&sInOutLoopWait);
    return TRUE;
}

/* 026F78F0 */
void hdPartsInOutLoop_c::finalize() {
    vDestroyLayout();
}

/* 026F7900 */
void hdPartsInOutLoop_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026F7908 */
void hdPartsInOutLoop_c::requestIn() {
    mIsActive = TRUE;
    mInRequested = TRUE;
}

/* 026F7918 */
void hdPartsInOutLoop_c::deactivate() {
    mIsActive = FALSE;
}

/* 026F7924 */
void hdPartsInOutLoop_c::requestOut() {
    if (mIsActive) {
        mOutRequested = TRUE;
    } else {
        mIsClosed = TRUE;
    }
}

/* 026F7948 */
void hdPartsInOutLoop_c::setLoopSound(hdUiSafeString* label) {
    mLoopSound.mText = label->mText;
}

/* 026F7954 */
u32 hdPartsInOutLoop_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sInOutLoopUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL) {
        return FALSE;
    }
    layout->vLoad(name, arg1, arg2, arg3, 3, 2, desc);
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

static hdTuHeaderStatics_c sInOutLoopHeaderStatics;
static hdTuHeaderDescriptors_c sInOutLoopHeaderDescriptors;
static hdTuHeaderZero_c sInOutLoopHeaderZero;
static hdTuPiPair_c sInOutLoopPiConstants;
/* 026F7A08 */
static void __sinit_hd_parts_in_out_loop_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sInOutLoopHeaderStatics), gabi::ea(&sInOutLoopHeaderDescriptors), gabi::ea(&sInOutLoopHeaderZero),
                                gabi::ea(&sInOutLoopPiConstants));
    hdParts_setSafeString(&sInOutLoopTypeName, &sInOutLoopSafeStringVtable, "InOutLoop");
    s32 number;
    if (sInOutLoopStateNumberReady) {
        number = sInOutLoopStateNumber;
    } else {
        number = 0;
        sInOutLoopStateNumberReady = 1;
    }
    sInOutLoopWait.setup(number + 1, "::StateID_Wait", &sInOutLoopStateIdVtable, 0x19, &sInOutLoopNoStateId);
    sInOutLoopIn.setup(number + 2, "::StateID_In", &sInOutLoopStateIdVtable, 0x1C, &sInOutLoopNoStateId);
    sInOutLoopLoop.setup(number + 3, "::StateID_Loop", &sInOutLoopStateIdVtable, 0x1F, &sInOutLoopNoStateId);
    sInOutLoopOut.setup(number + 4, "::StateID_Out", &sInOutLoopStateIdVtable, 0x22, &sInOutLoopNoStateId);
    sInOutLoopStateNumber = number + 4;
}

/* 026F7420 */
// the state number (copy of the previous TU's out-of-line StateID getter; listed with this unit)
static s32 hdPartsInOutLoop_getStateNumberPrev(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* 026F7CAC */
static s32 hdPartsInOutLoop_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sInOutLoopHeaderStatics 1049DB4C
data sInOutLoopHeaderDescriptors 101F719C
data sInOutLoopHeaderZero 1049DB58
data sInOutLoopPiConstants 10109FD8
data sInOutLoopVtable 1010A030 nocheck
data sInOutLoopHolderVtable 1010A158 nocheck
data sInOutLoopSecondaryVtable 1010A168 nocheck
data sInOutLoopSafeStringVtable 10109F14 nocheck
data sInOutLoopStateIdVtable 10109F80 nocheck
data sInOutLoopNoStateId 101FF32C nocheck
data sInOutLoopNullString 104A0CD8 nocheck
data sInOutLoopUiSystem 1018C404 nocheck
data sInOutLoopAnimSlots 10109F2C
data sInOutLoopTypeName 1049DC28 nocheck
data sInOutLoopWait 1049DB68 nocheck
data sInOutLoopIn 1049DB98 nocheck
data sInOutLoopLoop 1049DBC8 nocheck
data sInOutLoopOut 1049DBF8 nocheck
data sInOutLoopStateNumberReady 101FD8E4 nocheck
data sInOutLoopStateNumber 101FDD50 nocheck
str 10109FE0 "InOutLoop" in 026F7A08
str 10109FFC "::StateID_Wait" in 026F7A08
str 1010A00C "::StateID_In" in 026F7A08
str 1010A01C "::StateID_Loop" in 026F7A08
str 10109FEC "::StateID_Out" in 026F7A08
*/
