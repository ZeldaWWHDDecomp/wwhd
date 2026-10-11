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
    /* 0x3AC */ be<u8> m290;            /* bool: a cut-off part */
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<u32> m294;           /* parts mask */
    /* 0x3B4 */ be<u8> m298;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<u8> m29A;
    /* 0x3B7 */ be<u8> m29B;
    /* 0x3B8 */ be<u8> m29C;
    /* 0x3B9 */ u8 _3B9[3];
    /* 0x3BC */ be<f32> m2A0;
    /* 0x3C0 */ be<f32> m2A4;
    /* 0x3C4 */ be<f32> m2A8;
    /* 0x3C8 */ csXyz m2AC;
    /* 0x3CE */ be<s16> m2B2[5];
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ be<s16> m2C2;
    /* 0x3E0 */ be<s16> m2C4;
    /* 0x3E2 */ be<s16> m2C6;
    /* 0x3E4 */ be<u32> m2C8;           /* fpc_ProcID of the mother sign */
    /* 0x3E8 */ cXyz m2CC;
    /* 0x3F4 */ cXyz m2D8;
    /* 0x400 */ be<f32> m2E4;
    /* 0x404 */ be<f32> m2E8;
    /* 0x408 */ be<f32> m2EC;
    /* 0x40C */ csXyz m2F0;
    /* 0x412 */ u8 _412[2];
    /* 0x414 */ cXyz m2F8;
    /* 0x420 */ cXyz m304;
    /* 0x42C */ dBgS_AcchCir m310;
    /* 0x46C */ dBgS_ObjAcch m350;
    /* 0x630 */ dPa_rippleEcallBack_l m514;
    /* 0x644 */ cXyz m528;
    /* 0x650 */ u8 m534[0x658 - 0x650];
    /* 0x658 */ request_of_phase_process_class mPhase;
    /* 0x660 */ gptr<J3DModel> m544[11];
    /* 0x68C */ dCcD_Stts m574;         /* HD: m570 (shadow id) removed */
    /* 0x6C8 */ dCcD_Cyl m5B0;
};
WWHD_OFFSET(kanban_class, m2B2, 0x3CE);
WWHD_OFFSET(kanban_class, m2C8, 0x3E4);
WWHD_OFFSET(kanban_class, m2F8, 0x414);
WWHD_OFFSET(kanban_class, m350, 0x46C);
WWHD_OFFSET(kanban_class, m514, 0x630);
WWHD_OFFSET(kanban_class, mPhase, 0x658);
WWHD_OFFSET(kanban_class, m574, 0x68C);
WWHD_OFFSET(kanban_class, m5B0, 0x6C8);
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

// Main actor modes (the sub-actions remain in m2C0, as in the GameCube source).
enum KanbanMode {
    KANBAN_MOTHER = 0, KANBAN_MOTHER_RETURN = 1, KANBAN_MOTHER_SWIM = 2,
    KANBAN_PARTS = 100, KANBAN_PARTS_RETURN = 101, KANBAN_PARTS_SWIM = 102,
};
