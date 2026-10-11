#pragma once
#include "d/actor/d_a_npc_ji1.h"
#include "d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct Ji1MessageManager;
u32 Ji1_messageSet(Ji1MessageManager*, u32, cXyz*);
s32 Ji1_messageStatus(Ji1MessageManager*);
void Ji1_setMessageStatus(Ji1MessageManager*, u32);
// HD: preserve the complete returned register rather than narrowing to the GC u16.
u32 Ji1_nextMessageStatus(daNpc_Ji1_c*, be<u32>*);
void mDoAud_bgmStart(u32);
void mDoAud_bgmStop(u32);
