/**
 * c_counter.cpp
 * System component - frame counters
 *
 * WWHD, written in the style of zeldaret/tww and verified function by function against the
 * recompiled original (tools/verify, unit c_counter).
 */

#include "SSystem/SComponent/c_counter.h"

static counter_class g_Counter;

/* 0200E6EC */
void cCt_Counter(int resetCounter1) {
    // HD: counter 0 is read before counter 1 is changed; arithmetic wraps as a PPC word.
    u32 counter0 = g_Counter.mCounter0;
    if (resetCounter1 == 1) {
        g_Counter.mCounter1 = 0;
    } else {
        g_Counter.mCounter1 = (u32)g_Counter.mCounter1 + 1u;
    }

    g_Counter.mCounter0 = counter0 + 1u;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0200E728 */
static void __sinit_c_counter_cpp() {
    sinit_header_statics_rodata(0x101FF53C, 0x1018C6D8, 0x101FF548, 0x10001E3C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_Counter 101FF558
*/
