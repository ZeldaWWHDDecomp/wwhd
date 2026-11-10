#pragma once
#include "SSystem/SComponent/c_m3d_g_pla.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
namespace daTagLight {
// HD: mapped global header, registration records and mutable material layout.
struct TagLightHeaderHD {
    be<f32> mPiMin, mPiMax;
    be<u8> mAngleByte, mAngleFlag;
    u8 _A[2];
    be<u32> mZero[4];
};
struct TagLightRegistrationHD {
    u8 mBytes[12];
};
struct TagLightRegistrationTableHD {
    TagLightRegistrationHD mRecords[3];
};
struct TagLightSinCosPairHD {
    be<f32> mSine, mCosine;
};
struct TagLightSinCosHD {
    TagLightSinCosPairHD mEntries[8192];
};
struct TagLightSphereSourceHD {
    u8 mBytes[0x40];
};
struct TagLightTevVtableHD {
    u8 _0[0x3C];
    be<u32> mSetKColor;
    u8 _40[0xC];
    be<u32> mGetKColor;
};
struct TagLightTevBlockHD {
    u8 _0[4];
    gptr<TagLightTevVtableHD> mVtable;
};
struct TagLightColorBlockHD {
    be<u32> mFlags;
};
struct TagLightMaterialHD {
    u8 _0[4];
    gptr<TagLightMaterialHD> mpNext;
    gptr<J3DShape> mpShape;
    u8 _C[0xC];
    gptr<TagLightTevBlockHD> mpTev;
    u8 _1C[0xA0 - 0x1C];
    TagLightColorBlockHD mColorBlock;
};
struct TagLightJointHD {
    u8 _0[0x10];
    gptr<TagLightMaterialHD> mpMesh;
};
inline TagLightMaterialHD *TagLight_jointMesh(J3DModelData *data, u16 index) {
    return reinterpret_cast<TagLightJointHD *>(data->getJointNodePointer(index))->mpMesh;
}
inline be<u8> *TagLight_getKColor(TagLightTevBlockHD *block, s32 index) {
    return gabi::call_ptr<be<u8> *>(block->mVtable->mGetKColor, block, index);
}
inline void TagLight_setKColor(TagLightTevBlockHD *block, s32 index, be<u8> *color) {
    gabi::call_ptr(block->mVtable->mSetKColor, block, index, color);
}
inline void *TagLight_detect() {
    return gabi::at<void>(dComIfGp_ea() + PLAY_DETECT);
}
// HD: keep every line-query base vtable and pass flag in native order.
struct TagLightLineCheckHD {
    gptr<be<u32>> mpPolyPass, mpGroupPass;
    u8 _8[8];
    be<u32> mCheckVtable;
    be<u16> mPolyIndex, mBgIndex;
    u8 _18[8];
    be<u32> mPolyInfoVtable;
    cXyz mStart, mEnd;
    u8 _3C[0x58 - 0x3C];
    be<u32> mPolyVtable;
    be<u8> mPass[8];
    be<u32> mGroupVtable, mGroup;
};
struct TagLightProjectionInfoLocalsHD {
    cXyz offsets[4], transformed, cross[4];
    TagLightLineCheckHD check;
};
WWHD_SIZE(TagLightLineCheckHD, 0x6C);
WWHD_OFFSET(TagLightLineCheckHD, mEnd, 0x30);
WWHD_OFFSET(TagLightMaterialHD, mColorBlock, 0xA0);
WWHD_SIZE(TagLightHeaderHD, 0x1C);

struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_TYPE_W = 2,
        PRM_TYPE_S = 0,
        PRM_SWSAVE2_W = 8,
        PRM_SWSAVE2_S = 2,
        PRM_CCR_W = 2,
        PRM_CCR_S = 8,
        PRM_CONERATIO_W = 4,
        PRM_CONERATIO_S = 10,
        PRM_FADETYPE_W = 2,
        PRM_FADETYPE_S = 14,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 16,
        PRM_SCH_W = 8,
        PRM_SCH_S = 24
    };
    enum TypeHD { Type_Box, Type_Cone, Type_Detector };
    // HD: offsets move with the expanded actor base; the former TWW offset names are kept in the was-comments.

    void get_base_model_light_start_offset(cXyz *out);
    u8 chk_sw();
    u8 chk_sch();
    f32 get_cc_r();
    u8 _delete();
    u32 chk_light();
    u8 _execute();
    u8 create_heap();
    void set_spot_mtx();
    void set_spot();
    void init_spot();
    u8 _draw();
    s32 _create();
    void make_projection_mtx(const cXyz *point, const cXyz *normal, const cXyz *center, const cXyz *start);
    void get_projection_info(cXyz *point, cXyz *normal, cXyz *center, const cXyz *start, const cXyz *end);
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x3BC */ be<s32> mType;  /* was m2A0: _create 024ADB04: prm type (tww prm_get_type(); Type_Box / Type_Cone / Type_Detector); _execute 024AE0E8 / _draw 024AE430 / _delete 024AE044 branch on it */
    /* 0x3C0 */ be<f32> mMtx[12];  /* was m2A4: _create 024ADB04: copy of mDoMtx_stack_c (tww cMtx_copy), given to fopAcM_SetMtx in tww; set_spot_mtx 024AD894 / get_projection_info 024AD198 transform local points with it */
    /* 0x3F0 */ be<f32> mInvMtx[12];  /* was m2D4: _create 024ADB04: MTXInverse(mMtx, it) */
    /* 0x420 */ be<u8> mLightOn;  /* was m304: _create 024ADB04 / _execute 024AE0E8: chk_sw() && chk_sch() (tww), false for Type_Detector; picks the fade target of mAlpha */
    /* 0x421 */ be<u8> mProjMtxValid;  /* was m305: make_projection_mtx 024AD594: 0 when the cone angle's cosine is about 0 (no projection), else 1; tww _draw draws the light model only when set */
    /* 0x422 */ be<s16> mLitCount;  /* was m306: _execute 024AE0E8 (Type_Detector): +1 while chk_light(), else -1, limited to [0, 20]; above 10 it turns the switch on (tww); _create 024ADB04 0 */
    /* 0x424 */ be<f32> mAlpha;  /* was m308: _execute 024AE0E8: cLib_chaseF toward mLightOn ? 1 : 0 at M_alpha_spd[fadeType]; _draw 024AE430 draws the cone only above 0, alpha it * 255.5; _create 024ADB04 1 / 0 */
    /* 0x428 */ dCcD_Stts mStts;
    /* 0x464 */ dCcD_Sph mSph;
    /* 0x590 */ be<f32> mProjMtx[12];  /* was m474: make_projection_mtx 024AD594: copy of mDoMtx_stack_c (the light's projection matrix); tww _draw passes it to dComIfGd_setLightModel */
    /* 0x5C0 */ cXyz mSize;  /* was m4A4: _create 024ADB04: the scale-derived size (tww: scale with x/y/z doubled per type); make_projection_mtx 024AD594 / get_projection_info 024AD198 use its x and z as the light's width and depth */
};
WWHD_OFFSET(Act_c, mpModel, 0x3B4);
WWHD_OFFSET(Act_c, mType, 0x3BC);
WWHD_OFFSET(Act_c, mAlpha, 0x424);
WWHD_OFFSET(Act_c, mProjMtx, 0x590);
WWHD_OFFSET(Act_c, mSize, 0x5C0);
} // namespace daTagLight
