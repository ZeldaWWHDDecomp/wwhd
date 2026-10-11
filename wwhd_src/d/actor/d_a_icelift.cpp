/** Floating ice platform with path travel and rider-induced tilt.
 * Verified WWHD behavior with GameCube class/method names. */
#include "d/actor/d_a_icelift.h"
#include "bindings.h"
// HD: original ROM tables and singleton slots are mapped without invented initializers.
static u32 l_arcName[2] = {};
static u32 l_bmdIdx[2] = {};
static u32 l_dzbIdx[2] = {};
static u32 l_heapSize[2] = {};
static u32 l_resourceControl;
static u32 l_save;
static SinCosEntry sincosTable[8192] = {};
static IliftQuat c_identityQuat;
static char assertFile[1] = {};
static char assertMessage[1] = {};
static Mtx34 mtxStack;
static cXyz c_zero;
const gc_tu_data gc_tu = {
    .safeString = 0x10011BA8,
    .objAcch = {0x10011BC0, 0x10011BE0, 0x10011BD0},
    .objAcchDt = {0x10011BD0, 0x10011BE0},
};
static u8 l_dtorRecords[3][12] = {};
static u8 l_staticBytes[2] = {};
static u32 l_staticZero[4] = {};
static f32 l_pi[2] = {};
static u32 drawListOpaque;
static u32 drawListTranslucent;

static u32 archive(daIlift_c *a) {
    return l_arcName[u32(a->mType)];
}
static cXyz *vec(u32 a) {
    return gabi::at<cXyz>(a);
}
static f32 sine(u16 a) {
    return sincosTable[u32(a) >> 3].sin;
}
static f32 cosine(u16 a) {
    return sincosTable[u32(a) >> 3].cos;
}
static void quatCopy(IliftQuat *d, const IliftQuat *source) {
    // HD: quaternion assignment copies integer words, retaining signaling NaNs.
    auto *dst = reinterpret_cast<be<u32> *>(d);
    auto *src = reinterpret_cast<const be<u32> *>(source);
    for (u32 i = 0; i < 4; i++)
        dst[i] = (u32)src[i];
}

/* 0217C874 */
void rideCallBack(void *bg, daIlift_c *a, fopAc_ac_c *rider) {
    gabi::Local<cXyz> offset;
    ilift_call::vectorSubtract(&rider->current.pos, offset.get(), &a->current.pos);
    a->mRiderOffset.copy(*offset);
    if (!rider || fopAcM_GetName(rider) != IliftPlayerProcess)
        return;
    gabi::Local<cXyz> up, cross;
    up->y = 1.0f;
    up->z = 0.0f;
    up->x = 0.0f;
    a->mWaveTime = 0;
    a->mRiderPresent = 1;
    ilift_call::vectorCross(offset.get(), cross.get(), up.get());
    offset->copy(*cross);
    f32 mag = ilift_call::vectorSquaredMag(offset.get());
    a->mRiderRadius = ilift_call::sqrtf(mag);
    if (!ilift_call::vectorNormalizeRS(offset.get()))
        return;
    s16 target = s16(gabi::ftoi(-(f32(a->mRiderRadius) * 4.0f)));
    a->mWaveTarget = target;
    ilift_call::addCalcAngleS2(&a->mWaveAngle, target, 8, 0x200);
    f32 s = sine(u16(a->mWaveAngle));
    f32 x = offset->x, y = offset->y, z = offset->z;
    a->mTargetQuat.x = x * s;
    a->mTargetQuat.y = y * s;
    a->mTargetQuat.z = z * s;
    a->mTargetQuat.w = cosine(u16(a->mWaveAngle));
    a->mWaveDecay = 10.0f;
}

/* 0217C9E0 */
// HD: unlike the unfinished GC declaration, this entry returns BOOL to its caller.
BOOL daIlift_c::CreateHeap() {
    gabi::Local<SafeString> name;
    u32 ty = mType;
    u32 control = (u32)l_resourceControl;
    name->mStringTop = (u32)l_arcName[ty];
    name->__vtbl = gc_tu.safeString;
    u32 index = (u32)l_bmdIdx[ty];
    u32 data = ilift_call::getResource(control, name.get(), index);
    if (!data)
        ilift_call::showAssert(assertFile, 0xEB, assertMessage);
    u32 model = ilift_call::createModel(data, 0x80000, 0x11000022);
    mpModel = model;
    if (!model)
        return FALSE;
    u32 bg = ilift_call::createBgW(0);
    mpBgW = bg;
    if (!bg)
        return FALSE;
    ty = mType;
    control = (u32)l_resourceControl;
    name->mStringTop = (u32)l_arcName[ty];
    name->__vtbl = gc_tu.safeString;
    index = (u32)l_dzbIdx[ty];
    data = ilift_call::getResource(control, name.get(), index);
    if (ilift_call::setBgW(u32(mpBgW), data, 1, &mBgMtx[0]))
        return FALSE;
    gabi::at<IliftBgW>(mpBgW)->__vtbl = 0x024EE708;
    gabi::at<IliftBgW>(mpBgW)->mRideCallback = 0x0217C874;
    return TRUE;
}
/* 0217CB18 */
BOOL CheckCreateHeap(daIlift_c *a) {
    return a->CreateHeap();
}

/* 0217CB1C */
void daIlift_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    u32 m = mpModel;
    auto *model = gabi::at<IliftModel>(m);
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    ilift_call::translateMatrix(&mtxStack, x, y, z);
    ilift_call::multiplyQuaternion(&mCurrentQuat);
    f32 matrix[12];
    for (u32 i = 0; i < 12; i++)
        matrix[i] = (f32)mtxStack.m[i / 4][i % 4];
    m = mpModel;
    for (u32 i = 0; i < 12; i++)
        gabi::at<IliftModel>(m)->mBaseTransform.m[i / 4][i % 4] = matrix[i];
    x = scale.x;
    y = scale.y;
    ilift_call::scaleMatrix(x, y, x);
    ilift_call::copyMatrix(&mtxStack, &mBgMtx[0]);
}

/* 0217CC0C */
void daIlift_c::CreateInit() {
    daIlift_c *a = this;
    u32 m = mpModel;
    cullMtx = m ? m + 0xC8 : 0;
    ilift_call::setCullBox(this, -200.0f, -250.0f, -200.0f, 200.0f, 250.0f, 200.0f);
    cullSizeFar = 1.0f;
    ilift_call::setWall(&mAcchCir, 30.0f, 30.0f);
    ilift_call::setAcch(&mAcch, &current.pos, &old.pos, a, 1, &mAcchCir, &speed, 0, 0);
    mAcch.m_flags = (u32)mAcch.m_flags & ~(dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE);
    gravity = -5.0f;
    quatCopy(&mTargetQuat, &c_identityQuat);
    quatCopy(&mCurrentQuat, &c_identityQuat);
    u32 param = mParameters;
    mPathIndex = (param >> 4) & 0xFF;
    if (u8(mPathIndex) != 0xFF) {
        u32 p = ilift_call::getRoomPath(u32(u8(mPathIndex)), s32(current.roomNo));
        mpPath = gabi::at<IliftPath>(p);
        if (p) {
            mPointDirection = 1;
            mPointIndex = 1;
            IliftPath *path = gabi::at<IliftPath>(p);
            IliftPoint *pts = path->m_points;
            f32 x = pts[1].m_position.x, y = pts[1].m_position.y, z = pts[1].m_position.z;
            mTargetPoint.x = x;
            mTargetPoint.y = y;
            mTargetPoint.z = z;
            mPreviousPoint.x = x;
            mPreviousPoint.y = y;
            mPreviousPoint.z = z;
            current.pos.x = (f32)path->m_points[0].m_position.x;
            current.pos.y = (f32)path->m_points[0].m_position.y;
            current.pos.z = (f32)path->m_points[0].m_position.z;
        } else
            mPathIndex = 0xFF;
    }
    mSwitchIndex = (u32(mParameters) >> 12) & 0xFF;
    u32 play = ilift_call::getPlay();
    ilift_call::registerBgW(play + 0x12A0, u32(mpBgW), a);
    set_mtx();
    ilift_call::moveBgW(u32(mpBgW));
}

/* 0217CE04 */
s32 daIlift_c::_create() {
    daIlift_c *a = this;
    if (!(u32(actor_condition) & 8)) {
        if (a) {
            ilift_call::constructActor(this);
            __vtbl = 0x10011BF0;
            ilift_call::constructAcch(&mAcch);
            mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
            mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
            mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
            mAcch.mbObjThrough = 1;
            ilift_call::constructAcchCircle(&mAcchCir);
        }
        actor_condition = u32(actor_condition) | 8;
    }
    mType = u32(mParameters) & 15;
    if (u8(mType) >= 2)
        return cPhs_ERROR_e;
    s32 phase = ilift_call::loadResource(&mPhase, archive(this));
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 heapSize = (u32)l_heapSize[u32(mType)];
    if (!ilift_call::createSolidHeap(this, CheckCreateHeap, heapSize))
        return cPhs_ERROR_e;
    CreateInit();
    return phase;
}
/* 0217CF28 */
s32 daIlift_Create(daIlift_c *a) {
    return a->_create();
}
/* 0217CF2C */
BOOL daIlift_c::_delete() {
    if (heap) {
        u32 play = ilift_call::getPlay();
        ilift_call::releaseBgW(play + 0x12A0, u32(mpBgW));
    }
    ilift_call::deleteResource(&mPhase, archive(this));
    return TRUE;
}
/* 0217CF8C */
BOOL daIlift_Delete(daIlift_c *a) {
    return a->_delete();
}
/* 0217CF90 */
BOOL daIlift_c::_draw() {
    u32 env = ilift_call::getEnvLight();
    ilift_call::settingTev(env, 1, &current.pos, &tevStr);
    env = ilift_call::getEnvLight();
    ilift_call::setLightTev(env, u32(mpModel), &tevStr);
    u32 play = ilift_call::getPlay();
    drawListOpaque = (u32)gabi::at<IliftPlayLists>(play)->mOpaList;
    play = ilift_call::getPlay();
    drawListTranslucent = (u32)gabi::at<IliftPlayLists>(play)->mXluList;
    ilift_call::updateModel(u32(mpModel), 0);
    play = ilift_call::getPlay();
    drawListOpaque = (u32)gabi::at<IliftPlayLists>(play)->mOpaDefault;
    play = ilift_call::getPlay();
    drawListTranslucent = (u32)gabi::at<IliftPlayLists>(play)->mXluDefault;
    return TRUE;
}
/* 0217D028 */
BOOL daIlift_Draw(daIlift_c *a) {
    return a->_draw();
}

/* 0217D02C */
void daIlift_c::set_next_pnt() {
    if (u8(mPathIndex) == 0xFF)
        return;
    s32 next = s8(u8(mPointIndex) + u8(mPointDirection));
    mPointIndex = next;
    IliftPath *p = mpPath;
    if ((u8)p->m_closed & 1) {
        u16 count = (u16)p->m_num;
        if (next > s32(s8(count)) - 1)
            mPointIndex = 0;
        else if (next < 0)
            mPointIndex = count - 1;
    } else {
        u16 count = (u16)p->m_num;
        if (next > s32(count) - 1) {
            mPointDirection = -1;
            mPointIndex = (u16)mpPath->m_num - 2;
        } else if (next < 0) {
            mPointDirection = 1;
            mPointIndex = 1;
        }
    }
    mPreviousPoint.copy(mTargetPoint);
    IliftPoint *pts = &mpPath->m_points[s32(mPointIndex)];
    mTargetPoint.x = (f32)pts->m_position.x;
    mTargetPoint.y = (f32)pts->m_position.y;
    mTargetPoint.z = (f32)pts->m_position.z;
}

/* 0217D11C */
void daIlift_c::lift_normal_move() {
    u32 state = mMoveState;
    if (state == NEXT_POINT) {
        mMoveTimer = 0;
        mMoveState = ACCELERATE;
        set_next_pnt();
    }
    if (state <= ACCELERATE) {
        mMoveTimer = u16(mMoveTimer) + 1;
        f32 goal = mTargetSpeed;
        if (ilift_call::addCalc(&mMoveSpeed, goal, 0.25f, 1.0f, 1.0f) == 0.0f)
            mMoveState = CRUISE;
    } else if (state == CRUISE) {
        gabi::Local<cXyz> diff;
        mMoveTimer = 0;
        ilift_call::vectorSubtract(&current.pos, diff.get(), &mTargetPoint);
        f32 mag = ilift_call::vectorSquaredMag(diff.get());
        f32 distance = ilift_call::sqrtf(mag);
        if (distance < 50.0f)
            mMoveState = DECELERATE;
    } else if (state == DECELERATE) {
        f32 goal = f32(mTargetSpeed) / 2.2f;
        mMoveTimer = u16(mMoveTimer) + 1;
        if (ilift_call::addCalc(&mMoveSpeed, goal, 0.25f, 1.0f, 1.0f) == 0.0f)
            mMoveState = NEXT_POINT;
    }
    f32 speed = mMoveSpeed;
    ilift_call::addCalcPosXZ(&current.pos, &mTargetPoint, 1.0f, speed);
}
/* 0217D294 */
void daIlift_c::path_move() {
    if (u8(mPathIndex) != 0xFF)
        lift_normal_move();
}

/* 0217D2A4 */
void daIlift_c::lift_wave() {
    gabi::Local<cXyz> up;
    up->z = 0.0f;
    up->y = 1.0f;
    up->x = 0.0f;
    u32 play = ilift_call::getPlay();
    u32 player = gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.get());
    if (u8(mRiderPresent))
        return;
    if (u8(mRiderPrevious)) {
        gabi::Local<cXyz> delta, horizontal;
        ilift_call::vectorSubtract(&mPreviousPlayerPosition, delta.get(), vec(player + 0x314));
        horizontal->x = delta->x;
        horizontal->y = 0.0f;
        horizontal->z = delta->z;
        f32 mag = ilift_call::vectorSquaredMag(horizontal.get());
        f32 distance = ilift_call::sqrtf(mag);
        f32 factor = distance / 18.0f;
        f32 tilt = f32(mRiderRadius) * 4.0f;
        tilt = tilt * 10.0f;
        mRiderRadius = 0.0f;
        mWaveTarget = s16(gabi::ftoi(-(tilt * factor)));
        if (u8(mRiderPresent))
            return;
    }
    s16 angle = mWaveTarget;
    s32 time = mWaveTime;
    u16 phase = u16(u32(time) << 10) & 0xFC00;
    f32 target = (f32(angle) * cosine(phase)) / f32(time);
    ilift_call::addCalcAngleS(&mWaveAngle, s16(gabi::ftoi(target)), 4, 0x800, 0x400);
    f32 s = sine(u16(mWaveAngle));
    gabi::Local<cXyz> cross, normalized;
    ilift_call::vectorCross(&mRiderOffset, cross.get(), up.get());
    ilift_call::vectorNormalizeZP(cross.get(), normalized.get());
    cross->copy(*normalized);
    s32 nonzero = ilift_call::vectorNotEqual(cross.get(), &c_zero);
    time = mWaveTime;
    if (nonzero) {
        f32 x = cross->x, y = cross->y, z = cross->z;
        mTargetQuat.x = x * s;
        mTargetQuat.y = y * s;
        mTargetQuat.z = z * s;
        mTargetQuat.w = cosine(u16(mWaveAngle));
    }
    if (time > 120)
        mWaveTarget = 0;
}

/* 0217D4E0 */
// HD: preserve register-width BOOL results for the actor profile wrappers.
BOOL daIlift_c::_execute() {
    u32 play = ilift_call::getPlay();
    u32 count = mFrameCount;
    u32 player = gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.get());
    u32 time = mWaveTime;
    mFrameCount = count + 1;
    mWaveTime = time + 1;
    mTargetSpeed = 4.0f;
    path_move();
    lift_wave();
    ilift_call::quaternionLerp(&mCurrentQuat, &mTargetQuat, &mCurrentQuat, 0.25f);
    mRiderPrevious = u8(mRiderPresent);
    u32 sw = mSwitchIndex;
    mRiderPresent = 0;
    if (sw != 0xFF) {
        u32 save = (u32)l_save;
        s32 on = ilift_call::isSwitch(save + 0x20, sw, s32(home.roomNo));
        ilift_call::changeAttributeCode(u32(mpBgW), 0x40, on ? 0xF : 0x15);
        ilift_call::changeAttributeCode(u32(mpBgW), 0x41, on ? 0 : 9);
    }
    set_mtx();
    ilift_call::moveBgW(u32(mpBgW));
    mPreviousPlayerPosition.copy(*vec(player + 0x314));
    return TRUE;
}
/* 0217D608 */
BOOL daIlift_Execute(daIlift_c *a) {
    return a->_execute();
}

// HD: compiler-generated initialization, destructors and SafeString virtuals.
/* 0217D60C */
void __sinit_d_a_icelift_cpp() {
    l_staticZero[2] = 0;
    l_staticZero[0] = 0;
    l_staticZero[3] = 0;
    l_staticZero[1] = 0;
    ilift_call::registerGlobalObject(l_dtorRecords[0]);
    l_pi[0] = -3.1415927410125732f;
    l_pi[1] = 3.1415927410125732f;
    ilift_call::constructGlobalByteA(&l_staticBytes[0]);
    ilift_call::registerGlobalObject(l_dtorRecords[1]);
    ilift_call::constructGlobalByteB(&l_staticBytes[1]);
    ilift_call::registerGlobalObject(l_dtorRecords[2]);
}
/* 0217D6A0 */
void iliftSafeString_dtor(SafeString *p, u32 flags) {
    if (p && (flags & 1))
        ilift_call::deleteObject(p);
}
/* 0217D6B4 */
BOOL daIlift_IsDelete(daIlift_c *a) {
    return TRUE;
}
/* 0217D6BC */
void daIlift_c_dtor(daIlift_c *a, u32 flags) {
    if (!a)
        return;
    ilift_call::destroyAcchCirArray(&a->mAcchCir.m_cir, 2);
    a->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
    a->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    ilift_call::destroyAcch(&a->mAcch, 0);
    ilift_call::destroyActor(a, 0);
    if (flags & 1)
        ilift_call::deleteObject(a);
}
/* 0217D740 */
void iliftSafeString_assureTerminationImpl(SafeString *a) {}

/* WWHD glue (tools/verify/autoglue.py)
data l_arcName 101B7938 nocheck
data l_bmdIdx 10011C60 nocheck
data l_dzbIdx 10011C68 nocheck
data l_heapSize 10011C70 nocheck
data l_resourceControl 101F4F28
data l_save 101F84DC
data sincosTable 104A44F8 nocheck
data c_identityQuat 101E9C38
data assertFile 10011C04 nocheck
data assertMessage 10011C14 nocheck
data mtxStack 1048D0CC
data c_zero 101FFBA8
data l_dtorRecords 101B7914 nocheck
data l_staticBytes 104647BC nocheck
data l_staticZero 104647C0 nocheck
data l_pi 104647B4 nocheck
data drawListOpaque 104B4634
data drawListTranslucent 104B4638
*/
