/* Play scene part 6. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

static inline void assure(u32 s) {
    gabi::call_ptr(
        gabi::at<dScnPly_StringVtable>(gabi::at<dScnPly_SafeString>(s)->mVtable)->assureTermination,
        s);
}
static inline u32 relativeResource(be<u32> *field) {
    u32 offset = *field;
    return offset != 0 ? gabi::ea(field) + offset : 0;
}

/* stage name == literal, through two SafeString temporaries: the literal (assured twice), then
 * the stage name; a pointer test, then at most 0x40001 characters */
static bool stage_is(gabi::Local<dScnPly_SafeString> &lit, gabi::Local<dScnPly_SafeString> &stage,
                     u32 str) {
    lit->mVtable = gc_tu.safeString;
    lit->mString = str;
    u32 play = dComIfGp_ea();
    stage->mVtable = gc_tu.safeString;
    stage->mString = gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play.mCurStage);
    u32 a = gabi::ea(lit.get()), b = gabi::ea(stage.get());
    assure(a);
    assure(a);
    u32 s1 = (u32)*gabi::at<be<u32>>(a);
    assure(b);
    u32 s2 = (u32)*gabi::at<be<u32>>(b);
    if (s1 == s2)
        return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = (u8)*gabi::at<be<u8>>(s1);
        if (c != (u8)*gabi::at<be<u8>>(s2))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}

/* 025B1FF8 */
static s32 phase_4(dScnPly::dScnPly_ply_c *i_this) {
    /* particle scene */
    u32 stageObj = gabi::ea(&dScnPly_gameInfo()->play.mStageData);
    u32 info = gabi::call_ptr<u32>(gabi::at<dScnPly_StageData>(stageObj)->mVtable->getStagInfo,
                                   stageObj); /* dComIfGp_getStageStagInfo() */
    u32 partNo = (gabi::at<dScnPly_StageInfo>(info)->mParticleFlags >> 3) &
                 0xFF; /* dStage_stagInfo_GetParticleNo */
    gabi::Local<dScnPly_SafeString> resA, resB;
    resA->mVtable = gc_tu.safeString;
    resB->mVtable = gc_tu.safeString;
    resA->mString = gabi::guest_addr("Particle");       /* "Particle" */
    resB->mString = gabi::guest_addr("Particle.bfres"); /* "Particle.bfres" */
    u32 found = dScnPlyAPI_getResource_u324(l_resourceSystem.mValue, gabi::ea(resA.get()),
                                            gabi::ea(resB.get()), 0);
    u32 scene = 0;
    if (found != 0) {
        u32 files = dScnPlyAPI_getResourceFile();
        gabi::Local<dScnPly_FixedString32> name;
        u32 n = gabi::ea(name.get());
        name->mBuffer[0] = 0;
        name->mBuffer[0x1F] = 0;
        name->mCapacity = 0x20;
        name->mString = gabi::ea(name->mBuffer);
        name->mVtable = play_tu.mFixedString32Vtable;
        dScnPlyAPI_formatParticleName(n, gabi::guest_addr("Pscene%03d.jpc"),
                                      partNo); /* format("Pscene%03d.jpc", particle no) */
        assure(n);
        u32 dict = relativeResource(&gabi::at<dScnPly_ResourceFile>(files)->mFileDictionaryOffset);
        u32 e = dScnPlyAPI_findResource(dict, (u32)name->mString);
        if (e != 0)
            scene = relativeResource(gabi::at<be<u32>>(e));
    }
    dScnPlyAPI_createParticleScene(dScnPly_gameInfo()->play.mParticle.v,
                                   scene); /* dComIfGp_particle_createScene */

    u32 bgs = dComIfGp_ea();
    gabi::call_ptr(
        gabi::at<dScnPly_Background>(bgs + offsetof(dComIfG_inf_c, play))->mVtable->construct,
        gabi::ea(&gabi::at<dScnPly_GameInfo>(bgs)->play)); /* dComIfG_Bgsp()->Ct() */
    u32 r = dScnPlyAPI_constructCollision(
        gabi::ea(&dScnPly_gameInfo()->play.mCcS)); /* dComIfG_Ccsp()->Ct() */
    r = dScnPlyAPI_createDemo(r);                  /* dComIfGp_createDemo() */
    r = dScnPlyAPI_initializeSea(r);               /* daSea_Init() */
    r = dScnPlyAPI_getCurrentHeap_u321(r);
    dScnPlyAPI_createSnapshot(r); /* dSnap_Create() */
    u32 play = dComIfGp_ea();
    gabi::at<dScnPly_GameInfo>(play)->play.mPlayerInfo[0].mpPlayer =
        0; /* dComIfGp_setPlayerInfo(0, NULL, 0) */
    gabi::at<dScnPly_GameInfo>(play)->play.mPlayerInfo[0].mCameraID = 0;
    for (int i = 0; i < 3; i++)
        dScnPly_gameInfo()->play.mpPlayerPtr[i] = nullptr; /* dComIfGp_setPlayerPtr(i, NULL) */
    dScnPly_gameInfo()->play.mDlstWindowNum = 1;           /* dComIfGp_setWindowNum(1) */
    u32 win = gabi::ea(&dScnPly_gameInfo()->play.mDlstWindow);
    dScnPlyAPI_setViewport(win, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f); /* setViewPort */
    dScnPlyAPI_setScissor(win, 0.0f, 0.0f, 1280.0f, 720.0f);              /* setScissor */
    gabi::at<dScnPly_Window>(win)->mCameraID = 0;
    gabi::at<dScnPly_Window>(win)->mMode = 2;
    play = dComIfGp_ea(); /* dComIfGp_setCameraInfo(0, NULL, 0, 0, -1) */
    gabi::at<dScnPly_GameInfo>(play)->play.mCameraInfo[0].mDlstWindowIdx = 0;
    gabi::at<dScnPly_GameInfo>(play)->play.mCameraInfo[0].mCamP2Id = 0xFF;
    gabi::at<dScnPly_GameInfo>(play)->play.mCameraInfo[0].mpCamera = 0;
    gabi::at<dScnPly_GameInfo>(play)->play.mCameraInfo[0].mCamP1Id = 0;
    gabi::at<dScnPly_GameInfo>(play)->play.mCameraInfo[0].mCameraAttentionStatus = 0;
    dScnPly_gameInfo()->drawlist.mpWindow = 0;             /* dComIfGd_setWindow(NULL) */
    dScnPly_gameInfo()->drawlist.mpViewPort = 0; /* dComIfGd_setViewport(NULL) */
    dScnPly_gameInfo()->drawlist.mpView = 0;               /* dComIfGd_setView(NULL) */
    u32 heap = dScnPlyAPI_createMessageHeap(0x73EA1);      /* fopMsgM_createExpHeap */
    if (heap == 0)
        dScnPlyAPI_showAssert(gabi::guest_addr("d_s_play.cpp"), 0x1192,
                              gabi::guest_addr("heap != (0)"));
    gabi::at<dScnPly_GameHeapInfo>(dComIfGp_ea())->mExpHeap2D = heap; /* dComIfGp_setExpHeap2D */
    dScnPlyAPI_createStage();                                         /* dStage_Create() */

    u32 mode = i_this->mMode;
    if (mode == dScnPly::MODE_TITLE) {
        dScnPlyAPI_createTitleSystem(0);
        dScnPlyAPI_initializeTitleSystem(l_titleSystem.mValue);
    } else if (mode == dScnPly::MODE_GAME) {
        if (l_keepGameSystem.mValue == 0)
            dScnPlyAPI_createGameSystem(0);
        dScnPlyAPI_initializeGameSystem(l_gameSystem.mValue);
    } else if (mode == dScnPly::MODE_ENDING) {
        dScnPlyAPI_createEndingSystem(0);
        dScnPlyAPI_initializeEndingSystem(l_endingSystem.mValue);
    } else if (mode == dScnPly::MODE_OCEAN_ENDING) {
        dScnPlyAPI_createSeaEndingSystem(0);
        dScnPlyAPI_initializeSeaEndingSystem(l_seaEndingSystem.mValue);
    }
    u32 clk = dScnPlyAPI_getSystemInfo() /* OSGetSystemInfo (import) */;
    dScnPlyAPI_setTickRate(((u32)*gabi::at<be<u32>>(clk) >> 2) /
                           30); /* mDoGph_gInf_c::setTickRate((OS_BUS_CLOCK / 4) / 30) */
    g_darkHIO.mNo = (u8)dScnPlyAPI_createHIOChild(
        gabi::guest_addr("Darkness Spot"),
        gabi::ea(&g_darkHIO)); /* g_darkHIO.mNo = mDoHIO_createChild */

    u32 att = gabi::ea(&dScnPly_gameInfo()->play.mAttention); /* placement new of the attention */
    if (att != 0)
        dScnPlyAPI_initializeAttention(att, dScnPly_gameInfo()->play.mPlayerInfo[0].mpPlayer.v, 0);
    dScnPlyAPI_initializeVibration(
        gabi::ea(&dScnPly_gameInfo()->play.mVibration)); /* dComIfGp_getVibration().Init() */
    dScnPlyAPI_initializeSteamTags();                    /* daSteamTag_c::init() */
    /* daYkgr_c / daSalvage_c / daDai_c / daNpc_Kg2_c / daIball_c ::init() (inlined) */
    l_ykgrEmitter.mValue = 0;
    l_nowItemNum.mValue = 0;
    l_ykgrAlphaFlag.mValue = 1;
    l_canonGameResult.mValue = 0;
    l_iballState.mValue = 0;
    l_salvageID.mValue = (u32)-1;
    l_salvageRoomNo.mValue = 0xFF;
    l_nowDaizaNum.mValue = 0;
    l_ykgrAlpha.mValue = 0xFF;
    l_salvageTagData.mValue = 0;
    dScnPlyAPI_initializePlayActors();
    dScnPly_gameInfo()->mBrightness = 0xFF; /* dComIfG_setBrightness(0xFF) */
    l_fadeEnabled.mValue = 0;               /* mDoGph_gInf_c::offFade() */

    u32 so = gabi::ea(&dScnPly_gameInfo()->play.mStageData);
    u32 stag = gabi::call_ptr<u32>(gabi::at<dScnPly_StageData>(so)->mVtable->getStagInfo, so);
    if (stag != 0 && ((gabi::at<dScnPly_StageInfo>(stag)->mStageFlags >> 16) & 7) == 5 &&
        dScnPlyAPI_checkEventBit(
            gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)->mSave.mSavedata.mEvent),
            0x801)) { /* dStageType_FF1, isEventBit(0x0801) */
        dScnPlyAPI_setSelectEquip(0,
                                  dScnPly::dItemNo_NONE_e); /* dComIfGs_setSelectEquip(0, NONE) */
        dScnPly_gameInfo()->play.mSelectEquip[0] =
            dScnPly::dItemNo_NONE_e; /* dComIfGp_setSelectEquip(0, NONE) */
        dScnPlyAPI_clearCollect(
            gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)->mSave.mSavedata.mPlayer.mCollect),
            0, 0); /* dComIfGs_offCollect(0, 0) */
    }

    gabi::Local<dScnPly_SafeString> lit, stg;
    if (!stage_is(lit, stg, gabi::guest_addr("GTower"))) { /* "GTower" */
        if (gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                ->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] ==
            dScnPly::dItemNo_NONE_e) { /* the bow slot is empty: give it back */
            if (dScnPlyAPI_checkItemRecord(gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                                                         ->mSave.mSavedata.mPlayer.mGetItem),
                                           0xC, 2))
                gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                    ->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] =
                    dScnPly::dItemNo_LIGHT_ARROW_e;
            else if (dScnPlyAPI_checkItemRecord(gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                                                              ->mSave.mSavedata.mPlayer.mGetItem),
                                                0xC, 1))
                gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                    ->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] =
                    dScnPly::dItemNo_MAGIC_ARROW_e;
            else if (dScnPlyAPI_checkItemRecord(gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                                                              ->mSave.mSavedata.mPlayer.mGetItem),
                                                0xC, 0))
                gabi::at<dScnPly_SaveRoot>(l_save.mValue)
                    ->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] = dScnPly::dItemNo_BOW_e;
        }
    }
    gabi::Local<dScnPly_SafeString> lit2, stg2, lit3, stg3, lit4, stg4;
    bool xboss = stage_is(lit, stg, gabi::guest_addr("Xboss0")) ||
                 stage_is(lit2, stg2, gabi::guest_addr("Xboss1")) ||
                 stage_is(lit3, stg3, gabi::guest_addr("Xboss2")) ||
                 stage_is(lit4, stg4, gabi::guest_addr("Xboss3")); /* "Xboss0".."Xboss3" */
    if (xboss)
        dScnPlyAPI_savePlayerRecollection(); /* dComIfGs_setPlayerRecollectionData() */
    dScnPlyAPI_initializeMonsterSound();     /* mDoAud_monsSeInit() */
    s32 name = i_this != 0 ? (s16)i_this->mProcName : 0x7FFF;
    l_timers.pauseTimer = 0;     /* dScnPly_ply_c::pauseTimer */
    l_timers.nextPauseTimer = 0; /* dScnPly_ply_c::nextPauseTimer */
    l_bgmSet.mValue = name == 7; /* mDoAud_zelAudio_c::onBgmSet / offBgmSet */

    gabi::Local<dScnPly_SafeString> h1, s1, h2, s2, h3, s3, h4, s4;
    bool mono;
    bool hyrule = stage_is(h4, s4, gabi::guest_addr("Hyrule")) ||
                  stage_is(h1, s1, gabi::guest_addr("Hyroom")) ||
                  stage_is(h2, s2, gabi::guest_addr("kenroom"));
    if (hyrule && !dScnPlyAPI_checkEventBit(
                      gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)->mSave.mSavedata.mEvent),
                      0x3802)) /* COLORS_IN_HYRULE */
        mono = true;
    else
        mono = gabi::load<u8>(gabi::ea(dScnPly_gameInfo()->play.mCurStage.mName)) == 'X';
    if (mono) {
        dScnPlyAPI_enableMonotone(); /* mDoGph_gInf_c::onMonotone() */
        s16 rate = 400;
        if (stage_is(h3, s3, gabi::guest_addr("Hyrule")) &&
            (s8)dScnPly_gameInfo()->play.mCurStage.mLayer == 8)
            rate = -600;                   /* "Hyrule", layer 8 */
        l_monotoneRate.mValue = (u16)rate; /* setMonotoneRate */
        l_monotoneRateSpeed.mValue = 0;    /* setMonotoneRateSpeed(0) */
    } else {
        dScnPlyAPI_disableMonotone(); /* mDoGph_gInf_c::offMonotone() */
    }
    l_preLoad.preLoadNo = -1;       /* preLoadNo = -1 */
    if (l_preLoad.doPreLoad != 0) { /* doPreLoad */
        u32 stage = gabi::ea(&dScnPly_gameInfo()->play.mCurStage);
        for (u32 i = 0; i < 16; i++) {
            u32 a = stage, b = PreLoadInfoT.mEntries[i].stageName.v;
            u8 c1, c2;
            do {
                c1 = (u8)*gabi::at<be<u8>>(a++);
                c2 = (u8)*gabi::at<be<u8>>(b++);
            } while (c1 == c2 && c1 != 0);
            if (c1 == c2)
                l_preLoad.preLoadNo = (u8)i;
        }
    }
    gabi::at<dScnPly_Reset>(l_reset.mValue)->mReset = 0; /* mDoRst::offReset() */
    gabi::at<dScnPly_Reset>(l_reset.mValue)->mResetPrepare = 0;
    if ((s8)l_preLoad.preLoadNo < 0)
        return cPhs_COMPLEATE_e;
    u32 hi = dScnPlyAPI_getTimeHigh(); /* OSGetTime (import): r3:r4 */
    u32 lo = gabi::cpu->r[4];
    l_preLoadTimeHigh.mValue = hi; /* resPreLoadTime0 */
    l_preLoadTimeLow.mValue = lo;
    return cPhs_NEXT_e;
}
/* WWHD glue
str 10053364 "kenroom" in 025B1FF8
str 1005336C "GTower" in 025B1FF8
str 10053374 "Xboss0" in 025B1FF8
str 1005337C "Xboss1" in 025B1FF8
str 10053384 "Xboss2" in 025B1FF8
str 1005338C "Xboss3" in 025B1FF8
str 10053394 "Hyrule" in 025B1FF8
str 1005339C "Hyroom" in 025B1FF8
str 100533A4 "heap != (0)" in 025B1FF8
str 100533B0 "Particle" in 025B1FF8
str 100533BC "Particle.bfres" in 025B1FF8
str 100533CC "Pscene%03d.jpc" in 025B1FF8
str 100533DC "d_s_play.cpp" in 025B1FF8
str 100533EC "Darkness Spot" renamed in 025B1FF8
*/
