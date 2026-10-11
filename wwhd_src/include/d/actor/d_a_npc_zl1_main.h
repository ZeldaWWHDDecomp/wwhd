/* daNpc_Zl1_c (Tetra), WWHD layout.
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_zl1 (constructor 02303728 allocates
 * 0xD74; GameCube 0x900):
 * - fopNpc_npc_c is 0x7DC (see d/d_npc.h): members +0x118 up to mArcName (0x7F4);
 * - mShadowId (GameCube 0x6E0) is gone (HD shadows): mBtkAnm (0x7FC) is +0x114;
 * - mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14 to 0x74, and HD adds a second btp
 *   animation (0x8E8, set up together with the first by setBtp): +0x174 for mBtkAnmFrame /
 *   mBtpAnm, +0x248 for mBtpAnmFrame / mTimer;
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: +0x244 from mProcId1
 *   up to mLightInfluence1;
 * - LIGHT_INFLUENCE grew from 0x20 to 0x24 (an HD f32 at +0x20, 1.0 by the constructor): +0x248
 *   for mLightInfluence2, +0x24C from field_0x830 to field_0x8A8;
 * - HD: two s32 material indices at 0xB0C/0xB10 (bodyCreateHeap); the four
 *   mDoExt_*CupOn*AupPacket members grew from 0x10 to 0x98 each (0xB14..0xD74).
 * Field names are the GameCube ones (field_0xNNN = GameCube offset). */
#pragma once
#include "d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
struct ProcFunc_l { be<s16> d,i; be<u32> f; };
WWHD_SIZE(ProcFunc_l,8);

#define ZL1_SAFESTRING_VTBL 0x10023B10 /* this TU's sead::SafeString vtable */


/* daNpc_Zl1_matAnm_c : J3DMaterialAnm (HD size and layout as daNpc_Ls1_matAnm_c: vtable at +0x68,
 * to be confirmed by the constructor 0230242C) */
struct daNpc_Zl1_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ be<f32> mOffsetX; /* mOffset.x */
    /* 0x70 */ be<f32> mOffsetY; /* mOffset.y */
    /* 0x74 */ u8 field_0x74[8];
    /* 0x7C */ be<u8> field_0x7C; /* move flag */
    /* 0x7D */ u8 _7D[3];
};
WWHD_SIZE(daNpc_Zl1_matAnm_c, 0x80);

/* LIGHT_INFLUENCE: the shared one (bindings.h, HD 0x24) */

struct daNpc_Zl1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> field_0x0; /* bck number */
        /* 0x01 */ be<s8> field_0x1; /* btp number */
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mPlaySpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    BOOL set_startPos(int);
    bool init_ZL1_0();
    bool init_ZL1_1();
    bool init_ZL1_2();
    bool init_ZL1_3();
    bool init_ZL1_4();
    bool init_ZL1_5();
    bool init_ZL1_6();
    bool init_ZL1_7();
    void setEyeCtrl();
    void clrEyeCtrl();
    bool createInit();
    void play_animation();
    bool swoon_OnShip();
    void setMtx(u32);
    u32 bckResID(int); /* char* */
    u32 btpResID(int);
    u32 btkResID(int);
    u32 setBtp(s32, u32); /* HD: the full register reaches btpResID */
    void setMat();
    u32 setBtk(s32, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void eye_ctrl();
    void play_btk_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_ZL1_2();
    u32 getMsg_ZL1_4();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    u8 partner_search_sub(u32);
    void partner_search();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    f32 get_prmFloat(be<f32>*, f32);
    void set_LightPos(cXyz*); /* returns a cXyz through a hidden result pointer */
    void init_Light();
    void incEnvironment();
    void decEnvironment();
    void darkProc();
    void cut_init_LOK_PLYER(int);
    void cut_init_LOK_PARTNER(int);
    void cut_init_CHG_ANM_ATR(int);
    void cut_init_PLYER_TRN_PARTNER(int);
    void cut_init_PLYER_TRN_TETRA(int);
    void cut_init_MAJYU_START(int);
    void cut_init_OKIRU(int);
    u32 cut_move_OKIRU();
    void cut_init_OKIRU_2(int);
    u32 cut_move_OKIRU_2();
    void cut_init_DRW_ONOFF(int);
    void cut_init_PLYER_DRW_ONOFF(int);
    void cut_init_JMP_OFF(int);
    u32 cut_move_JMP_OFF();
    void cut_init_OMAMORI_ONOFF(int);
    void cut_init_SURPRISED(int);
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, f32, s16, cXyz*);
    void setWaterRipple();
    void setWaterSplash();
    void set_simpleLand(u32);
    void setEff();
    BOOL setFrontWallType();
    u8 move_jmp();
    void kyoroPos(cXyz*, int); /* returns a cXyz through a hidden result pointer */
    BOOL kyorokyoro();
    BOOL wait_1();
    BOOL talk_1();
    BOOL demo_1();
    BOOL demo_2();
    BOOL demo_3();
    BOOL demo_4();
    BOOL optn_1();
    BOOL optn_2();
    BOOL optn_3();
    BOOL wait_action1(void*);
    BOOL demo_action1(void*);
    BOOL demo_action2(void*);
    BOOL optn_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;     /* GameCube 0x6C4 */
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ u8 field_0x6CE[2];
    /* 0x7E8 */ gptr<J3DModel> mpModel;                   /* GameCube 0x6D0 */
    /* 0x7EC */ gptr<daNpc_Zl1_matAnm_c> field_0x6D4[2];
    /* 0x7F4 */ char mArcName[3];                         /* GameCube 0x6DC */
    /* 0x7F7 */ u8 _7F7;
    /* 0x7F8 */ u8 field_0x6E4[4];                        /* HD: no mShadowId before it */
    /* 0x7FC */ u8 mBtkAnm[0x74];                         /* mDoExt_btkAnm (HD 0x74), GameCube 0x6E8 */
    /* 0x870 */ be<u8> mBtkAnmFrame;                      /* GameCube 0x6FC */
    /* 0x871 */ u8 _871[3];
    /* 0x874 */ u8 mBtpAnm[0x74];                         /* mDoExt_btpAnm (HD 0x74), GameCube 0x700 */
    /* 0x8E8 */ u8 mBtpAnm2[0x74];                        /* HD: a second mDoExt_btpAnm */
    /* 0x95C */ be<u8> mBtpAnmFrame;                      /* GameCube 0x714 */
    /* 0x95D */ u8 _95D;
    /* 0x95E */ be<s16> mTimer;                           /* GameCube 0x716 */
    /* 0x960 */ ProcFunc_l mCurrActionFunc;               /* GameCube 0x718 (12 bytes) */
    /* 0x968 */ be<u32> mProcId1;                         /* GameCube 0x724 */
    /* 0x96C */ be<u32> mProcId2;
    /* 0x970 */ cXyz field_0x72C;
    /* 0x97C */ csXyz field_0x738;
    /* 0x982 */ csXyz field_0x73E;
    /* 0x988 */ csXyz field_0x744;
    /* 0x98E */ u8 field_0x74A[2];
    /* 0x990 */ cXyz field_0x74C;
    /* 0x99C */ cXyz field_0x758;
    /* 0x9A8 */ cXyz field_0x764;
    /* 0x9B4 */ cXyz field_0x770;
    /* 0x9C0 */ cXyz field_0x77C;
    /* 0x9CC */ be<f32> mRatio;                           /* GameCube 0x788 */
    /* 0x9D0 */ be<f32> mFrame;
    /* 0x9D4 */ u8 field_0x790[4];
    /* 0x9D8 */ csXyz field_0x794;
    /* 0x9DE */ u8 _9DE[2];
    /* 0x9E0 */ be<s32> field_0x79C;
    /* 0x9E4 */ be<s16> mEventIdx[4];                     /* GameCube 0x7A0 */
    /* 0x9EC */ be<s16> field_0x7A8;
    /* 0x9EE */ u8 field_0x7AA[4];
    /* 0x9F2 */ be<s16> field_0x7AE;
    /* 0x9F4 */ be<s16> field_0x7B0;
    /* 0x9F6 */ be<s16> field_0x7B2;
    /* 0x9F8 */ u8 field_0x7B4[2];
    /* 0x9FA */ be<s16> field_0x7B6;
    /* 0x9FC */ be<s16> field_0x7B8;
    /* 0x9FE */ be<s16> field_0x7BA;
    /* 0xA00 */ be<s16> field_0x7BC;
    /* 0xA02 */ u8 field_0x7BE[2];
    /* 0xA04 */ be<s16> field_0x7C0;
    /* 0xA06 */ be<s8> field_0x7C2;                       /* GameCube 0x7C2 */
    /* 0xA07 */ be<s8> field_0x7C3;
    /* 0xA08 */ be<s8> field_0x7C4;
    /* 0xA09 */ be<u8> mItemNo;
    /* 0xA0A */ be<u8> field_0x7C6;
    /* 0xA0B */ be<u8> field_0x7C7;
    /* 0xA0C */ be<u8> field_0x7C8;
    /* 0xA0D */ be<u8> field_0x7C9;
    /* 0xA0E */ be<u8> field_0x7CA;
    /* 0xA0F */ be<u8> field_0x7CB;
    /* 0xA10 */ be<u8> field_0x7CC;
    /* 0xA11 */ u8 field_0x7CD;
    /* 0xA12 */ be<u8> field_0x7CE;
    /* 0xA13 */ be<u8> mStateIsComplaete;
    /* 0xA14 */ be<u8> field_0x7D0;
    /* 0xA15 */ be<u8> field_0x7D1;
    /* 0xA16 */ be<u8> field_0x7D2;
    /* 0xA17 */ be<u8> field_0x7D3;
    /* 0xA18 */ be<u8> field_0x7D4;
    /* 0xA19 */ be<u8> field_0x7D5;
    /* 0xA1A */ be<u8> mHasAttention;
    /* 0xA1B */ be<u8> field_0x7D7;
    /* 0xA1C */ be<u8> field_0x7D8;
    /* 0xA1D */ be<u8> field_0x7D9;
    /* 0xA1E */ u8 _A1E[2];
    /* 0xA20 */ u8 mRippleCallBack[0x14];                 /* dPa_rippleEcallBack, GameCube 0x7DC */
    /* 0xA34 */ LIGHT_INFLUENCE mLightInfluence1;       /* GameCube 0x7F0 */
    /* 0xA58 */ LIGHT_INFLUENCE mLightInfluence2;       /* GameCube 0x810 */
    /* 0xA7C */ cXyz field_0x830;                         /* GameCube 0x830 */
    /* 0xA88 */ be<s16> field_0x83C;
    /* 0xA8A */ be<s16> field_0x83E;
    /* 0xA8C */ be<s16> field_0x840;
    /* 0xA8E */ be<s16> field_0x842;
    /* 0xA90 */ be<s8> mActIdx;                           /* GameCube 0x844 */
    /* 0xA91 */ be<u8> field_0x845;
    /* 0xA92 */ be<u8> field_0x846;
    /* 0xA93 */ be<s8> field_0x847;
    /* 0xA94 */ be<s8> field_0x848;
    /* 0xA95 */ be<s8> field_0x849;
    /* 0xA96 */ be<s8> field_0x84A;
    /* 0xA97 */ be<s8> field_0x84B;
    /* 0xA98 */ be<s8> field_0x84C;
    /* 0xA99 */ be<s8> field_0x84D;
    /* 0xA9A */ be<s8> field_0x84E;
    /* 0xA9B */ be<s8> field_0x84F;
    /* 0xA9C */ be<s8> field_0x850;
    /* 0xA9D */ be<s8> field_0x851;
    /* 0xA9E */ u8 _A9E[2];
    /* 0xAA0 */ gptr<J3DJoint> mJoint1;                   /* GameCube 0x854 */
    /* 0xAA4 */ gptr<J3DJoint> mJoint2;
    /* 0xAA8 */ gptr<J3DJoint> mJoint3;
    /* 0xAAC */ be<u32> field_0x860[6];                   /* J3DMaterial* */
    /* 0xAC4 */ be<u32> field_0x878[6];                   /* J3DMaterial* */
    /* 0xADC */ be<u32> field_0x890[6];                   /* J3DShape* */
    /* 0xAF4 */ be<u32> field_0x8A8[6];                   /* J3DShape* */
    /* 0xB0C */ be<s32> mB0C;                             /* HD: material index (bodyCreateHeap) */
    /* 0xB10 */ be<s32> mB10;                             /* HD: material index (bodyCreateHeap) */
    /* 0xB14 */ u8 mOffCupOnAupPacket1[0x98];             /* GameCube 0x8C0 (0x10) */
    /* 0xBAC */ u8 mOffCupOnAupPacket2[0x98];
    /* 0xC44 */ u8 mOnCupOffAupPacket1[0x98];
    /* 0xCDC */ u8 mOnCupOffAupPacket2[0x98];
};
WWHD_OFFSET(daNpc_Zl1_c, mArcName, 0x7F4);
WWHD_OFFSET(daNpc_Zl1_c, mBtkAnm, 0x7FC);
WWHD_OFFSET(daNpc_Zl1_c, mBtpAnm, 0x874);
WWHD_OFFSET(daNpc_Zl1_c, mBtpAnmFrame, 0x95C);
WWHD_OFFSET(daNpc_Zl1_c, mCurrActionFunc, 0x960);
WWHD_OFFSET(daNpc_Zl1_c, mProcId1, 0x968);
WWHD_OFFSET(daNpc_Zl1_c, field_0x74C, 0x990);
WWHD_OFFSET(daNpc_Zl1_c, mRatio, 0x9CC);
WWHD_OFFSET(daNpc_Zl1_c, field_0x79C, 0x9E0);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7B8, 0x9FC);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7C2, 0xA06);
WWHD_OFFSET(daNpc_Zl1_c, mStateIsComplaete, 0xA13);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7D9, 0xA1D);
WWHD_OFFSET(daNpc_Zl1_c, mRippleCallBack, 0xA20);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence1, 0xA34);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence2, 0xA58);
WWHD_OFFSET(daNpc_Zl1_c, field_0x830, 0xA7C);
WWHD_OFFSET(daNpc_Zl1_c, field_0x84A, 0xA96);
WWHD_OFFSET(daNpc_Zl1_c, field_0x84F, 0xA9B);
WWHD_OFFSET(daNpc_Zl1_c, mJoint1, 0xAA0);
WWHD_OFFSET(daNpc_Zl1_c, mB0C, 0xB0C);
WWHD_OFFSET(daNpc_Zl1_c, mOffCupOnAupPacket1, 0xB14);
WWHD_SIZE(daNpc_Zl1_c, 0xD74);

/* daNpc_Zl1_HIO_c (HD: vtable first, size 0x64; l_HIO at 0x10468C20, a_prm_tbl at 0x101C7574) */
struct daNpc_Zl1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> field_18;
        /* 0x14 */ be<f32> field_1C;
        /* 0x18 */ be<u8> field_20;
        /* 0x19 */ be<u8> field_21;
        /* 0x1A */ be<u8> field_22;
        /* 0x1B */ be<u8> field_23;
        /* 0x1C */ be<f32> field_24;
        /* 0x20 */ be<f32> field_28;
        /* 0x24 */ be<s16> field_2C;
        /* 0x26 */ be<s16> field_2E;
        /* 0x28 */ be<s16> field_30;
        /* 0x2A */ be<s16> field_32;
        /* 0x2C */ be<f32> field_34;
        /* 0x30 */ be<f32> field_38;
        /* 0x34 */ be<f32> field_3C;
        /* 0x38 */ be<f32> field_40;
        /* 0x3C */ be<f32> field_44;
        /* 0x40 */ be<f32> field_48;
        /* 0x44 */ be<f32> field_4C;
        /* 0x48 */ be<f32> field_50;
        /* 0x4C */ be<f32> field_54;
        /* 0x50 */ be<f32> field_58;
        /* 0x54 */ be<s16> field_5C;
        /* 0x56 */ be<s16> field_5E;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Zl1_HIO_c, 0x64);


namespace zl1_main {
inline daNpc_Zl1_HIO_c& hio() { return *gabi::at<daNpc_Zl1_HIO_c>(0x10468C20); }
struct Player : fopAc_ac_c { u8 pad3AC[0xC]; be<u32> mNoResetFlg0; u8 pad3BC[0x66]; be<s16> mDemoMoveAngle; };
WWHD_OFFSET(Player,mNoResetFlg0,0x3B8);
WWHD_OFFSET(Player,mDemoMoveAngle,0x422);
WWHD_SIZE(Player,0x424);
inline Player* player(fopAc_ac_c* p) { return reinterpret_cast<Player*>(p); }
struct SafeStringVtable { u8 pad0[0x14]; be<u32> assureTermination; };
WWHD_OFFSET(SafeStringVtable,assureTermination,0x14);
WWHD_SIZE(SafeStringVtable,0x18);
inline void assureTermination(SafeString* s) { gabi::call_ptr(gabi::at<SafeStringVtable>(s->__vtbl)->assureTermination,s); }
inline const be<u8>* bytes(const char* s) { return gabi::at<const be<u8>>(gabi::guest_addr(s)); }
inline be<u8>* archiveBytes(daNpc_Zl1_c* a) { return reinterpret_cast<be<u8>*>(a->mArcName); }
struct ActionWords { be<u32> adjustmentAndIndex,target; };
WWHD_SIZE(ActionWords,8);
inline void copyAction(ProcFunc_l* dst,const ProcFunc_l* src) { auto* d=reinterpret_cast<ActionWords*>(dst); auto* s=reinterpret_cast<const ActionWords*>(src); d->adjustmentAndIndex=s->adjustmentAndIndex; d->target=s->target; }
using ActionFunction=BOOL (daNpc_Zl1_c::*)(void*);
inline void callAction(daNpc_Zl1_c* actor,ProcFunc_l* record,void* argument) { (actor->*(*reinterpret_cast<gabi::ptmf<ActionFunction>*>(record)))(argument); }
inline dCcD_SrcCyl* npcCylinderSource() { return gabi::at<dCcD_SrcCyl>(0x101EA190); }
inline dSv_restart_c* restart() { return &gabi::at<dSv_info_c>(dComIfGs_ea()+0x20)->mRestart; }
// Missing H14 accessor: event-manager cancelStartDemo uses the play exception subobject.
inline void* eventException() { return gabi::at<void>(dComIfGp_ea()+0x52E8); }
constexpr u32 searchActor_Branch=0x023035BC,searchActor_Bm1=0x0230363C;
}
void Zl1main_cutEffectLight(LIGHT_INFLUENCE* light);
BOOL Zl1main_searchByIdOut(fpc_ProcID id,gptr<fopAc_ac_c>* actor);
fopAc_ac_c* Zl1main_searchByID(daNpc_Zl1_c* actor,fpc_ProcID id,be<s32>* deleted);
void Zl1main_cancelStartDemo(void* exception);
void Zl1main_setRestartRoom(dSv_restart_c* restart,const cXyz* pos,s16 angle,s8 room);
BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* morf,s32 loopMode,f32 morfF,f32 speed,u32 name,s32 snd,const char* arc);
s32 Zl1main_getRndValue(s32 base,s32 range);
s8 dBgS_GetRoomId(dBgS* bgs,void* poly);
u8 dBgS_GetPolyColor(dBgS* bgs,void* poly);

namespace zl1_main {
inline const be<u8>* safeStringBytes(const SafeString* string) { return gabi::at<const be<u8>>(string->mStringTop); }
inline const char* staffName(const dNpc_EventCut_c& cut) { return gabi::at<const char>(cut.mpEvtStaffName); }
}

WWHD_OFFSET(ProcFunc_l,d,0);
WWHD_OFFSET(ProcFunc_l,i,2);
WWHD_OFFSET(ProcFunc_l,f,4);
WWHD_OFFSET(daNpc_Zl1_HIO_c,mPrmTbl,0xC);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c,field_1C,0x14);
WWHD_SIZE(daNpc_Zl1_HIO_c::hio_prm_c,0x58);
