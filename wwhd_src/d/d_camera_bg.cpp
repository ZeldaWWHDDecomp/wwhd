/**
 * d_camera_bg.cpp (WWHD)
 * Follow camera dCamera_c: background (line, sphere) checks, relational positions, sight tests,
 * trimming.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_bg.h"
#include <bit>
#include "d/d_camera_accessors.h"

static dComIfG_camera_info_class* cameraInfo(s32 id) {
    return gabi::at<dComIfG_camera_info_class>(dComIfGp_ea() + PLAY_OFF(mCameraInfo) +
                                               id * sizeof(dComIfG_camera_info_class));
}
static CameraWindowCoreHD* cameraWindow(s32 id) {
    return gabi::at<CameraWindowCoreHD>(dComIfGp_ea() + PLAY_OFF(mDlstWindow) +
                                        id * sizeof(CameraWindowCoreHD));
}
// HD: event camera state and the mass manager live in opaque shared records.
static s32 eventCameraPlaying() { return gabi::at<be<s32>>(dComIfGp_ea() + 0x52E4)->get(); }
static u32 cameraMassManager() { return dComIfGp_ea() + PLAY_CCMASS; }

static CameraBackgroundHD::Setup* cameraSetup(dCamera_c* c) {
    return reinterpret_cast<CameraBackgroundHD::Setup*>(c->mCamSetup);
}
static CameraBackgroundHD::Style* cameraStyle(dCamera_c* c) {
    auto* p = reinterpret_cast<CameraBackgroundHD::Parameter*>(c->mCamParam);
    return gabi::at<CameraBackgroundHD::Style>(reinterpret_cast<be<u32>*>(&p->mStyle)->get());
}

using CameraByte = be<u8>;
using CameraWord = be<u32>;
using CameraFloat = be<f32>;
using CameraShort = be<s16>;
using CameraInt = be<s32>;
static CameraBackgroundHD::TrimSizes sTrimSizes;
static CameraWord sKazeInitialized;
static CameraWord sSeaInitialized;
static cXyz sKazeCenter;
static cXyz sSeaCenter;
static CameraFloat sKazeRadius;
static CameraFloat sSeaRadius;
static CameraShort sRightAngle;
static CameraShort sThreeQuarterAngle;
static CameraInt sPreviousHitType;
static CameraBackgroundHD::Registers sRegisters;

using namespace CameraBackgroundHD;
using CameraAngle = cSAngle_l;
using CameraGlobe = cSGlobe_l;
using CameraSphereData = camSphChkdata_l;
enum BackgroundCheckFlag : u32 {
    CheckGround = 1,
    CheckWall = 2,
    CheckRoof = 4,
    CheckWater = 8,
    CheckWallUp = 0x10,
    CheckBothDirections = 0x20,
    CheckBush = 0x40,
    CheckObject = 0x80,
};
enum HitType : s32 {
    NoHit = 0,
    WallHit = 2,
    CornerHit = 3,
    NewWallHit = 4,
    PreviousCornerHit = 5,
};
enum QueryVtable : u32 {
    kPolyInfoVtable = 0x1004A4DC,
    kGroupPassVtable = 0x1004A4EC,
    kGroundCheckVtable = 0x1004A4FC,
    kGroundPolyInfoVtable = 0x1004A50C,
    kGroundGroupVtable = 0x1004A51C,
    kGroundPassVtable = 0x1004A52C,
    kCameraGroundCheckVtable = 0x1004A53C,
    kCameraGroundPolyInfoVtable = 0x1004A54C,
    kCameraGroundGroupVtable = 0x1004A55C,
    kCameraGroundPassVtable = 0x1004A56C,
    kWaterGroundCheckVtable = 0x1004A57C,
    kWaterGroundPolyInfoVtable = 0x1004A58C,
    kWaterGroundGroupVtable = 0x1004A59C,
    kWaterGroundPassVtable = 0x1004A5AC,
    kRoofCheckVtable = 0x1004A5BC,
    kRoofPassVtable = 0x1004A5DC,
    kLinePassVtable = 0x1004A61C,
    kCameraLineCheckVtable = 0x1004A66C,
    kCameraLinePolyInfoVtable = 0x1004A67C,
    kCameraLineGroupVtable = 0x1004A68C,
    kCameraLinePassVtable = 0x1004A69C,
    kSpherePolyInfoVtable = 0x1004A6AC,
    kSphereCheckVtable = 0x1004A6BC,
    kSpherePassVtable = 0x1004A6DC,
    kCameraSphereVtable = 0x1004A6EC,
    kCameraSpherePolyInfoVtable = 0x1004A6FC,
    kCameraSphereCheckVtable = 0x1004A70C,
    kCameraSphereGroupVtable = 0x1004A71C,
    kCameraSpherePassVtable = 0x1004A72C,
};

template <class T> static T cameraArgument(T value) { return value; }
template <class T> static u32 cameraArgument(T* value) { return gabi::ea(value); }
template <class T, class Field> static T fieldRead(Field& field) {
    return reinterpret_cast<be<T>*>(&field)->get();
}
template <class T, class Field> static void fieldWrite(Field& field, T value) {
    *reinterpret_cast<be<T>*>(&field) = value;
}
f32 planeSignedLength(u32, u32);
void vectorNorm(u32, u32);
u32 massCameraResult(u32);
void capsuleConstruct(u32);
void capsuleSet(u32, u32, u32, f32);
void massSetCamera(u32, u32);
void capsuleDestroy(u32, u32);
void lineCheckSet(u32, u32, u32, u32);
bool backgroundLineCross(u32, u32);
void lineCheckConstruct(u32);
void lineCheckDestroy(u32, u32);
void sphereCheckConstruct(u32);
void sphereSet(u32, u32, f32);
bool backgroundSphereCheck(u32, u32, u32);
void globeFromVectorDiscard(u32, u32);
void angleAdd(u32, u32, u32);
u32 angleConstructValue(u32, u32);
void globeToVector(u32, u32);
s32 cameraGetParam(u32);
void projectPoint(u32, u32);
f32 horizontalDistance(u32, u32);
void angleSubtract(u32, u32, u32);
f32 angleCos(u32);
void groundCheckConstruct(u32);
void groundCheckDestroy(u32, u32);
f32 backgroundGroundCross(u32, u32);
void roofCheckConstruct(u32);
f32 backgroundRoofCheck(u32, u32);
void angleCopyDiscard(u32, u32);
u32 backgroundTrianglePlane(u32, u32, u32);
u32 globeFromVectorValue(u32, u32);
u32 angleCopyValue(u32, u32);
void vectorNormalize(u32, u32);
void vectorSubtract(u32, u32, u32);
void angleFromDegrees(u32, f32);
void angleNegate(u32, u32);
void matrixLookAt(u32, u32, u32, u32, u32);
f32 angleTan(u32);
void angleConstructDiscard(u32, u32);
void globeConstructPolar(u32, f32, u32, u32);
bool lineCheckCross(u32, u32, u32, u32, u32);
void angleMultiply(u32, u32, f32);
void angleMultiplyAssign(u32, f32);
bool backgroundIsMoving(u32, u32);
u32 backgroundActor(u32, u32);
void backgroundCorrectPosition(u32, u32, u32, u32, u32, u32);
s32 backgroundMoveCamera(u32, u32);
s32 backgroundPolyCamera(u32, u32, u32);
s32 backgroundRoomCamera(u32, u32);
s32 backgroundRoom(u32, u32);
void massCameraTop(u32, u32);
u32 degreeConstruct(u32, f32);
f32 degreeCos(u32);
u32 objectParameter(u32, u32, u32);
void globeCopy(u32, u32);
void angleAddAssign(u32, u32);
void globeConstruct(u32);
void vectorCross(u32, u32, u32);
bool twoPlaneLinePosition(u32, u32, u32, u32);
bool lineCheckSimple(u32, u32, u32, u32);
f32 angleSin(u32);
f32 angleDegrees(u32);

/* 024F7E54 */
static void sph_chk_callback(void* sphChk, void* vtxTbl, s32 i0, s32 i1, s32 i2, be<f32>* plane,
                             CameraSphereData* data) {

    f32 len = planeSignedLength(cameraArgument(plane),
                                cameraArgument(&data->field_0x8)); /* cM3d_SignedLenPlaAndPos */
    f32 dot = PSVECDotProduct((cXyz*)plane, &data->field_0x14);
    if (dot + plane[3] < -0.0001f)
        return; /* plane->getPlaneFunc(&field_0x14) */
    if (!(len < data->field_0x4))
        return;
    gabi::Local<cXyz> normal;
    gabi::Local<cXyz> push;
    f32 r = data->field_0x4;
    normal->copy(*(cXyz*)plane); /* *plane->GetNP() */
    cXyz_ml(normal, push, r - len);
    PSVECAdd(&data->field_0x8, push, &data->field_0x8);
}

/* 024F7F28 */
// HD-only: like sph_chk_callback, but pushes the position away from field_0x14 (used by compWallMargin in mode 4) instead of along the plane normal
static void sph_chk_callback2(void* sphChk, void* vtxTbl, s32 i0, s32 i1, s32 i2, be<f32>* plane,
                              CameraSphereData* data) {

    f32 len = planeSignedLength(cameraArgument(plane), cameraArgument(&data->field_0x8));
    f32 dot = PSVECDotProduct((cXyz*)plane, &data->field_0x14);
    if (dot + plane[3] < -0.0001f)
        return;
    if (!(len < data->field_0x4))
        return;
    gabi::Local<cXyz> push;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> dir;
    cXyz_mi(&data->field_0x14, diff, &data->field_0x8);
    vectorNorm(cameraArgument(diff.get()), cameraArgument(dir.get())); /* cXyz::norm */
    cXyz_ml(dir, push, data->field_0x4 - len);
    PSVECAdd(&data->field_0x8, push, &data->field_0x8);
}

/* 024F8F94 */
u32 dCamera_c::lineCollisionCheckBush(cXyz* start, cXyz* end) {

    u32 ret = 0;
    u32 result =
        massCameraResult(cameraArgument(cameraMassManager())); /* dCcMassS_Mng::GetResultCam */
    if (result & 2)
        ret = 1;
    if (result & 4)
        ret |= 2;
    if (result & 8)
        ret |= 4;
    gabi::Local<u8[0x30]> cps;
    capsuleConstruct(cameraArgument(cps.get())); /* cM3dGCps() */
    capsuleSet(cameraArgument(cps.get()), cameraArgument(start), cameraArgument(end),
               cameraArgument(30.0f)); /* cps.Set(*start, *end, 30.0f) */
    massSetCamera(cameraArgument(cameraMassManager()),
                  cameraArgument(cps.get()));                     /* dCcMassS_Mng::SetCam(cps) */
    capsuleDestroy(cameraArgument(cps.get()), cameraArgument(2)); /* ~cM3dGCps() */
    return ret;
}

/* 024FB4E8 */
bool dCamera_c::lineBGCheck(cXyz* start, cXyz* end, u8* linChk, u32 flags) {

    u32 chk = gabi::ea(linChk);
    if (flags & CheckObject) {
        fieldWrite<u8>(gabi::at<LineCheck>(chk)->mPolyPass.mbCamThrough, 0);
        fieldWrite<u8>(gabi::at<LineCheck>(chk)->mPolyPass.mbObjThrough, 1);
    } else {
        fieldWrite<u8>(gabi::at<LineCheck>(chk)->mPolyPass.mbObjThrough, 0);
        fieldWrite<u8>(gabi::at<LineCheck>(chk)->mPolyPass.mbCamThrough, 1);
    }
    lineCheckSet(cameraArgument(linChk), cameraArgument(start), cameraArgument(end),
                 cameraArgument(0)); /* dBgS_LinChk::Set(start, end, NULL) */
    u32 grp = fieldRead<u32>(gabi::at<LineCheck>(chk)->mLine.mFlag);
    if (flags & CheckRoof)
        grp &= ~0x20000000u;
    else
        grp |= 0x20000000u;
    if (flags & CheckWall)
        grp &= ~0x40000000u;
    else
        grp |= 0x40000000u;
    if (flags & CheckGround)
        grp &= ~0x80000000u;
    else
        grp |= 0x80000000u;
    fieldWrite<u32>(gabi::at<LineCheck>(chk)->mLine.mFlag, grp);
    u32 f = fieldRead<u32>(gabi::at<LineCheck>(chk)->mGroupPass.mGrp);
    if (flags & CheckWater)
        f |= 2;
    else
        f &= ~2u;
    fieldWrite<u32>(gabi::at<LineCheck>(chk)->mGroupPass.mGrp, f);
    return backgroundLineCross(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                               cameraArgument(linChk)); /* cBgS::LineCross */
}

/* dBgS_CamLinChk on the stack (HD: the constructor and destructor are partly inlined) */
static void camLinChk_ct(u32 o) {
    lineCheckConstruct(cameraArgument(o)); /* cBgS_LinChk() */
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mLine.cBgS_Chk::mPolyPassChk, o + 0x58);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mLine.cBgS_Chk::mGrpPassChk, o + 0x64);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mLine.cBgS_Chk::__vtbl, kCameraLineCheckVtable);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mLine.cBgS_PolyInfo::__vtbl, kCameraLinePolyInfoVtable);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mPolyPass.__vtbl, kCameraLinePassVtable);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mGroupPass.__vtbl, kCameraLineGroupVtable);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbObjThrough, 0);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbCamThrough, 1);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbLinkThrough, 0);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbBoomerangThrough, 0);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mPolyPass.mbRopeThrough, 0);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mGroupPass.mGrp, 3);
}
static void camLinChk_dt_vtables(u32 o) {
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mPolyPass.__vtbl, kLinePassVtable);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mLine.cBgS_PolyInfo::__vtbl, kPolyInfoVtable);
    fieldWrite<u32>(gabi::at<LineCheck>(o)->mGroupPass.__vtbl, kGroupPassVtable);
}

/* 024FB608 */
// lineBGCheck(cXyz* start, cXyz* end, cXyz* o_cross, u32 flags)
static bool dCamera_lineBGCheckCross(dCamera_c* i_this, cXyz* start, cXyz* end, cXyz* cross,
                                     u32 flags) {

    gabi::Local<LineCheck> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    if (i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags)) {
        camLinChk_dt_vtables(o);
        cross->copy(*(&gabi::at<LineCheck>(o)->mLine.mLin.mEnd)); /* lin_chk.GetCross() */
        lineCheckDestroy(cameraArgument(o), cameraArgument(0));   /* ~cBgS_LinChk() */
        return true;
    }
    cross->copy(*end);
    camLinChk_dt_vtables(o);
    lineCheckDestroy(cameraArgument(o), cameraArgument(0));
    return false;
}

/* 024FCBE8 */
// lineBGCheck(cXyz* start, cXyz* end, u32 flags)
static bool dCamera_lineBGCheck3(dCamera_c* i_this, cXyz* start, cXyz* end, u32 flags) {

    gabi::Local<LineCheck> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    bool ret = i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags);
    camLinChk_dt_vtables(o);
    lineCheckDestroy(cameraArgument(o), cameraArgument(0));
    return ret;
}

/* 0250FDB8 */
bool dCamera_c::lineBGCheckBoth(cXyz* start, cXyz* end, u8* linChk, u32 flags) {

    gabi::at<LineCheck>(gabi::ea(linChk))->mLine.mFrontFlag = 1;
    gabi::at<LineCheck>(gabi::ea(linChk))->mLine.mBackFlag = 1;
    return lineBGCheck(start, end, linChk, flags);
}

/* 024FBF9C */
// HD: mode 8 trims like the subject modes (1), GameCube 2; modes 1..3 come from a table; modes 10/11 keep the current size when no aim status is set
s32 dCamera_c::defaultTriming() {

    s32 force = mTrimTypeForce;
    if (force >= 0) {
        mTrimSize = force;
        return force;
    }
    fopAc_ac_c* player = mpPlayerActor.get();
    if (player == nullptr || fpcM_GetName(player) != 0xA8) {
        mTrimSize = 1;
        return 1;
    }
    u32 mode = mCurMode;
    s32 size;
    if (mode < 1) {
        size = 0;
    } else if (mode <= 3) {
        size = sTrimSizes.mSize[mode].get();
    } else if (mode == 8) {
        size = 1;
    } else if (mode < 10) {
        size = 0;
    } else if (mode < 12) {
        s32 pad = mPadId;
        if (dComIfGp_getPlayerStatus0(pad) & 0x40000) {
            size = 2;
        } else {
            pad = mPadId;
            if (dComIfGp_getPlayerStatus0(pad) & 0xA5000) {
                size = 1;
            } else {
                return mTrimSize;
            }
        }
    } else if (mode == 12) {
        size = chkFlag(0x1000) ? 1 : 0;
    } else {
        size = 0;
    }
    mTrimSize = size;
    return size;
}

/* 024FCCE0 */
// HD: in mode 4 the sphere check pushes the centre away from the view centre (sph_chk_callback2)
void dCamera_c::compWallMargin(cXyz* ret, cXyz* center, f32 radius) {

    gabi::Local<CameraSphereData> data;
    gabi::Local<SphereCheck> sph;
    u32 s = gabi::ea(sph.get());
    sphereCheckConstruct(cameraArgument(s)); /* dBgS_SphChk base constructor */
    data->field_0x0 = center;
    s32 mode = mCurMode;
    data->field_0x8.copy(*center);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mPolyInfo.__vtbl, kCameraSpherePolyInfoVtable);
    data->field_0x14.copy(mViewCache.mCenter);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mCheck.__vtbl, kCameraSphereCheckVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mSphereVtable, kCameraSphereVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mPolyPass.__vtbl, kCameraSpherePassVtable);
    data->field_0x4 = radius;
    fieldWrite<u8>(gabi::at<SphereCheck>(s)->mPolyPass.mbCamThrough, 1);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mGroupPass.__vtbl, kCameraSphereGroupVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mCallback,
                    mode == 4 ? 0x024F7F28u : 0x024F7E54u); /* SetCallback */
    sphereSet(cameraArgument(s), cameraArgument(center),
              cameraArgument(radius)); /* sph_chk.Set(*center, radius) */
    bool hit = backgroundSphereCheck(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(s),
                                     cameraArgument(data.get())); /* dBgS::SphChk */
    if (hit) {
        if (ret == nullptr)
            ret = (cXyz*)operator_new(0xC);
        if (ret != nullptr)
            ret->copy(data->field_0x8);
    } else {
        if (ret == nullptr)
            ret = (cXyz*)operator_new(0xC);
        if (ret != nullptr)
            ret->copy(*center);
    }
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mPolyInfo.__vtbl, kSpherePolyInfoVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mCheck.__vtbl, kSphereCheckVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mPolyPass.__vtbl, kSpherePassVtable);
    fieldWrite<u32>(gabi::at<SphereCheck>(s)->mGroupPass.__vtbl, kGroupPassVtable);
    lineCheckDestroy(cameraArgument(s + 0x24), cameraArgument(0));
}

/* 0250242C */
// relationalPos(fopAc_ac_c*, cXyz*)
void dCamera_c::relationalPos(cXyz* ret, fopAc_ac_c* actor, cXyz* offset) {

    if (actor == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr)
                return;
        }
        ret->copy(*&sZeroVector); /* cXyz::Zero */
        return;
    }
    gabi::Local<CameraGlobe> globe;
    gabi::Local<CameraAngle> dir;
    gabi::Local<CameraAngle> sum;
    gabi::Local<CameraAngle> tmp;
    gabi::Local<cXyz> att;
    gabi::Local<cXyz> xyz;
    globeFromVectorDiscard(cameraArgument(globe.get()),
                           cameraArgument(offset)); /* cSGlobe(const cXyz&) */
    directionOf(dir, actor);
    angleAdd(cameraArgument(dir.get()), cameraArgument(sum.get()),
             cameraArgument(&globe->mU)); /* cSAngle::operator+ */
    CameraAngle* a = gabi::at<CameraAngle>(
        angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)*sum)));
    globe->mU = (s16)*a;
    attentionPos(att, actor);
    globeToVector(cameraArgument(globe.get()), cameraArgument(xyz.get())); /* cSGlobe::Xyz */
    cXyz_pl(att, ret, xyz);
}

/* 025050E4 */
// HD: the projected point is tested against (0, 0)-(width, height) of the viewport
bool dCamera_c::pointInSight(cXyz* point) {

    s32 camId = cameraGetParam(cameraArgument(mpCamera.get())); /* fopCamM_GetParam */
    s32 winId = cameraInfo(camId)->mDlstWindowIdx.get();
    CameraWindowCoreHD* window = cameraWindow(winId);
    f32 h = window->mViewport.mScissor.mHeight.get();
    f32 w = window->mViewport.mScissor.mWidth.get();
    gabi::Local<cXyz> proj;
    projectPoint(cameraArgument(point), cameraArgument(proj.get())); /* mDoLib_project */
    f32 x = proj->x;
    if (x > 0.0f && x < w) {
        f32 y = proj->y;
        if (y > 0.0f && y < h)
            return true;
    }
    return false;
}

/* 02514D00 */
// relationalPos(fopAc_ac_c*, cXyz*, cSAngle)
static void dCamera_relationalPosA(dCamera_c* i_this, cXyz* ret, fopAc_ac_c* actor, cXyz* offset,
                                   CameraAngle* angle) {

    if (actor == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr)
                return;
        }
        ret->copy(*&sZeroVector); /* cXyz::Zero */
        return;
    }
    gabi::Local<CameraGlobe> globe;
    gabi::Local<CameraAngle> dir;
    gabi::Local<CameraAngle> sum1;
    gabi::Local<CameraAngle> sum2;
    gabi::Local<CameraAngle> tmp;
    gabi::Local<cXyz> att;
    gabi::Local<cXyz> xyz;
    globeFromVectorDiscard(cameraArgument(globe.get()), cameraArgument(offset));
    i_this->directionOf(dir, actor);
    angleAdd(cameraArgument(dir.get()), cameraArgument(sum1.get()), cameraArgument(&globe->mU));
    angleAdd(cameraArgument(sum1.get()), cameraArgument(sum2.get()), cameraArgument(angle));
    CameraAngle* a = gabi::at<CameraAngle>(
        angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)*sum2)));
    globe->mU = (s16)*a;
    i_this->attentionPos(att, actor);
    globeToVector(cameraArgument(globe.get()), cameraArgument(xyz.get()));
    cXyz_pl(att, ret, xyz);
}

/* 024F8DB4 */
// HD: the special areas use function-local statics (two centres)
void dCamera_c::checkSpecialArea() {

    if (sKazeInitialized.get() == 0) {
        sKazeInitialized = 1;
        sKazeCenter.x = 0.0f;
        sKazeCenter.y = -3650.0f;
        sKazeCenter.z = 0.0f;
    }
    if (sSeaInitialized.get() == 0) {
        sSeaCenter.z = -200000.0f;
        sSeaCenter.y = 750.0f;
        sSeaCenter.x = -180000.0f;
        sSeaInitialized = 1;
    }
    gabi::Local<cXyz> pos;
    positionOf(pos, mpPlayerActor.get());
    u8 kaze = m788;
    m787 = 0;
    if (kaze != 0) {
        f32 d = horizontalDistance(cameraArgument(&sKazeCenter),
                                   cameraArgument(pos.get())); /* dCamMath::xyzHorizontalDistance */
        if (d < sKazeRadius.get())
            m787 = 1;
    }
    u8 sea = m780;
    m786 = 0;
    if (sea != 0) {
        f32 d = horizontalDistance(cameraArgument(&sSeaCenter), cameraArgument(pos.get()));
        if (d < sSeaRadius.get())
            m786 = 1;
    }
}

/* 02508804 */
// relationalPos(fopAc_ac_c*, fopAc_ac_c*, cXyz*, f32)
static void dCamera_relationalPos2(dCamera_c* i_this, cXyz* ret, fopAc_ac_c* actor1,
                                   fopAc_ac_c* actor2, cXyz* offset, f32 scale) {

    if (actor1 == nullptr) {
        if (ret == nullptr) {
            ret = (cXyz*)operator_new(0xC);
            if (ret == nullptr)
                return;
        }
        ret->copy(*&sZeroVector); /* cXyz::Zero */
        return;
    }
    if (actor2 == nullptr) {
        i_this->relationalPos(ret, actor1, offset);
        return;
    }
    gabi::Local<cXyz> pos1, pos2, delta, half, mid, delta2, res;
    gabi::Local<CameraGlobe> deltaGlobe, offsetGlobe;
    gabi::Local<CameraAngle> dir, sum, tmp, diff;
    i_this->attentionPos(pos1, actor1);
    i_this->attentionPos(pos2, actor2);
    cXyz_mi(pos2, delta, pos1);
    cXyz_ml(delta, half, 0.5f);
    cXyz_pl(pos1, mid, half);
    cXyz_mi(pos2, delta2, pos1);
    globeFromVectorDiscard(cameraArgument(deltaGlobe.get()), cameraArgument(delta2.get()));
    globeFromVectorDiscard(cameraArgument(offsetGlobe.get()), cameraArgument(offset));
    i_this->directionOf(dir, actor1);
    angleAdd(cameraArgument(dir.get()), cameraArgument(sum.get()),
             cameraArgument(&offsetGlobe->mU));
    CameraAngle* a = gabi::at<CameraAngle>(
        angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)*sum)));
    offsetGlobe->mU = (s16)*a;
    angleSubtract(cameraArgument(&i_this->mViewCache.mDirection.mU), cameraArgument(diff.get()),
                  cameraArgument(&deltaGlobe->mU)); /* cSAngle::operator- */
    f32 r = gabi::fmuls_ppc(deltaGlobe->mRadius, 0.5f);
    f32 c = angleCos(cameraArgument(diff.get())); /* cSAngle::Cos */
    deltaGlobe->mRadius = gabi::fmuls_ppc(gabi::fmuls_ppc(r, c), scale);
    globeToVector(cameraArgument(deltaGlobe.get()), cameraArgument(delta.get())); /* cSGlobe::Xyz */
    cXyz_pl(mid, delta2, delta);
    globeToVector(cameraArgument(offsetGlobe.get()), cameraArgument(half.get()));
    cXyz_pl(delta2, res, half);
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(0xC);
        if (ret == nullptr)
            return;
    }
    ret->copy(*res);
}

/* 025051BC */
// lineBGCheckBack(cXyz* start, cXyz* end, u32 flags)
static bool dCamera_lineBGCheckBack(dCamera_c* i_this, cXyz* start, cXyz* end, u32 flags) {

    gabi::Local<LineCheck> linChk;
    u32 o = gabi::ea(linChk.get());
    camLinChk_ct(o);
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mLine.mBackFlag, 1);  /* OnBackFlag */
    fieldWrite<u8>(gabi::at<LineCheck>(o)->mLine.mFrontFlag, 0); /* OffFrontFlag */
    bool ret = i_this->lineBGCheck(start, end, (u8*)linChk.get(), flags);
    camLinChk_dt_vtables(o);
    lineCheckDestroy(cameraArgument(o), cameraArgument(0));
    return ret;
}

/* stack dBgS_GndChk / dBgS_CamGndChk_Wtr (HD: constructors and destructors partly inlined) */
static void gndChk_ct(u32 o, const cXyz* pos) {
    groundCheckConstruct(cameraArgument(o)); /* cBgS_GndChk() */
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbLinkThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbRopeThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbObjThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbCamThrough, 0);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_Chk::__vtbl, kGroundCheckVtable);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbArrowThrough, 0);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kGroundGroupVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrp, 1);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mPolyPassChk, o + 0x40);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrpPassChk, o + 0x4C);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBoomerangThrough, 0);
    (&gabi::at<dBgS_GndChk>(o)->m_pos)->copy(*pos); /* SetPos */
}
static void camGndChkWtr_ct(u32 o) {
    groundCheckConstruct(cameraArgument(o));
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mPolyPassChk, o + 0x40);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrpPassChk, o + 0x4C);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbObjThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbCamThrough, 1);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbLinkThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBoomerangThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbRopeThrough, 0);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_Chk::__vtbl, kWaterGroundCheckVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kWaterGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kWaterGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kWaterGroundGroupVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrp, 2);
}
static void gndChk_dt(u32 o) {
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kGroupPassVtable);
    groundCheckDestroy(cameraArgument(o), cameraArgument(0)); /* ~cBgS_Chk */
}

/* 024FB768 */
f32 dCamera_c::groundHeight(cXyz* pos) {

    gabi::Local<dBgS_GndChk> gnd, wtr;
    u32 g = gabi::ea(gnd.get()), w = gabi::ea(wtr.get());
    gndChk_ct(g, pos);
    f32 gndY = backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                     cameraArgument(g)); /* GroundCross */
    camGndChkWtr_ct(w);
    (&gabi::at<dBgS_GndChk>(w)->m_pos)->copy(*pos);
    f32 wtrY = backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(w));
    f32 y = (wtrY - gndY >= 0.0f) ? wtrY : gndY; /* fsel */
    if (y == -1000000000.0f)
        y = pos->y;
    gndChk_dt(w);
    gndChk_dt(g);
    return y;
}

/* 024FCEEC */
f32 dCamera_c::getWaterSurfaceHeight(cXyz* pos) {

    u32 yBits = fieldRead<u32>(pos->y);
    f32 best = -1000000000.0f;
    u32 xBits = fieldRead<u32>(pos->x);
    u32 zBits = fieldRead<u32>(pos->z);
    gabi::Local<RoofCheck> roof;
    gabi::Local<dBgS_GndChk> gnd;
    u32 r = gabi::ea(roof.get()), g = gabi::ea(gnd.get());
    roofCheckConstruct(cameraArgument(r));                         /* dBgS_RoofChk() */
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.x, xBits); /* SetPos */
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.z, zBits);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.y, yBits);
    f32 roofY = backgroundRoofCheck(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                    cameraArgument(r)); /* dBgS::RoofChk */
    if (std::bit_cast<f32>(yBits) < roofY)
        yBits = std::bit_cast<u32>(roofY);
    camGndChkWtr_ct(g);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(g)->m_pos.x, xBits); /* SetPos */
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(g)->m_pos.z, zBits);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(g)->m_pos.y, yBits);
    f32 gndY = backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(g));
    f32 h = gndY + 5.0f;
    if (h > pos->y)
        best = h;
    if (daSea_ChkArea(pos->x, pos->z)) {
        f32 wave = daSea_calcWave(pos->x, pos->z) + 20.0f;
        if (wave > pos->y && wave > best)
            best = wave;
    }
    if (best == -1000000000.0f)
        best = pos->y;
    gndChk_dt(g);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.dBgS_GrpPassChk::__vtbl, kGroupPassVtable);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.cBgS_Chk::__vtbl, kRoofCheckVtable);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.dBgS_PolyPassChk::__vtbl, kRoofPassVtable);
    lineCheckDestroy(cameraArgument(r + 0x10), cameraArgument(0));
    return best;
}

/* 0250252C */
CameraAngle* dCamera_c::calcPeepAngle(CameraAngle* ret) {

    gabi::Local<CameraAngle> res;
    angleCopyDiscard(cameraArgument(res.get()),
                     cameraArgument(&sZeroAngle)); /* cSAngle res(cSAngle::_0) */
    for (int side = 0; side < 2; side++) {
        s32 pad = mPadId;
        u32 status0 = dComIfGp_getPlayerStatus0(pad); /* check_owner_action */
        if (!(status0 & (side == 0 ? 0x20u : 0x40u)))
            continue;
        gabi::Local<cXyz> off1, off2, p1, p2;
        gabi::Local<LineCheck> linChk;
        gabi::Local<CameraAngle> dir, d, s;
        gabi::Local<CameraGlobe> globe;
        off1->x = 0.0f;
        off1->z = -30.0f;
        off1->y = 0.0f;
        off2->z = -30.0f;
        off2->y = 0.0f;
        off2->x = side == 0 ? -50.0f : 50.0f;
        relationalPos(p1, mpPlayerActor.get(), off1);
        relationalPos(p2, mpPlayerActor.get(), off2);
        u32 o = gabi::ea(linChk.get());
        camLinChk_ct(o);
        if (lineBGCheck(p2, p1, (u8*)linChk.get(), 0x7F)) {
            u32 pla = backgroundTrianglePlane(
                cameraArgument(dComIfGp_ea() + PLAY_BGS),
                cameraArgument(
                    (u32)fieldRead<u16>(gabi::at<LineCheck>(o)->mLine.cBgS_PolyInfo::mBgIndex)),
                cameraArgument((u32)fieldRead<u16>(
                    gabi::at<LineCheck>(o)
                        ->mLine
                        .cBgS_PolyInfo::mPolyIndex))); /* dComIfG_Bgsp()->GetTriPla(lin_chk) */
            CameraGlobe* g = gabi::at<CameraGlobe>(globeFromVectorValue(
                cameraArgument(globe.get()), cameraArgument(pla))); /* cSGlobe(plane->mNormal) */
            directionOf(dir, mpPlayerActor.get());
            angleSubtract(cameraArgument(&g->mU), cameraArgument(d.get()),
                          cameraArgument(dir.get())); /* U() - directionOf(player) */
            angleAdd(cameraArgument(side == 0 ? &sRightAngle : &sThreeQuarterAngle),
                     cameraArgument(s.get()), cameraArgument(d.get())); /* cSAngle::_90 / _270 + */
            *res = (s16)*s;
        }
        camLinChk_dt_vtables(o);
        lineCheckDestroy(cameraArgument(o), cameraArgument(0));
        break;
    }
    return gabi::at<CameraAngle>(
        angleCopyValue(cameraArgument(ret), cameraArgument(res.get()))); /* cSAngle(res) */
}

/* 024FC780 */
// HD: the half-angles come from the 1280x720 scissor; the actor delta is normalized in place but its unnormalized value is what gets scaled by 50 (cXyz::normalize returns a copy)
f32 dCamera_c::radiusActorInSight6(fopAc_ac_c* a1, fopAc_ac_c* a2, cXyz* center, cXyz* eye,
                                   f32 fovY, s16 bank) {

    gabi::Local<cXyz> d, p1, p2, dd, n, t2, v;
    gabi::Local<CameraGlobe> g, g1, g2;
    gabi::Local<CameraAngle> A, B, tmp, D;
    gabi::Local<Mtx34> mtx;
    cXyz_mi(eye, d, center);
    globeFromVectorDiscard(cameraArgument(g.get()),
                           cameraArgument(d.get())); /* cSGlobe(eye - center) */
    attentionPos(p1, a1);
    attentionPos(p2, a2);
    cXyz_mi(p1, dd, p2);
    vectorNormalize(cameraArgument(dd.get()), cameraArgument(n.get())); /* dd.normalize() */
    cXyz_ml(dd, n, 50.0f);
    PSVECAdd(p1, n, p1);
    cXyz_ml(dd, n, 50.0f);
    vectorSubtract(cameraArgument(p2.get()), cameraArgument(n.get()),
                   cameraArgument(p2.get())); /* PSVECSubtract */
    s32 camId = cameraGetParam(cameraArgument(mpCamera.get()));
    s32 winId = cameraInfo(camId)->mDlstWindowIdx.get();
    CameraWindowCoreHD* window = cameraWindow(winId);
    f32 half = fovY * 0.5f;
    f32 h = window->mViewport.mScissor.mHeight.get(); /* scissor height */
    angleFromDegrees(cameraArgument(A.get()), cameraArgument(half * (h / 720.0f) * 0.95f));
    f32 w = window->mViewport.mScissor.mWidth.get(); /* scissor width */
    f32 ws = w / 1280.0f;
    angleFromDegrees(cameraArgument(B.get()),
                     cameraArgument(fovY * mWindowAspectRatio * 0.5f * ws * 0.95f));
    cXyz_mi(eye, n, p1);
    globeFromVectorDiscard(cameraArgument(g1.get()), cameraArgument(n.get()));
    cXyz_mi(eye, d, p2);
    globeFromVectorDiscard(cameraArgument(g2.get()), cameraArgument(d.get()));
    cSAngle_ct(tmp);
    u32 flags = 0;
    struct {
        CameraAngle* a;
        CameraAngle* b;
        CameraAngle* lim;
        u32 bit;
    } checks[4] = {{&g1->mU, &g->mU, B, 1},
                   {&g1->mV, &g->mV, A, 2},
                   {&g2->mU, &g->mU, B, 4},
                   {&g2->mV, &g->mV, A, 8}};
    for (auto& c : checks) {
        angleSubtract(cameraArgument(c.a), cameraArgument(D.get()),
                      cameraArgument(c.b)); /* cSAngle::operator- */
        *tmp = (s16)*D;
        angleNegate(cameraArgument(c.lim), cameraArgument(D.get())); /* -lim */
        s16 t = *tmp;
        if (t < (s16)*D || t > (s16)*c.lim)
            flags |= c.bit;
    }
    if (flags == 0)
        return 0.0f;
    f32 radius = 0.0f;
    matrixLookAt(cameraArgument(mtx.get()), cameraArgument(eye), cameraArgument(center),
                 cameraArgument(&mUp), cameraArgument(bank)); /* mDoMtx_lookAt */
    for (int k = 0; k < 2; k++) {
        u32 fx = k == 0 ? 1u : 4u, fy = k == 0 ? 2u : 8u;
        if (!(flags & (fx | fy)))
            continue;
        PSMTXMultVec(mtx, k == 0 ? p1.get() : p2.get(), v);
        if (flags & fx) {
            f32 ax = std::fabs((f32)v->x);
            f32 t = angleTan(cameraArgument(B.get())); /* Tan */
            f32 r = ax / t + v->z;
            if (radius < r)
                radius = r;
        }
        if (flags & fy) {
            f32 ay = std::fabs((f32)v->y);
            f32 t = angleTan(cameraArgument(A.get()));
            f32 r = ay / t + v->z;
            if (radius < r)
                radius = r;
        }
    }
    return radius;
}

/* one probe of forwardCheckAngle (i = 0, 1): the pitch towards the ground ahead, weighted */
static s16 fwdProbe(dCamera_c* c, cXyz* pos, f32 h, s32 i, s16* zero) {
    gabi::Local<CameraAngle> chk, w;
    gabi::Local<CameraGlobe> g, g1;
    gabi::Local<cXyz> x, target, d, n, m, t, cross;
    s32 deg = gabi::ftoi(cameraSetup(c)->mForward[i == 0 ? 0 : 1].mCheckAngle.get() *
                         182.04445f); /* FwdChkAngle(i) */
    angleConstructDiscard(cameraArgument(chk.get()), cameraArgument((s16)deg));
    s16 inv = cSAngle_Inv(&c->mDirection.mU);
    f32 dist = h * cameraSetup(c)->mForward[i == 0 ? 0 : 1].mDistance.get(); /* FwdDistance(i) */
    globeConstructPolar(cameraArgument(g.get()), cameraArgument(dist), cameraArgument((s16)*chk),
                        cameraArgument(inv)); /* cSGlobe(f32, s16, s16) */
    globeToVector(cameraArgument(g.get()), cameraArgument(x.get()));
    cXyz_pl(pos, target, x);
    bool useGroundTarget = false;
    if (lineCheckCross(cameraArgument(c), cameraArgument(pos), cameraArgument(target.get()),
                       cameraArgument(cross.get()),
                       cameraArgument(0x7F))) { /* lineBGCheck(pos, target, &cross, 0x7f) */
        cXyz_mi(target, d, pos);
        f32 len = std_sqrtf(PSVECSquareMag(d));
        if (len < 1.0f) {
            target->copy(*cross);
            f32 gh = c->groundHeight(target);
            cross->y = gh + h;
            useGroundTarget = true;
        } else {
            vectorNorm(cameraArgument(d.get()), cameraArgument(n.get())); /* norm */
            cXyz_ml(n, m, cameraSetup(c)->mForwardBackMargin.get() /* FwdBackMargin */);
            cXyz_mi(cross, t, m);
            target->copy(*t);
        }
    }
    if (!useGroundTarget) {
        cross->copy(*target);
        f32 gh = c->groundHeight(target);
        cross->y = gh + h;
    }
    cXyz_mi(cross, x, pos);
    globeFromVectorDiscard(cameraArgument(g1.get()), cameraArgument(x.get()));
    if (i == 0)
        *zero = sZeroAngle.get(); /* cSAngle::_0 */
    f32 weight =
        (s16)g1->mV < *zero
            ? cameraSetup(c)->mForward[i == 0 ? 0 : 1].mWeightLow.get()   /* FwdWeightL(i) */
            : cameraSetup(c)->mForward[i == 0 ? 0 : 1].mWeightHigh.get(); /* FwdWeightH(i) */
    angleMultiply(cameraArgument(&g1->mV), cameraArgument(w.get()), cameraArgument(weight));
    return *w;
}

/* 024FB930 */
// HD: the player height comes from heightOf(); the line checks are the cross-point overload; the result is copied into the return slot by value
void dCamera_c::forwardCheckAngle(CameraAngle* ret) {

    gabi::Local<LineCheck> linChk; /* dBgS_CamLinChk_NorWtr (unused) */
    gabi::Local<CameraAngle> r, a, b, neg;
    gabi::Local<cXyz> pos;
    u32 lc = gabi::ea(linChk.get());
    camLinChk_ct(lc);
    angleCopyDiscard(cameraArgument(r.get()),
                     cameraArgument(&sZeroAngle)); /* cSAngle ret(cSAngle::_0) */
    cSAngle_ct(a);
    cSAngle_ct(b);
    positionOf(pos, mpPlayerActor.get());
    f32 h = heightOf(mpPlayerActor.get());
    pos->y = pos->y + h;
    s16 zero;
    *a = fwdProbe(this, pos, h, 0, &zero);
    *b = fwdProbe(this, pos, h, 1, &zero);
    s16 av = *a, bv = *b;
    CameraAngle* src;
    if (av >= zero && bv >= zero) {
        src = av < bv ? b.get() : a.get();
    } else if (av <= zero && bv <= zero) {
        src = av > bv ? b.get() : a.get();
    } else {
        src = a.get();
    }
    angleNegate(cameraArgument(src), cameraArgument(neg.get())); /* -src */
    s16 res = *neg;
    *r = res;
    if (!(res > zero))
        angleMultiplyAssign(cameraArgument(r.get()), cameraArgument(0.75f)); /* ret *= 0.75f */
    angleCopyDiscard(cameraArgument(ret), cameraArgument(r.get()));
    camLinChk_dt_vtables(lc);
    lineCheckDestroy(cameraArgument(lc), cameraArgument(0));
}

/* 024F9044 */
// HD: the ground under the player's head is always checked with a camera+object ground check (the result is copied into mBG.m5C when it is higher); GetPolyCamId/GetRoomCamId take the poly info by index
void dCamera_c::checkGroundInfo() {

    /* The memberwise copy mBG.m5C.m04 = gnd_chk below also copies gnd_chk's internal (virtual
     * base) pointers, i.e. addresses inside this stack frame: gnd_chk is placed where the original
     * frame (0x1E0 bytes) has it, at frame + 0x48, so the copied values are the same. */
    gabi::Local<GroundFrame> frame;
    gabi::Local<cXyz> pp, pos, dummy, t, A, B, d;
    gabi::Local<dBgS_GndChk> g2;
    gabi::Local<RoofCheck> roof;
    gabi::Local<CameraAngle> dir, ang;
    positionOf(pp, mpPlayerActor.get());
    u32 gyBits = fieldRead<u32>(pp->y);
    u32 gzBits = fieldRead<u32>(pp->z);
    u32 gxBits = fieldRead<u32>(pp->x);
    f32 h = heightOf(mpPlayerActor.get());
    pp->y = std::bit_cast<f32>(gyBits) + h;
    u32 r = gabi::ea(roof.get());
    roofCheckConstruct(cameraArgument(r));                          /* dBgS_RoofChk() */
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.x, gxBits); /* SetPos(gnd_chk_pos) */
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.y, gyBits);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.m_pos.z, gzBits);
    f32 roofY = backgroundRoofCheck(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                    cameraArgument(r)); /* dBgS::RoofChk */
    if (std::bit_cast<f32>(gyBits) < roofY)
        gyBits = std::bit_cast<u32>(roofY);

    /* dBgS_CamGndChk gnd_chk (camera + object) at the player's head */
    u32 o = gabi::ea(&frame->mGround);
    groundCheckConstruct(cameraArgument(o));
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mPolyPassChk, o + 0x40);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrpPassChk, o + 0x4C);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_Chk::__vtbl, kCameraGroundCheckVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kCameraGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kCameraGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kCameraGroundGroupVtable);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbObjThrough, 1);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbCamThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbLinkThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbBoomerangThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::mbRopeThrough, 0);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrp, 1);
    (&gabi::at<dBgS_GndChk>(o)->m_pos)->copy(*pp);
    f32 groundY =
        backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(o));
    u32 m = ea() + 0x2C0;                                                        /* mBG.m5C.m04 */
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->dBgS_PolyPassChk::mbCamThrough, 1); /* SetCam() */
    (&gabi::at<dBgS_GndChk>(m)->m_pos)->copy(*pp);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->dBgS_PolyPassChk::mbObjThrough, 0); /* ClrObj() */
    f32 camY = backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(m));
    f32 m5C;
    if (camY < groundY) {
        /* mBG.m5C.m04 = gnd_chk (memberwise copy) */
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mGrpPassChk,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mGrpPassChk));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mpBgW,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mpBgW));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->m_pos.y,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->m_pos.y));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mWallPrecheck,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mWallPrecheck));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mActorId,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mActorId));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mPolyPassChk,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mPolyPassChk));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->m_pos.x,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->m_pos.x));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mGndPrecheck,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mGndPrecheck));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mActorPid,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mActorPid));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mGrp,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mGrp));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->m_pos.z,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->m_pos.z));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mFlag,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mFlag));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mSameActorChk,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mSameActorChk));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbArrowThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbArrowThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbObjThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbObjThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbLinkThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbLinkThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbCamThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbCamThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbBombThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbBombThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbRopeThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbRopeThrough));
        fieldWrite<u8>(gabi::at<dBgS_GndChk>(m)->mbBoomerangThrough,
                       fieldRead<u8>(gabi::at<dBgS_GndChk>(o)->mbBoomerangThrough));
        fieldWrite<u16>(gabi::at<dBgS_GndChk>(m)->cBgS_PolyInfo::mPolyIndex,
                        fieldRead<u16>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::mPolyIndex));
        fieldWrite<u16>(gabi::at<dBgS_GndChk>(m)->cBgS_PolyInfo::mBgIndex,
                        fieldRead<u16>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::mBgIndex));
        fieldWrite<u32>(gabi::at<dBgS_GndChk>(m)->mNowY,
                        fieldRead<u32>(gabi::at<dBgS_GndChk>(o)->mNowY));
        mBG.m5C.m58 = groundY;
        m5C = groundY;
    } else {
        mBG.m5C.m58 = camY;
        m5C = camY;
    }
    fieldWrite<u32>(mBG.m00.m04.m_pos.x, gxBits); /* mBG.m00.m04.SetPos(&gnd_chk_pos) */
    fieldWrite<u32>(mBG.m00.m04.m_pos.y, gyBits);
    fieldWrite<u32>(mBG.m00.m04.m_pos.z, gzBits);
    mBG.m5C.m00 = m5C != -1000000000.0f;
    f32 m00 = backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                    cameraArgument(ea() + 0x264));
    mBG.m00.m58 = m00;
    m354 = m00;
    mBG.m00.m00 = m00 != -1000000000.0f;
    f32 foot = footHeightOf(mpPlayerActor.get());
    m360 =
        !(foot - mBG.m5C.m58 > fieldRead<f32>(cameraSetup(this)->mFloorMargin) /* FloorMargin */);

    m31D = 0;
    m33C = nullptr;
    if (backgroundIsMoving(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                           cameraArgument(ea() + 0x2D4))) { /* ChkMoveBG */
        fopAc_ac_c* a = gabi::at<fopAc_ac_c>(backgroundActor(
            cameraArgument(dComIfGp_ea() + PLAY_BGS),
            cameraArgument((u32)fieldRead<u16>(mBG.m5C.m04.mBgIndex)))); /* GetActorPointer */
        m33C = a;
        if (a != nullptr) {
            positionOf(pos, a);
            directionOf(dir, m33C.get());
            if (m31C != 0) {
                cXyz_mi(&m32C, t, pos);
                m320.copy(*t);
                angleSubtract(cameraArgument(&m33A), cameraArgument(ang.get()),
                              cameraArgument(dir.get())); /* m33A - angle */
                fopAc_ac_c* mv = m33C.get();
                m338 = (s16)*ang;
                if (mv != nullptr && fpcM_GetName(mv) == 0x39 /* Obj_Pirateship */) {
                    mViewCache.mCenter.y = gabi::fmadds(
                        m320.y, fieldRead<f32>(cameraSetup(this)->mMovingShipScale) /* m0B8 */,
                        mViewCache.mCenter.y);
                }
            }
            m31C = 1;
            if (eventCameraPlaying() == 0 && !chkFlag(0x20000000) && m360 != 0)
                m31D = 1;
            if (m31D != 0) {
                backgroundCorrectPosition(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                          cameraArgument(ea() + 0x2D4), cameraArgument(1),
                                          cameraArgument(&mViewCache.mCenter), cameraArgument(0),
                                          cameraArgument(0));
                backgroundCorrectPosition(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                          cameraArgument(ea() + 0x2D4), cameraArgument(1),
                                          cameraArgument(&mViewCache.mEye), cameraArgument(0),
                                          cameraArgument(0));
                cXyz_mi(&mViewCache.mEye, t, &mViewCache.mCenter);
                cSGlobe_Val(&mViewCache.mDirection, t);
            }
            m33A = (s16)*dir;
            m32C.copy(*pos);
        }
    } else {
        m31C = 0;
    }
    m350 = mBG.m5C.m00 != 0 ? backgroundMoveCamera(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                                   cameraArgument(ea() + 0x2D4)) /* GetCamMoveBG */
                            : 0;
    mRoomNo = -1;
    bool swim = false;
    if (mBG.m00.m00 != 0) {
        s32 pad = mPadId;
        swim = (dComIfGp_getPlayerStatus0(pad) & 0x100000) != 0; /* SWIM */
    }
    if (swim) {
        mRoomMapToolCameraIdx = backgroundPolyCamera(
            cameraArgument(dComIfGp_ea() + PLAY_BGS),
            cameraArgument((u32)fieldRead<u16>(mBG.m00.m04.mBgIndex)),
            cameraArgument((u32)fieldRead<u16>(mBG.m00.m04.mPolyIndex))); /* GetPolyCamId */
    } else if (m360 == 0) {
        mRoomMapToolCameraIdx = 0x1FF;
    } else if (mBG.m5C.m00 != 0) {
        s32 idx = backgroundRoomCamera(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                       cameraArgument(ea() + 0x2D4)); /* GetRoomCamId */
        mRoomMapToolCameraIdx = idx;
        if (idx == 0xFF) {
            mRoomMapToolCameraIdx =
                backgroundPolyCamera(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                     cameraArgument((u32)fieldRead<u16>(mBG.m5C.m04.mBgIndex)),
                                     cameraArgument((u32)fieldRead<u16>(mBG.m5C.m04.mPolyIndex)));
        } else {
            mRoomNo = backgroundRoom(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                     cameraArgument(ea() + 0x2D4)); /* GetRoomId */
        }
    } else {
        mRoomMapToolCameraIdx = 0xFF;
    }
    if (daSea_ChkArea(pp->x, pp->z)) {
        f32 w = daSea_calcWave(pp->x, pp->z);
        m314 = 1;
        m318 = w;
    } else {
        m314 = 0;
        m318 = -1000000000.0f;
    }
    if (m354 < m318)
        m354 = (f32)m318;

    u32 o2 = gabi::ea(g2.get());
    gndChk_ct(o2, &mEye); /* dBgS_GndChk gnd_chk_2; SetPos(&mEye) */
    f32 eyeGround =
        backgroundGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS), cameraArgument(o2));
    if (eyeGround < mBG.m5C.m58 + 40.0f) {
        A->copy(mEye);
        attentionPos(t, mpPlayerActor.get());
        B->copy(*t);
        cXyz_mi(A, d, B);
        cXyz_ml(d, t, 0.5f);
        PSVECAdd(B, t, B);
    } else {
        B->copy(mEye);
        attentionPos(t, mpPlayerActor.get());
        A->copy(*t);
        cXyz_mi(B, d, A);
        cXyz_ml(d, t, 0.5f);
        PSVECAdd(A, t, A);
    }
    if (m360 != 0) {
        u32 res = lineCollisionCheckBush(A, B) & 5;
        m364 = res;
        if (res & 4) {
            fieldWrite<u32>(
                m368, fieldRead<u32>(cameraSetup(this)->mBushHigh)); /* m368 = mCamSetup.m0C0 */
            res = m364;
        }
        if (res & 1) {
            fieldWrite<u32>(
                m368, fieldRead<u32>(cameraSetup(this)->mBushLow)); /* m368 = mCamSetup.m0C4 */
            res = m364;
        }
        if (res != 0)
            massCameraTop(cameraArgument(cameraMassManager()),
                          cameraArgument(&m36C)); /* GetMassCamTopPos */
    } else {
        m364 = 0;
        m368 = 0.0f;
    }
    gndChk_dt(o2);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kGroupPassVtable);
    groundCheckDestroy(cameraArgument(o), cameraArgument(0));
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.cBgS_Chk::__vtbl, kRoofCheckVtable);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.dBgS_PolyPassChk::__vtbl, kRoofPassVtable);
    fieldWrite<u32>(gabi::at<RoofCheck>(r)->mRoof.dBgS_GrpPassChk::__vtbl, kGroupPassVtable);
    lineCheckDestroy(cameraArgument(r + 0x10), cameraArgument(0));
}

/* 024FD11C */
// HD: the gaze back margin grows while swimming / in mode 4 (debug-register tuned); a missing second wall plane counts as no hit; while swimming the eye keeps a register-defined distance to the player; the corner case moves V towards the corner (GameCube wrote it into U); the wall-up offset tilts V by +90 degrees
bool dCamera_c::bumpCheck(u32 flags) {

    u32 res = 0;
    f32 gaze = fieldRead<f32>(cameraSetup(this)->mGazeBackMargin); /* GazeBackMargin */
    s32 pad = mPadId;
    u32 st1 = dComIfGp_getPlayerStatus1(pad);
    f32 cornerMax = fieldRead<f32>(cameraSetup(this)->mCornerAngleMax); /* CornerAngleMax */
    f32 cushion = fieldRead<f32>(cameraSetup(this)->mCornerCushion);    /* CornerCushion */
    gabi::Local<u8[8]> deg;
    if (st1 & 0x100000) {
        gaze = sRegisters.mFloat[0xFC4 / sizeof(f32)].get() + 3.0f;
    } else if (mCurMode == 4) {
        gaze = sRegisters.mFloat[0x1174 / sizeof(f32)].get() + 5.0f;
    }
    u32 dg = degreeConstruct(cameraArgument(deg.get()),
                             cameraArgument(cornerMax)); /* cDegree(CornerAngleMax) */
    f32 cosMax = degreeCos(cameraArgument(dg));          /* .Cos() */
    fopAc_ac_c* player = mpPlayerActor.get();
    f32 wallUp = fieldRead<f32>(cameraSetup(this)->mWallUpDistance); /* WallUpDistance */
    if (player != nullptr && fpcM_GetName(player) == 0xA8) {
        u32 vt = player->__vtbl.get();
        u32 grabId = gabi::call_ptr<u32>(gabi::at<PlayerVtable>(vt)->getGrabActorID.get(),
                                         player); /* getGrabActorID() */
        if (grabId != 0xFFFFFFFFu) {
            gabi::Local<be<u32>> key;
            *key = grabId;
            fopAc_ac_c* grab = fopAcIt_Judge(0x025E1234, key.get());
            if (grab != nullptr) {
                s16 name = fpcM_GetName(grab);
                if (name == 0x1C5 /* TSUBO */) {
                    u32 type = objectParameter(cameraArgument(grab), cameraArgument(4),
                                               cameraArgument(0x18)); /* daObj::PrmAbstract */
                    bool tall = (type >= 1 && type <= 4) || (type >= 7 && type <= 8) ||
                                (type >= 13 && type <= 15);
                    wallUp = tall ? 150.0f : 110.0f;
                } else if (name == 0x16F /* NPC_MD */) {
                    wallUp = 130.0f;
                } else if (name == 0x1CA /* Obj_Try */) {
                    wallUp = 200.0f;
                } else {
                    wallUp = 110.0f;
                }
            }
        }
    }
    gabi::Local<cXyz> eye;
    gabi::Local<CameraGlobe> dir;
    eye->copy(mViewCache.mEye);
    globeCopy(
        cameraArgument(dir.get()),
        cameraArgument(&mViewCache.mDirection)); /* cSGlobe direction(mViewCache.mDirection) */

    if (chkFlag(0x2000) && mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> x, e;
        f32 r = radiusActorInSight(mpPlayerActor.get(), mpLockonTarget.get());
        f32 m;
        if (r > 0.0f) {
            f32 cur = m14C;
            if (!(r < 3500.0f))
                r = 3500.0f;
            m = gabi::fmadds(r - cur, 0.33f, cur);
            res = 0x40;
        } else {
            f32 cur = m14C;
            m = gabi::fnmsubs(cur, 0.08f, cur);
        }
        u32 t108 = m108;
        m14C = m;
        f32 k = 1.0f;
        if (t108 < 10)
            k = (f32)t108 / 10.0f;
        dir->mRadius = gabi::fmadds(m, k, dir->mRadius);
        globeToVector(cameraArgument(dir.get()), cameraArgument(x.get()));
        cXyz_pl(&mViewCache.mCenter, e, x);
        eye->copy(*e);
    }
    if ((flags & CheckBush) && m364 != 0) {
        gabi::Local<cXyz> d, x, e;
        gabi::Local<CameraGlobe> g;
        gabi::Local<CameraAngle> a, t, u, tmp;
        cXyz_mi(&m36C, d, &mViewCache.mCenter);
        globeFromVectorDiscard(cameraArgument(g.get()), cameraArgument(d.get()));
        if ((s16)dir->mV < (s16)g->mV) {
            angleCopyDiscard(cameraArgument(a.get()), cameraArgument(&mDirection.mV));
            angleSubtract(cameraArgument(&g->mV), cameraArgument(t.get()), cameraArgument(a.get()));
            angleMultiply(cameraArgument(t.get()), cameraArgument(u.get()), cameraArgument(0.05f));
            angleAddAssign(cameraArgument(a.get()), cameraArgument(u.get()));
            CameraAngle* v = gabi::at<CameraAngle>(
                angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)*a)));
            dir->mV = (s16)*v;
            globeToVector(cameraArgument(dir.get()), cameraArgument(x.get()));
            cXyz_pl(&mViewCache.mCenter, e, x);
            eye->copy(*e);
            res |= 0x20;
        }
    }

    gabi::Local<LineCheck> lc1, lc2;
    u32 o1 = gabi::ea(lc1.get()), o2 = gabi::ea(lc2.get());
    camLinChk_ct(o1);
    camLinChk_ct(o2);
    s32 type;
    if (lineBGCheck(&mCenter, eye, (u8*)lc1.get(), flags)) {
        do {
            gabi::Local<CameraAngle> unusedA;
            gabi::Local<CameraGlobe> unusedG, globe;
            gabi::Local<cXyz> c, w, w278, cross, mid, d, h, midc, out, nsum, n2, s, x, p, p2, c2,
                att;
            cSAngle_ct(unusedA);
            globeConstruct(cameraArgument(unusedG.get()));
            globeConstruct(cameraArgument(globe.get()));
            u32 pla1 = backgroundTrianglePlane(
                cameraArgument(dComIfGp_ea() + PLAY_BGS),
                cameraArgument(
                    (u32)fieldRead<u16>(gabi::at<LineCheck>(o1)->mLine.cBgS_PolyInfo::mBgIndex)),
                cameraArgument(
                    (u32)fieldRead<u16>(gabi::at<LineCheck>(o1)->mLine.cBgS_PolyInfo::mPolyIndex)));
            u32 pla2 = 0;
            if (!(flags & CheckBothDirections)) {
                type = WallHit;
            } else if (pla1 != 0 && lineBGCheck(eye, &mCenter, (u8*)lc2.get(), flags)) {
                pla2 = backgroundTrianglePlane(
                    cameraArgument(dComIfGp_ea() + PLAY_BGS),
                    cameraArgument((u32)fieldRead<u16>(
                        gabi::at<LineCheck>(o2)->mLine.cBgS_PolyInfo::mBgIndex)),
                    cameraArgument((u32)fieldRead<u16>(
                        gabi::at<LineCheck>(o2)->mLine.cBgS_PolyInfo::mPolyIndex)));
                if (pla2 == 0) {
                    mEye.copy(*eye);
                    fieldWrite<u32>(mDirection.mRadius, fieldRead<u32>(dir->mRadius));
                    fieldWrite<u32>(mDirection.mV, fieldRead<u32>(dir->mV));
                    type = NoHit;
                    break;
                }
                f32 dot = PSVECDotProduct((cXyz*)gabi::at<cXyz>(pla1), gabi::at<cXyz>(pla2));
                vectorCross(cameraArgument(pla1), cameraArgument(pla2),
                            cameraArgument(cross.get())); /* PSVECCrossProduct */
                if (dot > cosMax && std::fabs((f32)cross->y) > 0.5f)
                    type = CornerHit;
                else
                    type = sPreviousHitType.get() == 3 ? 5 : 4;
            } else {
                s32 prev = sPreviousHitType.get();
                type = (prev == 3 || prev == 5) ? 5 : 2;
            }

            if (type == CornerHit) {
                c->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));  /* lin_chk1.GetCross() */
                c2->copy(*(&gabi::at<LineCheck>(o2)->mLine.mLin.mEnd)); /* lin_chk2.GetCross() */
                res |= 2;
                cXyz_mi(c2, d, c);
                cXyz_ml(d, h, 0.5f);
                cXyz_pl(c, mid, h);
                midc->copy(*mid);
                if (twoPlaneLinePosition(
                        cameraArgument(pla1), cameraArgument(pla2), cameraArgument(midc.get()),
                        cameraArgument(out.get()))) { /* cM3d_2PlaneLinePosNearPos */
                    cXyz_pl(gabi::at<cXyz>(pla1), nsum, gabi::at<cXyz>(pla2));
                    n2->copy(*nsum);
                    cXyz_ml(n2, s, 2.0f);
                    cXyz_pl(out, nsum, s);
                    m070.copy(*nsum);
                    cXyz_mi(&m070, s, &mCenter);
                    cSGlobe_Val(globe, s);
                    gabi::Local<CameraAngle> t1, t2, t3, tv;
                    fieldWrite<u32>(mDirection.mRadius,
                                    fieldRead<u32>(dir->mRadius)); /* mDirection.R(direction.R()) */
                    angleSubtract(cameraArgument(&globe->mV), cameraArgument(t1.get()),
                                  cameraArgument(&mDirection.mV));
                    angleMultiply(cameraArgument(t1.get()), cameraArgument(t2.get()),
                                  cameraArgument(0.05f));
                    angleAdd(cameraArgument(&mDirection.mV), cameraArgument(t3.get()),
                             cameraArgument(t2.get()));
                    CameraAngle* v = gabi::at<CameraAngle>(
                        angleConstructValue(cameraArgument(tv.get()), cameraArgument((s16)*t3)));
                    mDirection.mV = (s16)*v;
                    angleSubtract(cameraArgument(&globe->mU), cameraArgument(t3.get()),
                                  cameraArgument(&mDirection.mU));
                    angleMultiply(cameraArgument(t3.get()), cameraArgument(t2.get()),
                                  cameraArgument(cushion));
                    angleAdd(cameraArgument(&mDirection.mU), cameraArgument(t1.get()),
                             cameraArgument(t2.get()));
                    v = gabi::at<CameraAngle>(
                        angleConstructValue(cameraArgument(tv.get()), cameraArgument((s16)*t1)));
                    mDirection.mU = (s16)*v;
                    globeToVector(cameraArgument(&mDirection), cameraArgument(s.get()));
                    cXyz_pl(&mCenter, p, s);
                    globe->mRadius = globe->mRadius + 50.0f;
                    globeToVector(cameraArgument(globe.get()), cameraArgument(s.get()));
                    cXyz_pl(&mCenter, p2, s);
                    if (!lineCheckSimple(
                            cameraArgument(this), cameraArgument(&mCenter),
                            cameraArgument(p2.get()),
                            cameraArgument(0x7F))) { /* lineBGCheck(&mCenter, &p2, 0x7f) */
                        if (lineBGCheck(&m070, p, (u8*)lc1.get(), 0x7F)) {
                            gabi::Local<cXyz> cc, o;
                            cc->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));
                            compWallMargin(o, cc, gaze);
                            p->copy(*o);
                        }
                        lineBGCheck(&mCenter, p, (u8*)lc1.get(), flags);
                        mEye.copy(*p);
                        mEventFlags = mEventFlags | 0x80000;
                        break;
                    }
                    type = WallHit;
                }
            }
            /* cases 2, 4, 5 (and 3 when the corner could not be used) */
            mEventFlags = mEventFlags | 0x80;
            c->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));
            compWallMargin(w, c, gaze + 0.5f);
            w278->copy(*w);
            if (chkFlag(8) && (flags & CheckWallUp) && type != NewWallHit) {
                f32 hd = horizontalDistance(
                    cameraArgument(c.get()),
                    cameraArgument(&mCenter)); /* dCamMath::xyzHorizontalDistance */
                attentionPos(att, mpPlayerActor.get());
                f32 cy = mCenter.y;
                f32 up = wallUp - (cy - att->y);
                if (!(hd < 20.0f)) {
                    if (hd > 320.0f)
                        up = 0.0f;
                    else
                        up = up * (1.0f - (hd - 20.0f) / 300.0f);
                }
                if (w->y - cy < up) {
                    gabi::Local<CameraGlobe> g;
                    gabi::Local<CameraAngle> sa, tmp;
                    n2->copy(*gabi::at<cXyz>(pla1)); /* *plane1->GetNP() */
                    globeFromVectorDiscard(cameraArgument(g.get()), cameraArgument(n2.get()));
                    angleAdd(cameraArgument(&g->mV), cameraArgument(sa.get()),
                             cameraArgument(&sRightAngle)); /* V + cSAngle::_90 */
                    CameraAngle* v = gabi::at<CameraAngle>(
                        angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)*sa)));
                    g->mV = (s16)*v;
                    f32 sn = angleSin(cameraArgument(&g->mV)); /* Sin */
                    g->mRadius = up * sn;
                    globeToVector(cameraArgument(g.get()), cameraArgument(x.get()));
                    PSVECAdd(w, x, w);
                    if (lineBGCheck(w278, w, (u8*)lc1.get(), flags)) {
                        gabi::Local<cXyz> cc, o;
                        cc->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));
                        compWallMargin(o, cc, gaze);
                        w->copy(*o);
                        cXyz_mi(w, d, &mEye);
                        cXyz_ml(d, o,
                                fieldRead<f32>(cameraSetup(this)->mWallCushion) /* WallCushion */);
                        PSVECAdd(&mEye, o, &mEye);
                    } else {
                        cXyz_mi(w, d, &mEye);
                        cXyz_ml(d, h, fieldRead<f32>(cameraSetup(this)->mWallCushion));
                        PSVECAdd(&mEye, h, &mEye);
                    }
                    mEventFlags = mEventFlags | 0x4000;
                } else {
                    if (lineBGCheck(w278, w, (u8*)lc1.get(), flags)) {
                        gabi::Local<cXyz> cc, o;
                        cc->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));
                        compWallMargin(o, cc, gaze);
                        w->copy(*o);
                    }
                    cXyz_mi(w, d, &mEye);
                    cXyz_ml(
                        d, h,
                        fieldRead<f32>(cameraSetup(this)->mWallBackCushion) /* WallBackCushion */);
                    PSVECAdd(&mEye, h, &mEye);
                }
            } else {
                mEye.copy(*w);
            }
            {
                s32 pd = mPadId;
                if (dComIfGp_getPlayerStatus1(pd) & 0x100000) { /* swimming */
                    gabi::Local<CameraGlobe> g;
                    attentionPos(att, mpPlayerActor.get());
                    cXyz_mi(&mEye, d, att);
                    globeFromVectorDiscard(cameraArgument(g.get()), cameraArgument(d.get()));
                    f32 lim = sRegisters.mFloat[0xFC8 / sizeof(f32)].get() + 100.0f;
                    if (g->mRadius < lim) {
                        g->mRadius = lim;
                        globeToVector(cameraArgument(g.get()), cameraArgument(x.get()));
                        cXyz_pl(att, h, x);
                        mEye.copy(*h);
                    }
                }
            }
            {
                u32 alg = cameraStyle(this)->mAlgorithm.get(); /* mCamParam.Algorythmn() */
                if (alg == 1 || alg == 10) {                   /* FOLLOW, MANUAL */
                    gabi::Local<CameraGlobe> g;
                    attentionPos(att, mpPlayerActor.get());
                    cXyz_mi(&mEye, d, att);
                    globeFromVectorDiscard(cameraArgument(g.get()), cameraArgument(d.get()));
                    if (g->mRadius < 40.0f) {
                        g->mRadius = 40.0f;
                        globeToVector(cameraArgument(g.get()), cameraArgument(x.get()));
                        cXyz_pl(att, h, x);
                        mEye.copy(*h);
                    }
                }
            }
            cXyz_mi(&mEye, d, &mCenter);
            cSGlobe_Val(&mDirection, d);
            res |= 1;
        } while (false);
    } else {
        type = NoHit;
        if (chkFlag(0x4000)) {
            gabi::Local<CameraAngle> t, u, v2, tv;
            gabi::Local<cXyz> x, e;
            f32 k = (flags & CheckWallUp)
                        ? fieldRead<f32>(cameraSetup(this)->mWallBackCushion) /* WallBackCushion */
                        : 0.2f;
            f32 rr = mDirection.mRadius;
            mDirection.mRadius = gabi::fmadds(mViewCache.mDirection.mRadius - rr, k, rr);
            angleSubtract(cameraArgument(&mViewCache.mDirection.mV), cameraArgument(t.get()),
                          cameraArgument(&mDirection.mV));
            angleMultiply(cameraArgument(t.get()), cameraArgument(u.get()), cameraArgument(k));
            angleAdd(cameraArgument(&mDirection.mV), cameraArgument(v2.get()),
                     cameraArgument(u.get()));
            CameraAngle* a = gabi::at<CameraAngle>(
                angleConstructValue(cameraArgument(tv.get()), cameraArgument((s16)*v2)));
            mDirection.mV = (s16)*a;
            a = gabi::at<CameraAngle>(angleConstructValue(
                cameraArgument(tv.get()), cameraArgument((s16)mViewCache.mDirection.mU)));
            mDirection.mU = (s16)*a;
            globeToVector(cameraArgument(&mDirection), cameraArgument(x.get()));
            cXyz_pl(&mCenter, e, x);
            mEye.copy(*e);
            if (lineBGCheck(&mCenter, &mEye, (u8*)lc1.get(), flags)) {
                gabi::Local<cXyz> cc, o;
                cc->copy(*(&gabi::at<LineCheck>(o1)->mLine.mLin.mEnd));
                compWallMargin(o, cc, gaze + 0.5f);
                mEye.copy(*o);
            }
            angleSubtract(cameraArgument(&mDirection.mV), cameraArgument(t.get()),
                          cameraArgument(&mViewCache.mDirection.mV));
            f32 dd = angleDegrees(cameraArgument(t.get())); /* Degree */
            if (std::fabs(dd) < 0.2f)
                mEventFlags = mEventFlags & ~0x4000u;
        } else {
            mEye.copy(*eye);
            fieldWrite<u32>(mDirection.mRadius, fieldRead<u32>(dir->mRadius));
            fieldWrite<u32>(mDirection.mV, fieldRead<u32>(dir->mV));
        }
    }
    if (flags & CheckWater) {
        f32 wh = getWaterSurfaceHeight(&mEye);
        if (wh > mEye.y) {
            gabi::Local<cXyz> d;
            mEye.y = wh;
            cXyz_mi(&mEye, d, &mCenter);
            cSGlobe_Val(&mDirection, d);
            res |= 8;
        }
    }
    sPreviousHitType = type;
    if (m78B != 0) {
        if (camStyleAlg(mCurStyle) != 4 || !(mEventFlags & 0x10000800))
            mEye.y = mEye.y + 25.0f;
    }
    bool ret = res != 0;
    camLinChk_dt_vtables(o2);
    lineCheckDestroy(cameraArgument(o2), cameraArgument(0));
    camLinChk_dt_vtables(o1);
    lineCheckDestroy(cameraArgument(o1), cameraArgument(0));
    return ret;
}

/* WWHD glue (tools/verify/autoglue.py)
data sTrimSizes 1004A7EB
data sKazeInitialized 1046EFB0
data sSeaInitialized 1046EFB4
data sKazeCenter 1046EF78
data sSeaCenter 1046EF84
data sKazeRadius 101D5528
data sSeaRadius 101D552C
data sRightAngle 101FF358
data sThreeQuarterAngle 101FF35C
data sPreviousHitType 101D554C
data sRegisters 1047B60C

ext 02010C50 f32 planeSignedLength(u32, u32)
ext 0201B084 void vectorNorm(u32, u32)
ext 02516DF8 u32 massCameraResult(u32)
ext 02018150 void capsuleConstruct(u32)
ext 020181B0 void capsuleSet(u32, u32, u32, f32)
ext 02516DB4 void massSetCamera(u32, u32)
ext 0201819C void capsuleDestroy(u32, u32)
ext 024F1AFC void lineCheckSet(u32, u32, u32, u32)
ext 02008860 bool backgroundLineCross(u32, u32)
ext 02008FEC void lineCheckConstruct(u32)
ext 02008B4C void lineCheckDestroy(u32, u32)
ext 024EE8B8 void sphereCheckConstruct(u32)
ext 02018E88 void sphereSet(u32, u32, f32)
ext 024EF88C bool backgroundSphereCheck(u32, u32, u32)
ext 02007324 void globeFromVectorDiscard(u32, u32)
ext 020073AC void globeToVector(u32, u32)
ext 025F0C48 void projectPoint(u32, u32)
ext 024F7AC4 f32 horizontalDistance(u32, u32)
ext 020068B0 void angleSubtract(u32, u32, u32)
ext 02006838 f32 angleCos(u32)
ext 02008974 f32 backgroundGroundCross(u32, u32)
ext 024EE7AC void roofCheckConstruct(u32)
ext 024EF6E8 f32 backgroundRoofCheck(u32, u32)
ext 02006644 void angleCopyDiscard(u32, u32)
ext 020084C8 u32 backgroundTrianglePlane(u32, u32, u32)
ext 02007324 u32 globeFromVectorValue(u32, u32)
ext 02006644 u32 angleCopyValue(u32, u32)
ext 0201B31C void vectorNormalize(u32, u32)
ext 028E8DAC void vectorSubtract(u32, u32, u32)
ext 020066C0 void angleFromDegrees(u32, f32)
ext 02006880 void angleNegate(u32, u32)
ext 025F1EAC void matrixLookAt(u32, u32, u32, u32, u32)
ext 0200685C f32 angleTan(u32)
ext 0200658C void angleConstructDiscard(u32, u32)
ext 02007068 void globeConstructPolar(u32, f32, u32, u32)
ext 024FB608 bool lineCheckCross(u32, u32, u32, u32, u32)
ext 0200693C void angleMultiply(u32, u32, f32)
ext 020069A0 void angleMultiplyAssign(u32, f32)
ext 024EEABC bool backgroundIsMoving(u32, u32)
ext 02008438 u32 backgroundActor(u32, u32)
ext 024EFB08 void backgroundCorrectPosition(u32, u32, u32, u32, u32, u32)
ext 024EF324 s32 backgroundMoveCamera(u32, u32)
ext 024EEC9C s32 backgroundPolyCamera(u32, u32, u32)
ext 024EF340 s32 backgroundRoomCamera(u32, u32)
ext 024EF130 s32 backgroundRoom(u32, u32)
ext 02516E00 void massCameraTop(u32, u32)
ext 02006A3C u32 degreeConstruct(u32, f32)
ext 02006AE8 f32 degreeCos(u32)
ext 02048038 u32 objectParameter(u32, u32, u32)
ext 02007178 void globeCopy(u32, u32)
ext 020068CC void angleAddAssign(u32, u32)
ext 028E8D4C void vectorCross(u32, u32, u32)
ext 02017AEC bool twoPlaneLinePosition(u32, u32, u32, u32)
ext 024FCBE8 bool lineCheckSimple(u32, u32, u32, u32)
ext 02006814 f32 angleSin(u32)
ext 02006720 f32 angleDegrees(u32)
*/
