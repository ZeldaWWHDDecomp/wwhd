/* Play scene part 7. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

static inline void assure(u32 s) {
    gabi::call_ptr(
        gabi::at<dScnPly_StringVtable>(gabi::at<dScnPly_SafeString>(s)->mVtable)->assureTermination,
        s);
}

/* stage name == literal (literal assured twice, then the stage; pointer test, then at most
 * 0x40001 characters) */
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

/* 025B0630 */
static BOOL dScnPly_Delete(dScnPly::dScnPly_ply_c *i_this) {
    u32 sys = l_displaySystem.mValue;
    if (sys != 0) {
        if (gabi::at<dScnPly_DisplaySystem>(sys)->mState == 1)
            return FALSE;
        dScnPlyAPI_releaseDisplayResources(sys);
        u32 q = l_displaySystem.mValue;
        u32 st80 = gabi::at<dScnPly_DisplaySystem>(q)->mpHeapState->mState;
        if (st80 == 3 || st80 == 4)
            dScnPlyAPI_releaseDisplayHeap(dScnPlyAPI_getDisplayHeap(q));
    }
    dScnPlyAPI_destroyAttention(gabi::ea(&dScnPly_gameInfo()->play.mAttention),
                                2); /* dComIfGp_getAttention().~dAttention_c() */
    dScnPlyAPI_destroyVibration(gabi::ea(&dScnPly_gameInfo()->play.mVibration),
                                2); /* dComIfGp_getVibration().~dVibration_c() */
    u32 play = dComIfGp_ea();
    gabi::call_ptr(
        gabi::at<dScnPly_Background>(play + offsetof(dComIfG_inf_c, play))->mVtable->destroy,
        gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play)); /* dComIfG_Bgsp()->Dt() */
    u32 r = dScnPlyAPI_destroyCollision(
        gabi::ea(&dScnPly_gameInfo()->play.mCcS)); /* dComIfG_Ccsp()->Dt() */
    r = dScnPlyAPI_deleteSnapshot(r);              /* dSnap_Delete() */
    dScnPlyAPI_deleteStage(r);                     /* dStage_Delete() */
    dScnPlyAPI_removeEvent(
        gabi::ea(&dScnPly_gameInfo()->play.mEvtCtrl)); /* dComIfGp_event_remove() */
    r = dScnPlyAPI_removeParticleScene(
        dScnPly_gameInfo()->play.mParticle.v); /* dComIfGp_particle_removeScene() */
    r = dScnPlyAPI_removeDemo(r);              /* dComIfGp_removeDemo() */
    dScnPlyAPI_deleteEnvironment(r);
    dScnPlyAPI_destroyMessageHeap(
        gabi::at<dScnPly_GameHeapInfo>(dComIfGp_ea())
            ->mExpHeap2D); /* fopMsgM_destroyExpHeap(dComIfGp_getExpHeap2D()) */
    gabi::Local<dScnPly_SafeString> l0, s0, l1, s1, l2, s2, l3, s3;
    if (stage_is(l0, s0, gabi::guest_addr("Xboss0")) ||
        stage_is(l1, s1, gabi::guest_addr("Xboss1")) ||
        stage_is(l2, s2, gabi::guest_addr("Xboss2")) ||
        stage_is(l3, s3, gabi::guest_addr("Xboss3"))) /* "Xboss0".."Xboss3" */
        dScnPlyAPI_restorePlayerRecollection();       /* dComIfGs_revPlayerRecollectionData() */
    dScnPlyAPI_removeMagma(gabi::ea(&dScnPly_gameInfo()->play));  /* dComIfGp_removeMagma() */
    dScnPlyAPI_removeGrass(gabi::ea(&dScnPly_gameInfo()->play));  /* removeGrass */
    dScnPlyAPI_removeTree(gabi::ea(&dScnPly_gameInfo()->play));   /* removeTree */
    dScnPlyAPI_removeWood(gabi::ea(&dScnPly_gameInfo()->play));   /* removeWood */
    dScnPlyAPI_removeFlower(gabi::ea(&dScnPly_gameInfo()->play)); /* removeFlower */
    play = dComIfGp_ea();
    gabi::at<dScnPly_GameInfo>(play)->play.mAirMeter = 0; /* dComIfGp_clearItemTimeCount() */
    gabi::at<dScnPly_GameInfo>(play)->play.mItemSwimTimerStatus = 0;
    s32 darkNo = (s8)g_darkHIO.mNo;
    g_msgDHIO.field_0x10 = (u32)-1;              /* g_msgDHIO.field_0x10 */
    g_msgDHIO.field_0x06 = 0;                    /* g_msgDHIO.field_0x06 */
    dScnPlyAPI_deleteHIOChild(darkNo);           /* mDoHIO_deleteChild(g_darkHIO.mNo) */
    dScnPlyAPI_destroyOceanHeap();               /* the HD ocean heap */
    dScnPly_gameInfo()->play.mDlstWindowNum = 0; /* dComIfGp_setWindowNum(0) */
    s8 preLoadNo = (s8)l_preLoad.preLoadNo;
    if (preLoadNo >= 0) {
        const dScnPly_PreLoadInfo *info = &PreLoadInfoT.mEntries[preLoadNo];
        u32 resName = info->resName.v;
        u32 num = info->resNum;
        if (resName != 0 && (u32)*gabi::at<be<u32>>(resName) != 0 && (s32)num > 0) {
            u32 p = resName, off = 0;
            do {
                dScnPlyAPI_deleteResource(
                    gabi::ea(&resPhase.mPhases[off / sizeof(request_of_phase_process_class)]),
                    (u32)*gabi::at<be<u32>>(p)); /* dComIfG_resDelete(&resPhase[i], resName[i]) */
                p += 4;
                off += 8;
            } while (--num != 0);
        }
    }
    dScnPlyAPI_initializePlay(gabi::ea(&dScnPly_gameInfo()->play)); /* dComIfGp_init() */
    u32 g = l_sceneRenderer.mValue;
    l_activeSceneSystem.mValue = 0;
    u32 n = gabi::at<dScnPly_Renderer>(g)->mViewCount;
    u32 pp = gabi::at<dScnPly_Renderer>(g)->mpViews.v;
    if (n > 1)
        pp += 4;
    u32 o = gabi::at<gptr<dScnPly_RenderView>>(pp)->v;
    gabi::at<dScnPly_RenderView>(o)->mWindSystem = 0;
    gabi::at<dScnPly_RenderView>(o)->mWindController = 0;
    u32 mode = i_this->mMode;
    if (mode == dScnPly::MODE_TITLE) {
        dScnPlyAPI_destroyTitleSystem(dScnPlyAPI_finalizeTitleSystem(l_titleSystem.mValue));
    } else if (mode == dScnPly::MODE_GAME) {
        u32 x = dScnPlyAPI_finalizeGameSystem(l_gameSystem.mValue);
        if (l_keepGameSystem.mValue == 0)
            dScnPlyAPI_destroyGameSystem(x);
    } else if (mode == dScnPly::MODE_ENDING) {
        dScnPlyAPI_destroyEndingSystem(dScnPlyAPI_finalizeEndingSystem(l_endingSystem.mValue));
        dScnPlyAPI_releaseEndingResources(l_resourceLoader.mValue);
    } else if (mode == dScnPly::MODE_OCEAN_ENDING) {
        dScnPlyAPI_destroySeaEndingSystem(
            dScnPlyAPI_finalizeSeaEndingSystem(l_seaEndingSystem.mValue));
    }
    dScnPlyAPI_destroyWindSystem(gabi::ea(&i_this->mWindSystem), 2);
    dScnPlyAPI_destroySceneSystem(gabi::ea(&i_this->mSceneSystem), 2);
    dScnPlyAPI_destroySceneBase(gabi::ea(i_this), 2); /* scene_class base destructor */
    return TRUE;
}
/* WWHD glue
str 10053228 "Xboss0" in 025B0630
str 10053230 "Xboss1" in 025B0630
str 10053238 "Xboss2" in 025B0630
str 10053240 "Xboss3" in 025B0630
*/
