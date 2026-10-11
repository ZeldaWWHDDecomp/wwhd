/**
 * c_API_graphic.cpp
 * System component - graphics callbacks before and after drawing
 *
 * WWHD, written in the style of zeldaret/tww and verified function by function against the
 * recompiled original (tools/verify, unit c_API_graphic).
 */

#include "SSystem/SComponent/c_API_graphic.h"

static cAPIGph__Iface g_cAPI_Interface;

/* 02007E18 */
void cAPIGph_BeforeOfDraw() {
    gabi::call_ptr<void>(g_cAPI_Interface.mpBeforeOfDraw.get());
}

/* 02007E28 */
void cAPIGph_AfterOfDraw() {
    gabi::call_ptr<void>(g_cAPI_Interface.mpAfterOfDraw.get());
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02007E38 */
static void __sinit_c_API_graphic_cpp() {
    sinit_header_statics_rodata(0x101FF398, 0x1018C4D8, 0x101FF3A4, 0x10000B5C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_cAPI_Interface 1018C498
*/
