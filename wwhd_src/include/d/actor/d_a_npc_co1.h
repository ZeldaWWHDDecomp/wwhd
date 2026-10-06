/* daNpc_Co1_c (Prince Komali, before Dragon Roost Cavern), WWHD layout.
 *
 * The GameCube header has no members (all functions are "Nonmatching" stubs), so the layout is
 * measured from the WWHD code (constructor 02227BBC allocates 0xA38; field accesses of all the
 * unit's functions). Fields without a known meaning are named after their offset.
 * Base: fopNpc_npc_c, 0x7DC in HD (see d_a_npc_ba1.h). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l */

struct daNpc_Co1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ be<s16> field_0x02;
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* methods (signatures: each part's owner fixes its own) */
    void nodeCo1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    u32 btpNum_toResID(int);        /* 022275E0 (called by setBtp) */
    BOOL setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);        /* 022276E0 */
    J3DModelData* create_prl_Anm();
    BOOL setBtk(u32);
    bool create_itm_Mdl();
    BOOL CreateHeap();
    bool charDecide(int);           /* 02227C34 */
    BOOL set_action(ProcFunc_l*, void*);
    bool init_CO1_0();
    void plyTexPttrnAnm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    u32 setAnm_tex(s8);
    u32 anmNum_toResID(int);        /* 02228A5C (setAnm_anm: body) */
    u32 anmNum_toResID_prl(int);    /* 02228A70 (setAnm_anm: pearl) */
    BOOL setAnm_anm(anm_prm_c*);
    bool setAnm();
    void setStt(s8);
    void endEvent();
    void setAnm_NUM(int, int);
    void eInit_MDR_();
    void eInit_RED_LTR_();
    void event_actionInit(int);
    u32 eMove_MDR_();
    u32 eMove_RED_LTR_();
    u32 event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void eventOrder();              /* 02229378 */
    void setCollision_SP_();
    BOOL _execute();
    BOOL _draw();
    void set_target(int);
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    void control_anmAtr();
    void anmAtr(u16);
    bool chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_CO1_0();
    u32 getMsg();
    u8 chkAttention();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wakeup();
    BOOL talk_1();
    BOOL toru_1();
    BOOL read_1();
    BOOL modoru();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m7E4;               /* joint numbers (s8) */
    /* 0x7E5 */ be<s8> m7E5;
    /* 0x7E6 */ be<s8> m7E6;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ be<u32> m7E8;              /* pointer */
    /* 0x7EC */ be<u32> m7EC;              /* pointer */
    /* 0x7F0 */ be<s8> m7F0;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ u8 mBtkAnm[0x74];          /* mDoExt_btkAnm (HD 0x74, constructor 025E7C6C) */
    /* 0x868 */ be<u8> m868;
    /* 0x869 */ u8 _869[3];
    /* 0x86C */ be<u32> m86C;              /* pointer */
    /* 0x870 */ be<u32> m870;              /* pointer */
    /* 0x874 */ u8 mBtpAnm[0x74];          /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8E8 */ be<u8> mBlinkFrame;
    /* 0x8E9 */ u8 _8E9;
    /* 0x8EA */ be<s16> mBlinkTimer;
    /* 0x8EC */ ProcFunc_l mCurrProcFunc;
    /* 0x8F4 */ u8 _8F4[8];
    /* 0x8FC */ dNpc_EventCut_c mEventCut;  /* hides fopNpc_npc_c::mEventCut (constructor 0259F740) */
    /* 0x968 */ u8 _968[4];
    /* 0x96C */ cXyz m96C;
    /* 0x978 */ be<s16> m978;
    /* 0x97A */ be<s16> m97A;
    /* 0x97C */ be<s16> m97C;
    /* 0x97E */ u8 _97E[2];
    /* 0x980 */ cXyz m980;
    /* 0x98C */ cXyz m98C;                 /* initial position (createInit copies current.pos) */
    /* 0x998 */ u8 _998[0xC];
    /* 0x9A4 */ be<f32> m9A4;
    /* 0x9A8 */ u8 _9A8[0x14];
    /* 0x9BC */ be<s16> m9BC;
    /* 0x9BE */ be<s16> m9BE;
    /* 0x9C0 */ be<s16> m9C0;
    /* 0x9C2 */ be<s16> mEventIdx[3];       /* createInit: dEvent_manager_c::getEventIdx of 3 names */
    /* 0x9C8 */ be<s16> m9C8;
    /* 0x9CA */ u8 _9CA[2];
    /* 0x9CC */ be<s16> m9CC;
    /* 0x9CE */ be<s16> m9CE;
    /* 0x9D0 */ be<s16> m9D0;
    /* 0x9D2 */ be<s16> m9D2;
    /* 0x9D4 */ be<s16> m9D4;
    /* 0x9D6 */ u8 _9D6[2];
    /* 0x9D8 */ be<s8> m9D8;
    /* 0x9D9 */ be<u8> m9D9;
    /* 0x9DA */ be<s8> m9DA;
    /* 0x9DB */ be<u8> m9DB;
    /* 0x9DC */ be<u8> m9DC;
    /* 0x9DD */ be<u8> m9DD;
    /* 0x9DE */ be<u8> m9DE;
    /* 0x9DF */ be<u8> m9DF;
    /* 0x9E0 */ be<u8> m9E0;
    /* 0x9E1 */ be<u8> m9E1;
    /* 0x9E2 */ be<u8> m9E2;
    /* 0x9E3 */ be<u8> m9E3;
    /* 0x9E4 */ be<u8> m9E4;
    /* 0x9E5 */ be<u8> m9E5;
    /* 0x9E6 */ u8 _9E6[2];
    /* 0x9E8 */ be<s32> m9E8;
    /* 0x9EC */ be<u8> m9EC;
    /* 0x9ED */ be<u8> m9ED;
    /* 0x9EE */ be<u8> m9EE;
    /* 0x9EF */ be<u8> m9EF;
    /* 0x9F0 */ cXyz m9F0;
    /* 0x9FC */ csXyz m9FC;
    /* 0xA02 */ u8 _A02[2];
    /* 0xA04 */ be<f32> mA04;
    /* 0xA08 */ be<f32> mA08;
    /* 0xA0C */ be<f32> mA0C;
    /* 0xA10 */ be<f32> mA10;               /* 1.0 in the constructor */
    /* 0xA14 */ be<f32> mA14;
    /* 0xA18 */ be<f32> mA18;
    /* 0xA1C */ be<f32> mA1C;
    /* 0xA20 */ be<f32> mA20;
    /* 0xA24 */ be<s16> mA24;
    /* 0xA26 */ u8 _A26[2];
    /* 0xA28 */ be<s8> mA28;
    /* 0xA29 */ be<s8> mA29;
    /* 0xA2A */ be<u8> mA2A;
    /* 0xA2B */ be<u8> mA2B;
    /* 0xA2C */ be<s8> mA2C;                /* btp number (iniTexPttrnAnm) */
    /* 0xA2D */ be<s8> mA2D;
    /* 0xA2E */ u8 _A2E[2];
    /* 0xA30 */ be<s8> mA30;                /* eventOrder: order kind */
    /* 0xA31 */ be<s8> mA31;
    /* 0xA32 */ be<s8> mA32;
    /* 0xA33 */ be<s8> mA33;
    /* 0xA34 */ be<s8> mA34;
    /* 0xA35 */ be<s8> mA35;                /* charDecide: type */
    /* 0xA36 */ be<s8> mA36;
    /* 0xA37 */ be<s8> mA37;
};
WWHD_OFFSET(daNpc_Co1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Co1_c, mBtkAnm, 0x7F4);
WWHD_OFFSET(daNpc_Co1_c, mBtpAnm, 0x874);
WWHD_OFFSET(daNpc_Co1_c, mCurrProcFunc, 0x8EC);
WWHD_OFFSET(daNpc_Co1_c, mEventCut, 0x8FC);
WWHD_OFFSET(daNpc_Co1_c, m98C, 0x98C);
WWHD_OFFSET(daNpc_Co1_c, mEventIdx, 0x9C2);
WWHD_OFFSET(daNpc_Co1_c, m9F0, 0x9F0);
WWHD_OFFSET(daNpc_Co1_c, mA10, 0xA10);
WWHD_OFFSET(daNpc_Co1_c, mA30, 0xA30);
WWHD_SIZE(daNpc_Co1_c, 0xA38);
