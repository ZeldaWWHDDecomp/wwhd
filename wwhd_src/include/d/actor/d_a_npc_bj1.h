/* daNpc_Bj1_c (the Koroks of the Forest Haven), WWHD layout. 
 *
 * The GameCube header has no members (every function is a "Nonmatching" stub), so the layout is
 * measured from the WWHD code (the inline constructor in _create 021F5220 and the field accesses of
 * all the unit's functions). Fields without a known meaning are named after their HD offset.
 * Base: fopNpc_npc_c (0x7DC in HD). Size 0x9C8 (profiles 0x101BC3F4.., methods 0x101BBB98).
 * vtable 0x10016EF0 (stored at +0xB4 by _create). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l */

#define BJ1_VTBL 0x10016EF0

struct daNpc_Bj1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ u8 _01[3];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void nodeBj1Control(J3DNode*, J3DModel*);
    void nodePrpControl(J3DNode*, J3DModel*);
    BOOL init_BJ4_0();
    BOOL init_BJ6_0();
    BOOL init_BJ7_0();
    BOOL init_BJX_0();
    BOOL init_BJX_1();
    BOOL createInit();
    void setMtx_anmProc();
    void setMtx(int);
    u32 anmNum_toResID(int);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int);
    BOOL setAnm();
    void setAnm_prp(s8);
    void chg_anmTag();
    void control_anmTag();
    void chg_anmAtr(u8);
    void control_anmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    void eventOrder();
    void checkOrder();
    BOOL chk_talk();
    BOOL chk_drct(f32);
    BOOL chk_partsNotMove();
    void lookBack();
    BOOL getMaskInf(u8*);
    BOOL chkReg(u16);
    void setReg(u16);
    u32 next_msgStatus(be<u32>*);
    u32 getMsg_BJ1_0();
    u32 getMsg_BJ2_0();
    u32 getMsg_BJ3_0();
    u32 getMsg_BJ4_0();
    u32 getMsg_BJ5_0();
    u32 getMsg_BJ6_0();
    u32 getMsg_BJ7_0();
    u32 getMsg_BJ8_0();
    u32 getMsg_BJ9_0();
    u32 getMsg_Corog();
    u32 getMsg();
    BOOL chkAttention();
    void setAttention(bool);
    fopAc_ac_c* searchByID(fpc_ProcID);
    BOOL partner_srch_sub(u32 cb);
    void partner_srch();
    void setCollision_SP_();
    void set_pthPoint(u8);
    void bj_clcFlySpd();
    u32 bj_movPass(int);
    BOOL bj_flyMove();
    void bj_clcMovSpd();
    void bj_nMove();
    void setPrtcl_drugPot_1();
    void setPrtcl_drugPot_2();
    void delPrtcl_drugPot();
    void setPrtcl_danceLR();
    void flwPrtcl_danceLR();
    void delPrtcl_danceLR();
    void setPrtcl_peraProOpen();
    BOOL createSeed();
    BOOL deleteSeed();
    BOOL charDecide(int);
    void eInit_setLocFlag(int*);
    void eInit_setShapeAngleY(int*, s16);
    void eInit_setEvTimer(int*);
    u32 eInit_calcRelativPos(cXyz*, cXyz*, void*);
    f32 eInit_prmFloat(f32*, f32);
    void eInit_ATTENTION_(int*, int*, int*, cXyz*, int*, int*, int*);
    void eInit_PLYER_MOV_1_();
    void eInit_MOV_(f32*, f32*, f32*, int*);
    void eInit_JMP_(f32*, f32*);
    void eInit_CHG_PTH_(int*, int*);
    void eInit_END_MOV_();
    void eInit_SET_TNE_();
    void eInit_DEL_TNE_();
    void eInit_SET_ANM_(int*, f32*);
    void event_actionInit(int);
    void eMove_ATTENTION_();
    u32 eMove_MOV_();
    u32 eMove_JMP_();
    BOOL eMove_SET_TNE_();
    BOOL eMove_PTH_MOV_();
    u32 event_action();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL wait_4();
    BOOL flyMov();
    BOOL fall01();
    BOOL talk_1();
    BOOL walk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    BOOL wait_action3(void*);
    BOOL wait_action4(void*);
    BOOL demo();
    void shadowDraw();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    J3DModelData* create_Anm();
    J3DModelData* create_prp_Anm();
    BOOL create_itm_Mdl();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m7E4;  /* joint number */
    /* 0x7E5 */ be<s8> m7E5;  /* joint number */
    /* 0x7E6 */ be<s8> m7E6;  /* joint number */
    /* 0x7E7 */ be<s8> m7E7;  /* joint number */
    /* 0x7E8 */ gptr<J3DModel> mpMdl7E8;  /* item model (create_itm_Mdl) */
    /* 0x7EC */ gptr<J3DModel> mpMdl7EC;  /* item model (create_itm_Mdl) */
    /* 0x7F0 */ gptr<J3DModel> mpMdl7F0;  /* item model (create_itm_Mdl) */
    /* 0x7F4 */ be<s8> m7F4;  /* joint number in an item model */
    /* 0x7F5 */ be<s8> m7F5;  /* joint number in an item model */
    /* 0x7F6 */ u8 _7F6[0x2];
    /* 0x7F8 */ gptr<mDoExt_McaMorf> mpPrpMorf;  /* the propeller morf (create_prp_Anm) */
    /* 0x7FC */ be<s8> m7FC;  /* joint number */
    /* 0x7FD */ be<s8> m7FD;  /* joint number */
    /* 0x7FE */ u8 _7FE[0x6];
    /* 0x804 */ be<s16> m804;
    /* 0x806 */ be<s16> m806;
    /* 0x808 */ u8 _808[0x4];
    /* 0x80C */ be<s16> m80C;
    /* 0x80E */ be<s16> m80E;
    /* 0x810 */ be<u32> m810;
    /* 0x814 */ u8 _814[0x30];
    /* 0x844 */ be<u32> m844;
    /* 0x848 */ u8 _848[0x1];
    /* 0x849 */ be<u8> m849;
    /* 0x84A */ be<u8> m84A;
    /* 0x84B */ u8 _84B[0x1];
    /* 0x84C */ dNpc_EventCut_c mEventCut2;  /* the actor's own dNpc_EventCut_c (constructor 0259F740) */
    /* 0x8B8 */ be<u32> m8B8;
    /* 0x8BC */ be<u32> m8BC;
    /* 0x8C0 */ be<u32> m8C0;
    /* 0x8C4 */ be<u32> m8C4;
    /* 0x8C8 */ be<s16> m8C8;
    /* 0x8CA */ be<s16> m8CA;
    /* 0x8CC */ be<s16> m8CC;
    /* 0x8CE */ u8 _8CE[0x2];
    /* 0x8D0 */ be<f32> m8D0;
    /* 0x8D4 */ be<f32> m8D4;
    /* 0x8D8 */ be<f32> m8D8;
    /* 0x8DC */ be<u32> m8DC;
    /* 0x8E0 */ be<f32> m8E0;
    /* 0x8E4 */ be<u32> m8E4;
    /* 0x8E8 */ be<u32> m8E8;
    /* 0x8EC */ be<f32> m8EC;
    /* 0x8F0 */ be<u32> m8F0;
    /* 0x8F4 */ be<f32> m8F4;
    /* 0x8F8 */ be<f32> m8F8;
    /* 0x8FC */ be<f32> m8FC;
    /* 0x900 */ be<f32> m900;
    /* 0x904 */ be<f32> m904;
    /* 0x908 */ be<f32> m908;
    /* 0x90C */ be<f32> m90C;
    /* 0x910 */ be<f32> m910;
    /* 0x914 */ be<f32> m914;
    /* 0x918 */ be<f32> m918;
    /* 0x91C */ be<f32> m91C;
    /* 0x920 */ u8 _920[0x4];
    /* 0x924 */ be<f32> m924;
    /* 0x928 */ u8 _928[0x4];
    /* 0x92C */ be<f32> m92C;
    /* 0x930 */ be<s16> m930;
    /* 0x932 */ be<s16> m932;
    /* 0x934 */ be<s16> m934;
    /* 0x936 */ u8 _936[0x6];
    /* 0x93C */ be<s16> m93C;
    /* 0x93E */ be<s16> m93E;
    /* 0x940 */ be<s16> m940;
    /* 0x942 */ be<s16> m942;
    /* 0x944 */ be<s16> m944;
    /* 0x946 */ u8 _946[0x2];
    /* 0x948 */ be<s16> m948;
    /* 0x94A */ be<s16> m94A;
    /* 0x94C */ be<u8> m94C;
    /* 0x94D */ be<u8> m94D;
    /* 0x94E */ be<u8> m94E;
    /* 0x94F */ be<u8> m94F;
    /* 0x950 */ be<u8> m950;
    /* 0x951 */ be<u8> m951;
    /* 0x952 */ be<u8> m952;
    /* 0x953 */ be<u8> m953;
    /* 0x954 */ be<u8> m954;
    /* 0x955 */ be<u8> m955;
    /* 0x956 */ be<u8> m956;
    /* 0x957 */ be<u8> m957;
    /* 0x958 */ be<u8> m958;
    /* 0x959 */ be<u8> m959;
    /* 0x95A */ be<u8> m95A;
    /* 0x95B */ be<u8> m95B;
    /* 0x95C */ be<u8> m95C;
    /* 0x95D */ be<u8> m95D;
    /* 0x95E */ be<u8> m95E;
    /* 0x95F */ u8 _95F[0x1];
    /* 0x960 */ be<u32> m960;
    /* 0x964 */ be<u8> m964;
    /* 0x965 */ be<u8> m965;
    /* 0x966 */ be<u8> m966;
    /* 0x967 */ be<u8> m967;
    /* 0x968 */ be<s16> m968;
    /* 0x96A */ be<s16> m96A;
    /* 0x96C */ be<s16> m96C;
    /* 0x96E */ u8 _96E[0x2];
    /* 0x970 */ be<f32> m970;
    /* 0x974 */ be<f32> m974;
    /* 0x978 */ be<f32> m978;
    /* 0x97C */ be<f32> m97C;
    /* 0x980 */ be<f32> m980;
    /* 0x984 */ be<f32> m984;
    /* 0x988 */ be<u32> m988;
    /* 0x98C */ be<u32> m98C;
    /* 0x990 */ be<u32> m990;
    /* 0x994 */ be<u32> m994;
    /* 0x998 */ be<u32> m998;
    /* 0x99C */ be<u32> m99C;
    /* 0x9A0 */ be<u32> m9A0;
    /* 0x9A4 */ be<s16> m9A4;
    /* 0x9A6 */ u8 _9A6[0x12];
    /* 0x9B8 */ be<u8> m9B8;
    /* 0x9B9 */ be<u8> m9B9;
    /* 0x9BA */ be<u8> m9BA;
    /* 0x9BB */ be<u8> m9BB;
    /* 0x9BC */ be<u8> m9BC;
    /* 0x9BD */ be<u8> m9BD;
    /* 0x9BE */ be<u8> m9BE;
    /* 0x9BF */ be<u8> m9BF;
    /* 0x9C0 */ be<u8> m9C0;
    /* 0x9C1 */ be<u8> m9C1;
    /* 0x9C2 */ be<u8> m9C2;
    /* 0x9C3 */ be<u8> m9C3;
    /* 0x9C4 */ be<s8> mType;  /* the Korok (charDecide) */
    /* 0x9C5 */ be<u8> m9C5;
    /* 0x9C6 */ be<u8> m9C6;
    /* 0x9C7 */ be<u8> m9C7;
};
WWHD_OFFSET(daNpc_Bj1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Bj1_c, mEventCut2, 0x84C);
WWHD_SIZE(daNpc_Bj1_c, 0x9C8);
