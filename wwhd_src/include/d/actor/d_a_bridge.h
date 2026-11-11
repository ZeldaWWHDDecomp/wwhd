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
    /* 0x35C */ cXyz mSide1Pos[3];  /* was m0F8: daBridge_Execute 020E11E8: the plank's side 1 points: [1] the edge at -99 * scale.x, [2] 30 below it, [0] the rope top above [1] (while mRopeTopTimer runs); rope 1 / chain 1 hang from it (rope_fire, side_rope_segment, daBridge_Draw 020DFF60 draw_plank_rope / draw_chain); ride_call_back 020DF908 hands [1] to a Bokoblin */
    /* 0x380 */ cXyz mSide0Pos[3];  /* was m11C: daBridge_Execute 020E11E8: the plank's side 0 points: [1] the edge at +99 * scale.x, [2] 30 below it, [0] the rope top above [1] (while mRopeTopTimer runs); rope 0 / chain 0 hang from it (rope_fire, side_rope_segment, daBridge_Draw 020DFF60); ride_call_back 020DF908 hands [1] to a Bokoblin */
    /* 0x3A4 */ dCcD_Cyl mCyl[2];
    /* 0x604 */ be<s16> mRopeHitTimer[2];  /* was m3A0: rope_hit / chain_hit (in daBridge_Execute 020E11E8): [side] = 20 (rope) / 15 (chain) on a hit, only while below 10; daBridge_Execute 020E11E8 counts both down; amplitude of the rope's shake (daBridge_Draw 020DFF60, side_rope_segment) and with both, the plank's (bridge_move_normal) */
    /* 0x608 */ be<s8> mRope0Hp;  /* was m3A4: bridge_move (in daBridge_Execute 020E11E8): 3; rope_hit (in daBridge_Execute 020E11E8) subtracts the hit's damage (4 above 1) for side 0, the rope is cut at 0 or below (mRopeFlags bit 1 off); daBridge_Draw 020DFF60 thins the rope at 2 and 1 */
    /* 0x609 */ be<s8> mRope1Hp;  /* was m3A5: bridge_move (in daBridge_Execute 020E11E8): 3; rope_hit (in daBridge_Execute 020E11E8): as mRope0Hp for side 1 (mRopeFlags bit 2) */
    /* 0x60A */ u8 _60A[2];
    /* 0x60C */ cXyz mRopeMidPos[2];  /* was m3A8: daBridge_Draw 020DFF60 (draw_plank_rope): [line] = the middle point of the plank's rope (segment 2 of 5); rope_hit (in daBridge_Execute 020E11E8) puts the hit and splinter effects there */
    /* 0x624 */ be<s16> mRope0FireTimer;  /* was m3C0: rope_fire (in daBridge_Execute 020E11E8): 30 when the player's Bokoblin torch flame is within 50 of rope 0's top; counted down, rope 0 burns off at 0 (mRopeFlags bit 1 off) */
    /* 0x626 */ be<s16> mRope1FireTimer;  /* was m3C2: rope_fire (in daBridge_Execute 020E11E8): as mRope0FireTimer for rope 1 (mRopeFlags bit 2) */
    /* 0x628 */ gptr<JPABaseEmitter> mpRope0FireEmitter;  /* was m3C4: rope_fire (in daBridge_Execute 020E11E8): the ID_AK_SN_BRIDGEROPEFIRE00 emitter of rope 0, moved to the rope top while mRope0FireTimer runs, invalidated at 0 */
    /* 0x62C */ gptr<JPABaseEmitter> mpRope1FireEmitter;  /* was m3C8: rope_fire (in daBridge_Execute 020E11E8): as mpRope0FireEmitter for rope 1 */
    /* 0x630 */ cXyz mCtrlPos;  /* was m3CC: control1 / control2 / cut_control1 / cut_control2 (in daBridge_Execute 020E11E8): the plank's simulated chain position (75 apart, from home.pos and mEndPos); bridge_move_normal copies it (blended) into mPosition; daBridge_IsDelete 020E37C4 / sounds use it */
    /* 0x63C */ cXyz mPosition;
    /* 0x648 */ csXyz mRotation;
    /* 0x64E */ be<s16> mRotationYExtra;
    /* 0x650 */ be<f32> mGroundY;  /* was m3EC: bridge_move (in daBridge_Execute 020E11E8): -10000 while broken; cut_control1 / cut_control2 keep the hanging plank at least 30 above it */
    /* 0x654 */ be<f32> mSwayRate;  /* was m3F0: bridge_move (in daBridge_Execute 020E11E8): |sin(pi * i / (count - 1))| (1 for chain bridges); control1 / control2 scale the plank's wave, droop and sideways swing by it, bridge_move_normal its ride tilt */
    /* 0x658 */ be<f32> mRideSag;  /* was m3F4: ride_call_back 020DF908: -31 (player, -40 heavy), -40 (MO2), -25 (Bokoblin), -10 (others), -300 for a breaking hit, plus the bounce; bridge_move_normal (in daBridge_Execute 020E11E8) eases mSagY of the planks around to it, breaks below -200; b_hang 020A5674 (bk) sets -25 */
    /* 0x65C */ be<f32> mSagY;  /* was m3F8: bridge_move_normal (in daBridge_Execute 020E11E8): eased to mRideSag (or -80 / -30 at cut ropes) * the plank influence, then back to 0; control1 / control2 add half of it to the plank height */
    /* 0x660 */ be<f32> mDroopY;  /* was m3FC: bridge_move_normal (in daBridge_Execute 020E11E8): eased to -15; control1 / control2 add half of it (* mSwayRate) to the plank height; bridge_move 0 when broken, then eased to -50 (cut_control1 adds it) */
    /* 0x664 */ be<s16> mRideRollTarget;  /* was m400: ride_call_back 020DF908: -(rider's local x) * k; bridge_move_normal (in daBridge_Execute 020E11E8) eases mRideRoll of the planks around to it, then 0; b_hang 020A5674 (bk) sets it (+-0x2000) */
    /* 0x666 */ be<s16> mRideRoll;  /* was m402: bridge_move_normal (in daBridge_Execute 020E11E8): eased to mRideRollTarget * influence * mSwayRate, then back to 0; mRotation.z = it + mCutRoll */
    /* 0x668 */ be<s16> mCutRoll;  /* was m404: bridge_move_normal (in daBridge_Execute 020E11E8): eased to +-7000 (one rope cut) * influence, plus the rope shake; mRotation.z = mRideRoll + it */
    /* 0x66A */ be<u8> mPlankRideTimer;  /* was m406: ride_call_back 020DF908: 2 when something rides the plank (b_hang 020A5674 sets 1); bridge_move_normal (in daBridge_Execute 020E11E8) applies mRideSag / mRideRollTarget while it is not 0 and counts it down */
    /* 0x66B */ be<u8> mGroundHitCount;  /* was m407: cut_control1 / cut_control2 (in daBridge_Execute 020E11E8): + 1 each time the hanging plank is held at mGroundY + 30; nothing reads it */
    /* 0x66C */ be<u8> mRopeFlags;  /* was m408: CallbackCreateHeap 020E3890: 7 on every 4th plank; bit 4 = the plank carries ropes / chains, bit 1 / 2 = rope 0 / 1 intact (cleared when cut or burnt, rope_hit / rope_fire); bridge_move_normal and daBridge_Draw 020DFF60 act on them */
    /* 0x66D */ u8 _66D[3];
    /* 0x670 */ cXyz mScale;
    /* 0x67C */ be<s16> mRopeTopTimer;  /* was m418: CallbackCreateHeap 020E3890: 0x32 (chain bridge) or -1 on a rope plank; daBridge_Execute 020E11E8 places the rope tops ([0] of mSide0Pos / mSide1Pos) while it is not 0 and counts a positive value down */
    /* 0x67E */ u8 _67E[2];
};
WWHD_OFFSET(br_s, mLineMat1, 0x1D4);
WWHD_OFFSET(br_s, mCyl, 0x3A4);
WWHD_OFFSET(br_s, mCtrlPos, 0x630);
WWHD_OFFSET(br_s, mRopeTopTimer, 0x67C);
WWHD_SIZE(br_s, 0x680);

struct bridge_class : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhase;
    /* 0x03B4 */ be<s16> mMoveProcMode;
    /* 0x03B6 */ u8 _3B6[2];
    /* 0x03B8 */ BridgeLineMaterial mLineMat;
    /* 0x0540 */ be<u8> mTypeBits;
    /* 0x0541 */ be<u8> mRopePhaseOfs;  /* was m02D9: daBridge_Create 020E4098: (param >> 8) & 0xFF; bridge_move (in daBridge_Execute 020E11E8) sets 0 in the first move mode; daBridge_Execute 020E11E8 adds it * 0x5DC to the phase of the side ropes' wind sway (mFrameCounter * 0x578) */
    /* 0x0542 */ be<u8> mPathId;
    /* 0x0543 */ be<u8> mPathIdP;
    /* 0x0544 */ be<s8> mBrCount;
    /* 0x0545 */ be<s8> mActiveBrCount;  /* was m02DD: daBridge_Create 020E4098: mBrCount, or 15 / 11 / 7 (by mBrCount) for a bridge with a partner (mTypeBits & 2); daBridge_Execute 020E11E8 hides the planks from it on (scale 0, no ropes) and builds the bg vertices of the first it planks */
    /* 0x0546 */ u8 _546[2];
    /* 0x0548 */ be<f32> mWavePower;  /* was m02E0: ride_call_back 020DF908: raised to the rider's movement * 0.3 (at most 20), 20 on a breaking hit; bridge_move_normal (in daBridge_Execute 020E11E8) copies it to mWaveAmpH / mWaveAmpV and eases it to 2 (wind above 0.1) or 0 */
    /* 0x054C */ be<f32> mSwingPower;  /* was m02E4: ride_call_back 020DF908: + 0.5 while below the rider's sideways movement (at most 50); bridge_move_normal (in daBridge_Execute 020E11E8) copies it to mSwingAmp and eases it to 0.6 (wind) or 0 */
    /* 0x0550 */ gptr<dBgW> mpBgW;
    /* 0x0554 */ be<s16> mWavePhaseH;  /* was m02EC: control1 (in daBridge_Execute 020E11E8): + mWaveStepH per frame; phase of the sideways plank wave (sin(it + i * k) * mWaveAmpH) and of the whole bridge's swing (cos(it) * mSwingAmp, also in bridge_move_normal) */
    /* 0x0556 */ be<s16> mWavePhaseV;  /* was m02EE: control1 (in daBridge_Execute 020E11E8): + mWaveStepV per frame; phase of the vertical plank wave (sin(it + i * (k + 1000)) * mWaveAmpV) */
    /* 0x0558 */ be<s16> mWaveStepH;  /* was m02F0: bridge_move_normal (in daBridge_Execute 020E11E8): 0x578 every frame; control1 adds it to mWavePhaseH */
    /* 0x055A */ be<s16> mWaveStepV;  /* was m02F2: bridge_move_normal (in daBridge_Execute 020E11E8): 3000 every frame; control1 adds it to mWavePhaseV */
    /* 0x055C */ be<f32> mWaveAmpH;  /* was m02F4: bridge_move_normal (in daBridge_Execute 020E11E8): mWavePower every frame; control1 (in daBridge_Execute 020E11E8): amplitude of the sideways plank wave */
    /* 0x0560 */ be<f32> mSwingAmp;  /* was m02F8: bridge_move_normal (in daBridge_Execute 020E11E8): mSwingPower every frame; amplitude of the whole bridge's sideways swing (cos(mWavePhaseH) * it, the first plank -2 * it); control1 uses it too */
    /* 0x0564 */ be<f32> mWaveAmpV;  /* was m02FC: bridge_move_normal (in daBridge_Execute 020E11E8): mWavePower every frame; control1 (in daBridge_Execute 020E11E8): amplitude of the vertical plank wave; ride_call_back 020DF908 adds sin(mAnmPhase) * it * 0.03 to the rider's plank sag */
    /* 0x0568 */ be<s16> mAnmPhase;  /* was m0300: bridge_move_normal (in daBridge_Execute 020E11E8): + 3000 per frame (+ 4000 broken, bridge_move); phase of the ride bounce (ride_call_back 020DF908), the cut-rope shake (* 4), the side rope sway (* 6) and in daBridge_Draw 020DFF60 the plank ropes (* 5), chain and the draw rotation */
    /* 0x056A */ be<s16> mFrameCounter;  /* was m0302: daBridge_Execute 020E11E8: + 1 every frame; the side ropes' wind sway phase is it * 0x578 + mRopePhaseOfs * 0x5DC */
    /* 0x056C */ be<s32> mBreakBrNo;  /* was m0304: bridge_move_normal (in daBridge_Execute 020E11E8): the plank where the bridge breaks (both ropes cut and overloaded, or sag below -200), then mMoveProcMode 4; cut_control1 / cut_control2 hang the two halves from it; daBridge_Execute 020E11E8 moves the planks either side of it apart */
    /* 0x0570 */ be<s32> mBreakCount;  /* was m0308: bridge_move_normal (in daBridge_Execute 020E11E8): + 2 (+ 8 more for the heavy player) per frame while something rides near a plank whose ropes are both cut, - 1 per frame, 0 during an event; above 100 the bridge breaks */
    /* 0x0574 */ be<s32> mSideRopeNum;  /* was m030C: daBridge_Execute 020E11E8: 0 at the start, + 1 per plank whose side ropes it writes (segment index it + 1); daBridge_Draw 020DFF60 updates the side rope lines with it (+ the end) segments */
    /* 0x0578 */ u8 _578[2];
    /* 0x057A */ be<s16> mBreakTimer;  /* was m0312: bridge_move (in daBridge_Execute 020E11E8): 50 when the bridge breaks (mMoveProcMode 4), counted down in mode 5 */
    /* 0x057C */ cXyz mEndPos;
    /* 0x0588 */ cXyz mEndRopePos1;  /* was m0320: daBridge_Execute 020E11E8: copy of m11C[0] of the last active plank (mActiveBrCount - 1) of a partnered bridge; the partner's daBridge_Draw 020DFF60 (side_rope_end, via mpAite) ends its side rope 1 there */
    /* 0x0594 */ cXyz mEndRopePos0;  /* was m032C: daBridge_Execute 020E11E8: copy of m0F8[0] of the last active plank of a partnered bridge; the partner's daBridge_Draw 020DFF60 (side_rope_end, via mpAite) ends its side rope 0 there */
    /* 0x05A0 */ gptr<bridge_class> mpAite;
    /* 0x05A4 */ be<s8> mRideTimer;  /* was m033C: ride_call_back 020DF908: 5 when the player stands on the bridge; daBridge_Execute 020E11E8 counts it down and does nothing during an event while it is not 0; bridge_move_normal shakes the controller on a break only while it is not 0 */
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
    gptr<cXyz> mpHangPos;
    u8 paddingC70[12];
    csXyz mHangAngle;
    be<s16> mBridgeRideTimer, mBridgeRideRoll;
    u8 paddingC86[2];
    be<u32> mHangBridgeId;
    gptr<br_s> mpBridge;  /* mpBridge was m0B2C: bk_class mpBridge (d_a_bk.h); mpHangPos, mHangAngle, mBridgeRideTimer, mBridgeRideRoll, mHangBridgeId were m79C, m7AC, m7B2, m7B4, m7B8: bk_class dr (damagereaction_l) fields, named after damagereaction (c_damagereaction.h) */
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
WWHD_OFFSET(BokoReaction, mHangBridgeId, 0xC88);
WWHD_OFFSET(BokoReaction, mpBridge, 0xC8C);
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
    /* 0x0C */ csXyz mHitAngle;  /* was m0C: the CcAtInfo copy of this unit; see d/d_cc_uty.h */
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
