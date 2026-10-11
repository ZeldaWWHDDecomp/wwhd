/* Unused chandelier: tww actor fields shift by +0x11C in HD; size 0x6D4.
 * Private views preserve the verified HD joint and particle access order. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_SYAN_BCK_SYAN_e = 4, dRes_INDEX_SYAN_BDL_SYAN_e = 8,
       SYAN_JNT_JOINT5_e = 2, SYAN_JNT_JOINT8_e = 5 };
namespace dPa_name { enum { ID_AK_JN_TORCH = 0x1EA, ID_AK_JP_O_KAGEROU00 = 0x4004 }; }
struct syan_class : fopAc_ac_c {
    enum { kEmtrNum = 6 };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> morf;
    /* 0x3B8 */ be<s32> frameCount;
    /* 0x3BC */ be<u32> state;
    /* 0x3C0 */ csXyz field_0x2a4[2];
    /* 0x3CC */ csXyz field_0x2b0;
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ be<f32> field_0x2b8;
    /* 0x3D8 */ be<f32> field_0x2bc;
    /* 0x3DC */ be<f32> field_0x2c0;
    /* 0x3E0 */ be<f32> field_0x2c4;
    /* 0x3E4 */ be<f32> field_0x2c8;
    /* 0x3E8 */ be<f32> field_0x2cc;
    /* 0x3EC */ be<f32> field_0x2d0;
    /* 0x3F0 */ be<s16> timer[2];
    /* 0x3F4 */ be<f32> field_0x2d8;
    /* 0x3F8 */ be<f32> field_0x2dc;
    /* 0x3FC */ Mtx34 partMtx[kEmtrNum];
    /* 0x51C */ cXyz partPos[kEmtrNum];
    /* 0x564 */ cXyz partPosOld[kEmtrNum];
    /* 0x5AC */ be<s16> partRotY[kEmtrNum];
    /* 0x5B8 */ dPa_followEcallBack emtrCallBack[kEmtrNum];
    /* 0x630 */ be<f32> partAlpha[kEmtrNum];
    /* 0x648 */ be<f32> field_0x52c[kEmtrNum];
    /* 0x660 */ be<f32> partScale1[kEmtrNum];
    /* 0x678 */ be<f32> field_0x55c[kEmtrNum];
    /* 0x690 */ be<f32> partScale2[kEmtrNum];
    /* 0x6A8 */ be<s16> field_0x58c[kEmtrNum];
    /* 0x6B4 */ be<s16> field_0x598[kEmtrNum];
    /* 0x6C0 */ be<s16> field_0x5a4[kEmtrNum];
    /* 0x6CC */ be<u8> emtrEnabled[kEmtrNum];
    /* 0x6D2 */ u8 _6D2[2];
};
WWHD_OFFSET(syan_class, field_0x2b8, 0x3D4);
WWHD_OFFSET(syan_class, partMtx, 0x3FC);
WWHD_OFFSET(syan_class, emtrCallBack, 0x5B8);
WWHD_OFFSET(syan_class, emtrEnabled, 0x6CC);
WWHD_SIZE(syan_class, 0x6D4);


struct SyanJointData {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_OFFSET(SyanJointData, mJntNo, 4);
struct SyanModel : public J3DModel {
    // HD: capture all three scale components before writing any model component.
    void setBaseScale(const cXyz& s) {
        f32 x = s.x, y = s.y, z = s.z;
        mBaseScale.x = x; mBaseScale.y = y; mBaseScale.z = z;
    }
};
struct SyanEmitter {
    /* 0x000 */ u8 _00[0x238];
    /* 0x238 */ cXyz mGlobalParticleScale;
    void setGlobalParticleScale(f32 x, f32 y, f32 z) {
        mGlobalParticleScale.x = x; mGlobalParticleScale.y = y; mGlobalParticleScale.z = z;
    }
};
WWHD_OFFSET(SyanEmitter, mGlobalParticleScale, 0x238);
// HD: existing GHS array construction/destruction bindings retain the element callbacks.
inline void syan_constructArray(void* p, u32 n, u32 size, void (*ctor)(dPa_followEcallBack*)) {
    gabi::call(0x028EFFD0, p, n, size, gabi::guest_fn((const void*)ctor));
}
inline void syan_destroyArray(void* p, u32 n, u32 size, void (*dtor)(void*, s32), u32 flags) {
    gabi::call(0x028F0164, p, n, size, gabi::guest_fn((const void*)dtor), flags);
}
void syan_lYrotM(s16);
void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32);
