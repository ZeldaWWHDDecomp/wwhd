/**
 * d_com_lib_game.cpp - common game library
 * WWHD, written in the style of zeldaret/tww and verified against the original
 * (tools/verify, unit d_com_lib_game).
 */
#include "d/d_com_lib_game.h"

/* 02525FE4 */
cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* i_phaseReq, cPhs__Handler* i_handler, void* i_data) {
    // HD: repeat NEXT phases in a loop rather than recursively invoking this wrapper.
    cPhs_State phase;
    do {
        phase = cPhs_Handler(i_phaseReq, i_handler, i_data);
    } while (phase == cPhs_NEXT_e);
    return phase;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02526044 */
static void __sinit_d_com_lib_game_cpp() {
    // HD: trailing per-TU SDK initializer attributed by adjacency.
    sinit_header_statics_rodata(0x10475408, 0x101D5EC4, 0x10475414, 0x1004BCB8);
}

/* WWHD glue (tools/verify/autoglue.py): existing function from c_phase.cpp
ext 0201A0AC cPhs_State cPhs_Handler(request_of_phase_process_class* pPhase, cPhs__Handler* pHandlerTable, void* pUserData)
*/
