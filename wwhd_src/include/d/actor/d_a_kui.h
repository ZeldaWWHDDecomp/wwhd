/** Hookable posts and bell. HD actor fields are GameCube offsets +0x11C. */
#pragma once
#include "gc_api/d/d_kankyo_rain.h"
#include "gc_api/gc_api.h"
struct KuiCameraView;

#define KUI_VTBL 0x100138FC /* kui_class vtable (HD virtual destructor) */

enum { fpcNm_DR2_e = 0xDF, fpcNm_BTD_e = 0xEA };
enum { JA_SE_OBJ_ROPE_SW_ON = 0x69D2, JA_SE_OBJ_ST_CHIME = 0x69D3, JA_SE_READ_RIDDLE_1 = 0x806 };

#define REG_S_(c, i) REG_S(c, i)

// HD: bindings and partial views used only by this translation unit.
/* HD: fopAcM_SetMin / fopAcM_SetMax are out of line (025D672C / 025D673C) */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D672C, a, x, y, z);
}
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D673C, a, x, y, z);
}
/* HD fopAcM_seStartCurrent inline where the actor is known non-null (only &current.pos is checked)
 */
static inline void fopAcM_seStartCurrent_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 025E1988 mDoAud_seStart(id) (HD: no position, reads r3 only) */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
/* dVibration_c::StartShock(int, int, cXyz) with cXyz(0, 1, 0); the play object is fetched first,
 * then the strength (REG0_S(2) + 5 when reg, else 3) */
static inline void StartShock_up(bool reg) {
    dVibration_c* vibration = dComIfGp_getVibration();
    s32 s = reg ? REG0_S(2) + 5 : 3;
    gabi::Local<cXyz> up;
    up->set(0.0f, 1.0f, 0.0f);
    vibration->StartShock(s, -0x21, up.get());
}
/* camera_class: dComIfGp_getCamera(id) at play+0x5AF8 + id*0x34; its dCamera_c at +0x248; eye at
 * +0xDC */
static inline KuiCameraView* dCam_getCamera() { return gabi::call<KuiCameraView*>(0x024F8020); }
/* Reset / Set take two cXyz by value (pointers to copies) */
static inline BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 p2, u16 p3) {
    return gabi::call<BOOL>(0x025D7B24, a, flag, p2, p3);
}
/* 027EC9E8 JUTReport(x, y, fmt, ...) */
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) {
    gabi::call(0x027EC9E8, x, y, fmt, v);
}
struct KuiBackgroundView : dBgW {
    u8 _04[0xA8 - 4];
    be<u32> mpCrrFunc;
    void SetCrrFunc(u32 fn) { mpCrrFunc = fn; }
};
WWHD_OFFSET(KuiBackgroundView, mpCrrFunc, 0xA8);
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) {
    static_cast<KuiBackgroundView*>(w)->SetCrrFunc(fn);
}
inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) {
    gabi::call(0x028E9108, a, b, ab);
}
/* float copy of a cXyz through FPRs (GHS struct copies used for computation) */
static inline void cXyz_fcopy(cXyz* d, const cXyz* s) {
    f32 x = s->x, y = s->y, z = s->z;
    d->x = x;
    d->y = y;
    d->z = z;
}

struct kui_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel2;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> type;
    /* 0x3BD */ be<u8> mScaleType;  /* was field_0x2A1: daKui_Create 021ADD6C: parameter bits 8-15 (4 for type 3); its switch sets scale (0: 0.5 x/y, 1: 2 x/y, 2: 2 z, 3, 4: 2, 5: 4/2/4 ...) */
    /* 0x3BE */ be<u8> mOnDragonTail;  /* was field_0x2A2: daKui_Create 021ADD6C: parameter bits 4-7; daKui_Execute 021ACB0C: when set the post follows the dragon tail (dr2_class unk_3B0, found with s_a_i_sub / b_a_i_sub) and its collision matrix is scaled by 4 */
    /* 0x3BF */ be<u8> mSwitchNo;
    /* 0x3C0 */ u8 _3C0[4];
    /* 0x3C4 */ Mtx34 mBgMtx;  /* was field_0x2A8: daKui_CreateHeap 021ADB48: the matrix cBgW_Set gives mpBgW; daKui_Execute 021ACB0C and kui_bell (in daKui_Execute) fill it before dBgW_Move */
    /* 0x3F4 */ gptr<dBgW> mpBgW;  /* was field_0x2D8: daKui_CreateHeap 021ADB48: the created dBgW (cBgW_Set, dBgW_SetCrrFunc); daKui_Create 021ADD6C registers it (dBgS_Regist), daKui_Delete 021ADAFC releases it, daKui_Execute 021ACB0C moves it (dBgW_Move) */
    /* 0x3F8 */ be<s16> mBellTimers[3];  /* was field_0x2DC: kui_bell (in daKui_Execute 021ACB0C): [1] / [2] = 0x50 when the hooked bell hits its limit on that side (no new ring while set), [0] = 40 starts the ring: its countdown drives the shake and turns mSwitchNo on at 0; all counted down */
    /* 0x3FE */ be<s16> mSwingAngle;  /* was field_0x2E2: kui_bell (in daKui_Execute 021ACB0C): eased to mSwingTarget at mSwingStep; added to the bell model's X rotation */
    /* 0x400 */ be<s16> mSwingTarget;  /* was field_0x2E4: kui_bell (in daKui_Execute 021ACB0C): bure_xa_d[side] (+-0x1000) on a ring, eased back to 0; target of mSwingAngle */
    /* 0x402 */ be<s16> mSwingStep;  /* was field_0x2E6: kui_bell (in daKui_Execute 021ACB0C): 0x400 on a ring, eased to 0x100; step of mSwingAngle */
    /* 0x404 */ be<s8> mDemoMode;  /* was field_0x2E8: demo_camera (in daKui_Execute 021ACB0C): its switch (0 none, 1 order the event, 2 run the camera); daKui_Execute sets 1 when mSwitchTimer reaches 970 (type 2); 0 at the end (mDemoTimer 70) */
    /* 0x405 */ u8 _405;
    /* 0x406 */ be<s16> mDemoTimer;  /* was field_0x2EA: demo_camera (in daKui_Execute 021ACB0C): 0 at the demo start, +1 per frame (K SUB COUNT report); drives the post's sink (mDemoOffsetY) and ends the demo at 70 */
    /* 0x408 */ cXyz mDemoCamEye;  /* was field_0x2EC: demo_camera (in daKui_Execute 021ACB0C): home.pos + the rotated (800, 0, 100); the eye passed to the camera Set / Reset (KuiCameraControl Set(center, eye)) */
    /* 0x414 */ cXyz mDemoCamCenter;  /* was field_0x2F8: demo_camera (in daKui_Execute 021ACB0C): home.pos 200 higher; the center passed to the camera Set / Reset (KuiCameraControl Set(center, eye)) */
    /* 0x420 */ u8 _420[4];
    /* 0x424 */ be<s16> mSwitchTimer;  /* was field_0x308: daKui_Execute 021ACB0C (types 2 and 4): 1000 when health is 3 and the switch is off, counted down; at 970 type 2 starts the demo (mDemoMode 1), type 4 turns mSwitchNo on */
    /* 0x426 */ u8 _426[2];
    /* 0x428 */ be<f32> mDemoOffsetY;  /* was field_0x30C: demo_camera (in daKui_Execute 021ACB0C): current.pos.y = home.pos.y + it; a small sine shake, then eased to -70 (the post sinks), then a shake around -70 */
};
WWHD_OFFSET(kui_class, mBgMtx, 0x3C4);
WWHD_OFFSET(kui_class, mpBgW, 0x3F4);
WWHD_OFFSET(kui_class, mDemoCamEye, 0x408);
WWHD_OFFSET(kui_class, mDemoOffsetY, 0x428);
WWHD_SIZE(kui_class, 0x42C);

// HD: renderer instances have a relative texture table and per-slot dirty bits.
struct KuiTextureEntry {
    u8 _00[4];
    be<s32> mIndex;
    u8 _08[4];
    be<u16> mParentIndex;
    u8 _0E[6];
};
struct KuiMaterialData {
    u8 _00[0x34];
    be<s32> mTextureOffset;
    KuiTextureEntry* textureTable() {
        s32 offset = mTextureOffset;
        return gabi::at<KuiTextureEntry>(offset ? gabi::ea(&mTextureOffset) + offset : 0);
    }
};
struct KuiTextureInfo {
    be<u32> mInfo;
};
struct KuiMaterialInstance {
    gptr<KuiMaterialData> mpData;
    be<u16> mFlags;
    u8 _06[6];
    gptr<be<u32>> mDirtyBits;
    u8 _10[0x2C];
    KuiTextureInfo* getTexMtx(u32 index, be<s32>* slot) {
        return gabi::call<KuiTextureInfo*>(0x027FA974, this, index, slot);
    }
    gptr<Mtx34>* getEffectSlot(u32 index) {
        return gabi::call<gptr<Mtx34>*>(0x027FA678, this, index);
    }
    void markDirty(s32 index) {
        mFlags |= 4;
        mDirtyBits.get()[index >> 5] |= 1u << (index & 31);
    }
};
struct KuiModelView {
    u8 _00[0x2A];
    be<u16> mMaterialNum;
    u8 _2C[8];
    gptr<KuiMaterialInstance> mMaterials;
    u8 _38[0xF8 - 0x38];
    Mtx34 mEffectMtx;
};
struct KuiCameraView {
    u8 _00[0xDC];
    cXyz mEye;
};
WWHD_SIZE(KuiTextureEntry, 0x14);
WWHD_SIZE(KuiMaterialInstance, 0x3C);
WWHD_OFFSET(KuiModelView, mEffectMtx, 0xF8);
WWHD_OFFSET(KuiModelView, mMaterials, 0x34);
WWHD_OFFSET(KuiCameraView, mEye, 0xDC);
void C_VECHalfAngle(const cXyz*, const cXyz*, cXyz*);
void C_MTXLookAt(Mtx34*, const cXyz*, const cXyz*, const cXyz*);

// HD: BaseY lives in the common-header BSS vector storage.
inline cXyz& kui_baseY = *gabi::at<cXyz>(0x101FFBC0);

struct KuiCameraControl {
    void Stop() { gabi::call(0x02514F2C, this); }
    void Start() { gabi::call(0x02514F38, this); }
    void SetTrimSize(s32 size) { gabi::call(0x02515280, this, size); }
    void Reset(cXyz* center, cXyz* eye) { gabi::call(0x0251510C, this, center, eye); }
    void Set(cXyz* center, cXyz* eye) { gabi::call(0x02514F50, this, center, eye); }
};
struct KuiCameraProcessView {
    u8 _00[0x248];
    KuiCameraControl mCamera;
};
struct KuiDragonTailView : fopAc_ac_c {
    u8 _3AC[0x4CC - 0x3AC];
    cXyz mHookTargetPos; /* was unk_3B0: dr2_class mHookTargetPos (0x4CC); daKui_Execute 021ACB0C puts the hookshot target there */
    u8 _4D8[0x5D6 - 0x4D8];
    be<s16> mRockState; /* was unk_4BA: dr2_class mRockState (0x5D6); daKui_Execute 021ACB0C follows the tail only at 0 or from 10 on */
};
struct KuiBtdView : fopAc_ac_c {
    u8 _3AC[0x40C - 0x3AC];
    be<s16> mAction;
};
WWHD_OFFSET(KuiCameraProcessView, mCamera, 0x248);
WWHD_OFFSET(KuiDragonTailView, mHookTargetPos, 0x4CC);
WWHD_OFFSET(KuiDragonTailView, mRockState, 0x5D6);
WWHD_OFFSET(KuiBtdView, mAction, 0x40C);

enum {
    dRes_INDEX_KUI_BDL_HKANE1_e = 4,
    dRes_INDEX_KUI_BDL_HKANE2_e = 5,
    dRes_INDEX_KUI_BDL_MROPESW_e = 6,
    dRes_INDEX_KUI_BDL_OBI_ROPETAG_e = 7,
    dRes_INDEX_KUI_DZB_OBI_ROPETAG_e = 10,
    dEvtFlag_STAFF_ALL_e = 2,
};

WWHD_OFFSET(KuiMaterialData, mTextureOffset, 0x34);
WWHD_OFFSET(KuiMaterialInstance, mFlags, 4);
WWHD_OFFSET(KuiMaterialInstance, mDirtyBits, 0xC);
WWHD_OFFSET(KuiModelView, mMaterialNum, 0x2A);
WWHD_SIZE(KuiModelView, 0x128);
