/**
 * d_camera.cpp (WWHD)
 * Follow camera dCamera_c: accessors, Set/Reset, lock-on, blur and shake setters.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source. The other parts of the
 * translation unit are in d_camera_*.cpp.
 */
#include "d/d_camera.h"

/* 024F8000 */
static s16 dCam_getAngleY(camera_class* i_this) {
    WWHD_FUNC(0x024F8000, s16, i_this);
    dCamera_c* body = gabi::at<dCamera_c>(gabi::ea(i_this) + CAMERA_BODY);
    return cSAngle_Inv(&body->mDirection.mU);
}
VERIFY(0x024F8000, dCam_getAngleY);

/* 024F8008 */
static s16 dCam_getAngleX(camera_class* i_this) {
    WWHD_FUNC(0x024F8008, s16, i_this);
    dCamera_c* body = gabi::at<dCamera_c>(gabi::ea(i_this) + CAMERA_BODY);
    return body->mDirection.mV;
}
VERIFY(0x024F8008, dCam_getAngleX);

/* 024F8010 */
s16 dCamera_c::U2() {
    WWHD_FUNC(0x024F8010, s16, this);
    return mAngleY;
}
VERIFY(0x024F8010, &dCamera_c::U2);

/* 024F8018 */
static s16 dCam_getControledAngleY(camera_class* i_this) {
    WWHD_FUNC(0x024F8018, s16, i_this);
    return gabi::at<dCamera_c>(gabi::ea(i_this) + CAMERA_BODY)->U2();
}
VERIFY(0x024F8018, dCam_getControledAngleY);

/* 024F8020 */
static camera_class* dCam_getCamera() {
    WWHD_FUNC(0x024F8020, camera_class*);
    return gabi::at<camera_class>(gabi::load<u32>(dComIfGp_ea() + 0x5AF8)); /* dComIfGp_getCamera(0) */
}
VERIFY(0x024F8020, dCam_getCamera);

/* 024F8044 */
static dCamera_c* dCam_getBody() {
    WWHD_FUNC(0x024F8044, dCamera_c*);
    return gabi::at<dCamera_c>(gabi::ea(dCam_getCamera()) + CAMERA_BODY);
}
VERIFY(0x024F8044, dCam_getBody);

/* 024F8D3C */
u8 dCamera_c::Active() {
    WWHD_FUNC(0x024F8D3C, u8, this);
    return mActive;
}
VERIFY(0x024F8D3C, &dCamera_c::Active);

/* 024F8D44 */
u8 dCamera_c::Pause() {
    WWHD_FUNC(0x024F8D44, u8, this);
    return mPause;
}
VERIFY(0x024F8D44, &dCamera_c::Pause);

/* 024F8D4C */
static bool dCamForcusLine_Off(dCamForcusLine* i_this) {
    WWHD_FUNC(0x024F8D4C, bool, i_this);
    i_this->m49 = 0;
    return true; /* m49 == 0 */
}
VERIFY(0x024F8D4C, dCamForcusLine_Off);

/* cXyz copy construction into the return slot (allocates when the slot is NULL) */
static cXyz* ret_cXyz(cXyz* ret, const cXyz* src) {
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(0xC);
        if (ret == nullptr) return ret;
    }
    ret->copy(*src); /* lfs/stfs pairs: bit-exact in the recompiled code */
    return ret;
}

/* 024F8D5C */
cXyz* dCamera_c::positionOf(cXyz* ret, fopAc_ac_c* actor) {
    WWHD_FUNC(0x024F8D5C, cXyz*, this, ret, actor);
    return ret_cXyz(ret, &actor->current.pos);
}
VERIFY(0x024F8D5C, &dCamera_c::positionOf);

/* 024F8F28 */
f32 dCamera_c::footHeightOf(fopAc_ac_c* actor) {
    WWHD_FUNC(0x024F8F28, f32, this, actor);
    return actor->current.pos.y;
}
VERIFY(0x024F8F28, &dCamera_c::footHeightOf);

/* 024F8F30 */
cSAngle_l* dCamera_c::directionOf(cSAngle_l* ret, fopAc_ac_c* actor) {
    WWHD_FUNC(0x024F8F30, cSAngle_l*, this, ret, actor);
    return gabi::call<cSAngle_l*>(0x0200658C, ret, (s16)actor->shape_angle.y); /* cSAngle(s16) */
}
VERIFY(0x024F8F30, &dCamera_c::directionOf);

/* 024F8F3C */
cXyz* dCamera_c::attentionPos(cXyz* ret, fopAc_ac_c* actor) {
    WWHD_FUNC(0x024F8F3C, cXyz*, this, ret, actor);
    return ret_cXyz(ret, gabi::at<cXyz>(gabi::ea(actor) + 0x390)); /* attention_info.position */
}
VERIFY(0x024F8F3C, &dCamera_c::attentionPos);

/* 024FC0A8 */
bool dCamera_c::demoCamera(s32) {
    WWHD_FUNC(0x024FC0A8, bool, this, 0);
    return true;
}
VERIFY(0x024FC0A8, &dCamera_c::demoCamera);

/* 024FC0B0 */
cXyz* dCamera_c::eyePos(cXyz* ret, fopAc_ac_c* actor) {
    WWHD_FUNC(0x024FC0B0, cXyz*, this, ret, actor);
    return ret_cXyz(ret, &actor->eyePos);
}
VERIFY(0x024FC0B0, &dCamera_c::eyePos);

/* 024FCBD4 */
f32 dCamera_c::radiusActorInSight(fopAc_ac_c* a1, fopAc_ac_c* a2) {
    WWHD_FUNC(0x024FCBD4, f32, this, a1, a2);
    return gabi::call<f32>(0x024FC780, this, a1, a2, &mViewCache.mCenter, &mViewCache.mEye, (f32)mFovy,
                           (s16)mBank);
}
VERIFY(0x024FCBD4, &dCamera_c::radiusActorInSight);

/* 024FE3D8 */
cSAngle_l* dCamera_c::getDMCAngle(cSAngle_l* ret, cSAngle_l* unused) {
    WWHD_FUNC(0x024FE3D8, cSAngle_l*, this, ret, unused);
    return gabi::call<cSAngle_l*>(0x02006644, ret, &mDMCSystem.field_0x2); /* cSAngle(const cSAngle&) */
}
VERIFY(0x024FE3D8, &dCamera_c::getDMCAngle);

/* 02502424 */
bool dCamera_c::letCamera(s32) {
    WWHD_FUNC(0x02502424, bool, this, 0);
    return true;
}
VERIFY(0x02502424, &dCamera_c::letCamera);

/* 025028A0 */
static f32 limitf(f32 value, f32 min, f32 max) {
    WWHD_FUNC(0x025028A0, f32, value, min, max);
    if (value > max) return max;
    /* fsel: value < min ? min : value */
    return (min - value >= 0.0f) ? min : value;
}
VERIFY(0x025028A0, limitf);

/* 02508A58 */
cXyz* dCamera_c::positionPntOf(fopAc_ac_c* actor) {
    WWHD_FUNC(0x02508A58, cXyz*, this, actor);
    return &actor->current.pos;
}
VERIFY(0x02508A58, &dCamera_c::positionPntOf);

/* 0250D464 */
void dCamera_c::ResetBlure(s32 p) {
    WWHD_FUNC(0x0250D464, void, this, p);
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
VERIFY(0x0250D464, &dCamera_c::ResetBlure);

/* 0250D4C0 */
void dCamera_c::SetBlureTimer(s32 t) {
    WWHD_FUNC(0x0250D4C0, void, this, t);
    mBlureTimer = t;
}
VERIFY(0x0250D4C0, &dCamera_c::SetBlureTimer);

/* 0250D4C8 */
void dCamera_c::SetBlureAlpha(f32 alpha) {
    WWHD_FUNC(0x0250D4C8, void, this, alpha);
    mBlureAlpha = alpha;
}
VERIFY(0x0250D4C8, &dCamera_c::SetBlureAlpha);

/* 0250D4D0 SetBlureScale(f32) */
static void dCamera_SetBlureScale1(dCamera_c* i_this, f32 s) {
    WWHD_FUNC(0x0250D4D0, void, i_this, s);
    i_this->mBlureScale.x = s;
    i_this->mBlureScale.y = s;
    i_this->mBlureScale.z = 0.0f;
}
VERIFY(0x0250D4D0, dCamera_SetBlureScale1);

/* 02514CFC */
bool dCamera_c::followCamera2(s32 p) {
    WWHD_FUNC(0x02514CFC, bool, this, p);
    return followCamera(p);
}
VERIFY(0x02514CFC, &dCamera_c::followCamera2);


/* 02514EB8 SetTypeForce(s32, fopAc_ac_c*) */
static bool dCamera_SetTypeForceI(dCamera_c* i_this, s32 type, fopAc_ac_c* actor) {
    WWHD_FUNC(0x02514EB8, bool, i_this, type, actor);
    if (i_this->m524 == 0xFF) {
        i_this->m524 = type;
        i_this->m528 = actor;
        if (type != 0xFF) return true;
    }
    return false;
}
VERIFY(0x02514EB8, dCamera_SetTypeForceI);


/* 02514EE4 SetTypeForce(char*, fopAc_ac_c*) */
static void dCamera_SetTypeForceS(dCamera_c* i_this, const char* name, fopAc_ac_c* actor) {
    WWHD_FUNC(0x02514EE4, void, i_this, name, actor);
    s32 type = i_this->GetCameraTypeFromCameraName(name);
    gabi::call<bool>(0x02514EB8, i_this, type, actor);
}
VERIFY(0x02514EE4, dCamera_SetTypeForceS);

/* 02514F2C */
void dCamera_c::Stop() {
    WWHD_FUNC(0x02514F2C, void, this);
    mActive = 0;
}
VERIFY(0x02514F2C, &dCamera_c::Stop);

/* 02514F38 */
void dCamera_c::Start() {
    WWHD_FUNC(0x02514F38, void, this);
    mActive = 1;
}
VERIFY(0x02514F38, &dCamera_c::Start);

/* 02514F44 */
void dCamera_c::Stay() {
    WWHD_FUNC(0x02514F44, void, this);
    mPause = 1;
}
VERIFY(0x02514F44, &dCamera_c::Stay);

/* 02514F50 Set(cXyz center, cXyz eye) */
static bool dCamera_Set2(dCamera_c* i_this, cXyz* center, cXyz* eye) {
    WWHD_FUNC(0x02514F50, bool, i_this, center, eye);
    i_this->mCenter.copy(*center);
    i_this->mEye.copy(*eye);
    return true;
}
VERIFY(0x02514F50, dCamera_Set2);

/* 02514F88 Set(cXyz center, cXyz eye, f32 fovY, s16 bank) */
static bool dCamera_Set4fs(dCamera_c* i_this, cXyz* center, cXyz* eye, f32 fovY, s16 bank) {
    WWHD_FUNC(0x02514F88, bool, i_this, center, eye, fovY, bank);
    i_this->mCenter.copy(*center);
    i_this->mEye.copy(*eye);
    i_this->mFovy = fovY;
    cSAngle_Val(&i_this->mBank, bank);
    return true;
}
VERIFY(0x02514F88, dCamera_Set4fs);

/* 02514FE8 Set(cXyz center, cXyz eye, s16 bank, f32 fovY) */
static bool dCamera_Set4sf(dCamera_c* i_this, cXyz* center, cXyz* eye, s16 bank, f32 fovY) {
    WWHD_FUNC(0x02514FE8, bool, i_this, center, eye, bank, fovY);
    i_this->mCenter.copy(*center);
    i_this->mEye.copy(*eye);
    i_this->mFovy = fovY;
    cSAngle_Val(&i_this->mBank, bank);
    return true;
}
VERIFY(0x02514FE8, dCamera_Set4sf);

/* 02515048 */
bool dCamera_c::Reset() {
    WWHD_FUNC(0x02515048, bool, this);
    mViewCache.mCenter.copy(mCenter);
    mViewCache.mEye.copy(mEye);
    mViewCache.mFovy = (f32)mFovy;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&mEye, tmp, &mCenter);
    cSGlobe_Val(&mDirection, tmp);
    gabi::store<u32>(ea() + 0x3C, gabi::load<u32>(ea() + 0x8)); /* mViewCache.mDirection = mDirection */
    gabi::store<u32>(ea() + 0x40, gabi::load<u32>(ea() + 0xC));
    mViewCache.mBank = (s16)mBank;
    cXyz_mi(&mViewCache.mEye, tmp, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, tmp);
    return true;
}
VERIFY(0x02515048, &dCamera_c::Reset);

/* 0251510C Reset(cXyz center, cXyz eye) */
static bool dCamera_Reset2(dCamera_c* i_this, cXyz* center, cXyz* eye) {
    WWHD_FUNC(0x0251510C, bool, i_this, center, eye);
    i_this->mCenter.copy(*center);
    i_this->mViewCache.mCenter.copy(i_this->mCenter);
    i_this->mEye.copy(*eye);
    i_this->mViewCache.mEye.copy(i_this->mEye);
    i_this->mViewCache.mFovy = (f32)i_this->mFovy;
    i_this->mViewCache.mBank = (s16)i_this->mBank;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mViewCache.mEye, tmp, &i_this->mViewCache.mCenter);
    cSGlobe_Val(&i_this->mViewCache.mDirection, tmp);
    return i_this->Reset();
}
VERIFY(0x0251510C, dCamera_Reset2);

/* 025151BC Reset(cXyz center, cXyz eye, f32 fovY, s16 bank) */
static bool dCamera_Reset4(dCamera_c* i_this, cXyz* center, cXyz* eye, f32 fovY, s16 bank) {
    WWHD_FUNC(0x025151BC, bool, i_this, center, eye, fovY, bank);
    i_this->mCenter.copy(*center);
    i_this->mViewCache.mCenter.copy(i_this->mCenter);
    i_this->mEye.copy(*eye);
    i_this->mFovy = fovY;
    i_this->mViewCache.mEye.copy(i_this->mEye);
    i_this->mViewCache.mFovy = fovY;
    gabi::Local<cSAngle_l> ang;
    gabi::Local<cXyz> tmp;
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, ang.get(), bank);
    u16 v = gabi::load<u16>(gabi::ea(a));
    i_this->mBank = (s16)v;
    i_this->mViewCache.mBank = (s16)v;
    cXyz_mi(&i_this->mViewCache.mEye, tmp, &i_this->mViewCache.mCenter);
    cSGlobe_Val(&i_this->mViewCache.mDirection, tmp);
    return i_this->Reset();
}
VERIFY(0x025151BC, dCamera_Reset4);

/* 02515280 */
bool dCamera_c::SetTrimSize(s32 size) {
    WWHD_FUNC(0x02515280, bool, this, size);
    mTrimSize = size;
    return true;
}
VERIFY(0x02515280, &dCamera_c::SetTrimSize);

/* 0251528C */
bool dCamera_c::SetTrimTypeForce(s32 force) {
    WWHD_FUNC(0x0251528C, bool, this, force);
    mTrimTypeForce = force;
    return true;
}
VERIFY(0x0251528C, &dCamera_c::SetTrimTypeForce);

/* 02515298. HD: the two pattern buffers have 5 bytes (GameCube 4: a 32-bit pattern wrote one
 * byte past m544 into m548[0]) */
static s32 dCamera_StartShake(dCamera_c* i_this, s32 i_length, u8* i_pattern, s32 i_flags, cXyz* i_pos) {
    WWHD_FUNC(0x02515298, s32, i_this, i_length, i_pattern, i_flags, i_pos);
    u32 len = (u32)i_length;
    if (len > 0x20) len = 0x20; /* also catches negative lengths */
    i_this->m550 = (s32)len;
    u32 a544 = gabi::ea(i_this) + 0x548;
    u32 a548 = gabi::ea(i_this) + 0x54D;
    u32 pat = gabi::ea(i_pattern);
    s32 i;
    for (i = 0; i < 5; i++) {
        gabi::store<u8>(a548 + i, 0);
        gabi::store<u8>(a544 + i, 0);
    }
    s32 n = (s32)len >> 3;
    for (i = 0; i < n; i++) {
        u8 v = gabi::load<u8>(pat + i);
        gabi::store<u8>(a548 + i, v);
        gabi::store<u8>(a544 + i, v);
    }
    u32 r = len & 7;
    u32 m = gabi::load<u8>(pat + i) & (0xFFu << (8 - r));
    gabi::store<u8>(a544 + i, (u8)m);
    if (len == 0x20) m |= (u32)gabi::load<u8>(pat) >> r;
    gabi::store<u8>(a548 + i, (u8)m);
    gabi::Local<cXyz> nrm;
    gabi::call(0x0201B084, i_pos, nrm.get()); /* cXyz::norm */
    i_this->m55C.copy(*nrm);
    i_this->m554 = 0;
    i_this->m588 = i_flags;
    return 1;
}
VERIFY(0x02515298, dCamera_StartShake);

/* 025153D4 */
bool dCamera_c::StopShake() {
    WWHD_FUNC(0x025153D4, bool, this);
    m550 = 0;
    m588 = 0;
    m554 = 0;
    return true;
}
VERIFY(0x025153D4, &dCamera_c::StopShake);

/* 025153EC SetBlureScale(f32, f32, f32) */
static void dCamera_SetBlureScale3(dCamera_c* i_this, f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x025153EC, void, i_this, x, y, z);
    i_this->mBlureScale.x = x;
    i_this->mBlureScale.y = y;
    i_this->mBlureScale.z = z;
}
VERIFY(0x025153EC, dCamera_SetBlureScale3);

/* 025153FC */
void dCamera_c::SetBlurePositionType(s32 t) {
    WWHD_FUNC(0x025153FC, void, this, t);
    mBlurePositionType = t;
}
VERIFY(0x025153FC, &dCamera_c::SetBlurePositionType);

/* 02515404 */
void dCamera_c::SetBlurePosition(f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x02515404, void, this, x, y, z);
    SetBlurePositionType(1);
    mBlurePosition.x = x;
    mBlurePosition.y = y;
    mBlurePosition.z = z;
}
VERIFY(0x02515404, &dCamera_c::SetBlurePosition);

/* 02515434 */
bool dCamera_c::SubjectLockOn(fopAc_ac_c* target) {
    WWHD_FUNC(0x02515434, bool, this, target);
    mEventFlags = mEventFlags | 0x2800000; /* HD: GameCube sets 0x3000000 */
    mpLockonTarget = target;
    return true;
}
VERIFY(0x02515434, &dCamera_c::SubjectLockOn);

/* 0251544C */
bool dCamera_c::SubjectLockOff() {
    WWHD_FUNC(0x0251544C, bool, this);
    mEventFlags = mEventFlags & ~0x2800000u; /* HD: GameCube clears 0x3000000 */
    mpLockonTarget = nullptr;
    return true;
}
VERIFY(0x0251544C, &dCamera_c::SubjectLockOff);

/* 024FA018 */
fopAc_ac_c* dCamera_c::GetForceLockOnActor() {
    WWHD_FUNC(0x024FA018, fopAc_ac_c*, this);
    return fopAcM_SearchByID(mLockOnActorId);
}
VERIFY(0x024FA018, &dCamera_c::GetForceLockOnActor);

/* 02515470 */
bool dCamera_c::ForceLockOn(u32 id) {
    WWHD_FUNC(0x02515470, bool, this, id);
    mLockOnActorId = id;
    mForceLockTimer = 0;
    mpLockonActor = GetForceLockOnActor();
    return true;
}
VERIFY(0x02515470, &dCamera_c::ForceLockOn);

/* 025052BC */
bool dCamera_c::ForceLockOff(u32 id) {
    WWHD_FUNC(0x025052BC, bool, this, id);
    if (id == mLockOnActorId || id == fpcM_ERROR_PROCESS_ID_e) {
        mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
        return true;
    }
    return false;
}
VERIFY(0x025052BC, &dCamera_c::ForceLockOff);

/* 025154B0 */
bool dCamera_c::ScopeViewMsgModeOff() {
    WWHD_FUNC(0x025154B0, bool, this);
    mEventFlags = mEventFlags & ~0x400000u;
    return true;
}
VERIFY(0x025154B0, &dCamera_c::ScopeViewMsgModeOff);

/* 025154C4 */
bool dCamera_c::SetExtendedPosition(cXyz* pos) {
    WWHD_FUNC(0x025154C4, bool, this, pos);
    mExtendedPos.copy(*pos);
    return true;
}
VERIFY(0x025154C4, &dCamera_c::SetExtendedPosition);


/* 02500918 */
bool dCamera_c::Chtyp(s32 next) {
    WWHD_FUNC(0x02500918, bool, this, next);
    if (onTypeChange(mCurType, next)) {
        mCurType = next;
        return true;
    }
    return false;
}
VERIFY(0x02500918, &dCamera_c::Chtyp);

/* 0250235C */
static BOOL is_camera_delete(void*) {
    WWHD_FUNC(0x0250235C, BOOL, 0);
    return TRUE;
}
VERIFY(0x0250235C, is_camera_delete);

/* 024F8B64. HD: the window is looked up through the play object's camera table */
void dCamera_c::setView(f32 x, f32 y, f32 w, f32 h) {
    WWHD_FUNC(0x024F8B64, void, this, x, y, w, h);
    s32 camId = gabi::call<s32>(0x025DA64C, mpCamera.get()); /* fopCamM_GetParam */
    s32 winId = gabi::load<s8>(dComIfGp_ea() + camId * 0x34 + 0x5AFC); /* dComIfGp_getCameraWinID */
    u32 win = dComIfGp_ea() + winId * 0x2C;
    f32 nearZ = gabi::load<f32>(win + 0x5AE0);
    win += 0x5ACC; /* dComIfGp_getWindow */
    f32 n = gabi::load<f32>(win + 0x10);
    gabi::call(0x0252CC8C, win, x, y, w, h, n, nearZ); /* dDlst_window_c::setViewPort */
    gabi::call(0x0252CCA8, win, x, y, w, h);            /* dDlst_window_c::setScissor */
}
VERIFY(0x024F8B64, &dCamera_c::setView);

/* 024F8C54. HD: ResetView takes the view to restore (1280x720 viewport, then the centre, eye,
 * bank and fovy of the given view become the camera's, GameCube only resets the 640x480 view) */
static void dCamera_ResetView(dCamera_c* i_this, u8* view) {
    WWHD_FUNC(0x024F8C54, void, i_this, view);
    i_this->setView(0.0f, 0.0f, 1280.0f, 720.0f);
    u32 v = gabi::ea(view);
    cXyz* center = gabi::at<cXyz>(v + 0xE8); /* view lookat center */
    cXyz* eye = gabi::at<cXyz>(v + 0xDC);    /* view lookat eye */
    i_this->mViewCache.mCenter.copy(*center);
    i_this->mCenter.copy(i_this->mViewCache.mCenter);
    i_this->mViewCache.mEye.copy(*eye);
    i_this->mEye.copy(i_this->mViewCache.mEye);
    gabi::Local<cSAngle_l> ang;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mViewCache.mEye, tmp, &i_this->mViewCache.mCenter);
    cSGlobe_Val(&i_this->mViewCache.mDirection, tmp);
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, ang.get(), (s16)gabi::load<s16>(v + 0x100));
    u16 b = gabi::load<u16>(gabi::ea(a));
    i_this->mViewCache.mBank = (s16)b;
    i_this->mBank = (s16)b;
    f32 fovy = gabi::load<f32>(v + 0xD4);
    i_this->mFovy = fovy;
    i_this->mViewCache.mFovy = fovy;
}
VERIFY(0x024F8C54, dCamera_ResetView);

/* 024F7DF4 */
static bool limited_range_addition(be<f32>* p, f32 add, f32 lo, f32 hi) {
    WWHD_FUNC(0x024F7DF4, bool, p, add, lo, hi);
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
VERIFY(0x024F7DF4, limited_range_addition);

/* 025154E4: compiler-generated (header statics of the translation unit) */
static void __sinit_d_camera_cpp() {
    WWHD_FUNC(0x025154E4, void);
    sinit_header_statics_z(0x1046EECC, 0x101D55CC, 0x1046EF68);
}
VERIFY(0x025154E4, __sinit_d_camera_cpp);

/* 02515578: compiler-generated deleting destructor (header SafeString) */
static void d_camera_string_destructor(void* p, u32 flags) {
    WWHD_FUNC(0x02515578, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02515578, d_camera_string_destructor);

/* 0251558C: compiler-generated constructor of the m0A4[] elements (allocates when NULL) */
static dCamera_c::PosSet* dCamera_PosSet_ct(dCamera_c::PosSet* p) {
    WWHD_FUNC(0x0251558C, dCamera_c::PosSet*, p);
    if (p == nullptr) {
        p = (dCamera_c::PosSet*)operator_new(0x20);
        if (p == nullptr) return p;
    }
    cSAngle_ct(&p->mBank);
    return p;
}
VERIFY(0x0251558C, dCamera_PosSet_ct);

/* 025155D4: compiler-generated empty function (header SafeString) */
static void d_camera_string_terminate(void*) {
    WWHD_FUNC(0x025155D4, void, 0);
}
VERIFY(0x025155D4, d_camera_string_terminate);

/* 025155D8: per-TU copy of sead::Vector3f::normalize (returns the length) */
static f32 sead_Vector3_normalize(cXyz* v) {
    WWHD_FUNC(0x025155D8, f32, v);
    f32 len = gabi::call<f32>(0x028E8E10, v); /* PSVECMag */
    if (len > 0.0f) {
        f32 inv = 1.0f / len;
        v->x = v->x * inv;
        v->y = v->y * inv;
        v->z = v->z * inv;
    }
    return len;
}
VERIFY(0x025155D8, sead_Vector3_normalize);

/* 02515644: per-TU copy of sead::Quatf::setAxisAngle */
static void sead_Quat_setAxisAngle(be<f32>* q, cXyz* axis, f32 angle) {
    WWHD_FUNC(0x02515644, void, q, axis, angle);
    f32 half = angle * 0.5f;
    f32 c = gabi::call<f32>(0x028F4BE0, half); /* cosf */
    f32 s = gabi::call<f32>(0x028F43F8, half); /* sinf */
    f32 x = s * axis->x;
    f32 y = s * axis->y;
    f32 z = s * axis->z;
    q[3] = c;
    q[1] = y;
    q[0] = x;
    q[2] = z;
}
VERIFY(0x02515644, sead_Quat_setAxisAngle);

/* 025156EC: per-TU copy of sead::Matrix33f::fromQuat */
static void sead_Matrix33_fromQuat(be<f32>* m, be<f32>* q) {
    WWHD_FUNC(0x025156EC, void, m, q);
    f32 y = q[1];
    f32 z = q[2];
    f32 y2 = y + y;
    f32 w = q[3];
    f32 z2 = z + z;
    f32 x = q[0];
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
    m[0] = (1.0f - yy) - zz;
    m[2] = xz + wy;
    m[1] = xy - wz;
    m[3] = xy + wz;
    m[5] = yz - wx;
    m[4] = oneMinusXx - zz;
    m[8] = oneMinusXx - yy;
    m[6] = xz - wy;
    m[7] = yz + wx;
}
VERIFY(0x025156EC, sead_Matrix33_fromQuat);

/* 02515788: per-TU copy of a cXyz array element copy (out = array[idx]) */
static void cXyz_array_get(cXyz* out, cXyz* array, s32 idx) {
    WWHD_FUNC(0x02515788, void, out, array, idx);
    cXyz* src = gabi::at<cXyz>(gabi::ea(array) + idx * 0xC);
    out->copy(*src);
}
VERIFY(0x02515788, cXyz_array_get);
