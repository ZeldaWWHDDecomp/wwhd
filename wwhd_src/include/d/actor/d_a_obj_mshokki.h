#pragma once
#include "bindings.h"
class daObjMshokki_c : public fopAc_ac_c {
public:
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mModel;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    be<s32> mType;
    u8 _72C[2];
    be<s16> mHasRested;
    be<u32> mFrame, mLastLaunch;
    be<s16> mSwingReference;
    u8 _73A[2];
    void set_mtx();
    bool create_heap();
    static void co_hitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);
    cPhs_State _create();
    bool _delete();
    void set_se();
    void break_proc();
    bool checkCollision();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(daObjMshokki_c,mModel,0x3B4);
WWHD_OFFSET(daObjMshokki_c,mAcch,0x3B8);
WWHD_OFFSET(daObjMshokki_c,mAcchCir,0x57C);
WWHD_OFFSET(daObjMshokki_c,mStts,0x5BC);
WWHD_OFFSET(daObjMshokki_c,mCyl,0x5F8);
WWHD_OFFSET(daObjMshokki_c,mType,0x728);
WWHD_OFFSET(daObjMshokki_c,mSwingReference,0x738);
WWHD_SIZE(daObjMshokki_c,0x73C);
