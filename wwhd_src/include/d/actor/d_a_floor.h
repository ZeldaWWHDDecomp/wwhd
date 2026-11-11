/* Wind Temple breakable floor, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_HHYU1_BDL_HHYU1_e = 4, dRes_INDEX_HHYU1_DZB_HHYU1_e = 7 };
enum { fpcNm_PLAYER_e = 0xA8, JA_SE_OBJ_HEAVY_FLOOR_BRK = 0x69C3 };
namespace dPa_name { enum { ID_AK_SN_KAZEBREAKFLOOR00 = 0x81A6, ID_AK_JT_ELEMENTSMOKE01 = 0x2027 }; }
/* Unit-local views of existing player and background APIs. */
struct daPy_floor_view : daPy_py_c {
    bool checkEquipHeavyBoots() { return (mNoResetFlg0 & 0x02000000) != 0; }
};
struct dBgW_floor_view {
    u8 _00[0xB0];
    gfn mRideCallback;
    void SetRideCallback(void (*callback)(dBgW*, fopAc_ac_c*, fopAc_ac_c*)) {
        mRideCallback.v = gabi::guest_fn((const void*)callback);
    }
};
WWHD_OFFSET(dBgW_floor_view, mRideCallback, 0xB0);
/* HD: scale fields have GameCube offsets minus four (no emitter vtable). */
struct JPAEmitter_floor_view {
    u8 _00[8];
    cXyz mEmitterScale;
    u8 _14[0x238 - 0x14];
    cXyz mGlobalParticleScale;
    void setEmitterScale(f32 x, f32 y, f32 z) {
        mEmitterScale.z = z; mEmitterScale.x = x; mEmitterScale.y = y;
    }
    void setGlobalParticleScale(f32 x, f32 y, f32 z) {
        mGlobalParticleScale.x = x; mGlobalParticleScale.z = z; mGlobalParticleScale.y = y;
    }
};
WWHD_OFFSET(JPAEmitter_floor_view, mGlobalParticleScale, 0x238);
struct daFloor_c : dBgS_MoveBgActor {
    daFloor_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1000F1E8; // HD: actor vtable.
        __ct(&mSmokeCallBack, 1);
    }
    static const char m_arcname[6];
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    void set_effect();
    BOOL Draw();
    BOOL Execute(Mtx34** mtx);

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mSwitchNo;
    /* 0x3F0 */ be<u8> mBreakStarted;  /* was field_0x2d8: rideCallBack 0213FD18: set once (with mBreakTimer 6) when the player in iron boots lands on the floor after mPlayerStanding was set; only then (tww the same) */
    /* 0x3F1 */ be<u8> mPlayerStanding;  /* was field_0x2d9: rideCallBack 0213FD18 sets 1 while the player rides the floor without the iron boots; Execute 0214000C clears it whenever the player moves in XZ; the break needs it (tww the same) */
    /* 0x3F2 */ be<u8> mBreakTimer;  /* was field_0x2da: rideCallBack 0213FD18: 6 when the break starts; Execute 0214000C counts it down and at 0 sets the effect, the break sound, the switch and deletes the floor (tww the same) */
    /* 0x3F3 */ be<u8> field_0x2db;
    /* 0x3F4 */ dPa_smokeEcallBack mSmokeCallBack;
};
WWHD_OFFSET(daFloor_c, mPhs, 0x3E0);
WWHD_OFFSET(daFloor_c, mSwitchNo, 0x3EC);
WWHD_OFFSET(daFloor_c, mSmokeCallBack, 0x3F4);
WWHD_SIZE(daFloor_c, 0x414);
