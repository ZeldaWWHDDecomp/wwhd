/* Swaying lily-pad lift.
 * HD: actor fields move by 0x11C; CPU vertex deformation adds an eight-byte member. */
#pragma once
#include "gc_api/gc_api.h"
namespace Sk2 {
// HD: resolved joint data, after J3DNode_toJoint has already called the accessor.
struct JointData {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(JointData, mJntNo, 4);
struct ResourceIndices {
    be<s32> indices[2];
};
WWHD_SIZE(ResourceIndices, 8);
struct CpuVertices {
    gptr<void> mpData;
    gptr<cXyz> mpVtx;
};
struct dBgWDeform : dBgW {
    be<u32> __vtbl;
    u8 mDeformState[0x88];
    gptr<cXyz> mpVtx;
    u8 mDeformTail[0x30];
    void MoveAfterAnmCalc(cXyz* vertices);
};
WWHD_SIZE(CpuVertices, 8);
WWHD_SIZE(dBgWDeform, 0xC4);
WWHD_OFFSET(dBgWDeform, __vtbl, 4);
WWHD_OFFSET(dBgWDeform, mpVtx, 0x90);
} // namespace Sk2
struct sk2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ be<u8> m2B6;
    /* 0x3D3 */ u8 _3D3[3];
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<f32> m2C0;
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3E8 */ be<s16> m2CC;
    /* 0x3EA */ csXyz m2CE[4];
    /* 0x402 */ u8 _402[2];
    /* 0x404 */ cXyz m2E8[4];
    /* 0x434 */ u8 m318[4];
    /* 0x438 */ gptr<Sk2::dBgWDeform> mpBgW; /* dBgWDeform */
    /* 0x43C */ Sk2::CpuVertices mCvtx;      /* HD */
    /* 0x444 */ dBgS_AcchCir mAcchCir;
    /* 0x484 */ dBgS_ObjAcch mAcch; /* dBgS_ObjAcch */
};
WWHD_OFFSET(sk2_class, mpMorf, 0x3E4);
WWHD_OFFSET(sk2_class, m2CE, 0x3EA);
WWHD_OFFSET(sk2_class, m2E8, 0x404);
WWHD_OFFSET(sk2_class, mpBgW, 0x438);
WWHD_OFFSET(sk2_class, mAcch, 0x484);
WWHD_SIZE(sk2_class, 0x648);

// Tww names are deliberately retained for the unknown actor fields.

void sk2SetMin(fopAc_ac_c*, f32, f32, f32);
void sk2SetMax(fopAc_ac_c*, f32, f32, f32);
void sk2CopyBackVertices(dBgW*);
void sk2BgW_ct(dBgW*);
BOOL sk2BgWSet(Sk2::dBgWDeform*, cBgD_t*, u32);
BOOL sk2InitVertices(Sk2::CpuVertices*, void*);
void sk2CalcVertices(Sk2::CpuVertices*, J3DModel*);
void* sk2GetNamedResource(dRes_control_c*, SafeString*, SafeString*);

enum {
    dRes_INDEX_SK2_BCK_KSYLF_00_e = 5,
    dRes_INDEX_SK2_BCK_KSYLF_01_e = 6,
    dRes_INDEX_SK2_BDL_KSYLF_00_e = 9,
    dRes_INDEX_SK2_BDL_KSYLF_01_e = 10,
    dRes_INDEX_SK2_DZB_KSYLF_00_e = 13,
    dRes_INDEX_SK2_DZB_KSYLF_01_e = 14,
    KSYLF_00_JNT_B1_e = 3,
    JA_SE_OBJ_SHOKU_LIFT_MOVE = 0x3823,
    TEV_TYPE_BG0_PLIGHT = 0x5B,
};
