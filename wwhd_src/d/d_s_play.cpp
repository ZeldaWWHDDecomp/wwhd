/* Play scene. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"
const gc_tu_data gc_tu = {.safeString = 0x10052EEC, .safeStringAssure = 0x025B376C};
const dScnPly_TuData play_tu = {0x10052FBC, 0x10052FD4,
                                0x10052FFC, 0x10053014,
                                0x10053044, 0x10052FE4,
                                0x1005302C, 0x1005305C,
                                0x10052FAC, 0x10052F04,
                                0x10052F7C, 0x1005306C,
                                0x10052F64, {0x101EAC90, 0x101EAC9C, 0x101EACA8}};
static dScnPly_WipeTypes l_wipeType;
static dScnPly_WipeColor l_wipeWhite;
static dScnPly_WipeColor l_wipeBlack;
static dScnPly_J3DSystem l_j3dSystem;
static dScnPly_HeaderStatics l_headerStatics;
static dScnPly_DarkStatus l_defaultDarkStatus;
static dScnPly_Word l_gameSystem;
static dScnPly_Word l_endingSystem;
static dScnPly_Word l_titleSystem;
static dScnPly_Word l_seaEndingSystem;
static dScnPly_Word l_resourceManager;
static dScnPly_Word l_resourceSystem;
static dScnPly_Word l_resourceLoader;
static dScnPly_Word l_resourceAccess;
static dScnPly_Word l_sceneRenderer;
static dScnPly_Word l_environment;
static dScnPly_Word l_controllerSystem;
static dScnPly_Word l_reset;
static dScnPly_Word l_save;
static dScnPly_Word l_displaySystem;
static dScnPly_Byte l_displayReady;
static dScnPly_Byte l_keepGameSystem;
static dScnPly_Byte l_bgmSet;
static dScnPly_Byte l_wipeEnabled;
static dScnPly_Byte l_canonGameResult;
static dScnPly_Byte l_graphicsResetState;
static dScnPly_Byte l_fadeEnabled;
static dScnPly_Half l_monotoneRate;
static dScnPly_Half l_monotoneRateSpeed;
static dScnPly_Word l_activeSceneSystem;
static dScnPly_Word l_sceneIndex;
static dScnPly_Word l_sceneDescriptionIndex;
static dScnPly_Word l_sceneBaseIndex;
static dScnPly_Word l_sceneResourceIndex;
static dScnPly_Word l_messageManager;
static dScnPly_Word l_particleFrame;
static dScnPly_Word l_roomProcessID;
static dScnPly_Word l_ykgrEmitter;
static dScnPly_Word l_salvageTagData;
static dScnPly_Word l_salvageID;
static dScnPly_Byte l_iballState;
static dScnPly_Byte l_ykgrAlphaFlag;
static dScnPly_Byte l_ykgrAlpha;
static dScnPly_Byte l_salvageRoomNo;
static dScnPly_Byte l_nowDaizaNum;
static dScnPly_Byte l_nowItemNum;
static dScnPly_Word l_preLoadTimeHigh;
static dScnPly_Word l_preLoadTimeLow;
static dScnPly_Word l_preLoadHIO;
static dScnPly_PreLoadTable PreLoadInfoT;
static dScnPly_ResourcePhases resPhase;
static dScnPly_PhaseTable l_method;
static dScnPly_RuntimeType l_sceneType;
static dScnPly_RuntimeTypeGuard l_sceneTypeGuard;
static dScnPly_SystemState l_firstPauseState;
static dScnPly_SystemState l_secondPauseState;
static dScnPly_TimerData l_timers;
static dScnPly_HeapData l_oceanHeap;
static dScnPly_PreLoadData l_preLoad;
static dScnPly_dark_HIO_c g_darkHIO;
static dScnPly_msg_HIO_c g_msgDHIO;
static dScnPly_RegData g_regHIO;
static dScnPly_EnvData g_envHIO;

/* m_Do_controller_pad: trigger/hold checks on a pad port */

/* 025B02E8 */
s8 dScnPly::dScnPly_ply_c::calcPauseTimer() {
    s8 next = (s8)l_timers.nextPauseTimer;
    if (next != 0) {
        l_timers.pauseTimer = next;
        l_timers.nextPauseTimer = 0;
        return next;
    }
    return dScnPly_cLib_calcTimer_s8(gabi::ea(&l_timers.pauseTimer));
}

// HD: pause while the timer or a native system overlay is active.
/* 025AF2A4 */
static s32 dScnPly_isPause() {
    u32 sys = l_gameSystem.mValue;
    s32 ret = (s8)l_timers.pauseTimer;
    if (sys != 0) {
        s32 open = 1;
        if (dScnPly_queryPauseRequest() == 0 &&
            dScnPly_queryPrimaryPause(l_gameSystem.mValue) == 0 &&
            dScnPly_querySecondaryPause(l_gameSystem.mValue) == 0)
            open = 0;
        ret |= open;
    }
    u32 q = l_endingSystem.mValue;
    if (q != 0) {
        s32 open = 1;
        u32 s = dScnPly_getSystemState(gabi::ea(&gabi::at<dScnPly_StateContainer>(q)->mState));
        u32 vt = l_firstPauseState.mVtable.v;
        u32 a = gabi::call_ptr<u32>(gabi::at<dScnPly_SystemState>(s)->mVtable->queryState, s);
        u32 b = gabi::call_ptr<u32>(gabi::at<dScnPly_StateVtable>(vt)->queryState,
                                    gabi::ea(&l_firstPauseState));
        if (a != b) {
            u32 s2 = dScnPly_getSystemState(gabi::ea(&gabi::at<dScnPly_StateContainer>(q)->mState));
            u32 vt2 = l_secondPauseState.mVtable.v;
            u32 a2 =
                gabi::call_ptr<u32>(gabi::at<dScnPly_SystemState>(s2)->mVtable->queryState, s2);
            u32 b2 = gabi::call_ptr<u32>(gabi::at<dScnPly_StateVtable>(vt2)->queryState,
                                         gabi::ea(&l_secondPauseState));
            if (a2 != b2)
                open = 0;
        }
        ret |= open;
    }
    return ret;
}

/* 025AF3D4 */
void dScnPly_msg_HIO_c::dScnPly_msg_HIO_checkUpdate(u32 i_update) { mIsUpdate = (u8)i_update; }

/* 025AF43C */
void dScnPly_msg_HIO_c::dScnPly_msg_HIO_setUpdate(u32 param_0) { field_0x07 = (u8)param_0; }

// HD: mID has only a lower clamp; the retail JUTReport call is omitted.
/* 025AF3DC */
void dScnPly_msg_HIO_c::dScnPly_msg_HIO_numUpdate(s32 i_addGroup, s32 i_addID) {
    s16 group = (s16)((s16)mGroup + i_addGroup);
    s16 id = (s16)((s16)mID + i_addID);
    mGroup = (u16)group;
    mID = (u16)id;
    if (group > 99) {
        id = (s16)mID;
        mGroup = 99;
    } else if (group < 0) {
        id = (s16)mID;
        mGroup = 0;
    }
    if (id < 0)
        mID = 0;
}

/* 025AF444 */
void dScnPly_msg_HIO_c::dScnPly_msg_HIO_padCheck() {
    if (mIsUpdate == 0)
        return;
    if (dScnPly_pad_trigZ(3))
        dScnPly_msg_HIO_checkUpdate(0);
    if (dScnPly_pad_trigUp(3)) {
        if (dScnPly_pad_holdA(3))
            dScnPly_msg_HIO_numUpdate(10, 0);
        else
            dScnPly_msg_HIO_numUpdate(1, 0);
    } else if (dScnPly_pad_trigDown(3)) {
        if (dScnPly_pad_holdA(3))
            dScnPly_msg_HIO_numUpdate(-10, 0);
        else
            dScnPly_msg_HIO_numUpdate(-1, 0);
    } else if (dScnPly_pad_trigRight(3)) {
        if (dScnPly_pad_holdY(3))
            dScnPly_msg_HIO_numUpdate(0, 1000);
        else if (dScnPly_pad_holdX(3))
            dScnPly_msg_HIO_numUpdate(0, 100);
        else if (dScnPly_pad_holdA(3))
            dScnPly_msg_HIO_numUpdate(0, 10);
        else
            dScnPly_msg_HIO_numUpdate(0, 1);
    } else if (dScnPly_pad_trigLeft(3)) {
        if (dScnPly_pad_holdY(3))
            dScnPly_msg_HIO_numUpdate(0, -1000);
        else if (dScnPly_pad_holdX(3))
            dScnPly_msg_HIO_numUpdate(0, -100);
        else if (dScnPly_pad_holdA(3))
            dScnPly_msg_HIO_numUpdate(0, -10);
        else
            dScnPly_msg_HIO_numUpdate(0, -1);
    } else if (dScnPly_pad_trigB(3)) {
        if (field_0x06 == 0)
            field_0x06 = 1;
    } else if (field_0x07 != 0) {
        if (field_0x06 == 0)
            field_0x06 = 1;
        dScnPly_msg_HIO_setUpdate(0);
    }
}

// HD: the message number is not searched (group/ID reset to 0 instead), messages are driven through the HD message manager by id
/* 025AF738 */
void dScnPly_msg_HIO_c::dScnPly_msg_HIO_messageProc() {
    dScnPly_msg_HIO_padCheck();
    u32 mgr = l_messageManager.mValue;
    if (field_0x06 == 0)
        return;
    if ((s32)field_0x10 == -1) {
        s32 group = (s16)mGroup;
        u32 msg_num = ((u32)group << 16) | (u32)(s32)(s16)mID;
        gabi::Local<cXyz> pos;
        pos->x = 0.0f;
        pos->z = 0.0f;
        pos->y = 0.0f;
        if (group != 99 && group != 98 && group != 89) {
            mGroup = 0;
            mID = 0;
            field_0x10 = dScnPly_fopMsgM_messageSet(mgr, 0, gabi::ea(pos.get()));
            return;
        }
        field_0x10 = dScnPly_fopMsgM_messageSet(mgr, msg_num, gabi::ea(pos.get()));
        return;
    }
    if (dScnPly_fopMsgM_getStatus(mgr) == 0xE) {
        dScnPly_fopMsgM_setStatus(mgr, 0x10);
        return;
    }
    if (dScnPly_fopMsgM_getStatus(mgr) == 0x12) {
        dScnPly_fopMsgM_setStatus(mgr, 0x13);
        s16 id = (s16)mID;
        field_0x10 = (u32)-1;
        field_0x06 = 0;
        mID = (u16)(id + 1);
    }
}

// HD: select the native system for the current scene mode.
static inline void mode_update(dScnPly::dScnPly_ply_c *i_this) {
    u32 mode = i_this->mMode;
    if (mode == dScnPly::MODE_TITLE)
        dScnPly_updateTitleSystem(l_titleSystem.mValue);
    else if (mode == dScnPly::MODE_GAME)
        dScnPly_updateGameSystem(l_gameSystem.mValue);
    else if (mode == dScnPly::MODE_ENDING)
        dScnPly_updateEndingSystem(l_endingSystem.mValue);
    else if (mode == dScnPly::MODE_OCEAN_ENDING)
        dScnPly_updateSeaEndingSystem(l_seaEndingSystem.mValue);
}

// HD: pause handling also updates the selected native scene system.
/* 025B0314 */
static BOOL dScnPly_Execute(dScnPly::dScnPly_ply_c *i_this) {
    if (!dScnPly_fopOvlpM_IsPeek()) {
        if (l_bgmSet.mValue != 0) { /* mDoAud_zelAudio_c::isBgmSet() */
            dScnPly_mDoAud_sceneBgmStart();
            dScnPly_mDoAud_load2ndDynamicWave();
            l_bgmSet.mValue = 0;
        }
        if (i_this->calcPauseTimer() != 0) {
            if (i_this->mMode == dScnPly::MODE_GAME)
                dScnPly_updateGameSystem(l_gameSystem.mValue);
            return TRUE;
        }
    }
    dScnPly_dKy_itudemo_se();
    if (!dScnPly_dMenu_flag() && !dScnPly_isPause()) {
        dScnPly_updateSceneSystem(gabi::ea(&i_this->mSceneSystem));
        if (i_this->mIsOcean != 0) {
            u32 wv = dScnPly_dKyw_get_wind_vec();
            f32 pow = dScnPly_dKyw_get_wind_pow();
            f32 wz = gabi::at<cXyz>(wv)->z;
            f32 a = gabi::fmuls_ppc(0.0372f, wz);
            f32 wx = gabi::at<cXyz>(wv)->x;
            f32 d = gabi::fsubs_ppc(pow, 0.3f);
            f32 b = gabi::fmuls_ppc(0.0372f, wx);
            f32 p = d >= 0.0f ? pow : 0.3f; /* fsel */
            i_this->mWindSystem.mWindZ = gabi::fmuls_ppc(a, p);
            i_this->mWindSystem.mWindX = gabi::fmuls_ppc(b, p);
            dScnPly_updateWindSystem(gabi::ea(&i_this->mWindSystem), 1.0f);
            u32 idx = (u32)(s32)*gabi::at<be<s16>>(l_sceneIndex.mValue);
            u32 n1 = i_this->mSceneSystem.mEntryCount;
            dScnPly::SceneEntry *arr = i_this->mSceneSystem.mpEntries;
            dScnPly::SceneEntry *entry = idx < n1 ? arr + idx : arr;
            u32 obj;
            if (entry->mIsValid == 0) {
                obj = 0;
            } else {
                u32 n2 = i_this->mSceneSystem.mObjectCount;
                entry = idx < n1 ? arr + idx : arr;
                u32 k = entry->mObjectIndex;
                gptr<void> *object = nullptr;
                if (k < n2)
                    object = i_this->mSceneSystem.mpObjects + k;
                // HD: the invalid-table case still reads guest address zero.
                obj = object->v;
            }
            /* function-local static (an empty object with a vtable) */
            if (l_sceneTypeGuard.mInitialized == 0) {
                l_sceneTypeGuard.mInitialized = 1;
                l_sceneType.mVtable = play_tu.mSceneTypeVtable;
            }
            if (obj != 0 &&
                gabi::call_ptr<u32>(gabi::at<dScnPly_SceneObject>(obj)
                                        ->mRuntimeTypeVtable->checkDerivedRuntimeTypeInfo,
                                    obj, &l_sceneType) == 0)
                obj = 0;
            dScnPly_updateWindController(gabi::ea(&i_this->mWindController),
                                         gabi::ea(&i_this->mWindSystem.mWindParameters),
                                         gabi::ea(&gabi::at<dScnPly_SceneObject>(obj)->mWindInput));
        }
        dScnPly_updateDemo();
        dScnPly_updatePlay(gabi::ea(&dScnPly_gameInfo()->play));
        u32 att = gabi::ea(&dScnPly_gameInfo()->play.mAttention); /* dAttention_c */
        if (gabi::at<dScnPly_Attention>(att)->mOwner != 0) {
            dScnPly_dAttention_Run(att, -1);
        } else {
            u32 pl = dScnPly_gameInfo()->play.mPlayerInfo[0].mpPlayer.v;
            gabi::at<dScnPly_Attention>(att)->mParameter = 0; /* Init(player, 0) */
            gabi::at<dScnPly_Attention>(att)->mOwner = pl;
        }
        dScnPly_processDetect(
            gabi::ea(&dScnPly_gameInfo()->play.mDetect)); /* dComIfGp_getDetect().proc() */
    }
    mode_update(i_this);
    return TRUE;
}

/* 025B05DC */
static BOOL dScnPly_IsDelete(dScnPly::dScnPly_ply_c *i_this) { return TRUE; }

// HD: HD-only: destroys the extra actor heap 1047C8B4 (see fopAcM_entrySolidHeap)
/* 025B05E4 */
static void dScnPly_destroyExtraHeap() {
    u32 heap = l_oceanHeap.mpHeap;
    if (heap != 0) {
        gabi::call_ptr(gabi::at<dScnPly_HeapObject>(heap)->mVtable->destroy, heap); /* destroy */
        l_oceanHeap.mpHeap = 0;
    }
}

// HD: dScnPly_ply_c::dScnPly_ply_c (HD 0xB68)
/* 025B0F2C */
static dScnPly::dScnPly_ply_c *dScnPly_ply_c_ct(dScnPly::dScnPly_ply_c *self) {
    if (self == 0) {
        self =
            gabi::at<dScnPly::dScnPly_ply_c>(dScnPly_operator_new(sizeof(dScnPly::dScnPly_ply_c)));
        if (self == 0)
            return self;
    }
    dScnPly_constructSceneBase(gabi::ea(self)); /* scene_class base */
    self->sceneCommand = 0;
    dScnPly_constructSceneSystem(gabi::ea(&self->mSceneSystem));
    dScnPly_constructWindSystem(gabi::ea(&self->mWindSystem));
    if (gabi::ea(&self->mWindController) == 0)
        dScnPly_operator_new(0x40); /* inline constructor of an empty 0x40-byte member */
    dScnPly_constructWindModel(gabi::ea(&self->mWindModel));
    self->mMode = dScnPly::MODE_TITLE;
    self->mIsOcean = 0;
    return self;
}

/* 025B13EC */
static s32 phase_01(u32 i_this) {
    s32 r = dScnPly_mDoAud_load1stDynamicWave();
    if (r == 0)
        return r;
    return cPhs_NEXT_e;
}
/* WWHD glue
data l_wipeType 101EAC4C
data l_wipeWhite 101D5E9C
data l_wipeBlack 101D5E94
data l_j3dSystem 104B45C0
data l_headerStatics 1047B41C
data l_defaultDarkStatus 101EE5C8
data l_gameSystem 101F8344
data l_endingSystem 101F83D0
data l_titleSystem 101F83FC
data l_seaEndingSystem 101F8268
data l_resourceManager 101F4F28
data l_resourceSystem 101F4F7C
data l_resourceLoader 101F7274
data l_resourceAccess 101F8B18
data l_sceneRenderer 101F95D0
data l_environment 101F86E8
data l_controllerSystem 101F8710
data l_reset 101F4974
data l_save 101F84DC
data l_displaySystem 1018F47C
data l_displayReady 1018F484
data l_keepGameSystem 101F8350
data l_bgmSet 101F4706
data l_wipeEnabled 101D616C
data l_canonGameResult 101D5F42
data l_graphicsResetState 101F4825
data l_fadeEnabled 101F4827
data l_monotoneRate 101F4820
data l_monotoneRateSpeed 101F4822
data l_activeSceneSystem 104B4708
data l_sceneIndex 104A145C
data l_sceneDescriptionIndex 104A1460
data l_sceneBaseIndex 104A1F64
data l_sceneResourceIndex 104A1F70
data l_messageManager 101F4B5C
data l_particleFrame 101FF560
data l_roomProcessID 1047E6C4
data l_ykgrEmitter 10475628
data l_salvageTagData 10475634
data l_salvageID 10475638
data l_iballState 10475651
data l_ykgrAlphaFlag 10475652
data l_ykgrAlpha 10475653
data l_salvageRoomNo 10475654
data l_nowDaizaNum 10475655
data l_nowItemNum 10475656
data l_preLoadTimeHigh 1047B600
data l_preLoadTimeLow 1047B604
data l_preLoadHIO 1047B418
data PreLoadInfoT 100530D8
data resPhase 1047B448
data l_method 101EAC64
data l_sceneType 101FDD14
data l_sceneTypeGuard 101FD7E0
data l_firstPauseState 1049F668
data l_secondPauseState 1049F694
data l_timers 101EACB6
data l_oceanHeap 1047C8B4
data l_preLoad 101EACB4
data g_darkHIO 1047B560
data g_msgDHIO 1047C89C
data g_regHIO 1047B608
data g_envHIO 1047B588

ext 02006478 u32 dScnPlyAPI_getSystemState(u32)
ext 02006478 u32 dScnPly_getSystemState(u32)
ext 020076BC BOOL dScnPly_pad_holdA(s32)
ext 0200770C BOOL dScnPly_pad_holdX(s32)
ext 02007738 BOOL dScnPly_pad_holdY(s32)
ext 02007764 BOOL dScnPly_pad_trigUp(s32)
ext 02007790 BOOL dScnPly_pad_trigDown(s32)
ext 020077BC BOOL dScnPly_pad_trigLeft(s32)
ext 020077E8 BOOL dScnPly_pad_trigRight(s32)
ext 02007814 BOOL dScnPly_pad_trigZ(s32)
ext 020078BC BOOL dScnPly_pad_trigB(s32)
ext 02038624 void dScnPlyAPI_prepareDisplayHeap(u32)
ext 020386CC void dScnPlyAPI_releaseDisplayHeap(u32)
ext 02038744 void dScnPlyAPI_attachDisplayHeap(u32, u32)
ext 0203899C void dScnPlyAPI_releaseDisplayResources(u32)
ext 020389F0 u32 dScnPlyAPI_getDisplayHeap(u32)
ext 02038A3C void dScnPlyAPI_initializeDisplayResources(u32)
ext 0203E8F4 u32 dScnPlyAPI_getCurrentHeap_u320()
ext 0203E8F4 u32 dScnPlyAPI_getCurrentHeap_u321(u32)
ext 0203E900 u32 dScnPlyAPI_getDisplayAllocator()
ext 0220CCB4 s8 dScnPly_cLib_calcTimer_s8(u32)
ext 023AB3D0 void dScnPlyAPI_resetTribox()
ext 02443664 BOOL dScnPlyAPI_checkGameOverStart(u32)
ext 0246B42C u32 dScnPlyAPI_initializeSea(u32)
ext 024ED73C void dScnPlyAPI_initializeAttention(u32, u32, u32)
ext 024EDB20 void dScnPlyAPI_destroyAttention(u32, u32)
ext 024EDB80 void dScnPly_dAttention_Run(u32, s32)
ext 024EDDF8 void dScnPlyAPI_drawAttention(u32)
ext 024EE9C8 void dScnPlyAPI_clearBackgroundMoveFlags(u32)
ext 02516E1C void dScnPlyAPI_prepareMassCollision(u32)
ext 02518760 u32 dScnPlyAPI_constructCollision(u32)
ext 02518794 u32 dScnPlyAPI_destroyCollision(u32)
ext 02518798 void dScnPlyAPI_moveCollision(u32)
ext 0251879C void dScnPlyAPI_drawCollision(u32)
ext 025187D0 void dScnPlyAPI_clearCollision(u32)
ext 0251FD14 void dScnPlyAPI_initializePlay(u32)
ext 025202F8 BOOL dScnPlyAPI_resetToOpening(u32)
ext 02520460 s32 dScnPlyAPI_loadResource(u32, u32)
ext 025204C8 void dScnPlyAPI_deleteResource(u32, u32)
ext 02522398 void dScnPlyAPI_setSelectEquip(u32, u32)
ext 025224A4 void dScnPlyAPI_savePlayerRecollection()
ext 02522C60 void dScnPlyAPI_restorePlayerRecollection()
ext 02523A04 s32 dScnPlyAPI_setStageResource(u32, u32)
ext 02523D08 s32 dScnPlyAPI_syncStageResource(u32)
ext 02524BA8 void dScnPly_updatePlay(u32)
ext 02524C50 void dScnPlyAPI_removeMagma(u32)
ext 02524CA0 void dScnPlyAPI_calculateMagma(u32)
ext 02524CB0 void dScnPlyAPI_drawMagma(u32)
ext 02524D3C void dScnPlyAPI_removeGrass(u32)
ext 02524DA0 void dScnPlyAPI_calculateGrass(u32)
ext 02524DB0 void dScnPlyAPI_drawGrass(u32)
ext 02524E3C void dScnPlyAPI_removeTree(u32)
ext 02524EA0 void dScnPlyAPI_calculateTree(u32)
ext 02524EB0 void dScnPlyAPI_drawTree(u32)
ext 02524F3C void dScnPlyAPI_removeWood(u32)
ext 02524FA0 void dScnPlyAPI_calculateWood(u32)
ext 02524FB0 void dScnPlyAPI_drawWood(u32)
ext 0252503C void dScnPlyAPI_removeFlower(u32)
ext 025250A0 void dScnPlyAPI_calculateFlower(u32)
ext 025250B0 void dScnPlyAPI_drawFlower(u32)
ext 02525C94 void dScnPly_updateWindController(u32, u32, u32)
ext 02525FE4 s32 dScnPlyAPI_phaseHandler(u32, u32, u32)
ext 025260D8 void dScnPlyAPI_initializeSteamTags()
ext 02526124 void dScnPlyAPI_initializePlayActors()
ext 02526570 void dScnPlyAPI_enableTitleKirakiraSound()
ext 025286E0 u32 dScnPlyAPI_createDemo(u32)
ext 02528B48 u32 dScnPlyAPI_removeDemo(u32)
ext 025291C8 void dScnPly_updateDemo()
ext 0252A154 void dScnPly_processDetect(u32)
ext 0252CC8C void dScnPlyAPI_setViewport(u32, f32, f32, f32, f32, f32, f32)
ext 0252CCA8 void dScnPlyAPI_setScissor(u32, f32, f32, f32, f32)
ext 0253EA70 void dScnPlyAPI_removeEvent(u32)
ext 02556BC0 BOOL dScnPlyAPI_checkEventNightStop()
ext 02556C34 s32 dScnPlyAPI_getDaytimeHour()
ext 02560484 void dScnPly_dKy_itudemo_se()
ext 0256C8BC BOOL dScnPlyAPI_checkPoisonCollision()
ext 0256C8FC void dScnPlyAPI_calculatePoisonCollision()
ext 02578348 f32 dScnPly_dKyw_get_wind_pow()
ext 0257DAA8 u32 dScnPly_dKyw_get_wind_vec()
ext 0259149C void dScnPlyAPI_createEnvironment(u32)
ext 02591588 void dScnPlyAPI_deleteEnvironment(u32)
ext 025986BC BOOL dScnPlyAPI_checkMenu()
ext 025986BC BOOL dScnPly_dMenu_flag()
ext 025A4984 void dScnPlyAPI_drawParticleModels(u32)
ext 025A7E18 void dScnPlyAPI_createParticleScene(u32, u32)
ext 025A809C u32 dScnPlyAPI_removeParticleScene(u32)
ext 025A8148 void dScnPlyAPI_drawParticleDirect(u32)
ext 025A81A0 void dScnPlyAPI_calculateParticles(u32)
ext 025A81F8 void dScnPlyAPI_drawParticleIndirect(u32)
ext 025AF2A4 BOOL dScnPlyAPI_checkPause()
ext 025AF738 void dScnPlyAPI_processMessageHIO(u32)
ext 025B05E4 void dScnPlyAPI_destroyOceanHeap()
ext 025B0C30 BOOL dScnPlyAPI_checkHeapSize()
ext 025B0F2C void dScnPlyAPI_constructPlayScene(u32)
ext 025B3614 void dScnPlyAPI_formatLinkArchive(u32, u32, u32)
ext 025B376C void dScnPlyAPI_assureSafeString(u32)
ext 025B5D40 BOOL dScnPlyAPI_checkItemRecord(u32, u32, u32)
ext 025B79B8 void dScnPlyAPI_clearCollect(u32, u32, u32)
ext 025B8B94 BOOL dScnPlyAPI_checkEventBit(u32, u32)
ext 025B9A18 void dScnPlyAPI_initializeSave(u32)
ext 025BDBB0 void dScnPlyAPI_createSnapshot(u32)
ext 025BE8DC void dScnPlyAPI_executeSnapshot()
ext 025BED8C void dScnPlyAPI_drawSnapshot(u32)
ext 025BEF30 u32 dScnPlyAPI_deleteSnapshot(u32)
ext 025C2EEC void dScnPlyAPI_createStageInfo()
ext 025C3094 void dScnPlyAPI_createStage()
ext 025C3370 void dScnPlyAPI_deleteStage(u32)
ext 025C4364 void dScnPlyAPI_checkRoomDrawArea(u32)
ext 025CB6D4 void dScnPlyAPI_pauseVibration(u32)
ext 025CBB90 void dScnPlyAPI_runVibration(u32)
ext 025CC0C8 void dScnPlyAPI_initializeVibration(u32)
ext 025CC110 void dScnPlyAPI_destroyVibration(u32, u32)
ext 025DA7AC u32 dScnPlyAPI_beginDrawQueue()
ext 025DA7D0 u32 dScnPlyAPI_nextDrawQueue(u32)
ext 025DB8A4 u32 dScnPlyAPI_createMessageHeap(u32)
ext 025DB8F4 void dScnPlyAPI_destroyMessageHeap(u32)
ext 025DBE00 BOOL dScnPlyAPI_checkOverlap()
ext 025DBE00 BOOL dScnPly_fopOvlpM_IsPeek()
ext 025DC86C void dScnPlyAPI_requestSceneChange(u32, u32, u32, u32, u32)
ext 025DD5F0 void dScnPly_constructSceneBase(u32)
ext 025DD630 void dScnPlyAPI_destroySceneBase(u32, u32)
ext 025DF904 void dScnPlyAPI_drawProcess(u32)
ext 025DF944 void dScnPlyAPI_deleteProcess(u32)
ext 025E17CC void dScnPlyAPI_setAudioSceneName(u32, u32, u32)
ext 025E1828 BOOL dScnPly_mDoAud_load1stDynamicWave()
ext 025E18B8 s32 dScnPlyAPI_checkFirstDynamicWave()
ext 025E18C4 void dScnPly_mDoAud_load2ndDynamicWave()
ext 025E18D0 s32 dScnPlyAPI_checkAllWaveLoadStatus()
ext 025E18D4 void dScnPlyAPI_initializeMonsterSound()
ext 025E18E0 void dScnPly_mDoAud_sceneBgmStart()
ext 025E1904 void dScnPlyAPI_stopBgm(u32)
ext 025E1D8C void dScnPlyAPI_recoverAudioReset()
ext 025E20F0 BOOL dScnPlyAPI_checkStreamHeap()
ext 025E2FBC u32 dScnPlyAPI_getGameHeap()
ext 025E2FF8 s32 dScnPlyAPI_getSafeGameHeapSize()
ext 025E3268 u32 dScnPlyAPI_getArchiveHeap()
ext 025EE048 u32 dScnPlyAPI_getZeldaHeap()
ext 025EE054 s32 dScnPlyAPI_getSafeZeldaHeapSize()
ext 025EE060 u32 dScnPlyAPI_getCommandHeap()
ext 025EE06C s32 dScnPlyAPI_getSafeCommandHeapSize()
ext 025EE078 s32 dScnPlyAPI_getSafeArchiveHeapSize()
ext 025F05F8 void dScnPlyAPI_setWipeColor(u32)
ext 025F0820 void dScnPlyAPI_enableMonotone()
ext 025F0830 void dScnPlyAPI_disableMonotone()
ext 025F096C void dScnPlyAPI_setTickRate(u32)
ext 025F0A10 u32 dScnPlyAPI_createHIOChild(u32, u32)
ext 025F0A18 void dScnPlyAPI_deleteHIOChild(u32)
ext 025F74D0 void dScnPly_fopMsgM_setStatus(u32, u32)
ext 025F795C u32 dScnPly_fopMsgM_getStatus(u32)
ext 025F7DB0 u32 dScnPly_fopMsgM_messageSet(u32, u32, u32)
ext 0260524C u32 dScnPlyAPI_getModelResource(u32, u32)
ext 026124B0 u32 dScnPlyAPI_getResource_u324(u32, u32, u32, u32)
ext 026124B0 void dScnPlyAPI_getResource_void4(u32, u32, u32, u32)
ext 02612A80 void dScnPlyAPI_startResourceSystem(u32)
ext 026FBB7C s32 dScnPlyAPI_checkSystemLoad(u32)
ext 026FBB8C s32 dScnPlyAPI_loadSystem(u32, u32)
ext 026FBBE8 void dScnPlyAPI_prepareEndingSystem(u32)
ext 026FBC4C void dScnPlyAPI_releaseEndingResources(u32)
ext 02707D70 void dScnPlyAPI_initializeSeaEndingSystem(u32)
ext 02707E78 void dScnPly_updateSeaEndingSystem(u32)
ext 02707FA0 u32 dScnPlyAPI_finalizeSeaEndingSystem(u32)
ext 02708188 void dScnPlyAPI_createSeaEndingSystem(u32)
ext 02708270 void dScnPlyAPI_destroySeaEndingSystem(u32)
ext 02713368 void dScnPlyAPI_initializeGameSystem(u32)
ext 027152B4 void dScnPly_updateGameSystem(u32)
ext 02715C10 u32 dScnPlyAPI_finalizeGameSystem(u32)
ext 02717114 BOOL dScnPly_queryPrimaryPause(u32)
ext 02717120 BOOL dScnPly_querySecondaryPause(u32)
ext 027172E4 BOOL dScnPly_queryPauseRequest()
ext 02717AD0 void dScnPlyAPI_createGameSystem(u32)
ext 02717B70 void dScnPlyAPI_destroyGameSystem(u32)
ext 0271D9FC void dScnPlyAPI_initializeEndingSystem(u32)
ext 0271DC4C void dScnPly_updateEndingSystem(u32)
ext 0271DCC8 u32 dScnPlyAPI_finalizeEndingSystem(u32)
ext 0271DF30 void dScnPlyAPI_drawEndingSystem(u32)
ext 0271E618 void dScnPlyAPI_createEndingSystem(u32)
ext 0271E6B8 void dScnPlyAPI_destroyEndingSystem(u32)
ext 0271EF88 void dScnPlyAPI_initializeTitleSystem(u32)
ext 0271F078 void dScnPly_updateTitleSystem(u32)
ext 0271F090 u32 dScnPlyAPI_finalizeTitleSystem(u32)
ext 0271F2F0 void dScnPlyAPI_createTitleSystem(u32)
ext 0271F3D8 void dScnPlyAPI_destroyTitleSystem(u32)
ext 0272D16C void dScnPlyAPI_resetControllerSystem(u32)
ext 0273AA24 void dScnPlyAPI_showAssert(u32, u32, u32)
ext 0273AA24 void dScnPly_JUT_ASSERT(u32, s32, u32)
ext 0273AD10 u32 dScnPlyAPI_allocate(u32)
ext 0273AD10 u32 dScnPly_operator_new(u32)
ext 0273AF40 void dScnPlyAPI_deallocate(u32)
ext 0274FBF8 void dScnPlyAPI_beginResourceAccess(u32)
ext 0274FCCC void dScnPlyAPI_endResourceAccess(u32)
ext 02754C90 u32 dScnPlyAPI_getUnitHeapOverhead(u32)
ext 027564DC u32 dScnPlyAPI_createOceanUnitHeap(u32, u32, u32, u32, u32, u32)
ext 02759C10 void dScnPlyAPI_formatStringV(u32, u32, u32)
ext 02759C28 void dScnPlyAPI_formatParticleName(u32, u32, u32)
ext 0276B21C void dScnPlyAPI_destroySceneSystem(u32, u32)
ext 0276B710 void dScnPlyAPI_bindSceneModel(u32, u32, u32, u32, f32)
ext 0276C1DC void dScnPly_constructSceneSystem(u32)
ext 0276C3F8 void dScnPlyAPI_initializeSceneSystem(u32, u32, u32)
ext 0276C8B8 void dScnPlyAPI_updateSceneSystem(u32)
ext 0276C8B8 void dScnPly_updateSceneSystem(u32)
ext 0276C9B4 void dScnPlyAPI_drawSceneSystem(u32, u32, u32, u32)
ext 0276CAA4 void dScnPlyAPI_constructSceneDescription(u32)
ext 0276F9F0 void dScnPlyAPI_appendSceneDescription(u32, u32, u32)
ext 0277019C void dScnPlyAPI_enableSceneObject(u32, u32)
ext 02773798 void dScnPlyAPI_initializeWindModel(u32, u32)
ext 02792010 void dScnPlyAPI_destroyWindSystem(u32, u32)
ext 0279207C void dScnPly_constructWindSystem(u32)
ext 02792640 void dScnPly_updateWindSystem(u32, f32)
ext 027A7558 u32 dScnPlyAPI_resolveSceneModel(u32, u32)
ext 027BDEB4 void dScnPlyAPI_bindWindModel(u32, u32)
ext 027BE6B8 void dScnPly_constructWindModel(u32)
ext 027DF9B0 s32 dScnPlyAPI_findResourceIndex(u32, u32)
ext 027DFA24 u32 dScnPlyAPI_findResource(u32, u32)
ext 027E2DC0 u32 dScnPlyAPI_getResourceFile()
ext 027EC1FC s32 dScnPlyAPI_getTotalFreeHeapSize(u32)
ext 027EC938 void dScnPlyAPI_reportStage(u32, u32, u32)
ext 028EAB2C void dScnPlyAPI_constructHeaderStaticByte(u32)
ext 028ED6F8 void dScnPlyAPI_constructHeaderStaticFlag(u32)
ext 028EFFD0 void dScnPlyAPI_constructArray(u32, u32, u32, u32)
ext 028F026C void dScnPlyAPI_registerGlobalObject(u32)
ext C0009988 void dScnPlyAPI_blockMove(u32, u32, u32, u32)
ext C0009C80 u32 dScnPlyAPI_getSystemInfo()
ext C0009CD8 u32 dScnPlyAPI_getTimeHigh()

*/
