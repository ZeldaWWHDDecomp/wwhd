// Traveling merchant hats. HD: fields move from GC 0x290 to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"
class daObjHat_c : public fopAc_ac_c {
public:
    daObjHat_c();
    void executeNormal();
    enum Prm_e { PRM_HAT_NO_W = 8, PRM_HAT_NO_S = 0 };
    using MoveFunc_t = void (daObjHat_c::*)();
    struct MoveEntry {
        gabi::ptmf<MoveFunc_t> proc;
    };
    BOOL createHeap();
    u32 getPrmHatNo();
    void setSpeed(cXyz*);
    void setMtx();
    s32 createInit();
    s32 _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ Mtx34 mMtx;
    /* 0x3EC */ dBgS_ObjAcch mAcch;
    /* 0x5B0 */ dBgS_AcchCir mAcchCir;
    /* 0x5F0 */ dCcD_Stts mStts;
    /* 0x62C */ dCcD_Cyl mCyl;
    /* 0x75C */ cXyz mMoveNorm;
    /* 0x768 */ be<u8> mState, mHatNo;
    /* 0x76A */ u8 padding[2];
};
WWHD_OFFSET(daObjHat_c, mAcch, 0x3EC);
WWHD_OFFSET(daObjHat_c, mAcchCir, 0x5B0);
WWHD_OFFSET(daObjHat_c, mCyl, 0x62C);
WWHD_OFFSET(daObjHat_c, mMoveNorm, 0x75C);
WWHD_SIZE(daObjHat_c, 0x76C);

// HD: unit-local view of the merchant's wind vector.
struct HatMerchantView : fopAc_ac_c {
    u8 mPadding[0x894 - sizeof(fopAc_ac_c)];
    cXyz mWindVec;
};
WWHD_OFFSET(HatMerchantView, mWindVec, 0x894);
BOOL fopAcM_SearchByID(fpc_ProcID id, gptr<fopAc_ac_c>* actor);
void fopAcM_posMove(fopAc_ac_c* actor, dCcD_Stts* status);

void cLib_addCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep);

// HD: out-of-line actor construction remains a guest call from _create.
daObjHat_c* daObjHat_construct(daObjHat_c* self);
