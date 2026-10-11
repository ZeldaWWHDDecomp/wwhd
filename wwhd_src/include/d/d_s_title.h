/* Title scene, WWHD. The verified functions do not access the scene's own fields.
 * HD replaces the GameCube movie actor checks with reset, controller and save-manager state. */
#pragma once
#include "wwhd.h"
#include "gabi.h"

struct title_of_scene_class;

/* HD singleton prefixes: only the fields read by this translation unit are represented. */
struct hdTitleReset_c {
    /* 0x00 */ be<u32> mReset;
};
WWHD_OFFSET(hdTitleReset_c, mReset, 0);
WWHD_SIZE(hdTitleReset_c, 4);

struct hdTitleSaveMgr_c {
    enum Result_e : u32 {
        RESULT_ENDING_FIRST_e = 0xFFFFFFFD,
        RESULT_ENDING_LAST_e = 0xFFFFFFFE,
    };
    /* 0x00 */ u8 _00[0x20];
    /* 0x20 */ be<u32> mResult;
};
WWHD_OFFSET(hdTitleSaveMgr_c, mResult, 0x20);
WWHD_SIZE(hdTitleSaveMgr_c, 0x24);

struct hdInputCtrl_c {
    enum Mode_e : u32 { MODE_MAIN_e = 3 };
    void setMode(u32);
};

void fopScnM_ChangeReq(title_of_scene_class*, u32, u32, u32, u32);
void dComIfG_changeOpeningScene(title_of_scene_class*, u32);
void mDoGph_setFadeColor(GXColor*);
