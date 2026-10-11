/* Forbidden Woods solid vine floor, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_KANAT_BDL_KANAT_e = 4, dRes_INDEX_KANAT_DZB_KANAT_e = 7 };
namespace dPa_name {
enum { ID_AK_SN_KOKIRIHOUSEHAHEN00 = 0x82A2, ID_AK_ST_KOKIRIHOUSESMOKE00 = 0xA2A3 };
}
/* Unit-local API over the existing callback layout; +0x10 end flag, +0x11 rate-off. */
struct dPa_smokeEcallBack_kanat : dPa_smokeEcallBack {
    bool isEnd() { return (m10 & 1) != 0; }
    void setRateOff(u8 v) { m11 = v; }
};
inline u8 fpcM_CreateResult(base_process_class* p) { return (u8)p->mCreateResult; }
namespace daObjKanat {
struct Act_c : dBgS_MoveBgActor {
    Act_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1002BBF0; // HD: actor vtable.
        __ct(&mSmokeCb, 1);
    }
    static const char M_arcname[];
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Delete();
    BOOL Execute(Mtx34** pMtx);
    BOOL Draw();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dPa_smokeEcallBack_kanat mSmokeCb;
    /* 0x40C */ be<u8> mIsBroken;
    /* 0x40D */ be<u8> mIsVisible;
    /* 0x40E */ u8 _40E[2];
};
WWHD_OFFSET(Act_c, mSmokeCb, 0x3EC);
WWHD_OFFSET(Act_c, mIsBroken, 0x40C);
WWHD_SIZE(Act_c, 0x410);
}  // namespace daObjKanat
using daObjKanat::Act_c;

