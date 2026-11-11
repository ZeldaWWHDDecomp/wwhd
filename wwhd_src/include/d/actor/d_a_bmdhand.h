/* bmdhand_class (Kalle Demos ceiling tentacle), WWHD layout. 
 *
 * GameCube members +0x11C up to mLineMat; mDoExt_3DlineMat1_c is 0x188 in HD (GameCube 0x3C),
 * so the colliders and mHioOwner move by a further +0x14C. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

struct hand_s {
    /* 0x00 */ cXyz mPos;  /* was m00: the tentacle segment's position: control1 / control2 (inlined into daBmdhand_Execute 020B8D9C) and cut_control 020B88C4 chain each segment to the previous one, control2 pulls segment 0x13 to mHandPos, the blood effect is set at it (tww the same) */
    /* 0x0C */ cXyz mSpeed;  /* was m0C: cut_control 020B88C4: first a copy of the old position, then (new - old) * factor, added in the next step's position; the segment's velocity (tww the same) */
    /* 0x18 */ be<f32> mLineSize;  /* was m18: control3 020B882C and cut_control3 020B8C74 set it per segment (10.5, then tapering); hand_move (inlined into daBmdhand_Execute 020B8D9C) passes it to Bmdhand_setLineSize, the tentacle line's width (tww the same) */
};
WWHD_SIZE(hand_s, 0x1C);

struct bmdhand_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<s16> mFrameCounter;  /* was m2B8: daBmdhand_Execute 020B8D9C adds 1 per frame (daBmdhand_Create 020BAD4C starts it random); the sway phase of control1 / cut_control 020B88C4; hand_calc checks the ceiling every 16 frames; hand_move picks the mChainSph segment by it */
    /* 0x3D6 */ be<s16> mAction;  /* was m2BA: hand_move (in daBmdhand_Execute 020B8D9C) switch on the Bmdhand_* actions (Attached, Cut, BossDown, Intro, Disappear, RegrowWait); daBmdhand_Create 020BAD4C sets Intro */
    /* 0x3D8 */ be<s16> mHandStep;  /* was m2BC: hand_calc (in daBmdhand_Execute 020B8D9C) switch: 0 reach the ceiling, 1 hold, 2 move the hand to a new place; hand_move lets the tentacle be cut only above 0 */
    /* 0x3DA */ be<s16> mIntroStep;  /* was m2BE: hand_move (in daBmdhand_Execute 020B8D9C), Intro action: 0 wait for mVineCommand 5, 1 random delay, 2 grow to the ceiling (adds to mVineCount), 3 grown */
    /* 0x3DC */ be<s16> mTimers[4];  /* was m2C0: daBmdhand_Execute 020B8D9C counts all 4 down; hand_calc / hand_move load [0] (hand moves, regrow delay l_HIO.mRegrowTime) and [1] (boss-down delay) and wait on them */
    /* 0x3E4 */ be<s16> mCcOffTimer;  /* was m2C8: hand_move (in daBmdhand_Execute 020B8D9C) sets 20 / 10 / 70 and moves both spheres out of reach (no lock-on) while it runs; daBmdhand_Execute counts it down */
    /* 0x3E6 */ be<s16> mBloodTimer;  /* was m2CA: hand_move (in daBmdhand_Execute 020B8D9C) sets 2 when the tentacle is cut; cut_control 020B88C4 emits ID_AK_SN_O_BKMTENTACLEBLOOD00 at the cut end while it runs; daBmdhand_Execute counts it down */
    /* 0x3E8 */ cXyz mCeilingPos;  /* was m2CC: hand_ceiling_check (in daBmdhand_Execute 020B8D9C): the ceiling hit of a line 2500 up from mHandPos, + l_HIO.mCeilingOffsetY; hand_calc holds the hand at its height; daBmdhand_Execute opens / closes the hand model when it crosses it */
    /* 0x3F4 */ cXyz mHandPos;  /* was m2D8: hand_calc (in daBmdhand_Execute 020B8D9C) moves it toward mHandTargetPos; control2 pulls the last chain point to it; the vine-set sound plays there; daBmdhand_Delete 020BABAC deletes its sound object */
    /* 0x400 */ cXyz mHandTargetPos;  /* was m2E4: hand_calc / start_hand_calc (in daBmdhand_Execute 020B8D9C) set it 250 / 350 in front of the root at the ceiling height and move mHandPos toward it */
    /* 0x40C */ cXyz mHandModelPos;  /* was m2F0: control_tail (in daBmdhand_Execute 020B8D9C) copies the last chain point; hand_mtx_set 020B8724 places the hand model there; cut_control3 020B8C74 eases it to the ceiling */
    /* 0x418 */ be<f32> mHandModelPosYOld;  /* was m2FC: daBmdhand_Execute 020B8D9C keeps mHandModelPos.y of the previous frame to see it cross the ceiling height */
    /* 0x41C */ be<s16> mHandAngleX;  /* was m300: control_tail (in daBmdhand_Execute 020B8D9C) sets it from the last chain segment; hand_mtx_set 020B8724 turns the hand model about x by it */
    /* 0x41E */ be<s16> mHandAngleY;  /* was m302: control_tail (in daBmdhand_Execute 020B8D9C) sets it from the last chain segment; hand_mtx_set 020B8724 turns the hand model about y by it */
    /* 0x420 */ be<s8> mCloseTimer;  /* was m304: daBmdhand_Execute 020B8D9C sets 3 when the hand rises past the ceiling height, counts it down and closes the hand (FOOK_TOJIRU) at 0 */
    /* 0x421 */ u8 _421[3];
    /* 0x424 */ be<f32> mHandMoveRate;  /* was m308: hand_calc / start_hand_calc (in daBmdhand_Execute 020B8D9C) set 0 and ease it to 1; the step of mHandPos toward mHandTargetPos is 30 / 50 * it */
    /* 0x428 */ be<f32> mAttachRate;  /* was m30C: control2 (in daBmdhand_Execute 020B8D9C) pulls the last chain point to mHandPos at 50 * it and eases it to 1; hand_calc starts holding above 0.9; hand_move sets 0 / 1 / random 0.2 */
    /* 0x42C */ be<f32> mSegmentLength;  /* was m310: control1 / control2 / cut_control 020B88C4 place the chain points it apart; hand_move (in daBmdhand_Execute 020B8D9C) eases it to 30 / 20, 0 when it disappears; daBmdhand_Draw 020B8650 draws only above 0.1 */
    /* 0x430 */ be<f32> mDriftY;  /* was m314: control1 / cut_control 020B88C4 / start_control1 (in daBmdhand_Execute 020B8D9C) add it to every chain segment as y drift and ease it to -10 / -3 / 0; hand_move sets 3 on a cut */
    /* 0x434 */ be<f32> mDriftZ;  /* was m318: control1 / cut_control 020B88C4 / start_control1 (in daBmdhand_Execute 020B8D9C) add it to every chain segment as forward drift and ease it to 50 / 30 / 5; hand_move sets 40 on a cut */
    /* 0x438 */ be<f32> mSwayAmp;  /* was m31C: control1 / cut_control 020B88C4 multiply the chain sway (sin / cos of mFrameCounter) by it; hand_move (in daBmdhand_Execute 020B8D9C) sets it and eases it to 70 / 200 */
    /* 0x43C */ be<f32> mHandScale;  /* was m320: hand_mtx_set 020B8724 scales the hand model by it; daBmdhand_Draw 020B8650 draws the hand only above 0.01; hand_move (in daBmdhand_Execute 020B8D9C) eases it to 1 or 0 */
    /* 0x440 */ hand_s mChain[20];  /* was m324: control1 / control2 / cut_control 020B88C4 (in daBmdhand_Execute 020B8D9C) move the 20 chain points from the root to the hand; hand_move copies them to mLineMat; control3 020B882C sets their widths */
    /* 0x670 */ mDoExt_3DlineMat1_c mLineMat;
    /* 0x7F8 */ dCcD_Stts mStts;
    /* 0x834 */ dCcD_Sph mEyeSph;  /* was m5CC: daBmdhand_Create 020BAD4C sets it up (r 110 / 90); hand_move (in daBmdhand_Execute 020B8D9C) centres it on chain point 6 (= eyePos); a hit cuts the tentacle */
    /* 0x960 */ dCcD_Sph mChainSph;  /* was m6F8: daBmdhand_Create 020BAD4C sets it up; hand_move (in daBmdhand_Execute 020B8D9C) centres it on chain point 3 + 4 * (mFrameCounter & 3); a hit cuts the tentacle */
    /* 0xA8C */ be<u8> mHioOwner;  /* was m824: daBmdhand_Create 020BAD4C sets 1 when it created the l_HIO child; daBmdhand_Delete 020BABAC deletes the HIO child only then (same use as bgn_class mHioOwner) */
    /* 0xA8D */ u8 _A8D[3];
};
WWHD_OFFSET(bmdhand_class, mChain, 0x440);
WWHD_OFFSET(bmdhand_class, mLineMat, 0x670);
WWHD_OFFSET(bmdhand_class, mStts, 0x7F8);
WWHD_OFFSET(bmdhand_class, mChainSph, 0x960);
WWHD_OFFSET(bmdhand_class, mHioOwner, 0xA8C);

/* bmd_class (Kalle Demos), the fields the tentacles use (HD offsets) */
struct BmdhandBossView {
    /* 0x000 */ u8 _000[0x328];
    /* 0x328 */ csXyz shape_angle; /* fopAc_ac_c::shape_angle */
    /* 0x32E */ u8 _32E[0x3D0 - 0x32E];
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ u8 _3D4[0x424 - 0x3D4];
    /* 0x424 */ be<s16> mSubMode; /* GameCube 0x302 */
    /* 0x426 */ u8 _426[4];
    /* 0x42A */ be<s16> mTimers0; /* GameCube mTimers[0] */
    /* 0x42C */ u8 _42C[0x434 - 0x42C];
    /* 0x434 */ be<s16> mVineCutTimer;
    /* 0x436 */ u8 _436[0x448 - 0x436];
    /* 0x448 */ be<f32> mGroundY;
    /* 0x44C */ u8 _44C[0x451 - 0x44C];
    /* 0x451 */ be<s8> mVineCount;
    /* 0x452 */ be<s8> mVineCommand;
};
WWHD_OFFSET(BmdhandBossView, mVineCommand, 0x452);

struct daBmdhand_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mCeilingOffsetY;  /* was m08: hand_ceiling_check (inlined into daBmdhand_Execute 020B8D9C through hand_calc / start_hand_calc): added to the y of the ceiling the upward line check found, giving mCeilingPos; daBmdhand_HIO_c_ct 020BAF50 -20 (tww the same) */
    /* 0x08 */ be<s16> mRegrowTime;  /* was m0C: hand_move (inlined into daBmdhand_Execute 020B8D9C), Bmdhand_Attached: a cut tentacle sets mTimers[0] to it; Bmdhand_Cut shrinks the hand below 100 and re-attaches it at 0; daBmdhand_HIO_c_ct 020BAF50 1000 (tww the same) */
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
