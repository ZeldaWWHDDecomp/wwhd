#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_ID_DK_BCK_FLY1_e = 0x11,
    dRes_ID_DK_BDL_DK_e = 0x12,
    dRes_ID_DK_BDL_DK_KAMEN_e = 0x13,
    dRes_ID_DK_BDL_DK_TAIL_e = 0x14,
};
enum {
    DK_JNT_J_DK_ATAMA1_e = 0x18,
    DK_JNT_J_DK_O_LA2_e = 0x3A,
    DK_JNT_J_DK_O_LB2_e = 0x3C,
    DK_JNT_J_DK_O_RA2_e = 0x3E,
    DK_JNT_J_DK_O_RB2_e = 0x40,
};

struct tail_s {
    /* 0x000 */ gptr<J3DModel> models[9];
    /* 0x024 */ cXyz field_0x024[10];
    /* 0x09C */ csXyz field_0x09C[10];
    /* 0x0D8 */ cXyz field_0x0D8[10];
    /* 0x150 */ cXyz field_0x150[2];
    /* 0x168 */ csXyz field_0x168;
    /* 0x16E */ u8 _16E[2];
    /* 0x170 */ cXyz field_0x170;
};
WWHD_SIZE(tail_s, 0x17C);

struct dk_class : fopAc_ac_c {
    /* 0x3AC */ u8 field_0x290[0x1C];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> field_0x2B4;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> field_0x2B8;
    /* 0x3D8 */ be<s16> field_0x2BC;
    /* 0x3DA */ u8 field_0x2BE[2];
    /* 0x3DC */ be<s16> field_0x2C0[5];
    /* 0x3E6 */ u8 _3E6[2];
    /* 0x3E8 */ tail_s tails[4];
    /* 0x9D8 */ gptr<J3DModel> mpModelKamen;
    /* 0x9DC */ dBgS_AcchCir field_0x8C0;
    /* 0xA1C */ dBgS_ObjAcch field_0x900;
    /* 0xBE0 */ u8 field_0xAC4;
    /* 0xBE1 */ be<s8> field_0xAC5;
    /* 0xBE2 */ u8 _BE2[2];
    /* 0xBE4 */ be<s32> field_0xAC8;
    /* 0xBE8 */ dPa_smokeEcallBack field_0xACC;
    /* 0xC08 */ u8 field_0xAEC[2];
    /* 0xC0A */ be<u8> field_0xAEE;
    /* 0xC0B */ u8 _C0B;
};
WWHD_OFFSET(dk_class, tails, 0x3E8);
WWHD_OFFSET(dk_class, field_0x900, 0xA1C);
WWHD_OFFSET(dk_class, field_0xAEE, 0xC0A);
WWHD_SIZE(dk_class, 0xC0C);

fopAc_ac_c *fopAcM_searchFromName(const char *, u32, u32);
BOOL dDemo_setDemoData(fopAc_ac_c *, u8, mDoExt_McaMorf *, const char *, s32, void *, u32, s8);
BOOL Dk_startCheckOld(void *, const char *);
void dPa_smokeEcallBack_ct(void *, u8, u8, u8, u8);
void mDoExt_modelUpdate(J3DModel *);
void mDoMtx_MtxToRot(Mtx34 *, csXyz *);
u32 dDemo_object_getActor(u32, u8);
void fopAcM_SetMin_l(fopAc_ac_c *, f32, f32, f32);
void fopAcM_SetMax_l(fopAc_ac_c *, f32, f32, f32);
// HD: snapshot the matrix address before marking the buffer dirty.
static inline Mtx34 *Dk_getAnmMtx(J3DModel *model, s32 joint) {
    auto buffer = model->mpMtxBuffer.get();
    u16 flags = buffer->mFlags;
    u32 matrices = buffer->mpAnmMtx;
    buffer->mFlags = flags | 0x10;
    return gabi::at<Mtx34>(matrices + (u32)joint * 0x30);
}
struct DkJointNumberView {
    u8 mPrefix[4];
    be<u16> mJntNo;
};

// HD: the player demo byte and the compact demo actor are not present in the GC DK class.
struct DkPlayerDemoView : fopAc_ac_c {
    u8 mPrefix[0x68DE - sizeof(fopAc_ac_c)];
    be<u8> mDemoMode;
};
WWHD_OFFSET(DkPlayerDemoView, mDemoMode, 0x68DE);
struct DkDemoActorView {
    u8 mPrefix[8];
    cXyz mTranslation;
    u8 mReserved[0xC];
    csXyz mRotation;
};
WWHD_OFFSET(DkDemoActorView, mRotation, 0x20);
struct DkSafeStringVtable {
    u8 mPrefix[0x14];
    be<u32> assureTermination;
};
void Dk_assureString(SafeString *);
void Dk_destroyAcch(dBgS_Acch *, s32);
void Dk_destroyCircle(cM3dGCir_l *, s32);
void Dk_destroyActor(fopAc_ac_c *, s32);
