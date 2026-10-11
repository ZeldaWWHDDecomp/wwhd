/**
 * d_camera_proc.cpp (WWHD)
 * Camera process (camera_process_class): phases, execute, delete, view setup; dCamera_c
 * construction/destruction helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * camera_process_class (HD): view_class fields used here: near 0xCC, far 0xD0, fovy 0xD4,
 * aspect 0xD8, lookat eye 0xDC / center 0xE8 / up 0xF4, bank 0x100, view matrix 0x144,
 * no-trans view matrix 0x1E4; prm1..3 bytes 0x230..0x232; dCamera_c body at 0x248.
 */
#include "d/d_camera_proc.h"
#include <bit>

using namespace CameraProcessHD;
using CameraAngle = cSAngle_l;
using CameraGlobe = cSGlobe_l;
enum CameraImageAddress : u32 {
    kSafeStringVtable = 0x1004A4C4,
    kCameraGroundVtable = 0x1004A53C,
    kCameraGroupVtable = 0x1004A55C,
    kCameraPolyInfoVtable = 0x1004A54C,
    kCameraPassVtable = 0x1004A56C,
    kGroundPassVtable = 0x1004A52C,
    kGroundPolyInfoVtable = 0x1004A50C,
    kGroupBaseVtable = 0x1004A4EC,
    kGroundCheckVtable = 0x1004A4FC,
    kGroundGroupVtable = 0x1004A51C,
    kSeadCameraVtable = 0x101450D0,
    kSeadLookAtVtable = 0x101450F8,
    kPosSetConstructor = 0x0251558C,
    kCameraPhaseMethods = 0x101D55C0,
    kHyruleStage = 0x1004A754,
    kDemo07Name = 0x1004A75C,
    kDemo46Name = 0x1004A77C,
    kDemoName = 0x1047E6B8,
};
static be<u32>* playPlayerStatus() {
    return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerStatus[0][1];
}
static be<u8>* playWindowCount() {
    return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mDlstWindowNum;
}
static be<u8>* playScopeMessageStatus() {
    return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mScopeMesgStatus;
}
static be<s32>* playEventCameraPlaying() { return gabi::at<be<s32>>(dComIfGp_ea() + 0x52E4); }
static be<u8>* playEventRunning() {
    return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mMode;
}
enum : u32 { kSafeStringTerminate = 0x025155D4 };
template <class T, class Field> static T fieldRead(Field& field) {
    return reinterpret_cast<be<T>*>(&field)->get();
}
template <class T, class Field> static void fieldWrite(Field& field, T value) {
    *reinterpret_cast<be<T>*>(&field) = value;
}
static View* cameraView(camera_class* camera) { return reinterpret_cast<View*>(camera); }
static LookAtCamera* hdCamera(dCamera_c* body) {
    return reinterpret_cast<LookAtCamera*>(body->mHDLookAtCamera);
}
static Projection* hdProjection(dCamera_c* body) {
    return reinterpret_cast<Projection*>(body->mHDProjection);
}
static Zoom* hdZoom(dCamera_c* body) { return reinterpret_cast<Zoom*>(body->mHD730); }
static Setup* cameraSetup(dCamera_c* body) { return reinterpret_cast<Setup*>(body->mCamSetup); }
static dComIfG_camera_info_class* cameraInfo(s32 id) {
    return gabi::at<dComIfG_camera_info_class>(dComIfGp_ea() + PLAY_OFF(mCameraInfo) +
                                               id * sizeof(dComIfG_camera_info_class));
}

template <class T> static T cameraArgument(T value) { return value; }
template <class T> static u32 cameraArgument(T* value) { return gabi::ea(value); }
s32 cameraGetParam(u32);
void cameraLookAt(u32, u32, u32, u32, u32);
void clipperSetup(f32, f32, f32, f32);
u32 demoGetCamera(u32);
void cameraResetView(u32, u32);
void cameraRun(u32);
void cameraNotRun(u32);
void cameraStore(u32);
void audioCameraInfo(u32, u32, u32);
void groundCheckConstruct(u32);
void angleConstruct(u32);
void globeConstruct(u32);
void cameraConstruct(u32, u32);
void paramDestroy(u32, u32);
void setupDestroy(u32, u32);
void projectionDestroy(u32, u32);
void lookAtDestroy(u32, u32);
void groundCheckDestroy(u32, u32);
void effectLineDestroy(u32, u32);
void cameraDestroy(u32, u32);
void renderManagerDetach(u32, u32);
s32 cameraPhaseHandler(u32, u32, u32);
bool cameraEvent(u32, u32);
bool focusLineOff(u32);
f32 cameraShake(u32);
void cameraValueInitialize(u32, u32);
void cameraConstructArray(u32, u32, u32, u32, u32);
void focusLineConstruct(u32);
void backgroundConstruct(u32);
void splineConstruct(u32);
void projectionConstruct(u32);
void setupConstruct(u32);
void paramConstruct(u32, u32);
void cameraInitialize(u32, u32, u32, u32, u32);
void focusLineInit(u32);
f32 pirateShipOffsetY(u32, u32, f32);
void paramChange(u32, u32);
f32 paramCenterHeight(u32, f32);
u32 angleConstructValue(u32, u32);
void globeConstructAngles(u32, f32, u32, u32);
void globeXyz(u32, u32);
void globeSetAngles(u32, f32, u32, u32);
f32 paramFovy(u32, f32);
void safeStringTerminate(u32);
void angleConstructValueDiscard(u32, u32);
void cameraVectorScale(u32, u32, f32);
void angleAdd(u32, u32, u32);
void windowSetScissor(u32, f32, f32, f32, f32);
void cameraPerspective(u32, f32, f32, f32, f32);
f32 cameraVectorNormalize(u32);
void quaternionAxisAngle(u32, u32, f32);
void matrixFromQuaternion(u32, u32);
void projectionSet(u32, f32, f32, f32, f32);
void projectionSetZoom(u32, u32);
u32 cameraIsZoomRequested();
u32 displayGetSize();
f32 cameraAddCalc(u32, f32, f32, f32, f32);
u32 projectionGetMatrix(u32);
void cameraCopyProjection(u32, u32);
void cameraInverseView(u32, u32);
f32 cameraGroundCross(u32, u32);
u32 cameraMaterialSound(u32, u32);
void audioCameraMapInfo(u32);
u32 cameraGroupSound(u32, u32);
void audioCameraGroupInfo(u32);
void audioCameraPolygonPos(u32, u32);
void cameraConcatProjView(u32, u32, u32);
s32 cameraDrawPriority(u32);
bool cameraOverlapRequest();
void cameraMapDraw(u32, f32, f32, f32);

// HD: image-backed camera and render state.
static u32 sActorStopStatus;
static u32 sRenderManager;
static s32 sDisplayMode;
static u32 sDemo;
static u8 sAutoFocus;
static s16 sZeroAngle;
static cXyz sZeroVector;
static cXyz sInitialAudioPosition;
static WordMatrix sIdentity;
static Zoom sInitialZoom;
static Zoom sInitialProjectionOffset;
static WordMatrix sJ3DView;
static WordMatrix sJ3DProjection;
static cXyz sCenterJump;
static cXyz sEyeJump;
static u32 sCenterJumpInitialized;
static u32 sEyeJumpInitialized;
static s32 sDemoFrame;
static s32 sPreviousDemoFrame;
static s16 sBankJump;
static u32 sAudioInterface;
static s8 sStayRoom;

static inline s32 fopCamM_GetParam(camera_class* cam) {
    return cameraGetParam(cameraArgument(cam));
}
static WordVector* vectorWords(cXyz* value) { return reinterpret_cast<WordVector*>(value); }
static inline u32 cam_ea(camera_class* cam) { return gabi::ea(cam); }
/* sead render layers (HD): an array of layer pointers at +0x1024 (count at +0x1020) of the object
 * at *sRenderManager; layer +0x48 is the camera, +0x4C the projection */
static void setRenderLayerCamera(u32 cameraPtr, u32 projectionPtr) {
    u32 mgr = sRenderManager.get();
    u32 arr = gabi::at<RenderManager>(mgr)->mLayers.get();
    if (gabi::at<RenderManager>(mgr)->mLayerCount.get() > 1)
        arr += 4;
    gabi::at<RenderLayer>(gabi::at<be<u32>>(arr)->get())->mCamera = cameraPtr;
    arr = gabi::at<RenderManager>(mgr)->mLayers.get();
    if (gabi::at<RenderManager>(mgr)->mLayerCount.get() > 1)
        arr += 4;
    gabi::at<RenderLayer>(gabi::at<be<u32>>(arr)->get())->mProjection = projectionPtr;
    gabi::at<RenderLayer>(gabi::at<be<u32>>(gabi::at<RenderManager>(mgr)->mLayers)->get())
        ->mCamera = cameraPtr;
    gabi::at<RenderLayer>(gabi::at<be<u32>>(gabi::at<RenderManager>(mgr)->mLayers)->get())
        ->mProjection = projectionPtr;
}

/* 024F8068 */
/* HD: the window/viewport/view are stored in the play object (0x5F9C..0x5FA4)  */
static void view_setup(camera_class* i_this) {
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = fieldRead<s8>(cameraInfo(camId)->mDlstWindowIdx);
    u32 window = dComIfGp_ea() + PLAY_OFF(mDlstWindow) + winId * 0x2C; /* dComIfGp_getWindow */
    cameraLookAt(
        cameraArgument(gabi::ea(&cameraView(i_this)->mViewMtx)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mEye.x)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mCenter.x)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mUp.x)),
        cameraArgument((s16)fieldRead<s16>(cameraView(i_this)->mBank))); /* mDoMtx_lookAt */
    PSMTXCopy(gabi::at<Mtx34>(gabi::ea(&cameraView(i_this)->mViewMtx)),
              gabi::at<Mtx34>(gabi::ea(&cameraView(i_this)->mViewMtxNoTrans)));
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[0][3], 0.0f);
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[1][3], 0.0f);
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[2][3], 0.0f);
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mWindow = window;       /* dComIfGd_setWindow */
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mViewport = window;     /* dComIfGd_setViewport */
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mView = cam_ea(i_this); /* dComIfGd_setView */
    if (playScopeMessageStatus()->get() != 0) { /* dComIfGp_getScopeMesgStatus() */
        clipperSetup(
            fieldRead<f32>(cameraView(i_this)->mFovy), fieldRead<f32>(cameraView(i_this)->mAspect),
            fieldRead<f32>(cameraView(i_this)->mNear),
            fieldRead<f32>(
                cameraView(i_this)->mFar)); /* mDoLib_clipper::setup(fovy, aspect, near, far) */
    } else {
        u32 stage = dComIfGp_ea() + 0x5150;
        u32 info = gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(),
            stage); /* getStagInfo() */
        f32 far = (f32)fieldRead<u16>(
            gabi::at<StageInfo>(info)->mCullPoint); /* dStage_stagInfo_GetCullPoint */
        clipperSetup(fieldRead<f32>(cameraView(i_this)->mFovy),
                     fieldRead<f32>(cameraView(i_this)->mAspect),
                     fieldRead<f32>(cameraView(i_this)->mNear), far);
    }
}

/* 024FF8A0 */
/* HD: when the trim is suppressed (play byte 0x5292, or for the cinema-scope size the
 * global 101D6010 == 1) the bars shrink away instead  */
void dCamera_c::CalcTrimSize() {
    u32 size = mTrimSize;
    f32 h;
    switch (size) {
    case 0:
        break;
    case 1:
        if (playEventRunning()->get() != 0)
            break;
        h = mTrimHeight;
        mTrimHeight = gabi::fmadds(fieldRead<f32>(cameraSetup(this)->mVistaTrimHeight) - h, 0.25f,
                                   h); /* VistaTrimHeight */
        return;
    case 2:
        if (sDisplayMode.get() == 1)
            break;
        if (playEventRunning()->get() != 0)
            break;
        h = mTrimHeight;
        mTrimHeight = gabi::fmadds(fieldRead<f32>(cameraSetup(this)->mCinemaScopeTrimHeight) - h,
                                   0.25f, h); /* CinemaScopeTrimHeight */
        return;
    case 3:
        if (playEventRunning()->get() != 0)
            break;
        fieldWrite<u32>(this->mTrimHeight,
                        fieldRead<u32>(cameraSetup(this)->mCinemaScopeTrimHeight));
        return;
    case 4:
        mTrimHeight = 0.0f;
        return;
    default:
        return;
    }
    h = mTrimHeight;
    mTrimHeight = gabi::fnmsubs(h, 0.25f, h); /* h += (0 - h) * 0.25 */
}

/* 024FFA3C */
/* HD: fixed 16:9 aspect; auto focus is switched on every frame  */
static BOOL camera_execute(camera_class* i_this) {
    dCamera_c* body = gabi::at<dCamera_c>(gabi::ea(&reinterpret_cast<Process*>(i_this)->mCamera));
    /* preparation() */
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = fieldRead<s8>(cameraInfo(camId)->mDlstWindowIdx);
    auto* vp = gabi::at<DisplaySize>(dComIfGp_ea() + PLAY_OFF(mDlstWindow) + 8 + winId * 0x2C);
    u32 wBits = fieldRead<u32>(vp->mWidth);
    u32 hBits = fieldRead<u32>(vp->mHeight);
    fieldWrite<u32>(body->mWindowWidth, wBits);               /* SetWindow */
    fieldWrite<f32>(cameraView(i_this)->mAspect, 1.7777778f); /* fopCamM_SetAspect */
    fieldWrite<u32>(body->mWindowHeight, hBits);
    body->mWindowAspectRatio = fieldRead<f32>(vp->mWidth) / fieldRead<f32>(vp->mHeight);
    auto* stat = &cameraInfo(camId)->mCameraAttentionStatus;
    *stat = stat->get() & ~0x23u; /* dComIfGp_offCameraAttentionStatus(0x23) */

    u32 demo = sDemo.get();
    if (demo == 0) {
        JUT_ASSERT_fail(STR(0x1004AB04), 0x23E, STR(0x1004AAF4));
        demo = sDemo.get();
    }
    if (demoGetCamera(cameraArgument(demo)) != 0) { /* dComIfGp_demo_getCamera() */
        cameraResetView(cameraArgument(body), cameraArgument(i_this)); /* ResetView */
    }
    sAutoFocus = 1; /* mDoGph_gInf_c::onAutoForcus() */
    if (body->Active() && !body->Pause()) {
        cameraRun(cameraArgument(body)); /* Run */
    } else {
        cameraNotRun(cameraArgument(body)); /* NotRun */
    }
    body->CalcTrimSize();
    cameraStore(cameraArgument(i_this)); /* store */
    view_setup(i_this);
    return TRUE;
}

/* 0250043C */
/* HD: no player-2 id  */
static s32 init_phase1(camera_class* i_this) {
    s32 camId = fopCamM_GetParam(i_this);
    cameraInfo(camId)->mpCamera = cam_ea(i_this); /* dComIfGp_setCamera */
    fieldWrite<u8>(reinterpret_cast<Process*>(i_this)->mPrm1,
                   fieldRead<u8>(cameraInfo(camId)->mDlstWindowIdx)); /* prm1: window id */
    fieldWrite<u8>(reinterpret_cast<Process*>(i_this)->mPrm2,
                   fieldRead<u8>(cameraInfo(camId)->mCamP1Id)); /* prm2: player 1 id */
    fieldWrite<u8>(reinterpret_cast<Process*>(i_this)->mPrm3,
                   fieldRead<u8>(cameraInfo(camId)->mCamP2Id)); /* prm3: player 2 id */
    gabi::Local<cXyz> pos;
    pos->copy(*&sInitialAudioPosition); /* {1e7, 1e7, 1e7} */
    audioCameraInfo(
        cameraArgument(pos.get()), cameraArgument(gabi::ea(&sJ3DView)),
        cameraArgument(camId));      /* mDoAud_getCameraInfo(&pos, j3dSys.getViewMtx(), camId) */
    playWindowCount()->operator=(0); /* dComIfGp_setWindowNum(0) */
    sAutoFocus = 1;                  /* mDoGph_gInf_c::onAutoForcus() */
    return cPhs_NEXT_e;
}

/* 02500508 */
/* compiler-generated constructor of the BG block (HD 0x11C: two dBgS_CamGndChk and the
 * angles/globes that follow them)  */
static u8* dCamera_BG_ct(u8* p) {
    if (p == nullptr) {
        p = (u8*)operator_new(0x11C);
        if (p == nullptr)
            return p;
    }
    u32 o = gabi::ea(p);
    groundCheckConstruct(cameraArgument(gabi::ea(
        &gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mPolyPassChk))); /* cBgS_GndChk() */
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbRopeThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbCamThrough, 1);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbObjThrough, 0);
    fieldWrite<u32>(
        gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mGrpPassChk,
        gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.dBgS_GrpPassChk::__vtbl));
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbBombThrough, 0);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mGrp, 1);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.cBgS_Chk::__vtbl,
                    kCameraGroundVtable);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.dBgS_GrpPassChk::__vtbl,
                    kCameraGroupVtable);
    fieldWrite<u32>(
        gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mPolyPassChk,
        gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.dBgS_PolyPassChk::__vtbl));
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.cBgS_PolyInfo::__vtbl,
                    kCameraPolyInfoVtable);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbBoomerangThrough, 0);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.dBgS_PolyPassChk::__vtbl,
                    kCameraPassVtable);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m00.m04.mbLinkThrough, 0);
    groundCheckConstruct(
        cameraArgument(gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mPolyPassChk)));
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mGrp, 1);
    fieldWrite<u32>(
        gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mGrpPassChk,
        gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.dBgS_GrpPassChk::__vtbl));
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.dBgS_GrpPassChk::__vtbl,
                    kCameraGroupVtable);
    fieldWrite<u32>(
        gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mPolyPassChk,
        gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.dBgS_PolyPassChk::__vtbl));
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.cBgS_PolyInfo::__vtbl,
                    kCameraPolyInfoVtable);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.dBgS_PolyPassChk::__vtbl,
                    kCameraPassVtable);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbRopeThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbBoomerangThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbObjThrough, 0);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbLinkThrough, 0);
    fieldWrite<u32>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.cBgS_Chk::__vtbl,
                    kCameraGroundVtable);
    fieldWrite<u8>(gabi::at<CameraProcessHD::BG>(o)->mChecks.m5C.m04.mbCamThrough, 1);
    angleConstruct(
        cameraArgument(gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mFirstAngle))); /* cSAngle() */
    angleConstruct(cameraArgument(gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mSecondAngle)));
    globeConstruct(
        cameraArgument(gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mFirstGlobe))); /* cSGlobe() */
    globeConstruct(cameraArgument(gabi::ea(&gabi::at<CameraProcessHD::BG>(o)->mSecondGlobe)));
    return p;
}

/* 02501F98 */
/* HD: fixed 16:9 aspect; the body's sead camera and projection are attached to the
 * render layers  */
static s32 init_phase2(camera_class* i_this) {
    u32 body = gabi::ea(&reinterpret_cast<Process*>(i_this)->mCamera);
    fopCamM_GetParam(i_this);
    s32 camId = fopCamM_GetParam(i_this);
    s32 playerId = fieldRead<s8>(cameraInfo(camId)->mCamP1Id); /* dComIfGp_getCameraPlayer1ID */
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(fieldRead<u32>(
        *gabi::at<gptr<fopAc_ac_c>>(dComIfGp_ea() + PLAY_OFF(mPlayerInfo) + playerId * 8)));
    if (player == nullptr)
        return cPhs_INIT_e;
    fopAcM_setStageLayer(player);
    playWindowCount()->operator=(1); /* dComIfGp_setWindowNum(1) */
    if (body != 0)
        cameraConstruct(cameraArgument(body),
                        cameraArgument(i_this)); /* new (body) dCamera_c(i_this) */
    f32 farPlane = 160000.0f;
    u32 stage = dComIfGp_ea() + 0x5150;
    if (gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage) !=
        0) {
        stage = dComIfGp_ea() + 0x5150;
        gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage);
        u32 info = gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage);
        farPlane = fieldRead<f32>(gabi::at<StageInfo>(info)->mFarPlane); /* mFarPlane */
    }
    dComIfGp_ea(); /* get_window(camera_id): unused in HD */
    dComIfGp_ea();
    fieldWrite<f32>(cameraView(i_this)->mFar, farPlane);      /* fopCamM_SetFar */
    fieldWrite<f32>(cameraView(i_this)->mFovy, 30.0f);        /* fopCamM_SetFovy */
    fieldWrite<f32>(cameraView(i_this)->mNear, 1.0f);         /* fopCamM_SetNear */
    fieldWrite<f32>(cameraView(i_this)->mAspect, 1.7777778f); /* fopCamM_SetAspect */
    u32 pos = gabi::ea(&player->current.pos);
    u32 py = gabi::at<WordVector>(pos)->mWord[1].get();
    u32 px = gabi::at<WordVector>(pos)->mWord[0].get();
    u32 pz = gabi::at<WordVector>(pos)->mWord[2].get();
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.x, px); /* fopCamM_SetCenter */
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.y, py);
    fieldWrite<s16>(cameraView(i_this)->mBank, 0); /* fopCamM_SetBank */
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.z, pz);
    cameraStore(cameraArgument(i_this)); /* store */
    view_setup(i_this);
    setRenderLayerCamera(body + 0x614, body + 0x66C);
    return cPhs_NEXT_e;
}

/* 025021B8 */
/* dCamera_c::~dCamera_c (HD: destroys the sead camera/projection, setup and param)  */
static void dCamera_dt(dCamera_c* i_this, s32 flags) {
    if (i_this == nullptr)
        return;
    sActorStopStatus = 0; /* fopAc_ac_c::setStopStatus(0) */
    paramDestroy(cameraArgument(gabi::ea(&i_this->mCamParam)),
                 cameraArgument(2)); /* ~dCamParam_c */
    setupDestroy(cameraArgument(gabi::ea(&i_this->mCamSetup)),
                 cameraArgument(2)); /* ~dCamSetup_c */
    projectionDestroy(cameraArgument(gabi::ea(&i_this->mHDProjection)),
                      cameraArgument(2)); /* ~projection */
    lookAtDestroy(cameraArgument(gabi::ea(&i_this->mHDLookAtCamera)),
                  cameraArgument(2)); /* ~LookAtCamera */
    fieldWrite<u32>(i_this->mBG.m5C.m04.dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(i_this->mBG.m5C.m04.cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(i_this->mBG.m5C.m04.dBgS_GrpPassChk::__vtbl, kGroupBaseVtable);
    groundCheckDestroy(cameraArgument(gabi::ea(&i_this->mBG.m5C.m04.mPolyPassChk)),
                       cameraArgument(0)); /* ~dBgS_CamGndChk (m5C) */
    fieldWrite<u32>(i_this->mBG.m00.m04.cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(i_this->mBG.m00.m04.dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(i_this->mBG.m00.m04.dBgS_GrpPassChk::__vtbl, kGroupBaseVtable);
    groundCheckDestroy(cameraArgument(gabi::ea(&i_this->mBG.m00.m04.mPolyPassChk)),
                       cameraArgument(0)); /* ~dBgS_CamGndChk (m00) */
    effectLineDestroy(cameraArgument(gabi::ea(&i_this->mForcusLine)),
                      cameraArgument(0)); /* ~dDlst_effectLine_c */
    if (flags & 1)
        operator_delete(i_this);
}

/* 02502288 */
/* HD: clears the play object's draw view and detaches the render layers  */
static BOOL camera_delete(camera_class* i_this) {
    s32 camId = fopCamM_GetParam(i_this);
    cameraDestroy(cameraArgument(gabi::ea(&reinterpret_cast<Process*>(i_this)->mCamera)),
                  cameraArgument(2)); /* body->~dCamera_c() */
    cameraInfo(camId)->mpCamera = 0;  /* dComIfGp_setCamera(camId, NULL) */
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mWindow = 0;
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mViewport = 0;
    gabi::at<DrawView>(dComIfGp_ea() + 0x5F9C)->mView = 0;
    setRenderLayerCamera(0, 0);
    renderManagerDetach(cameraArgument(sRenderManager.get()), cameraArgument(0));
    return TRUE;
}

/* 025021A4 */
/* camera_create (matcher: unnamed). The phase handler (02525FE4, dComLbG_PhaseHandler)
 * runs l_method (101D55C0: init_phase1, init_phase2) on the process's phase request at +0x240  */
static s32 camera_create(u32 i_this) {
    return cameraPhaseHandler(cameraArgument(i_this + 0x240), cameraArgument(kCameraPhaseMethods),
                              cameraArgument(i_this));
}

/* 02502364 */
/* HD-only: an entry of the play object's table at 0x5C38 selected by the indices at
 * 0x5C34 then 0x5C30 (1..10; the later one wins)  */
static u32 dCamera_hdTableEntry() {
    auto* table = gabi::at<TalkTable>(dComIfGp_ea() + 0x5C30);
    u32 b = fieldRead<u32>(table->mSecondary);
    u32 a = fieldRead<u32>(table->mPrimary);
    u32 r = 0;
    if (b - 1 < 10)
        r = table->mEntries[b - 1].get();
    if (a - 1 < 10)
        r = table->mEntries[a - 1].get();
    return r;
}

/* 025023C4 */
/* HD-only: the index at 0x5C34 or 0x5C30 when it is beyond the table (> 10)  */
static s32 dCamera_hdTableIndex() {
    auto* table = gabi::at<TalkTable>(dComIfGp_ea() + 0x5C30);
    s32 b = fieldRead<s32>(table->mSecondary);
    s32 a = fieldRead<s32>(table->mPrimary);
    s32 r = 0;
    if (!((u32)(b - 1) < 10) && b > 0)
        r = b;
    if (!((u32)(a - 1) < 10) && a > 0)
        r = a;
    return r;
}

/* 024FF6C0 */
/* HD: the telescope view is 1280x720  */
bool dCamera_c::NotRun() {
    mEventFlags = mEventFlags & ~0x90149C21u;
    checkGroundInfo();
    clrComStat(0x80);
    if (playEventCameraPlaying()->get() != 0 /* dComIfGp_evmng_cameraPlay() */ ||
        chkFlag(0x20000000)) {
        s32 ev = mCamTypeEvent;
        if (mCurType != ev) {
            pushPos();
            mEventData.field_0x0c = (s32)mCurType;
            ev = mCamTypeEvent;
        }
        mCurType = ev;
        cameraEvent(cameraArgument(this),
                    cameraArgument((s32)camTypeStyle(ev, 3))); /* eventCamera */
        m07C = m07C + 1;
        m118 = m118 + 1;
        m11C = m11C + 1;
        m108 = m108 + 1;
    }
    if (playEventRunning()->get() != 0 /* dComIfGp_event_runCheck() */)
        clrComStat(0x48);
    setComStat(0x14);
    mEventFlags = mEventFlags & ~0x90080u;
    focusLineOff(cameraArgument(&mForcusLine)); /* mForcusLine.Off() */
    cameraShake(cameraArgument(this));          /* shakeCamera() */
    mPause = 0;
    if (getComStat(8)) {
        if (chkFlag(0x400000)) {
            setView(160.0f, 35.0f, 320.0f, 320.0f);
        } else {
            setView(0.0f, 0.0f, 1280.0f, 720.0f);
        }
    }
    return true;
}

static inline void memzero(u32 p, u32 n) {
    cameraValueInitialize(cameraArgument(p), cameraArgument(n));
} /* value-initialisation of PODs */
static inline void cSAngle_ct_at(u32 p) { angleConstruct(cameraArgument(p)); }
static inline void cSGlobe_ct_at(u32 p) { globeConstruct(cameraArgument(p)); }

/* 02501B8C */
/* dCamera_c::dCamera_c(camera_class*). HD: value-initialises its members and builds
 * the HD-only sead camera (identity matrix, eye (0,0,10), at 0, up +Y) and projection  */
static dCamera_c* dCamera_ct(dCamera_c* i_this, camera_class* cam) {
    if (i_this == nullptr) {
        i_this = (dCamera_c*)operator_new(0x8E0);
        if (i_this == nullptr)
            return i_this;
    }
    fieldWrite<u32>(i_this->mpCamera, 0);
    fieldWrite<u8>(i_this->mActive, 0);
    fieldWrite<u8>(i_this->mPause, 0);
    cSGlobe_ct_at(gabi::ea(&i_this->mDirection));
    cSAngle_ct_at(gabi::ea(&i_this->mBank));
    fieldWrite<f32>(i_this->mFovy, 0.0f);
    memzero(gabi::ea(&i_this->mViewCache), 0x28);
    cSGlobe_ct_at(gabi::ea(&i_this->mViewCache));
    cSAngle_ct_at(gabi::ea(&i_this->mViewCache.mBank));
    fieldWrite<f32>(i_this->m064, 0.0f);
    fieldWrite<u32>(i_this->m068, 0);
    cSAngle_ct_at(gabi::ea(&i_this->mAngleY));
    fieldWrite<u32>(i_this->m080, 0);
    fieldWrite<u32>(i_this->m07C, 0);
    memzero(gabi::ea(&i_this->m084), 0x20);
    cSAngle_ct_at(gabi::ea(&i_this->m0A0));
    cameraConstructArray(
        cameraArgument(gabi::ea(&i_this->m0A4)), cameraArgument(2), cameraArgument(0x20),
        cameraArgument(kPosSetConstructor),
        cameraArgument(0)); /* __construct_array(m0A4, 2, 0x20, PosSet ctor, NULL) */
    memzero(gabi::ea(&i_this->mStageMapToolCameraIdx), 8);
    memzero(gabi::ea(&i_this->mExtendedPos), 0x14);
    memzero(gabi::ea(&i_this->m100), 0x14);
    cSAngle_ct_at(gabi::ea(&i_this->m104));
    cSAngle_ct_at(gabi::ea(&i_this->m106));
    memzero(gabi::ea(&i_this->m114), 8);
    memzero(gabi::ea(&i_this->m11C), 4);
    fieldWrite<u32>(i_this->mpPlayerActor, 0);
    fieldWrite<u32>(i_this->mPadId, 0);
    fieldWrite<u32>(i_this->mpLockonTarget, 0);
    fieldWrite<u32>(i_this->mCameraID, 0);
    memzero(gabi::ea(&i_this->mLockOnActorId), 0xC);
    fieldWrite<u32>(i_this->m144, 0);
    fieldWrite<u32>(i_this->mCurMode, 0);
    fieldWrite<u32>(i_this->mNextMode, 0);
    cSAngle_ct_at(gabi::ea(&i_this->m148));
    fieldWrite<f32>(i_this->m150, 0.0f);
    fieldWrite<f32>(i_this->m154, 0.0f);
    fieldWrite<f32>(i_this->m14C, 0.0f);
    memzero(gabi::ea(&i_this->mStickMainPosXLast), 0x5C);
    memzero(gabi::ea(&i_this->mForcusLine), 0x70);
    focusLineConstruct(cameraArgument(gabi::ea(&i_this->mForcusLine))); /* dCamForcusLine() */
    memzero(gabi::ea(&i_this->mDMCSystem), 6);
    cSAngle_ct_at(gabi::ea(&i_this->mDMCSystem.field_0x2));
    cSAngle_ct_at(gabi::ea(&i_this->mDMCSystem.field_0x4));
    memzero(gabi::ea(&i_this->mMonitor), 0x20);
    memzero(gabi::ea(&i_this->m248), 0x14);
    memzero(gabi::ea(&i_this->mBG), 0x11C);
    backgroundConstruct(cameraArgument(gabi::ea(&i_this->mBG))); /* BG block */
    memzero(gabi::ea(&i_this->mWork), 0x80);
    memzero(gabi::ea(&i_this->mEventData), 0x114);
    splineConstruct(
        cameraArgument(gabi::ea(&i_this->mEventData.mSpline2DPath))); /* d2DBSplinePath() */
    fieldWrite<u32>(i_this->m528, 0);
    fieldWrite<u8>(i_this->m52C, 0);
    fieldWrite<u32>(i_this->mEventFlags, 0);
    fieldWrite<u32>(i_this->mMapToolType, 0);
    fieldWrite<u32>(i_this->mCurStyle, 0);
    fieldWrite<u8>(i_this->m514, 0);
    fieldWrite<u32>(i_this->mCurType, 0);
    fieldWrite<u32>(i_this->mNextType, 0);
    fieldWrite<u32>(i_this->m524, 0);
    memzero(gabi::ea(&i_this->m530), 0x14);
    memzero(gabi::ea(&i_this->m544), 0x7C);
    cSAngle_ct_at(gabi::ea(&i_this->mBankShake));
    memzero(gabi::ea(&i_this->mCurRoomCamEntry), 0x2C);
    memzero(gabi::ea(&i_this->mWindowWidth), 8);
    memzero(gabi::ea(&i_this->m5F4), 8);
    fieldWrite<u32>(i_this->mTrimTypeForce, 0);
    fieldWrite<f32>(i_this->m608, 0.0f);
    fieldWrite<f32>(i_this->mWindowAspectRatio, 0.0f);
    fieldWrite<u32>(hdCamera(i_this)->mVtable, kSeadCameraVtable); /* sead::Camera vtable */
    fieldWrite<u8>(i_this->mHD610, 0);
    fieldWrite<u32>(i_this->mTrimSize, 0);
    for (u32 k = 0; k < 0x30; k += 4)
        reinterpret_cast<WordMatrix*>(&hdCamera(i_this)->mMatrix)->mWord[k / 4] =
            sIdentity.mWord[k / 4].get();                          /* Matrix34f::ident */
    fieldWrite<u32>(hdCamera(i_this)->mVtable, kSeadLookAtVtable); /* sead::LookAtCamera vtable */
    u32 pos = gabi::ea(&hdCamera(i_this)->mPos);
    if (pos == 0)
        pos = gabi::ea(operator_new(0xC));
    if (pos != 0) {
        gabi::at<cXyz>(pos)->y = 0.0f;
        gabi::at<cXyz>(pos)->x = 0.0f;
        gabi::at<cXyz>(pos)->z = 10.0f;
    }
    u32 at = gabi::ea(&hdCamera(i_this)->mAt);
    if (at == 0)
        at = gabi::ea(operator_new(0xC));
    if (at != 0) {
        gabi::at<cXyz>(at)->y = 0.0f;
        gabi::at<cXyz>(at)->x = 0.0f;
        gabi::at<cXyz>(at)->z = 0.0f;
    }
    u32 up = gabi::ea(&hdCamera(i_this)->mUp);
    if (up == 0)
        up = gabi::ea(operator_new(0xC));
    if (up != 0) {
        gabi::at<cXyz>(up)->z = 0.0f;
        gabi::at<cXyz>(up)->y = 1.0f;
        gabi::at<cXyz>(up)->x = 0.0f;
    }
    projectionConstruct(cameraArgument(gabi::ea(&i_this->mHDProjection))); /* sead projection */
    if (gabi::ea(&i_this->mHD730) == 0)
        operator_new(8);
    fieldWrite<u8>(i_this->mHD738, 0);
    setupConstruct(cameraArgument(gabi::ea(&i_this->mCamSetup))); /* dCamSetup_c() */
    paramConstruct(cameraArgument(gabi::ea(&i_this->mCamParam)),
                   cameraArgument(0)); /* dCamParam_c(0) */
    memzero(gabi::ea(&i_this->mCamTypeField), 0x24);
    memzero(gabi::ea(&i_this->m780), 0xC);
    /* initialize(cam, get_player_actor(cam), get_camera_id(cam), get_controller_id(cam)) */
    s32 camId = fopCamM_GetParam(cam);
    s32 playerId = fieldRead<s8>(cameraInfo(camId)->mCamP1Id);
    u32 player = fieldRead<u32>(
        *gabi::at<gptr<fopAc_ac_c>>(dComIfGp_ea() + PLAY_OFF(mPlayerInfo) + playerId * 8));
    s32 infoIdx = fopCamM_GetParam(cam);
    s32 camId2 = fopCamM_GetParam(cam);
    s32 padId = cameraInfo(camId2)->mCamP1Id.get();
    cameraInitialize(cameraArgument(i_this), cameraArgument(cam), cameraArgument(player),
                     cameraArgument(infoIdx), cameraArgument(padId));
    return i_this;
}

/* is_current_map(name): HD compares sead::SafeString temporaries (literal vs the play object's
 * start stage name at 0x5134), calling each one's virtual 0x14 (terminate) first */
static bool is_current_map(u32 literal) {

    gabi::Local<CameraProcessHD::SafeString> a, b;
    a->top = literal;
    a->vtable = kSafeStringVtable;
    u32 stage = dComIfGp_ea() + 0x5134;
    b->vtable = kSafeStringVtable;
    b->top = stage;
    gabi::call_ptr(gabi::at<SafeStringVtable>(a->vtable)->terminate.get(), a.get());
    gabi::call_ptr(gabi::at<SafeStringVtable>(a->vtable)->terminate.get(), a.get());
    u32 vt = b->vtable;
    u32 aTop = a->top;
    gabi::call_ptr(gabi::at<SafeStringVtable>(vt)->terminate.get(), b.get());
    if (aTop == (u32)b->top)
        return true;
    u32 x = a->top, y = b->top;
    for (u32 n = 0x40001; n != 0; n--) {
        u32 cx = gabi::at<be<u8>>(x)->get(), cy = gabi::at<be<u8>>(y)->get();
        if (cx != cy)
            return false;
        if (cx == 0)
            return true;
        x++;
        y++;
    }
    return false;
}

/* float -> u32 (GHS: values >= 2^31 are converted after subtracting 2^31) */
static u32 ftou(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 02500984 */
/* HD: the 0x730/0x734 floats (HD-only) are initialised from 104A0920; the ship offset
 * is computed into m538 as on GameCube  */
void dCamera_c::initialize(camera_class* camera, fopAc_ac_c* player, u32 infoIdx, u32 padId) {
    mpPlayerActor = player;
    mCameraID = (s32)infoIdx;
    mPadId = (s32)padId;
    mActive = 1;
    mpCamera = camera;
    mPause = 0;
    initMonitor();
    initPad();
    focusLineInit(cameraArgument(&mForcusLine)); /* mForcusLine.Init() */
    mCamTypeField = GetCameraTypeFromCameraName(STR(0x1004AB58) /* "Field" */);
    mCamTypeEvent = GetCameraTypeFromCameraName(STR(0x1004AB60) /* "Event" */);
    mCamTypeWater = GetCameraTypeFromCameraName(STR(0x1004AB68) /* "Water" */);
    mCamTypeSubject = GetCameraTypeFromCameraName(STR(0x1004AB44) /* "Subject" */);
    mCamTypeBoat = GetCameraTypeFromCameraName(STR(0x1004AB90) /* "Boat" */);
    mCamTypeBoatBattle = GetCameraTypeFromCameraName(STR(0x1004ABC8) /* "BoatBattle" */);
    mCamTypeRestrict = GetCameraTypeFromCameraName(STR(0x1004ABD4) /* "Restrict" */);
    s32 keep = GetCameraTypeFromCameraName(STR(0x1004AB98) /* "Keep" */);
    mCurMode = 0;
    s32 field = mCamTypeField;
    mMapToolType = field;
    m258 = 0;
    m254 = 0;
    mCurType = field;
    m248[0] = 0x85E; /* JA_SE_MAN_CAMERA_NG */
    m14C = 0.0f;
    m144 = 1;
    m524 = 0xFF;
    m528 = nullptr;
    mEventFlags = 0;
    fieldWrite<u8>(this->m514, 0); /* m514 */
    m248[1] = 0x828;               /* JA_SE_CAMERA_TO_MANUAL */
    m248[2] = 0x381B;              /* JA_SE_ATM_PRT_SHIP_CREAK */
    mCamTypeKeep = keep;
    m148 = sZeroAngle.get(); /* cSAngle::_0 */
    m07C = 0;
    f32 rnd = cM_rndFX(32767.0f);
    m080 = ftou(rnd);
    m064 = 1.0f;
    m5F4 = 0.0f;
    mTrimSize = 0;
    mTrimHeight = 0.0f;
    mTrimTypeForce = -1;
    u32 stage = dComIfGp_ea() + 0x12A0 + 0x3EB0; /* &dComIfGp_getStage() */
    if (stage != 0) {
        u32 info = gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(),
            stage); /* getStagInfo() */
        if (info != 0) {
            u32 toolId = fieldRead<u8>(
                gabi::at<StageInfo>(info)->mCameraMapToolID); /* mCameraMapToolID (u8: never -1) */
            s32 type = GetCameraTypeFromMapToolID((s32)toolId, -1);
            if (type != 0xFF && Chtyp(type))
                mMapToolType = type;
        }
    }
    s16 style = camTypeStyle(mCurType, mCurMode);
    u32 grp = fieldRead<u32>(this->mBG.m00.m04.mGrp);
    m318 = -1000000000.0f;
    mBG.m5C.m58 = -1000000000.0f;
    m31D = 0;
    m31C = 0;
    mRoomNo = -1;
    mStageMapToolCameraIdx = 0xFF;
    mEventData.field_0x14 = -1;
    mBG.m00.m58 = -1000000000.0f;
    fieldWrite<u32>(this->mBG.m00.m04.mGrp,
                    (grp & ~1u) | 2); /* mBG.m00.m04.OffNormalGrp(); OnWaterGrp() */
    mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
    mEventData.field_0x18 = -1;
    mEventData.mStaffIdx = -1;
    mEventData.field_0x0c = -1;
    m0E8 = -1;
    mCurStyle = style;
    m32C.copy(*&sZeroVector); /* cXyz::Zero */
    m33C = nullptr;
    m320.copy(m32C);
    s16 zero = sZeroAngle.get();
    m350 = 0;
    m364 = 0;
    mRoomMapToolCameraIdx = 0xFF;
    m33A = zero;
    m338 = zero;
    m368 = 0.0f;
    m354 = -1000000000.0f;
    fieldWrite<u32>(
        this->m608,
        fieldRead<u32>(
            cameraSetup(this)->mWallUpDistance)); /* m608 = mCamSetup.mBGChk.WallUpDistance() */

    m780 = is_current_map(0x1004AB54) /* "sea" */;
    m788 = is_current_map(0x1004ABA0) /* "kaze" */;
    m789 = is_current_map(0x1004AB70) /* "M_Dai" */;
    m78B = is_current_map(0x1004AB78) /* "kazeB" */;
    m784 = is_current_map(0x1004ABA8) /* "GanonK" */;
    m785 = is_current_map(0x1004ABB0) /* "GTower" */;
    m781 = is_current_map(0x1004AB80) /* "Asoko" */ || is_current_map(0x1004ABB8) /* "Abship" */ ||
           is_current_map(0x1004AB88) /* "PShip" */;
    m782 = is_current_map(0x1004ABC0) /* "Obshop" */;
    bool umikz = is_current_map(0x1004AB4C) /* "A_umikz" */;
    m534 = 0;
    m536 = 0x180;
    m783 = umikz;
    f32 ofs;
    if (m781 != 0) {
        m540 = 1.0f;
        m530 = 1;
        ofs = pirateShipOffsetY(cameraArgument(&m534), cameraArgument(&m536), 130.0f) *
              m540; /* daObjPirateship::getShipOffsetY */
    } else if (m782 != 0) {
        m530 = 2;
        m540 = 0.12f;
        ofs = pirateShipOffsetY(cameraArgument(&m534), cameraArgument(&m536), 130.0f) * m540;
    } else if (m783 != 0) {
        m540 = 1.0f;
        m530 = 3;
        ofs = pirateShipOffsetY(cameraArgument(&m534), cameraArgument(&m536), 130.0f) * m540;
    } else {
        m530 = 0;
        ofs = 0.0f;
        m540 = 0.0f;
    }
    s32 curStyle = mCurStyle;
    m538 = ofs;
    paramChange(cameraArgument(gabi::ea(&this->mCamParam)),
                cameraArgument(curStyle)); /* mCamParam.Change(mCurStyle) */

    gabi::Local<cXyz> attnPos, center, xyz, eyeOfs;
    gabi::Local<CameraAngle> dir, zeroAng;
    gabi::Local<CameraGlobe> globe;
    attentionPos(attnPos, mpPlayerActor.get());
    f32 h = paramCenterHeight(cameraArgument(gabi::ea(&this->mCamParam)),
                              0.0f); /* mCamParam.CenterHeight(0.0f) */
    attnPos->y = gabi::fadds_ppc(attnPos->y, h);
    directionOf(dir, mpPlayerActor.get());
    CameraAngle* z = gabi::at<CameraAngle>(
        angleConstructValue(cameraArgument(zeroAng.get()), cameraArgument((s16)0)));
    globeConstructAngles(
        cameraArgument(globe.get()), 0.0f, cameraArgument(z),
        cameraArgument(dir.get())); /* cSGlobe(0.0f, cSAngle(0), directionOf(player)) */
    globeXyz(cameraArgument(globe.get()), cameraArgument(xyz.get())); /* .Xyz() */
    cXyz_pl(attnPos, center, xyz);
    mViewCache.mCenter.copy(*center);
    mCenter.copy(*center);
    directionOf(dir, mpPlayerActor.get());
    s16 inv = cSAngle_Inv(dir);
    globeSetAngles(cameraArgument(&mViewCache.mDirection), 200.0f, cameraArgument((s16)0),
                   cameraArgument(inv)); /* mViewCache.mDirection.Val(200.0f, 0, inv) */
    fieldWrite<u32>(this->mDirection,
                    fieldRead<u32>(this->mViewCache)); /* mDirection = mViewCache.mDirection */
    fieldWrite<u32>(this->mDirection.mV, fieldRead<u32>(this->mViewCache.mDirection.mV));
    globeXyz(cameraArgument(&mViewCache.mDirection), cameraArgument(xyz.get()));
    cXyz_pl(&mViewCache.mCenter, eyeOfs, xyz);
    mViewCache.mEye.copy(*eyeOfs);
    mEye.copy(*eyeOfs);
    s16 u = cSAngle_Inv(&mDirection.mU);
    CameraAngle* ay =
        gabi::at<CameraAngle>(angleConstructValue(cameraArgument(dir.get()), cameraArgument(u)));
    s16 angleY = *ay;
    mUp.x = 0.0f;
    mAngleY = angleY;
    mBank = zero;
    mUp.y = 1.0f;
    mViewCache.mBank = zero;
    mUp.z = 0.0f;
    f32 fovy =
        paramFovy(cameraArgument(gabi::ea(&this->mCamParam)), 0.0f); /* mCamParam.Fovy(0.0f) */
    mFovy = fovy;
    mViewCache.mFovy = fovy;
    mDMCSystem.field_0x0 = 0;
    fieldWrite<u32>(hdZoom(this)->mScale, fieldRead<u32>(sInitialZoom)); /* HD */
    fieldWrite<u32>(hdZoom(this)->mTarget, fieldRead<u32>(sInitialZoom.mTarget));
}

/* SafeString temporaries compared with each other (HD inline sead::SafeString::isEqual) */

static bool safeStringEqual(CameraProcessHD::SafeString* a, CameraProcessHD::SafeString* b,
                            bool directTerminate, bool indirect) {
    if (directTerminate) {
        if (indirect)
            gabi::call_ptr(kSafeStringTerminate,
                           a); /* a.terminate (inline copy, called through a register) */
        else
            safeStringTerminate(cameraArgument(a));
    }
    gabi::call_ptr(gabi::at<SafeStringVtable>(a->vtable)->terminate.get(), a);
    u32 vt = b->vtable;
    u32 aTop = a->top;
    gabi::call_ptr(gabi::at<SafeStringVtable>(vt)->terminate.get(), b);
    if (aTop == (u32)b->top)
        return true;
    u32 x = a->top, y = b->top;
    for (u32 n = 0x40001; n != 0; n--) {
        u32 cx = gabi::at<be<u8>>(x)->get(), cy = gabi::at<be<u8>>(y)->get();
        if (cx != cy)
            return false;
        if (cx == 0)
            return true;
        x++;
        y++;
    }
    return false;
}

/* 024F8184 */
/* HD-only: near plane for the current stage: at least 3; on the "Hyrule" stage (or stage
 * types 5/7) it grows with the camera height (3 at y <= 1000 .. 10 at y >= 8000); 10 during "Demo07"  */
static void dCamera_hdAdjustNear(dCamera_c* body, be<f32>* nearP, be<f32>* farP) {
    u32 stage = dComIfGp_ea() + 0x5150;
    u32 info = gabi::call_ptr<u32>(
        gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(),
        stage); /* getStagInfo() */
    u32 type = (fieldRead<u32>(gabi::at<StageInfo>(info)->mFlags) >> 16) & 7;
    gabi::Local<CameraProcessHD::SafeString> a, b, c, d;
    u32 name = dComIfGp_ea() + 0x5134;
    a->vtable = kSafeStringVtable;
    a->top = name;
    bool big = true;
    if (type != 7 && type != 5) {
        b->vtable = kSafeStringVtable;
        b->top = kHyruleStage; /* "Hyrule" */
        big = safeStringEqual(a, b, true, true);
    }
    f32 n = *nearP;
    if (n < 3.0f) {
        n = 3.0f;
        *nearP = n;
    }
    if (big && n < 10.0f) {
        f32 y = body->mEye.y;
        if (y > 1000.0f) {
            f32 t = (y - 1000.0f) / 7000.0f;
            t = (t - 1.0f >= 0.0f) ? 1.0f : t; /* fsel */
            f32 v = gabi::fmadds(7.0f, t, 3.0f);
            *nearP = (v - n >= 0.0f) ? v : n; /* fsel */
        }
    }
    c->vtable = kSafeStringVtable;
    c->top = kDemo07Name; /* "Demo07" */
    d->vtable = kSafeStringVtable;
    d->top = kDemoName; /* current demo name */
    if (safeStringEqual(c, d, true, true))
        *nearP = 10.0f;
}

/* 024F8430 */
/* HD: during demo "Demo46" frame 878 the cut is smoothed (the jump is remembered in
 * function-local statics and decays by 0.96 per frame); the near/far planes come from
 * dCamera_hdAdjustNear unless the telescope/picto box view is active  */
static void store(camera_class* i_this) {
    dCamera_c* body = gabi::at<dCamera_c>(gabi::ea(&reinterpret_cast<Process*>(i_this)->mCamera));
    s32 camId = fopCamM_GetParam(i_this);
    dComIfGp_ea();
    dComIfGp_ea();
    gabi::Local<cXyz> C, E, saveC, saveE, t;
    gabi::Local<CameraAngle> bank, tmp;
    u32 upY = fieldRead<u32>(cameraView(i_this)->mLookat.mUp.y);
    vectorWords(C.get())->mWord[1] = fieldRead<u32>(cameraView(i_this)->mLookat.mCenter.y);
    vectorWords(E.get())->mWord[0] = fieldRead<u32>(cameraView(i_this)->mLookat.mEye.x);
    u32 upX = fieldRead<u32>(cameraView(i_this)->mLookat.mUp.x);
    vectorWords(E.get())->mWord[1] = fieldRead<u32>(cameraView(i_this)->mLookat.mEye.y);
    vectorWords(C.get())->mWord[0] = fieldRead<u32>(cameraView(i_this)->mLookat.mCenter.x);
    vectorWords(E.get())->mWord[2] = fieldRead<u32>(cameraView(i_this)->mLookat.mEye.z);
    u32 upZ = fieldRead<u32>(cameraView(i_this)->mLookat.mUp.z);
    vectorWords(C.get())->mWord[2] = fieldRead<u32>(cameraView(i_this)->mLookat.mCenter.z);
    angleConstructValueDiscard(cameraArgument(bank.get()),
                               cameraArgument((s16)fieldRead<s16>(
                                   cameraView(i_this)->mBank))); /* cSAngle bank(fopCamM_GetBank) */
    u32 demo = sDemo.get();
    u32 fovy = fieldRead<u32>(cameraView(i_this)->mFovy);
    if (demo == 0) {
        JUT_ASSERT_fail(STR(0x1004A794), 0x23E, STR(0x1004A784));
        demo = sDemo.get();
    }
    u32 dc = demoGetCamera(cameraArgument(demo)); /* dComIfGp_demo_getCamera() */
    if (dc != 0) {
        saveC->copy(*C);
        saveE->copy(*E);
        u32 en = gabi::at<DemoCamera>(dc)->mEnable.get();
        s16 bank0 = *bank;
        if (en & 0x40) {
            C->copy(*gabi::at<cXyz>(gabi::ea(&gabi::at<DemoCamera>(dc)->mTarget)));
            en = gabi::at<DemoCamera>(dc)->mEnable.get();
        } /* getTarget() */
        if (en & 0x10) {
            E->copy(*gabi::at<cXyz>(gabi::ea(&gabi::at<DemoCamera>(dc)->mTrans)));
            en = gabi::at<DemoCamera>(dc)->mEnable.get();
        } /* getTrans() */
        if (en & 0x20) { /* getUp() */
            upY = fieldRead<u32>(gabi::at<DemoCamera>(dc)->mUp.y);
            upX = fieldRead<u32>(gabi::at<DemoCamera>(dc)->mUp.x);
            upZ = fieldRead<u32>(gabi::at<DemoCamera>(dc)->mUp.z);
        }
        if (en & 0x80) { /* bank = cAngle::d2s(-getRoll()) */
            s32 r = gabi::ftoi(-(fieldRead<f32>(gabi::at<DemoCamera>(dc)->mRoll) * 182.04445f));
            CameraAngle* a = gabi::at<CameraAngle>(
                angleConstructValue(cameraArgument(tmp.get()), cameraArgument((s16)r)));
            *bank = (s16)*a;
            en = gabi::at<DemoCamera>(dc)->mEnable.get();
        }
        if (en & 4)
            fovy = fieldRead<u32>(gabi::at<DemoCamera>(dc)->mFovy); /* getFovy() */
        gabi::Local<CameraProcessHD::SafeString> a, b;
        a->vtable = kSafeStringVtable;
        b->vtable = kSafeStringVtable;
        a->top = kDemo46Name; /* "Demo46" */
        b->top = kDemoName;   /* current demo name */
        if (safeStringEqual(a, b, true, false)) {
            const u32 S1 = gabi::ea(&sCenterJump), S2 = gabi::ea(&sEyeJump);
            if (sCenterJumpInitialized.get() == 0) {
                gabi::at<cXyz>(S1)->copy(*&sZeroVector);
                sCenterJumpInitialized = 1;
            }
            if (sEyeJumpInitialized.get() == 0) {
                gabi::at<cXyz>(S2)->copy(*&sZeroVector);
                sEyeJumpInitialized = 1;
            }
            s32 frame = sDemoFrame.get();
            bool firstDemoFrame = false;
            if (frame == 0x36E) {
                if (sPreviousDemoFrame.get() != 0x36E) {
                    cXyz_mi(C, t, saveC);
                    gabi::at<cXyz>(S1)->copy(*t);
                    cXyz_mi(E, t, saveE);
                    gabi::at<cXyz>(S2)->copy(*t);
                    s16 bk = *bank;
                    if (!firstDemoFrame)
                        sPreviousDemoFrame = frame;
                    sBankJump = (s16)(bk - bank0);
                    firstDemoFrame = true;
                }
                if (!firstDemoFrame) {
                    cameraVectorScale(cameraArgument(S1), cameraArgument(S1),
                                      0.96f); /* PSVECScale */
                    cameraVectorScale(cameraArgument(S2), cameraArgument(S2), 0.96f);
                    s32 db = gabi::ftoi((f32)sBankJump.get() * 0.96f);
                    sBankJump = (s16)db;
                    cXyz_pl(saveC, t, gabi::at<cXyz>(S1));
                    C->copy(*t);
                    cXyz_pl(saveE, t, gabi::at<cXyz>(S2));
                    E->copy(*t);
                    CameraAngle* a2 = gabi::at<CameraAngle>(angleConstructValue(
                        cameraArgument(tmp.get()), cameraArgument((s16)(bank0 + sBankJump.get()))));
                    *bank = (s16)*a2;
                }
            }
            if (!firstDemoFrame)
                sPreviousDemoFrame = frame;
        }
    } else if (!(body->mEventFlags & 1)) {
        cXyz_pl(&body->mCenter, t, &body->mCenterShake); /* body->Center() */
        C->copy(*t);
        cXyz_pl(&body->mEye, t, &body->mEyeShake); /* body->Eye() */
        E->copy(*t);
        upX = fieldRead<u32>(body->mUp.x); /* body->Up() */
        vectorWords(t.get())->mWord[0] = upX;
        upY = fieldRead<u32>(body->mUp.y);
        vectorWords(t.get())->mWord[1] = upY;
        upZ = fieldRead<u32>(body->mUp.z);
        vectorWords(t.get())->mWord[2] = upZ;
        angleAdd(cameraArgument(&body->mBank), cameraArgument(tmp.get()),
                 cameraArgument(&body->mBankShake)); /* body->Bank() */
        *bank = (s16)*tmp;
        fovy = std::bit_cast<u32>(body->mFovy + body->mFovYShake); /* body->Fovy() */
    }
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.x,
                    vectorWords(C.get())->mWord[0].get()); /* fopCamM_SetCenter */
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.y, vectorWords(C.get())->mWord[1].get());
    fieldWrite<u32>(cameraView(i_this)->mLookat.mCenter.z, vectorWords(C.get())->mWord[2].get());
    fieldWrite<u32>(cameraView(i_this)->mLookat.mEye.x,
                    vectorWords(E.get())->mWord[0].get()); /* fopCamM_SetEye */
    fieldWrite<u32>(cameraView(i_this)->mLookat.mEye.y, vectorWords(E.get())->mWord[1].get());
    fieldWrite<u32>(cameraView(i_this)->mLookat.mEye.z, vectorWords(E.get())->mWord[2].get());
    fieldWrite<u32>(cameraView(i_this)->mLookat.mUp.x, upX); /* fopCamM_SetUp */
    fieldWrite<u32>(cameraView(i_this)->mLookat.mUp.y, upY);
    fieldWrite<u32>(cameraView(i_this)->mLookat.mUp.z, upZ);
    fieldWrite<s16>(cameraView(i_this)->mBank, *bank); /* fopCamM_SetBank */
    fieldWrite<u32>(cameraView(i_this)->mFovy, fovy);  /* fopCamM_SetFovy */

    u32 stage = dComIfGp_ea() + 0x5150;
    u32 stat = cameraInfo(camId)->mCameraAttentionStatus.get();
    if ((stat & 0x48) && !(playPlayerStatus()->get() & 0x100000)) {
        fieldWrite<f32>(cameraView(i_this)->mNear, 30.0f); /* telescope: fopCamM_SetNear(30) */
        if (stage != 0) {
            u32 info = gabi::call_ptr<u32>(
                gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage);
            fieldWrite<u32>(
                cameraView(i_this)->mFar,
                fieldRead<u32>(
                    gabi::at<StageInfo>(info)->mFarPlane)); /* fopCamM_SetFar(mFarPlane) */
        }
    } else if (stage != 0) {
        gabi::Local<be<f32>> nearV, farV;
        u32 info = gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage);
        fieldWrite<u32>(*nearV,
                        fieldRead<u32>(gabi::at<StageInfo>(info)->mNearPlane)); /* mNearPlane */
        info = gabi::call_ptr<u32>(
            gabi::at<StageVtable>(gabi::at<Stage>(stage)->mVtable)->getStagInfo.get(), stage);
        fieldWrite<u32>(*farV,
                        fieldRead<u32>(gabi::at<StageInfo>(info)->mFarPlane)); /* mFarPlane */
        dCamera_hdAdjustNear(body, nearV, farV);
        fieldWrite<u32>(cameraView(i_this)->mNear, fieldRead<u32>(*nearV));
        fieldWrite<u32>(cameraView(i_this)->mFar, fieldRead<u32>(*farV));
    }
    fieldWrite<s16>(reinterpret_cast<Process*>(i_this)->mAngle.y,
                    cSAngle_Inv(&body->mDirection.mU)); /* fopCamM_SetAngleY */
    fieldWrite<s16>(reinterpret_cast<Process*>(i_this)->mAngle.x,
                    (s16)body->mDirection.mV); /* fopCamM_SetAngleX */
}

/* stack dBgS_GndChk (HD: constructor partly inlined; same as in d_camera_bg.cpp) */
static void gndChk_ct_at(u32 o, const cXyz* pos) {
    groundCheckConstruct(cameraArgument(o));
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbObjThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbCamThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbLinkThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbArrowThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbBombThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbBoomerangThrough, 0);
    fieldWrite<u8>(gabi::at<dBgS_GndChk>(o)->mbRopeThrough, 0);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_Chk::__vtbl, kGroundCheckVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl, kGroundGroupVtable);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrp, 1);
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mPolyPassChk,
                    gabi::ea(&gabi::at<dBgS_GndChk>(o)->dBgS_PolyPassChk::__vtbl));
    fieldWrite<u32>(gabi::at<dBgS_GndChk>(o)->mGrpPassChk,
                    gabi::ea(&gabi::at<dBgS_GndChk>(o)->dBgS_GrpPassChk::__vtbl));
    gabi::at<cXyz>(gabi::ea(&gabi::at<dBgS_GndChk>(o)->m_pos))->copy(*pos);
}

/* 024FFC40 */
/* HD: besides the GameCube work (scissor below/above the trim bars, projection, view,
 * audio camera info, ground sound, map draw) the body's sead::LookAtCamera (eye, centre, up rotated
 * by the bank around the view direction) and sead projection (near/far/fovy/aspect; 4:3 for the
 * cinema-scope trim in one display mode; a zoom factor eased towards 1) are updated, and the sead
 * camera matrix is copied to j3dSys's view matrix  */
static BOOL camera_draw(camera_class* i_this) {
    dCamera_c* body = gabi::at<dCamera_c>(gabi::ea(&reinterpret_cast<Process*>(i_this)->mCamera));
    s32 camId = fopCamM_GetParam(i_this);
    s32 winId = fieldRead<s8>(cameraInfo(camId)->mDlstWindowIdx);
    u32 window = dComIfGp_ea() + PLAY_OFF(mDlstWindow) + winId * 0x2C;
    s32 camId2 = fopCamM_GetParam(i_this);
    f32 trim = (f32)gabi::ftoi(body->mTrimHeight);
    windowSetScissor(cameraArgument(window), 0.0f, trim, 1280.0f,
                     720.0f - (trim + trim)); /* window->setScissor */
    cameraPerspective(
        cameraArgument(gabi::ea(&cameraView(i_this)->mProjMtx)),
        fieldRead<f32>(cameraView(i_this)->mFovy), fieldRead<f32>(cameraView(i_this)->mAspect),
        fieldRead<f32>(cameraView(i_this)->mNear),
        fieldRead<f32>(
            cameraView(i_this)->mFar)); /* C_MTXPerspective(proj, fovy, aspect, near, far) */
    cameraLookAt(
        cameraArgument(gabi::ea(&cameraView(i_this)->mViewMtx)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mEye.x)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mCenter.x)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mUp.x)),
        cameraArgument((s16)fieldRead<s16>(cameraView(i_this)->mBank))); /* mDoMtx_lookAt */

    /* sead camera: eye E, centre C, up U, direction D */
    gabi::Local<cXyz> E, C, U, D, Dsave;
    gabi::Local<be<f32>[4]> q;
    gabi::Local<be<f32>[9]> m;
    for (u32 k = 0; k < 12; k += 4) {
        vectorWords(E.get())->mWord[k / 4] =
            reinterpret_cast<WordVector*>(&cameraView(i_this)->mLookat.mEye)->mWord[k / 4].get();
        vectorWords(C.get())->mWord[k / 4] =
            reinterpret_cast<WordVector*>(&cameraView(i_this)->mLookat.mCenter)->mWord[k / 4].get();
        vectorWords(U.get())->mWord[k / 4] =
            reinterpret_cast<WordVector*>(&cameraView(i_this)->mLookat.mUp)->mWord[k / 4].get();
    }
    f32 dx = C->x - E->x;
    f32 dy = C->y - E->y;
    f32 dz = C->z - E->z;
    D->x = dx;
    D->y = dy;
    D->z = dz;
    f32 len = cameraVectorNormalize(cameraArgument(D.get())); /* sead::Vector3f::normalize */
    if (len == 0.0f) {
        C->z = C->z + 1.0f;
        D->x = dx;
        D->y = dy;
        D->z = C->z - E->z;
        cameraVectorNormalize(cameraArgument(D.get()));
    }
    f32 ux = U->x;
    const f32 eps = 3.8146973e-06f;
    f32 uy, uz;
    if (-eps > ux || ux > eps) {
        uz = U->z;
        uy = U->y;
    } else {
        uy = U->y;
        if (-eps > uy || uy > eps) {
            uz = U->z;
        } else {
            uz = U->z;
            if (!(-eps > uz) && !(uz > eps))
                uy = 1.0f;
        }
    }
    f32 bank = (f32)fieldRead<s16>(cameraView(i_this)->mBank) * 9.58738e-05f;
    quaternionAxisAngle(cameraArgument(q.get()), cameraArgument(D.get()),
                        bank); /* sead::Quatf::setAxisAngle(dir, bank) */
    matrixFromQuaternion(cameraArgument(m.get()),
                         cameraArgument(q.get())); /* sead::Matrix33f::fromQuat */
    be<f32>* M = *m;
    U->x = gabi::fmadds(M[2], uz, gabi::fmadds(M[0], ux, M[1] * uy));
    U->y = gabi::fmadds(M[5], uz, gabi::fmadds(M[3], ux, M[4] * uy));
    U->z = gabi::fmadds(M[8], uz, gabi::fmadds(M[6], ux, M[7] * uy));
    for (u32 k = 0; k < 12; k += 4) {
        vectorWords(&hdCamera(body)->mPos)->mWord[k / 4] =
            vectorWords(E.get())->mWord[k / 4].get(); /* LookAtCamera pos */
        vectorWords(&hdCamera(body)->mAt)->mWord[k / 4] =
            vectorWords(C.get())->mWord[k / 4].get(); /* at */
        vectorWords(&hdCamera(body)->mUp)->mWord[k / 4] =
            vectorWords(U.get())->mWord[k / 4].get(); /* up */
    }
    cameraVectorNormalize(cameraArgument(gabi::ea(&hdCamera(body)->mUp)));
    u32 lookAt = gabi::ea(&hdCamera(body)->mMatrix);
    gabi::call_ptr(gabi::at<CameraVtable>(hdCamera(body)->mVtable)->doUpdateMatrix.get(), lookAt,
                   lookAt); /* camera.doUpdateMatrix(&mtx) */
    projectionSet(
        cameraArgument(gabi::ea(&hdProjection(body)->mDirty)),
        fieldRead<f32>(cameraView(i_this)->mNear), fieldRead<f32>(cameraView(i_this)->mFar),
        fieldRead<f32>(cameraView(i_this)->mFovy) * 0.017453292f,
        fieldRead<f32>(cameraView(i_this)->mAspect)); /* projection.set(near, far, fovy, aspect) */
    s32 trimSize = body->mTrimSize;
    if ((trimSize == 2 || trimSize == 3) && sDisplayMode.get() == 1) {
        fieldWrite<f32>(hdZoom(body)->mScale, 1.3333334f);
        fieldWrite<f32>(hdZoom(body)->mTarget, 1.3333334f);
        projectionSetZoom(cameraArgument(gabi::ea(&hdProjection(body)->mDirty)),
                          cameraArgument(gabi::ea(&hdZoom(body)->mScale)));
    } else {
        gabi::Local<be<f32>> zoom;
        fieldWrite<u32>(*zoom, fieldRead<u32>(hdZoom(body)->mScale));
        if (cameraIsZoomRequested() != 0) {
            *zoom = 2.0f;
            fieldWrite<u8>(body->mHD738, 1);
            u32 p = displayGetSize();
            f32 k = 63.0f / gabi::at<DisplaySize>(p)->mHeight.get();
            fieldWrite<f32>(hdProjection(body)->mOffsetX, 0.0f);
            fieldWrite<u8>(hdProjection(body)->mDirty, 1);
            fieldWrite<f32>(hdProjection(body)->mOffsetY, k * *zoom);
        } else if (fieldRead<u8>(body->mHD738) != 0) {
            *zoom = 1.0f;
            fieldWrite<u8>(body->mHD738, 0);
            fieldWrite<u32>(hdProjection(body)->mOffsetX, fieldRead<u32>(sInitialProjectionOffset));
            fieldWrite<u8>(hdProjection(body)->mDirty, 1);
            fieldWrite<u32>(hdProjection(body)->mOffsetY,
                            fieldRead<u32>(sInitialProjectionOffset.mTarget));
        } else {
            cameraAddCalc(cameraArgument(zoom.get()), 1.0f, 0.2f, 0.3f, 0.1f); /* cLib_addCalc */
        }
        fieldWrite<u32>(hdZoom(body)->mScale, fieldRead<u32>(*zoom));
        fieldWrite<u32>(hdZoom(body)->mTarget, fieldRead<u32>(*zoom));
        projectionSetZoom(cameraArgument(gabi::ea(&hdProjection(body)->mDirty)),
                          cameraArgument(gabi::ea(&hdZoom(body)->mScale)));
    }
    for (u32 k = 0; k < 0x30; k += 4)
        sJ3DView.mWord[k / 4] = reinterpret_cast<WordMatrix*>(&hdCamera(body)->mMatrix)
                                    ->mWord[k / 4]
                                    .get(); /* j3dSys.setViewMtx */
    u32 proj = projectionGetMatrix(cameraArgument(gabi::ea(&hdProjection(body)->mDirty)));
    cameraCopyProjection(cameraArgument(proj), cameraArgument(gabi::ea(&sJ3DProjection)));
    cameraInverseView(cameraArgument(gabi::ea(&cameraView(i_this)->mViewMtx)),
                      cameraArgument(gabi::ea(
                          &cameraView(i_this)->mInvViewMtx))); /* PSMTXInverse(view, invView) */
    audioCameraInfo(cameraArgument(gabi::ea(&cameraView(i_this)->mLookat.mEye.x)),
                    cameraArgument(gabi::ea(&sJ3DView)),
                    cameraArgument(camId2)); /* mDoAud_getCameraInfo */

    gabi::Local<dBgS_GndChk> gnd;
    u32 g = gabi::ea(gnd.get());
    gndChk_ct_at(g, gabi::at<cXyz>(gabi::ea(&cameraView(i_this)->mLookat.mEye.x)));
    f32 groundY = cameraGroundCross(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                    cameraArgument(g)); /* GroundCross */
    if (groundY != -1000000000.0f) {
        u32 snd = cameraMaterialSound(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                      cameraArgument(g + 0x14)); /* GetMtrlSndId */
        audioCameraMapInfo(cameraArgument(snd));                 /* mDoAud_getCameraMapInfo */
        u32 grp = cameraGroupSound(cameraArgument(dComIfGp_ea() + PLAY_BGS),
                                   cameraArgument(g + 0x14)); /* GetGrpSoundId */
        audioCameraGroupInfo(cameraArgument(grp & 0xFF));     /* mDoAud_setCameraGroupInfo */
        u32 iface = sAudioInterface.get();
        gabi::Local<cXyz> polyPos;
        vectorWords(polyPos.get())->mWord[0] = fieldRead<u32>(cameraView(i_this)->mLookat.mEye.x);
        polyPos->y = groundY;
        vectorWords(polyPos.get())->mWord[2] = fieldRead<u32>(cameraView(i_this)->mLookat.mEye.z);
        if (iface != 0)
            audioCameraPolygonPos(cameraArgument(iface),
                                  cameraArgument(polyPos.get())); /* setCameraPolygonPos */
    } else {
        u32 iface = sAudioInterface.get();
        if (iface != 0)
            audioCameraPolygonPos(cameraArgument(iface), cameraArgument(0));
    }
    PSMTXCopy(gabi::at<Mtx34>(gabi::ea(&cameraView(i_this)->mViewMtx)),
              gabi::at<Mtx34>(gabi::ea(&cameraView(i_this)->mViewMtxNoTrans)));
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[0][3], 0.0f);
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[1][3], 0.0f);
    fieldWrite<f32>(cameraView(i_this)->mViewMtxNoTrans.m[2][3], 0.0f);
    cameraConcatProjView(
        cameraArgument(gabi::ea(&cameraView(i_this)->mProjMtx)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mViewMtx)),
        cameraArgument(gabi::ea(&cameraView(i_this)->mProjViewMtx))); /* mDoMtx_concatProjView */
    body->Draw();
    if (cameraDrawPriority(cameraArgument(i_this)) != 1) { /* fpcM_DrawPriority */
        fopCamM_GetParam(i_this);
        dComIfGp_ea();
        dComIfGp_ea();
        if (!cameraOverlapRequest()) { /* fopOvlpM_IsDoingReq */
            f32 px = gabi::at<dComIfG_inf_c>(dComIfGp_ea())
                         ->play.mPlayerInfo[0]
                         .mpPlayer->current.pos.x.get();
            f32 pz = gabi::at<dComIfG_inf_c>(dComIfGp_ea())
                         ->play.mPlayerInfo[0]
                         .mpPlayer->current.pos.z.get();
            s32 stay = sStayRoom.get(); /* dComIfGp_roomControl_getStayNo() */
            f32 py = gabi::at<dComIfG_inf_c>(dComIfGp_ea())
                         ->play.mPlayerInfo[0]
                         .mpPlayer->current.pos.y.get();
            cameraMapDraw(cameraArgument(stay), px, pz, py); /* dComIfGp_map_draw */
        }
    }
    fieldWrite<u32>(gnd->cBgS_PolyInfo::__vtbl, kGroundPolyInfoVtable);
    fieldWrite<u32>(gnd->dBgS_PolyPassChk::__vtbl, kGroundPassVtable);
    fieldWrite<u32>(gnd->dBgS_GrpPassChk::__vtbl, kGroupBaseVtable);
    groundCheckDestroy(cameraArgument(g), cameraArgument(0));
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025DA64C s32 cameraGetParam(u32)
ext 025F1EAC void cameraLookAt(u32,u32,u32,u32,u32)
ext 025F11AC void clipperSetup(f32,f32,f32,f32)
ext 025283F8 u32 demoGetCamera(u32)
ext 024F8C54 void cameraResetView(u32,u32)
ext 024FE3E8 void cameraRun(u32)
ext 024FF6C0 void cameraNotRun(u32)
ext 024F8430 void cameraStore(u32)
ext 025E1B44 void audioCameraInfo(u32,u32,u32)
ext 02008E0C void groundCheckConstruct(u32)
ext 020065FC void angleConstruct(u32)
ext 02007100 void globeConstruct(u32)
ext 02501B8C void cameraConstruct(u32,u32)
ext 024F7318 void paramDestroy(u32,u32)
ext 024F7858 void setupDestroy(u32,u32)
ext 02738A6C void projectionDestroy(u32,u32)
ext 0274CCB0 void lookAtDestroy(u32,u32)
ext 02008DAC void groundCheckDestroy(u32,u32)
ext 0252CCFC void effectLineDestroy(u32,u32)
ext 025021B8 void cameraDestroy(u32,u32)
ext 027C1F70 void renderManagerDetach(u32,u32)
ext 02525FE4 s32 cameraPhaseHandler(u32,u32,u32)
ext 024FF164 bool cameraEvent(u32,u32)
ext 024F8D4C bool focusLineOff(u32)
ext 024FC108 f32 cameraShake(u32)
ext 028F521C void cameraValueInitialize(u32,u32)
ext 028F0020 void cameraConstructArray(u32,u32,u32,u32,u32)
ext 0252CD20 void focusLineConstruct(u32)
ext 02500508 void backgroundConstruct(u32)
ext 025C0BD0 void splineConstruct(u32)
ext 027389F8 void projectionConstruct(u32)
ext 024F75E4 void setupConstruct(u32)
ext 024F72BC void paramConstruct(u32,u32)
ext 02500984 void cameraInitialize(u32,u32,u32,u32,u32)
ext 02500880 void focusLineInit(u32)
ext 025269A8 f32 pirateShipOffsetY(u32,u32,f32)
ext 024F727C void paramChange(u32,u32)
ext 024F7464 f32 paramCenterHeight(u32,f32)
ext 0200658C u32 angleConstructValue(u32,u32)
ext 02007248 void globeConstructAngles(u32,f32,u32,u32)
ext 020073AC void globeXyz(u32,u32)
ext 02006FE4 void globeSetAngles(u32,f32,u32,u32)
ext 024F7478 f32 paramFovy(u32,f32)
ext 025155D4 void safeStringTerminate(u32)
ext 0200658C void angleConstructValueDiscard(u32,u32)
ext 028E8E64 void cameraVectorScale(u32,u32,f32)
ext 02006894 void angleAdd(u32,u32,u32)
ext 0252CCA8 void windowSetScissor(u32,f32,f32,f32,f32)
ext 028E9948 void cameraPerspective(u32,f32,f32,f32,f32)
ext 025155D8 f32 cameraVectorNormalize(u32)
ext 02515644 void quaternionAxisAngle(u32,u32,f32)
ext 025156EC void matrixFromQuaternion(u32,u32)
ext 0274E04C void projectionSet(u32,f32,f32,f32,f32)
ext 02738AC0 void projectionSetZoom(u32,u32)
ext 025BDC60 u32 cameraIsZoomRequested()
ext 02739728 u32 displayGetSize()
ext 0200ECD4 f32 cameraAddCalc(u32,f32,f32,f32,f32)
ext 0274D83C u32 projectionGetMatrix(u32)
ext 028E8970 void cameraCopyProjection(u32,u32)
ext 028E91EC void cameraInverseView(u32,u32)
ext 02008974 f32 cameraGroundCross(u32,u32)
ext 024EECAC u32 cameraMaterialSound(u32,u32)
ext 025E1B60 void audioCameraMapInfo(u32)
ext 024EEDB8 u32 cameraGroupSound(u32,u32)
ext 025E1DE4 void audioCameraGroupInfo(u32)
ext 02027754 void audioCameraPolygonPos(u32,u32)
ext 025F1FC4 void cameraConcatProjView(u32,u32,u32)
ext 025DF2B8 s32 cameraDrawPriority(u32)
ext 025DBE38 bool cameraOverlapRequest()
ext 0258FD88 void cameraMapDraw(u32,f32,f32,f32)
data sRenderManager 101F95D0
data sDisplayMode 101D6010
data sDemo 101D5FFC
data sAutoFocus 101F4828
data sZeroAngle 101FF354
data sZeroVector 101FFBA8
data sInitialAudioPosition 1004AB24
data sIdentity 104A041C
data sInitialZoom 104A0920
data sInitialProjectionOffset 104A0908
data sJ3DView 104B45F8
data sJ3DProjection 104B470C
data sCenterJump 1046EF90
data sEyeJump 1046EF9C
data sCenterJumpInitialized 1046EFA8
data sEyeJumpInitialized 1046EFAC
data sDemoFrame 101D600C
data sPreviousDemoFrame 101D5530
data sBankJump 101D5534
data sAudioInterface 101FFC78
data sStayRoom 1047E6C8
data sActorStopStatus 101F3084
*/
