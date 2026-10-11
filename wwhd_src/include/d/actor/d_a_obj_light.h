// HD lighthouse layout; GameCube bodies and member layouts remain stubs.
#pragma once
#include "bindings.h"
namespace daObjLight {
struct LightBgW : dBgW {
    u8 mBeforeCrrFunc[0xA4];
    gfn mCrrFunc;
};
WWHD_OFFSET(LightBgW, mCrrFunc, 0xA8);
struct LightOffsets {
    cXyz mPosition[3];
    be<u32> mInitialized;
};
WWHD_OFFSET(LightOffsets, mInitialized, 0x24);
struct LightVtable {
    be<u32> mEntry;
};
struct LightStaticWord {
    be<u32> mValue;
};
struct LightStaticHalfword {
    be<s16> mValue;
};
struct LightStaticByte {
    be<u8> mValue;
};
enum SkanranResource { Skanran_BDL_LIGHT_e = 5, Skanran_BDL_LIGHTHOUSE_e = 7, Skanran_DZB_LIGHTHOUSE_e = 12 };
enum LightEvent { LightEvent_Lit = 0x1C02, LightEvent_Switch = 0x5D };
enum LightSound { LightSound_Rotate = 0x303B, LightSound_Ignite = 0x6A03, LightSound_Complete = 0x6A04 };

struct Act_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModels[3];
    /* 0x3C0 */ gptr<LightBgW> mBgW;
    /* 0x3C4 */ Mtx34 mBgMatrix;
    /* 0x3F4 */ be<s16> mAngle;
    /* 0x3F6 */ be<s16> mLightTimer;
    /* 0x3F8 */ be<s16> mLit;
    /* 0x3FA */ be<s16> mEventTimer;
    /* 0x3FC */ dCcD_Stts mStatus;
    /* 0x438 */ dCcD_Cyl mCylinder;
    /* 0x568 */ dPa_followEcallBack mFireCallback;
    /* 0x57C */ be<s16> mFirePhase;
    /* 0x57E */ be<s16> mFireRotation;
    /* 0x580 */ be<u8> mFireAlpha;
    u8 mBeforeFireScale[3];
    /* 0x584 */ be<f32> mFireScale;
    /* 0x588 */ Mtx34 mFireMatrix;
    /* 0x5B8 */ be<s16> mEventId;
    /* 0x5BA */ be<s16> mEventState;
    /* 0x5BC */ be<s16> mMainEventId;
    void set_mtx();
    bool create_heap();
    void init_collision();
    void exe_fire();
    bool set_fire(s32);
    cPhs_State _create();
    void delete_fire();
    bool _delete();
    void exe_event();
    bool now_event(s16);
    void renew_angle();
    void set_collision();
    bool set_event(s16);
    void control_light();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(Act_c, mModels, 0x3B4);
WWHD_OFFSET(Act_c, mBgMatrix, 0x3C4);
WWHD_OFFSET(Act_c, mStatus, 0x3FC);
WWHD_OFFSET(Act_c, mCylinder, 0x438);
WWHD_OFFSET(Act_c, mFireCallback, 0x568);
WWHD_OFFSET(Act_c, mFireMatrix, 0x588);
WWHD_OFFSET(Act_c, mMainEventId, 0x5BC);
WWHD_SIZE(LightOffsets, 0x28);
WWHD_SIZE(LightVtable, 4);
WWHD_SIZE(LightStaticWord, 4);
WWHD_SIZE(LightStaticHalfword, 2);
WWHD_SIZE(LightStaticByte, 1);
WWHD_OFFSET(Act_c, mFirePhase, 0x57C);
WWHD_OFFSET(Act_c, mFireScale, 0x584);
WWHD_OFFSET(Act_c, mEventId, 0x5B8);
} // namespace daObjLight

BOOL ObjLight_BgW_Set(dBgW*, cBgD_t*, u32, Mtx34*);
void ObjLight_sound(u32, cXyz*);
BOOL ObjLight_setLightAngle(s16);
s16 ObjLight_getLightAngle();
// HD: the GameCube poly-info reference is passed as a pointer by the guest ABI.
void dBgS_MoveBGProc_TypicalRotY(dBgW*, void*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*);

u8 ObjLight_createHeapByte(daObjLight::Act_c*);
