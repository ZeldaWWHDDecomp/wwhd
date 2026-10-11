/* Play scene part 3. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

static inline s32 dScnPly_getFreeHeapSize(u32 h) {
    return gabi::call_ptr<s32>(gabi::at<dScnPly_HeapObject>(h)->mVtable->getFreeSize, h);
} /* virtual */

/* 025B0C30 */
static BOOL heapSizeCheck() {
    s32 archive_free = dScnPly_getFreeHeapSize(dScnPlyAPI_getArchiveHeap());
    s32 archive_total_free = dScnPlyAPI_getTotalFreeHeapSize(dScnPlyAPI_getArchiveHeap());
    f32 at = (f32)archive_total_free;
    s32 archive_safe = dScnPlyAPI_getSafeArchiveHeapSize();
    f32 a1 = (f32)archive_free / at;
    f32 a2 = at / (f32)archive_safe;
    s32 game_free = dScnPly_getFreeHeapSize(dScnPlyAPI_getGameHeap());
    s32 game_total_free = dScnPlyAPI_getTotalFreeHeapSize(dScnPlyAPI_getGameHeap());
    f32 gt = (f32)game_total_free;
    s32 game_safe = dScnPlyAPI_getSafeGameHeapSize();
    f32 g1 = (f32)game_free / gt;
    f32 g2 = gt / (f32)game_safe;
    dScnPly_getFreeHeapSize(dScnPlyAPI_getZeldaHeap());
    f32 zt = (f32)dScnPlyAPI_getTotalFreeHeapSize(dScnPlyAPI_getZeldaHeap());
    f32 z = zt / (f32)dScnPlyAPI_getSafeZeldaHeapSize();
    dScnPly_getFreeHeapSize(dScnPlyAPI_getCommandHeap());
    f32 ct = (f32)dScnPlyAPI_getTotalFreeHeapSize(dScnPlyAPI_getCommandHeap());
    f32 c = ct / (f32)dScnPlyAPI_getSafeCommandHeapSize();
    if (a2 < 0.7f || a1 < 0.7f || g2 < 0.7f || g1 < 0.7f || z < 0.7f || c < 0.7f)
        return FALSE; /* bge on NaN: a NaN last ratio passes */
    return TRUE;
}

/* the HD scene system's entry for the current index (a bounded table lookup), or NULL */
/* inlined into phase_00 */
static u32 scene_entry(dScnPly::SceneSystem *sys, be<s16> *idxPtr) {
    u32 idx = (u32)(s32)*idxPtr;
    u32 n1 = sys->mEntryCount;
    dScnPly::SceneEntry *arr = sys->mpEntries;
    dScnPly::SceneEntry *entry = idx < n1 ? arr + idx : arr;
    if (entry->mIsValid == 0)
        return 0;
    u32 n2 = sys->mObjectCount;
    entry = idx < n1 ? arr + idx : arr;
    u32 k = entry->mObjectIndex;
    gptr<void> *object = nullptr;
    if (k < n2)
        object = sys->mpObjects + k;
    // HD: an invalid index retains the original read from guest address zero.
    return object->v;
}

// HD: see the file comment
/* 025B0FB4 */
static s32 phase_00(dScnPly::dScnPly_ply_c *i_this) {
    if (l_displayReady.mValue == 0) {
        dScnPlyAPI_prepareDisplayHeap(dScnPlyAPI_getDisplayAllocator());
        u32 x = dScnPlyAPI_getDisplayAllocator();
        dScnPlyAPI_attachDisplayHeap(l_displaySystem.mValue, x);
        dScnPlyAPI_initializeDisplayResources(l_displaySystem.mValue);
    }
    if (dScnPlyAPI_checkStreamHeap())
        return cPhs_INIT_e; /* mDoAud_isUsedHeapForStreamBuffer */
    if (i_this != 0)
        dScnPlyAPI_constructPlayScene(
            gabi::ea(i_this)); /* dScnPly_ply_c::dScnPly_ply_c (in place) */
    gabi::Local<dScnPly_SceneDescription> desc;
    u32 d = gabi::ea(desc.get());
    dScnPlyAPI_constructSceneDescription(d);
    dScnPlyAPI_appendSceneDescription(d, (u32)(s32)*gabi::at<be<s16>>(l_sceneBaseIndex.mValue), 1);
    dScnPlyAPI_appendSceneDescription(d, (u32)(s32)*gabi::at<be<s16>>(l_sceneResourceIndex.mValue),
                                      8);
    dScnPlyAPI_appendSceneDescription(
        d, (u32)(s32)*gabi::at<be<s16>>(l_sceneDescriptionIndex.mValue), 1);
    u32 idxPtr = l_sceneIndex.mValue;
    dScnPlyAPI_appendSceneDescription(d, (u32)(s32)*gabi::at<be<s16>>(idxPtr), 1);
    u32 sys = gabi::ea(&i_this->mSceneSystem);
    u32 heap = dScnPlyAPI_getCurrentHeap_u320();
    dScnPlyAPI_initializeSceneSystem(sys, d, heap);
    gabi::Local<dScnPly_SafeString> a, b;
    a->mVtable = gc_tu.safeString;
    b->mVtable = gc_tu.safeString;
    a->mString = gabi::guest_addr("Misc");
    b->mString = gabi::guest_addr("Misc.bfres");
    dScnPlyAPI_getResource_void4(l_resourceSystem.mValue, gabi::ea(a.get()), gabi::ea(b.get()), 0);
    u32 r = dScnPlyAPI_getResourceFile();
    u32 off = gabi::at<dScnPly_ResourceFile>(r)->mFileDictionaryOffset;
    u32 p = off != 0 ? gabi::ea(&gabi::at<dScnPly_ResourceFile>(r)->mFileDictionaryOffset) + off
                     : 0; /* relative pointer */
    u32 q = dScnPlyAPI_findResource(p, gabi::guest_addr("env.baglenv"));
    u32 off2 = *gabi::at<be<u32>>(q);
    u32 p2 = off2 != 0 ? q + off2 : 0;
    gabi::Local<dScnPly_ModelHandle> out;
    u32 res = dScnPlyAPI_resolveSceneModel(gabi::ea(out.get()), p2);
    u32 v = gabi::at<dScnPly_ModelHandle>(res)->mValue;
    gabi::Local<dScnPly_ModelPair> pair;
    pair->mSecond = v;
    pair->mFirst = v;
    dScnPlyAPI_bindSceneModel(sys, gabi::ea(&pair->mFirst), gabi::ea(&pair->mSecond), (u32)-1,
                              1.0f);
    u32 obj = scene_entry(gabi::at<dScnPly::SceneSystem>(sys), gabi::at<be<s16>>(idxPtr));
    if (l_sceneTypeGuard.mInitialized == 0) { /* function-local static */
        l_sceneTypeGuard.mInitialized = 1;
        l_sceneType.mVtable = play_tu.mSceneTypeVtable;
    }
    if (obj != 0 &&
        gabi::call_ptr<u32>(
            gabi::at<dScnPly_SceneObject>(obj)->mRuntimeTypeVtable->checkDerivedRuntimeTypeInfo,
            obj, &l_sceneType) == 0)
        obj = 0;
    dScnPlyAPI_enableSceneObject(obj, 1);
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[14] = 1.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mInitializationFlags = 1;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[26] = 1.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[22] = 10000.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[18] = 1.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[34] = 1000.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[7] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[1] = 8000.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[8] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[6] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[0] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[12] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[2] = 0.0f;
    gabi::at<dScnPly_SceneObject>(obj)->mRenderParameters[13] = 0.0f;
    dScnPlyAPI_updateSceneSystem(sys);
    l_activeSceneSystem.mValue = sys;
    i_this->mWindSystem.mFlags = i_this->mWindSystem.mFlags | 2;
    i_this->mWindSystem.mStateWords[1] = 0;
    i_this->mWindSystem.mSpeedX = 15.0f;
    i_this->mWindSystem.mSpeedZ = 15.0f;
    i_this->mWindSystem.mStateWords[0] = 0;
    i_this->mWindSystem.mWindX = 0.02f;
    i_this->mWindSystem.mStateWords[2] = 0;
    i_this->mWindSystem.mWindZ = 0.01f;
    i_this->mWindSystem.mIsEnabled = 1;
    dScnPlyAPI_resetControllerSystem(l_controllerSystem.mValue);
    l_graphicsResetState.mValue = 0;
    if (i_this != 0 &&
        (s16)i_this->mProcName == dScnPly::fpcNm_PLAY_SCENE_e) { /* fpcNm_PLAY_SCENE */
        dScnPlyAPI_checkHeapSize();
        return cPhs_NEXT_e;
    }
    dScnPlyAPI_checkHeapSize();
    if (gabi::at<dScnPly_Reset>(l_reset.mValue)->mReset != 0) { /* mDoRst::isReset() */
        dScnPlyAPI_recoverAudioReset();                         /* audio reset recovery */
        l_fadeEnabled.mValue = 0;                               /* mDoGph_gInf_c::offFade() */
        dScnPlyAPI_disableMonotone();                           /* mDoGph_gInf_c::offMonotone() */
        l_wipeEnabled.mValue = 0;                               /* dDlst_list_c::offWipe() */
        dScnPlyAPI_enableTitleKirakiraSound(); /* daTitle_proc_c::daTitle_Kirakira_Sound_flag_on() */
        dScnPlyAPI_resetTribox();              /* daObjTribox::Act_c::reset() */
        dScnPly_gameInfo()->play.mNextStage.enable = 0; /* dComIfGp_offEnableNextStage() */
    }
    dScnPlyAPI_initializeSave(
        gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)->mSave)); /* dComIfGs_init() */
    return cPhs_NEXT_e;
}
/* WWHD glue
str 1005326C "Misc" in 025B0FB4
str 10053274 "env.baglenv" in 025B0FB4
str 10053280 "Misc.bfres" in 025B0FB4
*/
