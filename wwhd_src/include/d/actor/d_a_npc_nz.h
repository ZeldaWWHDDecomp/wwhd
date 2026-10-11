/* daNpc_Nz_c (the rat shopkeeper), WWHD layout.
 *
 * GameCube -> WWHD: fopNpc_npc_c is 0x7DC (GameCube 0x6C4), so the members move by +0x118 up to
 * field_0x934; mDoExt_3DlineMat1_c grew from 0x3C to 0x188, so the tail arrays and the shop
 * item ids move by a further +0x14C. Size 0xCF8 (GameCube 0xA92). The GameCube vtable pointer of
 * fopNpc_npc_c is the HD C++ vtable at +0xB4 (daNpc_Nz_c: 0x1001F038). */
#pragma once
#include "gc_api/gc_api.h"

class daNpc_Nz_c : public fopNpc_npc_c {
public:
    enum Proc_e {
        PROC_INIT_e = 0,
        PROC_EXEC_e = 1,
    };
    typedef void (daNpc_Nz_c::*ProcFunc)();
    /* modeProc's table: {init, run, name} (GHS pointers to members, 8 bytes) */
    struct mode_entry_t {
        /* 0x00 */ gabi::ptmf<ProcFunc> init;
        /* 0x08 */ gabi::ptmf<ProcFunc> run;
        /* 0x10 */ gptr<const char> name;
    };
    /* cutProc's table: {start, proc} */
    struct cut_entry_t {
        /* 0x00 */ gabi::ptmf<ProcFunc> start;
        /* 0x08 */ gabi::ptmf<ProcFunc> proc;
    };
    enum : u32 { __vt = 0x1001F038 }; /* HD: daNpc_Nz_c's vtable */

    daNpc_Nz_c();

    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }

    BOOL NodeCallBack(J3DNode*, int);
    BOOL TailNodeCallBack(J3DNode*, int);
    void TailControl();
    BOOL _createHeap();
    s16 XyEventCB(int);
    void eventOrder();
    void checkOrder();
    void setAttention();
    void LookBack();
    void setAnm(s8, u8 /* GameCube bool: HD passes the byte on */);
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeEventEsaInit();
    void modeEventEsa();
    void modeProc(Proc_e, int);
    void createShopItem(u8, u8);
    daShopItem_c* getShopItem(int);
    void deleteShopItem();
    u16 next_msgStatus(be<u32>*);
    void anmAtr(u16);
    bool _execute();
    bool _draw();
    BOOL createInit();
    void setSmokeParticle();
    void getArg();
    cPhs_State _create();
    bool _delete();
    void* _searchEsa(fopAc_ac_c*);
    void cutProc();
    void cutEatesaStart();
    void cutEatesaProc();
    void cutEatesaFirstStart();
    void cutEatesaFirstProc();
    void cutKillAllStart();
    void cutKillAllProc();
    void cutShowStart();
    void cutShowProc();
    void cutHideStart();
    void cutHideProc();
    void cutSetAnmStart();
    void cutSetAnmProc();
    void cutGoHomeStart();
    void cutGoHomeProc();
    void cutChkGetItemStart();
    void cutChkGetItemProc();

public:
    static const char m_arc_name[];
    static const char m_bdl_arc_name[];

    /*         GC      */
    /* 0x7DC  0x6C4 */ cXyz mShopPos;              /* home position before the step back */
    /* 0x7E8  0x6D0 */ be<s32> mCurMode;
    /* 0x7EC  0x6D4 */ be<s8> mBckIdx;             /* dLib_setAnm state */
    /* 0x7ED  0x6D5 */ be<s8> mAnmPrmIdx;          /* the animation set by setAnm */
    /* 0x7EE  0x6D6 */ be<s8> mOldAnmPrmIdx;
    /* 0x7EF        */ u8 _7EF;
    /* 0x7F0  0x6D8 */ request_of_phase_process_class mPhs1;
    /* 0x7F8  0x6E0 */ request_of_phase_process_class mPhs2;
    /* 0x800  0x6E8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x804  0x6EC */ be<f32> mSpeedTarget;
    /* 0x808  0x6F0 */ dBgS_ObjAcch mAcch;
    /* 0x9CC  0x8B4 */ dBgS_AcchCir mAcchCir;
    /* 0xA0C  0x8F4 */ be<s32> mStaffId;
    /* 0xA10  0x8F8 */ be<u8> mTalkMode;           /* 1: order a talk, 2: can talk, 3: item bought */
    /* 0xA11        */ u8 _A11;
    /* 0xA12  0x8FA */ be<s16> mEventIdx;
    /* 0xA14  0x8FC */ be<u8> mIsShow;             /* bool */
    /* 0xA15        */ u8 _A15;
    /* 0xA16  0x8FE */ be<s16> field_0xA16;        /* unused */
    /* 0xA18  0x900 */ be<u32> mItemListMsgNo;     /* the message listing the two items */
    /* 0xA1C  0x904 */ be<s16> mItemListIdx;       /* which pair of items (0..3) */
    /* 0xA1E  0x906 */ be<s16> mLoopCount;         /* animation loops / eating timer */
    /* 0xA20  0x908 */ be<u8> mSelectItemNo;
    /* 0xA21  0x909 */ be<u8> mSelectItemIdx;
    /* 0xA22  0x90A */ be<s16> field_0xA22;        /* unused */
    /* 0xA24  0x90C */ gptr<fopAc_ac_c> mpBait;
    /* 0xA28  0x910 */ be<f32> mBaitDist;
    /* 0xA2C  0x914 */ dPa_smokeEcallBack mSmokeCB;
    /* 0xA4C  0x934 */ mDoExt_3DlineMat1_c mTailLine;  /* HD 0x188 (GameCube 0x3C) */
    /* 0xBD4  0x970 */ be<u32> field_0xBD4;        /* unused */
    /* 0xBD8  0x974 */ cXyz mTailPos[10];
    /* 0xC50  0x9EC */ cXyz mTailVel[10];
    /* 0xCC8  0xA64 */ cXyz mTailRootPos[2];       /* hip and tail-root joints (TailNodeCallBack) */
    /* 0xCE0  0xA7C */ u8 _CE0[0xC];
    /* 0xCEC  0xA88 */ be<u32> mShopItemId[2];     /* fpc_ProcID */
    /* 0xCF4  0xA90 */ be<s16> mItemScaleTimer;
    /* 0xCF6        */ u8 _CF6[2];
};
WWHD_OFFSET(daNpc_Nz_c, mShopPos, 0x7DC);
WWHD_OFFSET(daNpc_Nz_c, mCurMode, 0x7E8);
WWHD_OFFSET(daNpc_Nz_c, mBckIdx, 0x7EC);
WWHD_OFFSET(daNpc_Nz_c, mPhs1, 0x7F0);
WWHD_OFFSET(daNpc_Nz_c, mpMorf, 0x800);
WWHD_OFFSET(daNpc_Nz_c, mSpeedTarget, 0x804);
WWHD_OFFSET(daNpc_Nz_c, mAcch, 0x808);
WWHD_OFFSET(daNpc_Nz_c, mAcchCir, 0x9CC);
WWHD_OFFSET(daNpc_Nz_c, mStaffId, 0xA0C);
WWHD_OFFSET(daNpc_Nz_c, mTalkMode, 0xA10);
WWHD_OFFSET(daNpc_Nz_c, mEventIdx, 0xA12);
WWHD_OFFSET(daNpc_Nz_c, mIsShow, 0xA14);
WWHD_OFFSET(daNpc_Nz_c, mItemListMsgNo, 0xA18);
WWHD_OFFSET(daNpc_Nz_c, mItemListIdx, 0xA1C);
WWHD_OFFSET(daNpc_Nz_c, mLoopCount, 0xA1E);
WWHD_OFFSET(daNpc_Nz_c, mSelectItemNo, 0xA20);
WWHD_OFFSET(daNpc_Nz_c, mSelectItemIdx, 0xA21);
WWHD_OFFSET(daNpc_Nz_c, mpBait, 0xA24);
WWHD_OFFSET(daNpc_Nz_c, mBaitDist, 0xA28);
WWHD_OFFSET(daNpc_Nz_c, mSmokeCB, 0xA2C);
WWHD_OFFSET(daNpc_Nz_c, mTailLine, 0xA4C);
WWHD_OFFSET(daNpc_Nz_c, mTailPos, 0xBD8);
WWHD_OFFSET(daNpc_Nz_c, mTailVel, 0xC50);
WWHD_OFFSET(daNpc_Nz_c, mTailRootPos, 0xCC8);
WWHD_OFFSET(daNpc_Nz_c, mShopItemId, 0xCEC);
WWHD_OFFSET(daNpc_Nz_c, mItemScaleTimer, 0xCF4);
WWHD_SIZE(daNpc_Nz_c, 0xCF8); /* profile size 0xCF8 */
WWHD_SIZE(daNpc_Nz_c::mode_entry_t, 0x14);
WWHD_SIZE(daNpc_Nz_c::cut_entry_t, 0x10);
