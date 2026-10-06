/* daNpc_Os_c (the Great Fish Isle / Earth & Wind temple "Os" stone-head helper), WWHD layout.
 *
 * GameCube -> WWHD (size 0x80C -> 0x97C), measured from create 022AAEE0 (inlined constructor):
 * - daPy_npc_c base: HD 0x608 (daPy_npc_l from d_a_npc_md.h): mPhs/mpMorf/mpPedestal +0x11C;
 * - mDoExt_brkAnm grew from 0x18 to 0x78 and mShadowId (GameCube 0x514) is gone: +0x178 from mAcchCir;
 * - the pointers to member are 8 bytes (GHS) instead of 12: +0x170 from field_0x780.
 * Field names are the GameCube ones. Parts: d_a_npc_os.cpp, d_a_npc_os_a/b/c.cpp; a part adds a member
 * declaration here only with a small one-line replace. */
#pragma once
#include "d/actor/d_a_npc_md.h" /* daPy_npc_l, ProcFunc_l, cBgS_PolyInfo_l */

#define OS_VTBL 0x1001F718 /* daNpc_Os_c vtable (create 022AAEE0, stored at +0xB4) */

/* daNpc_Os_infiniteEcallBack_c (dPa_levelEcallBack: vtable, emitter) */
struct daNpc_Os_infiniteEcallBack_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ gptr<JPABaseEmitter> mpBaseEmitter;
};
WWHD_SIZE(daNpc_Os_infiniteEcallBack_l, 8);

struct mDoExt_brkAnm_os {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x78 - 0x10];
};
WWHD_SIZE(mDoExt_brkAnm_os, 0x78);

struct daNpc_Os_c : daPy_npc_l {
    cPhs_State create();
    BOOL createHeap();
    BOOL jointCheck(s8);
    BOOL wakeupCheck();
    void setWakeup();
    BOOL finishCheck();
    void setFinish();
    s8 getWakeupOrderEventNum();
    s8 getFinishOrderEventNum();
    int getMyStaffId();
    s8 getRestartNumber();
    BOOL checkGoalRoom();
    void checkPlayerRoom();
    void eventOrderCheck();
    void makeBeam(int);
    void endBeam();
    s32 wallHitCheck();
    u32 walkProc(f32, s16);
    BOOL setAction(ProcFunc_l*, ProcFunc_l*, void*);
    void npcAction(void*);
    void setNpcAction(ProcFunc_l*, void*);
    void playerAction(void*);
    void setPlayerAction(ProcFunc_l*, void*);
    s16 getStickAngY();
    int calcStickPos(s16, cXyz*);
    void returnLinkPlayer();
    BOOL returnLinkCheck();
    BOOL waitNpcAction(void*);
    BOOL finish01NpcAction(void*);
    BOOL finish02NpcAction(void*);
    BOOL talkNpcAction(void*);
    BOOL carryNpcAction(void*);
    BOOL throwNpcAction(void*);
    BOOL jumpNpcAction(void*);
    void routeAngCheck(cXyz*, be<s16>*); /* GameCube cXyz& */
    void routeWallCheck(cXyz*, cXyz*, be<s16>*); /* GameCube cXyz& */
    f32 checkForwardGroundY(s16);
    f32 checkWallJump(s16);
    BOOL routeCheck(f32, be<s16>*);
    BOOL searchNpcAction(void*);
    BOOL waitPlayerAction(void*);
    BOOL walkPlayerAction(void*);
    BOOL eventProc();
    void initialDefault(int);
    BOOL actionDefault(int);
    void initialWaitEvent(int);
    BOOL actionWaitEvent(int);
    void initialWakeupEvent(int);
    BOOL actionWakeupEvent(int);
    void initialMoveEvent(int);
    BOOL actionMoveEvent(int);
    void initialMoveEndEvent(int);
    void initialEndEvent(int);
    void initialTurnEvent(int);
    BOOL actionTurnEvent(int);
    void initialFinishEvent(int);
    BOOL actionFinishEvent(int);
    void initialMsgSetEvent(int);
    BOOL actionMsgSetEvent(int);
    BOOL actionMsgEndEvent(int);
    void initialSwitchOnEvent(int);
    void initialNextEvent(int);
    void initialSaveEvent(int);
    BOOL talk_init();
    BOOL talk();
    void setAnm(int);
    BOOL dNpc_Os_setAnm(mDoExt_McaMorf*, int, f32, f32, int, const char*);
    BOOL initBrkAnm(u8, bool);
    void playBrkAnm();
    void setAnm_brkAnm(int);
    BOOL chkAttention(cXyz*, s16);
    bool chkArea(cXyz*);
    void carryCheck();
    void eventOrder();
    void checkOrder();
    BOOL checkCommandTalk();
    u16 next_msgStatus(be<u32>* pMsgNo);
    u32 getMsg();
    void setCollision();
    void setAttention(bool);
    void lookBack(int, int, int);
    void setBaseMtx();
    BOOL init();
    BOOL draw();
    void animationPlay();
    void smokeSet(u16);
    BOOL execute();


    /* 0x608 */ request_of_phase_process_class mPhs;     /* GameCube 0x4EC */
    /* 0x610 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x614 */ gptr<fopAc_ac_c> mpPedestal;
    /* 0x618 */ mDoExt_brkAnm_os mBrkAnm;                 /* GameCube 0x4FC (0x18) */
    /* HD: no mShadowId (GameCube 0x514) */
    /* 0x690 */ dBgS_AcchCir mAcchCir[2];                 /* GameCube 0x518 */
    /* 0x710 */ dCcD_Stts mStts;
    /* 0x74C */ dCcD_Cyl mCyl;
    /* 0x87C */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x8B0 */ daNpc_Os_infiniteEcallBack_l field_0x738;
    /* 0x8B8 */ daNpc_Os_infiniteEcallBack_l field_0x740;
    /* 0x8C0 */ cXyz field_0x748;
    /* 0x8CC */ cXyz field_0x754;
    /* 0x8D8 */ be<f32> mPrevMorfFrame;
    /* 0x8DC */ be<f32> field_0x764;
    /* 0x8E0 */ ProcFunc_l mPlayerAction;                 /* GameCube 0x768 (12 bytes) */
    /* 0x8E8 */ ProcFunc_l mNpcAction;                    /* GameCube 0x774 */
    /* 0x8F0 */ be<u32> field_0x780;
    /* 0x8F4 */ be<u32> field_0x784;
    /* 0x8F8 */ be<f32> field_0x788;
    /* 0x8FC */ be<s32> field_0x78C;
    /* 0x900 */ u8 field_0x790[4];
    /* 0x904 */ be<u32> field_0x794;
    /* 0x908 */ be<s16> field_0x798;
    /* 0x90A */ u8 field_0x79A[2];
    /* 0x90C */ be<s8> mTuno1JointIdx;
    /* 0x90D */ be<s8> mTuno2JointIdx;
    /* 0x90E */ be<s8> mTuno3JointIdx;
    /* 0x90F */ be<s8> mReachedAnimEnd;
    /* 0x910 */ be<s8> field_0x7A0;
    /* 0x911 */ be<s8> field_0x7A1;
    /* 0x912 */ be<s8> field_0x7A2;
    /* 0x913 */ be<u8> field_0x7A3;
    /* 0x914 */ be<u8> field_0x7A4;
    /* 0x915 */ be<s8> field_0x7A5;
    /* 0x916 */ be<s8> field_0x7A6;
    /* 0x917 */ be<u8> field_0x7A7;
    /* 0x918 */ be<u8> field_0x7A8;
    /* 0x919 */ be<s8> field_0x7A9;
    /* 0x91A */ be<s8> field_0x7AA;
    /* 0x91B */ be<u8> field_0x7AB;
    /* 0x91C */ be<s16> field_0x7AC;
    /* 0x91E */ be<s16> field_0x7AE;
    /* 0x920 */ be<s16> field_0x7B0;
    /* 0x922 */ be<s16> field_0x7B2;
    /* 0x924 */ be<s16> field_0x7B4;
    /* 0x926 */ be<s16> field_0x7B6;
    /* 0x928 */ be<f32> field_0x7B8;
    /* 0x92C */ be<s16> field_0x7BC;
    /* 0x92E */ be<s16> field_0x7BE;
    /* 0x930 */ be<s16> field_0x7C0;
    /* 0x932 */ be<s16> field_0x7C2;
    /* 0x934 */ be<s16> field_0x7C4[0x10];
    /* 0x954 */ cXyz field_0x7E4;
    /* 0x960 */ cXyz field_0x7F0;
    /* 0x96C */ cBgS_PolyInfo_l field_0x7FC;
};
WWHD_OFFSET(daNpc_Os_c, mPhs, 0x608);
WWHD_OFFSET(daNpc_Os_c, mBrkAnm, 0x618);
WWHD_OFFSET(daNpc_Os_c, mAcchCir, 0x690);
WWHD_OFFSET(daNpc_Os_c, mStts, 0x710);
WWHD_OFFSET(daNpc_Os_c, mCyl, 0x74C);
WWHD_OFFSET(daNpc_Os_c, mJntCtrl, 0x87C);
WWHD_OFFSET(daNpc_Os_c, field_0x738, 0x8B0);
WWHD_OFFSET(daNpc_Os_c, field_0x748, 0x8C0);
WWHD_OFFSET(daNpc_Os_c, mPlayerAction, 0x8E0);
WWHD_OFFSET(daNpc_Os_c, field_0x780, 0x8F0);
WWHD_OFFSET(daNpc_Os_c, mTuno1JointIdx, 0x90C);
WWHD_OFFSET(daNpc_Os_c, field_0x7C4, 0x934);
WWHD_OFFSET(daNpc_Os_c, field_0x7FC, 0x96C);
WWHD_SIZE(daNpc_Os_c, 0x97C);
