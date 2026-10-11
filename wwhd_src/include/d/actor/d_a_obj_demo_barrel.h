/* Demo barrel, WWHD layout (actor fields shift by 0x11C). */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local bindings; resource and collection accessors use the shared d_com_inf_game API. */
void dComIfGs_setSelectEquip(s32, u8); // HD: existing external binding, absent from the shared d_com_inf_game API.
struct dDemo_actor_c;
dDemo_actor_c* dDemo_object_getActor(u32, u8);
BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8);
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
f32 dBgS_ObjGndChk_Wtr_Func(cXyz*);

/* Unit-local view of JPABaseEmitter's scale, beyond the current shared layout. */
struct DemoBarrelEmitter {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ cXyz mEmitterScl;
    void setEmitterScale(const cXyz& scale) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        mEmitterScl.y = y;
        mEmitterScl.x = x;
        mEmitterScl.z = z;
    }
};
WWHD_OFFSET(DemoBarrelEmitter, mEmitterScl, 8);
struct dDemo_prm_c {
    be<u32> mId;
    u32 getId() { return mId; }
};
struct dDemo_actor_c {
    enum { ENABLE_UNK_e = 1, ENABLE_TRANS_e = 2, ENABLE_ROTATE_e = 8,
           ENABLE_ANM_e = 0x20, ENABLE_ANM_FRAME_e = 0x40 };
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mEnable;
    /* 0x06 */ u8 _06[0x4C - 6];
    /* 0x4C */ dDemo_prm_c mPrm;
    bool checkEnable(u16 flags) { return (mEnable & flags) != 0; }
    dDemo_prm_c* getPrm() { return &mPrm; }
};
WWHD_OFFSET(dDemo_actor_c, mPrm, 0x4C);
struct dDemo_object_c {
    dDemo_actor_c* getActor(u8 id) { return dDemo_object_getActor(gabi::ea(this), id); }
};
namespace dPa_name {
enum {
    ID_IT_JN_TR_HAHEN_A = 0x3E5,
    ID_IT_JN_WP_HAMON01 = 0x003D,
    ID_IT_JN_WP_HAMON02 = 0x003E,
    ID_IT_SN_DEMO_HAMON_S = 0x80CB,
    ID_IT_SN_DEMO_SUIMEN_A = 0x80CC,
    ID_IT_SN_MIZUBASHIRA2 = 0x80CE,
    ID_IT_SN_DEMO_SHIBUKI_A = 0x80DC,
    ID_IT_SN_DEMO_SHIBUKI_B = 0x80DD,
};
}
enum { dRes_ID_DBARREL_BCK_02_TR_CD_e = 0, dRes_ID_DBARREL_BDL_KTARU_02_e = 5 };
class daObj_Demo_Barrel_c : public fopAc_ac_c {
public:
    daObj_Demo_Barrel_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x100271D4; // HD: this translation unit's actor vtable.
    }
    void setParticleHahen();
    void setParticleSibuki();
    BOOL CreateHeap();
    static const char M_arcname[];

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ u8 m2A0[0x3EC - 0x3BC];
    /* 0x3EC */ cXyz m2D0;
    /* 0x3F8 */ be<s16> m2DC;
    /* 0x3FA */ be<s16> m2DE;
};
WWHD_OFFSET(daObj_Demo_Barrel_c, m2D0, 0x3EC);
WWHD_OFFSET(daObj_Demo_Barrel_c, m2DE, 0x3FA);
WWHD_SIZE(daObj_Demo_Barrel_c, 0x3FC);
