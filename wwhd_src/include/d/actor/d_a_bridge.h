/**
 * d_a_bridge.h (WWHD)
 * Rope bridge: layout and helpers shared by the d_a_bridge source files.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww include/d/actor/d_a_bridge.h) to the WWHD layout.
 */
#pragma once
#include "bindings.h"

/* ---- this TU's constants and statics ---- */
#define BRIDGE_SAFESTRING_VTBL 0x1000B54C
#define BRIDGE_VTBL 0x1000B574
#define BRIDGE_AAB_VTBL 0x1000B564
#define BRIDGE_CYL_SRC 0x101927B0 /* CreateInit's himo_cyl_src */
/* static cXyz* wind_vec; static s16 wy; static f32* wp; (HD addresses) */

namespace BridgeHD {
enum {
    fpcNm_PLAYER_e = 0xA8,
    fpcNm_BRIDGE_e = 0x59,
    fpcNm_MO2_e = 0xBC,
    fpcNm_BK_e = 0xBD,
    fpcNm_BOMB_e = 0x126,
};
}

// HD: Unit-private verified line and constructor ABI bindings.
/* mDoExt_3DlineMat1_c (HD 0x188; GameCube 0x3C): vtable at +0x130, mpLines at +0x184; a line is
 * {cXyz* mpSegments; u8* mpSize; ...}. Constructor 025EB82C, destructor 025EB8B8 (the matcher
 * names it mDoExt_3DlineMat1_c::draw), init 025EBA58, update 025ED1BC (as d_a_sss). */
struct BridgeLine {
    gptr<cXyz> mpSegments;
    gptr<be<u8>> mpSize;
    u8 padding8[8];
};
struct BridgeLineVtable {
    u8 padding0[0x14];
    gfn getMaterialID;
};
struct BridgeLineMaterial {
    u8 padding0[0x130];
    gptr<BridgeLineVtable> mVtable;
    u8 padding134[0x50];
    gptr<BridgeLine> mpLines;
};
WWHD_SIZE(BridgeLine, 0x10);
WWHD_SIZE(BridgeLineMaterial, 0x188);
WWHD_OFFSET(BridgeLineMaterial, mpLines, 0x184);
static inline BOOL mDoExt_3DlineMat1_init(BridgeLineMaterial *l, u16 numLines, u16 numSegs, void *tex,
                                          BOOL hasSize) {
    return gabi::call<BOOL>(0x025EBA58, l, numLines, numSegs, tex, hasSize);
}
static inline void mDoExt_3DlineMat1_ct(BridgeLineMaterial *l) { gabi::call(0x025EB82C, l); }
static inline void mDoExt_3DlineMat1_dt(BridgeLineMaterial *l, s32 flags) {
    gabi::call(0x025EB8B8, l, flags);
}
/* GHS array helpers: __construct_array(ptr, n, size, ctor), __destroy_arr(ptr, n, size, dtor, flags) */
static inline void __construct_array(void *p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void __destroy_arr(void *p, u32 n, u32 size, u32 dtor, u32 flags) {
    gabi::call(0x028F0164, p, n, size, dtor, flags);
}
/* dBgWSv (HD): `new dBgWSv()` is the constructor 024F5A18 with this == NULL; Set 024F5A78;
 * CopyBackVtx 024F5B08; ride callback at +0xB0; vertex table at +0x90, vertex count *(+0x94) */
static inline dBgW *dBgWSv_new() { return gabi::call<dBgW *>(0x024F5A18, 0); }
static inline BOOL dBgWSv_Set(dBgW *w, void *bgd, u32 flag) {
    return gabi::call<BOOL>(0x024F5A78, w, bgd, flag);
}
static inline void dBgWSv_CopyBackVtx(dBgW *w) { gabi::call(0x024F5B08, w); }
/* 020CB648 daBomb_c::getBombRestTime */
static inline s32 daBomb_getBombRestTime(void *b) { return gabi::call<s32>(0x020CB648, b); }
/* fopAcM_GetName: base_process_class::mProcName (s16 at +8) */
static inline s16 bridge_getActorName(void *p) { return reinterpret_cast<fopAc_ac_c *>(p)->mProcName; }

/* ---- layout ---- */
struct br_s {
    /* 0x000 */ gptr<J3DModel> mpModel;
    /* 0x004 */ dKy_tevstr_c mTevStr;
    /* 0x1CC */ gptr<J3DModel> mpModelRope1;
    /* 0x1D0 */ gptr<J3DModel> mpModelRope0;
    /* 0x1D4 */ BridgeLineMaterial mLineMat1;
    /* 0x35C */ cXyz m0F8[3];
    /* 0x380 */ cXyz m11C[3];
    /* 0x3A4 */ dCcD_Cyl mCyl[2];
    /* 0x604 */ be<s16> m3A0[2];
    /* 0x608 */ be<s8> m3A4;
    /* 0x609 */ be<s8> m3A5;
    /* 0x60A */ u8 _60A[2];
    /* 0x60C */ cXyz m3A8[2];
    /* 0x624 */ be<s16> m3C0;
    /* 0x626 */ be<s16> m3C2;
    /* 0x628 */ gptr<JPABaseEmitter> m3C4;
    /* 0x62C */ gptr<JPABaseEmitter> m3C8;
    /* 0x630 */ cXyz m3CC;
    /* 0x63C */ cXyz mPosition;
    /* 0x648 */ csXyz mRotation;
    /* 0x64E */ be<s16> mRotationYExtra;
    /* 0x650 */ be<f32> m3EC;
    /* 0x654 */ be<f32> m3F0;
    /* 0x658 */ be<f32> m3F4;
    /* 0x65C */ be<f32> m3F8;
    /* 0x660 */ be<f32> m3FC;
    /* 0x664 */ be<s16> m400;
    /* 0x666 */ be<s16> m402;
    /* 0x668 */ be<s16> m404;
    /* 0x66A */ be<u8> m406;
    /* 0x66B */ be<u8> m407;
    /* 0x66C */ be<u8> m408;
    /* 0x66D */ u8 _66D[3];
    /* 0x670 */ cXyz mScale;
    /* 0x67C */ be<s16> m418;
    /* 0x67E */ u8 _67E[2];
};
WWHD_OFFSET(br_s, mLineMat1, 0x1D4);
WWHD_OFFSET(br_s, mCyl, 0x3A4);
WWHD_OFFSET(br_s, m3CC, 0x630);
WWHD_OFFSET(br_s, m418, 0x67C);
WWHD_SIZE(br_s, 0x680);

struct bridge_class : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhase;
    /* 0x03B4 */ be<s16> mMoveProcMode;
    /* 0x03B6 */ u8 _3B6[2];
    /* 0x03B8 */ BridgeLineMaterial mLineMat;
    /* 0x0540 */ be<u8> mTypeBits;
    /* 0x0541 */ be<u8> m02D9;
    /* 0x0542 */ be<u8> mPathId;
    /* 0x0543 */ be<u8> mPathIdP;
    /* 0x0544 */ be<s8> mBrCount;
    /* 0x0545 */ be<s8> m02DD;
    /* 0x0546 */ u8 _546[2];
    /* 0x0548 */ be<f32> m02E0;
    /* 0x054C */ be<f32> m02E4;
    /* 0x0550 */ gptr<dBgW> mpBgW;
    /* 0x0554 */ be<s16> m02EC;
    /* 0x0556 */ be<s16> m02EE;
    /* 0x0558 */ be<s16> m02F0;
    /* 0x055A */ be<s16> m02F2;
    /* 0x055C */ be<f32> m02F4;
    /* 0x0560 */ be<f32> m02F8;
    /* 0x0564 */ be<f32> m02FC;
    /* 0x0568 */ be<s16> m0300;
    /* 0x056A */ be<s16> m0302;
    /* 0x056C */ be<s32> m0304;
    /* 0x0570 */ be<s32> m0308;
    /* 0x0574 */ be<s32> m030C;
    /* 0x0578 */ u8 _578[2];
    /* 0x057A */ be<s16> m0312;
    /* 0x057C */ cXyz mEndPos;
    /* 0x0588 */ cXyz m0320;
    /* 0x0594 */ cXyz m032C;
    /* 0x05A0 */ gptr<bridge_class> mpAite;
    /* 0x05A4 */ be<s8> m033C;
    /* 0x05A5 */ u8 _5A5[3];
    /* 0x05A8 */ br_s mBr[50];
    /* 0x14AA8 */ dCcD_Stts mStts; /* HD: no mbStopDraw */
};
WWHD_OFFSET(bridge_class, mLineMat, 0x3B8);
WWHD_OFFSET(bridge_class, mTypeBits, 0x540);
WWHD_OFFSET(bridge_class, mpBgW, 0x550);
WWHD_OFFSET(bridge_class, mEndPos, 0x57C);
WWHD_OFFSET(bridge_class, mBr, 0x5A8);
WWHD_OFFSET(bridge_class, mStts, 0x14AA8);
WWHD_SIZE(bridge_class, 0x14AE4);

// Shared across the three source files; defined once in the owner source.
extern gptr<cXyz> &wind_vec;
extern gptr<be<f32>> &wp;
extern be<s16> &wy;
extern be<f32> (&ita_z_p)[11];

// HD: Views of rider, particle and mutable collision fields used by this unit.
namespace BridgeHD {
struct ActorName {
    u8 padding0[8];
    be<s16> mProcName;
};
struct PlayerFlags {
    u8 padding0[0x3B8];
    be<u32> mNoResetFlg0, mNoResetFlg1, mResetFlg0;
};
struct BokoReaction {
    u8 padding0[0xC6C];
    be<u32> m79C;
    u8 paddingC70[12];
    csXyz m7AC;
    be<s16> m7B2, m7B4;
    u8 paddingC86[2];
    be<u32> m7B8, m0B2C;
};
struct Emitter {
    u8 padding0[0x34];
    be<f32> mRate;
    u8 padding38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    u8 padding60[0x1C];
    be<f32> mVolumeSweep;
    u8 padding80[0x170];
    be<f32> mGlobalRotation[9];
    u8 padding214[0x18];
    cXyz mGlobalTranslation, mGlobalParticleScale;
    u8 padding244[0x10];
    be<u32> mFlags;
};
struct BackgroundData {
    be<s32> m_v_num;
};
struct Background {
    u8 padding0[0x90];
    gptr<cXyz> pm_vtx_tbl;
    gptr<BackgroundData> pm_bgd;
    u8 padding98[0x18];
    be<u32> mpRideCallback;
};
struct PathPoint {
    u8 mArgs[4];
    cXyz m_position;
};
struct Path {
    u8 padding0[8];
    gptr<PathPoint> m_points;
};
struct VectorWords {
    be<u32> mWords[3];
};
struct LightTemplate {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mAttenuation[4];
    be<f32> mScales[8];
};
struct LightState {
    LightTemplate mPrimary;
    u8 padding44[0x7C];
    LightTemplate mSecondary;
    u8 padding104[0x40];
    LightTemplate mAmbient;
};
struct EventState {
    u8 padding0[0x5292];
    be<u8> mRunning;
};
struct SinCos {
    be<f32> mSin, mCos;
};
struct PlayerVtable {
    u8 padding0[0x74];
    gfn getBokoFlamePos;
};
struct ActorVirtual {
    u8 padding0[0xB4];
    gptr<PlayerVtable> mVtable;
};
WWHD_OFFSET(PlayerFlags, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(BokoReaction, m7B8, 0xC88);
WWHD_OFFSET(BokoReaction, m0B2C, 0xC8C);
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_SIZE(LightTemplate, 0x44);
WWHD_OFFSET(LightState, mSecondary, 0xC0);
WWHD_OFFSET(LightState, mAmbient, 0x144);
} // namespace BridgeHD

namespace BridgeHD {
struct HeaderState {
    be<f32> mAngleMin, mAngleMax;
    be<s16> mWindAngle;
    be<u8> mByte, mFlag;
    be<u32> mZeroWords[4];
    cXyz mChainParticleScale;
    be<u32> mScaleGuard;
};
WWHD_OFFSET(HeaderState, mZeroWords, 0xC);
WWHD_OFFSET(HeaderState, mChainParticleScale, 0x1C);
inline HeaderState *getHeaderState() { return gabi::at<HeaderState>(0x1046294C); }
inline f32 getCos(s32 angle) { return gabi::at<SinCos>(0x104A44F8)[(u16)angle >> 3].mCos; }
inline s32 getBridgeModel(s32 index) { return gabi::at<be<s32>>(0x1000B674)[index]; }
inline LightTemplate *getLightTemplate() { return gabi::at<LightTemplate>(0x1016E414); }
/* 028ED6F8 [v d_a_bridge] */ inline void constructHeaderByte(void *p) { gabi::call(0x028ED6F8, p); }
/* 028EAB2C [v d_a_bridge] */ inline void constructHeaderFlag(void *p) { gabi::call(0x028EAB2C, p); }
/* 025D50BC [v d_a_bridge] */ inline void deleteActor(fopAc_ac_c *p, s32 flags) {
    gabi::call(0x025D50BC, p, flags);
}
} // namespace BridgeHD

namespace BridgeHD {
namespace SDK {
// HD: Verified caller binding [v d_a_bridge].
inline void mDoExt_3DlineMat1_update(BridgeLineMaterial *l, s32 segs, u32 color, dKy_tevstr_c *tev) {
    gabi::call(0x025ED1BC, l, segs, color, tev);
}
// HD: Verified caller binding [v d_a_bridge].
inline void mDoExt_3DlineMat1_update_w(BridgeLineMaterial *l, u16 segs, f32 width, u32 color, u16 p,
                                       dKy_tevstr_c *tev) {
    gabi::call(0x025EC62C, l, segs, width, color, p, tev);
}
// HD: Verified caller binding [v d_a_bridge].
inline void dComIfGd_set3DlineMat(BridgeLineMaterial *l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(l->mVtable->getMaterialID.get(), l);
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}
struct CcAtInfo {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo, 0x1C);
// HD: Verified caller binding [v d_a_bridge].
inline void kikuzu_set(bridge_class *i_this, cXyz *pPos) { gabi::call(0x020DFE7C, i_this, pPos); }
// HD: Verified caller binding [v d_a_bridge].
inline void himo_cut_control1(cXyz *pPos) { gabi::call(0x020E0FAC, pPos); }
// HD: Verified caller binding [v d_a_bridge].
inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
// HD: Verified caller binding [v d_a_bridge].
inline cXyz *dKyw_get_wind_vec() { return gabi::call<cXyz *>(0x0257DAA8); }
// HD: Verified caller binding [v d_a_bridge].
inline f64 sin_d(f64 x) { return gabi::call<f64>(0x028F43F8, x); }
// HD: Verified caller binding [v d_a_bridge].
inline void at_power_check(CcAtInfo *i) { gabi::call(0x02518DB0, i); }
// HD: Verified caller binding [v d_a_bridge].
inline void JPA_setGlobalRTMatrix(u32 e, Mtx34 *m) {
    gabi::call(0x028249B0, m, gabi::ea(&gabi::at<Emitter>(e)->mGlobalRotation),
               gabi::ea(&gabi::at<Emitter>(e)->mGlobalTranslation));
}
/* 028E8DAC [v d_a_bridge] */ inline void subtractVector(cXyz *a, cXyz *b, cXyz *out) {
    gabi::call(0x028E8DAC, a, b, out);
}
} // namespace SDK
} // namespace BridgeHD

namespace BridgeHD {
enum Callback_e : u32 {
    RideCallback = 0x020DF908,
    SearchPartner = 0x020E115C,
    CreateHeap = 0x020E3890,
    ConstructCylinder = 0x020E450C,
    ConstructPlank = 0x020E3E24,
    DeleteCylinder = 0x02515A70,
    DeletePlank = 0x020E4598
};
enum Resource_e {
    dRes_INDEX_BRIDGE_BDL_OBM_CHAIN1_e = 6,
    dRes_INDEX_BRIDGE_DZB_MBRDG_e = 9,
    dRes_INDEX_BRIDGE_DZB_MBRDG2_e = 10,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
    dRes_INDEX_ALWAYS_BTI_TXM_ROPE1_e = 0x8D
};
enum PlayerFlag_e : u32 { HeavyRider = 0x02000000, BreakPlank = 0x00020000 };
inline u32 plankRopeColor() { return 0x101927F4; }
inline u32 sideRopeColor() { return 0x101927F8; }
inline void registerVectorObject() { __register_global_object(0x101927FC); }
inline void registerAngleObject() { __register_global_object(0x10192808); }
inline void registerByteObject() { __register_global_object(0x10192814); }
} // namespace BridgeHD
