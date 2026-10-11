/* bmdhand_class (Kalle Demos ceiling tentacle), WWHD layout. 
 *
 * GameCube members +0x11C up to mLineMat; mDoExt_3DlineMat1_c is 0x188 in HD (GameCube 0x3C),
 * so the colliders and m824 move by a further +0x14C. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

struct hand_s {
    /* 0x00 */ cXyz m00;
    /* 0x0C */ cXyz m0C;
    /* 0x18 */ be<f32> m18;
};
WWHD_SIZE(hand_s, 0x1C);

struct bmdhand_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<s16> m2B8;
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0[4];
    /* 0x3E4 */ be<s16> m2C8;
    /* 0x3E6 */ be<s16> m2CA;
    /* 0x3E8 */ cXyz m2CC;
    /* 0x3F4 */ cXyz m2D8;
    /* 0x400 */ cXyz m2E4;
    /* 0x40C */ cXyz m2F0;
    /* 0x418 */ be<f32> m2FC;
    /* 0x41C */ be<s16> m300;
    /* 0x41E */ be<s16> m302;
    /* 0x420 */ be<s8> m304;
    /* 0x421 */ u8 _421[3];
    /* 0x424 */ be<f32> m308;
    /* 0x428 */ be<f32> m30C;
    /* 0x42C */ be<f32> m310;
    /* 0x430 */ be<f32> m314;
    /* 0x434 */ be<f32> m318;
    /* 0x438 */ be<f32> m31C;
    /* 0x43C */ be<f32> m320;
    /* 0x440 */ hand_s m324[20];
    /* 0x670 */ mDoExt_3DlineMat1_c mLineMat;
    /* 0x7F8 */ dCcD_Stts mStts;
    /* 0x834 */ dCcD_Sph m5CC;
    /* 0x960 */ dCcD_Sph m6F8;
    /* 0xA8C */ be<u8> m824;
    /* 0xA8D */ u8 _A8D[3];
};
WWHD_OFFSET(bmdhand_class, m324, 0x440);
WWHD_OFFSET(bmdhand_class, mLineMat, 0x670);
WWHD_OFFSET(bmdhand_class, mStts, 0x7F8);
WWHD_OFFSET(bmdhand_class, m6F8, 0x960);
WWHD_OFFSET(bmdhand_class, m824, 0xA8C);

/* bmd_class (Kalle Demos), the fields the tentacles use (HD offsets) */
struct BmdhandBossView {
    /* 0x000 */ u8 _000[0x328];
    /* 0x328 */ csXyz shape_angle; /* fopAc_ac_c::shape_angle */
    /* 0x32E */ u8 _32E[0x3D0 - 0x32E];
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ u8 _3D4[0x424 - 0x3D4];
    /* 0x424 */ be<s16> m302; /* GameCube 0x302 */
    /* 0x426 */ u8 _426[4];
    /* 0x42A */ be<s16> m308_0; /* GameCube m308[0] */
    /* 0x42C */ u8 _42C[0x434 - 0x42C];
    /* 0x434 */ be<s16> m312;
    /* 0x436 */ u8 _436[0x448 - 0x436];
    /* 0x448 */ be<f32> m328;
    /* 0x44C */ u8 _44C[0x451 - 0x44C];
    /* 0x451 */ be<s8> m331;
    /* 0x452 */ be<s8> m332;
};
WWHD_OFFSET(BmdhandBossView, m332, 0x452);

struct daBmdhand_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daBmdhand_HIO_c, 0x10);

// HD: GHS emits these vtable copies for this translation unit.
enum BmdhandVtable : u32 {
    BmdhandVtable_Actor = 0x10009F5C,
    BmdhandVtable_Hio = 0x10009F4C,
    BmdhandVtable_Line = 0x10009F0C,
    BmdhandVtable_Group = 0x10009F2C,
    BmdhandVtable_Poly = 0x10009F3C,
    BmdhandVtable_Check = 0x10009F1C,
    BmdhandVtable_GroupBase = 0x10009EFC,
    BmdhandVtable_CheckBase = 0x10009EEC,
    BmdhandVtable_Status = 0x1004AE88,
    BmdhandVtable_GlobalStatus = 0x1004AEC0,
};
struct BmdhandArchiveName {
    char name[8];
};
struct BmdhandJointIndices {
    be<s32> values[20];
};
struct BmdhandJointOffsets {
    be<f32> values[4];
};
struct BmdhandLineData {
    gptr<cXyz> positions;
    be<u32> sizes;
};
WWHD_OFFSET(BmdhandLineData, sizes, 4);
WWHD_SIZE(BmdhandLineData, 8);

void Bmdhand_updateLine(mDoExt_3DlineMat1_c*, s32, const GXColor*, dKy_tevstr_c*);
void Bmdhand_monsSeStart(u32, cXyz*, u32, u32, s32);
void Bmdhand_sphereCtor(dCcD_Sph*);
void Bmdhand_lineCtor(mDoExt_3DlineMat1_c*);
void Bmdhand_statusCtor(dCcD_Stts*);
void Bmdhand_globalStatusCtor(void*);
void Bmdhand_sphereDtor(dCcD_Sph*, s32);
void Bmdhand_statusDtor(dCcD_Stts*, s32);
void Bmdhand_lineDtor(mDoExt_3DlineMat1_c*, s32);
void Bmdhand_actorDtor(fopAc_ac_c*, s32);

// HD: read the matrix pointer before changing its dirty flag.
inline Mtx34* Bmdhand_getAnmMtx(J3DModel* model, s32 joint) {
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    u32 matrices = block->mpAnmMtx;
    block->mFlags |= 0x10;
    return gabi::at<Mtx34>(matrices + (u32)joint * sizeof(Mtx34));
}
// HD: this inline tests the guest eye-position address, including wrapped actor pointers.
inline void fopAcM_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param) {
    if (gabi::ea(&actor->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(actor);
        u32 processId = fopAcM_GetID(actor);
        s32 reverb = dComIfGp_getReverb(room);
        Bmdhand_monsSeStart(id, &actor->eyePos, processId, param, reverb);
    }
}
/* dBgS_LinChk (stack, 0x6C), per-TU vtables */
struct BmdhandLineCheck {
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
WWHD_SIZE(BmdhandLineCheck, 0x6C);
static inline void Bmdhand_constructLineCheck(BmdhandLineCheck* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = BmdhandVtable_Line;
    c->__vtbl_64 = BmdhandVtable_Group;
    c->__vtbl_58 = BmdhandVtable_Poly;
    c->__vtbl_20 = BmdhandVtable_Check;
}
static inline void Bmdhand_destroyLineCheck(BmdhandLineCheck* c) {
    c->__vtbl_58 = BmdhandVtable_Poly;
    c->__vtbl_64 = BmdhandVtable_GroupBase;
    c->__vtbl_20 = BmdhandVtable_CheckBase;
    cBgS_LinChk_dt(c, 0);
}

// HD: preserve guest address arithmetic for the line size buffer, including wrapping.
inline BmdhandLineData* Bmdhand_getLineData(mDoExt_3DlineMat1_c& material) {
    return gabi::at<BmdhandLineData>(material.mpLines);
}
inline void Bmdhand_setLineSize(u32 address, u8 size) { *gabi::at<be<u8>>(address) = size; }
WWHD_SIZE(BmdhandArchiveName, 8);
WWHD_SIZE(BmdhandJointIndices, 80);
WWHD_SIZE(BmdhandJointOffsets, 16);

inline BOOL fopAcM_IsActor(void* actor) { return fopAc_IsActor(actor); }
