/* Save-query handlers: GameCube enum names, shared d_com_inf_game accessors. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"

// HD: private binding for the one collect query not exposed by the shared accessor set.
s32 itemQuery_isTriforce(dSv_player_collect_c* collect, u32 piece);
enum ItemQueryValue {
    dBeastIdx_JOY_PENDANT_e = 7,
    dBeastIdx_SKULL_NECKLACE_e = 0,
    dBeastIdx_BOKOBABA_SEED_e = 1,
    dBeastIdx_GOLDEN_FEATHER_e = 2,
    dBeastIdx_KNIGHTS_CREST_e = 3,
    dBeastIdx_RED_JELLY_e = 4,
    dBeastIdx_GREEN_JELLY_e = 5,
    dBeastIdx_BLUE_JELLY_e = 6,
    dItemNo_EMPTY_BSHIP_e = 0x4F,
    dItemNo_EMPTY_BOTTLE_e = 0x50,
    dItemNo_RED_POTION_e = 0x51,
    dItemNo_GREEN_POTION_e = 0x52,
    dItemNo_BLUE_POTION_e = 0x53,
    dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
    dItemNo_SOUP_BOTTLE_e = 0x55,
    dItemNo_WATER_BOTTLE_e = 0x56,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    dSymbol_NAYRU_e = 0,
    dSymbol_DIN_e = 1,
    dSymbol_FARORE_e = 2,
};
struct dSv_event_flag_c {
    enum {
        LITHOGRAPH_1 = 0x3508,
        LITHOGRAPH_2 = 0x3504,
        LITHOGRAPH_3 = 0x3502,
        LITHOGRAPH_4 = 0x3501,
        LITHOGRAPH_5 = 0x3680,
        LITHOGRAPH_6 = 0x3640,
        LITHOGRAPH_7 = 0x3620,
        LITHOGRAPH_8 = 0x3610,
        LITHOGRAPH_9 = 0x3608,
        LITHOGRAPH_10 = 0x3604,
    };
};
