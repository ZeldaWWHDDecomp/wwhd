/* daNpc_Ls1_c (Aryll), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_ls1: fopNpc_npc_c is 0x7DC (see
 * d_a_npc_ba1.h). Members +0x118 up to mTelescopeScale/mpMatAnms/mArcName; mShadowID (GameCube
 * 0x6E4) is gone, so mpLsHandModel.. are +0x114; mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14
 * to 0x74 each (+0x180 / +0x1E0 after them) and mCurrProcFunc is 8 bytes (GHS) instead of 12,
 * so everything from mHand_L_Mtx on is +0x1D0. Size 0xA28 (GameCube 0x858). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

/* daNpc_Ls1_matAnm_c : J3DMaterialAnm, HD size 0x80 (constructor 0227DFF0), vtable at +0x68 */
struct daNpc_Ls1_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ be<f32> mOffsetX;
    /* 0x70 */ be<f32> mOffsetY;
    /* 0x74 */ u8 m74[0x7C - 0x74];
    /* 0x7C */ be<u8> mbMove;
    /* 0x7D */ u8 m7D[3];
};
WWHD_SIZE(daNpc_Ls1_matAnm_c, 0x80);

struct daNpc_Ls1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    void _nodeCB_Hand_L(J3DNode*, J3DModel*);
    void _nodeCB_Hand_R(J3DNode*, J3DModel*);
    void _Ls_hand_nodeCB_Hand_L(J3DNode*, J3DModel*);
    void _Ls_hand_nodeCB_Hand_R(J3DNode*, J3DModel*);
    bool init_LS1_0();
    bool init_LS1_1();
    bool init_LS1_2();
    bool init_LS1_3();
    bool init_LS1_4();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    int btkResID(int);
    u32 setBtp(s8, u32);
    void setMat();
    u32 setBtk(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void eye_ctrl();
    void play_btk_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, BOOL);
    void setAnm();
    u32 chngAnmTag();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR(BOOL);
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_LS1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    u8 partner_search_sub(u32);
    void partner_search();
    void setEyeCtrl();
    void clrEyeCtrl();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void cut_init_LOK_PLYER(int);
    void cut_init_PLYER_MOV(int);
    void cut_init_ANM_CHG(int);
    u32 cut_move_WAI();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, f32, s16, cXyz*);
    void get_playerEvnPos(cXyz*, int);
    u8 chkTelescope_sph(cXyz*, f32, f32);
    u8 chkTelescope(cXyz*, f32, f32);
    u8 telescope_proc();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL wait_4();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL demo_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL handCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num;
    /* 0x7E7 */ be<s8> m_hnd_R_jnt_num;
    /* 0x7E8 */ gptr<J3DModel> mpTelescopeModel;
    /* 0x7EC */ be<f32> mTelescopeScale;
    /* 0x7F0 */ gptr<daNpc_Ls1_matAnm_c> mpMatAnms[2];
    /* 0x7F8 */ char mArcName[3];
    /* 0x7FB */ u8 _7FB;
    /* 0x7FC */ gptr<J3DModel> mpLsHandModel;     /* HD: no mShadowID before it */
    /* 0x800 */ be<s8> m_lsHnd_L_jnt_num;
    /* 0x801 */ be<s8> m_lsHnd_R_jnt_num;
    /* 0x802 */ u8 _802[2];
    /* 0x804 */ u8 mBtkAnm[0x74];                 /* mDoExt_btkAnm (HD 0x74) */
    /* 0x878 */ be<u8> mBtkFrame;
    /* 0x879 */ u8 _879[3];
    /* 0x87C */ u8 mBtpAnm[0x74];                 /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8F0 */ be<u8> mBtpFrame;
    /* 0x8F1 */ u8 _8F1;
    /* 0x8F2 */ be<s16> mTimer1;
    /* 0x8F4 */ ProcFunc_l mCurrProcFunc;
    /* 0x8FC */ Mtx34 mHand_L_Mtx;
    /* 0x92C */ Mtx34 mHand_R_Mtx;
    /* 0x95C */ be<u32> mBm1ProcID;
    /* 0x960 */ be<u32> m790;
    /* 0x964 */ cXyz m794;
    /* 0x970 */ csXyz m7A0;
    /* 0x976 */ csXyz mAngle;
    /* 0x97C */ u8 m7AC[8];
    /* 0x984 */ cXyz mTransformedEyePos;
    /* 0x990 */ cXyz mPlayerEyePos;
    /* 0x99C */ cXyz m7CC[2];
    /* 0x9B4 */ cXyz m7E4;
    /* 0x9C0 */ u8 m7F0[0x10];
    /* 0x9D0 */ be<f32> mPrevMorfFrame;
    /* 0x9D4 */ be<s16> mActorAngleY;
    /* 0x9D6 */ be<s16> mJointHeadY;
    /* 0x9D8 */ be<s16> mJointBackboneY;
    /* 0x9DA */ u8 m80A[2];
    /* 0x9DC */ be<s32> mbSetEyePos;
    /* 0x9E0 */ be<u32> m810;
    /* 0x9E4 */ be<s16> mEventIDTbl[4];
    /* 0x9EC */ be<s16> mEventIndex;
    /* 0x9EE */ be<s16> mTimer2;
    /* 0x9F0 */ be<u8> m820;                      /* HD: used by telescope_proc */
    /* 0x9F1 */ be<u8> m821;                      /* HD: used by telescope_proc */
    /* 0x9F2 */ be<s16> mTimer3;
    /* 0x9F4 */ be<s16> mTimer4;
    /* 0x9F6 */ be<s16> mTimer5;
    /* 0x9F8 */ u8 m828[6];
    /* 0x9FE */ be<s16> m82E;
    /* 0xA00 */ be<s8> mbMorfAnimStopped;
    /* 0xA01 */ be<u8> m831;
    /* 0xA02 */ be<u8> mItemNo;
    /* 0xA03 */ be<u8> m833;
    /* 0xA04 */ be<u8> m834;
    /* 0xA05 */ be<u8> m835;
    /* 0xA06 */ be<u8> m836;
    /* 0xA07 */ be<u8> mbResLoadIsComplete;
    /* 0xA08 */ be<u8> m838;
    /* 0xA09 */ be<u8> mbEyeCtrlSet;
    /* 0xA0A */ be<u8> m83A;
    /* 0xA0B */ be<u8> m83B;
    /* 0xA0C */ be<u8> m83C;
    /* 0xA0D */ be<u8> m83D;
    /* 0xA0E */ be<u8> mbAttention;
    /* 0xA0F */ be<u8> m83F;
    /* 0xA10 */ be<u8> m840;
    /* 0xA11 */ be<u8> m841;
    /* 0xA12 */ be<s16> mHalfHeadAngleY;
    /* 0xA14 */ be<s16> mHalfHeadAngleX;
    /* 0xA16 */ be<s16> m846;
    /* 0xA18 */ be<s16> m848;
    /* 0xA1A */ be<s8> mActionIndex;
    /* 0xA1B */ be<u8> m84B;
    /* 0xA1C */ be<u8> mMesgAnimeTag;
    /* 0xA1D */ be<s8> mBtpNum;
    /* 0xA1E */ be<s8> mBtkNum;
    /* 0xA1F */ be<s8> mBckNum;
    /* 0xA20 */ be<s8> m850;
    /* 0xA21 */ be<s8> m851;
    /* 0xA22 */ be<s8> m852;
    /* 0xA23 */ be<s8> m853;
    /* 0xA24 */ be<s8> m854;
    /* 0xA25 */ be<s8> mType;
    /* 0xA26 */ be<s8> m856;
    /* 0xA27 */ be<s8> m857;
};
WWHD_OFFSET(daNpc_Ls1_c, mpLsHandModel, 0x7FC);
WWHD_OFFSET(daNpc_Ls1_c, mBtkAnm, 0x804);
WWHD_OFFSET(daNpc_Ls1_c, mBtpAnm, 0x87C);
WWHD_OFFSET(daNpc_Ls1_c, mCurrProcFunc, 0x8F4);
WWHD_OFFSET(daNpc_Ls1_c, mHand_L_Mtx, 0x8FC);
WWHD_OFFSET(daNpc_Ls1_c, mTransformedEyePos, 0x984);
WWHD_OFFSET(daNpc_Ls1_c, mPrevMorfFrame, 0x9D0);
WWHD_OFFSET(daNpc_Ls1_c, mbEyeCtrlSet, 0xA09);
WWHD_OFFSET(daNpc_Ls1_c, m857, 0xA27);
WWHD_SIZE(daNpc_Ls1_c, 0xA28);
