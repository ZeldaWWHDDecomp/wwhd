/** Item resource metadata and effects. GameCube names; measured HD tables.
 * HD adds one archive pointer and expands effect entry arguments to full u32.
 * Native archive spellings, including VamuK/VamuY casing, remain in their guest tables.
 * All tables view their single existing guest copy. */
#pragma once
#include "gc_api/JSystem/JUTAssert.h"

namespace item_data {
enum ItemTable {
    /* 0x00 */ dItemNo_HEART_e,
    /* 0x01 */ dItemNo_GREEN_RUPEE_e,
    /* 0x02 */ dItemNo_BLUE_RUPEE_e,
    /* 0x03 */ dItemNo_YELLOW_RUPEE_e,
    /* 0x04 */ dItemNo_RED_RUPEE_e,
    /* 0x05 */ dItemNo_PURPLE_RUPEE_e,
    /* 0x06 */ dItemNo_ORANGE_RUPEE_e,
    /* 0x07 */ dItemNo_HEART_PIECE_e,
    /* 0x08 */ dItemNo_HEART_CONTAINER_e,
    /* 0x09 */ dItemNo_SMALL_MAGIC_e,
    /* 0x0A */ dItemNo_LARGE_MAGIC_e,
    /* 0x0B */ dItemNo_BOMB_5_e,
    /* 0x0C */ dItemNo_BOMB_10_e,
    /* 0x0D */ dItemNo_BOMB_20_e,
    /* 0x0E */ dItemNo_BOMB_30_e,
    /* 0x0F */ dItemNo_SILVER_RUPEE_e,
    /* 0x10 */ dItemNo_ARROW_10_e,
    /* 0x11 */ dItemNo_ARROW_20_e,
    /* 0x12 */ dItemNo_ARROW_30_e,
    /* 0x13 */ dItemNo_NOENTRY_19_e, // Unused
    /* 0x14 */ dItemNo_NOENTRY_20_e, // Unused
    /* 0x15 */ dItemNo_SMALL_KEY_e,
    /* 0x16 */ dItemNo_RECOVER_FAIRY_e,
    /* 0x17 */ dItemNo_HEART_PIECE_2_e, // Not an item, used only for its msg no
    /* 0x18 */ dItemNo_HEART_PIECE_3_e, // Not an item, used only for its msg no
    /* 0x19 */ dItemNo_HEART_PIECE_4_e, // Not an item, used only for its msg no
    /* 0x1A */ dItemNo_SUB_DUN_RUPEE_e, // Used in VERSION_JPN only
    /* 0x1B */ dItemNo_NOENTRY_27_e,    // Unused
    /* 0x1C */ dItemNo_NOENTRY_28_e,    // Unused
    /* 0x1D */ dItemNo_NOENTRY_29_e,    // Unused
    /* 0x1E */ dItemNo_TRIPLE_HEART_e,
    /* 0x1F */ dItemNo_JOY_PENDANT_e,
    /* 0x20 */ dItemNo_TELESCOPE_e,
    /* 0x21 */ dItemNo_TINGLE_TUNER_e,
    /* 0x22 */ dItemNo_WIND_WAKER_e,
    /* 0x23 */ dItemNo_PICTO_BOX_e,
    /* 0x24 */ dItemNo_SPOILS_BAG_e,
    /* 0x25 */ dItemNo_GRAPPLING_HOOK_e,
    /* 0x26 */ dItemNo_DELUXE_PICTO_BOX_e,
    /* 0x27 */ dItemNo_BOW_e,
    /* 0x28 */ dItemNo_POWER_BRACELETS_e,
    /* 0x29 */ dItemNo_IRON_BOOTS_e,
    /* 0x2A */ dItemNo_MAGIC_ARMOR_e,
    /* 0x2B */ dItemNo_WATER_BOOTS_e, // Unused
    /* 0x2C */ dItemNo_BAIT_BAG_e,
    /* 0x2D */ dItemNo_BOOMERANG_e,
    /* 0x2E */ dItemNo_BARE_HAND_e, // Unused
    /* 0x2F */ dItemNo_HOOKSHOT_e,
    /* 0x30 */ dItemNo_DELIVERY_BAG_e,
    /* 0x31 */ dItemNo_BOMB_BAG_e,
    /* 0x32 */ dItemNo_FUKU_e,
    /* 0x33 */ dItemNo_SKULL_HAMMER_e,
    /* 0x34 */ dItemNo_DEKU_LEAF_e,
    /* 0x35 */ dItemNo_MAGIC_ARROW_e,
    /* 0x36 */ dItemNo_LIGHT_ARROW_e,
    /* 0x37 */ dItemNo_NEW_FUKU_e,
    /* 0x38 */ dItemNo_SWORD_e,
    /* 0x39 */ dItemNo_MASTER_SWORD_1_e,
    /* 0x3A */ dItemNo_MASTER_SWORD_2_e,
    /* 0x3B */ dItemNo_SHIELD_e,
    /* 0x3C */ dItemNo_MIRROR_SHIELD_e,
    /* 0x3D */ dItemNo_DROPPED_SWORD_e,
    /* 0x3E */ dItemNo_MASTER_SWORD_3_e,
    /* 0x3F */ dItemNo_HEART_PIECE_ALT_e,
    /* 0x40 */ dItemNo_NOENTRY_64_e, // Unused
    /* 0x41 */ dItemNo_NOENTRY_65_e, // Unused
    /* 0x42 */ dItemNo_PIRATES_CHARM_e,
    /* 0x43 */ dItemNo_HEROS_CHARM_e,
    /* 0x44 */ dItemNo_GRASS_BALL_e, // Unused
    /* 0x45 */ dItemNo_SKULL_NECKLACE_e,
    /* 0x46 */ dItemNo_BOKOBABA_SEED_e,
    /* 0x47 */ dItemNo_GOLDEN_FEATHER_e,
    /* 0x48 */ dItemNo_KNIGHTS_CREST_e,
    /* 0x49 */ dItemNo_RED_JELLY_e,
    /* 0x4A */ dItemNo_GREEN_JELLY_e,
    /* 0x4B */ dItemNo_BLUE_JELLY_e,
    /* 0x4C */ dItemNo_MAP_e,
    /* 0x4D */ dItemNo_COMPASS_e,
    /* 0x4E */ dItemNo_BOSS_KEY_e,
    /* 0x4F */ dItemNo_EMPTY_BSHIP_e, // Unused
    /* 0x50 */ dItemNo_EMPTY_BOTTLE_e,
    /* 0x51 */ dItemNo_RED_POTION_e,
    /* 0x52 */ dItemNo_GREEN_POTION_e,
    /* 0x53 */ dItemNo_BLUE_POTION_e,
    /* 0x54 */ dItemNo_HALF_SOUP_BOTTLE_e,
    /* 0x55 */ dItemNo_SOUP_BOTTLE_e,
    /* 0x56 */ dItemNo_WATER_BOTTLE_e,
    /* 0x57 */ dItemNo_FAIRY_BOTTLE_e,
    /* 0x58 */ dItemNo_FIREFLY_BOTTLE_e,
    /* 0x59 */ dItemNo_FOREST_WATER_e,
    /* 0x5A */ dItemNo_UNK_BOTTLE_5A_e, // Unused
    /* 0x5B */ dItemNo_UNK_BOTTLE_5B_e, // Unused
    /* 0x5C */ dItemNo_UNK_BOTTLE_5C_e, // Unused
    /* 0x5D */ dItemNo_UNK_BOTTLE_5D_e, // Unused
    /* 0x5E */ dItemNo_UNK_BOTTLE_5E_e, // Unused
    /* 0x5F */ dItemNo_UNK_BOTTLE_5F_e, // Unused
    /* 0x60 */ dItemNo_UNK_BOTTLE_60_e, // Unused
    /* 0x61 */ dItemNo_TRIFORCE1_e,
    /* 0x62 */ dItemNo_TRIFORCE2_e,
    /* 0x63 */ dItemNo_TRIFORCE3_e,
    /* 0x64 */ dItemNo_TRIFORCE4_e,
    /* 0x65 */ dItemNo_TRIFORCE5_e,
    /* 0x66 */ dItemNo_TRIFORCE6_e,
    /* 0x67 */ dItemNo_TRIFORCE7_e,
    /* 0x68 */ dItemNo_TRIFORCE8_e,
    /* 0x69 */ dItemNo_PEARL_NAYRU_e,
    /* 0x6A */ dItemNo_PEARL_DIN_e,
    /* 0x6B */ dItemNo_PEARL_FARORE_e,
    /* 0x6C */ dItemNo_KNOWLEDGE_TF_e, // Unused
    /* 0x6D */ dItemNo_WINDS_REQUIEM_e,
    /* 0x6E */ dItemNo_BALLAD_OF_GALES_e,
    /* 0x6F */ dItemNo_COMMAND_MELODY_e,
    /* 0x70 */ dItemNo_EARTH_GODS_LYRIC_e,
    /* 0x71 */ dItemNo_WIND_GODS_ARIA_e,
    /* 0x72 */ dItemNo_SONG_OF_PASSING_e,
    /* 0x73 */ dItemNo_NOENTRY_115_e, // Unused
    /* 0x74 */ dItemNo_NOENTRY_116_e, // Unused
    /* 0x75 */ dItemNo_NOENTRY_117_e, // Unused
    /* 0x76 */ dItemNo_NOENTRY_118_e, // Unused
    /* 0x77 */ dItemNo_NOENTRY_119_e, // Unused
    /* 0x78 */ dItemNo_SAIL_e,
    /* 0x79 */ dItemNo_TRIFORCE_MAP_1_e,
    /* 0x7A */ dItemNo_TRIFORCE_MAP_2_e,
    /* 0x7B */ dItemNo_TRIFORCE_MAP_3_e,
    /* 0x7C */ dItemNo_TRIFORCE_MAP_4_e,
    /* 0x7D */ dItemNo_TRIFORCE_MAP_5_e,
    /* 0x7E */ dItemNo_TRIFORCE_MAP_6_e,
    /* 0x7F */ dItemNo_TRIFORCE_MAP_7_e,
    /* 0x80 */ dItemNo_TRIFORCE_MAP_8_e,
    /* 0x81 */ dItemNo_NOENTRY_129_e, // Unused
    /* 0x82 */ dItemNo_BIRD_BAIT_5_e,
    /* 0x83 */ dItemNo_HYOI_PEAR_e,
    /* 0x84 */ dItemNo_ESA_1_e,       // Unused
    /* 0x85 */ dItemNo_ESA_2_e,       // Unused
    /* 0x86 */ dItemNo_ESA_3_e,       // Unused
    /* 0x87 */ dItemNo_ESA_4_e,       // Unused
    /* 0x88 */ dItemNo_ESA_5_e,       // Unused
    /* 0x89 */ dItemNo_MAGIC_BEAN_e,  // Unused
    /* 0x8A */ dItemNo_BIRD_ESA_10_e, // Unused
    /* 0x8B */ dItemNo_NOENTRY_139_e, // Unused
    /* 0x8C */ dItemNo_TOWN_FLOWER_e,
    /* 0x8D */ dItemNo_SEA_FLOWER_e,
    /* 0x8E */ dItemNo_EXOTIC_FLOWER_e,
    /* 0x8F */ dItemNo_HEROS_FLAG_e,
    /* 0x90 */ dItemNo_BIG_CATCH_FLAG_e,
    /* 0x91 */ dItemNo_BIG_SALE_FLAG_e,
    /* 0x92 */ dItemNo_PINWHEEL_e,
    /* 0x93 */ dItemNo_SICKLE_MOON_FLAG_e,
    /* 0x94 */ dItemNo_SKULL_TOWER_IDOL_e,
    /* 0x95 */ dItemNo_FOUNTAIN_IDOL_e,
    /* 0x96 */ dItemNo_POSTMAN_STATUE_e,
    /* 0x97 */ dItemNo_SHOP_GURU_STATUE_e,
    /* 0x98 */ dItemNo_FATHER_LETTER_e,
    /* 0x99 */ dItemNo_NOTE_TO_MOM_e,
    /* 0x9A */ dItemNo_MAGGIES_LETTER_e,
    /* 0x9B */ dItemNo_MOBLINS_LETTER_e,
    /* 0x9C */ dItemNo_CABANA_DEED_e,
    /* 0x9D */ dItemNo_COMPLIMENTARY_ID_e,
    /* 0x9E */ dItemNo_FILL_UP_COUPON_e,
    /* 0x9F */ dItemNo_LEGENDARY_PICTOGRAPH_e, // Originally "salvage item 1", but was repurposed
    /* 0xA0 */ dItemNo_SALVAGE_ITEM_2_e,       // Unused
    /* 0xA1 */ dItemNo_SALVAGE_ITEM_3_e,       // Unused
    /* 0xA2 */ dItemNo_XXX_039_e,              // Unused
    /* 0xA3 */ dItemNo_TINGLE_STATUE_1_e,
    /* 0xA4 */ dItemNo_TINGLE_STATUE_2_e,
    /* 0xA5 */ dItemNo_TINGLE_STATUE_3_e,
    /* 0xA6 */ dItemNo_TINGLE_STATUE_4_e,
    /* 0xA7 */ dItemNo_TINGLE_STATUE_5_e,
    /* 0xA8 */ dItemNo_TINGLE_STATUE_6_e, // Unused
    /* 0xA9 */ dItemNo_NOENTRY_169_e,     // Unused
    /* 0xAA */ dItemNo_HURRICANE_SPIN_e,
    /* 0xAB */ dItemNo_MAX_RUPEE_UP1_e,
    /* 0xAC */ dItemNo_MAX_RUPEE_UP2_e,
    /* 0xAD */ dItemNo_MAX_BOMB_UP1_e,
    /* 0xAE */ dItemNo_MAX_BOMB_UP2_e,
    /* 0xAF */ dItemNo_MAX_ARROW_UP1_e,
    /* 0xB0 */ dItemNo_MAX_ARROW_UP2_e,
    /* 0xB1 */ dItemNo_MAGIC_POWER_e,
    /* 0xB2 */ dItemNo_MAX_MP_UP1_e,
    /* 0xB3 */ dItemNo_TINGLE_RUPEE_1_e,
    /* 0xB4 */ dItemNo_TINGLE_RUPEE_2_e,
    /* 0xB5 */ dItemNo_TINGLE_RUPEE_3_e,
    /* 0xB6 */ dItemNo_TINGLE_RUPEE_4_e,
    /* 0xB7 */ dItemNo_TINGLE_RUPEE_5_e,
    /* 0xB8 */ dItemNo_TINGLE_RUPEE_6_e,
    /* 0xB9 */ dItemNo_LITHOGRAPH_1_e,   // Unused
    /* 0xBA */ dItemNo_LITHOGRAPH_2_e,   // Unused
    /* 0xBB */ dItemNo_LITHOGRAPH_3_e,   // Unused
    /* 0xBC */ dItemNo_LITHOGRAPH_4_e,   // Unused
    /* 0xBD */ dItemNo_LITHOGRAPH_5_e,   // Unused
    /* 0xBE */ dItemNo_LITHOGRAPH_6_e,   // Unused
    /* 0xBF */ dItemNo_COLLECT_MAP_64_e, // Unused
    /* 0xC0 */ dItemNo_COLLECT_MAP_63_e, // Unused
    /* 0xC1 */ dItemNo_COLLECT_MAP_62_e, // Unused
    /* 0xC2 */ dItemNo_COLLECT_MAP_61_e,
    /* 0xC3 */ dItemNo_COLLECT_MAP_60_e,
    /* 0xC4 */ dItemNo_COLLECT_MAP_59_e,
    /* 0xC5 */ dItemNo_COLLECT_MAP_58_e,
    /* 0xC6 */ dItemNo_COLLECT_MAP_57_e,
    /* 0xC7 */ dItemNo_COLLECT_MAP_56_e,
    /* 0xC8 */ dItemNo_COLLECT_MAP_55_e,
    /* 0xC9 */ dItemNo_COLLECT_MAP_54_e,
    /* 0xCA */ dItemNo_COLLECT_MAP_53_e,
    /* 0xCB */ dItemNo_COLLECT_MAP_52_e,
    /* 0xCC */ dItemNo_COLLECT_MAP_51_e,
    /* 0xCD */ dItemNo_COLLECT_MAP_50_e,
    /* 0xCE */ dItemNo_COLLECT_MAP_49_e,
    /* 0xCF */ dItemNo_COLLECT_MAP_48_e,
    /* 0xD0 */ dItemNo_COLLECT_MAP_47_e,
    /* 0xD1 */ dItemNo_COLLECT_MAP_46_e,
    /* 0xD2 */ dItemNo_COLLECT_MAP_45_e,
    /* 0xD3 */ dItemNo_COLLECT_MAP_44_e,
    /* 0xD4 */ dItemNo_COLLECT_MAP_43_e,
    /* 0xD5 */ dItemNo_COLLECT_MAP_42_e,
    /* 0xD6 */ dItemNo_COLLECT_MAP_41_e,
    /* 0xD7 */ dItemNo_COLLECT_MAP_40_e,
    /* 0xD8 */ dItemNo_COLLECT_MAP_39_e,
    /* 0xD9 */ dItemNo_COLLECT_MAP_38_e,
    /* 0xDA */ dItemNo_COLLECT_MAP_37_e,
    /* 0xDB */ dItemNo_COLLECT_MAP_36_e,
    /* 0xDC */ dItemNo_COLLECT_MAP_35_e,
    /* 0xDD */ dItemNo_COLLECT_MAP_34_e,
    /* 0xDE */ dItemNo_COLLECT_MAP_33_e,
    /* 0xDF */ dItemNo_COLLECT_MAP_32_e,
    /* 0xE0 */ dItemNo_COLLECT_MAP_31_e,
    /* 0xE1 */ dItemNo_COLLECT_MAP_30_e,
    /* 0xE2 */ dItemNo_COLLECT_MAP_29_e,
    /* 0xE3 */ dItemNo_COLLECT_MAP_28_e,
    /* 0xE4 */ dItemNo_COLLECT_MAP_27_e,
    /* 0xE5 */ dItemNo_COLLECT_MAP_26_e,
    /* 0xE6 */ dItemNo_COLLECT_MAP_25_e,
    /* 0xE7 */ dItemNo_COLLECT_MAP_24_e,
    /* 0xE8 */ dItemNo_COLLECT_MAP_23_e,
    /* 0xE9 */ dItemNo_COLLECT_MAP_22_e,
    /* 0xEA */ dItemNo_COLLECT_MAP_21_e,
    /* 0xEB */ dItemNo_COLLECT_MAP_20_e,
    /* 0xEC */ dItemNo_COLLECT_MAP_19_e,
    /* 0xED */ dItemNo_COLLECT_MAP_18_e,
    /* 0xEE */ dItemNo_COLLECT_MAP_17_e,
    /* 0xEF */ dItemNo_COLLECT_MAP_16_e,
    /* 0xF0 */ dItemNo_COLLECT_MAP_15_e,
    /* 0xF1 */ dItemNo_COLLECT_MAP_14_e,
    /* 0xF2 */ dItemNo_COLLECT_MAP_13_e,
    /* 0xF3 */ dItemNo_COLLECT_MAP_12_e,
    /* 0xF4 */ dItemNo_COLLECT_MAP_11_e,
    /* 0xF5 */ dItemNo_COLLECT_MAP_10_e,
    /* 0xF6 */ dItemNo_COLLECT_MAP_09_e,
    /* 0xF7 */ dItemNo_COLLECT_MAP_08_e,
    /* 0xF8 */ dItemNo_COLLECT_MAP_07_e,
    /* 0xF9 */ dItemNo_COLLECT_MAP_06_e,
    /* 0xFA */ dItemNo_COLLECT_MAP_05_e,
    /* 0xFB */ dItemNo_COLLECT_MAP_04_e,
    /* 0xFC */ dItemNo_COLLECT_MAP_03_e,
    /* 0xFD */ dItemNo_COLLECT_MAP_02_e,
    /* 0xFE */ dItemNo_COLLECT_MAP_01_e,
    /* 0xFF */ dItemNo_NONE_e,
};
}
enum ItemArcnameIdx {
    ARCNAME_IDX_ALWAYS,
    ARCNAME_IDX_VHUTU,
    ARCNAME_IDX_VHKAK,
    ARCNAME_IDX_VKEYN,
    ARCNAME_IDX_KEY_01,
    ARCNAME_IDX_TELE_00,
    ARCNAME_IDX_BOMB_00,
    ARCNAME_IDX_LINK,
    ARCNAME_IDX_VHANE,
    ARCNAME_IDX_VBOKO,
    ARCNAME_IDX_VCHUJ,
    ARCNAME_IDX_VMAPC,
    ARCNAME_IDX_VSAIM,
    ARCNAME_IDX_VSAIL,
    ARCNAME_IDX_VYATM,
    ARCNAME_IDX_VYATL,
    ARCNAME_IDX_BALL,
    ARCNAME_IDX_BIN,
    ARCNAME_IDX_VBOMM,
    ARCNAME_IDX_BOOM,
    ARCNAME_IDX_VBOOT,
    ARCNAME_IDX_VBOML,
    ARCNAME_IDX_BOTTLE,
    ARCNAME_IDX_BOTTLE_E,
    ARCNAME_IDX_VBOXN,
    ARCNAME_IDX_VDUNC,
    ARCNAME_IDX_VDUNM,
    ARCNAME_IDX_VLEAF,
    ARCNAME_IDX_VBAGH,
    ARCNAME_IDX_VBAGE,
    ARCNAME_IDX_VBAGF,
    ARCNAME_IDX_VHYOI,
    ARCNAME_IDX_VESA,
    ARCNAME_IDX_VOBJ07,
    ARCNAME_IDX_VOBJ06,
    ARCNAME_IDX_VOBJ05,
    ARCNAME_IDX_VOBJ04,
    ARCNAME_IDX_VOBJ03,
    ARCNAME_IDX_VOBJ00,
    ARCNAME_IDX_VOBJ01,
    ARCNAME_IDX_VOBJ02,
    ARCNAME_IDX_VHOOK,
    ARCNAME_IDX_VPOWR,
    ARCNAME_IDX_VHO,
    ARCNAME_IDX_VMANT,
    ARCNAME_IDX_VOHOM,
    ARCNAME_IDX_VCHIN,
    ARCNAME_IDX_VHAMM,
    ARCNAME_IDX_VAMUK,
    ARCNAME_IDX_VAMUY,
    ARCNAME_IDX_MED_B,
    ARCNAME_IDX_MED_G,
    ARCNAME_IDX_MED_R,
    ARCNAME_IDX_VOBJ08,
    ARCNAME_IDX_VOBJ09,
    ARCNAME_IDX_VOBJ11,
    ARCNAME_IDX_VOBJ10,
    ARCNAME_IDX_VHAPP,
    ARCNAME_IDX_VROPE,
    ARCNAME_IDX_VMAGS,
    ARCNAME_IDX_SHINJU_N,
    ARCNAME_IDX_SORD_M,
    ARCNAME_IDX_SORD_MVU,
    ARCNAME_IDX_VSWON,
    ARCNAME_IDX_VSHIM,
    ARCNAME_IDX_VSHIN,
    ARCNAME_IDX_VTRI1,
    ARCNAME_IDX_VTACT,
    ARCNAME_IDX_VARFI,
    ARCNAME_IDX_VMAPQ,
    ARCNAME_IDX_VARRL,
    ARCNAME_IDX_VBOW,
    ARCNAME_IDX_FA,
    ARCNAME_IDX_SHINJU_D,
    ARCNAME_IDX_SHINJU_F,
    ARCNAME_IDX_FWBIN,
    ARCNAME_IDX_VBINY,
    ARCNAME_IDX_VBINH,
    ARCNAME_IDX_VBINS,
    ARCNAME_IDX_VBINW,
    ARCNAME_IDX_VFUKU,
    ARCNAME_IDX_VDOKU,
    ARCNAME_IDX_VBELT,
    ARCNAME_IDX_VLUPY,
    ARCNAME_IDX_VLETT,
    ARCNAME_IDX_VLEOF,
    ARCNAME_IDX_VLEMG,
    ARCNAME_IDX_VLEMO,
    ARCNAME_IDX_FDAI_0x58,
    ARCNAME_IDX_FDAI_0x59,
    ARCNAME_IDX_FDAI_0x5A,
    ARCNAME_IDX_FDAI_0x5B,
    ARCNAME_IDX_FDAI_0x5C,
    ARCNAME_IDX_VTRI2,
    ARCNAME_IDX_VTRI3,
    ARCNAME_IDX_VTRI4,
    ARCNAME_IDX_VTRI5,
    ARCNAME_IDX_VTRI6,
    ARCNAME_IDX_VTRI7,
    ARCNAME_IDX_VTRI8,
    ARCNAME_IDX_FDAI_0x64,
    ARCNAME_IDX_VMAPT,
    ARCNAME_IDX_VMAPR,
    ARCNAME_IDX_VMAPH,
    ARCNAME_IDX_VMAPG,
    ARCNAME_IDX_VBOXA,
    ARCNAME_IDX_VTIN1,
    ARCNAME_IDX_VTIN2,
    ARCNAME_IDX_VTIN3,
    ARCNAME_IDX_VTIN4,
    ARCNAME_IDX_VTIN5,
    ARCNAME_IDX_VBESO, // HD: the additional Mii bottle archive follows the donor table.
    ARCNAME_IDX_MII_BING_HD,
};
enum ItemTextureIdx {
    TEXTURE_IDX_GET_RUPY,
    TEXTURE_IDX_GET_KEY,
    TEXTURE_IDX_HEART_UP_01,
    TEXTURE_IDX_HEART_UP_02,
    TEXTURE_IDX_TELESCOPE,
    TEXTURE_IDX_WHISTLE,
    TEXTURE_IDX_BATON,
    TEXTURE_IDX_CAMERA,
    TEXTURE_IDX_COVEROFBEAST,
    TEXTURE_IDX_BOOMERANG,
    TEXTURE_IDX_ROPE,
    TEXTURE_IDX_SAIL_00,
    TEXTURE_IDX_FAN,
    TEXTURE_IDX_HAMMER_01,
    TEXTURE_IDX_BOOTS_00,
    TEXTURE_IDX_BOOTS_01,
    TEXTURE_IDX_COVEROFBAIT,
    TEXTURE_IDX_BOW_01,
    TEXTURE_IDX_BOMB_00,
    TEXTURE_IDX_SAIL_01,
    TEXTURE_IDX_BOTTLE_00,
    TEXTURE_IDX_BOTTLE_01,
    TEXTURE_IDX_BOTTLE_02,
    TEXTURE_IDX_BOTTLE_03,
    TEXTURE_IDX_DELIVERY,
    TEXTURE_IDX_HOOKSHOT,
    TEXTURE_IDX_SHIELD_02,
    TEXTURE_IDX_SAIL_02,
    TEXTURE_IDX_BEAST_01,
    TEXTURE_IDX_BEAST_02,
    TEXTURE_IDX_BEAST_03,
    TEXTURE_IDX_BEAST_04,
    TEXTURE_IDX_BEAST_05,
    TEXTURE_IDX_BEAST_06,
    TEXTURE_IDX_BEAST_07,
    TEXTURE_IDX_BEAST_08,
    TEXTURE_IDX_BAIT_01,
    TEXTURE_IDX_BAIT_02,
    TEXTURE_IDX_BAIT_03,
    TEXTURE_IDX_BAIT_04,
    TEXTURE_IDX_BAIT_05,
    TEXTURE_IDX_BAIT_06,
    TEXTURE_IDX_BAIT_07,
    TEXTURE_IDX_BAIT_08,
    TEXTURE_IDX_BATON_STONE_0x2C,
    TEXTURE_IDX_GOD_SYMBOL_00,
    TEXTURE_IDX_GOD_SYMBOL_01,
    TEXTURE_IDX_GOD_SYMBOL_02,
    TEXTURE_IDX_SWORD_00,
    TEXTURE_IDX_SWORD_01,
    TEXTURE_IDX_SHIELD_00,
    TEXTURE_IDX_SHIELD_01,
    TEXTURE_IDX_GLOVES_00_0x34,
    TEXTURE_IDX_GLOVES_00_0x35,
    TEXTURE_IDX_AMULET_00,
    TEXTURE_IDX_AMULET_01,
    TEXTURE_IDX_DELIVERY_01,
    TEXTURE_IDX_DELIVERY_02,
    TEXTURE_IDX_DELIVERY_03,
    TEXTURE_IDX_DELIVERY_04,
    TEXTURE_IDX_DELIVERY_05,
    TEXTURE_IDX_DELIVERY_06,
    TEXTURE_IDX_DELIVERY_07,
    TEXTURE_IDX_DELIVERY_08,
    TEXTURE_IDX_DELIVERY_09,
    TEXTURE_IDX_DELIVERY_10,
    TEXTURE_IDX_DELIVERY_11,
    TEXTURE_IDX_DELIVERY_12,
    TEXTURE_IDX_DELIVERY_13,
    TEXTURE_IDX_DELIVERY_14,
    TEXTURE_IDX_DELIVERY_15,
    TEXTURE_IDX_DELIVERY_16,
    TEXTURE_IDX_DELIVERY_17,
    TEXTURE_IDX_DELIVERY_18,
    TEXTURE_IDX_DUNGEON_MAP,
    TEXTURE_IDX_COMPASS,
    TEXTURE_IDX_BOSS_KEY,
    TEXTURE_IDX_BOTTLE_04,
    TEXTURE_IDX_BOTTLE_05,
    TEXTURE_IDX_BOTTLE_06,
    TEXTURE_IDX_BOTTLE_07,
    TEXTURE_IDX_BOTTLE_08,
    TEXTURE_IDX_BATON_STONE_0x52,
    TEXTURE_IDX_TRIFORCE_00,
    TEXTURE_IDX_TRIFORCE_01,
    TEXTURE_IDX_TRIFORCE_02,
    TEXTURE_IDX_TRIFORCE_03,
    TEXTURE_IDX_TRIFORCE_04,
    TEXTURE_IDX_TRIFORCE_05,
    TEXTURE_IDX_TRIFORCE_06,
    TEXTURE_IDX_TRIFORCE_07,
    TEXTURE_IDX_ARROW_00,
    TEXTURE_IDX_ARROW_01,
    TEXTURE_IDX_ARROW_02,
    TEXTURE_IDX_ARROW_03,
    TEXTURE_IDX_BIG_PURSE,
    TEXTURE_IDX_MAX_PURSE,
    TEXTURE_IDX_SWORD_02,
    TEXTURE_IDX_SWORD_03,
    TEXTURE_IDX_CLOTHES,
    TEXTURE_IDX_BOMBPOUCH_1,
    TEXTURE_IDX_BOMBPOUCH_2,
    TEXTURE_IDX_ARROWCASE_1,
    TEXTURE_IDX_ARROWCASE_2,
    TEXTURE_IDX_CMAP_TRI2,
    TEXTURE_IDX_CMAP_TREASURE2_0x69,
    TEXTURE_IDX_CMAP_TREASURE2_0x6A,
    TEXTURE_IDX_CMAP_PHANTOMSHIP2,
    TEXTURE_IDX_CMAP_TINGLE2,
    TEXTURE_IDX_ARROW_POWER_01,
    TEXTURE_IDX_ARROW_POWER_02,
    TEXTURE_IDX_CAMERA_2,
    TEXTURE_IDX_TINGLE_FIGURE,
    TEXTURE_IDX_BOTTLE_09,
    TEXTURE_IDX_CMAP_HINT2,
    TEXTURE_IDX_DELIVERY_19,
};
namespace dPa_name {
enum { ID_SCENE_END = 0x8466 };
}
struct dItem_data_item_resource {
    /* 0x00 */ gptr<const char> mArcname;
    /* 0x04 */ gptr<const char> mTexture;
    /* 0x08 */ be<s16> mBmdIdx, mSrtIdx, mSrtIdx2, mTevIdx, mTevIdx2, mBckIdx;
    /* 0x14 */ be<s8> mTevFrm;
    /* 0x15 */ u8 _15;
    /* 0x16 */ be<s16> mItemMesgNum;
    /* 0x18 */ be<u32> mUnused, mUnknown;
    /* 0x20 */ be<u16> mHeapSize;
    /* 0x22 */ u8 _22[2];
};
struct dItem_data_field_item_res {
    /* 0x00 */ gptr<const char> mArc;
    /* 0x04 */ be<s16> mBmdIdx, mSrtIdx, mSrtIdx2, mTevIdx, mTevIdx2, mBckIdx;
    /* 0x10 */ be<s8> mTevFrm;
    /* 0x11 */ u8 _11[3];
    /* 0x14 */ be<u32> mUnknown;
    /* 0x18 */ be<u16> mHeapSize;
    /* 0x1A */ u8 _1A[2];
};
struct dItem_data_item_info {
    be<u8> mShadowSize, mCollisionH, mCollisionR, mFlag;
};
struct dItem_data_effect_info {
    be<u16> m_appear, m_sp_effect;
};
struct ItemArchiveNames {
    gptr<const char> mNames[113];
};
struct ItemTextureNames {
    gptr<const char> mNames[116];
};
struct ItemResources {
    dItem_data_item_resource mItems[256];
};
struct ItemFieldResources {
    dItem_data_field_item_res mItems[256];
};
struct ItemInfos {
    dItem_data_item_info mItems[256];
};
struct ItemEffects {
    dItem_data_effect_info mItems[129];
};
struct dItem_data {
    static gptr<const char> (&item_arcname_tbl)[113];
    static gptr<const char> (&item_texture_tbl)[116];
    static dItem_data_item_resource (&item_resource)[256];
    static dItem_data_field_item_res (&field_item_res)[256];
    static dItem_data_item_info (&item_info)[256];
    static dItem_data_effect_info (&effect_info)[129];
    // HD: preserve full-u32 entry indices and full integer predicate results.
    static BOOL checkAppearEffect(u32 no);
    static u16 getAppearEffect(u32 no);
    static BOOL checkSpecialEffect(u32 no);
    static u16 getSpecialEffect(u32 no);
};
struct ItemHeaderStatics {
    be<f32> mNegativePi, mPi;
    u8 mFirstObject, mSecondObject;
    u8 _0A[2];
    be<u32> mZeroWords[4];
};
struct ItemStaticRecord {
    u8 mStorage[12];
};
struct ItemStaticRecords {
    ItemStaticRecord mEntries[3];
};
struct ItemAngleBounds {
    be<f32> mNegativePi, mPi;
};
WWHD_SIZE(dItem_data_item_resource, 0x24);
WWHD_OFFSET(dItem_data_item_resource, mTexture, 4);
WWHD_OFFSET(dItem_data_item_resource, mBmdIdx, 8);
WWHD_OFFSET(dItem_data_item_resource, mTevFrm, 0x14);
WWHD_OFFSET(dItem_data_item_resource, mHeapSize, 0x20);
WWHD_SIZE(dItem_data_field_item_res, 0x1C);
WWHD_OFFSET(dItem_data_field_item_res, mBmdIdx, 4);
WWHD_OFFSET(dItem_data_field_item_res, mHeapSize, 0x18);
WWHD_SIZE(dItem_data_item_info, 4);
WWHD_SIZE(dItem_data_effect_info, 4);
WWHD_OFFSET(dItem_data_effect_info, m_sp_effect, 2);
WWHD_SIZE(ItemHeaderStatics, 0x1C);
WWHD_SIZE(ItemStaticRecords, 0x24);
WWHD_SIZE(ItemArchiveNames, 0x1C4);
WWHD_SIZE(ItemTextureNames, 0x1D0);
WWHD_SIZE(ItemResources, 0x2400);
WWHD_SIZE(ItemFieldResources, 0x1C00);
WWHD_SIZE(ItemInfos, 0x400);
WWHD_SIZE(ItemEffects, 0x204);
void Item_registerStatic(void* descriptor);
void Item_initFirstObject(u8* object);
void Item_initSecondObject(u8* object);
