/* Hyrule Castle knight statues. HD omits the ground-check and shadow members.
 * Retail member names are retained; animation/collision offsets reflect the HD prefix. */
#pragma once
#include "gc_api/gc_api.h"
static inline s32 zouk_daObj_PrmAbstract_i(fopAc_ac_c *, s32, s32);
namespace daObjZouk {
struct Act_c : fopAc_ac_c {
    enum BgMode { BEFORE, AFTER };
    s32 param_get_arg0() { return zouk_daObj_PrmAbstract_i(this, 0x04, 0x00); }
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void texture_scroll() {}
    u32 play_stop_joint_anime();
    BOOL jokai_demo();
    bool _execute();
    bool _draw();

    /* 0x3AC */ be<u32> field_0x294;
    /* 0x3B0 */ be<u32> field_0x298;
    /* 0x3B4 */ request_of_phase_process_class mPhs;
    /* 0x3BC */ Mtx34 mBgMtx;
    /* 0x3EC */ Mtx34 mMtx;
    /* 0x41C */ gptr<J3DAnmTransform> M_bck_data;
    /* 0x420 */ gptr<mDoExt_McaMorf> M_anm;
    /* 0x424 */ be<s32> mBgMode;
    /* 0x428 */ gptr<dBgW> mBgBefore;
    /* 0x42C */ gptr<dBgW> mBgAfter;
    /* 0x430 */ dCcD_Stts mStts0;
    /* 0x46C */ dCcD_Cyl mCyl0;
    /* 0x59C */ dCcD_Stts mStts1;
    /* 0x5D8 */ dCcD_Cyl mCyl1;
    /* 0x708 */ dCcD_Stts mStts2;
    /* 0x744 */ dCcD_Cyl mCyl2;
};
WWHD_OFFSET(Act_c, field_0x294, 0x3AC);
WWHD_OFFSET(Act_c, field_0x298, 0x3B0);
WWHD_OFFSET(Act_c, mPhs, 0x3B4);
WWHD_OFFSET(Act_c, mBgMtx, 0x3BC);
WWHD_OFFSET(Act_c, mMtx, 0x3EC);
WWHD_OFFSET(Act_c, M_bck_data, 0x41C);
WWHD_OFFSET(Act_c, M_anm, 0x420);
WWHD_OFFSET(Act_c, mBgMode, 0x424);
WWHD_OFFSET(Act_c, mBgBefore, 0x428);
WWHD_OFFSET(Act_c, mBgAfter, 0x42C);
WWHD_OFFSET(Act_c, mStts0, 0x430);
WWHD_OFFSET(Act_c, mCyl0, 0x46C);
WWHD_OFFSET(Act_c, mStts1, 0x59C);
WWHD_OFFSET(Act_c, mCyl1, 0x5D8);
WWHD_OFFSET(Act_c, mStts2, 0x708);
WWHD_OFFSET(Act_c, mCyl2, 0x744);
WWHD_SIZE(Act_c, 0x874);
} // namespace daObjZouk
struct ZoukBgW {
    u8 _00[0xA8];
    be<u32> mCrrFunc;
};
WWHD_OFFSET(ZoukBgW, mCrrFunc, 0xA8);
WWHD_SIZE(ZoukBgW, 0xAC);
struct ZoukCamera {
    u8 _00[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(ZoukCamera, mEye, 0xDC);
WWHD_SIZE(ZoukCamera, 0xE8);
struct ZoukFrameVtable {
    u8 _00[0x14];
    gfn mGetFrameMax;
};
WWHD_OFFSET(ZoukFrameVtable, mGetFrameMax, 0x14);
WWHD_SIZE(ZoukFrameVtable, 0x18);
struct ZoukFrameData {
    u8 _00[4];
    gptr<ZoukFrameVtable> mVtable;
};
WWHD_OFFSET(ZoukFrameData, mVtable, 4);
WWHD_SIZE(ZoukFrameData, 8);
struct ZoukTextureEntry {
    u8 _00[4];
    be<s32> mIndex;
    u8 _08[4];
    be<u16> mReference;
    u8 _0E[6];
};
WWHD_OFFSET(ZoukTextureEntry, mIndex, 4);
WWHD_OFFSET(ZoukTextureEntry, mReference, 0xC);
WWHD_SIZE(ZoukTextureEntry, 0x14);
struct ZoukMaterialResource {
    u8 _00[0x34];
    be<s32> mTextureTableOffset;
};
WWHD_OFFSET(ZoukMaterialResource, mTextureTableOffset, 0x34);
WWHD_SIZE(ZoukMaterialResource, 0x38);
struct ZoukMaterial {
    gptr<ZoukMaterialResource> mResource;
    be<u16> mFlags;
    u8 _06[6];
    gptr<be<u32>> mDirtyBits;
    u8 _10[0x2C];
};
WWHD_OFFSET(ZoukMaterial, mFlags, 4);
WWHD_OFFSET(ZoukMaterial, mDirtyBits, 0xC);
WWHD_SIZE(ZoukMaterial, 0x3C);
struct ZoukModel {
    u8 _00[0x2A];
    be<u16> mMaterialNum;
    u8 _2C[8];
    gptr<ZoukMaterial> mMaterials;
    u8 _38[0xF8 - 0x38];
    Mtx34 mEffectMtx;
};
WWHD_OFFSET(ZoukModel, mMaterialNum, 0x2A);
WWHD_OFFSET(ZoukModel, mMaterials, 0x34);
WWHD_OFFSET(ZoukModel, mEffectMtx, 0xF8);
WWHD_SIZE(ZoukModel, 0x128);
struct ZoukTexMtx {
    be<s32> mType;
};
WWHD_SIZE(ZoukTexMtx, 4);
struct ZoukText {
    char bytes[1];
};
WWHD_SIZE(ZoukText, 1);
enum { ACT_VTBL = 0x10033C30, AAB_VTBL = 0x10033C20, MOVE_BG = 1, TEX_EFFECT = 11, TEX_DIRTY = 4 };
enum {
    dRes_INDEX_VZOUK_BCK_VZOUK_e = 5,
    dRes_INDEX_VZOUK_BDL_VZOUK_e = 8,
    dRes_INDEX_VZOUK_DZB_MAEKISI_e = 0xB,
    dRes_INDEX_VZOUK_DZB_ATOKISI_e = 0xC,
};

/* 02349DFC daObj::PrmAbstract<int>(actor, width, shift) (shared out-of-line copy) */
static inline s32 zouk_daObj_PrmAbstract_i(fopAc_ac_c *a, s32 w, s32 s) {
    constexpr u32 callTarget = 0x02349DFC;
    return gabi::call<s32>(callTarget, a, w, s);
}
/* 025B7A2C dSv_player_collect_c::isCollect(int, u8) on save+0xD4 */

/* 025D69FC zouk_fopAcM_rollPlayerCrash(actor, f32 dist, u32 flags) */
static inline void zouk_fopAcM_rollPlayerCrash(fopAc_ac_c *a, f32 d, u32 f) {
    constexpr u32 callTarget = 0x025D69FC;
    gabi::call(callTarget, a, d, f);
}
/* dComIfGp_event_runCheck(): play byte +0x5292 */

/* dBgW::SetCrrFunc: +0xA8 */
static inline void zouk_dBgW_SetCrrFunc(dBgW *w, u32 f) {
    reinterpret_cast<ZoukBgW *>(w)->mCrrFunc = f;
}
/* 02527028 zouk_dDemo_setDemoData(actor, flags, morf, arc, n, ids, HD p6, HD p7) */
static inline BOOL zouk_dDemo_setDemoData(fopAc_ac_c *a, u8 flags, mDoExt_McaMorf *m,
                                          const char *arc, s32 n, u32 ids, u32 p6, s8 p7) {
    constexpr u32 callTarget = 0x02527028;
    return gabi::call<BOOL>(callTarget, a, flags, m, arc, n, ids, p6, p7);
}
/* zouk_dComIfGp_demo_getActor(id) (HD inline): id 0 or above 0x20 gives NULL; the demo object at
 * *0x101D5FFC (asserted), 02526E70 dDemo_object_c::getActor(u8) */
static inline void *zouk_dComIfGp_demo_getActor(u8 id) {
    if (id == 0 || id > 0x20)
        return nullptr;
    u32 obj = (u32)*gabi::at<be<u32>>(0x101D5FFC);
    if (obj == 0) { /* JUT_ASSERT(0x23a, ...) */
        JUT_ASSERT_fail(STR(0x10033C50), 0x23A, STR(0x10033C40));
        obj = (u32)*gabi::at<be<u32>>(0x101D5FFC);
    }
    constexpr u32 callTarget = 0x02526E70;
    return gabi::call<void *>(callTarget, obj, id);
}
/* matrix / vector helpers */
static inline void zouk_PSMTXScale(Mtx34 *m, f32 x, f32 y, f32 z) {
    constexpr u32 callTarget = 0x028E945C;
    gabi::call(callTarget, m, x, y, z);
}
static inline void zouk_PSMTXConcat(const Mtx34 *a, const Mtx34 *b, Mtx34 *ab) {
    constexpr u32 callTarget = 0x028E9108;
    gabi::call(callTarget, a, b, ab);
}
static inline void zouk_C_VECHalfAngle(const cXyz *a, const cXyz *b, cXyz *h) {
    constexpr u32 callTarget = 0x028E9E88;
    gabi::call(callTarget, a, b, h);
}
static inline void zouk_C_MTXLookAt(Mtx34 *m, const cXyz *eye, const cXyz *up, const cXyz *target) {
    constexpr u32 callTarget = 0x028E9684;
    gabi::call(callTarget, m, eye, up, target);
}
static inline void zouk_dKyr_get_vectle_calc(const cXyz *a, const cXyz *b, cXyz *out) {
    constexpr u32 callTarget = 0x02563F64;
    gabi::call(callTarget, a, b, out);
}
static inline u32 zouk_dCam_getCamera() {
    constexpr u32 callTarget = 0x024F8020;
    return gabi::call<u32>(callTarget);
}
/* HD J3D material helpers: 027FA974 texMtx(j, &idx), 027FA678 effect-matrix slot(j) */
static inline u32 zouk_J3DMat_getTexMtx(u32 mat, u32 j, be<s32> *idx) {
    constexpr u32 callTarget = 0x027FA974;
    return gabi::call<u32>(callTarget, mat, j, idx);
}
static inline u32 zouk_J3DMat_getEffectMtxSlot(u32 mat, u32 j) {
    constexpr u32 callTarget = 0x027FA678;
    return gabi::call<u32>(callTarget, mat, j);
}

inline u32 zouk_createHeapTail(fopAc_ac_c *p) {
    constexpr u32 target = 0x023BF1FC;
    return gabi::call<u32>(target, p);
}
inline u32 zouk_setBg(dBgW *p, cBgD_t *d, u32 f, Mtx34 *m) {
    constexpr u32 target = 0x0200A030;
    return gabi::call<u32>(target, p, d, f, m);
}
inline void zouk_destroyActor(fopAc_ac_c *p, s32 f) {
    constexpr u32 target = 0x025D50BC;
    gabi::call(target, p, f);
}
