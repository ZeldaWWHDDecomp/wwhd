#pragma once
#include "gc_api/d/d_lib.h"
#include "f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
struct daObj_Pfall_c : fopAc_ac_c {
    gptr<void> field_0x290;
    cXyz field_0x294[10], field_0x30C[10], field_0x384[2];
    u8 field_0x39C[12];
    be<s32> field_0x3A8, mTimer, field_0x3B0;
    be<s16> field_0x3B4;
    u8 _4D2[2];
    be<s32> field_0x3B8;
    request_of_phase_process_class mPhsPfall;
    gptr<J3DModel> field_0x3C4;
    gptr<dBgW> mpBgW;
    Mtx34 field_0x3CC;
    gptr<J3DModel> field_0x3FC;
    gptr<dBgW> mpBgW2;
    Mtx34 field_0x404;
    gptr<J3DModel> mpHimoModel;
    be<f32> field_0x438;
    gptr<mDoExt_McaMorf> mpMorf;
    // HD: expanded line material and no GameCube alignment word.
    mDoExt_3DlineMat1_c field_0x444;
    be<s8> mBckIdx, field_0x481, mOldAnmPrmIdx;
    be<u8> field_0x483;
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
WWHD_OFFSET(daObj_Pfall_c, field_0x481, 0x6E5);
WWHD_SIZE(daObj_Pfall_c, 0x6E8);
// HD: this HIO keeps its data first and compiler vtable at +0x0C.
struct daObj_PfallHIO_c {
    be<s8> mNo;
    be<u8> field_0x05, field_0x06;
    u8 _3;
    be<f32> field_0x08;
    be<s16> field_0x0C;
    u8 _A[2];
    be<u32> mVtable;
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
WWHD_OFFSET(daObj_Pfall_c, field_0x3A8, 0x4C4);
WWHD_OFFSET(daObj_Pfall_c, mTimer, 0x4C8);
WWHD_OFFSET(daObj_Pfall_c, field_0x3B0, 0x4CC);
WWHD_OFFSET(daObj_Pfall_c, field_0x3B4, 0x4D0);
WWHD_OFFSET(daObj_Pfall_c, field_0x3B8, 0x4D4);
WWHD_OFFSET(daObj_Pfall_c, mPhsPfall, 0x4D8);
WWHD_OFFSET(daObj_Pfall_c, field_0x3C4, 0x4E0);
WWHD_OFFSET(daObj_Pfall_c, mpBgW, 0x4E4);
WWHD_OFFSET(daObj_Pfall_c, field_0x3CC, 0x4E8);
WWHD_OFFSET(daObj_Pfall_c, field_0x3FC, 0x518);
WWHD_OFFSET(daObj_Pfall_c, mpBgW2, 0x51C);
WWHD_OFFSET(daObj_Pfall_c, field_0x404, 0x520);
WWHD_OFFSET(daObj_Pfall_c, mpHimoModel, 0x550);
WWHD_OFFSET(daObj_Pfall_c, field_0x438, 0x554);
WWHD_OFFSET(daObj_Pfall_c, mpMorf, 0x558);
WWHD_OFFSET(daObj_Pfall_c, field_0x444, 0x55C);
WWHD_OFFSET(daObj_Pfall_c, mBckIdx, 0x6E4);
WWHD_OFFSET(daObj_Pfall_c, mOldAnmPrmIdx, 0x6E6);
WWHD_OFFSET(daObj_Pfall_c, field_0x483, 0x6E7);
