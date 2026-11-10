#pragma once
#include "gc_api/d/d_lib.h"
#include "f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
struct daObj_Pfall_c : fopAc_ac_c {
    /* 0x3AC */ gptr<void> field_0x290;
    /* 0x3B0 */ cXyz field_0x294[10];
    /* 0x428 */ cXyz field_0x30C[10];
    /* 0x4A0 */ cXyz field_0x384[2];
    /* 0x4B8 */ u8 field_0x39C[12];
    /* 0x4C4 */ be<s32> mCurMode;  /* was field_0x3A8: mode_wait_init 023830CC MODE_WAIT, mode_event_init 02383D18 MODE_EVENT; mode_proc_call 02383748 calls mode_proc[it] (tww (this->*mode_proc[it])()); after tww daObj_Canon_c / daObj_Ikada_c mCurMode */
    /* 0x4C8 */ be<s32> mTimer;
    /* 0x4CC */ be<s32> mOpenStep;  /* was field_0x3B0: cutOpenStart 02383310 0; cutOpenProc 02383404 steps it through the seven target angles of mFloorAngle and ends the cut at 7 (tww) */
    /* 0x4D0 */ be<s16> mFloorAngle;  /* was field_0x3B4: set_mtx 0238299C turns both floor models by it (tww mDoMtx_stack_c::ZrotM(it)); cutOpenStart 02383310 0 (with the jail-floor-break sound); cutOpenProc 02383404 eases it to the table angles (tww cLib_addCalcAngleS(&it, temp[mOpenStep], ..)) */
    /* 0x4D2 */ u8 _4D2[2];
    /* 0x4D4 */ be<s32> mWaitTimer;  /* was field_0x3B8: mode_wait_init 023830CC l_HIO.mWaitTime; mode_wait 02383D24 starts the event once the player stands still and cLib_calcTimer(&it) reaches 0 (tww) */
    /* 0x4D8 */ request_of_phase_process_class mPhsPfall;
    /* 0x4E0 */ gptr<J3DModel> mpModel;  /* was field_0x3C4: CreateHeap 02382E64: the first floor model (tww it = mDoExt_J3DModel__create(..)); set_mtx 0238299C gives it the mBgMtx placement; _draw 02383C24 draws it; pairs with mpBgW */
    /* 0x4E4 */ gptr<dBgW> mpBgW;
    /* 0x4E8 */ Mtx34 mBgMtx;  /* was field_0x3CC: set_mtx 0238299C: copy of the first floor model's matrix (tww cMtx_copy(.., it)); CreateHeap 02382E64 sets mpBgW up with it (tww mpBgW->Set(.., &it)) */
    /* 0x518 */ gptr<J3DModel> mpModel2;  /* was field_0x3FC: CreateHeap 02382E64: the second floor model from the same model data (tww); set_mtx 0238299C gives it the mBgMtx2 placement; _draw 02383C24 draws it; pairs with mpBgW2 */
    /* 0x51C */ gptr<dBgW> mpBgW2;
    /* 0x520 */ Mtx34 mBgMtx2;  /* was field_0x404: set_mtx 0238299C: copy of the second floor model's matrix (tww); CreateHeap 02382E64 sets mpBgW2 up with it (tww mpBgW2->Set(.., &it)) */
    /* 0x550 */ gptr<J3DModel> mpHimoModel;
    /* 0x554 */ be<f32> mHimoY;  /* was field_0x438: set_mtx 0238299C: the rope model mpHimoModel is moved up by it + l_HIO.mHimoOffsetY (tww transM(0, it + l_HIO.field_0x08, 0)); _execute 023837E4 raises it during the pull animation and lowers it again, 0 otherwise (tww) */
    /* 0x558 */ gptr<mDoExt_McaMorf> mpMorf;
    // HD: expanded line material and no GameCube alignment word.
    /* 0x55C */ mDoExt_3DlineMat1_c field_0x444;
    /* 0x6E4 */ be<s8> mBckIdx;
    /* 0x6E5 */ be<s8> mAnmPrmIdx;  /* was field_0x481: setAnm 02383794 passes it as dLib_bcks_setAnm's pPrmIdx, next to mBckIdx / mOldAnmPrmIdx (tww); CreateInit 023830E4 ANM_WAIT, cutHikuStart 0238336C ANM_HIKU, cutHikuProc 02383510 back to ANM_WAIT (tww) */
    /* 0x6E6 */ be<s8> mOldAnmPrmIdx;
    /* 0x6E7 */ be<u8> mLaughSePlayed;  /* was field_0x483: cutWaitProc 02383378: when the cut timer runs out and it is 0, set 1 and start the rat laugh once (tww fopAcM_monsSeStart(this, JA_SE_CV_NZ_LAUGH, 0)) */
    void set_mtx();
    BOOL CreateHeap();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    void mode_wait_init();
    void cutWaitStart(s32);
    void cutOpenStart(s32);
    void cutHikuStart(s32);
    void cutWaitProc(s32);
    void cutOpenProc(s32);
    void cutHikuProc(s32);
    void cutProc();
    void mode_proc_call();
    void setAnm();
    BOOL _execute();
    void nz_draw();
    BOOL _draw();
    void mode_event_init();
    void mode_wait();
    void mode_event();
};
WWHD_OFFSET(daObj_Pfall_c, mAnmPrmIdx, 0x6E5);
WWHD_SIZE(daObj_Pfall_c, 0x6E8);
// HD: this HIO keeps its data first and compiler vtable at +0x0C.
struct daObj_PfallHIO_c {
    /* 0x000 */ be<s8> mNo;
    /* 0x001 */ be<u8> mDebugInit;  /* was field_0x05: _draw 02383C24 runs the one-time debug setup (pfallDebugInit) while it is set; pfallHioConstruct 02383EB0 0 (tww false) */
    /* 0x002 */ be<u8> mNoWarp;  /* was field_0x06: mode_event 02383DC4: when the NZFALL event ends, set goes back to mode_wait_init, clear warps to the sea stage (tww if (l_HIO.it != 0)); pfallHioConstruct 02383EB0 0 */
    /* 0x003 */ u8 _3;
    /* 0x004 */ be<f32> mHimoOffsetY;  /* was field_0x08: set_mtx 0238299C adds it to mHimoY for the rope model height (tww transM(0, field_0x438 + l_HIO.it, 0)); pfallHioConstruct 02383EB0 3 (tww 3.0f) */
    /* 0x008 */ be<s16> mWaitTime;  /* was field_0x0C: mode_wait_init 023830CC: mWaitTimer = it (tww field_0x3B8 = l_HIO.it); pfallHioConstruct 02383EB0 0 */
    /* 0x00A */ u8 _A[2];
    /* 0x00C */ be<u32> mVtable;
};
WWHD_SIZE(daObj_PfallHIO_c, 0x10);

// HD: compiler-generated class vtables.
enum : u32 { PFALL_VTABLE = 0x1002E674, PFALL_HIO_VTABLE = 0x1002E684 };
struct PfallTimerData {
    be<u16> high;
    be<s16> low;
};
struct PfallPositionBits {
    struct Vector {
        be<u32> x, y, z;
    };
    u8 actorPrefix[0x314];
    Vector current;
    u8 actorMiddle[0x37C - 0x320];
    Vector eye;
    u8 attentionFlags[8];
    Vector attention;
};
WWHD_OFFSET(PfallPositionBits, eye, 0x37C);
WWHD_OFFSET(PfallPositionBits, attention, 0x390);
struct PfallResourceHeader {
    u8 header[0x18];
    be<u32> materialNames;
};
void pfallJointEntry(J3DModel *, J3DJoint *);
void *pfallDebugInit(be<u8> *, const char *, u32);
void pfallRegister(be<u32> *);
void pfallInitChar(be<u8> *);
void pfallInitByte(be<u8> *);
s32 pfallGetReverb(s32);
void pfallCutEnd(dEvent_manager_c *, s32);
PfallTimerData *pfallGetInteger(dEvent_manager_c *, s32, const char *, s32);
s32 pfallCalcTimer(be<s32> *);
void pfallMonsterSound(u32, cXyz *, u32, u32, s32);
s32 pfallStaffId(dEvent_manager_c *, const char *, u32, u32);
s32 pfallActIdx(dEvent_manager_c *, s32, gptr<char> *, s32, u32, u32);
s32 pfallIsAdvance(dEvent_manager_c *, s32);
void pfallSetAnm(const char *, mDoExt_McaMorf *, be<s8> *, be<s8> *, be<s8> *, be<s32> *,
                 dLib_anm_prm_c *, u32);
s32 pfallCheckPlayer(cXyz *, f32, f32);
s32 pfallEndCheck(dEvent_manager_c *, const char *);
void pfallNextStage(const char *, s32, s32, s32, f32, u32, u32, u32);
void pfallOrderEvent(fopAc_ac_c *, const char *, u32, u32);
void *pfallAllocate(u32);
void pfallFree(void *);
void pfallDestroyActor(fopAc_ac_c *, s32);
dBgW *pfallNewBackground(dBgW *);
s32 pfallSetBackground(dBgW *, cBgD_t *, u32, Mtx34 *);

using PfallActionName = gptr<char>;
using PfallModeFunc = gabi::ptmf<void (daObj_Pfall_c::*)()>;

// GameCube member names, at the verified HD offsets.
WWHD_OFFSET(daObj_Pfall_c, field_0x290, 0x3AC);
WWHD_OFFSET(daObj_Pfall_c, mCurMode, 0x4C4);
WWHD_OFFSET(daObj_Pfall_c, mTimer, 0x4C8);
WWHD_OFFSET(daObj_Pfall_c, mOpenStep, 0x4CC);
WWHD_OFFSET(daObj_Pfall_c, mFloorAngle, 0x4D0);
WWHD_OFFSET(daObj_Pfall_c, mWaitTimer, 0x4D4);
WWHD_OFFSET(daObj_Pfall_c, mPhsPfall, 0x4D8);
WWHD_OFFSET(daObj_Pfall_c, mpModel, 0x4E0);
WWHD_OFFSET(daObj_Pfall_c, mpBgW, 0x4E4);
WWHD_OFFSET(daObj_Pfall_c, mBgMtx, 0x4E8);
WWHD_OFFSET(daObj_Pfall_c, mpModel2, 0x518);
WWHD_OFFSET(daObj_Pfall_c, mpBgW2, 0x51C);
WWHD_OFFSET(daObj_Pfall_c, mBgMtx2, 0x520);
WWHD_OFFSET(daObj_Pfall_c, mpHimoModel, 0x550);
WWHD_OFFSET(daObj_Pfall_c, mHimoY, 0x554);
WWHD_OFFSET(daObj_Pfall_c, mpMorf, 0x558);
WWHD_OFFSET(daObj_Pfall_c, field_0x444, 0x55C);
WWHD_OFFSET(daObj_Pfall_c, mBckIdx, 0x6E4);
WWHD_OFFSET(daObj_Pfall_c, mOldAnmPrmIdx, 0x6E6);
WWHD_OFFSET(daObj_Pfall_c, mLaughSePlayed, 0x6E7);
