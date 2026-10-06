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
    WWHD_FUNC(0x0226C434, cPhs_State, i_this);
    return ((daNpc_Kk1_c*)i_this)->_create();
}
VERIFY(0x0226C434, daNpc_Kk1_Create);

/* 0226C4C0 */
static BOOL daNpc_Kk1_Delete(daNpc_Kk1_c* i_this) {
    WWHD_FUNC(0x0226C4C0, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0226C4C0, daNpc_Kk1_Delete);

/* 0226ECA4 */
static BOOL daNpc_Kk1_Execute(daNpc_Kk1_c* i_this) {
    WWHD_FUNC(0x0226ECA4, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0226ECA4, daNpc_Kk1_Execute);

/* 0226EF40 */
static BOOL daNpc_Kk1_Draw(daNpc_Kk1_c* i_this) {
    WWHD_FUNC(0x0226EF40, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0226EF40, daNpc_Kk1_Draw);

/* 0226EF44 */
static BOOL daNpc_Kk1_IsDelete(daNpc_Kk1_c*) {
    WWHD_FUNC(0x0226EF44, BOOL, (daNpc_Kk1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0226EF44, daNpc_Kk1_IsDelete);
