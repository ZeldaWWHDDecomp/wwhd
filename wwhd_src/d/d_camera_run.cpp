/**
 * d_camera_run.cpp (WWHD)
 * Follow camera dCamera_c::Run: the per-frame update (type/mode/style selection, engine dispatch,
 * smoothing, up vector, sounds and attention status).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_run.h"

namespace camera_run {
static DemoPointer demoObject;
static PadPointer padObject;
static RunName assertFile;
static RunName assertDemo;
static RunName tornadoName;
static RunName cameraName;
static RunName trimName;
static RunName trimDefault;
static RunName waitAnyKeyName;
static RunName bgCheckName;
static RunName autoForcusName;
static RunName moveBgCheckName;
static EventActions ActionNames;
static BackgroundMasks backgroundMasks;
static f32 envZoom() {
    return cameraRunEnv()->mHDZoom;
}
static u32 readFloatBits(GuestFloat* value) {
    return (u32) * reinterpret_cast<GuestWord*>(value);
}
static void writeFloatBits(GuestFloat* value, u32 bits) {
    *reinterpret_cast<GuestWord*>(value) = bits;
}
static u32 callEngine(dCamera_c* camera, u32 algorithm, s32 style) {
    EngineEntry* entry = runEngineEntry(algorithm);
    s16 index = entry->mVtableIndex;
    u8* self = adjustThis(camera, entry->mThisDelta);
    if (index < 0)
        return gabi::call_ptr<u32>(entry->mFunction, self, style);
    EngineObject* object = engineObject(self, entry->mVtableOffset);
    return gabi::call_ptr<u32>(object->mpVtable[index].mFunction.get(), self, style);
}
static bool callEventEngine(dCamera_c* camera, u32 action) {
    EngineEntry* entry = eventEngineEntry(action);
    s16 index = entry->mVtableIndex;
    u8* self = adjustThis(camera, entry->mThisDelta);
    if (index < 0)
        return gabi::call_ptr<bool>(entry->mFunction, self);
    EngineObject* object = engineObject(self, entry->mVtableOffset);
    return gabi::call_ptr<bool>(object->mpVtable[index].mFunction.get(), self);
}
} // namespace camera_run
using namespace camera_run;
/* 024FE3E8 */
// HD: the field of view is scaled by an environment-light zoom factor (0.7 / 1.0 /
// eased to 0.8 or 1.0 by the light's state byte); the floor clamp keeps the subject camera's
// centre except in the "Tornado" type; switching to a subject style calls 026185BC; the pad is
// also updated in minigame 8; m514 is a byte
u32 dCamera_c::Run() {
    u32 res = 0;                     /* the engine's r3, returned unmasked */
    cameraRunFocusOff(&mForcusLine); /* mForcusLine.Off() */
    mEventFlags = mEventFlags & ~0x10149C01u;
    checkSpecialArea();
    checkGroundInfo();
    s32 ship = m530;
    if (ship != 0 && !chkFlag(0x200000) && !cameraRunIsPlaying() && !chkFlag(0x20000000)) {
        f32 ofs = cameraRunShipOffsetY(&m534, &m536, 130.0f); /* daObjPirateship::getShipOffsetY */
        f32 y = ofs * m540;
        f32 dy = y - m538;
        if (m530 == 1 && m53C < 0.0f && dy > 0.0f)
            m254 = m254 | 4;
        f32 cy = mViewCache.mCenter.y;
        f32 k = (f32)cameraRunSetup(this)->m0B8; /* mCamSetup.m0B8 */
        m53C = dy;
        m538 = y;
        mViewCache.mCenter.y = gabi::fnmsubs(dy, k, cy);
    }
    updateMonitor();
    Att();
    clrComStat(dCamAttnStts_00000400_e | dCamAttnStts_00001000_e | dCamAttnStts_00002000_e);
    if (!cameraRunIsPlaying() && !chkFlag(0x20000000)) {
        updatePad();
        cameraRunCstickShift(cameraRunSetup(this)->mCstick,
                             mPadId); /* mCamSetup.mCstick.Shift(mPadId) */
    }
    if (dComIfGp_getMiniGameType() == 8) { /* dComIfGp_getMiniGameType() */
        updatePad();
        cameraRunCstickShift(cameraRunSetup(this)->mCstick, mPadId);
    }
    dAttention_c* attn = dComIfGp_getAttention();
    if (cameraRunLockonTruth(attn) ||
        ((u32) reinterpret_cast<AttentionView*>(attn)->mFlags & 0x20000000)) {
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
        if (onTypeChange(mCurType, next))
            mCurType = (s32)mNextType;
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
        if (style < 0)
            styleOk = false;
    }
    if (styleOk && (u32)(s32)mCurStyle != (u32)style) {
        if (onStyleChange(mCurStyle, style)) {
            mCurStyle = (s32)camTypeStyle(mCurType, mCurMode);
            cameraRunParamChange(mCamParam, mCurStyle); /* mCamParam.Change(mCurStyle) */
            if (camStyleAlg(mCurStyle) == dCamAlg_SUBJECT_CAMERA_e /* SUBJECT */) {
                cameraRunSubjectStart(padObject);
                m154 = 0.0f;
            }
        }
    }
    u32 flags = mEventFlags & ~0x20u;
    s32 m = mCurMode;
    mEventFlags = flags;
    if (m == 12)
        flags |= 0x20;
    mEventFlags = flags & 0x7FFFFFFF;
    clrComStat(dCamAttnStts_00000080_e);

    bool fwd = false;
    if (camStyleFlags() & 4) {
        s32 pad = mPadId;
        if (!(dComIfGp_getPlayerStatus0(pad) & daPyStts0_UNK4000000_e)) { /* check_owner_action */
            pad = mPadId;
            if (!(dComIfGp_getPlayerStatus1(pad) & daPyStts1_UNK40000_e))
                fwd = true; /* check_owner_action1 */
        }
    }
    if (fwd) {
        cSAngle_l a;
        cSAngle_l d;
        cSAngle_l s;
        forwardCheckAngle(&a);                                          /* forwardCheckAngle() */
        cameraRunAngleSubtract(&a, &d, &m148);                          /* - m148 */
        cameraRunAngleScale(&d, &s, cameraRunSetup(this)->mFwdCushion); /* * FwdCushion */
        cameraRunAngleAddAssign(&m148, &s);                             /* m148 += */
    } else {
        m148 = (s16)zeroAngle(); /* cSAngle::_0 */
    }
    defaultTriming();
    mTrimTypeForce = -1;
    m068 = 9;

    if (chkFlag(0x200000) && camStyleAlg(mCurStyle) != dCamAlg_EVENT_CAMERA_e /* EVENT */) {
        s32 pad = mPadId;
        if (cameraRunStickValue(pad) > 0.001f || cameraRunSubstickValue(pad) > 0.001f ||
            cameraRunTrigger(pad) != 0 || mMonitor.field_0x0C.x > 10.0f || m360 == 0 || m31C != 0) {
            *reinterpret_cast<GuestByte*>(&m514) = 0; /* m514 */
            mEventFlags = mEventFlags & ~0x200000u;
        } else {
            *reinterpret_cast<GuestByte*>(&m514) = 0;
        }
    } else {
        DemoToken* demo = demoObject;
        if (demo == 0) {
            JUT_ASSERT_fail(assertFile.text, 0x23E, assertDemo.text);
            demo = demoObject;
        }
        if (cameraRunActiveDemoCamera(demo) != 0 &&
            (u32) reinterpret_cast<ParamView*>(mCamParam)->mpStyle->mAlgorithm !=
                dCamAlg_EVENT_CAMERA_e) {
            res = cameraRunDemoCamera(this, 0); /* demoCamera(0) */
        } else {
            res = callEngine(this, camStyleAlg(mCurStyle), mCurStyle);
            m07C = m07C + 1;
            m11C = m11C + 1;
            m118 = m118 + 1;
            m080 = m080 + 1;
            m108 = m108 + 1;
        }
        if (!res)
            *reinterpret_cast<GuestByte*>(&m514) = 0;
    }

    if (!chkFlag(0x400)) {
        cSAngle_l t;
        cameraRunAngleScale(&mViewCache.mBank, &t, 0.05f);   /* mBank * 0.05 */
        cameraRunAngleSubtractAssign(&mViewCache.mBank, &t); /* mBank -= */
    }
    cameraRunShake(this); /* shakeCamera() */
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
    u32 cyBits = readFloatBits(&mViewCache.mCenter.y); /* mViewCache.mCenter.y (copied bit-exact) */
    f32 cy = std::bit_cast<f32>(cyBits);
    f32 margin = (f32)cameraRunSetup(this)->mFloorMargin; /* mCamSetup.mBGChk.FloorMargin() */
    if (pf & 0x400)
        m068 = m068 | 0x40;
    floorY = floorY + margin;
    writeFloatBits(&mCenter.x,
                   readFloatBits(&mViewCache.mCenter.x)); /* mCenter.x = mViewCache.mCenter.x */
    if (cy < floorY) {
        writeFloatBits(&mCenter.z, readFloatBits(&mViewCache.mCenter.z)); /* mCenter.z */
        if (camStyleAlg(mCurStyle) == dCamAlg_SUBJECT_CAMERA_e && (mEventFlags & 0x10000800) &&
            GetCameraTypeFromCameraName(tornadoName.text /* "Tornado" */) != (s32)mCurType) {
            m068 = m068 & ~8u;
            writeFloatBits(&mCenter.y, readFloatBits(&mViewCache.mCenter.y));
        } else {
            mCenter.y = floorY;
        }
    } else {
        writeFloatBits(&mCenter.y, cyBits);
        writeFloatBits(&mCenter.z, readFloatBits(&mViewCache.mCenter.z));
    }

    // HD: environment-light zoom factor
    EnvLightView* el = cameraRunEnv();
    if ((u8)el->mHDZoomState == 2) {
        el = cameraRunEnv();
        el->mHDZoom = 0.7f;
    } else {
        el = cameraRunEnv();
        if ((u8)el->mHDZoomState == 0xFF) {
            el = cameraRunEnv();
            el->mHDZoom = 1.0f;
        } else {
            el = cameraRunEnv();
            f32 target = (u8)el->mHDZoomState != 0 ? 0.8f : 1.0f;
            el = cameraRunEnv();
            cameraRunAddCalc(&el->mHDZoom, target, 0.05f, 0.05f, 0.01f); /* cLib_addCalc */
        }
    }
    f32 zoom = envZoom();
    s16 bank = mViewCache.mBank;
    mBank = bank;
    mFovy = gabi::fmuls_ppc(mViewCache.mFovy, zoom);
    cameraRunBumpCheck(this, m068); /* bumpCheck(m068) */

    cSAngle_l stick;
    cSAngle_l diff;
    cSAngle_l lim1;
    cSAngle_l lim2;
    cSAngle_l inv;
    cSAngle_l stick2;
    cSAngle_l dmc;
    cSAngle_l vlo;
    cSAngle_l vhi;
    cSAngle_l* sa = cameraRunAngleShortCtor(&stick, CPad_GET_STICK_ANGLE(mPadId));
    cameraRunAngleSubtract(sa, &diff, &mDMCSystem.field_0x4);
    bool reset = false;
    if (mStickMainValueLast < (f32)cameraRunSetup(this)->mDMCValue /* DMCValue */) {
        reset = true;
    } else {
        cSAngle_l* hi =
            cameraRunAngleFloatCtor(&lim1, cameraRunSetup(this)->mDMCAngle); /* DMCAngle */
        if ((s16)diff > (s16)*hi) {
            reset = true;
        } else {
            cSAngle_l* lo = cameraRunAngleFloatCtor(&lim2, -(f32)cameraRunSetup(this)->mDMCAngle);
            if ((s16)diff < (s16)*lo)
                reset = true;
        }
    }
    s16 angleY;
    if (reset)
        mDMCSystem.field_0x0 = 0;
    if (!reset && mDMCSystem.field_0x0 != 0) {
        cSAngle_l* s2 = cameraRunAngleShortCtor(&stick2, CPad_GET_STICK_ANGLE(mPadId));
        getDMCAngle(&dmc, s2);
        angleY = dmc;
    } else {
        s16 v = cSAngle_Inv(&mDirection.mU);
        cSAngle_l* a = cameraRunAngleShortCtor(&inv, v);
        angleY = *a;
    }
    mAngleY = angleY;

    if (mCenter.x == mEye.x && mCenter.z == mEye.z) {
        mUp.z = 0.0f;
        mUp.y = 1.0f;
        mUp.x = 0.01f;
    } else {
        cSAngle_l* lo = cameraRunAngleFloatCtor(&vlo, -90.0f);
        bool down = true;
        if (!((s16)mDirection.mV < (s16)*lo)) {
            cSAngle_l* hi = cameraRunAngleFloatCtor(&vhi, 90.0f);
            if (!((s16)mDirection.mV > (s16)*hi))
                down = false;
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
            cameraRunSoundStart(m248[i]); /* mDoAud_seStart(m248[i]) */
            cur = (u32)m254;
        }
    }
    m258 = cur;
    m254 = 0;

    if (m100 != 0 && m101 != 0 && m102 != 0) {
        setComStat(dCamAttnStts_00000010_e);
    } else {
        clrComStat(dCamAttnStts_00000010_e);
    }
    u32 f = mEventFlags;
    if (f & 0x40000) {
        setComStat(dCamAttnStts_SUBJECT_e);
    } else if (mDirection.mRadius <
               (f32)cameraRunSetup(this)->mPlayerHideDist /* PlayerHideDist */) {
        if (f & 0x800) {
            setComStat(dCamAttnStts_SUBJECT_e);
            f = mEventFlags;
        }
        if (f & 0x10000000)
            setComStat(dCamAttnStts_00000020_e);
    }
    return res;
}

/* 024FF164 */
// HD: the "AutoForcus" event data no longer switches auto focus off; leaving a
// subject->event style (mHD610) is cancelled by any action other than PAUSE/WAIT
bool dCamera_c::eventCamera(s32) {
    if (m118 == 0) {
        PosSet* p0 = &m0A4[0];
        PosSet* p1 = &m0A4[1];
        u32 fovy = readFloatBits(&mFovy);
        writeFloatBits(&p1->mFovY, fovy);
        p0->mEye.copy(mEye);
        p1->mEye.copy(mEye);
        u16 bank = (u16)(s16)mBank;
        p1->mBank = (s16)bank;
        p0->mBank = (s16)bank;
        p0->mCenter.copy(mCenter);
        p1->mCenter.copy(mCenter);
        p0->m1E = 0;
        writeFloatBits(&p0->mFovY, fovy);
        p1->m1E = 0;
    }
    s32 act;
    if (!chkFlag(0x20000000)) {
        if (!cameraRunIsPlaying())
            return false; /* dComIfGp_evmng_cameraPlay() */
        s32 staff = dComIfGp_evmng_getMyStaffId(cameraName.text, nullptr, 0); /* getMyStaffId */
        if (staff < 0)
            return false;
        if ((u32)mEventData.mStaffIdx != (u32)staff) {
            m108 = 0;
            m11C = 0;
            mEventFlags = mEventFlags & ~0x200000u;
            m118 = 0;
        }
        mEventData.mStaffIdx = staff;
        if (dComIfGp_evmng_getIsAddvance(staff)) { /* getIsAddvance */
            m101 = 0;
            m100 = 0;
            m102 = 0;
            m11C = 0;
        }
        s32 idx = mEventData.mStaffIdx;
        act = dComIfGp_evmng_getMyActIdx(idx, ActionNames.mNames, EVENT_ACTION_COUNT, 0,
                                         0); /* getMyActIdx */
    } else {
        mEventData.mStaffIdx = -1;
        if (m118 == 0)
            m11C = 0;
        act = mEventData.field_0x18;
    }
    if ((u32)act >= EVENT_ACTION_COUNT) {
        s32 idx = mEventData.mStaffIdx;
        dComIfGp_evmng_cutEnd(idx); /* cutEnd */
        return false;
    }
    if (m11C == 0) {
        if (m118 == 0) {
            mEventData.field_0x1c = 2;
            mEventFlags = mEventFlags & ~0x200000u;
        }
        if (mHD610 != 0 && act != EVENT_PAUSE && act != EVENT_WAIT)
            mHD610 = 0;
        EventString trim;
        if (cameraRunGetString(this, &trim, trimName.text, trimDefault.text)) {
            u32 w = (u32)trim.mPrefix;
            if (w == TRIM_STANDARD)
                mEventData.field_0x1c = 0; /* 'STAN' */
            else if (w == TRIM_VISTA)
                mEventData.field_0x1c = 1; /* 'VIST' */
            else if (w == TRIM_DEMO)
                mEventData.field_0x1c = 3; /* 'DEMO' */
            else if (w == TRIM_NONE)
                mEventData.field_0x1c = 4; /* 'NONE' */
            else if (w == TRIM_KEEP)
                mEventData.field_0x1c = 999; /* 'KEEP' */
        }
        cameraRunGetInt(this, &mEventData.field_0x24, waitAnyKeyName.text /* "WaitAnyKey" */,
                        0); /* getEvIntData */
        if (mEventData.field_0x24 != 0)
            mEventFlags = mEventFlags | 0x200000;
        cameraRunGetInt(this, &mEventData.field_0x20, bgCheckName.text /* "BGCheck" */, 1);
        cameraRunGetInt(this, &mEventData.field_0x24, autoForcusName.text /* "AutoForcus" */, 1);
        cameraRunGetInt(this, &mEventData.field_0x28, moveBgCheckName.text /* "MoveBGCheck" */, 1);
    }
    u32 bg = (u32)(s32)mEventData.field_0x20;
    s32 trimSize = mEventData.field_0x1c;
    m068 = (bg >= 1 && bg <= 4) ? (u8)backgroundMasks.mValues[bg] : 9;
    mTrimSize = trimSize;
    if (m100 != 0 && m101 != 0 && m102 != 0) {
        setComStat(dCamAttnStts_00000004_e);
    } else {
        clrComStat(dCamAttnStts_00000004_e);
    }
    bool done = callEventEngine(this, act);
    if (done) {
        s32 idx = mEventData.mStaffIdx;
        dComIfGp_evmng_cutEnd(idx); /* cutEnd */
    }
    if (mEventData.field_0x20 == 4) {
        dBgS_LinChk linChk;
        cameraRunLineCtor(&linChk);
        linChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&linChk));
        linChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&linChk));
        linChk.cBgS_Chk::__vtbl = LINE_CHECK;
        linChk.cBgS_PolyInfo::__vtbl = LINE_POLY_INFO;
        linChk.dBgS_PolyPassChk::__vtbl = LINE_POLY_PASS;
        linChk.dBgS_GrpPassChk::__vtbl = LINE_GROUP_PASS;
        linChk.mbObjThrough = 0;
        linChk.mbCamThrough = 0;
        linChk.mbLinkThrough = 0;
        linChk.mbArrowThrough = 0;
        linChk.mbBombThrough = 0;
        linChk.mbBoomerangThrough = 0;
        linChk.mbRopeThrough = 0;
        linChk.mGrp = 1;
        if (lineBGCheck(&mViewCache.mCenter, &mViewCache.mEye, reinterpret_cast<u8*>(&linChk), 4))
            mViewCache.mEye.copy(linChk.mLin.mEnd);
        linChk.dBgS_PolyPassChk::__vtbl = LINE_POLY_PASS;
        linChk.dBgS_GrpPassChk::__vtbl = BASE_GROUP_PASS;
        linChk.cBgS_PolyInfo::__vtbl = BASE_POLY_INFO;
        cameraRunLineDtor(&linChk, 0);
    }
    return true;
}

/* WWHD glue (tools/verify/autoglue.py)
data demoObject 101D5FFC
data padObject 101F5088
data assertFile 1004A898
data assertDemo 1004A888
data tornadoName 1004A880
data cameraName 1004A8B8
data trimName 1004A8C0
data trimDefault 1004A8A4
data waitAnyKeyName 1004A8D4
data bgCheckName 1004A8AC
data autoForcusName 1004A8E0
data moveBgCheckName 1004A8C8
data ActionNames 101D5550
data backgroundMasks 1004A8B3
ext 024F8D4C bool cameraRunFocusOff(dCamForcusLine*)
ext 025269A8 f32 cameraRunShipOffsetY(be<s16>*,be<s16>*,f32)
ext 024F6E54 void cameraRunCstickShift(u8*,s32)
ext 024EDFCC bool cameraRunLockonTruth(dAttention_c*)
ext 024F727C void cameraRunParamChange(u8*,s32)
ext 026185BC void cameraRunSubjectStart(PadToken*)
ext 020068B0 void cameraRunAngleSubtract(cSAngle_l*,cSAngle_l*,cSAngle_l*)
ext 0200693C void cameraRunAngleScale(cSAngle_l*,cSAngle_l*,f32)
ext 020068CC void cameraRunAngleAddAssign(cSAngle_l*,cSAngle_l*)
ext 020079B4 f32 cameraRunStickValue(s32)
ext 02007B44 f32 cameraRunSubstickValue(s32)
ext 020075C0 u32 cameraRunTrigger(s32)
ext 025283F8 u32 cameraRunActiveDemoCamera(DemoToken*)
ext 024FC0A8 u32 cameraRunDemoCamera(dCamera_c*,s32)
ext 020068E0 void cameraRunAngleSubtractAssign(cSAngle_l*,cSAngle_l*)
ext 024FC108 f32 cameraRunShake(dCamera_c*)
ext 0200ECD4 f32 cameraRunAddCalc(GuestFloat*,f32,f32,f32,f32)
ext 024FD11C bool cameraRunBumpCheck(dCamera_c*,u32)
ext 0200658C cSAngle_l* cameraRunAngleShortCtor(cSAngle_l*,s32)
ext 020066C0 cSAngle_l* cameraRunAngleFloatCtor(cSAngle_l*,f32)
ext 025E1988 void cameraRunSoundStart(s32)
ext 02530998 bool cameraRunGetString(dCamera_c*,EventString*,const char*,const char*)
ext 02530634 void cameraRunGetInt(dCamera_c*,be<s32>*,const char*,s32)
ext 02008FEC void cameraRunLineCtor(dBgS_LinChk*)
ext 02008B4C void cameraRunLineDtor(dBgS_LinChk*,s32)
*/
