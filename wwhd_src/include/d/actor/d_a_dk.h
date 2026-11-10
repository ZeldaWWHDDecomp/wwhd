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
    /* 0x024 */ cXyz mJointPos[10];  /* was field_0x024: tail_control (in daDk_Execute 02124F00): [0] the tail root (mRootPos[1]), [1..9] each piece trailing the previous one; tail_draw (in daDk_Draw 02124C44) places the 9 tail models there */
    /* 0x09C */ csXyz mJointAngle[10];  /* was field_0x09C: tail_control (in daDk_Execute 02124F00): the atan2 angles from piece i - 1 to piece i; tail_draw (in daDk_Draw 02124C44) rotates the tail models by them */
    /* 0x0D8 */ cXyz mJointMoveVec[10];  /* was field_0x0D8: tail_control (in daDk_Execute 02124F00): last frame's piece position, then (new - old) * 0.77; added back into the next frame's trail direction (the tail's swing) */
    /* 0x150 */ cXyz mRootPos[2];  /* was field_0x150: nodeCallBack 02124B6C: [1] the tail joint's position, [0] the point 10 behind it; tail_control (in daDk_Execute 02124F00) aims the tail from [0] to [1] and starts it at [1] */
    /* 0x168 */ csXyz mRootAngle;  /* was field_0x168: tail_control (in daDk_Execute 02124F00): the atan2 angles from mRootPos[0] to mRootPos[1] */
    /* 0x16E */ u8 _16E[2];
    /* 0x170 */ cXyz mRootDir;  /* was field_0x170: tail_control (in daDk_Execute 02124F00): (0, 0, 30) rotated by mRootAngle; added (fading per piece) to every piece's trail direction */
};
WWHD_SIZE(tail_s, 0x17C);

struct dk_class : fopAc_ac_c {
    /* 0x3AC */ u8 field_0x290[0x1C];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> mType;  /* was field_0x2B4: daDk_Create 02125AA0: parameter byte 0; 1 = the Forsaken Fortress flight (event checks, not created once event bits 0x0310 and 0x0001 are set); below 2 the actor and its tails use ground collision (mAcch), from 2 not */
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;  /* was field_0x2B8: useHeapInit 02125858 creates the body model (McaMorf, joint callbacks); daDk_Execute 02124F00 plays / calcs it and hands it to dDemo_setDemoData; daDk_Draw 02124C44 draws it */
    /* 0x3D8 */ be<s16> mFrameCounter;  /* was field_0x2BC: daDk_Execute 02124F00 adds 1 per frame (not read in HD or zeldaret/tww) */
    /* 0x3DA */ u8 field_0x2BE[2];
    /* 0x3DC */ be<s16> mTimers[5];  /* was field_0x2C0: daDk_Execute 02124F00 counts all 5 down each frame (never loaded in HD or zeldaret/tww) */
    /* 0x3E6 */ u8 _3E6[2];
    /* 0x3E8 */ tail_s tails[4];
    /* 0x9D8 */ gptr<J3DModel> mpModelKamen;
    /* 0x9DC */ dBgS_AcchCir mAcchCir;  /* was field_0x8C0: daDk_Create 02125AA0 constructs it, SetWall(0, 300), and passes it to mAcch.Set */
    /* 0xA1C */ dBgS_ObjAcch mAcch;  /* was field_0x900: daDk_Create 02125AA0: Set with mAcchCir when mType is below 2; daDk_Execute 02124F00 CrrPos, tail_control (in daDk_Execute 02124F00) keeps the tails above its ground height */
    /* 0xBE0 */ u8 field_0xAC4;
    /* 0xBE1 */ be<s8> mEventState;  /* was field_0xAC5: daDk_Create 02125AA0 sets 0 (mType 1) or 10; daDk_Execute 02124F00: 0 waits for event bit 0x0310, 2 for the "zelda_fly" event, 3 for event bit 0x0001 and then deletes the actor (-1); daDk_Draw 02124C44 draws only above 2 */
    /* 0xBE2 */ u8 _BE2[2];
    /* 0xBE4 */ be<s32> field_0xAC8;
    /* 0xBE8 */ dPa_smokeEcallBack mSmokeCb;  /* was field_0xACC: daDk_Create 02125AA0 constructs it (dPa_smokeEcallBack(1, 1, 0, 0)); daDk_Delete 02125808 removes it */
    /* 0xC08 */ u8 field_0xAEC[2];
    /* 0xC0A */ be<u8> mNotInDemo;  /* was field_0xAEE: daDk_Execute 02124F00: 0 while dDemo_setDemoData drives the model, 1 otherwise (then it plays the morf itself); daDk_Draw 02124C44 draws nothing while it is 1 */
    /* 0xC0B */ u8 _C0B;
};
WWHD_OFFSET(dk_class, tails, 0x3E8);
WWHD_OFFSET(dk_class, mAcch, 0xA1C);
WWHD_OFFSET(dk_class, mNotInDemo, 0xC0A);
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
