/* kanban_class (cuttable sign), WWHD layout. 
 *
 * GameCube -> WWHD: fopAc_ac_c +0x11C, and every member +0x11C up to the eleven models; the
 * shadow id m570 is gone (HD shadows: daKanban_Draw sets none), so the dCcD_Stts and dCcD_Cyl
 * are +0x118. Size 0x7F8 (GameCube 0x6E0); offsets from daKanban_Create (inline constructors)
 * and the deleting destructor 0218CF60. */
#pragma once
#include "bindings.h"

/* dPa_rippleEcallBack (0x14): the emitter rate at +0x10 (setRate) */
struct dPa_rippleEcallBack_l {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ be<f32> mRate;
};
WWHD_SIZE(dPa_rippleEcallBack_l, 0x14);

struct kanban_class {
    /* 0x000 */ fopAc_ac_c actor;
    /* 0x3AC */ be<u8> mIsPart;  /* was m290: daKanban_Create 0218CA20: 1 for a cut-off piece (param parts mask not 0, linked to its mother sign by parentActorID), 0 for the whole sign; useHeapInit 0218C874 then creates only the masked models; bool: a cut-off part */
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<u32> mPartsMask;  /* was m294: daKanban_Create 0218CA20: param >> 20 (0 -> 1, the whole sign); daKanban_Draw 0218A548 / useHeapInit 0218C874 draw / create model i for bit i; cut_point_check 0218AD44 removes the cut parts (0x7FE all pieces); parts mask */
    /* 0x3B4 */ be<u8> mWobbleStep;  /* was m298: mother_move / parts_move (in daKanban_Execute 0218B234): (it + 1) & 3 per frame of the landing wobble, the wobble ends at 0; cut_point_check 0218AD44 sets 0 or 1 */
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<u8> mHammerHit;  /* was m29A: mother_move (in daKanban_Execute 0218B234): 2 on a skull hammer hit, 1 when the sign is hammered into the ground (not CUT_TYPE_HAMMER_SIDESWING); parts_move flies farther when its mother's is 2 */
    /* 0x3B7 */ be<u8> mRestored;  /* was m29B: mother_return_move (in daKanban_Execute 0218B234): 1 when the sign is whole again after the Song of Passing return; chield_parts_move then deletes the pieces; 0 when cut (cut_point_check 0218AD44) or hammered */
    /* 0x3B8 */ be<u8> mWaterEntered;  /* was m29C: sea_water_check 0218A6F8: 1 after the first entry into water (ripple and splash only once) */
    /* 0x3B9 */ u8 _3B9[3];
    /* 0x3BC */ be<f32> mWaterSinkY;  /* was m2A0: sea_water_check 0218A6F8: 7 in water for cut type 1, else 0; ret_keisan_move 0218B00C -> 0; daKanban_Execute 0218B234 lowers the position by it around fopAcM_posMove */
    /* 0x3C0 */ be<f32> mCylHeight;  /* was m2A4: daKanban_Create 0218CA20 105 (whole sign); cut_point_check 0218AD44 0 / 25; daKanban_Execute 0218B234 mCyl.SetH(it), and no cc move while it is 0 */
    /* 0x3C4 */ be<f32> mCylRadius;  /* was m2A8: daKanban_Create 0218CA20 50 (whole sign); cut_point_check 0218AD44 0 / 10; daKanban_Execute 0218B234 mCyl.SetR(it) */
    /* 0x3C8 */ csXyz mGroundAngle;  /* was m2AC: mother_move (in daKanban_Execute 0218B234): fopAcM_getGroundAngle on landing; daKanban_Draw 0218A548 tilts the models by x/z */
    /* 0x3CE */ be<s16> mTimers[5];  /* was m2B2: daKanban_Execute 0218B234: 5 timers counted down every frame ([0] the return delay, rnd 30, return_move; [1] keeps the pivot-y shift off while hammered in; [4] shock_damage_check 0218AB94 cool-down 100) */
    /* 0x3D8 */ be<s16> mBobPhase;  /* was m2BC: sea_water_check 0218A6F8: + REG + 1000 per frame in water, sin(it) bobs the height; 0 on water entry */
    /* 0x3DA */ be<s16> mMode;  /* was m2BE: daKanban_Execute 0218B234: switch KANBAN_MOTHER / _RETURN / _SWIM, KANBAN_PARTS / _RETURN / _SWIM; set by daKanban_Create 0218CA20 and tact_return */
    /* 0x3DC */ be<s16> mAction;  /* was m2C0: the step inside mMode: mother_move 10-14 / 0x14-0x15, mother_return_move 30-33, parts_move 0x6E-0x78, chield_parts_move 0x82-0x85, swim 0x28 / 0x8C (in daKanban_Execute 0218B234); cut_point_check 0218AD44 sets 0xB */
    /* 0x3DE */ be<s16> mCutFlags;  /* was m2C2: cut_point_check 0218AD44: bit 1 << mCutDir for each cut direction already done (a direction cuts once); mother_move (in daKanban_Execute 0218B234) reads 8 / 0x10 for the hammer depth; 0 when restored */
    /* 0x3E0 */ be<s16> mCutDir;  /* was m2C4: mother_move (in daKanban_Execute 0218B234): the cut direction from the player's cut type (pl_cut_no_dt), 4 for a non-sword hit; cut_point_check 0218AD44 picks the cut parts by it (cut_parts_arg_data[it * 8]); daKanban_Create 0218CA20: a piece's current.angle.z */
    /* 0x3E2 */ be<s16> mWobbleAngle;  /* was m2C6: mother_move / parts_move (in daKanban_Execute 0218B234): 0x800 on landing, shape_angle.x -> it + 0x4000 and it ^ 0xFF00 per frame (the wobble), 0x400 / 0 as mWobbleStep advances */
    /* 0x3E4 */ be<u32> mMotherId;  /* was m2C8: daKanban_Create 0218CA20: the piece's parentActorID (fpc_ProcID of the mother sign); parts_move / chield_parts_move (in daKanban_Execute 0218B234) look the mother up by it; fpc_ProcID of the mother sign */
    /* 0x3E8 */ cXyz mModelOffset;  /* was m2CC: daKanban_Draw 0218A548: translation of the models after the rotation (minus mPivotX/Y/Z); y 70 for a piece (daKanban_Create 0218CA20, cut_point_check 0218AD44); return_move (in daKanban_Execute 0218B234) clears it */
    /* 0x3F4 */ cXyz mCreatePos;  /* was m2D8: daKanban_Create 0218CA20: current.pos at creation; not read */
    /* 0x400 */ be<f32> mPivotX;  /* was m2E4: daKanban_Draw 0218A548: x of the rotation pivot (added before, subtracted after the rotations); return_move (in daKanban_Execute 0218B234) clears it */
    /* 0x404 */ be<f32> mPivotY;  /* was m2E8: daKanban_Draw 0218A548: y of the rotation pivot; 140 for a piece (cut_point_check 0218AD44, daKanban_Create 0218CA20); daKanban_Execute 0218B234 shifts the position by it around fopAcM_posMove */
    /* 0x408 */ be<f32> mPivotZ;  /* was m2EC: daKanban_Draw 0218A548: z of the rotation pivot; return_move (in daKanban_Execute 0218B234) clears it */
    /* 0x40C */ csXyz mHomeAngle;  /* was m2F0: daKanban_Create 0218CA20: current.angle (a piece copies its mother's); return_angle / mother_return_move (in daKanban_Execute 0218B234) turn back to it */
    /* 0x412 */ u8 _412[2];
    /* 0x414 */ cXyz mHomePos;  /* was m2F8: daKanban_Create 0218CA20: current.pos (a piece copies its mother's); ret_keisan_move 0218B00C moves back to it; mother_move (in daKanban_Execute 0218B234) sinks to it.y - 90/65/20 when hammered */
    /* 0x420 */ cXyz mCreateModelOffset;  /* was m304: daKanban_Create 0218CA20: a piece's mModelOffset at creation; not read */
    /* 0x42C */ dBgS_AcchCir mAcchCir;  /* was m310: daKanban_Create 0218CA20: dBgS_AcchCir, SetWall(20, 30), the circle of mAcch; daKanban_Execute 0218B234 SetWall(0, 40) */
    /* 0x46C */ dBgS_ObjAcch mAcch;  /* was m350: daKanban_Create 0218CA20: dBgS_ObjAcch with mAcchCir, water check offset 300; wall/ground/water checks in mother_move/parts_move (in daKanban_Execute 0218B234) and sea_water_check 0218A6F8 */
    /* 0x630 */ dPa_rippleEcallBack_l mRippleCb;  /* was m514: sea_water_check 0218A6F8: the ripple (ID_AK_JN_HAMON00) emitter callback at mRipplePos; daKanban_Delete 0218C824 / the return end it */
    /* 0x644 */ cXyz mRipplePos;  /* was m528: sea_water_check 0218A6F8: current.pos with y = the water surface, where the ripple is set */
    /* 0x650 */ u8 m534[0x658 - 0x650];
    /* 0x658 */ request_of_phase_process_class mPhase;
    /* 0x660 */ gptr<J3DModel> mpModels[11];  /* was m544: useHeapInit 0218C874: the 11 part models (kanban_bdl[i]); daKanban_Draw 0218A548 draws those in mPartsMask */
    /* 0x68C */ dCcD_Stts mStts;  /* was m574: daKanban_Create 0218CA20: dCcD_Stts, Init(0xFF, 0, this), the status of mCyl; daKanban_Execute 0218B234 moves by its m_cc_move; HD: m570 (shadow id) removed */
    /* 0x6C8 */ dCcD_Cyl mCyl;  /* was m5B0: daKanban_Create 0218CA20: dCcD_Cyl from cyl_src; mother_move (in daKanban_Execute 0218B234) cuts on its Tg hits; SetC/SetH(mCylHeight)/SetR(mCylRadius) */
};
WWHD_OFFSET(kanban_class, mTimers, 0x3CE);
WWHD_OFFSET(kanban_class, mMotherId, 0x3E4);
WWHD_OFFSET(kanban_class, mHomePos, 0x414);
WWHD_OFFSET(kanban_class, mAcch, 0x46C);
WWHD_OFFSET(kanban_class, mRippleCb, 0x630);
WWHD_OFFSET(kanban_class, mPhase, 0x658);
WWHD_OFFSET(kanban_class, mStts, 0x68C);
WWHD_OFFSET(kanban_class, mCyl, 0x6C8);
WWHD_SIZE(kanban_class, 0x7F8);

#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
// HD: Link fields used by the sign, including the byte-sized cut type.
struct KanbanPlayer : fopAc_ac_c {
    be<u8> mCutType;
    u8 padding3AD[0x3C0 - 0x3AD];
    be<u32> mQuakeFlags;
    u8 padding3C4[0x3E4 - 0x3C4];
    cXyz mSwordTopPos;
};
struct KanbanPlayerVtable { u8 padding[0x2C]; be<u32> getTactMusic; };
WWHD_OFFSET(KanbanPlayer, mCutType, 0x3AC);
WWHD_OFFSET(KanbanPlayer, mQuakeFlags, 0x3C0);
WWHD_OFFSET(KanbanPlayer, mSwordTopPos, 0x3E4);
WWHD_OFFSET(KanbanPlayerVtable, getTactMusic, 0x2C);
static inline KanbanPlayer* kanban_player(fopAc_ac_c* player) { return reinterpret_cast<KanbanPlayer*>(player); }
static inline s32 kanban_getTactMusic(fopAc_ac_c* player) {
    KanbanPlayerVtable* table = gabi::at<KanbanPlayerVtable>(player->__vtbl);
    return gabi::call_ptr<s32>(table->getTactMusic, player);
}
static inline void kanban_setPauseTimer(s8 timer) { *gabi::at<be<s8>>(0x101EACB7) = timer; }
static inline u32 kanban_msgManager() { return *gabi::at<be<u32>>(0x101F4B5C); }
#ifndef KANBAN_STORAGE_OWNER
// HD: autoglue exports the owner source's statics as guest references.
extern be<u32>& l_msgId;
extern const be<s16> (&pl_cut_real_no_dt)[32];
extern const be<s16> (&pl_cut_no_dt)[32];
#endif
BOOL kanban_bombState(void*, s32);
BOOL kanban_bombExploding(void*);
fpc_ProcID kanban_createChild(s16, u32, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32);
void kanban_rippleCtor(void*);
void kanban_acchDtor(void*, s32);
void kanban_circleDtor(void*, s32);
void kanban_actorDtor(void*, s32);
BOOL kanban_seaWaterCheck(kanban_class*);
BOOL kanban_shockDamageCheck(kanban_class*);
void kanban_cutPointCheck(kanban_class*);
BOOL kanban_returnPosition(kanban_class*);
u32 kanban_messageSet(u32, u32, cXyz*);
u8 kanban_getMessageStatus(u32);
void kanban_setMessageStatus(u32, u8);
BOOL kanban_rollPlayerCrash(fopAc_ac_c*, f32, u32);

// Main actor modes (the sub-actions remain in mAction, as in the GameCube source).
enum KanbanMode {
    KANBAN_MOTHER = 0, KANBAN_MOTHER_RETURN = 1, KANBAN_MOTHER_SWIM = 2,
    KANBAN_PARTS = 100, KANBAN_PARTS_RETURN = 101, KANBAN_PARTS_SWIM = 102,
};
