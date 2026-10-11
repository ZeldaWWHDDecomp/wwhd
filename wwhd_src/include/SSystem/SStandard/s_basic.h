/* System memory fill helpers, WWHD. */
#pragma once
#include "bindings.h"

void sBs_FillArea_s(void* pPtr, u32 pNumBytes, s16 pValue);
void sBs_ClearArea(void* pPtr, u32 pNumBytes);
