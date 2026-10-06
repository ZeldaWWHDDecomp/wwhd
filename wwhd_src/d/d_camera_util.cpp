/**
 * d_camera_util.cpp (WWHD)
 * Follow camera dCamera_c: pad and monitor state, focus line, force lock-on check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"

/* HD pad accessors (out of line, by pad id) */
static inline f32 CPad_GET_STICK_POS_X(s32 pad) { return gabi::call<f32>(0x0200796C, pad); }
static inline f32 CPad_GET_STICK_POS_Y(s32 pad) { return gabi::call<f32>(0x02007990, pad); }
static inline f32 CPad_GET_STICK_VALUE(s32 pad) { return gabi::call<f32>(0x020079B4, pad); }
static inline f32 CPad_GET_SUBSTICK_POS_X(s32 pad) { return gabi::call<f32>(0x02007AFC, pad); }
static inline f32 CPad_GET_SUBSTICK_POS_Y(s32 pad) { return gabi::call<f32>(0x02007B20, pad); }
static inline f32 CPad_GET_SUBSTICK_VALUE(s32 pad) { return gabi::call<f32>(0x02007B44, pad); }
static inline f32 CPad_GET_ANALOG_L(s32 pad) { return gabi::call<f32>(0x02007BFC, pad); }
static inline f32 CPad_GET_ANALOG_R(s32 pad) { return gabi::call<f32>(0x02007C50, pad); }
static inline u8 mDoCPd_L_LOCK_BUTTON(s32 pad) { return (u8)gabi::call<u32>(0x02007CA4, pad); }
static inline u8 mDoCPd_L_LOCK_TRIGGER(s32 pad) { return (u8)gabi::call<u32>(0x02007D14, pad); }
static inline u8 mDoCPd_R_LOCK_BUTTON(s32 pad) { return (u8)gabi::call<u32>(0x02007CDC, pad); }
static inline u8 mDoCPd_R_LOCK_TRIGGER(s32 pad) { return (u8)gabi::call<u32>(0x02007D4C, pad); }
static inline bool CPad_CHECK_HOLD_X(s32 pad) { return gabi::call<u32>(0x0200770C, pad) != 0; }
static inline bool CPad_CHECK_TRIG_X(s32 pad) { return gabi::call<u32>(0x020078E8, pad) != 0; }
static inline bool CPad_CHECK_HOLD_Y(s32 pad) { return gabi::call<u32>(0x02007738, pad) != 0; }
static inline bool CPad_CHECK_TRIG_Y(s32 pad) { return gabi::call<u32>(0x02007914, pad) != 0; }
static inline bool CPad_CHECK_HOLD_Z(s32 pad) { return gabi::call<u32>(0x02007638, pad) != 0; }
static inline bool CPad_CHECK_TRIG_Z(s32 pad) { return gabi::call<u32>(0x02007814, pad) != 0; }
static inline bool CPad_CHECK_HOLD_B(s32 pad) { return gabi::call<u32>(0x020076E0, pad) != 0; }
static inline bool CPad_CHECK_TRIG_B(s32 pad) { return gabi::call<u32>(0x020078BC, pad) != 0; }

/* 024F9A48 */
void dCamera_c::updateMonitor() {
    WWHD_FUNC(0x024F9A48, void, this);
    if (mpPlayerActor.get() == nullptr) return;
    gabi::Local<cXyz> playerPos;
    positionOf(playerPos, mpPlayerActor.get());
    if (m31D != 0) {
        /* dComIfG_Bgsp()->MoveBgMatrixCrrPos(mBG.m5C.m04 (poly info), TRUE, &mMonitor.mPos, NULL, NULL) */
        gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &mMonitor.mPos, 0, 0);
    }
    f32 dist = gabi::call<f32>(0x024F7AC4, playerPos.get(), &mMonitor.mPos); /* dCamMath::xyzHorizontalDistance */
    f32 avg = mMonitor.field_0x0C.y;
    f32 last = mMonitor.field_0x0C.x;
    mMonitor.field_0x0C.z = dist - last;
    mMonitor.field_0x0C.y = gabi::fmadds(dist - avg, 0.075f, avg);
    mMonitor.field_0x0C.x = dist;
    mMonitor.mPos.copy(*playerPos);
    if (m144 == 0 && gabi::call<u32>(0x020075C0, 0) == 0 /* no button held on pad 0 */ &&
        mStickMainValueLast < 0.05f && mStickCValueLast < 0.05f) {
        mMonitor.field_0x18 = mMonitor.field_0x18 + 1;
    } else {
        mMonitor.field_0x18 = 0;
    }
    mMonitor.field_0x1C = mDirection.mRadius - mMonitor.field_0x1C;
}
VERIFY(0x024F9A48, &dCamera_c::updateMonitor);

/* 024F9BF4. HD: the C stick X is negated; the trigger-edge bytes are m19B = m19A ^ 1 */
void dCamera_c::updatePad() {
    WWHD_FUNC(0x024F9BF4, void, this);
    f32 x, y, v;
    gabi::Local<cSAngle_l> unused;
    if (chkFlag(0x1000000)) {
        gabi::call(0x0200658C, unused.get(), CPad_GET_STICK_ANGLE(mPadId)); /* cSAngle unused(stick angle) */
        x = 0.0f;
        y = 0.0f;
        v = 0.0f;
    } else {
        x = CPad_GET_STICK_POS_X(mPadId);
        y = CPad_GET_STICK_POS_Y(mPadId);
        v = CPad_GET_STICK_VALUE(mPadId);
        gabi::call(0x0200658C, unused.get(), CPad_GET_STICK_ANGLE(mPadId));
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
    if (flags & 0x800000) {
        x = 0.0f;
        y = 0.0f;
        v = 0.0f;
    } else {
        x = -CPad_GET_SUBSTICK_POS_X(mPadId);
        y = CPad_GET_SUBSTICK_POS_Y(mPadId);
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
    if (mTriggerLeftLast > gabi::load<f32>(ea() + 0x7E4) /* mCamSetup.ManualEndVal() */) {
        m19B = m19A ^ 1;
        m19A = 1;
    } else {
        m19B = 0;
        m19A = 0;
    }
    f32 r = CPad_GET_ANALOG_R(mPadId);
    f32 lastR = mTriggerRightLast;
    mTriggerRightLast = r;
    mTriggerRightDelta = lastR - r;
    mHoldLockR = mDoCPd_R_LOCK_BUTTON(mPadId);
    mTrigLockR = mDoCPd_R_LOCK_TRIGGER(mPadId);
    if (mTriggerRightLast > gabi::load<f32>(ea() + 0x7E4)) {
        m1A7 = m1A6 ^ 1;
        m1A6 = 1;
    } else {
        m1A7 = 0;
        m1A6 = 0;
    }
    mHoldX = CPad_CHECK_HOLD_X(mPadId);
    mTrigX = CPad_CHECK_TRIG_X(mPadId);
    mHoldY = CPad_CHECK_HOLD_Y(mPadId);
    mTrigY = CPad_CHECK_TRIG_Y(mPadId);
    mHoldZ = CPad_CHECK_HOLD_B(mPadId);
    mTrigZ = CPad_CHECK_TRIG_B(mPadId);
    m1AE = 0;
}
VERIFY(0x024F9BF4, &dCamera_c::updatePad);

/* 024FA058. HD: returns false (no forced lock-on) when no actor id is set, as GameCube does */
bool dCamera_c::checkForceLockTarget() {
    WWHD_FUNC(0x024FA058, bool, this);
    if (mLockOnActorId == fpcM_ERROR_PROCESS_ID_e) return false;
    fopAc_ac_c* actor = GetForceLockOnActor();
    mpLockonActor = actor;
    if (actor == nullptr) return false;
    dAttention_c* attn = dComIfGp_getAttention();
    if (gabi::call<bool>(0x024EDFCC, attn) /* LockonTruth */ || (gabi::load<u32>(gabi::ea(attn) + 0x20) & 0x20000000)) {
        return false; /* attn.Lockon() */
    }
    if ((f32)(s32)mForceLockTimer > (f32)gabi::load<s32>(ea() + 0x7B8) /* mCamSetup.ForceLockOffTimer() */) return false;
    gabi::Local<cXyz> lockPos;
    gabi::Local<cXyz> playerPos;
    gabi::Local<cXyz> diff;
    positionOf(lockPos, mpLockonActor.get());
    positionOf(playerPos, mpPlayerActor.get());
    cXyz_mi(lockPos, diff, playerPos);
    f32 d = std_sqrtf(PSVECSquareMag(diff));
    if (d > gabi::load<f32>(ea() + 0x7B4) /* mCamSetup.ForceLockOffDist() */) return false;
    return true;
}
VERIFY(0x024FA058, &dCamera_c::checkForceLockTarget);

/* 024FFB94 */
static void dCamForcusLine_Draw(dCamForcusLine* i_this) {
    WWHD_FUNC(0x024FFB94, void, i_this);
    if (i_this->m49 == 0) return;
    if (i_this->m48 == 0) {
        /* mEffectLine.initRnd(m4C, m50, m54) */
        gabi::call(0x0252CD10, gabi::ea(i_this) + 4, (s32)i_this->m4C, (s32)i_this->m50, (s32)i_this->m54);
    }
    /* mEffectLine.update(m38, m44, m58, m5A, m5C, m5E, m60, m64, m68, m6C) */
    gabi::call(0x0252CDF0, i_this, &i_this->m38, &i_this->m44, (u16)i_this->m58, (u16)i_this->m5A, (u16)i_this->m5C,
               (u16)i_this->m5E, (f32)i_this->m60, (f32)i_this->m64, (f32)i_this->m68, (f32)i_this->m6C);
}
VERIFY(0x024FFB94, dCamForcusLine_Draw);

/* 024FFC18 */
bool dCamera_c::Draw() {
    WWHD_FUNC(0x024FFC18, bool, this);
    gabi::call(0x024FFB94, &mForcusLine);
    return true;
}
VERIFY(0x024FFC18, &dCamera_c::Draw);

/* 02500620 */
void dCamera_c::initMonitor() {
    WWHD_FUNC(0x02500620, void, this);
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
VERIFY(0x02500620, &dCamera_c::initMonitor);

/* 025006D8. HD: the C stick X is negated; Y is read twice (as on GameCube) */
void dCamera_c::initPad() {
    WWHD_FUNC(0x025006D8, void, this);
    mStickMainPosXLast = CPad_GET_STICK_POS_X(mPadId);
    mStickMainPosYLast = CPad_GET_STICK_POS_Y(mPadId);
    mStickMainValueLast = CPad_GET_STICK_VALUE(mPadId);
    mStickMainPosYDelta = 0.0f;
    mStickMainValueDelta = 0.0f;
    mStickMainPosXDelta = 0.0f;
    mStickCPosXLast = -CPad_GET_SUBSTICK_POS_X(mPadId);
    mStickCPosYLast = CPad_GET_SUBSTICK_POS_Y(mPadId);
    f32 cv = CPad_GET_SUBSTICK_VALUE(mPadId);
    mStickCPosYDelta = 0.0f;
    m18C = 0;
    m188 = 0;
    mStickCPosXDelta = 0.0f;
    mStickCValueDelta = 0.0f;
    mStickCValueLast = cv;
    m184 = 0;
    f32 l = CPad_GET_ANALOG_L(mPadId);
    mTriggerLeftDelta = 0.0f;
    m19A = 0;
    mTrigLockL = 0;
    m19B = 0;
    mHoldLockL = 0;
    mTriggerLeftLast = l;
    f32 r = CPad_GET_ANALOG_R(mPadId);
    m1A6 = 0;
    mTriggerRightDelta = 0.0f;
    m1A7 = 0;
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
VERIFY(0x025006D8, &dCamera_c::initPad);

/* 02500880 */
static void dCamForcusLine_Init(dCamForcusLine* i_this) {
    WWHD_FUNC(0x02500880, void, i_this);
    i_this->m49 = 0;
    i_this->m48 = 1;
    i_this->m38.x = 320.0f;
    i_this->m38.y = 240.0f;
    i_this->m38.z = 0.0f;
    i_this->m44.r = 0xFF;
    i_this->m44.g = 0xFF;
    i_this->m44.b = 0xFF;
    i_this->m44.a = 0x60;
    i_this->m4C = 100;
    i_this->m50 = 100;
    i_this->m54 = 100;
    i_this->m58 = 0x50;
    i_this->m5A = 0;
    i_this->m5C = 4;
    i_this->m5E = 4;
    i_this->m68 = 180.0f;
    i_this->m6C = 0.0f;
    i_this->m60 = 180.0f;
    i_this->m64 = 60.0f;
}
VERIFY(0x02500880, dCamForcusLine_Init);

/* blur matrix: mDoMtx_stack_c (at 1048D0CC) = T(pos) S(scale) Rx Ry Rz T(-pos), then onBlure(mtx) */
static void blureMtx(dCamera_c* c) {
    const u32 stack = 0x1048D0CC;
    gabi::call(0x028E93CC, stack, (f32)c->mBlurePosition.x, (f32)c->mBlurePosition.y, (f32)c->mBlurePosition.z); /* transS */
    mDoMtx_stack_scaleM(c->mBlureScale.x, c->mBlureScale.y, c->mBlureScale.z);
    gabi::call(0x025F1BF4, stack, (s16)c->mBlureRotation.x); /* XrotM */
    gabi::call(0x025F1C28, stack, (s16)c->mBlureRotation.y); /* YrotM */
    gabi::call(0x025F1C5C, stack, (s16)c->mBlureRotation.z); /* ZrotM */
    f32 x = c->mBlurePosition.x, y = c->mBlurePosition.y, z = c->mBlurePosition.z;
    mDoMtx_stack_transM(-x, -y, -z);
    gabi::call(0x025F0634, stack); /* mDoGph_gInf_c::onBlure(mDoMtx_stack_c::get()) */
}
static inline void setBlureRate(s32 r) { gabi::store<u8>(0x101F4826, (u8)r); } /* mDoGph_gInf_c::setBlureRate */

/* 024FC108. HD: the blur fades in over the first 5 frames (rate 255 * alpha - (5 - timer) * 25),
 * the blur position is the player's projected eye in 1280x720; m544/m548 patterns are 5 bytes */
f32 dCamera_c::shakeCamera() {
    WWHD_FUNC(0x024FC108, f32, this);
    f32 res = 0.0f;
    s32 pos = m554;
    if (pos < m550) {
        s32 idx = pos >> 3;
        u32 hi = gabi::load<u8>(ea() + 0x54D + idx); /* m548[idx] */
        u32 lo = gabi::load<u8>(ea() + 0x54E + idx);
        u32 bits = (hi << 8) | lo;
        s32 mask = 1 << (15 - (pos & 7));
        f32 amp = 1.0f;
        for (int i = 0; i < 4; i++) {
            if ((u32)mask & bits) {
                res = gabi::fmadds(gabi::load<f32>(0x1004A818 + i * 4) /* wave[i] */, amp, res);
            } else {
                amp = amp * 0.43f;
            }
            mask >>= 1;
        }
        m554 = pos + 1;
        f32 r = cM_rndFX(0.05f);
        res = gabi::fmuls_ppc(res, r + 0.95f);
        if (m554 & 1) res = -res;
        gabi::Local<cXyz> v, out;
        f32 x = m55C.x;
        gabi::store<u32>(gabi::ea(v.get()) + 8, gabi::load<u32>(ea() + 0x568));
        gabi::store<u32>(gabi::ea(v.get()) + 4, gabi::load<u32>(ea() + 0x564));
        v->x = x + cM_rndFX(0.045f);
        f32 rz = cM_rndFX(0.045f);
        v->z = v->z + rz;
        cXyz_ml(v, out, res);
        v->copy(*out);
        u32 flags = m588;
        if (flags & 2) {
            mEyeShake.copy(*v);
            mCenterShake.copy(*v);
            flags = m588;
        }
        if (flags & 4) {
            f32 rr = cM_rndFX(0.12f);
            mFovYShake = res * rr;
            flags = m588;
        }
        if (flags & 8) {
            gabi::Local<cSAngle_l> a;
            f32 rr = cM_rndFX(0.15f);
            cSAngle_l* b = gabi::call<cSAngle_l*>(0x020066C0, a.get(), res * rr);
            flags = m588;
            mBankShake = (s16)*b;
        }
        if (flags & 0x10) {
            setBlureRate(gabi::ftoi(30.0f * res));
            gabi::call(0x025F064C); /* mDoGph_gInf_c::onBlure() */
            mBlureTimer = 0;
        } else if (flags & 0x20) {
            if (mBlurePositionType == 0) {
                gabi::Local<cXyz> eye, proj;
                eyePos(eye, mpPlayerActor.get());
                gabi::call(0x025F0EA4, eye.get(), proj.get()); /* mDoLib_project */
                f32 px = proj->x + 640.0f;
                f32 py = 360.0f - proj->y;
                mBlurePosition.x = std::fabs(px / 1280.0f);
                mBlurePosition.y = std::fabs(py / 720.0f);
                mBlurePosition.z = 0.0f;
            }
            s32 timer = mBlureTimer;
            if (timer > 0) {
                mBlureTimer = timer - 1;
            } else if (m58C == 1) {
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
        gabi::call(0x028E8DAC, &mCenterShake, t.get(), &mCenterShake); /* PSVECSubtract */
        cXyz_ml(&mEyeShake, t, 0.1f);
        gabi::call(0x028E8DAC, &mEyeShake, t.get(), &mEyeShake);
        f32 fv = mFovYShake;
        mFovYShake = gabi::fnmsubs(fv, 0.1f, fv);
        gabi::call(0x0200693C, &mBankShake, a.get(), 0.1f); /* mBankShake * 0.1 */
        gabi::call(0x020068E0, &mBankShake, a.get());       /* mBankShake -= */
        s32 timer = mBlureTimer;
        if (timer <= 0) {
            gabi::store<u8>(0x101F4825, 0); /* mDoGph_gInf_c::offBlure() */
            m588 = m588 & ~0x20;
            mBlureTimer = 0;
        } else {
            if (m58C == 1) {
                blureMtx(this);
                s32 rate = gabi::ftoi(230.0f * mBlureAlpha);
                mDoMtx_stack_scaleM(mBlureScale.x, mBlureScale.y, mBlureScale.z);
                setBlureRate(rate);
                timer = mBlureTimer;
            }
            if (gabi::ftoi(230.0f * mBlureAlpha) > timer && m58C == 0) {
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
VERIFY(0x024FC108, &dCamera_c::shakeCamera);
