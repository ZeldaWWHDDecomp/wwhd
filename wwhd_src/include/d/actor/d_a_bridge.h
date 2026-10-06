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
#define BRIDGE_WIND_VEC 0x10462944
#define BRIDGE_WP 0x10462948
#define BRIDGE_WY 0x10462954

enum {
    fpcNm_PLAYER_e_hd = 0xA8,
    fpcNm_BRIDGE_e_hd = 0x59,
    fpcNm_MO2_e_hd = 0xBC,
    fpcNm_BK_e_hd = 0xBD,
    fpcNm_BOMB_e_hd = 0x126,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* mDoExt_3DlineMat1_c (HD 0x188; GameCube 0x3C): vtable at +0x130, mpLines at +0x184; a line is
 * {cXyz* mpSegments; u8* mpSize; ...}. Constructor 025EB82C, destructor 025EB8B8 (the matcher
 * names it mDoExt_3DlineMat1_c::draw), init 025EBA58, update 025ED1BC (as d_a_sss). */
struct mDoExt_3DlineMat1_l { u8 _[0x188]; };
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_l* l, u16 numLines, u16 numSegs, void* tex, BOOL hasSize) {
    return gabi::call<BOOL>(0x025EBA58, l, numLines, numSegs, tex, hasSize);
}
static inline void mDoExt_3DlineMat1_ct(mDoExt_3DlineMat1_l* l) { gabi::call(0x025EB82C, l); }
static inline void mDoExt_3DlineMat1_dt(mDoExt_3DlineMat1_l* l, s32 flags) { gabi::call(0x025EB8B8, l, flags); }
/* GHS array helpers: __construct_array(ptr, n, size, ctor), __destroy_arr(ptr, n, size, dtor, flags) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
/* dBgWSv (HD): `new dBgWSv()` is the constructor 024F5A18 with this == NULL; Set 024F5A78;
 * CopyBackVtx 024F5B08; ride callback at +0xB0; vertex table at +0x90, vertex count *(+0x94) */
static inline dBgW* dBgWSv_new() { return gabi::call<dBgW*>(0x024F5A18, 0); }
static inline BOOL dBgWSv_Set(dBgW* w, void* bgd, u32 flag) { return gabi::call<BOOL>(0x024F5A78, w, bgd, flag); }
static inline void dBgWSv_CopyBackVtx(dBgW* w) { gabi::call(0x024F5B08, w); }
/* 020CB648 daBomb_c::getBombRestTime */
static inline s32 daBomb_getBombRestTime(void* b) { return gabi::call<s32>(0x020CB648, b); }
/* fopAcM_GetName: base_process_class::mProcName (s16 at +8) */
static inline s16 fopAcM_GetName_l(void* p) { return gabi::load<s16>(gabi::ea(p) + 8); }

/* ---- layout ---- */
struct br_s {
    /* 0x000 */ gptr<J3DModel> mpModel;
    /* 0x004 */ dKy_tevstr_c mTevStr;
    /* 0x1CC */ gptr<J3DModel> mpModelRope1;
    /* 0x1D0 */ gptr<J3DModel> mpModelRope0;
    /* 0x1D4 */ mDoExt_3DlineMat1_l mLineMat1;
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
    /* 0x03B8 */ mDoExt_3DlineMat1_l mLineMat;
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
