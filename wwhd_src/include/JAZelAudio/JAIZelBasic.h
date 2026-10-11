/** JAIZelBasic sound effects and BGM controls.
 * HD: 32 SE slots replace GC's24; 16 array-allocation records, a separate system-sound handle,
 * and level-object sound mixing expand the GC layout. GC fields below retain tww names. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
namespace jaizel12 {
enum SoundIdsAndBgm : u32 {
    JA_BGM_GET_HEART = 0x80000024,
    JA_BGM_ITEM_GET_S = 0x80000025,
    JA_BGM_TAKT_KAZE = 0x80000031,
    JA_BGM_TAKT_SHIPPU = 0x80000032,
    JA_SE_ATM_VS_GANON_RAIN = 0x0000706D,
    JA_SE_CM_AM_NEEDLE_IN = 0x0000587E,
    JA_SE_CM_BKM_ATKVINE_BOOM = 0x00007818,
    JA_SE_CM_KS_MOVE = 0x0000587A,
    JA_SE_CM_PG_APPEAR = 0x0000594E,
    JA_SE_CM_PG_DISAPPEAR = 0x0000594F,
    JA_SE_CM_PT_JUMP = 0x0000592D,
    JA_SE_CM_PW_BECOME_CLEAR = 0x00005930,
    JA_SE_CM_PW_GO_HEAVEN = 0x00005934,
    JA_SE_CM_WZ_APPEAR = 0x00005909,
    JA_SE_CM_WZ_PRE_SUMMONBALL = 0x0000590D,
    JA_SE_CV_AM2_DAMAGE = 0x000048B6,
    JA_SE_CV_AM2_JUMP = 0x000048B4,
    JA_SE_CV_AM_JUMP = 0x00004875,
    JA_SE_CV_AM_OPEN_MOUTH = 0x00004876,
    JA_SE_CV_KS_ATTACK = 0x0000486D,
    JA_SE_CV_PG_EBALL_FIRE_L = 0x0000493D,
    JA_SE_CV_PG_EBALL_FIRE_S = 0x0000493C,
    JA_SE_CV_TT_DANCE_1 = 0x00004901,
    JA_SE_CV_TT_DANCE_2 = 0x00004902,
    JA_SE_DEMO_MSG_END = 0x000008FE,
    JA_SE_DEMO_MSG_NEXT = 0x000008FD,
    JA_SE_ITM_OMAMORI_SHIP = 0x0000085D,
    JA_SE_LK_FT_SW_SHIELD = 0x00003831,
    JA_SE_LK_SLIP_SUS_STONE = 0x0000205C,
    JA_SE_LK_WALK_HEAVY = 0x0000382F,
    JA_SE_OBJ_COL_SWC_CHNL = 0x00006807,
    JA_SE_OBJ_PIRATE_CRUISE_LPF = 0x0000706F,
    JA_SE_OBJ_TBOX_UNLOCK = 0x0000690B,
    JA_SE_OBJ_WZ_FIRE_B_IMP = 0x000069DF,
    JA_SE_OBJ_WZ_SUMMON_B_SMN = 0x000069E0,
    JA_SE_PLAYSPOT_DESTROY = 0x000008AA,
    JA_SE_TITLE_WIND = 0x0000090B,
    JA_SE_WTAKT_AUFTAKT_4 = 0x00002880,
    JA_SE_WTAKT_AUFTAKT_6 = 0x00002881,
    JA_SE_WTAKT_PULSE_3 = 0x00000893,
    JA_SE_WTAKT_PULSE_6 = 0x00000895,
    JA_STRM_DEMO_GO_MAJU = 0xC0000005,
    JA_STRM_MJ_DEMO_INTRO = 0xC0000006,

    JA_BGM_BATTLE_NORM = 0x80000004,
    JA_BGM_BGN_GET_BOX = 0x8000005D,
    JA_BGM_CB_NEXTYEAR = 0x80000036,
    JA_BGM_CB_NEXTYEAR_W = 0x80000037,
    JA_BGM_DIOCTA_2 = 0x80000046,
    JA_BGM_DIOCTA_BATTLE = 0x8000002B,
    JA_BGM_GET_PEARL = 0x8000004F,
    JA_BGM_GET_SONG = 0x80000027,
    JA_BGM_ITEM_GET = 0x80000002,
    JA_BGM_MBOSS = 0x80000019,
    JA_BGM_MBOSS_S = 0x8000001A,
    JA_BGM_OPEN_BOX = 0x80000009,
    JA_BGM_P_GANON_1 = 0x80000041,
    JA_BGM_P_GANON_2 = 0x80000047,
    JA_BGM_SEA_ENEMY = 0x8000001C,
    JA_BGM_SEA_GAME = 0x8000000B,
    JA_BGM_TAKT_CHUYA = 0x80000030,
    JA_BGM_TETRA_MEET = 0x80000012,
    JA_BGM_TRIFORCE_DEMO = 0x80000061,
    JA_SE_ATM_ICEBERG_WIND = 0x00001072,
    JA_SE_ATM_MAGMA_CHILL = 0x00001863,
    JA_SE_ATM_MJ_WATER_UP = 0x0000702C,
    JA_SE_ATM_RAIN_1 = 0x0000105B,
    JA_SE_ATM_RAKUBAN = 0x0000105F,
    JA_SE_ATM_RIVER_M = 0x00003033,
    JA_SE_ATM_RIVER_S = 0x00003034,
    JA_SE_ATM_RIVER_S_ISLE = 0x0000303A,
    JA_SE_ATM_SWIRL = 0x00001068,
    JA_SE_ATM_WATER_GATE = 0x0000701F,
    JA_SE_ATM_WIND_VAR = 0x0000106A,
    JA_SE_AUC_BID_GAUGE = 0x000008E4,
    JA_SE_CAMERA_C_MOVE = 0x0000001B,
    JA_SE_CAMERA_L_CANCEL = 0x0000081D,
    JA_SE_CAMERA_L_MOVE = 0x0000081C,
    JA_SE_CHR_PW_MOVE = 0x00007051,
    JA_SE_CHUYA_SWITCH = 0x000008DA,
    JA_SE_CM_AM2_JUMP2 = 0x000058B6,
    JA_SE_CM_AM2_LANDING = 0x000058B3,
    JA_SE_CM_AM_JUMP_L = 0x00005885,
    JA_SE_CM_AM_JUMP_S = 0x00005882,
    JA_SE_CM_BGN_BODY_RIPPLE = 0x00006A43,
    JA_SE_CM_BGN_D_KAZEKIRI = 0x00005973,
    JA_SE_CM_BGN_MECHA_ROPE = 0x0000705B,
    JA_SE_CM_BGN_MECHA_ROTATE = 0x0000705A,
    JA_SE_CM_BGN_M_HEAD_RIPPLE = 0x00006A44,
    JA_SE_CM_BGN_M_TAIL_RIPPL = 0x00006A45,
    JA_SE_CM_BGN_STRING_RIPPLE = 0x00006A42,
    JA_SE_CM_BGN_T_ROUND = 0x0000705C,
    JA_SE_CM_BKM_ATKVINE_DIG = 0x00005053,
    JA_SE_CM_BKM_ATKVINE_ENTER = 0x0000584C,
    JA_SE_CM_BKM_ATKVINE_L_ATK = 0x00005054,
    JA_SE_CM_BKM_VINE_SET = 0x0000584B,
    JA_SE_CM_BL_BOUND = 0x0000588C,
    JA_SE_CM_BOKOBOU_BOUND = 0x00005811,
    JA_SE_CM_BST_ARROW_BOUND = 0x00006988,
    JA_SE_CM_BST_BEAM_BOMB = 0x00006986,
    JA_SE_CM_BST_BEAM_FIRE = 0x000058E0,
    JA_SE_CM_BST_BOMB_BOUND = 0x00006989,
    JA_SE_CM_BST_GOKOU = 0x0000703A,
    JA_SE_CM_BST_HAND_SLEEPING = 0x00007037,
    JA_SE_CM_BST_HAND_WORKING = 0x00007036,
    JA_SE_CM_BST_HEAD_SLEEPING = 0x00007039,
    JA_SE_CM_BST_HEAD_WORKING = 0x00007038,
    JA_SE_CM_BST_HEART_BOUND = 0x00006991,
    JA_SE_CM_BST_HORI = 0x00007035,
    JA_SE_CM_BTD_ENT_BUBBLE = 0x00005825,
    JA_SE_CM_BTD_HIFUKI = 0x0000501F,
    JA_SE_CM_BTD_HIFUKI_BEF = 0x0000501E,
    JA_SE_CM_BWD_LAST_EXPLODE = 0x0000591B,
    JA_SE_CM_DK_BREAK_MASK = 0x00005875,
    JA_SE_CM_GY_CRUISING = 0x000050CE,
    JA_SE_CM_INOCHIDAMA_BLINK = 0x0000580E,
    JA_SE_CM_INOCHIDAMA_BREAK = 0x0000580D,
    JA_SE_CM_KS_DIE = 0x0000587C,
    JA_SE_CM_LANCE_BOUND = 0x00005807,
    JA_SE_CM_MAGBALL_BOUND = 0x00005815,
    JA_SE_CM_MAGTAIL_MOVE = 0x0000580F,
    JA_SE_CM_MONS_EXPLODE = 0x00005801,
    JA_SE_CM_NATA_BOUND = 0x00005814,
    JA_SE_CM_PG_FLYING = 0x0000303E,
    JA_SE_CM_PG_SWING_L = 0x00005951,
    JA_SE_CM_PRAPELLO_ROLLING = 0x000050BF,
    JA_SE_CM_PW_BECOME_SOLID = 0x00005135,
    JA_SE_CM_PW_SOLID_END = 0x00005936,
    JA_SE_CM_SH_CRUISING = 0x00005128,
    JA_SE_CM_SH_PROPELLER = 0x00005127,
    JA_SE_CM_SO_DIVE = 0x0000593B,
    JA_SE_CM_ST_CLUB_SWING_L = 0x00005895,
    JA_SE_CV_AM_JITABATA = 0x00004879,
    JA_SE_CV_G_SHIP_LAUGH = 0x00004925,
    JA_SE_CV_G_SHIP_SCREAM = 0x00004926,
    JA_SE_CV_KS_DAMAGE = 0x0000486E,
    JA_SE_CV_PG_DISAPPEAR = 0x00004944,
    JA_SE_CV_TT_DANCE_FIN = 0x000048A1,
    JA_SE_CV_WZ_ATTACK = 0x000048DA,
    JA_SE_CV_WZ_LAUGH = 0x000048D6,
    JA_SE_FIREBLAST_BLOW = 0x0000380D,
    JA_SE_FIREBLAST_NOISE = 0x00007009,
    JA_SE_FORCE_BACK = 0x000008B3,
    JA_SE_FT_ADD_GRASS = 0x00003814,
    JA_SE_FT_LADDER_CLIMB = 0x00003815,
    JA_SE_FT_LADDER_CLIMB_D = 0x00003818,
    JA_SE_FT_PW_RUN_AWAY = 0x0000383C,
    JA_SE_ITM_MENU_IN = 0x0000080F,
    JA_SE_ITM_MENU_OUT = 0x00000810,
    JA_SE_ITM_OMAMORI_TETLA = 0x00000854,
    JA_SE_LK_CUT_GRASS = 0x0000282C,
    JA_SE_LK_DAMAGE_LARGE = 0x0000282E,
    JA_SE_LK_DAMAGE_NORMAL = 0x0000282D,
    JA_SE_LK_INTO_WATER = 0x00003808,
    JA_SE_LK_LAST_HIT = 0x00002828,
    JA_SE_LK_RUN_SLIP = 0x0000280D,
    JA_SE_LK_SHIP_CANNON_FLY = 0x0000381F,
    JA_SE_LK_SLIP_SUS = 0x0000205A,
    JA_SE_LK_V_01_01 = 0x00001800,
    JA_SE_LK_WAPR_EFF_WARP = 0x0000285D,
    JA_SE_LK_WARP_TO_G_SHIP = 0x00002889,
    JA_SE_LK_WTAKT_SWING = 0x0000285F,
    JA_SE_L_FOCUS_RESET = 0x00000805,
    JA_SE_L_FOCUS_SET = 0x00000804,
    JA_SE_ME_ITEM_GET_S = 0x000008F5,
    JA_SE_MP_GAUGE_INC = 0x0000087D,
    JA_SE_OBJ_BAR_FRAME_BURN = 0x0000614F,
    JA_SE_OBJ_BOKKURI_BOUND = 0x000069F2,
    JA_SE_OBJ_BOKO_SEED_BOUND = 0x00006A0A,
    JA_SE_OBJ_BOMB_EXPLODE = 0x00006901,
    JA_SE_OBJ_BOMB_WATER = 0x00006982,
    JA_SE_OBJ_BREAK_WATER_POT = 0x00006921,
    JA_SE_OBJ_BRK_WRAIL_K = 0x0000693F,
    JA_SE_OBJ_BS_BOAT_CRUISE = 0x0000303D,
    JA_SE_OBJ_BS_MELODY_1 = 0x00006A2A,
    JA_SE_OBJ_BS_MELODY_2 = 0x00006A2B,
    JA_SE_OBJ_BTD_FIRE_DROP = 0x00006942,
    JA_SE_OBJ_COL_BRK_WRAILING = 0x00006847,
    JA_SE_OBJ_COL_SWC_CHNS = 0x00006806,
    JA_SE_OBJ_CUP_SWING = 0x000069E7,
    JA_SE_OBJ_DISH_SWING = 0x000069E8,
    JA_SE_OBJ_FER_WHEEL_ROUND = 0x0000107D,
    JA_SE_OBJ_HL_WAPR_EFF = 0x00001088,
    JA_SE_OBJ_HYOI_BOUND = 0x00006A0D,
    JA_SE_OBJ_H_PENDANT_BOUND = 0x00006A08,
    JA_SE_OBJ_KASSHA_LIFT_MOVE = 0x00003822,
    JA_SE_OBJ_KM_WINDMILL = 0x000061B1,
    JA_SE_OBJ_LADDER_FALL_2 = 0x00006955,
    JA_SE_OBJ_LUPY_BOUND = 0x00006A06,
    JA_SE_OBJ_LUPY_IN_POT = 0x00006981,
    JA_SE_OBJ_L_BRIDGE_OFF = 0x00006957,
    JA_SE_OBJ_L_BRIDGE_ON = 0x00006956,
    JA_SE_OBJ_M_POT_BOUND = 0x00006A07,
    JA_SE_OBJ_OTOSHIANA_OPEN = 0x00006934,
    JA_SE_OBJ_OTOSHIANA_SUS = 0x00003019,
    JA_SE_OBJ_PG_EBALL_EXP_L = 0x00006A36,
    JA_SE_OBJ_PIRATE_CRUISE = 0x00007034,
    JA_SE_OBJ_PIRATE_SAIL = 0x000050BE,
    JA_SE_OBJ_RACK_BREAK = 0x0000696D,
    JA_SE_OBJ_SALVAGE_TAG_NEAR = 0x000010A9,
    JA_SE_OBJ_SEARCH_LIGHT = 0x00006131,
    JA_SE_OBJ_SPRING = 0x000061FE,
    JA_SE_OBJ_STEAM = 0x0000612E,
    JA_SE_OBJ_STN_DOOR_MOVE_D = 0x00006908,
    JA_SE_OBJ_STN_DOOR_MOVE_U = 0x00006907,
    JA_SE_OBJ_STN_DOOR_STL_BAR = 0x00006906,
    JA_SE_OBJ_STN_DOOR_STOP_D = 0x0000690A,
    JA_SE_OBJ_STN_DOOR_STOP_U = 0x00006909,
    JA_SE_OBJ_ST_CLUB_FALL = 0x00006999,
    JA_SE_OBJ_S_NECKLACE_BOUND = 0x00006A09,
    JA_SE_OBJ_TN_SWORD_FALL = 0x00006998,
    JA_SE_OBJ_TORCH_BURNING = 0x00006103,
    JA_SE_OBJ_TORNADE_WARP = 0x0000186E,
    JA_SE_OBJ_TREE_SWING = 0x00006932,
    JA_SE_OBJ_TREE_SWING_S = 0x000069E6,
    JA_SE_OBJ_V_BAIT_BOUND = 0x00006A0C,
    JA_SE_OBJ_WIND_TAG = 0x0000701D,
    JA_SE_OBJ_WIND_TAG_L = 0x0000701E,
    JA_SE_OBJ_W_EMBLEM_BOUND = 0x00006A0B,
    JA_SE_OP_ENTER_GAME = 0x0000082F,
    JA_SE_PLAYSPOT_END_S = 0x000008AB,
    JA_SE_PRE_TAKT = 0x000008A7,
    JA_SE_QUIT_GAME = 0x000008C1,
    JA_SE_SHIP_SAIL_OUT = 0x00002831,
    JA_SE_TAKT_WIND_DEMO = 0x00000876,
    JA_SE_TALK_END = 0x00000825,
    JA_SE_TALK_NEXT = 0x0000080B,
    JA_SE_TALK_START = 0x0000080A,
    JA_SE_TALK_WIN_CLOSE = 0x0000080D,
    JA_SE_TALK_WIN_OPEN = 0x0000080C,
    JA_SE_TELESCOPE_ZOOM = 0x00000008,
    JA_SE_WTAKT_AMBIENT = 0x00002066,
    JA_SE_WTAKT_ARM_SWING = 0x00002867,
    JA_SE_WTAKT_AUFTAKT = 0x00002868,
    JA_SE_WTAKT_METRONOME = 0x00000887,
};

using GuestByte = be<u8>;
struct Sound : ::JAISound {
    void stop(u32);
    void setPortData(u32, u32);
    void setVolume(f32, u32, u32);
    void setPitch(f32, u32, u32);
    void setPan(f32, u32, u32);
    void setDolby(f32, u32, u32);
    void *getSeqParameter();
    void setTrackVolume(u32, f32, u32);
    void setSeqInterVolume(u32, f32, u32);
    void setTempoProportion(f32, u32);
};
struct SoundHead {
    be<u8> field_0x4, mState;
};
struct AudioCamera {
    gptr<cXyz> field_0x0, field_0x4;
    /* 0x04 */ gptr<Mtx34> field_0x8;
};
struct VectorWords {
    be<u32> w[3];
};
struct MonsterSlot {
    be<u32> field_0x00;
    gptr<Sound> field_0x04;
};
struct AllocationRecord {
    be<u32> count;
    gptr<Sound> sound;
};
struct Random {
    /* 0x04 */ be<u32> field_0x0, field_0x4, field_0x8[32];
};
struct SeqUpdateData {
    u8 padding[0x48];
    gptr<Sound> mSequence;
};
struct LevelObjectInstance {
    be<f32> volume, pan, surround;
    be<u8> reverb;
    u8 padding[3];
};
struct LevelObjectSound {
    be<u32> id, count;
    LevelObjectInstance instances[20];
};
struct JAIZelBasic {
    Sound *makeSound(s32 n);
    void kuroboMotionPlay(u32 id, cXyz *pos, u32 info, u32 reverb);
    s32 checkSePlaying(u32 id);
    s32 seLimitInstances(u32 id, cXyz *pos, s32 prio, s32 maxCount, f32 maxDist);
    void stopBattleBgm();
    void subBgmStop();
    void subBgmStopInner();
    s32 checkStreamPlaying(u32 id);
    void talkOut(u32 fade);
    void menuOut();
    void bgmMute(gptr<Sound> *h, u32 bgm, u32 set, u32 fade);
    void subBgmStart(u32 id);
    s32 checkEventBit(u32 flag);
    void onEnemyDamage();
    void seStop(u32 id, u32 fade);
    s32 isDemo();
    void menuIn();
    void setLevObjSE(u32 id, cXyz *pos, u32 reverb);
    gptr<Sound> *seStart(u32 id, cXyz *pos, u32 info, u32 reverb, f32 pitch, f32 volume, f32 pan,
                         f32 dolby, u32 flag);
    void startSoundVec(u32, gptr<Sound> *, cXyz *, u32, u32, u32);
    void setSeCategoryVolume(u32, u32);
    void deleteObject(cXyz *);
    f32 calcPosVolume(VectorWords *, f32);
    f32 calcPosPanLR(VectorWords *);
    f32 calcPosPanSR(VectorWords *, f32);
    AudioCamera *camera() {
        return gabi::at<AudioCamera>(mAudioCamera.v);
    }
    /* 0x0 */ gptr<AudioCamera> mAudioCamera;
    /* 0x04 */ gptr<void> field_0x8;
    u8 padding8[0x24];
    /* 0x2C */ gptr<void> __vtbl;
    /* 0x30 */ be<u8> field_0x0020;
    /* 0x31 */ be<u8> field_0x0021;
    u8 padding32[0x2];
    /* 0x34 */ gptr<be<u8>> field_0x0024;
    /* 0x38 */ be<u32> field_0x0028;
    /* 0x3C */ be<u8> mHour;
    /* 0x3D */ be<u8> mMinute;
    /* 0x3E */ be<u8> mWeekday;
    /* 0x3F */ be<u8> field_0x002f;
    /* 0x40 */ be<u16> field_0x0030;
    /* 0x42 */ be<u8> field_0x0032;
    /* 0x43 */ be<u8> field_0x0033;
    /* 0x44 */ be<s32> field_0x0034;
    /* 0x48 */ be<s32> field_0x0038;
    /* 0x4C */ be<f32> field_0x003c;
    /* 0x50 */ be<u8> field_0x0040;
    /* 0x51 */ be<u8> field_0x0041;
    /* 0x52 */ be<u8> field_0x0042;
    /* 0x53 */ be<u8> field_0x0043;
    /* 0x54 */ be<u8> field_0x0044;
    /* 0x55 */ be<u8> field_0x0045;
    /* 0x56 */ be<u8> field_0x0046;
    /* 0x57 */ be<u8> field_0x0047;
    /* 0x58 */ cXyz field_0x0048;
    /* 0x64 */ cXyz field_0x0054;
    /* 0x70 */ be<u8> mbLandingDemoStarted;
    /* 0x71 */ be<u8> field_0x0061;
    /* 0x72 */ be<u8> field_0x0062;
    /* 0x73 */ be<u8> field_0x0063;
    /* 0x74 */ be<u8> field_0x0064;
    /* 0x75 */ be<u8> field_0x0065;
    /* 0x76 */ be<u8> field_0x0066;
    u8 padding77[0x1];
    /* 0x78 */ gptr<Sound> mpMainBgmSound;
    /* 0x7C */ gptr<Sound> mpSubBgmSound;
    /* 0x80 */ gptr<Sound> mpStreamBgmSound;
    /* 0x84 */ be<u32> mSubBgmNum;
    /* 0x88 */ be<u32> mMainBgmNum;
    /* 0x8C */ be<u32> mStreamBgmNum;
    /* 0x90 */ be<f32> field_0x0080;
    /* 0x94 */ be<f32> field_0x0084;
    /* 0x98 */ be<f32> field_0x0088;
    /* 0x9C */ be<f32> field_0x008c;
    /* 0xA0 */ be<f32> field_0x0090;
    /* 0xA4 */ be<f32> field_0x0094;
    /* 0xA8 */ be<f32> field_0x0098;
    /* 0xAC */ be<f32> field_0x009c;
    /* 0xB0 */ be<f32> field_0x00a0;
    /* 0xB4 */ be<f32> field_0x00a4;
    /* 0xB8 */ be<f32> field_0x00a8;
    /* 0xBC */ be<f32> field_0x00ac;
    /* 0xC0 */ be<u32> field_0x00b0;
    /* 0xC4 */ be<u8> field_0x00b4;
    /* 0xC5 */ be<u8> mSomeSpecialBGMFlag;
    /* 0xC6 */ be<u8> mLastMinibossSubBGMType;
    /* 0xC7 */ be<u8> field_0x00b7;
    /* 0xC8 */ be<u8> field_0x00b8;
    /* 0xC9 */ be<u8> field_0x00b9;
    /* 0xCA */ be<u8> field_0x00ba;
    /* 0xCB */ be<u8> field_0x00bb;
    /* 0xCC */ be<u8> field_0x00bc;
    /* 0xCD */ be<u8> field_0x00bd;
    /* 0xCE */ be<u8> field_0x00be;
    /* 0xCF */ be<u8> field_0x00bf;
    /* 0xD0 */ be<u8> field_0x00c0;
    /* 0xD1 */ be<s8> field_0x00c1;
    u8 paddingD2[0x2];
    /* 0xD4 */ be<u32> field_0x00c4;
    /* 0xD8 */ be<u8> field_0x00c8;
    /* 0xD9 */ be<u8> field_0x00c9;
    /* 0xDA */ be<u8> field_0x00ca;
    /* 0xDB */ be<s8> field_0x00cb;
    /* 0xDC */ be<u8> field_0x00cc;
    /* 0xDD */ be<u8> field_0x00cd;
    /* 0xDE */ be<u8> field_0x00ce;
    u8 paddingDF[0x1];
    /* 0xE0 */ gptr<cXyz> field_0x00d0;
    /* 0xE4 */ gptr<Sound> mpSeSound[32];
    /* 0x164 */ be<u32> mSeNum[32];
    /* 0x1E4 */ be<u32> field_0x0194[32];
    /* 0x264 */ be<u32> field_0x01f4;
    /* 0x268 */ be<u8> field_0x01f8;
    /* 0x269 */ be<u8> field_0x01f9;
    /* 0x26A */ be<u8> field_0x01fa;
    /* 0x26B */ be<u8> field_0x01fb;
    /* 0x26C */ be<u8> field_0x01fc;
    /* 0x26D */ be<u8> field_0x01fd;
    /* 0x26E */ be<u8> field_0x01fe;
    /* 0x26F */ be<u8> field_0x01ff;
    /* 0x270 */ be<u8> field_0x0200;
    /* 0x271 */ be<u8> field_0x0201;
    /* 0x272 */ be<u8> field_0x0202[2];
    /* 0x274 */ be<u8> field_0x0204;
    /* 0x275 */ be<u8> field_0x0205;
    /* 0x276 */ be<u8> field_0x0206;
    /* 0x277 */ be<u8> field_0x0207;
    /* 0x278 */ be<u8> field_0x0208;
    u8 padding279[0x3];
    /* 0x27C */ be<u32> field_0x020c;
    /* 0x280 */ be<u32> field_0x0210;
    /* 0x284 */ gptr<Sound> field_0x0214;
    /* 0x288 */ be<u32> field_0x0218;
    /* 0x28C */ be<u8> mCameraSeaFloorGroupInfo;
    /* 0x28D */ be<u8> mLinkSeaFloorGroupInfo;
    /* 0x28E */ be<u8> field_0x021e;
    u8 padding28F[0x1];
    /* 0x290 */ be<s32> field_0x0220;
    /* 0x294 */ be<s32> field_0x0224;
    /* 0x298 */ be<u32> mNextSceneBgmId;
    /* 0x29C */ be<u8> field_0x022c;
    /* 0x29D */ be<u8> field_0x022d;
    /* 0x29E */ be<u8> field_0x022e;
    /* 0x29F */ be<u8> field_0x022f;
    /* 0x2A0 */ be<u8> field_0x0230;
    /* 0x2A1 */ be<u8> field_0x0231;
    /* 0x2A2 */ be<u8> mFirstDynamicSceneWaveIndex;
    /* 0x2A3 */ be<u8> field_0x0233;
    /* 0x2A4 */ be<u8> mSetNum;
    /* 0x2A5 */ be<u8> mSecondDynamicSceneWaveIndex;
    /* 0x2A6 */ be<u8> field_0x0236;
    /* 0x2A7 */ be<u8> field_0x0237;
    /* 0x2A8 */ be<u8> mIslandRoomNo;
    /* 0x2A9 */ be<u8> field_0x0239;
    /* 0x2AA */ be<u8> field_0x023a;
    u8 padding2AB[0x1];
    /* 0x2AC */ be<u32> mBgmWaveCountHD;
    /* 0x2B0 */ gptr<be<u8>> mBgmWaveInfoHD;
    /* 0x2B4 */ LevelObjectSound mLevelObjectSoundsHD[15];
    /* 0x15EC */ be<u32> mLevelObjectSoundCountHD;
    u8 padding15F0[0x600];
    /* 0x1BF0 */ be<u32> field_0x1b80;
    /* 0x1BF4 */ cXyz mSeaPositionHD;
    u8 padding1C00[0x240];
    /* 0x1E40 */ be<u32> field_0x1dd0;
    /* 0x1E44 */ be<u8> field_0x1dd4;
    u8 padding1E45[0xC3];
    /* 0x1F08 */ be<u32> field_0x1e98;
    u8 padding1F0C[0x24];
    /* 0x1F30 */ be<u32> field_0x1ec0;
    /* 0x1F34 */ be<u32> field_0x1ec4;
    /* 0x1F38 */ be<u32> field_0x1ec8;
    /* 0x1F3C */ be<u32> field_0x1ecc;
    /* 0x1F40 */ be<u32> field_0x1ed0;
    /* 0x1F44 */ VectorWords field_0x1ed4[8];
    /* 0x1FA4 */ be<u32> field_0x1f34;
    /* 0x1FA8 */ gptr<Sound> field_0x1f38;
    /* 0x1FAC */ be<u8> field_0x1f3c;
    /* 0x1FAD */ be<u8> mIsSailing;
    u8 padding1FAE[0x2];
    /* 0x1FB0 */ be<f32> field_0x1f40;
    /* 0x1FB4 */ gptr<Sound> field_0x1f44;
    /* 0x1FB8 */ gptr<Sound> field_0x1f48;
    /* 0x1FBC */ MonsterSlot field_0x1f4c[30];
    /* 0x20AC */ be<u8> field_0x203c;
    /* 0x20AD */ be<u8> field_0x203d;
    u8 padding20AE[0x2];
    /* 0x20B0 */ gptr<Sound> field_0x2040[4];
    /* 0x20C0 */ gptr<Sound> field_0x2050[4];
    /* 0x20D0 */ gptr<Sound> field_0x2060;
    /* 0x20D4 */ be<u16> mVoiceIndexHD;
    u8 padding20D6[0xE];
    /* 0x20E4 */ gptr<Sound> mSystemSoundHD;
    /* 0x20E8 */ AllocationRecord mAllocationRecordsHD[16];
    /* 0x2168 */ be<u32> mAllocationRecordIndexHD;
    /* 0x216C */ Random mRandomHD;
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, __vtbl, 0x2C);
WWHD_OFFSET(JAIZelBasic, field_0x0020, 0x30);
WWHD_OFFSET(JAIZelBasic, field_0x0021, 0x31);
WWHD_OFFSET(JAIZelBasic, field_0x0024, 0x34);
WWHD_OFFSET(JAIZelBasic, field_0x0028, 0x38);
WWHD_OFFSET(JAIZelBasic, mHour, 0x3C);
WWHD_OFFSET(JAIZelBasic, mMinute, 0x3D);
WWHD_OFFSET(JAIZelBasic, mWeekday, 0x3E);
WWHD_OFFSET(JAIZelBasic, field_0x002f, 0x3F);
WWHD_OFFSET(JAIZelBasic, field_0x0030, 0x40);
WWHD_OFFSET(JAIZelBasic, field_0x0032, 0x42);
WWHD_OFFSET(JAIZelBasic, field_0x0033, 0x43);
WWHD_OFFSET(JAIZelBasic, field_0x0034, 0x44);
WWHD_OFFSET(JAIZelBasic, field_0x0038, 0x48);
WWHD_OFFSET(JAIZelBasic, field_0x003c, 0x4C);
WWHD_OFFSET(JAIZelBasic, field_0x0040, 0x50);
WWHD_OFFSET(JAIZelBasic, field_0x0041, 0x51);
WWHD_OFFSET(JAIZelBasic, field_0x0042, 0x52);
WWHD_OFFSET(JAIZelBasic, field_0x0043, 0x53);
WWHD_OFFSET(JAIZelBasic, field_0x0044, 0x54);
WWHD_OFFSET(JAIZelBasic, field_0x0045, 0x55);
WWHD_OFFSET(JAIZelBasic, field_0x0046, 0x56);
WWHD_OFFSET(JAIZelBasic, field_0x0047, 0x57);
WWHD_OFFSET(JAIZelBasic, field_0x0048, 0x58);
WWHD_OFFSET(JAIZelBasic, field_0x0054, 0x64);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, field_0x0061, 0x71);
WWHD_OFFSET(JAIZelBasic, field_0x0062, 0x72);
WWHD_OFFSET(JAIZelBasic, field_0x0063, 0x73);
WWHD_OFFSET(JAIZelBasic, field_0x0064, 0x74);
WWHD_OFFSET(JAIZelBasic, field_0x0065, 0x75);
WWHD_OFFSET(JAIZelBasic, field_0x0066, 0x76);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mpSubBgmSound, 0x7C);
WWHD_OFFSET(JAIZelBasic, mpStreamBgmSound, 0x80);
WWHD_OFFSET(JAIZelBasic, mSubBgmNum, 0x84);
WWHD_OFFSET(JAIZelBasic, mMainBgmNum, 0x88);
WWHD_OFFSET(JAIZelBasic, mStreamBgmNum, 0x8C);
WWHD_OFFSET(JAIZelBasic, field_0x0080, 0x90);
WWHD_OFFSET(JAIZelBasic, field_0x0084, 0x94);
WWHD_OFFSET(JAIZelBasic, field_0x0088, 0x98);
WWHD_OFFSET(JAIZelBasic, field_0x008c, 0x9C);
WWHD_OFFSET(JAIZelBasic, field_0x0090, 0xA0);
WWHD_OFFSET(JAIZelBasic, field_0x0094, 0xA4);
WWHD_OFFSET(JAIZelBasic, field_0x0098, 0xA8);
WWHD_OFFSET(JAIZelBasic, field_0x009c, 0xAC);
WWHD_OFFSET(JAIZelBasic, field_0x00a0, 0xB0);
WWHD_OFFSET(JAIZelBasic, field_0x00a4, 0xB4);
WWHD_OFFSET(JAIZelBasic, field_0x00a8, 0xB8);
WWHD_OFFSET(JAIZelBasic, field_0x00ac, 0xBC);
WWHD_OFFSET(JAIZelBasic, field_0x00b0, 0xC0);
WWHD_OFFSET(JAIZelBasic, field_0x00b4, 0xC4);
WWHD_OFFSET(JAIZelBasic, mSomeSpecialBGMFlag, 0xC5);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, field_0x00b7, 0xC7);
WWHD_OFFSET(JAIZelBasic, field_0x00b8, 0xC8);
WWHD_OFFSET(JAIZelBasic, field_0x00b9, 0xC9);
WWHD_OFFSET(JAIZelBasic, field_0x00ba, 0xCA);
WWHD_OFFSET(JAIZelBasic, field_0x00bb, 0xCB);
WWHD_OFFSET(JAIZelBasic, field_0x00bc, 0xCC);
WWHD_OFFSET(JAIZelBasic, field_0x00bd, 0xCD);
WWHD_OFFSET(JAIZelBasic, field_0x00be, 0xCE);
WWHD_OFFSET(JAIZelBasic, field_0x00bf, 0xCF);
WWHD_OFFSET(JAIZelBasic, field_0x00c0, 0xD0);
WWHD_OFFSET(JAIZelBasic, field_0x00c1, 0xD1);
WWHD_OFFSET(JAIZelBasic, field_0x00c4, 0xD4);
WWHD_OFFSET(JAIZelBasic, field_0x00c8, 0xD8);
WWHD_OFFSET(JAIZelBasic, field_0x00c9, 0xD9);
WWHD_OFFSET(JAIZelBasic, field_0x00ca, 0xDA);
WWHD_OFFSET(JAIZelBasic, field_0x00cb, 0xDB);
WWHD_OFFSET(JAIZelBasic, field_0x00cc, 0xDC);
WWHD_OFFSET(JAIZelBasic, field_0x00cd, 0xDD);
WWHD_OFFSET(JAIZelBasic, field_0x00ce, 0xDE);
WWHD_OFFSET(JAIZelBasic, field_0x00d0, 0xE0);
WWHD_OFFSET(JAIZelBasic, mpSeSound, 0xE4);
WWHD_OFFSET(JAIZelBasic, mSeNum, 0x164);
WWHD_OFFSET(JAIZelBasic, field_0x0194, 0x1E4);
WWHD_OFFSET(JAIZelBasic, field_0x01f4, 0x264);
WWHD_OFFSET(JAIZelBasic, field_0x01f8, 0x268);
WWHD_OFFSET(JAIZelBasic, field_0x01f9, 0x269);
WWHD_OFFSET(JAIZelBasic, field_0x01fa, 0x26A);
WWHD_OFFSET(JAIZelBasic, field_0x01fb, 0x26B);
WWHD_OFFSET(JAIZelBasic, field_0x01fc, 0x26C);
WWHD_OFFSET(JAIZelBasic, field_0x01fd, 0x26D);
WWHD_OFFSET(JAIZelBasic, field_0x01fe, 0x26E);
WWHD_OFFSET(JAIZelBasic, field_0x01ff, 0x26F);
WWHD_OFFSET(JAIZelBasic, field_0x0200, 0x270);
WWHD_OFFSET(JAIZelBasic, field_0x0201, 0x271);
WWHD_OFFSET(JAIZelBasic, field_0x0202, 0x272);
WWHD_OFFSET(JAIZelBasic, field_0x0204, 0x274);
WWHD_OFFSET(JAIZelBasic, field_0x0205, 0x275);
WWHD_OFFSET(JAIZelBasic, field_0x0206, 0x276);
WWHD_OFFSET(JAIZelBasic, field_0x0207, 0x277);
WWHD_OFFSET(JAIZelBasic, field_0x0208, 0x278);
WWHD_OFFSET(JAIZelBasic, field_0x020c, 0x27C);
WWHD_OFFSET(JAIZelBasic, field_0x0210, 0x280);
WWHD_OFFSET(JAIZelBasic, field_0x0214, 0x284);
WWHD_OFFSET(JAIZelBasic, field_0x0218, 0x288);
WWHD_OFFSET(JAIZelBasic, mCameraSeaFloorGroupInfo, 0x28C);
WWHD_OFFSET(JAIZelBasic, mLinkSeaFloorGroupInfo, 0x28D);
WWHD_OFFSET(JAIZelBasic, field_0x021e, 0x28E);
WWHD_OFFSET(JAIZelBasic, field_0x0220, 0x290);
WWHD_OFFSET(JAIZelBasic, field_0x0224, 0x294);
WWHD_OFFSET(JAIZelBasic, mNextSceneBgmId, 0x298);
WWHD_OFFSET(JAIZelBasic, field_0x022c, 0x29C);
WWHD_OFFSET(JAIZelBasic, field_0x022d, 0x29D);
WWHD_OFFSET(JAIZelBasic, field_0x022e, 0x29E);
WWHD_OFFSET(JAIZelBasic, field_0x022f, 0x29F);
WWHD_OFFSET(JAIZelBasic, field_0x0230, 0x2A0);
WWHD_OFFSET(JAIZelBasic, field_0x0231, 0x2A1);
WWHD_OFFSET(JAIZelBasic, mFirstDynamicSceneWaveIndex, 0x2A2);
WWHD_OFFSET(JAIZelBasic, field_0x0233, 0x2A3);
WWHD_OFFSET(JAIZelBasic, mSetNum, 0x2A4);
WWHD_OFFSET(JAIZelBasic, mSecondDynamicSceneWaveIndex, 0x2A5);
WWHD_OFFSET(JAIZelBasic, field_0x0236, 0x2A6);
WWHD_OFFSET(JAIZelBasic, field_0x0237, 0x2A7);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_OFFSET(JAIZelBasic, field_0x0239, 0x2A9);
WWHD_OFFSET(JAIZelBasic, field_0x023a, 0x2AA);
WWHD_OFFSET(JAIZelBasic, mBgmWaveCountHD, 0x2AC);
WWHD_OFFSET(JAIZelBasic, mBgmWaveInfoHD, 0x2B0);
WWHD_OFFSET(JAIZelBasic, mLevelObjectSoundCountHD, 0x15EC);
WWHD_OFFSET(JAIZelBasic, field_0x1b80, 0x1BF0);
WWHD_OFFSET(JAIZelBasic, mSeaPositionHD, 0x1BF4);
WWHD_OFFSET(JAIZelBasic, field_0x1dd0, 0x1E40);
WWHD_OFFSET(JAIZelBasic, field_0x1dd4, 0x1E44);
WWHD_OFFSET(JAIZelBasic, field_0x1e98, 0x1F08);
WWHD_OFFSET(JAIZelBasic, field_0x1ec0, 0x1F30);
WWHD_OFFSET(JAIZelBasic, field_0x1ec4, 0x1F34);
WWHD_OFFSET(JAIZelBasic, field_0x1ec8, 0x1F38);
WWHD_OFFSET(JAIZelBasic, field_0x1ecc, 0x1F3C);
WWHD_OFFSET(JAIZelBasic, field_0x1ed0, 0x1F40);
WWHD_OFFSET(JAIZelBasic, field_0x1f34, 0x1FA4);
WWHD_OFFSET(JAIZelBasic, field_0x1f38, 0x1FA8);
WWHD_OFFSET(JAIZelBasic, field_0x1f3c, 0x1FAC);
WWHD_OFFSET(JAIZelBasic, mIsSailing, 0x1FAD);
WWHD_OFFSET(JAIZelBasic, field_0x1f40, 0x1FB0);
WWHD_OFFSET(JAIZelBasic, field_0x1f44, 0x1FB4);
WWHD_OFFSET(JAIZelBasic, field_0x1f48, 0x1FB8);
WWHD_OFFSET(JAIZelBasic, field_0x1f4c, 0x1FBC);
WWHD_OFFSET(JAIZelBasic, field_0x203c, 0x20AC);
WWHD_OFFSET(JAIZelBasic, field_0x203d, 0x20AD);
WWHD_OFFSET(JAIZelBasic, field_0x2040, 0x20B0);
WWHD_OFFSET(JAIZelBasic, field_0x2050, 0x20C0);
WWHD_OFFSET(JAIZelBasic, field_0x2060, 0x20D0);
WWHD_OFFSET(JAIZelBasic, mVoiceIndexHD, 0x20D4);
WWHD_OFFSET(JAIZelBasic, mSystemSoundHD, 0x20E4);
WWHD_OFFSET(JAIZelBasic, mAllocationRecordsHD, 0x20E8);
WWHD_OFFSET(JAIZelBasic, mAllocationRecordIndexHD, 0x2168);
WWHD_OFFSET(JAIZelBasic, mRandomHD, 0x216C);
WWHD_SIZE(JAIZelBasic, 0x21F4);
WWHD_SIZE(AudioCamera, 12);
WWHD_SIZE(VectorWords, 12);
WWHD_SIZE(MonsterSlot, 8);
WWHD_SIZE(AllocationRecord, 8);
WWHD_SIZE(Random, 136);
} // namespace jaizel12

namespace jaizel12 {
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestByte = be<u8>;
using ManagerPtr = gptr<void>;
struct CategoryVolumes {
    GuestByte entries[8];
};
struct MuteMasks {
    GuestWord masks[256][4];
};
struct SoundIds {
    be<u16> entries[15];
};
struct HeapVtable {
    u8 padding[0x34];
    gabi::gfn allocate;
};
struct Heap {
    u8 padding[0xC];
    gptr<HeapVtable> __vtbl;
};
inline Sound *allocateSoundArray(Heap *heap, u32 size) {
    u32 vtable = gabi::load<u32>(gabi::ea(heap) + 0xC);
    return gabi::call_ptr<Sound *>(gabi::load<u32>(vtable + 0x34), heap, size, 4);
}
extern ManagerPtr &l_soundManager;
extern GuestFloat &VOL_BGM_DEFAULT;
extern GuestFloat &VOL_BGM_PAUSING;
extern CategoryVolumes &l_defaultCategoryVolumes;
extern CategoryVolumes &l_pauseCategoryVolumes;
extern GuestByte &MBOSS_BTL_SWORD_USING_TRACK;
extern GuestWord &l_demoState;
extern MuteMasks &m_bgm_mute_state;
extern ManagerPtr &l_defaultHeap;
extern ManagerPtr &l_heapManager;
extern VectorWords &l_defaultLevelPosition;
extern VectorWords &l_defaultSePosition;
extern GuestFloat &l_pitchIncrement;
extern SoundIds &l_attackSoundIds;
// HD: guest addresses and table indices wrap at32 bits, including nullable camera reads.
inline Sound *soundAtByteOffset(Sound *array, u32 offset) {
    return gabi::at<Sound>(gabi::ea(array) + offset);
}
template <typename T> inline T *indexed(T *p, u32 n) {
    return gabi::at<T>(gabi::ea(p) + n * u32(sizeof(T)));
}
inline cXyz *cameraPosition(AudioCamera *p) {
    return gabi::at<cXyz>(gabi::load<u32>(gabi::ea(p)));
}
inline Mtx34 *cameraMatrix(AudioCamera *p) {
    return gabi::at<Mtx34>(gabi::load<u32>(gabi::ea(p) + 8));
}
inline f32 cameraHeight(AudioCamera *p) {
    return gabi::load<f32>(gabi::ea(cameraPosition(p)) + 4);
}
inline s32 soundPriority(Sound *p) {
    return gabi::load<s32>(gabi::ea(p) + 0x14);
}
inline cXyz *soundPosition(Sound *p) {
    return gabi::at<cXyz>(gabi::load<u32>(gabi::ea(p) + 0x24));
}
inline be<u16> *attackSoundId(u32 info) {
    return gabi::at<be<u16>>(gabi::ea(l_attackSoundIds.entries) + info * 2);
}
inline void *rootTrack(void *p) {
    return gabi::at<void>(gabi::ea(p) + 0x1360);
}
inline f32 mul(f32 a, f32 b) {
    return gabi::fmuls_ppc(a, b);
}
} // namespace jaizel12
void *hdStreamPlayer(void *);
s32 hdCheckStream(void *, u32, u32, u32, u32);
void *getDefaultAudioHeap(void *);
void constructZelSound(jaizel12::Sound *);
void writeTrackPort(void *, u32, u32);
void subtractSoundVector(cXyz *, cXyz *, jaizel12::VectorWords *);
f32 soundVectorMagnitude(jaizel12::VectorWords *);
void transformSoundVector(Mtx34 *, jaizel12::VectorWords *, jaizel12::VectorWords *);
f32 getParamDistanceMax();
f32 std_sqrtf(f32);
u32 checkSoundPointer(cXyz *);
WWHD_SIZE(jaizel12::LevelObjectSound, 0x148);
WWHD_SIZE(jaizel12::LevelObjectInstance, 16);
WWHD_OFFSET(jaizel12::JAIZelBasic, mLevelObjectSoundsHD, 0x2B4);
WWHD_OFFSET(jaizel12::JAIZelBasic, field_0x1ed4, 0x1F44);
WWHD_OFFSET(jaizel12::JAIZelBasic, field_0x8, 4);

WWHD_OFFSET(jaizel12::LevelObjectSound, id, 0);
WWHD_OFFSET(jaizel12::LevelObjectSound, count, 4);
WWHD_OFFSET(jaizel12::LevelObjectSound, instances, 8);
WWHD_OFFSET(jaizel12::LevelObjectInstance, volume, 0);
WWHD_OFFSET(jaizel12::LevelObjectInstance, pan, 4);
WWHD_OFFSET(jaizel12::LevelObjectInstance, surround, 8);
WWHD_OFFSET(jaizel12::LevelObjectInstance, reverb, 12);
WWHD_SIZE(jaizel12::Sound, 0x48);
WWHD_SIZE(jaizel12::CategoryVolumes, 8);
WWHD_SIZE(jaizel12::MuteMasks, 0x1000);
WWHD_SIZE(jaizel12::SoundIds, 30);
WWHD_SIZE(jaizel12::Heap, 0x10);
WWHD_SIZE(jaizel12::HeapVtable, 0x38);
