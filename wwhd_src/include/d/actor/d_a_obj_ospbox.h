/* WWHD surprise box. */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JPAEmitter.h"
#include "gc_api/d/d_bg_s.h"
#include "gc_api/f_op/f_op_actor_mng.h"

void ospbox_deleteRes(const char *);
u32 ospbox_create(s16, u32, cXyz *, s8, csXyz *, cXyz *, s32, void *);
void ospbox_MoveCAtTg(dCcD_Cyl *, cXyz *);
namespace daObjOspbox {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_ITEMNO_W = 6, PRM_ITEMNO_S = 0, PRM_SPEC_W = 3, PRM_SPEC_S = 8 };
    Act_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1002E2C4;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1002E100);
        dBgS_GndChk_ct(&mObjGndChk, gc_tu.gndChk, true);
    }
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    dBgS_ObjGndChk mObjGndChk;
    // GameCube field names retained: ground height, retries, then lock timer.
    be<f32> m494;
    be<s16> m498;
    be<u8> m49A;
    u8 _pad;
    s32 Mthd_Create();
    BOOL Mthd_Delete();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Delete();
    void set_mtx();
    void init_mtx();
    void set_ground();
    void init_ground();
    void make_item();
    void eff_break();
    void sound_break();
    BOOL Execute(gptr<Mtx34> *);
    BOOL Draw();
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mObjGndChk, 0x558);
WWHD_OFFSET(Act_c, m494, 0x5AC);
WWHD_SIZE(Act_c, 0x5B4);
} // namespace daObjOspbox
