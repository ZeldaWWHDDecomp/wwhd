/** Torches (ep_class), WWHD layout. Actor members start +0x11C from GameCube;
 * HD adds the flicker factor, expands light influence/info, and keeps two extra light-info copies. */
#pragma once
#include "gc_api/gc_api.h"

#define SAFESTRING_VTBL 0x1000E25C
#define EP_VTBL 0x1000E3D4 /* HD: ep_class vtable */
static const dBgS_ObjAcch_vt EP_OBJACCH_VT = {0x1000E364, 0x1000E384, 0x1000E374};
static const dBgS_LinChk_vt EP_LINCHK_VT = {0x1000E394, 0x1000E3A4, 0x1000E3C4, 0x1000E3B4};
static const dBgS_GndChk_vt EP_GNDCHK_VT = {0x1000E324, 0x1000E334, 0x1000E354, 0x1000E344};

enum { /* HD archive order */
       dRes_INDEX_EP_BDL_EP_GA_e = 3,
       dRes_INDEX_EP_BDL_OBM_SHOKUDAI1_e = 4,
       dRes_INDEX_EP_BDL_VKTSD_e = 5,
};
enum {
    JA_SE_OBJ_TORCH_BURNING = 0x6103,
    JA_SE_OBJ_TORCH_IGNITION = 0x6902,
    JA_SE_OBJ_TORCH_OFF = 0x6904,
};
enum { fpcNm_BDK_e = 0xEE };

enum EpState {
    EP_OFF = 0,
    EP_IGNITE = 3,
    EP_BURN = 4,
    EP_EXTINGUISH = 10,
};
enum EpType {
    EP_TORCH = 0,
    EP_FLAME = 1,
    EP_LIGHT_ONLY = 2,
    EP_TORCH_NO_EVENT = 3,
};
enum EpTimer {
    EP_ALPHA_TIMER = 0,
    EP_SCALE_TIMER = 1,
    EP_EXTINGUISH_TIMER = 3,
};

// HD: J3DLightInfo expands from GameCube 0x34 to 0x44.
struct EpLightInfo {
    /* 0x00 */ be<f32> mPositionAndDirection[6];
    /* 0x18 */ be<u8> mColor[4];
    /* 0x1C */ be<s16> mHdParameters[4];
    /* 0x24 */ be<f32> mAttenuation[8];
};
WWHD_OFFSET(EpLightInfo, mColor, 0x18);
WWHD_OFFSET(EpLightInfo, mHdParameters, 0x1C);
WWHD_OFFSET(EpLightInfo, mAttenuation, 0x24);
WWHD_SIZE(EpLightInfo, 0x44);

struct ep_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ cXyz m1C;
    /* 0x28 */ be<s16> rotX;
    /* 0x2A */ be<s16> rotY;
    /* 0x2C */ u8 m2C[4];
    /* 0x30 */ be<f32> scaleXZ;
    /* 0x34 */ be<f32> scaleY;
    /* 0x38 */ be<s16> m38;
    /* 0x3A */ be<u8> mbEnabled;
    /* 0x3B */ be<u8> m3B;
};
WWHD_SIZE(ep_ga_s, 0x3C);

struct ep_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ Mtx34 mAlphaModelMtx;
    /* 0x3E4 */ gptr<J3DModel> mpModel;
    /* 0x3E8 */ dBgS_AcchCir mAcchCir;
    /* 0x428 */ dBgS_ObjAcch mAcch;
    /* 0x5EC */ be<s32> m4D0;
    /* 0x5F0 */ u8 m4D4[8];
    /* 0x5F8 */ be<f32> mLightPower;
    /* 0x5FC */ be<f32> m4E0;
    /* 0x600 */ be<f32> mFlicker; /* HD */
    /* 0x604 */ be<f32> mAlphaModelScale;
    /* 0x608 */ be<f32> mAlphaModelScaleTarget;
    /* 0x60C */ be<f32> mAlphaModelAlpha;
    /* 0x610 */ be<f32> mAlphaModelAlphaTarget;
    /* 0x614 */ be<s16> mTimers[4];
    /* 0x61C */ u8 m4FC;
    /* 0x61D */ be<u8> mbNoEp;
    /* 0x61E */ be<u8> mbShouldDrawModel;
    /* 0x61F */ u8 m4FF;
    /* 0x620 */ be<s16> mAlphaModelRotX;
    /* 0x622 */ be<s16> mAlphaModelRotY;
    /* 0x624 */ be<u8> mType;
    /* 0x625 */ be<u8> m505;
    /* 0x626 */ be<u8> mOnSwitchNo;
    /* 0x627 */ be<u8> m507;
    /* 0x628 */ be<u8> m508;
    /* 0x629 */ u8 m509[3];
    /* 0x62C */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x630 */ LIGHT_INFLUENCE mLight;
    /* 0x654 */ cXyz mPosTop;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Cyl mCyl;
    /* 0x7CC */ dCcD_Sph mSph1;
    /* 0x8F8 */ be<s16> m7D4;
    /* 0x8FA */ u8 m7D6[2];
    /* 0x8FC */ be<f32> m7D8;
    /* 0x900 */ be<s16> m7DC;
    /* 0x902 */ be<s8> mbHasObm;
    /* 0x903 */ u8 m7DF;
    /* 0x904 */ be<s32> m7E0;
    /* 0x908 */ be<u8> mbHasGa;
    /* 0x909 */ u8 m7E5[3];
    /* 0x90C */ ep_ga_s mEpGa[2];
    /* 0x984 */ EpLightInfo m860; /* J3DLightInfo */
    /* 0x9C8 */ u8 mLightPadding1[0xA44 - 0x9C8];
    /* 0xA44 */ EpLightInfo mHdLightInfo1; /* HD: J3DLightInfo copy */
    /* 0xA88 */ u8 mLightPadding2[0xAC8 - 0xA88];
    /* 0xAC8 */ EpLightInfo mHdLightInfo2; /* HD: J3DLightInfo copy */
    /* 0xB0C */ u8 mLightPadding3[0xB4C - 0xB0C];
    /* 0xB4C */ be<u8> mGroundCheckTimer;
    /* 0xB4D */ u8 mGroundCheckPadding[3];
};
WWHD_OFFSET(ep_class, m4D0, 0x5EC);
WWHD_OFFSET(ep_class, mTimers, 0x614);
WWHD_OFFSET(ep_class, mLight, 0x630);
WWHD_OFFSET(ep_class, mSph1, 0x7CC);
WWHD_OFFSET(ep_class, mEpGa, 0x90C);
WWHD_OFFSET(ep_class, mGroundCheckTimer, 0xB4C);
WWHD_SIZE(ep_class, 0xB50);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) */
static inline BOOL ep_eventEndCheckOld(const char* name) {
    return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name);
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline BOOL fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D77DC, a, name, flag, hind);
}
/* 023129C4 daObj::HitSeStart(const cXyz*, int roomNo, const dCcD_GObjInf*, u32) */
static inline void daObj_HitSeStart(cXyz* pos, s32 roomNo, dCcD_GObjInf* obj, u32 p) {
    gabi::call(0x023129C4, pos, roomNo, obj, p);
}
/* 025D69FC fopAcM_rollPlayerCrash(actor, f32, u32) */
static inline void fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 r, u32 p) {
    gabi::call(0x025D69FC, a, r, p);
}
/* 025D6B70 fopAcM_checkCullingBox(mtx, x0, y0, z0, x1, y1, z1) */
static inline u8 fopAcM_checkCullingBox(Mtx34* m, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    return gabi::call<u8>(0x025D6B70, m, x0, y0, z0, x1, y1, z1);
}
// HD: the clipper keeps its default far plane beside its current far plane.
struct EpClipper {
    u8 mFrustum[0x54];
    be<f32> mFar;
    u8 mFarPadding[4];
    be<f32> mDefaultFar;
};
WWHD_OFFSET(EpClipper, mFar, 0x54);
WWHD_OFFSET(EpClipper, mDefaultFar, 0x5C);
WWHD_SIZE(EpClipper, 0x60);
static inline EpClipper* ep_clipper() { return reinterpret_cast<EpClipper*>(0x1048CFF0u); }
static inline void mDoLib_clipper_changeFar(f32 far) {
    EpClipper* clipper = ep_clipper();
    clipper->mFar = far;
    gabi::call(0x0283801C, clipper);
}
static inline void mDoLib_clipper_resetFar() {
    EpClipper* clipper = ep_clipper();
    f32 far = clipper->mDefaultFar;
    clipper->mFar = far;
    gabi::call(0x0283801C, clipper);
}
/* 025D672C / 025D673C fopAcM_SetMin / SetMax */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D672C, a, x, y, z);
}
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D673C, a, x, y, z);
}
/* 025166F0 dCcD_Sph::dCcD_Sph (out of line) */
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
// HD: only the virtual slots used by this unit are exposed here.
struct EpPlayerVTable {
    u8 mOtherMethods[0x74];
    be<u32> getBokoFlamePos;
};
WWHD_OFFSET(EpPlayerVTable, getBokoFlamePos, 0x74);
struct EpSafeStringVTable {
    u8 mOtherMethods[0x14];
    be<u32> assureTerminationImpl;
};
WWHD_OFFSET(EpSafeStringVTable, assureTerminationImpl, 0x14);
static inline void daPy_getBokoFlamePos(fopAc_ac_c* player, cXyz* pos) {
    auto* methods = reinterpret_cast<EpPlayerVTable*>(static_cast<u32>(player->__vtbl));
    gabi::call_ptr(methods->getBokoFlamePos, player, pos);
}
static inline void SafeString_assure(SafeString* string) {
    auto* methods = reinterpret_cast<EpSafeStringVTable*>(static_cast<u32>(string->__vtbl));
    gabi::call_ptr(methods->assureTerminationImpl, string);
}

// HD: point-light stage classification uses the word at stage-info +0xC.
struct EpStageInfo {
    u8 mRoomInfo[0xC];
    be<u32> mTypeAndFlags;
};
WWHD_OFFSET(EpStageInfo, mTypeAndFlags, 0xC);
WWHD_SIZE(EpStageInfo, 0x10);

// HD: this unit writes the lifetime before loading and setting the emitter status.
struct EpEmitterStatus {
    u8 mEmitterData[0x5C];
    be<s32> mMaxFrame;
    u8 mParticleData[0x254 - 0x60];
    be<u32> mStatus;
};
WWHD_OFFSET(EpEmitterStatus, mMaxFrame, 0x5C);
WWHD_OFFSET(EpEmitterStatus, mStatus, 0x254);
WWHD_SIZE(EpEmitterStatus, 0x258);

struct EpSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_SIZE(EpSphereSource, 0x40);

struct EpCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_SIZE(EpCylinderSource, 0x44);

// HD: event index forwarded as the full register, rather than the shared s16 accessor.
static inline u32 ep_eventIndex(const char* name) {
    return gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), name, 0xFF);
}
static inline u32 ep_eventData(dEvent_manager_c* manager, u32 index) {
    return gabi::call<u32>(0x02544044, manager, index);
}

struct EpAction {
    gptr<char> mName;
};
WWHD_SIZE(EpAction, 4);

// HD: out-of-line destructors retain this unit's destruction flags and call order.
static inline void ep_destroyGroundCheck(dBgS_GndChk* check, s32 flags) {
    gabi::call(0x02008DAC, check, flags);
}
static inline void ep_destroySphere(dCcD_Sph* sphere, s32 flags) {
    gabi::call(0x02515AE8, sphere, flags);
}
static inline void ep_destroyAcch(dBgS_ObjAcch* acch, s32 flags) {
    gabi::call(0x024EFD9C, acch, flags);
}
static inline void ep_destroyCircle(cM3dGCir_l* circle, s32 flags) {
    gabi::call(0x02018034, circle, flags);
}
static inline void ep_destroyActor(ep_class* actor, s32 flags) {
    gabi::call(0x025D50BC, actor, flags);
}
