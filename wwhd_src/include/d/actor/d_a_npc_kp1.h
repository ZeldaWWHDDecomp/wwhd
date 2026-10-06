/* daNpc_Kp1_c (Maggie, rich, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (no members in its header). The layout is d_a_npc_km1's
 * (Km1 code is largely shared) with three extra members after the joint numbers (a third joint
 * "handL" and two hand-held models), so everything from m_head_tex_pattern on is Km1 + 0xC,
 * except 0x92F..0x933, where Kp1 has four message/event flags before the first-frame flag.
 * Size 0x950 (profile at 0x101BFF0C, process name 0x163). Field names follow the GameCube Km1
 * header where the use is the same. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Kp1_c : fopNpc_npc_l {
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
    void setAnm_NUM(int, int);
    void ctrlAnmAtr();
    BOOL chk_talk();
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
    /* 0x7E6 */ be<s8> m_hand_jnt_num;                   /* HD (Kp1): a third joint (CreateHeap) */
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel[2];            /* Kp1: two extra models (CreateHeap ids 6, 7) */
    /* 0x7F0 */ be<u8> m7F0;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId / shadow model before it */
    /* 0x7F8 */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x86C */ be<u8> mBtpFrame;
    /* 0x86D */ be<u8> field_0x6F1;
    /* 0x86E */ be<s16> field_0x6F2;
    /* 0x870 */ ProcFunc_l mAction;                         /* field_0x6F4 */
    /* 0x878 */ dNpc_EventCut_c mEventCut;
    /* 0x8E4 */ csXyz field_0x76C;
    /* 0x8EA */ u8 _8EA[2];
    /* 0x8EC */ cXyz field_0x774;
    /* 0x8F8 */ cXyz mAttPos;
    /* 0x904 */ cXyz mEyePos;
    /* 0x910 */ cXyz field_0x798;
    /* 0x91C */ be<f32> field_0x7A4;
    /* 0x920 */ u8 _920[4];
    /* 0x924 */ be<f32> field_0x7AC;
    /* 0x928 */ be<s16> field_0x7B0;
    /* 0x92A */ be<s16> field_0x7B2;
    /* 0x92C */ be<s8> field_0x7B4;
    /* 0x92D */ be<s8> field_0x7B5;
    /* 0x92E */ be<u8> field_0x7B6;
    /* 0x92F */ be<u8> m92F;                              /* Kp1: getMsg flag (event 1 ended) */
    /* 0x930 */ be<u8> m930;                              /* Kp1: getMsg flag (event 2 ended) */
    /* 0x931 */ be<u8> m931;                              /* Kp1: next_msgStatus/getMsg flag */
    /* 0x932 */ be<u8> m932;                              /* Kp1: present to cancel (talk01) */
    /* 0x933 */ be<u8> field_0x7B8;                       /* _execute: first-frame init done */
    /* 0x934 */ be<s32> field_0x7BC;
    /* 0x938 */ be<s32> field_0x7C0;
    /* 0x93C */ be<u8> field_0x7C4;
    /* 0x93D */ be<u8> field_0x7C5;
    /* 0x93E */ be<u8> mHeadOnlyFollow;
    /* 0x93F */ be<u8> field_0x7C7;
    /* 0x940 */ be<u8> field_0x7C8;
    /* 0x941 */ be<s8> field_0x7C9;
    /* 0x942 */ be<s8> field_0x7CA;
    /* 0x943 */ be<u8> field_0x7CB;
    /* 0x944 */ be<u8> field_0x7CC;
    /* 0x945 */ be<s8> field_0x7CD;
    /* 0x946 */ be<s8> field_0x7CE;
    /* 0x947 */ be<s8> field_0x7CF;
    /* 0x948 */ be<s8> field_0x7D0;
    /* 0x949 */ be<s8> field_0x7D1;
    /* 0x94A */ be<s8> field_0x7D2;
    /* 0x94B */ be<s8> field_0x7D3;
    /* 0x94C */ be<s8> field_0x7D4;
    /* 0x94D */ be<s8> field_0x7D5;
    /* 0x94E */ be<s8> field_0x7D6;
    /* 0x94F */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Kp1_c, m_head_tex_pattern, 0x7F4);
WWHD_OFFSET(daNpc_Kp1_c, mAction, 0x870);
WWHD_OFFSET(daNpc_Kp1_c, mEventCut, 0x878);
WWHD_OFFSET(daNpc_Kp1_c, mEyePos, 0x904);
WWHD_OFFSET(daNpc_Kp1_c, field_0x7BC, 0x934);
WWHD_OFFSET(daNpc_Kp1_c, field_0x7CD, 0x945);
WWHD_SIZE(daNpc_Kp1_c, 0x950);
