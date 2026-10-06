/* daNpc_Yw1_c (Sue-Belle, Outset), WWHD layout. 
 *
 * The GameCube header is empty (the TU is "Nonmatching"): members are named from their use in the
 * WWHD code. Size 0x940 (profile 101C6F84). Base: fopNpc_npc_c, HD 0x7DC (see d_a_npc_ob1.h,
 * which also holds the NPC local bindings both units use). */
#pragma once
#include "d/actor/d_a_npc_ob1.h"

struct daNpc_Yw1_c : fopNpc_npc_c_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Hair(J3DNode* node, J3DModel* model);
    void _nodeCB_Head(J3DNode* node, J3DModel* model);
    void _nodeCB_BackBone(J3DNode* node, J3DModel* model);
    BOOL bodyCreateHeap();
    s32 btpResID(int num);
    BOOL init_texPttrnAnm(s8 num, s32 modify);
    BOOL headCreateHeap();
    BOOL CreateHeap();
    u8 decideType(int type);
    BOOL set_action(ptmf_l* action, void* arg);
    void set_pthPoint(u8 idx);
    BOOL init_YW1_0();
    BOOL init_YW1_1();
    BOOL init_YW1_2();
    BOOL init_YW1_3();
    void play_texPttrnAnm();
    void play_animation();
    void setHairAngle();
    fopAc_ac_c* searchByID(fpc_ProcID id, be<s32>* pErr);
    u8 upLift();
    void setAttention(s32 force);
    void setMtx(s32 force);
    BOOL createInit();
    cPhs_State _create();
    BOOL _delete();
    BOOL partner_search_sub(u32 searchFn);
    void partner_search();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void endEvent();
    void privateCut(int staffIdx);
    void lookBack();
    void event_proc(int staffIdx);
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 bckResID(int num);
    void setAnm_anm(anm_prm_c* prm);
    void setAnm_NUM(int num, int withTex);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u8 atr);
    void anmAtr(u16 msgStatus);
    u16 next_msgStatus(u32* pMsgNo);
    u32 getMsg_YW1_0();
    u32 getMsg_YW1_1();
    u32 getMsg_YW1_2();
    u32 getMsg_YW1_3();
    u32 getMsg();
    BOOL chk_talk();
    u8 chk_parts_notMov();
    BOOL chkAttention();
    void chngTsuboAnm();
    void setStt(s8 stt);
    BOOL chk_areaIN(f32 r, f32 h, s16 ang, cXyz* pos);
    u8 chk_brkTsubo();
    u8 chk_bm1Odoroki();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL walk_1();
    BOOL turn_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hair1_jnt_num;
    /* 0x7E7 */ be<s8> m_hair2_jnt_num;
    /* 0x7E8 */ be<s8> m_hair3_jnt_num;
    /* 0x7E9 */ u8 _7E9[7];
    /* 0x7F0 */ gptr<J3DModel> mpHedModel;
    /* 0x7F4 */ mDoExt_btpAnm_ob1 mBtpAnm;     /* its J3DAnmTexPattern* at +0x10 (0x804) */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ ptmf_l mAction;
    /* 0x874 */ be<u32> mBm1Id;               /* the cat (Bm1) */
    /* 0x878 */ be<u32> mTsuboId;             /* the pot she carries */
    /* 0x87C */ be<u32> mLookActorId;
    /* 0x880 */ dNpc_PathRun_c_l mPathRun;
    /* 0x888 */ cXyz mHomePos;
    /* 0x894 */ csXyz mHomeAngle;
    /* 0x89A */ csXyz mAngle;
    /* 0x8A0 */ cXyz mEyePos;
    /* 0x8AC */ cXyz mLookPos;
    /* 0x8B8 */ cXyz mAttPos;
    /* 0x8C4 */ be<f32> mPrevFrame;
    /* 0x8C8 */ u8 _8C8[4];
    /* 0x8CC */ be<s16> mSaveAngleY;
    /* 0x8CE */ be<s16> mSaveHeadY;
    /* 0x8D0 */ be<s16> mSaveBboneY;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<s32> mActRet;
    /* 0x8D8 */ u8 _8D8[2];
    /* 0x8DA */ be<s16> mWaitTimer;
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ be<s16> mHeadTurnSpd;
    /* 0x8E2 */ be<s16> mTargetAngY;
    /* 0x8E4 */ be<s8> mAnmEnd;
    /* 0x8E5 */ be<s8> mLoopCnt;
    /* 0x8E6 */ be<u8> mPreItemNo;
    /* 0x8E7 */ be<u8> mbTsubo;
    /* 0x8E8 */ be<u8> m8E8;
    /* 0x8E9 */ be<u8> mbTsuboLost;
    /* 0x8EA */ be<u8> mbPathEnd;
    /* 0x8EB */ be<u8> m8EB;
    /* 0x8EC */ be<u8> m8EC;
    /* 0x8ED */ be<u8> m8ED;
    /* 0x8EE */ be<u8> mbInit;
    /* 0x8EF */ be<u8> mbAttention;
    /* 0x8F0 */ be<u8> mbTalk;
    /* 0x8F1 */ be<u8> mbHeadOnly;
    /* 0x8F2 */ be<u8> mbDemo;
    /* 0x8F3 */ u8 _8F3;
    /* 0x8F4 */ be<s16> m8F4;               /* hair (setHairAngle / _nodeCB_Hair) */
    /* 0x8F6 */ be<s16> m8F6;
    /* 0x8F8 */ be<s16> m8F8;
    /* 0x8FA */ be<s16> m8FA;
    /* 0x8FC */ u8 _8FC[0x908 - 0x8FC];
    /* 0x908 */ cXyz mHairPrevPos;
    /* 0x914 */ be<s16> m914;
    /* 0x916 */ be<s16> m916;
    /* 0x918 */ be<s16> m918;
    /* 0x91A */ be<s16> m91A;
    /* 0x91C */ be<s16> m91C;
    /* 0x91E */ be<s16> m91E;
    /* 0x920 */ be<s16> m920;
    /* 0x922 */ be<s16> m922;
    /* 0x924 */ be<s16> m924;
    /* 0x926 */ be<s16> m926;
    /* 0x928 */ be<s16> mHairWave;
    /* 0x92A */ be<s16> m92A;
    /* 0x92C */ be<s16> m92C;
    /* 0x92E */ be<s16> m92E;
    /* 0x930 */ be<s8> mActIdx;
    /* 0x931 */ be<u8> mAnmAtr;
    /* 0x932 */ be<u8> mAnmTag;
    /* 0x933 */ be<s8> mTexNo;
    /* 0x934 */ be<s8> mAnmNo;
    /* 0x935 */ be<s8> mOrderType;
    /* 0x936 */ be<s8> mStt;
    /* 0x937 */ be<s8> mPrevStt;
    /* 0x938 */ be<s8> mLookMode;
    /* 0x939 */ be<s8> mHioNo;
    /* 0x93A */ be<s8> mType;
    /* 0x93B */ be<s8> mActStep;
    /* 0x93C */ be<s8> mAtrCnt;
    /* 0x93D */ u8 _93D[3];
};
WWHD_OFFSET(daNpc_Yw1_c, mBtpFrame, 0x868);
WWHD_OFFSET(daNpc_Yw1_c, mPathRun, 0x880);
WWHD_OFFSET(daNpc_Yw1_c, mAttPos, 0x8B8);
WWHD_OFFSET(daNpc_Yw1_c, mActRet, 0x8D4);
WWHD_OFFSET(daNpc_Yw1_c, m8F4, 0x8F4);
WWHD_OFFSET(daNpc_Yw1_c, m914, 0x914);
WWHD_OFFSET(daNpc_Yw1_c, mActIdx, 0x930);
WWHD_SIZE(daNpc_Yw1_c, 0x940);

/* l_HIO (0x10468A24, 0x44): vtable, s8 mNo, s32, daNpc_Yw1_childHIO_c mChild[1] (0x38 each; the
 * prm table 101C6F2C is copied to +4 of the child) */
struct daNpc_Yw1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s16> mPrm[9];          /* jnt setParam */
    /* 0x16 */ be<s16> mHeadTurnSpd;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<u8> mDebugDraw;
    /* 0x1D */ u8 _1D[3];
    /* 0x20 */ be<f32> mAreaRadius;
    /* 0x24 */ be<s16> mTurnScale;
    /* 0x26 */ be<s16> mTurnStep;
    /* 0x28 */ be<f32> mWalkAnmRate;
    /* 0x2C */ be<f32> mWalkSpd;
    /* 0x30 */ be<f32> mWalkAccel;
    /* 0x34 */ be<u32> field_0x34;
};
WWHD_SIZE(daNpc_Yw1_childHIO_c, 0x38);
struct daNpc_Yw1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ daNpc_Yw1_childHIO_c mChild[1];
};
WWHD_SIZE(daNpc_Yw1_HIO_c, 0x44);
inline daNpc_Yw1_childHIO_c& l_HIO_child(s32 i) { return *gabi::at<daNpc_Yw1_childHIO_c>(0x10468A30 + 0x38 * i); }
inline daNpc_Yw1_HIO_c& l_HIO_yw1() { return *gabi::at<daNpc_Yw1_HIO_c>(0x10468A24); }
