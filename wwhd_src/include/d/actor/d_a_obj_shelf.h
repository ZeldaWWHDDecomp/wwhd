/** Wooden shelf, WWHD layout.
 * HD: the moving-background actor base shifts tww members by 0x118. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjShelf {
struct Act_c : dBgS_MoveBgActor {
    enum Mode_e { Mode_WAIT_e, Mode_VIB_e, Mode_ROT_e, Mode_FELL_e };
    enum Prm_e { PRM_GROUNDMA_W=1, PRM_GROUNDMA_S=0 };
    request_of_phase_process_class mPhs; // 0x3E0
    gptr<J3DModel> mpModel;              // 0x3E8
    /* 0x3EC */ be<s32> mMode;
    /* 0x3F0 */ be<f32> mRotSpeed;
    /* 0x3F4 */ be<s16> mTargetAngle;
    /* 0x3F6 */ be<s16> mTimer;
    /* 0x3F8 */ be<s16> mVibY;
    /* 0x3FA */ be<s16> mVibX;
    /* 0x3FC */ be<s16> mVibZ;
    /* 0x3FE */ be<s8> mCurBounce;
    /* 0x3FF */ be<u8> m2e7;
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void hold_event();
    void mode_wait_init();
    void mode_wait();
    void mode_vib_init();
    void mode_vib();
    void mode_rot_init();
    void mode_rot_init2();
    void mode_rot_init3();
    void mode_rot();
    void mode_fell_init();
    void mode_fell();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>*);
    BOOL Draw();
};
WWHD_SIZE(Act_c, 0x400);
WWHD_OFFSET(Act_c, mPhs, 0x3E0);
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mTargetAngle, 0x3F4);
WWHD_OFFSET(Act_c, mVibY, 0x3F8);
WWHD_OFFSET(Act_c, mVibX, 0x3FA);
WWHD_OFFSET(Act_c, mVibZ, 0x3FC);
WWHD_OFFSET(Act_c, mCurBounce, 0x3FE);
WWHD_OFFSET(Act_c, mMode, 0x3EC);
WWHD_OFFSET(Act_c, mRotSpeed, 0x3F0);
WWHD_OFFSET(Act_c, mTimer, 0x3F6);
WWHD_OFFSET(Act_c, m2e7, 0x3FF);
}

namespace daObjShelf {
using ShelfModeFunc = void (Act_c::*)();
struct ShelfModeEntry {
    gabi::ptmf<ShelfModeFunc> proc;
};
WWHD_SIZE(ShelfModeEntry, 8);
// HD: this actor only accesses Granny's hold-event byte.
struct GrannyEventView : fopAc_ac_c {
    u8 padHold[0x969 - 0x3AC];
    be<u8> mbHoldEvent;
    void holdEventOn() { mbHoldEvent = 1; }
};
WWHD_OFFSET(GrannyEventView, mbHoldEvent, 0x969);
WWHD_SIZE(GrannyEventView, 0x96A);
}
enum { dRes_INDEX_OTANA_BDL_OTANA_e=4, dRes_INDEX_OTANA_DZB_OTANA_e=7 };
enum { fpcNm_NPC_BA1_e=0x14F, GrannyRecoveredEvent=1, JA_SE_OBJ_RACK_BREAK=0x696D };
