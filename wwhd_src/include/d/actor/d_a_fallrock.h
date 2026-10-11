#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
void mDoExt_modelUpdate(J3DModel *);
void fopKyM_createMpillar(const cXyz *, f32);
void mDoMtx_XYZrotM(Mtx34 *, s16, s16, s16);
// HD: allocating constructor has an extra argument before the lighting structure.
void *FallRock_newModelEmitter(void *, JPABaseEmitter *, J3DModelData *, u32, dKy_tevstr_c *, void *, u16,
                               s32);
struct dBgS_ObjGndChk_Yogan : dBgS_GndChk {};
inline void gabi_construct(dBgS_ObjGndChk_Yogan &c) {
    dBgS_GndChk_ct(&c, gc_tu.gndChk, true);
    c.mGrp = 4;
}
inline void gabi_destroy(dBgS_ObjGndChk_Yogan &c) {
    gabi_destroy((dBgS_GndChk &)c);
}
struct daFallRock_c : fopAc_ac_c {
    cPhs_State create();
    static constexpr f32 m_falllen = 7000.0f;
    static constexpr s16 m_rot_speed = 0x3E8;
    BOOL execute();
    BOOL CreateHeap();
    void set_mtx();
    void setParticle(int, cXyz *);

    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
    /* 0x3B8 */ dBgS_ObjAcch field_0x29C;
    /* 0x57C */ dBgS_AcchCir field_0x460;
    /* 0x5BC */ dCcD_Stts mStts;
    /* 0x5F8 */ dCcD_Cyl mCyl;
    /* 0x728 */ GXColor field_0x60C;
    /* 0x72C */ be<f32> field_0x610;
    /* 0x730 */ be<s32> field_0x614;
};
WWHD_OFFSET(daFallRock_c, field_0x29C, 0x3B8);
WWHD_OFFSET(daFallRock_c, mCyl, 0x5F8);
WWHD_OFFSET(daFallRock_c, field_0x614, 0x730);
WWHD_SIZE(daFallRock_c, 0x734);
