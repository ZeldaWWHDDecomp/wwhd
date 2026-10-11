/** Item acquisition, actor turn and saved-view restoration event cameras.
 * GameCube supplies names/layouts; its function bodies are nonmatching stubs.
 * HD: explicit-result bindings and work overlays preserve the verified implementation. */
#include "d/d_ev_camera_5.h"
using namespace event_camera_transition;

namespace event_camera_transition {
static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: word copies preserve the original memory order and NaN payloads.
    auto* out = reinterpret_cast<CameraPointWordsHD*>(destination);
    auto* in = reinterpret_cast<const CameraPointWordsHD*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void copyFloatPoint(cXyz* destination, const cXyz* source) {
    // HD: collision arguments use floating loads/stores rather than word copies.
    destination->x = (f32)source->x;
    destination->y = (f32)source->y;
    destination->z = (f32)source->z;
}
static void copySavedPositions(CameraSavedViewHD* destination, const CameraSavedViewHD* source) {
    auto* out = reinterpret_cast<CameraWordsHD*>(destination);
    auto* in = reinterpret_cast<const CameraWordsHD*>(source);
    for (int i = 0; i < 6; i++)
        out->mWords[i] = in->mWords[i];
}
static void setPoint(cXyz* point, f32 x, f32 y, f32 z) {
    point->x = x;
    point->y = y;
    point->z = z;
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
/* inlined into getItemEvCamera */
static void eyeFromGlobe(dCamera_c* camera) {
    gabi::Local<cXyz> direction;
    gabi::Local<cXyz> eye;
    cameraTransitionGlobeXyz(&camera->mViewCache.mDirection, direction.get());
    cameraTransitionAdd(&camera->mViewCache.mCenter, eye.get(), direction.get());
    copyPoint(&camera->mViewCache.mEye, eye.get());
}
static void assignAngleSum(be<s16>* destination, const be<s16>* a, const be<s16>* b,
                           be<s16>* sum, be<s16>* constructed) {
    // HD: GHS assigns through a signed-angle constructor result.
    cameraTransitionAngleAdd(a, sum, b);
    be<s16>* result = cameraTransitionAngleShort(constructed, (s16)*sum);
    *destination = (s16)*result;
}
/* inlined into restorePosEvCamera */
static void restoreGlobe(CameraGlobeHD* globe, const cXyz* eye, const cXyz* center) {
    gabi::Local<cXyz> difference;
    gabi::Local<cXyz> eyeDifference;
    gabi::Local<cXyz> normal;
    gabi::Local<cXyz> scaled;
    cameraTransitionSubtract(eye, difference.get(), center);
    cameraTransitionGlobeVector(globe, difference.get());
    cameraTransitionSubtract(eye, eyeDifference.get(), center);
    if (cameraTransitionVectorLength(eyeDifference.get()) < 45.0f) {
        cameraTransitionNormalize(eyeDifference.get(), normal.get());
        cameraTransitionScale(normal.get(), scaled.get(), 60.0f);
        copyPoint(eyeDifference.get(), scaled.get());
        cameraTransitionGlobeVector(globe, eyeDifference.get());
    }
}
} // namespace event_camera_transition

/* 02534964 */
bool event_camera_transition::dCamera_c::restorePosEvCamera() {
    auto* gaps = gabi::at<CameraDefaultGapsHD>(0x10475884);
    auto* timers = gabi::at<CameraTimersHD>(0x101D6230);
    auto* zero = gabi::at<cXyz>(0x101FFBA8);
    if ((u32)gaps->mRestoreGuard == 0) {
        f32 x = (f32)zero->x, y = (f32)zero->y;
        gaps->mRestoreGap.x = x;
        f32 z = (f32)zero->z;
        gaps->mRestoreGap.y = y;
        gaps->mRestoreGap.z = z;
        gaps->mRestoreGuard = 1;
    }
    auto& work = mWork.restore;
    if ((u32)m11C == 0) {
        gabi::Local<cXyz> def;
        auto* d = def.get();
        f32 z = (f32)gaps->mRestoreGap.z, y = (f32)gaps->mRestoreGap.y, x = (f32)gaps->mRestoreGap.x;
        d->z = z;
        d->x = x;
        d->y = y;
        cameraTransitionGetXyz(this, &work.mCenterGap, "CtrGap", d);
        cameraTransitionGetFloat(this, &work.mCushion, "Cushion", 1.0f);
        cameraTransitionGetFloat(this, &work.mNearDist, "NearDist", 750.0f);
        cameraTransitionGetFloat(this, &work.mFarDist, "FarDist", 1500.0f);
        cameraTransitionGetInt(this, &work.mNearTimer, "NearTimer", (s32)timers->mNearTimer);
        cameraTransitionGetInt(this, &work.mFarTimer, "FarTimer", (s32)timers->mFarTimer);
        cameraTransitionGetInt(this, &work.mDest, "Dest", 2);
        cameraTransitionGetInt(this, &work.mTargetType, "TargetType", 0);
        u32 dest = (u32)work.mDest;
        if (dest == RESTORE_PLAY_SLOT) {
            auto* play = reinterpret_cast<PlaySavedCameraHD*>(dComIfGp_get());
            copySavedPositions(&work.mSavedView, &play->mSavedView);
            work.mSavedView.mFovY = (f32)play->mSavedView.mFovY;
            work.mSavedView.mBank = (s16)play->mSavedView.mBank;
        } else {
            const CameraSavedViewHD* source =
                dest == 0 ? &m0A4[0] : dest == 1 ? &m0A4[1]
                                                 : reinterpret_cast<CameraSavedViewHD*>(&m084);
            copySavedPositions(&work.mSavedView, source);
            work.mSavedView.mFovY = (f32)source->mFovY;
        }
        fopAc_ac_c* a = cameraTransitionGetActor(this, "Target", "@PLAYER");
        work.mpTargetActor = a;
        if (a == 0) return true;
        gabi::Local<cXyz> r;
        gabi::Local<CameraGlobeHD> globe;
        auto* rr = r.get();
        auto* gg = globe.get();
        cameraTransitionRelational(this, rr, a, &work.mCenterGap);
        copyPoint(&work.mTargetCenter, rr);
        cameraTransitionSubtract(&work.mSavedView.mEye, rr, &mViewCache.mCenter);
        cameraTransitionGlobeFromVector(gg, rr); /* cSGlobe(const cXyz&) */
        f32 dist = gg->mRadius;
        u32 mode;
        if (dist < (f32)work.mNearDist) {
            mode = cameraTransitionPointInSight(this, &work.mTargetCenter) ^ 1; /* HD: preserve the original xor, including nonboolean callback results. */
        } else if (dist < (f32)work.mFarDist && !cameraTransitionLineCheck(this, &mEye, &work.mTargetCenter, CAMERA_LINE_DEFAULT)) {
            mode = RESTORE_BLEND;
        } else {
            mode = RESTORE_JUMP;
        }
        work.mMode = mode;
        SkipSmoother(this);
    }
    u32 mode = (u32)work.mMode;
    if (mode < RESTORE_NEAR || mode > RESTORE_JUMP) {
        SkipSmoother(this);
        return true;
    }
    if (mode == RESTORE_JUMP) {
        if ((u32)m11C == 0) {
            if ((u32)work.mTargetType == 1) copyPoint(&mViewCache.mCenter, &work.mSavedView.mCenter);
            else copyPoint(&mViewCache.mCenter, &work.mTargetCenter);
            restoreGlobe(&mViewCache.mDirection, &work.mSavedView.mEye, &work.mSavedView.mCenter);
            gabi::Local<cXyz> x;
            gabi::Local<cXyz> e;
            auto* xx = x.get();
            auto* ee = e.get();
            cameraTransitionGlobeXyz(&mViewCache.mDirection, xx);
            cameraTransitionAdd(&mViewCache.mCenter, ee, xx);
            copyPoint(&mViewCache.mEye, ee);
            mViewCache.mFovy = (f32)work.mSavedView.mFovY;
        }
        SkipSmoother(this);
        return true;
    }
    if ((u32)m11C == 0) restoreGlobe(&work.mDirection, &work.mSavedView.mEye, &work.mSavedView.mCenter);
    u32 elapsed = (u32)m11C;
    if (elapsed >= (u32)work.mFarTimer) {
        SkipSmoother(this);
        return true;
    }
    f32 ratio = (f32)(f64)elapsed / (f32)(f64)(s32)(u32)work.mFarTimer;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<be<s16>[4]> an;
    gabi::Local<be<s16>[2]> ct;
    auto* aa = a.get();
    auto* bb = b.get();
    auto* ang = &(*an.get())[0];
    auto* cto = &(*ct.get())[0];
    cameraTransitionSubtract((u32)work.mTargetType == 1 ? &work.mSavedView.mCenter : &work.mTargetCenter, aa, &mViewCache.mCenter);
    cameraTransitionScale(aa, bb, ratio);
    PSVECAdd(&mViewCache.mCenter, bb, &mViewCache.mCenter); /* PSVECAdd */
    f32 radius = (f32)mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mRadius = gabi::fmadds((f32)work.mDirection.mRadius - radius, ratio, radius);
    cameraTransitionAngleSubtract(&work.mDirection.mU, ang + 2, &mViewCache.mDirection.mU);
    cameraTransitionAngleMultiply(ang + 2, ang + 1, ratio);
    cameraTransitionAngleAdd(&mViewCache.mDirection.mU, ang, ang + 1);
    be<s16>* r = cameraTransitionAngleShort(cto, (s16)*ang);
    mViewCache.mDirection.mU = (u16)(s16)*r;
    cameraTransitionAngleSubtract(&work.mDirection.mV, ang, &mViewCache.mDirection.mV);
    cameraTransitionAngleMultiply(ang, ang + 1, ratio);
    cameraTransitionAngleAdd(&mViewCache.mDirection.mV, ang + 2, ang + 1);
    r = cameraTransitionAngleShort(cto + 1, (s16)*(ang + 2));
    mViewCache.mDirection.mV = (u16)(s16)*r;
    cameraTransitionGlobeXyz(&mViewCache.mDirection, bb);
    cameraTransitionAdd(&mViewCache.mCenter, aa, bb);
    copyPoint(&mViewCache.mEye, aa);
    f32 fovy = (f32)mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds((f32)work.mSavedView.mFovY - fovy, ratio, fovy);
    return false;
}

/* 02536404 */
bool event_camera_transition::dCamera_c::turnToActorEvCamera() {
    auto* gaps = gabi::at<CameraDefaultGapsHD>(0x10475884);
    auto* timers = gabi::at<CameraTimersHD>(0x101D6230);
    if ((u32)gaps->mTurnGuard == 0) {
        f32 z0 = 0.0f, y40 = 40.0f;
        gaps->mTurnGap.x = z0;
        gaps->mTurnGap.y = y40;
        gaps->mTurnGuard = 1;
        gaps->mTurnGap.z = z0;
    }
    auto& work = mWork.turn;
    if ((u32)m11C == 0) {
        gabi::Local<cXyz> def;
        auto* d = def.get();
        f32 x = (f32)gaps->mTurnGap.x, y = (f32)gaps->mTurnGap.y;
        d->x = x;
        f32 z = (f32)gaps->mTurnGap.z;
        d->y = y;
        d->z = z;
        cameraTransitionGetXyz(this, &work.mCenterGap, "CtrGap", d);
        cameraTransitionGetFloat(this, &work.mCushion, "Cushion", 1.0f);
        cameraTransitionGetInt(this, &work.mTimer, "Timer", (s32)timers->mTurnTimer);
        cameraTransitionGetFloat(this, &work.mFrontAngle, "FrontAngle", 179.0f);
        fopAc_ac_c* targetActor = cameraTransitionGetActor(this, "Target", "@STARTER");
        work.mpTargetActor = targetActor;
        if (targetActor == 0) {
            SkipSmoother(this);
            return true;
        }
        cameraTransitionAttention(this, d, targetActor);
        copyPoint(&work.mTargetPosition, d);
        u32 elapsed = (u32)m11C;
        SkipSmoother(this);
        if (elapsed == 0) {
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> v;
            gabi::Local<CameraGlobeHD> g1;
            gabi::Local<cXyz> p;
            gabi::Local<cXyz> v2;
            gabi::Local<CameraGlobeHD> g2;
            gabi::Local<be<s16>[2]> dir;
            gabi::Local<be<s16>[2]> diff;
            gabi::Local<be<s16>[24]> tmp;
            auto* cc = c.get();
            auto* vv = v.get();
            auto* targetGlobe = g1.get();
            auto* pp = p.get();
            auto* vv2 = v2.get();
            auto* eyeGlobe = g2.get();
            auto* dr = &(*dir.get())[0];
            auto* df = &(*diff.get())[0];
            auto* tp = &(*tmp.get())[0];
            cameraTransitionRelational(this, cc, mpPlayerActor.get(), &work.mCenterGap);
            cameraTransitionSubtract(cc, vv, &work.mTargetPosition);
            cameraTransitionGlobeFromVector(targetGlobe, vv); /* cSGlobe(const cXyz&) */
            cameraTransitionPosition(this, pp, work.mpTargetActor.get());
            cameraTransitionSubtract(&mViewCache.mEye, vv2, pp);
            cameraTransitionGlobeFromVector(eyeGlobe, vv2);
            cameraTransitionDirection(this, dr, work.mpTargetActor.get());
            cameraTransitionAngleSubtract(&eyeGlobe->mU, df, dr);
            bool belowMinimum;
            if ((s16)*df < (s16)*gabi::at<be<s16>>(0x101FF354)) { /* the actor faces away: turn +5 degrees */
                be<s16>* five = cameraTransitionAngleDegrees(tp, 5.0f);
                assignAngleSum(&targetGlobe->mU, &targetGlobe->mU, five, tp + 1, tp + 2);
            } else {
                be<s16>* five = cameraTransitionAngleDegrees(tp + 3, -5.0f);
                assignAngleSum(&targetGlobe->mU, &targetGlobe->mU, five, tp + 4, tp + 5);
            }
            cameraTransitionDirection(this, dr, work.mpTargetActor.get());
            cameraTransitionAngleSubtract(&targetGlobe->mU, df + 1, dr);
            cameraTransitionAngleDefault(tp + 6); /* cSAngle() */
            be<s16>* minimumAngle = cameraTransitionAngleDegrees(tp + 7, -(f32)work.mFrontAngle);
            belowMinimum = (s16)*(df + 1) < (s16)*minimumAngle;
            if (belowMinimum) {
                cameraTransitionDirection(this, dr + 1, work.mpTargetActor.get());
                be<s16>* b = cameraTransitionAngleDegrees(tp + 8, -(f32)work.mFrontAngle);
                assignAngleSum(&targetGlobe->mU, dr + 1, b, tp + 9, tp + 10);
            } else {
                be<s16>* maximumAngle = cameraTransitionAngleDegrees(tp + 11, (f32)work.mFrontAngle);
                if ((s16)*(df + 1) > (s16)*maximumAngle) {
                    cameraTransitionDirection(this, dr + 1, work.mpTargetActor.get());
                    be<s16>* b = cameraTransitionAngleDegrees(tp + 12, (f32)work.mFrontAngle);
                    assignAngleSum(&targetGlobe->mU, dr + 1, b, tp + 13, tp + 14);
                }
            }
            cameraTransitionGlobeSet(&work.mDirection, 120.0f, &targetGlobe->mV, &targetGlobe->mU); /* cSGlobe::Val(r, V, U) */
            cameraTransitionRelational(this, pp, mpPlayerActor.get(), &work.mCenterGap);
            copyPoint(&work.mCenter, pp);
        }
    }
    u32 elapsed = (u32)m11C;
    if (elapsed >= (u32)work.mTimer) {
        SkipSmoother(this);
        return true;
    }
    f32 ratio = (f32)(f64)elapsed / (f32)(f64)(s32)(u32)work.mTimer;
    gabi::Local<cXyz> targetActor;
    gabi::Local<cXyz> b;
    gabi::Local<be<s16>[4]> ang;
    gabi::Local<be<s16>[2]> ct;
    auto* aa = targetActor.get();
    auto* bb = b.get();
    auto* an = &(*ang.get())[0];
    auto* cto = &(*ct.get())[0];
    cameraTransitionSubtract(&work.mCenter, aa, &mViewCache.mCenter);
    cameraTransitionScale(aa, bb, ratio);
    PSVECAdd(&mViewCache.mCenter, bb, &mViewCache.mCenter); /* PSVECAdd */
    f32 radius = (f32)mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mRadius = gabi::fmadds((f32)work.mDirection.mRadius - radius, ratio, radius);
    /* U then V towards the target globe */
    cameraTransitionAngleSubtract(&work.mDirection.mU, an + 2, &mViewCache.mDirection.mU);
    cameraTransitionAngleMultiply(an + 2, an + 1, ratio);
    cameraTransitionAngleAdd(&mViewCache.mDirection.mU, an, an + 1);
    be<s16>* r = cameraTransitionAngleShort(cto, (s16)*an);
    mViewCache.mDirection.mU = (u16)(s16)*r;
    cameraTransitionAngleSubtract(&work.mDirection.mV, an, &mViewCache.mDirection.mV);
    cameraTransitionAngleMultiply(an, an + 1, ratio);
    cameraTransitionAngleAdd(&mViewCache.mDirection.mV, an + 2, an + 1);
    r = cameraTransitionAngleShort(cto + 1, (s16)*(an + 2));
    mViewCache.mDirection.mV = (u16)(s16)*r;
    cameraTransitionGlobeXyz(&mViewCache.mDirection, bb);
    cameraTransitionAdd(&mViewCache.mCenter, aa, bb);
    copyPoint(&mViewCache.mEye, aa);
    cameraTransitionSubtract(&mViewCache.mEye, aa, &mViewCache.mCenter);
    cameraTransitionGlobeVector(&mViewCache.mDirection, aa);
    return false;
}

/* 025351A0 */
bool event_camera_transition::dCamera_c::getItemEvCamera() {
    auto& work = mWork.item;
    gabi::Local<cXyz> ctrOfs;
    gabi::Local<cXyz[4]> eyeOfs;
    auto* co = ctrOfs.get();
    auto* eo = &(*eyeOfs.get())[0];
    setPoint(co, -0.095f, 0.428f, -7.177f);
    setPoint(eo + 0, 0.17f, 97.0f, -57.78f);
    setPoint(eo + 1, 35.098f, 100.791f, -31.185f);
    setPoint(eo + 2, -28.844f, 99.996f, -41.073f);
    setPoint(eo + 3, 0.17f, 97.0f, -57.78f);
    gabi::Local<ItemAngleSlotsHD> nameU; /* the original's sp+0x20: the process name searched for, then cSAngle U */
    gabi::Local<be<s16>> angV;
    auto* nm = &nameU->mProcessName;
    auto* au = &nameU->mU;
    auto* av = angV.get();
    cameraTransitionAngleDefault(au); /* cSAngle() */
    cameraTransitionAngleDefault(av);
    u32 state;
    if ((u32)m11C == 0) {
        state = 0;
        work.mState = 0;
    } else {
        state = (u32)work.mState;
    }
    if (state != ITEM_WAIT && state != ITEM_CHOOSE_EYE && state != ITEM_BLEND && state != ITEM_SETTLE && state != ITEM_HOLD) {
        cameraTransitionGetInt(this, &work.mTimer1, "Timer1", 27);
        cameraTransitionGetInt(this, &work.mTimer2, "Timer2", 5);
        work.mInitialCounter = 0;
        u32 t1 = (u32)work.mTimer1;
        work.mState = ITEM_WAIT;
        work.mElapsed = 0;
        work.mTimer = t1;
        state = ITEM_WAIT;
    }
    if (state == ITEM_WAIT) {
        mEventFlags = (u32)mEventFlags | CAMERA_HOLD_VIEW;
        if ((s32)(u32)work.mElapsed < (s32)(u32)work.mTimer) {
            work.mElapsed = (u32)work.mElapsed + 1;
            return false;
        }
        work.mState = ITEM_CHOOSE_EYE;
        work.mElapsed = (u32)work.mElapsed + 1;
        return false;
    }
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> d;
    auto* rr = r.get();
    auto* dd = d.get();
    if (state == ITEM_CHOOSE_EYE) {
        work.mElapsed = 0;
        cameraTransitionRelational(this, rr, mpPlayerActor.get(), co);
        copyPoint(&work.mCenter, rr);
        gabi::Local<cXyz> e;
        gabi::Local<cXyz> eye;
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> a1;
        gabi::Local<cXyz> a2;
        auto* ee = e.get();
        auto* ey = eye.get();
        auto* pp = pos.get();
        auto* p1 = a1.get();
        auto* p2 = a2.get();
        for (u32 k = 0; k < 4; k++) {
            cameraTransitionRelational(this, ee, mpPlayerActor.get(), eo + k);
            copyPoint(ey, ee);
            cameraTransitionPosition(this, ee, mpPlayerActor.get());
            f32 floor = (f32)ee->y + (f32)m368;
            if ((f32)ey->y < floor) { /* bge: taken on NaN */
                cameraTransitionPosition(this, pp, mpPlayerActor.get());
                ey->y = (f32)pp->y + (f32)m368;
            }
            fopAc_ac_c* ship = nullptr;
            s32 pad = (s32)(u32)mPadId;
            if (dComIfGp_checkPlayerStatus0(pad, PLAYER_HOLD_ITEM)) { /* the player holds an item up */
                *nm = fpcNm_SHIP_e;
                // HD: retain the mapped guest callback target; autoglue cannot register an ext-only host wrapper.
                ship = fopAcIt_Judge(JUDGE_FOR_PROCESS_NAME, nm);
            }
            if (cameraTransitionLineCheck(this, &work.mCenter, ey, CAMERA_LINE_DEFAULT)) continue;
            copyFloatPoint(p1, &work.mCenter);
            copyFloatPoint(p2, ey);
            fopAc_ac_c* pl = mpPlayerActor;
            auto* collision = dComIfG_Ccsp();
            if (!cameraTransitionCheckCamera(collision, p1, p2, 15.0f, pl, ship)) break; /* dCcS::ChkCamera */
        }
        cameraTransitionSubtract(ey, rr, &work.mCenter);
        cameraTransitionGlobeVector(&work.mDirection, rr);
        u32 t2 = (u32)work.mTimer2;
        work.mState = ITEM_BLEND;
        work.mTimer = t2;
        work.mFovy = 79.0f; /* 79 */
        state = ITEM_BLEND;
    }
    if (state == ITEM_BLEND) {
        f32 t = (f32)(f64)(s32)(u32)work.mElapsed / (f32)(f64)(s32)(u32)work.mTimer;
        f32 fovy = (f32)mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds((f32)work.mFovy - fovy, t, fovy);
        cameraTransitionRelational(this, rr, mpPlayerActor.get(), co);
        copyPoint(&work.mCenter, rr);
        gabi::Local<cXyz> s;
        auto* ss = s.get();
        cameraTransitionSubtract(&work.mCenter, ss, &mViewCache.mCenter);
        cameraTransitionScale(ss, rr, t);
        PSVECAdd(&mViewCache.mCenter, rr, &mViewCache.mCenter); /* PSVECAdd */
        *av = (u16)(s16)mViewCache.mDirection.mV;
        *au = (u16)(s16)mViewCache.mDirection.mU;
        f32 rad = (f32)mViewCache.mDirection.mRadius;
        rad = gabi::fmadds((f32)work.mDirection.mRadius - rad, t, rad);
        gabi::Local<be<s16>> t1;
        gabi::Local<be<s16>> t2;
        auto* a1 = t1.get();
        auto* a2 = t2.get();
        cameraTransitionAngleSubtract(&work.mDirection.mV, a1, av);
        cameraTransitionAngleMultiply(a1, a2, t);
        cameraTransitionAngleAddAssign(av, a2);
        cameraTransitionAngleSubtract(&work.mDirection.mU, a2, au);
        cameraTransitionAngleMultiply(a2, a1, t);
        cameraTransitionAngleAddAssign(au, a1);
        cameraTransitionGlobeSet(&mViewCache.mDirection, rad, av, au); /* cSGlobe::Val(r, V, U) */
        gabi::Local<cXyz> x;
        auto* xx = x.get();
        cameraTransitionGlobeXyz(&mViewCache.mDirection, rr);
        cameraTransitionAdd(&mViewCache.mCenter, xx, rr);
        copyPoint(&mViewCache.mEye, xx);
        if ((s32)(u32)work.mElapsed < (s32)(u32)work.mTimer) {
            work.mElapsed = (u32)work.mElapsed + 1;
            return false;
        }
        work.mState = ITEM_SETTLE;
        state = ITEM_SETTLE;
    }
    if (state == ITEM_SETTLE) {
        cameraTransitionRelational(this, dd, mpPlayerActor.get(), co);
        copyPoint(&mViewCache.mCenter, dd);
        eyeFromGlobe(this);
        work.mState = ITEM_HOLD;
    }
    SkipSmoother(this);
    cameraTransitionRelational(this, dd, mpPlayerActor.get(), co);
    copyPoint(&mViewCache.mCenter, dd);
    eyeFromGlobe(this);
    work.mElapsed = (u32)work.mElapsed + 1;
    return true;
}

// clang-format off
/* WWHD glue
str 1004D0A0 "Timer1" in 025351A0
str 1004D0A8 "Timer2" in 025351A0
str 1004D108 "CtrGap" in 02536404
str 1004D0F8 "Cushion" in 02536404
str 1004D100 "Timer" in 02536404
str 1004D118 "FrontAngle" in 02536404
str 1004D110 "Target" in 02536404
str 1004D124 "@STARTER" in 02536404
str 1004D024 "CtrGap" in 02534964
str 1004D00C "Cushion" in 02534964
str 1004D048 "NearDist" in 02534964
str 1004D014 "FarDist" in 02534964
str 1004D03C "NearTimer" in 02534964
str 1004D054 "FarTimer" in 02534964
str 1004D02C "Dest" in 02534964
str 1004D060 "TargetType" in 02534964
str 1004D034 "Target" in 02534964
str 1004D01C "@PLAYER" in 02534964
ext 020065FC void cameraTransitionAngleDefault(be<s16>*)
ext 020066C0 be<s16>* cameraTransitionAngleDegrees(be<s16>*,f32)
ext 0200658C be<s16>* cameraTransitionAngleShort(be<s16>*,s32)
ext 020068B0 void cameraTransitionAngleSubtract(const be<s16>*,be<s16>*,const be<s16>*)
ext 0200693C void cameraTransitionAngleMultiply(const be<s16>*,be<s16>*,f32)
ext 020068CC void cameraTransitionAngleAddAssign(be<s16>*,const be<s16>*)
ext 02006894 void cameraTransitionAngleAdd(const be<s16>*,be<s16>*,const be<s16>*)
ext 0201ADE0 void cameraTransitionSubtract(const cXyz*,cXyz*,const cXyz*)
ext 0201AD78 void cameraTransitionAdd(const cXyz*,cXyz*,const cXyz*)
ext 0201AE48 void cameraTransitionScale(const cXyz*,cXyz*,f32)
ext 0201B31C void cameraTransitionNormalize(const cXyz*,cXyz*)
ext 028E8DD0 f32 cameraTransitionSquareMagnitude(const cXyz*)
ext 028F4384 f32 cameraTransitionSquareRoot(f32)
ext 020072E0 void cameraTransitionGlobeVector(CameraGlobeHD*,const cXyz*)
ext 02007324 void cameraTransitionGlobeFromVector(CameraGlobeHD*,const cXyz*)
ext 020073AC void cameraTransitionGlobeXyz(const CameraGlobeHD*,cXyz*)
ext 020071E0 void cameraTransitionGlobeSet(CameraGlobeHD*,f32,const be<s16>*,const be<s16>*)
ext 0250242C void cameraTransitionRelational(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*,const cXyz*)
ext 024F8D5C void cameraTransitionPosition(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*)
ext 024F8F3C void cameraTransitionAttention(event_camera_transition::dCamera_c*,cXyz*,fopAc_ac_c*)
ext 024F8F30 void cameraTransitionDirection(event_camera_transition::dCamera_c*,be<s16>*,fopAc_ac_c*)
ext 024FCBE8 bool cameraTransitionLineCheck(event_camera_transition::dCamera_c*,const cXyz*,const cXyz*,u32)
ext 025187D8 bool cameraTransitionCheckCamera(cCcS*,cXyz*,cXyz*,f32,fopAc_ac_c*,fopAc_ac_c*)
ext 02530634 u32 cameraTransitionGetInt(event_camera_transition::dCamera_c*,be<u32>*,const char*,s32)
ext 0253072C u32 cameraTransitionGetFloat(event_camera_transition::dCamera_c*,be<f32>*,const char*,f32)
ext 0253086C u32 cameraTransitionGetXyz(event_camera_transition::dCamera_c*,cXyz*,const char*,const cXyz*)
ext 02530CB4 fopAc_ac_c* cameraTransitionGetActor(event_camera_transition::dCamera_c*,const char*,const char*)
ext 025050E4 u32 cameraTransitionPointInSight(event_camera_transition::dCamera_c*,const cXyz*)
*/
