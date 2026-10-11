/* Forsaken Fortress anchors, WWHD layout (actor fields shift by 0x11C). */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local pointer accessor (distinct from dKyw_get_wind_pow's value return). */
be<f32>* dKyw_get_wind_power();
enum {
    dRes_INDEX_IKARI_BDL_S_IKARI2_e = 3,
    dRes_INDEX_IKARI_BDL_S_IKARI3_e = 4,
    dRes_INDEX_IKARI_BDL_S_IKARI4_e = 5,
};
struct daObjIkariHIO_c {
    enum : u32 { __vt = 0x10011CA4 };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> unk[2];
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> mWindPowerScale;
};
WWHD_OFFSET(daObjIkariHIO_c, mWindPowerScale, 8);
WWHD_SIZE(daObjIkariHIO_c, 0xC);

/* Unit-local view beyond the shared environment-light prefix. */
struct daIkari_env_light_view {
    /* 0x000 */ u8 _000[0xB64];
    /* 0xB64 */ GXColor mActorC0;
    /* 0xB68 */ GXColor mActorK0;
};
WWHD_OFFSET(daIkari_env_light_view, mActorC0, 0xB64);
WWHD_OFFSET(daIkari_env_light_view, mActorK0, 0xB68);

class daIkari_c : public fopAc_ac_c {
public:
    daIkari_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x10011C94; // HD: this translation unit's actor vtable.
    }
    void setMtx();
    BOOL _createHeap();
    void getArg();
    bool _execute();
    bool _draw();
    cPhs_State _create();
    bool _delete();
    static const char M_arcname[6];

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ be<u8> mEnvType;
    /* 0x3B9 */ be<u8> mModelType;
    /* 0x3BA */ u8 m29E[2];
    /* 0x3BC */ be<s32> mTimer;
};
WWHD_OFFSET(daIkari_c, mpModel, 0x3B4);
WWHD_OFFSET(daIkari_c, mTimer, 0x3BC);
WWHD_SIZE(daIkari_c, 0x3C0);
