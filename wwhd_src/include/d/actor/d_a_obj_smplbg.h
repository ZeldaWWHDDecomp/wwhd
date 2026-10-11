// Tingle Tower top: WWHD MoveBgActor layout.
#pragma once
#include "gc_api/gc_api.h"
namespace daObjSmplbg {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c() { dBgS_MoveBgActor::ct(this); __vtbl = 0x1002F664; } // HD: constructor

    enum Prm_e { PRM_TYPE_W = 8, PRM_TYPE_S = 0 };
    s32 prm_get_type();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void exec_qtkhd();
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mType;
    /* 0x3F0 */ be<u8> mIsStop;
    /* 0x3F1 */ u8 _3F1[3];
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mIsStop, 0x3F0);
}  // namespace daObjSmplbg

enum { dRes_INDEX_QTKHD_BDL_QTKHD_e=4, dRes_INDEX_QTKHD_DZB_QTKHD_e=7, JA_SE_OBJ_TC_TOWER_ROUND=0x619A };
void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32);
s32 Smplbg_PrmAbstract(fopAc_ac_c*, s32, s32);

// HD: MoveBGCreate accepts a guest callback address.
constexpr u32 dBgS_MoveBGProc_TypicalRotY = 0x024EE708;
