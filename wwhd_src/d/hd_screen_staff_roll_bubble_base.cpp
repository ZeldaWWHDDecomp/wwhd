/**
 * hd_screen_staff_roll_bubble_base.cpp
 * HD - the staff roll bubbles' base class ("StaffRollBubbleBase"): a bubble layout that waits, then rises
 * from below the screen with a swaying motion until it leaves the screen; states ::StateID_Wait / _Move
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_title_staff_roll). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Class and state names are HD strings;
 * all other names are ours.
 */

#include "d/hd_screen_staff_roll_bubble.h"

static hdPartsVtbl_c sBubbleBaseVtable;
static u32 sBubbleBaseHolderVtable;
static u32 sBubbleBaseSecondaryVtable;
static u32 sBubbleBaseMotionVtable;
static hdUiStringTable sBubbleBaseSafeStringVtable;
static u32 sBubbleBaseStateIdVtable;
static void* sBubbleBaseRandom;

static f32 sBubbleBaseConst150;
static f32 sBubbleBaseScreenHeight;    /* 720 */
static f32 sBubbleBaseScreenWidth;     /* 1280 */
static f32 sBubbleBaseTop;             /* a bubble above this has left the screen */
static f32 sBubbleBaseStartY;          /* the bubbles start below the screen */
static f32 sBubbleBaseRight;
static f32 sBubbleBaseLeft;
static hdUiSafeString sBubbleBaseClassName;
/* the start velocities (x, y) of the thirteen motion kinds; z is 0 */
static hdStaffRollBubbleXy_c sBubbleBaseKindVelocities[13] = {};
/* the motion kinds of the small bubbles (the velocities are set by the static initialiser) */
static hdStaffRollBubbleKind_c sBubbleBaseSmallKinds[13] = {};
static f32 sBubbleBaseRiseScale;               /* 5: the rise added for a smaller bubble */
static f32 sBubbleBaseSwayXRange[2] = {};      /* -1.5 .. 1.5 */
static f32 sBubbleBaseRiseRange[2] = {};       /* 1 .. 4 */
static f32 sBubbleBaseAccelRange[2] = {};      /* 0 .. 0.06 */
static f32 sBubbleBaseScaleRange[2] = {};      /* 0.01 .. 0.8 */
static hdStaffRollBubbleStateId_c sBubbleBaseWait;
static hdStaffRollBubbleStateId_c sBubbleBaseMove;
static u32 sBubbleBaseStateNumberReady;
static s32 sBubbleBaseStateNumber;

/* the motion kinds without their start velocity: acceleration, sway amplitude, sway step, sway angle */
struct hdStaffRollBubbleKindParams_c {
    f32 mAccel;
    f32 mSwayAmplitude;
    s32 mSwayStep;
    s32 mSwayAngle;
};
static const hdStaffRollBubbleKindParams_c lBubbleBaseKindParams[STAFF_ROLL_BUBBLE_KIND_COUNT] = {
    {0.0f, 15.0f, 2, 60},  {0.01f, 10.0f, 3, 80},  {0.0f, 15.0f, 2, 30},   {0.0f, 10.0f, 2, 45},
    {0.01f, 5.0f, 3, 60},  {0.01f, 10.0f, 2, 75},  {0.02f, 10.0f, 4, 90},  {0.01f, 20.0f, 3, 105},
    {0.02f, 0.0f, 3, 120}, {0.03f, 5.0f, 4, 135},  {0.0f, 10.0f, 4, 150},  {0.01f, 15.0f, 5, 165},
    {0.1f, 20.0f, 5, 180},
};

/* HD: a float copied by an lfs directly followed by its stfs keeps a signalling NaN bit for bit (the
 * recompiled original folds the pair); other float copies quiet it, like a be<f32> read */
static void hdBubbleBase_copyFloat(be<f32>* dst, be<f32>* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }

/* a random value in [lo, hi) */
static f32 hdBubbleBase_randomRange(be<f32>* range) {
    f32 lo = range[0];
    f32 width = range[1] - lo;
    u32 value = hdStaffRollBubble_random(sBubbleBaseRandom);
    return gabi::fmadds((f32)value * 2.3283064e-10f, width, lo);
}

/* 026E3248 */
// hdStaffRollBubbleBase_c::~hdStaffRollBubbleBase_c() (GHS deleting destructor)
void hdStaffRollBubbleBase_c::destroy(hdStaffRollBubbleBase_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mVtable = &sBubbleBaseVtable;
    i_this->mStateHolder.mHolderVtable = &sBubbleBaseHolderVtable;
    i_this->mSecondaryVtable = &sBubbleBaseSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026E32C0 */
void hdStaffRollBubbleMotion_c::reset() {
    mType = 0;
    mVelocity.x = 0.0f;
    mPos.y = 0.0f;
    mPos.z = 0.0f;
    mSway.x = 0.0f;
    mPos.x = 0.0f;
    mAccel = 0.0f;
    mSwayAmplitude = 0.0f;
    mSway.y = 0.0f;
    mSwayStep = 0;
    mSway.z = 0.0f;
    mSwayAngle = 0;
    mScale = 0.0f;
    mFlags = 0;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;
}

/* 026E3310 */
// one frame: the velocity grows by the acceleration, the position moves, the sway angle turns
void hdStaffRollBubbleMotion_c::update() {
    s32 angle = (mSwayAngle + mSwayStep) % 360;
    mVelocity.y = mVelocity.y + mAccel;
    mPos.z = mPos.z + mVelocity.z;
    mPos.x = mPos.x + mVelocity.x;
    mPos.y = mPos.y + mVelocity.y;
    mSwayAngle = angle;
    f64 sine = hdStaffRollBubble_sin((f32)angle * 0.017453292f);
    mSway.x = (f32)((f64)(f32)mSwayAmplitude * sine);
}

/* 026E33DC */
// whether the bubble has left the screen (above the top or beyond a side)
u32 hdStaffRollBubbleMotion_c::isOut() {
    if (sBubbleBaseTop < mPos.y) {
        return TRUE;
    }
    if (sBubbleBaseRight < mPos.x) {
        return TRUE;
    }
    if (sBubbleBaseLeft > mPos.x) {
        return TRUE;
    }
    return FALSE;
}

/* 026E3424 */
// smaller bubbles rise faster
void hdStaffRollBubbleMotion_c::addScaleRise() {
    mVelocity.y = gabi::fmadds(1.0f - mScale, sBubbleBaseRiseScale, mVelocity.y);
}

/* 026E344C */
void hdStaffRollBubbleMotion_c::set(u32 type, cXyz* pos, cXyz* velocity, s32 swayStep, u32 flags, f32 accel, f32 swayAmplitude, f32 scale) {
    mType = type;
    mPos.copy(*pos);
    mVelocity.copy(*velocity);
    mStartVelocity.copy(*velocity);
    mSwayAmplitude = swayAmplitude;
    mScale = scale;
    mFlags = flags;
    mSwayStep = swayStep;
    mAccel = accel;
    addScaleRise();
}

/* 026E34B0 */
// the motion parameters of `other` (its velocity becomes the start velocity; position of the sway and
// sway angle stay)
void hdStaffRollBubbleMotion_c::copy(hdStaffRollBubbleMotion_c* other) {
    mType = other->mType;
    mPos.copy(other->mPos);
    mVelocity.copy(other->mVelocity);
    mStartVelocity.copy(other->mVelocity);
    hdBubbleBase_copyFloat(&mAccel, &other->mAccel);
    hdBubbleBase_copyFloat(&mSwayAmplitude, &other->mSwayAmplitude);
    mSwayStep = other->mSwayStep;
    hdBubbleBase_copyFloat(&mScale, &other->mScale);
    mFlags = other->mFlags;
}

/* 026E352C */
// start at height sStartY with motion kind `kind` (0..12)
void hdStaffRollBubbleMotion_c::setKind(u32 type, u32 kind, u32 flags, f32 x, f32 scale) {
    mType = type;
    mPos.x = x;
    mPos.y = sBubbleBaseStartY;
    mScale = scale;
    mFlags = flags;
    hdStaffRollBubbleXy_c* velocity = &sBubbleBaseKindVelocities[kind];
    const hdStaffRollBubbleKindParams_c* params = &lBubbleBaseKindParams[kind];
    mVelocity.x = velocity->x;
    mVelocity.y = velocity->y;
    mVelocity.z = 0.0f;
    mAccel = params->mAccel;
    mSwayAmplitude = params->mSwayAmplitude;
    mSwayStep = params->mSwayStep;
    mSwayAngle = params->mSwayAngle;
}

/* 026E3944 */
void hdStaffRollBubbleMotion_c::getPos(cXyz* out) {
    f32 x = mPos.x + mSway.x;
    f32 y = mPos.y + mSway.y;
    f32 z = mPos.z + mSway.z;
    out->x = x;
    out->y = y;
    out->z = z;
}

/* 026E3980 */
f32 hdStaffRollBubbleMotion_c::getScale() {
    return mScale;
}

/* 026E3988 */
// back to the start velocity, at a random place in the lower part of the screen
void hdStaffRollBubbleMotion_c::respawn() {
    mVelocity.copy(mStartVelocity);
    addScaleRise();
    f32 halfLeft = sBubbleBaseLeft * 0.5f;
    f32 halfWidth = gabi::fmsubs(sBubbleBaseRight, 0.5f, halfLeft);
    u32 random = hdStaffRollBubble_random(sBubbleBaseRandom);
    f32 bottom = sBubbleBaseStartY - sBubbleBaseScreenHeight;
    f32 height = sBubbleBaseStartY - bottom;
    f32 x = gabi::fmadds((f32)random * 2.3283064e-10f, halfWidth, halfLeft);
    random = hdStaffRollBubble_random(sBubbleBaseRandom);
    f32 y = gabi::fmadds((f32)random * 2.3283064e-10f, height, bottom);
    mPos.x = x;
    mPos.y = y;
    mPos.z = 0.0f;
}

/* 026E3AF8 */
void hdStaffRollBubbleBase_c::moveEnter() {
    vExecuteChildren();
    vCalcLayout();
}

/* 026E3B44 */
// after the delay: move, place the root pane, and wait again once off screen
void hdStaffRollBubbleBase_c::moveExecute() {
    if (mDelay != 0) {
        mDelay = mDelay - 1;
        return;
    }
    mMotion.update();
    gabi::Local<cXyz> pos;
    mMotion.getPos(pos);
    hdStaffRollBubblePane_c* root = rootPane();
    root->mTranslate.copy(*pos);
    root->mFlags = root->mFlags | STAFF_ROLL_BUBBLE_PANE_DIRTY;
    if (mMotion.isOut()) {
        changeState((hdPartsStateId_c*)&sBubbleBaseWait);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 026E3C1C */
// hdStaffRollBubbleBase_c::hdStaffRollBubbleBase_c() (GHS: allocates when called with NULL)
hdStaffRollBubbleBase_c* hdStaffRollBubbleBase_c::construct(hdStaffRollBubbleBase_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdStaffRollBubbleBase_c*)operator_new(sizeof(hdStaffRollBubbleBase_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    hdStaffRollBubbleMotion_c* motion = &i_this->mMotion;
    motion->mVtable = &sBubbleBaseMotionVtable;
    motion->mType = 0;
    motion->mPos.set(0.0f, 0.0f, 0.0f);
    motion->mVelocity.set(0.0f, 0.0f, 0.0f);
    motion->mStartVelocity.set(0.0f, 0.0f, 0.0f);
    motion->mAccel = 0.0f;
    motion->mSway.set(0.0f, 0.0f, 0.0f);
    motion->mSwayAmplitude = 0.0f;
    motion->mSwayStep = 0;
    motion->mSwayAngle = 0;
    motion->mScale = 0.0f;
    motion->mFlags = 0;
    i_this->mSecondaryVtable = &sBubbleBaseSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sBubbleBaseHolderVtable;
    i_this->mVtable = &sBubbleBaseVtable;
    i_this->mDelay = 0;
    motion->reset();
    return i_this;
}

/* 026E3CF8 */
void hdStaffRollBubbleBase_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 026E3D4C */
void hdStaffRollBubbleBase_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026E3D54 */
// smaller bubbles are fainter: alpha 100 at scale 0.4 or less, 255 at scale 1
f32 hdStaffRollBubbleBase_c::getAlpha() {
    f32 scale = mMotion.mScale;
    if (scale < 0.4f) {
        scale = 0.4f;
    }
    u32 alpha = (u8)gabi::ftoi(gabi::fmadds(155.0f, (scale - 0.4f) / 0.6f, 100.0f));
    return (f32)alpha;
}

/* 026E3DCC */
f32 hdStaffRollBubbleBase_c::getRandomSwayX() {
    return hdBubbleBase_randomRange(sBubbleBaseSwayXRange);
}

/* 026E3E64 */
f32 hdStaffRollBubbleBase_c::getRandomRise() {
    return hdBubbleBase_randomRange(sBubbleBaseRiseRange);
}

/* 026E3EFC */
f32 hdStaffRollBubbleBase_c::getRandomAccel() {
    return hdBubbleBase_randomRange(sBubbleBaseAccelRange);
}

/* 026E3F94 */
f32 hdStaffRollBubbleBase_c::getRandomScale() {
    return hdBubbleBase_randomRange(sBubbleBaseScaleRange);
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 026E402C */
static void __sinit_hd_screen_staff_roll_bubble_base_cpp() {
    hdStaffRoll_initHeaderStatics(0x1049C2EC, 0x101F6C28, 0x1049C2C4, 0x101066B8, 0x1049C2E8);
    sBubbleBaseConst150 = 150.0f;
    sBubbleBaseScreenHeight = 720.0f;
    sBubbleBaseScreenWidth = 1280.0f;
    sBubbleBaseTop = 510.0f;
    sBubbleBaseStartY = -660.0f;
    sBubbleBaseRight = 790.0f;
    sBubbleBaseLeft = -790.0f;
    sBubbleBaseSmallKinds[0].mVelocity.set(0.0f, 1.0f, 0.0f);
    sBubbleBaseSmallKinds[1].mVelocity.set(0.0f, 1.0f, 0.0f);
    sBubbleBaseSmallKinds[2].mVelocity.set(0.0f, 1.0f, 0.0f);
    sBubbleBaseSmallKinds[3].mVelocity.set(0.0f, 1.0f, 0.0f);
    sBubbleBaseSmallKinds[4].mVelocity.set(-0.5f, 1.3f, 0.0f);
    sBubbleBaseSmallKinds[5].mVelocity.set(0.7f, 1.4f, 0.0f);
    sBubbleBaseSmallKinds[6].mVelocity.set(-0.0f, 1.5f, 0.0f);
    sBubbleBaseSmallKinds[7].mVelocity.set(0.5f, 1.6f, 0.0f);
    sBubbleBaseSmallKinds[8].mVelocity.set(-0.0f, 1.7f, 0.0f);
    sBubbleBaseSmallKinds[9].mVelocity.set(0.5f, 1.3f, 0.0f);
    sBubbleBaseSmallKinds[10].mVelocity.set(-0.7f, 1.4f, 0.0f);
    sBubbleBaseSmallKinds[11].mVelocity.set(0.0f, 1.5f, 0.0f);
    sBubbleBaseSmallKinds[12].mVelocity.set(-0.5f, 1.6f, 0.0f);
    sBubbleBaseKindVelocities[0].x = 0.0f;
    sBubbleBaseKindVelocities[0].y = 3.0f;
    sBubbleBaseKindVelocities[1].x = 0.1f;
    sBubbleBaseKindVelocities[1].y = 3.2f;
    sBubbleBaseKindVelocities[2].x = 0.2f;
    sBubbleBaseKindVelocities[2].y = 2.0f;
    sBubbleBaseKindVelocities[3].x = 0.1f;
    sBubbleBaseKindVelocities[3].y = 2.1f;
    sBubbleBaseKindVelocities[4].x = -0.5f;
    sBubbleBaseKindVelocities[4].y = 3.1f;
    sBubbleBaseKindVelocities[5].x = 0.7f;
    sBubbleBaseKindVelocities[5].y = 3.3f;
    sBubbleBaseKindVelocities[6].x = 0.0f;
    sBubbleBaseKindVelocities[6].y = 3.6f;
    sBubbleBaseKindVelocities[7].x = 0.5f;
    sBubbleBaseKindVelocities[7].y = 1.6f;
    sBubbleBaseKindVelocities[8].x = 0.0f;
    sBubbleBaseKindVelocities[8].y = 1.7f;
    sBubbleBaseKindVelocities[9].x = 0.5f;
    sBubbleBaseKindVelocities[9].y = 1.3f;
    sBubbleBaseKindVelocities[10].x = -0.7f;
    sBubbleBaseKindVelocities[10].y = 1.4f;
    sBubbleBaseKindVelocities[11].x = 0.0f;
    sBubbleBaseKindVelocities[11].y = 1.5f;
    sBubbleBaseKindVelocities[12].x = -0.5f;
    sBubbleBaseKindVelocities[12].y = 1.6f;
    hdParts_setSafeString(&sBubbleBaseClassName, &sBubbleBaseSafeStringVtable, "StaffRollBubbleBase");
    s32 number;
    if (sBubbleBaseStateNumberReady) {
        number = sBubbleBaseStateNumber;
    } else {
        number = 0;
        sBubbleBaseStateNumberReady = 1;
    }
    sBubbleBaseWait.setup(number + 1, "::StateID_Wait", &sBubbleBaseStateIdVtable, 0x026E4588, 0x026E458C, 0x026E4590, 0x026E4594);
    sBubbleBaseMove.setup(number + 2, "::StateID_Move", &sBubbleBaseStateIdVtable, 0x026E3AF8, 0x026E3B44, 0x026E459C, 0x026E45A0);
    sBubbleBaseStateNumber = number + 2;
}

/* 026E4594 */
u32 hdStaffRollBubbleBase_c::waitFinal() {
    return 0;
}

/* 026E45A0 */
u32 hdStaffRollBubbleBase_c::moveFinal() {
    return 0;
}

/* 026E45A8 */
f32 hdStaffRollBubbleBase_c::getStartY() {
    return sBubbleBaseStartY;
}

/* 026E45B4 */
static s32 hdStaffRollBubbleBase_getStateNumber(hdStaffRollBubbleStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sBubbleBaseVtable 1010675C nocheck
data sBubbleBaseHolderVtable 10106824 nocheck
data sBubbleBaseSecondaryVtable 10106834 nocheck
data sBubbleBaseMotionVtable 101065D8 nocheck
data sBubbleBaseSafeStringVtable 101065A0 nocheck
data sBubbleBaseStateIdVtable 10106638 nocheck
data sBubbleBaseRandom 101F8B60 nocheck
data sBubbleBaseConst150 1049C2CC nocheck
data sBubbleBaseScreenHeight 1049C2D0 nocheck
data sBubbleBaseScreenWidth 1049C2D4 nocheck
data sBubbleBaseTop 1049C2D8 nocheck
data sBubbleBaseStartY 1049C2DC nocheck
data sBubbleBaseRight 1049C2E0 nocheck
data sBubbleBaseLeft 1049C2E4 nocheck
data sBubbleBaseClassName 1049C2FC nocheck
data sBubbleBaseKindVelocities 1049C304 nocheck
data sBubbleBaseSmallKinds 101F6D38 nocheck
data sBubbleBaseRiseScale 101F6D34 nocheck
data sBubbleBaseSwayXRange 101F6D14 nocheck
data sBubbleBaseRiseRange 101F6D1C nocheck
data sBubbleBaseAccelRange 101F6D24 nocheck
data sBubbleBaseScaleRange 101F6D2C nocheck
data sBubbleBaseWait 1049C36C nocheck
data sBubbleBaseMove 1049C398 nocheck
data sBubbleBaseStateNumberReady 101FD8E4 nocheck
data sBubbleBaseStateNumber 101FDD50 nocheck
str 10106724 "StaffRollBubbleBase" in 026E402C
str 10106738 "::StateID_Wait" in 026E402C
str 10106748 "::StateID_Move" in 026E402C
*/
