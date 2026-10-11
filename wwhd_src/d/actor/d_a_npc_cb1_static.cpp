/**
 * d_a_npc_cb1_static.cpp (WWHD)
 * NPC - Makar (Korok cellist): statics kept outside the actor's REL (d_a_npc_cb1_static)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_a_npc_cb1_static.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * The four statics (m_flyingTimer 0x101BDC28, m_status 0x101BDC2A, m_playerRoom 0x101BDC2C,
 * m_flying 0x101BDC2D) are .data/.bss of this TU and have no code. The HD build also gives the TU
 * the usual per-TU header statics (__sinit below).
 */
#include "d/actor/d_a_npc_cb1.h"

/* 02227124 */
// HD: this translation unit initializes only the shared header statics.
static void __sinit_d_a_npc_cb1_static_cpp() {
    sinit_header_statics(0x10466C88, 0x101BDC04);
}

/* 022271B8 */
// HD: free wrapper preserves the static accessor ABI until autoglue supports static methods.
static s16 daNpc_Cb1_c_getMaxFlyingTimer() {
    return 15 * 30;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit has no bound data.
*/
