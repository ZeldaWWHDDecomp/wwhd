/**
 * d_camera.cpp (WWHD)
 * Follow camera dCamera_c: accessors, Set/Reset, lock-on, blur and shake setters.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source. The other parts of the
 * translation unit are in d_camera_*.cpp.
 */
#include "d/d_camera_accessors.h"

static CameraHeaderStaticsCoreHD cameraHeaderStatics;
static CameraHeaderDescriptorsCoreHD cameraHeaderDescriptors;
static CameraHeaderZeroCoreHD cameraHeaderZero;
// HD: Reset copies the radius and the packed angle pair as words.
static inline void copyDirectionWords(cSGlobe_l& dst, const cSGlobe_l& src) {
    auto& d = reinterpret_cast<CameraGlobeWordsHD&>(dst);
    auto& s = reinterpret_cast<const CameraGlobeWordsHD&>(src);
    d.mRadius = s.mRadius.get();
    d.mAngles = s.mAngles.get();
}

/* 024F8000 */
static s16 dCam_getAngleY(camera_class* i_this) {
    dCamera_c* body = &gabi::at<CameraProcessCoreHD>(gabi::ea(i_this))->mCamera;
    return cSAngle_Inv(&body->mDirection.mU);
}

/* 024F8008 */
static s16 dCam_getAngleX(camera_class* i_this) {
    dCamera_c* body = &gabi::at<CameraProcessCoreHD>(gabi::ea(i_this))->mCamera;
    return body->mDirection.mV;
}

/* 024F8010 */
s16 dCamera_c::U2() { return mAngleY; }

/* 024F8018 */
static s16 dCam_getControledAngleY(camera_class* i_this) {
    return gabi::at<CameraProcessCoreHD>(gabi::ea(i_this))->mCamera.U2();
}

/* 024F8020 */
static camera_class* dCam_getCamera() { return dComIfGp_getCamera(0); }

/* 024F8044 */
static dCamera_c* dCam_getBody() {
    return &gabi::at<CameraProcessCoreHD>(gabi::ea(dCam_getCamera()))->mCamera;
}

/* 024F8D3C */
u8 dCamera_c::Active() { return mActive; }

/* 024F8D44 */
u8 dCamera_c::Pause() { return mPause; }

/* 024F8D4C */
static bool dCamForcusLine_Off(dCamForcusLine* i_this) {
    i_this->m49 = 0;
    return true; /* m49 == 0 */
}

/* cXyz copy construction into the return slot (allocates when the slot is NULL) */
// HD: a null caller-provided return slot requests allocation.
static inline cXyz* ret_cXyz(cXyz* ret, const cXyz* src) {
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(0xC);
        if (ret == nullptr)
            return ret;
    }
    ret->copy(*src); /* lfs/stfs pairs: bit-exact in the recompiled code */
    return ret;
}

/* 024F8D5C */
cXyz* dCamera_c::positionOf(cXyz* ret, fopAc_ac_c* actor) {
    return ret_cXyz(ret, &actor->current.pos);
}

/* 024F8F28 */
f32 dCamera_c::footHeightOf(fopAc_ac_c* actor) { return actor->current.pos.y; }

/* 024F8F30 */
cSAngle_l* dCamera_c::directionOf(cSAngle_l* ret, fopAc_ac_c* actor) {
    return camCore_angleFromShort(ret, (s16)actor->shape_angle.y); /* cSAngle(s16) */
}

/* 024F8F3C */
cXyz* dCamera_c::attentionPos(cXyz* ret, fopAc_ac_c* actor) {
    return ret_cXyz(ret, &actor->attention_info.position); /* attention_info.position */
}

/* 024FC0A8 */
bool dCamera_c::demoCamera(s32) { return true; }

/* 024FC0B0 */
cXyz* dCamera_c::eyePos(cXyz* ret, fopAc_ac_c* actor) { return ret_cXyz(ret, &actor->eyePos); }

/* 024FCBD4 */
f32 dCamera_c::radiusActorInSight(fopAc_ac_c* a1, fopAc_ac_c* a2) {
    return camCore_radiusActorInSight(this, a1, a2, &mViewCache.mCenter, &mViewCache.mEye,
                                      (f32)mFovy, (s16)mBank);
}

/* 024FE3D8 */
cSAngle_l* dCamera_c::getDMCAngle(cSAngle_l* ret, cSAngle_l* unused) {
    return camCore_angleCopy(ret, &mDMCSystem.field_0x2); /* cSAngle(const cSAngle&) */
}

/* 02502424 */
bool dCamera_c::letCamera(s32) { return true; }

/* 025028A0 */
static f32 limitf(f32 value, f32 min, f32 max) {
    if (value > max)
        return max;
    /* fsel: value < min ? min : value */
    return (min - value >= 0.0f) ? min : value;
}

/* 02508A58 */
cXyz* dCamera_c::positionPntOf(fopAc_ac_c* actor) { return &actor->current.pos; }

/* 0250D464 */
void dCamera_c::ResetBlure(s32 p) {
    m58C = p;
    mBlureAlpha = 0.75f;
    mBlurePositionType = 0;
    mBlurePosition.x = 0.5f;
    mBlurePosition.y = 0.5f;
    mBlurePosition.z = 0.0f;
    mBlureScale.x = 0.99f;
    mBlureScale.y = 0.99f;
    mBlureScale.z = 0.0f;
    mBlureRotation.x = 0;
    mBlureRotation.y = 0;
    mBlureRotation.z = 0;
    mBlureTimer = 0;
}

/* 0250D4C0 */
void dCamera_c::SetBlureTimer(s32 t) { mBlureTimer = t; }

/* 0250D4C8 */
void dCamera_c::SetBlureAlpha(f32 alpha) { mBlureAlpha = alpha; }

// SetBlureScale(f32)
/* 0250D4D0 */
void CameraBlureScaleHD::SetBlureScale(f32 s) {
    mBlureScale.x = s;
    mBlureScale.y = s;
    mBlureScale.z = 0.0f;
}

/* 02514CFC */
bool dCamera_c::followCamera2(s32 p) { return followCamera(p); }

// SetTypeForce(s32, fopAc_ac_c*)
/* 02514EB8 */
bool CameraTypeForceIdHD::SetTypeForce(s32 type, fopAc_ac_c* actor) {
    if (m524 == 0xFF) {
        m524 = type;
        m528 = actor;
        if (type != 0xFF)
            return true;
    }
    return false;
}

// SetTypeForce(char*, fopAc_ac_c*)
/* 02514EE4 */
void CameraTypeForceNameHD::SetTypeForce(const char* name, fopAc_ac_c* actor) {
    s32 type = GetCameraTypeFromCameraName(name);
    camCore_SetTypeForce(this, type, actor);
}

/* 02514F2C */
void dCamera_c::Stop() { mActive = 0; }

/* 02514F38 */
void dCamera_c::Start() { mActive = 1; }

/* 02514F44 */
void dCamera_c::Stay() { mPause = 1; }

// Set(cXyz center, cXyz eye)
/* 02514F50 */
bool CameraSetHD::Set(cXyz* center, cXyz* eye) {
    mCenter.copy(*center);
    mEye.copy(*eye);
    return true;
}

// Set(cXyz center, cXyz eye, f32 fovY, s16 bank)
/* 02514F88 */
bool CameraSetFovBankHD::Set(cXyz* center, cXyz* eye, f32 fovY, s16 bank) {
    mCenter.copy(*center);
    mEye.copy(*eye);
    mFovy = fovY;
    cSAngle_Val(&mBank, bank);
    return true;
}

// Set(cXyz center, cXyz eye, s16 bank, f32 fovY)
/* 02514FE8 */
bool CameraSetBankFovHD::Set(cXyz* center, cXyz* eye, s16 bank, f32 fovY) {
    mCenter.copy(*center);
    mEye.copy(*eye);
    mFovy = fovY;
    cSAngle_Val(&mBank, bank);
    return true;
}

/* 02515048 */
bool dCamera_c::Reset() {
    mViewCache.mCenter.copy(mCenter);
    mViewCache.mEye.copy(mEye);
    mViewCache.mFovy = (f32)mFovy;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&mEye, tmp, &mCenter);
    cSGlobe_Val(&mDirection, tmp);
    copyDirectionWords(mViewCache.mDirection, mDirection);
    mViewCache.mBank = (s16)mBank;
    cXyz_mi(&mViewCache.mEye, tmp, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, tmp);
    return true;
}

// Reset(cXyz center, cXyz eye)
/* 0251510C */
bool CameraResetHD::Reset(cXyz* center, cXyz* eye) {
    mCenter.copy(*center);
    mViewCache.mCenter.copy(mCenter);
    mEye.copy(*eye);
    mViewCache.mEye.copy(mEye);
    mViewCache.mFovy = (f32)mFovy;
    mViewCache.mBank = (s16)mBank;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&mViewCache.mEye, tmp, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, tmp);
    return dCamera_c::Reset();
}

// Reset(cXyz center, cXyz eye, f32 fovY, s16 bank)
/* 025151BC */
bool CameraResetFovBankHD::Reset(cXyz* center, cXyz* eye, f32 fovY, s16 bank) {
    mCenter.copy(*center);
    mViewCache.mCenter.copy(mCenter);
    mEye.copy(*eye);
    mFovy = fovY;
    mViewCache.mEye.copy(mEye);
    mViewCache.mFovy = fovY;
    gabi::Local<cSAngle_l> ang;
    gabi::Local<cXyz> tmp;
    cSAngle_l* a = camCore_angleFromShort(ang.get(), bank);
    u16 v = static_cast<u16>(a->get());
    mBank = (s16)v;
    mViewCache.mBank = (s16)v;
    cXyz_mi(&mViewCache.mEye, tmp, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, tmp);
    return dCamera_c::Reset();
}

/* 02515280 */
bool dCamera_c::SetTrimSize(s32 size) {
    mTrimSize = size;
    return true;
}

/* 0251528C */
bool dCamera_c::SetTrimTypeForce(s32 force) {
    mTrimTypeForce = force;
    return true;
}

// HD: both pattern buffers have five bytes. GameCube's 32-bit pattern wrote past
// the four-byte m544 into m548[0].
/* 02515298 */
s32 CameraAccessorsHD::StartShake(s32 i_length, u8* i_pattern, s32 i_flags, cXyz* i_pos) {
    u32 len = (u32)i_length;
    if (len > 0x20)
        len = 0x20; /* also catches negative lengths */
    m550 = (s32)len;
    auto& pattern = *reinterpret_cast<CameraShakePatternCoreHD*>(this);
    auto* input = reinterpret_cast<be<u8>*>(i_pattern);
    s32 i;
    for (i = 0; i < 5; i++) {
        pattern.m548[i] = 0;
        pattern.m544[i] = 0;
    }
    s32 n = (s32)len >> 3;
    for (i = 0; i < n; i++) {
        u8 v = input[i].get();
        pattern.m548[i] = v;
        pattern.m544[i] = v;
    }
    u32 r = len & 7;
    u32 m = input[i].get() & (0xFFu << (8 - r));
    pattern.m544[i] = (u8)m;
    if (len == 0x20)
        m |= (u32)input[0].get() >> r;
    pattern.m548[i] = (u8)m;
    gabi::Local<cXyz> nrm;
    camCore_cXyzNorm(i_pos, nrm.get()); /* cXyz::norm */
    m55C.copy(*nrm);
    m554 = 0;
    m588 = i_flags;
    return 1;
}

/* 025153D4 */
bool dCamera_c::StopShake() {
    m550 = 0;
    m588 = 0;
    m554 = 0;
    return true;
}

// SetBlureScale(f32, f32, f32)
/* 025153EC */
void CameraBlureScaleVectorHD::SetBlureScale(f32 x, f32 y, f32 z) {
    mBlureScale.x = x;
    mBlureScale.y = y;
    mBlureScale.z = z;
}

/* 025153FC */
void dCamera_c::SetBlurePositionType(s32 t) { mBlurePositionType = t; }

/* 02515404 */
void dCamera_c::SetBlurePosition(f32 x, f32 y, f32 z) {
    SetBlurePositionType(1);
    mBlurePosition.x = x;
    mBlurePosition.y = y;
    mBlurePosition.z = z;
}

/* 02515434 */
bool dCamera_c::SubjectLockOn(fopAc_ac_c* target) {
    mEventFlags = mEventFlags | 0x2800000; // HD: GameCube sets 0x3000000
    mpLockonTarget = target;
    return true;
}

/* 0251544C */
bool dCamera_c::SubjectLockOff() {
    mEventFlags = mEventFlags & ~0x2800000u; // HD: GameCube clears 0x3000000
    mpLockonTarget = nullptr;
    return true;
}

/* 024FA018 */
fopAc_ac_c* dCamera_c::GetForceLockOnActor() { return fopAcM_SearchByID(mLockOnActorId); }

/* 02515470 */
bool dCamera_c::ForceLockOn(u32 id) {
    mLockOnActorId = id;
    mForceLockTimer = 0;
    mpLockonActor = GetForceLockOnActor();
    return true;
}

/* 025052BC */
bool dCamera_c::ForceLockOff(u32 id) {
    if (id == mLockOnActorId || id == fpcM_ERROR_PROCESS_ID_e) {
        mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
        return true;
    }
    return false;
}

/* 025154B0 */
bool dCamera_c::ScopeViewMsgModeOff() {
    mEventFlags = mEventFlags & ~0x400000u;
    return true;
}

/* 025154C4 */
bool dCamera_c::SetExtendedPosition(cXyz* pos) {
    mExtendedPos.copy(*pos);
    return true;
}

/* 02500918 */
bool dCamera_c::Chtyp(s32 next) {
    if (onTypeChange(mCurType, next)) {
        mCurType = next;
        return true;
    }
    return false;
}

/* 0250235C */
static BOOL is_camera_delete(void*) { return TRUE; }

// HD: the window is looked up through the play object's camera table
/* 024F8B64 */
void dCamera_c::setView(f32 x, f32 y, f32 w, f32 h) {
    s32 camId = camCore_fopCamM_GetParam(mpCamera.get()); /* fopCamM_GetParam */
    u32 cameraInfo = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0]);
    s32 winId = gabi::at<dComIfG_camera_info_class>(cameraInfo + (u32)camId * 0x34)->mDlstWindowIdx;
    u32 windows = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mDlstWindow[0]);
    auto* win = gabi::at<CameraWindowCoreHD>(windows + (u32)winId * sizeof(CameraWindowCoreHD));
    f32 farZ = win->mViewport.mFarZ;
    f32 nearZ = win->mViewport.mNearZ;
    camCore_setViewPort(win, x, y, w, h, nearZ, farZ);
    camCore_setScissor(win, x, y, w, h);
}

// HD: ResetView restores a 1280x720 viewport and the supplied view's center, eye, bank
// and fovy. GameCube only resets its 640x480 viewport.
/* 024F8C54 */
void CameraAccessorsHD::ResetView(CameraRestoreCoreHD* view) {
    setView(0.0f, 0.0f, 1280.0f, 720.0f);
    cXyz* center = &view->mLookat.mCenter;
    cXyz* eye = &view->mLookat.mEye;
    mViewCache.mCenter.copy(*center);
    mCenter.copy(mViewCache.mCenter);
    mViewCache.mEye.copy(*eye);
    mEye.copy(mViewCache.mEye);
    gabi::Local<cSAngle_l> ang;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&mViewCache.mEye, tmp, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, tmp);
    cSAngle_l* a = camCore_angleFromShort(ang.get(), view->mBank.get());
    u16 b = static_cast<u16>(a->get());
    mViewCache.mBank = (s16)b;
    mBank = (s16)b;
    f32 fovy = view->mFovy.get();
    mFovy = fovy;
    mViewCache.mFovy = fovy;
}

/* 024F7DF4 */
static bool limited_range_addition(be<f32>* p, f32 add, f32 lo, f32 hi) {
    f32 cur = *p;
    f32 min = lo;
    f32 max = hi;
    if (lo > hi) {
        add = -add;
        min = hi;
        max = lo;
    }
    f32 v = gabi::fadds_ppc(cur, add);
    if (v < min) {
        *p = min;
        return false;
    }
    if (v > max) {
        *p = max;
        return false;
    }
    *p = v;
    return true;
}

// HD: compiler-generated (header statics of the translation unit)
/* 025154E4 */
static void __sinit_d_camera_cpp() {
    sinit_header_statics_z(gabi::ea(&cameraHeaderStatics), gabi::ea(&cameraHeaderDescriptors),
                           gabi::ea(&cameraHeaderZero));
}

// HD: compiler-generated deleting destructor (header SafeString)
/* 02515578 */
static void d_camera_string_destructor(void* p, u32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: compiler-generated constructor of the m0A4[] elements (allocates when NULL)
/* 0251558C */
static dCamera_c::PosSet* dCamera_PosSet_ct(dCamera_c::PosSet* p) {
    if (p == nullptr) {
        p = (dCamera_c::PosSet*)operator_new(0x20);
        if (p == nullptr)
            return p;
    }
    cSAngle_ct(&p->mBank);
    return p;
}

// HD: compiler-generated empty function (header SafeString)
/* 025155D4 */
static void d_camera_string_terminate(void*) {}

// HD: per-TU copy of sead::Vector3f::normalize (returns the length)
/* 025155D8 */
static f32 cameraVectorNormalize(cXyz* v) {
    f32 len = camCore_PSVECMag(v); /* PSVECMag */
    if (len > 0.0f) {
        f32 inv = 1.0f / len;
        v->x = v->x * inv;
        v->y = v->y * inv;
        v->z = v->z * inv;
    }
    return len;
}

// HD: per-TU copy of sead::Quatf::setAxisAngle
/* 02515644 */
static void cameraQuaternionSetAxisAngle(CameraQuaternionCoreHD* q, cXyz* axis, f32 angle) {
    f32 half = angle * 0.5f;
    f32 c = camCore_cosf(half); /* cosf */
    f32 s = camCore_sinf(half); /* sinf */
    f32 x = s * axis->x;
    f32 y = s * axis->y;
    f32 z = s * axis->z;
    q->w = c;
    q->y = y;
    q->x = x;
    q->z = z;
}

// HD: per-TU copy of sead::Matrix33f::fromQuat
/* 025156EC */
static void cameraMatrixFromQuaternion(CameraMatrixCoreHD* m, CameraQuaternionCoreHD* q) {
    f32 y = q->y;
    f32 z = q->z;
    f32 y2 = y + y;
    f32 w = q->w;
    f32 z2 = z + z;
    f32 x = q->x;
    f32 w2 = w + w;
    f32 x2 = x + x;
    f32 yy = y2 * y;
    f32 wx = w2 * x;
    f32 xy = x2 * y;
    f32 zz = z2 * z;
    f32 yz = y2 * z;
    f32 oneMinusXx = gabi::fnmsubs(x2, x, 1.0f);
    f32 wz = w2 * z;
    f32 wy = w2 * y;
    f32 xz = x2 * z;
    m->m[0][0] = (1.0f - yy) - zz;
    m->m[0][2] = xz + wy;
    m->m[0][1] = xy - wz;
    m->m[1][0] = xy + wz;
    m->m[1][2] = yz - wx;
    m->m[1][1] = oneMinusXx - zz;
    m->m[2][2] = oneMinusXx - yy;
    m->m[2][0] = xz - wy;
    m->m[2][1] = yz + wx;
}

// HD: per-TU copy of a cXyz array element copy (out = array[idx])
/* 02515788 */
static void cXyz_array_get(cXyz* out, cXyz* array, s32 idx) {
    // HD: retain 32-bit guest address arithmetic for unchecked indices.
    cXyz* src = gabi::at<cXyz>(gabi::ea(array) + (u32)idx * sizeof(cXyz));
    out->copy(*src);
}

/* WWHD glue
data cameraHeaderStatics 1046EECC
data cameraHeaderDescriptors 101D55CC
data cameraHeaderZero 1046EF68
ext 0200658C cSAngle_l* camCore_angleFromShort(cSAngle_l*, s16)
ext 02006644 cSAngle_l* camCore_angleCopy(cSAngle_l*, const cSAngle_l*)
ext 024FC780 f32 camCore_radiusActorInSight(dCamera_c*, fopAc_ac_c*, fopAc_ac_c*, cXyz*, cXyz*, f32, s16)
ext 02514EB8 bool camCore_SetTypeForce(dCamera_c*, s32, fopAc_ac_c*)
ext 0201B084 void camCore_cXyzNorm(cXyz*, cXyz*)
ext 025DA64C s32 camCore_fopCamM_GetParam(camera_class*)
ext 0252CC8C void camCore_setViewPort(CameraWindowCoreHD*, f32, f32, f32, f32, f32, f32)
ext 0252CCA8 void camCore_setScissor(CameraWindowCoreHD*, f32, f32, f32, f32)
ext 028E8E10 f32 camCore_PSVECMag(cXyz*)
ext 028F4BE0 f32 camCore_cosf(f32)
ext 028F43F8 f32 camCore_sinf(f32)
*/
