#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "SSystem/SComponent/c_m3d_g_tri.h"

struct MmrrLightLinChk {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x30 - 0x24];
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(MmrrLightLinChk, 0x6C);
/* cM3dGCpsS {cXyz start, end; f32 radius} */
struct MmrrCapsuleSource {
    cXyz mStart;
    cXyz mEnd;
    be<f32> mRadius;
};
WWHD_SIZE(MmrrCapsuleSource, 0x1C);


namespace daObjMmrr {
struct Eff_c : dPa_followEcallBack {
    void end();
    void remove();
};

struct Act_c : fopAc_ac_c {
    bool create_heap();
    void init_cc();
    void set_cc_rec_pos();
    void set_cc_trans_pos();
    void set_cull();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void init_mtx();
    bool chk_light();
    void eff_start();
    void eff_stop();
    void eff_remove();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpMirrorModel;
    /* 0x3B8 */ gptr<J3DModel> mpBeamModel;
    /* 0x3BC */ mDoExt_btkAnm mMirrorBtkAnm;        /* HD 0x74 (GameCube 0x14) */
    /* 0x430 */ mDoExt_btkAnm mBeamBtkAnm;
    /* 0x4A4 */ dCcD_Tri field_0x2C8[5];
    /* 0xB34 */ dCcD_Stts field_0x958[5];
    /* 0xC60 */ dCcD_Cps field_0xA84;
    /* 0xD98 */ dCcD_Stts field_0xBBC;
    /* 0xDD4 */ be<u8> field_0xBF8;
    /* 0xDD5 */ be<u8> field_0xBF9;
    /* 0xDD6 */ u8 _DD6[2];
    /* 0xDD8 */ be<f32> field_0xBFC;
    /* 0xDDC */ be<f32> field_0xC00;
    /* 0xDE0 */ Eff_c field_0xC04;
};
WWHD_OFFSET(Act_c, mBeamBtkAnm, 0x430);
WWHD_OFFSET(Act_c, field_0xA84, 0xC60);
WWHD_OFFSET(Act_c, field_0xBF8, 0xDD4);
WWHD_OFFSET(Act_c, field_0xC04, 0xDE0);
WWHD_SIZE(Act_c, 0xDF4);

}

struct MmrrTriangleView : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
    cM3dGTri mTri;
};
// HD: the shared capsule header imports a second legacy cylinder declaration.
// This unit-private geometry view preserves its checked 0x20 layout without that import.
struct MmrrCapsuleGeometry {
    cXyz mStart;
    cXyz mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(MmrrCapsuleGeometry, __vtbl, 0x18);
WWHD_SIZE(MmrrCapsuleGeometry, 0x20);
struct MmrrCapsuleView : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
    MmrrCapsuleGeometry mCps;
};
WWHD_OFFSET(MmrrTriangleView, mTri, 0x118);
WWHD_SIZE(MmrrTriangleView, sizeof(dCcD_Tri));
WWHD_OFFSET(MmrrCapsuleView, mCps, 0x118);
WWHD_SIZE(MmrrCapsuleView, sizeof(dCcD_Cps));
static MmrrTriangleView& triangleShape(dCcD_Tri* tri) { return *reinterpret_cast<MmrrTriangleView*>(tri); }
static MmrrCapsuleView& capsuleShape(dCcD_Cps* cps) { return *reinterpret_cast<MmrrCapsuleView*>(cps); }
struct MmrrEmitter { u8 _00[0x247]; be<u8> mGlobalAlpha; };
WWHD_OFFSET(MmrrEmitter, mGlobalAlpha, 0x247);
struct MmrrHitVtable { u8 _00[0x3C]; be<u32> clearTargetHit; };
WWHD_OFFSET(MmrrHitVtable, clearTargetHit, 0x3C);
static void Mmrr_clearTargetHit(dCcD_Tri* tri) {
    auto* table = gabi::at<MmrrHitVtable>(tri->__vtbl_hitinf);
    gabi::call_ptr(table->clearTargetHit, tri);
}
// Pure copies keep signaling-NaN payloads. Call sites retain the original word order.
struct MmrrPositionWords { be<u32> x, y, z; };
WWHD_SIZE(MmrrPositionWords, sizeof(cXyz));
static MmrrPositionWords& positionWords(cXyz& pos) { return *reinterpret_cast<MmrrPositionWords*>(&pos); }

struct MmrrAttributes {
    be<f32> field_0x00[5][3][3];
    be<f32> field_0xB4;
    be<f32> field_0xB8;
    be<s16> field_0xBC;
    be<u16> mAlignment; // HD: explicit two-byte padding, zero in the checked record.
    be<f32> field_0xC0[3];
    be<f32> radius;
    be<f32> field_0xD0;
    cXyz& vertex(int i, int j) { return *reinterpret_cast<cXyz*>(field_0x00[i][j]); }
};
WWHD_OFFSET(MmrrAttributes, field_0xB4, 0xB4);
WWHD_OFFSET(MmrrAttributes, field_0xBC, 0xBC);
WWHD_OFFSET(MmrrAttributes, field_0xC0, 0xC0);
WWHD_SIZE(MmrrAttributes, 0xD4);
struct MmrrTriangleSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u8> mAtp;
    be<u8> mAtpPadding[3]; // HD: checked zero padding of cCcD_SrcObjAt.
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtFlags;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgFlags;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mVertices[9];
};
WWHD_SIZE(MmrrTriangleSource, 0x54);
struct MmrrCapsuleCollisionSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u8> mAtp;
    be<u8> mAtpPadding[3]; // HD: checked zero padding of cCcD_SrcObjAt.
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtFlags;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgFlags;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mStartEndRadius[7];
};
WWHD_SIZE(MmrrCapsuleCollisionSource, 0x4C);

void Mmrr_constructArray(void*, s32, u32, u32);
void Mmrr_destroyArray(void*, s32, u32, u32, s32, s32);
BOOL Mmrr_detectLight(void*, cXyz*);
void Mmrr_setTri(cM3dGTri*, cXyz*, cXyz*, cXyz*);
void Mmrr_setCapsule(MmrrCapsuleGeometry*, MmrrCapsuleSource*);
void Mmrr_subtract(const cXyz*, const cXyz*, cXyz*);
f32 Mmrr_squareDistance(const cXyz*, const cXyz*);
void Mmrr_normalizeRS(cXyz*);
void Mmrr_setTriSource(dCcD_Tri*, const MmrrTriangleSource*);
void Mmrr_setCpsSource(dCcD_Cps*, const MmrrCapsuleCollisionSource*);
void Mmrr_constructGObj(dCcD_GObjInf*);
void Mmrr_constructCps(MmrrCapsuleGeometry*);
void Mmrr_constructTri(cM3dGTri*);
void Mmrr_destroyCps(dCcD_Cps*, s32);
void Mmrr_destroyActor(fopAc_ac_c*, s32);
u8 Mmrr_createHeapResult(daObjMmrr::Act_c*);
// HD: array helper callbacks outside this unit retain their guest addresses.
enum : u32 {
    Mmrr_triangleConstructor = 0x02375B20,
    Mmrr_statusConstructor = 0x02375AB8,
    Mmrr_triangleDestructor = 0x025159F8,
    Mmrr_statusDestructor = 0x02515860,
    Mmrr_solidHeapCallback = 0x02374D28,
};

// HD: the shared d_com_inf_game API does not expose the detect accessor; this private view names its embedded block.
struct MmrrDetectPlayView {
    u8 _00[PLAY_DETECT];
    be<u8> mDetect;
};
WWHD_OFFSET(MmrrDetectPlayView, mDetect, PLAY_DETECT);
static void* Mmrr_getDetect() {
    return &reinterpret_cast<MmrrDetectPlayView*>(dComIfGp_get())->mDetect;
}
