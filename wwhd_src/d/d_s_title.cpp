/**
 * d_s_title.cpp
 * Title scene - reset and transition to the opening scene.
 * WWHD (cking.rpx), verified against the recompiled original (unit d_s_title).
 */
#include "d/d_s_title.h"

typedef gptr<hdTitleReset_c> TitleResetPtr;
typedef gptr<hdInputCtrl_c> TitleInputPtr;
typedef gptr<hdTitleSaveMgr_c> TitleSaveMgrPtr;
static TitleResetPtr l_reset;
static TitleInputPtr l_input;
static TitleSaveMgrPtr l_saveMgr;
static GXColor g_blackColor;

enum {
    fpcNm_OVERLAP0_e = 0,
    fpcNm_LOGO_SCENE_e = 5,
    fpcNm_OPENING_SCENE_e = 8,
    fpcNm_ENDING_SCENE_e = 11,
};

/* 025ABF00 */
static u32 dScnTitle_Execute(title_of_scene_class* i_this) {
    // HD: reset returns to the logo, without checking the GameCube movie actor.
    if (l_reset->mReset) {
        fopScnM_ChangeReq(i_this, fpcNm_LOGO_SCENE_e, fpcNm_OVERLAP0_e, 5, 1);
        return TRUE;
    }

    // HD: select the main controller and branch on the save-manager result.
    l_input->setMode(hdInputCtrl_c::MODE_MAIN_e);
    u32 result = l_saveMgr->mResult;
    if (result >= hdTitleSaveMgr_c::RESULT_ENDING_FIRST_e && result <= hdTitleSaveMgr_c::RESULT_ENDING_LAST_e) {
        mDoGph_setFadeColor(&g_blackColor);
        fopScnM_ChangeReq(i_this, fpcNm_ENDING_SCENE_e, fpcNm_OVERLAP0_e, 5, 1);
    } else {
        dComIfG_changeOpeningScene(i_this, fpcNm_OPENING_SCENE_e);
    }
    return TRUE;
}

/* 025ABFCC */
static u32 dScnTitle_IsDelete(title_of_scene_class* i_this) {
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
 data l_reset 101F4974
 data l_input 101F5088
 data l_saveMgr 101F852C
 data g_blackColor 101D5E94
 ext 025DC86C void fopScnM_ChangeReq(title_of_scene_class*, u32, u32, u32, u32)
 ext 02520230 void dComIfG_changeOpeningScene(title_of_scene_class*, u32)
 ext 02618094 void hdInputCtrl_c::setMode(u32)
 ext 025F05F8 void mDoGph_setFadeColor(GXColor*)
*/
