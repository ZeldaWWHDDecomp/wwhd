/* Play scene part 5. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

static inline void assure(u32 s) {
    gabi::call_ptr(
        gabi::at<dScnPly_StringVtable>(gabi::at<dScnPly_SafeString>(s)->mVtable)->assureTermination,
        s);
}

/* strcmp of the stage SafeString with a literal SafeString (pointer test, then at most 0x40001
 * characters); `twice` = the stage string is assured twice (once otherwise) */
static bool same(u32 stage, u32 lit, bool twice) {
    assure(stage);
    if (twice)
        assure(stage);
    u32 s1 = (u32)*gabi::at<be<u32>>(stage);
    assure(lit);
    u32 s2 = (u32)*gabi::at<be<u32>>(lit);
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

static inline u32 relativeResource(be<u32> *field) {
    u32 offset = *field;
    return offset != 0 ? gabi::ea(field) + offset : 0;
}

/* 025B168C */
static s32 phase_2(dScnPly::dScnPly_ply_c *i_this) {
    s32 rt =
        dScnPlyAPI_syncStageResource(gabi::guest_addr("Stage")); /* dComIfG_syncStageRes("Stage") */
    if (rt > 0)
        return cPhs_INIT_e;
    if (rt < 0) {
        dScnPlyAPI_showAssert(gabi::guest_addr("d_s_play.cpp"), 0x1063,
                              gabi::guest_addr("rt >= 0"));
        return cPhs_INIT_e;
    }
    gabi::Local<dScnPly_SafeString> stage, l1, l2, l3, l4, l5, l6, l7, l8, l9;
    u32 S = gabi::ea(stage.get());
    stage->mString =
        gabi::ea(&dScnPly_gameInfo()->play.mCurStage); /* dComIfGp_getStartStageName() */
    stage->mVtable = gc_tu.safeString;
    i_this->mMode = dScnPly::MODE_GAME;
    if (i_this != 0 && (s16)i_this->mProcName == dScnPly::fpcNm_OPENING2_SCENE_e) {
        i_this->mMode = dScnPly::MODE_TITLE;
        if (dScnPlyAPI_loadSystem(l_resourceLoader.mValue, 1) == 0)
            return cPhs_INIT_e;
    } else {
        l1->mVtable = gc_tu.safeString;
        l1->mString = gabi::guest_addr("ENDumi"); /* "ENDumi" */
        dScnPlyAPI_assureSafeString(S);
        if (same(S, gabi::ea(l1.get()), false)) {
            i_this->mMode = dScnPly::MODE_ENDING;
            dScnPlyAPI_prepareEndingSystem(l_resourceLoader.mValue);
            if (dScnPlyAPI_loadSystem(l_resourceLoader.mValue, 5) == 0)
                return cPhs_INIT_e;
        } else {
            l2->mVtable = gc_tu.safeString;
            l2->mString = gabi::guest_addr("sea_E"); /* "sea_E" */
            if (same(S, gabi::ea(l2.get()), true)) {
                i_this->mMode = dScnPly::MODE_OCEAN_ENDING;
                if (dScnPlyAPI_loadSystem(l_resourceLoader.mValue, 6) == 0)
                    return cPhs_INIT_e;
            }
        }
    }
    /* the ocean stages */
    l3->mVtable = gc_tu.safeString;
    l3->mString = gabi::guest_addr("sea"); /* "sea" */
    i_this->mIsOcean = 0;
    bool ocean = same(S, gabi::ea(l3.get()), true);
    if (!ocean) {
        l4->mVtable = gc_tu.safeString;
        l4->mString = gabi::guest_addr("ADMumi");
        ocean = same(S, gabi::ea(l4.get()), true);
    } /* "ADMumi" */
    if (!ocean) {
        l5->mVtable = gc_tu.safeString;
        l5->mString = gabi::guest_addr("A_umikz");
        ocean = same(S, gabi::ea(l5.get()), true);
    } /* "A_umikz" */
    if (!ocean) {
        l6->mVtable = gc_tu.safeString;
        l6->mString = gabi::guest_addr("ENDumi");
        ocean = same(S, gabi::ea(l6.get()), true);
    } /* "ENDumi" */
    if (!ocean) {
        l7->mVtable = gc_tu.safeString;
        l7->mString = gabi::guest_addr("sea_T");
        ocean = same(S, gabi::ea(l7.get()), true);
    } /* "sea_T" */
    if (!ocean) {
        l8->mVtable = gc_tu.safeString;
        l8->mString = gabi::guest_addr("sea_E");
        ocean = same(S, gabi::ea(l8.get()), true);
    } /* "sea_E" */
    if (ocean) {
        gabi::Local<dScnPly_SafeString> res;
        res->mVtable = gc_tu.safeString;
        res->mString = gabi::guest_addr("sea_Stage"); /* "sea_Stage" */
        u32 model_res = dScnPlyAPI_getModelResource(l_resourceManager.mValue, gabi::ea(res.get()));
        if (model_res == 0)
            dScnPlyAPI_showAssert(gabi::guest_addr("d_s_play.cpp"), 0x109E,
                                  gabi::guest_addr("model_res != (0)"));
        res->mVtable = gc_tu.safeString;
        res->mString = gabi::guest_addr("ww_shadow_clouds"); /* "ww_shadow_clouds" */
        u32 file = gabi::at<dScnPly_ResourceHandle>(model_res)->mpFile.v;
        dScnPlyAPI_assureSafeString(gabi::ea(res.get()));
        u32 dict = relativeResource(&gabi::at<dScnPly_ResourceFile>(file)->mModelDictionaryOffset);
        s32 idx = dScnPlyAPI_findResourceIndex(dict, (u32)res->mString);
        if (idx != -1) {
            u32 dict2 = relativeResource(
                &gabi::at<dScnPly_ResourceHandle>(model_res)->mpFile->mModelDictionaryOffset);
            u32 model = relativeResource(
                &gabi::at<dScnPly_Dictionary>(dict2)->mEntries[idx].mResourceOffset);
            dScnPlyAPI_beginResourceAccess(l_resourceAccess.mValue);
            dScnPlyAPI_initializeWindModel(gabi::ea(&i_this->mWindModel), model);
            dScnPlyAPI_endResourceAccess(l_resourceAccess.mValue);
            u32 w = gabi::ea(&i_this->mWindSystem);
            bool same_cfg =
                i_this->mWindSystem.mBindingWords[1] == i_this->mWindModel.mBindingWords[0] &&
                i_this->mWindSystem.mBindingWords[2] == i_this->mWindModel.mBindingWords[1] &&
                i_this->mWindSystem.mBindingWords[3] == i_this->mWindModel.mBindingWords[2] &&
                i_this->mWindSystem.mBindingWords[4] == i_this->mWindModel.mBindingWords[3] &&
                i_this->mWindSystem.mBindingWords[5] == i_this->mWindModel.mBindingWords[4] &&
                i_this->mWindSystem.mBindingWords[6] == i_this->mWindModel.mBindingWords[5] &&
                i_this->mWindSystem.mBindingWords[14] == i_this->mWindModel.mBindingWords[13] &&
                i_this->mWindSystem.mBindingWords[13] == i_this->mWindModel.mBindingWords[12] &&
                i_this->mWindSystem.mBindingWords[7] == i_this->mWindModel.mBindingWords[6];
            if (same_cfg) {
                u32 a = i_this->mWindModel.mBindingWords[9],
                    b = i_this->mWindModel.mBindingWords[11];
                i_this->mWindSystem.mBindingWords[10] = a;
                i_this->mWindSystem.mBindingWords[55] = b;
                i_this->mWindSystem.mBindingWords[53] = a;
                i_this->mWindSystem.mBindingWords[12] = b;
            } else {
                dScnPlyAPI_bindWindModel(gabi::ea(i_this->mWindSystem.mBindingWords), gabi::ea(&i_this->mWindModel));
            }
            u32 g = l_sceneRenderer.mValue;
            u32 n = gabi::at<dScnPly_Renderer>(g)->mViewCount;
            u32 p = gabi::at<dScnPly_Renderer>(g)->mpViews.v;
            if (n > 1)
                p += 4;
            u32 o = gabi::at<gptr<dScnPly_RenderView>>(p)->v;
            gabi::at<dScnPly_RenderView>(o)->mWindController = gabi::ea(&i_this->mWindController);
            gabi::at<dScnPly_RenderView>(o)->mWindSystem = w;
            i_this->mIsOcean = 1;
        }
    }
    l9->mVtable = gc_tu.safeString;
    l9->mString = gabi::guest_addr("GanonK"); /* "GanonK" */
    bool ganon = same(S, gabi::ea(l9.get()), true);
    gabi::at<dScnPly_Environment>(l_environment.mValue)->mOceanShadowEnabled = ganon ? 0 : 1;
    dScnPlyAPI_createStageInfo(); /* dStage_infoCreate() */
    return cPhs_NEXT_e;
}
/* WWHD glue
str 100532D4 "rt >= 0" in 025B168C
str 100532DC "A_umikz" in 025B168C
str 100532E4 "sea" in 025B168C
str 100532E8 "Stage" in 025B168C
str 100532F0 "sea_E" in 025B168C
str 100532F8 "sea_T" in 025B168C
str 10053300 "ENDumi" in 025B168C
str 10053308 "ADMumi" in 025B168C
str 10053310 "GanonK" in 025B168C
str 10053318 "sea_Stage" in 025B168C
str 10053324 "d_s_play.cpp" in 025B168C
str 10053334 "model_res != (0)" in 025B168C
str 10053348 "ww_shadow_clouds" in 025B168C
*/
