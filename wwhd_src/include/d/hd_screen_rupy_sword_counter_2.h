/* HD: the rupee counter of the sword minigame (Orca's training, "RupySwordCounter_00"): slides in, shows a
 * rupee or sword picture by minigame kind, counts towards the hits, slides out
 * (hd_screen_rupy_sword_counter_2.cpp; the count display and the count roll are
 * hd_screen_rupy_sword_counter.cpp). No GameCube counterpart. Layout, pane, animation and state names are
 * HD strings; class, member and method names are ours.
 *
 * The class, hdScreenRupySwordCounter_c, is declared in d/hd_minigame_hud.h; its base is the "NoControl"
 * parts class (NoControlPart_l, FW-parts_msg_base); its child is a "BCollectIcon_00" parts
 * (MENU-btn_collect_icon, unmerged: two local bindings). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_minigame_hud.h"
#include "d/hd_screen_cursor.h"
#include "d/hd_screen_character.h"
#include "d/d_com_inf_game.h"

/* the collection button icon (MENU-btn_collect_icon), by address */
hdParts_c* hdBtnCollectIcon_create_l(u32 arg1, u32 arg2, void* desc);   /* 0264B66C */
void hdBtnCollectIcon_update_l(hdParts_c* icon);                         /* 0262A7BC */
struct hdBtnCollectIcon_l {
    /* 0x00 */ u8 _00[0x84];
    /* 0x84 */ be<u32> mKind; /* 0..6, probably the item shown */
};

/* the play state fields this screen reads (dComIfGp_get(); our view) */
struct hdRupySwordPlay_l {
    /* 0x0000 */ u8 _0000[0x5CEA];
    /* 0x5CEA */ be<u8> mMinigame;     /* 7: probably the sword training */
    /* 0x5CEB */ u8 _5CEB[1];
    /* 0x5CEC */ be<s16> mTargetCount;
};

enum hdRupySwordAnim_e {
    RUPY_SWORD_ANIM_IN_ALL = 0,
    RUPY_SWORD_ANIM_RUPY_SWORD = 1, /* its frame: 0 rupee, 1 sword, 2 ... */
    RUPY_SWORD_ANIM_OUT_ALL = 2,
};

