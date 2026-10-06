/* daNpc_Ym1_c (Mesa & Abe, Outset), WWHD layout. 
 *
 * GameCube -> WWHD: members +0x118 up to mArcName (fopNpc_npc_c is 0x7DC: GameCube 0x6C4 +
 * 0x11C - 4); mDoExt_btpAnm grew from 0x14 to 0x74 and the pointer to member mCurrProcFunc is 8
 * bytes, so from mCyl (GameCube 0x704) on everything is +0x170. Size 0xA24 (profiles NPC_YM1/
 * NPC_YM2 at 0x101C6D38/0x101C6D68; GameCube 0x8B4). Fields whose role is not known are named
 * by WWHD offset. The local bindings are shared with d_a_npc_ko1 (d_a_npc_ko1.h). */
#pragma once
#include "d/actor/d_a_npc_ko1.h" /* local bindings, ProcFunc_l */

struct daNpc_Ym1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* Methods, one section per part file. Each part owns its section: it may fix the
     * declarations there (return types, parameter types) and add new ones (unnamed functions).
     * Return types marked unchecked come from mkbind.py hints. */
    /* ---- part A (d_a_npc_ym1.cpp) ---- */
    void setKariFlg(); /* 022F68AC */
    void _nodeCB_Head(J3DNode*, J3DModel*); /* 022F68C4 */
    void _nodeCB_BackBone(J3DNode*, J3DModel*); /* 022F6A7C */
    bool bodyCreateHeap(); /* 022F6BE0 */
    s32 btpResID(s32); /* 022F6EF4 */
    bool init_texPttrnAnm(s8, u32); /* 022F6F38 */
    bool headCreateHeap(); /* 022F7024 */
    bool itemCreateHeap(); /* 022F7114 */
    bool CreateHeap(); /* 022F71E0 */
    bool decideType(int); /* 022F72B0 */
    BOOL set_action(ProcFunc_l*, void*); /* 022F7388 */
    bool init_YM1_0(); /* 022F74B4 */
    bool init_YM1_1(); /* 022F75E4 */
    bool init_YM2_0(); /* 022F7668 */
    bool init_YM2_1(); /* 022F7710 */
    bool init_YM2_2(); /* 022F77B8 */
    bool init_YM2_3(); /* 022F7848 */
    bool init_YMx_error(); /* 022F78D8 */
    void play_texPttrnAnm(); /* 022F7918 */
    void play_animation(); /* 022F79D0 (the r3 left is McaMorf::play's result: not a return value) */
    u8 chk_nbt_attn(); /* 022F7A3C */
    void setAttention(u32); /* 022F7A60 */
    void setMtx(u32); /* 022F7B28 */
    /* ---- end of part A ---- */
    /* ---- part A2 (d_a_npc_ym1_a2.cpp) ---- */
    bool createInit(); /* 022F7D68 */
    cPhs_State _create(); /* 022F8108 */
    BOOL _delete(); /* 022F82AC */
    void checkOrder(); /* 022F8304 */
    u8 demo(); /* 022F8344 */
    s32 isEventEntry(); /* 022F8504 */
    void endEvent(); /* 022F8544 */
    void privateCut(int); /* 022F8588 */
    void event_proc(int); /* 022F8620 */
    void lookBack(); /* 022F8670 */
    void eventOrder(); /* 022F8890 */
    void set_cutGrass(); /* 022F88C8 */
    void set_collision_sp(); /* 022F89CC */
    BOOL _execute(); /* 022F8C24 */
    /* ---- end of part A2 ---- */
    /* ---- part B (d_a_npc_ym1_b.cpp) ---- */
    BOOL _draw(); /* 022F8F30 */
    s32 bckResID(int); /* 022F915C */
    void setAnm_anm(anm_prm_c*); /* 022F9170 */
    void setAnm_NUM(int, int); /* 022F9230 */
    void setAnm(); /* 022F92A0 */
    void setAnm_ATR(); /* 022F9310 */
    void chngAnmAtr(u8); /* 022F9378 */
    void anmAtr(u16); /* 022F9394 */
    u16 next_msgStatus(be<u32>*); /* 022F9464 */
    u32 getMsg_YM1_0(); /* 022F9758 */
    u32 getMsg_YM1_1(); /* 022F980C */
    bool chk_BlackPig(); /* 022F9858 */
    u32 getMsg_YM2_0(); /* 022F9890 */
    u32 getMsg_YM2_1(); /* 022F9914 */
    u32 getMsg_YM2_2(); /* 022F9960 */
    u32 getMsg_YM2_3(); /* 022F9A44 */
    u32 getMsg(); /* 022F9A48 */
    bool chk_talk(); /* 022F9ACC */
    u8 chk_parts_notMov(); /* 022F9B64 */
    u8 chkAttention(); /* 022F9BA4 */
    void setStt(s8); /* 022F9C2C */
    bool chk_areaIN(f32, cXyz*); /* 022F9D04 (cXyz by value: pointer to a copy) */
    BOOL kari_1(); /* 022F9E00 */
    BOOL wait_1(); /* 022F9EF4 */
    BOOL wait_2(); /* 022FA0A0 */
    u8 talk_1(); /* 022FA288 */
    BOOL turn_1(); /* 022FA4E4 */
    BOOL NBTwai(); /* 022FA5B8 */
    BOOL SITwai(); /* 022FA70C */
    BOOL wait_action1(void*); /* 022FA828 */
    BOOL wait_action2(void*); /* 022FA940 */
    BOOL wait_action3(void*); /* 022FAA28 */
    BOOL wait_action4(void*); /* 022FAB88 */
    BOOL demo_action1(void*); /* 022FAC34 */
    /* ---- end of part B ---- */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> mHeadJointIdx;
    /* 0x7E5 */ be<s8> mBboneJointIdx;
    /* 0x7E6 */ be<s8> mHandLJointIndex;
    /* 0x7E7 */ be<s8> mHandRJointIndex;
    /* 0x7E8 */ gptr<J3DModel> m7E8;              /* GameCube m6D0 */
    /* 0x7EC */ char mArcName[4];
    /* 0x7F0 */ gptr<J3DModel> mpHeadModel;
    /* 0x7F4 */ u8 mBtpAnm[0x74];                 /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> mBlinkFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ ProcFunc_l mCurrProcFunc;
    /* 0x874 */ dCcD_Cyl mCyl;                    /* hides fopNpc_npc_c::mCyl (GameCube 0x704) */
    /* 0x9A4 */ be<u32> m9A4;
    /* 0x9A8 */ be<u32> m9A8;
    /* 0x9AC */ be<u32> m9AC;
    /* 0x9B0 */ be<s16> m9B0;
    /* 0x9B2 */ be<s16> mRotYTarget;              /* GameCube 0x842 */
    /* 0x9B4 */ be<s16> m9B4;
    /* 0x9B6 */ csXyz m9B6;                       /* GameCube m846 */
    /* 0x9BC */ cXyz m9BC;
    /* 0x9C8 */ cXyz m9C8;
    /* 0x9D4 */ cXyz m9D4;
    /* 0x9E0 */ be<f32> m9E0;
    /* 0x9E4 */ be<f32> m9E4;
    /* 0x9E8 */ be<f32> m9E8;
    /* 0x9EC */ be<f32> m9EC;
    /* 0x9F0 */ csXyz m9F0;
    /* 0x9F6 */ u8 _9F6[2];
    /* 0x9F8 */ be<u32> m9F8;
    /* 0x9FC */ be<s16> m9FC;
    /* 0x9FE */ be<s16> m9FE;
    /* 0xA00 */ be<s16> mKariTimer;               /* GameCube 0x890 */
    /* 0xA02 */ be<s16> mA02;
    /* 0xA04 */ be<s16> mA04;
    /* 0xA06 */ be<s16> mA06;
    /* 0xA08 */ be<u8> mA08;
    /* 0xA09 */ be<u8> mA09;
    /* 0xA0A */ be<u8> mA0A;
    /* 0xA0B */ be<u8> mKariFlag;                 /* GameCube 0x89B */
    /* 0xA0C */ be<u8> mA0C;
    /* 0xA0D */ be<u8> mA0D;
    /* 0xA0E */ be<u8> mA0E;
    /* 0xA0F */ be<u8> mA0F;
    /* 0xA10 */ be<u8> mA10;
    /* 0xA11 */ be<u8> mA11;
    /* 0xA12 */ be<s8> mA12;
    /* 0xA13 */ be<s8> mA13;
    /* 0xA14 */ be<s8> mA14;
    /* 0xA15 */ be<s8> mA15;
    /* 0xA16 */ be<s8> mA16;
    /* 0xA17 */ be<s8> mA17;
    /* 0xA18 */ be<s8> mA18;
    /* 0xA19 */ be<s8> mA19;
    /* 0xA1A */ be<s8> mA1A;
    /* 0xA1B */ be<s8> mA1B;
    /* 0xA1C */ be<s8> mA1C;
    /* 0xA1D */ be<s8> mA1D;
    /* 0xA1E */ be<s8> mA1E;
    /* 0xA1F */ be<s8> mA1F;
    /* 0xA20 */ be<s8> mSubType;                  /* GameCube 0x8B0 */
    /* 0xA21 */ be<s8> mStaff;                    /* GameCube 0x8B1 */
    /* 0xA22 */ be<s8> mA22;
    /* 0xA23 */ be<s8> mA23;
};
WWHD_OFFSET(daNpc_Ym1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ym1_c, mArcName, 0x7EC);
WWHD_OFFSET(daNpc_Ym1_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Ym1_c, mCurrProcFunc, 0x86C);
WWHD_OFFSET(daNpc_Ym1_c, mCyl, 0x874);
WWHD_OFFSET(daNpc_Ym1_c, mKariTimer, 0xA00);
WWHD_OFFSET(daNpc_Ym1_c, mSubType, 0xA20);
WWHD_SIZE(daNpc_Ym1_c, 0xA24);
