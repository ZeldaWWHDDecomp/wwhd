/* daNpc_Kk1_c (Mila, poor, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" and its header has almost no members: the layout is measured
 * from the WWHD code. Size 0xAD0 (profile at 0x101BF654, process name 0x161; GameCube 0x824).
 * Base fopNpc_npc_c (0x7DC, d/d_npc.h). Fields whose role is not known are named by their HD
 * offset (mXXX); signedness: cast at use ((s8)m926) rather than retyping.
 *
 * Includes d_a_npc_ko1.h for its local bindings (SAFESTRING_VTBL must be defined first). */
#pragma once
#include "d/actor/d_a_npc_ko1.h"

/* mDoExt_bckAnm (HD 0x8C, inline constructor: J3DFrameCtrl, vtables 1016E54C/1016D820) */
struct mDoExt_bckAnm_kk1 {
    u8 _[0x8C];
};

/* daNpc_Kk1_HIO_c (HD 0x60: vtable +0, mNo +4, field_0x8 +8, parameter table +0xC (0x54 bytes,
 * from .data 0x101BF5EC); the static l_HIO is at 0x10467698) */
struct daNpc_Kk1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ u8 mPrm[0x14];
    /* 0x20 */ be<f32> mAttentionYOffset;  /* setAttention */
    /* 0x24 */ u8 mPrm24[0x60 - 0x24];
};
WWHD_SIZE(daNpc_Kk1_HIO_c, 0x60);
static inline daNpc_Kk1_HIO_c& l_HIO_kk1() { return *gabi::at<daNpc_Kk1_HIO_c>(0x10467698); }

struct daNpc_Kk1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* methods (drafts from mkbind: each part's owner fixes the signatures of its functions) */
    void _nodeCB_Head(J3DNode* a0, J3DModel* a1); /* 0226AAF8 */
    void _nodeCB_BackBone(J3DNode* a0, J3DModel* a1); /* 0226ACB8 */
    BOOL setBtp(s8 i_num, u32 i_modify); /* 0226AE30 */
    s32 btpResID(int i_num); /* 0226AE24 (unnamed by the matcher) */
    BOOL init_texPttrnAnm(s8 i_num, u32 i_modify); /* 0226AF1C */
    bool bodyCreateHeap(); /* 0226AF20 */
    bool effcCreateHeap(); /* 0226B1E0 */
    bool CreateHeap(); /* 0226B3E8 */
    bool decideType(int i_type); /* 0226B550 */
    void set_pthPoint(u8 i_idx); /* 0226B690 (unnamed by the matcher) */
    BOOL set_action(ProcFunc_l* a0, void* a1); /* 0226B704 */
    bool init_KK1_0(); /* 0226B830 */
    void play_btp_anm(); /* 0226B8E4 */
    void setBikon(cXyz* a0 /* by value: a copy */); /* 0226B984 */
    void play_eff_anm(); /* 0226BA48 */
    void play_animation(); /* 0226BB5C */
    void flwAse(); /* 0226BCD4 */
    void setAttention(u32 i_force); /* 0226BD78 */
    void setMtx(u32 i_force); /* 0226BDCC */
    bool createInit(); /* 0226BFD0 */
    cPhs_State _create(); /* 0226C28C */
    void delAse(); /* 0226C438 */
    BOOL _delete(); /* 0226C464 */
    bool partner_search_sub(u32 /* fn */ a0); /* 0226C4C4 */
    void partner_search(); /* 0226C570 */
    void setAse(); /* 0226C5F0 */
    void setAnm_anm(anm_prm_c* a0); /* 0226C660 */
    void setAnm_NUM(s32 a0, s32 a1); /* 0226C730 */
    bool checkCommandTalk(); /* 0226C7A0 */
    void checkOrder(); /* 0226C7DC */
    u8 demo(); /* 0226C8D8 */
    s32 isEventEntry(); /* 0226CA94 */
    s32 bckResID(int i_num); /* 0226C5DC (unnamed by the matcher) */
    fopAc_ac_c* searchByID(fpc_ProcID i_id, be<s32>* o_res); /* 0226CC2C (matcher: cLib_getRndValue<i>) */
    void setAnm(); /* 0226CAD4 */
    void setStt(s8 a0); /* 0226CB44 */
    void endEvent(); /* 0226CBE8 */
    bool cut_init_RUN_START(s32 a0); /* 0226CC80 */
    void cut_init_RUN(s32 a0); /* 0226CD04 */
    void cut_init_CATCH_START(s32 a0); /* 0226CD94 */
    void cut_init_CATCH_END(s32 a0); /* 0226CDF0 */
    void cut_init_TRN(s32 a0); /* 0226CE6C */
    void cut_init_BYE_START(s32 a0); /* 0226D0A4 */
    void cut_init_BYE(s32 a0); /* 0226D0E8 */
    void cut_init_BYE_END(s32 a0); /* 0226D1F0 */
    void cut_init_OTOBOKE(s32 a0); /* 0226D2B8 */
    void cut_init_PLYER_MOV(s32 a0); /* 0226D34C */
    void cut_init_RUNAWAY_START(s32 a0); /* 0226D454 */
    void cut_init_RUNAWAY_END(s32 a0); /* 0226D60C */
    void cut_init_BYE_CONTINUE(s32 a0); /* 0226D664 */
    bool cut_move_RUN_START(); /* 0226D6CC */
    bool event_move(bool a0); /* 0226D83C */
    s32 cut_move_RUN(); /* 0226DB64 */
    bool cut_move_CATCH_START(); /* 0226DBB0 */
    bool cut_move_TRN(); /* 0226DBD4 */
    bool cut_move_BYE(); /* 0226DC70 */
    bool cut_move_OTOBOKE(); /* 0226DD7C */
    bool cut_move_RUNAWAY_START(); /* 0226DDD8 */
    bool cut_move_BYE_CONTINUE(); /* 0226DE88 */
    void privateCut(s32 a0); /* 0226DEF0 */
    void event_proc(s32 a0); /* 0226E21C */
    void kyoroPos(cXyz* o_pos, s32 i_idx); /* 0226E498: returns a cXyz (hidden result pointer) */
    bool kyorokyoro(); /* 0226E560 */
    void lookBack(); /* 0226E604 */
    void eventOrder(); /* 0226E92C */
    BOOL _execute(); /* 0226E99C */
    BOOL _draw(); /* 0226ECA8 */
    void setAnm_ATR(); /* 0226EF4C */
    void chngAnmAtr(u8 a0); /* 0226EFB4 */
    void ctrlAnmAtr(); /* 0226F088 */
    void anmAtr(u16 a0); /* 0226F170 */
    u16 next_msgStatus(be<u32>* a0); /* 0226F230 */
    u32 getMsg_KK1_0(); /* 0226F538 */
    u32 getMsg(); /* 0226F61C */
    bool chk_talk(); /* 0226F654 */
    u8 chk_parts_notMov(); /* 0226F6EC */
    BOOL chkAttention(); /* 0226F72C */
    void createTama(f32 a0); /* 0226F7B4 */
    bool chk_areaIN(f32 a0, cXyz* a1 /* by value: a copy */); /* 0226F908 */
    bool startEvent_check(); /* 0226FABC */
    BOOL chkHitPlayer(); /* 0226FB70 */
    u8 chk_attn(); /* 0226FC1C */
    s32 wait_1(); /* 0226FE78 */
    s32 walk_1(); /* 022700E0 */
    s32 wait_2(); /* 022703B4 */
    void init_CMT_WAI(); /* 0227051C */
    void move_CMT_WAI(); /* 02270570 */
    void init_CMT_TRN(); /* 0227066C */
    void move_CMT_TRN(); /* 022706E4 */
    void init_CMT_PCK(); /* 022708B8 */
    void move_CMT_PCK(); /* 02270918 */
    s32 cmmt_1(); /* 02270AE0 */
    s32 wait_3(); /* 02270C7C */
    s32 wait_4(); /* 02270DA8 */
    BOOL talk_1(); /* 02270F28 */
    BOOL wait_action1(void* a0); /* 02271084 */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_backbone_jnt_num;
    /* 0x7E6 */ char mArcName[6];                /* "Kk" (decideType) */
    /* 0x7EC */ u8 mBtpAnm[0x74];                /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> m861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mAction;
    /* 0x86C */ be<u32> m86C;
    /* 0x870 */ be<u32> m870;
    /* 0x874 */ dNpc_PathRun_l mPathRun;
    /* 0x87C */ cXyz m87C;
    /* 0x888 */ csXyz m888;
    /* 0x88E */ csXyz m88E;
    /* 0x894 */ u8 _894[0x89C - 0x894];
    /* 0x89C */ cXyz mAttPos;                    /* attention offset position (nodeCB_Head) */
    /* 0x8A8 */ cXyz m8A8;
    /* 0x8B4 */ u8 _8B4[0x8C0 - 0x8B4];
    /* 0x8C0 */ cXyz mEyePos;                    /* (nodeCB_Head) */
    /* 0x8CC */ u8 _8CC[0x8D8 - 0x8CC];
    /* 0x8D8 */ be<f32> m8D8;
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ csXyz m8E0;
    /* 0x8E6 */ u8 _8E6[2];
    /* 0x8E8 */ be<u32> m8E8;
    /* 0x8EC */ be<s16> m8EC;
    /* 0x8EE */ u8 _8EE[2];
    /* 0x8F0 */ be<s16> m8F0;
    /* 0x8F2 */ u8 _8F2[0x8FC - 0x8F2];
    /* 0x8FC */ be<s16> m8FC;
    /* 0x8FE */ u8 _8FE[4];
    /* 0x902 */ be<s16> m902;
    /* 0x904 */ be<s16> m904;
    /* 0x906 */ be<s16> m906;
    /* 0x908 */ be<s16> m908;
    /* 0x90A */ be<s16> m90A;
    /* 0x90C */ u8 _90C[2];
    /* 0x90E */ be<s16> m90E;
    /* 0x910 */ u8 _910[2];
    /* 0x912 */ be<s16> m912;
    /* 0x914 */ be<s16> m914;
    /* 0x916 */ u8 _916[4];
    /* 0x91A */ be<s16> m91A;
    /* 0x91C */ be<s16> m91C;
    /* 0x91E */ be<s16> m91E;
    /* 0x920 */ be<s16> m920;
    /* 0x922 */ be<u8> m922;
    /* 0x923 */ be<u8> m923;
    /* 0x924 */ be<u8> m924;
    /* 0x925 */ be<u8> m925;
    /* 0x926 */ be<u8> m926;
    /* 0x927 */ be<u8> m927;
    /* 0x928 */ be<u8> m928;
    /* 0x929 */ u8 _929;
    /* 0x92A */ be<u8> m92A;
    /* 0x92B */ be<u8> m92B;
    /* 0x92C */ be<u8> m92C;
    /* 0x92D */ be<u8> m92D;
    /* 0x92E */ be<u8> m92E;
    /* 0x92F */ be<u8> m92F;
    /* 0x930 */ be<u8> m930;
    /* 0x931 */ be<u8> m931;
    /* 0x932 */ be<u8> m932;
    /* 0x933 */ be<u8> m933;
    /* 0x934 */ be<u8> m934;
    /* 0x935 */ be<u8> m935;
    /* 0x936 */ be<u8> m936;
    /* 0x937 */ u8 _937;
    /* 0x938 */ u8 mBrkAnm[0x74];                /* mDoExt_brkAnm (HD 0x74, constructor 025E7480) */
    /* 0x9AC */ mDoExt_btkAnm mBtkAnm;           /* (HD 0x74, constructor 025E7C6C) */
    /* 0xA20 */ mDoExt_bckAnm_kk1 mBckAnm;       /* (HD 0x8C, inline constructor) */
    /* 0xAAC */ csXyz mAAC;
    /* 0xAB2 */ u8 _AB2[2];
    /* 0xAB4 */ be<u32> mAB4;
    /* 0xAB8 */ u8 _AB8[4];
    /* 0xABC */ be<u32> mABC;                    /* sweat (Ase) emitter */
    /* 0xAC0 */ be<u8> mAC0;
    /* 0xAC1 */ be<u8> mAC1;
    /* 0xAC2 */ be<u8> mAC2;
    /* 0xAC3 */ be<u8> mAC3;
    /* 0xAC4 */ be<u8> mAC4;
    /* 0xAC5 */ be<u8> mAC5;
    /* 0xAC6 */ be<u8> mAC6;
    /* 0xAC7 */ be<u8> mAC7;
    /* 0xAC8 */ be<u8> mAC8;
    /* 0xAC9 */ be<u8> mAC9;
    /* 0xACA */ be<u8> mACA;
    /* 0xACB */ be<s8> mType;                    /* decideType */
    /* 0xACC */ be<s8> mACC;
    /* 0xACD */ be<u8> mACD;
    /* 0xACE */ be<u8> mACE;
    /* 0xACF */ u8 _ACF;
};
WWHD_OFFSET(daNpc_Kk1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Kk1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Kk1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Kk1_c, mPathRun, 0x874);
WWHD_OFFSET(daNpc_Kk1_c, mAttPos, 0x89C);
WWHD_OFFSET(daNpc_Kk1_c, mEyePos, 0x8C0);
WWHD_OFFSET(daNpc_Kk1_c, m908, 0x908);
WWHD_OFFSET(daNpc_Kk1_c, m922, 0x922);
WWHD_OFFSET(daNpc_Kk1_c, mBrkAnm, 0x938);
WWHD_OFFSET(daNpc_Kk1_c, mBtkAnm, 0x9AC);
WWHD_OFFSET(daNpc_Kk1_c, mBckAnm, 0xA20);
WWHD_OFFSET(daNpc_Kk1_c, mAAC, 0xAAC);
WWHD_OFFSET(daNpc_Kk1_c, mABC, 0xABC);
WWHD_OFFSET(daNpc_Kk1_c, mType, 0xACB);
WWHD_SIZE(daNpc_Kk1_c, 0xAD0);
