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
    // HD: unknown member names follow TWW; their offsets move with the expanded actor base.

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
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    gptr<mDoExt_btkAnm> mpBtk;
    be<s32> m2A0;
    be<f32> m2A4[12], m2D4[12];
    be<u8> m304, m305;
    be<s16> m306;
    be<f32> m308;
    dCcD_Stts mStts;
    dCcD_Sph mSph;
    be<f32> m474[12];
    cXyz m4A4;
};
WWHD_OFFSET(Act_c, mpModel, 0x3B4);
WWHD_OFFSET(Act_c, m2A0, 0x3BC);
WWHD_OFFSET(Act_c, m308, 0x424);
WWHD_OFFSET(Act_c, m474, 0x590);
WWHD_OFFSET(Act_c, m4A4, 0x5C0);
} // namespace daTagLight
