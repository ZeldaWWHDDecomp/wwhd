// HD name scene.
#include "d/d_s_name.h"

using FrontendPointer = gptr<void>;
using ControllerPointer = gptr<NameScene::Controller>;
using ResetStatePointer = gptr<NameScene::ResetState>;
using SaveInfoPointer = gptr<NameScene::SaveView>;
static FrontendPointer l_frontend;
static ControllerPointer l_nameInput;
static ControllerPointer l_fileSelect;
static ControllerPointer l_fileError;
static ResetStatePointer l_resetState;
static SaveInfoPointer l_saveInfo;
static NameScene::StateDescriptor l_nameCancelled;
static NameScene::StateDescriptor l_nameAccepted;
static NameScene::StateDescriptor l_fileFinished;
static NameScene::StateDescriptor l_formatFinished;
static Mtx34 l_j3dViewMtx;

using RendererPointer = gptr<NameScene::Renderer>;
using GameInfoPointer = gptr<NameScene::GameInfo>;
using EnvironmentPointer = gptr<NameScene::Environment>;
static RendererPointer l_renderer;
static GameInfoPointer l_gameInfo;
static EnvironmentPointer l_environment;
static u8 l_cameraVtable;
static u8 l_seadCameraVtable;
static u8 l_lookAtVtable;
static NameScene::MatrixWords l_identity;
static NameScene::HeaderStatics l_headerStatics;
static NameScene::PiPair l_pi;

static NameScene::PlayView* getPlay() {
    return reinterpret_cast<NameScene::PlayView*>(dComIfGp_get());
}

using TypeIDPointer = gptr<be<s16>>;
using MenuSavePointer = gptr<void>;
static TypeIDPointer l_ambientType;
static TypeIDPointer l_directionalType;
static TypeIDPointer l_pointType;
static TypeIDPointer l_fogType;
static MenuSavePointer l_menuSave;
static u8 l_menuSaveActive;
static u8 l_autoFocus;
static u8 l_safeStringVtable;
static u8 l_phaseMethod;
static char l_heapName;
using FloatConstant = be<f32>;
static FloatConstant l_aspectRatio;
static FloatConstant l_degreesToRadians;

static void* resolveRelative(NameScene::RelativeOffset* pointer) {
    // HD: binary resource offsets are relative to the offset word and wrap at 32 bits.
    u32 offset = pointer->mOffset;
    return offset ? gabi::at<void>(gabi::ea(pointer) + offset) : nullptr;
}

/* 025AD9B8 */
static u32 phase_1(void* resName) {
    mDoAud_bgmStart(NameScene::JA_BGM_SELECT);
    return dComIfG_setStageRes(resName, 0) ? cPhs_NEXT_e : cPhs_ERROR_e;
}

/* 025ADA0C */
static u32 phase_2() {
    s32 result = dComIfG_syncStageRes();
    if (result < 0) return cPhs_ERROR_e;
    if (result > 0) return cPhs_INIT_e;
    // HD: stage loading also starts the UI frontend's asynchronous load.
    return Frontend_load(l_frontend, 3) ? cPhs_NEXT_e : cPhs_INIT_e;
}

/* 025ADA7C */
static u32 phase_3() {
    // HD: frontend readiness starts a separate font-loading phase.
    if (!Frontend_isReady(l_frontend)) return cPhs_INIT_e;
    Font_load();
    return cPhs_NEXT_e;
}

/* 025ADAC8 */
static u32 phase_4() {
    // HD: wait for the font manager before completing the resource phases.
    return Font_isReady() ? cPhs_COMPLEATE_e : cPhs_INIT_e;
}

/* 025ADB04 */
void dScnName_c::setView() {
    // HD: the play accessor is retained even though its result is unused here.
    dComIfGp_get();
    NameScene::view_class* view = &mCamera.view;
    C_MTXPerspective(&view->mProjMtx, view->mFovy, view->mAspect, view->mNear, view->mFar);
    mDoMtx_lookAt(&view->mViewMtx, &view->mLookat.mEye, &view->mLookat.mCenter, view->mBank);
    cMtx_inverse(&view->mViewMtx, &view->mInvViewMtx);
    PSMTXCopy(&view->mViewMtx, &view->mViewMtxNoTrans);
    // HD: GHS clears the translation entries in z/x/y order.
    view->mViewMtxNoTrans.m[2][3] = 0;
    view->mViewMtxNoTrans.m[0][3] = 0;
    view->mViewMtxNoTrans.m[1][3] = 0;
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = view->mViewMtx.m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i) l_j3dViewMtx.m[i / 4][i % 4] = matrix[i];
    cMtx_concatProjView(&view->mProjMtx, &view->mViewMtx, &view->mProjViewMtx);
}

/* 025ADC0C */
u32 dScnName_c::draw() {
    // HD: retain the guest call boundary for the separately verified setView entry.
    setView();
    create_tag_class* tag = fopDwIt_Begin();
    while (tag) {
        fpcM_Draw(gabi::at<void>(tag->mpTagData));
        tag = fopDwIt_Next(tag);
    }
    return 1;
}

/* 025ADC5C */
static u32 dScnName_Draw(dScnName_c* i_this) {
    return i_this->draw();
}

/* 025ADC60 */
void dScnName_c::changeGameScene() {
    if (fopOvlpM_IsPeek()) return;
    dComIfGs_gameStart();
    if (!fopScnM_ChangeReq(this, field_0x55f ? NameScene::OPEN_SCENE : NameScene::PLAY_SCENE,
                                   NameScene::OVERLAP0, 5, 1)) return;
    // HD: keep both save singleton reads and each play accessor call.
    l_saveInfo->field_0x7BC = 255;
    l_saveInfo->mRestartRoomParam = 0;
    NameScene::PlayView* play = reinterpret_cast<NameScene::PlayView*>(dComIfGp_get());
    NameScene::Stage* stage = &play->mNextStage;
    play = reinterpret_cast<NameScene::PlayView*>(dComIfGp_get());
    s32 room = play->mNextStage.mRoomNo;
    play = reinterpret_cast<NameScene::PlayView*>(dComIfGp_get());
    s32 layer = play->mNextStage.mLayer;
    mDoAud_setSceneName(stage, room, layer);
}

static bool isControllerState(NameScene::Controller* owner, NameScene::StateDescriptor* desc) {
    // HD: compare state IDs through the original virtual slots, preserving target-load order.
    NameScene::StateDescriptor* state = StateMgr_getState(owner->mStateMgr);
    NameScene::StateVtable* descVtable = desc->__vtbl;
    u32 a = gabi::call_ptr<u32>(state->__vtbl->getID.get(), state);
    u32 b = gabi::call_ptr<u32>(descVtable->getID.get(), desc);
    return a == b;
}

/* 025ADD1C */
void dScnName_c::NameInMain() {
    // HD: the GameCube name window is replaced by a state-based UI controller.
    NameScene::Controller* controller = l_nameInput;
    Controller_move(controller);
    if (isControllerState(controller, &l_nameCancelled)) {
        dComIfG_changeOpeningScene(this, 8);
        return;
    }
    if (isControllerState(controller, &l_nameAccepted)) {
        field_0x55f = controller->field_0x145;
        changeGameScene();
    }
}

/* 025ADE28 */
void dScnName_c::FileSelectMain() {
    // HD: use the file-select state controller rather than the GameCube window implementation.
    NameScene::Controller* controller = l_fileSelect;
    Controller_move(controller);
    if (isControllerState(controller, &l_fileFinished)) dComIfG_changeOpeningScene(this, 8);
}

/* 025ADEBC */
void dScnName_c::MemCardFormat() {
    // HD: formatting is owned by the UI state controller.
    NameScene::Controller* controller = l_fileError;
    Controller_move(controller);
    if (isControllerState(controller, &l_formatFinished)) dComIfG_changeOpeningScene(this, 8);
}

/* 025ADF50 */
u32 dScnName_c::execute() {
    if (!fopOvlpM_IsPeek()) dComIfG_resetToOpening(this);
    if (!l_resetState->mReset) {
        s16 name = mProcName;
        if (name == NameScene::NAME_SCENE) NameInMain();
        else if (name == NameScene::NAMEEX_SCENE) FileSelectMain();
        else if (name == NameScene::FORMAT_SCENE) MemCardFormat();
        Environment_update(&mEnvironment);
    }
    return 1;
}

/* 025ADFF8 */
static u32 dScnName_Execute(dScnName_c* i_this) {
    return i_this->execute();
}

// HD: deleting destructor for the larger sead camera/environment scene.
/* 025ADFFC */
static void dScnName_c__dt(dScnName_c* i_this, u32 flags) {
    if (!i_this) return;
    mDoExt_restoreCurrentHeap(i_this->oldHeap);
    dComIfG_deleteStageRes("Stage");
    getPlay()->mWindowNum = 0;
    dSv_restart_setRestartOption(l_saveInfo->mRestart, 0);
    switch (s16(i_this->mProcName)) {
    case NameScene::NAME_SCENE:
        Controller_delete(l_nameInput);
        NameController_deleteSingleton();
        break;
    case NameScene::NAMEEX_SCENE:
        Controller_delete(l_fileSelect);
        FileSelectController_deleteSingleton();
        break;
    case NameScene::FORMAT_SCENE:
        Controller_delete(l_fileError);
        FileErrorController_deleteSingleton();
        break;
    }
    NameScene::Heap* heap = i_this->heap;
    gabi::call_ptr<void>(heap->__vtbl->destroy.get(), heap);
    NameScene::Renderer* renderer = l_renderer;
    // HD: the current display and first display are both reset; each expression reloads the table.
    renderer->mDisplay[renderer->mDisplayCount > 1 ? 1 : 0]->mCamera = nullptr;
    renderer->mDisplay[renderer->mDisplayCount > 1 ? 1 : 0]->mProjection = nullptr;
    renderer->mDisplay[0]->mCamera = nullptr;
    renderer->mDisplay[0]->mProjection = nullptr;
    Renderer_setEnvironment(renderer, 0);
    getPlay()->mCamera = nullptr;
    getPlay()->mCurrentWindow = nullptr;
    getPlay()->mViewport = nullptr;
    getPlay()->mView = nullptr;
    l_environment = nullptr;
    Environment_dt(&i_this->mEnvironment, 2);
    Projection_dt(&i_this->mProjection, 2);
    LookAtCamera_dt(&i_this->mLookAtCamera, 2);
    NameSceneBaseProcess_dt(&i_this->mCamera, 0);
    NameSceneBaseProcess_dt(i_this, 2);
    if (flags & 1) operator_delete(i_this);
}

/* 025AE208 */
static u32 dScnName_Delete(dScnName_c* i_this) {
    // HD: defer deletion while the UI fade is in state 2.
    if (l_gameInfo->mFadeState == 2) return 0;
    dScnName_c__dt(i_this, 2);
    return 1;
}

// HD: GHS nullable constructor and subobject construction order.
/* 025AE25C */
static dScnName_c* dScnName_c__ct(dScnName_c* i_this) {
    if (!i_this) {
        i_this = static_cast<dScnName_c*>(operator_new(sizeof(dScnName_c)));
        if (!i_this) return nullptr;
    }
    NameSceneBaseProcess_ct(i_this);
    NameSceneBaseProcess_ct(&i_this->mCamera);
    i_this->mCamera.view.__vtbl = gabi::ea(&l_cameraVtable);
    i_this->mLookAtCamera.__vtbl = gabi::ea(&l_seadCameraVtable);
    for (u32 i = 0; i < 12; ++i) i_this->mLookAtCamera.mMatrixWords[i] = l_identity.m[i];
    i_this->mLookAtCamera.__vtbl = gabi::ea(&l_lookAtVtable);
    // HD: preserve GHS's nullable, wrapping subobject constructor checks.
    cXyz* eye = gabi::at<cXyz>(gabi::ea(&i_this->mLookAtCamera.mEye));
    if (!eye) eye = static_cast<cXyz*>(operator_new(sizeof(cXyz)));
    if (eye) { eye->y = 0; eye->x = 0; eye->z = 10; }
    cXyz* at = gabi::at<cXyz>(gabi::ea(&i_this->mLookAtCamera.mAt));
    if (!at) at = static_cast<cXyz*>(operator_new(sizeof(cXyz)));
    if (at) { at->y = 0; at->x = 0; at->z = 0; }
    cXyz* up = gabi::at<cXyz>(gabi::ea(&i_this->mLookAtCamera.mUp));
    if (!up) up = static_cast<cXyz*>(operator_new(sizeof(cXyz)));
    if (up) { up->z = 0; up->y = 1; up->x = 0; }
    Projection_ct(&i_this->mProjection);
    Environment_ct(&i_this->mEnvironment);
    return i_this;
}

// HD: sead environment/resource creation replaces the GameCube scene's UI archive setup.
/* 025AE3C0 */
void dScnName_c::createEnvironment() {
    gabi::Local<NameScene::EnvironmentConfig> config;
    EnvironmentConfig_ct(config.get());
    EnvironmentConfig_add(config.get(), *l_ambientType, 1);
    EnvironmentConfig_add(config.get(), *l_directionalType, 8);
    EnvironmentConfig_add(config.get(), *l_pointType, 1);
    EnvironmentConfig_add(config.get(), *l_fogType, 1);
    void* context = Environment_getContext();
    Environment_create(&mEnvironment, config.get(), context);
    gabi::Local<NameScene::SafeString> archiveName, fileName;
    archiveName->__vtbl = &l_safeStringVtable;
    fileName->__vtbl = &l_safeStringVtable;
    archiveName->mString = gabi::at<const char>(gabi::guest_addr("Misc"));
    fileName->mString = gabi::at<const char>(gabi::guest_addr("Misc.bfres"));
    GameInfo_loadResource(l_gameInfo, archiveName.get(), fileName.get(), 0);
    NameScene::ResourceArchive* archive = ResourceArchive_get();
    NameScene::EnvironmentResource* resource = ResourceArchive_getFile(
        resolveRelative(&archive->mEnvironmentFile), "env.baglenv");
    gabi::Local<be<u32>> result, left, right;
    be<u32>* data = Resource_getData(result.get(), resolveRelative(&resource->mData));
    *left.get() = u32(*data);
    *right.get() = u32(*data);
    Environment_setResource(&mEnvironment, left.get(), right.get(), -1, 1.0f);
    Environment_update(&mEnvironment);
    l_environment = &mEnvironment;
}

// HD: fixed 1280x720 window and sead renderer supplement the tww camera setup.
/* 025AE528 */
void dScnName_c::createScene() {
    getPlay()->mWindowNum = 1;
    NameScene::PlayView* play = getPlay();
    NameScene::Window* window = &play->mWindow;
    dDlst_window_setViewPort(window, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);
    dDlst_window_setScissor(window, 0.0f, 0.0f, 1280.0f, 720.0f);
    window->mCameraID = 0;
    window->mMode = 2;
    NameScene::camera_class* camera = &mCamera;
    getPlay()->mCamera = camera;
    play = getPlay();
    camera->view.mFovy = 60;
    camera->view.mFar = 160000;
    camera->view.mNear = 5;
    f32 aspect = f32(play->mWindow.mWidth / play->mWindow.mHeight);
    camera->view.mLookat.mUp.z = 0;
    camera->view.mLookat.mUp.x = 0;
    camera->view.mLookat.mEye.x = 9377;
    camera->view.mLookat.mCenter.z = 1000;
    camera->view.mLookat.mCenter.x = 0;
    camera->view.mLookat.mUp.y = 1;
    camera->view.mLookat.mEye.y = 0;
    camera->view.mLookat.mCenter.y = 6311;
    camera->view.mBank = 0;
    window = &play->mWindow;
    camera->view.mAspect = f32(aspect * l_aspectRatio);
    camera->view.mLookat.mEye.z = 7644;
    getPlay()->mPlayer = nullptr;
    getPlay()->mCurrentWindow = window;
    getPlay()->mViewport = window;
    getPlay()->mView = camera;
    l_autoFocus = 0;
    setView();
    dKy_setLight_init();
    enum { KYEFF = 0x1DE, KYEFF2 = 0x1DF, VRBOX = 0x1B5, VRBOX2 = 0x1B6 };
    fopKyM_Create(KYEFF, 0, 0);
    fopKyM_Create(KYEFF2, 0, 0);
    void* layer = fpcLy_CurrentLayer();
    fpcSCtRq_Request(layer, VRBOX, 0, 0, 0);
    layer = fpcLy_CurrentLayer();
    fpcSCtRq_Request(layer, VRBOX2, 0, 0, 0);
    // HD: every component assignment calls the environment accessor independently.
    NameScene_getEnvLight()->mVrSkyColor.r = 0x50;
    NameScene_getEnvLight()->mVrSkyColor.g = 0x78;
    NameScene_getEnvLight()->mVrSkyColor.b = 0xFF;
    NameScene_getEnvLight()->mVrSkyColor.a = 0xFF;
    NameScene_getEnvLight()->mVrkumoColor.r = 0xFF;
    NameScene_getEnvLight()->mVrkumoColor.g = 0xFF;
    NameScene_getEnvLight()->mVrkumoColor.b = 0xFF;
    NameScene_getEnvLight()->mVrkumoColor.a = 0xFF;
    NameScene_getEnvLight()->mVrKasumiMaeColor.r = 0xD2;
    NameScene_getEnvLight()->mVrKasumiMaeColor.g = 0xE5;
    NameScene_getEnvLight()->mVrKasumiMaeColor.b = 0xFF;
    NameScene_getEnvLight()->mVrKasumiMaeColor.a = 0xFF;
    // HD: load all nine float components before any sead camera store.
    f32 eye[3] = {camera->view.mLookat.mEye.x, camera->view.mLookat.mEye.y, camera->view.mLookat.mEye.z};
    f32 at[3] = {camera->view.mLookat.mCenter.x, camera->view.mLookat.mCenter.y, camera->view.mLookat.mCenter.z};
    f32 up[3] = {camera->view.mLookat.mUp.x, camera->view.mLookat.mUp.y, camera->view.mLookat.mUp.z};
    NameScene::Renderer* renderer = l_renderer;
    mLookAtCamera.mEye.x = eye[0];
    mLookAtCamera.mEye.y = eye[1];
    mLookAtCamera.mEye.z = eye[2];
    mLookAtCamera.mUp.x = up[0];
    mLookAtCamera.mAt.y = at[1];
    mLookAtCamera.mUp.y = up[1];
    mLookAtCamera.mUp.z = up[2];
    mLookAtCamera.mAt.z = at[2];
    mLookAtCamera.mAt.x = at[0];
    NameSceneVector_normalize(&mLookAtCamera.mUp);
    NameScene::CameraVtable* vtable = gabi::at<NameScene::CameraVtable>(mLookAtCamera.__vtbl);
    gabi::call_ptr<void>(vtable->update.get(), &mLookAtCamera, &mLookAtCamera);
    Projection_setPerspective(&mProjection, camera->view.mNear, camera->view.mFar,
                             f32(camera->view.mFovy * l_degreesToRadians), camera->view.mAspect);
    renderer->mDisplay[renderer->mDisplayCount > 1 ? 1 : 0]->mCamera = &mLookAtCamera;
    renderer->mDisplay[renderer->mDisplayCount > 1 ? 1 : 0]->mProjection = &mProjection;
    renderer->mDisplay[0]->mCamera = &mLookAtCamera;
    renderer->mDisplay[0]->mProjection = &mProjection;
    createEnvironment();
}

/* 025AE8DC */
u32 dScnName_c::create() {
    // HD: construction is guarded by a byte in the scene's extended tail.
    u8 constructed = mConstructed;
    if (!constructed) {
        if (gabi::ea(this) != 0) {
            dScnName_c__ct(this);
            constructed = mConstructed;
        }
        mConstructed = constructed + 1;
    }
    getPlay()->mEnableNextStage = 0;
    dComIfGp_setNextStage("Name", 0, 0, -1, 0, 1, 0, 0.0f);
    NameScene::Stage* nextStage = &getPlay()->mNextStage;
    NameScene::PlayView* play = getPlay();
    // HD: GHS copies the 12-byte stage record in halfwords, preserving arbitrary name bytes.
    be<u16>* source = reinterpret_cast<be<u16>*>(nextStage);
    be<u16>* destination = reinterpret_cast<be<u16>*>(&play->mStartStage);
    for (u32 i = 0; i < 6; ++i) destination[i] = u16(source[i]);
    getPlay()->mEnableNextStage = 0;
    getPlay()->field_0x62F1 = 255;
    u32 result = dComLbG_PhaseHandler(&mPhs, &l_phaseMethod, "Stage");
    if (result != cPhs_COMPLEATE_e) return result;
    NameScene::Heap* gameHeap = mDoExt_getGameHeap();
    NameScene::Heap* sceneHeap = JKRCreateExpHeap(0xB0000, gameHeap, 0);
    heap = sceneHeap;
    if (!sceneHeap) {
        // HD: retain the original assert spelling and the subsequent heap reload.
        JUT_ASSERT_fail("d_s_name.cpp", 0xF0, "heap != (0)");
        sceneHeap = heap;
    }
    // HD: the original Shift-JIS heap label is a data view, retaining its exact guest bytes.
    sceneHeap->mName = &l_heapName;
    oldHeap = mDoExt_setCurrentHeap(heap);
    createScene();
    void* menuSave = l_menuSave;
    if (menuSave) {
        l_menuSaveActive = 0;
        MenuSave_delete(menuSave);
        MenuSave_deleteSingleton();
    }
    switch (s16(mProcName)) {
    case NameScene::NAME_SCENE:
        l_saveInfo->mNewFile = 0;
        l_saveInfo->mNoFile = 0;
        NameController_create(0);
        Controller_create(l_nameInput);
        break;
    case NameScene::NAMEEX_SCENE:
        FileSelectController_create(0);
        Controller_create(l_fileSelect);
        break;
    case NameScene::FORMAT_SCENE:
        FileErrorController_create(0);
        Controller_create(l_fileError);
        break;
    }
    be<u32>* clock = NameSceneTimer_getTicks();
    u32 ticks = *clock;
    // HD: retain the unsigned GHS reciprocal multiply used by the tick-rate conversion.
    u32 tickRate = u32((u64(ticks >> 2) * 0x88888889u) >> 36);
    NameSceneDisplay_setTickRate(tickRate);
    mConstructed = 0;
    return cPhs_COMPLEATE_e;
}

/* 025AEAFC */
static u32 dScnName_Create(dScnName_c* i_this) {
    return i_this->create();
}

// HD: compiler-generated header statics and SafeString deleting destructor.
/* 025AEB00 */
static void __sinit_d_s_name_cpp() {
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[1] = 0;
    l_headerStatics.mZero[0] = 0;
    __register_global_object(0x101EA760);
    l_headerStatics.mNegativePi = l_pi.negative;
    l_headerStatics.mPositivePi = l_pi.positive;
    NameSceneFirstHeaderObject_ct(&l_headerStatics.mFirstObject);
    __register_global_object(0x101EA76C);
    NameSceneSecondHeaderObject_ct(&l_headerStatics.mSecondObject);
    __register_global_object(0x101EA778);
    l_headerStatics.mFarClip[0] = 50000;
    l_headerStatics.mNearClip[0] = 10000;
    l_headerStatics.mFarClip[1] = 50000;
    l_headerStatics.mNearClip[1] = 10000;
}

/* 025AEBC4 */
static u32 dScnName_IsDelete() {
    return 1;
}

/* 025AEBCC */
static void NameSceneSafeString_dt(void* string, u32 flags) {
    if (string && (flags & 1)) operator_delete(string);
}

/* WWHD glue (tools/verify/autoglue.py):
data l_frontend 101F7274
data l_nameInput 101F82EC
data l_fileSelect 101F82C0
data l_fileError 101F83A4
data l_resetState 101F4974
data l_saveInfo 101F84DC
data l_nameCancelled 1049EAB0
data l_nameAccepted 1049EB60
data l_fileFinished 1049E9C4
data l_formatFinished 1049F580
data l_j3dViewMtx 104B45F8
data l_renderer 101F95D0
data l_gameInfo 101F4F7C
data l_environment 104B4708
data l_cameraVtable 10052A5C
data l_seadCameraVtable 101450D0
data l_lookAtVtable 101450F8
data l_identity 104A041C
data l_headerStatics 1047B3B4
data l_pi 10052B08
data l_ambientType 104A1F64
data l_directionalType 104A1F70
data l_pointType 104A1460
data l_fogType 104A145C
data l_menuSave 101F8344
data l_menuSaveActive 101F8350
data l_autoFocus 101F4828
data l_safeStringVtable 10052A44
data l_phaseMethod 101EA73C
data l_heapName 10052AF4
data l_aspectRatio 104873D4
data l_degreesToRadians 10052AC4
str 10052A80
str 10052A94
str 10052A88
str 10052AD0
str 10052AC8 in 025AE8DC
str 10052AE4
str 10052AD8
str 10052A70 in 025ADFFC
ext 027EC220 void mDoExt_restoreCurrentHeap(NameScene::Heap*)
ext 02524180 void dComIfG_deleteStageRes(const char*)
ext 025B9864 void dSv_restart_setRestartOption(void*, u32)
ext 0270A3D0 void Controller_delete(NameScene::Controller*)
ext 0270BF24 void NameController_deleteSingleton()
ext 02709004 void FileSelectController_deleteSingleton()
ext 0271B458 void FileErrorController_deleteSingleton()
ext 027C1F70 void Renderer_setEnvironment(NameScene::Renderer*, u32)
ext 0276B21C void Environment_dt(NameScene::Environment*, u32)
ext 02738A6C void Projection_dt(NameScene::Projection*, u32)
ext 0274CCB0 void LookAtCamera_dt(NameScene::LookAtCamera*, u32)
ext 025DD630 void NameSceneBaseProcess_dt(void*, u32)
ext 025DD5F0 void NameSceneBaseProcess_ct(void*)
ext 027389F8 void Projection_ct(NameScene::Projection*)
ext 0276C1DC void Environment_ct(NameScene::Environment*)
ext 028ED6F8 void NameSceneFirstHeaderObject_ct(u8*)
ext 028EAB2C void NameSceneSecondHeaderObject_ct(u8*)
ext 0276CAA4 void EnvironmentConfig_ct(NameScene::EnvironmentConfig*)
ext 0276F9F0 void EnvironmentConfig_add(NameScene::EnvironmentConfig*, s32, u32)
ext 0203E8F4 void* Environment_getContext()
ext 0276C3F8 void Environment_create(NameScene::Environment*, NameScene::EnvironmentConfig*, void*)
ext 026124B0 void GameInfo_loadResource(NameScene::GameInfo*, NameScene::SafeString*, NameScene::SafeString*, u32)
ext 027E2DC0 NameScene::ResourceArchive* ResourceArchive_get()
ext 027DFA24 NameScene::EnvironmentResource* ResourceArchive_getFile(void*, const char*)
ext 027A7558 be<u32>* Resource_getData(be<u32>*, void*)
ext 0276B710 void Environment_setResource(NameScene::Environment*, be<u32>*, be<u32>*, s32, f32)
ext 0252CC8C void dDlst_window_setViewPort(NameScene::Window*, f32, f32, f32, f32, f32, f32)
ext 0252CCA8 void dDlst_window_setScissor(NameScene::Window*, f32, f32, f32, f32)
ext 0255DA78 void dKy_setLight_init()
ext 025DAD4C void fopKyM_Create(u32, u32, u32)
ext 025DED64 void* fpcLy_CurrentLayer()
ext 025E14A8 void fpcSCtRq_Request(void*, u32, u32, u32, u32)
ext 025155D8 void NameSceneVector_normalize(cXyz*)
ext 0274E04C void Projection_setPerspective(NameScene::Projection*, f32, f32, f32, f32)
ext 02555D0C NameScene::EnvLight* NameScene_getEnvLight()
ext 0252012C void dComIfGp_setNextStage(const char*, u32, u32, s32, u32, u32, u32, f32)
ext 02525FE4 u32 dComLbG_PhaseHandler(request_of_phase_process_class*, void*, const char*)
ext 025E2FBC NameScene::Heap* mDoExt_getGameHeap()
ext 027EBCB0 NameScene::Heap* JKRCreateExpHeap(u32, NameScene::Heap*, u32)
ext 025E3570 NameScene::Heap* mDoExt_setCurrentHeap(NameScene::Heap*)
ext 02715C10 void MenuSave_delete(void*)
ext 02717B70 void MenuSave_deleteSingleton()
ext 0270BE70 void NameController_create(u32)
ext 02708F64 void FileSelectController_create(u32)
ext 0271B3B8 void FileErrorController_create(u32)
ext 02709D24 void Controller_create(NameScene::Controller*)
ext C0009C80 be<u32>* NameSceneTimer_getTicks()
ext 025F096C void NameSceneDisplay_setTickRate(u32)
ext 025E18EC void mDoAud_bgmStart(u32)
ext 02523A04 u32 dComIfG_setStageRes(void*, u32)
ext 02523D08 s32 dComIfG_syncStageRes()
ext 026FBB8C u32 Frontend_load(void*, u32)
ext 026FBB7C u32 Frontend_isReady(void*)
ext 02031090 void Font_load()
ext 020310A4 u32 Font_isReady()
ext 028E9948 void C_MTXPerspective(NameScene::Mtx44*, f32, f32, f32, f32)
ext 025F1C90 void mDoMtx_lookAt(Mtx34*, cXyz*, cXyz*, s16)
ext 028E91EC void cMtx_inverse(Mtx34*, Mtx34*)
ext 025F1FC4 void cMtx_concatProjView(NameScene::Mtx44*, Mtx34*, NameScene::Mtx44*)
ext 025DA7AC create_tag_class* fopDwIt_Begin()
ext 025DA7D0 create_tag_class* fopDwIt_Next(create_tag_class*)
ext 025DF904 void fpcM_Draw(void*)
ext 025DBE00 u32 fopOvlpM_IsPeek()
ext 02522198 void dComIfGs_gameStart()
ext 025DC86C u32 fopScnM_ChangeReq(scene_class*, u32, u32, u32, u32)
ext 025E17CC void mDoAud_setSceneName(NameScene::Stage*, s32, s32)
ext 0270A218 void Controller_move(NameScene::Controller*)
ext 02006478 NameScene::StateDescriptor* StateMgr_getState(void*)
ext 02520230 void dComIfG_changeOpeningScene(scene_class*, u32)
ext 025202F8 void dComIfG_resetToOpening(scene_class*)
ext 0276C8B8 void Environment_update(NameScene::Environment*)
*/
