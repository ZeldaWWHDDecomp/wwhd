/* daNpc_Pm1_c (Maggie, poor, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (no members in its header). The WWHD code is the same as
 * d_a_npc_km1's apart from constants, so the layout is Km1's (d_a_npc_km1.h), confirmed by every
 * function of the unit: size 0x944 (profile at 0x101C53DC, process name 0x162). Field names follow
 * the GameCube Km1 header. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Pm1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    bool createInit();
    void setMtx();
    bool anmResID(int, be<s32>*, be<s32>*);
    void BtpNum2ResID(int, be<s32>*);
    u32 setAnm_tex(s8);
    bool init_btp(u32, int);
    bool initTexPatternAnm(u32);
    void playTexPatternAnm();
    s32 setAnm_anm(anm_prm_c*);
    void setAnm();
    void chngAnmAtr(u32);
    void setAnm_ATR(int);
    void anmAtr(u16);
    void setStt(s8);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    void lookBack();
    u8 chkAttention();
    void setAttention();
    bool decideType(int);
    void event_actionInit(int);
    bool event_action();
    void privateCut();
    void endEvent();
    void event_proc();
    bool set_action(ProcFunc_l*, void*);
    BOOL wait01();
    BOOL talk01();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_backbone_jnt_num;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId / shadow model before it */
    /* 0x7EC */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> field_0x6F1;
    /* 0x862 */ be<s16> field_0x6F2;
    /* 0x864 */ ProcFunc_l mAction;                         /* field_0x6F4 */
    /* 0x86C */ dNpc_EventCut_c mEventCut;
    /* 0x8D8 */ csXyz field_0x76C;
    /* 0x8DE */ u8 _8DE[2];
    /* 0x8E0 */ cXyz field_0x774;
    /* 0x8EC */ cXyz mAttPos;
    /* 0x8F8 */ cXyz mEyePos;
    /* 0x904 */ cXyz field_0x798;
    /* 0x910 */ be<f32> field_0x7A4;
    /* 0x914 */ u8 _914[4];
    /* 0x918 */ be<f32> field_0x7AC;
    /* 0x91C */ be<s16> field_0x7B0;
    /* 0x91E */ be<s16> field_0x7B2;
    /* 0x920 */ be<s8> field_0x7B4;
    /* 0x921 */ be<s8> field_0x7B5;
    /* 0x922 */ be<u8> field_0x7B6;
    /* 0x923 */ be<u8> field_0x7B7;
    /* 0x924 */ be<u8> field_0x7B8;
    /* 0x925 */ u8 _925[3];
    /* 0x928 */ be<s32> field_0x7BC;
    /* 0x92C */ be<s32> field_0x7C0;
    /* 0x930 */ be<u8> field_0x7C4;
    /* 0x931 */ be<u8> field_0x7C5;
    /* 0x932 */ be<u8> mHeadOnlyFollow;
    /* 0x933 */ be<u8> field_0x7C7;
    /* 0x934 */ be<u8> field_0x7C8;
    /* 0x935 */ be<s8> field_0x7C9;
    /* 0x936 */ be<s8> field_0x7CA;
    /* 0x937 */ be<u8> field_0x7CB;
    /* 0x938 */ be<u8> field_0x7CC;
    /* 0x939 */ be<s8> field_0x7CD;
    /* 0x93A */ be<s8> field_0x7CE;
    /* 0x93B */ be<s8> field_0x7CF;
    /* 0x93C */ be<s8> field_0x7D0;
    /* 0x93D */ be<s8> field_0x7D1;
    /* 0x93E */ be<s8> field_0x7D2;
    /* 0x93F */ be<s8> field_0x7D3;
    /* 0x940 */ be<s8> field_0x7D4;
    /* 0x941 */ be<s8> field_0x7D5;
    /* 0x942 */ be<s8> field_0x7D6;
    /* 0x943 */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Pm1_c, m_head_tex_pattern, 0x7E8);
WWHD_OFFSET(daNpc_Pm1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Pm1_c, mEventCut, 0x86C);
WWHD_OFFSET(daNpc_Pm1_c, mEyePos, 0x8F8);
WWHD_OFFSET(daNpc_Pm1_c, field_0x7BC, 0x928);
WWHD_OFFSET(daNpc_Pm1_c, field_0x7CD, 0x939);
WWHD_SIZE(daNpc_Pm1_c, 0x944);
