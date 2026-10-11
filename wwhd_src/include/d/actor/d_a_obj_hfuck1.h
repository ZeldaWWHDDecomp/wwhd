/* Hookshot target, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local bindings. */
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
void dCcD_Sph__ct(dCcD_Sph*);
void dCcD_Sph__dt(dCcD_Sph*, s32);
inline u32 fopAcM_GetID(fopAc_ac_c* actor) { return actor->mBsPcId; }

/* Unit-local view for the existing daPy_py_c virtual, until its shared header is expanded. */
struct daPy_hookshot_hfuck1 : daPy_py_c {
    void setHookshotCarryOffset(u32 id, const cXyz* offset) {
        // HD: virtual slot +0x10C; preserve the actor ID and constant-vector pointer.
        gabi::call_ptr(gabi::load<u32>(__vtbl + 0x10C), this, id, offset);
    }
};

enum { dRes_INDEX_HFUCK1_BDL_HFUCK1_e = 4, dRes_INDEX_HFUCK1_DZB_HFUCK1_e = 7 };
enum { fpcNm_HOOKSHOT_e = 0xA9, AT_TYPE_HOOKSHOT = 0x8000 };

class daObjHfuck1_c : public fopAc_ac_c {
public:
    daObjHfuck1_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002A28C; // HD: this translation unit's actor vtable.
        dCcD_Stts_ct(&mStts);
        dCcD_Sph__ct(&mSph);
    }
    void init_mtx();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    bool checkCollision();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ dCcD_Stts mStts;
    /* 0x3F8 */ dCcD_Sph mSph;
    /* 0x524 */ gptr<fopAc_ac_c> mpHookshotActor;
};
WWHD_OFFSET(daObjHfuck1_c, mStts, 0x3BC);
WWHD_OFFSET(daObjHfuck1_c, mSph, 0x3F8);
WWHD_OFFSET(daObjHfuck1_c, mpHookshotActor, 0x524);
WWHD_SIZE(daObjHfuck1_c, 0x528);
