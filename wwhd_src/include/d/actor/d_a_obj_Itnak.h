/* Unused Darknut statue: verified HD layout (GameCube has no member layout). */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_ITNAK_BDL_e = 3 };
/* Existing unit-local bindings; GameCube daObj::PrmAbstract/HitSeStart/HitEff_hibana. */
u32 PrmAbstract(fopAc_ac_c*, s32, s32);
void daObj_HitSeStart(const cXyz*, s32, dCcD_GObjInf*, u32);
void daObj_HitEff_hibana(fopAc_ac_c*, dCcD_Cyl*);
void dKy_Sound_set(cXyz*, s32, u32, s32);
BOOL fopAcM_rollPlayerCrash(fopAc_ac_c*, f32, u32);
/* HD: retain virtual dispatch of ClrTgHit, vtable +0x3C. */
inline void ClrTgHit_v(dCcD_GObjInf* o) { gabi::call_ptr(gabi::load<u32>(o->__vtbl_hitinf + 0x3C), o); }
namespace daObjItnak {
struct Act_c : fopAc_ac_c {
    Act_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002B890; // HD: actor vtable.
        dCcD_Stts_ct(&mStts0); dCcD_Cyl_ct(&mCyl0, 0x1002B880);
        dCcD_Stts_ct(&mStts1); dCcD_Cyl_ct(&mCyl1, 0x1002B880);
        dCcD_Stts_ct(&mStts2); dCcD_Cyl_ct(&mCyl2, 0x1002B880);
    }
    static const char M_arcname[];
    u32 param_get_arg0();
    bool is_switch();
    u8 create_heap(); /* bool, returned as u8 (clrlwi): a tail call passes the byte through */
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool set_co_se(dCcD_Cyl* cyl);
    void manage_draw_flag();
    void set_collision();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ Mtx34 mMtx;
    /* 0x3E8 */ dCcD_Stts mStts0;
    /* 0x424 */ dCcD_Cyl mCyl0;
    /* 0x554 */ dCcD_Stts mStts1;
    /* 0x590 */ dCcD_Cyl mCyl1;
    /* 0x6C0 */ dCcD_Stts mStts2;
    /* 0x6FC */ dCcD_Cyl mCyl2;
    /* 0x82C */ be<s32> mArg0;
    /* 0x830 */ be<s32> mDrawFlag;
};
WWHD_OFFSET(Act_c, mStts0, 0x3E8);
WWHD_OFFSET(Act_c, mCyl2, 0x6FC);
WWHD_OFFSET(Act_c, mDrawFlag, 0x830);
WWHD_SIZE(Act_c, 0x834);
}  // namespace daObjItnak
using daObjItnak::Act_c;

