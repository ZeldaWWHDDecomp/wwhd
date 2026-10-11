/* Forbidden Woods hanging flower platform.
 * HD: actor members are shifted +0x11C from tww; unknown names retain tww spelling. */
#pragma once
#include "gc_api/gc_api.h"

// HD: renderer and background callbacks are inline accesses in this unit.
struct KitaEmitterView {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x22C - 0x60];
    cXyz mGlobalTranslation;
    u8 _238[0x254 - 0x238];
    be<u32> mStatus;
    u8 _258[0x262 - 0x258];
    be<u8> mVersion;
};
WWHD_OFFSET(KitaEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(KitaEmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(KitaEmitterView, mStatus, 0x254);
WWHD_OFFSET(KitaEmitterView, mVersion, 0x262);
struct KitaBgWView {
    u8 _00[0xA8];
    be<u32> mCrrFunc;
    u8 _AC[4];
    be<u32> mRideCallback;
};
WWHD_OFFSET(KitaBgWView, mCrrFunc, 0xA8);
WWHD_OFFSET(KitaBgWView, mRideCallback, 0xB0);
// The hanging vine keeps pointers into its platform's endpoint and release-state arrays.
struct KitaShandView : fopAc_ac_c {
    u8 _3AC[0x424 - 0x3AC];
    be<fpc_ProcID> field_308;
    u8 _428[4];
    gptr<cXyz> field_310;
    gptr<be<u8>> field_314;
};
WWHD_OFFSET(KitaShandView, field_308, 0x424);
WWHD_OFFSET(KitaShandView, field_310, 0x42C);
WWHD_OFFSET(KitaShandView, field_314, 0x430);

// HD: unit-private bindings preserve this translation unit's inline forms.
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C) */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D672C, a, x, y, z);
}
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D673C, a, x, y, z);
}
/* HD fopAcM_seStartCurrent inline: checks the actor and &current.pos for NULL */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* JPABaseEmitter::becomeInvalidEmitter (HD inline): max frame (+0x5C) = -1, flags (+0x254) |= 1 */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    auto* emitter = reinterpret_cast<KitaEmitterView*>(e);
    u32 status = emitter->mStatus;
    emitter->mMaxFrame = -1;
    emitter->mStatus = status | 1;
}
/* JPABaseEmitter::setGlobalTranslation (HD inline): +0x22C; y negated for version (+0x262) >= 7 */
static inline void JPABaseEmitter_setGlobalTranslation(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    auto* emitter = reinterpret_cast<KitaEmitterView*>(e);
    if (emitter->mVersion >= 7)
        y = -y;
    emitter->mGlobalTranslation.x = x;
    emitter->mGlobalTranslation.y = y;
    emitter->mGlobalTranslation.z = z;
}
/* dBgW: SetCrrFunc +0xA8, SetRideCallback +0xB0; dBgS_MoveBGProc_Typical 024EE658 */
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) {
    reinterpret_cast<KitaBgWView*>(w)->mCrrFunc = fn;
}
static inline void dBgW_SetRideCallback(dBgW* w, u32 fn) {
    reinterpret_cast<KitaBgWView*>(w)->mRideCallback = fn;
}
/* csXyz::operator+ (0201A4DC) returns the 6-byte csXyz in r3:r4 */
static inline void csXyz_pl_to(csXyz* dst, csXyz* a, csXyz* b) {
    u32 hi = gabi::call<u32>(0x0201A4DC, a, b);
    u32 lo = gabi::cpu->r[4];
    dst->x = (s16)(hi >> 16);
    dst->y = (s16)hi;
    dst->z = (s16)(lo >> 16);
}
/* fopAcM_prm_class, fopAcM_CreateAppend, fopAcM_Create (inline: fpcSCtRq_Request on the current
 * layer) */
struct fopAcM_prm_class {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ u8 _16[0x21 - 0x16];
    /* 0x21 */ be<s8> room_no;
};
static inline fopAcM_prm_class* fopAcM_CreateAppend() {
    return gabi::call<fopAcM_prm_class*>(0x025D5600);
}
static inline fpc_ProcID fopAcM_Create(s16 name, fopAcM_prm_class* p) {
    u32 layer = gabi::call<u32>(0x025DED64); /* fpcLy_CurrentLayer */
    return gabi::call<fpc_ProcID>(0x025E14A8, layer, name, 0, 0, p);
}
static inline void dBgS_GndChk_dt(void* c, const dBgS_GndChk_vt& vt, u32 vt4C_dt) {
    auto* check = static_cast<dBgS_GndChk*>(c);
    check->dBgS_PolyPassChk::__vtbl = vt.v40;
    check->dBgS_GrpPassChk::__vtbl = vt4C_dt;
    check->cBgS_PolyInfo::__vtbl = vt.v20;
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}

struct kita_class : fopAc_ac_c {
    enum : u32 { __vt = 0x10012EC4 }; // HD: virtual destructor.
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mMoveCounter;
    /* 0x3B6 */ be<s16> field_29A;
    /* 0x3B8 */ gptr<J3DModel> mModel;
    /* 0x3BC */ be<u8> field_2A0;
    /* 0x3BD */ be<u8> field_2A1;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ cXyz mPosRel;
    /* 0x3CC */ cXyz field_2B0;
    /* 0x3D8 */ cXyz field_2BC;
    /* 0x3E4 */ csXyz field_2C8;
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ be<f32> mHeight;
    /* 0x3F0 */ be<u32> field_2D4[4];
    /* 0x400 */ be<u8> field_2E4[4];
    /* 0x404 */ cXyz field_2E8[4];
    /* 0x434 */ be<u8> field_318[4];
    /* 0x438 */ be<s16> mRotY;
    /* 0x43A */ be<s16> mRotX;
    /* 0x43C */ be<f32> field_320;
    /* 0x440 */ be<s32> unused_324;
    /* 0x444 */ Mtx34 mBgwMtx;
    /* 0x474 */ gptr<dBgW> pm_bgw;
    /* 0x478 */ be<f32> field_35C;
    /* 0x47C */ be<u8> field_360;
    /* 0x47D */ u8 _47D;
    /* 0x47E */ be<s16> mPlayerAngle;
    /* 0x480 */ be<f32> field_364;
    /* 0x484 */ be<f32> mAngleYSpeed;
    /* 0x488 */ be<f32> field_36C;
    /* 0x48C */ be<f32> field_370;
    /* 0x490 */ be<s16> field_374;
    /* 0x492 */ be<s16> mExecuteCount;
    /* 0x494 */ gptr<JPABaseEmitter> mBaseEmitter;
    /* 0x498 */ dCcD_Stts mStts;
    /* 0x4D4 */ dCcD_Sph mSph;
    /* 0x600 */ dBgS_AcchCir mAcchCir;
    /* 0x640 */ dBgS_ObjAcch mAcch;
};
WWHD_OFFSET(kita_class, mPosRel, 0x3C0);
WWHD_OFFSET(kita_class, field_2E8, 0x404);
WWHD_OFFSET(kita_class, mBgwMtx, 0x444);
WWHD_OFFSET(kita_class, field_360, 0x47C);
WWHD_OFFSET(kita_class, mBaseEmitter, 0x494);
WWHD_OFFSET(kita_class, mSph, 0x4D4);
WWHD_OFFSET(kita_class, mAcch, 0x640);
WWHD_SIZE(kita_class, 0x804);

// HD: sphere construction/destruction are out of line in this translation unit.
inline void kitaSphereConstruct(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void kitaSphereDestroy(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
inline BOOL kitaStartShock(dVibration_c* vibration, s32 power, s32 flags, cXyz* direction) {
    return gabi::call<BOOL>(0x025CB374, vibration, power, flags, direction);
}

// HD: external background correction callback.
static constexpr u32 kitaMoveBGProcTypical = 0x024EE658;

enum {
    dRes_INDEX_KITA_BDL_VHLIF_00_e = 4,
    dRes_INDEX_KITA_DZB_HLIF_00_e = 7,
};
WWHD_SIZE(KitaEmitterView, 0x263);
WWHD_SIZE(KitaBgWView, 0xB4);
WWHD_SIZE(KitaShandView, 0x434);
WWHD_OFFSET(fopAcM_prm_class, position, 4);
WWHD_OFFSET(fopAcM_prm_class, angle, 0x10);
WWHD_OFFSET(fopAcM_prm_class, room_no, 0x21);
WWHD_SIZE(fopAcM_prm_class, 0x22);
