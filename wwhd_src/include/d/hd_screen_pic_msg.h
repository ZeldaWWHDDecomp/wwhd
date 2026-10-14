/* HD opening unit-local interface. */
#pragma once
#include "d/hd_screen_opening.h"

void openingRegisterStatics(void*);
void openingConstructFirstContext(void*);
void openingConstructSecondContext(void*);
void openingSetupBase(hdOpening_screen_c*, hdUiSafeString*, u32);
hdPartsLayout_c* openingAllocateLayout(u32, void*, u32);
hdPartsLayout_c* openingConstructLayout(hdPartsLayout_c*);
void openingBindAnimation(void*, u32, hdUiSafeString*, hdUiSafeString*);
void openingFinishBase(hdOpening_screen_c*);
void openingUpdateState(void*);
void openingPlaySkipAnimation(void*, u32, u32, f32);
