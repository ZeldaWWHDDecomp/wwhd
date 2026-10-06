/**
 * d_camera_run.cpp (WWHD)
 * Follow camera dCamera_c::Run: the per-frame update (type/mode/style selection, engine dispatch,
 * smoothing, up vector, sounds and attention status).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"
#include <bit>

static inline f32 envZoom_get() { return gabi::load<f32>(gabi::ea(dKy_getEnvlight()) + 0x104C); }

/* dCamera_c::engine_tbl[alg] (pointer to member function: s16 this delta, s16 vtable index,
 * function or vtable offset) */
static u32 callEngine(dCamera_c* cam, u32 alg, s32 style) {
    u32 e = 0x101D5678 + alg * 8;
    s16 vidx = gabi::load<s16>(e + 2);
    u32 self = cam->ea() + gabi::load<s16>(e);
    if (vidx < 0) return gabi::call_ptr<u32>(gabi::load<u32>(e + 4), self, style);
    u32 vtbl = gabi::load<u32>(self + gabi::load<s16>(e + 6));
    return gabi::call_ptr<u32>(gabi::load<u32>(vtbl + vidx * 8 + 4), self, style);
}

/* 024FE3E8. HD: the field of view is scaled by an environment-light zoom factor (0.7 / 1.0 /
 * eased to 0.8 or 1.0 by the light's state byte); the floor clamp keeps the subject camera's
 * centre except in the "Tornado" type; switching to a subject style calls 026185BC; the pad is
 * also updated in minigame 8; m514 is a byte */
u32 dCamera_c::Run() {
    WWHD_FUNC(0x024FE3E8, u32, this);
    u32 res = 0; /* the engine's r3, returned unmasked */
    gabi::call<bool>(0x024F8D4C, &mForcusLine); /* mForcusLine.Off() */
    mEventFlags = mEventFlags & ~0x10149C01u;
    checkSpecialArea();
    checkGroundInfo();
    s32 ship = m530;
    if (ship != 0 && !chkFlag(0x200000) && gabi::load<s32>(dComIfGp_ea() + 0x52E4) == 0 && !chkFlag(0x20000000)) {
        f32 ofs = gabi::call<f32>(0x025269A8, &m534, &m536, 130.0f); /* daObjPirateship::getShipOffsetY */
        f32 y = ofs * m540;
        f32 dy = y - m538;
        if (m530 == 1 && m53C < 0.0f && dy > 0.0f) m254 = m254 | 4;
        f32 cy = mViewCache.mCenter.y;
        f32 k = gabi::load<f32>(ea() + 0x7FC); /* mCamSetup.m0B8 */
        m53C = dy;
        m538 = y;
        mViewCache.mCenter.y = gabi::fnmsubs(dy, k, cy);
    }
    updateMonitor();
    Att();
    clrComStat(0x3400);
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) == 0 && !chkFlag(0x20000000)) {
        updatePad();
        gabi::call(0x024F6E54, ea() + 0x82C, (s32)mPadId); /* mCamSetup.mCstick.Shift(mPadId) */
    }
    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 8) { /* dComIfGp_getMiniGameType() */
        updatePad();
        gabi::call(0x024F6E54, ea() + 0x82C, (s32)mPadId);
    }
    dAttention_c* attn = dComIfGp_getAttention();
    if (gabi::call<bool>(0x024EDFCC, attn) || (gabi::load<u32>(gabi::ea(attn) + 0x20) & 0x20000000)) {
        mEventFlags = mEventFlags | 0x1000; /* attention Lockon() */
    }
    bool forceLock = checkForceLockTarget();
    s32 curType = mCurType;
    if (!forceLock) {
        mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
    } else {
        mForceLockTimer = mForceLockTimer + 1;
    }
    s32 next = nextType(curType);
    mNextType = next;
    if ((u32)next != (u32)(s32)mCurType) {
        if (onTypeChange(mCurType, next)) mCurType = (s32)mNextType;
    }

    s32 nextM = nextMode(mCurMode);
    mNextMode = nextM;
    s32 mode = mCurMode;
    s32 type = mCurType;
    if ((u32)nextM != (u32)mode) {
        if (camTypeStyle(type, nextM) >= 0) {
            if (onModeChange(mode, nextM)) {
                type = mCurType;
                mode = mNextMode;
                mCurMode = mode;
            } else {
                type = mCurType;
                mode = mCurMode;
            }
        }
    }
    s32 style = camTypeStyle(type, mode);
    bool styleOk = true;
    if (style < 0) {
        type = mCurType;
        mCurMode = 0;
        style = camTypeStyle(type, 0);
        if (style < 0) styleOk = false;
    }
    if (styleOk && (u32)(s32)mCurStyle != (u32)style) {
        if (onStyleChange(mCurStyle, style)) {
            mCurStyle = (s32)camTypeStyle(mCurType, mCurMode);
            gabi::call(0x024F727C, ea() + 0x8A4, (s32)mCurStyle); /* mCamParam.Change(mCurStyle) */
            if (camStyleAlg(mCurStyle) == 4 /* SUBJECT */) {
                gabi::call(0x026185BC, gabi::load<u32>(0x101F5088));
                m154 = 0.0f;
            }
        }
    }
    u32 flags = mEventFlags & ~0x20u;
    s32 m = mCurMode;
    mEventFlags = flags;
    if (m == 12) flags |= 0x20;
    mEventFlags = flags & 0x7FFFFFFF;
    clrComStat(0x80);

    bool fwd = false;
    if (camStyleFlags() & 4) {
        s32 pad = mPadId;
        if (!(gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x4000000)) { /* check_owner_action */
            pad = mPadId;
            if (!(gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC) & 0x40000)) fwd = true; /* check_owner_action1 */
        }
    }
    if (fwd) {
        gabi::Local<cSAngle_l> a, d, s;
        gabi::call(0x024FB930, this, a.get()); /* forwardCheckAngle() */
        gabi::call(0x020068B0, a.get(), d.get(), &m148); /* - m148 */
        gabi::call(0x0200693C, d.get(), s.get(), gabi::load<f32>(ea() + 0x86C)); /* * FwdCushion */
        gabi::call(0x020068CC, &m148, s.get()); /* m148 += */
    } else {
        m148 = gabi::load<s16>(0x101FF354); /* cSAngle::_0 */
    }
    defaultTriming();
    mTrimTypeForce = -1;
    m068 = 9;

    if (chkFlag(0x200000) && camStyleAlg(mCurStyle) != 11 /* EVENT */) {
        s32 pad = mPadId;
        if (gabi::call<f32>(0x020079B4, pad) > 0.001f || gabi::call<f32>(0x02007B44, pad) > 0.001f ||
            gabi::call<u32>(0x020075C0, pad) != 0 || mMonitor.field_0x0C.x > 10.0f || m360 == 0 || m31C != 0) {
            gabi::store<u8>(ea() + 0x518, 0); /* m514 */
            mEventFlags = mEventFlags & ~0x200000u;
        } else {
            gabi::store<u8>(ea() + 0x518, 0);
        }
    } else {
        u32 demo = gabi::load<u32>(0x101D5FFC);
        if (demo == 0) {
            JUT_ASSERT_fail(STR(0x1004A898), 0x23E, STR(0x1004A888));
            demo = gabi::load<u32>(0x101D5FFC);
        }
        if (gabi::call<u32>(0x025283F8, demo) != 0 && gabi::load<u32>(gabi::load<u32>(ea() + 0x8A8) + 4) != 11) {
            res = gabi::call<u32>(0x024FC0A8, this, 0); /* demoCamera(0) */
        } else {
            res = callEngine(this, camStyleAlg(mCurStyle), mCurStyle);
            m07C = m07C + 1;
            m11C = m11C + 1;
            m118 = m118 + 1;
            m080 = m080 + 1;
            m108 = m108 + 1;
        }
        if (!res) gabi::store<u8>(ea() + 0x518, 0);
    }

    if (!chkFlag(0x400)) {
        gabi::Local<cSAngle_l> t;
        gabi::call(0x0200693C, &mViewCache.mBank, t.get(), 0.05f); /* mBank * 0.05 */
        gabi::call(0x020068E0, &mViewCache.mBank, t.get());        /* mBank -= */
    }
    gabi::call<f32>(0x024FC108, this); /* shakeCamera() */
    mEventFlags = mEventFlags & ~0x90080u;
    u16 pf = camStyleFlags();
    if (pf & 1) {
        m068 = 0x3F;
        pf = camStyleFlags();
    } else if (pf & 2) {
        m068 = 0xF;
        pf = camStyleFlags();
    }
    f32 floorY = m354;
    u32 cyBits = gabi::load<u32>(ea() + 0x48); /* mViewCache.mCenter.y (copied bit-exact) */
    f32 cy = std::bit_cast<f32>(cyBits);
    f32 margin = gabi::load<f32>(ea() + 0x844); /* mCamSetup.mBGChk.FloorMargin() */
    if (pf & 0x400) m068 = m068 | 0x40;
    floorY = floorY + margin;
    gabi::store<u32>(ea() + 0x10, gabi::load<u32>(ea() + 0x44)); /* mCenter.x = mViewCache.mCenter.x */
    if (cy < floorY) {
        gabi::store<u32>(ea() + 0x18, gabi::load<u32>(ea() + 0x4C)); /* mCenter.z */
        if (camStyleAlg(mCurStyle) == 4 && (mEventFlags & 0x10000800) &&
            GetCameraTypeFromCameraName(STR(0x1004A880) /* "Tornado" */) != (s32)mCurType) {
            m068 = m068 & ~8u;
            gabi::store<u32>(ea() + 0x14, gabi::load<u32>(ea() + 0x48));
        } else {
            mCenter.y = floorY;
        }
    } else {
        gabi::store<u32>(ea() + 0x14, cyBits);
        gabi::store<u32>(ea() + 0x18, gabi::load<u32>(ea() + 0x4C));
    }

    /* HD: environment-light zoom factor */
    u32 el = gabi::ea(dKy_getEnvlight());
    if (gabi::load<u8>(el + 0x10A4) == 2) {
        el = gabi::ea(dKy_getEnvlight());
        gabi::store<f32>(el + 0x104C, 0.7f);
    } else {
        el = gabi::ea(dKy_getEnvlight());
        if (gabi::load<u8>(el + 0x10A4) == 0xFF) {
            el = gabi::ea(dKy_getEnvlight());
            gabi::store<f32>(el + 0x104C, 1.0f);
        } else {
            el = gabi::ea(dKy_getEnvlight());
            f32 target = gabi::load<u8>(el + 0x10A4) != 0 ? 0.8f : 1.0f;
            el = gabi::ea(dKy_getEnvlight());
            gabi::call<f32>(0x0200ECD4, el + 0x104C, target, 0.05f, 0.05f, 0.01f); /* cLib_addCalc */
        }
    }
    f32 zoom = envZoom_get();
    s16 bank = mViewCache.mBank;
    mBank = bank;
    mFovy = gabi::fmuls_ppc(mViewCache.mFovy, zoom);
    gabi::call<bool>(0x024FD11C, this, (u32)m068); /* bumpCheck(m068) */

    gabi::Local<cSAngle_l> stick, diff, lim1, lim2, inv, stick2, dmc, vlo, vhi;
    cSAngle_l* sa = gabi::call<cSAngle_l*>(0x0200658C, stick.get(), CPad_GET_STICK_ANGLE(mPadId));
    gabi::call(0x020068B0, sa, diff.get(), &mDMCSystem.field_0x4);
    bool reset = false;
    if (mStickMainValueLast < gabi::load<f32>(ea() + 0x7CC) /* DMCValue */) {
        reset = true;
    } else {
        cSAngle_l* hi = gabi::call<cSAngle_l*>(0x020066C0, lim1.get(), gabi::load<f32>(ea() + 0x7D0)); /* DMCAngle */
        if ((s16)*diff > (s16)*hi) {
            reset = true;
        } else {
            cSAngle_l* lo = gabi::call<cSAngle_l*>(0x020066C0, lim2.get(), -gabi::load<f32>(ea() + 0x7D0));
            if ((s16)*diff < (s16)*lo) reset = true;
        }
    }
    s16 angleY;
    if (reset) mDMCSystem.field_0x0 = 0;
    if (!reset && mDMCSystem.field_0x0 != 0) {
        cSAngle_l* s2 = gabi::call<cSAngle_l*>(0x0200658C, stick2.get(), CPad_GET_STICK_ANGLE(mPadId));
        getDMCAngle(dmc, s2);
        angleY = *dmc;
    } else {
        s16 v = cSAngle_Inv(&mDirection.mU);
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, inv.get(), v);
        angleY = *a;
    }
    mAngleY = angleY;

    if (mCenter.x == mEye.x && mCenter.z == mEye.z) {
        mUp.z = 0.0f;
        mUp.y = 1.0f;
        mUp.x = 0.01f;
    } else {
        cSAngle_l* lo = gabi::call<cSAngle_l*>(0x020066C0, vlo.get(), -90.0f);
        bool down = true;
        if (!((s16)mDirection.mV < (s16)*lo)) {
            cSAngle_l* hi = gabi::call<cSAngle_l*>(0x020066C0, vhi.get(), 90.0f);
            if (!((s16)mDirection.mV > (s16)*hi)) down = false;
        }
        if (down) {
            mUp.x = 0.0f;
            mUp.z = 0.0f;
            mUp.y = -1.0f;
        } else {
            mUp.z = 0.0f;
            mUp.y = 1.0f;
            mUp.x = 0.0f;
        }
    }

    u32 cur = (u32)m254;
    for (u32 i = 0; i < 3; i++) {
        u32 bit = 1u << i;
        if ((cur & bit) && !((u32)m258 & bit)) {
            gabi::call(0x025E1988, (s32)m248[i]); /* mDoAud_seStart(m248[i]) */
            cur = (u32)m254;
        }
    }
    m258 = cur;
    m254 = 0;

    if (m100 != 0 && m101 != 0 && m102 != 0) {
        setComStat(0x10);
    } else {
        clrComStat(0x10);
    }
    u32 f = mEventFlags;
    if (f & 0x40000) {
        setComStat(2);
    } else if (mDirection.mRadius < gabi::load<f32>(ea() + 0x788) /* PlayerHideDist */) {
        if (f & 0x800) {
            setComStat(2);
            f = mEventFlags;
        }
        if (f & 0x10000000) setComStat(0x20);
    }
    return res;
}
VERIFY(0x024FE3E8, &dCamera_c::Run);

/* 024FF164. HD: the "AutoForcus" event data no longer switches auto focus off; leaving a
 * subject->event style (mHD610) is cancelled by any action other than PAUSE/WAIT */
bool dCamera_c::eventCamera(s32) {
    WWHD_FUNC(0x024FF164, bool, this, 0);
    if (m118 == 0) {
        PosSet* p0 = &m0A4[0];
        PosSet* p1 = &m0A4[1];
        u32 fovy = gabi::load<u32>(ea() + 0x38);
        gabi::store<u32>(gabi::ea(&p1->mFovY), fovy);
        p0->mEye.copy(mEye);
        p1->mEye.copy(mEye);
        u16 bank = gabi::load<u16>(ea() + 0x34);
        gabi::store<u16>(gabi::ea(&p1->mBank), bank);
        gabi::store<u16>(gabi::ea(&p0->mBank), bank);
        p0->mCenter.copy(mCenter);
        p1->mCenter.copy(mCenter);
        p0->m1E = 0;
        gabi::store<u32>(gabi::ea(&p0->mFovY), fovy);
        p1->m1E = 0;
    }
    u32 evmng = 0;
    s32 act;
    if (!chkFlag(0x20000000)) {
        if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) == 0) return false; /* dComIfGp_evmng_cameraPlay() */
        evmng = dComIfGp_ea() + 0x52C4;
        s32 staff = gabi::call<s32>(0x02542D88, evmng, STR(0x1004A8B8) /* "CAMERA" */, 0, 0); /* getMyStaffId */
        if (staff < 0) return false;
        if ((u32)mEventData.mStaffIdx != (u32)staff) {
            m108 = 0;
            m11C = 0;
            mEventFlags = mEventFlags & ~0x200000u;
            m118 = 0;
        }
        mEventData.mStaffIdx = staff;
        if (gabi::call<bool>(0x025447C8, dComIfGp_ea() + 0x52C4, staff)) { /* getIsAddvance */
            m101 = 0;
            m100 = 0;
            m102 = 0;
            m11C = 0;
        }
        s32 idx = mEventData.mStaffIdx;
        act = gabi::call<s32>(0x02542EDC, dComIfGp_ea() + 0x52C4, idx, 0x101D5550u /* ActionNames */, 0x1C, 0, 0); /* getMyActIdx */
    } else {
        mEventData.mStaffIdx = -1;
        if (m118 == 0) m11C = 0;
        act = mEventData.field_0x18;
    }
    if ((u32)act >= 0x1C) {
        s32 idx = mEventData.mStaffIdx;
        gabi::call(0x02543280, dComIfGp_ea() + 0x52C4, idx); /* cutEnd */
        return false;
    }
    if (m11C == 0) {
        if (m118 == 0) {
            mEventData.field_0x1c = 2;
            mEventFlags = mEventFlags & ~0x200000u;
        }
        if (mHD610 != 0 && act != 0 && act != 1) mHD610 = 0;
        gabi::Local<u8[12]> trim;
        if (gabi::call<bool>(0x02530998, this, trim.get(), STR(0x1004A8C0) /* "Trim" */, STR(0x1004A8A4) /* "CINESCO" */)) {
            u32 w = gabi::load<u32>(gabi::ea(trim.get()));
            if (w == 0x5354414E) mEventData.field_0x1c = 0;        /* 'STAN' */
            else if (w == 0x56495354) mEventData.field_0x1c = 1;   /* 'VIST' */
            else if (w == 0x44454D4F) mEventData.field_0x1c = 3;   /* 'DEMO' */
            else if (w == 0x4E4F4E45) mEventData.field_0x1c = 4;   /* 'NONE' */
            else if (w == 0x4B454550) mEventData.field_0x1c = 999; /* 'KEEP' */
        }
        gabi::call(0x02530634, this, &mEventData.field_0x24, STR(0x1004A8D4) /* "WaitAnyKey" */, 0); /* getEvIntData */
        if (mEventData.field_0x24 != 0) mEventFlags = mEventFlags | 0x200000;
        gabi::call(0x02530634, this, &mEventData.field_0x20, STR(0x1004A8AC) /* "BGCheck" */, 1);
        gabi::call(0x02530634, this, &mEventData.field_0x24, STR(0x1004A8E0) /* "AutoForcus" */, 1);
        gabi::call(0x02530634, this, &mEventData.field_0x28, STR(0x1004A8C8) /* "MoveBGCheck" */, 1);
    }
    u32 bg = (u32)(s32)mEventData.field_0x20;
    s32 trimSize = mEventData.field_0x1c;
    m068 = (bg >= 1 && bg <= 4) ? gabi::load<u8>(0x1004A8B3 + bg) : 9;
    mTrimSize = trimSize;
    if (m100 != 0 && m101 != 0 && m102 != 0) {
        setComStat(4);
    } else {
        clrComStat(4);
    }
    /* (this->*l_func[act])() */
    u32 e = 0x1004A8EC + act * 8;
    s16 vidx = gabi::load<s16>(e + 2);
    u32 self = ea() + gabi::load<s16>(e);
    bool done;
    if (vidx < 0) {
        done = gabi::call_ptr<bool>(gabi::load<u32>(e + 4), self);
    } else {
        u32 vtbl = gabi::load<u32>(self + gabi::load<s16>(e + 6));
        done = gabi::call_ptr<bool>(gabi::load<u32>(vtbl + vidx * 8 + 4), self);
    }
    if (done) {
        s32 idx = mEventData.mStaffIdx;
        gabi::call(0x02543280, dComIfGp_ea() + 0x52C4, idx); /* cutEnd */
    }
    if (mEventData.field_0x20 == 4) {
        gabi::Local<u8[0x6C]> linChk; /* dBgS_LinChk */
        u32 o = gabi::ea(linChk.get());
        gabi::call(0x02008FEC, o);
        gabi::store<u32>(o + 0x00, o + 0x58);
        gabi::store<u32>(o + 0x04, o + 0x64);
        gabi::store<u32>(o + 0x10, 0x1004A5EC);
        gabi::store<u32>(o + 0x20, 0x1004A5FC);
        gabi::store<u32>(o + 0x58, 0x1004A61C);
        gabi::store<u32>(o + 0x64, 0x1004A60C);
        gabi::store<u8>(o + 0x5C, 0);
        gabi::store<u8>(o + 0x5D, 0);
        gabi::store<u8>(o + 0x5E, 0);
        gabi::store<u8>(o + 0x5F, 0);
        gabi::store<u8>(o + 0x60, 0);
        gabi::store<u8>(o + 0x61, 0);
        gabi::store<u8>(o + 0x62, 0);
        gabi::store<u32>(o + 0x68, 1);
        if (lineBGCheck(&mViewCache.mCenter, &mViewCache.mEye, (u8*)linChk.get(), 4)) {
            mViewCache.mEye.copy(*gabi::at<cXyz>(o + 0x30)); /* lin_chk.GetCross() */
        }
        gabi::store<u32>(o + 0x58, 0x1004A61C);
        gabi::store<u32>(o + 0x64, 0x1004A4EC);
        gabi::store<u32>(o + 0x20, 0x1004A4DC);
        gabi::call(0x02008B4C, o, 0);
    }
    return true;
}
VERIFY(0x024FF164, &dCamera_c::eventCamera);
