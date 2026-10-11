/* daPy_py_c (the player base) and the matrix-follow emitter callback, WWHD layouts with GameCube
 * names.
 *
 * daPy_py_c: every GameCube member +0x11C (fopAc_ac_c grew by 0x11C), size 0x43C (GameCube 0x320).
 * Verified offsets: mCutType 0x3AC (getCutType in bk, am2, mo2), mQuakeTimer 0x3B2, mNoResetFlg0
 * 0x3B8, mNoResetFlg1 0x3BC / mResetFlg0 0x3C0 (daPy_lk_c accessors), mWindVelocity (GameCube
 * field_0x2f8) 0x414. The flag enums are GameCube's; HD uses the same bits where verified code
 * tests them. */
#pragma once
#include "f_op/f_op_actor.h"

struct daPy_mtxFollowEcallBack_c {
    be<u32> mVtable;
    gptr<void> mpEmitter;
    gptr<void> mpMatrix;
};
WWHD_SIZE(daPy_mtxFollowEcallBack_c, 0xC);

/* daPy_demo_c (0x18 + padding, unchanged from GameCube) */
struct daPy_demo_c {
    enum { TYPE_NONE_e, TYPE_TOOL_e, TYPE_SYSTEM_e, TYPE_ORIGINAL_e, TYPE_START_e, TYPE_SPECIAL_e };
    enum {
        DEMO_UNK00_e = 0,
        DEMO_N_WAIT_e = 1,
        DEMO_N_WALK_e = 2,
        DEMO_N_DASH_e = 3,
        DEMO_INIT_WAIT_e = 4,
        DEMO_WAIT_TURN_e = 5,
        DEMO_N_TALK_e = 6,
        DEMO_DAM_ACTION_e = 7,
        DEMO_E_TALK_e = 8,
        DEMO_LDAM_e = 9,
        DEMO_OPEN_TREASURE_e = 10,
        DEMO_GET_ITEM_e = 11,
        DEMO_UNEQUIP_e = 12,
        DEMO_HOLDUP_e = 13,
        DEMO_KEEP_e = 14,
        DEMO_LAROUND_e = 15,
        DEMO_BJUMP_e = 16,
        DEMO_PFALL_e = 17,
        DEMO_UNK_018_e = 18,
        DEMO_SALUTE_e = 19,
        DEMO_L_AROUND2_e = 20,
        DEMO_T_PICKUP_e = 21,
        DEMO_T_WAIT_e = 22,
        DEMO_A_WAIT_e = 23,
        DEMO_SURPRISED_e = 24,
        DEMO_TBACK_e = 25,
        DEMO_LOOKUP_e = 26,
        DEMO_UNK_027_e = 27,
        DEMO_UNK_028_e = 28,
        DEMO_UNK_029_e = 29,
        DEMO_UNK_030_e = 30,
        DEMO_LWAIT_e = 31,
        DEMO_PWAIT_032_e = 32,
        DEMO_PUSH_e = 33,
        DEMO_TACT_e = 34,
        DEMO_DOOR_e = 35,
        DEMO_NOD_e = 36,
        DEMO_PRESENT_e = 37,
        DEMO_WCHANGE_e = 38,
        DEMO_SHIP_e = 39,
        DEMO_SIPUT_e = 40,
        DEMO_UNK_041_e = 41,
        DEMO_KM_WAIT_e = 42,
        DEMO_CUT_ROLL_e = 43,
        DEMO_POS_INIT_e = 44,
        DEMO_UNK_045_e = 45,
        DEMO_PGET_e = 46,
        DEMO_VOLCANO_e = 47,
        DEMO_WARP_e = 48,
        DEMO_S_SURP_e = 49,
        DEMO_SMILE_e = 50,
        DEMO_UNK_051_e = 51,
        DEMO_AGB_e = 52,
        DEMO_LTURN_e = 53,
        DEMO_LET_OPEN_e = 54,
        DEMO_LET_READ_e = 55,
        DEMO_GRAB_PUT_e = 56,
        DEMO_RD_STOP_e = 57,
        DEMO_RD_CATCH_e = 58,
        DEMO_GET_DANCE_e = 59,
        DEMO_BO_FAIRY_e = 60,
        DEMO_BO_OPEN_e = 61,
        DEMO_S_WARP_e = 62,
        DEMO_OPEN_SBOX_e = 63,
        DEMO_SFALL_e = 64,
        DEMO_FOODSET_e = 65,
        DEMO_SWAIT_e = 66,
        DEMO_PWAIT_067_e = 67,
        DEMO_BOW_MINIGAME_e = 68,
        DEMO_SHIPSIT_e = 69,
        DEMO_UNK_070_e = 70,
        DEMO_SHIPOFF_e = 71,
        DEMO_UNK_072_e = 72,
        DEMO_FOODTHROW_e = 73,
        DEMO_UNK_074_e = 74,
        DEMO_NEW_ANM0_e = 0x200,
    };

    /* 0x00 */ be<u16> mDemoType;
    /* 0x02 */ be<s16> mDemoMoveAngle;
    /* 0x04 */ be<s16> mTimer;
    /* 0x06 */ be<s16> mParam2;
    /* 0x08 */ be<s32> mParam0;
    /* 0x0C */ be<s32> mParam1;
    /* 0x10 */ be<u32> mDemoMode;
    /* 0x14 */ be<f32> mStick;
    int getDemoType() { return mDemoType; }
    void setDemoType(u16 t) { mDemoType = t; }
    void resetDemoType() { setDemoType(TYPE_NONE_e); }
    u32 getDemoMode() { return mDemoMode; }
    void setDemoMode(u32 mode) { mDemoMode = mode; }
    s16 getTimer() { return mTimer; }
    void setTimer(s16 t) { mTimer = t; }
    s16 getMoveAngle() { return mDemoMoveAngle; }
    void setMoveAngle(s16 a) { mDemoMoveAngle = a; }
    int getParam0() { return mParam0; }
    void setParam0(int v) { mParam0 = v; }
    int getParam1() { return mParam1; }
    void setParam1(int v) { mParam1 = v; }
    void setParam2(int v) { mParam2 = v; }
    f32 getStick() { return mStick; }
    void setStick(f32 s) { mStick = s; }
};
WWHD_SIZE(daPy_demo_c, 0x18);

struct daPy_py_c : fopAc_ac_c {
    enum daPy_FLG0 {
        daPyFlg0_UNK1 = 0x00000001,
        daPyFlg0_UNK2 = 0x00000002,
        daPyFlg0_UNK4 = 0x00000004,
        daPyFlg0_UNK8 = 0x00000008,
        daPyFlg0_DEKU_SP_RETURN_FLG = 0x00000010,
        daPyFlg0_UNK20 = 0x00000020,
        daPyFlg0_CUT_AT_FLG = 0x00000040,
        daPyFlg0_UNK80 = 0x00000080,
        daPyFlg0_UNK100 = 0x00000100,
        daPyFlg0_SHIP_DROP = 0x00000200,
        daPyFlg0_UNK400 = 0x00000400,
        daPyFlg0_PUSH_PULL_KEEP = 0x00000800,
        daPyFlg0_HOVER_BOOTS = 0x00001000,
        daPyFlg0_UNK4000 = 0x00004000,
        daPyFlg0_UNK10000 = 0x00010000,
        daPyFlg0_UNK20000 = 0x00020000,
        daPyFlg0_NO_FALL_VOICE = 0x00040000,
        daPyFlg0_SCOPE_CANCEL = 0x00080000,
        daPyFlg0_PHOTO_BOX_CANCEL = 0x00080000,
        daPyFlg0_UNK100000 = 0x00100000,
        daPyFlg0_UNK200000 = 0x00200000,
        daPyFlg0_UNK400000 = 0x00400000,
        daPyFlg0_UNK800000 = 0x00800000,
        daPyFlg0_UNK1000000 = 0x01000000,
        daPyFlg0_EQUIP_HEAVY_BOOTS = 0x02000000,
        daPyFlg0_NO_DRAW = 0x08000000,
        daPyFlg0_UNK10000000 = 0x10000000,
        daPyFlg0_UNK20000000 = 0x20000000,
        daPyFlg0_HEAVY_STATE = 0x40000000,
        daPyFlg0_UNK80000000 = 0x80000000,
    };
    enum daPy_FLG1 {
        daPyFlg1_EQUIP_DRAGON_SHIELD = 0x00000001,
        daPyFlg1_NPC_CALL_COMMAND = 0x00000002,
        daPyFlg1_UNK4 = 0x00000004,
        daPyFlg1_CASUAL_CLOTHES = 0x00000008,
        daPyFlg1_FORCE_VOMIT_JUMP = 0x00000010,
        daPyFlg1_NPC_NOT_CHANGE = 0x00000040,
        daPyFlg1_UNK80 = 0x00000080,
        daPyFlg1_CONFUSE = 0x00000100,
        daPyFlg1_UNK200 = 0x00000200,
        daPyFlg1_UNK400 = 0x00000400,
        daPyFlg1_FREEZE_STATE = 0x00000800,
        daPyFlg1_SHIP_TACT = 0x00001000,
        daPyFlg1_USE_ARROW_EFFECT = 0x00002000,
        daPyFlg1_LETTER_READ_EYE_MOVE = 0x00004000,
        daPyFlg1_SOUP_POWER_UP = 0x00008000,
        daPyFlg1_FORCE_VOMIT_JUMP_SHORT = 0x00010000,
        daPyFlg1_FOREST_WATER_USE = 0x00020000,
        daPyFlg1_UNK40000 = 0x00040000,
        daPyFlg1_WATER_DROP = 0x00080000,
        daPyFlg1_UNK100000 = 0x00100000,
        daPyFlg1_UNK200000 = 0x00200000,
        daPyFlg1_UNK800000 = 0x00800000,
        daPyFlg1_UNK1000000 = 0x01000000,
        daPyFlg1_VINE_CATCH = 0x02000000,
        daPyFlg1_UNK4000000 = 0x04000000,
        daPyFlg1_UNK8000000 = 0x08000000,
        daPyFlg1_UNK10000000 = 0x10000000,
        daPyFlg1_LAST_COMBO_WAIT = 0x20000000,
        daPyFlg1_UNK40000000 = 0x40000000,
    };
    enum daPy_RFLG0 {
        daPyRFlg0_UNK1 = 0x00000001,
        daPyRFlg0_UNK2 = 0x00000002,
        daPyRFlg0_ROPE_GRAB_RIGHT_HAND = 0x00000004,
        daPyRFlg0_UNK8 = 0x00000008,
        daPyRFlg0_UNK10 = 0x00000010,
        daPyRFlg0_GRAB_UP_END = 0x00000020,
        daPyRFlg0_AUTO_JUMP_LAND = 0x00000040,
        daPyRFlg0_UNK80 = 0x00000080,
        daPyRFlg0_UNK100 = 0x00000100,
        daPyRFlg0_ROPE_JUMP_LAND = 0x00000200,
        daPyRFlg0_RIGHT_FOOT_ON_GROUND = 0x00000400,
        daPyRFlg0_LEFT_FOOT_ON_GROUND = 0x00000800,
        daPyRFlg0_CRAWL_AUTO_MOVE = 0x00001000,
        daPyRFlg0_FRONT_ROLL_CRASH = 0x00002000,
        daPyRFlg0_UNK4000 = 0x00004000,
        daPyRFlg0_GRAB_UP_START = 0x00008000,
        daPyRFlg0_ATTENTION_LOCK = 0x00010000,
        daPyRFlg0_HAMMER_QUAKE = 0x00020000,
        daPyRFlg0_UNK40000 = 0x00040000,
        daPyRFlg0_UNK80000 = 0x00080000,
        daPyRFlg0_POISON_CURSE = 0x00100000,
        daPyRFlg0_LIGHT_REFLECT = 0x00200000,
        daPyRFlg0_GRAB_PUT_START = 0x00400000,
        daPyRFlg0_ORIGINAL_HAT_ANIM = 0x00800000,
        daPyRFlg0_TACT_INPUT = 0x01000000,
        daPyRFlg0_FAIRY_USE = 0x02000000,
        daPyRFlg0_SUBJECT_ACCEPT = 0x04000000,
        daPyRFlg0_NOT_ATTACKING = 0x08000000,
        daPyRFlg0_UNK10000000 = 0x10000000,
        daPyRFlg0_ARROW_SHOOT = 0x20000000,
        daPyRFlg0_ROPE_FORCE_END = 0x40000000,
    };

    /* virtual (HD vtable +0x114; daPy_lk_c: 02443A28) [v nz] */
    void setPlayerPosAndAngle(cXyz* pos, s16 angle) { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x114), this, pos, angle); }
    /* 0x3AC */ u8 mCutType;                 /* GameCube 0x290 [v bk, am2, mo2] */
    /* 0x3AD */ u8 mCutCount;
    /* 0x3AE */ u8 pad3AE[2];
    /* 0x3B0 */ be<s16> mDamageWaitTimer;
    /* 0x3B2 */ be<s16> mQuakeTimer;
    /* 0x3B4 */ be<s32> mFace;
    /* 0x3B8 */ be<u32> mNoResetFlg0;
    /* 0x3BC */ be<u32> mNoResetFlg1;
    /* 0x3C0 */ be<u32> mResetFlg0;
    /* 0x3C4 */ be<f32> mMaxNormalSpeed;
    /* 0x3C8 */ be<f32> mHeight;
    /* 0x3CC */ be<f32> field_0x2b0;
    /* 0x3D0 */ csXyz mBodyAngle;
    /* 0x3D6 */ u8 field_0x2BA[2];
    /* 0x3D8 */ cXyz mHeadTopPos;
    /* 0x3E4 */ cXyz mSwordTopPos;
    /* 0x3F0 */ cXyz mLeftHandPos;
    /* 0x3FC */ cXyz mRightHandPos;
    /* 0x408 */ cXyz mRopePos;
    /* 0x414 */ cXyz mWindVelocity;          /* GameCube field_0x2f8 */
    /* 0x420 */ daPy_demo_c mDemo;
    /* 0x438 */ u8 _438[4];

    u8 getCutType() { return mCutType; }
    u8 getCutCount() { return mCutCount; }
    s16 getDamageWaitTimer() { return mDamageWaitTimer; }
    f32 getHeight() { return mHeight; }
    s16 getBodyAngleX() { return mBodyAngle.x; }
    s16 getBodyAngleY() { return mBodyAngle.y; }
    void onNoResetFlg0(u32 flag) { mNoResetFlg0 |= flag; }
    void offNoResetFlg0(u32 flag) { mNoResetFlg0 &= ~flag; }
    u32 checkNoResetFlg0(u32 flag) { return mNoResetFlg0 & flag; }
    void onNoResetFlg1(u32 flag) { mNoResetFlg1 |= flag; }
    void offNoResetFlg1(u32 flag) { mNoResetFlg1 &= ~flag; }
    u32 checkNoResetFlg1(u32 flag) { return mNoResetFlg1 & flag; }
    void onResetFlg0(u32 flag) { mResetFlg0 |= flag; }
    void offResetFlg0(u32 flag) { mResetFlg0 &= ~flag; }
    u32 checkResetFlg0(u32 flag) { return mResetFlg0 & flag; }
    void setFace(int face) { mFace = face; }
    BOOL checkBowMiniGame() { return mDemo.getDemoMode() == daPy_demo_c::DEMO_BOW_MINIGAME_e; }
};
WWHD_SIZE(daPy_py_c, 0x43C);

WWHD_OFFSET(daPy_mtxFollowEcallBack_c, mpEmitter, 0x4);
WWHD_OFFSET(daPy_mtxFollowEcallBack_c, mpMatrix, 0x8);
WWHD_OFFSET(daPy_py_c, mCutType, 0x3AC);
WWHD_OFFSET(daPy_py_c, mQuakeTimer, 0x3B2);
WWHD_OFFSET(daPy_py_c, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(daPy_py_c, mNoResetFlg1, 0x3BC);
WWHD_OFFSET(daPy_py_c, mResetFlg0, 0x3C0);
WWHD_OFFSET(daPy_py_c, mBodyAngle, 0x3D0);
WWHD_OFFSET(daPy_py_c, mWindVelocity, 0x414);
WWHD_OFFSET(daPy_py_c, mDemo, 0x420);
