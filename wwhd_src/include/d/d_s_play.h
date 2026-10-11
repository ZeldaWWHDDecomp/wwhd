/** Play scene and debug HIO objects.
 * HD: the scene owns native rendering/wind systems. Message HIO drops the host-I/O number;
 * dark HIO moves its vtable to the end. The other retail field names follow tww. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_scene.h"
#include "gc_api/tu.h"

// HD: kept in the unit namespace because the camera header exposes a pause-only adapter.
namespace dScnPly {
struct SceneEntry {
    be<u16> mObjectIndex;
    be<u16> mIsValid;
};
WWHD_SIZE(SceneEntry, 4);
struct SceneSystem {
    u8 padding[8];
    be<u32> mEntryCount;
    gptr<SceneEntry> mpEntries;
    be<u32> mObjectCount;
    gptr<gptr<void>> mpObjects;
    u8 paddingObjects[0x400 - 0x18];
};
WWHD_OFFSET(SceneSystem, mEntryCount, 8);
WWHD_OFFSET(SceneSystem, mpEntries, 0xC);
WWHD_OFFSET(SceneSystem, mObjectCount, 0x10);
WWHD_OFFSET(SceneSystem, mpObjects, 0x14);
WWHD_SIZE(SceneSystem, 0x400);
struct WindSystem {
    u8 paddingBinding[0x1CC];
    // HD: native model bindings retain word copies, including float-shaped bit patterns.
    be<u32> mBindingWords[57];
    u8 paddingStatePrefix[0x328 - 0x2B0];
    be<u32> mStateWords[3];
    u8 paddingState[0x930 - 0x5D4 - 0x334];
    be<u8> mFlags;
    u8 paddingFlags[7];
    u8 mWindParameters[0x9F0 - 0x938];
    be<f32> mSpeedX;
    be<f32> mSpeedZ;
    u8 paddingSpeed[0xA04 - 0x9F8];
    be<f32> mWindX;
    be<f32> mWindZ;
    u8 paddingWind[0xA80 - 0xA0C];
    be<u8> mIsEnabled;
    u8 paddingEnabled[0xA90 - 0xA81];
};
WWHD_OFFSET(WindSystem, mFlags, 0x930 - 0x5D4);
WWHD_OFFSET(WindSystem, mWindX, 0xA04 - 0x5D4);
WWHD_OFFSET(WindSystem, mWindZ, 0xA08 - 0x5D4);
WWHD_SIZE(WindSystem, 0xA90 - 0x5D4);
struct WindModel {
    be<u32> mVtable;
    be<u32> mBindingWords[35];
};
WWHD_SIZE(WindModel, 0x90);
struct dScnPly_ply_c : scene_class {
    request_of_phase_process_class mLoadPhs;
    gptr<void> sceneCommand;
    SceneSystem mSceneSystem;
    WindSystem mWindSystem;
    u8 mWindController[0x40];
    WindModel mWindModel;
    be<u8> mIsOcean;
    u8 paddingMode[3];
    be<u32> mMode;
    s8 calcPauseTimer();
};
WWHD_OFFSET(dScnPly_ply_c, mLoadPhs, 0x1C8);
WWHD_OFFSET(dScnPly_ply_c, sceneCommand, 0x1D0);
WWHD_OFFSET(dScnPly_ply_c, mSceneSystem, 0x1D4);
WWHD_OFFSET(dScnPly_ply_c, mWindSystem, 0x5D4);
WWHD_OFFSET(dScnPly_ply_c, mWindController, 0xA90);
WWHD_OFFSET(dScnPly_ply_c, mWindModel, 0xAD0);
WWHD_OFFSET(dScnPly_ply_c, mIsOcean, 0xB60);
WWHD_OFFSET(dScnPly_ply_c, mMode, 0xB64);
WWHD_SIZE(dScnPly_ply_c, 0xB68);
} // namespace dScnPly
struct dScnPly_msg_HIO_c {
    be<u32> mVtable;
    be<u8> mIsUpdate;
    be<u8> field_0x06;
    be<u8> field_0x07;
    be<u8> field_0x08;
    be<u8> field_0x09;
    u8 padding[1];
    be<s16> mGroup;
    be<s16> mID;
    u8 paddingID[2];
    be<u32> field_0x10;
    // HD: argument widths remain full registers instead of retail u8/s16.
    void dScnPly_msg_HIO_checkUpdate(u32);
    void dScnPly_msg_HIO_numUpdate(s32, s32);
    void dScnPly_msg_HIO_setUpdate(u32);
    void dScnPly_msg_HIO_padCheck();
    void dScnPly_msg_HIO_messageProc();
};
WWHD_OFFSET(dScnPly_msg_HIO_c, mIsUpdate, 4);
WWHD_OFFSET(dScnPly_msg_HIO_c, field_0x06, 5);
WWHD_OFFSET(dScnPly_msg_HIO_c, field_0x07, 6);
WWHD_OFFSET(dScnPly_msg_HIO_c, mGroup, 0xA);
WWHD_OFFSET(dScnPly_msg_HIO_c, mID, 0xC);
WWHD_OFFSET(dScnPly_msg_HIO_c, field_0x10, 0x10);
WWHD_SIZE(dScnPly_msg_HIO_c, 0x14);
struct dScnPly_reg_childHIO_c {
    be<u32> mVtable;
    be<f32> mFloatRegs[30];
    be<s16> mShortRegs[10];
};
WWHD_OFFSET(dScnPly_reg_childHIO_c, mFloatRegs, 4);
WWHD_OFFSET(dScnPly_reg_childHIO_c, mShortRegs, 0x7C);
WWHD_SIZE(dScnPly_reg_childHIO_c, 0x90);
struct dScnPly_dark_HIO_c {
    be<s8> mNo;
    be<u8> field_0x05;
    be<u8> field_0x06;
    u8 padding[1];
    be<u32> mDarkStatus[8];
    be<u32> mVtable;
};
WWHD_OFFSET(dScnPly_dark_HIO_c, mDarkStatus, 4);
WWHD_OFFSET(dScnPly_dark_HIO_c, mVtable, 0x24);
WWHD_SIZE(dScnPly_dark_HIO_c, 0x28);
struct dScnPly_env_otherHIO_c {
    be<u32> mVtable;
    be<u8> field_0x05, field_0x06, field_0x07, field_0x08, field_0x09, field_0x0a;
    be<u8> field_0x0b, field_0x0c, field_0x0d, field_0x0e, field_0x0f, field_0x10;
    be<u8> field_0x11, field_0x12;
    u8 padding[2];
    be<u8> field_0x15, field_0x16;
    u8 paddingArray[2];
    be<u16> field_0x18[20];
    be<s16> field_0x40;
    u8 paddingFloat[2];
    be<f32> field_0x44;
};
WWHD_OFFSET(dScnPly_env_otherHIO_c, field_0x05, 4);
WWHD_OFFSET(dScnPly_env_otherHIO_c, field_0x15, 0x14);
WWHD_OFFSET(dScnPly_env_otherHIO_c, field_0x18, 0x18);
WWHD_OFFSET(dScnPly_env_otherHIO_c, field_0x40, 0x40);
WWHD_OFFSET(dScnPly_env_otherHIO_c, field_0x44, 0x44);
WWHD_SIZE(dScnPly_env_otherHIO_c, 0x48);
struct dScnPly_env_debugHIO_c {
    be<u32> mVtable;
    be<f32> field_0x08, field_0x0c, field_0x10, field_0x14, field_0x18;
    be<f32> field_0x1c, field_0x20, field_0x24, field_0x28, field_0x2c;
};
WWHD_OFFSET(dScnPly_env_debugHIO_c, field_0x08, 4);
WWHD_OFFSET(dScnPly_env_debugHIO_c, field_0x2c, 0x28);
WWHD_SIZE(dScnPly_env_debugHIO_c, 0x2C);

void dScnPly_JUT_ASSERT(u32, s32, u32);
BOOL dScnPly_dMenu_flag();
u32 dScnPly_getSystemState(u32);
BOOL dScnPly_queryPauseRequest();
BOOL dScnPly_queryPrimaryPause(u32);
BOOL dScnPly_querySecondaryPause(u32);
BOOL dScnPly_pad_trigZ(s32);
BOOL dScnPly_pad_trigUp(s32);
BOOL dScnPly_pad_trigDown(s32);
BOOL dScnPly_pad_trigRight(s32);
BOOL dScnPly_pad_trigLeft(s32);
BOOL dScnPly_pad_trigB(s32);
BOOL dScnPly_pad_holdA(s32);
BOOL dScnPly_pad_holdX(s32);
BOOL dScnPly_pad_holdY(s32);
u32 dScnPly_fopMsgM_messageSet(u32, u32, u32);
u32 dScnPly_fopMsgM_getStatus(u32);
void dScnPly_fopMsgM_setStatus(u32, u32);
s8 dScnPly_cLib_calcTimer_s8(u32);
BOOL dScnPly_fopOvlpM_IsPeek();
void dScnPly_mDoAud_sceneBgmStart();
void dScnPly_mDoAud_load2ndDynamicWave();
void dScnPly_dKy_itudemo_se();
u32 dScnPly_dKyw_get_wind_vec();
f32 dScnPly_dKyw_get_wind_pow();
void dScnPly_dAttention_Run(u32, s32);
BOOL dScnPly_mDoAud_load1stDynamicWave();
u32 dScnPly_operator_new(u32);
void dScnPly_updateTitleSystem(u32);
void dScnPly_updateGameSystem(u32);
void dScnPly_updateEndingSystem(u32);
void dScnPly_updateSeaEndingSystem(u32);
void dScnPly_updateSceneSystem(u32);
void dScnPly_updateWindSystem(u32, f32);
void dScnPly_updateWindController(u32, u32, u32);
void dScnPly_updateDemo();
void dScnPly_updatePlay(u32);
void dScnPly_processDetect(u32);
void dScnPly_constructSceneBase(u32);
void dScnPly_constructSceneSystem(u32);
void dScnPly_constructWindSystem(u32);
void dScnPly_constructWindModel(u32);
struct dScnPly_TimerData {
    be<s8> pauseTimer, nextPauseTimer;
};
WWHD_SIZE(dScnPly_TimerData, 2);
struct dScnPly_HeapData {
    be<u32> mpHeap;
};
struct dScnPly_PreLoadData {
    be<s8> preLoadNo;
    be<u8> doPreLoad;
};
struct dScnPly_RegData {
    be<u32> mVtable;
    dScnPly_reg_childHIO_c mChildren[33];
};
struct dScnPly_EnvData {
    be<u32> mVtable;
    dScnPly_env_otherHIO_c mOther;
    dScnPly_env_debugHIO_c mEnvDebug;
};
WWHD_OFFSET(dScnPly_EnvData, mEnvDebug, 0x4C);
WWHD_SIZE(dScnPly_EnvData, 0x78);
extern dScnPly_TimerData &l_timers;
extern dScnPly_HeapData &l_oceanHeap;
extern dScnPly_PreLoadData &l_preLoad;
extern dScnPly_dark_HIO_c &g_darkHIO;
extern dScnPly_msg_HIO_c &g_msgDHIO;
extern dScnPly_RegData &g_regHIO;
extern dScnPly_EnvData &g_envHIO;
struct dScnPly_TuData {
    u32 mRegisterVtable, mDarkVtable, mEnvOtherVtable, mEnvDebugVtable;
    u32 mMessageVtable, mRegVtable, mEnvVtable, mPreLoadVtable;
    u32 mSceneTypeVtable, mFixedStringBaseVtable, mBufferedStringVtable;
    u32 mFixedString8Vtable, mFixedString32Vtable;
    u32 mStaticDestructors[3];
};
extern const dScnPly_TuData play_tu;

// HD: unit-private full-register interfaces preserve the verified call sites.
u32 dScnPlyAPI_getSystemState(u32);
void dScnPlyAPI_prepareDisplayHeap(u32);
void dScnPlyAPI_releaseDisplayHeap(u32);
void dScnPlyAPI_attachDisplayHeap(u32, u32);
void dScnPlyAPI_releaseDisplayResources(u32);
u32 dScnPlyAPI_getDisplayHeap(u32);
void dScnPlyAPI_initializeDisplayResources(u32);
u32 dScnPlyAPI_getCurrentHeap_u320();
u32 dScnPlyAPI_getCurrentHeap_u321(u32);
u32 dScnPlyAPI_getDisplayAllocator();
void dScnPlyAPI_resetTribox();
BOOL dScnPlyAPI_checkGameOverStart(u32);
u32 dScnPlyAPI_initializeSea(u32);
void dScnPlyAPI_initializeAttention(u32, u32, u32);
void dScnPlyAPI_destroyAttention(u32, u32);
void dScnPlyAPI_drawAttention(u32);
void dScnPlyAPI_clearBackgroundMoveFlags(u32);
void dScnPlyAPI_prepareMassCollision(u32);
u32 dScnPlyAPI_constructCollision(u32);
u32 dScnPlyAPI_destroyCollision(u32);
void dScnPlyAPI_moveCollision(u32);
void dScnPlyAPI_drawCollision(u32);
void dScnPlyAPI_clearCollision(u32);
void dScnPlyAPI_initializePlay(u32);
BOOL dScnPlyAPI_resetToOpening(u32);
s32 dScnPlyAPI_loadResource(u32, u32);
void dScnPlyAPI_deleteResource(u32, u32);
void dScnPlyAPI_setSelectEquip(u32, u32);
void dScnPlyAPI_savePlayerRecollection();
void dScnPlyAPI_restorePlayerRecollection();
s32 dScnPlyAPI_setStageResource(u32, u32);
s32 dScnPlyAPI_syncStageResource(u32);
void dScnPlyAPI_removeMagma(u32);
void dScnPlyAPI_calculateMagma(u32);
void dScnPlyAPI_drawMagma(u32);
void dScnPlyAPI_removeGrass(u32);
void dScnPlyAPI_calculateGrass(u32);
void dScnPlyAPI_drawGrass(u32);
void dScnPlyAPI_removeTree(u32);
void dScnPlyAPI_calculateTree(u32);
void dScnPlyAPI_drawTree(u32);
void dScnPlyAPI_removeWood(u32);
void dScnPlyAPI_calculateWood(u32);
void dScnPlyAPI_drawWood(u32);
void dScnPlyAPI_removeFlower(u32);
void dScnPlyAPI_calculateFlower(u32);
void dScnPlyAPI_drawFlower(u32);
s32 dScnPlyAPI_phaseHandler(u32, u32, u32);
void dScnPlyAPI_initializeSteamTags();
void dScnPlyAPI_initializePlayActors();
void dScnPlyAPI_enableTitleKirakiraSound();
u32 dScnPlyAPI_createDemo(u32);
u32 dScnPlyAPI_removeDemo(u32);
void dScnPlyAPI_setViewport(u32, f32, f32, f32, f32, f32, f32);
void dScnPlyAPI_setScissor(u32, f32, f32, f32, f32);
void dScnPlyAPI_removeEvent(u32);
BOOL dScnPlyAPI_checkEventNightStop();
s32 dScnPlyAPI_getDaytimeHour();
BOOL dScnPlyAPI_checkPoisonCollision();
void dScnPlyAPI_calculatePoisonCollision();
void dScnPlyAPI_createEnvironment(u32);
void dScnPlyAPI_deleteEnvironment(u32);
BOOL dScnPlyAPI_checkMenu();
void dScnPlyAPI_drawParticleModels(u32);
void dScnPlyAPI_createParticleScene(u32, u32);
u32 dScnPlyAPI_removeParticleScene(u32);
void dScnPlyAPI_drawParticleDirect(u32);
void dScnPlyAPI_calculateParticles(u32);
void dScnPlyAPI_drawParticleIndirect(u32);
BOOL dScnPlyAPI_checkPause();
void dScnPlyAPI_processMessageHIO(u32);
void dScnPlyAPI_destroyOceanHeap();
BOOL dScnPlyAPI_checkHeapSize();
void dScnPlyAPI_constructPlayScene(u32);
void dScnPlyAPI_formatLinkArchive(u32, u32, u32);
void dScnPlyAPI_assureSafeString(u32);
BOOL dScnPlyAPI_checkItemRecord(u32, u32, u32);
void dScnPlyAPI_clearCollect(u32, u32, u32);
BOOL dScnPlyAPI_checkEventBit(u32, u32);
void dScnPlyAPI_initializeSave(u32);
void dScnPlyAPI_createSnapshot(u32);
void dScnPlyAPI_executeSnapshot();
void dScnPlyAPI_drawSnapshot(u32);
u32 dScnPlyAPI_deleteSnapshot(u32);
void dScnPlyAPI_createStageInfo();
void dScnPlyAPI_createStage();
void dScnPlyAPI_deleteStage(u32);
void dScnPlyAPI_checkRoomDrawArea(u32);
void dScnPlyAPI_pauseVibration(u32);
void dScnPlyAPI_runVibration(u32);
void dScnPlyAPI_initializeVibration(u32);
void dScnPlyAPI_destroyVibration(u32, u32);
u32 dScnPlyAPI_beginDrawQueue();
u32 dScnPlyAPI_nextDrawQueue(u32);
u32 dScnPlyAPI_createMessageHeap(u32);
void dScnPlyAPI_destroyMessageHeap(u32);
BOOL dScnPlyAPI_checkOverlap();
void dScnPlyAPI_requestSceneChange(u32, u32, u32, u32, u32);
void dScnPlyAPI_destroySceneBase(u32, u32);
void dScnPlyAPI_drawProcess(u32);
void dScnPlyAPI_deleteProcess(u32);
void dScnPlyAPI_setAudioSceneName(u32, u32, u32);
s32 dScnPlyAPI_checkFirstDynamicWave();
s32 dScnPlyAPI_checkAllWaveLoadStatus();
void dScnPlyAPI_initializeMonsterSound();
void dScnPlyAPI_stopBgm(u32);
void dScnPlyAPI_recoverAudioReset();
BOOL dScnPlyAPI_checkStreamHeap();
u32 dScnPlyAPI_getGameHeap();
s32 dScnPlyAPI_getSafeGameHeapSize();
u32 dScnPlyAPI_getArchiveHeap();
u32 dScnPlyAPI_getZeldaHeap();
s32 dScnPlyAPI_getSafeZeldaHeapSize();
u32 dScnPlyAPI_getCommandHeap();
s32 dScnPlyAPI_getSafeCommandHeapSize();
s32 dScnPlyAPI_getSafeArchiveHeapSize();
void dScnPlyAPI_setWipeColor(u32);
void dScnPlyAPI_enableMonotone();
void dScnPlyAPI_disableMonotone();
void dScnPlyAPI_setTickRate(u32);
u32 dScnPlyAPI_createHIOChild(u32, u32);
void dScnPlyAPI_deleteHIOChild(u32);
u32 dScnPlyAPI_getModelResource(u32, u32);
u32 dScnPlyAPI_getResource_u324(u32, u32, u32, u32);
void dScnPlyAPI_getResource_void4(u32, u32, u32, u32);
void dScnPlyAPI_startResourceSystem(u32);
s32 dScnPlyAPI_checkSystemLoad(u32);
s32 dScnPlyAPI_loadSystem(u32, u32);
void dScnPlyAPI_prepareEndingSystem(u32);
void dScnPlyAPI_releaseEndingResources(u32);
void dScnPlyAPI_initializeSeaEndingSystem(u32);
u32 dScnPlyAPI_finalizeSeaEndingSystem(u32);
void dScnPlyAPI_createSeaEndingSystem(u32);
void dScnPlyAPI_destroySeaEndingSystem(u32);
void dScnPlyAPI_initializeGameSystem(u32);
u32 dScnPlyAPI_finalizeGameSystem(u32);
void dScnPlyAPI_createGameSystem(u32);
void dScnPlyAPI_destroyGameSystem(u32);
void dScnPlyAPI_initializeEndingSystem(u32);
u32 dScnPlyAPI_finalizeEndingSystem(u32);
void dScnPlyAPI_drawEndingSystem(u32);
void dScnPlyAPI_createEndingSystem(u32);
void dScnPlyAPI_destroyEndingSystem(u32);
void dScnPlyAPI_initializeTitleSystem(u32);
u32 dScnPlyAPI_finalizeTitleSystem(u32);
void dScnPlyAPI_createTitleSystem(u32);
void dScnPlyAPI_destroyTitleSystem(u32);
void dScnPlyAPI_resetControllerSystem(u32);
void dScnPlyAPI_showAssert(u32, u32, u32);
u32 dScnPlyAPI_allocate(u32);
void dScnPlyAPI_deallocate(u32);
void dScnPlyAPI_beginResourceAccess(u32);
void dScnPlyAPI_endResourceAccess(u32);
u32 dScnPlyAPI_getUnitHeapOverhead(u32);
u32 dScnPlyAPI_createOceanUnitHeap(u32, u32, u32, u32, u32, u32);
void dScnPlyAPI_formatStringV(u32, u32, u32);
void dScnPlyAPI_formatParticleName(u32, u32, u32);
void dScnPlyAPI_destroySceneSystem(u32, u32);
void dScnPlyAPI_bindSceneModel(u32, u32, u32, u32, f32);
void dScnPlyAPI_initializeSceneSystem(u32, u32, u32);
void dScnPlyAPI_updateSceneSystem(u32);
void dScnPlyAPI_drawSceneSystem(u32, u32, u32, u32);
void dScnPlyAPI_constructSceneDescription(u32);
void dScnPlyAPI_appendSceneDescription(u32, u32, u32);
void dScnPlyAPI_enableSceneObject(u32, u32);
void dScnPlyAPI_initializeWindModel(u32, u32);
void dScnPlyAPI_destroyWindSystem(u32, u32);
u32 dScnPlyAPI_resolveSceneModel(u32, u32);
void dScnPlyAPI_bindWindModel(u32, u32);
s32 dScnPlyAPI_findResourceIndex(u32, u32);
u32 dScnPlyAPI_findResource(u32, u32);
u32 dScnPlyAPI_getResourceFile();
s32 dScnPlyAPI_getTotalFreeHeapSize(u32);
void dScnPlyAPI_reportStage(u32, u32, u32);
void dScnPlyAPI_constructHeaderStaticByte(u32);
void dScnPlyAPI_constructHeaderStaticFlag(u32);
void dScnPlyAPI_constructArray(u32, u32, u32, u32);
void dScnPlyAPI_registerGlobalObject(u32);
void dScnPlyAPI_blockMove(u32, u32, u32, u32);
u32 dScnPlyAPI_getSystemInfo();
u32 dScnPlyAPI_getTimeHigh();
struct dScnPly_DrawList {
    u8 padding[0x26C];
    be<u32> mpWindow, mpViewPort, mpView;
    u8 paddingTail[0x5C0 - 0x278];
};
WWHD_OFFSET(dScnPly_DrawList, mpWindow, 0x26C);
WWHD_SIZE(dScnPly_DrawList, 0x5C0);
struct dScnPly_GameInfo {
    dSv_info_c save;
    dComIfG_play_c play;
    dScnPly_DrawList drawlist;
    u8 paddingBrightness[1];
    be<u8> mBrightness;
};
WWHD_OFFSET(dScnPly_GameInfo, mBrightness, 0x62F1);
struct dScnPly_ArchiveInfo {
    u8 padding[0x5A50];
    gptr<be<u8>> mpArchiveBuffer;
    be<u32> mArchiveVtable;
    be<s32> mArchiveCapacity;
};
WWHD_OFFSET(dScnPly_ArchiveInfo, mpArchiveBuffer, 0x5A50);
WWHD_OFFSET(dScnPly_ArchiveInfo, mArchiveCapacity, 0x5A58);
inline dScnPly_GameInfo *dScnPly_gameInfo() {
    return reinterpret_cast<dScnPly_GameInfo *>(dComIfGp_get());
}
inline dScnPly_ArchiveInfo *dScnPly_archiveInfo(u32 game) {
    return gabi::at<dScnPly_ArchiveInfo>(game);
}
// HD: native string and scene-description stack objects keep their exact verified storage.
struct dScnPly_SafeString {
    be<u32> mString, mVtable;
};
struct dScnPly_FixedString8 {
    be<u32> mString, mVtable, mCapacity;
    be<u8> mBuffer[8];
};
struct dScnPly_FixedString32 {
    be<u32> mString, mVtable, mCapacity;
    be<u8> mBuffer[32];
};
struct dScnPly_SceneDescription {
    u8 mStorage[0x214];
};
struct dScnPly_ModelHandle {
    be<u32> mValue;
};
struct dScnPly_ModelPair {
    be<u32> mFirst, mSecond;
};
struct dScnPly_ViewMatrix {
    be<f32> m[12];
};
WWHD_SIZE(dScnPly_SafeString, 8);
WWHD_SIZE(dScnPly_FixedString8, 0x14);
WWHD_SIZE(dScnPly_FixedString32, 0x2C);
WWHD_SIZE(dScnPly_SceneDescription, 0x214);
WWHD_SIZE(dScnPly_ModelHandle, 4);
WWHD_SIZE(dScnPly_ModelPair, 8);
WWHD_SIZE(dScnPly_ViewMatrix, 0x30);
struct dScnPly_SceneObjectVtable {
    u8 padding[0x44];
    be<u32> checkDerivedRuntimeTypeInfo;
};
struct dScnPly_SceneObject {
    u8 paddingType[0x58];
    gptr<dScnPly_SceneObjectVtable> mRuntimeTypeVtable;
    u8 paddingEnabled[0xF4 - 0x5C];
    be<u32> mInitializationFlags;
    u8 paddingRender[0x104 - 0xF8];
    // HD: the native scene's render-parameter block has no retail counterpart.
    be<f32> mRenderParameters[35];
    u8 paddingWind[0x200 - 0x190];
    u8 mWindInput[1];
};
WWHD_OFFSET(dScnPly_SceneObject, mRuntimeTypeVtable, 0x58);
WWHD_OFFSET(dScnPly_SceneObject, mInitializationFlags, 0xF4);
WWHD_OFFSET(dScnPly_SceneObject, mRenderParameters, 0x104);
WWHD_OFFSET(dScnPly_SceneObject, mWindInput, 0x200);
struct dScnPly_RuntimeType {
    be<u32> mVtable;
};
struct dScnPly_RuntimeTypeGuard {
    be<u32> mInitialized;
};
extern dScnPly_RuntimeType &l_sceneType;
extern dScnPly_RuntimeTypeGuard &l_sceneTypeGuard;
struct dScnPly_StateVtable {
    u8 padding[0x14];
    be<u32> queryState;
};
struct dScnPly_SystemState {
    u8 padding[8];
    gptr<dScnPly_StateVtable> mVtable;
};
WWHD_OFFSET(dScnPly_SystemState, mVtable, 8);
extern dScnPly_SystemState &l_firstPauseState;
extern dScnPly_SystemState &l_secondPauseState;
struct dScnPly_PreLoadInfo {
    gptr<const char> stageName;
    gptr<gptr<const char>> resName;
    be<u8> resNum;
    u8 padding[3];
};
WWHD_OFFSET(dScnPly_PreLoadInfo, resName, 4);
WWHD_OFFSET(dScnPly_PreLoadInfo, resNum, 8);
WWHD_SIZE(dScnPly_PreLoadInfo, 12);
struct dScnPly_PreLoadTable {
    dScnPly_PreLoadInfo mEntries[16];
};
struct dScnPly_ResourcePhases {
    request_of_phase_process_class mPhases[35];
};
struct dScnPly_PhaseTable {
    be<u32> mFunctions[11];
};
extern dScnPly_PreLoadTable &PreLoadInfoT;
extern dScnPly_ResourcePhases &resPhase;
extern dScnPly_PhaseTable &l_method;

struct dScnPly_Word {
    be<u32> mValue;
};
struct dScnPly_Byte {
    be<u8> mValue;
};
struct dScnPly_Half {
    be<u16> mValue;
};
extern dScnPly_Word &l_gameSystem;
extern dScnPly_Word &l_endingSystem;
extern dScnPly_Word &l_titleSystem;
extern dScnPly_Word &l_seaEndingSystem;
extern dScnPly_Word &l_resourceManager;
extern dScnPly_Word &l_resourceSystem;
extern dScnPly_Word &l_resourceLoader;
extern dScnPly_Word &l_resourceAccess;
extern dScnPly_Word &l_sceneRenderer;
extern dScnPly_Word &l_environment;
extern dScnPly_Word &l_controllerSystem;
extern dScnPly_Word &l_reset;
extern dScnPly_Word &l_save;
extern dScnPly_Word &l_displaySystem;
extern dScnPly_Byte &l_displayReady;
extern dScnPly_Byte &l_keepGameSystem;
extern dScnPly_Byte &l_bgmSet;
extern dScnPly_Byte &l_wipeEnabled;
extern dScnPly_Byte &l_canonGameResult;
extern dScnPly_Byte &l_graphicsResetState;
extern dScnPly_Byte &l_fadeEnabled;
extern dScnPly_Half &l_monotoneRate;
extern dScnPly_Half &l_monotoneRateSpeed;
extern dScnPly_Word &l_activeSceneSystem;
extern dScnPly_Word &l_sceneIndex;
extern dScnPly_Word &l_sceneDescriptionIndex;
extern dScnPly_Word &l_sceneBaseIndex;
extern dScnPly_Word &l_sceneResourceIndex;
extern dScnPly_Word &l_messageManager;
extern dScnPly_Word &l_particleFrame;
extern dScnPly_Word &l_roomProcessID;
extern dScnPly_Word &l_ykgrEmitter;
extern dScnPly_Word &l_salvageTagData;
extern dScnPly_Word &l_salvageID;
extern dScnPly_Byte &l_iballState;
extern dScnPly_Byte &l_ykgrAlphaFlag;
extern dScnPly_Byte &l_ykgrAlpha;
extern dScnPly_Byte &l_salvageRoomNo;
extern dScnPly_Byte &l_nowDaizaNum;
extern dScnPly_Byte &l_nowItemNum;
extern dScnPly_Word &l_preLoadTimeHigh;
extern dScnPly_Word &l_preLoadTimeLow;
extern dScnPly_Word &l_preLoadHIO;
struct dScnPly_StateContainer {
    u8 padding[0x10];
    u8 mState[1];
};
struct dScnPly_HeapVtable {
    u8 paddingDestroy[0x24];
    be<u32> destroy;
    u8 paddingFreeSize[0x74 - 0x28];
    be<u32> getFreeSize;
};
struct dScnPly_HeapObject {
    u8 padding[0xC];
    gptr<dScnPly_HeapVtable> mVtable;
};
WWHD_OFFSET(dScnPly_HeapObject, mVtable, 0xC);
struct dScnPly_StringVtable {
    u8 padding[0x14];
    be<u32> assureTermination;
};
struct dScnPly_ResourceFile {
    u8 paddingModel[0x24];
    be<u32> mModelDictionaryOffset;
    u8 paddingFiles[0x4C - 0x28];
    be<u32> mFileDictionaryOffset;
};
WWHD_OFFSET(dScnPly_ResourceFile, mModelDictionaryOffset, 0x24);
WWHD_OFFSET(dScnPly_ResourceFile, mFileDictionaryOffset, 0x4C);
struct dScnPly_ResourceHandle {
    u8 padding[0x14];
    gptr<dScnPly_ResourceFile> mpFile;
};
struct dScnPly_DictionaryEntry {
    be<u32> mResourceOffset;
    u8 padding[12];
};
struct dScnPly_Dictionary {
    u8 padding[0x24];
    dScnPly_DictionaryEntry mEntries[1];
};
WWHD_SIZE(dScnPly_DictionaryEntry, 16);
WWHD_OFFSET(dScnPly_Dictionary, mEntries, 0x24);
namespace dScnPly {
enum SceneMode { MODE_TITLE = 0, MODE_GAME = 1, MODE_ENDING = 2, MODE_OCEAN_ENDING = 3 };
enum ProcessName { fpcNm_PLAY_SCENE_e = 7, fpcNm_OPENING2_SCENE_e = 8 };
enum ItemNo {
    dItemNo_BOW_e = 0x27,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_LIGHT_ARROW_e = 0x36,
    dItemNo_NONE_e = 0xFF
};
} // namespace dScnPly
struct dScnPly_DisplayHeapState {
    u8 padding[0x80];
    be<u32> mState;
};
struct dScnPly_DisplaySystem {
    u8 padding[0x18];
    be<u32> mState;
    gptr<dScnPly_DisplayHeapState> mpHeapState;
};
struct dScnPly_ResourceManager {
    u8 padding[0x212C];
    be<u8> mIsStarted;
};
struct dScnPly_SceneCommand {
    u8 padding[0xC];
    be<u8> mIsComplete;
};
struct dScnPly_RenderView {
    u8 padding[0x6BCC];
    be<u32> mWindSystem, mWindController;
};
struct dScnPly_Renderer {
    u8 padding[0x1020];
    be<u32> mViewCount;
    gptr<gptr<dScnPly_RenderView>> mpViews;
};
struct dScnPly_Environment {
    u8 padding[0x168A];
    be<u8> mOceanShadowEnabled;
};
struct dScnPly_StageInfo {
    u8 padding[0xA];
    be<u16> mParticleFlags;
    be<u32> mStageFlags;
};
struct dScnPly_StageVtable {
    u8 padding[0x15C];
    be<u32> getStagInfo;
};
struct dScnPly_StageData {
    gptr<dScnPly_StageVtable> mVtable;
};
struct dScnPly_BackgroundVtable {
    u8 padding[0x14];
    be<u32> construct;
    u8 paddingDestroy[4];
    be<u32> destroy;
    u8 paddingMove[4];
    be<u32> move;
};
struct dScnPly_Background {
    u8 padding[0x1400];
    gptr<dScnPly_BackgroundVtable> mVtable;
};
struct dScnPly_Attention {
    be<u32> mOwner;
    u8 padding[8];
    be<u32> mParameter;
};
struct dScnPly_Particle {
    u8 padding[0x130];
    be<u32> mpModelControl;
};
struct dScnPly_DrawTag {
    u8 padding[0xC];
    be<u32> mData;
};
struct dScnPly_Window {
    u8 padding[0x28];
    be<u8> mCameraID, mMode;
};
struct dScnPly_GameHeapInfo {
    u8 padding[0x5C24];
    be<u32> mExpHeap2D;
};
struct dScnPly_Reset {
    be<u32> mReset, mResetPrepare;
};
struct dScnPly_SaveRoot {
    u8 padding[0x20];
    dSv_info_c mSave;
};
WWHD_OFFSET(dScnPly_DisplaySystem, mState, 0x18);
WWHD_OFFSET(dScnPly_DisplaySystem, mpHeapState, 0x1C);
WWHD_OFFSET(dScnPly_Renderer, mViewCount, 0x1020);
WWHD_OFFSET(dScnPly_Renderer, mpViews, 0x1024);
WWHD_OFFSET(dScnPly_RenderView, mWindSystem, 0x6BCC);
WWHD_OFFSET(dScnPly_RenderView, mWindController, 0x6BD0);
WWHD_OFFSET(dScnPly_StageInfo, mParticleFlags, 0xA);
WWHD_OFFSET(dScnPly_StageInfo, mStageFlags, 0xC);
WWHD_OFFSET(dScnPly_Background, mVtable, 0x1400);
WWHD_OFFSET(dScnPly_Window, mCameraID, 0x28);
WWHD_OFFSET(dScnPly_GameHeapInfo, mExpHeap2D, 0x5C24);
struct dScnPly_HeaderStatics {
    be<f32> mAngles[2];
    be<f32> mFloatDefaults[4];
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
WWHD_OFFSET(dScnPly_HeaderStatics, mFlag, 0x18);
WWHD_OFFSET(dScnPly_HeaderStatics, mZeroWords, 0x1C);
struct dScnPly_DarkStatus {
    be<u32> mWords[8];
};
extern dScnPly_HeaderStatics &l_headerStatics;
extern dScnPly_DarkStatus &l_defaultDarkStatus;

struct dScnPly_VaList {
    be<u8> gpr, fpr;
    be<u16> _pad0;
    be<u32> overflow_arg_area;
    be<u32> reg_save_area;
    be<u32> _pad1;
    be<u32> gprs[8];  /* r3..r10 */
    be<u32> fprs[16]; /* f1..f8 as doubles */
    be<u32> _pad2[4];
};
WWHD_SIZE(dScnPly_VaList, 0x80);
WWHD_OFFSET(dScnPly_VaList, gprs, 0x10);
WWHD_OFFSET(dScnPly_VaList, fprs, 0x30);

struct dScnPly_WipeTypes {
    be<s16> mTypes[12];
};
struct dScnPly_WipeColor {
    be<u8> r, g, b, a;
};
struct dScnPly_J3DSystem {
    u8 paddingView[0x38];
    be<f32> mViewMatrix[12];
    u8 paddingProjection[0x14C - 0x68];
    be<f32> mProjectionMatrix[16];
};
WWHD_OFFSET(dScnPly_J3DSystem, mViewMatrix, 0x38);
WWHD_OFFSET(dScnPly_J3DSystem, mProjectionMatrix, 0x14C);
extern dScnPly_WipeTypes &l_wipeType;
extern dScnPly_WipeColor &l_wipeWhite;
extern dScnPly_WipeColor &l_wipeBlack;
extern dScnPly_J3DSystem &l_j3dSystem;

// HD: the shared play view exposes cCcS only; keep the native mass-manager tail local.
struct dScnPly_CollisionSystem {
    u8 padding[0x2854];
    be<u8> mMassManager[0x1A0];
};
WWHD_OFFSET(dScnPly_CollisionSystem, mMassManager, 0x2854);

// Assert every unit-private guest view used by the scene or its native systems.
WWHD_SIZE(dScnPly_HeapData, 4);
WWHD_SIZE(dScnPly_PreLoadData, 2);
WWHD_OFFSET(dScnPly_RegData, mChildren, 4);
WWHD_OFFSET(dScnPly_SceneObjectVtable, checkDerivedRuntimeTypeInfo, 0x44);
WWHD_SIZE(dScnPly_RuntimeType, 4);
WWHD_SIZE(dScnPly_RuntimeTypeGuard, 4);
WWHD_OFFSET(dScnPly_StateVtable, queryState, 0x14);
WWHD_SIZE(dScnPly_PreLoadTable, 16 * 12);
WWHD_SIZE(dScnPly_ResourcePhases, 35 * 8);
WWHD_SIZE(dScnPly_PhaseTable, 11 * 4);
WWHD_SIZE(dScnPly_Word, 4);
WWHD_SIZE(dScnPly_Byte, 1);
WWHD_SIZE(dScnPly_Half, 2);
WWHD_OFFSET(dScnPly_StateContainer, mState, 0x10);
WWHD_OFFSET(dScnPly_HeapVtable, destroy, 0x24);
WWHD_OFFSET(dScnPly_HeapVtable, getFreeSize, 0x74);
WWHD_OFFSET(dScnPly_StringVtable, assureTermination, 0x14);
WWHD_OFFSET(dScnPly_ResourceHandle, mpFile, 0x14);
WWHD_OFFSET(dScnPly_DisplayHeapState, mState, 0x80);
WWHD_OFFSET(dScnPly_ResourceManager, mIsStarted, 0x212C);
WWHD_OFFSET(dScnPly_SceneCommand, mIsComplete, 0xC);
WWHD_OFFSET(dScnPly_Environment, mOceanShadowEnabled, 0x168A);
WWHD_OFFSET(dScnPly_StageVtable, getStagInfo, 0x15C);
WWHD_SIZE(dScnPly_StageData, 4);
WWHD_OFFSET(dScnPly_BackgroundVtable, construct, 0x14);
WWHD_OFFSET(dScnPly_BackgroundVtable, destroy, 0x1C);
WWHD_OFFSET(dScnPly_BackgroundVtable, move, 0x24);
WWHD_OFFSET(dScnPly_Attention, mOwner, 0);
WWHD_OFFSET(dScnPly_Attention, mParameter, 0xC);
WWHD_OFFSET(dScnPly_Particle, mpModelControl, 0x130);
WWHD_OFFSET(dScnPly_DrawTag, mData, 0xC);
WWHD_SIZE(dScnPly_Reset, 8);
WWHD_OFFSET(dScnPly_SaveRoot, mSave, 0x20);
WWHD_SIZE(dScnPly_DarkStatus, 32);
WWHD_SIZE(dScnPly_WipeTypes, 24);
WWHD_SIZE(dScnPly_WipeColor, 4);
