/* c_phase.h (GameCube phase requests), WWHD layout unchanged: table address then 32-bit index.
 * The shared request layout keeps the handler table as an address for existing raw units;
 * this unit views its entries as callable, four-byte guest function pointers. */
#pragma once
#include "bindings.h"
#include "gc_api/SSystem/c_phase.h"

struct cPhs__Handler {
    /* 0x00 */ be<u32> mFunction;
    explicit operator bool() const { return mFunction != 0; }
    cPhs_State operator()(void* pUserData) const {
        return gabi::call_ptr<cPhs_State>(mFunction, pUserData);
    }
};
WWHD_SIZE(cPhs__Handler, 4);
WWHD_OFFSET(request_of_phase_process_class, mpHandlerTable, 0);
WWHD_OFFSET(request_of_phase_process_class, id, 4);
WWHD_SIZE(request_of_phase_process_class, 8);

void cPhs_Reset(request_of_phase_process_class*);
void cPhs_Set(request_of_phase_process_class*, cPhs__Handler*);
void cPhs_UnCompleate(request_of_phase_process_class*);
cPhs_State cPhs_Compleate(request_of_phase_process_class*);
cPhs_State cPhs_Next(request_of_phase_process_class*);
cPhs_State cPhs_Do(request_of_phase_process_class*, void*);
cPhs_State cPhs_Handler(request_of_phase_process_class*, cPhs__Handler*, void*);
