/**
 * c_phase.cpp - phase request handler tables.
 * WWHD (cking.rpx), verified against the recompiled original (unit c_phase).
 */
#include "SSystem/SComponent/c_phase.h"

/* 02019F00 */
void cPhs_Reset(request_of_phase_process_class* pPhase) {
    pPhase->id = 0;
}

/* 02019F0C */
void cPhs_Set(request_of_phase_process_class* pPhase, cPhs__Handler* pHandlerTable) {
    pPhase->mpHandlerTable = gabi::ea(pHandlerTable);
    pPhase->id = 0;
}

/* 02019F1C */
void cPhs_UnCompleate(request_of_phase_process_class* pPhase) {
    pPhase->mpHandlerTable = 0;
    cPhs_Reset(pPhase);
}

/* 02019F28 */
cPhs_State cPhs_Compleate(request_of_phase_process_class* pPhase) {
    pPhase->mpHandlerTable = 0;
    return cPhs_COMPLEATE_e;
}

/* 02019F38 */
cPhs_State cPhs_Next(request_of_phase_process_class* pPhase) {
    const cPhs__Handler* handlerTable = gabi::at<cPhs__Handler>(pPhase->mpHandlerTable);
    if (handlerTable) {
        // HD: retain the 32-bit register wrap when advancing the signed index.
        u32 index = (u32)pPhase->id + 1u;
        pPhase->id = (s32)index;
        if (!handlerTable[index]) {
            return cPhs_Compleate(pPhase);
        }
        return cPhs_LOADING_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 02019F74 */
cPhs_State cPhs_Do(request_of_phase_process_class* pPhase, void* pUserData) {
    cPhs__Handler* handler = gabi::at<cPhs__Handler>(pPhase->mpHandlerTable);
    if (handler) {
        u32 index = pPhase->id;
        int newStep = handler[index](pUserData);
        switch (newStep) {
        case cPhs_LOADING_e:
            return cPhs_Next(pPhase);
        case cPhs_NEXT_e:
            return cPhs_Next(pPhase) == cPhs_LOADING_e ? cPhs_NEXT_e : cPhs_COMPLEATE_e;
        case cPhs_STOP_e:
            cPhs_UnCompleate(pPhase);
            return cPhs_STOP_e;
        case cPhs_COMPLEATE_e:
            return cPhs_Compleate(pPhase);
        case cPhs_ERROR_e:
            cPhs_UnCompleate(pPhase);
            return cPhs_ERROR_e;
        case cPhs_INIT_e:
        default:
            return newStep;
        }
    }
    return cPhs_Compleate(pPhase);
}

/* 0201A0AC */
cPhs_State cPhs_Handler(request_of_phase_process_class* pPhase, cPhs__Handler* pHandlerTable, void* pUserData) {
    pPhase->mpHandlerTable = gabi::ea(pHandlerTable);
    return cPhs_Do(pPhase, pUserData);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0201A0B8 */
static void __sinit_c_phase_cpp() {
    sinit_header_statics_rodata(0x101FFA50, 0x1018D2D4, 0x101FFA5C, 0x10003650);
}

/* 0201A14C */
// HD: the next three header-only translation units have unknown original names.
static void __sinit_adjacent_header_first_cpp() {
    sinit_header_statics_rodata(0x101FFA6C, 0x1018D2F8, 0x101FFA78, 0x10003660);
}

/* 0201A1E0 */
static void __sinit_adjacent_header_second_cpp() {
    sinit_header_statics_rodata(0x101FFA88, 0x1018D31C, 0x101FFA94, 0x1000367C);
}

/* 0201A274 */
static void __sinit_adjacent_header_third_cpp() {
    sinit_header_statics_rodata(0x101FFAA4, 0x1018D340, 0x101FFAB0, 0x10003694);
}

/* WWHD glue (tools/verify/autoglue.py): no local data bindings
*/
