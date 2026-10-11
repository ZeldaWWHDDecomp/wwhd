/** Name scene: tww process/camera names with explicit HD render and UI views.
 * HD: camera starts at +0x1D4; sead camera/projection/environment extend the scene
 * to 0x93C. Singleton views retain the larger HD save/play offsets used here.
 */
#pragma once
#include "bindings.h"
#include "f_op/f_op_scene.h"

namespace NameScene {
struct Mtx44 { be<f32> m[4][4]; };
struct lookat_class { cXyz mEye, mCenter, mUp; };
struct view_class {
    u8 mProcessData[0xB4];
    be<u32> __vtbl;
    u8 mProcessTail[0x14];
    be<f32> mNear, mFar, mFovy, mAspect;
    lookat_class mLookat;
    be<s16> mBank;
    u8 padding[2];
    Mtx44 mProjMtx;
    Mtx34 mViewMtx, mInvViewMtx;
    Mtx44 mProjViewMtx;
    Mtx34 mViewMtxNoTrans;
};
struct camera_class { view_class view; u8 mCameraData[0x28]; };
// HD: sead camera/projection/environment objects supplement the GameCube camera.
struct LookAtCamera {
    be<u32> mMatrixWords[12];
    be<u32> __vtbl;
    cXyz mEye, mAt, mUp;
};
struct Projection { u8 data[0xC4]; };
struct Environment { u8 data[0x400]; };
struct Controller { u8 prefix[0x10]; u8 mStateMgr[0x135]; be<u8> field_0x145; };
struct StateVtable { u8 prefix[0x14]; gabi::gfn getID; };
struct StateDescriptor { u8 prefix[8]; gptr<StateVtable> __vtbl; };
struct ResetState { be<u32> mReset; };
struct Stage {
    be<char> mName[8];
    be<s16> mPoint;
    be<s8> mRoomNo, mLayer;
};
// HD: these play/save fields are in the name scene's larger singleton views.
struct Window {
    be<f32> mX, mY, mWidth, mHeight, mNear, mFar;
    be<f32> mScissorX, mScissorY, mScissorWidth, mScissorHeight;
    be<u8> mCameraID, mMode;
    u8 padding[2];
};
struct PlayView {
    u8 prefix[0x5134];
    Stage mStartStage, mNextStage;
    be<u8> mEnableNextStage;
    u8 pad_514D[0x5AC9 - 0x514D];
    be<u8> mWindowNum;
    u8 pad_5ACA[2];
    Window mWindow;
    gptr<camera_class> mCamera;
    u8 pad_5AFC[0x5B2C - 0x5AFC];
    gptr<void> mPlayer;
    u8 pad_5B30[0x5F9C - 0x5B30];
    gptr<Window> mCurrentWindow, mViewport;
    gptr<camera_class> mView;
    u8 pad_5FA8[0x62F1 - 0x5FA8];
    be<u8> field_0x62F1;
};
struct SaveView {
    u8 prefix[0x7BC];
    be<u8> field_0x7BC;
    u8 padding[0x1148 - 0x7BD];
    u8 mRestart[0x24];
    be<u32> mRestartRoomParam;
    u8 pad_1170[0x12B1 - 0x1170];
    be<u8> mNewFile, mNoFile;
};
struct EnvironmentConfig { be<u32> mWords[132]; };
struct SafeString { gptr<const char> mString; gptr<void> __vtbl; };
struct RelativeOffset { be<u32> mOffset; };
struct ResourceArchive { u8 prefix[0x4C]; RelativeOffset mEnvironmentFile; };
struct EnvironmentResource { RelativeOffset mData; };
struct EnvLight {
    u8 prefix[0xB90];
    GXColor mVrSkyColor;
    u8 padding_1[4];
    GXColor mVrkumoColor;
    u8 padding_2[4];
    GXColor mVrKasumiMaeColor;
};
struct CameraVtable { u8 prefix[0x24]; gabi::gfn update; };

struct HeapVtable { u8 prefix[0x24]; gabi::gfn destroy; };
struct Heap {
    u8 prefix[0xC];
    gptr<HeapVtable> __vtbl;
    gptr<const be<char>> mName;
};
struct RenderView {
    u8 prefix[0x48];
    gptr<LookAtCamera> mCamera;
    gptr<Projection> mProjection;
};
struct Renderer {
    u8 prefix[0x1020];
    be<u32> mDisplayCount;
    gptr<gptr<RenderView>> mDisplay;
};
struct GameInfo { u8 prefix[0x11248]; be<u32> mFadeState; };
struct HeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    be<f32> mFarClip[2], mNearClip[2];
    u8 mFirstObject, mSecondObject;
    u8 padding[2];
    be<u32> mZero[4];
};
struct PiPair { be<f32> negative, positive; };
struct MatrixWords { be<u32> m[12]; };

enum { NAME_SCENE = 9, NAMEEX_SCENE = 10, FORMAT_SCENE = 11,
       OPEN_SCENE = 12, PLAY_SCENE = 7, OVERLAP0 = 0, JA_BGM_SELECT = 0x8000001E };
}

void mDoAud_bgmStart(u32);
u32 dComIfG_setStageRes(void*, u32);
s32 dComIfG_syncStageRes();
u32 Frontend_load(void*, u32);
u32 Frontend_isReady(void*);
void Font_load();
u32 Font_isReady();
void C_MTXPerspective(NameScene::Mtx44*, f32, f32, f32, f32);
void mDoMtx_lookAt(Mtx34*, cXyz*, cXyz*, s16);
void cMtx_inverse(Mtx34*, Mtx34*);
void cMtx_concatProjView(NameScene::Mtx44*, Mtx34*, NameScene::Mtx44*);
create_tag_class* fopDwIt_Begin();
create_tag_class* fopDwIt_Next(create_tag_class*);
void fpcM_Draw(void*);
u32 fopOvlpM_IsPeek();
void dComIfGs_gameStart();
u32 fopScnM_ChangeReq(scene_class*, u32, u32, u32, u32);
void mDoAud_setSceneName(NameScene::Stage*, s32, s32);
void Controller_move(NameScene::Controller*);
NameScene::StateDescriptor* StateMgr_getState(void*);
void dComIfG_changeOpeningScene(scene_class*, u32);
void dComIfG_resetToOpening(scene_class*);
void Environment_update(NameScene::Environment*);

class dScnName_c : public scene_class {
public:
    void setView();
    u32 draw();
    void changeGameScene();
    void NameInMain();
    void FileSelectMain();
    void MemCardFormat();
    u32 execute();
    void createEnvironment();
    void createScene();
    u32 create();
    /* 0x1C8 */ request_of_phase_process_class mPhs;
    /* 0x1D0 */ gptr<NameScene::Heap> heap;
    /* 0x1D4 */ NameScene::camera_class mCamera;
    /* 0x410 */ NameScene::LookAtCamera mLookAtCamera;
    /* 0x468 */ NameScene::Projection mProjection;
    /* 0x52C */ gptr<NameScene::Heap> oldHeap;
    /* 0x530 */ u8 padding_530[4];
    /* 0x534 */ be<u8> field_0x55f; // HD: the opening-scene selector moves from GC +0x55F to +0x534.
    /* 0x535 */ u8 padding_535[3];
    /* 0x538 */ NameScene::Environment mEnvironment;
    /* 0x938 */ be<u8> mConstructed;
    /* 0x939 */ u8 padding_939[3];
};
WWHD_SIZE(NameScene::view_class, 0x214);
WWHD_SIZE(NameScene::camera_class, 0x23C);
WWHD_SIZE(NameScene::LookAtCamera, 0x58);
WWHD_SIZE(dScnName_c, 0x93C);
WWHD_OFFSET(dScnName_c, mPhs, 0x1C8);
WWHD_OFFSET(dScnName_c, heap, 0x1D0);
WWHD_OFFSET(dScnName_c, mCamera, 0x1D4);
WWHD_OFFSET(dScnName_c, mCamera.view.mNear, 0x2A0);
WWHD_OFFSET(dScnName_c, mCamera.view.mLookat, 0x2B0);
WWHD_OFFSET(dScnName_c, mCamera.view.mProjMtx, 0x2D8);
WWHD_OFFSET(dScnName_c, mCamera.view.mViewMtx, 0x318);
WWHD_OFFSET(dScnName_c, mLookAtCamera, 0x410);
WWHD_OFFSET(dScnName_c, mProjection, 0x468);
WWHD_OFFSET(dScnName_c, oldHeap, 0x52C);
WWHD_OFFSET(dScnName_c, field_0x55f, 0x534);
WWHD_OFFSET(dScnName_c, mEnvironment, 0x538);
WWHD_OFFSET(dScnName_c, mConstructed, 0x938);

WWHD_SIZE(NameScene::Window, 0x2C);
WWHD_OFFSET(NameScene::PlayView, mEnableNextStage, 0x514C);
WWHD_OFFSET(NameScene::PlayView, mWindowNum, 0x5AC9);
WWHD_OFFSET(NameScene::PlayView, mWindow, 0x5ACC);
WWHD_OFFSET(NameScene::PlayView, mCamera, 0x5AF8);
WWHD_OFFSET(NameScene::PlayView, mPlayer, 0x5B2C);
WWHD_OFFSET(NameScene::PlayView, mCurrentWindow, 0x5F9C);
WWHD_OFFSET(NameScene::PlayView, field_0x62F1, 0x62F1);
WWHD_OFFSET(NameScene::SaveView, mRestart, 0x1148);
WWHD_OFFSET(NameScene::SaveView, mRestartRoomParam, 0x116C);
WWHD_OFFSET(NameScene::SaveView, mNewFile, 0x12B1);
WWHD_SIZE(NameScene::HeaderStatics, 0x2C);
void mDoExt_restoreCurrentHeap(NameScene::Heap*);
void dComIfG_deleteStageRes(const char*);
void dSv_restart_setRestartOption(void*, u32);
void Controller_delete(NameScene::Controller*);
void NameController_deleteSingleton();
void FileSelectController_deleteSingleton();
void FileErrorController_deleteSingleton();
void Renderer_setEnvironment(NameScene::Renderer*, u32);
void Environment_dt(NameScene::Environment*, u32);
void Projection_dt(NameScene::Projection*, u32);
void LookAtCamera_dt(NameScene::LookAtCamera*, u32);
void NameSceneBaseProcess_dt(void*, u32);
void NameSceneBaseProcess_ct(void*);
void Projection_ct(NameScene::Projection*);
void Environment_ct(NameScene::Environment*);
void NameSceneFirstHeaderObject_ct(u8*);
void NameSceneSecondHeaderObject_ct(u8*);

WWHD_SIZE(NameScene::EnvironmentConfig, 0x210);
WWHD_SIZE(NameScene::SafeString, 8);
WWHD_OFFSET(NameScene::EnvLight, mVrSkyColor, 0xB90);
WWHD_OFFSET(NameScene::EnvLight, mVrkumoColor, 0xB98);
WWHD_OFFSET(NameScene::EnvLight, mVrKasumiMaeColor, 0xBA0);
void EnvironmentConfig_ct(NameScene::EnvironmentConfig*);
void EnvironmentConfig_add(NameScene::EnvironmentConfig*, s32, u32);
void* Environment_getContext();
void Environment_create(NameScene::Environment*, NameScene::EnvironmentConfig*, void*);
void GameInfo_loadResource(NameScene::GameInfo*, NameScene::SafeString*, NameScene::SafeString*, u32);
NameScene::ResourceArchive* ResourceArchive_get();
NameScene::EnvironmentResource* ResourceArchive_getFile(void*, const char*);
be<u32>* Resource_getData(be<u32>*, void*);
void Environment_setResource(NameScene::Environment*, be<u32>*, be<u32>*, s32, f32);
void dDlst_window_setViewPort(NameScene::Window*, f32, f32, f32, f32, f32, f32);
void dDlst_window_setScissor(NameScene::Window*, f32, f32, f32, f32);
void dKy_setLight_init();
void fopKyM_Create(u32, u32, u32);
void* fpcLy_CurrentLayer();
void fpcSCtRq_Request(void*, u32, u32, u32, u32);
void NameSceneVector_normalize(cXyz*);
void Projection_setPerspective(NameScene::Projection*, f32, f32, f32, f32);
NameScene::EnvLight* NameScene_getEnvLight();
void dComIfGp_setNextStage(const char*, u32, u32, s32, u32, u32, u32, f32);
u32 dComLbG_PhaseHandler(request_of_phase_process_class*, void*, const char*);
NameScene::Heap* mDoExt_getGameHeap();
NameScene::Heap* JKRCreateExpHeap(u32, NameScene::Heap*, u32);
NameScene::Heap* mDoExt_setCurrentHeap(NameScene::Heap*);
void MenuSave_delete(void*);
void MenuSave_deleteSingleton();
void NameController_create(u32);
void FileSelectController_create(u32);
void FileErrorController_create(u32);
void Controller_create(NameScene::Controller*);
be<u32>* NameSceneTimer_getTicks();
void NameSceneDisplay_setTickRate(u32);

WWHD_SIZE(NameScene::Mtx44, 0x40);
WWHD_SIZE(NameScene::lookat_class, 0x24);
WWHD_SIZE(NameScene::Projection, 0xC4);
WWHD_SIZE(NameScene::Environment, 0x400);
WWHD_SIZE(NameScene::Controller, 0x146);
WWHD_SIZE(NameScene::StateVtable, 0x18);
WWHD_SIZE(NameScene::StateDescriptor, 0xC);
WWHD_SIZE(NameScene::Stage, 0xC);
WWHD_SIZE(NameScene::Heap, 0x14);
WWHD_SIZE(NameScene::RenderView, 0x50);
WWHD_SIZE(NameScene::Renderer, 0x1028);
WWHD_SIZE(NameScene::MatrixWords, 0x30);
WWHD_SIZE(NameScene::CameraVtable, 0x28);
WWHD_OFFSET(NameScene::LookAtCamera, mEye, 0x34);
WWHD_OFFSET(NameScene::LookAtCamera, mAt, 0x40);
WWHD_OFFSET(NameScene::LookAtCamera, mUp, 0x4C);
