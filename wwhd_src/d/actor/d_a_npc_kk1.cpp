/**
 * d_a_npc_kk1.cpp (WWHD)
 * NPC - Mila (poor, Windfall)
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 * Part files: d_a_npc_kk1_*.cpp; d_a_npc_kk1_pending.cpp holds weak guest-call stubs.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_kk1.h"

/* 0226C434 */
static cPhs_State daNpc_Kk1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Kk1_c*)i_this)->_create();
}

/* 0226C4C0 */
static BOOL daNpc_Kk1_Delete(daNpc_Kk1_c* i_this) {
    return i_this->_delete();
}

/* 0226ECA4 */
static BOOL daNpc_Kk1_Execute(daNpc_Kk1_c* i_this) {
    return i_this->_execute();
}

/* 0226EF40 */
static BOOL daNpc_Kk1_Draw(daNpc_Kk1_c* i_this) {
    return i_this->_draw();
}

/* 0226EF44 */
static BOOL daNpc_Kk1_IsDelete(daNpc_Kk1_c*) {
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): method implementations are supplied by the part files.
*/
