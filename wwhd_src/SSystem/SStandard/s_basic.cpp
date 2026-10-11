/**
 * s_basic.cpp
 * System standard - memory fill and clear
 *
 * WWHD, written in the style of zeldaret/tww and verified function by function against the
 * recompiled original (tools/verify, unit s_basic).
 */

#include "SSystem/SStandard/s_basic.h"

/* 0201B664 */
void sBs_FillArea_s(void* pPtr, u32 pNumBytes, s16 pValue) {
    be<s16>* castPtr = (be<s16>*)pPtr;
    // HD: keep the unsigned word loop count for sizes above the signed range.
    for (u32 i = 0; i < pNumBytes / 2; i++) {
        *castPtr = pValue;
        castPtr++;
    }
}

/* 0201B680 */
void sBs_ClearArea(void* pPtr, u32 pNumBytes) {
    sBs_FillArea_s(pPtr, pNumBytes, 0);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0201B688 */
static void __sinit_s_basic_cpp() {
    sinit_header_statics_rodata(0x101FFC08, 0x1018D484, 0x101FFC14, 0x10003758);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
