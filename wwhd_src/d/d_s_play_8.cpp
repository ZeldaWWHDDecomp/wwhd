/* Play scene part 8. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

static inline void assure(u32 s) {
    gabi::call_ptr(
        gabi::at<dScnPly_StringVtable>(gabi::at<dScnPly_SafeString>(s)->mVtable)->assureTermination,
        s);
}

/* the next stage name (play+0x5140) == literal (literal assured twice, then the stage name) */
static bool next_is(gabi::Local<dScnPly_SafeString> &lit, gabi::Local<dScnPly_SafeString> &stage,
                    u32 str) {
    lit->mString = str;
    lit->mVtable = gc_tu.safeString;
    u32 play = dComIfGp_ea();
    stage->mVtable = gc_tu.safeString;
    stage->mString = gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play.mNextStage);
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

static inline s32 wipe() {
    return (s8)dScnPly_gameInfo()->play.mNextStage.wipe;
} /* dComIfGp_getNextStageWipe() */

static inline void set_scene_name() { /* mDoAud_setSceneName(next stage, room, layer) */
    u32 name = gabi::ea(&dScnPly_gameInfo()->play.mNextStage);
    s32 room = (s8)dScnPly_gameInfo()->play.mNextStage.mRoomNo;
    s32 layer = (s8)dScnPly_gameInfo()->play.mNextStage.mLayer;
    dScnPlyAPI_setAudioSceneName(name, room, layer);
}

/* the HD system's two virtual results compared (same pattern as dScnPly_isPause) */
static inline bool sys_is(u32 q, u32 obj) {
    u32 s = dScnPlyAPI_getSystemState(gabi::ea(&gabi::at<dScnPly_StateContainer>(q)->mState));
    u32 vt = gabi::at<dScnPly_SystemState>(obj)->mVtable.v;
    u32 a = gabi::call_ptr<u32>(gabi::at<dScnPly_SystemState>(s)->mVtable->queryState, s);
    u32 b = gabi::call_ptr<u32>(gabi::at<dScnPly_StateVtable>(vt)->queryState, obj);
    return a == b;
}

/* the stage change part; returns through the per-frame part */
static void change_stage(dScnPly::dScnPly_ply_c *i_this) {
    s32 w = wipe();
    if ((u32)w >= 0xC)
        dScnPlyAPI_showAssert(
            gabi::guest_addr("d_s_play.cpp"), 0x4C2,
            gabi::guest_addr(
                "dComIfGp_getNextStageWipe() < (sizeof(l_wipeType) / sizeof(l_wipeType[0]))"));
    gabi::Local<dScnPly_SafeString> l0, s0, l1, s1, l2, s2, l3, s3, l4, s4;
    if (next_is(l0, s0, gabi::guest_addr("sea_E"))) { /* "sea_E" */
        u32 q = l_endingSystem.mValue;
        if (q == 0)
            return;
        u32 s = dScnPlyAPI_getSystemState(gabi::ea(&gabi::at<dScnPly_StateContainer>(q)->mState));
        u32 vt1 = l_secondPauseState.mVtable.v;
        u32 a = gabi::call_ptr<u32>(gabi::at<dScnPly_SystemState>(s)->mVtable->queryState, s);
        u32 b = gabi::call_ptr<u32>(gabi::at<dScnPly_StateVtable>(vt1)->queryState,
                                    gabi::ea(&l_secondPauseState));
        if (a == b) {
            dScnPlyAPI_requestSceneChange(gabi::ea(i_this), dScnPly::fpcNm_PLAY_SCENE_e, 0, 5,
                                          1); /* fopScnM_ChangeReq */
            dScnPlyAPI_setWipeColor(gabi::ea(&l_wipeWhite));
            dScnPlyAPI_stopBgm(0x1E);
            set_scene_name();
            return;
        }
        u32 q2 = l_endingSystem.mValue;
        if (sys_is(q2, gabi::ea(&l_firstPauseState)))
            return;
        u32 s2b =
            dScnPlyAPI_getSystemState(gabi::ea(&gabi::at<dScnPly_StateContainer>(q2)->mState));
        gabi::call_ptr<u32>(gabi::at<dScnPly_SystemState>(s2b)->mVtable->queryState, s2b);
        gabi::call_ptr<u32>(gabi::at<dScnPly_StateVtable>(vt1)->queryState,
                            gabi::ea(&l_secondPauseState));
        return;
    }
    if (next_is(l1, s1, gabi::guest_addr("ENDING")))
        return; /* "ENDING" */
    s32 type = wipe();
    bool night;
    bool requestWipe = true;
    if (type == 4) {
        if (next_is(l2, s2, gabi::guest_addr("sea"))) { /* "sea" */
            dScnPlyAPI_requestSceneChange(gabi::ea(i_this), dScnPly::fpcNm_PLAY_SCENE_e,
                                          (u32)(s32)(s16)l_wipeType.mTypes[9], 5, 0);
            type = 9;
            requestWipe = false;
        } else {
            type = next_is(l3, s3, gabi::guest_addr("Obombh")) ? 8 : 3;
        } /* "Obombh" */
    }
    if (requestWipe)
        dScnPlyAPI_requestSceneChange(gabi::ea(i_this), dScnPly::fpcNm_PLAY_SCENE_e,
                                      (u32)(s32)(s16)l_wipeType.mTypes[type], 5,
                                      0); /* l_wipeType[type] */
    {
        s32 hour = dScnPlyAPI_getDaytimeHour(); /* dKy_getdaytime_hour() */
        if (dScnPlyAPI_checkEventNightStop())   /* dKy_checkEventNightStop() */
            night = true;
        else
            night = !((u32)(hour - 6) < 12);
    }
    bool day_wipe;
    if (wipe() == 1 || wipe() == 2 || wipe() == 7) {
        day_wipe = true;
    } else {
        day_wipe = false;
        if (wipe() == 8 || wipe() == 0xA || type == 8) {
            if (night)
                day_wipe = true;
        }
        if (!day_wipe && (wipe() == 9 || wipe() == 0xB || type == 9)) {
            if (!night)
                day_wipe = true;
        }
    }
    dScnPlyAPI_setWipeColor(day_wipe ? gabi::ea(&l_wipeWhite) : gabi::ea(&l_wipeBlack));
    if (!next_is(l4, s4, gabi::guest_addr("ENDumi")))
        l_keepGameSystem.mValue = 1; /* "ENDumi" */
    set_scene_name();
}

/* 025AF8A0 */
static BOOL dScnPly_Draw(dScnPly::dScnPly_ply_c *i_this) {
    {
        gabi::Local<dScnPly_ViewMatrix> view;
        for (int i = 0; i < 12; i++)
            view->m[i] = (f32)l_j3dSystem.mViewMatrix[i]; /* j3dSys view matrix */
        dScnPlyAPI_drawSceneSystem(gabi::ea(&i_this->mSceneSystem), gabi::ea(view.get()),
                                   gabi::ea(l_j3dSystem.mProjectionMatrix), 0);
    }
    dScnPlyAPI_moveCollision(gabi::ea(&dScnPly_gameInfo()->play.mCcS)); /* dComIfG_Ccsp()->Move() */
    dScnPlyAPI_clearBackgroundMoveFlags(
        gabi::ea(&dScnPly_gameInfo()->play)); /* dComIfG_Bgsp()->ClrMoveFlag() */
    if (!dScnPlyAPI_checkOverlap() && !dScnPlyAPI_resetToOpening(gabi::ea(i_this)) && i_this != 0 &&
        (s16)i_this->mProcName == dScnPly::fpcNm_PLAY_SCENE_e &&
        (s8)dScnPly_gameInfo()->play.mNextStage.enable !=
            0) /* !IsPeek, !resetToOpening, PLAY_SCENE, isEnableNextStage */
        change_stage(i_this);
    bool paused = dScnPlyAPI_checkMenu() || dScnPlyAPI_checkPause();
    if (!paused) {
        dScnPlyAPI_runVibration(
            gabi::ea(&dScnPly_gameInfo()->play.mVibration)); /* dVibration_c::Run */
        dScnPlyAPI_calculateMagma(gabi::ea(&dScnPly_gameInfo()->play));
        if (dScnPly_gameInfo()->play.mpGrassPacket != 0 ||
            dScnPly_gameInfo()->play.mpTreePacket != 0 ||
            dScnPly_gameInfo()->play.mpWoodPacket != 0 ||
            dScnPly_gameInfo()->play.mpFlowerPacket != 0 || dScnPlyAPI_checkPoisonCollision())
            dScnPlyAPI_prepareMassCollision(gabi::ea(&reinterpret_cast<dScnPly_CollisionSystem*>(&dScnPly_gameInfo()->play.mCcS)->mMassManager)); /* dCcMassS_Mng::Prepare */
        dScnPlyAPI_calculateGrass(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_calculateTree(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_calculateWood(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_calculateFlower(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_calculatePoisonCollision(); /* dKyr_poison_light_colision */
        u32 play = dComIfGp_ea();
        gabi::call_ptr(
            gabi::at<dScnPly_Background>(play + offsetof(dComIfG_inf_c, play))->mVtable->move,
            gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play)); /* dComIfG_Bgsp() virtual +0x24 */
        dScnPlyAPI_executeSnapshot();
        u32 pl = dScnPly_gameInfo()->play.mpPlayerPtr[0].v;
        if (pl == 0 || !dScnPlyAPI_checkGameOverStart(pl)) /* daPy_lk_c::checkGameOverStart */
            dScnPlyAPI_calculateParticles(dScnPly_gameInfo()->play.mParticle.v);
        dScnPlyAPI_drawParticleDirect(dScnPly_gameInfo()->play.mParticle.v);
        l_particleFrame.mValue = l_particleFrame.mValue + 1;
    } else {
        dScnPlyAPI_pauseVibration(
            gabi::ea(&dScnPly_gameInfo()->play.mVibration)); /* dVibration_c::Pause */
    }
    dScnPlyAPI_clearCollision(gabi::ea(&dScnPly_gameInfo()->play.mCcS));
    dScnPlyAPI_drawParticleIndirect(dScnPly_gameInfo()->play.mParticle.v);
    if (i_this->mMode == dScnPly::MODE_ENDING)
        dScnPlyAPI_drawEndingSystem(l_endingSystem.mValue);
    dScnPlyAPI_processMessageHIO(
        gabi::ea(&g_msgDHIO)); /* g_msgDHIO.dScnPly_msg_HIO_messageProc() */
    dScnPlyAPI_checkRoomDrawArea(
        gabi::ea(&dScnPly_gameInfo()->play.mRoomCtrl)); /* dStage_roomControl_c::checkDrawArea */
    for (u32 p = dScnPlyAPI_beginDrawQueue(); p != 0; p = dScnPlyAPI_nextDrawQueue(p))
        dScnPlyAPI_drawProcess(gabi::at<dScnPly_DrawTag>(p)->mData); /* the HD draw list */
    if (!dScnPlyAPI_checkMenu()) {
        dScnPlyAPI_drawParticleModels(
            gabi::at<dScnPly_Particle>(dScnPly_gameInfo()->play.mParticle.v)
                ->mpModelControl); /* dPa_modelControl_c::draw */
        dScnPlyAPI_drawMagma(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_drawGrass(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_drawTree(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_drawWood(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_drawFlower(gabi::ea(&dScnPly_gameInfo()->play));
        dScnPlyAPI_drawCollision(gabi::ea(&dScnPly_gameInfo()->play.mCcS)); /* dCcS::Draw */
        dScnPlyAPI_drawSnapshot(dComIfGp_ea());
        dScnPlyAPI_drawAttention(
            gabi::ea(&dScnPly_gameInfo()->play.mAttention)); /* dAttention_c::Draw */
    }
    return TRUE;
}
/* WWHD glue
str 1005319C "sea" in 025AF8A0
str 100531A0 "sea_E" in 025AF8A0
str 100531A8 "ENDING" in 025AF8A0
str 100531B0 "Obombh" in 025AF8A0
str 100531B8 "ENDumi" in 025AF8A0
str 100531C0 "d_s_play.cpp" in 025AF8A0
str 100531D0 "dComIfGp_getNextStageWipe() < (sizeof(l_wipeType) / sizeof(l_wipeType[0]))" in 025AF8A0
*/
