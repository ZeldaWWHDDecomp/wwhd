/**
 * d_camera_util.cpp (WWHD)
 * Follow camera dCamera_c: pad and monitor state, focus line, force lock-on check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_util.h"
#include "port_hooks.h"

/* HD pad accessors (out of line, by pad id) */
static inline f32 CPad_GET_STICK_POS_X(s32 pad) { return cameraUtilCPad_GET_STICK_POS_X(pad); }
static inline f32 CPad_GET_STICK_POS_Y(s32 pad) { return cameraUtilCPad_GET_STICK_POS_Y(pad); }
static inline f32 CPad_GET_STICK_VALUE(s32 pad) { return cameraUtilCPad_GET_STICK_VALUE(pad); }
static inline f32 CPad_GET_SUBSTICK_POS_X(s32 pad) { return cameraUtilCPad_GET_SUBSTICK_POS_X(pad); }
static inline f32 CPad_GET_SUBSTICK_POS_Y(s32 pad) { return cameraUtilCPad_GET_SUBSTICK_POS_Y(pad); }
static inline f32 CPad_GET_SUBSTICK_VALUE(s32 pad) { return cameraUtilCPad_GET_SUBSTICK_VALUE(pad); }
static inline f32 CPad_GET_ANALOG_L(s32 pad) { return cameraUtilCPad_GET_ANALOG_L(pad); }
static inline f32 CPad_GET_ANALOG_R(s32 pad) { return cameraUtilCPad_GET_ANALOG_R(pad); }
static inline u8 mDoCPd_L_LOCK_BUTTON(s32 pad) { return (u8)cameraUtilmDoCPd_L_LOCK_BUTTON(pad); }
static inline u8 mDoCPd_L_LOCK_TRIGGER(s32 pad) { return (u8)cameraUtilmDoCPd_L_LOCK_TRIGGER(pad); }
static inline u8 mDoCPd_R_LOCK_BUTTON(s32 pad) { return (u8)cameraUtilmDoCPd_R_LOCK_BUTTON(pad); }
static inline u8 mDoCPd_R_LOCK_TRIGGER(s32 pad) { return (u8)cameraUtilmDoCPd_R_LOCK_TRIGGER(pad); }
static inline bool CPad_CHECK_HOLD_X(s32 pad) { return cameraUtilCPad_CHECK_HOLD_X(pad) != 0; }
static inline bool CPad_CHECK_TRIG_X(s32 pad) { return cameraUtilCPad_CHECK_TRIG_X(pad) != 0; }
static inline bool CPad_CHECK_HOLD_Y(s32 pad) { return cameraUtilCPad_CHECK_HOLD_Y(pad) != 0; }
static inline bool CPad_CHECK_TRIG_Y(s32 pad) { return cameraUtilCPad_CHECK_TRIG_Y(pad) != 0; }
static inline bool CPad_CHECK_HOLD_Z(s32 pad) { return cameraUtilCPad_CHECK_HOLD_Z(pad) != 0; }
static inline bool CPad_CHECK_TRIG_Z(s32 pad) { return cameraUtilCPad_CHECK_TRIG_Z(pad) != 0; }
static inline bool CPad_CHECK_HOLD_B(s32 pad) { return cameraUtilCPad_CHECK_HOLD_B(pad) != 0; }
static inline bool CPad_CHECK_TRIG_B(s32 pad) { return cameraUtilCPad_CHECK_TRIG_B(pad) != 0; }

/* blur matrix: mDoMtx_stack_c (at 1048D0CC) = T(pos) S(scale) Rx Ry Rz T(-pos), then onBlure(mtx) */
static void blureMtx(dCamera_c* c) {
    auto* stack = gabi::at<CameraUtilMatrixHD>(0x1048D0CC);
    cameraUtilMatrixTrans(stack, (f32)c->mBlurePosition.x, (f32)c->mBlurePosition.y, (f32)c->mBlurePosition.z); /* transS */
    mDoMtx_stack_scaleM(c->mBlureScale.x, c->mBlureScale.y, c->mBlureScale.z);
    cameraUtilMatrixXrot(stack, (s16)c->mBlureRotation.x); /* XrotM */
    cameraUtilMatrixYrot(stack, (s16)c->mBlureRotation.y); /* YrotM */
    cameraUtilMatrixZrot(stack, (s16)c->mBlureRotation.z); /* ZrotM */
    f32 x = c->mBlurePosition.x, y = c->mBlurePosition.y, z = c->mBlurePosition.z;
    mDoMtx_stack_transM(-x, -y, -z);
    cameraUtilOnBlureMatrix(stack); /* mDoGph_gInf_c::onBlure(mDoMtx_stack_c::get()) */
}
static inline void setBlureRate(s32 r) { gabi::at<CameraUtilGraphicsHD>(0x101F4825)->mBlureRate = (u8)r; } /* mDoGph_gInf_c::setBlureRate */

/* 025006D8 */
/* HD: the C stick X is negated; Y is read twice (as on GameCube) */
void dCamera_c::initPad() {
    mStickMainPosXLast = CPad_GET_STICK_POS_X(mPadId);
    mStickMainPosYLast = CPad_GET_STICK_POS_Y(mPadId);
    mStickMainValueLast = CPad_GET_STICK_VALUE(mPadId);
    mStickMainPosYDelta = 0.0f;
    mStickMainValueDelta = 0.0f;
    mStickMainPosXDelta = 0.0f;
    mStickCPosXLast = -CPad_GET_SUBSTICK_POS_X(mPadId);
    mStickCPosYLast = PORT_CAMERA_F(0x0250074C, this, CPad_GET_SUBSTICK_POS_Y(mPadId)); /* the port's camera controls (port_hooks.h) */
    f32 cv = CPad_GET_SUBSTICK_VALUE(mPadId);
    mStickCPosYDelta = 0.0f;
    mCStickUpLatch = 0;
    mCStickYHoldCount = 0;
    mStickCPosXDelta = 0.0f;
    mStickCValueDelta = 0.0f;
    mStickCValueLast = cv;
    mCStickYState = 0;
    f32 l = CPad_GET_ANALOG_L(mPadId);
    mTriggerLeftDelta = 0.0f;
    mLockLActive = 0;
    mTrigLockL = 0;
    mLockLJustActivated = 0;
    mHoldLockL = 0;
    mTriggerLeftLast = l;
    f32 r = CPad_GET_ANALOG_R(mPadId);
    mLockRActive = 0;
    mTriggerRightDelta = 0.0f;
    mLockRJustActivated = 0;
    mHoldLockR = 0;
    mTrigLockR = 0;
    mTriggerRightLast = r;
    mHoldX = CPad_CHECK_HOLD_X(mPadId);
    mTrigX = CPad_CHECK_TRIG_X(mPadId);
    mHoldY = CPad_CHECK_HOLD_Y(mPadId);
    mTrigY = CPad_CHECK_TRIG_Y(mPadId);
    mHoldY = CPad_CHECK_HOLD_Y(mPadId);
    mTrigY = CPad_CHECK_TRIG_Y(mPadId);
    mHoldZ = CPad_CHECK_HOLD_Z(mPadId);
    bool trigZ = CPad_CHECK_TRIG_Z(mPadId);
    m1AE = 0;
    mTrigZ = trigZ;
}

/* 024F9BF4 */
/* HD: the C stick X is negated; the trigger-edge bytes are mLockLJustActivated = mLockLActive ^ 1 */
void dCamera_c::updatePad() {
    f32 x, y, v;
    gabi::Local<cSAngle_l> unused;
    if (chkFlag(CAMERA_STICK_USELESS)) {
        cameraUtilAngleShort(unused.get(), CPad_GET_STICK_ANGLE(mPadId)); /* cSAngle unused(stick angle) */
        x = 0.0f;
        y = 0.0f;
        v = 0.0f;
    } else {
        x = CPad_GET_STICK_POS_X(mPadId);
        y = CPad_GET_STICK_POS_Y(mPadId);
        v = CPad_GET_STICK_VALUE(mPadId);
        cameraUtilAngleShort(unused.get(), CPad_GET_STICK_ANGLE(mPadId));
    }
    f32 lastV = mStickMainValueLast;
    mStickMainValueLast = v;
    f32 dv = v - lastV;
    u32 flags = mEventFlags;
    f32 lastX = mStickMainPosXLast;
    mStickMainPosXLast = x;
    f32 dx = x - lastX;
    f32 lastY = mStickMainPosYLast;
    mStickMainPosYLast = y;
    f32 dy = y - lastY;
    mStickMainPosXDelta = dx;
    mStickMainValueDelta = dv;
    mStickMainPosYDelta = dy;
    if (flags & CAMERA_CSTICK_USELESS) {
        x = 0.0f;
        y = 0.0f;
        v = 0.0f;
    } else {
        x = -CPad_GET_SUBSTICK_POS_X(mPadId);
        y = PORT_CAMERA_F(0x024F9DA4, this, CPad_GET_SUBSTICK_POS_Y(mPadId)); /* the port's camera controls (port_hooks.h) */
        v = CPad_GET_SUBSTICK_VALUE(mPadId);
    }
    f32 cy = mStickCPosYLast;
    f32 cv = mStickCValueLast;
    f32 dcy = y - cy;
    f32 cx = mStickCPosXLast;
    f32 dcv = v - cv;
    f32 dcx = x - cx;
    mStickCPosXLast = x;
    mStickCPosYLast = y;
    mStickCPosXDelta = dcx;
    mStickCValueDelta = dcv;
    mStickCPosYDelta = dcy;
    mStickCValueLast = v;

    f32 l = CPad_GET_ANALOG_L(mPadId);
    mTriggerLeftDelta = mTriggerLeftLast - l;
    mTriggerLeftLast = l;
    mHoldLockL = mDoCPd_L_LOCK_BUTTON(mPadId);
    mTrigLockL = mDoCPd_L_LOCK_TRIGGER(mPadId);
    if (mTriggerLeftLast > cameraUtilSetup(this)->ManualEndVal() /* mCamSetup.ManualEndVal() */) {
        mLockLJustActivated = mLockLActive ^ 1;
        mLockLActive = 1;
    } else {
        mLockLJustActivated = 0;
        mLockLActive = 0;
    }
    f32 r = CPad_GET_ANALOG_R(mPadId);
    f32 lastR = mTriggerRightLast;
    mTriggerRightLast = r;
    mTriggerRightDelta = lastR - r;
    mHoldLockR = mDoCPd_R_LOCK_BUTTON(mPadId);
    mTrigLockR = mDoCPd_R_LOCK_TRIGGER(mPadId);
    if (mTriggerRightLast > cameraUtilSetup(this)->ManualEndVal()) {
        mLockRJustActivated = mLockRActive ^ 1;
        mLockRActive = 1;
    } else {
        mLockRJustActivated = 0;
        mLockRActive = 0;
    }
    mHoldX = CPad_CHECK_HOLD_X(mPadId);
    mTrigX = CPad_CHECK_TRIG_X(mPadId);
    mHoldY = CPad_CHECK_HOLD_Y(mPadId);
    mTrigY = CPad_CHECK_TRIG_Y(mPadId);
    mHoldZ = CPad_CHECK_HOLD_B(mPadId);
    mTrigZ = CPad_CHECK_TRIG_B(mPadId);
    m1AE = 0;
}

/* 02500620 */
void dCamera_c::initMonitor() {
    if (mpPlayerActor.get() != nullptr) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        mMonitor.mPos.copy(*pos);
    } else {
        mMonitor.mPos.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
    }
    mMonitor.field_0x0C.x = 0.0f;
    mMonitor.field_0x0C.y = 0.0f;
    mMonitor.field_0x0C.z = 0.0f;
    mMonitor.field_0x18 = 0;
    mMonitor.field_0x1C = 0.0f;
}

/* 024F9A48 */
void dCamera_c::updateMonitor() {
    if (mpPlayerActor.get() == nullptr) return;
    gabi::Local<cXyz> playerPos;
    positionOf(playerPos, mpPlayerActor.get());
    if (mFollowMoveBg != 0) {
        /* dComIfG_Bgsp()->MoveBgMatrixCrrPos(mBG.m5C.m04 (poly info), TRUE, &mMonitor.mPos, NULL, NULL) */
        cameraUtilMoveBgMatrixCrrPos(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mBG.mCameraGround.mGndChk), TRUE, &mMonitor.mPos, nullptr, nullptr);
    }
    f32 dist = cameraUtilHorizontalDistance(playerPos.get(), &mMonitor.mPos); /* dCamMath::xyzHorizontalDistance */
    f32 avg = mMonitor.field_0x0C.y;
    f32 last = mMonitor.field_0x0C.x;
    mMonitor.field_0x0C.z = dist - last;
    mMonitor.field_0x0C.y = gabi::fmadds(dist - avg, 0.075f, avg);
    mMonitor.field_0x0C.x = dist;
    mMonitor.mPos.copy(*playerPos);
    if (mManualInactive == 0 && cameraUtilButtonHold(0) == 0 /* no button held on pad 0 */ &&
        mStickMainValueLast < 0.05f && mStickCValueLast < 0.05f) {
        mMonitor.field_0x18 = mMonitor.field_0x18 + 1;
    } else {
        mMonitor.field_0x18 = 0;
    }
    mMonitor.field_0x1C = mDirection.mRadius - mMonitor.field_0x1C;
}

/* 024FA058 */
/* HD: returns false (no forced lock-on) when no actor id is set, as GameCube does */
bool dCamera_c::checkForceLockTarget() {
    if (mLockOnActorId == fpcM_ERROR_PROCESS_ID_e) return false;
    fopAc_ac_c* actor = GetForceLockOnActor();
    mpLockonActor = actor;
    if (actor == nullptr) return false;
    dAttention_c* attn = dComIfGp_getAttention();
    if (cameraUtilLockonTruth(attn) /* LockonTruth */ || (reinterpret_cast<CameraUtilAttentionHD*>(attn)->mFlags & ATTENTION_LOCKON)) {
        return false; /* attn.Lockon() */
    }
    if ((f32)(s32)mForceLockTimer > (f32)cameraUtilSetup(this)->ForceLockOffTimer() /* mCamSetup.ForceLockOffTimer() */) return false;
    gabi::Local<cXyz> lockPos;
    gabi::Local<cXyz> playerPos;
    gabi::Local<cXyz> diff;
    positionOf(lockPos, mpLockonActor.get());
    positionOf(playerPos, mpPlayerActor.get());
    cXyz_mi(lockPos, diff, playerPos);
    f32 d = std_sqrtf(PSVECSquareMag(diff));
    if (d > cameraUtilSetup(this)->ForceLockOffDist() /* mCamSetup.ForceLockOffDist() */) return false;
    return true;
}

/* 024FFC18 */
bool dCamera_c::Draw() {
    reinterpret_cast<camera_util::dCamForcusLine*>(&mForcusLine)->Draw();
    return true;
}

/* 024FC108 */
/* HD: the blur fades in over the first 5 frames (rate 255 * alpha - (5 - timer) * 25),
 * the blur position is the player's projected eye in 1280x720; mShakePattern/mShakeCyclePattern patterns are 5 bytes */
f32 dCamera_c::shakeCamera() {
    f32 res = 0.0f;
    s32 pos = mShakeFrame;
    if (pos < mShakeLength) {
        s32 idx = pos >> 3;
        auto* pattern = reinterpret_cast<CameraUtilShakePatternHD*>(&mShakeCyclePattern[1]);
        u32 hi = pattern->mShakeCyclePattern[idx]; /* mShakeCyclePattern[idx] */
        u32 lo = pattern->mShakeCyclePattern[idx + 1];
        u32 bits = (hi << 8) | lo;
        s32 mask = 1 << (15 - (pos & 7));
        f32 amp = 1.0f;
        for (int i = 0; i < 4; i++) {
            if ((u32)mask & bits) {
                res = gabi::fmadds((*gabi::at<be<f32>[4]>(0x1004A818))[i] /* wave[i] */, amp, res);
            } else {
                amp = amp * 0.43f;
            }
            mask >>= 1;
        }
        mShakeFrame = pos + 1;
        f32 r = cM_rndFX(0.05f);
        res = gabi::fmuls_ppc(res, r + 0.95f);
        if (mShakeFrame & 1) res = -res;
        gabi::Local<cXyz> v, out;
        f32 x = mShakeDirection.x;
        reinterpret_cast<CameraUtilPointWordsHD*>(v.get())->z = reinterpret_cast<CameraUtilPointWordsHD*>(&mShakeDirection)->z;
        reinterpret_cast<CameraUtilPointWordsHD*>(v.get())->y = reinterpret_cast<CameraUtilPointWordsHD*>(&mShakeDirection)->y;
        v->x = x + cM_rndFX(0.045f);
        f32 rz = cM_rndFX(0.045f);
        v->z = v->z + rz;
        cXyz_ml(v, out, res);
        v->copy(*out);
        u32 flags = mShakeFlags;
        if (flags & SHAKE_POSITION) {
            mEyeShake.copy(*v);
            mCenterShake.copy(*v);
            flags = mShakeFlags;
        }
        if (flags & SHAKE_FOVY) {
            f32 rr = cM_rndFX(0.12f);
            mFovYShake = res * rr;
            flags = mShakeFlags;
        }
        if (flags & SHAKE_BANK) {
            gabi::Local<cSAngle_l> a;
            f32 rr = cM_rndFX(0.15f);
            cSAngle_l* b = cameraUtilAngleDegrees(a.get(), res * rr);
            flags = mShakeFlags;
            mBankShake = (s16)*b;
        }
        if (flags & SHAKE_BLURE_RATE) {
            setBlureRate(gabi::ftoi(30.0f * res));
            cameraUtilOnBlure(); /* mDoGph_gInf_c::onBlure() */
            mBlureTimer = 0;
        } else if (flags & SHAKE_BLURE_MATRIX) {
            if (mBlurePositionType == 0) {
                gabi::Local<cXyz> eye, proj;
                eyePos(eye, mpPlayerActor.get());
                cameraUtilProject(eye.get(), proj.get()); /* mDoLib_project */
                f32 px = proj->x + 640.0f;
                f32 py = 360.0f - proj->y;
                mBlurePosition.x = std::fabs(px / 1280.0f);
                mBlurePosition.y = std::fabs(py / 720.0f);
                mBlurePosition.z = 0.0f;
            }
            s32 timer = mBlureTimer;
            if (timer > 0) {
                mBlureTimer = timer - 1;
            } else if (mBlureType == 1) {
                return res;
            }
            blureMtx(this);
            s32 rate = gabi::ftoi(255.0f * mBlureAlpha);
            s32 t = mBlureTimer;
            s32 k = t > 5 ? 0 : 5 - t;
            rate -= k * 25;
            if (rate < 0) rate = 0;
            mDoMtx_stack_scaleM(mBlureScale.x, mBlureScale.y, mBlureScale.z);
            setBlureRate(rate);
        }
    } else {
        gabi::Local<cXyz> t;
        gabi::Local<cSAngle_l> a;
        cXyz_ml(&mCenterShake, t, 0.1f);
        cameraUtilVectorSubtract(&mCenterShake, t.get(), &mCenterShake); /* PSVECSubtract */
        cXyz_ml(&mEyeShake, t, 0.1f);
        cameraUtilVectorSubtract(&mEyeShake, t.get(), &mEyeShake);
        f32 fv = mFovYShake;
        mFovYShake = gabi::fnmsubs(fv, 0.1f, fv);
        cameraUtilAngleMultiply(&mBankShake, a.get(), 0.1f); /* mBankShake * 0.1 */
        cameraUtilAngleSubtractAssign(&mBankShake, a.get());       /* mBankShake -= */
        s32 timer = mBlureTimer;
        if (timer <= 0) {
            gabi::at<CameraUtilGraphicsHD>(0x101F4825)->mBlure = 0; /* mDoGph_gInf_c::offBlure() */
            mShakeFlags = mShakeFlags & ~SHAKE_BLURE_MATRIX;
            mBlureTimer = 0;
        } else {
            if (mBlureType == 1) {
                blureMtx(this);
                s32 rate = gabi::ftoi(230.0f * mBlureAlpha);
                mDoMtx_stack_scaleM(mBlureScale.x, mBlureScale.y, mBlureScale.z);
                setBlureRate(rate);
                timer = mBlureTimer;
            }
            if (gabi::ftoi(230.0f * mBlureAlpha) > timer && mBlureType == 0) {
                setBlureRate(timer);
                timer = mBlureTimer;
            }
            if (mBlurePositionType == 0) {
                s32 rate = gabi::ftoi(255.0f * mBlureAlpha);
                s32 k = timer > 5 ? 0 : 5 - timer;
                rate -= k * 25;
                if (rate < 0) rate = 0;
                setBlureRate(rate);
                timer = mBlureTimer;
            }
            mBlureTimer = timer - 1;
        }
    }
    return res;
}

/* 02500880 */
void camera_util::dCamForcusLine::Init() {
    mbOn = 0;
    mbNoReseed = 1;
    mCenterPos.x = 320.0f;
    mCenterPos.y = 240.0f;
    mCenterPos.z = 0.0f;
    mColor.r = 0xFF;
    mColor.g = 0xFF;
    mColor.b = 0xFF;
    mColor.a = 0x60;
    mRndSeed0 = 100;
    mRndSeed1 = 100;
    mRndSeed2 = 100;
    mLineNumMin = 0x50;
    mLineNumRange = 0;
    mLineWidthMin = 4;
    mLineWidthRange = 4;
    mLineLengthMin = 180.0f;
    mLineLengthRange = 0.0f;
    mStartRadiusMin = 180.0f;
    mStartRadiusRange = 60.0f;
}

/* 024FFB94 */
void camera_util::dCamForcusLine::Draw() {
    if (mbOn == 0) return;
    if (mbNoReseed == 0) {
        // HD: initRnd receives the embedded effect-line data at +4; update receives its original base.
        /* mEffectLine.initRnd(mRndSeed0, mRndSeed1, mRndSeed2) */
        cameraUtilEffectLineInit(&mEffectLine[4], (s32)mRndSeed0, (s32)mRndSeed1, (s32)mRndSeed2);
    }
    /* mEffectLine.update(mCenterPos, mColor, mLineNumMin, mLineNumRange, mLineWidthMin, mLineWidthRange, mStartRadiusMin, mStartRadiusRange, mLineLengthMin, mLineLengthRange) */
    cameraUtilEffectLineUpdate(this, &mCenterPos, &mColor, (u16)mLineNumMin, (u16)mLineNumRange, (u16)mLineWidthMin,
               (u16)mLineWidthRange, (f32)mStartRadiusMin, (f32)mStartRadiusRange, (f32)mLineLengthMin, (f32)mLineLengthRange);
}

// clang-format off
/* WWHD glue
ext 0200796C f32 cameraUtilCPad_GET_STICK_POS_X(s32)
ext 02007990 f32 cameraUtilCPad_GET_STICK_POS_Y(s32)
ext 020079B4 f32 cameraUtilCPad_GET_STICK_VALUE(s32)
ext 02007AFC f32 cameraUtilCPad_GET_SUBSTICK_POS_X(s32)
ext 02007B20 f32 cameraUtilCPad_GET_SUBSTICK_POS_Y(s32)
ext 02007B44 f32 cameraUtilCPad_GET_SUBSTICK_VALUE(s32)
ext 02007BFC f32 cameraUtilCPad_GET_ANALOG_L(s32)
ext 02007C50 f32 cameraUtilCPad_GET_ANALOG_R(s32)
ext 02007CA4 u32 cameraUtilmDoCPd_L_LOCK_BUTTON(s32)
ext 02007D14 u32 cameraUtilmDoCPd_L_LOCK_TRIGGER(s32)
ext 02007CDC u32 cameraUtilmDoCPd_R_LOCK_BUTTON(s32)
ext 02007D4C u32 cameraUtilmDoCPd_R_LOCK_TRIGGER(s32)
ext 0200770C u32 cameraUtilCPad_CHECK_HOLD_X(s32)
ext 020078E8 u32 cameraUtilCPad_CHECK_TRIG_X(s32)
ext 02007738 u32 cameraUtilCPad_CHECK_HOLD_Y(s32)
ext 02007914 u32 cameraUtilCPad_CHECK_TRIG_Y(s32)
ext 02007638 u32 cameraUtilCPad_CHECK_HOLD_Z(s32)
ext 02007814 u32 cameraUtilCPad_CHECK_TRIG_Z(s32)
ext 020076E0 u32 cameraUtilCPad_CHECK_HOLD_B(s32)
ext 020078BC u32 cameraUtilCPad_CHECK_TRIG_B(s32)
ext 024EFB08 void cameraUtilMoveBgMatrixCrrPos(dBgS*,cBgS_PolyInfo*,BOOL,cXyz*,csXyz*,csXyz*)
ext 024F7AC4 f32 cameraUtilHorizontalDistance(const cXyz*,const cXyz*)
ext 020075C0 u32 cameraUtilButtonHold(s32)
ext 0200658C cSAngle_l* cameraUtilAngleShort(cSAngle_l*,s32)
ext 024EDFCC bool cameraUtilLockonTruth(dAttention_c*)
ext 0252CD10 void cameraUtilEffectLineInit(void*,s32,s32,s32)
ext 0252CDF0 void cameraUtilEffectLineUpdate(void*,const cXyz*,const GXColor*,u32,u32,u32,u32,f32,f32,f32,f32)
ext 028E93CC void cameraUtilMatrixTrans(CameraUtilMatrixHD*,f32,f32,f32)
ext 025F1BF4 void cameraUtilMatrixXrot(CameraUtilMatrixHD*,s32)
ext 025F1C28 void cameraUtilMatrixYrot(CameraUtilMatrixHD*,s32)
ext 025F1C5C void cameraUtilMatrixZrot(CameraUtilMatrixHD*,s32)
ext 025F0634 void cameraUtilOnBlureMatrix(CameraUtilMatrixHD*)
ext 020066C0 cSAngle_l* cameraUtilAngleDegrees(cSAngle_l*,f32)
ext 025F064C void cameraUtilOnBlure()
ext 025F0EA4 void cameraUtilProject(const cXyz*,cXyz*)
ext 028E8DAC void cameraUtilVectorSubtract(const cXyz*,const cXyz*,cXyz*)
ext 0200693C void cameraUtilAngleMultiply(const cSAngle_l*,cSAngle_l*,f32)
ext 020068E0 void cameraUtilAngleSubtractAssign(cSAngle_l*,const cSAngle_l*)
*/
