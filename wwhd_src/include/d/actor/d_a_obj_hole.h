/* Grotto entrance, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
enum { dRes_INDEX_AANA_BDL_AANA_e = 3 };
BOOL dLib_checkPlayerInCircle(cXyz*, f32, f32);
void dLib_setNextStageBySclsNum(u8, s8);
void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16);
void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
// HD: the named-event end check is an older manager entry point.
inline BOOL hole_eventEndCheck(const char* name) {
    return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name);
}
struct HoleCamera_view { u8 rest[0x234]; csXyz mAngle; };
inline s16 fopCamM_GetAngleY(camera_class* camera) { return ((HoleCamera_view*)camera)->mAngle.y; }
// HD: inline destruction stores this TU's base-class vtables in this order.
inline void __dt(dBgS_ObjLinChk* p) {
    p->dBgS_PolyPassChk::__vtbl = 0x1002A8B8;
    p->dBgS_GrpPassChk::__vtbl = 0x1002A848;
    p->cBgS_PolyInfo::__vtbl = 0x1002A838;
    gabi::call(0x02008B4C, p, 0);
}
struct daObj_Hole_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl;
};

struct daObj_Hole_c : fopAc_ac_c {
    enum Mode_e { MODE_WAIT, MODE_EVENT, MODE_NULL };
    enum Proc_e { PROC_INIT_e, PROC_EXEC_e };
    daObj_Hole_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002A908;
        dBgS_LinChk_ct(&mLinChk, {0x1002A8C8, 0x1002A8D8, 0x1002A8F8, 0x1002A8E8}, true);
        __ct(&mAcch);
        __ct(&mAcchCir);
    }
    static const char m_arc_name[];
    void setMtx();
    void getPosAndAngle();
    void modeWaitInit();
    void modeWait();
    void modeEventInit();
    void modeEvent();
    void modeProc(s32 proc, s32 newMode);
    bool _execute();
    void debugDraw();
    bool _draw();
    void createInit();
    BOOL _createHeap();
    void getArg();
    cPhs_State _create();
    bool _delete();

    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ be<u8> mHasModel;
    /* 0x3B1 */ be<u8> mExitIdx;
    /* 0x3B2 */ be<u16> mScaleLocal;
    /* 0x3B4 */ request_of_phase_process_class mPhs;
    /* 0x3BC */ gptr<J3DModel> mpMdl;
    /* 0x3C0 */ dBgS_ObjLinChk mLinChk;
    /* 0x42C */ dBgS_ObjAcch mAcch;
    /* 0x5F0 */ dBgS_AcchCir mAcchCir;
};
WWHD_OFFSET(daObj_Hole_c, mAcch, 0x42C);
WWHD_SIZE(daObj_Hole_c, 0x630);

